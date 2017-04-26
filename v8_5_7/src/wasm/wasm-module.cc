// Copyright 2015 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <memory>

#include "src/base/adapters.h"
#include "src/base/atomic-utils.h"
#include "src/code-stubs.h"
#include "src/compiler/wasm-compiler.h"
#include "src/debug/interface-types.h"
#include "src/macro-assembler.h"
#include "src/objects.h"
#include "src/property-descriptor.h"
#include "src/simulator.h"
#include "src/snapshot/snapshot.h"
#include "src/v8.h"

#include "src/wasm/ast-decoder.h"
#include "src/wasm/module-decoder.h"
#include "src/wasm/wasm-js.h"
#include "src/wasm/wasm-limits.h"
#include "src/wasm/wasm-module.h"
#include "src/wasm/wasm-objects.h"
#include "src/wasm/wasm-result.h"

using namespace v8::internal;
using namespace v8::internal::wasm;
namespace base = v8::base;

#define TRACE(...)                                      \
  do {                                                  \
    if (FLAG_trace_wasm_instances) PrintF(__VA_ARGS__); \
  } while (false)

#define TRACE_CHAIN(instance)        \
  do {                               \
    instance->PrintInstancesChain(); \
  } while (false)

namespace {

static const int kInvalidSigIndex = -1;
static const int kPlaceholderMarker = 1000000000;

byte* raw_buffer_ptr(MaybeHandle<JSArrayBuffer> buffer, int offset) {
  return static_cast<byte*>(buffer.ToHandleChecked()->backing_store()) + offset;
}

MaybeHandle<String> ExtractStringFromModuleBytes(
    Isolate* isolate, Handle<WasmCompiledModule> compiled_module,
    uint32_t offset, uint32_t size) {
  // TODO(wasm): cache strings from modules if it's a performance win.
  Handle<SeqOneByteString> module_bytes = compiled_module->module_bytes();
  DCHECK_GE(module_bytes->length(), offset);
  DCHECK_GE(module_bytes->length() - offset, size);
  Address raw = module_bytes->GetCharsAddress() + offset;
  if (!unibrow::Utf8::Validate(reinterpret_cast<const byte*>(raw), size))
    return {};  // UTF8 decoding error for name.
  return isolate->factory()->NewStringFromUtf8SubString(
      module_bytes, static_cast<int>(offset), static_cast<int>(size));
}

void ReplaceReferenceInCode(Handle<Code> code, Handle<Object> old_ref,
                            Handle<Object> new_ref) {
  for (RelocIterator it(*code, 1 << RelocInfo::EMBEDDED_OBJECT); !it.done();
       it.next()) {
    if (it.rinfo()->target_object() == *old_ref) {
      it.rinfo()->set_target_object(*new_ref);
    }
  }
}

static void MemoryFinalizer(const v8::WeakCallbackInfo<void>& data) {
  DisallowHeapAllocation no_gc;
  JSArrayBuffer** p = reinterpret_cast<JSArrayBuffer**>(data.GetParameter());
  JSArrayBuffer* buffer = *p;

  void* memory = buffer->backing_store();
  base::OS::Free(memory,
                 RoundUp(kWasmMaxHeapOffset, base::OS::CommitPageSize()));

  data.GetIsolate()->AdjustAmountOfExternalAllocatedMemory(
      -buffer->byte_length()->Number());

  GlobalHandles::Destroy(reinterpret_cast<Object**>(p));
}

#if V8_TARGET_ARCH_64_BIT
const bool kGuardRegionsSupported = true;
#else
const bool kGuardRegionsSupported = false;
#endif

bool EnableGuardRegions() {
  return FLAG_wasm_guard_pages && kGuardRegionsSupported;
}

void* TryAllocateBackingStore(Isolate* isolate, size_t size,
                              bool enable_guard_regions, bool& is_external) {
  is_external = false;
  // TODO(eholk): Right now enable_guard_regions has no effect on 32-bit
  // systems. It may be safer to fail instead, given that other code might do
  // things that would be unsafe if they expected guard pages where there
  // weren't any.
  if (enable_guard_regions && kGuardRegionsSupported) {
    // TODO(eholk): On Windows we want to make sure we don't commit the guard
    // pages yet.

    // We always allocate the largest possible offset into the heap, so the
    // addressable memory after the guard page can be made inaccessible.
    const size_t alloc_size =
        RoundUp(kWasmMaxHeapOffset, base::OS::CommitPageSize());
    DCHECK_EQ(0, size % base::OS::CommitPageSize());

    // AllocateGuarded makes the whole region inaccessible by default.
    void* memory = base::OS::AllocateGuarded(alloc_size);
    if (memory == nullptr) {
      return nullptr;
    }

    // Make the part we care about accessible.
    base::OS::Unprotect(memory, size);

    reinterpret_cast<v8::Isolate*>(isolate)
        ->AdjustAmountOfExternalAllocatedMemory(size);

    is_external = true;
    return memory;
  } else {
    void* memory = isolate->array_buffer_allocator()->Allocate(size);
    return memory;
  }
}

void RelocateMemoryReferencesInCode(Handle<FixedArray> code_table,
                                    Address old_start, Address start,
                                    uint32_t prev_size, uint32_t new_size) {
  for (int i = 0; i < code_table->length(); ++i) {
    DCHECK(code_table->get(i)->IsCode());
    Handle<Code> code = Handle<Code>(Code::cast(code_table->get(i)));
    AllowDeferredHandleDereference embedding_raw_address;
    int mask = (1 << RelocInfo::WASM_MEMORY_REFERENCE) |
               (1 << RelocInfo::WASM_MEMORY_SIZE_REFERENCE);
    for (RelocIterator it(*code, mask); !it.done(); it.next()) {
      it.rinfo()->update_wasm_memory_reference(old_start, start, prev_size,
                                               new_size);
    }
  }
}

void RelocateGlobals(Handle<FixedArray> code_table, Address old_start,
                     Address globals_start) {
  for (int i = 0; i < code_table->length(); ++i) {
    DCHECK(code_table->get(i)->IsCode());
    Handle<Code> code = Handle<Code>(Code::cast(code_table->get(i)));
    AllowDeferredHandleDereference embedding_raw_address;
    int mask = 1 << RelocInfo::WASM_GLOBAL_REFERENCE;
    for (RelocIterator it(*code, mask); !it.done(); it.next()) {
      it.rinfo()->update_wasm_global_reference(old_start, globals_start);
    }
  }
}

Handle<Code> CreatePlaceholder(Factory* factory, uint32_t index,
                               Code::Kind kind) {
  // Create a placeholder code object and encode the corresponding index in
  // the {constant_pool_offset} field of the code object.
  // TODO(titzer): instead of placeholders, use a reloc_info mode.
  static byte buffer[] = {0, 0, 0, 0};  // fake instructions.
  static CodeDesc desc = {
      buffer, arraysize(buffer), arraysize(buffer), 0, 0, nullptr, 0, nullptr};
  Handle<Code> code = factory->NewCode(desc, Code::KindField::encode(kind),
                                       Handle<Object>::null());
  code->set_constant_pool_offset(static_cast<int>(index) + kPlaceholderMarker);
  return code;
}

bool LinkFunction(Handle<Code> unlinked,
                  std::vector<Handle<Code>>& code_table) {
  bool modified = false;
  int mode_mask = RelocInfo::ModeMask(RelocInfo::CODE_TARGET);
  AllowDeferredHandleDereference embedding_raw_address;
  for (RelocIterator it(*unlinked, mode_mask); !it.done(); it.next()) {
    RelocInfo::Mode mode = it.rinfo()->rmode();
    if (RelocInfo::IsCodeTarget(mode)) {
      Code* target =
          Code::GetCodeFromTargetAddress(it.rinfo()->target_address());
      if (target->constant_pool_offset() < kPlaceholderMarker) continue;
      switch (target->kind()) {
        case Code::WASM_FUNCTION:        // fall through
        case Code::WASM_TO_JS_FUNCTION:  // fall through
        case Code::JS_TO_WASM_FUNCTION: {
          // Patch direct calls to placeholder code objects.
          uint32_t index = target->constant_pool_offset() - kPlaceholderMarker;
          Handle<Code> new_target = code_table[index];
          if (target != *new_target) {
            it.rinfo()->set_target_address(new_target->instruction_start(),
                                           UPDATE_WRITE_BARRIER,
                                           SKIP_ICACHE_FLUSH);
            modified = true;
          }
          break;
        }
        default:
          break;
      }
    }
  }
  return modified;
}

void FlushICache(Isolate* isolate, Handle<FixedArray> code_table) {
  for (int i = 0; i < code_table->length(); ++i) {
    Handle<Code> code = code_table->GetValueChecked<Code>(isolate, i);
    Assembler::FlushICache(isolate, code->instruction_start(),
                           code->instruction_size());
  }
}

// Fetches the compilation unit of a wasm function and executes its parallel
// phase.
bool FetchAndExecuteCompilationUnit(
    Isolate* isolate,
    std::vector<compiler::WasmCompilationUnit*>* compilation_units,
    std::queue<compiler::WasmCompilationUnit*>* executed_units,
    base::Mutex* result_mutex, base::AtomicNumber<size_t>* next_unit) {
  DisallowHeapAllocation no_allocation;
  DisallowHandleAllocation no_handles;
  DisallowHandleDereference no_deref;
  DisallowCodeDependencyChange no_dependency_change;

  // - 1 because AtomicIncrement returns the value after the atomic increment.
  size_t index = next_unit->Increment(1) - 1;
  if (index >= compilation_units->size()) {
    return false;
  }

  compiler::WasmCompilationUnit* unit = compilation_units->at(index);
  if (unit != nullptr) {
    unit->ExecuteCompilation();
    base::LockGuard<base::Mutex> guard(result_mutex);
    executed_units->push(unit);
  }
  return true;
}

class WasmCompilationTask : public CancelableTask {
 public:
  WasmCompilationTask(
      Isolate* isolate,
      std::vector<compiler::WasmCompilationUnit*>* compilation_units,
      std::queue<compiler::WasmCompilationUnit*>* executed_units,
      base::Semaphore* on_finished, base::Mutex* result_mutex,
      base::AtomicNumber<size_t>* next_unit)
      : CancelableTask(isolate),
        isolate_(isolate),
        compilation_units_(compilation_units),
        executed_units_(executed_units),
        on_finished_(on_finished),
        result_mutex_(result_mutex),
        next_unit_(next_unit) {}

  void RunInternal() override {
    while (FetchAndExecuteCompilationUnit(isolate_, compilation_units_,
                                          executed_units_, result_mutex_,
                                          next_unit_)) {
    }
    on_finished_->Signal();
  }

