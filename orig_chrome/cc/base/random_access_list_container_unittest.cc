// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/base/random_access_list_container.h"

#include <algorithm>
#include <vector>

#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace cc {
namespace {

    class Base {
    public:
        virtual ~Base() { }
        int get_value() const { return value_; }

    protected:
        explicit Base(int value)
            : value_(value)
        {
        }

        int value_;
    };

    const int kMagicNumberOne = 1;
    const int kMagicNumberTwo = 2;
    const int kMagicNumberThree = 3;

    class Derived1 : public Base {
    public:
        Derived1()
            : Base(kMagicNumberOne)
        {
        }
    };

    class Derived2 : public Base {
    public:
        Derived2()
            : Base(kMagicNumberTwo)
        {
        }
    };

    class Derived3 : public Base {
    public:
        Derived3()
            : Base(kMagicNumberThree)
        {
        }
    };

    size_t LargestDerivedElementSize()
    {
        static_assert(sizeof(Derived1) >= sizeof(Derived2),
            "Derived2 is larger than Derived1");
        static_assert(sizeof(Derived1) >= sizeof(Derived3),
            "Derived3 is larger than Derived1");
        return sizeof(Derived1);
    }

    TEST(RandomAccessListContainerTest, RandomAccess)
    {
        RandomAccessListContainer<Base> list(LargestDerivedElementSize(), 1);

        list.AllocateAndConstruct<Derived1>();
        list.AllocateAndConstruct<Derived2>();
        list.AllocateAndConstruct<Derived3>();
        list.AllocateAndConstruct<Derived1>();
        list.AllocateAndConstruct<Derived2>();
        list.AllocateAndConstruct<Derived3>();

        EXPECT_EQ(kMagicNumberOne, list[0]->get_value());
        EXPECT_EQ(kMagicNumberTwo, list[1]->get_value());
        EXPECT_EQ(kMagicNumberThree, list[2]->get_value());
        EXPECT_EQ(kMagicNumberOne, list[3]->get_value());
        EXPECT_EQ(kMagicNumberTwo, list[4]->get_value());
        EXPECT_EQ(kMagicNumberThree, list[5]->get_value());

        list.RemoveLast();
        list.RemoveLast();
        list.RemoveLast();

        EXPECT_EQ(kMagicNumberOne, list[0]->get_value());
        EXPECT_EQ(kMagicNumberTwo, list[1]->get_value());
        EXPECT_EQ(kMagicNumberThree, list[2]->get_value());

        list.AllocateAndConstruct<Derived3>();
        list.AllocateAndConstruct<Derived2>();
        list.AllocateAndConstruct<Derived1>();

        EXPECT_EQ(kMagicNumberOne, list[0]->get_value());
        EXPECT_EQ(kMagicNumberTwo, list[1]->get_value());
        EXPECT_EQ(kMagicNumberThree, list[2]->get_value());
        EXPECT_EQ(kMagicNumberThree, list[3]->get_value());
        EXPECT_EQ(kMagicNumberTwo, list[4]->get_value());
        EXPECT_EQ(kMagicNumberOne, list[5]->get_value());
    }

    TEST(RandomAccessListContainerTest, Clear)
    {
        RandomAccessListContainer<Base> list(LargestDerivedElementSize(), 1);

        list.AllocateAndConstruct<Derived1>();
        list.AllocateAndConstruct<Derived2>();

        EXPECT_EQ(kMagicNumberOne, list[0]->get_value());
        EXPECT_EQ(kMagicNumberTwo, list[1]->get_value());

        list.clear();
        list.AllocateAndConstruct<Derived3>();

        EXPECT_EQ(kMagicNumberThree, list[0]->get_value());
    }

} // namespace
} // namespace cc
