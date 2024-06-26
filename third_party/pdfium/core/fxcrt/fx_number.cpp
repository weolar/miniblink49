// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcrt/fx_number.h"

#include <limits>

#include "core/fxcrt/fx_extension.h"
#include "core/fxcrt/fx_string.h"

FX_Number::FX_Number()
    : m_bInteger(true), m_bSigned(false), m_UnsignedValue(0) {}

FX_Number::FX_Number(uint32_t value)
    : m_bInteger(true), m_bSigned(false), m_UnsignedValue(value) {}

FX_Number::FX_Number(int32_t value)
    : m_bInteger(true), m_bSigned(true), m_SignedValue(value) {}

FX_Number::FX_Number(float value)
    : m_bInteger(false), m_bSigned(true), m_FloatValue(value) {}

FX_Number::FX_Number(ByteStringView strc)
    : m_bInteger(true), m_bSigned(false), m_UnsignedValue(0) {
  if (strc.IsEmpty())
    return;

  if (strc.Contains('.')) {
    m_bInteger = false;
    m_bSigned = true;
    m_FloatValue = StringToFloat(strc);
    return;
  }

  // Note, numbers in PDF are typically of the form 123, -123, etc. But,
  // for things like the Permissions on the encryption hash the number is
  // actually an unsigned value. We use a uint32_t so we can deal with the
  // unsigned and then check for overflow if the user actually signed the value.
  // The Permissions flag is listed in Table 3.20 PDF 1.7 spec.
  pdfium::base::CheckedNumeric<uint32_t> unsigned_val = 0;
  bool bNegative = false;
  size_t cc = 0;
  if (strc[0] == '+') {
    cc++;
    m_bSigned = true;
  } else if (strc[0] == '-') {
    bNegative = true;
    m_bSigned = true;
    cc++;
  }

  while (cc < strc.GetLength() && std::isdigit(strc[cc])) {
    unsigned_val = unsigned_val * 10 + FXSYS_DecimalCharToInt(strc.CharAt(cc));
    if (!unsigned_val.IsValid())
      break;
    cc++;
  }

  uint32_t uValue = unsigned_val.ValueOrDefault(0);
  if (!m_bSigned) {
    m_UnsignedValue = uValue;
    return;
  }

  // We have a sign, so if the value was greater then the signed integer
  // limits, then we've overflowed and must reset to the default value.
  constexpr uint32_t uLimit =
      static_cast<uint32_t>(std::numeric_limits<int>::max());

  if (uValue > (bNegative ? uLimit + 1 : uLimit))
    uValue = 0;

  // Switch back to the int space so we can flip to a negative if we need.
  int32_t value = static_cast<int32_t>(uValue);
  if (bNegative) {
    // |value| is usually positive, except in the corner case of "-2147483648",
    // where |uValue| is 2147483648. When it gets casted to an int, |value|
    // becomes -2147483648. For this case, avoid undefined behavior, because
    // an int32_t cannot represent 2147483648.
    static constexpr int kMinInt = std::numeric_limits<int>::min();
    m_SignedValue = LIKELY(value != kMinInt) ? -value : kMinInt;
  } else {
    m_SignedValue = value;
  }
}

uint32_t FX_Number::GetUnsigned() const {
  return m_bInteger ? m_UnsignedValue : static_cast<uint32_t>(m_FloatValue);
}

int32_t FX_Number::GetSigned() const {
  return m_bInteger ? m_SignedValue : static_cast<int32_t>(m_FloatValue);
}

float FX_Number::GetFloat() const {
  if (!m_bInteger)
    return m_FloatValue;

  return m_bSigned ? static_cast<float>(m_SignedValue)
                   : static_cast<float>(m_UnsignedValue);
}
