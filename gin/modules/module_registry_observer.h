// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GIN_MODULES_MODULE_REGISTRY_OBSERVER_H_
#define GIN_MODULES_MODULE_REGISTRY_OBSERVER_H_

#include <string>
#include <vector>

#include "gin/gin_export.h"

namespace gin {

// Notified of interesting events from ModuleRegistry.
class GIN_EXPORT ModuleRegistryObserver {
 public:
  // Called from AddPendingModule(). |id| is the id/name of the module and
  // |dependencies| this list of modules |id| depends upon.
  virtual void OnDidAddPendingModule(
      const std::string& id,
      const std::vector<std::string>& dependencies) = 0;

 protected:
  virtual ~ModuleRegistryObserver() {}
};

}  // namespace gin

#endif  // GIN_MODULES_MODULE_REGISTRY_OBSERVER_H_

