// Copyright 2015 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <memory>
#include <set>
#include <string>
#include <vector>

#include "core/fpdfapi/parser/cpdf_document.h"
#include "core/fxcrt/fx_string.h"
#include "fpdfsdk/cpdfsdk_helpers.h"
#include "public/cpp/fpdf_scopers.h"
#include "public/fpdf_doc.h"
#include "public/fpdf_edit.h"
#include "public/fpdfview.h"
#include "testing/embedder_test.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "testing/test_support.h"

class FPDFDocEmbedderTest : public EmbedderTest {};

TEST_F(FPDFDocEmbedderTest, MultipleSamePage) {
  EXPECT_TRUE(OpenDocument("hello_world.pdf"));
  CPDF_Document* pDoc = CPDFDocumentFromFPDFDocument(document());

  std::set<FPDF_PAGE> unique_pages;
  std::vector<ScopedFPDFPage> owned_pages(4);
  for (auto& ref : owned_pages) {
    ref.reset(FPDF_LoadPage(document(), 0));
    unique_pages.insert(ref.get());
  }
#ifdef PDF_ENABLE_XFA
  EXPECT_EQ(1u, unique_pages.size());
  EXPECT_EQ(1u, pDoc->GetParsedPageCountForTesting());
#else   // PDF_ENABLE_XFA
  EXPECT_EQ(4u, unique_pages.size());
  EXPECT_EQ(4u, pDoc->GetParsedPageCountForTesting());
#endif  // PDF_ENABLE_XFA
}

TEST_F(FPDFDocEmbedderTest, DestGetPageIndex) {
  EXPECT_TRUE(OpenDocument("named_dests.pdf"));

  // NULL argument cases.
  EXPECT_EQ(-1, FPDFDest_GetDestPageIndex(nullptr, nullptr));
  EXPECT_EQ(-1, FPDFDest_GetDestPageIndex(document(), nullptr));

  // Page number directly in item from Dests NameTree.
  FPDF_DEST dest = FPDF_GetNamedDestByName(document(), "First");
  EXPECT_TRUE(dest);
  EXPECT_EQ(1, FPDFDest_GetDestPageIndex(document(), dest));

  // Page number via object reference in item from Dests NameTree.
  dest = FPDF_GetNamedDestByName(document(), "Next");
  EXPECT_TRUE(dest);
  EXPECT_EQ(1, FPDFDest_GetDestPageIndex(document(), dest));

  // Page number directly in item from Dests dictionary.
  dest = FPDF_GetNamedDestByName(document(), "FirstAlternate");
  EXPECT_TRUE(dest);
  EXPECT_EQ(11, FPDFDest_GetDestPageIndex(document(), dest));

  // Invalid object reference in item from Dests NameTree.
  dest = FPDF_GetNamedDestByName(document(), "LastAlternate");
  EXPECT_TRUE(dest);
  EXPECT_EQ(-1, FPDFDest_GetDestPageIndex(document(), dest));
}

