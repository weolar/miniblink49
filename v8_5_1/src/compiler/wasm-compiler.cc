// Copyright 2015 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/compiler/wasm-compiler.h"

#include "src/isolate-inl.h"

#include "src/base/platform/elapsed-timer.h"
#include "src/base/platform/platform.h"

#include "src/compiler/access-builder.h"
#include "src/compiler/change-lowering.h"
#include "src/compiler/common-operator.h"
#include "src/compiler/diamond.h"
#include "src/compiler/graph.h"
#include "src/compiler/graph-visualizer.h"
#include "src/compiler/instruction-selector.h"
#include "src/compiler/int64-lowering.h"
#include "src/compiler/js-generic-lowering.h"
#include "src/compiler/js-graph.h"
#include "src/compiler/js-operator.h"
#include "src/compiler/linkage.h"
#include "src/compiler/machine-operator.h"
#include "src/compiler/node-matchers.h"
#include "src/compiler/pipeline.h"
#include "src/compiler/simplified-lowering.h"
#include "src/compiler/simplified-operator.h"
#include "src/compiler/source-position.h"
#include "src/compiler/typer.h"

#include "src/code-factory.h"
#include "src/code-stubs.h"
#include "src/factory.h"
#include "src/log-inl.h"
#include "src/profiler/cpu-profiler.h"

#include "src/wasm/ast-decoder.h"
#include "src/wasm/wasm-module.h"
#include "src/wasm/wasm-opcodes.h"

// TODO(titzer): pull WASM_64 up to a common header.
#if !V8_TARGET_ARCH_32_BIT || V8_TARGET_ARCH_X64
#define WASM_64 1
#else
#define WASM_64 0
#endif

namespace v8 {
namespace internal {
namespace compiler {

namespace {
const Operator* UnsupportedOpcode(wasm::WasmOpcode opcode) {
  if (wasm::WasmOpcodes::IsSupported(opcode)) {
    V8_Fatal(__FILE__, __LINE__,
             "Unsupported opcode #%d:%s reported as supported", opcode,
             wasm::WasmOpcodes::OpcodeName(opcode));
  }
  V8_Fatal(__FILE__, __LINE__, "Unsupported opcode #%d:%s", opcode,
           wasm::WasmOpcodes::OpcodeName(opcode));
  return nullptr;
}


void MergeControlToEnd(JSGraph* jsgraph, Node* node) {
  Graph* g = jsgraph->graph();
  if (g->end()) {
    NodeProperties::MergeControlToEnd(g, jsgraph->common(), node);
  } else {
    g->SetEnd(g->NewNode(jsgraph->common()->End(1), node));
  }
}

}  // namespace

// A helper that handles building graph fragments for trapping.
// To avoid generating a ton of redundant code that just calls the runtime
// to trap, we generate a per-trap-reason block of code that all trap sites
// in this function will branch to.
class WasmTrapHelper : public ZoneObject {
 public:
  explicit WasmTrapHelper(WasmGraphBuilder* builder)
      : builder_(builder),
        jsgraph_(builder->jsgraph()),
        graph_(builder->jsgraph() ? builder->jsgraph()->graph() : nullptr) {
    for (int i = 0; i < wasm::kTrapCount; i++) traps_[i] = nullptr;
  }

  // Make the current control path trap to unreachable.
  void Unreachable() { ConnectTrap(wasm::kTrapUnreachable); }

  // Always trap with the given reason.
  void TrapAlways(wasm::TrapReason reason) { ConnectTrap(reason); }

  // Add a check that traps if {node} is equal to {val}.
  Node* TrapIfEq32(wasm::TrapReason reason, Node* node, int32_t val) {
    Int32Matcher m(node);
    if (m.HasValue() && !m.Is(val)) return graph()->start();
    if (val == 0) {
      AddTrapIfFalse(reason, node);
    } else {
      AddTrapIfTrue(reason,
                    graph()->NewNode(jsgraph()->machine()->Word32Equal(), node,
                                     jsgraph()->Int32Constant(val)));
    }
    return builder_->Control();
  }

  // Add a check that traps if {node} is zero.
  Node* ZeroCheck32(wasm::TrapReason reason, Node* node) {
    return TrapIfEq32(reason, node, 0);
  }

  // Add a check that traps if {node} is equal to {val}.
  Node* TrapIfEq64(wasm::TrapReason reason, Node* node, int64_t val) {
    Int64Matcher m(node);
    if (m.HasValue() && !m.Is(val)) return graph()->start();
    AddTrapIfTrue(reason,
                  graph()->NewNode(jsgraph()->machine()->Word64Equal(), node,
                                   jsgraph()->Int64Constant(val)));
    return builder_->Control();
  }

  // Add a check that traps if {node} is zero.
  Node* ZeroCheck64(wasm::TrapReason reason, Node* node) {
    return TrapIfEq64(reason, node, 0);
  }

  // Add a trap if {cond} is true.
  void AddTrapIfTrue(wasm::TrapReason reason, Node* cond) {
    AddTrapIf(reason, cond, true);
  }

  // Add a trap if {cond} is false.
  void AddTrapIfFalse(wasm::TrapReason reason, Node* cond) {
    AddTrapIf(reason, cond, false);
  }

  // Add a trap if {cond} is true or false according to {iftrue}.
  void AddTrapIf(wasm::TrapReason reason, Node* cond, bool iftrue) {
    Node** effect_ptr = builder_->effect_;
    Node** control_ptr = builder_->control_;
    Node* before = *effect_ptr;
    BranchHint hint = iftrue ? BranchHint::kFalse : BranchHint::kTrue;
    Node* branch = graph()->NewNode(common()->Branch(hint), cond, *control_ptr);
    Node* if_true = graph()->NewNode(common()->IfTrue(), branch);
    Node* if_false = graph()->NewNode(common()->IfFalse(), branch);

    *control_ptr = iftrue ? if_true : if_false;
    ConnectTrap(reason);
    *control_ptr = iftrue ? if_false : if_true;
    *effect_ptr = before;
  }

  Node* GetTrapValue(wasm::FunctionSig* sig) {
    if (sig->return_count() > 0) {
      switch (sig->GetReturn()) {
        case wasm::kAstI32:
          return jsgraph()->Int32Constant(0xdeadbeef);
        case wasm::kAstI64:
          return jsgraph()->Int64Constant(0xdeadbeefdeadbeef);
        case wasm::kAstF32:
          return jsgraph()->Float32Constant(bit_cast<float>(0xdeadbeef));
        case wasm::kAstF64:
          return jsgraph()->Float64Constant(
              bit_cast<double>(0xdeadbeefdeadbeef));
          break;
        default:
          UNREACHABLE();
          return nullptr;
      }
    } else {
      return jsgraph()->Int32Constant(0xdeadbeef);
    }
  }

 private:
  WasmGraphBuilder* builder_;
  JSGraph* jsgraph_;
  Graph* graph_;
  Node* traps_[wasm::kTrapCount];
  Node* effects_[wasm::kTrapCount];

  JSGraph* jsgraph() { return jsgraph_; }
  Graph* graph() { return jsgraph_->graph(); }
  CommonOperatorBuilder* common() { return jsgraph()->common(); }

  void ConnectTrap(wasm::TrapReason reason) {
    if (traps_[reason] == nullptr) {
      // Create trap code for the first time this trap is used.
      return BuildTrapCode(reason);
    }
    // Connect the current control and effect to the existing trap code.
    builder_->AppendToMerge(traps_[reason], builder_->Control());
    builder_->AppendToPhi(traps_[reason], effects_[reason], builder_->Effect());
  }

