/*
 * Copyright (c) 2013, Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/html/TimeRanges.h"

#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include <gtest/gtest.h>
#include <sstream>

namespace blink {

static std::string ToString(const TimeRanges& ranges)
{
    std::stringstream ss;
    ss << "{";
    for (unsigned i = 0; i < ranges.length(); ++i)
        ss << " [" << ranges.start(i, IGNORE_EXCEPTION) << "," << ranges.end(i, IGNORE_EXCEPTION) << ")";
    ss << " }";

    return ss.str();
}

#define ASSERT_RANGE(expected, range) ASSERT_EQ(expected, ToString(*range))

TEST(TimeRangesTest, Empty)
{
    ASSERT_RANGE("{ }", TimeRanges::create());
}

TEST(TimeRangesTest, SingleRange)
{
    ASSERT_RANGE("{ [1,2) }", TimeRanges::create(1, 2));
}

TEST(TimeRangesTest, CreateFromWebTimeRanges)
{
    blink::WebTimeRanges webRanges(static_cast<size_t>(2));
    webRanges[0].start = 0;
    webRanges[0].end = 1;
    webRanges[1].start = 2;
    webRanges[1].end = 3;
    ASSERT_RANGE("{ [0,1) [2,3) }", TimeRanges::create(webRanges));
}

TEST(TimeRangesTest, AddOrder)
{
    RefPtrWillBeRawPtr<TimeRanges> rangeA = TimeRanges::create();
    RefPtrWillBeRawPtr<TimeRanges> rangeB = TimeRanges::create();

    rangeA->add(0, 2);
    rangeA->add(3, 4);
    rangeA->add(5, 100);

    std::string expected = "{ [0,2) [3,4) [5,100) }";
    ASSERT_RANGE(expected, rangeA);

    // Add the values in rangeA to rangeB in reverse order.
    for (int i = rangeA->length() - 1; i >= 0; --i)
        rangeB->add(rangeA->start(i, IGNORE_EXCEPTION), rangeA->end(i, IGNORE_EXCEPTION));

    ASSERT_RANGE(expected, rangeB);
}

TEST(TimeRangesTest, OverlappingAdds)
{
    RefPtrWillBeRawPtr<TimeRanges> ranges = TimeRanges::create();

    ranges->add(0, 2);
    ranges->add(10, 11);
    ASSERT_RANGE("{ [0,2) [10,11) }", ranges);

    ranges->add(0, 2);
    ASSERT_RANGE("{ [0,2) [10,11) }", ranges);

    ranges->add(2, 3);
    ASSERT_RANGE("{ [0,3) [10,11) }", ranges);

    ranges->add(2, 6);
    ASSERT_RANGE("{ [0,6) [10,11) }", ranges);

    ranges->add(9, 10);
    ASSERT_RANGE("{ [0,6) [9,11) }", ranges);

    ranges->add(8, 10);
    ASSERT_RANGE("{ [0,6) [8,11) }", ranges);

    ranges->add(-1, 7);
    ASSERT_RANGE("{ [-1,7) [8,11) }", ranges);

    ranges->add(6, 9);
    ASSERT_RANGE("{ [-1,11) }", ranges);
}

TEST(TimeRangesTest, IntersectWith_Self)
{
    RefPtrWillBeRawPtr<TimeRanges> ranges = TimeRanges::create(0, 2);

    ASSERT_RANGE("{ [0,2) }", ranges);

    ranges->intersectWith(ranges.get());

    ASSERT_RANGE("{ [0,2) }", ranges);
}

TEST(TimeRangesTest, IntersectWith_IdenticalRange)
{
    RefPtrWillBeRawPtr<TimeRanges> rangesA = TimeRanges::create(0, 2);
    RefPtrWillBeRawPtr<TimeRanges> rangesB = rangesA->copy();

    ASSERT_RANGE("{ [0,2) }", rangesA);
    ASSERT_RANGE("{ [0,2) }", rangesB);

    rangesA->intersectWith(rangesB.get());

    ASSERT_RANGE("{ [0,2) }", rangesA);
    ASSERT_RANGE("{ [0,2) }", rangesB);
}

TEST(TimeRangesTest, IntersectWith_Empty)
{
    RefPtrWillBeRawPtr<TimeRanges> rangesA = TimeRanges::create(0, 2);
    RefPtrWillBeRawPtr<TimeRanges> rangesB = TimeRanges::create();

    ASSERT_RANGE("{ [0,2) }", rangesA);
    ASSERT_RANGE("{ }", rangesB);

    rangesA->intersectWith(rangesB.get());

    ASSERT_RANGE("{ }", rangesA);
    ASSERT_RANGE("{ }", rangesB);
}

TEST(TimeRangesTest, IntersectWith_DisjointRanges1)
{
    RefPtrWillBeRawPtr<TimeRanges> rangesA = TimeRanges::create();
    RefPtrWillBeRawPtr<TimeRanges> rangesB = TimeRanges::create();

    rangesA->add(0, 1);
    rangesA->add(4, 5);

    rangesB->add(2, 3);
    rangesB->add(6, 7);

    ASSERT_RANGE("{ [0,1) [4,5) }", rangesA);
    ASSERT_RANGE("{ [2,3) [6,7) }", rangesB);

    rangesA->intersectWith(rangesB.get());

    ASSERT_RANGE("{ }", rangesA);
    ASSERT_RANGE("{ [2,3) [6,7) }", rangesB);
}

TEST(TimeRangesTest, IntersectWith_DisjointRanges2)
{
    RefPtrWillBeRawPtr<TimeRanges> rangesA = TimeRanges::create();
    RefPtrWillBeRawPtr<TimeRanges> rangesB = TimeRanges::create();

    rangesA->add(0, 1);
    rangesA->add(4, 5);

    rangesB->add(1, 4);
    rangesB->add(5, 7);

    ASSERT_RANGE("{ [0,1) [4,5) }", rangesA);
    ASSERT_RANGE("{ [1,4) [5,7) }", rangesB);

    rangesA->intersectWith(rangesB.get());

    ASSERT_RANGE("{ }", rangesA);
    ASSERT_RANGE("{ [1,4) [5,7) }", rangesB);
}

TEST(TimeRangesTest, IntersectWith_CompleteOverlap1)
{
    RefPtrWillBeRawPtr<TimeRanges> rangesA = TimeRanges::create();
    RefPtrWillBeRawPtr<TimeRanges> rangesB = TimeRanges::create();

    rangesA->add(1, 3);
    rangesA->add(4, 5);
    rangesA->add(6, 9);

    rangesB->add(0, 10);

    ASSERT_RANGE("{ [1,3) [4,5) [6,9) }", rangesA);
    ASSERT_RANGE("{ [0,10) }", rangesB);

    rangesA->intersectWith(rangesB.get());

    ASSERT_RANGE("{ [1,3) [4,5) [6,9) }", rangesA);
    ASSERT_RANGE("{ [0,10) }", rangesB);
}

TEST(TimeRangesTest, IntersectWith_CompleteOverlap2)
{
    RefPtrWillBeRawPtr<TimeRanges> rangesA = TimeRanges::create();
    RefPtrWillBeRawPtr<TimeRanges> rangesB = TimeRanges::create();

    rangesA->add(1, 3);
    rangesA->add(4, 5);
    rangesA->add(6, 9);

    rangesB->add(1, 9);

    ASSERT_RANGE("{ [1,3) [4,5) [6,9) }", rangesA);
    ASSERT_RANGE("{ [1,9) }", rangesB);

    rangesA->intersectWith(rangesB.get());

    ASSERT_RANGE("{ [1,3) [4,5) [6,9) }", rangesA);
    ASSERT_RANGE("{ [1,9) }", rangesB);
}

TEST(TimeRangesTest, IntersectWith_Gaps1)
{
    RefPtrWillBeRawPtr<TimeRanges> rangesA = TimeRanges::create();
    RefPtrWillBeRawPtr<TimeRanges> rangesB = TimeRanges::create();

    rangesA->add(0, 2);
    rangesA->add(4, 6);

    rangesB->add(1, 5);

    ASSERT_RANGE("{ [0,2) [4,6) }", rangesA);
    ASSERT_RANGE("{ [1,5) }", rangesB);

    rangesA->intersectWith(rangesB.get());

    ASSERT_RANGE("{ [1,2) [4,5) }", rangesA);
    ASSERT_RANGE("{ [1,5) }", rangesB);
}

TEST(TimeRangesTest, IntersectWith_Gaps2)
{
    RefPtrWillBeRawPtr<TimeRanges> rangesA = TimeRanges::create();
    RefPtrWillBeRawPtr<TimeRanges> rangesB = TimeRanges::create();

    rangesA->add(0, 2);
    rangesA->add(4, 6);
    rangesA->add(8, 10);

    rangesB->add(1, 9);

    ASSERT_RANGE("{ [0,2) [4,6) [8,10) }", rangesA);
    ASSERT_RANGE("{ [1,9) }", rangesB);

    rangesA->intersectWith(rangesB.get());

    ASSERT_RANGE("{ [1,2) [4,6) [8,9) }", rangesA);
    ASSERT_RANGE("{ [1,9) }", rangesB);
}

TEST(TimeRangesTest, IntersectWith_Gaps3)
{
    RefPtrWillBeRawPtr<TimeRanges> rangesA = TimeRanges::create();
    RefPtrWillBeRawPtr<TimeRanges> rangesB = TimeRanges::create();

    rangesA->add(0, 2);
    rangesA->add(4, 7);
    rangesA->add(8, 10);

    rangesB->add(1, 5);
    rangesB->add(6, 9);

    ASSERT_RANGE("{ [0,2) [4,7) [8,10) }", rangesA);
    ASSERT_RANGE("{ [1,5) [6,9) }", rangesB);

    rangesA->intersectWith(rangesB.get());

    ASSERT_RANGE("{ [1,2) [4,5) [6,7) [8,9) }", rangesA);
    ASSERT_RANGE("{ [1,5) [6,9) }", rangesB);
}

TEST(TimeRangesTest, Nearest)
{
    RefPtrWillBeRawPtr<TimeRanges> ranges = TimeRanges::create();
    ranges->add(0, 2);
    ranges->add(5, 7);

    ASSERT_EQ(0, ranges->nearest(0, 0));
    ASSERT_EQ(1, ranges->nearest(1, 0));
    ASSERT_EQ(2, ranges->nearest(2, 0));
    ASSERT_EQ(2, ranges->nearest(3, 0));
    ASSERT_EQ(5, ranges->nearest(4, 0));
    ASSERT_EQ(5, ranges->nearest(5, 0));
    ASSERT_EQ(7, ranges->nearest(8, 0));

    ranges->add(9, 11);
    ASSERT_EQ(7, ranges->nearest(8, 6));
    ASSERT_EQ(7, ranges->nearest(8, 8));
    ASSERT_EQ(9, ranges->nearest(8, 10));
}

} // namespace blink
