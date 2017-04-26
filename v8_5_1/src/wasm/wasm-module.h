// Copyright 2015 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_WASM_MODULE_H_
#define V8_WASM_MODULE_H_

#include "src/wasm/wasm-opcodes.h"
#include "src/wasm/wasm-result.h"

#include "src/api.h"
#include "src/handles.h"

namespace v8 {
namespace internal {

namespace compiler {
class CallDescriptor;
}

namespace wasm {
const size_t kMaxModuleSize = 1024 * 1024 * 1024;
const size_t kMaxFunctionSize = 128 * 1024;
const size_t kMaxStringSize = 256;
const uint32_t kWasmMagic = 0x6d736100;
const uint32_t kWasmVersion = 0x0a;

// WebAssembly sections are named as strings in the binary format, but
// internally V8 uses an enum to handle them.
//
// Entries have the form F(enumerator, string).
#define FOR_EACH_WASM_SECTION_TYPE(F)          \
  F(Memory, "memory")                          \
  F(Signatures, "signatures")                  \
  F(Functions, "functions")                    \
  F(Globals, "globals")                        \
  F(DataSegments, "data_segments")             \
  F(FunctionTable, "function_table")           \
  F(End, "end")                                \
  F(StartFunction, "start_function")           \
  F(ImportTable, "import_table")               \
  F(ExportTable, "export_table")               \
  F(FunctionSignatures, "function_signatures") \
  F(FunctionBodies, "function_bodies")         \
  F(Names, "names")

// Contants for the above section types: {LEB128 length, characters...}.
#define WASM_SECTION_MEMORY 6, 'm', 'e', 'm', 'o', 'r', 'y'
#define WASM_SECTION_SIGNATURES \
  10, 's', 'i', 'g', 'n', 'a', 't', 'u', 'r', 'e', 's'
#define WASM_SECTION_FUNCTIONS 9, 'f', 'u', 'n', 'c', 't', 'i', 'o', 'n', 's'
#define WASM_SECTION_GLOBALS 7, 'g', 'l', 'o', 'b', 'a', 'l', 's'
#define WASM_SECTION_DATA_SEGMENTS \
  13, 'd', 'a', 't', 'a', '_', 's', 'e', 'g', 'm', 'e', 'n', 't', 's'
#define WASM_SECTION_FUNCTION_TABLE \
  14, 'f', 'u', 'n', 'c', 't', 'i', 'o', 'n', '_', 't', 'a', 'b', 'l', 'e'
#define WASM_SECTION_END 3, 'e', 'n', 'd'
#define WASM_SECTION_START_FUNCTION \
  14, 's', 't', 'a', 'r', 't', '_', 'f', 'u', 'n', 'c', 't', 'i', 'o', 'n'
#define WASM_SECTION_IMPORT_TABLE \
  12, 'i', 'm', 'p', 'o', 'r', 't', '_', 't', 'a', 'b', 'l', 'e'
#define WASM_SECTION_EXPORT_TABLE \
  12, 'e', 'x', 'p', 'o', 'r', 't', '_', 't', 'a', 'b', 'l', 'e'
#define WASM_SECTION_FUNCTION_SIGNATURES                                    \
  19, 'f', 'u', 'n', 'c', 't', 'i', 'o', 'n', '_', 's', 'i', 'g', 'n', 'a', \
      't', 'u', 'r', 'e', 's'
#define WASM_SECTION_FUNCTION_BODIES \
  15, 'f', 'u', 'n', 'c', 't', 'i', 'o', 'n', '_', 'b', 'o', 'd', 'i', 'e', 's'
#define WASM_SECTION_NAMES 5, 'n', 'a', 'm', 'e', 's'

// Constants for the above section headers' size (LEB128 + characters).
#define WASM_SECTION_MEMORY_SIZE ((size_t)7)
#define WASM_SECTION_SIGNATURES_SIZE ((size_t)11)
#define WASM_SECTION_FUNCTIONS_SIZE ((size_t)10)
#define WASM_SECTION_GLOBALS_SIZE ((size_t)8)
#define WASM_SECTION_DATA_SEGMENTS_SIZE ((size_t)14)
#define WASM_SECTION_FUNCTION_TABLE_SIZE ((size_t)15)
#define WASM_SECTION_END_SIZE ((size_t)4)
#define WASM_SECTION_START_FUNCTION_SIZE ((size_t)15)
#define WASM_SECTION_IMPORT_TABLE_SIZE ((size_t)13)
#define WASM_SECTION_EXPORT_TABLE_SIZE ((size_t)13)
#define WASM_SECTION_FUNCTION_SIGNATURES_SIZE ((size_t)20)
#define WASM_SECTION_FUNCTION_BODIES_SIZE ((size_t)16)
#define WASM_SECTION_NAMES_SIZE ((size_t)6)

struct WasmSection {
  enum class Code : uint32_t {
#define F(enumerator, string) enumerator,
    FOR_EACH_WASM_SECTION_TYPE(F)
#undef F
        Max
  };
  static WasmSection::Code begin();
  static WasmSection::Code end();
  static WasmSection::Code next(WasmSection::Code code);
  static const char* getName(Code code);
  static size_t getNameLength(Code code);
};

enum WasmFunctionDeclBit {
  kDeclFunctionName = 0x01,
  kDeclFunctionImport = 0x02,
  kDeclFunctionLocals = 0x04,
  kDeclFunctionExport = 0x08
};

// Constants for fixed-size elements within a module.
static const size_t kDeclMemorySize = 3;
static const size_t kDeclDataSegmentSize = 13;

// Static representation of a WASM function.
struct WasmFunction {
  FunctionSig* sig;      // signature of the function.
  uint32_t func_index;   // index into the function table.
  uint32_t sig_index;    // index into the signature table.
  uint32_t name_offset;  // offset in the module bytes of the name, if any.
  uint32_t name_length;  // length in bytes of the name.
  uint32_t code_start_offset;    // offset in the module bytes of code start.
  uint32_t code_end_offset;      // offset in the module bytes of code end.
  uint16_t local_i32_count;      // number of i32 local variables.
  uint16_t local_i64_count;      // number of i64 local variables.
  uint16_t local_f32_count;      // number of f32 local variables.
  uint16_t local_f64_count;      // number of f64 local variables.
  bool exported;                 // true if this function is exported.
  bool external;  // true if this function is externally supplied.
};

// Static representation of an imported WASM function.
struct WasmImport {
  FunctionSig* sig;               // signature of the function.
  uint32_t sig_index;             // index into the signature table.
  uint32_t module_name_offset;    // offset in module bytes of the module name.
  uint32_t module_name_length;    // length in bytes of the module name.
  uint32_t function_name_offset;  // offset in module bytes of the import name.
  uint32_t function_name_length;  // length in bytes of the import name.
};

// Static representation of an exported WASM function.
struct WasmExport {
  uint32_t func_index;   // index into the function table.
  uint32_t name_offset;  // offset in module bytes of the name to export.
  uint32_t name_length;  // length in bytes of the exported name.
};

// Static representation of a wasm global variable.
struct WasmGlobal {
  uint32_t name_offset;  // offset in the module bytes of the name, if any.
  uint32_t name_length;  // length in bytes of the global name.
  MachineType type;      // type of the global.
  uint32_t offset;       // offset from beginning of globals area.
  bool exported;         // true if this global is exported.
};

// Static representation of a wasm data segment.
struct WasmDataSegment {
  uint32_t dest_addr;      // destination memory address of the data.
  uint32_t source_offset;  // start offset in the module bytes.
  uint32_t source_size;    // end offset in the module bytes.
  bool init;               // true if loaded upon instantiation.
};

enum ModuleOrigin { kWasmOrigin, kAsmJsOrigin };

// Static representation of a module.
struct WasmModule {
  static const uint32_t kPageSize = 0x10000;    // Page size, 64kb.
  static const uint32_t kMinMemPages = 1;       // Minimum memory size = 64kb
  static const uint32_t kMaxMemPages = 16384;   // Maximum memory size =  1gb

