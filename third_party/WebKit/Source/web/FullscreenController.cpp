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
#include "web/FullscreenController.h"

#include "core/dom/Document.h"
#include "core/dom/Fullscreen.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/PageScaleConstraintsSet.h"
#include "core/html/HTMLMediaElement.h"
#include "core/html/HTMLVideoElement.h"
#include "platform/LayoutTestSupport.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "public/platform/WebLayerTreeView.h"
#include "public/web/WebFrameClient.h"
#include "web/WebLocalFrameImpl.h"
#include "web/WebSettingsImpl.h"
#include "web/WebViewImpl.h"

namespace blink {

PassOwnPtrWillBeRawPtr<FullscreenController> FullscreenController::create(WebViewImpl* webViewImpl)
{
    return adoptPtrWillBeNoop(new FullscreenController(webViewImpl));
}

FullscreenController::FullscreenController(WebViewImpl* webViewImpl)
    : m_webViewImpl(webViewImpl)
    , m_exitFullscreenPageScaleFactor(0)
    , m_isCancelingFullScreen(false)
{
}

void FullscreenController::didEnterFullScreen()
{
    if (!m_provisionalFullScreenElement)
        return;

    RefPtrWillBeRawPtr<Element> element = m_provisionalFullScreenElement.release();
    Document& document = element->document();
    m_fullScreenFrame = document.frame();

    if (!m_fullScreenFrame)
        return;

    if (!m_exitFullscreenPageScaleFactor) {
        m_exitFullscreenPageScaleFactor = m_webViewImpl->pageScaleFactor();
        m_exitFullscreenScrollOffset = m_webViewImpl->mainFrame()->scrollOffset();
        m_exitFullscreenPinchViewportOffset = m_webViewImpl->pinchViewportOffset();

        updatePageScaleConstraints(false);
        m_webViewImpl->setPageScaleFactor(1.0f);
        m_webViewImpl->mainFrame()->setScrollOffset(WebSize());
        m_webViewImpl->setPinchViewportOffset(FloatPoint());
    }

    Fullscreen::from(document).didEnterFullScreenForElement(element.get());
    ASSERT(Fullscreen::currentFullScreenElementFrom(document) == element);

    if (RuntimeEnabledFeatures::overlayFullscreenVideoEnabled()) {
        if (isHTMLVideoElement(element)) {
            HTMLVideoElement* videoElement = toHTMLVideoElement(element);
            if (HTMLMediaElement::isMediaStreamURL(videoElement->sourceURL().string()))
                return;
            if (videoElement->webMediaPlayer()
                // FIXME: There is no embedder-side handling in layout test mode.
                && !LayoutTestSupport::isRunningLayoutTest()) {
                videoElement->webMediaPlayer()->enterFullscreen();
            }
            if (m_webViewImpl->layerTreeView())
                m_webViewImpl->layerTreeView()->setHasTransparentBackground(true);
        }
    }
}

void FullscreenController::didExitFullScreen()
{
    if (!m_fullScreenFrame)
        return;

    if (Document* document = m_fullScreenFrame->document()) {
        if (Fullscreen* fullscreen = Fullscreen::fromIfExists(*document)) {
            if (fullscreen->webkitCurrentFullScreenElement()) {
                // When the client exits from full screen we have to call fullyExitFullscreen to notify
                // the document. While doing that, suppress notifications back to the client.
                m_isCancelingFullScreen = true;
                Fullscreen::fullyExitFullscreen(*document);
                m_isCancelingFullScreen = false;

                if (RuntimeEnabledFeatures::overlayFullscreenVideoEnabled() && m_webViewImpl->layerTreeView())
                    m_webViewImpl->layerTreeView()->setHasTransparentBackground(m_webViewImpl->isTransparent());

                if (m_exitFullscreenPageScaleFactor) {
                    updatePageScaleConstraints(true);
                    m_webViewImpl->setPageScaleFactor(m_exitFullscreenPageScaleFactor);
                    m_webViewImpl->mainFrame()->setScrollOffset(WebSize(m_exitFullscreenScrollOffset));
                    m_webViewImpl->setPinchViewportOffset(m_exitFullscreenPinchViewportOffset);
                    m_exitFullscreenPageScaleFactor = 0;
                    m_exitFullscreenScrollOffset = IntSize();
                }

                fullscreen->didExitFullScreenForElement(0);
            }
        }
    }

    m_fullScreenFrame.clear();
}

void FullscreenController::enterFullScreenForElement(Element* element)
{
    // We are already transitioning to fullscreen for a different element.
    if (m_provisionalFullScreenElement) {
        m_provisionalFullScreenElement = element;
        return;
    }

    // We are already in fullscreen mode.
    if (m_fullScreenFrame) {
        m_provisionalFullScreenElement = element;
        didEnterFullScreen();
        return;
    }

    // We need to transition to fullscreen mode.
    WebLocalFrameImpl* frame = WebLocalFrameImpl::fromFrame(element->document().frame());
    if (frame && frame->client()) {
        frame->client()->enterFullscreen();
        m_provisionalFullScreenElement = element;
    }
}

void FullscreenController::exitFullScreenForElement(Element* element)
{
    ASSERT(element);

    // The client is exiting full screen, so don't send a notification.
    if (m_isCancelingFullScreen)
        return;

    WebLocalFrameImpl* frame = WebLocalFrameImpl::fromFrame(element->document().frame());
    if (frame && frame->client())
        frame->client()->exitFullscreen();
}

void FullscreenController::updateSize()
{
    if (!isFullscreen())
        return;

    updatePageScaleConstraints(false);

    LayoutFullScreen* layoutObject = Fullscreen::from(*m_fullScreenFrame->document()).fullScreenLayoutObject();
    if (layoutObject)
        layoutObject->updateStyle();
}

void FullscreenController::updatePageScaleConstraints(bool removeConstraints)
{
    PageScaleConstraints fullscreenConstraints;
    if (!removeConstraints) {
        fullscreenConstraints = PageScaleConstraints(1.0, 1.0, 1.0);
        fullscreenConstraints.layoutSize = IntSize(m_webViewImpl->size());
    }
    m_webViewImpl->pageScaleConstraintsSet().setFullscreenConstraints(fullscreenConstraints);
    m_webViewImpl->pageScaleConstraintsSet().computeFinalConstraints();
    m_webViewImpl->updateMainFrameLayoutSize();
}

DEFINE_TRACE(FullscreenController)
{
    visitor->trace(m_provisionalFullScreenElement);
    visitor->trace(m_fullScreenFrame);
}

} // namespace blink

