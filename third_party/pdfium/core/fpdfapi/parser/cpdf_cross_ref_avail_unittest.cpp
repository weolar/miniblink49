// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/fpdfapi/parser/cpdf_cross_ref_avail.h"

#include <memory>
#include <string>

#include "core/fpdfapi/parser/cpdf_syntax_parser.h"
#include "core/fxcrt/cfx_readonlymemorystream.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/base/ptr_util.h"

namespace {

std::unique_ptr<CPDF_SyntaxParser> MakeParserForBuffer(
    pdfium::span<const uint8_t> buffer) {
  return pdfium::MakeUnique<CPDF_SyntaxParser>(
      pdfium::MakeRetain<CFX_ReadOnlyMemoryStream>(buffer));
}

}  // namespace

TEST(CPDF_CrossRefAvailTest, CheckCrossRefV4) {
  const unsigned char xref_table[] =
      "xref \n"
      "0 6 \n"
      "0000000003 65535 f \n"
      "0000000017 00000 n \n"
      "0000000081 00000 n \n"
      "0000000000 00007 f \n"
      "0000000331 00000 n \n"
      "0000000409 00000 n \n"
      "trailer\n"
      "<</Root 14 0 R/ID "
      "[<afbb0f593c2d2aea5b519cb61da1c17b><4f9bb2e7978401808f8f1f2a75c322c8>]"
      "/Info 15 0 R/Size 16>>";
  const FX_FILESIZE last_crossref_offset = 0;

  auto parser = MakeParserForBuffer(xref_table);
  auto cross_ref_avail = pdfium::MakeUnique<CPDF_CrossRefAvail>(
      parser.get(), last_crossref_offset);

  EXPECT_EQ(CPDF_DataAvail::DataAvailable, cross_ref_avail->CheckAvail());
}

TEST(CPDF_CrossRefAvailTest, CheckCrossRefStream) {
  const unsigned char xref_stream[] =
      "16 0 obj\n"
      "<</Filter /FlateDecode>>"
      " stream \n"
      "STREAM DATA STREAM DATA STREAM DATA\n"
      "endstream\n"
      "endobj\n";
  const FX_FILESIZE last_crossref_offset = 0;

  auto parser = MakeParserForBuffer(xref_stream);
  auto cross_ref_avail = pdfium::MakeUnique<CPDF_CrossRefAvail>(
      parser.get(), last_crossref_offset);

  EXPECT_EQ(CPDF_DataAvail::DataAvailable, cross_ref_avail->CheckAvail());
}

TEST(CPDF_CrossRefAvailTest, IncorrectStartOffset) {
  const unsigned char xref_stream[] =
      "16 0 obj\n"
      "<</Filter /FlateDecode>>"
      " stream \n"
      "STREAM DATA STREAM DATA STREAM DATA\n"
      "endstream\n"
      "endobj\n";

  const FX_FILESIZE last_crossref_offset = 70000;

  auto parser = MakeParserForBuffer(xref_stream);
  auto cross_ref_avail = pdfium::MakeUnique<CPDF_CrossRefAvail>(
      parser.get(), last_crossref_offset);

  EXPECT_EQ(CPDF_DataAvail::DataError, cross_ref_avail->CheckAvail());
}

TEST(CPDF_CrossRefAvailTest, IncorrectPrevOffset) {
  const unsigned char xref_stream[] =
      "16 0 obj\n"
      "<</Type /XRef /Prev 70000>>"
      " stream \n"
      "STREAM DATA STREAM DATA STREAM DATA\n"
      "endstream\n"
      "endobj\n";
  const FX_FILESIZE last_crossref_offset = 0;

  auto parser = MakeParserForBuffer(xref_stream);
  auto cross_ref_avail = pdfium::MakeUnique<CPDF_CrossRefAvail>(
      parser.get(), last_crossref_offset);
  EXPECT_EQ(CPDF_DataAvail::DataError, cross_ref_avail->CheckAvail());
}

TEST(CPDF_CrossRefAvailTest, IncorrectPrevStreamOffset) {
  const unsigned char xref_table[] =
      "xref \n"
      "0 6 \n"
      "0000000003 65535 f \n"
      "0000000017 00000 n \n"
      "0000000081 00000 n \n"
      "0000000000 00007 f \n"
      "0000000331 00000 n \n"
      "0000000409 00000 n \n"
      "trailer\n"
      "<</Root 14 0 R/ID "
      "[<afbb0f593c2d2aea5b519cb61da1c17b><4f9bb2e7978401808f8f1f2a75c322c8>]"
      "/Info 15 0 R/Size 16 /XRefStm 70000>>";
  const FX_FILESIZE last_crossref_offset = 0;

  auto parser = MakeParserForBuffer(xref_table);
  auto cross_ref_avail = pdfium::MakeUnique<CPDF_CrossRefAvail>(
      parser.get(), last_crossref_offset);
  EXPECT_EQ(CPDF_DataAvail::DataError, cross_ref_avail->CheckAvail());
}

