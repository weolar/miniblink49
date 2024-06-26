// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/fpdfapi/parser/cpdf_read_validator.h"

#include <limits>
#include <utility>
#include <vector>

#include "core/fxcrt/cfx_readonlymemorystream.h"
#include "testing/fx_string_testhelpers.h"
#include "testing/gtest/include/gtest/gtest.h"

namespace {

constexpr uint32_t kTestDataSize = 64 * 1024 - 467;

std::pair<FX_FILESIZE, FX_FILESIZE> MakeRange(uint32_t start, uint32_t end) {
  return std::pair<FX_FILESIZE, FX_FILESIZE>(start, end);
}

class MockFileAvail final : public CPDF_DataAvail::FileAvail {
 public:
  MockFileAvail() : available_range_(0, 0) {}
  ~MockFileAvail() override {}

  bool IsDataAvail(FX_FILESIZE offset, size_t size) override {
    return available_range_.first <= offset &&
           available_range_.second >= static_cast<FX_FILESIZE>(offset + size);
  }

  void SetAvailableRange(const std::pair<FX_FILESIZE, FX_FILESIZE>& range) {
    available_range_ = range;
  }

  void SetAvailableRange(uint32_t start, uint32_t end) {
    SetAvailableRange(MakeRange(start, end));
  }

 private:
  std::pair<FX_FILESIZE, FX_FILESIZE> available_range_;
};

class MockDownloadHints final : public CPDF_DataAvail::DownloadHints {
 public:
  MockDownloadHints() : last_requested_range_(0, 0) {}
  ~MockDownloadHints() override {}

  void AddSegment(FX_FILESIZE offset, size_t size) override {
    last_requested_range_.first = offset;
    last_requested_range_.second = offset + size;
  }

  const std::pair<FX_FILESIZE, FX_FILESIZE>& GetLastRequstedRange() const {
    return last_requested_range_;
  }

  void Reset() { last_requested_range_ = MakeRange(0, 0); }

 private:
  std::pair<FX_FILESIZE, FX_FILESIZE> last_requested_range_;
};

}  // namespace

TEST(CPDF_ReadValidatorTest, UnavailableData) {
  std::vector<uint8_t> test_data(kTestDataSize);
  auto file = pdfium::MakeRetain<CFX_ReadOnlyMemoryStream>(test_data);
  MockFileAvail file_avail;
  auto validator = pdfium::MakeRetain<CPDF_ReadValidator>(file, &file_avail);

  std::vector<uint8_t> read_buffer(100);
  EXPECT_FALSE(validator->ReadBlockAtOffset(read_buffer.data(), 5000,
                                            read_buffer.size()));

  EXPECT_FALSE(validator->read_error());
  EXPECT_TRUE(validator->has_unavailable_data());

  validator->ResetErrors();

  file_avail.SetAvailableRange(5000, 5000 + read_buffer.size());

  EXPECT_TRUE(validator->ReadBlockAtOffset(read_buffer.data(), 5000,
                                           read_buffer.size()));
  EXPECT_FALSE(validator->read_error());
  EXPECT_FALSE(validator->has_unavailable_data());
}

TEST(CPDF_ReadValidatorTest, UnavailableDataWithHints) {
  std::vector<uint8_t> test_data(kTestDataSize);
  auto file = pdfium::MakeRetain<CFX_ReadOnlyMemoryStream>(test_data);
  MockFileAvail file_avail;
  auto validator = pdfium::MakeRetain<CPDF_ReadValidator>(file, &file_avail);

  MockDownloadHints hints;
  validator->SetDownloadHints(&hints);

  std::vector<uint8_t> read_buffer(100);

  EXPECT_FALSE(validator->ReadBlockAtOffset(read_buffer.data(), 5000,
                                            read_buffer.size()));
  EXPECT_FALSE(validator->read_error());
  EXPECT_TRUE(validator->has_unavailable_data());

  // Requested range should be enlarged and aligned.
  EXPECT_EQ(MakeRange(4608, 5120), hints.GetLastRequstedRange());

  file_avail.SetAvailableRange(hints.GetLastRequstedRange());
  hints.Reset();

  validator->ResetErrors();
  EXPECT_TRUE(validator->ReadBlockAtOffset(read_buffer.data(), 5000,
                                           read_buffer.size()));
  // No new request on already available data.
  EXPECT_EQ(MakeRange(0, 0), hints.GetLastRequstedRange());
  EXPECT_FALSE(validator->read_error());
  EXPECT_FALSE(validator->has_unavailable_data());

  validator->ResetErrors();
  // Try read unavailable data at file end.
  EXPECT_FALSE(validator->ReadBlockAtOffset(
      read_buffer.data(), validator->GetSize() - read_buffer.size(),
      read_buffer.size()));
  // Should not enlarge request at file end.
  EXPECT_EQ(validator->GetSize(), hints.GetLastRequstedRange().second);
  EXPECT_FALSE(validator->read_error());
  EXPECT_TRUE(validator->has_unavailable_data());

  validator->SetDownloadHints(nullptr);
}

