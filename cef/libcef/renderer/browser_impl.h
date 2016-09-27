// Copyright (c) 2012 The Chromium Embedded Framework Authors.
// Portions copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_LIBCEF_RENDERER_BROWSER_IMPL_H_
#define CEF_LIBCEF_RENDERER_BROWSER_IMPL_H_
#pragma once

#include <map>
#include <string>
#include <vector>

#include "include/cef_browser.h"
#include "include/cef_client.h"
#include "libcef/common/tracker.h"
#include "libcef/renderer/frame_impl.h"

#include "base/memory/scoped_ptr.h"
#include "content/public/renderer/render_view_observer.h"

class GURL;
struct CefMsg_LoadRequest_Params;
struct Cef_Request_Params;
struct Cef_Response_Params;
class CefContentRendererClient;
class CefResponseManager;

namespace base {
class ListValue;
}

// Renderer plumbing for CEF features. There is a one-to-one relationship
// between RenderView on the renderer side and RenderViewHost on the browser
// side.
//
// RenderViewObserver: Interface for observing RenderView notifications and IPC
// messages. IPC messages received by the RenderView will be forwarded to this
// RenderViewObserver implementation. IPC messages sent using
// RenderViewObserver::Send() will be forwarded to the RenderView. Use
// RenderViewObserver::routing_id() when sending IPC messages.
class CefBrowserImpl : public CefBrowser,
                       public content::RenderViewObserver {
 public:
  // Returns the browser associated with the specified RenderView.
  static CefRefPtr<CefBrowserImpl> GetBrowserForView(content::RenderView* view);
  // Returns the browser associated with the specified main WebFrame.
  static CefRefPtr<CefBrowserImpl> GetBrowserForMainFrame(
      blink::WebFrame* frame);

  // CefBrowser methods.
  CefRefPtr<CefBrowserHost> GetHost() override;
  bool CanGoBack() override;
  void GoBack() override;
  bool CanGoForward() override;
  void GoForward() override;
  bool IsLoading() override;
  void Reload() override;
  void ReloadIgnoreCache() override;
  void StopLoad() override;
  int GetIdentifier() override;
  bool IsSame(CefRefPtr<CefBrowser> that) override;
  bool IsPopup() override;
  bool HasDocument() override;
  CefRefPtr<CefFrame> GetMainFrame() override;
  CefRefPtr<CefFrame> GetFocusedFrame() override;
  CefRefPtr<CefFrame> GetFrame(int64 identifier) override;
  CefRefPtr<CefFrame> GetFrame(const CefString& name) override;
  size_t GetFrameCount() override;
  void GetFrameIdentifiers(std::vector<int64>& identifiers) override;
  void GetFrameNames(std::vector<CefString>& names) override;
  bool SendProcessMessage(
      CefProcessId target_process,
      CefRefPtr<CefProcessMessage> message) override;

  CefBrowserImpl(content::RenderView* render_view,
                 int browser_id,
                 bool is_popup,
                 bool is_windowless);
  ~CefBrowserImpl() override;

  void LoadRequest(const CefMsg_LoadRequest_Params& params);

  // Avoids unnecessary string type conversions.
  bool SendProcessMessage(CefProcessId target_process,
                          const std::string& name,
                          base::ListValue* arguments,
                          bool user_initiated);

  // Returns the matching CefFrameImpl reference or creates a new one.
  CefRefPtr<CefFrameImpl> GetWebFrameImpl(blink::WebFrame* frame);
  CefRefPtr<CefFrameImpl> GetWebFrameImpl(int64 frame_id);

  // Frame objects will be deleted immediately before the frame is closed.
  void AddFrameObject(int64 frame_id, CefTrackNode* tracked_object);

  int browser_id() const { return browser_id_; }
  bool is_popup() const { return is_popup_; }
  bool is_windowless() const { return is_windowless_; }
  content::RenderView* render_view() const {
    return content::RenderViewObserver::render_view();
  }

  bool is_swapped_out() const;

 private:
  // RenderViewObserver methods.
  void OnDestruct() override;
  void DidStartLoading() override;
  void DidStopLoading() override;
  void DidFailLoad(blink::WebLocalFrame* frame,
                   const blink::WebURLError& error) override;
  void DidFinishLoad(blink::WebLocalFrame* frame) override;
  void DidStartProvisionalLoad(blink::WebLocalFrame* frame) override;
  void DidFailProvisionalLoad(
      blink::WebLocalFrame* frame,
      const blink::WebURLError& error) override;
  void DidCommitProvisionalLoad(blink::WebLocalFrame* frame,
                                bool is_new_navigation) override;
  void FrameDetached(blink::WebFrame* frame) override;
  void FocusedNodeChanged(const blink::WebNode& node) override;
  void DraggableRegionsChanged(blink::WebFrame* frame) override;
  bool OnMessageReceived(const IPC::Message& message) override;

  // RenderViewObserver::OnMessageReceived message handlers.
  void OnRequest(const Cef_Request_Params& params);
  void OnResponse(const Cef_Response_Params& params);
  void OnResponseAck(int request_id);

  void OnLoadingStateChange(bool isLoading);
  void OnLoadStart(blink::WebLocalFrame* frame);
  void OnLoadEnd(blink::WebLocalFrame* frame);
  void OnLoadError(blink::WebLocalFrame* frame,
                   const blink::WebURLError& error);

  // ID of the browser that this RenderView is associated with. During loading
  // of cross-origin requests multiple RenderViews may be associated with the
  // same browser ID.
  int browser_id_;
  bool is_popup_;
  bool is_windowless_;

  // Map of unique frame ids to CefFrameImpl references.
  typedef std::map<int64, CefRefPtr<CefFrameImpl> > FrameMap;
  FrameMap frames_;

  // Map of unique frame ids to CefTrackManager objects that need to be cleaned
  // up when the frame is deleted.
  typedef std::map<int64, CefRefPtr<CefTrackManager> > FrameObjectMap;
  FrameObjectMap frame_objects_;

  // Manages response registrations.
  scoped_ptr<CefResponseManager> response_manager_;

  IMPLEMENT_REFCOUNTING(CefBrowserImpl);
  DISALLOW_COPY_AND_ASSIGN(CefBrowserImpl);
};

#endif  // CEF_LIBCEF_RENDERER_BROWSER_IMPL_H_
