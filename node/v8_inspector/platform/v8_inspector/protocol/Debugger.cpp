// This file is generated

// Copyright (c) 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "platform\v8_inspector\protocol/Debugger.h"

#include "platform/inspector_protocol/DispatcherBase.h"
#include "platform/inspector_protocol/Parser.h"

namespace blink {
namespace protocol {
namespace Debugger {

// ------------- Enum values from types.

const char Metainfo::domainName[] = "Debugger";
const char Metainfo::commandPrefix[] = "Debugger.";

std::unique_ptr<Location> Location::parse(protocol::Value* value, ErrorSupport* errors)
{
    if (!value || value->type() != protocol::Value::TypeObject) {
        errors->addError("object expected");
        return nullptr;
    }

    std::unique_ptr<Location> result(new Location());
    protocol::DictionaryValue* object = DictionaryValue::cast(value);
    errors->push();
    protocol::Value* scriptIdValue = object->get("scriptId");
    errors->setName("scriptId");
    result->m_scriptId = ValueConversions<String16>::parse(scriptIdValue, errors);
    protocol::Value* lineNumberValue = object->get("lineNumber");
    errors->setName("lineNumber");
    result->m_lineNumber = ValueConversions<int>::parse(lineNumberValue, errors);
    protocol::Value* columnNumberValue = object->get("columnNumber");
    if (columnNumberValue) {
        errors->setName("columnNumber");
        result->m_columnNumber = ValueConversions<int>::parse(columnNumberValue, errors);
    }
    errors->pop();
    if (errors->hasErrors())
        return nullptr;
    return result;
}

std::unique_ptr<protocol::DictionaryValue> Location::serialize() const
{
    std::unique_ptr<protocol::DictionaryValue> result = DictionaryValue::create();
    result->setValue("scriptId", ValueConversions<String16>::serialize(m_scriptId));
    result->setValue("lineNumber", ValueConversions<int>::serialize(m_lineNumber));
    if (m_columnNumber.isJust())
        result->setValue("columnNumber", ValueConversions<int>::serialize(m_columnNumber.fromJust()));
    return result;
}

std::unique_ptr<Location> Location::clone() const
{
    ErrorSupport errors;
    return parse(serialize().get(), &errors);
}

std::unique_ptr<ScriptPosition> ScriptPosition::parse(protocol::Value* value, ErrorSupport* errors)
{
    if (!value || value->type() != protocol::Value::TypeObject) {
        errors->addError("object expected");
        return nullptr;
    }

    std::unique_ptr<ScriptPosition> result(new ScriptPosition());
    protocol::DictionaryValue* object = DictionaryValue::cast(value);
    errors->push();
    protocol::Value* lineNumberValue = object->get("lineNumber");
    errors->setName("lineNumber");
    result->m_lineNumber = ValueConversions<int>::parse(lineNumberValue, errors);
    protocol::Value* columnNumberValue = object->get("columnNumber");
    errors->setName("columnNumber");
    result->m_columnNumber = ValueConversions<int>::parse(columnNumberValue, errors);
    errors->pop();
    if (errors->hasErrors())
        return nullptr;
    return result;
}

std::unique_ptr<protocol::DictionaryValue> ScriptPosition::serialize() const
{
    std::unique_ptr<protocol::DictionaryValue> result = DictionaryValue::create();
    result->setValue("lineNumber", ValueConversions<int>::serialize(m_lineNumber));
    result->setValue("columnNumber", ValueConversions<int>::serialize(m_columnNumber));
    return result;
}

std::unique_ptr<ScriptPosition> ScriptPosition::clone() const
{
    ErrorSupport errors;
    return parse(serialize().get(), &errors);
}

std::unique_ptr<CallFrame> CallFrame::parse(protocol::Value* value, ErrorSupport* errors)
{
    if (!value || value->type() != protocol::Value::TypeObject) {
        errors->addError("object expected");
        return nullptr;
    }

    std::unique_ptr<CallFrame> result(new CallFrame());
    protocol::DictionaryValue* object = DictionaryValue::cast(value);
    errors->push();
    protocol::Value* callFrameIdValue = object->get("callFrameId");
    errors->setName("callFrameId");
    result->m_callFrameId = ValueConversions<String16>::parse(callFrameIdValue, errors);
    protocol::Value* functionNameValue = object->get("functionName");
    errors->setName("functionName");
    result->m_functionName = ValueConversions<String16>::parse(functionNameValue, errors);
    protocol::Value* functionLocationValue = object->get("functionLocation");
    if (functionLocationValue) {
        errors->setName("functionLocation");
        result->m_functionLocation = ValueConversions<protocol::Debugger::Location>::parse(functionLocationValue, errors);
    }
    protocol::Value* locationValue = object->get("location");
    errors->setName("location");
    result->m_location = ValueConversions<protocol::Debugger::Location>::parse(locationValue, errors);
    protocol::Value* scopeChainValue = object->get("scopeChain");
    errors->setName("scopeChain");
    result->m_scopeChain = ValueConversions<protocol::Array<protocol::Debugger::Scope>>::parse(scopeChainValue, errors);
    protocol::Value* thisValue = object->get("this");
    errors->setName("this");
    result->m_this = ValueConversions<protocol::Runtime::RemoteObject>::parse(thisValue, errors);
    protocol::Value* returnValueValue = object->get("returnValue");
    if (returnValueValue) {
        errors->setName("returnValue");
        result->m_returnValue = ValueConversions<protocol::Runtime::RemoteObject>::parse(returnValueValue, errors);
    }
    errors->pop();
    if (errors->hasErrors())
        return nullptr;
    return result;
}

std::unique_ptr<protocol::DictionaryValue> CallFrame::serialize() const
{
    std::unique_ptr<protocol::DictionaryValue> result = DictionaryValue::create();
    result->setValue("callFrameId", ValueConversions<String16>::serialize(m_callFrameId));
    result->setValue("functionName", ValueConversions<String16>::serialize(m_functionName));
    if (m_functionLocation.isJust())
        result->setValue("functionLocation", ValueConversions<protocol::Debugger::Location>::serialize(m_functionLocation.fromJust()));
    result->setValue("location", ValueConversions<protocol::Debugger::Location>::serialize(m_location.get()));
    result->setValue("scopeChain", ValueConversions<protocol::Array<protocol::Debugger::Scope>>::serialize(m_scopeChain.get()));
    result->setValue("this", ValueConversions<protocol::Runtime::RemoteObject>::serialize(m_this.get()));
    if (m_returnValue.isJust())
        result->setValue("returnValue", ValueConversions<protocol::Runtime::RemoteObject>::serialize(m_returnValue.fromJust()));
    return result;
}

std::unique_ptr<CallFrame> CallFrame::clone() const
{
    ErrorSupport errors;
    return parse(serialize().get(), &errors);
}

const char* Scope::TypeEnum::Global = "global";
const char* Scope::TypeEnum::Local = "local";
const char* Scope::TypeEnum::With = "with";
const char* Scope::TypeEnum::Closure = "closure";
const char* Scope::TypeEnum::Catch = "catch";
const char* Scope::TypeEnum::Block = "block";
const char* Scope::TypeEnum::Script = "script";

std::unique_ptr<Scope> Scope::parse(protocol::Value* value, ErrorSupport* errors)
{
    if (!value || value->type() != protocol::Value::TypeObject) {
        errors->addError("object expected");
        return nullptr;
    }

    std::unique_ptr<Scope> result(new Scope());
    protocol::DictionaryValue* object = DictionaryValue::cast(value);
    errors->push();
    protocol::Value* typeValue = object->get("type");
    errors->setName("type");
    result->m_type = ValueConversions<String16>::parse(typeValue, errors);
    protocol::Value* objectValue = object->get("object");
    errors->setName("object");
    result->m_object = ValueConversions<protocol::Runtime::RemoteObject>::parse(objectValue, errors);
    protocol::Value* nameValue = object->get("name");
    if (nameValue) {
        errors->setName("name");
        result->m_name = ValueConversions<String16>::parse(nameValue, errors);
    }
    protocol::Value* startLocationValue = object->get("startLocation");
    if (startLocationValue) {
        errors->setName("startLocation");
        result->m_startLocation = ValueConversions<protocol::Debugger::Location>::parse(startLocationValue, errors);
    }
    protocol::Value* endLocationValue = object->get("endLocation");
    if (endLocationValue) {
        errors->setName("endLocation");
        result->m_endLocation = ValueConversions<protocol::Debugger::Location>::parse(endLocationValue, errors);
    }
    errors->pop();
    if (errors->hasErrors())
        return nullptr;
    return result;
}

std::unique_ptr<protocol::DictionaryValue> Scope::serialize() const
{
    std::unique_ptr<protocol::DictionaryValue> result = DictionaryValue::create();
    result->setValue("type", ValueConversions<String16>::serialize(m_type));
    result->setValue("object", ValueConversions<protocol::Runtime::RemoteObject>::serialize(m_object.get()));
    if (m_name.isJust())
        result->setValue("name", ValueConversions<String16>::serialize(m_name.fromJust()));
    if (m_startLocation.isJust())
        result->setValue("startLocation", ValueConversions<protocol::Debugger::Location>::serialize(m_startLocation.fromJust()));
    if (m_endLocation.isJust())
        result->setValue("endLocation", ValueConversions<protocol::Debugger::Location>::serialize(m_endLocation.fromJust()));
    return result;
}

std::unique_ptr<Scope> Scope::clone() const
{
    ErrorSupport errors;
    return parse(serialize().get(), &errors);
}

std::unique_ptr<SearchMatch> SearchMatch::parse(protocol::Value* value, ErrorSupport* errors)
{
    if (!value || value->type() != protocol::Value::TypeObject) {
        errors->addError("object expected");
        return nullptr;
    }

    std::unique_ptr<SearchMatch> result(new SearchMatch());
    protocol::DictionaryValue* object = DictionaryValue::cast(value);
    errors->push();
    protocol::Value* lineNumberValue = object->get("lineNumber");
    errors->setName("lineNumber");
    result->m_lineNumber = ValueConversions<double>::parse(lineNumberValue, errors);
    protocol::Value* lineContentValue = object->get("lineContent");
    errors->setName("lineContent");
    result->m_lineContent = ValueConversions<String16>::parse(lineContentValue, errors);
    errors->pop();
    if (errors->hasErrors())
        return nullptr;
    return result;
}

std::unique_ptr<protocol::DictionaryValue> SearchMatch::serialize() const
{
    std::unique_ptr<protocol::DictionaryValue> result = DictionaryValue::create();
    result->setValue("lineNumber", ValueConversions<double>::serialize(m_lineNumber));
    result->setValue("lineContent", ValueConversions<String16>::serialize(m_lineContent));
    return result;
}

std::unique_ptr<SearchMatch> SearchMatch::clone() const
{
    ErrorSupport errors;
    return parse(serialize().get(), &errors);
}

String16 SearchMatch::toJSONString() const
{
    return serialize()->toJSONString();
}

// static
std::unique_ptr<API::SearchMatch> API::SearchMatch::fromJSONString(const String16& json)
{
    ErrorSupport errors;
    std::unique_ptr<Value> value = parseJSON(json);
    if (!value)
        return nullptr;
    return protocol::Debugger::SearchMatch::parse(value.get(), &errors);
}

// ------------- Enum values from params.


namespace SetPauseOnExceptions {
namespace StateEnum {
const char* None = "none";
const char* Uncaught = "uncaught";
const char* All = "all";
} // namespace StateEnum
} // namespace SetPauseOnExceptions

namespace Paused {
namespace ReasonEnum {
const char* XHR = "XHR";
const char* DOM = "DOM";
const char* EventListener = "EventListener";
const char* Exception = "exception";
const char* Assert = "assert";
const char* DebugCommand = "debugCommand";
const char* PromiseRejection = "promiseRejection";
const char* Other = "other";
} // namespace ReasonEnum
} // namespace Paused

namespace API {
namespace Paused {
namespace ReasonEnum {
const char* XHR = "XHR";
const char* DOM = "DOM";
const char* EventListener = "EventListener";
const char* Exception = "exception";
const char* Assert = "assert";
const char* DebugCommand = "debugCommand";
const char* PromiseRejection = "promiseRejection";
const char* Other = "other";
} // namespace ReasonEnum
} // namespace Paused
} // namespace API

// ------------- Frontend notifications.

void Frontend::scriptParsed(const String16& scriptId, const String16& url, int startLine, int startColumn, int endLine, int endColumn, int executionContextId, const String16& hash, const Maybe<protocol::DictionaryValue>& executionContextAuxData, const Maybe<bool>& isInternalScript, const Maybe<bool>& isLiveEdit, const Maybe<String16>& sourceMapURL, const Maybe<bool>& hasSourceURL, const Maybe<bool>& deprecatedCommentWasUsed)
{
    std::unique_ptr<protocol::DictionaryValue> jsonMessage = DictionaryValue::create();
    jsonMessage->setString("method", "Debugger.scriptParsed");
    std::unique_ptr<protocol::DictionaryValue> paramsObject = DictionaryValue::create();
    paramsObject->setValue("scriptId", ValueConversions<String16>::serialize(scriptId));
    paramsObject->setValue("url", ValueConversions<String16>::serialize(url));
    paramsObject->setValue("startLine", ValueConversions<int>::serialize(startLine));
    paramsObject->setValue("startColumn", ValueConversions<int>::serialize(startColumn));
    paramsObject->setValue("endLine", ValueConversions<int>::serialize(endLine));
    paramsObject->setValue("endColumn", ValueConversions<int>::serialize(endColumn));
    paramsObject->setValue("executionContextId", ValueConversions<int>::serialize(executionContextId));
    paramsObject->setValue("hash", ValueConversions<String16>::serialize(hash));
    if (executionContextAuxData.isJust())
        paramsObject->setValue("executionContextAuxData", ValueConversions<protocol::DictionaryValue>::serialize(executionContextAuxData.fromJust()));
    if (isInternalScript.isJust())
        paramsObject->setValue("isInternalScript", ValueConversions<bool>::serialize(isInternalScript.fromJust()));
    if (isLiveEdit.isJust())
        paramsObject->setValue("isLiveEdit", ValueConversions<bool>::serialize(isLiveEdit.fromJust()));
    if (sourceMapURL.isJust())
        paramsObject->setValue("sourceMapURL", ValueConversions<String16>::serialize(sourceMapURL.fromJust()));
    if (hasSourceURL.isJust())
        paramsObject->setValue("hasSourceURL", ValueConversions<bool>::serialize(hasSourceURL.fromJust()));
    if (deprecatedCommentWasUsed.isJust())
        paramsObject->setValue("deprecatedCommentWasUsed", ValueConversions<bool>::serialize(deprecatedCommentWasUsed.fromJust()));
    jsonMessage->setObject("params", std::move(paramsObject));
    if (m_frontendChannel)
        m_frontendChannel->sendProtocolNotification(jsonMessage->toJSONString());
}

void Frontend::scriptFailedToParse(const String16& scriptId, const String16& url, int startLine, int startColumn, int endLine, int endColumn, int executionContextId, const String16& hash, const Maybe<protocol::DictionaryValue>& executionContextAuxData, const Maybe<bool>& isInternalScript, const Maybe<String16>& sourceMapURL, const Maybe<bool>& hasSourceURL, const Maybe<bool>& deprecatedCommentWasUsed)
{
    std::unique_ptr<protocol::DictionaryValue> jsonMessage = DictionaryValue::create();
    jsonMessage->setString("method", "Debugger.scriptFailedToParse");
    std::unique_ptr<protocol::DictionaryValue> paramsObject = DictionaryValue::create();
    paramsObject->setValue("scriptId", ValueConversions<String16>::serialize(scriptId));
    paramsObject->setValue("url", ValueConversions<String16>::serialize(url));
    paramsObject->setValue("startLine", ValueConversions<int>::serialize(startLine));
    paramsObject->setValue("startColumn", ValueConversions<int>::serialize(startColumn));
    paramsObject->setValue("endLine", ValueConversions<int>::serialize(endLine));
    paramsObject->setValue("endColumn", ValueConversions<int>::serialize(endColumn));
    paramsObject->setValue("executionContextId", ValueConversions<int>::serialize(executionContextId));
    paramsObject->setValue("hash", ValueConversions<String16>::serialize(hash));
    if (executionContextAuxData.isJust())
        paramsObject->setValue("executionContextAuxData", ValueConversions<protocol::DictionaryValue>::serialize(executionContextAuxData.fromJust()));
    if (isInternalScript.isJust())
        paramsObject->setValue("isInternalScript", ValueConversions<bool>::serialize(isInternalScript.fromJust()));
    if (sourceMapURL.isJust())
        paramsObject->setValue("sourceMapURL", ValueConversions<String16>::serialize(sourceMapURL.fromJust()));
    if (hasSourceURL.isJust())
        paramsObject->setValue("hasSourceURL", ValueConversions<bool>::serialize(hasSourceURL.fromJust()));
    if (deprecatedCommentWasUsed.isJust())
        paramsObject->setValue("deprecatedCommentWasUsed", ValueConversions<bool>::serialize(deprecatedCommentWasUsed.fromJust()));
    jsonMessage->setObject("params", std::move(paramsObject));
    if (m_frontendChannel)
        m_frontendChannel->sendProtocolNotification(jsonMessage->toJSONString());
}

void Frontend::breakpointResolved(const String16& breakpointId, std::unique_ptr<protocol::Debugger::Location> location)
{
    std::unique_ptr<protocol::DictionaryValue> jsonMessage = DictionaryValue::create();
    jsonMessage->setString("method", "Debugger.breakpointResolved");
    std::unique_ptr<protocol::DictionaryValue> paramsObject = DictionaryValue::create();
    paramsObject->setValue("breakpointId", ValueConversions<String16>::serialize(breakpointId));
    paramsObject->setValue("location", ValueConversions<protocol::Debugger::Location>::serialize(location.get()));
    jsonMessage->setObject("params", std::move(paramsObject));
    if (m_frontendChannel)
        m_frontendChannel->sendProtocolNotification(jsonMessage->toJSONString());
}

void Frontend::paused(std::unique_ptr<protocol::Array<protocol::Debugger::CallFrame>> callFrames, const String16& reason, const Maybe<protocol::DictionaryValue>& data, const Maybe<protocol::Array<String16>>& hitBreakpoints, const Maybe<protocol::Runtime::StackTrace>& asyncStackTrace)
{
    std::unique_ptr<protocol::DictionaryValue> jsonMessage = DictionaryValue::create();
    jsonMessage->setString("method", "Debugger.paused");
    std::unique_ptr<protocol::DictionaryValue> paramsObject = DictionaryValue::create();
    paramsObject->setValue("callFrames", ValueConversions<protocol::Array<protocol::Debugger::CallFrame>>::serialize(callFrames.get()));
    paramsObject->setValue("reason", ValueConversions<String16>::serialize(reason));
    if (data.isJust())
        paramsObject->setValue("data", ValueConversions<protocol::DictionaryValue>::serialize(data.fromJust()));
    if (hitBreakpoints.isJust())
        paramsObject->setValue("hitBreakpoints", ValueConversions<protocol::Array<String16>>::serialize(hitBreakpoints.fromJust()));
    if (asyncStackTrace.isJust())
        paramsObject->setValue("asyncStackTrace", ValueConversions<protocol::Runtime::StackTrace>::serialize(asyncStackTrace.fromJust()));
    jsonMessage->setObject("params", std::move(paramsObject));
    if (m_frontendChannel)
        m_frontendChannel->sendProtocolNotification(jsonMessage->toJSONString());
}

void Frontend::resumed()
{
    std::unique_ptr<protocol::DictionaryValue> jsonMessage = DictionaryValue::create();
    jsonMessage->setString("method", "Debugger.resumed");
    std::unique_ptr<protocol::DictionaryValue> paramsObject = DictionaryValue::create();
    jsonMessage->setObject("params", std::move(paramsObject));
    if (m_frontendChannel)
        m_frontendChannel->sendProtocolNotification(jsonMessage->toJSONString());
}

// --------------------- Dispatcher.

class DispatcherImpl : public protocol::DispatcherBase {
public:
    DispatcherImpl(FrontendChannel* frontendChannel, Backend* backend)
        : DispatcherBase(frontendChannel)
        , m_backend(backend) {
        m_dispatchMap["Debugger.enable"] = &DispatcherImpl::enable;
        m_dispatchMap["Debugger.disable"] = &DispatcherImpl::disable;
        m_dispatchMap["Debugger.setBreakpointsActive"] = &DispatcherImpl::setBreakpointsActive;
        m_dispatchMap["Debugger.setSkipAllPauses"] = &DispatcherImpl::setSkipAllPauses;
        m_dispatchMap["Debugger.setBreakpointByUrl"] = &DispatcherImpl::setBreakpointByUrl;
        m_dispatchMap["Debugger.setBreakpoint"] = &DispatcherImpl::setBreakpoint;
        m_dispatchMap["Debugger.removeBreakpoint"] = &DispatcherImpl::removeBreakpoint;
        m_dispatchMap["Debugger.continueToLocation"] = &DispatcherImpl::continueToLocation;
        m_dispatchMap["Debugger.stepOver"] = &DispatcherImpl::stepOver;
        m_dispatchMap["Debugger.stepInto"] = &DispatcherImpl::stepInto;
        m_dispatchMap["Debugger.stepOut"] = &DispatcherImpl::stepOut;
        m_dispatchMap["Debugger.pause"] = &DispatcherImpl::pause;
        m_dispatchMap["Debugger.resume"] = &DispatcherImpl::resume;
        m_dispatchMap["Debugger.searchInContent"] = &DispatcherImpl::searchInContent;
        m_dispatchMap["Debugger.canSetScriptSource"] = &DispatcherImpl::canSetScriptSource;
        m_dispatchMap["Debugger.setScriptSource"] = &DispatcherImpl::setScriptSource;
        m_dispatchMap["Debugger.restartFrame"] = &DispatcherImpl::restartFrame;
        m_dispatchMap["Debugger.getScriptSource"] = &DispatcherImpl::getScriptSource;
        m_dispatchMap["Debugger.setPauseOnExceptions"] = &DispatcherImpl::setPauseOnExceptions;
        m_dispatchMap["Debugger.evaluateOnCallFrame"] = &DispatcherImpl::evaluateOnCallFrame;
        m_dispatchMap["Debugger.setVariableValue"] = &DispatcherImpl::setVariableValue;
        m_dispatchMap["Debugger.getBacktrace"] = &DispatcherImpl::getBacktrace;
        m_dispatchMap["Debugger.setAsyncCallStackDepth"] = &DispatcherImpl::setAsyncCallStackDepth;
        m_dispatchMap["Debugger.setBlackboxPatterns"] = &DispatcherImpl::setBlackboxPatterns;
        m_dispatchMap["Debugger.setBlackboxedRanges"] = &DispatcherImpl::setBlackboxedRanges;
    }
    ~DispatcherImpl() override { }
    void dispatch(int callId, const String16& method, std::unique_ptr<protocol::DictionaryValue> messageObject) override;

protected:
    using CallHandler = void (DispatcherImpl::*)(int callId, std::unique_ptr<DictionaryValue> messageObject, ErrorSupport* errors);
    using DispatchMap = protocol::HashMap<String16, CallHandler>;
    DispatchMap m_dispatchMap;

