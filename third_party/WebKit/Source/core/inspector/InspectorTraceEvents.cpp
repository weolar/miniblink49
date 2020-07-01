// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/inspector/InspectorTraceEvents.h"

#include "bindings/core/v8/ScriptCallStackFactory.h"
#include "bindings/core/v8/ScriptSourceCode.h"
#include "core/animation/Animation.h"
#include "core/animation/KeyframeEffect.h"
#include "core/css/invalidation/DescendantInvalidationSet.h"
#include "core/dom/DOMNodeIds.h"
#include "core/dom/StyleChangeReason.h"
#include "core/events/Event.h"
#include "core/fetch/CSSStyleSheetResource.h"
#include "core/frame/FrameView.h"
#include "core/frame/LocalFrame.h"
#include "core/inspector/IdentifiersFactory.h"
#include "core/inspector/ScriptCallStack.h"
#include "core/layout/HitTestResult.h"
#include "core/layout/LayoutImage.h"
#include "core/layout/LayoutObject.h"
#include "core/page/Page.h"
#include "core/paint/DeprecatedPaintLayer.h"
#include "core/workers/WorkerThread.h"
#include "core/xmlhttprequest/XMLHttpRequest.h"
#include "platform/JSONValues.h"
#include "platform/TracedValue.h"
#include "platform/graphics/GraphicsLayer.h"
#include "platform/network/ResourceRequest.h"
#include "platform/network/ResourceResponse.h"
#include "platform/weborigin/KURL.h"
#include "wtf/Vector.h"
#include <inttypes.h>
#include <v8.h>

