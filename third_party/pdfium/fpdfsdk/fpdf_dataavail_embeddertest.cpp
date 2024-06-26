// Copyright 2015 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <algorithm>
#include <memory>
#include <set>
#include <string>
#include <utility>
#include <vector>

#include "core/fxcrt/bytestring.h"
#include "core/fxcrt/widestring.h"
#include "public/fpdfview.h"
#include "testing/embedder_test.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "testing/range_set.h"
#include "testing/test_support.h"
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

class TestAsyncLoader final : public FX_DOWNLOADHINTS, FX_FILEAVAIL {
 public:
  explicit TestAsyncLoader(const std::string& file_name) {
    std::string file_path;
    if (!PathService::GetTestFilePath(file_name, &file_path))
      return;
    file_contents_ = GetFileContents(file_path.c_str(), &file_length_);
    if (!file_contents_)
      return;

    file_access_.m_FileLen = static_cast<unsigned long>(file_length_);
    file_access_.m_GetBlock = SGetBlock;
    file_access_.m_Param = this;

    FX_DOWNLOADHINTS::version = 1;
    FX_DOWNLOADHINTS::AddSegment = SAddSegment;

    FX_FILEAVAIL::version = 1;
    FX_FILEAVAIL::IsDataAvail = SIsDataAvail;
  }

  bool IsOpened() const { return !!file_contents_; }

  FPDF_FILEACCESS* file_access() { return &file_access_; }
  FX_DOWNLOADHINTS* hints() { return this; }
  FX_FILEAVAIL* file_avail() { return this; }

  const std::vector<std::pair<size_t, size_t>>& requested_segments() const {
    return requested_segments_;
  }

  size_t max_requested_bound() const { return max_requested_bound_; }

  void ClearRequestedSegments() {
    requested_segments_.clear();
    max_requested_bound_ = 0;
  }

  bool is_new_data_available() const { return is_new_data_available_; }
  void set_is_new_data_available(bool is_new_data_available) {
    is_new_data_available_ = is_new_data_available;
  }

  size_t max_already_available_bound() const {
    return available_ranges_.IsEmpty()
               ? 0
               : available_ranges_.ranges().rbegin()->second;
  }

  void FlushRequestedData() {
    for (const auto& it : requested_segments_) {
      SetDataAvailable(it.first, it.second);
    }
    ClearRequestedSegments();
  }

  char* file_contents() { return file_contents_.get(); }
  size_t file_length() const { return file_length_; }

 private:
  void SetDataAvailable(size_t start, size_t size) {
    available_ranges_.Union(RangeSet::Range(start, start + size));
  }

  bool CheckDataAlreadyAvailable(size_t start, size_t size) const {
    return available_ranges_.Contains(RangeSet::Range(start, start + size));
  }

  int GetBlockImpl(unsigned long pos, unsigned char* pBuf, unsigned long size) {
    if (!IsDataAvailImpl(pos, size))
      return 0;
    const unsigned long end =
        std::min(static_cast<unsigned long>(file_length_), pos + size);
    if (end <= pos)
      return 0;
    memcpy(pBuf, file_contents_.get() + pos, end - pos);
    SetDataAvailable(pos, end - pos);
    return static_cast<int>(end - pos);
  }

  void AddSegmentImpl(size_t offset, size_t size) {
    requested_segments_.push_back(std::make_pair(offset, size));
    max_requested_bound_ = std::max(max_requested_bound_, offset + size);
  }

  bool IsDataAvailImpl(size_t offset, size_t size) {
    if (offset + size > file_length_)
      return false;
    if (is_new_data_available_) {
      SetDataAvailable(offset, size);
      return true;
    }
    return CheckDataAlreadyAvailable(offset, size);
  }

  static int SGetBlock(void* param,
                       unsigned long pos,
                       unsigned char* pBuf,
                       unsigned long size) {
    return static_cast<TestAsyncLoader*>(param)->GetBlockImpl(pos, pBuf, size);
  }

