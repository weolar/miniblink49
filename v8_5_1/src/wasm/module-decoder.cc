// Copyright 2015 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#define _CRT_SECURE_NO_WARNINGS
#include "src/wasm/module-decoder.h"

#include "src/base/functional.h"
#include "src/base/platform/platform.h"
#include "src/macro-assembler.h"
#include "src/objects.h"
#include "src/v8.h"

#include "src/wasm/decoder.h"

namespace v8 {
namespace internal {
namespace wasm {

#if DEBUG
#define TRACE(...)                                    \
  do {                                                \
    if (FLAG_trace_wasm_decoder) PrintF(__VA_ARGS__); \
  } while (false)
#else
#define TRACE(...)
#endif


// The main logic for decoding the bytes of a module.
class ModuleDecoder : public Decoder {
 public:
  ModuleDecoder(Zone* zone, const byte* module_start, const byte* module_end,
                ModuleOrigin origin)
      : Decoder(module_start, module_end), module_zone(zone), origin_(origin) {
    result_.start = start_;
    if (limit_ < start_) {
      error(start_, "end is less than start");
      limit_ = start_;
    }
  }

  virtual void onFirstError() {
    pc_ = limit_;  // On error, terminate section decoding loop.
  }

  static void DumpModule(WasmModule* module, ModuleResult result) {
    std::string path;
    if (FLAG_dump_wasm_module_path) {
      path = FLAG_dump_wasm_module_path;
      if (path.size() &&
          !base::OS::isDirectorySeparator(path[path.size() - 1])) {
        path += base::OS::DirectorySeparator();
      }
    }
    // File are named `HASH.{ok,failed}.wasm`.
    size_t hash = base::hash_range(module->module_start, module->module_end);
    char buf[32] = {'\0'};
#if V8_OS_WIN && _MSC_VER < 1900
#define snprintf sprintf_s
#endif
    snprintf(buf, sizeof(buf) - 1, "%016zx.%s.wasm", hash,
             result.ok() ? "ok" : "failed");
    std::string name(buf);
    if (FILE* wasm_file = base::OS::FOpen((path + name).c_str(), "wb")) {
      fwrite(module->module_start, module->module_end - module->module_start, 1,
             wasm_file);
      fclose(wasm_file);
    }
  }

