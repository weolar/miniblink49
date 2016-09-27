// Copyright (c) 2014 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

// Include this first to avoid type conflicts with CEF headers.
#include "tests/unittests/chromium_includes.h"

#include "tests/unittests/routing_test_handler.h"
#include "tests/cefclient/renderer/client_app_renderer.h"

using client::ClientAppRenderer;

namespace {

void SetRouterConfig(CefMessageRouterConfig& config) {
  config.js_query_function = "testQuery";
  config.js_cancel_function = "testQueryCancel";
}

// Handle the renderer side of the routing implementation.
class RoutingRenderDelegate : public ClientAppRenderer::Delegate {
 public:
  RoutingRenderDelegate() {}

  void OnWebKitInitialized(CefRefPtr<ClientAppRenderer> app) override {
    // Create the renderer-side router for query handling.
    CefMessageRouterConfig config;
    SetRouterConfig(config);
    message_router_ = CefMessageRouterRendererSide::Create(config);
  }

  void OnContextCreated(CefRefPtr<ClientAppRenderer> app,
                        CefRefPtr<CefBrowser> browser,
                        CefRefPtr<CefFrame> frame,
                        CefRefPtr<CefV8Context> context) override {
    message_router_->OnContextCreated(browser,  frame, context);
  }

  void OnContextReleased(CefRefPtr<ClientAppRenderer> app,
                         CefRefPtr<CefBrowser> browser,
                         CefRefPtr<CefFrame> frame,
                         CefRefPtr<CefV8Context> context) override {
    message_router_->OnContextReleased(browser,  frame, context);
  }

  bool OnProcessMessageReceived(
      CefRefPtr<ClientAppRenderer> app,
      CefRefPtr<CefBrowser> browser,
      CefProcessId source_process,
      CefRefPtr<CefProcessMessage> message) override {
    return message_router_->OnProcessMessageReceived(
        browser, source_process, message);
  }

 private:
  CefRefPtr<CefMessageRouterRendererSide> message_router_;

  IMPLEMENT_REFCOUNTING(RoutingRenderDelegate);
};

}  // namespace

RoutingTestHandler::RoutingTestHandler() {
}

void RoutingTestHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser) {
  if (!message_router_.get()) {
    // Create the browser-side router for query handling.
    CefMessageRouterConfig config;
    SetRouterConfig(config);
    message_router_ = CefMessageRouterBrowserSide::Create(config);
    message_router_->AddHandler(this, false);
  }
  TestHandler::OnAfterCreated(browser);
}

void RoutingTestHandler::OnBeforeClose(CefRefPtr<CefBrowser> browser) {
  message_router_->OnBeforeClose(browser);
  TestHandler::OnBeforeClose(browser);
}

void RoutingTestHandler::OnRenderProcessTerminated(
    CefRefPtr<CefBrowser> browser,
    TerminationStatus status) {
  message_router_->OnRenderProcessTerminated(browser);
}

bool RoutingTestHandler::OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
                                        CefRefPtr<CefFrame> frame,
                                        CefRefPtr<CefRequest> request,
                                        bool is_redirect) {
  message_router_->OnBeforeBrowse(browser, frame);
  return false;
}

bool RoutingTestHandler::OnProcessMessageReceived(
    CefRefPtr<CefBrowser> browser,
    CefProcessId source_process,
    CefRefPtr<CefProcessMessage> message) {
  return message_router_->OnProcessMessageReceived(
      browser, source_process,  message);
}

// Entry point for creating the test delegate.
// Called from client_app_delegates.cc.
void CreateRoutingTestHandlerDelegate(
    ClientAppRenderer::DelegateSet& delegates) {
  delegates.insert(new RoutingRenderDelegate);
}
