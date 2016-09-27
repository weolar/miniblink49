// Copyright (c) 2012 The Chromium Embedded Framework Authors.
// Portions copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "libcef/renderer/browser_impl.h"

#include <string>
#include <vector>

#include "libcef/common/cef_messages.h"
#include "libcef/common/content_client.h"
#include "libcef/common/process_message_impl.h"
#include "libcef/common/response_manager.h"
#include "libcef/renderer/content_renderer_client.h"
#include "libcef/renderer/dom_document_impl.h"
#include "libcef/renderer/thread_util.h"
#include "libcef/renderer/webkit_glue.h"

#include "base/strings/string16.h"
#include "base/strings/string_util.h"
#include "base/strings/sys_string_conversions.h"
#include "base/strings/utf_string_conversions.h"
#include "content/public/renderer/document_state.h"
#include "content/public/renderer/navigation_state.h"
#include "content/public/renderer/render_view.h"
#include "content/renderer/navigation_state_impl.h"
#include "content/renderer/render_view_impl.h"
#include "net/http/http_util.h"
#include "third_party/WebKit/public/platform/WebString.h"
#include "third_party/WebKit/public/platform/WebURL.h"
#include "third_party/WebKit/public/platform/WebURLError.h"
#include "third_party/WebKit/public/platform/WebURLResponse.h"
#include "third_party/WebKit/public/web/WebDataSource.h"
#include "third_party/WebKit/public/web/WebDocument.h"
#include "third_party/WebKit/public/web/WebFrame.h"
#include "third_party/WebKit/public/web/WebLocalFrame.h"
#include "third_party/WebKit/public/web/WebNode.h"
#include "third_party/WebKit/public/web/WebScriptSource.h"
#include "third_party/WebKit/public/web/WebSecurityPolicy.h"
#include "third_party/WebKit/public/web/WebView.h"

using blink::WebFrame;
using blink::WebScriptSource;
using blink::WebString;
using blink::WebURL;
using blink::WebView;

namespace {

blink::WebString FilePathStringToWebString(
    const base::FilePath::StringType& str) {
#if defined(OS_POSIX)
  return base::WideToUTF16(base::SysNativeMBToWide(str));
#elif defined(OS_WIN)
  return base::WideToUTF16(str);
#endif
}

}  // namespace


// CefBrowserImpl static methods.
// -----------------------------------------------------------------------------

// static
CefRefPtr<CefBrowserImpl> CefBrowserImpl::GetBrowserForView(
    content::RenderView* view) {
  return CefContentRendererClient::Get()->GetBrowserForView(view);
}

// static
CefRefPtr<CefBrowserImpl> CefBrowserImpl::GetBrowserForMainFrame(
    blink::WebFrame* frame) {
  return CefContentRendererClient::Get()->GetBrowserForMainFrame(frame);
}


// CefBrowser methods.
// -----------------------------------------------------------------------------

CefRefPtr<CefBrowserHost> CefBrowserImpl::GetHost() {
  NOTREACHED() << "GetHost cannot be called from the render process";
  return NULL;
}

bool CefBrowserImpl::CanGoBack() {
  CEF_REQUIRE_RT_RETURN(false);

  return webkit_glue::CanGoBack(render_view()->GetWebView());
}

void CefBrowserImpl::GoBack() {
  CEF_REQUIRE_RT_RETURN_VOID();

  webkit_glue::GoBack(render_view()->GetWebView());
}

bool CefBrowserImpl::CanGoForward() {
  CEF_REQUIRE_RT_RETURN(false);

  return webkit_glue::CanGoForward(render_view()->GetWebView());
}

void CefBrowserImpl::GoForward() {
  CEF_REQUIRE_RT_RETURN_VOID();

  webkit_glue::GoForward(render_view()->GetWebView());
}

bool CefBrowserImpl::IsLoading() {
  CEF_REQUIRE_RT_RETURN(false);

  if (render_view()->GetWebView()) {
    blink::WebFrame* main_frame = render_view()->GetWebView()->mainFrame();
    if (main_frame)
      return main_frame->toWebLocalFrame()->isLoading();
  }
  return false;
}

