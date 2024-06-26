// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <set>

#include "cc/base/scoped_ptr_vector.h"
#include "testing/gmock/include/gmock/gmock.h"

namespace cc {
namespace {

    class Data {
    public:
        static scoped_ptr<Data> Create(int i) { return make_scoped_ptr(new Data(i)); }
        int data() const { return data_; }

    private:
        explicit Data(int i)
            : data_(i)
        {
        }
        int data_;
    };

    class IsOddPredicate {
    public:
        bool operator()(const Data* data) { return (data->data() % 2) == 1; }
    };

    TEST(ScopedPtrVectorTest, PushBack)
    {
        ScopedPtrVector<Data> v;

        // Insert 5 things into the vector.
        v.push_back(Data::Create(1));
        v.push_back(Data::Create(2));
        v.push_back(Data::Create(3));
        v.push_back(Data::Create(4));
        v.push_back(Data::Create(5));

        EXPECT_EQ(5u, v.size());
        EXPECT_EQ(1, v[0]->data());
        EXPECT_EQ(2, v[1]->data());
        EXPECT_EQ(3, v[2]->data());
        EXPECT_EQ(4, v[3]->data());
        EXPECT_EQ(5, v[4]->data());
    }

    TEST(ScopedPtrVectorTest, InsertAndTake)
    {
        // Insert 3 things into each vector.
        ScopedPtrVector<Data> v;
        v.push_back(Data::Create(1));
        v.push_back(Data::Create(2));
        v.push_back(Data::Create(6));

        ScopedPtrVector<Data> v2;
        v2.push_back(Data::Create(3));
        v2.push_back(Data::Create(4));
        v2.push_back(Data::Create(5));

        ScopedPtrVector<Data>::iterator it = v.begin();
        ++it;
        ++it;
        EXPECT_EQ(6, (*it)->data());

        v.insert_and_take(it, &v2);

        EXPECT_EQ(6u, v.size());
        EXPECT_EQ(1, v[0]->data());
        EXPECT_EQ(2, v[1]->data());
        EXPECT_EQ(3, v[2]->data());
        EXPECT_EQ(4, v[3]->data());
        EXPECT_EQ(5, v[4]->data());
        EXPECT_EQ(6, v[5]->data());

        EXPECT_EQ(3u, v2.size());
        EXPECT_EQ(nullptr, v2[0]);
        EXPECT_EQ(nullptr, v2[1]);
        EXPECT_EQ(nullptr, v2[2]);
    }

    TEST(ScopedPtrVectorTest, Partition)
    {
        ScopedPtrVector<Data> v;
        v.push_back(Data::Create(1));
        v.push_back(Data::Create(2));
        v.push_back(Data::Create(3));
        v.push_back(Data::Create(4));
        v.push_back(Data::Create(5));

        ScopedPtrVector<Data>::iterator it = v.partition(IsOddPredicate());
        std::set<int> odd_numbers;
        for (ScopedPtrVector<Data>::iterator second_it = v.begin();
             second_it != it;
             ++second_it) {
            EXPECT_EQ(1, (*second_it)->data() % 2);
            odd_numbers.insert((*second_it)->data());
        }
        EXPECT_EQ(3u, odd_numbers.size());

        std::set<int> even_numbers;
        for (; it != v.end(); ++it) {
            EXPECT_EQ(0, (*it)->data() % 2);
            even_numbers.insert((*it)->data());
        }
        EXPECT_EQ(2u, even_numbers.size());
    }

    class DataWithDestruction {
    public:
        static scoped_ptr<DataWithDestruction> Create(int i, int* destroy_count)
        {
            return make_scoped_ptr(new DataWithDestruction(i, destroy_count));
        }
        int data() const { return data_; }
        ~DataWithDestruction() { ++(*destroy_count_); }

    private:
        explicit DataWithDestruction(int i, int* destroy_count)
            : data_(i)
            , destroy_count_(destroy_count)
        {
        }
        int data_;
        int* destroy_count_;
    };

    TEST(ScopedPtrVectorTest, RemoveIf)
    {
        ScopedPtrVector<DataWithDestruction> v;
        int destroyed[6] = { 0 };
        v.push_back(DataWithDestruction::Create(1, &destroyed[0]));
        v.push_back(DataWithDestruction::Create(2, &destroyed[1]));
        v.push_back(DataWithDestruction::Create(3, &destroyed[2]));
        v.push_back(DataWithDestruction::Create(3, &destroyed[3]));
        v.push_back(DataWithDestruction::Create(4, &destroyed[4]));
        v.push_back(DataWithDestruction::Create(5, &destroyed[5]));

        int expect_destroyed[6] = { 0 };

        // Removing more than one thing that matches.
        auto is_three = [](DataWithDestruction* d) { return d->data() == 3; };
        v.erase(v.remove_if(is_three), v.end());
        EXPECT_EQ(4u, v.size());
        expect_destroyed[2]++;
        expect_destroyed[3]++;
        for (size_t i = 0; i < arraysize(destroyed); ++i)
            EXPECT_EQ(expect_destroyed[i], destroyed[i]) << i;
        {
            int expect_data[4] = { 1, 2, 4, 5 };
            for (size_t i = 0; i < arraysize(expect_data); ++i)
                EXPECT_EQ(expect_data[i], v[i]->data()) << i;
        }

        // Removing from the back of the vector.
        auto is_five = [](DataWithDestruction* d) { return d->data() == 5; };
        v.erase(v.remove_if(is_five), v.end());
        EXPECT_EQ(3u, v.size());
        expect_destroyed[5]++;
        for (size_t i = 0; i < arraysize(destroyed); ++i)
            EXPECT_EQ(expect_destroyed[i], destroyed[i]) << i;
        {
            int expect_data[3] = { 1, 2, 4 };
            for (size_t i = 0; i < arraysize(expect_data); ++i)
                EXPECT_EQ(expect_data[i], v[i]->data()) << i;
        }

        // Removing from the front of the vector.
        auto is_one = [](DataWithDestruction* d) { return d->data() == 1; };
        v.erase(v.remove_if(is_one), v.end());
        EXPECT_EQ(2u, v.size());
        expect_destroyed[0]++;
        for (size_t i = 0; i < arraysize(destroyed); ++i)
            EXPECT_EQ(expect_destroyed[i], destroyed[i]) << i;
        {
            int expect_data[2] = { 2, 4 };
            for (size_t i = 0; i < arraysize(expect_data); ++i)
                EXPECT_EQ(expect_data[i], v[i]->data()) << i;
        }

        // Removing things that aren't in the vector does nothing.
        v.erase(v.remove_if(is_one), v.end());
        EXPECT_EQ(2u, v.size());
        for (size_t i = 0; i < arraysize(destroyed); ++i)
            EXPECT_EQ(expect_destroyed[i], destroyed[i]) << i;
        {
            int expect_data[2] = { 2, 4 };
            for (size_t i = 0; i < arraysize(expect_data); ++i)
                EXPECT_EQ(expect_data[i], v[i]->data()) << i;
        }
    }

} // namespace
} // namespace cc