TEST_F(FPDFDocEmbedderTest, DestGetView) {
  EXPECT_TRUE(OpenDocument("named_dests.pdf"));

  unsigned long numParams;
  FS_FLOAT params[4];

  numParams = 42;
  std::fill_n(params, 4, 42.4242f);
  EXPECT_EQ(static_cast<unsigned long>(PDFDEST_VIEW_UNKNOWN_MODE),
            FPDFDest_GetView(nullptr, &numParams, params));
  EXPECT_EQ(0U, numParams);
  EXPECT_FLOAT_EQ(42.4242f, params[0]);

  numParams = 42;
  std::fill_n(params, 4, 42.4242f);
  FPDF_DEST dest = FPDF_GetNamedDestByName(document(), "First");
  EXPECT_TRUE(dest);
  EXPECT_EQ(static_cast<unsigned long>(PDFDEST_VIEW_XYZ),
            FPDFDest_GetView(dest, &numParams, params));
  EXPECT_EQ(3U, numParams);
  EXPECT_FLOAT_EQ(0, params[0]);
  EXPECT_FLOAT_EQ(0, params[1]);
  EXPECT_FLOAT_EQ(1, params[2]);
  EXPECT_FLOAT_EQ(42.4242f, params[3]);

  numParams = 42;
  std::fill_n(params, 4, 42.4242f);
  dest = FPDF_GetNamedDestByName(document(), "Next");
  EXPECT_TRUE(dest);
  EXPECT_EQ(static_cast<unsigned long>(PDFDEST_VIEW_FIT),
            FPDFDest_GetView(dest, &numParams, params));
  EXPECT_EQ(0U, numParams);
  EXPECT_FLOAT_EQ(42.4242f, params[0]);

  numParams = 42;
  std::fill_n(params, 4, 42.4242f);
  dest = FPDF_GetNamedDestByName(document(), "FirstAlternate");
  EXPECT_TRUE(dest);
  EXPECT_EQ(static_cast<unsigned long>(PDFDEST_VIEW_XYZ),
            FPDFDest_GetView(dest, &numParams, params));
  EXPECT_EQ(3U, numParams);
  EXPECT_FLOAT_EQ(200, params[0]);
  EXPECT_FLOAT_EQ(400, params[1]);
  EXPECT_FLOAT_EQ(800, params[2]);
  EXPECT_FLOAT_EQ(42.4242f, params[3]);

  numParams = 42;
  std::fill_n(params, 4, 42.4242f);
  dest = FPDF_GetNamedDestByName(document(), "LastAlternate");
  EXPECT_TRUE(dest);
  EXPECT_EQ(static_cast<unsigned long>(PDFDEST_VIEW_XYZ),
            FPDFDest_GetView(dest, &numParams, params));
  EXPECT_EQ(3U, numParams);
  EXPECT_FLOAT_EQ(0, params[0]);
  EXPECT_FLOAT_EQ(0, params[1]);
  EXPECT_FLOAT_EQ(-200, params[2]);
  EXPECT_FLOAT_EQ(42.4242f, params[3]);
}

TEST_F(FPDFDocEmbedderTest, DestGetLocationInPage) {
  EXPECT_TRUE(OpenDocument("named_dests.pdf"));

  FPDF_DEST dest = FPDF_GetNamedDestByName(document(), "First");
  EXPECT_TRUE(dest);

  FPDF_BOOL hasX = 0;
  FPDF_BOOL hasY = 0;
  FPDF_BOOL hasZoom = 0;
  FS_FLOAT x = -1.0f;
  FS_FLOAT y = -1.0f;
  FS_FLOAT zoom = -1.0f;

  // NULL argument case
  EXPECT_FALSE(FPDFDest_GetLocationInPage(nullptr, &hasX, &hasY, &hasZoom, &x,
                                          &y, &zoom));

  // Actual argument case.
  EXPECT_TRUE(
      FPDFDest_GetLocationInPage(dest, &hasX, &hasY, &hasZoom, &x, &y, &zoom));
  EXPECT_TRUE(hasX);
  EXPECT_TRUE(hasY);
  EXPECT_TRUE(hasZoom);
  EXPECT_EQ(0, x);
  EXPECT_EQ(0, y);
  EXPECT_EQ(1, zoom);
}

TEST_F(FPDFDocEmbedderTest, BUG_680376) {
  EXPECT_TRUE(OpenDocument("bug_680376.pdf"));

  // Page number directly in item from Dests NameTree.
  FPDF_DEST dest = FPDF_GetNamedDestByName(document(), "First");
  EXPECT_TRUE(dest);
  EXPECT_EQ(-1, FPDFDest_GetDestPageIndex(document(), dest));
}

