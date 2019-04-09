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

const char* StringRepeatBuiltinsFromDSLAssembler::kBuiltinName() {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

    ca_.Bind(&block0);
return "String.prototype.repeat";
}

TF_BUILTIN(StringRepeat, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<String> parameter1 = UncheckedCast<String>(Parameter(Descriptor::kString));
  USE(parameter1);
  compiler::TNode<Smi> parameter2 = UncheckedCast<Smi>(Parameter(Descriptor::kCount));
  USE(parameter2);
  compiler::CodeAssemblerParameterizedLabel<Context, String, Smi> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, String, Smi> block2(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, String, Smi> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, String, Smi> block4(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, String, Smi> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, String, Smi, String, String, IntPtrT> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, String, Smi, String, String, IntPtrT> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, String, Smi, String, String, IntPtrT> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, String, Smi, String, String, IntPtrT> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, String, Smi, String, String, IntPtrT> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, String, Smi, String, String, IntPtrT> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, String, Smi, String, String, IntPtrT> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<String> tmp1;
    compiler::TNode<Smi> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../src/builtins/string-repeat.tq", 10);
    compiler::TNode<Smi> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiGreaterThanOrEqual(compiler::TNode<Smi>{tmp2}, compiler::TNode<Smi>{tmp3}));
    ca_.Branch(tmp4, &block1, &block2, tmp0, tmp1, tmp2);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp5;
    compiler::TNode<String> tmp6;
    compiler::TNode<Smi> tmp7;
    ca_.Bind(&block2, &tmp5, &tmp6, &tmp7);
    CodeStubAssembler(state_).FailAssert("Torque assert \'count >= 0\' failed", "../../src/builtins/string-repeat.tq", 10);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp8;
    compiler::TNode<String> tmp9;
    compiler::TNode<Smi> tmp10;
    ca_.Bind(&block1, &tmp8, &tmp9, &tmp10);
    ca_.SetSourcePosition("../../src/builtins/string-repeat.tq", 11);
    compiler::TNode<String> tmp11;
    USE(tmp11);
    tmp11 = BaseBuiltinsFromDSLAssembler(state_).kEmptyString();
    compiler::TNode<BoolT> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordNotEqual(compiler::TNode<Object>{tmp9}, compiler::TNode<HeapObject>{tmp11}));
    ca_.Branch(tmp12, &block3, &block4, tmp8, tmp9, tmp10);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp13;
    compiler::TNode<String> tmp14;
    compiler::TNode<Smi> tmp15;
    ca_.Bind(&block4, &tmp13, &tmp14, &tmp15);
    CodeStubAssembler(state_).FailAssert("Torque assert \'string != kEmptyString\' failed", "../../src/builtins/string-repeat.tq", 11);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp16;
    compiler::TNode<String> tmp17;
    compiler::TNode<Smi> tmp18;
    ca_.Bind(&block3, &tmp16, &tmp17, &tmp18);
    ca_.SetSourcePosition("../../src/builtins/string-repeat.tq", 13);
    compiler::TNode<String> tmp19;
    USE(tmp19);
    tmp19 = BaseBuiltinsFromDSLAssembler(state_).kEmptyString();
    ca_.SetSourcePosition("../../src/builtins/string-repeat.tq", 14);
    ca_.SetSourcePosition("../../src/builtins/string-repeat.tq", 15);
    compiler::TNode<IntPtrT> tmp20;
    USE(tmp20);
    tmp20 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr5ATSmi(compiler::TNode<Smi>{tmp18}));
    ca_.SetSourcePosition("../../src/builtins/string-repeat.tq", 17);
    ca_.Goto(&block7, tmp16, tmp17, tmp18, tmp19, tmp17, tmp20);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp21;
    compiler::TNode<String> tmp22;
    compiler::TNode<Smi> tmp23;
    compiler::TNode<String> tmp24;
    compiler::TNode<String> tmp25;
    compiler::TNode<IntPtrT> tmp26;
    ca_.Bind(&block7, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26);
    compiler::TNode<BoolT> tmp27;
    USE(tmp27);
    tmp27 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    ca_.Branch(tmp27, &block5, &block6, tmp21, tmp22, tmp23, tmp24, tmp25, tmp26);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp28;
    compiler::TNode<String> tmp29;
    compiler::TNode<Smi> tmp30;
    compiler::TNode<String> tmp31;
    compiler::TNode<String> tmp32;
    compiler::TNode<IntPtrT> tmp33;
    ca_.Bind(&block5, &tmp28, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33);
    ca_.SetSourcePosition("../../src/builtins/string-repeat.tq", 18);
    compiler::TNode<IntPtrT> tmp34;
    USE(tmp34);
    tmp34 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<IntPtrT> tmp35;
    USE(tmp35);
    tmp35 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).WordAnd(compiler::TNode<IntPtrT>{tmp33}, compiler::TNode<IntPtrT>{tmp34}));
    compiler::TNode<IntPtrT> tmp36;
    USE(tmp36);
    tmp36 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<BoolT> tmp37;
    USE(tmp37);
    tmp37 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<IntPtrT>{tmp35}, compiler::TNode<IntPtrT>{tmp36}));
    ca_.Branch(tmp37, &block8, &block9, tmp28, tmp29, tmp30, tmp31, tmp32, tmp33);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp38;
    compiler::TNode<String> tmp39;
    compiler::TNode<Smi> tmp40;
    compiler::TNode<String> tmp41;
    compiler::TNode<String> tmp42;
    compiler::TNode<IntPtrT> tmp43;
    ca_.Bind(&block8, &tmp38, &tmp39, &tmp40, &tmp41, &tmp42, &tmp43);
    compiler::TNode<String> tmp44;
    USE(tmp44);
    tmp44 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).StringAdd(compiler::TNode<Context>{tmp38}, compiler::TNode<String>{tmp41}, compiler::TNode<String>{tmp42}));
    ca_.Goto(&block9, tmp38, tmp39, tmp40, tmp44, tmp42, tmp43);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp45;
    compiler::TNode<String> tmp46;
    compiler::TNode<Smi> tmp47;
    compiler::TNode<String> tmp48;
    compiler::TNode<String> tmp49;
    compiler::TNode<IntPtrT> tmp50;
    ca_.Bind(&block9, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50);
    ca_.SetSourcePosition("../../src/builtins/string-repeat.tq", 20);
    compiler::TNode<IntPtrT> tmp51;
    USE(tmp51);
    tmp51 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<IntPtrT> tmp52;
    USE(tmp52);
    tmp52 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).WordSar(compiler::TNode<IntPtrT>{tmp50}, compiler::TNode<IntPtrT>{tmp51}));
    ca_.SetSourcePosition("../../src/builtins/string-repeat.tq", 21);
    compiler::TNode<IntPtrT> tmp53;
    USE(tmp53);
    tmp53 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp54;
    USE(tmp54);
    tmp54 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordEqual(compiler::TNode<IntPtrT>{tmp52}, compiler::TNode<IntPtrT>{tmp53}));
    ca_.Branch(tmp54, &block10, &block11, tmp45, tmp46, tmp47, tmp48, tmp49, tmp52);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp55;
    compiler::TNode<String> tmp56;
    compiler::TNode<Smi> tmp57;
    compiler::TNode<String> tmp58;
    compiler::TNode<String> tmp59;
    compiler::TNode<IntPtrT> tmp60;
    ca_.Bind(&block10, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60);
    ca_.Goto(&block6, tmp55, tmp56, tmp57, tmp58, tmp59, tmp60);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp61;
    compiler::TNode<String> tmp62;
    compiler::TNode<Smi> tmp63;
    compiler::TNode<String> tmp64;
    compiler::TNode<String> tmp65;
    compiler::TNode<IntPtrT> tmp66;
    ca_.Bind(&block11, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65, &tmp66);
    ca_.SetSourcePosition("../../src/builtins/string-repeat.tq", 23);
    compiler::TNode<String> tmp67;
    USE(tmp67);
    tmp67 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).StringAdd(compiler::TNode<Context>{tmp61}, compiler::TNode<String>{tmp65}, compiler::TNode<String>{tmp65}));
    ca_.SetSourcePosition("../../src/builtins/string-repeat.tq", 17);
    ca_.Goto(&block7, tmp61, tmp62, tmp63, tmp64, tmp67, tmp66);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp68;
    compiler::TNode<String> tmp69;
    compiler::TNode<Smi> tmp70;
    compiler::TNode<String> tmp71;
    compiler::TNode<String> tmp72;
    compiler::TNode<IntPtrT> tmp73;
    ca_.Bind(&block6, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72, &tmp73);
    ca_.SetSourcePosition("../../src/builtins/string-repeat.tq", 26);
    CodeStubAssembler(state_).Return(tmp71);
  }
}

