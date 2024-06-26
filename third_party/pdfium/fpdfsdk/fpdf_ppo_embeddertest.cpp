// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <memory>
#include <string>

#include "fpdfsdk/cpdfsdk_helpers.h"
#include "public/cpp/fpdf_scopers.h"
#include "public/fpdf_edit.h"
#include "public/fpdf_ppo.h"
#include "public/fpdf_save.h"
#include "public/fpdfview.h"
#include "testing/embedder_test.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "testing/test_support.h"

namespace {

class FPDFPPOEmbedderTest : public EmbedderTest {};

int FakeBlockWriter(FPDF_FILEWRITE* pThis,
                    const void* pData,
                    unsigned long size) {
  return size;
}

}  // namespace

TEST_F(FPDFPPOEmbedderTest, NoViewerPreferences) {
  EXPECT_TRUE(OpenDocument("hello_world.pdf"));

  FPDF_DOCUMENT output_doc = FPDF_CreateNewDocument();
  EXPECT_TRUE(output_doc);
  EXPECT_FALSE(FPDF_CopyViewerPreferences(output_doc, document()));
  FPDF_CloseDocument(output_doc);
}

TEST_F(FPDFPPOEmbedderTest, ViewerPreferences) {
  EXPECT_TRUE(OpenDocument("viewer_ref.pdf"));

  FPDF_DOCUMENT output_doc = FPDF_CreateNewDocument();
  EXPECT_TRUE(output_doc);
  EXPECT_TRUE(FPDF_CopyViewerPreferences(output_doc, document()));
  FPDF_CloseDocument(output_doc);
}

TEST_F(FPDFPPOEmbedderTest, ImportPages) {
  ASSERT_TRUE(OpenDocument("viewer_ref.pdf"));

  FPDF_PAGE page = LoadPage(0);
  EXPECT_TRUE(page);

  FPDF_DOCUMENT output_doc = FPDF_CreateNewDocument();
  ASSERT_TRUE(output_doc);
  EXPECT_TRUE(FPDF_CopyViewerPreferences(output_doc, document()));
  EXPECT_TRUE(FPDF_ImportPages(output_doc, document(), "1", 0));
  EXPECT_EQ(1, FPDF_GetPageCount(output_doc));
  FPDF_CloseDocument(output_doc);

  UnloadPage(page);
}

TEST_F(FPDFPPOEmbedderTest, ImportNPages) {
  ASSERT_TRUE(OpenDocument("rectangles_multi_pages.pdf"));

  ScopedFPDFDocument output_doc_2up(
      FPDF_ImportNPagesToOne(document(), 612, 792, 2, 1));
  ASSERT_TRUE(output_doc_2up);
  EXPECT_EQ(3, FPDF_GetPageCount(output_doc_2up.get()));
  ScopedFPDFDocument output_doc_5up(
      FPDF_ImportNPagesToOne(document(), 612, 792, 5, 1));
  ASSERT_TRUE(output_doc_5up);
  EXPECT_EQ(1, FPDF_GetPageCount(output_doc_5up.get()));
  ScopedFPDFDocument output_doc_8up(
      FPDF_ImportNPagesToOne(document(), 792, 612, 8, 1));
  ASSERT_TRUE(output_doc_8up);
  EXPECT_EQ(1, FPDF_GetPageCount(output_doc_8up.get()));
  ScopedFPDFDocument output_doc_128up(
      FPDF_ImportNPagesToOne(document(), 792, 612, 128, 1));
  ASSERT_TRUE(output_doc_128up);
  EXPECT_EQ(1, FPDF_GetPageCount(output_doc_128up.get()));
}

TEST_F(FPDFPPOEmbedderTest, BadNupParams) {
  ASSERT_TRUE(OpenDocument("rectangles_multi_pages.pdf"));

  FPDF_DOCUMENT output_doc_zero_row =
      FPDF_ImportNPagesToOne(document(), 612, 792, 0, 3);
  ASSERT_FALSE(output_doc_zero_row);
  FPDF_DOCUMENT output_doc_zero_col =
      FPDF_ImportNPagesToOne(document(), 612, 792, 2, 0);
  ASSERT_FALSE(output_doc_zero_col);
  FPDF_DOCUMENT output_doc_zero_width =
      FPDF_ImportNPagesToOne(document(), 0, 792, 2, 1);
  ASSERT_FALSE(output_doc_zero_width);
  FPDF_DOCUMENT output_doc_zero_height =
      FPDF_ImportNPagesToOne(document(), 612, 0, 7, 1);
  ASSERT_FALSE(output_doc_zero_height);
}