TEST_F(FPDFDocEmbedderTest, BUG_821454) {
  EXPECT_TRUE(OpenDocument("bug_821454.pdf"));

  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);

  // Cover some NULL arg cases while we're at it.
  EXPECT_FALSE(FPDFLink_GetLinkAtPoint(nullptr, 150, 360));
  EXPECT_EQ(-1, FPDFLink_GetLinkZOrderAtPoint(nullptr, 150, 360));

  FPDF_LINK link1 = FPDFLink_GetLinkAtPoint(page, 150, 360);
  ASSERT_TRUE(link1);
  FPDF_LINK link2 = FPDFLink_GetLinkAtPoint(page, 150, 420);
  ASSERT_TRUE(link2);

  EXPECT_EQ(0, FPDFLink_GetLinkZOrderAtPoint(page, 150, 360));
  EXPECT_EQ(1, FPDFLink_GetLinkZOrderAtPoint(page, 150, 420));

  FPDF_DEST dest1 = FPDFLink_GetDest(document(), link1);
  ASSERT_TRUE(dest1);
  FPDF_DEST dest2 = FPDFLink_GetDest(document(), link2);
  ASSERT_TRUE(dest2);

  EXPECT_EQ(0, FPDFDest_GetDestPageIndex(document(), dest1));
  EXPECT_EQ(0, FPDFDest_GetDestPageIndex(document(), dest2));

  {
    FPDF_BOOL has_x_coord;
    FPDF_BOOL has_y_coord;
    FPDF_BOOL has_zoom;
    FS_FLOAT x;
    FS_FLOAT y;
    FS_FLOAT zoom;
    FPDF_BOOL success = FPDFDest_GetLocationInPage(
        dest1, &has_x_coord, &has_y_coord, &has_zoom, &x, &y, &zoom);
    ASSERT_TRUE(success);
    EXPECT_TRUE(has_x_coord);
    EXPECT_TRUE(has_y_coord);
    EXPECT_FALSE(has_zoom);
    EXPECT_FLOAT_EQ(100.0f, x);
    EXPECT_FLOAT_EQ(200.0f, y);
  }
  {
    FPDF_BOOL has_x_coord;
    FPDF_BOOL has_y_coord;
    FPDF_BOOL has_zoom;
    FS_FLOAT x;
    FS_FLOAT y;
    FS_FLOAT zoom;
    FPDF_BOOL success = FPDFDest_GetLocationInPage(
        dest2, &has_x_coord, &has_y_coord, &has_zoom, &x, &y, &zoom);
    ASSERT_TRUE(success);
    EXPECT_TRUE(has_x_coord);
    EXPECT_TRUE(has_y_coord);
    EXPECT_FALSE(has_zoom);
    EXPECT_FLOAT_EQ(150.0f, x);
    EXPECT_FLOAT_EQ(250.0f, y);
  }

  UnloadPage(page);
}

TEST_F(FPDFDocEmbedderTest, ActionBadArguments) {
  EXPECT_TRUE(OpenDocument("launch_action.pdf"));
  EXPECT_EQ(static_cast<unsigned long>(PDFACTION_UNSUPPORTED),
            FPDFAction_GetType(nullptr));

  EXPECT_EQ(nullptr, FPDFAction_GetDest(nullptr, nullptr));
  EXPECT_EQ(nullptr, FPDFAction_GetDest(document(), nullptr));
  EXPECT_EQ(0u, FPDFAction_GetFilePath(nullptr, nullptr, 0));
  EXPECT_EQ(0u, FPDFAction_GetURIPath(nullptr, nullptr, nullptr, 0));
  EXPECT_EQ(0u, FPDFAction_GetURIPath(document(), nullptr, nullptr, 0));
}

TEST_F(FPDFDocEmbedderTest, ActionLaunch) {
  EXPECT_TRUE(OpenDocument("launch_action.pdf"));

  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);

  // The target action is nearly the size of the whole page.
  FPDF_LINK link = FPDFLink_GetLinkAtPoint(page, 100, 100);
  ASSERT_TRUE(link);

  FPDF_ACTION action = FPDFLink_GetAction(link);
  ASSERT_TRUE(action);
  EXPECT_EQ(static_cast<unsigned long>(PDFACTION_LAUNCH),
            FPDFAction_GetType(action));

  const char kExpectedResult[] = "test.pdf";
  const unsigned long kExpectedLength = sizeof(kExpectedResult);
  unsigned long bufsize = FPDFAction_GetFilePath(action, nullptr, 0);
  EXPECT_EQ(kExpectedLength, bufsize);

  char buf[1024];
  EXPECT_EQ(bufsize, FPDFAction_GetFilePath(action, buf, bufsize));
  EXPECT_STREQ(kExpectedResult, buf);

  // Other public methods are not appropriate for launch actions.
  EXPECT_EQ(nullptr, FPDFAction_GetDest(document(), action));
  EXPECT_EQ(0u, FPDFAction_GetURIPath(document(), action, buf, sizeof(buf)));

  UnloadPage(page);
}