  Isolate* isolate_;
  std::vector<compiler::WasmCompilationUnit*>* compilation_units_;
  std::queue<compiler::WasmCompilationUnit*>* executed_units_;
  base::Semaphore* on_finished_;
  base::Mutex* result_mutex_;
  base::AtomicNumber<size_t>* next_unit_;
};

static void RecordStats(Isolate* isolate, Code* code) {
  isolate->counters()->wasm_generated_code_size()->Increment(code->body_size());
  isolate->counters()->wasm_reloc_size()->Increment(
      code->relocation_info()->length());
}

static void RecordStats(Isolate* isolate, Handle<FixedArray> functions) {
  DisallowHeapAllocation no_gc;
  for (int i = 0; i < functions->length(); ++i) {
    RecordStats(isolate, Code::cast(functions->get(i)));
  }
}

Address GetGlobalStartAddressFromCodeTemplate(Object* undefined,
                                              JSObject* object) {
  auto instance = WasmInstanceObject::cast(object);
  Address old_address = nullptr;
  if (instance->has_globals_buffer()) {
    old_address =
        static_cast<Address>(instance->get_globals_buffer()->backing_store());
  }
  return old_address;
}

void InitializeParallelCompilation(
    Isolate* isolate, const std::vector<WasmFunction>& functions,
    std::vector<compiler::WasmCompilationUnit*>& compilation_units,
    ModuleBytesEnv& module_env, ErrorThrower* thrower) {
  for (uint32_t i = FLAG_skip_compiling_wasm_funcs; i < functions.size(); ++i) {
    const WasmFunction* func = &functions[i];
    compilation_units[i] =
        func->imported ? nullptr : new compiler::WasmCompilationUnit(
                                       thrower, isolate, &module_env, func, i);
  }
}

uint32_t* StartCompilationTasks(
    Isolate* isolate,
    std::vector<compiler::WasmCompilationUnit*>& compilation_units,
    std::queue<compiler::WasmCompilationUnit*>& executed_units,
    base::Semaphore* pending_tasks, base::Mutex& result_mutex,
    base::AtomicNumber<size_t>& next_unit) {
  const size_t num_tasks =
      Min(static_cast<size_t>(FLAG_wasm_num_compilation_tasks),
          V8::GetCurrentPlatform()->NumberOfAvailableBackgroundThreads());
  uint32_t* task_ids = new uint32_t[num_tasks];
  for (size_t i = 0; i < num_tasks; ++i) {
    WasmCompilationTask* task =
        new WasmCompilationTask(isolate, &compilation_units, &executed_units,
                                pending_tasks, &result_mutex, &next_unit);
    task_ids[i] = task->id();
    V8::GetCurrentPlatform()->CallOnBackgroundThread(
        task, v8::Platform::kShortRunningTask);
  }
  return task_ids;
}

void WaitForCompilationTasks(Isolate* isolate, uint32_t* task_ids,
                             base::Semaphore* pending_tasks) {
  const size_t num_tasks =
      Min(static_cast<size_t>(FLAG_wasm_num_compilation_tasks),
          V8::GetCurrentPlatform()->NumberOfAvailableBackgroundThreads());
  for (size_t i = 0; i < num_tasks; ++i) {
    // If the task has not started yet, then we abort it. Otherwise we wait for
    // it to finish.
    if (isolate->cancelable_task_manager()->TryAbort(task_ids[i]) !=
        CancelableTaskManager::kTaskAborted) {
      pending_tasks->Wait();
    }
  }
}

void FinishCompilationUnits(
    std::queue<compiler::WasmCompilationUnit*>& executed_units,
    std::vector<Handle<Code>>& results, base::Mutex& result_mutex) {
  while (true) {
    compiler::WasmCompilationUnit* unit = nullptr;
    {
      base::LockGuard<base::Mutex> guard(&result_mutex);
      if (executed_units.empty()) {
        break;
      }
      unit = executed_units.front();
      executed_units.pop();
    }
    int j = unit->index();
    results[j] = unit->FinishCompilation();
    delete unit;
  }
}

void CompileInParallel(Isolate* isolate, ModuleBytesEnv* module_env,
                       std::vector<Handle<Code>>& functions,
                       ErrorThrower* thrower) {
  const WasmModule* module = module_env->module;
  // Data structures for the parallel compilation.
  std::vector<compiler::WasmCompilationUnit*> compilation_units(
      module->functions.size());
  std::queue<compiler::WasmCompilationUnit*> executed_units;

  //-----------------------------------------------------------------------
  // For parallel compilation:
  // 1) The main thread allocates a compilation unit for each wasm function
  //    and stores them in the vector {compilation_units}.
  // 2) The main thread spawns {WasmCompilationTask} instances which run on
  //    the background threads.
  // 3.a) The background threads and the main thread pick one compilation
  //      unit at a time and execute the parallel phase of the compilation
  //      unit. After finishing the execution of the parallel phase, the
  //      result is enqueued in {executed_units}.
  // 3.b) If {executed_units} contains a compilation unit, the main thread
  //      dequeues it and finishes the compilation.
  // 4) After the parallel phase of all compilation units has started, the
  //    main thread waits for all {WasmCompilationTask} instances to finish.
  // 5) The main thread finishes the compilation.

  // Turn on the {CanonicalHandleScope} so that the background threads can
  // use the node cache.
  CanonicalHandleScope canonical(isolate);

  // 1) The main thread allocates a compilation unit for each wasm function
  //    and stores them in the vector {compilation_units}.
  InitializeParallelCompilation(isolate, module->functions, compilation_units,
                                *module_env, thrower);

  // Objects for the synchronization with the background threads.
  base::Mutex result_mutex;
  base::AtomicNumber<size_t> next_unit(
      static_cast<size_t>(FLAG_skip_compiling_wasm_funcs));

  // 2) The main thread spawns {WasmCompilationTask} instances which run on
  //    the background threads.
  std::unique_ptr<uint32_t[]> task_ids(StartCompilationTasks(
      isolate, compilation_units, executed_units, module->pending_tasks.get(),
      result_mutex, next_unit));

  // 3.a) The background threads and the main thread pick one compilation
  //      unit at a time and execute the parallel phase of the compilation
  //      unit. After finishing the execution of the parallel phase, the
  //      result is enqueued in {executed_units}.
  while (FetchAndExecuteCompilationUnit(isolate, &compilation_units,
                                        &executed_units, &result_mutex,
                                        &next_unit)) {
    // 3.b) If {executed_units} contains a compilation unit, the main thread
    //      dequeues it and finishes the compilation unit. Compilation units
    //      are finished concurrently to the background threads to save
    //      memory.
    FinishCompilationUnits(executed_units, functions, result_mutex);
  }
  // 4) After the parallel phase of all compilation units has started, the
  //    main thread waits for all {WasmCompilationTask} instances to finish.
  WaitForCompilationTasks(isolate, task_ids.get(), module->pending_tasks.get());
  // Finish the compilation of the remaining compilation units.
  FinishCompilationUnits(executed_units, functions, result_mutex);
}

void CompileSequentially(Isolate* isolate, ModuleBytesEnv* module_env,
                         std::vector<Handle<Code>>& functions,
                         ErrorThrower* thrower) {
  DCHECK(!thrower->error());

  const WasmModule* module = module_env->module;
  for (uint32_t i = FLAG_skip_compiling_wasm_funcs;
       i < module->functions.size(); ++i) {
    const WasmFunction& func = module->functions[i];
    if (func.imported) continue;  // Imports are compiled at instantiation time.

    Handle<Code> code = Handle<Code>::null();
    // Compile the function.
    code = compiler::WasmCompilationUnit::CompileWasmFunction(
        thrower, isolate, module_env, &func);
    if (code.is_null()) {
      WasmName str = module_env->GetName(&func);
      thrower->CompileError("Compilation of #%d:%.*s failed.", i, str.length(),
                            str.start());
      break;
    }
      // Install the code into the linker table.
    functions[i] = code;
  }
}

void PatchDirectCalls(Handle<FixedArray> old_functions,
                      Handle<FixedArray> new_functions, int start) {
  DCHECK_EQ(new_functions->length(), old_functions->length());

  DisallowHeapAllocation no_gc;
  std::map<Code*, Code*> old_to_new_code;
  for (int i = 0; i < new_functions->length(); ++i) {
    old_to_new_code.insert(std::make_pair(Code::cast(old_functions->get(i)),
                                          Code::cast(new_functions->get(i))));
  }
  int mode_mask = RelocInfo::ModeMask(RelocInfo::CODE_TARGET);
  AllowDeferredHandleDereference embedding_raw_address;
  for (int i = start; i < new_functions->length(); ++i) {
    Code* wasm_function = Code::cast(new_functions->get(i));
    for (RelocIterator it(wasm_function, mode_mask); !it.done(); it.next()) {
      Code* old_code =
          Code::GetCodeFromTargetAddress(it.rinfo()->target_address());
      if (old_code->kind() == Code::WASM_TO_JS_FUNCTION ||
          old_code->kind() == Code::WASM_FUNCTION) {
        auto found = old_to_new_code.find(old_code);
        DCHECK(found != old_to_new_code.end());
        Code* new_code = found->second;
        if (new_code != old_code) {
          it.rinfo()->set_target_address(new_code->instruction_start(),
                                         UPDATE_WRITE_BARRIER,
                                         SKIP_ICACHE_FLUSH);
        }
      }
    }
  }
}

static void ResetCompiledModule(Isolate* isolate, WasmInstanceObject* owner,
                                WasmCompiledModule* compiled_module) {
  TRACE("Resetting %d\n", compiled_module->instance_id());
  Object* undefined = *isolate->factory()->undefined_value();
  uint32_t old_mem_size = compiled_module->mem_size();
  uint32_t default_mem_size = compiled_module->default_mem_size();
  Object* mem_start = compiled_module->maybe_ptr_to_memory();
  Address old_mem_address = nullptr;
  Address globals_start =
      GetGlobalStartAddressFromCodeTemplate(undefined, owner);

  // Reset function tables.
  FixedArray* function_tables = nullptr;
  FixedArray* empty_function_tables = nullptr;
  if (compiled_module->has_function_tables()) {
    function_tables = compiled_module->ptr_to_function_tables();
    empty_function_tables = compiled_module->ptr_to_empty_function_tables();
    compiled_module->set_ptr_to_function_tables(empty_function_tables);
  }

  if (old_mem_size > 0) {
    CHECK_NE(mem_start, undefined);
    old_mem_address =
        static_cast<Address>(JSArrayBuffer::cast(mem_start)->backing_store());
  }
  int mode_mask = RelocInfo::ModeMask(RelocInfo::WASM_MEMORY_REFERENCE) |
                  RelocInfo::ModeMask(RelocInfo::WASM_MEMORY_SIZE_REFERENCE) |
                  RelocInfo::ModeMask(RelocInfo::WASM_GLOBAL_REFERENCE) |
                  RelocInfo::ModeMask(RelocInfo::EMBEDDED_OBJECT);

  // Patch code to update memory references, global references, and function
  // table references.
  Object* fct_obj = compiled_module->ptr_to_code_table();
  if (fct_obj != nullptr && fct_obj != undefined &&
      (old_mem_size > 0 || globals_start != nullptr || function_tables)) {
    FixedArray* functions = FixedArray::cast(fct_obj);
    for (int i = 0; i < functions->length(); ++i) {
      Code* code = Code::cast(functions->get(i));
      bool changed = false;
      for (RelocIterator it(code, mode_mask); !it.done(); it.next()) {
        RelocInfo::Mode mode = it.rinfo()->rmode();
        if (RelocInfo::IsWasmMemoryReference(mode) ||
            RelocInfo::IsWasmMemorySizeReference(mode)) {
          it.rinfo()->update_wasm_memory_reference(
              old_mem_address, nullptr, old_mem_size, default_mem_size);
          changed = true;
        } else if (RelocInfo::IsWasmGlobalReference(mode)) {
          it.rinfo()->update_wasm_global_reference(globals_start, nullptr);
          changed = true;
        } else if (RelocInfo::IsEmbeddedObject(mode) && function_tables) {
          Object* old = it.rinfo()->target_object();
          for (int j = 0; j < function_tables->length(); ++j) {
            if (function_tables->get(j) == old) {
              it.rinfo()->set_target_object(empty_function_tables->get(j));
              changed = true;
            }
          }
        }
      }
      if (changed) {
        Assembler::FlushICache(isolate, code->instruction_start(),
                               code->instruction_size());
      }
    }
  }
  compiled_module->reset_memory();
}

static void MemoryInstanceFinalizer(Isolate* isolate,
                                    WasmInstanceObject* instance) {
  DisallowHeapAllocation no_gc;
  // If the memory object is destroyed, nothing needs to be done here.
  if (!instance->has_memory_object()) return;
  Handle<WasmInstanceWrapper> instance_wrapper =
      handle(instance->get_instance_wrapper());
  DCHECK(WasmInstanceWrapper::IsWasmInstanceWrapper(*instance_wrapper));
  DCHECK(instance_wrapper->has_instance());
  bool has_prev = instance_wrapper->has_previous();
  bool has_next = instance_wrapper->has_next();
  Handle<WasmMemoryObject> memory_object(instance->get_memory_object());

  if (!has_prev && !has_next) {
    memory_object->ResetInstancesLink(isolate);
    return;
  } else {
    Handle<WasmInstanceWrapper> next_wrapper, prev_wrapper;
    if (!has_prev) {
      Handle<WasmInstanceWrapper> next_wrapper =
          instance_wrapper->next_wrapper();
      next_wrapper->reset_previous_wrapper();
      // As this is the first link in the memory object, destroying
      // without updating memory object would corrupt the instance chain in
      // the memory object.
      memory_object->set_instances_link(*next_wrapper);
    } else if (!has_next) {
      instance_wrapper->previous_wrapper()->reset_next_wrapper();
    } else {
      DCHECK(has_next && has_prev);
      Handle<WasmInstanceWrapper> prev_wrapper =
          instance_wrapper->previous_wrapper();
      Handle<WasmInstanceWrapper> next_wrapper =
          instance_wrapper->next_wrapper();
      prev_wrapper->set_next_wrapper(*next_wrapper);
      next_wrapper->set_previous_wrapper(*prev_wrapper);
    }
    // Reset to avoid dangling pointers
    instance_wrapper->reset();
  }
}

static void InstanceFinalizer(const v8::WeakCallbackInfo<void>& data) {
  DisallowHeapAllocation no_gc;
  JSObject** p = reinterpret_cast<JSObject**>(data.GetParameter());
  WasmInstanceObject* owner = reinterpret_cast<WasmInstanceObject*>(*p);
  Isolate* isolate = reinterpret_cast<Isolate*>(data.GetIsolate());
  // Is a link to shared memory instances exists, update the list of memory
  // instances before the instance is destroyed.
  if (owner->has_instance_wrapper()) MemoryInstanceFinalizer(isolate, owner);
  WasmCompiledModule* compiled_module = owner->get_compiled_module();
  TRACE("Finalizing %d {\n", compiled_module->instance_id());
  DCHECK(compiled_module->has_weak_wasm_module());
  WeakCell* weak_wasm_module = compiled_module->ptr_to_weak_wasm_module();

  // weak_wasm_module may have been cleared, meaning the module object
  // was GC-ed. In that case, there won't be any new instances created,
  // and we don't need to maintain the links between instances.
  if (!weak_wasm_module->cleared()) {
    JSObject* wasm_module = JSObject::cast(weak_wasm_module->value());
    WasmCompiledModule* current_template =
        WasmCompiledModule::cast(wasm_module->GetInternalField(0));

    TRACE("chain before {\n");
    TRACE_CHAIN(current_template);
    TRACE("}\n");

    DCHECK(!current_template->has_weak_prev_instance());
    WeakCell* next = compiled_module->maybe_ptr_to_weak_next_instance();
    WeakCell* prev = compiled_module->maybe_ptr_to_weak_prev_instance();

    if (current_template == compiled_module) {
      if (next == nullptr) {
        ResetCompiledModule(isolate, owner, compiled_module);
      } else {
        DCHECK(next->value()->IsFixedArray());
        wasm_module->SetInternalField(0, next->value());
        DCHECK_NULL(prev);
        WasmCompiledModule::cast(next->value())->reset_weak_prev_instance();
      }
    } else {
      DCHECK(!(prev == nullptr && next == nullptr));
      // the only reason prev or next would be cleared is if the
      // respective objects got collected, but if that happened,
      // we would have relinked the list.
      if (prev != nullptr) {
        DCHECK(!prev->cleared());
        if (next == nullptr) {
          WasmCompiledModule::cast(prev->value())->reset_weak_next_instance();
        } else {
          WasmCompiledModule::cast(prev->value())
              ->set_ptr_to_weak_next_instance(next);
        }
      }
      if (next != nullptr) {
        DCHECK(!next->cleared());
        if (prev == nullptr) {
          WasmCompiledModule::cast(next->value())->reset_weak_prev_instance();
        } else {
          WasmCompiledModule::cast(next->value())
              ->set_ptr_to_weak_prev_instance(prev);
        }
      }
    }
    TRACE("chain after {\n");
    TRACE_CHAIN(WasmCompiledModule::cast(wasm_module->GetInternalField(0)));
    TRACE("}\n");
  }
  compiled_module->reset_weak_owning_instance();
  GlobalHandles::Destroy(reinterpret_cast<Object**>(p));
  TRACE("}\n");
}

std::pair<int, int> GetFunctionOffsetAndLength(
    Handle<WasmCompiledModule> compiled_module, int func_index) {
  WasmModule* module = compiled_module->module();
  if (func_index < 0 ||
      static_cast<size_t>(func_index) > module->functions.size()) {
    return {0, 0};
  }
  WasmFunction& func = module->functions[func_index];
  return {static_cast<int>(func.code_start_offset),
          static_cast<int>(func.code_end_offset - func.code_start_offset)};
}

}  // namespace

