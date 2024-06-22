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

compiler::TNode<Number> ArrayCopywithinBuiltinsFromDSLAssembler::ConvertToRelativeIndex(compiler::TNode<Number> p_index, compiler::TNode<Number> p_length) {
  compiler::CodeAssemblerParameterizedLabel<Number, Number> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Number, Number, Number> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Number, Number, Number> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Number, Number> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Number, Number> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Number, Number, Number> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Number, Number, Number> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Number, Number, Number> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Number, Number, Number> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_index, p_length);

  if (block0.is_used()) {
    compiler::TNode<Number> tmp0;
    compiler::TNode<Number> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../v8/src/builtins/array-copywithin.tq", 7);
    compiler::TNode<Number> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(0));
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    CodeStubAssembler(state_).BranchIfNumberLessThan(compiler::TNode<Number>{tmp0}, compiler::TNode<Number>{tmp2}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block6, tmp0, tmp1, tmp0);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block7, tmp0, tmp1, tmp0);
    }
  }

  if (block6.is_used()) {
    compiler::TNode<Number> tmp3;
    compiler::TNode<Number> tmp4;
    compiler::TNode<Number> tmp5;
    ca_.Bind(&block6, &tmp3, &tmp4, &tmp5);
    ca_.Goto(&block2, tmp3, tmp4);
  }

  if (block7.is_used()) {
    compiler::TNode<Number> tmp6;
    compiler::TNode<Number> tmp7;
    compiler::TNode<Number> tmp8;
    ca_.Bind(&block7, &tmp6, &tmp7, &tmp8);
    ca_.Goto(&block3, tmp6, tmp7);
  }

  if (block2.is_used()) {
    compiler::TNode<Number> tmp9;
    compiler::TNode<Number> tmp10;
    ca_.Bind(&block2, &tmp9, &tmp10);
    compiler::TNode<Number> tmp11;
    USE(tmp11);
    tmp11 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp9}, compiler::TNode<Number>{tmp10}));
    compiler::TNode<Number> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(0));
    compiler::TNode<Number> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).Max(compiler::TNode<Number>{tmp11}, compiler::TNode<Number>{tmp12}));
    ca_.Goto(&block5, tmp9, tmp10, tmp13);
  }

  if (block3.is_used()) {
    compiler::TNode<Number> tmp14;
    compiler::TNode<Number> tmp15;
    ca_.Bind(&block3, &tmp14, &tmp15);
    compiler::TNode<Number> tmp16;
    USE(tmp16);
    tmp16 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).Min(compiler::TNode<Number>{tmp14}, compiler::TNode<Number>{tmp15}));
    ca_.Goto(&block4, tmp14, tmp15, tmp16);
  }

  if (block5.is_used()) {
    compiler::TNode<Number> tmp17;
    compiler::TNode<Number> tmp18;
    compiler::TNode<Number> tmp19;
    ca_.Bind(&block5, &tmp17, &tmp18, &tmp19);
    ca_.Goto(&block4, tmp17, tmp18, tmp19);
  }

  if (block4.is_used()) {
    compiler::TNode<Number> tmp20;
    compiler::TNode<Number> tmp21;
    compiler::TNode<Number> tmp22;
    ca_.Bind(&block4, &tmp20, &tmp21, &tmp22);
    ca_.Goto(&block1, tmp20, tmp21, tmp22);
  }

  if (block1.is_used()) {
    compiler::TNode<Number> tmp23;
    compiler::TNode<Number> tmp24;
    compiler::TNode<Number> tmp25;
    ca_.Bind(&block1, &tmp23, &tmp24, &tmp25);
    ca_.SetSourcePosition("../../v8/src/builtins/array-copywithin.tq", 6);
    ca_.Goto(&block8, tmp23, tmp24, tmp25);
  }

    compiler::TNode<Number> tmp26;
    compiler::TNode<Number> tmp27;
    compiler::TNode<Number> tmp28;
    ca_.Bind(&block8, &tmp26, &tmp27, &tmp28);
  return compiler::TNode<Number>{tmp28};
}

