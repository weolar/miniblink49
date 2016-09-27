// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GIN_MODULES_FILE_MODULE_PROVIDER_H_
#define GIN_MODULES_FILE_MODULE_PROVIDER_H_

#include <set>
#include <string>
#include <vector>

#include "base/files/file_path.h"
#include "gin/gin_export.h"
#include "gin/runner.h"

namespace gin {

// FileModuleProvider knows how to load AMD modules off disk. It searches for
// modules in the directories indiciated by |search_paths|. Although we still
// read from the file system on the main thread, we'll eventually want to move
// the reads to a background thread.
class GIN_EXPORT FileModuleProvider {
 public:
  explicit FileModuleProvider(
      const std::vector<base::FilePath>& search_paths);
  ~FileModuleProvider();

  // Searches for modules with |ids| in the file system. If found, the modules
  // will be executed asynchronously by |runner|.
  void AttempToLoadModules(Runner* runner, const std::set<std::string>& ids);

 private:
  std::vector<base::FilePath> search_paths_;

  // We'll only search for a given module once. We remember the set of modules
  // we've already looked for in |attempted_ids_|.
  std::set<std::string> attempted_ids_;

  DISALLOW_COPY_AND_ASSIGN(FileModuleProvider);
};

}  // namespace gin

#endif  // GIN_MODULES_FILE_MODULE_PROVIDER_H_
