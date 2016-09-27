// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "gin/modules/module_registry.h"

#include "base/bind.h"
#include "gin/modules/module_registry_observer.h"
#include "gin/modules/module_runner_delegate.h"
#include "gin/public/context_holder.h"
#include "gin/public/isolate_holder.h"
#include "gin/shell_runner.h"
#include "gin/test/v8_test.h"
#include "v8/include/v8.h"

namespace gin {

namespace {

struct TestHelper {
  TestHelper(v8::Isolate* isolate)
      : delegate(std::vector<base::FilePath>()),
        runner(new ShellRunner(&delegate, isolate)),
        scope(runner.get()) {
  }

  ModuleRunnerDelegate delegate;
  scoped_ptr<ShellRunner> runner;
  Runner::Scope scope;
};

class ModuleRegistryObserverImpl : public ModuleRegistryObserver {
 public:
  ModuleRegistryObserverImpl() : did_add_count_(0) {}

  void OnDidAddPendingModule(
      const std::string& id,
      const std::vector<std::string>& dependencies) override {
    did_add_count_++;
    id_ = id;
    dependencies_ = dependencies;
  }

  int did_add_count() { return did_add_count_; }
  const std::string& id() const { return id_; }
  const std::vector<std::string>& dependencies() const { return dependencies_; }

 private:
  int did_add_count_;
  std::string id_;
  std::vector<std::string> dependencies_;

  DISALLOW_COPY_AND_ASSIGN(ModuleRegistryObserverImpl);
};

void NestedCallback(v8::Local<v8::Value> value) {
  FAIL() << "Should not be called";
}

void OnModuleLoaded(TestHelper* helper,
                    v8::Isolate* isolate,
                    int64_t* counter,
                    v8::Local<v8::Value> value) {
  ASSERT_TRUE(value->IsNumber());
  v8::Local<v8::Integer> int_value = v8::Local<v8::Integer>::Cast(value);
  *counter += int_value->Value();
  ModuleRegistry::From(helper->runner->GetContextHolder()->context())
      ->LoadModule(isolate, "two", base::Bind(NestedCallback));
}

void OnModuleLoadedNoOp(v8::Local<v8::Value> value) {
  ASSERT_TRUE(value->IsNumber());
}

}  // namespace

typedef V8Test ModuleRegistryTest;

// Verifies ModuleRegistry is not available after ContextHolder has been
// deleted.
TEST_F(ModuleRegistryTest, DestroyedWithContext) {
  v8::Isolate::Scope isolate_scope(instance_->isolate());
  v8::HandleScope handle_scope(instance_->isolate());
  v8::Local<v8::Context> context = v8::Context::New(
      instance_->isolate(), NULL, v8::Local<v8::ObjectTemplate>());
  {
    ContextHolder context_holder(instance_->isolate());
    context_holder.SetContext(context);
    ModuleRegistry* registry = ModuleRegistry::From(context);
    EXPECT_TRUE(registry != NULL);
  }
  ModuleRegistry* registry = ModuleRegistry::From(context);
  EXPECT_TRUE(registry == NULL);
}

// Verifies ModuleRegistryObserver is notified appropriately.
TEST_F(ModuleRegistryTest, ModuleRegistryObserverTest) {
  TestHelper helper(instance_->isolate());
  std::string source =
     "define('id', ['dep1', 'dep2'], function() {"
     "  return function() {};"
     "});";

  ModuleRegistryObserverImpl observer;
  ModuleRegistry::From(helper.runner->GetContextHolder()->context())->
      AddObserver(&observer);
  helper.runner->Run(source, "script");
  ModuleRegistry::From(helper.runner->GetContextHolder()->context())->
      RemoveObserver(&observer);
  EXPECT_EQ(1, observer.did_add_count());
  EXPECT_EQ("id", observer.id());
  ASSERT_EQ(2u, observer.dependencies().size());
  EXPECT_EQ("dep1", observer.dependencies()[0]);
  EXPECT_EQ("dep2", observer.dependencies()[1]);
}

// Verifies that multiple LoadModule calls for the same module are handled
// correctly.
TEST_F(ModuleRegistryTest, LoadModuleTest) {
  TestHelper helper(instance_->isolate());
  int64_t counter = 0;
  std::string source =
      "define('one', [], function() {"
      "  return 1;"
      "});";

  ModuleRegistry::LoadModuleCallback callback =
      base::Bind(OnModuleLoaded, &helper, instance_->isolate(), &counter);
  for (int i = 0; i < 3; i++) {
    ModuleRegistry::From(helper.runner->GetContextHolder()->context())
        ->LoadModule(instance_->isolate(), "one", callback);
  }
  EXPECT_EQ(0, counter);
  helper.runner->Run(source, "script");
  EXPECT_EQ(3, counter);
}

// Verifies that explicitly loading a module that's already pending does
// not cause the ModuleRegistry's unsatisfied_dependency set to grow.
TEST_F(ModuleRegistryTest, UnsatisfiedDependenciesTest) {
  TestHelper helper(instance_->isolate());
  std::string source =
      "define('one', ['no_such_module'], function(nsm) {"
      "  return 1;"
      "});";
  ModuleRegistry* registry =
    ModuleRegistry::From(helper.runner->GetContextHolder()->context());

  std::set<std::string> no_such_module_set;
  no_such_module_set.insert("no_such_module");

  // Adds one unsatisfied dependency on "no-such-module".
  helper.runner->Run(source, "script");
  EXPECT_EQ(no_such_module_set, registry->unsatisfied_dependencies());

  // Should have no effect on the unsatisfied_dependencies set.
  ModuleRegistry::LoadModuleCallback callback = base::Bind(OnModuleLoadedNoOp);
  registry->LoadModule(instance_->isolate(), "one", callback);
  EXPECT_EQ(no_such_module_set, registry->unsatisfied_dependencies());
}

}  // namespace gin
