// Copyright 2015 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <cmath>
#include <limits>
#include <memory>
#include <string>

#include "core/fpdfapi/parser/cpdf_document.h"
#include "fpdfsdk/cpdfsdk_helpers.h"
#include "fpdfsdk/fpdf_view_c_api_test.h"
#include "public/cpp/fpdf_scopers.h"
#include "public/fpdfview.h"
#include "testing/embedder_test.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "testing/utils/path_service.h"

namespace {

class MockDownloadHints final : public FX_DOWNLOADHINTS {
 public:
  static void SAddSegment(FX_DOWNLOADHINTS* pThis, size_t offset, size_t size) {
  }

  MockDownloadHints() {
    FX_DOWNLOADHINTS::version = 1;
    FX_DOWNLOADHINTS::AddSegment = SAddSegment;
  }

  ~MockDownloadHints() {}
};

}  // namespace

TEST(fpdf, CApiTest) {
  EXPECT_TRUE(CheckPDFiumCApi());
}

class FPDFViewEmbedderTest : public EmbedderTest {
 protected:
  void TestRenderPageBitmapWithMatrix(FPDF_PAGE page,
                                      const int bitmap_width,
                                      const int bitmap_height,
                                      const FS_MATRIX& matrix,
                                      const FS_RECTF& rect,
                                      const char* expected_md5);
};

TEST_F(FPDFViewEmbedderTest, Document) {
  EXPECT_TRUE(OpenDocument("about_blank.pdf"));
  EXPECT_EQ(1, GetPageCount());
  EXPECT_EQ(0, GetFirstPageNum());

  int version;
  EXPECT_TRUE(FPDF_GetFileVersion(document(), &version));
  EXPECT_EQ(14, version);

  EXPECT_EQ(0xFFFFFFFF, FPDF_GetDocPermissions(document()));
  EXPECT_EQ(-1, FPDF_GetSecurityHandlerRevision(document()));
}

TEST_F(FPDFViewEmbedderTest, LoadNonexistentDocument) {
  FPDF_DOCUMENT doc = FPDF_LoadDocument("nonexistent_document.pdf", "");
  ASSERT_FALSE(doc);
  EXPECT_EQ(static_cast<int>(FPDF_GetLastError()), FPDF_ERR_FILE);
}

// See bug 465.
TEST_F(FPDFViewEmbedderTest, EmptyDocument) {
  EXPECT_TRUE(CreateEmptyDocument());

  {
    int version = 42;
    EXPECT_FALSE(FPDF_GetFileVersion(document(), &version));
    EXPECT_EQ(0, version);
  }

  {
#ifdef PDF_ENABLE_XFA
    const unsigned long kExpected = static_cast<uint32_t>(-1);
#else   // PDF_ENABLE_XFA
    const unsigned long kExpected = 0;
#endif  // PDF_ENABLE_XFA
    EXPECT_EQ(kExpected, FPDF_GetDocPermissions(document()));
  }

  EXPECT_EQ(-1, FPDF_GetSecurityHandlerRevision(document()));

  EXPECT_EQ(0, FPDF_GetPageCount(document()));

  EXPECT_TRUE(FPDF_VIEWERREF_GetPrintScaling(document()));
  EXPECT_EQ(1, FPDF_VIEWERREF_GetNumCopies(document()));
  EXPECT_EQ(DuplexUndefined, FPDF_VIEWERREF_GetDuplex(document()));

  char buf[100];
  EXPECT_EQ(0U, FPDF_VIEWERREF_GetName(document(), "foo", nullptr, 0));
  EXPECT_EQ(0U, FPDF_VIEWERREF_GetName(document(), "foo", buf, sizeof(buf)));

  EXPECT_EQ(0u, FPDF_CountNamedDests(document()));
}

TEST_F(FPDFViewEmbedderTest, LinearizedDocument) {
  EXPECT_TRUE(OpenDocumentLinearized("feature_linearized_loading.pdf"));
  int version;
  EXPECT_TRUE(FPDF_GetFileVersion(document(), &version));
  EXPECT_EQ(16, version);
}

TEST_F(FPDFViewEmbedderTest, LoadCustomDocumentWithoutFileAccess) {
  EXPECT_FALSE(FPDF_LoadCustomDocument(nullptr, ""));
}

