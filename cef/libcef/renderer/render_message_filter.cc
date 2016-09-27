/// Copyright (c) 2012 The Chromium Embedded Framework Authors.
// Portions (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libcef/renderer/render_message_filter.h"
#include "libcef/renderer/thread_util.h"
#include "libcef/common/cef_messages.h"

#include "base/bind.h"
#include "base/message_loop/message_loop.h"
#include "content/common/devtools_messages.h"
#include "content/renderer/devtools/devtools_agent.h"
#include "third_party/WebKit/public/platform/WebString.h"
#include "third_party/WebKit/public/web/WebSecurityPolicy.h"
#include "url/gurl.h"
#include "url/url_util.h"

CefRenderMessageFilter::CefRenderMessageFilter()
    : sender_(NULL) {
}

CefRenderMessageFilter::~CefRenderMessageFilter() {
}

void CefRenderMessageFilter::OnFilterAdded(IPC::Sender* sender) {
  sender_ = sender;
}

void CefRenderMessageFilter::OnFilterRemoved() {
}

bool CefRenderMessageFilter::OnMessageReceived(const IPC::Message& message) {
  bool handled = true;

  // Observe the DevTools messages but don't handle them.
  if (message.type() == DevToolsAgentMsg_Attach::ID) {
    handled = false;
  } else if (message.type() == DevToolsAgentMsg_Detach::ID) {
    OnDevToolsAgentDetach(message.routing_id());
    return false;
  }

  IPC_BEGIN_MESSAGE_MAP(CefRenderMessageFilter, message)
    IPC_MESSAGE_HANDLER(DevToolsAgentMsg_Attach, OnDevToolsAgentAttach)
    IPC_MESSAGE_UNHANDLED(handled = false)
  IPC_END_MESSAGE_MAP()
  return handled;
}

void CefRenderMessageFilter::OnDevToolsAgentAttach(
    const std::string& host_id) {
  CEF_POST_TASK_RT(
      base::Bind(&CefRenderMessageFilter::OnDevToolsAgentAttach_RT, this));
}

void CefRenderMessageFilter::OnDevToolsAgentDetach(int32 routing_id) {
  CEF_POST_TASK_RT(
      base::Bind(&CefRenderMessageFilter::OnDevToolsAgentDetach_RT, this,
                 routing_id));
}

void CefRenderMessageFilter::OnDevToolsAgentAttach_RT() {
  CEF_REQUIRE_RT();
  CefContentRendererClient::Get()->DevToolsAgentAttached();
}

void CefRenderMessageFilter::OnDevToolsAgentDetach_RT(int32 routing_id) {
  CEF_REQUIRE_RT();

  // Wait for the DevToolsAgent to detach. It would be better to receive
  // notification when the DevToolsAgent detaches but that's not currently
  // available.
  content::DevToolsAgent* agent =
      content::DevToolsAgent::FromRoutingId(routing_id);
  if (agent && agent->IsAttached()) {
    // Try again in a bit.
    CEF_POST_DELAYED_TASK_RT(
        base::Bind(&CefRenderMessageFilter::OnDevToolsAgentDetach_RT, this,
                   routing_id), 50);
    return;
  }

  CefContentRendererClient::Get()->DevToolsAgentDetached();
}
