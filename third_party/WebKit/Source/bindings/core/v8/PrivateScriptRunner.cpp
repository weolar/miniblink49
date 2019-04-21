// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "bindings/core/v8/PrivateScriptRunner.h"

#include "bindings/core/v8/DOMWrapperWorld.h"
#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/V8Binding.h"
#include "bindings/core/v8/V8PerContextData.h"
#include "bindings/core/v8/V8ScriptRunner.h"
#include "core/PrivateScriptSources.h"
#ifndef NDEBUG
#include "core/PrivateScriptSourcesForTesting.h"
#endif
#include "core/dom/Document.h"
#include "core/dom/ExceptionCode.h"
#include "platform/PlatformResourceLoader.h"

namespace blink {

static void dumpV8Message(v8::Local<v8::Context> context, v8::Local<v8::Message> message)
{
    if (message.IsEmpty())
        return;

    // FIXME: GetScriptOrigin() and GetLineNumber() return empty handles
    // when they are called at the first time if V8 has a pending exception.
    // So we need to call twice to get a correct ScriptOrigin and line number.
    // This is a bug of V8.
    message->GetScriptOrigin();
    v8::Maybe<int> unused = message->GetLineNumber(context);
    ALLOW_UNUSED_LOCAL(unused);

    v8::Local<v8::Value> resourceName = message->GetScriptOrigin().ResourceName();
    String fileName = "Unknown JavaScript file";
    if (!resourceName.IsEmpty() && resourceName->IsString())
        fileName = toCoreString(v8::Local<v8::String>::Cast(resourceName));
    int lineNumber = 0;
    v8Call(message->GetLineNumber(context), lineNumber);
    v8::Local<v8::String> errorMessage = message->Get();
    fprintf(stderr, "%s (line %d): %s\n", fileName.utf8().data(), lineNumber, toCoreString(errorMessage).utf8().data());
}

static void importFunction(const v8::FunctionCallbackInfo<v8::Value>& args);

static v8::Local<v8::Value> compileAndRunPrivateScript(ScriptState* scriptState, String scriptClassName, const char* source, size_t size)
{
    v8::TryCatch block(scriptState->isolate());
    String sourceString(source, size);
    String fileName = scriptClassName + ".js";

    v8::Isolate* isolate = scriptState->isolate();
    v8::Local<v8::Context> context = scriptState->context();
    v8::Local<v8::Object> global = context->Global();
    v8::Local<v8::Value> privateScriptController = global->Get(context, v8String(isolate, "privateScriptController")).ToLocalChecked();
    RELEASE_ASSERT(privateScriptController->IsUndefined() || privateScriptController->IsObject());
    if (privateScriptController->IsObject()) {
        v8::Local<v8::Object> privateScriptControllerObject = privateScriptController.As<v8::Object>();
        v8::Local<v8::Value> importFunctionValue = privateScriptControllerObject->Get(context, v8String(isolate, "import")).ToLocalChecked();
        if (importFunctionValue->IsUndefined()) {
            v8::Local<v8::Function> function;
            if (!v8::FunctionTemplate::New(isolate, importFunction)->GetFunction(context).ToLocal(&function)
                || !v8CallBoolean(privateScriptControllerObject->Set(context, v8String(isolate, "import"), function))) {
                fprintf(stderr, "Private script error: Setting import function failed. (Class name = %s)\n", scriptClassName.utf8().data());
                dumpV8Message(context, block.Message());
                RELEASE_ASSERT_NOT_REACHED();
            }
        }
    }

    v8::Local<v8::Script> script;
    if (!v8Call(V8ScriptRunner::compileScript(v8String(isolate, sourceString), fileName, String(), TextPosition::minimumPosition(), isolate, nullptr, nullptr, nullptr, NotSharableCrossOrigin), script, block)) {
        fprintf(stderr, "Private script error: Compile failed. (Class name = %s)\n", scriptClassName.utf8().data());
        dumpV8Message(context, block.Message());
        RELEASE_ASSERT_NOT_REACHED();
    }

    v8::Local<v8::Value> result;
    if (!v8Call(V8ScriptRunner::runCompiledInternalScript(isolate, script), result, block)) {
        fprintf(stderr, "Private script error: installClass() failed. (Class name = %s)\n", scriptClassName.utf8().data());
        dumpV8Message(context, block.Message());
        RELEASE_ASSERT_NOT_REACHED();
    }
    return result;
}

// Private scripts can use privateScriptController.import(bundledResource, compileAndRunScript) to import dependent resources.
// |bundledResource| is a string resource name.
// |compileAndRunScript| optional boolean representing if the javascript should be executed. Default: true.
void importFunction(const v8::FunctionCallbackInfo<v8::Value>& args)
{
    v8::Isolate* isolate = args.GetIsolate();
    RELEASE_ASSERT(isolate && (args.Length() >= 1));
    String resourceFileName = toCoreString(args[0]->ToString(isolate->GetCurrentContext()).ToLocalChecked());
    String resourceData = loadResourceAsASCIIString(resourceFileName.utf8().data());
    RELEASE_ASSERT(resourceData.length());
    bool compileAndRunScript = true;
    if (args.Length() == 2) {
        RELEASE_ASSERT(args[1]->IsBoolean());
        compileAndRunScript = args[1].As<v8::Boolean>()->Value();
    }

    if (resourceFileName.endsWith(".js") && compileAndRunScript)
        compileAndRunPrivateScript(ScriptState::current(isolate), resourceFileName.replace(".js", ""), resourceData.utf8().data(), resourceData.length());
    args.GetReturnValue().Set(v8String(isolate, resourceData));
}

// FIXME: If we have X.js, XPartial-1.js and XPartial-2.js, currently all of the JS files
// are compiled when any of the JS files is requested. Ideally we should avoid compiling
// unrelated JS files. For example, if a method in XPartial-1.js is requested, we just
// need to compile X.js and XPartial-1.js, and don't need to compile XPartial-2.js.
static void installPrivateScript(v8::Isolate* isolate, String className)
{
    ScriptState* scriptState = ScriptState::current(isolate);
    int compiledScriptCount = 0;
    // |kPrivateScriptSourcesForTesting| is defined in V8PrivateScriptSources.h, which is auto-generated
    // by make_private_script_source.py.
#ifndef NDEBUG
    for (size_t index = 0; index < WTF_ARRAY_LENGTH(kPrivateScriptSourcesForTesting); index++) {
        if (className == kPrivateScriptSourcesForTesting[index].className) {
            compileAndRunPrivateScript(scriptState, kPrivateScriptSourcesForTesting[index].scriptClassName, kPrivateScriptSourcesForTesting[index].source, kPrivateScriptSourcesForTesting[index].size);
            compiledScriptCount++;
        }
    }
#endif

    // |kPrivateScriptSources| is defined in V8PrivateScriptSources.h, which is auto-generated
    // by make_private_script_source.py.
    for (size_t index = 0; index < WTF_ARRAY_LENGTH(kPrivateScriptSources); index++) {
        if (className == kPrivateScriptSources[index].className) {
            String resourceData = loadResourceAsASCIIString(kPrivateScriptSources[index].resourceFile);
            compileAndRunPrivateScript(scriptState, kPrivateScriptSources[index].scriptClassName, resourceData.utf8().data(), resourceData.length());
            compiledScriptCount++;
        }
    }

    if (!compiledScriptCount) {
        fprintf(stderr, "Private script error: Target source code was not found. (Class name = %s)\n", className.utf8().data());
        RELEASE_ASSERT_NOT_REACHED();
    }
}

static v8::Local<v8::Value> installPrivateScriptRunner(v8::Isolate* isolate)
{
    const String className = "PrivateScriptRunner";
    size_t index;
    // |kPrivateScriptSources| is defined in V8PrivateScriptSources.h, which is auto-generated
    // by make_private_script_source.py.
    for (index = 0; index < WTF_ARRAY_LENGTH(kPrivateScriptSources); index++) {
        if (className == kPrivateScriptSources[index].className)
            break;
    }
    if (index == WTF_ARRAY_LENGTH(kPrivateScriptSources)) {
        fprintf(stderr, "Private script error: Target source code was not found. (Class name = %s)\n", className.utf8().data());
        RELEASE_ASSERT_NOT_REACHED();
    }
    String resourceData = loadResourceAsASCIIString(kPrivateScriptSources[index].resourceFile);
    return compileAndRunPrivateScript(ScriptState::current(isolate), className, resourceData.utf8().data(), resourceData.length());
}

static v8::Local<v8::Object> classObjectOfPrivateScript(ScriptState* scriptState, String className)
{
    ASSERT(scriptState->perContextData());
    ASSERT(scriptState->executionContext());
    v8::Isolate* isolate = scriptState->isolate();
    v8::Local<v8::Value> compiledClass = scriptState->perContextData()->compiledPrivateScript(className);
    if (compiledClass.IsEmpty()) {
        v8::Local<v8::Value> installedClasses = scriptState->perContextData()->compiledPrivateScript("PrivateScriptRunner");
        if (installedClasses.IsEmpty()) {
            installedClasses = installPrivateScriptRunner(isolate);
            scriptState->perContextData()->setCompiledPrivateScript("PrivateScriptRunner", installedClasses);
        }
        RELEASE_ASSERT(!installedClasses.IsEmpty());
        RELEASE_ASSERT(installedClasses->IsObject());

        installPrivateScript(isolate, className);
        compiledClass = v8::Local<v8::Object>::Cast(installedClasses)->Get(scriptState->context(), v8String(isolate, className)).ToLocalChecked();
        RELEASE_ASSERT(compiledClass->IsObject());
        scriptState->perContextData()->setCompiledPrivateScript(className, compiledClass);
    }
    return v8::Local<v8::Object>::Cast(compiledClass);
}

static void initializeHolderIfNeeded(ScriptState* scriptState, v8::Local<v8::Object> classObject, v8::Local<v8::Value> holder)
{
    RELEASE_ASSERT(!holder.IsEmpty());
    RELEASE_ASSERT(holder->IsObject());
    v8::Local<v8::Object> holderObject = v8::Local<v8::Object>::Cast(holder);
    v8::Isolate* isolate = scriptState->isolate();
    v8::Local<v8::Context> context = scriptState->context();
    v8::Local<v8::Value> isInitialized = V8HiddenValue::getHiddenValue(isolate, holderObject, V8HiddenValue::privateScriptObjectIsInitialized(isolate));
    if (isInitialized.IsEmpty()) {
        v8::TryCatch block(isolate);
        v8::Local<v8::Value> initializeFunction;
        if (classObject->Get(scriptState->context(), v8String(isolate, "initialize")).ToLocal(&initializeFunction) && initializeFunction->IsFunction()) {
            v8::TryCatch block(isolate);
            v8::Local<v8::Value> result;
            if (!V8ScriptRunner::callFunction(v8::Local<v8::Function>::Cast(initializeFunction), scriptState->executionContext(), holder, 0, 0, isolate).ToLocal(&result)) {
                fprintf(stderr, "Private script error: Object constructor threw an exception.\n");
                dumpV8Message(context, block.Message());
                RELEASE_ASSERT_NOT_REACHED();
            }
        }

        // Inject the prototype object of the private script into the prototype chain of the holder object.
        // This is necessary to let the holder object use properties defined on the prototype object
        // of the private script. (e.g., if the prototype object has |foo|, the holder object should be able
        // to use it with |this.foo|.)
        if (classObject->GetPrototype() != holderObject->GetPrototype()) {
            if (!v8CallBoolean(classObject->SetPrototype(context, holderObject->GetPrototype()))) {
                fprintf(stderr, "Private script error: SetPrototype failed.\n");
                dumpV8Message(context, block.Message());
                RELEASE_ASSERT_NOT_REACHED();
            }
        }
        if (!v8CallBoolean(holderObject->SetPrototype(context, classObject))) {
            fprintf(stderr, "Private script error: SetPrototype failed.\n");
            dumpV8Message(context, block.Message());
            RELEASE_ASSERT_NOT_REACHED();
        }

        isInitialized = v8Boolean(true, isolate);
        V8HiddenValue::setHiddenValue(isolate, holderObject, V8HiddenValue::privateScriptObjectIsInitialized(isolate), isInitialized);
    }
}

v8::Local<v8::Value> PrivateScriptRunner::installClassIfNeeded(Document* document, String className)
{
    v8::HandleScope handleScope(toIsolate(document));
    ScriptState* scriptState = ScriptState::forWorld(document->contextDocument()->frame(), DOMWrapperWorld::privateScriptIsolatedWorld());
    if (!scriptState->contextIsValid())
        return v8::Local<v8::Value>();

    ScriptState::Scope scope(scriptState);
    return classObjectOfPrivateScript(scriptState, className);
}

namespace {

void rethrowExceptionInPrivateScript(v8::Isolate* isolate, v8::TryCatch& block, ScriptState* scriptStateInUserScript, ExceptionState::Context errorContext, const char* propertyName, const char* interfaceName)
{
    v8::Local<v8::Context> context = scriptStateInUserScript->context();
    v8::Local<v8::Value> exception = block.Exception();
    RELEASE_ASSERT(!exception.IsEmpty() && exception->IsObject());

    v8::Local<v8::Object> exceptionObject = v8::Local<v8::Object>::Cast(exception);
    v8::Local<v8::Value> name = exceptionObject->Get(context, v8String(isolate, "name")).ToLocalChecked();
    RELEASE_ASSERT(name->IsString());

    v8::Local<v8::Message> tryCatchMessage = block.Message();
    v8::Local<v8::Value> message;
    String messageString;
    if (exceptionObject->Get(context, v8String(isolate, "message")).ToLocal(&message) && message->IsString())
        messageString = toCoreString(v8::Local<v8::String>::Cast(message));

    String exceptionName = toCoreString(v8::Local<v8::String>::Cast(name));
    if (exceptionName == "PrivateScriptException") {
        v8::Local<v8::Value> code = exceptionObject->Get(context, v8String(isolate, "code")).ToLocalChecked();
        RELEASE_ASSERT(code->IsInt32());
        int exceptionCode = code.As<v8::Int32>()->Value();
        ScriptState::Scope scope(scriptStateInUserScript);
        ExceptionState exceptionState(errorContext, propertyName, interfaceName, context->Global(), scriptStateInUserScript->isolate());
        exceptionState.throwDOMException(exceptionCode, messageString);
        exceptionState.throwIfNeeded();
        return;
    }

    // Standard JS errors thrown by a private script are treated as real errors
    // of the private script and crash the renderer, except for a stack overflow
    // error. A stack overflow error can happen in a valid private script
    // if user's script can create a recursion that involves the private script.
    if (exceptionName == "RangeError" && messageString.contains("Maximum call stack size exceeded")) {
        ScriptState::Scope scope(scriptStateInUserScript);
        ExceptionState exceptionState(errorContext, propertyName, interfaceName, scriptStateInUserScript->context()->Global(), scriptStateInUserScript->isolate());
        exceptionState.throwDOMException(V8RangeError, messageString);
        exceptionState.throwIfNeeded();
        return;
    }

    fprintf(stderr, "Private script error: %s was thrown.\n", exceptionName.utf8().data());
    dumpV8Message(context, tryCatchMessage);
    RELEASE_ASSERT_NOT_REACHED();
}

} // namespace

v8::Local<v8::Value> PrivateScriptRunner::runDOMAttributeGetter(ScriptState* scriptState, ScriptState* scriptStateInUserScript, const char* className, const char* attributeName, v8::Local<v8::Value> holder)
{
    v8::Isolate* isolate = scriptState->isolate();
    v8::Local<v8::Object> classObject = classObjectOfPrivateScript(scriptState, className);
    v8::Local<v8::Value> descriptor;
    if (!classObject->GetOwnPropertyDescriptor(scriptState->context(), v8String(isolate, attributeName)).ToLocal(&descriptor) || !descriptor->IsObject()) {
        fprintf(stderr, "Private script error: Target DOM attribute getter was not found. (Class name = %s, Attribute name = %s)\n", className, attributeName);
        RELEASE_ASSERT_NOT_REACHED();
    }
    v8::Local<v8::Value> getter;
    if (!v8::Local<v8::Object>::Cast(descriptor)->Get(scriptState->context(), v8String(isolate, "get")).ToLocal(&getter) || !getter->IsFunction()) {
        fprintf(stderr, "Private script error: Target DOM attribute getter was not found. (Class name = %s, Attribute name = %s)\n", className, attributeName);
        RELEASE_ASSERT_NOT_REACHED();
    }
    initializeHolderIfNeeded(scriptState, classObject, holder);

    v8::TryCatch block(isolate);
    v8::Local<v8::Value> result;
    if (!V8ScriptRunner::callFunction(v8::Local<v8::Function>::Cast(getter), scriptState->executionContext(), holder, 0, 0, isolate).ToLocal(&result)) {
        rethrowExceptionInPrivateScript(isolate, block, scriptStateInUserScript, ExceptionState::GetterContext, attributeName, className);
        block.ReThrow();
        return v8::Local<v8::Value>();
    }
    return result;
}

bool PrivateScriptRunner::runDOMAttributeSetter(ScriptState* scriptState, ScriptState* scriptStateInUserScript, const char* className, const char* attributeName, v8::Local<v8::Value> holder, v8::Local<v8::Value> v8Value)
{
    v8::Isolate* isolate = scriptState->isolate();
    v8::Local<v8::Object> classObject = classObjectOfPrivateScript(scriptState, className);
    v8::Local<v8::Value> descriptor;
    if (!classObject->GetOwnPropertyDescriptor(scriptState->context(), v8String(isolate, attributeName)).ToLocal(&descriptor) || !descriptor->IsObject()) {
        fprintf(stderr, "Private script error: Target DOM attribute setter was not found. (Class name = %s, Attribute name = %s)\n", className, attributeName);
        RELEASE_ASSERT_NOT_REACHED();
    }
    v8::Local<v8::Value> setter;
    if (!v8::Local<v8::Object>::Cast(descriptor)->Get(scriptState->context(), v8String(isolate, "set")).ToLocal(&setter) || !setter->IsFunction()) {
        fprintf(stderr, "Private script error: Target DOM attribute setter was not found. (Class name = %s, Attribute name = %s)\n", className, attributeName);
        RELEASE_ASSERT_NOT_REACHED();
    }
    initializeHolderIfNeeded(scriptState, classObject, holder);
    v8::Local<v8::Value> argv[] = { v8Value };
    v8::TryCatch block(isolate);
    v8::Local<v8::Value> result;
    if (!V8ScriptRunner::callFunction(v8::Local<v8::Function>::Cast(setter), scriptState->executionContext(), holder, WTF_ARRAY_LENGTH(argv), argv, isolate).ToLocal(&result)) {
        rethrowExceptionInPrivateScript(isolate, block, scriptStateInUserScript, ExceptionState::SetterContext, attributeName, className);
        block.ReThrow();
        return false;
    }
    return true;
}

v8::Local<v8::Value> PrivateScriptRunner::runDOMMethod(ScriptState* scriptState, ScriptState* scriptStateInUserScript, const char* className, const char* methodName, v8::Local<v8::Value> holder, int argc, v8::Local<v8::Value> argv[])
{
    v8::Local<v8::Object> classObject = classObjectOfPrivateScript(scriptState, className);
    v8::Local<v8::Value> method;
    if (!classObject->Get(scriptState->context(), v8String(scriptState->isolate(), methodName)).ToLocal(&method) || !method->IsFunction()) {
        fprintf(stderr, "Private script error: Target DOM method was not found. (Class name = %s, Method name = %s)\n", className, methodName);
        RELEASE_ASSERT_NOT_REACHED();
    }
    initializeHolderIfNeeded(scriptState, classObject, holder);
    v8::TryCatch block(scriptState->isolate());
    v8::Local<v8::Value> result;
    if (!V8ScriptRunner::callFunction(v8::Local<v8::Function>::Cast(method), scriptState->executionContext(), holder, argc, argv, scriptState->isolate()).ToLocal(&result)) {
        rethrowExceptionInPrivateScript(scriptState->isolate(), block, scriptStateInUserScript, ExceptionState::ExecutionContext, methodName, className);
        block.ReThrow();
        return v8::Local<v8::Value>();
    }
    return result;
}

} // namespace blink
