// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "public/fpdf_edit.h"

#include "core/fpdfapi/cpdf_modulemgr.h"
#include "testing/gtest/include/gtest/gtest.h"

class PDFEditTest : public testing::Test {
  void SetUp() override { CPDF_ModuleMgr::Get()->Init(); }

  void TearDown() override { CPDF_ModuleMgr::Destroy(); }
};

TEST_F(PDFEditTest, LineJoin) {
  EXPECT_FALSE(FPDFPageObj_SetLineJoin(nullptr, -1));
  EXPECT_FALSE(FPDFPageObj_SetLineJoin(nullptr, FPDF_LINEJOIN_MITER));
  EXPECT_FALSE(FPDFPageObj_SetLineJoin(nullptr, FPDF_LINEJOIN_ROUND));
  EXPECT_FALSE(FPDFPageObj_SetLineJoin(nullptr, FPDF_LINEJOIN_BEVEL));
  EXPECT_FALSE(FPDFPageObj_SetLineJoin(nullptr, 3));
  EXPECT_FALSE(FPDFPageObj_SetLineJoin(nullptr, 1000));

  FPDF_PAGEOBJECT path = FPDFPageObj_CreateNewPath(0, 0);
  EXPECT_EQ(FPDF_LINEJOIN_MITER, FPDFPageObj_GetLineJoin(path));

  EXPECT_FALSE(FPDFPageObj_SetLineJoin(path, -1));
  EXPECT_FALSE(FPDFPageObj_SetLineJoin(path, 3));
  EXPECT_FALSE(FPDFPageObj_SetLineJoin(path, 1000));

  EXPECT_TRUE(FPDFPageObj_SetLineJoin(path, FPDF_LINEJOIN_BEVEL));
  EXPECT_EQ(FPDF_LINEJOIN_BEVEL, FPDFPageObj_GetLineJoin(path));

  EXPECT_TRUE(FPDFPageObj_SetLineJoin(path, FPDF_LINEJOIN_ROUND));
  EXPECT_EQ(FPDF_LINEJOIN_ROUND, FPDFPageObj_GetLineJoin(path));

  EXPECT_TRUE(FPDFPageObj_SetLineJoin(path, FPDF_LINEJOIN_MITER));
  EXPECT_EQ(FPDF_LINEJOIN_MITER, FPDFPageObj_GetLineJoin(path));

  FPDFPageObj_Destroy(path);
}

TEST_F(PDFEditTest, LineCap) {
  EXPECT_FALSE(FPDFPageObj_SetLineCap(nullptr, -1));
  EXPECT_FALSE(FPDFPageObj_SetLineCap(nullptr, FPDF_LINECAP_BUTT));
  EXPECT_FALSE(FPDFPageObj_SetLineCap(nullptr, FPDF_LINECAP_ROUND));
  EXPECT_FALSE(FPDFPageObj_SetLineCap(nullptr, FPDF_LINECAP_PROJECTING_SQUARE));
  EXPECT_FALSE(FPDFPageObj_SetLineCap(nullptr, 3));
  EXPECT_FALSE(FPDFPageObj_SetLineCap(nullptr, 1000));

  FPDF_PAGEOBJECT path = FPDFPageObj_CreateNewPath(0, 0);
  EXPECT_EQ(FPDF_LINECAP_BUTT, FPDFPageObj_GetLineCap(path));

  EXPECT_FALSE(FPDFPageObj_SetLineCap(path, -1));
  EXPECT_FALSE(FPDFPageObj_SetLineCap(path, 3));
  EXPECT_FALSE(FPDFPageObj_SetLineCap(path, 1000));

  EXPECT_TRUE(FPDFPageObj_SetLineCap(path, FPDF_LINECAP_PROJECTING_SQUARE));
  EXPECT_EQ(FPDF_LINECAP_PROJECTING_SQUARE, FPDFPageObj_GetLineCap(path));

  EXPECT_TRUE(FPDFPageObj_SetLineCap(path, FPDF_LINECAP_ROUND));
  EXPECT_EQ(FPDF_LINECAP_ROUND, FPDFPageObj_GetLineCap(path));

  EXPECT_TRUE(FPDFPageObj_SetLineCap(path, FPDF_LINECAP_BUTT));
  EXPECT_EQ(FPDF_LINECAP_BUTT, FPDFPageObj_GetLineCap(path));

  FPDFPageObj_Destroy(path);
}
