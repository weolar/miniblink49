// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "bindings/core/v8/inspector/V8InjectedScriptHost.h"

#include "bindings/core/v8/BindingSecurity.h"
#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/ScriptValue.h"
#include "bindings/core/v8/V8AbstractEventListener.h"
#include "bindings/core/v8/V8Binding.h"
#include "bindings/core/v8/V8DOMException.h"
#include "bindings/core/v8/V8DOMTokenList.h"
#include "bindings/core/v8/V8Event.h"
#include "bindings/core/v8/V8EventTarget.h"
#include "bindings/core/v8/V8HTMLAllCollection.h"
#include "bindings/core/v8/V8HTMLCollection.h"
#include "bindings/core/v8/V8Node.h"
#include "bindings/core/v8/V8NodeList.h"
#include "bindings/core/v8/V8ScriptRunner.h"
#include "bindings/core/v8/inspector/InspectorWrapper.h"
#include "core/events/EventTarget.h"
#include "core/inspector/EventListenerInfo.h"
#include "core/inspector/InjectedScript.h"
#include "core/inspector/InjectedScriptHost.h"
#include "core/inspector/JavaScriptCallFrame.h"
#include "core/inspector/V8Debugger.h"
#include "platform/JSONValues.h"
#include "wtf/RefPtr.h"
#include "wtf/StdLibExtras.h"
#include <algorithm>

