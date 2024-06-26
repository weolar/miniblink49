// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/fpdfapi/parser/cpdf_hint_tables.h"

#include <memory>
#include <string>
#include <utility>

#include "core/fpdfapi/cpdf_modulemgr.h"
#include "core/fpdfapi/parser/cpdf_data_avail.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_linearized_header.h"
#include "core/fpdfapi/parser/cpdf_object.h"
#include "core/fpdfapi/parser/cpdf_read_validator.h"
#include "core/fpdfapi/parser/cpdf_stream.h"
#include "core/fpdfapi/parser/cpdf_syntax_parser.h"
#include "core/fxcrt/cfx_readonlymemorystream.h"
#include "core/fxcrt/fx_stream.h"
#include "testing/gmock/include/gmock/gmock.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "testing/utils/path_service.h"
#include "third_party/base/ptr_util.h"

namespace {

RetainPtr<CPDF_ReadValidator> MakeValidatorFromFile(
    const std::string& file_name) {
  std::string file_path;
  PathService::GetTestFilePath(file_name, &file_path);
  ASSERT(!file_path.empty());
  return pdfium::MakeRetain<CPDF_ReadValidator>(
      IFX_SeekableReadStream::CreateFromFilename(file_path.c_str()), nullptr);
}

std::unique_ptr<CPDF_DataAvail> MakeDataAvailFromFile(
    const std::string& file_name) {
  return pdfium::MakeUnique<CPDF_DataAvail>(
      nullptr, MakeValidatorFromFile(file_name), true);
}

class TestLinearizedHeader final : public CPDF_LinearizedHeader {
 public:
  TestLinearizedHeader(const CPDF_Dictionary* pDict,
                       FX_FILESIZE szLastXRefOffset)
      : CPDF_LinearizedHeader(pDict, szLastXRefOffset) {}

  static std::unique_ptr<CPDF_LinearizedHeader> MakeHeader(
      const std::string& inline_data) {
    CPDF_SyntaxParser parser(pdfium::MakeRetain<CFX_ReadOnlyMemoryStream>(
        pdfium::as_bytes(pdfium::make_span(inline_data))));
    std::unique_ptr<CPDF_Dictionary> dict =
        ToDictionary(parser.GetObjectBody(nullptr));
    ASSERT(dict);
    return pdfium::MakeUnique<TestLinearizedHeader>(dict.get(), 0);
  }
};

}  // namespace

class CPDF_HintTablesTest : public testing::Test {
 public:
  CPDF_HintTablesTest() {
    // Needs for encoding Hint table stream.
    CPDF_ModuleMgr::Get()->Init();
  }

  ~CPDF_HintTablesTest() override { CPDF_ModuleMgr::Destroy(); }
};

TEST_F(CPDF_HintTablesTest, Load) {
  auto data_avail = MakeDataAvailFromFile("feature_linearized_loading.pdf");
  ASSERT_EQ(CPDF_DataAvail::DocAvailStatus::DataAvailable,
            data_avail->IsDocAvail(nullptr));

  ASSERT_TRUE(data_avail->GetHintTables());

  const CPDF_HintTables* hint_tables = data_avail->GetHintTables();
  FX_FILESIZE page_start = 0;
  FX_FILESIZE page_length = 0;
  uint32_t page_obj_num = 0;

  ASSERT_TRUE(
      hint_tables->GetPagePos(0, &page_start, &page_length, &page_obj_num));
  EXPECT_EQ(777, page_start);
  EXPECT_EQ(4328, page_length);
  EXPECT_EQ(39u, page_obj_num);

  ASSERT_TRUE(
      hint_tables->GetPagePos(1, &page_start, &page_length, &page_obj_num));
  EXPECT_EQ(5105, page_start);
  EXPECT_EQ(767, page_length);
  EXPECT_EQ(1u, page_obj_num);

  ASSERT_FALSE(
      hint_tables->GetPagePos(2, &page_start, &page_length, &page_obj_num));
}

