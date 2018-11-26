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

// How ownership works
// -------------------
//
// Big oh represents a refcounted relationship: owner O--- ownee
//
// WebView (for the toplevel frame only)
//    O
//    |           WebFrame
//    |              O
//    |              |
//   Page O------- LocalFrame (m_mainFrame) O-------O FrameView
//                   ||
//                   ||
//               FrameLoader
//
// FrameLoader and LocalFrame are formerly one object that was split apart because
// it got too big. They basically have the same lifetime, hence the double line.
//
// From the perspective of the embedder, WebFrame is simply an object that it
// allocates by calling WebFrame::create() and must be freed by calling close().
// Internally, WebFrame is actually refcounted and it holds a reference to its
// corresponding LocalFrame in blink.
//
// Oilpan: the middle objects + Page in the above diagram are Oilpan heap allocated,
// WebView and FrameView are currently not. In terms of ownership and control, the
// relationships stays the same, but the references from the off-heap WebView to the
// on-heap Page is handled by a Persistent<>, not a RefPtr<>. Similarly, the mutual
// strong references between the on-heap LocalFrame and the off-heap FrameView
// is through a RefPtr (from LocalFrame to FrameView), and a Persistent refers
// to the LocalFrame in the other direction.
//
// From the embedder's point of view, the use of Oilpan brings no changes. close()
// must still be used to signal that the embedder is through with the WebFrame.
// Calling it will bring about the release and finalization of the frame object,
// and everything underneath.
//
// How frames are destroyed
// ------------------------
//
// The main frame is never destroyed and is re-used. The FrameLoader is re-used
// and a reference to the main frame is kept by the Page.
//
// When frame content is replaced, all subframes are destroyed. This happens
// in FrameLoader::detachFromParent for each subframe in a pre-order depth-first
// traversal. Note that child node order may not match DOM node order!
// detachFromParent() calls FrameLoaderClient::detachedFromParent(), which calls
// WebFrame::frameDetached(). This triggers WebFrame to clear its reference to
// LocalFrame, and also notifies the embedder via WebFrameClient that the frame is
// detached. Most embedders will invoke close() on the WebFrame at this point,
// triggering its deletion unless something else is still retaining a reference.
//
// The client is expected to be set whenever the WebLocalFrameImpl is attached to
// the DOM.

#include "config.h"
#include "web/WebLocalFrameImpl.h"

#include "bindings/core/v8/BindingSecurity.h"
#include "bindings/core/v8/DOMWrapperWorld.h"
#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "bindings/core/v8/ScriptController.h"
#include "bindings/core/v8/ScriptSourceCode.h"
#include "bindings/core/v8/ScriptValue.h"
#include "bindings/core/v8/V8Binding.h"
#include "bindings/core/v8/V8GCController.h"
#include "bindings/core/v8/V8PerIsolateData.h"
#include "core/HTMLNames.h"
#include "core/dom/Document.h"
#include "core/dom/IconURL.h"
#include "core/dom/MessagePort.h"
#include "core/dom/Node.h"
#include "core/dom/NodeTraversal.h"
#include "core/dom/SuspendableTask.h"
#include "core/dom/shadow/ShadowRoot.h"
#include "core/editing/Editor.h"
#include "core/editing/FrameSelection.h"
#include "core/editing/InputMethodController.h"
#include "core/editing/PlainTextRange.h"
#include "core/editing/SpellChecker.h"
#include "core/editing/TextAffinity.h"
#include "core/editing/htmlediting.h"
#include "core/editing/iterators/TextIterator.h"
#include "core/editing/markup.h"
#include "core/fetch/ResourceFetcher.h"
#include "core/fetch/SubstituteData.h"
#include "core/frame/Console.h"
#include "core/frame/LocalDOMWindow.h"
#include "core/frame/FrameHost.h"
#include "core/frame/FrameView.h"
#include "core/frame/Settings.h"
#include "core/html/HTMLAnchorElement.h"
#include "core/html/HTMLCollection.h"
#include "core/html/HTMLFormElement.h"
#include "core/html/HTMLFrameElementBase.h"
#include "core/html/HTMLFrameOwnerElement.h"
#include "core/html/HTMLHeadElement.h"
#include "core/html/HTMLImageElement.h"
#include "core/html/HTMLInputElement.h"
#include "core/html/HTMLLinkElement.h"
#include "core/html/PluginDocument.h"
#include "core/input/EventHandler.h"
#include "core/inspector/ConsoleMessage.h"
#include "core/inspector/ScriptCallStack.h"
#include "core/layout/HitTestResult.h"
#include "core/layout/LayoutBox.h"
#include "core/layout/LayoutObject.h"
#include "core/layout/LayoutPart.h"
#include "core/layout/LayoutTreeAsText.h"
#include "core/layout/LayoutView.h"
#include "core/style/StyleInheritedData.h"
#include "core/loader/DocumentLoader.h"
#include "core/loader/FrameLoadRequest.h"
#include "core/loader/FrameLoader.h"
#include "core/loader/HistoryItem.h"
#include "core/loader/MixedContentChecker.h"
#include "core/page/FocusController.h"
#include "core/page/FrameTree.h"
#include "core/page/Page.h"
#include "core/page/PrintContext.h"
#include "core/paint/DeprecatedPaintLayer.h"
#include "core/paint/ScopeRecorder.h"
#include "core/paint/TransformRecorder.h"
#include "core/timing/DOMWindowPerformance.h"
#include "core/timing/Performance.h"
#include "modules/app_banner/AppBannerController.h"
#include "modules/geolocation/GeolocationController.h"
#include "modules/notifications/NotificationPermissionClient.h"
#include "modules/permissions/PermissionController.h"
#include "modules/presentation/PresentationController.h"
#include "modules/push_messaging/PushController.h"
#include "modules/screen_orientation/ScreenOrientationController.h"
#include "modules/vr/VRController.h"
#include "platform/ScriptForbiddenScope.h"
#include "platform/TraceEvent.h"
#include "platform/UserGestureIndicator.h"
#include "platform/clipboard/ClipboardUtilities.h"
#include "platform/fonts/FontCache.h"
#include "platform/graphics/GraphicsContext.h"
#include "platform/graphics/GraphicsLayerClient.h"
#include "platform/graphics/paint/ClipRecorder.h"
#include "platform/graphics/paint/DrawingRecorder.h"
#include "platform/graphics/paint/SkPictureBuilder.h"
#include "platform/graphics/skia/SkiaUtils.h"
#include "platform/heap/Handle.h"
#include "platform/network/ResourceRequest.h"
#include "platform/scroll/ScrollTypes.h"
#include "platform/scroll/ScrollbarTheme.h"
#include "platform/weborigin/KURL.h"
#include "platform/weborigin/SchemeRegistry.h"
#include "platform/weborigin/SecurityPolicy.h"
#include "public/platform/Platform.h"
#include "public/platform/WebFloatPoint.h"
#include "public/platform/WebFloatRect.h"
#include "public/platform/WebLayer.h"
#include "public/platform/WebPoint.h"
#include "public/platform/WebRect.h"
#include "public/platform/WebSecurityOrigin.h"
#include "public/platform/WebSize.h"
#include "public/platform/WebSuspendableTask.h"
#include "public/platform/WebURLError.h"
#include "public/platform/WebVector.h"
#include "public/web/WebAutofillClient.h"
#include "public/web/WebConsoleMessage.h"
#include "public/web/WebDOMEvent.h"
#include "public/web/WebDocument.h"
#include "public/web/WebFindOptions.h"
#include "public/web/WebFormElement.h"
#include "public/web/WebFrameClient.h"
#include "public/web/WebHistoryItem.h"
#include "public/web/WebIconURL.h"
#include "public/web/WebInputElement.h"
#include "public/web/WebKit.h"
#include "public/web/WebNode.h"
#include "public/web/WebPerformance.h"
#include "public/web/WebPlugin.h"
#include "public/web/WebPrintParams.h"
#include "public/web/WebPrintPresetOptions.h"
#include "public/web/WebRange.h"
#include "public/web/WebScriptSource.h"
#include "public/web/WebSerializedScriptValue.h"
#include "public/web/WebTestInterfaceFactory.h"
#include "public/web/WebTreeScopeType.h"
#include "skia/ext/platform_device.h"
#include "web/AssociatedURLLoader.h"
#include "web/CompositionUnderlineVectorBuilder.h"
#include "web/FindInPageCoordinates.h"
#include "web/GeolocationClientProxy.h"
#include "web/InspectorOverlayImpl.h"
#include "web/LocalFileSystemClient.h"
#include "web/MIDIClientProxy.h"
#include "web/NavigatorContentUtilsClientImpl.h"
#include "web/NotificationPermissionClientImpl.h"
#include "web/PageOverlay.h"
#include "web/RemoteBridgeFrameOwner.h"
//#include "web/SharedWorkerRepositoryClientImpl.h"
#include "web/SuspendableScriptExecutor.h"
#include "web/TextFinder.h"
#include "web/WebDataSourceImpl.h"
#include "web/WebDevToolsAgentImpl.h"
#include "web/WebFrameWidgetImpl.h"
#include "web/WebPluginContainerImpl.h"
#include "web/WebRemoteFrameImpl.h"
#include "web/WebViewImpl.h"
#include "wtf/CurrentTime.h"
#include "wtf/HashMap.h"
#include "wtf/RefCountedLeakCounter.h"
#include <algorithm>