TEST_F(FPDFDocEmbedderTest, ActionURI) {
  EXPECT_TRUE(OpenDocument("uri_action.pdf"));

  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);

  // The target action is nearly the size of the whole page.
  FPDF_LINK link = FPDFLink_GetLinkAtPoint(page, 100, 100);
  ASSERT_TRUE(link);

  FPDF_ACTION action = FPDFLink_GetAction(link);
  ASSERT_TRUE(action);
  EXPECT_EQ(static_cast<unsigned long>(PDFACTION_URI),
            FPDFAction_GetType(action));

  const char kExpectedResult[] = "https://example.com/page.html";
  const unsigned long kExpectedLength = sizeof(kExpectedResult);
  unsigned long bufsize = FPDFAction_GetURIPath(document(), action, nullptr, 0);
  ASSERT_EQ(kExpectedLength, bufsize);

  char buf[1024];
  EXPECT_EQ(bufsize, FPDFAction_GetURIPath(document(), action, buf, bufsize));
  EXPECT_STREQ(kExpectedResult, buf);

  // Other public methods are not appropriate for URI actions
  EXPECT_EQ(nullptr, FPDFAction_GetDest(document(), action));
  EXPECT_EQ(0u, FPDFAction_GetFilePath(action, buf, sizeof(buf)));

  UnloadPage(page);
}

TEST_F(FPDFDocEmbedderTest, ActionGoto) {
  EXPECT_TRUE(OpenDocument("goto_action.pdf"));

  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);

  // The target action is nearly the size of the whole page.
  FPDF_LINK link = FPDFLink_GetLinkAtPoint(page, 100, 100);
  ASSERT_TRUE(link);

  FPDF_ACTION action = FPDFLink_GetAction(link);
  ASSERT_TRUE(action);
  EXPECT_EQ(static_cast<unsigned long>(PDFACTION_GOTO),
            FPDFAction_GetType(action));

  EXPECT_TRUE(FPDFAction_GetDest(document(), action));

  // Other public methods are not appropriate for GoTo actions.
  char buf[1024];
  EXPECT_EQ(0u, FPDFAction_GetFilePath(action, buf, sizeof(buf)));
  EXPECT_EQ(0u, FPDFAction_GetURIPath(document(), action, buf, sizeof(buf)));

  UnloadPage(page);
}

TEST_F(FPDFDocEmbedderTest, ActionNonesuch) {
  EXPECT_TRUE(OpenDocument("nonesuch_action.pdf"));

  FPDF_PAGE page = LoadPage(0);
  ASSERT_TRUE(page);

  // The target action is nearly the size of the whole page.
  FPDF_LINK link = FPDFLink_GetLinkAtPoint(page, 100, 100);
  ASSERT_TRUE(link);

  FPDF_ACTION action = FPDFLink_GetAction(link);
  ASSERT_TRUE(action);
  EXPECT_EQ(static_cast<unsigned long>(PDFACTION_UNSUPPORTED),
            FPDFAction_GetType(action));

  // No public methods are appropriate for unsupported actions.
  char buf[1024];
  EXPECT_FALSE(FPDFAction_GetDest(document(), action));
  EXPECT_EQ(0u, FPDFAction_GetFilePath(action, buf, sizeof(buf)));
  EXPECT_EQ(0u, FPDFAction_GetURIPath(document(), action, buf, sizeof(buf)));

  UnloadPage(page);
}

TEST_F(FPDFDocEmbedderTest, NoBookmarks) {
  // Open a file with no bookmarks.
  EXPECT_TRUE(OpenDocument("named_dests.pdf"));

  // The non-existent top-level bookmark has no title.
  unsigned short buf[128];
  EXPECT_EQ(0u, FPDFBookmark_GetTitle(nullptr, buf, sizeof(buf)));

  // NULL argument cases.
  EXPECT_EQ(nullptr, FPDFBookmark_GetFirstChild(nullptr, nullptr));
  EXPECT_EQ(nullptr, FPDFBookmark_GetFirstChild(document(), nullptr));
  EXPECT_EQ(nullptr, FPDFBookmark_GetNextSibling(nullptr, nullptr));
  EXPECT_EQ(nullptr, FPDFBookmark_GetNextSibling(document(), nullptr));
  EXPECT_EQ(nullptr, FPDFBookmark_Find(nullptr, nullptr));
  EXPECT_EQ(nullptr, FPDFBookmark_Find(document(), nullptr));
  EXPECT_EQ(nullptr, FPDFBookmark_GetDest(nullptr, nullptr));
  EXPECT_EQ(nullptr, FPDFBookmark_GetDest(document(), nullptr));
  EXPECT_EQ(nullptr, FPDFBookmark_GetAction(nullptr));
}