  // Decodes an entire module.
  ModuleResult DecodeModule(WasmModule* module, bool verify_functions = true) {
    pc_ = start_;
    module->module_start = start_;
    module->module_end = limit_;
    module->min_mem_pages = 0;
    module->max_mem_pages = 0;
    module->mem_export = false;
    module->mem_external = false;
    module->origin = origin_;

    bool sections[(size_t)WasmSection::Code::Max] = {false};

    const byte* pos = pc_;
    uint32_t magic_word = consume_u32("wasm magic");
#define BYTES(x) (x & 0xff), (x >> 8) & 0xff, (x >> 16) & 0xff, (x >> 24) & 0xff
    if (magic_word != kWasmMagic) {
      error(pos, pos,
            "expected magic word %02x %02x %02x %02x, "
            "found %02x %02x %02x %02x",
            BYTES(kWasmMagic), BYTES(magic_word));
      goto done;
    }

    pos = pc_;
    {
      uint32_t magic_version = consume_u32("wasm version");
      if (magic_version != kWasmVersion) {
        error(pos, pos,
              "expected version %02x %02x %02x %02x, "
              "found %02x %02x %02x %02x",
              BYTES(kWasmVersion), BYTES(magic_version));
        goto done;
      }
    }

    // Decode the module sections.
    while (pc_ < limit_) {
      TRACE("DecodeSection\n");
      pos = pc_;

      int length;
      uint32_t section_length = consume_u32v(&length, "section size");

      int section_string_leb_length = 0;
      uint32_t section_string_length = 0;
      WasmSection::Code section = consume_section_name(
          &section_string_leb_length, &section_string_length);
      uint32_t string_and_leb_length =
          section_string_leb_length + section_string_length;
      if (string_and_leb_length > section_length) {
        error(pos, pos,
              "section string of size %u longer than total section bytes %u",
              string_and_leb_length, section_length);
        break;
      }

      if (section == WasmSection::Code::Max) {
        // Skip unknown section.
        uint32_t skip = section_length - string_and_leb_length;
        TRACE("skipping %u bytes from unknown section\n", skip);
        consume_bytes(skip);
        continue;
      }

      // Each section should appear at most once.
      CheckForPreviousSection(sections, section, false);
      sections[(size_t)section] = true;

      switch (section) {
        case WasmSection::Code::End:
          // Terminate section decoding.
          limit_ = pc_;
          break;
        case WasmSection::Code::Memory:
          int length;
          module->min_mem_pages = consume_u32v(&length, "min memory");
          module->max_mem_pages = consume_u32v(&length, "max memory");
          module->mem_export = consume_u8("export memory") != 0;
          break;
        case WasmSection::Code::Signatures: {
          int length;
          uint32_t signatures_count = consume_u32v(&length, "signatures count");
          module->signatures.reserve(SafeReserve(signatures_count));
          // Decode signatures.
          for (uint32_t i = 0; i < signatures_count; i++) {
            if (failed()) break;
            TRACE("DecodeSignature[%d] module+%d\n", i,
                  static_cast<int>(pc_ - start_));
            FunctionSig* s = consume_sig();  // read function sig.
            module->signatures.push_back(s);
          }
          break;
        }
        case WasmSection::Code::FunctionSignatures: {
          // Functions require a signature table first.
          CheckForPreviousSection(sections, WasmSection::Code::Signatures,
                                  true);
          int length;
          uint32_t functions_count = consume_u32v(&length, "functions count");
          module->functions.reserve(SafeReserve(functions_count));
          for (uint32_t i = 0; i < functions_count; i++) {
            module->functions.push_back(
                {nullptr, i, 0, 0, 0, 0, 0, 0, false, false});
            WasmFunction* function = &module->functions.back();
            function->sig_index = consume_sig_index(module, &function->sig);
          }
          break;
        }
        case WasmSection::Code::FunctionBodies: {
          // Function bodies should follow signatures.
          CheckForPreviousSection(sections,
                                  WasmSection::Code::FunctionSignatures, true);
          int length;
          const byte* pos = pc_;
          uint32_t functions_count = consume_u32v(&length, "functions count");
          if (functions_count != module->functions.size()) {
            error(pos, pos, "function body count %u mismatch (%u expected)",
                  functions_count,
                  static_cast<uint32_t>(module->functions.size()));
            break;
          }
          for (uint32_t i = 0; i < functions_count; i++) {
            WasmFunction* function = &module->functions[i];
            int length;
            uint32_t size = consume_u32v(&length, "body size");
            function->code_start_offset = pc_offset();
            function->code_end_offset = pc_offset() + size;

            TRACE("  +%d  %-20s: (%d bytes)\n", pc_offset(), "function body",
                  size);
            pc_ += size;
            if (pc_ > limit_) {
              error(pc_, "function body extends beyond end of file");
            }
          }
          break;
        }
        case WasmSection::Code::Functions: {
          // Functions require a signature table first.
          CheckForPreviousSection(sections, WasmSection::Code::Signatures,
                                  true);
          int length;
          uint32_t functions_count = consume_u32v(&length, "functions count");
          module->functions.reserve(SafeReserve(functions_count));
          // Set up module environment for verification.
          ModuleEnv menv;
          menv.module = module;
          menv.instance = nullptr;
          menv.origin = origin_;
          // Decode functions.
          for (uint32_t i = 0; i < functions_count; i++) {
            if (failed()) break;
            TRACE("DecodeFunction[%d] module+%d\n", i,
                  static_cast<int>(pc_ - start_));

            module->functions.push_back(
                {nullptr, i, 0, 0, 0, 0, 0, 0, false, false});
            WasmFunction* function = &module->functions.back();
            DecodeFunctionInModule(module, function, false);
          }
          if (ok() && verify_functions) {
            for (uint32_t i = 0; i < functions_count; i++) {
              if (failed()) break;
              WasmFunction* function = &module->functions[i];
              if (!function->external) {
                VerifyFunctionBody(i, &menv, function);
                if (result_.failed())
                  error(result_.error_pc, result_.error_msg.get());
              }
            }
          }
          break;
        }
        case WasmSection::Code::Names: {
          // Names correspond to functions.
          CheckForPreviousSection(sections,
                                  WasmSection::Code::FunctionSignatures, true);
          int length;
          const byte* pos = pc_;
          uint32_t functions_count = consume_u32v(&length, "functions count");
          if (functions_count != module->functions.size()) {
            error(pos, pos, "function name count %u mismatch (%u expected)",
                  functions_count,
                  static_cast<uint32_t>(module->functions.size()));
            break;
          }

          for (uint32_t i = 0; i < functions_count; i++) {
            WasmFunction* function = &module->functions[i];
            function->name_offset =
                consume_string(&function->name_length, "function name");

            uint32_t local_names_count =
                consume_u32v(&length, "local names count");
            for (uint32_t j = 0; j < local_names_count; j++) {
              uint32_t unused = 0;
              uint32_t offset = consume_string(&unused, "local name");
              USE(unused);
              USE(offset);
            }
          }
          break;
        }
        case WasmSection::Code::Globals: {
          int length;
          uint32_t globals_count = consume_u32v(&length, "globals count");
          module->globals.reserve(SafeReserve(globals_count));
          // Decode globals.
          for (uint32_t i = 0; i < globals_count; i++) {
            if (failed()) break;
            TRACE("DecodeGlobal[%d] module+%d\n", i,
                  static_cast<int>(pc_ - start_));
            module->globals.push_back({0, 0, MachineType::Int32(), 0, false});
            WasmGlobal* global = &module->globals.back();
            DecodeGlobalInModule(global);
          }
          break;
        }
        case WasmSection::Code::DataSegments: {
          int length;
          uint32_t data_segments_count =
              consume_u32v(&length, "data segments count");
          module->data_segments.reserve(SafeReserve(data_segments_count));
          // Decode data segments.
          for (uint32_t i = 0; i < data_segments_count; i++) {
            if (failed()) break;
            TRACE("DecodeDataSegment[%d] module+%d\n", i,
                  static_cast<int>(pc_ - start_));
            module->data_segments.push_back({0, 0, 0});
            WasmDataSegment* segment = &module->data_segments.back();
            DecodeDataSegmentInModule(module, segment);
          }
          break;
        }
        case WasmSection::Code::FunctionTable: {
          // An indirect function table requires functions first.
          CheckForFunctions(module, section);
          int length;
          uint32_t function_table_count =
              consume_u32v(&length, "function table count");
          module->function_table.reserve(SafeReserve(function_table_count));
          // Decode function table.
          for (uint32_t i = 0; i < function_table_count; i++) {
            if (failed()) break;
            TRACE("DecodeFunctionTable[%d] module+%d\n", i,
                  static_cast<int>(pc_ - start_));
            uint16_t index = consume_u32v(&length);
            if (index >= module->functions.size()) {
              error(pc_ - 2, "invalid function index");
              break;
            }
            module->function_table.push_back(index);
          }
          break;
        }
        case WasmSection::Code::StartFunction: {
          // Declares a start function for a module.
          CheckForFunctions(module, section);
          if (module->start_function_index >= 0) {
            error("start function already declared");
            break;
          }
          WasmFunction* func;
          const byte* pos = pc_;
          module->start_function_index = consume_func_index(module, &func);
          if (func && func->sig->parameter_count() > 0) {
            error(pos, "invalid start function: non-zero parameter count");
            break;
          }
          break;
        }
        case WasmSection::Code::ImportTable: {
          // Declares an import table.
          CheckForPreviousSection(sections, WasmSection::Code::Signatures,
                                  true);
          int length;
          uint32_t import_table_count =
              consume_u32v(&length, "import table count");
          module->import_table.reserve(SafeReserve(import_table_count));
          // Decode import table.
          for (uint32_t i = 0; i < import_table_count; i++) {
            if (failed()) break;
            TRACE("DecodeImportTable[%d] module+%d\n", i,
                  static_cast<int>(pc_ - start_));

            module->import_table.push_back({nullptr, 0, 0});
            WasmImport* import = &module->import_table.back();

            import->sig_index = consume_sig_index(module, &import->sig);
            const byte* pos = pc_;
            import->module_name_offset = consume_string(
                &import->module_name_length, "import module name");
            if (import->module_name_length == 0) {
              error(pos, "import module name cannot be NULL");
            }
            import->function_name_offset = consume_string(
                &import->function_name_length, "import function name");
          }
          break;
        }
        case WasmSection::Code::ExportTable: {
          // Declares an export table.
          CheckForFunctions(module, section);
          int length;
          uint32_t export_table_count =
              consume_u32v(&length, "export table count");
          module->export_table.reserve(SafeReserve(export_table_count));
          // Decode export table.
          for (uint32_t i = 0; i < export_table_count; i++) {
            if (failed()) break;
            TRACE("DecodeExportTable[%d] module+%d\n", i,
                  static_cast<int>(pc_ - start_));

            module->export_table.push_back({0, 0});
            WasmExport* exp = &module->export_table.back();

            WasmFunction* func;
            exp->func_index = consume_func_index(module, &func);
            exp->name_offset = consume_string(&exp->name_length, "export name");
          }
          break;
        }
        case WasmSection::Code::Max:
          UNREACHABLE();  // Already skipped unknown sections.
      }
    }

  done:
    ModuleResult result = toResult(module);
    if (FLAG_dump_wasm_module) {
      DumpModule(module, result);
    }
    return result;
  }

