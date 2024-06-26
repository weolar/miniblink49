// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "xfa/fxfa/fm2js/cxfa_fmexpression.h"

#include <memory>
#include <utility>

#include "core/fxcrt/cfx_widetextbuf.h"
#include "core/fxcrt/fx_string.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "testing/test_support.h"
#include "third_party/base/ptr_util.h"
#include "xfa/fxfa/fm2js/cxfa_fmtojavascriptdepth.h"

TEST(CXFA_FMExpressionTest, VarExpressionInitNull) {
  CXFA_FMToJavaScriptDepth::Reset();
  CFX_WideTextBuf accumulator;

  CXFA_FMVarExpression(L"s", nullptr)
      .ToJavaScript(&accumulator, ReturnType::kInfered);
  EXPECT_STREQ(
      LR"***(var s = "";
)***",
      accumulator.MakeString().c_str());
}

TEST(CXFA_FMExpressionTest, VarExpressionInitBlank) {
  CXFA_FMToJavaScriptDepth::Reset();
  CFX_WideTextBuf accumulator;

  auto init = pdfium::MakeUnique<CXFA_FMStringExpression>(LR"("")");
  CXFA_FMVarExpression(L"s", std::move(init))
      .ToJavaScript(&accumulator, ReturnType::kInfered);
  EXPECT_STREQ(
      LR"***(var s = "";
s = pfm_rt.var_filter(s);
)***",
      accumulator.MakeString().c_str());
}

TEST(CXFA_FMExpressionTest, VarExpressionInitString) {
  CXFA_FMToJavaScriptDepth::Reset();
  CFX_WideTextBuf accumulator;

  auto init = pdfium::MakeUnique<CXFA_FMStringExpression>(LR"("foo")");
  CXFA_FMVarExpression(L"s", std::move(init))
      .ToJavaScript(&accumulator, ReturnType::kInfered);
  EXPECT_STREQ(
      LR"***(var s = "foo";
s = pfm_rt.var_filter(s);
)***",
      accumulator.MakeString().c_str());
}

TEST(CXFA_FMExpressionTest, VarExpressionInitNumeric) {
  CXFA_FMToJavaScriptDepth::Reset();
  CFX_WideTextBuf accumulator;

  auto init = pdfium::MakeUnique<CXFA_FMNumberExpression>(L"112");
  CXFA_FMVarExpression(L"s", std::move(init))
      .ToJavaScript(&accumulator, ReturnType::kInfered);
  EXPECT_STREQ(
      LR"***(var s = 112;
s = pfm_rt.var_filter(s);
)***",
      accumulator.MakeString().c_str());
}
