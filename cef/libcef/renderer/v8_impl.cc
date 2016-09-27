// Copyright (c) 2013 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

// MSVC++ requires this to be set before any other includes to get M_PI.
// Otherwise there will be compile errors in wtf/MathExtras.h.
#define _USE_MATH_DEFINES

#include <map>
#include <string>

#include "base/command_line.h"
#include "base/compiler_specific.h"

#include "config.h"
MSVC_PUSH_WARNING_LEVEL(0);
#include "core/frame/Frame.h"
#include "core/frame/LocalFrame.h"
#include "core/workers/WorkerGlobalScope.h"
#include "bindings/core/v8/ScriptController.h"
#include "bindings/core/v8/V8Binding.h"
MSVC_POP_WARNING();
#undef FROM_HERE
#undef LOG

#include "libcef/renderer/v8_impl.h"

#include "libcef/common/cef_switches.h"
#include "libcef/common/content_client.h"
#include "libcef/common/task_runner_impl.h"
#include "libcef/common/tracker.h"
#include "libcef/renderer/browser_impl.h"
#include "libcef/renderer/thread_util.h"
#include "libcef/renderer/webkit_glue.h"

#include "base/bind.h"
#include "base/lazy_instance.h"
#include "base/strings/string_number_conversions.h"
#include "base/threading/thread_local.h"
#include "third_party/WebKit/public/web/WebKit.h"
#include "third_party/WebKit/public/web/WebFrame.h"
#include "third_party/WebKit/public/web/WebLocalFrame.h"
#include "third_party/WebKit/public/web/WebScriptController.h"
#include "url/gurl.h"

namespace {

static const char kCefTrackObject[] = "Cef::TrackObject";
static const char kCefContextState[] = "Cef::ContextState";

void MessageListenerCallbackImpl(v8::Handle<v8::Message> message,
                                 v8::Handle<v8::Value> data);

// Manages memory and state information associated with a single Isolate.
class CefV8IsolateManager {
 public:
  CefV8IsolateManager()
      : isolate_(v8::Isolate::GetCurrent()),
        task_runner_(CefContentRendererClient::Get()->GetCurrentTaskRunner()),
        context_safety_impl_(IMPL_HASH),
        message_listener_registered_(false),
        worker_id_(0) {
    DCHECK(isolate_);
    DCHECK(task_runner_.get());

    const base::CommandLine* command_line =
        base::CommandLine::ForCurrentProcess();
    if (command_line->HasSwitch(switches::kContextSafetyImplementation)) {
      std::string value = command_line->GetSwitchValueASCII(
          switches::kContextSafetyImplementation);
      int mode;
      if (base::StringToInt(value, &mode)) {
        if (mode < 0)
          context_safety_impl_ = IMPL_DISABLED;
        else if (mode == 1)
          context_safety_impl_ = IMPL_VALUE;
      }
    }
  }
  ~CefV8IsolateManager() {
    DCHECK_EQ(isolate_, v8::Isolate::GetCurrent());
    DCHECK(context_map_.empty());
  }

  scoped_refptr<CefV8ContextState> GetContextState(
      v8::Local<v8::Context> context) {
    DCHECK_EQ(isolate_, v8::Isolate::GetCurrent());
    DCHECK(context.IsEmpty() || isolate_ == context->GetIsolate());

    if (context_safety_impl_ == IMPL_DISABLED)
      return scoped_refptr<CefV8ContextState>();

    if (context.IsEmpty()) {
      if (isolate_->InContext())
        context = isolate_->GetCurrentContext();
      else
        return scoped_refptr<CefV8ContextState>();
    }

    if (context_safety_impl_ == IMPL_HASH) {
      int hash = context->Global()->GetIdentityHash();
      ContextMap::const_iterator it = context_map_.find(hash);
      if (it != context_map_.end())
        return it->second;

      scoped_refptr<CefV8ContextState> state = new CefV8ContextState();
      context_map_.insert(std::make_pair(hash, state));

      return state;
    } else {
      v8::Local<v8::String> key =
          v8::String::NewFromUtf8(isolate_, kCefContextState);

      v8::Local<v8::Object> object = context->Global();
      v8::Local<v8::Value> value = object->GetHiddenValue(key);
      if (!value.IsEmpty()) {
        return static_cast<CefV8ContextState*>(
            v8::External::Cast(*value)->Value());
      }

      scoped_refptr<CefV8ContextState> state = new CefV8ContextState();
      object->SetHiddenValue(key, v8::External::New(isolate_, state.get()));

      // Reference will be released in ReleaseContext.
      state->AddRef();

      return state;
    }
  }

  void ReleaseContext(v8::Local<v8::Context> context) {
    DCHECK_EQ(isolate_, v8::Isolate::GetCurrent());
    DCHECK_EQ(isolate_, context->GetIsolate());

    if (context_safety_impl_ == IMPL_DISABLED)
      return;

    if (context_safety_impl_ == IMPL_HASH) {
      int hash = context->Global()->GetIdentityHash();
      ContextMap::iterator it = context_map_.find(hash);
      if (it != context_map_.end()) {
        it->second->Detach();
        context_map_.erase(it);
      }
    } else {
      v8::Local<v8::String> key =
          v8::String::NewFromUtf8(isolate_, kCefContextState);
      v8::Local<v8::Object> object = context->Global();
      v8::Local<v8::Value> value = object->GetHiddenValue(key);
      if (value.IsEmpty())
        return;

      scoped_refptr<CefV8ContextState> state =
          static_cast<CefV8ContextState*>(v8::External::Cast(*value)->Value());
      state->Detach();
      object->DeleteHiddenValue(key);

      // Match the AddRef in GetContextState.
      state->Release();
    }
  }

  void AddGlobalTrackObject(CefTrackNode* object) {
    DCHECK_EQ(isolate_, v8::Isolate::GetCurrent());
    global_manager_.Add(object);
  }

  void DeleteGlobalTrackObject(CefTrackNode* object) {
    DCHECK_EQ(isolate_, v8::Isolate::GetCurrent());
    global_manager_.Delete(object);
  }

  void SetUncaughtExceptionStackSize(int stack_size) {
    if (stack_size <= 0)
      return;

    if (!message_listener_registered_) {
      v8::V8::AddMessageListener(&MessageListenerCallbackImpl);
      message_listener_registered_ = true;
    }

    v8::V8::SetCaptureStackTraceForUncaughtExceptions(true,
          stack_size, v8::StackTrace::kDetailed);
  }

  void SetWorkerAttributes(int worker_id, const GURL& worker_url) {
    worker_id_ = worker_id;
    worker_url_ = worker_url;
  }

  v8::Isolate* isolate() const { return isolate_; }
  scoped_refptr<base::SequencedTaskRunner> task_runner() const {
    return task_runner_;
  }

  int worker_id() const {
    return worker_id_;
  }

  const GURL& worker_url() const {
    return worker_url_;
  }

 private:
  v8::Isolate* isolate_;
  scoped_refptr<base::SequencedTaskRunner> task_runner_;

  enum ContextSafetyImpl {
    IMPL_DISABLED,
    IMPL_HASH,
    IMPL_VALUE,
  };
  ContextSafetyImpl context_safety_impl_;

  // Used with IMPL_HASH.
  typedef std::map<int, scoped_refptr<CefV8ContextState> > ContextMap;
  ContextMap context_map_;

  // Used for globally tracked objects that are not associated with a particular
  // context.
  CefTrackManager global_manager_;

  // True if the message listener has been registered.
  bool message_listener_registered_;

  // Attributes associated with WebWorker threads.
  int worker_id_;
  GURL worker_url_;
};

// Chromium uses the default Isolate for the main render process thread and a
// new Isolate for each WebWorker thread. Continue this pattern by tracking
// Isolate information on a per-thread basis. This implementation will need to
// be re-worked (perhaps using a map keyed on v8::Isolate::GetCurrent()) if
// in the future Chromium begins using the same Isolate across multiple threads.
class CefV8StateManager {
public:
  CefV8StateManager() {
  }

  void CreateIsolateManager() {
    DCHECK(!current_tls_.Get());
    current_tls_.Set(new CefV8IsolateManager());
  }

  void DestroyIsolateManager() {
    DCHECK(current_tls_.Get());
    delete current_tls_.Get();
    current_tls_.Set(NULL);
  }

  CefV8IsolateManager* GetIsolateManager() {
    CefV8IsolateManager* manager = current_tls_.Get();
    DCHECK(manager);
    return manager;
  }

 private:
  base::ThreadLocalPointer<CefV8IsolateManager> current_tls_;
};

base::LazyInstance<CefV8StateManager> g_v8_state = LAZY_INSTANCE_INITIALIZER;

CefV8IsolateManager* GetIsolateManager() {
  return g_v8_state.Pointer()->GetIsolateManager();
}

class V8TrackObject : public CefTrackNode {
 public:
  explicit V8TrackObject(v8::Isolate* isolate)
      : isolate_(isolate),
        external_memory_(0) {
    DCHECK(isolate_);
    isolate_->AdjustAmountOfExternalAllocatedMemory(
        static_cast<int>(sizeof(V8TrackObject)));
  }
  ~V8TrackObject() {
    isolate_->AdjustAmountOfExternalAllocatedMemory(
        -static_cast<int>(sizeof(V8TrackObject)) - external_memory_);
  }

