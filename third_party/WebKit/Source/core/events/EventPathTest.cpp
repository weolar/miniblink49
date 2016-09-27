// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/events/EventPath.h"

#include "core/HTMLNames.h"
#include "core/dom/Document.h"
#include "core/dom/PseudoElement.h"
#include "core/style/ComputedStyleConstants.h"
#include "core/testing/DummyPageHolder.h"
#include <gtest/gtest.h>

namespace blink {

class EventPathTest : public ::testing::Test {
protected:
    Document& document() const { return m_dummyPageHolder->document(); }

private:
    void SetUp() override;

    OwnPtr<DummyPageHolder> m_dummyPageHolder;
};

void EventPathTest::SetUp()
{
    m_dummyPageHolder = DummyPageHolder::create(IntSize(800, 600));
}

TEST_F(EventPathTest, ShouldBeEmptyForPseudoElementWithoutParentElement)
{
    RefPtrWillBeRawPtr<Element> div = document().createElement(HTMLNames::divTag, false);
    RefPtrWillBeRawPtr<PseudoElement> pseudo = PseudoElement::create(div.get(), FIRST_LETTER);
    pseudo->dispose();
    EventPath eventPath(*pseudo);
    EXPECT_TRUE(eventPath.isEmpty());
}

} // namespace blink
