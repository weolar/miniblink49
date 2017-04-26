// Copyright 2015 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_WASM_MODULE_DECODER_H_
#define V8_WASM_MODULE_DECODER_H_

#include "src/wasm/ast-decoder.h"
#include "src/wasm/wasm-module.h"

namespace v8 {
namespace internal {
namespace wasm {
// Decodes the bytes of a WASM module between {module_start} and {module_end}.
ModuleResult DecodeWasmModule(Isolate* isolate, Zone* zone,
                              const byte* module_start, const byte* module_end,
                              bool verify_functions, ModuleOrigin origin);

// Exposed for testing. Decodes a single function signature, allocating it
// in the given zone. Returns {nullptr} upon failure.
FunctionSig* DecodeWasmSignatureForTesting(Zone* zone, const byte* start,
                                           const byte* end);

// Decodes the bytes of a WASM function between
// {function_start} and {function_end}.
FunctionResult DecodeWasmFunction(Isolate* isolate, Zone* zone, ModuleEnv* env,
                                  const byte* function_start,
                                  const byte* function_end);
}  // namespace wasm
}  // namespace internal
}  // namespace v8

#endif  // V8_WASM_MODULE_DECODER_H_