TEST_F(CPDF_HintTablesTest, PageAndGroupInfos) {
  auto data_avail = MakeDataAvailFromFile("feature_linearized_loading.pdf");
  ASSERT_EQ(CPDF_DataAvail::DocAvailStatus::DataAvailable,
            data_avail->IsDocAvail(nullptr));

  const CPDF_HintTables* hint_tables = data_avail->GetHintTables();
  ASSERT_TRUE(hint_tables);
  ASSERT_EQ(2u, hint_tables->PageInfos().size());

  EXPECT_EQ(5u, hint_tables->PageInfos()[0].objects_count());
  EXPECT_EQ(777, hint_tables->PageInfos()[0].page_offset());
  EXPECT_EQ(4328u, hint_tables->PageInfos()[0].page_length());
  EXPECT_EQ(39u, hint_tables->PageInfos()[0].start_obj_num());
  ASSERT_EQ(2u, hint_tables->PageInfos()[0].Identifiers().size());

  EXPECT_EQ(0u, hint_tables->PageInfos()[0].Identifiers()[0]);
  EXPECT_EQ(0u, hint_tables->PageInfos()[0].Identifiers()[1]);

  EXPECT_EQ(3u, hint_tables->PageInfos()[1].objects_count());
  EXPECT_EQ(5105, hint_tables->PageInfos()[1].page_offset());
  EXPECT_EQ(767u, hint_tables->PageInfos()[1].page_length());
  EXPECT_EQ(1u, hint_tables->PageInfos()[1].start_obj_num());
  ASSERT_EQ(3u, hint_tables->PageInfos()[1].Identifiers().size());

  EXPECT_EQ(2u, hint_tables->PageInfos()[1].Identifiers()[0]);
  EXPECT_EQ(5u, hint_tables->PageInfos()[1].Identifiers()[1]);
  EXPECT_EQ(3u, hint_tables->PageInfos()[1].Identifiers()[2]);

  // SharedGroupInfo
  ASSERT_EQ(6u, hint_tables->SharedGroupInfos().size());

  EXPECT_EQ(777, hint_tables->SharedGroupInfos()[0].m_szOffset);
  EXPECT_EQ(254u, hint_tables->SharedGroupInfos()[0].m_dwLength);
  EXPECT_EQ(39u, hint_tables->SharedGroupInfos()[0].m_dwStartObjNum);
  EXPECT_EQ(1u, hint_tables->SharedGroupInfos()[0].m_dwObjectsCount);

  EXPECT_EQ(1031, hint_tables->SharedGroupInfos()[1].m_szOffset);
  EXPECT_EQ(389u, hint_tables->SharedGroupInfos()[1].m_dwLength);
  EXPECT_EQ(40u, hint_tables->SharedGroupInfos()[1].m_dwStartObjNum);
  EXPECT_EQ(1u, hint_tables->SharedGroupInfos()[1].m_dwObjectsCount);

  EXPECT_EQ(1420, hint_tables->SharedGroupInfos()[2].m_szOffset);
  EXPECT_EQ(726u, hint_tables->SharedGroupInfos()[2].m_dwLength);
  EXPECT_EQ(41u, hint_tables->SharedGroupInfos()[2].m_dwStartObjNum);
  EXPECT_EQ(1u, hint_tables->SharedGroupInfos()[2].m_dwObjectsCount);

  EXPECT_EQ(2146, hint_tables->SharedGroupInfos()[3].m_szOffset);
  EXPECT_EQ(290u, hint_tables->SharedGroupInfos()[3].m_dwLength);
  EXPECT_EQ(42u, hint_tables->SharedGroupInfos()[3].m_dwStartObjNum);
  EXPECT_EQ(1u, hint_tables->SharedGroupInfos()[3].m_dwObjectsCount);

  EXPECT_EQ(2436, hint_tables->SharedGroupInfos()[4].m_szOffset);
  EXPECT_EQ(2669u, hint_tables->SharedGroupInfos()[4].m_dwLength);
  EXPECT_EQ(43u, hint_tables->SharedGroupInfos()[4].m_dwStartObjNum);
  EXPECT_EQ(1u, hint_tables->SharedGroupInfos()[4].m_dwObjectsCount);

  EXPECT_EQ(10939, hint_tables->SharedGroupInfos()[5].m_szOffset);
  EXPECT_EQ(544u, hint_tables->SharedGroupInfos()[5].m_dwLength);
  EXPECT_EQ(4u, hint_tables->SharedGroupInfos()[5].m_dwStartObjNum);
  EXPECT_EQ(1u, hint_tables->SharedGroupInfos()[5].m_dwObjectsCount);
}

TEST_F(CPDF_HintTablesTest, FirstPageOffset) {
  // Test that valid hint table is loaded, and have correct offset of first page
  // object.
  const auto linearized_header = TestLinearizedHeader::MakeHeader(
      "<< /Linearized 1 /L 19326762 /H [ 123730 3816 ] /O 5932 /E 639518 /N "
      "102 /T 19220281 >>");
  ASSERT_TRUE(linearized_header);
  // This hint table is extracted from linearized file, generated by qpdf tool.
  RetainPtr<CPDF_ReadValidator> validator =
      MakeValidatorFromFile("hint_table_102p.bin");
  CPDF_SyntaxParser parser(validator, 0);
  std::unique_ptr<CPDF_Stream> stream = ToStream(parser.GetObjectBody(nullptr));
  ASSERT_TRUE(stream);
  auto hint_tables = pdfium::MakeUnique<CPDF_HintTables>(
      validator.Get(), linearized_header.get());
  // Check that hint table will load.
  ASSERT_TRUE(hint_tables->LoadHintStream(stream.get()));
  // Check that hint table have correct first page offset.
  // 127546 is predefined real value from original file.
  EXPECT_EQ(127546, hint_tables->GetFirstPageObjOffset());
}
