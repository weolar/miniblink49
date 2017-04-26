// Copyright 2014 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/runtime/runtime-utils.h"

#include "src/accessors.h"
#include "src/arguments.h"
#include "src/compiler.h"
#include "src/deoptimizer.h"
#include "src/frames-inl.h"
#include "src/isolate-inl.h"
#include "src/messages.h"
#include "src/profiler/cpu-profiler.h"

namespace v8 {
namespace internal {

RUNTIME_FUNCTION(Runtime_FunctionGetName) {
  SealHandleScope shs(isolate);
  DCHECK(args.length() == 1);

  CONVERT_ARG_CHECKED(JSFunction, f, 0);
  return f->shared()->name();
}


RUNTIME_FUNCTION(Runtime_FunctionSetName) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 2);

  CONVERT_ARG_HANDLE_CHECKED(JSFunction, f, 0);
  CONVERT_ARG_HANDLE_CHECKED(String, name, 1);

  name = String::Flatten(name);
  f->shared()->set_name(*name);
  return isolate->heap()->undefined_value();
}


RUNTIME_FUNCTION(Runtime_FunctionNameShouldPrintAsAnonymous) {
  SealHandleScope shs(isolate);
  DCHECK(args.length() == 1);
  CONVERT_ARG_CHECKED(JSFunction, f, 0);
  return isolate->heap()->ToBoolean(
      f->shared()->name_should_print_as_anonymous());
}


RUNTIME_FUNCTION(Runtime_CompleteFunctionConstruction) {
  SealHandleScope shs(isolate);
  DCHECK(args.length() == 3);
  CONVERT_ARG_HANDLE_CHECKED(JSFunction, func, 0);
  CONVERT_ARG_HANDLE_CHECKED(JSFunction, constructor, 1);
  CONVERT_ARG_HANDLE_CHECKED(Object, new_target, 2);
  func->shared()->set_name_should_print_as_anonymous(true);

  // If new.target is equal to |constructor| then the function |func| created
  // is already correctly setup and nothing else should be done here.
  // But if new.target is not equal to |constructor| then we are have a
  // Function builtin subclassing case and therefore the function |func|
  // has wrong initial map. To fix that we create a new function object with
  // correct initial map.
  if (new_target->IsUndefined() || *constructor == *new_target) {
    return *func;
  }

  // Create a new JSFunction object with correct initial map.
  HandleScope handle_scope(isolate);
  Handle<JSFunction> original_constructor =
      Handle<JSFunction>::cast(new_target);

  DCHECK(constructor->has_initial_map());
  Handle<Map> initial_map =
      JSFunction::EnsureDerivedHasInitialMap(original_constructor, constructor);

  Handle<SharedFunctionInfo> shared_info(func->shared(), isolate);
  Handle<Context> context(func->context(), isolate);
  Handle<JSFunction> result =
      isolate->factory()->NewFunctionFromSharedFunctionInfo(
          initial_map, shared_info, context, NOT_TENURED);
  DCHECK_EQ(func->IsConstructor(), result->IsConstructor());
  return *result;
}


RUNTIME_FUNCTION(Runtime_FunctionIsArrow) {
  SealHandleScope shs(isolate);
  DCHECK(args.length() == 1);
  CONVERT_ARG_CHECKED(JSFunction, f, 0);
  return isolate->heap()->ToBoolean(f->shared()->is_arrow());
}


RUNTIME_FUNCTION(Runtime_FunctionIsConciseMethod) {
  SealHandleScope shs(isolate);
  DCHECK(args.length() == 1);
  CONVERT_ARG_CHECKED(JSFunction, f, 0);
  return isolate->heap()->ToBoolean(f->shared()->is_concise_method());
}


RUNTIME_FUNCTION(Runtime_FunctionRemovePrototype) {
  SealHandleScope shs(isolate);
  DCHECK(args.length() == 1);

  CONVERT_ARG_CHECKED(JSFunction, f, 0);
  RUNTIME_ASSERT(f->RemovePrototype());

  return isolate->heap()->undefined_value();
}


