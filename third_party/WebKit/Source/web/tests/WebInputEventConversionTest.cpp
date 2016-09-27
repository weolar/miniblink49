/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
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
#include "web/WebInputEventConversion.h"

#include "core/dom/Touch.h"
#include "core/dom/TouchList.h"
#include "core/events/GestureEvent.h"
#include "core/events/KeyboardEvent.h"
#include "core/events/MouseEvent.h"
#include "core/events/TouchEvent.h"
#include "core/events/WheelEvent.h"
#include "core/frame/FrameHost.h"
#include "core/frame/FrameView.h"
#include "core/frame/LocalFrame.h"
#include "core/layout/LayoutView.h"
#include "core/page/Page.h"
#include "platform/testing/URLTestHelpers.h"
#include "public/web/WebFrame.h"
#include "public/web/WebSettings.h"
#include "web/WebViewImpl.h"
#include "web/tests/FrameTestHelpers.h"
#include <gtest/gtest.h>

namespace blink {

PassRefPtrWillBeRawPtr<KeyboardEvent> createKeyboardEventWithLocation(KeyboardEvent::KeyLocationCode location)
{
    return KeyboardEvent::create("keydown", true, true, 0, "", "", "", location, false, false, false, false);
}

int getModifiersForKeyLocationCode(KeyboardEvent::KeyLocationCode location)
{
    RefPtrWillBeRawPtr<KeyboardEvent> event = createKeyboardEventWithLocation(location);
    WebKeyboardEventBuilder convertedEvent(*event);
    return convertedEvent.modifiers;
}

TEST(WebInputEventConversionTest, WebKeyboardEventBuilder)
{
    // Test key location conversion.
    int modifiers = getModifiersForKeyLocationCode(KeyboardEvent::DOM_KEY_LOCATION_STANDARD);
    EXPECT_FALSE(modifiers & WebInputEvent::IsKeyPad || modifiers & WebInputEvent::IsLeft || modifiers & WebInputEvent::IsRight);

    modifiers = getModifiersForKeyLocationCode(KeyboardEvent::DOM_KEY_LOCATION_LEFT);
    EXPECT_TRUE(modifiers & WebInputEvent::IsLeft);
    EXPECT_FALSE(modifiers & WebInputEvent::IsKeyPad || modifiers & WebInputEvent::IsRight);

    modifiers = getModifiersForKeyLocationCode(KeyboardEvent::DOM_KEY_LOCATION_RIGHT);
    EXPECT_TRUE(modifiers & WebInputEvent::IsRight);
    EXPECT_FALSE(modifiers & WebInputEvent::IsKeyPad || modifiers & WebInputEvent::IsLeft);

    modifiers = getModifiersForKeyLocationCode(KeyboardEvent::DOM_KEY_LOCATION_NUMPAD);
    EXPECT_TRUE(modifiers & WebInputEvent::IsKeyPad);
    EXPECT_FALSE(modifiers & WebInputEvent::IsLeft || modifiers & WebInputEvent::IsRight);
}

TEST(WebInputEventConversionTest, WebMouseEventBuilder)
{
    RefPtrWillBeRawPtr<TouchEvent> event = TouchEvent::create();
    WebMouseEventBuilder mouse(0, 0, *event);
    EXPECT_EQ(WebInputEvent::Undefined, mouse.type);
}

TEST(WebInputEventConversionTest, WebTouchEventBuilder)
{
    const std::string baseURL("http://www.test0.com/");
    const std::string fileName("fixed_layout.html");

    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(baseURL.c_str()), WebString::fromUTF8("fixed_layout.html"));
    FrameTestHelpers::WebViewHelper webViewHelper;
    WebViewImpl* webViewImpl = webViewHelper.initializeAndLoad(baseURL + fileName, true);
    int pageWidth = 640;
    int pageHeight = 480;
    webViewImpl->resize(WebSize(pageWidth, pageHeight));
    webViewImpl->layout();

    RefPtrWillBeRawPtr<Document> document = toLocalFrame(webViewImpl->page()->mainFrame())->document();
    LocalDOMWindow* domWindow = document->domWindow();
    LayoutView* documentLayoutView = document->layoutView();

    WebTouchPoint p0, p1;
    p0.id = 1;
    p1.id = 2;
    p0.screenPosition = WebFloatPoint(100.f, 50.f);
    p1.screenPosition = WebFloatPoint(150.f, 25.f);
    p0.position = WebFloatPoint(10.f, 10.f);
    p1.position = WebFloatPoint(5.f, 5.f);
    p0.radiusX = p1.radiusY = 10.f;
    p0.radiusY = p1.radiusX = 5.f;
    p0.rotationAngle = p1.rotationAngle = 1.f;
    p0.force = p1.force = 25.f;

    RefPtrWillBeRawPtr<Touch> touch0 = Touch::create(toLocalFrame(webViewImpl->page()->mainFrame()), document.get(), p0.id, p0.screenPosition, p0.position, FloatSize(p0.radiusX, p0.radiusY), p0.rotationAngle, p0.force);
    RefPtrWillBeRawPtr<Touch> touch1 = Touch::create(toLocalFrame(webViewImpl->page()->mainFrame()), document.get(), p1.id, p1.screenPosition, p1.position, FloatSize(p1.radiusX, p1.radiusY), p1.rotationAngle, p1.force);

    // Test touchstart.
    {
        RefPtrWillBeRawPtr<TouchList> touchList = TouchList::create();
        touchList->append(touch0);
        RefPtrWillBeRawPtr<TouchEvent> touchEvent = TouchEvent::create(touchList.get(), touchList.get(), touchList.get(), EventTypeNames::touchstart, domWindow, false, false, false, false, false, false);

        WebTouchEventBuilder webTouchBuilder(documentLayoutView, *touchEvent);
        ASSERT_EQ(1u, webTouchBuilder.touchesLength);
        EXPECT_EQ(WebInputEvent::TouchStart, webTouchBuilder.type);
        EXPECT_EQ(WebTouchPoint::StatePressed, webTouchBuilder.touches[0].state);
        EXPECT_FLOAT_EQ(p0.screenPosition.x, webTouchBuilder.touches[0].screenPosition.x);
        EXPECT_FLOAT_EQ(p0.screenPosition.y, webTouchBuilder.touches[0].screenPosition.y);
        EXPECT_FLOAT_EQ(p0.position.x, webTouchBuilder.touches[0].position.x);
        EXPECT_FLOAT_EQ(p0.position.y, webTouchBuilder.touches[0].position.y);
        EXPECT_FLOAT_EQ(p0.radiusX, webTouchBuilder.touches[0].radiusX);
        EXPECT_FLOAT_EQ(p0.radiusY, webTouchBuilder.touches[0].radiusY);
        EXPECT_FLOAT_EQ(p0.rotationAngle, webTouchBuilder.touches[0].rotationAngle);
        EXPECT_FLOAT_EQ(p0.force, webTouchBuilder.touches[0].force);
    }

