// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/fpdfapi/parser/cpdf_object_avail.h"

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

class CPDF_ObjectAvailFailOnExclude final : public CPDF_ObjectAvail {
 public:
  using CPDF_ObjectAvail::CPDF_ObjectAvail;
  ~CPDF_ObjectAvailFailOnExclude() override {}
  bool ExcludeObject(const CPDF_Object* object) const override {
    NOTREACHED();
    return false;
  }
};

class CPDF_ObjectAvailExcludeArray final : public CPDF_ObjectAvail {
 public:
  using CPDF_ObjectAvail::CPDF_ObjectAvail;
  ~CPDF_ObjectAvailExcludeArray() override {}
  bool ExcludeObject(const CPDF_Object* object) const override {
    return object->IsArray();
  }
};

class CPDF_ObjectAvailExcludeTypeKey final : public CPDF_ObjectAvail {
 public:
  using CPDF_ObjectAvail::CPDF_ObjectAvail;
  ~CPDF_ObjectAvailExcludeTypeKey() override {}
  bool ExcludeObject(const CPDF_Object* object) const override {
    // The value of "Type" may be reference, and if it is not available, we can
    // incorrect filter objects.
    // In this case CPDF_ObjectAvail should wait availability of this item and
    // call ExcludeObject again.
    return object->IsDictionary() &&
           object->GetDict()->GetStringFor("Type") == "Exclude me";
  }
};

}  // namespace

TEST(CPDF_ObjectAvailTest, OneObject) {
  TestHolder holder;
  holder.AddObject(1, pdfium::MakeUnique<CPDF_String>(nullptr, "string", false),
                   TestHolder::ObjectState::Unavailable);
  CPDF_ObjectAvail avail(holder.GetValidator().Get(), &holder, 1);
  EXPECT_EQ(CPDF_DataAvail::DocAvailStatus::DataNotAvailable,
            avail.CheckAvail());
  holder.SetObjectState(1, TestHolder::ObjectState::Available);
  EXPECT_EQ(CPDF_DataAvail::DocAvailStatus::DataAvailable, avail.CheckAvail());
}

TEST(CPDF_ObjectAvailTest, OneReferencedObject) {
  TestHolder holder;
  holder.AddObject(1, pdfium::MakeUnique<CPDF_Reference>(&holder, 2),
                   TestHolder::ObjectState::Unavailable);
  holder.AddObject(2, pdfium::MakeUnique<CPDF_String>(nullptr, "string", false),
                   TestHolder::ObjectState::Unavailable);
  CPDF_ObjectAvail avail(holder.GetValidator().Get(), &holder, 1);
  EXPECT_EQ(CPDF_DataAvail::DocAvailStatus::DataNotAvailable,
            avail.CheckAvail());

  holder.SetObjectState(1, TestHolder::ObjectState::Available);
  EXPECT_EQ(CPDF_DataAvail::DocAvailStatus::DataNotAvailable,
            avail.CheckAvail());

  holder.SetObjectState(2, TestHolder::ObjectState::Available);
  EXPECT_EQ(CPDF_DataAvail::DocAvailStatus::DataAvailable, avail.CheckAvail());
}

TEST(CPDF_ObjectAvailTest, CycledReferences) {
  TestHolder holder;
  holder.AddObject(1, pdfium::MakeUnique<CPDF_Reference>(&holder, 2),
                   TestHolder::ObjectState::Unavailable);
  holder.AddObject(2, pdfium::MakeUnique<CPDF_Reference>(&holder, 3),
                   TestHolder::ObjectState::Unavailable);
  holder.AddObject(3, pdfium::MakeUnique<CPDF_Reference>(&holder, 1),
                   TestHolder::ObjectState::Unavailable);

  CPDF_ObjectAvail avail(holder.GetValidator().Get(), &holder, 1);
  EXPECT_EQ(CPDF_DataAvail::DocAvailStatus::DataNotAvailable,
            avail.CheckAvail());

  holder.SetObjectState(1, TestHolder::ObjectState::Available);
  EXPECT_EQ(CPDF_DataAvail::DocAvailStatus::DataNotAvailable,
            avail.CheckAvail());

  holder.SetObjectState(2, TestHolder::ObjectState::Available);
  EXPECT_EQ(CPDF_DataAvail::DocAvailStatus::DataNotAvailable,
            avail.CheckAvail());

  holder.SetObjectState(3, TestHolder::ObjectState::Available);
  EXPECT_EQ(CPDF_DataAvail::DocAvailStatus::DataAvailable, avail.CheckAvail());
}

