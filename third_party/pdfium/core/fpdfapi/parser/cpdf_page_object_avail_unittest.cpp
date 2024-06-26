// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/fpdfapi/parser/cpdf_page_object_avail.h"

#include <map>
#include <memory>
#include <utility>

#include "core/fpdfapi/parser/cpdf_array.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_indirect_object_holder.h"
#include "core/fpdfapi/parser/cpdf_read_validator.h"
#include "core/fpdfapi/parser/cpdf_reference.h"
#include "core/fpdfapi/parser/cpdf_string.h"
#include "core/fxcrt/fx_stream.h"
#include "testing/fx_string_testhelpers.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/base/ptr_util.h"

namespace {

class TestReadValidator final : public CPDF_ReadValidator {
 public:
  template <typename T, typename... Args>
  friend RetainPtr<T> pdfium::MakeRetain(Args&&... args);

  void SimulateReadError() { ReadBlockAtOffset(nullptr, 0, 1); }

 private:
  TestReadValidator()
      : CPDF_ReadValidator(
            pdfium::MakeRetain<CFX_InvalidSeekableReadStream>(100),
            nullptr) {}
  ~TestReadValidator() override {}
};

class TestHolder final : public CPDF_IndirectObjectHolder {
 public:
  enum class ObjectState {
    Unavailable,
    Available,
  };
  TestHolder() : validator_(pdfium::MakeRetain<TestReadValidator>()) {}
  ~TestHolder() override {}

  // CPDF_IndirectObjectHolder overrides:
  CPDF_Object* GetOrParseIndirectObject(uint32_t objnum) override {
    auto it = objects_data_.find(objnum);
    if (it == objects_data_.end())
      return nullptr;

    ObjectData& obj_data = it->second;
    if (obj_data.state == ObjectState::Unavailable) {
      validator_->SimulateReadError();
      return nullptr;
    }
    return obj_data.object.get();
  }

  RetainPtr<CPDF_ReadValidator> GetValidator() { return validator_; }

  void AddObject(uint32_t objnum,
                 std::unique_ptr<CPDF_Object> object,
                 ObjectState state) {
    ObjectData object_data;
    object_data.object = std::move(object);
    object_data.state = state;
    ASSERT(objects_data_.find(objnum) == objects_data_.end());
    objects_data_[objnum] = std::move(object_data);
  }

  void SetObjectState(uint32_t objnum, ObjectState state) {
    auto it = objects_data_.find(objnum);
    ASSERT(it != objects_data_.end());
    ObjectData& obj_data = it->second;
    obj_data.state = state;
  }

  CPDF_Object* GetTestObject(uint32_t objnum) {
    auto it = objects_data_.find(objnum);
    if (it == objects_data_.end())
      return nullptr;
    return it->second.object.get();
  }

 private:
  struct ObjectData {
    std::unique_ptr<CPDF_Object> object;
    ObjectState state = ObjectState::Unavailable;
  };
  std::map<uint32_t, ObjectData> objects_data_;
  RetainPtr<TestReadValidator> validator_;
};

}  // namespace

TEST(CPDF_PageObjectAvailTest, ExcludePages) {
  TestHolder holder;
  holder.AddObject(1, pdfium::MakeUnique<CPDF_Dictionary>(),
                   TestHolder::ObjectState::Available);
  holder.GetTestObject(1)->GetDict()->SetNewFor<CPDF_Reference>("Kids", &holder,
                                                                2);
  holder.AddObject(2, pdfium::MakeUnique<CPDF_Array>(),
                   TestHolder::ObjectState::Available);
  holder.GetTestObject(2)->AsArray()->AddNew<CPDF_Reference>(&holder, 3);

  holder.AddObject(3, pdfium::MakeUnique<CPDF_Dictionary>(),
                   TestHolder::ObjectState::Available);
  holder.GetTestObject(3)->GetDict()->SetFor(
      "Type", pdfium::MakeUnique<CPDF_String>(nullptr, "Page", false));
  holder.GetTestObject(3)->GetDict()->SetNewFor<CPDF_Reference>("OtherPageData",
                                                                &holder, 4);
  // Add unavailable object related to other page.
  holder.AddObject(
      4, pdfium::MakeUnique<CPDF_String>(nullptr, "Other page data", false),
      TestHolder::ObjectState::Unavailable);

  CPDF_PageObjectAvail avail(holder.GetValidator().Get(), &holder, 1);
  // Now object should be available, although the object '4' is not available,
  // because it is in skipped other page.
  EXPECT_EQ(CPDF_DataAvail::DocAvailStatus::DataAvailable, avail.CheckAvail());
}
