// Copyright 2015 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "src/base/adapters.h"
#include "src/compiler/instruction-selector-impl.h"
#include "src/compiler/node-matchers.h"
#include "src/compiler/node-properties.h"
#include "src/s390/frames-s390.h"

namespace v8 {
namespace internal {
namespace compiler {

enum ImmediateMode {
  kInt16Imm,
  kInt16Imm_Unsigned,
  kInt16Imm_Negate,
  kInt16Imm_4ByteAligned,
  kShift32Imm,
  kShift64Imm,
  kNoImmediate
};

// Adds S390-specific methods for generating operands.
class S390OperandGenerator final : public OperandGenerator {
 public:
  explicit S390OperandGenerator(InstructionSelector* selector)
      : OperandGenerator(selector) {}

  InstructionOperand UseOperand(Node* node, ImmediateMode mode) {
    if (CanBeImmediate(node, mode)) {
      return UseImmediate(node);
    }
    return UseRegister(node);
  }

  bool CanBeImmediate(Node* node, ImmediateMode mode) {
    int64_t value;
    if (node->opcode() == IrOpcode::kInt32Constant)
      value = OpParameter<int32_t>(node);
    else if (node->opcode() == IrOpcode::kInt64Constant)
      value = OpParameter<int64_t>(node);
    else
      return false;
    return CanBeImmediate(value, mode);
  }

