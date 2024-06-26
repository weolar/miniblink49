// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "base/containers/scoped_ptr_hash_map.h"

#include "base/memory/scoped_ptr.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace base {
namespace {

    namespace namespace_with_ignore_result {

        class Value {
        };

        template <typename T>
        void ignore_result(const T&) { }

    } // namespace namespace_with_ignore_result

    struct DeleteCounter {
    public:
        DeleteCounter() { }
        ~DeleteCounter() { g_delete_count++; }

        static void ResetCounter() { g_delete_count = 0; }
        static int delete_count() { return g_delete_count; }

    private:
        static int g_delete_count;
    };

    int DeleteCounter::g_delete_count = 0;

    struct CountingDeleter {
    public:
        inline void operator()(DeleteCounter* ptr) const
        {
            g_deleter_call_count++;
            delete ptr;
        }

        static int count() { return g_deleter_call_count; }
        static void ResetCounter() { g_deleter_call_count = 0; }

    private:
        static int g_deleter_call_count;
    };

    int CountingDeleter::g_deleter_call_count = 0;

    TEST(ScopedPtrHashMapTest, CustomDeleter)
    {
        int key = 123;

        // Test dtor.
        DeleteCounter::ResetCounter();
        CountingDeleter::ResetCounter();
        {
            ScopedPtrHashMap<int, scoped_ptr<DeleteCounter, CountingDeleter>> map;
            map.set(key, scoped_ptr<DeleteCounter, CountingDeleter>(new DeleteCounter));
        }
        EXPECT_EQ(1, DeleteCounter::delete_count());
        EXPECT_EQ(1, CountingDeleter::count());

        // Test set and erase.
        DeleteCounter::ResetCounter();
        CountingDeleter::ResetCounter();
        {
            ScopedPtrHashMap<int, scoped_ptr<DeleteCounter, CountingDeleter>> map;
            map.erase(map.set(
                key, scoped_ptr<DeleteCounter, CountingDeleter>(new DeleteCounter)));
            EXPECT_EQ(1, DeleteCounter::delete_count());
            EXPECT_EQ(1, CountingDeleter::count());
        }
        EXPECT_EQ(1, DeleteCounter::delete_count());
        EXPECT_EQ(1, CountingDeleter::count());

        // Test set more than once.
        DeleteCounter::ResetCounter();
        CountingDeleter::ResetCounter();
        {
            ScopedPtrHashMap<int, scoped_ptr<DeleteCounter, CountingDeleter>> map;
            map.set(key, scoped_ptr<DeleteCounter, CountingDeleter>(new DeleteCounter));
            map.set(key, scoped_ptr<DeleteCounter, CountingDeleter>(new DeleteCounter));
            map.set(key, scoped_ptr<DeleteCounter, CountingDeleter>(new DeleteCounter));
            EXPECT_EQ(2, DeleteCounter::delete_count());
            EXPECT_EQ(2, CountingDeleter::count());
        }
        EXPECT_EQ(3, DeleteCounter::delete_count());
        EXPECT_EQ(3, CountingDeleter::count());
    }

    // Test that using a value type from a namespace containing an ignore_result
    // function compiles correctly.
    TEST(ScopedPtrHashMapTest, IgnoreResultCompile)
    {
        ScopedPtrHashMap<int, scoped_ptr<namespace_with_ignore_result::Value>>
            scoped_map;
        scoped_map.add(1, make_scoped_ptr(new namespace_with_ignore_result::Value));
    }

} // namespace
} // namespace base
