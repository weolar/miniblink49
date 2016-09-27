// Copyright (c) 2012 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

// Include this first to avoid type conflicts with CEF headers.
#include "tests/unittests/chromium_includes.h"

#if defined(OS_LINUX)
#include <X11/Xlib.h>
// Definitions conflict with gtest.
#undef None
#undef Bool
#endif

#include "base/threading/thread.h"

#include "include/base/cef_bind.h"
#include "include/cef_app.h"
#include "include/cef_task.h"
#include "include/wrapper/cef_helpers.h"
#include "include/wrapper/cef_closure_task.h"
#include "tests/cefclient/browser/client_app_browser.h"
#include "tests/cefclient/common/client_app_other.h"
#include "tests/cefclient/renderer/client_app_renderer.h"
#include "tests/unittests/test_handler.h"
#include "tests/unittests/test_suite.h"

#if defined(OS_WIN)
#include "include/cef_sandbox_win.h"
#endif

namespace {

// Thread used to run the test suite.
class CefTestThread : public base::Thread {
 public:
  explicit CefTestThread(CefTestSuite* test_suite)
    : base::Thread("test_thread"),
      test_suite_(test_suite) {
  }

  void RunTests() {
    // Run the test suite.
    retval_ = test_suite_->Run();

    // Wait for all browsers to exit.
    while (TestHandler::HasBrowser())
      base::PlatformThread::Sleep(base::TimeDelta::FromMilliseconds(100));

    // Quit the CEF message loop.
    CefPostTask(TID_UI, base::Bind(&CefQuitMessageLoop));
  }

  int retval() { return retval_; }

 protected:
  CefTestSuite* test_suite_;
  int retval_;
};

// Called on the UI thread.
void RunTests(CefTestThread* thread) {
  // Run the test suite on the test thread.
  thread->message_loop()->PostTask(FROM_HERE,
      base::Bind(&CefTestThread::RunTests, base::Unretained(thread)));
}

#if defined(OS_LINUX)
int XErrorHandlerImpl(Display *display, XErrorEvent *event) {
  LOG(WARNING)
        << "X error received: "
        << "type " << event->type << ", "
        << "serial " << event->serial << ", "
        << "error_code " << static_cast<int>(event->error_code) << ", "
        << "request_code " << static_cast<int>(event->request_code) << ", "
        << "minor_code " << static_cast<int>(event->minor_code);
  return 0;
}

int XIOErrorHandlerImpl(Display *display) {
  return 0;
}
#endif  // defined(OS_LINUX)

}  // namespace


int main(int argc, char* argv[]) {
#if defined(OS_LINUX)
  // Create a copy of |argv| on Linux because Chromium mangles the value
  // internally (see issue #620).
  CefScopedArgArray scoped_arg_array(argc, argv);
  char** argv_copy = scoped_arg_array.array();
#else
  char** argv_copy = argv;
#endif

#if defined(OS_WIN)
  CefMainArgs main_args(::GetModuleHandle(NULL));
#else
  CefMainArgs main_args(argc, argv);
#endif

  void* windows_sandbox_info = NULL;

#if defined(OS_WIN)
  // Manages the life span of the sandbox information object.
  CefScopedSandboxInfo scoped_sandbox;
  windows_sandbox_info = scoped_sandbox.sandbox_info();
#endif

  // Parse command-line arguments.
  CefRefPtr<CefCommandLine> command_line = CefCommandLine::CreateCommandLine();
#if defined(OS_WIN)
  command_line->InitFromString(::GetCommandLineW());
#else
  command_line->InitFromArgv(argc, argv);
#endif

  // Create a ClientApp of the correct type.
  CefRefPtr<CefApp> app;
  client::ClientApp::ProcessType process_type =
      client::ClientApp::GetProcessType(command_line);
  if (process_type == client::ClientApp::BrowserProcess) {
    app = new client::ClientAppBrowser();
  } else if (process_type == client::ClientApp::RendererProcess ||
             process_type == client::ClientApp::ZygoteProcess) {
    app = new client::ClientAppRenderer();
  } else if (process_type == client::ClientApp::OtherProcess) {
    app = new client::ClientAppOther();
  }

  // Execute the secondary process, if any.
  int exit_code = CefExecuteProcess(main_args, app, windows_sandbox_info);
  if (exit_code >= 0)
    return exit_code;

  // Initialize the CommandLine object.
  CefTestSuite::InitCommandLine(argc, argv_copy);

  CefSettings settings;
  CefTestSuite::GetSettings(settings);

#if defined(OS_MACOSX)
  // Platform-specific initialization.
  extern void PlatformInit();
  PlatformInit();
#endif

#if defined(OS_LINUX)
  // Install xlib error handlers so that the application won't be terminated
  // on non-fatal errors.
  XSetErrorHandler(XErrorHandlerImpl);
  XSetIOErrorHandler(XIOErrorHandlerImpl);
#endif

  // Initialize CEF.
  CefInitialize(main_args, settings, app, windows_sandbox_info);

  // Create the test suite object. TestSuite will modify |argv_copy|.
  CefTestSuite test_suite(argc, argv_copy);

  int retval;

  if (settings.multi_threaded_message_loop) {
    // Run the test suite on the main thread.
    retval = test_suite.Run();
  } else {
    // Create the test thread.
    scoped_ptr<CefTestThread> thread;
    thread.reset(new CefTestThread(&test_suite));
    if (!thread->Start())
      return 1;

    // Start the tests from the UI thread so that any pending UI tasks get a
    // chance to execute first.
    CefPostTask(TID_UI, base::Bind(&RunTests, thread.get()));

    // Run the CEF message loop.
    CefRunMessageLoop();

    // The test suite has completed.
    retval = thread->retval();

    // Terminate the test thread.
    thread.reset();
  }

  // Shut down CEF.
  CefShutdown();

#if defined(OS_MACOSX)
  // Platform-specific cleanup.
  extern void PlatformCleanup();
  PlatformCleanup();
#endif

  return retval;
}
