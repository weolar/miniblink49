// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#include "core/layout/MultiColumnFragmentainerGroup.h"

#include "core/layout/LayoutMultiColumnFlowThread.h"
#include "core/layout/LayoutMultiColumnSet.h"
#include "core/layout/LayoutTestHelper.h"

#include <gtest/gtest.h>

namespace blink {

namespace {

class MultiColumnFragmentainerGroupTest : public RenderingTest {
public:
    MultiColumnFragmentainerGroupTest() : m_flowThread(nullptr), m_columnSet(nullptr) { }

protected:
    virtual void SetUp() override;
    virtual void TearDown() override;

    LayoutMultiColumnSet& columnSet() { return *m_columnSet; }

    static int groupCount(const MultiColumnFragmentainerGroupList&);

private:
    LayoutMultiColumnFlowThread* m_flowThread;
    LayoutMultiColumnSet* m_columnSet;
};

void MultiColumnFragmentainerGroupTest::SetUp()
{
    RenderingTest::SetUp();
    RefPtr<ComputedStyle> style = ComputedStyle::create();
    m_flowThread = LayoutMultiColumnFlowThread::createAnonymous(document(), *style.get());
    m_columnSet = LayoutMultiColumnSet::createAnonymous(*m_flowThread, *m_flowThread->style());
}

void MultiColumnFragmentainerGroupTest::TearDown()
{
    m_columnSet->destroy();
    m_flowThread->destroy();
    RenderingTest::TearDown();
}

int MultiColumnFragmentainerGroupTest::groupCount(const MultiColumnFragmentainerGroupList& groupList)
{
    int count = 0;
    for (const auto& dummyGroup : groupList) {
        (void) dummyGroup;
        count++;
    }
    return count;
}

TEST_F(MultiColumnFragmentainerGroupTest, Create)
{
    MultiColumnFragmentainerGroupList groupList(columnSet());
    EXPECT_EQ(groupCount(groupList), 1);
}

TEST_F(MultiColumnFragmentainerGroupTest, DeleteExtra)
{
    MultiColumnFragmentainerGroupList groupList(columnSet());
    EXPECT_EQ(groupCount(groupList), 1);
    groupList.deleteExtraGroups();
    EXPECT_EQ(groupCount(groupList), 1);
}

TEST_F(MultiColumnFragmentainerGroupTest, AddThenDeleteExtra)
{
    MultiColumnFragmentainerGroupList groupList(columnSet());
    EXPECT_EQ(groupCount(groupList), 1);
    groupList.addExtraGroup();
    EXPECT_EQ(groupCount(groupList), 2);
    groupList.deleteExtraGroups();
    EXPECT_EQ(groupCount(groupList), 1);
}

TEST_F(MultiColumnFragmentainerGroupTest, AddTwoThenDeleteExtraThenAddThreeThenDeleteExtra)
{
    MultiColumnFragmentainerGroupList groupList(columnSet());
    EXPECT_EQ(groupCount(groupList), 1);
    groupList.addExtraGroup();
    EXPECT_EQ(groupCount(groupList), 2);
    groupList.addExtraGroup();
    EXPECT_EQ(groupCount(groupList), 3);
    groupList.deleteExtraGroups();
    EXPECT_EQ(groupCount(groupList), 1);
    groupList.addExtraGroup();
    EXPECT_EQ(groupCount(groupList), 2);
    groupList.addExtraGroup();
    EXPECT_EQ(groupCount(groupList), 3);
    groupList.addExtraGroup();
    EXPECT_EQ(groupCount(groupList), 4);
    groupList.deleteExtraGroups();
    EXPECT_EQ(groupCount(groupList), 1);
}

} // anonymous namespace

} // blink
