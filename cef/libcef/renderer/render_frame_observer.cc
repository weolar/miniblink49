// Copyright 2014 The Chromium Embedded Framework Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be found
// in the LICENSE file.


#include "base/compiler_specific.h"

#include "config.h"
MSVC_PUSH_WARNING_LEVEL(0);
#include "bindings/core/v8/V8RecursionScope.h"
MSVC_POP_WARNING();
#undef FROM_HERE
#undef LOG

#include "libcef/renderer/render_frame_observer.h"

#include "libcef/common/content_client.h"
#include "libcef/renderer/content_renderer_client.h"
#include "libcef/renderer/v8_impl.h"

#include "content/public/renderer/render_frame.h"
#include "third_party/WebKit/public/web/WebLocalFrame.h"
#include "third_party/WebKit/public/web/WebKit.h"

CefRenderFrameObserver::CefRenderFrameObserver(
    content::RenderFrame* render_frame)
    : content::RenderFrameObserver(render_frame) {
}

CefRenderFrameObserver::~CefRenderFrameObserver() {
}

void CefRenderFrameObserver::DidCreateScriptContext(
    v8::Handle<v8::Context> context,
    int extension_group,
    int world_id) {
  blink::WebLocalFrame* frame = render_frame()->GetWebFrame();

  CefRefPtr<CefBrowserImpl> browserPtr =
      CefBrowserImpl::GetBrowserForMainFrame(frame->top());
  if (!browserPtr.get())
    return;

  CefRefPtr<CefRenderProcessHandler> handler;
  CefRefPtr<CefApp> application = CefContentClient::Get()->application();
  if (application.get())
    handler = application->GetRenderProcessHandler();
  if (!handler.get())
    return;

  CefRefPtr<CefFrameImpl> framePtr = browserPtr->GetWebFrameImpl(frame);

  v8::Isolate* isolate = blink::mainThreadIsolate();
  v8::HandleScope handle_scope(isolate);
  v8::Context::Scope scope(context);
  blink::V8RecursionScope recursion_scope(isolate);

  CefRefPtr<CefV8Context> contextPtr(new CefV8ContextImpl(isolate, context));

  handler->OnContextCreated(browserPtr.get(), framePtr.get(), contextPtr);
}

void CefRenderFrameObserver::WillReleaseScriptContext(
    v8::Handle<v8::Context> context,
    int world_id) {
  blink::WebLocalFrame* frame = render_frame()->GetWebFrame();

  CefRefPtr<CefBrowserImpl> browserPtr =
      CefBrowserImpl::GetBrowserForMainFrame(frame->top());
  if (browserPtr.get()) {
    CefRefPtr<CefApp> application = CefContentClient::Get()->application();
    if (application.get()) {
      CefRefPtr<CefRenderProcessHandler> handler =
          application->GetRenderProcessHandler();
      if (handler.get()) {
        CefRefPtr<CefFrameImpl> framePtr = browserPtr->GetWebFrameImpl(frame);

        v8::Isolate* isolate = blink::mainThreadIsolate();
        v8::HandleScope handle_scope(isolate);
        v8::Context::Scope scope(context);
        blink::V8RecursionScope recursion_scope(isolate);

        CefRefPtr<CefV8Context> contextPtr(
            new CefV8ContextImpl(isolate, context));

        handler->OnContextReleased(browserPtr.get(), framePtr.get(),
                                   contextPtr);
      }
    }
  }

  CefV8ReleaseContext(context);
}