namespace blink {

Node* InjectedScriptHost::scriptValueAsNode(ScriptState* scriptState, ScriptValue value)
{
    ScriptState::Scope scope(scriptState);
    if (!value.isObject() || value.isNull())
        return 0;
    return V8Node::toImpl(v8::Local<v8::Object>::Cast(value.v8Value()));
}

ScriptValue InjectedScriptHost::nodeAsScriptValue(ScriptState* scriptState, Node* node)
{
    ScriptState::Scope scope(scriptState);
    v8::Isolate* isolate = scriptState->isolate();
    ExceptionState exceptionState(ExceptionState::ExecutionContext, "nodeAsScriptValue", "InjectedScriptHost", scriptState->context()->Global(), isolate);
    if (!BindingSecurity::shouldAllowAccessToNode(isolate, node, exceptionState))
        return ScriptValue(scriptState, v8::Null(isolate));
    return ScriptValue(scriptState, toV8(node, scriptState->context()->Global(), isolate));
}

static EventTarget* eventTargetFromScriptValue(v8::Isolate* isolate, v8::Local<v8::Value> value)
{
    EventTarget* target = V8EventTarget::toImplWithTypeCheck(isolate, value);
    // We need to handle LocalDOMWindow specially, because LocalDOMWindow wrapper exists on prototype chain.
    if (!target)
        target = toDOMWindow(isolate, value);
    if (!target || !target->executionContext())
        return nullptr;
    return target;
}

EventTarget* InjectedScriptHost::scriptValueAsEventTarget(ScriptState* scriptState, ScriptValue value)
{
    ScriptState::Scope scope(scriptState);
    if (value.isNull() || !value.isObject())
        return nullptr;
    return eventTargetFromScriptValue(scriptState->isolate(), value.v8Value());
}

void V8InjectedScriptHost::clearConsoleMessagesCallback(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    InjectedScriptHost* impl = V8InjectedScriptHost::unwrap(info.Holder());
    impl->clearConsoleMessages();
}

void V8InjectedScriptHost::inspectedObjectCallback(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    if (info.Length() < 1)
        return;

    if (!info[0]->IsInt32()) {
        V8ThrowException::throwTypeError(info.GetIsolate(), "argument has to be an integer");
        return;
    }

    InjectedScriptHost* host = V8InjectedScriptHost::unwrap(info.Holder());
    InjectedScriptHost::InspectableObject* object = host->inspectedObject(info[0].As<v8::Int32>()->Value());
    v8SetReturnValue(info, object->get(ScriptState::current(info.GetIsolate())).v8Value());
}

static v8::Local<v8::String> functionDisplayName(v8::Local<v8::Function> function)
{
    v8::Local<v8::Value> value = function->GetDisplayName();
    if (value->IsString() && v8::Local<v8::String>::Cast(value)->Length())
        return v8::Local<v8::String>::Cast(value);

    value = function->GetName();
    if (value->IsString() && v8::Local<v8::String>::Cast(value)->Length())
        return v8::Local<v8::String>::Cast(value);

    value = function->GetInferredName();
    if (value->IsString() && v8::Local<v8::String>::Cast(value)->Length())
        return v8::Local<v8::String>::Cast(value);

    return v8::Local<v8::String>();
}

void V8InjectedScriptHost::internalConstructorNameCallback(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    if (info.Length() < 1 || !info[0]->IsObject())
        return;

    v8::Local<v8::Object> object = info[0].As<v8::Object>();
    v8::Local<v8::String> result = object->GetConstructorName();
    v8::Isolate *isolae = info.GetIsolate();
    if (!result.IsEmpty() && toCoreStringWithUndefinedOrNullCheck(result) == "Object") {
        v8::Local<v8::String> constructorSymbol = v8AtomicString(info.GetIsolate(), "constructor");
        if (object->HasRealNamedProperty(isolae->GetCurrentContext(), constructorSymbol).FromJust() && !object->HasRealNamedCallbackProperty(isolae->GetCurrentContext(), constructorSymbol).FromJust()) {
            v8::TryCatch tryCatch(isolae);
            v8::Local<v8::Value> constructor = object->GetRealNamedProperty(isolae->GetCurrentContext(), constructorSymbol).FromMaybe(v8::Local<v8::Value>());

            if (!constructor.IsEmpty() && constructor->IsFunction()) {
                v8::Local<v8::String> constructorName = functionDisplayName(v8::Local<v8::Function>::Cast(constructor));
                if (!constructorName.IsEmpty() && !tryCatch.HasCaught())
                    result = constructorName;
            }
        }
        if (toCoreStringWithUndefinedOrNullCheck(result) == "Object" && object->IsFunction())
            result = v8AtomicString(info.GetIsolate(), "Function");
    }

    v8SetReturnValue(info, result);
}

void V8InjectedScriptHost::isDOMWrapperCallback(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    if (info.Length() < 1)
        return;

    v8SetReturnValue(info, V8DOMWrapper::isWrapper(info.GetIsolate(), info[0]));
}

void V8InjectedScriptHost::isHTMLAllCollectionCallback(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    if (info.Length() < 1)
        return;

    if (!info[0]->IsObject()) {
        v8SetReturnValue(info, false);
        return;
    }

    v8SetReturnValue(info, V8HTMLAllCollection::hasInstance(info[0], info.GetIsolate()));
}

void V8InjectedScriptHost::isTypedArrayCallback(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    if (info.Length() < 1)
        return;
    v8SetReturnValue(info, info[0]->IsTypedArray());
}

void V8InjectedScriptHost::subtypeCallback(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    if (info.Length() < 1)
        return;
    v8::Isolate* isolate = info.GetIsolate();

    v8::Local<v8::Value> value = info[0];
    if (value->IsArray() || value->IsTypedArray() || value->IsArgumentsObject()) {
        v8SetReturnValue(info, v8AtomicString(isolate, "array"));
        return;
    }
    if (value->IsDate()) {
        v8SetReturnValue(info, v8AtomicString(isolate, "date"));
        return;
    }
    if (value->IsRegExp()) {
        v8SetReturnValue(info, v8AtomicString(isolate, "regexp"));
        return;
    }
    if (value->IsMap() || value->IsWeakMap()) {
        v8SetReturnValue(info, v8AtomicString(isolate, "map"));
        return;
    }
    if (value->IsSet() || value->IsWeakSet()) {
        v8SetReturnValue(info, v8AtomicString(isolate, "set"));
        return;
    }
    if (value->IsMapIterator() || value->IsSetIterator()) {
        v8SetReturnValue(info, v8AtomicString(isolate, "iterator"));
        return;
    }
    if (value->IsGeneratorObject()) {
        v8SetReturnValue(info, v8AtomicString(isolate, "generator"));
        return;
    }
    if (V8Node::hasInstance(value, isolate)) {
        v8SetReturnValue(info, v8AtomicString(isolate, "node"));
        return;
    }
    if (V8NodeList::hasInstance(value, isolate)
        || V8DOMTokenList::hasInstance(value, isolate)
        || V8HTMLCollection::hasInstance(value, isolate)
        || V8HTMLAllCollection::hasInstance(value, isolate)) {
        v8SetReturnValue(info, v8AtomicString(isolate, "array"));
        return;
    }
    if (value->IsNativeError() || V8DOMException::hasInstance(value, isolate)) {
        v8SetReturnValue(info, v8AtomicString(isolate, "error"));
        return;
    }
}

void V8InjectedScriptHost::functionDetailsCallback(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    if (info.Length() < 1 || !info[0]->IsFunction())
        return;

    v8::Local<v8::Function> function = v8::Local<v8::Function>::Cast(info[0]);
    int lineNumber = function->GetScriptLineNumber();
    int columnNumber = function->GetScriptColumnNumber();

    v8::Isolate* isolate = info.GetIsolate();
    v8::Local<v8::Object> location = v8::Object::New(isolate);
    location->Set(v8AtomicString(isolate, "lineNumber"), v8::Integer::New(isolate, lineNumber));
    location->Set(v8AtomicString(isolate, "columnNumber"), v8::Integer::New(isolate, columnNumber));
    location->Set(v8AtomicString(isolate, "scriptId"), v8::Integer::New(isolate, function->ScriptId())->ToString(isolate));

    v8::Local<v8::Object> result = v8::Object::New(isolate);
    result->Set(v8AtomicString(isolate, "location"), location);

    v8::Local<v8::String> name = functionDisplayName(function);
    result->Set(v8AtomicString(isolate, "functionName"), name.IsEmpty() ? v8AtomicString(isolate, "") : name);

    result->Set(v8AtomicString(isolate, "isGenerator"), v8::Boolean::New(isolate, function->IsGeneratorFunction()));

    InjectedScriptHost* host = V8InjectedScriptHost::unwrap(info.Holder());
    V8Debugger& debugger = host->debugger();
    v8::Local<v8::Value> scopes = debugger.functionScopes(function);
    if (!scopes.IsEmpty() && scopes->IsArray())
        result->Set(v8AtomicString(isolate, "rawScopes"), scopes);

    v8SetReturnValue(info, result);
}

void V8InjectedScriptHost::generatorObjectDetailsCallback(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    if (info.Length() < 1 || !info[0]->IsObject())
        return;

    v8::Local<v8::Object> object = v8::Local<v8::Object>::Cast(info[0]);

    InjectedScriptHost* host = V8InjectedScriptHost::unwrap(info.Holder());
    V8Debugger& debugger = host->debugger();
    v8SetReturnValue(info, debugger.generatorObjectDetails(object));
}

void V8InjectedScriptHost::collectionEntriesCallback(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    if (info.Length() < 1 || !info[0]->IsObject())
        return;

    v8::Local<v8::Object> object = v8::Local<v8::Object>::Cast(info[0]);

    InjectedScriptHost* host = V8InjectedScriptHost::unwrap(info.Holder());
    V8Debugger& debugger = host->debugger();
    v8SetReturnValue(info, debugger.collectionEntries(object));
}

void V8InjectedScriptHost::getInternalPropertiesCallback(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    if (info.Length() < 1 || !info[0]->IsObject())
        return;

    v8::Local<v8::Object> object = v8::Local<v8::Object>::Cast(info[0]);
    v8::MaybeLocal<v8::Array> properties = v8::Debug::GetInternalProperties(info.GetIsolate(), object);
    v8SetReturnValue(info, properties);
}

static v8::Local<v8::Array> getJSListenerFunctions(v8::Isolate* isolate, ExecutionContext* executionContext, const EventListenerInfo& listenerInfo)
{
    v8::Local<v8::Array> result = v8::Array::New(isolate);
    size_t handlersCount = listenerInfo.eventListenerVector.size();
    for (size_t i = 0, outputIndex = 0; i < handlersCount; ++i) {
        RefPtr<EventListener> listener = listenerInfo.eventListenerVector[i].listener;
        if (listener->type() != EventListener::JSEventListenerType) {
            ASSERT_NOT_REACHED();
            continue;
        }
        V8AbstractEventListener* v8Listener = static_cast<V8AbstractEventListener*>(listener.get());
        v8::Local<v8::Context> context = toV8Context(executionContext, v8Listener->world());
        // Hide listeners from other contexts.
        if (context != isolate->GetCurrentContext())
            continue;
        v8::Local<v8::Object> function;
        {
            // getListenerObject() may cause JS in the event attribute to get compiled, potentially unsuccessfully.
            v8::TryCatch block(isolate);
            function = v8Listener->getListenerObject(executionContext);
            if (block.HasCaught())
                continue;
        }
        ASSERT(!function.IsEmpty());
        v8::Local<v8::Object> listenerEntry = v8::Object::New(isolate);
        listenerEntry->Set(v8AtomicString(isolate, "listener"), function);
        listenerEntry->Set(v8AtomicString(isolate, "useCapture"), v8::Boolean::New(isolate, listenerInfo.eventListenerVector[i].useCapture));
        result->Set(v8::Number::New(isolate, outputIndex++), listenerEntry);
    }
    return result;
}

void V8InjectedScriptHost::getEventListenersCallback(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    if (info.Length() < 1)
        return;

    EventTarget* target = eventTargetFromScriptValue(info.GetIsolate(), info[0]);
    if (!target)
        return;
    InjectedScriptHost* host = V8InjectedScriptHost::unwrap(info.Holder());
    Vector<EventListenerInfo> listenersArray;
    host->getEventListenersImpl(target, listenersArray);

    v8::Local<v8::Object> result = v8::Object::New(info.GetIsolate());
    for (size_t i = 0; i < listenersArray.size(); ++i) {
        v8::Local<v8::Array> listeners = getJSListenerFunctions(info.GetIsolate(), target->executionContext(), listenersArray[i]);
        if (!listeners->Length())
            continue;
        AtomicString eventType = listenersArray[i].eventType;
        result->Set(v8String(info.GetIsolate(), eventType), listeners);
    }

    v8SetReturnValue(info, result);
}

void V8InjectedScriptHost::inspectCallback(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    if (info.Length() < 2)
        return;

    InjectedScriptHost* host = V8InjectedScriptHost::unwrap(info.Holder());
    ScriptState* scriptState = ScriptState::current(info.GetIsolate());
    ScriptValue object(scriptState, info[0]);
    ScriptValue hints(scriptState, info[1]);
    host->inspectImpl(toJSONValue(object), toJSONValue(hints));
}

void V8InjectedScriptHost::evalCallback(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    if (info.Length() < 1) {
        isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate, "One argument expected.")));
        return;
    }

    v8::Local<v8::String> expression = info[0]->ToString(isolate);
    if (expression.IsEmpty()) {
        isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate, "The argument must be a string.")));
        return;
    }

    ASSERT(isolate->InContext());
    v8::TryCatch tryCatch(isolate);
    v8::Local<v8::Value> result;
    if (!v8Call(V8ScriptRunner::compileAndRunInternalScript(expression, info.GetIsolate()), result, tryCatch)) {
        v8SetReturnValue(info, tryCatch.ReThrow());
        return;
    }
    v8SetReturnValue(info, result);
}

