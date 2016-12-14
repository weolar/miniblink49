// This file is generated

// Copyright (c) 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "platform\v8_inspector\protocol/HeapProfiler.h"

#include "platform/inspector_protocol/DispatcherBase.h"
#include "platform/inspector_protocol/Parser.h"

namespace blink {
namespace protocol {
namespace HeapProfiler {

// ------------- Enum values from types.

const char Metainfo::domainName[] = "HeapProfiler";
const char Metainfo::commandPrefix[] = "HeapProfiler.";

std::unique_ptr<SamplingHeapProfileNode> SamplingHeapProfileNode::parse(protocol::Value* value, ErrorSupport* errors)
{
    if (!value || value->type() != protocol::Value::TypeObject) {
        errors->addError("object expected");
        return nullptr;
    }

    std::unique_ptr<SamplingHeapProfileNode> result(new SamplingHeapProfileNode());
    protocol::DictionaryValue* object = DictionaryValue::cast(value);
    errors->push();
    protocol::Value* callFrameValue = object->get("callFrame");
    errors->setName("callFrame");
    result->m_callFrame = ValueConversions<protocol::Runtime::CallFrame>::parse(callFrameValue, errors);
    protocol::Value* selfSizeValue = object->get("selfSize");
    errors->setName("selfSize");
    result->m_selfSize = ValueConversions<double>::parse(selfSizeValue, errors);
    protocol::Value* childrenValue = object->get("children");
    errors->setName("children");
    result->m_children = ValueConversions<protocol::Array<protocol::HeapProfiler::SamplingHeapProfileNode>>::parse(childrenValue, errors);
    errors->pop();
    if (errors->hasErrors())
        return nullptr;
    return result;
}

std::unique_ptr<protocol::DictionaryValue> SamplingHeapProfileNode::serialize() const
{
    std::unique_ptr<protocol::DictionaryValue> result = DictionaryValue::create();
    result->setValue("callFrame", ValueConversions<protocol::Runtime::CallFrame>::serialize(m_callFrame.get()));
    result->setValue("selfSize", ValueConversions<double>::serialize(m_selfSize));
    result->setValue("children", ValueConversions<protocol::Array<protocol::HeapProfiler::SamplingHeapProfileNode>>::serialize(m_children.get()));
    return result;
}

std::unique_ptr<SamplingHeapProfileNode> SamplingHeapProfileNode::clone() const
{
    ErrorSupport errors;
    return parse(serialize().get(), &errors);
}

std::unique_ptr<SamplingHeapProfile> SamplingHeapProfile::parse(protocol::Value* value, ErrorSupport* errors)
{
    if (!value || value->type() != protocol::Value::TypeObject) {
        errors->addError("object expected");
        return nullptr;
    }

    std::unique_ptr<SamplingHeapProfile> result(new SamplingHeapProfile());
    protocol::DictionaryValue* object = DictionaryValue::cast(value);
    errors->push();
    protocol::Value* headValue = object->get("head");
    errors->setName("head");
    result->m_head = ValueConversions<protocol::HeapProfiler::SamplingHeapProfileNode>::parse(headValue, errors);
    errors->pop();
    if (errors->hasErrors())
        return nullptr;
    return result;
}

std::unique_ptr<protocol::DictionaryValue> SamplingHeapProfile::serialize() const
{
    std::unique_ptr<protocol::DictionaryValue> result = DictionaryValue::create();
    result->setValue("head", ValueConversions<protocol::HeapProfiler::SamplingHeapProfileNode>::serialize(m_head.get()));
    return result;
}

std::unique_ptr<SamplingHeapProfile> SamplingHeapProfile::clone() const
{
    ErrorSupport errors;
    return parse(serialize().get(), &errors);
}

// ------------- Enum values from params.


// ------------- Frontend notifications.

void Frontend::addHeapSnapshotChunk(const String16& chunk)
{
    std::unique_ptr<protocol::DictionaryValue> jsonMessage = DictionaryValue::create();
    jsonMessage->setString("method", "HeapProfiler.addHeapSnapshotChunk");
    std::unique_ptr<protocol::DictionaryValue> paramsObject = DictionaryValue::create();
    paramsObject->setValue("chunk", ValueConversions<String16>::serialize(chunk));
    jsonMessage->setObject("params", std::move(paramsObject));
    if (m_frontendChannel)
        m_frontendChannel->sendProtocolNotification(jsonMessage->toJSONString());
}

void Frontend::resetProfiles()
{
    std::unique_ptr<protocol::DictionaryValue> jsonMessage = DictionaryValue::create();
    jsonMessage->setString("method", "HeapProfiler.resetProfiles");
    std::unique_ptr<protocol::DictionaryValue> paramsObject = DictionaryValue::create();
    jsonMessage->setObject("params", std::move(paramsObject));
    if (m_frontendChannel)
        m_frontendChannel->sendProtocolNotification(jsonMessage->toJSONString());
}

void Frontend::reportHeapSnapshotProgress(int done, int total, const Maybe<bool>& finished)
{
    std::unique_ptr<protocol::DictionaryValue> jsonMessage = DictionaryValue::create();
    jsonMessage->setString("method", "HeapProfiler.reportHeapSnapshotProgress");
    std::unique_ptr<protocol::DictionaryValue> paramsObject = DictionaryValue::create();
    paramsObject->setValue("done", ValueConversions<int>::serialize(done));
    paramsObject->setValue("total", ValueConversions<int>::serialize(total));
    if (finished.isJust())
        paramsObject->setValue("finished", ValueConversions<bool>::serialize(finished.fromJust()));
    jsonMessage->setObject("params", std::move(paramsObject));
    if (m_frontendChannel)
        m_frontendChannel->sendProtocolNotification(jsonMessage->toJSONString());
}

void Frontend::lastSeenObjectId(int lastSeenObjectId, double timestamp)
{
    std::unique_ptr<protocol::DictionaryValue> jsonMessage = DictionaryValue::create();
    jsonMessage->setString("method", "HeapProfiler.lastSeenObjectId");
    std::unique_ptr<protocol::DictionaryValue> paramsObject = DictionaryValue::create();
    paramsObject->setValue("lastSeenObjectId", ValueConversions<int>::serialize(lastSeenObjectId));
    paramsObject->setValue("timestamp", ValueConversions<double>::serialize(timestamp));
    jsonMessage->setObject("params", std::move(paramsObject));
    if (m_frontendChannel)
        m_frontendChannel->sendProtocolNotification(jsonMessage->toJSONString());
}

void Frontend::heapStatsUpdate(std::unique_ptr<protocol::Array<int>> statsUpdate)
{
    std::unique_ptr<protocol::DictionaryValue> jsonMessage = DictionaryValue::create();
    jsonMessage->setString("method", "HeapProfiler.heapStatsUpdate");
    std::unique_ptr<protocol::DictionaryValue> paramsObject = DictionaryValue::create();
    paramsObject->setValue("statsUpdate", ValueConversions<protocol::Array<int>>::serialize(statsUpdate.get()));
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
        m_dispatchMap["HeapProfiler.enable"] = &DispatcherImpl::enable;
        m_dispatchMap["HeapProfiler.disable"] = &DispatcherImpl::disable;
        m_dispatchMap["HeapProfiler.startTrackingHeapObjects"] = &DispatcherImpl::startTrackingHeapObjects;
        m_dispatchMap["HeapProfiler.stopTrackingHeapObjects"] = &DispatcherImpl::stopTrackingHeapObjects;
        m_dispatchMap["HeapProfiler.takeHeapSnapshot"] = &DispatcherImpl::takeHeapSnapshot;
        m_dispatchMap["HeapProfiler.collectGarbage"] = &DispatcherImpl::collectGarbage;
        m_dispatchMap["HeapProfiler.getObjectByHeapObjectId"] = &DispatcherImpl::getObjectByHeapObjectId;
        m_dispatchMap["HeapProfiler.addInspectedHeapObject"] = &DispatcherImpl::addInspectedHeapObject;
        m_dispatchMap["HeapProfiler.getHeapObjectId"] = &DispatcherImpl::getHeapObjectId;
        m_dispatchMap["HeapProfiler.startSampling"] = &DispatcherImpl::startSampling;
        m_dispatchMap["HeapProfiler.stopSampling"] = &DispatcherImpl::stopSampling;
    }
    ~DispatcherImpl() override { }
    void dispatch(int callId, const String16& method, std::unique_ptr<protocol::DictionaryValue> messageObject) override;

protected:
    using CallHandler = void (DispatcherImpl::*)(int callId, std::unique_ptr<DictionaryValue> messageObject, ErrorSupport* errors);
    using DispatchMap = protocol::HashMap<String16, CallHandler>;
    DispatchMap m_dispatchMap;