Handle<JSArrayBuffer> wasm::NewArrayBuffer(Isolate* isolate, size_t size,
                                           bool enable_guard_regions) {
  if (size > (kV8MaxWasmMemoryPages * WasmModule::kPageSize)) {
    // TODO(titzer): lift restriction on maximum memory allocated here.
    return Handle<JSArrayBuffer>::null();
  }

  enable_guard_regions = enable_guard_regions && kGuardRegionsSupported;

  bool is_external;  // Set by TryAllocateBackingStore
  void* memory =
      TryAllocateBackingStore(isolate, size, enable_guard_regions, is_external);

  if (memory == nullptr) {
    return Handle<JSArrayBuffer>::null();
  }

#if DEBUG
  // Double check the API allocator actually zero-initialized the memory.
  const byte* bytes = reinterpret_cast<const byte*>(memory);
  for (size_t i = 0; i < size; ++i) {
    DCHECK_EQ(0, bytes[i]);
  }
#endif

  Handle<JSArrayBuffer> buffer = isolate->factory()->NewJSArrayBuffer();
  JSArrayBuffer::Setup(buffer, isolate, is_external, memory,
                       static_cast<int>(size));
  buffer->set_is_neuterable(false);
  buffer->set_has_guard_region(enable_guard_regions);

  if (is_external) {
    // We mark the buffer as external if we allocated it here with guard
    // pages. That means we need to arrange for it to be freed.

    // TODO(eholk): Finalizers may not run when the main thread is shutting
    // down, which means we may leak memory here.
    Handle<Object> global_handle = isolate->global_handles()->Create(*buffer);
    GlobalHandles::MakeWeak(global_handle.location(), global_handle.location(),
                            &MemoryFinalizer, v8::WeakCallbackType::kFinalizer);
  }

  return buffer;
}

const char* wasm::SectionName(WasmSectionCode code) {
  switch (code) {
    case kUnknownSectionCode:
      return "Unknown";
    case kTypeSectionCode:
      return "Type";
    case kImportSectionCode:
      return "Import";
    case kFunctionSectionCode:
      return "Function";
    case kTableSectionCode:
      return "Table";
    case kMemorySectionCode:
      return "Memory";
    case kGlobalSectionCode:
      return "Global";
    case kExportSectionCode:
      return "Export";
    case kStartSectionCode:
      return "Start";
    case kCodeSectionCode:
      return "Code";
    case kElementSectionCode:
      return "Element";
    case kDataSectionCode:
      return "Data";
    case kNameSectionCode:
      return "Name";
    default:
      return "<unknown>";
  }
}

std::ostream& wasm::operator<<(std::ostream& os, const WasmModule& module) {
  os << "WASM module with ";
  os << (module.min_mem_pages * module.kPageSize) << " min mem";
  os << (module.max_mem_pages * module.kPageSize) << " max mem";
  os << module.functions.size() << " functions";
  os << module.functions.size() << " globals";
  os << module.functions.size() << " data segments";
  return os;
}

std::ostream& wasm::operator<<(std::ostream& os, const WasmFunction& function) {
  os << "WASM function with signature " << *function.sig;

  os << " code bytes: "
     << (function.code_end_offset - function.code_start_offset);
  return os;
}

std::ostream& wasm::operator<<(std::ostream& os, const WasmFunctionName& name) {
  os << "#" << name.function_->func_index;
  if (name.function_->name_offset > 0) {
    if (name.name_.start()) {
      os << ":";
      os.write(name.name_.start(), name.name_.length());
    }
  } else {
    os << "?";
  }
  return os;
}

WasmInstanceObject* wasm::GetOwningWasmInstance(Code* code) {
  DCHECK(code->kind() == Code::WASM_FUNCTION);
  DisallowHeapAllocation no_gc;
  FixedArray* deopt_data = code->deoptimization_data();
  DCHECK_NOT_NULL(deopt_data);
  DCHECK(deopt_data->length() == 2);
  Object* weak_link = deopt_data->get(0);
  if (!weak_link->IsWeakCell()) return nullptr;
  WeakCell* cell = WeakCell::cast(weak_link);
  if (!cell->value()) return nullptr;
  return WasmInstanceObject::cast(cell->value());
}

int wasm::GetFunctionCodeOffset(Handle<WasmCompiledModule> compiled_module,
                                int func_index) {
  return GetFunctionOffsetAndLength(compiled_module, func_index).first;
}

WasmModule::WasmModule(Zone* owned)
    : owned_zone(owned), pending_tasks(new base::Semaphore(0)) {}

MaybeHandle<WasmCompiledModule> WasmModule::CompileFunctions(
    Isolate* isolate, Handle<WasmModuleWrapper> module_wrapper,
    ErrorThrower* thrower, const ModuleWireBytes& wire_bytes) const {
  Factory* factory = isolate->factory();

  MaybeHandle<WasmCompiledModule> nothing;

  WasmInstance temp_instance(this);
  temp_instance.context = isolate->native_context();
  temp_instance.mem_size = WasmModule::kPageSize * min_mem_pages;
  temp_instance.mem_start = nullptr;
  temp_instance.globals_start = nullptr;

  // Initialize the indirect tables with placeholders.
  int function_table_count = static_cast<int>(function_tables.size());
  Handle<FixedArray> function_tables =
      factory->NewFixedArray(function_table_count);
  for (int i = 0; i < function_table_count; ++i) {
    temp_instance.function_tables[i] = factory->NewFixedArray(0);
    function_tables->set(i, *temp_instance.function_tables[i]);
  }

  HistogramTimerScope wasm_compile_module_time_scope(
      isolate->counters()->wasm_compile_module_time());

  ModuleBytesEnv module_env(this, &temp_instance, wire_bytes);

  // The {code_table} array contains import wrappers and functions (which
  // are both included in {functions.size()}, and export wrappers.
  int code_table_size =
      static_cast<int>(functions.size() + num_exported_functions);
  Handle<FixedArray> code_table =
      factory->NewFixedArray(static_cast<int>(code_table_size), TENURED);

  // Initialize the code table with placeholders.
  for (uint32_t i = 0; i < functions.size(); ++i) {
    Code::Kind kind = Code::WASM_FUNCTION;
    if (i < num_imported_functions) kind = Code::WASM_TO_JS_FUNCTION;
    Handle<Code> placeholder = CreatePlaceholder(factory, i, kind);
    code_table->set(static_cast<int>(i), *placeholder);
    temp_instance.function_code[i] = placeholder;
  }

  isolate->counters()->wasm_functions_per_module()->AddSample(
      static_cast<int>(functions.size()));
  if (!FLAG_trace_wasm_decoder && FLAG_wasm_num_compilation_tasks != 0) {
    // Avoid a race condition by collecting results into a second vector.
    std::vector<Handle<Code>> results;
    results.reserve(temp_instance.function_code.size());
    for (size_t i = 0; i < temp_instance.function_code.size(); ++i) {
      results.push_back(temp_instance.function_code[i]);
    }
    CompileInParallel(isolate, &module_env, results, thrower);

    for (size_t i = 0; i < results.size(); ++i) {
      temp_instance.function_code[i] = results[i];
    }
  } else {
    CompileSequentially(isolate, &module_env, temp_instance.function_code,
                        thrower);
  }
  if (thrower->error()) return nothing;

  // At this point, compilation has completed. Update the code table.
  for (size_t i = FLAG_skip_compiling_wasm_funcs;
       i < temp_instance.function_code.size(); ++i) {
    Code* code = *temp_instance.function_code[i];
    code_table->set(static_cast<int>(i), code);
    RecordStats(isolate, code);
  }

  // Link the functions in the module.
  for (size_t i = FLAG_skip_compiling_wasm_funcs;
       i < temp_instance.function_code.size(); ++i) {
    Handle<Code> code = temp_instance.function_code[i];
    bool modified = LinkFunction(code, temp_instance.function_code);
    if (modified) {
      // TODO(mtrofin): do we need to flush the cache here?
      Assembler::FlushICache(isolate, code->instruction_start(),
                             code->instruction_size());
    }
  }

  // Create the compiled module object, and populate with compiled functions
  // and information needed at instantiation time. This object needs to be
  // serializable. Instantiation may occur off a deserialized version of this
  // object.
  Handle<WasmCompiledModule> ret =
      WasmCompiledModule::New(isolate, module_wrapper);
  ret->set_code_table(code_table);
  ret->set_min_mem_pages(min_mem_pages);
  ret->set_max_mem_pages(max_mem_pages);
  if (function_table_count > 0) {
    ret->set_function_tables(function_tables);
    ret->set_empty_function_tables(function_tables);
  }

  // Compile JS->WASM wrappers for exported functions.
  int func_index = 0;
  for (auto exp : export_table) {
    if (exp.kind != kExternalFunction) continue;
    Handle<Code> wasm_code =
        code_table->GetValueChecked<Code>(isolate, exp.index);
    Handle<Code> wrapper_code =
        compiler::CompileJSToWasmWrapper(isolate, this, wasm_code, exp.index);
    int export_index = static_cast<int>(functions.size() + func_index);
    code_table->set(export_index, *wrapper_code);
    RecordStats(isolate, *wrapper_code);
    func_index++;
  }

  {
    // TODO(wasm): only save the sections necessary to deserialize a
    // {WasmModule}. E.g. function bodies could be omitted.
    Handle<String> module_bytes_string =
        factory->NewStringFromOneByte(wire_bytes.module_bytes, TENURED)
            .ToHandleChecked();
    DCHECK(module_bytes_string->IsSeqOneByteString());
    ret->set_module_bytes(Handle<SeqOneByteString>::cast(module_bytes_string));
  }

  return ret;
}

