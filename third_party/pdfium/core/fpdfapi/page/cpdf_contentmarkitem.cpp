// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/page/cpdf_contentmarkitem.h"

#include <utility>

#include "core/fpdfapi/parser/cpdf_dictionary.h"

CPDF_ContentMarkItem::CPDF_ContentMarkItem(ByteString name)
    : m_MarkName(std::move(name)) {}

CPDF_ContentMarkItem::~CPDF_ContentMarkItem() {}

const CPDF_Dictionary* CPDF_ContentMarkItem::GetParam() const {
  switch (m_ParamType) {
    case kPropertiesDict:
      return m_pPropertiesHolder->GetDictFor(m_PropertyName);
    case kDirectDict:
      return m_pDirectDict.get();
    case kNone:
    default:
      return nullptr;
  }
}

CPDF_Dictionary* CPDF_ContentMarkItem::GetParam() {
  return const_cast<CPDF_Dictionary*>(
      static_cast<const CPDF_ContentMarkItem*>(this)->GetParam());
}

bool CPDF_ContentMarkItem::HasMCID() const {
  const CPDF_Dictionary* pDict = GetParam();
  return pDict && pDict->KeyExist("MCID");
}

void CPDF_ContentMarkItem::SetDirectDict(
    std::unique_ptr<CPDF_Dictionary> pDict) {
  m_ParamType = kDirectDict;
  m_pDirectDict = std::move(pDict);
}

void CPDF_ContentMarkItem::SetPropertiesHolder(
    CPDF_Dictionary* pHolder,
    const ByteString& property_name) {
  m_ParamType = kPropertiesDict;
  m_pPropertiesHolder = pHolder;
  m_PropertyName = property_name;
}