RUNTIME_FUNCTION(Runtime_FunctionGetScript) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 1);

  CONVERT_ARG_CHECKED(JSFunction, fun, 0);
  Handle<Object> script = Handle<Object>(fun->shared()->script(), isolate);
  if (!script->IsScript()) return isolate->heap()->undefined_value();

  return *Script::GetWrapper(Handle<Script>::cast(script));
}


RUNTIME_FUNCTION(Runtime_FunctionGetSourceCode) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 1);

  CONVERT_ARG_HANDLE_CHECKED(JSFunction, f, 0);
  Handle<SharedFunctionInfo> shared(f->shared());
  return *shared->GetSourceCode();
}


RUNTIME_FUNCTION(Runtime_FunctionGetScriptSourcePosition) {
  SealHandleScope shs(isolate);
  DCHECK(args.length() == 1);

  CONVERT_ARG_CHECKED(JSFunction, fun, 0);
  int pos = fun->shared()->start_position();
  return Smi::FromInt(pos);
}


RUNTIME_FUNCTION(Runtime_FunctionGetPositionForOffset) {
  SealHandleScope shs(isolate);
  DCHECK(args.length() == 2);

  CONVERT_ARG_CHECKED(Code, code, 0);
  CONVERT_NUMBER_CHECKED(int, offset, Int32, args[1]);

  RUNTIME_ASSERT(0 <= offset && offset < code->Size());

  Address pc = code->address() + offset;
  return Smi::FromInt(code->SourcePosition(pc));
}


RUNTIME_FUNCTION(Runtime_FunctionSetInstanceClassName) {
  SealHandleScope shs(isolate);
  DCHECK(args.length() == 2);

  CONVERT_ARG_CHECKED(JSFunction, fun, 0);
  CONVERT_ARG_CHECKED(String, name, 1);
  fun->shared()->set_instance_class_name(name);
  return isolate->heap()->undefined_value();
}


RUNTIME_FUNCTION(Runtime_FunctionSetLength) {
  SealHandleScope shs(isolate);
  DCHECK(args.length() == 2);

  CONVERT_ARG_CHECKED(JSFunction, fun, 0);
  CONVERT_SMI_ARG_CHECKED(length, 1);
  RUNTIME_ASSERT((length & 0xC0000000) == 0xC0000000 ||
                 (length & 0xC0000000) == 0x0);
  fun->shared()->set_length(length);
  return isolate->heap()->undefined_value();
}


RUNTIME_FUNCTION(Runtime_FunctionSetPrototype) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 2);

  CONVERT_ARG_HANDLE_CHECKED(JSFunction, fun, 0);
  CONVERT_ARG_HANDLE_CHECKED(Object, value, 1);
  RUNTIME_ASSERT(fun->IsConstructor());
  RETURN_FAILURE_ON_EXCEPTION(isolate,
                              Accessors::FunctionSetPrototype(fun, value));
  return args[0];  // return TOS
}


RUNTIME_FUNCTION(Runtime_FunctionIsAPIFunction) {
  SealHandleScope shs(isolate);
  DCHECK(args.length() == 1);

  CONVERT_ARG_CHECKED(JSFunction, f, 0);
  return isolate->heap()->ToBoolean(f->shared()->IsApiFunction());
}


RUNTIME_FUNCTION(Runtime_FunctionHidesSource) {
  SealHandleScope shs(isolate);
  DCHECK(args.length() == 1);
  CONVERT_ARG_CHECKED(JSFunction, f, 0);

  SharedFunctionInfo* shared = f->shared();
  bool hide_source = !shared->script()->IsScript() ||
                     Script::cast(shared->script())->hide_source();
  return isolate->heap()->ToBoolean(hide_source);
}