static WasmFunction* GetWasmFunctionForImportWrapper(Isolate* isolate,
                                                     Handle<Object> target) {
  if (target->IsJSFunction()) {
    Handle<JSFunction> func = Handle<JSFunction>::cast(target);
    if (func->code()->kind() == Code::JS_TO_WASM_FUNCTION) {
      auto exported = Handle<WasmExportedFunction>::cast(func);
      Handle<WasmInstanceObject> other_instance(exported->instance(), isolate);
      int func_index = exported->function_index();
      return &other_instance->module()->functions[func_index];
    }
  }
  return nullptr;
}

static Handle<Code> UnwrapImportWrapper(Handle<Object> target) {
  Handle<JSFunction> func = Handle<JSFunction>::cast(target);
  Handle<Code> export_wrapper_code = handle(func->code());
  int found = 0;
  int mask = RelocInfo::ModeMask(RelocInfo::CODE_TARGET);
  Handle<Code> code;
  for (RelocIterator it(*export_wrapper_code, mask); !it.done(); it.next()) {
    RelocInfo* rinfo = it.rinfo();
    Address target_address = rinfo->target_address();
    Code* target = Code::GetCodeFromTargetAddress(target_address);
    if (target->kind() == Code::WASM_FUNCTION ||
        target->kind() == Code::WASM_TO_JS_FUNCTION) {
      ++found;
      code = handle(target);
    }
  }
  DCHECK(found == 1);
  return code;
}

static Handle<Code> CompileImportWrapper(Isolate* isolate, int index,
                                         FunctionSig* sig,
                                         Handle<JSReceiver> target,
                                         Handle<String> module_name,
                                         MaybeHandle<String> import_name,
                                         ModuleOrigin origin) {
  Handle<Code> code;
  WasmFunction* other_func = GetWasmFunctionForImportWrapper(isolate, target);
  if (other_func) {
    if (sig->Equals(other_func->sig)) {
      // Signature matched. Unwrap the JS->WASM wrapper and return the raw
      // WASM function code.
      return UnwrapImportWrapper(target);
    } else {
      return Handle<Code>::null();
    }
  } else {
    // Signature mismatch. Compile a new wrapper for the new signature.
    return compiler::CompileWasmToJSWrapper(isolate, target, sig, index,
                                            module_name, import_name, origin);
  }
}

static void UpdateDispatchTablesInternal(Isolate* isolate,
                                         Handle<FixedArray> dispatch_tables,
                                         int index, WasmFunction* function,
                                         Handle<Code> code) {
  DCHECK_EQ(0, dispatch_tables->length() % 3);
  for (int i = 0; i < dispatch_tables->length(); i += 3) {
    int table_index = Smi::cast(dispatch_tables->get(i + 1))->value();
    Handle<FixedArray> dispatch_table(
        FixedArray::cast(dispatch_tables->get(i + 2)), isolate);
    if (function) {
      // TODO(titzer): the signature might need to be copied to avoid
      // a dangling pointer in the signature map.
      Handle<WasmInstanceObject> instance(
          WasmInstanceObject::cast(dispatch_tables->get(i)), isolate);
      int sig_index = static_cast<int>(
          instance->module()->function_tables[table_index].map.FindOrInsert(
              function->sig));
      dispatch_table->set(index, Smi::FromInt(sig_index));
      dispatch_table->set(index + (dispatch_table->length() / 2), *code);
    } else {
      Code* code = nullptr;
      dispatch_table->set(index, Smi::FromInt(-1));
      dispatch_table->set(index + (dispatch_table->length() / 2), code);
    }
  }
}

void wasm::UpdateDispatchTables(Isolate* isolate,
                                Handle<FixedArray> dispatch_tables, int index,
                                Handle<JSFunction> function) {
  if (function.is_null()) {
    UpdateDispatchTablesInternal(isolate, dispatch_tables, index, nullptr,
                                 Handle<Code>::null());
  } else {
    UpdateDispatchTablesInternal(
        isolate, dispatch_tables, index,
        GetWasmFunctionForImportWrapper(isolate, function),
        UnwrapImportWrapper(function));
  }
}

// A helper class to simplify instantiating a module from a compiled module.
// It closes over the {Isolate}, the {ErrorThrower}, the {WasmCompiledModule},
// etc.
class WasmInstanceBuilder {
 public:
  WasmInstanceBuilder(Isolate* isolate, ErrorThrower* thrower,
                      Handle<JSObject> module_object, Handle<JSReceiver> ffi,
                      Handle<JSArrayBuffer> memory)
      : isolate_(isolate),
        thrower_(thrower),
        module_object_(module_object),
        ffi_(ffi),
        memory_(memory) {}