  bool CanBeImmediate(int64_t value, ImmediateMode mode) {
    switch (mode) {
      case kInt16Imm:
        return is_int16(value);
      case kInt16Imm_Unsigned:
        return is_uint16(value);
      case kInt16Imm_Negate:
        return is_int16(-value);
      case kInt16Imm_4ByteAligned:
        return is_int16(value) && !(value & 3);
      case kShift32Imm:
        return 0 <= value && value < 32;
      case kShift64Imm:
        return 0 <= value && value < 64;
      case kNoImmediate:
        return false;
    }
    return false;
  }
};

namespace {

void VisitRR(InstructionSelector* selector, ArchOpcode opcode, Node* node) {
  S390OperandGenerator g(selector);
  selector->Emit(opcode, g.DefineAsRegister(node),
                 g.UseRegister(node->InputAt(0)));
}

void VisitRRR(InstructionSelector* selector, ArchOpcode opcode, Node* node) {
  S390OperandGenerator g(selector);
  selector->Emit(opcode, g.DefineAsRegister(node),
                 g.UseRegister(node->InputAt(0)),
                 g.UseRegister(node->InputAt(1)));
}

void VisitRRO(InstructionSelector* selector, ArchOpcode opcode, Node* node,
              ImmediateMode operand_mode) {
  S390OperandGenerator g(selector);
  selector->Emit(opcode, g.DefineAsRegister(node),
                 g.UseRegister(node->InputAt(0)),
                 g.UseOperand(node->InputAt(1), operand_mode));
}

#if V8_TARGET_ARCH_S390X
void VisitTryTruncateDouble(InstructionSelector* selector, ArchOpcode opcode,
                            Node* node) {
  S390OperandGenerator g(selector);
  InstructionOperand inputs[] = {g.UseRegister(node->InputAt(0))};
  InstructionOperand outputs[2];
  size_t output_count = 0;
  outputs[output_count++] = g.DefineAsRegister(node);

  Node* success_output = NodeProperties::FindProjection(node, 1);
  if (success_output) {
    outputs[output_count++] = g.DefineAsRegister(success_output);
  }

  selector->Emit(opcode, output_count, outputs, 1, inputs);
}
#endif

// Shared routine for multiple binary operations.
template <typename Matcher>
void VisitBinop(InstructionSelector* selector, Node* node,
                InstructionCode opcode, ImmediateMode operand_mode,
                FlagsContinuation* cont) {
  S390OperandGenerator g(selector);
  Matcher m(node);
  InstructionOperand inputs[4];
  size_t input_count = 0;
  InstructionOperand outputs[2];
  size_t output_count = 0;

  inputs[input_count++] = g.UseRegister(m.left().node());
  inputs[input_count++] = g.UseOperand(m.right().node(), operand_mode);

  if (cont->IsBranch()) {
    inputs[input_count++] = g.Label(cont->true_block());
    inputs[input_count++] = g.Label(cont->false_block());
  }

  outputs[output_count++] = g.DefineAsRegister(node);
  if (cont->IsSet()) {
    outputs[output_count++] = g.DefineAsRegister(cont->result());
  }

  DCHECK_NE(0u, input_count);
  DCHECK_NE(0u, output_count);
  DCHECK_GE(arraysize(inputs), input_count);
  DCHECK_GE(arraysize(outputs), output_count);

  opcode = cont->Encode(opcode);
  if (cont->IsDeoptimize()) {
    selector->EmitDeoptimize(opcode, output_count, outputs, input_count, inputs,
                             cont->frame_state());
  } else {
    selector->Emit(opcode, output_count, outputs, input_count, inputs);
  }
}

// Shared routine for multiple binary operations.
template <typename Matcher>
void VisitBinop(InstructionSelector* selector, Node* node, ArchOpcode opcode,
                ImmediateMode operand_mode) {
  FlagsContinuation cont;
  VisitBinop<Matcher>(selector, node, opcode, operand_mode, &cont);
}

}  // namespace

void InstructionSelector::VisitLoad(Node* node) {
  LoadRepresentation load_rep = LoadRepresentationOf(node->op());
  S390OperandGenerator g(this);
  Node* base = node->InputAt(0);
  Node* offset = node->InputAt(1);
  ArchOpcode opcode = kArchNop;
  ImmediateMode mode = kInt16Imm;
  switch (load_rep.representation()) {
    case MachineRepresentation::kFloat32:
      opcode = kS390_LoadFloat32;
      break;
    case MachineRepresentation::kFloat64:
      opcode = kS390_LoadDouble;
      break;
    case MachineRepresentation::kBit:  // Fall through.
    case MachineRepresentation::kWord8:
      opcode = load_rep.IsSigned() ? kS390_LoadWordS8 : kS390_LoadWordU8;
      break;
    case MachineRepresentation::kWord16:
      opcode = load_rep.IsSigned() ? kS390_LoadWordS16 : kS390_LoadWordU16;
      break;
#if !V8_TARGET_ARCH_S390X
    case MachineRepresentation::kTagged:  // Fall through.
#endif
    case MachineRepresentation::kWord32:
      opcode = kS390_LoadWordS32;
#if V8_TARGET_ARCH_S390X
      // TODO(john.yan): Remove this mode since s390 do not has this restriction
      mode = kInt16Imm_4ByteAligned;
#endif
      break;
#if V8_TARGET_ARCH_S390X
    case MachineRepresentation::kTagged:  // Fall through.
    case MachineRepresentation::kWord64:
      opcode = kS390_LoadWord64;
      mode = kInt16Imm_4ByteAligned;
      break;
#else
    case MachineRepresentation::kWord64:    // Fall through.
#endif
    case MachineRepresentation::kSimd128:  // Fall through.
    case MachineRepresentation::kNone:
      UNREACHABLE();
      return;
  }
  if (g.CanBeImmediate(offset, mode)) {
    Emit(opcode | AddressingModeField::encode(kMode_MRI),
         g.DefineAsRegister(node), g.UseRegister(base), g.UseImmediate(offset));
  } else if (g.CanBeImmediate(base, mode)) {
    Emit(opcode | AddressingModeField::encode(kMode_MRI),
         g.DefineAsRegister(node), g.UseRegister(offset), g.UseImmediate(base));
  } else {
    Emit(opcode | AddressingModeField::encode(kMode_MRR),
         g.DefineAsRegister(node), g.UseRegister(base), g.UseRegister(offset));
  }
}

void InstructionSelector::VisitStore(Node* node) {
  S390OperandGenerator g(this);
  Node* base = node->InputAt(0);
  Node* offset = node->InputAt(1);
  Node* value = node->InputAt(2);

  StoreRepresentation store_rep = StoreRepresentationOf(node->op());
  WriteBarrierKind write_barrier_kind = store_rep.write_barrier_kind();
  MachineRepresentation rep = store_rep.representation();

  if (write_barrier_kind != kNoWriteBarrier) {
    DCHECK_EQ(MachineRepresentation::kTagged, rep);
    AddressingMode addressing_mode;
    InstructionOperand inputs[3];
    size_t input_count = 0;
    inputs[input_count++] = g.UseUniqueRegister(base);
    // OutOfLineRecordWrite uses the offset in an 'AddP' instruction as well as
    // for the store itself, so we must check compatibility with both.
    if (g.CanBeImmediate(offset, kInt16Imm)
#if V8_TARGET_ARCH_S390X
        && g.CanBeImmediate(offset, kInt16Imm_4ByteAligned)
#endif
            ) {
      inputs[input_count++] = g.UseImmediate(offset);
      addressing_mode = kMode_MRI;
    } else {
      inputs[input_count++] = g.UseUniqueRegister(offset);
      addressing_mode = kMode_MRR;
    }
    inputs[input_count++] = g.UseUniqueRegister(value);
    RecordWriteMode record_write_mode = RecordWriteMode::kValueIsAny;
    switch (write_barrier_kind) {
      case kNoWriteBarrier:
        UNREACHABLE();
        break;
      case kMapWriteBarrier:
        record_write_mode = RecordWriteMode::kValueIsMap;
        break;
      case kPointerWriteBarrier:
        record_write_mode = RecordWriteMode::kValueIsPointer;
        break;
      case kFullWriteBarrier:
        record_write_mode = RecordWriteMode::kValueIsAny;
        break;
    }
    InstructionOperand temps[] = {g.TempRegister(), g.TempRegister()};
    size_t const temp_count = arraysize(temps);
    InstructionCode code = kArchStoreWithWriteBarrier;
    code |= AddressingModeField::encode(addressing_mode);
    code |= MiscField::encode(static_cast<int>(record_write_mode));
    Emit(code, 0, nullptr, input_count, inputs, temp_count, temps);
  } else {
    ArchOpcode opcode = kArchNop;
    ImmediateMode mode = kInt16Imm;
    switch (rep) {
      case MachineRepresentation::kFloat32:
        opcode = kS390_StoreFloat32;
        break;
      case MachineRepresentation::kFloat64:
        opcode = kS390_StoreDouble;
        break;
      case MachineRepresentation::kBit:  // Fall through.
      case MachineRepresentation::kWord8:
        opcode = kS390_StoreWord8;
        break;
      case MachineRepresentation::kWord16:
        opcode = kS390_StoreWord16;
        break;
#if !V8_TARGET_ARCH_S390X
      case MachineRepresentation::kTagged:  // Fall through.
#endif
      case MachineRepresentation::kWord32:
        opcode = kS390_StoreWord32;
        break;
#if V8_TARGET_ARCH_S390X
      case MachineRepresentation::kTagged:  // Fall through.
      case MachineRepresentation::kWord64:
        opcode = kS390_StoreWord64;
        mode = kInt16Imm_4ByteAligned;
        break;
#else
      case MachineRepresentation::kWord64:  // Fall through.
#endif
      case MachineRepresentation::kSimd128:  // Fall through.
      case MachineRepresentation::kNone:
        UNREACHABLE();
        return;
    }
    if (g.CanBeImmediate(offset, mode)) {
      Emit(opcode | AddressingModeField::encode(kMode_MRI), g.NoOutput(),
           g.UseRegister(base), g.UseImmediate(offset), g.UseRegister(value));
    } else if (g.CanBeImmediate(base, mode)) {
      Emit(opcode | AddressingModeField::encode(kMode_MRI), g.NoOutput(),
           g.UseRegister(offset), g.UseImmediate(base), g.UseRegister(value));
    } else {
      Emit(opcode | AddressingModeField::encode(kMode_MRR), g.NoOutput(),
           g.UseRegister(base), g.UseRegister(offset), g.UseRegister(value));
    }
  }
}

void InstructionSelector::VisitCheckedLoad(Node* node) {
  CheckedLoadRepresentation load_rep = CheckedLoadRepresentationOf(node->op());
  S390OperandGenerator g(this);
  Node* const base = node->InputAt(0);
  Node* const offset = node->InputAt(1);
  Node* const length = node->InputAt(2);
  ArchOpcode opcode = kArchNop;
  switch (load_rep.representation()) {
    case MachineRepresentation::kWord8:
      opcode = load_rep.IsSigned() ? kCheckedLoadInt8 : kCheckedLoadUint8;
      break;
    case MachineRepresentation::kWord16:
      opcode = load_rep.IsSigned() ? kCheckedLoadInt16 : kCheckedLoadUint16;
      break;
    case MachineRepresentation::kWord32:
      opcode = kCheckedLoadWord32;
      break;
#if V8_TARGET_ARCH_S390X
    case MachineRepresentation::kWord64:
      opcode = kCheckedLoadWord64;
      break;
#endif
    case MachineRepresentation::kFloat32:
      opcode = kCheckedLoadFloat32;
      break;
    case MachineRepresentation::kFloat64:
      opcode = kCheckedLoadFloat64;
      break;
    case MachineRepresentation::kBit:     // Fall through.
    case MachineRepresentation::kTagged:  // Fall through.
#if !V8_TARGET_ARCH_S390X
    case MachineRepresentation::kWord64:  // Fall through.
#endif
    case MachineRepresentation::kSimd128:  // Fall through.
    case MachineRepresentation::kNone:
      UNREACHABLE();
      return;
  }
  AddressingMode addressingMode = kMode_MRR;
  Emit(opcode | AddressingModeField::encode(addressingMode),
       g.DefineAsRegister(node), g.UseRegister(base), g.UseRegister(offset),
       g.UseOperand(length, kInt16Imm_Unsigned));
}

void InstructionSelector::VisitCheckedStore(Node* node) {
  MachineRepresentation rep = CheckedStoreRepresentationOf(node->op());
  S390OperandGenerator g(this);
  Node* const base = node->InputAt(0);
  Node* const offset = node->InputAt(1);
  Node* const length = node->InputAt(2);
  Node* const value = node->InputAt(3);
  ArchOpcode opcode = kArchNop;
  switch (rep) {
    case MachineRepresentation::kWord8:
      opcode = kCheckedStoreWord8;
      break;
    case MachineRepresentation::kWord16:
      opcode = kCheckedStoreWord16;
      break;
    case MachineRepresentation::kWord32:
      opcode = kCheckedStoreWord32;
      break;
#if V8_TARGET_ARCH_S390X
    case MachineRepresentation::kWord64:
      opcode = kCheckedStoreWord64;
      break;
#endif
    case MachineRepresentation::kFloat32:
      opcode = kCheckedStoreFloat32;
      break;
    case MachineRepresentation::kFloat64:
      opcode = kCheckedStoreFloat64;
      break;
    case MachineRepresentation::kBit:     // Fall through.
    case MachineRepresentation::kTagged:  // Fall through.
#if !V8_TARGET_ARCH_S390X
    case MachineRepresentation::kWord64:  // Fall through.
#endif
    case MachineRepresentation::kSimd128:  // Fall through.
    case MachineRepresentation::kNone:
      UNREACHABLE();
      return;
  }
  AddressingMode addressingMode = kMode_MRR;
  Emit(opcode | AddressingModeField::encode(addressingMode), g.NoOutput(),
       g.UseRegister(base), g.UseRegister(offset),
       g.UseOperand(length, kInt16Imm_Unsigned), g.UseRegister(value));
}

template <typename Matcher>
static void VisitLogical(InstructionSelector* selector, Node* node, Matcher* m,
                         ArchOpcode opcode, bool left_can_cover,
                         bool right_can_cover, ImmediateMode imm_mode) {
  S390OperandGenerator g(selector);

  // Map instruction to equivalent operation with inverted right input.
  ArchOpcode inv_opcode = opcode;
  switch (opcode) {
    case kS390_And:
      inv_opcode = kS390_AndComplement;
      break;
    case kS390_Or:
      inv_opcode = kS390_OrComplement;
      break;
    default:
      UNREACHABLE();
  }

  // Select Logical(y, ~x) for Logical(Xor(x, -1), y).
  if ((m->left().IsWord32Xor() || m->left().IsWord64Xor()) && left_can_cover) {
    Matcher mleft(m->left().node());
    if (mleft.right().Is(-1)) {
      selector->Emit(inv_opcode, g.DefineAsRegister(node),
                     g.UseRegister(m->right().node()),
                     g.UseRegister(mleft.left().node()));
      return;
    }
  }

  // Select Logical(x, ~y) for Logical(x, Xor(y, -1)).
  if ((m->right().IsWord32Xor() || m->right().IsWord64Xor()) &&
      right_can_cover) {
    Matcher mright(m->right().node());
    if (mright.right().Is(-1)) {
      // TODO(all): support shifted operand on right.
      selector->Emit(inv_opcode, g.DefineAsRegister(node),
                     g.UseRegister(m->left().node()),
                     g.UseRegister(mright.left().node()));
      return;
    }
  }

  VisitBinop<Matcher>(selector, node, opcode, imm_mode);
}

static inline bool IsContiguousMask32(uint32_t value, int* mb, int* me) {
  int mask_width = base::bits::CountPopulation32(value);
  int mask_msb = base::bits::CountLeadingZeros32(value);
  int mask_lsb = base::bits::CountTrailingZeros32(value);
  if ((mask_width == 0) || (mask_msb + mask_width + mask_lsb != 32))
    return false;
  *mb = mask_lsb + mask_width - 1;
  *me = mask_lsb;
  return true;
}

#if V8_TARGET_ARCH_S390X
static inline bool IsContiguousMask64(uint64_t value, int* mb, int* me) {
  int mask_width = base::bits::CountPopulation64(value);
  int mask_msb = base::bits::CountLeadingZeros64(value);
  int mask_lsb = base::bits::CountTrailingZeros64(value);
  if ((mask_width == 0) || (mask_msb + mask_width + mask_lsb != 64))
    return false;
  *mb = mask_lsb + mask_width - 1;
  *me = mask_lsb;
  return true;
}
#endif

void InstructionSelector::VisitWord32And(Node* node) {
  S390OperandGenerator g(this);
  Int32BinopMatcher m(node);
  int mb = 0;
  int me = 0;
  if (m.right().HasValue() && IsContiguousMask32(m.right().Value(), &mb, &me)) {
    int sh = 0;
    Node* left = m.left().node();
    if ((m.left().IsWord32Shr() || m.left().IsWord32Shl()) &&
        CanCover(node, left)) {
      Int32BinopMatcher mleft(m.left().node());
      if (mleft.right().IsInRange(0, 31)) {
        left = mleft.left().node();
        sh = mleft.right().Value();
        if (m.left().IsWord32Shr()) {
          // Adjust the mask such that it doesn't include any rotated bits.
          if (mb > 31 - sh) mb = 31 - sh;
          sh = (32 - sh) & 0x1f;
        } else {
          // Adjust the mask such that it doesn't include any rotated bits.
          if (me < sh) me = sh;
        }
      }
    }
    if (mb >= me) {
      Emit(kS390_RotLeftAndMask32, g.DefineAsRegister(node),
           g.UseRegister(left), g.TempImmediate(sh), g.TempImmediate(mb),
           g.TempImmediate(me));
      return;
    }
  }
  VisitLogical<Int32BinopMatcher>(
      this, node, &m, kS390_And, CanCover(node, m.left().node()),
      CanCover(node, m.right().node()), kInt16Imm_Unsigned);
}

#if V8_TARGET_ARCH_S390X
void InstructionSelector::VisitWord64And(Node* node) {
  S390OperandGenerator g(this);
  Int64BinopMatcher m(node);
  int mb = 0;
  int me = 0;
  if (m.right().HasValue() && IsContiguousMask64(m.right().Value(), &mb, &me)) {
    int sh = 0;
    Node* left = m.left().node();
    if ((m.left().IsWord64Shr() || m.left().IsWord64Shl()) &&
        CanCover(node, left)) {
      Int64BinopMatcher mleft(m.left().node());
      if (mleft.right().IsInRange(0, 63)) {
        left = mleft.left().node();
        sh = mleft.right().Value();
        if (m.left().IsWord64Shr()) {
          // Adjust the mask such that it doesn't include any rotated bits.
          if (mb > 63 - sh) mb = 63 - sh;
          sh = (64 - sh) & 0x3f;
        } else {
          // Adjust the mask such that it doesn't include any rotated bits.
          if (me < sh) me = sh;
        }
      }
    }
    if (mb >= me) {
      bool match = false;
      ArchOpcode opcode;
      int mask;
      if (me == 0) {
        match = true;
        opcode = kS390_RotLeftAndClearLeft64;
        mask = mb;
      } else if (mb == 63) {
        match = true;
        opcode = kS390_RotLeftAndClearRight64;
        mask = me;
      } else if (sh && me <= sh && m.left().IsWord64Shl()) {
        match = true;
        opcode = kS390_RotLeftAndClear64;
        mask = mb;
      }
      if (match) {
        Emit(opcode, g.DefineAsRegister(node), g.UseRegister(left),
             g.TempImmediate(sh), g.TempImmediate(mask));
        return;
      }
    }
  }
  VisitLogical<Int64BinopMatcher>(
      this, node, &m, kS390_And, CanCover(node, m.left().node()),
      CanCover(node, m.right().node()), kInt16Imm_Unsigned);
}
#endif

void InstructionSelector::VisitWord32Or(Node* node) {
  Int32BinopMatcher m(node);
  VisitLogical<Int32BinopMatcher>(
      this, node, &m, kS390_Or, CanCover(node, m.left().node()),
      CanCover(node, m.right().node()), kInt16Imm_Unsigned);
}

#if V8_TARGET_ARCH_S390X
void InstructionSelector::VisitWord64Or(Node* node) {
  Int64BinopMatcher m(node);
  VisitLogical<Int64BinopMatcher>(
      this, node, &m, kS390_Or, CanCover(node, m.left().node()),
      CanCover(node, m.right().node()), kInt16Imm_Unsigned);
}
#endif

void InstructionSelector::VisitWord32Xor(Node* node) {
  S390OperandGenerator g(this);
  Int32BinopMatcher m(node);
  if (m.right().Is(-1)) {
    Emit(kS390_Not, g.DefineAsRegister(node), g.UseRegister(m.left().node()));
  } else {
    VisitBinop<Int32BinopMatcher>(this, node, kS390_Xor, kInt16Imm_Unsigned);
  }
}

#if V8_TARGET_ARCH_S390X
void InstructionSelector::VisitWord64Xor(Node* node) {
  S390OperandGenerator g(this);
  Int64BinopMatcher m(node);
  if (m.right().Is(-1)) {
    Emit(kS390_Not, g.DefineAsRegister(node), g.UseRegister(m.left().node()));
  } else {
    VisitBinop<Int64BinopMatcher>(this, node, kS390_Xor, kInt16Imm_Unsigned);
  }
}
#endif

void InstructionSelector::VisitWord32Shl(Node* node) {
  S390OperandGenerator g(this);
  Int32BinopMatcher m(node);
  if (m.left().IsWord32And() && m.right().IsInRange(0, 31)) {
    Int32BinopMatcher mleft(m.left().node());
    int sh = m.right().Value();
    int mb;
    int me;
    if (mleft.right().HasValue() &&
        IsContiguousMask32(mleft.right().Value() << sh, &mb, &me)) {
      // Adjust the mask such that it doesn't include any rotated bits.
      if (me < sh) me = sh;
      if (mb >= me) {
        Emit(kS390_RotLeftAndMask32, g.DefineAsRegister(node),
             g.UseRegister(mleft.left().node()), g.TempImmediate(sh),
             g.TempImmediate(mb), g.TempImmediate(me));
        return;
      }
    }
  }
  VisitRRO(this, kS390_ShiftLeft32, node, kShift32Imm);
}

#if V8_TARGET_ARCH_S390X
void InstructionSelector::VisitWord64Shl(Node* node) {
  S390OperandGenerator g(this);
  Int64BinopMatcher m(node);
  // TODO(mbrandy): eliminate left sign extension if right >= 32
  if (m.left().IsWord64And() && m.right().IsInRange(0, 63)) {
    Int64BinopMatcher mleft(m.left().node());
    int sh = m.right().Value();
    int mb;
    int me;
    if (mleft.right().HasValue() &&
        IsContiguousMask64(mleft.right().Value() << sh, &mb, &me)) {
      // Adjust the mask such that it doesn't include any rotated bits.
      if (me < sh) me = sh;
      if (mb >= me) {
        bool match = false;
        ArchOpcode opcode;
        int mask;
        if (me == 0) {
          match = true;
          opcode = kS390_RotLeftAndClearLeft64;
          mask = mb;
        } else if (mb == 63) {
          match = true;
          opcode = kS390_RotLeftAndClearRight64;
          mask = me;
        } else if (sh && me <= sh) {
          match = true;
          opcode = kS390_RotLeftAndClear64;
          mask = mb;
        }
        if (match) {
          Emit(opcode, g.DefineAsRegister(node),
               g.UseRegister(mleft.left().node()), g.TempImmediate(sh),
               g.TempImmediate(mask));
          return;
        }
      }
    }
  }
  VisitRRO(this, kS390_ShiftLeft64, node, kShift64Imm);
}
#endif

void InstructionSelector::VisitWord32Shr(Node* node) {
  S390OperandGenerator g(this);
  Int32BinopMatcher m(node);
  if (m.left().IsWord32And() && m.right().IsInRange(0, 31)) {
    Int32BinopMatcher mleft(m.left().node());
    int sh = m.right().Value();
    int mb;
    int me;
    if (mleft.right().HasValue() &&
        IsContiguousMask32((uint32_t)(mleft.right().Value()) >> sh, &mb, &me)) {
      // Adjust the mask such that it doesn't include any rotated bits.
      if (mb > 31 - sh) mb = 31 - sh;
      sh = (32 - sh) & 0x1f;
      if (mb >= me) {
        Emit(kS390_RotLeftAndMask32, g.DefineAsRegister(node),
             g.UseRegister(mleft.left().node()), g.TempImmediate(sh),
             g.TempImmediate(mb), g.TempImmediate(me));
        return;
      }
    }
  }
  VisitRRO(this, kS390_ShiftRight32, node, kShift32Imm);
}

#if V8_TARGET_ARCH_S390X
void InstructionSelector::VisitWord64Shr(Node* node) {
  S390OperandGenerator g(this);
  Int64BinopMatcher m(node);
  if (m.left().IsWord64And() && m.right().IsInRange(0, 63)) {
    Int64BinopMatcher mleft(m.left().node());
    int sh = m.right().Value();
    int mb;
    int me;
    if (mleft.right().HasValue() &&
        IsContiguousMask64((uint64_t)(mleft.right().Value()) >> sh, &mb, &me)) {
      // Adjust the mask such that it doesn't include any rotated bits.
      if (mb > 63 - sh) mb = 63 - sh;
      sh = (64 - sh) & 0x3f;
      if (mb >= me) {
        bool match = false;
        ArchOpcode opcode;
        int mask;
        if (me == 0) {
          match = true;
          opcode = kS390_RotLeftAndClearLeft64;
          mask = mb;
        } else if (mb == 63) {
          match = true;
          opcode = kS390_RotLeftAndClearRight64;
          mask = me;
        }
        if (match) {
          Emit(opcode, g.DefineAsRegister(node),
               g.UseRegister(mleft.left().node()), g.TempImmediate(sh),
               g.TempImmediate(mask));
          return;
        }
      }
    }
  }
  VisitRRO(this, kS390_ShiftRight64, node, kShift64Imm);
}
#endif

void InstructionSelector::VisitWord32Sar(Node* node) {
  S390OperandGenerator g(this);
  Int32BinopMatcher m(node);
  // Replace with sign extension for (x << K) >> K where K is 16 or 24.
  if (CanCover(node, m.left().node()) && m.left().IsWord32Shl()) {
    Int32BinopMatcher mleft(m.left().node());
    if (mleft.right().Is(16) && m.right().Is(16)) {
      Emit(kS390_ExtendSignWord16, g.DefineAsRegister(node),
           g.UseRegister(mleft.left().node()));
      return;
    } else if (mleft.right().Is(24) && m.right().Is(24)) {
      Emit(kS390_ExtendSignWord8, g.DefineAsRegister(node),
           g.UseRegister(mleft.left().node()));
      return;
    }
  }
  VisitRRO(this, kS390_ShiftRightArith32, node, kShift32Imm);
}

#if !V8_TARGET_ARCH_S390X
void VisitPairBinop(InstructionSelector* selector, ArchOpcode opcode,
                    Node* node) {
  S390OperandGenerator g(selector);

  // We use UseUniqueRegister here to avoid register sharing with the output
  // registers.
  InstructionOperand inputs[] = {
      g.UseRegister(node->InputAt(0)), g.UseUniqueRegister(node->InputAt(1)),
      g.UseRegister(node->InputAt(2)), g.UseUniqueRegister(node->InputAt(3))};

  InstructionOperand outputs[] = {
      g.DefineAsRegister(node),
      g.DefineAsRegister(NodeProperties::FindProjection(node, 1))};

  selector->Emit(opcode, 2, outputs, 4, inputs);
}

void InstructionSelector::VisitInt32PairAdd(Node* node) {
  VisitPairBinop(this, kS390_AddPair, node);
}

void InstructionSelector::VisitInt32PairSub(Node* node) {
  VisitPairBinop(this, kS390_SubPair, node);
}

void InstructionSelector::VisitInt32PairMul(Node* node) {
  S390OperandGenerator g(this);
  InstructionOperand inputs[] = {g.UseUniqueRegister(node->InputAt(0)),
                                 g.UseUniqueRegister(node->InputAt(1)),
                                 g.UseUniqueRegister(node->InputAt(2)),
                                 g.UseUniqueRegister(node->InputAt(3))};

  InstructionOperand outputs[] = {
      g.DefineAsRegister(node),
      g.DefineAsRegister(NodeProperties::FindProjection(node, 1))};

  Emit(kS390_MulPair, 2, outputs, 4, inputs);
}

void VisitPairShift(InstructionSelector* selector, ArchOpcode opcode,
                    Node* node) {
  S390OperandGenerator g(selector);
  Int32Matcher m(node->InputAt(2));
  InstructionOperand shift_operand;
  if (m.HasValue()) {
    shift_operand = g.UseImmediate(m.node());
  } else {
    shift_operand = g.UseUniqueRegister(m.node());
  }

  InstructionOperand inputs[] = {g.UseRegister(node->InputAt(0)),
                                 g.UseRegister(node->InputAt(1)),
                                 shift_operand};

  InstructionOperand outputs[] = {
      g.DefineSameAsFirst(node),
      g.DefineAsRegister(NodeProperties::FindProjection(node, 1))};

  selector->Emit(opcode, 2, outputs, 3, inputs);
}

void InstructionSelector::VisitWord32PairShl(Node* node) {
  VisitPairShift(this, kS390_ShiftLeftPair, node);
}

void InstructionSelector::VisitWord32PairShr(Node* node) {
  VisitPairShift(this, kS390_ShiftRightPair, node);
}

void InstructionSelector::VisitWord32PairSar(Node* node) {
  VisitPairShift(this, kS390_ShiftRightArithPair, node);
}
#endif

#if V8_TARGET_ARCH_S390X
void InstructionSelector::VisitWord64Sar(Node* node) {
  VisitRRO(this, kS390_ShiftRightArith64, node, kShift64Imm);
}
#endif

void InstructionSelector::VisitWord32Ror(Node* node) {
  VisitRRO(this, kS390_RotRight32, node, kShift32Imm);
}

#if V8_TARGET_ARCH_S390X
void InstructionSelector::VisitWord64Ror(Node* node) {
  VisitRRO(this, kS390_RotRight64, node, kShift64Imm);
}
#endif

void InstructionSelector::VisitWord32Clz(Node* node) {
  S390OperandGenerator g(this);
  Emit(kS390_Cntlz32, g.DefineAsRegister(node),
       g.UseRegister(node->InputAt(0)));
}

#if V8_TARGET_ARCH_S390X
void InstructionSelector::VisitWord64Clz(Node* node) {
  S390OperandGenerator g(this);
  Emit(kS390_Cntlz64, g.DefineAsRegister(node),
       g.UseRegister(node->InputAt(0)));
}
#endif

void InstructionSelector::VisitWord32Popcnt(Node* node) {
  S390OperandGenerator g(this);
  Emit(kS390_Popcnt32, g.DefineAsRegister(node),
       g.UseRegister(node->InputAt(0)));
}

#if V8_TARGET_ARCH_S390X
void InstructionSelector::VisitWord64Popcnt(Node* node) {
  S390OperandGenerator g(this);
  Emit(kS390_Popcnt64, g.DefineAsRegister(node),
       g.UseRegister(node->InputAt(0)));
}
#endif

void InstructionSelector::VisitWord32Ctz(Node* node) { UNREACHABLE(); }

#if V8_TARGET_ARCH_S390X
void InstructionSelector::VisitWord64Ctz(Node* node) { UNREACHABLE(); }
#endif

void InstructionSelector::VisitWord32ReverseBits(Node* node) { UNREACHABLE(); }

#if V8_TARGET_ARCH_S390X
void InstructionSelector::VisitWord64ReverseBits(Node* node) { UNREACHABLE(); }
#endif

void InstructionSelector::VisitInt32Add(Node* node) {
  VisitBinop<Int32BinopMatcher>(this, node, kS390_Add, kInt16Imm);
}

#if V8_TARGET_ARCH_S390X
void InstructionSelector::VisitInt64Add(Node* node) {
  VisitBinop<Int64BinopMatcher>(this, node, kS390_Add, kInt16Imm);
}
#endif

void InstructionSelector::VisitInt32Sub(Node* node) {
  S390OperandGenerator g(this);
  Int32BinopMatcher m(node);
  if (m.left().Is(0)) {
    Emit(kS390_Neg, g.DefineAsRegister(node), g.UseRegister(m.right().node()));
  } else {
    VisitBinop<Int32BinopMatcher>(this, node, kS390_Sub, kInt16Imm_Negate);
  }
}

#if V8_TARGET_ARCH_S390X
void InstructionSelector::VisitInt64Sub(Node* node) {
  S390OperandGenerator g(this);
  Int64BinopMatcher m(node);
  if (m.left().Is(0)) {
    Emit(kS390_Neg, g.DefineAsRegister(node), g.UseRegister(m.right().node()));
  } else {
    VisitBinop<Int64BinopMatcher>(this, node, kS390_Sub, kInt16Imm_Negate);
  }
}
#endif

void InstructionSelector::VisitInt32Mul(Node* node) {
  VisitRRR(this, kS390_Mul32, node);
}

#if V8_TARGET_ARCH_S390X
void InstructionSelector::VisitInt64Mul(Node* node) {
  VisitRRR(this, kS390_Mul64, node);
}
#endif

void InstructionSelector::VisitInt32MulHigh(Node* node) {
  S390OperandGenerator g(this);
  Emit(kS390_MulHigh32, g.DefineAsRegister(node),
       g.UseRegister(node->InputAt(0)), g.UseRegister(node->InputAt(1)));
}

void InstructionSelector::VisitUint32MulHigh(Node* node) {
  S390OperandGenerator g(this);
  Emit(kS390_MulHighU32, g.DefineAsRegister(node),
       g.UseRegister(node->InputAt(0)), g.UseRegister(node->InputAt(1)));
}

void InstructionSelector::VisitInt32Div(Node* node) {
  VisitRRR(this, kS390_Div32, node);
}

#if V8_TARGET_ARCH_S390X
void InstructionSelector::VisitInt64Div(Node* node) {
  VisitRRR(this, kS390_Div64, node);
}
#endif

void InstructionSelector::VisitUint32Div(Node* node) {
  VisitRRR(this, kS390_DivU32, node);
}

#if V8_TARGET_ARCH_S390X
void InstructionSelector::VisitUint64Div(Node* node) {
  VisitRRR(this, kS390_DivU64, node);
}
#endif

void InstructionSelector::VisitInt32Mod(Node* node) {
  VisitRRR(this, kS390_Mod32, node);
}

#if V8_TARGET_ARCH_S390X
void InstructionSelector::VisitInt64Mod(Node* node) {
  VisitRRR(this, kS390_Mod64, node);
}
#endif

void InstructionSelector::VisitUint32Mod(Node* node) {
  VisitRRR(this, kS390_ModU32, node);
}

#if V8_TARGET_ARCH_S390X
void InstructionSelector::VisitUint64Mod(Node* node) {
  VisitRRR(this, kS390_ModU64, node);
}
#endif

void InstructionSelector::VisitChangeFloat32ToFloat64(Node* node) {
  VisitRR(this, kS390_Float32ToDouble, node);
}

void InstructionSelector::VisitRoundInt32ToFloat32(Node* node) {
  VisitRR(this, kS390_Int32ToFloat32, node);
}

void InstructionSelector::VisitRoundUint32ToFloat32(Node* node) {
  VisitRR(this, kS390_Uint32ToFloat32, node);
}

void InstructionSelector::VisitChangeInt32ToFloat64(Node* node) {
  VisitRR(this, kS390_Int32ToDouble, node);
}

void InstructionSelector::VisitChangeUint32ToFloat64(Node* node) {
  VisitRR(this, kS390_Uint32ToDouble, node);
}

void InstructionSelector::VisitChangeFloat64ToInt32(Node* node) {
  VisitRR(this, kS390_DoubleToInt32, node);
}

void InstructionSelector::VisitChangeFloat64ToUint32(Node* node) {
  VisitRR(this, kS390_DoubleToUint32, node);
}

void InstructionSelector::VisitTruncateFloat64ToUint32(Node* node) {
  VisitRR(this, kS390_DoubleToUint32, node);
}

#if V8_TARGET_ARCH_S390X
void InstructionSelector::VisitTryTruncateFloat32ToInt64(Node* node) {
  VisitTryTruncateDouble(this, kS390_Float32ToInt64, node);
}

void InstructionSelector::VisitTryTruncateFloat64ToInt64(Node* node) {
  VisitTryTruncateDouble(this, kS390_DoubleToInt64, node);
}

void InstructionSelector::VisitTryTruncateFloat32ToUint64(Node* node) {
  VisitTryTruncateDouble(this, kS390_Float32ToUint64, node);
}

void InstructionSelector::VisitTryTruncateFloat64ToUint64(Node* node) {
  VisitTryTruncateDouble(this, kS390_DoubleToUint64, node);
}

void InstructionSelector::VisitChangeInt32ToInt64(Node* node) {
  // TODO(mbrandy): inspect input to see if nop is appropriate.
  VisitRR(this, kS390_ExtendSignWord32, node);
}

void InstructionSelector::VisitChangeUint32ToUint64(Node* node) {
  // TODO(mbrandy): inspect input to see if nop is appropriate.
  VisitRR(this, kS390_Uint32ToUint64, node);
}
#endif

void InstructionSelector::VisitTruncateFloat64ToFloat32(Node* node) {
  VisitRR(this, kS390_DoubleToFloat32, node);
}

void InstructionSelector::VisitTruncateFloat64ToInt32(Node* node) {
  switch (TruncationModeOf(node->op())) {
    case TruncationMode::kJavaScript:
      return VisitRR(this, kArchTruncateDoubleToI, node);
    case TruncationMode::kRoundToZero:
      return VisitRR(this, kS390_DoubleToInt32, node);
  }
  UNREACHABLE();
}

void InstructionSelector::VisitTruncateFloat32ToInt32(Node* node) {
  VisitRR(this, kS390_Float32ToInt32, node);
}

void InstructionSelector::VisitTruncateFloat32ToUint32(Node* node) {
  VisitRR(this, kS390_Float32ToUint32, node);
}

#if V8_TARGET_ARCH_S390X
void InstructionSelector::VisitTruncateInt64ToInt32(Node* node) {
  // TODO(mbrandy): inspect input to see if nop is appropriate.
  VisitRR(this, kS390_Int64ToInt32, node);
}

void InstructionSelector::VisitRoundInt64ToFloat32(Node* node) {
  VisitRR(this, kS390_Int64ToFloat32, node);
}

void InstructionSelector::VisitRoundInt64ToFloat64(Node* node) {
  VisitRR(this, kS390_Int64ToDouble, node);
}

void InstructionSelector::VisitRoundUint64ToFloat32(Node* node) {
  VisitRR(this, kS390_Uint64ToFloat32, node);
}

void InstructionSelector::VisitRoundUint64ToFloat64(Node* node) {
  VisitRR(this, kS390_Uint64ToDouble, node);
}
#endif

void InstructionSelector::VisitBitcastFloat32ToInt32(Node* node) {
  VisitRR(this, kS390_BitcastFloat32ToInt32, node);
}

#if V8_TARGET_ARCH_S390X
void InstructionSelector::VisitBitcastFloat64ToInt64(Node* node) {
  VisitRR(this, kS390_BitcastDoubleToInt64, node);
}
#endif

void InstructionSelector::VisitBitcastInt32ToFloat32(Node* node) {
  VisitRR(this, kS390_BitcastInt32ToFloat32, node);
}

#if V8_TARGET_ARCH_S390X
void InstructionSelector::VisitBitcastInt64ToFloat64(Node* node) {
  VisitRR(this, kS390_BitcastInt64ToDouble, node);
}
#endif

void InstructionSelector::VisitFloat32Add(Node* node) {
  VisitRRR(this, kS390_AddFloat, node);
}

void InstructionSelector::VisitFloat64Add(Node* node) {
  // TODO(mbrandy): detect multiply-add
  VisitRRR(this, kS390_AddDouble, node);
}

void InstructionSelector::VisitFloat32Sub(Node* node) {
  S390OperandGenerator g(this);
  Float32BinopMatcher m(node);
  if (m.left().IsMinusZero()) {
    Emit(kS390_NegDouble, g.DefineAsRegister(node),
         g.UseRegister(m.right().node()));
    return;
  }
  VisitRRR(this, kS390_SubFloat, node);
}

void InstructionSelector::VisitFloat64Sub(Node* node) {
  // TODO(mbrandy): detect multiply-subtract
  S390OperandGenerator g(this);
  Float64BinopMatcher m(node);
  if (m.left().IsMinusZero()) {
    if (m.right().IsFloat64RoundDown() &&
        CanCover(m.node(), m.right().node())) {
      if (m.right().InputAt(0)->opcode() == IrOpcode::kFloat64Sub &&
          CanCover(m.right().node(), m.right().InputAt(0))) {
        Float64BinopMatcher mright0(m.right().InputAt(0));
        if (mright0.left().IsMinusZero()) {
          // -floor(-x) = ceil(x)
          Emit(kS390_CeilDouble, g.DefineAsRegister(node),
               g.UseRegister(mright0.right().node()));
          return;
        }
      }
    }
    Emit(kS390_NegDouble, g.DefineAsRegister(node),
         g.UseRegister(m.right().node()));
    return;
  }
  VisitRRR(this, kS390_SubDouble, node);
}

void InstructionSelector::VisitFloat32Mul(Node* node) {
  VisitRRR(this, kS390_MulFloat, node);
}

void InstructionSelector::VisitFloat64Mul(Node* node) {
  // TODO(mbrandy): detect negate
  VisitRRR(this, kS390_MulDouble, node);
}

void InstructionSelector::VisitFloat32Div(Node* node) {
  VisitRRR(this, kS390_DivFloat, node);
}

void InstructionSelector::VisitFloat64Div(Node* node) {
  VisitRRR(this, kS390_DivDouble, node);
}

void InstructionSelector::VisitFloat64Mod(Node* node) {
  S390OperandGenerator g(this);
  Emit(kS390_ModDouble, g.DefineAsFixed(node, d1),
       g.UseFixed(node->InputAt(0), d1), g.UseFixed(node->InputAt(1), d2))
      ->MarkAsCall();
}

void InstructionSelector::VisitFloat32Max(Node* node) { UNREACHABLE(); }

void InstructionSelector::VisitFloat64Max(Node* node) { UNREACHABLE(); }

void InstructionSelector::VisitFloat32Min(Node* node) { UNREACHABLE(); }

void InstructionSelector::VisitFloat64Min(Node* node) { UNREACHABLE(); }

void InstructionSelector::VisitFloat32Abs(Node* node) {
  VisitRR(this, kS390_AbsFloat, node);
}

void InstructionSelector::VisitFloat64Abs(Node* node) {
  VisitRR(this, kS390_AbsDouble, node);
}

void InstructionSelector::VisitFloat32Sqrt(Node* node) {
  VisitRR(this, kS390_SqrtFloat, node);
}

void InstructionSelector::VisitFloat64Sqrt(Node* node) {
  VisitRR(this, kS390_SqrtDouble, node);
}

void InstructionSelector::VisitFloat32RoundDown(Node* node) {
  VisitRR(this, kS390_FloorFloat, node);
}

void InstructionSelector::VisitFloat64RoundDown(Node* node) {
  VisitRR(this, kS390_FloorDouble, node);
}

void InstructionSelector::VisitFloat32RoundUp(Node* node) {
  VisitRR(this, kS390_CeilFloat, node);
}

void InstructionSelector::VisitFloat64RoundUp(Node* node) {
  VisitRR(this, kS390_CeilDouble, node);
}

void InstructionSelector::VisitFloat32RoundTruncate(Node* node) {
  VisitRR(this, kS390_TruncateFloat, node);
}

void InstructionSelector::VisitFloat64RoundTruncate(Node* node) {
  VisitRR(this, kS390_TruncateDouble, node);
}

void InstructionSelector::VisitFloat64RoundTiesAway(Node* node) {
  VisitRR(this, kS390_RoundDouble, node);
}

void InstructionSelector::VisitFloat32RoundTiesEven(Node* node) {
  UNREACHABLE();
}

void InstructionSelector::VisitFloat64RoundTiesEven(Node* node) {
  UNREACHABLE();
}

void InstructionSelector::VisitInt32AddWithOverflow(Node* node) {
  if (Node* ovf = NodeProperties::FindProjection(node, 1)) {
    FlagsContinuation cont = FlagsContinuation::ForSet(kOverflow, ovf);
    return VisitBinop<Int32BinopMatcher>(this, node, kS390_AddWithOverflow32,
                                         kInt16Imm, &cont);
  }
  FlagsContinuation cont;
  VisitBinop<Int32BinopMatcher>(this, node, kS390_AddWithOverflow32, kInt16Imm,
                                &cont);
}

void InstructionSelector::VisitInt32SubWithOverflow(Node* node) {
  if (Node* ovf = NodeProperties::FindProjection(node, 1)) {
    FlagsContinuation cont = FlagsContinuation::ForSet(kOverflow, ovf);
    return VisitBinop<Int32BinopMatcher>(this, node, kS390_SubWithOverflow32,
                                         kInt16Imm_Negate, &cont);
  }
  FlagsContinuation cont;
  VisitBinop<Int32BinopMatcher>(this, node, kS390_SubWithOverflow32,
                                kInt16Imm_Negate, &cont);
}

#if V8_TARGET_ARCH_S390X
void InstructionSelector::VisitInt64AddWithOverflow(Node* node) {
  if (Node* ovf = NodeProperties::FindProjection(node, 1)) {
    FlagsContinuation cont = FlagsContinuation::ForSet(kOverflow, ovf);
    return VisitBinop<Int64BinopMatcher>(this, node, kS390_Add, kInt16Imm,
                                         &cont);
  }
  FlagsContinuation cont;
  VisitBinop<Int64BinopMatcher>(this, node, kS390_Add, kInt16Imm, &cont);
}

void InstructionSelector::VisitInt64SubWithOverflow(Node* node) {
  if (Node* ovf = NodeProperties::FindProjection(node, 1)) {
    FlagsContinuation cont = FlagsContinuation::ForSet(kOverflow, ovf);
    return VisitBinop<Int64BinopMatcher>(this, node, kS390_Sub,
                                         kInt16Imm_Negate, &cont);
  }
  FlagsContinuation cont;
  VisitBinop<Int64BinopMatcher>(this, node, kS390_Sub, kInt16Imm_Negate, &cont);
}
#endif

static bool CompareLogical(FlagsContinuation* cont) {
  switch (cont->condition()) {
    case kUnsignedLessThan:
    case kUnsignedGreaterThanOrEqual:
    case kUnsignedLessThanOrEqual:
    case kUnsignedGreaterThan:
      return true;
    default:
      return false;
  }
  UNREACHABLE();
  return false;
}

namespace {

// Shared routine for multiple compare operations.
void VisitCompare(InstructionSelector* selector, InstructionCode opcode,
                  InstructionOperand left, InstructionOperand right,
                  FlagsContinuation* cont) {
  S390OperandGenerator g(selector);
  opcode = cont->Encode(opcode);
  if (cont->IsBranch()) {
    selector->Emit(opcode, g.NoOutput(), left, right,
                   g.Label(cont->true_block()), g.Label(cont->false_block()));
  } else if (cont->IsDeoptimize()) {
    selector->EmitDeoptimize(opcode, g.NoOutput(), left, right,
                             cont->frame_state());
  } else {
    DCHECK(cont->IsSet());
    selector->Emit(opcode, g.DefineAsRegister(cont->result()), left, right);
  }
}

// Shared routine for multiple word compare operations.
void VisitWordCompare(InstructionSelector* selector, Node* node,
                      InstructionCode opcode, FlagsContinuation* cont,
                      bool commutative, ImmediateMode immediate_mode) {
  S390OperandGenerator g(selector);
  Node* left = node->InputAt(0);
  Node* right = node->InputAt(1);

  // Match immediates on left or right side of comparison.
  if (g.CanBeImmediate(right, immediate_mode)) {
    VisitCompare(selector, opcode, g.UseRegister(left), g.UseImmediate(right),
                 cont);
  } else if (g.CanBeImmediate(left, immediate_mode)) {
    if (!commutative) cont->Commute();
    VisitCompare(selector, opcode, g.UseRegister(right), g.UseImmediate(left),
                 cont);
  } else {
    VisitCompare(selector, opcode, g.UseRegister(left), g.UseRegister(right),
                 cont);
  }
}

void VisitWord32Compare(InstructionSelector* selector, Node* node,
                        FlagsContinuation* cont) {
  ImmediateMode mode = (CompareLogical(cont) ? kInt16Imm_Unsigned : kInt16Imm);
  VisitWordCompare(selector, node, kS390_Cmp32, cont, false, mode);
}

#if V8_TARGET_ARCH_S390X
void VisitWord64Compare(InstructionSelector* selector, Node* node,
                        FlagsContinuation* cont) {
  ImmediateMode mode = (CompareLogical(cont) ? kInt16Imm_Unsigned : kInt16Imm);
  VisitWordCompare(selector, node, kS390_Cmp64, cont, false, mode);
}
#endif

// Shared routine for multiple float32 compare operations.
void VisitFloat32Compare(InstructionSelector* selector, Node* node,
                         FlagsContinuation* cont) {
  S390OperandGenerator g(selector);
  Node* left = node->InputAt(0);
  Node* right = node->InputAt(1);
  VisitCompare(selector, kS390_CmpFloat, g.UseRegister(left),
               g.UseRegister(right), cont);
}

// Shared routine for multiple float64 compare operations.
void VisitFloat64Compare(InstructionSelector* selector, Node* node,
                         FlagsContinuation* cont) {
  S390OperandGenerator g(selector);
  Node* left = node->InputAt(0);
  Node* right = node->InputAt(1);
  VisitCompare(selector, kS390_CmpDouble, g.UseRegister(left),
               g.UseRegister(right), cont);
}

// Shared routine for word comparisons against zero.
void VisitWordCompareZero(InstructionSelector* selector, Node* user,
                          Node* value, InstructionCode opcode,
                          FlagsContinuation* cont) {
  while (selector->CanCover(user, value)) {
    switch (value->opcode()) {
      case IrOpcode::kWord32Equal: {
        // Combine with comparisons against 0 by simply inverting the
        // continuation.
        Int32BinopMatcher m(value);
        if (m.right().Is(0)) {
          user = value;
          value = m.left().node();
          cont->Negate();
          continue;
        }
        cont->OverwriteAndNegateIfEqual(kEqual);
        return VisitWord32Compare(selector, value, cont);
      }
      case IrOpcode::kInt32LessThan:
        cont->OverwriteAndNegateIfEqual(kSignedLessThan);
        return VisitWord32Compare(selector, value, cont);
      case IrOpcode::kInt32LessThanOrEqual:
        cont->OverwriteAndNegateIfEqual(kSignedLessThanOrEqual);
        return VisitWord32Compare(selector, value, cont);
      case IrOpcode::kUint32LessThan:
        cont->OverwriteAndNegateIfEqual(kUnsignedLessThan);
        return VisitWord32Compare(selector, value, cont);
      case IrOpcode::kUint32LessThanOrEqual:
        cont->OverwriteAndNegateIfEqual(kUnsignedLessThanOrEqual);
        return VisitWord32Compare(selector, value, cont);
#if V8_TARGET_ARCH_S390X
      case IrOpcode::kWord64Equal:
        cont->OverwriteAndNegateIfEqual(kEqual);
        return VisitWord64Compare(selector, value, cont);
      case IrOpcode::kInt64LessThan:
        cont->OverwriteAndNegateIfEqual(kSignedLessThan);
        return VisitWord64Compare(selector, value, cont);
      case IrOpcode::kInt64LessThanOrEqual:
        cont->OverwriteAndNegateIfEqual(kSignedLessThanOrEqual);
        return VisitWord64Compare(selector, value, cont);
      case IrOpcode::kUint64LessThan:
        cont->OverwriteAndNegateIfEqual(kUnsignedLessThan);
        return VisitWord64Compare(selector, value, cont);
      case IrOpcode::kUint64LessThanOrEqual:
        cont->OverwriteAndNegateIfEqual(kUnsignedLessThanOrEqual);
        return VisitWord64Compare(selector, value, cont);
#endif
      case IrOpcode::kFloat32Equal:
        cont->OverwriteAndNegateIfEqual(kEqual);
        return VisitFloat32Compare(selector, value, cont);
      case IrOpcode::kFloat32LessThan:
        cont->OverwriteAndNegateIfEqual(kUnsignedLessThan);
        return VisitFloat32Compare(selector, value, cont);
      case IrOpcode::kFloat32LessThanOrEqual:
        cont->OverwriteAndNegateIfEqual(kUnsignedLessThanOrEqual);
        return VisitFloat32Compare(selector, value, cont);
      case IrOpcode::kFloat64Equal:
        cont->OverwriteAndNegateIfEqual(kEqual);
        return VisitFloat64Compare(selector, value, cont);
      case IrOpcode::kFloat64LessThan:
        cont->OverwriteAndNegateIfEqual(kUnsignedLessThan);
        return VisitFloat64Compare(selector, value, cont);
      case IrOpcode::kFloat64LessThanOrEqual:
        cont->OverwriteAndNegateIfEqual(kUnsignedLessThanOrEqual);
        return VisitFloat64Compare(selector, value, cont);
      case IrOpcode::kProjection:
        // Check if this is the overflow output projection of an
        // <Operation>WithOverflow node.
        if (ProjectionIndexOf(value->op()) == 1u) {
          // We cannot combine the <Operation>WithOverflow with this branch
          // unless the 0th projection (the use of the actual value of the
          // <Operation> is either nullptr, which means there's no use of the
          // actual value, or was already defined, which means it is scheduled
          // *AFTER* this branch).
          Node* const node = value->InputAt(0);
          Node* const result = NodeProperties::FindProjection(node, 0);
          if (result == nullptr || selector->IsDefined(result)) {
            switch (node->opcode()) {
              case IrOpcode::kInt32AddWithOverflow:
                cont->OverwriteAndNegateIfEqual(kOverflow);
                return VisitBinop<Int32BinopMatcher>(
                    selector, node, kS390_AddWithOverflow32, kInt16Imm, cont);
              case IrOpcode::kInt32SubWithOverflow:
                cont->OverwriteAndNegateIfEqual(kOverflow);
                return VisitBinop<Int32BinopMatcher>(selector, node,
                                                     kS390_SubWithOverflow32,
                                                     kInt16Imm_Negate, cont);
#if V8_TARGET_ARCH_S390X
              case IrOpcode::kInt64AddWithOverflow:
                cont->OverwriteAndNegateIfEqual(kOverflow);
                return VisitBinop<Int64BinopMatcher>(selector, node, kS390_Add,
                                                     kInt16Imm, cont);
              case IrOpcode::kInt64SubWithOverflow:
                cont->OverwriteAndNegateIfEqual(kOverflow);
                return VisitBinop<Int64BinopMatcher>(selector, node, kS390_Sub,
                                                     kInt16Imm_Negate, cont);
#endif
              default:
                break;
            }
          }
        }
        break;
      case IrOpcode::kInt32Sub:
        return VisitWord32Compare(selector, value, cont);
      case IrOpcode::kWord32And:
        return VisitWordCompare(selector, value, kS390_Tst32, cont, true,
                                kInt16Imm_Unsigned);
// TODO(mbrandy): Handle?
// case IrOpcode::kInt32Add:
// case IrOpcode::kWord32Or:
// case IrOpcode::kWord32Xor:
// case IrOpcode::kWord32Sar:
// case IrOpcode::kWord32Shl:
// case IrOpcode::kWord32Shr:
// case IrOpcode::kWord32Ror:
#if V8_TARGET_ARCH_S390X
      case IrOpcode::kInt64Sub:
        return VisitWord64Compare(selector, value, cont);
      case IrOpcode::kWord64And:
        return VisitWordCompare(selector, value, kS390_Tst64, cont, true,
                                kInt16Imm_Unsigned);
// TODO(mbrandy): Handle?
// case IrOpcode::kInt64Add:
// case IrOpcode::kWord64Or:
// case IrOpcode::kWord64Xor:
// case IrOpcode::kWord64Sar:
// case IrOpcode::kWord64Shl:
// case IrOpcode::kWord64Shr:
// case IrOpcode::kWord64Ror:
#endif
      default:
        break;
    }
    break;
  }

  // Branch could not be combined with a compare, emit compare against 0.
  S390OperandGenerator g(selector);
  VisitCompare(selector, opcode, g.UseRegister(value), g.TempImmediate(0),
               cont);
}

void VisitWord32CompareZero(InstructionSelector* selector, Node* user,
                            Node* value, FlagsContinuation* cont) {
  VisitWordCompareZero(selector, user, value, kS390_Cmp32, cont);
}

#if V8_TARGET_ARCH_S390X
void VisitWord64CompareZero(InstructionSelector* selector, Node* user,
                            Node* value, FlagsContinuation* cont) {
  VisitWordCompareZero(selector, user, value, kS390_Cmp64, cont);
}
#endif

}  // namespace

void InstructionSelector::VisitBranch(Node* branch, BasicBlock* tbranch,
                                      BasicBlock* fbranch) {
  FlagsContinuation cont(kNotEqual, tbranch, fbranch);
  VisitWord32CompareZero(this, branch, branch->InputAt(0), &cont);
}

void InstructionSelector::VisitDeoptimizeIf(Node* node) {
  FlagsContinuation cont =
      FlagsContinuation::ForDeoptimize(kNotEqual, node->InputAt(1));
  VisitWord32CompareZero(this, node, node->InputAt(0), &cont);
}

void InstructionSelector::VisitDeoptimizeUnless(Node* node) {
  FlagsContinuation cont =
      FlagsContinuation::ForDeoptimize(kEqual, node->InputAt(1));
  VisitWord32CompareZero(this, node, node->InputAt(0), &cont);
}

void InstructionSelector::VisitSwitch(Node* node, const SwitchInfo& sw) {
  S390OperandGenerator g(this);
  InstructionOperand value_operand = g.UseRegister(node->InputAt(0));

  // Emit either ArchTableSwitch or ArchLookupSwitch.
  size_t table_space_cost = 4 + sw.value_range;
  size_t table_time_cost = 3;
  size_t lookup_space_cost = 3 + 2 * sw.case_count;
  size_t lookup_time_cost = sw.case_count;
  if (sw.case_count > 0 &&
      table_space_cost + 3 * table_time_cost <=
          lookup_space_cost + 3 * lookup_time_cost &&
      sw.min_value > std::numeric_limits<int32_t>::min()) {
    InstructionOperand index_operand = value_operand;
    if (sw.min_value) {
      index_operand = g.TempRegister();
      Emit(kS390_Sub, index_operand, value_operand,
           g.TempImmediate(sw.min_value));
    }
    // Generate a table lookup.
    return EmitTableSwitch(sw, index_operand);
  }

  // Generate a sequence of conditional jumps.
  return EmitLookupSwitch(sw, value_operand);
}

void InstructionSelector::VisitWord32Equal(Node* const node) {
  FlagsContinuation cont = FlagsContinuation::ForSet(kEqual, node);
  Int32BinopMatcher m(node);
  if (m.right().Is(0)) {
    return VisitWord32CompareZero(this, m.node(), m.left().node(), &cont);
  }
  VisitWord32Compare(this, node, &cont);
}

void InstructionSelector::VisitInt32LessThan(Node* node) {
  FlagsContinuation cont = FlagsContinuation::ForSet(kSignedLessThan, node);
  VisitWord32Compare(this, node, &cont);
}

void InstructionSelector::VisitInt32LessThanOrEqual(Node* node) {
  FlagsContinuation cont =
      FlagsContinuation::ForSet(kSignedLessThanOrEqual, node);
  VisitWord32Compare(this, node, &cont);
}

void InstructionSelector::VisitUint32LessThan(Node* node) {
  FlagsContinuation cont = FlagsContinuation::ForSet(kUnsignedLessThan, node);
  VisitWord32Compare(this, node, &cont);
}

void InstructionSelector::VisitUint32LessThanOrEqual(Node* node) {
  FlagsContinuation cont =
      FlagsContinuation::ForSet(kUnsignedLessThanOrEqual, node);
  VisitWord32Compare(this, node, &cont);
}

#if V8_TARGET_ARCH_S390X
void InstructionSelector::VisitWord64Equal(Node* const node) {
  FlagsContinuation cont = FlagsContinuation::ForSet(kEqual, node);
  Int64BinopMatcher m(node);
  if (m.right().Is(0)) {
    return VisitWord64CompareZero(this, m.node(), m.left().node(), &cont);
  }
  VisitWord64Compare(this, node, &cont);
}

void InstructionSelector::VisitInt64LessThan(Node* node) {
  FlagsContinuation cont = FlagsContinuation::ForSet(kSignedLessThan, node);
  VisitWord64Compare(this, node, &cont);
}

void InstructionSelector::VisitInt64LessThanOrEqual(Node* node) {
  FlagsContinuation cont =
      FlagsContinuation::ForSet(kSignedLessThanOrEqual, node);
  VisitWord64Compare(this, node, &cont);
}

void InstructionSelector::VisitUint64LessThan(Node* node) {
  FlagsContinuation cont = FlagsContinuation::ForSet(kUnsignedLessThan, node);
  VisitWord64Compare(this, node, &cont);
}

void InstructionSelector::VisitUint64LessThanOrEqual(Node* node) {
  FlagsContinuation cont =
      FlagsContinuation::ForSet(kUnsignedLessThanOrEqual, node);
  VisitWord64Compare(this, node, &cont);
}
#endif

void InstructionSelector::VisitFloat32Equal(Node* node) {
  FlagsContinuation cont = FlagsContinuation::ForSet(kEqual, node);
  VisitFloat32Compare(this, node, &cont);
}

void InstructionSelector::VisitFloat32LessThan(Node* node) {
  FlagsContinuation cont = FlagsContinuation::ForSet(kUnsignedLessThan, node);
  VisitFloat32Compare(this, node, &cont);
}

void InstructionSelector::VisitFloat32LessThanOrEqual(Node* node) {
  FlagsContinuation cont =
      FlagsContinuation::ForSet(kUnsignedLessThanOrEqual, node);
  VisitFloat32Compare(this, node, &cont);
}

void InstructionSelector::VisitFloat64Equal(Node* node) {
  FlagsContinuation cont = FlagsContinuation::ForSet(kEqual, node);
  VisitFloat64Compare(this, node, &cont);
}

void InstructionSelector::VisitFloat64LessThan(Node* node) {
  FlagsContinuation cont = FlagsContinuation::ForSet(kUnsignedLessThan, node);
  VisitFloat64Compare(this, node, &cont);
}

void InstructionSelector::VisitFloat64LessThanOrEqual(Node* node) {
  FlagsContinuation cont =
      FlagsContinuation::ForSet(kUnsignedLessThanOrEqual, node);
  VisitFloat64Compare(this, node, &cont);
}

void InstructionSelector::EmitPrepareArguments(
    ZoneVector<PushParameter>* arguments, const CallDescriptor* descriptor,
    Node* node) {
  S390OperandGenerator g(this);

  // Prepare for C function call.
  if (descriptor->IsCFunctionCall()) {
    Emit(kArchPrepareCallCFunction |
             MiscField::encode(static_cast<int>(descriptor->CParameterCount())),
         0, nullptr, 0, nullptr);

    // Poke any stack arguments.
    int slot = kStackFrameExtraParamSlot;
    for (PushParameter input : (*arguments)) {
      Emit(kS390_StoreToStackSlot, g.NoOutput(), g.UseRegister(input.node()),
           g.TempImmediate(slot));
      ++slot;
    }
  } else {
    // Push any stack arguments.
    int num_slots = static_cast<int>(descriptor->StackParameterCount());
    int slot = 0;
    for (PushParameter input : (*arguments)) {
      if (slot == 0) {
        DCHECK(input.node());
        Emit(kS390_PushFrame, g.NoOutput(), g.UseRegister(input.node()),
             g.TempImmediate(num_slots));
      } else {
        // Skip any alignment holes in pushed nodes.
        if (input.node()) {
          Emit(kS390_StoreToStackSlot, g.NoOutput(),
               g.UseRegister(input.node()), g.TempImmediate(slot));
        }
      }
      ++slot;
    }
  }
}

bool InstructionSelector::IsTailCallAddressImmediate() { return false; }

int InstructionSelector::GetTempsCountForTailCallFromJSFunction() { return 3; }

void InstructionSelector::VisitFloat64ExtractLowWord32(Node* node) {
  S390OperandGenerator g(this);
  Emit(kS390_DoubleExtractLowWord32, g.DefineAsRegister(node),
       g.UseRegister(node->InputAt(0)));
}

void InstructionSelector::VisitFloat64ExtractHighWord32(Node* node) {
  S390OperandGenerator g(this);
  Emit(kS390_DoubleExtractHighWord32, g.DefineAsRegister(node),
       g.UseRegister(node->InputAt(0)));
}

void InstructionSelector::VisitFloat64InsertLowWord32(Node* node) {
  S390OperandGenerator g(this);
  Node* left = node->InputAt(0);
  Node* right = node->InputAt(1);
  if (left->opcode() == IrOpcode::kFloat64InsertHighWord32 &&
      CanCover(node, left)) {
    left = left->InputAt(1);
    Emit(kS390_DoubleConstruct, g.DefineAsRegister(node), g.UseRegister(left),
         g.UseRegister(right));
    return;
  }
  Emit(kS390_DoubleInsertLowWord32, g.DefineSameAsFirst(node),
       g.UseRegister(left), g.UseRegister(right));
}

void InstructionSelector::VisitFloat64InsertHighWord32(Node* node) {
  S390OperandGenerator g(this);
  Node* left = node->InputAt(0);
  Node* right = node->InputAt(1);
  if (left->opcode() == IrOpcode::kFloat64InsertLowWord32 &&
      CanCover(node, left)) {
    left = left->InputAt(1);
    Emit(kS390_DoubleConstruct, g.DefineAsRegister(node), g.UseRegister(right),
         g.UseRegister(left));
    return;
  }
  Emit(kS390_DoubleInsertHighWord32, g.DefineSameAsFirst(node),
       g.UseRegister(left), g.UseRegister(right));
}

// static
MachineOperatorBuilder::Flags
InstructionSelector::SupportedMachineOperatorFlags() {
  return MachineOperatorBuilder::kFloat32RoundDown |
         MachineOperatorBuilder::kFloat64RoundDown |
         MachineOperatorBuilder::kFloat32RoundUp |
         MachineOperatorBuilder::kFloat64RoundUp |
         MachineOperatorBuilder::kFloat32RoundTruncate |
         MachineOperatorBuilder::kFloat64RoundTruncate |
         MachineOperatorBuilder::kFloat64RoundTiesAway |
         MachineOperatorBuilder::kWord32Popcnt |
         MachineOperatorBuilder::kWord64Popcnt;
}

}  // namespace compiler
}  // namespace internal
}  // namespace v8
