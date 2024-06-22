/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
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
#include "bindings/core/v8/V8GCController.h"

#include "bindings/core/v8/RetainedDOMInfo.h"
#include "bindings/core/v8/V8AbstractEventListener.h"
#include "bindings/core/v8/V8Binding.h"
#include "bindings/core/v8/V8MutationObserver.h"
#include "bindings/core/v8/V8Node.h"
#include "bindings/core/v8/V8ScriptRunner.h"
#include "bindings/core/v8/WrapperTypeInfo.h"
#include "bindings/core/v8/ScopedPersistent.h"
#include "core/dom/Attr.h"
#include "core/dom/Document.h"
#include "core/dom/NodeTraversal.h"
#include "core/dom/TemplateContentDocumentFragment.h"
#include "core/dom/shadow/ElementShadow.h"
#include "core/dom/shadow/ShadowRoot.h"
#include "core/html/HTMLImageElement.h"
#include "core/html/HTMLTemplateElement.h"
#include "core/html/imports/HTMLImportsController.h"
#include "core/inspector/InspectorTraceEvents.h"
#include "core/svg/SVGElement.h"
#include "platform/TraceEvent.h"
#include "wtf/Partitions.h"
#include "wtf/Vector.h"
#include <algorithm>

#if V8_MAJOR_VERSION >= 5

namespace blink {

// FIXME: This should use opaque GC roots.
static void addReferencesForNodeWithEventListeners(v8::Isolate* isolate, Node* node, const v8::Persistent<v8::Object>& wrapper)
{
    ASSERT(node->hasEventListeners());

#if V8_MAJOR_VERSION >= 7
    Node* root = V8GCController::opaqueRootForGC(isolate, node);
    if (!root->isDocumentNode())
        return; // weolar: vue发现的内存泄露,见W:\test\web_test\yunci\ru2\dist\assets\RecommenSoft-7ac9c6d1.js

    v8::EmbedderHeapTracer* tracer = V8PerIsolateData::from(isolate)->getEmbedderHeapTracer(isolate);
    std::vector<std::pair<void*, void*> > refs;           
#endif

    EventListenerIterator iterator(node);

    while (EventListener* listener = iterator.nextListener()) {
        if (listener->type() != EventListener::JSEventListenerType)
            continue;
        V8AbstractEventListener* v8listener = static_cast<V8AbstractEventListener*>(listener);
        if (!v8listener->hasExistingListenerObject())
            continue;

        isolate->SetReference(wrapper, v8::Persistent<v8::Value>::Cast(v8listener->existingListenerObjectPersistentHandle()));
#if V8_MAJOR_VERSION >= 7
        v8::Persistent<v8::Value>* newPersistent = new v8::Persistent<v8::Value>();
        newPersistent->Reset(isolate, v8listener->existingListenerObjectPersistentHandle());
        refs.push_back(std::pair<void*, void*>((void*)newPersistent, nullptr));
// 
//         char* output = (char*)malloc(0x100);
//         sprintf_s(output, 0x99, "addReferencesForNodeWithEventListeners, listener: node:%p, listener:%p\n", node, listener);
//         OutputDebugStringA(output);
//         free(output);
#endif
    }

#if V8_MAJOR_VERSION >= 7
    tracer->RegisterV8References(refs);
#endif
}

Node* V8GCController::opaqueRootForGC(v8::Isolate*, Node* node)
{
    ASSERT(node);
    // FIXME: Remove the special handling for image elements.
    // The same special handling is in V8GCController::gcTree().
    // Maybe should image elements be active DOM nodes?
    // See https://code.google.com/p/chromium/issues/detail?id=164882
    if (node->inDocument() || (isHTMLImageElement(*node) && toHTMLImageElement(*node).hasPendingActivity())) {
        Document& document = node->document();
        if (HTMLImportsController* controller = document.importsController())
            return controller->master();
        return &document;
    }

    if (node->isAttributeNode()) {
        Node* ownerElement = toAttr(node)->ownerElement();
        if (!ownerElement)
            return node;
        node = ownerElement;
    }

    while (Node* parent = node->parentOrShadowHostOrTemplateHostNode())
        node = parent;

    return node;
}

class MinorGCUnmodifiedWrapperVisitor : public v8::PersistentHandleVisitor {
public:
    explicit MinorGCUnmodifiedWrapperVisitor(v8::Isolate* isolate)
        : m_isolate(isolate) {}

