#ifndef SRC_NODE_LTTNG_PROVIDER_H_
#define SRC_NODE_LTTNG_PROVIDER_H_

#define TRACEPOINT_CREATE_PROBES
#define TRACEPOINT_DEFINE
#include "node_lttng_tp.h"

namespace node {

void NODE_HTTP_SERVER_REQUEST(node_lttng_http_server_request_t* req,
                              node_lttng_connection_t* conn,
                              const char *remote, int port,
                              const char *method, const char *url,
                              int fd) {
  tracepoint(node, http_server_request, req->url, req->method, \
             req->forwarded_for);
}

void NODE_HTTP_SERVER_RESPONSE(node_lttng_connection_t* conn,
                              const char *remote, int port, int fd) {
  tracepoint(node, http_server_response, port, conn->remote, fd);
}

void NODE_HTTP_CLIENT_REQUEST(node_lttng_http_client_request_t* req,
                              node_lttng_connection_t* conn,
                              const char *remote, int port,
                              const char *method, const char *url,
                              int fd) {
  tracepoint(node, http_client_request, req->url, req->method);
}

void NODE_HTTP_CLIENT_RESPONSE(node_lttng_connection_t* conn,
                               const char *remote, int port, int fd) {
  tracepoint(node, http_client_response, port, conn->remote, fd);
}

void NODE_NET_SERVER_CONNECTION(node_lttng_connection_t* conn,
                                const char *remote, int port, int fd) {
  tracepoint(node, net_server_connection, conn->remote, port, fd, \
             conn->buffered);
}

void NODE_NET_STREAM_END(node_lttng_connection_t* conn,
                        const char *remote, int port, int fd) {
  tracepoint(node, net_stream_end, conn->remote, port, fd);
}

void NODE_GC_START(v8::GCType type,
                   v8::GCCallbackFlags flags,
                   v8::Isolate* isolate) {
  const char* typeStr = "";
  const char* flagsStr = "";
  if (type == v8::GCType::kGCTypeScavenge) {
    typeStr = "kGCTypeScavenge";
  } else if (type == v8::GCType::kGCTypeMarkSweepCompact) {
    typeStr = "kGCTypeMarkSweepCompact";
  } else if (type == v8::GCType::kGCTypeAll) {
    typeStr = "kGCTypeAll";
  }
  if (flags == v8::GCCallbackFlags::kNoGCCallbackFlags) {
    flagsStr = "kNoGCCallbackFlags";
  } else if (flags == v8::GCCallbackFlags::kGCCallbackFlagCompacted) {
    flagsStr = "kGCCallbackFlagCompacted";
  }

  tracepoint(node, gc_start, typeStr, flagsStr);
}


void NODE_GC_DONE(v8::GCType type,
                  v8::GCCallbackFlags flags,
                  v8::Isolate* isolate) {
  const char* typeStr = "";
  const char* flagsStr = "";
  if (type == v8::GCType::kGCTypeScavenge) {
    typeStr = "kGCTypeScavenge";
  } else if (type == v8::GCType::kGCTypeMarkSweepCompact) {
    typeStr = "kGCTypeMarkSweepCompact";
  } else if (type == v8::GCType::kGCTypeAll) {
    typeStr = "kGCTypeAll";
  }
  if (flags == v8::GCCallbackFlags::kNoGCCallbackFlags) {
    flagsStr = "kNoGCCallbackFlags";
  } else if (flags == v8::GCCallbackFlags::kGCCallbackFlagCompacted) {
    flagsStr = "kGCCallbackFlagCompacted";
  }

  tracepoint(node, gc_done, typeStr, flagsStr);
}

bool NODE_HTTP_SERVER_REQUEST_ENABLED() { return true; }
bool NODE_HTTP_SERVER_RESPONSE_ENABLED() { return true; }
bool NODE_HTTP_CLIENT_REQUEST_ENABLED() { return true; }
bool NODE_HTTP_CLIENT_RESPONSE_ENABLED() { return true; }
bool NODE_NET_SERVER_CONNECTION_ENABLED() { return true; }
bool NODE_NET_STREAM_END_ENABLED() { return true; }

}  // namespace node

#endif  // SRC_NODE_LTTNG_PROVIDER_H_