  inline int GetExternallyAllocatedMemory() {
    return external_memory_;
  }

  int AdjustExternallyAllocatedMemory(int change_in_bytes) {
    int new_value = external_memory_ + change_in_bytes;
    if (new_value < 0) {
      NOTREACHED() << "External memory usage cannot be less than 0 bytes";
      change_in_bytes = -(external_memory_);
      new_value = 0;
    }

    if (change_in_bytes != 0)
      isolate_->AdjustAmountOfExternalAllocatedMemory(change_in_bytes);
    external_memory_ = new_value;

    return new_value;
  }

  inline void SetAccessor(CefRefPtr<CefV8Accessor> accessor) {
    accessor_ = accessor;
  }

  inline CefRefPtr<CefV8Accessor> GetAccessor() {
    return accessor_;
  }

  inline void SetHandler(CefRefPtr<CefV8Handler> handler) {
    handler_ = handler;
  }

  inline CefRefPtr<CefV8Handler> GetHandler() {
    return handler_;
  }

  inline void SetUserData(CefRefPtr<CefBase> user_data) {
    user_data_ = user_data;
  }

  inline CefRefPtr<CefBase> GetUserData() {
    return user_data_;
  }

  // Attach this track object to the specified V8 object.
  void AttachTo(v8::Local<v8::Object> object) {
    object->SetHiddenValue(v8::String::NewFromUtf8(isolate_, kCefTrackObject),
                           v8::External::New(isolate_, this));
  }

  // Retrieve the track object for the specified V8 object.
  static V8TrackObject* Unwrap(v8::Isolate* isolate,
                               v8::Local<v8::Object> object) {
    DCHECK(isolate);
    v8::Local<v8::Value> value =
        object->GetHiddenValue(
            v8::String::NewFromUtf8(isolate, kCefTrackObject));
    if (!value.IsEmpty())
      return static_cast<V8TrackObject*>(v8::External::Cast(*value)->Value());

    return NULL;
  }

 private:
  v8::Isolate* isolate_;
  CefRefPtr<CefV8Accessor> accessor_;
  CefRefPtr<CefV8Handler> handler_;
  CefRefPtr<CefBase> user_data_;
  int external_memory_;
};

class V8TrackString : public CefTrackNode {
 public:
  explicit V8TrackString(const std::string& str) : string_(str) {}
  const char* GetString() { return string_.c_str(); }

 private:
  std::string string_;
};


// Convert a CefString to a V8::String.
v8::Local<v8::String> GetV8String(v8::Isolate* isolate,
                                   const CefString& str) {
#if defined(CEF_STRING_TYPE_UTF16)
  // Already a UTF16 string.
  return v8::String::NewFromTwoByte(
      isolate,
      reinterpret_cast<uint16_t*>(
          const_cast<CefString::char_type*>(str.c_str())),
      v8::String::kNormalString,
      str.length());
#elif defined(CEF_STRING_TYPE_UTF8)
  // Already a UTF8 string.
  return v8::String::NewFromUtf8(isolate,
                                 const_cast<char*>(str.c_str()),
                                 v8::String::kNormalString,
                                 str.length());
#else
  // Convert the string to UTF8.
  std::string tmpStr = str;
  return v8::String::NewFromUtf8(isolate,
                                 tmpStr.c_str(),
                                 v8::String::kNormalString,
                                 tmpStr.length());
#endif
}

#if defined(CEF_STRING_TYPE_UTF16)
void v8impl_string_dtor(char16* str) {
  delete [] str;
}
#elif defined(CEF_STRING_TYPE_UTF8)
void v8impl_string_dtor(char* str) {
  delete [] str;
}
#endif

// Convert a v8::String to CefString.
void GetCefString(v8::Local<v8::String> str, CefString& out) {
  if (str.IsEmpty())
    return;

#if defined(CEF_STRING_TYPE_WIDE)
  // Allocate enough space for a worst-case conversion.
  int len = str->Utf8Length();
  if (len == 0)
    return;
  char* buf = new char[len + 1];
  str->WriteUtf8(buf, len + 1);

  // Perform conversion to the wide type.
  cef_string_t* retws = out.GetWritableStruct();
  cef_string_utf8_to_wide(buf, len, retws);

  delete [] buf;
#else  // !defined(CEF_STRING_TYPE_WIDE)
#if defined(CEF_STRING_TYPE_UTF16)
  int len = str->Length();
  if (len == 0)
    return;
  char16* buf = new char16[len + 1];
  str->Write(reinterpret_cast<uint16_t*>(buf), 0, len + 1);
#else
  // Allocate enough space for a worst-case conversion.
  int len = str->Utf8Length();
  if (len == 0)
    return;
  char* buf = new char[len + 1];
  str->WriteUtf8(buf, len + 1);
#endif

  // Don't perform an extra string copy.
  out.clear();
  cef_string_t* retws = out.GetWritableStruct();
  retws->str = buf;
  retws->length = len;
  retws->dtor = v8impl_string_dtor;
#endif  // !defined(CEF_STRING_TYPE_WIDE)
}

// V8 function callback.
void FunctionCallbackImpl(const v8::FunctionCallbackInfo<v8::Value>& info) {
  v8::Isolate* isolate = info.GetIsolate();

  CefV8Handler* handler =
      static_cast<CefV8Handler*>(v8::External::Cast(*info.Data())->Value());

  CefV8ValueList params;
  for (int i = 0; i < info.Length(); i++)
    params.push_back(new CefV8ValueImpl(isolate, info[i]));

  CefString func_name;
  GetCefString(v8::Handle<v8::String>::Cast(info.Callee()->GetName()),
               func_name);
  CefRefPtr<CefV8Value> object = new CefV8ValueImpl(isolate, info.This());
  CefRefPtr<CefV8Value> retval;
  CefString exception;

  if (handler->Execute(func_name, object, params, retval, exception)) {
    if (!exception.empty()) {
      info.GetReturnValue().Set(
          isolate->ThrowException(
              v8::Exception::Error(GetV8String(isolate, exception))));
      return;
    } else {
      CefV8ValueImpl* rv = static_cast<CefV8ValueImpl*>(retval.get());
      if (rv && rv->IsValid()) {
        info.GetReturnValue().Set(rv->GetV8Value(true));
        return;
      }
    }
  }

  info.GetReturnValue().SetUndefined();
}

// V8 Accessor callbacks
void AccessorGetterCallbackImpl(
    v8::Local<v8::String> property,
    const v8::PropertyCallbackInfo<v8::Value>& info) {
  v8::Isolate* isolate = info.GetIsolate();

  v8::Local<v8::Object> obj = info.This();

  CefRefPtr<CefV8Accessor> accessorPtr;

  V8TrackObject* tracker = V8TrackObject::Unwrap(isolate, obj);
  if (tracker)
    accessorPtr = tracker->GetAccessor();

  if (accessorPtr.get()) {
    CefRefPtr<CefV8Value> retval;
    CefRefPtr<CefV8Value> object = new CefV8ValueImpl(isolate, obj);
    CefString name, exception;
    GetCefString(property, name);
    if (accessorPtr->Get(name, object, retval, exception)) {
      if (!exception.empty()) {
          info.GetReturnValue().Set(
              isolate->ThrowException(
                  v8::Exception::Error(GetV8String(isolate, exception))));
          return;
      } else {
          CefV8ValueImpl* rv = static_cast<CefV8ValueImpl*>(retval.get());
          if (rv && rv->IsValid()) {
            info.GetReturnValue().Set(rv->GetV8Value(true));
            return;
          }
      }
    }
  }

  return info.GetReturnValue().SetUndefined();
}

void AccessorSetterCallbackImpl(
    v8::Local<v8::String> property,
    v8::Local<v8::Value> value,
    const v8::PropertyCallbackInfo<void>& info) {
  v8::Isolate* isolate = info.GetIsolate();

  v8::Local<v8::Object> obj = info.This();

  CefRefPtr<CefV8Accessor> accessorPtr;

  V8TrackObject* tracker = V8TrackObject::Unwrap(isolate, obj);
  if (tracker)
    accessorPtr = tracker->GetAccessor();

  if (accessorPtr.get()) {
    CefRefPtr<CefV8Value> object = new CefV8ValueImpl(isolate, obj);
    CefRefPtr<CefV8Value> cefValue = new CefV8ValueImpl(isolate, value);
    CefString name, exception;
    GetCefString(property, name);
    accessorPtr->Set(name, object, cefValue, exception);
    if (!exception.empty()) {
      isolate->ThrowException(
          v8::Exception::Error(GetV8String(isolate, exception)));
      return;
    }
  }
}

v8::MaybeLocal<v8::Value> CallV8Function(v8::Local<v8::Context> context,
                                         v8::Local<v8::Function> function,
                                         v8::Local<v8::Object> receiver,
                                         int argc,
                                         v8::Local<v8::Value> args[],
                                         v8::Isolate* isolate) {
  v8::MaybeLocal<v8::Value> func_rv;

  // Execute the function call using the ScriptController so that inspector
  // instrumentation works.
  if (CEF_CURRENTLY_ON_RT()) {
    RefPtr<blink::LocalFrame> frame =
        toLocalFrame(blink::toFrameIfNotDetached(context));
    DCHECK(frame);
    if (frame &&
        frame->script().canExecuteScripts(blink::AboutToExecuteScript)) {
      func_rv = frame->script().callFunction(function, receiver, argc, args);
    }
  } else {
    func_rv = blink::ScriptController::callFunction(
        blink::toExecutionContext(context),
        function, receiver, argc, args, isolate);
  }

  return func_rv;
}


// V8 extension registration.

class ExtensionWrapper : public v8::Extension {
 public:
  ExtensionWrapper(const char* extension_name,
                   const char* javascript_code,
                   CefV8Handler* handler)
    : v8::Extension(extension_name, javascript_code), handler_(handler) {
  }