TEST_F(FPDFViewEmbedderTest, Page) {
  EXPECT_TRUE(OpenDocument("about_blank.pdf"));
  FPDF_PAGE page = LoadPage(0);
  EXPECT_NE(nullptr, page);

  EXPECT_EQ(612.0, FPDF_GetPageWidth(page));
  EXPECT_EQ(792.0, FPDF_GetPageHeight(page));

  FS_RECTF rect;
  EXPECT_TRUE(FPDF_GetPageBoundingBox(page, &rect));
  EXPECT_EQ(0.0, rect.left);
  EXPECT_EQ(0.0, rect.bottom);
  EXPECT_EQ(612.0, rect.right);
  EXPECT_EQ(792.0, rect.top);

  UnloadPage(page);
  EXPECT_EQ(nullptr, LoadPage(1));
}

TEST_F(FPDFViewEmbedderTest, ViewerRefDummy) {
  EXPECT_TRUE(OpenDocument("about_blank.pdf"));
  EXPECT_TRUE(FPDF_VIEWERREF_GetPrintScaling(document()));
  EXPECT_EQ(1, FPDF_VIEWERREF_GetNumCopies(document()));
  EXPECT_EQ(DuplexUndefined, FPDF_VIEWERREF_GetDuplex(document()));

  char buf[100];
  EXPECT_EQ(0U, FPDF_VIEWERREF_GetName(document(), "foo", nullptr, 0));
  EXPECT_EQ(0U, FPDF_VIEWERREF_GetName(document(), "foo", buf, sizeof(buf)));

  FPDF_PAGERANGE page_range = FPDF_VIEWERREF_GetPrintPageRange(document());
  EXPECT_FALSE(page_range);
  EXPECT_EQ(0U, FPDF_VIEWERREF_GetPrintPageRangeCount(page_range));
  EXPECT_EQ(-1, FPDF_VIEWERREF_GetPrintPageRangeElement(page_range, 0));
  EXPECT_EQ(-1, FPDF_VIEWERREF_GetPrintPageRangeElement(page_range, 1));
}

TEST_F(FPDFViewEmbedderTest, ViewerRef) {
  EXPECT_TRUE(OpenDocument("viewer_ref.pdf"));
  EXPECT_TRUE(FPDF_VIEWERREF_GetPrintScaling(document()));
  EXPECT_EQ(5, FPDF_VIEWERREF_GetNumCopies(document()));
  EXPECT_EQ(DuplexUndefined, FPDF_VIEWERREF_GetDuplex(document()));

  // Test some corner cases.
  char buf[100];
  EXPECT_EQ(0U, FPDF_VIEWERREF_GetName(document(), "", buf, sizeof(buf)));
  EXPECT_EQ(0U, FPDF_VIEWERREF_GetName(document(), "foo", nullptr, 0));
  EXPECT_EQ(0U, FPDF_VIEWERREF_GetName(document(), "foo", buf, sizeof(buf)));

  // Make sure |buf| does not get written into when it appears to be too small.
  // NOLINTNEXTLINE(runtime/printf)
  strcpy(buf, "ABCD");
  EXPECT_EQ(4U, FPDF_VIEWERREF_GetName(document(), "Foo", buf, 1));
  EXPECT_STREQ("ABCD", buf);

  // Note "Foo" is a different key from "foo".
  EXPECT_EQ(4U,
            FPDF_VIEWERREF_GetName(document(), "Foo", nullptr, sizeof(buf)));
  ASSERT_EQ(4U, FPDF_VIEWERREF_GetName(document(), "Foo", buf, sizeof(buf)));
  EXPECT_STREQ("foo", buf);

  // Try to retrieve a boolean and an integer.
  EXPECT_EQ(
      0U, FPDF_VIEWERREF_GetName(document(), "HideToolbar", buf, sizeof(buf)));
  EXPECT_EQ(0U,
            FPDF_VIEWERREF_GetName(document(), "NumCopies", buf, sizeof(buf)));

  // Try more valid cases.
  ASSERT_EQ(4U,
            FPDF_VIEWERREF_GetName(document(), "Direction", buf, sizeof(buf)));
  EXPECT_STREQ("R2L", buf);
  ASSERT_EQ(8U,
            FPDF_VIEWERREF_GetName(document(), "ViewArea", buf, sizeof(buf)));
  EXPECT_STREQ("CropBox", buf);

  FPDF_PAGERANGE page_range = FPDF_VIEWERREF_GetPrintPageRange(document());
  EXPECT_TRUE(page_range);
  EXPECT_EQ(4U, FPDF_VIEWERREF_GetPrintPageRangeCount(page_range));
  EXPECT_EQ(0, FPDF_VIEWERREF_GetPrintPageRangeElement(page_range, 0));
  EXPECT_EQ(2, FPDF_VIEWERREF_GetPrintPageRangeElement(page_range, 1));
  EXPECT_EQ(4, FPDF_VIEWERREF_GetPrintPageRangeElement(page_range, 2));
  EXPECT_EQ(4, FPDF_VIEWERREF_GetPrintPageRangeElement(page_range, 3));
  EXPECT_EQ(-1, FPDF_VIEWERREF_GetPrintPageRangeElement(page_range, 4));
}

