// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/parser/cpdf_name.h"

#include "core/fpdfapi/parser/fpdf_parser_decode.h"
#include "core/fpdfapi/parser/fpdf_parser_utility.h"
#include "core/fxcrt/fx_stream.h"
#include "third_party/base/ptr_util.h"

CPDF_Name::CPDF_Name(WeakPtr<ByteStringPool> pPool, const ByteString& str)
    : m_Name(str) {
  if (pPool)
    m_Name = pPool->Intern(m_Name);
}

CPDF_Name::~CPDF_Name() {}

CPDF_Object::Type CPDF_Name::GetType() const {
  return kName;
}

std::unique_ptr<CPDF_Object> CPDF_Name::Clone() const {
  return pdfium::MakeUnique<CPDF_Name>(nullptr, m_Name);
}

ByteString CPDF_Name::GetString() const {
  return m_Name;
}

void CPDF_Name::SetString(const ByteString& str) {
  m_Name = str;
}

bool CPDF_Name::IsName() const {
  return true;
}

CPDF_Name* CPDF_Name::AsName() {
  return this;
}

const CPDF_Name* CPDF_Name::AsName() const {
  return this;
}

WideString CPDF_Name::GetUnicodeText() const {
  return PDF_DecodeText(m_Name.AsRawSpan());
}

bool CPDF_Name::WriteTo(IFX_ArchiveStream* archive,
                        const CPDF_Encryptor* encryptor) const {
  return archive->WriteString("/") &&
         archive->WriteString(PDF_NameEncode(GetString()).AsStringView());
}
