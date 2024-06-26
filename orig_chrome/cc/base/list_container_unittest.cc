// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/base/list_container.h"

#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include <algorithm>
#include <vector>

namespace cc {
namespace {

    // Element class having derived classes.
    class DerivedElement {
    public:
        virtual ~DerivedElement() { }

    protected:
        bool bool_values[1];
        char char_values[1];
        int int_values[1];
        long long_values[1];
    };

    class DerivedElement1 : public DerivedElement {
    protected:
        bool bool_values1[1];
        char char_values1[1];
        int int_values1[1];
        long long_values1[1];
    };

    class DerivedElement2 : public DerivedElement {
    protected:
        bool bool_values2[2];
        char char_values2[2];
        int int_values2[2];
        long long_values2[2];
    };

    class DerivedElement3 : public DerivedElement {
    protected:
        bool bool_values3[3];
        char char_values3[3];
        int int_values3[3];
        long long_values3[3];
    };

    const size_t kLargestDerivedElementSize = sizeof(DerivedElement3);

    size_t LargestDerivedElementSize()
    {
        static_assert(sizeof(DerivedElement1) <= kLargestDerivedElementSize,
            "Largest Derived Element size needs update. DerivedElement1 is "
            "currently largest.");
        static_assert(sizeof(DerivedElement2) <= kLargestDerivedElementSize,
            "Largest Derived Element size needs update. DerivedElement2 is "
            "currently largest.");

        return kLargestDerivedElementSize;
    }

    // Element class having no derived classes.
    class NonDerivedElement {
    public:
        NonDerivedElement() { }
        ~NonDerivedElement() { }

        int int_values[1];
    };

    bool isConstNonDerivedElementPointer(const NonDerivedElement* ptr)
    {
        return true;
    }

    bool isConstNonDerivedElementPointer(NonDerivedElement* ptr)
    {
        return false;
    }

    const int kMagicNumberToUseForSimpleDerivedElementOne = 42;
    const int kMagicNumberToUseForSimpleDerivedElementTwo = 314;
    const int kMagicNumberToUseForSimpleDerivedElementThree = 1618;

    class SimpleDerivedElement : public DerivedElement {
    public:
        ~SimpleDerivedElement() override { }
        void set_value(int val) { value = val; }
        int get_value() { return value; }

    private:
        int value;
    };

    class SimpleDerivedElementConstructMagicNumberOne
        : public SimpleDerivedElement {
    public:
        SimpleDerivedElementConstructMagicNumberOne()
        {
            set_value(kMagicNumberToUseForSimpleDerivedElementOne);
        }
    };

    class SimpleDerivedElementConstructMagicNumberTwo
        : public SimpleDerivedElement {
    public:
        SimpleDerivedElementConstructMagicNumberTwo()
        {
            set_value(kMagicNumberToUseForSimpleDerivedElementTwo);
        }
    };

    class SimpleDerivedElementConstructMagicNumberThree
        : public SimpleDerivedElement {
    public:
        SimpleDerivedElementConstructMagicNumberThree()
        {
            set_value(kMagicNumberToUseForSimpleDerivedElementThree);
        }
    };

    class MockDerivedElement : public SimpleDerivedElementConstructMagicNumberOne {
    public:
        ~MockDerivedElement() override { Destruct(); }
        MOCK_METHOD0(Destruct, void());
    };

    class MockDerivedElementSubclass : public MockDerivedElement {
    public:
        MockDerivedElementSubclass()
        {
            set_value(kMagicNumberToUseForSimpleDerivedElementTwo);
        }
    };

    const size_t kCurrentLargestDerivedElementSize = std::max(LargestDerivedElementSize(), sizeof(MockDerivedElementSubclass));

    TEST(ListContainerTest, ConstructorCalledInAllocateAndConstruct)
    {
        ListContainer<DerivedElement> list(kCurrentLargestDerivedElementSize);

        size_t size = 2;
        SimpleDerivedElementConstructMagicNumberOne* de_1 = list.AllocateAndConstruct<SimpleDerivedElementConstructMagicNumberOne>();
        SimpleDerivedElementConstructMagicNumberTwo* de_2 = list.AllocateAndConstruct<SimpleDerivedElementConstructMagicNumberTwo>();

        EXPECT_EQ(size, list.size());
        EXPECT_EQ(de_1, list.front());
        EXPECT_EQ(de_2, list.back());

        EXPECT_EQ(kMagicNumberToUseForSimpleDerivedElementOne, de_1->get_value());
        EXPECT_EQ(kMagicNumberToUseForSimpleDerivedElementTwo, de_2->get_value());
    }

    TEST(ListContainerTest, DestructorCalled)
    {
        ListContainer<DerivedElement> list(kCurrentLargestDerivedElementSize);

        size_t size = 1;
        MockDerivedElement* de_1 = list.AllocateAndConstruct<MockDerivedElement>();

        EXPECT_CALL(*de_1, Destruct());
        EXPECT_EQ(size, list.size());
        EXPECT_EQ(de_1, list.front());
    }

    TEST(ListContainerTest, DestructorCalledOnceWhenClear)
    {
        ListContainer<DerivedElement> list(kCurrentLargestDerivedElementSize);
        size_t size = 1;
        MockDerivedElement* de_1 = list.AllocateAndConstruct<MockDerivedElement>();

        EXPECT_EQ(size, list.size());
        EXPECT_EQ(de_1, list.front());

        // Make sure destructor is called once during clear, and won't be called
        // again.
        testing::MockFunction<void()> separator;
        {
            testing::InSequence s;
            EXPECT_CALL(*de_1, Destruct());
            EXPECT_CALL(separator, Call());
            EXPECT_CALL(*de_1, Destruct()).Times(0);
        }

        list.clear();
        separator.Call();
    }

