// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file has the unit tests for the IdAllocator class.

#include "gpu/command_buffer/common/id_allocator.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace gpu {

class IdAllocatorTest : public testing::Test {
 protected:
  virtual void SetUp() {}
  virtual void TearDown() {}

  IdAllocator* id_allocator() { return &id_allocator_; }

 private:
  IdAllocator id_allocator_;
};

// Checks basic functionality: AllocateID, FreeID, InUse.
TEST_F(IdAllocatorTest, TestBasic) {
  IdAllocator *allocator = id_allocator();
  // Check that resource 1 is not in use
  EXPECT_FALSE(allocator->InUse(1));

  // Allocate an ID, check that it's in use.
  ResourceId id1 = allocator->AllocateID();
  EXPECT_TRUE(allocator->InUse(id1));

  // Allocate another ID, check that it's in use, and different from the first
  // one.
  ResourceId id2 = allocator->AllocateID();
  EXPECT_TRUE(allocator->InUse(id2));
  EXPECT_NE(id1, id2);

  // Free one of the IDs, check that it's not in use any more.
  allocator->FreeID(id1);
  EXPECT_FALSE(allocator->InUse(id1));

  // Frees the other ID, check that it's not in use any more.
  allocator->FreeID(id2);
  EXPECT_FALSE(allocator->InUse(id2));
}

// Checks that the resource IDs are re-used after being freed.
TEST_F(IdAllocatorTest, TestAdvanced) {
  IdAllocator *allocator = id_allocator();

  // Allocate the highest possible ID, to make life awkward.
  allocator->AllocateIDAtOrAbove(-1);

  // Allocate a significant number of resources.
  const unsigned int kNumResources = 100;
  ResourceId ids[kNumResources];
  for (unsigned int i = 0; i < kNumResources; ++i) {
    ids[i] = allocator->AllocateID();
    EXPECT_TRUE(allocator->InUse(ids[i]));
  }

  // Check that a new allocation re-uses the resource we just freed.
  ResourceId id1 = ids[kNumResources / 2];
  allocator->FreeID(id1);
  EXPECT_FALSE(allocator->InUse(id1));
  ResourceId id2 = allocator->AllocateID();
  EXPECT_TRUE(allocator->InUse(id2));
  EXPECT_EQ(id1, id2);
}

// Checks that we can choose our own ids and they won't be reused.
TEST_F(IdAllocatorTest, MarkAsUsed) {
  IdAllocator* allocator = id_allocator();
  ResourceId id = allocator->AllocateID();
  allocator->FreeID(id);
  EXPECT_FALSE(allocator->InUse(id));
  EXPECT_TRUE(allocator->MarkAsUsed(id));
  EXPECT_TRUE(allocator->InUse(id));
  ResourceId id2 = allocator->AllocateID();
  EXPECT_NE(id, id2);
  EXPECT_TRUE(allocator->MarkAsUsed(id2 + 1));
  ResourceId id3 = allocator->AllocateID();
  // Checks our algorithm. If the algorithm changes this check should be
  // changed.
  EXPECT_EQ(id3, id2 + 2);
}

// Checks AllocateIdAtOrAbove.
TEST_F(IdAllocatorTest, AllocateIdAtOrAbove) {
  const ResourceId kOffset = 123456;
  IdAllocator* allocator = id_allocator();
  ResourceId id1 = allocator->AllocateIDAtOrAbove(kOffset);
  EXPECT_EQ(kOffset, id1);
  ResourceId id2 = allocator->AllocateIDAtOrAbove(kOffset);
  EXPECT_GT(id2, kOffset);
  ResourceId id3 = allocator->AllocateIDAtOrAbove(kOffset);
  EXPECT_GT(id3, kOffset);
}

// Checks that AllocateIdAtOrAbove wraps around at the maximum 32-bit value.
TEST_F(IdAllocatorTest, AllocateIdAtOrAboveWrapsAround) {
  const ResourceId kMaxPossibleOffset = -1;
  IdAllocator* allocator = id_allocator();
  ResourceId id1 = allocator->AllocateIDAtOrAbove(kMaxPossibleOffset);
  EXPECT_EQ(kMaxPossibleOffset, id1);
  ResourceId id2 = allocator->AllocateIDAtOrAbove(kMaxPossibleOffset);
  EXPECT_EQ(1u, id2);
  ResourceId id3 = allocator->AllocateIDAtOrAbove(kMaxPossibleOffset);
  EXPECT_EQ(2u, id3);
}

TEST_F(IdAllocatorTest, RedundantFreeIsIgnored) {
  IdAllocator* allocator = id_allocator();
  ResourceId id1 = allocator->AllocateID();
  allocator->FreeID(0);
  allocator->FreeID(id1);
  allocator->FreeID(id1);
  allocator->FreeID(id1 + 1);

  ResourceId id2 = allocator->AllocateID();
  ResourceId id3 = allocator->AllocateID();
  EXPECT_NE(id2, id3);
  EXPECT_NE(kInvalidResource, id2);
  EXPECT_NE(kInvalidResource, id3);
}

}  // namespace gpu
