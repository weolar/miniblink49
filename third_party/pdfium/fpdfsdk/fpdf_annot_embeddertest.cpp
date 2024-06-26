// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <algorithm>
#include <cwchar>
#include <memory>
#include <string>
#include <vector>

#include "core/fxcrt/fx_system.h"
#include "public/cpp/fpdf_scopers.h"
#include "public/fpdf_annot.h"
#include "public/fpdf_edit.h"
#include "public/fpdfview.h"
#include "testing/embedder_test.h"
#include "testing/gmock/include/gmock/gmock-matchers.h"
#include "testing/gtest/include/gtest/gtest.h"

static constexpr char kContentsKey[] = "Contents";

class FPDFAnnotEmbedderTest : public EmbedderTest {};

std::wstring BufferToWString(const std::vector<char>& buf) {
  return GetPlatformWString(reinterpret_cast<FPDF_WIDESTRING>(buf.data()));
}

std::string BufferToString(const std::vector<char>& buf) {
  return GetPlatformString(reinterpret_cast<FPDF_WIDESTRING>(buf.data()));
}

TEST_F(FPDFAnnotEmbedderTest, BadParams) {
  ASSERT_TRUE(OpenDocument("hello_world.pdf"));
  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);

  EXPECT_EQ(0, FPDFPage_GetAnnotCount(nullptr));

  EXPECT_FALSE(FPDFPage_GetAnnot(nullptr, 0));
  EXPECT_FALSE(FPDFPage_GetAnnot(nullptr, -1));
  EXPECT_FALSE(FPDFPage_GetAnnot(nullptr, 1));
  EXPECT_FALSE(FPDFPage_GetAnnot(page, -1));
  EXPECT_FALSE(FPDFPage_GetAnnot(page, 1));

  EXPECT_EQ(FPDF_ANNOT_UNKNOWN, FPDFAnnot_GetSubtype(nullptr));

  EXPECT_EQ(0, FPDFAnnot_GetObjectCount(nullptr));

  EXPECT_FALSE(FPDFAnnot_GetObject(nullptr, 0));
  EXPECT_FALSE(FPDFAnnot_GetObject(nullptr, -1));
  EXPECT_FALSE(FPDFAnnot_GetObject(nullptr, 1));

  EXPECT_FALSE(FPDFAnnot_HasKey(nullptr, "foo"));

  static constexpr wchar_t kContents[] = L"Bar";
  std::unique_ptr<unsigned short, pdfium::FreeDeleter> text =
      GetFPDFWideString(kContents);
  EXPECT_FALSE(FPDFAnnot_SetStringValue(nullptr, "foo", text.get()));

  char buffer[128];
  EXPECT_EQ(0u, FPDFAnnot_GetStringValue(nullptr, "foo", nullptr, 0));
  EXPECT_EQ(0u, FPDFAnnot_GetStringValue(nullptr, "foo", buffer, 0));
  EXPECT_EQ(0u,
            FPDFAnnot_GetStringValue(nullptr, "foo", buffer, sizeof(buffer)));

  UnloadPage(page);
}

TEST_F(FPDFAnnotEmbedderTest, RenderAnnotWithOnlyRolloverAP) {
  // Open a file with one annotation and load its first page.
  ASSERT_TRUE(OpenDocument("annotation_highlight_rollover_ap.pdf"));
  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);

  // This annotation has a malformed appearance stream, which does not have its
  // normal appearance defined, only its rollover appearance. In this case, its
  // normal appearance should be generated, allowing the highlight annotation to
  // still display.
  ScopedFPDFBitmap bitmap = RenderLoadedPageWithFlags(page, FPDF_ANNOT);
  CompareBitmap(bitmap.get(), 612, 792, "dc98f06da047bd8aabfa99562d2cbd1e");

  UnloadPage(page);
}

TEST_F(FPDFAnnotEmbedderTest, RenderMultilineMarkupAnnotWithoutAP) {
  const char md5_hash[] = "76512832d88017668d9acc7aacd13dae";
  // Open a file with multiline markup annotations.
  ASSERT_TRUE(OpenDocument("annotation_markup_multiline_no_ap.pdf"));
  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);

  ScopedFPDFBitmap bitmap = RenderLoadedPageWithFlags(page, FPDF_ANNOT);
  CompareBitmap(bitmap.get(), 595, 842, md5_hash);

  UnloadPage(page);
}

TEST_F(FPDFAnnotEmbedderTest, ExtractHighlightLongContent) {
  // Open a file with one annotation and load its first page.
  ASSERT_TRUE(OpenDocument("annotation_highlight_long_content.pdf"));
  FPDF_PAGE page = LoadPageNoEvents(0);
  ASSERT_TRUE(page);

  // Check that there is a total of 1 annotation on its first page.
  EXPECT_EQ(1, FPDFPage_GetAnnotCount(page));

  // Check that the annotation is of type "highlight".
  {
    ScopedFPDFAnnotation annot(FPDFPage_GetAnnot(page, 0));
    ASSERT_TRUE(annot);
    EXPECT_EQ(FPDF_ANNOT_HIGHLIGHT, FPDFAnnot_GetSubtype(annot.get()));

    // Check that the annotation color is yellow.
    unsigned int R;
    unsigned int G;
    unsigned int B;
    unsigned int A;
    ASSERT_TRUE(FPDFAnnot_GetColor(annot.get(), FPDFANNOT_COLORTYPE_Color, &R,
                                   &G, &B, &A));
    EXPECT_EQ(255u, R);
    EXPECT_EQ(255u, G);
    EXPECT_EQ(0u, B);
    EXPECT_EQ(255u, A);

    // Check that the author is correct.
    static constexpr char kAuthorKey[] = "T";
    EXPECT_EQ(FPDF_OBJECT_STRING,
              FPDFAnnot_GetValueType(annot.get(), kAuthorKey));
    unsigned long len =
        FPDFAnnot_GetStringValue(annot.get(), kAuthorKey, nullptr, 0);
    std::vector<char> buf(len);
    EXPECT_EQ(28u, FPDFAnnot_GetStringValue(annot.get(), kAuthorKey, buf.data(),
                                            len));
    EXPECT_STREQ(L"Jae Hyun Park", BufferToWString(buf).c_str());

    // Check that the content is correct.
    EXPECT_EQ(FPDF_OBJECT_STRING,
              FPDFAnnot_GetValueType(annot.get(), kContentsKey));
    len = FPDFAnnot_GetStringValue(annot.get(), kContentsKey, nullptr, 0);
    buf.clear();
    buf.resize(len);
    EXPECT_EQ(2690u, FPDFAnnot_GetStringValue(annot.get(), kContentsKey,
                                              buf.data(), len));
    const wchar_t contents[] =
        L"This is a note for that highlight annotation. Very long highlight "
        "annotation. Long long long Long long longLong long longLong long "
        "longLong long longLong long longLong long longLong long longLong long "
        "longLong long longLong long longLong long longLong long longLong long "
        "longLong long longLong long longLong long longLong long longLong long "
        "longLong long longLong long longLong long longLong long longLong long "
        "longLong long longLong long longLong long longLong long longLong long "
        "longLong long longLong long longLong long longLong long longLong long "
        "longLong long longLong long longLong long longLong long longLong long "
        "longLong long longLong long longLong long longLong long longLong long "
        "longLong long longLong long longLong long longLong long longLong long "
        "longLong long longLong long longLong long longLong long longLong long "
        "longLong long longLong long longLong long longLong long longLong long "
        "longLong long longLong long longLong long longLong long longLong long "
        "longLong long longLong long longLong long longLong long longLong long "
        "longLong long longLong long longLong long longLong long longLong long "
        "longLong long longLong long longLong long longLong long longLong long "
        "longLong long longLong long longLong long longLong long longLong long "
        "longLong long longLong long longLong long longLong long longLong long "
        "longLong long long. END";
    EXPECT_STREQ(contents, BufferToWString(buf).c_str());

    // Check that the quadpoints are correct.
    FS_QUADPOINTSF quadpoints;
    ASSERT_TRUE(FPDFAnnot_GetAttachmentPoints(annot.get(), 0, &quadpoints));
    EXPECT_EQ(115.802643f, quadpoints.x1);
    EXPECT_EQ(718.913940f, quadpoints.y1);
    EXPECT_EQ(157.211182f, quadpoints.x4);
    EXPECT_EQ(706.264465f, quadpoints.y4);
  }
  UnloadPageNoEvents(page);
}

TEST_F(FPDFAnnotEmbedderTest, ExtractInkMultiple) {
  // Open a file with three annotations and load its first page.
  ASSERT_TRUE(OpenDocument("annotation_ink_multiple.pdf"));
  FPDF_PAGE page = LoadPageNoEvents(0);
  ASSERT_TRUE(page);

  // Check that there is a total of 3 annotation on its first page.
  EXPECT_EQ(3, FPDFPage_GetAnnotCount(page));

  {
    // Check that the third annotation is of type "ink".
    ScopedFPDFAnnotation annot(FPDFPage_GetAnnot(page, 2));
    ASSERT_TRUE(annot);
    EXPECT_EQ(FPDF_ANNOT_INK, FPDFAnnot_GetSubtype(annot.get()));

    // Check that the annotation color is blue with opacity.
    unsigned int R;
    unsigned int G;
    unsigned int B;
    unsigned int A;
    ASSERT_TRUE(FPDFAnnot_GetColor(annot.get(), FPDFANNOT_COLORTYPE_Color, &R,
                                   &G, &B, &A));
    EXPECT_EQ(0u, R);
    EXPECT_EQ(0u, G);
    EXPECT_EQ(255u, B);
    EXPECT_EQ(76u, A);

    // Check that there is no content.
    EXPECT_EQ(2u,
              FPDFAnnot_GetStringValue(annot.get(), kContentsKey, nullptr, 0));

    // Check that the rectange coordinates are correct.
    // Note that upon rendering, the rectangle coordinates will be adjusted.
    FS_RECTF rect;
    ASSERT_TRUE(FPDFAnnot_GetRect(annot.get(), &rect));
    EXPECT_EQ(351.820404f, rect.left);
    EXPECT_EQ(583.830688f, rect.bottom);
    EXPECT_EQ(475.336090f, rect.right);
    EXPECT_EQ(681.535034f, rect.top);
  }
  {
    ScopedFPDFBitmap bitmap = RenderLoadedPageWithFlags(page, FPDF_ANNOT);
    CompareBitmap(bitmap.get(), 612, 792, "354002e1c4386d38fdde29ef8d61074a");
  }
  UnloadPageNoEvents(page);
}

