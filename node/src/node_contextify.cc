#include "node.h"
#include "node_internals.h"
#include "node_watchdog.h"
#include "base-object.h"
#include "base-object-inl.h"
#include "env.h"
#include "env-inl.h"
#include "util.h"
#include "util-inl.h"
#include "v8-debug.h"

namespace node {

using v8::AccessType;
using v8::Array;
using v8::Boolean;
using v8::Context;
using v8::Debug;
using v8::EscapableHandleScope;
using v8::External;
using v8::Function;
using v8::FunctionCallbackInfo;
using v8::FunctionTemplate;
using v8::HandleScope;
using v8::Integer;
using v8::Isolate;
using v8::Local;
using v8::Maybe;
using v8::MaybeLocal;
using v8::Name;
using v8::NamedPropertyHandlerConfiguration;
using v8::None;
using v8::Object;
using v8::ObjectTemplate;
using v8::Persistent;
using v8::PropertyAttribute;
using v8::PropertyCallbackInfo;
using v8::Script;
using v8::ScriptCompiler;
using v8::ScriptOrigin;
using v8::String;
using v8::TryCatch;
using v8::UnboundScript;
using v8::V8;
using v8::Value;
using v8::WeakCallbackData;


class ContextifyContext {
 protected:
  enum Kind {
    kSandbox,
    kContext,
    kProxyGlobal
  };

  Environment* const env_;
  Persistent<Object> sandbox_;
  Persistent<Context> context_;
  Persistent<Object> proxy_global_;
  int references_;

 public:
  explicit ContextifyContext(Environment* env, Local<Object> sandbox)
      : env_(env),
        sandbox_(env->isolate(), sandbox),
        // Wait for sandbox_, proxy_global_, and context_ to die
        references_(0) {
    context_.Reset(env->isolate(), CreateV8Context(env));

    sandbox_.SetWeak(this, WeakCallback<Object, kSandbox>);
    sandbox_.MarkIndependent();
    references_++;

    // Allocation failure or maximum call stack size reached
    if (context_.IsEmpty())
      return;
    context_.SetWeak(this, WeakCallback<Context, kContext>);
    context_.MarkIndependent();
    references_++;

    proxy_global_.Reset(env->isolate(), context()->Global());
    proxy_global_.SetWeak(this, WeakCallback<Object, kProxyGlobal>);
    proxy_global_.MarkIndependent();
    references_++;
  }


  ~ContextifyContext() {
    context_.Reset();
    proxy_global_.Reset();
    sandbox_.Reset();
  }


  inline Environment* env() const {
    return env_;
  }


  inline Local<Context> context() const {
    return PersistentToLocal(env()->isolate(), context_);
  }


