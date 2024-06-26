// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcrt/css/cfx_cssselector.h"

#include <utility>

#include "core/fxcrt/fx_extension.h"
#include "third_party/base/ptr_util.h"

namespace {

int32_t GetCSSNameLen(const wchar_t* psz, const wchar_t* pEnd) {
  const wchar_t* pStart = psz;
  while (psz < pEnd) {
    if (!isascii(*psz) || (!isalnum(*psz) && *psz != '_' && *psz != '-')) {
      break;
    }
    ++psz;
  }
  return psz - pStart;
}

}  // namespace

CFX_CSSSelector::CFX_CSSSelector(CFX_CSSSelectorType eType,
                                 const wchar_t* psz,
                                 int32_t iLen,
                                 bool bIgnoreCase)
    : m_eType(eType),
      m_dwHash(FX_HashCode_GetW(WideStringView(psz, iLen), bIgnoreCase)) {}

CFX_CSSSelector::~CFX_CSSSelector() {}

CFX_CSSSelectorType CFX_CSSSelector::GetType() const {
  return m_eType;
}

uint32_t CFX_CSSSelector::GetNameHash() const {
  return m_dwHash;
}

CFX_CSSSelector* CFX_CSSSelector::GetNextSelector() const {
  return m_pNext.get();
}

// static.
std::unique_ptr<CFX_CSSSelector> CFX_CSSSelector::FromString(
    WideStringView str) {
  ASSERT(!str.IsEmpty());

  const wchar_t* psz = str.unterminated_c_str();
  const wchar_t* pStart = psz;
  const wchar_t* pEnd = psz + str.GetLength();
  for (; psz < pEnd; ++psz) {
    switch (*psz) {
      case '>':
      case '[':
      case '+':
        return nullptr;
    }
  }

  std::unique_ptr<CFX_CSSSelector> pFirst = nullptr;
  for (psz = pStart; psz < pEnd;) {
    wchar_t wch = *psz;
    if ((isascii(wch) && isalpha(wch)) || wch == '*') {
      int32_t iNameLen = wch == '*' ? 1 : GetCSSNameLen(psz, pEnd);
      auto p = pdfium::MakeUnique<CFX_CSSSelector>(CFX_CSSSelectorType::Element,
                                                   psz, iNameLen, true);
      if (pFirst) {
        pFirst->SetType(CFX_CSSSelectorType::Descendant);
        p->SetNext(std::move(pFirst));
      }
      pFirst = std::move(p);
      psz += iNameLen;
    } else if (wch == ' ') {
      psz++;
    } else {
      return nullptr;
    }
  }
  return pFirst;
}