// TODO(Xlou): Add more tests to check output doc content of
// FPDF_ImportNPagesToOne()
TEST_F(FPDFPPOEmbedderTest, NupRenderImage) {
  ASSERT_TRUE(OpenDocument("rectangles_multi_pages.pdf"));
  const int kPageCount = 2;
  constexpr const char* kExpectedMD5s[kPageCount] = {
      "4d225b961da0f1bced7c83273e64c9b6", "fb18142190d770cfbc329d2b071aee4d"};
  ScopedFPDFDocument output_doc_3up(
      FPDF_ImportNPagesToOne(document(), 792, 612, 3, 1));
  ASSERT_TRUE(output_doc_3up);
  ASSERT_EQ(kPageCount, FPDF_GetPageCount(output_doc_3up.get()));
  for (int i = 0; i < kPageCount; ++i) {
    ScopedFPDFPage page(FPDF_LoadPage(output_doc_3up.get(), i));
    ASSERT_TRUE(page);
    ScopedFPDFBitmap bitmap(RenderPageWithFlags(page.get(), nullptr, 0));
    EXPECT_EQ(792, FPDFBitmap_GetWidth(bitmap.get()));
    EXPECT_EQ(612, FPDFBitmap_GetHeight(bitmap.get()));
    EXPECT_EQ(kExpectedMD5s[i], HashBitmap(bitmap.get()));
  }
}

TEST_F(FPDFPPOEmbedderTest, BadRepeatViewerPref) {
  ASSERT_TRUE(OpenDocument("repeat_viewer_ref.pdf"));

  FPDF_DOCUMENT output_doc = FPDF_CreateNewDocument();
  EXPECT_TRUE(output_doc);
  EXPECT_TRUE(FPDF_CopyViewerPreferences(output_doc, document()));

  FPDF_FILEWRITE writer;
  writer.version = 1;
  writer.WriteBlock = FakeBlockWriter;

  EXPECT_TRUE(FPDF_SaveAsCopy(output_doc, &writer, 0));
  FPDF_CloseDocument(output_doc);
}

TEST_F(FPDFPPOEmbedderTest, BadCircularViewerPref) {
  ASSERT_TRUE(OpenDocument("circular_viewer_ref.pdf"));

  FPDF_DOCUMENT output_doc = FPDF_CreateNewDocument();
  EXPECT_TRUE(output_doc);
  EXPECT_TRUE(FPDF_CopyViewerPreferences(output_doc, document()));

  FPDF_FILEWRITE writer;
  writer.version = 1;
  writer.WriteBlock = FakeBlockWriter;

  EXPECT_TRUE(FPDF_SaveAsCopy(output_doc, &writer, 0));
  FPDF_CloseDocument(output_doc);
}

TEST_F(FPDFPPOEmbedderTest, BadRanges) {
  EXPECT_TRUE(OpenDocument("hello_world.pdf"));

  FPDF_PAGE page = LoadPage(0);
  EXPECT_TRUE(page);

  FPDF_DOCUMENT output_doc = FPDF_CreateNewDocument();
  EXPECT_TRUE(output_doc);
  EXPECT_FALSE(FPDF_ImportPages(output_doc, document(), "clams", 0));
  EXPECT_FALSE(FPDF_ImportPages(output_doc, document(), "0", 0));
  EXPECT_FALSE(FPDF_ImportPages(output_doc, document(), "42", 0));
  EXPECT_FALSE(FPDF_ImportPages(output_doc, document(), "1,2", 0));
  EXPECT_FALSE(FPDF_ImportPages(output_doc, document(), "1-2", 0));
  EXPECT_FALSE(FPDF_ImportPages(output_doc, document(), ",1", 0));
  EXPECT_FALSE(FPDF_ImportPages(output_doc, document(), "1,", 0));
  EXPECT_FALSE(FPDF_ImportPages(output_doc, document(), "1-", 0));
  EXPECT_FALSE(FPDF_ImportPages(output_doc, document(), "-1", 0));
  EXPECT_FALSE(FPDF_ImportPages(output_doc, document(), "-,0,,,1-", 0));
  FPDF_CloseDocument(output_doc);

  UnloadPage(page);
}

TEST_F(FPDFPPOEmbedderTest, GoodRanges) {
  EXPECT_TRUE(OpenDocument("viewer_ref.pdf"));

  FPDF_PAGE page = LoadPage(0);
  EXPECT_TRUE(page);

  FPDF_DOCUMENT output_doc = FPDF_CreateNewDocument();
  EXPECT_TRUE(output_doc);
  EXPECT_TRUE(FPDF_CopyViewerPreferences(output_doc, document()));
  EXPECT_TRUE(FPDF_ImportPages(output_doc, document(), "1,1,1,1", 0));
  EXPECT_EQ(4, FPDF_GetPageCount(output_doc));
  EXPECT_TRUE(FPDF_ImportPages(output_doc, document(), "1-1", 0));
  EXPECT_EQ(5, FPDF_GetPageCount(output_doc));
  EXPECT_TRUE(FPDF_ImportPages(output_doc, document(), "5-5", 0));
  EXPECT_EQ(6, FPDF_GetPageCount(output_doc));
  EXPECT_TRUE(FPDF_ImportPages(output_doc, document(), "2-4", 0));
  EXPECT_EQ(9, FPDF_GetPageCount(output_doc));
  FPDF_CloseDocument(output_doc);

  UnloadPage(page);
}