static void setExceptionAsReturnValue(const v8::FunctionCallbackInfo<v8::Value>& info, v8::Local<v8::Object> returnValue, v8::TryCatch& tryCatch)
{
    v8::Isolate* isolate = info.GetIsolate();
    returnValue->Set(v8::String::NewFromUtf8(isolate, "result"), tryCatch.Exception());
    returnValue->Set(v8::String::NewFromUtf8(isolate, "exceptionDetails"), JavaScriptCallFrame::createExceptionDetails(isolate, tryCatch.Message()));
    v8SetReturnValue(info, returnValue);
}

void V8InjectedScriptHost::evaluateWithExceptionDetailsCallback(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    v8::Isolate* isolate = info.GetIsolate();
    if (info.Length() < 1) {
        isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate, "One argument expected.")));
        return;
    }

    v8::Local<v8::String> expression = info[0]->ToString(isolate);
    if (expression.IsEmpty()) {
        isolate->ThrowException(v8::Exception::Error(v8::String::NewFromUtf8(isolate, "The argument must be a string.")));
        return;
    }

    ASSERT(isolate->InContext());
    v8::Local<v8::Object> wrappedResult = v8::Object::New(isolate);
    if (wrappedResult.IsEmpty())
        return;

    v8::TryCatch tryCatch(isolate);
    v8::Local<v8::Script> script;
    v8::Local<v8::Value> result;
    if (!v8Call(V8ScriptRunner::compileScript(expression, String(), String(), TextPosition(), isolate), script, tryCatch)) {
        setExceptionAsReturnValue(info, wrappedResult, tryCatch);
        return;
    }
    if (!v8Call(V8ScriptRunner::runCompiledScript(isolate, script, currentExecutionContext(isolate)), result, tryCatch)) {
        setExceptionAsReturnValue(info, wrappedResult, tryCatch);
        return;
    }

    wrappedResult->Set(v8::String::NewFromUtf8(isolate, "result"), result);
    wrappedResult->Set(v8::String::NewFromUtf8(isolate, "exceptionDetails"), v8::Undefined(isolate));
    v8SetReturnValue(info, wrappedResult);
}

