// Copyright (c) 2011 The Chromium Embedded Framework Authors. All rights
// reserved. Use of this source code is governed by a BSD-style license that
// can be found in the LICENSE file.

// Include this first to avoid type conflicts with CEF headers.
#include "tests/unittests/chromium_includes.h"

#include "tests/unittests/test_suite.h"

#include "base/command_line.h"
#include "base/logging.h"
#include "base/test/test_suite.h"

#if defined(OS_MACOSX)
#include "base/debug/stack_trace.h"
#include "base/files/file_path.h"
#include "base/i18n/icu_util.h"
#include "base/path_service.h"
#include "base/test/test_timeouts.h"
#endif

#include "tests/cefclient/common/client_switches.h"

base::CommandLine* CefTestSuite::commandline_ = NULL;

CefTestSuite::CefTestSuite(int argc, char** argv)
  : TestSuite(argc, argv) {
}

// static
void CefTestSuite::InitCommandLine(int argc, const char* const* argv) {
  if (commandline_) {
    // If this is intentional, Reset() must be called first. If we are using
    // the shared build mode, we have to share a single object across multiple
    // shared libraries.
    return;
  }

  commandline_ = new base::CommandLine(base::CommandLine::NO_PROGRAM);
#if defined(OS_WIN)
  commandline_->ParseFromString(::GetCommandLineW());
#elif defined(OS_POSIX)
  commandline_->InitFromArgv(argc, argv);
#endif
}

// static
void CefTestSuite::GetSettings(CefSettings& settings) {
#if defined(OS_WIN)
  settings.multi_threaded_message_loop =
      commandline_->HasSwitch(client::switches::kMultiThreadedMessageLoop);
#endif

  CefString(&settings.cache_path) =
      commandline_->GetSwitchValueASCII(client::switches::kCachePath);

  // Always expose the V8 gc() function to give tests finer-grained control over
  // memory management.
  std::string javascript_flags = "--expose-gc";
  // Value of kJavascriptFlags switch.
  std::string other_javascript_flags =
      commandline_->GetSwitchValueASCII("js-flags");
  if (!other_javascript_flags.empty())
    javascript_flags += " " + other_javascript_flags;
  CefString(&settings.javascript_flags) = javascript_flags;

  // Necessary for V8Test.OnUncaughtException tests.
  settings.uncaught_exception_stack_size = 10;

  // Necessary for the OSRTest tests.
  settings.windowless_rendering_enabled = true;

  // For Accept-Language test
  CefString(&settings.accept_language_list) = CEF_SETTINGS_ACCEPT_LANGUAGE;
}

// static
bool CefTestSuite::GetCachePath(std::string& path) {
  DCHECK(commandline_);

  if (commandline_->HasSwitch(client::switches::kCachePath)) {
    // Set the cache_path value.
    path = commandline_->GetSwitchValueASCII(client::switches::kCachePath);
    return true;
  }

  return false;
}

#if defined(OS_MACOSX)
void CefTestSuite::Initialize() {
  // The below code is copied from base/test/test_suite.cc to avoid calling
  // RegisterMockCrApp() on Mac.

  base::FilePath exe;
  PathService::Get(base::FILE_EXE, &exe);
  
  // Initialize logging.
  logging::LoggingSettings log_settings;
  log_settings.log_file =
      exe.ReplaceExtension(FILE_PATH_LITERAL("log")).value().c_str();
  log_settings.logging_dest = logging::LOG_TO_ALL;
  log_settings.lock_log = logging::LOCK_LOG_FILE;
  log_settings.delete_old = logging::DELETE_OLD_LOG_FILE;
  logging::InitLogging(log_settings);

  // We want process and thread IDs because we may have multiple processes.
  // Note: temporarily enabled timestamps in an effort to catch bug 6361.
  logging::SetLogItems(true, true, true, true);

  CHECK(base::debug::EnableInProcessStackDumping());

  // In some cases, we do not want to see standard error dialogs.
  if (!base::debug::BeingDebugged() &&
      !base::CommandLine::ForCurrentProcess()->HasSwitch(
          "show-error-dialogs")) {
    SuppressErrorDialogs();
    base::debug::SetSuppressDebugUI(true);
    logging::SetLogAssertHandler(UnitTestAssertHandler);
  }

  base::i18n::InitializeICU();

  CatchMaybeTests();
  ResetCommandLine();

  TestTimeouts::Initialize();
}
#endif  // defined(OS_MACOSX)
