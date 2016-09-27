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

#ifndef InspectorHeapProfilerAgent_h
#define InspectorHeapProfilerAgent_h

#include "core/CoreExport.h"
#include "core/InspectorFrontend.h"
#include "core/inspector/InspectorBaseAgent.h"
#include "wtf/Forward.h"
#include "wtf/HashMap.h"
#include "wtf/Noncopyable.h"
#include "wtf/OwnPtr.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/text/WTFString.h"

namespace blink {

class InjectedScriptManager;
class HeapStatsUpdateTask;

typedef String ErrorString;

class CORE_EXPORT InspectorHeapProfilerAgent final : public InspectorBaseAgent<InspectorHeapProfilerAgent, InspectorFrontend::HeapProfiler>, public InspectorBackendDispatcher::HeapProfilerCommandHandler {
    WTF_MAKE_NONCOPYABLE(InspectorHeapProfilerAgent);
    WTF_MAKE_FAST_ALLOCATED_WILL_BE_REMOVED(InspectorHeapProfilerAgent);
public:
    static PassOwnPtrWillBeRawPtr<InspectorHeapProfilerAgent> create(InjectedScriptManager*);
    virtual ~InspectorHeapProfilerAgent();
    DECLARE_VIRTUAL_TRACE();

    virtual void collectGarbage(ErrorString*) override;

    virtual void enable(ErrorString*) override;
    virtual void startTrackingHeapObjects(ErrorString*, const bool* trackAllocations) override;
    virtual void stopTrackingHeapObjects(ErrorString*, const bool* reportProgress) override;

    void disable(ErrorString*) override;
    void restore() override;

    virtual void takeHeapSnapshot(ErrorString*, const bool* reportProgress) override;

    virtual void getObjectByHeapObjectId(ErrorString*, const String& heapSnapshotObjectId, const String* objectGroup, RefPtr<TypeBuilder::Runtime::RemoteObject>& result) override;
    virtual void addInspectedHeapObject(ErrorString*, const String& inspectedHeapObjectId) override;
    virtual void getHeapObjectId(ErrorString*, const String& objectId, String* heapSnapshotObjectId) override;

private:
    class HeapStatsStream;
    class HeapStatsUpdateTask;

    explicit InspectorHeapProfilerAgent(InjectedScriptManager*);

    void requestHeapStatsUpdate();
    void pushHeapStatsUpdate(const uint32_t* const data, const int size);

    void startTrackingHeapObjectsInternal(bool trackAllocations);
    void stopTrackingHeapObjectsInternal();

    RawPtrWillBeMember<InjectedScriptManager> m_injectedScriptManager;
    OwnPtrWillBeMember<HeapStatsUpdateTask> m_heapStatsUpdateTask;
};

} // namespace blink


#endif // !defined(InspectorHeapProfilerAgent_h)
