#include "config.h"
//#include <inttypes.h>
#include "InspectorTraceEvents.h"
#include "platform/TracedValue.h"

//#if USING_VC6RT == 1
#define PRIx64 "llx"
//#endif

namespace blink {

static const unsigned maxInvalidationTrackingCallstackSize = 5;

String toHexString(const void* p)
{
    return String::format("0x%" PRIx64, static_cast<uint64_t>(reinterpret_cast<intptr_t>(p)));
}

String descendantInvalidationSetToIdString(const DescendantInvalidationSet& set)
{
    return toHexString(&set);
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

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorXhrReadyStateChangeEvent::data(ExecutionContext* context, XMLHttpRequest* request)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorXhrLoadEvent::data(ExecutionContext* context, XMLHttpRequest* request)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorParseHtmlEvent::beginData(Document* document, unsigned startLine)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorParseHtmlEvent::endData(unsigned endLine)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorLayoutEvent::beginData(FrameView* frameView)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorLayoutEvent::endData(LayoutObject* rootForThisLayout)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorParseAuthorStyleSheetEvent::data(const CSSStyleSheetResource* cachedStyleSheet)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TracedValue> InspectorStyleInvalidatorInvalidateEvent::fillCommonPart(Element& element, const char* reason)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorStyleInvalidatorInvalidateEvent::data(Element& element, const char* reason)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorStyleInvalidatorInvalidateEvent::selectorPart(Element& element, const char* reason, const DescendantInvalidationSet& invalidationSet, const String& selectorPart)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorStyleInvalidatorInvalidateEvent::invalidationList(Element& element, const WillBeHeapVector<RefPtrWillBeMember<DescendantInvalidationSet> >& invalidationList)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorLayerInvalidationTrackingEvent::data(const DeprecatedPaintLayer* layer, const char* reason)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorPaintEvent::data(LayoutObject* layoutObject, const LayoutRect& clipRect, const GraphicsLayer* graphicsLayer)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> frameEventData(LocalFrame* frame)
{
    notImplemented();
    return nullptr;
}


PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorCommitLoadEvent::data(LocalFrame* frame)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorMarkLoadEvent::data(LocalFrame* frame)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorScrollLayerEvent::data(LayoutObject* layoutObject)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorUpdateLayerTreeEvent::data(LocalFrame* frame)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorEvaluateScriptEvent::data(LocalFrame* frame, const String& url, int lineNumber)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorFunctionCallEvent::data(ExecutionContext* context, int scriptId, const String& scriptName, int scriptLine)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorPaintImageEvent::data(const LayoutImage& layoutImage)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorPaintImageEvent::data(const LayoutObject& owningLayoutObject, const StyleImage& styleImage)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorPaintImageEvent::data(const LayoutObject* owningLayoutObject, const ImageResource& imageResource)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorUpdateCountersEvent::data()
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorInvalidateLayoutEvent::data(LocalFrame* frame)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorRecalculateStylesEvent::data(LocalFrame* frame)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorEventDispatchEvent::data(const Event& event)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorTimeStampEvent::data(ExecutionContext* context, const String& message)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorTracingSessionIdForWorkerEvent::data(const String& sessionId, const String& workerId, WorkerThread* workerThread)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorTracingStartedInFrame::data(const String& sessionId, LocalFrame* frame)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorSetLayerTreeId::data(const String& sessionId, int layerTreeId)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorAnimationEvent::data(const Animation& player)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorAnimationStateEvent::data(const Animation& player)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorHitTestEvent::endData(const HitTestRequest& request, const HitTestLocation& location, const HitTestResult& result)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorTimerInstallEvent::data(ExecutionContext* context, int timerId, int timeout, bool singleShot)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorTimerRemoveEvent::data(ExecutionContext* context, int timerId)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorTimerFireEvent::data(ExecutionContext* context, int timerId)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorAnimationFrameEvent::data(ExecutionContext* context, int callbackId)
{
    notImplemented();
    return nullptr;
}

PassRefPtr<TraceEvent::ConvertableToTraceFormat> InspectorStyleRecalcInvalidationTrackingEvent::data(Node*, const StyleChangeReasonForTracing&)
{
    notImplemented();
    return nullptr;
}


} // namespace blink