  uint32_t SafeReserve(uint32_t count) {
    // Avoid OOM by only reserving up to a certain size.
    const uint32_t kMaxReserve = 20000;
    return count < kMaxReserve ? count : kMaxReserve;
  }

  void CheckForFunctions(WasmModule* module, WasmSection::Code section) {
    if (module->functions.size() == 0) {
      error(pc_ - 1, nullptr, "functions must appear before section %s",
            WasmSection::getName(section));
    }
  }

  void CheckForPreviousSection(bool* sections, WasmSection::Code section,
                               bool present) {
    if (section >= WasmSection::Code::Max) return;
    if (sections[(size_t)section] == present) return;
    if (present) {
      error(pc_ - 1, nullptr, "required %s section missing",
            WasmSection::getName(section));
    } else {
      error(pc_ - 1, nullptr, "%s section already present",
            WasmSection::getName(section));
    }
  }

  // Decodes a single anonymous function starting at {start_}.
  FunctionResult DecodeSingleFunction(ModuleEnv* module_env,
                                      WasmFunction* function) {
    pc_ = start_;
    function->sig = consume_sig();            // read signature
    function->name_offset = 0;                // ---- name
    function->name_length = 0;                // ---- name length
    function->code_start_offset = off(pc_);   // ---- code start
    function->code_end_offset = off(limit_);  // ---- code end
    function->exported = false;               // ---- exported
    function->external = false;               // ---- external

    if (ok()) VerifyFunctionBody(0, module_env, function);

    FunctionResult result;
    result.CopyFrom(result_);  // Copy error code and location.
    result.val = function;
    return result;
  }

