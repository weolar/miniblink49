// Copyright 2019 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "fxjs/xfa/cfxjse_value.h"

#include <memory>
#include <utility>
#include <vector>

#include "fxjs/xfa/cfxjse_engine.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "testing/xfa_js_embedder_test.h"
#include "third_party/base/ptr_util.h"

class CFXJSE_ValueEmbedderTest : public XFAJSEmbedderTest {};

TEST_F(CFXJSE_ValueEmbedderTest, Empty) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  auto pValue = pdfium::MakeUnique<CFXJSE_Value>(GetIsolate());
  EXPECT_TRUE(pValue->IsEmpty());
  EXPECT_FALSE(pValue->IsUndefined());
  EXPECT_FALSE(pValue->IsNull());
  EXPECT_FALSE(pValue->IsBoolean());
  EXPECT_FALSE(pValue->IsString());
  EXPECT_FALSE(pValue->IsNumber());
  EXPECT_FALSE(pValue->IsObject());
  EXPECT_FALSE(pValue->IsArray());
  EXPECT_FALSE(pValue->IsFunction());
  EXPECT_FALSE(pValue->IsDate());
}

TEST_F(CFXJSE_ValueEmbedderTest, EmptyArrayInsert) {
  ASSERT_TRUE(OpenDocument("simple_xfa.pdf"));

  // Test inserting empty values into arrays.
  auto pValue = pdfium::MakeUnique<CFXJSE_Value>(GetIsolate());
  std::vector<std::unique_ptr<CFXJSE_Value>> vec;
  vec.push_back(std::move(pValue));

  CFXJSE_Value array(GetIsolate());
  array.SetArray(vec);
  EXPECT_TRUE(array.IsArray());
}