TEST_F(FPDFAnnotEmbedderTest, AddIllegalSubtypeAnnotation) {
  // Open a file with one annotation and load its first page.
  ASSERT_TRUE(OpenDocument("annotation_highlight_long_content.pdf"));
  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);

  // Add an annotation with an illegal subtype.
  ASSERT_FALSE(FPDFPage_CreateAnnot(page, -1));

  UnloadPage(page);
}

TEST_F(FPDFAnnotEmbedderTest, AddFirstTextAnnotation) {
  // Open a file with no annotation and load its first page.
  ASSERT_TRUE(OpenDocument("hello_world.pdf"));
  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);
  EXPECT_EQ(0, FPDFPage_GetAnnotCount(page));

  {
    // Add a text annotation to the page.
    ScopedFPDFAnnotation annot(FPDFPage_CreateAnnot(page, FPDF_ANNOT_TEXT));
    ASSERT_TRUE(annot);

    // Check that there is now 1 annotations on this page.
    EXPECT_EQ(1, FPDFPage_GetAnnotCount(page));

    // Check that the subtype of the annotation is correct.
    EXPECT_EQ(FPDF_ANNOT_TEXT, FPDFAnnot_GetSubtype(annot.get()));
  }

  {
    ScopedFPDFAnnotation annot(FPDFPage_GetAnnot(page, 0));
    ASSERT_TRUE(annot);
    EXPECT_EQ(FPDF_ANNOT_TEXT, FPDFAnnot_GetSubtype(annot.get()));

    // Set the color of the annotation.
    ASSERT_TRUE(FPDFAnnot_SetColor(annot.get(), FPDFANNOT_COLORTYPE_Color, 51,
                                   102, 153, 204));
    // Check that the color has been set correctly.
    unsigned int R;
    unsigned int G;
    unsigned int B;
    unsigned int A;
    ASSERT_TRUE(FPDFAnnot_GetColor(annot.get(), FPDFANNOT_COLORTYPE_Color, &R,
                                   &G, &B, &A));
    EXPECT_EQ(51u, R);
    EXPECT_EQ(102u, G);
    EXPECT_EQ(153u, B);
    EXPECT_EQ(204u, A);

    // Change the color of the annotation.
    ASSERT_TRUE(FPDFAnnot_SetColor(annot.get(), FPDFANNOT_COLORTYPE_Color, 204,
                                   153, 102, 51));
    // Check that the color has been set correctly.
    ASSERT_TRUE(FPDFAnnot_GetColor(annot.get(), FPDFANNOT_COLORTYPE_Color, &R,
                                   &G, &B, &A));
    EXPECT_EQ(204u, R);
    EXPECT_EQ(153u, G);
    EXPECT_EQ(102u, B);
    EXPECT_EQ(51u, A);

    // Set the annotation rectangle.
    FS_RECTF rect;
    ASSERT_TRUE(FPDFAnnot_GetRect(annot.get(), &rect));
    EXPECT_EQ(0.f, rect.left);
    EXPECT_EQ(0.f, rect.right);
    rect.left = 35;
    rect.bottom = 150;
    rect.right = 53;
    rect.top = 165;
    ASSERT_TRUE(FPDFAnnot_SetRect(annot.get(), &rect));
    // Check that the annotation rectangle has been set correctly.
    ASSERT_TRUE(FPDFAnnot_GetRect(annot.get(), &rect));
    EXPECT_EQ(35.f, rect.left);
    EXPECT_EQ(150.f, rect.bottom);
    EXPECT_EQ(53.f, rect.right);
    EXPECT_EQ(165.f, rect.top);

    // Set the content of the annotation.
    static constexpr wchar_t kContents[] =
        L"Hello! This is a customized content.";
    std::unique_ptr<unsigned short, pdfium::FreeDeleter> text =
        GetFPDFWideString(kContents);
    ASSERT_TRUE(
        FPDFAnnot_SetStringValue(annot.get(), kContentsKey, text.get()));
    // Check that the content has been set correctly.
    unsigned long len =
        FPDFAnnot_GetStringValue(annot.get(), kContentsKey, nullptr, 0);
    std::vector<char> buf(len);
    EXPECT_EQ(74u, FPDFAnnot_GetStringValue(annot.get(), kContentsKey,
                                            buf.data(), len));
    EXPECT_STREQ(kContents, BufferToWString(buf).c_str());
  }
  UnloadPage(page);
}

TEST_F(FPDFAnnotEmbedderTest, AddAndSaveUnderlineAnnotation) {
  // Open a file with one annotation and load its first page.
  ASSERT_TRUE(OpenDocument("annotation_highlight_long_content.pdf"));
  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);

  // Check that there is a total of one annotation on its first page, and verify
  // its quadpoints.
  EXPECT_EQ(1, FPDFPage_GetAnnotCount(page));
  FS_QUADPOINTSF quadpoints;
  {
    ScopedFPDFAnnotation annot(FPDFPage_GetAnnot(page, 0));
    ASSERT_TRUE(annot);
    ASSERT_TRUE(FPDFAnnot_GetAttachmentPoints(annot.get(), 0, &quadpoints));
    EXPECT_EQ(115.802643f, quadpoints.x1);
    EXPECT_EQ(718.913940f, quadpoints.y1);
    EXPECT_EQ(157.211182f, quadpoints.x4);
    EXPECT_EQ(706.264465f, quadpoints.y4);
  }

  // Add an underline annotation to the page and set its quadpoints.
  {
    ScopedFPDFAnnotation annot(
        FPDFPage_CreateAnnot(page, FPDF_ANNOT_UNDERLINE));
    ASSERT_TRUE(annot);
    quadpoints.x1 = 140.802643f;
    quadpoints.x3 = 140.802643f;
    ASSERT_TRUE(FPDFAnnot_AppendAttachmentPoints(annot.get(), &quadpoints));
  }

  // Save the document, closing the page and document.
  EXPECT_TRUE(FPDF_SaveAsCopy(document(), this, 0));
  UnloadPage(page);

  // Open the saved document.
  const char md5[] = "dba153419f67b7c0c0e3d22d3e8910d5";

  OpenSavedDocument(nullptr);
  page = LoadSavedPage(0);
  VerifySavedRendering(page, 612, 792, md5);

  // Check that the saved document has 2 annotations on the first page
  EXPECT_EQ(2, FPDFPage_GetAnnotCount(page));

  {
    // Check that the second annotation is an underline annotation and verify
    // its quadpoints.
    ScopedFPDFAnnotation new_annot(FPDFPage_GetAnnot(page, 1));
    ASSERT_TRUE(new_annot);
    EXPECT_EQ(FPDF_ANNOT_UNDERLINE, FPDFAnnot_GetSubtype(new_annot.get()));
    FS_QUADPOINTSF new_quadpoints;
    ASSERT_TRUE(
        FPDFAnnot_GetAttachmentPoints(new_annot.get(), 0, &new_quadpoints));
    EXPECT_NEAR(quadpoints.x1, new_quadpoints.x1, 0.001f);
    EXPECT_NEAR(quadpoints.y1, new_quadpoints.y1, 0.001f);
    EXPECT_NEAR(quadpoints.x4, new_quadpoints.x4, 0.001f);
    EXPECT_NEAR(quadpoints.y4, new_quadpoints.y4, 0.001f);
  }

  CloseSavedPage(page);
  CloseSavedDocument();
}

TEST_F(FPDFAnnotEmbedderTest, GetAndSetQuadPoints) {
  // Open a file with four annotations and load its first page.
  ASSERT_TRUE(OpenDocument("annotation_highlight_square_with_ap.pdf"));
  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);
  EXPECT_EQ(4, FPDFPage_GetAnnotCount(page));

  // Retrieve the highlight annotation.
  FPDF_ANNOTATION annot = FPDFPage_GetAnnot(page, 0);
  ASSERT_TRUE(annot);
  ASSERT_EQ(FPDF_ANNOT_HIGHLIGHT, FPDFAnnot_GetSubtype(annot));

  FS_QUADPOINTSF quadpoints;
  ASSERT_TRUE(FPDFAnnot_GetAttachmentPoints(annot, 0, &quadpoints));

  {
    // Verify the current one set of quadpoints.
    ASSERT_EQ(1u, FPDFAnnot_CountAttachmentPoints(annot));

    EXPECT_NEAR(72.0000f, quadpoints.x1, 0.001f);
    EXPECT_NEAR(720.792f, quadpoints.y1, 0.001f);
    EXPECT_NEAR(132.055f, quadpoints.x4, 0.001f);
    EXPECT_NEAR(704.796f, quadpoints.y4, 0.001f);
  }

  {
    // Update the quadpoints.
    FS_QUADPOINTSF new_quadpoints = quadpoints;
    new_quadpoints.y1 -= 20.f;
    new_quadpoints.y2 -= 20.f;
    new_quadpoints.y3 -= 20.f;
    new_quadpoints.y4 -= 20.f;
    ASSERT_TRUE(FPDFAnnot_SetAttachmentPoints(annot, 0, &new_quadpoints));

    // Verify added quadpoint set
    ASSERT_EQ(1u, FPDFAnnot_CountAttachmentPoints(annot));
    ASSERT_TRUE(FPDFAnnot_GetAttachmentPoints(annot, 0, &quadpoints));
    EXPECT_NEAR(new_quadpoints.x1, quadpoints.x1, 0.001f);
    EXPECT_NEAR(new_quadpoints.y1, quadpoints.y1, 0.001f);
    EXPECT_NEAR(new_quadpoints.x4, quadpoints.x4, 0.001f);
    EXPECT_NEAR(new_quadpoints.y4, quadpoints.y4, 0.001f);
  }

  {
    // Append a new set of quadpoints.
    FS_QUADPOINTSF new_quadpoints = quadpoints;
    new_quadpoints.y1 += 20.f;
    new_quadpoints.y2 += 20.f;
    new_quadpoints.y3 += 20.f;
    new_quadpoints.y4 += 20.f;
    ASSERT_TRUE(FPDFAnnot_AppendAttachmentPoints(annot, &new_quadpoints));

    // Verify added quadpoint set
    ASSERT_EQ(2u, FPDFAnnot_CountAttachmentPoints(annot));
    ASSERT_TRUE(FPDFAnnot_GetAttachmentPoints(annot, 1, &quadpoints));
    EXPECT_NEAR(new_quadpoints.x1, quadpoints.x1, 0.001f);
    EXPECT_NEAR(new_quadpoints.y1, quadpoints.y1, 0.001f);
    EXPECT_NEAR(new_quadpoints.x4, quadpoints.x4, 0.001f);
    EXPECT_NEAR(new_quadpoints.y4, quadpoints.y4, 0.001f);
  }

  {
    // Setting and getting quadpoints at out-of-bound index should fail
    EXPECT_FALSE(FPDFAnnot_SetAttachmentPoints(annot, 300000, &quadpoints));
    EXPECT_FALSE(FPDFAnnot_GetAttachmentPoints(annot, 300000, &quadpoints));
  }

  FPDFPage_CloseAnnot(annot);

  // Retrieve the square annotation
  FPDF_ANNOTATION squareAnnot = FPDFPage_GetAnnot(page, 2);

  {
    // Check that attempting to set its quadpoints would fail
    ASSERT_TRUE(squareAnnot);
    EXPECT_EQ(FPDF_ANNOT_SQUARE, FPDFAnnot_GetSubtype(squareAnnot));
    EXPECT_EQ(0u, FPDFAnnot_CountAttachmentPoints(squareAnnot));
    EXPECT_FALSE(FPDFAnnot_SetAttachmentPoints(squareAnnot, 0, &quadpoints));
  }

  FPDFPage_CloseAnnot(squareAnnot);
  UnloadPage(page);
}

