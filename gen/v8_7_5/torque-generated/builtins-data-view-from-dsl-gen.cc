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

compiler::TNode<String> DataViewBuiltinsFromDSLAssembler::MakeDataViewGetterNameString(ElementsKind p_kind) {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block18(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block20(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block21(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block23(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block24(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block26(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block27(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block29(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block30(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<String> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<String> block32(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

  if (block0.is_used()) {
    ca_.Bind(&block0);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 10);
    if (((CodeStubAssembler(state_).ElementsKindEqual(p_kind, UINT8_ELEMENTS)))) {
      ca_.Goto(&block2);
    } else {
      ca_.Goto(&block3);
    }
  }

  if (block2.is_used()) {
    ca_.Bind(&block2);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 11);
    compiler::TNode<String> tmp0;
    USE(tmp0);
    tmp0 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("DataView.prototype.getUint8"));
    ca_.Goto(&block1, tmp0);
  }

  if (block3.is_used()) {
    ca_.Bind(&block3);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 12);
    if (((CodeStubAssembler(state_).ElementsKindEqual(p_kind, INT8_ELEMENTS)))) {
      ca_.Goto(&block5);
    } else {
      ca_.Goto(&block6);
    }
  }

  if (block5.is_used()) {
    ca_.Bind(&block5);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 13);
    compiler::TNode<String> tmp1;
    USE(tmp1);
    tmp1 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("DataView.prototype.getInt8"));
    ca_.Goto(&block1, tmp1);
  }

  if (block6.is_used()) {
    ca_.Bind(&block6);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 14);
    if (((CodeStubAssembler(state_).ElementsKindEqual(p_kind, UINT16_ELEMENTS)))) {
      ca_.Goto(&block8);
    } else {
      ca_.Goto(&block9);
    }
  }

  if (block8.is_used()) {
    ca_.Bind(&block8);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 15);
    compiler::TNode<String> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("DataView.prototype.getUint16"));
    ca_.Goto(&block1, tmp2);
  }

  if (block9.is_used()) {
    ca_.Bind(&block9);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 16);
    if (((CodeStubAssembler(state_).ElementsKindEqual(p_kind, INT16_ELEMENTS)))) {
      ca_.Goto(&block11);
    } else {
      ca_.Goto(&block12);
    }
  }

  if (block11.is_used()) {
    ca_.Bind(&block11);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 17);
    compiler::TNode<String> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("DataView.prototype.getInt16"));
    ca_.Goto(&block1, tmp3);
  }

  if (block12.is_used()) {
    ca_.Bind(&block12);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 18);
    if (((CodeStubAssembler(state_).ElementsKindEqual(p_kind, UINT32_ELEMENTS)))) {
      ca_.Goto(&block14);
    } else {
      ca_.Goto(&block15);
    }
  }

  if (block14.is_used()) {
    ca_.Bind(&block14);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 19);
    compiler::TNode<String> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("DataView.prototype.getUint32"));
    ca_.Goto(&block1, tmp4);
  }

  if (block15.is_used()) {
    ca_.Bind(&block15);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 20);
    if (((CodeStubAssembler(state_).ElementsKindEqual(p_kind, INT32_ELEMENTS)))) {
      ca_.Goto(&block17);
    } else {
      ca_.Goto(&block18);
    }
  }

  if (block17.is_used()) {
    ca_.Bind(&block17);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 21);
    compiler::TNode<String> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("DataView.prototype.getInt32"));
    ca_.Goto(&block1, tmp5);
  }

  if (block18.is_used()) {
    ca_.Bind(&block18);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 22);
    if (((CodeStubAssembler(state_).ElementsKindEqual(p_kind, FLOAT32_ELEMENTS)))) {
      ca_.Goto(&block20);
    } else {
      ca_.Goto(&block21);
    }
  }

  if (block20.is_used()) {
    ca_.Bind(&block20);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 23);
    compiler::TNode<String> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("DataView.prototype.getFloat32"));
    ca_.Goto(&block1, tmp6);
  }

  if (block21.is_used()) {
    ca_.Bind(&block21);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 24);
    if (((CodeStubAssembler(state_).ElementsKindEqual(p_kind, FLOAT64_ELEMENTS)))) {
      ca_.Goto(&block23);
    } else {
      ca_.Goto(&block24);
    }
  }

  if (block23.is_used()) {
    ca_.Bind(&block23);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 25);
    compiler::TNode<String> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("DataView.prototype.getFloat64"));
    ca_.Goto(&block1, tmp7);
  }

  if (block24.is_used()) {
    ca_.Bind(&block24);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 26);
    if (((CodeStubAssembler(state_).ElementsKindEqual(p_kind, BIGINT64_ELEMENTS)))) {
      ca_.Goto(&block26);
    } else {
      ca_.Goto(&block27);
    }
  }

  if (block26.is_used()) {
    ca_.Bind(&block26);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 27);
    compiler::TNode<String> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("DataView.prototype.getBigInt64"));
    ca_.Goto(&block1, tmp8);
  }

  if (block27.is_used()) {
    ca_.Bind(&block27);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 28);
    if (((CodeStubAssembler(state_).ElementsKindEqual(p_kind, BIGUINT64_ELEMENTS)))) {
      ca_.Goto(&block29);
    } else {
      ca_.Goto(&block30);
    }
  }

  if (block29.is_used()) {
    ca_.Bind(&block29);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 29);
    compiler::TNode<String> tmp9;
    USE(tmp9);
    tmp9 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("DataView.prototype.getBigUint64"));
    ca_.Goto(&block1, tmp9);
  }

  if (block30.is_used()) {
    ca_.Bind(&block30);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 31);
    CodeStubAssembler(state_).Unreachable();
  }

  if (block1.is_used()) {
    compiler::TNode<String> tmp10;
    ca_.Bind(&block1, &tmp10);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 9);
    ca_.Goto(&block32, tmp10);
  }

    compiler::TNode<String> tmp11;
    ca_.Bind(&block32, &tmp11);
  return compiler::TNode<String>{tmp11};
}

compiler::TNode<String> DataViewBuiltinsFromDSLAssembler::MakeDataViewSetterNameString(ElementsKind p_kind) {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block18(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block20(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block21(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block23(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block24(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block26(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block27(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block29(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<> block30(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<String> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<String> block32(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

  if (block0.is_used()) {
    ca_.Bind(&block0);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 36);
    if (((CodeStubAssembler(state_).ElementsKindEqual(p_kind, UINT8_ELEMENTS)))) {
      ca_.Goto(&block2);
    } else {
      ca_.Goto(&block3);
    }
  }

  if (block2.is_used()) {
    ca_.Bind(&block2);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 37);
    compiler::TNode<String> tmp0;
    USE(tmp0);
    tmp0 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("DataView.prototype.setUint8"));
    ca_.Goto(&block1, tmp0);
  }

  if (block3.is_used()) {
    ca_.Bind(&block3);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 38);
    if (((CodeStubAssembler(state_).ElementsKindEqual(p_kind, INT8_ELEMENTS)))) {
      ca_.Goto(&block5);
    } else {
      ca_.Goto(&block6);
    }
  }

  if (block5.is_used()) {
    ca_.Bind(&block5);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 39);
    compiler::TNode<String> tmp1;
    USE(tmp1);
    tmp1 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("DataView.prototype.setInt8"));
    ca_.Goto(&block1, tmp1);
  }

  if (block6.is_used()) {
    ca_.Bind(&block6);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 40);
    if (((CodeStubAssembler(state_).ElementsKindEqual(p_kind, UINT16_ELEMENTS)))) {
      ca_.Goto(&block8);
    } else {
      ca_.Goto(&block9);
    }
  }

  if (block8.is_used()) {
    ca_.Bind(&block8);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 41);
    compiler::TNode<String> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("DataView.prototype.setUint16"));
    ca_.Goto(&block1, tmp2);
  }

  if (block9.is_used()) {
    ca_.Bind(&block9);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 42);
    if (((CodeStubAssembler(state_).ElementsKindEqual(p_kind, INT16_ELEMENTS)))) {
      ca_.Goto(&block11);
    } else {
      ca_.Goto(&block12);
    }
  }

  if (block11.is_used()) {
    ca_.Bind(&block11);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 43);
    compiler::TNode<String> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("DataView.prototype.setInt16"));
    ca_.Goto(&block1, tmp3);
  }

  if (block12.is_used()) {
    ca_.Bind(&block12);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 44);
    if (((CodeStubAssembler(state_).ElementsKindEqual(p_kind, UINT32_ELEMENTS)))) {
      ca_.Goto(&block14);
    } else {
      ca_.Goto(&block15);
    }
  }

  if (block14.is_used()) {
    ca_.Bind(&block14);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 45);
    compiler::TNode<String> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("DataView.prototype.setUint32"));
    ca_.Goto(&block1, tmp4);
  }

  if (block15.is_used()) {
    ca_.Bind(&block15);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 46);
    if (((CodeStubAssembler(state_).ElementsKindEqual(p_kind, INT32_ELEMENTS)))) {
      ca_.Goto(&block17);
    } else {
      ca_.Goto(&block18);
    }
  }

  if (block17.is_used()) {
    ca_.Bind(&block17);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 47);
    compiler::TNode<String> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("DataView.prototype.setInt32"));
    ca_.Goto(&block1, tmp5);
  }

  if (block18.is_used()) {
    ca_.Bind(&block18);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 48);
    if (((CodeStubAssembler(state_).ElementsKindEqual(p_kind, FLOAT32_ELEMENTS)))) {
      ca_.Goto(&block20);
    } else {
      ca_.Goto(&block21);
    }
  }

  if (block20.is_used()) {
    ca_.Bind(&block20);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 49);
    compiler::TNode<String> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("DataView.prototype.setFloat32"));
    ca_.Goto(&block1, tmp6);
  }

  if (block21.is_used()) {
    ca_.Bind(&block21);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 50);
    if (((CodeStubAssembler(state_).ElementsKindEqual(p_kind, FLOAT64_ELEMENTS)))) {
      ca_.Goto(&block23);
    } else {
      ca_.Goto(&block24);
    }
  }

  if (block23.is_used()) {
    ca_.Bind(&block23);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 51);
    compiler::TNode<String> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("DataView.prototype.setFloat64"));
    ca_.Goto(&block1, tmp7);
  }

  if (block24.is_used()) {
    ca_.Bind(&block24);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 52);
    if (((CodeStubAssembler(state_).ElementsKindEqual(p_kind, BIGINT64_ELEMENTS)))) {
      ca_.Goto(&block26);
    } else {
      ca_.Goto(&block27);
    }
  }

  if (block26.is_used()) {
    ca_.Bind(&block26);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 53);
    compiler::TNode<String> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("DataView.prototype.setBigInt64"));
    ca_.Goto(&block1, tmp8);
  }

  if (block27.is_used()) {
    ca_.Bind(&block27);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 54);
    if (((CodeStubAssembler(state_).ElementsKindEqual(p_kind, BIGUINT64_ELEMENTS)))) {
      ca_.Goto(&block29);
    } else {
      ca_.Goto(&block30);
    }
  }

  if (block29.is_used()) {
    ca_.Bind(&block29);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 55);
    compiler::TNode<String> tmp9;
    USE(tmp9);
    tmp9 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("DataView.prototype.setBigUint64"));
    ca_.Goto(&block1, tmp9);
  }

  if (block30.is_used()) {
    ca_.Bind(&block30);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 57);
    CodeStubAssembler(state_).Unreachable();
  }

  if (block1.is_used()) {
    compiler::TNode<String> tmp10;
    ca_.Bind(&block1, &tmp10);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 35);
    ca_.Goto(&block32, tmp10);
  }

    compiler::TNode<String> tmp11;
    ca_.Bind(&block32, &tmp11);
  return compiler::TNode<String>{tmp11};
}

compiler::TNode<BoolT> DataViewBuiltinsFromDSLAssembler::WasNeutered(compiler::TNode<JSArrayBufferView> p_view) {
  compiler::CodeAssemblerParameterizedLabel<JSArrayBufferView> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBufferView, BoolT> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBufferView, BoolT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_view);

  if (block0.is_used()) {
    compiler::TNode<JSArrayBufferView> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 62);
    compiler::TNode<IntPtrT> tmp1 = ca_.IntPtrConstant(JSArrayBufferView::kBufferOffset);
    USE(tmp1);
    compiler::TNode<JSArrayBuffer>tmp2 = CodeStubAssembler(state_).LoadReference<JSArrayBuffer>(CodeStubAssembler::Reference{tmp0, tmp1});
    compiler::TNode<BoolT> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsDetachedBuffer(compiler::TNode<JSArrayBuffer>{tmp2}));
    ca_.Goto(&block1, tmp0, tmp3);
  }

  if (block1.is_used()) {
    compiler::TNode<JSArrayBufferView> tmp4;
    compiler::TNode<BoolT> tmp5;
    ca_.Bind(&block1, &tmp4, &tmp5);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 61);
    ca_.Goto(&block2, tmp4, tmp5);
  }

    compiler::TNode<JSArrayBufferView> tmp6;
    compiler::TNode<BoolT> tmp7;
    ca_.Bind(&block2, &tmp6, &tmp7);
  return compiler::TNode<BoolT>{tmp7};
}

compiler::TNode<JSDataView> DataViewBuiltinsFromDSLAssembler::ValidateDataView(compiler::TNode<Context> p_context, compiler::TNode<Object> p_o, compiler::TNode<String> p_method) {
  compiler::CodeAssemblerParameterizedLabel<Context, Object, String> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, String, Object> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, String, Object, JSDataView> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, String> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, String, JSDataView> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, String, JSDataView> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_o, p_method);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<String> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 68);
    compiler::TNode<JSDataView> tmp3;
    USE(tmp3);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp3 = BaseBuiltinsFromDSLAssembler(state_).Cast10JSDataView(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}, &label0);
    ca_.Goto(&block4, tmp0, tmp1, tmp2, tmp1, tmp3);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block5, tmp0, tmp1, tmp2, tmp1);
    }
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp4;
    compiler::TNode<Object> tmp5;
    compiler::TNode<String> tmp6;
    compiler::TNode<Object> tmp7;
    ca_.Bind(&block5, &tmp4, &tmp5, &tmp6, &tmp7);
    ca_.Goto(&block3, tmp4, tmp5, tmp6);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp8;
    compiler::TNode<Object> tmp9;
    compiler::TNode<String> tmp10;
    compiler::TNode<Object> tmp11;
    compiler::TNode<JSDataView> tmp12;
    ca_.Bind(&block4, &tmp8, &tmp9, &tmp10, &tmp11, &tmp12);
    ca_.Goto(&block1, tmp8, tmp9, tmp10, tmp12);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp13;
    compiler::TNode<Object> tmp14;
    compiler::TNode<String> tmp15;
    ca_.Bind(&block3, &tmp13, &tmp14, &tmp15);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 71);
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp13}, MessageTemplate::kIncompatibleMethodReceiver, compiler::TNode<Object>{tmp15});
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp16;
    compiler::TNode<Object> tmp17;
    compiler::TNode<String> tmp18;
    compiler::TNode<JSDataView> tmp19;
    ca_.Bind(&block1, &tmp16, &tmp17, &tmp18, &tmp19);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 65);
    ca_.Goto(&block6, tmp16, tmp17, tmp18, tmp19);
  }

    compiler::TNode<Context> tmp20;
    compiler::TNode<Object> tmp21;
    compiler::TNode<String> tmp22;
    compiler::TNode<JSDataView> tmp23;
    ca_.Bind(&block6, &tmp20, &tmp21, &tmp22, &tmp23);
  return compiler::TNode<JSDataView>{tmp23};
}

TF_BUILTIN(DataViewPrototypeGetBuffer, CodeStubAssembler) {
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
    ca_.Goto(&block0, parameter0, parameter1, torque_arguments.frame, torque_arguments.base, torque_arguments.length);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<RawPtrT> tmp2;
    compiler::TNode<RawPtrT> tmp3;
    compiler::TNode<IntPtrT> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 79);
    compiler::TNode<String> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("get DataView.prototype.buffer"));
    compiler::TNode<JSDataView> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<JSDataView>(DataViewBuiltinsFromDSLAssembler(state_).ValidateDataView(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}, compiler::TNode<String>{tmp5}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 78);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 80);
    compiler::TNode<IntPtrT> tmp7 = ca_.IntPtrConstant(JSArrayBufferView::kBufferOffset);
    USE(tmp7);
    compiler::TNode<JSArrayBuffer>tmp8 = CodeStubAssembler(state_).LoadReference<JSArrayBuffer>(CodeStubAssembler::Reference{tmp6, tmp7});
    arguments.PopAndReturn(tmp8);
  }
}

TF_BUILTIN(DataViewPrototypeGetByteLength, CodeStubAssembler) {
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
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSDataView> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSDataView> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, torque_arguments.frame, torque_arguments.base, torque_arguments.length);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<RawPtrT> tmp2;
    compiler::TNode<RawPtrT> tmp3;
    compiler::TNode<IntPtrT> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 87);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 86);
    compiler::TNode<String> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("get DataView.prototype.byte_length"));
    compiler::TNode<JSDataView> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<JSDataView>(DataViewBuiltinsFromDSLAssembler(state_).ValidateDataView(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}, compiler::TNode<String>{tmp5}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 88);
    compiler::TNode<BoolT> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<BoolT>(DataViewBuiltinsFromDSLAssembler(state_).WasNeutered(compiler::TNode<JSArrayBufferView>{tmp6}));
    ca_.Branch(tmp7, &block1, &block2, tmp0, tmp1, tmp2, tmp3, tmp4, tmp6);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp8;
    compiler::TNode<Object> tmp9;
    compiler::TNode<RawPtrT> tmp10;
    compiler::TNode<RawPtrT> tmp11;
    compiler::TNode<IntPtrT> tmp12;
    compiler::TNode<JSDataView> tmp13;
    ca_.Bind(&block1, &tmp8, &tmp9, &tmp10, &tmp11, &tmp12, &tmp13);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 91);
    compiler::TNode<Number> tmp14;
    USE(tmp14);
    tmp14 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(0));
    arguments.PopAndReturn(tmp14);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp15;
    compiler::TNode<Object> tmp16;
    compiler::TNode<RawPtrT> tmp17;
    compiler::TNode<RawPtrT> tmp18;
    compiler::TNode<IntPtrT> tmp19;
    compiler::TNode<JSDataView> tmp20;
    ca_.Bind(&block2, &tmp15, &tmp16, &tmp17, &tmp18, &tmp19, &tmp20);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 93);
    compiler::TNode<IntPtrT> tmp21 = ca_.IntPtrConstant(JSArrayBufferView::kByteLengthOffset);
    USE(tmp21);
    compiler::TNode<UintPtrT>tmp22 = CodeStubAssembler(state_).LoadReference<UintPtrT>(CodeStubAssembler::Reference{tmp20, tmp21});
    compiler::TNode<Number> tmp23;
    USE(tmp23);
    tmp23 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).Convert20UT5ATSmi10HeapNumber9ATuintptr(compiler::TNode<UintPtrT>{tmp22}));
    arguments.PopAndReturn(tmp23);
  }
}

TF_BUILTIN(DataViewPrototypeGetByteOffset, CodeStubAssembler) {
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
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSDataView> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, JSDataView> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, torque_arguments.frame, torque_arguments.base, torque_arguments.length);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<RawPtrT> tmp2;
    compiler::TNode<RawPtrT> tmp3;
    compiler::TNode<IntPtrT> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 100);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 99);
    compiler::TNode<String> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<String>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6String18ATconstexpr_string("get DataView.prototype.byte_offset"));
    compiler::TNode<JSDataView> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<JSDataView>(DataViewBuiltinsFromDSLAssembler(state_).ValidateDataView(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}, compiler::TNode<String>{tmp5}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 101);
    compiler::TNode<BoolT> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<BoolT>(DataViewBuiltinsFromDSLAssembler(state_).WasNeutered(compiler::TNode<JSArrayBufferView>{tmp6}));
    ca_.Branch(tmp7, &block1, &block2, tmp0, tmp1, tmp2, tmp3, tmp4, tmp6);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp8;
    compiler::TNode<Object> tmp9;
    compiler::TNode<RawPtrT> tmp10;
    compiler::TNode<RawPtrT> tmp11;
    compiler::TNode<IntPtrT> tmp12;
    compiler::TNode<JSDataView> tmp13;
    ca_.Bind(&block1, &tmp8, &tmp9, &tmp10, &tmp11, &tmp12, &tmp13);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 104);
    compiler::TNode<Number> tmp14;
    USE(tmp14);
    tmp14 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr20UT5ATSmi10HeapNumber17ATconstexpr_int31(0));
    arguments.PopAndReturn(tmp14);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp15;
    compiler::TNode<Object> tmp16;
    compiler::TNode<RawPtrT> tmp17;
    compiler::TNode<RawPtrT> tmp18;
    compiler::TNode<IntPtrT> tmp19;
    compiler::TNode<JSDataView> tmp20;
    ca_.Bind(&block2, &tmp15, &tmp16, &tmp17, &tmp18, &tmp19, &tmp20);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 106);
    compiler::TNode<IntPtrT> tmp21 = ca_.IntPtrConstant(JSArrayBufferView::kByteOffsetOffset);
    USE(tmp21);
    compiler::TNode<UintPtrT>tmp22 = CodeStubAssembler(state_).LoadReference<UintPtrT>(CodeStubAssembler::Reference{tmp20, tmp21});
    compiler::TNode<Number> tmp23;
    USE(tmp23);
    tmp23 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).Convert20UT5ATSmi10HeapNumber9ATuintptr(compiler::TNode<UintPtrT>{tmp22}));
    arguments.PopAndReturn(tmp23);
  }
}

compiler::TNode<Smi> DataViewBuiltinsFromDSLAssembler::LoadDataView8(compiler::TNode<JSArrayBuffer> p_buffer, compiler::TNode<UintPtrT> p_offset, bool p_signed) {
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, Smi> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, Smi> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_buffer, p_offset);

  if (block0.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp0;
    compiler::TNode<UintPtrT> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 121);
    if ((p_signed)) {
      ca_.Goto(&block2, tmp0, tmp1);
    } else {
      ca_.Goto(&block3, tmp0, tmp1);
    }
  }

  if (block2.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp2;
    compiler::TNode<UintPtrT> tmp3;
    ca_.Bind(&block2, &tmp2, &tmp3);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 122);
    compiler::TNode<IntPtrT> tmp4 = ca_.IntPtrConstant(JSArrayBuffer::kBackingStoreOffset);
    USE(tmp4);
    compiler::TNode<RawPtrT>tmp5 = CodeStubAssembler(state_).LoadReference<RawPtrT>(CodeStubAssembler::Reference{tmp2, tmp4});
    compiler::TNode<Int32T> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<Int32T>(DataViewBuiltinsAssembler(state_).LoadInt8(compiler::TNode<RawPtrT>{tmp5}, compiler::TNode<UintPtrT>{tmp3}));
    compiler::TNode<Smi> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).Convert5ATSmi7ATint32(compiler::TNode<Int32T>{tmp6}));
    ca_.Goto(&block1, tmp2, tmp3, tmp7);
  }

  if (block3.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp8;
    compiler::TNode<UintPtrT> tmp9;
    ca_.Bind(&block3, &tmp8, &tmp9);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 124);
    compiler::TNode<IntPtrT> tmp10 = ca_.IntPtrConstant(JSArrayBuffer::kBackingStoreOffset);
    USE(tmp10);
    compiler::TNode<RawPtrT>tmp11 = CodeStubAssembler(state_).LoadReference<RawPtrT>(CodeStubAssembler::Reference{tmp8, tmp10});
    compiler::TNode<Uint32T> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<Uint32T>(DataViewBuiltinsAssembler(state_).LoadUint8(compiler::TNode<RawPtrT>{tmp11}, compiler::TNode<UintPtrT>{tmp9}));
    compiler::TNode<Smi> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).Convert5ATSmi8ATuint32(compiler::TNode<Uint32T>{tmp12}));
    ca_.Goto(&block1, tmp8, tmp9, tmp13);
  }

  if (block1.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp14;
    compiler::TNode<UintPtrT> tmp15;
    compiler::TNode<Smi> tmp16;
    ca_.Bind(&block1, &tmp14, &tmp15, &tmp16);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 119);
    ca_.Goto(&block5, tmp14, tmp15, tmp16);
  }

    compiler::TNode<JSArrayBuffer> tmp17;
    compiler::TNode<UintPtrT> tmp18;
    compiler::TNode<Smi> tmp19;
    ca_.Bind(&block5, &tmp17, &tmp18, &tmp19);
  return compiler::TNode<Smi>{tmp19};
}

compiler::TNode<Number> DataViewBuiltinsFromDSLAssembler::LoadDataView16(compiler::TNode<JSArrayBuffer> p_buffer, compiler::TNode<UintPtrT> p_offset, compiler::TNode<BoolT> p_requestedLittleEndian, bool p_signed) {
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, BoolT> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, BoolT, RawPtrT, Int32T, Int32T, Int32T> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, BoolT, RawPtrT, Int32T, Int32T, Int32T> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, BoolT, RawPtrT, Int32T, Int32T, Int32T> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, BoolT, RawPtrT, Int32T, Int32T, Int32T> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, BoolT, RawPtrT, Int32T, Int32T, Int32T> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, BoolT, Number> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, BoolT, Number> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_buffer, p_offset, p_requestedLittleEndian);

  if (block0.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp0;
    compiler::TNode<UintPtrT> tmp1;
    compiler::TNode<BoolT> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 131);
    compiler::TNode<IntPtrT> tmp3 = ca_.IntPtrConstant(JSArrayBuffer::kBackingStoreOffset);
    USE(tmp3);
    compiler::TNode<RawPtrT>tmp4 = CodeStubAssembler(state_).LoadReference<RawPtrT>(CodeStubAssembler::Reference{tmp0, tmp3});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 133);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 134);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 135);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 138);
    ca_.Branch(tmp2, &block2, &block3, tmp0, tmp1, tmp2, tmp4, ca_.Uninitialized<Int32T>(), ca_.Uninitialized<Int32T>(), ca_.Uninitialized<Int32T>());
  }

  if (block2.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp5;
    compiler::TNode<UintPtrT> tmp6;
    compiler::TNode<BoolT> tmp7;
    compiler::TNode<RawPtrT> tmp8;
    compiler::TNode<Int32T> tmp9;
    compiler::TNode<Int32T> tmp10;
    compiler::TNode<Int32T> tmp11;
    ca_.Bind(&block2, &tmp5, &tmp6, &tmp7, &tmp8, &tmp9, &tmp10, &tmp11);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 139);
    compiler::TNode<Uint32T> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<Uint32T>(DataViewBuiltinsAssembler(state_).LoadUint8(compiler::TNode<RawPtrT>{tmp8}, compiler::TNode<UintPtrT>{tmp6}));
    compiler::TNode<Int32T> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<Int32T>(CodeStubAssembler(state_).Signed(compiler::TNode<Uint32T>{tmp12}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 140);
    compiler::TNode<UintPtrT> tmp14;
    USE(tmp14);
    tmp14 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(1));
    compiler::TNode<UintPtrT> tmp15;
    USE(tmp15);
    tmp15 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrAdd(compiler::TNode<UintPtrT>{tmp6}, compiler::TNode<UintPtrT>{tmp14}));
    compiler::TNode<Int32T> tmp16;
    USE(tmp16);
    tmp16 = ca_.UncheckedCast<Int32T>(DataViewBuiltinsAssembler(state_).LoadInt8(compiler::TNode<RawPtrT>{tmp8}, compiler::TNode<UintPtrT>{tmp15}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 141);
    compiler::TNode<Int32T> tmp17;
    USE(tmp17);
    tmp17 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr7ATint3217ATconstexpr_int31(8));
    compiler::TNode<Int32T> tmp18;
    USE(tmp18);
    tmp18 = ca_.UncheckedCast<Int32T>(CodeStubAssembler(state_).Word32Shl(compiler::TNode<Int32T>{tmp16}, compiler::TNode<Int32T>{tmp17}));
    compiler::TNode<Int32T> tmp19;
    USE(tmp19);
    tmp19 = ca_.UncheckedCast<Int32T>(CodeStubAssembler(state_).Int32Add(compiler::TNode<Int32T>{tmp18}, compiler::TNode<Int32T>{tmp13}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 138);
    ca_.Goto(&block4, tmp5, tmp6, tmp7, tmp8, tmp13, tmp16, tmp19);
  }

  if (block3.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp20;
    compiler::TNode<UintPtrT> tmp21;
    compiler::TNode<BoolT> tmp22;
    compiler::TNode<RawPtrT> tmp23;
    compiler::TNode<Int32T> tmp24;
    compiler::TNode<Int32T> tmp25;
    compiler::TNode<Int32T> tmp26;
    ca_.Bind(&block3, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 143);
    compiler::TNode<Int32T> tmp27;
    USE(tmp27);
    tmp27 = ca_.UncheckedCast<Int32T>(DataViewBuiltinsAssembler(state_).LoadInt8(compiler::TNode<RawPtrT>{tmp23}, compiler::TNode<UintPtrT>{tmp21}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 144);
    compiler::TNode<UintPtrT> tmp28;
    USE(tmp28);
    tmp28 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(1));
    compiler::TNode<UintPtrT> tmp29;
    USE(tmp29);
    tmp29 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrAdd(compiler::TNode<UintPtrT>{tmp21}, compiler::TNode<UintPtrT>{tmp28}));
    compiler::TNode<Uint32T> tmp30;
    USE(tmp30);
    tmp30 = ca_.UncheckedCast<Uint32T>(DataViewBuiltinsAssembler(state_).LoadUint8(compiler::TNode<RawPtrT>{tmp23}, compiler::TNode<UintPtrT>{tmp29}));
    compiler::TNode<Int32T> tmp31;
    USE(tmp31);
    tmp31 = ca_.UncheckedCast<Int32T>(CodeStubAssembler(state_).Signed(compiler::TNode<Uint32T>{tmp30}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 145);
    compiler::TNode<Int32T> tmp32;
    USE(tmp32);
    tmp32 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr7ATint3217ATconstexpr_int31(8));
    compiler::TNode<Int32T> tmp33;
    USE(tmp33);
    tmp33 = ca_.UncheckedCast<Int32T>(CodeStubAssembler(state_).Word32Shl(compiler::TNode<Int32T>{tmp27}, compiler::TNode<Int32T>{tmp32}));
    compiler::TNode<Int32T> tmp34;
    USE(tmp34);
    tmp34 = ca_.UncheckedCast<Int32T>(CodeStubAssembler(state_).Int32Add(compiler::TNode<Int32T>{tmp33}, compiler::TNode<Int32T>{tmp31}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 138);
    ca_.Goto(&block4, tmp20, tmp21, tmp22, tmp23, tmp27, tmp31, tmp34);
  }

  if (block4.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp35;
    compiler::TNode<UintPtrT> tmp36;
    compiler::TNode<BoolT> tmp37;
    compiler::TNode<RawPtrT> tmp38;
    compiler::TNode<Int32T> tmp39;
    compiler::TNode<Int32T> tmp40;
    compiler::TNode<Int32T> tmp41;
    ca_.Bind(&block4, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 147);
    if ((p_signed)) {
      ca_.Goto(&block5, tmp35, tmp36, tmp37, tmp38, tmp39, tmp40, tmp41);
    } else {
      ca_.Goto(&block6, tmp35, tmp36, tmp37, tmp38, tmp39, tmp40, tmp41);
    }
  }

  if (block5.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp42;
    compiler::TNode<UintPtrT> tmp43;
    compiler::TNode<BoolT> tmp44;
    compiler::TNode<RawPtrT> tmp45;
    compiler::TNode<Int32T> tmp46;
    compiler::TNode<Int32T> tmp47;
    compiler::TNode<Int32T> tmp48;
    ca_.Bind(&block5, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 148);
    compiler::TNode<Smi> tmp49;
    USE(tmp49);
    tmp49 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).Convert5ATSmi7ATint32(compiler::TNode<Int32T>{tmp48}));
    ca_.Goto(&block1, tmp42, tmp43, tmp44, tmp49);
  }

  if (block6.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp50;
    compiler::TNode<UintPtrT> tmp51;
    compiler::TNode<BoolT> tmp52;
    compiler::TNode<RawPtrT> tmp53;
    compiler::TNode<Int32T> tmp54;
    compiler::TNode<Int32T> tmp55;
    compiler::TNode<Int32T> tmp56;
    ca_.Bind(&block6, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55, &tmp56);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 151);
    compiler::TNode<Int32T> tmp57;
    USE(tmp57);
    tmp57 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr7ATint3217ATconstexpr_int31(0xFFFF));
    compiler::TNode<Int32T> tmp58;
    USE(tmp58);
    tmp58 = ca_.UncheckedCast<Int32T>(CodeStubAssembler(state_).Word32And(compiler::TNode<Int32T>{tmp56}, compiler::TNode<Int32T>{tmp57}));
    compiler::TNode<Smi> tmp59;
    USE(tmp59);
    tmp59 = ca_.UncheckedCast<Smi>(BaseBuiltinsFromDSLAssembler(state_).Convert5ATSmi7ATint32(compiler::TNode<Int32T>{tmp58}));
    ca_.Goto(&block1, tmp50, tmp51, tmp52, tmp59);
  }

  if (block1.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp60;
    compiler::TNode<UintPtrT> tmp61;
    compiler::TNode<BoolT> tmp62;
    compiler::TNode<Number> tmp63;
    ca_.Bind(&block1, &tmp60, &tmp61, &tmp62, &tmp63);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 128);
    ca_.Goto(&block8, tmp60, tmp61, tmp62, tmp63);
  }

    compiler::TNode<JSArrayBuffer> tmp64;
    compiler::TNode<UintPtrT> tmp65;
    compiler::TNode<BoolT> tmp66;
    compiler::TNode<Number> tmp67;
    ca_.Bind(&block8, &tmp64, &tmp65, &tmp66, &tmp67);
  return compiler::TNode<Number>{tmp67};
}

compiler::TNode<Number> DataViewBuiltinsFromDSLAssembler::LoadDataView32(compiler::TNode<JSArrayBuffer> p_buffer, compiler::TNode<UintPtrT> p_offset, compiler::TNode<BoolT> p_requestedLittleEndian, ElementsKind p_kind) {
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, BoolT> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, BoolT, RawPtrT, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, BoolT, RawPtrT, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, BoolT, RawPtrT, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, BoolT, RawPtrT, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, BoolT, RawPtrT, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, BoolT, RawPtrT, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, BoolT, RawPtrT, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, BoolT, RawPtrT, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, BoolT, RawPtrT, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, BoolT, Number> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, BoolT, Number> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_buffer, p_offset, p_requestedLittleEndian);

  if (block0.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp0;
    compiler::TNode<UintPtrT> tmp1;
    compiler::TNode<BoolT> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 158);
    compiler::TNode<IntPtrT> tmp3 = ca_.IntPtrConstant(JSArrayBuffer::kBackingStoreOffset);
    USE(tmp3);
    compiler::TNode<RawPtrT>tmp4 = CodeStubAssembler(state_).LoadReference<RawPtrT>(CodeStubAssembler::Reference{tmp0, tmp3});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 160);
    compiler::TNode<Uint32T> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<Uint32T>(DataViewBuiltinsAssembler(state_).LoadUint8(compiler::TNode<RawPtrT>{tmp4}, compiler::TNode<UintPtrT>{tmp1}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 161);
    compiler::TNode<UintPtrT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(1));
    compiler::TNode<UintPtrT> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrAdd(compiler::TNode<UintPtrT>{tmp1}, compiler::TNode<UintPtrT>{tmp6}));
    compiler::TNode<Uint32T> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<Uint32T>(DataViewBuiltinsAssembler(state_).LoadUint8(compiler::TNode<RawPtrT>{tmp4}, compiler::TNode<UintPtrT>{tmp7}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 162);
    compiler::TNode<UintPtrT> tmp9;
    USE(tmp9);
    tmp9 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(2));
    compiler::TNode<UintPtrT> tmp10;
    USE(tmp10);
    tmp10 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrAdd(compiler::TNode<UintPtrT>{tmp1}, compiler::TNode<UintPtrT>{tmp9}));
    compiler::TNode<Uint32T> tmp11;
    USE(tmp11);
    tmp11 = ca_.UncheckedCast<Uint32T>(DataViewBuiltinsAssembler(state_).LoadUint8(compiler::TNode<RawPtrT>{tmp4}, compiler::TNode<UintPtrT>{tmp10}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 163);
    compiler::TNode<UintPtrT> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(3));
    compiler::TNode<UintPtrT> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrAdd(compiler::TNode<UintPtrT>{tmp1}, compiler::TNode<UintPtrT>{tmp12}));
    compiler::TNode<Uint32T> tmp14;
    USE(tmp14);
    tmp14 = ca_.UncheckedCast<Uint32T>(DataViewBuiltinsAssembler(state_).LoadUint8(compiler::TNode<RawPtrT>{tmp4}, compiler::TNode<UintPtrT>{tmp13}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 164);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 166);
    ca_.Branch(tmp2, &block2, &block3, tmp0, tmp1, tmp2, tmp4, tmp5, tmp8, tmp11, tmp14, ca_.Uninitialized<Uint32T>());
  }

  if (block2.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp15;
    compiler::TNode<UintPtrT> tmp16;
    compiler::TNode<BoolT> tmp17;
    compiler::TNode<RawPtrT> tmp18;
    compiler::TNode<Uint32T> tmp19;
    compiler::TNode<Uint32T> tmp20;
    compiler::TNode<Uint32T> tmp21;
    compiler::TNode<Uint32T> tmp22;
    compiler::TNode<Uint32T> tmp23;
    ca_.Bind(&block2, &tmp15, &tmp16, &tmp17, &tmp18, &tmp19, &tmp20, &tmp21, &tmp22, &tmp23);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 167);
    compiler::TNode<Uint32T> tmp24;
    USE(tmp24);
    tmp24 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(24));
    compiler::TNode<Uint32T> tmp25;
    USE(tmp25);
    tmp25 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Shl(compiler::TNode<Uint32T>{tmp22}, compiler::TNode<Uint32T>{tmp24}));
    compiler::TNode<Uint32T> tmp26;
    USE(tmp26);
    tmp26 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(16));
    compiler::TNode<Uint32T> tmp27;
    USE(tmp27);
    tmp27 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Shl(compiler::TNode<Uint32T>{tmp21}, compiler::TNode<Uint32T>{tmp26}));
    compiler::TNode<Uint32T> tmp28;
    USE(tmp28);
    tmp28 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Or(compiler::TNode<Uint32T>{tmp25}, compiler::TNode<Uint32T>{tmp27}));
    compiler::TNode<Uint32T> tmp29;
    USE(tmp29);
    tmp29 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(8));
    compiler::TNode<Uint32T> tmp30;
    USE(tmp30);
    tmp30 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Shl(compiler::TNode<Uint32T>{tmp20}, compiler::TNode<Uint32T>{tmp29}));
    compiler::TNode<Uint32T> tmp31;
    USE(tmp31);
    tmp31 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Or(compiler::TNode<Uint32T>{tmp28}, compiler::TNode<Uint32T>{tmp30}));
    compiler::TNode<Uint32T> tmp32;
    USE(tmp32);
    tmp32 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Or(compiler::TNode<Uint32T>{tmp31}, compiler::TNode<Uint32T>{tmp19}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 166);
    ca_.Goto(&block4, tmp15, tmp16, tmp17, tmp18, tmp19, tmp20, tmp21, tmp22, tmp32);
  }

  if (block3.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp33;
    compiler::TNode<UintPtrT> tmp34;
    compiler::TNode<BoolT> tmp35;
    compiler::TNode<RawPtrT> tmp36;
    compiler::TNode<Uint32T> tmp37;
    compiler::TNode<Uint32T> tmp38;
    compiler::TNode<Uint32T> tmp39;
    compiler::TNode<Uint32T> tmp40;
    compiler::TNode<Uint32T> tmp41;
    ca_.Bind(&block3, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 169);
    compiler::TNode<Uint32T> tmp42;
    USE(tmp42);
    tmp42 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(24));
    compiler::TNode<Uint32T> tmp43;
    USE(tmp43);
    tmp43 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Shl(compiler::TNode<Uint32T>{tmp37}, compiler::TNode<Uint32T>{tmp42}));
    compiler::TNode<Uint32T> tmp44;
    USE(tmp44);
    tmp44 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(16));
    compiler::TNode<Uint32T> tmp45;
    USE(tmp45);
    tmp45 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Shl(compiler::TNode<Uint32T>{tmp38}, compiler::TNode<Uint32T>{tmp44}));
    compiler::TNode<Uint32T> tmp46;
    USE(tmp46);
    tmp46 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Or(compiler::TNode<Uint32T>{tmp43}, compiler::TNode<Uint32T>{tmp45}));
    compiler::TNode<Uint32T> tmp47;
    USE(tmp47);
    tmp47 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(8));
    compiler::TNode<Uint32T> tmp48;
    USE(tmp48);
    tmp48 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Shl(compiler::TNode<Uint32T>{tmp39}, compiler::TNode<Uint32T>{tmp47}));
    compiler::TNode<Uint32T> tmp49;
    USE(tmp49);
    tmp49 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Or(compiler::TNode<Uint32T>{tmp46}, compiler::TNode<Uint32T>{tmp48}));
    compiler::TNode<Uint32T> tmp50;
    USE(tmp50);
    tmp50 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Or(compiler::TNode<Uint32T>{tmp49}, compiler::TNode<Uint32T>{tmp40}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 166);
    ca_.Goto(&block4, tmp33, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39, tmp40, tmp50);
  }

  if (block4.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp51;
    compiler::TNode<UintPtrT> tmp52;
    compiler::TNode<BoolT> tmp53;
    compiler::TNode<RawPtrT> tmp54;
    compiler::TNode<Uint32T> tmp55;
    compiler::TNode<Uint32T> tmp56;
    compiler::TNode<Uint32T> tmp57;
    compiler::TNode<Uint32T> tmp58;
    compiler::TNode<Uint32T> tmp59;
    ca_.Bind(&block4, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 172);
    if (((CodeStubAssembler(state_).ElementsKindEqual(p_kind, INT32_ELEMENTS)))) {
      ca_.Goto(&block5, tmp51, tmp52, tmp53, tmp54, tmp55, tmp56, tmp57, tmp58, tmp59);
    } else {
      ca_.Goto(&block6, tmp51, tmp52, tmp53, tmp54, tmp55, tmp56, tmp57, tmp58, tmp59);
    }
  }

  if (block5.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp60;
    compiler::TNode<UintPtrT> tmp61;
    compiler::TNode<BoolT> tmp62;
    compiler::TNode<RawPtrT> tmp63;
    compiler::TNode<Uint32T> tmp64;
    compiler::TNode<Uint32T> tmp65;
    compiler::TNode<Uint32T> tmp66;
    compiler::TNode<Uint32T> tmp67;
    compiler::TNode<Uint32T> tmp68;
    ca_.Bind(&block5, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65, &tmp66, &tmp67, &tmp68);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 173);
    compiler::TNode<Int32T> tmp69;
    USE(tmp69);
    tmp69 = ca_.UncheckedCast<Int32T>(CodeStubAssembler(state_).Signed(compiler::TNode<Uint32T>{tmp68}));
    compiler::TNode<Number> tmp70;
    USE(tmp70);
    tmp70 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).Convert20UT5ATSmi10HeapNumber7ATint32(compiler::TNode<Int32T>{tmp69}));
    ca_.Goto(&block1, tmp60, tmp61, tmp62, tmp70);
  }

  if (block6.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp71;
    compiler::TNode<UintPtrT> tmp72;
    compiler::TNode<BoolT> tmp73;
    compiler::TNode<RawPtrT> tmp74;
    compiler::TNode<Uint32T> tmp75;
    compiler::TNode<Uint32T> tmp76;
    compiler::TNode<Uint32T> tmp77;
    compiler::TNode<Uint32T> tmp78;
    compiler::TNode<Uint32T> tmp79;
    ca_.Bind(&block6, &tmp71, &tmp72, &tmp73, &tmp74, &tmp75, &tmp76, &tmp77, &tmp78, &tmp79);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 174);
    if (((CodeStubAssembler(state_).ElementsKindEqual(p_kind, UINT32_ELEMENTS)))) {
      ca_.Goto(&block8, tmp71, tmp72, tmp73, tmp74, tmp75, tmp76, tmp77, tmp78, tmp79);
    } else {
      ca_.Goto(&block9, tmp71, tmp72, tmp73, tmp74, tmp75, tmp76, tmp77, tmp78, tmp79);
    }
  }

  if (block8.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp80;
    compiler::TNode<UintPtrT> tmp81;
    compiler::TNode<BoolT> tmp82;
    compiler::TNode<RawPtrT> tmp83;
    compiler::TNode<Uint32T> tmp84;
    compiler::TNode<Uint32T> tmp85;
    compiler::TNode<Uint32T> tmp86;
    compiler::TNode<Uint32T> tmp87;
    compiler::TNode<Uint32T> tmp88;
    ca_.Bind(&block8, &tmp80, &tmp81, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 175);
    compiler::TNode<Number> tmp89;
    USE(tmp89);
    tmp89 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).Convert20UT5ATSmi10HeapNumber8ATuint32(compiler::TNode<Uint32T>{tmp88}));
    ca_.Goto(&block1, tmp80, tmp81, tmp82, tmp89);
  }

  if (block9.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp90;
    compiler::TNode<UintPtrT> tmp91;
    compiler::TNode<BoolT> tmp92;
    compiler::TNode<RawPtrT> tmp93;
    compiler::TNode<Uint32T> tmp94;
    compiler::TNode<Uint32T> tmp95;
    compiler::TNode<Uint32T> tmp96;
    compiler::TNode<Uint32T> tmp97;
    compiler::TNode<Uint32T> tmp98;
    ca_.Bind(&block9, &tmp90, &tmp91, &tmp92, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97, &tmp98);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 176);
    if (((CodeStubAssembler(state_).ElementsKindEqual(p_kind, FLOAT32_ELEMENTS)))) {
      ca_.Goto(&block11, tmp90, tmp91, tmp92, tmp93, tmp94, tmp95, tmp96, tmp97, tmp98);
    } else {
      ca_.Goto(&block12, tmp90, tmp91, tmp92, tmp93, tmp94, tmp95, tmp96, tmp97, tmp98);
    }
  }

  if (block11.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp99;
    compiler::TNode<UintPtrT> tmp100;
    compiler::TNode<BoolT> tmp101;
    compiler::TNode<RawPtrT> tmp102;
    compiler::TNode<Uint32T> tmp103;
    compiler::TNode<Uint32T> tmp104;
    compiler::TNode<Uint32T> tmp105;
    compiler::TNode<Uint32T> tmp106;
    compiler::TNode<Uint32T> tmp107;
    ca_.Bind(&block11, &tmp99, &tmp100, &tmp101, &tmp102, &tmp103, &tmp104, &tmp105, &tmp106, &tmp107);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 177);
    compiler::TNode<Float32T> tmp108;
    USE(tmp108);
    tmp108 = ca_.UncheckedCast<Float32T>(CodeStubAssembler(state_).BitcastInt32ToFloat32(compiler::TNode<Uint32T>{tmp107}));
    compiler::TNode<Float64T> tmp109;
    USE(tmp109);
    tmp109 = ca_.UncheckedCast<Float64T>(BaseBuiltinsFromDSLAssembler(state_).Convert9ATfloat649ATfloat32(compiler::TNode<Float32T>{tmp108}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 178);
    compiler::TNode<Number> tmp110;
    USE(tmp110);
    tmp110 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).Convert20UT5ATSmi10HeapNumber9ATfloat64(compiler::TNode<Float64T>{tmp109}));
    ca_.Goto(&block1, tmp99, tmp100, tmp101, tmp110);
  }

  if (block12.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp111;
    compiler::TNode<UintPtrT> tmp112;
    compiler::TNode<BoolT> tmp113;
    compiler::TNode<RawPtrT> tmp114;
    compiler::TNode<Uint32T> tmp115;
    compiler::TNode<Uint32T> tmp116;
    compiler::TNode<Uint32T> tmp117;
    compiler::TNode<Uint32T> tmp118;
    compiler::TNode<Uint32T> tmp119;
    ca_.Bind(&block12, &tmp111, &tmp112, &tmp113, &tmp114, &tmp115, &tmp116, &tmp117, &tmp118, &tmp119);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 180);
    CodeStubAssembler(state_).Unreachable();
  }

  if (block1.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp120;
    compiler::TNode<UintPtrT> tmp121;
    compiler::TNode<BoolT> tmp122;
    compiler::TNode<Number> tmp123;
    ca_.Bind(&block1, &tmp120, &tmp121, &tmp122, &tmp123);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 155);
    ca_.Goto(&block14, tmp120, tmp121, tmp122, tmp123);
  }

    compiler::TNode<JSArrayBuffer> tmp124;
    compiler::TNode<UintPtrT> tmp125;
    compiler::TNode<BoolT> tmp126;
    compiler::TNode<Number> tmp127;
    ca_.Bind(&block14, &tmp124, &tmp125, &tmp126, &tmp127);
  return compiler::TNode<Number>{tmp127};
}

compiler::TNode<Number> DataViewBuiltinsFromDSLAssembler::LoadDataViewFloat64(compiler::TNode<JSArrayBuffer> p_buffer, compiler::TNode<UintPtrT> p_offset, compiler::TNode<BoolT> p_requestedLittleEndian) {
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, BoolT> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, BoolT, RawPtrT, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, BoolT, RawPtrT, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, BoolT, RawPtrT, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, BoolT, Number> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, BoolT, Number> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_buffer, p_offset, p_requestedLittleEndian);

  if (block0.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp0;
    compiler::TNode<UintPtrT> tmp1;
    compiler::TNode<BoolT> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 187);
    compiler::TNode<IntPtrT> tmp3 = ca_.IntPtrConstant(JSArrayBuffer::kBackingStoreOffset);
    USE(tmp3);
    compiler::TNode<RawPtrT>tmp4 = CodeStubAssembler(state_).LoadReference<RawPtrT>(CodeStubAssembler::Reference{tmp0, tmp3});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 189);
    compiler::TNode<Uint32T> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<Uint32T>(DataViewBuiltinsAssembler(state_).LoadUint8(compiler::TNode<RawPtrT>{tmp4}, compiler::TNode<UintPtrT>{tmp1}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 190);
    compiler::TNode<UintPtrT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(1));
    compiler::TNode<UintPtrT> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrAdd(compiler::TNode<UintPtrT>{tmp1}, compiler::TNode<UintPtrT>{tmp6}));
    compiler::TNode<Uint32T> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<Uint32T>(DataViewBuiltinsAssembler(state_).LoadUint8(compiler::TNode<RawPtrT>{tmp4}, compiler::TNode<UintPtrT>{tmp7}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 191);
    compiler::TNode<UintPtrT> tmp9;
    USE(tmp9);
    tmp9 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(2));
    compiler::TNode<UintPtrT> tmp10;
    USE(tmp10);
    tmp10 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrAdd(compiler::TNode<UintPtrT>{tmp1}, compiler::TNode<UintPtrT>{tmp9}));
    compiler::TNode<Uint32T> tmp11;
    USE(tmp11);
    tmp11 = ca_.UncheckedCast<Uint32T>(DataViewBuiltinsAssembler(state_).LoadUint8(compiler::TNode<RawPtrT>{tmp4}, compiler::TNode<UintPtrT>{tmp10}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 192);
    compiler::TNode<UintPtrT> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(3));
    compiler::TNode<UintPtrT> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrAdd(compiler::TNode<UintPtrT>{tmp1}, compiler::TNode<UintPtrT>{tmp12}));
    compiler::TNode<Uint32T> tmp14;
    USE(tmp14);
    tmp14 = ca_.UncheckedCast<Uint32T>(DataViewBuiltinsAssembler(state_).LoadUint8(compiler::TNode<RawPtrT>{tmp4}, compiler::TNode<UintPtrT>{tmp13}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 193);
    compiler::TNode<UintPtrT> tmp15;
    USE(tmp15);
    tmp15 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(4));
    compiler::TNode<UintPtrT> tmp16;
    USE(tmp16);
    tmp16 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrAdd(compiler::TNode<UintPtrT>{tmp1}, compiler::TNode<UintPtrT>{tmp15}));
    compiler::TNode<Uint32T> tmp17;
    USE(tmp17);
    tmp17 = ca_.UncheckedCast<Uint32T>(DataViewBuiltinsAssembler(state_).LoadUint8(compiler::TNode<RawPtrT>{tmp4}, compiler::TNode<UintPtrT>{tmp16}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 194);
    compiler::TNode<UintPtrT> tmp18;
    USE(tmp18);
    tmp18 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(5));
    compiler::TNode<UintPtrT> tmp19;
    USE(tmp19);
    tmp19 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrAdd(compiler::TNode<UintPtrT>{tmp1}, compiler::TNode<UintPtrT>{tmp18}));
    compiler::TNode<Uint32T> tmp20;
    USE(tmp20);
    tmp20 = ca_.UncheckedCast<Uint32T>(DataViewBuiltinsAssembler(state_).LoadUint8(compiler::TNode<RawPtrT>{tmp4}, compiler::TNode<UintPtrT>{tmp19}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 195);
    compiler::TNode<UintPtrT> tmp21;
    USE(tmp21);
    tmp21 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(6));
    compiler::TNode<UintPtrT> tmp22;
    USE(tmp22);
    tmp22 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrAdd(compiler::TNode<UintPtrT>{tmp1}, compiler::TNode<UintPtrT>{tmp21}));
    compiler::TNode<Uint32T> tmp23;
    USE(tmp23);
    tmp23 = ca_.UncheckedCast<Uint32T>(DataViewBuiltinsAssembler(state_).LoadUint8(compiler::TNode<RawPtrT>{tmp4}, compiler::TNode<UintPtrT>{tmp22}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 196);
    compiler::TNode<UintPtrT> tmp24;
    USE(tmp24);
    tmp24 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(7));
    compiler::TNode<UintPtrT> tmp25;
    USE(tmp25);
    tmp25 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrAdd(compiler::TNode<UintPtrT>{tmp1}, compiler::TNode<UintPtrT>{tmp24}));
    compiler::TNode<Uint32T> tmp26;
    USE(tmp26);
    tmp26 = ca_.UncheckedCast<Uint32T>(DataViewBuiltinsAssembler(state_).LoadUint8(compiler::TNode<RawPtrT>{tmp4}, compiler::TNode<UintPtrT>{tmp25}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 197);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 198);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 200);
    ca_.Branch(tmp2, &block2, &block3, tmp0, tmp1, tmp2, tmp4, tmp5, tmp8, tmp11, tmp14, tmp17, tmp20, tmp23, tmp26, ca_.Uninitialized<Uint32T>(), ca_.Uninitialized<Uint32T>());
  }

  if (block2.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp27;
    compiler::TNode<UintPtrT> tmp28;
    compiler::TNode<BoolT> tmp29;
    compiler::TNode<RawPtrT> tmp30;
    compiler::TNode<Uint32T> tmp31;
    compiler::TNode<Uint32T> tmp32;
    compiler::TNode<Uint32T> tmp33;
    compiler::TNode<Uint32T> tmp34;
    compiler::TNode<Uint32T> tmp35;
    compiler::TNode<Uint32T> tmp36;
    compiler::TNode<Uint32T> tmp37;
    compiler::TNode<Uint32T> tmp38;
    compiler::TNode<Uint32T> tmp39;
    compiler::TNode<Uint32T> tmp40;
    ca_.Bind(&block2, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 201);
    compiler::TNode<Uint32T> tmp41;
    USE(tmp41);
    tmp41 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(24));
    compiler::TNode<Uint32T> tmp42;
    USE(tmp42);
    tmp42 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Shl(compiler::TNode<Uint32T>{tmp34}, compiler::TNode<Uint32T>{tmp41}));
    compiler::TNode<Uint32T> tmp43;
    USE(tmp43);
    tmp43 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(16));
    compiler::TNode<Uint32T> tmp44;
    USE(tmp44);
    tmp44 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Shl(compiler::TNode<Uint32T>{tmp33}, compiler::TNode<Uint32T>{tmp43}));
    compiler::TNode<Uint32T> tmp45;
    USE(tmp45);
    tmp45 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Or(compiler::TNode<Uint32T>{tmp42}, compiler::TNode<Uint32T>{tmp44}));
    compiler::TNode<Uint32T> tmp46;
    USE(tmp46);
    tmp46 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(8));
    compiler::TNode<Uint32T> tmp47;
    USE(tmp47);
    tmp47 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Shl(compiler::TNode<Uint32T>{tmp32}, compiler::TNode<Uint32T>{tmp46}));
    compiler::TNode<Uint32T> tmp48;
    USE(tmp48);
    tmp48 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Or(compiler::TNode<Uint32T>{tmp45}, compiler::TNode<Uint32T>{tmp47}));
    compiler::TNode<Uint32T> tmp49;
    USE(tmp49);
    tmp49 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Or(compiler::TNode<Uint32T>{tmp48}, compiler::TNode<Uint32T>{tmp31}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 202);
    compiler::TNode<Uint32T> tmp50;
    USE(tmp50);
    tmp50 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(24));
    compiler::TNode<Uint32T> tmp51;
    USE(tmp51);
    tmp51 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Shl(compiler::TNode<Uint32T>{tmp38}, compiler::TNode<Uint32T>{tmp50}));
    compiler::TNode<Uint32T> tmp52;
    USE(tmp52);
    tmp52 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(16));
    compiler::TNode<Uint32T> tmp53;
    USE(tmp53);
    tmp53 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Shl(compiler::TNode<Uint32T>{tmp37}, compiler::TNode<Uint32T>{tmp52}));
    compiler::TNode<Uint32T> tmp54;
    USE(tmp54);
    tmp54 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Or(compiler::TNode<Uint32T>{tmp51}, compiler::TNode<Uint32T>{tmp53}));
    compiler::TNode<Uint32T> tmp55;
    USE(tmp55);
    tmp55 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(8));
    compiler::TNode<Uint32T> tmp56;
    USE(tmp56);
    tmp56 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Shl(compiler::TNode<Uint32T>{tmp36}, compiler::TNode<Uint32T>{tmp55}));
    compiler::TNode<Uint32T> tmp57;
    USE(tmp57);
    tmp57 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Or(compiler::TNode<Uint32T>{tmp54}, compiler::TNode<Uint32T>{tmp56}));
    compiler::TNode<Uint32T> tmp58;
    USE(tmp58);
    tmp58 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Or(compiler::TNode<Uint32T>{tmp57}, compiler::TNode<Uint32T>{tmp35}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 200);
    ca_.Goto(&block4, tmp27, tmp28, tmp29, tmp30, tmp31, tmp32, tmp33, tmp34, tmp35, tmp36, tmp37, tmp38, tmp49, tmp58);
  }

  if (block3.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp59;
    compiler::TNode<UintPtrT> tmp60;
    compiler::TNode<BoolT> tmp61;
    compiler::TNode<RawPtrT> tmp62;
    compiler::TNode<Uint32T> tmp63;
    compiler::TNode<Uint32T> tmp64;
    compiler::TNode<Uint32T> tmp65;
    compiler::TNode<Uint32T> tmp66;
    compiler::TNode<Uint32T> tmp67;
    compiler::TNode<Uint32T> tmp68;
    compiler::TNode<Uint32T> tmp69;
    compiler::TNode<Uint32T> tmp70;
    compiler::TNode<Uint32T> tmp71;
    compiler::TNode<Uint32T> tmp72;
    ca_.Bind(&block3, &tmp59, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 204);
    compiler::TNode<Uint32T> tmp73;
    USE(tmp73);
    tmp73 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(24));
    compiler::TNode<Uint32T> tmp74;
    USE(tmp74);
    tmp74 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Shl(compiler::TNode<Uint32T>{tmp63}, compiler::TNode<Uint32T>{tmp73}));
    compiler::TNode<Uint32T> tmp75;
    USE(tmp75);
    tmp75 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(16));
    compiler::TNode<Uint32T> tmp76;
    USE(tmp76);
    tmp76 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Shl(compiler::TNode<Uint32T>{tmp64}, compiler::TNode<Uint32T>{tmp75}));
    compiler::TNode<Uint32T> tmp77;
    USE(tmp77);
    tmp77 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Or(compiler::TNode<Uint32T>{tmp74}, compiler::TNode<Uint32T>{tmp76}));
    compiler::TNode<Uint32T> tmp78;
    USE(tmp78);
    tmp78 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(8));
    compiler::TNode<Uint32T> tmp79;
    USE(tmp79);
    tmp79 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Shl(compiler::TNode<Uint32T>{tmp65}, compiler::TNode<Uint32T>{tmp78}));
    compiler::TNode<Uint32T> tmp80;
    USE(tmp80);
    tmp80 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Or(compiler::TNode<Uint32T>{tmp77}, compiler::TNode<Uint32T>{tmp79}));
    compiler::TNode<Uint32T> tmp81;
    USE(tmp81);
    tmp81 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Or(compiler::TNode<Uint32T>{tmp80}, compiler::TNode<Uint32T>{tmp66}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 205);
    compiler::TNode<Uint32T> tmp82;
    USE(tmp82);
    tmp82 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(24));
    compiler::TNode<Uint32T> tmp83;
    USE(tmp83);
    tmp83 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Shl(compiler::TNode<Uint32T>{tmp67}, compiler::TNode<Uint32T>{tmp82}));
    compiler::TNode<Uint32T> tmp84;
    USE(tmp84);
    tmp84 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(16));
    compiler::TNode<Uint32T> tmp85;
    USE(tmp85);
    tmp85 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Shl(compiler::TNode<Uint32T>{tmp68}, compiler::TNode<Uint32T>{tmp84}));
    compiler::TNode<Uint32T> tmp86;
    USE(tmp86);
    tmp86 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Or(compiler::TNode<Uint32T>{tmp83}, compiler::TNode<Uint32T>{tmp85}));
    compiler::TNode<Uint32T> tmp87;
    USE(tmp87);
    tmp87 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(8));
    compiler::TNode<Uint32T> tmp88;
    USE(tmp88);
    tmp88 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Shl(compiler::TNode<Uint32T>{tmp69}, compiler::TNode<Uint32T>{tmp87}));
    compiler::TNode<Uint32T> tmp89;
    USE(tmp89);
    tmp89 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Or(compiler::TNode<Uint32T>{tmp86}, compiler::TNode<Uint32T>{tmp88}));
    compiler::TNode<Uint32T> tmp90;
    USE(tmp90);
    tmp90 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Or(compiler::TNode<Uint32T>{tmp89}, compiler::TNode<Uint32T>{tmp70}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 200);
    ca_.Goto(&block4, tmp59, tmp60, tmp61, tmp62, tmp63, tmp64, tmp65, tmp66, tmp67, tmp68, tmp69, tmp70, tmp90, tmp81);
  }

  if (block4.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp91;
    compiler::TNode<UintPtrT> tmp92;
    compiler::TNode<BoolT> tmp93;
    compiler::TNode<RawPtrT> tmp94;
    compiler::TNode<Uint32T> tmp95;
    compiler::TNode<Uint32T> tmp96;
    compiler::TNode<Uint32T> tmp97;
    compiler::TNode<Uint32T> tmp98;
    compiler::TNode<Uint32T> tmp99;
    compiler::TNode<Uint32T> tmp100;
    compiler::TNode<Uint32T> tmp101;
    compiler::TNode<Uint32T> tmp102;
    compiler::TNode<Uint32T> tmp103;
    compiler::TNode<Uint32T> tmp104;
    ca_.Bind(&block4, &tmp91, &tmp92, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102, &tmp103, &tmp104);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 208);
    compiler::TNode<Float64T> tmp105;
    USE(tmp105);
    tmp105 = ca_.UncheckedCast<Float64T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATfloat6417ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 209);
    compiler::TNode<Float64T> tmp106;
    USE(tmp106);
    tmp106 = ca_.UncheckedCast<Float64T>(CodeStubAssembler(state_).Float64InsertLowWord32(compiler::TNode<Float64T>{tmp105}, compiler::TNode<Uint32T>{tmp103}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 210);
    compiler::TNode<Float64T> tmp107;
    USE(tmp107);
    tmp107 = ca_.UncheckedCast<Float64T>(CodeStubAssembler(state_).Float64InsertHighWord32(compiler::TNode<Float64T>{tmp106}, compiler::TNode<Uint32T>{tmp104}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 212);
    compiler::TNode<Number> tmp108;
    USE(tmp108);
    tmp108 = ca_.UncheckedCast<Number>(BaseBuiltinsFromDSLAssembler(state_).Convert20UT5ATSmi10HeapNumber9ATfloat64(compiler::TNode<Float64T>{tmp107}));
    ca_.Goto(&block1, tmp91, tmp92, tmp93, tmp108);
  }

  if (block1.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp109;
    compiler::TNode<UintPtrT> tmp110;
    compiler::TNode<BoolT> tmp111;
    compiler::TNode<Number> tmp112;
    ca_.Bind(&block1, &tmp109, &tmp110, &tmp111, &tmp112);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 184);
    ca_.Goto(&block5, tmp109, tmp110, tmp111, tmp112);
  }

    compiler::TNode<JSArrayBuffer> tmp113;
    compiler::TNode<UintPtrT> tmp114;
    compiler::TNode<BoolT> tmp115;
    compiler::TNode<Number> tmp116;
    ca_.Bind(&block5, &tmp113, &tmp114, &tmp115, &tmp116);
  return compiler::TNode<Number>{tmp116};
}

bool DataViewBuiltinsFromDSLAssembler::kPositiveBigInt() {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

    ca_.Bind(&block0);
return false;
}

bool DataViewBuiltinsFromDSLAssembler::kNegativeBigInt() {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

    ca_.Bind(&block0);
return true;
}

int31_t DataViewBuiltinsFromDSLAssembler::kZeroDigitBigInt() {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

    ca_.Bind(&block0);
return 0;
}

int31_t DataViewBuiltinsFromDSLAssembler::kOneDigitBigInt() {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

    ca_.Bind(&block0);
return 1;
}

int31_t DataViewBuiltinsFromDSLAssembler::kTwoDigitBigInt() {
  compiler::CodeAssemblerParameterizedLabel<> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0);

    ca_.Bind(&block0);
return 2;
}

compiler::TNode<BigInt> DataViewBuiltinsFromDSLAssembler::CreateEmptyBigInt(compiler::TNode<BoolT> p_isPositive, int31_t p_length) {
  compiler::CodeAssemblerParameterizedLabel<BoolT> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BigInt> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BigInt> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BigInt> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BigInt> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<BoolT, BigInt> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_isPositive);

  if (block0.is_used()) {
    compiler::TNode<BoolT> tmp0;
    ca_.Bind(&block0, &tmp0);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 229);
    compiler::TNode<IntPtrT> tmp1;
    USE(tmp1);
    tmp1 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(p_length));
    compiler::TNode<BigInt> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<BigInt>(CodeStubAssembler(state_).AllocateBigInt(compiler::TNode<IntPtrT>{tmp1}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 232);
    ca_.Branch(tmp0, &block2, &block3, tmp0, tmp2);
  }

  if (block2.is_used()) {
    compiler::TNode<BoolT> tmp3;
    compiler::TNode<BigInt> tmp4;
    ca_.Bind(&block2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 234);
    compiler::TNode<Uint32T> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<Uint32T>(DataViewBuiltinsAssembler(state_).DataViewEncodeBigIntBits(DataViewBuiltinsFromDSLAssembler(state_).kPositiveBigInt(), p_length));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 233);
    CodeStubAssembler(state_).StoreBigIntBitfield(compiler::TNode<BigInt>{tmp4}, compiler::TNode<Uint32T>{tmp5});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 232);
    ca_.Goto(&block4, tmp3, tmp4);
  }

  if (block3.is_used()) {
    compiler::TNode<BoolT> tmp6;
    compiler::TNode<BigInt> tmp7;
    ca_.Bind(&block3, &tmp6, &tmp7);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 237);
    compiler::TNode<Uint32T> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<Uint32T>(DataViewBuiltinsAssembler(state_).DataViewEncodeBigIntBits(DataViewBuiltinsFromDSLAssembler(state_).kNegativeBigInt(), p_length));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 236);
    CodeStubAssembler(state_).StoreBigIntBitfield(compiler::TNode<BigInt>{tmp7}, compiler::TNode<Uint32T>{tmp8});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 232);
    ca_.Goto(&block4, tmp6, tmp7);
  }

  if (block4.is_used()) {
    compiler::TNode<BoolT> tmp9;
    compiler::TNode<BigInt> tmp10;
    ca_.Bind(&block4, &tmp9, &tmp10);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 240);
    ca_.Goto(&block1, tmp9, tmp10);
  }

  if (block1.is_used()) {
    compiler::TNode<BoolT> tmp11;
    compiler::TNode<BigInt> tmp12;
    ca_.Bind(&block1, &tmp11, &tmp12);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 227);
    ca_.Goto(&block5, tmp11, tmp12);
  }

    compiler::TNode<BoolT> tmp13;
    compiler::TNode<BigInt> tmp14;
    ca_.Bind(&block5, &tmp13, &tmp14);
  return compiler::TNode<BigInt>{tmp14};
}

compiler::TNode<BigInt> DataViewBuiltinsFromDSLAssembler::MakeBigIntOn64Bit(compiler::TNode<Uint32T> p_lowWord, compiler::TNode<Uint32T> p_highWord, bool p_signed) {
  compiler::CodeAssemblerParameterizedLabel<Uint32T, Uint32T> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Uint32T, Uint32T> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Uint32T, Uint32T> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Uint32T, Uint32T> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Uint32T, Uint32T, BoolT, IntPtrT, IntPtrT, IntPtrT> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Uint32T, Uint32T, BoolT, IntPtrT, IntPtrT, IntPtrT> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Uint32T, Uint32T, BoolT, IntPtrT, IntPtrT, IntPtrT> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Uint32T, Uint32T, BoolT, IntPtrT, IntPtrT, IntPtrT> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Uint32T, Uint32T, BoolT, IntPtrT, IntPtrT, IntPtrT> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Uint32T, Uint32T, BigInt> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Uint32T, Uint32T, BigInt> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_lowWord, p_highWord);

  if (block0.is_used()) {
    compiler::TNode<Uint32T> tmp0;
    compiler::TNode<Uint32T> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 247);
    compiler::TNode<Uint32T> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32Equal(compiler::TNode<Uint32T>{tmp0}, compiler::TNode<Uint32T>{tmp2}));
    ca_.Branch(tmp3, &block4, &block3, tmp0, tmp1);
  }

  if (block4.is_used()) {
    compiler::TNode<Uint32T> tmp4;
    compiler::TNode<Uint32T> tmp5;
    ca_.Bind(&block4, &tmp4, &tmp5);
    compiler::TNode<Uint32T> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32Equal(compiler::TNode<Uint32T>{tmp5}, compiler::TNode<Uint32T>{tmp6}));
    ca_.Branch(tmp7, &block2, &block3, tmp4, tmp5);
  }

  if (block2.is_used()) {
    compiler::TNode<Uint32T> tmp8;
    compiler::TNode<Uint32T> tmp9;
    ca_.Bind(&block2, &tmp8, &tmp9);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 248);
    compiler::TNode<IntPtrT> tmp10;
    USE(tmp10);
    tmp10 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(DataViewBuiltinsFromDSLAssembler(state_).kZeroDigitBigInt()));
    compiler::TNode<BigInt> tmp11;
    USE(tmp11);
    tmp11 = ca_.UncheckedCast<BigInt>(CodeStubAssembler(state_).AllocateBigInt(compiler::TNode<IntPtrT>{tmp10}));
    ca_.Goto(&block1, tmp8, tmp9, tmp11);
  }

  if (block3.is_used()) {
    compiler::TNode<Uint32T> tmp12;
    compiler::TNode<Uint32T> tmp13;
    ca_.Bind(&block3, &tmp12, &tmp13);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 251);
    compiler::TNode<BoolT> tmp14;
    USE(tmp14);
    tmp14 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 252);
    compiler::TNode<UintPtrT> tmp15;
    USE(tmp15);
    tmp15 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert9ATuintptr8ATuint32(compiler::TNode<Uint32T>{tmp13}));
    compiler::TNode<IntPtrT> tmp16;
    USE(tmp16);
    tmp16 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).Signed(compiler::TNode<UintPtrT>{tmp15}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 253);
    compiler::TNode<UintPtrT> tmp17;
    USE(tmp17);
    tmp17 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert9ATuintptr8ATuint32(compiler::TNode<Uint32T>{tmp12}));
    compiler::TNode<IntPtrT> tmp18;
    USE(tmp18);
    tmp18 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).Signed(compiler::TNode<UintPtrT>{tmp17}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 254);
    compiler::TNode<IntPtrT> tmp19;
    USE(tmp19);
    tmp19 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(32));
    compiler::TNode<IntPtrT> tmp20;
    USE(tmp20);
    tmp20 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).WordShl(compiler::TNode<IntPtrT>{tmp16}, compiler::TNode<IntPtrT>{tmp19}));
    compiler::TNode<IntPtrT> tmp21;
    USE(tmp21);
    tmp21 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrAdd(compiler::TNode<IntPtrT>{tmp20}, compiler::TNode<IntPtrT>{tmp18}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 256);
    if ((p_signed)) {
      ca_.Goto(&block5, tmp12, tmp13, tmp14, tmp16, tmp18, tmp21);
    } else {
      ca_.Goto(&block6, tmp12, tmp13, tmp14, tmp16, tmp18, tmp21);
    }
  }

  if (block5.is_used()) {
    compiler::TNode<Uint32T> tmp22;
    compiler::TNode<Uint32T> tmp23;
    compiler::TNode<BoolT> tmp24;
    compiler::TNode<IntPtrT> tmp25;
    compiler::TNode<IntPtrT> tmp26;
    compiler::TNode<IntPtrT> tmp27;
    ca_.Bind(&block5, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 257);
    compiler::TNode<IntPtrT> tmp28;
    USE(tmp28);
    tmp28 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp29;
    USE(tmp29);
    tmp29 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrLessThan(compiler::TNode<IntPtrT>{tmp27}, compiler::TNode<IntPtrT>{tmp28}));
    ca_.Branch(tmp29, &block8, &block9, tmp22, tmp23, tmp24, tmp25, tmp26, tmp27);
  }

  if (block8.is_used()) {
    compiler::TNode<Uint32T> tmp30;
    compiler::TNode<Uint32T> tmp31;
    compiler::TNode<BoolT> tmp32;
    compiler::TNode<IntPtrT> tmp33;
    compiler::TNode<IntPtrT> tmp34;
    compiler::TNode<IntPtrT> tmp35;
    ca_.Bind(&block8, &tmp30, &tmp31, &tmp32, &tmp33, &tmp34, &tmp35);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 258);
    compiler::TNode<BoolT> tmp36;
    USE(tmp36);
    tmp36 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 260);
    compiler::TNode<IntPtrT> tmp37;
    USE(tmp37);
    tmp37 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<IntPtrT> tmp38;
    USE(tmp38);
    tmp38 = ca_.UncheckedCast<IntPtrT>(CodeStubAssembler(state_).IntPtrSub(compiler::TNode<IntPtrT>{tmp37}, compiler::TNode<IntPtrT>{tmp35}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 257);
    ca_.Goto(&block9, tmp30, tmp31, tmp36, tmp33, tmp34, tmp38);
  }

  if (block9.is_used()) {
    compiler::TNode<Uint32T> tmp39;
    compiler::TNode<Uint32T> tmp40;
    compiler::TNode<BoolT> tmp41;
    compiler::TNode<IntPtrT> tmp42;
    compiler::TNode<IntPtrT> tmp43;
    compiler::TNode<IntPtrT> tmp44;
    ca_.Bind(&block9, &tmp39, &tmp40, &tmp41, &tmp42, &tmp43, &tmp44);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 256);
    ca_.Goto(&block7, tmp39, tmp40, tmp41, tmp42, tmp43, tmp44);
  }

  if (block6.is_used()) {
    compiler::TNode<Uint32T> tmp45;
    compiler::TNode<Uint32T> tmp46;
    compiler::TNode<BoolT> tmp47;
    compiler::TNode<IntPtrT> tmp48;
    compiler::TNode<IntPtrT> tmp49;
    compiler::TNode<IntPtrT> tmp50;
    ca_.Bind(&block6, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50);
    ca_.Goto(&block7, tmp45, tmp46, tmp47, tmp48, tmp49, tmp50);
  }

  if (block7.is_used()) {
    compiler::TNode<Uint32T> tmp51;
    compiler::TNode<Uint32T> tmp52;
    compiler::TNode<BoolT> tmp53;
    compiler::TNode<IntPtrT> tmp54;
    compiler::TNode<IntPtrT> tmp55;
    compiler::TNode<IntPtrT> tmp56;
    ca_.Bind(&block7, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55, &tmp56);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 265);
    compiler::TNode<BigInt> tmp57;
    USE(tmp57);
    tmp57 = ca_.UncheckedCast<BigInt>(DataViewBuiltinsFromDSLAssembler(state_).CreateEmptyBigInt(compiler::TNode<BoolT>{tmp53}, DataViewBuiltinsFromDSLAssembler(state_).kOneDigitBigInt()));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 267);
    compiler::TNode<UintPtrT> tmp58;
    USE(tmp58);
    tmp58 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).Unsigned(compiler::TNode<IntPtrT>{tmp56}));
    CodeStubAssembler(state_).StoreBigIntDigit(compiler::TNode<BigInt>{tmp57}, 0, compiler::TNode<UintPtrT>{tmp58});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 269);
    ca_.Goto(&block1, tmp51, tmp52, tmp57);
  }

  if (block1.is_used()) {
    compiler::TNode<Uint32T> tmp59;
    compiler::TNode<Uint32T> tmp60;
    compiler::TNode<BigInt> tmp61;
    ca_.Bind(&block1, &tmp59, &tmp60, &tmp61);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 244);
    ca_.Goto(&block10, tmp59, tmp60, tmp61);
  }

    compiler::TNode<Uint32T> tmp62;
    compiler::TNode<Uint32T> tmp63;
    compiler::TNode<BigInt> tmp64;
    ca_.Bind(&block10, &tmp62, &tmp63, &tmp64);
  return compiler::TNode<BigInt>{tmp64};
}

compiler::TNode<BigInt> DataViewBuiltinsFromDSLAssembler::MakeBigIntOn32Bit(compiler::TNode<Uint32T> p_lowWord, compiler::TNode<Uint32T> p_highWord, bool p_signed) {
  compiler::CodeAssemblerParameterizedLabel<Uint32T, Uint32T> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Uint32T, Uint32T> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Uint32T, Uint32T> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Uint32T, Uint32T> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Uint32T, Uint32T, BoolT, BoolT, Int32T, Int32T> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Uint32T, Uint32T, BoolT, BoolT, Int32T, Int32T> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Uint32T, Uint32T, BoolT, BoolT, Int32T, Int32T> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Uint32T, Uint32T, BoolT, BoolT, Int32T, Int32T> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Uint32T, Uint32T, BoolT, BoolT, Int32T, Int32T> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Uint32T, Uint32T, BoolT, BoolT, Int32T, Int32T> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Uint32T, Uint32T, BoolT, BoolT, Int32T, Int32T> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Uint32T, Uint32T, BoolT, BoolT, Int32T, Int32T> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Uint32T, Uint32T, BoolT, BoolT, Int32T, Int32T> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Uint32T, Uint32T, BoolT, BoolT, Int32T, Int32T> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Uint32T, Uint32T, BoolT, BoolT, Int32T, Int32T> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Uint32T, Uint32T, BoolT, BoolT, Int32T, Int32T> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Uint32T, Uint32T, BoolT, BoolT, Int32T, Int32T, BigInt> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Uint32T, Uint32T, BoolT, BoolT, Int32T, Int32T, BigInt> block18(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Uint32T, Uint32T, BoolT, BoolT, Int32T, Int32T, BigInt> block19(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Uint32T, Uint32T, BoolT, BoolT, Int32T, Int32T, BigInt> block20(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Uint32T, Uint32T, BoolT, BoolT, Int32T, Int32T, BigInt> block21(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Uint32T, Uint32T, BigInt> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Uint32T, Uint32T, BigInt> block22(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_lowWord, p_highWord);

  if (block0.is_used()) {
    compiler::TNode<Uint32T> tmp0;
    compiler::TNode<Uint32T> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 276);
    compiler::TNode<Uint32T> tmp2;
    USE(tmp2);
    tmp2 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp3;
    USE(tmp3);
    tmp3 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32Equal(compiler::TNode<Uint32T>{tmp0}, compiler::TNode<Uint32T>{tmp2}));
    ca_.Branch(tmp3, &block4, &block3, tmp0, tmp1);
  }

  if (block4.is_used()) {
    compiler::TNode<Uint32T> tmp4;
    compiler::TNode<Uint32T> tmp5;
    ca_.Bind(&block4, &tmp4, &tmp5);
    compiler::TNode<Uint32T> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32Equal(compiler::TNode<Uint32T>{tmp5}, compiler::TNode<Uint32T>{tmp6}));
    ca_.Branch(tmp7, &block2, &block3, tmp4, tmp5);
  }

  if (block2.is_used()) {
    compiler::TNode<Uint32T> tmp8;
    compiler::TNode<Uint32T> tmp9;
    ca_.Bind(&block2, &tmp8, &tmp9);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 277);
    compiler::TNode<IntPtrT> tmp10;
    USE(tmp10);
    tmp10 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(DataViewBuiltinsFromDSLAssembler(state_).kZeroDigitBigInt()));
    compiler::TNode<BigInt> tmp11;
    USE(tmp11);
    tmp11 = ca_.UncheckedCast<BigInt>(CodeStubAssembler(state_).AllocateBigInt(compiler::TNode<IntPtrT>{tmp10}));
    ca_.Goto(&block1, tmp8, tmp9, tmp11);
  }

  if (block3.is_used()) {
    compiler::TNode<Uint32T> tmp12;
    compiler::TNode<Uint32T> tmp13;
    ca_.Bind(&block3, &tmp12, &tmp13);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 281);
    compiler::TNode<BoolT> tmp14;
    USE(tmp14);
    tmp14 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 282);
    compiler::TNode<BoolT> tmp15;
    USE(tmp15);
    tmp15 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 286);
    compiler::TNode<Int32T> tmp16;
    USE(tmp16);
    tmp16 = ca_.UncheckedCast<Int32T>(CodeStubAssembler(state_).Signed(compiler::TNode<Uint32T>{tmp12}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 287);
    compiler::TNode<Int32T> tmp17;
    USE(tmp17);
    tmp17 = ca_.UncheckedCast<Int32T>(CodeStubAssembler(state_).Signed(compiler::TNode<Uint32T>{tmp13}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 292);
    compiler::TNode<Uint32T> tmp18;
    USE(tmp18);
    tmp18 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp19;
    USE(tmp19);
    tmp19 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32NotEqual(compiler::TNode<Uint32T>{tmp13}, compiler::TNode<Uint32T>{tmp18}));
    ca_.Branch(tmp19, &block5, &block6, tmp12, tmp13, tmp14, tmp15, tmp16, tmp17);
  }

  if (block5.is_used()) {
    compiler::TNode<Uint32T> tmp20;
    compiler::TNode<Uint32T> tmp21;
    compiler::TNode<BoolT> tmp22;
    compiler::TNode<BoolT> tmp23;
    compiler::TNode<Int32T> tmp24;
    compiler::TNode<Int32T> tmp25;
    ca_.Bind(&block5, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 293);
    if ((p_signed)) {
      ca_.Goto(&block7, tmp20, tmp21, tmp22, tmp23, tmp24, tmp25);
    } else {
      ca_.Goto(&block8, tmp20, tmp21, tmp22, tmp23, tmp24, tmp25);
    }
  }

  if (block7.is_used()) {
    compiler::TNode<Uint32T> tmp26;
    compiler::TNode<Uint32T> tmp27;
    compiler::TNode<BoolT> tmp28;
    compiler::TNode<BoolT> tmp29;
    compiler::TNode<Int32T> tmp30;
    compiler::TNode<Int32T> tmp31;
    ca_.Bind(&block7, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 295);
    compiler::TNode<Int32T> tmp32;
    USE(tmp32);
    tmp32 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr7ATint3217ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp33;
    USE(tmp33);
    tmp33 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Int32LessThan(compiler::TNode<Int32T>{tmp31}, compiler::TNode<Int32T>{tmp32}));
    ca_.Branch(tmp33, &block10, &block11, tmp26, tmp27, tmp28, tmp29, tmp30, tmp31);
  }

  if (block10.is_used()) {
    compiler::TNode<Uint32T> tmp34;
    compiler::TNode<Uint32T> tmp35;
    compiler::TNode<BoolT> tmp36;
    compiler::TNode<BoolT> tmp37;
    compiler::TNode<Int32T> tmp38;
    compiler::TNode<Int32T> tmp39;
    ca_.Bind(&block10, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 296);
    compiler::TNode<BoolT> tmp40;
    USE(tmp40);
    tmp40 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(false));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 301);
    compiler::TNode<Int32T> tmp41;
    USE(tmp41);
    tmp41 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr7ATint3217ATconstexpr_int31(0));
    compiler::TNode<Int32T> tmp42;
    USE(tmp42);
    tmp42 = ca_.UncheckedCast<Int32T>(CodeStubAssembler(state_).Int32Sub(compiler::TNode<Int32T>{tmp41}, compiler::TNode<Int32T>{tmp39}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 302);
    compiler::TNode<Int32T> tmp43;
    USE(tmp43);
    tmp43 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr7ATint3217ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp44;
    USE(tmp44);
    tmp44 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32NotEqual(compiler::TNode<Int32T>{tmp38}, compiler::TNode<Int32T>{tmp43}));
    ca_.Branch(tmp44, &block13, &block14, tmp34, tmp35, tmp36, tmp40, tmp38, tmp42);
  }

  if (block13.is_used()) {
    compiler::TNode<Uint32T> tmp45;
    compiler::TNode<Uint32T> tmp46;
    compiler::TNode<BoolT> tmp47;
    compiler::TNode<BoolT> tmp48;
    compiler::TNode<Int32T> tmp49;
    compiler::TNode<Int32T> tmp50;
    ca_.Bind(&block13, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 303);
    compiler::TNode<Int32T> tmp51;
    USE(tmp51);
    tmp51 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr7ATint3217ATconstexpr_int31(1));
    compiler::TNode<Int32T> tmp52;
    USE(tmp52);
    tmp52 = ca_.UncheckedCast<Int32T>(CodeStubAssembler(state_).Int32Sub(compiler::TNode<Int32T>{tmp50}, compiler::TNode<Int32T>{tmp51}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 302);
    ca_.Goto(&block14, tmp45, tmp46, tmp47, tmp48, tmp49, tmp52);
  }

  if (block14.is_used()) {
    compiler::TNode<Uint32T> tmp53;
    compiler::TNode<Uint32T> tmp54;
    compiler::TNode<BoolT> tmp55;
    compiler::TNode<BoolT> tmp56;
    compiler::TNode<Int32T> tmp57;
    compiler::TNode<Int32T> tmp58;
    ca_.Bind(&block14, &tmp53, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 305);
    compiler::TNode<Int32T> tmp59;
    USE(tmp59);
    tmp59 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr7ATint3217ATconstexpr_int31(0));
    compiler::TNode<Int32T> tmp60;
    USE(tmp60);
    tmp60 = ca_.UncheckedCast<Int32T>(CodeStubAssembler(state_).Int32Sub(compiler::TNode<Int32T>{tmp59}, compiler::TNode<Int32T>{tmp57}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 308);
    compiler::TNode<Int32T> tmp61;
    USE(tmp61);
    tmp61 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr7ATint3217ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp62;
    USE(tmp62);
    tmp62 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32NotEqual(compiler::TNode<Int32T>{tmp58}, compiler::TNode<Int32T>{tmp61}));
    ca_.Branch(tmp62, &block15, &block16, tmp53, tmp54, tmp55, tmp56, tmp60, tmp58);
  }

  if (block15.is_used()) {
    compiler::TNode<Uint32T> tmp63;
    compiler::TNode<Uint32T> tmp64;
    compiler::TNode<BoolT> tmp65;
    compiler::TNode<BoolT> tmp66;
    compiler::TNode<Int32T> tmp67;
    compiler::TNode<Int32T> tmp68;
    ca_.Bind(&block15, &tmp63, &tmp64, &tmp65, &tmp66, &tmp67, &tmp68);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 309);
    compiler::TNode<BoolT> tmp69;
    USE(tmp69);
    tmp69 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 308);
    ca_.Goto(&block16, tmp63, tmp64, tmp69, tmp66, tmp67, tmp68);
  }

  if (block16.is_used()) {
    compiler::TNode<Uint32T> tmp70;
    compiler::TNode<Uint32T> tmp71;
    compiler::TNode<BoolT> tmp72;
    compiler::TNode<BoolT> tmp73;
    compiler::TNode<Int32T> tmp74;
    compiler::TNode<Int32T> tmp75;
    ca_.Bind(&block16, &tmp70, &tmp71, &tmp72, &tmp73, &tmp74, &tmp75);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 295);
    ca_.Goto(&block12, tmp70, tmp71, tmp72, tmp73, tmp74, tmp75);
  }

  if (block11.is_used()) {
    compiler::TNode<Uint32T> tmp76;
    compiler::TNode<Uint32T> tmp77;
    compiler::TNode<BoolT> tmp78;
    compiler::TNode<BoolT> tmp79;
    compiler::TNode<Int32T> tmp80;
    compiler::TNode<Int32T> tmp81;
    ca_.Bind(&block11, &tmp76, &tmp77, &tmp78, &tmp79, &tmp80, &tmp81);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 314);
    compiler::TNode<BoolT> tmp82;
    USE(tmp82);
    tmp82 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 295);
    ca_.Goto(&block12, tmp76, tmp77, tmp82, tmp79, tmp80, tmp81);
  }

  if (block12.is_used()) {
    compiler::TNode<Uint32T> tmp83;
    compiler::TNode<Uint32T> tmp84;
    compiler::TNode<BoolT> tmp85;
    compiler::TNode<BoolT> tmp86;
    compiler::TNode<Int32T> tmp87;
    compiler::TNode<Int32T> tmp88;
    ca_.Bind(&block12, &tmp83, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 293);
    ca_.Goto(&block9, tmp83, tmp84, tmp85, tmp86, tmp87, tmp88);
  }

  if (block8.is_used()) {
    compiler::TNode<Uint32T> tmp89;
    compiler::TNode<Uint32T> tmp90;
    compiler::TNode<BoolT> tmp91;
    compiler::TNode<BoolT> tmp92;
    compiler::TNode<Int32T> tmp93;
    compiler::TNode<Int32T> tmp94;
    ca_.Bind(&block8, &tmp89, &tmp90, &tmp91, &tmp92, &tmp93, &tmp94);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 320);
    compiler::TNode<BoolT> tmp95;
    USE(tmp95);
    tmp95 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr6ATbool16ATconstexpr_bool(true));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 293);
    ca_.Goto(&block9, tmp89, tmp90, tmp95, tmp92, tmp93, tmp94);
  }

  if (block9.is_used()) {
    compiler::TNode<Uint32T> tmp96;
    compiler::TNode<Uint32T> tmp97;
    compiler::TNode<BoolT> tmp98;
    compiler::TNode<BoolT> tmp99;
    compiler::TNode<Int32T> tmp100;
    compiler::TNode<Int32T> tmp101;
    ca_.Bind(&block9, &tmp96, &tmp97, &tmp98, &tmp99, &tmp100, &tmp101);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 292);
    ca_.Goto(&block6, tmp96, tmp97, tmp98, tmp99, tmp100, tmp101);
  }

  if (block6.is_used()) {
    compiler::TNode<Uint32T> tmp102;
    compiler::TNode<Uint32T> tmp103;
    compiler::TNode<BoolT> tmp104;
    compiler::TNode<BoolT> tmp105;
    compiler::TNode<Int32T> tmp106;
    compiler::TNode<Int32T> tmp107;
    ca_.Bind(&block6, &tmp102, &tmp103, &tmp104, &tmp105, &tmp106, &tmp107);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 325);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 326);
    ca_.Branch(tmp104, &block17, &block18, tmp102, tmp103, tmp104, tmp105, tmp106, tmp107, ca_.Uninitialized<BigInt>());
  }

  if (block17.is_used()) {
    compiler::TNode<Uint32T> tmp108;
    compiler::TNode<Uint32T> tmp109;
    compiler::TNode<BoolT> tmp110;
    compiler::TNode<BoolT> tmp111;
    compiler::TNode<Int32T> tmp112;
    compiler::TNode<Int32T> tmp113;
    compiler::TNode<BigInt> tmp114;
    ca_.Bind(&block17, &tmp108, &tmp109, &tmp110, &tmp111, &tmp112, &tmp113, &tmp114);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 327);
    compiler::TNode<BigInt> tmp115;
    USE(tmp115);
    tmp115 = ca_.UncheckedCast<BigInt>(DataViewBuiltinsFromDSLAssembler(state_).CreateEmptyBigInt(compiler::TNode<BoolT>{tmp111}, DataViewBuiltinsFromDSLAssembler(state_).kTwoDigitBigInt()));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 326);
    ca_.Goto(&block19, tmp108, tmp109, tmp110, tmp111, tmp112, tmp113, tmp115);
  }

  if (block18.is_used()) {
    compiler::TNode<Uint32T> tmp116;
    compiler::TNode<Uint32T> tmp117;
    compiler::TNode<BoolT> tmp118;
    compiler::TNode<BoolT> tmp119;
    compiler::TNode<Int32T> tmp120;
    compiler::TNode<Int32T> tmp121;
    compiler::TNode<BigInt> tmp122;
    ca_.Bind(&block18, &tmp116, &tmp117, &tmp118, &tmp119, &tmp120, &tmp121, &tmp122);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 329);
    compiler::TNode<BigInt> tmp123;
    USE(tmp123);
    tmp123 = ca_.UncheckedCast<BigInt>(DataViewBuiltinsFromDSLAssembler(state_).CreateEmptyBigInt(compiler::TNode<BoolT>{tmp119}, DataViewBuiltinsFromDSLAssembler(state_).kOneDigitBigInt()));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 326);
    ca_.Goto(&block19, tmp116, tmp117, tmp118, tmp119, tmp120, tmp121, tmp123);
  }

  if (block19.is_used()) {
    compiler::TNode<Uint32T> tmp124;
    compiler::TNode<Uint32T> tmp125;
    compiler::TNode<BoolT> tmp126;
    compiler::TNode<BoolT> tmp127;
    compiler::TNode<Int32T> tmp128;
    compiler::TNode<Int32T> tmp129;
    compiler::TNode<BigInt> tmp130;
    ca_.Bind(&block19, &tmp124, &tmp125, &tmp126, &tmp127, &tmp128, &tmp129, &tmp130);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 333);
    compiler::TNode<IntPtrT> tmp131;
    USE(tmp131);
    tmp131 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr7ATint32(compiler::TNode<Int32T>{tmp128}));
    compiler::TNode<UintPtrT> tmp132;
    USE(tmp132);
    tmp132 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).Unsigned(compiler::TNode<IntPtrT>{tmp131}));
    CodeStubAssembler(state_).StoreBigIntDigit(compiler::TNode<BigInt>{tmp130}, 0, compiler::TNode<UintPtrT>{tmp132});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 335);
    ca_.Branch(tmp126, &block20, &block21, tmp124, tmp125, tmp126, tmp127, tmp128, tmp129, tmp130);
  }

  if (block20.is_used()) {
    compiler::TNode<Uint32T> tmp133;
    compiler::TNode<Uint32T> tmp134;
    compiler::TNode<BoolT> tmp135;
    compiler::TNode<BoolT> tmp136;
    compiler::TNode<Int32T> tmp137;
    compiler::TNode<Int32T> tmp138;
    compiler::TNode<BigInt> tmp139;
    ca_.Bind(&block20, &tmp133, &tmp134, &tmp135, &tmp136, &tmp137, &tmp138, &tmp139);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 336);
    compiler::TNode<IntPtrT> tmp140;
    USE(tmp140);
    tmp140 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATintptr7ATint32(compiler::TNode<Int32T>{tmp138}));
    compiler::TNode<UintPtrT> tmp141;
    USE(tmp141);
    tmp141 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).Unsigned(compiler::TNode<IntPtrT>{tmp140}));
    CodeStubAssembler(state_).StoreBigIntDigit(compiler::TNode<BigInt>{tmp139}, 1, compiler::TNode<UintPtrT>{tmp141});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 335);
    ca_.Goto(&block21, tmp133, tmp134, tmp135, tmp136, tmp137, tmp138, tmp139);
  }

  if (block21.is_used()) {
    compiler::TNode<Uint32T> tmp142;
    compiler::TNode<Uint32T> tmp143;
    compiler::TNode<BoolT> tmp144;
    compiler::TNode<BoolT> tmp145;
    compiler::TNode<Int32T> tmp146;
    compiler::TNode<Int32T> tmp147;
    compiler::TNode<BigInt> tmp148;
    ca_.Bind(&block21, &tmp142, &tmp143, &tmp144, &tmp145, &tmp146, &tmp147, &tmp148);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 339);
    ca_.Goto(&block1, tmp142, tmp143, tmp148);
  }

  if (block1.is_used()) {
    compiler::TNode<Uint32T> tmp149;
    compiler::TNode<Uint32T> tmp150;
    compiler::TNode<BigInt> tmp151;
    ca_.Bind(&block1, &tmp149, &tmp150, &tmp151);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 273);
    ca_.Goto(&block22, tmp149, tmp150, tmp151);
  }

    compiler::TNode<Uint32T> tmp152;
    compiler::TNode<Uint32T> tmp153;
    compiler::TNode<BigInt> tmp154;
    ca_.Bind(&block22, &tmp152, &tmp153, &tmp154);
  return compiler::TNode<BigInt>{tmp154};
}

compiler::TNode<BigInt> DataViewBuiltinsFromDSLAssembler::MakeBigInt(compiler::TNode<Uint32T> p_lowWord, compiler::TNode<Uint32T> p_highWord, bool p_signed) {
  compiler::CodeAssemblerParameterizedLabel<Uint32T, Uint32T> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Uint32T, Uint32T> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Uint32T, Uint32T> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Uint32T, Uint32T, BigInt> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Uint32T, Uint32T, BigInt> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_lowWord, p_highWord);

  if (block0.is_used()) {
    compiler::TNode<Uint32T> tmp0;
    compiler::TNode<Uint32T> tmp1;
    ca_.Bind(&block0, &tmp0, &tmp1);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 346);
    if (((CodeStubAssembler(state_).Is64()))) {
      ca_.Goto(&block2, tmp0, tmp1);
    } else {
      ca_.Goto(&block3, tmp0, tmp1);
    }
  }

  if (block2.is_used()) {
    compiler::TNode<Uint32T> tmp2;
    compiler::TNode<Uint32T> tmp3;
    ca_.Bind(&block2, &tmp2, &tmp3);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 347);
    compiler::TNode<BigInt> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<BigInt>(DataViewBuiltinsFromDSLAssembler(state_).MakeBigIntOn64Bit(compiler::TNode<Uint32T>{tmp2}, compiler::TNode<Uint32T>{tmp3}, p_signed));
    ca_.Goto(&block1, tmp2, tmp3, tmp4);
  }

  if (block3.is_used()) {
    compiler::TNode<Uint32T> tmp5;
    compiler::TNode<Uint32T> tmp6;
    ca_.Bind(&block3, &tmp5, &tmp6);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 349);
    compiler::TNode<BigInt> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<BigInt>(DataViewBuiltinsFromDSLAssembler(state_).MakeBigIntOn32Bit(compiler::TNode<Uint32T>{tmp5}, compiler::TNode<Uint32T>{tmp6}, p_signed));
    ca_.Goto(&block1, tmp5, tmp6, tmp7);
  }

  if (block1.is_used()) {
    compiler::TNode<Uint32T> tmp8;
    compiler::TNode<Uint32T> tmp9;
    compiler::TNode<BigInt> tmp10;
    ca_.Bind(&block1, &tmp8, &tmp9, &tmp10);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 342);
    ca_.Goto(&block5, tmp8, tmp9, tmp10);
  }

    compiler::TNode<Uint32T> tmp11;
    compiler::TNode<Uint32T> tmp12;
    compiler::TNode<BigInt> tmp13;
    ca_.Bind(&block5, &tmp11, &tmp12, &tmp13);
  return compiler::TNode<BigInt>{tmp13};
}

compiler::TNode<BigInt> DataViewBuiltinsFromDSLAssembler::LoadDataViewBigInt(compiler::TNode<JSArrayBuffer> p_buffer, compiler::TNode<UintPtrT> p_offset, compiler::TNode<BoolT> p_requestedLittleEndian, bool p_signed) {
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, BoolT> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, BoolT, RawPtrT, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, BoolT, RawPtrT, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, BoolT, RawPtrT, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, BoolT, BigInt> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, BoolT, BigInt> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_buffer, p_offset, p_requestedLittleEndian);

  if (block0.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp0;
    compiler::TNode<UintPtrT> tmp1;
    compiler::TNode<BoolT> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 356);
    compiler::TNode<IntPtrT> tmp3 = ca_.IntPtrConstant(JSArrayBuffer::kBackingStoreOffset);
    USE(tmp3);
    compiler::TNode<RawPtrT>tmp4 = CodeStubAssembler(state_).LoadReference<RawPtrT>(CodeStubAssembler::Reference{tmp0, tmp3});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 358);
    compiler::TNode<Uint32T> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<Uint32T>(DataViewBuiltinsAssembler(state_).LoadUint8(compiler::TNode<RawPtrT>{tmp4}, compiler::TNode<UintPtrT>{tmp1}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 359);
    compiler::TNode<UintPtrT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(1));
    compiler::TNode<UintPtrT> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrAdd(compiler::TNode<UintPtrT>{tmp1}, compiler::TNode<UintPtrT>{tmp6}));
    compiler::TNode<Uint32T> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<Uint32T>(DataViewBuiltinsAssembler(state_).LoadUint8(compiler::TNode<RawPtrT>{tmp4}, compiler::TNode<UintPtrT>{tmp7}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 360);
    compiler::TNode<UintPtrT> tmp9;
    USE(tmp9);
    tmp9 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(2));
    compiler::TNode<UintPtrT> tmp10;
    USE(tmp10);
    tmp10 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrAdd(compiler::TNode<UintPtrT>{tmp1}, compiler::TNode<UintPtrT>{tmp9}));
    compiler::TNode<Uint32T> tmp11;
    USE(tmp11);
    tmp11 = ca_.UncheckedCast<Uint32T>(DataViewBuiltinsAssembler(state_).LoadUint8(compiler::TNode<RawPtrT>{tmp4}, compiler::TNode<UintPtrT>{tmp10}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 361);
    compiler::TNode<UintPtrT> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(3));
    compiler::TNode<UintPtrT> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrAdd(compiler::TNode<UintPtrT>{tmp1}, compiler::TNode<UintPtrT>{tmp12}));
    compiler::TNode<Uint32T> tmp14;
    USE(tmp14);
    tmp14 = ca_.UncheckedCast<Uint32T>(DataViewBuiltinsAssembler(state_).LoadUint8(compiler::TNode<RawPtrT>{tmp4}, compiler::TNode<UintPtrT>{tmp13}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 362);
    compiler::TNode<UintPtrT> tmp15;
    USE(tmp15);
    tmp15 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(4));
    compiler::TNode<UintPtrT> tmp16;
    USE(tmp16);
    tmp16 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrAdd(compiler::TNode<UintPtrT>{tmp1}, compiler::TNode<UintPtrT>{tmp15}));
    compiler::TNode<Uint32T> tmp17;
    USE(tmp17);
    tmp17 = ca_.UncheckedCast<Uint32T>(DataViewBuiltinsAssembler(state_).LoadUint8(compiler::TNode<RawPtrT>{tmp4}, compiler::TNode<UintPtrT>{tmp16}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 363);
    compiler::TNode<UintPtrT> tmp18;
    USE(tmp18);
    tmp18 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(5));
    compiler::TNode<UintPtrT> tmp19;
    USE(tmp19);
    tmp19 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrAdd(compiler::TNode<UintPtrT>{tmp1}, compiler::TNode<UintPtrT>{tmp18}));
    compiler::TNode<Uint32T> tmp20;
    USE(tmp20);
    tmp20 = ca_.UncheckedCast<Uint32T>(DataViewBuiltinsAssembler(state_).LoadUint8(compiler::TNode<RawPtrT>{tmp4}, compiler::TNode<UintPtrT>{tmp19}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 364);
    compiler::TNode<UintPtrT> tmp21;
    USE(tmp21);
    tmp21 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(6));
    compiler::TNode<UintPtrT> tmp22;
    USE(tmp22);
    tmp22 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrAdd(compiler::TNode<UintPtrT>{tmp1}, compiler::TNode<UintPtrT>{tmp21}));
    compiler::TNode<Uint32T> tmp23;
    USE(tmp23);
    tmp23 = ca_.UncheckedCast<Uint32T>(DataViewBuiltinsAssembler(state_).LoadUint8(compiler::TNode<RawPtrT>{tmp4}, compiler::TNode<UintPtrT>{tmp22}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 365);
    compiler::TNode<UintPtrT> tmp24;
    USE(tmp24);
    tmp24 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(7));
    compiler::TNode<UintPtrT> tmp25;
    USE(tmp25);
    tmp25 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrAdd(compiler::TNode<UintPtrT>{tmp1}, compiler::TNode<UintPtrT>{tmp24}));
    compiler::TNode<Uint32T> tmp26;
    USE(tmp26);
    tmp26 = ca_.UncheckedCast<Uint32T>(DataViewBuiltinsAssembler(state_).LoadUint8(compiler::TNode<RawPtrT>{tmp4}, compiler::TNode<UintPtrT>{tmp25}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 366);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 367);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 369);
    ca_.Branch(tmp2, &block2, &block3, tmp0, tmp1, tmp2, tmp4, tmp5, tmp8, tmp11, tmp14, tmp17, tmp20, tmp23, tmp26, ca_.Uninitialized<Uint32T>(), ca_.Uninitialized<Uint32T>());
  }

  if (block2.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp27;
    compiler::TNode<UintPtrT> tmp28;
    compiler::TNode<BoolT> tmp29;
    compiler::TNode<RawPtrT> tmp30;
    compiler::TNode<Uint32T> tmp31;
    compiler::TNode<Uint32T> tmp32;
    compiler::TNode<Uint32T> tmp33;
    compiler::TNode<Uint32T> tmp34;
    compiler::TNode<Uint32T> tmp35;
    compiler::TNode<Uint32T> tmp36;
    compiler::TNode<Uint32T> tmp37;
    compiler::TNode<Uint32T> tmp38;
    compiler::TNode<Uint32T> tmp39;
    compiler::TNode<Uint32T> tmp40;
    ca_.Bind(&block2, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 370);
    compiler::TNode<Uint32T> tmp41;
    USE(tmp41);
    tmp41 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(24));
    compiler::TNode<Uint32T> tmp42;
    USE(tmp42);
    tmp42 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Shl(compiler::TNode<Uint32T>{tmp34}, compiler::TNode<Uint32T>{tmp41}));
    compiler::TNode<Uint32T> tmp43;
    USE(tmp43);
    tmp43 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(16));
    compiler::TNode<Uint32T> tmp44;
    USE(tmp44);
    tmp44 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Shl(compiler::TNode<Uint32T>{tmp33}, compiler::TNode<Uint32T>{tmp43}));
    compiler::TNode<Uint32T> tmp45;
    USE(tmp45);
    tmp45 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Or(compiler::TNode<Uint32T>{tmp42}, compiler::TNode<Uint32T>{tmp44}));
    compiler::TNode<Uint32T> tmp46;
    USE(tmp46);
    tmp46 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(8));
    compiler::TNode<Uint32T> tmp47;
    USE(tmp47);
    tmp47 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Shl(compiler::TNode<Uint32T>{tmp32}, compiler::TNode<Uint32T>{tmp46}));
    compiler::TNode<Uint32T> tmp48;
    USE(tmp48);
    tmp48 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Or(compiler::TNode<Uint32T>{tmp45}, compiler::TNode<Uint32T>{tmp47}));
    compiler::TNode<Uint32T> tmp49;
    USE(tmp49);
    tmp49 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Or(compiler::TNode<Uint32T>{tmp48}, compiler::TNode<Uint32T>{tmp31}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 371);
    compiler::TNode<Uint32T> tmp50;
    USE(tmp50);
    tmp50 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(24));
    compiler::TNode<Uint32T> tmp51;
    USE(tmp51);
    tmp51 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Shl(compiler::TNode<Uint32T>{tmp38}, compiler::TNode<Uint32T>{tmp50}));
    compiler::TNode<Uint32T> tmp52;
    USE(tmp52);
    tmp52 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(16));
    compiler::TNode<Uint32T> tmp53;
    USE(tmp53);
    tmp53 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Shl(compiler::TNode<Uint32T>{tmp37}, compiler::TNode<Uint32T>{tmp52}));
    compiler::TNode<Uint32T> tmp54;
    USE(tmp54);
    tmp54 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Or(compiler::TNode<Uint32T>{tmp51}, compiler::TNode<Uint32T>{tmp53}));
    compiler::TNode<Uint32T> tmp55;
    USE(tmp55);
    tmp55 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(8));
    compiler::TNode<Uint32T> tmp56;
    USE(tmp56);
    tmp56 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Shl(compiler::TNode<Uint32T>{tmp36}, compiler::TNode<Uint32T>{tmp55}));
    compiler::TNode<Uint32T> tmp57;
    USE(tmp57);
    tmp57 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Or(compiler::TNode<Uint32T>{tmp54}, compiler::TNode<Uint32T>{tmp56}));
    compiler::TNode<Uint32T> tmp58;
    USE(tmp58);
    tmp58 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Or(compiler::TNode<Uint32T>{tmp57}, compiler::TNode<Uint32T>{tmp35}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 369);
    ca_.Goto(&block4, tmp27, tmp28, tmp29, tmp30, tmp31, tmp32, tmp33, tmp34, tmp35, tmp36, tmp37, tmp38, tmp49, tmp58);
  }

  if (block3.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp59;
    compiler::TNode<UintPtrT> tmp60;
    compiler::TNode<BoolT> tmp61;
    compiler::TNode<RawPtrT> tmp62;
    compiler::TNode<Uint32T> tmp63;
    compiler::TNode<Uint32T> tmp64;
    compiler::TNode<Uint32T> tmp65;
    compiler::TNode<Uint32T> tmp66;
    compiler::TNode<Uint32T> tmp67;
    compiler::TNode<Uint32T> tmp68;
    compiler::TNode<Uint32T> tmp69;
    compiler::TNode<Uint32T> tmp70;
    compiler::TNode<Uint32T> tmp71;
    compiler::TNode<Uint32T> tmp72;
    ca_.Bind(&block3, &tmp59, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 373);
    compiler::TNode<Uint32T> tmp73;
    USE(tmp73);
    tmp73 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(24));
    compiler::TNode<Uint32T> tmp74;
    USE(tmp74);
    tmp74 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Shl(compiler::TNode<Uint32T>{tmp63}, compiler::TNode<Uint32T>{tmp73}));
    compiler::TNode<Uint32T> tmp75;
    USE(tmp75);
    tmp75 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(16));
    compiler::TNode<Uint32T> tmp76;
    USE(tmp76);
    tmp76 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Shl(compiler::TNode<Uint32T>{tmp64}, compiler::TNode<Uint32T>{tmp75}));
    compiler::TNode<Uint32T> tmp77;
    USE(tmp77);
    tmp77 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Or(compiler::TNode<Uint32T>{tmp74}, compiler::TNode<Uint32T>{tmp76}));
    compiler::TNode<Uint32T> tmp78;
    USE(tmp78);
    tmp78 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(8));
    compiler::TNode<Uint32T> tmp79;
    USE(tmp79);
    tmp79 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Shl(compiler::TNode<Uint32T>{tmp65}, compiler::TNode<Uint32T>{tmp78}));
    compiler::TNode<Uint32T> tmp80;
    USE(tmp80);
    tmp80 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Or(compiler::TNode<Uint32T>{tmp77}, compiler::TNode<Uint32T>{tmp79}));
    compiler::TNode<Uint32T> tmp81;
    USE(tmp81);
    tmp81 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Or(compiler::TNode<Uint32T>{tmp80}, compiler::TNode<Uint32T>{tmp66}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 374);
    compiler::TNode<Uint32T> tmp82;
    USE(tmp82);
    tmp82 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(24));
    compiler::TNode<Uint32T> tmp83;
    USE(tmp83);
    tmp83 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Shl(compiler::TNode<Uint32T>{tmp67}, compiler::TNode<Uint32T>{tmp82}));
    compiler::TNode<Uint32T> tmp84;
    USE(tmp84);
    tmp84 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(16));
    compiler::TNode<Uint32T> tmp85;
    USE(tmp85);
    tmp85 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Shl(compiler::TNode<Uint32T>{tmp68}, compiler::TNode<Uint32T>{tmp84}));
    compiler::TNode<Uint32T> tmp86;
    USE(tmp86);
    tmp86 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Or(compiler::TNode<Uint32T>{tmp83}, compiler::TNode<Uint32T>{tmp85}));
    compiler::TNode<Uint32T> tmp87;
    USE(tmp87);
    tmp87 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(8));
    compiler::TNode<Uint32T> tmp88;
    USE(tmp88);
    tmp88 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Shl(compiler::TNode<Uint32T>{tmp69}, compiler::TNode<Uint32T>{tmp87}));
    compiler::TNode<Uint32T> tmp89;
    USE(tmp89);
    tmp89 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Or(compiler::TNode<Uint32T>{tmp86}, compiler::TNode<Uint32T>{tmp88}));
    compiler::TNode<Uint32T> tmp90;
    USE(tmp90);
    tmp90 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Or(compiler::TNode<Uint32T>{tmp89}, compiler::TNode<Uint32T>{tmp70}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 369);
    ca_.Goto(&block4, tmp59, tmp60, tmp61, tmp62, tmp63, tmp64, tmp65, tmp66, tmp67, tmp68, tmp69, tmp70, tmp90, tmp81);
  }

  if (block4.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp91;
    compiler::TNode<UintPtrT> tmp92;
    compiler::TNode<BoolT> tmp93;
    compiler::TNode<RawPtrT> tmp94;
    compiler::TNode<Uint32T> tmp95;
    compiler::TNode<Uint32T> tmp96;
    compiler::TNode<Uint32T> tmp97;
    compiler::TNode<Uint32T> tmp98;
    compiler::TNode<Uint32T> tmp99;
    compiler::TNode<Uint32T> tmp100;
    compiler::TNode<Uint32T> tmp101;
    compiler::TNode<Uint32T> tmp102;
    compiler::TNode<Uint32T> tmp103;
    compiler::TNode<Uint32T> tmp104;
    ca_.Bind(&block4, &tmp91, &tmp92, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102, &tmp103, &tmp104);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 377);
    compiler::TNode<BigInt> tmp105;
    USE(tmp105);
    tmp105 = ca_.UncheckedCast<BigInt>(DataViewBuiltinsFromDSLAssembler(state_).MakeBigInt(compiler::TNode<Uint32T>{tmp103}, compiler::TNode<Uint32T>{tmp104}, p_signed));
    ca_.Goto(&block1, tmp91, tmp92, tmp93, tmp105);
  }

  if (block1.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp106;
    compiler::TNode<UintPtrT> tmp107;
    compiler::TNode<BoolT> tmp108;
    compiler::TNode<BigInt> tmp109;
    ca_.Bind(&block1, &tmp106, &tmp107, &tmp108, &tmp109);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 353);
    ca_.Goto(&block5, tmp106, tmp107, tmp108, tmp109);
  }

    compiler::TNode<JSArrayBuffer> tmp110;
    compiler::TNode<UintPtrT> tmp111;
    compiler::TNode<BoolT> tmp112;
    compiler::TNode<BigInt> tmp113;
    ca_.Bind(&block5, &tmp110, &tmp111, &tmp112, &tmp113);
  return compiler::TNode<BigInt>{tmp113};
}

compiler::TNode<Numeric> DataViewBuiltinsFromDSLAssembler::DataViewGet(compiler::TNode<Context> p_context, compiler::TNode<Object> p_receiver, compiler::TNode<Object> p_offset, compiler::TNode<Object> p_requestedLittleEndian, ElementsKind p_kind) {
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSDataView, Number, Object, Context> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSDataView, Number, Object, Context, Number> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSDataView, Number> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSDataView, Number> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSDataView, Number, BoolT, JSArrayBuffer> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSDataView, Number, BoolT, JSArrayBuffer> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSDataView, Number, BoolT, JSArrayBuffer, Float64T, UintPtrT, UintPtrT, Float64T, Float64T> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSDataView, Number, BoolT, JSArrayBuffer, Float64T, UintPtrT, UintPtrT, Float64T, Float64T> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSDataView, Number, BoolT, JSArrayBuffer, Float64T, UintPtrT, UintPtrT, Float64T, Float64T, UintPtrT> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSDataView, Number, BoolT, JSArrayBuffer, Float64T, UintPtrT, UintPtrT, Float64T, Float64T, UintPtrT> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSDataView, Number, BoolT, JSArrayBuffer, Float64T, UintPtrT, UintPtrT, Float64T, Float64T, UintPtrT> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSDataView, Number, BoolT, JSArrayBuffer, Float64T, UintPtrT, UintPtrT, Float64T, Float64T, UintPtrT> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSDataView, Number, BoolT, JSArrayBuffer, Float64T, UintPtrT, UintPtrT, Float64T, Float64T, UintPtrT> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSDataView, Number, BoolT, JSArrayBuffer, Float64T, UintPtrT, UintPtrT, Float64T, Float64T, UintPtrT> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSDataView, Number, BoolT, JSArrayBuffer, Float64T, UintPtrT, UintPtrT, Float64T, Float64T, UintPtrT> block19(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSDataView, Number, BoolT, JSArrayBuffer, Float64T, UintPtrT, UintPtrT, Float64T, Float64T, UintPtrT> block20(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSDataView, Number, BoolT, JSArrayBuffer, Float64T, UintPtrT, UintPtrT, Float64T, Float64T, UintPtrT> block22(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSDataView, Number, BoolT, JSArrayBuffer, Float64T, UintPtrT, UintPtrT, Float64T, Float64T, UintPtrT> block23(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSDataView, Number, BoolT, JSArrayBuffer, Float64T, UintPtrT, UintPtrT, Float64T, Float64T, UintPtrT> block25(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSDataView, Number, BoolT, JSArrayBuffer, Float64T, UintPtrT, UintPtrT, Float64T, Float64T, UintPtrT> block26(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSDataView, Number, BoolT, JSArrayBuffer, Float64T, UintPtrT, UintPtrT, Float64T, Float64T, UintPtrT> block28(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSDataView, Number, BoolT, JSArrayBuffer, Float64T, UintPtrT, UintPtrT, Float64T, Float64T, UintPtrT> block29(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSDataView, Number, BoolT, JSArrayBuffer, Float64T, UintPtrT, UintPtrT, Float64T, Float64T, UintPtrT> block31(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSDataView, Number, BoolT, JSArrayBuffer, Float64T, UintPtrT, UintPtrT, Float64T, Float64T, UintPtrT> block32(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSDataView, Number, BoolT, JSArrayBuffer, Float64T, UintPtrT, UintPtrT, Float64T, Float64T, UintPtrT> block34(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSDataView, Number, BoolT, JSArrayBuffer, Float64T, UintPtrT, UintPtrT, Float64T, Float64T, UintPtrT> block35(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSDataView, Number, BoolT, JSArrayBuffer, Float64T, UintPtrT, UintPtrT, Float64T, Float64T, UintPtrT> block37(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, JSDataView, Number, BoolT, JSArrayBuffer, Float64T, UintPtrT, UintPtrT, Float64T, Float64T, UintPtrT> block38(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Numeric> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Numeric> block40(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_receiver, p_offset, p_requestedLittleEndian);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<Object> tmp2;
    compiler::TNode<Object> tmp3;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 389);
    compiler::TNode<String> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<String>(DataViewBuiltinsFromDSLAssembler(state_).MakeDataViewGetterNameString(p_kind));
    compiler::TNode<JSDataView> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<JSDataView>(DataViewBuiltinsFromDSLAssembler(state_).ValidateDataView(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}, compiler::TNode<String>{tmp4}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 388);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 391);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 393);
    compiler::TNode<Number> tmp6;
    USE(tmp6);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp6 = BaseBuiltinsFromDSLAssembler(state_).ToIndex(compiler::TNode<Object>{tmp2}, compiler::TNode<Context>{tmp0}, &label0);
    ca_.Goto(&block4, tmp0, tmp1, tmp2, tmp3, tmp5, ca_.Uninitialized<Number>(), tmp2, tmp0, tmp6);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block5, tmp0, tmp1, tmp2, tmp3, tmp5, ca_.Uninitialized<Number>(), tmp2, tmp0);
    }
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<Object> tmp8;
    compiler::TNode<Object> tmp9;
    compiler::TNode<Object> tmp10;
    compiler::TNode<JSDataView> tmp11;
    compiler::TNode<Number> tmp12;
    compiler::TNode<Object> tmp13;
    compiler::TNode<Context> tmp14;
    ca_.Bind(&block5, &tmp7, &tmp8, &tmp9, &tmp10, &tmp11, &tmp12, &tmp13, &tmp14);
    ca_.Goto(&block3, tmp7, tmp8, tmp9, tmp10, tmp11, tmp12);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp15;
    compiler::TNode<Object> tmp16;
    compiler::TNode<Object> tmp17;
    compiler::TNode<Object> tmp18;
    compiler::TNode<JSDataView> tmp19;
    compiler::TNode<Number> tmp20;
    compiler::TNode<Object> tmp21;
    compiler::TNode<Context> tmp22;
    compiler::TNode<Number> tmp23;
    ca_.Bind(&block4, &tmp15, &tmp16, &tmp17, &tmp18, &tmp19, &tmp20, &tmp21, &tmp22, &tmp23);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 395);
    ca_.Goto(&block2, tmp15, tmp16, tmp17, tmp18, tmp19, tmp23);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp24;
    compiler::TNode<Object> tmp25;
    compiler::TNode<Object> tmp26;
    compiler::TNode<Object> tmp27;
    compiler::TNode<JSDataView> tmp28;
    compiler::TNode<Number> tmp29;
    ca_.Bind(&block3, &tmp24, &tmp25, &tmp26, &tmp27, &tmp28, &tmp29);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 396);
    CodeStubAssembler(state_).ThrowRangeError(compiler::TNode<Context>{tmp24}, MessageTemplate::kInvalidDataViewAccessorOffset);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp30;
    compiler::TNode<Object> tmp31;
    compiler::TNode<Object> tmp32;
    compiler::TNode<Object> tmp33;
    compiler::TNode<JSDataView> tmp34;
    compiler::TNode<Number> tmp35;
    ca_.Bind(&block2, &tmp30, &tmp31, &tmp32, &tmp33, &tmp34, &tmp35);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 399);
    compiler::TNode<BoolT> tmp36;
    USE(tmp36);
    tmp36 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).ToBoolean(compiler::TNode<Object>{tmp33}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 400);
    compiler::TNode<IntPtrT> tmp37 = ca_.IntPtrConstant(JSArrayBufferView::kBufferOffset);
    USE(tmp37);
    compiler::TNode<JSArrayBuffer>tmp38 = CodeStubAssembler(state_).LoadReference<JSArrayBuffer>(CodeStubAssembler::Reference{tmp34, tmp37});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 402);
    compiler::TNode<BoolT> tmp39;
    USE(tmp39);
    tmp39 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsDetachedBuffer(compiler::TNode<JSArrayBuffer>{tmp38}));
    ca_.Branch(tmp39, &block6, &block7, tmp30, tmp31, tmp32, tmp33, tmp34, tmp35, tmp36, tmp38);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp40;
    compiler::TNode<Object> tmp41;
    compiler::TNode<Object> tmp42;
    compiler::TNode<Object> tmp43;
    compiler::TNode<JSDataView> tmp44;
    compiler::TNode<Number> tmp45;
    compiler::TNode<BoolT> tmp46;
    compiler::TNode<JSArrayBuffer> tmp47;
    ca_.Bind(&block6, &tmp40, &tmp41, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 403);
    compiler::TNode<String> tmp48;
    USE(tmp48);
    tmp48 = ca_.UncheckedCast<String>(DataViewBuiltinsFromDSLAssembler(state_).MakeDataViewGetterNameString(p_kind));
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp40}, MessageTemplate::kDetachedOperation, compiler::TNode<Object>{tmp48});
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp49;
    compiler::TNode<Object> tmp50;
    compiler::TNode<Object> tmp51;
    compiler::TNode<Object> tmp52;
    compiler::TNode<JSDataView> tmp53;
    compiler::TNode<Number> tmp54;
    compiler::TNode<BoolT> tmp55;
    compiler::TNode<JSArrayBuffer> tmp56;
    ca_.Bind(&block7, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55, &tmp56);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 406);
    compiler::TNode<Float64T> tmp57;
    USE(tmp57);
    tmp57 = ca_.UncheckedCast<Float64T>(BaseBuiltinsFromDSLAssembler(state_).Convert9ATfloat6420UT5ATSmi10HeapNumber(compiler::TNode<Number>{tmp54}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 407);
    compiler::TNode<UintPtrT> tmp58;
    USE(tmp58);
    tmp58 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert9ATuintptr9ATfloat64(compiler::TNode<Float64T>{tmp57}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 409);
    compiler::TNode<IntPtrT> tmp59 = ca_.IntPtrConstant(JSArrayBufferView::kByteOffsetOffset);
    USE(tmp59);
    compiler::TNode<UintPtrT>tmp60 = CodeStubAssembler(state_).LoadReference<UintPtrT>(CodeStubAssembler::Reference{tmp53, tmp59});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 410);
    compiler::TNode<IntPtrT> tmp61 = ca_.IntPtrConstant(JSArrayBufferView::kByteLengthOffset);
    USE(tmp61);
    compiler::TNode<UintPtrT>tmp62 = CodeStubAssembler(state_).LoadReference<UintPtrT>(CodeStubAssembler::Reference{tmp53, tmp61});
    compiler::TNode<Float64T> tmp63;
    USE(tmp63);
    tmp63 = ca_.UncheckedCast<Float64T>(BaseBuiltinsFromDSLAssembler(state_).Convert9ATfloat649ATuintptr(compiler::TNode<UintPtrT>{tmp62}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 411);
    compiler::TNode<Float64T> tmp64;
    USE(tmp64);
    tmp64 = ca_.UncheckedCast<Float64T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATfloat6417ATconstexpr_int31((DataViewBuiltinsAssembler(state_).DataViewElementSize(p_kind))));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 413);
    compiler::TNode<Float64T> tmp65;
    USE(tmp65);
    tmp65 = ca_.UncheckedCast<Float64T>(CodeStubAssembler(state_).Float64Add(compiler::TNode<Float64T>{tmp57}, compiler::TNode<Float64T>{tmp64}));
    compiler::TNode<BoolT> tmp66;
    USE(tmp66);
    tmp66 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Float64GreaterThan(compiler::TNode<Float64T>{tmp65}, compiler::TNode<Float64T>{tmp63}));
    ca_.Branch(tmp66, &block8, &block9, tmp49, tmp50, tmp51, tmp52, tmp53, tmp54, tmp55, tmp56, tmp57, tmp58, tmp60, tmp63, tmp64);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp67;
    compiler::TNode<Object> tmp68;
    compiler::TNode<Object> tmp69;
    compiler::TNode<Object> tmp70;
    compiler::TNode<JSDataView> tmp71;
    compiler::TNode<Number> tmp72;
    compiler::TNode<BoolT> tmp73;
    compiler::TNode<JSArrayBuffer> tmp74;
    compiler::TNode<Float64T> tmp75;
    compiler::TNode<UintPtrT> tmp76;
    compiler::TNode<UintPtrT> tmp77;
    compiler::TNode<Float64T> tmp78;
    compiler::TNode<Float64T> tmp79;
    ca_.Bind(&block8, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72, &tmp73, &tmp74, &tmp75, &tmp76, &tmp77, &tmp78, &tmp79);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 414);
    CodeStubAssembler(state_).ThrowRangeError(compiler::TNode<Context>{tmp67}, MessageTemplate::kInvalidDataViewAccessorOffset);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp80;
    compiler::TNode<Object> tmp81;
    compiler::TNode<Object> tmp82;
    compiler::TNode<Object> tmp83;
    compiler::TNode<JSDataView> tmp84;
    compiler::TNode<Number> tmp85;
    compiler::TNode<BoolT> tmp86;
    compiler::TNode<JSArrayBuffer> tmp87;
    compiler::TNode<Float64T> tmp88;
    compiler::TNode<UintPtrT> tmp89;
    compiler::TNode<UintPtrT> tmp90;
    compiler::TNode<Float64T> tmp91;
    compiler::TNode<Float64T> tmp92;
    ca_.Bind(&block9, &tmp80, &tmp81, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89, &tmp90, &tmp91, &tmp92);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 417);
    compiler::TNode<UintPtrT> tmp93;
    USE(tmp93);
    tmp93 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrAdd(compiler::TNode<UintPtrT>{tmp89}, compiler::TNode<UintPtrT>{tmp90}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 419);
    if (((CodeStubAssembler(state_).ElementsKindEqual(p_kind, UINT8_ELEMENTS)))) {
      ca_.Goto(&block10, tmp80, tmp81, tmp82, tmp83, tmp84, tmp85, tmp86, tmp87, tmp88, tmp89, tmp90, tmp91, tmp92, tmp93);
    } else {
      ca_.Goto(&block11, tmp80, tmp81, tmp82, tmp83, tmp84, tmp85, tmp86, tmp87, tmp88, tmp89, tmp90, tmp91, tmp92, tmp93);
    }
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp94;
    compiler::TNode<Object> tmp95;
    compiler::TNode<Object> tmp96;
    compiler::TNode<Object> tmp97;
    compiler::TNode<JSDataView> tmp98;
    compiler::TNode<Number> tmp99;
    compiler::TNode<BoolT> tmp100;
    compiler::TNode<JSArrayBuffer> tmp101;
    compiler::TNode<Float64T> tmp102;
    compiler::TNode<UintPtrT> tmp103;
    compiler::TNode<UintPtrT> tmp104;
    compiler::TNode<Float64T> tmp105;
    compiler::TNode<Float64T> tmp106;
    compiler::TNode<UintPtrT> tmp107;
    ca_.Bind(&block10, &tmp94, &tmp95, &tmp96, &tmp97, &tmp98, &tmp99, &tmp100, &tmp101, &tmp102, &tmp103, &tmp104, &tmp105, &tmp106, &tmp107);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 420);
    compiler::TNode<Smi> tmp108;
    USE(tmp108);
    tmp108 = ca_.UncheckedCast<Smi>(DataViewBuiltinsFromDSLAssembler(state_).LoadDataView8(compiler::TNode<JSArrayBuffer>{tmp101}, compiler::TNode<UintPtrT>{tmp107}, false));
    ca_.Goto(&block1, tmp94, tmp95, tmp96, tmp97, tmp108);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp109;
    compiler::TNode<Object> tmp110;
    compiler::TNode<Object> tmp111;
    compiler::TNode<Object> tmp112;
    compiler::TNode<JSDataView> tmp113;
    compiler::TNode<Number> tmp114;
    compiler::TNode<BoolT> tmp115;
    compiler::TNode<JSArrayBuffer> tmp116;
    compiler::TNode<Float64T> tmp117;
    compiler::TNode<UintPtrT> tmp118;
    compiler::TNode<UintPtrT> tmp119;
    compiler::TNode<Float64T> tmp120;
    compiler::TNode<Float64T> tmp121;
    compiler::TNode<UintPtrT> tmp122;
    ca_.Bind(&block11, &tmp109, &tmp110, &tmp111, &tmp112, &tmp113, &tmp114, &tmp115, &tmp116, &tmp117, &tmp118, &tmp119, &tmp120, &tmp121, &tmp122);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 421);
    if (((CodeStubAssembler(state_).ElementsKindEqual(p_kind, INT8_ELEMENTS)))) {
      ca_.Goto(&block13, tmp109, tmp110, tmp111, tmp112, tmp113, tmp114, tmp115, tmp116, tmp117, tmp118, tmp119, tmp120, tmp121, tmp122);
    } else {
      ca_.Goto(&block14, tmp109, tmp110, tmp111, tmp112, tmp113, tmp114, tmp115, tmp116, tmp117, tmp118, tmp119, tmp120, tmp121, tmp122);
    }
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp123;
    compiler::TNode<Object> tmp124;
    compiler::TNode<Object> tmp125;
    compiler::TNode<Object> tmp126;
    compiler::TNode<JSDataView> tmp127;
    compiler::TNode<Number> tmp128;
    compiler::TNode<BoolT> tmp129;
    compiler::TNode<JSArrayBuffer> tmp130;
    compiler::TNode<Float64T> tmp131;
    compiler::TNode<UintPtrT> tmp132;
    compiler::TNode<UintPtrT> tmp133;
    compiler::TNode<Float64T> tmp134;
    compiler::TNode<Float64T> tmp135;
    compiler::TNode<UintPtrT> tmp136;
    ca_.Bind(&block13, &tmp123, &tmp124, &tmp125, &tmp126, &tmp127, &tmp128, &tmp129, &tmp130, &tmp131, &tmp132, &tmp133, &tmp134, &tmp135, &tmp136);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 422);
    compiler::TNode<Smi> tmp137;
    USE(tmp137);
    tmp137 = ca_.UncheckedCast<Smi>(DataViewBuiltinsFromDSLAssembler(state_).LoadDataView8(compiler::TNode<JSArrayBuffer>{tmp130}, compiler::TNode<UintPtrT>{tmp136}, true));
    ca_.Goto(&block1, tmp123, tmp124, tmp125, tmp126, tmp137);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp138;
    compiler::TNode<Object> tmp139;
    compiler::TNode<Object> tmp140;
    compiler::TNode<Object> tmp141;
    compiler::TNode<JSDataView> tmp142;
    compiler::TNode<Number> tmp143;
    compiler::TNode<BoolT> tmp144;
    compiler::TNode<JSArrayBuffer> tmp145;
    compiler::TNode<Float64T> tmp146;
    compiler::TNode<UintPtrT> tmp147;
    compiler::TNode<UintPtrT> tmp148;
    compiler::TNode<Float64T> tmp149;
    compiler::TNode<Float64T> tmp150;
    compiler::TNode<UintPtrT> tmp151;
    ca_.Bind(&block14, &tmp138, &tmp139, &tmp140, &tmp141, &tmp142, &tmp143, &tmp144, &tmp145, &tmp146, &tmp147, &tmp148, &tmp149, &tmp150, &tmp151);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 423);
    if (((CodeStubAssembler(state_).ElementsKindEqual(p_kind, UINT16_ELEMENTS)))) {
      ca_.Goto(&block16, tmp138, tmp139, tmp140, tmp141, tmp142, tmp143, tmp144, tmp145, tmp146, tmp147, tmp148, tmp149, tmp150, tmp151);
    } else {
      ca_.Goto(&block17, tmp138, tmp139, tmp140, tmp141, tmp142, tmp143, tmp144, tmp145, tmp146, tmp147, tmp148, tmp149, tmp150, tmp151);
    }
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp152;
    compiler::TNode<Object> tmp153;
    compiler::TNode<Object> tmp154;
    compiler::TNode<Object> tmp155;
    compiler::TNode<JSDataView> tmp156;
    compiler::TNode<Number> tmp157;
    compiler::TNode<BoolT> tmp158;
    compiler::TNode<JSArrayBuffer> tmp159;
    compiler::TNode<Float64T> tmp160;
    compiler::TNode<UintPtrT> tmp161;
    compiler::TNode<UintPtrT> tmp162;
    compiler::TNode<Float64T> tmp163;
    compiler::TNode<Float64T> tmp164;
    compiler::TNode<UintPtrT> tmp165;
    ca_.Bind(&block16, &tmp152, &tmp153, &tmp154, &tmp155, &tmp156, &tmp157, &tmp158, &tmp159, &tmp160, &tmp161, &tmp162, &tmp163, &tmp164, &tmp165);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 424);
    compiler::TNode<Number> tmp166;
    USE(tmp166);
    tmp166 = ca_.UncheckedCast<Number>(DataViewBuiltinsFromDSLAssembler(state_).LoadDataView16(compiler::TNode<JSArrayBuffer>{tmp159}, compiler::TNode<UintPtrT>{tmp165}, compiler::TNode<BoolT>{tmp158}, false));
    ca_.Goto(&block1, tmp152, tmp153, tmp154, tmp155, tmp166);
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp167;
    compiler::TNode<Object> tmp168;
    compiler::TNode<Object> tmp169;
    compiler::TNode<Object> tmp170;
    compiler::TNode<JSDataView> tmp171;
    compiler::TNode<Number> tmp172;
    compiler::TNode<BoolT> tmp173;
    compiler::TNode<JSArrayBuffer> tmp174;
    compiler::TNode<Float64T> tmp175;
    compiler::TNode<UintPtrT> tmp176;
    compiler::TNode<UintPtrT> tmp177;
    compiler::TNode<Float64T> tmp178;
    compiler::TNode<Float64T> tmp179;
    compiler::TNode<UintPtrT> tmp180;
    ca_.Bind(&block17, &tmp167, &tmp168, &tmp169, &tmp170, &tmp171, &tmp172, &tmp173, &tmp174, &tmp175, &tmp176, &tmp177, &tmp178, &tmp179, &tmp180);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 425);
    if (((CodeStubAssembler(state_).ElementsKindEqual(p_kind, INT16_ELEMENTS)))) {
      ca_.Goto(&block19, tmp167, tmp168, tmp169, tmp170, tmp171, tmp172, tmp173, tmp174, tmp175, tmp176, tmp177, tmp178, tmp179, tmp180);
    } else {
      ca_.Goto(&block20, tmp167, tmp168, tmp169, tmp170, tmp171, tmp172, tmp173, tmp174, tmp175, tmp176, tmp177, tmp178, tmp179, tmp180);
    }
  }

  if (block19.is_used()) {
    compiler::TNode<Context> tmp181;
    compiler::TNode<Object> tmp182;
    compiler::TNode<Object> tmp183;
    compiler::TNode<Object> tmp184;
    compiler::TNode<JSDataView> tmp185;
    compiler::TNode<Number> tmp186;
    compiler::TNode<BoolT> tmp187;
    compiler::TNode<JSArrayBuffer> tmp188;
    compiler::TNode<Float64T> tmp189;
    compiler::TNode<UintPtrT> tmp190;
    compiler::TNode<UintPtrT> tmp191;
    compiler::TNode<Float64T> tmp192;
    compiler::TNode<Float64T> tmp193;
    compiler::TNode<UintPtrT> tmp194;
    ca_.Bind(&block19, &tmp181, &tmp182, &tmp183, &tmp184, &tmp185, &tmp186, &tmp187, &tmp188, &tmp189, &tmp190, &tmp191, &tmp192, &tmp193, &tmp194);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 426);
    compiler::TNode<Number> tmp195;
    USE(tmp195);
    tmp195 = ca_.UncheckedCast<Number>(DataViewBuiltinsFromDSLAssembler(state_).LoadDataView16(compiler::TNode<JSArrayBuffer>{tmp188}, compiler::TNode<UintPtrT>{tmp194}, compiler::TNode<BoolT>{tmp187}, true));
    ca_.Goto(&block1, tmp181, tmp182, tmp183, tmp184, tmp195);
  }

  if (block20.is_used()) {
    compiler::TNode<Context> tmp196;
    compiler::TNode<Object> tmp197;
    compiler::TNode<Object> tmp198;
    compiler::TNode<Object> tmp199;
    compiler::TNode<JSDataView> tmp200;
    compiler::TNode<Number> tmp201;
    compiler::TNode<BoolT> tmp202;
    compiler::TNode<JSArrayBuffer> tmp203;
    compiler::TNode<Float64T> tmp204;
    compiler::TNode<UintPtrT> tmp205;
    compiler::TNode<UintPtrT> tmp206;
    compiler::TNode<Float64T> tmp207;
    compiler::TNode<Float64T> tmp208;
    compiler::TNode<UintPtrT> tmp209;
    ca_.Bind(&block20, &tmp196, &tmp197, &tmp198, &tmp199, &tmp200, &tmp201, &tmp202, &tmp203, &tmp204, &tmp205, &tmp206, &tmp207, &tmp208, &tmp209);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 427);
    if (((CodeStubAssembler(state_).ElementsKindEqual(p_kind, UINT32_ELEMENTS)))) {
      ca_.Goto(&block22, tmp196, tmp197, tmp198, tmp199, tmp200, tmp201, tmp202, tmp203, tmp204, tmp205, tmp206, tmp207, tmp208, tmp209);
    } else {
      ca_.Goto(&block23, tmp196, tmp197, tmp198, tmp199, tmp200, tmp201, tmp202, tmp203, tmp204, tmp205, tmp206, tmp207, tmp208, tmp209);
    }
  }

  if (block22.is_used()) {
    compiler::TNode<Context> tmp210;
    compiler::TNode<Object> tmp211;
    compiler::TNode<Object> tmp212;
    compiler::TNode<Object> tmp213;
    compiler::TNode<JSDataView> tmp214;
    compiler::TNode<Number> tmp215;
    compiler::TNode<BoolT> tmp216;
    compiler::TNode<JSArrayBuffer> tmp217;
    compiler::TNode<Float64T> tmp218;
    compiler::TNode<UintPtrT> tmp219;
    compiler::TNode<UintPtrT> tmp220;
    compiler::TNode<Float64T> tmp221;
    compiler::TNode<Float64T> tmp222;
    compiler::TNode<UintPtrT> tmp223;
    ca_.Bind(&block22, &tmp210, &tmp211, &tmp212, &tmp213, &tmp214, &tmp215, &tmp216, &tmp217, &tmp218, &tmp219, &tmp220, &tmp221, &tmp222, &tmp223);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 428);
    compiler::TNode<Number> tmp224;
    USE(tmp224);
    tmp224 = ca_.UncheckedCast<Number>(DataViewBuiltinsFromDSLAssembler(state_).LoadDataView32(compiler::TNode<JSArrayBuffer>{tmp217}, compiler::TNode<UintPtrT>{tmp223}, compiler::TNode<BoolT>{tmp216}, p_kind));
    ca_.Goto(&block1, tmp210, tmp211, tmp212, tmp213, tmp224);
  }

  if (block23.is_used()) {
    compiler::TNode<Context> tmp225;
    compiler::TNode<Object> tmp226;
    compiler::TNode<Object> tmp227;
    compiler::TNode<Object> tmp228;
    compiler::TNode<JSDataView> tmp229;
    compiler::TNode<Number> tmp230;
    compiler::TNode<BoolT> tmp231;
    compiler::TNode<JSArrayBuffer> tmp232;
    compiler::TNode<Float64T> tmp233;
    compiler::TNode<UintPtrT> tmp234;
    compiler::TNode<UintPtrT> tmp235;
    compiler::TNode<Float64T> tmp236;
    compiler::TNode<Float64T> tmp237;
    compiler::TNode<UintPtrT> tmp238;
    ca_.Bind(&block23, &tmp225, &tmp226, &tmp227, &tmp228, &tmp229, &tmp230, &tmp231, &tmp232, &tmp233, &tmp234, &tmp235, &tmp236, &tmp237, &tmp238);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 429);
    if (((CodeStubAssembler(state_).ElementsKindEqual(p_kind, INT32_ELEMENTS)))) {
      ca_.Goto(&block25, tmp225, tmp226, tmp227, tmp228, tmp229, tmp230, tmp231, tmp232, tmp233, tmp234, tmp235, tmp236, tmp237, tmp238);
    } else {
      ca_.Goto(&block26, tmp225, tmp226, tmp227, tmp228, tmp229, tmp230, tmp231, tmp232, tmp233, tmp234, tmp235, tmp236, tmp237, tmp238);
    }
  }

  if (block25.is_used()) {
    compiler::TNode<Context> tmp239;
    compiler::TNode<Object> tmp240;
    compiler::TNode<Object> tmp241;
    compiler::TNode<Object> tmp242;
    compiler::TNode<JSDataView> tmp243;
    compiler::TNode<Number> tmp244;
    compiler::TNode<BoolT> tmp245;
    compiler::TNode<JSArrayBuffer> tmp246;
    compiler::TNode<Float64T> tmp247;
    compiler::TNode<UintPtrT> tmp248;
    compiler::TNode<UintPtrT> tmp249;
    compiler::TNode<Float64T> tmp250;
    compiler::TNode<Float64T> tmp251;
    compiler::TNode<UintPtrT> tmp252;
    ca_.Bind(&block25, &tmp239, &tmp240, &tmp241, &tmp242, &tmp243, &tmp244, &tmp245, &tmp246, &tmp247, &tmp248, &tmp249, &tmp250, &tmp251, &tmp252);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 430);
    compiler::TNode<Number> tmp253;
    USE(tmp253);
    tmp253 = ca_.UncheckedCast<Number>(DataViewBuiltinsFromDSLAssembler(state_).LoadDataView32(compiler::TNode<JSArrayBuffer>{tmp246}, compiler::TNode<UintPtrT>{tmp252}, compiler::TNode<BoolT>{tmp245}, p_kind));
    ca_.Goto(&block1, tmp239, tmp240, tmp241, tmp242, tmp253);
  }

  if (block26.is_used()) {
    compiler::TNode<Context> tmp254;
    compiler::TNode<Object> tmp255;
    compiler::TNode<Object> tmp256;
    compiler::TNode<Object> tmp257;
    compiler::TNode<JSDataView> tmp258;
    compiler::TNode<Number> tmp259;
    compiler::TNode<BoolT> tmp260;
    compiler::TNode<JSArrayBuffer> tmp261;
    compiler::TNode<Float64T> tmp262;
    compiler::TNode<UintPtrT> tmp263;
    compiler::TNode<UintPtrT> tmp264;
    compiler::TNode<Float64T> tmp265;
    compiler::TNode<Float64T> tmp266;
    compiler::TNode<UintPtrT> tmp267;
    ca_.Bind(&block26, &tmp254, &tmp255, &tmp256, &tmp257, &tmp258, &tmp259, &tmp260, &tmp261, &tmp262, &tmp263, &tmp264, &tmp265, &tmp266, &tmp267);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 431);
    if (((CodeStubAssembler(state_).ElementsKindEqual(p_kind, FLOAT32_ELEMENTS)))) {
      ca_.Goto(&block28, tmp254, tmp255, tmp256, tmp257, tmp258, tmp259, tmp260, tmp261, tmp262, tmp263, tmp264, tmp265, tmp266, tmp267);
    } else {
      ca_.Goto(&block29, tmp254, tmp255, tmp256, tmp257, tmp258, tmp259, tmp260, tmp261, tmp262, tmp263, tmp264, tmp265, tmp266, tmp267);
    }
  }

  if (block28.is_used()) {
    compiler::TNode<Context> tmp268;
    compiler::TNode<Object> tmp269;
    compiler::TNode<Object> tmp270;
    compiler::TNode<Object> tmp271;
    compiler::TNode<JSDataView> tmp272;
    compiler::TNode<Number> tmp273;
    compiler::TNode<BoolT> tmp274;
    compiler::TNode<JSArrayBuffer> tmp275;
    compiler::TNode<Float64T> tmp276;
    compiler::TNode<UintPtrT> tmp277;
    compiler::TNode<UintPtrT> tmp278;
    compiler::TNode<Float64T> tmp279;
    compiler::TNode<Float64T> tmp280;
    compiler::TNode<UintPtrT> tmp281;
    ca_.Bind(&block28, &tmp268, &tmp269, &tmp270, &tmp271, &tmp272, &tmp273, &tmp274, &tmp275, &tmp276, &tmp277, &tmp278, &tmp279, &tmp280, &tmp281);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 432);
    compiler::TNode<Number> tmp282;
    USE(tmp282);
    tmp282 = ca_.UncheckedCast<Number>(DataViewBuiltinsFromDSLAssembler(state_).LoadDataView32(compiler::TNode<JSArrayBuffer>{tmp275}, compiler::TNode<UintPtrT>{tmp281}, compiler::TNode<BoolT>{tmp274}, p_kind));
    ca_.Goto(&block1, tmp268, tmp269, tmp270, tmp271, tmp282);
  }

  if (block29.is_used()) {
    compiler::TNode<Context> tmp283;
    compiler::TNode<Object> tmp284;
    compiler::TNode<Object> tmp285;
    compiler::TNode<Object> tmp286;
    compiler::TNode<JSDataView> tmp287;
    compiler::TNode<Number> tmp288;
    compiler::TNode<BoolT> tmp289;
    compiler::TNode<JSArrayBuffer> tmp290;
    compiler::TNode<Float64T> tmp291;
    compiler::TNode<UintPtrT> tmp292;
    compiler::TNode<UintPtrT> tmp293;
    compiler::TNode<Float64T> tmp294;
    compiler::TNode<Float64T> tmp295;
    compiler::TNode<UintPtrT> tmp296;
    ca_.Bind(&block29, &tmp283, &tmp284, &tmp285, &tmp286, &tmp287, &tmp288, &tmp289, &tmp290, &tmp291, &tmp292, &tmp293, &tmp294, &tmp295, &tmp296);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 433);
    if (((CodeStubAssembler(state_).ElementsKindEqual(p_kind, FLOAT64_ELEMENTS)))) {
      ca_.Goto(&block31, tmp283, tmp284, tmp285, tmp286, tmp287, tmp288, tmp289, tmp290, tmp291, tmp292, tmp293, tmp294, tmp295, tmp296);
    } else {
      ca_.Goto(&block32, tmp283, tmp284, tmp285, tmp286, tmp287, tmp288, tmp289, tmp290, tmp291, tmp292, tmp293, tmp294, tmp295, tmp296);
    }
  }

  if (block31.is_used()) {
    compiler::TNode<Context> tmp297;
    compiler::TNode<Object> tmp298;
    compiler::TNode<Object> tmp299;
    compiler::TNode<Object> tmp300;
    compiler::TNode<JSDataView> tmp301;
    compiler::TNode<Number> tmp302;
    compiler::TNode<BoolT> tmp303;
    compiler::TNode<JSArrayBuffer> tmp304;
    compiler::TNode<Float64T> tmp305;
    compiler::TNode<UintPtrT> tmp306;
    compiler::TNode<UintPtrT> tmp307;
    compiler::TNode<Float64T> tmp308;
    compiler::TNode<Float64T> tmp309;
    compiler::TNode<UintPtrT> tmp310;
    ca_.Bind(&block31, &tmp297, &tmp298, &tmp299, &tmp300, &tmp301, &tmp302, &tmp303, &tmp304, &tmp305, &tmp306, &tmp307, &tmp308, &tmp309, &tmp310);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 434);
    compiler::TNode<Number> tmp311;
    USE(tmp311);
    tmp311 = ca_.UncheckedCast<Number>(DataViewBuiltinsFromDSLAssembler(state_).LoadDataViewFloat64(compiler::TNode<JSArrayBuffer>{tmp304}, compiler::TNode<UintPtrT>{tmp310}, compiler::TNode<BoolT>{tmp303}));
    ca_.Goto(&block1, tmp297, tmp298, tmp299, tmp300, tmp311);
  }

  if (block32.is_used()) {
    compiler::TNode<Context> tmp312;
    compiler::TNode<Object> tmp313;
    compiler::TNode<Object> tmp314;
    compiler::TNode<Object> tmp315;
    compiler::TNode<JSDataView> tmp316;
    compiler::TNode<Number> tmp317;
    compiler::TNode<BoolT> tmp318;
    compiler::TNode<JSArrayBuffer> tmp319;
    compiler::TNode<Float64T> tmp320;
    compiler::TNode<UintPtrT> tmp321;
    compiler::TNode<UintPtrT> tmp322;
    compiler::TNode<Float64T> tmp323;
    compiler::TNode<Float64T> tmp324;
    compiler::TNode<UintPtrT> tmp325;
    ca_.Bind(&block32, &tmp312, &tmp313, &tmp314, &tmp315, &tmp316, &tmp317, &tmp318, &tmp319, &tmp320, &tmp321, &tmp322, &tmp323, &tmp324, &tmp325);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 435);
    if (((CodeStubAssembler(state_).ElementsKindEqual(p_kind, BIGUINT64_ELEMENTS)))) {
      ca_.Goto(&block34, tmp312, tmp313, tmp314, tmp315, tmp316, tmp317, tmp318, tmp319, tmp320, tmp321, tmp322, tmp323, tmp324, tmp325);
    } else {
      ca_.Goto(&block35, tmp312, tmp313, tmp314, tmp315, tmp316, tmp317, tmp318, tmp319, tmp320, tmp321, tmp322, tmp323, tmp324, tmp325);
    }
  }

  if (block34.is_used()) {
    compiler::TNode<Context> tmp326;
    compiler::TNode<Object> tmp327;
    compiler::TNode<Object> tmp328;
    compiler::TNode<Object> tmp329;
    compiler::TNode<JSDataView> tmp330;
    compiler::TNode<Number> tmp331;
    compiler::TNode<BoolT> tmp332;
    compiler::TNode<JSArrayBuffer> tmp333;
    compiler::TNode<Float64T> tmp334;
    compiler::TNode<UintPtrT> tmp335;
    compiler::TNode<UintPtrT> tmp336;
    compiler::TNode<Float64T> tmp337;
    compiler::TNode<Float64T> tmp338;
    compiler::TNode<UintPtrT> tmp339;
    ca_.Bind(&block34, &tmp326, &tmp327, &tmp328, &tmp329, &tmp330, &tmp331, &tmp332, &tmp333, &tmp334, &tmp335, &tmp336, &tmp337, &tmp338, &tmp339);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 436);
    compiler::TNode<BigInt> tmp340;
    USE(tmp340);
    tmp340 = ca_.UncheckedCast<BigInt>(DataViewBuiltinsFromDSLAssembler(state_).LoadDataViewBigInt(compiler::TNode<JSArrayBuffer>{tmp333}, compiler::TNode<UintPtrT>{tmp339}, compiler::TNode<BoolT>{tmp332}, false));
    ca_.Goto(&block1, tmp326, tmp327, tmp328, tmp329, tmp340);
  }

  if (block35.is_used()) {
    compiler::TNode<Context> tmp341;
    compiler::TNode<Object> tmp342;
    compiler::TNode<Object> tmp343;
    compiler::TNode<Object> tmp344;
    compiler::TNode<JSDataView> tmp345;
    compiler::TNode<Number> tmp346;
    compiler::TNode<BoolT> tmp347;
    compiler::TNode<JSArrayBuffer> tmp348;
    compiler::TNode<Float64T> tmp349;
    compiler::TNode<UintPtrT> tmp350;
    compiler::TNode<UintPtrT> tmp351;
    compiler::TNode<Float64T> tmp352;
    compiler::TNode<Float64T> tmp353;
    compiler::TNode<UintPtrT> tmp354;
    ca_.Bind(&block35, &tmp341, &tmp342, &tmp343, &tmp344, &tmp345, &tmp346, &tmp347, &tmp348, &tmp349, &tmp350, &tmp351, &tmp352, &tmp353, &tmp354);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 437);
    if (((CodeStubAssembler(state_).ElementsKindEqual(p_kind, BIGINT64_ELEMENTS)))) {
      ca_.Goto(&block37, tmp341, tmp342, tmp343, tmp344, tmp345, tmp346, tmp347, tmp348, tmp349, tmp350, tmp351, tmp352, tmp353, tmp354);
    } else {
      ca_.Goto(&block38, tmp341, tmp342, tmp343, tmp344, tmp345, tmp346, tmp347, tmp348, tmp349, tmp350, tmp351, tmp352, tmp353, tmp354);
    }
  }

  if (block37.is_used()) {
    compiler::TNode<Context> tmp355;
    compiler::TNode<Object> tmp356;
    compiler::TNode<Object> tmp357;
    compiler::TNode<Object> tmp358;
    compiler::TNode<JSDataView> tmp359;
    compiler::TNode<Number> tmp360;
    compiler::TNode<BoolT> tmp361;
    compiler::TNode<JSArrayBuffer> tmp362;
    compiler::TNode<Float64T> tmp363;
    compiler::TNode<UintPtrT> tmp364;
    compiler::TNode<UintPtrT> tmp365;
    compiler::TNode<Float64T> tmp366;
    compiler::TNode<Float64T> tmp367;
    compiler::TNode<UintPtrT> tmp368;
    ca_.Bind(&block37, &tmp355, &tmp356, &tmp357, &tmp358, &tmp359, &tmp360, &tmp361, &tmp362, &tmp363, &tmp364, &tmp365, &tmp366, &tmp367, &tmp368);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 438);
    compiler::TNode<BigInt> tmp369;
    USE(tmp369);
    tmp369 = ca_.UncheckedCast<BigInt>(DataViewBuiltinsFromDSLAssembler(state_).LoadDataViewBigInt(compiler::TNode<JSArrayBuffer>{tmp362}, compiler::TNode<UintPtrT>{tmp368}, compiler::TNode<BoolT>{tmp361}, true));
    ca_.Goto(&block1, tmp355, tmp356, tmp357, tmp358, tmp369);
  }

  if (block38.is_used()) {
    compiler::TNode<Context> tmp370;
    compiler::TNode<Object> tmp371;
    compiler::TNode<Object> tmp372;
    compiler::TNode<Object> tmp373;
    compiler::TNode<JSDataView> tmp374;
    compiler::TNode<Number> tmp375;
    compiler::TNode<BoolT> tmp376;
    compiler::TNode<JSArrayBuffer> tmp377;
    compiler::TNode<Float64T> tmp378;
    compiler::TNode<UintPtrT> tmp379;
    compiler::TNode<UintPtrT> tmp380;
    compiler::TNode<Float64T> tmp381;
    compiler::TNode<Float64T> tmp382;
    compiler::TNode<UintPtrT> tmp383;
    ca_.Bind(&block38, &tmp370, &tmp371, &tmp372, &tmp373, &tmp374, &tmp375, &tmp376, &tmp377, &tmp378, &tmp379, &tmp380, &tmp381, &tmp382, &tmp383);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 440);
    CodeStubAssembler(state_).Unreachable();
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp384;
    compiler::TNode<Object> tmp385;
    compiler::TNode<Object> tmp386;
    compiler::TNode<Object> tmp387;
    compiler::TNode<Numeric> tmp388;
    ca_.Bind(&block1, &tmp384, &tmp385, &tmp386, &tmp387, &tmp388);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 385);
    ca_.Goto(&block40, tmp384, tmp385, tmp386, tmp387, tmp388);
  }

    compiler::TNode<Context> tmp389;
    compiler::TNode<Object> tmp390;
    compiler::TNode<Object> tmp391;
    compiler::TNode<Object> tmp392;
    compiler::TNode<Numeric> tmp393;
    ca_.Bind(&block40, &tmp389, &tmp390, &tmp391, &tmp392, &tmp393);
  return compiler::TNode<Numeric>{tmp393};
}

TF_BUILTIN(DataViewPrototypeGetUint8, CodeStubAssembler) {
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
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, torque_arguments.frame, torque_arguments.base, torque_arguments.length);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<RawPtrT> tmp2;
    compiler::TNode<RawPtrT> tmp3;
    compiler::TNode<IntPtrT> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 446);
    compiler::TNode<IntPtrT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp4}, compiler::TNode<IntPtrT>{tmp5}));
    ca_.Branch(tmp6, &block1, &block2, tmp0, tmp1, tmp2, tmp3, tmp4);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<Object> tmp8;
    compiler::TNode<RawPtrT> tmp9;
    compiler::TNode<RawPtrT> tmp10;
    compiler::TNode<IntPtrT> tmp11;
    ca_.Bind(&block1, &tmp7, &tmp8, &tmp9, &tmp10, &tmp11);
    compiler::TNode<IntPtrT> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp9}, compiler::TNode<RawPtrT>{tmp10}, compiler::TNode<IntPtrT>{tmp11}}, compiler::TNode<IntPtrT>{tmp12}));
    ca_.Goto(&block4, tmp7, tmp8, tmp9, tmp10, tmp11, tmp13);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp14;
    compiler::TNode<Object> tmp15;
    compiler::TNode<RawPtrT> tmp16;
    compiler::TNode<RawPtrT> tmp17;
    compiler::TNode<IntPtrT> tmp18;
    ca_.Bind(&block2, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18);
    compiler::TNode<Oddball> tmp19;
    USE(tmp19);
    tmp19 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block3, tmp14, tmp15, tmp16, tmp17, tmp18, tmp19);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp20;
    compiler::TNode<Object> tmp21;
    compiler::TNode<RawPtrT> tmp22;
    compiler::TNode<RawPtrT> tmp23;
    compiler::TNode<IntPtrT> tmp24;
    compiler::TNode<Object> tmp25;
    ca_.Bind(&block4, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25);
    ca_.Goto(&block3, tmp20, tmp21, tmp22, tmp23, tmp24, tmp25);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp26;
    compiler::TNode<Object> tmp27;
    compiler::TNode<RawPtrT> tmp28;
    compiler::TNode<RawPtrT> tmp29;
    compiler::TNode<IntPtrT> tmp30;
    compiler::TNode<Object> tmp31;
    ca_.Bind(&block3, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 447);
    compiler::TNode<Oddball> tmp32;
    USE(tmp32);
    tmp32 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    compiler::TNode<Numeric> tmp33;
    USE(tmp33);
    tmp33 = ca_.UncheckedCast<Numeric>(DataViewBuiltinsFromDSLAssembler(state_).DataViewGet(compiler::TNode<Context>{tmp26}, compiler::TNode<Object>{tmp27}, compiler::TNode<Object>{tmp31}, compiler::TNode<Object>{tmp32}, UINT8_ELEMENTS));
    arguments.PopAndReturn(tmp33);
  }
}

TF_BUILTIN(DataViewPrototypeGetInt8, CodeStubAssembler) {
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
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, torque_arguments.frame, torque_arguments.base, torque_arguments.length);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<RawPtrT> tmp2;
    compiler::TNode<RawPtrT> tmp3;
    compiler::TNode<IntPtrT> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 452);
    compiler::TNode<IntPtrT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp4}, compiler::TNode<IntPtrT>{tmp5}));
    ca_.Branch(tmp6, &block1, &block2, tmp0, tmp1, tmp2, tmp3, tmp4);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<Object> tmp8;
    compiler::TNode<RawPtrT> tmp9;
    compiler::TNode<RawPtrT> tmp10;
    compiler::TNode<IntPtrT> tmp11;
    ca_.Bind(&block1, &tmp7, &tmp8, &tmp9, &tmp10, &tmp11);
    compiler::TNode<IntPtrT> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp9}, compiler::TNode<RawPtrT>{tmp10}, compiler::TNode<IntPtrT>{tmp11}}, compiler::TNode<IntPtrT>{tmp12}));
    ca_.Goto(&block4, tmp7, tmp8, tmp9, tmp10, tmp11, tmp13);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp14;
    compiler::TNode<Object> tmp15;
    compiler::TNode<RawPtrT> tmp16;
    compiler::TNode<RawPtrT> tmp17;
    compiler::TNode<IntPtrT> tmp18;
    ca_.Bind(&block2, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18);
    compiler::TNode<Oddball> tmp19;
    USE(tmp19);
    tmp19 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block3, tmp14, tmp15, tmp16, tmp17, tmp18, tmp19);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp20;
    compiler::TNode<Object> tmp21;
    compiler::TNode<RawPtrT> tmp22;
    compiler::TNode<RawPtrT> tmp23;
    compiler::TNode<IntPtrT> tmp24;
    compiler::TNode<Object> tmp25;
    ca_.Bind(&block4, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25);
    ca_.Goto(&block3, tmp20, tmp21, tmp22, tmp23, tmp24, tmp25);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp26;
    compiler::TNode<Object> tmp27;
    compiler::TNode<RawPtrT> tmp28;
    compiler::TNode<RawPtrT> tmp29;
    compiler::TNode<IntPtrT> tmp30;
    compiler::TNode<Object> tmp31;
    ca_.Bind(&block3, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 453);
    compiler::TNode<Oddball> tmp32;
    USE(tmp32);
    tmp32 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    compiler::TNode<Numeric> tmp33;
    USE(tmp33);
    tmp33 = ca_.UncheckedCast<Numeric>(DataViewBuiltinsFromDSLAssembler(state_).DataViewGet(compiler::TNode<Context>{tmp26}, compiler::TNode<Object>{tmp27}, compiler::TNode<Object>{tmp31}, compiler::TNode<Object>{tmp32}, INT8_ELEMENTS));
    arguments.PopAndReturn(tmp33);
  }
}

TF_BUILTIN(DataViewPrototypeGetUint16, CodeStubAssembler) {
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
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, torque_arguments.frame, torque_arguments.base, torque_arguments.length);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<RawPtrT> tmp2;
    compiler::TNode<RawPtrT> tmp3;
    compiler::TNode<IntPtrT> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 458);
    compiler::TNode<IntPtrT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp4}, compiler::TNode<IntPtrT>{tmp5}));
    ca_.Branch(tmp6, &block1, &block2, tmp0, tmp1, tmp2, tmp3, tmp4);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<Object> tmp8;
    compiler::TNode<RawPtrT> tmp9;
    compiler::TNode<RawPtrT> tmp10;
    compiler::TNode<IntPtrT> tmp11;
    ca_.Bind(&block1, &tmp7, &tmp8, &tmp9, &tmp10, &tmp11);
    compiler::TNode<IntPtrT> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp9}, compiler::TNode<RawPtrT>{tmp10}, compiler::TNode<IntPtrT>{tmp11}}, compiler::TNode<IntPtrT>{tmp12}));
    ca_.Goto(&block4, tmp7, tmp8, tmp9, tmp10, tmp11, tmp13);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp14;
    compiler::TNode<Object> tmp15;
    compiler::TNode<RawPtrT> tmp16;
    compiler::TNode<RawPtrT> tmp17;
    compiler::TNode<IntPtrT> tmp18;
    ca_.Bind(&block2, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18);
    compiler::TNode<Oddball> tmp19;
    USE(tmp19);
    tmp19 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block3, tmp14, tmp15, tmp16, tmp17, tmp18, tmp19);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp20;
    compiler::TNode<Object> tmp21;
    compiler::TNode<RawPtrT> tmp22;
    compiler::TNode<RawPtrT> tmp23;
    compiler::TNode<IntPtrT> tmp24;
    compiler::TNode<Object> tmp25;
    ca_.Bind(&block4, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25);
    ca_.Goto(&block3, tmp20, tmp21, tmp22, tmp23, tmp24, tmp25);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp26;
    compiler::TNode<Object> tmp27;
    compiler::TNode<RawPtrT> tmp28;
    compiler::TNode<RawPtrT> tmp29;
    compiler::TNode<IntPtrT> tmp30;
    compiler::TNode<Object> tmp31;
    ca_.Bind(&block3, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 460);
    compiler::TNode<IntPtrT> tmp32;
    USE(tmp32);
    tmp32 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<BoolT> tmp33;
    USE(tmp33);
    tmp33 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp30}, compiler::TNode<IntPtrT>{tmp32}));
    ca_.Branch(tmp33, &block5, &block6, tmp26, tmp27, tmp28, tmp29, tmp30, tmp31);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp34;
    compiler::TNode<Object> tmp35;
    compiler::TNode<RawPtrT> tmp36;
    compiler::TNode<RawPtrT> tmp37;
    compiler::TNode<IntPtrT> tmp38;
    compiler::TNode<Object> tmp39;
    ca_.Bind(&block5, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39);
    compiler::TNode<IntPtrT> tmp40;
    USE(tmp40);
    tmp40 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<Object> tmp41;
    USE(tmp41);
    tmp41 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp36}, compiler::TNode<RawPtrT>{tmp37}, compiler::TNode<IntPtrT>{tmp38}}, compiler::TNode<IntPtrT>{tmp40}));
    ca_.Goto(&block8, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39, tmp41);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp42;
    compiler::TNode<Object> tmp43;
    compiler::TNode<RawPtrT> tmp44;
    compiler::TNode<RawPtrT> tmp45;
    compiler::TNode<IntPtrT> tmp46;
    compiler::TNode<Object> tmp47;
    ca_.Bind(&block6, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47);
    compiler::TNode<Oddball> tmp48;
    USE(tmp48);
    tmp48 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block7, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp49;
    compiler::TNode<Object> tmp50;
    compiler::TNode<RawPtrT> tmp51;
    compiler::TNode<RawPtrT> tmp52;
    compiler::TNode<IntPtrT> tmp53;
    compiler::TNode<Object> tmp54;
    compiler::TNode<Object> tmp55;
    ca_.Bind(&block8, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55);
    ca_.Goto(&block7, tmp49, tmp50, tmp51, tmp52, tmp53, tmp54, tmp55);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp56;
    compiler::TNode<Object> tmp57;
    compiler::TNode<RawPtrT> tmp58;
    compiler::TNode<RawPtrT> tmp59;
    compiler::TNode<IntPtrT> tmp60;
    compiler::TNode<Object> tmp61;
    compiler::TNode<Object> tmp62;
    ca_.Bind(&block7, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 459);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 462);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 461);
    compiler::TNode<Numeric> tmp63;
    USE(tmp63);
    tmp63 = ca_.UncheckedCast<Numeric>(DataViewBuiltinsFromDSLAssembler(state_).DataViewGet(compiler::TNode<Context>{tmp56}, compiler::TNode<Object>{tmp57}, compiler::TNode<Object>{tmp61}, compiler::TNode<Object>{tmp62}, UINT16_ELEMENTS));
    arguments.PopAndReturn(tmp63);
  }
}

TF_BUILTIN(DataViewPrototypeGetInt16, CodeStubAssembler) {
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
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, torque_arguments.frame, torque_arguments.base, torque_arguments.length);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<RawPtrT> tmp2;
    compiler::TNode<RawPtrT> tmp3;
    compiler::TNode<IntPtrT> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 467);
    compiler::TNode<IntPtrT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp4}, compiler::TNode<IntPtrT>{tmp5}));
    ca_.Branch(tmp6, &block1, &block2, tmp0, tmp1, tmp2, tmp3, tmp4);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<Object> tmp8;
    compiler::TNode<RawPtrT> tmp9;
    compiler::TNode<RawPtrT> tmp10;
    compiler::TNode<IntPtrT> tmp11;
    ca_.Bind(&block1, &tmp7, &tmp8, &tmp9, &tmp10, &tmp11);
    compiler::TNode<IntPtrT> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp9}, compiler::TNode<RawPtrT>{tmp10}, compiler::TNode<IntPtrT>{tmp11}}, compiler::TNode<IntPtrT>{tmp12}));
    ca_.Goto(&block4, tmp7, tmp8, tmp9, tmp10, tmp11, tmp13);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp14;
    compiler::TNode<Object> tmp15;
    compiler::TNode<RawPtrT> tmp16;
    compiler::TNode<RawPtrT> tmp17;
    compiler::TNode<IntPtrT> tmp18;
    ca_.Bind(&block2, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18);
    compiler::TNode<Oddball> tmp19;
    USE(tmp19);
    tmp19 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block3, tmp14, tmp15, tmp16, tmp17, tmp18, tmp19);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp20;
    compiler::TNode<Object> tmp21;
    compiler::TNode<RawPtrT> tmp22;
    compiler::TNode<RawPtrT> tmp23;
    compiler::TNode<IntPtrT> tmp24;
    compiler::TNode<Object> tmp25;
    ca_.Bind(&block4, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25);
    ca_.Goto(&block3, tmp20, tmp21, tmp22, tmp23, tmp24, tmp25);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp26;
    compiler::TNode<Object> tmp27;
    compiler::TNode<RawPtrT> tmp28;
    compiler::TNode<RawPtrT> tmp29;
    compiler::TNode<IntPtrT> tmp30;
    compiler::TNode<Object> tmp31;
    ca_.Bind(&block3, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 469);
    compiler::TNode<IntPtrT> tmp32;
    USE(tmp32);
    tmp32 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<BoolT> tmp33;
    USE(tmp33);
    tmp33 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp30}, compiler::TNode<IntPtrT>{tmp32}));
    ca_.Branch(tmp33, &block5, &block6, tmp26, tmp27, tmp28, tmp29, tmp30, tmp31);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp34;
    compiler::TNode<Object> tmp35;
    compiler::TNode<RawPtrT> tmp36;
    compiler::TNode<RawPtrT> tmp37;
    compiler::TNode<IntPtrT> tmp38;
    compiler::TNode<Object> tmp39;
    ca_.Bind(&block5, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39);
    compiler::TNode<IntPtrT> tmp40;
    USE(tmp40);
    tmp40 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<Object> tmp41;
    USE(tmp41);
    tmp41 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp36}, compiler::TNode<RawPtrT>{tmp37}, compiler::TNode<IntPtrT>{tmp38}}, compiler::TNode<IntPtrT>{tmp40}));
    ca_.Goto(&block8, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39, tmp41);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp42;
    compiler::TNode<Object> tmp43;
    compiler::TNode<RawPtrT> tmp44;
    compiler::TNode<RawPtrT> tmp45;
    compiler::TNode<IntPtrT> tmp46;
    compiler::TNode<Object> tmp47;
    ca_.Bind(&block6, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47);
    compiler::TNode<Oddball> tmp48;
    USE(tmp48);
    tmp48 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block7, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp49;
    compiler::TNode<Object> tmp50;
    compiler::TNode<RawPtrT> tmp51;
    compiler::TNode<RawPtrT> tmp52;
    compiler::TNode<IntPtrT> tmp53;
    compiler::TNode<Object> tmp54;
    compiler::TNode<Object> tmp55;
    ca_.Bind(&block8, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55);
    ca_.Goto(&block7, tmp49, tmp50, tmp51, tmp52, tmp53, tmp54, tmp55);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp56;
    compiler::TNode<Object> tmp57;
    compiler::TNode<RawPtrT> tmp58;
    compiler::TNode<RawPtrT> tmp59;
    compiler::TNode<IntPtrT> tmp60;
    compiler::TNode<Object> tmp61;
    compiler::TNode<Object> tmp62;
    ca_.Bind(&block7, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 468);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 471);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 470);
    compiler::TNode<Numeric> tmp63;
    USE(tmp63);
    tmp63 = ca_.UncheckedCast<Numeric>(DataViewBuiltinsFromDSLAssembler(state_).DataViewGet(compiler::TNode<Context>{tmp56}, compiler::TNode<Object>{tmp57}, compiler::TNode<Object>{tmp61}, compiler::TNode<Object>{tmp62}, INT16_ELEMENTS));
    arguments.PopAndReturn(tmp63);
  }
}

TF_BUILTIN(DataViewPrototypeGetUint32, CodeStubAssembler) {
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
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, torque_arguments.frame, torque_arguments.base, torque_arguments.length);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<RawPtrT> tmp2;
    compiler::TNode<RawPtrT> tmp3;
    compiler::TNode<IntPtrT> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 476);
    compiler::TNode<IntPtrT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp4}, compiler::TNode<IntPtrT>{tmp5}));
    ca_.Branch(tmp6, &block1, &block2, tmp0, tmp1, tmp2, tmp3, tmp4);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<Object> tmp8;
    compiler::TNode<RawPtrT> tmp9;
    compiler::TNode<RawPtrT> tmp10;
    compiler::TNode<IntPtrT> tmp11;
    ca_.Bind(&block1, &tmp7, &tmp8, &tmp9, &tmp10, &tmp11);
    compiler::TNode<IntPtrT> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp9}, compiler::TNode<RawPtrT>{tmp10}, compiler::TNode<IntPtrT>{tmp11}}, compiler::TNode<IntPtrT>{tmp12}));
    ca_.Goto(&block4, tmp7, tmp8, tmp9, tmp10, tmp11, tmp13);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp14;
    compiler::TNode<Object> tmp15;
    compiler::TNode<RawPtrT> tmp16;
    compiler::TNode<RawPtrT> tmp17;
    compiler::TNode<IntPtrT> tmp18;
    ca_.Bind(&block2, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18);
    compiler::TNode<Oddball> tmp19;
    USE(tmp19);
    tmp19 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block3, tmp14, tmp15, tmp16, tmp17, tmp18, tmp19);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp20;
    compiler::TNode<Object> tmp21;
    compiler::TNode<RawPtrT> tmp22;
    compiler::TNode<RawPtrT> tmp23;
    compiler::TNode<IntPtrT> tmp24;
    compiler::TNode<Object> tmp25;
    ca_.Bind(&block4, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25);
    ca_.Goto(&block3, tmp20, tmp21, tmp22, tmp23, tmp24, tmp25);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp26;
    compiler::TNode<Object> tmp27;
    compiler::TNode<RawPtrT> tmp28;
    compiler::TNode<RawPtrT> tmp29;
    compiler::TNode<IntPtrT> tmp30;
    compiler::TNode<Object> tmp31;
    ca_.Bind(&block3, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 478);
    compiler::TNode<IntPtrT> tmp32;
    USE(tmp32);
    tmp32 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<BoolT> tmp33;
    USE(tmp33);
    tmp33 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp30}, compiler::TNode<IntPtrT>{tmp32}));
    ca_.Branch(tmp33, &block5, &block6, tmp26, tmp27, tmp28, tmp29, tmp30, tmp31);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp34;
    compiler::TNode<Object> tmp35;
    compiler::TNode<RawPtrT> tmp36;
    compiler::TNode<RawPtrT> tmp37;
    compiler::TNode<IntPtrT> tmp38;
    compiler::TNode<Object> tmp39;
    ca_.Bind(&block5, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39);
    compiler::TNode<IntPtrT> tmp40;
    USE(tmp40);
    tmp40 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<Object> tmp41;
    USE(tmp41);
    tmp41 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp36}, compiler::TNode<RawPtrT>{tmp37}, compiler::TNode<IntPtrT>{tmp38}}, compiler::TNode<IntPtrT>{tmp40}));
    ca_.Goto(&block8, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39, tmp41);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp42;
    compiler::TNode<Object> tmp43;
    compiler::TNode<RawPtrT> tmp44;
    compiler::TNode<RawPtrT> tmp45;
    compiler::TNode<IntPtrT> tmp46;
    compiler::TNode<Object> tmp47;
    ca_.Bind(&block6, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47);
    compiler::TNode<Oddball> tmp48;
    USE(tmp48);
    tmp48 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block7, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp49;
    compiler::TNode<Object> tmp50;
    compiler::TNode<RawPtrT> tmp51;
    compiler::TNode<RawPtrT> tmp52;
    compiler::TNode<IntPtrT> tmp53;
    compiler::TNode<Object> tmp54;
    compiler::TNode<Object> tmp55;
    ca_.Bind(&block8, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55);
    ca_.Goto(&block7, tmp49, tmp50, tmp51, tmp52, tmp53, tmp54, tmp55);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp56;
    compiler::TNode<Object> tmp57;
    compiler::TNode<RawPtrT> tmp58;
    compiler::TNode<RawPtrT> tmp59;
    compiler::TNode<IntPtrT> tmp60;
    compiler::TNode<Object> tmp61;
    compiler::TNode<Object> tmp62;
    ca_.Bind(&block7, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 477);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 480);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 479);
    compiler::TNode<Numeric> tmp63;
    USE(tmp63);
    tmp63 = ca_.UncheckedCast<Numeric>(DataViewBuiltinsFromDSLAssembler(state_).DataViewGet(compiler::TNode<Context>{tmp56}, compiler::TNode<Object>{tmp57}, compiler::TNode<Object>{tmp61}, compiler::TNode<Object>{tmp62}, UINT32_ELEMENTS));
    arguments.PopAndReturn(tmp63);
  }
}

TF_BUILTIN(DataViewPrototypeGetInt32, CodeStubAssembler) {
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
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, torque_arguments.frame, torque_arguments.base, torque_arguments.length);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<RawPtrT> tmp2;
    compiler::TNode<RawPtrT> tmp3;
    compiler::TNode<IntPtrT> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 485);
    compiler::TNode<IntPtrT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp4}, compiler::TNode<IntPtrT>{tmp5}));
    ca_.Branch(tmp6, &block1, &block2, tmp0, tmp1, tmp2, tmp3, tmp4);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<Object> tmp8;
    compiler::TNode<RawPtrT> tmp9;
    compiler::TNode<RawPtrT> tmp10;
    compiler::TNode<IntPtrT> tmp11;
    ca_.Bind(&block1, &tmp7, &tmp8, &tmp9, &tmp10, &tmp11);
    compiler::TNode<IntPtrT> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp9}, compiler::TNode<RawPtrT>{tmp10}, compiler::TNode<IntPtrT>{tmp11}}, compiler::TNode<IntPtrT>{tmp12}));
    ca_.Goto(&block4, tmp7, tmp8, tmp9, tmp10, tmp11, tmp13);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp14;
    compiler::TNode<Object> tmp15;
    compiler::TNode<RawPtrT> tmp16;
    compiler::TNode<RawPtrT> tmp17;
    compiler::TNode<IntPtrT> tmp18;
    ca_.Bind(&block2, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18);
    compiler::TNode<Oddball> tmp19;
    USE(tmp19);
    tmp19 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block3, tmp14, tmp15, tmp16, tmp17, tmp18, tmp19);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp20;
    compiler::TNode<Object> tmp21;
    compiler::TNode<RawPtrT> tmp22;
    compiler::TNode<RawPtrT> tmp23;
    compiler::TNode<IntPtrT> tmp24;
    compiler::TNode<Object> tmp25;
    ca_.Bind(&block4, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25);
    ca_.Goto(&block3, tmp20, tmp21, tmp22, tmp23, tmp24, tmp25);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp26;
    compiler::TNode<Object> tmp27;
    compiler::TNode<RawPtrT> tmp28;
    compiler::TNode<RawPtrT> tmp29;
    compiler::TNode<IntPtrT> tmp30;
    compiler::TNode<Object> tmp31;
    ca_.Bind(&block3, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 487);
    compiler::TNode<IntPtrT> tmp32;
    USE(tmp32);
    tmp32 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<BoolT> tmp33;
    USE(tmp33);
    tmp33 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp30}, compiler::TNode<IntPtrT>{tmp32}));
    ca_.Branch(tmp33, &block5, &block6, tmp26, tmp27, tmp28, tmp29, tmp30, tmp31);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp34;
    compiler::TNode<Object> tmp35;
    compiler::TNode<RawPtrT> tmp36;
    compiler::TNode<RawPtrT> tmp37;
    compiler::TNode<IntPtrT> tmp38;
    compiler::TNode<Object> tmp39;
    ca_.Bind(&block5, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39);
    compiler::TNode<IntPtrT> tmp40;
    USE(tmp40);
    tmp40 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<Object> tmp41;
    USE(tmp41);
    tmp41 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp36}, compiler::TNode<RawPtrT>{tmp37}, compiler::TNode<IntPtrT>{tmp38}}, compiler::TNode<IntPtrT>{tmp40}));
    ca_.Goto(&block8, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39, tmp41);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp42;
    compiler::TNode<Object> tmp43;
    compiler::TNode<RawPtrT> tmp44;
    compiler::TNode<RawPtrT> tmp45;
    compiler::TNode<IntPtrT> tmp46;
    compiler::TNode<Object> tmp47;
    ca_.Bind(&block6, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47);
    compiler::TNode<Oddball> tmp48;
    USE(tmp48);
    tmp48 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block7, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp49;
    compiler::TNode<Object> tmp50;
    compiler::TNode<RawPtrT> tmp51;
    compiler::TNode<RawPtrT> tmp52;
    compiler::TNode<IntPtrT> tmp53;
    compiler::TNode<Object> tmp54;
    compiler::TNode<Object> tmp55;
    ca_.Bind(&block8, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55);
    ca_.Goto(&block7, tmp49, tmp50, tmp51, tmp52, tmp53, tmp54, tmp55);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp56;
    compiler::TNode<Object> tmp57;
    compiler::TNode<RawPtrT> tmp58;
    compiler::TNode<RawPtrT> tmp59;
    compiler::TNode<IntPtrT> tmp60;
    compiler::TNode<Object> tmp61;
    compiler::TNode<Object> tmp62;
    ca_.Bind(&block7, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 486);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 489);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 488);
    compiler::TNode<Numeric> tmp63;
    USE(tmp63);
    tmp63 = ca_.UncheckedCast<Numeric>(DataViewBuiltinsFromDSLAssembler(state_).DataViewGet(compiler::TNode<Context>{tmp56}, compiler::TNode<Object>{tmp57}, compiler::TNode<Object>{tmp61}, compiler::TNode<Object>{tmp62}, INT32_ELEMENTS));
    arguments.PopAndReturn(tmp63);
  }
}

TF_BUILTIN(DataViewPrototypeGetFloat32, CodeStubAssembler) {
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
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, torque_arguments.frame, torque_arguments.base, torque_arguments.length);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<RawPtrT> tmp2;
    compiler::TNode<RawPtrT> tmp3;
    compiler::TNode<IntPtrT> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 494);
    compiler::TNode<IntPtrT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp4}, compiler::TNode<IntPtrT>{tmp5}));
    ca_.Branch(tmp6, &block1, &block2, tmp0, tmp1, tmp2, tmp3, tmp4);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<Object> tmp8;
    compiler::TNode<RawPtrT> tmp9;
    compiler::TNode<RawPtrT> tmp10;
    compiler::TNode<IntPtrT> tmp11;
    ca_.Bind(&block1, &tmp7, &tmp8, &tmp9, &tmp10, &tmp11);
    compiler::TNode<IntPtrT> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp9}, compiler::TNode<RawPtrT>{tmp10}, compiler::TNode<IntPtrT>{tmp11}}, compiler::TNode<IntPtrT>{tmp12}));
    ca_.Goto(&block4, tmp7, tmp8, tmp9, tmp10, tmp11, tmp13);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp14;
    compiler::TNode<Object> tmp15;
    compiler::TNode<RawPtrT> tmp16;
    compiler::TNode<RawPtrT> tmp17;
    compiler::TNode<IntPtrT> tmp18;
    ca_.Bind(&block2, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18);
    compiler::TNode<Oddball> tmp19;
    USE(tmp19);
    tmp19 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block3, tmp14, tmp15, tmp16, tmp17, tmp18, tmp19);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp20;
    compiler::TNode<Object> tmp21;
    compiler::TNode<RawPtrT> tmp22;
    compiler::TNode<RawPtrT> tmp23;
    compiler::TNode<IntPtrT> tmp24;
    compiler::TNode<Object> tmp25;
    ca_.Bind(&block4, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25);
    ca_.Goto(&block3, tmp20, tmp21, tmp22, tmp23, tmp24, tmp25);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp26;
    compiler::TNode<Object> tmp27;
    compiler::TNode<RawPtrT> tmp28;
    compiler::TNode<RawPtrT> tmp29;
    compiler::TNode<IntPtrT> tmp30;
    compiler::TNode<Object> tmp31;
    ca_.Bind(&block3, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 496);
    compiler::TNode<IntPtrT> tmp32;
    USE(tmp32);
    tmp32 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<BoolT> tmp33;
    USE(tmp33);
    tmp33 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp30}, compiler::TNode<IntPtrT>{tmp32}));
    ca_.Branch(tmp33, &block5, &block6, tmp26, tmp27, tmp28, tmp29, tmp30, tmp31);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp34;
    compiler::TNode<Object> tmp35;
    compiler::TNode<RawPtrT> tmp36;
    compiler::TNode<RawPtrT> tmp37;
    compiler::TNode<IntPtrT> tmp38;
    compiler::TNode<Object> tmp39;
    ca_.Bind(&block5, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39);
    compiler::TNode<IntPtrT> tmp40;
    USE(tmp40);
    tmp40 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<Object> tmp41;
    USE(tmp41);
    tmp41 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp36}, compiler::TNode<RawPtrT>{tmp37}, compiler::TNode<IntPtrT>{tmp38}}, compiler::TNode<IntPtrT>{tmp40}));
    ca_.Goto(&block8, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39, tmp41);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp42;
    compiler::TNode<Object> tmp43;
    compiler::TNode<RawPtrT> tmp44;
    compiler::TNode<RawPtrT> tmp45;
    compiler::TNode<IntPtrT> tmp46;
    compiler::TNode<Object> tmp47;
    ca_.Bind(&block6, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47);
    compiler::TNode<Oddball> tmp48;
    USE(tmp48);
    tmp48 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block7, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp49;
    compiler::TNode<Object> tmp50;
    compiler::TNode<RawPtrT> tmp51;
    compiler::TNode<RawPtrT> tmp52;
    compiler::TNode<IntPtrT> tmp53;
    compiler::TNode<Object> tmp54;
    compiler::TNode<Object> tmp55;
    ca_.Bind(&block8, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55);
    ca_.Goto(&block7, tmp49, tmp50, tmp51, tmp52, tmp53, tmp54, tmp55);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp56;
    compiler::TNode<Object> tmp57;
    compiler::TNode<RawPtrT> tmp58;
    compiler::TNode<RawPtrT> tmp59;
    compiler::TNode<IntPtrT> tmp60;
    compiler::TNode<Object> tmp61;
    compiler::TNode<Object> tmp62;
    ca_.Bind(&block7, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 495);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 498);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 497);
    compiler::TNode<Numeric> tmp63;
    USE(tmp63);
    tmp63 = ca_.UncheckedCast<Numeric>(DataViewBuiltinsFromDSLAssembler(state_).DataViewGet(compiler::TNode<Context>{tmp56}, compiler::TNode<Object>{tmp57}, compiler::TNode<Object>{tmp61}, compiler::TNode<Object>{tmp62}, FLOAT32_ELEMENTS));
    arguments.PopAndReturn(tmp63);
  }
}

TF_BUILTIN(DataViewPrototypeGetFloat64, CodeStubAssembler) {
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
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, torque_arguments.frame, torque_arguments.base, torque_arguments.length);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<RawPtrT> tmp2;
    compiler::TNode<RawPtrT> tmp3;
    compiler::TNode<IntPtrT> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 503);
    compiler::TNode<IntPtrT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp4}, compiler::TNode<IntPtrT>{tmp5}));
    ca_.Branch(tmp6, &block1, &block2, tmp0, tmp1, tmp2, tmp3, tmp4);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<Object> tmp8;
    compiler::TNode<RawPtrT> tmp9;
    compiler::TNode<RawPtrT> tmp10;
    compiler::TNode<IntPtrT> tmp11;
    ca_.Bind(&block1, &tmp7, &tmp8, &tmp9, &tmp10, &tmp11);
    compiler::TNode<IntPtrT> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp9}, compiler::TNode<RawPtrT>{tmp10}, compiler::TNode<IntPtrT>{tmp11}}, compiler::TNode<IntPtrT>{tmp12}));
    ca_.Goto(&block4, tmp7, tmp8, tmp9, tmp10, tmp11, tmp13);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp14;
    compiler::TNode<Object> tmp15;
    compiler::TNode<RawPtrT> tmp16;
    compiler::TNode<RawPtrT> tmp17;
    compiler::TNode<IntPtrT> tmp18;
    ca_.Bind(&block2, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18);
    compiler::TNode<Oddball> tmp19;
    USE(tmp19);
    tmp19 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block3, tmp14, tmp15, tmp16, tmp17, tmp18, tmp19);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp20;
    compiler::TNode<Object> tmp21;
    compiler::TNode<RawPtrT> tmp22;
    compiler::TNode<RawPtrT> tmp23;
    compiler::TNode<IntPtrT> tmp24;
    compiler::TNode<Object> tmp25;
    ca_.Bind(&block4, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25);
    ca_.Goto(&block3, tmp20, tmp21, tmp22, tmp23, tmp24, tmp25);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp26;
    compiler::TNode<Object> tmp27;
    compiler::TNode<RawPtrT> tmp28;
    compiler::TNode<RawPtrT> tmp29;
    compiler::TNode<IntPtrT> tmp30;
    compiler::TNode<Object> tmp31;
    ca_.Bind(&block3, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 505);
    compiler::TNode<IntPtrT> tmp32;
    USE(tmp32);
    tmp32 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<BoolT> tmp33;
    USE(tmp33);
    tmp33 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp30}, compiler::TNode<IntPtrT>{tmp32}));
    ca_.Branch(tmp33, &block5, &block6, tmp26, tmp27, tmp28, tmp29, tmp30, tmp31);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp34;
    compiler::TNode<Object> tmp35;
    compiler::TNode<RawPtrT> tmp36;
    compiler::TNode<RawPtrT> tmp37;
    compiler::TNode<IntPtrT> tmp38;
    compiler::TNode<Object> tmp39;
    ca_.Bind(&block5, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39);
    compiler::TNode<IntPtrT> tmp40;
    USE(tmp40);
    tmp40 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<Object> tmp41;
    USE(tmp41);
    tmp41 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp36}, compiler::TNode<RawPtrT>{tmp37}, compiler::TNode<IntPtrT>{tmp38}}, compiler::TNode<IntPtrT>{tmp40}));
    ca_.Goto(&block8, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39, tmp41);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp42;
    compiler::TNode<Object> tmp43;
    compiler::TNode<RawPtrT> tmp44;
    compiler::TNode<RawPtrT> tmp45;
    compiler::TNode<IntPtrT> tmp46;
    compiler::TNode<Object> tmp47;
    ca_.Bind(&block6, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47);
    compiler::TNode<Oddball> tmp48;
    USE(tmp48);
    tmp48 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block7, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp49;
    compiler::TNode<Object> tmp50;
    compiler::TNode<RawPtrT> tmp51;
    compiler::TNode<RawPtrT> tmp52;
    compiler::TNode<IntPtrT> tmp53;
    compiler::TNode<Object> tmp54;
    compiler::TNode<Object> tmp55;
    ca_.Bind(&block8, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55);
    ca_.Goto(&block7, tmp49, tmp50, tmp51, tmp52, tmp53, tmp54, tmp55);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp56;
    compiler::TNode<Object> tmp57;
    compiler::TNode<RawPtrT> tmp58;
    compiler::TNode<RawPtrT> tmp59;
    compiler::TNode<IntPtrT> tmp60;
    compiler::TNode<Object> tmp61;
    compiler::TNode<Object> tmp62;
    ca_.Bind(&block7, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 504);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 507);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 506);
    compiler::TNode<Numeric> tmp63;
    USE(tmp63);
    tmp63 = ca_.UncheckedCast<Numeric>(DataViewBuiltinsFromDSLAssembler(state_).DataViewGet(compiler::TNode<Context>{tmp56}, compiler::TNode<Object>{tmp57}, compiler::TNode<Object>{tmp61}, compiler::TNode<Object>{tmp62}, FLOAT64_ELEMENTS));
    arguments.PopAndReturn(tmp63);
  }
}

TF_BUILTIN(DataViewPrototypeGetBigUint64, CodeStubAssembler) {
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
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, torque_arguments.frame, torque_arguments.base, torque_arguments.length);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<RawPtrT> tmp2;
    compiler::TNode<RawPtrT> tmp3;
    compiler::TNode<IntPtrT> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 512);
    compiler::TNode<IntPtrT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp4}, compiler::TNode<IntPtrT>{tmp5}));
    ca_.Branch(tmp6, &block1, &block2, tmp0, tmp1, tmp2, tmp3, tmp4);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<Object> tmp8;
    compiler::TNode<RawPtrT> tmp9;
    compiler::TNode<RawPtrT> tmp10;
    compiler::TNode<IntPtrT> tmp11;
    ca_.Bind(&block1, &tmp7, &tmp8, &tmp9, &tmp10, &tmp11);
    compiler::TNode<IntPtrT> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp9}, compiler::TNode<RawPtrT>{tmp10}, compiler::TNode<IntPtrT>{tmp11}}, compiler::TNode<IntPtrT>{tmp12}));
    ca_.Goto(&block4, tmp7, tmp8, tmp9, tmp10, tmp11, tmp13);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp14;
    compiler::TNode<Object> tmp15;
    compiler::TNode<RawPtrT> tmp16;
    compiler::TNode<RawPtrT> tmp17;
    compiler::TNode<IntPtrT> tmp18;
    ca_.Bind(&block2, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18);
    compiler::TNode<Oddball> tmp19;
    USE(tmp19);
    tmp19 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block3, tmp14, tmp15, tmp16, tmp17, tmp18, tmp19);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp20;
    compiler::TNode<Object> tmp21;
    compiler::TNode<RawPtrT> tmp22;
    compiler::TNode<RawPtrT> tmp23;
    compiler::TNode<IntPtrT> tmp24;
    compiler::TNode<Object> tmp25;
    ca_.Bind(&block4, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25);
    ca_.Goto(&block3, tmp20, tmp21, tmp22, tmp23, tmp24, tmp25);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp26;
    compiler::TNode<Object> tmp27;
    compiler::TNode<RawPtrT> tmp28;
    compiler::TNode<RawPtrT> tmp29;
    compiler::TNode<IntPtrT> tmp30;
    compiler::TNode<Object> tmp31;
    ca_.Bind(&block3, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 514);
    compiler::TNode<IntPtrT> tmp32;
    USE(tmp32);
    tmp32 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<BoolT> tmp33;
    USE(tmp33);
    tmp33 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp30}, compiler::TNode<IntPtrT>{tmp32}));
    ca_.Branch(tmp33, &block5, &block6, tmp26, tmp27, tmp28, tmp29, tmp30, tmp31);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp34;
    compiler::TNode<Object> tmp35;
    compiler::TNode<RawPtrT> tmp36;
    compiler::TNode<RawPtrT> tmp37;
    compiler::TNode<IntPtrT> tmp38;
    compiler::TNode<Object> tmp39;
    ca_.Bind(&block5, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39);
    compiler::TNode<IntPtrT> tmp40;
    USE(tmp40);
    tmp40 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<Object> tmp41;
    USE(tmp41);
    tmp41 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp36}, compiler::TNode<RawPtrT>{tmp37}, compiler::TNode<IntPtrT>{tmp38}}, compiler::TNode<IntPtrT>{tmp40}));
    ca_.Goto(&block8, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39, tmp41);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp42;
    compiler::TNode<Object> tmp43;
    compiler::TNode<RawPtrT> tmp44;
    compiler::TNode<RawPtrT> tmp45;
    compiler::TNode<IntPtrT> tmp46;
    compiler::TNode<Object> tmp47;
    ca_.Bind(&block6, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47);
    compiler::TNode<Oddball> tmp48;
    USE(tmp48);
    tmp48 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block7, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp49;
    compiler::TNode<Object> tmp50;
    compiler::TNode<RawPtrT> tmp51;
    compiler::TNode<RawPtrT> tmp52;
    compiler::TNode<IntPtrT> tmp53;
    compiler::TNode<Object> tmp54;
    compiler::TNode<Object> tmp55;
    ca_.Bind(&block8, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55);
    ca_.Goto(&block7, tmp49, tmp50, tmp51, tmp52, tmp53, tmp54, tmp55);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp56;
    compiler::TNode<Object> tmp57;
    compiler::TNode<RawPtrT> tmp58;
    compiler::TNode<RawPtrT> tmp59;
    compiler::TNode<IntPtrT> tmp60;
    compiler::TNode<Object> tmp61;
    compiler::TNode<Object> tmp62;
    ca_.Bind(&block7, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 513);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 516);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 515);
    compiler::TNode<Numeric> tmp63;
    USE(tmp63);
    tmp63 = ca_.UncheckedCast<Numeric>(DataViewBuiltinsFromDSLAssembler(state_).DataViewGet(compiler::TNode<Context>{tmp56}, compiler::TNode<Object>{tmp57}, compiler::TNode<Object>{tmp61}, compiler::TNode<Object>{tmp62}, BIGUINT64_ELEMENTS));
    arguments.PopAndReturn(tmp63);
  }
}

TF_BUILTIN(DataViewPrototypeGetBigInt64, CodeStubAssembler) {
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
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, torque_arguments.frame, torque_arguments.base, torque_arguments.length);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<RawPtrT> tmp2;
    compiler::TNode<RawPtrT> tmp3;
    compiler::TNode<IntPtrT> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 521);
    compiler::TNode<IntPtrT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp4}, compiler::TNode<IntPtrT>{tmp5}));
    ca_.Branch(tmp6, &block1, &block2, tmp0, tmp1, tmp2, tmp3, tmp4);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<Object> tmp8;
    compiler::TNode<RawPtrT> tmp9;
    compiler::TNode<RawPtrT> tmp10;
    compiler::TNode<IntPtrT> tmp11;
    ca_.Bind(&block1, &tmp7, &tmp8, &tmp9, &tmp10, &tmp11);
    compiler::TNode<IntPtrT> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp9}, compiler::TNode<RawPtrT>{tmp10}, compiler::TNode<IntPtrT>{tmp11}}, compiler::TNode<IntPtrT>{tmp12}));
    ca_.Goto(&block4, tmp7, tmp8, tmp9, tmp10, tmp11, tmp13);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp14;
    compiler::TNode<Object> tmp15;
    compiler::TNode<RawPtrT> tmp16;
    compiler::TNode<RawPtrT> tmp17;
    compiler::TNode<IntPtrT> tmp18;
    ca_.Bind(&block2, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18);
    compiler::TNode<Oddball> tmp19;
    USE(tmp19);
    tmp19 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block3, tmp14, tmp15, tmp16, tmp17, tmp18, tmp19);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp20;
    compiler::TNode<Object> tmp21;
    compiler::TNode<RawPtrT> tmp22;
    compiler::TNode<RawPtrT> tmp23;
    compiler::TNode<IntPtrT> tmp24;
    compiler::TNode<Object> tmp25;
    ca_.Bind(&block4, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25);
    ca_.Goto(&block3, tmp20, tmp21, tmp22, tmp23, tmp24, tmp25);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp26;
    compiler::TNode<Object> tmp27;
    compiler::TNode<RawPtrT> tmp28;
    compiler::TNode<RawPtrT> tmp29;
    compiler::TNode<IntPtrT> tmp30;
    compiler::TNode<Object> tmp31;
    ca_.Bind(&block3, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 523);
    compiler::TNode<IntPtrT> tmp32;
    USE(tmp32);
    tmp32 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<BoolT> tmp33;
    USE(tmp33);
    tmp33 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp30}, compiler::TNode<IntPtrT>{tmp32}));
    ca_.Branch(tmp33, &block5, &block6, tmp26, tmp27, tmp28, tmp29, tmp30, tmp31);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp34;
    compiler::TNode<Object> tmp35;
    compiler::TNode<RawPtrT> tmp36;
    compiler::TNode<RawPtrT> tmp37;
    compiler::TNode<IntPtrT> tmp38;
    compiler::TNode<Object> tmp39;
    ca_.Bind(&block5, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39);
    compiler::TNode<IntPtrT> tmp40;
    USE(tmp40);
    tmp40 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<Object> tmp41;
    USE(tmp41);
    tmp41 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp36}, compiler::TNode<RawPtrT>{tmp37}, compiler::TNode<IntPtrT>{tmp38}}, compiler::TNode<IntPtrT>{tmp40}));
    ca_.Goto(&block8, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39, tmp41);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp42;
    compiler::TNode<Object> tmp43;
    compiler::TNode<RawPtrT> tmp44;
    compiler::TNode<RawPtrT> tmp45;
    compiler::TNode<IntPtrT> tmp46;
    compiler::TNode<Object> tmp47;
    ca_.Bind(&block6, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47);
    compiler::TNode<Oddball> tmp48;
    USE(tmp48);
    tmp48 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block7, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp49;
    compiler::TNode<Object> tmp50;
    compiler::TNode<RawPtrT> tmp51;
    compiler::TNode<RawPtrT> tmp52;
    compiler::TNode<IntPtrT> tmp53;
    compiler::TNode<Object> tmp54;
    compiler::TNode<Object> tmp55;
    ca_.Bind(&block8, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55);
    ca_.Goto(&block7, tmp49, tmp50, tmp51, tmp52, tmp53, tmp54, tmp55);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp56;
    compiler::TNode<Object> tmp57;
    compiler::TNode<RawPtrT> tmp58;
    compiler::TNode<RawPtrT> tmp59;
    compiler::TNode<IntPtrT> tmp60;
    compiler::TNode<Object> tmp61;
    compiler::TNode<Object> tmp62;
    ca_.Bind(&block7, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 522);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 525);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 524);
    compiler::TNode<Numeric> tmp63;
    USE(tmp63);
    tmp63 = ca_.UncheckedCast<Numeric>(DataViewBuiltinsFromDSLAssembler(state_).DataViewGet(compiler::TNode<Context>{tmp56}, compiler::TNode<Object>{tmp57}, compiler::TNode<Object>{tmp61}, compiler::TNode<Object>{tmp62}, BIGINT64_ELEMENTS));
    arguments.PopAndReturn(tmp63);
  }
}

void DataViewBuiltinsFromDSLAssembler::StoreDataView8(compiler::TNode<JSArrayBuffer> p_buffer, compiler::TNode<UintPtrT> p_offset, compiler::TNode<Uint32T> p_value) {
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, Uint32T> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, Uint32T> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, Uint32T> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_buffer, p_offset, p_value);

  if (block0.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp0;
    compiler::TNode<UintPtrT> tmp1;
    compiler::TNode<Uint32T> tmp2;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 537);
    compiler::TNode<IntPtrT> tmp3 = ca_.IntPtrConstant(JSArrayBuffer::kBackingStoreOffset);
    USE(tmp3);
    compiler::TNode<RawPtrT>tmp4 = CodeStubAssembler(state_).LoadReference<RawPtrT>(CodeStubAssembler::Reference{tmp0, tmp3});
    compiler::TNode<Uint32T> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(0xFF));
    compiler::TNode<Uint32T> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32And(compiler::TNode<Uint32T>{tmp2}, compiler::TNode<Uint32T>{tmp5}));
    DataViewBuiltinsAssembler(state_).StoreWord8(compiler::TNode<RawPtrT>{tmp4}, compiler::TNode<UintPtrT>{tmp1}, compiler::TNode<Uint32T>{tmp6});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 536);
    ca_.Goto(&block1, tmp0, tmp1, tmp2);
  }

  if (block1.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp7;
    compiler::TNode<UintPtrT> tmp8;
    compiler::TNode<Uint32T> tmp9;
    ca_.Bind(&block1, &tmp7, &tmp8, &tmp9);
    ca_.Goto(&block2, tmp7, tmp8, tmp9);
  }

    compiler::TNode<JSArrayBuffer> tmp10;
    compiler::TNode<UintPtrT> tmp11;
    compiler::TNode<Uint32T> tmp12;
    ca_.Bind(&block2, &tmp10, &tmp11, &tmp12);
}

void DataViewBuiltinsFromDSLAssembler::StoreDataView16(compiler::TNode<JSArrayBuffer> p_buffer, compiler::TNode<UintPtrT> p_offset, compiler::TNode<Uint32T> p_value, compiler::TNode<BoolT> p_requestedLittleEndian) {
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, Uint32T, BoolT> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, Uint32T, BoolT, RawPtrT, Uint32T, Uint32T> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, Uint32T, BoolT, RawPtrT, Uint32T, Uint32T> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, Uint32T, BoolT, RawPtrT, Uint32T, Uint32T> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, Uint32T, BoolT> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, Uint32T, BoolT> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_buffer, p_offset, p_value, p_requestedLittleEndian);

  if (block0.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp0;
    compiler::TNode<UintPtrT> tmp1;
    compiler::TNode<Uint32T> tmp2;
    compiler::TNode<BoolT> tmp3;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 543);
    compiler::TNode<IntPtrT> tmp4 = ca_.IntPtrConstant(JSArrayBuffer::kBackingStoreOffset);
    USE(tmp4);
    compiler::TNode<RawPtrT>tmp5 = CodeStubAssembler(state_).LoadReference<RawPtrT>(CodeStubAssembler::Reference{tmp0, tmp4});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 545);
    compiler::TNode<Uint32T> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(0xFF));
    compiler::TNode<Uint32T> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32And(compiler::TNode<Uint32T>{tmp2}, compiler::TNode<Uint32T>{tmp6}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 546);
    compiler::TNode<Uint32T> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(8));
    compiler::TNode<Uint32T> tmp9;
    USE(tmp9);
    tmp9 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Shr(compiler::TNode<Uint32T>{tmp2}, compiler::TNode<Uint32T>{tmp8}));
    compiler::TNode<Uint32T> tmp10;
    USE(tmp10);
    tmp10 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(0xFF));
    compiler::TNode<Uint32T> tmp11;
    USE(tmp11);
    tmp11 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32And(compiler::TNode<Uint32T>{tmp9}, compiler::TNode<Uint32T>{tmp10}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 548);
    ca_.Branch(tmp3, &block2, &block3, tmp0, tmp1, tmp2, tmp3, tmp5, tmp7, tmp11);
  }

  if (block2.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp12;
    compiler::TNode<UintPtrT> tmp13;
    compiler::TNode<Uint32T> tmp14;
    compiler::TNode<BoolT> tmp15;
    compiler::TNode<RawPtrT> tmp16;
    compiler::TNode<Uint32T> tmp17;
    compiler::TNode<Uint32T> tmp18;
    ca_.Bind(&block2, &tmp12, &tmp13, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 549);
    DataViewBuiltinsAssembler(state_).StoreWord8(compiler::TNode<RawPtrT>{tmp16}, compiler::TNode<UintPtrT>{tmp13}, compiler::TNode<Uint32T>{tmp17});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 550);
    compiler::TNode<UintPtrT> tmp19;
    USE(tmp19);
    tmp19 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(1));
    compiler::TNode<UintPtrT> tmp20;
    USE(tmp20);
    tmp20 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrAdd(compiler::TNode<UintPtrT>{tmp13}, compiler::TNode<UintPtrT>{tmp19}));
    DataViewBuiltinsAssembler(state_).StoreWord8(compiler::TNode<RawPtrT>{tmp16}, compiler::TNode<UintPtrT>{tmp20}, compiler::TNode<Uint32T>{tmp18});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 548);
    ca_.Goto(&block4, tmp12, tmp13, tmp14, tmp15, tmp16, tmp17, tmp18);
  }

  if (block3.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp21;
    compiler::TNode<UintPtrT> tmp22;
    compiler::TNode<Uint32T> tmp23;
    compiler::TNode<BoolT> tmp24;
    compiler::TNode<RawPtrT> tmp25;
    compiler::TNode<Uint32T> tmp26;
    compiler::TNode<Uint32T> tmp27;
    ca_.Bind(&block3, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26, &tmp27);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 552);
    DataViewBuiltinsAssembler(state_).StoreWord8(compiler::TNode<RawPtrT>{tmp25}, compiler::TNode<UintPtrT>{tmp22}, compiler::TNode<Uint32T>{tmp27});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 553);
    compiler::TNode<UintPtrT> tmp28;
    USE(tmp28);
    tmp28 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(1));
    compiler::TNode<UintPtrT> tmp29;
    USE(tmp29);
    tmp29 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrAdd(compiler::TNode<UintPtrT>{tmp22}, compiler::TNode<UintPtrT>{tmp28}));
    DataViewBuiltinsAssembler(state_).StoreWord8(compiler::TNode<RawPtrT>{tmp25}, compiler::TNode<UintPtrT>{tmp29}, compiler::TNode<Uint32T>{tmp26});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 548);
    ca_.Goto(&block4, tmp21, tmp22, tmp23, tmp24, tmp25, tmp26, tmp27);
  }

  if (block4.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp30;
    compiler::TNode<UintPtrT> tmp31;
    compiler::TNode<Uint32T> tmp32;
    compiler::TNode<BoolT> tmp33;
    compiler::TNode<RawPtrT> tmp34;
    compiler::TNode<Uint32T> tmp35;
    compiler::TNode<Uint32T> tmp36;
    ca_.Bind(&block4, &tmp30, &tmp31, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 542);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 540);
    ca_.Goto(&block1, tmp30, tmp31, tmp32, tmp33);
  }

  if (block1.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp37;
    compiler::TNode<UintPtrT> tmp38;
    compiler::TNode<Uint32T> tmp39;
    compiler::TNode<BoolT> tmp40;
    ca_.Bind(&block1, &tmp37, &tmp38, &tmp39, &tmp40);
    ca_.Goto(&block5, tmp37, tmp38, tmp39, tmp40);
  }

    compiler::TNode<JSArrayBuffer> tmp41;
    compiler::TNode<UintPtrT> tmp42;
    compiler::TNode<Uint32T> tmp43;
    compiler::TNode<BoolT> tmp44;
    ca_.Bind(&block5, &tmp41, &tmp42, &tmp43, &tmp44);
}

void DataViewBuiltinsFromDSLAssembler::StoreDataView32(compiler::TNode<JSArrayBuffer> p_buffer, compiler::TNode<UintPtrT> p_offset, compiler::TNode<Uint32T> p_value, compiler::TNode<BoolT> p_requestedLittleEndian) {
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, Uint32T, BoolT> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, Uint32T, BoolT, RawPtrT, Uint32T, Uint32T, Uint32T, Uint32T> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, Uint32T, BoolT, RawPtrT, Uint32T, Uint32T, Uint32T, Uint32T> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, Uint32T, BoolT, RawPtrT, Uint32T, Uint32T, Uint32T, Uint32T> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, Uint32T, BoolT> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, Uint32T, BoolT> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_buffer, p_offset, p_value, p_requestedLittleEndian);

  if (block0.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp0;
    compiler::TNode<UintPtrT> tmp1;
    compiler::TNode<Uint32T> tmp2;
    compiler::TNode<BoolT> tmp3;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 560);
    compiler::TNode<IntPtrT> tmp4 = ca_.IntPtrConstant(JSArrayBuffer::kBackingStoreOffset);
    USE(tmp4);
    compiler::TNode<RawPtrT>tmp5 = CodeStubAssembler(state_).LoadReference<RawPtrT>(CodeStubAssembler::Reference{tmp0, tmp4});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 562);
    compiler::TNode<Uint32T> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(0xFF));
    compiler::TNode<Uint32T> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32And(compiler::TNode<Uint32T>{tmp2}, compiler::TNode<Uint32T>{tmp6}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 563);
    compiler::TNode<Uint32T> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(8));
    compiler::TNode<Uint32T> tmp9;
    USE(tmp9);
    tmp9 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Shr(compiler::TNode<Uint32T>{tmp2}, compiler::TNode<Uint32T>{tmp8}));
    compiler::TNode<Uint32T> tmp10;
    USE(tmp10);
    tmp10 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(0xFF));
    compiler::TNode<Uint32T> tmp11;
    USE(tmp11);
    tmp11 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32And(compiler::TNode<Uint32T>{tmp9}, compiler::TNode<Uint32T>{tmp10}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 564);
    compiler::TNode<Uint32T> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(16));
    compiler::TNode<Uint32T> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Shr(compiler::TNode<Uint32T>{tmp2}, compiler::TNode<Uint32T>{tmp12}));
    compiler::TNode<Uint32T> tmp14;
    USE(tmp14);
    tmp14 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(0xFF));
    compiler::TNode<Uint32T> tmp15;
    USE(tmp15);
    tmp15 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32And(compiler::TNode<Uint32T>{tmp13}, compiler::TNode<Uint32T>{tmp14}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 565);
    compiler::TNode<Uint32T> tmp16;
    USE(tmp16);
    tmp16 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(24));
    compiler::TNode<Uint32T> tmp17;
    USE(tmp17);
    tmp17 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Shr(compiler::TNode<Uint32T>{tmp2}, compiler::TNode<Uint32T>{tmp16}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 567);
    ca_.Branch(tmp3, &block2, &block3, tmp0, tmp1, tmp2, tmp3, tmp5, tmp7, tmp11, tmp15, tmp17);
  }

  if (block2.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp18;
    compiler::TNode<UintPtrT> tmp19;
    compiler::TNode<Uint32T> tmp20;
    compiler::TNode<BoolT> tmp21;
    compiler::TNode<RawPtrT> tmp22;
    compiler::TNode<Uint32T> tmp23;
    compiler::TNode<Uint32T> tmp24;
    compiler::TNode<Uint32T> tmp25;
    compiler::TNode<Uint32T> tmp26;
    ca_.Bind(&block2, &tmp18, &tmp19, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 568);
    DataViewBuiltinsAssembler(state_).StoreWord8(compiler::TNode<RawPtrT>{tmp22}, compiler::TNode<UintPtrT>{tmp19}, compiler::TNode<Uint32T>{tmp23});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 569);
    compiler::TNode<UintPtrT> tmp27;
    USE(tmp27);
    tmp27 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(1));
    compiler::TNode<UintPtrT> tmp28;
    USE(tmp28);
    tmp28 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrAdd(compiler::TNode<UintPtrT>{tmp19}, compiler::TNode<UintPtrT>{tmp27}));
    DataViewBuiltinsAssembler(state_).StoreWord8(compiler::TNode<RawPtrT>{tmp22}, compiler::TNode<UintPtrT>{tmp28}, compiler::TNode<Uint32T>{tmp24});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 570);
    compiler::TNode<UintPtrT> tmp29;
    USE(tmp29);
    tmp29 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(2));
    compiler::TNode<UintPtrT> tmp30;
    USE(tmp30);
    tmp30 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrAdd(compiler::TNode<UintPtrT>{tmp19}, compiler::TNode<UintPtrT>{tmp29}));
    DataViewBuiltinsAssembler(state_).StoreWord8(compiler::TNode<RawPtrT>{tmp22}, compiler::TNode<UintPtrT>{tmp30}, compiler::TNode<Uint32T>{tmp25});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 571);
    compiler::TNode<UintPtrT> tmp31;
    USE(tmp31);
    tmp31 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(3));
    compiler::TNode<UintPtrT> tmp32;
    USE(tmp32);
    tmp32 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrAdd(compiler::TNode<UintPtrT>{tmp19}, compiler::TNode<UintPtrT>{tmp31}));
    DataViewBuiltinsAssembler(state_).StoreWord8(compiler::TNode<RawPtrT>{tmp22}, compiler::TNode<UintPtrT>{tmp32}, compiler::TNode<Uint32T>{tmp26});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 567);
    ca_.Goto(&block4, tmp18, tmp19, tmp20, tmp21, tmp22, tmp23, tmp24, tmp25, tmp26);
  }

  if (block3.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp33;
    compiler::TNode<UintPtrT> tmp34;
    compiler::TNode<Uint32T> tmp35;
    compiler::TNode<BoolT> tmp36;
    compiler::TNode<RawPtrT> tmp37;
    compiler::TNode<Uint32T> tmp38;
    compiler::TNode<Uint32T> tmp39;
    compiler::TNode<Uint32T> tmp40;
    compiler::TNode<Uint32T> tmp41;
    ca_.Bind(&block3, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 573);
    DataViewBuiltinsAssembler(state_).StoreWord8(compiler::TNode<RawPtrT>{tmp37}, compiler::TNode<UintPtrT>{tmp34}, compiler::TNode<Uint32T>{tmp41});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 574);
    compiler::TNode<UintPtrT> tmp42;
    USE(tmp42);
    tmp42 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(1));
    compiler::TNode<UintPtrT> tmp43;
    USE(tmp43);
    tmp43 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrAdd(compiler::TNode<UintPtrT>{tmp34}, compiler::TNode<UintPtrT>{tmp42}));
    DataViewBuiltinsAssembler(state_).StoreWord8(compiler::TNode<RawPtrT>{tmp37}, compiler::TNode<UintPtrT>{tmp43}, compiler::TNode<Uint32T>{tmp40});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 575);
    compiler::TNode<UintPtrT> tmp44;
    USE(tmp44);
    tmp44 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(2));
    compiler::TNode<UintPtrT> tmp45;
    USE(tmp45);
    tmp45 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrAdd(compiler::TNode<UintPtrT>{tmp34}, compiler::TNode<UintPtrT>{tmp44}));
    DataViewBuiltinsAssembler(state_).StoreWord8(compiler::TNode<RawPtrT>{tmp37}, compiler::TNode<UintPtrT>{tmp45}, compiler::TNode<Uint32T>{tmp39});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 576);
    compiler::TNode<UintPtrT> tmp46;
    USE(tmp46);
    tmp46 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(3));
    compiler::TNode<UintPtrT> tmp47;
    USE(tmp47);
    tmp47 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrAdd(compiler::TNode<UintPtrT>{tmp34}, compiler::TNode<UintPtrT>{tmp46}));
    DataViewBuiltinsAssembler(state_).StoreWord8(compiler::TNode<RawPtrT>{tmp37}, compiler::TNode<UintPtrT>{tmp47}, compiler::TNode<Uint32T>{tmp38});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 567);
    ca_.Goto(&block4, tmp33, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39, tmp40, tmp41);
  }

  if (block4.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp48;
    compiler::TNode<UintPtrT> tmp49;
    compiler::TNode<Uint32T> tmp50;
    compiler::TNode<BoolT> tmp51;
    compiler::TNode<RawPtrT> tmp52;
    compiler::TNode<Uint32T> tmp53;
    compiler::TNode<Uint32T> tmp54;
    compiler::TNode<Uint32T> tmp55;
    compiler::TNode<Uint32T> tmp56;
    ca_.Bind(&block4, &tmp48, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55, &tmp56);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 559);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 557);
    ca_.Goto(&block1, tmp48, tmp49, tmp50, tmp51);
  }

  if (block1.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp57;
    compiler::TNode<UintPtrT> tmp58;
    compiler::TNode<Uint32T> tmp59;
    compiler::TNode<BoolT> tmp60;
    ca_.Bind(&block1, &tmp57, &tmp58, &tmp59, &tmp60);
    ca_.Goto(&block5, tmp57, tmp58, tmp59, tmp60);
  }

    compiler::TNode<JSArrayBuffer> tmp61;
    compiler::TNode<UintPtrT> tmp62;
    compiler::TNode<Uint32T> tmp63;
    compiler::TNode<BoolT> tmp64;
    ca_.Bind(&block5, &tmp61, &tmp62, &tmp63, &tmp64);
}

void DataViewBuiltinsFromDSLAssembler::StoreDataView64(compiler::TNode<JSArrayBuffer> p_buffer, compiler::TNode<UintPtrT> p_offset, compiler::TNode<Uint32T> p_lowWord, compiler::TNode<Uint32T> p_highWord, compiler::TNode<BoolT> p_requestedLittleEndian) {
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, Uint32T, Uint32T, BoolT> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, Uint32T, Uint32T, BoolT, RawPtrT, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, Uint32T, Uint32T, BoolT, RawPtrT, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, Uint32T, Uint32T, BoolT, RawPtrT, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T, Uint32T> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, Uint32T, Uint32T, BoolT> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, Uint32T, Uint32T, BoolT> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_buffer, p_offset, p_lowWord, p_highWord, p_requestedLittleEndian);

  if (block0.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp0;
    compiler::TNode<UintPtrT> tmp1;
    compiler::TNode<Uint32T> tmp2;
    compiler::TNode<Uint32T> tmp3;
    compiler::TNode<BoolT> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 583);
    compiler::TNode<IntPtrT> tmp5 = ca_.IntPtrConstant(JSArrayBuffer::kBackingStoreOffset);
    USE(tmp5);
    compiler::TNode<RawPtrT>tmp6 = CodeStubAssembler(state_).LoadReference<RawPtrT>(CodeStubAssembler::Reference{tmp0, tmp5});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 585);
    compiler::TNode<Uint32T> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(0xFF));
    compiler::TNode<Uint32T> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32And(compiler::TNode<Uint32T>{tmp2}, compiler::TNode<Uint32T>{tmp7}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 586);
    compiler::TNode<Uint32T> tmp9;
    USE(tmp9);
    tmp9 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(8));
    compiler::TNode<Uint32T> tmp10;
    USE(tmp10);
    tmp10 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Shr(compiler::TNode<Uint32T>{tmp2}, compiler::TNode<Uint32T>{tmp9}));
    compiler::TNode<Uint32T> tmp11;
    USE(tmp11);
    tmp11 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(0xFF));
    compiler::TNode<Uint32T> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32And(compiler::TNode<Uint32T>{tmp10}, compiler::TNode<Uint32T>{tmp11}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 587);
    compiler::TNode<Uint32T> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(16));
    compiler::TNode<Uint32T> tmp14;
    USE(tmp14);
    tmp14 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Shr(compiler::TNode<Uint32T>{tmp2}, compiler::TNode<Uint32T>{tmp13}));
    compiler::TNode<Uint32T> tmp15;
    USE(tmp15);
    tmp15 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(0xFF));
    compiler::TNode<Uint32T> tmp16;
    USE(tmp16);
    tmp16 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32And(compiler::TNode<Uint32T>{tmp14}, compiler::TNode<Uint32T>{tmp15}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 588);
    compiler::TNode<Uint32T> tmp17;
    USE(tmp17);
    tmp17 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(24));
    compiler::TNode<Uint32T> tmp18;
    USE(tmp18);
    tmp18 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Shr(compiler::TNode<Uint32T>{tmp2}, compiler::TNode<Uint32T>{tmp17}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 590);
    compiler::TNode<Uint32T> tmp19;
    USE(tmp19);
    tmp19 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(0xFF));
    compiler::TNode<Uint32T> tmp20;
    USE(tmp20);
    tmp20 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32And(compiler::TNode<Uint32T>{tmp3}, compiler::TNode<Uint32T>{tmp19}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 591);
    compiler::TNode<Uint32T> tmp21;
    USE(tmp21);
    tmp21 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(8));
    compiler::TNode<Uint32T> tmp22;
    USE(tmp22);
    tmp22 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Shr(compiler::TNode<Uint32T>{tmp3}, compiler::TNode<Uint32T>{tmp21}));
    compiler::TNode<Uint32T> tmp23;
    USE(tmp23);
    tmp23 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(0xFF));
    compiler::TNode<Uint32T> tmp24;
    USE(tmp24);
    tmp24 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32And(compiler::TNode<Uint32T>{tmp22}, compiler::TNode<Uint32T>{tmp23}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 592);
    compiler::TNode<Uint32T> tmp25;
    USE(tmp25);
    tmp25 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(16));
    compiler::TNode<Uint32T> tmp26;
    USE(tmp26);
    tmp26 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Shr(compiler::TNode<Uint32T>{tmp3}, compiler::TNode<Uint32T>{tmp25}));
    compiler::TNode<Uint32T> tmp27;
    USE(tmp27);
    tmp27 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(0xFF));
    compiler::TNode<Uint32T> tmp28;
    USE(tmp28);
    tmp28 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32And(compiler::TNode<Uint32T>{tmp26}, compiler::TNode<Uint32T>{tmp27}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 593);
    compiler::TNode<Uint32T> tmp29;
    USE(tmp29);
    tmp29 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(24));
    compiler::TNode<Uint32T> tmp30;
    USE(tmp30);
    tmp30 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Word32Shr(compiler::TNode<Uint32T>{tmp3}, compiler::TNode<Uint32T>{tmp29}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 595);
    ca_.Branch(tmp4, &block2, &block3, tmp0, tmp1, tmp2, tmp3, tmp4, tmp6, tmp8, tmp12, tmp16, tmp18, tmp20, tmp24, tmp28, tmp30);
  }

  if (block2.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp31;
    compiler::TNode<UintPtrT> tmp32;
    compiler::TNode<Uint32T> tmp33;
    compiler::TNode<Uint32T> tmp34;
    compiler::TNode<BoolT> tmp35;
    compiler::TNode<RawPtrT> tmp36;
    compiler::TNode<Uint32T> tmp37;
    compiler::TNode<Uint32T> tmp38;
    compiler::TNode<Uint32T> tmp39;
    compiler::TNode<Uint32T> tmp40;
    compiler::TNode<Uint32T> tmp41;
    compiler::TNode<Uint32T> tmp42;
    compiler::TNode<Uint32T> tmp43;
    compiler::TNode<Uint32T> tmp44;
    ca_.Bind(&block2, &tmp31, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40, &tmp41, &tmp42, &tmp43, &tmp44);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 596);
    DataViewBuiltinsAssembler(state_).StoreWord8(compiler::TNode<RawPtrT>{tmp36}, compiler::TNode<UintPtrT>{tmp32}, compiler::TNode<Uint32T>{tmp37});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 597);
    compiler::TNode<UintPtrT> tmp45;
    USE(tmp45);
    tmp45 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(1));
    compiler::TNode<UintPtrT> tmp46;
    USE(tmp46);
    tmp46 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrAdd(compiler::TNode<UintPtrT>{tmp32}, compiler::TNode<UintPtrT>{tmp45}));
    DataViewBuiltinsAssembler(state_).StoreWord8(compiler::TNode<RawPtrT>{tmp36}, compiler::TNode<UintPtrT>{tmp46}, compiler::TNode<Uint32T>{tmp38});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 598);
    compiler::TNode<UintPtrT> tmp47;
    USE(tmp47);
    tmp47 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(2));
    compiler::TNode<UintPtrT> tmp48;
    USE(tmp48);
    tmp48 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrAdd(compiler::TNode<UintPtrT>{tmp32}, compiler::TNode<UintPtrT>{tmp47}));
    DataViewBuiltinsAssembler(state_).StoreWord8(compiler::TNode<RawPtrT>{tmp36}, compiler::TNode<UintPtrT>{tmp48}, compiler::TNode<Uint32T>{tmp39});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 599);
    compiler::TNode<UintPtrT> tmp49;
    USE(tmp49);
    tmp49 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(3));
    compiler::TNode<UintPtrT> tmp50;
    USE(tmp50);
    tmp50 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrAdd(compiler::TNode<UintPtrT>{tmp32}, compiler::TNode<UintPtrT>{tmp49}));
    DataViewBuiltinsAssembler(state_).StoreWord8(compiler::TNode<RawPtrT>{tmp36}, compiler::TNode<UintPtrT>{tmp50}, compiler::TNode<Uint32T>{tmp40});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 600);
    compiler::TNode<UintPtrT> tmp51;
    USE(tmp51);
    tmp51 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(4));
    compiler::TNode<UintPtrT> tmp52;
    USE(tmp52);
    tmp52 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrAdd(compiler::TNode<UintPtrT>{tmp32}, compiler::TNode<UintPtrT>{tmp51}));
    DataViewBuiltinsAssembler(state_).StoreWord8(compiler::TNode<RawPtrT>{tmp36}, compiler::TNode<UintPtrT>{tmp52}, compiler::TNode<Uint32T>{tmp41});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 601);
    compiler::TNode<UintPtrT> tmp53;
    USE(tmp53);
    tmp53 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(5));
    compiler::TNode<UintPtrT> tmp54;
    USE(tmp54);
    tmp54 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrAdd(compiler::TNode<UintPtrT>{tmp32}, compiler::TNode<UintPtrT>{tmp53}));
    DataViewBuiltinsAssembler(state_).StoreWord8(compiler::TNode<RawPtrT>{tmp36}, compiler::TNode<UintPtrT>{tmp54}, compiler::TNode<Uint32T>{tmp42});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 602);
    compiler::TNode<UintPtrT> tmp55;
    USE(tmp55);
    tmp55 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(6));
    compiler::TNode<UintPtrT> tmp56;
    USE(tmp56);
    tmp56 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrAdd(compiler::TNode<UintPtrT>{tmp32}, compiler::TNode<UintPtrT>{tmp55}));
    DataViewBuiltinsAssembler(state_).StoreWord8(compiler::TNode<RawPtrT>{tmp36}, compiler::TNode<UintPtrT>{tmp56}, compiler::TNode<Uint32T>{tmp43});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 603);
    compiler::TNode<UintPtrT> tmp57;
    USE(tmp57);
    tmp57 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(7));
    compiler::TNode<UintPtrT> tmp58;
    USE(tmp58);
    tmp58 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrAdd(compiler::TNode<UintPtrT>{tmp32}, compiler::TNode<UintPtrT>{tmp57}));
    DataViewBuiltinsAssembler(state_).StoreWord8(compiler::TNode<RawPtrT>{tmp36}, compiler::TNode<UintPtrT>{tmp58}, compiler::TNode<Uint32T>{tmp44});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 595);
    ca_.Goto(&block4, tmp31, tmp32, tmp33, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39, tmp40, tmp41, tmp42, tmp43, tmp44);
  }

  if (block3.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp59;
    compiler::TNode<UintPtrT> tmp60;
    compiler::TNode<Uint32T> tmp61;
    compiler::TNode<Uint32T> tmp62;
    compiler::TNode<BoolT> tmp63;
    compiler::TNode<RawPtrT> tmp64;
    compiler::TNode<Uint32T> tmp65;
    compiler::TNode<Uint32T> tmp66;
    compiler::TNode<Uint32T> tmp67;
    compiler::TNode<Uint32T> tmp68;
    compiler::TNode<Uint32T> tmp69;
    compiler::TNode<Uint32T> tmp70;
    compiler::TNode<Uint32T> tmp71;
    compiler::TNode<Uint32T> tmp72;
    ca_.Bind(&block3, &tmp59, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 605);
    DataViewBuiltinsAssembler(state_).StoreWord8(compiler::TNode<RawPtrT>{tmp64}, compiler::TNode<UintPtrT>{tmp60}, compiler::TNode<Uint32T>{tmp72});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 606);
    compiler::TNode<UintPtrT> tmp73;
    USE(tmp73);
    tmp73 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(1));
    compiler::TNode<UintPtrT> tmp74;
    USE(tmp74);
    tmp74 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrAdd(compiler::TNode<UintPtrT>{tmp60}, compiler::TNode<UintPtrT>{tmp73}));
    DataViewBuiltinsAssembler(state_).StoreWord8(compiler::TNode<RawPtrT>{tmp64}, compiler::TNode<UintPtrT>{tmp74}, compiler::TNode<Uint32T>{tmp71});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 607);
    compiler::TNode<UintPtrT> tmp75;
    USE(tmp75);
    tmp75 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(2));
    compiler::TNode<UintPtrT> tmp76;
    USE(tmp76);
    tmp76 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrAdd(compiler::TNode<UintPtrT>{tmp60}, compiler::TNode<UintPtrT>{tmp75}));
    DataViewBuiltinsAssembler(state_).StoreWord8(compiler::TNode<RawPtrT>{tmp64}, compiler::TNode<UintPtrT>{tmp76}, compiler::TNode<Uint32T>{tmp70});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 608);
    compiler::TNode<UintPtrT> tmp77;
    USE(tmp77);
    tmp77 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(3));
    compiler::TNode<UintPtrT> tmp78;
    USE(tmp78);
    tmp78 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrAdd(compiler::TNode<UintPtrT>{tmp60}, compiler::TNode<UintPtrT>{tmp77}));
    DataViewBuiltinsAssembler(state_).StoreWord8(compiler::TNode<RawPtrT>{tmp64}, compiler::TNode<UintPtrT>{tmp78}, compiler::TNode<Uint32T>{tmp69});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 609);
    compiler::TNode<UintPtrT> tmp79;
    USE(tmp79);
    tmp79 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(4));
    compiler::TNode<UintPtrT> tmp80;
    USE(tmp80);
    tmp80 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrAdd(compiler::TNode<UintPtrT>{tmp60}, compiler::TNode<UintPtrT>{tmp79}));
    DataViewBuiltinsAssembler(state_).StoreWord8(compiler::TNode<RawPtrT>{tmp64}, compiler::TNode<UintPtrT>{tmp80}, compiler::TNode<Uint32T>{tmp68});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 610);
    compiler::TNode<UintPtrT> tmp81;
    USE(tmp81);
    tmp81 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(5));
    compiler::TNode<UintPtrT> tmp82;
    USE(tmp82);
    tmp82 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrAdd(compiler::TNode<UintPtrT>{tmp60}, compiler::TNode<UintPtrT>{tmp81}));
    DataViewBuiltinsAssembler(state_).StoreWord8(compiler::TNode<RawPtrT>{tmp64}, compiler::TNode<UintPtrT>{tmp82}, compiler::TNode<Uint32T>{tmp67});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 611);
    compiler::TNode<UintPtrT> tmp83;
    USE(tmp83);
    tmp83 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(6));
    compiler::TNode<UintPtrT> tmp84;
    USE(tmp84);
    tmp84 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrAdd(compiler::TNode<UintPtrT>{tmp60}, compiler::TNode<UintPtrT>{tmp83}));
    DataViewBuiltinsAssembler(state_).StoreWord8(compiler::TNode<RawPtrT>{tmp64}, compiler::TNode<UintPtrT>{tmp84}, compiler::TNode<Uint32T>{tmp66});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 612);
    compiler::TNode<UintPtrT> tmp85;
    USE(tmp85);
    tmp85 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(7));
    compiler::TNode<UintPtrT> tmp86;
    USE(tmp86);
    tmp86 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrAdd(compiler::TNode<UintPtrT>{tmp60}, compiler::TNode<UintPtrT>{tmp85}));
    DataViewBuiltinsAssembler(state_).StoreWord8(compiler::TNode<RawPtrT>{tmp64}, compiler::TNode<UintPtrT>{tmp86}, compiler::TNode<Uint32T>{tmp65});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 595);
    ca_.Goto(&block4, tmp59, tmp60, tmp61, tmp62, tmp63, tmp64, tmp65, tmp66, tmp67, tmp68, tmp69, tmp70, tmp71, tmp72);
  }

  if (block4.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp87;
    compiler::TNode<UintPtrT> tmp88;
    compiler::TNode<Uint32T> tmp89;
    compiler::TNode<Uint32T> tmp90;
    compiler::TNode<BoolT> tmp91;
    compiler::TNode<RawPtrT> tmp92;
    compiler::TNode<Uint32T> tmp93;
    compiler::TNode<Uint32T> tmp94;
    compiler::TNode<Uint32T> tmp95;
    compiler::TNode<Uint32T> tmp96;
    compiler::TNode<Uint32T> tmp97;
    compiler::TNode<Uint32T> tmp98;
    compiler::TNode<Uint32T> tmp99;
    compiler::TNode<Uint32T> tmp100;
    ca_.Bind(&block4, &tmp87, &tmp88, &tmp89, &tmp90, &tmp91, &tmp92, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97, &tmp98, &tmp99, &tmp100);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 582);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 580);
    ca_.Goto(&block1, tmp87, tmp88, tmp89, tmp90, tmp91);
  }

  if (block1.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp101;
    compiler::TNode<UintPtrT> tmp102;
    compiler::TNode<Uint32T> tmp103;
    compiler::TNode<Uint32T> tmp104;
    compiler::TNode<BoolT> tmp105;
    ca_.Bind(&block1, &tmp101, &tmp102, &tmp103, &tmp104, &tmp105);
    ca_.Goto(&block5, tmp101, tmp102, tmp103, tmp104, tmp105);
  }

    compiler::TNode<JSArrayBuffer> tmp106;
    compiler::TNode<UintPtrT> tmp107;
    compiler::TNode<Uint32T> tmp108;
    compiler::TNode<Uint32T> tmp109;
    compiler::TNode<BoolT> tmp110;
    ca_.Bind(&block5, &tmp106, &tmp107, &tmp108, &tmp109, &tmp110);
}

void DataViewBuiltinsFromDSLAssembler::StoreDataViewBigInt(compiler::TNode<JSArrayBuffer> p_buffer, compiler::TNode<UintPtrT> p_offset, compiler::TNode<BigInt> p_bigIntValue, compiler::TNode<BoolT> p_requestedLittleEndian) {
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, BigInt, BoolT> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, BigInt, BoolT, Uint32T, Uint32T, Uint32T, Uint32T> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, BigInt, BoolT, Uint32T, Uint32T, Uint32T, Uint32T> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, BigInt, BoolT, Uint32T, Uint32T, Uint32T, Uint32T> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, BigInt, BoolT, Uint32T, Uint32T, Uint32T, Uint32T> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, BigInt, BoolT, Uint32T, Uint32T, Uint32T, Uint32T> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, BigInt, BoolT, Uint32T, Uint32T, Uint32T, Uint32T> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, BigInt, BoolT, Uint32T, Uint32T, Uint32T, Uint32T> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, BigInt, BoolT, Uint32T, Uint32T, Uint32T, Uint32T> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, BigInt, BoolT, Uint32T, Uint32T, Uint32T, Uint32T> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, BigInt, BoolT, Uint32T, Uint32T, Uint32T, Uint32T> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, BigInt, BoolT, Uint32T, Uint32T, Uint32T, Uint32T> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, BigInt, BoolT> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<JSArrayBuffer, UintPtrT, BigInt, BoolT> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_buffer, p_offset, p_bigIntValue, p_requestedLittleEndian);

  if (block0.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp0;
    compiler::TNode<UintPtrT> tmp1;
    compiler::TNode<BigInt> tmp2;
    compiler::TNode<BoolT> tmp3;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 628);
    compiler::TNode<Uint32T> tmp4;
    USE(tmp4);
    tmp4 = ca_.UncheckedCast<Uint32T>(DataViewBuiltinsAssembler(state_).DataViewDecodeBigIntLength(compiler::TNode<BigInt>{tmp2}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 629);
    compiler::TNode<Uint32T> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<Uint32T>(DataViewBuiltinsAssembler(state_).DataViewDecodeBigIntSign(compiler::TNode<BigInt>{tmp2}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 633);
    compiler::TNode<Uint32T> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 634);
    compiler::TNode<Uint32T> tmp7;
    USE(tmp7);
    tmp7 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(0));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 637);
    compiler::TNode<Uint32T> tmp8;
    USE(tmp8);
    tmp8 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp9;
    USE(tmp9);
    tmp9 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32NotEqual(compiler::TNode<Uint32T>{tmp4}, compiler::TNode<Uint32T>{tmp8}));
    ca_.Branch(tmp9, &block2, &block3, tmp0, tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7);
  }

  if (block2.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp10;
    compiler::TNode<UintPtrT> tmp11;
    compiler::TNode<BigInt> tmp12;
    compiler::TNode<BoolT> tmp13;
    compiler::TNode<Uint32T> tmp14;
    compiler::TNode<Uint32T> tmp15;
    compiler::TNode<Uint32T> tmp16;
    compiler::TNode<Uint32T> tmp17;
    ca_.Bind(&block2, &tmp10, &tmp11, &tmp12, &tmp13, &tmp14, &tmp15, &tmp16, &tmp17);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 638);
    if (((CodeStubAssembler(state_).Is64()))) {
      ca_.Goto(&block4, tmp10, tmp11, tmp12, tmp13, tmp14, tmp15, tmp16, tmp17);
    } else {
      ca_.Goto(&block5, tmp10, tmp11, tmp12, tmp13, tmp14, tmp15, tmp16, tmp17);
    }
  }

  if (block4.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp18;
    compiler::TNode<UintPtrT> tmp19;
    compiler::TNode<BigInt> tmp20;
    compiler::TNode<BoolT> tmp21;
    compiler::TNode<Uint32T> tmp22;
    compiler::TNode<Uint32T> tmp23;
    compiler::TNode<Uint32T> tmp24;
    compiler::TNode<Uint32T> tmp25;
    ca_.Bind(&block4, &tmp18, &tmp19, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 640);
    compiler::TNode<UintPtrT> tmp26;
    USE(tmp26);
    tmp26 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).LoadBigIntDigit(compiler::TNode<BigInt>{tmp20}, 0));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 641);
    compiler::TNode<Uint32T> tmp27;
    USE(tmp27);
    tmp27 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATuint329ATuintptr(compiler::TNode<UintPtrT>{tmp26}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 642);
    compiler::TNode<UintPtrT> tmp28;
    USE(tmp28);
    tmp28 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATuintptr17ATconstexpr_int31(32));
    compiler::TNode<UintPtrT> tmp29;
    USE(tmp29);
    tmp29 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).WordShr(compiler::TNode<UintPtrT>{tmp26}, compiler::TNode<UintPtrT>{tmp28}));
    compiler::TNode<Uint32T> tmp30;
    USE(tmp30);
    tmp30 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATuint329ATuintptr(compiler::TNode<UintPtrT>{tmp29}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 638);
    ca_.Goto(&block6, tmp18, tmp19, tmp20, tmp21, tmp22, tmp23, tmp27, tmp30);
  }

  if (block5.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp31;
    compiler::TNode<UintPtrT> tmp32;
    compiler::TNode<BigInt> tmp33;
    compiler::TNode<BoolT> tmp34;
    compiler::TNode<Uint32T> tmp35;
    compiler::TNode<Uint32T> tmp36;
    compiler::TNode<Uint32T> tmp37;
    compiler::TNode<Uint32T> tmp38;
    ca_.Bind(&block5, &tmp31, &tmp32, &tmp33, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 644);
    compiler::TNode<UintPtrT> tmp39;
    USE(tmp39);
    tmp39 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).LoadBigIntDigit(compiler::TNode<BigInt>{tmp33}, 0));
    compiler::TNode<Uint32T> tmp40;
    USE(tmp40);
    tmp40 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATuint329ATuintptr(compiler::TNode<UintPtrT>{tmp39}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 645);
    compiler::TNode<Uint32T> tmp41;
    USE(tmp41);
    tmp41 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(2));
    compiler::TNode<BoolT> tmp42;
    USE(tmp42);
    tmp42 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Uint32GreaterThanOrEqual(compiler::TNode<Uint32T>{tmp35}, compiler::TNode<Uint32T>{tmp41}));
    ca_.Branch(tmp42, &block7, &block8, tmp31, tmp32, tmp33, tmp34, tmp35, tmp36, tmp40, tmp38);
  }

  if (block7.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp43;
    compiler::TNode<UintPtrT> tmp44;
    compiler::TNode<BigInt> tmp45;
    compiler::TNode<BoolT> tmp46;
    compiler::TNode<Uint32T> tmp47;
    compiler::TNode<Uint32T> tmp48;
    compiler::TNode<Uint32T> tmp49;
    compiler::TNode<Uint32T> tmp50;
    ca_.Bind(&block7, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 646);
    compiler::TNode<UintPtrT> tmp51;
    USE(tmp51);
    tmp51 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).LoadBigIntDigit(compiler::TNode<BigInt>{tmp45}, 1));
    compiler::TNode<Uint32T> tmp52;
    USE(tmp52);
    tmp52 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).Convert8ATuint329ATuintptr(compiler::TNode<UintPtrT>{tmp51}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 645);
    ca_.Goto(&block8, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48, tmp49, tmp52);
  }

  if (block8.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp53;
    compiler::TNode<UintPtrT> tmp54;
    compiler::TNode<BigInt> tmp55;
    compiler::TNode<BoolT> tmp56;
    compiler::TNode<Uint32T> tmp57;
    compiler::TNode<Uint32T> tmp58;
    compiler::TNode<Uint32T> tmp59;
    compiler::TNode<Uint32T> tmp60;
    ca_.Bind(&block8, &tmp53, &tmp54, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 638);
    ca_.Goto(&block6, tmp53, tmp54, tmp55, tmp56, tmp57, tmp58, tmp59, tmp60);
  }

  if (block6.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp61;
    compiler::TNode<UintPtrT> tmp62;
    compiler::TNode<BigInt> tmp63;
    compiler::TNode<BoolT> tmp64;
    compiler::TNode<Uint32T> tmp65;
    compiler::TNode<Uint32T> tmp66;
    compiler::TNode<Uint32T> tmp67;
    compiler::TNode<Uint32T> tmp68;
    ca_.Bind(&block6, &tmp61, &tmp62, &tmp63, &tmp64, &tmp65, &tmp66, &tmp67, &tmp68);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 637);
    ca_.Goto(&block3, tmp61, tmp62, tmp63, tmp64, tmp65, tmp66, tmp67, tmp68);
  }

  if (block3.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp69;
    compiler::TNode<UintPtrT> tmp70;
    compiler::TNode<BigInt> tmp71;
    compiler::TNode<BoolT> tmp72;
    compiler::TNode<Uint32T> tmp73;
    compiler::TNode<Uint32T> tmp74;
    compiler::TNode<Uint32T> tmp75;
    compiler::TNode<Uint32T> tmp76;
    ca_.Bind(&block3, &tmp69, &tmp70, &tmp71, &tmp72, &tmp73, &tmp74, &tmp75, &tmp76);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 651);
    compiler::TNode<Uint32T> tmp77;
    USE(tmp77);
    tmp77 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp78;
    USE(tmp78);
    tmp78 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32NotEqual(compiler::TNode<Uint32T>{tmp74}, compiler::TNode<Uint32T>{tmp77}));
    ca_.Branch(tmp78, &block9, &block10, tmp69, tmp70, tmp71, tmp72, tmp73, tmp74, tmp75, tmp76);
  }

  if (block9.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp79;
    compiler::TNode<UintPtrT> tmp80;
    compiler::TNode<BigInt> tmp81;
    compiler::TNode<BoolT> tmp82;
    compiler::TNode<Uint32T> tmp83;
    compiler::TNode<Uint32T> tmp84;
    compiler::TNode<Uint32T> tmp85;
    compiler::TNode<Uint32T> tmp86;
    ca_.Bind(&block9, &tmp79, &tmp80, &tmp81, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 652);
    compiler::TNode<Int32T> tmp87;
    USE(tmp87);
    tmp87 = ca_.UncheckedCast<Int32T>(CodeStubAssembler(state_).Signed(compiler::TNode<Uint32T>{tmp86}));
    compiler::TNode<Int32T> tmp88;
    USE(tmp88);
    tmp88 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr7ATint3217ATconstexpr_int31(0));
    compiler::TNode<Int32T> tmp89;
    USE(tmp89);
    tmp89 = ca_.UncheckedCast<Int32T>(CodeStubAssembler(state_).Int32Sub(compiler::TNode<Int32T>{tmp88}, compiler::TNode<Int32T>{tmp87}));
    compiler::TNode<Uint32T> tmp90;
    USE(tmp90);
    tmp90 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Unsigned(compiler::TNode<Int32T>{tmp89}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 653);
    compiler::TNode<Uint32T> tmp91;
    USE(tmp91);
    tmp91 = ca_.UncheckedCast<Uint32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATuint3217ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp92;
    USE(tmp92);
    tmp92 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Word32NotEqual(compiler::TNode<Uint32T>{tmp85}, compiler::TNode<Uint32T>{tmp91}));
    ca_.Branch(tmp92, &block11, &block12, tmp79, tmp80, tmp81, tmp82, tmp83, tmp84, tmp85, tmp90);
  }

  if (block11.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp93;
    compiler::TNode<UintPtrT> tmp94;
    compiler::TNode<BigInt> tmp95;
    compiler::TNode<BoolT> tmp96;
    compiler::TNode<Uint32T> tmp97;
    compiler::TNode<Uint32T> tmp98;
    compiler::TNode<Uint32T> tmp99;
    compiler::TNode<Uint32T> tmp100;
    ca_.Bind(&block11, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97, &tmp98, &tmp99, &tmp100);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 654);
    compiler::TNode<Int32T> tmp101;
    USE(tmp101);
    tmp101 = ca_.UncheckedCast<Int32T>(CodeStubAssembler(state_).Signed(compiler::TNode<Uint32T>{tmp100}));
    compiler::TNode<Int32T> tmp102;
    USE(tmp102);
    tmp102 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr7ATint3217ATconstexpr_int31(1));
    compiler::TNode<Int32T> tmp103;
    USE(tmp103);
    tmp103 = ca_.UncheckedCast<Int32T>(CodeStubAssembler(state_).Int32Sub(compiler::TNode<Int32T>{tmp101}, compiler::TNode<Int32T>{tmp102}));
    compiler::TNode<Uint32T> tmp104;
    USE(tmp104);
    tmp104 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Unsigned(compiler::TNode<Int32T>{tmp103}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 653);
    ca_.Goto(&block12, tmp93, tmp94, tmp95, tmp96, tmp97, tmp98, tmp99, tmp104);
  }

  if (block12.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp105;
    compiler::TNode<UintPtrT> tmp106;
    compiler::TNode<BigInt> tmp107;
    compiler::TNode<BoolT> tmp108;
    compiler::TNode<Uint32T> tmp109;
    compiler::TNode<Uint32T> tmp110;
    compiler::TNode<Uint32T> tmp111;
    compiler::TNode<Uint32T> tmp112;
    ca_.Bind(&block12, &tmp105, &tmp106, &tmp107, &tmp108, &tmp109, &tmp110, &tmp111, &tmp112);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 656);
    compiler::TNode<Int32T> tmp113;
    USE(tmp113);
    tmp113 = ca_.UncheckedCast<Int32T>(CodeStubAssembler(state_).Signed(compiler::TNode<Uint32T>{tmp111}));
    compiler::TNode<Int32T> tmp114;
    USE(tmp114);
    tmp114 = ca_.UncheckedCast<Int32T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr7ATint3217ATconstexpr_int31(0));
    compiler::TNode<Int32T> tmp115;
    USE(tmp115);
    tmp115 = ca_.UncheckedCast<Int32T>(CodeStubAssembler(state_).Int32Sub(compiler::TNode<Int32T>{tmp114}, compiler::TNode<Int32T>{tmp113}));
    compiler::TNode<Uint32T> tmp116;
    USE(tmp116);
    tmp116 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Unsigned(compiler::TNode<Int32T>{tmp115}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 651);
    ca_.Goto(&block10, tmp105, tmp106, tmp107, tmp108, tmp109, tmp110, tmp116, tmp112);
  }

  if (block10.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp117;
    compiler::TNode<UintPtrT> tmp118;
    compiler::TNode<BigInt> tmp119;
    compiler::TNode<BoolT> tmp120;
    compiler::TNode<Uint32T> tmp121;
    compiler::TNode<Uint32T> tmp122;
    compiler::TNode<Uint32T> tmp123;
    compiler::TNode<Uint32T> tmp124;
    ca_.Bind(&block10, &tmp117, &tmp118, &tmp119, &tmp120, &tmp121, &tmp122, &tmp123, &tmp124);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 659);
    DataViewBuiltinsFromDSLAssembler(state_).StoreDataView64(compiler::TNode<JSArrayBuffer>{tmp117}, compiler::TNode<UintPtrT>{tmp118}, compiler::TNode<Uint32T>{tmp123}, compiler::TNode<Uint32T>{tmp124}, compiler::TNode<BoolT>{tmp120});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 627);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 625);
    ca_.Goto(&block1, tmp117, tmp118, tmp119, tmp120);
  }

  if (block1.is_used()) {
    compiler::TNode<JSArrayBuffer> tmp125;
    compiler::TNode<UintPtrT> tmp126;
    compiler::TNode<BigInt> tmp127;
    compiler::TNode<BoolT> tmp128;
    ca_.Bind(&block1, &tmp125, &tmp126, &tmp127, &tmp128);
    ca_.Goto(&block13, tmp125, tmp126, tmp127, tmp128);
  }

    compiler::TNode<JSArrayBuffer> tmp129;
    compiler::TNode<UintPtrT> tmp130;
    compiler::TNode<BigInt> tmp131;
    compiler::TNode<BoolT> tmp132;
    ca_.Bind(&block13, &tmp129, &tmp130, &tmp131, &tmp132);
}

compiler::TNode<Object> DataViewBuiltinsFromDSLAssembler::DataViewSet(compiler::TNode<Context> p_context, compiler::TNode<Object> p_receiver, compiler::TNode<Object> p_offset, compiler::TNode<Object> p_value, compiler::TNode<Object> p_requestedLittleEndian, ElementsKind p_kind) {
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object> block0(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, JSDataView, Number, Object, Context> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, JSDataView, Number, Object, Context, Number> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, JSDataView, Number> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, JSDataView, Number> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, JSDataView, Number, BoolT, JSArrayBuffer> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, JSDataView, Number, BoolT, JSArrayBuffer, BigInt> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, JSDataView, Number, BoolT, JSArrayBuffer, BigInt> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, JSDataView, Number, BoolT, JSArrayBuffer, BigInt, Float64T, UintPtrT, UintPtrT, Float64T, Float64T> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, JSDataView, Number, BoolT, JSArrayBuffer, BigInt, Float64T, UintPtrT, UintPtrT, Float64T, Float64T> block13(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, JSDataView, Number, BoolT, JSArrayBuffer> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, JSDataView, Number, BoolT, JSArrayBuffer, Number> block14(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, JSDataView, Number, BoolT, JSArrayBuffer, Number> block15(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, JSDataView, Number, BoolT, JSArrayBuffer, Number, Float64T, UintPtrT, UintPtrT, Float64T, Float64T> block16(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, JSDataView, Number, BoolT, JSArrayBuffer, Number, Float64T, UintPtrT, UintPtrT, Float64T, Float64T> block17(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, JSDataView, Number, BoolT, JSArrayBuffer, Number, Float64T, UintPtrT, UintPtrT, Float64T, Float64T, UintPtrT, Float64T> block19(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, JSDataView, Number, BoolT, JSArrayBuffer, Number, Float64T, UintPtrT, UintPtrT, Float64T, Float64T, UintPtrT, Float64T> block20(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, JSDataView, Number, BoolT, JSArrayBuffer, Number, Float64T, UintPtrT, UintPtrT, Float64T, Float64T, UintPtrT, Float64T> block23(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, JSDataView, Number, BoolT, JSArrayBuffer, Number, Float64T, UintPtrT, UintPtrT, Float64T, Float64T, UintPtrT, Float64T> block24(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, JSDataView, Number, BoolT, JSArrayBuffer, Number, Float64T, UintPtrT, UintPtrT, Float64T, Float64T, UintPtrT, Float64T> block27(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, JSDataView, Number, BoolT, JSArrayBuffer, Number, Float64T, UintPtrT, UintPtrT, Float64T, Float64T, UintPtrT, Float64T> block28(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, JSDataView, Number, BoolT, JSArrayBuffer, Number, Float64T, UintPtrT, UintPtrT, Float64T, Float64T, UintPtrT, Float64T> block30(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, JSDataView, Number, BoolT, JSArrayBuffer, Number, Float64T, UintPtrT, UintPtrT, Float64T, Float64T, UintPtrT, Float64T> block31(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, JSDataView, Number, BoolT, JSArrayBuffer, Number, Float64T, UintPtrT, UintPtrT, Float64T, Float64T, UintPtrT, Float64T> block33(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, JSDataView, Number, BoolT, JSArrayBuffer, Number, Float64T, UintPtrT, UintPtrT, Float64T, Float64T, UintPtrT, Float64T> block34(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, JSDataView, Number, BoolT, JSArrayBuffer, Number, Float64T, UintPtrT, UintPtrT, Float64T, Float64T, UintPtrT, Float64T> block35(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, JSDataView, Number, BoolT, JSArrayBuffer, Number, Float64T, UintPtrT, UintPtrT, Float64T, Float64T, UintPtrT, Float64T> block32(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, JSDataView, Number, BoolT, JSArrayBuffer, Number, Float64T, UintPtrT, UintPtrT, Float64T, Float64T, UintPtrT, Float64T> block29(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, JSDataView, Number, BoolT, JSArrayBuffer, Number, Float64T, UintPtrT, UintPtrT, Float64T, Float64T, UintPtrT, Float64T> block25(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, JSDataView, Number, BoolT, JSArrayBuffer, Number, Float64T, UintPtrT, UintPtrT, Float64T, Float64T, UintPtrT, Float64T> block21(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, JSDataView, Number, BoolT, JSArrayBuffer> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, Object, Object, Object, Object> block36(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, p_context, p_receiver, p_offset, p_value, p_requestedLittleEndian);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<Object> tmp2;
    compiler::TNode<Object> tmp3;
    compiler::TNode<Object> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 666);
    compiler::TNode<String> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<String>(DataViewBuiltinsFromDSLAssembler(state_).MakeDataViewSetterNameString(p_kind));
    compiler::TNode<JSDataView> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<JSDataView>(DataViewBuiltinsFromDSLAssembler(state_).ValidateDataView(compiler::TNode<Context>{tmp0}, compiler::TNode<Object>{tmp1}, compiler::TNode<String>{tmp5}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 665);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 668);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 670);
    compiler::TNode<Number> tmp7;
    USE(tmp7);
    compiler::CodeAssemblerLabel label0(&ca_);
    tmp7 = BaseBuiltinsFromDSLAssembler(state_).ToIndex(compiler::TNode<Object>{tmp2}, compiler::TNode<Context>{tmp0}, &label0);
    ca_.Goto(&block4, tmp0, tmp1, tmp2, tmp3, tmp4, tmp6, ca_.Uninitialized<Number>(), tmp2, tmp0, tmp7);
    if (label0.is_used()) {
      ca_.Bind(&label0);
      ca_.Goto(&block5, tmp0, tmp1, tmp2, tmp3, tmp4, tmp6, ca_.Uninitialized<Number>(), tmp2, tmp0);
    }
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp8;
    compiler::TNode<Object> tmp9;
    compiler::TNode<Object> tmp10;
    compiler::TNode<Object> tmp11;
    compiler::TNode<Object> tmp12;
    compiler::TNode<JSDataView> tmp13;
    compiler::TNode<Number> tmp14;
    compiler::TNode<Object> tmp15;
    compiler::TNode<Context> tmp16;
    ca_.Bind(&block5, &tmp8, &tmp9, &tmp10, &tmp11, &tmp12, &tmp13, &tmp14, &tmp15, &tmp16);
    ca_.Goto(&block3, tmp8, tmp9, tmp10, tmp11, tmp12, tmp13, tmp14);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp17;
    compiler::TNode<Object> tmp18;
    compiler::TNode<Object> tmp19;
    compiler::TNode<Object> tmp20;
    compiler::TNode<Object> tmp21;
    compiler::TNode<JSDataView> tmp22;
    compiler::TNode<Number> tmp23;
    compiler::TNode<Object> tmp24;
    compiler::TNode<Context> tmp25;
    compiler::TNode<Number> tmp26;
    ca_.Bind(&block4, &tmp17, &tmp18, &tmp19, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25, &tmp26);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 672);
    ca_.Goto(&block2, tmp17, tmp18, tmp19, tmp20, tmp21, tmp22, tmp26);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp27;
    compiler::TNode<Object> tmp28;
    compiler::TNode<Object> tmp29;
    compiler::TNode<Object> tmp30;
    compiler::TNode<Object> tmp31;
    compiler::TNode<JSDataView> tmp32;
    compiler::TNode<Number> tmp33;
    ca_.Bind(&block3, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31, &tmp32, &tmp33);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 673);
    CodeStubAssembler(state_).ThrowRangeError(compiler::TNode<Context>{tmp27}, MessageTemplate::kInvalidDataViewAccessorOffset);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp34;
    compiler::TNode<Object> tmp35;
    compiler::TNode<Object> tmp36;
    compiler::TNode<Object> tmp37;
    compiler::TNode<Object> tmp38;
    compiler::TNode<JSDataView> tmp39;
    compiler::TNode<Number> tmp40;
    ca_.Bind(&block2, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39, &tmp40);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 676);
    compiler::TNode<BoolT> tmp41;
    USE(tmp41);
    tmp41 = ca_.UncheckedCast<BoolT>(BaseBuiltinsFromDSLAssembler(state_).ToBoolean(compiler::TNode<Object>{tmp38}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 677);
    compiler::TNode<IntPtrT> tmp42 = ca_.IntPtrConstant(JSArrayBufferView::kBufferOffset);
    USE(tmp42);
    compiler::TNode<JSArrayBuffer>tmp43 = CodeStubAssembler(state_).LoadReference<JSArrayBuffer>(CodeStubAssembler::Reference{tmp39, tmp42});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 681);
    if ((((CodeStubAssembler(state_).ElementsKindEqual(p_kind, BIGUINT64_ELEMENTS)) || (CodeStubAssembler(state_).ElementsKindEqual(p_kind, BIGINT64_ELEMENTS))))) {
      ca_.Goto(&block7, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39, tmp40, tmp41, tmp43);
    } else {
      ca_.Goto(&block8, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39, tmp40, tmp41, tmp43);
    }
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp44;
    compiler::TNode<Object> tmp45;
    compiler::TNode<Object> tmp46;
    compiler::TNode<Object> tmp47;
    compiler::TNode<Object> tmp48;
    compiler::TNode<JSDataView> tmp49;
    compiler::TNode<Number> tmp50;
    compiler::TNode<BoolT> tmp51;
    compiler::TNode<JSArrayBuffer> tmp52;
    ca_.Bind(&block7, &tmp44, &tmp45, &tmp46, &tmp47, &tmp48, &tmp49, &tmp50, &tmp51, &tmp52);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 682);
    compiler::TNode<BigInt> tmp53;
    USE(tmp53);
    tmp53 = ca_.UncheckedCast<BigInt>(CodeStubAssembler(state_).ToBigInt(compiler::TNode<Context>{tmp44}, compiler::TNode<Object>{tmp47}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 684);
    compiler::TNode<BoolT> tmp54;
    USE(tmp54);
    tmp54 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsDetachedBuffer(compiler::TNode<JSArrayBuffer>{tmp52}));
    ca_.Branch(tmp54, &block10, &block11, tmp44, tmp45, tmp46, tmp47, tmp48, tmp49, tmp50, tmp51, tmp52, tmp53);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp55;
    compiler::TNode<Object> tmp56;
    compiler::TNode<Object> tmp57;
    compiler::TNode<Object> tmp58;
    compiler::TNode<Object> tmp59;
    compiler::TNode<JSDataView> tmp60;
    compiler::TNode<Number> tmp61;
    compiler::TNode<BoolT> tmp62;
    compiler::TNode<JSArrayBuffer> tmp63;
    compiler::TNode<BigInt> tmp64;
    ca_.Bind(&block10, &tmp55, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62, &tmp63, &tmp64);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 685);
    compiler::TNode<String> tmp65;
    USE(tmp65);
    tmp65 = ca_.UncheckedCast<String>(DataViewBuiltinsFromDSLAssembler(state_).MakeDataViewSetterNameString(p_kind));
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp55}, MessageTemplate::kDetachedOperation, compiler::TNode<Object>{tmp65});
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp66;
    compiler::TNode<Object> tmp67;
    compiler::TNode<Object> tmp68;
    compiler::TNode<Object> tmp69;
    compiler::TNode<Object> tmp70;
    compiler::TNode<JSDataView> tmp71;
    compiler::TNode<Number> tmp72;
    compiler::TNode<BoolT> tmp73;
    compiler::TNode<JSArrayBuffer> tmp74;
    compiler::TNode<BigInt> tmp75;
    ca_.Bind(&block11, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71, &tmp72, &tmp73, &tmp74, &tmp75);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 688);
    compiler::TNode<Float64T> tmp76;
    USE(tmp76);
    tmp76 = ca_.UncheckedCast<Float64T>(BaseBuiltinsFromDSLAssembler(state_).Convert9ATfloat6420UT5ATSmi10HeapNumber(compiler::TNode<Number>{tmp72}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 689);
    compiler::TNode<UintPtrT> tmp77;
    USE(tmp77);
    tmp77 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert9ATuintptr9ATfloat64(compiler::TNode<Float64T>{tmp76}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 691);
    compiler::TNode<IntPtrT> tmp78 = ca_.IntPtrConstant(JSArrayBufferView::kByteOffsetOffset);
    USE(tmp78);
    compiler::TNode<UintPtrT>tmp79 = CodeStubAssembler(state_).LoadReference<UintPtrT>(CodeStubAssembler::Reference{tmp71, tmp78});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 692);
    compiler::TNode<IntPtrT> tmp80 = ca_.IntPtrConstant(JSArrayBufferView::kByteLengthOffset);
    USE(tmp80);
    compiler::TNode<UintPtrT>tmp81 = CodeStubAssembler(state_).LoadReference<UintPtrT>(CodeStubAssembler::Reference{tmp71, tmp80});
    compiler::TNode<Float64T> tmp82;
    USE(tmp82);
    tmp82 = ca_.UncheckedCast<Float64T>(BaseBuiltinsFromDSLAssembler(state_).Convert9ATfloat649ATuintptr(compiler::TNode<UintPtrT>{tmp81}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 693);
    compiler::TNode<Float64T> tmp83;
    USE(tmp83);
    tmp83 = ca_.UncheckedCast<Float64T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATfloat6417ATconstexpr_int31((DataViewBuiltinsAssembler(state_).DataViewElementSize(p_kind))));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 695);
    compiler::TNode<Float64T> tmp84;
    USE(tmp84);
    tmp84 = ca_.UncheckedCast<Float64T>(CodeStubAssembler(state_).Float64Add(compiler::TNode<Float64T>{tmp76}, compiler::TNode<Float64T>{tmp83}));
    compiler::TNode<BoolT> tmp85;
    USE(tmp85);
    tmp85 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Float64GreaterThan(compiler::TNode<Float64T>{tmp84}, compiler::TNode<Float64T>{tmp82}));
    ca_.Branch(tmp85, &block12, &block13, tmp66, tmp67, tmp68, tmp69, tmp70, tmp71, tmp72, tmp73, tmp74, tmp75, tmp76, tmp77, tmp79, tmp82, tmp83);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp86;
    compiler::TNode<Object> tmp87;
    compiler::TNode<Object> tmp88;
    compiler::TNode<Object> tmp89;
    compiler::TNode<Object> tmp90;
    compiler::TNode<JSDataView> tmp91;
    compiler::TNode<Number> tmp92;
    compiler::TNode<BoolT> tmp93;
    compiler::TNode<JSArrayBuffer> tmp94;
    compiler::TNode<BigInt> tmp95;
    compiler::TNode<Float64T> tmp96;
    compiler::TNode<UintPtrT> tmp97;
    compiler::TNode<UintPtrT> tmp98;
    compiler::TNode<Float64T> tmp99;
    compiler::TNode<Float64T> tmp100;
    ca_.Bind(&block12, &tmp86, &tmp87, &tmp88, &tmp89, &tmp90, &tmp91, &tmp92, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97, &tmp98, &tmp99, &tmp100);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 696);
    CodeStubAssembler(state_).ThrowRangeError(compiler::TNode<Context>{tmp86}, MessageTemplate::kInvalidDataViewAccessorOffset);
  }

  if (block13.is_used()) {
    compiler::TNode<Context> tmp101;
    compiler::TNode<Object> tmp102;
    compiler::TNode<Object> tmp103;
    compiler::TNode<Object> tmp104;
    compiler::TNode<Object> tmp105;
    compiler::TNode<JSDataView> tmp106;
    compiler::TNode<Number> tmp107;
    compiler::TNode<BoolT> tmp108;
    compiler::TNode<JSArrayBuffer> tmp109;
    compiler::TNode<BigInt> tmp110;
    compiler::TNode<Float64T> tmp111;
    compiler::TNode<UintPtrT> tmp112;
    compiler::TNode<UintPtrT> tmp113;
    compiler::TNode<Float64T> tmp114;
    compiler::TNode<Float64T> tmp115;
    ca_.Bind(&block13, &tmp101, &tmp102, &tmp103, &tmp104, &tmp105, &tmp106, &tmp107, &tmp108, &tmp109, &tmp110, &tmp111, &tmp112, &tmp113, &tmp114, &tmp115);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 699);
    compiler::TNode<UintPtrT> tmp116;
    USE(tmp116);
    tmp116 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrAdd(compiler::TNode<UintPtrT>{tmp112}, compiler::TNode<UintPtrT>{tmp113}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 700);
    DataViewBuiltinsFromDSLAssembler(state_).StoreDataViewBigInt(compiler::TNode<JSArrayBuffer>{tmp109}, compiler::TNode<UintPtrT>{tmp116}, compiler::TNode<BigInt>{tmp110}, compiler::TNode<BoolT>{tmp108});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 681);
    ca_.Goto(&block9, tmp101, tmp102, tmp103, tmp104, tmp105, tmp106, tmp107, tmp108, tmp109);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp117;
    compiler::TNode<Object> tmp118;
    compiler::TNode<Object> tmp119;
    compiler::TNode<Object> tmp120;
    compiler::TNode<Object> tmp121;
    compiler::TNode<JSDataView> tmp122;
    compiler::TNode<Number> tmp123;
    compiler::TNode<BoolT> tmp124;
    compiler::TNode<JSArrayBuffer> tmp125;
    ca_.Bind(&block8, &tmp117, &tmp118, &tmp119, &tmp120, &tmp121, &tmp122, &tmp123, &tmp124, &tmp125);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 702);
    compiler::TNode<Number> tmp126;
    USE(tmp126);
    tmp126 = ca_.UncheckedCast<Number>(CodeStubAssembler(state_).ToNumber(compiler::TNode<Context>{tmp117}, compiler::TNode<Object>{tmp120}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 704);
    compiler::TNode<BoolT> tmp127;
    USE(tmp127);
    tmp127 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IsDetachedBuffer(compiler::TNode<JSArrayBuffer>{tmp125}));
    ca_.Branch(tmp127, &block14, &block15, tmp117, tmp118, tmp119, tmp120, tmp121, tmp122, tmp123, tmp124, tmp125, tmp126);
  }

  if (block14.is_used()) {
    compiler::TNode<Context> tmp128;
    compiler::TNode<Object> tmp129;
    compiler::TNode<Object> tmp130;
    compiler::TNode<Object> tmp131;
    compiler::TNode<Object> tmp132;
    compiler::TNode<JSDataView> tmp133;
    compiler::TNode<Number> tmp134;
    compiler::TNode<BoolT> tmp135;
    compiler::TNode<JSArrayBuffer> tmp136;
    compiler::TNode<Number> tmp137;
    ca_.Bind(&block14, &tmp128, &tmp129, &tmp130, &tmp131, &tmp132, &tmp133, &tmp134, &tmp135, &tmp136, &tmp137);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 705);
    compiler::TNode<String> tmp138;
    USE(tmp138);
    tmp138 = ca_.UncheckedCast<String>(DataViewBuiltinsFromDSLAssembler(state_).MakeDataViewSetterNameString(p_kind));
    CodeStubAssembler(state_).ThrowTypeError(compiler::TNode<Context>{tmp128}, MessageTemplate::kDetachedOperation, compiler::TNode<Object>{tmp138});
  }

  if (block15.is_used()) {
    compiler::TNode<Context> tmp139;
    compiler::TNode<Object> tmp140;
    compiler::TNode<Object> tmp141;
    compiler::TNode<Object> tmp142;
    compiler::TNode<Object> tmp143;
    compiler::TNode<JSDataView> tmp144;
    compiler::TNode<Number> tmp145;
    compiler::TNode<BoolT> tmp146;
    compiler::TNode<JSArrayBuffer> tmp147;
    compiler::TNode<Number> tmp148;
    ca_.Bind(&block15, &tmp139, &tmp140, &tmp141, &tmp142, &tmp143, &tmp144, &tmp145, &tmp146, &tmp147, &tmp148);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 708);
    compiler::TNode<Float64T> tmp149;
    USE(tmp149);
    tmp149 = ca_.UncheckedCast<Float64T>(BaseBuiltinsFromDSLAssembler(state_).Convert9ATfloat6420UT5ATSmi10HeapNumber(compiler::TNode<Number>{tmp145}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 709);
    compiler::TNode<UintPtrT> tmp150;
    USE(tmp150);
    tmp150 = ca_.UncheckedCast<UintPtrT>(BaseBuiltinsFromDSLAssembler(state_).Convert9ATuintptr9ATfloat64(compiler::TNode<Float64T>{tmp149}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 711);
    compiler::TNode<IntPtrT> tmp151 = ca_.IntPtrConstant(JSArrayBufferView::kByteOffsetOffset);
    USE(tmp151);
    compiler::TNode<UintPtrT>tmp152 = CodeStubAssembler(state_).LoadReference<UintPtrT>(CodeStubAssembler::Reference{tmp144, tmp151});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 712);
    compiler::TNode<IntPtrT> tmp153 = ca_.IntPtrConstant(JSArrayBufferView::kByteLengthOffset);
    USE(tmp153);
    compiler::TNode<UintPtrT>tmp154 = CodeStubAssembler(state_).LoadReference<UintPtrT>(CodeStubAssembler::Reference{tmp144, tmp153});
    compiler::TNode<Float64T> tmp155;
    USE(tmp155);
    tmp155 = ca_.UncheckedCast<Float64T>(BaseBuiltinsFromDSLAssembler(state_).Convert9ATfloat649ATuintptr(compiler::TNode<UintPtrT>{tmp154}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 713);
    compiler::TNode<Float64T> tmp156;
    USE(tmp156);
    tmp156 = ca_.UncheckedCast<Float64T>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr9ATfloat6417ATconstexpr_int31((DataViewBuiltinsAssembler(state_).DataViewElementSize(p_kind))));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 715);
    compiler::TNode<Float64T> tmp157;
    USE(tmp157);
    tmp157 = ca_.UncheckedCast<Float64T>(CodeStubAssembler(state_).Float64Add(compiler::TNode<Float64T>{tmp149}, compiler::TNode<Float64T>{tmp156}));
    compiler::TNode<BoolT> tmp158;
    USE(tmp158);
    tmp158 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).Float64GreaterThan(compiler::TNode<Float64T>{tmp157}, compiler::TNode<Float64T>{tmp155}));
    ca_.Branch(tmp158, &block16, &block17, tmp139, tmp140, tmp141, tmp142, tmp143, tmp144, tmp145, tmp146, tmp147, tmp148, tmp149, tmp150, tmp152, tmp155, tmp156);
  }

  if (block16.is_used()) {
    compiler::TNode<Context> tmp159;
    compiler::TNode<Object> tmp160;
    compiler::TNode<Object> tmp161;
    compiler::TNode<Object> tmp162;
    compiler::TNode<Object> tmp163;
    compiler::TNode<JSDataView> tmp164;
    compiler::TNode<Number> tmp165;
    compiler::TNode<BoolT> tmp166;
    compiler::TNode<JSArrayBuffer> tmp167;
    compiler::TNode<Number> tmp168;
    compiler::TNode<Float64T> tmp169;
    compiler::TNode<UintPtrT> tmp170;
    compiler::TNode<UintPtrT> tmp171;
    compiler::TNode<Float64T> tmp172;
    compiler::TNode<Float64T> tmp173;
    ca_.Bind(&block16, &tmp159, &tmp160, &tmp161, &tmp162, &tmp163, &tmp164, &tmp165, &tmp166, &tmp167, &tmp168, &tmp169, &tmp170, &tmp171, &tmp172, &tmp173);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 716);
    CodeStubAssembler(state_).ThrowRangeError(compiler::TNode<Context>{tmp159}, MessageTemplate::kInvalidDataViewAccessorOffset);
  }

  if (block17.is_used()) {
    compiler::TNode<Context> tmp174;
    compiler::TNode<Object> tmp175;
    compiler::TNode<Object> tmp176;
    compiler::TNode<Object> tmp177;
    compiler::TNode<Object> tmp178;
    compiler::TNode<JSDataView> tmp179;
    compiler::TNode<Number> tmp180;
    compiler::TNode<BoolT> tmp181;
    compiler::TNode<JSArrayBuffer> tmp182;
    compiler::TNode<Number> tmp183;
    compiler::TNode<Float64T> tmp184;
    compiler::TNode<UintPtrT> tmp185;
    compiler::TNode<UintPtrT> tmp186;
    compiler::TNode<Float64T> tmp187;
    compiler::TNode<Float64T> tmp188;
    ca_.Bind(&block17, &tmp174, &tmp175, &tmp176, &tmp177, &tmp178, &tmp179, &tmp180, &tmp181, &tmp182, &tmp183, &tmp184, &tmp185, &tmp186, &tmp187, &tmp188);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 719);
    compiler::TNode<UintPtrT> tmp189;
    USE(tmp189);
    tmp189 = ca_.UncheckedCast<UintPtrT>(CodeStubAssembler(state_).UintPtrAdd(compiler::TNode<UintPtrT>{tmp185}, compiler::TNode<UintPtrT>{tmp186}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 721);
    compiler::TNode<Float64T> tmp190;
    USE(tmp190);
    tmp190 = ca_.UncheckedCast<Float64T>(CodeStubAssembler(state_).ChangeNumberToFloat64(compiler::TNode<Number>{tmp183}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 723);
    if ((((CodeStubAssembler(state_).ElementsKindEqual(p_kind, UINT8_ELEMENTS)) || (CodeStubAssembler(state_).ElementsKindEqual(p_kind, INT8_ELEMENTS))))) {
      ca_.Goto(&block19, tmp174, tmp175, tmp176, tmp177, tmp178, tmp179, tmp180, tmp181, tmp182, tmp183, tmp184, tmp185, tmp186, tmp187, tmp188, tmp189, tmp190);
    } else {
      ca_.Goto(&block20, tmp174, tmp175, tmp176, tmp177, tmp178, tmp179, tmp180, tmp181, tmp182, tmp183, tmp184, tmp185, tmp186, tmp187, tmp188, tmp189, tmp190);
    }
  }

  if (block19.is_used()) {
    compiler::TNode<Context> tmp191;
    compiler::TNode<Object> tmp192;
    compiler::TNode<Object> tmp193;
    compiler::TNode<Object> tmp194;
    compiler::TNode<Object> tmp195;
    compiler::TNode<JSDataView> tmp196;
    compiler::TNode<Number> tmp197;
    compiler::TNode<BoolT> tmp198;
    compiler::TNode<JSArrayBuffer> tmp199;
    compiler::TNode<Number> tmp200;
    compiler::TNode<Float64T> tmp201;
    compiler::TNode<UintPtrT> tmp202;
    compiler::TNode<UintPtrT> tmp203;
    compiler::TNode<Float64T> tmp204;
    compiler::TNode<Float64T> tmp205;
    compiler::TNode<UintPtrT> tmp206;
    compiler::TNode<Float64T> tmp207;
    ca_.Bind(&block19, &tmp191, &tmp192, &tmp193, &tmp194, &tmp195, &tmp196, &tmp197, &tmp198, &tmp199, &tmp200, &tmp201, &tmp202, &tmp203, &tmp204, &tmp205, &tmp206, &tmp207);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 725);
    compiler::TNode<Uint32T> tmp208;
    USE(tmp208);
    tmp208 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).TruncateFloat64ToWord32(compiler::TNode<Float64T>{tmp207}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 724);
    DataViewBuiltinsFromDSLAssembler(state_).StoreDataView8(compiler::TNode<JSArrayBuffer>{tmp199}, compiler::TNode<UintPtrT>{tmp206}, compiler::TNode<Uint32T>{tmp208});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 723);
    ca_.Goto(&block21, tmp191, tmp192, tmp193, tmp194, tmp195, tmp196, tmp197, tmp198, tmp199, tmp200, tmp201, tmp202, tmp203, tmp204, tmp205, tmp206, tmp207);
  }

  if (block20.is_used()) {
    compiler::TNode<Context> tmp209;
    compiler::TNode<Object> tmp210;
    compiler::TNode<Object> tmp211;
    compiler::TNode<Object> tmp212;
    compiler::TNode<Object> tmp213;
    compiler::TNode<JSDataView> tmp214;
    compiler::TNode<Number> tmp215;
    compiler::TNode<BoolT> tmp216;
    compiler::TNode<JSArrayBuffer> tmp217;
    compiler::TNode<Number> tmp218;
    compiler::TNode<Float64T> tmp219;
    compiler::TNode<UintPtrT> tmp220;
    compiler::TNode<UintPtrT> tmp221;
    compiler::TNode<Float64T> tmp222;
    compiler::TNode<Float64T> tmp223;
    compiler::TNode<UintPtrT> tmp224;
    compiler::TNode<Float64T> tmp225;
    ca_.Bind(&block20, &tmp209, &tmp210, &tmp211, &tmp212, &tmp213, &tmp214, &tmp215, &tmp216, &tmp217, &tmp218, &tmp219, &tmp220, &tmp221, &tmp222, &tmp223, &tmp224, &tmp225);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 726);
    if ((((CodeStubAssembler(state_).ElementsKindEqual(p_kind, UINT16_ELEMENTS)) || (CodeStubAssembler(state_).ElementsKindEqual(p_kind, INT16_ELEMENTS))))) {
      ca_.Goto(&block23, tmp209, tmp210, tmp211, tmp212, tmp213, tmp214, tmp215, tmp216, tmp217, tmp218, tmp219, tmp220, tmp221, tmp222, tmp223, tmp224, tmp225);
    } else {
      ca_.Goto(&block24, tmp209, tmp210, tmp211, tmp212, tmp213, tmp214, tmp215, tmp216, tmp217, tmp218, tmp219, tmp220, tmp221, tmp222, tmp223, tmp224, tmp225);
    }
  }

  if (block23.is_used()) {
    compiler::TNode<Context> tmp226;
    compiler::TNode<Object> tmp227;
    compiler::TNode<Object> tmp228;
    compiler::TNode<Object> tmp229;
    compiler::TNode<Object> tmp230;
    compiler::TNode<JSDataView> tmp231;
    compiler::TNode<Number> tmp232;
    compiler::TNode<BoolT> tmp233;
    compiler::TNode<JSArrayBuffer> tmp234;
    compiler::TNode<Number> tmp235;
    compiler::TNode<Float64T> tmp236;
    compiler::TNode<UintPtrT> tmp237;
    compiler::TNode<UintPtrT> tmp238;
    compiler::TNode<Float64T> tmp239;
    compiler::TNode<Float64T> tmp240;
    compiler::TNode<UintPtrT> tmp241;
    compiler::TNode<Float64T> tmp242;
    ca_.Bind(&block23, &tmp226, &tmp227, &tmp228, &tmp229, &tmp230, &tmp231, &tmp232, &tmp233, &tmp234, &tmp235, &tmp236, &tmp237, &tmp238, &tmp239, &tmp240, &tmp241, &tmp242);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 728);
    compiler::TNode<Uint32T> tmp243;
    USE(tmp243);
    tmp243 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).TruncateFloat64ToWord32(compiler::TNode<Float64T>{tmp242}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 729);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 727);
    DataViewBuiltinsFromDSLAssembler(state_).StoreDataView16(compiler::TNode<JSArrayBuffer>{tmp234}, compiler::TNode<UintPtrT>{tmp241}, compiler::TNode<Uint32T>{tmp243}, compiler::TNode<BoolT>{tmp233});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 726);
    ca_.Goto(&block25, tmp226, tmp227, tmp228, tmp229, tmp230, tmp231, tmp232, tmp233, tmp234, tmp235, tmp236, tmp237, tmp238, tmp239, tmp240, tmp241, tmp242);
  }

  if (block24.is_used()) {
    compiler::TNode<Context> tmp244;
    compiler::TNode<Object> tmp245;
    compiler::TNode<Object> tmp246;
    compiler::TNode<Object> tmp247;
    compiler::TNode<Object> tmp248;
    compiler::TNode<JSDataView> tmp249;
    compiler::TNode<Number> tmp250;
    compiler::TNode<BoolT> tmp251;
    compiler::TNode<JSArrayBuffer> tmp252;
    compiler::TNode<Number> tmp253;
    compiler::TNode<Float64T> tmp254;
    compiler::TNode<UintPtrT> tmp255;
    compiler::TNode<UintPtrT> tmp256;
    compiler::TNode<Float64T> tmp257;
    compiler::TNode<Float64T> tmp258;
    compiler::TNode<UintPtrT> tmp259;
    compiler::TNode<Float64T> tmp260;
    ca_.Bind(&block24, &tmp244, &tmp245, &tmp246, &tmp247, &tmp248, &tmp249, &tmp250, &tmp251, &tmp252, &tmp253, &tmp254, &tmp255, &tmp256, &tmp257, &tmp258, &tmp259, &tmp260);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 730);
    if ((((CodeStubAssembler(state_).ElementsKindEqual(p_kind, UINT32_ELEMENTS)) || (CodeStubAssembler(state_).ElementsKindEqual(p_kind, INT32_ELEMENTS))))) {
      ca_.Goto(&block27, tmp244, tmp245, tmp246, tmp247, tmp248, tmp249, tmp250, tmp251, tmp252, tmp253, tmp254, tmp255, tmp256, tmp257, tmp258, tmp259, tmp260);
    } else {
      ca_.Goto(&block28, tmp244, tmp245, tmp246, tmp247, tmp248, tmp249, tmp250, tmp251, tmp252, tmp253, tmp254, tmp255, tmp256, tmp257, tmp258, tmp259, tmp260);
    }
  }

  if (block27.is_used()) {
    compiler::TNode<Context> tmp261;
    compiler::TNode<Object> tmp262;
    compiler::TNode<Object> tmp263;
    compiler::TNode<Object> tmp264;
    compiler::TNode<Object> tmp265;
    compiler::TNode<JSDataView> tmp266;
    compiler::TNode<Number> tmp267;
    compiler::TNode<BoolT> tmp268;
    compiler::TNode<JSArrayBuffer> tmp269;
    compiler::TNode<Number> tmp270;
    compiler::TNode<Float64T> tmp271;
    compiler::TNode<UintPtrT> tmp272;
    compiler::TNode<UintPtrT> tmp273;
    compiler::TNode<Float64T> tmp274;
    compiler::TNode<Float64T> tmp275;
    compiler::TNode<UintPtrT> tmp276;
    compiler::TNode<Float64T> tmp277;
    ca_.Bind(&block27, &tmp261, &tmp262, &tmp263, &tmp264, &tmp265, &tmp266, &tmp267, &tmp268, &tmp269, &tmp270, &tmp271, &tmp272, &tmp273, &tmp274, &tmp275, &tmp276, &tmp277);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 732);
    compiler::TNode<Uint32T> tmp278;
    USE(tmp278);
    tmp278 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).TruncateFloat64ToWord32(compiler::TNode<Float64T>{tmp277}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 733);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 731);
    DataViewBuiltinsFromDSLAssembler(state_).StoreDataView32(compiler::TNode<JSArrayBuffer>{tmp269}, compiler::TNode<UintPtrT>{tmp276}, compiler::TNode<Uint32T>{tmp278}, compiler::TNode<BoolT>{tmp268});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 730);
    ca_.Goto(&block29, tmp261, tmp262, tmp263, tmp264, tmp265, tmp266, tmp267, tmp268, tmp269, tmp270, tmp271, tmp272, tmp273, tmp274, tmp275, tmp276, tmp277);
  }

  if (block28.is_used()) {
    compiler::TNode<Context> tmp279;
    compiler::TNode<Object> tmp280;
    compiler::TNode<Object> tmp281;
    compiler::TNode<Object> tmp282;
    compiler::TNode<Object> tmp283;
    compiler::TNode<JSDataView> tmp284;
    compiler::TNode<Number> tmp285;
    compiler::TNode<BoolT> tmp286;
    compiler::TNode<JSArrayBuffer> tmp287;
    compiler::TNode<Number> tmp288;
    compiler::TNode<Float64T> tmp289;
    compiler::TNode<UintPtrT> tmp290;
    compiler::TNode<UintPtrT> tmp291;
    compiler::TNode<Float64T> tmp292;
    compiler::TNode<Float64T> tmp293;
    compiler::TNode<UintPtrT> tmp294;
    compiler::TNode<Float64T> tmp295;
    ca_.Bind(&block28, &tmp279, &tmp280, &tmp281, &tmp282, &tmp283, &tmp284, &tmp285, &tmp286, &tmp287, &tmp288, &tmp289, &tmp290, &tmp291, &tmp292, &tmp293, &tmp294, &tmp295);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 734);
    if (((CodeStubAssembler(state_).ElementsKindEqual(p_kind, FLOAT32_ELEMENTS)))) {
      ca_.Goto(&block30, tmp279, tmp280, tmp281, tmp282, tmp283, tmp284, tmp285, tmp286, tmp287, tmp288, tmp289, tmp290, tmp291, tmp292, tmp293, tmp294, tmp295);
    } else {
      ca_.Goto(&block31, tmp279, tmp280, tmp281, tmp282, tmp283, tmp284, tmp285, tmp286, tmp287, tmp288, tmp289, tmp290, tmp291, tmp292, tmp293, tmp294, tmp295);
    }
  }

  if (block30.is_used()) {
    compiler::TNode<Context> tmp296;
    compiler::TNode<Object> tmp297;
    compiler::TNode<Object> tmp298;
    compiler::TNode<Object> tmp299;
    compiler::TNode<Object> tmp300;
    compiler::TNode<JSDataView> tmp301;
    compiler::TNode<Number> tmp302;
    compiler::TNode<BoolT> tmp303;
    compiler::TNode<JSArrayBuffer> tmp304;
    compiler::TNode<Number> tmp305;
    compiler::TNode<Float64T> tmp306;
    compiler::TNode<UintPtrT> tmp307;
    compiler::TNode<UintPtrT> tmp308;
    compiler::TNode<Float64T> tmp309;
    compiler::TNode<Float64T> tmp310;
    compiler::TNode<UintPtrT> tmp311;
    compiler::TNode<Float64T> tmp312;
    ca_.Bind(&block30, &tmp296, &tmp297, &tmp298, &tmp299, &tmp300, &tmp301, &tmp302, &tmp303, &tmp304, &tmp305, &tmp306, &tmp307, &tmp308, &tmp309, &tmp310, &tmp311, &tmp312);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 735);
    compiler::TNode<Float32T> tmp313;
    USE(tmp313);
    tmp313 = ca_.UncheckedCast<Float32T>(CodeStubAssembler(state_).TruncateFloat64ToFloat32(compiler::TNode<Float64T>{tmp312}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 737);
    compiler::TNode<Uint32T> tmp314;
    USE(tmp314);
    tmp314 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).BitcastFloat32ToInt32(compiler::TNode<Float32T>{tmp313}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 738);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 736);
    DataViewBuiltinsFromDSLAssembler(state_).StoreDataView32(compiler::TNode<JSArrayBuffer>{tmp304}, compiler::TNode<UintPtrT>{tmp311}, compiler::TNode<Uint32T>{tmp314}, compiler::TNode<BoolT>{tmp303});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 734);
    ca_.Goto(&block32, tmp296, tmp297, tmp298, tmp299, tmp300, tmp301, tmp302, tmp303, tmp304, tmp305, tmp306, tmp307, tmp308, tmp309, tmp310, tmp311, tmp312);
  }

  if (block31.is_used()) {
    compiler::TNode<Context> tmp315;
    compiler::TNode<Object> tmp316;
    compiler::TNode<Object> tmp317;
    compiler::TNode<Object> tmp318;
    compiler::TNode<Object> tmp319;
    compiler::TNode<JSDataView> tmp320;
    compiler::TNode<Number> tmp321;
    compiler::TNode<BoolT> tmp322;
    compiler::TNode<JSArrayBuffer> tmp323;
    compiler::TNode<Number> tmp324;
    compiler::TNode<Float64T> tmp325;
    compiler::TNode<UintPtrT> tmp326;
    compiler::TNode<UintPtrT> tmp327;
    compiler::TNode<Float64T> tmp328;
    compiler::TNode<Float64T> tmp329;
    compiler::TNode<UintPtrT> tmp330;
    compiler::TNode<Float64T> tmp331;
    ca_.Bind(&block31, &tmp315, &tmp316, &tmp317, &tmp318, &tmp319, &tmp320, &tmp321, &tmp322, &tmp323, &tmp324, &tmp325, &tmp326, &tmp327, &tmp328, &tmp329, &tmp330, &tmp331);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 739);
    if (((CodeStubAssembler(state_).ElementsKindEqual(p_kind, FLOAT64_ELEMENTS)))) {
      ca_.Goto(&block33, tmp315, tmp316, tmp317, tmp318, tmp319, tmp320, tmp321, tmp322, tmp323, tmp324, tmp325, tmp326, tmp327, tmp328, tmp329, tmp330, tmp331);
    } else {
      ca_.Goto(&block34, tmp315, tmp316, tmp317, tmp318, tmp319, tmp320, tmp321, tmp322, tmp323, tmp324, tmp325, tmp326, tmp327, tmp328, tmp329, tmp330, tmp331);
    }
  }

  if (block33.is_used()) {
    compiler::TNode<Context> tmp332;
    compiler::TNode<Object> tmp333;
    compiler::TNode<Object> tmp334;
    compiler::TNode<Object> tmp335;
    compiler::TNode<Object> tmp336;
    compiler::TNode<JSDataView> tmp337;
    compiler::TNode<Number> tmp338;
    compiler::TNode<BoolT> tmp339;
    compiler::TNode<JSArrayBuffer> tmp340;
    compiler::TNode<Number> tmp341;
    compiler::TNode<Float64T> tmp342;
    compiler::TNode<UintPtrT> tmp343;
    compiler::TNode<UintPtrT> tmp344;
    compiler::TNode<Float64T> tmp345;
    compiler::TNode<Float64T> tmp346;
    compiler::TNode<UintPtrT> tmp347;
    compiler::TNode<Float64T> tmp348;
    ca_.Bind(&block33, &tmp332, &tmp333, &tmp334, &tmp335, &tmp336, &tmp337, &tmp338, &tmp339, &tmp340, &tmp341, &tmp342, &tmp343, &tmp344, &tmp345, &tmp346, &tmp347, &tmp348);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 740);
    compiler::TNode<Uint32T> tmp349;
    USE(tmp349);
    tmp349 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Float64ExtractLowWord32(compiler::TNode<Float64T>{tmp348}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 741);
    compiler::TNode<Uint32T> tmp350;
    USE(tmp350);
    tmp350 = ca_.UncheckedCast<Uint32T>(CodeStubAssembler(state_).Float64ExtractHighWord32(compiler::TNode<Float64T>{tmp348}));
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 742);
    DataViewBuiltinsFromDSLAssembler(state_).StoreDataView64(compiler::TNode<JSArrayBuffer>{tmp340}, compiler::TNode<UintPtrT>{tmp347}, compiler::TNode<Uint32T>{tmp349}, compiler::TNode<Uint32T>{tmp350}, compiler::TNode<BoolT>{tmp339});
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 739);
    ca_.Goto(&block35, tmp332, tmp333, tmp334, tmp335, tmp336, tmp337, tmp338, tmp339, tmp340, tmp341, tmp342, tmp343, tmp344, tmp345, tmp346, tmp347, tmp348);
  }

  if (block34.is_used()) {
    compiler::TNode<Context> tmp351;
    compiler::TNode<Object> tmp352;
    compiler::TNode<Object> tmp353;
    compiler::TNode<Object> tmp354;
    compiler::TNode<Object> tmp355;
    compiler::TNode<JSDataView> tmp356;
    compiler::TNode<Number> tmp357;
    compiler::TNode<BoolT> tmp358;
    compiler::TNode<JSArrayBuffer> tmp359;
    compiler::TNode<Number> tmp360;
    compiler::TNode<Float64T> tmp361;
    compiler::TNode<UintPtrT> tmp362;
    compiler::TNode<UintPtrT> tmp363;
    compiler::TNode<Float64T> tmp364;
    compiler::TNode<Float64T> tmp365;
    compiler::TNode<UintPtrT> tmp366;
    compiler::TNode<Float64T> tmp367;
    ca_.Bind(&block34, &tmp351, &tmp352, &tmp353, &tmp354, &tmp355, &tmp356, &tmp357, &tmp358, &tmp359, &tmp360, &tmp361, &tmp362, &tmp363, &tmp364, &tmp365, &tmp366, &tmp367);
    ca_.Goto(&block35, tmp351, tmp352, tmp353, tmp354, tmp355, tmp356, tmp357, tmp358, tmp359, tmp360, tmp361, tmp362, tmp363, tmp364, tmp365, tmp366, tmp367);
  }

  if (block35.is_used()) {
    compiler::TNode<Context> tmp368;
    compiler::TNode<Object> tmp369;
    compiler::TNode<Object> tmp370;
    compiler::TNode<Object> tmp371;
    compiler::TNode<Object> tmp372;
    compiler::TNode<JSDataView> tmp373;
    compiler::TNode<Number> tmp374;
    compiler::TNode<BoolT> tmp375;
    compiler::TNode<JSArrayBuffer> tmp376;
    compiler::TNode<Number> tmp377;
    compiler::TNode<Float64T> tmp378;
    compiler::TNode<UintPtrT> tmp379;
    compiler::TNode<UintPtrT> tmp380;
    compiler::TNode<Float64T> tmp381;
    compiler::TNode<Float64T> tmp382;
    compiler::TNode<UintPtrT> tmp383;
    compiler::TNode<Float64T> tmp384;
    ca_.Bind(&block35, &tmp368, &tmp369, &tmp370, &tmp371, &tmp372, &tmp373, &tmp374, &tmp375, &tmp376, &tmp377, &tmp378, &tmp379, &tmp380, &tmp381, &tmp382, &tmp383, &tmp384);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 734);
    ca_.Goto(&block32, tmp368, tmp369, tmp370, tmp371, tmp372, tmp373, tmp374, tmp375, tmp376, tmp377, tmp378, tmp379, tmp380, tmp381, tmp382, tmp383, tmp384);
  }

  if (block32.is_used()) {
    compiler::TNode<Context> tmp385;
    compiler::TNode<Object> tmp386;
    compiler::TNode<Object> tmp387;
    compiler::TNode<Object> tmp388;
    compiler::TNode<Object> tmp389;
    compiler::TNode<JSDataView> tmp390;
    compiler::TNode<Number> tmp391;
    compiler::TNode<BoolT> tmp392;
    compiler::TNode<JSArrayBuffer> tmp393;
    compiler::TNode<Number> tmp394;
    compiler::TNode<Float64T> tmp395;
    compiler::TNode<UintPtrT> tmp396;
    compiler::TNode<UintPtrT> tmp397;
    compiler::TNode<Float64T> tmp398;
    compiler::TNode<Float64T> tmp399;
    compiler::TNode<UintPtrT> tmp400;
    compiler::TNode<Float64T> tmp401;
    ca_.Bind(&block32, &tmp385, &tmp386, &tmp387, &tmp388, &tmp389, &tmp390, &tmp391, &tmp392, &tmp393, &tmp394, &tmp395, &tmp396, &tmp397, &tmp398, &tmp399, &tmp400, &tmp401);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 730);
    ca_.Goto(&block29, tmp385, tmp386, tmp387, tmp388, tmp389, tmp390, tmp391, tmp392, tmp393, tmp394, tmp395, tmp396, tmp397, tmp398, tmp399, tmp400, tmp401);
  }

  if (block29.is_used()) {
    compiler::TNode<Context> tmp402;
    compiler::TNode<Object> tmp403;
    compiler::TNode<Object> tmp404;
    compiler::TNode<Object> tmp405;
    compiler::TNode<Object> tmp406;
    compiler::TNode<JSDataView> tmp407;
    compiler::TNode<Number> tmp408;
    compiler::TNode<BoolT> tmp409;
    compiler::TNode<JSArrayBuffer> tmp410;
    compiler::TNode<Number> tmp411;
    compiler::TNode<Float64T> tmp412;
    compiler::TNode<UintPtrT> tmp413;
    compiler::TNode<UintPtrT> tmp414;
    compiler::TNode<Float64T> tmp415;
    compiler::TNode<Float64T> tmp416;
    compiler::TNode<UintPtrT> tmp417;
    compiler::TNode<Float64T> tmp418;
    ca_.Bind(&block29, &tmp402, &tmp403, &tmp404, &tmp405, &tmp406, &tmp407, &tmp408, &tmp409, &tmp410, &tmp411, &tmp412, &tmp413, &tmp414, &tmp415, &tmp416, &tmp417, &tmp418);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 726);
    ca_.Goto(&block25, tmp402, tmp403, tmp404, tmp405, tmp406, tmp407, tmp408, tmp409, tmp410, tmp411, tmp412, tmp413, tmp414, tmp415, tmp416, tmp417, tmp418);
  }

  if (block25.is_used()) {
    compiler::TNode<Context> tmp419;
    compiler::TNode<Object> tmp420;
    compiler::TNode<Object> tmp421;
    compiler::TNode<Object> tmp422;
    compiler::TNode<Object> tmp423;
    compiler::TNode<JSDataView> tmp424;
    compiler::TNode<Number> tmp425;
    compiler::TNode<BoolT> tmp426;
    compiler::TNode<JSArrayBuffer> tmp427;
    compiler::TNode<Number> tmp428;
    compiler::TNode<Float64T> tmp429;
    compiler::TNode<UintPtrT> tmp430;
    compiler::TNode<UintPtrT> tmp431;
    compiler::TNode<Float64T> tmp432;
    compiler::TNode<Float64T> tmp433;
    compiler::TNode<UintPtrT> tmp434;
    compiler::TNode<Float64T> tmp435;
    ca_.Bind(&block25, &tmp419, &tmp420, &tmp421, &tmp422, &tmp423, &tmp424, &tmp425, &tmp426, &tmp427, &tmp428, &tmp429, &tmp430, &tmp431, &tmp432, &tmp433, &tmp434, &tmp435);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 723);
    ca_.Goto(&block21, tmp419, tmp420, tmp421, tmp422, tmp423, tmp424, tmp425, tmp426, tmp427, tmp428, tmp429, tmp430, tmp431, tmp432, tmp433, tmp434, tmp435);
  }

  if (block21.is_used()) {
    compiler::TNode<Context> tmp436;
    compiler::TNode<Object> tmp437;
    compiler::TNode<Object> tmp438;
    compiler::TNode<Object> tmp439;
    compiler::TNode<Object> tmp440;
    compiler::TNode<JSDataView> tmp441;
    compiler::TNode<Number> tmp442;
    compiler::TNode<BoolT> tmp443;
    compiler::TNode<JSArrayBuffer> tmp444;
    compiler::TNode<Number> tmp445;
    compiler::TNode<Float64T> tmp446;
    compiler::TNode<UintPtrT> tmp447;
    compiler::TNode<UintPtrT> tmp448;
    compiler::TNode<Float64T> tmp449;
    compiler::TNode<Float64T> tmp450;
    compiler::TNode<UintPtrT> tmp451;
    compiler::TNode<Float64T> tmp452;
    ca_.Bind(&block21, &tmp436, &tmp437, &tmp438, &tmp439, &tmp440, &tmp441, &tmp442, &tmp443, &tmp444, &tmp445, &tmp446, &tmp447, &tmp448, &tmp449, &tmp450, &tmp451, &tmp452);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 701);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 681);
    ca_.Goto(&block9, tmp436, tmp437, tmp438, tmp439, tmp440, tmp441, tmp442, tmp443, tmp444);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp453;
    compiler::TNode<Object> tmp454;
    compiler::TNode<Object> tmp455;
    compiler::TNode<Object> tmp456;
    compiler::TNode<Object> tmp457;
    compiler::TNode<JSDataView> tmp458;
    compiler::TNode<Number> tmp459;
    compiler::TNode<BoolT> tmp460;
    compiler::TNode<JSArrayBuffer> tmp461;
    ca_.Bind(&block9, &tmp453, &tmp454, &tmp455, &tmp456, &tmp457, &tmp458, &tmp459, &tmp460, &tmp461);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 745);
    compiler::TNode<Oddball> tmp462;
    USE(tmp462);
    tmp462 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block1, tmp453, tmp454, tmp455, tmp456, tmp457, tmp462);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp463;
    compiler::TNode<Object> tmp464;
    compiler::TNode<Object> tmp465;
    compiler::TNode<Object> tmp466;
    compiler::TNode<Object> tmp467;
    compiler::TNode<Object> tmp468;
    ca_.Bind(&block1, &tmp463, &tmp464, &tmp465, &tmp466, &tmp467, &tmp468);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 662);
    ca_.Goto(&block36, tmp463, tmp464, tmp465, tmp466, tmp467, tmp468);
  }

    compiler::TNode<Context> tmp469;
    compiler::TNode<Object> tmp470;
    compiler::TNode<Object> tmp471;
    compiler::TNode<Object> tmp472;
    compiler::TNode<Object> tmp473;
    compiler::TNode<Object> tmp474;
    ca_.Bind(&block36, &tmp469, &tmp470, &tmp471, &tmp472, &tmp473, &tmp474);
  return compiler::TNode<Object>{tmp474};
}

TF_BUILTIN(DataViewPrototypeSetUint8, CodeStubAssembler) {
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
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, torque_arguments.frame, torque_arguments.base, torque_arguments.length);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<RawPtrT> tmp2;
    compiler::TNode<RawPtrT> tmp3;
    compiler::TNode<IntPtrT> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 750);
    compiler::TNode<IntPtrT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp4}, compiler::TNode<IntPtrT>{tmp5}));
    ca_.Branch(tmp6, &block1, &block2, tmp0, tmp1, tmp2, tmp3, tmp4);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<Object> tmp8;
    compiler::TNode<RawPtrT> tmp9;
    compiler::TNode<RawPtrT> tmp10;
    compiler::TNode<IntPtrT> tmp11;
    ca_.Bind(&block1, &tmp7, &tmp8, &tmp9, &tmp10, &tmp11);
    compiler::TNode<IntPtrT> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp9}, compiler::TNode<RawPtrT>{tmp10}, compiler::TNode<IntPtrT>{tmp11}}, compiler::TNode<IntPtrT>{tmp12}));
    ca_.Goto(&block4, tmp7, tmp8, tmp9, tmp10, tmp11, tmp13);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp14;
    compiler::TNode<Object> tmp15;
    compiler::TNode<RawPtrT> tmp16;
    compiler::TNode<RawPtrT> tmp17;
    compiler::TNode<IntPtrT> tmp18;
    ca_.Bind(&block2, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18);
    compiler::TNode<Oddball> tmp19;
    USE(tmp19);
    tmp19 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block3, tmp14, tmp15, tmp16, tmp17, tmp18, tmp19);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp20;
    compiler::TNode<Object> tmp21;
    compiler::TNode<RawPtrT> tmp22;
    compiler::TNode<RawPtrT> tmp23;
    compiler::TNode<IntPtrT> tmp24;
    compiler::TNode<Object> tmp25;
    ca_.Bind(&block4, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25);
    ca_.Goto(&block3, tmp20, tmp21, tmp22, tmp23, tmp24, tmp25);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp26;
    compiler::TNode<Object> tmp27;
    compiler::TNode<RawPtrT> tmp28;
    compiler::TNode<RawPtrT> tmp29;
    compiler::TNode<IntPtrT> tmp30;
    compiler::TNode<Object> tmp31;
    ca_.Bind(&block3, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 751);
    compiler::TNode<IntPtrT> tmp32;
    USE(tmp32);
    tmp32 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<BoolT> tmp33;
    USE(tmp33);
    tmp33 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp30}, compiler::TNode<IntPtrT>{tmp32}));
    ca_.Branch(tmp33, &block5, &block6, tmp26, tmp27, tmp28, tmp29, tmp30, tmp31);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp34;
    compiler::TNode<Object> tmp35;
    compiler::TNode<RawPtrT> tmp36;
    compiler::TNode<RawPtrT> tmp37;
    compiler::TNode<IntPtrT> tmp38;
    compiler::TNode<Object> tmp39;
    ca_.Bind(&block5, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39);
    compiler::TNode<IntPtrT> tmp40;
    USE(tmp40);
    tmp40 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<Object> tmp41;
    USE(tmp41);
    tmp41 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp36}, compiler::TNode<RawPtrT>{tmp37}, compiler::TNode<IntPtrT>{tmp38}}, compiler::TNode<IntPtrT>{tmp40}));
    ca_.Goto(&block8, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39, tmp41);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp42;
    compiler::TNode<Object> tmp43;
    compiler::TNode<RawPtrT> tmp44;
    compiler::TNode<RawPtrT> tmp45;
    compiler::TNode<IntPtrT> tmp46;
    compiler::TNode<Object> tmp47;
    ca_.Bind(&block6, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47);
    compiler::TNode<Oddball> tmp48;
    USE(tmp48);
    tmp48 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block7, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp49;
    compiler::TNode<Object> tmp50;
    compiler::TNode<RawPtrT> tmp51;
    compiler::TNode<RawPtrT> tmp52;
    compiler::TNode<IntPtrT> tmp53;
    compiler::TNode<Object> tmp54;
    compiler::TNode<Object> tmp55;
    ca_.Bind(&block8, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55);
    ca_.Goto(&block7, tmp49, tmp50, tmp51, tmp52, tmp53, tmp54, tmp55);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp56;
    compiler::TNode<Object> tmp57;
    compiler::TNode<RawPtrT> tmp58;
    compiler::TNode<RawPtrT> tmp59;
    compiler::TNode<IntPtrT> tmp60;
    compiler::TNode<Object> tmp61;
    compiler::TNode<Object> tmp62;
    ca_.Bind(&block7, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 753);
    compiler::TNode<Oddball> tmp63;
    USE(tmp63);
    tmp63 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 752);
    compiler::TNode<Object> tmp64;
    USE(tmp64);
    tmp64 = ca_.UncheckedCast<Object>(DataViewBuiltinsFromDSLAssembler(state_).DataViewSet(compiler::TNode<Context>{tmp56}, compiler::TNode<Object>{tmp57}, compiler::TNode<Object>{tmp61}, compiler::TNode<Object>{tmp62}, compiler::TNode<Object>{tmp63}, UINT8_ELEMENTS));
    arguments.PopAndReturn(tmp64);
  }
}

TF_BUILTIN(DataViewPrototypeSetInt8, CodeStubAssembler) {
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
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, torque_arguments.frame, torque_arguments.base, torque_arguments.length);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<RawPtrT> tmp2;
    compiler::TNode<RawPtrT> tmp3;
    compiler::TNode<IntPtrT> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 758);
    compiler::TNode<IntPtrT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp4}, compiler::TNode<IntPtrT>{tmp5}));
    ca_.Branch(tmp6, &block1, &block2, tmp0, tmp1, tmp2, tmp3, tmp4);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<Object> tmp8;
    compiler::TNode<RawPtrT> tmp9;
    compiler::TNode<RawPtrT> tmp10;
    compiler::TNode<IntPtrT> tmp11;
    ca_.Bind(&block1, &tmp7, &tmp8, &tmp9, &tmp10, &tmp11);
    compiler::TNode<IntPtrT> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp9}, compiler::TNode<RawPtrT>{tmp10}, compiler::TNode<IntPtrT>{tmp11}}, compiler::TNode<IntPtrT>{tmp12}));
    ca_.Goto(&block4, tmp7, tmp8, tmp9, tmp10, tmp11, tmp13);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp14;
    compiler::TNode<Object> tmp15;
    compiler::TNode<RawPtrT> tmp16;
    compiler::TNode<RawPtrT> tmp17;
    compiler::TNode<IntPtrT> tmp18;
    ca_.Bind(&block2, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18);
    compiler::TNode<Oddball> tmp19;
    USE(tmp19);
    tmp19 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block3, tmp14, tmp15, tmp16, tmp17, tmp18, tmp19);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp20;
    compiler::TNode<Object> tmp21;
    compiler::TNode<RawPtrT> tmp22;
    compiler::TNode<RawPtrT> tmp23;
    compiler::TNode<IntPtrT> tmp24;
    compiler::TNode<Object> tmp25;
    ca_.Bind(&block4, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25);
    ca_.Goto(&block3, tmp20, tmp21, tmp22, tmp23, tmp24, tmp25);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp26;
    compiler::TNode<Object> tmp27;
    compiler::TNode<RawPtrT> tmp28;
    compiler::TNode<RawPtrT> tmp29;
    compiler::TNode<IntPtrT> tmp30;
    compiler::TNode<Object> tmp31;
    ca_.Bind(&block3, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 759);
    compiler::TNode<IntPtrT> tmp32;
    USE(tmp32);
    tmp32 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<BoolT> tmp33;
    USE(tmp33);
    tmp33 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp30}, compiler::TNode<IntPtrT>{tmp32}));
    ca_.Branch(tmp33, &block5, &block6, tmp26, tmp27, tmp28, tmp29, tmp30, tmp31);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp34;
    compiler::TNode<Object> tmp35;
    compiler::TNode<RawPtrT> tmp36;
    compiler::TNode<RawPtrT> tmp37;
    compiler::TNode<IntPtrT> tmp38;
    compiler::TNode<Object> tmp39;
    ca_.Bind(&block5, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39);
    compiler::TNode<IntPtrT> tmp40;
    USE(tmp40);
    tmp40 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<Object> tmp41;
    USE(tmp41);
    tmp41 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp36}, compiler::TNode<RawPtrT>{tmp37}, compiler::TNode<IntPtrT>{tmp38}}, compiler::TNode<IntPtrT>{tmp40}));
    ca_.Goto(&block8, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39, tmp41);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp42;
    compiler::TNode<Object> tmp43;
    compiler::TNode<RawPtrT> tmp44;
    compiler::TNode<RawPtrT> tmp45;
    compiler::TNode<IntPtrT> tmp46;
    compiler::TNode<Object> tmp47;
    ca_.Bind(&block6, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47);
    compiler::TNode<Oddball> tmp48;
    USE(tmp48);
    tmp48 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block7, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp49;
    compiler::TNode<Object> tmp50;
    compiler::TNode<RawPtrT> tmp51;
    compiler::TNode<RawPtrT> tmp52;
    compiler::TNode<IntPtrT> tmp53;
    compiler::TNode<Object> tmp54;
    compiler::TNode<Object> tmp55;
    ca_.Bind(&block8, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55);
    ca_.Goto(&block7, tmp49, tmp50, tmp51, tmp52, tmp53, tmp54, tmp55);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp56;
    compiler::TNode<Object> tmp57;
    compiler::TNode<RawPtrT> tmp58;
    compiler::TNode<RawPtrT> tmp59;
    compiler::TNode<IntPtrT> tmp60;
    compiler::TNode<Object> tmp61;
    compiler::TNode<Object> tmp62;
    ca_.Bind(&block7, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 761);
    compiler::TNode<Oddball> tmp63;
    USE(tmp63);
    tmp63 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 760);
    compiler::TNode<Object> tmp64;
    USE(tmp64);
    tmp64 = ca_.UncheckedCast<Object>(DataViewBuiltinsFromDSLAssembler(state_).DataViewSet(compiler::TNode<Context>{tmp56}, compiler::TNode<Object>{tmp57}, compiler::TNode<Object>{tmp61}, compiler::TNode<Object>{tmp62}, compiler::TNode<Object>{tmp63}, INT8_ELEMENTS));
    arguments.PopAndReturn(tmp64);
  }
}

TF_BUILTIN(DataViewPrototypeSetUint16, CodeStubAssembler) {
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
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, Object> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, Object> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, torque_arguments.frame, torque_arguments.base, torque_arguments.length);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<RawPtrT> tmp2;
    compiler::TNode<RawPtrT> tmp3;
    compiler::TNode<IntPtrT> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 766);
    compiler::TNode<IntPtrT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp4}, compiler::TNode<IntPtrT>{tmp5}));
    ca_.Branch(tmp6, &block1, &block2, tmp0, tmp1, tmp2, tmp3, tmp4);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<Object> tmp8;
    compiler::TNode<RawPtrT> tmp9;
    compiler::TNode<RawPtrT> tmp10;
    compiler::TNode<IntPtrT> tmp11;
    ca_.Bind(&block1, &tmp7, &tmp8, &tmp9, &tmp10, &tmp11);
    compiler::TNode<IntPtrT> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp9}, compiler::TNode<RawPtrT>{tmp10}, compiler::TNode<IntPtrT>{tmp11}}, compiler::TNode<IntPtrT>{tmp12}));
    ca_.Goto(&block4, tmp7, tmp8, tmp9, tmp10, tmp11, tmp13);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp14;
    compiler::TNode<Object> tmp15;
    compiler::TNode<RawPtrT> tmp16;
    compiler::TNode<RawPtrT> tmp17;
    compiler::TNode<IntPtrT> tmp18;
    ca_.Bind(&block2, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18);
    compiler::TNode<Oddball> tmp19;
    USE(tmp19);
    tmp19 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block3, tmp14, tmp15, tmp16, tmp17, tmp18, tmp19);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp20;
    compiler::TNode<Object> tmp21;
    compiler::TNode<RawPtrT> tmp22;
    compiler::TNode<RawPtrT> tmp23;
    compiler::TNode<IntPtrT> tmp24;
    compiler::TNode<Object> tmp25;
    ca_.Bind(&block4, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25);
    ca_.Goto(&block3, tmp20, tmp21, tmp22, tmp23, tmp24, tmp25);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp26;
    compiler::TNode<Object> tmp27;
    compiler::TNode<RawPtrT> tmp28;
    compiler::TNode<RawPtrT> tmp29;
    compiler::TNode<IntPtrT> tmp30;
    compiler::TNode<Object> tmp31;
    ca_.Bind(&block3, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 767);
    compiler::TNode<IntPtrT> tmp32;
    USE(tmp32);
    tmp32 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<BoolT> tmp33;
    USE(tmp33);
    tmp33 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp30}, compiler::TNode<IntPtrT>{tmp32}));
    ca_.Branch(tmp33, &block5, &block6, tmp26, tmp27, tmp28, tmp29, tmp30, tmp31);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp34;
    compiler::TNode<Object> tmp35;
    compiler::TNode<RawPtrT> tmp36;
    compiler::TNode<RawPtrT> tmp37;
    compiler::TNode<IntPtrT> tmp38;
    compiler::TNode<Object> tmp39;
    ca_.Bind(&block5, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39);
    compiler::TNode<IntPtrT> tmp40;
    USE(tmp40);
    tmp40 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<Object> tmp41;
    USE(tmp41);
    tmp41 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp36}, compiler::TNode<RawPtrT>{tmp37}, compiler::TNode<IntPtrT>{tmp38}}, compiler::TNode<IntPtrT>{tmp40}));
    ca_.Goto(&block8, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39, tmp41);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp42;
    compiler::TNode<Object> tmp43;
    compiler::TNode<RawPtrT> tmp44;
    compiler::TNode<RawPtrT> tmp45;
    compiler::TNode<IntPtrT> tmp46;
    compiler::TNode<Object> tmp47;
    ca_.Bind(&block6, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47);
    compiler::TNode<Oddball> tmp48;
    USE(tmp48);
    tmp48 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block7, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp49;
    compiler::TNode<Object> tmp50;
    compiler::TNode<RawPtrT> tmp51;
    compiler::TNode<RawPtrT> tmp52;
    compiler::TNode<IntPtrT> tmp53;
    compiler::TNode<Object> tmp54;
    compiler::TNode<Object> tmp55;
    ca_.Bind(&block8, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55);
    ca_.Goto(&block7, tmp49, tmp50, tmp51, tmp52, tmp53, tmp54, tmp55);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp56;
    compiler::TNode<Object> tmp57;
    compiler::TNode<RawPtrT> tmp58;
    compiler::TNode<RawPtrT> tmp59;
    compiler::TNode<IntPtrT> tmp60;
    compiler::TNode<Object> tmp61;
    compiler::TNode<Object> tmp62;
    ca_.Bind(&block7, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 769);
    compiler::TNode<IntPtrT> tmp63;
    USE(tmp63);
    tmp63 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(2));
    compiler::TNode<BoolT> tmp64;
    USE(tmp64);
    tmp64 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp60}, compiler::TNode<IntPtrT>{tmp63}));
    ca_.Branch(tmp64, &block9, &block10, tmp56, tmp57, tmp58, tmp59, tmp60, tmp61, tmp62);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp65;
    compiler::TNode<Object> tmp66;
    compiler::TNode<RawPtrT> tmp67;
    compiler::TNode<RawPtrT> tmp68;
    compiler::TNode<IntPtrT> tmp69;
    compiler::TNode<Object> tmp70;
    compiler::TNode<Object> tmp71;
    ca_.Bind(&block9, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71);
    compiler::TNode<IntPtrT> tmp72;
    USE(tmp72);
    tmp72 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(2));
    compiler::TNode<Object> tmp73;
    USE(tmp73);
    tmp73 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp67}, compiler::TNode<RawPtrT>{tmp68}, compiler::TNode<IntPtrT>{tmp69}}, compiler::TNode<IntPtrT>{tmp72}));
    ca_.Goto(&block12, tmp65, tmp66, tmp67, tmp68, tmp69, tmp70, tmp71, tmp73);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp74;
    compiler::TNode<Object> tmp75;
    compiler::TNode<RawPtrT> tmp76;
    compiler::TNode<RawPtrT> tmp77;
    compiler::TNode<IntPtrT> tmp78;
    compiler::TNode<Object> tmp79;
    compiler::TNode<Object> tmp80;
    ca_.Bind(&block10, &tmp74, &tmp75, &tmp76, &tmp77, &tmp78, &tmp79, &tmp80);
    compiler::TNode<Oddball> tmp81;
    USE(tmp81);
    tmp81 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block11, tmp74, tmp75, tmp76, tmp77, tmp78, tmp79, tmp80, tmp81);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp82;
    compiler::TNode<Object> tmp83;
    compiler::TNode<RawPtrT> tmp84;
    compiler::TNode<RawPtrT> tmp85;
    compiler::TNode<IntPtrT> tmp86;
    compiler::TNode<Object> tmp87;
    compiler::TNode<Object> tmp88;
    compiler::TNode<Object> tmp89;
    ca_.Bind(&block12, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89);
    ca_.Goto(&block11, tmp82, tmp83, tmp84, tmp85, tmp86, tmp87, tmp88, tmp89);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp90;
    compiler::TNode<Object> tmp91;
    compiler::TNode<RawPtrT> tmp92;
    compiler::TNode<RawPtrT> tmp93;
    compiler::TNode<IntPtrT> tmp94;
    compiler::TNode<Object> tmp95;
    compiler::TNode<Object> tmp96;
    compiler::TNode<Object> tmp97;
    ca_.Bind(&block11, &tmp90, &tmp91, &tmp92, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 768);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 771);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 770);
    compiler::TNode<Object> tmp98;
    USE(tmp98);
    tmp98 = ca_.UncheckedCast<Object>(DataViewBuiltinsFromDSLAssembler(state_).DataViewSet(compiler::TNode<Context>{tmp90}, compiler::TNode<Object>{tmp91}, compiler::TNode<Object>{tmp95}, compiler::TNode<Object>{tmp96}, compiler::TNode<Object>{tmp97}, UINT16_ELEMENTS));
    arguments.PopAndReturn(tmp98);
  }
}

TF_BUILTIN(DataViewPrototypeSetInt16, CodeStubAssembler) {
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
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, Object> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, Object> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, torque_arguments.frame, torque_arguments.base, torque_arguments.length);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<RawPtrT> tmp2;
    compiler::TNode<RawPtrT> tmp3;
    compiler::TNode<IntPtrT> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 776);
    compiler::TNode<IntPtrT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp4}, compiler::TNode<IntPtrT>{tmp5}));
    ca_.Branch(tmp6, &block1, &block2, tmp0, tmp1, tmp2, tmp3, tmp4);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<Object> tmp8;
    compiler::TNode<RawPtrT> tmp9;
    compiler::TNode<RawPtrT> tmp10;
    compiler::TNode<IntPtrT> tmp11;
    ca_.Bind(&block1, &tmp7, &tmp8, &tmp9, &tmp10, &tmp11);
    compiler::TNode<IntPtrT> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp9}, compiler::TNode<RawPtrT>{tmp10}, compiler::TNode<IntPtrT>{tmp11}}, compiler::TNode<IntPtrT>{tmp12}));
    ca_.Goto(&block4, tmp7, tmp8, tmp9, tmp10, tmp11, tmp13);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp14;
    compiler::TNode<Object> tmp15;
    compiler::TNode<RawPtrT> tmp16;
    compiler::TNode<RawPtrT> tmp17;
    compiler::TNode<IntPtrT> tmp18;
    ca_.Bind(&block2, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18);
    compiler::TNode<Oddball> tmp19;
    USE(tmp19);
    tmp19 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block3, tmp14, tmp15, tmp16, tmp17, tmp18, tmp19);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp20;
    compiler::TNode<Object> tmp21;
    compiler::TNode<RawPtrT> tmp22;
    compiler::TNode<RawPtrT> tmp23;
    compiler::TNode<IntPtrT> tmp24;
    compiler::TNode<Object> tmp25;
    ca_.Bind(&block4, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25);
    ca_.Goto(&block3, tmp20, tmp21, tmp22, tmp23, tmp24, tmp25);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp26;
    compiler::TNode<Object> tmp27;
    compiler::TNode<RawPtrT> tmp28;
    compiler::TNode<RawPtrT> tmp29;
    compiler::TNode<IntPtrT> tmp30;
    compiler::TNode<Object> tmp31;
    ca_.Bind(&block3, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 777);
    compiler::TNode<IntPtrT> tmp32;
    USE(tmp32);
    tmp32 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<BoolT> tmp33;
    USE(tmp33);
    tmp33 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp30}, compiler::TNode<IntPtrT>{tmp32}));
    ca_.Branch(tmp33, &block5, &block6, tmp26, tmp27, tmp28, tmp29, tmp30, tmp31);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp34;
    compiler::TNode<Object> tmp35;
    compiler::TNode<RawPtrT> tmp36;
    compiler::TNode<RawPtrT> tmp37;
    compiler::TNode<IntPtrT> tmp38;
    compiler::TNode<Object> tmp39;
    ca_.Bind(&block5, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39);
    compiler::TNode<IntPtrT> tmp40;
    USE(tmp40);
    tmp40 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<Object> tmp41;
    USE(tmp41);
    tmp41 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp36}, compiler::TNode<RawPtrT>{tmp37}, compiler::TNode<IntPtrT>{tmp38}}, compiler::TNode<IntPtrT>{tmp40}));
    ca_.Goto(&block8, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39, tmp41);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp42;
    compiler::TNode<Object> tmp43;
    compiler::TNode<RawPtrT> tmp44;
    compiler::TNode<RawPtrT> tmp45;
    compiler::TNode<IntPtrT> tmp46;
    compiler::TNode<Object> tmp47;
    ca_.Bind(&block6, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47);
    compiler::TNode<Oddball> tmp48;
    USE(tmp48);
    tmp48 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block7, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp49;
    compiler::TNode<Object> tmp50;
    compiler::TNode<RawPtrT> tmp51;
    compiler::TNode<RawPtrT> tmp52;
    compiler::TNode<IntPtrT> tmp53;
    compiler::TNode<Object> tmp54;
    compiler::TNode<Object> tmp55;
    ca_.Bind(&block8, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55);
    ca_.Goto(&block7, tmp49, tmp50, tmp51, tmp52, tmp53, tmp54, tmp55);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp56;
    compiler::TNode<Object> tmp57;
    compiler::TNode<RawPtrT> tmp58;
    compiler::TNode<RawPtrT> tmp59;
    compiler::TNode<IntPtrT> tmp60;
    compiler::TNode<Object> tmp61;
    compiler::TNode<Object> tmp62;
    ca_.Bind(&block7, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 779);
    compiler::TNode<IntPtrT> tmp63;
    USE(tmp63);
    tmp63 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(2));
    compiler::TNode<BoolT> tmp64;
    USE(tmp64);
    tmp64 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp60}, compiler::TNode<IntPtrT>{tmp63}));
    ca_.Branch(tmp64, &block9, &block10, tmp56, tmp57, tmp58, tmp59, tmp60, tmp61, tmp62);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp65;
    compiler::TNode<Object> tmp66;
    compiler::TNode<RawPtrT> tmp67;
    compiler::TNode<RawPtrT> tmp68;
    compiler::TNode<IntPtrT> tmp69;
    compiler::TNode<Object> tmp70;
    compiler::TNode<Object> tmp71;
    ca_.Bind(&block9, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71);
    compiler::TNode<IntPtrT> tmp72;
    USE(tmp72);
    tmp72 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(2));
    compiler::TNode<Object> tmp73;
    USE(tmp73);
    tmp73 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp67}, compiler::TNode<RawPtrT>{tmp68}, compiler::TNode<IntPtrT>{tmp69}}, compiler::TNode<IntPtrT>{tmp72}));
    ca_.Goto(&block12, tmp65, tmp66, tmp67, tmp68, tmp69, tmp70, tmp71, tmp73);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp74;
    compiler::TNode<Object> tmp75;
    compiler::TNode<RawPtrT> tmp76;
    compiler::TNode<RawPtrT> tmp77;
    compiler::TNode<IntPtrT> tmp78;
    compiler::TNode<Object> tmp79;
    compiler::TNode<Object> tmp80;
    ca_.Bind(&block10, &tmp74, &tmp75, &tmp76, &tmp77, &tmp78, &tmp79, &tmp80);
    compiler::TNode<Oddball> tmp81;
    USE(tmp81);
    tmp81 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block11, tmp74, tmp75, tmp76, tmp77, tmp78, tmp79, tmp80, tmp81);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp82;
    compiler::TNode<Object> tmp83;
    compiler::TNode<RawPtrT> tmp84;
    compiler::TNode<RawPtrT> tmp85;
    compiler::TNode<IntPtrT> tmp86;
    compiler::TNode<Object> tmp87;
    compiler::TNode<Object> tmp88;
    compiler::TNode<Object> tmp89;
    ca_.Bind(&block12, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89);
    ca_.Goto(&block11, tmp82, tmp83, tmp84, tmp85, tmp86, tmp87, tmp88, tmp89);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp90;
    compiler::TNode<Object> tmp91;
    compiler::TNode<RawPtrT> tmp92;
    compiler::TNode<RawPtrT> tmp93;
    compiler::TNode<IntPtrT> tmp94;
    compiler::TNode<Object> tmp95;
    compiler::TNode<Object> tmp96;
    compiler::TNode<Object> tmp97;
    ca_.Bind(&block11, &tmp90, &tmp91, &tmp92, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 778);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 781);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 780);
    compiler::TNode<Object> tmp98;
    USE(tmp98);
    tmp98 = ca_.UncheckedCast<Object>(DataViewBuiltinsFromDSLAssembler(state_).DataViewSet(compiler::TNode<Context>{tmp90}, compiler::TNode<Object>{tmp91}, compiler::TNode<Object>{tmp95}, compiler::TNode<Object>{tmp96}, compiler::TNode<Object>{tmp97}, INT16_ELEMENTS));
    arguments.PopAndReturn(tmp98);
  }
}

TF_BUILTIN(DataViewPrototypeSetUint32, CodeStubAssembler) {
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
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, Object> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, Object> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, torque_arguments.frame, torque_arguments.base, torque_arguments.length);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<RawPtrT> tmp2;
    compiler::TNode<RawPtrT> tmp3;
    compiler::TNode<IntPtrT> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 786);
    compiler::TNode<IntPtrT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp4}, compiler::TNode<IntPtrT>{tmp5}));
    ca_.Branch(tmp6, &block1, &block2, tmp0, tmp1, tmp2, tmp3, tmp4);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<Object> tmp8;
    compiler::TNode<RawPtrT> tmp9;
    compiler::TNode<RawPtrT> tmp10;
    compiler::TNode<IntPtrT> tmp11;
    ca_.Bind(&block1, &tmp7, &tmp8, &tmp9, &tmp10, &tmp11);
    compiler::TNode<IntPtrT> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp9}, compiler::TNode<RawPtrT>{tmp10}, compiler::TNode<IntPtrT>{tmp11}}, compiler::TNode<IntPtrT>{tmp12}));
    ca_.Goto(&block4, tmp7, tmp8, tmp9, tmp10, tmp11, tmp13);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp14;
    compiler::TNode<Object> tmp15;
    compiler::TNode<RawPtrT> tmp16;
    compiler::TNode<RawPtrT> tmp17;
    compiler::TNode<IntPtrT> tmp18;
    ca_.Bind(&block2, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18);
    compiler::TNode<Oddball> tmp19;
    USE(tmp19);
    tmp19 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block3, tmp14, tmp15, tmp16, tmp17, tmp18, tmp19);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp20;
    compiler::TNode<Object> tmp21;
    compiler::TNode<RawPtrT> tmp22;
    compiler::TNode<RawPtrT> tmp23;
    compiler::TNode<IntPtrT> tmp24;
    compiler::TNode<Object> tmp25;
    ca_.Bind(&block4, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25);
    ca_.Goto(&block3, tmp20, tmp21, tmp22, tmp23, tmp24, tmp25);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp26;
    compiler::TNode<Object> tmp27;
    compiler::TNode<RawPtrT> tmp28;
    compiler::TNode<RawPtrT> tmp29;
    compiler::TNode<IntPtrT> tmp30;
    compiler::TNode<Object> tmp31;
    ca_.Bind(&block3, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 787);
    compiler::TNode<IntPtrT> tmp32;
    USE(tmp32);
    tmp32 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<BoolT> tmp33;
    USE(tmp33);
    tmp33 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp30}, compiler::TNode<IntPtrT>{tmp32}));
    ca_.Branch(tmp33, &block5, &block6, tmp26, tmp27, tmp28, tmp29, tmp30, tmp31);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp34;
    compiler::TNode<Object> tmp35;
    compiler::TNode<RawPtrT> tmp36;
    compiler::TNode<RawPtrT> tmp37;
    compiler::TNode<IntPtrT> tmp38;
    compiler::TNode<Object> tmp39;
    ca_.Bind(&block5, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39);
    compiler::TNode<IntPtrT> tmp40;
    USE(tmp40);
    tmp40 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<Object> tmp41;
    USE(tmp41);
    tmp41 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp36}, compiler::TNode<RawPtrT>{tmp37}, compiler::TNode<IntPtrT>{tmp38}}, compiler::TNode<IntPtrT>{tmp40}));
    ca_.Goto(&block8, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39, tmp41);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp42;
    compiler::TNode<Object> tmp43;
    compiler::TNode<RawPtrT> tmp44;
    compiler::TNode<RawPtrT> tmp45;
    compiler::TNode<IntPtrT> tmp46;
    compiler::TNode<Object> tmp47;
    ca_.Bind(&block6, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47);
    compiler::TNode<Oddball> tmp48;
    USE(tmp48);
    tmp48 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block7, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp49;
    compiler::TNode<Object> tmp50;
    compiler::TNode<RawPtrT> tmp51;
    compiler::TNode<RawPtrT> tmp52;
    compiler::TNode<IntPtrT> tmp53;
    compiler::TNode<Object> tmp54;
    compiler::TNode<Object> tmp55;
    ca_.Bind(&block8, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55);
    ca_.Goto(&block7, tmp49, tmp50, tmp51, tmp52, tmp53, tmp54, tmp55);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp56;
    compiler::TNode<Object> tmp57;
    compiler::TNode<RawPtrT> tmp58;
    compiler::TNode<RawPtrT> tmp59;
    compiler::TNode<IntPtrT> tmp60;
    compiler::TNode<Object> tmp61;
    compiler::TNode<Object> tmp62;
    ca_.Bind(&block7, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 789);
    compiler::TNode<IntPtrT> tmp63;
    USE(tmp63);
    tmp63 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(2));
    compiler::TNode<BoolT> tmp64;
    USE(tmp64);
    tmp64 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp60}, compiler::TNode<IntPtrT>{tmp63}));
    ca_.Branch(tmp64, &block9, &block10, tmp56, tmp57, tmp58, tmp59, tmp60, tmp61, tmp62);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp65;
    compiler::TNode<Object> tmp66;
    compiler::TNode<RawPtrT> tmp67;
    compiler::TNode<RawPtrT> tmp68;
    compiler::TNode<IntPtrT> tmp69;
    compiler::TNode<Object> tmp70;
    compiler::TNode<Object> tmp71;
    ca_.Bind(&block9, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71);
    compiler::TNode<IntPtrT> tmp72;
    USE(tmp72);
    tmp72 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(2));
    compiler::TNode<Object> tmp73;
    USE(tmp73);
    tmp73 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp67}, compiler::TNode<RawPtrT>{tmp68}, compiler::TNode<IntPtrT>{tmp69}}, compiler::TNode<IntPtrT>{tmp72}));
    ca_.Goto(&block12, tmp65, tmp66, tmp67, tmp68, tmp69, tmp70, tmp71, tmp73);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp74;
    compiler::TNode<Object> tmp75;
    compiler::TNode<RawPtrT> tmp76;
    compiler::TNode<RawPtrT> tmp77;
    compiler::TNode<IntPtrT> tmp78;
    compiler::TNode<Object> tmp79;
    compiler::TNode<Object> tmp80;
    ca_.Bind(&block10, &tmp74, &tmp75, &tmp76, &tmp77, &tmp78, &tmp79, &tmp80);
    compiler::TNode<Oddball> tmp81;
    USE(tmp81);
    tmp81 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block11, tmp74, tmp75, tmp76, tmp77, tmp78, tmp79, tmp80, tmp81);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp82;
    compiler::TNode<Object> tmp83;
    compiler::TNode<RawPtrT> tmp84;
    compiler::TNode<RawPtrT> tmp85;
    compiler::TNode<IntPtrT> tmp86;
    compiler::TNode<Object> tmp87;
    compiler::TNode<Object> tmp88;
    compiler::TNode<Object> tmp89;
    ca_.Bind(&block12, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89);
    ca_.Goto(&block11, tmp82, tmp83, tmp84, tmp85, tmp86, tmp87, tmp88, tmp89);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp90;
    compiler::TNode<Object> tmp91;
    compiler::TNode<RawPtrT> tmp92;
    compiler::TNode<RawPtrT> tmp93;
    compiler::TNode<IntPtrT> tmp94;
    compiler::TNode<Object> tmp95;
    compiler::TNode<Object> tmp96;
    compiler::TNode<Object> tmp97;
    ca_.Bind(&block11, &tmp90, &tmp91, &tmp92, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 788);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 791);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 790);
    compiler::TNode<Object> tmp98;
    USE(tmp98);
    tmp98 = ca_.UncheckedCast<Object>(DataViewBuiltinsFromDSLAssembler(state_).DataViewSet(compiler::TNode<Context>{tmp90}, compiler::TNode<Object>{tmp91}, compiler::TNode<Object>{tmp95}, compiler::TNode<Object>{tmp96}, compiler::TNode<Object>{tmp97}, UINT32_ELEMENTS));
    arguments.PopAndReturn(tmp98);
  }
}

TF_BUILTIN(DataViewPrototypeSetInt32, CodeStubAssembler) {
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
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, Object> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, Object> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, torque_arguments.frame, torque_arguments.base, torque_arguments.length);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<RawPtrT> tmp2;
    compiler::TNode<RawPtrT> tmp3;
    compiler::TNode<IntPtrT> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 796);
    compiler::TNode<IntPtrT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp4}, compiler::TNode<IntPtrT>{tmp5}));
    ca_.Branch(tmp6, &block1, &block2, tmp0, tmp1, tmp2, tmp3, tmp4);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<Object> tmp8;
    compiler::TNode<RawPtrT> tmp9;
    compiler::TNode<RawPtrT> tmp10;
    compiler::TNode<IntPtrT> tmp11;
    ca_.Bind(&block1, &tmp7, &tmp8, &tmp9, &tmp10, &tmp11);
    compiler::TNode<IntPtrT> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp9}, compiler::TNode<RawPtrT>{tmp10}, compiler::TNode<IntPtrT>{tmp11}}, compiler::TNode<IntPtrT>{tmp12}));
    ca_.Goto(&block4, tmp7, tmp8, tmp9, tmp10, tmp11, tmp13);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp14;
    compiler::TNode<Object> tmp15;
    compiler::TNode<RawPtrT> tmp16;
    compiler::TNode<RawPtrT> tmp17;
    compiler::TNode<IntPtrT> tmp18;
    ca_.Bind(&block2, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18);
    compiler::TNode<Oddball> tmp19;
    USE(tmp19);
    tmp19 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block3, tmp14, tmp15, tmp16, tmp17, tmp18, tmp19);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp20;
    compiler::TNode<Object> tmp21;
    compiler::TNode<RawPtrT> tmp22;
    compiler::TNode<RawPtrT> tmp23;
    compiler::TNode<IntPtrT> tmp24;
    compiler::TNode<Object> tmp25;
    ca_.Bind(&block4, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25);
    ca_.Goto(&block3, tmp20, tmp21, tmp22, tmp23, tmp24, tmp25);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp26;
    compiler::TNode<Object> tmp27;
    compiler::TNode<RawPtrT> tmp28;
    compiler::TNode<RawPtrT> tmp29;
    compiler::TNode<IntPtrT> tmp30;
    compiler::TNode<Object> tmp31;
    ca_.Bind(&block3, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 797);
    compiler::TNode<IntPtrT> tmp32;
    USE(tmp32);
    tmp32 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<BoolT> tmp33;
    USE(tmp33);
    tmp33 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp30}, compiler::TNode<IntPtrT>{tmp32}));
    ca_.Branch(tmp33, &block5, &block6, tmp26, tmp27, tmp28, tmp29, tmp30, tmp31);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp34;
    compiler::TNode<Object> tmp35;
    compiler::TNode<RawPtrT> tmp36;
    compiler::TNode<RawPtrT> tmp37;
    compiler::TNode<IntPtrT> tmp38;
    compiler::TNode<Object> tmp39;
    ca_.Bind(&block5, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39);
    compiler::TNode<IntPtrT> tmp40;
    USE(tmp40);
    tmp40 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<Object> tmp41;
    USE(tmp41);
    tmp41 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp36}, compiler::TNode<RawPtrT>{tmp37}, compiler::TNode<IntPtrT>{tmp38}}, compiler::TNode<IntPtrT>{tmp40}));
    ca_.Goto(&block8, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39, tmp41);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp42;
    compiler::TNode<Object> tmp43;
    compiler::TNode<RawPtrT> tmp44;
    compiler::TNode<RawPtrT> tmp45;
    compiler::TNode<IntPtrT> tmp46;
    compiler::TNode<Object> tmp47;
    ca_.Bind(&block6, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47);
    compiler::TNode<Oddball> tmp48;
    USE(tmp48);
    tmp48 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block7, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp49;
    compiler::TNode<Object> tmp50;
    compiler::TNode<RawPtrT> tmp51;
    compiler::TNode<RawPtrT> tmp52;
    compiler::TNode<IntPtrT> tmp53;
    compiler::TNode<Object> tmp54;
    compiler::TNode<Object> tmp55;
    ca_.Bind(&block8, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55);
    ca_.Goto(&block7, tmp49, tmp50, tmp51, tmp52, tmp53, tmp54, tmp55);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp56;
    compiler::TNode<Object> tmp57;
    compiler::TNode<RawPtrT> tmp58;
    compiler::TNode<RawPtrT> tmp59;
    compiler::TNode<IntPtrT> tmp60;
    compiler::TNode<Object> tmp61;
    compiler::TNode<Object> tmp62;
    ca_.Bind(&block7, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 799);
    compiler::TNode<IntPtrT> tmp63;
    USE(tmp63);
    tmp63 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(2));
    compiler::TNode<BoolT> tmp64;
    USE(tmp64);
    tmp64 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp60}, compiler::TNode<IntPtrT>{tmp63}));
    ca_.Branch(tmp64, &block9, &block10, tmp56, tmp57, tmp58, tmp59, tmp60, tmp61, tmp62);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp65;
    compiler::TNode<Object> tmp66;
    compiler::TNode<RawPtrT> tmp67;
    compiler::TNode<RawPtrT> tmp68;
    compiler::TNode<IntPtrT> tmp69;
    compiler::TNode<Object> tmp70;
    compiler::TNode<Object> tmp71;
    ca_.Bind(&block9, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71);
    compiler::TNode<IntPtrT> tmp72;
    USE(tmp72);
    tmp72 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(2));
    compiler::TNode<Object> tmp73;
    USE(tmp73);
    tmp73 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp67}, compiler::TNode<RawPtrT>{tmp68}, compiler::TNode<IntPtrT>{tmp69}}, compiler::TNode<IntPtrT>{tmp72}));
    ca_.Goto(&block12, tmp65, tmp66, tmp67, tmp68, tmp69, tmp70, tmp71, tmp73);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp74;
    compiler::TNode<Object> tmp75;
    compiler::TNode<RawPtrT> tmp76;
    compiler::TNode<RawPtrT> tmp77;
    compiler::TNode<IntPtrT> tmp78;
    compiler::TNode<Object> tmp79;
    compiler::TNode<Object> tmp80;
    ca_.Bind(&block10, &tmp74, &tmp75, &tmp76, &tmp77, &tmp78, &tmp79, &tmp80);
    compiler::TNode<Oddball> tmp81;
    USE(tmp81);
    tmp81 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block11, tmp74, tmp75, tmp76, tmp77, tmp78, tmp79, tmp80, tmp81);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp82;
    compiler::TNode<Object> tmp83;
    compiler::TNode<RawPtrT> tmp84;
    compiler::TNode<RawPtrT> tmp85;
    compiler::TNode<IntPtrT> tmp86;
    compiler::TNode<Object> tmp87;
    compiler::TNode<Object> tmp88;
    compiler::TNode<Object> tmp89;
    ca_.Bind(&block12, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89);
    ca_.Goto(&block11, tmp82, tmp83, tmp84, tmp85, tmp86, tmp87, tmp88, tmp89);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp90;
    compiler::TNode<Object> tmp91;
    compiler::TNode<RawPtrT> tmp92;
    compiler::TNode<RawPtrT> tmp93;
    compiler::TNode<IntPtrT> tmp94;
    compiler::TNode<Object> tmp95;
    compiler::TNode<Object> tmp96;
    compiler::TNode<Object> tmp97;
    ca_.Bind(&block11, &tmp90, &tmp91, &tmp92, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 798);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 801);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 800);
    compiler::TNode<Object> tmp98;
    USE(tmp98);
    tmp98 = ca_.UncheckedCast<Object>(DataViewBuiltinsFromDSLAssembler(state_).DataViewSet(compiler::TNode<Context>{tmp90}, compiler::TNode<Object>{tmp91}, compiler::TNode<Object>{tmp95}, compiler::TNode<Object>{tmp96}, compiler::TNode<Object>{tmp97}, INT32_ELEMENTS));
    arguments.PopAndReturn(tmp98);
  }
}

TF_BUILTIN(DataViewPrototypeSetFloat32, CodeStubAssembler) {
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
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, Object> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, Object> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, torque_arguments.frame, torque_arguments.base, torque_arguments.length);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<RawPtrT> tmp2;
    compiler::TNode<RawPtrT> tmp3;
    compiler::TNode<IntPtrT> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 806);
    compiler::TNode<IntPtrT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp4}, compiler::TNode<IntPtrT>{tmp5}));
    ca_.Branch(tmp6, &block1, &block2, tmp0, tmp1, tmp2, tmp3, tmp4);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<Object> tmp8;
    compiler::TNode<RawPtrT> tmp9;
    compiler::TNode<RawPtrT> tmp10;
    compiler::TNode<IntPtrT> tmp11;
    ca_.Bind(&block1, &tmp7, &tmp8, &tmp9, &tmp10, &tmp11);
    compiler::TNode<IntPtrT> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp9}, compiler::TNode<RawPtrT>{tmp10}, compiler::TNode<IntPtrT>{tmp11}}, compiler::TNode<IntPtrT>{tmp12}));
    ca_.Goto(&block4, tmp7, tmp8, tmp9, tmp10, tmp11, tmp13);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp14;
    compiler::TNode<Object> tmp15;
    compiler::TNode<RawPtrT> tmp16;
    compiler::TNode<RawPtrT> tmp17;
    compiler::TNode<IntPtrT> tmp18;
    ca_.Bind(&block2, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18);
    compiler::TNode<Oddball> tmp19;
    USE(tmp19);
    tmp19 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block3, tmp14, tmp15, tmp16, tmp17, tmp18, tmp19);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp20;
    compiler::TNode<Object> tmp21;
    compiler::TNode<RawPtrT> tmp22;
    compiler::TNode<RawPtrT> tmp23;
    compiler::TNode<IntPtrT> tmp24;
    compiler::TNode<Object> tmp25;
    ca_.Bind(&block4, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25);
    ca_.Goto(&block3, tmp20, tmp21, tmp22, tmp23, tmp24, tmp25);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp26;
    compiler::TNode<Object> tmp27;
    compiler::TNode<RawPtrT> tmp28;
    compiler::TNode<RawPtrT> tmp29;
    compiler::TNode<IntPtrT> tmp30;
    compiler::TNode<Object> tmp31;
    ca_.Bind(&block3, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 807);
    compiler::TNode<IntPtrT> tmp32;
    USE(tmp32);
    tmp32 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<BoolT> tmp33;
    USE(tmp33);
    tmp33 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp30}, compiler::TNode<IntPtrT>{tmp32}));
    ca_.Branch(tmp33, &block5, &block6, tmp26, tmp27, tmp28, tmp29, tmp30, tmp31);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp34;
    compiler::TNode<Object> tmp35;
    compiler::TNode<RawPtrT> tmp36;
    compiler::TNode<RawPtrT> tmp37;
    compiler::TNode<IntPtrT> tmp38;
    compiler::TNode<Object> tmp39;
    ca_.Bind(&block5, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39);
    compiler::TNode<IntPtrT> tmp40;
    USE(tmp40);
    tmp40 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<Object> tmp41;
    USE(tmp41);
    tmp41 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp36}, compiler::TNode<RawPtrT>{tmp37}, compiler::TNode<IntPtrT>{tmp38}}, compiler::TNode<IntPtrT>{tmp40}));
    ca_.Goto(&block8, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39, tmp41);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp42;
    compiler::TNode<Object> tmp43;
    compiler::TNode<RawPtrT> tmp44;
    compiler::TNode<RawPtrT> tmp45;
    compiler::TNode<IntPtrT> tmp46;
    compiler::TNode<Object> tmp47;
    ca_.Bind(&block6, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47);
    compiler::TNode<Oddball> tmp48;
    USE(tmp48);
    tmp48 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block7, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp49;
    compiler::TNode<Object> tmp50;
    compiler::TNode<RawPtrT> tmp51;
    compiler::TNode<RawPtrT> tmp52;
    compiler::TNode<IntPtrT> tmp53;
    compiler::TNode<Object> tmp54;
    compiler::TNode<Object> tmp55;
    ca_.Bind(&block8, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55);
    ca_.Goto(&block7, tmp49, tmp50, tmp51, tmp52, tmp53, tmp54, tmp55);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp56;
    compiler::TNode<Object> tmp57;
    compiler::TNode<RawPtrT> tmp58;
    compiler::TNode<RawPtrT> tmp59;
    compiler::TNode<IntPtrT> tmp60;
    compiler::TNode<Object> tmp61;
    compiler::TNode<Object> tmp62;
    ca_.Bind(&block7, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 809);
    compiler::TNode<IntPtrT> tmp63;
    USE(tmp63);
    tmp63 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(2));
    compiler::TNode<BoolT> tmp64;
    USE(tmp64);
    tmp64 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp60}, compiler::TNode<IntPtrT>{tmp63}));
    ca_.Branch(tmp64, &block9, &block10, tmp56, tmp57, tmp58, tmp59, tmp60, tmp61, tmp62);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp65;
    compiler::TNode<Object> tmp66;
    compiler::TNode<RawPtrT> tmp67;
    compiler::TNode<RawPtrT> tmp68;
    compiler::TNode<IntPtrT> tmp69;
    compiler::TNode<Object> tmp70;
    compiler::TNode<Object> tmp71;
    ca_.Bind(&block9, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71);
    compiler::TNode<IntPtrT> tmp72;
    USE(tmp72);
    tmp72 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(2));
    compiler::TNode<Object> tmp73;
    USE(tmp73);
    tmp73 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp67}, compiler::TNode<RawPtrT>{tmp68}, compiler::TNode<IntPtrT>{tmp69}}, compiler::TNode<IntPtrT>{tmp72}));
    ca_.Goto(&block12, tmp65, tmp66, tmp67, tmp68, tmp69, tmp70, tmp71, tmp73);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp74;
    compiler::TNode<Object> tmp75;
    compiler::TNode<RawPtrT> tmp76;
    compiler::TNode<RawPtrT> tmp77;
    compiler::TNode<IntPtrT> tmp78;
    compiler::TNode<Object> tmp79;
    compiler::TNode<Object> tmp80;
    ca_.Bind(&block10, &tmp74, &tmp75, &tmp76, &tmp77, &tmp78, &tmp79, &tmp80);
    compiler::TNode<Oddball> tmp81;
    USE(tmp81);
    tmp81 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block11, tmp74, tmp75, tmp76, tmp77, tmp78, tmp79, tmp80, tmp81);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp82;
    compiler::TNode<Object> tmp83;
    compiler::TNode<RawPtrT> tmp84;
    compiler::TNode<RawPtrT> tmp85;
    compiler::TNode<IntPtrT> tmp86;
    compiler::TNode<Object> tmp87;
    compiler::TNode<Object> tmp88;
    compiler::TNode<Object> tmp89;
    ca_.Bind(&block12, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89);
    ca_.Goto(&block11, tmp82, tmp83, tmp84, tmp85, tmp86, tmp87, tmp88, tmp89);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp90;
    compiler::TNode<Object> tmp91;
    compiler::TNode<RawPtrT> tmp92;
    compiler::TNode<RawPtrT> tmp93;
    compiler::TNode<IntPtrT> tmp94;
    compiler::TNode<Object> tmp95;
    compiler::TNode<Object> tmp96;
    compiler::TNode<Object> tmp97;
    ca_.Bind(&block11, &tmp90, &tmp91, &tmp92, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 808);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 811);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 810);
    compiler::TNode<Object> tmp98;
    USE(tmp98);
    tmp98 = ca_.UncheckedCast<Object>(DataViewBuiltinsFromDSLAssembler(state_).DataViewSet(compiler::TNode<Context>{tmp90}, compiler::TNode<Object>{tmp91}, compiler::TNode<Object>{tmp95}, compiler::TNode<Object>{tmp96}, compiler::TNode<Object>{tmp97}, FLOAT32_ELEMENTS));
    arguments.PopAndReturn(tmp98);
  }
}

TF_BUILTIN(DataViewPrototypeSetFloat64, CodeStubAssembler) {
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
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, Object> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, Object> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, torque_arguments.frame, torque_arguments.base, torque_arguments.length);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<RawPtrT> tmp2;
    compiler::TNode<RawPtrT> tmp3;
    compiler::TNode<IntPtrT> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 816);
    compiler::TNode<IntPtrT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp4}, compiler::TNode<IntPtrT>{tmp5}));
    ca_.Branch(tmp6, &block1, &block2, tmp0, tmp1, tmp2, tmp3, tmp4);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<Object> tmp8;
    compiler::TNode<RawPtrT> tmp9;
    compiler::TNode<RawPtrT> tmp10;
    compiler::TNode<IntPtrT> tmp11;
    ca_.Bind(&block1, &tmp7, &tmp8, &tmp9, &tmp10, &tmp11);
    compiler::TNode<IntPtrT> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp9}, compiler::TNode<RawPtrT>{tmp10}, compiler::TNode<IntPtrT>{tmp11}}, compiler::TNode<IntPtrT>{tmp12}));
    ca_.Goto(&block4, tmp7, tmp8, tmp9, tmp10, tmp11, tmp13);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp14;
    compiler::TNode<Object> tmp15;
    compiler::TNode<RawPtrT> tmp16;
    compiler::TNode<RawPtrT> tmp17;
    compiler::TNode<IntPtrT> tmp18;
    ca_.Bind(&block2, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18);
    compiler::TNode<Oddball> tmp19;
    USE(tmp19);
    tmp19 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block3, tmp14, tmp15, tmp16, tmp17, tmp18, tmp19);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp20;
    compiler::TNode<Object> tmp21;
    compiler::TNode<RawPtrT> tmp22;
    compiler::TNode<RawPtrT> tmp23;
    compiler::TNode<IntPtrT> tmp24;
    compiler::TNode<Object> tmp25;
    ca_.Bind(&block4, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25);
    ca_.Goto(&block3, tmp20, tmp21, tmp22, tmp23, tmp24, tmp25);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp26;
    compiler::TNode<Object> tmp27;
    compiler::TNode<RawPtrT> tmp28;
    compiler::TNode<RawPtrT> tmp29;
    compiler::TNode<IntPtrT> tmp30;
    compiler::TNode<Object> tmp31;
    ca_.Bind(&block3, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 817);
    compiler::TNode<IntPtrT> tmp32;
    USE(tmp32);
    tmp32 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<BoolT> tmp33;
    USE(tmp33);
    tmp33 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp30}, compiler::TNode<IntPtrT>{tmp32}));
    ca_.Branch(tmp33, &block5, &block6, tmp26, tmp27, tmp28, tmp29, tmp30, tmp31);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp34;
    compiler::TNode<Object> tmp35;
    compiler::TNode<RawPtrT> tmp36;
    compiler::TNode<RawPtrT> tmp37;
    compiler::TNode<IntPtrT> tmp38;
    compiler::TNode<Object> tmp39;
    ca_.Bind(&block5, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39);
    compiler::TNode<IntPtrT> tmp40;
    USE(tmp40);
    tmp40 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<Object> tmp41;
    USE(tmp41);
    tmp41 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp36}, compiler::TNode<RawPtrT>{tmp37}, compiler::TNode<IntPtrT>{tmp38}}, compiler::TNode<IntPtrT>{tmp40}));
    ca_.Goto(&block8, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39, tmp41);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp42;
    compiler::TNode<Object> tmp43;
    compiler::TNode<RawPtrT> tmp44;
    compiler::TNode<RawPtrT> tmp45;
    compiler::TNode<IntPtrT> tmp46;
    compiler::TNode<Object> tmp47;
    ca_.Bind(&block6, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47);
    compiler::TNode<Oddball> tmp48;
    USE(tmp48);
    tmp48 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block7, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp49;
    compiler::TNode<Object> tmp50;
    compiler::TNode<RawPtrT> tmp51;
    compiler::TNode<RawPtrT> tmp52;
    compiler::TNode<IntPtrT> tmp53;
    compiler::TNode<Object> tmp54;
    compiler::TNode<Object> tmp55;
    ca_.Bind(&block8, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55);
    ca_.Goto(&block7, tmp49, tmp50, tmp51, tmp52, tmp53, tmp54, tmp55);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp56;
    compiler::TNode<Object> tmp57;
    compiler::TNode<RawPtrT> tmp58;
    compiler::TNode<RawPtrT> tmp59;
    compiler::TNode<IntPtrT> tmp60;
    compiler::TNode<Object> tmp61;
    compiler::TNode<Object> tmp62;
    ca_.Bind(&block7, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 819);
    compiler::TNode<IntPtrT> tmp63;
    USE(tmp63);
    tmp63 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(2));
    compiler::TNode<BoolT> tmp64;
    USE(tmp64);
    tmp64 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp60}, compiler::TNode<IntPtrT>{tmp63}));
    ca_.Branch(tmp64, &block9, &block10, tmp56, tmp57, tmp58, tmp59, tmp60, tmp61, tmp62);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp65;
    compiler::TNode<Object> tmp66;
    compiler::TNode<RawPtrT> tmp67;
    compiler::TNode<RawPtrT> tmp68;
    compiler::TNode<IntPtrT> tmp69;
    compiler::TNode<Object> tmp70;
    compiler::TNode<Object> tmp71;
    ca_.Bind(&block9, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71);
    compiler::TNode<IntPtrT> tmp72;
    USE(tmp72);
    tmp72 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(2));
    compiler::TNode<Object> tmp73;
    USE(tmp73);
    tmp73 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp67}, compiler::TNode<RawPtrT>{tmp68}, compiler::TNode<IntPtrT>{tmp69}}, compiler::TNode<IntPtrT>{tmp72}));
    ca_.Goto(&block12, tmp65, tmp66, tmp67, tmp68, tmp69, tmp70, tmp71, tmp73);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp74;
    compiler::TNode<Object> tmp75;
    compiler::TNode<RawPtrT> tmp76;
    compiler::TNode<RawPtrT> tmp77;
    compiler::TNode<IntPtrT> tmp78;
    compiler::TNode<Object> tmp79;
    compiler::TNode<Object> tmp80;
    ca_.Bind(&block10, &tmp74, &tmp75, &tmp76, &tmp77, &tmp78, &tmp79, &tmp80);
    compiler::TNode<Oddball> tmp81;
    USE(tmp81);
    tmp81 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block11, tmp74, tmp75, tmp76, tmp77, tmp78, tmp79, tmp80, tmp81);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp82;
    compiler::TNode<Object> tmp83;
    compiler::TNode<RawPtrT> tmp84;
    compiler::TNode<RawPtrT> tmp85;
    compiler::TNode<IntPtrT> tmp86;
    compiler::TNode<Object> tmp87;
    compiler::TNode<Object> tmp88;
    compiler::TNode<Object> tmp89;
    ca_.Bind(&block12, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89);
    ca_.Goto(&block11, tmp82, tmp83, tmp84, tmp85, tmp86, tmp87, tmp88, tmp89);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp90;
    compiler::TNode<Object> tmp91;
    compiler::TNode<RawPtrT> tmp92;
    compiler::TNode<RawPtrT> tmp93;
    compiler::TNode<IntPtrT> tmp94;
    compiler::TNode<Object> tmp95;
    compiler::TNode<Object> tmp96;
    compiler::TNode<Object> tmp97;
    ca_.Bind(&block11, &tmp90, &tmp91, &tmp92, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 818);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 821);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 820);
    compiler::TNode<Object> tmp98;
    USE(tmp98);
    tmp98 = ca_.UncheckedCast<Object>(DataViewBuiltinsFromDSLAssembler(state_).DataViewSet(compiler::TNode<Context>{tmp90}, compiler::TNode<Object>{tmp91}, compiler::TNode<Object>{tmp95}, compiler::TNode<Object>{tmp96}, compiler::TNode<Object>{tmp97}, FLOAT64_ELEMENTS));
    arguments.PopAndReturn(tmp98);
  }
}

TF_BUILTIN(DataViewPrototypeSetBigUint64, CodeStubAssembler) {
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
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, Object> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, Object> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, torque_arguments.frame, torque_arguments.base, torque_arguments.length);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<RawPtrT> tmp2;
    compiler::TNode<RawPtrT> tmp3;
    compiler::TNode<IntPtrT> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 826);
    compiler::TNode<IntPtrT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp4}, compiler::TNode<IntPtrT>{tmp5}));
    ca_.Branch(tmp6, &block1, &block2, tmp0, tmp1, tmp2, tmp3, tmp4);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<Object> tmp8;
    compiler::TNode<RawPtrT> tmp9;
    compiler::TNode<RawPtrT> tmp10;
    compiler::TNode<IntPtrT> tmp11;
    ca_.Bind(&block1, &tmp7, &tmp8, &tmp9, &tmp10, &tmp11);
    compiler::TNode<IntPtrT> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp9}, compiler::TNode<RawPtrT>{tmp10}, compiler::TNode<IntPtrT>{tmp11}}, compiler::TNode<IntPtrT>{tmp12}));
    ca_.Goto(&block4, tmp7, tmp8, tmp9, tmp10, tmp11, tmp13);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp14;
    compiler::TNode<Object> tmp15;
    compiler::TNode<RawPtrT> tmp16;
    compiler::TNode<RawPtrT> tmp17;
    compiler::TNode<IntPtrT> tmp18;
    ca_.Bind(&block2, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18);
    compiler::TNode<Oddball> tmp19;
    USE(tmp19);
    tmp19 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block3, tmp14, tmp15, tmp16, tmp17, tmp18, tmp19);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp20;
    compiler::TNode<Object> tmp21;
    compiler::TNode<RawPtrT> tmp22;
    compiler::TNode<RawPtrT> tmp23;
    compiler::TNode<IntPtrT> tmp24;
    compiler::TNode<Object> tmp25;
    ca_.Bind(&block4, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25);
    ca_.Goto(&block3, tmp20, tmp21, tmp22, tmp23, tmp24, tmp25);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp26;
    compiler::TNode<Object> tmp27;
    compiler::TNode<RawPtrT> tmp28;
    compiler::TNode<RawPtrT> tmp29;
    compiler::TNode<IntPtrT> tmp30;
    compiler::TNode<Object> tmp31;
    ca_.Bind(&block3, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 827);
    compiler::TNode<IntPtrT> tmp32;
    USE(tmp32);
    tmp32 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<BoolT> tmp33;
    USE(tmp33);
    tmp33 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp30}, compiler::TNode<IntPtrT>{tmp32}));
    ca_.Branch(tmp33, &block5, &block6, tmp26, tmp27, tmp28, tmp29, tmp30, tmp31);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp34;
    compiler::TNode<Object> tmp35;
    compiler::TNode<RawPtrT> tmp36;
    compiler::TNode<RawPtrT> tmp37;
    compiler::TNode<IntPtrT> tmp38;
    compiler::TNode<Object> tmp39;
    ca_.Bind(&block5, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39);
    compiler::TNode<IntPtrT> tmp40;
    USE(tmp40);
    tmp40 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<Object> tmp41;
    USE(tmp41);
    tmp41 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp36}, compiler::TNode<RawPtrT>{tmp37}, compiler::TNode<IntPtrT>{tmp38}}, compiler::TNode<IntPtrT>{tmp40}));
    ca_.Goto(&block8, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39, tmp41);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp42;
    compiler::TNode<Object> tmp43;
    compiler::TNode<RawPtrT> tmp44;
    compiler::TNode<RawPtrT> tmp45;
    compiler::TNode<IntPtrT> tmp46;
    compiler::TNode<Object> tmp47;
    ca_.Bind(&block6, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47);
    compiler::TNode<Oddball> tmp48;
    USE(tmp48);
    tmp48 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block7, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp49;
    compiler::TNode<Object> tmp50;
    compiler::TNode<RawPtrT> tmp51;
    compiler::TNode<RawPtrT> tmp52;
    compiler::TNode<IntPtrT> tmp53;
    compiler::TNode<Object> tmp54;
    compiler::TNode<Object> tmp55;
    ca_.Bind(&block8, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55);
    ca_.Goto(&block7, tmp49, tmp50, tmp51, tmp52, tmp53, tmp54, tmp55);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp56;
    compiler::TNode<Object> tmp57;
    compiler::TNode<RawPtrT> tmp58;
    compiler::TNode<RawPtrT> tmp59;
    compiler::TNode<IntPtrT> tmp60;
    compiler::TNode<Object> tmp61;
    compiler::TNode<Object> tmp62;
    ca_.Bind(&block7, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 829);
    compiler::TNode<IntPtrT> tmp63;
    USE(tmp63);
    tmp63 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(2));
    compiler::TNode<BoolT> tmp64;
    USE(tmp64);
    tmp64 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp60}, compiler::TNode<IntPtrT>{tmp63}));
    ca_.Branch(tmp64, &block9, &block10, tmp56, tmp57, tmp58, tmp59, tmp60, tmp61, tmp62);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp65;
    compiler::TNode<Object> tmp66;
    compiler::TNode<RawPtrT> tmp67;
    compiler::TNode<RawPtrT> tmp68;
    compiler::TNode<IntPtrT> tmp69;
    compiler::TNode<Object> tmp70;
    compiler::TNode<Object> tmp71;
    ca_.Bind(&block9, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71);
    compiler::TNode<IntPtrT> tmp72;
    USE(tmp72);
    tmp72 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(2));
    compiler::TNode<Object> tmp73;
    USE(tmp73);
    tmp73 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp67}, compiler::TNode<RawPtrT>{tmp68}, compiler::TNode<IntPtrT>{tmp69}}, compiler::TNode<IntPtrT>{tmp72}));
    ca_.Goto(&block12, tmp65, tmp66, tmp67, tmp68, tmp69, tmp70, tmp71, tmp73);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp74;
    compiler::TNode<Object> tmp75;
    compiler::TNode<RawPtrT> tmp76;
    compiler::TNode<RawPtrT> tmp77;
    compiler::TNode<IntPtrT> tmp78;
    compiler::TNode<Object> tmp79;
    compiler::TNode<Object> tmp80;
    ca_.Bind(&block10, &tmp74, &tmp75, &tmp76, &tmp77, &tmp78, &tmp79, &tmp80);
    compiler::TNode<Oddball> tmp81;
    USE(tmp81);
    tmp81 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block11, tmp74, tmp75, tmp76, tmp77, tmp78, tmp79, tmp80, tmp81);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp82;
    compiler::TNode<Object> tmp83;
    compiler::TNode<RawPtrT> tmp84;
    compiler::TNode<RawPtrT> tmp85;
    compiler::TNode<IntPtrT> tmp86;
    compiler::TNode<Object> tmp87;
    compiler::TNode<Object> tmp88;
    compiler::TNode<Object> tmp89;
    ca_.Bind(&block12, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89);
    ca_.Goto(&block11, tmp82, tmp83, tmp84, tmp85, tmp86, tmp87, tmp88, tmp89);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp90;
    compiler::TNode<Object> tmp91;
    compiler::TNode<RawPtrT> tmp92;
    compiler::TNode<RawPtrT> tmp93;
    compiler::TNode<IntPtrT> tmp94;
    compiler::TNode<Object> tmp95;
    compiler::TNode<Object> tmp96;
    compiler::TNode<Object> tmp97;
    ca_.Bind(&block11, &tmp90, &tmp91, &tmp92, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 828);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 831);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 830);
    compiler::TNode<Object> tmp98;
    USE(tmp98);
    tmp98 = ca_.UncheckedCast<Object>(DataViewBuiltinsFromDSLAssembler(state_).DataViewSet(compiler::TNode<Context>{tmp90}, compiler::TNode<Object>{tmp91}, compiler::TNode<Object>{tmp95}, compiler::TNode<Object>{tmp96}, compiler::TNode<Object>{tmp97}, BIGUINT64_ELEMENTS));
    arguments.PopAndReturn(tmp98);
  }
}

TF_BUILTIN(DataViewPrototypeSetBigInt64, CodeStubAssembler) {
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
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block1(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT> block2(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block4(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block3(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block5(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object> block6(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block8(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block7(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block9(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object> block10(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, Object> block12(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
  compiler::CodeAssemblerParameterizedLabel<Context, Object, RawPtrT, RawPtrT, IntPtrT, Object, Object, Object> block11(&ca_, compiler::CodeAssemblerLabel::kNonDeferred);
    ca_.Goto(&block0, parameter0, parameter1, torque_arguments.frame, torque_arguments.base, torque_arguments.length);

  if (block0.is_used()) {
    compiler::TNode<Context> tmp0;
    compiler::TNode<Object> tmp1;
    compiler::TNode<RawPtrT> tmp2;
    compiler::TNode<RawPtrT> tmp3;
    compiler::TNode<IntPtrT> tmp4;
    ca_.Bind(&block0, &tmp0, &tmp1, &tmp2, &tmp3, &tmp4);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 836);
    compiler::TNode<IntPtrT> tmp5;
    USE(tmp5);
    tmp5 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<BoolT> tmp6;
    USE(tmp6);
    tmp6 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp4}, compiler::TNode<IntPtrT>{tmp5}));
    ca_.Branch(tmp6, &block1, &block2, tmp0, tmp1, tmp2, tmp3, tmp4);
  }

  if (block1.is_used()) {
    compiler::TNode<Context> tmp7;
    compiler::TNode<Object> tmp8;
    compiler::TNode<RawPtrT> tmp9;
    compiler::TNode<RawPtrT> tmp10;
    compiler::TNode<IntPtrT> tmp11;
    ca_.Bind(&block1, &tmp7, &tmp8, &tmp9, &tmp10, &tmp11);
    compiler::TNode<IntPtrT> tmp12;
    USE(tmp12);
    tmp12 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(0));
    compiler::TNode<Object> tmp13;
    USE(tmp13);
    tmp13 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp9}, compiler::TNode<RawPtrT>{tmp10}, compiler::TNode<IntPtrT>{tmp11}}, compiler::TNode<IntPtrT>{tmp12}));
    ca_.Goto(&block4, tmp7, tmp8, tmp9, tmp10, tmp11, tmp13);
  }

  if (block2.is_used()) {
    compiler::TNode<Context> tmp14;
    compiler::TNode<Object> tmp15;
    compiler::TNode<RawPtrT> tmp16;
    compiler::TNode<RawPtrT> tmp17;
    compiler::TNode<IntPtrT> tmp18;
    ca_.Bind(&block2, &tmp14, &tmp15, &tmp16, &tmp17, &tmp18);
    compiler::TNode<Oddball> tmp19;
    USE(tmp19);
    tmp19 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block3, tmp14, tmp15, tmp16, tmp17, tmp18, tmp19);
  }

  if (block4.is_used()) {
    compiler::TNode<Context> tmp20;
    compiler::TNode<Object> tmp21;
    compiler::TNode<RawPtrT> tmp22;
    compiler::TNode<RawPtrT> tmp23;
    compiler::TNode<IntPtrT> tmp24;
    compiler::TNode<Object> tmp25;
    ca_.Bind(&block4, &tmp20, &tmp21, &tmp22, &tmp23, &tmp24, &tmp25);
    ca_.Goto(&block3, tmp20, tmp21, tmp22, tmp23, tmp24, tmp25);
  }

  if (block3.is_used()) {
    compiler::TNode<Context> tmp26;
    compiler::TNode<Object> tmp27;
    compiler::TNode<RawPtrT> tmp28;
    compiler::TNode<RawPtrT> tmp29;
    compiler::TNode<IntPtrT> tmp30;
    compiler::TNode<Object> tmp31;
    ca_.Bind(&block3, &tmp26, &tmp27, &tmp28, &tmp29, &tmp30, &tmp31);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 837);
    compiler::TNode<IntPtrT> tmp32;
    USE(tmp32);
    tmp32 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<BoolT> tmp33;
    USE(tmp33);
    tmp33 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp30}, compiler::TNode<IntPtrT>{tmp32}));
    ca_.Branch(tmp33, &block5, &block6, tmp26, tmp27, tmp28, tmp29, tmp30, tmp31);
  }

  if (block5.is_used()) {
    compiler::TNode<Context> tmp34;
    compiler::TNode<Object> tmp35;
    compiler::TNode<RawPtrT> tmp36;
    compiler::TNode<RawPtrT> tmp37;
    compiler::TNode<IntPtrT> tmp38;
    compiler::TNode<Object> tmp39;
    ca_.Bind(&block5, &tmp34, &tmp35, &tmp36, &tmp37, &tmp38, &tmp39);
    compiler::TNode<IntPtrT> tmp40;
    USE(tmp40);
    tmp40 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(1));
    compiler::TNode<Object> tmp41;
    USE(tmp41);
    tmp41 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp36}, compiler::TNode<RawPtrT>{tmp37}, compiler::TNode<IntPtrT>{tmp38}}, compiler::TNode<IntPtrT>{tmp40}));
    ca_.Goto(&block8, tmp34, tmp35, tmp36, tmp37, tmp38, tmp39, tmp41);
  }

  if (block6.is_used()) {
    compiler::TNode<Context> tmp42;
    compiler::TNode<Object> tmp43;
    compiler::TNode<RawPtrT> tmp44;
    compiler::TNode<RawPtrT> tmp45;
    compiler::TNode<IntPtrT> tmp46;
    compiler::TNode<Object> tmp47;
    ca_.Bind(&block6, &tmp42, &tmp43, &tmp44, &tmp45, &tmp46, &tmp47);
    compiler::TNode<Oddball> tmp48;
    USE(tmp48);
    tmp48 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block7, tmp42, tmp43, tmp44, tmp45, tmp46, tmp47, tmp48);
  }

  if (block8.is_used()) {
    compiler::TNode<Context> tmp49;
    compiler::TNode<Object> tmp50;
    compiler::TNode<RawPtrT> tmp51;
    compiler::TNode<RawPtrT> tmp52;
    compiler::TNode<IntPtrT> tmp53;
    compiler::TNode<Object> tmp54;
    compiler::TNode<Object> tmp55;
    ca_.Bind(&block8, &tmp49, &tmp50, &tmp51, &tmp52, &tmp53, &tmp54, &tmp55);
    ca_.Goto(&block7, tmp49, tmp50, tmp51, tmp52, tmp53, tmp54, tmp55);
  }

  if (block7.is_used()) {
    compiler::TNode<Context> tmp56;
    compiler::TNode<Object> tmp57;
    compiler::TNode<RawPtrT> tmp58;
    compiler::TNode<RawPtrT> tmp59;
    compiler::TNode<IntPtrT> tmp60;
    compiler::TNode<Object> tmp61;
    compiler::TNode<Object> tmp62;
    ca_.Bind(&block7, &tmp56, &tmp57, &tmp58, &tmp59, &tmp60, &tmp61, &tmp62);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 839);
    compiler::TNode<IntPtrT> tmp63;
    USE(tmp63);
    tmp63 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(2));
    compiler::TNode<BoolT> tmp64;
    USE(tmp64);
    tmp64 = ca_.UncheckedCast<BoolT>(CodeStubAssembler(state_).IntPtrGreaterThan(compiler::TNode<IntPtrT>{tmp60}, compiler::TNode<IntPtrT>{tmp63}));
    ca_.Branch(tmp64, &block9, &block10, tmp56, tmp57, tmp58, tmp59, tmp60, tmp61, tmp62);
  }

  if (block9.is_used()) {
    compiler::TNode<Context> tmp65;
    compiler::TNode<Object> tmp66;
    compiler::TNode<RawPtrT> tmp67;
    compiler::TNode<RawPtrT> tmp68;
    compiler::TNode<IntPtrT> tmp69;
    compiler::TNode<Object> tmp70;
    compiler::TNode<Object> tmp71;
    ca_.Bind(&block9, &tmp65, &tmp66, &tmp67, &tmp68, &tmp69, &tmp70, &tmp71);
    compiler::TNode<IntPtrT> tmp72;
    USE(tmp72);
    tmp72 = ca_.UncheckedCast<IntPtrT>(BaseBuiltinsFromDSLAssembler(state_).FromConstexpr8ATintptr17ATconstexpr_int31(2));
    compiler::TNode<Object> tmp73;
    USE(tmp73);
    tmp73 = ca_.UncheckedCast<Object>(CodeStubAssembler(state_).GetArgumentValue(BaseBuiltinsFromDSLAssembler::Arguments{compiler::TNode<RawPtrT>{tmp67}, compiler::TNode<RawPtrT>{tmp68}, compiler::TNode<IntPtrT>{tmp69}}, compiler::TNode<IntPtrT>{tmp72}));
    ca_.Goto(&block12, tmp65, tmp66, tmp67, tmp68, tmp69, tmp70, tmp71, tmp73);
  }

  if (block10.is_used()) {
    compiler::TNode<Context> tmp74;
    compiler::TNode<Object> tmp75;
    compiler::TNode<RawPtrT> tmp76;
    compiler::TNode<RawPtrT> tmp77;
    compiler::TNode<IntPtrT> tmp78;
    compiler::TNode<Object> tmp79;
    compiler::TNode<Object> tmp80;
    ca_.Bind(&block10, &tmp74, &tmp75, &tmp76, &tmp77, &tmp78, &tmp79, &tmp80);
    compiler::TNode<Oddball> tmp81;
    USE(tmp81);
    tmp81 = BaseBuiltinsFromDSLAssembler(state_).Undefined();
    ca_.Goto(&block11, tmp74, tmp75, tmp76, tmp77, tmp78, tmp79, tmp80, tmp81);
  }

  if (block12.is_used()) {
    compiler::TNode<Context> tmp82;
    compiler::TNode<Object> tmp83;
    compiler::TNode<RawPtrT> tmp84;
    compiler::TNode<RawPtrT> tmp85;
    compiler::TNode<IntPtrT> tmp86;
    compiler::TNode<Object> tmp87;
    compiler::TNode<Object> tmp88;
    compiler::TNode<Object> tmp89;
    ca_.Bind(&block12, &tmp82, &tmp83, &tmp84, &tmp85, &tmp86, &tmp87, &tmp88, &tmp89);
    ca_.Goto(&block11, tmp82, tmp83, tmp84, tmp85, tmp86, tmp87, tmp88, tmp89);
  }

  if (block11.is_used()) {
    compiler::TNode<Context> tmp90;
    compiler::TNode<Object> tmp91;
    compiler::TNode<RawPtrT> tmp92;
    compiler::TNode<RawPtrT> tmp93;
    compiler::TNode<IntPtrT> tmp94;
    compiler::TNode<Object> tmp95;
    compiler::TNode<Object> tmp96;
    compiler::TNode<Object> tmp97;
    ca_.Bind(&block11, &tmp90, &tmp91, &tmp92, &tmp93, &tmp94, &tmp95, &tmp96, &tmp97);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 838);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 841);
    ca_.SetSourcePosition("../../v8/src/builtins/data-view.tq", 840);
    compiler::TNode<Object> tmp98;
    USE(tmp98);
    tmp98 = ca_.UncheckedCast<Object>(DataViewBuiltinsFromDSLAssembler(state_).DataViewSet(compiler::TNode<Context>{tmp90}, compiler::TNode<Object>{tmp91}, compiler::TNode<Object>{tmp95}, compiler::TNode<Object>{tmp96}, compiler::TNode<Object>{tmp97}, BIGINT64_ELEMENTS));
    arguments.PopAndReturn(tmp98);
  }
}

}  // namespace internal
}  // namespace v8

