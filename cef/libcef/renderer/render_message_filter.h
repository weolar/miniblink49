// Copyright (c) 2012 The Chromium Embedded Framework Authors.
// Portions copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_LIBCEF_RENDERER_RENDER_MESSAGE_FILTER_H_
#define CEF_LIBCEF_RENDERER_RENDER_MESSAGE_FILTER_H_

#include <string>
#include "ipc/ipc_channel_proxy.h"
#include "ipc/message_filter.h"

// This class sends and receives control messages on the renderer process.
class CefRenderMessageFilter : public IPC::MessageFilter {
 public:
  CefRenderMessageFilter();
  ~CefRenderMessageFilter() override;

  // IPC::ChannelProxy::MessageFilter implementation.
  void OnFilterAdded(IPC::Sender* sender) override;
  void OnFilterRemoved() override;
  bool OnMessageReceived(const IPC::Message& message) override;

 private:
  // Message handlers called on the IO thread.
  void OnDevToolsAgentAttach(const std::string& host_id);
  void OnDevToolsAgentDetach(int32 routing_id);

  void OnDevToolsAgentAttach_RT();
  void OnDevToolsAgentDetach_RT(int32 routing_id);

  IPC::Sender* sender_;

  DISALLOW_COPY_AND_ASSIGN(CefRenderMessageFilter);
};


#endif  // CEF_LIBCEF_RENDERER_RENDER_MESSAGE_FILTER_H_