void V8InjectedScriptHost::setFunctionVariableValueCallback(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    if (info.Length() < 4 || !info[0]->IsFunction() || !info[1]->IsInt32() || !info[2]->IsString())
        return;

    v8::Local<v8::Value> functionValue = info[0];
    int scopeIndex = info[1].As<v8::Int32>()->Value();
    String variableName = toCoreStringWithUndefinedOrNullCheck(info[2]);
    v8::Local<v8::Value> newValue = info[3];

    InjectedScriptHost* host = V8InjectedScriptHost::unwrap(info.Holder());
    V8Debugger& debugger = host->debugger();
    v8SetReturnValue(info, debugger.setFunctionVariableValue(functionValue, scopeIndex, variableName, newValue));
}

static bool getFunctionLocation(const v8::FunctionCallbackInfo<v8::Value>& info, String* scriptId, int* lineNumber, int* columnNumber)
{
    if (info.Length() < 1 || !info[0]->IsFunction())
        return false;
    v8::Local<v8::Function> function = v8::Local<v8::Function>::Cast(info[0]);
    *lineNumber = function->GetScriptLineNumber();
    *columnNumber = function->GetScriptColumnNumber();
    if (*lineNumber == v8::Function::kLineOffsetNotFound || *columnNumber == v8::Function::kLineOffsetNotFound)
        return false;
    *scriptId = String::number(function->ScriptId());
    return true;
}

