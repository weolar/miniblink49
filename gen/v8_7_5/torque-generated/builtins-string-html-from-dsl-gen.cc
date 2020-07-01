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

TF_BUILTIN(CreateHTML, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  compiler::TNode<Object> parameter1 = UncheckedCast<Object>(Parameter(Descriptor::kReceiver));
  USE(parameter1);
  compiler::TNode<String> parameter2 = UncheckedCast<String>(Parameter(Descriptor::kMethodName));
  USE(parameter2);
  compiler::TNode<String> parameter3 = UncheckedCast<String>(Parameter(Descriptor::kTagName));
  USE(parameter3);
  compiler::TNode<String> parameter4 = UncheckedCast<String>(Parameter(Descriptor::kAttr));
  USE(parameter4);
  compiler::TNode<Object> parameter5 = UncheckedCast<Object>(Parameter(Descriptor::kAttrValue));
  USE(parameter5);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, String, String, String, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, String, String, String, Object, String, String> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, String, String, String, Object, String, String> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, parameter2, parameter3, parameter4, parameter5);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<String> tmp2;
    compiler::TNode<String> tmp3;
    compiler::TNode<String> tmp4;
    compiler::TNode<Object> tmp5;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4, &tmp5);
    ca_.SetSourcePosition("../../src/builtins/string-html.tq", 12);
    compiler::TNode<String> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<String>(CodeStubAssembler(state_).ToThisString(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}, compiler::TNode<String>{tmp2}));
    ca_.SetSourcePosition("../../src/builtins/string-html.tq", 13);
    compiler::TNode<String> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("<"));
    compiler::TNode<String> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).StringAdd(compiler::TNode<Context>{tmp0}, compiler::TNode<String>{tmp7}, compiler::TNode<String>{tmp3}));
    ca_.SetSourcePosition("../../src/builtins/string-html.tq", 14);
    compiler::TNode<String> tmp9;
    USE(tmp9);
    tmp9 = BaseBuiltinsFromDSLAssembler(state_).kEmptyString();
    compiler::TNode<BoolT> tmp10;
    USE(tmp10);
    tmp10 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).WordNotEqual(compiler::TNode<Object>{tmp4}, compiler::TNode<HeapObject>{tmp9}));
    ca_.Branch(tmp10, &block1, &block2, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp8);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp11;
    compiler::TNode<Object> tmp12;
    compiler::TNode<String> tmp13;
    compiler::TNode<String> tmp14;
    compiler::TNode<String> tmp15;
    compiler::TNode<Object> tmp16;
    compiler::TNode<String> tmp17;
    compiler::TNode<String> tmp18;
    ca_.Bind(&block1, &tmp11, &tmp12, &tmp13, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18);
    ca_.SetSourcePosition("../../src/builtins/string-html.tq", 16);
    compiler::TNode<String> tmp19;
    USE(tmp19);
    tmp19 = ca_.UncheckedCast<String>(CodeStubAssembler(state_).ToString_Inline(compiler::TNode<Context>{tmp11}, compiler::TNode<Object>{tmp16}));
    compiler::TNode<String> tmp20;
    tmp20 = TORQUE_CAST(CodeStubAssembler(state_).CallRuntime(Runtime::kStringEscapeQuotes, tmp11, tmp19));
    USE(tmp20);
    ca_.SetSourcePosition("../../src/builtins/string-html.tq", 15);
    ca_.SetSourcePosition("../../src/builtins/string-html.tq", 17);
    compiler::TNode<String> tmp21;
    USE(tmp21);
    tmp21 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string(" "));
    compiler::TNode<String> tmp22;
    USE(tmp22);
    tmp22 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).StringAdd(compiler::TNode<Context>{tmp11}, compiler::TNode<String>{tmp18}, compiler::TNode<String>{tmp21}));
    compiler::TNode<String> tmp23;
    USE(tmp23);
    tmp23 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).StringAdd(compiler::TNode<Context>{tmp11}, compiler::TNode<String>{tmp22}, compiler::TNode<String>{tmp15}));
    compiler::TNode<String> tmp24;
    USE(tmp24);
    tmp24 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("=\""));
    compiler::TNode<String> tmp25;
    USE(tmp25);
    tmp25 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).StringAdd(compiler::TNode<Context>{tmp11}, compiler::TNode<String>{tmp23}, compiler::TNode<String>{tmp24}));
    compiler::TNode<String> tmp26;
    USE(tmp26);
    tmp26 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).StringAdd(compiler::TNode<Context>{tmp11}, compiler::TNode<String>{tmp25}, compiler::TNode<String>{tmp20}));
    compiler::TNode<String> tmp27;
    USE(tmp27);
    tmp27 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("\""));
    compiler::TNode<String> tmp28;
    USE(tmp28);
    tmp28 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).StringAdd(compiler::TNode<Context>{tmp11}, compiler::TNode<String>{tmp26}, compiler::TNode<String>{tmp27}));
    ca_.SetSourcePosition("../../src/builtins/string-html.tq", 14);
    ca_.Goto(&block2, tmp11, tmp12, tmp13, tmp14, tmp15, tmp16, tmp17, tmp28);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp29;
    compiler::TNode<Object> tmp30;
    compiler::TNode<String> tmp31;
    compiler::TNode<String> tmp32;
    compiler::TNode<String> tmp33;
    compiler::TNode<Object> tmp34;
    compiler::TNode<String> tmp35;
    compiler::TNode<String> tmp36;
    ca_.Bind(&block2, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36);
    ca_.SetSourcePosition("../../src/builtins/string-html.tq", 20);
    compiler::TNode<String> tmp37;
    USE(tmp37);
    tmp37 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string(">"));
    compiler::TNode<String> tmp38;
    USE(tmp38);
    tmp38 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).StringAdd(compiler::TNode<Context>{tmp29}, compiler::TNode<String>{tmp36}, compiler::TNode<String>{tmp37}));
    compiler::TNode<String> tmp39;
    USE(tmp39);
    tmp39 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).StringAdd(compiler::TNode<Context>{tmp29}, compiler::TNode<String>{tmp38}, compiler::TNode<String>{tmp35}));
    compiler::TNode<String> tmp40;
    USE(tmp40);
    tmp40 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("</"));
    compiler::TNode<String> tmp41;
    USE(tmp41);
    tmp41 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).StringAdd(compiler::TNode<Context>{tmp29}, compiler::TNode<String>{tmp39}, compiler::TNode<String>{tmp40}));
    compiler::TNode<String> tmp42;
    USE(tmp42);
    tmp42 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).StringAdd(compiler::TNode<Context>{tmp29}, compiler::TNode<String>{tmp41}, compiler::TNode<String>{tmp32}));
    compiler::TNode<String> tmp43;
    USE(tmp43);
    tmp43 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string(">"));
    compiler::TNode<String> tmp44;
    USE(tmp44);
    tmp44 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).StringAdd(compiler::TNode<Context>{tmp29}, compiler::TNode<String>{tmp42}, compiler::TNode<String>{tmp43}));
    CodeStubAssembler(state_).Return(tmp44);
  }
}