void CefBrowserImpl::Reload() {
  CEF_REQUIRE_RT_RETURN_VOID();

  if (render_view()->GetWebView() && render_view()->GetWebView()->mainFrame())
    render_view()->GetWebView()->mainFrame()->reload(false);
}

void CefBrowserImpl::ReloadIgnoreCache() {
  CEF_REQUIRE_RT_RETURN_VOID();

  if (render_view()->GetWebView() && render_view()->GetWebView()->mainFrame())
    render_view()->GetWebView()->mainFrame()->reload(true);
}

void CefBrowserImpl::StopLoad() {
  CEF_REQUIRE_RT_RETURN_VOID();

  if (render_view()->GetWebView() && render_view()->GetWebView()->mainFrame())
    render_view()->GetWebView()->mainFrame()->stopLoading();
}

int CefBrowserImpl::GetIdentifier() {
  CEF_REQUIRE_RT_RETURN(0);

  return browser_id();
}

bool CefBrowserImpl::IsSame(CefRefPtr<CefBrowser> that) {
  CEF_REQUIRE_RT_RETURN(false);

  CefBrowserImpl* impl = static_cast<CefBrowserImpl*>(that.get());
  return (impl == this);
}

bool CefBrowserImpl::IsPopup() {
  CEF_REQUIRE_RT_RETURN(false);

  return is_popup();
}

bool CefBrowserImpl::HasDocument() {
  CEF_REQUIRE_RT_RETURN(false);

  if (render_view()->GetWebView() && render_view()->GetWebView()->mainFrame())
    return !render_view()->GetWebView()->mainFrame()->document().isNull();
  return false;
}

CefRefPtr<CefFrame> CefBrowserImpl::GetMainFrame() {
  CEF_REQUIRE_RT_RETURN(NULL);

  if (render_view()->GetWebView() && render_view()->GetWebView()->mainFrame())
    return GetWebFrameImpl(render_view()->GetWebView()->mainFrame()).get();
  return NULL;
}

CefRefPtr<CefFrame> CefBrowserImpl::GetFocusedFrame() {
  CEF_REQUIRE_RT_RETURN(NULL);

  if (render_view()->GetWebView() &&
      render_view()->GetWebView()->focusedFrame()) {
    return GetWebFrameImpl(render_view()->GetWebView()->focusedFrame()).get();
  }
  return NULL;
}

CefRefPtr<CefFrame> CefBrowserImpl::GetFrame(int64 identifier) {
  CEF_REQUIRE_RT_RETURN(NULL);

  return GetWebFrameImpl(identifier).get();
}

CefRefPtr<CefFrame> CefBrowserImpl::GetFrame(const CefString& name) {
  CEF_REQUIRE_RT_RETURN(NULL);

  blink::WebView* web_view = render_view()->GetWebView();
  if (web_view) {
    const blink::WebString& frame_name = name.ToString16();
    // Search by assigned frame name (Frame::name).
    WebFrame* frame = web_view->findFrameByName(frame_name,
                                                web_view->mainFrame());
    if (!frame) {
      // Search by unique frame name (Frame::uniqueName).
      frame = webkit_glue::FindFrameByUniqueName(frame_name,
                                                 web_view->mainFrame());
    }
    if (frame)
      return GetWebFrameImpl(frame).get();
  }

  return NULL;
}

size_t CefBrowserImpl::GetFrameCount() {
  CEF_REQUIRE_RT_RETURN(0);

  int count = 0;

  if (render_view()->GetWebView()) {
    WebFrame* main_frame = render_view()->GetWebView()->mainFrame();
    if (main_frame) {
      WebFrame* cur = main_frame;
      do {
        count++;
        cur = cur->traverseNext(true);
      } while (cur != main_frame);
    }
  }

  return count;
}