TEST_F(FPDFViewEmbedderTest, NamedDests) {
  EXPECT_TRUE(OpenDocument("named_dests.pdf"));
  long buffer_size;
  char fixed_buffer[512];
  FPDF_DEST dest;

  // Query the size of the first item.
  buffer_size = 2000000;  // Absurdly large, check not used for this case.
  dest = FPDF_GetNamedDest(document(), 0, nullptr, &buffer_size);
  EXPECT_NE(nullptr, dest);
  EXPECT_EQ(12, buffer_size);

  // Try to retrieve the first item with too small a buffer.
  buffer_size = 10;
  dest = FPDF_GetNamedDest(document(), 0, fixed_buffer, &buffer_size);
  EXPECT_NE(nullptr, dest);
  EXPECT_EQ(-1, buffer_size);

  // Try to retrieve the first item with correctly sized buffer. Item is
  // taken from Dests NameTree in named_dests.pdf.
  buffer_size = 12;
  dest = FPDF_GetNamedDest(document(), 0, fixed_buffer, &buffer_size);
  EXPECT_NE(nullptr, dest);
  EXPECT_EQ(12, buffer_size);
  EXPECT_EQ(std::string("F\0i\0r\0s\0t\0\0\0", 12),
            std::string(fixed_buffer, buffer_size));

  // Try to retrieve the second item with ample buffer. Item is taken
  // from Dests NameTree but has a sub-dictionary in named_dests.pdf.
  buffer_size = sizeof(fixed_buffer);
  dest = FPDF_GetNamedDest(document(), 1, fixed_buffer, &buffer_size);
  EXPECT_NE(nullptr, dest);
  EXPECT_EQ(10, buffer_size);
  EXPECT_EQ(std::string("N\0e\0x\0t\0\0\0", 10),
            std::string(fixed_buffer, buffer_size));

  // Try to retrieve third item with ample buffer. Item is taken
  // from Dests NameTree but has a bad sub-dictionary in named_dests.pdf.
  // in named_dests.pdf).
  buffer_size = sizeof(fixed_buffer);
  dest = FPDF_GetNamedDest(document(), 2, fixed_buffer, &buffer_size);
  EXPECT_EQ(nullptr, dest);
  EXPECT_EQ(sizeof(fixed_buffer),
            static_cast<size_t>(buffer_size));  // unmodified.

  // Try to retrieve the forth item with ample buffer. Item is taken
  // from Dests NameTree but has a vale of the wrong type in named_dests.pdf.
  buffer_size = sizeof(fixed_buffer);
  dest = FPDF_GetNamedDest(document(), 3, fixed_buffer, &buffer_size);
  EXPECT_EQ(nullptr, dest);
  EXPECT_EQ(sizeof(fixed_buffer),
            static_cast<size_t>(buffer_size));  // unmodified.

  // Try to retrieve fifth item with ample buffer. Item taken from the
  // old-style Dests dictionary object in named_dests.pdf.
  buffer_size = sizeof(fixed_buffer);
  dest = FPDF_GetNamedDest(document(), 4, fixed_buffer, &buffer_size);
  EXPECT_NE(nullptr, dest);
  EXPECT_EQ(30, buffer_size);
  EXPECT_EQ(std::string("F\0i\0r\0s\0t\0A\0l\0t\0e\0r\0n\0a\0t\0e\0\0\0", 30),
            std::string(fixed_buffer, buffer_size));

  // Try to retrieve sixth item with ample buffer. Item istaken from the
  // old-style Dests dictionary object but has a sub-dictionary in
  // named_dests.pdf.
  buffer_size = sizeof(fixed_buffer);
  dest = FPDF_GetNamedDest(document(), 5, fixed_buffer, &buffer_size);
  EXPECT_NE(nullptr, dest);
  EXPECT_EQ(28, buffer_size);
  EXPECT_EQ(std::string("L\0a\0s\0t\0A\0l\0t\0e\0r\0n\0a\0t\0e\0\0\0", 28),
            std::string(fixed_buffer, buffer_size));

  // Try to retrieve non-existent item with ample buffer.
  buffer_size = sizeof(fixed_buffer);
  dest = FPDF_GetNamedDest(document(), 6, fixed_buffer, &buffer_size);
  EXPECT_EQ(nullptr, dest);
  EXPECT_EQ(sizeof(fixed_buffer),
            static_cast<size_t>(buffer_size));  // unmodified.

  // Try to underflow/overflow the integer index.
  buffer_size = sizeof(fixed_buffer);
  dest = FPDF_GetNamedDest(document(), std::numeric_limits<int>::max(),
                           fixed_buffer, &buffer_size);
  EXPECT_EQ(nullptr, dest);
  EXPECT_EQ(sizeof(fixed_buffer),
            static_cast<size_t>(buffer_size));  // unmodified.

  buffer_size = sizeof(fixed_buffer);
  dest = FPDF_GetNamedDest(document(), std::numeric_limits<int>::min(),
                           fixed_buffer, &buffer_size);
  EXPECT_EQ(nullptr, dest);
  EXPECT_EQ(sizeof(fixed_buffer),
            static_cast<size_t>(buffer_size));  // unmodified.

  buffer_size = sizeof(fixed_buffer);
  dest = FPDF_GetNamedDest(document(), -1, fixed_buffer, &buffer_size);
  EXPECT_EQ(nullptr, dest);
  EXPECT_EQ(sizeof(fixed_buffer),
            static_cast<size_t>(buffer_size));  // unmodified.
}