  // Decodes a single function signature at {start}.
  FunctionSig* DecodeFunctionSignature(const byte* start) {
    pc_ = start;
    FunctionSig* result = consume_sig();
    return ok() ? result : nullptr;
  }

 private:
  Zone* module_zone;
  ModuleResult result_;
  ModuleOrigin origin_;

  uint32_t off(const byte* ptr) { return static_cast<uint32_t>(ptr - start_); }

  // Decodes a single global entry inside a module starting at {pc_}.
  void DecodeGlobalInModule(WasmGlobal* global) {
    global->name_offset = consume_string(&global->name_length, "global name");
    global->type = mem_type();
    global->offset = 0;
    global->exported = consume_u8("exported") != 0;
  }

  // Decodes a single function entry inside a module starting at {pc_}.
  // TODO(titzer): legacy function body; remove
  void DecodeFunctionInModule(WasmModule* module, WasmFunction* function,
                              bool verify_body = true) {
    byte decl_bits = consume_u8("function decl");

    const byte* sigpos = pc_;
    function->sig_index = consume_u16("signature index");

    if (function->sig_index >= module->signatures.size()) {
      return error(sigpos, "invalid signature index");
    } else {
      function->sig = module->signatures[function->sig_index];
    }

    TRACE("  +%d  <function attributes:%s%s%s%s%s>\n",
          static_cast<int>(pc_ - start_),
          decl_bits & kDeclFunctionName ? " name" : "",
          decl_bits & kDeclFunctionImport ? " imported" : "",
          decl_bits & kDeclFunctionLocals ? " locals" : "",
          decl_bits & kDeclFunctionExport ? " exported" : "",
          (decl_bits & kDeclFunctionImport) == 0 ? " body" : "");

    if (decl_bits & kDeclFunctionName) {
      function->name_offset =
          consume_string(&function->name_length, "function name");
    }

    function->exported = decl_bits & kDeclFunctionExport;

    // Imported functions have no locals or body.
    if (decl_bits & kDeclFunctionImport) {
      function->external = true;
      return;
    }

    if (decl_bits & kDeclFunctionLocals) {
      function->local_i32_count = consume_u16("i32 count");
      function->local_i64_count = consume_u16("i64 count");
      function->local_f32_count = consume_u16("f32 count");
      function->local_f64_count = consume_u16("f64 count");
    }

    uint16_t size = consume_u16("body size");
    if (ok()) {
      if ((pc_ + size) > limit_) {
        return error(pc_, limit_,
                     "expected %d bytes for function body, fell off end", size);
      }
      function->code_start_offset = static_cast<uint32_t>(pc_ - start_);
      function->code_end_offset = function->code_start_offset + size;
      TRACE("  +%d  %-20s: (%d bytes)\n", static_cast<int>(pc_ - start_),
            "function body", size);
      pc_ += size;
    }
  }