TF_BUILTIN(StringPrototypeAnchor, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  Node* argc = Parameter(Descriptor::kJSActualArgumentsCount);
  CodeStubArguments arguments_impl(this, ChangeInt32ToIntPtr(argc));
  TNode<Object> parameter1 = arguments_impl.GetReceiver();
auto arguments = &arguments_impl;
USE(arguments);
USE(parameter1);
  compiler::CodeAssemblerParameterizedLabel<Context, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../src/builtins/string-html.tq", 27);
    compiler::TNode<IntPtrT> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(arguments, compiler::TNode<IntPtrT>{tmp2}));
    ca_.SetSourcePosition("../../src/builtins/string-html.tq", 26);
    compiler::TNode<String> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("String.prototype.anchor"));
    compiler::TNode<String> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("a"));
    compiler::TNode<String> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("name"));
    compiler::TNode<String> tmp7;
    tmp7 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kCreateHTML, tmp0, tmp1, tmp4, tmp5, tmp6, tmp3));
    USE(tmp7);
    arguments->PopAndReturn(tmp7);
  }
}

TF_BUILTIN(StringPrototypeBig, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  Node* argc = Parameter(Descriptor::kJSActualArgumentsCount);
  CodeStubArguments arguments_impl(this, ChangeInt32ToIntPtr(argc));
  TNode<Object> parameter1 = arguments_impl.GetReceiver();
auto arguments = &arguments_impl;
USE(arguments);
USE(parameter1);
  compiler::CodeAssemblerParameterizedLabel<Context, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../src/builtins/string-html.tq", 34);
    compiler::TNode<String> tmp2;
    USE(tmp2);
    tmp2 = BaseBuiltinsFromDSLAssembler(state_).kEmptyString();
    compiler::TNode<String> tmp3;
    USE(tmp3);
    tmp3 = BaseBuiltinsFromDSLAssembler(state_).kEmptyString();
    ca_.SetSourcePosition("../../src/builtins/string-html.tq", 33);
    compiler::TNode<String> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("String.prototype.big"));
    compiler::TNode<String> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("big"));
    compiler::TNode<String> tmp6;
    tmp6 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kCreateHTML, tmp0, tmp1, tmp4, tmp5, tmp2, tmp3));
    USE(tmp6);
    arguments->PopAndReturn(tmp6);
  }
}