    // Test touchmove.
    {
        RefPtrWillBeRawPtr<TouchList> activeTouchList = TouchList::create();
        RefPtrWillBeRawPtr<TouchList> movedTouchList = TouchList::create();
        activeTouchList->append(touch0);
        activeTouchList->append(touch1);
        movedTouchList->append(touch0);
        RefPtrWillBeRawPtr<TouchEvent> touchEvent = TouchEvent::create(activeTouchList.get(), activeTouchList.get(), movedTouchList.get(), EventTypeNames::touchmove, domWindow, false, false, false, false, false, false);

        WebTouchEventBuilder webTouchBuilder(documentLayoutView, *touchEvent);
        ASSERT_EQ(2u, webTouchBuilder.touchesLength);
        EXPECT_EQ(WebInputEvent::TouchMove, webTouchBuilder.type);
        EXPECT_EQ(WebTouchPoint::StateMoved, webTouchBuilder.touches[0].state);
        EXPECT_EQ(WebTouchPoint::StateStationary, webTouchBuilder.touches[1].state);
        EXPECT_EQ(p0.id, webTouchBuilder.touches[0].id);
        EXPECT_EQ(p1.id, webTouchBuilder.touches[1].id);
    }

    // Test touchmove, different point yields same ordering.
    {
        RefPtrWillBeRawPtr<TouchList> activeTouchList = TouchList::create();
        RefPtrWillBeRawPtr<TouchList> movedTouchList = TouchList::create();
        activeTouchList->append(touch0);
        activeTouchList->append(touch1);
        movedTouchList->append(touch1);
        RefPtrWillBeRawPtr<TouchEvent> touchEvent = TouchEvent::create(activeTouchList.get(), activeTouchList.get(), movedTouchList.get(), EventTypeNames::touchmove, domWindow, false, false, false, false, false, false);

        WebTouchEventBuilder webTouchBuilder(documentLayoutView, *touchEvent);
        ASSERT_EQ(2u, webTouchBuilder.touchesLength);
        EXPECT_EQ(WebInputEvent::TouchMove, webTouchBuilder.type);
        EXPECT_EQ(WebTouchPoint::StateStationary, webTouchBuilder.touches[0].state);
        EXPECT_EQ(WebTouchPoint::StateMoved, webTouchBuilder.touches[1].state);
        EXPECT_EQ(p0.id, webTouchBuilder.touches[0].id);
        EXPECT_EQ(p1.id, webTouchBuilder.touches[1].id);
    }

    // Test touchend.
    {
        RefPtrWillBeRawPtr<TouchList> activeTouchList = TouchList::create();
        RefPtrWillBeRawPtr<TouchList> releasedTouchList = TouchList::create();
        activeTouchList->append(touch0);
        releasedTouchList->append(touch1);
        RefPtrWillBeRawPtr<TouchEvent> touchEvent = TouchEvent::create(activeTouchList.get(), activeTouchList.get(), releasedTouchList.get(), EventTypeNames::touchend, domWindow, false, false, false, false, false, false);

        WebTouchEventBuilder webTouchBuilder(documentLayoutView, *touchEvent);
        ASSERT_EQ(2u, webTouchBuilder.touchesLength);
        EXPECT_EQ(WebInputEvent::TouchEnd, webTouchBuilder.type);
        EXPECT_EQ(WebTouchPoint::StateStationary, webTouchBuilder.touches[0].state);
        EXPECT_EQ(WebTouchPoint::StateReleased, webTouchBuilder.touches[1].state);
        EXPECT_EQ(p0.id, webTouchBuilder.touches[0].id);
        EXPECT_EQ(p1.id, webTouchBuilder.touches[1].id);
    }

    // Test touchcancel.
    {
        RefPtrWillBeRawPtr<TouchList> activeTouchList = TouchList::create();
        RefPtrWillBeRawPtr<TouchList> cancelledTouchList = TouchList::create();
        cancelledTouchList->append(touch0);
        cancelledTouchList->append(touch1);
        RefPtrWillBeRawPtr<TouchEvent> touchEvent = TouchEvent::create(activeTouchList.get(), activeTouchList.get(), cancelledTouchList.get(), EventTypeNames::touchcancel, domWindow, false, false, false, false, false, false);

        WebTouchEventBuilder webTouchBuilder(documentLayoutView, *touchEvent);
        ASSERT_EQ(2u, webTouchBuilder.touchesLength);
        EXPECT_EQ(WebInputEvent::TouchCancel, webTouchBuilder.type);
        EXPECT_EQ(WebTouchPoint::StateCancelled, webTouchBuilder.touches[0].state);
        EXPECT_EQ(WebTouchPoint::StateCancelled, webTouchBuilder.touches[1].state);
        EXPECT_EQ(p0.id, webTouchBuilder.touches[0].id);
        EXPECT_EQ(p1.id, webTouchBuilder.touches[1].id);
    }

    // Test max point limit.
    {
        RefPtrWillBeRawPtr<TouchList> touchList = TouchList::create();
        RefPtrWillBeRawPtr<TouchList> changedTouchList = TouchList::create();
        for (int i = 0; i <= static_cast<int>(WebTouchEvent::touchesLengthCap) * 2; ++i) {
            RefPtrWillBeRawPtr<Touch> touch = Touch::create(toLocalFrame(webViewImpl->page()->mainFrame()), document.get(), i, p0.screenPosition, p0.position, FloatSize(p0.radiusX, p0.radiusY), p0.rotationAngle, p0.force);
            touchList->append(touch);
            changedTouchList->append(touch);
        }
        RefPtrWillBeRawPtr<TouchEvent> touchEvent = TouchEvent::create(touchList.get(), touchList.get(), touchList.get(), EventTypeNames::touchstart, domWindow, false, false, false, false, false, false);

        WebTouchEventBuilder webTouchBuilder(documentLayoutView, *touchEvent);
        ASSERT_EQ(static_cast<unsigned>(WebTouchEvent::touchesLengthCap), webTouchBuilder.touchesLength);
    }
}