    TEST(ListContainerTest, ClearDoesNotMalloc)
    {
        const size_t reserve = 10;
        ListContainer<DerivedElement> list(kCurrentLargestDerivedElementSize,
            reserve);

        // Memory from the initial inner list that should be re-used after clear().
        std::vector<DerivedElement*> reserved_element_pointers;
        for (size_t i = 0; i < reserve; i++) {
            DerivedElement* element = list.AllocateAndConstruct<DerivedElement>();
            reserved_element_pointers.push_back(element);
        }
        EXPECT_EQ(0u, list.AvailableSizeWithoutAnotherAllocationForTesting());

        // Allocate more than the reserve count, forcing new capacity to be added.
        list.AllocateAndConstruct<DerivedElement>();
        EXPECT_NE(0u, list.AvailableSizeWithoutAnotherAllocationForTesting());

        // Clear should free all memory except the first |reserve| elements.
        list.clear();
        EXPECT_EQ(reserve, list.AvailableSizeWithoutAnotherAllocationForTesting());

        // Verify the first |reserve| elements are re-used after clear().
        for (size_t i = 0; i < reserve; i++) {
            DerivedElement* element = list.AllocateAndConstruct<DerivedElement>();
            EXPECT_EQ(element, reserved_element_pointers[i]);
        }
        EXPECT_EQ(0u, list.AvailableSizeWithoutAnotherAllocationForTesting());

        // Verify that capacity can still grow properly.
        list.AllocateAndConstruct<DerivedElement>();
        EXPECT_NE(0u, list.AvailableSizeWithoutAnotherAllocationForTesting());
    }

    TEST(ListContainerTest, ReplaceExistingElement)
    {
        ListContainer<DerivedElement> list(kCurrentLargestDerivedElementSize);
        size_t size = 1;
        MockDerivedElement* de_1 = list.AllocateAndConstruct<MockDerivedElement>();

        EXPECT_EQ(size, list.size());
        EXPECT_EQ(de_1, list.front());

        // Make sure destructor is called once during clear, and won't be called
        // again.
        testing::MockFunction<void()> separator;
        {
            testing::InSequence s;
            EXPECT_CALL(*de_1, Destruct());
            EXPECT_CALL(separator, Call());
            EXPECT_CALL(*de_1, Destruct()).Times(0);
        }

        list.ReplaceExistingElement<MockDerivedElementSubclass>(list.begin());
        EXPECT_EQ(kMagicNumberToUseForSimpleDerivedElementTwo, de_1->get_value());
        separator.Call();

        EXPECT_CALL(*de_1, Destruct());
        list.clear();
    }

    TEST(ListContainerTest, DestructorCalledOnceWhenErase)
    {
        ListContainer<DerivedElement> list(kCurrentLargestDerivedElementSize);
        size_t size = 1;
        MockDerivedElement* de_1 = list.AllocateAndConstruct<MockDerivedElement>();

        EXPECT_EQ(size, list.size());
        EXPECT_EQ(de_1, list.front());

        // Make sure destructor is called once during clear, and won't be called
        // again.
        testing::MockFunction<void()> separator;
        {
            testing::InSequence s;
            EXPECT_CALL(*de_1, Destruct());
            EXPECT_CALL(separator, Call());
            EXPECT_CALL(*de_1, Destruct()).Times(0);
        }

        list.EraseAndInvalidateAllPointers(list.begin());
        separator.Call();
    }

    TEST(ListContainerTest, SimpleIndexAccessNonDerivedElement)
    {
        ListContainer<NonDerivedElement> list;

        size_t size = 3;
        NonDerivedElement* nde_1 = list.AllocateAndConstruct<NonDerivedElement>();
        NonDerivedElement* nde_2 = list.AllocateAndConstruct<NonDerivedElement>();
        NonDerivedElement* nde_3 = list.AllocateAndConstruct<NonDerivedElement>();

        EXPECT_EQ(size, list.size());
        EXPECT_EQ(nde_1, list.front());
        EXPECT_EQ(nde_3, list.back());
        EXPECT_EQ(list.front(), list.ElementAt(0));
        EXPECT_EQ(nde_2, list.ElementAt(1));
        EXPECT_EQ(list.back(), list.ElementAt(2));
    }

    TEST(ListContainerTest, SimpleInsertionNonDerivedElement)
    {
        ListContainer<NonDerivedElement> list;

        size_t size = 3;
        NonDerivedElement* nde_1 = list.AllocateAndConstruct<NonDerivedElement>();
        list.AllocateAndConstruct<NonDerivedElement>();
        NonDerivedElement* nde_3 = list.AllocateAndConstruct<NonDerivedElement>();

        EXPECT_EQ(size, list.size());
        EXPECT_EQ(nde_1, list.front());
        EXPECT_EQ(nde_3, list.back());
    }

    TEST(ListContainerTest, SimpleInsertionAndClearNonDerivedElement)
    {
        ListContainer<NonDerivedElement> list;
        EXPECT_TRUE(list.empty());
        EXPECT_EQ(0u, list.size());

        size_t size = 3;
        NonDerivedElement* nde_1 = list.AllocateAndConstruct<NonDerivedElement>();
        list.AllocateAndConstruct<NonDerivedElement>();
        NonDerivedElement* nde_3 = list.AllocateAndConstruct<NonDerivedElement>();

        EXPECT_EQ(size, list.size());
        EXPECT_EQ(nde_1, list.front());
        EXPECT_EQ(nde_3, list.back());
        EXPECT_FALSE(list.empty());

        list.clear();
        EXPECT_TRUE(list.empty());
        EXPECT_EQ(0u, list.size());
    }

    TEST(ListContainerTest, SimpleInsertionClearAndInsertAgainNonDerivedElement)
    {
        ListContainer<NonDerivedElement> list;
        EXPECT_TRUE(list.empty());
        EXPECT_EQ(0u, list.size());

        size_t size = 2;
        NonDerivedElement* nde_front = list.AllocateAndConstruct<NonDerivedElement>();
        NonDerivedElement* nde_back = list.AllocateAndConstruct<NonDerivedElement>();

        EXPECT_EQ(size, list.size());
        EXPECT_EQ(nde_front, list.front());
        EXPECT_EQ(nde_back, list.back());
        EXPECT_FALSE(list.empty());

        list.clear();
        EXPECT_TRUE(list.empty());
        EXPECT_EQ(0u, list.size());

        size = 3;
        nde_front = list.AllocateAndConstruct<NonDerivedElement>();
        list.AllocateAndConstruct<NonDerivedElement>();
        nde_back = list.AllocateAndConstruct<NonDerivedElement>();

        EXPECT_EQ(size, list.size());
        EXPECT_EQ(nde_front, list.front());
        EXPECT_EQ(nde_back, list.back());
        EXPECT_FALSE(list.empty());
    }