  // Build an instance, in all of its glory.
  MaybeHandle<WasmInstanceObject> Build() {
    MaybeHandle<WasmInstanceObject> nothing;
    HistogramTimerScope wasm_instantiate_module_time_scope(
        isolate_->counters()->wasm_instantiate_module_time());
    Factory* factory = isolate_->factory();

    //--------------------------------------------------------------------------
    // Reuse the compiled module (if no owner), otherwise clone.
    //--------------------------------------------------------------------------
    Handle<FixedArray> code_table;
    Handle<FixedArray> old_code_table;
    MaybeHandle<WasmInstanceObject> owner;

    TRACE("Starting new module instantiation\n");
    {
      // Root the owner, if any, before doing any allocations, which
      // may trigger GC.
      // Both owner and original template need to be in sync. Even
      // after we lose the original template handle, the code
      // objects we copied from it have data relative to the
      // instance - such as globals addresses.
      Handle<WasmCompiledModule> original;
      {
        DisallowHeapAllocation no_gc;
        original = handle(
            WasmCompiledModule::cast(module_object_->GetInternalField(0)));
        if (original->has_weak_owning_instance()) {
          owner = handle(WasmInstanceObject::cast(
              original->weak_owning_instance()->value()));
        }
      }
      DCHECK(!original.is_null());
      // Always make a new copy of the code_table, since the old_code_table
      // may still have placeholders for imports.
      old_code_table = original->code_table();
      code_table = factory->CopyFixedArray(old_code_table);

      if (original->has_weak_owning_instance()) {
        // Clone, but don't insert yet the clone in the instances chain.
        // We do that last. Since we are holding on to the owner instance,
        // the owner + original state used for cloning and patching
        // won't be mutated by possible finalizer runs.
        DCHECK(!owner.is_null());
        TRACE("Cloning from %d\n", original->instance_id());
        compiled_module_ = WasmCompiledModule::Clone(isolate_, original);
        // Avoid creating too many handles in the outer scope.
        HandleScope scope(isolate_);

        // Clone the code for WASM functions and exports.
        for (int i = 0; i < code_table->length(); ++i) {
          Handle<Code> orig_code =
              code_table->GetValueChecked<Code>(isolate_, i);
          switch (orig_code->kind()) {
            case Code::WASM_TO_JS_FUNCTION:
              // Imports will be overwritten with newly compiled wrappers.
              break;
            case Code::JS_TO_WASM_FUNCTION:
            case Code::WASM_FUNCTION: {
              Handle<Code> code = factory->CopyCode(orig_code);
              code_table->set(i, *code);
              break;
            }
            default:
              UNREACHABLE();
          }
        }
        RecordStats(isolate_, code_table);
      } else {
        // There was no owner, so we can reuse the original.
        compiled_module_ = original;
        TRACE("Reusing existing instance %d\n",
              compiled_module_->instance_id());
      }
      compiled_module_->set_code_table(code_table);
    }
    module_ = reinterpret_cast<WasmModuleWrapper*>(
                  *compiled_module_->module_wrapper())
                  ->get();

    //--------------------------------------------------------------------------
    // Allocate the instance object.
    //--------------------------------------------------------------------------
    Handle<WasmInstanceObject> instance =
        WasmInstanceObject::New(isolate_, compiled_module_);

    //--------------------------------------------------------------------------
    // Set up the globals for the new instance.
    //--------------------------------------------------------------------------
    MaybeHandle<JSArrayBuffer> old_globals;
    uint32_t globals_size = module_->globals_size;
    if (globals_size > 0) {
      const bool enable_guard_regions = false;
      Handle<JSArrayBuffer> global_buffer =
          NewArrayBuffer(isolate_, globals_size, enable_guard_regions);
      globals_ = global_buffer;
      if (globals_.is_null()) {
        thrower_->RangeError("Out of memory: wasm globals");
        return nothing;
      }
      Address old_address =
          owner.is_null() ? nullptr : GetGlobalStartAddressFromCodeTemplate(
                                          isolate_->heap()->undefined_value(),
                                          *owner.ToHandleChecked());
      RelocateGlobals(code_table, old_address,
                      static_cast<Address>(global_buffer->backing_store()));
      instance->set_globals_buffer(*global_buffer);
    }

    //--------------------------------------------------------------------------
    // Prepare for initialization of function tables.
    //--------------------------------------------------------------------------
    int function_table_count =
        static_cast<int>(module_->function_tables.size());
    table_instances_.reserve(module_->function_tables.size());
    for (int index = 0; index < function_table_count; ++index) {
      table_instances_.push_back({Handle<WasmTableObject>::null(),
                                  Handle<FixedArray>::null(),
                                  Handle<FixedArray>::null()});
    }

    //--------------------------------------------------------------------------
    // Process the imports for the module.
    //--------------------------------------------------------------------------
    int num_imported_functions = ProcessImports(code_table, instance);
    if (num_imported_functions < 0) return nothing;

    //--------------------------------------------------------------------------
    // Process the initialization for the module's globals.
    //--------------------------------------------------------------------------
    InitGlobals();

    //--------------------------------------------------------------------------
    // Set up the memory for the new instance.
    //--------------------------------------------------------------------------
    MaybeHandle<JSArrayBuffer> old_memory;

    uint32_t min_mem_pages = module_->min_mem_pages;
    isolate_->counters()->wasm_min_mem_pages_count()->AddSample(min_mem_pages);

    if (!memory_.is_null()) {
      // Set externally passed ArrayBuffer non neuterable.
      memory_->set_is_neuterable(false);

      DCHECK_IMPLIES(EnableGuardRegions(), module_->origin == kAsmJsOrigin ||
                                               memory_->has_guard_region());
    } else if (min_mem_pages > 0) {
      memory_ = AllocateMemory(min_mem_pages);
      if (memory_.is_null()) return nothing;  // failed to allocate memory
    }

    if (!memory_.is_null()) {
      instance->set_memory_buffer(*memory_);
      Address mem_start = static_cast<Address>(memory_->backing_store());
      uint32_t mem_size =
          static_cast<uint32_t>(memory_->byte_length()->Number());
      LoadDataSegments(mem_start, mem_size);

      uint32_t old_mem_size = compiled_module_->mem_size();
      Address old_mem_start =
          compiled_module_->has_memory()
              ? static_cast<Address>(
                    compiled_module_->memory()->backing_store())
              : nullptr;
      RelocateMemoryReferencesInCode(code_table, old_mem_start, mem_start,
                                     old_mem_size, mem_size);
      compiled_module_->set_memory(memory_);
    } else {
      LoadDataSegments(nullptr, 0);
    }

    //--------------------------------------------------------------------------
    // Set up the runtime support for the new instance.
    //--------------------------------------------------------------------------
    Handle<WeakCell> weak_link = factory->NewWeakCell(instance);

    for (int i = num_imported_functions + FLAG_skip_compiling_wasm_funcs;
         i < code_table->length(); ++i) {
      Handle<Code> code = code_table->GetValueChecked<Code>(isolate_, i);
      if (code->kind() == Code::WASM_FUNCTION) {
        Handle<FixedArray> deopt_data = factory->NewFixedArray(2, TENURED);
        deopt_data->set(0, *weak_link);
        deopt_data->set(1, Smi::FromInt(static_cast<int>(i)));
        deopt_data->set_length(2);
        code->set_deoptimization_data(*deopt_data);
      }
    }

    //--------------------------------------------------------------------------
    // Set up the exports object for the new instance.
    //--------------------------------------------------------------------------
    ProcessExports(code_table, instance);

    //--------------------------------------------------------------------------
    // Add instance to Memory object
    //--------------------------------------------------------------------------
    DCHECK(wasm::IsWasmInstance(*instance));
    if (instance->has_memory_object()) {
      instance->get_memory_object()->AddInstance(isolate_, instance);
    }

    //--------------------------------------------------------------------------
    // Set up the indirect function tables for the new instance.
    //--------------------------------------------------------------------------
    if (function_table_count > 0) InitializeTables(code_table, instance);

    if (num_imported_functions > 0 || !owner.is_null()) {
      // If the code was cloned, or new imports were compiled, patch.
      PatchDirectCalls(old_code_table, code_table, num_imported_functions);
    }

    FlushICache(isolate_, code_table);

    //--------------------------------------------------------------------------
    // Unpack and notify signal handler of protected instructions.
    //--------------------------------------------------------------------------
    {
      for (int i = 0; i < code_table->length(); ++i) {
        Handle<Code> code = code_table->GetValueChecked<Code>(isolate_, i);

        if (code->kind() != Code::WASM_FUNCTION) {
          continue;
        }

        FixedArray* protected_instructions = code->protected_instructions();

        Zone zone(isolate_->allocator(), "Wasm Module");
        ZoneVector<trap_handler::ProtectedInstructionData> unpacked(&zone);
        for (int i = 0; i < protected_instructions->length();
             i += Code::kTrapDataSize) {
          trap_handler::ProtectedInstructionData data;
          data.instr_offset =
              protected_instructions
                  ->GetValueChecked<Smi>(isolate_, i + Code::kTrapCodeOffset)
                  ->value();
          data.landing_offset =
              protected_instructions
                  ->GetValueChecked<Smi>(isolate_, i + Code::kTrapLandingOffset)
                  ->value();

          unpacked
#if USING_VC6RT != 1
            .emplace_back
#else
            .push_back
#endif
            (data);

        }
        // TODO(eholk): Register the protected instruction information once the
        // trap handler is in place.
      }
    }

    //--------------------------------------------------------------------------
    // Set up and link the new instance.
    //--------------------------------------------------------------------------
    {
      Handle<Object> global_handle =
          isolate_->global_handles()->Create(*instance);
      Handle<WeakCell> link_to_clone = factory->NewWeakCell(compiled_module_);
      Handle<WeakCell> link_to_owning_instance = factory->NewWeakCell(instance);
      MaybeHandle<WeakCell> link_to_original;
      MaybeHandle<WasmCompiledModule> original;
      if (!owner.is_null()) {
        // prepare the data needed for publishing in a chain, but don't link
        // just yet, because
        // we want all the publishing to happen free from GC interruptions, and
        // so we do it in
        // one GC-free scope afterwards.
        original = handle(owner.ToHandleChecked()->get_compiled_module());
        link_to_original = factory->NewWeakCell(original.ToHandleChecked());
      }
      // Publish the new instance to the instances chain.
      {
        DisallowHeapAllocation no_gc;
        if (!link_to_original.is_null()) {
          compiled_module_->set_weak_next_instance(
              link_to_original.ToHandleChecked());
          original.ToHandleChecked()->set_weak_prev_instance(link_to_clone);
          compiled_module_->set_weak_wasm_module(
              original.ToHandleChecked()->weak_wasm_module());
        }
        module_object_->SetInternalField(0, *compiled_module_);
        compiled_module_->set_weak_owning_instance(link_to_owning_instance);
        GlobalHandles::MakeWeak(global_handle.location(),
                                global_handle.location(), &InstanceFinalizer,
                                v8::WeakCallbackType::kFinalizer);
      }
    }
    //--------------------------------------------------------------------------
    // Run the start function if one was specified.
    //--------------------------------------------------------------------------
    if (module_->start_function_index >= 0) {
      HandleScope scope(isolate_);
      int start_index = module_->start_function_index;
      Handle<Code> startup_code =
          code_table->GetValueChecked<Code>(isolate_, start_index);
      FunctionSig* sig = module_->functions[start_index].sig;
      Handle<Code> wrapper_code = compiler::CompileJSToWasmWrapper(
          isolate_, module_, startup_code, start_index);
      Handle<WasmExportedFunction> startup_fct = WasmExportedFunction::New(
          isolate_, instance, MaybeHandle<String>(), start_index,
          static_cast<int>(sig->parameter_count()), wrapper_code);
      RecordStats(isolate_, *startup_code);
      // Call the JS function.
      Handle<Object> undefined = factory->undefined_value();
      MaybeHandle<Object> retval =
          Execution::Call(isolate_, startup_fct, undefined, 0, nullptr);

      if (retval.is_null()) {
        DCHECK(isolate_->has_pending_exception());
        isolate_->OptionalRescheduleException(false);
        // It's unfortunate that the new instance is already linked in the
        // chain. However, we need to set up everything before executing the
        // start function, such that stack trace information can be generated
        // correctly already in the start function.
        return nothing;
      }
    }

    DCHECK(!isolate_->has_pending_exception());
    TRACE("Finishing instance %d\n", compiled_module_->instance_id());
    TRACE_CHAIN(WasmCompiledModule::cast(module_object_->GetInternalField(0)));
    return instance;
  }

 private:
  // Represents the initialized state of a table.
  struct TableInstance {
    Handle<WasmTableObject> table_object;    // WebAssembly.Table instance
    Handle<FixedArray> js_wrappers;          // JSFunctions exported
    Handle<FixedArray> dispatch_table;       // internal (code, sig) pairs
  };

  Isolate* isolate_;
  WasmModule* module_;
  ErrorThrower* thrower_;
  Handle<JSObject> module_object_;
  Handle<JSReceiver> ffi_;
  Handle<JSArrayBuffer> memory_;
  Handle<JSArrayBuffer> globals_;
  Handle<WasmCompiledModule> compiled_module_;
  std::vector<TableInstance> table_instances_;
  std::vector<Handle<JSFunction>> js_wrappers_;

  // Helper routine to print out errors with imports (FFI).
  MaybeHandle<JSFunction> ReportFFIError(const char* error, uint32_t index,
                                         Handle<String> module_name,
                                         MaybeHandle<String> function_name) {
    Handle<String> function_name_handle;
    if (function_name.ToHandle(&function_name_handle)) {
      thrower_->TypeError(
          "Import #%d module=\"%.*s\" function=\"%.*s\" error: %s", index,
          module_name->length(), module_name->ToCString().get(),
          function_name_handle->length(),
          function_name_handle->ToCString().get(), error);
    } else {
      thrower_->TypeError("Import #%d module=\"%.*s\" error: %s", index,
                          module_name->length(), module_name->ToCString().get(),
                          error);
    }
    thrower_->TypeError("Import ");
    return MaybeHandle<JSFunction>();
  }

  // Look up an import value in the {ffi_} object.
  MaybeHandle<Object> LookupImport(uint32_t index, Handle<String> module_name,
                                   MaybeHandle<String> import_name) {
    if (ffi_.is_null()) {
      return ReportFFIError("FFI is not an object", index, module_name,
                            import_name);
    }

    // Look up the module first.
    MaybeHandle<Object> result =
        Object::GetPropertyOrElement(ffi_, module_name);
    if (result.is_null()) {
      return ReportFFIError("module not found", index, module_name,
                            import_name);
    }

    Handle<Object> module = result.ToHandleChecked();

    if (!import_name.is_null()) {
      // Look up the value in the module.
      if (!module->IsJSReceiver()) {
        return ReportFFIError("module is not an object or function", index,
                              module_name, import_name);
      }

      result =
          Object::GetPropertyOrElement(module, import_name.ToHandleChecked());
      if (result.is_null()) {
        return ReportFFIError("import not found", index, module_name,
                              import_name);
      }
    } else {
      // No function specified. Use the "default export".
      result = module;
    }

    return result;
  }

  uint32_t EvalUint32InitExpr(const WasmInitExpr& expr) {
    switch (expr.kind) {
      case WasmInitExpr::kI32Const:
        return expr.val.i32_const;
      case WasmInitExpr::kGlobalIndex: {
        uint32_t offset = module_->globals[expr.val.global_index].offset;
        return *reinterpret_cast<uint32_t*>(raw_buffer_ptr(globals_, offset));
      }
      default:
        UNREACHABLE();
        return 0;
    }
  }

  // Load data segments into the memory.
  void LoadDataSegments(Address mem_addr, size_t mem_size) {
    Handle<SeqOneByteString> module_bytes = compiled_module_->module_bytes();
    for (const WasmDataSegment& segment : module_->data_segments) {
      uint32_t source_size = segment.source_size;
      // Segments of size == 0 are just nops.
      if (source_size == 0) continue;
      uint32_t dest_offset = EvalUint32InitExpr(segment.dest_addr);
      if (dest_offset >= mem_size || source_size >= mem_size ||
          dest_offset > (mem_size - source_size)) {
        thrower_->TypeError("data segment (start = %" PRIu32 ", size = %" PRIu32
                            ") does not fit into memory (size = %" PRIuS ")",
                            dest_offset, source_size, mem_size);
        return;
      }
      byte* dest = mem_addr + dest_offset;
      const byte* src = reinterpret_cast<const byte*>(
          module_bytes->GetCharsAddress() + segment.source_offset);
      memcpy(dest, src, source_size);
    }
  }

  void WriteGlobalValue(WasmGlobal& global, Handle<Object> value) {
    double num = 0;
    if (value->IsSmi()) {
      num = Smi::cast(*value)->value();
    } else if (value->IsHeapNumber()) {
      num = HeapNumber::cast(*value)->value();
    } else {
      UNREACHABLE();
    }
    TRACE("init [globals+%u] = %lf, type = %s\n", global.offset, num,
          WasmOpcodes::TypeName(global.type));
    switch (global.type) {
      case kAstI32:
        *GetRawGlobalPtr<int32_t>(global) = static_cast<int32_t>(num);
        break;
      case kAstI64:
        // TODO(titzer): initialization of imported i64 globals.
        UNREACHABLE();
        break;
      case kAstF32:
        *GetRawGlobalPtr<float>(global) = static_cast<float>(num);
        break;
      case kAstF64:
        *GetRawGlobalPtr<double>(global) = static_cast<double>(num);
        break;
      default:
        UNREACHABLE();
    }
  }