void CefBrowserImpl::GetFrameIdentifiers(std::vector<int64>& identifiers) {
  CEF_REQUIRE_RT_RETURN_VOID();

  if (identifiers.size() > 0)
    identifiers.clear();

  if (render_view()->GetWebView()) {
    WebFrame* main_frame = render_view()->GetWebView()->mainFrame();
    if (main_frame) {
      WebFrame* cur = main_frame;
      do {
        identifiers.push_back(webkit_glue::GetIdentifier(cur));
        cur = cur->traverseNext(true);
      } while (cur != main_frame);
    }
  }
}

void CefBrowserImpl::GetFrameNames(std::vector<CefString>& names) {
  CEF_REQUIRE_RT_RETURN_VOID();

  if (names.size() > 0)
    names.clear();

  if (render_view()->GetWebView()) {
    WebFrame* main_frame = render_view()->GetWebView()->mainFrame();
    if (main_frame) {
      WebFrame* cur = main_frame;
      do {
        names.push_back(CefString(cur->uniqueName().utf8()));
        cur = cur->traverseNext(true);
      } while (cur != main_frame);
    }
  }
}

bool CefBrowserImpl::SendProcessMessage(CefProcessId target_process,
                                        CefRefPtr<CefProcessMessage> message) {
  Cef_Request_Params params;
  CefProcessMessageImpl* impl =
      static_cast<CefProcessMessageImpl*>(message.get());
  if (impl->CopyTo(params)) {
    return SendProcessMessage(target_process, params.name, &params.arguments,
                              true);
  }

  return false;
}


// CefBrowserImpl public methods.
// -----------------------------------------------------------------------------

CefBrowserImpl::CefBrowserImpl(content::RenderView* render_view,
                               int browser_id,
                               bool is_popup,
                               bool is_windowless)
    : content::RenderViewObserver(render_view),
      browser_id_(browser_id),
      is_popup_(is_popup),
      is_windowless_(is_windowless) {
  response_manager_.reset(new CefResponseManager);
}

CefBrowserImpl::~CefBrowserImpl() {
}

void CefBrowserImpl::LoadRequest(const CefMsg_LoadRequest_Params& params) {
  CefRefPtr<CefFrameImpl> framePtr = GetWebFrameImpl(params.frame_id);
  if (!framePtr.get())
    return;

  WebFrame* web_frame = framePtr->web_frame();

  blink::WebURLRequest request(params.url);

  if (!params.method.empty())
    request.setHTTPMethod(base::ASCIIToUTF16(params.method));

  if (params.referrer.is_valid()) {
    WebString referrer = blink::WebSecurityPolicy::generateReferrerHeader(
        static_cast<blink::WebReferrerPolicy>(params.referrer_policy),
        params.url,
        WebString::fromUTF8(params.referrer.spec()));
    if (!referrer.isEmpty())
      request.setHTTPHeaderField(WebString::fromUTF8("Referer"), referrer);
  }

  if (params.first_party_for_cookies.is_valid())
    request.setFirstPartyForCookies(params.first_party_for_cookies);

  if (!params.headers.empty()) {
    for (net::HttpUtil::HeadersIterator i(params.headers.begin(),
                                          params.headers.end(), "\n");
         i.GetNext(); ) {
      request.addHTTPHeaderField(WebString::fromUTF8(i.name()),
                                 WebString::fromUTF8(i.values()));
    }
  }

  if (params.upload_data.get()) {
    base::string16 method = request.httpMethod();
    if (method == base::ASCIIToUTF16("GET") ||
        method == base::ASCIIToUTF16("HEAD")) {
      request.setHTTPMethod(base::ASCIIToUTF16("POST"));
    }

    if (request.httpHeaderField(
            base::ASCIIToUTF16("Content-Type")).length() == 0) {
      request.setHTTPHeaderField(
          base::ASCIIToUTF16("Content-Type"),
          base::ASCIIToUTF16("application/x-www-form-urlencoded"));
    }

    blink::WebHTTPBody body;
    body.initialize();

    const ScopedVector<net::UploadElement>& elements =
        params.upload_data->elements();
    ScopedVector<net::UploadElement>::const_iterator it =
        elements.begin();
    for (; it != elements.end(); ++it) {
      const net::UploadElement& element = **it;
      if (element.type() == net::UploadElement::TYPE_BYTES) {
        blink::WebData data;
        data.assign(element.bytes(), element.bytes_length());
        body.appendData(data);
      } else if (element.type() == net::UploadElement::TYPE_FILE) {
        body.appendFile(FilePathStringToWebString(element.file_path().value()));
      } else {
        NOTREACHED();
      }
    }

    request.setHTTPBody(body);
  }

  web_frame->loadRequest(request);
}