    // This test is used to test when there is more than one allocation needed
    // for, ListContainer can still perform like normal vector.
    TEST(ListContainerTest,
        SimpleInsertionTriggerMoreThanOneAllocationNonDerivedElement)
    {
        ListContainer<NonDerivedElement> list(sizeof(NonDerivedElement), 2);
        std::vector<NonDerivedElement*> nde_list;
        size_t size = 10;
        for (size_t i = 0; i < size; ++i) {
            nde_list.push_back(list.AllocateAndConstruct<NonDerivedElement>());
        }
        EXPECT_EQ(size, list.size());

        ListContainer<NonDerivedElement>::Iterator iter = list.begin();
        for (std::vector<NonDerivedElement*>::const_iterator nde_iter = nde_list.begin();
             nde_iter != nde_list.end(); ++nde_iter) {
            EXPECT_EQ(*nde_iter, *iter);
            ++iter;
        }
    }

    TEST(ListContainerTest,
        CorrectAllocationSizeForMoreThanOneAllocationNonDerivedElement)
    {
        // Constructor sets the allocation size to 2. Every time ListContainer needs
        // to allocate again, it doubles allocation size. In this test, 10 elements is
        // needed, thus ListContainerShould allocate spaces 2, 4 and 8 elements.
        ListContainer<NonDerivedElement> list(sizeof(NonDerivedElement), 2);
        std::vector<NonDerivedElement*> nde_list;
        size_t size = 10;
        for (size_t i = 0; i < size; ++i) {
            // Before asking for a new element, space available without another
            // allocation follows.
            switch (i) {
            case 2:
            case 6:
                EXPECT_EQ(0u, list.AvailableSizeWithoutAnotherAllocationForTesting());
                break;
            case 1:
            case 5:
                EXPECT_EQ(1u, list.AvailableSizeWithoutAnotherAllocationForTesting());
                break;
            case 0:
            case 4:
                EXPECT_EQ(2u, list.AvailableSizeWithoutAnotherAllocationForTesting());
                break;
            case 3:
                EXPECT_EQ(3u, list.AvailableSizeWithoutAnotherAllocationForTesting());
                break;
            case 9:
                EXPECT_EQ(5u, list.AvailableSizeWithoutAnotherAllocationForTesting());
                break;
            case 8:
                EXPECT_EQ(6u, list.AvailableSizeWithoutAnotherAllocationForTesting());
                break;
            case 7:
                EXPECT_EQ(7u, list.AvailableSizeWithoutAnotherAllocationForTesting());
                break;
            default:
                break;
            }
            nde_list.push_back(list.AllocateAndConstruct<NonDerivedElement>());
            // After asking for a new element, space available without another
            // allocation follows.
            switch (i) {
            case 1:
            case 5:
                EXPECT_EQ(0u, list.AvailableSizeWithoutAnotherAllocationForTesting());
                break;
            case 0:
            case 4:
                EXPECT_EQ(1u, list.AvailableSizeWithoutAnotherAllocationForTesting());
                break;
            case 3:
                EXPECT_EQ(2u, list.AvailableSizeWithoutAnotherAllocationForTesting());
                break;
            case 2:
                EXPECT_EQ(3u, list.AvailableSizeWithoutAnotherAllocationForTesting());
                break;
            case 9:
                EXPECT_EQ(4u, list.AvailableSizeWithoutAnotherAllocationForTesting());
                break;
            case 8:
                EXPECT_EQ(5u, list.AvailableSizeWithoutAnotherAllocationForTesting());
                break;
            case 7:
                EXPECT_EQ(6u, list.AvailableSizeWithoutAnotherAllocationForTesting());
                break;
            case 6:
                EXPECT_EQ(7u, list.AvailableSizeWithoutAnotherAllocationForTesting());
                break;
            default:
                break;
            }
        }
        EXPECT_EQ(size, list.size());

        ListContainer<NonDerivedElement>::Iterator iter = list.begin();
        for (std::vector<NonDerivedElement*>::const_iterator nde_iter = nde_list.begin();
             nde_iter != nde_list.end(); ++nde_iter) {
            EXPECT_EQ(*nde_iter, *iter);
            ++iter;
        }
    }

    TEST(ListContainerTest, SimpleIterationNonDerivedElement)
    {
        ListContainer<NonDerivedElement> list;
        std::vector<NonDerivedElement*> nde_list;
        size_t size = 10;
        for (size_t i = 0; i < size; ++i) {
            nde_list.push_back(list.AllocateAndConstruct<NonDerivedElement>());
        }
        EXPECT_EQ(size, list.size());

        size_t num_iters_in_list = 0;
        {
            std::vector<NonDerivedElement*>::const_iterator nde_iter = nde_list.begin();
            for (ListContainer<NonDerivedElement>::Iterator iter = list.begin();
                 iter != list.end(); ++iter) {
                EXPECT_EQ(*nde_iter, *iter);
                ++num_iters_in_list;
                ++nde_iter;
            }
        }

        size_t num_iters_in_vector = 0;
        {
            ListContainer<NonDerivedElement>::Iterator iter = list.begin();
            for (std::vector<NonDerivedElement*>::const_iterator nde_iter = nde_list.begin();
                 nde_iter != nde_list.end(); ++nde_iter) {
                EXPECT_EQ(*nde_iter, *iter);
                ++num_iters_in_vector;
                ++iter;
            }
        }

        EXPECT_EQ(num_iters_in_vector, num_iters_in_list);
    }