namespace blink {

static int frameCount = 0;

// Key for a StatsCounter tracking how many WebFrames are active.
static const char webFrameActiveCount[] = "WebFrameActiveCount";

static void frameContentAsPlainText(size_t maxChars, LocalFrame* frame, StringBuilder& output)
{
    Document* document = frame->document();
    if (!document)
        return;

    if (!frame->view())
        return;

    // Select the document body.
    RefPtrWillBeRawPtr<Range> range(document->createRange());
    TrackExceptionState exceptionState;
    range->selectNodeContents(document->body(), exceptionState);

    if (!exceptionState.hadException()) {
        // The text iterator will walk nodes giving us text. This is similar to
        // the plainText() function in core/editing/TextIterator.h, but we implement the maximum
        // size and also copy the results directly into a wstring, avoiding the
        // string conversion.
        for (TextIterator it(range->startPosition(), range->endPosition()); !it.atEnd(); it.advance()) {
            it.text().appendTextToStringBuilder(output, 0, maxChars - output.length());
            if (output.length() >= maxChars)
                return; // Filled up the buffer.
        }
    }

    // The separator between frames when the frames are converted to plain text.
    const LChar frameSeparator[] = { '\n', '\n' };
    const size_t frameSeparatorLength = WTF_ARRAY_LENGTH(frameSeparator);

    // Recursively walk the children.
    const FrameTree& frameTree = frame->tree();
    for (Frame* curChild = frameTree.firstChild(); curChild; curChild = curChild->tree().nextSibling()) {
        if (!curChild->isLocalFrame())
            continue;
        LocalFrame* curLocalChild = toLocalFrame(curChild);
        // Ignore the text of non-visible frames.
        LayoutView* contentLayoutObject = curLocalChild->contentLayoutObject();
        LayoutPart* ownerLayoutObject = curLocalChild->ownerLayoutObject();
        if (!contentLayoutObject || !contentLayoutObject->size().width() || !contentLayoutObject->size().height()
            || (contentLayoutObject->location().x() + contentLayoutObject->size().width() <= 0) || (contentLayoutObject->location().y() + contentLayoutObject->size().height() <= 0)
            || (ownerLayoutObject && ownerLayoutObject->style() && ownerLayoutObject->style()->visibility() != VISIBLE)) {
            continue;
        }

        // Make sure the frame separator won't fill up the buffer, and give up if
        // it will. The danger is if the separator will make the buffer longer than
        // maxChars. This will cause the computation above:
        //   maxChars - output->size()
        // to be a negative number which will crash when the subframe is added.
        if (output.length() >= maxChars - frameSeparatorLength)
            return;

        output.append(frameSeparator, frameSeparatorLength);
        frameContentAsPlainText(maxChars, curLocalChild, output);
        if (output.length() >= maxChars)
            return; // Filled up the buffer.
    }
}

static WillBeHeapVector<ScriptSourceCode> createSourcesVector(const WebScriptSource* sourcesIn, unsigned numSources)
{
    WillBeHeapVector<ScriptSourceCode> sources;
    sources.append(sourcesIn, numSources);
    return sources;
}

WebPluginContainerImpl* WebLocalFrameImpl::pluginContainerFromFrame(LocalFrame* frame)
{
    if (!frame)
        return 0;
    if (!frame->document() || !frame->document()->isPluginDocument())
        return 0;
    PluginDocument* pluginDocument = toPluginDocument(frame->document());
    return toWebPluginContainerImpl(pluginDocument->pluginWidget());
}

WebPluginContainerImpl* WebLocalFrameImpl::pluginContainerFromNode(LocalFrame* frame, const WebNode& node)
{
    WebPluginContainerImpl* pluginContainer = pluginContainerFromFrame(frame);
    if (pluginContainer)
        return pluginContainer;
    return toWebPluginContainerImpl(node.pluginContainer());
}

// Simple class to override some of PrintContext behavior. Some of the methods
// made virtual so that they can be overridden by ChromePluginPrintContext.
class ChromePrintContext : public PrintContext {
    WTF_MAKE_NONCOPYABLE(ChromePrintContext);
public:
    ChromePrintContext(LocalFrame* frame)
        : PrintContext(frame)
        , m_printedPageWidth(0)
    {
    }

    ~ChromePrintContext() override {}

    virtual void begin(float width, float height)
    {
        ASSERT(!m_printedPageWidth);
        m_printedPageWidth = width;
        PrintContext::begin(m_printedPageWidth, height);
    }

    virtual float getPageShrink(int pageNumber) const
    {
        IntRect pageRect = m_pageRects[pageNumber];
        return m_printedPageWidth / pageRect.width();
    }

    float spoolSinglePage(WebCanvas* canvas, int pageNumber)
    {
        dispatchEventsForPrintingOnAllFrames();
        if (!frame()->document() || !frame()->document()->layoutView())
            return 0;

        frame()->view()->updateAllLifecyclePhases();
        if (!frame()->document() || !frame()->document()->layoutView())
            return 0;

        IntRect pageRect = m_pageRects[pageNumber];
        SkPictureBuilder pictureBuilder(pageRect, &skia::getMetaData(*canvas));
        pictureBuilder.context().setPrinting(true);

        float scale = spoolPage(pictureBuilder.context(), pageNumber);
        pictureBuilder.endRecording()->playback(canvas);
        outputLinkedDestinations(canvas, pageRect);
        return scale;
    }

    void spoolAllPagesWithBoundaries(WebCanvas* canvas, const FloatSize& pageSizeInPixels)
    {
        dispatchEventsForPrintingOnAllFrames();
        if (!frame()->document() || !frame()->document()->layoutView())
            return;

        frame()->view()->updateAllLifecyclePhases();
        if (!frame()->document() || !frame()->document()->layoutView())
            return;

        float pageHeight;
        computePageRects(FloatRect(FloatPoint(0, 0), pageSizeInPixels), 0, 0, 1, pageHeight);

        const float pageWidth = pageSizeInPixels.width();
        size_t numPages = pageRects().size();
        int totalHeight = numPages * (pageSizeInPixels.height() + 1) - 1;
        IntRect allPagesRect(0, 0, pageWidth, totalHeight);

        SkPictureBuilder pictureBuilder(allPagesRect, &skia::getMetaData(*canvas));
        pictureBuilder.context().setPrinting(true);

        GraphicsContext& context = pictureBuilder.context();

        // Fill the whole background by white.
        if (!DrawingRecorder::useCachedDrawingIfPossible(context, *this, DisplayItem::PrintedContentBackground)) {
            DrawingRecorder backgroundRecorder(context, *this, DisplayItem::PrintedContentBackground, allPagesRect);
            context.fillRect(FloatRect(0, 0, pageWidth, totalHeight), Color::white);
        }

        int currentHeight = 0;
        for (size_t pageIndex = 0; pageIndex < numPages; pageIndex++) {
            ScopeRecorder scopeRecorder(context, *this);
            // Draw a line for a page boundary if this isn't the first page.
            if (pageIndex > 0 && !DrawingRecorder::useCachedDrawingIfPossible(context, *this, DisplayItem::PrintedContentLineBoundary)) {
                DrawingRecorder lineBoundaryRecorder(context, *this, DisplayItem::PrintedContentLineBoundary, allPagesRect);
                context.save();
                context.setStrokeColor(Color(0, 0, 255));
                context.setFillColor(Color(0, 0, 255));
                context.drawLine(IntPoint(0, currentHeight), IntPoint(pageWidth, currentHeight));
                context.restore();
            }

            AffineTransform transform;
            transform.translate(0, currentHeight);
#if OS(WIN) || OS(MACOSX)
            // Account for the disabling of scaling in spoolPage. In the context
            // of spoolAllPagesWithBoundaries the scale HAS NOT been pre-applied.
            float scale = getPageShrink(pageIndex);
            transform.scale(scale, scale);
#endif
            TransformRecorder transformRecorder(context, *this, transform);
            spoolPage(context, pageIndex);

            currentHeight += pageSizeInPixels.height() + 1;
        }
        pictureBuilder.endRecording()->playback(canvas);
        outputLinkedDestinations(canvas, allPagesRect);
    }

    DisplayItemClient displayItemClient() const { return toDisplayItemClient(this); }

    String debugName() const { return "ChromePrintContext"; }

protected:
    // Spools the printed page, a subrect of frame(). Skip the scale step.
    // NativeTheme doesn't play well with scaling. Scaling is done browser side
    // instead. Returns the scale to be applied.
    // On Linux, we don't have the problem with NativeTheme, hence we let WebKit
    // do the scaling and ignore the return value.
    virtual float spoolPage(GraphicsContext& context, int pageNumber)
    {
        IntRect pageRect = m_pageRects[pageNumber];
        float scale = m_printedPageWidth / pageRect.width();

        AffineTransform transform;
#if OS(POSIX) && !OS(MACOSX)
        transform.scale(scale);
#endif
        transform.translate(static_cast<float>(-pageRect.x()), static_cast<float>(-pageRect.y()));
        TransformRecorder transformRecorder(context, *this, transform);

        ClipRecorder clipRecorder(context, *this, DisplayItem::ClipPrintedPage, LayoutRect(pageRect));

        frame()->view()->paintContents(&context, pageRect);

        return scale;
    }

private:
    void dispatchEventsForPrintingOnAllFrames()
    {
        WillBeHeapVector<RefPtrWillBeMember<Document>> documents;
        for (Frame* currentFrame = frame(); currentFrame; currentFrame = currentFrame->tree().traverseNext(frame())) {
            if (currentFrame->isLocalFrame())
                documents.append(toLocalFrame(currentFrame)->document());
        }

        for (auto& doc : documents)
            doc->dispatchEventsForPrinting();
    }

    // Set when printing.
    float m_printedPageWidth;
};

// Simple class to override some of PrintContext behavior. This is used when
// the frame hosts a plugin that supports custom printing. In this case, we
// want to delegate all printing related calls to the plugin.
class ChromePluginPrintContext final : public ChromePrintContext {
public:
    ChromePluginPrintContext(LocalFrame* frame, WebPluginContainerImpl* plugin, const WebPrintParams& printParams)
        : ChromePrintContext(frame), m_plugin(plugin), m_printParams(printParams)
    {
    }

    ~ChromePluginPrintContext() override {}

    void begin(float width, float height) override
    {
    }

    void end() override
    {
        m_plugin->printEnd();
    }

    float getPageShrink(int pageNumber) const override
    {
        // We don't shrink the page (maybe we should ask the widget ??)
        return 1.0;
    }

    void computePageRects(const FloatRect& printRect, float headerHeight, float footerHeight, float userScaleFactor, float& outPageHeight) override
    {
        m_printParams.printContentArea = IntRect(printRect);
        m_pageRects.fill(IntRect(printRect), m_plugin->printBegin(m_printParams));
    }

    void computePageRectsWithPageSize(const FloatSize& pageSizeInPixels) override
    {
        ASSERT_NOT_REACHED();
    }

protected:
    // Spools the printed page, a subrect of frame(). Skip the scale step.
    // NativeTheme doesn't play well with scaling. Scaling is done browser side
    // instead. Returns the scale to be applied.
    float spoolPage(GraphicsContext& context, int pageNumber) override
    {
        IntRect pageRect = m_pageRects[pageNumber];
        m_plugin->printPage(pageNumber, &context, pageRect);

        return 1.0;
    }

private:
    // Set when printing.
    WebPluginContainerImpl* m_plugin;
    WebPrintParams m_printParams;
};

static WebDataSource* DataSourceForDocLoader(DocumentLoader* loader)
{
    return loader ? WebDataSourceImpl::fromDocumentLoader(loader) : 0;
}

// WebSuspendableTaskWrapper --------------------------------------------------

class WebSuspendableTaskWrapper: public SuspendableTask {
public:
    static PassOwnPtr<WebSuspendableTaskWrapper> create(PassOwnPtr<WebSuspendableTask> task)
    {
        return adoptPtr(new WebSuspendableTaskWrapper(task));
    }

    void run() override
    {
        m_task->run();
    }

    void contextDestroyed() override
    {
        m_task->contextDestroyed();
    }

private:
    explicit WebSuspendableTaskWrapper(PassOwnPtr<WebSuspendableTask> task)
        : m_task(task)
    {
    }

