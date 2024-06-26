// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <memory>
#include <utility>
#include <vector>

#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_name.h"
#include "core/fpdfapi/parser/cpdf_number.h"
#include "core/fpdfapi/parser/cpdf_stream.h"
#include "core/fpdfapi/parser/cpdf_string.h"
#include "core/fpdfdoc/cpdf_filespec.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "testing/test_support.h"
#include "third_party/base/ptr_util.h"

TEST(cpdf_filespec, EncodeDecodeFileName) {
  static const std::vector<pdfium::NullTermWstrFuncTestData> test_data = {
    // Empty src string.
    {L"", L""},
    // only file name.
    {L"test.pdf", L"test.pdf"},
#if _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_
    // With drive identifier.
    {L"r:\\pdfdocs\\spec.pdf", L"/r/pdfdocs/spec.pdf"},
    // Relative path.
    {L"My Document\\test.pdf", L"My Document/test.pdf"},
    // Absolute path without drive identifier.
    {L"\\pdfdocs\\spec.pdf", L"//pdfdocs/spec.pdf"},
    // Absolute path with double backslashes.
    {L"\\\\pdfdocs\\spec.pdf", L"/pdfdocs/spec.pdf"},
// Network resource name. It is not supported yet.
// {L"pclib/eng:\\pdfdocs\\spec.pdf", L"/pclib/eng/pdfdocs/spec.pdf"},
#elif _FX_PLATFORM_ == _FX_PLATFORM_APPLE_
    // Absolute path with colon separator.
    {L"Mac HD:PDFDocs:spec.pdf", L"/Mac HD/PDFDocs/spec.pdf"},
    // Relative path with colon separator.
    {L"PDFDocs:spec.pdf", L"PDFDocs/spec.pdf"},
#else
    // Relative path.
    {L"./docs/test.pdf", L"./docs/test.pdf"},
    // Relative path with parent dir.
    {L"../test_docs/test.pdf", L"../test_docs/test.pdf"},
    // Absolute path.
    {L"/usr/local/home/test.pdf", L"/usr/local/home/test.pdf"},
#endif
  };
  for (const auto& data : test_data) {
    EXPECT_STREQ(data.expected,
                 CPDF_FileSpec::EncodeFileName(data.input).c_str());
    // DecodeFileName is the reverse procedure of EncodeFileName.
    EXPECT_STREQ(data.input,
                 CPDF_FileSpec::DecodeFileName(data.expected).c_str());
  }
}