TF_BUILTIN(StringPrototypeBlink, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  Node* argc = Parameter(Descriptor::kJSActualArgumentsCount);
  CodeStubArguments arguments_impl(this, ChangeInt32ToIntPtr(argc));
  TNode<Object> parameter1 = arguments_impl.GetReceiver();
auto arguments = &arguments_impl;
USE(arguments);
USE(parameter1);
  compiler::CodeAssemblerParameterizedLabel<Context, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../src/builtins/string-html.tq", 42);
    compiler::TNode<String> tmp2;
    USE(tmp2);
    tmp2 = BaseBuiltinsFromDSLAssembler(state_).kEmptyString();
    ca_.SetSourcePosition("../../src/builtins/string-html.tq", 43);
    compiler::TNode<String> tmp3;
    USE(tmp3);
    tmp3 = BaseBuiltinsFromDSLAssembler(state_).kEmptyString();
    ca_.SetSourcePosition("../../src/builtins/string-html.tq", 41);
    compiler::TNode<String> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("String.prototype.blink"));
    compiler::TNode<String> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("blink"));
    compiler::TNode<String> tmp6;
    tmp6 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kCreateHTML, tmp0, tmp1, tmp4, tmp5, tmp2, tmp3));
    USE(tmp6);
    arguments->PopAndReturn(tmp6);
  }
}

TF_BUILTIN(StringPrototypeBold, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  Node* argc = Parameter(Descriptor::kJSActualArgumentsCount);
  CodeStubArguments arguments_impl(this, ChangeInt32ToIntPtr(argc));
  TNode<Object> parameter1 = arguments_impl.GetReceiver();
auto arguments = &arguments_impl;
USE(arguments);
USE(parameter1);
  compiler::CodeAssemblerParameterizedLabel<Context, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../src/builtins/string-html.tq", 51);
    compiler::TNode<String> tmp2;
    USE(tmp2);
    tmp2 = BaseBuiltinsFromDSLAssembler(state_).kEmptyString();
    compiler::TNode<String> tmp3;
    USE(tmp3);
    tmp3 = BaseBuiltinsFromDSLAssembler(state_).kEmptyString();
    ca_.SetSourcePosition("../../src/builtins/string-html.tq", 50);
    compiler::TNode<String> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("String.prototype.bold"));
    compiler::TNode<String> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("b"));
    compiler::TNode<String> tmp6;
    tmp6 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kCreateHTML, tmp0, tmp1, tmp4, tmp5, tmp2, tmp3));
    USE(tmp6);
    arguments->PopAndReturn(tmp6);
  }
}

