/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "config.h"
#include "web/ValidationMessageClientImpl.h"

#include "core/dom/Element.h"
#include "core/frame/FrameView.h"
#include "core/layout/LayoutObject.h"
#include "platform/HostWindow.h"
#include "public/platform/WebRect.h"
#include "public/platform/WebString.h"
#include "public/web/WebTextDirection.h"
#include "public/web/WebViewClient.h"
#include "web/WebViewImpl.h"
#include "wtf/CurrentTime.h"

namespace blink {

ValidationMessageClientImpl::ValidationMessageClientImpl(WebViewImpl& webView)
    : m_webView(webView)
    , m_currentAnchor(nullptr)
    , m_lastPageScaleFactor(1)
    , m_finishTime(0)
    , m_timer(this, &ValidationMessageClientImpl::checkAnchorStatus)
{
}

PassOwnPtrWillBeRawPtr<ValidationMessageClientImpl> ValidationMessageClientImpl::create(WebViewImpl& webView)
{
    return adoptPtrWillBeNoop(new ValidationMessageClientImpl(webView));
}

ValidationMessageClientImpl::~ValidationMessageClientImpl()
{
}

FrameView* ValidationMessageClientImpl::currentView()
{
    return m_currentAnchor->document().view();
}

void ValidationMessageClientImpl::showValidationMessage(const Element& anchor, const String& message, TextDirection messageDir, const String& subMessage, TextDirection subMessageDir)
{
    if (message.isEmpty()) {
        hideValidationMessage(anchor);
        return;
    }
    if (!anchor.layoutBox())
        return;
    if (m_currentAnchor)
        hideValidationMessage(*m_currentAnchor);
    m_currentAnchor = &anchor;
    IntRect anchorInViewport = currentView()->contentsToViewport(anchor.pixelSnappedBoundingBox());
    m_lastAnchorRectInScreen = currentView()->hostWindow()->viewportToScreen(anchorInViewport);
    m_lastPageScaleFactor = m_webView.pageScaleFactor();
    m_message = message;
    const double minimumSecondToShowValidationMessage = 5.0;
    const double secondPerCharacter = 0.05;
    const double statusCheckInterval = 0.1;

    m_webView.client()->showValidationMessage(anchorInViewport, m_message, toWebTextDirection(messageDir),
        subMessage, toWebTextDirection(subMessageDir));

    m_finishTime = monotonicallyIncreasingTime() + std::max(minimumSecondToShowValidationMessage, (message.length() + subMessage.length()) * secondPerCharacter);
    // FIXME: We should invoke checkAnchorStatus actively when layout, scroll,
    // or page scale change happen.
    m_timer.startRepeating(statusCheckInterval, FROM_HERE);
}

void ValidationMessageClientImpl::hideValidationMessage(const Element& anchor)
{
    if (!m_currentAnchor || !isValidationMessageVisible(anchor))
        return;
    m_timer.stop();
    m_currentAnchor = nullptr;
    m_message = String();
    m_finishTime = 0;
    m_webView.client()->hideValidationMessage();
}

bool ValidationMessageClientImpl::isValidationMessageVisible(const Element& anchor)
{
    return m_currentAnchor == &anchor;
}

void ValidationMessageClientImpl::documentDetached(const Document& document)
{
    if (m_currentAnchor && m_currentAnchor->document() == document)
        hideValidationMessage(*m_currentAnchor);
}

void ValidationMessageClientImpl::checkAnchorStatus(Timer<ValidationMessageClientImpl>*)
{
    ASSERT(m_currentAnchor);
    if (monotonicallyIncreasingTime() >= m_finishTime || !currentView()) {
        hideValidationMessage(*m_currentAnchor);
        return;
    }

    // Check the visibility of the element.
    // FIXME: Can we check invisibility by scrollable non-frame elements?
    IntRect newAnchorRectInViewport = currentView()->contentsToViewport(m_currentAnchor->pixelSnappedBoundingBox());

    // FIXME: This intersection eliminates the part of the rect outside the root view.
    // If this is meant as a visiblity test, intersecting it against the viewport rect
    // likely makes more sense.
    newAnchorRectInViewport = intersection(currentView()->convertToContainingWindow(currentView()->boundsRect()), newAnchorRectInViewport);
    if (newAnchorRectInViewport.isEmpty()) {
        hideValidationMessage(*m_currentAnchor);
        return;
    }

    IntRect newAnchorRectInViewportInScreen = currentView()->hostWindow()->viewportToScreen(newAnchorRectInViewport);
    if (newAnchorRectInViewportInScreen == m_lastAnchorRectInScreen && m_webView.pageScaleFactor() == m_lastPageScaleFactor)
        return;
    m_lastAnchorRectInScreen = newAnchorRectInViewportInScreen;
    m_lastPageScaleFactor = m_webView.pageScaleFactor();
    m_webView.client()->moveValidationMessage(newAnchorRectInViewport);
}

void ValidationMessageClientImpl::willBeDestroyed()
{
    if (m_currentAnchor)
        hideValidationMessage(*m_currentAnchor);
}

DEFINE_TRACE(ValidationMessageClientImpl)
{
    visitor->trace(m_currentAnchor);
    ValidationMessageClient::trace(visitor);
}

} // namespace blink