  static void SAddSegment(FX_DOWNLOADHINTS* pThis, size_t offset, size_t size) {
    return static_cast<TestAsyncLoader*>(pThis)->AddSegmentImpl(offset, size);
  }

  static FPDF_BOOL SIsDataAvail(FX_FILEAVAIL* pThis,
                                size_t offset,
                                size_t size) {
    return static_cast<TestAsyncLoader*>(pThis)->IsDataAvailImpl(offset, size);
  }

  FPDF_FILEACCESS file_access_;

  std::unique_ptr<char, pdfium::FreeDeleter> file_contents_;
  size_t file_length_;
  std::vector<std::pair<size_t, size_t>> requested_segments_;
  size_t max_requested_bound_ = 0;
  bool is_new_data_available_ = true;

  RangeSet available_ranges_;
};

}  // namespace

class FPDFDataAvailEmbedderTest : public EmbedderTest {};

TEST_F(FPDFDataAvailEmbedderTest, TrailerUnterminated) {
  // Document must load without crashing but is too malformed to be available.
  EXPECT_FALSE(OpenDocument("trailer_unterminated.pdf"));
  MockDownloadHints hints;
  EXPECT_FALSE(FPDFAvail_IsDocAvail(avail_, &hints));
}

TEST_F(FPDFDataAvailEmbedderTest, TrailerAsHexstring) {
  // Document must load without crashing but is too malformed to be available.
  EXPECT_FALSE(OpenDocument("trailer_as_hexstring.pdf"));
  MockDownloadHints hints;
  EXPECT_FALSE(FPDFAvail_IsDocAvail(avail_, &hints));
}

TEST_F(FPDFDataAvailEmbedderTest, LoadUsingHintTables) {
  TestAsyncLoader loader("feature_linearized_loading.pdf");
  avail_ = FPDFAvail_Create(loader.file_avail(), loader.file_access());
  ASSERT_EQ(PDF_DATA_AVAIL, FPDFAvail_IsDocAvail(avail_, loader.hints()));
  document_ = FPDFAvail_GetDocument(avail_, nullptr);
  ASSERT_TRUE(document_);
  ASSERT_EQ(PDF_DATA_AVAIL, FPDFAvail_IsPageAvail(avail_, 1, loader.hints()));

  // No new data available, to prevent load "Pages" node.
  loader.set_is_new_data_available(false);
  FPDF_PAGE page = FPDF_LoadPage(document(), 1);
  EXPECT_TRUE(page);
  FPDF_ClosePage(page);
}

TEST_F(FPDFDataAvailEmbedderTest, CheckFormAvailIfLinearized) {
  TestAsyncLoader loader("feature_linearized_loading.pdf");
  avail_ = FPDFAvail_Create(loader.file_avail(), loader.file_access());
  ASSERT_EQ(PDF_DATA_AVAIL, FPDFAvail_IsDocAvail(avail_, loader.hints()));
  document_ = FPDFAvail_GetDocument(avail_, nullptr);
  ASSERT_TRUE(document_);

  // Prevent access to non requested data to coerce the parser to send new
  // request for non available (non requested before) data.
  loader.set_is_new_data_available(false);
  loader.ClearRequestedSegments();

  int status = PDF_FORM_NOTAVAIL;
  while (status == PDF_FORM_NOTAVAIL) {
    loader.FlushRequestedData();
    status = FPDFAvail_IsFormAvail(avail_, loader.hints());
  }
  EXPECT_NE(PDF_FORM_ERROR, status);
}