  bool IsWithinLimit(uint32_t limit, uint32_t offset, uint32_t size) {
    if (offset > limit) return false;
    if ((offset + size) < offset) return false;  // overflow
    return (offset + size) <= limit;
  }

  // Decodes a single data segment entry inside a module starting at {pc_}.
  void DecodeDataSegmentInModule(WasmModule* module, WasmDataSegment* segment) {
    const byte* start = pc_;
    int length;
    segment->dest_addr = consume_u32v(&length, "destination");
    segment->source_size = consume_u32v(&length, "source size");
    segment->source_offset = static_cast<uint32_t>(pc_ - start_);
    segment->init = true;

    // Validate the data is in the module.
    uint32_t module_limit = static_cast<uint32_t>(limit_ - start_);
    if (!IsWithinLimit(module_limit, segment->source_offset,
                       segment->source_size)) {
      error(start, "segment out of bounds of module");
    }

    // Validate that the segment will fit into the (minimum) memory.
    uint32_t memory_limit =
        WasmModule::kPageSize * (module ? module->min_mem_pages
                                        : WasmModule::kMaxMemPages);
    if (!IsWithinLimit(memory_limit, segment->dest_addr,
                       segment->source_size)) {
      error(start, "segment out of bounds of memory");
    }

    consume_bytes(segment->source_size);
  }