  v8::Handle<v8::FunctionTemplate> GetNativeFunctionTemplate(
    v8::Isolate* isolate,
    v8::Handle<v8::String> name) override {
    if (!handler_)
      return v8::Local<v8::FunctionTemplate>();

    return v8::FunctionTemplate::New(isolate,
                                     FunctionCallbackImpl,
                                     v8::External::New(isolate, handler_));
  }

 private:
  CefV8Handler* handler_;
};

class CefV8ExceptionImpl : public CefV8Exception {
 public:
  explicit CefV8ExceptionImpl(v8::Local<v8::Message> message)
    : line_number_(0),
      start_position_(0),
      end_position_(0),
      start_column_(0),
      end_column_(0) {
    if (message.IsEmpty())
      return;

    GetCefString(message->Get(), message_);
    GetCefString(message->GetSourceLine(), source_line_);

    if (!message->GetScriptResourceName().IsEmpty())
      GetCefString(message->GetScriptResourceName()->ToString(), script_);

    line_number_ = message->GetLineNumber();
    start_position_ = message->GetStartPosition();
    end_position_ = message->GetEndPosition();
    start_column_ = message->GetStartColumn();
    end_column_ = message->GetEndColumn();
  }

  CefString GetMessage() override { return message_; }
  CefString GetSourceLine() override { return source_line_; }
  CefString GetScriptResourceName() override { return script_; }
  int GetLineNumber() override { return line_number_; }
  int GetStartPosition() override { return start_position_; }
  int GetEndPosition() override { return end_position_; }
  int GetStartColumn() override { return start_column_; }
  int GetEndColumn() override { return end_column_; }

 protected:
  CefString message_;
  CefString source_line_;
  CefString script_;
  int line_number_;
  int start_position_;
  int end_position_;
  int start_column_;
  int end_column_;

  IMPLEMENT_REFCOUNTING(CefV8ExceptionImpl);
};

void MessageListenerCallbackImpl(v8::Handle<v8::Message> message,
                                 v8::Handle<v8::Value> data) {
  CefRefPtr<CefApp> application = CefContentClient::Get()->application();
  if (!application.get())
    return;

  CefRefPtr<CefRenderProcessHandler> handler =
      application->GetRenderProcessHandler();
  if (!handler.get())
    return;

  v8::Isolate* isolate = GetIsolateManager()->isolate();
  CefRefPtr<CefV8Context> context = CefV8Context::GetCurrentContext();
  v8::Local<v8::StackTrace> v8Stack = message->GetStackTrace();
  CefRefPtr<CefV8StackTrace> stackTrace =
      new CefV8StackTraceImpl(isolate, v8Stack);

  CefRefPtr<CefV8Exception> exception = new CefV8ExceptionImpl(message);

  if (CEF_CURRENTLY_ON_RT()) {
    handler->OnUncaughtException(context->GetBrowser(), context->GetFrame(),
        context, exception, stackTrace);
  }
}

}  // namespace


// Global functions.

void CefV8IsolateCreated() {
  g_v8_state.Pointer()->CreateIsolateManager();
}

void CefV8IsolateDestroyed() {
  g_v8_state.Pointer()->DestroyIsolateManager();
}

void CefV8ReleaseContext(v8::Local<v8::Context> context) {
  GetIsolateManager()->ReleaseContext(context);
}

void CefV8SetUncaughtExceptionStackSize(int stack_size) {
  GetIsolateManager()->SetUncaughtExceptionStackSize(stack_size);
}

void CefV8SetWorkerAttributes(int worker_id, const GURL& worker_url) {
  GetIsolateManager()->SetWorkerAttributes(worker_id, worker_url);
}

bool CefRegisterExtension(const CefString& extension_name,
                          const CefString& javascript_code,
                          CefRefPtr<CefV8Handler> handler) {
  // Verify that this method was called on the correct thread.
  CEF_REQUIRE_RT_RETURN(false);

  CefV8IsolateManager* isolate_manager = GetIsolateManager();

  V8TrackString* name = new V8TrackString(extension_name);
  isolate_manager->AddGlobalTrackObject(name);
  V8TrackString* code = new V8TrackString(javascript_code);
  isolate_manager->AddGlobalTrackObject(code);

  if (handler.get()) {
    // The reference will be released when the process exits.
    V8TrackObject* object = new V8TrackObject(isolate_manager->isolate());
    object->SetHandler(handler);
    isolate_manager->AddGlobalTrackObject(object);
  }

  ExtensionWrapper* wrapper = new ExtensionWrapper(
      name->GetString(),
      code->GetString(),
      handler.get());

  content::RenderThread::Get()->RegisterExtension(wrapper);
  return true;
}


// Helper macros

#define CEF_V8_HAS_ISOLATE() (!!GetIsolateManager())
#define CEF_V8_REQUIRE_ISOLATE_RETURN(var) \
  if (!CEF_V8_HAS_ISOLATE()) { \
    NOTREACHED() << "V8 isolate is not valid"; \
    return var; \
  }

#define CEF_V8_CURRENTLY_ON_MLT() \
    (!handle_.get() || handle_->BelongsToCurrentThread())
#define CEF_V8_REQUIRE_MLT_RETURN(var) \
  CEF_V8_REQUIRE_ISOLATE_RETURN(var); \
  if (!CEF_V8_CURRENTLY_ON_MLT()) { \
    NOTREACHED() << "called on incorrect thread"; \
    return var; \
  }

#define CEF_V8_HANDLE_IS_VALID() (handle_.get() && handle_->IsValid())
#define CEF_V8_REQUIRE_VALID_HANDLE_RETURN(ret) \
  CEF_V8_REQUIRE_MLT_RETURN(ret); \
  if (!CEF_V8_HANDLE_IS_VALID()) { \
    NOTREACHED() << "V8 handle is not valid"; \
    return ret; \
  }

#define CEF_V8_IS_VALID() \
  (CEF_V8_HAS_ISOLATE() && \
   CEF_V8_CURRENTLY_ON_MLT() && \
   CEF_V8_HANDLE_IS_VALID())

#define CEF_V8_REQUIRE_OBJECT_RETURN(ret) \
  CEF_V8_REQUIRE_VALID_HANDLE_RETURN(ret); \
  if (type_ != TYPE_OBJECT) { \
    NOTREACHED() << "V8 value is not an object"; \
    return ret; \
  }


// CefV8HandleBase

CefV8HandleBase::~CefV8HandleBase() {
  DCHECK(BelongsToCurrentThread());
}

bool CefV8HandleBase::BelongsToCurrentThread() const {
  return task_runner_->RunsTasksOnCurrentThread();
}

CefV8HandleBase::CefV8HandleBase(v8::Isolate* isolate,
                                 v8::Local<v8::Context> context)
    : isolate_(isolate) {
  DCHECK(isolate_);

  CefV8IsolateManager* manager = GetIsolateManager();
  DCHECK(manager);
  DCHECK_EQ(isolate_, manager->isolate());

  task_runner_ = manager->task_runner();
  context_state_ = manager->GetContextState(context);
}


// CefV8Context

// static
CefRefPtr<CefV8Context> CefV8Context::GetCurrentContext() {
  CefRefPtr<CefV8Context> context;
  CEF_V8_REQUIRE_ISOLATE_RETURN(context);
  v8::Isolate* isolate = GetIsolateManager()->isolate();
  if (isolate->InContext()) {
    v8::HandleScope handle_scope(isolate);
    context = new CefV8ContextImpl(isolate, isolate->GetCurrentContext());
  }
  return context;
}

// static
CefRefPtr<CefV8Context> CefV8Context::GetEnteredContext() {
  CefRefPtr<CefV8Context> context;
  CEF_V8_REQUIRE_ISOLATE_RETURN(context);
  v8::Isolate* isolate = GetIsolateManager()->isolate();
  if (isolate->InContext()) {
    v8::HandleScope handle_scope(isolate);
    context = new CefV8ContextImpl(isolate, isolate->GetEnteredContext());
  }
  return context;
}

// static
bool CefV8Context::InContext() {
  CEF_V8_REQUIRE_ISOLATE_RETURN(false);
  v8::Isolate* isolate = GetIsolateManager()->isolate();
  return isolate->InContext();
}


// CefV8ContextImpl

CefV8ContextImpl::CefV8ContextImpl(v8::Isolate* isolate,
                                   v8::Local<v8::Context> context)
  : handle_(new Handle(isolate, context, context))
#ifndef NDEBUG
    , enter_count_(0)
#endif
{  // NOLINT(whitespace/braces)
}