namespace blink {

static const unsigned maxInvalidationTrackingCallstackSize = 5;

String toHexString(const void* p)
{
    return String::format("0x%" PRIx64, static_cast<uint64_t>(reinterpret_cast<intptr_t>(p)));
}

void setCallStack(TracedValue* value)
{
    static const unsigned char* traceCategoryEnabled = 0;
    WTF_ANNOTATE_BENIGN_RACE(&traceCategoryEnabled, "trace_event category");
#ifdef MINIBLINK_NOT_IMPLEMENTED
    if (!traceCategoryEnabled) // weolar
        traceCategoryEnabled = TRACE_EVENT_API_GET_CATEGORY_ENABLED(TRACE_DISABLED_BY_DEFAULT("devtools.timeline.stack"));
    if (!*traceCategoryEnabled)
        return;
#endif
    RefPtrWillBeRawPtr<ScriptCallStack> scriptCallStack = createScriptCallStack(ScriptCallStack::maxCallStackSizeToCapture, true);
    if (scriptCallStack)
        scriptCallStack->toTracedValue(value, "stackTrace");
}

namespace {

void setNodeInfo(TracedValue* value, Node* node, const char* idFieldName, const char* nameFieldName = nullptr)
{
    value->setInteger(idFieldName, DOMNodeIds::idForNode(node));
    if (nameFieldName)
        value->setString(nameFieldName, node->debugName());
}

const char* pseudoTypeToString(CSSSelector::PseudoType pseudoType)
{
    switch (pseudoType) {
#define DEFINE_STRING_MAPPING(pseudoType) case CSSSelector::pseudoType: return #pseudoType;
        DEFINE_STRING_MAPPING(PseudoUnknown)
        DEFINE_STRING_MAPPING(PseudoEmpty)
        DEFINE_STRING_MAPPING(PseudoFirstChild)
        DEFINE_STRING_MAPPING(PseudoFirstOfType)
        DEFINE_STRING_MAPPING(PseudoLastChild)
        DEFINE_STRING_MAPPING(PseudoLastOfType)
        DEFINE_STRING_MAPPING(PseudoOnlyChild)
        DEFINE_STRING_MAPPING(PseudoOnlyOfType)
        DEFINE_STRING_MAPPING(PseudoFirstLine)
        DEFINE_STRING_MAPPING(PseudoFirstLetter)
        DEFINE_STRING_MAPPING(PseudoNthChild)
        DEFINE_STRING_MAPPING(PseudoNthOfType)
        DEFINE_STRING_MAPPING(PseudoNthLastChild)
        DEFINE_STRING_MAPPING(PseudoNthLastOfType)
        DEFINE_STRING_MAPPING(PseudoLink)
        DEFINE_STRING_MAPPING(PseudoVisited)
        DEFINE_STRING_MAPPING(PseudoAny)
        DEFINE_STRING_MAPPING(PseudoAnyLink)
        DEFINE_STRING_MAPPING(PseudoAutofill)
        DEFINE_STRING_MAPPING(PseudoHover)
        DEFINE_STRING_MAPPING(PseudoDrag)
        DEFINE_STRING_MAPPING(PseudoFocus)
        DEFINE_STRING_MAPPING(PseudoActive)
        DEFINE_STRING_MAPPING(PseudoChecked)
        DEFINE_STRING_MAPPING(PseudoEnabled)
        DEFINE_STRING_MAPPING(PseudoFullPageMedia)
        DEFINE_STRING_MAPPING(PseudoDefault)
        DEFINE_STRING_MAPPING(PseudoDisabled)
        DEFINE_STRING_MAPPING(PseudoOptional)
        DEFINE_STRING_MAPPING(PseudoRequired)
        DEFINE_STRING_MAPPING(PseudoReadOnly)
        DEFINE_STRING_MAPPING(PseudoReadWrite)
        DEFINE_STRING_MAPPING(PseudoValid)
        DEFINE_STRING_MAPPING(PseudoInvalid)
        DEFINE_STRING_MAPPING(PseudoIndeterminate)
        DEFINE_STRING_MAPPING(PseudoTarget)
        DEFINE_STRING_MAPPING(PseudoBefore)
        DEFINE_STRING_MAPPING(PseudoAfter)
        DEFINE_STRING_MAPPING(PseudoBackdrop)
        DEFINE_STRING_MAPPING(PseudoLang)
        DEFINE_STRING_MAPPING(PseudoNot)
        DEFINE_STRING_MAPPING(PseudoResizer)
        DEFINE_STRING_MAPPING(PseudoRoot)
        DEFINE_STRING_MAPPING(PseudoScope)
        DEFINE_STRING_MAPPING(PseudoScrollbar)
        DEFINE_STRING_MAPPING(PseudoScrollbarButton)
        DEFINE_STRING_MAPPING(PseudoScrollbarCorner)
        DEFINE_STRING_MAPPING(PseudoScrollbarThumb)
        DEFINE_STRING_MAPPING(PseudoScrollbarTrack)
        DEFINE_STRING_MAPPING(PseudoScrollbarTrackPiece)
        DEFINE_STRING_MAPPING(PseudoWindowInactive)
        DEFINE_STRING_MAPPING(PseudoCornerPresent)
        DEFINE_STRING_MAPPING(PseudoDecrement)
        DEFINE_STRING_MAPPING(PseudoIncrement)
        DEFINE_STRING_MAPPING(PseudoHorizontal)
        DEFINE_STRING_MAPPING(PseudoVertical)
        DEFINE_STRING_MAPPING(PseudoStart)
        DEFINE_STRING_MAPPING(PseudoEnd)
        DEFINE_STRING_MAPPING(PseudoDoubleButton)
        DEFINE_STRING_MAPPING(PseudoSingleButton)
        DEFINE_STRING_MAPPING(PseudoNoButton)
        DEFINE_STRING_MAPPING(PseudoSelection)
        DEFINE_STRING_MAPPING(PseudoLeftPage)
        DEFINE_STRING_MAPPING(PseudoRightPage)
        DEFINE_STRING_MAPPING(PseudoFirstPage)
        DEFINE_STRING_MAPPING(PseudoFullScreen)
        DEFINE_STRING_MAPPING(PseudoFullScreenDocument)
        DEFINE_STRING_MAPPING(PseudoFullScreenAncestor)
        DEFINE_STRING_MAPPING(PseudoInRange)
        DEFINE_STRING_MAPPING(PseudoOutOfRange)
        DEFINE_STRING_MAPPING(PseudoWebKitCustomElement)
        DEFINE_STRING_MAPPING(PseudoCue)
        DEFINE_STRING_MAPPING(PseudoFutureCue)
        DEFINE_STRING_MAPPING(PseudoPastCue)
        DEFINE_STRING_MAPPING(PseudoUnresolved)
        DEFINE_STRING_MAPPING(PseudoContent)
        DEFINE_STRING_MAPPING(PseudoHost)
        DEFINE_STRING_MAPPING(PseudoHostContext)
        DEFINE_STRING_MAPPING(PseudoShadow)
        DEFINE_STRING_MAPPING(PseudoSpatialNavigationFocus)
        DEFINE_STRING_MAPPING(PseudoListBox)
#undef DEFINE_STRING_MAPPING
    }

    ASSERT_NOT_REACHED();
    return "";
}

}

PassRefPtr<TracedValue> InspectorScheduleStyleInvalidationTrackingEvent::fillCommonPart(Element& element, const DescendantInvalidationSet& invalidationSet, const char* invalidatedSelector)
{
    RefPtr<TracedValue> value = TracedValue::create();
    value->setString("frame", toHexString(element.document().frame()));
    setNodeInfo(value.get(), &element, "nodeId", "nodeName");
    value->setString("invalidationSet", descendantInvalidationSetToIdString(invalidationSet));
    value->setString("invalidatedSelectorId", invalidatedSelector);
    if (RefPtrWillBeRawPtr<ScriptCallStack> stackTrace = createScriptCallStack(maxInvalidationTrackingCallstackSize, true))
        stackTrace->toTracedValue(value.get(), "stackTrace");
    return value.release();
}

const char InspectorScheduleStyleInvalidationTrackingEvent::Attribute[] = "attribute";
const char InspectorScheduleStyleInvalidationTrackingEvent::Class[] = "class";
const char InspectorScheduleStyleInvalidationTrackingEvent::Id[] = "id";
const char InspectorScheduleStyleInvalidationTrackingEvent::Pseudo[] = "pseudo";

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorScheduleStyleInvalidationTrackingEvent::idChange(Element& element, const DescendantInvalidationSet& invalidationSet, const AtomicString& id)
{
    RefPtr<TracedValue> value = fillCommonPart(element, invalidationSet, Id);
    value->setString("changedId", id);
    return value.release();
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorScheduleStyleInvalidationTrackingEvent::classChange(Element& element, const DescendantInvalidationSet& invalidationSet, const AtomicString& className)
{
    RefPtr<TracedValue> value = fillCommonPart(element, invalidationSet, Class);
    value->setString("changedClass", className);
    return value.release();
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorScheduleStyleInvalidationTrackingEvent::attributeChange(Element& element, const DescendantInvalidationSet& invalidationSet, const QualifiedName& attributeName)
{
    RefPtr<TracedValue> value = fillCommonPart(element, invalidationSet, Attribute);
    value->setString("changedAttribute", attributeName.toString());
    return value.release();
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorScheduleStyleInvalidationTrackingEvent::pseudoChange(Element& element, const DescendantInvalidationSet& invalidationSet, CSSSelector::PseudoType pseudoType)
{
    RefPtr<TracedValue> value = fillCommonPart(element, invalidationSet, Attribute);
    value->setString("changedPseudo", pseudoTypeToString(pseudoType));
    return value.release();
}

String descendantInvalidationSetToIdString(const DescendantInvalidationSet& set)
{
    return toHexString(&set);
}

const char InspectorStyleInvalidatorInvalidateEvent::ElementHasPendingInvalidationList[] = "Element has pending invalidation list";
const char InspectorStyleInvalidatorInvalidateEvent::InvalidateCustomPseudo[] = "Invalidate custom pseudo element";
const char InspectorStyleInvalidatorInvalidateEvent::InvalidationSetMatchedAttribute[] = "Invalidation set matched attribute";
const char InspectorStyleInvalidatorInvalidateEvent::InvalidationSetMatchedClass[] = "Invalidation set matched class";
const char InspectorStyleInvalidatorInvalidateEvent::InvalidationSetMatchedId[] = "Invalidation set matched id";
const char InspectorStyleInvalidatorInvalidateEvent::InvalidationSetMatchedTagName[] = "Invalidation set matched tagName";
const char InspectorStyleInvalidatorInvalidateEvent::PreventStyleSharingForParent[] = "Prevent style sharing for parent";

PassRefPtr<TracedValue> InspectorStyleInvalidatorInvalidateEvent::fillCommonPart(Element& element, const char* reason)
{
    RefPtr<TracedValue> value = TracedValue::create();
    value->setString("frame", toHexString(element.document().frame()));
    setNodeInfo(value.get(), &element, "nodeId", "nodeName");
    value->setString("reason", reason);
    return value.release();
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorStyleInvalidatorInvalidateEvent::data(Element& element, const char* reason)
{
    return fillCommonPart(element, reason);
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorStyleInvalidatorInvalidateEvent::selectorPart(Element& element, const char* reason, const DescendantInvalidationSet& invalidationSet, const String& selectorPart)
{
    RefPtr<TracedValue> value = fillCommonPart(element, reason);
    value->beginArray("invalidationList");
    invalidationSet.toTracedValue(value.get());
    value->endArray();
    value->setString("selectorPart", selectorPart);
    return value.release();
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorStyleInvalidatorInvalidateEvent::invalidationList(Element& element, const WillBeHeapVector<RefPtrWillBeMember<DescendantInvalidationSet> >& invalidationList)
{
    RefPtr<TracedValue> value = fillCommonPart(element, ElementHasPendingInvalidationList);
    value->beginArray("invalidationList");
    for (const auto& invalidationSet : invalidationList)
        invalidationSet->toTracedValue(value.get());
    value->endArray();
    return value.release();
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorStyleRecalcInvalidationTrackingEvent::data(Node* node, const StyleChangeReasonForTracing& reason)
{
    ASSERT(node);

    RefPtr<TracedValue> value = TracedValue::create();
    value->setString("frame", toHexString(node->document().frame()));
    setNodeInfo(value.get(), node, "nodeId", "nodeName");
    value->setString("reason", reason.reasonString());
    value->setString("extraData", reason.extraData());
    if (RefPtrWillBeRawPtr<ScriptCallStack> stackTrace = createScriptCallStack(maxInvalidationTrackingCallstackSize, true))
        stackTrace->toTracedValue(value.get(), "stackTrace");
    return value.release();
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorLayoutEvent::beginData(FrameView* frameView)
{
    bool isPartial;
    unsigned needsLayoutObjects;
    unsigned totalObjects;
    LocalFrame& frame = frameView->frame();
    frame.view()->countObjectsNeedingLayout(needsLayoutObjects, totalObjects, isPartial);

    RefPtr<TracedValue> value = TracedValue::create();
    value->setInteger("dirtyObjects", needsLayoutObjects);
    value->setInteger("totalObjects", totalObjects);
    value->setBoolean("partialLayout", isPartial);
    value->setString("frame", toHexString(&frame));
    setCallStack(value.get());
    return value.release();
}

static void createQuad(TracedValue* value, const char* name, const FloatQuad& quad)
{
    value->beginArray(name);
    value->pushDouble(quad.p1().x());
    value->pushDouble(quad.p1().y());
    value->pushDouble(quad.p2().x());
    value->pushDouble(quad.p2().y());
    value->pushDouble(quad.p3().x());
    value->pushDouble(quad.p3().y());
    value->pushDouble(quad.p4().x());
    value->pushDouble(quad.p4().y());
    value->endArray();
}

static void setGeneratingNodeInfo(TracedValue* value, const LayoutObject* layoutObject, const char* idFieldName, const char* nameFieldName = nullptr)
{
    Node* node = nullptr;
    for (; layoutObject && !node; layoutObject = layoutObject->parent())
        node = layoutObject->generatingNode();
    if (!node)
        return;

    setNodeInfo(value, node, idFieldName, nameFieldName);
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorLayoutEvent::endData(LayoutObject* rootForThisLayout)
{
    Vector<FloatQuad> quads;
    rootForThisLayout->absoluteQuads(quads);

    RefPtr<TracedValue> value = TracedValue::create();
    if (quads.size() >= 1) {
        createQuad(value.get(), "root", quads[0]);
        setGeneratingNodeInfo(value.get(), rootForThisLayout, "rootNode");
    } else {
        ASSERT_NOT_REACHED();
    }
    return value.release();
}

namespace LayoutInvalidationReason {
const char Unknown[] = "Unknown";
const char SizeChanged[] = "Size changed";
const char AncestorMoved[] = "Ancestor moved";
const char StyleChange[] = "Style changed";
const char DomChanged[] = "DOM changed";
const char TextChanged[] = "Text changed";
const char PrintingChanged[] = "Printing changed";
const char AttributeChanged[] = "Attribute changed";
const char ColumnsChanged[] = "Attribute changed";
const char ChildAnonymousBlockChanged[] = "Child anonymous block changed";
const char AnonymousBlockChange[] = "Anonymous block change";
const char Fullscreen[] = "Fullscreen change";
const char ChildChanged[] = "Child changed";
const char ListValueChange[] = "List value change";
const char ImageChanged[] = "Image changed";
const char LineBoxesChanged[] = "Line boxes changed";
const char SliderValueChanged[] = "Slider value changed";
const char AncestorMarginCollapsing[] = "Ancestor margin collapsing";
const char FieldsetChanged[] = "Fieldset changed";
const char TextAutosizing[] = "Text autosizing (font boosting)";
const char SvgResourceInvalidated[] = "SVG resource invalidated";
const char FloatDescendantChanged[] = "Floating descendant changed";
const char CountersChanged[] = "Counters changed";
const char GridChanged[] = "Grid changed";
const char MenuWidthChanged[] = "Menu width changed";
const char RemovedFromLayout[] = "Removed from layout";
const char AddedToLayout[] = "Added to layout";
const char TableChanged[] = "Table changed";
const char PaddingChanged[] = "Padding changed";
const char TextControlChanged[] = "Text control changed";
const char SvgChanged[] = "SVG changed";
const char ScrollbarChanged[] = "Scrollbar changed";
} // namespace LayoutInvalidationReason

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorLayoutInvalidationTrackingEvent::data(const LayoutObject* layoutObject, LayoutInvalidationReasonForTracing reason)
{
    ASSERT(layoutObject);
    RefPtr<TracedValue> value = TracedValue::create();
    value->setString("frame", toHexString(layoutObject->frame()));
    setGeneratingNodeInfo(value.get(), layoutObject, "nodeId", "nodeName");
    value->setString("reason", reason);
    if (RefPtrWillBeRawPtr<ScriptCallStack> stackTrace = createScriptCallStack(maxInvalidationTrackingCallstackSize, true))
        stackTrace->toTracedValue(value.get(), "stackTrace");
    return value.release();
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorPaintInvalidationTrackingEvent::data(const LayoutObject* layoutObject, const LayoutObject& paintContainer)
{
    ASSERT(layoutObject);
    RefPtr<TracedValue> value = TracedValue::create();
    value->setString("frame", toHexString(layoutObject->frame()));
    setGeneratingNodeInfo(value.get(), &paintContainer, "paintId");
    setGeneratingNodeInfo(value.get(), layoutObject, "nodeId", "nodeName");
    return value.release();
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorScrollInvalidationTrackingEvent::data(const LayoutObject& layoutObject)
{
    static const char ScrollInvalidationReason[] = "Scroll with viewport-constrained element";

    RefPtr<TracedValue> value = TracedValue::create();
    value->setString("frame", toHexString(layoutObject.frame()));
    value->setString("reason", ScrollInvalidationReason);
    setGeneratingNodeInfo(value.get(), &layoutObject, "nodeId", "nodeName");
    if (RefPtrWillBeRawPtr<ScriptCallStack> stackTrace = createScriptCallStack(maxInvalidationTrackingCallstackSize, true))
        stackTrace->toTracedValue(value.get(), "stackTrace");
    return value.release();
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorSendRequestEvent::data(unsigned long identifier, LocalFrame* frame, const ResourceRequest& request)
{
    String requestId = IdentifiersFactory::requestId(identifier);

    RefPtr<TracedValue> value = TracedValue::create();
    value->setString("requestId", requestId);
    value->setString("frame", toHexString(frame));
    value->setString("url", request.url().string());
    value->setString("requestMethod", request.httpMethod());
    setCallStack(value.get());
    return value.release();
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorReceiveResponseEvent::data(unsigned long identifier, LocalFrame* frame, const ResourceResponse& response)
{
    String requestId = IdentifiersFactory::requestId(identifier);

    RefPtr<TracedValue> value = TracedValue::create();
    value->setString("requestId", requestId);
    value->setString("frame", toHexString(frame));
    value->setInteger("statusCode", response.httpStatusCode());
    value->setString("mimeType", response.mimeType().string().isolatedCopy());
    return value.release();
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorReceiveDataEvent::data(unsigned long identifier, LocalFrame* frame, int encodedDataLength)
{
    String requestId = IdentifiersFactory::requestId(identifier);

    RefPtr<TracedValue> value = TracedValue::create();
    value->setString("requestId", requestId);
    value->setString("frame", toHexString(frame));
    value->setInteger("encodedDataLength", encodedDataLength);
    return value.release();
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorResourceFinishEvent::data(unsigned long identifier, double finishTime, bool didFail)
{
    String requestId = IdentifiersFactory::requestId(identifier);

    RefPtr<TracedValue> value = TracedValue::create();
    value->setString("requestId", requestId);
    value->setBoolean("didFail", didFail);
    if (finishTime)
        value->setDouble("networkTime", finishTime);
    return value.release();
}

static LocalFrame* frameForExecutionContext(ExecutionContext* context)
{
    LocalFrame* frame = nullptr;
    if (context->isDocument())
        frame = toDocument(context)->frame();
    return frame;
}

static PassRefPtr<TracedValue> genericTimerData(ExecutionContext* context, int timerId)
{
    RefPtr<TracedValue> value = TracedValue::create();
    value->setInteger("timerId", timerId);
    if (LocalFrame* frame = frameForExecutionContext(context))
        value->setString("frame", toHexString(frame));
    setCallStack(value.get());
    return value.release();
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorTimerInstallEvent::data(ExecutionContext* context, int timerId, int timeout, bool singleShot)
{
    RefPtr<TracedValue> value = genericTimerData(context, timerId);
    value->setInteger("timeout", timeout);
    value->setBoolean("singleShot", singleShot);
    return value.release();
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorTimerRemoveEvent::data(ExecutionContext* context, int timerId)
{
    return genericTimerData(context, timerId);
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorTimerFireEvent::data(ExecutionContext* context, int timerId)
{
    return genericTimerData(context, timerId);
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorAnimationFrameEvent::data(ExecutionContext* context, int callbackId)
{
    RefPtr<TracedValue> value = TracedValue::create();
    value->setInteger("id", callbackId);
    if (context->isDocument())
        value->setString("frame", toHexString(toDocument(context)->frame()));
    else if (context->isWorkerGlobalScope())
        value->setString("worker", toHexString(toWorkerGlobalScope(context)));
    setCallStack(value.get());
    return value.release();
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorParseHtmlEvent::beginData(Document* document, unsigned startLine)
{
    RefPtr<TracedValue> value = TracedValue::create();
    value->setInteger("startLine", startLine);
    value->setString("frame", toHexString(document->frame()));
    value->setString("url", document->url().string());
    setCallStack(value.get());
    return value.release();
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorParseHtmlEvent::endData(unsigned endLine)
{
    RefPtr<TracedValue> value = TracedValue::create();
    value->setInteger("endLine", endLine);
    return value.release();
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorParseAuthorStyleSheetEvent::data(const CSSStyleSheetResource* cachedStyleSheet)
{
    RefPtr<TracedValue> value = TracedValue::create();
    value->setString("styleSheetUrl", cachedStyleSheet->url().string());
    return value.release();
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorXhrReadyStateChangeEvent::data(ExecutionContext* context, XMLHttpRequest* request)
{
    RefPtr<TracedValue> value = TracedValue::create();
    value->setString("url", request->url().string());
    value->setInteger("readyState", request->readyState());
    if (LocalFrame* frame = frameForExecutionContext(context))
        value->setString("frame", toHexString(frame));
    setCallStack(value.get());
    return value.release();
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorXhrLoadEvent::data(ExecutionContext* context, XMLHttpRequest* request)
{
    RefPtr<TracedValue> value = TracedValue::create();
    value->setString("url", request->url().string());
    if (LocalFrame* frame = frameForExecutionContext(context))
        value->setString("frame", toHexString(frame));
    setCallStack(value.get());
    return value.release();
}

static void localToPageQuad(const LayoutObject& layoutObject, const LayoutRect& rect, FloatQuad* quad)
{
    LocalFrame* frame = layoutObject.frame();
    FrameView* view = frame->view();
    FloatQuad absolute = layoutObject.localToAbsoluteQuad(FloatQuad(rect));
    quad->setP1(view->contentsToRootFrame(roundedIntPoint(absolute.p1())));
    quad->setP2(view->contentsToRootFrame(roundedIntPoint(absolute.p2())));
    quad->setP3(view->contentsToRootFrame(roundedIntPoint(absolute.p3())));
    quad->setP4(view->contentsToRootFrame(roundedIntPoint(absolute.p4())));
}

const char InspectorLayerInvalidationTrackingEvent::SquashingLayerGeometryWasUpdated[] = "Squashing layer geometry was updated";
const char InspectorLayerInvalidationTrackingEvent::AddedToSquashingLayer[] = "The layer may have been added to an already-existing squashing layer";
const char InspectorLayerInvalidationTrackingEvent::RemovedFromSquashingLayer[] = "Removed the layer from a squashing layer";
const char InspectorLayerInvalidationTrackingEvent::ReflectionLayerChanged[] = "Reflection layer change";
const char InspectorLayerInvalidationTrackingEvent::NewCompositedLayer[] = "Assigned a new composited layer";

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorLayerInvalidationTrackingEvent::data(const DeprecatedPaintLayer* layer, const char* reason)
{
    const LayoutObject* paintInvalidationContainer = layer->layoutObject()->containerForPaintInvalidation();

    RefPtr<TracedValue> value = TracedValue::create();
    value->setString("frame", toHexString(paintInvalidationContainer->frame()));
    setGeneratingNodeInfo(value.get(), paintInvalidationContainer, "paintId");
    value->setString("reason", reason);
    return value.release();
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorPaintEvent::data(LayoutObject* layoutObject, const LayoutRect& clipRect, const GraphicsLayer* graphicsLayer)
{
    RefPtr<TracedValue> value = TracedValue::create();
    value->setString("frame", toHexString(layoutObject->frame()));
    FloatQuad quad;
    localToPageQuad(*layoutObject, clipRect, &quad);
    createQuad(value.get(), "clip", quad);
    setGeneratingNodeInfo(value.get(), layoutObject, "nodeId");
    int graphicsLayerId = graphicsLayer ? graphicsLayer->platformLayer()->id() : 0;
    value->setInteger("layerId", graphicsLayerId);
    setCallStack(value.get());
    return value.release();
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> frameEventData(LocalFrame* frame)
{
    RefPtr<TracedValue> value = TracedValue::create();
    value->setString("frame", toHexString(frame));
    bool isMainFrame = frame && frame->isMainFrame();
    value->setBoolean("isMainFrame", isMainFrame);
    value->setString("page", toHexString(frame));
    return value.release();
}


PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorCommitLoadEvent::data(LocalFrame* frame)
{
    return frameEventData(frame);
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorMarkLoadEvent::data(LocalFrame* frame)
{
    return frameEventData(frame);
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorScrollLayerEvent::data(LayoutObject* layoutObject)
{
    RefPtr<TracedValue> value = TracedValue::create();
    value->setString("frame", toHexString(layoutObject->frame()));
    setGeneratingNodeInfo(value.get(), layoutObject, "nodeId");
    return value.release();
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorUpdateLayerTreeEvent::data(LocalFrame* frame)
{
    RefPtr<TracedValue> value = TracedValue::create();
    value->setString("frame", toHexString(frame));
    return value.release();
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorEvaluateScriptEvent::data(LocalFrame* frame, const String& url, int lineNumber)
{
    RefPtr<TracedValue> value = TracedValue::create();
    value->setString("frame", toHexString(frame));
    value->setString("url", url);
    value->setInteger("lineNumber", lineNumber);
    setCallStack(value.get());
    return value.release();
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorFunctionCallEvent::data(ExecutionContext* context, int scriptId, const String& scriptName, int scriptLine)
{
    RefPtr<TracedValue> value = TracedValue::create();
    value->setString("scriptId", String::number(scriptId));
    value->setString("scriptName", scriptName);
    value->setInteger("scriptLine", scriptLine);
    if (LocalFrame* frame = frameForExecutionContext(context))
        value->setString("frame", toHexString(frame));
    setCallStack(value.get());
    return value.release();
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorPaintImageEvent::data(const LayoutImage& layoutImage)
{
    RefPtr<TracedValue> value = TracedValue::create();
    setGeneratingNodeInfo(value.get(), &layoutImage, "nodeId");
    if (const ImageResource* resource = layoutImage.cachedImage())
        value->setString("url", resource->url().string());
    return value.release();
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorPaintImageEvent::data(const LayoutObject& owningLayoutObject, const StyleImage& styleImage)
{
    RefPtr<TracedValue> value = TracedValue::create();
    setGeneratingNodeInfo(value.get(), &owningLayoutObject, "nodeId");
    if (const ImageResource* resource = styleImage.cachedImage())
        value->setString("url", resource->url().string());
    return value.release();
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorPaintImageEvent::data(const LayoutObject* owningLayoutObject, const ImageResource& imageResource)
{
    RefPtr<TracedValue> value = TracedValue::create();
    setGeneratingNodeInfo(value.get(), owningLayoutObject, "nodeId");
    value->setString("url", imageResource.url().string());
    return value.release();
}

static size_t usedHeapSize()
{
    v8::HeapStatistics heapStatistics;
    v8::Isolate::GetCurrent()->GetHeapStatistics(&heapStatistics);
    return heapStatistics.used_heap_size();
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorUpdateCountersEvent::data()
{
    RefPtr<TracedValue> value = TracedValue::create();
    if (isMainThread()) {
        value->setInteger("documents", InstanceCounters::counterValue(InstanceCounters::DocumentCounter));
        value->setInteger("nodes", InstanceCounters::counterValue(InstanceCounters::NodeCounter));
        value->setInteger("jsEventListeners", InstanceCounters::counterValue(InstanceCounters::JSEventListenerCounter));
    }
    value->setDouble("jsHeapSizeUsed", static_cast<double>(usedHeapSize()));
    return value.release();
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorInvalidateLayoutEvent::data(LocalFrame* frame)
{
    RefPtr<TracedValue> value = TracedValue::create();
    value->setString("frame", toHexString(frame));
    setCallStack(value.get());
    return value.release();
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorRecalculateStylesEvent::data(LocalFrame* frame)
{
    RefPtr<TracedValue> value = TracedValue::create();
    value->setString("frame", toHexString(frame));
    setCallStack(value.get());
    return value.release();
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorEventDispatchEvent::data(const Event& event)
{
    RefPtr<TracedValue> value = TracedValue::create();
    value->setString("type", event.type());
    setCallStack(value.get());
    return value.release();
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorTimeStampEvent::data(ExecutionContext* context, const String& message)
{
    RefPtr<TracedValue> value = TracedValue::create();
    value->setString("message", message);
    if (LocalFrame* frame = frameForExecutionContext(context))
        value->setString("frame", toHexString(frame));
    return value.release();
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorTracingSessionIdForWorkerEvent::data(const String& sessionId, const String& workerId, WorkerThread* workerThread)
{
    RefPtr<TracedValue> value = TracedValue::create();
    value->setString("sessionId", sessionId);
    value->setString("workerId", workerId);
    value->setDouble("workerThreadId", workerThread->platformThreadId());
    return value.release();
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorTracingStartedInFrame::data(const String& sessionId, LocalFrame* frame)
{
    RefPtr<TracedValue> value = TracedValue::create();
    value->setString("sessionId", sessionId);
    value->setString("page", toHexString(frame));
    return value.release();
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorSetLayerTreeId::data(const String& sessionId, int layerTreeId)
{
    RefPtr<TracedValue> value = TracedValue::create();
    value->setString("sessionId", sessionId);
    value->setInteger("layerTreeId", layerTreeId);
    return value.release();
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorAnimationEvent::data(const Animation& player)
{
    RefPtr<TracedValue> value = TracedValue::create();
    value->setString("id", String::number(player.sequenceNumber()));
    value->setString("state", player.playState());
    if (const AnimationEffect* effect = player.effect()) {
        value->setString("name", effect->name());
        if (effect->isAnimation()) {
            if (Element* target = toKeyframeEffect(effect)->target())
                setNodeInfo(value.get(), target, "nodeId", "nodeName");
        }
    }
    return value.release();
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorAnimationStateEvent::data(const Animation& player)
{
    RefPtr<TracedValue> value = TracedValue::create();
    value->setString("state", player.playState());
    return value.release();
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorHitTestEvent::endData(const HitTestRequest& request, const HitTestLocation& location, const HitTestResult& result)
{
    RefPtr<TracedValue> value(TracedValue::create());
    value->setInteger("x", location.roundedPoint().x());
    value->setInteger("y", location.roundedPoint().y());
    if (location.isRectBasedTest())
        value->setBoolean("rect", true);
    if (location.isRectilinear())
        value->setBoolean("rectilinear", true);
    if (request.touchEvent())
        value->setBoolean("touch", true);
    if (request.move())
        value->setBoolean("move", true);
    if (request.listBased())
        value->setBoolean("listBased", true);
    else if (Node* node = result.innerNode())
        setNodeInfo(value.get(), node, "nodeId", "nodeName");
    return value;
}

}