TEST_F(FPDFViewEmbedderTest, NamedDestsByName) {
  EXPECT_TRUE(OpenDocument("named_dests.pdf"));

  // Null pointer returns nullptr.
  FPDF_DEST dest = FPDF_GetNamedDestByName(document(), nullptr);
  EXPECT_EQ(nullptr, dest);

  // Empty string returns nullptr.
  dest = FPDF_GetNamedDestByName(document(), "");
  EXPECT_EQ(nullptr, dest);

  // Item from Dests NameTree.
  dest = FPDF_GetNamedDestByName(document(), "First");
  EXPECT_NE(nullptr, dest);

  long ignore_len = 0;
  FPDF_DEST dest_by_index =
      FPDF_GetNamedDest(document(), 0, nullptr, &ignore_len);
  EXPECT_EQ(dest_by_index, dest);

  // Item from Dests dictionary.
  dest = FPDF_GetNamedDestByName(document(), "FirstAlternate");
  EXPECT_NE(nullptr, dest);

  ignore_len = 0;
  dest_by_index = FPDF_GetNamedDest(document(), 4, nullptr, &ignore_len);
  EXPECT_EQ(dest_by_index, dest);

  // Bad value type for item from Dests NameTree array.
  dest = FPDF_GetNamedDestByName(document(), "WrongType");
  EXPECT_EQ(nullptr, dest);

  // No such destination in either Dest NameTree or dictionary.
  dest = FPDF_GetNamedDestByName(document(), "Bogus");
  EXPECT_EQ(nullptr, dest);
}

// The following tests pass if the document opens without crashing.
TEST_F(FPDFViewEmbedderTest, Crasher_113) {
  EXPECT_TRUE(OpenDocument("bug_113.pdf"));
}

TEST_F(FPDFViewEmbedderTest, Crasher_451830) {
  // Document is damaged and can't be opened.
  EXPECT_FALSE(OpenDocument("bug_451830.pdf"));
}

TEST_F(FPDFViewEmbedderTest, Crasher_452455) {
  EXPECT_TRUE(OpenDocument("bug_452455.pdf"));
  FPDF_PAGE page = LoadPage(0);
  EXPECT_NE(nullptr, page);
  UnloadPage(page);
}

TEST_F(FPDFViewEmbedderTest, Crasher_454695) {
  // Document is damaged and can't be opened.
  EXPECT_FALSE(OpenDocument("bug_454695.pdf"));
}

TEST_F(FPDFViewEmbedderTest, Crasher_572871) {
  EXPECT_TRUE(OpenDocument("bug_572871.pdf"));
}

// It tests that document can still be loaded even the trailer has no 'Size'
// field if other information is right.
TEST_F(FPDFViewEmbedderTest, Failed_213) {
  EXPECT_TRUE(OpenDocument("bug_213.pdf"));
}

