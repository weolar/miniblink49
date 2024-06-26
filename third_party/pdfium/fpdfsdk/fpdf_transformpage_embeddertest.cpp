// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "public/fpdf_transformpage.h"
#include "testing/embedder_test.h"

class FPDFTransformEmbedderTest : public EmbedderTest {};

TEST_F(FPDFTransformEmbedderTest, GetBoundingBoxes) {
  ASSERT_TRUE(OpenDocument("cropped_text.pdf"));
  ASSERT_EQ(4, FPDF_GetPageCount(document()));

  {
    FPDF_PAGE page = LoadPage(1);
    ASSERT_TRUE(page);

    FS_RECTF mediabox;
    EXPECT_TRUE(FPDFPage_GetMediaBox(page, &mediabox.left, &mediabox.bottom,
                                     &mediabox.right, &mediabox.top));
    EXPECT_EQ(-50, mediabox.left);
    EXPECT_EQ(-50, mediabox.bottom);
    EXPECT_EQ(200, mediabox.right);
    EXPECT_EQ(200, mediabox.top);

    FS_RECTF cropbox;
    EXPECT_TRUE(FPDFPage_GetCropBox(page, &cropbox.left, &cropbox.bottom,
                                    &cropbox.right, &cropbox.top));
    EXPECT_EQ(50, cropbox.left);
    EXPECT_EQ(50, cropbox.bottom);
    EXPECT_EQ(150, cropbox.right);
    EXPECT_EQ(150, cropbox.top);

    FS_RECTF bleedbox;
    EXPECT_TRUE(FPDFPage_GetBleedBox(page, &bleedbox.left, &bleedbox.bottom,
                                     &bleedbox.right, &bleedbox.top));
    EXPECT_EQ(0, bleedbox.left);
    EXPECT_EQ(10, bleedbox.bottom);
    EXPECT_EQ(150, bleedbox.right);
    EXPECT_EQ(145, bleedbox.top);

    FS_RECTF trimbox;
    EXPECT_TRUE(FPDFPage_GetTrimBox(page, &trimbox.left, &trimbox.bottom,
                                    &trimbox.right, &trimbox.top));
    EXPECT_EQ(25, trimbox.left);
    EXPECT_EQ(30, trimbox.bottom);
    EXPECT_EQ(140, trimbox.right);
    EXPECT_EQ(145, trimbox.top);

    FS_RECTF artbox;
    EXPECT_TRUE(FPDFPage_GetArtBox(page, &artbox.left, &artbox.bottom,
                                   &artbox.right, &artbox.top));
    EXPECT_EQ(50, artbox.left);
    EXPECT_EQ(60, artbox.bottom);
    EXPECT_EQ(135, artbox.right);
    EXPECT_EQ(140, artbox.top);

    UnloadPage(page);
  }

  {
    FPDF_PAGE page = LoadPage(3);
    ASSERT_TRUE(page);

    FS_RECTF mediabox;
    EXPECT_TRUE(FPDFPage_GetMediaBox(page, &mediabox.left, &mediabox.bottom,
                                     &mediabox.right, &mediabox.top));
    EXPECT_EQ(0, mediabox.left);
    EXPECT_EQ(0, mediabox.bottom);
    EXPECT_EQ(200, mediabox.right);
    EXPECT_EQ(200, mediabox.top);

    FS_RECTF cropbox;
    EXPECT_TRUE(FPDFPage_GetCropBox(page, &cropbox.left, &cropbox.bottom,
                                    &cropbox.right, &cropbox.top));
    EXPECT_EQ(150, cropbox.left);
    EXPECT_EQ(150, cropbox.bottom);
    EXPECT_EQ(60, cropbox.right);
    EXPECT_EQ(60, cropbox.top);

    EXPECT_FALSE(FPDFPage_GetCropBox(page, nullptr, &cropbox.bottom,
                                     &cropbox.right, &cropbox.top));
    EXPECT_FALSE(FPDFPage_GetCropBox(page, &cropbox.left, nullptr,
                                     &cropbox.right, &cropbox.top));
    EXPECT_FALSE(FPDFPage_GetCropBox(page, &cropbox.left, &cropbox.bottom,
                                     nullptr, &cropbox.top));
    EXPECT_FALSE(FPDFPage_GetCropBox(page, &cropbox.left, &cropbox.bottom,
                                     &cropbox.right, nullptr));
    EXPECT_FALSE(FPDFPage_GetCropBox(page, nullptr, nullptr, nullptr, nullptr));

    FS_RECTF bleedbox;
    EXPECT_TRUE(FPDFPage_GetBleedBox(page, &bleedbox.left, &bleedbox.bottom,
                                     &bleedbox.right, &bleedbox.top));
    EXPECT_EQ(160, bleedbox.left);
    EXPECT_EQ(165, bleedbox.bottom);
    EXPECT_EQ(0, bleedbox.right);
    EXPECT_EQ(10, bleedbox.top);

    FS_RECTF trimbox;
    EXPECT_TRUE(FPDFPage_GetTrimBox(page, &trimbox.left, &trimbox.bottom,
                                    &trimbox.right, &trimbox.top));
    EXPECT_EQ(155, trimbox.left);
    EXPECT_EQ(165, trimbox.bottom);
    EXPECT_EQ(25, trimbox.right);
    EXPECT_EQ(30, trimbox.top);

    FS_RECTF artbox;
    EXPECT_TRUE(FPDFPage_GetArtBox(page, &artbox.left, &artbox.bottom,
                                   &artbox.right, &artbox.top));
    EXPECT_EQ(140, artbox.left);
    EXPECT_EQ(145, artbox.bottom);
    EXPECT_EQ(65, artbox.right);
    EXPECT_EQ(70, artbox.top);

    UnloadPage(page);
  }
}

