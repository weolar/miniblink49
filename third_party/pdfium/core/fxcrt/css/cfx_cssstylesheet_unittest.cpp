// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcrt/css/cfx_cssstylesheet.h"

#include <memory>
#include <vector>

#include "core/fxcrt/css/cfx_cssdeclaration.h"
#include "core/fxcrt/css/cfx_cssenumvalue.h"
#include "core/fxcrt/css/cfx_cssnumbervalue.h"
#include "core/fxcrt/css/cfx_cssstylerule.h"
#include "core/fxcrt/css/cfx_cssvaluelist.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/base/ptr_util.h"
#include "third_party/base/stl_util.h"

class CFX_CSSStyleSheetTest : public testing::Test {
 public:
  void SetUp() override {
    sheet_ = pdfium::MakeUnique<CFX_CSSStyleSheet>();
    decl_ = nullptr;
  }

  void TearDown() override { decl_ = nullptr; }

  void LoadAndVerifyDecl(const wchar_t* buf,
                         const std::vector<WideString>& selectors,
                         size_t decl_count) {
    ASSERT(sheet_);

    EXPECT_TRUE(sheet_->LoadBuffer(buf, wcslen(buf)));
    EXPECT_EQ(sheet_->CountRules(), 1);

    CFX_CSSStyleRule* style = sheet_->GetRule(0);
    EXPECT_EQ(selectors.size(), style->CountSelectorLists());

    for (size_t i = 0; i < selectors.size(); i++) {
      uint32_t hash = FX_HashCode_GetW(selectors[i].AsStringView(), true);
      EXPECT_EQ(hash, style->GetSelectorList(i)->GetNameHash());
    }

    decl_ = style->GetDeclaration();
    EXPECT_EQ(decl_->PropertyCountForTesting(), decl_count);
  }

  void VerifyFloat(CFX_CSSProperty prop, float val, CFX_CSSNumberType type) {
    ASSERT(decl_);

    bool important;
    RetainPtr<CFX_CSSValue> v = decl_->GetProperty(prop, &important);
    EXPECT_EQ(v->GetType(), CFX_CSSPrimitiveType::Number);
    EXPECT_EQ(v.As<CFX_CSSNumberValue>()->Kind(), type);
    EXPECT_EQ(v.As<CFX_CSSNumberValue>()->Value(), val);
  }

  void VerifyEnum(CFX_CSSProperty prop, CFX_CSSPropertyValue val) {
    ASSERT(decl_);

    bool important;
    RetainPtr<CFX_CSSValue> v = decl_->GetProperty(prop, &important);
    EXPECT_EQ(v->GetType(), CFX_CSSPrimitiveType::Enum);
    EXPECT_EQ(v.As<CFX_CSSEnumValue>()->Value(), val);
  }

  void VerifyList(CFX_CSSProperty prop,
                  std::vector<CFX_CSSPropertyValue> values) {
    ASSERT(decl_);

    bool important;
    RetainPtr<CFX_CSSValueList> list =
        decl_->GetProperty(prop, &important).As<CFX_CSSValueList>();
    EXPECT_EQ(list->CountValues(), pdfium::CollectionSize<int32_t>(values));

    for (size_t i = 0; i < values.size(); i++) {
      RetainPtr<CFX_CSSValue> val = list->GetValue(i);
      EXPECT_EQ(val->GetType(), CFX_CSSPrimitiveType::Enum);
      EXPECT_EQ(val.As<CFX_CSSEnumValue>()->Value(), values[i]);
    }
  }

  std::unique_ptr<CFX_CSSStyleSheet> sheet_;
  CFX_CSSDeclaration* decl_;
};