TEST_F(FPDFAnnotEmbedderTest, ModifyRectQuadpointsWithAP) {
#if _FX_PLATFORM_ == _FX_PLATFORM_APPLE_
  const char md5_original[] = "63af8432fab95a67cdebb7cd0e514941";
  const char md5_modified_highlight[] = "aec26075011349dec9bace891856b5f2";
  const char md5_modified_square[] = "057f57a32be95975775e5ec513fdcb56";
#elif _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_
  const char md5_original[] = "0e27376094f11490f74c65f3dc3a42c5";
  const char md5_modified_highlight[] = "66f3caef3a7d488a4fa1ad37fc06310e";
  const char md5_modified_square[] = "a456dad0bc6801ee2d6408a4394af563";
#else
  const char md5_original[] = "0e27376094f11490f74c65f3dc3a42c5";
  const char md5_modified_highlight[] = "66f3caef3a7d488a4fa1ad37fc06310e";
  const char md5_modified_square[] = "a456dad0bc6801ee2d6408a4394af563";
#endif

  // Open a file with four annotations and load its first page.
  ASSERT_TRUE(OpenDocument("annotation_highlight_square_with_ap.pdf"));
  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);
  EXPECT_EQ(4, FPDFPage_GetAnnotCount(page));

  // Check that the original file renders correctly.
  {
    ScopedFPDFBitmap bitmap = RenderLoadedPageWithFlags(page, FPDF_ANNOT);
    CompareBitmap(bitmap.get(), 612, 792, md5_original);
  }

  FS_RECTF rect;
  FS_RECTF new_rect;

  // Retrieve the highlight annotation which has its AP stream already defined.
  {
    ScopedFPDFAnnotation annot(FPDFPage_GetAnnot(page, 0));
    ASSERT_TRUE(annot);
    EXPECT_EQ(FPDF_ANNOT_HIGHLIGHT, FPDFAnnot_GetSubtype(annot.get()));

    // Check that color cannot be set when an AP stream is defined already.
    EXPECT_FALSE(FPDFAnnot_SetColor(annot.get(), FPDFANNOT_COLORTYPE_Color, 51,
                                    102, 153, 204));

    // Verify its attachment points.
    FS_QUADPOINTSF quadpoints;
    ASSERT_TRUE(FPDFAnnot_GetAttachmentPoints(annot.get(), 0, &quadpoints));
    EXPECT_NEAR(72.0000f, quadpoints.x1, 0.001f);
    EXPECT_NEAR(720.792f, quadpoints.y1, 0.001f);
    EXPECT_NEAR(132.055f, quadpoints.x4, 0.001f);
    EXPECT_NEAR(704.796f, quadpoints.y4, 0.001f);

    // Check that updating the attachment points would succeed.
    quadpoints.x1 -= 50.f;
    quadpoints.x2 -= 50.f;
    quadpoints.x3 -= 50.f;
    quadpoints.x4 -= 50.f;
    ASSERT_TRUE(FPDFAnnot_SetAttachmentPoints(annot.get(), 0, &quadpoints));
    FS_QUADPOINTSF new_quadpoints;
    ASSERT_TRUE(FPDFAnnot_GetAttachmentPoints(annot.get(), 0, &new_quadpoints));
    EXPECT_EQ(quadpoints.x1, new_quadpoints.x1);
    EXPECT_EQ(quadpoints.y1, new_quadpoints.y1);
    EXPECT_EQ(quadpoints.x4, new_quadpoints.x4);
    EXPECT_EQ(quadpoints.y4, new_quadpoints.y4);

    // Check that updating quadpoints does not change the annotation's position.
    {
      ScopedFPDFBitmap bitmap = RenderLoadedPageWithFlags(page, FPDF_ANNOT);
      CompareBitmap(bitmap.get(), 612, 792, md5_original);
    }

    // Verify its annotation rectangle.
    ASSERT_TRUE(FPDFAnnot_GetRect(annot.get(), &rect));
    EXPECT_NEAR(67.7299f, rect.left, 0.001f);
    EXPECT_NEAR(704.296f, rect.bottom, 0.001f);
    EXPECT_NEAR(136.325f, rect.right, 0.001f);
    EXPECT_NEAR(721.292f, rect.top, 0.001f);

    // Check that updating the rectangle would succeed.
    rect.left -= 60.f;
    rect.right -= 60.f;
    ASSERT_TRUE(FPDFAnnot_SetRect(annot.get(), &rect));
    ASSERT_TRUE(FPDFAnnot_GetRect(annot.get(), &new_rect));
    EXPECT_EQ(rect.right, new_rect.right);
  }

  // Check that updating the rectangle changes the annotation's position.
  {
    ScopedFPDFBitmap bitmap = RenderLoadedPageWithFlags(page, FPDF_ANNOT);
    CompareBitmap(bitmap.get(), 612, 792, md5_modified_highlight);
  }

  {
    // Retrieve the square annotation which has its AP stream already defined.
    ScopedFPDFAnnotation annot(FPDFPage_GetAnnot(page, 2));
    ASSERT_TRUE(annot);
    EXPECT_EQ(FPDF_ANNOT_SQUARE, FPDFAnnot_GetSubtype(annot.get()));

    // Check that updating the rectangle would succeed.
    ASSERT_TRUE(FPDFAnnot_GetRect(annot.get(), &rect));
    rect.left += 70.f;
    rect.right += 70.f;
    ASSERT_TRUE(FPDFAnnot_SetRect(annot.get(), &rect));
    ASSERT_TRUE(FPDFAnnot_GetRect(annot.get(), &new_rect));
    EXPECT_EQ(rect.right, new_rect.right);

    // Check that updating the rectangle changes the square annotation's
    // position.
    ScopedFPDFBitmap bitmap = RenderLoadedPageWithFlags(page, FPDF_ANNOT);
    CompareBitmap(bitmap.get(), 612, 792, md5_modified_square);
  }

  UnloadPage(page);
}

TEST_F(FPDFAnnotEmbedderTest, CountAttachmentPoints) {
  // Open a file with multiline markup annotations.
  ASSERT_TRUE(OpenDocument("annotation_markup_multiline_no_ap.pdf"));
  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);
  {
    ScopedFPDFAnnotation annot(FPDFPage_GetAnnot(page, 0));
    ASSERT_TRUE(annot);

    // This is a three line annotation.
    EXPECT_EQ(3u, FPDFAnnot_CountAttachmentPoints(annot.get()));
  }
  UnloadPage(page);

  // null annotation should return 0
  EXPECT_EQ(0u, FPDFAnnot_CountAttachmentPoints(nullptr));
}

