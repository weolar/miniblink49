// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/layout/LayoutBlock.h"

#include "core/layout/LayoutBlockFlow.h"
#include "core/layout/LayoutTestHelper.h"
#include <gtest/gtest.h>

namespace blink {

class LayoutBlockTest : public RenderingTest {
};

TEST_F(LayoutBlockTest, LayoutNameCalledWithNullStyle)
{
    LayoutObject* obj = LayoutBlockFlow::createAnonymous(&document());
    EXPECT_FALSE(obj->style());
    EXPECT_STREQ("LayoutBlockFlow (anonymous)", obj->decoratedName().ascii().data());
    obj->destroy();
}

}