TEST_F(CFX_CSSStyleSheetTest, ParseMultipleSelectors) {
  const wchar_t* buf =
      L"a { border: 10px; }\nb { text-decoration: underline; }";
  EXPECT_TRUE(sheet_->LoadBuffer(buf, wcslen(buf)));
  EXPECT_EQ(2, sheet_->CountRules());

  CFX_CSSStyleRule* style = sheet_->GetRule(0);
  EXPECT_EQ(1UL, style->CountSelectorLists());

  bool found_selector = false;
  uint32_t hash = FX_HashCode_GetW(L"a", true);
  for (size_t i = 0; i < style->CountSelectorLists(); i++) {
    if (style->GetSelectorList(i)->GetNameHash() == hash) {
      found_selector = true;
      break;
    }
  }
  EXPECT_TRUE(found_selector);

  decl_ = style->GetDeclaration();
  EXPECT_EQ(4UL, decl_->PropertyCountForTesting());

  VerifyFloat(CFX_CSSProperty::BorderLeftWidth, 10.0,
              CFX_CSSNumberType::Pixels);
  VerifyFloat(CFX_CSSProperty::BorderRightWidth, 10.0,
              CFX_CSSNumberType::Pixels);
  VerifyFloat(CFX_CSSProperty::BorderTopWidth, 10.0, CFX_CSSNumberType::Pixels);
  VerifyFloat(CFX_CSSProperty::BorderBottomWidth, 10.0,
              CFX_CSSNumberType::Pixels);

  style = sheet_->GetRule(1);
  EXPECT_EQ(1UL, style->CountSelectorLists());

  found_selector = false;
  hash = FX_HashCode_GetW(L"b", true);
  for (size_t i = 0; i < style->CountSelectorLists(); i++) {
    if (style->GetSelectorList(i)->GetNameHash() == hash) {
      found_selector = true;
      break;
    }
  }
  EXPECT_TRUE(found_selector);

  decl_ = style->GetDeclaration();
  EXPECT_EQ(1UL, decl_->PropertyCountForTesting());
  VerifyList(CFX_CSSProperty::TextDecoration,
             {CFX_CSSPropertyValue::Underline});
}

TEST_F(CFX_CSSStyleSheetTest, ParseChildSelectors) {
  const wchar_t* buf = L"a b c { border: 10px; }";
  EXPECT_TRUE(sheet_->LoadBuffer(buf, wcslen(buf)));
  EXPECT_EQ(1, sheet_->CountRules());

  CFX_CSSStyleRule* style = sheet_->GetRule(0);
  EXPECT_EQ(1UL, style->CountSelectorLists());

  auto* sel = style->GetSelectorList(0);
  EXPECT_TRUE(sel != nullptr);
  EXPECT_EQ(FX_HashCode_GetW(L"c", true), sel->GetNameHash());

  sel = sel->GetNextSelector();
  EXPECT_TRUE(sel != nullptr);
  EXPECT_EQ(FX_HashCode_GetW(L"b", true), sel->GetNameHash());

  sel = sel->GetNextSelector();
  EXPECT_TRUE(sel != nullptr);
  EXPECT_EQ(FX_HashCode_GetW(L"a", true), sel->GetNameHash());

  sel = sel->GetNextSelector();
  EXPECT_TRUE(sel == nullptr);

  decl_ = style->GetDeclaration();
  EXPECT_EQ(4UL, decl_->PropertyCountForTesting());

  VerifyFloat(CFX_CSSProperty::BorderLeftWidth, 10.0,
              CFX_CSSNumberType::Pixels);
  VerifyFloat(CFX_CSSProperty::BorderRightWidth, 10.0,
              CFX_CSSNumberType::Pixels);
  VerifyFloat(CFX_CSSProperty::BorderTopWidth, 10.0, CFX_CSSNumberType::Pixels);
  VerifyFloat(CFX_CSSProperty::BorderBottomWidth, 10.0,
              CFX_CSSNumberType::Pixels);
}

