/*
 * Copyright (C) 2009 Google Inc. All rights reserved.
 * Copyright (C) 2014 Opera Software ASA. All rights reserved.
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
#include "web/WebPluginContainerImpl.h"

#include "bindings/core/v8/ScriptController.h"
#include "bindings/core/v8/ScriptSourceCode.h"
#include "bindings/core/v8/V8Element.h"
#include "bindings/core/v8/V8NPObject.h"
#include "core/HTMLNames.h"
#include "core/clipboard/DataObject.h"
#include "core/clipboard/DataTransfer.h"
#include "core/events/GestureEvent.h"
#include "core/events/KeyboardEvent.h"
#include "core/events/MouseEvent.h"
#include "core/events/TouchEvent.h"
#include "core/events/WheelEvent.h"
#include "core/frame/EventHandlerRegistry.h"
#include "core/frame/FrameView.h"
#include "core/frame/LocalFrame.h"
#include "core/html/HTMLFormElement.h"
#include "core/html/HTMLPlugInElement.h"
#include "core/input/EventHandler.h"
#include "core/layout/HitTestResult.h"
#include "core/layout/LayoutBox.h"
#include "core/layout/LayoutPart.h"
#include "core/loader/FrameLoadRequest.h"
#include "core/page/FocusController.h"
#include "core/page/Page.h"
#include "core/page/scrolling/ScrollingCoordinator.h"
#include "core/paint/DeprecatedPaintLayer.h"
#include "core/paint/LayoutObjectDrawingRecorder.h"
#include "modules/plugins/PluginOcclusionSupport.h"
#include "platform/HostWindow.h"
#include "platform/KeyboardCodes.h"
#include "platform/PlatformGestureEvent.h"
#include "platform/UserGestureIndicator.h"
#include "platform/exported/WrappedResourceResponse.h"
#include "platform/graphics/GraphicsContext.h"
#include "platform/graphics/GraphicsLayer.h"
#include "platform/scroll/ScrollAnimator.h"
#include "platform/scroll/ScrollbarTheme.h"
#include "public/platform/Platform.h"
#include "public/platform/WebClipboard.h"
#include "public/platform/WebCompositorSupport.h"
#include "public/platform/WebCursorInfo.h"
#include "public/platform/WebDragData.h"
#include "public/platform/WebExternalTextureLayer.h"
#include "public/platform/WebRect.h"
#include "public/platform/WebString.h"
#include "public/platform/WebURL.h"
#include "public/platform/WebURLError.h"
#include "public/platform/WebURLRequest.h"
#include "public/web/WebElement.h"
#include "public/web/WebInputEvent.h"
#include "public/web/WebPlugin.h"
#include "public/web/WebPrintParams.h"
#include "public/web/WebPrintPresetOptions.h"
#include "public/web/WebViewClient.h"
#include "web/ChromeClientImpl.h"
#include "web/WebDataSourceImpl.h"
#include "web/WebInputEventConversion.h"
#include "web/WebViewImpl.h"
#include "wtf/Assertions.h"

namespace blink {

// Public methods --------------------------------------------------------------

void WebPluginContainerImpl::setFrameRect(const IntRect& frameRect)
{
    Widget::setFrameRect(frameRect);
    reportGeometry();
}

void WebPluginContainerImpl::layoutIfNeeded()
{
    RELEASE_ASSERT(m_webPlugin);
    m_webPlugin->layoutIfNeeded();
}

void WebPluginContainerImpl::paint(GraphicsContext* context, const IntRect& rect)
{
    if (!parent())
        return;

    // Don't paint anything if the plugin doesn't intersect.
    if (!frameRect().intersects(rect))
        return;

    if (LayoutObjectDrawingRecorder::useCachedDrawingIfPossible(*context, *m_element->layoutObject(), DisplayItem::Type::WebPlugin))
        return;

    LayoutObjectDrawingRecorder drawingRecorder(*context, *m_element->layoutObject(), DisplayItem::Type::WebPlugin, rect);
    context->save();

    ASSERT(parent()->isFrameView());
    FrameView* view =  toFrameView(parent());

    // The plugin is positioned in the root frame's coordinates, so it needs to
    // be painted in them too.
    IntPoint origin = view->contentsToRootFrameAero(IntPoint(0, 0));

    context->translate(static_cast<float>(-origin.x()), static_cast<float>(-origin.y()));

    WebCanvas* canvas = context->canvas();

    IntRect windowRect = view->contentsToRootFrame(rect);
    m_webPlugin->paint(canvas, windowRect);

    context->restore();
}

void WebPluginContainerImpl::invalidateRect(const IntRect& rect)
{
    if (!parent())
        return;

    LayoutBox* layoutObject = toLayoutBox(m_element->layoutObject());
    if (!layoutObject)
        return;

    IntRect dirtyRect = rect;
    dirtyRect.move(
        layoutObject->borderLeft() + layoutObject->paddingLeft(),
        layoutObject->borderTop() + layoutObject->paddingTop());

    m_pendingInvalidationRect.unite(dirtyRect);

    layoutObject->setMayNeedPaintInvalidation();
}

void WebPluginContainerImpl::setFocus(bool focused, WebFocusType focusType)
{
    Widget::setFocus(focused, focusType);
    m_webPlugin->updateFocus(focused, focusType);
}

void WebPluginContainerImpl::show()
{
    setSelfVisible(true);
    m_webPlugin->updateVisibility(true);

    Widget::show();
}

void WebPluginContainerImpl::hide()
{
    setSelfVisible(false);
    m_webPlugin->updateVisibility(false);

    Widget::hide();
}

void WebPluginContainerImpl::handleEvent(Event* event)
{
    if (!m_webPlugin->acceptsInputEvents())
        return;

    RefPtrWillBeRawPtr<WebPluginContainerImpl> protector(this);
    // The events we pass are defined at:
    //    http://devedge-temp.mozilla.org/library/manuals/2002/plugin/1.0/structures5.html#1000000
    // Don't take the documentation as truth, however.  There are many cases
    // where mozilla behaves differently than the spec.
    if (event->isMouseEvent())
        handleMouseEvent(toMouseEvent(event));
    else if (event->isWheelEvent())
        handleWheelEvent(toWheelEvent(event));
    else if (event->isKeyboardEvent())
        handleKeyboardEvent(toKeyboardEvent(event));
    else if (event->isTouchEvent())
        handleTouchEvent(toTouchEvent(event));
    else if (event->isGestureEvent())
        handleGestureEvent(toGestureEvent(event));

    // FIXME: it would be cleaner if Widget::handleEvent returned true/false and
    // HTMLPluginElement called setDefaultHandled or defaultEventHandler.
    if (!event->defaultHandled())
        m_element->Node::defaultEventHandler(event);
}

void WebPluginContainerImpl::frameRectsChanged()
{
    Widget::frameRectsChanged();
    reportGeometry();
}

void WebPluginContainerImpl::widgetPositionsUpdated()
{
    Widget::widgetPositionsUpdated();
    reportGeometry();
}

void WebPluginContainerImpl::eventListenersRemoved()
{
    // We're no longer registered to receive touch events, so don't try to remove
    // the touch event handlers in our destructor.
    m_touchEventRequestType = TouchEventRequestTypeNone;
}

void WebPluginContainerImpl::setParentVisible(bool parentVisible)
{
    // We override this function to make sure that geometry updates are sent
    // over to the plugin. For e.g. when a plugin is instantiated it does not
    // have a valid parent. As a result the first geometry update from webkit
    // is ignored. This function is called when the plugin eventually gets a
    // parent.

    if (isParentVisible() == parentVisible)
        return;  // No change.

    Widget::setParentVisible(parentVisible);
    if (!isSelfVisible())
        return;  // This widget has explicitely been marked as not visible.

    m_webPlugin->updateVisibility(isVisible());
}

void WebPluginContainerImpl::setParent(Widget* widget)
{
    // We override this function so that if the plugin is windowed, we can call
    // NPP_SetWindow at the first possible moment.  This ensures that
    // NPP_SetWindow is called before the manual load data is sent to a plugin.
    // If this order is reversed, Flash won't load videos.

    Widget::setParent(widget);
    if (widget)
        reportGeometry();
    else if (m_webPlugin)
        m_webPlugin->containerDidDetachFromParent();
}

void WebPluginContainerImpl::setPlugin(WebPlugin* plugin)
{
    if (plugin != m_webPlugin) {
        m_element->resetInstance();
        m_webPlugin = plugin;
    }
}

float WebPluginContainerImpl::deviceScaleFactor()
{
    Page* page = m_element->document().page();
    if (!page)
        return 1.0;
    return page->deviceScaleFactor();
}

float WebPluginContainerImpl::pageScaleFactor()
{
    Page* page = m_element->document().page();
    if (!page)
        return 1.0;
    return page->pageScaleFactor();
}

float WebPluginContainerImpl::pageZoomFactor()
{
    LocalFrame* frame = m_element->document().frame();
    if (!frame)
        return 1.0;
    return frame->pageZoomFactor();
}

void WebPluginContainerImpl::setWebLayer(WebLayer* layer)
{
    if (m_webLayer == layer)
        return;

    if (m_webLayer)
        GraphicsLayer::unregisterContentsLayer(m_webLayer);
    if (layer)
        GraphicsLayer::registerContentsLayer(layer);

    // If either of the layers is null we need to switch between hardware
    // and software compositing.
    bool needsCompositingUpdate = !m_webLayer || !layer;

    m_webLayer = layer;

    if (!needsCompositingUpdate)
        return;

#if ENABLE(OILPAN)
    if (!m_element)
        return;
#endif

    m_element->setNeedsCompositingUpdate();
    // Being composited or not affects the self painting layer bit
    // on the DeprecatedPaintLayer.
    if (LayoutPart* layoutObject = m_element->layoutPart()) {
        ASSERT(layoutObject->hasLayer());
        layoutObject->layer()->updateSelfPaintingLayer();
    }
}

bool WebPluginContainerImpl::supportsPaginatedPrint() const
{
    return m_webPlugin->supportsPaginatedPrint();
}

bool WebPluginContainerImpl::isPrintScalingDisabled() const
{
    return m_webPlugin->isPrintScalingDisabled();
}

bool WebPluginContainerImpl::getPrintPresetOptionsFromDocument(WebPrintPresetOptions* presetOptions) const
{
    return m_webPlugin->getPrintPresetOptionsFromDocument(presetOptions);
}

int WebPluginContainerImpl::printBegin(const WebPrintParams& printParams) const
{
    return m_webPlugin->printBegin(printParams);
}

void WebPluginContainerImpl::printPage(int pageNumber, GraphicsContext* gc, const IntRect& printRect)
{
    if (LayoutObjectDrawingRecorder::useCachedDrawingIfPossible(*gc, *m_element->layoutObject(), DisplayItem::Type::WebPlugin))
        return;

    LayoutObjectDrawingRecorder drawingRecorder(*gc, *m_element->layoutObject(), DisplayItem::Type::WebPlugin, printRect);
    gc->save();
    WebCanvas* canvas = gc->canvas();
    m_webPlugin->printPage(pageNumber, canvas);
    gc->restore();
}

void WebPluginContainerImpl::printEnd()
{
    m_webPlugin->printEnd();
}

void WebPluginContainerImpl::copy()
{
    if (!m_webPlugin->hasSelection())
        return;

    Platform::current()->clipboard()->writeHTML(m_webPlugin->selectionAsMarkup(), WebURL(), m_webPlugin->selectionAsText(), false);
}

bool WebPluginContainerImpl::executeEditCommand(const WebString& name)
{
    if (m_webPlugin->executeEditCommand(name))
        return true;

    if (name != "Copy")
        return false;

    copy();
    return true;
}

bool WebPluginContainerImpl::executeEditCommand(const WebString& name, const WebString& value)
{
    return m_webPlugin->executeEditCommand(name, value);
}

WebElement WebPluginContainerImpl::element()
{
    return WebElement(m_element);
}

void WebPluginContainerImpl::invalidate()
{
    Widget::invalidate();
}

void WebPluginContainerImpl::invalidateRect(const WebRect& rect)
{
    invalidateRect(static_cast<IntRect>(rect));
}

void WebPluginContainerImpl::scrollRect(const WebRect& rect)
{
    invalidateRect(rect);
}

void WebPluginContainerImpl::setNeedsLayout()
{
    if (m_element->layoutObject())
        m_element->layoutObject()->setNeedsLayoutAndFullPaintInvalidation("Plugin needs layout");
}

void WebPluginContainerImpl::reportGeometry()
{
    // We cannot compute geometry without a parent or layoutObject.
    if (!parent() || !m_element || !m_element->layoutObject())
        return;

    IntRect windowRect, clipRect, unobscuredRect;
    Vector<IntRect> cutOutRects;
    calculateGeometry(windowRect, clipRect, unobscuredRect, cutOutRects);

    m_webPlugin->updateGeometry(windowRect, clipRect, unobscuredRect, cutOutRects, isVisible());
}

void WebPluginContainerImpl::allowScriptObjects()
{
}

void WebPluginContainerImpl::clearScriptObjects()
{
    if (!frame())
        return;

    frame()->script().cleanupScriptObjectsForPlugin(this);
}

NPObject* WebPluginContainerImpl::scriptableObjectForElement()
{
    return m_element->getNPObject();
}

v8::Local<v8::Object> WebPluginContainerImpl::v8ObjectForElement()
{
    LocalFrame* frame = m_element->document().frame();
    if (!frame)
        return v8::Local<v8::Object>();

    if (!frame->script().canExecuteScripts(NotAboutToExecuteScript))
        return v8::Local<v8::Object>();

    ScriptState* scriptState = ScriptState::forMainWorld(frame);
    if (!scriptState->contextIsValid())
        return v8::Local<v8::Object>();

    v8::Local<v8::Value> v8value = toV8(m_element.get(), scriptState->context()->Global(), scriptState->isolate());
    if (v8value.IsEmpty())
        return v8::Local<v8::Object>();
    ASSERT(v8value->IsObject());

    return v8::Local<v8::Object>::Cast(v8value);
}

WebString WebPluginContainerImpl::executeScriptURL(const WebURL& url, bool popupsAllowed)
{
    LocalFrame* frame = m_element->document().frame();
    if (!frame)
        return WebString();

    const KURL& kurl = url;
    ASSERT(kurl.protocolIs("javascript"));

    String script = decodeURLEscapeSequences(
        kurl.string().substring(strlen("javascript:")));

    UserGestureIndicator gestureIndicator(popupsAllowed ? DefinitelyProcessingNewUserGesture : PossiblyProcessingUserGesture);
    v8::HandleScope handleScope(toIsolate(frame));
    v8::Local<v8::Value> result = frame->script().executeScriptInMainWorldAndReturnValue(ScriptSourceCode(script));

    // Failure is reported as a null string.
    if (result.IsEmpty() || !result->IsString())
        return WebString();
    return toCoreString(v8::Local<v8::String>::Cast(result));
}

void WebPluginContainerImpl::loadFrameRequest(const WebURLRequest& request, const WebString& target, bool notifyNeeded, void* notifyData)
{
    LocalFrame* frame = m_element->document().frame();
    if (!frame || !frame->loader().documentLoader())
        return;  // FIXME: send a notification in this case?

    if (notifyNeeded) {
        // FIXME: This is a bit of hack to allow us to observe completion of
        // our frame request.  It would be better to evolve FrameLoader to
        // support a completion callback instead.
        OwnPtr<WebPluginLoadObserver> observer = adoptPtr(new WebPluginLoadObserver(this, request.url(), notifyData));
        // FIXME: Calling get here is dangerous! What if observer is freed?
        m_pluginLoadObservers.append(observer.get());
        WebDataSourceImpl::setNextPluginLoadObserver(observer.release());
    }

    FrameLoadRequest frameRequest(frame->document(), request.toResourceRequest(), target);
    frame->loader().load(frameRequest);
}

void WebPluginContainerImpl::zoomLevelChanged(double zoomLevel)
{
    WebViewImpl* view = WebViewImpl::fromPage(m_element->document().frame()->page());
    view->fullFramePluginZoomLevelChanged(zoomLevel);
}

bool WebPluginContainerImpl::isRectTopmost(const WebRect& rect)
{
    // Disallow access to the frame during dispose(), because it is not guaranteed to
    // be valid memory once this object has started disposal. In particular, we might be being
    // disposed because the frame has already be deleted and then something else dropped the
    // last reference to the this object.
    if (m_inDispose || !m_element)
        return false;

    LocalFrame* frame = m_element->document().frame();
    if (!frame)
        return false;

    // hitTestResultAtPoint() takes a padding rectangle.
    // FIXME: We'll be off by 1 when the width or height is even.
    IntRect documentRect(x() + rect.x, y() + rect.y, rect.width, rect.height);
    LayoutPoint center = documentRect.center();
    // Make the rect we're checking (the point surrounded by padding rects) contained inside the requested rect. (Note that -1/2 is 0.)
    LayoutSize padding((documentRect.width() - 1) / 2, (documentRect.height() - 1) / 2);
    HitTestResult result = frame->eventHandler().hitTestResultAtPoint(center, HitTestRequest::ReadOnly | HitTestRequest::Active | HitTestRequest::ListBased, padding);
    const HitTestResult::NodeSet& nodes = result.listBasedTestResult();
    if (nodes.size() != 1)
        return false;
    return nodes.first().get() == m_element;
}

void WebPluginContainerImpl::requestTouchEventType(TouchEventRequestType requestType)
{
    if (m_touchEventRequestType == requestType)
        return;

    if (m_element->document().frameHost()) {
        EventHandlerRegistry& registry = m_element->document().frameHost()->eventHandlerRegistry();
        if (requestType != TouchEventRequestTypeNone && m_touchEventRequestType == TouchEventRequestTypeNone)
            registry.didAddEventHandler(*m_element, EventHandlerRegistry::TouchEvent);
        else if (requestType == TouchEventRequestTypeNone && m_touchEventRequestType != TouchEventRequestTypeNone)
            registry.didRemoveEventHandler(*m_element, EventHandlerRegistry::TouchEvent);
    }
    m_touchEventRequestType = requestType;
}

void WebPluginContainerImpl::setWantsWheelEvents(bool wantsWheelEvents)
{
    if (m_wantsWheelEvents == wantsWheelEvents)
        return;
    m_wantsWheelEvents = wantsWheelEvents;
    if (Page* page = m_element->document().page()) {
        if (ScrollingCoordinator* scrollingCoordinator = page->scrollingCoordinator()) {
            if (parent() && parent()->isFrameView())
                scrollingCoordinator->notifyLayoutUpdated();
        }
    }
}

WebPoint WebPluginContainerImpl::rootFrameToLocalPoint(const WebPoint& pointInRootFrame)
{
    FrameView* view = toFrameView(parent());
    if (!view)
        return pointInRootFrame;
    WebPoint pointInContent = view->rootFrameToContents(pointInRootFrame);
    return roundedIntPoint(m_element->layoutObject()->absoluteToLocal(FloatPoint(pointInContent), UseTransforms));
}

WebPoint WebPluginContainerImpl::localToRootFramePoint(const WebPoint& pointInLocal)
{
    FrameView* view = toFrameView(parent());
    if (!view)
        return pointInLocal;
    IntPoint absolutePoint = roundedIntPoint(m_element->layoutObject()->localToAbsolute(FloatPoint(pointInLocal), UseTransforms));
    return view->contentsToRootFrame(absolutePoint);
}

void WebPluginContainerImpl::didReceiveResponse(const ResourceResponse& response)
{
    // Make sure that the plugin receives window geometry before data, or else
    // plugins misbehave.
    frameRectsChanged();

    WrappedResourceResponse urlResponse(response);
    m_webPlugin->didReceiveResponse(urlResponse);
}

void WebPluginContainerImpl::didReceiveData(const char *data, int dataLength)
{
    m_webPlugin->didReceiveData(data, dataLength);
}

void WebPluginContainerImpl::didFinishLoading()
{
    m_webPlugin->didFinishLoading();
}

void WebPluginContainerImpl::didFailLoading(const ResourceError& error)
{
    m_webPlugin->didFailLoading(error);
}

WebLayer* WebPluginContainerImpl::platformLayer() const
{
    return m_webLayer;
}

v8::Local<v8::Object> WebPluginContainerImpl::scriptableObject(v8::Isolate* isolate)
{
    // The plugin may be destroyed due to re-entrancy when calling
    // v8ScriptableObject below. crbug.com/458776. Hold a reference to the
    // plugin container to prevent this from happening. For Oilpan, 'this'
    // is already stack reachable, so redundant.
    RefPtrWillBeRawPtr<WebPluginContainerImpl> protector(this);

    v8::Local<v8::Object> object = m_webPlugin->v8ScriptableObject(isolate);

    // If the plugin has been destroyed and the reference on the stack is the
    // only one left, then don't return the scriptable object.
#if ENABLE(OILPAN)
    if (!m_webPlugin)
#else
    if (hasOneRef())
#endif
        return v8::Local<v8::Object>();

    if (!object.IsEmpty()) {
        // WebPlugin implementation can't provide the obsolete NPObject at the same time:
        ASSERT(!m_webPlugin->scriptableObject());
        return object;
    }

    NPObject* npObject = m_webPlugin->scriptableObject();
    if (npObject)
        return createV8ObjectForNPObject(isolate, npObject, 0);
    return v8::Local<v8::Object>();
}

bool WebPluginContainerImpl::getFormValue(String& value)
{
    WebString webValue;
    if (m_webPlugin->getFormValue(webValue)) {
        value = webValue;
        return true;
    }
    return false;
}

bool WebPluginContainerImpl::supportsKeyboardFocus() const
{
    return m_webPlugin->supportsKeyboardFocus();
}

bool WebPluginContainerImpl::supportsInputMethod() const
{
    return m_webPlugin->supportsInputMethod();
}

bool WebPluginContainerImpl::canProcessDrag() const
{
    return m_webPlugin->canProcessDrag();
}

bool WebPluginContainerImpl::wantsWheelEvents()
{
    return m_wantsWheelEvents;
}

void WebPluginContainerImpl::willDestroyPluginLoadObserver(WebPluginLoadObserver* observer)
{
    size_t pos = m_pluginLoadObservers.find(observer);
    if (pos == kNotFound)
        return;
    m_pluginLoadObservers.remove(pos);
}

// Private methods -------------------------------------------------------------

WebPluginContainerImpl::WebPluginContainerImpl(HTMLPlugInElement* element, WebPlugin* webPlugin)
    : LocalFrameLifecycleObserver(element->document().frame())
    , m_element(element)
    , m_webPlugin(webPlugin)
    , m_webLayer(nullptr)
    , m_touchEventRequestType(TouchEventRequestTypeNone)
    , m_wantsWheelEvents(false)
    , m_inDispose(false)
#if ENABLE(OILPAN)
    , m_shouldDisposePlugin(false)
#endif
{
}

WebPluginContainerImpl::~WebPluginContainerImpl()
{
#if ENABLE(OILPAN)
    if (m_shouldDisposePlugin)
        dispose();
    // The plugin container must have been disposed of by now.
    ASSERT(!m_webPlugin);
#else
    dispose();
#endif
}

void WebPluginContainerImpl::dispose()
{
    m_inDispose = true;

    if (m_element && m_touchEventRequestType != TouchEventRequestTypeNone && m_element->document().frameHost())
        m_element->document().frameHost()->eventHandlerRegistry().didRemoveEventHandler(*m_element, EventHandlerRegistry::TouchEvent);

    for (size_t i = 0; i < m_pluginLoadObservers.size(); ++i)
        m_pluginLoadObservers[i]->clearPluginContainer();

    m_webPlugin->destroy();
    m_webPlugin = nullptr;

    if (m_webLayer)
        GraphicsLayer::unregisterContentsLayer(m_webLayer);

    m_pluginLoadObservers.clear();
    m_element = nullptr;
}

#if ENABLE(OILPAN)
void WebPluginContainerImpl::shouldDisposePlugin()
{
    // If the LocalFrame is still alive, but the plugin element isn't, the
    // LocalFrame will set m_shouldDisposePlugin via its weak pointer
    // callback. This is a signal that the plugin container
    // must dispose of its plugin when finalizing. The LocalFrame and
    // all objects accessible from it can safely be accessed, but not
    // the plugin element itself.
    ASSERT(!m_shouldDisposePlugin);
    m_shouldDisposePlugin = true;
    m_element = nullptr;
}
#endif

DEFINE_TRACE(WebPluginContainerImpl)
{
    visitor->trace(m_element);
    LocalFrameLifecycleObserver::trace(visitor);
    PluginView::trace(visitor);
}

void WebPluginContainerImpl::handleMouseEvent(MouseEvent* event)
{
    ASSERT(parent()->isFrameView());

    if (event->isDragEvent()) {
        if (m_webPlugin->canProcessDrag())
            handleDragEvent(event);
        return;
    }

    // We cache the parent FrameView here as the plugin widget could be deleted
    // in the call to HandleEvent. See http://b/issue?id=1362948
    FrameView* parentView = toFrameView(parent());

    WebMouseEventBuilder webEvent(this, m_element->layoutObject(), *event);
    if (webEvent.type == WebInputEvent::Undefined)
        return;

    if (event->type() == EventTypeNames::mousedown)
        focusPlugin();

    WebCursorInfo cursorInfo;
    if (m_webPlugin->handleInputEvent(webEvent, cursorInfo))
        event->setDefaultHandled();

    // A windowless plugin can change the cursor in response to a mouse move
    // event.  We need to reflect the changed cursor in the frame view as the
    // mouse is moved in the boundaries of the windowless plugin.
    Page* page = parentView->frame().page();
    if (!page)
        return;
    toChromeClientImpl(page->chromeClient()).setCursorForPlugin(cursorInfo);
}

void WebPluginContainerImpl::handleDragEvent(MouseEvent* event)
{
    ASSERT(event->isDragEvent());

    WebDragStatus dragStatus = WebDragStatusUnknown;
    if (event->type() == EventTypeNames::dragenter)
        dragStatus = WebDragStatusEnter;
    else if (event->type() == EventTypeNames::dragleave)
        dragStatus = WebDragStatusLeave;
    else if (event->type() == EventTypeNames::dragover)
        dragStatus = WebDragStatusOver;
    else if (event->type() == EventTypeNames::drop)
        dragStatus = WebDragStatusDrop;

    if (dragStatus == WebDragStatusUnknown)
        return;

    DataTransfer* dataTransfer = event->dataTransfer();
    WebDragData dragData = dataTransfer->dataObject()->toWebDragData();
    WebDragOperationsMask dragOperationMask = static_cast<WebDragOperationsMask>(dataTransfer->sourceOperation());
    WebPoint dragScreenLocation(event->screenX(), event->screenY());
    WebPoint dragLocation(event->absoluteLocation().x() - location().x(), event->absoluteLocation().y() - location().y());

    m_webPlugin->handleDragStatusUpdate(dragStatus, dragData, dragOperationMask, dragLocation, dragScreenLocation);
}

void WebPluginContainerImpl::handleWheelEvent(WheelEvent* event)
{
    WebMouseWheelEventBuilder webEvent(this, m_element->layoutObject(), *event);
    if (webEvent.type == WebInputEvent::Undefined)
        return;

    WebCursorInfo cursorInfo;
    if (m_webPlugin->handleInputEvent(webEvent, cursorInfo))
        event->setDefaultHandled();
}

void WebPluginContainerImpl::handleKeyboardEvent(KeyboardEvent* event)
{
    WebKeyboardEventBuilder webEvent(*event);
    if (webEvent.type == WebInputEvent::Undefined)
        return;

    if (webEvent.type == WebInputEvent::KeyDown) {
#if OS(MACOSX)
        if (webEvent.modifiers == WebInputEvent::MetaKey
#else
        if (webEvent.modifiers == WebInputEvent::ControlKey
#endif
            && (webEvent.windowsKeyCode == VKEY_C || webEvent.windowsKeyCode == VKEY_INSERT)
            // Only copy if there's a selection, so that we only ever do this
            // for Pepper plugins that support copying.  Windowless NPAPI
            // plugins will get the event as before.
            && m_webPlugin->hasSelection()) {
            copy();
            event->setDefaultHandled();
            return;
        }
    }

    const WebInputEvent* currentInputEvent = WebViewImpl::currentInputEvent();

    // Copy stashed info over, and only copy here in order not to interfere
    // the ctrl-c logic above.
    if (currentInputEvent
        && WebInputEvent::isKeyboardEventType(currentInputEvent->type)) {
        webEvent.modifiers |= currentInputEvent->modifiers &
            (WebInputEvent::CapsLockOn | WebInputEvent::NumLockOn);
    }

    // Give the client a chance to issue edit comamnds.
    WebViewImpl* view = WebViewImpl::fromPage(m_element->document().frame()->page());
    if (m_webPlugin->supportsEditCommands() && view->client())
        view->client()->handleCurrentKeyboardEvent();

    WebCursorInfo cursorInfo;
    if (m_webPlugin->handleInputEvent(webEvent, cursorInfo))
        event->setDefaultHandled();
}

void WebPluginContainerImpl::handleTouchEvent(TouchEvent* event)
{
    switch (m_touchEventRequestType) {
    case TouchEventRequestTypeNone:
        return;
    case TouchEventRequestTypeRaw: {
        WebTouchEventBuilder webEvent(m_element->layoutObject(), *event);
        if (webEvent.type == WebInputEvent::Undefined)
            return;

        if (event->type() == EventTypeNames::touchstart)
            focusPlugin();

        WebCursorInfo cursorInfo;
        if (m_webPlugin->handleInputEvent(webEvent, cursorInfo))
            event->setDefaultHandled();
        // FIXME: Can a plugin change the cursor from a touch-event callback?
        return;
    }
    case TouchEventRequestTypeSynthesizedMouse:
        synthesizeMouseEventIfPossible(event);
        return;
    }
}

void WebPluginContainerImpl::handleGestureEvent(GestureEvent* event)
{
    WebGestureEventBuilder webEvent(m_element->layoutObject(), *event);
    if (webEvent.type == WebInputEvent::Undefined)
        return;
    if (event->type() == EventTypeNames::gesturetapdown)
        focusPlugin();
    WebCursorInfo cursorInfo;
    if (m_webPlugin->handleInputEvent(webEvent, cursorInfo)) {
        event->setDefaultHandled();
        return;
    }

    // FIXME: Can a plugin change the cursor from a touch-event callback?
}

void WebPluginContainerImpl::synthesizeMouseEventIfPossible(TouchEvent* event)
{
    WebMouseEventBuilder webEvent(this, m_element->layoutObject(), *event);
    if (webEvent.type == WebInputEvent::Undefined)
        return;

    WebCursorInfo cursorInfo;
    if (m_webPlugin->handleInputEvent(webEvent, cursorInfo))
        event->setDefaultHandled();
}

void WebPluginContainerImpl::focusPlugin()
{
    LocalFrame& containingFrame = toFrameView(parent())->frame();
    if (Page* currentPage = containingFrame.page())
        currentPage->focusController().setFocusedElement(m_element, &containingFrame);
    else
        containingFrame.document()->setFocusedElement(m_element);
}

void WebPluginContainerImpl::issuePaintInvalidations()
{
    if (m_pendingInvalidationRect.isEmpty())
        return;

    LayoutBox* layoutObject = toLayoutBox(m_element->layoutObject());
    if (!layoutObject)
        return;

    layoutObject->invalidatePaintRectangle(LayoutRect(m_pendingInvalidationRect));
    m_pendingInvalidationRect = IntRect();
}

void WebPluginContainerImpl::calculateGeometry(IntRect& windowRect, IntRect& clipRect, IntRect& unobscuredRect, Vector<IntRect>& cutOutRects)
{
    windowRect = toFrameView(parent())->contentsToRootFrameAero(frameRect());

    // Calculate a clip-rect so that we don't overlap the scrollbars, etc.
    clipRect = convertToContainingWindow(IntRect(0, 0, width(), height()));
    unobscuredRect = clipRect;

    // document().layoutView() can be 0 when we receive messages from the
    // plugins while we are destroying a frame.
    // FIXME: Can we just check m_element->document().isActive() ?
    if (m_element->layoutObject()->document().layoutView()) {
        // Take our element and get the clip rect from the enclosing layer and
        // frame view.
        IntRect elementUnobscuredRect;
        IntRect elementWindowClipRect = m_element->document().view()->clipRectsForFrameOwner(m_element, &elementUnobscuredRect);
        clipRect.intersect(elementWindowClipRect);
        unobscuredRect.intersect(elementUnobscuredRect);
    }

    clipRect.move(-windowRect.x(), -windowRect.y());
    unobscuredRect.move(-windowRect.x(), -windowRect.y());
    getPluginOcclusions(m_element, this->parent(), frameRect(), cutOutRects);
    
    // Convert to the plugin position.
    for (size_t i = 0; i < cutOutRects.size(); i++)
        cutOutRects[i].move(-frameRect().x(), -frameRect().y());
}

bool WebPluginContainerImpl::pluginShouldPersist() const
{
    return m_webPlugin->shouldPersist();
}

} // namespace blink