TF_BUILTIN(StringPrototypeFontcolor, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  Node* argc = Parameter(Descriptor::kJSActualArgumentsCount);
  CodeStubArguments arguments_impl(this, ChangeInt32ToIntPtr(argc));
  TNode<Object> parameter1 = arguments_impl.GetReceiver();
auto arguments = &arguments_impl;
USE(arguments);
USE(parameter1);
  compiler::CodeAssemblerParameterizedLabel<Context, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../src/builtins/string-html.tq", 59);
    compiler::TNode<IntPtrT> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(arguments, compiler::TNode<IntPtrT>{tmp2}));
    ca_.SetSourcePosition("../../src/builtins/string-html.tq", 58);
    compiler::TNode<String> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("String.prototype.fontcolor"));
    compiler::TNode<String> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("font"));
    compiler::TNode<String> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("color"));
    compiler::TNode<String> tmp7;
    tmp7 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kCreateHTML, tmp0, tmp1, tmp4, tmp5, tmp6, tmp3));
    USE(tmp7);
    arguments->PopAndReturn(tmp7);
  }
}

TF_BUILTIN(StringPrototypeFontsize, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  Node* argc = Parameter(Descriptor::kJSActualArgumentsCount);
  CodeStubArguments arguments_impl(this, ChangeInt32ToIntPtr(argc));
  TNode<Object> parameter1 = arguments_impl.GetReceiver();
auto arguments = &arguments_impl;
USE(arguments);
USE(parameter1);
  compiler::CodeAssemblerParameterizedLabel<Context, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../src/builtins/string-html.tq", 67);
    compiler::TNode<IntPtrT> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(arguments, compiler::TNode<IntPtrT>{tmp2}));
    ca_.SetSourcePosition("../../src/builtins/string-html.tq", 66);
    compiler::TNode<String> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("String.prototype.fontsize"));
    compiler::TNode<String> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("font"));
    compiler::TNode<String> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("size"));
    compiler::TNode<String> tmp7;
    tmp7 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kCreateHTML, tmp0, tmp1, tmp4, tmp5, tmp6, tmp3));
    USE(tmp7);
    arguments->PopAndReturn(tmp7);
  }
}

TF_BUILTIN(StringPrototypeFixed, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  Node* argc = Parameter(Descriptor::kJSActualArgumentsCount);
  CodeStubArguments arguments_impl(this, ChangeInt32ToIntPtr(argc));
  TNode<Object> parameter1 = arguments_impl.GetReceiver();
auto arguments = &arguments_impl;
USE(arguments);
USE(parameter1);
  compiler::CodeAssemblerParameterizedLabel<Context, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../src/builtins/string-html.tq", 75);
    compiler::TNode<String> tmp2;
    USE(tmp2);
    tmp2 = BaseBuiltinsFromDSLAssembler(state_).kEmptyString();
    compiler::TNode<String> tmp3;
    USE(tmp3);
    tmp3 = BaseBuiltinsFromDSLAssembler(state_).kEmptyString();
    ca_.SetSourcePosition("../../src/builtins/string-html.tq", 74);
    compiler::TNode<String> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("String.prototype.fixed"));
    compiler::TNode<String> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("tt"));
    compiler::TNode<String> tmp6;
    tmp6 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kCreateHTML, tmp0, tmp1, tmp4, tmp5, tmp2, tmp3));
    USE(tmp6);
    arguments->PopAndReturn(tmp6);
  }
}

