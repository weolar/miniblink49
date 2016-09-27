// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "wtf/ASCIICType.h"

#include <gtest/gtest.h>

namespace WTF {

TEST(ASCIICTypeTest, ASCIICaseFoldTable)
{
    LChar symbol = 0xff;
    while (symbol--) {
        EXPECT_EQ(toASCIILower<LChar>(symbol), ASCIICaseFoldTable[symbol]);
    }
}

}