TEST(WebInputEventConversionTest, InputEventsScaling)
{
    const std::string baseURL("http://www.test1.com/");
    const std::string fileName("fixed_layout.html");

    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(baseURL.c_str()), WebString::fromUTF8("fixed_layout.html"));
    FrameTestHelpers::WebViewHelper webViewHelper;
    WebViewImpl* webViewImpl = webViewHelper.initializeAndLoad(baseURL + fileName, true);
    webViewImpl->settings()->setViewportEnabled(true);
    int pageWidth = 640;
    int pageHeight = 480;
    webViewImpl->resize(WebSize(pageWidth, pageHeight));
    webViewImpl->layout();

    webViewImpl->setPageScaleFactor(2);

    FrameView* view = toLocalFrame(webViewImpl->page()->mainFrame())->view();
    RefPtrWillBeRawPtr<Document> document = toLocalFrame(webViewImpl->page()->mainFrame())->document();
    LocalDOMWindow* domWindow = document->domWindow();
    LayoutView* documentLayoutView = document->layoutView();

    {
        WebMouseEvent webMouseEvent;
        webMouseEvent.type = WebInputEvent::MouseMove;
        webMouseEvent.x = 10;
        webMouseEvent.y = 10;
        webMouseEvent.windowX = 10;
        webMouseEvent.windowY = 10;
        webMouseEvent.globalX = 10;
        webMouseEvent.globalY = 10;
        webMouseEvent.movementX = 10;
        webMouseEvent.movementY = 10;

        PlatformMouseEventBuilder platformMouseBuilder(view, webMouseEvent);
        EXPECT_EQ(5, platformMouseBuilder.position().x());
        EXPECT_EQ(5, platformMouseBuilder.position().y());
        EXPECT_EQ(10, platformMouseBuilder.globalPosition().x());
        EXPECT_EQ(10, platformMouseBuilder.globalPosition().y());
        EXPECT_EQ(5, platformMouseBuilder.movementDelta().x());
        EXPECT_EQ(5, platformMouseBuilder.movementDelta().y());
    }

    {
        WebGestureEvent webGestureEvent;
        webGestureEvent.type = WebInputEvent::GestureScrollUpdate;
        webGestureEvent.x = 10;
        webGestureEvent.y = 12;
        webGestureEvent.globalX = 20;
        webGestureEvent.globalY = 22;
        webGestureEvent.data.scrollUpdate.deltaX = 30;
        webGestureEvent.data.scrollUpdate.deltaY = 32;
        webGestureEvent.data.scrollUpdate.velocityX = 40;
        webGestureEvent.data.scrollUpdate.velocityY = 42;
        webGestureEvent.data.scrollUpdate.inertial = true;
        webGestureEvent.data.scrollUpdate.preventPropagation = true;

        PlatformGestureEventBuilder platformGestureBuilder(view, webGestureEvent);
        EXPECT_EQ(5, platformGestureBuilder.position().x());
        EXPECT_EQ(6, platformGestureBuilder.position().y());
        EXPECT_EQ(20, platformGestureBuilder.globalPosition().x());
        EXPECT_EQ(22, platformGestureBuilder.globalPosition().y());
        EXPECT_EQ(15, platformGestureBuilder.deltaX());
        EXPECT_EQ(16, platformGestureBuilder.deltaY());
        // TODO: The velocity values may need to be scaled to page scale in
        // order to remain consist with delta values.
        EXPECT_EQ(40, platformGestureBuilder.velocityX());
        EXPECT_EQ(42, platformGestureBuilder.velocityY());
        EXPECT_TRUE(platformGestureBuilder.inertial());
        EXPECT_TRUE(platformGestureBuilder.preventPropagation());
    }

    {
        WebGestureEvent webGestureEvent;
        webGestureEvent.type = WebInputEvent::GestureScrollEnd;
        webGestureEvent.x = 10;
        webGestureEvent.y = 12;
        webGestureEvent.globalX = 20;
        webGestureEvent.globalY = 22;

        PlatformGestureEventBuilder platformGestureBuilder(view, webGestureEvent);
        EXPECT_EQ(5, platformGestureBuilder.position().x());
        EXPECT_EQ(6, platformGestureBuilder.position().y());
        EXPECT_EQ(20, platformGestureBuilder.globalPosition().x());
        EXPECT_EQ(22, platformGestureBuilder.globalPosition().y());
        EXPECT_FALSE(platformGestureBuilder.inertial());
    }

    {
        WebGestureEvent webGestureEvent;
        webGestureEvent.type = WebInputEvent::GestureTap;
        webGestureEvent.data.tap.width = 10;
        webGestureEvent.data.tap.height = 10;

        PlatformGestureEventBuilder platformGestureBuilder(view, webGestureEvent);
        EXPECT_EQ(5, platformGestureBuilder.area().width());
        EXPECT_EQ(5, platformGestureBuilder.area().height());
    }

    {
        WebGestureEvent webGestureEvent;
        webGestureEvent.type = WebInputEvent::GestureTapUnconfirmed;
        webGestureEvent.data.tap.width = 10;
        webGestureEvent.data.tap.height = 10;

        PlatformGestureEventBuilder platformGestureBuilder(view, webGestureEvent);
        EXPECT_EQ(5, platformGestureBuilder.area().width());
        EXPECT_EQ(5, platformGestureBuilder.area().height());
    }

    {
        WebGestureEvent webGestureEvent;
        webGestureEvent.type = WebInputEvent::GestureTapDown;
        webGestureEvent.data.tapDown.width = 10;
        webGestureEvent.data.tapDown.height = 10;

        PlatformGestureEventBuilder platformGestureBuilder(view, webGestureEvent);
        EXPECT_EQ(5, platformGestureBuilder.area().width());
        EXPECT_EQ(5, platformGestureBuilder.area().height());
    }

    {
        WebGestureEvent webGestureEvent;
        webGestureEvent.type = WebInputEvent::GestureShowPress;
        webGestureEvent.data.showPress.width = 10;
        webGestureEvent.data.showPress.height = 10;

        PlatformGestureEventBuilder platformGestureBuilder(view, webGestureEvent);
        EXPECT_EQ(5, platformGestureBuilder.area().width());
        EXPECT_EQ(5, platformGestureBuilder.area().height());
    }

    {
        WebGestureEvent webGestureEvent;
        webGestureEvent.type = WebInputEvent::GestureLongPress;
        webGestureEvent.data.longPress.width = 10;
        webGestureEvent.data.longPress.height = 10;

        PlatformGestureEventBuilder platformGestureBuilder(view, webGestureEvent);
        EXPECT_EQ(5, platformGestureBuilder.area().width());
        EXPECT_EQ(5, platformGestureBuilder.area().height());
    }

    {
        WebGestureEvent webGestureEvent;
        webGestureEvent.type = WebInputEvent::GestureTwoFingerTap;
        webGestureEvent.data.twoFingerTap.firstFingerWidth = 10;
        webGestureEvent.data.twoFingerTap.firstFingerHeight = 10;

        PlatformGestureEventBuilder platformGestureBuilder(view, webGestureEvent);
        EXPECT_EQ(5, platformGestureBuilder.area().width());
        EXPECT_EQ(5, platformGestureBuilder.area().height());
    }

    {
        WebTouchEvent webTouchEvent;
        webTouchEvent.type = WebInputEvent::TouchMove;
        webTouchEvent.touchesLength = 1;
        webTouchEvent.touches[0].state = WebTouchPoint::StateMoved;
        webTouchEvent.touches[0].screenPosition.x = 10.6f;
        webTouchEvent.touches[0].screenPosition.y = 10.4f;
        webTouchEvent.touches[0].position.x = 10.6f;
        webTouchEvent.touches[0].position.y = 10.4f;
        webTouchEvent.touches[0].radiusX = 10.6f;
        webTouchEvent.touches[0].radiusY = 10.4f;

        EXPECT_FLOAT_EQ(10.6f, webTouchEvent.touches[0].screenPosition.x);
        EXPECT_FLOAT_EQ(10.4f, webTouchEvent.touches[0].screenPosition.y);
        EXPECT_FLOAT_EQ(10.6f, webTouchEvent.touches[0].position.x);
        EXPECT_FLOAT_EQ(10.4f, webTouchEvent.touches[0].position.y);
        EXPECT_FLOAT_EQ(10.6f, webTouchEvent.touches[0].radiusX);
        EXPECT_FLOAT_EQ(10.4f, webTouchEvent.touches[0].radiusY);

        PlatformTouchEventBuilder platformTouchBuilder(view, webTouchEvent);
        EXPECT_FLOAT_EQ(10.6f, platformTouchBuilder.touchPoints()[0].screenPos().x());
        EXPECT_FLOAT_EQ(10.4f, platformTouchBuilder.touchPoints()[0].screenPos().y());
        EXPECT_FLOAT_EQ(5.3f, platformTouchBuilder.touchPoints()[0].pos().x());
        EXPECT_FLOAT_EQ(5.2f, platformTouchBuilder.touchPoints()[0].pos().y());
        EXPECT_FLOAT_EQ(5.3f, platformTouchBuilder.touchPoints()[0].radius().width());
        EXPECT_FLOAT_EQ(5.2f, platformTouchBuilder.touchPoints()[0].radius().height());
    }

    // Reverse builders should *not* go back to physical pixels, as they are used for plugins
    // which expect CSS pixel coordinates.
    {
        PlatformMouseEvent platformMouseEvent(IntPoint(10, 10), IntPoint(10, 10), LeftButton, PlatformEvent::MouseMoved, 1, false, false, false, false, PlatformMouseEvent::RealOrIndistinguishable, 0);
        RefPtrWillBeRawPtr<MouseEvent> mouseEvent = MouseEvent::create(EventTypeNames::mousemove, domWindow, platformMouseEvent, 0, document);
        WebMouseEventBuilder webMouseBuilder(view, documentLayoutView, *mouseEvent);

        EXPECT_EQ(10, webMouseBuilder.x);
        EXPECT_EQ(10, webMouseBuilder.y);
        EXPECT_EQ(10, webMouseBuilder.globalX);
        EXPECT_EQ(10, webMouseBuilder.globalY);
        EXPECT_EQ(10, webMouseBuilder.windowX);
        EXPECT_EQ(10, webMouseBuilder.windowY);
    }

    {
        PlatformMouseEvent platformMouseEvent(IntPoint(10, 10), IntPoint(10, 10), NoButton, PlatformEvent::MouseMoved, 1, false, false, false, false, PlatformMouseEvent::RealOrIndistinguishable, 0);
        RefPtrWillBeRawPtr<MouseEvent> mouseEvent = MouseEvent::create(EventTypeNames::mousemove, domWindow, platformMouseEvent, 0, document);
        WebMouseEventBuilder webMouseBuilder(view, documentLayoutView, *mouseEvent);
        EXPECT_EQ(WebMouseEvent::ButtonNone, webMouseBuilder.button);
    }

    {
        PlatformGestureEvent platformGestureEvent(PlatformEvent::GestureScrollUpdate, IntPoint(10, 12), IntPoint(20, 22), IntSize(25, 27), 0,
            false, false, false, false);
        platformGestureEvent.setScrollGestureData(30, 32, 40, 42, true, true);
        // FIXME: GestureEvent does not preserve velocityX, velocityY,
        // preventPropagation, or inertial. It also fails to scale
        // coordinates (x, y, deltaX, deltaY) to the page scale. This
        // may lead to unexpected bugs if a PlatformGestureEvent is
        // transformed into WebGestureEvent and back.
        RefPtrWillBeRawPtr<GestureEvent> gestureEvent = GestureEvent::create(domWindow, platformGestureEvent);
        WebGestureEventBuilder webGestureBuilder(documentLayoutView, *gestureEvent);

        EXPECT_EQ(10, webGestureBuilder.x);
        EXPECT_EQ(12, webGestureBuilder.y);
        EXPECT_EQ(20, webGestureBuilder.globalX);
        EXPECT_EQ(22, webGestureBuilder.globalY);
        EXPECT_EQ(30, webGestureBuilder.data.scrollUpdate.deltaX);
        EXPECT_EQ(32, webGestureBuilder.data.scrollUpdate.deltaY);
        EXPECT_EQ(0, webGestureBuilder.data.scrollUpdate.velocityX);
        EXPECT_EQ(0, webGestureBuilder.data.scrollUpdate.velocityY);
        EXPECT_FALSE(webGestureBuilder.data.scrollUpdate.inertial);
        EXPECT_FALSE(webGestureBuilder.data.scrollUpdate.preventPropagation);
    }

    {
        RefPtrWillBeRawPtr<Touch> touch = Touch::create(toLocalFrame(webViewImpl->page()->mainFrame()), document.get(), 0, FloatPoint(10, 9.5), FloatPoint(3.5, 2), FloatSize(4, 4.5), 0, 0);
        RefPtrWillBeRawPtr<TouchList> touchList = TouchList::create();
        touchList->append(touch);
        RefPtrWillBeRawPtr<TouchEvent> touchEvent = TouchEvent::create(touchList.get(), touchList.get(), touchList.get(), EventTypeNames::touchmove, domWindow, false, false, false, false, false, false);

        WebTouchEventBuilder webTouchBuilder(documentLayoutView, *touchEvent);
        ASSERT_EQ(1u, webTouchBuilder.touchesLength);
        EXPECT_EQ(10, webTouchBuilder.touches[0].screenPosition.x);
        EXPECT_FLOAT_EQ(9.5, webTouchBuilder.touches[0].screenPosition.y);
        EXPECT_FLOAT_EQ(3.5, webTouchBuilder.touches[0].position.x);
        EXPECT_FLOAT_EQ(2, webTouchBuilder.touches[0].position.y);
        EXPECT_FLOAT_EQ(4, webTouchBuilder.touches[0].radiusX);
        EXPECT_FLOAT_EQ(4.5, webTouchBuilder.touches[0].radiusY);
        EXPECT_FALSE(webTouchBuilder.cancelable);
    }
}

