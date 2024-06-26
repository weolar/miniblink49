// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/fpdfdoc/cpdf_metadata.h"

#include "core/fpdfapi/parser/cpdf_stream.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "testing/test_support.h"

TEST(CPDF_MetadataTest, CheckSharedFormEmailAtTopLevel) {
  static const char data[] =
      "<?xml charset=\"utf-8\"?>\n"
      "<node xmlns:adhocwf=\"http://ns.adobe.com/AcrobatAdhocWorkflow/1.0/\">\n"
      "<adhocwf:workflowType>0</adhocwf:workflowType>\n"
      "<adhocwf:version>1.1</adhocwf:version>\n"
      "</node>";

  CPDF_Stream stream;
  stream.SetData(ByteStringView(data).span());
  CPDF_Metadata metadata(&stream);

  auto results = metadata.CheckForSharedForm();
  ASSERT_EQ(1U, results.size());
  EXPECT_EQ(UnsupportedFeature::kDocumentSharedFormEmail, results[0]);
}

TEST(CPDF_MetadataTest, CheckSharedFormAcrobatAtTopLevel) {
  static const char data[] =
      "<?xml charset=\"utf-8\"?>\n"
      "<node xmlns:adhocwf=\"http://ns.adobe.com/AcrobatAdhocWorkflow/1.0/\">\n"
      "<adhocwf:workflowType>1</adhocwf:workflowType>\n"
      "<adhocwf:version>1.1</adhocwf:version>\n"
      "</node>";

  CPDF_Stream stream;
  stream.SetData(ByteStringView(data).span());
  CPDF_Metadata metadata(&stream);

  auto results = metadata.CheckForSharedForm();
  ASSERT_EQ(1U, results.size());
  EXPECT_EQ(UnsupportedFeature::kDocumentSharedFormAcrobat, results[0]);
}

TEST(CPDF_MetadataTest, CheckSharedFormFilesystemAtTopLevel) {
  static const char data[] =
      "<?xml charset=\"utf-8\"?>\n"
      "<node xmlns:adhocwf=\"http://ns.adobe.com/AcrobatAdhocWorkflow/1.0/\">\n"
      "<adhocwf:workflowType>2</adhocwf:workflowType>\n"
      "<adhocwf:version>1.1</adhocwf:version>\n"
      "</node>";

  CPDF_Stream stream;
  stream.SetData(ByteStringView(data).span());
  CPDF_Metadata metadata(&stream);

  auto results = metadata.CheckForSharedForm();
  ASSERT_EQ(1U, results.size());
  EXPECT_EQ(UnsupportedFeature::kDocumentSharedFormFilesystem, results[0]);
}

TEST(CPDF_MetadataTest, CheckSharedFormWithoutWorkflow) {
  static const char data[] =
      "<?xml charset=\"utf-8\"?>\n"
      "<node xmlns:adhocwf=\"http://ns.adobe.com/AcrobatAdhocWorkflow/1.0/\">\n"
      "<adhocwf:state>2</adhocwf:state>\n"
      "<adhocwf:version>1.1</adhocwf:version>\n"
      "</node>";

  CPDF_Stream stream;
  stream.SetData(ByteStringView(data).span());
  CPDF_Metadata metadata(&stream);

  auto results = metadata.CheckForSharedForm();
  EXPECT_EQ(0U, results.size());
}

TEST(CPDF_MetadataTest, CheckSharedFormAsChild) {
  static const char data[] =
      "<?xml charset=\"utf-8\"?>\n"
      "<grandparent><parent>\n"
      "<node xmlns:adhocwf=\"http://ns.adobe.com/AcrobatAdhocWorkflow/1.0/\">\n"
      "<adhocwf:workflowType>0</adhocwf:workflowType>\n"
      "<adhocwf:version>1.1</adhocwf:version>\n"
      "</node>"
      "</parent></grandparent>";

  CPDF_Stream stream;
  stream.SetData(ByteStringView(data).span());
  CPDF_Metadata metadata(&stream);

  auto results = metadata.CheckForSharedForm();
  ASSERT_EQ(1U, results.size());
  EXPECT_EQ(UnsupportedFeature::kDocumentSharedFormEmail, results[0]);
}

TEST(CPDF_MetadataTest, CheckSharedFormAsNoAdhoc) {
  static const char data[] =
      "<?xml charset=\"utf-8\"?>\n"
      "<node></node>";

  CPDF_Stream stream;
  stream.SetData(ByteStringView(data).span());
  CPDF_Metadata metadata(&stream);

  auto results = metadata.CheckForSharedForm();
  EXPECT_EQ(0U, results.size());
}

TEST(CPDF_MetadataTest, CheckSharedFormWrongNamespace) {
  static const char data[] =
      "<?xml charset=\"utf-8\"?>\n"
      "<node xmlns:adhocwf=\"http://ns.adobe.com/AcrobatAdhocWorkflow/2.0/\">\n"
      "<adhocwf:workflowType>1</adhocwf:workflowType>\n"
      "<adhocwf:version>1.1</adhocwf:version>\n"
      "</node>";

  CPDF_Stream stream;
  stream.SetData(ByteStringView(data).span());
  CPDF_Metadata metadata(&stream);

  auto results = metadata.CheckForSharedForm();
  EXPECT_EQ(0U, results.size());
}

TEST(CPDF_MetadataTest, CheckSharedFormMultipleErrors) {
  static const char data[] =
      "<?xml charset=\"utf-8\"?>\n"
      "<grandparent>"
      "<parent>\n"
      "<node xmlns:adhocwf=\"http://ns.adobe.com/AcrobatAdhocWorkflow/1.0/\">\n"
      "<adhocwf:workflowType>0</adhocwf:workflowType>\n"
      "<adhocwf:version>1.1</adhocwf:version>\n"
      "</node>"
      "</parent>"
      "<node2 "
      "xmlns:adhocwf=\"http://ns.adobe.com/AcrobatAdhocWorkflow/1.0/\">\n"
      "<adhocwf:workflowType>2</adhocwf:workflowType>\n"
      "<adhocwf:version>1.1</adhocwf:version>\n"
      "</node2>"
      "<node3 "
      "xmlns:adhocwf=\"http://ns.adobe.com/AcrobatAdhocWorkflow/1.0/\">\n"
      "<adhocwf:workflowType>1</adhocwf:workflowType>\n"
      "<adhocwf:version>1.1</adhocwf:version>\n"
      "</node3>"
      "</grandparent>";

  CPDF_Stream stream;
  stream.SetData(ByteStringView(data).span());
  CPDF_Metadata metadata(&stream);

  auto results = metadata.CheckForSharedForm();
  ASSERT_EQ(3U, results.size());
  EXPECT_EQ(UnsupportedFeature::kDocumentSharedFormEmail, results[0]);
  EXPECT_EQ(UnsupportedFeature::kDocumentSharedFormFilesystem, results[1]);
  EXPECT_EQ(UnsupportedFeature::kDocumentSharedFormAcrobat, results[2]);
}
