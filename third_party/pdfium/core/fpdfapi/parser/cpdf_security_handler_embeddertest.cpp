// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <memory>
#include <string>

#include "core/fxcrt/fx_system.h"
#include "public/cpp/fpdf_scopers.h"
#include "public/fpdf_edit.h"
#include "public/fpdf_save.h"
#include "public/fpdfview.h"
#include "testing/embedder_test.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace {

const char kAgeUTF8[] =
    "\xc3\xa2"
    "ge";
const char kAgeLatin1[] =
    "\xe2"
    "ge";

const char kHotelUTF8[] =
    "h"
    "\xc3\xb4"
    "tel";
const char kHotelLatin1[] =
    "h"
    "\xf4"
    "tel";

}  // namespace

class CPDFSecurityHandlerEmbedderTest : public EmbedderTest {};

TEST_F(CPDFSecurityHandlerEmbedderTest, Unencrypted) {
  ASSERT_TRUE(OpenDocument("about_blank.pdf"));
  EXPECT_EQ(0xFFFFFFFF, FPDF_GetDocPermissions(document()));
}

TEST_F(CPDFSecurityHandlerEmbedderTest, UnencryptedWithPassword) {
  ASSERT_TRUE(OpenDocumentWithPassword("about_blank.pdf", "foobar"));
  EXPECT_EQ(0xFFFFFFFF, FPDF_GetDocPermissions(document()));
}

TEST_F(CPDFSecurityHandlerEmbedderTest, NoPassword) {
  EXPECT_FALSE(OpenDocument("encrypted.pdf"));
}

TEST_F(CPDFSecurityHandlerEmbedderTest, BadPassword) {
  EXPECT_FALSE(OpenDocumentWithPassword("encrypted.pdf", "tiger"));
}

TEST_F(CPDFSecurityHandlerEmbedderTest, UserPassword) {
  ASSERT_TRUE(OpenDocumentWithPassword("encrypted.pdf", "1234"));
  EXPECT_EQ(0xFFFFF2C0, FPDF_GetDocPermissions(document()));
}

TEST_F(CPDFSecurityHandlerEmbedderTest, OwnerPassword) {
  ASSERT_TRUE(OpenDocumentWithPassword("encrypted.pdf", "5678"));
  EXPECT_EQ(0xFFFFFFFC, FPDF_GetDocPermissions(document()));
}

TEST_F(CPDFSecurityHandlerEmbedderTest, PasswordAfterGenerateSave) {
#if _FX_PLATFORM_ == _FX_PLATFORM_LINUX_
  const char md5[] = "7048dca58e2ed8f93339008b91e4eb4e";
#elif _FX_PLATFORM_ == _FX_PLATFORM_APPLE_
  const char md5[] = "6951b6c9891dfe0332a5b1983e484400";
#else
  const char md5[] = "041c2fb541c8907cc22ce101b686c79e";
#endif  // _FX_PLATFORM_ == _FX_PLATFORM_LINUX_
  {
    ASSERT_TRUE(OpenDocumentWithOptions("encrypted.pdf", "5678",
                                        LinearizeOption::kMustLinearize,
                                        JavaScriptOption::kEnableJavaScript));
    FPDF_PAGE page = LoadPage(0);
    ASSERT_TRUE(page);
    FPDF_PAGEOBJECT red_rect = FPDFPageObj_CreateNewRect(10, 10, 20, 20);
    ASSERT_TRUE(red_rect);
    EXPECT_TRUE(FPDFPath_SetFillColor(red_rect, 255, 0, 0, 255));
    EXPECT_TRUE(FPDFPath_SetDrawMode(red_rect, FPDF_FILLMODE_ALTERNATE, 0));
    FPDFPage_InsertObject(page, red_rect);
    ScopedFPDFBitmap bitmap = RenderLoadedPage(page);
    CompareBitmap(bitmap.get(), 612, 792, md5);
    EXPECT_TRUE(FPDFPage_GenerateContent(page));
    SetWholeFileAvailable();
    EXPECT_TRUE(FPDF_SaveAsCopy(document(), this, 0));
    UnloadPage(page);
  }
  std::string new_file = GetString();
  FPDF_FILEACCESS file_access;
  memset(&file_access, 0, sizeof(file_access));
  file_access.m_FileLen = new_file.size();
  file_access.m_GetBlock = GetBlockFromString;
  file_access.m_Param = &new_file;
  EXPECT_FALSE(FPDF_LoadCustomDocument(&file_access, nullptr));

  struct {
    const char* password;
    const unsigned long permissions;
  } tests[] = {{"1234", 0xFFFFF2C0}, {"5678", 0xFFFFFFFC}};

  for (const auto& test : tests) {
    OpenSavedDocument(test.password);
    FPDF_PAGE page = LoadSavedPage(0);
    VerifySavedRendering(page, 612, 792, md5);
    EXPECT_EQ(test.permissions, FPDF_GetDocPermissions(saved_document_));

    CloseSavedPage(page);
    CloseSavedDocument();
  }
}

