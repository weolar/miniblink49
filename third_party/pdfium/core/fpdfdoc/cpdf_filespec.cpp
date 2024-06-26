// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfdoc/cpdf_filespec.h"

#include <vector>

#include "constants/stream_dict_common.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_name.h"
#include "core/fpdfapi/parser/cpdf_object.h"
#include "core/fpdfapi/parser/cpdf_stream.h"
#include "core/fpdfapi/parser/cpdf_string.h"
#include "core/fpdfapi/parser/fpdf_parser_decode.h"
#include "core/fxcrt/fx_system.h"

namespace {

#if _FX_PLATFORM_ == _FX_PLATFORM_APPLE_ || \
    _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_
WideString ChangeSlashToPlatform(const wchar_t* str) {
  WideString result;
  while (*str) {
    if (*str == '/') {
#if _FX_PLATFORM_ == _FX_PLATFORM_APPLE_
      result += L':';
#else
      result += L'\\';
#endif
    } else {
      result += *str;
    }
    str++;
  }
  return result;
}

WideString ChangeSlashToPDF(const wchar_t* str) {
  WideString result;
  while (*str) {
    if (*str == '\\' || *str == ':')
      result += L'/';
    else
      result += *str;

    str++;
  }
  return result;
}
#endif  // _FX_PLATFORM_APPLE_ || _FX_PLATFORM_WINDOWS_

}  // namespace

CPDF_FileSpec::CPDF_FileSpec(const CPDF_Object* pObj) : m_pObj(pObj) {
  ASSERT(m_pObj);
}

CPDF_FileSpec::CPDF_FileSpec(CPDF_Object* pObj)
    : m_pObj(pObj), m_pWritableObj(pObj) {
  ASSERT(m_pObj);
}

CPDF_FileSpec::~CPDF_FileSpec() {}

WideString CPDF_FileSpec::DecodeFileName(const WideString& filepath) {
  if (filepath.GetLength() <= 1)
    return WideString();

#if _FX_PLATFORM_ == _FX_PLATFORM_APPLE_
  if (filepath.Left(sizeof("/Mac") - 1) == WideStringView(L"/Mac"))
    return ChangeSlashToPlatform(filepath.c_str() + 1);
  return ChangeSlashToPlatform(filepath.c_str());
#elif _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_

  if (filepath[0] != L'/')
    return ChangeSlashToPlatform(filepath.c_str());
  if (filepath[1] == L'/')
    return ChangeSlashToPlatform(filepath.c_str() + 1);
  if (filepath[2] == L'/') {
    WideString result;
    result += filepath[1];
    result += L':';
    result += ChangeSlashToPlatform(filepath.c_str() + 2);
    return result;
  }
  WideString result;
  result += L'\\';
  result += ChangeSlashToPlatform(filepath.c_str());
  return result;
#else
  return WideString(filepath);
#endif
}

WideString CPDF_FileSpec::GetFileName() const {
  WideString csFileName;
  if (const CPDF_Dictionary* pDict = m_pObj->AsDictionary()) {
    csFileName = pDict->GetUnicodeTextFor("UF");
    if (csFileName.IsEmpty()) {
      csFileName = WideString::FromDefANSI(
          pDict->GetStringFor(pdfium::stream::kF).AsStringView());
    }
    if (pDict->GetStringFor("FS") == "URL")
      return csFileName;

    if (csFileName.IsEmpty()) {
      constexpr const char* keys[] = {"DOS", "Mac", "Unix"};
      for (const auto* key : keys) {
        if (pDict->KeyExist(key)) {
          csFileName =
              WideString::FromDefANSI(pDict->GetStringFor(key).AsStringView());
          break;
        }
      }
    }
  } else if (m_pObj->IsString()) {
    csFileName = WideString::FromDefANSI(m_pObj->GetString().AsStringView());
  }
  return DecodeFileName(csFileName);
}

const CPDF_Stream* CPDF_FileSpec::GetFileStream() const {
  const CPDF_Dictionary* pDict = m_pObj->AsDictionary();
  if (!pDict)
    return nullptr;

  // Get the embedded files dictionary.
  const CPDF_Dictionary* pFiles = pDict->GetDictFor("EF");
  if (!pFiles)
    return nullptr;

  // List of keys to check for the file specification string.
  // Follows the same precedence order as GetFileName().
  constexpr const char* kKeys[] = {"UF", "F", "DOS", "Mac", "Unix"};
  size_t end = pDict->GetStringFor("FS") == "URL" ? 2 : FX_ArraySize(kKeys);
  for (size_t i = 0; i < end; ++i) {
    ByteString key = kKeys[i];
    if (!pDict->GetUnicodeTextFor(key).IsEmpty()) {
      const CPDF_Stream* pStream = pFiles->GetStreamFor(key);
      if (pStream)
        return pStream;
    }
  }
  return nullptr;
}

CPDF_Stream* CPDF_FileSpec::GetFileStream() {
  return const_cast<CPDF_Stream*>(
      static_cast<const CPDF_FileSpec*>(this)->GetFileStream());
}

const CPDF_Dictionary* CPDF_FileSpec::GetParamsDict() const {
  const CPDF_Stream* pStream = GetFileStream();
  if (!pStream)
    return nullptr;

  const CPDF_Dictionary* pDict = pStream->GetDict();
  return pDict ? pDict->GetDictFor("Params") : nullptr;
}

CPDF_Dictionary* CPDF_FileSpec::GetParamsDict() {
  return const_cast<CPDF_Dictionary*>(
      static_cast<const CPDF_FileSpec*>(this)->GetParamsDict());
}

WideString CPDF_FileSpec::EncodeFileName(const WideString& filepath) {
  if (filepath.GetLength() <= 1)
    return WideString();

#if _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_
  if (filepath[1] == L':') {
    WideString result(L'/');
    result += filepath[0];
    if (filepath[2] != L'\\')
      result += L'/';

    result += ChangeSlashToPDF(filepath.c_str() + 2);
    return result;
  }
  if (filepath[0] == L'\\' && filepath[1] == L'\\')
    return ChangeSlashToPDF(filepath.c_str() + 1);

  if (filepath[0] == L'\\')
    return L'/' + ChangeSlashToPDF(filepath.c_str());
  return ChangeSlashToPDF(filepath.c_str());
#elif _FX_PLATFORM_ == _FX_PLATFORM_APPLE_
  if (filepath.Left(sizeof("Mac") - 1).EqualsASCII("Mac"))
    return L'/' + ChangeSlashToPDF(filepath.c_str());
  return ChangeSlashToPDF(filepath.c_str());
#else
  return WideString(filepath);
#endif
}

void CPDF_FileSpec::SetFileName(const WideString& wsFileName) {
  if (!m_pWritableObj) {
    NOTREACHED();
    return;
  }

  WideString wsStr = EncodeFileName(wsFileName);
  if (m_pObj->IsString()) {
    m_pWritableObj->SetString(wsStr.ToDefANSI());
  } else if (CPDF_Dictionary* pDict = m_pWritableObj->AsDictionary()) {
    pDict->SetNewFor<CPDF_String>(pdfium::stream::kF, wsStr.ToDefANSI(), false);
    pDict->SetNewFor<CPDF_String>("UF", wsStr);
  }
}
