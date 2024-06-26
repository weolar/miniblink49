// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfdoc/cpdf_link.h"

#include "core/fpdfapi/parser/cpdf_array.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfdoc/cpdf_nametree.h"

CPDF_Link::CPDF_Link() = default;

CPDF_Link::CPDF_Link(CPDF_Dictionary* pDict) : m_pDict(pDict) {}

CPDF_Link::CPDF_Link(const CPDF_Link& that) = default;

CPDF_Link::~CPDF_Link() = default;

CFX_FloatRect CPDF_Link::GetRect() {
  return m_pDict->GetRectFor("Rect");
}

CPDF_Dest CPDF_Link::GetDest(CPDF_Document* pDoc) {
  CPDF_Object* pDest = m_pDict->GetDirectObjectFor("Dest");
  if (!pDest)
    return CPDF_Dest();

  if (pDest->IsString() || pDest->IsName()) {
    CPDF_NameTree name_tree(pDoc, "Dests");
    return CPDF_Dest(name_tree.LookupNamedDest(pDoc, pDest->GetUnicodeText()));
  }
  if (CPDF_Array* pArray = pDest->AsArray())
    return CPDF_Dest(pArray);
  return CPDF_Dest();
}

CPDF_Action CPDF_Link::GetAction() {
  return CPDF_Action(m_pDict->GetDictFor("A"));
}
