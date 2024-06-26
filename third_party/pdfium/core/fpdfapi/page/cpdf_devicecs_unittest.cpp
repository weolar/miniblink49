// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/page/cpdf_devicecs.h"
#include "testing/gtest/include/gtest/gtest.h"

TEST(CPDF_DeviceCSTest, GetRGBFromGray) {
  float R;
  float G;
  float B;
  CPDF_DeviceCS device_gray(PDFCS_DEVICEGRAY);

  // Test normal values. For gray, only first value from buf should be used.
  float buf[3] = {0.43f, 0.11f, 0.34f};
  ASSERT_TRUE(device_gray.GetRGB(buf, &R, &G, &B));
  EXPECT_FLOAT_EQ(0.43f, R);
  EXPECT_FLOAT_EQ(0.43f, G);
  EXPECT_FLOAT_EQ(0.43f, B);
  buf[0] = 0.872f;
  ASSERT_TRUE(device_gray.GetRGB(buf, &R, &G, &B));
  EXPECT_FLOAT_EQ(0.872f, R);
  EXPECT_FLOAT_EQ(0.872f, G);
  EXPECT_FLOAT_EQ(0.872f, B);

  // Test boundary values
  buf[0] = {0.0f};
  ASSERT_TRUE(device_gray.GetRGB(buf, &R, &G, &B));
  EXPECT_FLOAT_EQ(0.0f, R);
  EXPECT_FLOAT_EQ(0.0f, G);
  EXPECT_FLOAT_EQ(0.0f, B);
  buf[0] = 1.0f;
  ASSERT_TRUE(device_gray.GetRGB(buf, &R, &G, &B));
  EXPECT_FLOAT_EQ(1.0f, R);
  EXPECT_FLOAT_EQ(1.0f, G);
  EXPECT_FLOAT_EQ(1.0f, B);

  // Test out of range values
  buf[0] = -0.01f;
  ASSERT_TRUE(device_gray.GetRGB(buf, &R, &G, &B));
  EXPECT_FLOAT_EQ(0.0f, R);
  EXPECT_FLOAT_EQ(0.0f, G);
  EXPECT_FLOAT_EQ(0.0f, B);
  buf[0] = 12.5f;
  ASSERT_TRUE(device_gray.GetRGB(buf, &R, &G, &B));
  EXPECT_FLOAT_EQ(1.0f, R);
  EXPECT_FLOAT_EQ(1.0f, G);
  EXPECT_FLOAT_EQ(1.0f, B);
}

TEST(CPDF_DeviceCSTest, GetRGBFromRGB) {
  float R;
  float G;
  float B;
  CPDF_DeviceCS device_rgb(PDFCS_DEVICERGB);

  // Test normal values
  float buf[3] = {0.13f, 1.0f, 0.652f};
  ASSERT_TRUE(device_rgb.GetRGB(buf, &R, &G, &B));
  EXPECT_FLOAT_EQ(0.13f, R);
  EXPECT_FLOAT_EQ(1.0f, G);
  EXPECT_FLOAT_EQ(0.652f, B);
  buf[0] = 0.0f;
  buf[1] = 0.52f;
  buf[2] = 0.78f;
  ASSERT_TRUE(device_rgb.GetRGB(buf, &R, &G, &B));
  EXPECT_FLOAT_EQ(0.0f, R);
  EXPECT_FLOAT_EQ(0.52f, G);
  EXPECT_FLOAT_EQ(0.78f, B);

  // Test out of range values
  buf[0] = -10.5f;
  buf[1] = 100.0f;
  ASSERT_TRUE(device_rgb.GetRGB(buf, &R, &G, &B));
  EXPECT_FLOAT_EQ(0.0f, R);
  EXPECT_FLOAT_EQ(1.0f, G);
  EXPECT_FLOAT_EQ(0.78f, B);
}

TEST(CPDF_DeviceCSTest, GetRGBFromCMYK) {
  float R;
  float G;
  float B;
  CPDF_DeviceCS device_cmyk(PDFCS_DEVICECMYK);

  // Test normal values
  float buf[4] = {0.6f, 0.5f, 0.3f, 0.9f};
  ASSERT_TRUE(device_cmyk.GetRGB(buf, &R, &G, &B));
  EXPECT_FLOAT_EQ(0.0627451f, R);
  EXPECT_FLOAT_EQ(0.0627451f, G);
  EXPECT_FLOAT_EQ(0.10588236f, B);
  buf[0] = 0.15f;
  buf[2] = 0.0f;
  ASSERT_TRUE(device_cmyk.GetRGB(buf, &R, &G, &B));
  EXPECT_FLOAT_EQ(0.2f, R);
  EXPECT_FLOAT_EQ(0.0862745f, G);
  EXPECT_FLOAT_EQ(0.16470589f, B);
  buf[2] = 1.0f;
  buf[3] = 0.0f;
  ASSERT_TRUE(device_cmyk.GetRGB(buf, &R, &G, &B));
  EXPECT_FLOAT_EQ(0.85098046f, R);
  EXPECT_FLOAT_EQ(0.552941f, G);
  EXPECT_FLOAT_EQ(0.15686275f, B);

  // Test out of range values
  buf[2] = 1.5f;
  buf[3] = -0.6f;
  ASSERT_TRUE(device_cmyk.GetRGB(buf, &R, &G, &B));
  EXPECT_FLOAT_EQ(0.85098046f, R);
  EXPECT_FLOAT_EQ(0.552941f, G);
  EXPECT_FLOAT_EQ(0.15686275f, B);
}
