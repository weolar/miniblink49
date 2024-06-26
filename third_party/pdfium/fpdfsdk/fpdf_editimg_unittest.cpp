// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "public/fpdf_edit.h"

#include "core/fpdfapi/cpdf_modulemgr.h"
#include "public/cpp/fpdf_scopers.h"
#include "testing/gtest/include/gtest/gtest.h"

class PDFEditImgTest : public testing::Test {
  void SetUp() override { CPDF_ModuleMgr::Get()->Init(); }

  void TearDown() override { CPDF_ModuleMgr::Destroy(); }
};

TEST_F(PDFEditImgTest, InsertObjectWithInvalidPage) {
  FPDF_DOCUMENT doc = FPDF_CreateNewDocument();
  FPDF_PAGE page = FPDFPage_New(doc, 0, 100, 100);
  EXPECT_EQ(0, FPDFPage_CountObjects(page));

  FPDFPage_InsertObject(nullptr, nullptr);
  EXPECT_EQ(0, FPDFPage_CountObjects(page));

  FPDFPage_InsertObject(page, nullptr);
  EXPECT_EQ(0, FPDFPage_CountObjects(page));

  FPDF_PAGEOBJECT page_image = FPDFPageObj_NewImageObj(doc);
  FPDFPage_InsertObject(nullptr, page_image);
  EXPECT_EQ(0, FPDFPage_CountObjects(page));

  FPDF_ClosePage(page);
  FPDF_CloseDocument(doc);
}

TEST_F(PDFEditImgTest, NewImageObj) {
  FPDF_DOCUMENT doc = FPDF_CreateNewDocument();
  FPDF_PAGE page = FPDFPage_New(doc, 0, 100, 100);
  EXPECT_EQ(0, FPDFPage_CountObjects(page));

  FPDF_PAGEOBJECT page_image = FPDFPageObj_NewImageObj(doc);
  FPDFPage_InsertObject(page, page_image);
  EXPECT_EQ(1, FPDFPage_CountObjects(page));
  EXPECT_TRUE(FPDFPage_GenerateContent(page));

  FPDF_ClosePage(page);
  FPDF_CloseDocument(doc);
}

TEST_F(PDFEditImgTest, NewImageObjGenerateContent) {
  FPDF_DOCUMENT doc = FPDF_CreateNewDocument();
  FPDF_PAGE page = FPDFPage_New(doc, 0, 100, 100);
  EXPECT_EQ(0, FPDFPage_CountObjects(page));

  constexpr int kBitmapSize = 50;
  FPDF_BITMAP bitmap = FPDFBitmap_Create(kBitmapSize, kBitmapSize, 0);
  FPDFBitmap_FillRect(bitmap, 0, 0, kBitmapSize, kBitmapSize, 0x00000000);
  EXPECT_EQ(kBitmapSize, FPDFBitmap_GetWidth(bitmap));
  EXPECT_EQ(kBitmapSize, FPDFBitmap_GetHeight(bitmap));

  FPDF_PAGEOBJECT page_image = FPDFPageObj_NewImageObj(doc);
  ASSERT_TRUE(FPDFImageObj_SetBitmap(&page, 0, page_image, bitmap));
  ASSERT_TRUE(
      FPDFImageObj_SetMatrix(page_image, kBitmapSize, 0, 0, kBitmapSize, 0, 0));
  FPDFPage_InsertObject(page, page_image);
  EXPECT_EQ(1, FPDFPage_CountObjects(page));
  EXPECT_TRUE(FPDFPage_GenerateContent(page));

  FPDFBitmap_Destroy(bitmap);
  FPDF_ClosePage(page);
  FPDF_CloseDocument(doc);
}

TEST_F(PDFEditImgTest, SetBitmap) {
  ScopedFPDFDocument doc(FPDF_CreateNewDocument());
  ScopedFPDFPage page(FPDFPage_New(doc.get(), 0, 100, 100));
  ScopedFPDFPageObject image(FPDFPageObj_NewImageObj(doc.get()));
  ScopedFPDFBitmap bitmap(FPDFBitmap_Create(100, 100, 0));

  FPDF_PAGE page_ptr = page.get();
  FPDF_PAGE* pages = &page_ptr;
  EXPECT_TRUE(FPDFImageObj_SetBitmap(nullptr, 1, image.get(), bitmap.get()));
  EXPECT_TRUE(FPDFImageObj_SetBitmap(pages, 0, image.get(), bitmap.get()));
  EXPECT_FALSE(FPDFImageObj_SetBitmap(pages, 1, nullptr, bitmap.get()));
  EXPECT_FALSE(FPDFImageObj_SetBitmap(pages, 1, image.get(), nullptr));
}

