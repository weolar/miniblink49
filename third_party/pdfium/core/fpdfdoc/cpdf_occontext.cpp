// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfdoc/cpdf_occontext.h"

#include "core/fpdfapi/page/cpdf_pageobject.h"
#include "core/fpdfapi/parser/cpdf_array.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_document.h"

namespace {

int32_t FindGroup(const CPDF_Array* pArray, const CPDF_Dictionary* pGroupDict) {
  if (!pArray || !pGroupDict)
    return -1;

  for (size_t i = 0; i < pArray->size(); i++) {
    if (pArray->GetDictAt(i) == pGroupDict)
      return i;
  }
  return -1;
}

bool HasIntent(const CPDF_Dictionary* pDict,
               ByteStringView csElement,
               ByteStringView csDef) {
  const CPDF_Object* pIntent = pDict->GetDirectObjectFor("Intent");
  if (!pIntent)
    return csElement == csDef;

  ByteString bsIntent;
  if (const CPDF_Array* pArray = pIntent->AsArray()) {
    for (size_t i = 0; i < pArray->size(); i++) {
      bsIntent = pArray->GetStringAt(i);
      if (bsIntent == "All" || bsIntent == csElement)
        return true;
    }
    return false;
  }
  bsIntent = pIntent->GetString();
  return bsIntent == "All" || bsIntent == csElement;
}

CPDF_Dictionary* GetConfig(CPDF_Document* pDoc,
                           const CPDF_Dictionary* pOCGDict) {
  ASSERT(pOCGDict);
  CPDF_Dictionary* pOCProperties = pDoc->GetRoot()->GetDictFor("OCProperties");
  if (!pOCProperties)
    return nullptr;

  CPDF_Array* pOCGs = pOCProperties->GetArrayFor("OCGs");
  if (!pOCGs)
    return nullptr;

  if (FindGroup(pOCGs, pOCGDict) < 0)
    return nullptr;

  CPDF_Dictionary* pConfig = pOCProperties->GetDictFor("D");
  CPDF_Array* pConfigs = pOCProperties->GetArrayFor("Configs");
  if (!pConfigs)
    return pConfig;

  for (size_t i = 0; i < pConfigs->size(); i++) {
    CPDF_Dictionary* pFind = pConfigs->GetDictAt(i);
    if (pFind && HasIntent(pFind, "View", "View"))
      return pFind;
  }
  return pConfig;
}

ByteString GetUsageTypeString(CPDF_OCContext::UsageType eType) {
  ByteString csState;
  switch (eType) {
    case CPDF_OCContext::Design:
      csState = "Design";
      break;
    case CPDF_OCContext::Print:
      csState = "Print";
      break;
    case CPDF_OCContext::Export:
      csState = "Export";
      break;
    default:
      csState = "View";
      break;
  }
  return csState;
}

}  // namespace

CPDF_OCContext::CPDF_OCContext(CPDF_Document* pDoc, UsageType eUsageType)
    : m_pDocument(pDoc), m_eUsageType(eUsageType) {
  ASSERT(pDoc);
}

CPDF_OCContext::~CPDF_OCContext() {}

bool CPDF_OCContext::LoadOCGStateFromConfig(
    const ByteString& csConfig,
    const CPDF_Dictionary* pOCGDict) const {
  CPDF_Dictionary* pConfig = GetConfig(m_pDocument.Get(), pOCGDict);
  if (!pConfig)
    return true;

  bool bState = pConfig->GetStringFor("BaseState", "ON") != "OFF";
  CPDF_Array* pArray = pConfig->GetArrayFor("ON");
  if (pArray) {
    if (FindGroup(pArray, pOCGDict) >= 0)
      bState = true;
  }
  pArray = pConfig->GetArrayFor("OFF");
  if (pArray) {
    if (FindGroup(pArray, pOCGDict) >= 0)
      bState = false;
  }
  pArray = pConfig->GetArrayFor("AS");
  if (!pArray)
    return bState;

  ByteString csFind = csConfig + "State";
  for (size_t i = 0; i < pArray->size(); i++) {
    CPDF_Dictionary* pUsage = pArray->GetDictAt(i);
    if (!pUsage)
      continue;

    if (pUsage->GetStringFor("Event", "View") != csConfig)
      continue;

    CPDF_Array* pOCGs = pUsage->GetArrayFor("OCGs");
    if (!pOCGs)
      continue;

    if (FindGroup(pOCGs, pOCGDict) < 0)
      continue;

    CPDF_Dictionary* pState = pUsage->GetDictFor(csConfig);
    if (!pState)
      continue;

    bState = pState->GetStringFor(csFind) != "OFF";
  }
  return bState;
}