TEST_F(FPDFPPOEmbedderTest, BUG_664284) {
  EXPECT_TRUE(OpenDocument("bug_664284.pdf"));

  FPDF_PAGE page = LoadPage(0);
  ASSERT_NE(nullptr, page);

  FPDF_DOCUMENT output_doc = FPDF_CreateNewDocument();
  EXPECT_TRUE(output_doc);
  EXPECT_TRUE(FPDF_ImportPages(output_doc, document(), "1", 0));
  FPDF_CloseDocument(output_doc);

  UnloadPage(page);
}

TEST_F(FPDFPPOEmbedderTest, BUG_750568) {
  const char* const kHashes[] = {
      "64ad08132a1c5a166768298c8a578f57", "83b83e2f6bc80707d0a917c7634140b9",
      "913cd3723a451e4e46fbc2c05702d1ee", "81fb7cfd4860f855eb468f73dfeb6d60"};

  ASSERT_TRUE(OpenDocument("bug_750568.pdf"));
  ASSERT_EQ(4, FPDF_GetPageCount(document()));

  for (size_t i = 0; i < 4; ++i) {
    FPDF_PAGE page = LoadPage(i);
    ASSERT_TRUE(page);

    ScopedFPDFBitmap bitmap = RenderLoadedPage(page);
    ASSERT_EQ(200, FPDFBitmap_GetWidth(bitmap.get()));
    ASSERT_EQ(200, FPDFBitmap_GetHeight(bitmap.get()));
    ASSERT_EQ(800, FPDFBitmap_GetStride(bitmap.get()));

    EXPECT_EQ(kHashes[i], HashBitmap(bitmap.get()));
    UnloadPage(page);
  }

  FPDF_DOCUMENT output_doc = FPDF_CreateNewDocument();
  ASSERT_TRUE(output_doc);
  EXPECT_TRUE(FPDF_ImportPages(output_doc, document(), "1,2,3,4", 0));
  ASSERT_EQ(4, FPDF_GetPageCount(output_doc));
  for (size_t i = 0; i < 4; ++i) {
    FPDF_PAGE page = FPDF_LoadPage(output_doc, i);
    ASSERT_TRUE(page);

    ScopedFPDFBitmap bitmap = RenderPageWithFlags(page, nullptr, 0);
    ASSERT_EQ(200, FPDFBitmap_GetWidth(bitmap.get()));
    ASSERT_EQ(200, FPDFBitmap_GetHeight(bitmap.get()));
    ASSERT_EQ(800, FPDFBitmap_GetStride(bitmap.get()));

    EXPECT_EQ(kHashes[i], HashBitmap(bitmap.get()));
    FPDF_ClosePage(page);
  }
  FPDF_CloseDocument(output_doc);
}

TEST_F(FPDFPPOEmbedderTest, ImportWithZeroLengthStream) {
  EXPECT_TRUE(OpenDocument("zero_length_stream.pdf"));
  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);

  ScopedFPDFBitmap bitmap = RenderLoadedPage(page);
  ASSERT_EQ(200, FPDFBitmap_GetWidth(bitmap.get()));
  ASSERT_EQ(200, FPDFBitmap_GetHeight(bitmap.get()));
  ASSERT_EQ(800, FPDFBitmap_GetStride(bitmap.get()));

  std::string digest = HashBitmap(bitmap.get());
  UnloadPage(page);

  FPDF_DOCUMENT new_doc = FPDF_CreateNewDocument();
  EXPECT_TRUE(new_doc);
  EXPECT_TRUE(FPDF_ImportPages(new_doc, document(), "1", 0));

  EXPECT_EQ(1, FPDF_GetPageCount(new_doc));
  FPDF_PAGE new_page = FPDF_LoadPage(new_doc, 0);
  ASSERT_NE(nullptr, new_page);
  ScopedFPDFBitmap new_bitmap = RenderPageWithFlags(new_page, nullptr, 0);
  ASSERT_EQ(200, FPDFBitmap_GetWidth(new_bitmap.get()));
  ASSERT_EQ(200, FPDFBitmap_GetHeight(new_bitmap.get()));
  ASSERT_EQ(800, FPDFBitmap_GetStride(new_bitmap.get()));

  EXPECT_EQ(digest, HashBitmap(new_bitmap.get()));
  FPDF_ClosePage(new_page);
  FPDF_CloseDocument(new_doc);
}
