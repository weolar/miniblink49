/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/inspector/InspectorHeapProfilerAgent.h"

#include "bindings/core/v8/ScriptProfiler.h"
#include "core/inspector/InjectedScript.h"
#include "core/inspector/InjectedScriptHost.h"
#include "core/inspector/InspectorState.h"
#include "platform/Timer.h"
#include "wtf/CurrentTime.h"

namespace blink {

typedef uint32_t SnapshotObjectId;

namespace HeapProfilerAgentState {
static const char heapProfilerEnabled[] = "heapProfilerEnabled";
static const char heapObjectsTrackingEnabled[] = "heapObjectsTrackingEnabled";
static const char allocationTrackingEnabled[] = "allocationTrackingEnabled";
}

class InspectorHeapProfilerAgent::HeapStatsUpdateTask final : public NoBaseWillBeGarbageCollectedFinalized<InspectorHeapProfilerAgent::HeapStatsUpdateTask> {
public:
    explicit HeapStatsUpdateTask(InspectorHeapProfilerAgent*);
    void startTimer();
    void resetTimer() { m_timer.stop(); }
    void onTimer(Timer<HeapStatsUpdateTask>*);
    DECLARE_TRACE();

private:
    RawPtrWillBeMember<InspectorHeapProfilerAgent> m_heapProfilerAgent;
    Timer<HeapStatsUpdateTask> m_timer;
};

PassOwnPtrWillBeRawPtr<InspectorHeapProfilerAgent> InspectorHeapProfilerAgent::create(InjectedScriptManager* injectedScriptManager)
{
    return adoptPtrWillBeNoop(new InspectorHeapProfilerAgent(injectedScriptManager));
}

InspectorHeapProfilerAgent::InspectorHeapProfilerAgent(InjectedScriptManager* injectedScriptManager)
    : InspectorBaseAgent<InspectorHeapProfilerAgent, InspectorFrontend::HeapProfiler>("HeapProfiler")
    , m_injectedScriptManager(injectedScriptManager)
{
}

InspectorHeapProfilerAgent::~InspectorHeapProfilerAgent()
{
}

void InspectorHeapProfilerAgent::restore()
{
    if (m_state->getBoolean(HeapProfilerAgentState::heapProfilerEnabled))
        frontend()->resetProfiles();
    if (m_state->getBoolean(HeapProfilerAgentState::heapObjectsTrackingEnabled))
        startTrackingHeapObjectsInternal(m_state->getBoolean(HeapProfilerAgentState::allocationTrackingEnabled));
}

void InspectorHeapProfilerAgent::collectGarbage(ErrorString*)
{
    ScriptProfiler::collectGarbage();
}

InspectorHeapProfilerAgent::HeapStatsUpdateTask::HeapStatsUpdateTask(InspectorHeapProfilerAgent* heapProfilerAgent)
    : m_heapProfilerAgent(heapProfilerAgent)
    , m_timer(this, &HeapStatsUpdateTask::onTimer)
{
}

void InspectorHeapProfilerAgent::HeapStatsUpdateTask::onTimer(Timer<HeapStatsUpdateTask>*)
{
    // The timer is stopped on m_heapProfilerAgent destruction,
    // so this method will never be called after m_heapProfilerAgent has been destroyed.
    m_heapProfilerAgent->requestHeapStatsUpdate();
}

void InspectorHeapProfilerAgent::HeapStatsUpdateTask::startTimer()
{
    ASSERT(!m_timer.isActive());
    m_timer.startRepeating(0.05, FROM_HERE);
}

DEFINE_TRACE(InspectorHeapProfilerAgent::HeapStatsUpdateTask)
{
    visitor->trace(m_heapProfilerAgent);
}

class InspectorHeapProfilerAgent::HeapStatsStream final : public ScriptProfiler::OutputStream {
public:
    HeapStatsStream(InspectorHeapProfilerAgent* heapProfilerAgent)
        : m_heapProfilerAgent(heapProfilerAgent)
    {
    }