    void VisitPersistentHandle(v8::Persistent<v8::Value>* value, uint16_t classId) override
    {
        if (classId != WrapperTypeInfo::NodeClassId && classId != WrapperTypeInfo::ObjectClassId)
            return;

        // MinorGC does not collect objects because it may be expensive to
        // update references during minorGC
        if (classId == WrapperTypeInfo::ObjectClassId) {
            v8::Persistent<v8::Object>::Cast(*value).MarkActive();
            return;
        }

        v8::Local<v8::Object> wrapper = v8::Local<v8::Object>::New(m_isolate, v8::Persistent<v8::Object>::Cast(*value));
        ASSERT(V8DOMWrapper::hasInternalFieldsSet(wrapper));

        const WrapperTypeInfo* type = toWrapperTypeInfo(wrapper);
        ActiveDOMObject* activeDOMObject = type->toActiveDOMObject(wrapper);
        if (activeDOMObject && activeDOMObject->hasPendingActivity()) {
            v8::Persistent<v8::Object>::Cast(*value).MarkActive();
            return;
        }

        if (classId == WrapperTypeInfo::NodeClassId) {
            ASSERT(V8Node::hasInstance(wrapper, m_isolate));
            Node* node = V8Node::toImpl(wrapper);
            if (node->hasEventListeners()) {
                v8::Persistent<v8::Object>::Cast(*value).MarkActive();
                return;
            }
            // FIXME: Remove the special handling for SVG elements.
            // We currently can't collect SVG Elements from minor gc, as we have
            // strong references from SVG property tear-offs keeping context SVG
            // element alive.
            if (node->isSVGElement()) {
                v8::Persistent<v8::Object>::Cast(*value).MarkActive();
                return;
            }
        }
    }

private:
    v8::Isolate* m_isolate;
};

class MajorGCWrapperVisitor : public v8::PersistentHandleVisitor {
public:
    explicit MajorGCWrapperVisitor(v8::Isolate* isolate, bool constructRetainedObjectInfos)
        : m_isolate(isolate)
        , m_domObjectsWithPendingActivity(0)
        , m_liveRootGroupIdSet(false)
        , m_constructRetainedObjectInfos(constructRetainedObjectInfos)
    {
    }