  Isolate* shared_isolate;    // isolate for storing shared code.
  const byte* module_start;   // starting address for the module bytes.
  const byte* module_end;     // end address for the module bytes.
  uint32_t min_mem_pages;     // minimum size of the memory in 64k pages.
  uint32_t max_mem_pages;     // maximum size of the memory in 64k pages.
  bool mem_export;            // true if the memory is exported.
  bool mem_external;          // true if the memory is external.
  int start_function_index;   // start function, if any.
  ModuleOrigin origin;        // origin of the module

  std::vector<WasmGlobal> globals;             // globals in this module.
  std::vector<FunctionSig*> signatures;        // signatures in this module.
  std::vector<WasmFunction> functions;         // functions in this module.
  std::vector<WasmDataSegment> data_segments;  // data segments in this module.
  std::vector<uint16_t> function_table;        // function table.
  std::vector<WasmImport> import_table;        // import table.
  std::vector<WasmExport> export_table;        // export table.

  WasmModule();

  // Get a string stored in the module bytes representing a name.
  WasmName GetName(uint32_t offset, uint32_t length) const {
    if (length == 0) return {"<?>", 3};  // no name.
    CHECK(BoundsCheck(offset, offset + length));
    return {reinterpret_cast<const char*>(module_start + offset), length};
  }

  // Get a string stored in the module bytes representing a name.
  WasmName GetNameOrNull(uint32_t offset, uint32_t length) const {
    if (length == 0) return {NULL, 0};  // no name.
    CHECK(BoundsCheck(offset, offset + length));
    return {reinterpret_cast<const char*>(module_start + offset), length};
  }

