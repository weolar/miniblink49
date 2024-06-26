// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/base/rtree.h"

#include "testing/gtest/include/gtest/gtest.h"

namespace cc {

TEST(RTreeTest, NoOverlap)
{
    std::vector<gfx::RectF> rects;
    for (int y = 0; y < 50; ++y) {
        for (int x = 0; x < 50; ++x) {
            rects.push_back(gfx::RectF(x, y, 1.f, 1.f));
        }
    }

    RTree rtree;
    rtree.Build(rects);

    std::vector<size_t> results;
    rtree.Search(gfx::RectF(0.f, 0.f, 50.f, 50.f), &results);
    ASSERT_EQ(2500u, results.size());
    for (size_t i = 0; i < 2500; ++i) {
        ASSERT_EQ(results[i], i);
    }

    results.clear();
    rtree.Search(gfx::RectF(0.f, 0.f, 50.f, 49.f), &results);
    ASSERT_EQ(2450u, results.size());
    for (size_t i = 0; i < 2450; ++i) {
        ASSERT_EQ(results[i], i);
    }

    results.clear();
    rtree.Search(gfx::RectF(5.2f, 6.3f, 0.1f, 0.2f), &results);
    ASSERT_EQ(1u, results.size());
    EXPECT_EQ(6u * 50 + 5u, results[0]);
}

TEST(RTreeTest, Overlap)
{
    std::vector<gfx::RectF> rects;
    for (int h = 1; h <= 50; ++h) {
        for (int w = 1; w <= 50; ++w) {
            rects.push_back(gfx::RectF(0, 0, w, h));
        }
    }

    RTree rtree;
    rtree.Build(rects);

    std::vector<size_t> results;
    rtree.Search(gfx::RectF(0.f, 0.f, 1.f, 1.f), &results);
    ASSERT_EQ(2500u, results.size());
    for (size_t i = 0; i < 2500; ++i) {
        ASSERT_EQ(results[i], i);
    }

    results.clear();
    rtree.Search(gfx::RectF(0.f, 49.f, 1.f, 1.f), &results);
    ASSERT_EQ(50u, results.size());
    for (size_t i = 0; i < 50; ++i) {
        EXPECT_EQ(results[i], 2450u + i);
    }
}

} // namespace cc