bool CefBrowserImpl::SendProcessMessage(CefProcessId target_process,
                                        const std::string& name,
                                        base::ListValue* arguments,
                                        bool user_initiated) {
  DCHECK_EQ(PID_BROWSER, target_process);
  DCHECK(!name.empty());

  Cef_Request_Params params;
  params.name = name;
  if (arguments)
    params.arguments.Swap(arguments);
  params.frame_id = -1;
  params.user_initiated = user_initiated;
  params.request_id = -1;
  params.expect_response = false;

  return Send(new CefHostMsg_Request(routing_id(), params));
}

CefRefPtr<CefFrameImpl> CefBrowserImpl::GetWebFrameImpl(
    blink::WebFrame* frame) {
  DCHECK(frame);
  int64 frame_id = webkit_glue::GetIdentifier(frame);

  // Frames are re-used between page loads. Only add the frame to the map once.
  FrameMap::const_iterator it = frames_.find(frame_id);
  if (it != frames_.end())
    return it->second;

  CefRefPtr<CefFrameImpl> framePtr(new CefFrameImpl(this, frame));
  frames_.insert(std::make_pair(frame_id, framePtr));

  int64 parent_id = frame->parent() == NULL ?
      webkit_glue::kInvalidFrameId :
      webkit_glue::GetIdentifier(frame->parent());
  base::string16 name = frame->uniqueName();

  // Notify the browser that the frame has been identified.
  Send(new CefHostMsg_FrameIdentified(routing_id(), frame_id, parent_id, name));

  return framePtr;
}

CefRefPtr<CefFrameImpl> CefBrowserImpl::GetWebFrameImpl(int64 frame_id) {
  if (frame_id == webkit_glue::kInvalidFrameId) {
    if (render_view()->GetWebView() && render_view()->GetWebView()->mainFrame())
      return GetWebFrameImpl(render_view()->GetWebView()->mainFrame());
    return NULL;
  }

  // Check if we already know about the frame.
  FrameMap::const_iterator it = frames_.find(frame_id);
  if (it != frames_.end())
    return it->second;

  if (render_view()->GetWebView()) {
    // Check if the frame exists but we don't know about it yet.
    WebFrame* main_frame = render_view()->GetWebView()->mainFrame();
    if (main_frame) {
      WebFrame* cur = main_frame;
      do {
        if (webkit_glue::GetIdentifier(cur) == frame_id)
          return GetWebFrameImpl(cur);
        cur = cur->traverseNext(true);
      } while (cur != main_frame);
    }
  }

  return NULL;
}

void CefBrowserImpl::AddFrameObject(int64 frame_id,
                                    CefTrackNode* tracked_object) {
  CefRefPtr<CefTrackManager> manager;

  if (!frame_objects_.empty()) {
    FrameObjectMap::const_iterator it = frame_objects_.find(frame_id);
    if (it != frame_objects_.end())
      manager = it->second;
  }

  if (!manager.get()) {
    manager = new CefTrackManager();
    frame_objects_.insert(std::make_pair(frame_id, manager));
  }

  manager->Add(tracked_object);
}

bool CefBrowserImpl::is_swapped_out() const {
  content::RenderViewImpl* render_view_impl =
      static_cast<content::RenderViewImpl*>(render_view());
  return (!render_view_impl || render_view_impl->is_swapped_out());
}


// RenderViewObserver methods.
// -----------------------------------------------------------------------------