TEST(WebInputEventConversionTest, InputEventsTransform)
{
    const std::string baseURL("http://www.test2.com/");
    const std::string fileName("fixed_layout.html");

    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(baseURL.c_str()), WebString::fromUTF8("fixed_layout.html"));
    FrameTestHelpers::WebViewHelper webViewHelper;
    WebViewImpl* webViewImpl = webViewHelper.initializeAndLoad(baseURL + fileName, true);
    webViewImpl->settings()->setViewportEnabled(true);
    int pageWidth = 640;
    int pageHeight = 480;
    webViewImpl->resize(WebSize(pageWidth, pageHeight));
    webViewImpl->layout();

    webViewImpl->setPageScaleFactor(2);
    webViewImpl->setRootLayerTransform(WebSize(10, 20), 1.5);

    FrameView* view = toLocalFrame(webViewImpl->page()->mainFrame())->view();

    {
        WebMouseEvent webMouseEvent;
        webMouseEvent.type = WebInputEvent::MouseMove;
        webMouseEvent.x = 100;
        webMouseEvent.y = 110;
        webMouseEvent.windowX = 100;
        webMouseEvent.windowY = 110;
        webMouseEvent.globalX = 100;
        webMouseEvent.globalY = 110;
        webMouseEvent.movementX = 60;
        webMouseEvent.movementY = 60;

        PlatformMouseEventBuilder platformMouseBuilder(view, webMouseEvent);
        EXPECT_EQ(30, platformMouseBuilder.position().x());
        EXPECT_EQ(30, platformMouseBuilder.position().y());
        EXPECT_EQ(100, platformMouseBuilder.globalPosition().x());
        EXPECT_EQ(110, platformMouseBuilder.globalPosition().y());
        EXPECT_EQ(20, platformMouseBuilder.movementDelta().x());
        EXPECT_EQ(20, platformMouseBuilder.movementDelta().y());
    }

    {
        WebGestureEvent webGestureEvent;
        webGestureEvent.type = WebInputEvent::GestureScrollUpdate;
        webGestureEvent.x = 100;
        webGestureEvent.y = 110;
        webGestureEvent.globalX = 100;
        webGestureEvent.globalY = 110;
        webGestureEvent.data.scrollUpdate.deltaX = 60;
        webGestureEvent.data.scrollUpdate.deltaY = 60;

        PlatformGestureEventBuilder platformGestureBuilder(view, webGestureEvent);
        EXPECT_EQ(30, platformGestureBuilder.position().x());
        EXPECT_EQ(30, platformGestureBuilder.position().y());
        EXPECT_EQ(100, platformGestureBuilder.globalPosition().x());
        EXPECT_EQ(110, platformGestureBuilder.globalPosition().y());
        EXPECT_EQ(20, platformGestureBuilder.deltaX());
        EXPECT_EQ(20, platformGestureBuilder.deltaY());
    }

    {
        WebGestureEvent webGestureEvent;
        webGestureEvent.type = WebInputEvent::GestureTap;
        webGestureEvent.data.tap.width = 30;
        webGestureEvent.data.tap.height = 30;

        PlatformGestureEventBuilder platformGestureBuilder(view, webGestureEvent);
        EXPECT_EQ(10, platformGestureBuilder.area().width());
        EXPECT_EQ(10, platformGestureBuilder.area().height());
    }

    {
        WebGestureEvent webGestureEvent;
        webGestureEvent.type = WebInputEvent::GestureTapUnconfirmed;
        webGestureEvent.data.tap.width = 30;
        webGestureEvent.data.tap.height = 30;

        PlatformGestureEventBuilder platformGestureBuilder(view, webGestureEvent);
        EXPECT_EQ(10, platformGestureBuilder.area().width());
        EXPECT_EQ(10, platformGestureBuilder.area().height());
    }

    {
        WebGestureEvent webGestureEvent;
        webGestureEvent.type = WebInputEvent::GestureTapDown;
        webGestureEvent.data.tapDown.width = 30;
        webGestureEvent.data.tapDown.height = 30;

        PlatformGestureEventBuilder platformGestureBuilder(view, webGestureEvent);
        EXPECT_EQ(10, platformGestureBuilder.area().width());
        EXPECT_EQ(10, platformGestureBuilder.area().height());
    }

    {
        WebGestureEvent webGestureEvent;
        webGestureEvent.type = WebInputEvent::GestureShowPress;
        webGestureEvent.data.showPress.width = 30;
        webGestureEvent.data.showPress.height = 30;

        PlatformGestureEventBuilder platformGestureBuilder(view, webGestureEvent);
        EXPECT_EQ(10, platformGestureBuilder.area().width());
        EXPECT_EQ(10, platformGestureBuilder.area().height());
    }

    {
        WebGestureEvent webGestureEvent;
        webGestureEvent.type = WebInputEvent::GestureLongPress;
        webGestureEvent.data.longPress.width = 30;
        webGestureEvent.data.longPress.height = 30;

        PlatformGestureEventBuilder platformGestureBuilder(view, webGestureEvent);
        EXPECT_EQ(10, platformGestureBuilder.area().width());
        EXPECT_EQ(10, platformGestureBuilder.area().height());
    }

    {
        WebGestureEvent webGestureEvent;
        webGestureEvent.type = WebInputEvent::GestureTwoFingerTap;
        webGestureEvent.data.twoFingerTap.firstFingerWidth = 30;
        webGestureEvent.data.twoFingerTap.firstFingerHeight = 30;

        PlatformGestureEventBuilder platformGestureBuilder(view, webGestureEvent);
        EXPECT_EQ(10, platformGestureBuilder.area().width());
        EXPECT_EQ(10, platformGestureBuilder.area().height());
    }

    {
        WebTouchEvent webTouchEvent;
        webTouchEvent.type = WebInputEvent::TouchMove;
        webTouchEvent.touchesLength = 1;
        webTouchEvent.touches[0].state = WebTouchPoint::StateMoved;
        webTouchEvent.touches[0].screenPosition.x = 100;
        webTouchEvent.touches[0].screenPosition.y = 110;
        webTouchEvent.touches[0].position.x = 100;
        webTouchEvent.touches[0].position.y = 110;
        webTouchEvent.touches[0].radiusX = 30;
        webTouchEvent.touches[0].radiusY = 30;

        PlatformTouchEventBuilder platformTouchBuilder(view, webTouchEvent);
        EXPECT_FLOAT_EQ(100, platformTouchBuilder.touchPoints()[0].screenPos().x());
        EXPECT_FLOAT_EQ(110, platformTouchBuilder.touchPoints()[0].screenPos().y());
        EXPECT_FLOAT_EQ(30, platformTouchBuilder.touchPoints()[0].pos().x());
        EXPECT_FLOAT_EQ(30, platformTouchBuilder.touchPoints()[0].pos().y());
        EXPECT_FLOAT_EQ(10, platformTouchBuilder.touchPoints()[0].radius().width());
        EXPECT_FLOAT_EQ(10, platformTouchBuilder.touchPoints()[0].radius().height());
    }
}