TEST(CPDF_CrossRefAvailTest, IncorrectData) {
  const unsigned char incorrect_data[] =
      "fiajaoilf w9ifaoihwoiafhja wfijaofijoiaw fhj oiawhfoiah "
      "wfoihoiwfghouiafghwoigahfi";
  const FX_FILESIZE last_crossref_offset = 0;

  auto parser = MakeParserForBuffer(incorrect_data);
  auto cross_ref_avail = pdfium::MakeUnique<CPDF_CrossRefAvail>(
      parser.get(), last_crossref_offset);
  EXPECT_EQ(CPDF_DataAvail::DataError, cross_ref_avail->CheckAvail());
}

TEST(CPDF_CrossRefAvailTest, ThreeCrossRefV4) {
  char int_buffer[100];
  std::string table = "pdf blah blah blah\n";
  size_t cur_offset = table.size();
  table +=
      "xref \n"
      "0 6 \n"
      "0000000003 65535 f \n"
      "trailer\n"
      "<</Root 14 0 R/ID "
      "[<afbb0f593c2d2aea5b519cb61da1c17b><4f9bb2e7978401808f8f1f2a75c322c8>]"
      "/Info 15 0 R/Size 16>>\n";
  table += "Dummy Data jgwhughouiwbahng";
  size_t prev_offset = cur_offset;
  cur_offset = table.size();
  table += std::string(
               "xref \n"
               "0 6 \n"
               "0000000003 65535 f \n"
               "trailer\n"
               "<</Root 14 0 R/ID "
               "[<afbb0f593c2d2aea5b519cb61da1c17b><"
               "4f9bb2e7978401808f8f1f2a75c322c8>]"
               "/Info 15 0 R/Size 16"
               "/Prev ") +
           FXSYS_itoa(static_cast<int>(prev_offset), int_buffer, 10) + ">>\n";
  table += "More Dummy Data jgwhughouiwbahng";
  prev_offset = cur_offset;
  cur_offset = table.size();
  table += std::string(
               "xref \n"
               "0 6 \n"
               "0000000003 65535 f \n"
               "trailer\n"
               "<</Root 14 0 R/ID "
               "[<afbb0f593c2d2aea5b519cb61da1c17b><"
               "4f9bb2e7978401808f8f1f2a75c322c8>]"
               "/Info 15 0 R/Size 16"
               "/Prev ") +
           FXSYS_itoa(static_cast<int>(prev_offset), int_buffer, 10) + ">>\n";
  const FX_FILESIZE last_crossref_offset = static_cast<FX_FILESIZE>(cur_offset);

  auto parser = MakeParserForBuffer(pdfium::as_bytes(pdfium::make_span(table)));
  auto cross_ref_avail = pdfium::MakeUnique<CPDF_CrossRefAvail>(
      parser.get(), last_crossref_offset);
  EXPECT_EQ(CPDF_DataAvail::DataAvailable, cross_ref_avail->CheckAvail());
}

TEST(CPDF_CrossRefAvailTest, ThreeCrossRefV5) {
  char int_buffer[100];
  std::string table = "pdf blah blah blah\n";
  size_t cur_offset = table.size();
  table +=
      "16 0 obj\n"
      "<</Type /XRef>>"
      " stream \n"
      "STREAM DATA STREAM DATA STREAM DATA ahfcuabfkuabfu\n"
      "endstream\n"
      "endobj\n";
  table += "Dummy Data jgwhughouiwbahng";

  size_t prev_offset = cur_offset;
  cur_offset = table.size();
  table += std::string(
               "55 0 obj\n"
               "<</Type /XRef /Prev ") +
           FXSYS_itoa(static_cast<int>(prev_offset), int_buffer, 10) +
           ">>"
           " stream \n"
           "STREAM DATA STREAM DATA STREAM DATA\n"
           "endstream\n"
           "endobj\n";
  table += "More Dummy Data jgwhughouiwbahng";
  prev_offset = cur_offset;
  cur_offset = table.size();
  table += std::string(
               "88 0 obj\n"
               "<</Type /XRef /NNNN /Prev ") +
           FXSYS_itoa(static_cast<int>(prev_offset), int_buffer, 10) +
           ">>"
           " stream \n"
           "STREAM DATA STREAM DATA STREAM DATA favav\n"
           "endstream\n"
           "endobj\n";
  const FX_FILESIZE last_crossref_offset = static_cast<FX_FILESIZE>(cur_offset);

  auto parser = MakeParserForBuffer(pdfium::as_bytes(pdfium::make_span(table)));
  auto cross_ref_avail = pdfium::MakeUnique<CPDF_CrossRefAvail>(
      parser.get(), last_crossref_offset);
  EXPECT_EQ(CPDF_DataAvail::DataAvailable, cross_ref_avail->CheckAvail());
}

