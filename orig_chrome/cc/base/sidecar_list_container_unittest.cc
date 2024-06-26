// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/base/sidecar_list_container.h"

#include <algorithm>

#include "base/logging.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace cc {
namespace {

    const size_t kNumInitiallyReservedElements = 2;
    const size_t kNumElementsForTest = 100;

    class DestructionNotifier {
    public:
        DestructionNotifier()
            : flag_(nullptr)
        {
        }
        explicit DestructionNotifier(bool* flag) { set_flag(flag); }
        ~DestructionNotifier()
        {
            if (flag_)
                *flag_ = true;
        }
        void set_flag(bool* flag)
        {
            if (flag)
                DCHECK(!*flag);
            flag_ = flag;
        }

    private:
        bool* flag_;
    };

    class TestElement {
    public:
        TestElement() { }
        virtual ~TestElement() { }
        void set_destruction_flag(bool* flag) { notifier_.set_flag(flag); }

    private:
        DestructionNotifier notifier_;
    };

    class DerivedTestElement : public TestElement {
    public:
        int additional_field = 0;
    };

    class TestSidecar {
    public:
        TestSidecar() { }
        explicit TestSidecar(bool* destruction_flag)
            : notifier_(destruction_flag)
        {
        }

        static void Destroy(void* sidecar)
        {
            static_cast<TestSidecar*>(sidecar)->~TestSidecar();
        }

    protected:
        virtual ~TestSidecar() { }

    private:
        DestructionNotifier notifier_;
    };

    class DerivedTestSidecar : public TestSidecar {
    public:
        DerivedTestSidecar() { }
        explicit DerivedTestSidecar(bool* destruction_flag)
            : TestSidecar(destruction_flag)
        {
        }
        int additional_field = 0;

    private:
        ~DerivedTestSidecar() override { }
    };

    class TestContainer : public SidecarListContainer<TestElement> {
    public:
        TestContainer()
            : SidecarListContainer(
                std::max(sizeof(TestElement), sizeof(DerivedTestElement)),
                std::max(sizeof(TestSidecar), sizeof(DerivedTestSidecar)),
                kNumInitiallyReservedElements,
                &TestSidecar::Destroy)
        {
        }
    };

    TEST(SidecarListContainerTest, Destructor)
    {
        bool element_destroyed = false;
        bool sidecar_destroyed = false;

        {
            TestContainer container;

            TestElement* element = container.AllocateAndConstruct<TestElement>();
            TestSidecar* sidecar = new (container.GetSidecar(element)) TestSidecar(&sidecar_destroyed);
            element->set_destruction_flag(&element_destroyed);

            // They shouldn't be destroyed yet. And they shouldn't overlap in memory.
            ASSERT_FALSE(element_destroyed);
            ASSERT_FALSE(sidecar_destroyed);
            ASSERT_GE(reinterpret_cast<char*>(sidecar),
                reinterpret_cast<char*>(element) + sizeof(TestElement));
        }

        // They should, however, be destroyed when going out of scope.
        ASSERT_TRUE(element_destroyed);
        ASSERT_TRUE(sidecar_destroyed);
    }

    TEST(SidecarListContainerTest, Clear)
    {
        bool element_destroyed = false;
        bool sidecar_destroyed = false;

        TestContainer container;

        TestElement* element = container.AllocateAndConstruct<TestElement>();
        new (container.GetSidecar(element)) TestSidecar(&sidecar_destroyed);
        element->set_destruction_flag(&element_destroyed);

        // They shouldn't be destroyed yet.
        ASSERT_FALSE(element_destroyed);
        ASSERT_FALSE(sidecar_destroyed);

        // They should, however, be destroyed after clearing.
        container.clear();
        EXPECT_TRUE(element_destroyed);
        EXPECT_TRUE(sidecar_destroyed);
    }

    TEST(SidecarListContainerTest, DerivedTypes)
    {
        bool element_destroyed = false;
        bool sidecar_destroyed = false;

        {
            TestContainer container;

            DerivedTestElement* element = container.AllocateAndConstruct<DerivedTestElement>();
            DerivedTestSidecar* sidecar = new (container.GetSidecar(element))
                DerivedTestSidecar(&sidecar_destroyed);
            element->set_destruction_flag(&element_destroyed);
            element->additional_field = 12;
            sidecar->additional_field = 13;

            // They shouldn't be destroyed yet.
            ASSERT_FALSE(element_destroyed);
            ASSERT_FALSE(sidecar_destroyed);
        }

        // They should, however, be destroyed when going out of scope.
        EXPECT_TRUE(element_destroyed);
        EXPECT_TRUE(sidecar_destroyed);
    }

    TEST(SidecarListContainerTest, AddingAndRemovingElements)
    {
        TestContainer container;
        EXPECT_TRUE(container.empty());
        EXPECT_EQ(0u, container.size());
        EXPECT_EQ(container.end(), container.begin());

        for (size_t i = 1; i <= kNumElementsForTest; i++) {
            TestElement* element = container.AllocateAndConstruct<TestElement>();
            new (container.GetSidecar(element)) TestSidecar();

            ASSERT_FALSE(container.empty());
            ASSERT_EQ(i, container.size());
            ASSERT_NE(container.end(), container.begin());
        }

        size_t num_elements = 0;
        for (const auto* element : container) {
            (void)element;
            num_elements++;
        }
        EXPECT_EQ(kNumElementsForTest, num_elements);

        container.clear();
        EXPECT_TRUE(container.empty());
        EXPECT_EQ(0u, container.size());
        EXPECT_EQ(container.end(), container.begin());
    }

    TEST(SidecarListContainerTest, RemoveLast)
    {
        // We need only ensure that the sidecar is also destroyed on RemoveLast.
        // The rest is logic already present in ListContainer.
        bool sidecar_destroyed = false;
        TestContainer container;
        TestElement* element = container.AllocateAndConstruct<TestElement>();
        new (container.GetSidecar(element)) TestSidecar(&sidecar_destroyed);
        ASSERT_FALSE(sidecar_destroyed);
        container.RemoveLast();
        ASSERT_TRUE(sidecar_destroyed);
    }

} // namespace
} // namespace cc