TEST(WebInputEventConversionTest, InputEventsConversions)
{
    const std::string baseURL("http://www.test3.com/");
    const std::string fileName("fixed_layout.html");

    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(baseURL.c_str()), WebString::fromUTF8("fixed_layout.html"));
    FrameTestHelpers::WebViewHelper webViewHelper;
    WebViewImpl* webViewImpl = webViewHelper.initializeAndLoad(baseURL + fileName, true);
    int pageWidth = 640;
    int pageHeight = 480;
    webViewImpl->resize(WebSize(pageWidth, pageHeight));
    webViewImpl->layout();

    FrameView* view = toLocalFrame(webViewImpl->page()->mainFrame())->view();
    RefPtrWillBeRawPtr<Document> document = toLocalFrame(webViewImpl->page()->mainFrame())->document();
    LocalDOMWindow* domWindow = document->domWindow();
    LayoutView* documentLayoutView = document->layoutView();

    {
        WebGestureEvent webGestureEvent;
        webGestureEvent.type = WebInputEvent::GestureTap;
        webGestureEvent.x = 10;
        webGestureEvent.y = 10;
        webGestureEvent.globalX = 10;
        webGestureEvent.globalY = 10;
        webGestureEvent.data.tap.tapCount = 1;
        webGestureEvent.data.tap.width = 10;
        webGestureEvent.data.tap.height = 10;

        PlatformGestureEventBuilder platformGestureBuilder(view, webGestureEvent);
        EXPECT_EQ(10.f, platformGestureBuilder.position().x());
        EXPECT_EQ(10.f, platformGestureBuilder.position().y());
        EXPECT_EQ(10.f, platformGestureBuilder.globalPosition().x());
        EXPECT_EQ(10.f, platformGestureBuilder.globalPosition().y());
        EXPECT_EQ(1, platformGestureBuilder.tapCount());

        RefPtrWillBeRawPtr<GestureEvent> coreGestureEvent = GestureEvent::create(domWindow, platformGestureBuilder);
        WebGestureEventBuilder recreatedWebGestureEvent(documentLayoutView, *coreGestureEvent);
        EXPECT_EQ(webGestureEvent.type, recreatedWebGestureEvent.type);
        EXPECT_EQ(webGestureEvent.x, recreatedWebGestureEvent.x);
        EXPECT_EQ(webGestureEvent.y, recreatedWebGestureEvent.y);
        EXPECT_EQ(webGestureEvent.globalX, recreatedWebGestureEvent.globalX);
        EXPECT_EQ(webGestureEvent.globalY, recreatedWebGestureEvent.globalY);
        EXPECT_EQ(webGestureEvent.data.tap.tapCount, recreatedWebGestureEvent.data.tap.tapCount);
    }
}