RUNTIME_FUNCTION(Runtime_SetCode) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 2);

  CONVERT_ARG_HANDLE_CHECKED(JSFunction, target, 0);
  CONVERT_ARG_HANDLE_CHECKED(JSFunction, source, 1);

  Handle<SharedFunctionInfo> target_shared(target->shared());
  Handle<SharedFunctionInfo> source_shared(source->shared());
  RUNTIME_ASSERT(!source_shared->bound());

  if (!Compiler::Compile(source, KEEP_EXCEPTION)) {
    return isolate->heap()->exception();
  }

  // Mark both, the source and the target, as un-flushable because the
  // shared unoptimized code makes them impossible to enqueue in a list.
  DCHECK(target_shared->code()->gc_metadata() == NULL);
  DCHECK(source_shared->code()->gc_metadata() == NULL);
  target_shared->set_dont_flush(true);
  source_shared->set_dont_flush(true);

  // Set the code, scope info, formal parameter count, and the length
  // of the target shared function info.
  target_shared->ReplaceCode(source_shared->code());
  target_shared->set_scope_info(source_shared->scope_info());
  target_shared->set_length(source_shared->length());
  target_shared->set_feedback_vector(source_shared->feedback_vector());
  target_shared->set_internal_formal_parameter_count(
      source_shared->internal_formal_parameter_count());
  target_shared->set_start_position_and_type(
      source_shared->start_position_and_type());
  target_shared->set_end_position(source_shared->end_position());
  bool was_native = target_shared->native();
  target_shared->set_compiler_hints(source_shared->compiler_hints());
  target_shared->set_opt_count_and_bailout_reason(
      source_shared->opt_count_and_bailout_reason());
  target_shared->set_native(was_native);
  target_shared->set_profiler_ticks(source_shared->profiler_ticks());
  SharedFunctionInfo::SetScript(
      target_shared, Handle<Object>(source_shared->script(), isolate));

  // Set the code of the target function.
  target->ReplaceCode(source_shared->code());
  DCHECK(target->next_function_link()->IsUndefined());

  // Make sure we get a fresh copy of the literal vector to avoid cross
  // context contamination.
  Handle<Context> context(source->context());
  target->set_context(*context);

  int number_of_literals = source->NumberOfLiterals();
  Handle<LiteralsArray> literals =
      LiteralsArray::New(isolate, handle(target_shared->feedback_vector()),
                         number_of_literals, TENURED);
  target->set_literals(*literals);

  if (isolate->logger()->is_logging_code_events() ||
      isolate->cpu_profiler()->is_profiling()) {
    isolate->logger()->LogExistingFunction(source_shared,
                                           Handle<Code>(source_shared->code()));
  }

  return *target;
}


// Set the native flag on the function.
// This is used to decide if we should transform null and undefined
// into the global object when doing call and apply.
RUNTIME_FUNCTION(Runtime_SetNativeFlag) {
  SealHandleScope shs(isolate);
  RUNTIME_ASSERT(args.length() == 1);

  CONVERT_ARG_CHECKED(Object, object, 0);

  if (object->IsJSFunction()) {
    JSFunction* func = JSFunction::cast(object);
    func->shared()->set_native(true);
  }
  return isolate->heap()->undefined_value();
}


RUNTIME_FUNCTION(Runtime_IsConstructor) {
  SealHandleScope shs(isolate);
  DCHECK_EQ(1, args.length());
  CONVERT_ARG_CHECKED(Object, object, 0);
  return isolate->heap()->ToBoolean(object->IsConstructor());
}


RUNTIME_FUNCTION(Runtime_SetForceInlineFlag) {
  SealHandleScope shs(isolate);
  RUNTIME_ASSERT(args.length() == 1);
  CONVERT_ARG_HANDLE_CHECKED(Object, object, 0);

  if (object->IsJSFunction()) {
    JSFunction* func = JSFunction::cast(*object);
    func->shared()->set_force_inline(true);
  }
  return isolate->heap()->undefined_value();
}