    OwnPtr<WebSuspendableTask> m_task;
};

// WebFrame -------------------------------------------------------------------

int WebFrame::instanceCount()
{
    return frameCount;
}

WebLocalFrame* WebLocalFrame::frameForCurrentContext()
{
    v8::Local<v8::Context> context = v8::Isolate::GetCurrent()->GetCurrentContext();
    if (context.IsEmpty())
        return 0;
    return frameForContext(context);
}

WebLocalFrame* WebLocalFrame::frameForContext(v8::Local<v8::Context> context)
{
    return WebLocalFrameImpl::fromFrame(toLocalFrame(toFrameIfNotDetached(context)));
}

WebLocalFrame* WebLocalFrame::fromFrameOwnerElement(const WebElement& element)
{
    return WebLocalFrameImpl::fromFrameOwnerElement(PassRefPtrWillBeRawPtr<Element>(element).get());
}

bool WebLocalFrameImpl::isWebLocalFrame() const
{
    return true;
}

WebLocalFrame* WebLocalFrameImpl::toWebLocalFrame()
{
    return this;
}

bool WebLocalFrameImpl::isWebRemoteFrame() const
{
    return false;
}

WebRemoteFrame* WebLocalFrameImpl::toWebRemoteFrame()
{
    ASSERT_NOT_REACHED();
    return 0;
}

void WebLocalFrameImpl::close()
{
    m_client = nullptr;

    if (m_devToolsAgent) {
        m_devToolsAgent->dispose();
        m_devToolsAgent.clear();
    }

#if ENABLE(OILPAN)
    m_selfKeepAlive.clear();
#else
    deref(); // Balances ref() acquired in WebFrame::create
#endif
}

WebString WebLocalFrameImpl::uniqueName() const
{
    return frame()->tree().uniqueName();
}

WebString WebLocalFrameImpl::assignedName() const
{
    return frame()->tree().name();
}

void WebLocalFrameImpl::setName(const WebString& name)
{
    frame()->tree().setName(name);
}

WebVector<WebIconURL> WebLocalFrameImpl::iconURLs(int iconTypesMask) const
{
    // The URL to the icon may be in the header. As such, only
    // ask the loader for the icon if it's finished loading.
    if (frame()->document()->loadEventFinished())
        return frame()->document()->iconURLs(iconTypesMask);
    return WebVector<WebIconURL>();
}

void WebLocalFrameImpl::setRemoteWebLayer(WebLayer* webLayer)
{
    ASSERT_NOT_REACHED();
}

void WebLocalFrameImpl::setContentSettingsClient(WebContentSettingsClient* contentSettingsClient)
{
    m_contentSettingsClient = contentSettingsClient;
}

void WebLocalFrameImpl::setSharedWorkerRepositoryClient(WebSharedWorkerRepositoryClient* client)
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    m_sharedWorkerRepositoryClient = SharedWorkerRepositoryClientImpl::create(client);
#endif // MINIBLINK_NOT_IMPLEMENTED
    notImplemented();
}

ScrollableArea* WebLocalFrameImpl::layoutViewportScrollableArea() const
{
    if (FrameView* view = frameView())
        return view->layoutViewportScrollableArea();
    return nullptr;
}

WebSize WebLocalFrameImpl::scrollOffset() const
{
    if (ScrollableArea* scrollableArea = layoutViewportScrollableArea())
        return toIntSize(scrollableArea->scrollPosition());
    return WebSize();
}

WebSize WebLocalFrameImpl::minimumScrollOffset() const
{
    if (ScrollableArea* scrollableArea = layoutViewportScrollableArea())
        return toIntSize(scrollableArea->minimumScrollPosition());
    return WebSize();
}

WebSize WebLocalFrameImpl::maximumScrollOffset() const
{
    if (ScrollableArea* scrollableArea = layoutViewportScrollableArea())
        return toIntSize(scrollableArea->maximumScrollPosition());
    return WebSize();
}

void WebLocalFrameImpl::setScrollOffset(const WebSize& offset)
{
    if (ScrollableArea* scrollableArea = layoutViewportScrollableArea())
        scrollableArea->setScrollPosition(IntPoint(offset.width, offset.height), ProgrammaticScroll);
}

WebSize WebLocalFrameImpl::contentsSize() const
{
    if (FrameView* view = frameView())
        return view->contentsSize();
    return WebSize();
}

bool WebLocalFrameImpl::hasVisibleContent() const
{
    if (LayoutPart* layoutObject = frame()->ownerLayoutObject()) {
        if (layoutObject->style()->visibility() != VISIBLE)
            return false;
    }

    if (FrameView* view = frameView())
        return view->visibleWidth() > 0 && view->visibleHeight() > 0;
    return false;
}

WebRect WebLocalFrameImpl::visibleContentRect() const
{
    if (FrameView* view = frameView())
        return view->visibleContentRect();
    return WebRect();
}

bool WebLocalFrameImpl::hasHorizontalScrollbar() const
{
    return frame() && frame()->view() && frame()->view()->horizontalScrollbar();
}

bool WebLocalFrameImpl::hasVerticalScrollbar() const
{
    return frame() && frame()->view() && frame()->view()->verticalScrollbar();
}

WebView* WebLocalFrameImpl::view() const
{
    return viewImpl();
}

void WebLocalFrameImpl::setOpener(WebFrame* opener)
{
    // FIXME: Does this need to move up into WebFrame too?
    if (WebFrame::opener() && !opener && m_client)
        m_client->didDisownOpener(this);

    WebFrame::setOpener(opener);

    ASSERT(m_frame);
    if (m_frame && m_frame->document())
        m_frame->document()->initSecurityContext();
}

WebDocument WebLocalFrameImpl::document() const
{
    if (!frame() || !frame()->document())
        return WebDocument();
    return WebDocument(frame()->document());
}

#ifdef MINIBLINK_NOT_IMPLEMENTED
WebPerformance WebLocalFrameImpl::performance() const
{
    if (!frame())
        return WebPerformance();
    return WebPerformance(DOMWindowPerformance::performance(*(frame()->domWindow())));
}
#endif // MINIBLINK_NOT_IMPLEMENTED

bool WebLocalFrameImpl::dispatchBeforeUnloadEvent()
{
    if (!frame())
        return true;
    return frame()->loader().shouldClose();
}

void WebLocalFrameImpl::dispatchUnloadEvent()
{
    if (!frame())
        return;
    frame()->loader().dispatchUnloadEvent();
}

NPObject* WebLocalFrameImpl::windowObject() const
{
    if (!frame() || ScriptForbiddenScope::isScriptForbidden())
        return 0;
    return frame()->script().windowScriptNPObject();
}

void WebLocalFrameImpl::bindToWindowObject(const WebString& name, NPObject* object)
{
    bindToWindowObject(name, object, 0);
}

void WebLocalFrameImpl::bindToWindowObject(const WebString& name, NPObject* object, void*)
{
    if (!frame() || !frame()->script().canExecuteScripts(NotAboutToExecuteScript))
        return;
    frame()->script().bindToWindowObject(frame(), String(name), object);
}

void WebLocalFrameImpl::executeScript(const WebScriptSource& source)
{
    ASSERT(frame());
    TextPosition position(OrdinalNumber::fromOneBasedInt(source.startLine), OrdinalNumber::first());
    v8::HandleScope handleScope(toIsolate(frame()));
    frame()->script().executeScriptInMainWorld(ScriptSourceCode(source.code, source.url, position));
}

void WebLocalFrameImpl::executeScriptInIsolatedWorld(int worldID, const WebScriptSource* sourcesIn, unsigned numSources, int extensionGroup)
{
    ASSERT(frame());
    RELEASE_ASSERT(worldID > 0);
    RELEASE_ASSERT(worldID < EmbedderWorldIdLimit);

    WillBeHeapVector<ScriptSourceCode> sources = createSourcesVector(sourcesIn, numSources);
    v8::HandleScope handleScope(toIsolate(frame()));
    frame()->script().executeScriptInIsolatedWorld(worldID, sources, extensionGroup, 0);
}

void WebLocalFrameImpl::setIsolatedWorldSecurityOrigin(int worldID, const WebSecurityOrigin& securityOrigin)
{
    ASSERT(frame());
    DOMWrapperWorld::setIsolatedWorldSecurityOrigin(worldID, securityOrigin.get());
}

void WebLocalFrameImpl::setIsolatedWorldContentSecurityPolicy(int worldID, const WebString& policy)
{
    ASSERT(frame());
    DOMWrapperWorld::setIsolatedWorldContentSecurityPolicy(worldID, policy);
}

void WebLocalFrameImpl::setIsolatedWorldHumanReadableName(int worldID, const WebString& humanReadableName)
{
    ASSERT(frame());
    DOMWrapperWorld::setIsolatedWorldHumanReadableName(worldID, humanReadableName);
}

void WebLocalFrameImpl::addMessageToConsole(const WebConsoleMessage& message)
{
    ASSERT(frame());

    MessageLevel webCoreMessageLevel;
    switch (message.level) {
    case WebConsoleMessage::LevelDebug:
        webCoreMessageLevel = DebugMessageLevel;
        break;
    case WebConsoleMessage::LevelLog:
        webCoreMessageLevel = LogMessageLevel;
        break;
    case WebConsoleMessage::LevelWarning:
        webCoreMessageLevel = WarningMessageLevel;
        break;
    case WebConsoleMessage::LevelError:
        webCoreMessageLevel = ErrorMessageLevel;
        break;
    default:
        ASSERT_NOT_REACHED();
        return;
    }

    frame()->document()->addConsoleMessage(ConsoleMessage::create(OtherMessageSource, webCoreMessageLevel, message.text));
}

void WebLocalFrameImpl::collectGarbage()
{
    if (!frame())
        return;
    if (!frame()->settings()->scriptEnabled())
        return;
    V8GCController::collectGarbage(v8::Isolate::GetCurrent());
}

bool WebLocalFrameImpl::checkIfRunInsecureContent(const WebURL& url) const
{
    ASSERT(frame());

    // This is only called (eventually, through proxies and delegates and IPC) from
    // PluginURLFetcher::OnReceivedRedirect for redirects of NPAPI resources.
    //
    // FIXME: Remove this method entirely once we smother NPAPI.
    return !MixedContentChecker::shouldBlockFetch(frame(), WebURLRequest::RequestContextObject, WebURLRequest::FrameTypeNested, url);
}

v8::Local<v8::Value> WebLocalFrameImpl::executeScriptAndReturnValue(const WebScriptSource& source)
{
    ASSERT(frame());

    TextPosition position(OrdinalNumber::fromOneBasedInt(source.startLine), OrdinalNumber::first());
    return frame()->script().executeScriptInMainWorldAndReturnValue(ScriptSourceCode(source.code, source.url, position));
}

void WebLocalFrameImpl::requestExecuteScriptAndReturnValue(const WebScriptSource& source, bool userGesture, WebScriptExecutionCallback* callback)
{
    ASSERT(frame());
    SuspendableScriptExecutor::createAndRun(frame(), 0, createSourcesVector(&source, 1), 0, userGesture, callback);
}