  // XXX(isaacs): This function only exists because of a shortcoming of
  // the V8 SetNamedPropertyHandler function.
  //
  // It does not provide a way to intercept Object.defineProperty(..)
  // calls.  As a result, these properties are not copied onto the
  // contextified sandbox when a new global property is added via either
  // a function declaration or a Object.defineProperty(global, ...) call.
  //
  // Note that any function declarations or Object.defineProperty()
  // globals that are created asynchronously (in a setTimeout, callback,
  // etc.) will happen AFTER the call to copy properties, and thus not be
  // caught.
  //
  // The way to properly fix this is to add some sort of a
  // Object::SetNamedDefinePropertyHandler() function that takes a callback,
  // which receives the property name and property descriptor as arguments.
  //
  // Luckily, such situations are rare, and asynchronously-added globals
  // weren't supported by Node's VM module until 0.12 anyway.  But, this
  // should be fixed properly in V8, and this copy function should be
  // removed once there is a better way.
  void CopyProperties() {
    HandleScope scope(env()->isolate());

    Local<Context> context = PersistentToLocal(env()->isolate(), context_);
    Local<Object> global =
        context->Global()->GetPrototype()->ToObject(env()->isolate());
    Local<Object> sandbox = PersistentToLocal(env()->isolate(), sandbox_);

    Local<Function> clone_property_method;

    Local<Array> names = global->GetOwnPropertyNames();
    int length = names->Length();
    for (int i = 0; i < length; i++) {
      Local<String> key = names->Get(i)->ToString(env()->isolate());
      bool has = sandbox->HasOwnProperty(key);
      if (!has) {
        // Could also do this like so:
        //
        // PropertyAttribute att = global->GetPropertyAttributes(key_v);
        // Local<Value> val = global->Get(key_v);
        // sandbox->ForceSet(key_v, val, att);
        //
        // However, this doesn't handle ES6-style properties configured with
        // Object.defineProperty, and that's exactly what we're up against at
        // this point.  ForceSet(key,val,att) only supports value properties
        // with the ES3-style attribute flags (DontDelete/DontEnum/ReadOnly),
        // which doesn't faithfully capture the full range of configurations
        // that can be done using Object.defineProperty.
        if (clone_property_method.IsEmpty()) {
          Local<String> code = FIXED_ONE_BYTE_STRING(env()->isolate(),
              "(function cloneProperty(source, key, target) {\n"
              "  if (key === 'Proxy') return;\n"
              "  try {\n"
              "    var desc = Object.getOwnPropertyDescriptor(source, key);\n"
              "    if (desc.value === source) desc.value = target;\n"
              "    Object.defineProperty(target, key, desc);\n"
              "  } catch (e) {\n"
              "   // Catch sealed properties errors\n"
              "  }\n"
              "})");

          Local<String> fname = FIXED_ONE_BYTE_STRING(env()->isolate(),
              "binding:script");
          Local<Script> script = Script::Compile(code, fname);
          clone_property_method = Local<Function>::Cast(script->Run());
          CHECK(clone_property_method->IsFunction());
        }
        Local<Value> args[] = { global, key, sandbox };
        clone_property_method->Call(global, ARRAY_SIZE(args), args);
      }
    }
  }


  // This is an object that just keeps an internal pointer to this
  // ContextifyContext.  It's passed to the NamedPropertyHandler.  If we
  // pass the main JavaScript context object we're embedded in, then the
  // NamedPropertyHandler will store a reference to it forever and keep it
  // from getting gc'd.
  Local<Value> CreateDataWrapper(Environment* env) {
    EscapableHandleScope scope(env->isolate());
    Local<Object> wrapper =
        env->script_data_constructor_function()->NewInstance();
    if (wrapper.IsEmpty())
      return scope.Escape(Local<Value>::New(env->isolate(), Local<Value>()));

    Wrap(wrapper, this);
    return scope.Escape(wrapper);
  }


  Local<Context> CreateV8Context(Environment* env) {
    EscapableHandleScope scope(env->isolate());
    Local<FunctionTemplate> function_template =
        FunctionTemplate::New(env->isolate());
    function_template->SetHiddenPrototype(true);

    Local<Object> sandbox = PersistentToLocal(env->isolate(), sandbox_);
    function_template->SetClassName(sandbox->GetConstructorName());

    Local<ObjectTemplate> object_template =
        function_template->InstanceTemplate();

    NamedPropertyHandlerConfiguration config(GlobalPropertyGetterCallback,
                                             GlobalPropertySetterCallback,
                                             GlobalPropertyQueryCallback,
                                             GlobalPropertyDeleterCallback,
                                             GlobalPropertyEnumeratorCallback,
                                             CreateDataWrapper(env));
    object_template->SetHandler(config);

    Local<Context> ctx = Context::New(env->isolate(), nullptr, object_template);

    CHECK(!ctx.IsEmpty());
    ctx->SetSecurityToken(env->context()->GetSecurityToken());

    env->AssignToContext(ctx);

    return scope.Escape(ctx);
  }


  static void Init(Environment* env, Local<Object> target) {
    Local<FunctionTemplate> function_template =
        FunctionTemplate::New(env->isolate());
    function_template->InstanceTemplate()->SetInternalFieldCount(1);
    env->set_script_data_constructor_function(function_template->GetFunction());

    env->SetMethod(target, "runInDebugContext", RunInDebugContext);
    env->SetMethod(target, "makeContext", MakeContext);
    env->SetMethod(target, "isContext", IsContext);
  }


