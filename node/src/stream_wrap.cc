#include "stream_wrap.h"
#include "stream_base.h"
#include "stream_base-inl.h"

#include "env-inl.h"
#include "env.h"
#include "handle_wrap.h"
#include "node_buffer.h"
#include "node_counters.h"
#include "pipe_wrap.h"
#include "req-wrap.h"
#include "req-wrap-inl.h"
#include "tcp_wrap.h"
#include "udp_wrap.h"
#include "util.h"
#include "util-inl.h"

#include <stdlib.h>  // abort()
#include <string.h>  // memcpy()
#include <limits.h>  // INT_MAX


namespace node {

using v8::Array;
using v8::Context;
using v8::EscapableHandleScope;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::HandleScope;
using v8::Integer;
using v8::Local;
using v8::Number;
using v8::Object;
using v8::PropertyCallbackInfo;
using v8::String;
using v8::True;
using v8::Undefined;
using v8::Value;


void StreamWrap::Initialize(Local<Object> target,
                            Local<Value> unused,
                            Local<Context> context) {
  Environment* env = Environment::GetCurrent(context);

  Local<FunctionTemplate> sw =
      FunctionTemplate::New(env->isolate(), ShutdownWrap::NewShutdownWrap);
  sw->InstanceTemplate()->SetInternalFieldCount(1);
  sw->SetClassName(FIXED_ONE_BYTE_STRING(env->isolate(), "ShutdownWrap"));
  target->Set(FIXED_ONE_BYTE_STRING(env->isolate(), "ShutdownWrap"),
              sw->GetFunction());

  Local<FunctionTemplate> ww =
      FunctionTemplate::New(env->isolate(), WriteWrap::NewWriteWrap);
  ww->InstanceTemplate()->SetInternalFieldCount(1);
  ww->SetClassName(FIXED_ONE_BYTE_STRING(env->isolate(), "WriteWrap"));
  target->Set(FIXED_ONE_BYTE_STRING(env->isolate(), "WriteWrap"),
              ww->GetFunction());
  env->set_write_wrap_constructor_function(ww->GetFunction());
}


StreamWrap::StreamWrap(Environment* env,
                       Local<Object> object,
                       uv_stream_t* stream,
                       AsyncWrap::ProviderType provider,
                       AsyncWrap* parent)
    : HandleWrap(env,
                 object,
                 reinterpret_cast<uv_handle_t*>(stream),
                 provider,
                 parent),
      StreamBase(env),
      stream_(stream) {
  set_after_write_cb({ OnAfterWriteImpl, this });
  set_alloc_cb({ OnAllocImpl, this });
  set_read_cb({ OnReadImpl, this });
}


void StreamWrap::AddMethods(Environment* env,
                            v8::Local<v8::FunctionTemplate> target,
                            int flags) {
  env->SetProtoMethod(target, "setBlocking", SetBlocking);
  StreamBase::AddMethods<StreamWrap>(env, target, flags);
}


int StreamWrap::GetFD() {
  int fd = -1;
#if !defined(_WIN32)
  if (stream() != nullptr)
    fd = stream()->io_watcher.fd;
#endif
  return fd;
}


bool StreamWrap::IsAlive() {
  return HandleWrap::IsAlive(this);
}


bool StreamWrap::IsClosing() {
  return uv_is_closing(reinterpret_cast<uv_handle_t*>(stream()));
}


void* StreamWrap::Cast() {
  return reinterpret_cast<void*>(this);
}


AsyncWrap* StreamWrap::GetAsyncWrap() {
  return static_cast<AsyncWrap*>(this);
}


bool StreamWrap::IsIPCPipe() {
  return is_named_pipe_ipc();
}


void StreamWrap::UpdateWriteQueueSize() {
  HandleScope scope(env()->isolate());
  Local<Integer> write_queue_size =
      Integer::NewFromUnsigned(env()->isolate(), stream()->write_queue_size);
  object()->Set(env()->write_queue_size_string(), write_queue_size);
}


int StreamWrap::ReadStart() {
  return uv_read_start(stream(), OnAlloc, OnRead);
}


int StreamWrap::ReadStop() {
  return uv_read_stop(stream());
}


void StreamWrap::OnAlloc(uv_handle_t* handle,
                         size_t suggested_size,
                         uv_buf_t* buf) {
  StreamWrap* wrap = static_cast<StreamWrap*>(handle->data);
  HandleScope scope(wrap->env()->isolate());
  Context::Scope context_scope(wrap->env()->context());

  CHECK_EQ(wrap->stream(), reinterpret_cast<uv_stream_t*>(handle));

  return static_cast<StreamBase*>(wrap)->OnAlloc(suggested_size, buf);
}


void StreamWrap::OnAllocImpl(size_t size, uv_buf_t* buf, void* ctx) {
  buf->base = static_cast<char*>(malloc(size));
  buf->len = size;

  if (buf->base == nullptr && size > 0) {
    FatalError(
        "node::StreamWrap::DoAlloc(size_t, uv_buf_t*, void*)",
        "Out Of Memory");
  }
}


template <class WrapType, class UVType>
static Local<Object> AcceptHandle(Environment* env, StreamWrap* parent) {
  EscapableHandleScope scope(env->isolate());
  Local<Object> wrap_obj;
  UVType* handle;

  wrap_obj = WrapType::Instantiate(env, parent);
  if (wrap_obj.IsEmpty())
    return Local<Object>();

  WrapType* wrap = Unwrap<WrapType>(wrap_obj);
  handle = wrap->UVHandle();

  if (uv_accept(parent->stream(), reinterpret_cast<uv_stream_t*>(handle)))
    ABORT();

  return scope.Escape(wrap_obj);
}


void StreamWrap::OnReadImpl(ssize_t nread,
                            const uv_buf_t* buf,
                            uv_handle_type pending,
                            void* ctx) {
  StreamWrap* wrap = static_cast<StreamWrap*>(ctx);
  Environment* env = wrap->env();
  HandleScope handle_scope(env->isolate());
  Context::Scope context_scope(env->context());

  Local<Object> pending_obj;

  if (nread < 0)  {
    if (buf->base != nullptr)
      free(buf->base);
    wrap->EmitData(nread, Local<Object>(), pending_obj);
    return;
  }

  if (nread == 0) {
    if (buf->base != nullptr)
      free(buf->base);
    return;
  }

  char* base = static_cast<char*>(realloc(buf->base, nread));
  CHECK_LE(static_cast<size_t>(nread), buf->len);

  if (pending == UV_TCP) {
    pending_obj = AcceptHandle<TCPWrap, uv_tcp_t>(env, wrap);
  } else if (pending == UV_NAMED_PIPE) {
    pending_obj = AcceptHandle<PipeWrap, uv_pipe_t>(env, wrap);
  } else if (pending == UV_UDP) {
    pending_obj = AcceptHandle<UDPWrap, uv_udp_t>(env, wrap);
  } else {
    CHECK_EQ(pending, UV_UNKNOWN_HANDLE);
  }

  Local<Object> obj = Buffer::New(env, base, nread).ToLocalChecked();
  wrap->EmitData(nread, obj, pending_obj);
}


void StreamWrap::OnReadCommon(uv_stream_t* handle,
                              ssize_t nread,
                              const uv_buf_t* buf,
                              uv_handle_type pending) {
  StreamWrap* wrap = static_cast<StreamWrap*>(handle->data);
  HandleScope scope(wrap->env()->isolate());
  Context::Scope context_scope(wrap->env()->context());

  // We should not be getting this callback if someone as already called
  // uv_close() on the handle.
  CHECK_EQ(wrap->persistent().IsEmpty(), false);

  if (nread > 0) {
    if (wrap->is_tcp()) {
      NODE_COUNT_NET_BYTES_RECV(nread);
    } else if (wrap->is_named_pipe()) {
      NODE_COUNT_PIPE_BYTES_RECV(nread);
    }
  }

  static_cast<StreamBase*>(wrap)->OnRead(nread, buf, pending);
}


void StreamWrap::OnRead(uv_stream_t* handle,
                        ssize_t nread,
                        const uv_buf_t* buf) {
  StreamWrap* wrap = static_cast<StreamWrap*>(handle->data);
  uv_handle_type type = UV_UNKNOWN_HANDLE;

  if (wrap->is_named_pipe_ipc() &&
      uv_pipe_pending_count(reinterpret_cast<uv_pipe_t*>(handle)) > 0) {
    type = uv_pipe_pending_type(reinterpret_cast<uv_pipe_t*>(handle));
  }

  OnReadCommon(handle, nread, buf, type);
}


void StreamWrap::SetBlocking(const FunctionCallbackInfo<Value>& args) {
  StreamWrap* wrap = Unwrap<StreamWrap>(args.Holder());

  CHECK_GT(args.Length(), 0);
  if (!wrap->IsAlive())
    return args.GetReturnValue().Set(UV_EINVAL);

  bool enable = args[0]->IsTrue();
  args.GetReturnValue().Set(uv_stream_set_blocking(wrap->stream(), enable));
}


int StreamWrap::DoShutdown(ShutdownWrap* req_wrap) {
  int err;
  err = uv_shutdown(&req_wrap->req_, stream(), AfterShutdown);
  req_wrap->Dispatched();
  return err;
}


void StreamWrap::AfterShutdown(uv_shutdown_t* req, int status) {
  ShutdownWrap* req_wrap = ContainerOf(&ShutdownWrap::req_, req);
  HandleScope scope(req_wrap->env()->isolate());
  Context::Scope context_scope(req_wrap->env()->context());
  req_wrap->Done(status);
}


// NOTE: Call to this function could change both `buf`'s and `count`'s
// values, shifting their base and decrementing their length. This is
// required in order to skip the data that was successfully written via
// uv_try_write().
int StreamWrap::DoTryWrite(uv_buf_t** bufs, size_t* count) {
  int err;
  size_t written;
  uv_buf_t* vbufs = *bufs;
  size_t vcount = *count;

  err = uv_try_write(stream(), vbufs, vcount);
  if (err == UV_ENOSYS || err == UV_EAGAIN)
    return 0;
  if (err < 0)
    return err;

  // Slice off the buffers: skip all written buffers and slice the one that
  // was partially written.
  written = err;
  for (; vcount > 0; vbufs++, vcount--) {
    // Slice
    if (vbufs[0].len > written) {
      vbufs[0].base += written;
      vbufs[0].len -= written;
      written = 0;
      break;

    // Discard
    } else {
      written -= vbufs[0].len;
    }
  }

  *bufs = vbufs;
  *count = vcount;

  return 0;
}


int StreamWrap::DoWrite(WriteWrap* w,
                        uv_buf_t* bufs,
                        size_t count,
                        uv_stream_t* send_handle) {
  int r;
  if (send_handle == nullptr) {
    r = uv_write(&w->req_, stream(), bufs, count, AfterWrite);
  } else {
    r = uv_write2(&w->req_, stream(), bufs, count, send_handle, AfterWrite);
  }

  if (!r) {
    size_t bytes = 0;
    for (size_t i = 0; i < count; i++)
      bytes += bufs[i].len;
    if (stream()->type == UV_TCP) {
      NODE_COUNT_NET_BYTES_SENT(bytes);
    } else if (stream()->type == UV_NAMED_PIPE) {
      NODE_COUNT_PIPE_BYTES_SENT(bytes);
    }
  }

  w->Dispatched();
  UpdateWriteQueueSize();

  return r;
}


void StreamWrap::AfterWrite(uv_write_t* req, int status) {
  WriteWrap* req_wrap = ContainerOf(&WriteWrap::req_, req);
  HandleScope scope(req_wrap->env()->isolate());
  Context::Scope context_scope(req_wrap->env()->context());
  req_wrap->Done(status);
}


void StreamWrap::OnAfterWriteImpl(WriteWrap* w, void* ctx) {
  StreamWrap* wrap = static_cast<StreamWrap*>(ctx);
  wrap->UpdateWriteQueueSize();
}

}  // namespace node

NODE_MODULE_CONTEXT_AWARE_BUILTIN(stream_wrap, node::StreamWrap::Initialize)