TEST(WebInputEventConversionTest, PinchViewportOffset)
{
    const std::string baseURL("http://www.test4.com/");
    const std::string fileName("fixed_layout.html");

    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(baseURL.c_str()), WebString::fromUTF8("fixed_layout.html"));
    FrameTestHelpers::WebViewHelper webViewHelper;
    WebViewImpl* webViewImpl = webViewHelper.initializeAndLoad(baseURL + fileName, true);
    int pageWidth = 640;
    int pageHeight = 480;
    webViewImpl->resize(WebSize(pageWidth, pageHeight));
    webViewImpl->layout();

    webViewImpl->setPageScaleFactor(2);

    IntPoint pinchOffset(35, 60);
    webViewImpl->page()->frameHost().pinchViewport().setLocation(pinchOffset);

    FrameView* view = toLocalFrame(webViewImpl->page()->mainFrame())->view();

    {
        WebMouseEvent webMouseEvent;
        webMouseEvent.type = WebInputEvent::MouseMove;
        webMouseEvent.x = 10;
        webMouseEvent.y = 10;
        webMouseEvent.windowX = 10;
        webMouseEvent.windowY = 10;
        webMouseEvent.globalX = 10;
        webMouseEvent.globalY = 10;

        PlatformMouseEventBuilder platformMouseBuilder(view, webMouseEvent);
        EXPECT_EQ(5 + pinchOffset.x(), platformMouseBuilder.position().x());
        EXPECT_EQ(5 + pinchOffset.y(), platformMouseBuilder.position().y());
        EXPECT_EQ(10, platformMouseBuilder.globalPosition().x());
        EXPECT_EQ(10, platformMouseBuilder.globalPosition().y());
    }

    {
        WebMouseWheelEvent webMouseWheelEvent;
        webMouseWheelEvent.type = WebInputEvent::MouseWheel;
        webMouseWheelEvent.x = 10;
        webMouseWheelEvent.y = 10;
        webMouseWheelEvent.windowX = 10;
        webMouseWheelEvent.windowY = 10;
        webMouseWheelEvent.globalX = 10;
        webMouseWheelEvent.globalY = 10;

        PlatformWheelEventBuilder platformWheelBuilder(view, webMouseWheelEvent);
        EXPECT_EQ(5 + pinchOffset.x(), platformWheelBuilder.position().x());
        EXPECT_EQ(5 + pinchOffset.y(), platformWheelBuilder.position().y());
        EXPECT_EQ(10, platformWheelBuilder.globalPosition().x());
        EXPECT_EQ(10, platformWheelBuilder.globalPosition().y());
    }

    {
        WebGestureEvent webGestureEvent;
        webGestureEvent.type = WebInputEvent::GestureScrollUpdate;
        webGestureEvent.x = 10;
        webGestureEvent.y = 10;
        webGestureEvent.globalX = 10;
        webGestureEvent.globalY = 10;

        PlatformGestureEventBuilder platformGestureBuilder(view, webGestureEvent);
        EXPECT_EQ(5 + pinchOffset.x(), platformGestureBuilder.position().x());
        EXPECT_EQ(5 + pinchOffset.y(), platformGestureBuilder.position().y());
        EXPECT_EQ(10, platformGestureBuilder.globalPosition().x());
        EXPECT_EQ(10, platformGestureBuilder.globalPosition().y());
    }

    {
        WebTouchEvent webTouchEvent;
        webTouchEvent.type = WebInputEvent::TouchMove;
        webTouchEvent.touchesLength = 1;
        webTouchEvent.touches[0].state = WebTouchPoint::StateMoved;
        webTouchEvent.touches[0].screenPosition.x = 10.6f;
        webTouchEvent.touches[0].screenPosition.y = 10.4f;
        webTouchEvent.touches[0].position.x = 10.6f;
        webTouchEvent.touches[0].position.y = 10.4f;

        EXPECT_FLOAT_EQ(10.6f, webTouchEvent.touches[0].screenPosition.x);
        EXPECT_FLOAT_EQ(10.4f, webTouchEvent.touches[0].screenPosition.y);
        EXPECT_FLOAT_EQ(10.6f, webTouchEvent.touches[0].position.x);
        EXPECT_FLOAT_EQ(10.4f, webTouchEvent.touches[0].position.y);

        PlatformTouchEventBuilder platformTouchBuilder(view, webTouchEvent);
        EXPECT_FLOAT_EQ(10.6f, platformTouchBuilder.touchPoints()[0].screenPos().x());
        EXPECT_FLOAT_EQ(10.4f, platformTouchBuilder.touchPoints()[0].screenPos().y());
        EXPECT_FLOAT_EQ(5.3f + pinchOffset.x(), platformTouchBuilder.touchPoints()[0].pos().x());
        EXPECT_FLOAT_EQ(5.2f + pinchOffset.y(), platformTouchBuilder.touchPoints()[0].pos().y());
    }
}

