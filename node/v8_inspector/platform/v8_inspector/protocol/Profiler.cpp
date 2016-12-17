// This file is generated

// Copyright (c) 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "platform\v8_inspector\protocol/Profiler.h"

#include "platform/inspector_protocol/DispatcherBase.h"
#include "platform/inspector_protocol/Parser.h"

namespace blink {
namespace protocol {
namespace Profiler {

// ------------- Enum values from types.

const char Metainfo::domainName[] = "Profiler";
const char Metainfo::commandPrefix[] = "Profiler.";

std::unique_ptr<CPUProfileNode> CPUProfileNode::parse(protocol::Value* value, ErrorSupport* errors)
{
    if (!value || value->type() != protocol::Value::TypeObject) {
        errors->addError("object expected");
        return nullptr;
    }

    std::unique_ptr<CPUProfileNode> result(new CPUProfileNode());
    protocol::DictionaryValue* object = DictionaryValue::cast(value);
    errors->push();
    protocol::Value* callFrameValue = object->get("callFrame");
    errors->setName("callFrame");
    result->m_callFrame = ValueConversions<protocol::Runtime::CallFrame>::parse(callFrameValue, errors);
    protocol::Value* hitCountValue = object->get("hitCount");
    errors->setName("hitCount");
    result->m_hitCount = ValueConversions<int>::parse(hitCountValue, errors);
    protocol::Value* childrenValue = object->get("children");
    errors->setName("children");
    result->m_children = ValueConversions<protocol::Array<protocol::Profiler::CPUProfileNode>>::parse(childrenValue, errors);
    protocol::Value* deoptReasonValue = object->get("deoptReason");
    errors->setName("deoptReason");
    result->m_deoptReason = ValueConversions<String16>::parse(deoptReasonValue, errors);
    protocol::Value* idValue = object->get("id");
    errors->setName("id");
    result->m_id = ValueConversions<int>::parse(idValue, errors);
    protocol::Value* positionTicksValue = object->get("positionTicks");
    errors->setName("positionTicks");
    result->m_positionTicks = ValueConversions<protocol::Array<protocol::Profiler::PositionTickInfo>>::parse(positionTicksValue, errors);
    errors->pop();
    if (errors->hasErrors())
        return nullptr;
    return result;
}

std::unique_ptr<protocol::DictionaryValue> CPUProfileNode::serialize() const
{
    std::unique_ptr<protocol::DictionaryValue> result = DictionaryValue::create();
    result->setValue("callFrame", ValueConversions<protocol::Runtime::CallFrame>::serialize(m_callFrame.get()));
    result->setValue("hitCount", ValueConversions<int>::serialize(m_hitCount));
    result->setValue("children", ValueConversions<protocol::Array<protocol::Profiler::CPUProfileNode>>::serialize(m_children.get()));
    result->setValue("deoptReason", ValueConversions<String16>::serialize(m_deoptReason));
    result->setValue("id", ValueConversions<int>::serialize(m_id));
    result->setValue("positionTicks", ValueConversions<protocol::Array<protocol::Profiler::PositionTickInfo>>::serialize(m_positionTicks.get()));
    return result;
}

std::unique_ptr<CPUProfileNode> CPUProfileNode::clone() const
{
    ErrorSupport errors;
    return parse(serialize().get(), &errors);
}

std::unique_ptr<CPUProfile> CPUProfile::parse(protocol::Value* value, ErrorSupport* errors)
{
    if (!value || value->type() != protocol::Value::TypeObject) {
        errors->addError("object expected");
        return nullptr;
    }

    std::unique_ptr<CPUProfile> result(new CPUProfile());
    protocol::DictionaryValue* object = DictionaryValue::cast(value);
    errors->push();
    protocol::Value* headValue = object->get("head");
    errors->setName("head");
    result->m_head = ValueConversions<protocol::Profiler::CPUProfileNode>::parse(headValue, errors);
    protocol::Value* startTimeValue = object->get("startTime");
    errors->setName("startTime");
    result->m_startTime = ValueConversions<double>::parse(startTimeValue, errors);
    protocol::Value* endTimeValue = object->get("endTime");
    errors->setName("endTime");
    result->m_endTime = ValueConversions<double>::parse(endTimeValue, errors);
    protocol::Value* samplesValue = object->get("samples");
    if (samplesValue) {
        errors->setName("samples");
        result->m_samples = ValueConversions<protocol::Array<int>>::parse(samplesValue, errors);
    }
    protocol::Value* timestampsValue = object->get("timestamps");
    if (timestampsValue) {
        errors->setName("timestamps");
        result->m_timestamps = ValueConversions<protocol::Array<double>>::parse(timestampsValue, errors);
    }
    errors->pop();
    if (errors->hasErrors())
        return nullptr;
    return result;
}

std::unique_ptr<protocol::DictionaryValue> CPUProfile::serialize() const
{
    std::unique_ptr<protocol::DictionaryValue> result = DictionaryValue::create();
    result->setValue("head", ValueConversions<protocol::Profiler::CPUProfileNode>::serialize(m_head.get()));
    result->setValue("startTime", ValueConversions<double>::serialize(m_startTime));
    result->setValue("endTime", ValueConversions<double>::serialize(m_endTime));
    if (m_samples.isJust())
        result->setValue("samples", ValueConversions<protocol::Array<int>>::serialize(m_samples.fromJust()));
    if (m_timestamps.isJust())
        result->setValue("timestamps", ValueConversions<protocol::Array<double>>::serialize(m_timestamps.fromJust()));
    return result;
}

std::unique_ptr<CPUProfile> CPUProfile::clone() const
{
    ErrorSupport errors;
    return parse(serialize().get(), &errors);
}

std::unique_ptr<PositionTickInfo> PositionTickInfo::parse(protocol::Value* value, ErrorSupport* errors)
{
    if (!value || value->type() != protocol::Value::TypeObject) {
        errors->addError("object expected");
        return nullptr;
    }

    std::unique_ptr<PositionTickInfo> result(new PositionTickInfo());
    protocol::DictionaryValue* object = DictionaryValue::cast(value);
    errors->push();
    protocol::Value* lineValue = object->get("line");
    errors->setName("line");
    result->m_line = ValueConversions<int>::parse(lineValue, errors);
    protocol::Value* ticksValue = object->get("ticks");
    errors->setName("ticks");
    result->m_ticks = ValueConversions<int>::parse(ticksValue, errors);
    errors->pop();
    if (errors->hasErrors())
        return nullptr;
    return result;
}

std::unique_ptr<protocol::DictionaryValue> PositionTickInfo::serialize() const
{
    std::unique_ptr<protocol::DictionaryValue> result = DictionaryValue::create();
    result->setValue("line", ValueConversions<int>::serialize(m_line));
    result->setValue("ticks", ValueConversions<int>::serialize(m_ticks));
    return result;
}

std::unique_ptr<PositionTickInfo> PositionTickInfo::clone() const
{
    ErrorSupport errors;
    return parse(serialize().get(), &errors);
}

// ------------- Enum values from params.


// ------------- Frontend notifications.

void Frontend::consoleProfileStarted(const String16& id, std::unique_ptr<protocol::Debugger::Location> location, const Maybe<String16>& title)
{
    std::unique_ptr<protocol::DictionaryValue> jsonMessage = DictionaryValue::create();
    jsonMessage->setString("method", "Profiler.consoleProfileStarted");
    std::unique_ptr<protocol::DictionaryValue> paramsObject = DictionaryValue::create();
    paramsObject->setValue("id", ValueConversions<String16>::serialize(id));
    paramsObject->setValue("location", ValueConversions<protocol::Debugger::Location>::serialize(location.get()));
    if (title.isJust())
        paramsObject->setValue("title", ValueConversions<String16>::serialize(title.fromJust()));
    jsonMessage->setObject("params", std::move(paramsObject));
    if (m_frontendChannel)
        m_frontendChannel->sendProtocolNotification(jsonMessage->toJSONString());
}

void Frontend::consoleProfileFinished(const String16& id, std::unique_ptr<protocol::Debugger::Location> location, std::unique_ptr<protocol::Profiler::CPUProfile> profile, const Maybe<String16>& title)
{
    std::unique_ptr<protocol::DictionaryValue> jsonMessage = DictionaryValue::create();
    jsonMessage->setString("method", "Profiler.consoleProfileFinished");
    std::unique_ptr<protocol::DictionaryValue> paramsObject = DictionaryValue::create();
    paramsObject->setValue("id", ValueConversions<String16>::serialize(id));
    paramsObject->setValue("location", ValueConversions<protocol::Debugger::Location>::serialize(location.get()));
    paramsObject->setValue("profile", ValueConversions<protocol::Profiler::CPUProfile>::serialize(profile.get()));
    if (title.isJust())
        paramsObject->setValue("title", ValueConversions<String16>::serialize(title.fromJust()));
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
        m_dispatchMap["Profiler.enable"] = &DispatcherImpl::enable;
        m_dispatchMap["Profiler.disable"] = &DispatcherImpl::disable;
        m_dispatchMap["Profiler.setSamplingInterval"] = &DispatcherImpl::setSamplingInterval;
        m_dispatchMap["Profiler.start"] = &DispatcherImpl::start;
        m_dispatchMap["Profiler.stop"] = &DispatcherImpl::stop;
    }
    ~DispatcherImpl() override { }
    void dispatch(int callId, const String16& method, std::unique_ptr<protocol::DictionaryValue> messageObject) override;

protected:
    using CallHandler = void (DispatcherImpl::*)(int callId, std::unique_ptr<DictionaryValue> messageObject, ErrorSupport* errors);
    using DispatchMap = protocol::HashMap<String16, CallHandler>;
    DispatchMap m_dispatchMap;

