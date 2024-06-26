// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/parser/cpdf_number.h"

#include "core/fxcrt/fx_stream.h"
#include "third_party/base/ptr_util.h"

CPDF_Number::CPDF_Number() {}

CPDF_Number::CPDF_Number(int value) : m_Number(value) {}

CPDF_Number::CPDF_Number(float value) : m_Number(value) {}

CPDF_Number::CPDF_Number(ByteStringView str) : m_Number(str) {}

CPDF_Number::~CPDF_Number() {}

CPDF_Object::Type CPDF_Number::GetType() const {
  return kNumber;
}

std::unique_ptr<CPDF_Object> CPDF_Number::Clone() const {
  return m_Number.IsInteger()
             ? pdfium::MakeUnique<CPDF_Number>(m_Number.GetSigned())
             : pdfium::MakeUnique<CPDF_Number>(m_Number.GetFloat());
}

float CPDF_Number::GetNumber() const {
  return m_Number.GetFloat();
}

int CPDF_Number::GetInteger() const {
  return m_Number.GetSigned();
}

bool CPDF_Number::IsNumber() const {
  return true;
}

CPDF_Number* CPDF_Number::AsNumber() {
  return this;
}

const CPDF_Number* CPDF_Number::AsNumber() const {
  return this;
}

void CPDF_Number::SetString(const ByteString& str) {
  m_Number = FX_Number(str.AsStringView());
}

ByteString CPDF_Number::GetString() const {
  return m_Number.IsInteger() ? ByteString::FormatInteger(m_Number.GetSigned())
                              : ByteString::FormatFloat(m_Number.GetFloat());
}

bool CPDF_Number::WriteTo(IFX_ArchiveStream* archive,
                          const CPDF_Encryptor* encryptor) const {
  return archive->WriteString(" ") &&
         archive->WriteString(GetString().AsStringView());
}