TEST(CPDF_ReadValidatorTest, ReadError) {
  auto file = pdfium::MakeRetain<CFX_InvalidSeekableReadStream>(kTestDataSize);
  auto validator = pdfium::MakeRetain<CPDF_ReadValidator>(file, nullptr);

  static const uint32_t kBufferSize = 3 * 1000;
  std::vector<uint8_t> buffer(kBufferSize);

  EXPECT_FALSE(validator->ReadBlockAtOffset(buffer.data(), 5000, 100));
  EXPECT_TRUE(validator->read_error());
  EXPECT_TRUE(validator->has_unavailable_data());
}

TEST(CPDF_ReadValidatorTest, IntOverflow) {
  std::vector<uint8_t> test_data(kTestDataSize);
  auto file = pdfium::MakeRetain<CFX_ReadOnlyMemoryStream>(test_data);
  MockFileAvail file_avail;
  auto validator = pdfium::MakeRetain<CPDF_ReadValidator>(file, &file_avail);

  std::vector<uint8_t> read_buffer(100);

  // If we have int overflow, this is equal reading after file end. This is not
  // read_error, and in this case we have not unavailable data. It is just error
  // of input params.
  EXPECT_FALSE(validator->ReadBlockAtOffset(
      read_buffer.data(), std::numeric_limits<FX_FILESIZE>::max() - 1,
      read_buffer.size()));
  EXPECT_FALSE(validator->read_error());
  EXPECT_FALSE(validator->has_unavailable_data());
}

TEST(CPDF_ReadValidatorTest, Session) {
  std::vector<uint8_t> test_data(kTestDataSize);

  auto file = pdfium::MakeRetain<CFX_InvalidSeekableReadStream>(kTestDataSize);
  MockFileAvail file_avail;
  MockDownloadHints hints;
  auto validator = pdfium::MakeRetain<CPDF_ReadValidator>(file, &file_avail);
  validator->SetDownloadHints(&hints);

  const CPDF_ReadValidator::Session read_session(validator.Get());
  ASSERT_FALSE(validator->has_read_problems());

  // Data is unavailable
  validator->ReadBlockAtOffset(test_data.data(), 0, 100);

  EXPECT_TRUE(validator->has_read_problems());
  EXPECT_TRUE(validator->has_unavailable_data());
  EXPECT_FALSE(validator->read_error());

  {
    const CPDF_ReadValidator::Session read_subsession(validator.Get());
    // The read problems should be hidden.
    EXPECT_FALSE(validator->has_read_problems());

    file_avail.SetAvailableRange(0, 100);
    // Read fail.
    validator->ReadBlockAtOffset(test_data.data(), 0, 100);
    EXPECT_TRUE(validator->has_read_problems());
    EXPECT_TRUE(validator->has_unavailable_data());
    EXPECT_TRUE(validator->read_error());
  }

  // The problems should be merged
  EXPECT_TRUE(validator->has_read_problems());
  EXPECT_TRUE(validator->has_unavailable_data());
  EXPECT_TRUE(validator->read_error());
}

