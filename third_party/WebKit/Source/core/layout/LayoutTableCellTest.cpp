/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/layout/LayoutTableCell.h"

#include "core/layout/LayoutTestHelper.h"

namespace blink {

namespace {

class LayoutTableCellDeathTest : public RenderingTest {
protected:
    virtual void SetUp()
    {
        RenderingTest::SetUp();
        m_cell = LayoutTableCell::createAnonymous(&document());
    }

    virtual void TearDown()
    {
        m_cell->destroy();
    }

    LayoutTableCell* m_cell;
};

TEST_F(LayoutTableCellDeathTest, CanSetColumn)
{
    static const unsigned columnIndex = 10;
    m_cell->setCol(columnIndex);
    EXPECT_EQ(columnIndex, m_cell->col());
}

TEST_F(LayoutTableCellDeathTest, CanSetColumnToMaxColumnIndex)
{
    m_cell->setCol(maxColumnIndex);
    EXPECT_EQ(maxColumnIndex, m_cell->col());
}

// FIXME: Re-enable these tests once ASSERT_DEATH is supported for Android.
// See: https://bugs.webkit.org/show_bug.cgi?id=74089
#if !OS(ANDROID)

TEST_F(LayoutTableCellDeathTest, CrashIfColumnOverflowOnSetting)
{
    ASSERT_DEATH(m_cell->setCol(maxColumnIndex + 1), "");
}

TEST_F(LayoutTableCellDeathTest, CrashIfSettingUnsetColumnIndex)
{
    ASSERT_DEATH(m_cell->setCol(unsetColumnIndex), "");
}

#endif

class LayoutTableCellTest : public RenderingTest { };

TEST_F(LayoutTableCellTest, ResetColspanIfTooBig)
{
    setBodyInnerHTML("<table><td colspan='14000'></td></table>");

    LayoutTableCell* cell = toLayoutTableCell(document().body()->firstChild()->firstChild()->firstChild()->firstChild()->layoutObject());
    ASSERT_EQ(cell->colSpan(), 8190U);
}

TEST_F(LayoutTableCellTest, DoNotResetColspanJustBelowBoundary)
{
    setBodyInnerHTML("<table><td colspan='8190'></td></table>");

    LayoutTableCell* cell = toLayoutTableCell(document().body()->firstChild()->firstChild()->firstChild()->firstChild()->layoutObject());
    ASSERT_EQ(cell->colSpan(), 8190U);
}

TEST_F(LayoutTableCellTest, ResetRowspanIfTooBig)
{
    setBodyInnerHTML("<table><td rowspan='14000'></td></table>");

    LayoutTableCell* cell = toLayoutTableCell(document().body()->firstChild()->firstChild()->firstChild()->firstChild()->layoutObject());
    ASSERT_EQ(cell->rowSpan(), 8190U);
}

TEST_F(LayoutTableCellTest, DoNotResetRowspanJustBelowBoundary)
{
    setBodyInnerHTML("<table><td rowspan='8190'></td></table>");

    LayoutTableCell* cell = toLayoutTableCell(document().body()->firstChild()->firstChild()->firstChild()->firstChild()->layoutObject());
    ASSERT_EQ(cell->rowSpan(), 8190U);
}

}

} // namespace blink