TEST_F(FPDFDataAvailEmbedderTest,
       DoNotLoadMainCrossRefForFirstPageIfLinearized) {
  TestAsyncLoader loader("feature_linearized_loading.pdf");
  avail_ = FPDFAvail_Create(loader.file_avail(), loader.file_access());
  ASSERT_EQ(PDF_DATA_AVAIL, FPDFAvail_IsDocAvail(avail_, loader.hints()));
  document_ = FPDFAvail_GetDocument(avail_, nullptr);
  ASSERT_TRUE(document_);
  const int first_page_num = FPDFAvail_GetFirstPageNum(document_);

  // The main cross ref table should not be processed.
  // (It is always at file end)
  EXPECT_GT(loader.file_access()->m_FileLen,
            loader.max_already_available_bound());

  // Prevent access to non requested data to coerce the parser to send new
  // request for non available (non requested before) data.
  loader.set_is_new_data_available(false);
  FPDFAvail_IsPageAvail(avail_, first_page_num, loader.hints());

  // The main cross ref table should not be requested.
  // (It is always at file end)
  EXPECT_GT(loader.file_access()->m_FileLen, loader.max_requested_bound());

  // Allow parse page.
  loader.set_is_new_data_available(true);
  ASSERT_EQ(PDF_DATA_AVAIL,
            FPDFAvail_IsPageAvail(avail_, first_page_num, loader.hints()));

  // The main cross ref table should not be processed.
  // (It is always at file end)
  EXPECT_GT(loader.file_access()->m_FileLen,
            loader.max_already_available_bound());

  // Prevent loading data, while page loading.
  loader.set_is_new_data_available(false);
  FPDF_PAGE page = FPDF_LoadPage(document(), first_page_num);
  EXPECT_TRUE(page);
  FPDF_ClosePage(page);
}

TEST_F(FPDFDataAvailEmbedderTest, LoadSecondPageIfLinearizedWithHints) {
  TestAsyncLoader loader("feature_linearized_loading.pdf");
  avail_ = FPDFAvail_Create(loader.file_avail(), loader.file_access());
  ASSERT_EQ(PDF_DATA_AVAIL, FPDFAvail_IsDocAvail(avail_, loader.hints()));
  document_ = FPDFAvail_GetDocument(avail_, nullptr);
  ASSERT_TRUE(document_);

  static constexpr uint32_t kSecondPageNum = 1;

  // Prevent access to non requested data to coerce the parser to send new
  // request for non available (non requested before) data.
  loader.set_is_new_data_available(false);
  loader.ClearRequestedSegments();

  int status = PDF_DATA_NOTAVAIL;
  while (status == PDF_DATA_NOTAVAIL) {
    loader.FlushRequestedData();
    status = FPDFAvail_IsPageAvail(avail_, kSecondPageNum, loader.hints());
  }
  EXPECT_EQ(PDF_DATA_AVAIL, status);

  // Prevent loading data, while page loading.
  loader.set_is_new_data_available(false);
  FPDF_PAGE page = FPDF_LoadPage(document(), kSecondPageNum);
  EXPECT_TRUE(page);
  FPDF_ClosePage(page);
}

TEST_F(FPDFDataAvailEmbedderTest, LoadInfoAfterReceivingWholeDocument) {
  TestAsyncLoader loader("linearized.pdf");
  loader.set_is_new_data_available(false);
  avail_ = FPDFAvail_Create(loader.file_avail(), loader.file_access());
  while (PDF_DATA_AVAIL != FPDFAvail_IsDocAvail(avail_, loader.hints())) {
    loader.FlushRequestedData();
  }

  document_ = FPDFAvail_GetDocument(avail_, nullptr);
  ASSERT_TRUE(document_);

  // The "info" dictionary should still be unavailable.
  EXPECT_FALSE(FPDF_GetMetaText(document_, "CreationDate", nullptr, 0));

  // Simulate receiving whole file.
  loader.set_is_new_data_available(true);
  // Load second page, to parse additional crossref sections.
  EXPECT_EQ(PDF_DATA_AVAIL, FPDFAvail_IsPageAvail(avail_, 1, loader.hints()));

  EXPECT_TRUE(FPDF_GetMetaText(document_, "CreationDate", nullptr, 0));
}