void CefBrowserImpl::OnDestruct() {
  // Notify that the browser window has been destroyed.
  CefRefPtr<CefApp> app = CefContentClient::Get()->application();
  if (app.get()) {
    CefRefPtr<CefRenderProcessHandler> handler =
        app->GetRenderProcessHandler();
    if (handler.get())
      handler->OnBrowserDestroyed(this);
  }

  response_manager_.reset(NULL);

  CefContentRendererClient::Get()->OnBrowserDestroyed(this);
}

void CefBrowserImpl::DidStartLoading() {
  OnLoadingStateChange(true);
}

void CefBrowserImpl::DidStopLoading() {
  OnLoadingStateChange(false);
}

void CefBrowserImpl::DidFailLoad(
    blink::WebLocalFrame* frame,
    const blink::WebURLError& error) {
  OnLoadError(frame, error);
  OnLoadEnd(frame);
}

void CefBrowserImpl::DidFinishLoad(blink::WebLocalFrame* frame) {
  blink::WebDataSource* ds = frame->dataSource();
  Send(new CefHostMsg_DidFinishLoad(routing_id(),
                                    webkit_glue::GetIdentifier(frame),
                                    ds->request().url(),
                                    !frame->parent(),
                                    ds->response().httpStatusCode()));
  OnLoadEnd(frame);
}

void CefBrowserImpl::DidStartProvisionalLoad(blink::WebLocalFrame* frame) {
  // Send the frame creation notification if necessary.
  GetWebFrameImpl(frame);
}

void CefBrowserImpl::DidFailProvisionalLoad(
    blink::WebLocalFrame* frame,
    const blink::WebURLError& error) {
  OnLoadError(frame, error);
}

void CefBrowserImpl::DidCommitProvisionalLoad(blink::WebLocalFrame* frame,
                                              bool is_new_navigation) {
  OnLoadStart(frame);
}

void CefBrowserImpl::FrameDetached(WebFrame* frame) {
  int64 frame_id = webkit_glue::GetIdentifier(frame);

  if (!frames_.empty()) {
    // Remove the frame from the map.
    FrameMap::iterator it = frames_.find(frame_id);
    if (it != frames_.end()) {
      it->second->Detach();
      frames_.erase(it);
    }
  }

  if (!frame_objects_.empty()) {
    // Remove any tracked objects associated with the frame.
    FrameObjectMap::iterator it = frame_objects_.find(frame_id);
    if (it != frame_objects_.end())
      frame_objects_.erase(it);
  }
}

void CefBrowserImpl::FocusedNodeChanged(const blink::WebNode& node) {
  // Notify the handler.
  CefRefPtr<CefApp> app = CefContentClient::Get()->application();
  if (app.get()) {
    CefRefPtr<CefRenderProcessHandler> handler =
        app->GetRenderProcessHandler();
    if (handler.get()) {
      if (node.isNull()) {
        handler->OnFocusedNodeChanged(this, GetFocusedFrame(), NULL);
      } else {
        const blink::WebDocument& document = node.document();
        if (!document.isNull()) {
          blink::WebFrame* frame = document.frame();
          CefRefPtr<CefDOMDocumentImpl> documentImpl =
              new CefDOMDocumentImpl(this, frame);
          handler->OnFocusedNodeChanged(this,
              GetWebFrameImpl(frame).get(),
              documentImpl->GetOrCreateNode(node));
          documentImpl->Detach();
        }
      }
    }
  }
}

void CefBrowserImpl::DraggableRegionsChanged(blink::WebFrame* frame) {
  blink::WebVector<blink::WebDraggableRegion> webregions =
      frame->document().draggableRegions();
  std::vector<Cef_DraggableRegion_Params> regions;
  for (size_t i = 0; i < webregions.size(); ++i) {
    Cef_DraggableRegion_Params region;
    region.bounds = webregions[i].bounds;
    region.draggable = webregions[i].draggable;
    regions.push_back(region);
  }
  Send(new CefHostMsg_UpdateDraggableRegions(routing_id(), regions));
}