// The following tests pass if the document opens without infinite looping.
TEST_F(FPDFViewEmbedderTest, Hang_298) {
  EXPECT_FALSE(OpenDocument("bug_298.pdf"));
}

TEST_F(FPDFViewEmbedderTest, Crasher_773229) {
  EXPECT_TRUE(OpenDocument("bug_773229.pdf"));
}

// Test if the document opens without infinite looping.
// Previously this test will hang in a loop inside LoadAllCrossRefV4. After
// the fix, LoadAllCrossRefV4 will return false after detecting a cross
// reference loop. Cross references will be rebuilt successfully.
TEST_F(FPDFViewEmbedderTest, CrossRefV4Loop) {
  EXPECT_TRUE(OpenDocument("bug_xrefv4_loop.pdf"));
  MockDownloadHints hints;

  // Make sure calling FPDFAvail_IsDocAvail() on this file does not infinite
  // loop either. See bug 875.
  int ret = PDF_DATA_NOTAVAIL;
  while (ret == PDF_DATA_NOTAVAIL)
    ret = FPDFAvail_IsDocAvail(avail_, &hints);
  EXPECT_EQ(PDF_DATA_AVAIL, ret);
}

// The test should pass when circular references to ParseIndirectObject will not
// cause infinite loop.
TEST_F(FPDFViewEmbedderTest, Hang_343) {
  EXPECT_FALSE(OpenDocument("bug_343.pdf"));
}

// The test should pass when the absence of 'Contents' field in a signature
// dictionary will not cause an infinite loop in CPDF_SyntaxParser::GetObject().
TEST_F(FPDFViewEmbedderTest, Hang_344) {
  EXPECT_FALSE(OpenDocument("bug_344.pdf"));
}

// The test should pass when there is no infinite recursion in
// CPDF_SyntaxParser::GetString().
TEST_F(FPDFViewEmbedderTest, Hang_355) {
  EXPECT_FALSE(OpenDocument("bug_355.pdf"));
}
// The test should pass even when the file has circular references to pages.
TEST_F(FPDFViewEmbedderTest, Hang_360) {
  EXPECT_FALSE(OpenDocument("bug_360.pdf"));
}

// Deliberately damaged version of linearized.pdf with bad data in the shared
// object hint table.
TEST_F(FPDFViewEmbedderTest, Hang_1055) {
  EXPECT_TRUE(OpenDocumentLinearized("linearized_bug_1055.pdf"));
  int version;
  EXPECT_TRUE(FPDF_GetFileVersion(document(), &version));
  EXPECT_EQ(16, version);
}

void FPDFViewEmbedderTest::TestRenderPageBitmapWithMatrix(
    FPDF_PAGE page,
    const int bitmap_width,
    const int bitmap_height,
    const FS_MATRIX& matrix,
    const FS_RECTF& rect,
    const char* expected_md5) {
  FPDF_BITMAP bitmap = FPDFBitmap_Create(bitmap_width, bitmap_height, 0);
  FPDFBitmap_FillRect(bitmap, 0, 0, bitmap_width, bitmap_height, 0xFFFFFFFF);
  FPDF_RenderPageBitmapWithMatrix(bitmap, page, &matrix, &rect, 0);
  CompareBitmap(bitmap, bitmap_width, bitmap_height, expected_md5);
  FPDFBitmap_Destroy(bitmap);
}