// Find the arguments of the JavaScript function invocation that called
// into C++ code. Collect these in a newly allocated array of handles (possibly
// prefixed by a number of empty handles).
base::SmartArrayPointer<Handle<Object>> Runtime::GetCallerArguments(
    Isolate* isolate, int prefix_argc, int* total_argc) {
  // Find frame containing arguments passed to the caller.
  JavaScriptFrameIterator it(isolate);
  JavaScriptFrame* frame = it.frame();
  List<JSFunction*> functions(2);
  frame->GetFunctions(&functions);
  if (functions.length() > 1) {
    int inlined_jsframe_index = functions.length() - 1;
    TranslatedState translated_values(frame);
    translated_values.Prepare(false, frame->fp());

    int argument_count = 0;
    TranslatedFrame* translated_frame =
        translated_values.GetArgumentsInfoFromJSFrameIndex(
            inlined_jsframe_index, &argument_count);
    TranslatedFrame::iterator iter = translated_frame->begin();

    // Skip the function.
    iter++;

    // Skip the receiver.
    iter++;
    argument_count--;

    *total_argc = prefix_argc + argument_count;
    base::SmartArrayPointer<Handle<Object> > param_data(
        NewArray<Handle<Object> >(*total_argc));
    bool should_deoptimize = false;
    for (int i = 0; i < argument_count; i++) {
      should_deoptimize = should_deoptimize || iter->IsMaterializedObject();
      Handle<Object> value = iter->GetValue();
      param_data[prefix_argc + i] = value;
      iter++;
    }

    if (should_deoptimize) {
      translated_values.StoreMaterializedValuesAndDeopt();
    }

    return param_data;
  } else {
    it.AdvanceToArgumentsFrame();
    frame = it.frame();
    int args_count = frame->ComputeParametersCount();

    *total_argc = prefix_argc + args_count;
    base::SmartArrayPointer<Handle<Object> > param_data(
        NewArray<Handle<Object> >(*total_argc));
    for (int i = 0; i < args_count; i++) {
      Handle<Object> val = Handle<Object>(frame->GetParameter(i), isolate);
      param_data[prefix_argc + i] = val;
    }
    return param_data;
  }
}


RUNTIME_FUNCTION(Runtime_FunctionBindArguments) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 4);
  CONVERT_ARG_HANDLE_CHECKED(JSFunction, bound_function, 0);
  CONVERT_ARG_HANDLE_CHECKED(JSReceiver, bindee, 1);
  CONVERT_ARG_HANDLE_CHECKED(Object, this_object, 2);
  CONVERT_NUMBER_ARG_HANDLE_CHECKED(new_length, 3);

  // TODO(lrn): Create bound function in C++ code from premade shared info.
  bound_function->shared()->set_bound(true);
  bound_function->shared()->set_optimized_code_map(Smi::FromInt(0));
  bound_function->shared()->set_inferred_name(isolate->heap()->empty_string());
  // Get all arguments of calling function (Function.prototype.bind).
  int argc = 0;
  base::SmartArrayPointer<Handle<Object>> arguments =
      Runtime::GetCallerArguments(isolate, 0, &argc);
  // Don't count the this-arg.
  if (argc > 0) {
    RUNTIME_ASSERT(arguments[0].is_identical_to(this_object));
    argc--;
  } else {
    RUNTIME_ASSERT(this_object->IsUndefined());
  }
  // Initialize array of bindings (function, this, and any existing arguments
  // if the function was already bound).
  Handle<BindingsArray> new_bindings;
  int out_index = 0;
  Handle<TypeFeedbackVector> vector(
      bound_function->shared()->feedback_vector());
  if (bindee->IsJSFunction() && JSFunction::cast(*bindee)->shared()->bound()) {
    Handle<BindingsArray> old_bindings(
        JSFunction::cast(*bindee)->function_bindings());
    RUNTIME_ASSERT(old_bindings->bindings_count() >= 0);
    bindee = handle(old_bindings->bound_function(), isolate);
    Handle<Object> old_bound_this(old_bindings->bound_this(), isolate);
    new_bindings = BindingsArray::New(isolate, vector, bindee, old_bound_this,
                                      old_bindings->bindings_count() + argc);
    for (int n = old_bindings->bindings_count(); out_index < n; out_index++) {
      new_bindings->set_binding(out_index, old_bindings->binding(out_index));
    }
  } else {
    new_bindings =
        BindingsArray::New(isolate, vector, bindee, this_object, argc);
  }
  // Copy arguments, skipping the first which is "this_arg".
  for (int j = 0; j < argc; j++, out_index++) {
    new_bindings->set_binding(out_index, *arguments[j + 1]);
  }
  new_bindings->set_map_no_write_barrier(isolate->heap()->fixed_array_map());
  bound_function->set_function_bindings(*new_bindings);

  // Update length. Have to remove the prototype first so that map migration
  // is happy about the number of fields.
  RUNTIME_ASSERT(bound_function->RemovePrototype());

  // The new function should have the same [[Prototype]] as the bindee.
  Handle<Map> bound_function_map =
      bindee->IsConstructor()
          ? isolate->bound_function_with_constructor_map()
          : isolate->bound_function_without_constructor_map();
  PrototypeIterator iter(isolate, bindee);
  Handle<Object> proto = PrototypeIterator::GetCurrent(iter);
  if (bound_function_map->prototype() != *proto) {
    bound_function_map = Map::TransitionToPrototype(bound_function_map, proto,
                                                    REGULAR_PROTOTYPE);
  }
  JSObject::MigrateToMap(bound_function, bound_function_map);
  DCHECK_EQ(bindee->IsConstructor(), bound_function->IsConstructor());

  Handle<String> length_string = isolate->factory()->length_string();
  // These attributes must be kept in sync with how the bootstrapper
  // configures the bound_function_map retrieved above.
  // We use ...IgnoreAttributes() here because of length's read-onliness.
  PropertyAttributes attr =
      static_cast<PropertyAttributes>(DONT_ENUM | READ_ONLY);
  RETURN_FAILURE_ON_EXCEPTION(
      isolate, JSObject::SetOwnPropertyIgnoreAttributes(
                   bound_function, length_string, new_length, attr));
  return *bound_function;
}