    void enable(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);
    void disable(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);
    void setSamplingInterval(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);
    void start(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);
    void stop(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);

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

void DispatcherImpl::setSamplingInterval(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{
    // Prepare input parameters.
    protocol::DictionaryValue* object = DictionaryValue::cast(requestMessageObject->get("params"));
    errors->push();
    protocol::Value* intervalValue = object ? object->get("interval") : nullptr;
    errors->setName("interval");
    int in_interval = ValueConversions<int>::parse(intervalValue, errors);
    errors->pop();
    if (errors->hasErrors()) {
        reportProtocolError(callId, InvalidParams, kInvalidRequest, errors);
        return;
    }

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    ErrorString error;
    m_backend->setSamplingInterval(&error, in_interval);
    if (weak->get())
        weak->get()->sendResponse(callId, error);
}

void DispatcherImpl::start(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    ErrorString error;
    m_backend->start(&error);
    if (weak->get())
        weak->get()->sendResponse(callId, error);
}

void DispatcherImpl::stop(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{
    // Declare output parameters.
    std::unique_ptr<protocol::DictionaryValue> result = DictionaryValue::create();
    std::unique_ptr<protocol::Profiler::CPUProfile> out_profile;

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    ErrorString error;
    m_backend->stop(&error, &out_profile);
    if (!error.length()) {
        result->setValue("profile", ValueConversions<protocol::Profiler::CPUProfile>::serialize(out_profile.get()));
    }
    if (weak->get())
        weak->get()->sendResponse(callId, error, std::move(result));
}

// static
void Dispatcher::wire(UberDispatcher* dispatcher, Backend* backend)
{
    dispatcher->registerBackend("Profiler", wrapUnique(new DispatcherImpl(dispatcher->channel(), backend)));
}

} // Profiler
} // namespace protocol
} // namespace blink
