// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#include "core/layout/LayoutMultiColumnFlowThread.h"

#include "core/layout/LayoutMultiColumnSet.h"
#include "core/layout/LayoutMultiColumnSpannerPlaceholder.h"
#include "core/layout/LayoutTestHelper.h"

#include <gtest/gtest.h>

namespace blink {

namespace {

class MultiColumnRenderingTest : public RenderingTest {
public:
    LayoutMultiColumnFlowThread* findFlowThread(const char* id) const;

    // Generate a signature string based on what kind of column boxes the flow thread has
    // established. 'c' is used for regular column content sets, while 's' is used for spanners.
    // '?' is used when there's an unknown box type (which should be considered a failure).
    String columnSetSignature(LayoutMultiColumnFlowThread*);
    String columnSetSignature(const char* multicolId);

    void setMulticolHTML(const String&);
};

LayoutMultiColumnFlowThread* MultiColumnRenderingTest::findFlowThread(const char* id) const
{
    Node* multicol = document().getElementById(id);
    if (!multicol)
        return nullptr;
    LayoutBlockFlow* multicolContainer = toLayoutBlockFlow(multicol->layoutObject());
    if (!multicolContainer)
        return nullptr;
    return multicolContainer->multiColumnFlowThread();
}

String MultiColumnRenderingTest::columnSetSignature(LayoutMultiColumnFlowThread* flowThread)
{
    String signature = "";
    for (LayoutBox* columnBox = flowThread->firstMultiColumnBox();
        columnBox;
        columnBox = columnBox->nextSiblingMultiColumnBox()) {
        if (columnBox->isLayoutMultiColumnSpannerPlaceholder())
            signature.append('s');
        else if (columnBox->isLayoutMultiColumnSet())
            signature.append('c');
        else
            signature.append('?');
    }
    return signature;
}

String MultiColumnRenderingTest::columnSetSignature(const char* multicolId)
{
    return columnSetSignature(findFlowThread(multicolId));
}

void MultiColumnRenderingTest::setMulticolHTML(const String& html)
{
    const char* style =
        "<style>"
        "  #mc { -webkit-columns:2; }"
        "  .s, #spanner, #spanner1, #spanner2 { -webkit-column-span:all; }"
        "</style>";
    setBodyInnerHTML(style + html);
}

TEST_F(MultiColumnRenderingTest, OneBlockWithInDepthTreeStructureCheck)
{
    // Examine the layout tree established by a simple multicol container with a block with some text inside.
    setMulticolHTML("<div id='mc'><div>xxx</div></div>");
    Node* multicol = document().getElementById("mc");
    ASSERT_TRUE(multicol);
    LayoutBlockFlow* multicolContainer = toLayoutBlockFlow(multicol->layoutObject());
    ASSERT_TRUE(multicolContainer);
    LayoutMultiColumnFlowThread* flowThread = multicolContainer->multiColumnFlowThread();
    ASSERT_TRUE(flowThread);
    EXPECT_EQ(columnSetSignature(flowThread), "c");
    EXPECT_EQ(flowThread->parent(), multicolContainer);
    EXPECT_FALSE(flowThread->previousSibling());
    LayoutMultiColumnSet* columnSet = flowThread->firstMultiColumnSet();
    ASSERT_TRUE(columnSet);
    EXPECT_EQ(columnSet->previousSibling(), flowThread);
    EXPECT_FALSE(columnSet->nextSibling());
    LayoutBlockFlow* block = toLayoutBlockFlow(flowThread->firstChild());
    ASSERT_TRUE(block);
    EXPECT_FALSE(block->nextSibling());
    ASSERT_TRUE(block->firstChild());
    EXPECT_TRUE(block->firstChild()->isText());
    EXPECT_FALSE(block->firstChild()->nextSibling());
}

TEST_F(MultiColumnRenderingTest, Empty)
{
    // If there's no column content, there should be no column set.
    setMulticolHTML("<div id='mc'></div>");
    EXPECT_EQ(columnSetSignature("mc"), "");
}

TEST_F(MultiColumnRenderingTest, OneBlock)
{
    // There is some content, so we should create a column set.
    setMulticolHTML("<div id='mc'><div id='block'></div></div>");
    LayoutMultiColumnFlowThread* flowThread = findFlowThread("mc");
    ASSERT_EQ(columnSetSignature(flowThread), "c");
    LayoutMultiColumnSet* columnSet = flowThread->firstMultiColumnSet();
    EXPECT_EQ(flowThread->mapDescendantToColumnSet(document().getElementById("block")->layoutObject()), columnSet);
}

TEST_F(MultiColumnRenderingTest, TwoBlocks)
{
    // No matter how much content, we should only create one column set (unless there are spanners).
    setMulticolHTML("<div id='mc'><div id='block1'></div><div id='block2'></div></div>");
    LayoutMultiColumnFlowThread* flowThread = findFlowThread("mc");
    ASSERT_EQ(columnSetSignature(flowThread), "c");
    LayoutMultiColumnSet* columnSet = flowThread->firstMultiColumnSet();
    EXPECT_EQ(flowThread->mapDescendantToColumnSet(document().getElementById("block1")->layoutObject()), columnSet);
    EXPECT_EQ(flowThread->mapDescendantToColumnSet(document().getElementById("block2")->layoutObject()), columnSet);
}

TEST_F(MultiColumnRenderingTest, Spanner)
{
    // With one spanner and no column content, we should create a spanner set.
    setMulticolHTML("<div id='mc'><div id='spanner'></div></div>");
    LayoutMultiColumnFlowThread* flowThread = findFlowThread("mc");
    ASSERT_EQ(columnSetSignature(flowThread), "s");
    LayoutBox* columnBox = flowThread->firstMultiColumnBox();
    EXPECT_EQ(flowThread->firstMultiColumnSet(), nullptr);
    EXPECT_EQ(flowThread->containingColumnSpannerPlaceholder(document().getElementById("spanner")->layoutObject()), columnBox);
    EXPECT_EQ(document().getElementById("spanner")->layoutObject()->spannerPlaceholder(), columnBox);
}

TEST_F(MultiColumnRenderingTest, ContentThenSpanner)
{
    // With some column content followed by a spanner, we need a column set followed by a spanner set.
    setMulticolHTML("<div id='mc'><div id='columnContent'></div><div id='spanner'></div></div>");
    LayoutMultiColumnFlowThread* flowThread = findFlowThread("mc");
    ASSERT_EQ(columnSetSignature(flowThread), "cs");
    LayoutBox* columnBox = flowThread->firstMultiColumnBox();
    EXPECT_EQ(flowThread->mapDescendantToColumnSet(document().getElementById("columnContent")->layoutObject()), columnBox);
    columnBox = columnBox->nextSiblingMultiColumnBox();
    EXPECT_EQ(flowThread->containingColumnSpannerPlaceholder(document().getElementById("spanner")->layoutObject()), columnBox);
    EXPECT_EQ(flowThread->containingColumnSpannerPlaceholder(document().getElementById("columnContent")->layoutObject()), nullptr);
}

TEST_F(MultiColumnRenderingTest, SpannerThenContent)
{
    // With a spanner followed by some column content, we need a spanner set followed by a column set.
    setMulticolHTML("<div id='mc'><div id='spanner'></div><div id='columnContent'></div></div>");
    LayoutMultiColumnFlowThread* flowThread = findFlowThread("mc");
    ASSERT_EQ(columnSetSignature(flowThread), "sc");
    LayoutBox* columnBox = flowThread->firstMultiColumnBox();
    EXPECT_EQ(flowThread->containingColumnSpannerPlaceholder(document().getElementById("spanner")->layoutObject()), columnBox);
    columnBox = columnBox->nextSiblingMultiColumnBox();
    EXPECT_EQ(flowThread->mapDescendantToColumnSet(document().getElementById("columnContent")->layoutObject()), columnBox);
    EXPECT_EQ(flowThread->containingColumnSpannerPlaceholder(document().getElementById("columnContent")->layoutObject()), nullptr);
}

TEST_F(MultiColumnRenderingTest, ContentThenSpannerThenContent)
{
    // With column content followed by a spanner followed by some column content, we need a column
    // set followed by a spanner set followed by a column set.
    setMulticolHTML("<div id='mc'><div id='columnContentBefore'></div><div id='spanner'></div><div id='columnContentAfter'></div></div>");
    LayoutMultiColumnFlowThread* flowThread = findFlowThread("mc");
    ASSERT_EQ(columnSetSignature(flowThread), "csc");
    LayoutBox* columnBox = flowThread->firstMultiColumnSet();
    EXPECT_EQ(flowThread->mapDescendantToColumnSet(document().getElementById("columnContentBefore")->layoutObject()), columnBox);
    EXPECT_EQ(flowThread->containingColumnSpannerPlaceholder(document().getElementById("columnContentBefore")->layoutObject()), nullptr);
    columnBox = columnBox->nextSiblingMultiColumnBox();
    EXPECT_EQ(flowThread->containingColumnSpannerPlaceholder(document().getElementById("spanner")->layoutObject()), columnBox);
    columnBox = columnBox->nextSiblingMultiColumnBox();
    EXPECT_EQ(flowThread->mapDescendantToColumnSet(document().getElementById("columnContentAfter")->layoutObject()), columnBox);
    EXPECT_EQ(flowThread->containingColumnSpannerPlaceholder(document().getElementById("columnContentAfter")->layoutObject()), nullptr);
}

TEST_F(MultiColumnRenderingTest, TwoSpanners)
{
    // With two spanners and no column content, we need two spanner sets.
    setMulticolHTML("<div id='mc'><div id='spanner1'></div><div id='spanner2'></div></div>");
    LayoutMultiColumnFlowThread* flowThread = findFlowThread("mc");
    ASSERT_EQ(columnSetSignature(flowThread), "ss");
    LayoutBox* columnBox = flowThread->firstMultiColumnBox();
    EXPECT_EQ(flowThread->firstMultiColumnSet(), nullptr);
    EXPECT_EQ(flowThread->containingColumnSpannerPlaceholder(document().getElementById("spanner1")->layoutObject()), columnBox);
    EXPECT_EQ(document().getElementById("spanner1")->layoutObject()->spannerPlaceholder(), columnBox);
    columnBox = columnBox->nextSiblingMultiColumnBox();
    EXPECT_EQ(flowThread->containingColumnSpannerPlaceholder(document().getElementById("spanner2")->layoutObject()), columnBox);
    EXPECT_EQ(document().getElementById("spanner2")->layoutObject()->spannerPlaceholder(), columnBox);
}

TEST_F(MultiColumnRenderingTest, SpannerThenContentThenSpanner)
{
    // With two spanners and some column content in-between, we need a spanner set, a column set and another spanner set.
    setMulticolHTML("<div id='mc'><div id='spanner1'></div><div id='columnContent'></div><div id='spanner2'></div></div>");
    LayoutMultiColumnFlowThread* flowThread = findFlowThread("mc");
    ASSERT_EQ(columnSetSignature(flowThread), "scs");
    LayoutMultiColumnSet* columnSet = flowThread->firstMultiColumnSet();
    EXPECT_EQ(columnSet->nextSiblingMultiColumnSet(), nullptr);
    LayoutBox* columnBox = flowThread->firstMultiColumnBox();
    EXPECT_EQ(flowThread->containingColumnSpannerPlaceholder(document().getElementById("spanner1")->layoutObject()), columnBox);
    columnBox = columnBox->nextSiblingMultiColumnBox();
    EXPECT_EQ(columnBox, columnSet);
    EXPECT_EQ(flowThread->mapDescendantToColumnSet(document().getElementById("columnContent")->layoutObject()), columnSet);
    EXPECT_EQ(flowThread->containingColumnSpannerPlaceholder(document().getElementById("columnContent")->layoutObject()), nullptr);
    columnBox = columnBox->nextSiblingMultiColumnBox();
    EXPECT_EQ(flowThread->containingColumnSpannerPlaceholder(document().getElementById("spanner2")->layoutObject()), columnBox);
}

TEST_F(MultiColumnRenderingTest, SpannerWithSpanner)
{
    // column-span:all on something inside column-span:all has no effect.
    setMulticolHTML("<div id='mc'><div id='spanner'><div id='invalidSpanner' class='s'></div></div></div>");
    LayoutMultiColumnFlowThread* flowThread = findFlowThread("mc");
    ASSERT_EQ(columnSetSignature(flowThread), "s");
    LayoutBox* columnBox = flowThread->firstMultiColumnBox();
    EXPECT_EQ(flowThread->containingColumnSpannerPlaceholder(document().getElementById("spanner")->layoutObject()), columnBox);
    EXPECT_EQ(flowThread->containingColumnSpannerPlaceholder(document().getElementById("invalidSpanner")->layoutObject()), columnBox);
    EXPECT_EQ(toLayoutMultiColumnSpannerPlaceholder(columnBox)->layoutObjectInFlowThread(), document().getElementById("spanner")->layoutObject());
    EXPECT_EQ(document().getElementById("spanner")->layoutObject()->spannerPlaceholder(), columnBox);
    EXPECT_EQ(document().getElementById("invalidSpanner")->layoutObject()->spannerPlaceholder(), nullptr);
}

TEST_F(MultiColumnRenderingTest, SubtreeWithSpanner)
{
    setMulticolHTML("<div id='mc'><div id='outer'><div id='block1'></div><div id='spanner'></div><div id='block2'></div></div></div>");
    LayoutMultiColumnFlowThread* flowThread = findFlowThread("mc");
    EXPECT_EQ(columnSetSignature(flowThread), "csc");
    LayoutBox* columnBox = flowThread->firstMultiColumnBox();
    EXPECT_EQ(flowThread->mapDescendantToColumnSet(document().getElementById("outer")->layoutObject()), columnBox);
    EXPECT_EQ(flowThread->mapDescendantToColumnSet(document().getElementById("block1")->layoutObject()), columnBox);
    columnBox = columnBox->nextSiblingMultiColumnBox();
    EXPECT_EQ(flowThread->containingColumnSpannerPlaceholder(document().getElementById("spanner")->layoutObject()), columnBox);
    EXPECT_EQ(document().getElementById("spanner")->layoutObject()->spannerPlaceholder(), columnBox);
    EXPECT_EQ(toLayoutMultiColumnSpannerPlaceholder(columnBox)->layoutObjectInFlowThread(), document().getElementById("spanner")->layoutObject());
    EXPECT_EQ(flowThread->containingColumnSpannerPlaceholder(document().getElementById("outer")->layoutObject()), nullptr);
    EXPECT_EQ(flowThread->containingColumnSpannerPlaceholder(document().getElementById("block1")->layoutObject()), nullptr);
    EXPECT_EQ(flowThread->containingColumnSpannerPlaceholder(document().getElementById("block2")->layoutObject()), nullptr);
    columnBox = columnBox->nextSiblingMultiColumnBox();
    EXPECT_EQ(flowThread->mapDescendantToColumnSet(document().getElementById("block2")->layoutObject()), columnBox);
}

TEST_F(MultiColumnRenderingTest, SubtreeWithSpannerAfterSpanner)
{
    setMulticolHTML("<div id='mc'><div id='spanner1'></div><div id='outer'>text<div id='spanner2'></div><div id='after'></div></div></div>");
    LayoutMultiColumnFlowThread* flowThread = findFlowThread("mc");
    EXPECT_EQ(columnSetSignature(flowThread), "scsc");
    LayoutBox* columnBox = flowThread->firstMultiColumnBox();
    EXPECT_EQ(flowThread->containingColumnSpannerPlaceholder(document().getElementById("spanner1")->layoutObject()), columnBox);
    EXPECT_EQ(toLayoutMultiColumnSpannerPlaceholder(columnBox)->layoutObjectInFlowThread(), document().getElementById("spanner1")->layoutObject());
    EXPECT_EQ(document().getElementById("spanner1")->layoutObject()->spannerPlaceholder(), columnBox);
    columnBox = columnBox->nextSiblingMultiColumnBox();
    EXPECT_EQ(flowThread->mapDescendantToColumnSet(document().getElementById("outer")->layoutObject()), columnBox);
    columnBox = columnBox->nextSiblingMultiColumnBox();
    EXPECT_EQ(flowThread->containingColumnSpannerPlaceholder(document().getElementById("spanner2")->layoutObject()), columnBox);
    EXPECT_EQ(toLayoutMultiColumnSpannerPlaceholder(columnBox)->layoutObjectInFlowThread(), document().getElementById("spanner2")->layoutObject());
    EXPECT_EQ(document().getElementById("spanner2")->layoutObject()->spannerPlaceholder(), columnBox);
    EXPECT_EQ(flowThread->containingColumnSpannerPlaceholder(document().getElementById("outer")->layoutObject()), nullptr);
    EXPECT_EQ(flowThread->containingColumnSpannerPlaceholder(document().getElementById("after")->layoutObject()), nullptr);
    columnBox = columnBox->nextSiblingMultiColumnBox();
    EXPECT_EQ(flowThread->mapDescendantToColumnSet(document().getElementById("after")->layoutObject()), columnBox);
}

TEST_F(MultiColumnRenderingTest, SubtreeWithSpannerBeforeSpanner)
{
    setMulticolHTML("<div id='mc'><div id='outer'>text<div id='spanner1'></div>text</div><div id='spanner2'></div></div>");
    LayoutMultiColumnFlowThread* flowThread = findFlowThread("mc");
    EXPECT_EQ(columnSetSignature(flowThread), "cscs");
    LayoutBox* columnBox = flowThread->firstMultiColumnSet();
    EXPECT_EQ(flowThread->mapDescendantToColumnSet(document().getElementById("outer")->layoutObject()), columnBox);
    columnBox = columnBox->nextSiblingMultiColumnBox();
    EXPECT_EQ(flowThread->containingColumnSpannerPlaceholder(document().getElementById("spanner1")->layoutObject()), columnBox);
    EXPECT_EQ(document().getElementById("spanner1")->layoutObject()->spannerPlaceholder(), columnBox);
    EXPECT_EQ(toLayoutMultiColumnSpannerPlaceholder(columnBox)->layoutObjectInFlowThread(), document().getElementById("spanner1")->layoutObject());
    columnBox = columnBox->nextSiblingMultiColumnBox()->nextSiblingMultiColumnBox();
    EXPECT_EQ(flowThread->containingColumnSpannerPlaceholder(document().getElementById("spanner2")->layoutObject()), columnBox);
    EXPECT_EQ(document().getElementById("spanner2")->layoutObject()->spannerPlaceholder(), columnBox);
    EXPECT_EQ(toLayoutMultiColumnSpannerPlaceholder(columnBox)->layoutObjectInFlowThread(), document().getElementById("spanner2")->layoutObject());
    EXPECT_EQ(flowThread->containingColumnSpannerPlaceholder(document().getElementById("outer")->layoutObject()), nullptr);
}

TEST_F(MultiColumnRenderingTest, columnSetAtBlockOffset)
{
    setMulticolHTML("<div id='mc' style='line-height:100px;'>text<br>text<br>text<br>text<br>text<div id='spanner1'>spanner</div>text<br>text<div id='spanner2'>text<br>text</div>text</div>");
    LayoutMultiColumnFlowThread* flowThread = findFlowThread("mc");
    EXPECT_EQ(columnSetSignature(flowThread), "cscsc");
    LayoutMultiColumnSet* firstRow = flowThread->firstMultiColumnSet();
    EXPECT_EQ(flowThread->columnSetAtBlockOffset(LayoutUnit(-10000)), firstRow); // negative overflow
    EXPECT_EQ(flowThread->columnSetAtBlockOffset(LayoutUnit()), firstRow);
    EXPECT_EQ(flowThread->columnSetAtBlockOffset(LayoutUnit(499)), firstRow); // bottom of last line in first row.
    LayoutMultiColumnSet* secondRow = firstRow->nextSiblingMultiColumnSet();
    EXPECT_EQ(flowThread->columnSetAtBlockOffset(LayoutUnit(500)), secondRow);
    EXPECT_EQ(flowThread->columnSetAtBlockOffset(LayoutUnit(699)), secondRow);
    LayoutMultiColumnSet* thirdRow = secondRow->nextSiblingMultiColumnSet();
    EXPECT_EQ(flowThread->columnSetAtBlockOffset(LayoutUnit(700)), thirdRow);
    EXPECT_EQ(flowThread->columnSetAtBlockOffset(LayoutUnit(799)), thirdRow); // bottom of last row
    EXPECT_EQ(flowThread->columnSetAtBlockOffset(LayoutUnit(10000)), thirdRow); // overflow
}

TEST_F(MultiColumnRenderingTest, columnSetAtBlockOffsetVerticalRl)
{
    setMulticolHTML("<div id='mc' style='line-height:100px; -webkit-writing-mode:vertical-rl;'>text<br>text<br>text<br>text<br>text<div id='spanner1'>spanner</div>text<br>text<div id='spanner2'>text<br>text</div>text</div>");
    LayoutMultiColumnFlowThread* flowThread = findFlowThread("mc");
    EXPECT_EQ(columnSetSignature(flowThread), "cscsc");
    LayoutMultiColumnSet* firstRow = flowThread->firstMultiColumnSet();
    EXPECT_EQ(flowThread->columnSetAtBlockOffset(LayoutUnit(-10000)), firstRow); // negative overflow
    EXPECT_EQ(flowThread->columnSetAtBlockOffset(LayoutUnit()), firstRow);
    EXPECT_EQ(flowThread->columnSetAtBlockOffset(LayoutUnit(499)), firstRow); // bottom of last line in first row.
    LayoutMultiColumnSet* secondRow = firstRow->nextSiblingMultiColumnSet();
    EXPECT_EQ(flowThread->columnSetAtBlockOffset(LayoutUnit(500)), secondRow);
    EXPECT_EQ(flowThread->columnSetAtBlockOffset(LayoutUnit(699)), secondRow);
    LayoutMultiColumnSet* thirdRow = secondRow->nextSiblingMultiColumnSet();
    EXPECT_EQ(flowThread->columnSetAtBlockOffset(LayoutUnit(700)), thirdRow);
    EXPECT_EQ(flowThread->columnSetAtBlockOffset(LayoutUnit(799)), thirdRow); // bottom of last row
    EXPECT_EQ(flowThread->columnSetAtBlockOffset(LayoutUnit(10000)), thirdRow); // overflow
}

TEST_F(MultiColumnRenderingTest, columnSetAtBlockOffsetVerticalLr)
{
    setMulticolHTML("<div id='mc' style='line-height:100px; -webkit-writing-mode:vertical-lr;'>text<br>text<br>text<br>text<br>text<div id='spanner1'>spanner</div>text<br>text<div id='spanner2'>text<br>text</div>text</div>");
    LayoutMultiColumnFlowThread* flowThread = findFlowThread("mc");
    EXPECT_EQ(columnSetSignature(flowThread), "cscsc");
    LayoutMultiColumnSet* firstRow = flowThread->firstMultiColumnSet();
    EXPECT_EQ(flowThread->columnSetAtBlockOffset(LayoutUnit(-10000)), firstRow); // negative overflow
    EXPECT_EQ(flowThread->columnSetAtBlockOffset(LayoutUnit()), firstRow);
    EXPECT_EQ(flowThread->columnSetAtBlockOffset(LayoutUnit(499)), firstRow); // bottom of last line in first row.
    LayoutMultiColumnSet* secondRow = firstRow->nextSiblingMultiColumnSet();
    EXPECT_EQ(flowThread->columnSetAtBlockOffset(LayoutUnit(500)), secondRow);
    EXPECT_EQ(flowThread->columnSetAtBlockOffset(LayoutUnit(699)), secondRow);
    LayoutMultiColumnSet* thirdRow = secondRow->nextSiblingMultiColumnSet();
    EXPECT_EQ(flowThread->columnSetAtBlockOffset(LayoutUnit(700)), thirdRow);
    EXPECT_EQ(flowThread->columnSetAtBlockOffset(LayoutUnit(799)), thirdRow); // bottom of last row
    EXPECT_EQ(flowThread->columnSetAtBlockOffset(LayoutUnit(10000)), thirdRow); // overflow
}

class MultiColumnTreeModifyingTest : public MultiColumnRenderingTest {
public:
    void setMulticolHTML(const char*);
    void reparentLayoutObject(const char* newParentId, const char* childId, const char* insertBeforeId = nullptr);
    void destroyLayoutObject(LayoutObject* child);
    void destroyLayoutObject(const char* childId);
};

void MultiColumnTreeModifyingTest::setMulticolHTML(const char* html)
{
    MultiColumnRenderingTest::setMulticolHTML(html);
    // Allow modifications to the layout tree structure, because that's what we want to test.
    document().lifecycle().advanceTo(DocumentLifecycle::InStyleRecalc);
}

void MultiColumnTreeModifyingTest::reparentLayoutObject(const char* newParentId, const char* childId, const char* insertBeforeId)
{
    LayoutObject* newParent = document().getElementById(newParentId)->layoutObject();
    LayoutObject* child = document().getElementById(childId)->layoutObject();
    LayoutObject* insertBefore = insertBeforeId ? document().getElementById(insertBeforeId)->layoutObject() : nullptr;
    child->remove();
    newParent->addChild(child, insertBefore);
}

void MultiColumnTreeModifyingTest::destroyLayoutObject(LayoutObject* child)
{
    // Remove and destroy in separate steps, so that we get to test removal of subtrees.
    child->remove();
    child->node()->detach();
}

void MultiColumnTreeModifyingTest::destroyLayoutObject(const char* childId)
{
    destroyLayoutObject(document().getElementById(childId)->layoutObject());
}

TEST_F(MultiColumnTreeModifyingTest, InsertFirstContentAndRemove)
{
    setMulticolHTML("<div id='block'></div><div id='mc'></div>");
    LayoutMultiColumnFlowThread* flowThread = findFlowThread("mc");
    LayoutBlockFlow* block = toLayoutBlockFlow(document().getElementById("block")->layoutObject());
    LayoutBlockFlow* multicolContainer = toLayoutBlockFlow(document().getElementById("mc")->layoutObject());
    block->remove();
    multicolContainer->addChild(block);
    EXPECT_EQ(block->parent(), flowThread);
    // A set should have appeared, now that the multicol container has content.
    EXPECT_EQ(columnSetSignature(flowThread), "c");

    destroyLayoutObject(block);
    // The set should be gone again now, since there's nothing inside the multicol container anymore.
    EXPECT_EQ(columnSetSignature("mc"), "");
}

TEST_F(MultiColumnTreeModifyingTest, InsertContentBeforeContentAndRemove)
{
    setMulticolHTML("<div id='block'></div><div id='mc'><div id='insertBefore'></div></div>");
    EXPECT_EQ(columnSetSignature("mc"), "c");
    reparentLayoutObject("mc", "block", "insertBefore");
    // There was already some content prior to our insertion, so no new set should be inserted.
    EXPECT_EQ(columnSetSignature("mc"), "c");
    destroyLayoutObject("block");
    // There's still some content after the removal, so the set should remain.
    EXPECT_EQ(columnSetSignature("mc"), "c");
}

TEST_F(MultiColumnTreeModifyingTest, InsertContentAfterContentAndRemove)
{
    setMulticolHTML("<div id='block'></div><div id='mc'><div></div></div>");
    EXPECT_EQ(columnSetSignature("mc"), "c");
    reparentLayoutObject("mc", "block");
    // There was already some content prior to our insertion, so no new set should be inserted.
    EXPECT_EQ(columnSetSignature("mc"), "c");
    destroyLayoutObject("block");
    // There's still some content after the removal, so the set should remain.
    EXPECT_EQ(columnSetSignature("mc"), "c");
}

TEST_F(MultiColumnTreeModifyingTest, InsertSpannerAndRemove)
{
    setMulticolHTML("<div id='spanner'></div><div id='mc'></div>");
    LayoutMultiColumnFlowThread* flowThread = findFlowThread("mc");
    LayoutBlockFlow* spanner = toLayoutBlockFlow(document().getElementById("spanner")->layoutObject());
    LayoutBlockFlow* multicolContainer = toLayoutBlockFlow(document().getElementById("mc")->layoutObject());
    spanner->remove();
    multicolContainer->addChild(spanner);
    EXPECT_EQ(spanner->parent(), flowThread);
    // We should now have a spanner placeholder, since we just moved a spanner into the multicol container.
    EXPECT_EQ(columnSetSignature(flowThread), "s");
    destroyLayoutObject(spanner);
    EXPECT_EQ(columnSetSignature(flowThread), "");
}

TEST_F(MultiColumnTreeModifyingTest, InsertTwoSpannersAndRemove)
{
    setMulticolHTML("<div id='block'>ee<div class='s'></div><div class='s'></div></div><div id='mc'></div>");
    reparentLayoutObject("mc", "block");
    EXPECT_EQ(columnSetSignature("mc"), "css");
    destroyLayoutObject("block");
    EXPECT_EQ(columnSetSignature("mc"), "");
}

TEST_F(MultiColumnTreeModifyingTest, InsertSpannerAfterContentAndRemove)
{
    setMulticolHTML("<div id='spanner'></div><div id='mc'><div></div></div>");
    reparentLayoutObject("mc", "spanner");
    // We should now have a spanner placeholder, since we just moved a spanner into the multicol container.
    EXPECT_EQ(columnSetSignature("mc"), "cs");
    destroyLayoutObject("spanner");
    EXPECT_EQ(columnSetSignature("mc"), "c");
}

TEST_F(MultiColumnTreeModifyingTest, InsertSpannerBeforeContentAndRemove)
{
    setMulticolHTML("<div id='spanner'></div><div id='mc'><div id='columnContent'></div></div>");
    reparentLayoutObject("mc", "spanner", "columnContent");
    // We should now have a spanner placeholder, since we just moved a spanner into the multicol container.
    EXPECT_EQ(columnSetSignature("mc"), "sc");
    destroyLayoutObject("spanner");
    EXPECT_EQ(columnSetSignature("mc"), "c");
}

TEST_F(MultiColumnTreeModifyingTest, InsertSpannerBetweenContentAndRemove)
{
    setMulticolHTML("<div id='spanner'></div><div id='mc'><div></div><div id='insertBefore'></div></div>");
    reparentLayoutObject("mc", "spanner", "insertBefore");
    // Since the spanner was inserted in the middle of column content, what used to be one column
    // set had to be split in two, in order to get a spot to insert the spanner placeholder.
    EXPECT_EQ(columnSetSignature("mc"), "csc");
    destroyLayoutObject("spanner");
    // The spanner placeholder should be gone again now, and the two sets be merged into one.
    EXPECT_EQ(columnSetSignature("mc"), "c");
}

TEST_F(MultiColumnTreeModifyingTest, InsertSubtreeWithContentAndSpannerAndRemove)
{
    setMulticolHTML("<div id='block'>text<div id='spanner'></div>text</div><div id='mc'></div>");
    reparentLayoutObject("mc", "block");
    EXPECT_EQ(columnSetSignature("mc"), "csc");
    destroyLayoutObject("block");
    EXPECT_EQ(columnSetSignature("mc"), "");
}

TEST_F(MultiColumnTreeModifyingTest, InsertInsideSpannerAndRemove)
{
    setMulticolHTML("<div id='block'>text</div><div id='mc'><div id='spanner'></div></div>");
    reparentLayoutObject("spanner", "block");
    EXPECT_EQ(columnSetSignature("mc"), "s");
    destroyLayoutObject("block");
    EXPECT_EQ(columnSetSignature("mc"), "s");
}

TEST_F(MultiColumnTreeModifyingTest, InsertSpannerInContentBeforeSpannerAndRemove)
{
    setMulticolHTML("<div id='spanner'></div><div id='mc'><div></div><div id='insertBefore'></div><div class='s'></div></div>");
    EXPECT_EQ(columnSetSignature("mc"), "cs");
    reparentLayoutObject("mc", "spanner", "insertBefore");
    EXPECT_EQ(columnSetSignature("mc"), "cscs");
    destroyLayoutObject("spanner");
    EXPECT_EQ(columnSetSignature("mc"), "cs");
}

TEST_F(MultiColumnTreeModifyingTest, InsertSpannerInContentAfterSpannerAndRemove)
{
    setMulticolHTML("<div id='spanner'></div><div id='mc'><div class='s'></div><div></div><div id='insertBefore'></div></div>");
    EXPECT_EQ(columnSetSignature("mc"), "sc");
    reparentLayoutObject("mc", "spanner", "insertBefore");
    EXPECT_EQ(columnSetSignature("mc"), "scsc");
    destroyLayoutObject("spanner");
    EXPECT_EQ(columnSetSignature("mc"), "sc");
}

TEST_F(MultiColumnTreeModifyingTest, InsertSpannerAfterSpannerAndRemove)
{
    setMulticolHTML("<div id='spanner'></div><div id='mc'><div class='s'></div></div>");
    reparentLayoutObject("mc", "spanner");
    EXPECT_EQ(columnSetSignature("mc"), "ss");
    destroyLayoutObject("spanner");
    EXPECT_EQ(columnSetSignature("mc"), "s");
}

TEST_F(MultiColumnTreeModifyingTest, InsertSpannerBeforeSpannerAndRemove)
{
    setMulticolHTML("<div id='spanner'></div><div id='mc'><div id='insertBefore' class='s'></div></div>");
    reparentLayoutObject("mc", "spanner", "insertBefore");
    EXPECT_EQ(columnSetSignature("mc"), "ss");
    destroyLayoutObject("spanner");
    EXPECT_EQ(columnSetSignature("mc"), "s");
}

TEST_F(MultiColumnTreeModifyingTest, InsertContentBeforeSpannerAndRemove)
{
    setMulticolHTML("<div id='block'></div><div id='mc'><div id='insertBefore' class='s'></div></div>");
    reparentLayoutObject("mc", "block", "insertBefore");
    EXPECT_EQ(columnSetSignature("mc"), "cs");
    destroyLayoutObject("block");
    EXPECT_EQ(columnSetSignature("mc"), "s");
}

TEST_F(MultiColumnTreeModifyingTest, InsertContentAfterContentBeforeSpannerAndRemove)
{
    setMulticolHTML("<div id='block'></div><div id='mc'>text<div id='insertBefore' class='s'></div></div>");
    EXPECT_EQ(columnSetSignature("mc"), "cs");
    reparentLayoutObject("mc", "block", "insertBefore");
    // There was already some content before the spanner prior to our insertion, so no new set
    // should be inserted.
    EXPECT_EQ(columnSetSignature("mc"), "cs");
    destroyLayoutObject("block");
    EXPECT_EQ(columnSetSignature("mc"), "cs");
}

TEST_F(MultiColumnTreeModifyingTest, InsertContentAfterContentAndSpannerAndRemove)
{
    setMulticolHTML("<div id='block'></div><div id='mc'>content<div class='s'></div></div>");
    EXPECT_EQ(columnSetSignature("mc"), "cs");
    reparentLayoutObject("mc", "block");
    EXPECT_EQ(columnSetSignature("mc"), "csc");
    destroyLayoutObject("block");
    EXPECT_EQ(columnSetSignature("mc"), "cs");
}

TEST_F(MultiColumnTreeModifyingTest, InsertContentBeforeSpannerAndContentAndRemove)
{
    setMulticolHTML("<div id='block'></div><div id='mc'><div id='insertBefore' class='s'></div>content</div>");
    EXPECT_EQ(columnSetSignature("mc"), "sc");
    reparentLayoutObject("mc", "block", "insertBefore");
    EXPECT_EQ(columnSetSignature("mc"), "csc");
    destroyLayoutObject("block");
    EXPECT_EQ(columnSetSignature("mc"), "sc");
}

TEST_F(MultiColumnTreeModifyingTest, InsertSpannerIntoContentBeforeSpannerAndRemove)
{
    setMulticolHTML("<div id='spanner'></div><div id='mc'><div></div><div id='insertBefore'></div><div class='s'></div><div class='s'></div><div></div></div>");
    EXPECT_EQ(columnSetSignature("mc"), "cssc");
    reparentLayoutObject("mc", "spanner", "insertBefore");
    EXPECT_EQ(columnSetSignature("mc"), "cscssc");
    destroyLayoutObject("spanner");
    EXPECT_EQ(columnSetSignature("mc"), "cssc");
}

TEST_F(MultiColumnTreeModifyingTest, InsertSpannerIntoContentAfterSpannerAndRemove)
{
    setMulticolHTML("<div id='spanner'></div><div id='mc'><div></div><div class='s'></div><div class='s'></div><div></div><div id='insertBefore'></div></div>");
    EXPECT_EQ(columnSetSignature("mc"), "cssc");
    reparentLayoutObject("mc", "spanner", "insertBefore");
    EXPECT_EQ(columnSetSignature("mc"), "csscsc");
    destroyLayoutObject("spanner");
    EXPECT_EQ(columnSetSignature("mc"), "cssc");
}

TEST_F(MultiColumnTreeModifyingTest, InsertInvalidSpannerAndRemove)
{
    setMulticolHTML("<div class='s' id='invalidSpanner'></div><div id='mc'><div id='spanner'></div></div>");
    EXPECT_EQ(columnSetSignature("mc"), "s");
    reparentLayoutObject("spanner", "invalidSpanner");
    // It's not allowed to nest spanners.
    EXPECT_EQ(columnSetSignature("mc"), "s");
    destroyLayoutObject("invalidSpanner");
    EXPECT_EQ(columnSetSignature("mc"), "s");
}

TEST_F(MultiColumnTreeModifyingTest, InsertSpannerWithInvalidSpannerAndRemove)
{
    setMulticolHTML("<div id='spanner'><div class='s' id='invalidSpanner'></div></div><div id='mc'></div>");
    reparentLayoutObject("mc", "spanner");
    // It's not allowed to nest spanners.
    EXPECT_EQ(columnSetSignature("mc"), "s");
    destroyLayoutObject("spanner");
    EXPECT_EQ(columnSetSignature("mc"), "");
}

TEST_F(MultiColumnTreeModifyingTest, InsertInvalidSpannerInSpannerBetweenContentAndRemove)
{
    setMulticolHTML("<div class='s' id='invalidSpanner'></div><div id='mc'>text<div id='spanner'></div>text</div>");
    EXPECT_EQ(columnSetSignature("mc"), "csc");
    reparentLayoutObject("spanner", "invalidSpanner");
    EXPECT_EQ(columnSetSignature("mc"), "csc");
    destroyLayoutObject("invalidSpanner");
    EXPECT_EQ(columnSetSignature("mc"), "csc");
}

TEST_F(MultiColumnTreeModifyingTest, InsertContentAndSpannerAndRemove)
{
    setMulticolHTML("<div id='block'>text<div id='spanner'></div></div><div id='mc'>text</div>");
    reparentLayoutObject("mc", "block");
    EXPECT_EQ(columnSetSignature("mc"), "cs");
    destroyLayoutObject("block");
    EXPECT_EQ(columnSetSignature("mc"), "c");
}

TEST_F(MultiColumnTreeModifyingTest, InsertContentAndSpannerAndContentAndRemove)
{
    setMulticolHTML("<div id='block'><div id='spanner'></div>text</div><div id='mc'></div>");
    reparentLayoutObject("mc", "block");
    EXPECT_EQ(columnSetSignature("mc"), "csc");
    destroyLayoutObject("block");
    EXPECT_EQ(columnSetSignature("mc"), "");
}

TEST_F(MultiColumnTreeModifyingTest, InsertSubtreeWithSpannerAndRemove)
{
    setMulticolHTML("<div id='block'>text<div class='s'></div>text</div><div id='mc'></div>");
    reparentLayoutObject("mc", "block");
    EXPECT_EQ(columnSetSignature("mc"), "csc");
    destroyLayoutObject("block");
    EXPECT_EQ(columnSetSignature("mc"), "");
}

TEST_F(MultiColumnTreeModifyingTest, InsertSubtreeWithSpannerAfterContentAndRemove)
{
    setMulticolHTML("<div id='block'>text<div class='s'></div>text</div><div id='mc'>column content</div>");
    reparentLayoutObject("mc", "block");
    EXPECT_EQ(columnSetSignature("mc"), "csc");
    destroyLayoutObject("block");
    EXPECT_EQ(columnSetSignature("mc"), "c");
}

TEST_F(MultiColumnTreeModifyingTest, InsertSubtreeWithSpannerBeforeContentAndRemove)
{
    setMulticolHTML("<div id='block'>text<div class='s'></div>text</div><div id='mc'><div id='insertBefore'>column content</div></div>");
    reparentLayoutObject("mc", "block", "insertBefore");
    EXPECT_EQ(columnSetSignature("mc"), "csc");
    destroyLayoutObject("block");
    EXPECT_EQ(columnSetSignature("mc"), "c");
}

TEST_F(MultiColumnTreeModifyingTest, InsertSubtreeWithSpannerInsideContentAndRemove)
{
    setMulticolHTML("<div id='block'>text<div class='s'></div>text</div><div id='mc'><div>outside<div id='insertBefore'>outside</div></div></div>");
    EXPECT_EQ(columnSetSignature("mc"), "c");
    reparentLayoutObject("mc", "block", "insertBefore");
    EXPECT_EQ(columnSetSignature("mc"), "csc");
    destroyLayoutObject("block");
    EXPECT_EQ(columnSetSignature("mc"), "c");
}

TEST_F(MultiColumnTreeModifyingTest, InsertSubtreeWithSpannerAfterSpannerAndRemove)
{
    setMulticolHTML("<div id='block'>text<div class='s'></div>text</div><div id='mc'><div class='s'></div></div>");
    EXPECT_EQ(columnSetSignature("mc"), "s");
    reparentLayoutObject("mc", "block");
    EXPECT_EQ(columnSetSignature("mc"), "scsc");
    destroyLayoutObject("block");
    EXPECT_EQ(columnSetSignature("mc"), "s");
}

TEST_F(MultiColumnTreeModifyingTest, InsertSubtreeWithSpannerBeforeSpannerAndRemove)
{
    setMulticolHTML("<div id='block'>text<div class='s'></div>text</div><div id='mc'><div id='insertBefore' class='s'></div></div>");
    EXPECT_EQ(columnSetSignature("mc"), "s");
    reparentLayoutObject("mc", "block", "insertBefore");
    EXPECT_EQ(columnSetSignature("mc"), "cscs");
    destroyLayoutObject("block");
    EXPECT_EQ(columnSetSignature("mc"), "s");
}

TEST_F(MultiColumnTreeModifyingTest, RemoveSpannerAndContent)
{
    setMulticolHTML("<div id='mc'><div id='block'>text<div class='s'></div>text</div></div>");
    EXPECT_EQ(columnSetSignature("mc"), "csc");
    destroyLayoutObject("block");
    EXPECT_EQ(columnSetSignature("mc"), "");
}

TEST_F(MultiColumnTreeModifyingTest, RemoveSpannerAndSomeContentBefore)
{
    setMulticolHTML("<div id='mc'>text<div id='block'>text<div class='s'></div></div></div>");
    EXPECT_EQ(columnSetSignature("mc"), "cs");
    destroyLayoutObject("block");
    EXPECT_EQ(columnSetSignature("mc"), "c");
}

TEST_F(MultiColumnTreeModifyingTest, RemoveSpannerAndAllContentBefore)
{
    setMulticolHTML("<div id='mc'><div id='block'>text<div class='s'></div></div></div>");
    EXPECT_EQ(columnSetSignature("mc"), "cs");
    destroyLayoutObject("block");
    EXPECT_EQ(columnSetSignature("mc"), "");
}

TEST_F(MultiColumnTreeModifyingTest, RemoveSpannerAndAllContentBeforeWithContentAfter)
{
    setMulticolHTML("<div id='mc'><div id='block'>text<div class='s'></div></div>text</div>");
    EXPECT_EQ(columnSetSignature("mc"), "csc");
    destroyLayoutObject("block");
    EXPECT_EQ(columnSetSignature("mc"), "c");
}

TEST_F(MultiColumnTreeModifyingTest, RemoveSpannerAndSomeContentAfter)
{
    setMulticolHTML("<div id='mc'><div id='block'><div class='s'></div>text</div>text</div>");
    EXPECT_EQ(columnSetSignature("mc"), "csc");
    destroyLayoutObject("block");
    EXPECT_EQ(columnSetSignature("mc"), "c");
}

TEST_F(MultiColumnTreeModifyingTest, RemoveSpannerAndAllContentAfter)
{
    setMulticolHTML("<div id='mc'><div id='block'><div class='s'></div>text</div></div>");
    EXPECT_EQ(columnSetSignature("mc"), "csc");
    destroyLayoutObject("block");
    EXPECT_EQ(columnSetSignature("mc"), "");
}

TEST_F(MultiColumnTreeModifyingTest, RemoveSpannerAndAllContentAfterWithContentBefore)
{
    setMulticolHTML("<div id='mc'>text<div id='block'><div class='s'></div>text</div></div>");
    EXPECT_EQ(columnSetSignature("mc"), "csc");
    destroyLayoutObject("block");
    EXPECT_EQ(columnSetSignature("mc"), "c");
}

TEST_F(MultiColumnTreeModifyingTest, RemoveTwoSpannersBeforeContent)
{
    setMulticolHTML("<div id='mc'><div id='block'><div class='s'></div><div class='s'></div></div>text</div>");
    EXPECT_EQ(columnSetSignature("mc"), "cssc");
    destroyLayoutObject("block");
    EXPECT_EQ(columnSetSignature("mc"), "c");
}

TEST_F(MultiColumnTreeModifyingTest, RemoveSpannerAndContentAndSpanner)
{
    setMulticolHTML("<div id='mc'><div id='block'><div class='s'></div>text<div class='s'></div>text</div></div>");
    EXPECT_EQ(columnSetSignature("mc"), "cscsc");
    destroyLayoutObject("block");
    EXPECT_EQ(columnSetSignature("mc"), "");
}

TEST_F(MultiColumnTreeModifyingTest, RemoveSpannerAndContentAndSpannerBeforeContent)
{
    setMulticolHTML("<div id='mc'><div id='block'><div class='s'></div>text<div class='s'></div></div>text</div>");
    EXPECT_EQ(columnSetSignature("mc"), "cscsc");
    destroyLayoutObject("block");
    EXPECT_EQ(columnSetSignature("mc"), "c");
}

TEST_F(MultiColumnTreeModifyingTest, RemoveSpannerAndContentAndSpannerAfterContent)
{
    setMulticolHTML("<div id='mc'>text<div id='block'><div class='s'></div>text<div class='s'></div></div></div>");
    EXPECT_EQ(columnSetSignature("mc"), "cscs");
    destroyLayoutObject("block");
    EXPECT_EQ(columnSetSignature("mc"), "c");
}

TEST_F(MultiColumnTreeModifyingTest, RemoveInvalidSpannerInSpannerBetweenContent)
{
    setMulticolHTML("<div id='mc'>text<div class='s'><div id='spanner'></div></div>text</div>");
    EXPECT_EQ(columnSetSignature("mc"), "csc");
    destroyLayoutObject("spanner");
    EXPECT_EQ(columnSetSignature("mc"), "csc");
}

TEST_F(MultiColumnTreeModifyingTest, RemoveSpannerWithInvalidSpannerBetweenContent)
{
    setMulticolHTML("<div id='mc'>text<div id='spanner'><div class='s'></div></div>text</div>");
    EXPECT_EQ(columnSetSignature("mc"), "csc");
    destroyLayoutObject("spanner");
    EXPECT_EQ(columnSetSignature("mc"), "c");
}

} // anonymous namespace

} // namespace blink