  // Process the imports, including functions, tables, globals, and memory, in
  // order, loading them from the {ffi_} object. Returns the number of imported
  // functions.
  int ProcessImports(Handle<FixedArray> code_table,
                     Handle<WasmInstanceObject> instance) {
    int num_imported_functions = 0;
    int num_imported_tables = 0;
    for (int index = 0; index < static_cast<int>(module_->import_table.size());
         ++index) {
      WasmImport& import = module_->import_table[index];
      Handle<String> module_name =
          ExtractStringFromModuleBytes(isolate_, compiled_module_,
                                       import.module_name_offset,
                                       import.module_name_length)
              .ToHandleChecked();
      Handle<String> function_name = Handle<String>::null();
      if (import.field_name_length > 0) {
        function_name = ExtractStringFromModuleBytes(isolate_, compiled_module_,
                                                     import.field_name_offset,
                                                     import.field_name_length)
                            .ToHandleChecked();
      }

      MaybeHandle<Object> result =
          LookupImport(index, module_name, function_name);
      if (thrower_->error()) return -1;

      switch (import.kind) {
        case kExternalFunction: {
          // Function imports must be callable.
          Handle<Object> function = result.ToHandleChecked();
          if (!function->IsCallable()) {
            ReportFFIError("function import requires a callable", index,
                           module_name, function_name);
            return -1;
          }

          Handle<Code> import_wrapper = CompileImportWrapper(
              isolate_, index, module_->functions[import.index].sig,
              Handle<JSReceiver>::cast(function), module_name, function_name,
              module_->origin);
          if (import_wrapper.is_null()) {
            ReportFFIError("imported function does not match the expected type",
                           index, module_name, function_name);
            return -1;
          }
          code_table->set(num_imported_functions, *import_wrapper);
          RecordStats(isolate_, *import_wrapper);
          num_imported_functions++;
          break;
        }
        case kExternalTable: {
          Handle<Object> value = result.ToHandleChecked();
          if (!WasmJs::IsWasmTableObject(isolate_, value)) {
            ReportFFIError("table import requires a WebAssembly.Table", index,
                           module_name, function_name);
            return -1;
          }
          WasmIndirectFunctionTable& table =
              module_->function_tables[num_imported_tables];
          TableInstance& table_instance = table_instances_[num_imported_tables];
          table_instance.table_object = Handle<WasmTableObject>::cast(value);
          table_instance.js_wrappers = Handle<FixedArray>(
              table_instance.table_object->get_functions(), isolate_);

          // TODO(titzer): import table size must match exactly for now.
          int table_size = table_instance.js_wrappers->length();
          if (table_size != static_cast<int>(table.min_size)) {
            thrower_->TypeError(
                "table import %d is wrong size (%d), expected %u", index,
                table_size, table.min_size);
            return -1;
          }

          // Allocate a new dispatch table.
          table_instance.dispatch_table =
              isolate_->factory()->NewFixedArray(table_size * 2);
          for (int i = 0; i < table_size * 2; ++i) {
            table_instance.dispatch_table->set(i,
                                               Smi::FromInt(kInvalidSigIndex));
          }
          // Initialize the dispatch table with the (foreign) JS functions
          // that are already in the table.
          for (int i = 0; i < table_size; ++i) {
            Handle<Object> val(table_instance.js_wrappers->get(i), isolate_);
            if (!val->IsJSFunction()) continue;
            WasmFunction* function =
                GetWasmFunctionForImportWrapper(isolate_, val);
            if (function == nullptr) {
              thrower_->TypeError("table import %d[%d] is not a WASM function",
                                  index, i);
              return -1;
            }
            int sig_index = table.map.FindOrInsert(function->sig);
            table_instance.dispatch_table->set(i, Smi::FromInt(sig_index));
            table_instance.dispatch_table->set(i + table_size,
                                               *UnwrapImportWrapper(val));
          }

          num_imported_tables++;
          break;
        }
        case kExternalMemory: {
          Handle<Object> object = result.ToHandleChecked();
          if (!WasmJs::IsWasmMemoryObject(isolate_, object)) {
            ReportFFIError("memory import must be a WebAssembly.Memory object",
                           index, module_name, function_name);
            return -1;
          }
          auto memory = Handle<WasmMemoryObject>::cast(object);
          DCHECK(WasmJs::IsWasmMemoryObject(isolate_, memory));
          instance->set_memory_object(*memory);
          memory_ = Handle<JSArrayBuffer>(memory->get_buffer(), isolate_);
          break;
        }
        case kExternalGlobal: {
          // Global imports are converted to numbers and written into the
          // {globals_} array buffer.
          Handle<Object> object = result.ToHandleChecked();
          MaybeHandle<Object> number = Object::ToNumber(object);
          if (number.is_null()) {
            ReportFFIError("global import could not be converted to number",
                           index, module_name, function_name);
            return -1;
          }
          Handle<Object> val = number.ToHandleChecked();
          WriteGlobalValue(module_->globals[import.index], val);
          break;
        }
        default:
          UNREACHABLE();
          break;
      }
    }
    return num_imported_functions;
  }

  template <typename T>
  T* GetRawGlobalPtr(WasmGlobal& global) {
    return reinterpret_cast<T*>(raw_buffer_ptr(globals_, global.offset));
  }

  // Process initialization of globals.
  void InitGlobals() {
    for (auto global : module_->globals) {
      switch (global.init.kind) {
        case WasmInitExpr::kI32Const:
          *GetRawGlobalPtr<int32_t>(global) = global.init.val.i32_const;
          break;
        case WasmInitExpr::kI64Const:
          *GetRawGlobalPtr<int64_t>(global) = global.init.val.i64_const;
          break;
        case WasmInitExpr::kF32Const:
          *GetRawGlobalPtr<float>(global) = global.init.val.f32_const;
          break;
        case WasmInitExpr::kF64Const:
          *GetRawGlobalPtr<double>(global) = global.init.val.f64_const;
          break;
        case WasmInitExpr::kGlobalIndex: {
          // Initialize with another global.
          uint32_t new_offset = global.offset;
          uint32_t old_offset =
              module_->globals[global.init.val.global_index].offset;
          TRACE("init [globals+%u] = [globals+%d]\n", global.offset,
                old_offset);
          size_t size = (global.type == kAstI64 || global.type == kAstF64)
                            ? sizeof(double)
                            : sizeof(int32_t);
          memcpy(raw_buffer_ptr(globals_, new_offset),
                 raw_buffer_ptr(globals_, old_offset), size);
          break;
        }
        case WasmInitExpr::kNone:
          // Happens with imported globals.
          break;
        default:
          UNREACHABLE();
          break;
      }
    }
  }

  // Allocate memory for a module instance as a new JSArrayBuffer.
  Handle<JSArrayBuffer> AllocateMemory(uint32_t min_mem_pages) {
    if (min_mem_pages > kV8MaxWasmMemoryPages) {
      thrower_->RangeError("Out of memory: wasm memory too large");
      return Handle<JSArrayBuffer>::null();
    }
    const bool enable_guard_regions = EnableGuardRegions();
    Handle<JSArrayBuffer> mem_buffer = NewArrayBuffer(
        isolate_, min_mem_pages * WasmModule::kPageSize, enable_guard_regions);

    if (mem_buffer.is_null()) {
      thrower_->RangeError("Out of memory: wasm memory");
    }
    return mem_buffer;
  }

  // Process the exports, creating wrappers for functions, tables, memories,
  // and globals.
  void ProcessExports(Handle<FixedArray> code_table,
                      Handle<WasmInstanceObject> instance) {
    bool needs_wrappers = module_->num_exported_functions > 0;
    for (auto table_instance : table_instances_) {
      if (!table_instance.js_wrappers.is_null()) {
        needs_wrappers = true;
        break;
      }
    }
    for (auto table : module_->function_tables) {
      if (table.exported) {
        needs_wrappers = true;
        break;
      }
    }
    if (needs_wrappers) {
      // Fill the table to cache the exported JSFunction wrappers.
      js_wrappers_.insert(js_wrappers_.begin(), module_->functions.size(),
                          Handle<JSFunction>::null());
    }

    Handle<JSObject> exports_object = instance;
    if (module_->export_table.size() > 0 && module_->origin == kWasmOrigin) {
      // Create the "exports" object.
      Handle<JSFunction> object_function = Handle<JSFunction>(
          isolate_->native_context()->object_function(), isolate_);
      exports_object =
          isolate_->factory()->NewJSObject(object_function, TENURED);
      Handle<String> exports_name =
          isolate_->factory()->InternalizeUtf8String("exports");
      JSObject::AddProperty(instance, exports_name, exports_object, READ_ONLY);
    }

    PropertyDescriptor desc;
    desc.set_writable(false);

    // Count up export indexes.
    int export_index = 0;
    for (auto exp : module_->export_table) {
      if (exp.kind == kExternalFunction) {
        ++export_index;
      }
    }
    // Process each export in the export table (go in reverse so asm.js
    // can skip duplicates).
    for (auto exp : base::Reversed(module_->export_table)) {
      Handle<String> name =
          ExtractStringFromModuleBytes(isolate_, compiled_module_,
                                       exp.name_offset, exp.name_length)
              .ToHandleChecked();
      switch (exp.kind) {
        case kExternalFunction: {
          // Wrap and export the code as a JSFunction.
          WasmFunction& function = module_->functions[exp.index];
          int func_index =
              static_cast<int>(module_->functions.size() + --export_index);
          Handle<JSFunction> js_function = js_wrappers_[exp.index];
          if (js_function.is_null()) {
            // Wrap the exported code as a JSFunction.
            Handle<Code> export_code =
                code_table->GetValueChecked<Code>(isolate_, func_index);
            MaybeHandle<String> func_name;
            if (module_->origin == kAsmJsOrigin) {
              // For modules arising from asm.js, honor the names section.
              func_name = ExtractStringFromModuleBytes(
                              isolate_, compiled_module_, function.name_offset,
                              function.name_length)
                              .ToHandleChecked();
            }
            js_function = WasmExportedFunction::New(
                isolate_, instance, func_name, function.func_index,
                static_cast<int>(function.sig->parameter_count()), export_code);
            js_wrappers_[exp.index] = js_function;
          }
          desc.set_value(js_function);
          break;
        }
        case kExternalTable: {
          // Export a table as a WebAssembly.Table object.
          TableInstance& table_instance = table_instances_[exp.index];
          WasmIndirectFunctionTable& table =
              module_->function_tables[exp.index];
          if (table_instance.table_object.is_null()) {
            uint32_t maximum =
                table.has_max ? table.max_size : kV8MaxWasmTableSize;
            table_instance.table_object = WasmTableObject::New(
                isolate_, table.min_size, maximum, &table_instance.js_wrappers);
          }
          desc.set_value(table_instance.table_object);
          break;
        }
        case kExternalMemory: {
          // Export the memory as a WebAssembly.Memory object.
          Handle<WasmMemoryObject> memory_object;
          if (!instance->has_memory_object()) {
            // If there was no imported WebAssembly.Memory object, create one.
            Handle<JSArrayBuffer> buffer(instance->get_memory_buffer(),
                                         isolate_);
            memory_object = WasmMemoryObject::New(
                isolate_, buffer,
                (module_->max_mem_pages != 0) ? module_->max_mem_pages : -1);
            instance->set_memory_object(*memory_object);
          } else {
            memory_object = Handle<WasmMemoryObject>(
                instance->get_memory_object(), isolate_);
            DCHECK(WasmJs::IsWasmMemoryObject(isolate_, memory_object));
            memory_object->ResetInstancesLink(isolate_);
          }

          desc.set_value(memory_object);
          break;
        }
        case kExternalGlobal: {
          // Export the value of the global variable as a number.
          WasmGlobal& global = module_->globals[exp.index];
          double num = 0;
          switch (global.type) {
            case kAstI32:
              num = *GetRawGlobalPtr<int32_t>(global);
              break;
            case kAstF32:
              num = *GetRawGlobalPtr<float>(global);
              break;
            case kAstF64:
              num = *GetRawGlobalPtr<double>(global);
              break;
            default:
              UNREACHABLE();
          }
          desc.set_value(isolate_->factory()->NewNumber(num));
          break;
        }
        default:
          UNREACHABLE();
          break;
      }

      // Skip duplicates for asm.js.
      if (module_->origin == kAsmJsOrigin) {
        v8::Maybe<bool> status =
            JSReceiver::HasOwnProperty(exports_object, name);
        if (status.FromMaybe(false)) {
          continue;
        }
      }
      v8::Maybe<bool> status = JSReceiver::DefineOwnProperty(
          isolate_, exports_object, name, &desc, Object::THROW_ON_ERROR);
      if (!status.IsJust()) {
        thrower_->TypeError("export of %.*s failed.", name->length(),
                            name->ToCString().get());
        return;
      }
    }
  }