TEST_F(FPDFTransformEmbedderTest, NoCropBox) {
  ASSERT_TRUE(OpenDocument("hello_world.pdf"));
  ASSERT_EQ(1, FPDF_GetPageCount(document()));

  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);

  FS_RECTF cropbox = {-1.0f, 0.0f, 3.0f, -2.0f};
  EXPECT_FALSE(FPDFPage_GetCropBox(page, &cropbox.left, &cropbox.bottom,
                                   &cropbox.right, &cropbox.top));
  EXPECT_EQ(-1.0f, cropbox.left);
  EXPECT_EQ(-2.0f, cropbox.bottom);
  EXPECT_EQ(3.0f, cropbox.right);
  EXPECT_EQ(0.0f, cropbox.top);

  UnloadPage(page);
}

TEST_F(FPDFTransformEmbedderTest, NoBleedBox) {
  ASSERT_TRUE(OpenDocument("hello_world.pdf"));
  ASSERT_EQ(1, FPDF_GetPageCount(document()));

  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);

  FS_RECTF bleedbox = {-1.0f, 10.f, 3.0f, -1.0f};
  EXPECT_FALSE(FPDFPage_GetBleedBox(page, &bleedbox.left, &bleedbox.bottom,
                                    &bleedbox.right, &bleedbox.top));
  EXPECT_EQ(-1.0f, bleedbox.left);
  EXPECT_EQ(-1.0f, bleedbox.bottom);
  EXPECT_EQ(3.0f, bleedbox.right);
  EXPECT_EQ(10.0f, bleedbox.top);

  UnloadPage(page);
}

TEST_F(FPDFTransformEmbedderTest, NoTrimBox) {
  ASSERT_TRUE(OpenDocument("hello_world.pdf"));
  ASSERT_EQ(1, FPDF_GetPageCount(document()));

  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);

  FS_RECTF trimbox = {-11.0f, 0.0f, 3.0f, -10.0f};
  EXPECT_FALSE(FPDFPage_GetTrimBox(page, &trimbox.left, &trimbox.bottom,
                                   &trimbox.right, &trimbox.top));
  EXPECT_EQ(-11.0f, trimbox.left);
  EXPECT_EQ(-10.0f, trimbox.bottom);
  EXPECT_EQ(3.0f, trimbox.right);
  EXPECT_EQ(0.0f, trimbox.top);

  UnloadPage(page);
}

TEST_F(FPDFTransformEmbedderTest, NoArtBox) {
  ASSERT_TRUE(OpenDocument("hello_world.pdf"));
  ASSERT_EQ(1, FPDF_GetPageCount(document()));

  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);

  FS_RECTF artbox = {-1.0f, 0.0f, 3.0f, -1.0f};
  EXPECT_FALSE(FPDFPage_GetArtBox(page, &artbox.left, &artbox.bottom,
                                  &artbox.right, &artbox.top));
  EXPECT_EQ(-1.0f, artbox.left);
  EXPECT_EQ(-1.0f, artbox.bottom);
  EXPECT_EQ(3.0f, artbox.right);
  EXPECT_EQ(0.0f, artbox.top);

  UnloadPage(page);
}