bool CefBrowserImpl::OnMessageReceived(const IPC::Message& message) {
  bool handled = true;
  IPC_BEGIN_MESSAGE_MAP(CefBrowserImpl, message)
    IPC_MESSAGE_HANDLER(CefMsg_Request, OnRequest)
    IPC_MESSAGE_HANDLER(CefMsg_Response, OnResponse)
    IPC_MESSAGE_HANDLER(CefMsg_ResponseAck, OnResponseAck)
    IPC_MESSAGE_HANDLER(CefMsg_LoadRequest, LoadRequest)
    IPC_MESSAGE_UNHANDLED(handled = false)
  IPC_END_MESSAGE_MAP()
  return handled;
}


// RenderViewObserver::OnMessageReceived message handlers.
// -----------------------------------------------------------------------------

void CefBrowserImpl::OnRequest(const Cef_Request_Params& params) {
  bool success = false;
  std::string response;
  bool expect_response_ack = false;

  TRACE_EVENT2("libcef", "CefBrowserImpl::OnRequest",
               "request_id", params.request_id,
               "expect_response", params.expect_response ? 1 : 0);

  if (params.user_initiated) {
    // Give the user a chance to handle the request.
    CefRefPtr<CefApp> app = CefContentClient::Get()->application();
    if (app.get()) {
      CefRefPtr<CefRenderProcessHandler> handler =
          app->GetRenderProcessHandler();
      if (handler.get()) {
        CefRefPtr<CefProcessMessageImpl> message(
            new CefProcessMessageImpl(const_cast<Cef_Request_Params*>(&params),
                                      false, true));
        success = handler->OnProcessMessageReceived(this, PID_BROWSER,
                                                    message.get());
        message->Detach(NULL);
      }
    }
  } else if (params.name == "execute-code") {
    // Execute code.
    CefRefPtr<CefFrameImpl> framePtr = GetWebFrameImpl(params.frame_id);
    if (framePtr.get()) {
      WebFrame* web_frame = framePtr->web_frame();
      if (web_frame) {
        DCHECK_EQ(params.arguments.GetSize(), (size_t)4);

        bool is_javascript = false;
        std::string code, script_url;
        int script_start_line = 0;

        params.arguments.GetBoolean(0, &is_javascript);
        params.arguments.GetString(1, &code);
        DCHECK(!code.empty());
        params.arguments.GetString(2, &script_url);
        params.arguments.GetInteger(3, &script_start_line);
        DCHECK_GE(script_start_line, 0);

        if (is_javascript) {
          web_frame->executeScript(
              WebScriptSource(base::UTF8ToUTF16(code),
                              GURL(script_url),
                              script_start_line));
          success = true;
        } else {
          // TODO(cef): implement support for CSS code.
          NOTIMPLEMENTED();
        }
      }
    }
  } else if (params.name == "execute-command") {
    // Execute command.
    CefRefPtr<CefFrameImpl> framePtr = GetWebFrameImpl(params.frame_id);
    if (framePtr.get()) {
      WebFrame* web_frame = framePtr->web_frame();
      if (web_frame) {
        DCHECK_EQ(params.arguments.GetSize(), (size_t)1);

        std::string command;

        params.arguments.GetString(0, &command);
        DCHECK(!command.empty());

        if (base::LowerCaseEqualsASCII(command, "getsource")) {
          response = web_frame->contentAsMarkup().utf8();
          success = true;
        } else if (base::LowerCaseEqualsASCII(command, "gettext")) {
          response = webkit_glue::DumpDocumentText(web_frame);
          success = true;
        } else if (web_frame->executeCommand(base::UTF8ToUTF16(command))) {
          success = true;
        }
      }
    }
  } else if (params.name == "load-string") {
    // Load a string.
    CefRefPtr<CefFrameImpl> framePtr = GetWebFrameImpl(params.frame_id);
    if (framePtr.get()) {
      WebFrame* web_frame = framePtr->web_frame();
      if (web_frame) {
        DCHECK_EQ(params.arguments.GetSize(), (size_t)2);

        std::string string, url;

        params.arguments.GetString(0, &string);
        params.arguments.GetString(1, &url);

        web_frame->loadHTMLString(string, GURL(url));
      }
    }
  } else {
    // Invalid request.
    NOTREACHED();
  }

  if (params.expect_response) {
    DCHECK_GE(params.request_id, 0);

    // Send a response to the browser.
    Cef_Response_Params response_params;
    response_params.request_id = params.request_id;
    response_params.success = success;
    response_params.response = response;
    response_params.expect_response_ack = expect_response_ack;
    Send(new CefHostMsg_Response(routing_id(), response_params));
  }
}