void WebLocalFrameImpl::executeScriptInIsolatedWorld(int worldID, const WebScriptSource* sourcesIn, unsigned numSources, int extensionGroup, WebVector<v8::Local<v8::Value>>* results)
{
    ASSERT(frame());
    RELEASE_ASSERT(worldID > 0);
    RELEASE_ASSERT(worldID < EmbedderWorldIdLimit);

    WillBeHeapVector<ScriptSourceCode> sources = createSourcesVector(sourcesIn, numSources);

    if (results) {
        Vector<v8::Local<v8::Value>> scriptResults;
        frame()->script().executeScriptInIsolatedWorld(worldID, sources, extensionGroup, &scriptResults);
        WebVector<v8::Local<v8::Value>> v8Results(scriptResults.size());
        for (unsigned i = 0; i < scriptResults.size(); i++)
            v8Results[i] = v8::Local<v8::Value>::New(toIsolate(frame()), scriptResults[i]);
        results->swap(v8Results);
    } else {
        v8::HandleScope handleScope(toIsolate(frame()));
        frame()->script().executeScriptInIsolatedWorld(worldID, sources, extensionGroup, 0);
    }
}

void WebLocalFrameImpl::requestExecuteScriptInIsolatedWorld(int worldID, const WebScriptSource* sourcesIn, unsigned numSources, int extensionGroup, bool userGesture, WebScriptExecutionCallback* callback)
{
    ASSERT(frame());
    RELEASE_ASSERT(worldID > 0);
    RELEASE_ASSERT(worldID < EmbedderWorldIdLimit);
    SuspendableScriptExecutor::createAndRun(frame(), worldID, createSourcesVector(sourcesIn, numSources), extensionGroup, userGesture, callback);
}

// TODO(bashi): Consider returning MaybeLocal.
v8::Local<v8::Value> WebLocalFrameImpl::callFunctionEvenIfScriptDisabled(v8::Local<v8::Function> function, v8::Local<v8::Value> receiver, int argc, v8::Local<v8::Value> argv[])
{
    ASSERT(frame());
    v8::Local<v8::Value> result;
    if (!frame()->script().callFunction(function, receiver, argc, static_cast<v8::Local<v8::Value>*>(argv)).ToLocal(&result))
        return v8::Local<v8::Value>();
    return result;
}

v8::Local<v8::Context> WebLocalFrameImpl::mainWorldScriptContext() const
{
    ScriptState* scriptState = ScriptState::forMainWorld(frame());
    ASSERT(scriptState->contextIsValid());
    return scriptState->context();
}

bool WebFrame::scriptCanAccess(WebFrame* target)
{
    return BindingSecurity::shouldAllowAccessToFrame(mainThreadIsolate(), toCoreFrame(target), DoNotReportSecurityError);
}

void WebLocalFrameImpl::reload(bool ignoreCache)
{
    // TODO(clamy): Remove this function once RenderFrame calls load for all
    // requests.
    reloadWithOverrideURL(KURL(), ignoreCache);
}

void WebLocalFrameImpl::reloadWithOverrideURL(const WebURL& overrideUrl, bool ignoreCache)
{
    // TODO(clamy): Remove this function once RenderFrame calls load for all
    // requests.
    ASSERT(frame());
    WebFrameLoadType loadType = ignoreCache ?
        WebFrameLoadType::ReloadFromOrigin : WebFrameLoadType::Reload;
    WebURLRequest request = requestForReload(loadType, overrideUrl);
    if (request.isNull())
        return;
    load(request, loadType, WebHistoryItem(), WebHistoryDifferentDocumentLoad);
}

void WebLocalFrameImpl::reloadImage(const WebNode& webNode)
{
    const Node* node = webNode.constUnwrap<Node>();
    if (isHTMLImageElement(*node)) {
        const HTMLImageElement& imageElement = toHTMLImageElement(*node);
        imageElement.forceReload();
    }
}

void WebLocalFrameImpl::loadRequest(const WebURLRequest& request)
{
    // TODO(clamy): Remove this function once RenderFrame calls load for all
    // requests.
    load(request, WebFrameLoadType::Standard, WebHistoryItem(), WebHistoryDifferentDocumentLoad);
}

void WebLocalFrameImpl::loadHistoryItem(const WebHistoryItem& item, WebHistoryLoadType loadType,
    WebURLRequest::CachePolicy cachePolicy)
{
    // TODO(clamy): Remove this function once RenderFrame calls load for all
    // requests.
    WebURLRequest request = requestFromHistoryItem(item, cachePolicy);
    load(request, WebFrameLoadType::BackForward, item, loadType);
}

void WebLocalFrameImpl::loadData(const WebData& data, const WebString& mimeType, const WebString& textEncoding, const WebURL& baseURL, const WebURL& unreachableURL, bool replace)
{
    ASSERT(frame());

    // If we are loading substitute data to replace an existing load, then
    // inherit all of the properties of that original request. This way,
    // reload will re-attempt the original request. It is essential that
    // we only do this when there is an unreachableURL since a non-empty
    // unreachableURL informs FrameLoader::reload to load unreachableURL
    // instead of the currently loaded URL.
    ResourceRequest request;
    if (replace && !unreachableURL.isEmpty() && frame()->loader().provisionalDocumentLoader())
        request = frame()->loader().provisionalDocumentLoader()->originalRequest();
    request.setURL(baseURL);
    request.setCheckForBrowserSideNavigation(false);

    FrameLoadRequest frameRequest(0, request, SubstituteData(data, mimeType, textEncoding, unreachableURL));
    ASSERT(frameRequest.substituteData().isValid());
    frameRequest.setLockBackForwardList(replace);
    frame()->loader().load(frameRequest);
}

void WebLocalFrameImpl::loadHTMLString(const WebData& data, const WebURL& baseURL, const WebURL& unreachableURL, bool replace)
{
    ASSERT(frame());
    loadData(data, WebString::fromUTF8("text/html"), WebString::fromUTF8("UTF-8"), baseURL, unreachableURL, replace);
}

void WebLocalFrameImpl::stopLoading()
{
    if (!frame())
        return;
    // FIXME: Figure out what we should really do here. It seems like a bug
    // that FrameLoader::stopLoading doesn't call stopAllLoaders.
    frame()->loader().stopAllLoaders();
}

WebDataSource* WebLocalFrameImpl::provisionalDataSource() const
{
    ASSERT(frame());

    // We regard the policy document loader as still provisional.
    DocumentLoader* documentLoader = frame()->loader().provisionalDocumentLoader();
    if (!documentLoader)
        documentLoader = frame()->loader().policyDocumentLoader();

    return DataSourceForDocLoader(documentLoader);
}

WebDataSource* WebLocalFrameImpl::dataSource() const
{
    ASSERT(frame());
    return DataSourceForDocLoader(frame()->loader().documentLoader());
}

void WebLocalFrameImpl::enableViewSourceMode(bool enable)
{
    if (frame())
        frame()->setInViewSourceMode(enable);
}

bool WebLocalFrameImpl::isViewSourceModeEnabled() const
{
    if (!frame())
        return false;
    return frame()->inViewSourceMode();
}

void WebLocalFrameImpl::setReferrerForRequest(WebURLRequest& request, const WebURL& referrerURL)
{
    String referrer = referrerURL.isEmpty() ? frame()->document()->outgoingReferrer() : String(referrerURL.spec().utf16());
    request.toMutableResourceRequest().setHTTPReferrer(SecurityPolicy::generateReferrer(frame()->document()->referrerPolicy(), request.url(), referrer));
}

void WebLocalFrameImpl::dispatchWillSendRequest(WebURLRequest& request)
{
    ResourceResponse response;
    frame()->loader().client()->dispatchWillSendRequest(0, 0, request.toMutableResourceRequest(), response);
}

WebURLLoader* WebLocalFrameImpl::createAssociatedURLLoader(const WebURLLoaderOptions& options)
{
    return new AssociatedURLLoader(this, options);
}

unsigned WebLocalFrameImpl::unloadListenerCount() const
{
    return frame()->localDOMWindow()->pendingUnloadEventListeners();
}

void WebLocalFrameImpl::replaceSelection(const WebString& text)
{
    bool selectReplacement = false;
    bool smartReplace = true;
    frame()->editor().replaceSelectionWithText(text, selectReplacement, smartReplace);
}

void WebLocalFrameImpl::insertText(const WebString& text)
{
    if (frame()->inputMethodController().hasComposition())
        frame()->inputMethodController().confirmComposition(text);
    else
        frame()->editor().insertText(text, 0);
}

void WebLocalFrameImpl::setMarkedText(const WebString& text, unsigned location, unsigned length)
{
    Vector<CompositionUnderline> decorations;
    frame()->inputMethodController().setComposition(text, decorations, location, length);
}

void WebLocalFrameImpl::unmarkText()
{
    frame()->inputMethodController().cancelComposition();
}

bool WebLocalFrameImpl::hasMarkedText() const
{
    return frame()->inputMethodController().hasComposition();
}

WebRange WebLocalFrameImpl::markedRange() const
{
    return frame()->inputMethodController().compositionRange();
}

bool WebLocalFrameImpl::firstRectForCharacterRange(unsigned location, unsigned length, WebRect& rectInViewport) const
{
    if ((location + length < location) && (location + length))
        length = 0;

    Element* editable = frame()->selection().rootEditableElementOrDocumentElement();
    if (!editable)
        return false;
    RefPtrWillBeRawPtr<Range> range = PlainTextRange(location, location + length).createRange(*editable);
    if (!range)
        return false;
    IntRect intRect = frame()->editor().firstRectForRange(range.get());
    rectInViewport = WebRect(intRect);
    rectInViewport = frame()->view()->contentsToViewport(rectInViewport);
    return true;
}

size_t WebLocalFrameImpl::characterIndexForPoint(const WebPoint& pointInViewport) const
{
    if (!frame())
        return kNotFound;

    IntPoint point = frame()->view()->viewportToContents(pointInViewport);
    HitTestResult result = frame()->eventHandler().hitTestResultAtPoint(point, HitTestRequest::ReadOnly | HitTestRequest::Active);
    RefPtrWillBeRawPtr<Range> range = frame()->rangeForPoint(result.roundedPointInInnerNodeFrame());
    if (!range)
        return kNotFound;
    Element* editable = frame()->selection().rootEditableElementOrDocumentElement();
    ASSERT(editable);
    return PlainTextRange::create(*editable, *range.get()).start();
}

bool WebLocalFrameImpl::executeCommand(const WebString& name, const WebNode& node)
{
    ASSERT(frame());

    if (name.length() <= 2)
        return false;

    // Since we don't have NSControl, we will convert the format of command
    // string and call the function on Editor directly.
    String command = name;

    // Make sure the first letter is upper case.
    command.replace(0, 1, command.substring(0, 1).upper());

    // Remove the trailing ':' if existing.
    if (command[command.length() - 1] == UChar(':'))
        command = command.substring(0, command.length() - 1);

    WebPluginContainerImpl* pluginContainer = pluginContainerFromNode(frame(), node);
    if (pluginContainer && pluginContainer->executeEditCommand(name))
        return true;

    return frame()->editor().executeCommand(command);
}