TEST(CPDF_ReadValidatorTest, SessionReset) {
  std::vector<uint8_t> test_data(kTestDataSize);

  auto file = pdfium::MakeRetain<CFX_InvalidSeekableReadStream>(kTestDataSize);
  MockFileAvail file_avail;
  MockDownloadHints hints;
  auto validator = pdfium::MakeRetain<CPDF_ReadValidator>(file, &file_avail);
  validator->SetDownloadHints(&hints);

  const CPDF_ReadValidator::Session read_session(validator.Get());
  ASSERT_FALSE(validator->has_read_problems());

  // Data is unavailable
  validator->ReadBlockAtOffset(test_data.data(), 0, 100);

  EXPECT_TRUE(validator->has_read_problems());
  EXPECT_TRUE(validator->has_unavailable_data());
  EXPECT_FALSE(validator->read_error());

  {
    const CPDF_ReadValidator::Session read_subsession(validator.Get());
    // The read problems should be hidden.
    EXPECT_FALSE(validator->has_read_problems());

    file_avail.SetAvailableRange(0, 100);
    // Read fail.
    validator->ReadBlockAtOffset(test_data.data(), 0, 100);
    EXPECT_TRUE(validator->has_read_problems());
    EXPECT_TRUE(validator->has_unavailable_data());
    EXPECT_TRUE(validator->read_error());

    // Reset session.
    validator->ResetErrors();
    EXPECT_FALSE(validator->has_read_problems());
  }

  // The problems should be restored.
  EXPECT_TRUE(validator->has_read_problems());
  EXPECT_TRUE(validator->has_unavailable_data());
  EXPECT_FALSE(validator->read_error());
}

TEST(CPDF_ReadValidatorTest, CheckDataRangeAndRequestIfUnavailable) {
  std::vector<uint8_t> test_data(kTestDataSize);
  auto file = pdfium::MakeRetain<CFX_ReadOnlyMemoryStream>(test_data);
  MockFileAvail file_avail;
  auto validator = pdfium::MakeRetain<CPDF_ReadValidator>(file, &file_avail);

  MockDownloadHints hints;
  validator->SetDownloadHints(&hints);

  EXPECT_FALSE(validator->CheckDataRangeAndRequestIfUnavailable(5000, 100));
  EXPECT_FALSE(validator->read_error());
  EXPECT_TRUE(validator->has_unavailable_data());

  // Requested range should be enlarged and aligned.
  EXPECT_EQ(MakeRange(4608, 5632), hints.GetLastRequstedRange());

  file_avail.SetAvailableRange(hints.GetLastRequstedRange());
  hints.Reset();

  validator->ResetErrors();
  EXPECT_TRUE(validator->CheckDataRangeAndRequestIfUnavailable(5000, 100));
  // No new request on already available data.
  EXPECT_EQ(MakeRange(0, 0), hints.GetLastRequstedRange());
  EXPECT_FALSE(validator->read_error());
  EXPECT_FALSE(validator->has_unavailable_data());

  std::vector<uint8_t> read_buffer(100);
  EXPECT_TRUE(validator->ReadBlockAtOffset(read_buffer.data(), 5000,
                                           read_buffer.size()));
  // No new request on already available data.
  EXPECT_EQ(MakeRange(0, 0), hints.GetLastRequstedRange());
  EXPECT_FALSE(validator->read_error());
  EXPECT_FALSE(validator->has_unavailable_data());

  validator->ResetErrors();
  // Try request unavailable data at file end.
  EXPECT_FALSE(validator->CheckDataRangeAndRequestIfUnavailable(
      validator->GetSize() - 100, 100));

  // Should not enlarge request at file end.
  EXPECT_EQ(validator->GetSize(), hints.GetLastRequstedRange().second);
  EXPECT_FALSE(validator->read_error());
  EXPECT_TRUE(validator->has_unavailable_data());

  validator->ResetErrors();
  // Offset > file size should yield |true| and not cause a fetch.
  EXPECT_TRUE(
      validator->CheckDataRangeAndRequestIfUnavailable(kTestDataSize + 1, 1));
  // No new request on already available data.
  EXPECT_FALSE(validator->read_error());
  EXPECT_FALSE(validator->has_unavailable_data());

  validator->SetDownloadHints(nullptr);
}