TEST_F(FPDFAnnotEmbedderTest, RemoveAnnotation) {
  // Open a file with 3 annotations on its first page.
  ASSERT_TRUE(OpenDocument("annotation_ink_multiple.pdf"));
  FPDF_PAGE page = LoadPageNoEvents(0);
  ASSERT_TRUE(page);
  EXPECT_EQ(3, FPDFPage_GetAnnotCount(page));

  FS_RECTF rect;

  // Check that the annotations have the expected rectangle coordinates.
  {
    ScopedFPDFAnnotation annot(FPDFPage_GetAnnot(page, 0));
    ASSERT_TRUE(FPDFAnnot_GetRect(annot.get(), &rect));
    EXPECT_NEAR(86.1971f, rect.left, 0.001f);
  }

  {
    ScopedFPDFAnnotation annot(FPDFPage_GetAnnot(page, 1));
    ASSERT_TRUE(FPDFAnnot_GetRect(annot.get(), &rect));
    EXPECT_NEAR(149.8127f, rect.left, 0.001f);
  }

  {
    ScopedFPDFAnnotation annot(FPDFPage_GetAnnot(page, 2));
    ASSERT_TRUE(FPDFAnnot_GetRect(annot.get(), &rect));
    EXPECT_NEAR(351.8204f, rect.left, 0.001f);
  }

  // Check that nothing happens when attempting to remove an annotation with an
  // out-of-bound index.
  EXPECT_FALSE(FPDFPage_RemoveAnnot(page, 4));
  EXPECT_FALSE(FPDFPage_RemoveAnnot(page, -1));
  EXPECT_EQ(3, FPDFPage_GetAnnotCount(page));

  // Remove the second annotation.
  EXPECT_TRUE(FPDFPage_RemoveAnnot(page, 1));
  EXPECT_EQ(2, FPDFPage_GetAnnotCount(page));
  EXPECT_FALSE(FPDFPage_GetAnnot(page, 2));

  // Save the document, closing the page and document.
  EXPECT_TRUE(FPDF_SaveAsCopy(document(), this, 0));
  UnloadPageNoEvents(page);

  // TODO(npm): VerifySavedRendering changes annot rect dimensions by 1??
  // Open the saved document.
  std::string new_file = GetString();
  FPDF_FILEACCESS file_access;
  memset(&file_access, 0, sizeof(file_access));
  file_access.m_FileLen = new_file.size();
  file_access.m_GetBlock = GetBlockFromString;
  file_access.m_Param = &new_file;
  FPDF_DOCUMENT new_doc = FPDF_LoadCustomDocument(&file_access, nullptr);
  ASSERT_TRUE(new_doc);
  FPDF_PAGE new_page = FPDF_LoadPage(new_doc, 0);
  ASSERT_TRUE(new_page);

  // Check that the saved document has 2 annotations on the first page.
  EXPECT_EQ(2, FPDFPage_GetAnnotCount(new_page));

  // Check that the remaining 2 annotations are the original 1st and 3rd ones
  // by verifying their rectangle coordinates.
  {
    ScopedFPDFAnnotation annot(FPDFPage_GetAnnot(new_page, 0));
    ASSERT_TRUE(FPDFAnnot_GetRect(annot.get(), &rect));
    EXPECT_NEAR(86.1971f, rect.left, 0.001f);
  }

  {
    ScopedFPDFAnnotation annot(FPDFPage_GetAnnot(new_page, 1));
    ASSERT_TRUE(FPDFAnnot_GetRect(annot.get(), &rect));
    EXPECT_NEAR(351.8204f, rect.left, 0.001f);
  }
  FPDF_ClosePage(new_page);
  FPDF_CloseDocument(new_doc);
}

TEST_F(FPDFAnnotEmbedderTest, AddAndModifyPath) {
#if _FX_PLATFORM_ == _FX_PLATFORM_APPLE_
  const char md5_original[] = "c35408717759562d1f8bf33d317483d2";
  const char md5_modified_path[] = "873b92ea83ccf006e58415d866ce145b";
  const char md5_two_paths[] = "6f1f1c91f50240e9cc9d7c87c48b93a7";
  const char md5_new_annot[] = "078bf58f939645ac305854f31ee9a828";
#elif _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_
  const char md5_original[] = "6f3cc2dd37479ce7cc072bfb0c63c275";
  const char md5_modified_path[] = "c66293426cbf1f568502d1f7c0728fc7";
  const char md5_two_paths[] = "122ac4625393b1dfe4be8707b73cbb13";
  const char md5_new_annot[] = "253c7fd739646ba2568e1e8bfc782336";
#else
  const char md5_original[] = "964f89bbe8911e540a465cf1a64b7f7e";
  const char md5_modified_path[] = "5a4a6091cff648a4ece3ce7e245e3e38";
  const char md5_two_paths[] = "d6e4072a4415cfc6ec17201fb6be0ee0";
  const char md5_new_annot[] = "fc338b97bf66a656916c6198697a8a28";
#endif

  // Open a file with two annotations and load its first page.
  ASSERT_TRUE(OpenDocument("annotation_stamp_with_ap.pdf"));
  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);
  EXPECT_EQ(2, FPDFPage_GetAnnotCount(page));

  // Check that the page renders correctly.
  {
    ScopedFPDFBitmap bitmap = RenderLoadedPageWithFlags(page, FPDF_ANNOT);
    CompareBitmap(bitmap.get(), 595, 842, md5_original);
  }

  {
    // Retrieve the stamp annotation which has its AP stream already defined.
    ScopedFPDFAnnotation annot(FPDFPage_GetAnnot(page, 0));
    ASSERT_TRUE(annot);

    // Check that this annotation has one path object and retrieve it.
    EXPECT_EQ(1, FPDFAnnot_GetObjectCount(annot.get()));
    ASSERT_EQ(32, FPDFPage_CountObjects(page));
    FPDF_PAGEOBJECT path = FPDFAnnot_GetObject(annot.get(), 1);
    EXPECT_FALSE(path);
    path = FPDFAnnot_GetObject(annot.get(), 0);
    EXPECT_EQ(FPDF_PAGEOBJ_PATH, FPDFPageObj_GetType(path));
    EXPECT_TRUE(path);

    // Modify the color of the path object.
    EXPECT_TRUE(FPDFPath_SetStrokeColor(path, 0, 0, 0, 255));
    EXPECT_TRUE(FPDFAnnot_UpdateObject(annot.get(), path));

    // Check that the page with the modified annotation renders correctly.
    {
      ScopedFPDFBitmap bitmap = RenderLoadedPageWithFlags(page, FPDF_ANNOT);
      CompareBitmap(bitmap.get(), 595, 842, md5_modified_path);
    }

    // Add a second path object to the same annotation.
    FPDF_PAGEOBJECT dot = FPDFPageObj_CreateNewPath(7, 84);
    EXPECT_TRUE(FPDFPath_BezierTo(dot, 9, 86, 10, 87, 11, 88));
    EXPECT_TRUE(FPDFPath_SetStrokeColor(dot, 255, 0, 0, 100));
    EXPECT_TRUE(FPDFPath_SetStrokeWidth(dot, 14));
    EXPECT_TRUE(FPDFPath_SetDrawMode(dot, 0, 1));
    EXPECT_TRUE(FPDFAnnot_AppendObject(annot.get(), dot));
    EXPECT_EQ(2, FPDFAnnot_GetObjectCount(annot.get()));

    // The object is in the annontation, not in the page, so the page object
    // array should not change.
    ASSERT_EQ(32, FPDFPage_CountObjects(page));

    // Check that the page with an annotation with two paths renders correctly.
    {
      ScopedFPDFBitmap bitmap = RenderLoadedPageWithFlags(page, FPDF_ANNOT);
      CompareBitmap(bitmap.get(), 595, 842, md5_two_paths);
    }

    // Delete the newly added path object.
    EXPECT_TRUE(FPDFAnnot_RemoveObject(annot.get(), 1));
    EXPECT_EQ(1, FPDFAnnot_GetObjectCount(annot.get()));
    ASSERT_EQ(32, FPDFPage_CountObjects(page));
  }

  // Check that the page renders the same as before.
  {
    ScopedFPDFBitmap bitmap = RenderLoadedPageWithFlags(page, FPDF_ANNOT);
    CompareBitmap(bitmap.get(), 595, 842, md5_modified_path);
  }

  FS_RECTF rect;

  {
    // Create another stamp annotation and set its annotation rectangle.
    ScopedFPDFAnnotation annot(FPDFPage_CreateAnnot(page, FPDF_ANNOT_STAMP));
    ASSERT_TRUE(annot);
    rect.left = 200.f;
    rect.bottom = 400.f;
    rect.right = 500.f;
    rect.top = 600.f;
    EXPECT_TRUE(FPDFAnnot_SetRect(annot.get(), &rect));

    // Add a new path to the annotation.
    FPDF_PAGEOBJECT check = FPDFPageObj_CreateNewPath(200, 500);
    EXPECT_TRUE(FPDFPath_LineTo(check, 300, 400));
    EXPECT_TRUE(FPDFPath_LineTo(check, 500, 600));
    EXPECT_TRUE(FPDFPath_MoveTo(check, 350, 550));
    EXPECT_TRUE(FPDFPath_LineTo(check, 450, 450));
    EXPECT_TRUE(FPDFPath_SetStrokeColor(check, 0, 255, 255, 180));
    EXPECT_TRUE(FPDFPath_SetStrokeWidth(check, 8.35f));
    EXPECT_TRUE(FPDFPath_SetDrawMode(check, 0, 1));
    EXPECT_TRUE(FPDFAnnot_AppendObject(annot.get(), check));
    EXPECT_EQ(1, FPDFAnnot_GetObjectCount(annot.get()));

    // Check that the annotation's bounding box came from its rectangle.
    FS_RECTF new_rect;
    ASSERT_TRUE(FPDFAnnot_GetRect(annot.get(), &new_rect));
    EXPECT_EQ(rect.left, new_rect.left);
    EXPECT_EQ(rect.bottom, new_rect.bottom);
    EXPECT_EQ(rect.right, new_rect.right);
    EXPECT_EQ(rect.top, new_rect.top);
  }

  // Save the document, closing the page and document.
  EXPECT_TRUE(FPDF_SaveAsCopy(document(), this, 0));
  UnloadPage(page);

  // Open the saved document.
  OpenSavedDocument(nullptr);
  page = LoadSavedPage(0);
  VerifySavedRendering(page, 595, 842, md5_new_annot);

  // Check that the document has a correct count of annotations and objects.
  EXPECT_EQ(3, FPDFPage_GetAnnotCount(page));

  {
    ScopedFPDFAnnotation annot(FPDFPage_GetAnnot(page, 2));
    ASSERT_TRUE(annot);
    EXPECT_EQ(1, FPDFAnnot_GetObjectCount(annot.get()));

    // Check that the new annotation's rectangle is as defined.
    FS_RECTF new_rect;
    ASSERT_TRUE(FPDFAnnot_GetRect(annot.get(), &new_rect));
    EXPECT_EQ(rect.left, new_rect.left);
    EXPECT_EQ(rect.bottom, new_rect.bottom);
    EXPECT_EQ(rect.right, new_rect.right);
    EXPECT_EQ(rect.top, new_rect.top);
  }

  CloseSavedPage(page);
  CloseSavedDocument();
}

