// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/layout/LayoutMenuList.h"

#include "core/layout/LayoutTestHelper.h"
#include <gtest/gtest.h>

namespace blink {

class LayoutMenuListTest : public RenderingTest {
};

TEST_F(LayoutMenuListTest, ElementRectRelativeToViewport)
{
    setBodyInnerHTML("<select style='position:fixed; top:12.3px; height:24px; -webkit-appearance:none;'><option>o1</select>");
    Element* select = toElement(document().body()->firstChild());
    ASSERT(select);
    LayoutMenuList* obj = toLayoutMenuList(select->layoutObject());
    ASSERT(obj);
    IntRect bounds = obj->elementRectRelativeToViewport();
    EXPECT_EQ(24, bounds.height());
}

}