TEST(CPDF_ObjectAvailTest, DoNotCheckParent) {
  TestHolder holder;
  holder.AddObject(1, pdfium::MakeUnique<CPDF_Dictionary>(),
                   TestHolder::ObjectState::Unavailable);
  holder.AddObject(2, pdfium::MakeUnique<CPDF_Dictionary>(),
                   TestHolder::ObjectState::Unavailable);

  holder.GetTestObject(2)->GetDict()->SetNewFor<CPDF_Reference>("Parent",
                                                                &holder, 1);

  CPDF_ObjectAvail avail(holder.GetValidator().Get(), &holder, 2);
  EXPECT_EQ(CPDF_DataAvail::DocAvailStatus::DataNotAvailable,
            avail.CheckAvail());

  holder.SetObjectState(2, TestHolder::ObjectState::Available);
  //  Object should be available in case when "Parent" object is unavailable.
  EXPECT_EQ(CPDF_DataAvail::DocAvailStatus::DataAvailable, avail.CheckAvail());
}

TEST(CPDF_ObjectAvailTest, Generic) {
  TestHolder holder;
  const uint32_t kDepth = 100;
  for (uint32_t i = 1; i < kDepth; ++i) {
    holder.AddObject(i, pdfium::MakeUnique<CPDF_Dictionary>(),
                     TestHolder::ObjectState::Unavailable);
    // Add ref to next dictionary.
    holder.GetTestObject(i)->GetDict()->SetNewFor<CPDF_Reference>(
        "Child", &holder, i + 1);
  }
  // Add final object
  holder.AddObject(kDepth, pdfium::MakeUnique<CPDF_Dictionary>(),
                   TestHolder::ObjectState::Unavailable);

  CPDF_ObjectAvail avail(holder.GetValidator().Get(), &holder, 1);

  for (uint32_t i = 1; i <= kDepth; ++i) {
    EXPECT_EQ(CPDF_DataAvail::DocAvailStatus::DataNotAvailable,
              avail.CheckAvail());
    holder.SetObjectState(i, TestHolder::ObjectState::Available);
  }
  EXPECT_EQ(CPDF_DataAvail::DocAvailStatus::DataAvailable, avail.CheckAvail());
}

TEST(CPDF_ObjectAvailTest, NotExcludeRoot) {
  TestHolder holder;
  holder.AddObject(1, pdfium::MakeUnique<CPDF_Dictionary>(),
                   TestHolder::ObjectState::Available);
  CPDF_ObjectAvailFailOnExclude avail(holder.GetValidator().Get(), &holder, 1);
  EXPECT_EQ(CPDF_DataAvail::DocAvailStatus::DataAvailable, avail.CheckAvail());
}

TEST(CPDF_ObjectAvailTest, NotExcludeReferedRoot) {
  TestHolder holder;
  holder.AddObject(1, pdfium::MakeUnique<CPDF_Reference>(&holder, 2),
                   TestHolder::ObjectState::Available);
  holder.AddObject(2, pdfium::MakeUnique<CPDF_Dictionary>(),
                   TestHolder::ObjectState::Available);
  CPDF_ObjectAvailFailOnExclude avail(holder.GetValidator().Get(), &holder, 1);
  EXPECT_EQ(CPDF_DataAvail::DocAvailStatus::DataAvailable, avail.CheckAvail());
}

TEST(CPDF_ObjectAvailTest, Exclude) {
  TestHolder holder;
  holder.AddObject(1, pdfium::MakeUnique<CPDF_Dictionary>(),
                   TestHolder::ObjectState::Available);
  holder.GetTestObject(1)->GetDict()->SetNewFor<CPDF_Reference>("ArrayRef",
                                                                &holder, 2);
  holder.AddObject(2, pdfium::MakeUnique<CPDF_Array>(),
                   TestHolder::ObjectState::Available);
  holder.GetTestObject(2)->AsArray()->AddNew<CPDF_Reference>(&holder, 2);

  // Add string, which is refered by array item. It is should not be checked.
  holder.AddObject(
      3,
      pdfium::MakeUnique<CPDF_String>(nullptr, "Not available string", false),
      TestHolder::ObjectState::Unavailable);
  CPDF_ObjectAvailExcludeArray avail(holder.GetValidator().Get(), &holder, 1);
  EXPECT_EQ(CPDF_DataAvail::DocAvailStatus::DataAvailable, avail.CheckAvail());
}