TEST_F(FPDFAnnotEmbedderTest, ModifyAnnotationFlags) {
  // Open a file with an annotation and load its first page.
  ASSERT_TRUE(OpenDocument("annotation_highlight_rollover_ap.pdf"));
  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);

  // Check that the page renders correctly.
  {
    ScopedFPDFBitmap bitmap = RenderLoadedPageWithFlags(page, FPDF_ANNOT);
    CompareBitmap(bitmap.get(), 612, 792, "dc98f06da047bd8aabfa99562d2cbd1e");
  }

  {
    // Retrieve the annotation.
    ScopedFPDFAnnotation annot(FPDFPage_GetAnnot(page, 0));
    ASSERT_TRUE(annot);

    // Check that the original flag values are as expected.
    int flags = FPDFAnnot_GetFlags(annot.get());
    EXPECT_FALSE(flags & FPDF_ANNOT_FLAG_HIDDEN);
    EXPECT_TRUE(flags & FPDF_ANNOT_FLAG_PRINT);

    // Set the HIDDEN flag.
    flags |= FPDF_ANNOT_FLAG_HIDDEN;
    EXPECT_TRUE(FPDFAnnot_SetFlags(annot.get(), flags));
    flags = FPDFAnnot_GetFlags(annot.get());
    EXPECT_TRUE(flags & FPDF_ANNOT_FLAG_HIDDEN);
    EXPECT_TRUE(flags & FPDF_ANNOT_FLAG_PRINT);

    // Check that the page renders correctly without rendering the annotation.
    {
      ScopedFPDFBitmap bitmap = RenderLoadedPageWithFlags(page, FPDF_ANNOT);
      CompareBitmap(bitmap.get(), 612, 792, "1940568c9ba33bac5d0b1ee9558c76b3");
    }

    // Unset the HIDDEN flag.
    EXPECT_TRUE(FPDFAnnot_SetFlags(annot.get(), FPDF_ANNOT_FLAG_NONE));
    EXPECT_FALSE(FPDFAnnot_GetFlags(annot.get()));
    flags &= ~FPDF_ANNOT_FLAG_HIDDEN;
    EXPECT_TRUE(FPDFAnnot_SetFlags(annot.get(), flags));
    flags = FPDFAnnot_GetFlags(annot.get());
    EXPECT_FALSE(flags & FPDF_ANNOT_FLAG_HIDDEN);
    EXPECT_TRUE(flags & FPDF_ANNOT_FLAG_PRINT);

    // Check that the page renders correctly as before.
    {
      ScopedFPDFBitmap bitmap = RenderLoadedPageWithFlags(page, FPDF_ANNOT);
      CompareBitmap(bitmap.get(), 612, 792, "dc98f06da047bd8aabfa99562d2cbd1e");
    }
  }

  UnloadPage(page);
}

TEST_F(FPDFAnnotEmbedderTest, AddAndModifyImage) {
#if _FX_PLATFORM_ == _FX_PLATFORM_APPLE_
  const char md5_original[] = "c35408717759562d1f8bf33d317483d2";
  const char md5_new_image[] = "ff012f5697436dfcaec25b32d1333596";
  const char md5_modified_image[] = "86cf8cb2755a7a2046a543e66d9c1e61";
#elif _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_
  const char md5_original[] = "6f3cc2dd37479ce7cc072bfb0c63c275";
  const char md5_new_image[] = "d19c6fcfd9a170802fcfb9adfa13557e";
  const char md5_modified_image[] = "1273cf2363570a50d1aa0c95b1318197";
#else
  const char md5_original[] = "964f89bbe8911e540a465cf1a64b7f7e";
  const char md5_new_image[] = "9ea8732dc9d579f68853f16892856208";
  const char md5_modified_image[] = "74239d2a8c55c9de1dbb9cd8781895aa";
#endif

  // Open a file with two annotations and load its first page.
  ASSERT_TRUE(OpenDocument("annotation_stamp_with_ap.pdf"));
  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);
  EXPECT_EQ(2, FPDFPage_GetAnnotCount(page));

  // Check that the page renders correctly.
  {
    ScopedFPDFBitmap bitmap = RenderLoadedPageWithFlags(page, FPDF_ANNOT);
    CompareBitmap(bitmap.get(), 595, 842, md5_original);
  }

  constexpr int kBitmapSize = 200;
  FPDF_BITMAP image_bitmap;

  {
    // Create a stamp annotation and set its annotation rectangle.
    ScopedFPDFAnnotation annot(FPDFPage_CreateAnnot(page, FPDF_ANNOT_STAMP));
    ASSERT_TRUE(annot);
    FS_RECTF rect;
    rect.left = 200.f;
    rect.bottom = 600.f;
    rect.right = 400.f;
    rect.top = 800.f;
    EXPECT_TRUE(FPDFAnnot_SetRect(annot.get(), &rect));

    // Add a solid-color translucent image object to the new annotation.
    image_bitmap = FPDFBitmap_Create(kBitmapSize, kBitmapSize, 1);
    FPDFBitmap_FillRect(image_bitmap, 0, 0, kBitmapSize, kBitmapSize,
                        0xeeeecccc);
    EXPECT_EQ(kBitmapSize, FPDFBitmap_GetWidth(image_bitmap));
    EXPECT_EQ(kBitmapSize, FPDFBitmap_GetHeight(image_bitmap));
    FPDF_PAGEOBJECT image_object = FPDFPageObj_NewImageObj(document());
    ASSERT_TRUE(FPDFImageObj_SetBitmap(&page, 0, image_object, image_bitmap));
    ASSERT_TRUE(FPDFImageObj_SetMatrix(image_object, kBitmapSize, 0, 0,
                                       kBitmapSize, 0, 0));
    FPDFPageObj_Transform(image_object, 1, 0, 0, 1, 200, 600);
    EXPECT_TRUE(FPDFAnnot_AppendObject(annot.get(), image_object));
  }

  // Check that the page renders correctly with the new image object.
  {
    ScopedFPDFBitmap bitmap = RenderLoadedPageWithFlags(page, FPDF_ANNOT);
    CompareBitmap(bitmap.get(), 595, 842, md5_new_image);
  }

  {
    // Retrieve the newly added stamp annotation and its image object.
    ScopedFPDFAnnotation annot(FPDFPage_GetAnnot(page, 2));
    ASSERT_TRUE(annot);
    EXPECT_EQ(1, FPDFAnnot_GetObjectCount(annot.get()));
    FPDF_PAGEOBJECT image_object = FPDFAnnot_GetObject(annot.get(), 0);
    EXPECT_EQ(FPDF_PAGEOBJ_IMAGE, FPDFPageObj_GetType(image_object));

    // Modify the image in the new annotation.
    FPDFBitmap_FillRect(image_bitmap, 0, 0, kBitmapSize, kBitmapSize,
                        0xff000000);
    ASSERT_TRUE(FPDFImageObj_SetBitmap(&page, 0, image_object, image_bitmap));
    EXPECT_TRUE(FPDFAnnot_UpdateObject(annot.get(), image_object));
  }

  // Save the document, closing the page and document.
  EXPECT_TRUE(FPDF_SaveAsCopy(document(), this, 0));
  UnloadPage(page);
  FPDFBitmap_Destroy(image_bitmap);

  // Test that the saved document renders the modified image object correctly.
  VerifySavedDocument(595, 842, md5_modified_image);
}

TEST_F(FPDFAnnotEmbedderTest, AddAndModifyText) {
#if _FX_PLATFORM_ == _FX_PLATFORM_APPLE_
  const char md5_original[] = "c35408717759562d1f8bf33d317483d2";
  const char md5_new_text[] = "e5680ed048c2cfd9a1d27212cdf41286";
  const char md5_modified_text[] = "79f5cfb0b07caaf936f65f6a7a57ce77";
#elif _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_
  const char md5_original[] = "6f3cc2dd37479ce7cc072bfb0c63c275";
  const char md5_new_text[] = "554d625b52144816aaabb0dd66962c55";
  const char md5_modified_text[] = "26e94fbd3af4b1e65479327507600114";
#else
  const char md5_original[] = "964f89bbe8911e540a465cf1a64b7f7e";
  const char md5_new_text[] = "00b14fa2dc1c90d1b0d034e1608efef5";
  const char md5_modified_text[] = "076c8f24a09ddc0e49f7e758edead6f0";
#endif

  // Open a file with two annotations and load its first page.
  ASSERT_TRUE(OpenDocument("annotation_stamp_with_ap.pdf"));
  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);
  EXPECT_EQ(2, FPDFPage_GetAnnotCount(page));

  // Check that the page renders correctly.
  {
    ScopedFPDFBitmap bitmap = RenderLoadedPageWithFlags(page, FPDF_ANNOT);
    CompareBitmap(bitmap.get(), 595, 842, md5_original);
  }

  {
    // Create a stamp annotation and set its annotation rectangle.
    ScopedFPDFAnnotation annot(FPDFPage_CreateAnnot(page, FPDF_ANNOT_STAMP));
    ASSERT_TRUE(annot);
    FS_RECTF rect;
    rect.left = 200.f;
    rect.bottom = 550.f;
    rect.right = 450.f;
    rect.top = 650.f;
    EXPECT_TRUE(FPDFAnnot_SetRect(annot.get(), &rect));

    // Add a translucent text object to the new annotation.
    FPDF_PAGEOBJECT text_object =
        FPDFPageObj_NewTextObj(document(), "Arial", 12.0f);
    EXPECT_TRUE(text_object);
    std::unique_ptr<unsigned short, pdfium::FreeDeleter> text =
        GetFPDFWideString(L"I'm a translucent text laying on other text.");
    EXPECT_TRUE(FPDFText_SetText(text_object, text.get()));
    EXPECT_TRUE(FPDFText_SetFillColor(text_object, 0, 0, 255, 150));
    FPDFPageObj_Transform(text_object, 1, 0, 0, 1, 200, 600);
    EXPECT_TRUE(FPDFAnnot_AppendObject(annot.get(), text_object));
  }

  // Check that the page renders correctly with the new text object.
  {
    ScopedFPDFBitmap bitmap = RenderLoadedPageWithFlags(page, FPDF_ANNOT);
    CompareBitmap(bitmap.get(), 595, 842, md5_new_text);
  }

  {
    // Retrieve the newly added stamp annotation and its text object.
    ScopedFPDFAnnotation annot(FPDFPage_GetAnnot(page, 2));
    ASSERT_TRUE(annot);
    EXPECT_EQ(1, FPDFAnnot_GetObjectCount(annot.get()));
    FPDF_PAGEOBJECT text_object = FPDFAnnot_GetObject(annot.get(), 0);
    EXPECT_EQ(FPDF_PAGEOBJ_TEXT, FPDFPageObj_GetType(text_object));

    // Modify the text in the new annotation.
    std::unique_ptr<unsigned short, pdfium::FreeDeleter> new_text =
        GetFPDFWideString(L"New text!");
    EXPECT_TRUE(FPDFText_SetText(text_object, new_text.get()));
    EXPECT_TRUE(FPDFAnnot_UpdateObject(annot.get(), text_object));
  }

  // Check that the page renders correctly with the modified text object.
  {
    ScopedFPDFBitmap bitmap = RenderLoadedPageWithFlags(page, FPDF_ANNOT);
    CompareBitmap(bitmap.get(), 595, 842, md5_modified_text);
  }

  // Remove the new annotation, and check that the page renders as before.
  EXPECT_TRUE(FPDFPage_RemoveAnnot(page, 2));
  {
    ScopedFPDFBitmap bitmap = RenderLoadedPageWithFlags(page, FPDF_ANNOT);
    CompareBitmap(bitmap.get(), 595, 842, md5_original);
  }

  UnloadPage(page);
}