    void enable(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);
    void disable(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);
    void startTrackingHeapObjects(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);
    void stopTrackingHeapObjects(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);
    void takeHeapSnapshot(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);
    void collectGarbage(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);
    void getObjectByHeapObjectId(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);
    void addInspectedHeapObject(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);
    void getHeapObjectId(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);
    void startSampling(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);
    void stopSampling(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport*);

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

void DispatcherImpl::startTrackingHeapObjects(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{
    // Prepare input parameters.
    protocol::DictionaryValue* object = DictionaryValue::cast(requestMessageObject->get("params"));
    errors->push();
    protocol::Value* trackAllocationsValue = object ? object->get("trackAllocations") : nullptr;
    Maybe<bool> in_trackAllocations;
    if (trackAllocationsValue) {
        errors->setName("trackAllocations");
        in_trackAllocations = ValueConversions<bool>::parse(trackAllocationsValue, errors);
    }
    errors->pop();
    if (errors->hasErrors()) {
        reportProtocolError(callId, InvalidParams, kInvalidRequest, errors);
        return;
    }

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    ErrorString error;
    m_backend->startTrackingHeapObjects(&error, in_trackAllocations);
    if (weak->get())
        weak->get()->sendResponse(callId, error);
}

void DispatcherImpl::stopTrackingHeapObjects(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{
    // Prepare input parameters.
    protocol::DictionaryValue* object = DictionaryValue::cast(requestMessageObject->get("params"));
    errors->push();
    protocol::Value* reportProgressValue = object ? object->get("reportProgress") : nullptr;
    Maybe<bool> in_reportProgress;
    if (reportProgressValue) {
        errors->setName("reportProgress");
        in_reportProgress = ValueConversions<bool>::parse(reportProgressValue, errors);
    }
    errors->pop();
    if (errors->hasErrors()) {
        reportProtocolError(callId, InvalidParams, kInvalidRequest, errors);
        return;
    }

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    ErrorString error;
    m_backend->stopTrackingHeapObjects(&error, in_reportProgress);
    if (weak->get())
        weak->get()->sendResponse(callId, error);
}

void DispatcherImpl::takeHeapSnapshot(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{
    // Prepare input parameters.
    protocol::DictionaryValue* object = DictionaryValue::cast(requestMessageObject->get("params"));
    errors->push();
    protocol::Value* reportProgressValue = object ? object->get("reportProgress") : nullptr;
    Maybe<bool> in_reportProgress;
    if (reportProgressValue) {
        errors->setName("reportProgress");
        in_reportProgress = ValueConversions<bool>::parse(reportProgressValue, errors);
    }
    errors->pop();
    if (errors->hasErrors()) {
        reportProtocolError(callId, InvalidParams, kInvalidRequest, errors);
        return;
    }

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    ErrorString error;
    m_backend->takeHeapSnapshot(&error, in_reportProgress);
    if (weak->get())
        weak->get()->sendResponse(callId, error);
}

void DispatcherImpl::collectGarbage(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    ErrorString error;
    m_backend->collectGarbage(&error);
    if (weak->get())
        weak->get()->sendResponse(callId, error);
}

void DispatcherImpl::getObjectByHeapObjectId(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{
    // Prepare input parameters.
    protocol::DictionaryValue* object = DictionaryValue::cast(requestMessageObject->get("params"));
    errors->push();
    protocol::Value* objectIdValue = object ? object->get("objectId") : nullptr;
    errors->setName("objectId");
    String16 in_objectId = ValueConversions<String16>::parse(objectIdValue, errors);
    protocol::Value* objectGroupValue = object ? object->get("objectGroup") : nullptr;
    Maybe<String16> in_objectGroup;
    if (objectGroupValue) {
        errors->setName("objectGroup");
        in_objectGroup = ValueConversions<String16>::parse(objectGroupValue, errors);
    }
    errors->pop();
    if (errors->hasErrors()) {
        reportProtocolError(callId, InvalidParams, kInvalidRequest, errors);
        return;
    }
    // Declare output parameters.
    std::unique_ptr<protocol::DictionaryValue> result = DictionaryValue::create();
    std::unique_ptr<protocol::Runtime::RemoteObject> out_result;

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    ErrorString error;
    m_backend->getObjectByHeapObjectId(&error, in_objectId, in_objectGroup, &out_result);
    if (!error.length()) {
        result->setValue("result", ValueConversions<protocol::Runtime::RemoteObject>::serialize(out_result.get()));
    }
    if (weak->get())
        weak->get()->sendResponse(callId, error, std::move(result));
}

void DispatcherImpl::addInspectedHeapObject(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{
    // Prepare input parameters.
    protocol::DictionaryValue* object = DictionaryValue::cast(requestMessageObject->get("params"));
    errors->push();
    protocol::Value* heapObjectIdValue = object ? object->get("heapObjectId") : nullptr;
    errors->setName("heapObjectId");
    String16 in_heapObjectId = ValueConversions<String16>::parse(heapObjectIdValue, errors);
    errors->pop();
    if (errors->hasErrors()) {
        reportProtocolError(callId, InvalidParams, kInvalidRequest, errors);
        return;
    }

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    ErrorString error;
    m_backend->addInspectedHeapObject(&error, in_heapObjectId);
    if (weak->get())
        weak->get()->sendResponse(callId, error);
}

void DispatcherImpl::getHeapObjectId(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{
    // Prepare input parameters.
    protocol::DictionaryValue* object = DictionaryValue::cast(requestMessageObject->get("params"));
    errors->push();
    protocol::Value* objectIdValue = object ? object->get("objectId") : nullptr;
    errors->setName("objectId");
    String16 in_objectId = ValueConversions<String16>::parse(objectIdValue, errors);
    errors->pop();
    if (errors->hasErrors()) {
        reportProtocolError(callId, InvalidParams, kInvalidRequest, errors);
        return;
    }
    // Declare output parameters.
    std::unique_ptr<protocol::DictionaryValue> result = DictionaryValue::create();
    String16 out_heapSnapshotObjectId;

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    ErrorString error;
    m_backend->getHeapObjectId(&error, in_objectId, &out_heapSnapshotObjectId);
    if (!error.length()) {
        result->setValue("heapSnapshotObjectId", ValueConversions<String16>::serialize(out_heapSnapshotObjectId));
    }
    if (weak->get())
        weak->get()->sendResponse(callId, error, std::move(result));
}

void DispatcherImpl::startSampling(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{
    // Prepare input parameters.
    protocol::DictionaryValue* object = DictionaryValue::cast(requestMessageObject->get("params"));
    errors->push();
    protocol::Value* samplingIntervalValue = object ? object->get("samplingInterval") : nullptr;
    Maybe<double> in_samplingInterval;
    if (samplingIntervalValue) {
        errors->setName("samplingInterval");
        in_samplingInterval = ValueConversions<double>::parse(samplingIntervalValue, errors);
    }
    errors->pop();
    if (errors->hasErrors()) {
        reportProtocolError(callId, InvalidParams, kInvalidRequest, errors);
        return;
    }

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    ErrorString error;
    m_backend->startSampling(&error, in_samplingInterval);
    if (weak->get())
        weak->get()->sendResponse(callId, error);
}

void DispatcherImpl::stopSampling(int callId, std::unique_ptr<DictionaryValue> requestMessageObject, ErrorSupport* errors)
{
    // Declare output parameters.
    std::unique_ptr<protocol::DictionaryValue> result = DictionaryValue::create();
    std::unique_ptr<protocol::HeapProfiler::SamplingHeapProfile> out_profile;

    std::unique_ptr<DispatcherBase::WeakPtr> weak = weakPtr();
    ErrorString error;
    m_backend->stopSampling(&error, &out_profile);
    if (!error.length()) {
        result->setValue("profile", ValueConversions<protocol::HeapProfiler::SamplingHeapProfile>::serialize(out_profile.get()));
    }
    if (weak->get())
        weak->get()->sendResponse(callId, error, std::move(result));
}

// static
void Dispatcher::wire(UberDispatcher* dispatcher, Backend* backend)
{
    dispatcher->registerBackend("HeapProfiler", wrapUnique(new DispatcherImpl(dispatcher->channel(), backend)));
}

} // HeapProfiler
} // namespace protocol
} // namespace blink
