// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/fxcrt/weak_ptr.h"

#include <memory>
#include <utility>

#include "testing/gtest/include/gtest/gtest.h"

namespace fxcrt {
namespace {

class PseudoDeletable;
using WeakTestPtr = WeakPtr<PseudoDeletable, ReleaseDeleter<PseudoDeletable>>;
using UniqueTestPtr =
    std::unique_ptr<PseudoDeletable, ReleaseDeleter<PseudoDeletable>>;

class PseudoDeletable {
 public:
  PseudoDeletable() : delete_count_(0) {}
  void Release() {
    ++delete_count_;
    next_.Reset();
  }
  void SetNext(const WeakTestPtr& next) { next_ = next; }
  int delete_count() const { return delete_count_; }

 private:
  int delete_count_;
  WeakTestPtr next_;
};

}  // namespace

TEST(WeakPtr, Null) {
  WeakTestPtr ptr1;
  EXPECT_FALSE(ptr1);

  WeakTestPtr ptr2;
  EXPECT_TRUE(ptr1 == ptr2);
  EXPECT_FALSE(ptr1 != ptr2);

  WeakTestPtr ptr3(ptr1);
  EXPECT_TRUE(ptr1 == ptr3);
  EXPECT_FALSE(ptr1 != ptr3);

  WeakTestPtr ptr4 = ptr1;
  EXPECT_TRUE(ptr1 == ptr4);
  EXPECT_FALSE(ptr1 != ptr4);
}

TEST(WeakPtr, NonNull) {
  PseudoDeletable thing;
  EXPECT_EQ(0, thing.delete_count());
  {
    UniqueTestPtr unique(&thing);
    WeakTestPtr ptr1(std::move(unique));
    EXPECT_TRUE(ptr1);
    EXPECT_EQ(&thing, ptr1.Get());

    WeakTestPtr ptr2;
    EXPECT_FALSE(ptr1 == ptr2);
    EXPECT_TRUE(ptr1 != ptr2);
    {
      WeakTestPtr ptr3(ptr1);
      EXPECT_TRUE(ptr1 == ptr3);
      EXPECT_FALSE(ptr1 != ptr3);
      EXPECT_EQ(&thing, ptr3.Get());
      {
        WeakTestPtr ptr4 = ptr1;
        EXPECT_TRUE(ptr1 == ptr4);
        EXPECT_FALSE(ptr1 != ptr4);
        EXPECT_EQ(&thing, ptr4.Get());
      }
    }
    EXPECT_EQ(0, thing.delete_count());
  }
  EXPECT_EQ(1, thing.delete_count());
}

TEST(WeakPtr, ResetNull) {
  PseudoDeletable thing;
  {
    UniqueTestPtr unique(&thing);
    WeakTestPtr ptr1(std::move(unique));
    WeakTestPtr ptr2 = ptr1;
    ptr1.Reset();
    EXPECT_FALSE(ptr1);
    EXPECT_EQ(nullptr, ptr1.Get());
    EXPECT_TRUE(ptr2);
    EXPECT_EQ(&thing, ptr2.Get());
    EXPECT_FALSE(ptr1 == ptr2);
    EXPECT_TRUE(ptr1 != ptr2);
    EXPECT_EQ(0, thing.delete_count());
  }
  EXPECT_EQ(1, thing.delete_count());
}

TEST(WeakPtr, ResetNonNull) {
  PseudoDeletable thing1;
  PseudoDeletable thing2;
  {
    UniqueTestPtr unique1(&thing1);
    WeakTestPtr ptr1(std::move(unique1));
    WeakTestPtr ptr2 = ptr1;
    UniqueTestPtr unique2(&thing2);
    ptr2.Reset(std::move(unique2));
    EXPECT_TRUE(ptr1);
    EXPECT_EQ(&thing1, ptr1.Get());
    EXPECT_TRUE(ptr2);
    EXPECT_EQ(&thing2, ptr2.Get());
    EXPECT_FALSE(ptr1 == ptr2);
    EXPECT_TRUE(ptr1 != ptr2);
    EXPECT_EQ(0, thing1.delete_count());
    EXPECT_EQ(0, thing2.delete_count());
  }
  EXPECT_EQ(1, thing1.delete_count());
  EXPECT_EQ(1, thing2.delete_count());
}

TEST(WeakPtr, DeleteObject) {
  PseudoDeletable thing;
  {
    UniqueTestPtr unique(&thing);
    WeakTestPtr ptr1(std::move(unique));
    WeakTestPtr ptr2 = ptr1;
    ptr1.DeleteObject();
    EXPECT_FALSE(ptr1);
    EXPECT_EQ(nullptr, ptr1.Get());
    EXPECT_FALSE(ptr2);
    EXPECT_EQ(nullptr, ptr2.Get());
    EXPECT_FALSE(ptr1 == ptr2);
    EXPECT_TRUE(ptr1 != ptr2);
    EXPECT_EQ(1, thing.delete_count());
  }
  EXPECT_EQ(1, thing.delete_count());
}

TEST(WeakPtr, Cyclic) {
  PseudoDeletable thing1;
  PseudoDeletable thing2;
  {
    UniqueTestPtr unique1(&thing1);
    UniqueTestPtr unique2(&thing2);
    WeakTestPtr ptr1(std::move(unique1));
    WeakTestPtr ptr2(std::move(unique2));
    ptr1->SetNext(ptr2);
    ptr2->SetNext(ptr1);
  }
  // Leaks without explicit clear.
  EXPECT_EQ(0, thing1.delete_count());
  EXPECT_EQ(0, thing2.delete_count());
}

TEST(WeakPtr, CyclicDeleteObject) {
  PseudoDeletable thing1;
  PseudoDeletable thing2;
  {
    UniqueTestPtr unique1(&thing1);
    UniqueTestPtr unique2(&thing2);
    WeakTestPtr ptr1(std::move(unique1));
    WeakTestPtr ptr2(std::move(unique2));
    ptr1->SetNext(ptr2);
    ptr2->SetNext(ptr1);
    ptr1.DeleteObject();
    EXPECT_EQ(1, thing1.delete_count());
    EXPECT_EQ(0, thing2.delete_count());
  }
  EXPECT_EQ(1, thing1.delete_count());
  EXPECT_EQ(1, thing2.delete_count());
}

}  // namespace fxcrt