bool CPDF_OCContext::LoadOCGState(const CPDF_Dictionary* pOCGDict) const {
  if (!HasIntent(pOCGDict, "View", "View"))
    return true;

  ByteString csState = GetUsageTypeString(m_eUsageType);
  const CPDF_Dictionary* pUsage = pOCGDict->GetDictFor("Usage");
  if (pUsage) {
    const CPDF_Dictionary* pState = pUsage->GetDictFor(csState);
    if (pState) {
      ByteString csFind = csState + "State";
      if (pState->KeyExist(csFind))
        return pState->GetStringFor(csFind) != "OFF";
    }
    if (csState != "View") {
      pState = pUsage->GetDictFor("View");
      if (pState && pState->KeyExist("ViewState"))
        return pState->GetStringFor("ViewState") != "OFF";
    }
  }
  return LoadOCGStateFromConfig(csState, pOCGDict);
}

bool CPDF_OCContext::GetOCGVisible(const CPDF_Dictionary* pOCGDict) const {
  if (!pOCGDict)
    return false;

  const auto it = m_OGCStateCache.find(pOCGDict);
  if (it != m_OGCStateCache.end())
    return it->second;

  bool bState = LoadOCGState(pOCGDict);
  m_OGCStateCache[pOCGDict] = bState;
  return bState;
}

bool CPDF_OCContext::CheckObjectVisible(const CPDF_PageObject* pObj) const {
  for (size_t i = 0; i < pObj->m_ContentMarks.CountItems(); ++i) {
    const CPDF_ContentMarkItem* item = pObj->m_ContentMarks.GetItem(i);
    if (item->GetName() == "OC" &&
        item->GetParamType() == CPDF_ContentMarkItem::kPropertiesDict &&
        !CheckOCGVisible(item->GetParam())) {
      return false;
    }
  }
  return true;
}

bool CPDF_OCContext::GetOCGVE(const CPDF_Array* pExpression, int nLevel) const {
  if (nLevel > 32 || !pExpression)
    return false;

  ByteString csOperator = pExpression->GetStringAt(0);
  if (csOperator == "Not") {
    const CPDF_Object* pOCGObj = pExpression->GetDirectObjectAt(1);
    if (!pOCGObj)
      return false;
    if (const CPDF_Dictionary* pDict = pOCGObj->AsDictionary())
      return !GetOCGVisible(pDict);
    if (const CPDF_Array* pArray = pOCGObj->AsArray())
      return !GetOCGVE(pArray, nLevel + 1);
    return false;
  }

  if (csOperator != "Or" && csOperator != "And")
    return false;

  bool bValue = false;
  for (size_t i = 1; i < pExpression->size(); i++) {
    const CPDF_Object* pOCGObj = pExpression->GetDirectObjectAt(1);
    if (!pOCGObj)
      continue;

    bool bItem = false;
    if (const CPDF_Dictionary* pDict = pOCGObj->AsDictionary())
      bItem = GetOCGVisible(pDict);
    else if (const CPDF_Array* pArray = pOCGObj->AsArray())
      bItem = GetOCGVE(pArray, nLevel + 1);

    if (i == 1) {
      bValue = bItem;
    } else {
      if (csOperator == "Or") {
        bValue = bValue || bItem;
      } else {
        bValue = bValue && bItem;
      }
    }
  }
  return bValue;
}

bool CPDF_OCContext::LoadOCMDState(const CPDF_Dictionary* pOCMDDict) const {
  const CPDF_Array* pVE = pOCMDDict->GetArrayFor("VE");
  if (pVE)
    return GetOCGVE(pVE, 0);

  ByteString csP = pOCMDDict->GetStringFor("P", "AnyOn");
  const CPDF_Object* pOCGObj = pOCMDDict->GetDirectObjectFor("OCGs");
  if (!pOCGObj)
    return true;

  if (const CPDF_Dictionary* pDict = pOCGObj->AsDictionary())
    return GetOCGVisible(pDict);

  const CPDF_Array* pArray = pOCGObj->AsArray();
  if (!pArray)
    return true;

  bool bState = (csP == "AllOn" || csP == "AllOff");
  // At least one entry of OCGs needs to be a valid dictionary for it to be
  // considered present. See "OCGs" in table 4.49 in the PDF 1.7 spec.
  bool bValidEntrySeen = false;
  for (size_t i = 0; i < pArray->size(); i++) {
    bool bItem = true;
    const CPDF_Dictionary* pItemDict = pArray->GetDictAt(i);
    if (!pItemDict)
      continue;

    bValidEntrySeen = true;
    bItem = GetOCGVisible(pItemDict);

    if ((csP == "AnyOn" && bItem) || (csP == "AnyOff" && !bItem))
      return true;
    if ((csP == "AllOn" && !bItem) || (csP == "AllOff" && bItem))
      return false;
  }

  return !bValidEntrySeen || bState;
}

bool CPDF_OCContext::CheckOCGVisible(const CPDF_Dictionary* pOCGDict) const {
  if (!pOCGDict)
    return true;

  ByteString csType = pOCGDict->GetStringFor("Type", "OCG");
  if (csType == "OCG")
    return GetOCGVisible(pOCGDict);
  return LoadOCMDState(pOCGDict);
}
