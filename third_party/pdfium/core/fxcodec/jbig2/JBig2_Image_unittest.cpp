// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// TODO(tsepez) this requires a lot more testing.

#include <stdint.h>

#include "core/fxcodec/jbig2/JBig2_Image.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/base/ptr_util.h"

namespace {

const int32_t kWidthPixels = 80;
const int32_t kWidthBytes = 10;
const int32_t kStrideBytes = kWidthBytes + 2;  // For testing stride != width.
const int32_t kHeightLines = 20;
const int32_t kLargerHeightLines = 100;
const int32_t kTooLargeHeightLines = 40000000;

void CheckImageEq(CJBig2_Image* img1, CJBig2_Image* img2, int line) {
  EXPECT_EQ(img1->width(), img2->width());
  EXPECT_EQ(img1->height(), img2->height());
  for (int32_t y = 0; y < img1->height(); ++y) {
    for (int32_t x = 0; x < img1->width(); ++x) {
      EXPECT_EQ(img1->GetPixel(x, y), img2->GetPixel(x, y))
          << " at " << x << " " << y << " actual line " << line;
    }
  }
}

}  // namespace

TEST(fxcodec, EmptyImage) {
  CJBig2_Image empty(0, 0);
  EXPECT_EQ(empty.width(), 0);
  EXPECT_EQ(empty.height(), 0);

  // Out-of-bounds SetPixel() is silent no-op.
  empty.SetPixel(0, 0, true);
  empty.SetPixel(1, 1, true);

  // Out-of-bounds GetPixel returns 0.
  EXPECT_EQ(empty.GetPixel(0, 0), 0);
  EXPECT_EQ(empty.GetPixel(1, 1), 0);

  // Out-of-bounds GetLine() returns null.
  EXPECT_EQ(empty.GetLine(0), nullptr);
  EXPECT_EQ(empty.GetLine(1), nullptr);
}

TEST(fxcodec, JBig2ImageCreate) {
  CJBig2_Image img(kWidthPixels, kHeightLines);
  EXPECT_EQ(kWidthPixels, img.width());
  EXPECT_EQ(kHeightLines, img.height());
  EXPECT_EQ(0, img.GetPixel(0, 0));
  EXPECT_EQ(0, img.GetLine(0)[0]);
  EXPECT_EQ(0, img.GetPixel(kWidthPixels - 1, kHeightLines - 1));
  EXPECT_EQ(0, img.GetLine(kHeightLines - 1)[kWidthBytes - 1]);

  img.SetPixel(0, 0, true);
  img.SetPixel(kWidthPixels - 1, kHeightLines - 1, true);
  EXPECT_EQ(1, img.GetPixel(0, 0));
  EXPECT_EQ(1, img.GetPixel(kWidthPixels - 1, kHeightLines - 1));
  EXPECT_EQ(0x80, img.GetLine(0)[0]);
  EXPECT_EQ(0x01, img.GetLine(kHeightLines - 1)[kWidthBytes - 1]);

  // Out-of-bounds SetPixel() is silent no-op.
  img.SetPixel(-1, 1, true);
  img.SetPixel(kWidthPixels, kHeightLines, true);

  // Out-of-bounds GetPixel returns 0.
  EXPECT_EQ(0, img.GetPixel(-1, -1));
  EXPECT_EQ(0, img.GetPixel(kWidthPixels, kHeightLines));

  // Out-of-bounds GetLine() returns null.
  EXPECT_EQ(nullptr, img.GetLine(-1));
  EXPECT_EQ(nullptr, img.GetLine(kHeightLines));
}

TEST(fxcodec, JBig2ImageCreateTooBig) {
  CJBig2_Image img(kWidthPixels, kTooLargeHeightLines);
  EXPECT_EQ(0, img.width());
  EXPECT_EQ(0, img.height());
  EXPECT_EQ(nullptr, img.data());
}

TEST(fxcodec, JBig2ImageCreateExternal) {
  uint8_t buf[kHeightLines * kStrideBytes];
  CJBig2_Image img(kWidthPixels, kHeightLines, kStrideBytes, buf);
  img.SetPixel(0, 0, true);
  img.SetPixel(kWidthPixels - 1, kHeightLines - 1, false);
  EXPECT_EQ(kWidthPixels, img.width());
  EXPECT_EQ(kHeightLines, img.height());
  EXPECT_TRUE(img.GetPixel(0, 0));
  EXPECT_FALSE(img.GetPixel(kWidthPixels - 1, kHeightLines - 1));
}

TEST(fxcodec, JBig2ImageCreateExternalTooBig) {
  uint8_t buf[kHeightLines * kStrideBytes];
  CJBig2_Image img(kWidthPixels, kTooLargeHeightLines, kStrideBytes, buf);
  EXPECT_EQ(0, img.width());
  EXPECT_EQ(0, img.height());
  EXPECT_EQ(nullptr, img.data());
}