TEST(CPDF_ObjectAvailTest, ReadErrorOnExclude) {
  TestHolder holder;
  holder.AddObject(1, pdfium::MakeUnique<CPDF_Dictionary>(),
                   TestHolder::ObjectState::Available);
  holder.GetTestObject(1)->GetDict()->SetNewFor<CPDF_Reference>("DictRef",
                                                                &holder, 2);
  holder.AddObject(2, pdfium::MakeUnique<CPDF_Dictionary>(),
                   TestHolder::ObjectState::Available);

  holder.GetTestObject(2)->GetDict()->SetNewFor<CPDF_Reference>("Type", &holder,
                                                                3);
  // The value of "Type" key is not available at start
  holder.AddObject(
      3, pdfium::MakeUnique<CPDF_String>(nullptr, "Exclude me", false),
      TestHolder::ObjectState::Unavailable);

  holder.GetTestObject(2)->GetDict()->SetNewFor<CPDF_Reference>("OtherData",
                                                                &holder, 4);
  // Add string, which is refered by dictionary item. It is should not be
  // checked, because the dictionary with it, should be skipped.
  holder.AddObject(
      4,
      pdfium::MakeUnique<CPDF_String>(nullptr, "Not available string", false),
      TestHolder::ObjectState::Unavailable);

  CPDF_ObjectAvailExcludeTypeKey avail(holder.GetValidator().Get(), &holder, 1);

  EXPECT_EQ(CPDF_DataAvail::DocAvailStatus::DataNotAvailable,
            avail.CheckAvail());

  // Make "Type" value object available.
  holder.SetObjectState(3, TestHolder::ObjectState::Available);

  // Now object should be available, although the object '4' is not available,
  // because it is in skipped dictionary.
  EXPECT_EQ(CPDF_DataAvail::DocAvailStatus::DataAvailable, avail.CheckAvail());
}

TEST(CPDF_ObjectAvailTest, IgnoreNotExistsObject) {
  TestHolder holder;
  holder.AddObject(1, pdfium::MakeUnique<CPDF_Dictionary>(),
                   TestHolder::ObjectState::Available);
  holder.GetTestObject(1)->GetDict()->SetNewFor<CPDF_Reference>(
      "NotExistsObjRef", &holder, 2);
  CPDF_ObjectAvail avail(holder.GetValidator().Get(), &holder, 1);
  // Now object should be available, although the object '2' is not exists. But
  // all exists in file related data are checked.
  EXPECT_EQ(CPDF_DataAvail::DocAvailStatus::DataAvailable, avail.CheckAvail());
}

TEST(CPDF_ObjectAvailTest, CheckTwice) {
  TestHolder holder;
  holder.AddObject(1, pdfium::MakeUnique<CPDF_String>(nullptr, "string", false),
                   TestHolder::ObjectState::Unavailable);

  CPDF_ObjectAvail avail(holder.GetValidator().Get(), &holder, 1);
  EXPECT_EQ(avail.CheckAvail(), avail.CheckAvail());

  holder.SetObjectState(1, TestHolder::ObjectState::Available);
  EXPECT_EQ(avail.CheckAvail(), avail.CheckAvail());
}

TEST(CPDF_ObjectAvailTest, SelfReferedInlinedObject) {
  TestHolder holder;
  holder.AddObject(1, pdfium::MakeUnique<CPDF_Dictionary>(),
                   TestHolder::ObjectState::Available);

  holder.GetTestObject(1)->GetDict()->SetNewFor<CPDF_Reference>("Data", &holder,
                                                                2);
  auto* root =
      holder.GetTestObject(1)->GetDict()->SetNewFor<CPDF_Dictionary>("Dict");

  root->SetNewFor<CPDF_Reference>("Self", &holder, 1);

  holder.AddObject(2, pdfium::MakeUnique<CPDF_String>(nullptr, "Data", false),
                   TestHolder::ObjectState::Unavailable);

  CPDF_ObjectAvail avail(holder.GetValidator().Get(), &holder, root);

  EXPECT_EQ(CPDF_DataAvail::DocAvailStatus::DataNotAvailable,
            avail.CheckAvail());

  holder.SetObjectState(2, TestHolder::ObjectState::Available);

  EXPECT_EQ(CPDF_DataAvail::DocAvailStatus::DataAvailable, avail.CheckAvail());
}