TEST(cpdf_filespec, GetFileName) {
  {
    // String object.
    static const pdfium::NullTermWstrFuncTestData test_data = {
#if _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_
      L"/C/docs/test.pdf",
      L"C:\\docs\\test.pdf"
#elif _FX_PLATFORM_ == _FX_PLATFORM_APPLE_
      L"/Mac HD/docs/test.pdf",
      L"Mac HD:docs:test.pdf"
#else
      L"/docs/test.pdf",
      L"/docs/test.pdf"
#endif
    };
    auto str_obj = pdfium::MakeUnique<CPDF_String>(nullptr, test_data.input);
    CPDF_FileSpec file_spec(str_obj.get());
    EXPECT_STREQ(test_data.expected, file_spec.GetFileName().c_str());
  }
  {
    // Dictionary object.
    static const pdfium::NullTermWstrFuncTestData test_data[] = {
#if _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_
      {L"/C/docs/test.pdf", L"C:\\docs\\test.pdf"},
      {L"/D/docs/test.pdf", L"D:\\docs\\test.pdf"},
      {L"/E/docs/test.pdf", L"E:\\docs\\test.pdf"},
      {L"/F/docs/test.pdf", L"F:\\docs\\test.pdf"},
      {L"/G/docs/test.pdf", L"G:\\docs\\test.pdf"},
#elif _FX_PLATFORM_ == _FX_PLATFORM_APPLE_
      {L"/Mac HD/docs1/test.pdf", L"Mac HD:docs1:test.pdf"},
      {L"/Mac HD/docs2/test.pdf", L"Mac HD:docs2:test.pdf"},
      {L"/Mac HD/docs3/test.pdf", L"Mac HD:docs3:test.pdf"},
      {L"/Mac HD/docs4/test.pdf", L"Mac HD:docs4:test.pdf"},
      {L"/Mac HD/docs5/test.pdf", L"Mac HD:docs5:test.pdf"},
#else
      {L"/docs/a/test.pdf", L"/docs/a/test.pdf"},
      {L"/docs/b/test.pdf", L"/docs/b/test.pdf"},
      {L"/docs/c/test.pdf", L"/docs/c/test.pdf"},
      {L"/docs/d/test.pdf", L"/docs/d/test.pdf"},
      {L"/docs/e/test.pdf", L"/docs/e/test.pdf"},
#endif
    };
    // Keyword fields in reverse order of precedence to retrieve the file name.
    const char* const keywords[] = {"Unix", "Mac", "DOS", "F", "UF"};
    static_assert(FX_ArraySize(test_data) == FX_ArraySize(keywords),
                  "size mismatch");
    auto dict_obj = pdfium::MakeUnique<CPDF_Dictionary>();
    CPDF_FileSpec file_spec(dict_obj.get());
    EXPECT_TRUE(file_spec.GetFileName().IsEmpty());
    for (size_t i = 0; i < FX_ArraySize(keywords); ++i) {
      dict_obj->SetNewFor<CPDF_String>(keywords[i], test_data[i].input);
      EXPECT_STREQ(test_data[i].expected, file_spec.GetFileName().c_str());
    }

    // With all the former fields and 'FS' field suggests 'URL' type.
    dict_obj->SetNewFor<CPDF_String>("FS", "URL", false);
    // Url string is not decoded.
    EXPECT_STREQ(test_data[4].input, file_spec.GetFileName().c_str());
  }
  {
    // Invalid object.
    auto name_obj = pdfium::MakeUnique<CPDF_Name>(nullptr, "test.pdf");
    CPDF_FileSpec file_spec(name_obj.get());
    EXPECT_TRUE(file_spec.GetFileName().IsEmpty());
  }
}

TEST(cpdf_filespec, SetFileName) {
  static const pdfium::NullTermWstrFuncTestData test_data = {
#if _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_
    L"C:\\docs\\test.pdf",
    L"/C/docs/test.pdf"
#elif _FX_PLATFORM_ == _FX_PLATFORM_APPLE_
    L"Mac HD:docs:test.pdf",
    L"/Mac HD/docs/test.pdf"
#else
    L"/docs/test.pdf",
    L"/docs/test.pdf"
#endif
  };
  // String object.
  auto str_obj = pdfium::MakeUnique<CPDF_String>(nullptr, L"babababa");
  CPDF_FileSpec file_spec1(str_obj.get());
  file_spec1.SetFileName(test_data.input);
  // Check internal object value.
  EXPECT_STREQ(test_data.expected, str_obj->GetUnicodeText().c_str());
  // Check we can get the file name back.
  EXPECT_STREQ(test_data.input, file_spec1.GetFileName().c_str());

  // Dictionary object.
  auto dict_obj = pdfium::MakeUnique<CPDF_Dictionary>();
  CPDF_FileSpec file_spec2(dict_obj.get());
  file_spec2.SetFileName(test_data.input);
  // Check internal object value.
  EXPECT_STREQ(test_data.expected, dict_obj->GetUnicodeTextFor("F").c_str());
  EXPECT_STREQ(test_data.expected, dict_obj->GetUnicodeTextFor("UF").c_str());
  // Check we can get the file name back.
  EXPECT_STREQ(test_data.input, file_spec2.GetFileName().c_str());
}