  // Verifies the body (code) of a given function.
  void VerifyFunctionBody(uint32_t func_num, ModuleEnv* menv,
                          WasmFunction* function) {
    if (FLAG_trace_wasm_decode_time) {
      OFStream os(stdout);
      os << "Verifying WASM function " << WasmFunctionName(function, menv)
         << std::endl;
      os << std::endl;
    }
    FunctionBody body = {menv, function->sig, start_,
                         start_ + function->code_start_offset,
                         start_ + function->code_end_offset};
    TreeResult result = VerifyWasmCode(module_zone->allocator(), body);
    if (result.failed()) {
      // Wrap the error message from the function decoder.
      std::ostringstream str;
      str << "in function " << WasmFunctionName(function, menv) << ": ";
      str << result;
      std::string strval = str.str();
      const char* raw = strval.c_str();
      size_t len = strlen(raw);
      char* buffer = new char[len];
      strncpy(buffer, raw, len);
      buffer[len - 1] = 0;

      // Copy error code and location.
      result_.CopyFrom(result);
      result_.error_msg.Reset(buffer);
    }
  }

  // Reads a single 32-bit unsigned integer interpreted as an offset, checking
  // the offset is within bounds and advances.
  uint32_t consume_offset(const char* name = nullptr) {
    uint32_t offset = consume_u32(name ? name : "offset");
    if (offset > static_cast<uint32_t>(limit_ - start_)) {
      error(pc_ - sizeof(uint32_t), "offset out of bounds of module");
    }
    return offset;
  }

  // Reads a length-prefixed string, checking that it is within bounds. Returns
  // the offset of the string, and the length as an out parameter.
  uint32_t consume_string(uint32_t* length, const char* name = nullptr) {
    int varint_length;
    *length = consume_u32v(&varint_length, "string length");
    uint32_t offset = pc_offset();
    TRACE("  +%u  %-20s: (%u bytes)\n", offset, "string", *length);
    consume_bytes(*length);
    return offset;
  }

  uint32_t consume_sig_index(WasmModule* module, FunctionSig** sig) {
    const byte* pos = pc_;
    int length;
    uint32_t sig_index = consume_u32v(&length, "signature index");
    if (sig_index >= module->signatures.size()) {
      error(pos, pos, "signature index %u out of bounds (%d signatures)",
            sig_index, static_cast<int>(module->signatures.size()));
      *sig = nullptr;
      return 0;
    }
    *sig = module->signatures[sig_index];
    return sig_index;
  }

  uint32_t consume_func_index(WasmModule* module, WasmFunction** func) {
    const byte* pos = pc_;
    int length;
    uint32_t func_index = consume_u32v(&length, "function index");
    if (func_index >= module->functions.size()) {
      error(pos, pos, "function index %u out of bounds (%d functions)",
            func_index, static_cast<int>(module->functions.size()));
      *func = nullptr;
      return 0;
    }
    *func = &module->functions[func_index];
    return func_index;
  }

  // Reads a section name.
  WasmSection::Code consume_section_name(int* string_leb_length,
                                         uint32_t* string_length) {
    *string_length = consume_u32v(string_leb_length, "name length");
    const byte* start = pc_;
    consume_bytes(*string_length);
    if (failed()) {
      TRACE("Section name of length %u couldn't be read\n", *string_length);
      return WasmSection::Code::Max;
    }
    // TODO(jfb) Linear search, it may be better to do a common-prefix search.
    for (WasmSection::Code i = WasmSection::begin(); i != WasmSection::end();
         i = WasmSection::next(i)) {
      if (WasmSection::getNameLength(i) == *string_length &&
          0 == memcmp(WasmSection::getName(i), start, *string_length)) {
        return i;
      }
    }
    TRACE("Unknown section: '");
    for (uint32_t i = 0; i != *string_length; ++i) TRACE("%c", *(start + i));
    TRACE("'\n");
    return WasmSection::Code::Max;
  }