    TEST(ListContainerTest, SimpleConstIteratorIterationNonDerivedElement)
    {
        ListContainer<NonDerivedElement> list;
        std::vector<const NonDerivedElement*> nde_list;
        size_t size = 10;
        for (size_t i = 0; i < size; ++i) {
            nde_list.push_back(list.AllocateAndConstruct<NonDerivedElement>());
        }
        EXPECT_EQ(size, list.size());

        {
            std::vector<const NonDerivedElement*>::const_iterator nde_iter = nde_list.begin();
            for (ListContainer<NonDerivedElement>::ConstIterator iter = list.begin();
                 iter != list.end(); ++iter) {
                EXPECT_TRUE(isConstNonDerivedElementPointer(*iter));
                EXPECT_EQ(*nde_iter, *iter);
                ++nde_iter;
            }
        }

        {
            std::vector<const NonDerivedElement*>::const_iterator nde_iter = nde_list.begin();
            for (ListContainer<NonDerivedElement>::Iterator iter = list.begin();
                 iter != list.end(); ++iter) {
                EXPECT_FALSE(isConstNonDerivedElementPointer(*iter));
                EXPECT_EQ(*nde_iter, *iter);
                ++nde_iter;
            }
        }

        {
            ListContainer<NonDerivedElement>::ConstIterator iter = list.begin();
            for (std::vector<const NonDerivedElement*>::const_iterator nde_iter = nde_list.begin();
                 nde_iter != nde_list.end(); ++nde_iter) {
                EXPECT_EQ(*nde_iter, *iter);
                ++iter;
            }
        }
    }

    TEST(ListContainerTest, SimpleReverseInsertionNonDerivedElement)
    {
        ListContainer<NonDerivedElement> list;
        std::vector<NonDerivedElement*> nde_list;
        size_t size = 10;
        for (size_t i = 0; i < size; ++i) {
            nde_list.push_back(list.AllocateAndConstruct<NonDerivedElement>());
        }
        EXPECT_EQ(size, list.size());

        {
            std::vector<NonDerivedElement*>::const_reverse_iterator nde_iter = nde_list.rbegin();
            for (ListContainer<NonDerivedElement>::ReverseIterator iter = list.rbegin();
                 iter != list.rend(); ++iter) {
                EXPECT_EQ(*nde_iter, *iter);
                ++nde_iter;
            }
        }

        {
            ListContainer<NonDerivedElement>::ReverseIterator iter = list.rbegin();
            for (std::vector<NonDerivedElement*>::reverse_iterator nde_iter = nde_list.rbegin();
                 nde_iter != nde_list.rend(); ++nde_iter) {
                EXPECT_EQ(*nde_iter, *iter);
                ++iter;
            }
        }
    }

    TEST(ListContainerTest, SimpleDeletion)
    {
        ListContainer<DerivedElement> list(kCurrentLargestDerivedElementSize);
        std::vector<SimpleDerivedElement*> sde_list;
        int size = 10;
        for (int i = 0; i < size; ++i) {
            sde_list.push_back(list.AllocateAndConstruct<SimpleDerivedElement>());
            sde_list.back()->set_value(i);
        }
        EXPECT_EQ(static_cast<size_t>(size), list.size());

        list.EraseAndInvalidateAllPointers(list.begin());
        --size;
        EXPECT_EQ(static_cast<size_t>(size), list.size());
        int i = 1;
        for (ListContainer<DerivedElement>::Iterator iter = list.begin();
             iter != list.end(); ++iter) {
            EXPECT_EQ(i, static_cast<SimpleDerivedElement*>(*iter)->get_value());
            ++i;
        }
    }

    TEST(ListContainerTest, DeletionAllInAllocation)
    {
        const size_t kReserve = 10;
        ListContainer<DerivedElement> list(kCurrentLargestDerivedElementSize,
            kReserve);
        std::vector<SimpleDerivedElement*> sde_list;
        // Add enough elements to cause another allocation.
        for (size_t i = 0; i < kReserve + 1; ++i) {
            sde_list.push_back(list.AllocateAndConstruct<SimpleDerivedElement>());
            sde_list.back()->set_value(static_cast<int>(i));
        }
        EXPECT_EQ(kReserve + 1, list.size());

        // Remove everything in the first allocation.
        for (size_t i = 0; i < kReserve; ++i)
            list.EraseAndInvalidateAllPointers(list.begin());
        EXPECT_EQ(1u, list.size());

        // The last element is left.
        SimpleDerivedElement* de = static_cast<SimpleDerivedElement*>(*list.begin());
        EXPECT_EQ(static_cast<int>(kReserve), de->get_value());

        // Remove the element from the 2nd allocation.
        list.EraseAndInvalidateAllPointers(list.begin());
        EXPECT_EQ(0u, list.size());
    }

    TEST(ListContainerTest, DeletionAllInAllocationReversed)
    {
        const size_t kReserve = 10;
        ListContainer<DerivedElement> list(kCurrentLargestDerivedElementSize,
            kReserve);
        std::vector<SimpleDerivedElement*> sde_list;
        // Add enough elements to cause another allocation.
        for (size_t i = 0; i < kReserve + 1; ++i) {
            sde_list.push_back(list.AllocateAndConstruct<SimpleDerivedElement>());
            sde_list.back()->set_value(static_cast<int>(i));
        }
        EXPECT_EQ(kReserve + 1, list.size());

        // Remove everything in the 2nd allocation.
        auto it = list.begin();
        for (size_t i = 0; i < kReserve; ++i)
            ++it;
        list.EraseAndInvalidateAllPointers(it);

        // The 2nd-last element is next, and the rest of the elements exist.
        size_t i = kReserve - 1;
        for (auto it = list.rbegin(); it != list.rend(); ++it) {
            SimpleDerivedElement* de = static_cast<SimpleDerivedElement*>(*it);
            EXPECT_EQ(static_cast<int>(i), de->get_value());
            --i;
        }

        // Can forward iterate too.
        i = 0;
        for (auto it = list.begin(); it != list.end(); ++it) {
            SimpleDerivedElement* de = static_cast<SimpleDerivedElement*>(*it);
            EXPECT_EQ(static_cast<int>(i), de->get_value());
            ++i;
        }

        // Remove the last thing from the 1st allocation.
        it = list.begin();
        for (size_t i = 0; i < kReserve - 1; ++i)
            ++it;
        list.EraseAndInvalidateAllPointers(it);

        // The 2nd-last element is next, and the rest of the elements exist.
        i = kReserve - 2;
        for (auto it = list.rbegin(); it != list.rend(); ++it) {
            SimpleDerivedElement* de = static_cast<SimpleDerivedElement*>(*it);
            EXPECT_EQ(static_cast<int>(i), de->get_value());
            --i;
        }

        // Can forward iterate too.
        i = 0;
        for (auto it = list.begin(); it != list.end(); ++it) {
            SimpleDerivedElement* de = static_cast<SimpleDerivedElement*>(*it);
            EXPECT_EQ(static_cast<int>(i), de->get_value());
            ++i;
        }
    }

