// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/fpdfapi/parser/cpdf_indirect_object_holder.h"

#include <memory>
#include <utility>

#include "core/fpdfapi/parser/cpdf_null.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/base/ptr_util.h"

namespace {

class MockIndirectObjectHolder final : public CPDF_IndirectObjectHolder {
 public:
  MockIndirectObjectHolder() {}
  ~MockIndirectObjectHolder() override {}

  MOCK_METHOD1(ParseIndirectObject,
               std::unique_ptr<CPDF_Object>(uint32_t objnum));
};

}  // namespace

TEST(CPDF_IndirectObjectHolderTest, RecursiveParseOfSameObject) {
  MockIndirectObjectHolder mock_holder;
  // ParseIndirectObject should not be called again on recursively same object
  // parse request.
  EXPECT_CALL(mock_holder, ParseIndirectObject(::testing::_))
      .WillOnce(::testing::WithArg<0>(::testing::Invoke(
          [&mock_holder](uint32_t objnum) -> std::unique_ptr<CPDF_Object> {
            const CPDF_Object* same_parse =
                mock_holder.GetOrParseIndirectObject(objnum);
            CHECK(!same_parse);
            return pdfium::MakeUnique<CPDF_Null>();
          })));

  EXPECT_TRUE(mock_holder.GetOrParseIndirectObject(1000));
}

TEST(CPDF_IndirectObjectHolderTest, GetObjectMethods) {
  static constexpr uint32_t kObjNum = 1000;
  MockIndirectObjectHolder mock_holder;

  EXPECT_CALL(mock_holder, ParseIndirectObject(::testing::_)).Times(0);
  EXPECT_FALSE(mock_holder.GetIndirectObject(kObjNum));
  ::testing::Mock::VerifyAndClearExpectations(&mock_holder);

  EXPECT_CALL(mock_holder, ParseIndirectObject(::testing::_))
      .WillOnce(::testing::WithArg<0>(::testing::Invoke(
          [](uint32_t objnum) -> std::unique_ptr<CPDF_Object> {
            return pdfium::MakeUnique<CPDF_Null>();
          })));
  EXPECT_TRUE(mock_holder.GetOrParseIndirectObject(kObjNum));
  ::testing::Mock::VerifyAndClearExpectations(&mock_holder);

  EXPECT_CALL(mock_holder, ParseIndirectObject(::testing::_)).Times(0);
  ASSERT_TRUE(mock_holder.GetIndirectObject(kObjNum));
  ::testing::Mock::VerifyAndClearExpectations(&mock_holder);

  EXPECT_EQ(kObjNum, mock_holder.GetIndirectObject(kObjNum)->GetObjNum());
}

TEST(CPDF_IndirectObjectHolderTest, ParseInvalidObjNum) {
  MockIndirectObjectHolder mock_holder;

  EXPECT_CALL(mock_holder, ParseIndirectObject(::testing::_)).Times(0);
  EXPECT_FALSE(
      mock_holder.GetOrParseIndirectObject(CPDF_Object::kInvalidObjNum));
}

TEST(CPDF_IndirectObjectHolderTest, ReplaceObjectWithInvalidObjNum) {
  MockIndirectObjectHolder mock_holder;

  EXPECT_CALL(mock_holder, ParseIndirectObject(::testing::_)).Times(0);
  EXPECT_FALSE(mock_holder.ReplaceIndirectObjectIfHigherGeneration(
      CPDF_Object::kInvalidObjNum, pdfium::MakeUnique<CPDF_Null>()));
}