TEST(fxcodec, JBig2ImageCreateExternalBadStride) {
  uint8_t buf[kHeightLines * kStrideBytes];
  CJBig2_Image img(kWidthPixels, kTooLargeHeightLines, kStrideBytes - 1, buf);
  EXPECT_EQ(0, img.width());
  EXPECT_EQ(0, img.height());
  EXPECT_EQ(nullptr, img.data());
}

TEST(fxcodec, JBig2ImageExpand) {
  CJBig2_Image img(kWidthPixels, kHeightLines);
  img.SetPixel(0, 0, true);
  img.SetPixel(kWidthPixels - 1, kHeightLines - 1, false);
  img.Expand(kLargerHeightLines, true);
  EXPECT_EQ(kWidthPixels, img.width());
  EXPECT_EQ(kLargerHeightLines, img.height());
  EXPECT_TRUE(img.GetPixel(0, 0));
  EXPECT_FALSE(img.GetPixel(kWidthPixels - 1, kHeightLines - 1));
  EXPECT_TRUE(img.GetPixel(kWidthPixels - 1, kLargerHeightLines - 1));
}

TEST(fxcodec, JBig2ImageExpandTooBig) {
  CJBig2_Image img(kWidthPixels, kHeightLines);
  img.SetPixel(0, 0, true);
  img.SetPixel(kWidthPixels - 1, kHeightLines - 1, false);
  img.Expand(kTooLargeHeightLines, true);
  EXPECT_EQ(kWidthPixels, img.width());
  EXPECT_EQ(kHeightLines, img.height());
  EXPECT_TRUE(img.GetPixel(0, 0));
  EXPECT_FALSE(img.GetPixel(kWidthPixels - 1, kHeightLines - 1));
}

TEST(fxcodec, JBig2ImageExpandExternal) {
  uint8_t buf[kHeightLines * kStrideBytes];
  CJBig2_Image img(kWidthPixels, kHeightLines, kStrideBytes, buf);
  img.SetPixel(0, 0, true);
  img.SetPixel(kWidthPixels - 1, kHeightLines - 1, false);
  img.Expand(kLargerHeightLines, true);
  EXPECT_EQ(kWidthPixels, img.width());
  EXPECT_EQ(kLargerHeightLines, img.height());
  EXPECT_TRUE(img.GetPixel(0, 0));
  EXPECT_FALSE(img.GetPixel(kWidthPixels - 1, kHeightLines - 1));
  EXPECT_TRUE(img.GetPixel(kWidthPixels - 1, kLargerHeightLines - 1));
}

TEST(fxcodec, JBig2ImageExpandExternalTooBig) {
  uint8_t buf[kHeightLines * kStrideBytes];
  CJBig2_Image img(kWidthPixels, kHeightLines, kStrideBytes, buf);
  img.SetPixel(0, 0, true);
  img.SetPixel(kWidthPixels - 1, kHeightLines - 1, false);
  img.Expand(kTooLargeHeightLines, true);
  EXPECT_EQ(kWidthPixels, img.width());
  EXPECT_EQ(kHeightLines, img.height());
  EXPECT_TRUE(img.GetPixel(0, 0));
  EXPECT_FALSE(img.GetPixel(kWidthPixels - 1, kHeightLines - 1));
}

TEST(fxcodec, JBig2EmptyImage) {
  auto empty = pdfium::MakeUnique<CJBig2_Image>(0, 0);

  // Empty subimage.
  auto sub1 = empty->SubImage(0, 0, 0, 0);
  EXPECT_EQ(sub1->width(), 0);
  EXPECT_EQ(sub1->height(), 0);

  // Larger dimensions are zero-padded.
  auto sub2 = empty->SubImage(0, 0, 1, 1);
  EXPECT_EQ(1, sub2->width());
  EXPECT_EQ(1, sub2->height());
  EXPECT_EQ(0, sub2->GetPixel(0, 0));

  // Bad dimensions give an empty image.
  sub2 = empty->SubImage(0, 0, -1, -1);
  EXPECT_EQ(sub2->width(), 0);
  EXPECT_EQ(sub2->height(), 0);

  // Bad offsets zero pad the image.
  auto sub3 = empty->SubImage(-1, -1, 2, 2);
  EXPECT_EQ(sub3->width(), 2);
  EXPECT_EQ(sub3->height(), 2);

  // Bad dimensions and bad offsets give an empty image.
  sub3 = empty->SubImage(-1, -1, -100, -100);
  EXPECT_EQ(sub3->width(), 0);
  EXPECT_EQ(sub3->height(), 0);
}

