// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef InspectorTraceEvents_h
#define InspectorTraceEvents_h

#include "core/CoreExport.h"
#include "core/css/CSSSelector.h"
#include "platform/EventTracer.h"
#include "platform/TraceEvent.h"
#include "platform/heap/Handle.h"
#include "wtf/Forward.h"
#include "wtf/Functional.h"

namespace blink {
class Animation;
class CSSStyleSheetResource;
class DescendantInvalidationSet;
class Document;
class Element;
class Event;
class ExecutionContext;
class FrameView;
class GraphicsLayer;
class HitTestLocation;
class HitTestRequest;
class HitTestResult;
class ImageResource;
class KURL;
class DeprecatedPaintLayer;
class LayoutRect;
class LocalFrame;
class Node;
class QualifiedName;
class Page;
class LayoutImage;
class LayoutObject;
class ResourceRequest;
class ResourceResponse;
class StyleChangeReasonForTracing;
class StyleImage;
class TracedValue;
class WorkerThread;
class XMLHttpRequest;

class InspectorLayoutEvent {
public:
    static PassRefPtr<TraceEvent::ConvertableToTraceFormat> beginData(FrameView*);
    static PassRefPtr<TraceEvent::ConvertableToTraceFormat> endData(LayoutObject* rootForThisLayout);
};

class InspectorScheduleStyleInvalidationTrackingEvent {
public:
    static const char Attribute[];
    static const char Class[];
    static const char Id[];
    static const char Pseudo[];

    static PassRefPtr<TraceEvent::ConvertableToTraceFormat> attributeChange(Element&, const DescendantInvalidationSet&, const QualifiedName&);
    static PassRefPtr<TraceEvent::ConvertableToTraceFormat> classChange(Element&, const DescendantInvalidationSet&, const AtomicString&);
    static PassRefPtr<TraceEvent::ConvertableToTraceFormat> idChange(Element&, const DescendantInvalidationSet&, const AtomicString&);
    static PassRefPtr<TraceEvent::ConvertableToTraceFormat> pseudoChange(Element&, const DescendantInvalidationSet&, CSSSelector::PseudoType);

private:
    static PassRefPtr<TracedValue> fillCommonPart(Element&, const DescendantInvalidationSet&, const char* invalidatedSelector);
};

#define TRACE_SCHEDULE_STYLE_INVALIDATION(element, invalidationSet, changeType, ...) \
    TRACE_EVENT_INSTANT1( \
        TRACE_DISABLED_BY_DEFAULT("devtools.timeline.invalidationTracking"), \
        "ScheduleStyleInvalidationTracking", \
        TRACE_EVENT_SCOPE_THREAD, \
        "data", \
        InspectorScheduleStyleInvalidationTrackingEvent::changeType((element), (invalidationSet), __VA_ARGS__))

class InspectorStyleRecalcInvalidationTrackingEvent {
public:
    static PassRefPtr<TraceEvent::ConvertableToTraceFormat> data(Node*, const StyleChangeReasonForTracing&);
};

String descendantInvalidationSetToIdString(const DescendantInvalidationSet&);

class InspectorStyleInvalidatorInvalidateEvent {
public:
    static const char ElementHasPendingInvalidationList[];
    static const char InvalidateCustomPseudo[];
    static const char InvalidationSetMatchedAttribute[];
    static const char InvalidationSetMatchedClass[];
    static const char InvalidationSetMatchedId[];
    static const char InvalidationSetMatchedTagName[];
    static const char PreventStyleSharingForParent[];