TF_BUILTIN(ArrayPrototypeCopyWithin, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  Node* argc = Parameter(Descriptor::kJSActualArgumentsCount);
  TNode<IntPtrT> arguments_length(ChangeInt32ToIntPtr(argc));
  TNode<RawPtrT> arguments_frame = UncheckedCast<RawPtrT>(LoadFramePointer());
  BaseBuiltinsFromDSLAssembler::Arguments torque_arguments(GetFrameArguments(arguments_frame, arguments_length));
  CodeStubArguments arguments(this, torque_arguments);
  TNode<Object> parameter1 = arguments.GetReceiver();
USE(parameter1);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Number, Number, Number, Number> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Number, Number, Number, Number> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Number, Number, Number, Number, Number, Number, Number, Number, Number> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Number, Number, Number, Number, Number, Number, Number, Number, Number> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Number, Number, Number, Number, Number, Number, Number> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Number, Number, Number, Number, Number, Number, Number, Number, Number> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Number, Number, Number, Number, Number, Number, Number, Number, Number> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Number, Number, Number, Number, Number, Number, Number> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Number, Number, Number, Number, Number, Number, Number> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Number, Number, Number, Number, Number, Number, Number> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Number, Number, Number, Number, Number, Number, Number, Number> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Number, Number, Number, Number, Number, Number, Number, Number> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Number, Number, Number, Number, Number, Number, Number> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Number, Number, Number, Number, Number, Number, Number, Oddball> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Number, Number, Number, Number, Number, Number, Number, Oddball> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Number, Number, Number, Number, Number, Number, Number, Oddball> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSReceiver, Number, Number, Number, Number, Number, Number, Number, Number, Number> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, torque_arguments.frame, torque_arguments.base, torque_arguments.length);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<RawPtrT> tmp2;
    compiler::TNode<RawPtrT> tmp3;
    compiler::TNode<IntPtrT> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../v8/src/builtins/array-copywithin.tq", 14);
    compiler::TNode<JSReceiver> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<JSReceiver>(CodeStubAssembler(state_).ToObject_Inline(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-copywithin.tq", 17);
    compiler::TNode<Number> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).GetLengthProperty(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp5}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-copywithin.tq", 20);
    compiler::TNode<IntPtrT> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp2}, compiler::TNode<RawPtrT>{tmp3}, compiler::TNode<IntPtrT>{tmp4}}, compiler::TNode<IntPtrT>{tmp7}));
    compiler::TNode<Number> tmp9;
    USE(tmp9);
    tmp9 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).ToInteger_Inline(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp8}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-copywithin.tq", 24);
    compiler::TNode<Number> tmp10;
    USE(tmp10);
    tmp10 = ca_.UncheckedCast<Number>(ArrayCopywithinBuiltinsFromDSLAssembler(state_).ConvertToRelativeIndex(compiler::TNode<Number>{tmp9}, compiler::TNode<Number>{tmp6}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-copywithin.tq", 27);
    compiler::TNode<IntPtrT> tmp11;
    USE(tmp11);
    tmp11 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<Object> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp2}, compiler::TNode<RawPtrT>{tmp3}, compiler::TNode<IntPtrT>{tmp4}}, compiler::TNode<IntPtrT>{tmp11}));
    compiler::TNode<Number> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).ToInteger_Inline(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp12}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-copywithin.tq", 31);
    compiler::TNode<Number> tmp14;
    USE(tmp14);
    tmp14 = ca_.UncheckedCast<Number>(ArrayCopywithinBuiltinsFromDSLAssembler(state_).ConvertToRelativeIndex(compiler::TNode<Number>{tmp13}, compiler::TNode<Number>{tmp6}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-copywithin.tq", 35);
    ca_.SetSourcePosition("../../v8/src/builtins/array-copywithin.tq", 36);
    compiler::TNode<IntPtrT> tmp15;
    USE(tmp15);
    tmp15 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(2));
    compiler::TNode<Object> tmp16;
    USE(tmp16);
    tmp16 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp2}, compiler::TNode<RawPtrT>{tmp3}, compiler::TNode<IntPtrT>{tmp4}}, compiler::TNode<IntPtrT>{tmp15}));
    compiler::TNode<Oddball> tmp17;
    USE(tmp17);
    tmp17 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    compiler::TNode<BoolT> tmp18;
    USE(tmp18);
    tmp18 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordNotEqual(compiler::TNode<Object>{tmp16}, compiler::TNode<HeapObject>{tmp17}));
    ca_.Branch(tmp18, &block1, &block2, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp9, tmp10, tmp13, tmp14, tmp6);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp19;
    compiler::TNode<Object> tmp20;
    compiler::TNode<RawPtrT> tmp21;
    compiler::TNode<RawPtrT> tmp22;
    compiler::TNode<IntPtrT> tmp23;
    compiler::TNode<JSReceiver> tmp24;
    compiler::TNode<Number> tmp25;
    compiler::TNode<Number> tmp26;
    compiler::TNode<Number> tmp27;
    compiler::TNode<Number> tmp28;
    compiler::TNode<Number> tmp29;
    compiler::TNode<Number> tmp30;
    ca_.Bind(&block1, &tmp19, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30);
    ca_.SetSourcePosition("../../v8/src/builtins/array-copywithin.tq", 37);
    compiler::TNode<IntPtrT> tmp31;
    USE(tmp31);
    tmp31 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(2));
    compiler::TNode<Object> tmp32;
    USE(tmp32);
    tmp32 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp21}, compiler::TNode<RawPtrT>{tmp22}, compiler::TNode<IntPtrT>{tmp23}}, compiler::TNode<IntPtrT>{tmp31}));
    compiler::TNode<Number> tmp33;
    USE(tmp33);
    tmp33 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).ToInteger_Inline(compiler::TNode<Context>{tmp19}, compiler::TNode<Object>{tmp32}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-copywithin.tq", 36);
    ca_.Goto(&block2, tmp19, tmp20, tmp21, tmp22, tmp23, tmp24, tmp25, tmp26, tmp27, tmp28, tmp29, tmp33);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp34;
    compiler::TNode<Object> tmp35;
    compiler::TNode<RawPtrT> tmp36;
    compiler::TNode<RawPtrT> tmp37;
    compiler::TNode<IntPtrT> tmp38;
    compiler::TNode<JSReceiver> tmp39;
    compiler::TNode<Number> tmp40;
    compiler::TNode<Number> tmp41;
    compiler::TNode<Number> tmp42;
    compiler::TNode<Number> tmp43;
    compiler::TNode<Number> tmp44;
    compiler::TNode<Number> tmp45;
    ca_.Bind(&block2, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41, &tmp42, &tmp43, &tmp44, &tmp45);
    ca_.SetSourcePosition("../../v8/src/builtins/array-copywithin.tq", 42);
    compiler::TNode<Number> tmp46;
    USE(tmp46);
    tmp46 = ca_.UncheckedCast<Number>(ArrayCopywithinBuiltinsFromDSLAssembler(state_).ConvertToRelativeIndex(compiler::TNode<Number>{tmp45}, compiler::TNode<Number>{tmp40}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-copywithin.tq", 45);
    compiler::TNode<Number> tmp47;
    USE(tmp47);
    tmp47 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberSub(compiler::TNode<Number>{tmp46}, compiler::TNode<Number>{tmp44}));
    compiler::TNode<Number> tmp48;
    USE(tmp48);
    tmp48 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberSub(compiler::TNode<Number>{tmp40}, compiler::TNode<Number>{tmp42}));
    compiler::TNode<Number> tmp49;
    USE(tmp49);
    tmp49 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).Min(compiler::TNode<Number>{tmp47}, compiler::TNode<Number>{tmp48}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-copywithin.tq", 48);
    compiler::TNode<Number> tmp50;
    USE(tmp50);
    tmp50 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(1));
    ca_.SetSourcePosition("../../v8/src/builtins/array-copywithin.tq", 50);
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    CodeStubAssembler(state_).BranchIfNumberLessThan(compiler::TNode<Number>{tmp44}, compiler::TNode<Number>{tmp42}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block6, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39, tmp40, tmp41, tmp42, tmp43, tmp44, tmp45, tmp46, tmp49, tmp50, tmp44, tmp42);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block7, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39, tmp40, tmp41, tmp42, tmp43, tmp44, tmp45, tmp46, tmp49, tmp50, tmp44, tmp42);
    }
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp51;
    compiler::TNode<Object> tmp52;
    compiler::TNode<RawPtrT> tmp53;
    compiler::TNode<RawPtrT> tmp54;
    compiler::TNode<IntPtrT> tmp55;
    compiler::TNode<JSReceiver> tmp56;
    compiler::TNode<Number> tmp57;
    compiler::TNode<Number> tmp58;
    compiler::TNode<Number> tmp59;
    compiler::TNode<Number> tmp60;
    compiler::TNode<Number> tmp61;
    compiler::TNode<Number> tmp62;
    compiler::TNode<Number> tmp63;
    compiler::TNode<Number> tmp64;
    compiler::TNode<Number> tmp65;
    compiler::TNode<Number> tmp66;
    compiler::TNode<Number> tmp67;
    ca_.Bind(&block6, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65, &tmp66, &tmp67);
    ca_.Goto(&block5, tmp51, tmp52, tmp53, tmp54, tmp55, tmp56, tmp57, tmp58, tmp59, tmp60, tmp61, tmp62, tmp63, tmp64, tmp65);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp68;
    compiler::TNode<Object> tmp69;
    compiler::TNode<RawPtrT> tmp70;
    compiler::TNode<RawPtrT> tmp71;
    compiler::TNode<IntPtrT> tmp72;
    compiler::TNode<JSReceiver> tmp73;
    compiler::TNode<Number> tmp74;
    compiler::TNode<Number> tmp75;
    compiler::TNode<Number> tmp76;
    compiler::TNode<Number> tmp77;
    compiler::TNode<Number> tmp78;
    compiler::TNode<Number> tmp79;
    compiler::TNode<Number> tmp80;
    compiler::TNode<Number> tmp81;
    compiler::TNode<Number> tmp82;
    compiler::TNode<Number> tmp83;
    compiler::TNode<Number> tmp84;
    ca_.Bind(&block7, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72, &tmp73, &tmp74, &tmp75, &tmp76, &tmp77, &tmp78, &tmp79, &tmp80, &tmp81, &tmp82, &tmp83, &tmp84);
    ca_.Goto(&block4, tmp68, tmp69, tmp70, tmp71, tmp72, tmp73, tmp74, tmp75, tmp76, tmp77, tmp78, tmp79, tmp80, tmp81, tmp82);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp85;
    compiler::TNode<Object> tmp86;
    compiler::TNode<RawPtrT> tmp87;
    compiler::TNode<RawPtrT> tmp88;
    compiler::TNode<IntPtrT> tmp89;
    compiler::TNode<JSReceiver> tmp90;
    compiler::TNode<Number> tmp91;
    compiler::TNode<Number> tmp92;
    compiler::TNode<Number> tmp93;
    compiler::TNode<Number> tmp94;
    compiler::TNode<Number> tmp95;
    compiler::TNode<Number> tmp96;
    compiler::TNode<Number> tmp97;
    compiler::TNode<Number> tmp98;
    compiler::TNode<Number> tmp99;
    ca_.Bind(&block5, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89, &tmp90, &tmp91, &tmp92, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97, &tmp98, &tmp99);
    compiler::TNode<Number> tmp100;
    USE(tmp100);
    tmp100 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp95}, compiler::TNode<Number>{tmp98}));
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    CodeStubAssembler(state_).BranchIfNumberLessThan(compiler::TNode<Number>{tmp93}, compiler::TNode<Number>{tmp100}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block8, tmp85, tmp86, tmp87, tmp88, tmp89, tmp90, tmp91, tmp92, tmp93, tmp94, tmp95, tmp96, tmp97, tmp98, tmp99, tmp93, tmp100);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block9, tmp85, tmp86, tmp87, tmp88, tmp89, tmp90, tmp91, tmp92, tmp93, tmp94, tmp95, tmp96, tmp97, tmp98, tmp99, tmp93, tmp100);
    }
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp101;
    compiler::TNode<Object> tmp102;
    compiler::TNode<RawPtrT> tmp103;
    compiler::TNode<RawPtrT> tmp104;
    compiler::TNode<IntPtrT> tmp105;
    compiler::TNode<JSReceiver> tmp106;
    compiler::TNode<Number> tmp107;
    compiler::TNode<Number> tmp108;
    compiler::TNode<Number> tmp109;
    compiler::TNode<Number> tmp110;
    compiler::TNode<Number> tmp111;
    compiler::TNode<Number> tmp112;
    compiler::TNode<Number> tmp113;
    compiler::TNode<Number> tmp114;
    compiler::TNode<Number> tmp115;
    compiler::TNode<Number> tmp116;
    compiler::TNode<Number> tmp117;
    ca_.Bind(&block8, &tmp101, &tmp102, &tmp103, &tmp104, &tmp105, &tmp106, &tmp107, &tmp108, &tmp109, &tmp110, &tmp111, &tmp112, &tmp113, &tmp114, &tmp115, &tmp116, &tmp117);
    ca_.Goto(&block3, tmp101, tmp102, tmp103, tmp104, tmp105, tmp106, tmp107, tmp108, tmp109, tmp110, tmp111, tmp112, tmp113, tmp114, tmp115);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp118;
    compiler::TNode<Object> tmp119;
    compiler::TNode<RawPtrT> tmp120;
    compiler::TNode<RawPtrT> tmp121;
    compiler::TNode<IntPtrT> tmp122;
    compiler::TNode<JSReceiver> tmp123;
    compiler::TNode<Number> tmp124;
    compiler::TNode<Number> tmp125;
    compiler::TNode<Number> tmp126;
    compiler::TNode<Number> tmp127;
    compiler::TNode<Number> tmp128;
    compiler::TNode<Number> tmp129;
    compiler::TNode<Number> tmp130;
    compiler::TNode<Number> tmp131;
    compiler::TNode<Number> tmp132;
    compiler::TNode<Number> tmp133;
    compiler::TNode<Number> tmp134;
    ca_.Bind(&block9, &tmp118, &tmp119, &tmp120, &tmp121, &tmp122, &tmp123, &tmp124, &tmp125, &tmp126, &tmp127, &tmp128, &tmp129, &tmp130, &tmp131, &tmp132, &tmp133, &tmp134);
    ca_.Goto(&block4, tmp118, tmp119, tmp120, tmp121, tmp122, tmp123, tmp124, tmp125, tmp126, tmp127, tmp128, tmp129, tmp130, tmp131, tmp132);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp135;
    compiler::TNode<Object> tmp136;
    compiler::TNode<RawPtrT> tmp137;
    compiler::TNode<RawPtrT> tmp138;
    compiler::TNode<IntPtrT> tmp139;
    compiler::TNode<JSReceiver> tmp140;
    compiler::TNode<Number> tmp141;
    compiler::TNode<Number> tmp142;
    compiler::TNode<Number> tmp143;
    compiler::TNode<Number> tmp144;
    compiler::TNode<Number> tmp145;
    compiler::TNode<Number> tmp146;
    compiler::TNode<Number> tmp147;
    compiler::TNode<Number> tmp148;
    compiler::TNode<Number> tmp149;
    ca_.Bind(&block3, &tmp135, &tmp136, &tmp137, &tmp138, &tmp139, &tmp140, &tmp141, &tmp142, &tmp143, &tmp144, &tmp145, &tmp146, &tmp147, &tmp148, &tmp149);
    ca_.SetSourcePosition("../../v8/src/builtins/array-copywithin.tq", 52);
    compiler::TNode<Number> tmp150;
    USE(tmp150);
    tmp150 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(-1));
    ca_.SetSourcePosition("../../v8/src/builtins/array-copywithin.tq", 55);
    compiler::TNode<Number> tmp151;
    USE(tmp151);
    tmp151 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp145}, compiler::TNode<Number>{tmp148}));
    compiler::TNode<Number> tmp152;
    USE(tmp152);
    tmp152 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(1));
    compiler::TNode<Number> tmp153;
    USE(tmp153);
    tmp153 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberSub(compiler::TNode<Number>{tmp151}, compiler::TNode<Number>{tmp152}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-copywithin.tq", 58);
    compiler::TNode<Number> tmp154;
    USE(tmp154);
    tmp154 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp143}, compiler::TNode<Number>{tmp148}));
    compiler::TNode<Number> tmp155;
    USE(tmp155);
    tmp155 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(1));
    compiler::TNode<Number> tmp156;
    USE(tmp156);
    tmp156 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberSub(compiler::TNode<Number>{tmp154}, compiler::TNode<Number>{tmp155}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-copywithin.tq", 50);
    ca_.Goto(&block4, tmp135, tmp136, tmp137, tmp138, tmp139, tmp140, tmp141, tmp142, tmp156, tmp144, tmp153, tmp146, tmp147, tmp148, tmp150);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp157;
    compiler::TNode<Object> tmp158;
    compiler::TNode<RawPtrT> tmp159;
    compiler::TNode<RawPtrT> tmp160;
    compiler::TNode<IntPtrT> tmp161;
    compiler::TNode<JSReceiver> tmp162;
    compiler::TNode<Number> tmp163;
    compiler::TNode<Number> tmp164;
    compiler::TNode<Number> tmp165;
    compiler::TNode<Number> tmp166;
    compiler::TNode<Number> tmp167;
    compiler::TNode<Number> tmp168;
    compiler::TNode<Number> tmp169;
    compiler::TNode<Number> tmp170;
    compiler::TNode<Number> tmp171;
    ca_.Bind(&block4, &tmp157, &tmp158, &tmp159, &tmp160, &tmp161, &tmp162, &tmp163, &tmp164, &tmp165, &tmp166, &tmp167, &tmp168, &tmp169, &tmp170, &tmp171);
    ca_.SetSourcePosition("../../v8/src/builtins/array-copywithin.tq", 62);
    ca_.Goto(&block12, tmp157, tmp158, tmp159, tmp160, tmp161, tmp162, tmp163, tmp164, tmp165, tmp166, tmp167, tmp168, tmp169, tmp170, tmp171);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp172;
    compiler::TNode<Object> tmp173;
    compiler::TNode<RawPtrT> tmp174;
    compiler::TNode<RawPtrT> tmp175;
    compiler::TNode<IntPtrT> tmp176;
    compiler::TNode<JSReceiver> tmp177;
    compiler::TNode<Number> tmp178;
    compiler::TNode<Number> tmp179;
    compiler::TNode<Number> tmp180;
    compiler::TNode<Number> tmp181;
    compiler::TNode<Number> tmp182;
    compiler::TNode<Number> tmp183;
    compiler::TNode<Number> tmp184;
    compiler::TNode<Number> tmp185;
    compiler::TNode<Number> tmp186;
    ca_.Bind(&block12, &tmp172, &tmp173, &tmp174, &tmp175, &tmp176, &tmp177, &tmp178, &tmp179, &tmp180, &tmp181, &tmp182, &tmp183, &tmp184, &tmp185, &tmp186);
    compiler::TNode<Number> tmp187;
    USE(tmp187);
    tmp187 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(0));
    compiler::CodeAssemblerLabel label0(&ca_);
    compiler::CodeAssemblerLabel label1(&ca_);
    CodeStubAssembler(state_).BranchIfNumberGreaterThan(compiler::TNode<Number>{tmp185}, compiler::TNode<Number>{tmp187}, &label0, &label1);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block13, tmp172, tmp173, tmp174, tmp175, tmp176, tmp177, tmp178, tmp179, tmp180, tmp181, tmp182, tmp183, tmp184, tmp185, tmp186, tmp185);
    }
    if (label1.is_used()) {
      ca_.Bind(&label1);
      ca_.Goto(&block14, tmp172, tmp173, tmp174, tmp175, tmp176, tmp177, tmp178, tmp179, tmp180, tmp181, tmp182, tmp183, tmp184, tmp185, tmp186, tmp185);
    }
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp188;
    compiler::TNode<Object> tmp189;
    compiler::TNode<RawPtrT> tmp190;
    compiler::TNode<RawPtrT> tmp191;
    compiler::TNode<IntPtrT> tmp192;
    compiler::TNode<JSReceiver> tmp193;
    compiler::TNode<Number> tmp194;
    compiler::TNode<Number> tmp195;
    compiler::TNode<Number> tmp196;
    compiler::TNode<Number> tmp197;
    compiler::TNode<Number> tmp198;
    compiler::TNode<Number> tmp199;
    compiler::TNode<Number> tmp200;
    compiler::TNode<Number> tmp201;
    compiler::TNode<Number> tmp202;
    compiler::TNode<Number> tmp203;
    ca_.Bind(&block13, &tmp188, &tmp189, &tmp190, &tmp191, &tmp192, &tmp193, &tmp194, &tmp195, &tmp196, &tmp197, &tmp198, &tmp199, &tmp200, &tmp201, &tmp202, &tmp203);
    ca_.Goto(&block10, tmp188, tmp189, tmp190, tmp191, tmp192, tmp193, tmp194, tmp195, tmp196, tmp197, tmp198, tmp199, tmp200, tmp201, tmp202);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp204;
    compiler::TNode<Object> tmp205;
    compiler::TNode<RawPtrT> tmp206;
    compiler::TNode<RawPtrT> tmp207;
    compiler::TNode<IntPtrT> tmp208;
    compiler::TNode<JSReceiver> tmp209;
    compiler::TNode<Number> tmp210;
    compiler::TNode<Number> tmp211;
    compiler::TNode<Number> tmp212;
    compiler::TNode<Number> tmp213;
    compiler::TNode<Number> tmp214;
    compiler::TNode<Number> tmp215;
    compiler::TNode<Number> tmp216;
    compiler::TNode<Number> tmp217;
    compiler::TNode<Number> tmp218;
    compiler::TNode<Number> tmp219;
    ca_.Bind(&block14, &tmp204, &tmp205, &tmp206, &tmp207, &tmp208, &tmp209, &tmp210, &tmp211, &tmp212, &tmp213, &tmp214, &tmp215, &tmp216, &tmp217, &tmp218, &tmp219);
    ca_.Goto(&block11, tmp204, tmp205, tmp206, tmp207, tmp208, tmp209, tmp210, tmp211, tmp212, tmp213, tmp214, tmp215, tmp216, tmp217, tmp218);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp220;
    compiler::TNode<Object> tmp221;
    compiler::TNode<RawPtrT> tmp222;
    compiler::TNode<RawPtrT> tmp223;
    compiler::TNode<IntPtrT> tmp224;
    compiler::TNode<JSReceiver> tmp225;
    compiler::TNode<Number> tmp226;
    compiler::TNode<Number> tmp227;
    compiler::TNode<Number> tmp228;
    compiler::TNode<Number> tmp229;
    compiler::TNode<Number> tmp230;
    compiler::TNode<Number> tmp231;
    compiler::TNode<Number> tmp232;
    compiler::TNode<Number> tmp233;
    compiler::TNode<Number> tmp234;
    ca_.Bind(&block10, &tmp220, &tmp221, &tmp222, &tmp223, &tmp224, &tmp225, &tmp226, &tmp227, &tmp228, &tmp229, &tmp230, &tmp231, &tmp232, &tmp233, &tmp234);
    ca_.SetSourcePosition("../../v8/src/builtins/array-copywithin.tq", 66);
    compiler::TNode<Oddball> tmp235;
    tmp235 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kHasProperty, tmp220, tmp225, tmp230));
    USE(tmp235);
    ca_.SetSourcePosition("../../v8/src/builtins/array-copywithin.tq", 69);
    compiler::TNode<Oddball> tmp236;
    USE(tmp236);
    tmp236 = BaseBuiltinsFromDSLAssembler(state_).True();
    compiler::TNode<BoolT> tmp237;
    USE(tmp237);
    tmp237 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<HeapObject>{tmp235}, compiler::TNode<HeapObject>{tmp236}));
    ca_.Branch(tmp237, &block15, &block16, tmp220, tmp221, tmp222, tmp223, tmp224, tmp225, tmp226, tmp227, tmp228, tmp229, tmp230, tmp231, tmp232, tmp233, tmp234, tmp235);
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp238;
    compiler::TNode<Object> tmp239;
    compiler::TNode<RawPtrT> tmp240;
    compiler::TNode<RawPtrT> tmp241;
    compiler::TNode<IntPtrT> tmp242;
    compiler::TNode<JSReceiver> tmp243;
    compiler::TNode<Number> tmp244;
    compiler::TNode<Number> tmp245;
    compiler::TNode<Number> tmp246;
    compiler::TNode<Number> tmp247;
    compiler::TNode<Number> tmp248;
    compiler::TNode<Number> tmp249;
    compiler::TNode<Number> tmp250;
    compiler::TNode<Number> tmp251;
    compiler::TNode<Number> tmp252;
    compiler::TNode<Oddball> tmp253;
    ca_.Bind(&block15, &tmp238, &tmp239, &tmp240, &tmp241, &tmp242, &tmp243, &tmp244, &tmp245, &tmp246, &tmp247, &tmp248, &tmp249, &tmp250, &tmp251, &tmp252, &tmp253);
    ca_.SetSourcePosition("../../v8/src/builtins/array-copywithin.tq", 71);
    compiler::TNode<Object> tmp254;
    USE(tmp254);
    tmp254 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetProperty(compiler::TNode<Context>{tmp238}, compiler::TNode<Object>{tmp243}, compiler::TNode<Object>{tmp248}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-copywithin.tq", 74);
    CodeStubAssembler(state_).CallBuiltin(Builtins::kSetProperty, tmp238, tmp243, tmp246, tmp254);
    ca_.SetSourcePosition("../../v8/src/builtins/array-copywithin.tq", 69);
    ca_.Goto(&block17, tmp238, tmp239, tmp240, tmp241, tmp242, tmp243, tmp244, tmp245, tmp246, tmp247, tmp248, tmp249, tmp250, tmp251, tmp252, tmp253);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp256;
    compiler::TNode<Object> tmp257;
    compiler::TNode<RawPtrT> tmp258;
    compiler::TNode<RawPtrT> tmp259;
    compiler::TNode<IntPtrT> tmp260;
    compiler::TNode<JSReceiver> tmp261;
    compiler::TNode<Number> tmp262;
    compiler::TNode<Number> tmp263;
    compiler::TNode<Number> tmp264;
    compiler::TNode<Number> tmp265;
    compiler::TNode<Number> tmp266;
    compiler::TNode<Number> tmp267;
    compiler::TNode<Number> tmp268;
    compiler::TNode<Number> tmp269;
    compiler::TNode<Number> tmp270;
    compiler::TNode<Oddball> tmp271;
    ca_.Bind(&block16, &tmp256, &tmp257, &tmp258, &tmp259, &tmp260, &tmp261, &tmp262, &tmp263, &tmp264, &tmp265, &tmp266, &tmp267, &tmp268, &tmp269, &tmp270, &tmp271);
    ca_.SetSourcePosition("../../v8/src/builtins/array-copywithin.tq", 77);
    compiler::TNode<Smi> tmp272;
    USE(tmp272);
    tmp272 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr14ATLanguageMode24ATconstexpr_LanguageMode(LanguageMode::kStrict));
    CodeStubAssembler(state_).CallBuiltin(Builtins::kDeleteProperty, tmp256, tmp261, tmp264, tmp272);
    ca_.SetSourcePosition("../../v8/src/builtins/array-copywithin.tq", 69);
    ca_.Goto(&block17, tmp256, tmp257, tmp258, tmp259, tmp260, tmp261, tmp262, tmp263, tmp264, tmp265, tmp266, tmp267, tmp268, tmp269, tmp270, tmp271);
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp274;
    compiler::TNode<Object> tmp275;
    compiler::TNode<RawPtrT> tmp276;
    compiler::TNode<RawPtrT> tmp277;
    compiler::TNode<IntPtrT> tmp278;
    compiler::TNode<JSReceiver> tmp279;
    compiler::TNode<Number> tmp280;
    compiler::TNode<Number> tmp281;
    compiler::TNode<Number> tmp282;
    compiler::TNode<Number> tmp283;
    compiler::TNode<Number> tmp284;
    compiler::TNode<Number> tmp285;
    compiler::TNode<Number> tmp286;
    compiler::TNode<Number> tmp287;
    compiler::TNode<Number> tmp288;
    compiler::TNode<Oddball> tmp289;
    ca_.Bind(&block17, &tmp274, &tmp275, &tmp276, &tmp277, &tmp278, &tmp279, &tmp280, &tmp281, &tmp282, &tmp283, &tmp284, &tmp285, &tmp286, &tmp287, &tmp288, &tmp289);
    ca_.SetSourcePosition("../../v8/src/builtins/array-copywithin.tq", 81);
    compiler::TNode<Number> tmp290;
    USE(tmp290);
    tmp290 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp284}, compiler::TNode<Number>{tmp288}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-copywithin.tq", 84);
    compiler::TNode<Number> tmp291;
    USE(tmp291);
    tmp291 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberAdd(compiler::TNode<Number>{tmp282}, compiler::TNode<Number>{tmp288}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-copywithin.tq", 87);
    compiler::TNode<Number> tmp292;
    USE(tmp292);
    tmp292 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(1));
    compiler::TNode<Number> tmp293;
    USE(tmp293);
    tmp293 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).NumberSub(compiler::TNode<Number>{tmp287}, compiler::TNode<Number>{tmp292}));
    ca_.SetSourcePosition("../../v8/src/builtins/array-copywithin.tq", 62);
    ca_.Goto(&block12, tmp274, tmp275, tmp276, tmp277, tmp278, tmp279, tmp280, tmp281, tmp291, tmp283, tmp290, tmp285, tmp286, tmp293, tmp288);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp294;
    compiler::TNode<Object> tmp295;
    compiler::TNode<RawPtrT> tmp296;
    compiler::TNode<RawPtrT> tmp297;
    compiler::TNode<IntPtrT> tmp298;
    compiler::TNode<JSReceiver> tmp299;
    compiler::TNode<Number> tmp300;
    compiler::TNode<Number> tmp301;
    compiler::TNode<Number> tmp302;
    compiler::TNode<Number> tmp303;
    compiler::TNode<Number> tmp304;
    compiler::TNode<Number> tmp305;
    compiler::TNode<Number> tmp306;
    compiler::TNode<Number> tmp307;
    compiler::TNode<Number> tmp308;
    ca_.Bind(&block11, &tmp294, &tmp295, &tmp296, &tmp297, &tmp298, &tmp299, &tmp300, &tmp301, &tmp302, &tmp303, &tmp304, &tmp305, &tmp306, &tmp307, &tmp308);
    ca_.SetSourcePosition("../../v8/src/builtins/array-copywithin.tq", 91);
    arguments.PopAndReturn(tmp299);
  }
}

}  // namespace internal
}  // namespace v8

