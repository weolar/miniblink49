// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GIN_TEST_FILE_RUNNER_H_
#define GIN_TEST_FILE_RUNNER_H_

#include "base/compiler_specific.h"
#include "base/files/file_path.h"
#include "gin/modules/module_runner_delegate.h"
#include "gin/runner.h"

namespace gin {

// FileRunnerDelegate is a simple RunnerDelegate that's useful for running
// tests. The FileRunnerDelegate provides built-in modules for "console" and
// "gtest" that are useful when writing unit tests.
//
// TODO(abarth): Rename FileRunnerDelegate to TestRunnerDelegate.
class FileRunnerDelegate : public ModuleRunnerDelegate {
 public:
  FileRunnerDelegate();
  ~FileRunnerDelegate() override;

 private:
  // From ModuleRunnerDelegate:
  void UnhandledException(ShellRunner* runner, TryCatch& try_catch) override;

  DISALLOW_COPY_AND_ASSIGN(FileRunnerDelegate);
};

void RunTestFromFile(const base::FilePath& path, FileRunnerDelegate* delegate,
                     bool run_until_idle = true);

}  // namespace gin

#endif  // GIN_TEST_FILE_RUNNER_H_