    TEST(ListContainerTest, DeletionWhileIterating)
    {
        ListContainer<SimpleDerivedElement> list(kCurrentLargestDerivedElementSize);
        for (int i = 0; i < 4; ++i)
            list.AllocateAndConstruct<SimpleDerivedElement>()->set_value(i);

        // Delete odd elements.
        for (auto it = list.begin(); it != list.end();) {
            if ((*it)->get_value() % 2)
                it = list.EraseAndInvalidateAllPointers(it);
            else
                ++it;
        }

        EXPECT_EQ(2u, list.size());
        EXPECT_EQ(0, list.front()->get_value());
        EXPECT_EQ(2, list.back()->get_value());

        // Erase all elements.
        for (auto it = list.begin(); it != list.end();)
            it = list.EraseAndInvalidateAllPointers(it);

        EXPECT_TRUE(list.empty());
    }

    TEST(ListContainerTest, InsertBeforeBegin)
    {
        ListContainer<DerivedElement> list(kCurrentLargestDerivedElementSize);
        std::vector<SimpleDerivedElement*> sde_list;
        const int size = 4;
        for (int i = 0; i < size; ++i) {
            sde_list.push_back(list.AllocateAndConstruct<SimpleDerivedElement>());
            sde_list.back()->set_value(i);
        }
        EXPECT_EQ(static_cast<size_t>(size), list.size());

        const int count = 2;
        ListContainer<DerivedElement>::Iterator iter = list.InsertBeforeAndInvalidateAllPointers<SimpleDerivedElement>(
            list.begin(), count);
        for (int i = 0; i < count; ++i) {
            static_cast<SimpleDerivedElement*>(*iter)->set_value(100 + i);
            ++iter;
        }

        const int expected_result[] = { 100, 101, 0, 1, 2, 3 };
        int iter_index = 0;
        for (iter = list.begin(); iter != list.end(); ++iter) {
            EXPECT_EQ(expected_result[iter_index],
                static_cast<SimpleDerivedElement*>(*iter)->get_value());
            ++iter_index;
        }
        EXPECT_EQ(size + count, iter_index);
    }

    TEST(ListContainerTest, InsertBeforeEnd)
    {
        ListContainer<DerivedElement> list(kCurrentLargestDerivedElementSize);
        std::vector<SimpleDerivedElement*> sde_list;
        const int size = 4;
        for (int i = 0; i < size; ++i) {
            sde_list.push_back(list.AllocateAndConstruct<SimpleDerivedElement>());
            sde_list.back()->set_value(i);
        }
        EXPECT_EQ(static_cast<size_t>(size), list.size());

        const int count = 3;
        ListContainer<DerivedElement>::Iterator iter = list.InsertBeforeAndInvalidateAllPointers<SimpleDerivedElement>(
            list.end(), count);
        for (int i = 0; i < count; ++i) {
            static_cast<SimpleDerivedElement*>(*iter)->set_value(100 + i);
            ++iter;
        }

        const int expected_result[] = { 0, 1, 2, 3, 100, 101, 102 };
        int iter_index = 0;
        for (iter = list.begin(); iter != list.end(); ++iter) {
            EXPECT_EQ(expected_result[iter_index],
                static_cast<SimpleDerivedElement*>(*iter)->get_value());
            ++iter_index;
        }
        EXPECT_EQ(size + count, iter_index);
    }

    TEST(ListContainerTest, InsertBeforeEmpty)
    {
        ListContainer<DerivedElement> list(kCurrentLargestDerivedElementSize);

        const int count = 3;
        ListContainer<DerivedElement>::Iterator iter = list.InsertBeforeAndInvalidateAllPointers<SimpleDerivedElement>(
            list.end(), count);
        for (int i = 0; i < count; ++i) {
            static_cast<SimpleDerivedElement*>(*iter)->set_value(100 + i);
            ++iter;
        }

        const int expected_result[] = { 100, 101, 102 };
        int iter_index = 0;
        for (iter = list.begin(); iter != list.end(); ++iter) {
            EXPECT_EQ(expected_result[iter_index],
                static_cast<SimpleDerivedElement*>(*iter)->get_value());
            ++iter_index;
        }
        EXPECT_EQ(count, iter_index);
    }

    TEST(ListContainerTest, InsertBeforeMany)
    {
        ListContainer<DerivedElement> list(kCurrentLargestDerivedElementSize);
        std::vector<SimpleDerivedElement*> sde_list;
        // Create a partial list of 1,...,99.
        int initial_list[] = {
            0,
            1,
            4,
            5,
            6,
            7,
            8,
            9,
            11,
            12,
            17,
            18,
            19,
            20,
            21,
            22,
            23,
            24,
            25,
            26,
            27,
            28,
            29,
            30,
            32,
            34,
            36,
            37,
            51,
            52,
            54,
            56,
            60,
            64,
            65,
            70,
            75,
            76,
            80,
            81,
            83,
            86,
            87,
            90,
            93,
            95,
            97,
            98,
        };
        const size_t size = sizeof(initial_list) / sizeof(initial_list[0]);
        for (size_t i = 0; i < size; ++i) {
            sde_list.push_back(list.AllocateAndConstruct<SimpleDerivedElement>());
            sde_list.back()->set_value(initial_list[i]);
        }
        EXPECT_EQ(static_cast<size_t>(size), list.size());

        // Insert the missing elements.
        ListContainer<DerivedElement>::Iterator iter = list.begin();
        while (iter != list.end()) {
            ListContainer<DerivedElement>::Iterator iter_next = iter;
            ++iter_next;

            int value = static_cast<SimpleDerivedElement*>(*iter)->get_value();
            int value_next = iter_next != list.end()
                ? static_cast<SimpleDerivedElement*>(*iter_next)->get_value()
                : 100;
            int count = value_next - value - 1;

            iter = list.InsertBeforeAndInvalidateAllPointers<SimpleDerivedElement>(
                iter_next, count);
            for (int i = value + 1; i < value_next; ++i) {
                static_cast<SimpleDerivedElement*>(*iter)->set_value(i);
                ++iter;
            }
        }

        int iter_index = 0;
        for (iter = list.begin(); iter != list.end(); ++iter) {
            EXPECT_EQ(iter_index,
                static_cast<SimpleDerivedElement*>(*iter)->get_value());
            ++iter_index;
        }
        EXPECT_EQ(100, iter_index);
    }