    void enable(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);
    void disable(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);
    void setBreakpointsActive(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);
    void setSkipAllPauses(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);
    void setBreakpointByUrl(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);
    void setBreakpoint(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);
    void removeBreakpoint(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);
    void continueToLocation(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);
    void stepOver(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);
    void stepInto(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);
    void stepOut(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);
    void pause(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);
    void resume(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);
    void searchInContent(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);
    void canSetScriptSource(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);
    void setScriptSource(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);
    void restartFrame(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);
    void getScriptSource(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);
    void setPauseOnExceptions(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);
    void evaluateOnCallFrame(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);
    void setVariableValue(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);
    void getBacktrace(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);
    void setAsyncCallStackDepth(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);
    void setBlackboxPatterns(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);
    void setBlackboxedRanges(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);

    Backend* m_backend;
};

void DispatcherImpl::dispatch(int callId, const String16& method, std::unique_ptr<protocol::DictionaryValue> messageObject)
{
    protocol::HashMap<String16, CallHandler>::iterator it = m_dispatchMap.find(method);
    if (it == m_dispatchMap.end()) {
        reportProtocolError(callId, MethodNotFound, "'" + method + "' wasn't found", nullptr);
        return;
    }

    protocol::ErrorSupport errors;
    (this->*(it->second))(callId, std::move(messageObject), &errors);
}


void DispatcherImpl::enable(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    ErrorString error;
    m_backend->enable(&error);
    if (weak->get())
        weak->get()->sendResponse(callId, error);
}

void DispatcherImpl::disable(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    ErrorString error;
    m_backend->disable(&error);
    if (weak->get())
        weak->get()->sendResponse(callId, error);
}

void DispatcherImpl::setBreakpointsActive(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{
    // Prepare input parameters.
    protocol::DictionaryValue* object = DictionaryValue::cast(requestMessageObject->get("params"));
    errors->push();
    protocol::Value* activeValue = object ? object->get("active") : nullptr;
    errors->setName("active");
    bool in_active = ValueConversions<bool>::parse(activeValue, errors);
    errors->pop();
    if (errors->hasErrors()) {
        reportProtocolError(callId, InvalidParams, kInvalidRequest, errors);
        return;
    }

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    ErrorString error;
    m_backend->setBreakpointsActive(&error, in_active);
    if (weak->get())
        weak->get()->sendResponse(callId, error);
}

void DispatcherImpl::setSkipAllPauses(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{
    // Prepare input parameters.
    protocol::DictionaryValue* object = DictionaryValue::cast(requestMessageObject->get("params"));
    errors->push();
    protocol::Value* skippedValue = object ? object->get("skipped") : nullptr;
    errors->setName("skipped");
    bool in_skipped = ValueConversions<bool>::parse(skippedValue, errors);
    errors->pop();
    if (errors->hasErrors()) {
        reportProtocolError(callId, InvalidParams, kInvalidRequest, errors);
        return;
    }

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    ErrorString error;
    m_backend->setSkipAllPauses(&error, in_skipped);
    if (weak->get())
        weak->get()->sendResponse(callId, error);
}

void DispatcherImpl::setBreakpointByUrl(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{
    // Prepare input parameters.
    protocol::DictionaryValue* object = DictionaryValue::cast(requestMessageObject->get("params"));
    errors->push();
    protocol::Value* lineNumberValue = object ? object->get("lineNumber") : nullptr;
    errors->setName("lineNumber");
    int in_lineNumber = ValueConversions<int>::parse(lineNumberValue, errors);
    protocol::Value* urlValue = object ? object->get("url") : nullptr;
    Maybe<String16> in_url;
    if (urlValue) {
        errors->setName("url");
        in_url = ValueConversions<String16>::parse(urlValue, errors);
    }
    protocol::Value* urlRegexValue = object ? object->get("urlRegex") : nullptr;
    Maybe<String16> in_urlRegex;
    if (urlRegexValue) {
        errors->setName("urlRegex");
        in_urlRegex = ValueConversions<String16>::parse(urlRegexValue, errors);
    }
    protocol::Value* columnNumberValue = object ? object->get("columnNumber") : nullptr;
    Maybe<int> in_columnNumber;
    if (columnNumberValue) {
        errors->setName("columnNumber");
        in_columnNumber = ValueConversions<int>::parse(columnNumberValue, errors);
    }
    protocol::Value* conditionValue = object ? object->get("condition") : nullptr;
    Maybe<String16> in_condition;
    if (conditionValue) {
        errors->setName("condition");
        in_condition = ValueConversions<String16>::parse(conditionValue, errors);
    }
    errors->pop();
    if (errors->hasErrors()) {
        reportProtocolError(callId, InvalidParams, kInvalidRequest, errors);
        return;
    }
    // Declare output parameters.
    std::unique_ptr<protocol::DictionaryValue> result = DictionaryValue::create();
    String16 out_breakpointId;
    std::unique_ptr<protocol::Array<protocol::Debugger::Location>> out_locations;

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    ErrorString error;
    m_backend->setBreakpointByUrl(&error, in_lineNumber, in_url, in_urlRegex, in_columnNumber, in_condition, &out_breakpointId, &out_locations);
    if (!error.length()) {
        result->setValue("breakpointId", ValueConversions<String16>::serialize(out_breakpointId));
        result->setValue("locations", ValueConversions<protocol::Array<protocol::Debugger::Location>>::serialize(out_locations.get()));
    }
    if (weak->get())
        weak->get()->sendResponse(callId, error, std::move(result));
}

void DispatcherImpl::setBreakpoint(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{
    // Prepare input parameters.
    protocol::DictionaryValue* object = DictionaryValue::cast(requestMessageObject->get("params"));
    errors->push();
    protocol::Value* locationValue = object ? object->get("location") : nullptr;
    errors->setName("location");
    std::unique_ptr<protocol::Debugger::Location> in_location = ValueConversions<protocol::Debugger::Location>::parse(locationValue, errors);
    protocol::Value* conditionValue = object ? object->get("condition") : nullptr;
    Maybe<String16> in_condition;
    if (conditionValue) {
        errors->setName("condition");
        in_condition = ValueConversions<String16>::parse(conditionValue, errors);
    }
    errors->pop();
    if (errors->hasErrors()) {
        reportProtocolError(callId, InvalidParams, kInvalidRequest, errors);
        return;
    }
    // Declare output parameters.
    std::unique_ptr<protocol::DictionaryValue> result = DictionaryValue::create();
    String16 out_breakpointId;
    std::unique_ptr<protocol::Debugger::Location> out_actualLocation;

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    ErrorString error;
    m_backend->setBreakpoint(&error, std::move(in_location), in_condition, &out_breakpointId, &out_actualLocation);
    if (!error.length()) {
        result->setValue("breakpointId", ValueConversions<String16>::serialize(out_breakpointId));
        result->setValue("actualLocation", ValueConversions<protocol::Debugger::Location>::serialize(out_actualLocation.get()));
    }
    if (weak->get())
        weak->get()->sendResponse(callId, error, std::move(result));
}

void DispatcherImpl::removeBreakpoint(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{
    // Prepare input parameters.
    protocol::DictionaryValue* object = DictionaryValue::cast(requestMessageObject->get("params"));
    errors->push();
    protocol::Value* breakpointIdValue = object ? object->get("breakpointId") : nullptr;
    errors->setName("breakpointId");
    String16 in_breakpointId = ValueConversions<String16>::parse(breakpointIdValue, errors);
    errors->pop();
    if (errors->hasErrors()) {
        reportProtocolError(callId, InvalidParams, kInvalidRequest, errors);
        return;
    }

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    ErrorString error;
    m_backend->removeBreakpoint(&error, in_breakpointId);
    if (weak->get())
        weak->get()->sendResponse(callId, error);
}

void DispatcherImpl::continueToLocation(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{
    // Prepare input parameters.
    protocol::DictionaryValue* object = DictionaryValue::cast(requestMessageObject->get("params"));
    errors->push();
    protocol::Value* locationValue = object ? object->get("location") : nullptr;
    errors->setName("location");
    std::unique_ptr<protocol::Debugger::Location> in_location = ValueConversions<protocol::Debugger::Location>::parse(locationValue, errors);
    protocol::Value* interstatementLocationValue = object ? object->get("interstatementLocation") : nullptr;
    Maybe<bool> in_interstatementLocation;
    if (interstatementLocationValue) {
        errors->setName("interstatementLocation");
        in_interstatementLocation = ValueConversions<bool>::parse(interstatementLocationValue, errors);
    }
    errors->pop();
    if (errors->hasErrors()) {
        reportProtocolError(callId, InvalidParams, kInvalidRequest, errors);
        return;
    }

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    ErrorString error;
    m_backend->continueToLocation(&error, std::move(in_location), in_interstatementLocation);
    if (weak->get())
        weak->get()->sendResponse(callId, error);
}

void DispatcherImpl::stepOver(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    ErrorString error;
    m_backend->stepOver(&error);
    if (weak->get())
        weak->get()->sendResponse(callId, error);
}

void DispatcherImpl::stepInto(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    ErrorString error;
    m_backend->stepInto(&error);
    if (weak->get())
        weak->get()->sendResponse(callId, error);
}

void DispatcherImpl::stepOut(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    ErrorString error;
    m_backend->stepOut(&error);
    if (weak->get())
        weak->get()->sendResponse(callId, error);
}

void DispatcherImpl::pause(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    ErrorString error;
    m_backend->pause(&error);
    if (weak->get())
        weak->get()->sendResponse(callId, error);
}

void DispatcherImpl::resume(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    ErrorString error;
    m_backend->resume(&error);
    if (weak->get())
        weak->get()->sendResponse(callId, error);
}

void DispatcherImpl::searchInContent(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{
    // Prepare input parameters.
    protocol::DictionaryValue* object = DictionaryValue::cast(requestMessageObject->get("params"));
    errors->push();
    protocol::Value* scriptIdValue = object ? object->get("scriptId") : nullptr;
    errors->setName("scriptId");
    String16 in_scriptId = ValueConversions<String16>::parse(scriptIdValue, errors);
    protocol::Value* queryValue = object ? object->get("query") : nullptr;
    errors->setName("query");
    String16 in_query = ValueConversions<String16>::parse(queryValue, errors);
    protocol::Value* caseSensitiveValue = object ? object->get("caseSensitive") : nullptr;
    Maybe<bool> in_caseSensitive;
    if (caseSensitiveValue) {
        errors->setName("caseSensitive");
        in_caseSensitive = ValueConversions<bool>::parse(caseSensitiveValue, errors);
    }
    protocol::Value* isRegexValue = object ? object->get("isRegex") : nullptr;
    Maybe<bool> in_isRegex;
    if (isRegexValue) {
        errors->setName("isRegex");
        in_isRegex = ValueConversions<bool>::parse(isRegexValue, errors);
    }
    errors->pop();
    if (errors->hasErrors()) {
        reportProtocolError(callId, InvalidParams, kInvalidRequest, errors);
        return;
    }
    // Declare output parameters.
    std::unique_ptr<protocol::DictionaryValue> result = DictionaryValue::create();
    std::unique_ptr<protocol::Array<protocol::Debugger::SearchMatch>> out_result;

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    ErrorString error;
    m_backend->searchInContent(&error, in_scriptId, in_query, in_caseSensitive, in_isRegex, &out_result);
    if (!error.length()) {
        result->setValue("result", ValueConversions<protocol::Array<protocol::Debugger::SearchMatch>>::serialize(out_result.get()));
    }
    if (weak->get())
        weak->get()->sendResponse(callId, error, std::move(result));
}

void DispatcherImpl::canSetScriptSource(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{
    // Declare output parameters.
    std::unique_ptr<protocol::DictionaryValue> result = DictionaryValue::create();
    bool out_result;

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    ErrorString error;
    m_backend->canSetScriptSource(&error, &out_result);
    if (!error.length()) {
        result->setValue("result", ValueConversions<bool>::serialize(out_result));
    }
    if (weak->get())
        weak->get()->sendResponse(callId, error, std::move(result));
}

void DispatcherImpl::setScriptSource(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{
    // Prepare input parameters.
    protocol::DictionaryValue* object = DictionaryValue::cast(requestMessageObject->get("params"));
    errors->push();
    protocol::Value* scriptIdValue = object ? object->get("scriptId") : nullptr;
    errors->setName("scriptId");
    String16 in_scriptId = ValueConversions<String16>::parse(scriptIdValue, errors);
    protocol::Value* scriptSourceValue = object ? object->get("scriptSource") : nullptr;
    errors->setName("scriptSource");
    String16 in_scriptSource = ValueConversions<String16>::parse(scriptSourceValue, errors);
    protocol::Value* previewValue = object ? object->get("preview") : nullptr;
    Maybe<bool> in_preview;
    if (previewValue) {
        errors->setName("preview");
        in_preview = ValueConversions<bool>::parse(previewValue, errors);
    }
    errors->pop();
    if (errors->hasErrors()) {
        reportProtocolError(callId, InvalidParams, kInvalidRequest, errors);
        return;
    }
    // Declare output parameters.
    std::unique_ptr<protocol::DictionaryValue> result = DictionaryValue::create();
    Maybe<protocol::Array<protocol::Debugger::CallFrame>> out_callFrames;
    Maybe<bool> out_stackChanged;
    Maybe<protocol::Runtime::StackTrace> out_asyncStackTrace;
    Maybe<protocol::Runtime::ExceptionDetails> out_compileError;

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    ErrorString error;
    m_backend->setScriptSource(&error, in_scriptId, in_scriptSource, in_preview, &out_callFrames, &out_stackChanged, &out_asyncStackTrace, &out_compileError);
    if (!error.length()) {
        if (out_callFrames.isJust())
            result->setValue("callFrames", ValueConversions<protocol::Array<protocol::Debugger::CallFrame>>::serialize(out_callFrames.fromJust()));
        if (out_stackChanged.isJust())
            result->setValue("stackChanged", ValueConversions<bool>::serialize(out_stackChanged.fromJust()));
        if (out_asyncStackTrace.isJust())
            result->setValue("asyncStackTrace", ValueConversions<protocol::Runtime::StackTrace>::serialize(out_asyncStackTrace.fromJust()));
        if (out_compileError.isJust())
            result->setValue("compileError", ValueConversions<protocol::Runtime::ExceptionDetails>::serialize(out_compileError.fromJust()));
    }
    if (weak->get())
        weak->get()->sendResponse(callId, error, std::move(result));
}

void DispatcherImpl::restartFrame(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{
    // Prepare input parameters.
    protocol::DictionaryValue* object = DictionaryValue::cast(requestMessageObject->get("params"));
    errors->push();
    protocol::Value* callFrameIdValue = object ? object->get("callFrameId") : nullptr;
    errors->setName("callFrameId");
    String16 in_callFrameId = ValueConversions<String16>::parse(callFrameIdValue, errors);
    errors->pop();
    if (errors->hasErrors()) {
        reportProtocolError(callId, InvalidParams, kInvalidRequest, errors);
        return;
    }
    // Declare output parameters.
    std::unique_ptr<protocol::DictionaryValue> result = DictionaryValue::create();
    std::unique_ptr<protocol::Array<protocol::Debugger::CallFrame>> out_callFrames;
    Maybe<protocol::Runtime::StackTrace> out_asyncStackTrace;

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    ErrorString error;
    m_backend->restartFrame(&error, in_callFrameId, &out_callFrames, &out_asyncStackTrace);
    if (!error.length()) {
        result->setValue("callFrames", ValueConversions<protocol::Array<protocol::Debugger::CallFrame>>::serialize(out_callFrames.get()));
        if (out_asyncStackTrace.isJust())
            result->setValue("asyncStackTrace", ValueConversions<protocol::Runtime::StackTrace>::serialize(out_asyncStackTrace.fromJust()));
    }
    if (weak->get())
        weak->get()->sendResponse(callId, error, std::move(result));
}

void DispatcherImpl::getScriptSource(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{
    // Prepare input parameters.
    protocol::DictionaryValue* object = DictionaryValue::cast(requestMessageObject->get("params"));
    errors->push();
    protocol::Value* scriptIdValue = object ? object->get("scriptId") : nullptr;
    errors->setName("scriptId");
    String16 in_scriptId = ValueConversions<String16>::parse(scriptIdValue, errors);
    errors->pop();
    if (errors->hasErrors()) {
        reportProtocolError(callId, InvalidParams, kInvalidRequest, errors);
        return;
    }
    // Declare output parameters.
    std::unique_ptr<protocol::DictionaryValue> result = DictionaryValue::create();
    String16 out_scriptSource;

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    ErrorString error;
    m_backend->getScriptSource(&error, in_scriptId, &out_scriptSource);
    if (!error.length()) {
        result->setValue("scriptSource", ValueConversions<String16>::serialize(out_scriptSource));
    }
    if (weak->get())
        weak->get()->sendResponse(callId, error, std::move(result));
}

void DispatcherImpl::setPauseOnExceptions(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{
    // Prepare input parameters.
    protocol::DictionaryValue* object = DictionaryValue::cast(requestMessageObject->get("params"));
    errors->push();
    protocol::Value* stateValue = object ? object->get("state") : nullptr;
    errors->setName("state");
    String16 in_state = ValueConversions<String16>::parse(stateValue, errors);
    errors->pop();
    if (errors->hasErrors()) {
        reportProtocolError(callId, InvalidParams, kInvalidRequest, errors);
        return;
    }

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    ErrorString error;
    m_backend->setPauseOnExceptions(&error, in_state);
    if (weak->get())
        weak->get()->sendResponse(callId, error);
}

void DispatcherImpl::evaluateOnCallFrame(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{
    // Prepare input parameters.
    protocol::DictionaryValue* object = DictionaryValue::cast(requestMessageObject->get("params"));
    errors->push();
    protocol::Value* callFrameIdValue = object ? object->get("callFrameId") : nullptr;
    errors->setName("callFrameId");
    String16 in_callFrameId = ValueConversions<String16>::parse(callFrameIdValue, errors);
    protocol::Value* expressionValue = object ? object->get("expression") : nullptr;
    errors->setName("expression");
    String16 in_expression = ValueConversions<String16>::parse(expressionValue, errors);
    protocol::Value* objectGroupValue = object ? object->get("objectGroup") : nullptr;
    Maybe<String16> in_objectGroup;
    if (objectGroupValue) {
        errors->setName("objectGroup");
        in_objectGroup = ValueConversions<String16>::parse(objectGroupValue, errors);
    }
    protocol::Value* includeCommandLineAPIValue = object ? object->get("includeCommandLineAPI") : nullptr;
    Maybe<bool> in_includeCommandLineAPI;
    if (includeCommandLineAPIValue) {
        errors->setName("includeCommandLineAPI");
        in_includeCommandLineAPI = ValueConversions<bool>::parse(includeCommandLineAPIValue, errors);
    }
    protocol::Value* doNotPauseOnExceptionsAndMuteConsoleValue = object ? object->get("doNotPauseOnExceptionsAndMuteConsole") : nullptr;
    Maybe<bool> in_doNotPauseOnExceptionsAndMuteConsole;
    if (doNotPauseOnExceptionsAndMuteConsoleValue) {
        errors->setName("doNotPauseOnExceptionsAndMuteConsole");
        in_doNotPauseOnExceptionsAndMuteConsole = ValueConversions<bool>::parse(doNotPauseOnExceptionsAndMuteConsoleValue, errors);
    }
    protocol::Value* returnByValueValue = object ? object->get("returnByValue") : nullptr;
    Maybe<bool> in_returnByValue;
    if (returnByValueValue) {
        errors->setName("returnByValue");
        in_returnByValue = ValueConversions<bool>::parse(returnByValueValue, errors);
    }
    protocol::Value* generatePreviewValue = object ? object->get("generatePreview") : nullptr;
    Maybe<bool> in_generatePreview;
    if (generatePreviewValue) {
        errors->setName("generatePreview");
        in_generatePreview = ValueConversions<bool>::parse(generatePreviewValue, errors);
    }
    errors->pop();
    if (errors->hasErrors()) {
        reportProtocolError(callId, InvalidParams, kInvalidRequest, errors);
        return;
    }
    // Declare output parameters.
    std::unique_ptr<protocol::DictionaryValue> result = DictionaryValue::create();
    std::unique_ptr<protocol::Runtime::RemoteObject> out_result;
    Maybe<bool> out_wasThrown;
    Maybe<protocol::Runtime::ExceptionDetails> out_exceptionDetails;

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    ErrorString error;
    m_backend->evaluateOnCallFrame(&error, in_callFrameId, in_expression, in_objectGroup, in_includeCommandLineAPI, in_doNotPauseOnExceptionsAndMuteConsole, in_returnByValue, in_generatePreview, &out_result, &out_wasThrown, &out_exceptionDetails);
    if (!error.length()) {
        result->setValue("result", ValueConversions<protocol::Runtime::RemoteObject>::serialize(out_result.get()));
        if (out_wasThrown.isJust())
            result->setValue("wasThrown", ValueConversions<bool>::serialize(out_wasThrown.fromJust()));
        if (out_exceptionDetails.isJust())
            result->setValue("exceptionDetails", ValueConversions<protocol::Runtime::ExceptionDetails>::serialize(out_exceptionDetails.fromJust()));
    }
    if (weak->get())
        weak->get()->sendResponse(callId, error, std::move(result));
}

void DispatcherImpl::setVariableValue(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{
    // Prepare input parameters.
    protocol::DictionaryValue* object = DictionaryValue::cast(requestMessageObject->get("params"));
    errors->push();
    protocol::Value* scopeNumberValue = object ? object->get("scopeNumber") : nullptr;
    errors->setName("scopeNumber");
    int in_scopeNumber = ValueConversions<int>::parse(scopeNumberValue, errors);
    protocol::Value* variableNameValue = object ? object->get("variableName") : nullptr;
    errors->setName("variableName");
    String16 in_variableName = ValueConversions<String16>::parse(variableNameValue, errors);
    protocol::Value* newValueValue = object ? object->get("newValue") : nullptr;
    errors->setName("newValue");
    std::unique_ptr<protocol::Runtime::CallArgument> in_newValue = ValueConversions<protocol::Runtime::CallArgument>::parse(newValueValue, errors);
    protocol::Value* callFrameIdValue = object ? object->get("callFrameId") : nullptr;
    errors->setName("callFrameId");
    String16 in_callFrameId = ValueConversions<String16>::parse(callFrameIdValue, errors);
    errors->pop();
    if (errors->hasErrors()) {
        reportProtocolError(callId, InvalidParams, kInvalidRequest, errors);
        return;
    }

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    ErrorString error;
    m_backend->setVariableValue(&error, in_scopeNumber, in_variableName, std::move(in_newValue), in_callFrameId);
    if (weak->get())
        weak->get()->sendResponse(callId, error);
}

void DispatcherImpl::getBacktrace(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{
    // Declare output parameters.
    std::unique_ptr<protocol::DictionaryValue> result = DictionaryValue::create();
    std::unique_ptr<protocol::Array<protocol::Debugger::CallFrame>> out_callFrames;
    Maybe<protocol::Runtime::StackTrace> out_asyncStackTrace;

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    ErrorString error;
    m_backend->getBacktrace(&error, &out_callFrames, &out_asyncStackTrace);
    if (!error.length()) {
        result->setValue("callFrames", ValueConversions<protocol::Array<protocol::Debugger::CallFrame>>::serialize(out_callFrames.get()));
        if (out_asyncStackTrace.isJust())
            result->setValue("asyncStackTrace", ValueConversions<protocol::Runtime::StackTrace>::serialize(out_asyncStackTrace.fromJust()));
    }
    if (weak->get())
        weak->get()->sendResponse(callId, error, std::move(result));
}

void DispatcherImpl::setAsyncCallStackDepth(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{
    // Prepare input parameters.
    protocol::DictionaryValue* object = DictionaryValue::cast(requestMessageObject->get("params"));
    errors->push();
    protocol::Value* maxDepthValue = object ? object->get("maxDepth") : nullptr;
    errors->setName("maxDepth");
    int in_maxDepth = ValueConversions<int>::parse(maxDepthValue, errors);
    errors->pop();
    if (errors->hasErrors()) {
        reportProtocolError(callId, InvalidParams, kInvalidRequest, errors);
        return;
    }

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    ErrorString error;
    m_backend->setAsyncCallStackDepth(&error, in_maxDepth);
    if (weak->get())
        weak->get()->sendResponse(callId, error);
}

void DispatcherImpl::setBlackboxPatterns(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{
    // Prepare input parameters.
    protocol::DictionaryValue* object = DictionaryValue::cast(requestMessageObject->get("params"));
    errors->push();
    protocol::Value* patternsValue = object ? object->get("patterns") : nullptr;
    errors->setName("patterns");
    std::unique_ptr<protocol::Array<String16>> in_patterns = ValueConversions<protocol::Array<String16>>::parse(patternsValue, errors);
    errors->pop();
    if (errors->hasErrors()) {
        reportProtocolError(callId, InvalidParams, kInvalidRequest, errors);
        return;
    }

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    ErrorString error;
    m_backend->setBlackboxPatterns(&error, std::move(in_patterns));
    if (weak->get())
        weak->get()->sendResponse(callId, error);
}

void DispatcherImpl::setBlackboxedRanges(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{
    // Prepare input parameters.
    protocol::DictionaryValue* object = DictionaryValue::cast(requestMessageObject->get("params"));
    errors->push();
    protocol::Value* scriptIdValue = object ? object->get("scriptId") : nullptr;
    errors->setName("scriptId");
    String16 in_scriptId = ValueConversions<String16>::parse(scriptIdValue, errors);
    protocol::Value* positionsValue = object ? object->get("positions") : nullptr;
    errors->setName("positions");
    std::unique_ptr<protocol::Array<protocol::Debugger::ScriptPosition>> in_positions = ValueConversions<protocol::Array<protocol::Debugger::ScriptPosition>>::parse(positionsValue, errors);
    errors->pop();
    if (errors->hasErrors()) {
        reportProtocolError(callId, InvalidParams, kInvalidRequest, errors);
        return;
    }

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    ErrorString error;
    m_backend->setBlackboxedRanges(&error, in_scriptId, std::move(in_positions));
    if (weak->get())
        weak->get()->sendResponse(callId, error);
}

// static
void Dispatcher::wire(UberDispatcher* dispatcher, Backend* backend)
{
    dispatcher->registerBackend("Debugger", wrapUnique(new DispatcherImpl(dispatcher->channel(), backend)));
}

} // Debugger
} // namespace protocol
} // namespace blink