CefV8ContextImpl::~CefV8ContextImpl() {
  DLOG_ASSERT(0 == enter_count_);
}

CefRefPtr<CefTaskRunner> CefV8ContextImpl::GetTaskRunner() {
  return new CefTaskRunnerImpl(handle_->task_runner());
}

bool CefV8ContextImpl::IsValid() {
  return CEF_V8_IS_VALID();
}

CefRefPtr<CefBrowser> CefV8ContextImpl::GetBrowser() {
  CefRefPtr<CefBrowser> browser;
  CEF_V8_REQUIRE_VALID_HANDLE_RETURN(browser);

  // Return NULL for WebWorkers.
  if (!CEF_CURRENTLY_ON_RT())
    return browser;

  blink::WebFrame* webframe = GetWebFrame();
  if (webframe)
    browser = CefBrowserImpl::GetBrowserForMainFrame(webframe->top());

  return browser;
}

CefRefPtr<CefFrame> CefV8ContextImpl::GetFrame() {
  CefRefPtr<CefFrame> frame;
  CEF_V8_REQUIRE_VALID_HANDLE_RETURN(frame);

  // Return NULL for WebWorkers.
  if (!CEF_CURRENTLY_ON_RT())
    return frame;

  blink::WebFrame* webframe = GetWebFrame();
  if (webframe) {
    CefRefPtr<CefBrowserImpl> browser =
        CefBrowserImpl::GetBrowserForMainFrame(webframe->top());
    frame = browser->GetFrame(webkit_glue::GetIdentifier(webframe));
  }

  return frame;
}

CefRefPtr<CefV8Value> CefV8ContextImpl::GetGlobal() {
  CEF_V8_REQUIRE_VALID_HANDLE_RETURN(NULL);

  v8::Isolate* isolate = handle_->isolate();
  v8::HandleScope handle_scope(isolate);
  v8::Local<v8::Context> context = GetV8Context();
  v8::Context::Scope context_scope(context);
  return new CefV8ValueImpl(isolate, context->Global());
}

bool CefV8ContextImpl::Enter() {
  CEF_V8_REQUIRE_VALID_HANDLE_RETURN(false);

  v8::Isolate* isolate = handle_->isolate();
  v8::HandleScope handle_scope(isolate);

  blink::V8PerIsolateData::from(isolate)->incrementRecursionLevel();
  handle_->GetNewV8Handle()->Enter();
#ifndef NDEBUG
  ++enter_count_;
#endif
  return true;
}

bool CefV8ContextImpl::Exit() {
  CEF_V8_REQUIRE_VALID_HANDLE_RETURN(false);

  v8::Isolate* isolate = handle_->isolate();
  v8::HandleScope handle_scope(handle_->isolate());

  DLOG_ASSERT(enter_count_ > 0);
  handle_->GetNewV8Handle()->Exit();
  blink::V8PerIsolateData::from(isolate)->decrementRecursionLevel();
#ifndef NDEBUG
  --enter_count_;
#endif
  return true;
}

bool CefV8ContextImpl::IsSame(CefRefPtr<CefV8Context> that) {
  CEF_V8_REQUIRE_VALID_HANDLE_RETURN(false);

  CefV8ContextImpl* impl = static_cast<CefV8ContextImpl*>(that.get());
  if (!impl || !impl->IsValid())
    return false;

  return (handle_->GetPersistentV8Handle() ==
          impl->handle_->GetPersistentV8Handle());
}

bool CefV8ContextImpl::Eval(const CefString& code,
                            CefRefPtr<CefV8Value>& retval,
                            CefRefPtr<CefV8Exception>& exception) {
  CEF_V8_REQUIRE_VALID_HANDLE_RETURN(false);

  if (code.empty()) {
    NOTREACHED() << "invalid input parameter";
    return false;
  }

  v8::Isolate* isolate = handle_->isolate();
  v8::HandleScope handle_scope(isolate);
  v8::Local<v8::Context> context = GetV8Context();
  v8::Context::Scope context_scope(context);
  v8::Local<v8::Object> obj = context->Global();

  // Retrieve the eval function.
  v8::Local<v8::Value> val = obj->Get(v8::String::NewFromUtf8(isolate, "eval"));
  if (val.IsEmpty() || !val->IsFunction())
    return false;

  v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(val);
  v8::Local<v8::Value> code_val = GetV8String(isolate, code);

  v8::TryCatch try_catch;
  try_catch.SetVerbose(true);

  retval = NULL;
  exception = NULL;

  v8::MaybeLocal<v8::Value> func_rv =
      CallV8Function(context, func, obj, 1, &code_val, handle_->isolate());

  if (try_catch.HasCaught()) {
    exception = new CefV8ExceptionImpl(try_catch.Message());
    return false;
  } else if (!func_rv.IsEmpty()) {
    retval = new CefV8ValueImpl(isolate, func_rv.ToLocalChecked());
  }
  return true;
}

v8::Local<v8::Context> CefV8ContextImpl::GetV8Context() {
  return handle_->GetNewV8Handle();
}

blink::WebFrame* CefV8ContextImpl::GetWebFrame() {
  CEF_REQUIRE_RT();
  v8::HandleScope handle_scope(handle_->isolate());
  v8::Context::Scope context_scope(GetV8Context());
  return blink::WebLocalFrame::frameForCurrentContext();
}


// CefV8ValueImpl::Handle

CefV8ValueImpl::Handle::Handle(v8::Isolate* isolate,
                               v8::Local<v8::Context> context,
                               handleType v,
                               CefTrackNode* tracker)
    : CefV8HandleBase(isolate, context),
      handle_(isolate, v),
      tracker_(tracker),
      should_persist_(false),
      is_set_weak_(false) {
}

CefV8ValueImpl::Handle::~Handle() {
  DCHECK(BelongsToCurrentThread());

  if (tracker_) {
    if (is_set_weak_) {
      if (context_state_.get()) {
        // If the associated context is still valid then delete |tracker_|.
        // Otherwise, |tracker_| will already have been deleted.
        if (context_state_->IsValid())
          context_state_->DeleteTrackObject(tracker_);
      } else {
        GetIsolateManager()->DeleteGlobalTrackObject(tracker_);
      }

      isolate_->AdjustAmountOfExternalAllocatedMemory(
          -static_cast<int>(sizeof(Handle)));
    } else {
      delete tracker_;
    }
  }

  // Always call Reset() on a persistent handle to avoid the
  // CHECK(state() != NEAR_DEATH) in V8's PostGarbageCollectionProcessing.
  handle_.Reset();
}

CefV8ValueImpl::Handle::handleType
CefV8ValueImpl::Handle::GetNewV8Handle(bool should_persist) {
  DCHECK(IsValid());
  if (should_persist && !should_persist_)
    should_persist_ = true;
  return handleType::New(isolate(), handle_);
}

CefV8ValueImpl::Handle::persistentType&
CefV8ValueImpl::Handle::GetPersistentV8Handle() {
  return handle_;
}

void CefV8ValueImpl::Handle::SetWeakIfNecessary() {
  if (!BelongsToCurrentThread()) {
    task_runner()->PostTask(FROM_HERE,
        base::Bind(&CefV8ValueImpl::Handle::SetWeakIfNecessary, this));
    return;
  }

  // Persist the handle (call SetWeak) if:
  // A. The handle has been passed into a V8 function or used as a return value
  //    from a V8 callback, and
  // B. The associated context, if any, is still valid.
  if (should_persist_ && (!context_state_.get() || context_state_->IsValid())) {
    is_set_weak_ = true;

    if (tracker_) {
      if (context_state_.get()) {
        // |tracker_| will be deleted when:
        // A. The associated context is released, or
        // B. Destructor is called for the weak handle.
        DCHECK(context_state_->IsValid());
        context_state_->AddTrackObject(tracker_);
      } else {
        // |tracker_| will be deleted when:
        // A. The process shuts down, or
        // B. Destructor is called for the weak handle.
        GetIsolateManager()->AddGlobalTrackObject(tracker_);
      }
    }

    isolate_->AdjustAmountOfExternalAllocatedMemory(
        static_cast<int>(sizeof(Handle)));

    // The added reference will be released in Destructor.
    AddRef();
    handle_.SetWeak(this, Destructor);
  }
}

// static
void CefV8ValueImpl::Handle::Destructor(
    const v8::WeakCallbackData<v8::Value, Handle>& data) {
  data.GetParameter()->Release();
}


// CefV8Value

// static
CefRefPtr<CefV8Value> CefV8Value::CreateUndefined() {
  CEF_V8_REQUIRE_ISOLATE_RETURN(NULL);
  v8::Isolate* isolate = GetIsolateManager()->isolate();
  CefRefPtr<CefV8ValueImpl> impl = new CefV8ValueImpl(isolate);
  impl->InitUndefined();
  return impl.get();
}

// static
CefRefPtr<CefV8Value> CefV8Value::CreateNull() {
  CEF_V8_REQUIRE_ISOLATE_RETURN(NULL);
  v8::Isolate* isolate = GetIsolateManager()->isolate();
  CefRefPtr<CefV8ValueImpl> impl = new CefV8ValueImpl(isolate);
  impl->InitNull();
  return impl.get();
}