  // Reads a single 8-bit integer, interpreting it as a local type.
  LocalType consume_local_type() {
    byte val = consume_u8("local type");
    LocalTypeCode t = static_cast<LocalTypeCode>(val);
    switch (t) {
      case kLocalVoid:
        return kAstStmt;
      case kLocalI32:
        return kAstI32;
      case kLocalI64:
        return kAstI64;
      case kLocalF32:
        return kAstF32;
      case kLocalF64:
        return kAstF64;
      default:
        error(pc_ - 1, "invalid local type");
        return kAstStmt;
    }
  }

  // Reads a single 8-bit integer, interpreting it as a memory type.
  MachineType mem_type() {
    byte val = consume_u8("memory type");
    MemTypeCode t = static_cast<MemTypeCode>(val);
    switch (t) {
      case kMemI8:
        return MachineType::Int8();
      case kMemU8:
        return MachineType::Uint8();
      case kMemI16:
        return MachineType::Int16();
      case kMemU16:
        return MachineType::Uint16();
      case kMemI32:
        return MachineType::Int32();
      case kMemU32:
        return MachineType::Uint32();
      case kMemI64:
        return MachineType::Int64();
      case kMemU64:
        return MachineType::Uint64();
      case kMemF32:
        return MachineType::Float32();
      case kMemF64:
        return MachineType::Float64();
      default:
        error(pc_ - 1, "invalid memory type");
        return MachineType::None();
    }
  }

  // Parses an inline function signature.
  FunctionSig* consume_sig() {
    int length;
    byte count = consume_u32v(&length, "param count");
    LocalType ret = consume_local_type();
    FunctionSig::Builder builder(module_zone, ret == kAstStmt ? 0 : 1, count);
    if (ret != kAstStmt) builder.AddReturn(ret);

    for (int i = 0; i < count; i++) {
      LocalType param = consume_local_type();
      if (param == kAstStmt) error(pc_ - 1, "invalid void parameter type");
      builder.AddParam(param);
    }
    return builder.Build();
  }
};


// Helpers for nice error messages.
class ModuleError : public ModuleResult {
 public:
  explicit ModuleError(const char* msg) {
    error_code = kError;
    size_t len = strlen(msg) + 1;
    char* result = new char[len];
    strncpy(result, msg, len);
    result[len - 1] = 0;
    error_msg.Reset(result);
  }
};


// Helpers for nice error messages.
class FunctionError : public FunctionResult {
 public:
  explicit FunctionError(const char* msg) {
    error_code = kError;
    size_t len = strlen(msg) + 1;
    char* result = new char[len];
    strncpy(result, msg, len);
    result[len - 1] = 0;
    error_msg.Reset(result);
  }
};

ModuleResult DecodeWasmModule(Isolate* isolate, Zone* zone,
                              const byte* module_start, const byte* module_end,
                              bool verify_functions, ModuleOrigin origin) {
  size_t size = module_end - module_start;
  if (module_start > module_end) return ModuleError("start > end");
  if (size >= kMaxModuleSize) return ModuleError("size > maximum module size");
  WasmModule* module = new WasmModule();
  ModuleDecoder decoder(zone, module_start, module_end, origin);
  return decoder.DecodeModule(module, verify_functions);
}


FunctionSig* DecodeWasmSignatureForTesting(Zone* zone, const byte* start,
                                           const byte* end) {
  ModuleDecoder decoder(zone, start, end, kWasmOrigin);
  return decoder.DecodeFunctionSignature(start);
}


FunctionResult DecodeWasmFunction(Isolate* isolate, Zone* zone,
                                  ModuleEnv* module_env,
                                  const byte* function_start,
                                  const byte* function_end) {
  size_t size = function_end - function_start;
  if (function_start > function_end) return FunctionError("start > end");
  if (size > kMaxFunctionSize)
    return FunctionError("size > maximum function size");
  WasmFunction* function = new WasmFunction();
  ModuleDecoder decoder(zone, function_start, function_end, kWasmOrigin);
  return decoder.DecodeSingleFunction(module_env, function);
}
}  // namespace wasm
}  // namespace internal
}  // namespace v8