  void InitializeTables(Handle<FixedArray> code_table,
                        Handle<WasmInstanceObject> instance) {
    Handle<FixedArray> old_function_tables =
        compiled_module_->function_tables();
    int function_table_count =
        static_cast<int>(module_->function_tables.size());
    Handle<FixedArray> new_function_tables =
        isolate_->factory()->NewFixedArray(function_table_count);
    for (int index = 0; index < function_table_count; ++index) {
      WasmIndirectFunctionTable& table = module_->function_tables[index];
      TableInstance& table_instance = table_instances_[index];
      int table_size = static_cast<int>(table.min_size);

      if (table_instance.dispatch_table.is_null()) {
        // Create a new dispatch table if necessary.
        table_instance.dispatch_table =
            isolate_->factory()->NewFixedArray(table_size * 2);
        for (int i = 0; i < table_size; ++i) {
          // Fill the table with invalid signature indexes so that
          // uninitialized entries will always fail the signature check.
          table_instance.dispatch_table->set(i, Smi::FromInt(kInvalidSigIndex));
        }
      }

      new_function_tables->set(static_cast<int>(index),
                               *table_instance.dispatch_table);

      Handle<FixedArray> all_dispatch_tables;
      if (!table_instance.table_object.is_null()) {
        // Get the existing dispatch table(s) with the WebAssembly.Table object.
        all_dispatch_tables = WasmTableObject::AddDispatchTable(
            isolate_, table_instance.table_object,
            Handle<WasmInstanceObject>::null(), index,
            Handle<FixedArray>::null());
      }

      // TODO(titzer): this does redundant work if there are multiple tables,
      // since initializations are not sorted by table index.
      for (auto table_init : module_->table_inits) {
        uint32_t base = EvalUint32InitExpr(table_init.offset);
        if (base > static_cast<uint32_t>(table_size) ||
            (base + table_init.entries.size() >
             static_cast<uint32_t>(table_size))) {
          thrower_->CompileError("table initializer is out of bounds");
          continue;
        }
        for (int i = 0; i < static_cast<int>(table_init.entries.size()); ++i) {
          uint32_t func_index = table_init.entries[i];
          WasmFunction* function = &module_->functions[func_index];
          int table_index = static_cast<int>(i + base);
          int32_t sig_index = table.map.Find(function->sig);
          DCHECK_GE(sig_index, 0);
          table_instance.dispatch_table->set(table_index,
                                             Smi::FromInt(sig_index));
          table_instance.dispatch_table->set(table_index + table_size,
                                             code_table->get(func_index));

          if (!all_dispatch_tables.is_null()) {
            Handle<Code> wasm_code(Code::cast(code_table->get(func_index)),
                                   isolate_);
            if (js_wrappers_[func_index].is_null()) {
              // No JSFunction entry yet exists for this function. Create one.
              // TODO(titzer): We compile JS->WASM wrappers for functions are
              // not exported but are in an exported table. This should be done
              // at module compile time and cached instead.
              WasmInstance temp_instance(module_);
              temp_instance.context = isolate_->native_context();
              temp_instance.mem_size = 0;
              temp_instance.mem_start = nullptr;
              temp_instance.globals_start = nullptr;

              Handle<Code> wrapper_code = compiler::CompileJSToWasmWrapper(
                  isolate_, module_, wasm_code, func_index);
              MaybeHandle<String> func_name;
              if (module_->origin == kAsmJsOrigin) {
                // For modules arising from asm.js, honor the names section.
                func_name = ExtractStringFromModuleBytes(
                                isolate_, compiled_module_,
                                function->name_offset, function->name_length)
                                .ToHandleChecked();
              }
              Handle<WasmExportedFunction> js_function =
                  WasmExportedFunction::New(
                      isolate_, instance, func_name, func_index,
                      static_cast<int>(function->sig->parameter_count()),
                      wrapper_code);
              js_wrappers_[func_index] = js_function;
            }
            table_instance.js_wrappers->set(table_index,
                                            *js_wrappers_[func_index]);

            UpdateDispatchTablesInternal(isolate_, all_dispatch_tables,
                                         table_index, function, wasm_code);
          }
        }
      }

      // TODO(titzer): we add the new dispatch table at the end to avoid
      // redundant work and also because the new instance is not yet fully
      // initialized.
      if (!table_instance.table_object.is_null()) {
        // Add the new dispatch table to the WebAssembly.Table object.
        all_dispatch_tables = WasmTableObject::AddDispatchTable(
            isolate_, table_instance.table_object, instance, index,
            table_instance.dispatch_table);
      }
    }
    // Patch all code that has references to the old indirect tables.
    for (int i = 0; i < code_table->length(); ++i) {
      if (!code_table->get(i)->IsCode()) continue;
      Handle<Code> code(Code::cast(code_table->get(i)), isolate_);
      for (int j = 0; j < function_table_count; ++j) {
        ReplaceReferenceInCode(
            code, Handle<Object>(old_function_tables->get(j), isolate_),
            Handle<Object>(new_function_tables->get(j), isolate_));
      }
    }
    compiled_module_->set_function_tables(new_function_tables);
  }
};

// Instantiates a WASM module, creating a WebAssembly.Instance from a
// WebAssembly.Module.
MaybeHandle<WasmInstanceObject> WasmModule::Instantiate(
    Isolate* isolate, ErrorThrower* thrower, Handle<JSObject> wasm_module,
    Handle<JSReceiver> ffi, Handle<JSArrayBuffer> memory) {
  WasmInstanceBuilder builder(isolate, thrower, wasm_module, ffi, memory);
  return builder.Build();
}

Handle<String> wasm::GetWasmFunctionName(Isolate* isolate,
                                         Handle<Object> instance_or_undef,
                                         uint32_t func_index) {
  if (!instance_or_undef->IsUndefined(isolate)) {
    Handle<WasmCompiledModule> compiled_module(
        Handle<WasmInstanceObject>::cast(instance_or_undef)
            ->get_compiled_module());
    MaybeHandle<String> maybe_name =
        WasmCompiledModule::GetFunctionName(compiled_module, func_index);
    if (!maybe_name.is_null()) return maybe_name.ToHandleChecked();
  }
  return isolate->factory()->NewStringFromStaticChars("<WASM UNNAMED>");
}

bool wasm::IsWasmInstance(Object* object) {
  return WasmInstanceObject::IsWasmInstanceObject(object);
}

bool wasm::WasmIsAsmJs(Object* instance, Isolate* isolate) {
  if (instance->IsUndefined(isolate)) return false;
  DCHECK(IsWasmInstance(instance));
  WasmCompiledModule* compiled_module =
      WasmInstanceObject::cast(instance)->get_compiled_module();
  DCHECK_EQ(compiled_module->has_asm_js_offset_table(),
            compiled_module->script()->type() == Script::TYPE_NORMAL);
  return compiled_module->has_asm_js_offset_table();
}

Handle<Script> wasm::GetScript(Handle<JSObject> instance) {
  WasmCompiledModule* compiled_module =
      WasmInstanceObject::cast(*instance)->get_compiled_module();
  DCHECK(compiled_module->has_script());
  return compiled_module->script();
}

Handle<WasmDebugInfo> wasm::GetDebugInfo(Handle<JSObject> object) {
  auto instance = Handle<WasmInstanceObject>::cast(object);
  if (instance->has_debug_info()) {
    Handle<WasmDebugInfo> info(instance->get_debug_info(),
                               instance->GetIsolate());
    return info;
  }
  Handle<WasmDebugInfo> new_info = WasmDebugInfo::New(instance);
  instance->set_debug_info(*new_info);
  return new_info;
}

// TODO(clemensh): origin can be inferred from asm_js_script; remove it.
MaybeHandle<WasmModuleObject> wasm::CreateModuleObjectFromBytes(
    Isolate* isolate, const byte* start, const byte* end, ErrorThrower* thrower,
    ModuleOrigin origin, Handle<Script> asm_js_script,
    const byte* asm_js_offset_tables_start,
    const byte* asm_js_offset_tables_end) {
  MaybeHandle<WasmModuleObject> nothing;
  ModuleResult result = DecodeWasmModule(isolate, start, end, false, origin);
  if (result.failed()) {
    if (result.val) delete result.val;
    thrower->CompileFailed("Wasm decoding failed", result);
    return nothing;
  }

  // The {module_wrapper} will take ownership of the {WasmModule} object,
  // and it will be destroyed when the GC reclaims the wrapper object.
  Handle<WasmModuleWrapper> module_wrapper =
      WasmModuleWrapper::New(isolate, const_cast<WasmModule*>(result.val));

  // Compile the functions of the module, producing a compiled module.
  MaybeHandle<WasmCompiledModule> maybe_compiled_module =
      result.val->CompileFunctions(isolate, module_wrapper, thrower,
                                   ModuleWireBytes(start, end));

  if (maybe_compiled_module.is_null()) return nothing;

  Handle<WasmCompiledModule> compiled_module =
      maybe_compiled_module.ToHandleChecked();

  DCHECK_EQ(origin == kAsmJsOrigin, !asm_js_script.is_null());
  DCHECK(!compiled_module->has_script());
  DCHECK(!compiled_module->has_asm_js_offset_table());
  if (origin == kAsmJsOrigin) {
    // Set script for the asm.js source, and the offset table mapping wasm byte
    // offsets to source positions.
    compiled_module->set_script(asm_js_script);
    size_t offset_table_len =
        asm_js_offset_tables_end - asm_js_offset_tables_start;
    DCHECK_GE(kMaxInt, offset_table_len);
    Handle<ByteArray> offset_table =
        isolate->factory()->NewByteArray(static_cast<int>(offset_table_len));
    memcpy(offset_table->GetDataStartAddress(), asm_js_offset_tables_start,
           offset_table_len);
    compiled_module->set_asm_js_offset_table(offset_table);
  } else {
    // Create a new Script object representing this wasm module, store it in the
    // compiled wasm module, and register it at the debugger.
    Handle<Script> script =
        isolate->factory()->NewScript(isolate->factory()->empty_string());
    script->set_type(Script::TYPE_WASM);

    DCHECK_GE(kMaxInt, end - start);
    int hash = StringHasher::HashSequentialString(
        reinterpret_cast<const char*>(start), static_cast<int>(end - start),
        kZeroHashSeed);

    const int kBufferSize = 50;
    char buffer[kBufferSize];
    int url_chars = SNPrintF(ArrayVector(buffer), "wasm://wasm/%08x", hash);
    DCHECK(url_chars >= 0 && url_chars < kBufferSize);
    MaybeHandle<String> url_str = isolate->factory()->NewStringFromOneByte(
        Vector<const uint8_t>(reinterpret_cast<uint8_t*>(buffer), url_chars),
        TENURED);
    script->set_source_url(*url_str.ToHandleChecked());

    int name_chars = SNPrintF(ArrayVector(buffer), "wasm-%08x", hash);
    DCHECK(name_chars >= 0 && name_chars < kBufferSize);
    MaybeHandle<String> name_str = isolate->factory()->NewStringFromOneByte(
        Vector<const uint8_t>(reinterpret_cast<uint8_t*>(buffer), name_chars),
        TENURED);
    script->set_name(*name_str.ToHandleChecked());

    script->set_wasm_compiled_module(*compiled_module);
    compiled_module->set_script(script);
    isolate->debug()->OnAfterCompile(script);
  }

  return WasmModuleObject::New(isolate, compiled_module);
}

bool wasm::ValidateModuleBytes(Isolate* isolate, const byte* start,
                               const byte* end, ErrorThrower* thrower,
                               ModuleOrigin origin) {
  ModuleResult result = DecodeWasmModule(isolate, start, end, true, origin);
  if (result.val) {
    delete result.val;
  } else {
    DCHECK(!result.ok());
  }
  return result.ok();
}

MaybeHandle<JSArrayBuffer> wasm::GetInstanceMemory(
    Isolate* isolate, Handle<WasmInstanceObject> object) {
  auto instance = Handle<WasmInstanceObject>::cast(object);
  if (instance->has_memory_buffer()) {
    return Handle<JSArrayBuffer>(instance->get_memory_buffer(), isolate);
  }
  return MaybeHandle<JSArrayBuffer>();
}

void SetInstanceMemory(Handle<WasmInstanceObject> instance,
                       JSArrayBuffer* buffer) {
  DisallowHeapAllocation no_gc;
  instance->set_memory_buffer(buffer);
  instance->get_compiled_module()->set_ptr_to_memory(buffer);
}