// static
CefRefPtr<CefV8Value> CefV8Value::CreateBool(bool value) {
  CEF_V8_REQUIRE_ISOLATE_RETURN(NULL);
  v8::Isolate* isolate = GetIsolateManager()->isolate();
  CefRefPtr<CefV8ValueImpl> impl = new CefV8ValueImpl(isolate);
  impl->InitBool(value);
  return impl.get();
}

// static
CefRefPtr<CefV8Value> CefV8Value::CreateInt(int32 value) {
  CEF_V8_REQUIRE_ISOLATE_RETURN(NULL);
  v8::Isolate* isolate = GetIsolateManager()->isolate();
  CefRefPtr<CefV8ValueImpl> impl = new CefV8ValueImpl(isolate);
  impl->InitInt(value);
  return impl.get();
}

// static
CefRefPtr<CefV8Value> CefV8Value::CreateUInt(uint32 value) {
  CEF_V8_REQUIRE_ISOLATE_RETURN(NULL);
  v8::Isolate* isolate = GetIsolateManager()->isolate();
  CefRefPtr<CefV8ValueImpl> impl = new CefV8ValueImpl(isolate);
  impl->InitUInt(value);
  return impl.get();
}

// static
CefRefPtr<CefV8Value> CefV8Value::CreateDouble(double value) {
  CEF_V8_REQUIRE_ISOLATE_RETURN(NULL);
  v8::Isolate* isolate = GetIsolateManager()->isolate();
  CefRefPtr<CefV8ValueImpl> impl = new CefV8ValueImpl(isolate);
  impl->InitDouble(value);
  return impl.get();
}

// static
CefRefPtr<CefV8Value> CefV8Value::CreateDate(const CefTime& value) {
  CEF_V8_REQUIRE_ISOLATE_RETURN(NULL);
  v8::Isolate* isolate = GetIsolateManager()->isolate();
  CefRefPtr<CefV8ValueImpl> impl = new CefV8ValueImpl(isolate);
  impl->InitDate(value);
  return impl.get();
}

// static
CefRefPtr<CefV8Value> CefV8Value::CreateString(const CefString& value) {
  CEF_V8_REQUIRE_ISOLATE_RETURN(NULL);
  v8::Isolate* isolate = GetIsolateManager()->isolate();
  CefRefPtr<CefV8ValueImpl> impl = new CefV8ValueImpl(isolate);
  CefString str(value);
  impl->InitString(str);
  return impl.get();
}

// static
CefRefPtr<CefV8Value> CefV8Value::CreateObject(
    CefRefPtr<CefV8Accessor> accessor) {
  CEF_V8_REQUIRE_ISOLATE_RETURN(NULL);

  v8::Isolate* isolate = GetIsolateManager()->isolate();
  v8::HandleScope handle_scope(isolate);

  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  if (context.IsEmpty()) {
    NOTREACHED() << "not currently in a V8 context";
    return NULL;
  }

  // Create the new V8 object.
  v8::Local<v8::Object> obj = v8::Object::New(isolate);

  // Create a tracker object that will cause the user data and/or accessor
  // reference to be released when the V8 object is destroyed.
  V8TrackObject* tracker = new V8TrackObject(isolate);
  tracker->SetAccessor(accessor);

  // Attach the tracker object.
  tracker->AttachTo(obj);

  CefRefPtr<CefV8ValueImpl> impl = new CefV8ValueImpl(isolate);
  impl->InitObject(obj, tracker);
  return impl.get();
}

// static
CefRefPtr<CefV8Value> CefV8Value::CreateArray(int length) {
  CEF_V8_REQUIRE_ISOLATE_RETURN(NULL);

  v8::Isolate* isolate = GetIsolateManager()->isolate();
  v8::HandleScope handle_scope(isolate);

  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  if (context.IsEmpty()) {
    NOTREACHED() << "not currently in a V8 context";
    return NULL;
  }

  // Create a tracker object that will cause the user data reference to be
  // released when the V8 object is destroyed.
  V8TrackObject* tracker = new V8TrackObject(isolate);

  // Create the new V8 array.
  v8::Local<v8::Array> arr = v8::Array::New(isolate, length);

  // Attach the tracker object.
  tracker->AttachTo(arr);

  CefRefPtr<CefV8ValueImpl> impl = new CefV8ValueImpl(isolate);
  impl->InitObject(arr, tracker);
  return impl.get();
}

// static
CefRefPtr<CefV8Value> CefV8Value::CreateFunction(
    const CefString& name,
    CefRefPtr<CefV8Handler> handler) {
  CEF_V8_REQUIRE_ISOLATE_RETURN(NULL);

  if (!handler.get()) {
    NOTREACHED() << "invalid parameter";
    return NULL;
  }

  v8::Isolate* isolate = GetIsolateManager()->isolate();
  v8::HandleScope handle_scope(isolate);

  v8::Local<v8::Context> context = isolate->GetCurrentContext();
  if (context.IsEmpty()) {
    NOTREACHED() << "not currently in a V8 context";
    return NULL;
  }

  // Create a new V8 function template.
  v8::Local<v8::FunctionTemplate> tmpl = v8::FunctionTemplate::New(isolate);

  v8::Local<v8::Value> data = v8::External::New(isolate, handler.get());

  // Set the function handler callback.
  tmpl->SetCallHandler(FunctionCallbackImpl, data);

  // Retrieve the function object and set the name.
  v8::Local<v8::Function> func = tmpl->GetFunction();
  if (func.IsEmpty()) {
    NOTREACHED() << "failed to create V8 function";
    return NULL;
  }

  func->SetName(GetV8String(isolate, name));

  // Create a tracker object that will cause the user data and/or handler
  // reference to be released when the V8 object is destroyed.
  V8TrackObject* tracker = new V8TrackObject(isolate);
  tracker->SetHandler(handler);

  // Attach the tracker object.
  tracker->AttachTo(func);

  // Create the CefV8ValueImpl and provide a tracker object that will cause
  // the handler reference to be released when the V8 object is destroyed.
  CefRefPtr<CefV8ValueImpl> impl = new CefV8ValueImpl(isolate);
  impl->InitObject(func, tracker);
  return impl.get();
}


// CefV8ValueImpl

CefV8ValueImpl::CefV8ValueImpl(v8::Isolate* isolate)
    : isolate_(isolate),
      type_(TYPE_INVALID),
      rethrow_exceptions_(false) {
  DCHECK(isolate_);
}

CefV8ValueImpl::CefV8ValueImpl(v8::Isolate* isolate,
                               v8::Local<v8::Value> value)
    : isolate_(isolate),
      type_(TYPE_INVALID),
      rethrow_exceptions_(false) {
  DCHECK(isolate_);
  InitFromV8Value(value);
}

CefV8ValueImpl::~CefV8ValueImpl() {
  if (type_ == TYPE_STRING)
    cef_string_clear(&string_value_);
  if (handle_.get())
    handle_->SetWeakIfNecessary();
}

void CefV8ValueImpl::InitFromV8Value(v8::Local<v8::Value> value) {
  if (value->IsUndefined()) {
    InitUndefined();
  } else if (value->IsNull()) {
    InitNull();
  } else if (value->IsTrue()) {
    InitBool(true);
  } else if (value->IsFalse()) {
    InitBool(false);
  } else if (value->IsBoolean()) {
    InitBool(value->ToBoolean()->Value());
  } else if (value->IsInt32()) {
    InitInt(value->ToInt32()->Value());
  } else if (value->IsUint32()) {
    InitUInt(value->ToUint32()->Value());
  } else if (value->IsNumber()) {
    InitDouble(value->ToNumber()->Value());
  } else if (value->IsDate()) {
    // Convert from milliseconds to seconds.
    InitDate(CefTime(value->ToNumber()->Value() / 1000));
  } else if (value->IsString()) {
    CefString rv;
    GetCefString(value->ToString(), rv);
    InitString(rv);
  } else if (value->IsObject()) {
    InitObject(value, NULL);
  }
}

void CefV8ValueImpl::InitUndefined() {
  DCHECK_EQ(type_, TYPE_INVALID);
  type_ = TYPE_UNDEFINED;
}

void CefV8ValueImpl::InitNull() {
  DCHECK_EQ(type_, TYPE_INVALID);
  type_ = TYPE_NULL;
}

void CefV8ValueImpl::InitBool(bool value) {
  DCHECK_EQ(type_, TYPE_INVALID);
  type_ = TYPE_BOOL;
  bool_value_ = value;
}

void CefV8ValueImpl::InitInt(int32 value) {
  DCHECK_EQ(type_, TYPE_INVALID);
  type_ = TYPE_INT;
  int_value_ = value;
}

void CefV8ValueImpl::InitUInt(uint32 value) {
  DCHECK_EQ(type_, TYPE_INVALID);
  type_ = TYPE_UINT;
  uint_value_ = value;
}

void CefV8ValueImpl::InitDouble(double value) {
  DCHECK_EQ(type_, TYPE_INVALID);
  type_ = TYPE_DOUBLE;
  double_value_ = value;
}

void CefV8ValueImpl::InitDate(const CefTime& value) {
  DCHECK_EQ(type_, TYPE_INVALID);
  type_ = TYPE_DATE;
  date_value_ = value;
}