    virtual void write(const uint32_t* chunk, const int size) override
    {
        ASSERT(chunk);
        ASSERT(size > 0);
        m_heapProfilerAgent->pushHeapStatsUpdate(chunk, size);
    }
private:
    InspectorHeapProfilerAgent* m_heapProfilerAgent;
};

void InspectorHeapProfilerAgent::startTrackingHeapObjects(ErrorString*, const bool* trackAllocations)
{
    m_state->setBoolean(HeapProfilerAgentState::heapObjectsTrackingEnabled, true);
    bool allocationTrackingEnabled = asBool(trackAllocations);
    m_state->setBoolean(HeapProfilerAgentState::allocationTrackingEnabled, allocationTrackingEnabled);
    startTrackingHeapObjectsInternal(allocationTrackingEnabled);
}

void InspectorHeapProfilerAgent::requestHeapStatsUpdate()
{
    if (!frontend())
        return;
    HeapStatsStream stream(this);
    SnapshotObjectId lastSeenObjectId = ScriptProfiler::requestHeapStatsUpdate(&stream);
    frontend()->lastSeenObjectId(lastSeenObjectId, WTF::currentTimeMS());
}

void InspectorHeapProfilerAgent::pushHeapStatsUpdate(const uint32_t* const data, const int size)
{
    if (!frontend())
        return;
    RefPtr<TypeBuilder::Array<int> > statsDiff = TypeBuilder::Array<int>::create();
    for (int i = 0; i < size; ++i)
        statsDiff->addItem(data[i]);
    frontend()->heapStatsUpdate(statsDiff.release());
}

void InspectorHeapProfilerAgent::stopTrackingHeapObjects(ErrorString* error, const bool* reportProgress)
{
    if (!m_heapStatsUpdateTask) {
        *error = "Heap object tracking is not started.";
        return;
    }
    requestHeapStatsUpdate();
    takeHeapSnapshot(error, reportProgress);
    stopTrackingHeapObjectsInternal();
}

void InspectorHeapProfilerAgent::startTrackingHeapObjectsInternal(bool trackAllocations)
{
    if (m_heapStatsUpdateTask)
        return;
    ScriptProfiler::startTrackingHeapObjects(trackAllocations);
    m_heapStatsUpdateTask = adoptPtrWillBeNoop(new HeapStatsUpdateTask(this));
    m_heapStatsUpdateTask->startTimer();
}

void InspectorHeapProfilerAgent::stopTrackingHeapObjectsInternal()
{
    if (!m_heapStatsUpdateTask)
        return;
    ScriptProfiler::stopTrackingHeapObjects();
    m_heapStatsUpdateTask->resetTimer();
    m_heapStatsUpdateTask.clear();
    m_state->setBoolean(HeapProfilerAgentState::heapObjectsTrackingEnabled, false);
    m_state->setBoolean(HeapProfilerAgentState::allocationTrackingEnabled, false);
}

void InspectorHeapProfilerAgent::enable(ErrorString*)
{
    m_state->setBoolean(HeapProfilerAgentState::heapProfilerEnabled, true);
}

void InspectorHeapProfilerAgent::disable(ErrorString* error)
{
    stopTrackingHeapObjectsInternal();
    ScriptProfiler::clearHeapObjectIds();
    m_state->setBoolean(HeapProfilerAgentState::heapProfilerEnabled, false);
}

void InspectorHeapProfilerAgent::takeHeapSnapshot(ErrorString* errorString, const bool* reportProgress)
{
    class HeapSnapshotProgress final : public ScriptProfiler::HeapSnapshotProgress {
    public:
        explicit HeapSnapshotProgress(InspectorFrontend::HeapProfiler* frontend)
            : m_frontend(frontend)
            , m_totalWork(0) { }
        virtual void Start(int totalWork) override
        {
            m_totalWork = totalWork;
        }
        virtual void Worked(int workDone) override
        {
            if (m_frontend) {
                m_frontend->reportHeapSnapshotProgress(workDone, m_totalWork, 0);
                m_frontend->flush();
            }
        }
        virtual void Done() override
        {
            const bool finished = true;
            if (m_frontend) {
                m_frontend->reportHeapSnapshotProgress(m_totalWork, m_totalWork, &finished);
                m_frontend->flush();
            }
        }
        virtual bool isCanceled() override { return false; }
    private:
        InspectorFrontend::HeapProfiler* m_frontend;
        int m_totalWork;
    };

    HeapSnapshotProgress progress(asBool(reportProgress) ? frontend() : 0);
    RefPtr<ScriptHeapSnapshot> snapshot = ScriptProfiler::takeHeapSnapshot(&progress);
    if (!snapshot) {
        *errorString = "Failed to take heap snapshot";
        return;
    }

    class OutputStream : public ScriptHeapSnapshot::OutputStream {
    public:
        explicit OutputStream(InspectorFrontend::HeapProfiler* frontend)
            : m_frontend(frontend) { }
        void Write(const String& chunk)
        {
            m_frontend->addHeapSnapshotChunk(chunk);
            m_frontend->flush();
        }
        void Close() { }
    private:
        InspectorFrontend::HeapProfiler* m_frontend;
    };

    if (frontend()) {
        OutputStream stream(frontend());
        snapshot->writeJSON(&stream);
    }
}

void InspectorHeapProfilerAgent::getObjectByHeapObjectId(ErrorString* error, const String& heapSnapshotObjectId, const String* objectGroup, RefPtr<TypeBuilder::Runtime::RemoteObject>& result)
{
    bool ok;
    unsigned id = heapSnapshotObjectId.toUInt(&ok);
    if (!ok) {
        *error = "Invalid heap snapshot object id";
        return;
    }
    ScriptValue heapObject = ScriptProfiler::objectByHeapObjectId(id);
    if (heapObject.isEmpty()) {
        *error = "Object is not available";
        return;
    }
    InjectedScript injectedScript = m_injectedScriptManager->injectedScriptFor(heapObject.scriptState());
    if (injectedScript.isEmpty()) {
        *error = "Object is not available. Inspected context is gone";
        return;
    }
    result = injectedScript.wrapObject(heapObject, objectGroup ? *objectGroup : "");
    if (!result)
        *error = "Failed to wrap object";
}

class InspectableHeapObject final : public InjectedScriptHost::InspectableObject {
public:
    explicit InspectableHeapObject(unsigned heapObjectId) : m_heapObjectId(heapObjectId) { }
    virtual ScriptValue get(ScriptState*) override
    {
        return ScriptProfiler::objectByHeapObjectId(m_heapObjectId);
    }
private:
    unsigned m_heapObjectId;
};

void InspectorHeapProfilerAgent::addInspectedHeapObject(ErrorString* errorString, const String& inspectedHeapObjectId)
{
    bool ok;
    unsigned id = inspectedHeapObjectId.toUInt(&ok);
    if (!ok) {
        *errorString = "Invalid heap snapshot object id";
        return;
    }
    m_injectedScriptManager->injectedScriptHost()->addInspectedObject(adoptPtr(new InspectableHeapObject(id)));
}

void InspectorHeapProfilerAgent::getHeapObjectId(ErrorString* errorString, const String& objectId, String* heapSnapshotObjectId)
{
    InjectedScript injectedScript = m_injectedScriptManager->injectedScriptForObjectId(objectId);
    if (injectedScript.isEmpty()) {
        *errorString = "Inspected context has gone";
        return;
    }
    ScriptValue value = injectedScript.findObjectById(objectId);
    ScriptState::Scope scope(injectedScript.scriptState());
    if (value.isEmpty() || value.isUndefined()) {
        *errorString = "Object with given id not found";
        return;
    }
    unsigned id = ScriptProfiler::getHeapObjectId(value);
    *heapSnapshotObjectId = String::number(id);
}

DEFINE_TRACE(InspectorHeapProfilerAgent)
{
    visitor->trace(m_injectedScriptManager);
    visitor->trace(m_heapStatsUpdateTask);
    InspectorBaseAgent::trace(visitor);
}

} // namespace blink