TEST_F(FPDFViewEmbedderTest, FPDF_RenderPageBitmapWithMatrix) {
  const char kOriginalMD5[] = "0a90de37f52127619c3dfb642b5fa2fe";
  const char kClippedMD5[] = "a84cab93c102b9b9290fba3047ba702c";
  const char kTopLeftQuarterMD5[] = "f11a11137c8834389e31cf555a4a6979";
  const char kHoriStretchedMD5[] = "48ef9205941ed19691ccfa00d717187e";
  const char kRotated90ClockwiseMD5[] = "d8da2c7bf77521550d0f2752b9cf3482";
  const char kRotated180ClockwiseMD5[] = "0113386bb0bd45125bacc6dee78bfe78";
  const char kRotated270ClockwiseMD5[] = "a287e0f74ce203699cda89f9cc97a240";
  const char kMirrorHoriMD5[] = "6e8d7a6fde39d8e720fb9e620102918c";
  const char kMirrorVertMD5[] = "8f3a555ef9c0d5031831ae3715273707";
  const char kLargerTopLeftQuarterMD5[] = "172a2f4adafbadbe98017b1c025b9e27";
  const char kLargerMD5[] = "c806145641c3e6fc4e022c7065343749";
  const char kLargerClippedMD5[] = "091d3b1c7933c8f6945eb2cb41e588e9";
  const char kLargerRotatedMD5[] = "115f13353ebfc82ddb392d1f0059eb12";
  const char kLargerRotatedLandscapeMD5[] = "c901239d17d84ac84cb6f2124da71b0d";
  const char kLargerRotatedDiagonalMD5[] = "3d62417468bdaff0eb14391a0c30a3b1";
  const char kTileMD5[] = "0a190003c97220bf8877684c8d7e89cf";

  EXPECT_TRUE(OpenDocument("rectangles.pdf"));
  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);
  const int page_width = static_cast<int>(FPDF_GetPageWidth(page));
  const int page_height = static_cast<int>(FPDF_GetPageHeight(page));
  EXPECT_EQ(200, page_width);
  EXPECT_EQ(300, page_height);

  ScopedFPDFBitmap bitmap = RenderLoadedPage(page);
  CompareBitmap(bitmap.get(), page_width, page_height, kOriginalMD5);

  FS_RECTF page_rect{0, 0, page_width, page_height};

  // Try rendering with an identity matrix. The output should be the same as
  // the RenderLoadedPage() output.
  FS_MATRIX identity_matrix{1, 0, 0, 1, 0, 0};
  TestRenderPageBitmapWithMatrix(page, page_width, page_height, identity_matrix,
                                 page_rect, kOriginalMD5);

  // Again render with an identity matrix but with a smaller clipping rect.
  FS_RECTF middle_of_page_rect{page_width / 4, page_height / 4,
                               page_width * 3 / 4, page_height * 3 / 4};
  TestRenderPageBitmapWithMatrix(page, page_width, page_height, identity_matrix,
                                 middle_of_page_rect, kClippedMD5);

  // Now render again with the image scaled smaller.
  FS_MATRIX half_scale_matrix{0.5, 0, 0, 0.5, 0, 0};
  TestRenderPageBitmapWithMatrix(page, page_width, page_height,
                                 half_scale_matrix, page_rect,
                                 kTopLeftQuarterMD5);

  // Now render again with the image scaled larger horizontally (the right half
  // will be clipped).
  FS_MATRIX stretch_x_matrix{2, 0, 0, 1, 0, 0};
  TestRenderPageBitmapWithMatrix(page, page_width, page_height,
                                 stretch_x_matrix, page_rect,
                                 kHoriStretchedMD5);

  // Try a 90 degree rotation clockwise but with the same bitmap size, so part
  // will be clipped.
  FS_MATRIX rotate_90_matrix{0, 1, -1, 0, page_width, 0};
  TestRenderPageBitmapWithMatrix(page, page_width, page_height,
                                 rotate_90_matrix, page_rect,
                                 kRotated90ClockwiseMD5);

  // 180 degree rotation clockwise.
  FS_MATRIX rotate_180_matrix{-1, 0, 0, -1, page_width, page_height};
  TestRenderPageBitmapWithMatrix(page, page_width, page_height,
                                 rotate_180_matrix, page_rect,
                                 kRotated180ClockwiseMD5);

  // 270 degree rotation clockwise.
  FS_MATRIX rotate_270_matrix{0, -1, 1, 0, 0, page_width};
  TestRenderPageBitmapWithMatrix(page, page_width, page_height,
                                 rotate_270_matrix, page_rect,
                                 kRotated270ClockwiseMD5);

  // Mirror horizontally.
  FS_MATRIX mirror_hori_matrix{-1, 0, 0, 1, page_width, 0};
  TestRenderPageBitmapWithMatrix(page, page_width, page_height,
                                 mirror_hori_matrix, page_rect, kMirrorHoriMD5);

  // Mirror vertically.
  FS_MATRIX mirror_vert_matrix{1, 0, 0, -1, 0, page_height};
  TestRenderPageBitmapWithMatrix(page, page_width, page_height,
                                 mirror_vert_matrix, page_rect, kMirrorVertMD5);

  // Tests rendering to a larger bitmap
  const int bitmap_width = page_width * 2;
  const int bitmap_height = page_height * 2;

  // Render using an identity matrix and the whole bitmap area as clipping rect.
  FS_RECTF bitmap_rect{0, 0, bitmap_width, bitmap_height};
  TestRenderPageBitmapWithMatrix(page, bitmap_width, bitmap_height,
                                 identity_matrix, bitmap_rect,
                                 kLargerTopLeftQuarterMD5);

  // Render using a scaling matrix to fill the larger bitmap.
  FS_MATRIX double_scale_matrix{2, 0, 0, 2, 0, 0};
  TestRenderPageBitmapWithMatrix(page, bitmap_width, bitmap_height,
                                 double_scale_matrix, bitmap_rect, kLargerMD5);

  // Render the larger image again but with clipping.
  FS_RECTF middle_of_bitmap_rect{bitmap_width / 4, bitmap_height / 4,
                                 bitmap_width * 3 / 4, bitmap_height * 3 / 4};
  TestRenderPageBitmapWithMatrix(page, bitmap_width, bitmap_height,
                                 double_scale_matrix, middle_of_bitmap_rect,
                                 kLargerClippedMD5);

  // On the larger bitmap, try a 90 degree rotation but with the same bitmap
  // size, so part will be clipped.
  FS_MATRIX rotate_90_scale_2_matrix{0, 2, -2, 0, bitmap_width, 0};
  TestRenderPageBitmapWithMatrix(page, bitmap_width, bitmap_height,
                                 rotate_90_scale_2_matrix, bitmap_rect,
                                 kLargerRotatedMD5);

  // On the larger bitmap, apply 90 degree rotation to a bitmap with the
  // appropriate dimensions.
  const int landscape_bitmap_width = bitmap_height;
  const int landscape_bitmap_height = bitmap_width;
  FS_RECTF landscape_bitmap_rect{0, 0, landscape_bitmap_width,
                                 landscape_bitmap_height};
  FS_MATRIX landscape_rotate_90_scale_2_matrix{
      0, 2, -2, 0, landscape_bitmap_width, 0};
  TestRenderPageBitmapWithMatrix(
      page, landscape_bitmap_width, landscape_bitmap_height,
      landscape_rotate_90_scale_2_matrix, landscape_bitmap_rect,
      kLargerRotatedLandscapeMD5);

  // On the larger bitmap, apply 45 degree rotation to a bitmap with the
  // appropriate dimensions.
  const float sqrt2 = 1.41421356f;
  const int diagonal_bitmap_size = ceil((bitmap_width + bitmap_height) / sqrt2);
  FS_RECTF diagonal_bitmap_rect{0, 0, diagonal_bitmap_size,
                                diagonal_bitmap_size};
  FS_MATRIX rotate_45_scale_2_matrix{
      sqrt2, sqrt2, -sqrt2, sqrt2, bitmap_height / sqrt2, 0};
  TestRenderPageBitmapWithMatrix(page, diagonal_bitmap_size,
                                 diagonal_bitmap_size, rotate_45_scale_2_matrix,
                                 diagonal_bitmap_rect,
                                 kLargerRotatedDiagonalMD5);

  // Render the (2, 1) tile of the page (third column, second row) when the page
  // is divided in 50x50 pixel tiles. The tile is scaled by a factor of 7.
  const float scale = 7.0;
  const int tile_size = 50;
  const int tile_x = 2;
  const int tile_y = 1;
  int tile_bitmap_size = scale * tile_size;
  FS_RECTF tile_bitmap_rect{0, 0, tile_bitmap_size, tile_bitmap_size};
  FS_MATRIX tile_2_1_matrix{scale,
                            0,
                            0,
                            scale,
                            -tile_x * tile_bitmap_size,
                            -tile_y * tile_bitmap_size};
  TestRenderPageBitmapWithMatrix(page, tile_bitmap_size, tile_bitmap_size,
                                 tile_2_1_matrix, tile_bitmap_rect, kTileMD5);

  UnloadPage(page);
}