void CefV8ValueImpl::InitString(CefString& value) {
  DCHECK_EQ(type_, TYPE_INVALID);
  type_ = TYPE_STRING;
  // Take ownership of the underling string value.
  const cef_string_t* str = value.GetStruct();
  if (str) {
    string_value_ = *str;
    cef_string_t* writable_struct = value.GetWritableStruct();
    writable_struct->str = NULL;
    writable_struct->length = 0;
  } else {
    string_value_.str = NULL;
    string_value_.length = 0;
  }
}

void CefV8ValueImpl::InitObject(v8::Local<v8::Value> value, CefTrackNode* tracker) {
  DCHECK_EQ(type_, TYPE_INVALID);
  type_ = TYPE_OBJECT;
  handle_ = new Handle(isolate_, v8::Local<v8::Context>(), value, tracker);
}

v8::Local<v8::Value> CefV8ValueImpl::GetV8Value(bool should_persist) {
  switch (type_) {
    case TYPE_UNDEFINED:
      return v8::Undefined(isolate_);
    case TYPE_NULL:
      return v8::Null(isolate_);
    case TYPE_BOOL:
      return v8::Boolean::New(isolate_, bool_value_);
    case TYPE_INT:
      return v8::Int32::New(isolate_, int_value_);
    case TYPE_UINT:
      return v8::Uint32::New(isolate_, uint_value_);
    case TYPE_DOUBLE:
      return v8::Number::New(isolate_, double_value_);
    case TYPE_DATE:
      // Convert from seconds to milliseconds.
      return v8::Date::New(isolate_, CefTime(date_value_).GetDoubleT() * 1000);
    case TYPE_STRING:
      return GetV8String(isolate_, CefString(&string_value_));
    case TYPE_OBJECT:
      return handle_->GetNewV8Handle(should_persist);
    default:
      break;
  }

  NOTREACHED() << "Invalid type for CefV8ValueImpl";
  return v8::Local<v8::Value>();
}

bool CefV8ValueImpl::IsValid() {
  if (!CEF_V8_HAS_ISOLATE() || type_ == TYPE_INVALID ||
      (type_ == TYPE_OBJECT &&
      (!handle_->BelongsToCurrentThread() || !handle_->IsValid()))) {
    return false;
  }
  return true;
}

bool CefV8ValueImpl::IsUndefined() {
  CEF_V8_REQUIRE_ISOLATE_RETURN(false);
  return (type_ == TYPE_UNDEFINED);
}

bool CefV8ValueImpl::IsNull() {
  CEF_V8_REQUIRE_ISOLATE_RETURN(false);
  return (type_ == TYPE_NULL);
}

bool CefV8ValueImpl::IsBool() {
  CEF_V8_REQUIRE_ISOLATE_RETURN(false);
  return (type_ == TYPE_BOOL);
}

bool CefV8ValueImpl::IsInt() {
  CEF_V8_REQUIRE_ISOLATE_RETURN(false);
  return (type_ == TYPE_INT || type_ == TYPE_UINT);
}

bool CefV8ValueImpl::IsUInt() {
  CEF_V8_REQUIRE_ISOLATE_RETURN(false);
  return (type_ == TYPE_INT || type_ == TYPE_UINT);
}

bool CefV8ValueImpl::IsDouble() {
  CEF_V8_REQUIRE_ISOLATE_RETURN(false);
  return (type_ == TYPE_INT || type_ == TYPE_UINT || type_ == TYPE_DOUBLE);
}

bool CefV8ValueImpl::IsDate() {
  CEF_V8_REQUIRE_ISOLATE_RETURN(false);
  return (type_ == TYPE_DATE);
}

bool CefV8ValueImpl::IsString() {
  CEF_V8_REQUIRE_ISOLATE_RETURN(false);
  return (type_ == TYPE_STRING);
}

bool CefV8ValueImpl::IsObject() {
  CEF_V8_REQUIRE_ISOLATE_RETURN(false);
  return (type_ == TYPE_OBJECT);
}

bool CefV8ValueImpl::IsArray() {
  CEF_V8_REQUIRE_MLT_RETURN(false);
  if (type_ == TYPE_OBJECT) {
    v8::HandleScope handle_scope(handle_->isolate());
    return handle_->GetNewV8Handle(false)->IsArray();
  } else {
    return false;
  }
}

bool CefV8ValueImpl::IsFunction() {
  CEF_V8_REQUIRE_MLT_RETURN(false);
  if (type_ == TYPE_OBJECT) {
    v8::HandleScope handle_scope(handle_->isolate());
    return handle_->GetNewV8Handle(false)->IsFunction();
  } else {
    return false;
  }
}

bool CefV8ValueImpl::IsSame(CefRefPtr<CefV8Value> that) {
  CEF_V8_REQUIRE_MLT_RETURN(false);

  CefV8ValueImpl* thatValue = static_cast<CefV8ValueImpl*>(that.get());
  if (!thatValue || !thatValue->IsValid() || type_ != thatValue->type_)
    return false;

  switch (type_) {
    case TYPE_UNDEFINED:
    case TYPE_NULL:
      return true;
    case TYPE_BOOL:
      return (bool_value_ == thatValue->bool_value_);
    case TYPE_INT:
      return (int_value_ == thatValue->int_value_);
    case TYPE_UINT:
      return (uint_value_ == thatValue->uint_value_);
    case TYPE_DOUBLE:
      return (double_value_ == thatValue->double_value_);
    case TYPE_DATE:
      return (CefTime(date_value_).GetTimeT() ==
              CefTime(thatValue->date_value_).GetTimeT());
    case TYPE_STRING:
      return (CefString(&string_value_) ==
              CefString(&thatValue->string_value_));
    case TYPE_OBJECT: {
      return (handle_->GetPersistentV8Handle() ==
              thatValue->handle_->GetPersistentV8Handle());
    }
    default:
      break;
  }

  return false;
}

bool CefV8ValueImpl::GetBoolValue() {
  CEF_V8_REQUIRE_ISOLATE_RETURN(false);
  if (type_ == TYPE_BOOL)
    return bool_value_;
  return false;
}

int32 CefV8ValueImpl::GetIntValue() {
  CEF_V8_REQUIRE_ISOLATE_RETURN(0);
  if (type_ == TYPE_INT || type_ == TYPE_UINT)
    return int_value_;
  return 0;
}

uint32 CefV8ValueImpl::GetUIntValue() {
  CEF_V8_REQUIRE_ISOLATE_RETURN(0);
  if (type_ == TYPE_INT || type_ == TYPE_UINT)
    return uint_value_;
  return 0;
}

double CefV8ValueImpl::GetDoubleValue() {
  CEF_V8_REQUIRE_ISOLATE_RETURN(0.);
  if (type_ == TYPE_DOUBLE)
    return double_value_;
  else if (type_ == TYPE_INT)
    return int_value_;
  else if (type_ == TYPE_UINT)
    return uint_value_;
  return 0.;
}

CefTime CefV8ValueImpl::GetDateValue() {
  CEF_V8_REQUIRE_ISOLATE_RETURN(CefTime(0.));
  if (type_ == TYPE_DATE)
    return date_value_;
  return CefTime(0.);
}

CefString CefV8ValueImpl::GetStringValue() {
  CefString rv;
  CEF_V8_REQUIRE_ISOLATE_RETURN(rv);
  if (type_ == TYPE_STRING)
    rv = CefString(&string_value_);
  return rv;
}

bool CefV8ValueImpl::IsUserCreated() {
  CEF_V8_REQUIRE_OBJECT_RETURN(false);

  v8::Isolate* isolate = handle_->isolate();
  v8::HandleScope handle_scope(isolate);
  v8::Local<v8::Value> value = handle_->GetNewV8Handle(false);
  v8::Local<v8::Object> obj = value->ToObject();

  V8TrackObject* tracker = V8TrackObject::Unwrap(isolate, obj);
  return (tracker != NULL);
}

bool CefV8ValueImpl::HasException() {
  CEF_V8_REQUIRE_OBJECT_RETURN(false);

  return (last_exception_.get() != NULL);
}

CefRefPtr<CefV8Exception> CefV8ValueImpl::GetException() {
  CEF_V8_REQUIRE_OBJECT_RETURN(NULL);

  return last_exception_;
}

bool CefV8ValueImpl::ClearException() {
  CEF_V8_REQUIRE_OBJECT_RETURN(false);

  last_exception_ = NULL;
  return true;
}

bool CefV8ValueImpl::WillRethrowExceptions() {
  CEF_V8_REQUIRE_OBJECT_RETURN(false);

  return rethrow_exceptions_;
}

bool CefV8ValueImpl::SetRethrowExceptions(bool rethrow) {
  CEF_V8_REQUIRE_OBJECT_RETURN(false);

  rethrow_exceptions_ = rethrow;
  return true;
}

bool CefV8ValueImpl::HasValue(const CefString& key) {
  CEF_V8_REQUIRE_OBJECT_RETURN(false);

  v8::Isolate* isolate = handle_->isolate();
  v8::HandleScope handle_scope(isolate);
  v8::Local<v8::Value> value = handle_->GetNewV8Handle(false);
  v8::Local<v8::Object> obj = value->ToObject();
  return obj->Has(GetV8String(isolate, key));
}

