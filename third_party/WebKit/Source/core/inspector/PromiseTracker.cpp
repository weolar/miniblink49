// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/inspector/PromiseTracker.h"

#include "bindings/core/v8/ScriptCallStackFactory.h"
#include "bindings/core/v8/ScriptState.h"
#include "bindings/core/v8/ScriptValue.h"
#include "core/inspector/ScriptAsyncCallStack.h"
#include "wtf/CurrentTime.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/WeakPtr.h"

using blink::TypeBuilder::Array;
using blink::TypeBuilder::Console::CallFrame;
using blink::TypeBuilder::Debugger::PromiseDetails;

namespace blink {

class PromiseTracker::PromiseWeakCallbackData final {
    WTF_MAKE_NONCOPYABLE(PromiseWeakCallbackData);
public:
    PromiseWeakCallbackData(PromiseTracker* tracker, int id)
        : m_tracker(tracker->m_weakPtrFactory.createWeakPtr())
        , m_id(id)
    {
    }

    ~PromiseWeakCallbackData()
    {
        if (!m_tracker)
            return;
        RefPtr<PromiseDetails> promiseDetails = PromiseDetails::create().setId(m_id);
        m_tracker->m_listener->didUpdatePromise(InspectorFrontend::Debugger::EventType::Gc, promiseDetails.release());
    }