TEST(CPDF_CrossRefAvailTest, Mixed) {
  char int_buffer[100];
  std::string table = "pdf blah blah blah\n";

  const int first_v5_table_offset = static_cast<int>(table.size());
  table +=
      "16 0 obj\n"
      "<</Type /XRef>>"
      " stream \n"
      "STREAM DATA STREAM DATA STREAM DATA ahfcuabfkuabfu\n"
      "endstream\n"
      "endobj\n";
  table += "Dummy Data jgwhughouiwbahng";

  const int second_v4_table_offset = static_cast<int>(table.size());
  table += std::string(
               "xref \n"
               "0 6 \n"
               "0000000003 65535 f \n"
               "trailer\n"
               "<</Root 14 0 R/ID "
               "[<afbb0f593c2d2aea5b519cb61da1c17b><"
               "4f9bb2e7978401808f8f1f2a75c322c8>]"
               "/Info 15 0 R/Size 16"
               "/Prev ") +
           FXSYS_itoa(first_v5_table_offset, int_buffer, 10) + ">>\n";
  table += "More Dummy Data jgwhughouiwbahng";

  const int last_v4_table_offset = static_cast<int>(table.size());
  table += std::string(
               "xref \n"
               "0 6 \n"
               "0000000003 65535 f \n"
               "trailer\n"
               "<</Root 14 0 R/ID "
               "[<afbb0f593c2d2aea5b519cb61da1c17b><"
               "4f9bb2e7978401808f8f1f2a75c322c8>]"
               "/Info 15 0 R/Size 16"
               "/Prev ") +
           FXSYS_itoa(second_v4_table_offset, int_buffer, 10) + " /XRefStm " +
           FXSYS_itoa(first_v5_table_offset, int_buffer, 10) + ">>\n";
  const FX_FILESIZE last_crossref_offset = last_v4_table_offset;

  auto parser = MakeParserForBuffer(pdfium::as_bytes(pdfium::make_span(table)));
  auto cross_ref_avail = pdfium::MakeUnique<CPDF_CrossRefAvail>(
      parser.get(), last_crossref_offset);
  EXPECT_EQ(CPDF_DataAvail::DataAvailable, cross_ref_avail->CheckAvail());
}

TEST(CPDF_CrossRefAvailTest, CrossRefV5IsNotStream) {
  const unsigned char invalid_xref_stream[] =
      "16 0 obj\n"
      "[/array /object]\n"
      "endstream\n"
      "endobj\n";
  const FX_FILESIZE last_crossref_offset = 0;

  auto parser = MakeParserForBuffer(invalid_xref_stream);
  auto cross_ref_avail = pdfium::MakeUnique<CPDF_CrossRefAvail>(
      parser.get(), last_crossref_offset);
  EXPECT_EQ(CPDF_DataAvail::DataError, cross_ref_avail->CheckAvail());
}

TEST(CPDF_CrossRefAvailTest, CrossRefV4WithEncryptRef) {
  const unsigned char xref_table[] =
      "xref \n"
      "0 6 \n"
      "0000000003 65535 f \n"
      "0000000017 00000 n \n"
      "0000000081 00000 n \n"
      "0000000000 00007 f \n"
      "0000000331 00000 n \n"
      "0000000409 00000 n \n"
      "trailer\n"
      "<</Root 14 0 R/ID "
      "[<afbb0f593c2d2aea5b519cb61da1c17b><4f9bb2e7978401808f8f1f2a75c322c8>]"
      "/Encrypt 77 0 R"
      "/Info 15 0 R/Size 16>>";
  const FX_FILESIZE last_crossref_offset = 0;

  auto parser = MakeParserForBuffer(xref_table);
  auto cross_ref_avail = pdfium::MakeUnique<CPDF_CrossRefAvail>(
      parser.get(), last_crossref_offset);
  EXPECT_EQ(CPDF_DataAvail::DataError, cross_ref_avail->CheckAvail());
}

TEST(CPDF_CrossRefAvailTest, CrossRefStreamWithEncryptRef) {
  const unsigned char xref_stream[] =
      "16 0 obj\n"
      "<</Filter /FlateDecode /Encrypt 77 0 R>>"
      " stream \n"
      "STREAM DATA STREAM DATA STREAM DATA\n"
      "endstream\n"
      "endobj\n";
  const FX_FILESIZE last_crossref_offset = 0;

  auto parser = MakeParserForBuffer(xref_stream);
  auto cross_ref_avail = pdfium::MakeUnique<CPDF_CrossRefAvail>(
      parser.get(), last_crossref_offset);
  EXPECT_EQ(CPDF_DataAvail::DataError, cross_ref_avail->CheckAvail());
}