TEST(WebInputEventConversionTest, ElasticOverscroll)
{
    const std::string baseURL("http://www.test5.com/");
    const std::string fileName("fixed_layout.html");

    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(baseURL.c_str()), WebString::fromUTF8("fixed_layout.html"));
    FrameTestHelpers::WebViewHelper webViewHelper;
    WebViewImpl* webViewImpl = webViewHelper.initializeAndLoad(baseURL + fileName, true);
    int pageWidth = 640;
    int pageHeight = 480;
    webViewImpl->resize(WebSize(pageWidth, pageHeight));
    webViewImpl->layout();

    FrameView* view = toLocalFrame(webViewImpl->page()->mainFrame())->view();

    FloatSize elasticOverscroll(10, -20);
    webViewImpl->applyViewportDeltas(WebFloatSize(), WebFloatSize(), elasticOverscroll, 1.0f, 0.0f);

    // Just elastic overscroll.
    {
        WebMouseEvent webMouseEvent;
        webMouseEvent.type = WebInputEvent::MouseMove;
        webMouseEvent.x = 10;
        webMouseEvent.y = 50;
        webMouseEvent.windowX = 10;
        webMouseEvent.windowY = 50;
        webMouseEvent.globalX = 10;
        webMouseEvent.globalY = 50;

        PlatformMouseEventBuilder platformMouseBuilder(view, webMouseEvent);
        EXPECT_EQ(webMouseEvent.x + elasticOverscroll.width(), platformMouseBuilder.position().x());
        EXPECT_EQ(webMouseEvent.y + elasticOverscroll.height(), platformMouseBuilder.position().y());
        EXPECT_EQ(webMouseEvent.globalX, platformMouseBuilder.globalPosition().x());
        EXPECT_EQ(webMouseEvent.globalY, platformMouseBuilder.globalPosition().y());
    }

    // Elastic overscroll and pinch-zoom (this doesn't actually ever happen,
    // but ensure that if it were to, the overscroll would be applied after the
    // pinch-zoom).
    float pageScale = 2;
    webViewImpl->setPageScaleFactor(pageScale);
    IntPoint pinchOffset(35, 60);
    webViewImpl->page()->frameHost().pinchViewport().setLocation(pinchOffset);
    {
        WebMouseEvent webMouseEvent;
        webMouseEvent.type = WebInputEvent::MouseMove;
        webMouseEvent.x = 10;
        webMouseEvent.y = 10;
        webMouseEvent.windowX = 10;
        webMouseEvent.windowY = 10;
        webMouseEvent.globalX = 10;
        webMouseEvent.globalY = 10;

        PlatformMouseEventBuilder platformMouseBuilder(view, webMouseEvent);
        EXPECT_EQ(webMouseEvent.x / pageScale + pinchOffset.x() + elasticOverscroll.width(), platformMouseBuilder.position().x());
        EXPECT_EQ(webMouseEvent.y / pageScale + pinchOffset.y() + elasticOverscroll.height(), platformMouseBuilder.position().y());
        EXPECT_EQ(webMouseEvent.globalX, platformMouseBuilder.globalPosition().x());
        EXPECT_EQ(webMouseEvent.globalY, platformMouseBuilder.globalPosition().y());
    }
}

// Page reload/navigation should not reset elastic overscroll.
TEST(WebInputEventConversionTest, ElasticOverscrollWithPageReload)
{
    const std::string baseURL("http://www.test5.com/");
    const std::string fileName("fixed_layout.html");

    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(baseURL.c_str()), WebString::fromUTF8("fixed_layout.html"));
    FrameTestHelpers::WebViewHelper webViewHelper;
    WebViewImpl* webViewImpl = webViewHelper.initializeAndLoad(baseURL + fileName, true);
    int pageWidth = 640;
    int pageHeight = 480;
    webViewImpl->resize(WebSize(pageWidth, pageHeight));
    webViewImpl->layout();

    FloatSize elasticOverscroll(10, -20);
    webViewImpl->applyViewportDeltas(WebFloatSize(), WebFloatSize(), elasticOverscroll, 1.0f, 0.0f);
    FrameTestHelpers::reloadFrame(webViewHelper.webView()->mainFrame());
    FrameView* view = toLocalFrame(webViewImpl->page()->mainFrame())->view();

    // Just elastic overscroll.
    {
        WebMouseEvent webMouseEvent;
        webMouseEvent.type = WebInputEvent::MouseMove;
        webMouseEvent.x = 10;
        webMouseEvent.y = 50;
        webMouseEvent.windowX = 10;
        webMouseEvent.windowY = 50;
        webMouseEvent.globalX = 10;
        webMouseEvent.globalY = 50;

        PlatformMouseEventBuilder platformMouseBuilder(view, webMouseEvent);
        EXPECT_EQ(webMouseEvent.x + elasticOverscroll.width(), platformMouseBuilder.position().x());
        EXPECT_EQ(webMouseEvent.y + elasticOverscroll.height(), platformMouseBuilder.position().y());
        EXPECT_EQ(webMouseEvent.globalX, platformMouseBuilder.globalPosition().x());
        EXPECT_EQ(webMouseEvent.globalY, platformMouseBuilder.globalPosition().y());
    }
}