void V8InjectedScriptHost::debugFunctionCallback(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    String scriptId;
    int lineNumber;
    int columnNumber;
    if (!getFunctionLocation(info, &scriptId, &lineNumber, &columnNumber))
        return;

    InjectedScriptHost* host = V8InjectedScriptHost::unwrap(info.Holder());
    host->debugFunction(scriptId, lineNumber, columnNumber);
}

void V8InjectedScriptHost::undebugFunctionCallback(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    String scriptId;
    int lineNumber;
    int columnNumber;
    if (!getFunctionLocation(info, &scriptId, &lineNumber, &columnNumber))
        return;

    InjectedScriptHost* host = V8InjectedScriptHost::unwrap(info.Holder());
    host->undebugFunction(scriptId, lineNumber, columnNumber);
}

void V8InjectedScriptHost::monitorFunctionCallback(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    String scriptId;
    int lineNumber;
    int columnNumber;
    if (!getFunctionLocation(info, &scriptId, &lineNumber, &columnNumber))
        return;

    v8::Local<v8::Value> name;
    if (info.Length() > 0 && info[0]->IsFunction()) {
        v8::Local<v8::Function> function = v8::Local<v8::Function>::Cast(info[0]);
        name = function->GetName();
        if (!name->IsString() || !v8::Local<v8::String>::Cast(name)->Length())
            name = function->GetInferredName();
    }

    InjectedScriptHost* host = V8InjectedScriptHost::unwrap(info.Holder());
    host->monitorFunction(scriptId, lineNumber, columnNumber, toCoreStringWithUndefinedOrNullCheck(name));
}

