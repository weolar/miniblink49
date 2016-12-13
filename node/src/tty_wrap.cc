#include "tty_wrap.h"

#include "env.h"
#include "env-inl.h"
#include "handle_wrap.h"
#include "node_buffer.h"
#include "node_wrap.h"
#include "req-wrap.h"
#include "req-wrap-inl.h"
#include "stream_wrap.h"
#include "util.h"
#include "util-inl.h"

namespace node {

using v8::Array;
using v8::Context;
using v8::Function;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::Integer;
using v8::Local;
using v8::Object;
using v8::PropertyAttribute;
using v8::String;
using v8::Value;


void TTYWrap::Initialize(Local<Object> target,
                         Local<Value> unused,
                         Local<Context> context) {
  Environment* env = Environment::GetCurrent(context);

  Local<FunctionTemplate> t = env->NewFunctionTemplate(New);
  t->SetClassName(FIXED_ONE_BYTE_STRING(env->isolate(), "TTY"));
  t->InstanceTemplate()->SetInternalFieldCount(1);

  env->SetProtoMethod(t, "close", HandleWrap::Close);
  env->SetProtoMethod(t, "unref", HandleWrap::Unref);

  StreamWrap::AddMethods(env, t, StreamBase::kFlagNoShutdown);

  env->SetProtoMethod(t, "getWindowSize", TTYWrap::GetWindowSize);
  env->SetProtoMethod(t, "setRawMode", SetRawMode);

  env->SetMethod(target, "isTTY", IsTTY);
  env->SetMethod(target, "guessHandleType", GuessHandleType);

  target->Set(FIXED_ONE_BYTE_STRING(env->isolate(), "TTY"), t->GetFunction());
  env->set_tty_constructor_template(t);
}


uv_tty_t* TTYWrap::UVHandle() {
  return &handle_;
}


void TTYWrap::GuessHandleType(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);
  int fd = args[0]->Int32Value();
  CHECK_GE(fd, 0);

  uv_handle_type t = uv_guess_handle(fd);
  const char* type = nullptr;

  switch (t) {
  case UV_TCP: type = "TCP"; break;
  case UV_TTY: type = "TTY"; break;
  case UV_UDP: type = "UDP"; break;
  case UV_FILE: type = "FILE"; break;
  case UV_NAMED_PIPE: type = "PIPE"; break;
  case UV_UNKNOWN_HANDLE: type = "UNKNOWN"; break;
  default:
    ABORT();
  }

  args.GetReturnValue().Set(OneByteString(env->isolate(), type));
}


void TTYWrap::IsTTY(const FunctionCallbackInfo<Value>& args) {
  int fd = args[0]->Int32Value();
  CHECK_GE(fd, 0);
  bool rc = uv_guess_handle(fd) == UV_TTY;
  args.GetReturnValue().Set(rc);
}


void TTYWrap::GetWindowSize(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  TTYWrap* wrap = Unwrap<TTYWrap>(args.Holder());
  CHECK(args[0]->IsArray());

  int width, height;
  int err = uv_tty_get_winsize(&wrap->handle_, &width, &height);

  if (err == 0) {
    Local<v8::Array> a = args[0].As<Array>();
    a->Set(0, Integer::New(env->isolate(), width));
    a->Set(1, Integer::New(env->isolate(), height));
  }

  args.GetReturnValue().Set(err);
}


void TTYWrap::SetRawMode(const FunctionCallbackInfo<Value>& args) {
  TTYWrap* wrap = Unwrap<TTYWrap>(args.Holder());
  int err = uv_tty_set_mode(&wrap->handle_, args[0]->IsTrue());
  args.GetReturnValue().Set(err);
}


void TTYWrap::New(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  // This constructor should not be exposed to public javascript.
  // Therefore we assert that we are not trying to call this as a
  // normal function.
  CHECK(args.IsConstructCall());

  int fd = args[0]->Int32Value();
  CHECK_GE(fd, 0);

  TTYWrap* wrap = new TTYWrap(env, args.This(), fd, args[1]->IsTrue());
  wrap->UpdateWriteQueueSize();
}


TTYWrap::TTYWrap(Environment* env, Local<Object> object, int fd, bool readable)
    : StreamWrap(env,
                 object,
                 reinterpret_cast<uv_stream_t*>(&handle_),
                 AsyncWrap::PROVIDER_TTYWRAP) {
  uv_tty_init(env->event_loop(), &handle_, fd, readable);
}

}  // namespace node

NODE_MODULE_CONTEXT_AWARE_BUILTIN(tty_wrap, node::TTYWrap::Initialize)