TEST_F(FPDFAnnotEmbedderTest, GetSetStringValue) {
  // Open a file with four annotations and load its first page.
  ASSERT_TRUE(OpenDocument("annotation_stamp_with_ap.pdf"));
  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);

  static constexpr char kDateKey[] = "M";
  static constexpr wchar_t kNewDate[] = L"D:201706282359Z00'00'";

  {
    // Retrieve the first annotation.
    ScopedFPDFAnnotation annot(FPDFPage_GetAnnot(page, 0));
    ASSERT_TRUE(annot);

    // Check that a non-existent key does not exist.
    EXPECT_FALSE(FPDFAnnot_HasKey(annot.get(), "none"));

    // Check that the string value of a non-string dictionary entry is empty.
    static constexpr char kApKey[] = "AP";
    EXPECT_TRUE(FPDFAnnot_HasKey(annot.get(), kApKey));
    EXPECT_EQ(FPDF_OBJECT_REFERENCE,
              FPDFAnnot_GetValueType(annot.get(), kApKey));
    EXPECT_EQ(2u, FPDFAnnot_GetStringValue(annot.get(), kApKey, nullptr, 0));

    // Check that the string value of the hash is correct.
    static constexpr char kHashKey[] = "AAPL:Hash";
    EXPECT_EQ(FPDF_OBJECT_NAME, FPDFAnnot_GetValueType(annot.get(), kHashKey));
    unsigned long len =
        FPDFAnnot_GetStringValue(annot.get(), kHashKey, nullptr, 0);
    std::vector<char> buf(len);
    EXPECT_EQ(66u,
              FPDFAnnot_GetStringValue(annot.get(), kHashKey, buf.data(), len));
    EXPECT_STREQ(L"395fbcb98d558681742f30683a62a2ad",
                 BufferToWString(buf).c_str());

    // Check that the string value of the modified date is correct.
    EXPECT_EQ(FPDF_OBJECT_NAME, FPDFAnnot_GetValueType(annot.get(), kHashKey));
    len = FPDFAnnot_GetStringValue(annot.get(), kDateKey, nullptr, 0);
    buf.clear();
    buf.resize(len);
    EXPECT_EQ(44u,
              FPDFAnnot_GetStringValue(annot.get(), kDateKey, buf.data(), len));
    EXPECT_STREQ(L"D:201706071721Z00'00'", BufferToWString(buf).c_str());

    // Update the date entry for the annotation.
    std::unique_ptr<unsigned short, pdfium::FreeDeleter> text =
        GetFPDFWideString(kNewDate);
    EXPECT_TRUE(FPDFAnnot_SetStringValue(annot.get(), kDateKey, text.get()));
  }

  // Save the document, closing the page and document.
  EXPECT_TRUE(FPDF_SaveAsCopy(document(), this, 0));
  UnloadPage(page);

#if _FX_PLATFORM_ == _FX_PLATFORM_APPLE_
  const char md5[] = "4d64e61c9c0f8c60ab3cc3234bb73b1c";
#elif _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_
  const char md5[] = "9ee141f698c3fcb56c050dffd6c82624";
#else
  const char md5[] = "c96ee1f316d7f5a1b154de9f9d467f01";
#endif

  // Open the saved annotation.
  OpenSavedDocument(nullptr);
  page = LoadSavedPage(0);
  VerifySavedRendering(page, 595, 842, md5);
  {
    ScopedFPDFAnnotation new_annot(FPDFPage_GetAnnot(page, 0));

    // Check that the string value of the modified date is the newly-set value.
    EXPECT_EQ(FPDF_OBJECT_STRING,
              FPDFAnnot_GetValueType(new_annot.get(), kDateKey));
    unsigned long len =
        FPDFAnnot_GetStringValue(new_annot.get(), kDateKey, nullptr, 0);
    std::vector<char> buf(len);
    EXPECT_EQ(44u, FPDFAnnot_GetStringValue(new_annot.get(), kDateKey,
                                            buf.data(), len));
    EXPECT_STREQ(kNewDate, BufferToWString(buf).c_str());
  }

  CloseSavedPage(page);
  CloseSavedDocument();
}

TEST_F(FPDFAnnotEmbedderTest, GetSetAP) {
  // Open a file with four annotations and load its first page.
  ASSERT_TRUE(OpenDocument("annotation_stamp_with_ap.pdf"));
  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);

  {
    // Retrieve the first annotation.
    ScopedFPDFAnnotation annot(FPDFPage_GetAnnot(page, 0));
    ASSERT_TRUE(annot);

    // Check that the string value of an AP returns the expected length.
    unsigned long normal_len = FPDFAnnot_GetAP(
        annot.get(), FPDF_ANNOT_APPEARANCEMODE_NORMAL, nullptr, 0);
    EXPECT_EQ(73970u, normal_len);

    // Check that the string value of an AP is not returned if the buffer is too
    // small. The result buffer should be overwritten with an empty string.
    std::vector<char> buf(normal_len - 1);
    // Write L"z" in the buffer to verify it's not overwritten.
    wcscpy(reinterpret_cast<wchar_t*>(buf.data()), L"z");
    EXPECT_EQ(73970u,
              FPDFAnnot_GetAP(annot.get(), FPDF_ANNOT_APPEARANCEMODE_NORMAL,
                              buf.data(), buf.size()));
    std::string ap = BufferToString(buf);
    EXPECT_STREQ("z", ap.c_str());

    // Check that the string value of an AP is returned through a buffer that is
    // the right size.
    buf.clear();
    buf.resize(normal_len);
    EXPECT_EQ(73970u,
              FPDFAnnot_GetAP(annot.get(), FPDF_ANNOT_APPEARANCEMODE_NORMAL,
                              buf.data(), buf.size()));
    ap = BufferToString(buf);
    EXPECT_THAT(ap, testing::StartsWith("q Q q 7.442786 w 2 J"));
    EXPECT_THAT(ap, testing::EndsWith("c 716.5381 327.7156 l S Q Q"));

    // Check that the string value of an AP is returned through a buffer that is
    // larger than necessary.
    buf.clear();
    buf.resize(normal_len + 1);
    EXPECT_EQ(73970u,
              FPDFAnnot_GetAP(annot.get(), FPDF_ANNOT_APPEARANCEMODE_NORMAL,
                              buf.data(), buf.size()));
    ap = BufferToString(buf);
    EXPECT_THAT(ap, testing::StartsWith("q Q q 7.442786 w 2 J"));
    EXPECT_THAT(ap, testing::EndsWith("c 716.5381 327.7156 l S Q Q"));

    // Check that getting an AP for a mode that does not have an AP returns an
    // empty string.
    unsigned long rollover_len = FPDFAnnot_GetAP(
        annot.get(), FPDF_ANNOT_APPEARANCEMODE_ROLLOVER, nullptr, 0);
    EXPECT_EQ(2u, rollover_len);

    buf.clear();
    buf.resize(1000);
    EXPECT_EQ(2u,
              FPDFAnnot_GetAP(annot.get(), FPDF_ANNOT_APPEARANCEMODE_ROLLOVER,
                              buf.data(), buf.size()));
    EXPECT_STREQ("", BufferToString(buf).c_str());

    // Check that setting the AP for an invalid appearance mode fails.
    std::unique_ptr<unsigned short, pdfium::FreeDeleter> apText =
        GetFPDFWideString(L"new test ap");
    EXPECT_FALSE(FPDFAnnot_SetAP(annot.get(), -1, apText.get()));
    EXPECT_FALSE(FPDFAnnot_SetAP(annot.get(), FPDF_ANNOT_APPEARANCEMODE_COUNT,
                                 apText.get()));
    EXPECT_FALSE(FPDFAnnot_SetAP(
        annot.get(), FPDF_ANNOT_APPEARANCEMODE_COUNT + 1, apText.get()));

    // Set the AP correctly now.
    EXPECT_TRUE(FPDFAnnot_SetAP(annot.get(), FPDF_ANNOT_APPEARANCEMODE_ROLLOVER,
                                apText.get()));

    // Check that the new annotation value is equal to the value we just set.
    rollover_len = FPDFAnnot_GetAP(
        annot.get(), FPDF_ANNOT_APPEARANCEMODE_ROLLOVER, nullptr, 0);
    EXPECT_EQ(24u, rollover_len);
    buf.clear();
    buf.resize(rollover_len);
    EXPECT_EQ(24u,
              FPDFAnnot_GetAP(annot.get(), FPDF_ANNOT_APPEARANCEMODE_ROLLOVER,
                              buf.data(), buf.size()));
    EXPECT_STREQ(L"new test ap", BufferToWString(buf).c_str());

    // Check that the Normal AP was not touched when the Rollover AP was set.
    buf.clear();
    buf.resize(normal_len);
    EXPECT_EQ(73970u,
              FPDFAnnot_GetAP(annot.get(), FPDF_ANNOT_APPEARANCEMODE_NORMAL,
                              buf.data(), buf.size()));
    ap = BufferToString(buf);
    EXPECT_THAT(ap, testing::StartsWith("q Q q 7.442786 w 2 J"));
    EXPECT_THAT(ap, testing::EndsWith("c 716.5381 327.7156 l S Q Q"));
  }

  // Save the modified document, then reopen it.
  EXPECT_TRUE(FPDF_SaveAsCopy(document(), this, 0));
  UnloadPage(page);

  OpenSavedDocument(nullptr);
  page = LoadSavedPage(0);
  {
    ScopedFPDFAnnotation new_annot(FPDFPage_GetAnnot(page, 0));

    // Check that the new annotation value is equal to the value we set before
    // saving.
    unsigned long rollover_len = FPDFAnnot_GetAP(
        new_annot.get(), FPDF_ANNOT_APPEARANCEMODE_ROLLOVER, nullptr, 0);
    EXPECT_EQ(24u, rollover_len);
    std::vector<char> buf(rollover_len);
    EXPECT_EQ(24u, FPDFAnnot_GetAP(new_annot.get(),
                                   FPDF_ANNOT_APPEARANCEMODE_ROLLOVER,
                                   buf.data(), buf.size()));
    EXPECT_STREQ(L"new test ap", BufferToWString(buf).c_str());
  }

  // Close saved document.
  CloseSavedPage(page);
  CloseSavedDocument();
}

