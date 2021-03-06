AM_CPPFLAGS = -I./protobufs
AM_CXXFLAGS = $(PICKY_CXXFLAGS)
SUBDIRS = protobufs man_pages
common_source = exception.hh ezio.cc ezio.hh                                   \
	file_descriptor.hh netdevice.cc netdevice.hh timestamp.cc timestamp.hh \
	child_process.hh child_process.cc signalfd.hh signalfd.cc              \
	socket.cc socket.hh address.cc address.hh                              \
	system_runner.hh system_runner.cc nat.hh nat.cc                        \
	socket_type.hh util.hh util.cc dns_proxy.hh dns_proxy.cc               \
	interfaces.hh interfaces.cc                                            \
	poller.hh poller.cc bytestream_queue.hh bytestream_queue.cc            \
	event_loop.hh event_loop.cc \
	temp_file.hh temp_file.cc dns_server.hh dns_server.cc \
	socketpair.hh socketpair.cc int32.hh

packet_shell_source = packetshell.cc packetshell.hh

bin_PROGRAMS = delayshell
delayshell_SOURCES = $(common_source) $(packet_shell_source) delayshell.cc \
	delay_queue.hh delay_queue.cc
delayshell_LDADD = -lrt
delayshell_LDFLAGS = -pthread

bin_PROGRAMS += linkshell
linkshell_SOURCES = $(common_source) $(packet_shell_source) linkshell.cc \
	link_queue.hh link_queue.cc
linkshell_LDADD = -lrt
linkshell_LDFLAGS = -pthread

bin_PROGRAMS += recordshell
recordshell_SOURCES = $(common_source) $(http_source) $(http_server_source) $(http_recorder_source) recordshell.cc
recordshell_LDADD = protobufs/libhttprecordprotos.a -lrt $(protobuf_LIBS) $(libssl_LIBS)
recordshell_LDFLAGS = -pthread

http_source = http_request_parser.hh \
	http_request.hh http_request.cc \
	http_response.hh http_response.cc \
	http_header.hh http_header.cc \
	http_response_parser.hh http_response_parser.cc \
	tokenize.hh mime_type.hh mime_type.cc \
	body_parser.hh \
	chunked_parser.hh chunked_parser.cc \
	http_message.hh http_message.cc \
	http_message_sequence.hh \
	backing_store.hh backing_store.cc \
	http_console_store.hh http_console_store.cc \
	http_memory_store.hh http_memory_store.cc

http_recorder_source =	process_recorder.hh process_recorder.cc

http_server_source = http_proxy.hh http_proxy.cc \
	secure_socket.hh secure_socket.cc certificate.hh apache_configuration.hh

bin_PROGRAMS += replayshell
replayshell_SOURCES = $(common_source) $(http_source) replayshell.cc web_server.hh web_server.cc
replayshell_LDADD = protobufs/libhttprecordprotos.a -lrt $(protobuf_LIBS)
replayshell_LDFLAGS = -pthread

bin_PROGRAMS += nph-replayserver.cgi
nph_replayserver_cgi_SOURCES = $(common_source) $(http_source) replayserver.cc
nph_replayserver_cgi_LDADD = protobufs/libhttprecordprotos.a -lrt $(protobuf_LIBS)
nph_replayserver_cgi_LDFLAGS = -pthread

bin_PROGRAMS += remote_proxy
remote_proxy_SOURCES = $(common_source) $(http_server_source) $(http_source) $(http_recorder_source) remote_proxy.cc phantomjs_configuration.hh
remote_proxy_LDADD = protobufs/libhttprecordprotos.a -lrt $(protobuf_LIBS) $(libssl_LIBS)
remote_proxy_LDFLAGS = -pthread

bin_PROGRAMS += remote_proxy_test_client
remote_proxy_test_client_SOURCES = $(common_source) $(http_server_source) $(http_source) remote_proxy_test_client.cc bulk_parser.hh bulk_parser.cc
remote_proxy_test_client_LDADD = protobufs/libhttprecordprotos.a -lrt $(protobuf_LIBS) $(libssl_LIBS)
remote_proxy_test_client_LDFLAGS = -pthread

bin_PROGRAMS += bulkparser
bulkparser_SOURCES = $(common_source) $(http_server_source) $(http_source) bulkparser.cc
bulkparser_LDADD = protobufs/libhttprecordprotos.a -lrt $(protobuf_LIBS) $(libssl_LIBS)
bulkparser_LDFLAGS = -pthread

bin_PROGRAMS += bulkrequestparser
bulkrequestparser_SOURCES = $(common_source) $(http_server_source) $(http_source) bulkrequestparser.cc
bulkrequestparser_LDADD = protobufs/libhttprecordprotos.a -lrt $(protobuf_LIBS) $(libssl_LIBS)
bulkrequestparser_LDFLAGS = -pthread

install-exec-hook:
	chown root $(DESTDIR)$(bindir)/delayshell
	chmod u+s $(DESTDIR)$(bindir)/delayshell
	chown root $(DESTDIR)$(bindir)/linkshell
	chmod u+s $(DESTDIR)$(bindir)/linkshell
	chown root $(DESTDIR)$(bindir)/recordshell
	chmod u+s $(DESTDIR)$(bindir)/recordshell
	chown root $(DESTDIR)$(bindir)/replayshell
	chmod u+s $(DESTDIR)$(bindir)/replayshell
	chown root $(DESTDIR)$(bindir)/remote_proxy
	chmod u+s $(DESTDIR)$(bindir)/remote_proxy