TEST_F(CFX_CSSStyleSheetTest, ParseUnhandledSelectors) {
  const wchar_t* buf = L"a > b { padding: 0; }";
  EXPECT_TRUE(sheet_->LoadBuffer(buf, wcslen(buf)));
  EXPECT_EQ(0, sheet_->CountRules());

  buf = L"a[first] { padding: 0; }";
  EXPECT_TRUE(sheet_->LoadBuffer(buf, wcslen(buf)));
  EXPECT_EQ(0, sheet_->CountRules());

  buf = L"a+b { padding: 0; }";
  EXPECT_TRUE(sheet_->LoadBuffer(buf, wcslen(buf)));
  EXPECT_EQ(0, sheet_->CountRules());

  buf = L"a ^ b { padding: 0; }";
  EXPECT_TRUE(sheet_->LoadBuffer(buf, wcslen(buf)));
  EXPECT_EQ(0, sheet_->CountRules());
}

TEST_F(CFX_CSSStyleSheetTest, ParseMultipleSelectorsCombined) {
  LoadAndVerifyDecl(L"a, b, c { border: 5px; }", {L"a", L"b", L"c"}, 4);
}

TEST_F(CFX_CSSStyleSheetTest, ParseBorder) {
  LoadAndVerifyDecl(L"a { border: 5px; }", {L"a"}, 4);
  VerifyFloat(CFX_CSSProperty::BorderLeftWidth, 5.0, CFX_CSSNumberType::Pixels);
  VerifyFloat(CFX_CSSProperty::BorderRightWidth, 5.0,
              CFX_CSSNumberType::Pixels);
  VerifyFloat(CFX_CSSProperty::BorderTopWidth, 5.0, CFX_CSSNumberType::Pixels);
  VerifyFloat(CFX_CSSProperty::BorderBottomWidth, 5.0,
              CFX_CSSNumberType::Pixels);
}

TEST_F(CFX_CSSStyleSheetTest, ParseBorderFull) {
  LoadAndVerifyDecl(L"a { border: 5px solid red; }", {L"a"}, 4);
  VerifyFloat(CFX_CSSProperty::BorderLeftWidth, 5.0, CFX_CSSNumberType::Pixels);
  VerifyFloat(CFX_CSSProperty::BorderRightWidth, 5.0,
              CFX_CSSNumberType::Pixels);
  VerifyFloat(CFX_CSSProperty::BorderTopWidth, 5.0, CFX_CSSNumberType::Pixels);
  VerifyFloat(CFX_CSSProperty::BorderBottomWidth, 5.0,
              CFX_CSSNumberType::Pixels);
}

TEST_F(CFX_CSSStyleSheetTest, ParseBorderLeft) {
  LoadAndVerifyDecl(L"a { border-left: 2.5pc; }", {L"a"}, 1);
  VerifyFloat(CFX_CSSProperty::BorderLeftWidth, 2.5, CFX_CSSNumberType::Picas);
}

TEST_F(CFX_CSSStyleSheetTest, ParseBorderLeftThick) {
  LoadAndVerifyDecl(L"a { border-left: thick; }", {L"a"}, 1);
  VerifyEnum(CFX_CSSProperty::BorderLeftWidth, CFX_CSSPropertyValue::Thick);
}

TEST_F(CFX_CSSStyleSheetTest, ParseBorderRight) {
  LoadAndVerifyDecl(L"a { border-right: 2.5pc; }", {L"a"}, 1);
  VerifyFloat(CFX_CSSProperty::BorderRightWidth, 2.5, CFX_CSSNumberType::Picas);
}

TEST_F(CFX_CSSStyleSheetTest, ParseBorderTop) {
  LoadAndVerifyDecl(L"a { border-top: 2.5pc; }", {L"a"}, 1);
  VerifyFloat(CFX_CSSProperty::BorderTopWidth, 2.5, CFX_CSSNumberType::Picas);
}

TEST_F(CFX_CSSStyleSheetTest, ParseBorderBottom) {
  LoadAndVerifyDecl(L"a { border-bottom: 2.5pc; }", {L"a"}, 1);
  VerifyFloat(CFX_CSSProperty::BorderBottomWidth, 2.5,
              CFX_CSSNumberType::Picas);
}