TEST_F(FPDFDataAvailEmbedderTest, LoadInfoAfterReceivingFirstPage) {
  TestAsyncLoader loader("linearized.pdf");
  // Map "Info" to an object within the first section without breaking
  // linearization.
  ByteString data(loader.file_contents(), loader.file_length());
  Optional<size_t> index = data.Find("/Info 27 0 R");
  ASSERT_TRUE(index);
  memcpy(loader.file_contents() + *index, "/Info 29 0 R", 12);

  loader.set_is_new_data_available(false);
  avail_ = FPDFAvail_Create(loader.file_avail(), loader.file_access());
  while (PDF_DATA_AVAIL != FPDFAvail_IsDocAvail(avail_, loader.hints())) {
    loader.FlushRequestedData();
  }

  document_ = FPDFAvail_GetDocument(avail_, nullptr);
  ASSERT_TRUE(document_);

  // The "Info" dictionary should be available for the linearized document, if
  // it is located in the first page section.
  // Info was remapped to a dictionary with Type "Catalog"
  unsigned short buffer[100] = {0};
  EXPECT_TRUE(FPDF_GetMetaText(document_, "Type", buffer, sizeof(buffer)));
  constexpr wchar_t kExpectedValue[] = L"Catalog";
  EXPECT_EQ(WideString(kExpectedValue),
            WideString::FromUTF16LE(buffer, FXSYS_len(kExpectedValue)));
}

TEST_F(FPDFDataAvailEmbedderTest, TryLoadInvalidInfo) {
  TestAsyncLoader loader("linearized.pdf");
  // Map "Info" to an invalid object without breaking linearization.
  ByteString data(loader.file_contents(), loader.file_length());
  Optional<size_t> index = data.Find("/Info 27 0 R");
  ASSERT_TRUE(index);
  memcpy(loader.file_contents() + *index, "/Info 99 0 R", 12);

  loader.set_is_new_data_available(false);
  avail_ = FPDFAvail_Create(loader.file_avail(), loader.file_access());
  while (PDF_DATA_AVAIL != FPDFAvail_IsDocAvail(avail_, loader.hints())) {
    loader.FlushRequestedData();
  }

  document_ = FPDFAvail_GetDocument(avail_, nullptr);
  ASSERT_TRUE(document_);

  // Set all data available.
  loader.set_is_new_data_available(true);
  // Check second page, to load additional crossrefs.
  ASSERT_EQ(PDF_DATA_AVAIL, FPDFAvail_IsPageAvail(avail_, 0, loader.hints()));

  // Test that api is robust enough to handle the bad case.
  EXPECT_FALSE(FPDF_GetMetaText(document_, "Type", nullptr, 0));
}

TEST_F(FPDFDataAvailEmbedderTest, TryLoadNonExistsInfo) {
  TestAsyncLoader loader("linearized.pdf");
  // Break the "Info" parameter without breaking linearization.
  ByteString data(loader.file_contents(), loader.file_length());
  Optional<size_t> index = data.Find("/Info 27 0 R");
  ASSERT_TRUE(index);
  memcpy(loader.file_contents() + *index, "/I_fo 27 0 R", 12);

  loader.set_is_new_data_available(false);
  avail_ = FPDFAvail_Create(loader.file_avail(), loader.file_access());
  while (PDF_DATA_AVAIL != FPDFAvail_IsDocAvail(avail_, loader.hints())) {
    loader.FlushRequestedData();
  }

  document_ = FPDFAvail_GetDocument(avail_, nullptr);
  ASSERT_TRUE(document_);

  // Set all data available.
  loader.set_is_new_data_available(true);
  // Check second page, to load additional crossrefs.
  ASSERT_EQ(PDF_DATA_AVAIL, FPDFAvail_IsPageAvail(avail_, 0, loader.hints()));

  // Test that api is robust enough to handle the bad case.
  EXPECT_FALSE(FPDF_GetMetaText(document_, "Type", nullptr, 0));
}