TEST_F(FPDFViewEmbedderTest, FPDF_GetPageSizeByIndex) {
  EXPECT_TRUE(OpenDocument("rectangles.pdf"));

  double width = 0;
  double height = 0;

  EXPECT_FALSE(FPDF_GetPageSizeByIndex(nullptr, 0, &width, &height));
  EXPECT_FALSE(FPDF_GetPageSizeByIndex(document(), 0, nullptr, &height));
  EXPECT_FALSE(FPDF_GetPageSizeByIndex(document(), 0, &width, nullptr));

  // Page -1 doesn't exist.
  EXPECT_FALSE(FPDF_GetPageSizeByIndex(document(), -1, &width, &height));

  // Page 1 doesn't exist.
  EXPECT_FALSE(FPDF_GetPageSizeByIndex(document(), 1, &width, &height));

  // Page 0 exists.
  EXPECT_TRUE(FPDF_GetPageSizeByIndex(document(), 0, &width, &height));
  EXPECT_EQ(200.0, width);
  EXPECT_EQ(300.0, height);

  CPDF_Document* pDoc = CPDFDocumentFromFPDFDocument(document());
#ifdef PDF_ENABLE_XFA
  // TODO(tsepez): XFA must obtain this size without parsing.
  EXPECT_EQ(1u, pDoc->GetParsedPageCountForTesting());
#else   // PDF_ENABLE_XFA
  EXPECT_EQ(0u, pDoc->GetParsedPageCountForTesting());
#endif  // PDF_ENABLE_XFA

  // Double-check against values from when page is actually parsed.
  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);
  EXPECT_EQ(width, FPDF_GetPageWidth(page));
  EXPECT_EQ(height, FPDF_GetPageHeight(page));
  EXPECT_EQ(1u, pDoc->GetParsedPageCountForTesting());
  UnloadPage(page);
}