TEST_F(FPDFDocEmbedderTest, Bookmarks) {
  // Open a file with two bookmarks.
  EXPECT_TRUE(OpenDocument("bookmarks.pdf"));

  // The existent top-level bookmark has no title.
  unsigned short buf[128];
  EXPECT_EQ(0u, FPDFBookmark_GetTitle(nullptr, buf, sizeof(buf)));

  FPDF_BOOKMARK child = FPDFBookmark_GetFirstChild(document(), nullptr);
  EXPECT_TRUE(child);
  EXPECT_EQ(34u, FPDFBookmark_GetTitle(child, buf, sizeof(buf)));
  EXPECT_EQ(WideString(L"A Good Beginning"), WideString::FromUTF16LE(buf, 16));

  EXPECT_EQ(nullptr, FPDFBookmark_GetFirstChild(document(), child));

  FPDF_BOOKMARK sibling = FPDFBookmark_GetNextSibling(document(), child);
  EXPECT_TRUE(sibling);
  EXPECT_EQ(28u, FPDFBookmark_GetTitle(sibling, buf, sizeof(buf)));
  EXPECT_EQ(WideString(L"A Good Ending"), WideString::FromUTF16LE(buf, 13));

  EXPECT_EQ(nullptr, FPDFBookmark_GetNextSibling(document(), sibling));
}

TEST_F(FPDFDocEmbedderTest, FindBookmarks) {
  // Open a file with two bookmarks.
  EXPECT_TRUE(OpenDocument("bookmarks.pdf"));

  // Find the first one, based on its known title.
  std::unique_ptr<unsigned short, pdfium::FreeDeleter> title =
      GetFPDFWideString(L"A Good Beginning");
  FPDF_BOOKMARK child = FPDFBookmark_Find(document(), title.get());
  EXPECT_TRUE(child);

  // Check that the string matches.
  unsigned short buf[128];
  EXPECT_EQ(34u, FPDFBookmark_GetTitle(child, buf, sizeof(buf)));
  EXPECT_EQ(WideString(L"A Good Beginning"), WideString::FromUTF16LE(buf, 16));

  // Check that it is them same as the one returned by GetFirstChild.
  EXPECT_EQ(child, FPDFBookmark_GetFirstChild(document(), nullptr));

  // Try to find one using a non-existent title.
  std::unique_ptr<unsigned short, pdfium::FreeDeleter> bad_title =
      GetFPDFWideString(L"A BAD Beginning");
  EXPECT_EQ(nullptr, FPDFBookmark_Find(document(), bad_title.get()));
}

// Check circular bookmarks will not cause infinite loop.
TEST_F(FPDFDocEmbedderTest, FindBookmarks_bug420) {
  // Open a file with circular bookmarks.
  EXPECT_TRUE(OpenDocument("bookmarks_circular.pdf"));

  // Try to find a title.
  std::unique_ptr<unsigned short, pdfium::FreeDeleter> title =
      GetFPDFWideString(L"anything");
  EXPECT_EQ(nullptr, FPDFBookmark_Find(document(), title.get()));
}

TEST_F(FPDFDocEmbedderTest, DeletePage) {
  EXPECT_TRUE(OpenDocument("hello_world.pdf"));
  EXPECT_EQ(1, FPDF_GetPageCount(document()));
  FPDFPage_Delete(document(), 0);
  EXPECT_EQ(0, FPDF_GetPageCount(document()));
}

