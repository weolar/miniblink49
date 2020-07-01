#include "src/builtins/builtins-utils-gen.h"
#include "src/builtins/builtins.h"
#include "src/code-factory.h"
#include "src/elements-kind.h"
#include "src/heap/factory-inl.h"
#include "src/objects.h"
#include "src/objects/arguments.h"
#include "src/objects/bigint.h"
#include "src/objects/free-space.h"
#include "src/objects/js-generator.h"
#include "src/objects/js-promise.h"
#include "src/objects/js-regexp-string-iterator.h"
#include "src/objects/module.h"
#include "src/objects/stack-frame-info.h"
#include "src/builtins/builtins-array-gen.h"
#include "src/builtins/builtins-collections-gen.h"
#include "src/builtins/builtins-data-view-gen.h"
#include "src/builtins/builtins-iterator-gen.h"
#include "src/builtins/builtins-proxy-gen.h"
#include "src/builtins/builtins-regexp-gen.h"
#include "src/builtins/builtins-regexp-gen.h"
#include "src/builtins/builtins-typed-array-gen.h"
#include "src/builtins/builtins-constructor-gen.h"
#include "src/builtins/builtins-typed-array-gen.h"
#include "src/builtins/builtins-typed-array-gen.h"
#include "src/builtins/builtins-typed-array-gen.h"
#include "src/builtins/builtins-typed-array-gen.h"
#include "src/builtins/builtins-typed-array-gen.h"
#include "src/builtins/builtins-typed-array-gen.h"
#include "src/builtins/builtins-typed-array-gen.h"
#include "src/builtins/builtins-typed-array-gen.h"
#include "torque-generated/builtins-base-from-dsl-gen.h"
#include "torque-generated/builtins-growable-fixed-array-from-dsl-gen.h"
#include "torque-generated/builtins-arguments-from-dsl-gen.h"
#include "torque-generated/builtins-array-from-dsl-gen.h"
#include "torque-generated/builtins-array-copywithin-from-dsl-gen.h"
#include "torque-generated/builtins-array-filter-from-dsl-gen.h"
#include "torque-generated/builtins-array-find-from-dsl-gen.h"
#include "torque-generated/builtins-array-findindex-from-dsl-gen.h"
#include "torque-generated/builtins-array-foreach-from-dsl-gen.h"
#include "torque-generated/builtins-array-join-from-dsl-gen.h"
#include "torque-generated/builtins-array-lastindexof-from-dsl-gen.h"
#include "torque-generated/builtins-array-of-from-dsl-gen.h"
#include "torque-generated/builtins-array-map-from-dsl-gen.h"
#include "torque-generated/builtins-array-reverse-from-dsl-gen.h"
#include "torque-generated/builtins-array-shift-from-dsl-gen.h"
#include "torque-generated/builtins-array-slice-from-dsl-gen.h"
#include "torque-generated/builtins-array-splice-from-dsl-gen.h"
#include "torque-generated/builtins-array-unshift-from-dsl-gen.h"
#include "torque-generated/builtins-collections-from-dsl-gen.h"
#include "torque-generated/builtins-data-view-from-dsl-gen.h"
#include "torque-generated/builtins-extras-utils-from-dsl-gen.h"
#include "torque-generated/builtins-iterator-from-dsl-gen.h"
#include "torque-generated/builtins-object-from-dsl-gen.h"
#include "torque-generated/builtins-proxy-from-dsl-gen.h"
#include "torque-generated/builtins-regexp-from-dsl-gen.h"
#include "torque-generated/builtins-regexp-replace-from-dsl-gen.h"
#include "torque-generated/builtins-string-from-dsl-gen.h"
#include "torque-generated/builtins-string-html-from-dsl-gen.h"
#include "torque-generated/builtins-string-repeat-from-dsl-gen.h"
#include "torque-generated/builtins-typed-array-from-dsl-gen.h"
#include "torque-generated/builtins-typed-array-createtypedarray-from-dsl-gen.h"
#include "torque-generated/builtins-typed-array-every-from-dsl-gen.h"
#include "torque-generated/builtins-typed-array-filter-from-dsl-gen.h"
#include "torque-generated/builtins-typed-array-find-from-dsl-gen.h"
#include "torque-generated/builtins-typed-array-findindex-from-dsl-gen.h"
#include "torque-generated/builtins-typed-array-foreach-from-dsl-gen.h"
#include "torque-generated/builtins-typed-array-reduce-from-dsl-gen.h"
#include "torque-generated/builtins-typed-array-reduceright-from-dsl-gen.h"
#include "torque-generated/builtins-typed-array-slice-from-dsl-gen.h"
#include "torque-generated/builtins-typed-array-some-from-dsl-gen.h"
#include "torque-generated/builtins-typed-array-subarray-from-dsl-gen.h"
#include "torque-generated/builtins-test-from-dsl-gen.h"