  void BuildTrapCode(wasm::TrapReason reason) {
    Node* exception =
        builder_->String(wasm::WasmOpcodes::TrapReasonName(reason));
    Node* end;
    Node** control_ptr = builder_->control_;
    Node** effect_ptr = builder_->effect_;
    wasm::ModuleEnv* module = builder_->module_;
    *control_ptr = traps_[reason] =
        graph()->NewNode(common()->Merge(1), *control_ptr);
    *effect_ptr = effects_[reason] =
        graph()->NewNode(common()->EffectPhi(1), *effect_ptr, *control_ptr);

    if (module && !module->instance->context.is_null()) {
      // Use the module context to call the runtime to throw an exception.
      Runtime::FunctionId f = Runtime::kThrow;
      const Runtime::Function* fun = Runtime::FunctionForId(f);
      CallDescriptor* desc = Linkage::GetRuntimeCallDescriptor(
          jsgraph()->zone(), f, fun->nargs, Operator::kNoProperties,
          CallDescriptor::kNoFlags);
      Node* inputs[] = {
          jsgraph()->CEntryStubConstant(fun->result_size),  // C entry
          exception,                                        // exception
          jsgraph()->ExternalConstant(
              ExternalReference(f, jsgraph()->isolate())),  // ref
          jsgraph()->Int32Constant(fun->nargs),             // arity
          jsgraph()->Constant(module->instance->context),   // context
          *effect_ptr,
          *control_ptr};

      Node* node = graph()->NewNode(
          common()->Call(desc), static_cast<int>(arraysize(inputs)), inputs);
      *control_ptr = node;
      *effect_ptr = node;
    }
    if (false) {
      // End the control flow with a throw
      Node* thrw =
          graph()->NewNode(common()->Throw(), jsgraph()->ZeroConstant(),
                           *effect_ptr, *control_ptr);
      end = thrw;
    } else {
      // End the control flow with returning 0xdeadbeef
      Node* ret_value = GetTrapValue(builder_->GetFunctionSignature());
      end = graph()->NewNode(jsgraph()->common()->Return(), ret_value,
                             *effect_ptr, *control_ptr);
    }

    MergeControlToEnd(jsgraph(), end);
  }
};

WasmGraphBuilder::WasmGraphBuilder(Zone* zone, JSGraph* jsgraph,
                                   wasm::FunctionSig* function_signature)
    : zone_(zone),
      jsgraph_(jsgraph),
      module_(nullptr),
      mem_buffer_(nullptr),
      mem_size_(nullptr),
      function_table_(nullptr),
      control_(nullptr),
      effect_(nullptr),
      cur_buffer_(def_buffer_),
      cur_bufsize_(kDefaultBufferSize),
      trap_(new (zone) WasmTrapHelper(this)),
      function_signature_(function_signature) {
  DCHECK_NOT_NULL(jsgraph_);
}


Node* WasmGraphBuilder::Error() { return jsgraph()->Dead(); }


Node* WasmGraphBuilder::Start(unsigned params) {
  Node* start = graph()->NewNode(jsgraph()->common()->Start(params));
  graph()->SetStart(start);
  return start;
}


Node* WasmGraphBuilder::Param(unsigned index, wasm::LocalType type) {
  return graph()->NewNode(jsgraph()->common()->Parameter(index),
                          graph()->start());
}


Node* WasmGraphBuilder::Loop(Node* entry) {
  return graph()->NewNode(jsgraph()->common()->Loop(1), entry);
}


Node* WasmGraphBuilder::Terminate(Node* effect, Node* control) {
  Node* terminate =
      graph()->NewNode(jsgraph()->common()->Terminate(), effect, control);
  MergeControlToEnd(jsgraph(), terminate);
  return terminate;
}


unsigned WasmGraphBuilder::InputCount(Node* node) {
  return static_cast<unsigned>(node->InputCount());
}


bool WasmGraphBuilder::IsPhiWithMerge(Node* phi, Node* merge) {
  return phi && IrOpcode::IsPhiOpcode(phi->opcode()) &&
         NodeProperties::GetControlInput(phi) == merge;
}


void WasmGraphBuilder::AppendToMerge(Node* merge, Node* from) {
  DCHECK(IrOpcode::IsMergeOpcode(merge->opcode()));
  merge->AppendInput(jsgraph()->zone(), from);
  int new_size = merge->InputCount();
  NodeProperties::ChangeOp(
      merge, jsgraph()->common()->ResizeMergeOrPhi(merge->op(), new_size));
}


void WasmGraphBuilder::AppendToPhi(Node* merge, Node* phi, Node* from) {
  DCHECK(IrOpcode::IsPhiOpcode(phi->opcode()));
  DCHECK(IrOpcode::IsMergeOpcode(merge->opcode()));
  int new_size = phi->InputCount();
  phi->InsertInput(jsgraph()->zone(), phi->InputCount() - 1, from);
  NodeProperties::ChangeOp(
      phi, jsgraph()->common()->ResizeMergeOrPhi(phi->op(), new_size));
}


Node* WasmGraphBuilder::Merge(unsigned count, Node** controls) {
  return graph()->NewNode(jsgraph()->common()->Merge(count), count, controls);
}


Node* WasmGraphBuilder::Phi(wasm::LocalType type, unsigned count, Node** vals,
                            Node* control) {
  DCHECK(IrOpcode::IsMergeOpcode(control->opcode()));
  Node** buf = Realloc(vals, count, count + 1);
  buf[count] = control;
  return graph()->NewNode(jsgraph()->common()->Phi(type, count), count + 1,
                          buf);
}


Node* WasmGraphBuilder::EffectPhi(unsigned count, Node** effects,
                                  Node* control) {
  DCHECK(IrOpcode::IsMergeOpcode(control->opcode()));
  Node** buf = Realloc(effects, count, count + 1);
  buf[count] = control;
  return graph()->NewNode(jsgraph()->common()->EffectPhi(count), count + 1,
                          buf);
}


Node* WasmGraphBuilder::Int32Constant(int32_t value) {
  return jsgraph()->Int32Constant(value);
}


Node* WasmGraphBuilder::Int64Constant(int64_t value) {
  return jsgraph()->Int64Constant(value);
}


Node* WasmGraphBuilder::Binop(wasm::WasmOpcode opcode, Node* left,
                              Node* right) {
  const Operator* op;
  MachineOperatorBuilder* m = jsgraph()->machine();
  switch (opcode) {
    case wasm::kExprI32Add:
      op = m->Int32Add();
      break;
    case wasm::kExprI32Sub:
      op = m->Int32Sub();
      break;
    case wasm::kExprI32Mul:
      op = m->Int32Mul();
      break;
    case wasm::kExprI32DivS:
      return BuildI32DivS(left, right);
    case wasm::kExprI32DivU:
      return BuildI32DivU(left, right);
    case wasm::kExprI32RemS:
      return BuildI32RemS(left, right);
    case wasm::kExprI32RemU:
      return BuildI32RemU(left, right);
    case wasm::kExprI32And:
      op = m->Word32And();
      break;
    case wasm::kExprI32Ior:
      op = m->Word32Or();
      break;
    case wasm::kExprI32Xor:
      op = m->Word32Xor();
      break;
    case wasm::kExprI32Shl:
      op = m->Word32Shl();
      right = MaskShiftCount32(right);
      break;
    case wasm::kExprI32ShrU:
      op = m->Word32Shr();
      right = MaskShiftCount32(right);
      break;
    case wasm::kExprI32ShrS:
      op = m->Word32Sar();
      right = MaskShiftCount32(right);
      break;
    case wasm::kExprI32Ror:
      op = m->Word32Ror();
      right = MaskShiftCount32(right);
      break;
    case wasm::kExprI32Rol:
      right = MaskShiftCount32(right);
      return BuildI32Rol(left, right);
    case wasm::kExprI32Eq:
      op = m->Word32Equal();
      break;
    case wasm::kExprI32Ne:
      return Invert(Binop(wasm::kExprI32Eq, left, right));
    case wasm::kExprI32LtS:
      op = m->Int32LessThan();
      break;
    case wasm::kExprI32LeS:
      op = m->Int32LessThanOrEqual();
      break;
    case wasm::kExprI32LtU:
      op = m->Uint32LessThan();
      break;
    case wasm::kExprI32LeU:
      op = m->Uint32LessThanOrEqual();
      break;
    case wasm::kExprI32GtS:
      op = m->Int32LessThan();
      std::swap(left, right);
      break;
    case wasm::kExprI32GeS:
      op = m->Int32LessThanOrEqual();
      std::swap(left, right);
      break;
    case wasm::kExprI32GtU:
      op = m->Uint32LessThan();
      std::swap(left, right);
      break;
    case wasm::kExprI32GeU:
      op = m->Uint32LessThanOrEqual();
      std::swap(left, right);
      break;
    case wasm::kExprI64And:
      op = m->Word64And();
      break;
    // todo(ahaas): I added a list of missing instructions here to make merging
    // easier when I do them one by one.
    // kExprI64Add:
    case wasm::kExprI64Add:
      op = m->Int64Add();
      break;
    // kExprI64Sub:
    case wasm::kExprI64Sub:
      op = m->Int64Sub();
      break;
    // kExprI64Mul:
    case wasm::kExprI64Mul:
      op = m->Int64Mul();
      break;
    // kExprI64DivS:
    case wasm::kExprI64DivS:
      return BuildI64DivS(left, right);
    // kExprI64DivU:
    case wasm::kExprI64DivU:
      return BuildI64DivU(left, right);
    // kExprI64RemS:
    case wasm::kExprI64RemS:
      return BuildI64RemS(left, right);
    // kExprI64RemU:
    case wasm::kExprI64RemU:
      return BuildI64RemU(left, right);
    case wasm::kExprI64Ior:
      op = m->Word64Or();
      break;
// kExprI64Xor:
    case wasm::kExprI64Xor:
      op = m->Word64Xor();
      break;
// kExprI64Shl:
    case wasm::kExprI64Shl:
      op = m->Word64Shl();
      right = MaskShiftCount64(right);
      break;
    // kExprI64ShrU:
    case wasm::kExprI64ShrU:
      op = m->Word64Shr();
      right = MaskShiftCount64(right);
      break;
    // kExprI64ShrS:
    case wasm::kExprI64ShrS:
      op = m->Word64Sar();
      right = MaskShiftCount64(right);
      break;
    // kExprI64Eq:
    case wasm::kExprI64Eq:
      op = m->Word64Equal();
      break;
// kExprI64Ne:
    case wasm::kExprI64Ne:
      return Invert(Binop(wasm::kExprI64Eq, left, right));
// kExprI64LtS:
    case wasm::kExprI64LtS:
      op = m->Int64LessThan();
      break;
    case wasm::kExprI64LeS:
      op = m->Int64LessThanOrEqual();
      break;
    case wasm::kExprI64LtU:
      op = m->Uint64LessThan();
      break;
    case wasm::kExprI64LeU:
      op = m->Uint64LessThanOrEqual();
      break;
    case wasm::kExprI64GtS:
      op = m->Int64LessThan();
      std::swap(left, right);
      break;
    case wasm::kExprI64GeS:
      op = m->Int64LessThanOrEqual();
      std::swap(left, right);
      break;
    case wasm::kExprI64GtU:
      op = m->Uint64LessThan();
      std::swap(left, right);
      break;
    case wasm::kExprI64GeU:
      op = m->Uint64LessThanOrEqual();
      std::swap(left, right);
      break;
    case wasm::kExprI64Ror:
      op = m->Word64Ror();
      right = MaskShiftCount64(right);
      break;
    case wasm::kExprI64Rol:
      return BuildI64Rol(left, right);
    case wasm::kExprF32CopySign:
      return BuildF32CopySign(left, right);
    case wasm::kExprF64CopySign:
      return BuildF64CopySign(left, right);
    case wasm::kExprF32Add:
      op = m->Float32Add();
      break;
    case wasm::kExprF32Sub:
      op = m->Float32Sub();
      break;
    case wasm::kExprF32Mul:
      op = m->Float32Mul();
      break;
    case wasm::kExprF32Div:
      op = m->Float32Div();
      break;
    case wasm::kExprF32Eq:
      op = m->Float32Equal();
      break;
    case wasm::kExprF32Ne:
      return Invert(Binop(wasm::kExprF32Eq, left, right));
    case wasm::kExprF32Lt:
      op = m->Float32LessThan();
      break;
    case wasm::kExprF32Ge:
      op = m->Float32LessThanOrEqual();
      std::swap(left, right);
      break;
    case wasm::kExprF32Gt:
      op = m->Float32LessThan();
      std::swap(left, right);
      break;
    case wasm::kExprF32Le:
      op = m->Float32LessThanOrEqual();
      break;
    case wasm::kExprF64Add:
      op = m->Float64Add();
      break;
    case wasm::kExprF64Sub:
      op = m->Float64Sub();
      break;
    case wasm::kExprF64Mul:
      op = m->Float64Mul();
      break;
    case wasm::kExprF64Div:
      op = m->Float64Div();
      break;
    case wasm::kExprF64Eq:
      op = m->Float64Equal();
      break;
    case wasm::kExprF64Ne:
      return Invert(Binop(wasm::kExprF64Eq, left, right));
    case wasm::kExprF64Lt:
      op = m->Float64LessThan();
      break;
    case wasm::kExprF64Le:
      op = m->Float64LessThanOrEqual();
      break;
    case wasm::kExprF64Gt:
      op = m->Float64LessThan();
      std::swap(left, right);
      break;
    case wasm::kExprF64Ge:
      op = m->Float64LessThanOrEqual();
      std::swap(left, right);
      break;
    case wasm::kExprF32Min:
      return BuildF32Min(left, right);
    case wasm::kExprF64Min:
      return BuildF64Min(left, right);
    case wasm::kExprF32Max:
      return BuildF32Max(left, right);
    case wasm::kExprF64Max:
      return BuildF64Max(left, right);
    case wasm::kExprF64Pow: {
      return BuildF64Pow(left, right);
    }
    case wasm::kExprF64Atan2: {
      return BuildF64Atan2(left, right);
    }
    case wasm::kExprF64Mod: {
      return BuildF64Mod(left, right);
    }
    default:
      op = UnsupportedOpcode(opcode);
  }
  return graph()->NewNode(op, left, right);
}


Node* WasmGraphBuilder::Unop(wasm::WasmOpcode opcode, Node* input) {
  const Operator* op;
  MachineOperatorBuilder* m = jsgraph()->machine();
  switch (opcode) {
    case wasm::kExprI32Eqz:
      op = m->Word32Equal();
      return graph()->NewNode(op, input, jsgraph()->Int32Constant(0));
    case wasm::kExprF32Abs:
      op = m->Float32Abs();
      break;
    case wasm::kExprF32Neg:
      return BuildF32Neg(input);
    case wasm::kExprF32Sqrt:
      op = m->Float32Sqrt();
      break;
    case wasm::kExprF64Abs:
      op = m->Float64Abs();
      break;
    case wasm::kExprF64Neg:
      return BuildF64Neg(input);
    case wasm::kExprF64Sqrt:
      op = m->Float64Sqrt();
      break;
    case wasm::kExprI32SConvertF64:
      return BuildI32SConvertF64(input);
    case wasm::kExprI32UConvertF64:
      return BuildI32UConvertF64(input);
    case wasm::kExprF32ConvertF64:
      op = m->TruncateFloat64ToFloat32();
      break;
    case wasm::kExprF64SConvertI32:
      op = m->ChangeInt32ToFloat64();
      break;
    case wasm::kExprF64UConvertI32:
      op = m->ChangeUint32ToFloat64();
      break;
    case wasm::kExprF32SConvertI32:
      op = m->RoundInt32ToFloat32();
      break;
    case wasm::kExprF32UConvertI32:
      op = m->RoundUint32ToFloat32();
      break;
    case wasm::kExprI32SConvertF32:
      return BuildI32SConvertF32(input);
    case wasm::kExprI32UConvertF32:
      return BuildI32UConvertF32(input);
    case wasm::kExprF64ConvertF32:
      op = m->ChangeFloat32ToFloat64();
      break;
    case wasm::kExprF32ReinterpretI32:
      op = m->BitcastInt32ToFloat32();
      break;
    case wasm::kExprI32ReinterpretF32:
      op = m->BitcastFloat32ToInt32();
      break;
    case wasm::kExprI32Clz:
      op = m->Word32Clz();
      break;
    case wasm::kExprI32Ctz: {
      if (m->Word32Ctz().IsSupported()) {
        op = m->Word32Ctz().op();
        break;
      } else if (m->Word32ReverseBits().IsSupported()) {
        Node* reversed = graph()->NewNode(m->Word32ReverseBits().op(), input);
        Node* result = graph()->NewNode(m->Word32Clz(), reversed);
        return result;
      } else {
        return BuildI32Ctz(input);
      }
    }
    case wasm::kExprI32Popcnt: {
      if (m->Word32Popcnt().IsSupported()) {
        op = m->Word32Popcnt().op();
        break;
      } else {
        return BuildI32Popcnt(input);
      }
    }
    case wasm::kExprF32Floor: {
      if (!m->Float32RoundDown().IsSupported()) return BuildF32Floor(input);
      op = m->Float32RoundDown().op();
      break;
    }
    case wasm::kExprF32Ceil: {
      if (!m->Float32RoundUp().IsSupported()) return BuildF32Ceil(input);
      op = m->Float32RoundUp().op();
      break;
    }
    case wasm::kExprF32Trunc: {
      if (!m->Float32RoundTruncate().IsSupported()) return BuildF32Trunc(input);
      op = m->Float32RoundTruncate().op();
      break;
    }
    case wasm::kExprF32NearestInt: {
      if (!m->Float32RoundTiesEven().IsSupported())
        return BuildF32NearestInt(input);
      op = m->Float32RoundTiesEven().op();
      break;
    }
    case wasm::kExprF64Floor: {
      if (!m->Float64RoundDown().IsSupported()) return BuildF64Floor(input);
      op = m->Float64RoundDown().op();
      break;
    }
    case wasm::kExprF64Ceil: {
      if (!m->Float64RoundUp().IsSupported()) return BuildF64Ceil(input);
      op = m->Float64RoundUp().op();
      break;
    }
    case wasm::kExprF64Trunc: {
      if (!m->Float64RoundTruncate().IsSupported()) return BuildF64Trunc(input);
      op = m->Float64RoundTruncate().op();
      break;
    }
    case wasm::kExprF64NearestInt: {
      if (!m->Float64RoundTiesEven().IsSupported())
        return BuildF64NearestInt(input);
      op = m->Float64RoundTiesEven().op();
      break;
    }
    case wasm::kExprF64Acos: {
      return BuildF64Acos(input);
    }
    case wasm::kExprF64Asin: {
      return BuildF64Asin(input);
    }
    case wasm::kExprF64Atan: {
      return BuildF64Atan(input);
    }
    case wasm::kExprF64Cos: {
      return BuildF64Cos(input);
    }
    case wasm::kExprF64Sin: {
      return BuildF64Sin(input);
    }
    case wasm::kExprF64Tan: {
      return BuildF64Tan(input);
    }
    case wasm::kExprF64Exp: {
      return BuildF64Exp(input);
    }
    case wasm::kExprF64Log: {
      return BuildF64Log(input);
    }
    // kExprI32ConvertI64:
    case wasm::kExprI32ConvertI64:
      op = m->TruncateInt64ToInt32();
      break;
    // kExprI64SConvertI32:
    case wasm::kExprI64SConvertI32:
      op = m->ChangeInt32ToInt64();
      break;
    // kExprI64UConvertI32:
    case wasm::kExprI64UConvertI32:
      op = m->ChangeUint32ToUint64();
      break;
    // kExprF64ReinterpretI64:
    case wasm::kExprF64ReinterpretI64:
      op = m->BitcastInt64ToFloat64();
      break;
    // kExprI64ReinterpretF64:
    case wasm::kExprI64ReinterpretF64:
      op = m->BitcastFloat64ToInt64();
      break;
    // kExprI64Clz:
    case wasm::kExprI64Clz:
      op = m->Word64Clz();
      break;
    // kExprI64Ctz:
    case wasm::kExprI64Ctz: {
      if (m->Word64Ctz().IsSupported()) {
        op = m->Word64Ctz().op();
        break;
      } else if (m->Is32() && m->Word32Ctz().IsSupported()) {
        op = m->Word64CtzPlaceholder();
        break;
      } else if (m->Word64ReverseBits().IsSupported()) {
        Node* reversed = graph()->NewNode(m->Word64ReverseBits().op(), input);
        Node* result = graph()->NewNode(m->Word64Clz(), reversed);
        return result;
      } else {
        return BuildI64Ctz(input);
      }
    }
    // kExprI64Popcnt:
    case wasm::kExprI64Popcnt: {
      if (m->Word64Popcnt().IsSupported()) {
        op = m->Word64Popcnt().op();
      } else if (m->Is32() && m->Word32Popcnt().IsSupported()) {
        op = m->Word64PopcntPlaceholder();
      } else {
        return BuildI64Popcnt(input);
      }
      break;
    }
    // kExprF32SConvertI64:
    case wasm::kExprI64Eqz:
      op = m->Word64Equal();
      return graph()->NewNode(op, input, jsgraph()->Int64Constant(0));
    case wasm::kExprF32SConvertI64:
      if (m->Is32()) {
        return BuildF32SConvertI64(input);
      }
      op = m->RoundInt64ToFloat32();
      break;
    // kExprF32UConvertI64:
    case wasm::kExprF32UConvertI64:
      if (m->Is32()) {
        return BuildF32UConvertI64(input);
      }
      op = m->RoundUint64ToFloat32();
      break;
    // kExprF64SConvertI64:
    case wasm::kExprF64SConvertI64:
      if (m->Is32()) {
        return BuildF64SConvertI64(input);
      }
      op = m->RoundInt64ToFloat64();
      break;
    // kExprF64UConvertI64:
    case wasm::kExprF64UConvertI64:
      if (m->Is32()) {
        return BuildF64UConvertI64(input);
      }
      op = m->RoundUint64ToFloat64();
      break;
// kExprI64SConvertF32:
    case wasm::kExprI64SConvertF32: {
      return BuildI64SConvertF32(input);
    }
    // kExprI64SConvertF64:
    case wasm::kExprI64SConvertF64: {
      return BuildI64SConvertF64(input);
    }
    // kExprI64UConvertF32:
    case wasm::kExprI64UConvertF32: {
      return BuildI64UConvertF32(input);
    }
    // kExprI64UConvertF64:
    case wasm::kExprI64UConvertF64: {
      return BuildI64UConvertF64(input);
    }
    default:
      op = UnsupportedOpcode(opcode);
  }
  return graph()->NewNode(op, input);
}


Node* WasmGraphBuilder::Float32Constant(float value) {
  return jsgraph()->Float32Constant(value);
}


Node* WasmGraphBuilder::Float64Constant(double value) {
  return jsgraph()->Float64Constant(value);
}


Node* WasmGraphBuilder::Constant(Handle<Object> value) {
  return jsgraph()->Constant(value);
}


Node* WasmGraphBuilder::Branch(Node* cond, Node** true_node,
                               Node** false_node) {
  DCHECK_NOT_NULL(cond);
  DCHECK_NOT_NULL(*control_);
  Node* branch =
      graph()->NewNode(jsgraph()->common()->Branch(), cond, *control_);
  *true_node = graph()->NewNode(jsgraph()->common()->IfTrue(), branch);
  *false_node = graph()->NewNode(jsgraph()->common()->IfFalse(), branch);
  return branch;
}


Node* WasmGraphBuilder::Switch(unsigned count, Node* key) {
  return graph()->NewNode(jsgraph()->common()->Switch(count), key, *control_);
}


Node* WasmGraphBuilder::IfValue(int32_t value, Node* sw) {
  DCHECK_EQ(IrOpcode::kSwitch, sw->opcode());
  return graph()->NewNode(jsgraph()->common()->IfValue(value), sw);
}


Node* WasmGraphBuilder::IfDefault(Node* sw) {
  DCHECK_EQ(IrOpcode::kSwitch, sw->opcode());
  return graph()->NewNode(jsgraph()->common()->IfDefault(), sw);
}


Node* WasmGraphBuilder::Return(unsigned count, Node** vals) {
  DCHECK_NOT_NULL(*control_);
  DCHECK_NOT_NULL(*effect_);

  if (count == 0) {
    // Handle a return of void.
    vals[0] = jsgraph()->Int32Constant(0);
    count = 1;
  }

  Node** buf = Realloc(vals, count, count + 2);
  buf[count] = *effect_;
  buf[count + 1] = *control_;
  Node* ret = graph()->NewNode(jsgraph()->common()->Return(), count + 2, vals);

  MergeControlToEnd(jsgraph(), ret);
  return ret;
}


Node* WasmGraphBuilder::ReturnVoid() { return Return(0, Buffer(0)); }


Node* WasmGraphBuilder::Unreachable() {
  trap_->Unreachable();
  return nullptr;
}

Node* WasmGraphBuilder::MaskShiftCount32(Node* node) {
  static const int32_t kMask32 = 0x1f;
  if (!jsgraph()->machine()->Word32ShiftIsSafe()) {
    // Shifts by constants are so common we pattern-match them here.
    Int32Matcher match(node);
    if (match.HasValue()) {
      int32_t masked = (match.Value() & kMask32);
      if (match.Value() != masked) node = jsgraph()->Int32Constant(masked);
    } else {
      node = graph()->NewNode(jsgraph()->machine()->Word32And(), node,
                              jsgraph()->Int32Constant(kMask32));
    }
  }
  return node;
}

Node* WasmGraphBuilder::MaskShiftCount64(Node* node) {
  static const int64_t kMask64 = 0x3f;
  if (!jsgraph()->machine()->Word32ShiftIsSafe()) {
    // Shifts by constants are so common we pattern-match them here.
    Int64Matcher match(node);
    if (match.HasValue()) {
      int64_t masked = (match.Value() & kMask64);
      if (match.Value() != masked) node = jsgraph()->Int64Constant(masked);
    } else {
      node = graph()->NewNode(jsgraph()->machine()->Word64And(), node,
                              jsgraph()->Int64Constant(kMask64));
    }
  }
  return node;
}

Node* WasmGraphBuilder::BuildF32Neg(Node* input) {
  Node* result =
      Unop(wasm::kExprF32ReinterpretI32,
           Binop(wasm::kExprI32Xor, Unop(wasm::kExprI32ReinterpretF32, input),
                 jsgraph()->Int32Constant(0x80000000)));

  return result;
}


Node* WasmGraphBuilder::BuildF64Neg(Node* input) {
#if WASM_64
  Node* result =
      Unop(wasm::kExprF64ReinterpretI64,
           Binop(wasm::kExprI64Xor, Unop(wasm::kExprI64ReinterpretF64, input),
                 jsgraph()->Int64Constant(0x8000000000000000)));

  return result;
#else
  MachineOperatorBuilder* m = jsgraph()->machine();

  Node* old_high_word = graph()->NewNode(m->Float64ExtractHighWord32(), input);
  Node* new_high_word = Binop(wasm::kExprI32Xor, old_high_word,
                              jsgraph()->Int32Constant(0x80000000));

  return graph()->NewNode(m->Float64InsertHighWord32(), input, new_high_word);
#endif
}


Node* WasmGraphBuilder::BuildF32CopySign(Node* left, Node* right) {
  Node* result = Unop(
      wasm::kExprF32ReinterpretI32,
      Binop(wasm::kExprI32Ior,
            Binop(wasm::kExprI32And, Unop(wasm::kExprI32ReinterpretF32, left),
                  jsgraph()->Int32Constant(0x7fffffff)),
            Binop(wasm::kExprI32And, Unop(wasm::kExprI32ReinterpretF32, right),
                  jsgraph()->Int32Constant(0x80000000))));

  return result;
}


Node* WasmGraphBuilder::BuildF64CopySign(Node* left, Node* right) {
#if WASM_64
  Node* result = Unop(
      wasm::kExprF64ReinterpretI64,
      Binop(wasm::kExprI64Ior,
            Binop(wasm::kExprI64And, Unop(wasm::kExprI64ReinterpretF64, left),
                  jsgraph()->Int64Constant(0x7fffffffffffffff)),
            Binop(wasm::kExprI64And, Unop(wasm::kExprI64ReinterpretF64, right),
                  jsgraph()->Int64Constant(0x8000000000000000))));

  return result;
#else
  MachineOperatorBuilder* m = jsgraph()->machine();

  Node* high_word_left = graph()->NewNode(m->Float64ExtractHighWord32(), left);
  Node* high_word_right =
      graph()->NewNode(m->Float64ExtractHighWord32(), right);

  Node* new_high_word =
      Binop(wasm::kExprI32Ior, Binop(wasm::kExprI32And, high_word_left,
                                     jsgraph()->Int32Constant(0x7fffffff)),
            Binop(wasm::kExprI32And, high_word_right,
                  jsgraph()->Int32Constant(0x80000000)));

  return graph()->NewNode(m->Float64InsertHighWord32(), left, new_high_word);
#endif
}


Node* WasmGraphBuilder::BuildF32Min(Node* left, Node* right) {
  Diamond left_le_right(graph(), jsgraph()->common(),
                        Binop(wasm::kExprF32Le, left, right));

  Diamond right_lt_left(graph(), jsgraph()->common(),
                        Binop(wasm::kExprF32Lt, right, left));

  Diamond left_is_not_nan(graph(), jsgraph()->common(),
                          Binop(wasm::kExprF32Eq, left, left));

  return left_le_right.Phi(
      wasm::kAstF32, left,
      right_lt_left.Phi(
          wasm::kAstF32, right,
          left_is_not_nan.Phi(
              wasm::kAstF32,
              Binop(wasm::kExprF32Mul, right, Float32Constant(1.0)),
              Binop(wasm::kExprF32Mul, left, Float32Constant(1.0)))));
}


Node* WasmGraphBuilder::BuildF32Max(Node* left, Node* right) {
  Diamond left_ge_right(graph(), jsgraph()->common(),
                        Binop(wasm::kExprF32Ge, left, right));

  Diamond right_gt_left(graph(), jsgraph()->common(),
                        Binop(wasm::kExprF32Gt, right, left));

  Diamond left_is_not_nan(graph(), jsgraph()->common(),
                          Binop(wasm::kExprF32Eq, left, left));

  return left_ge_right.Phi(
      wasm::kAstF32, left,
      right_gt_left.Phi(
          wasm::kAstF32, right,
          left_is_not_nan.Phi(
              wasm::kAstF32,
              Binop(wasm::kExprF32Mul, right, Float32Constant(1.0)),
              Binop(wasm::kExprF32Mul, left, Float32Constant(1.0)))));
}


Node* WasmGraphBuilder::BuildF64Min(Node* left, Node* right) {
  Diamond left_le_right(graph(), jsgraph()->common(),
                        Binop(wasm::kExprF64Le, left, right));

  Diamond right_lt_left(graph(), jsgraph()->common(),
                        Binop(wasm::kExprF64Lt, right, left));

  Diamond left_is_not_nan(graph(), jsgraph()->common(),
                          Binop(wasm::kExprF64Eq, left, left));

  return left_le_right.Phi(
      wasm::kAstF64, left,
      right_lt_left.Phi(
          wasm::kAstF64, right,
          left_is_not_nan.Phi(
              wasm::kAstF64,
              Binop(wasm::kExprF64Mul, right, Float64Constant(1.0)),
              Binop(wasm::kExprF64Mul, left, Float64Constant(1.0)))));
}


Node* WasmGraphBuilder::BuildF64Max(Node* left, Node* right) {
  Diamond left_ge_right(graph(), jsgraph()->common(),
                        Binop(wasm::kExprF64Ge, left, right));

  Diamond right_gt_left(graph(), jsgraph()->common(),
                        Binop(wasm::kExprF64Lt, right, left));

  Diamond left_is_not_nan(graph(), jsgraph()->common(),
                          Binop(wasm::kExprF64Eq, left, left));

  return left_ge_right.Phi(
      wasm::kAstF64, left,
      right_gt_left.Phi(
          wasm::kAstF64, right,
          left_is_not_nan.Phi(
              wasm::kAstF64,
              Binop(wasm::kExprF64Mul, right, Float64Constant(1.0)),
              Binop(wasm::kExprF64Mul, left, Float64Constant(1.0)))));
}


Node* WasmGraphBuilder::BuildI32SConvertF32(Node* input) {
  MachineOperatorBuilder* m = jsgraph()->machine();
  if (module_ && module_->asm_js()) {
    // asm.js must use the wacky JS semantics.
    input = graph()->NewNode(m->ChangeFloat32ToFloat64(), input);
    return graph()->NewNode(
        m->TruncateFloat64ToInt32(TruncationMode::kJavaScript), input);
  }

  // Truncation of the input value is needed for the overflow check later.
  Node* trunc = Unop(wasm::kExprF32Trunc, input);
  Node* result = graph()->NewNode(m->TruncateFloat32ToInt32(), trunc);

  // Convert the result back to f64. If we end up at a different value than the
  // truncated input value, then there has been an overflow and we trap.
  Node* check = Unop(wasm::kExprF32SConvertI32, result);
  Node* overflow = Binop(wasm::kExprF32Ne, trunc, check);
  trap_->AddTrapIfTrue(wasm::kTrapFloatUnrepresentable, overflow);

  return result;
}


Node* WasmGraphBuilder::BuildI32SConvertF64(Node* input) {
  MachineOperatorBuilder* m = jsgraph()->machine();
  if (module_ && module_->asm_js()) {
    // asm.js must use the wacky JS semantics.
    return graph()->NewNode(
        m->TruncateFloat64ToInt32(TruncationMode::kJavaScript), input);
  }
  // Truncation of the input value is needed for the overflow check later.
  Node* trunc = Unop(wasm::kExprF64Trunc, input);
  Node* result = graph()->NewNode(m->ChangeFloat64ToInt32(), trunc);

  // Convert the result back to f64. If we end up at a different value than the
  // truncated input value, then there has been an overflow and we trap.
  Node* check = Unop(wasm::kExprF64SConvertI32, result);
  Node* overflow = Binop(wasm::kExprF64Ne, trunc, check);
  trap_->AddTrapIfTrue(wasm::kTrapFloatUnrepresentable, overflow);

  return result;
}


Node* WasmGraphBuilder::BuildI32UConvertF32(Node* input) {
  MachineOperatorBuilder* m = jsgraph()->machine();
  if (module_ && module_->asm_js()) {
    // asm.js must use the wacky JS semantics.
    input = graph()->NewNode(m->ChangeFloat32ToFloat64(), input);
    return graph()->NewNode(
        m->TruncateFloat64ToInt32(TruncationMode::kJavaScript), input);
  }

  // Truncation of the input value is needed for the overflow check later.
  Node* trunc = Unop(wasm::kExprF32Trunc, input);
  Node* result = graph()->NewNode(m->TruncateFloat32ToUint32(), trunc);

  // Convert the result back to f32. If we end up at a different value than the
  // truncated input value, then there has been an overflow and we trap.
  Node* check = Unop(wasm::kExprF32UConvertI32, result);
  Node* overflow = Binop(wasm::kExprF32Ne, trunc, check);
  trap_->AddTrapIfTrue(wasm::kTrapFloatUnrepresentable, overflow);

  return result;
}


Node* WasmGraphBuilder::BuildI32UConvertF64(Node* input) {
  MachineOperatorBuilder* m = jsgraph()->machine();
  if (module_ && module_->asm_js()) {
    // asm.js must use the wacky JS semantics.
    return graph()->NewNode(
        m->TruncateFloat64ToInt32(TruncationMode::kJavaScript), input);
  }
  // Truncation of the input value is needed for the overflow check later.
  Node* trunc = Unop(wasm::kExprF64Trunc, input);
  Node* result = graph()->NewNode(m->TruncateFloat64ToUint32(), trunc);

  // Convert the result back to f64. If we end up at a different value than the
  // truncated input value, then there has been an overflow and we trap.
  Node* check = Unop(wasm::kExprF64UConvertI32, result);
  Node* overflow = Binop(wasm::kExprF64Ne, trunc, check);
  trap_->AddTrapIfTrue(wasm::kTrapFloatUnrepresentable, overflow);

  return result;
}


Node* WasmGraphBuilder::BuildI32Ctz(Node* input) {
  //// Implement the following code as TF graph.
  // value = value | (value << 1);
  // value = value | (value << 2);
  // value = value | (value << 4);
  // value = value | (value << 8);
  // value = value | (value << 16);
  // return CountPopulation32(0xffffffff XOR value);

  Node* result =
      Binop(wasm::kExprI32Ior, input,
            Binop(wasm::kExprI32Shl, input, jsgraph()->Int32Constant(1)));

  result = Binop(wasm::kExprI32Ior, result,
                 Binop(wasm::kExprI32Shl, result, jsgraph()->Int32Constant(2)));

  result = Binop(wasm::kExprI32Ior, result,
                 Binop(wasm::kExprI32Shl, result, jsgraph()->Int32Constant(4)));

  result = Binop(wasm::kExprI32Ior, result,
                 Binop(wasm::kExprI32Shl, result, jsgraph()->Int32Constant(8)));

  result =
      Binop(wasm::kExprI32Ior, result,
            Binop(wasm::kExprI32Shl, result, jsgraph()->Int32Constant(16)));

  result = BuildI32Popcnt(
      Binop(wasm::kExprI32Xor, jsgraph()->Int32Constant(0xffffffff), result));

  return result;
}


Node* WasmGraphBuilder::BuildI64Ctz(Node* input) {
  //// Implement the following code as TF graph.
  // value = value | (value << 1);
  // value = value | (value << 2);
  // value = value | (value << 4);
  // value = value | (value << 8);
  // value = value | (value << 16);
  // value = value | (value << 32);
  // return CountPopulation64(0xffffffffffffffff XOR value);

  Node* result =
      Binop(wasm::kExprI64Ior, input,
            Binop(wasm::kExprI64Shl, input, jsgraph()->Int64Constant(1)));

  result = Binop(wasm::kExprI64Ior, result,
                 Binop(wasm::kExprI64Shl, result, jsgraph()->Int64Constant(2)));

  result = Binop(wasm::kExprI64Ior, result,
                 Binop(wasm::kExprI64Shl, result, jsgraph()->Int64Constant(4)));

  result = Binop(wasm::kExprI64Ior, result,
                 Binop(wasm::kExprI64Shl, result, jsgraph()->Int64Constant(8)));

  result =
      Binop(wasm::kExprI64Ior, result,
            Binop(wasm::kExprI64Shl, result, jsgraph()->Int64Constant(16)));

  result =
      Binop(wasm::kExprI64Ior, result,
            Binop(wasm::kExprI64Shl, result, jsgraph()->Int64Constant(32)));

  result = BuildI64Popcnt(Binop(
      wasm::kExprI64Xor, jsgraph()->Int64Constant(0xffffffffffffffff), result));

  return result;
}


Node* WasmGraphBuilder::BuildI32Popcnt(Node* input) {
  //// Implement the following code as a TF graph.
  // value = ((value >> 1) & 0x55555555) + (value & 0x55555555);
  // value = ((value >> 2) & 0x33333333) + (value & 0x33333333);
  // value = ((value >> 4) & 0x0f0f0f0f) + (value & 0x0f0f0f0f);
  // value = ((value >> 8) & 0x00ff00ff) + (value & 0x00ff00ff);
  // value = ((value >> 16) & 0x0000ffff) + (value & 0x0000ffff);

  Node* result = Binop(
      wasm::kExprI32Add,
      Binop(wasm::kExprI32And,
            Binop(wasm::kExprI32ShrU, input, jsgraph()->Int32Constant(1)),
            jsgraph()->Int32Constant(0x55555555)),
      Binop(wasm::kExprI32And, input, jsgraph()->Int32Constant(0x55555555)));

  result = Binop(
      wasm::kExprI32Add,
      Binop(wasm::kExprI32And,
            Binop(wasm::kExprI32ShrU, result, jsgraph()->Int32Constant(2)),
            jsgraph()->Int32Constant(0x33333333)),
      Binop(wasm::kExprI32And, result, jsgraph()->Int32Constant(0x33333333)));

  result = Binop(
      wasm::kExprI32Add,
      Binop(wasm::kExprI32And,
            Binop(wasm::kExprI32ShrU, result, jsgraph()->Int32Constant(4)),
            jsgraph()->Int32Constant(0x0f0f0f0f)),
      Binop(wasm::kExprI32And, result, jsgraph()->Int32Constant(0x0f0f0f0f)));

  result = Binop(
      wasm::kExprI32Add,
      Binop(wasm::kExprI32And,
            Binop(wasm::kExprI32ShrU, result, jsgraph()->Int32Constant(8)),
            jsgraph()->Int32Constant(0x00ff00ff)),
      Binop(wasm::kExprI32And, result, jsgraph()->Int32Constant(0x00ff00ff)));

  result = Binop(
      wasm::kExprI32Add,
      Binop(wasm::kExprI32And,
            Binop(wasm::kExprI32ShrU, result, jsgraph()->Int32Constant(16)),
            jsgraph()->Int32Constant(0x0000ffff)),
      Binop(wasm::kExprI32And, result, jsgraph()->Int32Constant(0x0000ffff)));

  return result;
}


Node* WasmGraphBuilder::BuildI64Popcnt(Node* input) {
  //// Implement the following code as a TF graph.
  // value = ((value >> 1) & 0x5555555555555555) + (value & 0x5555555555555555);
  // value = ((value >> 2) & 0x3333333333333333) + (value & 0x3333333333333333);
  // value = ((value >> 4) & 0x0f0f0f0f0f0f0f0f) + (value & 0x0f0f0f0f0f0f0f0f);
  // value = ((value >> 8) & 0x00ff00ff00ff00ff) + (value & 0x00ff00ff00ff00ff);
  // value = ((value >> 16) & 0x0000ffff0000ffff) + (value &
  // 0x0000ffff0000ffff);
  // value = ((value >> 32) & 0x00000000ffffffff) + (value &
  // 0x00000000ffffffff);

  Node* result =
      Binop(wasm::kExprI64Add,
            Binop(wasm::kExprI64And,
                  Binop(wasm::kExprI64ShrU, input, jsgraph()->Int64Constant(1)),
                  jsgraph()->Int64Constant(0x5555555555555555)),
            Binop(wasm::kExprI64And, input,
                  jsgraph()->Int64Constant(0x5555555555555555)));

  result = Binop(wasm::kExprI64Add,
                 Binop(wasm::kExprI64And, Binop(wasm::kExprI64ShrU, result,
                                                jsgraph()->Int64Constant(2)),
                       jsgraph()->Int64Constant(0x3333333333333333)),
                 Binop(wasm::kExprI64And, result,
                       jsgraph()->Int64Constant(0x3333333333333333)));

  result = Binop(wasm::kExprI64Add,
                 Binop(wasm::kExprI64And, Binop(wasm::kExprI64ShrU, result,
                                                jsgraph()->Int64Constant(4)),
                       jsgraph()->Int64Constant(0x0f0f0f0f0f0f0f0f)),
                 Binop(wasm::kExprI64And, result,
                       jsgraph()->Int64Constant(0x0f0f0f0f0f0f0f0f)));

  result = Binop(wasm::kExprI64Add,
                 Binop(wasm::kExprI64And, Binop(wasm::kExprI64ShrU, result,
                                                jsgraph()->Int64Constant(8)),
                       jsgraph()->Int64Constant(0x00ff00ff00ff00ff)),
                 Binop(wasm::kExprI64And, result,
                       jsgraph()->Int64Constant(0x00ff00ff00ff00ff)));

  result = Binop(wasm::kExprI64Add,
                 Binop(wasm::kExprI64And, Binop(wasm::kExprI64ShrU, result,
                                                jsgraph()->Int64Constant(16)),
                       jsgraph()->Int64Constant(0x0000ffff0000ffff)),
                 Binop(wasm::kExprI64And, result,
                       jsgraph()->Int64Constant(0x0000ffff0000ffff)));

  result = Binop(wasm::kExprI64Add,
                 Binop(wasm::kExprI64And, Binop(wasm::kExprI64ShrU, result,
                                                jsgraph()->Int64Constant(32)),
                       jsgraph()->Int64Constant(0x00000000ffffffff)),
                 Binop(wasm::kExprI64And, result,
                       jsgraph()->Int64Constant(0x00000000ffffffff)));

  return result;
}

Node* WasmGraphBuilder::BuildF32Trunc(Node* input) {
  MachineType type = MachineType::Float32();
  ExternalReference ref =
      ExternalReference::wasm_f32_trunc(jsgraph()->isolate());

  return BuildCFuncInstruction(ref, type, input);
}

Node* WasmGraphBuilder::BuildF32Floor(Node* input) {
  MachineType type = MachineType::Float32();
  ExternalReference ref =
      ExternalReference::wasm_f32_floor(jsgraph()->isolate());
  return BuildCFuncInstruction(ref, type, input);
}

Node* WasmGraphBuilder::BuildF32Ceil(Node* input) {
  MachineType type = MachineType::Float32();
  ExternalReference ref =
      ExternalReference::wasm_f32_ceil(jsgraph()->isolate());
  return BuildCFuncInstruction(ref, type, input);
}

Node* WasmGraphBuilder::BuildF32NearestInt(Node* input) {
  MachineType type = MachineType::Float32();
  ExternalReference ref =
      ExternalReference::wasm_f32_nearest_int(jsgraph()->isolate());
  return BuildCFuncInstruction(ref, type, input);
}

Node* WasmGraphBuilder::BuildF64Trunc(Node* input) {
  MachineType type = MachineType::Float64();
  ExternalReference ref =
      ExternalReference::wasm_f64_trunc(jsgraph()->isolate());
  return BuildCFuncInstruction(ref, type, input);
}

Node* WasmGraphBuilder::BuildF64Floor(Node* input) {
  MachineType type = MachineType::Float64();
  ExternalReference ref =
      ExternalReference::wasm_f64_floor(jsgraph()->isolate());
  return BuildCFuncInstruction(ref, type, input);
}

Node* WasmGraphBuilder::BuildF64Ceil(Node* input) {
  MachineType type = MachineType::Float64();
  ExternalReference ref =
      ExternalReference::wasm_f64_ceil(jsgraph()->isolate());
  return BuildCFuncInstruction(ref, type, input);
}

Node* WasmGraphBuilder::BuildF64NearestInt(Node* input) {
  MachineType type = MachineType::Float64();
  ExternalReference ref =
      ExternalReference::wasm_f64_nearest_int(jsgraph()->isolate());
  return BuildCFuncInstruction(ref, type, input);
}

Node* WasmGraphBuilder::BuildF64Acos(Node* input) {
  MachineType type = MachineType::Float64();
  ExternalReference ref =
      ExternalReference::f64_acos_wrapper_function(jsgraph()->isolate());
  return BuildCFuncInstruction(ref, type, input);
}

Node* WasmGraphBuilder::BuildF64Asin(Node* input) {
  MachineType type = MachineType::Float64();
  ExternalReference ref =
      ExternalReference::f64_asin_wrapper_function(jsgraph()->isolate());
  return BuildCFuncInstruction(ref, type, input);
}

Node* WasmGraphBuilder::BuildF64Atan(Node* input) {
  MachineType type = MachineType::Float64();
  ExternalReference ref =
      ExternalReference::f64_atan_wrapper_function(jsgraph()->isolate());
  return BuildCFuncInstruction(ref, type, input);
}

Node* WasmGraphBuilder::BuildF64Cos(Node* input) {
  MachineType type = MachineType::Float64();
  ExternalReference ref =
      ExternalReference::f64_cos_wrapper_function(jsgraph()->isolate());
  return BuildCFuncInstruction(ref, type, input);
}

Node* WasmGraphBuilder::BuildF64Sin(Node* input) {
  MachineType type = MachineType::Float64();
  ExternalReference ref =
      ExternalReference::f64_sin_wrapper_function(jsgraph()->isolate());
  return BuildCFuncInstruction(ref, type, input);
}

Node* WasmGraphBuilder::BuildF64Tan(Node* input) {
  MachineType type = MachineType::Float64();
  ExternalReference ref =
      ExternalReference::f64_tan_wrapper_function(jsgraph()->isolate());
  return BuildCFuncInstruction(ref, type, input);
}

Node* WasmGraphBuilder::BuildF64Exp(Node* input) {
  MachineType type = MachineType::Float64();
  ExternalReference ref =
      ExternalReference::f64_exp_wrapper_function(jsgraph()->isolate());
  return BuildCFuncInstruction(ref, type, input);
}

Node* WasmGraphBuilder::BuildF64Log(Node* input) {
  MachineType type = MachineType::Float64();
  ExternalReference ref =
      ExternalReference::f64_log_wrapper_function(jsgraph()->isolate());
  return BuildCFuncInstruction(ref, type, input);
}

Node* WasmGraphBuilder::BuildF64Atan2(Node* left, Node* right) {
  MachineType type = MachineType::Float64();
  ExternalReference ref =
      ExternalReference::f64_atan2_wrapper_function(jsgraph()->isolate());
  return BuildCFuncInstruction(ref, type, left, right);
}

Node* WasmGraphBuilder::BuildF64Pow(Node* left, Node* right) {
  MachineType type = MachineType::Float64();
  ExternalReference ref =
      ExternalReference::f64_pow_wrapper_function(jsgraph()->isolate());
  return BuildCFuncInstruction(ref, type, left, right);
}

Node* WasmGraphBuilder::BuildF64Mod(Node* left, Node* right) {
  MachineType type = MachineType::Float64();
  ExternalReference ref =
      ExternalReference::f64_mod_wrapper_function(jsgraph()->isolate());
  return BuildCFuncInstruction(ref, type, left, right);
}

Node* WasmGraphBuilder::BuildCFuncInstruction(ExternalReference ref,
                                              MachineType type, Node* input0,
                                              Node* input1) {
  // We do truncation by calling a C function which calculates the result.
  // The input is passed to the C function as a double*'s to avoid double
  // parameters. For this we reserve slots on the stack, store the parameters
  // in those slots, pass pointers to the slot to the C function,
  // and after calling the C function we collect the return value from
  // the stack slot.

  Node* stack_slot_param0 =
      graph()->NewNode(jsgraph()->machine()->StackSlot(type.representation()));

  const Operator* store_op0 = jsgraph()->machine()->Store(
      StoreRepresentation(type.representation(), kNoWriteBarrier));
  *effect_ = graph()->NewNode(store_op0, stack_slot_param0,
                              jsgraph()->Int32Constant(0), input0, *effect_,
                              *control_);

  Node* function = graph()->NewNode(jsgraph()->common()->ExternalConstant(ref));
  Node** args = Buffer(5);
  args[0] = function;
  args[1] = stack_slot_param0;
  int input_count = 1;

  if (input1 != nullptr) {
    Node* stack_slot_param1 = graph()->NewNode(
        jsgraph()->machine()->StackSlot(type.representation()));
    const Operator* store_op1 = jsgraph()->machine()->Store(
        StoreRepresentation(type.representation(), kNoWriteBarrier));
    *effect_ = graph()->NewNode(store_op1, stack_slot_param1,
                                jsgraph()->Int32Constant(0), input1, *effect_,
                                *control_);
    args[2] = stack_slot_param1;
    ++input_count;
  }

  Signature<MachineType>::Builder sig_builder(jsgraph()->zone(), 0,
                                              input_count);
  sig_builder.AddParam(MachineType::Pointer());
  if (input1 != nullptr) {
    sig_builder.AddParam(MachineType::Pointer());
  }
  BuildCCall(sig_builder.Build(), args);

  const Operator* load_op = jsgraph()->machine()->Load(type);

  Node* load =
      graph()->NewNode(load_op, stack_slot_param0, jsgraph()->Int32Constant(0),
                       *effect_, *control_);
  *effect_ = load;
  return load;
}

Node* WasmGraphBuilder::BuildF32SConvertI64(Node* input) {
  // TODO(titzer/bradnelson): Check handlng of asm.js case.
  return BuildIntToFloatConversionInstruction(
      input, ExternalReference::wasm_int64_to_float32(jsgraph()->isolate()),
      MachineRepresentation::kWord64, MachineType::Float32());
}
Node* WasmGraphBuilder::BuildF32UConvertI64(Node* input) {
  // TODO(titzer/bradnelson): Check handlng of asm.js case.
  return BuildIntToFloatConversionInstruction(
      input, ExternalReference::wasm_uint64_to_float32(jsgraph()->isolate()),
      MachineRepresentation::kWord64, MachineType::Float32());
}
Node* WasmGraphBuilder::BuildF64SConvertI64(Node* input) {
  return BuildIntToFloatConversionInstruction(
      input, ExternalReference::wasm_int64_to_float64(jsgraph()->isolate()),
      MachineRepresentation::kWord64, MachineType::Float64());
}
Node* WasmGraphBuilder::BuildF64UConvertI64(Node* input) {
  return BuildIntToFloatConversionInstruction(
      input, ExternalReference::wasm_uint64_to_float64(jsgraph()->isolate()),
      MachineRepresentation::kWord64, MachineType::Float64());
}

Node* WasmGraphBuilder::BuildIntToFloatConversionInstruction(
    Node* input, ExternalReference ref,
    MachineRepresentation parameter_representation,
    const MachineType result_type) {
  Node* stack_slot_param = graph()->NewNode(
      jsgraph()->machine()->StackSlot(parameter_representation));
  Node* stack_slot_result = graph()->NewNode(
      jsgraph()->machine()->StackSlot(result_type.representation()));
  const Operator* store_op = jsgraph()->machine()->Store(
      StoreRepresentation(parameter_representation, kNoWriteBarrier));
  *effect_ =
      graph()->NewNode(store_op, stack_slot_param, jsgraph()->Int32Constant(0),
                       input, *effect_, *control_);
  MachineSignature::Builder sig_builder(jsgraph()->zone(), 0, 2);
  sig_builder.AddParam(MachineType::Pointer());
  sig_builder.AddParam(MachineType::Pointer());
  Node* function = graph()->NewNode(jsgraph()->common()->ExternalConstant(ref));
  Node* args[] = {function, stack_slot_param, stack_slot_result};
  BuildCCall(sig_builder.Build(), args);
  const Operator* load_op = jsgraph()->machine()->Load(result_type);
  Node* load =
      graph()->NewNode(load_op, stack_slot_result, jsgraph()->Int32Constant(0),
                       *effect_, *control_);
  *effect_ = load;
  return load;
}

Node* WasmGraphBuilder::BuildI64SConvertF32(Node* input) {
  if (jsgraph()->machine()->Is32()) {
    return BuildFloatToIntConversionInstruction(
        input, ExternalReference::wasm_float32_to_int64(jsgraph()->isolate()),
        MachineRepresentation::kFloat32, MachineType::Int64());
  } else {
    Node* trunc = graph()->NewNode(
        jsgraph()->machine()->TryTruncateFloat32ToInt64(), input);
    Node* result = graph()->NewNode(jsgraph()->common()->Projection(0), trunc);
    Node* overflow =
        graph()->NewNode(jsgraph()->common()->Projection(1), trunc);
    trap_->ZeroCheck64(wasm::kTrapFloatUnrepresentable, overflow);
    return result;
  }
}

Node* WasmGraphBuilder::BuildI64UConvertF32(Node* input) {
  if (jsgraph()->machine()->Is32()) {
    return BuildFloatToIntConversionInstruction(
        input, ExternalReference::wasm_float32_to_uint64(jsgraph()->isolate()),
        MachineRepresentation::kFloat32, MachineType::Int64());
  } else {
    Node* trunc = graph()->NewNode(
        jsgraph()->machine()->TryTruncateFloat32ToUint64(), input);
    Node* result = graph()->NewNode(jsgraph()->common()->Projection(0), trunc);
    Node* overflow =
        graph()->NewNode(jsgraph()->common()->Projection(1), trunc);
    trap_->ZeroCheck64(wasm::kTrapFloatUnrepresentable, overflow);
    return result;
  }
}

Node* WasmGraphBuilder::BuildI64SConvertF64(Node* input) {
  if (jsgraph()->machine()->Is32()) {
    return BuildFloatToIntConversionInstruction(
        input, ExternalReference::wasm_float64_to_int64(jsgraph()->isolate()),
        MachineRepresentation::kFloat64, MachineType::Int64());
  } else {
    Node* trunc = graph()->NewNode(
        jsgraph()->machine()->TryTruncateFloat64ToInt64(), input);
    Node* result = graph()->NewNode(jsgraph()->common()->Projection(0), trunc);
    Node* overflow =
        graph()->NewNode(jsgraph()->common()->Projection(1), trunc);
    trap_->ZeroCheck64(wasm::kTrapFloatUnrepresentable, overflow);
    return result;
  }
}

Node* WasmGraphBuilder::BuildI64UConvertF64(Node* input) {
  if (jsgraph()->machine()->Is32()) {
    return BuildFloatToIntConversionInstruction(
        input, ExternalReference::wasm_float64_to_uint64(jsgraph()->isolate()),
        MachineRepresentation::kFloat64, MachineType::Int64());
  } else {
    Node* trunc = graph()->NewNode(
        jsgraph()->machine()->TryTruncateFloat64ToUint64(), input);
    Node* result = graph()->NewNode(jsgraph()->common()->Projection(0), trunc);
    Node* overflow =
        graph()->NewNode(jsgraph()->common()->Projection(1), trunc);
    trap_->ZeroCheck64(wasm::kTrapFloatUnrepresentable, overflow);
    return result;
  }
}

Node* WasmGraphBuilder::BuildFloatToIntConversionInstruction(
    Node* input, ExternalReference ref,
    MachineRepresentation parameter_representation,
    const MachineType result_type) {
  Node* stack_slot_param = graph()->NewNode(
      jsgraph()->machine()->StackSlot(parameter_representation));
  Node* stack_slot_result = graph()->NewNode(
      jsgraph()->machine()->StackSlot(result_type.representation()));
  const Operator* store_op = jsgraph()->machine()->Store(
      StoreRepresentation(parameter_representation, kNoWriteBarrier));
  *effect_ =
      graph()->NewNode(store_op, stack_slot_param, jsgraph()->Int32Constant(0),
                       input, *effect_, *control_);
  MachineSignature::Builder sig_builder(jsgraph()->zone(), 1, 2);
  sig_builder.AddReturn(MachineType::Int32());
  sig_builder.AddParam(MachineType::Pointer());
  sig_builder.AddParam(MachineType::Pointer());
  Node* function = graph()->NewNode(jsgraph()->common()->ExternalConstant(ref));
  Node* args[] = {function, stack_slot_param, stack_slot_result};
  trap_->ZeroCheck32(wasm::kTrapFloatUnrepresentable,
                     BuildCCall(sig_builder.Build(), args));
  const Operator* load_op = jsgraph()->machine()->Load(result_type);
  Node* load =
      graph()->NewNode(load_op, stack_slot_result, jsgraph()->Int32Constant(0),
                       *effect_, *control_);
  *effect_ = load;
  return load;
}

Node* WasmGraphBuilder::BuildI32DivS(Node* left, Node* right) {
  MachineOperatorBuilder* m = jsgraph()->machine();
  if (module_ && module_->asm_js()) {
    // asm.js semantics return 0 on divide or mod by zero.
    if (m->Int32DivIsSafe()) {
      // The hardware instruction does the right thing (e.g. arm).
      return graph()->NewNode(m->Int32Div(), left, right, graph()->start());
    }

    // Check denominator for zero.
    Diamond z(
        graph(), jsgraph()->common(),
        graph()->NewNode(m->Word32Equal(), right, jsgraph()->Int32Constant(0)),
        BranchHint::kFalse);

    // Check numerator for -1. (avoid minint / -1 case).
    Diamond n(
        graph(), jsgraph()->common(),
        graph()->NewNode(m->Word32Equal(), right, jsgraph()->Int32Constant(-1)),
        BranchHint::kFalse);

    Node* div = graph()->NewNode(m->Int32Div(), left, right, z.if_false);
    Node* neg =
        graph()->NewNode(m->Int32Sub(), jsgraph()->Int32Constant(0), left);

    return n.Phi(MachineRepresentation::kWord32, neg,
                 z.Phi(MachineRepresentation::kWord32,
                       jsgraph()->Int32Constant(0), div));
  }

  trap_->ZeroCheck32(wasm::kTrapDivByZero, right);
  Node* before = *control_;
  Node* denom_is_m1;
  Node* denom_is_not_m1;
  Branch(
      graph()->NewNode(m->Word32Equal(), right, jsgraph()->Int32Constant(-1)),
      &denom_is_m1, &denom_is_not_m1);
  *control_ = denom_is_m1;
  trap_->TrapIfEq32(wasm::kTrapDivUnrepresentable, left, kMinInt);
  if (*control_ != denom_is_m1) {
    *control_ = graph()->NewNode(jsgraph()->common()->Merge(2), denom_is_not_m1,
                                 *control_);
  } else {
    *control_ = before;
  }
  return graph()->NewNode(m->Int32Div(), left, right, *control_);
}

Node* WasmGraphBuilder::BuildI32RemS(Node* left, Node* right) {
  MachineOperatorBuilder* m = jsgraph()->machine();
  if (module_ && module_->asm_js()) {
    // asm.js semantics return 0 on divide or mod by zero.
    // Explicit check for x % 0.
    Diamond z(
        graph(), jsgraph()->common(),
        graph()->NewNode(m->Word32Equal(), right, jsgraph()->Int32Constant(0)),
        BranchHint::kFalse);

    // Explicit check for x % -1.
    Diamond d(
        graph(), jsgraph()->common(),
        graph()->NewNode(m->Word32Equal(), right, jsgraph()->Int32Constant(-1)),
        BranchHint::kFalse);
    d.Chain(z.if_false);

    return z.Phi(
        MachineRepresentation::kWord32, jsgraph()->Int32Constant(0),
        d.Phi(MachineRepresentation::kWord32, jsgraph()->Int32Constant(0),
              graph()->NewNode(m->Int32Mod(), left, right, d.if_false)));
  }

  trap_->ZeroCheck32(wasm::kTrapRemByZero, right);

  Diamond d(
      graph(), jsgraph()->common(),
      graph()->NewNode(m->Word32Equal(), right, jsgraph()->Int32Constant(-1)),
      BranchHint::kFalse);
  d.Chain(*control_);

  return d.Phi(MachineRepresentation::kWord32, jsgraph()->Int32Constant(0),
               graph()->NewNode(m->Int32Mod(), left, right, d.if_false));
}

Node* WasmGraphBuilder::BuildI32DivU(Node* left, Node* right) {
  MachineOperatorBuilder* m = jsgraph()->machine();
  if (module_ && module_->asm_js()) {
    // asm.js semantics return 0 on divide or mod by zero.
    if (m->Uint32DivIsSafe()) {
      // The hardware instruction does the right thing (e.g. arm).
      return graph()->NewNode(m->Uint32Div(), left, right, graph()->start());
    }

    // Explicit check for x % 0.
    Diamond z(
        graph(), jsgraph()->common(),
        graph()->NewNode(m->Word32Equal(), right, jsgraph()->Int32Constant(0)),
        BranchHint::kFalse);

    return z.Phi(MachineRepresentation::kWord32, jsgraph()->Int32Constant(0),
                 graph()->NewNode(jsgraph()->machine()->Uint32Div(), left,
                                  right, z.if_false));
  }
  return graph()->NewNode(m->Uint32Div(), left, right,
                          trap_->ZeroCheck32(wasm::kTrapDivByZero, right));
}

Node* WasmGraphBuilder::BuildI32RemU(Node* left, Node* right) {
  MachineOperatorBuilder* m = jsgraph()->machine();
  if (module_ && module_->asm_js()) {
    // asm.js semantics return 0 on divide or mod by zero.
    // Explicit check for x % 0.
    Diamond z(
        graph(), jsgraph()->common(),
        graph()->NewNode(m->Word32Equal(), right, jsgraph()->Int32Constant(0)),
        BranchHint::kFalse);

    Node* rem = graph()->NewNode(jsgraph()->machine()->Uint32Mod(), left, right,
                                 z.if_false);
    return z.Phi(MachineRepresentation::kWord32, jsgraph()->Int32Constant(0),
                 rem);
  }

  return graph()->NewNode(m->Uint32Mod(), left, right,
                          trap_->ZeroCheck32(wasm::kTrapRemByZero, right));
}

Node* WasmGraphBuilder::BuildI64DivS(Node* left, Node* right) {
  if (jsgraph()->machine()->Is32()) {
    return BuildDiv64Call(
        left, right, ExternalReference::wasm_int64_div(jsgraph()->isolate()),
        MachineType::Int64(), wasm::kTrapDivByZero);
  }
  trap_->ZeroCheck64(wasm::kTrapDivByZero, right);
  Node* before = *control_;
  Node* denom_is_m1;
  Node* denom_is_not_m1;
  Branch(graph()->NewNode(jsgraph()->machine()->Word64Equal(), right,
                          jsgraph()->Int64Constant(-1)),
         &denom_is_m1, &denom_is_not_m1);
  *control_ = denom_is_m1;
  trap_->TrapIfEq64(wasm::kTrapDivUnrepresentable, left,
                    std::numeric_limits<int64_t>::min());
  if (*control_ != denom_is_m1) {
    *control_ = graph()->NewNode(jsgraph()->common()->Merge(2), denom_is_not_m1,
                                 *control_);
  } else {
    *control_ = before;
  }
  return graph()->NewNode(jsgraph()->machine()->Int64Div(), left, right,
                          *control_);
}

Node* WasmGraphBuilder::BuildI64RemS(Node* left, Node* right) {
  if (jsgraph()->machine()->Is32()) {
    return BuildDiv64Call(
        left, right, ExternalReference::wasm_int64_mod(jsgraph()->isolate()),
        MachineType::Int64(), wasm::kTrapRemByZero);
  }
  trap_->ZeroCheck64(wasm::kTrapRemByZero, right);
  Diamond d(jsgraph()->graph(), jsgraph()->common(),
            graph()->NewNode(jsgraph()->machine()->Word64Equal(), right,
                             jsgraph()->Int64Constant(-1)));

  Node* rem = graph()->NewNode(jsgraph()->machine()->Int64Mod(), left, right,
                               d.if_false);

  return d.Phi(MachineRepresentation::kWord64, jsgraph()->Int64Constant(0),
               rem);
}

Node* WasmGraphBuilder::BuildI64DivU(Node* left, Node* right) {
  if (jsgraph()->machine()->Is32()) {
    return BuildDiv64Call(
        left, right, ExternalReference::wasm_uint64_div(jsgraph()->isolate()),
        MachineType::Int64(), wasm::kTrapDivByZero);
  }
  return graph()->NewNode(jsgraph()->machine()->Uint64Div(), left, right,
                          trap_->ZeroCheck64(wasm::kTrapDivByZero, right));
}
Node* WasmGraphBuilder::BuildI64RemU(Node* left, Node* right) {
  if (jsgraph()->machine()->Is32()) {
    return BuildDiv64Call(
        left, right, ExternalReference::wasm_uint64_mod(jsgraph()->isolate()),
        MachineType::Int64(), wasm::kTrapRemByZero);
  }
  return graph()->NewNode(jsgraph()->machine()->Uint64Mod(), left, right,
                          trap_->ZeroCheck64(wasm::kTrapRemByZero, right));
}

Node* WasmGraphBuilder::BuildDiv64Call(Node* left, Node* right,
                                       ExternalReference ref,
                                       MachineType result_type, int trap_zero) {
  Node* stack_slot_dst = graph()->NewNode(
      jsgraph()->machine()->StackSlot(MachineRepresentation::kWord64));
  Node* stack_slot_src = graph()->NewNode(
      jsgraph()->machine()->StackSlot(MachineRepresentation::kWord64));

  const Operator* store_op = jsgraph()->machine()->Store(
      StoreRepresentation(MachineRepresentation::kWord64, kNoWriteBarrier));
  *effect_ =
      graph()->NewNode(store_op, stack_slot_dst, jsgraph()->Int32Constant(0),
                       left, *effect_, *control_);
  *effect_ =
      graph()->NewNode(store_op, stack_slot_src, jsgraph()->Int32Constant(0),
                       right, *effect_, *control_);

  MachineSignature::Builder sig_builder(jsgraph()->zone(), 1, 2);
  sig_builder.AddReturn(MachineType::Int32());
  sig_builder.AddParam(MachineType::Pointer());
  sig_builder.AddParam(MachineType::Pointer());

  Node* function = graph()->NewNode(jsgraph()->common()->ExternalConstant(ref));
  Node* args[] = {function, stack_slot_dst, stack_slot_src};

  Node* call = BuildCCall(sig_builder.Build(), args);

  // TODO(wasm): This can get simpler if we have a specialized runtime call to
  // throw WASM exceptions by trap code instead of by string.
  trap_->ZeroCheck32(static_cast<wasm::TrapReason>(trap_zero), call);
  trap_->TrapIfEq32(wasm::kTrapDivUnrepresentable, call, -1);
  const Operator* load_op = jsgraph()->machine()->Load(result_type);
  Node* load =
      graph()->NewNode(load_op, stack_slot_dst, jsgraph()->Int32Constant(0),
                       *effect_, *control_);
  *effect_ = load;
  return load;
}

Node* WasmGraphBuilder::BuildCCall(MachineSignature* sig, Node** args) {
  const size_t params = sig->parameter_count();
  const size_t extra = 2;  // effect and control inputs.
  const size_t count = 1 + params + extra;

  // Reallocate the buffer to make space for extra inputs.
  args = Realloc(args, 1 + params, count);

  // Add effect and control inputs.
  args[params + 1] = *effect_;
  args[params + 2] = *control_;

  CallDescriptor* desc =
      Linkage::GetSimplifiedCDescriptor(jsgraph()->zone(), sig);

  const Operator* op = jsgraph()->common()->Call(desc);
  Node* call = graph()->NewNode(op, static_cast<int>(count), args);
  *effect_ = call;
  return call;
}

Node* WasmGraphBuilder::BuildWasmCall(wasm::FunctionSig* sig, Node** args) {
  const size_t params = sig->parameter_count();
  const size_t extra = 2;  // effect and control inputs.
  const size_t count = 1 + params + extra;

  // Reallocate the buffer to make space for extra inputs.
  args = Realloc(args, 1 + params, count);

  // Add effect and control inputs.
  args[params + 1] = *effect_;
  args[params + 2] = *control_;

  CallDescriptor* descriptor =
      wasm::ModuleEnv::GetWasmCallDescriptor(jsgraph()->zone(), sig);
  const Operator* op = jsgraph()->common()->Call(descriptor);
  Node* call = graph()->NewNode(op, static_cast<int>(count), args);

  *effect_ = call;
  return call;
}

Node* WasmGraphBuilder::CallDirect(uint32_t index, Node** args) {
  DCHECK_NULL(args[0]);

  // Add code object as constant.
  args[0] = Constant(module_->GetFunctionCode(index));
  wasm::FunctionSig* sig = module_->GetFunctionSignature(index);

  return BuildWasmCall(sig, args);
}

Node* WasmGraphBuilder::CallImport(uint32_t index, Node** args) {
  DCHECK_NULL(args[0]);

  // Add code object as constant.
  args[0] = Constant(module_->GetImportCode(index));
  wasm::FunctionSig* sig = module_->GetImportSignature(index);

  return BuildWasmCall(sig, args);
}

Node* WasmGraphBuilder::CallIndirect(uint32_t index, Node** args) {
  DCHECK_NOT_NULL(args[0]);
  DCHECK(module_ && module_->instance);

  MachineOperatorBuilder* machine = jsgraph()->machine();

  // Compute the code object by loading it from the function table.
  Node* key = args[0];

  // Bounds check the index.
  int table_size = static_cast<int>(module_->FunctionTableSize());
  if (table_size > 0) {
    // Bounds check against the table size.
    Node* size = Int32Constant(static_cast<int>(table_size));
    Node* in_bounds = graph()->NewNode(machine->Uint32LessThan(), key, size);
    trap_->AddTrapIfFalse(wasm::kTrapFuncInvalid, in_bounds);
  } else {
    // No function table. Generate a trap and return a constant.
    trap_->AddTrapIfFalse(wasm::kTrapFuncInvalid, Int32Constant(0));
    return trap_->GetTrapValue(module_->GetSignature(index));
  }
  Node* table = FunctionTable();

  // Load signature from the table and check.
  // The table is a FixedArray; signatures are encoded as SMIs.
  // [sig1, sig2, sig3, ...., code1, code2, code3 ...]
  ElementAccess access = AccessBuilder::ForFixedArrayElement();
  const int fixed_offset = access.header_size - access.tag();
  {
    Node* load_sig = graph()->NewNode(
        machine->Load(MachineType::AnyTagged()), table,
        graph()->NewNode(machine->Int32Add(),
                         graph()->NewNode(machine->Word32Shl(), key,
                                          Int32Constant(kPointerSizeLog2)),
                         Int32Constant(fixed_offset)),
        *effect_, *control_);
    Node* sig_match = graph()->NewNode(machine->WordEqual(), load_sig,
                                       jsgraph()->SmiConstant(index));
    trap_->AddTrapIfFalse(wasm::kTrapFuncSigMismatch, sig_match);
  }

  // Load code object from the table.
  int offset = fixed_offset + kPointerSize * table_size;
  Node* load_code = graph()->NewNode(
      machine->Load(MachineType::AnyTagged()), table,
      graph()->NewNode(machine->Int32Add(),
                       graph()->NewNode(machine->Word32Shl(), key,
                                        Int32Constant(kPointerSizeLog2)),
                       Int32Constant(offset)),
      *effect_, *control_);

  args[0] = load_code;
  wasm::FunctionSig* sig = module_->GetSignature(index);
  return BuildWasmCall(sig, args);
}


Node* WasmGraphBuilder::ToJS(Node* node, Node* context, wasm::LocalType type) {
  SimplifiedOperatorBuilder simplified(jsgraph()->zone());
  switch (type) {
    case wasm::kAstI32:
      return graph()->NewNode(simplified.ChangeInt32ToTagged(), node);
    case wasm::kAstI64:
      // TODO(titzer): i64->JS has no good solution right now. Using lower 32
      // bits.
      node =
          graph()->NewNode(jsgraph()->machine()->TruncateInt64ToInt32(), node);
      return graph()->NewNode(simplified.ChangeInt32ToTagged(), node);
    case wasm::kAstF32:
      node = graph()->NewNode(jsgraph()->machine()->ChangeFloat32ToFloat64(),
                              node);
      return graph()->NewNode(simplified.ChangeFloat64ToTagged(), node);
    case wasm::kAstF64:
      return graph()->NewNode(simplified.ChangeFloat64ToTagged(), node);
    case wasm::kAstStmt:
      return jsgraph()->UndefinedConstant();
    default:
      UNREACHABLE();
      return nullptr;
  }
}


Node* WasmGraphBuilder::FromJS(Node* node, Node* context,
                               wasm::LocalType type) {
  // Do a JavaScript ToNumber.
  Node* num =
      graph()->NewNode(jsgraph()->javascript()->ToNumber(), node, context,
                       jsgraph()->EmptyFrameState(), *effect_, *control_);
  *control_ = num;
  *effect_ = num;

  // Change representation.
  SimplifiedOperatorBuilder simplified(jsgraph()->zone());
  num = graph()->NewNode(simplified.ChangeTaggedToFloat64(), num);

  switch (type) {
    case wasm::kAstI32: {
      num = graph()->NewNode(jsgraph()->machine()->TruncateFloat64ToInt32(
                                 TruncationMode::kJavaScript),
                             num);
      break;
    }
    case wasm::kAstI64:
      // TODO(titzer): JS->i64 has no good solution right now. Using 32 bits.
      num = graph()->NewNode(jsgraph()->machine()->TruncateFloat64ToInt32(
                                 TruncationMode::kJavaScript),
                             num);
      num = graph()->NewNode(jsgraph()->machine()->ChangeInt32ToInt64(), num);
      break;
    case wasm::kAstF32:
      num = graph()->NewNode(jsgraph()->machine()->TruncateFloat64ToFloat32(),
                             num);
      break;
    case wasm::kAstF64:
      break;
    case wasm::kAstStmt:
      num = jsgraph()->Int32Constant(0);
      break;
    default:
      UNREACHABLE();
      return nullptr;
  }
  return num;
}

Node* WasmGraphBuilder::BuildI32Rol(Node* left, Node* right) {
  // Implement Rol by Ror since TurboFan does not have Rol opcode.
  // TODO(weiliang): support Word32Rol opcode in TurboFan.
  Int32Matcher m(right);
  if (m.HasValue()) {
    return Binop(wasm::kExprI32Ror, left,
                 jsgraph()->Int32Constant(32 - m.Value()));
  } else {
    return Binop(wasm::kExprI32Ror, left,
                 Binop(wasm::kExprI32Sub, jsgraph()->Int32Constant(32), right));
  }
}

Node* WasmGraphBuilder::BuildI64Rol(Node* left, Node* right) {
  // Implement Rol by Ror since TurboFan does not have Rol opcode.
  // TODO(weiliang): support Word64Rol opcode in TurboFan.
  Int64Matcher m(right);
  if (m.HasValue()) {
    return Binop(wasm::kExprI64Ror, left,
                 jsgraph()->Int64Constant(64 - m.Value()));
  } else {
    return Binop(wasm::kExprI64Ror, left,
                 Binop(wasm::kExprI64Sub, jsgraph()->Int64Constant(64), right));
  }
}

Node* WasmGraphBuilder::Invert(Node* node) {
  return Unop(wasm::kExprI32Eqz, node);
}


void WasmGraphBuilder::BuildJSToWasmWrapper(Handle<Code> wasm_code,
                                            wasm::FunctionSig* sig) {
  int params = static_cast<int>(sig->parameter_count());
  int count = params + 3;
  Node** args = Buffer(count);

  // Build the start and the JS parameter nodes.
  Node* start = Start(params + 5);
  *control_ = start;
  *effect_ = start;
  // Create the context parameter
  Node* context = graph()->NewNode(
      jsgraph()->common()->Parameter(
          Linkage::GetJSCallContextParamIndex(params + 1), "%context"),
      graph()->start());

  int pos = 0;
  args[pos++] = Constant(wasm_code);

  // Convert JS parameters to WASM numbers.
  for (int i = 0; i < params; i++) {
    Node* param =
        graph()->NewNode(jsgraph()->common()->Parameter(i + 1), start);
    args[pos++] = FromJS(param, context, sig->GetParam(i));
  }

  args[pos++] = *effect_;
  args[pos++] = *control_;

  // Call the WASM code.
  CallDescriptor* desc =
      wasm::ModuleEnv::GetWasmCallDescriptor(jsgraph()->zone(), sig);
  Node* call = graph()->NewNode(jsgraph()->common()->Call(desc), count, args);
  Node* jsval =
      ToJS(call, context,
           sig->return_count() == 0 ? wasm::kAstStmt : sig->GetReturn());
  Node* ret =
      graph()->NewNode(jsgraph()->common()->Return(), jsval, call, start);

  MergeControlToEnd(jsgraph(), ret);
}


void WasmGraphBuilder::BuildWasmToJSWrapper(Handle<JSFunction> function,
                                            wasm::FunctionSig* sig) {
  int js_count = function->shared()->internal_formal_parameter_count();
  int wasm_count = static_cast<int>(sig->parameter_count());

  // Build the start and the parameter nodes.
  Isolate* isolate = jsgraph()->isolate();
  CallDescriptor* desc;
  Node* start = Start(wasm_count + 3);
  *effect_ = start;
  *control_ = start;
  // JS context is the last parameter.
  Node* context = Constant(Handle<Context>(function->context(), isolate));
  Node** args = Buffer(wasm_count + 7);

  bool arg_count_before_args = false;
  bool add_new_target_undefined = false;

  int pos = 0;
  if (js_count == wasm_count) {
    // exact arity match, just call the function directly.
    desc = Linkage::GetJSCallDescriptor(graph()->zone(), false, wasm_count + 1,
                                        CallDescriptor::kNoFlags);
    arg_count_before_args = false;
    add_new_target_undefined = true;
  } else {
    // Use the Call builtin.
    Callable callable = CodeFactory::Call(isolate);
    args[pos++] = jsgraph()->HeapConstant(callable.code());
    desc = Linkage::GetStubCallDescriptor(isolate, graph()->zone(),
                                          callable.descriptor(), wasm_count + 1,
                                          CallDescriptor::kNoFlags);
    arg_count_before_args = true;
  }

  args[pos++] = jsgraph()->Constant(function);  // JS function.
  if (arg_count_before_args) {
    args[pos++] = jsgraph()->Int32Constant(wasm_count);  // argument count
  }
  // JS receiver.
  Handle<Object> global(function->context()->global_object(), isolate);
  args[pos++] = jsgraph()->Constant(global);

  // Convert WASM numbers to JS values.
  for (int i = 0; i < wasm_count; i++) {
    Node* param = graph()->NewNode(jsgraph()->common()->Parameter(i), start);
    args[pos++] = ToJS(param, context, sig->GetParam(i));
  }

  if (add_new_target_undefined) {
    args[pos++] = jsgraph()->UndefinedConstant();  // new target
  }

  if (!arg_count_before_args) {
    args[pos++] = jsgraph()->Int32Constant(wasm_count);  // argument count
  }
  args[pos++] = context;
  args[pos++] = *effect_;
  args[pos++] = *control_;

  Node* call = graph()->NewNode(jsgraph()->common()->Call(desc), pos, args);

  // Convert the return value back.
  Node* val =
      FromJS(call, context,
             sig->return_count() == 0 ? wasm::kAstStmt : sig->GetReturn());
  Node* ret = graph()->NewNode(jsgraph()->common()->Return(), val, call, start);

  MergeControlToEnd(jsgraph(), ret);
}


Node* WasmGraphBuilder::MemBuffer(uint32_t offset) {
  DCHECK(module_ && module_->instance);
  if (offset == 0) {
    if (!mem_buffer_) {
      mem_buffer_ = jsgraph()->IntPtrConstant(
          reinterpret_cast<uintptr_t>(module_->instance->mem_start));
    }
    return mem_buffer_;
  } else {
    return jsgraph()->IntPtrConstant(
        reinterpret_cast<uintptr_t>(module_->instance->mem_start + offset));
  }
}


Node* WasmGraphBuilder::MemSize(uint32_t offset) {
  DCHECK(module_ && module_->instance);
  uint32_t size = static_cast<uint32_t>(module_->instance->mem_size);
  if (offset == 0) {
    if (!mem_size_) mem_size_ = jsgraph()->Int32Constant(size);
    return mem_size_;
  } else {
    return jsgraph()->Int32Constant(size + offset);
  }
}


Node* WasmGraphBuilder::FunctionTable() {
  DCHECK(module_ && module_->instance &&
         !module_->instance->function_table.is_null());
  if (!function_table_) {
    function_table_ = jsgraph()->Constant(module_->instance->function_table);
  }
  return function_table_;
}


Node* WasmGraphBuilder::LoadGlobal(uint32_t index) {
  DCHECK(module_ && module_->instance && module_->instance->globals_start);
  MachineType mem_type = module_->GetGlobalType(index);
  Node* addr = jsgraph()->IntPtrConstant(
      reinterpret_cast<uintptr_t>(module_->instance->globals_start +
                                  module_->module->globals[index].offset));
  const Operator* op = jsgraph()->machine()->Load(mem_type);
  Node* node = graph()->NewNode(op, addr, jsgraph()->Int32Constant(0), *effect_,
                                *control_);
  *effect_ = node;
  return node;
}


Node* WasmGraphBuilder::StoreGlobal(uint32_t index, Node* val) {
  DCHECK(module_ && module_->instance && module_->instance->globals_start);
  MachineType mem_type = module_->GetGlobalType(index);
  Node* addr = jsgraph()->IntPtrConstant(
      reinterpret_cast<uintptr_t>(module_->instance->globals_start +
                                  module_->module->globals[index].offset));
  const Operator* op = jsgraph()->machine()->Store(
      StoreRepresentation(mem_type.representation(), kNoWriteBarrier));
  Node* node = graph()->NewNode(op, addr, jsgraph()->Int32Constant(0), val,
                                *effect_, *control_);
  *effect_ = node;
  return node;
}


void WasmGraphBuilder::BoundsCheckMem(MachineType memtype, Node* index,
                                      uint32_t offset) {
  // TODO(turbofan): fold bounds checks for constant indexes.
  DCHECK(module_ && module_->instance);
  size_t size = module_->instance->mem_size;
  byte memsize = wasm::WasmOpcodes::MemSize(memtype);
  Node* cond;
  if (offset >= size || (static_cast<uint64_t>(offset) + memsize) > size) {
    // The access will always throw.
    cond = jsgraph()->Int32Constant(0);
  } else {
    // Check against the limit.
    size_t limit = size - offset - memsize;
    CHECK(limit <= kMaxUInt32);
    cond = graph()->NewNode(
        jsgraph()->machine()->Uint32LessThanOrEqual(), index,
        jsgraph()->Int32Constant(static_cast<uint32_t>(limit)));
  }

  trap_->AddTrapIfFalse(wasm::kTrapMemOutOfBounds, cond);
}


Node* WasmGraphBuilder::LoadMem(wasm::LocalType type, MachineType memtype,
                                Node* index, uint32_t offset) {
  Node* load;

  if (module_ && module_->asm_js()) {
    // asm.js semantics use CheckedLoad (i.e. OOB reads return 0ish).
    DCHECK_EQ(0, offset);
    const Operator* op = jsgraph()->machine()->CheckedLoad(memtype);
    load = graph()->NewNode(op, MemBuffer(0), index, MemSize(0), *effect_,
                            *control_);
  } else {
    // WASM semantics throw on OOB. Introduce explicit bounds check.
    BoundsCheckMem(memtype, index, offset);
    load = graph()->NewNode(jsgraph()->machine()->Load(memtype),
                            MemBuffer(offset), index, *effect_, *control_);
  }

  *effect_ = load;

  if (type == wasm::kAstI64 &&
      ElementSizeLog2Of(memtype.representation()) < 3) {
    // TODO(titzer): TF zeroes the upper bits of 64-bit loads for subword sizes.
    if (memtype.IsSigned()) {
      // sign extend
      load = graph()->NewNode(jsgraph()->machine()->ChangeInt32ToInt64(), load);
    } else {
      // zero extend
      load =
          graph()->NewNode(jsgraph()->machine()->ChangeUint32ToUint64(), load);
    }
  }

  return load;
}


Node* WasmGraphBuilder::StoreMem(MachineType memtype, Node* index,
                                 uint32_t offset, Node* val) {
  Node* store;
  if (module_ && module_->asm_js()) {
    // asm.js semantics use CheckedStore (i.e. ignore OOB writes).
    DCHECK_EQ(0, offset);
    const Operator* op =
        jsgraph()->machine()->CheckedStore(memtype.representation());
    store = graph()->NewNode(op, MemBuffer(0), index, MemSize(0), val, *effect_,
                             *control_);
  } else {
    // WASM semantics throw on OOB. Introduce explicit bounds check.
    BoundsCheckMem(memtype, index, offset);
    StoreRepresentation rep(memtype.representation(), kNoWriteBarrier);
    store =
        graph()->NewNode(jsgraph()->machine()->Store(rep), MemBuffer(offset),
                         index, val, *effect_, *control_);
  }
  *effect_ = store;
  return store;
}


void WasmGraphBuilder::PrintDebugName(Node* node) {
  PrintF("#%d:%s", node->id(), node->op()->mnemonic());
}


Node* WasmGraphBuilder::String(const char* string) {
  return jsgraph()->Constant(
      jsgraph()->isolate()->factory()->NewStringFromAsciiChecked(string));
}


Graph* WasmGraphBuilder::graph() { return jsgraph()->graph(); }

void WasmGraphBuilder::Int64LoweringForTesting() {
  if (jsgraph()->machine()->Is32()) {
    Int64Lowering r(jsgraph()->graph(), jsgraph()->machine(),
                    jsgraph()->common(), jsgraph()->zone(),
                    function_signature_);
    r.LowerGraph();
  }
}

static void RecordFunctionCompilation(Logger::LogEventsAndTags tag,
                                      CompilationInfo* info,
                                      const char* message, uint32_t index,
                                      wasm::WasmName func_name) {
  Isolate* isolate = info->isolate();
  if (isolate->logger()->is_logging_code_events() ||
      isolate->cpu_profiler()->is_profiling()) {
    ScopedVector<char> buffer(128);
    SNPrintF(buffer, "%s#%d:%.*s", message, index, func_name.length,
             func_name.name);
    Handle<String> name_str =
        isolate->factory()->NewStringFromAsciiChecked(buffer.start());
    Handle<String> script_str =
        isolate->factory()->NewStringFromAsciiChecked("(WASM)");
    Handle<Code> code = info->code();
    Handle<SharedFunctionInfo> shared =
        isolate->factory()->NewSharedFunctionInfo(name_str, code, false);
    PROFILE(isolate, CodeCreateEvent(tag, AbstractCode::cast(*code), *shared,
                                     info, *script_str, 0, 0));
  }
}

Handle<JSFunction> CompileJSToWasmWrapper(
    Isolate* isolate, wasm::ModuleEnv* module, Handle<String> name,
    Handle<Code> wasm_code, Handle<JSObject> module_object, uint32_t index) {
  wasm::WasmFunction* func = &module->module->functions[index];

  //----------------------------------------------------------------------------
  // Create the JSFunction object.
  //----------------------------------------------------------------------------
  Handle<SharedFunctionInfo> shared =
      isolate->factory()->NewSharedFunctionInfo(name, wasm_code, false);
  int params = static_cast<int>(func->sig->parameter_count());
  shared->set_length(params);
  shared->set_internal_formal_parameter_count(params);
  Handle<JSFunction> function = isolate->factory()->NewFunction(
      isolate->wasm_function_map(), name, MaybeHandle<Code>());
  function->SetInternalField(0, *module_object);
  function->set_shared(*shared);

  //----------------------------------------------------------------------------
  // Create the Graph
  //----------------------------------------------------------------------------
  Zone zone(isolate->allocator());
  Graph graph(&zone);
  CommonOperatorBuilder common(&zone);
  JSOperatorBuilder javascript(&zone);
  MachineOperatorBuilder machine(&zone);
  JSGraph jsgraph(isolate, &graph, &common, &javascript, nullptr, &machine);

  Node* control = nullptr;
  Node* effect = nullptr;

  WasmGraphBuilder builder(&zone, &jsgraph, func->sig);
  builder.set_control_ptr(&control);
  builder.set_effect_ptr(&effect);
  builder.set_module(module);
  builder.BuildJSToWasmWrapper(wasm_code, func->sig);

  //----------------------------------------------------------------------------
  // Run the compilation pipeline.
  //----------------------------------------------------------------------------
  {
    // Changes lowering requires types.
    Typer typer(isolate, &graph);
    NodeVector roots(&zone);
    jsgraph.GetCachedNodes(&roots);
    typer.Run(roots);

    // Run generic and change lowering.
    JSGenericLowering generic(true, &jsgraph);
    ChangeLowering changes(&jsgraph);
    GraphReducer graph_reducer(&zone, &graph, jsgraph.Dead());
    graph_reducer.AddReducer(&changes);
    graph_reducer.AddReducer(&generic);
    graph_reducer.ReduceGraph();

    if (FLAG_trace_turbo_graph) {  // Simple textual RPO.
      OFStream os(stdout);
      os << "-- Graph after change lowering -- " << std::endl;
      os << AsRPO(graph);
    }

    // Schedule and compile to machine code.
    int params = static_cast<int>(
        module->GetFunctionSignature(index)->parameter_count());
    CallDescriptor* incoming = Linkage::GetJSCallDescriptor(
        &zone, false, params + 1, CallDescriptor::kNoFlags);
    Code::Flags flags = Code::ComputeFlags(Code::JS_TO_WASM_FUNCTION);
    bool debugging =
#if DEBUG
        true;
#else
        FLAG_print_opt_code || FLAG_trace_turbo || FLAG_trace_turbo_graph;
#endif
    const char* func_name = "js-to-wasm";

    static unsigned id = 0;
    Vector<char> buffer;
    if (debugging) {
      buffer = Vector<char>::New(128);
      SNPrintF(buffer, "js-to-wasm#%d", id);
      func_name = buffer.start();
    }

    CompilationInfo info(func_name, isolate, &zone, flags);
    Handle<Code> code =
        Pipeline::GenerateCodeForTesting(&info, incoming, &graph, nullptr);
#ifdef ENABLE_DISASSEMBLER
    if (FLAG_print_opt_code && !code.is_null()) {
      OFStream os(stdout);
      code->Disassemble(buffer.start(), os);
    }
#endif
    if (debugging) {
      buffer.Dispose();
    }

    RecordFunctionCompilation(
        Logger::FUNCTION_TAG, &info, "js-to-wasm", index,
        module->module->GetName(func->name_offset, func->name_length));
    // Set the JSFunction's machine code.
    function->set_code(*code);
  }
  return function;
}

Handle<Code> CompileWasmToJSWrapper(Isolate* isolate, wasm::ModuleEnv* module,
                                    Handle<JSFunction> function,
                                    wasm::FunctionSig* sig,
                                    wasm::WasmName module_name,
                                    wasm::WasmName function_name) {
  //----------------------------------------------------------------------------
  // Create the Graph
  //----------------------------------------------------------------------------
  Zone zone(isolate->allocator());
  Graph graph(&zone);
  CommonOperatorBuilder common(&zone);
  JSOperatorBuilder javascript(&zone);
  MachineOperatorBuilder machine(&zone);
  JSGraph jsgraph(isolate, &graph, &common, &javascript, nullptr, &machine);

  Node* control = nullptr;
  Node* effect = nullptr;

  WasmGraphBuilder builder(&zone, &jsgraph, sig);
  builder.set_control_ptr(&control);
  builder.set_effect_ptr(&effect);
  builder.set_module(module);
  builder.BuildWasmToJSWrapper(function, sig);

  Handle<Code> code = Handle<Code>::null();
  {
    // Changes lowering requires types.
    Typer typer(isolate, &graph);
    NodeVector roots(&zone);
    jsgraph.GetCachedNodes(&roots);
    typer.Run(roots);

    // Run generic and change lowering.
    JSGenericLowering generic(true, &jsgraph);
    ChangeLowering changes(&jsgraph);
    GraphReducer graph_reducer(&zone, &graph, jsgraph.Dead());
    graph_reducer.AddReducer(&changes);
    graph_reducer.AddReducer(&generic);
    graph_reducer.ReduceGraph();

    if (FLAG_trace_turbo_graph) {  // Simple textual RPO.
      OFStream os(stdout);
      os << "-- Graph after change lowering -- " << std::endl;
      os << AsRPO(graph);
    }

    // Schedule and compile to machine code.
    CallDescriptor* incoming =
        wasm::ModuleEnv::GetWasmCallDescriptor(&zone, sig);
    Code::Flags flags = Code::ComputeFlags(Code::WASM_TO_JS_FUNCTION);
    bool debugging =
#if DEBUG
        true;
#else
        FLAG_print_opt_code || FLAG_trace_turbo || FLAG_trace_turbo_graph;
#endif
    const char* func_name = "wasm-to-js";
    static unsigned id = 0;
    Vector<char> buffer;
    if (debugging) {
      buffer = Vector<char>::New(128);
      SNPrintF(buffer, "wasm-to-js#%d", id);
      func_name = buffer.start();
    }

    CompilationInfo info(func_name, isolate, &zone, flags);
    code = Pipeline::GenerateCodeForTesting(&info, incoming, &graph, nullptr);
#ifdef ENABLE_DISASSEMBLER
    if (FLAG_print_opt_code && !code.is_null()) {
      OFStream os(stdout);
      code->Disassemble(buffer.start(), os);
    }
#endif
    if (debugging) {
      buffer.Dispose();
    }

    RecordFunctionCompilation(Logger::FUNCTION_TAG, &info, "wasm-to-js", 0,
                              module_name);
  }
  return code;
}


// Helper function to compile a single function.
Handle<Code> CompileWasmFunction(wasm::ErrorThrower& thrower, Isolate* isolate,
                                 wasm::ModuleEnv* module_env,
                                 const wasm::WasmFunction& function) {
  if (FLAG_trace_wasm_compiler) {
    OFStream os(stdout);
    os << "Compiling WASM function "
       << wasm::WasmFunctionName(&function, module_env) << std::endl;
    os << std::endl;
  }

  double decode_ms = 0;
  base::ElapsedTimer decode_timer;
  if (FLAG_trace_wasm_decode_time) {
    decode_timer.Start();
  }

  // Create a TF graph during decoding.
  Zone zone(isolate->allocator());
  Graph graph(&zone);
  CommonOperatorBuilder common(&zone);
  MachineOperatorBuilder machine(
      &zone, MachineType::PointerRepresentation(),
      InstructionSelector::SupportedMachineOperatorFlags());
  JSGraph jsgraph(isolate, &graph, &common, nullptr, nullptr, &machine);
  WasmGraphBuilder builder(&zone, &jsgraph, function.sig);
  wasm::FunctionBody body = {
      module_env, function.sig, module_env->module->module_start,
      module_env->module->module_start + function.code_start_offset,
      module_env->module->module_start + function.code_end_offset};
  wasm::TreeResult result =
      wasm::BuildTFGraph(isolate->allocator(), &builder, body);

  if (result.failed()) {
    if (FLAG_trace_wasm_compiler) {
      OFStream os(stdout);
      os << "Compilation failed: " << result << std::endl;
    }
    // Add the function as another context for the exception
    ScopedVector<char> buffer(128);
    wasm::WasmName name =
        module_env->module->GetName(function.name_offset, function.name_length);
    SNPrintF(buffer, "Compiling WASM function #%d:%.*s failed:",
             function.func_index, name.length, name.name);
    thrower.Failed(buffer.start(), result);
    return Handle<Code>::null();
  }

  int index = static_cast<int>(function.func_index);
  if (index >= FLAG_trace_wasm_ast_start && index < FLAG_trace_wasm_ast_end) {
    PrintAst(isolate->allocator(), body);
  }

  if (FLAG_trace_wasm_decode_time) {
    decode_ms = decode_timer.Elapsed().InMillisecondsF();
  }

  base::ElapsedTimer compile_timer;
  if (FLAG_trace_wasm_decode_time) {
    compile_timer.Start();
  }
  // Run the compiler pipeline to generate machine code.
  CallDescriptor* descriptor =
      wasm::ModuleEnv::GetWasmCallDescriptor(&zone, function.sig);
  if (machine.Is32()) {
    descriptor = module_env->GetI32WasmCallDescriptor(&zone, descriptor);
  }
  Code::Flags flags = Code::ComputeFlags(Code::WASM_FUNCTION);
  // add flags here if a meaningful name is helpful for debugging.
  bool debugging =
#if DEBUG
      true;
#else
      FLAG_print_opt_code || FLAG_trace_turbo || FLAG_trace_turbo_graph;
#endif
  const char* func_name = "wasm";
  Vector<char> buffer;
  if (debugging) {
    buffer = Vector<char>::New(128);
    wasm::WasmName name =
        module_env->module->GetName(function.name_offset, function.name_length);
    SNPrintF(buffer, "WASM_function_#%d:%.*s", function.func_index, name.length,
             name.name);
    func_name = buffer.start();
  }
  CompilationInfo info(func_name, isolate, &zone, flags);

  Handle<Code> code =
      Pipeline::GenerateCodeForTesting(&info, descriptor, &graph);
  if (debugging) {
    buffer.Dispose();
  }
  if (!code.is_null()) {
    RecordFunctionCompilation(Logger::FUNCTION_TAG, &info, "WASM_function",
                              function.func_index,
                              module_env->module->GetName(
                                  function.name_offset, function.name_length));
  }

  if (FLAG_trace_wasm_decode_time) {
    double compile_ms = compile_timer.Elapsed().InMillisecondsF();
    PrintF(
        "wasm-compile ok: %d bytes, %0.3f ms decode, %d nodes, %0.3f ms "
        "compile\n",
        static_cast<int>(function.code_end_offset - function.code_start_offset),
        decode_ms, static_cast<int>(graph.NodeCount()), compile_ms);
  }
  return code;
}


}  // namespace compiler
}  // namespace internal
}  // namespace v8