int32_t wasm::GetInstanceMemorySize(Isolate* isolate,
                                    Handle<WasmInstanceObject> instance) {
  DCHECK(IsWasmInstance(*instance));
  MaybeHandle<JSArrayBuffer> maybe_mem_buffer =
      GetInstanceMemory(isolate, instance);
  Handle<JSArrayBuffer> buffer;
  if (!maybe_mem_buffer.ToHandle(&buffer)) {
    return 0;
  } else {
    return buffer->byte_length()->Number() / WasmModule::kPageSize;
  }
}

uint32_t GetMaxInstanceMemorySize(Isolate* isolate,
                                  Handle<WasmInstanceObject> instance) {
  if (instance->has_memory_object()) {
    Handle<WasmMemoryObject> memory_object(instance->get_memory_object(),
                                           isolate);

    int maximum = memory_object->maximum_pages();
    if (maximum > 0) return static_cast<uint32_t>(maximum);
  }
  uint32_t compiled_max_pages =
      instance->get_compiled_module()->max_mem_pages();
  isolate->counters()->wasm_max_mem_pages_count()->AddSample(
      compiled_max_pages);
  if (compiled_max_pages != 0) return compiled_max_pages;
  return kV8MaxWasmMemoryPages;
}

Handle<JSArrayBuffer> GrowMemoryBuffer(Isolate* isolate,
                                       MaybeHandle<JSArrayBuffer> buffer,
                                       uint32_t pages, uint32_t max_pages) {
  Handle<JSArrayBuffer> old_buffer;
  Address old_mem_start = nullptr;
  uint32_t old_size = 0;
  if (buffer.ToHandle(&old_buffer) && old_buffer->backing_store() != nullptr) {
    old_mem_start = static_cast<Address>(old_buffer->backing_store());
    DCHECK_NOT_NULL(old_mem_start);
    old_size = old_buffer->byte_length()->Number();
  }
  DCHECK(old_size + pages * WasmModule::kPageSize <=
         std::numeric_limits<uint32_t>::max());
  uint32_t new_size = old_size + pages * WasmModule::kPageSize;
  if (new_size <= old_size || max_pages * WasmModule::kPageSize < new_size ||
      kV8MaxWasmMemoryPages * WasmModule::kPageSize < new_size) {
    return Handle<JSArrayBuffer>::null();
  }

  Handle<JSArrayBuffer> new_buffer;
  if (!old_buffer.is_null() && old_buffer->has_guard_region()) {
    // We don't move the backing store, we simply change the protection to make
    // more of it accessible.
    base::OS::Unprotect(old_buffer->backing_store(), new_size);
    reinterpret_cast<v8::Isolate*>(isolate)
        ->AdjustAmountOfExternalAllocatedMemory(pages * WasmModule::kPageSize);
    Handle<Object> new_size_object =
        isolate->factory()->NewNumberFromSize(new_size);
    old_buffer->set_byte_length(*new_size_object);
    new_buffer = old_buffer;
  } else {
    const bool enable_guard_regions = false;
    new_buffer = NewArrayBuffer(isolate, new_size, enable_guard_regions);
    if (new_buffer.is_null()) return new_buffer;
    Address new_mem_start = static_cast<Address>(new_buffer->backing_store());
    if (old_size != 0) {
      memcpy(new_mem_start, old_mem_start, old_size);
    }
  }
  return new_buffer;
}

void UncheckedUpdateInstanceMemory(Isolate* isolate,
                                   Handle<WasmInstanceObject> instance,
                                   Address old_mem_start, uint32_t old_size) {
  DCHECK(instance->has_memory_buffer());
  Handle<JSArrayBuffer> new_buffer(instance->get_memory_buffer());
  uint32_t new_size = new_buffer->byte_length()->Number();
  DCHECK(new_size <= std::numeric_limits<uint32_t>::max());
  Address new_mem_start = static_cast<Address>(new_buffer->backing_store());
  DCHECK_NOT_NULL(new_mem_start);
  Handle<FixedArray> code_table = instance->get_compiled_module()->code_table();
  RelocateMemoryReferencesInCode(code_table, old_mem_start, new_mem_start,
                                 old_size, new_size);
}

int32_t wasm::GrowWebAssemblyMemory(Isolate* isolate, Handle<Object> receiver,
                                    uint32_t pages) {
  DCHECK(WasmJs::IsWasmMemoryObject(isolate, receiver));
  Handle<WasmMemoryObject> memory_object =
      handle(WasmMemoryObject::cast(*receiver));
  Handle<WasmInstanceWrapper> instance_wrapper(
      memory_object->get_instances_link());
  DCHECK(WasmInstanceWrapper::IsWasmInstanceWrapper(*instance_wrapper));
  DCHECK(instance_wrapper->has_instance());
  Handle<WasmInstanceObject> instance = instance_wrapper->instance_object();
  DCHECK(IsWasmInstance(*instance));
  if (pages == 0) return GetInstanceMemorySize(isolate, instance);
  uint32_t max_pages = GetMaxInstanceMemorySize(isolate, instance);

  // Grow memory object buffer and update instances associated with it.
  MaybeHandle<JSArrayBuffer> memory_buffer =
      handle(memory_object->get_buffer());
  Handle<JSArrayBuffer> old_buffer;
  uint32_t old_size = 0;
  Address old_mem_start = nullptr;
  if (memory_buffer.ToHandle(&old_buffer) &&
      old_buffer->backing_store() != nullptr) {
    old_size = old_buffer->byte_length()->Number();
    old_mem_start = static_cast<Address>(old_buffer->backing_store());
  }
  Handle<JSArrayBuffer> new_buffer =
      GrowMemoryBuffer(isolate, memory_buffer, pages, max_pages);
  if (new_buffer.is_null()) return -1;
  DCHECK(!instance_wrapper->has_previous());
  SetInstanceMemory(instance, *new_buffer);
  UncheckedUpdateInstanceMemory(isolate, instance, old_mem_start, old_size);
  while (instance_wrapper->has_next()) {
    instance_wrapper = instance_wrapper->next_wrapper();
    DCHECK(WasmInstanceWrapper::IsWasmInstanceWrapper(*instance_wrapper));
    Handle<WasmInstanceObject> instance = instance_wrapper->instance_object();
    DCHECK(IsWasmInstance(*instance));
    SetInstanceMemory(instance, *new_buffer);
    UncheckedUpdateInstanceMemory(isolate, instance, old_mem_start, old_size);
  }
  memory_object->set_buffer(*new_buffer);
  DCHECK(old_size % WasmModule::kPageSize == 0);
  return (old_size / WasmModule::kPageSize);
}

int32_t wasm::GrowMemory(Isolate* isolate, Handle<WasmInstanceObject> instance,
                         uint32_t pages) {
  if (!IsWasmInstance(*instance)) return -1;
  if (pages == 0) return GetInstanceMemorySize(isolate, instance);
  Handle<WasmInstanceObject> instance_obj(WasmInstanceObject::cast(*instance));
  if (!instance_obj->has_memory_object()) {
    // No other instances to grow, grow just the one.
    MaybeHandle<JSArrayBuffer> instance_buffer =
        GetInstanceMemory(isolate, instance);
    Handle<JSArrayBuffer> old_buffer;
    uint32_t old_size = 0;
    Address old_mem_start = nullptr;
    if (instance_buffer.ToHandle(&old_buffer) &&
        old_buffer->backing_store() != nullptr) {
      old_size = old_buffer->byte_length()->Number();
      old_mem_start = static_cast<Address>(old_buffer->backing_store());
    }
    uint32_t max_pages = GetMaxInstanceMemorySize(isolate, instance_obj);
    Handle<JSArrayBuffer> buffer =
        GrowMemoryBuffer(isolate, instance_buffer, pages, max_pages);
    if (buffer.is_null()) return -1;
    SetInstanceMemory(instance, *buffer);
    UncheckedUpdateInstanceMemory(isolate, instance, old_mem_start, old_size);
    DCHECK(old_size % WasmModule::kPageSize == 0);
    return (old_size / WasmModule::kPageSize);
  } else {
    return GrowWebAssemblyMemory(
        isolate, handle(instance_obj->get_memory_object()), pages);
  }
}

void testing::ValidateInstancesChain(Isolate* isolate,
                                     Handle<WasmModuleObject> module_obj,
                                     int instance_count) {
  CHECK_GE(instance_count, 0);
  DisallowHeapAllocation no_gc;
  WasmCompiledModule* compiled_module = module_obj->get_compiled_module();
  CHECK_EQ(JSObject::cast(compiled_module->ptr_to_weak_wasm_module()->value()),
           *module_obj);
  Object* prev = nullptr;
  int found_instances = compiled_module->has_weak_owning_instance() ? 1 : 0;
  WasmCompiledModule* current_instance = compiled_module;
  while (current_instance->has_weak_next_instance()) {
    CHECK((prev == nullptr && !current_instance->has_weak_prev_instance()) ||
          current_instance->ptr_to_weak_prev_instance()->value() == prev);
    CHECK_EQ(current_instance->ptr_to_weak_wasm_module()->value(), *module_obj);
    CHECK(IsWasmInstance(
        current_instance->ptr_to_weak_owning_instance()->value()));
    prev = current_instance;
    current_instance = WasmCompiledModule::cast(
        current_instance->ptr_to_weak_next_instance()->value());
    ++found_instances;
    CHECK_LE(found_instances, instance_count);
  }
  CHECK_EQ(found_instances, instance_count);
}

void testing::ValidateModuleState(Isolate* isolate,
                                  Handle<WasmModuleObject> module_obj) {
  DisallowHeapAllocation no_gc;
  WasmCompiledModule* compiled_module = module_obj->get_compiled_module();
  CHECK(compiled_module->has_weak_wasm_module());
  CHECK_EQ(compiled_module->ptr_to_weak_wasm_module()->value(), *module_obj);
  CHECK(!compiled_module->has_weak_prev_instance());
  CHECK(!compiled_module->has_weak_next_instance());
  CHECK(!compiled_module->has_weak_owning_instance());
}

void testing::ValidateOrphanedInstance(Isolate* isolate,
                                       Handle<WasmInstanceObject> instance) {
  DisallowHeapAllocation no_gc;
  WasmCompiledModule* compiled_module = instance->get_compiled_module();
  CHECK(compiled_module->has_weak_wasm_module());
  CHECK(compiled_module->ptr_to_weak_wasm_module()->cleared());
}

void WasmCompiledModule::RecreateModuleWrapper(Isolate* isolate,
                                               Handle<FixedArray> array) {
  Handle<WasmCompiledModule> compiled_module(
      reinterpret_cast<WasmCompiledModule*>(*array), isolate);

  WasmModule* module = nullptr;
  {
    Handle<SeqOneByteString> module_bytes = compiled_module->module_bytes();
    // We parse the module again directly from the module bytes, so
    // the underlying storage must not be moved meanwhile.
    DisallowHeapAllocation no_allocation;
    const byte* start =
        reinterpret_cast<const byte*>(module_bytes->GetCharsAddress());
    const byte* end = start + module_bytes->length();
    // TODO(titzer): remember the module origin in the compiled_module
    // For now, we assume serialized modules did not originate from asm.js.
    ModuleResult result =
        DecodeWasmModule(isolate, start, end, false, kWasmOrigin);
    CHECK(result.ok());
    CHECK_NOT_NULL(result.val);
    module = const_cast<WasmModule*>(result.val);
  }

  Handle<WasmModuleWrapper> module_wrapper =
      WasmModuleWrapper::New(isolate, module);

  compiled_module->set_module_wrapper(module_wrapper);
  DCHECK(WasmCompiledModule::IsWasmCompiledModule(*compiled_module));
}

MaybeHandle<String> WasmCompiledModule::GetFunctionName(
    Handle<WasmCompiledModule> compiled_module, uint32_t func_index) {
  DCHECK_LT(func_index, compiled_module->module()->functions.size());
  WasmFunction& function = compiled_module->module()->functions[func_index];
  Isolate* isolate = compiled_module->GetIsolate();
  MaybeHandle<String> string = ExtractStringFromModuleBytes(
      isolate, compiled_module, function.name_offset, function.name_length);
  if (!string.is_null()) return string.ToHandleChecked();
  return {};
}