    void VisitPersistentHandle(v8::Persistent<v8::Value>* value, uint16_t classId) override
    {
#if V8_MAJOR_VERSION >= 7
        v8::EmbedderHeapTracer* tracer = V8PerIsolateData::from(m_isolate)->getEmbedderHeapTracer(m_isolate);
#endif
        if (classId != WrapperTypeInfo::NodeClassId && classId != WrapperTypeInfo::ObjectClassId)
            return;

#if V8_MAJOR_VERSION < 7
        if (value->IsIndependent())
            return;
#endif

        v8::Local<v8::Object> wrapper = v8::Local<v8::Object>::New(m_isolate, v8::Persistent<v8::Object>::Cast(*value));
        ASSERT(V8DOMWrapper::hasInternalFieldsSet(wrapper));

        const WrapperTypeInfo* type = toWrapperTypeInfo(wrapper);

        ActiveDOMObject* activeDOMObject = type->toActiveDOMObject(wrapper);
        if (activeDOMObject && activeDOMObject->hasPendingActivity()) {
            // Enable hasPendingActivity only when the associated
            // ExecutionContext is not yet detached. This is a work-around
            // to avoid memory leaks caused by hasPendingActivity that keeps
            // returning true forever. This will be okay in practice because
            // the spec requires to stop almost all DOM activities when the
            // associated browsing context is detached. However, the real
            // problem is that some hasPendingActivity's are wrongly implemented
            // and never return false.
            // TODO(haraken): Implement correct lifetime using traceWrapper.
            ExecutionContext* context = toExecutionContext(wrapper->CreationContext());
            if (context/* && !context->isContextDestroyed()*/) {
                m_isolate->SetObjectGroupId(*value, liveRootId());
                ++m_domObjectsWithPendingActivity;
            }

#if V8_MAJOR_VERSION >= 7
            ScriptWrappable* scriptWrap = toScriptWrappable(wrapper);

            // 这里调用RegisterEmbedderReference无效，要在v8::EmbedderHeapTracer::AdvanceTracing回调里调用才行
            std::vector<std::pair<void*, void*>> refs;
            if (type->ginEmbedder == gin::GinEmbedder::kEmbedderBlink && scriptWrap) {
                v8::Persistent<v8::Value>* newPersistent = new v8::Persistent<v8::Value>();
                newPersistent->Reset(m_isolate, *value);

                //v8::TracedGlobal<v8::Value>* newPersistent = new v8::TracedGlobal<v8::Value>(m_isolate, wrapper);

                refs.push_back(std::pair<void*, void*>((void*)newPersistent, nullptr));
                tracer->RegisterV8References(refs);
            }

//             char* output = (char*)malloc(0x100);
//             sprintf_s(output, 0x99, "MajorGCWrapperVisitor.VisitPersistentHandle: %p %p\n", activeDOMObject, scriptWrap);
//             OutputDebugStringA(output);
//             free(output);
#endif
        }

        if (classId == WrapperTypeInfo::NodeClassId) {
            ASSERT(V8Node::hasInstance(wrapper, m_isolate));
            Node* node = V8Node::toImpl(wrapper);
            if (node->hasEventListeners())
                addReferencesForNodeWithEventListeners(m_isolate, node, v8::Persistent<v8::Object>::Cast(*value));

            Node* root = V8GCController::opaqueRootForGC(m_isolate, node);
            m_isolate->SetObjectGroupId(*value, v8::UniqueId(reinterpret_cast<intptr_t>(root)));

            if (m_constructRetainedObjectInfos)
                m_groupsWhichNeedRetainerInfo.append(root);
        } else if (classId == WrapperTypeInfo::ObjectClassId) {
            if (true/*!RuntimeEnabledFeatures::traceWrappablesEnabled()*/)
                type->visitDOMWrapper(m_isolate, toScriptWrappable(wrapper), v8::Persistent<v8::Object>::Cast(*value));
        } else {
            DebugBreak();
        }

        //OutputDebugStringA("MajorGCWrapperVisitor.VisitPersistentHandle 2\n");
    }

    void notifyFinished()
    {
        if (!m_constructRetainedObjectInfos)
            return;
        std::sort(m_groupsWhichNeedRetainerInfo.begin(), m_groupsWhichNeedRetainerInfo.end());

        Node* alreadyAdded = 0;
        v8::HeapProfiler* profiler = m_isolate->GetHeapProfiler();
        for (size_t i = 0; i < m_groupsWhichNeedRetainerInfo.size(); ++i) {
            Node* root = m_groupsWhichNeedRetainerInfo[i];
            if (root != alreadyAdded) {
#if V8_MAJOR_VERSION < 7
                profiler->SetRetainedObjectInfo(v8::UniqueId(reinterpret_cast<intptr_t>(root)), new RetainedDOMInfo(root));
#endif
                alreadyAdded = root;
            }
        }
        if (m_liveRootGroupIdSet) {
#if V8_MAJOR_VERSION < 7
            profiler->SetRetainedObjectInfo(liveRootId(), new ActiveDOMObjectsInfo(m_domObjectsWithPendingActivity));
#endif
        }
    }

private:
    v8::UniqueId liveRootId()
    {
        const v8::Persistent<v8::Value>& liveRoot = V8PerIsolateData::from(m_isolate)->ensureLiveRoot();
        const intptr_t* idPointer = reinterpret_cast<const intptr_t*>(&liveRoot);
        v8::UniqueId id(*idPointer);
        if (!m_liveRootGroupIdSet) {
            m_isolate->SetObjectGroupId(liveRoot, id);
            m_liveRootGroupIdSet = true;
            ++m_domObjectsWithPendingActivity;
        }
        return id;
    }

