// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/html/parser/HTMLParserThread.h"

#include <gtest/gtest.h>

namespace blink {

TEST(HTMLParserThreadTest, Init)
{
    // The harness has already run init() for us, so tear down the parser first.
    ASSERT_TRUE(HTMLParserThread::shared());
    HTMLParserThread::shutdown();

    // Make sure starting the parser thread brings it back to life.
    ASSERT_FALSE(HTMLParserThread::shared());
    HTMLParserThread::init();
    ASSERT_TRUE(HTMLParserThread::shared());
}

} // namespace blink