bool WebLocalFrameImpl::executeCommand(const WebString& name, const WebString& value, const WebNode& node)
{
    ASSERT(frame());

    WebPluginContainerImpl* pluginContainer = pluginContainerFromNode(frame(), node);
    if (pluginContainer && pluginContainer->executeEditCommand(name, value))
        return true;

    return frame()->editor().executeCommand(name, value);
}

bool WebLocalFrameImpl::isCommandEnabled(const WebString& name) const
{
    ASSERT(frame());
    return frame()->editor().command(name).isEnabled();
}

void WebLocalFrameImpl::enableContinuousSpellChecking(bool enable)
{
    if (enable == isContinuousSpellCheckingEnabled())
        return;
    frame()->spellChecker().toggleContinuousSpellChecking();
}

bool WebLocalFrameImpl::isContinuousSpellCheckingEnabled() const
{
    return frame()->spellChecker().isContinuousSpellCheckingEnabled();
}

void WebLocalFrameImpl::requestTextChecking(const WebElement& webElement)
{
    if (webElement.isNull())
        return;
    frame()->spellChecker().requestTextChecking(*webElement.constUnwrap<Element>());
}

void WebLocalFrameImpl::replaceMisspelledRange(const WebString& text)
{
    // If this caret selection has two or more markers, this function replace the range covered by the first marker with the specified word as Microsoft Word does.
    if (pluginContainerFromFrame(frame()))
        return;
    frame()->spellChecker().replaceMisspelledRange(text);
}

void WebLocalFrameImpl::removeSpellingMarkers()
{
    frame()->spellChecker().removeSpellingMarkers();
}

bool WebLocalFrameImpl::hasSelection() const
{
    WebPluginContainerImpl* pluginContainer = pluginContainerFromFrame(frame());
    if (pluginContainer)
        return pluginContainer->plugin()->hasSelection();

    // frame()->selection()->isNone() never returns true.
    return frame()->selection().start() != frame()->selection().end();
}

WebRange WebLocalFrameImpl::selectionRange() const
{
    return frame()->selection().toNormalizedRange();
}

WebString WebLocalFrameImpl::selectionAsText() const
{
    WebPluginContainerImpl* pluginContainer = pluginContainerFromFrame(frame());
    if (pluginContainer)
        return pluginContainer->plugin()->selectionAsText();

    RefPtrWillBeRawPtr<Range> range = frame()->selection().toNormalizedRange();
    if (!range)
        return WebString();

    String text = range->text();
#if OS(WIN)
    replaceNewlinesWithWindowsStyleNewlines(text);
#endif
    replaceNBSPWithSpace(text);
    return text;
}

WebString WebLocalFrameImpl::selectionAsMarkup() const
{
    WebPluginContainerImpl* pluginContainer = pluginContainerFromFrame(frame());
    if (pluginContainer)
        return pluginContainer->plugin()->selectionAsMarkup();

    Position startPosition;
    Position endPosition;
    if (!frame()->selection().selection().toNormalizedPositions(startPosition, endPosition))
        return WebString();

    return createMarkup(startPosition, endPosition, AnnotateForInterchange, ConvertBlocksToInlines::NotConvert, ResolveNonLocalURLs);
}

void WebLocalFrameImpl::selectWordAroundPosition(LocalFrame* frame, VisiblePosition position)
{
    TRACE_EVENT0("blink", "WebLocalFrameImpl::selectWordAroundPosition");
    frame->selection().selectWordAroundPosition(position);
}

bool WebLocalFrameImpl::selectWordAroundCaret()
{
    TRACE_EVENT0("blink", "WebLocalFrameImpl::selectWordAroundCaret");
    FrameSelection& selection = frame()->selection();
    if (selection.isNone() || selection.isRange())
        return false;
    return frame()->selection().selectWordAroundPosition(selection.selection().visibleStart());
}

void WebLocalFrameImpl::selectRange(const WebPoint& baseInViewport, const WebPoint& extentInViewport)
{
    moveRangeSelection(baseInViewport, extentInViewport);
}

void WebLocalFrameImpl::selectRange(const WebRange& webRange)
{
    TRACE_EVENT0("blink", "WebLocalFrameImpl::selectRange");
    if (RefPtrWillBeRawPtr<Range> range = static_cast<PassRefPtrWillBeRawPtr<Range>>(webRange))
        frame()->selection().setSelectedRange(range.get(), VP_DEFAULT_AFFINITY, FrameSelection::NonDirectional, NotUserTriggered);
}

void WebLocalFrameImpl::moveRangeSelectionExtent(const WebPoint& point)
{
    TRACE_EVENT0("blink", "WebLocalFrameImpl::moveRangeSelectionExtent");
    frame()->selection().moveRangeSelectionExtent(frame()->view()->viewportToContents(point));
}

void WebLocalFrameImpl::moveRangeSelection(const WebPoint& baseInViewport, const WebPoint& extentInViewport, WebFrame::TextGranularity granularity)
{
    TRACE_EVENT0("blink", "WebLocalFrameImpl::moveRangeSelection");
    blink::TextGranularity blinkGranularity = blink::CharacterGranularity;
    if (granularity == WebFrame::WordGranularity)
        blinkGranularity = blink::WordGranularity;
    frame()->selection().moveRangeSelection(
        visiblePositionForViewportPoint(baseInViewport),
        visiblePositionForViewportPoint(extentInViewport),
        blinkGranularity);
}

void WebLocalFrameImpl::moveCaretSelection(const WebPoint& pointInViewport)
{
    TRACE_EVENT0("blink", "WebLocalFrameImpl::moveCaretSelection");
    Element* editable = frame()->selection().rootEditableElement();
    if (!editable)
        return;

    VisiblePosition position = visiblePositionForViewportPoint(pointInViewport);
    frame()->selection().moveTo(position, UserTriggered);
}

bool WebLocalFrameImpl::setEditableSelectionOffsets(int start, int end)
{
    TRACE_EVENT0("blink", "WebLocalFrameImpl::setEditableSelectionOffsets");
    return frame()->inputMethodController().setEditableSelectionOffsets(PlainTextRange(start, end));
}

bool WebLocalFrameImpl::setCompositionFromExistingText(int compositionStart, int compositionEnd, const WebVector<WebCompositionUnderline>& underlines)
{
    TRACE_EVENT0("blink", "WebLocalFrameImpl::setCompositionFromExistingText");
    if (!frame()->editor().canEdit())
        return false;

    InputMethodController& inputMethodController = frame()->inputMethodController();
    inputMethodController.cancelComposition();

    if (compositionStart == compositionEnd)
        return true;

    inputMethodController.setCompositionFromExistingText(CompositionUnderlineVectorBuilder(underlines), compositionStart, compositionEnd);

    return true;
}

void WebLocalFrameImpl::extendSelectionAndDelete(int before, int after)
{
    TRACE_EVENT0("blink", "WebLocalFrameImpl::extendSelectionAndDelete");
    if (WebPlugin* plugin = focusedPluginIfInputMethodSupported()) {
        plugin->extendSelectionAndDelete(before, after);
        return;
    }
    frame()->inputMethodController().extendSelectionAndDelete(before, after);
}

void WebLocalFrameImpl::setCaretVisible(bool visible)
{
    frame()->selection().setCaretVisible(visible);
}

VisiblePosition WebLocalFrameImpl::visiblePositionForViewportPoint(const WebPoint& pointInViewport)
{
    return visiblePositionForContentsPoint(frame()->view()->viewportToContents(pointInViewport), frame());
}

WebPlugin* WebLocalFrameImpl::focusedPluginIfInputMethodSupported()
{
    WebPluginContainerImpl* container = WebLocalFrameImpl::pluginContainerFromNode(frame(), WebNode(frame()->document()->focusedElement()));
    if (container && container->supportsInputMethod())
        return container->plugin();
    return 0;
}

int WebLocalFrameImpl::printBegin(const WebPrintParams& printParams, const WebNode& constrainToNode)
{
    ASSERT(!frame()->document()->isFrameSet());
    WebPluginContainerImpl* pluginContainer = nullptr;
    if (constrainToNode.isNull()) {
        // If this is a plugin document, check if the plugin supports its own
        // printing. If it does, we will delegate all printing to that.
        pluginContainer = pluginContainerFromFrame(frame());
    } else {
        // We only support printing plugin nodes for now.
        pluginContainer = toWebPluginContainerImpl(constrainToNode.pluginContainer());
    }

    if (pluginContainer && pluginContainer->supportsPaginatedPrint())
        m_printContext = adoptPtrWillBeNoop(new ChromePluginPrintContext(frame(), pluginContainer, printParams));
    else
        m_printContext = adoptPtrWillBeNoop(new ChromePrintContext(frame()));

    FloatRect rect(0, 0, static_cast<float>(printParams.printContentArea.width), static_cast<float>(printParams.printContentArea.height));
    m_printContext->begin(rect.width(), rect.height());
    float pageHeight;
    // We ignore the overlays calculation for now since they are generated in the
    // browser. pageHeight is actually an output parameter.
    m_printContext->computePageRects(rect, 0, 0, 1.0, pageHeight);

    return static_cast<int>(m_printContext->pageCount());
}

float WebLocalFrameImpl::getPrintPageShrink(int page)
{
    ASSERT(m_printContext && page >= 0);
    return m_printContext->getPageShrink(page);
}

float WebLocalFrameImpl::printPage(int page, WebCanvas* canvas)
{
#if ENABLE(PRINTING)

    ASSERT(m_printContext && page >= 0 && frame() && frame()->document());

    return m_printContext->spoolSinglePage(canvas, page);
#else
    return 0;
#endif
}

void WebLocalFrameImpl::printEnd()
{
    ASSERT(m_printContext);
    m_printContext->end();
    m_printContext.clear();
}

bool WebLocalFrameImpl::isPrintScalingDisabledForPlugin(const WebNode& node)
{
    WebPluginContainerImpl* pluginContainer =  node.isNull() ? pluginContainerFromFrame(frame()) : toWebPluginContainerImpl(node.pluginContainer());

    if (!pluginContainer || !pluginContainer->supportsPaginatedPrint())
        return false;

    return pluginContainer->isPrintScalingDisabled();
}

bool WebLocalFrameImpl::getPrintPresetOptionsForPlugin(const WebNode& node, WebPrintPresetOptions* presetOptions)
{
    WebPluginContainerImpl* pluginContainer = node.isNull() ? pluginContainerFromFrame(frame()) : toWebPluginContainerImpl(node.pluginContainer());

    if (!pluginContainer || !pluginContainer->supportsPaginatedPrint())
        return false;

    return pluginContainer->getPrintPresetOptionsFromDocument(presetOptions);
}

