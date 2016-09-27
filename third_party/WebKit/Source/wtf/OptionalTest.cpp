// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "wtf/Optional.h"

#include <gtest/gtest.h>

namespace WTF {
namespace {

struct IntBox {
    IntBox(int n) : number(n) { }
    int number;
};

class DestructionNotifier {
public:
    DestructionNotifier(bool& flag) : m_flag(flag) { }
    ~DestructionNotifier() { m_flag = true; }
private:
    bool& m_flag;
};

TEST(OptionalTest, BooleanTest)
{
    Optional<int> optional;
    EXPECT_FALSE(optional);
    optional.emplace(0);
    EXPECT_TRUE(optional);
}

TEST(OptionalTest, Dereference)
{
    Optional<int> optional;
    optional.emplace(1);
    EXPECT_EQ(1, *optional);

    Optional<IntBox> optionalIntbox;
    optionalIntbox.emplace(42);
    EXPECT_EQ(42, optionalIntbox->number);
}

TEST(OptionalTest, DestructorCalled)
{
    // Destroying a disengaged optional shouldn't do anything.
    {
        Optional<DestructionNotifier> optional;
    }

    // Destroying an engaged optional should call the destructor.
    bool isDestroyed = false;
    {
        Optional<DestructionNotifier> optional;
        optional.emplace(isDestroyed);
        EXPECT_FALSE(isDestroyed);
    }
    EXPECT_TRUE(isDestroyed);
}

} // namespace
} // namespace WTF