    TEST(ListContainerTest, SimpleManipulationWithIndexSimpleDerivedElement)
    {
        ListContainer<DerivedElement> list(kCurrentLargestDerivedElementSize);
        std::vector<SimpleDerivedElement*> de_list;
        int size = 10;
        for (int i = 0; i < size; ++i) {
            de_list.push_back(list.AllocateAndConstruct<SimpleDerivedElement>());
        }
        EXPECT_EQ(static_cast<size_t>(size), list.size());

        for (int i = 0; i < size; ++i) {
            static_cast<SimpleDerivedElement*>(list.ElementAt(i))->set_value(i);
        }

        int i = 0;
        for (std::vector<SimpleDerivedElement*>::const_iterator
                 de_iter
             = de_list.begin();
             de_iter != de_list.end(); ++de_iter, ++i) {
            EXPECT_EQ(i, (*de_iter)->get_value());
        }
    }

    TEST(ListContainerTest,
        SimpleManipulationWithIndexMoreThanOneAllocationSimpleDerivedElement)
    {
        ListContainer<DerivedElement> list(LargestDerivedElementSize(), 2);
        std::vector<SimpleDerivedElement*> de_list;
        int size = 10;
        for (int i = 0; i < size; ++i) {
            de_list.push_back(list.AllocateAndConstruct<SimpleDerivedElement>());
        }
        EXPECT_EQ(static_cast<size_t>(size), list.size());

        for (int i = 0; i < size; ++i) {
            static_cast<SimpleDerivedElement*>(list.ElementAt(i))->set_value(i);
        }

        int i = 0;
        for (std::vector<SimpleDerivedElement*>::const_iterator
                 de_iter
             = de_list.begin();
             de_iter != de_list.end(); ++de_iter, ++i) {
            EXPECT_EQ(i, (*de_iter)->get_value());
        }
    }

    TEST(ListContainerTest,
        SimpleIterationAndReverseIterationWithIndexNonDerivedElement)
    {
        ListContainer<NonDerivedElement> list;
        std::vector<NonDerivedElement*> nde_list;
        size_t size = 10;
        for (size_t i = 0; i < size; ++i) {
            nde_list.push_back(list.AllocateAndConstruct<NonDerivedElement>());
        }
        EXPECT_EQ(size, list.size());

        size_t i = 0;
        for (ListContainer<NonDerivedElement>::Iterator iter = list.begin();
             iter != list.end(); ++iter) {
            EXPECT_EQ(i, iter.index());
            ++i;
        }

        i = 0;
        for (ListContainer<NonDerivedElement>::ReverseIterator iter = list.rbegin();
             iter != list.rend(); ++iter) {
            EXPECT_EQ(i, iter.index());
            ++i;
        }
    }

    // Increments an int when constructed (or the counter pointer is supplied) and
    // decrements when destructed.
    class InstanceCounter {
    public:
        InstanceCounter()
            : counter_(nullptr)
        {
        }
        explicit InstanceCounter(int* counter) { SetCounter(counter); }
        ~InstanceCounter()
        {
            if (counter_)
                --*counter_;
        }
        void SetCounter(int* counter)
        {
            counter_ = counter;
            ++*counter_;
        }

    private:
        int* counter_;
    };

    TEST(ListContainerTest, RemoveLastDestruction)
    {
        // We keep an explicit instance count to make sure that the destructors are
        // indeed getting called.
        int counter = 0;
        ListContainer<InstanceCounter> list(sizeof(InstanceCounter), 1);
        EXPECT_EQ(0, counter);
        EXPECT_EQ(0u, list.size());

        // We should be okay to add one and then go back to zero.
        list.AllocateAndConstruct<InstanceCounter>()->SetCounter(&counter);
        EXPECT_EQ(1, counter);
        EXPECT_EQ(1u, list.size());
        list.RemoveLast();
        EXPECT_EQ(0, counter);
        EXPECT_EQ(0u, list.size());

        // We should also be okay to remove the last multiple times, as long as there
        // are enough elements in the first place.
        list.AllocateAndConstruct<InstanceCounter>()->SetCounter(&counter);
        list.AllocateAndConstruct<InstanceCounter>()->SetCounter(&counter);
        list.AllocateAndConstruct<InstanceCounter>()->SetCounter(&counter);
        list.AllocateAndConstruct<InstanceCounter>()->SetCounter(&counter);
        list.AllocateAndConstruct<InstanceCounter>()->SetCounter(&counter);
        list.AllocateAndConstruct<InstanceCounter>()->SetCounter(&counter);
        list.RemoveLast();
        list.RemoveLast();
        EXPECT_EQ(4, counter); // Leaves one in the last list.
        EXPECT_EQ(4u, list.size());
        list.RemoveLast();
        EXPECT_EQ(3, counter); // Removes an inner list from before.
        EXPECT_EQ(3u, list.size());
    }

    // TODO(jbroman): std::equal would work if ListContainer iterators satisfied the
    // usual STL iterator constraints. We should fix that.
    template <typename It1, typename It2>
    bool Equal(It1 it1, const It1& end1, It2 it2)
    {
        for (; it1 != end1; ++it1, ++it2) {
            if (!(*it1 == *it2))
                return false;
        }
        return true;
    }