namespace v8 {
namespace internal {

ArgumentsBuiltinsFromDSLAssembler::ArgumentsInfo ArgumentsBuiltinsFromDSLAssembler::GetArgumentsFrameAndCount(compiler::TNode<Context> p_context, compiler::TNode<JSFunction> p_f) {
  compiler::CodeAssemblerParameterizedLabel<Context, JSFunction> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSFunction, RawPtrT> block3(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSFunction, RawPtrT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSFunction, RawPtrT, SharedFunctionInfo, BInt, BInt, RawPtrT> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSFunction, RawPtrT, SharedFunctionInfo, BInt, BInt, RawPtrT, RawPtrT> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSFunction, RawPtrT, SharedFunctionInfo, BInt, BInt> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSFunction, RawPtrT, SharedFunctionInfo, BInt, BInt, RawPtrT> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSFunction, RawPtrT, BInt, BInt> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSFunction, RawPtrT, BInt, BInt> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_f);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSFunction> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../src/builtins/arguments.tq", 26);
    compiler::TNode<RawPtrT> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<RawPtrT>(CodeStubAssembler(state_).LoadParentFramePointer());
    ca_.SetSourcePosition("../../src/builtins/arguments.tq", 27);
    compiler::TNode<JSFunction> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<JSFunction>(BaseBuiltinsFromDSLAssembler(state_).LoadFunctionFromFrame(compiler::TNode<RawPtrT>{tmp2}));
    compiler::TNode<BoolT> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<HeapObject>{tmp3}, compiler::TNode<HeapObject>{tmp1}));
    ca_.Branch(tmp4, &block2, &block3, tmp0, tmp1, tmp2);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp5;
    compiler::TNode<JSFunction> tmp6;
    compiler::TNode<RawPtrT> tmp7;
    ca_.Bind(&block3, &tmp5, &tmp6, &tmp7);
    CodeStubAssembler(state_).FailAssert("Torque assert \'frame.function == f\' failed", "../../src/builtins/arguments.tq", 27);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp8;
    compiler::TNode<JSFunction> tmp9;
    compiler::TNode<RawPtrT> tmp10;
    ca_.Bind(&block2, &tmp8, &tmp9, &tmp10);
    ca_.SetSourcePosition("../../src/builtins/arguments.tq", 29);
    compiler::TNode<SharedFunctionInfo> tmp11;
    USE(tmp11);
    tmp11 = ca_.UncheckedCast<SharedFunctionInfo>(BaseBuiltinsFromDSLAssembler(state_).LoadJSFunctionSharedFunctionInfo(compiler::TNode<JSFunction>{tmp9}));
    ca_.SetSourcePosition("../../src/builtins/arguments.tq", 31);
    compiler::TNode<Uint32T> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).LoadSharedFunctionInfoFormalParameterCount(compiler::TNode<SharedFunctionInfo>{tmp11}));
    compiler::TNode<Int32T> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).Convert7ATint328ATuint16(compiler::TNode<Uint32T>{tmp12}));
    compiler::TNode<BInt> tmp14;
    USE(tmp14);
    tmp14 = ca_.UncheckedCast<BInt>(BaseBuiltinsFromDSLAssembler(state_).Convert6ATbint7ATint32(compiler::TNode<Int32T>{tmp13}));
    ca_.SetSourcePosition("../../src/builtins/arguments.tq", 30);
    ca_.SetSourcePosition("../../src/builtins/arguments.tq", 32);
    ca_.SetSourcePosition("../../src/builtins/arguments.tq", 35);
    compiler::TNode<RawPtrT> tmp15;
    USE(tmp15);
    tmp15 = ca_.UncheckedCast<RawPtrT>(BaseBuiltinsFromDSLAssembler(state_).LoadCallerFromFrame(compiler::TNode<RawPtrT>{tmp10}));
    compiler::TNode<RawPtrT> tmp16;
    USE(tmp16);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp16 = BaseBuiltinsFromDSLAssembler(state_).Cast23ATArgumentsAdaptorFrame(compiler::TNode<Context>{tmp8}, compiler::TNode<RawPtrT>{tmp15}, &label0);
    ca_.Goto(&block6, tmp8, tmp9, tmp10, tmp11, tmp14, tmp14, tmp15, tmp16);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block7, tmp8, tmp9, tmp10, tmp11, tmp14, tmp14, tmp15);
    }
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp17;
    compiler::TNode<JSFunction> tmp18;
    compiler::TNode<RawPtrT> tmp19;
    compiler::TNode<SharedFunctionInfo> tmp20;
    compiler::TNode<BInt> tmp21;
    compiler::TNode<BInt> tmp22;
    compiler::TNode<RawPtrT> tmp23;
    ca_.Bind(&block7, &tmp17, &tmp18, &tmp19, &tmp20, &tmp21, &tmp22, &tmp23);
    ca_.Goto(&block5, tmp17, tmp18, tmp19, tmp20, tmp21, tmp22);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp24;
    compiler::TNode<JSFunction> tmp25;
    compiler::TNode<RawPtrT> tmp26;
    compiler::TNode<SharedFunctionInfo> tmp27;
    compiler::TNode<BInt> tmp28;
    compiler::TNode<BInt> tmp29;
    compiler::TNode<RawPtrT> tmp30;
    compiler::TNode<RawPtrT> tmp31;
    ca_.Bind(&block6, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31);
    ca_.Goto(&block4, tmp24, tmp25, tmp26, tmp27, tmp28, tmp29, tmp31);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp32;
    compiler::TNode<JSFunction> tmp33;
    compiler::TNode<RawPtrT> tmp34;
    compiler::TNode<SharedFunctionInfo> tmp35;
    compiler::TNode<BInt> tmp36;
    compiler::TNode<BInt> tmp37;
    ca_.Bind(&block5, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37);
    ca_.SetSourcePosition("../../src/builtins/arguments.tq", 36);
    ca_.Goto(&block1, tmp32, tmp33, tmp34, tmp37, tmp36);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp38;
    compiler::TNode<JSFunction> tmp39;
    compiler::TNode<RawPtrT> tmp40;
    compiler::TNode<SharedFunctionInfo> tmp41;
    compiler::TNode<BInt> tmp42;
    compiler::TNode<BInt> tmp43;
    compiler::TNode<RawPtrT> tmp44;
    ca_.Bind(&block4, &tmp38, &tmp39, &tmp40, &tmp41, &tmp42, &tmp43, &tmp44);
    ca_.SetSourcePosition("../../src/builtins/arguments.tq", 34);
    ca_.SetSourcePosition("../../src/builtins/arguments.tq", 39);
    ca_.SetSourcePosition("../../src/builtins/arguments.tq", 40);
    compiler::TNode<Smi> tmp45;
    USE(tmp45);
    tmp45 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).LoadLengthFromAdapterFrame(compiler::TNode<Context>{tmp38}, compiler::TNode<RawPtrT>{tmp44}));
    compiler::TNode<BInt> tmp46;
    USE(tmp46);
    tmp46 = ca_.UncheckedCast<BInt>(BaseBuiltinsFromDSLAssembler(state_).Convert6ATbint5ATSmi(compiler::TNode<Smi>{tmp45}));
    ca_.SetSourcePosition("../../src/builtins/arguments.tq", 41);
    ca_.SetSourcePosition("../../src/builtins/arguments.tq", 38);
    ca_.Goto(&block1, tmp38, tmp39, tmp44, tmp46, tmp42);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp47;
    compiler::TNode<JSFunction> tmp48;
    compiler::TNode<RawPtrT> tmp49;
    compiler::TNode<BInt> tmp50;
    compiler::TNode<BInt> tmp51;
    ca_.Bind(&block1, &tmp47, &tmp48, &tmp49, &tmp50, &tmp51);
    ca_.SetSourcePosition("../../src/builtins/arguments.tq", 24);
    ca_.Goto(&block8, tmp47, tmp48, tmp49, tmp50, tmp51);
  }

    compiler::TNode<Context> tmp52;
    compiler::TNode<JSFunction> tmp53;
    compiler::TNode<RawPtrT> tmp54;
    compiler::TNode<BInt> tmp55;
    compiler::TNode<BInt> tmp56;
    ca_.Bind(&block8, &tmp52, &tmp53, &tmp54, &tmp55, &tmp56);
  return ArgumentsBuiltinsFromDSLAssembler::ArgumentsInfo{compiler::TNode<RawPtrT>{tmp54}, compiler::TNode<BInt>{tmp55}, compiler::TNode<BInt>{tmp56}};
}

}  // namespace internal
}  // namespace v8