  static void RunInDebugContext(const FunctionCallbackInfo<Value>& args) {
    Local<String> script_source(args[0]->ToString(args.GetIsolate()));
    if (script_source.IsEmpty())
      return;  // Exception pending.
    Local<Context> debug_context = Debug::GetDebugContext();
    Environment* env = Environment::GetCurrent(args);
    if (debug_context.IsEmpty()) {
      // Force-load the debug context.
      Debug::GetMirror(args.GetIsolate()->GetCurrentContext(), args[0]);
      debug_context = Debug::GetDebugContext();
      CHECK(!debug_context.IsEmpty());
      // Ensure that the debug context has an Environment assigned in case
      // a fatal error is raised.  The fatal exception handler in node.cc
      // is not equipped to deal with contexts that don't have one and
      // can't easily be taught that due to a deficiency in the V8 API:
      // there is no way for the embedder to tell if the data index is
      // in use.
      const int index = Environment::kContextEmbedderDataIndex;
      debug_context->SetAlignedPointerInEmbedderData(index, env);
    }

    Context::Scope context_scope(debug_context);
    Local<Script> script = Script::Compile(script_source);
    if (script.IsEmpty())
      return;  // Exception pending.
    args.GetReturnValue().Set(script->Run());
  }


  static void MakeContext(const FunctionCallbackInfo<Value>& args) {
    Environment* env = Environment::GetCurrent(args);

    if (!args[0]->IsObject()) {
      return env->ThrowTypeError("sandbox argument must be an object.");
    }
    Local<Object> sandbox = args[0].As<Object>();

    Local<String> hidden_name =
        FIXED_ONE_BYTE_STRING(env->isolate(), "_contextifyHidden");

    // Don't allow contextifying a sandbox multiple times.
    CHECK(sandbox->GetHiddenValue(hidden_name).IsEmpty());

    TryCatch try_catch;
    ContextifyContext* context = new ContextifyContext(env, sandbox);

    if (try_catch.HasCaught()) {
      try_catch.ReThrow();
      return;
    }

    if (context->context().IsEmpty())
      return;

    Local<External> hidden_context = External::New(env->isolate(), context);
    sandbox->SetHiddenValue(hidden_name, hidden_context);
  }


  static void IsContext(const FunctionCallbackInfo<Value>& args) {
    Environment* env = Environment::GetCurrent(args);

    if (!args[0]->IsObject()) {
      env->ThrowTypeError("sandbox must be an object");
      return;
    }
    Local<Object> sandbox = args[0].As<Object>();

    Local<String> hidden_name =
        FIXED_ONE_BYTE_STRING(env->isolate(), "_contextifyHidden");

    args.GetReturnValue().Set(!sandbox->GetHiddenValue(hidden_name).IsEmpty());
  }


  template <class T, Kind kind>
  static void WeakCallback(const WeakCallbackData<T, ContextifyContext>& data) {
    ContextifyContext* context = data.GetParameter();
    if (kind == kSandbox)
      context->sandbox_.ClearWeak();
    else if (kind == kContext)
      context->context_.ClearWeak();
    else
      context->proxy_global_.ClearWeak();

    if (--context->references_ == 0)
      delete context;
  }


  static ContextifyContext* ContextFromContextifiedSandbox(
      Isolate* isolate,
      const Local<Object>& sandbox) {
    Local<String> hidden_name =
        FIXED_ONE_BYTE_STRING(isolate, "_contextifyHidden");
    Local<Value> context_external_v = sandbox->GetHiddenValue(hidden_name);
    if (context_external_v.IsEmpty() || !context_external_v->IsExternal()) {
      return nullptr;
    }
    Local<External> context_external = context_external_v.As<External>();

    return static_cast<ContextifyContext*>(context_external->Value());
  }


  static void GlobalPropertyGetterCallback(
      Local<Name> property,
      const PropertyCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();

    ContextifyContext* ctx =
        Unwrap<ContextifyContext>(args.Data().As<Object>());

    // Stil initializing
    if (ctx->context_.IsEmpty())
      return;

    Local<Object> sandbox = PersistentToLocal(isolate, ctx->sandbox_);
    MaybeLocal<Value> maybe_rv =
        sandbox->GetRealNamedProperty(ctx->context(), property);
    if (maybe_rv.IsEmpty()) {
      Local<Object> proxy_global = PersistentToLocal(isolate,
                                                     ctx->proxy_global_);
      maybe_rv = proxy_global->GetRealNamedProperty(ctx->context(), property);
    }

    Local<Value> rv;
    if (maybe_rv.ToLocal(&rv)) {
      if (rv == ctx->sandbox_)
        rv = PersistentToLocal(isolate, ctx->proxy_global_);

      args.GetReturnValue().Set(rv);
    }
  }