class UnSupRecordDelegate final : public EmbedderTest::Delegate {
 public:
  UnSupRecordDelegate() : type_(-1) {}
  ~UnSupRecordDelegate() override {}

  void UnsupportedHandler(int type) override { type_ = type; }

  int type_;
};

TEST_F(FPDFViewEmbedderTest, UnSupportedOperations_NotFound) {
  UnSupRecordDelegate delegate;
  SetDelegate(&delegate);
  ASSERT_TRUE(OpenDocument("hello_world.pdf"));
  EXPECT_EQ(delegate.type_, -1);
  SetDelegate(nullptr);
}

TEST_F(FPDFViewEmbedderTest, UnSupportedOperations_LoadCustomDocument) {
  UnSupRecordDelegate delegate;
  SetDelegate(&delegate);
  ASSERT_TRUE(OpenDocument("unsupported_feature.pdf"));
  EXPECT_EQ(FPDF_UNSP_DOC_PORTABLECOLLECTION, delegate.type_);
  SetDelegate(nullptr);
}

TEST_F(FPDFViewEmbedderTest, UnSupportedOperations_LoadDocument) {
  std::string file_path;
  ASSERT_TRUE(
      PathService::GetTestFilePath("unsupported_feature.pdf", &file_path));

  UnSupRecordDelegate delegate;
  SetDelegate(&delegate);
  FPDF_DOCUMENT doc = FPDF_LoadDocument(file_path.c_str(), "");
  EXPECT_TRUE(doc != nullptr);
  EXPECT_EQ(FPDF_UNSP_DOC_PORTABLECOLLECTION, delegate.type_);
  FPDF_CloseDocument(doc);
  SetDelegate(nullptr);
}

TEST_F(FPDFViewEmbedderTest, DocumentHasValidCrossReferenceTable) {
  ASSERT_TRUE(OpenDocument("hello_world.pdf"));
  EXPECT_TRUE(FPDF_DocumentHasValidCrossReferenceTable(document()));
}

TEST_F(FPDFViewEmbedderTest, DocumentHasInvalidCrossReferenceTable) {
  EXPECT_FALSE(FPDF_DocumentHasValidCrossReferenceTable(nullptr));

  ASSERT_TRUE(OpenDocument("bug_664284.pdf"));
  EXPECT_FALSE(FPDF_DocumentHasValidCrossReferenceTable(document()));
}

// Related to https://crbug.com/pdfium/1197
TEST_F(FPDFViewEmbedderTest, LoadDocumentWithEmptyXRefConsistently) {
  ASSERT_TRUE(OpenDocument("empty_xref.pdf"));
  EXPECT_TRUE(FPDF_DocumentHasValidCrossReferenceTable(document()));

  std::string file_path;
  ASSERT_TRUE(PathService::GetTestFilePath("empty_xref.pdf", &file_path));
  {
    ScopedFPDFDocument doc(FPDF_LoadDocument(file_path.c_str(), ""));
    ASSERT_TRUE(doc);
    EXPECT_TRUE(FPDF_DocumentHasValidCrossReferenceTable(doc.get()));
  }
  {
    size_t file_length = 0;
    std::unique_ptr<char, pdfium::FreeDeleter> file_contents =
        GetFileContents(file_path.c_str(), &file_length);
    ASSERT(file_contents);
    ScopedFPDFDocument doc(
        FPDF_LoadMemDocument(file_contents.get(), file_length, ""));
    ASSERT_TRUE(doc);
    EXPECT_TRUE(FPDF_DocumentHasValidCrossReferenceTable(doc.get()));
  }
}