TEST_F(PDFEditImgTest, GetSetImageMatrix) {
  FPDF_DOCUMENT doc = FPDF_CreateNewDocument();
  FPDF_PAGEOBJECT image = FPDFPageObj_NewImageObj(doc);

  double a;
  double b;
  double c;
  double d;
  double e;
  double f;
  EXPECT_FALSE(FPDFImageObj_GetMatrix(nullptr, nullptr, nullptr, nullptr,
                                      nullptr, nullptr, nullptr));
  EXPECT_FALSE(FPDFImageObj_GetMatrix(nullptr, &a, nullptr, nullptr, nullptr,
                                      nullptr, nullptr));
  EXPECT_FALSE(FPDFImageObj_GetMatrix(nullptr, &a, &b, nullptr, nullptr,
                                      nullptr, nullptr));
  EXPECT_FALSE(
      FPDFImageObj_GetMatrix(nullptr, &a, &b, &c, nullptr, nullptr, nullptr));
  EXPECT_FALSE(
      FPDFImageObj_GetMatrix(nullptr, &a, &b, &c, nullptr, nullptr, nullptr));
  EXPECT_FALSE(
      FPDFImageObj_GetMatrix(nullptr, &a, &b, &c, &d, nullptr, nullptr));
  EXPECT_FALSE(FPDFImageObj_GetMatrix(nullptr, &a, &b, &c, &d, &e, nullptr));
  EXPECT_FALSE(FPDFImageObj_GetMatrix(nullptr, &a, &b, &c, &d, &e, &f));
  EXPECT_FALSE(FPDFImageObj_GetMatrix(nullptr, &a, nullptr, &c, &d, &e, &f));

  EXPECT_FALSE(FPDFImageObj_GetMatrix(image, nullptr, nullptr, nullptr, nullptr,
                                      nullptr, nullptr));
  EXPECT_FALSE(FPDFImageObj_GetMatrix(image, &a, nullptr, nullptr, nullptr,
                                      nullptr, nullptr));
  EXPECT_FALSE(FPDFImageObj_GetMatrix(image, &a, &b, nullptr, nullptr, nullptr,
                                      nullptr));
  EXPECT_FALSE(
      FPDFImageObj_GetMatrix(image, &a, &b, &c, nullptr, nullptr, nullptr));
  EXPECT_FALSE(
      FPDFImageObj_GetMatrix(image, &a, &b, &c, nullptr, nullptr, nullptr));
  EXPECT_FALSE(FPDFImageObj_GetMatrix(image, &a, &b, &c, &d, nullptr, nullptr));
  EXPECT_FALSE(FPDFImageObj_GetMatrix(image, &a, &b, &c, &d, &e, nullptr));
  EXPECT_FALSE(FPDFImageObj_GetMatrix(image, &a, nullptr, &c, &d, &e, &f));

  EXPECT_TRUE(FPDFImageObj_GetMatrix(image, &a, &b, &c, &d, &e, &f));
  EXPECT_DOUBLE_EQ(1.0, a);
  EXPECT_DOUBLE_EQ(0.0, b);
  EXPECT_DOUBLE_EQ(0.0, c);
  EXPECT_DOUBLE_EQ(1.0, d);
  EXPECT_DOUBLE_EQ(0.0, e);
  EXPECT_DOUBLE_EQ(0.0, f);

  EXPECT_TRUE(FPDFImageObj_SetMatrix(image, 1, 2, 3, 4, 5, 6));
  EXPECT_TRUE(FPDFImageObj_GetMatrix(image, &a, &b, &c, &d, &e, &f));
  EXPECT_DOUBLE_EQ(1.0, a);
  EXPECT_DOUBLE_EQ(2.0, b);
  EXPECT_DOUBLE_EQ(3.0, c);
  EXPECT_DOUBLE_EQ(4.0, d);
  EXPECT_DOUBLE_EQ(5.0, e);
  EXPECT_DOUBLE_EQ(6.0, f);

  FPDFPageObj_Destroy(image);
  FPDF_CloseDocument(doc);
}
