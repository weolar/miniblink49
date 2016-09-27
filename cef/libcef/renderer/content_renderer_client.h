// Copyright (c) 2013 The Chromium Embedded Framework Authors.
// Portions copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CEF_LIBCEF_RENDERER_CONTENT_RENDERER_CLIENT_H_
#define CEF_LIBCEF_RENDERER_CONTENT_RENDERER_CLIENT_H_
#pragma once

#include <list>
#include <map>
#include <string>
#include <vector>

#include "libcef/renderer/browser_impl.h"

#include "base/compiler_specific.h"
#include "base/memory/scoped_ptr.h"
#include "base/message_loop/message_loop.h"
#include "base/sequenced_task_runner.h"
#include "content/public/renderer/content_renderer_client.h"

namespace extensions {
class Dispatcher;
class DispatcherDelegate;
class ExtensionsClient;
class ExtensionsGuestViewContainerDispatcher;
class ExtensionsRendererClient;
class ResourceRequestPolicy;
}

namespace web_cache {
class WebCacheRenderProcessObserver;
}

class CefRenderProcessObserver;
struct Cef_CrossOriginWhiteListEntry_Params;
struct CefViewHostMsg_GetPluginInfo_Output;
class ChromePDFPrintClient;
class SpellCheck;

class CefContentRendererClient : public content::ContentRendererClient,
                                 public base::MessageLoop::DestructionObserver {
 public:
  CefContentRendererClient();
  ~CefContentRendererClient() override;

  // Returns the singleton CefContentRendererClient instance.
  static CefContentRendererClient* Get();

  // Returns the browser associated with the specified RenderView.
  CefRefPtr<CefBrowserImpl> GetBrowserForView(content::RenderView* view);

  // Returns the browser associated with the specified main WebFrame.
  CefRefPtr<CefBrowserImpl> GetBrowserForMainFrame(blink::WebFrame* frame);

  // Called from CefBrowserImpl::OnDestruct().
  void OnBrowserDestroyed(CefBrowserImpl* browser);

  // Render thread task runner.
  base::SequencedTaskRunner* render_task_runner() const {
    return render_task_runner_.get();
  }

  int uncaught_exception_stack_size() const {
    return uncaught_exception_stack_size_;
  }

  void WebKitInitialized();
  void OnRenderProcessShutdown();

  void DevToolsAgentAttached();
  void DevToolsAgentDetached();

  // Returns the task runner for the current thread. Returns NULL if the current
  // thread is not the main render process thread.
  scoped_refptr<base::SequencedTaskRunner> GetCurrentTaskRunner();

  // Perform cleanup work that needs to occur before shutdown when running in
  // single-process mode. Blocks until cleanup is complete.
  void RunSingleProcessCleanup();

  // ContentRendererClient implementation.
  void RenderThreadStarted() override;
  void RenderFrameCreated(content::RenderFrame* render_frame) override;
  void RenderViewCreated(content::RenderView* render_view) override;
  bool OverrideCreatePlugin(
      content::RenderFrame* render_frame,
      blink::WebLocalFrame* frame,
      const blink::WebPluginParams& params,
      blink::WebPlugin** plugin) override;
  bool HandleNavigation(content::RenderFrame* render_frame,
                        content::DocumentState* document_state,
                        int opener_id,
                        blink::WebFrame* frame,
                        const blink::WebURLRequest& request,
                        blink::WebNavigationType type,
                        blink::WebNavigationPolicy default_policy,
                        bool is_redirect) override;
  bool ShouldFork(blink::WebLocalFrame* frame,
                  const GURL& url,
                  const std::string& http_method,
                  bool is_initial_navigation,
                  bool is_server_redirect,
                  bool* send_referrer) override;
  bool WillSendRequest(blink::WebFrame* frame,
                       ui::PageTransition transition_type,
                       const GURL& url,
                       const GURL& first_party_for_cookies,
                       GURL* new_url) override;
  content::BrowserPluginDelegate* CreateBrowserPluginDelegate(
      content::RenderFrame* render_frame,
      const std::string& mime_type,
      const GURL& original_url) override;
  void AddKeySystems(std::vector<media::KeySystemInfo>* key_systems) override;

  // MessageLoop::DestructionObserver implementation.
  void WillDestroyCurrentMessageLoop() override;

  static bool IsExtensionOrSharedModuleWhitelisted(
      const GURL& url, const std::set<std::string>& whitelist);

  static blink::WebPlugin* CreatePlugin(
      content::RenderFrame* render_frame,
      blink::WebLocalFrame* frame,
      const blink::WebPluginParams& params,
      const CefViewHostMsg_GetPluginInfo_Output& output);

 private:
  void BrowserCreated(content::RenderView* render_view,
                      content::RenderFrame* render_frame);

  // Perform cleanup work for single-process mode.
  void RunSingleProcessCleanupOnUIThread();

  scoped_refptr<base::SequencedTaskRunner> render_task_runner_;
  scoped_ptr<CefRenderProcessObserver> observer_;
  scoped_ptr<web_cache::WebCacheRenderProcessObserver> web_cache_observer_;
  scoped_ptr<SpellCheck> spellcheck_;

  // Map of RenderView pointers to CefBrowserImpl references.
  typedef std::map<content::RenderView*, CefRefPtr<CefBrowserImpl> > BrowserMap;
  BrowserMap browsers_;

  // Cross-origin white list entries that need to be registered with WebKit.
  typedef std::vector<Cef_CrossOriginWhiteListEntry_Params> CrossOriginList;
  CrossOriginList cross_origin_whitelist_entries_;

  scoped_ptr<ChromePDFPrintClient> pdf_print_client_;

  scoped_ptr<extensions::ExtensionsClient> extensions_client_;
  scoped_ptr<extensions::ExtensionsRendererClient> extensions_renderer_client_;
  scoped_ptr<extensions::DispatcherDelegate> extension_dispatcher_delegate_;
  scoped_ptr<extensions::Dispatcher> extension_dispatcher_;
  scoped_ptr<extensions::ExtensionsGuestViewContainerDispatcher>
      guest_view_container_dispatcher_;
  scoped_ptr<extensions::ResourceRequestPolicy> resource_request_policy_;

  int devtools_agent_count_;
  int uncaught_exception_stack_size_;

  // Used in single-process mode to test when cleanup is complete.
  // Access must be protected by |single_process_cleanup_lock_|.
  bool single_process_cleanup_complete_;
  base::Lock single_process_cleanup_lock_;
};

#endif  // CEF_LIBCEF_RENDERER_CONTENT_RENDERER_CLIENT_H_