bool WebLocalFrameImpl::hasCustomPageSizeStyle(int pageIndex)
{
    return frame()->document()->styleForPage(pageIndex)->pageSizeType() != PAGE_SIZE_AUTO;
}

bool WebLocalFrameImpl::isPageBoxVisible(int pageIndex)
{
    return frame()->document()->isPageBoxVisible(pageIndex);
}

void WebLocalFrameImpl::pageSizeAndMarginsInPixels(int pageIndex, WebSize& pageSize, int& marginTop, int& marginRight, int& marginBottom, int& marginLeft)
{
    IntSize size = pageSize;
    frame()->document()->pageSizeAndMarginsInPixels(pageIndex, size, marginTop, marginRight, marginBottom, marginLeft);
    pageSize = size;
}

WebString WebLocalFrameImpl::pageProperty(const WebString& propertyName, int pageIndex)
{
    ASSERT(m_printContext);
    return m_printContext->pageProperty(frame(), propertyName.utf8().data(), pageIndex);
}

bool WebLocalFrameImpl::find(int identifier, const WebString& searchText, const WebFindOptions& options, bool wrapWithinFrame, WebRect* selectionRect)
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    return ensureTextFinder().find(identifier, searchText, options, wrapWithinFrame, selectionRect);
#endif // MINIBLINK_NOT_IMPLEMENTED
    return false;
}

void WebLocalFrameImpl::stopFinding(bool clearSelection)
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    if (m_textFinder) {
        if (!clearSelection)
            setFindEndstateFocusAndSelection();
        m_textFinder->stopFindingAndClearSelection();
    }
#endif // MINIBLINK_NOT_IMPLEMENTED
}

void WebLocalFrameImpl::scopeStringMatches(int identifier, const WebString& searchText, const WebFindOptions& options, bool reset)
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    ensureTextFinder().scopeStringMatches(identifier, searchText, options, reset);
#endif // MINIBLINK_NOT_IMPLEMENTED
}

void WebLocalFrameImpl::cancelPendingScopingEffort()
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    if (m_textFinder)
        m_textFinder->cancelPendingScopingEffort();
#endif // MINIBLINK_NOT_IMPLEMENTED
}

void WebLocalFrameImpl::increaseMatchCount(int count, int identifier)
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    // This function should only be called on the mainframe.
    ASSERT(!parent());
    ensureTextFinder().increaseMatchCount(identifier, count);
#endif // MINIBLINK_NOT_IMPLEMENTED
}

void WebLocalFrameImpl::resetMatchCount()
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    ensureTextFinder().resetMatchCount();
#endif // MINIBLINK_NOT_IMPLEMENTED
}

void WebLocalFrameImpl::dispatchMessageEventWithOriginCheck(const WebSecurityOrigin& intendedTargetOrigin, const WebDOMEvent& event)
{
    ASSERT(!event.isNull());
    frame()->localDOMWindow()->dispatchMessageEventWithOriginCheck(intendedTargetOrigin.get(), event, nullptr);
}

int WebLocalFrameImpl::findMatchMarkersVersion() const
{
    ASSERT(!parent());
#ifdef MINIBLINK_NOT_IMPLEMENTED
    if (m_textFinder)
        return m_textFinder->findMatchMarkersVersion();
#endif // MINIBLINK_NOT_IMPLEMENTED
    return 0;
}

int WebLocalFrameImpl::selectNearestFindMatch(const WebFloatPoint& point, WebRect* selectionRect)
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    ASSERT(!parent());
    return ensureTextFinder().selectNearestFindMatch(point, selectionRect);
#endif // MINIBLINK_NOT_IMPLEMENTED
    return -1;
}

WebFloatRect WebLocalFrameImpl::activeFindMatchRect()
{
    ASSERT(!parent());
#ifdef MINIBLINK_NOT_IMPLEMENTED
    if (m_textFinder)
        return m_textFinder->activeFindMatchRect();
#endif // MINIBLINK_NOT_IMPLEMENTED
    return WebFloatRect();
}

void WebLocalFrameImpl::findMatchRects(WebVector<WebFloatRect>& outputRects)
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    ASSERT(!parent());
    ensureTextFinder().findMatchRects(outputRects);
#endif // MINIBLINK_NOT_IMPLEMENTED
}

void WebLocalFrameImpl::setTickmarks(const WebVector<WebRect>& tickmarks)
{
    if (frameView()) {
        Vector<IntRect> tickmarksConverted(tickmarks.size());
        for (size_t i = 0; i < tickmarks.size(); ++i)
            tickmarksConverted[i] = tickmarks[i];
        frameView()->setTickmarks(tickmarksConverted);
    }
}

WebString WebLocalFrameImpl::contentAsText(size_t maxChars) const
{
    if (!frame())
        return WebString();
    StringBuilder text;
    frameContentAsPlainText(maxChars, frame(), text);
    return text.toString();
}

WebString WebLocalFrameImpl::contentAsMarkup() const
{
    if (!frame())
        return WebString();
    return createMarkup(frame()->document());
}

WebString WebLocalFrameImpl::layoutTreeAsText(LayoutAsTextControls toShow) const
{
    LayoutAsTextBehavior behavior = LayoutAsTextShowAllLayers;

    if (toShow & LayoutAsTextWithLineTrees)
        behavior |= LayoutAsTextShowLineTrees;

    if (toShow & LayoutAsTextDebug)
        behavior |= LayoutAsTextShowCompositedLayers | LayoutAsTextShowAddresses | LayoutAsTextShowIDAndClass | LayoutAsTextShowLayerNesting;

    if (toShow & LayoutAsTextPrinting)
        behavior |= LayoutAsTextPrintingMode;

    return externalRepresentation(frame(), behavior);
}

void WebLocalFrameImpl::registerTestInterface(const WebString& name, WebTestInterfaceFactory* factory)
{
    m_testInterfaces.set(name, adoptPtr(factory));
}

v8::Local<v8::Value> WebLocalFrameImpl::createTestInterface(const AtomicString& name)
{
    if (WebTestInterfaceFactory* factory = m_testInterfaces.get(name)) {
        ScriptState* scriptState = ScriptState::forMainWorld(frame());
        ASSERT(scriptState->contextIsValid());
        v8::EscapableHandleScope handleScope(scriptState->isolate());
        ScriptState::Scope scope(scriptState);
        return handleScope.Escape(factory->createInstance(scriptState->context()));
    }
    return v8::Local<v8::Value>();
}

WebString WebLocalFrameImpl::markerTextForListItem(const WebElement& webElement) const
{
    return blink::markerTextForListItem(const_cast<Element*>(webElement.constUnwrap<Element>()));
}

void WebLocalFrameImpl::printPagesWithBoundaries(WebCanvas* canvas, const WebSize& pageSizeInPixels)
{
    ASSERT(m_printContext);

    m_printContext->spoolAllPagesWithBoundaries(canvas, FloatSize(pageSizeInPixels.width, pageSizeInPixels.height));
}

WebRect WebLocalFrameImpl::selectionBoundsRect() const
{
    return hasSelection() ? WebRect(IntRect(frame()->selection().bounds())) : WebRect();
}

bool WebLocalFrameImpl::selectionStartHasSpellingMarkerFor(int from, int length) const
{
    if (!frame())
        return false;
    return frame()->spellChecker().selectionStartHasSpellingMarkerFor(from, length);
}

WebString WebLocalFrameImpl::layerTreeAsText(bool showDebugInfo) const
{
    if (!frame())
        return WebString();

    return WebString(frame()->layerTreeAsText(showDebugInfo ? LayerTreeIncludesDebugInfo : LayerTreeNormal));
}

// WebLocalFrameImpl public ---------------------------------------------------------

WebLocalFrame* WebLocalFrame::create(WebTreeScopeType scope, WebFrameClient* client)
{
    return WebLocalFrameImpl::create(scope, client);
}

WebLocalFrameImpl* WebLocalFrameImpl::create(WebTreeScopeType scope, WebFrameClient* client)
{
    WebLocalFrameImpl* frame = new WebLocalFrameImpl(scope, client);
#if ENABLE(OILPAN)
    return frame;
#else
    return adoptRef(frame).leakRef();
#endif
}

#ifndef NDEBUG
DEFINE_DEBUG_ONLY_GLOBAL(WTF::RefCountedLeakCounter, webLocalFrameImplCounter, ("WebLocalFrameImplCounter"));
#endif

WebLocalFrameImpl::WebLocalFrameImpl(WebTreeScopeType scope, WebFrameClient* client)
    : WebLocalFrame(scope)
    , m_frameLoaderClientImpl(this)
    , m_frameWidget(0)
    , m_client(client)
    , m_autofillClient(0)
    , m_contentSettingsClient(0)
    , m_inputEventsScaleFactorForEmulation(1)
#ifdef MINIBLINK_NOT_IMPLEMENTED
    , m_userMediaClientImpl(this)
    , m_geolocationClientProxy(GeolocationClientProxy::create(client ? client->geolocationClient() : 0))
#endif // MINIBLINK_NOT_IMPLEMENTED
    , m_webDevToolsFrontend(0)
#if ENABLE(OILPAN)
    , m_selfKeepAlive(this)
#endif
{
    Platform::current()->incrementStatsCounter(webFrameActiveCount);
    frameCount++;
#ifndef NDEBUG
    webLocalFrameImplCounter.increment();
#endif
}

WebLocalFrameImpl::~WebLocalFrameImpl()
{
    Platform::current()->decrementStatsCounter(webFrameActiveCount);
    frameCount--;
#ifndef NDEBUG
    webLocalFrameImplCounter.decrement();
#endif

#if !ENABLE(OILPAN)
    cancelPendingScopingEffort();
#endif
}

#if ENABLE(OILPAN)
DEFINE_TRACE(WebLocalFrameImpl)
{
    visitor->trace(m_frame);
    visitor->trace(m_devToolsAgent);
    visitor->trace(m_inspectorOverlay);
    visitor->trace(m_printContext);
#ifdef MINIBLINK_NOT_IMPLEMENTED
    visitor->trace(m_textFinder);
    visitor->trace(m_geolocationClientProxy);
#endif // MINIBLINK_NOT_IMPLEMENTED
    visitor->template registerWeakMembers<WebFrame, &WebFrame::clearWeakFrames>(this);
    WebFrame::traceFrames(visitor, this);
}
#endif

