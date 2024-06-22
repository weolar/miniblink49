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
#include "src/builtins/builtins-regexp-gen.h"
#include "src/builtins/builtins-array-gen.h"
#include "src/builtins/builtins-collections-gen.h"
#include "src/builtins/builtins-data-view-gen.h"
#include "src/builtins/builtins-iterator-gen.h"
#include "src/builtins/builtins-proxy-gen.h"
#include "src/builtins/builtins-proxy-gen.h"
#include "src/builtins/builtins-proxy-gen.h"
#include "src/builtins/builtins-proxy-gen.h"
#include "src/builtins/builtins-regexp-gen.h"
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
  compiler::CodeAssemblerParameterizedLabel<Context, JSFunction, RawPtrT, SharedFunctionInfo, BInt, BInt, RawPtrT> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSFunction, RawPtrT, SharedFunctionInfo, BInt, BInt, RawPtrT, RawPtrT> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSFunction, RawPtrT, SharedFunctionInfo, BInt, BInt> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSFunction, RawPtrT, SharedFunctionInfo, BInt, BInt, RawPtrT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSFunction, RawPtrT, BInt, BInt> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, JSFunction, RawPtrT, BInt, BInt> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_f);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<JSFunction> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../v8/src/builtins/arguments.tq", 36);
    compiler::TNode<RawPtrT> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<RawPtrT>(CodeStubAssembler(state_).LoadParentFramePointer());
    ca_.SetSourcePosition("../../v8/src/builtins/arguments.tq", 39);
    compiler::TNode<IntPtrT> tmp3 = ca_.IntPtrConstant(JSFunction::kSharedFunctionInfoOffset);
    USE(tmp3);
    compiler::TNode<SharedFunctionInfo>tmp4 = CodeStubAssembler(state_).LoadReference<SharedFunctionInfo>(CodeStubAssembler::Reference{tmp1, tmp3});
    ca_.SetSourcePosition("../../v8/src/builtins/arguments.tq", 41);
    compiler::TNode<IntPtrT> tmp5 = ca_.IntPtrConstant(SharedFunctionInfo::kFormalParameterCountOffset);
    USE(tmp5);
    compiler::TNode<Uint16T>tmp6 = CodeStubAssembler(state_).LoadReference<Uint16T>(CodeStubAssembler::Reference{tmp4, tmp5});
    compiler::TNode<Int32T> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).Convert7ATint328ATuint16(compiler::TNode<Uint16T>{tmp6}));
    compiler::TNode<BInt> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<BInt>(BaseBuiltinsFromDSLAssembler(state_).Convert6ATbint7ATint32(compiler::TNode<Int32T>{tmp7}));
    ca_.SetSourcePosition("../../v8/src/builtins/arguments.tq", 40);
    ca_.SetSourcePosition("../../v8/src/builtins/arguments.tq", 42);
    ca_.SetSourcePosition("../../v8/src/builtins/arguments.tq", 45);
    compiler::TNode<RawPtrT> tmp9;
    USE(tmp9);
    tmp9 = ca_.UncheckedCast<RawPtrT>(BaseBuiltinsFromDSLAssembler(state_).LoadCallerFromFrame(compiler::TNode<RawPtrT>{tmp2}));
    compiler::TNode<RawPtrT> tmp10;
    USE(tmp10);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp10 = BaseBuiltinsFromDSLAssembler(state_).Cast23ATArgumentsAdaptorFrame(compiler::TNode<Context>{tmp0}, compiler::TNode<RawPtrT>{tmp9}, &label0);
    ca_.Goto(&block4, tmp0, tmp1, tmp2, tmp4, tmp8, tmp8, tmp9, tmp10);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block5, tmp0, tmp1, tmp2, tmp4, tmp8, tmp8, tmp9);
    }
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp11;
    compiler::TNode<JSFunction> tmp12;
    compiler::TNode<RawPtrT> tmp13;
    compiler::TNode<SharedFunctionInfo> tmp14;
    compiler::TNode<BInt> tmp15;
    compiler::TNode<BInt> tmp16;
    compiler::TNode<RawPtrT> tmp17;
    ca_.Bind(&block5, &tmp11, &tmp12, &tmp13, &tmp14, &tmp15, &tmp16, &tmp17);
    ca_.Goto(&block3, tmp11, tmp12, tmp13, tmp14, tmp15, tmp16);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp18;
    compiler::TNode<JSFunction> tmp19;
    compiler::TNode<RawPtrT> tmp20;
    compiler::TNode<SharedFunctionInfo> tmp21;
    compiler::TNode<BInt> tmp22;
    compiler::TNode<BInt> tmp23;
    compiler::TNode<RawPtrT> tmp24;
    compiler::TNode<RawPtrT> tmp25;
    ca_.Bind(&block4, &tmp18, &tmp19, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25);
    ca_.Goto(&block2, tmp18, tmp19, tmp20, tmp21, tmp22, tmp23, tmp25);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp26;
    compiler::TNode<JSFunction> tmp27;
    compiler::TNode<RawPtrT> tmp28;
    compiler::TNode<SharedFunctionInfo> tmp29;
    compiler::TNode<BInt> tmp30;
    compiler::TNode<BInt> tmp31;
    ca_.Bind(&block3, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31);
    ca_.SetSourcePosition("../../v8/src/builtins/arguments.tq", 47);
    ca_.SetSourcePosition("../../v8/src/builtins/arguments.tq", 48);
    ca_.SetSourcePosition("../../v8/src/builtins/arguments.tq", 49);
    ca_.SetSourcePosition("../../v8/src/builtins/arguments.tq", 46);
    ca_.Goto(&block1, tmp26, tmp27, tmp28, tmp31, tmp30);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp32;
    compiler::TNode<JSFunction> tmp33;
    compiler::TNode<RawPtrT> tmp34;
    compiler::TNode<SharedFunctionInfo> tmp35;
    compiler::TNode<BInt> tmp36;
    compiler::TNode<BInt> tmp37;
    compiler::TNode<RawPtrT> tmp38;
    ca_.Bind(&block2, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38);
    ca_.SetSourcePosition("../../v8/src/builtins/arguments.tq", 44);
    ca_.SetSourcePosition("../../v8/src/builtins/arguments.tq", 53);
    ca_.SetSourcePosition("../../v8/src/builtins/arguments.tq", 54);
    compiler::TNode<Smi> tmp39;
    USE(tmp39);
    tmp39 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).LoadLengthFromAdapterFrame(compiler::TNode<Context>{tmp32}, compiler::TNode<RawPtrT>{tmp38}));
    compiler::TNode<BInt> tmp40;
    USE(tmp40);
    tmp40 = ca_.UncheckedCast<BInt>(BaseBuiltinsFromDSLAssembler(state_).Convert6ATbint5ATSmi(compiler::TNode<Smi>{tmp39}));
    ca_.SetSourcePosition("../../v8/src/builtins/arguments.tq", 55);
    ca_.SetSourcePosition("../../v8/src/builtins/arguments.tq", 52);
    ca_.Goto(&block1, tmp32, tmp33, tmp38, tmp40, tmp36);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp41;
    compiler::TNode<JSFunction> tmp42;
    compiler::TNode<RawPtrT> tmp43;
    compiler::TNode<BInt> tmp44;
    compiler::TNode<BInt> tmp45;
    ca_.Bind(&block1, &tmp41, &tmp42, &tmp43, &tmp44, &tmp45);
    ca_.SetSourcePosition("../../v8/src/builtins/arguments.tq", 34);
    ca_.Goto(&block6, tmp41, tmp42, tmp43, tmp44, tmp45);
  }

    compiler::TNode<Context> tmp46;
    compiler::TNode<JSFunction> tmp47;
    compiler::TNode<RawPtrT> tmp48;
    compiler::TNode<BInt> tmp49;
    compiler::TNode<BInt> tmp50;
    ca_.Bind(&block6, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50);
  return ArgumentsBuiltinsFromDSLAssembler::ArgumentsInfo{compiler::TNode<RawPtrT>{tmp48}, compiler::TNode<BInt>{tmp49}, compiler::TNode<BInt>{tmp50}};
}

}  // namespace internal
}  // namespace v8