  static void GlobalPropertySetterCallback(
      Local<Name> property,
      Local<Value> value,
      const PropertyCallbackInfo<Value>& args) {
    Isolate* isolate = args.GetIsolate();

    ContextifyContext* ctx =
        Unwrap<ContextifyContext>(args.Data().As<Object>());

    // Stil initializing
    if (ctx->context_.IsEmpty())
      return;

    PersistentToLocal(isolate, ctx->sandbox_)->Set(property, value);
  }


  static void GlobalPropertyQueryCallback(
      Local<Name> property,
      const PropertyCallbackInfo<Integer>& args) {
    Isolate* isolate = args.GetIsolate();

    ContextifyContext* ctx =
        Unwrap<ContextifyContext>(args.Data().As<Object>());

    // Stil initializing
    if (ctx->context_.IsEmpty())
      return;

    Local<Object> sandbox = PersistentToLocal(isolate, ctx->sandbox_);
    Maybe<PropertyAttribute> maybe_prop_attr =
        sandbox->GetRealNamedPropertyAttributes(ctx->context(), property);

    if (maybe_prop_attr.IsNothing()) {
      Local<Object> proxy_global = PersistentToLocal(isolate,
          ctx->proxy_global_);

      maybe_prop_attr =
          proxy_global->GetRealNamedPropertyAttributes(ctx->context(),
              property);
    }

    if (maybe_prop_attr.IsJust()) {
      PropertyAttribute prop_attr = maybe_prop_attr.FromJust();
      args.GetReturnValue().Set(prop_attr);
    }
  }


  static void GlobalPropertyDeleterCallback(
      Local<Name> property,
      const PropertyCallbackInfo<Boolean>& args) {
    Isolate* isolate = args.GetIsolate();

    ContextifyContext* ctx =
        Unwrap<ContextifyContext>(args.Data().As<Object>());

    // Stil initializing
    if (ctx->context_.IsEmpty())
      return;

    Local<Object> sandbox = PersistentToLocal(isolate, ctx->sandbox_);

    Maybe<bool> success = sandbox->Delete(ctx->context(), property);

    if (success.IsJust())
      args.GetReturnValue().Set(success.FromJust());
  }


  static void GlobalPropertyEnumeratorCallback(
      const PropertyCallbackInfo<Array>& args) {
    ContextifyContext* ctx =
        Unwrap<ContextifyContext>(args.Data().As<Object>());

    // Stil initializing
    if (ctx->context_.IsEmpty())
      return;

    Local<Object> sandbox = PersistentToLocal(args.GetIsolate(), ctx->sandbox_);
    args.GetReturnValue().Set(sandbox->GetPropertyNames());
  }
};

class ContextifyScript : public BaseObject {
 private:
  Persistent<UnboundScript> script_;

 public:
  static void Init(Environment* env, Local<Object> target) {
    HandleScope scope(env->isolate());
    Local<String> class_name =
        FIXED_ONE_BYTE_STRING(env->isolate(), "ContextifyScript");

    Local<FunctionTemplate> script_tmpl = env->NewFunctionTemplate(New);
    script_tmpl->InstanceTemplate()->SetInternalFieldCount(1);
    script_tmpl->SetClassName(class_name);
    env->SetProtoMethod(script_tmpl, "runInContext", RunInContext);
    env->SetProtoMethod(script_tmpl, "runInThisContext", RunInThisContext);

    target->Set(class_name, script_tmpl->GetFunction());
    env->set_script_context_constructor_template(script_tmpl);
  }