    v8::Isolate* m_isolate;
    // v8 guarantees that Blink will not regain control while a v8 GC runs
    // (=> no Oilpan GCs will be triggered), hence raw, untraced members
    // can safely be kept here.
    WillBePersistentHeapVector<RawPtrWillBeMember<Node>> m_groupsWhichNeedRetainerInfo;
    int m_domObjectsWithPendingActivity;
    bool m_liveRootGroupIdSet;
    bool m_constructRetainedObjectInfos;
};

static unsigned long long usedHeapSize(v8::Isolate* isolate)
{
    v8::HeapStatistics heapStatistics;
    isolate->GetHeapStatistics(&heapStatistics);
    return heapStatistics.used_heap_size();
}

namespace {

void visitWeakHandlesForMinorGC(v8::Isolate* isolate)
{
    MinorGCUnmodifiedWrapperVisitor visitor(isolate);
    isolate->VisitWeakHandles(&visitor);
}

void objectGroupingForMajorGC(v8::Isolate* isolate, bool constructRetainedObjectInfos)
{
    MajorGCWrapperVisitor visitor(isolate, constructRetainedObjectInfos);
    isolate->VisitHandlesWithClassIds(&visitor);
    visitor.notifyFinished();
}

void gcPrologueForMajorGC(v8::Isolate* isolate, bool constructRetainedObjectInfos)
{
    if (true/*!RuntimeEnabledFeatures::traceWrappablesEnabled() || constructRetainedObjectInfos*/)
        objectGroupingForMajorGC(isolate, constructRetainedObjectInfos);
}

}  // namespace

void V8GCController::gcPrologue(
#if V8_MAJOR_VERSION > 5
    v8::Isolate* isolate, v8::GCType type, v8::GCCallbackFlags flags, void* data
#else
    v8::GCType type, v8::GCCallbackFlags flags
#endif
    )
{
//     char* output = (char*)malloc(0x100);
//     sprintf(output, "V8GCController::gcPrologue entry: %d\n", type);
//     OutputDebugStringA(output);
//     free(output);

    if (isMainThread())
        ScriptForbiddenScope::enter();

    // Attribute garbage collection to the all frames instead of a specific
    // frame.
//     if (BlameContext* blameContext = Platform::current()->topLevelBlameContext())
//         blameContext->Enter();

    // TODO(haraken): A GC callback is not allowed to re-enter V8. This means
    // that it's unsafe to run Oilpan's GC in the GC callback because it may
    // run finalizers that call into V8. To avoid the risk, we should post
    // a task to schedule the Oilpan's GC.
    // (In practice, there is no finalizer that calls into V8 and thus is safe.)
#if V8_MAJOR_VERSION <= 5
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
#endif
    v8::HandleScope scope(isolate);
    switch (type) {
    case v8::kGCTypeScavenge:
        TRACE_EVENT_BEGIN1("devtools.timeline,v8", "MinorGC", "usedHeapSizeBefore", usedHeapSize(isolate));
        visitWeakHandlesForMinorGC(isolate);
        break;
    case v8::kGCTypeMarkSweepCompact:
        TRACE_EVENT_BEGIN2("devtools.timeline,v8", "MajorGC", "usedHeapSizeBefore", usedHeapSize(isolate), "type", "atomic pause");
        //OutputDebugStringA("V8GCController::gcPrologue kGCTypeMarkSweepCompact 1\n");
        gcPrologueForMajorGC(isolate, flags & v8::kGCCallbackFlagConstructRetainedObjectInfos);
        //OutputDebugStringA("V8GCController::gcPrologue kGCTypeMarkSweepCompact 2\n");
        break;
    case v8::kGCTypeIncrementalMarking:

        TRACE_EVENT_BEGIN2("devtools.timeline,v8", "MajorGC", "usedHeapSizeBefore", usedHeapSize(isolate), "type", "incremental marking");
        gcPrologueForMajorGC(isolate, flags & v8::kGCCallbackFlagConstructRetainedObjectInfos);
        break;
    case v8::kGCTypeProcessWeakCallbacks:
        TRACE_EVENT_BEGIN2("devtools.timeline,v8", "MajorGC", "usedHeapSizeBefore", usedHeapSize(isolate), "type", "weak processing");
        break;
    default:
        ASSERT_NOT_REACHED();
    }
    //OutputDebugStringA("V8GCController::gcPrologue end\n");
}

namespace {

void UpdateCollectedPhantomHandles(v8::Isolate* isolate) {
//         ThreadHeapStats& heapStats = ThreadState::current()->heap().heapStats();
//         size_t count = isolate->NumberOfPhantomHandleResetsSinceLastCall();
//         heapStats.decreaseWrapperCount(count);
//         heapStats.increaseCollectedWrapperCount(count);
}

}  // namespace

void V8GCController::gcEpilogue(
#if V8_MAJOR_VERSION > 5
    v8::Isolate* isolate, v8::GCType type, v8::GCCallbackFlags flags, void* data
#else
    v8::GCType type, v8::GCCallbackFlags flags
#endif
    )
{
    //OutputDebugStringA("V8GCController::gcEpilogue entry\n");
#if V8_MAJOR_VERSION <= 5
    v8::Isolate* isolate = v8::Isolate::GetCurrent();
#endif
    UpdateCollectedPhantomHandles(isolate);
    switch (type) {
    case v8::kGCTypeScavenge:
        TRACE_EVENT_END1("devtools.timeline,v8", "MinorGC", "usedHeapSizeAfter", usedHeapSize(isolate));
        // TODO(haraken): Remove this. See the comment in gcPrologue.
        break;
    case v8::kGCTypeMarkSweepCompact:
        TRACE_EVENT_END1("devtools.timeline,v8", "MajorGC", "usedHeapSizeAfter", usedHeapSize(isolate));
        break;
    case v8::kGCTypeIncrementalMarking:
        TRACE_EVENT_END1("devtools.timeline,v8", "MajorGC", "usedHeapSizeAfter", usedHeapSize(isolate));
        break;
    case v8::kGCTypeProcessWeakCallbacks:
        TRACE_EVENT_END1("devtools.timeline,v8", "MajorGC", "usedHeapSizeAfter", usedHeapSize(isolate));
        break;
    default:
        ASSERT_NOT_REACHED();
    }

    if (isMainThread())
        ScriptForbiddenScope::exit();

    ThreadState* currentThreadState = ThreadState::current();
    if (currentThreadState && !currentThreadState->isGCForbidden()) {
        // v8::kGCCallbackFlagForced forces a Blink heap garbage collection
        // when a garbage collection was forced from V8. This is either used
        // for tests that force GCs from JavaScript to verify that objects die
        // when expected.
        if (flags & v8::kGCCallbackFlagForced) {
            // This single GC is not enough for two reasons:
            //   (1) The GC is not precise because the GC scans on-stack pointers
            //       conservatively.
            //   (2) One GC is not enough to break a chain of persistent handles. It's
            //       possible that some heap allocated objects own objects that
            //       contain persistent handles pointing to other heap allocated
            //       objects. To break the chain, we need multiple GCs.
            //
            // Regarding (1), we force a precise GC at the end of the current event
            // loop. So if you want to collect all garbage, you need to wait until the
            // next event loop.  Regarding (2), it would be OK in practice to trigger
            // only one GC per gcEpilogue, because GCController.collectAll() forces
            // multiple V8's GC.

            //OutputDebugStringA("V8GCController::gcEpilogue 1\n");
            Heap::collectGarbage(ThreadState::HeapPointersOnStack, ThreadState::GCWithSweep, Heap::ForcedGC);
            //OutputDebugStringA("V8GCController::gcEpilogue 2\n");

            // Forces a precise GC at the end of the current event loop.
            RELEASE_ASSERT(!currentThreadState->isInGC());
            currentThreadState->setGCState(ThreadState::FullGCScheduled);
        }

        // v8::kGCCallbackFlagCollectAllAvailableGarbage is used when V8 handles
        // low memory notifications.
        if ((flags & v8::kGCCallbackFlagCollectAllAvailableGarbage) || (flags & v8::kGCCallbackFlagCollectAllExternalMemory)) {
            // This single GC is not enough. See the above comment.
            Heap::collectGarbage(ThreadState::HeapPointersOnStack, ThreadState::GCWithSweep, Heap::ForcedGC);

            // The conservative GC might have left floating garbage. Schedule
            // precise GC to ensure that we collect all available garbage.
            currentThreadState->schedulePreciseGC();
        }
    }

    TRACE_EVENT_INSTANT1(TRACE_DISABLED_BY_DEFAULT("devtools.timeline"), "UpdateCounters", TRACE_EVENT_SCOPE_THREAD, "data", InspectorUpdateCountersEvent::data());
    //OutputDebugStringA("V8GCController::gcEpilogue end\n");
}

void V8GCController::collectGarbage(v8::Isolate* isolate/*, bool onlyMinorGC*/) {
    v8::HandleScope handleScope(isolate);
    RefPtr<ScriptState> scriptState = ScriptState::create(v8::Context::New(isolate), DOMWrapperWorld::create(isolate));
    ScriptState::Scope scope(scriptState.get());
    StringBuilder builder;
    builder.append("if (gc) gc(");
    //builder.append(onlyMinorGC ? "true" : "false");
    builder.append(")");
    V8ScriptRunner::compileAndRunInternalScript(v8String(isolate, builder.toString()), isolate);
    scriptState->disposePerContextData();
}

void V8GCController::reportDOMMemoryUsageToV8(v8::Isolate* isolate)
{
    if (!isMainThread())
        return;

    static size_t lastUsageReportedToV8 = 0;

    size_t currentUsage = WTF::Partitions::currentDOMMemoryUsage();
    int64_t diff = static_cast<int64_t>(currentUsage) - static_cast<int64_t>(lastUsageReportedToV8);
    isolate->AdjustAmountOfExternalAllocatedMemory(diff);

    lastUsageReportedToV8 = currentUsage;
}

namespace {

bool IsDOMWrapperClassId(uint16_t class_id)
{
    return class_id == WrapperTypeInfo::NodeClassId || class_id == WrapperTypeInfo::ObjectClassId 
        //|| class_id == WrapperTypeInfo::kCustomWrappableId
        ;
}

class DOMWrapperTracer : public v8::PersistentHandleVisitor {
public:
    explicit DOMWrapperTracer(Visitor* visitor) : m_visitor(visitor) {}

