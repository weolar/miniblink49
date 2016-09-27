// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "public/platform/WebVector.h"

#include "wtf/Vector.h"
#include <gtest/gtest.h>

namespace blink {

TEST(WebVectorTest, beginend)
{
    Vector<int> input;
    for (int i = 0; i < 5; ++i)
        input.append(i);

    WebVector<int> webVector(input);
    ASSERT_EQ(input.size(), webVector.size());

    EXPECT_EQ(webVector.data(), webVector.begin());
    EXPECT_EQ(webVector.data() + webVector.size(), webVector.end());

    // Use begin()/end() iterators directly.
    Vector<int> output;
    for (WebVector<int>::iterator it = webVector.begin(); it != webVector.end(); ++it)
        output.append(*it);
    ASSERT_EQ(input.size(), output.size());
    for (size_t i = 0; i < input.size(); ++i)
        EXPECT_EQ(input[i], output[i]);

    // Use begin()/end() const_iterators directly.
    output.clear();
    const WebVector<int>& constWebVector = webVector;
    for (WebVector<int>::const_iterator it = constWebVector.begin(); it != constWebVector.end(); ++it)
        output.append(*it);
    ASSERT_EQ(input.size(), output.size());
    for (size_t i = 0; i < input.size(); ++i)
        EXPECT_EQ(input[i], output[i]);

    // Use range-based for loop.
    output.clear();
    for (int x : webVector)
        output.append(x);
    ASSERT_EQ(input.size(), output.size());
    for (size_t i = 0; i < input.size(); ++i)
        EXPECT_EQ(input[i], output[i]);
}

} // namespace blink