RUNTIME_FUNCTION(Runtime_BoundFunctionGetBindings) {
  HandleScope handles(isolate);
  DCHECK(args.length() == 1);
  CONVERT_ARG_HANDLE_CHECKED(JSReceiver, callable, 0);
  if (callable->IsJSFunction()) {
    Handle<JSFunction> function = Handle<JSFunction>::cast(callable);
    if (function->shared()->bound()) {
      RUNTIME_ASSERT(function->function_bindings()->IsBindingsArray());
      Handle<BindingsArray> bindings(function->function_bindings());
      return *BindingsArray::CreateRuntimeBindings(bindings);
    }
  }
  return isolate->heap()->undefined_value();
}


RUNTIME_FUNCTION(Runtime_NewObjectFromBound) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 1);
  // First argument is a function to use as a constructor.
  CONVERT_ARG_HANDLE_CHECKED(JSFunction, function, 0);
  RUNTIME_ASSERT(function->shared()->bound());

  // The argument is a bound function. Extract its bound arguments
  // and callable.
  Handle<BindingsArray> bound_args =
      handle(BindingsArray::cast(function->function_bindings()));
  int bound_argc = bound_args->bindings_count();
  Handle<Object> bound_function(bound_args->bound_function(), isolate);
  DCHECK(!bound_function->IsJSFunction() ||
         !Handle<JSFunction>::cast(bound_function)->shared()->bound());

  int total_argc = 0;
  base::SmartArrayPointer<Handle<Object>> param_data =
      Runtime::GetCallerArguments(isolate, bound_argc, &total_argc);
  for (int i = 0; i < bound_argc; i++) {
    param_data[i] = handle(bound_args->binding(i), isolate);
  }

  Handle<Object> result;
  ASSIGN_RETURN_FAILURE_ON_EXCEPTION(
      isolate, result, Execution::New(isolate, bound_function, bound_function,
                                      total_argc, param_data.get()));
  return *result;
}