TF_BUILTIN(StringPrototypeItalics, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  Node* argc = Parameter(Descriptor::kJSActualArgumentsCount);
  CodeStubArguments arguments_impl(this, ChangeInt32ToIntPtr(argc));
  TNode<Object> parameter1 = arguments_impl.GetReceiver();
auto arguments = &arguments_impl;
USE(arguments);
USE(parameter1);
  compiler::CodeAssemblerParameterizedLabel<Context, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../src/builtins/string-html.tq", 83);
    compiler::TNode<String> tmp2;
    USE(tmp2);
    tmp2 = BaseBuiltinsFromDSLAssembler(state_).kEmptyString();
    compiler::TNode<String> tmp3;
    USE(tmp3);
    tmp3 = BaseBuiltinsFromDSLAssembler(state_).kEmptyString();
    ca_.SetSourcePosition("../../src/builtins/string-html.tq", 82);
    compiler::TNode<String> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("String.prototype.italics"));
    compiler::TNode<String> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("i"));
    compiler::TNode<String> tmp6;
    tmp6 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kCreateHTML, tmp0, tmp1, tmp4, tmp5, tmp2, tmp3));
    USE(tmp6);
    arguments->PopAndReturn(tmp6);
  }
}

TF_BUILTIN(StringPrototypeLink, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  Node* argc = Parameter(Descriptor::kJSActualArgumentsCount);
  CodeStubArguments arguments_impl(this, ChangeInt32ToIntPtr(argc));
  TNode<Object> parameter1 = arguments_impl.GetReceiver();
auto arguments = &arguments_impl;
USE(arguments);
USE(parameter1);
  compiler::CodeAssemblerParameterizedLabel<Context, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../src/builtins/string-html.tq", 91);
    compiler::TNode<IntPtrT> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(arguments, compiler::TNode<IntPtrT>{tmp2}));
    ca_.SetSourcePosition("../../src/builtins/string-html.tq", 90);
    compiler::TNode<String> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("String.prototype.link"));
    compiler::TNode<String> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("a"));
    compiler::TNode<String> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("href"));
    compiler::TNode<String> tmp7;
    tmp7 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kCreateHTML, tmp0, tmp1, tmp4, tmp5, tmp6, tmp3));
    USE(tmp7);
    arguments->PopAndReturn(tmp7);
  }
}

TF_BUILTIN(StringPrototypeSmall, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  Node* argc = Parameter(Descriptor::kJSActualArgumentsCount);
  CodeStubArguments arguments_impl(this, ChangeInt32ToIntPtr(argc));
  TNode<Object> parameter1 = arguments_impl.GetReceiver();
auto arguments = &arguments_impl;
USE(arguments);
USE(parameter1);
  compiler::CodeAssemblerParameterizedLabel<Context, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../src/builtins/string-html.tq", 99);
    compiler::TNode<String> tmp2;
    USE(tmp2);
    tmp2 = BaseBuiltinsFromDSLAssembler(state_).kEmptyString();
    ca_.SetSourcePosition("../../src/builtins/string-html.tq", 100);
    compiler::TNode<String> tmp3;
    USE(tmp3);
    tmp3 = BaseBuiltinsFromDSLAssembler(state_).kEmptyString();
    ca_.SetSourcePosition("../../src/builtins/string-html.tq", 98);
    compiler::TNode<String> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("String.prototype.small"));
    compiler::TNode<String> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("small"));
    compiler::TNode<String> tmp6;
    tmp6 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kCreateHTML, tmp0, tmp1, tmp4, tmp5, tmp2, tmp3));
    USE(tmp6);
    arguments->PopAndReturn(tmp6);
  }
}