bool CefV8ValueImpl::HasValue(int index) {
  CEF_V8_REQUIRE_OBJECT_RETURN(false);

  if (index < 0) {
    NOTREACHED() << "invalid input parameter";
    return false;
  }

  v8::HandleScope handle_scope(handle_->isolate());
  v8::Local<v8::Value> value = handle_->GetNewV8Handle(false);
  v8::Local<v8::Object> obj = value->ToObject();
  return obj->Has(index);
}

bool CefV8ValueImpl::DeleteValue(const CefString& key) {
  CEF_V8_REQUIRE_OBJECT_RETURN(false);

  v8::Isolate* isolate = handle_->isolate();
  v8::HandleScope handle_scope(isolate);
  v8::Local<v8::Value> value = handle_->GetNewV8Handle(false);
  v8::Local<v8::Object> obj = value->ToObject();

  v8::TryCatch try_catch;
  try_catch.SetVerbose(true);
  bool del = obj->Delete(GetV8String(isolate, key));
  return (!HasCaught(try_catch) && del);
}

bool CefV8ValueImpl::DeleteValue(int index) {
  CEF_V8_REQUIRE_OBJECT_RETURN(false);

  if (index < 0) {
    NOTREACHED() << "invalid input parameter";
    return false;
  }

  v8::HandleScope handle_scope(handle_->isolate());
  v8::Local<v8::Value> value = handle_->GetNewV8Handle(false);
  v8::Local<v8::Object> obj = value->ToObject();

  v8::TryCatch try_catch;
  try_catch.SetVerbose(true);
  bool del = obj->Delete(index);
  return (!HasCaught(try_catch) && del);
}

CefRefPtr<CefV8Value> CefV8ValueImpl::GetValue(const CefString& key) {
  CEF_V8_REQUIRE_OBJECT_RETURN(NULL);

  v8::Isolate* isolate = handle_->isolate();
  v8::HandleScope handle_scope(isolate);
  v8::Local<v8::Value> value = handle_->GetNewV8Handle(false);
  v8::Local<v8::Object> obj = value->ToObject();

  v8::TryCatch try_catch;
  try_catch.SetVerbose(true);
  v8::Local<v8::Value> ret_value = obj->Get(GetV8String(isolate, key));
  if (!HasCaught(try_catch) && !ret_value.IsEmpty())
    return new CefV8ValueImpl(isolate, ret_value);
  return NULL;
}

CefRefPtr<CefV8Value> CefV8ValueImpl::GetValue(int index) {
  CEF_V8_REQUIRE_OBJECT_RETURN(NULL);

  if (index < 0) {
    NOTREACHED() << "invalid input parameter";
    return NULL;
  }

  v8::Isolate* isolate = handle_->isolate();
  v8::HandleScope handle_scope(isolate);
  v8::Local<v8::Value> value = handle_->GetNewV8Handle(false);
  v8::Local<v8::Object> obj = value->ToObject();

  v8::TryCatch try_catch;
  try_catch.SetVerbose(true);
  v8::Local<v8::Value> ret_value = obj->Get(v8::Number::New(isolate, index));
  if (!HasCaught(try_catch) && !ret_value.IsEmpty())
    return new CefV8ValueImpl(isolate, ret_value);
  return NULL;
}

bool CefV8ValueImpl::SetValue(const CefString& key,
                              CefRefPtr<CefV8Value> value,
                              PropertyAttribute attribute) {
  CEF_V8_REQUIRE_OBJECT_RETURN(false);

  CefV8ValueImpl* impl = static_cast<CefV8ValueImpl*>(value.get());
  if (impl && impl->IsValid()) {
    v8::Isolate* isolate = handle_->isolate();
    v8::HandleScope handle_scope(isolate);
    v8::Local<v8::Value> value = handle_->GetNewV8Handle(false);
    v8::Local<v8::Object> obj = value->ToObject();

    v8::TryCatch try_catch;
    try_catch.SetVerbose(true);
    bool set;
    // TODO(cef): This usage may not exactly match the previous implementation.
    // Set will trigger interceptors and/or accessors whereas ForceSet will not.
    // It might be better to split this functionality into separate methods.
    if (attribute == V8_PROPERTY_ATTRIBUTE_NONE) {
      set = obj->Set(GetV8String(isolate, key), impl->GetV8Value(true));
    } else {
      set = obj->ForceSet(GetV8String(isolate, key),
                          impl->GetV8Value(true),
                          static_cast<v8::PropertyAttribute>(attribute));
    }
    return (!HasCaught(try_catch) && set);
  } else {
    NOTREACHED() << "invalid input parameter";
    return false;
  }
}

bool CefV8ValueImpl::SetValue(int index, CefRefPtr<CefV8Value> value) {
  CEF_V8_REQUIRE_OBJECT_RETURN(false);

  if (index < 0) {
    NOTREACHED() << "invalid input parameter";
    return false;
  }

  CefV8ValueImpl* impl = static_cast<CefV8ValueImpl*>(value.get());
  if (impl && impl->IsValid()) {
    v8::HandleScope handle_scope(handle_->isolate());
    v8::Local<v8::Value> value = handle_->GetNewV8Handle(false);
    v8::Local<v8::Object> obj = value->ToObject();

    v8::TryCatch try_catch;
    try_catch.SetVerbose(true);
    bool set = obj->Set(index, impl->GetV8Value(true));
    return (!HasCaught(try_catch) && set);
  } else {
    NOTREACHED() << "invalid input parameter";
    return false;
  }
}

bool CefV8ValueImpl::SetValue(const CefString& key, AccessControl settings,
                              PropertyAttribute attribute) {
  CEF_V8_REQUIRE_OBJECT_RETURN(false);

  v8::Isolate* isolate = handle_->isolate();
  v8::HandleScope handle_scope(isolate);
  v8::Local<v8::Value> value = handle_->GetNewV8Handle(false);
  v8::Local<v8::Object> obj = value->ToObject();

  CefRefPtr<CefV8Accessor> accessorPtr;

  V8TrackObject* tracker = V8TrackObject::Unwrap(isolate, obj);
  if (tracker)
    accessorPtr = tracker->GetAccessor();

  // Verify that an accessor exists for this object.
  if (!accessorPtr.get())
    return false;

  v8::AccessorGetterCallback getter = AccessorGetterCallbackImpl;
  v8::AccessorSetterCallback setter =
      (attribute & V8_PROPERTY_ATTRIBUTE_READONLY) ?
          NULL : AccessorSetterCallbackImpl;

  v8::TryCatch try_catch;
  try_catch.SetVerbose(true);
  bool set = obj->SetAccessor(GetV8String(isolate, key), getter, setter, obj,
                              static_cast<v8::AccessControl>(settings),
                              static_cast<v8::PropertyAttribute>(attribute));
  return (!HasCaught(try_catch) && set);
}

bool CefV8ValueImpl::GetKeys(std::vector<CefString>& keys) {
  CEF_V8_REQUIRE_OBJECT_RETURN(false);

  v8::Isolate* isolate = handle_->isolate();
  v8::HandleScope handle_scope(isolate);
  v8::Local<v8::Value> value = handle_->GetNewV8Handle(false);
  v8::Local<v8::Object> obj = value->ToObject();

  v8::Local<v8::Array> arr_keys = obj->GetPropertyNames();
  uint32_t len = arr_keys->Length();
  for (uint32_t i = 0; i < len; ++i) {
    v8::Local<v8::Value> value = arr_keys->Get(v8::Integer::New(isolate, i));
    CefString str;
    GetCefString(value->ToString(), str);
    keys.push_back(str);
  }
  return true;
}

bool CefV8ValueImpl::SetUserData(CefRefPtr<CefBase> user_data) {
  CEF_V8_REQUIRE_OBJECT_RETURN(false);

  v8::Isolate* isolate = handle_->isolate();
  v8::HandleScope handle_scope(isolate);
  v8::Local<v8::Value> value = handle_->GetNewV8Handle(false);
  v8::Local<v8::Object> obj = value->ToObject();

  V8TrackObject* tracker = V8TrackObject::Unwrap(isolate, obj);
  if (tracker) {
    tracker->SetUserData(user_data);
    return true;
  }

  return false;
}

CefRefPtr<CefBase> CefV8ValueImpl::GetUserData() {
  CEF_V8_REQUIRE_OBJECT_RETURN(NULL);

  v8::Isolate* isolate = handle_->isolate();
  v8::HandleScope handle_scope(isolate);
  v8::Local<v8::Value> value = handle_->GetNewV8Handle(false);
  v8::Local<v8::Object> obj = value->ToObject();

  V8TrackObject* tracker = V8TrackObject::Unwrap(isolate, obj);
  if (tracker)
    return tracker->GetUserData();

  return NULL;
}

int CefV8ValueImpl::GetExternallyAllocatedMemory() {
  CEF_V8_REQUIRE_OBJECT_RETURN(0);

  v8::Isolate* isolate = handle_->isolate();
  v8::HandleScope handle_scope(isolate);
  v8::Local<v8::Value> value = handle_->GetNewV8Handle(false);
  v8::Local<v8::Object> obj = value->ToObject();

  V8TrackObject* tracker = V8TrackObject::Unwrap(isolate, obj);
  if (tracker)
    return tracker->GetExternallyAllocatedMemory();

  return 0;
}