RUNTIME_FUNCTION(Runtime_Call) {
  HandleScope scope(isolate);
  DCHECK_LE(2, args.length());
  int const argc = args.length() - 2;
  CONVERT_ARG_HANDLE_CHECKED(JSReceiver, target, 0);
  CONVERT_ARG_HANDLE_CHECKED(Object, receiver, 1);
  ScopedVector<Handle<Object>> argv(argc);
  for (int i = 0; i < argc; ++i) {
    argv[i] = args.at<Object>(2 + i);
  }
  Handle<Object> result;
  ASSIGN_RETURN_FAILURE_ON_EXCEPTION(
      isolate, result,
      Execution::Call(isolate, target, receiver, argc, argv.start()));
  return *result;
}


RUNTIME_FUNCTION(Runtime_Apply) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 5);
  CONVERT_ARG_HANDLE_CHECKED(JSReceiver, fun, 0);
  CONVERT_ARG_HANDLE_CHECKED(Object, receiver, 1);
  CONVERT_ARG_HANDLE_CHECKED(JSObject, arguments, 2);
  CONVERT_INT32_ARG_CHECKED(offset, 3);
  CONVERT_INT32_ARG_CHECKED(argc, 4);
  RUNTIME_ASSERT(offset >= 0);
  // Loose upper bound to allow fuzzing. We'll most likely run out of
  // stack space before hitting this limit.
  static int kMaxArgc = 1000000;
  RUNTIME_ASSERT(argc >= 0 && argc <= kMaxArgc);

  // If there are too many arguments, allocate argv via malloc.
  const int argv_small_size = 10;
  Handle<Object> argv_small_buffer[argv_small_size];
  base::SmartArrayPointer<Handle<Object> > argv_large_buffer;
  Handle<Object>* argv = argv_small_buffer;
  if (argc > argv_small_size) {
    argv = new Handle<Object>[argc];
    if (argv == NULL) return isolate->StackOverflow();
    argv_large_buffer = base::SmartArrayPointer<Handle<Object> >(argv);
  }

  for (int i = 0; i < argc; ++i) {
    ASSIGN_RETURN_FAILURE_ON_EXCEPTION(
        isolate, argv[i], Object::GetElement(isolate, arguments, offset + i));
  }

  Handle<Object> result;
  ASSIGN_RETURN_FAILURE_ON_EXCEPTION(
      isolate, result, Execution::Call(isolate, fun, receiver, argc, argv));
  return *result;
}


RUNTIME_FUNCTION(Runtime_GetOriginalConstructor) {
  SealHandleScope shs(isolate);
  DCHECK(args.length() == 0);
  JavaScriptFrameIterator it(isolate);
  JavaScriptFrame* frame = it.frame();
  // Currently we don't inline [[Construct]] calls.
  return frame->IsConstructor() && !frame->HasInlinedFrames()
             ? frame->GetOriginalConstructor()
             : isolate->heap()->undefined_value();
}


// ES6 section 9.2.1.2, OrdinaryCallBindThis for sloppy callee.
RUNTIME_FUNCTION(Runtime_ConvertReceiver) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 1);
  CONVERT_ARG_HANDLE_CHECKED(Object, receiver, 0);
  if (receiver->IsNull() || receiver->IsUndefined()) {
    return isolate->global_proxy();
  }
  return *Object::ToObject(isolate, receiver).ToHandleChecked();
}


RUNTIME_FUNCTION(Runtime_IsConstructCall) {
  SealHandleScope shs(isolate);
  DCHECK(args.length() == 0);
  JavaScriptFrameIterator it(isolate);
  JavaScriptFrame* frame = it.frame();
  return isolate->heap()->ToBoolean(frame->IsConstructor());
}


RUNTIME_FUNCTION(Runtime_IsFunction) {
  SealHandleScope shs(isolate);
  DCHECK(args.length() == 1);
  CONVERT_ARG_CHECKED(Object, obj, 0);
  return isolate->heap()->ToBoolean(obj->IsJSFunction());
}


RUNTIME_FUNCTION(Runtime_ThrowStrongModeTooFewArguments) {
  HandleScope scope(isolate);
  DCHECK(args.length() == 0);
  THROW_NEW_ERROR_RETURN_FAILURE(isolate,
                                 NewTypeError(MessageTemplate::kStrongArity));
}

}  // namespace internal
}  // namespace v8