    void VisitPersistentHandle(v8::Persistent<v8::Value>* value, uint16_t classId) override
    {
        if (classId != WrapperTypeInfo::NodeClassId && classId != WrapperTypeInfo::ObjectClassId)
            return;

        const v8::Persistent<v8::Object>& wrapper = v8::Persistent<v8::Object>::Cast(*value);

        if (m_visitor)
            toWrapperTypeInfo(wrapper)->trace(m_visitor, toScriptWrappable(wrapper));
    }

private:
    Visitor* m_visitor;
};

#if V8_MAJOR_VERSION >= 7

// for third_party\WebKit\Source\bindings\core\v8\WrapperTypeInfo.h
template <typename T, int offset>
inline T* getInternalField(const v8::TracedGlobal<v8::Object>& global)
{
    ASSERT(offset < v8::Object::InternalFieldCount(global));
    return reinterpret_cast<T*>(v8::Object::GetAlignedPointerFromInternalField(global, offset));
}

template <typename T, int offset>
inline T* getInternalField(const v8::PersistentBase<v8::Object>& persistent)
{
    ASSERT(offset < v8::Object::InternalFieldCount(persistent));
    return reinterpret_cast<T*>(v8::Object::GetAlignedPointerFromInternalField(persistent, offset));
}

void* toUntypedWrappable(const v8::PersistentBase<v8::Object>& wrapper)
{
    return getInternalField<void, v8DOMWrapperObjectIndex>(wrapper);
}

void* toUntypedWrappable(const v8::TracedGlobal<v8::Object>& wrapper)
{
    return getInternalField<void, v8DOMWrapperObjectIndex>(wrapper);
}

const WrapperTypeInfo* toWrapperTypeInfo(const v8::PersistentBase<v8::Object>& wrapper)
{
    return getInternalField<WrapperTypeInfo, v8DOMWrapperTypeIndex>(wrapper);
}

const WrapperTypeInfo* toWrapperTypeInfo(const v8::TracedGlobal<v8::Object>& wrapper)
{
    return getInternalField<WrapperTypeInfo, v8DOMWrapperTypeIndex>(wrapper);
}

// Visitor forwarding all DOM wrapper handles to the provided Blink visitor.
class DOMWrapperForwardingVisitor final 
    : public v8::PersistentHandleVisitor
    , public v8::EmbedderHeapTracer::TracedGlobalHandleVisitor {
public:
    explicit DOMWrapperForwardingVisitor(Visitor* visitor) : m_visitor(visitor)
    {
        ASSERT(m_visitor);
    }

    void VisitPersistentHandle(v8::Persistent<v8::Value>* value, uint16_t class_id) final
    {
        // TODO(mlippautz): There should be no more v8::Persistent that have a class id set.
        VisitHandle(value, class_id);
    }

    void VisitTracedGlobalHandle(const v8::TracedGlobal<v8::Value>& value) final
    {
        VisitHandle(&value, value.WrapperClassId());
    }

private:
    template <typename T>
    void VisitHandle(T* value, uint16_t class_id)
    {
        if (!IsDOMWrapperClassId(class_id))
            return;

        WrapperTypeInfo* wrapperTypeInfo = const_cast<WrapperTypeInfo*>(toWrapperTypeInfo(value->template As<v8::Object>()));

        // WrapperTypeInfo pointer may have been cleared before termination GCs on worker threads.
        if (!wrapperTypeInfo)
            return;

        ScriptWrappable* scriptWrap = (ScriptWrappable*)toUntypedWrappable(value->template As<v8::Object>());
        wrapperTypeInfo->trace(m_visitor, scriptWrap);
    }

    Visitor* const m_visitor;
};

#endif

}  // namespace

void V8GCController::traceDOMWrappers(v8::Isolate* isolate, Visitor* parentVisitor) {
#if V8_MAJOR_VERSION >= 7
    // 不需要在这里手动trace。只要v8::EmbedderHeapTracer::AdvanceTracing里注册了，v8都会被DOMWrapperForwardingVisitor遍历出来
//    std::vector<std::pair<void*, void*>>* v8References = V8PerIsolateData::from(isolate)->leakV8References();
//
//     for (size_t i = 0; v8References && i < v8References->size(); ++i) {
//         std::pair<void*, void*> internalFields = v8References->at(i);
//         WrapperTypeInfo* wrapperTypeInfo = reinterpret_cast<WrapperTypeInfo*>(internalFields.first);
// 
//         void* scriptWrappablePtr = internalFields.second;
//         ScriptWrappable* scriptWrappable = (ScriptWrappable*)scriptWrappablePtr;
// 
//         if (wrapperTypeInfo->ginEmbedder != gin::GinEmbedder::kEmbedderBlink)
//             continue;
// 
//         output = (char*)malloc(0x100);
//         sprintf_s(output, 0x99, "V8GCController::traceDOMWrappers: %p\n", scriptWrappable);
//         OutputDebugStringA(output);
//         free(output);
// 
//         wrapperTypeInfo->trace(parentVisitor, scriptWrappable);
//     }

    DOMWrapperForwardingVisitor visitor(parentVisitor);
    isolate->VisitHandlesWithClassIds(&visitor);

    v8::EmbedderHeapTracer* tracer = V8PerIsolateData::from(isolate)->getEmbedderHeapTracer(isolate);
    // There may be no tracer during tear down garbage collections.
    // Not all threads have a tracer attached.
    if (tracer)
        tracer->IterateTracedGlobalHandles(&visitor);
#else
    DOMWrapperTracer tracer(parentVisitor);
    isolate->VisitHandlesWithClassIds(&tracer);
#endif
}

class PendingActivityVisitor : public v8::PersistentHandleVisitor {
public:
    PendingActivityVisitor(v8::Isolate* isolate, ExecutionContext* executionContext)
        : m_isolate(isolate)
        , m_executionContext(executionContext)
        , m_pendingActivityFound(false) {
    }