    TEST(ListContainerTest, RemoveLastIteration)
    {
        struct SmallStruct {
            char dummy[16];
        };
        ListContainer<SmallStruct> list(sizeof(SmallStruct), 1);
        std::vector<SmallStruct*> pointers;

        // Utilities which keep these two lists in sync and check that their iteration
        // order matches.
        auto push = [&list, &pointers]() {
            pointers.push_back(list.AllocateAndConstruct<SmallStruct>());
        };
        auto pop = [&list, &pointers]() {
            pointers.pop_back();
            list.RemoveLast();
        };
        auto check_equal = [&list, &pointers]() {
            // They should be of the same size, and compare equal with all four kinds of
            // iteration.
            // Apparently Mac doesn't have vector::cbegin and vector::crbegin?
            const auto& const_pointers = pointers;
            ASSERT_EQ(list.size(), pointers.size());
            ASSERT_TRUE(Equal(list.begin(), list.end(), pointers.begin()));
            ASSERT_TRUE(Equal(list.cbegin(), list.cend(), const_pointers.begin()));
            ASSERT_TRUE(Equal(list.rbegin(), list.rend(), pointers.rbegin()));
            ASSERT_TRUE(Equal(list.crbegin(), list.crend(), const_pointers.rbegin()));
        };

        check_equal(); // Initially empty.
        push();
        check_equal(); // One full inner list.
        push();
        check_equal(); // One full, one partially full.
        push();
        push();
        check_equal(); // Two full, one partially full.
        pop();
        check_equal(); // Two full, one empty.
        pop();
        check_equal(); // One full, one partially full, one empty.
        pop();
        check_equal(); // One full, one empty.
        push();
        pop();
        pop();
        ASSERT_TRUE(list.empty());
        check_equal(); // Empty.
    }

    TEST(ListContainerTest, AppendByMovingSameList)
    {
        ListContainer<SimpleDerivedElement> list(kCurrentLargestDerivedElementSize);
        list.AllocateAndConstruct<SimpleDerivedElementConstructMagicNumberOne>();

        list.AppendByMoving(list.front());
        EXPECT_EQ(kMagicNumberToUseForSimpleDerivedElementOne,
            list.back()->get_value());
        EXPECT_EQ(2u, list.size());

        list.front()->set_value(kMagicNumberToUseForSimpleDerivedElementTwo);
        EXPECT_EQ(kMagicNumberToUseForSimpleDerivedElementTwo,
            list.front()->get_value());
        list.AppendByMoving(list.front());
        EXPECT_EQ(kMagicNumberToUseForSimpleDerivedElementTwo,
            list.back()->get_value());
        EXPECT_EQ(3u, list.size());
    }

    TEST(ListContainerTest, AppendByMovingDoesNotDestruct)
    {
        ListContainer<DerivedElement> list_1(kCurrentLargestDerivedElementSize);
        ListContainer<DerivedElement> list_2(kCurrentLargestDerivedElementSize);
        MockDerivedElement* mde_1 = list_1.AllocateAndConstruct<MockDerivedElement>();

        // Make sure destructor isn't called during AppendByMoving.
        list_2.AppendByMoving(mde_1);
        EXPECT_CALL(*mde_1, Destruct()).Times(0);
        testing::Mock::VerifyAndClearExpectations(mde_1);
        mde_1 = static_cast<MockDerivedElement*>(list_2.back());
        EXPECT_CALL(*mde_1, Destruct());
    }

    TEST(ListContainerTest, AppendByMovingReturnsMovedPointer)
    {
        ListContainer<SimpleDerivedElement> list_1(kCurrentLargestDerivedElementSize);
        ListContainer<SimpleDerivedElement> list_2(kCurrentLargestDerivedElementSize);
        SimpleDerivedElement* simple_element = list_1.AllocateAndConstruct<SimpleDerivedElement>();

        SimpleDerivedElement* moved_1 = list_2.AppendByMoving(simple_element);
        EXPECT_EQ(list_2.back(), moved_1);

        SimpleDerivedElement* moved_2 = list_1.AppendByMoving(moved_1);
        EXPECT_EQ(list_1.back(), moved_2);
        EXPECT_NE(moved_1, moved_2);
    }

    TEST(ListContainerTest, AppendByMovingReplacesSourceWithNewDerivedElement)
    {
        ListContainer<SimpleDerivedElementConstructMagicNumberOne> list_1(
            kCurrentLargestDerivedElementSize);
        ListContainer<SimpleDerivedElementConstructMagicNumberTwo> list_2(
            kCurrentLargestDerivedElementSize);

        list_1.AllocateAndConstruct<SimpleDerivedElementConstructMagicNumberOne>();
        EXPECT_EQ(kMagicNumberToUseForSimpleDerivedElementOne,
            list_1.front()->get_value());

        list_2.AppendByMoving(list_1.front());
        EXPECT_EQ(kMagicNumberToUseForSimpleDerivedElementOne,
            list_1.front()->get_value());
        EXPECT_EQ(kMagicNumberToUseForSimpleDerivedElementOne,
            list_2.front()->get_value());

        // Change the value of list_2.front() to ensure the value is actually moved.
        list_2.back()->set_value(kMagicNumberToUseForSimpleDerivedElementThree);

        list_1.AppendByMoving(list_2.back());
        EXPECT_EQ(kMagicNumberToUseForSimpleDerivedElementOne,
            list_1.front()->get_value());
        EXPECT_EQ(kMagicNumberToUseForSimpleDerivedElementThree,
            list_1.back()->get_value());
        EXPECT_EQ(kMagicNumberToUseForSimpleDerivedElementTwo,
            list_2.back()->get_value());

        // AppendByMoving replaces the source element with a new derived element so
        // we do not expect sizes to shrink after AppendByMoving is called.
        EXPECT_EQ(2u, list_1.size()); // One direct allocation, one AppendByMoving.
        EXPECT_EQ(1u, list_2.size()); // One AppendByMoving.
    }

    const size_t kLongCountForLongSimpleDerivedElement = 5;

