// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/layout/LayoutInline.h"

#include "core/layout/LayoutTestHelper.h"
#include <gtest/gtest.h>

namespace blink {

class LayoutInlineTest : public RenderingTest {
};

TEST_F(LayoutInlineTest, LayoutNameCalledWithNullStyle)
{
    LayoutObject* obj = LayoutInline::createAnonymous(&document());
    EXPECT_FALSE(obj->style());
    EXPECT_STREQ("LayoutInline (anonymous)", obj->decoratedName().ascii().data());
    obj->destroy();
}

}
