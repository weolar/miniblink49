// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "web/InspectorRenderingAgent.h"

#include "core/frame/FrameView.h"
#include "core/frame/Settings.h"
#include "core/inspector/InspectorState.h"
#include "core/page/Page.h"
#include "web/WebLocalFrameImpl.h"
#include "web/WebViewImpl.h"

namespace blink {

namespace RenderingAgentState {
static const char continuousPaintingEnabled[] = "continuousPaintingEnabled";
static const char showDebugBorders[] = "showDebugBorders";
static const char showFPSCounter[] = "showFPSCounter";
static const char showPaintRects[] = "showPaintRects";
static const char showScrollBottleneckRects[] = "showScrollBottleneckRects";
}

PassOwnPtrWillBeRawPtr<InspectorRenderingAgent> InspectorRenderingAgent::create(WebViewImpl* webViewImpl)
{
    return adoptPtrWillBeNoop(new InspectorRenderingAgent(webViewImpl));
}

InspectorRenderingAgent::InspectorRenderingAgent(WebViewImpl* webViewImpl)
    : InspectorBaseAgent<InspectorRenderingAgent, InspectorFrontend::Rendering>("Rendering")
    , m_webViewImpl(webViewImpl)
{
}

void InspectorRenderingAgent::restore()
{
    ErrorString error;
    setContinuousPaintingEnabled(&error, m_state->getBoolean(RenderingAgentState::continuousPaintingEnabled));
    setShowDebugBorders(&error, m_state->getBoolean(RenderingAgentState::showDebugBorders));
    setShowFPSCounter(&error, m_state->getBoolean(RenderingAgentState::showFPSCounter));
    setShowPaintRects(&error, m_state->getBoolean(RenderingAgentState::showPaintRects));
    setShowScrollBottleneckRects(&error, m_state->getBoolean(RenderingAgentState::showScrollBottleneckRects));
}

void InspectorRenderingAgent::disable(ErrorString*)
{
    ErrorString error;
    if (m_state->getBoolean(RenderingAgentState::continuousPaintingEnabled))
        setContinuousPaintingEnabled(&error, false);
    setShowDebugBorders(&error, false);
    setShowFPSCounter(&error, false);
    setShowPaintRects(&error, false);
    setShowScrollBottleneckRects(&error, false);
}

void InspectorRenderingAgent::setContinuousPaintingEnabled(ErrorString* errorString, bool enabled)
{
    m_state->setBoolean(RenderingAgentState::continuousPaintingEnabled, enabled);
    if (enabled && !compositingEnabled(errorString))
        return;
    m_webViewImpl->setContinuousPaintingEnabled(enabled);
}

void InspectorRenderingAgent::setShowDebugBorders(ErrorString* errorString, bool show)
{
    m_state->setBoolean(RenderingAgentState::showDebugBorders, show);
    if (show && !compositingEnabled(errorString))
        return;
    m_webViewImpl->setShowDebugBorders(show);
}

void InspectorRenderingAgent::setShowFPSCounter(ErrorString* errorString, bool show)
{
    m_state->setBoolean(RenderingAgentState::showFPSCounter, show);
    if (show && !compositingEnabled(errorString))
        return;
    m_webViewImpl->setShowFPSCounter(show);
}

void InspectorRenderingAgent::setShowPaintRects(ErrorString*, bool show)
{
    m_state->setBoolean(RenderingAgentState::showPaintRects, show);
    m_webViewImpl->setShowPaintRects(show);
    if (!show && m_webViewImpl->mainFrameImpl()->frameView())
        m_webViewImpl->mainFrameImpl()->frameView()->invalidate();
}

void InspectorRenderingAgent::setShowScrollBottleneckRects(ErrorString* errorString, bool show)
{
    m_state->setBoolean(RenderingAgentState::showScrollBottleneckRects, show);
    if (show && !compositingEnabled(errorString))
        return;
    m_webViewImpl->setShowScrollBottleneckRects(show);
}

bool InspectorRenderingAgent::compositingEnabled(ErrorString* errorString)
{
    if (!m_webViewImpl->page()->settings().acceleratedCompositingEnabled()) {
        if (errorString)
            *errorString = "Compositing mode is not supported";
        return false;
    }
    return true;
}

DEFINE_TRACE(InspectorRenderingAgent)
{
    InspectorBaseAgent::trace(visitor);
}

} // namespace blink
