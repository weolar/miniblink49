// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/at_exit.h"
#include "base/bind.h"
#include "base/command_line.h"
#include "base/files/file_util.h"
#include "base/i18n/icu_util.h"
#include "base/message_loop/message_loop.h"
#include "gin/array_buffer.h"
#include "gin/modules/console.h"
#include "gin/modules/module_runner_delegate.h"
#include "gin/public/isolate_holder.h"
#include "gin/try_catch.h"
#include "gin/v8_initializer.h"

namespace gin {
namespace {

std::string Load(const base::FilePath& path) {
  std::string source;
  if (!ReadFileToString(path, &source))
    LOG(FATAL) << "Unable to read " << path.LossyDisplayName();
  return source;
}

void Run(base::WeakPtr<Runner> runner, const base::FilePath& path) {
  if (!runner)
    return;
  Runner::Scope scope(runner.get());
  runner->Run(Load(path), path.AsUTF8Unsafe());
}

std::vector<base::FilePath> GetModuleSearchPaths() {
  std::vector<base::FilePath> module_base(1);
  CHECK(base::GetCurrentDirectory(&module_base[0]));
  return module_base;
}

class GinShellRunnerDelegate : public ModuleRunnerDelegate {
 public:
  GinShellRunnerDelegate() : ModuleRunnerDelegate(GetModuleSearchPaths()) {
    AddBuiltinModule(Console::kModuleName, Console::GetModule);
  }

  void UnhandledException(ShellRunner* runner, TryCatch& try_catch) override {
    ModuleRunnerDelegate::UnhandledException(runner, try_catch);
    LOG(ERROR) << try_catch.GetStackTrace();
  }

 private:
  DISALLOW_COPY_AND_ASSIGN(GinShellRunnerDelegate);
};

}  // namespace
}  // namespace gin

int main(int argc, char** argv) {
  base::AtExitManager at_exit;
  base::CommandLine::Init(argc, argv);
  base::i18n::InitializeICU();
#ifdef V8_USE_EXTERNAL_STARTUP_DATA
  gin::V8Initializer::LoadV8Snapshot();
  gin::V8Initializer::LoadV8Natives();
#endif

  base::MessageLoop message_loop;

  gin::IsolateHolder::Initialize(gin::IsolateHolder::kStrictMode,
                                 gin::ArrayBufferAllocator::SharedInstance());
  gin::IsolateHolder instance;


  gin::GinShellRunnerDelegate delegate;
  gin::ShellRunner runner(&delegate, instance.isolate());

  {
    gin::Runner::Scope scope(&runner);
    runner.GetContextHolder()
        ->isolate()
        ->SetCaptureStackTraceForUncaughtExceptions(true);
  }

  base::CommandLine::StringVector args =
      base::CommandLine::ForCurrentProcess()->GetArgs();
  for (base::CommandLine::StringVector::const_iterator it = args.begin();
       it != args.end(); ++it) {
    base::MessageLoop::current()->PostTask(FROM_HERE, base::Bind(
        gin::Run, runner.GetWeakPtr(), base::FilePath(*it)));
  }

  message_loop.RunUntilIdle();
  return 0;
}
