// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gin/modules/module_registry.h"

#include <string>
#include <vector>

#include "base/logging.h"
#include "gin/arguments.h"
#include "gin/converter.h"
#include "gin/modules/module_registry_observer.h"
#include "gin/per_context_data.h"
#include "gin/per_isolate_data.h"
#include "gin/public/wrapper_info.h"
#include "gin/runner.h"

using v8::Context;
using v8::External;
using v8::Function;
using v8::FunctionTemplate;
using v8::Isolate;
using v8::Local;
using v8::Object;
using v8::ObjectTemplate;
using v8::Persistent;
using v8::StackTrace;
using v8::String;
using v8::Value;

namespace gin {

struct PendingModule {
  PendingModule();
  ~PendingModule();

  std::string id;
  std::vector<std::string> dependencies;
  Persistent<Value> factory;
};

PendingModule::PendingModule() {
}

PendingModule::~PendingModule() {
  factory.Reset();
}

namespace {

// Key for base::SupportsUserData::Data.
const char kModuleRegistryKey[] = "ModuleRegistry";

struct ModuleRegistryData : public base::SupportsUserData::Data {
  scoped_ptr<ModuleRegistry> registry;
};

void Define(const v8::FunctionCallbackInfo<Value>& info) {
  Arguments args(info);

  if (!info.Length())
    return args.ThrowTypeError("At least one argument is required.");

  std::string id;
  std::vector<std::string> dependencies;
  v8::Local<Value> factory;

  if (args.PeekNext()->IsString())
    args.GetNext(&id);
  if (args.PeekNext()->IsArray())
    args.GetNext(&dependencies);
  if (!args.GetNext(&factory))
    return args.ThrowError();

  scoped_ptr<PendingModule> pending(new PendingModule);
  pending->id = id;
  pending->dependencies = dependencies;
  pending->factory.Reset(args.isolate(), factory);

  ModuleRegistry* registry =
      ModuleRegistry::From(args.isolate()->GetCurrentContext());
  registry->AddPendingModule(args.isolate(), pending.Pass());
}

WrapperInfo g_wrapper_info = { kEmbedderNativeGin };

Local<FunctionTemplate> GetDefineTemplate(Isolate* isolate) {
  PerIsolateData* data = PerIsolateData::From(isolate);
  Local<FunctionTemplate> templ = data->GetFunctionTemplate(
      &g_wrapper_info);
  if (templ.IsEmpty()) {
    templ = FunctionTemplate::New(isolate, Define);
    data->SetFunctionTemplate(&g_wrapper_info, templ);
  }
  return templ;
}

}  // namespace

ModuleRegistry::ModuleRegistry(Isolate* isolate)
    : modules_(isolate, Object::New(isolate)) {
}

ModuleRegistry::~ModuleRegistry() {
  modules_.Reset();
}

// static
void ModuleRegistry::RegisterGlobals(Isolate* isolate,
                                     v8::Local<ObjectTemplate> templ) {
  templ->Set(StringToSymbol(isolate, "define"), GetDefineTemplate(isolate));
}

// static
bool ModuleRegistry::InstallGlobals(v8::Isolate* isolate,
                                    v8::Local<v8::Object> obj) {
  v8::Local<v8::Function> function;
  auto maybe_function =
      GetDefineTemplate(isolate)->GetFunction(isolate->GetCurrentContext());
  if (!maybe_function.ToLocal(&function))
    return false;
  return SetProperty(isolate, obj, StringToSymbol(isolate, "define"), function);
}

// static
ModuleRegistry* ModuleRegistry::From(v8::Local<Context> context) {
  PerContextData* data = PerContextData::From(context);
  if (!data)
    return NULL;

  ModuleRegistryData* registry_data = static_cast<ModuleRegistryData*>(
      data->GetUserData(kModuleRegistryKey));
  if (!registry_data) {
    // PerContextData takes ownership of ModuleRegistryData.
    registry_data = new ModuleRegistryData;
    registry_data->registry.reset(new ModuleRegistry(context->GetIsolate()));
    data->SetUserData(kModuleRegistryKey, registry_data);
  }
  return registry_data->registry.get();
}

void ModuleRegistry::AddObserver(ModuleRegistryObserver* observer) {
  observer_list_.AddObserver(observer);
}

void ModuleRegistry::RemoveObserver(ModuleRegistryObserver* observer) {
  observer_list_.RemoveObserver(observer);
}

void ModuleRegistry::AddBuiltinModule(Isolate* isolate, const std::string& id,
                                      v8::Local<Value> module) {
  DCHECK(!id.empty());
  RegisterModule(isolate, id, module);
}

void ModuleRegistry::AddPendingModule(Isolate* isolate,
                                      scoped_ptr<PendingModule> pending) {
  const std::string pending_id = pending->id;
  const std::vector<std::string> pending_dependencies = pending->dependencies;
  AttemptToLoad(isolate, pending.Pass());
  FOR_EACH_OBSERVER(ModuleRegistryObserver, observer_list_,
                    OnDidAddPendingModule(pending_id, pending_dependencies));
}

void ModuleRegistry::LoadModule(Isolate* isolate,
                                const std::string& id,
                                LoadModuleCallback callback) {
  if (available_modules_.find(id) != available_modules_.end()) {
    // Should we call the callback asynchronously?
    callback.Run(GetModule(isolate, id));
    return;
  }
  waiting_callbacks_.insert(std::make_pair(id, callback));

  for (size_t i = 0; i < pending_modules_.size(); ++i) {
    if (pending_modules_[i]->id == id)
      return;
  }

  unsatisfied_dependencies_.insert(id);
}

bool ModuleRegistry::RegisterModule(Isolate* isolate,
                                    const std::string& id,
                                    v8::Local<Value> module) {
  if (id.empty() || module.IsEmpty())
    return false;

  v8::Local<Object> modules = Local<Object>::New(isolate, modules_);
  if (!SetProperty(isolate, modules, StringToSymbol(isolate, id), module))
    return false;
  unsatisfied_dependencies_.erase(id);
  available_modules_.insert(id);

  std::pair<LoadModuleCallbackMap::iterator, LoadModuleCallbackMap::iterator>
      range = waiting_callbacks_.equal_range(id);
  std::vector<LoadModuleCallback> callbacks;
  callbacks.reserve(waiting_callbacks_.count(id));
  for (LoadModuleCallbackMap::iterator it = range.first; it != range.second;
       ++it) {
    callbacks.push_back(it->second);
  }
  waiting_callbacks_.erase(range.first, range.second);
  for (std::vector<LoadModuleCallback>::iterator it = callbacks.begin();
       it != callbacks.end();
       ++it) {
    // Should we call the callback asynchronously?
    it->Run(module);
  }
  return true;
}

bool ModuleRegistry::CheckDependencies(PendingModule* pending) {
  size_t num_missing_dependencies = 0;
  size_t len = pending->dependencies.size();
  for (size_t i = 0; i < len; ++i) {
    const std::string& dependency = pending->dependencies[i];
    if (available_modules_.count(dependency))
      continue;
    unsatisfied_dependencies_.insert(dependency);
    num_missing_dependencies++;
  }
  return num_missing_dependencies == 0;
}

bool ModuleRegistry::Load(Isolate* isolate, scoped_ptr<PendingModule> pending) {
  if (!pending->id.empty() && available_modules_.count(pending->id))
    return true;  // We've already loaded this module.

  uint32_t argc = static_cast<uint32_t>(pending->dependencies.size());
  std::vector<v8::Local<Value> > argv(argc);
  for (uint32_t i = 0; i < argc; ++i)
    argv[i] = GetModule(isolate, pending->dependencies[i]);

  v8::Local<Value> module = Local<Value>::New(isolate, pending->factory);

  v8::Local<Function> factory;
  if (ConvertFromV8(isolate, module, &factory)) {
    PerContextData* data = PerContextData::From(isolate->GetCurrentContext());
    Runner* runner = data->runner();
    module = runner->Call(factory, runner->global(), argc,
                          argv.empty() ? NULL : &argv.front());
    if (pending->id.empty())
      ConvertFromV8(isolate, factory->GetScriptOrigin().ResourceName(),
                    &pending->id);
  }

  return RegisterModule(isolate, pending->id, module);
}

bool ModuleRegistry::AttemptToLoad(Isolate* isolate,
                                   scoped_ptr<PendingModule> pending) {
  if (!CheckDependencies(pending.get())) {
    pending_modules_.push_back(pending.release());
    return false;
  }
  return Load(isolate, pending.Pass());
}

v8::Local<v8::Value> ModuleRegistry::GetModule(v8::Isolate* isolate,
                                                const std::string& id) {
  v8::Local<Object> modules = Local<Object>::New(isolate, modules_);
  v8::Local<String> key = StringToSymbol(isolate, id);
  DCHECK(modules->HasOwnProperty(isolate->GetCurrentContext(), key).FromJust());
  return modules->Get(isolate->GetCurrentContext(), key).ToLocalChecked();
}

void ModuleRegistry::AttemptToLoadMoreModules(Isolate* isolate) {
  bool keep_trying = true;
  while (keep_trying) {
    keep_trying = false;
    PendingModuleVector pending_modules;
    pending_modules.swap(pending_modules_);
    for (size_t i = 0; i < pending_modules.size(); ++i) {
      scoped_ptr<PendingModule> pending(pending_modules[i]);
      pending_modules[i] = NULL;
      if (AttemptToLoad(isolate, pending.Pass()))
        keep_trying = true;
    }
  }
}

}  // namespace gin
