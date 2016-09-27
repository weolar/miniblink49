// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/dom/NthIndexCache.h"

#include "core/dom/Document.h"
#include "core/html/HTMLElement.h"
#include "core/testing/DummyPageHolder.h"
#include <gtest/gtest.h>

namespace blink {

class NthIndexCacheTest : public ::testing::Test {
protected:
    void SetUp() override;

    Document& document() const { return m_dummyPageHolder->document(); }
    void setHtmlInnerHTML(const char* htmlContent);

private:
    OwnPtr<DummyPageHolder> m_dummyPageHolder;
};

void NthIndexCacheTest::SetUp()
{
    m_dummyPageHolder = DummyPageHolder::create(IntSize(800, 600));
}

TEST_F(NthIndexCacheTest, NthIndex)
{
    document().documentElement()->setInnerHTML("<body>"
        "<span id=first></span><span></span><span></span><span></span><span></span>"
        "<span></span><span></span><span></span><span></span><span></span>"
        "Text does not count"
        "<span id=nth-last-child></span>"
        "<span id=nth-child></span>"
        "<span></span><span></span><span></span><span></span><span></span>"
        "<span></span><span></span><span></span><span></span><span id=last></span>"
        "</body>", ASSERT_NO_EXCEPTION);

    NthIndexCache nthIndexCache(document());

    EXPECT_EQ(nthIndexCache.nthChildIndex(*document().getElementById("nth-child")), 12U);
    EXPECT_EQ(nthIndexCache.nthLastChildIndex(*document().getElementById("nth-last-child")), 12U);
}

} // namespace blink
