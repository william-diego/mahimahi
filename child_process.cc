/* -*-mode:c++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

#include <sys/types.h>
#include <sys/wait.h>
#include <cassert>
#include <cstdlib>
#include <sys/syscall.h>
#include <string>

#include "child_process.hh"
#include "exception.hh"
#include "signalfd.hh"
#include "util.hh"

using namespace std;

/* start up a child process running the supplied lambda */
/* the return value of the lambda is the child's exit status */
ChildProcess::ChildProcess( const string & name,
                            function<int()> && child_procedure, const bool new_namespace,
                            const int termination_signal )
    : name_( name ),
      pid_( SystemCall( "clone", syscall( SYS_clone,
                                          SIGCHLD | (new_namespace ? CLONE_NEWNET : 0),
                                          nullptr, nullptr, nullptr, nullptr ) ) ),
      running_( true ),
      terminated_( false ),
      exit_status_(),
      died_on_signal_( false ),
      graceful_termination_signal_( termination_signal ),
      moved_away_( false )
{
    if ( pid_ == 0 ) { /* child */
        try {
            SignalMask( {} ).set_as_mask();
            _exit( child_procedure() );
        } catch ( const Exception & e ) {
            e.perror();
            _exit( EXIT_FAILURE );
        }
    }
}

/* is process in a waitable state? */
bool ChildProcess::waitable( void ) const
{
    assert( !moved_away_ );
    assert( !terminated_ );

    siginfo_t infop;
    zero( infop );
    SystemCall( "waitid", waitid( P_PID, pid_, &infop,
                                  WEXITED | WSTOPPED | WCONTINUED | WNOHANG | WNOWAIT ) );

    if ( infop.si_pid == 0 ) {
        return false;
    } else if ( infop.si_pid == pid_ ) {
        return true;
    } else {
        throw Exception( "waitid", "unexpected value in siginfo_t si_pid field (not 0 or pid)" );
    }
}

/* wait for process to change state */
void ChildProcess::wait( const bool nonblocking )
{
    assert( !moved_away_ );
    assert( !terminated_ );

    siginfo_t infop;
    zero( infop );
    SystemCall( "waitid", waitid( P_PID, pid_, &infop,
                                  WEXITED | WSTOPPED | WCONTINUED | (nonblocking ? WNOHANG : 0) ) );

    if ( nonblocking and (infop.si_pid == 0) ) {
        throw Exception( "nonblocking wait", "process was not waitable" );
    }

    if ( infop.si_pid != pid_ ) {
        throw Exception( "waitid", "unexpected value in siginfo_t si_pid field" );
    }

    if ( infop.si_signo != SIGCHLD ) {
        throw Exception( "waitid", "unexpected value in siginfo_t si_signo field (not SIGCHLD)" );
    }

    /* how did the process change state? */
    switch ( infop.si_code ) {
    case CLD_EXITED:
        terminated_ = true;
        exit_status_ = infop.si_status;
        break;
    case CLD_KILLED:
    case CLD_DUMPED:
        terminated_ = true;
        exit_status_ = infop.si_status;
        died_on_signal_ = true;
        break;
    case CLD_STOPPED:
        running_ = false;
        break;
    case CLD_CONTINUED:
        running_ = true;
        break;
    default:
        throw Exception( "waitid", "unexpected siginfo_t si_code" );
    }
}

/* if child process was suspended, resume it */
void ChildProcess::resume( void )
{
    assert( !moved_away_ );

    if ( !running_ ) {
        signal( SIGCONT );
    }
}

/* send a signal to the child process */
void ChildProcess::signal( const int sig )
{
    assert( !moved_away_ );

    if ( !terminated_ ) {
        SystemCall( "kill", kill( pid_, sig ) );
    }
}

ChildProcess::~ChildProcess()
{
    if ( moved_away_ ) { return; }

    try {
        while ( !terminated_ ) {
            resume();
            signal( graceful_termination_signal_ );
            wait();
        }
    } catch ( const Exception & e ) {
        e.perror();
    }
}

/* move constructor */
ChildProcess::ChildProcess( ChildProcess && other )
    : name_( other.name_ ),
      pid_( other.pid_ ),
      running_( other.running_ ),
      terminated_( other.terminated_ ),
      exit_status_( other.exit_status_ ),
      died_on_signal_( other.died_on_signal_ ),
      graceful_termination_signal_( other.graceful_termination_signal_ ),
      moved_away_( other.moved_away_ )
{
    assert( !other.moved_away_ );

    other.moved_away_ = true;
}

void ChildProcess::throw_exception( void ) const
{
    throw Exception( "`" + name() + "'", "process "
                     + (died_on_signal()
                        ? string("died on signal ")
                        : string("exited with failure status "))
                     + to_string( exit_status() ) );
}
