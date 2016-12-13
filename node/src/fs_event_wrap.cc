#include "async-wrap.h"
#include "async-wrap-inl.h"
#include "env.h"
#include "env-inl.h"
#include "util.h"
#include "util-inl.h"
#include "node.h"
#include "handle_wrap.h"

#include <stdlib.h>

namespace node {

using v8::Context;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::HandleScope;
using v8::Integer;
using v8::Local;
using v8::Object;
using v8::String;
using v8::Value;

class FSEventWrap: public HandleWrap {
 public:
  static void Initialize(Local<Object> target,
                         Local<Value> unused,
                         Local<Context> context);
  static void New(const FunctionCallbackInfo<Value>& args);
  static void Start(const FunctionCallbackInfo<Value>& args);
  static void Close(const FunctionCallbackInfo<Value>& args);

  size_t self_size() const override { return sizeof(*this); }

 private:
  FSEventWrap(Environment* env, Local<Object> object);
  virtual ~FSEventWrap() override;

  static void OnEvent(uv_fs_event_t* handle, const char* filename, int events,
    int status);

  uv_fs_event_t handle_;
  bool initialized_;
};


FSEventWrap::FSEventWrap(Environment* env, Local<Object> object)
    : HandleWrap(env,
                 object,
                 reinterpret_cast<uv_handle_t*>(&handle_),
                 AsyncWrap::PROVIDER_FSEVENTWRAP) {
  initialized_ = false;
}


FSEventWrap::~FSEventWrap() {
  CHECK_EQ(initialized_, false);
}


void FSEventWrap::Initialize(Local<Object> target,
                             Local<Value> unused,
                             Local<Context> context) {
  Environment* env = Environment::GetCurrent(context);

  Local<FunctionTemplate> t = env->NewFunctionTemplate(New);
  t->InstanceTemplate()->SetInternalFieldCount(1);
  t->SetClassName(env->fsevent_string());

  env->SetProtoMethod(t, "start", Start);
  env->SetProtoMethod(t, "close", Close);

  target->Set(env->fsevent_string(), t->GetFunction());
}


void FSEventWrap::New(const FunctionCallbackInfo<Value>& args) {
  CHECK(args.IsConstructCall());
  Environment* env = Environment::GetCurrent(args);
  new FSEventWrap(env, args.This());
}


void FSEventWrap::Start(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  FSEventWrap* wrap = Unwrap<FSEventWrap>(args.Holder());

  if (args.Length() < 1 || !args[0]->IsString()) {
    return env->ThrowTypeError("filename must be a valid string");
  }

  node::Utf8Value path(env->isolate(), args[0]);

  unsigned int flags = 0;
  if (args[2]->IsTrue())
    flags |= UV_FS_EVENT_RECURSIVE;

  int err = uv_fs_event_init(wrap->env()->event_loop(), &wrap->handle_);
  if (err == 0) {
    wrap->initialized_ = true;

    err = uv_fs_event_start(&wrap->handle_, OnEvent, *path, flags);

    if (err == 0) {
      // Check for persistent argument
      if (!args[1]->IsTrue()) {
        uv_unref(reinterpret_cast<uv_handle_t*>(&wrap->handle_));
      }
    } else {
      FSEventWrap::Close(args);
    }
  }

  args.GetReturnValue().Set(err);
}


void FSEventWrap::OnEvent(uv_fs_event_t* handle, const char* filename,
    int events, int status) {
  FSEventWrap* wrap = static_cast<FSEventWrap*>(handle->data);
  Environment* env = wrap->env();

  HandleScope handle_scope(env->isolate());
  Context::Scope context_scope(env->context());

  CHECK_EQ(wrap->persistent().IsEmpty(), false);

  // We're in a bind here. libuv can set both UV_RENAME and UV_CHANGE but
  // the Node API only lets us pass a single event to JS land.
  //
  // The obvious solution is to run the callback twice, once for each event.
  // However, since the second event is not allowed to fire if the handle is
  // closed after the first event, and since there is no good way to detect
  // closed handles, that option is out.
  //
  // For now, ignore the UV_CHANGE event if UV_RENAME is also set. Make the
  // assumption that a rename implicitly means an attribute change. Not too
  // unreasonable, right? Still, we should revisit this before v1.0.
  Local<String> event_string;
  if (status) {
    event_string = String::Empty(env->isolate());
  } else if (events & UV_RENAME) {
    event_string = env->rename_string();
  } else if (events & UV_CHANGE) {
    event_string = env->change_string();
  } else {
    CHECK(0 && "bad fs events flag");
    ABORT();
  }

  Local<Value> argv[] = {
    Integer::New(env->isolate(), status),
    event_string,
    Null(env->isolate())
  };

  if (filename != nullptr) {
    argv[2] = OneByteString(env->isolate(), filename);
  }

  wrap->MakeCallback(env->onchange_string(), ARRAY_SIZE(argv), argv);
}


void FSEventWrap::Close(const FunctionCallbackInfo<Value>& args) {
  FSEventWrap* wrap = Unwrap<FSEventWrap>(args.Holder());

  if (wrap == nullptr || wrap->initialized_ == false)
    return;
  wrap->initialized_ = false;

  HandleWrap::Close(args);
}

}  // namespace node

NODE_MODULE_CONTEXT_AWARE_BUILTIN(fs_event_wrap, node::FSEventWrap::Initialize)