int CefV8ValueImpl::AdjustExternallyAllocatedMemory(int change_in_bytes) {
  CEF_V8_REQUIRE_OBJECT_RETURN(0);

  v8::Isolate* isolate = handle_->isolate();
  v8::HandleScope handle_scope(isolate);
  v8::Local<v8::Value> value = handle_->GetNewV8Handle(false);
  v8::Local<v8::Object> obj = value->ToObject();

  V8TrackObject* tracker = V8TrackObject::Unwrap(isolate, obj);
  if (tracker)
    return tracker->AdjustExternallyAllocatedMemory(change_in_bytes);

  return 0;
}

int CefV8ValueImpl::GetArrayLength() {
  CEF_V8_REQUIRE_OBJECT_RETURN(0);

  v8::HandleScope handle_scope(handle_->isolate());
  v8::Local<v8::Value> value = handle_->GetNewV8Handle(false);
  if (!value->IsArray()) {
    NOTREACHED() << "V8 value is not an array";
    return 0;
  }

  v8::Local<v8::Object> obj = value->ToObject();
  v8::Local<v8::Array> arr = v8::Local<v8::Array>::Cast(obj);
  return arr->Length();
}

CefString CefV8ValueImpl::GetFunctionName() {
  CefString rv;
  CEF_V8_REQUIRE_OBJECT_RETURN(rv);

  v8::HandleScope handle_scope(handle_->isolate());
  v8::Local<v8::Value> value = handle_->GetNewV8Handle(false);
  if (!value->IsFunction()) {
    NOTREACHED() << "V8 value is not a function";
    return rv;
  }

  v8::Local<v8::Object> obj = value->ToObject();
  v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(obj);
  GetCefString(v8::Handle<v8::String>::Cast(func->GetName()), rv);
  return rv;
}

CefRefPtr<CefV8Handler> CefV8ValueImpl::GetFunctionHandler() {
  CEF_V8_REQUIRE_OBJECT_RETURN(NULL);

  v8::Isolate* isolate = handle_->isolate();
  v8::HandleScope handle_scope(isolate);
  v8::Local<v8::Value> value = handle_->GetNewV8Handle(false);
  if (!value->IsFunction()) {
    NOTREACHED() << "V8 value is not a function";
    return 0;
  }

  v8::Local<v8::Object> obj = value->ToObject();
  V8TrackObject* tracker = V8TrackObject::Unwrap(isolate, obj);
  if (tracker)
    return tracker->GetHandler();

  return NULL;
}

CefRefPtr<CefV8Value> CefV8ValueImpl::ExecuteFunction(
      CefRefPtr<CefV8Value> object,
      const CefV8ValueList& arguments) {
  // An empty context value defaults to the current context.
  CefRefPtr<CefV8Context> context;
  return ExecuteFunctionWithContext(context, object, arguments);
}

CefRefPtr<CefV8Value> CefV8ValueImpl::ExecuteFunctionWithContext(
      CefRefPtr<CefV8Context> context,
      CefRefPtr<CefV8Value> object,
      const CefV8ValueList& arguments)  {
  CEF_V8_REQUIRE_OBJECT_RETURN(NULL);

  v8::Isolate* isolate = handle_->isolate();
  v8::HandleScope handle_scope(isolate);
  v8::Local<v8::Value> value = handle_->GetNewV8Handle(false);
  if (!value->IsFunction()) {
    NOTREACHED() << "V8 value is not a function";
    return 0;
  }

  if (context.get() && !context->IsValid()) {
    NOTREACHED() << "invalid V8 context parameter";
    return NULL;
  }
  if (object.get() && (!object->IsValid() || !object->IsObject())) {
    NOTREACHED() << "invalid V8 object parameter";
    return NULL;
  }

  int argc = arguments.size();
  if (argc > 0) {
    for (int i = 0; i < argc; ++i) {
      if (!arguments[i].get() || !arguments[i]->IsValid()) {
        NOTREACHED() << "invalid V8 arguments parameter";
        return NULL;
      }
    }
  }

  v8::Local<v8::Context> context_local;
  if (context.get()) {
    CefV8ContextImpl* context_impl =
        static_cast<CefV8ContextImpl*>(context.get());
    context_local = context_impl->GetV8Context();
  } else {
    context_local = isolate->GetCurrentContext();
  }

  v8::Context::Scope context_scope(context_local);

  v8::Local<v8::Object> obj = value->ToObject();
  v8::Local<v8::Function> func = v8::Local<v8::Function>::Cast(obj);
  v8::Local<v8::Object> recv;

  // Default to the global object if no object was provided.
  if (object.get()) {
    CefV8ValueImpl* recv_impl = static_cast<CefV8ValueImpl*>(object.get());
    recv = v8::Local<v8::Object>::Cast(recv_impl->GetV8Value(true));
  } else {
    recv = context_local->Global();
  }

  v8::Local<v8::Value> *argv = NULL;
  if (argc > 0) {
    argv = new v8::Local<v8::Value>[argc];
    for (int i = 0; i < argc; ++i) {
      argv[i] =
          static_cast<CefV8ValueImpl*>(arguments[i].get())->GetV8Value(true);
    }
  }

  CefRefPtr<CefV8Value> retval;

  {
    v8::TryCatch try_catch;
    try_catch.SetVerbose(true);

    v8::MaybeLocal<v8::Value> func_rv =
        CallV8Function(context_local, func, recv, argc, argv,
                       handle_->isolate());

    if (!HasCaught(try_catch) && !func_rv.IsEmpty())
      retval = new CefV8ValueImpl(isolate, func_rv.ToLocalChecked());
  }

  if (argv)
    delete [] argv;

  return retval;
}

bool CefV8ValueImpl::HasCaught(v8::TryCatch& try_catch) {
  if (try_catch.HasCaught()) {
    last_exception_ = new CefV8ExceptionImpl(try_catch.Message());
    if (rethrow_exceptions_)
      try_catch.ReThrow();
    return true;
  } else {
    if (last_exception_.get())
      last_exception_ = NULL;
    return false;
  }
}


// CefV8StackTrace

// static
CefRefPtr<CefV8StackTrace> CefV8StackTrace::GetCurrent(int frame_limit) {
  CEF_V8_REQUIRE_ISOLATE_RETURN(NULL);

  v8::Isolate* isolate = GetIsolateManager()->isolate();
  v8::HandleScope handle_scope(isolate);
  v8::Local<v8::StackTrace> stackTrace =
        v8::StackTrace::CurrentStackTrace(
            isolate, frame_limit, v8::StackTrace::kDetailed);
  if (stackTrace.IsEmpty())
    return NULL;
  return new CefV8StackTraceImpl(isolate, stackTrace);
}


// CefV8StackTraceImpl

CefV8StackTraceImpl::CefV8StackTraceImpl(
    v8::Isolate* isolate,
    v8::Local<v8::StackTrace> handle) {
  if  (!handle.IsEmpty()) {
    int frame_count = handle->GetFrameCount();
    if (frame_count > 0) {
      frames_.reserve(frame_count);
      for (int i = 0; i < frame_count; ++i)
        frames_.push_back(
            new CefV8StackFrameImpl(isolate, handle->GetFrame(i)));
    }
  }
}

CefV8StackTraceImpl::~CefV8StackTraceImpl() {
}

bool CefV8StackTraceImpl::IsValid() {
  return true;
}

int CefV8StackTraceImpl::GetFrameCount() {
  return frames_.size();
}

CefRefPtr<CefV8StackFrame> CefV8StackTraceImpl::GetFrame(int index) {
  if (index < 0 || index >= static_cast<int>(frames_.size()))
    return NULL;
  return frames_[index];
}


// CefV8StackFrameImpl

CefV8StackFrameImpl::CefV8StackFrameImpl(
    v8::Isolate* isolate,
    v8::Local<v8::StackFrame> handle)
    : line_number_(0),
      column_(0),
      is_eval_(false),
      is_constructor_(false) {
  if (handle.IsEmpty())
    return;
  GetCefString(v8::Local<v8::String>::Cast(handle->GetScriptName()),
      script_name_);
  GetCefString(
      v8::Local<v8::String>::Cast(handle->GetScriptNameOrSourceURL()),
      script_name_or_source_url_);
  GetCefString(v8::Local<v8::String>::Cast(handle->GetFunctionName()),
      function_name_);
  line_number_ = handle->GetLineNumber();
  column_ = handle->GetColumn();
  is_eval_ = handle->IsEval();
  is_constructor_ = handle->IsConstructor();
}

CefV8StackFrameImpl::~CefV8StackFrameImpl() {
}

bool CefV8StackFrameImpl::IsValid() {
  return true;
}

CefString CefV8StackFrameImpl::GetScriptName() {
  return script_name_;
}

CefString CefV8StackFrameImpl::GetScriptNameOrSourceURL() {
  return script_name_or_source_url_;
}

CefString CefV8StackFrameImpl::GetFunctionName() {
  return function_name_;
}

int CefV8StackFrameImpl::GetLineNumber() {
  return line_number_;
}

int CefV8StackFrameImpl::GetColumn() {
  return column_;
}

bool CefV8StackFrameImpl::IsEval() {
  return is_eval_;
}

bool CefV8StackFrameImpl::IsConstructor() {
  return is_constructor_;
}