void V8InjectedScriptHost::unmonitorFunctionCallback(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    String scriptId;
    int lineNumber;
    int columnNumber;
    if (!getFunctionLocation(info, &scriptId, &lineNumber, &columnNumber))
        return;

    InjectedScriptHost* host = V8InjectedScriptHost::unwrap(info.Holder());
    host->unmonitorFunction(scriptId, lineNumber, columnNumber);
}

void V8InjectedScriptHost::callFunctionCallback(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    if (info.Length() < 2 || info.Length() > 3 || !info[0]->IsFunction()) {
        ASSERT_NOT_REACHED();
        return;
    }

    v8::Local<v8::Function> function = v8::Local<v8::Function>::Cast(info[0]);
    v8::Local<v8::Value> receiver = info[1];

    if (info.Length() < 3 || info[2]->IsUndefined()) {
        v8::Local<v8::Value> result = function->Call(receiver, 0, 0);
        v8SetReturnValue(info, result);
        return;
    }

    if (!info[2]->IsArray()) {
        ASSERT_NOT_REACHED();
        return;
    }

    v8::Local<v8::Array> arguments = v8::Local<v8::Array>::Cast(info[2]);
    size_t argc = arguments->Length();
    OwnPtr<v8::Local<v8::Value>[]> argv = adoptArrayPtr(new v8::Local<v8::Value>[argc]);
    for (size_t i = 0; i < argc; ++i) {
        if (!arguments->Get(info.GetIsolate()->GetCurrentContext(), v8::Integer::New(info.GetIsolate(), i)).ToLocal(&argv[i]))
            return;
    }

    v8::Local<v8::Value> result = function->Call(receiver, argc, argv.get());
    v8SetReturnValue(info, result);
}

void V8InjectedScriptHost::suppressWarningsAndCallFunctionCallback(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    InjectedScriptHost* host = V8InjectedScriptHost::unwrap(info.Holder());
    host->client()->muteWarningsAndDeprecations();

    callFunctionCallback(info);

    host->client()->unmuteWarningsAndDeprecations();
}

void V8InjectedScriptHost::setNonEnumPropertyCallback(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    if (info.Length() < 3 || !info[0]->IsObject() || !info[1]->IsString())
        return;

    v8::Local<v8::Object> object = info[0].As<v8::Object>();
    // TODO(bashi): Use DefineOwnProperty() if possible.
    object->ForceSet(info.GetIsolate()->GetCurrentContext(), info[1], info[2], v8::DontEnum);
}

void V8InjectedScriptHost::bindCallback(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    if (info.Length() < 2 || !info[1]->IsString())
        return;
    InjectedScriptNative* injectedScriptNative = InjectedScriptNative::fromInjectedScriptHost(info.Holder());
    if (!injectedScriptNative)
        return;

    v8::Local<v8::String> v8groupName = info[1]->ToString(info.GetIsolate());
    String groupName = toCoreStringWithUndefinedOrNullCheck(v8groupName);
    int id = injectedScriptNative->bind(info[0], groupName);
    info.GetReturnValue().Set(id);
}

void V8InjectedScriptHost::objectForIdCallback(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    if (info.Length() < 1 || !info[0]->IsInt32())
        return;
    InjectedScriptNative* injectedScriptNative = InjectedScriptNative::fromInjectedScriptHost(info.Holder());
    if (!injectedScriptNative)
        return;
    int id = info[0].As<v8::Int32>()->Value();
    v8::Local<v8::Value> value = injectedScriptNative->objectForId(id);
    if (!value.IsEmpty())
        info.GetReturnValue().Set(value);
}

void V8InjectedScriptHost::idToObjectGroupNameCallback(const v8::FunctionCallbackInfo<v8::Value>& info)
{
    if (info.Length() < 1 || !info[0]->IsInt32())
        return;
    InjectedScriptNative* injectedScriptNative = InjectedScriptNative::fromInjectedScriptHost(info.Holder());
    if (!injectedScriptNative)
        return;
    int id = info[0].As<v8::Int32>()->Value();
    String groupName = injectedScriptNative->groupName(id);
    if (!groupName.isEmpty())
        info.GetReturnValue().Set(v8String(info.GetIsolate(), groupName));
}