TEST_F(FPDFDocEmbedderTest, GetMetaText) {
  ASSERT_TRUE(OpenDocument("bug_601362.pdf"));

  // Invalid document / tag results in 0.
  unsigned short buf[128];
  EXPECT_EQ(0u, FPDF_GetMetaText(document(), nullptr, buf, sizeof(buf)));
  EXPECT_EQ(0u, FPDF_GetMetaText(nullptr, "", buf, sizeof(buf)));

  // Tags that do not eixst results in an empty wide string.
  EXPECT_EQ(2u, FPDF_GetMetaText(document(), "", buf, sizeof(buf)));
  EXPECT_EQ(2u, FPDF_GetMetaText(document(), "foo", buf, sizeof(buf)));
  ASSERT_EQ(2u, FPDF_GetMetaText(document(), "Title", buf, sizeof(buf)));
  ASSERT_EQ(2u, FPDF_GetMetaText(document(), "Author", buf, sizeof(buf)));
  ASSERT_EQ(2u, FPDF_GetMetaText(document(), "Subject", buf, sizeof(buf)));
  ASSERT_EQ(2u, FPDF_GetMetaText(document(), "Keywords", buf, sizeof(buf)));
  ASSERT_EQ(2u, FPDF_GetMetaText(document(), "Producer", buf, sizeof(buf)));

  constexpr wchar_t kExpectedCreator[] = L"Microsoft Word";
  ASSERT_EQ(30u, FPDF_GetMetaText(document(), "Creator", buf, sizeof(buf)));
  EXPECT_EQ(WideString(kExpectedCreator),
            WideString::FromUTF16LE(buf, FXSYS_len(kExpectedCreator)));

  constexpr wchar_t kExpectedCreationDate[] = L"D:20160411190039+00'00'";
  ASSERT_EQ(48u,
            FPDF_GetMetaText(document(), "CreationDate", buf, sizeof(buf)));
  EXPECT_EQ(WideString(kExpectedCreationDate),
            WideString::FromUTF16LE(buf, FXSYS_len(kExpectedCreationDate)));

  constexpr wchar_t kExpectedModDate[] = L"D:20160411190039+00'00'";
  ASSERT_EQ(48u, FPDF_GetMetaText(document(), "ModDate", buf, sizeof(buf)));
  EXPECT_EQ(WideString(kExpectedModDate),
            WideString::FromUTF16LE(buf, FXSYS_len(kExpectedModDate)));
}

TEST_F(FPDFDocEmbedderTest, Bug_182) {
  ASSERT_TRUE(OpenDocument("bug_182.pdf"));

  unsigned short buf[128];
  constexpr wchar_t kExpectedTitle[] = L"Super Visual Formade 印刷";

  ASSERT_EQ(48u, FPDF_GetMetaText(document(), "Title", buf, sizeof(buf)));
  EXPECT_EQ(WideString(kExpectedTitle),
            WideString::FromUTF16LE(buf, FXSYS_len(kExpectedTitle)));
}

TEST_F(FPDFDocEmbedderTest, GetMetaTextSameObjectNumber) {
  ASSERT_TRUE(OpenDocument("annotation_highlight_square_with_ap.pdf"));

  // The PDF has been edited. It has two %%EOF markers, and 2 objects numbered
  // (1 0). Both objects are /Info dictionaries, but contain different data.
  // Make sure ModDate is the date of the last modification.
  unsigned short buf[128];
  constexpr wchar_t kExpectedModDate[] = L"D:20170612232940-04'00'";
  ASSERT_EQ(48u, FPDF_GetMetaText(document(), "ModDate", buf, sizeof(buf)));
  EXPECT_EQ(WideString(kExpectedModDate),
            WideString::FromUTF16LE(buf, FXSYS_len(kExpectedModDate)));
}

TEST_F(FPDFDocEmbedderTest, GetMetaTextInAttachmentFile) {
  ASSERT_TRUE(OpenDocument("embedded_attachments.pdf"));

  // Make sure this is the date from the PDF itself and not the attached PDF.
  unsigned short buf[128];
  constexpr wchar_t kExpectedModDate[] = L"D:20170712214448-07'00'";
  ASSERT_EQ(48u, FPDF_GetMetaText(document(), "ModDate", buf, sizeof(buf)));
  EXPECT_EQ(WideString(kExpectedModDate),
            WideString::FromUTF16LE(buf, FXSYS_len(kExpectedModDate)));
}

TEST_F(FPDFDocEmbedderTest, GetMetaTextFromNewDocument) {
  FPDF_DOCUMENT empty_doc = FPDF_CreateNewDocument();
  unsigned short buf[128];
  EXPECT_EQ(2u, FPDF_GetMetaText(empty_doc, "Title", buf, sizeof(buf)));
  FPDF_CloseDocument(empty_doc);
}