TEST(WebInputEventConversionTest, WebMouseWheelEventBuilder)
{
    const std::string baseURL("http://www.test6.com/");
    const std::string fileName("fixed_layout.html");

    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(baseURL.c_str()), WebString::fromUTF8("fixed_layout.html"));
    FrameTestHelpers::WebViewHelper webViewHelper;
    WebViewImpl* webViewImpl = webViewHelper.initializeAndLoad(baseURL + fileName, true);
    int pageWidth = 640;
    int pageHeight = 480;
    webViewImpl->resize(WebSize(pageWidth, pageHeight));
    webViewImpl->layout();

    RefPtrWillBeRawPtr<Document> document = toLocalFrame(webViewImpl->page()->mainFrame())->document();
    RefPtrWillBeRawPtr<WheelEvent> event = WheelEvent::create(FloatPoint(1, 3), FloatPoint(5, 10),
        WheelEvent::DOM_DELTA_PAGE, document.get()->domWindow(),  IntPoint(2, 6), IntPoint(10, 30),
        true, false, false, false, 0, true, true, Event::RailsModeHorizontal);
    WebMouseWheelEventBuilder webMouseWheel(toLocalFrame(webViewImpl->page()->mainFrame())->view(), document.get()->layoutView(), *event);
    EXPECT_EQ(1, webMouseWheel.wheelTicksX);
    EXPECT_EQ(3, webMouseWheel.wheelTicksY);
    EXPECT_EQ(5, webMouseWheel.deltaX);
    EXPECT_EQ(10, webMouseWheel.deltaY);
    EXPECT_EQ(2, webMouseWheel.globalX);
    EXPECT_EQ(6, webMouseWheel.globalY);
    EXPECT_EQ(10, webMouseWheel.windowX);
    EXPECT_EQ(30, webMouseWheel.windowY);
    EXPECT_TRUE(webMouseWheel.scrollByPage);
    EXPECT_EQ(WebInputEvent::ControlKey, webMouseWheel.modifiers);
    EXPECT_TRUE(webMouseWheel.canScroll);
    EXPECT_EQ(WebInputEvent::RailsModeHorizontal, webMouseWheel.railsMode);
}

TEST(WebInputEventConversionTest, PlatformWheelEventBuilder)
{
    const std::string baseURL("http://www.test7.com/");
    const std::string fileName("fixed_layout.html");

    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(baseURL.c_str()), WebString::fromUTF8("fixed_layout.html"));
    FrameTestHelpers::WebViewHelper webViewHelper;
    WebViewImpl* webViewImpl = webViewHelper.initializeAndLoad(baseURL + fileName, true);
    int pageWidth = 640;
    int pageHeight = 480;
    webViewImpl->resize(WebSize(pageWidth, pageHeight));
    webViewImpl->layout();

    FrameView* view = toLocalFrame(webViewImpl->page()->mainFrame())->view();

    {
        WebMouseWheelEvent webMouseWheelEvent;
        webMouseWheelEvent.type = WebInputEvent::MouseWheel;
        webMouseWheelEvent.x = 0;
        webMouseWheelEvent.y = 5;
        webMouseWheelEvent.deltaX = 10;
        webMouseWheelEvent.deltaY = 15;
        webMouseWheelEvent.modifiers = WebInputEvent::ControlKey;
        webMouseWheelEvent.hasPreciseScrollingDeltas = true;
        webMouseWheelEvent.canScroll = true;
        webMouseWheelEvent.railsMode = WebInputEvent::RailsModeHorizontal;

        PlatformWheelEventBuilder platformWheelBuilder(view, webMouseWheelEvent);
        EXPECT_EQ(0, platformWheelBuilder.position().x());
        EXPECT_EQ(5, platformWheelBuilder.position().y());
        EXPECT_EQ(10, platformWheelBuilder.deltaX());
        EXPECT_EQ(15, platformWheelBuilder.deltaY());
        EXPECT_EQ(PlatformEvent::CtrlKey, platformWheelBuilder.modifiers());
        EXPECT_TRUE(platformWheelBuilder.hasPreciseScrollingDeltas());
        EXPECT_TRUE(platformWheelBuilder.canScroll());
        EXPECT_EQ(platformWheelBuilder.railsMode(), PlatformEvent::RailsModeHorizontal);
    }

    {
        WebMouseWheelEvent webMouseWheelEvent;
        webMouseWheelEvent.type = WebInputEvent::MouseWheel;
        webMouseWheelEvent.x = 5;
        webMouseWheelEvent.y = 0;
        webMouseWheelEvent.deltaX = 15;
        webMouseWheelEvent.deltaY = 10;
        webMouseWheelEvent.modifiers = WebInputEvent::ShiftKey;
        webMouseWheelEvent.hasPreciseScrollingDeltas = false;
        webMouseWheelEvent.canScroll = false;
        webMouseWheelEvent.railsMode = WebInputEvent::RailsModeFree;

        PlatformWheelEventBuilder platformWheelBuilder(view, webMouseWheelEvent);
        EXPECT_EQ(5, platformWheelBuilder.position().x());
        EXPECT_EQ(0, platformWheelBuilder.position().y());
        EXPECT_EQ(15, platformWheelBuilder.deltaX());
        EXPECT_EQ(10, platformWheelBuilder.deltaY());
        EXPECT_EQ(PlatformEvent::ShiftKey, platformWheelBuilder.modifiers());
        EXPECT_FALSE(platformWheelBuilder.hasPreciseScrollingDeltas());
        EXPECT_FALSE(platformWheelBuilder.canScroll());
        EXPECT_EQ(platformWheelBuilder.railsMode(), PlatformEvent::RailsModeFree);
    }

    {
        WebMouseWheelEvent webMouseWheelEvent;
        webMouseWheelEvent.type = WebInputEvent::MouseWheel;
        webMouseWheelEvent.x = 5;
        webMouseWheelEvent.y = 0;
        webMouseWheelEvent.deltaX = 15;
        webMouseWheelEvent.deltaY = 10;
        webMouseWheelEvent.modifiers = WebInputEvent::AltKey;
        webMouseWheelEvent.hasPreciseScrollingDeltas = true;
        webMouseWheelEvent.canScroll = false;
        webMouseWheelEvent.railsMode = WebInputEvent::RailsModeVertical;

        PlatformWheelEventBuilder platformWheelBuilder(view, webMouseWheelEvent);
        EXPECT_EQ(5, platformWheelBuilder.position().x());
        EXPECT_EQ(0, platformWheelBuilder.position().y());
        EXPECT_EQ(15, platformWheelBuilder.deltaX());
        EXPECT_EQ(10, platformWheelBuilder.deltaY());
        EXPECT_EQ(PlatformEvent::AltKey, platformWheelBuilder.modifiers());
        EXPECT_TRUE(platformWheelBuilder.hasPreciseScrollingDeltas());
        EXPECT_FALSE(platformWheelBuilder.canScroll());
        EXPECT_EQ(platformWheelBuilder.railsMode(), PlatformEvent::RailsModeVertical);
    }
}

} // namespace blink