TEST(cpdf_filespec, GetFileStream) {
  {
    // Invalid object.
    auto name_obj = pdfium::MakeUnique<CPDF_Name>(nullptr, "test.pdf");
    CPDF_FileSpec file_spec(name_obj.get());
    EXPECT_FALSE(file_spec.GetFileStream());
  }
  {
    // Dictionary object missing its embedded files dictionary.
    auto dict_obj = pdfium::MakeUnique<CPDF_Dictionary>();
    CPDF_FileSpec file_spec(dict_obj.get());
    EXPECT_FALSE(file_spec.GetFileStream());
  }
  {
    // Dictionary object with an empty embedded files dictionary.
    auto dict_obj = pdfium::MakeUnique<CPDF_Dictionary>();
    dict_obj->SetNewFor<CPDF_Dictionary>("EF");
    CPDF_FileSpec file_spec(dict_obj.get());
    EXPECT_FALSE(file_spec.GetFileStream());
  }
  {
    // Dictionary object with a non-empty embedded files dictionary.
    auto dict_obj = pdfium::MakeUnique<CPDF_Dictionary>();
    dict_obj->SetNewFor<CPDF_Dictionary>("EF");
    CPDF_FileSpec file_spec(dict_obj.get());

    const wchar_t file_name[] = L"test.pdf";
    const char* const keys[] = {"Unix", "Mac", "DOS", "F", "UF"};
    const char* const streams[] = {"test1", "test2", "test3", "test4", "test5"};
    static_assert(FX_ArraySize(keys) == FX_ArraySize(streams), "size mismatch");
    CPDF_Dictionary* file_dict =
        file_spec.GetObj()->AsDictionary()->GetDictFor("EF");

    // Keys in reverse order of precedence to retrieve the file content stream.
    for (size_t i = 0; i < FX_ArraySize(keys); ++i) {
      // Set the file name.
      dict_obj->SetNewFor<CPDF_String>(keys[i], file_name);

      // Set the file stream.
      auto pDict = pdfium::MakeUnique<CPDF_Dictionary>();
      size_t buf_len = strlen(streams[i]) + 1;
      std::unique_ptr<uint8_t, FxFreeDeleter> buf(FX_Alloc(uint8_t, buf_len));
      memcpy(buf.get(), streams[i], buf_len);
      file_dict->SetNewFor<CPDF_Stream>(keys[i], std::move(buf), buf_len,
                                        std::move(pDict));

      // Check that the file content stream is as expected.
      EXPECT_STREQ(
          streams[i],
          file_spec.GetFileStream()->GetUnicodeText().ToUTF8().c_str());

      if (i == 2) {
        dict_obj->SetNewFor<CPDF_String>("FS", "URL", false);
        EXPECT_FALSE(file_spec.GetFileStream());
      }
    }
  }
}

TEST(cpdf_filespec, GetParamsDict) {
  {
    // Invalid object.
    auto name_obj = pdfium::MakeUnique<CPDF_Name>(nullptr, "test.pdf");
    CPDF_FileSpec file_spec(name_obj.get());
    EXPECT_FALSE(file_spec.GetParamsDict());
  }
  {
    // Dictionary object.
    auto dict_obj = pdfium::MakeUnique<CPDF_Dictionary>();
    dict_obj->SetNewFor<CPDF_Dictionary>("EF");
    dict_obj->SetNewFor<CPDF_String>("UF", L"test.pdf");
    CPDF_FileSpec file_spec(dict_obj.get());
    EXPECT_FALSE(file_spec.GetParamsDict());

    // Add a file stream to the embedded files dictionary.
    CPDF_Dictionary* file_dict =
        file_spec.GetObj()->AsDictionary()->GetDictFor("EF");
    auto pDict = pdfium::MakeUnique<CPDF_Dictionary>();
    std::unique_ptr<uint8_t, FxFreeDeleter> buf(FX_Alloc(uint8_t, 6));
    memcpy(buf.get(), "hello", 6);
    file_dict->SetNewFor<CPDF_Stream>("UF", std::move(buf), 6,
                                      std::move(pDict));

    // Add a params dictionary to the file stream.
    CPDF_Stream* stream = file_dict->GetStreamFor("UF");
    CPDF_Dictionary* stream_dict = stream->GetDict();
    stream_dict->SetNewFor<CPDF_Dictionary>("Params");
    EXPECT_TRUE(file_spec.GetParamsDict());

    // Add a parameter to the params dictionary.
    CPDF_Dictionary* params_dict = stream_dict->GetDictFor("Params");
    params_dict->SetNewFor<CPDF_Number>("Size", 6);
    EXPECT_EQ(6, file_spec.GetParamsDict()->GetIntegerFor("Size"));
  }
}