void WebLocalFrameImpl::setCoreFrame(PassRefPtrWillBeRawPtr<LocalFrame> frame)
{
    m_frame = frame;

#ifdef MINIBLINK_NOT_IMPLEMENTED
    // FIXME: we shouldn't add overhead to every frame by registering these objects when they're not used.
    if (m_frame) {
        if (m_client)
            providePushControllerTo(*m_frame, m_client->pushClient());

        provideNotificationPermissionClientTo(*m_frame, NotificationPermissionClientImpl::create());
        provideUserMediaTo(*m_frame, &m_userMediaClientImpl);
        provideGeolocationTo(*m_frame, m_geolocationClientProxy.get());
        m_geolocationClientProxy->setController(GeolocationController::from(m_frame.get()));
        provideMIDITo(*m_frame, MIDIClientProxy::create(m_client ? m_client->webMIDIClient() : nullptr));
        provideLocalFileSystemTo(*m_frame, LocalFileSystemClient::create());
        provideNavigatorContentUtilsTo(*m_frame, NavigatorContentUtilsClientImpl::create(this));

        if (RuntimeEnabledFeatures::screenOrientationEnabled())
            ScreenOrientationController::provideTo(*m_frame, m_client ? m_client->webScreenOrientationClient() : nullptr);
        if (RuntimeEnabledFeatures::presentationEnabled())
            PresentationController::provideTo(*m_frame, m_client ? m_client->presentationClient() : nullptr);
        if (RuntimeEnabledFeatures::permissionsEnabled())
            PermissionController::provideTo(*m_frame, m_client ? m_client->permissionClient() : nullptr);
        if (RuntimeEnabledFeatures::webVREnabled())
            VRController::provideTo(*m_frame, m_client ? m_client->webVRClient() : nullptr);
    }
#endif // MINIBLINK_NOT_IMPLEMENTED
}

PassRefPtrWillBeRawPtr<LocalFrame> WebLocalFrameImpl::initializeCoreFrame(FrameHost* host, FrameOwner* owner, const AtomicString& name, const AtomicString& fallbackName)
{
    RefPtrWillBeRawPtr<LocalFrame> frame = LocalFrame::create(&m_frameLoaderClientImpl, host, owner);
    setCoreFrame(frame);
    frame->tree().setName(name, fallbackName);
    // We must call init() after m_frame is assigned because it is referenced
    // during init(). Note that this may dispatch JS events; the frame may be
    // detached after init() returns.
    frame->init();
    return frame;
}

PassRefPtrWillBeRawPtr<LocalFrame> WebLocalFrameImpl::createChildFrame(const FrameLoadRequest& request,
    const AtomicString& name, HTMLFrameOwnerElement* ownerElement)
{
    ASSERT(m_client);
    WebTreeScopeType scope = frame()->document() == ownerElement->treeScope()
        ? WebTreeScopeType::Document
        : WebTreeScopeType::Shadow;
    WebLocalFrameImpl* webframeChild = toWebLocalFrameImpl(m_client->createChildFrame(this, scope, name, static_cast<WebSandboxFlags>(ownerElement->sandboxFlags())));
    if (!webframeChild)
        return nullptr;

    // FIXME: Using subResourceAttributeName as fallback is not a perfect
    // solution. subResourceAttributeName returns just one attribute name. The
    // element might not have the attribute, and there might be other attributes
    // which can identify the element.
    RefPtrWillBeRawPtr<LocalFrame> child = webframeChild->initializeCoreFrame(frame()->host(), ownerElement, name, ownerElement->getAttribute(ownerElement->subResourceAttributeName()));
    // Initializing the core frame may cause the new child to be detached, since
    // it may dispatch a load event in the parent.
    if (!child->tree().parent())
        return nullptr;

    // If we're moving in the back/forward list, we might want to replace the content
    // of this child frame with whatever was there at that point.
    RefPtrWillBeRawPtr<HistoryItem> childItem = nullptr;
    if (isBackForwardLoadType(frame()->loader().loadType()) && !frame()->document()->loadEventFinished())
        childItem = PassRefPtrWillBeRawPtr<HistoryItem>(webframeChild->client()->historyItemForNewChildFrame(webframeChild));

    FrameLoadRequest newRequest = request;
    FrameLoadType loadType = FrameLoadTypeStandard;
    if (childItem) {
        newRequest = FrameLoadRequest(request.originDocument(),
            FrameLoader::resourceRequestFromHistoryItem(childItem.get(), UseProtocolCachePolicy));
        loadType = FrameLoadTypeInitialHistoryLoad;
    }
    child->loader().load(newRequest, loadType, childItem.get());

    // Note a synchronous navigation (about:blank) would have already processed
    // onload, so it is possible for the child frame to have already been
    // detached by script in the page.
    if (!child->tree().parent())
        return nullptr;
    return child;
}

void WebLocalFrameImpl::didChangeContentsSize(const IntSize& size)
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    // This is only possible on the main frame.
    if (m_textFinder && m_textFinder->totalMatchCount() > 0) {
        ASSERT(!parent());
        m_textFinder->increaseMarkerVersion();
    }
#endif // MINIBLINK_NOT_IMPLEMENTED
}

void WebLocalFrameImpl::createFrameView()
{
    TRACE_EVENT0("blink", "WebLocalFrameImpl::createFrameView");

    ASSERT(frame()); // If frame() doesn't exist, we probably didn't init properly.

    WebViewImpl* webView = viewImpl();
    bool isLocalRoot = frame()->isLocalRoot();
    if (isLocalRoot)
        webView->suppressInvalidations(true);

    IntSize initialSize = frameWidget() ? (IntSize)frameWidget()->size() : webView->mainFrameSize();

    frame()->createView(initialSize, webView->baseBackgroundColor(), webView->isTransparent());
    if (webView->shouldAutoResize() && isLocalRoot)
        frame()->view()->enableAutoSizeMode(webView->minAutoSize(), webView->maxAutoSize());

    frame()->view()->setInputEventsTransformForEmulation(m_inputEventsOffsetForEmulation, m_inputEventsScaleFactorForEmulation);
    frame()->view()->setDisplayMode(webView->displayMode());
    frame()->view()->setTopContentInset(m_topContentInset);

    if (isLocalRoot)
        webView->suppressInvalidations(false);
}

WebLocalFrameImpl* WebLocalFrameImpl::fromFrame(LocalFrame* frame)
{
    if (!frame)
        return 0;
    return fromFrame(*frame);
}

WebLocalFrameImpl* WebLocalFrameImpl::fromFrame(LocalFrame& frame)
{
    FrameLoaderClient* client = frame.loader().client();
    if (!client || !client->isFrameLoaderClientImpl())
        return 0;
    return toFrameLoaderClientImpl(client)->webFrame();
}

WebLocalFrameImpl* WebLocalFrameImpl::fromFrameOwnerElement(Element* element)
{
    // FIXME: Why do we check specifically for <iframe> and <frame> here? Why can't we get the WebLocalFrameImpl from an <object> element, for example.
    if (!isHTMLFrameElementBase(element))
        return 0;
    return fromFrame(toLocalFrame(toHTMLFrameElementBase(element)->contentFrame()));
}

WebViewImpl* WebLocalFrameImpl::viewImpl() const
{
    if (!frame())
        return 0;
    return WebViewImpl::fromPage(frame()->page());
}

WebDataSourceImpl* WebLocalFrameImpl::dataSourceImpl() const
{
    return static_cast<WebDataSourceImpl*>(dataSource());
}

WebDataSourceImpl* WebLocalFrameImpl::provisionalDataSourceImpl() const
{
    return static_cast<WebDataSourceImpl*>(provisionalDataSource());
}

void WebLocalFrameImpl::setFindEndstateFocusAndSelection()
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    WebLocalFrameImpl* mainFrameImpl = viewImpl()->mainFrameImpl();

    if (this != mainFrameImpl->activeMatchFrame())
        return;

    if (Range* activeMatch = m_textFinder->activeMatch()) {
        // If the user has set the selection since the match was found, we
        // don't focus anything.
        VisibleSelection selection(frame()->selection().selection());
        if (!selection.isNone())
            return;

        // Need to clean out style and layout state before querying Element::isFocusable().
        frame()->document()->updateLayoutIgnorePendingStylesheets();

        // Try to find the first focusable node up the chain, which will, for
        // example, focus links if we have found text within the link.
        Node* node = activeMatch->firstNode();
        if (node && node->isInShadowTree()) {
            if (Node* host = node->shadowHost()) {
                if (isHTMLInputElement(*host) || isHTMLTextAreaElement(*host))
                    node = host;
            }
        }
        for (; node; node = node->parentNode()) {
            if (!node->isElementNode())
                continue;
            Element* element = toElement(node);
            if (element->isFocusable()) {
                // Found a focusable parent node. Set the active match as the
                // selection and focus to the focusable node.
                frame()->selection().setSelection(VisibleSelection(activeMatch));
                frame()->document()->setFocusedElement(element);
                return;
            }
        }

        // Iterate over all the nodes in the range until we find a focusable node.
        // This, for example, sets focus to the first link if you search for
        // text and text that is within one or more links.
        node = activeMatch->firstNode();
        for (; node && node != activeMatch->pastLastNode(); node = NodeTraversal::next(*node)) {
            if (!node->isElementNode())
                continue;
            Element* element = toElement(node);
            if (element->isFocusable()) {
                frame()->document()->setFocusedElement(element);
                return;
            }
        }

        // No node related to the active match was focusable, so set the
        // active match as the selection (so that when you end the Find session,
        // you'll have the last thing you found highlighted) and make sure that
        // we have nothing focused (otherwise you might have text selected but
        // a link focused, which is weird).
        frame()->selection().setSelection(VisibleSelection(activeMatch));
        frame()->document()->setFocusedElement(nullptr);

        // Finally clear the active match, for two reasons:
        // We just finished the find 'session' and we don't want future (potentially
        // unrelated) find 'sessions' operations to start at the same place.
        // The WebLocalFrameImpl could get reused and the activeMatch could end up pointing
        // to a document that is no longer valid. Keeping an invalid reference around
        // is just asking for trouble.
        m_textFinder->resetActiveMatch();
    }
#endif // MINIBLINK_NOT_IMPLEMENTED
}

void WebLocalFrameImpl::didFail(const ResourceError& error, bool wasProvisional, HistoryCommitType commitType)
{
    if (!client())
        return;
    WebURLError webError = error;
    WebHistoryCommitType webCommitType = static_cast<WebHistoryCommitType>(commitType);
    if (wasProvisional)
        client()->didFailProvisionalLoad(this, webError, webCommitType);
    else
        client()->didFailLoad(this, webError, webCommitType);
}

void WebLocalFrameImpl::setCanHaveScrollbars(bool canHaveScrollbars)
{
    frame()->view()->setCanHaveScrollbars(canHaveScrollbars);
}

void WebLocalFrameImpl::setInputEventsTransformForEmulation(const IntSize& offset, float contentScaleFactor)
{
    m_inputEventsOffsetForEmulation = offset;
    m_inputEventsScaleFactorForEmulation = contentScaleFactor;
    if (frame()->view())
        frame()->view()->setInputEventsTransformForEmulation(m_inputEventsOffsetForEmulation, m_inputEventsScaleFactorForEmulation);
}

