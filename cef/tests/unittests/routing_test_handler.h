// Copyright (c) 2014 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

#ifndef CEF_TESTS_UNITTESTS_ROUTING_TEST_HANDLER_H_
#define CEF_TESTS_UNITTESTS_ROUTING_TEST_HANDLER_H_
#pragma once

#include "include/wrapper/cef_message_router.h"
#include "tests/unittests/test_handler.h"

// Extends TestHandler to provide message routing functionality. The
// RoutingTestHandler implementation must be called from subclass
// overrides unless otherwise indicated.
class RoutingTestHandler :
    public TestHandler,
    public CefMessageRouterBrowserSide::Handler {
 public:
  RoutingTestHandler();

  void OnAfterCreated(CefRefPtr<CefBrowser> browser) override;
  void OnBeforeClose(CefRefPtr<CefBrowser> browser) override;
  void OnRenderProcessTerminated(
      CefRefPtr<CefBrowser> browser,
      TerminationStatus status) override;

  // Only call this method if the navigation isn't canceled.
  bool OnBeforeBrowse(CefRefPtr<CefBrowser> browser,
                      CefRefPtr<CefFrame> frame,
                      CefRefPtr<CefRequest> request,
                     bool is_redirect) override;

  // Returns true if the router handled the navigation.
  bool OnProcessMessageReceived(
      CefRefPtr<CefBrowser> browser,
      CefProcessId source_process,
      CefRefPtr<CefProcessMessage> message) override;

 private:
  CefRefPtr<CefMessageRouterBrowserSide> message_router_;
};


#endif  // CEF_TESTS_UNITTESTS_ROUTING_TEST_HANDLER_H_
