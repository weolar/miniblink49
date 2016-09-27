/*
* Copyright (C) 2010-2011 weolar Inc. All rights reserved.
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
#include "web/WebDevToolsAgentImpl.h"

namespace blink {

WebDevToolsAgentImpl::WebDevToolsAgentImpl(WebLocalFrameImpl*, WebDevToolsAgentClient*, InspectorOverlay*)
{

}

 WebDevToolsAgentImpl::~WebDevToolsAgentImpl()
{

}

void WebDevToolsAgentImpl::dispose()
{

}

// static
void WebDevToolsAgentImpl::webViewImplClosed(WebViewImpl* webViewImpl)
{
}

// static
void WebDevToolsAgentImpl::webFrameWidgetImplClosed(WebFrameWidgetImpl* webFrameWidgetImpl)
{
}

DEFINE_TRACE(WebDevToolsAgentImpl)
{
}

void WebDevToolsAgentImpl::willBeDestroyed()
{
}

void WebDevToolsAgentImpl::initializeDeferredAgents()
{
}

void WebDevToolsAgentImpl::registerAgent(PassOwnPtrWillBeRawPtr<InspectorAgent> agent)
{
}

void WebDevToolsAgentImpl::attach(const WebString& hostId)
{
}

void WebDevToolsAgentImpl::reattach(const WebString& hostId, const WebString& savedState)
{
}

void WebDevToolsAgentImpl::detach()
{
}

void WebDevToolsAgentImpl::continueProgram()
{
}

bool WebDevToolsAgentImpl::handleInputEvent(const WebInputEvent& inputEvent)
{
    return false;
}

bool WebDevToolsAgentImpl::handleGestureEvent(LocalFrame* frame, const PlatformGestureEvent& event)
{
    return false;
}

bool WebDevToolsAgentImpl::handleMouseEvent(LocalFrame* frame, const PlatformMouseEvent& event)
{
    return false;
}

bool WebDevToolsAgentImpl::handleTouchEvent(LocalFrame* frame, const PlatformTouchEvent& event)
{
    return false;
}

void WebDevToolsAgentImpl::didCommitLoadForLocalFrame(LocalFrame* frame)
{
}

bool WebDevToolsAgentImpl::screencastEnabled()
{
    return false;
}

void WebDevToolsAgentImpl::willAddPageOverlay(const GraphicsLayer* layer)
{
}

void WebDevToolsAgentImpl::didRemovePageOverlay(const GraphicsLayer* layer)
{
}

void WebDevToolsAgentImpl::layerTreeViewChanged(WebLayerTreeView* layerTreeView)
{
}

void WebDevToolsAgentImpl::enableTracing(const String& categoryFilter)
{
}

void WebDevToolsAgentImpl::disableTracing()
{
}

void WebDevToolsAgentImpl::dispatchOnInspectorBackend(const WebString& message)
{
}

void WebDevToolsAgentImpl::dispatchMessageFromFrontend(const String& message)
{
}

void WebDevToolsAgentImpl::inspectElementAt(const WebPoint& pointInRootFrame)
{
}

void WebDevToolsAgentImpl::sendProtocolResponse(int callId, PassRefPtr<JSONObject> message)
{
}

void WebDevToolsAgentImpl::sendProtocolNotification(PassRefPtr<JSONObject> message)
{
}

void WebDevToolsAgentImpl::flush()
{
}

void WebDevToolsAgentImpl::updateInspectorStateCookie(const String& state)
{
}

void WebDevToolsAgentImpl::resumeStartup()
{
}

void WebDevToolsAgentImpl::evaluateInWebInspector(long callId, const WebString& script)
{
}

void WebDevToolsAgentImpl::flushPendingProtocolNotifications()
{
}

void WebDevToolsAgentImpl::willProcessTask()
{
}

void WebDevToolsAgentImpl::didProcessTask()
{
}

void WebDevToolsAgent::interruptAndDispatch(MessageDescriptor* rawDescriptor)
{
}

 bool WebDevToolsAgent::shouldInterruptForMessage(const WebString& message)
{
     return false;
}


} // namespace blink