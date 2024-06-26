// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/image/image_util.h"

#include <vector>

#include "base/memory/scoped_ptr.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "third_party/skia/include/core/SkRect.h"
#include "ui/gfx/image/image_skia.h"
#include "ui/gfx/image/image_unittest_util.h"

TEST(ImageUtilTest, JPEGEncodeAndDecode)
{
    gfx::Image original = gfx::test::CreateImage(100, 100);

    std::vector<unsigned char> encoded;
    ASSERT_TRUE(gfx::JPEG1xEncodedDataFromImage(original, 80, &encoded));

    gfx::Image decoded = gfx::ImageFrom1xJPEGEncodedData(&encoded.front(), encoded.size());

    // JPEG is lossy, so simply check that the image decoded successfully.
    EXPECT_FALSE(decoded.IsEmpty());
}

TEST(ImageUtilTest, TestVisibleMargins)
{
    // Image with non-transparent piece should return margins at those
    // columns.
    SkBitmap bitmap1;
    bitmap1.allocN32Pixels(16, 16);
    bitmap1.eraseColor(SK_ColorTRANSPARENT);
    bitmap1.eraseArea(SkIRect::MakeLTRB(3, 3, 14, 14), SK_ColorYELLOW);
    gfx::ImageSkia img = gfx::ImageSkia::CreateFrom1xBitmap(bitmap1);
    int x = 0;
    int y = 0;
    gfx::VisibleMargins(img, &x, &y);
    EXPECT_EQ(3, x);
    EXPECT_EQ(13, y);
    EXPECT_EQ(16, img.width());

    // Full-width-transparent image should return margins in the center
    // of the image.
    SkBitmap bitmap2;
    bitmap2.allocN32Pixels(16, 16);
    bitmap2.eraseColor(SK_ColorTRANSPARENT);
    gfx::ImageSkia img_transparent = gfx::ImageSkia::CreateFrom1xBitmap(bitmap2);
    x = 0;
    y = 0;
    gfx::VisibleMargins(img_transparent, &x, &y);
    EXPECT_EQ(8, x);
    EXPECT_EQ(9, y);
    EXPECT_EQ(16, img_transparent.width());

    // Image with non-transparent piece that is skewed to one side should
    // return margins at those columns.
    SkBitmap bitmap3;
    bitmap3.allocN32Pixels(16, 16);
    bitmap3.eraseColor(SK_ColorTRANSPARENT);
    bitmap3.eraseArea(SkIRect::MakeLTRB(3, 3, 5, 5), SK_ColorYELLOW);
    gfx::ImageSkia img3 = gfx::ImageSkia::CreateFrom1xBitmap(bitmap3);
    x = 0;
    y = 0;
    gfx::VisibleMargins(img3, &x, &y);
    EXPECT_EQ(3, x);
    EXPECT_EQ(4, y);
    EXPECT_EQ(16, img3.width());

    // Image with non-transparent piece that is at one edge should
    // return margins at those columns.
    SkBitmap bitmap4;
    bitmap4.allocN32Pixels(16, 16);
    bitmap4.eraseColor(SK_ColorTRANSPARENT);
    bitmap4.eraseArea(SkIRect::MakeLTRB(0, 3, 5, 5), SK_ColorYELLOW);
    gfx::ImageSkia img4 = gfx::ImageSkia::CreateFrom1xBitmap(bitmap4);
    x = 0;
    y = 0;
    gfx::VisibleMargins(img4, &x, &y);
    EXPECT_EQ(0, x);
    EXPECT_EQ(4, y);
    EXPECT_EQ(16, img4.width());

    // Image with non-transparent piece that is at trailing edge should
    // return margins at those columns.
    SkBitmap bitmap5;
    bitmap5.allocN32Pixels(16, 16);
    bitmap5.eraseColor(SK_ColorTRANSPARENT);
    bitmap5.eraseArea(SkIRect::MakeLTRB(4, 3, 16, 16), SK_ColorYELLOW);
    gfx::ImageSkia img5 = gfx::ImageSkia::CreateFrom1xBitmap(bitmap5);
    x = 0;
    y = 0;
    gfx::VisibleMargins(img5, &x, &y);
    EXPECT_EQ(4, x);
    EXPECT_EQ(15, y);
    EXPECT_EQ(16, img5.width());
}