    class LongSimpleDerivedElement : public SimpleDerivedElement {
    public:
        ~LongSimpleDerivedElement() override { }
        void SetAllValues(unsigned long value)
        {
            for (size_t i = 0; i < kLongCountForLongSimpleDerivedElement; i++)
                values[i] = value;
        }
        bool AreAllValuesEqualTo(size_t value) const
        {
            for (size_t i = 1; i < kLongCountForLongSimpleDerivedElement; i++) {
                if (values[i] != values[0])
                    return false;
            }
            return true;
        }

    private:
        unsigned long values[kLongCountForLongSimpleDerivedElement];
    };

    const unsigned long kMagicNumberToUseForLongSimpleDerivedElement = 2718ul;

    class LongSimpleDerivedElementConstructMagicNumber
        : public LongSimpleDerivedElement {
    public:
        LongSimpleDerivedElementConstructMagicNumber()
        {
            SetAllValues(kMagicNumberToUseForLongSimpleDerivedElement);
        }
    };

    TEST(ListContainerTest, AppendByMovingLongAndSimpleDerivedElements)
    {
        static_assert(sizeof(LongSimpleDerivedElement) > sizeof(SimpleDerivedElement),
            "LongSimpleDerivedElement should be larger than "
            "SimpleDerivedElement's size.");
        static_assert(sizeof(LongSimpleDerivedElement) <= kLargestDerivedElementSize,
            "LongSimpleDerivedElement should be smaller than the maximum "
            "DerivedElement size.");

        ListContainer<SimpleDerivedElement> list(kCurrentLargestDerivedElementSize);
        list.AllocateAndConstruct<LongSimpleDerivedElementConstructMagicNumber>();
        list.AllocateAndConstruct<SimpleDerivedElementConstructMagicNumberOne>();

        EXPECT_TRUE(
            static_cast<LongSimpleDerivedElement*>(list.front())
                ->AreAllValuesEqualTo(kMagicNumberToUseForLongSimpleDerivedElement));
        EXPECT_EQ(kMagicNumberToUseForSimpleDerivedElementOne,
            list.back()->get_value());

        // Test that moving a simple derived element actually moves enough data so
        // that the LongSimpleDerivedElement at this location is entirely moved.
        SimpleDerivedElement* simple_element = list.back();
        list.AppendByMoving(list.front());
        EXPECT_TRUE(
            static_cast<LongSimpleDerivedElement*>(list.back())
                ->AreAllValuesEqualTo(kMagicNumberToUseForLongSimpleDerivedElement));
        EXPECT_EQ(kMagicNumberToUseForSimpleDerivedElementOne,
            simple_element->get_value());

        LongSimpleDerivedElement* long_element = static_cast<LongSimpleDerivedElement*>(list.back());
        list.AppendByMoving(simple_element);
        EXPECT_TRUE(long_element->AreAllValuesEqualTo(
            kMagicNumberToUseForLongSimpleDerivedElement));
        EXPECT_EQ(kMagicNumberToUseForSimpleDerivedElementOne,
            list.back()->get_value());
    }

    TEST(ListContainerTest, Swap)
    {
        ListContainer<SimpleDerivedElement> list_1(kCurrentLargestDerivedElementSize);
        list_1.AllocateAndConstruct<SimpleDerivedElementConstructMagicNumberOne>();
        ListContainer<SimpleDerivedElement> list_2(kCurrentLargestDerivedElementSize);
        list_2.AllocateAndConstruct<SimpleDerivedElementConstructMagicNumberTwo>();
        list_2.AllocateAndConstruct<SimpleDerivedElementConstructMagicNumberThree>();

        SimpleDerivedElement* pre_swap_list_1_front = list_1.front();

        EXPECT_EQ(kMagicNumberToUseForSimpleDerivedElementOne,
            list_1.front()->get_value());
        EXPECT_EQ(1u, list_1.size());

        EXPECT_EQ(kMagicNumberToUseForSimpleDerivedElementTwo,
            list_2.front()->get_value());
        EXPECT_EQ(kMagicNumberToUseForSimpleDerivedElementThree,
            list_2.back()->get_value());
        EXPECT_EQ(2u, list_2.size());

        list_2.swap(list_1);

        EXPECT_EQ(kMagicNumberToUseForSimpleDerivedElementTwo,
            list_1.front()->get_value());
        EXPECT_EQ(kMagicNumberToUseForSimpleDerivedElementThree,
            list_1.back()->get_value());
        EXPECT_EQ(2u, list_1.size());

        EXPECT_EQ(kMagicNumberToUseForSimpleDerivedElementOne,
            list_2.front()->get_value());
        EXPECT_EQ(1u, list_2.size());

        // Ensure pointers are still valid after swapping.
        EXPECT_EQ(pre_swap_list_1_front, list_2.front());
    }

    TEST(ListContainerTest, GetCapacityInBytes)
    {
        const int iterations = 500;
        const size_t initial_capacity = 10;
        const size_t upper_bound_on_min_capacity = initial_capacity;

        // At time of writing, removing elements from the end can cause up to 7x the
        // memory required to be consumed, in the worst case, since we can have up to
        // two trailing inner lists that are empty (for 2*size + 4*size in unused
        // memory, due to the exponential growth strategy).
        const size_t max_waste_factor = 8;

        ListContainer<DerivedElement> list(LargestDerivedElementSize(),
            initial_capacity);

        // The capacity should grow with the list.
        for (int i = 0; i < iterations; i++) {
            size_t capacity = list.GetCapacityInBytes();
            ASSERT_GE(capacity, list.size() * LargestDerivedElementSize());
            ASSERT_LE(capacity, std::max(list.size(), upper_bound_on_min_capacity) * max_waste_factor * LargestDerivedElementSize());
            list.AllocateAndConstruct<DerivedElement1>();
        }

        // The capacity should shrink with the list.
        for (int i = 0; i < iterations; i++) {
            size_t capacity = list.GetCapacityInBytes();
            ASSERT_GE(capacity, list.size() * LargestDerivedElementSize());
            ASSERT_LE(capacity, std::max(list.size(), upper_bound_on_min_capacity) * max_waste_factor * LargestDerivedElementSize());
            list.RemoveLast();
        }
    }

} // namespace
} // namespace cc