TF_BUILTIN(StringPrototypeRepeat, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<Object> parameter1 = UncheckedCast<Object>(Parameter(Descriptor::kReceiver));
  USE(parameter1);
  compiler::TNode<Object> parameter2 = UncheckedCast<Object>(Parameter(Descriptor::kCount));
  USE(parameter2);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, String, Number, Number> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, String, Number, Number, Smi> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, String, Number, Smi> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, String, Number, Smi> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, String, Number, Smi> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, String, Number, Smi> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, String, Number, Smi> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, String, Number, Smi> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, String, Number, Smi> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, String, Number> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, String, Number, HeapNumber> block19(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, String, Number, HeapNumber> block18(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, String, Number, HeapNumber, Float64T> block22(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, String, Number, HeapNumber, Float64T> block20(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, String, Number, HeapNumber, Float64T> block21(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, String, Number, HeapNumber, Float64T> block23(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, String, Number, HeapNumber, Float64T> block24(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, String> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, String> block4(&ca_, compiler::CodeAssemblerLabel::kDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, String> block2(&ca_, compiler::CodeAssemblerLabel::kDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<Object> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../src/builtins/string-repeat.tq", 34);
    compiler::TNode<String> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string(StringRepeatBuiltinsFromDSLAssembler(state_).kBuiltinName()));
    compiler::TNode<String> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<String>(CodeStubAssembler(state_).ToThisString(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}, compiler::TNode<String>{tmp3}));
    ca_.SetSourcePosition("../../src/builtins/string-repeat.tq", 38);
    compiler::TNode<Number> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).ToInteger_Inline(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp2}, CodeStubAssembler::ToIntegerTruncationMode::kTruncateMinusZero));
    ca_.SetSourcePosition("../../src/builtins/string-repeat.tq", 39);
    compiler::TNode<Smi> tmp6;
    USE(tmp6);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp6 = BaseBuiltinsFromDSLAssembler(state_).Cast5ATSmi(compiler::TNode<Object>{tmp5}, &label0);
    ca_.Goto(&block9, tmp0, tmp1, tmp2, tmp4, tmp5, tmp5, tmp6);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block10, tmp0, tmp1, tmp2, tmp4, tmp5, tmp5);
    }
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<Object> tmp8;
    compiler::TNode<Object> tmp9;
    compiler::TNode<String> tmp10;
    compiler::TNode<Number> tmp11;
    compiler::TNode<Number> tmp12;
    ca_.Bind(&block10, &tmp7, &tmp8, &tmp9, &tmp10, &tmp11, &tmp12);
    ca_.Goto(&block8, tmp7, tmp8, tmp9, tmp10, tmp11);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp13;
    compiler::TNode<Object> tmp14;
    compiler::TNode<Object> tmp15;
    compiler::TNode<String> tmp16;
    compiler::TNode<Number> tmp17;
    compiler::TNode<Number> tmp18;
    compiler::TNode<Smi> tmp19;
    ca_.Bind(&block9, &tmp13, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18, &tmp19);
    ca_.SetSourcePosition("../../src/builtins/string-repeat.tq", 41);
    compiler::TNode<Smi> tmp20;
    USE(tmp20);
    tmp20 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp21;
    USE(tmp21);
    tmp21 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiLessThan(compiler::TNode<Smi>{tmp19}, compiler::TNode<Smi>{tmp20}));
    ca_.Branch(tmp21, &block11, &block12, tmp13, tmp14, tmp15, tmp16, tmp17, tmp19);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp22;
    compiler::TNode<Object> tmp23;
    compiler::TNode<Object> tmp24;
    compiler::TNode<String> tmp25;
    compiler::TNode<Number> tmp26;
    compiler::TNode<Smi> tmp27;
    ca_.Bind(&block11, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27);
    ca_.Goto(&block4, tmp22, tmp23, tmp24, tmp25);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp28;
    compiler::TNode<Object> tmp29;
    compiler::TNode<Object> tmp30;
    compiler::TNode<String> tmp31;
    compiler::TNode<Number> tmp32;
    compiler::TNode<Smi> tmp33;
    ca_.Bind(&block12, &tmp28, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33);
    ca_.SetSourcePosition("../../src/builtins/string-repeat.tq", 44);
    compiler::TNode<Smi> tmp34;
    USE(tmp34);
    tmp34 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp35;
    USE(tmp35);
    tmp35 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiEqual(compiler::TNode<Smi>{tmp33}, compiler::TNode<Smi>{tmp34}));
    ca_.Branch(tmp35, &block13, &block15, tmp28, tmp29, tmp30, tmp31, tmp32, tmp33);
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp36;
    compiler::TNode<Object> tmp37;
    compiler::TNode<Object> tmp38;
    compiler::TNode<String> tmp39;
    compiler::TNode<Number> tmp40;
    compiler::TNode<Smi> tmp41;
    ca_.Bind(&block15, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41);
    compiler::TNode<Uint32T> tmp42;
    USE(tmp42);
    tmp42 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).LoadStringLengthAsWord32(compiler::TNode<String>{tmp39}));
    compiler::TNode<Uint32T> tmp43;
    USE(tmp43);
    tmp43 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp44;
    USE(tmp44);
    tmp44 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32Equal(compiler::TNode<Uint32T>{tmp42}, compiler::TNode<Uint32T>{tmp43}));
    ca_.Branch(tmp44, &block13, &block14, tmp36, tmp37, tmp38, tmp39, tmp40, tmp41);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp45;
    compiler::TNode<Object> tmp46;
    compiler::TNode<Object> tmp47;
    compiler::TNode<String> tmp48;
    compiler::TNode<Number> tmp49;
    compiler::TNode<Smi> tmp50;
    ca_.Bind(&block13, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50);
    ca_.Goto(&block6, tmp45, tmp46, tmp47, tmp48);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp51;
    compiler::TNode<Object> tmp52;
    compiler::TNode<Object> tmp53;
    compiler::TNode<String> tmp54;
    compiler::TNode<Number> tmp55;
    compiler::TNode<Smi> tmp56;
    ca_.Bind(&block14, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55, &tmp56);
    ca_.SetSourcePosition("../../src/builtins/string-repeat.tq", 46);
    compiler::TNode<Smi> tmp57;
    USE(tmp57);
    tmp57 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr5ATSmi17ATconstexpr_int31(String::kMaxLength));
    compiler::TNode<BoolT> tmp58;
    USE(tmp58);
    tmp58 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).SmiGreaterThan(compiler::TNode<Smi>{tmp56}, compiler::TNode<Smi>{tmp57}));
    ca_.Branch(tmp58, &block16, &block17, tmp51, tmp52, tmp53, tmp54, tmp55, tmp56);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp59;
    compiler::TNode<Object> tmp60;
    compiler::TNode<Object> tmp61;
    compiler::TNode<String> tmp62;
    compiler::TNode<Number> tmp63;
    compiler::TNode<Smi> tmp64;
    ca_.Bind(&block16, &tmp59, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64);
    ca_.Goto(&block2, tmp59, tmp60, tmp61, tmp62);
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp65;
    compiler::TNode<Object> tmp66;
    compiler::TNode<Object> tmp67;
    compiler::TNode<String> tmp68;
    compiler::TNode<Number> tmp69;
    compiler::TNode<Smi> tmp70;
    ca_.Bind(&block17, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70);
    ca_.SetSourcePosition("../../src/builtins/string-repeat.tq", 50);
    compiler::TNode<String> tmp71;
    tmp71 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kStringRepeat, tmp65, tmp68, tmp70));
    USE(tmp71);
    CodeStubAssembler(state_).Return(tmp71);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp72;
    compiler::TNode<Object> tmp73;
    compiler::TNode<Object> tmp74;
    compiler::TNode<String> tmp75;
    compiler::TNode<Number> tmp76;
    ca_.Bind(&block8, &tmp72, &tmp73, &tmp74, &tmp75, &tmp76);
    ca_.SetSourcePosition("../../src/builtins/string-repeat.tq", 52);
    ca_.SetSourcePosition("../../src/builtins/string-repeat.tq", 53);
    compiler::TNode<BoolT> tmp77;
    USE(tmp77);
    tmp77 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsNumberNormalized(compiler::TNode<Number>{ca_.UncheckedCast<HeapNumber>(tmp76)}));
    ca_.Branch(tmp77, &block18, &block19, tmp72, tmp73, tmp74, tmp75, tmp76, ca_.UncheckedCast<HeapNumber>(tmp76));
  }

  if (block19.is_used()) {
    compiler::TNode<Context> tmp78;
    compiler::TNode<Object> tmp79;
    compiler::TNode<Object> tmp80;
    compiler::TNode<String> tmp81;
    compiler::TNode<Number> tmp82;
    compiler::TNode<HeapNumber> tmp83;
    ca_.Bind(&block19, &tmp78, &tmp79, &tmp80, &tmp81, &tmp82, &tmp83);
    CodeStubAssembler(state_).FailAssert("Torque assert \'IsNumberNormalized(heapNum)\' failed", "../../src/builtins/string-repeat.tq", 53);
  }

  if (block18.is_used()) {
    compiler::TNode<Context> tmp84;
    compiler::TNode<Object> tmp85;
    compiler::TNode<Object> tmp86;
    compiler::TNode<String> tmp87;
    compiler::TNode<Number> tmp88;
    compiler::TNode<HeapNumber> tmp89;
    ca_.Bind(&block18, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89);
    ca_.SetSourcePosition("../../src/builtins/string-repeat.tq", 54);
    compiler::TNode<Float64T> tmp90;
    USE(tmp90);
    tmp90 = ca_.UncheckedCast<Float64T>(BaseBuiltinsFromDSLAssembler(state_).LoadHeapNumberValue(compiler::TNode<HeapNumber>{tmp89}));
    ca_.SetSourcePosition("../../src/builtins/string-repeat.tq", 58);
    compiler::TNode<Float64T> tmp91;
    USE(tmp91);
    tmp91 = ca_.UncheckedCast<Float64T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATfloat6419ATconstexpr_float64(V8_INFINITY));
    compiler::TNode<BoolT> tmp92;
    USE(tmp92);
    tmp92 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Float64Equal(compiler::TNode<Float64T>{tmp90}, compiler::TNode<Float64T>{tmp91}));
    ca_.Branch(tmp92, &block20, &block22, tmp84, tmp85, tmp86, tmp87, tmp88, tmp89, tmp90);
  }

  if (block22.is_used()) {
    compiler::TNode<Context> tmp93;
    compiler::TNode<Object> tmp94;
    compiler::TNode<Object> tmp95;
    compiler::TNode<String> tmp96;
    compiler::TNode<Number> tmp97;
    compiler::TNode<HeapNumber> tmp98;
    compiler::TNode<Float64T> tmp99;
    ca_.Bind(&block22, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97, &tmp98, &tmp99);
    compiler::TNode<Float64T> tmp100;
    USE(tmp100);
    tmp100 = ca_.UncheckedCast<Float64T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATfloat6417ATconstexpr_int31(0.0));
    compiler::TNode<BoolT> tmp101;
    USE(tmp101);
    tmp101 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Float64LessThan(compiler::TNode<Float64T>{tmp99}, compiler::TNode<Float64T>{tmp100}));
    ca_.Branch(tmp101, &block20, &block21, tmp93, tmp94, tmp95, tmp96, tmp97, tmp98, tmp99);
  }

  if (block20.is_used()) {
    compiler::TNode<Context> tmp102;
    compiler::TNode<Object> tmp103;
    compiler::TNode<Object> tmp104;
    compiler::TNode<String> tmp105;
    compiler::TNode<Number> tmp106;
    compiler::TNode<HeapNumber> tmp107;
    compiler::TNode<Float64T> tmp108;
    ca_.Bind(&block20, &tmp102, &tmp103, &tmp104, &tmp105, &tmp106, &tmp107, &tmp108);
    ca_.Goto(&block4, tmp102, tmp103, tmp104, tmp105);
  }

  if (block21.is_used()) {
    compiler::TNode<Context> tmp109;
    compiler::TNode<Object> tmp110;
    compiler::TNode<Object> tmp111;
    compiler::TNode<String> tmp112;
    compiler::TNode<Number> tmp113;
    compiler::TNode<HeapNumber> tmp114;
    compiler::TNode<Float64T> tmp115;
    ca_.Bind(&block21, &tmp109, &tmp110, &tmp111, &tmp112, &tmp113, &tmp114, &tmp115);
    ca_.SetSourcePosition("../../src/builtins/string-repeat.tq", 61);
    compiler::TNode<Uint32T> tmp116;
    USE(tmp116);
    tmp116 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).LoadStringLengthAsWord32(compiler::TNode<String>{tmp112}));
    compiler::TNode<Uint32T> tmp117;
    USE(tmp117);
    tmp117 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp118;
    USE(tmp118);
    tmp118 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32Equal(compiler::TNode<Uint32T>{tmp116}, compiler::TNode<Uint32T>{tmp117}));
    ca_.Branch(tmp118, &block23, &block24, tmp109, tmp110, tmp111, tmp112, tmp113, tmp114, tmp115);
  }

  if (block23.is_used()) {
    compiler::TNode<Context> tmp119;
    compiler::TNode<Object> tmp120;
    compiler::TNode<Object> tmp121;
    compiler::TNode<String> tmp122;
    compiler::TNode<Number> tmp123;
    compiler::TNode<HeapNumber> tmp124;
    compiler::TNode<Float64T> tmp125;
    ca_.Bind(&block23, &tmp119, &tmp120, &tmp121, &tmp122, &tmp123, &tmp124, &tmp125);
    ca_.Goto(&block6, tmp119, tmp120, tmp121, tmp122);
  }

  if (block24.is_used()) {
    compiler::TNode<Context> tmp126;
    compiler::TNode<Object> tmp127;
    compiler::TNode<Object> tmp128;
    compiler::TNode<String> tmp129;
    compiler::TNode<Number> tmp130;
    compiler::TNode<HeapNumber> tmp131;
    compiler::TNode<Float64T> tmp132;
    ca_.Bind(&block24, &tmp126, &tmp127, &tmp128, &tmp129, &tmp130, &tmp131, &tmp132);
    ca_.SetSourcePosition("../../src/builtins/string-repeat.tq", 63);
    ca_.Goto(&block2, tmp126, tmp127, tmp128, tmp129);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp133;
    compiler::TNode<Object> tmp134;
    compiler::TNode<Object> tmp135;
    compiler::TNode<String> tmp136;
    ca_.Bind(&block6, &tmp133, &tmp134, &tmp135, &tmp136);
    ca_.SetSourcePosition("../../src/builtins/string-repeat.tq", 68);
    compiler::TNode<String> tmp137;
    USE(tmp137);
    tmp137 = BaseBuiltinsFromDSLAssembler(state_).kEmptyString();
    CodeStubAssembler(state_).Return(tmp137);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp138;
    compiler::TNode<Object> tmp139;
    compiler::TNode<Object> tmp140;
    compiler::TNode<String> tmp141;
    ca_.Bind(&block4, &tmp138, &tmp139, &tmp140, &tmp141);
    ca_.SetSourcePosition("../../src/builtins/string-repeat.tq", 71);
    CodeStubAssembler(state_).ThrowRangeError(compiler::TNode<Context>{tmp138}, MessageTemplate::kInvalidCountValue, compiler::TNode<Object>{tmp140});
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp142;
    compiler::TNode<Object> tmp143;
    compiler::TNode<Object> tmp144;
    compiler::TNode<String> tmp145;
    ca_.Bind(&block2, &tmp142, &tmp143, &tmp144, &tmp145);
    ca_.SetSourcePosition("../../src/builtins/string-repeat.tq", 74);
    CodeStubAssembler(state_).CallRuntime(Runtime::kThrowInvalidStringLength, tmp142);
    CodeStubAssembler(state_).Unreachable();
  }
}

}  // namespace internal
}  // namespace v8