TEST_F(FPDFAnnotEmbedderTest, RemoveOptionalAP) {
  // Open a file with four annotations and load its first page.
  ASSERT_TRUE(OpenDocument("annotation_stamp_with_ap.pdf"));
  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);

  {
    // Retrieve the first annotation.
    ScopedFPDFAnnotation annot(FPDFPage_GetAnnot(page, 0));
    ASSERT_TRUE(annot);

    // Set Down AP. Normal AP is already set.
    std::unique_ptr<unsigned short, pdfium::FreeDeleter> apText =
        GetFPDFWideString(L"new test ap");
    EXPECT_TRUE(FPDFAnnot_SetAP(annot.get(), FPDF_ANNOT_APPEARANCEMODE_DOWN,
                                apText.get()));
    EXPECT_EQ(73970u,
              FPDFAnnot_GetAP(annot.get(), FPDF_ANNOT_APPEARANCEMODE_NORMAL,
                              nullptr, 0));
    EXPECT_EQ(24u, FPDFAnnot_GetAP(annot.get(), FPDF_ANNOT_APPEARANCEMODE_DOWN,
                                   nullptr, 0));

    // Check that setting the Down AP to null removes the Down entry but keeps
    // Normal intact.
    EXPECT_TRUE(
        FPDFAnnot_SetAP(annot.get(), FPDF_ANNOT_APPEARANCEMODE_DOWN, nullptr));
    EXPECT_EQ(73970u,
              FPDFAnnot_GetAP(annot.get(), FPDF_ANNOT_APPEARANCEMODE_NORMAL,
                              nullptr, 0));
    EXPECT_EQ(2u, FPDFAnnot_GetAP(annot.get(), FPDF_ANNOT_APPEARANCEMODE_DOWN,
                                  nullptr, 0));
  }

  UnloadPage(page);
}

TEST_F(FPDFAnnotEmbedderTest, RemoveRequiredAP) {
  // Open a file with four annotations and load its first page.
  ASSERT_TRUE(OpenDocument("annotation_stamp_with_ap.pdf"));
  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);

  {
    // Retrieve the first annotation.
    ScopedFPDFAnnotation annot(FPDFPage_GetAnnot(page, 0));
    ASSERT_TRUE(annot);

    // Set Down AP. Normal AP is already set.
    std::unique_ptr<unsigned short, pdfium::FreeDeleter> apText =
        GetFPDFWideString(L"new test ap");
    EXPECT_TRUE(FPDFAnnot_SetAP(annot.get(), FPDF_ANNOT_APPEARANCEMODE_DOWN,
                                apText.get()));
    EXPECT_EQ(73970u,
              FPDFAnnot_GetAP(annot.get(), FPDF_ANNOT_APPEARANCEMODE_NORMAL,
                              nullptr, 0));
    EXPECT_EQ(24u, FPDFAnnot_GetAP(annot.get(), FPDF_ANNOT_APPEARANCEMODE_DOWN,
                                   nullptr, 0));

    // Check that setting the Normal AP to null removes the whole AP dictionary.
    EXPECT_TRUE(FPDFAnnot_SetAP(annot.get(), FPDF_ANNOT_APPEARANCEMODE_NORMAL,
                                nullptr));
    EXPECT_EQ(2u, FPDFAnnot_GetAP(annot.get(), FPDF_ANNOT_APPEARANCEMODE_NORMAL,
                                  nullptr, 0));
    EXPECT_EQ(2u, FPDFAnnot_GetAP(annot.get(), FPDF_ANNOT_APPEARANCEMODE_DOWN,
                                  nullptr, 0));
  }

  UnloadPage(page);
}

TEST_F(FPDFAnnotEmbedderTest, ExtractLinkedAnnotations) {
  // Open a file with annotations and load its first page.
  ASSERT_TRUE(OpenDocument("annotation_highlight_square_with_ap.pdf"));
  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);
  EXPECT_EQ(-1, FPDFPage_GetAnnotIndex(page, nullptr));

  {
    // Retrieve the highlight annotation which has its popup defined.
    ScopedFPDFAnnotation annot(FPDFPage_GetAnnot(page, 0));
    ASSERT_TRUE(annot);
    EXPECT_EQ(FPDF_ANNOT_HIGHLIGHT, FPDFAnnot_GetSubtype(annot.get()));
    EXPECT_EQ(0, FPDFPage_GetAnnotIndex(page, annot.get()));
    static constexpr char kPopupKey[] = "Popup";
    ASSERT_TRUE(FPDFAnnot_HasKey(annot.get(), kPopupKey));
    ASSERT_EQ(FPDF_OBJECT_REFERENCE,
              FPDFAnnot_GetValueType(annot.get(), kPopupKey));

    // Retrieve and verify the popup of the highlight annotation.
    ScopedFPDFAnnotation popup(
        FPDFAnnot_GetLinkedAnnot(annot.get(), kPopupKey));
    ASSERT_TRUE(popup);
    EXPECT_EQ(FPDF_ANNOT_POPUP, FPDFAnnot_GetSubtype(popup.get()));
    EXPECT_EQ(1, FPDFPage_GetAnnotIndex(page, popup.get()));
    FS_RECTF rect;
    ASSERT_TRUE(FPDFAnnot_GetRect(popup.get(), &rect));
    EXPECT_NEAR(612.0f, rect.left, 0.001f);
    EXPECT_NEAR(578.792, rect.bottom, 0.001f);

    // Attempting to retrieve |annot|'s "IRT"-linked annotation would fail,
    // since "IRT" is not a key in |annot|'s dictionary.
    static constexpr char kIRTKey[] = "IRT";
    ASSERT_FALSE(FPDFAnnot_HasKey(annot.get(), kIRTKey));
    EXPECT_FALSE(FPDFAnnot_GetLinkedAnnot(annot.get(), kIRTKey));

    // Attempting to retrieve |annot|'s parent dictionary as an annotation
    // would fail, since its parent is not an annotation.
    static constexpr char kPKey[] = "P";
    ASSERT_TRUE(FPDFAnnot_HasKey(annot.get(), kPKey));
    EXPECT_EQ(FPDF_OBJECT_REFERENCE,
              FPDFAnnot_GetValueType(annot.get(), kPKey));
    EXPECT_FALSE(FPDFAnnot_GetLinkedAnnot(annot.get(), kPKey));
  }

  UnloadPage(page);
}

TEST_F(FPDFAnnotEmbedderTest, GetFormFieldFlagsTextField) {
  // Open file with form text fields.
  ASSERT_TRUE(OpenDocument("text_form_multiple.pdf"));
  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);

  {
    // Retrieve the first annotation: user-editable text field.
    ScopedFPDFAnnotation annot(FPDFPage_GetAnnot(page, 0));
    ASSERT_TRUE(annot);

    // Check that the flag values are as expected.
    int flags = FPDFAnnot_GetFormFieldFlags(page, annot.get());
    EXPECT_FALSE(flags & FPDF_FORMFLAG_READONLY);
  }

  {
    // Retrieve the second annotation: read-only text field.
    ScopedFPDFAnnotation annot(FPDFPage_GetAnnot(page, 1));
    ASSERT_TRUE(annot);

    // Check that the flag values are as expected.
    int flags = FPDFAnnot_GetFormFieldFlags(page, annot.get());
    EXPECT_TRUE(flags & FPDF_FORMFLAG_READONLY);
  }

  UnloadPage(page);
}

TEST_F(FPDFAnnotEmbedderTest, GetFormFieldFlagsComboBox) {
  // Open file with form text fields.
  ASSERT_TRUE(OpenDocument("combobox_form.pdf"));
  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);

  {
    // Retrieve the first annotation: user-editable combobox.
    ScopedFPDFAnnotation annot(FPDFPage_GetAnnot(page, 0));
    ASSERT_TRUE(annot);

    // Check that the flag values are as expected.
    int flags = FPDFAnnot_GetFormFieldFlags(page, annot.get());
    EXPECT_FALSE(flags & FPDF_FORMFLAG_READONLY);
    EXPECT_TRUE(flags & FPDF_FORMFLAG_CHOICE_COMBO);
    EXPECT_TRUE(flags & FPDF_FORMFLAG_CHOICE_EDIT);
  }

  {
    // Retrieve the second annotation: regular combobox.
    ScopedFPDFAnnotation annot(FPDFPage_GetAnnot(page, 1));
    ASSERT_TRUE(annot);

    // Check that the flag values are as expected.
    int flags = FPDFAnnot_GetFormFieldFlags(page, annot.get());
    EXPECT_FALSE(flags & FPDF_FORMFLAG_READONLY);
    EXPECT_TRUE(flags & FPDF_FORMFLAG_CHOICE_COMBO);
    EXPECT_FALSE(flags & FPDF_FORMFLAG_CHOICE_EDIT);
  }

  {
    // Retrieve the third annotation: read-only combobox.
    ScopedFPDFAnnotation annot(FPDFPage_GetAnnot(page, 2));
    ASSERT_TRUE(annot);

    // Check that the flag values are as expected.
    int flags = FPDFAnnot_GetFormFieldFlags(page, annot.get());
    EXPECT_TRUE(flags & FPDF_FORMFLAG_READONLY);
    EXPECT_TRUE(flags & FPDF_FORMFLAG_CHOICE_COMBO);
    EXPECT_FALSE(flags & FPDF_FORMFLAG_CHOICE_EDIT);
  }

  UnloadPage(page);
}

