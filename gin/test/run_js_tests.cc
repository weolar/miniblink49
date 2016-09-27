// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/files/file_util.h"
#include "base/path_service.h"
#include "gin/test/file_runner.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace gin {
namespace {

base::FilePath BasePath() {
  base::FilePath path;
  PathService::Get(base::DIR_SOURCE_ROOT, &path);
  return path.AppendASCII("gin");
}

void RunTest(const base::FilePath& path) {
  FileRunnerDelegate delegate;
  RunTestFromFile(path, &delegate);
}

TEST(JSTest, File) {
  RunTest(BasePath()
          .AppendASCII("test")
          .AppendASCII("file_unittests.js"));
}

TEST(JSTest, GTest) {
  RunTest(BasePath()
          .AppendASCII("test")
          .AppendASCII("gtest_unittests.js"));
}

TEST(JSTest, ModuleRegistry) {
  RunTest(BasePath()
          .AppendASCII("modules")
          .AppendASCII("module_registry_unittests.js"));
}

}  // namespace
}  // gin