    void VisitPersistentHandle(v8::Persistent<v8::Value>* value, uint16_t classId) override {
        // If we have already found any wrapper that has a pending activity,
        // we don't need to check other wrappers.
        if (m_pendingActivityFound)
            return;

        if (classId != WrapperTypeInfo::NodeClassId && classId != WrapperTypeInfo::ObjectClassId)
            return;

        v8::Local<v8::Object> wrapper = v8::Local<v8::Object>::New(m_isolate, v8::Persistent<v8::Object>::Cast(*value));
        ASSERT(V8DOMWrapper::hasInternalFieldsSet(wrapper));

        // The ExecutionContext check is heavy, so it should be done at the last.
        const WrapperTypeInfo* type = toWrapperTypeInfo(wrapper);
        ActiveDOMObject* activeDOMObject = type->toActiveDOMObject(wrapper);

        if (activeDOMObject && activeDOMObject->hasPendingActivity()) {
            // See the comment in MajorGCWrapperVisitor::VisitPersistentHandle.
            ExecutionContext* context = toExecutionContext(wrapper->CreationContext());
            if (context == m_executionContext && context /*&& !context->isContextDestroyed()*/)
                m_pendingActivityFound = true;
        }
    }

    bool pendingActivityFound() const { return m_pendingActivityFound; }

private:
    v8::Isolate* m_isolate;
    Persistent<ExecutionContext> m_executionContext;
    bool m_pendingActivityFound;
};

// see to InProcessWorkerObjectProxy::didCreateWorkerGlobalScope
bool V8GCController::hasPendingActivity(v8::Isolate* isolate, ExecutionContext* executionContext)
{
    // V8GCController::hasPendingActivity is used only when a worker checks if
    // the worker contains any wrapper that has pending activities.
    ASSERT(!isMainThread());

    //DEFINE_THREAD_SAFE_STATIC_LOCAL(CustomCountHistogram, scanPendingActivityHistogram, new CustomCountHistogram("Blink.ScanPendingActivityDuration", 1, 1000, 50));
    double startTime = WTF::currentTimeMS();
    v8::HandleScope scope(isolate);
    PendingActivityVisitor visitor(isolate, executionContext);
    toIsolate(executionContext)->VisitHandlesWithClassIds(&visitor);
    //scanPendingActivityHistogram.count(static_cast<int>(WTF::currentTimeMS() - startTime));
    return visitor.pendingActivityFound();
}

}  // namespace blink

#endif