TEST_F(CPDFSecurityHandlerEmbedderTest, NoPasswordVersion5) {
  ASSERT_FALSE(OpenDocument("bug_644.pdf"));
}

TEST_F(CPDFSecurityHandlerEmbedderTest, BadPasswordVersion5) {
  ASSERT_FALSE(OpenDocumentWithPassword("bug_644.pdf", "tiger"));
}

TEST_F(CPDFSecurityHandlerEmbedderTest, OwnerPasswordVersion5) {
  ASSERT_TRUE(OpenDocumentWithPassword("bug_644.pdf", "a"));
  EXPECT_EQ(0xFFFFFFFC, FPDF_GetDocPermissions(document()));
}

TEST_F(CPDFSecurityHandlerEmbedderTest, UserPasswordVersion5) {
  ASSERT_TRUE(OpenDocumentWithPassword("bug_644.pdf", "b"));
  EXPECT_EQ(0xFFFFFFFC, FPDF_GetDocPermissions(document()));
}

TEST_F(CPDFSecurityHandlerEmbedderTest, OwnerPasswordVersion2UTF8) {
  // The password is "age", where the 'a' has a circumflex. Encoding the
  // password as UTF-8 works.
  ASSERT_TRUE(
      OpenDocumentWithPassword("encrypted_hello_world_r2.pdf", kAgeUTF8));
}

TEST_F(CPDFSecurityHandlerEmbedderTest, OwnerPasswordVersion2Latin1) {
  // The same password encoded as Latin-1 also works at revision 2.
  ASSERT_TRUE(
      OpenDocumentWithPassword("encrypted_hello_world_r2.pdf", kAgeLatin1));
  EXPECT_EQ(2, FPDF_GetSecurityHandlerRevision(document()));
}

TEST_F(CPDFSecurityHandlerEmbedderTest, OwnerPasswordVersion3UTF8) {
  // Same as OwnerPasswordVersion2UTF8 test above.
  ASSERT_TRUE(
      OpenDocumentWithPassword("encrypted_hello_world_r3.pdf", kAgeUTF8));
}

TEST_F(CPDFSecurityHandlerEmbedderTest, OwnerPasswordVersion3Latin1) {
  // Same as OwnerPasswordVersion2Latin1 test above.
  ASSERT_TRUE(
      OpenDocumentWithPassword("encrypted_hello_world_r3.pdf", kAgeLatin1));
  EXPECT_EQ(3, FPDF_GetSecurityHandlerRevision(document()));
}

TEST_F(CPDFSecurityHandlerEmbedderTest, OwnerPasswordVersion5UTF8) {
  // Same as OwnerPasswordVersion2UTF8 test above.
  ASSERT_TRUE(
      OpenDocumentWithPassword("encrypted_hello_world_r5.pdf", kAgeUTF8));
  EXPECT_EQ(5, FPDF_GetSecurityHandlerRevision(document()));
}