void WebLocalFrameImpl::loadJavaScriptURL(const KURL& url)
{
    // This is copied from ScriptController::executeScriptIfJavaScriptURL.
    // Unfortunately, we cannot just use that method since it is private, and
    // it also doesn't quite behave as we require it to for bookmarklets. The
    // key difference is that we need to suppress loading the string result
    // from evaluating the JS URL if executing the JS URL resulted in a
    // location change. We also allow a JS URL to be loaded even if scripts on
    // the page are otherwise disabled.

    if (!frame()->document() || !frame()->page())
        return;

    RefPtrWillBeRawPtr<Document> ownerDocument(frame()->document());

    // Protect privileged pages against bookmarklets and other javascript manipulations.
    if (SchemeRegistry::shouldTreatURLSchemeAsNotAllowingJavascriptURLs(frame()->document()->url().protocol()))
        return;

    String script = decodeURLEscapeSequences(url.string().substring(strlen("javascript:")));
    UserGestureIndicator gestureIndicator(DefinitelyProcessingNewUserGesture);
    v8::HandleScope handleScope(toIsolate(frame()));
    v8::Local<v8::Value> result = frame()->script().executeScriptInMainWorldAndReturnValue(ScriptSourceCode(script));
    if (result.IsEmpty() || !result->IsString())
        return;
    String scriptResult = toCoreString(v8::Local<v8::String>::Cast(result));
    if (!frame()->navigationScheduler().locationChangePending())
        frame()->loader().replaceDocumentWhileExecutingJavaScriptURL(scriptResult, ownerDocument.get());
}

static void ensureFrameLoaderHasCommitted(FrameLoader& frameLoader)
{
    // Internally, Blink uses CommittedMultipleRealLoads to track whether the
    // next commit should create a new history item or not. Ensure we have
    // reached that state.
    if (frameLoader.stateMachine()->committedMultipleRealLoads())
        return;
    frameLoader.stateMachine()->advanceTo(FrameLoaderStateMachine::CommittedMultipleRealLoads);
}

void WebLocalFrameImpl::initializeToReplaceRemoteFrame(WebRemoteFrame* oldWebFrame, const WebString& name, WebSandboxFlags flags)
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    Frame* oldFrame = toCoreFrame(oldWebFrame);
    // Note: this *always* temporarily sets a frame owner, even for main frames!
    // When a core Frame is created with no owner, it attempts to set itself as
    // the main frame of the Page. However, this is a provisional frame, and may
    // disappear, so Page::m_mainFrame can't be updated just yet.
    OwnPtrWillBeRawPtr<FrameOwner> tempOwner = RemoteBridgeFrameOwner::create(nullptr, SandboxNone);
    m_frame = LocalFrame::create(&m_frameLoaderClientImpl, oldFrame->host(), tempOwner.get());
    m_frame->setOwner(oldFrame->owner());
    if (m_frame->owner() && !m_frame->owner()->isLocal())
        toRemoteBridgeFrameOwner(m_frame->owner())->setSandboxFlags(static_cast<SandboxFlags>(flags));
    m_frame->tree().setName(name);
    setParent(oldWebFrame->parent());
    // We must call init() after m_frame is assigned because it is referenced
    // during init(). Note that this may dispatch JS events; the frame may be
    // detached after init() returns.
    m_frame->init();
#endif // MINIBLINK_NOT_IMPLEMENTED
}

void WebLocalFrameImpl::setAutofillClient(WebAutofillClient* autofillClient)
{
    m_autofillClient = autofillClient;
}

WebAutofillClient* WebLocalFrameImpl::autofillClient()
{
    return m_autofillClient;
}

void WebLocalFrameImpl::setDevToolsAgentClient(WebDevToolsAgentClient* devToolsClient)
{
#ifndef MINIBLINK_NO_DEVTOOLS
    if (devToolsClient) {
        m_devToolsAgent = WebDevToolsAgentImpl::create(this, devToolsClient);
    } else {
        m_devToolsAgent->willBeDestroyed();
        m_devToolsAgent->dispose();
        m_devToolsAgent.clear();
    }
#endif // MINIBLINK_NO_DEVTOOLSLINK_NOT_IMPLEMENTED
}

InspectorOverlay* WebLocalFrameImpl::inspectorOverlay()
{
    if (!m_inspectorOverlay)
        m_inspectorOverlay = InspectorOverlayImpl::createEmpty();
    return m_inspectorOverlay.get();
}

WebDevToolsAgent* WebLocalFrameImpl::devToolsAgent()
{
    return m_devToolsAgent.get();
}

void WebLocalFrameImpl::sendPings(const WebNode& contextNode, const WebURL& destinationURL)
{
    ASSERT(frame());
    Element* anchor = contextNode.constUnwrap<Node>()->enclosingLinkEventParentOrSelf();
    if (isHTMLAnchorElement(anchor))
        toHTMLAnchorElement(anchor)->sendPings(destinationURL);
}

WebURLRequest WebLocalFrameImpl::requestFromHistoryItem(const WebHistoryItem& item,
    WebURLRequest::CachePolicy cachePolicy) const
{
    RefPtrWillBeRawPtr<HistoryItem> historyItem = PassRefPtrWillBeRawPtr<HistoryItem>(item);
    ResourceRequest request = FrameLoader::resourceRequestFromHistoryItem(
        historyItem.get(), static_cast<ResourceRequestCachePolicy>(cachePolicy));
    return WrappedResourceRequest(request);
}

WebURLRequest WebLocalFrameImpl::requestForReload(WebFrameLoadType loadType,
    const WebURL& overrideUrl) const
{
    ASSERT(frame());
    ResourceRequest request = frame()->loader().resourceRequestForReload(
        static_cast<FrameLoadType>(loadType), overrideUrl);
    return WrappedResourceRequest(request);
}

void WebLocalFrameImpl::load(const WebURLRequest& request, WebFrameLoadType webFrameLoadType,
    const WebHistoryItem& item, WebHistoryLoadType webHistoryLoadType)
{
    ASSERT(frame());
    ASSERT(!request.isNull());
    const ResourceRequest& resourceRequest = request.toResourceRequest();

    if (resourceRequest.url().protocolIs("javascript")
        && webFrameLoadType == WebFrameLoadType::Standard) {
        loadJavaScriptURL(resourceRequest.url());
        return;
    }

    FrameLoadRequest frameRequest = FrameLoadRequest(nullptr, resourceRequest);
    RefPtrWillBeRawPtr<HistoryItem> historyItem = PassRefPtrWillBeRawPtr<HistoryItem>(item);
    frame()->loader().load(
        frameRequest, static_cast<FrameLoadType>(webFrameLoadType), historyItem.get(),
        static_cast<HistoryLoadType>(webHistoryLoadType));
}

bool WebLocalFrameImpl::isLoading() const
{
    if (!frame() || !frame()->document())
        return false;
    return frame()->loader().stateMachine()->isDisplayingInitialEmptyDocument() || frame()->loader().provisionalDocumentLoader() || !frame()->document()->loadEventFinished();
}

bool WebLocalFrameImpl::isResourceLoadInProgress() const
{
    if (!frame() || !frame()->document())
        return false;
    return frame()->document()->fetcher()->requestCount();
}

void WebLocalFrameImpl::setCommittedFirstRealLoad()
{
    ASSERT(frame());
    ensureFrameLoaderHasCommitted(frame()->loader());
}

void WebLocalFrameImpl::sendOrientationChangeEvent()
{
    if (!frame())
        return;

#ifdef MINIBLINK_NOT_IMPLEMENTED
    // Screen Orientation API
    if (ScreenOrientationController::from(*frame()))
        ScreenOrientationController::from(*frame())->notifyOrientationChanged();
#endif // MINIBLINK_NOT_IMPLEMENTED

    // Legacy window.orientation API
    if (RuntimeEnabledFeatures::orientationEventEnabled() && frame()->domWindow())
        frame()->localDOMWindow()->sendOrientationChangeEvent();
}

void WebLocalFrameImpl::willShowInstallBannerPrompt(int requestId, const WebVector<WebString>& platforms, WebAppBannerPromptReply* reply)
{
    if (!RuntimeEnabledFeatures::appBannerEnabled() || !frame())
        return;
#ifdef MINIBLINK_NOT_IMPLEMENTED
    AppBannerController::willShowInstallBannerPrompt(requestId, client()->appBannerClient(), frame(), platforms, reply);
#endif // MINIBLINK_NOT_IMPLEMENTED
}

void WebLocalFrameImpl::requestRunTask(WebSuspendableTask* task) const
{
    ASSERT(frame());
    ASSERT(frame()->document());
    frame()->document()->postSuspendableTask(WebSuspendableTaskWrapper::create(adoptPtr(task)));
}

void WebLocalFrameImpl::willBeDetached()
{
    if (m_devToolsAgent)
        m_devToolsAgent->willBeDestroyed();
}

void WebLocalFrameImpl::willDetachParent()
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    // Do not expect string scoping results from any frames that got detached
    // in the middle of the operation.
    if (m_textFinder && m_textFinder->scopingInProgress()) {

        // There is a possibility that the frame being detached was the only
        // pending one. We need to make sure final replies can be sent.
        m_textFinder->flushCurrentScoping();

        m_textFinder->cancelPendingScopingEffort();
    }
#endif // MINIBLINK_NOT_IMPLEMENTED
}

WebLocalFrameImpl* WebLocalFrameImpl::activeMatchFrame() const
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    ASSERT(!parent());

    if (m_textFinder)
        return m_textFinder->activeMatchFrame();
#endif // MINIBLINK_NOT_IMPLEMENTED
    return 0;
}

Range* WebLocalFrameImpl::activeMatch() const
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    if (m_textFinder)
        return m_textFinder->activeMatch();
#endif // MINIBLINK_NOT_IMPLEMENTED
    return 0;
}

#ifdef MINIBLINK_NOT_IMPLEMENTED
TextFinder& WebLocalFrameImpl::ensureTextFinder()
{
    if (!m_textFinder)
        m_textFinder = TextFinder::create(*this);

    return *m_textFinder;
}
#endif // MINIBLINK_NOT_IMPLEMENTED

void WebLocalFrameImpl::setFrameWidget(WebFrameWidgetImpl* frameWidget)
{
    m_frameWidget = frameWidget;
}

WebFrameWidgetImpl* WebLocalFrameImpl::frameWidget() const
{
    return m_frameWidget;
}

WebSandboxFlags WebLocalFrameImpl::effectiveSandboxFlags() const
{
    if (!frame())
        return WebSandboxFlags::None;
    return static_cast<WebSandboxFlags>(frame()->loader().effectiveSandboxFlags());
}

int WebLocalFrameImpl::topContentInset() const {
  if (frame() && frame()->view()) {
    return frame()->view()->topContentInset();
  }
  return 0;
}

void WebLocalFrameImpl::setTopContentInset(int offset) {
  m_topContentInset = offset;
  if (frame() && frame()->view()) {
    frame()->view()->setTopContentInset(m_topContentInset);
    WebViewImpl* webView = viewImpl();
    if (webView)
      webView->layout();
  }
  return;
}

} // namespace blink