TEST_F(FPDFDocEmbedderTest, NoPageLabels) {
  EXPECT_TRUE(OpenDocument("about_blank.pdf"));
  EXPECT_EQ(1, FPDF_GetPageCount(document()));

  ASSERT_EQ(0u, FPDF_GetPageLabel(document(), 0, nullptr, 0));
}

TEST_F(FPDFDocEmbedderTest, GetPageLabels) {
  EXPECT_TRUE(OpenDocument("page_labels.pdf"));
  EXPECT_EQ(7, FPDF_GetPageCount(document()));

  // We do not request labels, when use FPDFAvail_IsXXXAvail.
  // Flush all data, to allow read labels.
  SetWholeFileAvailable();

  unsigned short buf[128];
  EXPECT_EQ(0u, FPDF_GetPageLabel(document(), -2, buf, sizeof(buf)));
  EXPECT_EQ(0u, FPDF_GetPageLabel(document(), -1, buf, sizeof(buf)));

  const wchar_t kExpectedPageLabel0[] = L"i";
  ASSERT_EQ(4u, FPDF_GetPageLabel(document(), 0, buf, sizeof(buf)));
  EXPECT_EQ(WideString(kExpectedPageLabel0),
            WideString::FromUTF16LE(buf, FXSYS_len(kExpectedPageLabel0)));

  const wchar_t kExpectedPageLabel1[] = L"ii";
  ASSERT_EQ(6u, FPDF_GetPageLabel(document(), 1, buf, sizeof(buf)));
  EXPECT_EQ(WideString(kExpectedPageLabel1),
            WideString::FromUTF16LE(buf, FXSYS_len(kExpectedPageLabel1)));

  const wchar_t kExpectedPageLabel2[] = L"1";
  ASSERT_EQ(4u, FPDF_GetPageLabel(document(), 2, buf, sizeof(buf)));
  EXPECT_EQ(WideString(kExpectedPageLabel2),
            WideString::FromUTF16LE(buf, FXSYS_len(kExpectedPageLabel2)));

  const wchar_t kExpectedPageLabel3[] = L"2";
  ASSERT_EQ(4u, FPDF_GetPageLabel(document(), 3, buf, sizeof(buf)));
  EXPECT_EQ(WideString(kExpectedPageLabel3),
            WideString::FromUTF16LE(buf, FXSYS_len(kExpectedPageLabel3)));

  const wchar_t kExpectedPageLabel4[] = L"zzA";
  ASSERT_EQ(8u, FPDF_GetPageLabel(document(), 4, buf, sizeof(buf)));
  EXPECT_EQ(WideString(kExpectedPageLabel4),
            WideString::FromUTF16LE(buf, FXSYS_len(kExpectedPageLabel4)));

  const wchar_t kExpectedPageLabel5[] = L"zzB";
  ASSERT_EQ(8u, FPDF_GetPageLabel(document(), 5, buf, sizeof(buf)));
  EXPECT_EQ(WideString(kExpectedPageLabel5),
            WideString::FromUTF16LE(buf, FXSYS_len(kExpectedPageLabel5)));

  const wchar_t kExpectedPageLabel6[] = L"";
  ASSERT_EQ(2u, FPDF_GetPageLabel(document(), 6, buf, sizeof(buf)));
  EXPECT_EQ(WideString(kExpectedPageLabel6),
            WideString::FromUTF16LE(buf, FXSYS_len(kExpectedPageLabel6)));

  ASSERT_EQ(0u, FPDF_GetPageLabel(document(), 7, buf, sizeof(buf)));
  ASSERT_EQ(0u, FPDF_GetPageLabel(document(), 8, buf, sizeof(buf)));
}

#ifdef PDF_ENABLE_XFA
TEST_F(FPDFDocEmbedderTest, GetXFALinks) {
  EXPECT_TRUE(OpenDocument("simple_xfa.pdf"));

  ScopedFPDFPage page(FPDF_LoadPage(document(), 0));
  ASSERT_TRUE(page);

  FPDFLink_GetLinkAtPoint(page.get(), 150, 360);
  FPDFLink_GetLinkAtPoint(page.get(), 150, 420);

  // Test passes if it doesn't crash. See https://crbug.com/840922
}
#endif  // PDF_ENABLE_XFA
