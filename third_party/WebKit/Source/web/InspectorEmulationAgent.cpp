// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "web/InspectorEmulationAgent.h"

#include "core/frame/FrameHost.h"
#include "core/frame/FrameView.h"
#include "core/frame/Settings.h"
#include "core/inspector/InspectorState.h"
#include "core/page/Page.h"
#include "platform/geometry/DoubleRect.h"
#include "web/DevToolsEmulator.h"
#include "web/WebLocalFrameImpl.h"
#include "web/WebViewImpl.h"

namespace blink {

namespace EmulationAgentState {
static const char scriptExecutionDisabled[] = "scriptExecutionDisabled";
static const char touchEventEmulationEnabled[] = "touchEventEmulationEnabled";
static const char emulatedMedia[] = "emulatedMedia";
}

PassOwnPtrWillBeRawPtr<InspectorEmulationAgent> InspectorEmulationAgent::create(WebViewImpl* webViewImpl)
{
    return adoptPtrWillBeNoop(new InspectorEmulationAgent(webViewImpl));
}

InspectorEmulationAgent::InspectorEmulationAgent(WebViewImpl* webViewImpl)
    : InspectorBaseAgent<InspectorEmulationAgent, InspectorFrontend::Emulation>("Emulation")
    , m_webViewImpl(webViewImpl)
{
    m_webViewImpl->devToolsEmulator()->setEmulationAgent(this);
}

InspectorEmulationAgent::~InspectorEmulationAgent()
{
}

void InspectorEmulationAgent::restore()
{
    ErrorString error;
    setScriptExecutionDisabled(&error, m_state->getBoolean(EmulationAgentState::scriptExecutionDisabled));
    setTouchEmulationEnabled(&error, m_state->getBoolean(EmulationAgentState::touchEventEmulationEnabled), nullptr);
    setEmulatedMedia(&error, m_state->getString(EmulationAgentState::emulatedMedia));
}

void InspectorEmulationAgent::disable(ErrorString*)
{
    ErrorString error;
    setScriptExecutionDisabled(&error, false);
    setTouchEmulationEnabled(&error, false, nullptr);
    setEmulatedMedia(&error, String());
}

void InspectorEmulationAgent::discardAgent()
{
    m_webViewImpl->devToolsEmulator()->setEmulationAgent(nullptr);
}

void InspectorEmulationAgent::didCommitLoadForLocalFrame(LocalFrame* frame)
{
    if (frame == m_webViewImpl->mainFrameImpl()->frame())
        viewportChanged();
}

void InspectorEmulationAgent::resetScrollAndPageScaleFactor(ErrorString*)
{
    m_webViewImpl->resetScrollAndScaleState();
}

void InspectorEmulationAgent::setPageScaleFactor(ErrorString*, double pageScaleFactor)
{
    m_webViewImpl->setPageScaleFactor(static_cast<float>(pageScaleFactor));
}

void InspectorEmulationAgent::setScriptExecutionDisabled(ErrorString*, bool value)
{
    m_state->setBoolean(EmulationAgentState::scriptExecutionDisabled, value);
    m_webViewImpl->devToolsEmulator()->setScriptExecutionDisabled(value);
}

void InspectorEmulationAgent::setTouchEmulationEnabled(ErrorString*, bool enabled, const String* configuration)
{
    m_state->setBoolean(EmulationAgentState::touchEventEmulationEnabled, enabled);
    m_webViewImpl->devToolsEmulator()->setTouchEventEmulationEnabled(enabled);
}

void InspectorEmulationAgent::setEmulatedMedia(ErrorString*, const String& media)
{
    m_state->setString(EmulationAgentState::emulatedMedia, media);
    m_webViewImpl->page()->settings().setMediaTypeOverride(media);
}

void InspectorEmulationAgent::viewportChanged()
{
    if (!m_webViewImpl->devToolsEmulator()->deviceEmulationEnabled() || !frontend())
        return;

    FrameView* view = m_webViewImpl->mainFrameImpl()->frameView();
    IntSize contentsSize = view->contentsSize();
    FloatPoint scrollOffset;
    scrollOffset = FloatPoint(view->scrollableArea()->visibleContentRectDouble().location());

    RefPtr<TypeBuilder::Emulation::Viewport> viewport = TypeBuilder::Emulation::Viewport::create()
        .setScrollX(scrollOffset.x())
        .setScrollY(scrollOffset.y())
        .setContentsWidth(contentsSize.width())
        .setContentsHeight(contentsSize.height())
        .setPageScaleFactor(m_webViewImpl->page()->pageScaleFactor())
        .setMinimumPageScaleFactor(m_webViewImpl->minimumPageScaleFactor())
        .setMaximumPageScaleFactor(m_webViewImpl->maximumPageScaleFactor());
    frontend()->viewportChanged(viewport);
}

DEFINE_TRACE(InspectorEmulationAgent)
{
    InspectorBaseAgent::trace(visitor);
}

} // namespace blink