  // Checks the given offset range is contained within the module bytes.
  bool BoundsCheck(uint32_t start, uint32_t end) const {
    size_t size = module_end - module_start;
    return start < size && end < size;
  }

  // Creates a new instantiation of the module in the given isolate.
  MaybeHandle<JSObject> Instantiate(Isolate* isolate, Handle<JSObject> ffi,
                                    Handle<JSArrayBuffer> memory);
};

// An instantiated WASM module, including memory, function table, etc.
struct WasmModuleInstance {
  WasmModule* module;  // static representation of the module.
  // -- Heap allocated --------------------------------------------------------
  Handle<JSObject> js_object;            // JavaScript module object.
  Handle<Context> context;               // JavaScript native context.
  Handle<JSArrayBuffer> mem_buffer;      // Handle to array buffer of memory.
  Handle<JSArrayBuffer> globals_buffer;  // Handle to array buffer of globals.
  Handle<FixedArray> function_table;     // indirect function table.
  std::vector<Handle<Code>> function_code;  // code objects for each function.
  std::vector<Handle<Code>> import_code;    // code objects for each import.
  // -- raw memory ------------------------------------------------------------
  byte* mem_start;  // start of linear memory.
  size_t mem_size;  // size of the linear memory.
  // -- raw globals -----------------------------------------------------------
  byte* globals_start;  // start of the globals area.
  size_t globals_size;  // size of the globals area.

  explicit WasmModuleInstance(WasmModule* m)
      : module(m),
        mem_start(nullptr),
        mem_size(0),
        globals_start(nullptr),
        globals_size(0) {}
};

// forward declaration.
class WasmLinker;

// Interface provided to the decoder/graph builder which contains only
// minimal information about the globals, functions, and function tables.
struct ModuleEnv {
  WasmModule* module;
  WasmModuleInstance* instance;
  WasmLinker* linker;
  ModuleOrigin origin;

  bool IsValidGlobal(uint32_t index) {
    return module && index < module->globals.size();
  }
  bool IsValidFunction(uint32_t index) {
    return module && index < module->functions.size();
  }
  bool IsValidSignature(uint32_t index) {
    return module && index < module->signatures.size();
  }
  bool IsValidImport(uint32_t index) {
    return module && index < module->import_table.size();
  }
  MachineType GetGlobalType(uint32_t index) {
    DCHECK(IsValidGlobal(index));
    return module->globals[index].type;
  }
  FunctionSig* GetFunctionSignature(uint32_t index) {
    DCHECK(IsValidFunction(index));
    return module->functions[index].sig;
  }
  FunctionSig* GetImportSignature(uint32_t index) {
    DCHECK(IsValidImport(index));
    return module->import_table[index].sig;
  }
  FunctionSig* GetSignature(uint32_t index) {
    DCHECK(IsValidSignature(index));
    return module->signatures[index];
  }
  size_t FunctionTableSize() {
    return module ? module->function_table.size() : 0;
  }

  bool asm_js() { return origin == kAsmJsOrigin; }

  Handle<Code> GetFunctionCode(uint32_t index);
  Handle<Code> GetImportCode(uint32_t index);
  Handle<FixedArray> GetFunctionTable();

  static compiler::CallDescriptor* GetWasmCallDescriptor(Zone* zone,
                                                         FunctionSig* sig);
  static compiler::CallDescriptor* GetI32WasmCallDescriptor(
      Zone* zone, compiler::CallDescriptor* descriptor);
  compiler::CallDescriptor* GetCallDescriptor(Zone* zone, uint32_t index);
};

// A helper for printing out the names of functions.
struct WasmFunctionName {
  const WasmFunction* function_;
  const WasmModule* module_;
  WasmFunctionName(const WasmFunction* function, const ModuleEnv* menv)
      : function_(function), module_(menv ? menv->module : nullptr) {}
};

std::ostream& operator<<(std::ostream& os, const WasmModule& module);
std::ostream& operator<<(std::ostream& os, const WasmFunction& function);
std::ostream& operator<<(std::ostream& os, const WasmFunctionName& name);

typedef Result<WasmModule*> ModuleResult;
typedef Result<WasmFunction*> FunctionResult;

// For testing. Decode, verify, and run the last exported function in the
// given encoded module.
int32_t CompileAndRunWasmModule(Isolate* isolate, const byte* module_start,
                                const byte* module_end, bool asm_js = false);

// For testing. Decode, verify, and run the last exported function in the
// given decoded module.
int32_t CompileAndRunWasmModule(Isolate* isolate, WasmModule* module);

}  // namespace wasm
}  // namespace internal
}  // namespace v8

#endif  // V8_WASM_MODULE_H_