  // args: code, [options]
  static void New(const FunctionCallbackInfo<Value>& args) {
    Environment* env = Environment::GetCurrent(args);

    if (!args.IsConstructCall()) {
      return env->ThrowError("Must call vm.Script as a constructor.");
    }

    ContextifyScript* contextify_script =
        new ContextifyScript(env, args.This());

    TryCatch try_catch;
    Local<String> code = args[0]->ToString(env->isolate());
    Local<String> filename = GetFilenameArg(args, 1);
    Local<Integer> lineOffset = GetLineOffsetArg(args, 1);
    Local<Integer> columnOffset = GetColumnOffsetArg(args, 1);
    bool display_errors = GetDisplayErrorsArg(args, 1);
    if (try_catch.HasCaught()) {
      try_catch.ReThrow();
      return;
    }

    ScriptOrigin origin(filename, lineOffset, columnOffset);
    ScriptCompiler::Source source(code, origin);
    Local<UnboundScript> v8_script =
        ScriptCompiler::CompileUnbound(env->isolate(), &source);

    if (v8_script.IsEmpty()) {
      if (display_errors) {
        AppendExceptionLine(env, try_catch.Exception(), try_catch.Message());
      }
      try_catch.ReThrow();
      return;
    }
    contextify_script->script_.Reset(env->isolate(), v8_script);
  }


  static bool InstanceOf(Environment* env, const Local<Value>& value) {
    return !value.IsEmpty() &&
           env->script_context_constructor_template()->HasInstance(value);
  }


  // args: [options]
  static void RunInThisContext(const FunctionCallbackInfo<Value>& args) {
    // Assemble arguments
    TryCatch try_catch;
    uint64_t timeout = GetTimeoutArg(args, 0);
    bool display_errors = GetDisplayErrorsArg(args, 0);
    if (try_catch.HasCaught()) {
      try_catch.ReThrow();
      return;
    }

    // Do the eval within this context
    Environment* env = Environment::GetCurrent(args);
    EvalMachine(env, timeout, display_errors, args, try_catch);
  }

  // args: sandbox, [options]
  static void RunInContext(const FunctionCallbackInfo<Value>& args) {
    Environment* env = Environment::GetCurrent(args);

    int64_t timeout;
    bool display_errors;

    // Assemble arguments
    if (!args[0]->IsObject()) {
      return env->ThrowTypeError(
          "contextifiedSandbox argument must be an object.");
    }

    Local<Object> sandbox = args[0].As<Object>();
    {
      TryCatch try_catch;
      timeout = GetTimeoutArg(args, 1);
      display_errors = GetDisplayErrorsArg(args, 1);
      if (try_catch.HasCaught()) {
        try_catch.ReThrow();
        return;
      }
    }

    // Get the context from the sandbox
    ContextifyContext* contextify_context =
        ContextifyContext::ContextFromContextifiedSandbox(env->isolate(),
                                                          sandbox);
    if (contextify_context == nullptr) {
      return env->ThrowTypeError(
          "sandbox argument must have been converted to a context.");
    }

    if (contextify_context->context().IsEmpty())
      return;

    {
      TryCatch try_catch;
      // Do the eval within the context
      Context::Scope context_scope(contextify_context->context());
      if (EvalMachine(contextify_context->env(),
                      timeout,
                      display_errors,
                      args,
                      try_catch)) {
        contextify_context->CopyProperties();
      }

      if (try_catch.HasCaught()) {
        try_catch.ReThrow();
        return;
      }
    }
  }

  static int64_t GetTimeoutArg(const FunctionCallbackInfo<Value>& args,
                               const int i) {
    if (args[i]->IsUndefined() || args[i]->IsString()) {
      return -1;
    }
    if (!args[i]->IsObject()) {
      Environment::ThrowTypeError(args.GetIsolate(),
                                  "options must be an object");
      return -1;
    }

    Local<String> key = FIXED_ONE_BYTE_STRING(args.GetIsolate(), "timeout");
    Local<Value> value = args[i].As<Object>()->Get(key);
    if (value->IsUndefined()) {
      return -1;
    }
    int64_t timeout = value->IntegerValue();

    if (timeout <= 0) {
      Environment::ThrowRangeError(args.GetIsolate(),
                                   "timeout must be a positive number");
      return -1;
    }
    return timeout;
  }


  static bool GetDisplayErrorsArg(const FunctionCallbackInfo<Value>& args,
                                  const int i) {
    if (args[i]->IsUndefined() || args[i]->IsString()) {
      return true;
    }
    if (!args[i]->IsObject()) {
      Environment::ThrowTypeError(args.GetIsolate(),
                                  "options must be an object");
      return false;
    }

    Local<String> key = FIXED_ONE_BYTE_STRING(args.GetIsolate(),
                                              "displayErrors");
    Local<Value> value = args[i].As<Object>()->Get(key);

    return value->IsUndefined() ? true : value->BooleanValue();
  }


