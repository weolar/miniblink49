// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/parser/cpdf_string.h"

#include <utility>
#include <vector>

#include "core/fpdfapi/edit/cpdf_encryptor.h"
#include "core/fpdfapi/parser/fpdf_parser_decode.h"
#include "core/fxcrt/fx_stream.h"
#include "third_party/base/ptr_util.h"

CPDF_String::CPDF_String() : m_bHex(false) {}

CPDF_String::CPDF_String(WeakPtr<ByteStringPool> pPool,
                         const ByteString& str,
                         bool bHex)
    : m_String(str), m_bHex(bHex) {
  if (pPool)
    m_String = pPool->Intern(m_String);
}

CPDF_String::CPDF_String(WeakPtr<ByteStringPool> pPool, const WideString& str)
    : m_String(PDF_EncodeText(str)), m_bHex(false) {
  if (pPool)
    m_String = pPool->Intern(m_String);
}

CPDF_String::~CPDF_String() {}

CPDF_Object::Type CPDF_String::GetType() const {
  return kString;
}

std::unique_ptr<CPDF_Object> CPDF_String::Clone() const {
  auto pRet = pdfium::MakeUnique<CPDF_String>();
  pRet->m_String = m_String;
  pRet->m_bHex = m_bHex;
  return std::move(pRet);
}

ByteString CPDF_String::GetString() const {
  return m_String;
}

void CPDF_String::SetString(const ByteString& str) {
  m_String = str;
}

bool CPDF_String::IsString() const {
  return true;
}

CPDF_String* CPDF_String::AsString() {
  return this;
}

const CPDF_String* CPDF_String::AsString() const {
  return this;
}

WideString CPDF_String::GetUnicodeText() const {
  return PDF_DecodeText(m_String.AsRawSpan());
}

bool CPDF_String::WriteTo(IFX_ArchiveStream* archive,
                          const CPDF_Encryptor* encryptor) const {
  std::vector<uint8_t> encrypted_data;
  pdfium::span<const uint8_t> data = m_String.AsRawSpan();
  if (encryptor) {
    encrypted_data = encryptor->Encrypt(data);
    data = encrypted_data;
  }
  const ByteString content =
      PDF_EncodeString(ByteString(data.data(), data.size()), IsHex());
  return archive->WriteString(content.AsStringView());
}