TEST(fxcodec, JBig2SubImage) {
  // 1-px wide rectangle in image.
  uint8_t pattern[5][8] = {
      {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
      {0x00, 0x01, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00},
      {0x00, 0x01, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00},
      {0x00, 0x01, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00},
      {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  };

  // 1-px wide rectangle in image, offset 2 in x.
  uint8_t pattern20[5][8] = {
      {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
      {0x00, 0x07, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00},
      {0x00, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00},
      {0x00, 0x07, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00},
      {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  };

  // 1-px wide rectangle in image, offset 2 in x and y, padded.
  uint8_t pattern22[5][8] = {
      {0x00, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00},
      {0x00, 0x07, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00},
      {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
      {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
      {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  };

  // 1-px wide rectangle in image, offset 16 in x, 1 in y, padded.
  uint8_t pattern161[5][8] = {
      {0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
      {0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
      {0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
      {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
      {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  };

  // Image size a nice clean power of two.
  auto img32 = pdfium::MakeUnique<CJBig2_Image>(
      32, 5, 8, reinterpret_cast<uint8_t*>(pattern));

  // Image size not a nice clean value.
  auto img37 = pdfium::MakeUnique<CJBig2_Image>(
      37, 5, 8, reinterpret_cast<uint8_t*>(pattern));

  // Expected results to check against.
  auto expected20 = pdfium::MakeUnique<CJBig2_Image>(
      30, 5, 8, reinterpret_cast<uint8_t*>(pattern20));

  auto expected22 = pdfium::MakeUnique<CJBig2_Image>(
      30, 5, 8, reinterpret_cast<uint8_t*>(pattern22));

  auto expected161 = pdfium::MakeUnique<CJBig2_Image>(
      25, 5, 8, reinterpret_cast<uint8_t*>(pattern161));

  auto expected_zeros = pdfium::MakeUnique<CJBig2_Image>(32, 5);

  // Empty subimage.
  auto sub = img32->SubImage(0, 0, 0, 0);
  EXPECT_EQ(sub->width(), 0);
  EXPECT_EQ(sub->height(), 0);

  // Full sub-image.
  sub = img32->SubImage(0, 0, 32, 5);
  EXPECT_EQ(sub->width(), 32);
  EXPECT_EQ(sub->height(), 5);
  CheckImageEq(img32.get(), sub.get(), __LINE__);

  sub = img37->SubImage(0, 0, 32, 5);
  EXPECT_EQ(sub->width(), 32);
  EXPECT_EQ(sub->height(), 5);
  CheckImageEq(img32.get(), sub.get(), __LINE__);

  // Actual bit manipulations.
  sub = img32->SubImage(2, 0, 30, 5);
  CheckImageEq(expected20.get(), sub.get(), __LINE__);

  sub = img37->SubImage(2, 2, 30, 5);
  CheckImageEq(expected22.get(), sub.get(), __LINE__);

  // Fast path.
  sub = img37->SubImage(16, 1, 25, 5);
  CheckImageEq(expected161.get(), sub.get(), __LINE__);

  // Aligned Sub-image including cruft in stride beyond width.
  sub = img37->SubImage(32, 0, 32, 5);
  CheckImageEq(expected_zeros.get(), sub.get(), __LINE__);

  // Sub-image waaaaay beyond width.
  sub = img37->SubImage(2000, 0, 32, 5);
  CheckImageEq(expected_zeros.get(), sub.get(), __LINE__);

  // Sub-image waaaaay beyond height.
  sub = img37->SubImage(0, 2000, 32, 5);
  CheckImageEq(expected_zeros.get(), sub.get(), __LINE__);

  // Sub-image with negative x offset.
  sub = img37->SubImage(-1, 0, 32, 5);
  CheckImageEq(expected_zeros.get(), sub.get(), __LINE__);

  // Sub-image with negative y offset.
  sub = img37->SubImage(0, -1, 32, 5);
  CheckImageEq(expected_zeros.get(), sub.get(), __LINE__);

  // Sub-image with negative width.
  sub = img37->SubImage(-1, 0, 32, 5);
  CheckImageEq(expected_zeros.get(), sub.get(), __LINE__);

  // Sub-image with negative height.
  sub = img37->SubImage(0, -1, 32, 5);
  CheckImageEq(expected_zeros.get(), sub.get(), __LINE__);

  // Sub-image wider than original.
  sub = img37->SubImage(0, 0, 128, 5);
  EXPECT_EQ(128, sub->width());
  EXPECT_EQ(5, sub->height());

  // Sub-image higher than original.
  sub = img37->SubImage(0, 0, 32, 40);
  EXPECT_EQ(32, sub->width());
  EXPECT_EQ(40, sub->height());
}

TEST(fxcodec, JBig2CopyLine) {
  // Horizontal line in image.
  uint8_t pattern[3][8] = {
      {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
      {0x00, 0x01, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00},
      {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  };

  uint8_t expected_pattern[3][8] = {
      {0x00, 0x01, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00},
      {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
      {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
  };

  auto img = pdfium::MakeUnique<CJBig2_Image>(
      37, 3, 8, reinterpret_cast<uint8_t*>(pattern));

  auto expected = pdfium::MakeUnique<CJBig2_Image>(
      37, 3, 8, reinterpret_cast<uint8_t*>(expected_pattern));

  // Shuffle.
  img->CopyLine(2, 1);
  img->CopyLine(1, 0);
  img->CopyLine(0, 2);

  // Clear top line via invalid |from| offset.
  img->CopyLine(2, 3);

  // Copies with invalid |to|s don't mess with things.
  img->CopyLine(-1, 0);
  img->CopyLine(4, 0);
  img->CopyLine(-1, -1);
  img->CopyLine(4, 4);

  CheckImageEq(expected.get(), img.get(), __LINE__);
}
