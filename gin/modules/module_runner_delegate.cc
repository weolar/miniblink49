// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gin/modules/module_runner_delegate.h"

#include "base/bind.h"
#include "base/bind_helpers.h"
#include "gin/modules/module_registry.h"
#include "gin/object_template_builder.h"
#include "gin/public/context_holder.h"

namespace gin {

ModuleRunnerDelegate::ModuleRunnerDelegate(
  const std::vector<base::FilePath>& search_paths)
    : module_provider_(search_paths) {
}

ModuleRunnerDelegate::~ModuleRunnerDelegate() {
}

void ModuleRunnerDelegate::AddBuiltinModule(const std::string& id,
                                            ModuleGetter getter) {
  builtin_modules_[id] = base::Bind(getter);
}

void ModuleRunnerDelegate::AddBuiltinModule(const std::string& id,
    const ModuleGetterCallback& getter) {
  builtin_modules_[id] = getter;
}

void ModuleRunnerDelegate::AttemptToLoadMoreModules(Runner* runner) {
  ModuleRegistry* registry = ModuleRegistry::From(
      runner->GetContextHolder()->context());
  registry->AttemptToLoadMoreModules(runner->GetContextHolder()->isolate());
  module_provider_.AttempToLoadModules(
      runner, registry->unsatisfied_dependencies());
}

v8::Local<v8::ObjectTemplate> ModuleRunnerDelegate::GetGlobalTemplate(
    ShellRunner* runner,
    v8::Isolate* isolate) {
  v8::Local<v8::ObjectTemplate> templ = ObjectTemplateBuilder(isolate).Build();
  ModuleRegistry::RegisterGlobals(isolate, templ);
  return templ;
}

void ModuleRunnerDelegate::DidCreateContext(ShellRunner* runner) {
  ShellRunnerDelegate::DidCreateContext(runner);

  v8::Local<v8::Context> context = runner->GetContextHolder()->context();
  ModuleRegistry* registry = ModuleRegistry::From(context);

  v8::Isolate* isolate = runner->GetContextHolder()->isolate();

  for (BuiltinModuleMap::const_iterator it = builtin_modules_.begin();
       it != builtin_modules_.end(); ++it) {
    registry->AddBuiltinModule(isolate, it->first, it->second.Run(isolate));
  }
}

void ModuleRunnerDelegate::DidRunScript(ShellRunner* runner) {
  AttemptToLoadMoreModules(runner);
}

}  // namespace gin