    static PassRefPtr<TraceEvent::ConvertableToTraceFormat> data(Element&, const char* reason);
    static PassRefPtr<TraceEvent::ConvertableToTraceFormat> selectorPart(Element&, const char* reason, const DescendantInvalidationSet&, const String&);
    static PassRefPtr<TraceEvent::ConvertableToTraceFormat> invalidationList(Element&, const WillBeHeapVector<RefPtrWillBeMember<DescendantInvalidationSet> >&);

private:
    static PassRefPtr<TracedValue> fillCommonPart(Element&, const char* reason);
};

#define TRACE_STYLE_INVALIDATOR_INVALIDATION(element, reason) \
    TRACE_EVENT_INSTANT1( \
        TRACE_DISABLED_BY_DEFAULT("devtools.timeline.invalidationTracking"), \
        "StyleInvalidatorInvalidationTracking", \
        TRACE_EVENT_SCOPE_THREAD, \
        "data", \
        InspectorStyleInvalidatorInvalidateEvent::data((element), (InspectorStyleInvalidatorInvalidateEvent::reason)))

#define TRACE_STYLE_INVALIDATOR_INVALIDATION_SELECTORPART(element, reason, invalidationSet, singleSelectorPart) \
    TRACE_EVENT_INSTANT1( \
        TRACE_DISABLED_BY_DEFAULT("devtools.timeline.invalidationTracking"), \
        "StyleInvalidatorInvalidationTracking", \
        TRACE_EVENT_SCOPE_THREAD, \
        "data", \
        InspectorStyleInvalidatorInvalidateEvent::selectorPart((element), (InspectorStyleInvalidatorInvalidateEvent::reason), (invalidationSet), (singleSelectorPart)))

// From a web developer's perspective: what caused this layout? This is strictly
// for tracing. Blink logic must not depend on these.
namespace LayoutInvalidationReason {
extern const char Unknown[];
extern const char SizeChanged[];
extern const char AncestorMoved[];
extern const char StyleChange[];
extern const char DomChanged[];
extern const char TextChanged[];
extern const char PrintingChanged[];
extern const char AttributeChanged[];
extern const char ColumnsChanged[];
extern const char ChildAnonymousBlockChanged[];
extern const char AnonymousBlockChange[];
extern const char Fullscreen[];
extern const char ChildChanged[];
extern const char ListValueChange[];
extern const char ImageChanged[];
extern const char LineBoxesChanged[];
extern const char SliderValueChanged[];
extern const char AncestorMarginCollapsing[];
extern const char FieldsetChanged[];
extern const char TextAutosizing[];
extern const char SvgResourceInvalidated[];
extern const char FloatDescendantChanged[];
extern const char CountersChanged[];
extern const char GridChanged[];
extern const char MenuWidthChanged[];
extern const char RemovedFromLayout[];
extern const char AddedToLayout[];
extern const char TableChanged[];
extern const char PaddingChanged[];
extern const char TextControlChanged[];
// FIXME: This is too generic, we should be able to split out transform and
// size related invalidations.
extern const char SvgChanged[];
extern const char ScrollbarChanged[];
}

// LayoutInvalidationReasonForTracing is strictly for tracing. Blink logic must
// not depend on this value.
typedef const char LayoutInvalidationReasonForTracing[];

class CORE_EXPORT InspectorLayoutInvalidationTrackingEvent {
public:
    static PassRefPtr<TraceEvent::ConvertableToTraceFormat> data(const LayoutObject*, LayoutInvalidationReasonForTracing);
};

class InspectorPaintInvalidationTrackingEvent {
public:
    static PassRefPtr<TraceEvent::ConvertableToTraceFormat> data(const LayoutObject*, const LayoutObject& paintContainer);
};

class InspectorScrollInvalidationTrackingEvent {
public:
    static PassRefPtr<TraceEvent::ConvertableToTraceFormat> data(const LayoutObject&);
};

class InspectorSendRequestEvent {
public:
    static PassRefPtr<TraceEvent::ConvertableToTraceFormat> data(unsigned long identifier, LocalFrame*, const ResourceRequest&);
};

class InspectorReceiveResponseEvent {
public:
    static PassRefPtr<TraceEvent::ConvertableToTraceFormat> data(unsigned long identifier, LocalFrame*, const ResourceResponse&);
};

class InspectorReceiveDataEvent {
public:
    static PassRefPtr<TraceEvent::ConvertableToTraceFormat> data(unsigned long identifier, LocalFrame*, int encodedDataLength);
};

class InspectorResourceFinishEvent {
public:
    static PassRefPtr<TraceEvent::ConvertableToTraceFormat> data(unsigned long identifier, double finishTime, bool didFail);
};

class InspectorTimerInstallEvent {
public:
    static PassRefPtr<TraceEvent::ConvertableToTraceFormat> data(ExecutionContext*, int timerId, int timeout, bool singleShot);
};

class InspectorTimerRemoveEvent {
public:
    static PassRefPtr<TraceEvent::ConvertableToTraceFormat> data(ExecutionContext*, int timerId);
};

class InspectorTimerFireEvent {
public:
    static PassRefPtr<TraceEvent::ConvertableToTraceFormat> data(ExecutionContext*, int timerId);
};

class InspectorAnimationFrameEvent {
public:
    static PassRefPtr<TraceEvent::ConvertableToTraceFormat> data(ExecutionContext*, int callbackId);
};

class InspectorParseHtmlEvent {
public:
    static PassRefPtr<TraceEvent::ConvertableToTraceFormat> beginData(Document*, unsigned startLine);
    static PassRefPtr<TraceEvent::ConvertableToTraceFormat> endData(unsigned endLine);
};

class InspectorParseAuthorStyleSheetEvent {
public:
    static PassRefPtr<TraceEvent::ConvertableToTraceFormat> data(const CSSStyleSheetResource*);
};

class InspectorXhrReadyStateChangeEvent {
public:
    static PassRefPtr<TraceEvent::ConvertableToTraceFormat> data(ExecutionContext*, XMLHttpRequest*);
};

class InspectorXhrLoadEvent {
public:
    static PassRefPtr<TraceEvent::ConvertableToTraceFormat> data(ExecutionContext*, XMLHttpRequest*);
};

class InspectorLayerInvalidationTrackingEvent {
public:
    static const char SquashingLayerGeometryWasUpdated[];
    static const char AddedToSquashingLayer[];
    static const char RemovedFromSquashingLayer[];
    static const char ReflectionLayerChanged[];
    static const char NewCompositedLayer[];

