// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef GIN_MODULES_MODULE_REGISTRY_H_
#define GIN_MODULES_MODULE_REGISTRY_H_

#include <list>
#include <map>
#include <set>
#include <string>

#include "base/callback.h"
#include "base/compiler_specific.h"
#include "base/memory/scoped_ptr.h"
#include "base/memory/scoped_vector.h"
#include "base/observer_list.h"
#include "gin/gin_export.h"
#include "v8/include/v8.h"

namespace gin {

class ModuleRegistryObserver;
struct PendingModule;

// This class implements the Asynchronous Module Definition (AMD) API.
// https://github.com/amdjs/amdjs-api/wiki/AMD
//
// Our implementation isn't complete yet. Missing features:
//   1) Built-in support for require, exports, and module.
//   2) Path resoltuion in module names.
//
// For these reasons, we don't have an "amd" property on the "define"
// function. The spec says we should only add that property once our
// implementation complies with the specification.
//
class GIN_EXPORT ModuleRegistry {
 public:
  typedef base::Callback<void (v8::Local<v8::Value>)> LoadModuleCallback;

  virtual ~ModuleRegistry();

  static ModuleRegistry* From(v8::Local<v8::Context> context);

  static void RegisterGlobals(v8::Isolate* isolate,
                              v8::Local<v8::ObjectTemplate> templ);

  // Installs the necessary functions needed for modules.
  // WARNING: this may execute script in the page.
  static bool InstallGlobals(v8::Isolate* isolate, v8::Local<v8::Object> obj);

  void AddObserver(ModuleRegistryObserver* observer);
  void RemoveObserver(ModuleRegistryObserver* observer);

  // The caller must have already entered our context.
  void AddBuiltinModule(v8::Isolate* isolate, const std::string& id,
                        v8::Local<v8::Value> module);

  // The caller must have already entered our context.
  void AddPendingModule(v8::Isolate* isolate,
                        scoped_ptr<PendingModule> pending);

  void LoadModule(v8::Isolate* isolate,
                  const std::string& id,
                  LoadModuleCallback callback);

  // The caller must have already entered our context.
  void AttemptToLoadMoreModules(v8::Isolate* isolate);

  const std::set<std::string>& available_modules() const {
    return available_modules_;
  }

  const std::set<std::string>& unsatisfied_dependencies() const {
    return unsatisfied_dependencies_;
  }

 private:
  typedef ScopedVector<PendingModule> PendingModuleVector;
  typedef std::multimap<std::string, LoadModuleCallback> LoadModuleCallbackMap;

  explicit ModuleRegistry(v8::Isolate* isolate);

  bool Load(v8::Isolate* isolate, scoped_ptr<PendingModule> pending);
  bool RegisterModule(v8::Isolate* isolate,
                      const std::string& id,
                      v8::Local<v8::Value> module);

  bool CheckDependencies(PendingModule* pending);
  bool AttemptToLoad(v8::Isolate* isolate, scoped_ptr<PendingModule> pending);

  v8::Local<v8::Value> GetModule(v8::Isolate* isolate, const std::string& id);

  std::set<std::string> available_modules_;
  std::set<std::string> unsatisfied_dependencies_;

  LoadModuleCallbackMap waiting_callbacks_;

  PendingModuleVector pending_modules_;
  v8::Persistent<v8::Object> modules_;

  base::ObserverList<ModuleRegistryObserver> observer_list_;

  DISALLOW_COPY_AND_ASSIGN(ModuleRegistry);
};

}  // namespace gin

#endif  // GIN_MODULES_MODULE_REGISTRY_H_