  static Local<String> GetFilenameArg(const FunctionCallbackInfo<Value>& args,
                                      const int i) {
    Local<String> defaultFilename =
        FIXED_ONE_BYTE_STRING(args.GetIsolate(), "evalmachine.<anonymous>");

    if (args[i]->IsUndefined()) {
      return defaultFilename;
    }
    if (args[i]->IsString()) {
      return args[i].As<String>();
    }
    if (!args[i]->IsObject()) {
      Environment::ThrowTypeError(args.GetIsolate(),
                                  "options must be an object");
      return Local<String>();
    }

    Local<String> key = FIXED_ONE_BYTE_STRING(args.GetIsolate(), "filename");
    Local<Value> value = args[i].As<Object>()->Get(key);

    if (value->IsUndefined())
      return defaultFilename;
    return value->ToString(args.GetIsolate());
  }


  static Local<Integer> GetLineOffsetArg(
                                      const FunctionCallbackInfo<Value>& args,
                                      const int i) {
    Local<Integer> defaultLineOffset = Integer::New(args.GetIsolate(), 0);

    if (!args[i]->IsObject()) {
      return defaultLineOffset;
    }

    Local<String> key = FIXED_ONE_BYTE_STRING(args.GetIsolate(), "lineOffset");
    Local<Value> value = args[i].As<Object>()->Get(key);

    return value->IsUndefined() ? defaultLineOffset : value->ToInteger();
  }


  static Local<Integer> GetColumnOffsetArg(
                                      const FunctionCallbackInfo<Value>& args,
                                      const int i) {
    Local<Integer> defaultColumnOffset = Integer::New(args.GetIsolate(), 0);

    if (!args[i]->IsObject()) {
      return defaultColumnOffset;
    }

    Local<String> key = FIXED_ONE_BYTE_STRING(args.GetIsolate(),
                                              "columnOffset");
    Local<Value> value = args[i].As<Object>()->Get(key);

    return value->IsUndefined() ? defaultColumnOffset : value->ToInteger();
  }


  static bool EvalMachine(Environment* env,
                          const int64_t timeout,
                          const bool display_errors,
                          const FunctionCallbackInfo<Value>& args,
                          TryCatch& try_catch) {
    if (!ContextifyScript::InstanceOf(env, args.Holder())) {
      env->ThrowTypeError(
          "Script methods can only be called on script instances.");
      return false;
    }

    ContextifyScript* wrapped_script = Unwrap<ContextifyScript>(args.Holder());
    Local<UnboundScript> unbound_script =
        PersistentToLocal(env->isolate(), wrapped_script->script_);
    Local<Script> script = unbound_script->BindToCurrentContext();

    Local<Value> result;
    if (timeout != -1) {
      Watchdog wd(env, timeout);
      result = script->Run();
    } else {
      result = script->Run();
    }

    if (try_catch.HasCaught() && try_catch.HasTerminated()) {
      V8::CancelTerminateExecution(env->isolate());
      env->ThrowError("Script execution timed out.");
      try_catch.ReThrow();
      return false;
    }

    if (result.IsEmpty()) {
      // Error occurred during execution of the script.
      if (display_errors) {
        AppendExceptionLine(env, try_catch.Exception(), try_catch.Message());
      }
      try_catch.ReThrow();
      return false;
    }

    args.GetReturnValue().Set(result);
    return true;
  }


  ContextifyScript(Environment* env, Local<Object> object)
      : BaseObject(env, object) {
    MakeWeak<ContextifyScript>(this);
  }


  ~ContextifyScript() override {
    script_.Reset();
  }
};


void InitContextify(Local<Object> target,
                    Local<Value> unused,
                    Local<Context> context) {
  Environment* env = Environment::GetCurrent(context);
  ContextifyContext::Init(env, target);
  ContextifyScript::Init(env, target);
}

}  // namespace node

NODE_MODULE_CONTEXT_AWARE_BUILTIN(contextify, node::InitContextify);