TEST_F(FPDFTransformEmbedderTest, SetCropBox) {
  const char kOriginalMD5[] = "0a90de37f52127619c3dfb642b5fa2fe";
  const char kCroppedMD5[] = "9937883715d5144c079fb8f7e3d4f395";

  {
    ASSERT_TRUE(OpenDocument("rectangles.pdf"));
    FPDF_PAGE page = LoadPage(0);
    ASSERT_TRUE(page);

    {
      // Render the page as is.
      FS_RECTF cropbox;
      EXPECT_FALSE(FPDFPage_GetCropBox(page, &cropbox.left, &cropbox.bottom,
                                       &cropbox.right, &cropbox.top));
      const int page_width = static_cast<int>(FPDF_GetPageWidth(page));
      const int page_height = static_cast<int>(FPDF_GetPageHeight(page));
      EXPECT_EQ(200, page_width);
      EXPECT_EQ(300, page_height);
      ScopedFPDFBitmap bitmap = RenderLoadedPage(page);
      CompareBitmap(bitmap.get(), page_width, page_height, kOriginalMD5);
    }

    FPDFPage_SetCropBox(page, 10, 20, 100, 150);

    {
      // Render the page after setting the CropBox.
      // Note that the change affects the rendering, as expected.
      // It behaves just like the case below, rather than the case above.
      FS_RECTF cropbox;
      EXPECT_TRUE(FPDFPage_GetCropBox(page, &cropbox.left, &cropbox.bottom,
                                      &cropbox.right, &cropbox.top));
      EXPECT_EQ(10, cropbox.left);
      EXPECT_EQ(20, cropbox.bottom);
      EXPECT_EQ(100, cropbox.right);
      EXPECT_EQ(150, cropbox.top);
      const int page_width = static_cast<int>(FPDF_GetPageWidth(page));
      const int page_height = static_cast<int>(FPDF_GetPageHeight(page));
      EXPECT_EQ(90, page_width);
      EXPECT_EQ(130, page_height);
      ScopedFPDFBitmap bitmap = RenderLoadedPage(page);
      CompareBitmap(bitmap.get(), page_width, page_height, kCroppedMD5);
    }

    UnloadPage(page);
  }

  {
    // Save a copy, open the copy, and render it.
    // Note that it renders the rotation.
    EXPECT_TRUE(FPDF_SaveAsCopy(document(), this, 0));
    OpenSavedDocument(nullptr);
    FPDF_PAGE saved_page = LoadSavedPage(0);
    ASSERT_TRUE(saved_page);

    FS_RECTF cropbox;
    EXPECT_TRUE(FPDFPage_GetCropBox(saved_page, &cropbox.left, &cropbox.bottom,
                                    &cropbox.right, &cropbox.top));
    EXPECT_EQ(10, cropbox.left);
    EXPECT_EQ(20, cropbox.bottom);
    EXPECT_EQ(100, cropbox.right);
    EXPECT_EQ(150, cropbox.top);
    const int page_width = static_cast<int>(FPDF_GetPageWidth(saved_page));
    const int page_height = static_cast<int>(FPDF_GetPageHeight(saved_page));
    EXPECT_EQ(90, page_width);
    EXPECT_EQ(130, page_height);
    ScopedFPDFBitmap bitmap = RenderSavedPage(saved_page);
    CompareBitmap(bitmap.get(), page_width, page_height, kCroppedMD5);

    CloseSavedPage(saved_page);
    CloseSavedDocument();
  }
}