void CefBrowserImpl::OnResponse(const Cef_Response_Params& params) {
  response_manager_->RunHandler(params);
  if (params.expect_response_ack)
    Send(new CefHostMsg_ResponseAck(routing_id(), params.request_id));
}

void CefBrowserImpl::OnResponseAck(int request_id) {
  response_manager_->RunAckHandler(request_id);
}

void CefBrowserImpl::OnLoadingStateChange(bool isLoading) {
  if (is_swapped_out())
    return;

  CefRefPtr<CefApp> app = CefContentClient::Get()->application();
  if (app.get()) {
    CefRefPtr<CefRenderProcessHandler> handler =
        app->GetRenderProcessHandler();
    if (handler.get()) {
      CefRefPtr<CefLoadHandler> load_handler = handler->GetLoadHandler();
      if (load_handler.get()) {
        WebView* web_view = render_view()->GetWebView();
        const bool canGoBack = webkit_glue::CanGoBack(web_view);
        const bool canGoForward = webkit_glue::CanGoForward(web_view);

        load_handler->OnLoadingStateChange(this, isLoading, canGoBack,
                                           canGoForward);
      }
    }
  }
}

void CefBrowserImpl::OnLoadStart(blink::WebLocalFrame* frame) {
  if (is_swapped_out())
    return;

  CefRefPtr<CefApp> app = CefContentClient::Get()->application();
  if (app.get()) {
    CefRefPtr<CefRenderProcessHandler> handler =
        app->GetRenderProcessHandler();
    if (handler.get()) {
      CefRefPtr<CefLoadHandler> load_handler = handler->GetLoadHandler();
      if (load_handler.get()) {
        CefRefPtr<CefFrameImpl> cef_frame = GetWebFrameImpl(frame);
        load_handler->OnLoadStart(this, cef_frame.get());
      }
    }
  }
}

void CefBrowserImpl::OnLoadEnd(blink::WebLocalFrame* frame) {
  if (is_swapped_out())
    return;

  CefRefPtr<CefApp> app = CefContentClient::Get()->application();
  if (app.get()) {
    CefRefPtr<CefRenderProcessHandler> handler =
        app->GetRenderProcessHandler();
    if (handler.get()) {
      CefRefPtr<CefLoadHandler> load_handler = handler->GetLoadHandler();
      if (load_handler.get()) {
        CefRefPtr<CefFrameImpl> cef_frame = GetWebFrameImpl(frame);
        int httpStatusCode = frame->dataSource()->response().httpStatusCode();
        load_handler->OnLoadEnd(this, cef_frame.get(), httpStatusCode);
      }
    }
  }
}

void CefBrowserImpl::OnLoadError(blink::WebLocalFrame* frame,
                                 const blink::WebURLError& error) {
  if (is_swapped_out())
    return;

  CefRefPtr<CefApp> app = CefContentClient::Get()->application();
  if (app.get()) {
    CefRefPtr<CefRenderProcessHandler> handler =
        app->GetRenderProcessHandler();
    if (handler.get()) {
      CefRefPtr<CefLoadHandler> load_handler = handler->GetLoadHandler();
      if (load_handler.get()) {
        CefRefPtr<CefFrameImpl> cef_frame = GetWebFrameImpl(frame);
        const cef_errorcode_t errorCode =
            static_cast<cef_errorcode_t>(error.reason);
        const std::string& errorText = error.localizedDescription.utf8();
        const GURL& failedUrl = error.unreachableURL;
        load_handler->OnLoadError(this, cef_frame.get(), errorCode, errorText,
                                  failedUrl.spec());
      }
    }
  }
}
