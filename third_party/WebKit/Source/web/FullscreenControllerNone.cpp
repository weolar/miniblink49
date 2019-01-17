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
#include "core/dom/NodeTraversal.h"
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
    
}

void FullscreenController::didExitFullScreen()
{
    m_provisionalFullScreenElement = nullptr;
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

static HTMLVideoElement* findVideoElement(Element* element)
{
    Node* nextNode = element;
    for (; nextNode; nextNode = NodeTraversal::next(*nextNode)) {
        if (!isHTMLVideoElement(nextNode))
            continue;

        HTMLVideoElement* videoElement = toHTMLVideoElement(nextNode);
        
        if (HTMLMediaElement::isMediaStreamURL(videoElement->sourceURL().string()))
            continue;
        break;
    }

    return (HTMLVideoElement*)nextNode;
}

// 新流程和旧的不一样。旧流程会通知WebviewImpl，再回调到didEnterFullScreen，退出全屏也一样。
// 新流程直接通知外部dll，而且退出全屏是外部dll通知回来
void FullscreenController::enterFullScreenForElement(Element* element)
{
    HTMLVideoElement* videoElement = findVideoElement(element);
    if (!videoElement)
        return;
    if (m_provisionalFullScreenElement)
        return;
    m_provisionalFullScreenElement = videoElement;

    Document& document = videoElement->document();
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

    Fullscreen::from(document).didEnterFullScreenForElement(videoElement);
    ASSERT(Fullscreen::currentFullScreenElementFrom(document) == videoElement);

    if (videoElement->webMediaPlayer())
        videoElement->webMediaPlayer()->enterFullscreen();
}

void FullscreenController::exitFullScreenForElement(Element* element)
{
}

void FullscreenController::updateSize()
{
}

void FullscreenController::updatePageScaleConstraints(bool removeConstraints)
{
}

DEFINE_TRACE(FullscreenController)
{

}

} // namespace blink