TF_BUILTIN(StringPrototypeStrike, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  Node* argc = Parameter(Descriptor::kJSActualArgumentsCount);
  CodeStubArguments arguments_impl(this, ChangeInt32ToIntPtr(argc));
  TNode<Object> parameter1 = arguments_impl.GetReceiver();
auto arguments = &arguments_impl;
USE(arguments);
USE(parameter1);
  compiler::CodeAssemblerParameterizedLabel<Context, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../src/builtins/string-html.tq", 108);
    compiler::TNode<String> tmp2;
    USE(tmp2);
    tmp2 = BaseBuiltinsFromDSLAssembler(state_).kEmptyString();
    ca_.SetSourcePosition("../../src/builtins/string-html.tq", 109);
    compiler::TNode<String> tmp3;
    USE(tmp3);
    tmp3 = BaseBuiltinsFromDSLAssembler(state_).kEmptyString();
    ca_.SetSourcePosition("../../src/builtins/string-html.tq", 107);
    compiler::TNode<String> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("String.prototype.strike"));
    compiler::TNode<String> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("strike"));
    compiler::TNode<String> tmp6;
    tmp6 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kCreateHTML, tmp0, tmp1, tmp4, tmp5, tmp2, tmp3));
    USE(tmp6);
    arguments->PopAndReturn(tmp6);
  }
}

TF_BUILTIN(StringPrototypeSub, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  Node* argc = Parameter(Descriptor::kJSActualArgumentsCount);
  CodeStubArguments arguments_impl(this, ChangeInt32ToIntPtr(argc));
  TNode<Object> parameter1 = arguments_impl.GetReceiver();
auto arguments = &arguments_impl;
USE(arguments);
USE(parameter1);
  compiler::CodeAssemblerParameterizedLabel<Context, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../src/builtins/string-html.tq", 116);
    compiler::TNode<String> tmp2;
    USE(tmp2);
    tmp2 = BaseBuiltinsFromDSLAssembler(state_).kEmptyString();
    compiler::TNode<String> tmp3;
    USE(tmp3);
    tmp3 = BaseBuiltinsFromDSLAssembler(state_).kEmptyString();
    ca_.SetSourcePosition("../../src/builtins/string-html.tq", 115);
    compiler::TNode<String> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("String.prototype.sub"));
    compiler::TNode<String> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("sub"));
    compiler::TNode<String> tmp6;
    tmp6 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kCreateHTML, tmp0, tmp1, tmp4, tmp5, tmp2, tmp3));
    USE(tmp6);
    arguments->PopAndReturn(tmp6);
  }
}

TF_BUILTIN(StringPrototypeSup, CodeStubAssembler) {
  compiler::CodeAssemblerState* state_ = state();  compiler::CodeAssembler ca_(state());
  TNode<Context> parameter0 = UncheckedCast<Context>(Parameter(Descriptor::kContext));
  USE(parameter0);
  Node* argc = Parameter(Descriptor::kJSActualArgumentsCount);
  CodeStubArguments arguments_impl(this, ChangeInt32ToIntPtr(argc));
  TNode<Object> parameter1 = arguments_impl.GetReceiver();
auto arguments = &arguments_impl;
USE(arguments);
USE(parameter1);
  compiler::CodeAssemblerParameterizedLabel<Context, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../src/builtins/string-html.tq", 123);
    compiler::TNode<String> tmp2;
    USE(tmp2);
    tmp2 = BaseBuiltinsFromDSLAssembler(state_).kEmptyString();
    compiler::TNode<String> tmp3;
    USE(tmp3);
    tmp3 = BaseBuiltinsFromDSLAssembler(state_).kEmptyString();
    ca_.SetSourcePosition("../../src/builtins/string-html.tq", 122);
    compiler::TNode<String> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("String.prototype.sup"));
    compiler::TNode<String> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("sup"));
    compiler::TNode<String> tmp6;
    tmp6 = TORQUE_CAST(CodeStubAssembler(state_).CallBuiltin(Builtins::kCreateHTML, tmp0, tmp1, tmp4, tmp5, tmp2, tmp3));
    USE(tmp6);
    arguments->PopAndReturn(tmp6);
  }
}

}  // namespace internal
}  // namespace v8