    static PassRefPtr<TraceEvent::ConvertableToTraceFormat> data(const DeprecatedPaintLayer*, const char* reason);
};
#define TRACE_LAYER_INVALIDATION(LAYER, REASON) \
    TRACE_EVENT_INSTANT1( \
        TRACE_DISABLED_BY_DEFAULT("devtools.timeline.invalidationTracking"), \
        "LayerInvalidationTracking", \
        TRACE_EVENT_SCOPE_THREAD, \
        "data", \
        InspectorLayerInvalidationTrackingEvent::data((LAYER), (REASON)))

class InspectorPaintEvent {
public:
    static PassRefPtr<TraceEvent::ConvertableToTraceFormat> data(LayoutObject*, const LayoutRect& clipRect, const GraphicsLayer*);
};

class InspectorPaintImageEvent {
public:
    static PassRefPtr<TraceEvent::ConvertableToTraceFormat> data(const LayoutImage&);
    static PassRefPtr<TraceEvent::ConvertableToTraceFormat> data(const LayoutObject&, const StyleImage&);
    static PassRefPtr<TraceEvent::ConvertableToTraceFormat> data(const LayoutObject*, const ImageResource&);
};

class InspectorCommitLoadEvent {
public:
    static PassRefPtr<TraceEvent::ConvertableToTraceFormat> data(LocalFrame*);
};

class InspectorMarkLoadEvent {
public:
    static PassRefPtr<TraceEvent::ConvertableToTraceFormat> data(LocalFrame*);
};

class InspectorScrollLayerEvent {
public:
    static PassRefPtr<TraceEvent::ConvertableToTraceFormat> data(LayoutObject*);
};

class InspectorUpdateLayerTreeEvent {
public:
    static PassRefPtr<TraceEvent::ConvertableToTraceFormat> data(LocalFrame*);
};

class InspectorEvaluateScriptEvent {
public:
    static PassRefPtr<TraceEvent::ConvertableToTraceFormat> data(LocalFrame*, const String& url, int lineNumber);
};

class InspectorFunctionCallEvent {
public:
    static PassRefPtr<TraceEvent::ConvertableToTraceFormat> data(ExecutionContext*, int scriptId, const String& scriptName, int scriptLine);
};

class InspectorUpdateCountersEvent {
public:
    static PassRefPtr<TraceEvent::ConvertableToTraceFormat> data();
};

class InspectorInvalidateLayoutEvent {
public:
    static PassRefPtr<TraceEvent::ConvertableToTraceFormat> data(LocalFrame*);
};

class InspectorRecalculateStylesEvent {
public:
    static PassRefPtr<TraceEvent::ConvertableToTraceFormat> data(LocalFrame*);
};

class InspectorEventDispatchEvent {
public:
    static PassRefPtr<TraceEvent::ConvertableToTraceFormat> data(const Event&);
};

class InspectorTimeStampEvent {
public:
    static PassRefPtr<TraceEvent::ConvertableToTraceFormat> data(ExecutionContext*, const String& message);
};

class InspectorTracingSessionIdForWorkerEvent {
public:
    static PassRefPtr<TraceEvent::ConvertableToTraceFormat> data(const String& sessionId, const String& workerId, WorkerThread*);
};

class InspectorTracingStartedInFrame {
public:
    static PassRefPtr<TraceEvent::ConvertableToTraceFormat> data(const String& sessionId, LocalFrame*);
};

class InspectorSetLayerTreeId {
public:
    static PassRefPtr<TraceEvent::ConvertableToTraceFormat> data(const String& sessionId, int layerTreeId);
};

class InspectorAnimationEvent {
public:
    static PassRefPtr<TraceEvent::ConvertableToTraceFormat> data(const Animation&);
};

class InspectorAnimationStateEvent {
public:
    static PassRefPtr<TraceEvent::ConvertableToTraceFormat> data(const Animation&);
};

class InspectorHitTestEvent {
public:
    static PassRefPtr<TraceEvent::ConvertableToTraceFormat> endData(const HitTestRequest&, const HitTestLocation&, const HitTestResult&);
};

CORE_EXPORT String toHexString(const void* p);
CORE_EXPORT void setCallStack(TracedValue*);

} // namespace blink


#endif // !defined(InspectorTraceEvents_h)