TEST_F(FPDFAnnotEmbedderTest, GetFormAnnotNull) {
  // Open file with form text fields.
  EXPECT_TRUE(OpenDocument("text_form.pdf"));
  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);

  // Attempt to get an annotation where no annotation exists on page.
  EXPECT_FALSE(FPDFAnnot_GetFormFieldAtPoint(form_handle(), page, 0, 0));

  {
    // Verify there is an annotation.
    ScopedFPDFAnnotation annot(
        FPDFAnnot_GetFormFieldAtPoint(form_handle(), page, 120, 120));
    EXPECT_TRUE(annot);
  }

  // Try other bad inputs at a valid location.
  EXPECT_FALSE(FPDFAnnot_GetFormFieldAtPoint(nullptr, nullptr, 120, 120));
  EXPECT_FALSE(FPDFAnnot_GetFormFieldAtPoint(nullptr, page, 120, 120));
  EXPECT_FALSE(FPDFAnnot_GetFormFieldAtPoint(form_handle(), nullptr, 120, 120));

  UnloadPage(page);
}

TEST_F(FPDFAnnotEmbedderTest, GetFormAnnotAndCheckFlagsTextField) {
  // Open file with form text fields.
  EXPECT_TRUE(OpenDocument("text_form_multiple.pdf"));
  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);

  {
    // Retrieve user-editable text field annotation.
    ScopedFPDFAnnotation annot(
        FPDFAnnot_GetFormFieldAtPoint(form_handle(), page, 105, 118));
    ASSERT_TRUE(annot);

    // Check that interactive form annotation flag values are as expected.
    int flags = FPDFAnnot_GetFormFieldFlags(page, annot.get());
    EXPECT_FALSE(flags & FPDF_FORMFLAG_READONLY);
  }

  {
    // Retrieve read-only text field annotation.
    ScopedFPDFAnnotation annot(
        FPDFAnnot_GetFormFieldAtPoint(form_handle(), page, 105, 202));
    ASSERT_TRUE(annot);

    // Check that interactive form annotation flag values are as expected.
    int flags = FPDFAnnot_GetFormFieldFlags(page, annot.get());
    EXPECT_TRUE(flags & FPDF_FORMFLAG_READONLY);
  }

  UnloadPage(page);
}

TEST_F(FPDFAnnotEmbedderTest, GetFormAnnotAndCheckFlagsComboBox) {
  // Open file with form comboboxes.
  EXPECT_TRUE(OpenDocument("combobox_form.pdf"));
  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);

  {
    // Retrieve user-editable combobox annotation.
    ScopedFPDFAnnotation annot(
        FPDFAnnot_GetFormFieldAtPoint(form_handle(), page, 102, 363));
    ASSERT_TRUE(annot);

    // Check that interactive form annotation flag values are as expected.
    int flags = FPDFAnnot_GetFormFieldFlags(page, annot.get());
    EXPECT_FALSE(flags & FPDF_FORMFLAG_READONLY);
    EXPECT_TRUE(flags & FPDF_FORMFLAG_CHOICE_COMBO);
    EXPECT_TRUE(flags & FPDF_FORMFLAG_CHOICE_EDIT);
  }

  {
    // Retrieve regular combobox annotation.
    ScopedFPDFAnnotation annot(
        FPDFAnnot_GetFormFieldAtPoint(form_handle(), page, 102, 413));
    ASSERT_TRUE(annot);

    // Check that interactive form annotation flag values are as expected.
    int flags = FPDFAnnot_GetFormFieldFlags(page, annot.get());
    EXPECT_FALSE(flags & FPDF_FORMFLAG_READONLY);
    EXPECT_TRUE(flags & FPDF_FORMFLAG_CHOICE_COMBO);
    EXPECT_FALSE(flags & FPDF_FORMFLAG_CHOICE_EDIT);
  }

  {
    // Retrieve read-only combobox annotation.
    ScopedFPDFAnnotation annot(
        FPDFAnnot_GetFormFieldAtPoint(form_handle(), page, 102, 513));
    ASSERT_TRUE(annot);

    // Check that interactive form annotation flag values are as expected.
    int flags = FPDFAnnot_GetFormFieldFlags(page, annot.get());
    EXPECT_TRUE(flags & FPDF_FORMFLAG_READONLY);
    EXPECT_TRUE(flags & FPDF_FORMFLAG_CHOICE_COMBO);
    EXPECT_FALSE(flags & FPDF_FORMFLAG_CHOICE_EDIT);
  }

  UnloadPage(page);
}

TEST_F(FPDFAnnotEmbedderTest, BUG_1212) {
  ASSERT_TRUE(OpenDocument("hello_world.pdf"));
  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);
  EXPECT_EQ(0, FPDFPage_GetAnnotCount(page));

  static const char kTestKey[] = "test";
  static constexpr wchar_t kData[] = L"\xf6\xe4";
  std::vector<char> buf(12);

  {
    // Add a text annotation to the page.
    ScopedFPDFAnnotation annot(FPDFPage_CreateAnnot(page, FPDF_ANNOT_TEXT));
    ASSERT_TRUE(annot);
    EXPECT_EQ(1, FPDFPage_GetAnnotCount(page));
    EXPECT_EQ(FPDF_ANNOT_TEXT, FPDFAnnot_GetSubtype(annot.get()));

    // Make sure there is no test key, add set a value there, and read it back.
    std::fill(buf.begin(), buf.end(), 'x');
    ASSERT_EQ(2u, FPDFAnnot_GetStringValue(annot.get(), kTestKey, buf.data(),
                                           buf.size()));
    EXPECT_STREQ(L"", BufferToWString(buf).c_str());

    std::unique_ptr<unsigned short, pdfium::FreeDeleter> text =
        GetFPDFWideString(kData);
    EXPECT_TRUE(FPDFAnnot_SetStringValue(annot.get(), kTestKey, text.get()));

    std::fill(buf.begin(), buf.end(), 'x');
    ASSERT_EQ(6u, FPDFAnnot_GetStringValue(annot.get(), kTestKey, buf.data(),
                                           buf.size()));
    EXPECT_STREQ(kData, BufferToWString(buf).c_str());
  }

  {
    ScopedFPDFAnnotation annot(FPDFPage_CreateAnnot(page, FPDF_ANNOT_STAMP));
    ASSERT_TRUE(annot);
    EXPECT_EQ(2, FPDFPage_GetAnnotCount(page));
    EXPECT_EQ(FPDF_ANNOT_STAMP, FPDFAnnot_GetSubtype(annot.get()));
    // Also do the same test for its appearance string.
    std::fill(buf.begin(), buf.end(), 'x');
    ASSERT_EQ(2u,
              FPDFAnnot_GetAP(annot.get(), FPDF_ANNOT_APPEARANCEMODE_ROLLOVER,
                              buf.data(), buf.size()));
    EXPECT_STREQ(L"", BufferToWString(buf).c_str());

    std::unique_ptr<unsigned short, pdfium::FreeDeleter> text =
        GetFPDFWideString(kData);
    EXPECT_TRUE(FPDFAnnot_SetAP(annot.get(), FPDF_ANNOT_APPEARANCEMODE_ROLLOVER,
                                text.get()));

    std::fill(buf.begin(), buf.end(), 'x');
    ASSERT_EQ(6u,
              FPDFAnnot_GetAP(annot.get(), FPDF_ANNOT_APPEARANCEMODE_ROLLOVER,
                              buf.data(), buf.size()));
    EXPECT_STREQ(kData, BufferToWString(buf).c_str());
  }

  UnloadPage(page);

  {
    // Save a copy, open the copy, and check the annotation again.
    // Note that it renders the rotation.
    EXPECT_TRUE(FPDF_SaveAsCopy(document(), this, 0));
    OpenSavedDocument(nullptr);
    FPDF_PAGE saved_page = LoadSavedPage(0);
    ASSERT_TRUE(saved_page);

    EXPECT_EQ(2, FPDFPage_GetAnnotCount(saved_page));
    {
      ScopedFPDFAnnotation annot(FPDFPage_GetAnnot(saved_page, 0));
      ASSERT_TRUE(annot);
      EXPECT_EQ(FPDF_ANNOT_TEXT, FPDFAnnot_GetSubtype(annot.get()));

      std::fill(buf.begin(), buf.end(), 'x');
      ASSERT_EQ(6u, FPDFAnnot_GetStringValue(annot.get(), kTestKey, buf.data(),
                                             buf.size()));
      EXPECT_STREQ(kData, BufferToWString(buf).c_str());
    }

    {
      ScopedFPDFAnnotation annot(FPDFPage_GetAnnot(saved_page, 0));
      ASSERT_TRUE(annot);
      // TODO(thestig): This return FPDF_ANNOT_UNKNOWN for some reason.
      // EXPECT_EQ(FPDF_ANNOT_TEXT, FPDFAnnot_GetSubtype(annot.get()));

      std::fill(buf.begin(), buf.end(), 'x');
      ASSERT_EQ(6u, FPDFAnnot_GetStringValue(annot.get(), kTestKey, buf.data(),
                                             buf.size()));
      EXPECT_STREQ(kData, BufferToWString(buf).c_str());
    }

    CloseSavedPage(saved_page);
    CloseSavedDocument();
  }
}