namespace {

char hiddenPropertyName[] = "v8inspector::InjectedScriptHost";
char className[] = "V8InjectedScriptHost";
using InjectedScriptHostWrapper = InspectorWrapper<InjectedScriptHost, hiddenPropertyName, className>;

const InjectedScriptHostWrapper::V8MethodConfiguration V8InjectedScriptHostMethods[] = {
    {"clearConsoleMessages", V8InjectedScriptHost::clearConsoleMessagesCallback},
    {"inspect", V8InjectedScriptHost::inspectCallback},
    {"inspectedObject", V8InjectedScriptHost::inspectedObjectCallback},
    {"internalConstructorName", V8InjectedScriptHost::internalConstructorNameCallback},
    {"isDOMWrapper", V8InjectedScriptHost::isDOMWrapperCallback},
    {"isHTMLAllCollection", V8InjectedScriptHost::isHTMLAllCollectionCallback},
    {"isTypedArray", V8InjectedScriptHost::isTypedArrayCallback},
    {"subtype", V8InjectedScriptHost::subtypeCallback},
    {"functionDetails", V8InjectedScriptHost::functionDetailsCallback},
    {"generatorObjectDetails", V8InjectedScriptHost::generatorObjectDetailsCallback},
    {"collectionEntries", V8InjectedScriptHost::collectionEntriesCallback},
    {"getInternalProperties", V8InjectedScriptHost::getInternalPropertiesCallback},
    {"getEventListeners", V8InjectedScriptHost::getEventListenersCallback},
    {"eval", V8InjectedScriptHost::evalCallback},
    {"evaluateWithExceptionDetails", V8InjectedScriptHost::evaluateWithExceptionDetailsCallback},
    {"debugFunction", V8InjectedScriptHost::debugFunctionCallback},
    {"undebugFunction", V8InjectedScriptHost::undebugFunctionCallback},
    {"monitorFunction", V8InjectedScriptHost::monitorFunctionCallback},
    {"unmonitorFunction", V8InjectedScriptHost::unmonitorFunctionCallback},
    {"callFunction", V8InjectedScriptHost::callFunctionCallback},
    {"suppressWarningsAndCallFunction", V8InjectedScriptHost::suppressWarningsAndCallFunctionCallback},
    {"setNonEnumProperty", V8InjectedScriptHost::setNonEnumPropertyCallback},
    {"setFunctionVariableValue", V8InjectedScriptHost::setFunctionVariableValueCallback},
    {"bind", V8InjectedScriptHost::bindCallback},
    {"objectForId", V8InjectedScriptHost::objectForIdCallback},
    {"idToObjectGroupName", V8InjectedScriptHost::idToObjectGroupNameCallback},
};

} // namespace

v8::Local<v8::FunctionTemplate> V8InjectedScriptHost::createWrapperTemplate(v8::Isolate* isolate)
{
    Vector<InspectorWrapperBase::V8MethodConfiguration> methods(WTF_ARRAY_LENGTH(V8InjectedScriptHostMethods));
    std::copy(V8InjectedScriptHostMethods, V8InjectedScriptHostMethods + WTF_ARRAY_LENGTH(V8InjectedScriptHostMethods), methods.begin());
    Vector<InspectorWrapperBase::V8AttributeConfiguration> attributes;
    return InjectedScriptHostWrapper::createWrapperTemplate(isolate, methods, attributes);
}

v8::Local<v8::Object> V8InjectedScriptHost::wrap(v8::Local<v8::FunctionTemplate> constructorTemplate, v8::Local<v8::Context> context, PassRefPtrWillBeRawPtr<InjectedScriptHost> host)
{
    return InjectedScriptHostWrapper::wrap(constructorTemplate, context, host);
}

InjectedScriptHost* V8InjectedScriptHost::unwrap(v8::Local<v8::Object> object)
{
    return InjectedScriptHostWrapper::unwrap(object);
}

} // namespace blink
