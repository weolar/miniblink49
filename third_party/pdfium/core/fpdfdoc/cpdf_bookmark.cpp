// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfdoc/cpdf_bookmark.h"

#include <memory>
#include <vector>

#include "core/fpdfapi/parser/cpdf_array.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_string.h"
#include "core/fpdfdoc/cpdf_nametree.h"
#include "core/fxge/fx_dib.h"

CPDF_Bookmark::CPDF_Bookmark() = default;

CPDF_Bookmark::CPDF_Bookmark(const CPDF_Bookmark& that) = default;

CPDF_Bookmark::CPDF_Bookmark(const CPDF_Dictionary* pDict) : m_pDict(pDict) {}

CPDF_Bookmark::~CPDF_Bookmark() = default;

uint32_t CPDF_Bookmark::GetFontStyle() const {
  return m_pDict ? m_pDict->GetIntegerFor("F") : 0;
}

WideString CPDF_Bookmark::GetTitle() const {
  if (!m_pDict)
    return WideString();

  const CPDF_String* pString = ToString(m_pDict->GetDirectObjectFor("Title"));
  if (!pString)
    return WideString();

  WideString title = pString->GetUnicodeText();
  int len = title.GetLength();
  if (!len)
    return WideString();

  std::vector<wchar_t> buf(len);
  for (int i = 0; i < len; i++) {
    wchar_t w = title[i];
    buf[i] = w > 0x20 ? w : 0x20;
  }
  return WideString(buf.data(), len);
}

CPDF_Dest CPDF_Bookmark::GetDest(CPDF_Document* pDocument) const {
  if (!m_pDict)
    return CPDF_Dest();

  const CPDF_Object* pDest = m_pDict->GetDirectObjectFor("Dest");
  if (!pDest)
    return CPDF_Dest();
  if (pDest->IsString() || pDest->IsName()) {
    CPDF_NameTree name_tree(pDocument, "Dests");
    return CPDF_Dest(
        name_tree.LookupNamedDest(pDocument, pDest->GetUnicodeText()));
  }
  if (const CPDF_Array* pArray = pDest->AsArray())
    return CPDF_Dest(pArray);
  return CPDF_Dest();
}

CPDF_Action CPDF_Bookmark::GetAction() const {
  return CPDF_Action(m_pDict ? m_pDict->GetDictFor("A") : nullptr);
}
