#include "node.h"
#include "node_watchdog.h"
#include "v8.h"
#include "env.h"
#include "env-inl.h"
#include "gin/public/isolate_holder.h"

namespace node {
namespace util {

using v8::Array;
using v8::Context;
using v8::FunctionCallbackInfo;
using v8::Local;
using v8::Object;
using v8::Private;
#if !(V8_MAJOR_VERSION == 4 && V8_MINOR_VERSION == 8)
using v8::Proxy;
#endif
using v8::String;
using v8::Value;


#define VALUE_METHOD_MAP(V)                                                   \
  V(isArrayBuffer, IsArrayBuffer)                                             \
  V(isDataView, IsDataView)                                                   \
  V(isDate, IsDate)                                                           \
  V(isMap, IsMap)                                                             \
  V(isMapIterator, IsMapIterator)                                             \
  V(isPromise, IsPromise)                                                     \
  V(isRegExp, IsRegExp)                                                       \
  V(isSet, IsSet)                                                             \
  V(isSetIterator, IsSetIterator)                                             \
  V(isSharedArrayBuffer, IsSharedArrayBuffer)                                 \
  V(isTypedArray, IsTypedArray)


#define V(_, ucname) \
  static void ucname(const FunctionCallbackInfo<Value>& args) {               \
    CHECK_EQ(1, args.Length());                                               \
    args.GetReturnValue().Set(args[0]->ucname());                             \
  }

  VALUE_METHOD_MAP(V)
#undef V

static void GetProxyDetails(const FunctionCallbackInfo<Value>& args) {
  // Return undefined if it's not a proxy.
#if !(V8_MAJOR_VERSION == 4 && V8_MINOR_VERSION == 8)
  if (!args[0]->IsProxy())
    return;

  Local<Proxy> proxy = args[0].As<Proxy>();

  Local<Array> ret = Array::New(args.GetIsolate(), 2);
  ret->Set(0, proxy->GetTarget());
  ret->Set(1, proxy->GetHandler());

  args.GetReturnValue().Set(ret);
#endif
}

static void GetHiddenValue(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  if (!args[0]->IsObject())
    return env->ThrowTypeError("obj must be an object");

  if (!args[1]->IsString())
    return env->ThrowTypeError("name must be a string");

  Local<Object> obj = args[0].As<Object>();
  Local<String> name = args[1].As<String>();
  auto private_symbol = Private::ForApi(env->isolate(), name);
  auto maybe_value = obj->GetPrivate(env->context(), private_symbol);

  args.GetReturnValue().Set(maybe_value.ToLocalChecked());
}

static void SetHiddenValue(const FunctionCallbackInfo<Value>& args) {
  Environment* env = Environment::GetCurrent(args);

  if (!args[0]->IsObject())
    return env->ThrowTypeError("obj must be an object");

  if (!args[1]->IsString())
    return env->ThrowTypeError("name must be a string");

  Local<Object> obj = args[0].As<Object>();
  Local<String> name = args[1].As<String>();
  auto private_symbol = Private::ForApi(env->isolate(), name);
  auto maybe_value = obj->SetPrivate(env->context(), private_symbol, args[2]);

  args.GetReturnValue().Set(maybe_value.FromJust());
}


void StartSigintWatchdog(const FunctionCallbackInfo<Value>& args) {
  int ret = SigintWatchdogHelper::GetInstance()->Start();
  if (ret != 0) {
    Environment* env = Environment::GetCurrent(args);
    env->ThrowErrnoException(ret, "StartSigintWatchdog");
  }
}


void StopSigintWatchdog(const FunctionCallbackInfo<Value>& args) {
  bool had_pending_signals = SigintWatchdogHelper::GetInstance()->Stop();
  args.GetReturnValue().Set(had_pending_signals);
}


void WatchdogHasPendingSigint(const FunctionCallbackInfo<Value>& args) {
  bool ret = SigintWatchdogHelper::GetInstance()->HasPendingSignal();
  args.GetReturnValue().Set(ret);
}


void Initialize(Local<Object> target,
                Local<Value> unused,
                Local<Context> context) {
  Environment* env = Environment::GetCurrent(context);

#define V(lcname, ucname) env->SetMethod(target, #lcname, ucname);
  VALUE_METHOD_MAP(V)
#undef V

  env->SetMethod(target, "getHiddenValue", GetHiddenValue);
  env->SetMethod(target, "setHiddenValue", SetHiddenValue);
  env->SetMethod(target, "getProxyDetails", GetProxyDetails);

  env->SetMethod(target, "startSigintWatchdog", StartSigintWatchdog);
  env->SetMethod(target, "stopSigintWatchdog", StopSigintWatchdog);
  env->SetMethod(target, "watchdogHasPendingSigint", WatchdogHasPendingSigint);
}

}  // namespace util

void* Realloc(void* pointer, size_t size) {
    void* newPtr = nullptr;
    if (!pointer) {
        if (size != 0)
            newPtr = gin::IsolateHolder::get_allocator()->Allocate(size);
        return newPtr;
    }

    size_t oldSize = gin::IsolateHolder::GetPointerMemSize(pointer);
    if (size == 0) {
        gin::IsolateHolder::get_allocator()->Free(pointer, gin::IsolateHolder::GetPointerMemSize(pointer));
        return nullptr;
    }
 
    newPtr = gin::IsolateHolder::get_allocator()->Allocate(size);
    size_t copySize = oldSize < size ? oldSize : size;
    memcpy(newPtr, pointer, copySize);
    return newPtr;
}

// As per spec realloc behaves like malloc if passed nullptr.
void* Malloc(size_t size) {
    if (size == 0) size = 1;
    return Realloc(nullptr, size);
}

void* Calloc(size_t n, size_t size) {
    if (n == 0) n = 1;
    if (size == 0) size = 1;
    CHECK_GE(n * size, n);  // Overflow guard.
    return calloc(n, size);
}

}  // namespace node

NODE_MODULE_CONTEXT_AWARE_BUILTIN(util, node::util::Initialize)