TEST_F(CPDFSecurityHandlerEmbedderTest, OwnerPasswordVersion5Latin1) {
  // Same as OwnerPasswordVersion2Latin1 test above.
  ASSERT_TRUE(
      OpenDocumentWithPassword("encrypted_hello_world_r5.pdf", kAgeLatin1));
}

TEST_F(CPDFSecurityHandlerEmbedderTest, OwnerPasswordVersion6UTF8) {
  // Same as OwnerPasswordVersion2UTF8 test above.
  ASSERT_TRUE(
      OpenDocumentWithPassword("encrypted_hello_world_r6.pdf", kAgeUTF8));
  EXPECT_EQ(6, FPDF_GetSecurityHandlerRevision(document()));
}

TEST_F(CPDFSecurityHandlerEmbedderTest, OwnerPasswordVersion6Latin1) {
  // Same as OwnerPasswordVersion2Latin1 test above.
  ASSERT_TRUE(
      OpenDocumentWithPassword("encrypted_hello_world_r6.pdf", kAgeLatin1));
}

TEST_F(CPDFSecurityHandlerEmbedderTest, UserPasswordVersion2UTF8) {
  // The password is "hotel", where the 'o' has a circumflex. Encoding the
  // password as UTF-8 works.
  ASSERT_TRUE(
      OpenDocumentWithPassword("encrypted_hello_world_r2.pdf", kHotelUTF8));
}

TEST_F(CPDFSecurityHandlerEmbedderTest, UserPasswordVersion2Latin1) {
  // The same password encoded as Latin-1 also works at revision 2.
  ASSERT_TRUE(
      OpenDocumentWithPassword("encrypted_hello_world_r2.pdf", kHotelLatin1));
  EXPECT_EQ(2, FPDF_GetSecurityHandlerRevision(document()));
}

TEST_F(CPDFSecurityHandlerEmbedderTest, UserPasswordVersion3UTF8) {
  // Same as UserPasswordVersion2UTF8 test above.
  ASSERT_TRUE(
      OpenDocumentWithPassword("encrypted_hello_world_r3.pdf", kHotelUTF8));
}

TEST_F(CPDFSecurityHandlerEmbedderTest, UserPasswordVersion3Latin1) {
  // Same as UserPasswordVersion2Latin1 test above.
  ASSERT_TRUE(
      OpenDocumentWithPassword("encrypted_hello_world_r3.pdf", kHotelLatin1));
  EXPECT_EQ(3, FPDF_GetSecurityHandlerRevision(document()));
}

TEST_F(CPDFSecurityHandlerEmbedderTest, UserPasswordVersion5UTF8) {
  // Same as UserPasswordVersion2UTF8 test above.
  ASSERT_TRUE(
      OpenDocumentWithPassword("encrypted_hello_world_r5.pdf", kHotelUTF8));
  EXPECT_EQ(5, FPDF_GetSecurityHandlerRevision(document()));
}

TEST_F(CPDFSecurityHandlerEmbedderTest, UserPasswordVersion5Latin1) {
  // Same as UserPasswordVersion2Latin1 test above.
  ASSERT_TRUE(
      OpenDocumentWithPassword("encrypted_hello_world_r5.pdf", kHotelLatin1));
}

TEST_F(CPDFSecurityHandlerEmbedderTest, UserPasswordVersion6UTF8) {
  // Same as UserPasswordVersion2UTF8 test above.
  ASSERT_TRUE(
      OpenDocumentWithPassword("encrypted_hello_world_r6.pdf", kHotelUTF8));
  EXPECT_EQ(6, FPDF_GetSecurityHandlerRevision(document()));
}

TEST_F(CPDFSecurityHandlerEmbedderTest, UserPasswordVersion6Latin1) {
  // Same as UserPasswordVersion2Latin1 test above.
  ASSERT_TRUE(
      OpenDocumentWithPassword("encrypted_hello_world_r6.pdf", kHotelLatin1));
}