TEST_F(FPDFTransformEmbedderTest, SetMediaBox) {
  const char kOriginalMD5[] = "0a90de37f52127619c3dfb642b5fa2fe";
  const char kShrunkMD5[] = "eab5958f62f7ce65d7c32de98389fee1";

  {
    ASSERT_TRUE(OpenDocument("rectangles.pdf"));
    FPDF_PAGE page = LoadPage(0);
    ASSERT_TRUE(page);

    {
      // Render the page as is.
      FS_RECTF mediabox;
      EXPECT_FALSE(FPDFPage_GetMediaBox(page, &mediabox.left, &mediabox.bottom,
                                        &mediabox.right, &mediabox.top));
      const int page_width = static_cast<int>(FPDF_GetPageWidth(page));
      const int page_height = static_cast<int>(FPDF_GetPageHeight(page));
      EXPECT_EQ(200, page_width);
      EXPECT_EQ(300, page_height);
      ScopedFPDFBitmap bitmap = RenderLoadedPage(page);
      CompareBitmap(bitmap.get(), page_width, page_height, kOriginalMD5);
    }

    FPDFPage_SetMediaBox(page, 20, 30, 100, 150);

    {
      // Render the page after setting the MediaBox.
      // Note that the change affects the rendering, as expected.
      // It behaves just like the case below, rather than the case above.
      FS_RECTF mediabox;
      EXPECT_TRUE(FPDFPage_GetMediaBox(page, &mediabox.left, &mediabox.bottom,
                                       &mediabox.right, &mediabox.top));
      EXPECT_EQ(20, mediabox.left);
      EXPECT_EQ(30, mediabox.bottom);
      EXPECT_EQ(100, mediabox.right);
      EXPECT_EQ(150, mediabox.top);
      const int page_width = static_cast<int>(FPDF_GetPageWidth(page));
      const int page_height = static_cast<int>(FPDF_GetPageHeight(page));
      EXPECT_EQ(80, page_width);
      EXPECT_EQ(120, page_height);
      ScopedFPDFBitmap bitmap = RenderLoadedPage(page);
      CompareBitmap(bitmap.get(), page_width, page_height, kShrunkMD5);
    }

    UnloadPage(page);
  }

  {
    // Save a copy, open the copy, and render it.
    // Note that it renders the rotation.
    EXPECT_TRUE(FPDF_SaveAsCopy(document(), this, 0));
    OpenSavedDocument(nullptr);
    FPDF_PAGE saved_page = LoadSavedPage(0);
    ASSERT_TRUE(saved_page);

    FS_RECTF mediabox;
    EXPECT_TRUE(FPDFPage_GetMediaBox(saved_page, &mediabox.left,
                                     &mediabox.bottom, &mediabox.right,
                                     &mediabox.top));
    EXPECT_EQ(20, mediabox.left);
    EXPECT_EQ(30, mediabox.bottom);
    EXPECT_EQ(100, mediabox.right);
    EXPECT_EQ(150, mediabox.top);
    const int page_width = static_cast<int>(FPDF_GetPageWidth(saved_page));
    const int page_height = static_cast<int>(FPDF_GetPageHeight(saved_page));
    EXPECT_EQ(80, page_width);
    EXPECT_EQ(120, page_height);
    ScopedFPDFBitmap bitmap = RenderSavedPage(saved_page);
    CompareBitmap(bitmap.get(), page_width, page_height, kShrunkMD5);

    CloseSavedPage(saved_page);
    CloseSavedDocument();
  }
}

TEST_F(FPDFTransformEmbedderTest, ClipPath) {
  ASSERT_TRUE(OpenDocument("hello_world.pdf"));

  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);

  FPDF_CLIPPATH clip = FPDF_CreateClipPath(10.0f, 10.0f, 90.0f, 90.0f);
  EXPECT_TRUE(clip);

  // NULL arg call is a no-op.
  FPDFPage_InsertClipPath(nullptr, clip);

  // Do actual work.
  FPDFPage_InsertClipPath(page, clip);

  // TODO(tsepez): test how inserting path affects page rendering.

  FPDF_DestroyClipPath(clip);
  UnloadPage(page);
}

TEST_F(FPDFTransformEmbedderTest, TransFormWithClip) {
  const FS_MATRIX half_matrix{0.5, 0, 0, 0.5, 0, 0};
  const FS_RECTF clip_rect = {0.0f, 0.0f, 20.0f, 10.0f};

  ASSERT_TRUE(OpenDocument("hello_world.pdf"));

  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);

  EXPECT_FALSE(FPDFPage_TransFormWithClip(nullptr, nullptr, nullptr));
  EXPECT_FALSE(FPDFPage_TransFormWithClip(nullptr, &half_matrix, nullptr));
  EXPECT_FALSE(FPDFPage_TransFormWithClip(nullptr, nullptr, &clip_rect));
  EXPECT_FALSE(FPDFPage_TransFormWithClip(nullptr, &half_matrix, &clip_rect));
  EXPECT_FALSE(FPDFPage_TransFormWithClip(page, nullptr, nullptr));
  EXPECT_TRUE(FPDFPage_TransFormWithClip(page, &half_matrix, nullptr));
  EXPECT_TRUE(FPDFPage_TransFormWithClip(page, nullptr, &clip_rect));
  EXPECT_TRUE(FPDFPage_TransFormWithClip(page, &half_matrix, &clip_rect));

  UnloadPage(page);
}

TEST_F(FPDFTransformEmbedderTest, TransFormWithClipWithPatterns) {
  const FS_MATRIX half_matrix{0.5, 0, 0, 0.5, 0, 0};
  const FS_RECTF clip_rect = {0.0f, 0.0f, 20.0f, 10.0f};

  ASSERT_TRUE(OpenDocument("bug_547706.pdf"));

  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);

  EXPECT_TRUE(FPDFPage_TransFormWithClip(page, &half_matrix, nullptr));
  EXPECT_TRUE(FPDFPage_TransFormWithClip(page, nullptr, &clip_rect));
  EXPECT_TRUE(FPDFPage_TransFormWithClip(page, &half_matrix, &clip_rect));

  UnloadPage(page);
}
