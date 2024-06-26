// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "public/fpdf_catalog.h"

#include <memory>

#include "core/fpdfapi/cpdf_modulemgr.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_document.h"
#include "core/fpdfapi/parser/cpdf_number.h"
#include "core/fpdfapi/parser/cpdf_parser.h"
#include "core/fpdfapi/parser/cpdf_string.h"
#include "fpdfsdk/cpdfsdk_helpers.h"
#include "public/cpp/fpdf_scopers.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "testing/test_support.h"
#include "third_party/base/ptr_util.h"

class CPDF_TestDocument final : public CPDF_Document {
 public:
  CPDF_TestDocument() : CPDF_Document() {}

  void SetRoot(CPDF_Dictionary* root) {
    m_pRootDict = root;
  }
};

class PDFCatalogTest : public testing::Test {
 public:
  void SetUp() override {
    CPDF_ModuleMgr::Get()->Init();
    auto pTestDoc = pdfium::MakeUnique<CPDF_TestDocument>();
    m_pDoc.reset(FPDFDocumentFromCPDFDocument(pTestDoc.release()));
    m_pRootObj = pdfium::MakeUnique<CPDF_Dictionary>();
  }

  void TearDown() override {
    m_pDoc.reset();
    CPDF_ModuleMgr::Destroy();
  }

 protected:
  ScopedFPDFDocument m_pDoc;
  std::unique_ptr<CPDF_Dictionary> m_pRootObj;
};

TEST_F(PDFCatalogTest, IsTagged) {
  // Null doc
  EXPECT_FALSE(FPDFCatalog_IsTagged(nullptr));

  CPDF_TestDocument* pTestDoc = static_cast<CPDF_TestDocument*>(
      CPDFDocumentFromFPDFDocument(m_pDoc.get()));

  // No root
  pTestDoc->SetRoot(nullptr);
  EXPECT_FALSE(FPDFCatalog_IsTagged(m_pDoc.get()));

  // Empty root
  pTestDoc->SetRoot(m_pRootObj.get());
  EXPECT_FALSE(FPDFCatalog_IsTagged(m_pDoc.get()));

  // Root with other key
  m_pRootObj->SetNewFor<CPDF_String>("OTHER_KEY", "other value", false);
  EXPECT_FALSE(FPDFCatalog_IsTagged(m_pDoc.get()));

  // Root with empty MarkInfo
  CPDF_Dictionary* markInfoDict =
      m_pRootObj->SetNewFor<CPDF_Dictionary>("MarkInfo");
  EXPECT_FALSE(FPDFCatalog_IsTagged(m_pDoc.get()));

  // MarkInfo present but Marked is 0
  markInfoDict->SetNewFor<CPDF_Number>("Marked", 0);
  EXPECT_FALSE(FPDFCatalog_IsTagged(m_pDoc.get()));

  // MarkInfo present and Marked is 1, PDF is considered tagged.
  markInfoDict->SetNewFor<CPDF_Number>("Marked", 1);
  EXPECT_TRUE(FPDFCatalog_IsTagged(m_pDoc.get()));
}