    WeakPtr<PromiseTracker> m_tracker;
    int m_id;
};

PromiseTracker::IdToPromiseMapTraits::WeakCallbackDataType* PromiseTracker::IdToPromiseMapTraits::WeakCallbackParameter(MapType* map, int key, v8::Local<v8::Object>& value)
{
    // This method is called when promise is added into the map, hence the map must be alive at this point. The tracker in turn must be alive too.
    PromiseTracker* tracker = reinterpret_cast<PromiseTracker*>(reinterpret_cast<intptr_t>(map) - offsetof(PromiseTracker, m_idToPromise));
    return new PromiseWeakCallbackData(tracker, key);
}

void PromiseTracker::IdToPromiseMapTraits::DisposeCallbackData(WeakCallbackDataType* callbackData)
{
    delete callbackData;
}

void PromiseTracker::IdToPromiseMapTraits::DisposeWeak(const v8::WeakCallbackInfo<WeakCallbackDataType>& data)
{
    delete data.GetParameter();
}

PromiseTracker::IdToPromiseMapTraits::MapType* PromiseTracker::IdToPromiseMapTraits::MapFromWeakCallbackInfo(const v8::WeakCallbackInfo<WeakCallbackDataType>& info)
{
    return &info.GetParameter()->m_tracker->m_idToPromise;
}

int PromiseTracker::IdToPromiseMapTraits::KeyFromWeakCallbackInfo(const v8::WeakCallbackInfo<WeakCallbackDataType>& info)
{
    return info.GetParameter()->m_id;
}

PromiseTracker::PromiseTracker(Listener* listener, v8::Isolate* isolate)
    : m_circularSequentialId(0)
    , m_isEnabled(false)
    , m_captureStacks(false)
    , m_listener(listener)
    , m_isolate(isolate)
    , m_weakPtrFactory(this)
    , m_idToPromise(isolate)
{
    clear();
}

PromiseTracker::~PromiseTracker()
{
}

DEFINE_TRACE(PromiseTracker)
{
#if ENABLE(OILPAN)
    visitor->trace(m_listener);
#endif
}

void PromiseTracker::setEnabled(bool enabled, bool captureStacks)
{
    m_isEnabled = enabled;
    m_captureStacks = captureStacks;
    if (!enabled)
        clear();
}

void PromiseTracker::clear()
{
    v8::HandleScope scope(m_isolate);
    m_promiseToId.Reset(m_isolate, v8::NativeWeakMap::New(m_isolate));
    m_idToPromise.Clear();
}

int PromiseTracker::circularSequentialId()
{
    ++m_circularSequentialId;
    if (m_circularSequentialId <= 0)
        m_circularSequentialId = 1;
    return m_circularSequentialId;
}

int PromiseTracker::promiseId(v8::Local<v8::Object> promise, bool* isNewPromise)
{
    v8::HandleScope scope(m_isolate);
    v8::Local<v8::NativeWeakMap> map = v8::Local<v8::NativeWeakMap>::New(m_isolate, m_promiseToId);
    v8::Local<v8::Value> value = map->Get(promise);
    if (value->IsInt32()) {
        *isNewPromise = false;
        return value.As<v8::Int32>()->Value();
    }
    *isNewPromise = true;
    int id = circularSequentialId();
    map->Set(promise, v8::Int32::New(m_isolate, id));
    m_idToPromise.Set(id, promise);
    return id;
}

void PromiseTracker::didReceiveV8PromiseEvent(ScriptState* scriptState, v8::Local<v8::Object> promise, v8::Local<v8::Value> parentPromise, int status)
{
    ASSERT(isEnabled());
    ASSERT(scriptState->contextIsValid());

    bool isNewPromise = false;
    int id = promiseId(promise, &isNewPromise);

    ScriptState::Scope scope(scriptState);
    InspectorFrontend::Debugger::EventType::Enum eventType = isNewPromise ? InspectorFrontend::Debugger::EventType::New : InspectorFrontend::Debugger::EventType::Update;

    PromiseDetails::Status::Enum promiseStatus;
    switch (status) {
    case 0:
        promiseStatus = PromiseDetails::Status::Pending;
        break;
    case 1:
        promiseStatus = PromiseDetails::Status::Resolved;
        break;
    default:
        promiseStatus = PromiseDetails::Status::Rejected;
    };
    RefPtr<PromiseDetails> promiseDetails = PromiseDetails::create().setId(id);
    promiseDetails->setStatus(promiseStatus);

    if (!parentPromise.IsEmpty() && parentPromise->IsObject()) {
        v8::Local<v8::Object> handle = parentPromise->ToObject(scriptState->isolate());
        bool parentIsNewPromise = false;
        int parentPromiseId = promiseId(handle, &parentIsNewPromise);
        promiseDetails->setParentId(parentPromiseId);
    } else {
        if (!status) {
            if (isNewPromise) {
                promiseDetails->setCreationTime(currentTimeMS());
                RefPtrWillBeRawPtr<ScriptCallStack> stack = createScriptCallStack(m_captureStacks ? ScriptCallStack::maxCallStackSizeToCapture : 1, true);
                if (stack && stack->size()) {
                    promiseDetails->setCallFrame(stack->at(0).buildInspectorObject());
                    if (m_captureStacks) {
                        promiseDetails->setCreationStack(stack->buildInspectorArray());
                        RefPtrWillBeRawPtr<ScriptAsyncCallStack> asyncCallStack = stack->asyncCallStack();
                        if (asyncCallStack)
                            promiseDetails->setAsyncCreationStack(asyncCallStack->buildInspectorObject());
                    }
                }
            }
        } else {
            promiseDetails->setSettlementTime(currentTimeMS());
            if (m_captureStacks) {
                RefPtrWillBeRawPtr<ScriptCallStack> stack = createScriptCallStack(ScriptCallStack::maxCallStackSizeToCapture, true);
                if (stack && stack->size()) {
                    promiseDetails->setSettlementStack(stack->buildInspectorArray());
                    RefPtrWillBeRawPtr<ScriptAsyncCallStack> asyncCallStack = stack->asyncCallStack();
                    if (asyncCallStack)
                        promiseDetails->setAsyncSettlementStack(asyncCallStack->buildInspectorObject());
                }
            }
        }
    }

    m_listener->didUpdatePromise(eventType, promiseDetails.release());
}

ScriptValue PromiseTracker::promiseById(int promiseId)
{
    ASSERT(isEnabled());
    v8::HandleScope scope(m_isolate);
    v8::Local<v8::Object> value = m_idToPromise.Get(promiseId);
    if (value.IsEmpty())
        return ScriptValue();
    return ScriptValue(ScriptState::from(value->CreationContext()) , value);
}

} // namespace blink
