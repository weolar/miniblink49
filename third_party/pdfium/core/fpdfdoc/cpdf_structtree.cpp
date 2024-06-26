// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfdoc/cpdf_structtree.h"

#include "core/fpdfapi/parser/cpdf_array.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_document.h"
#include "core/fpdfapi/parser/cpdf_number.h"
#include "core/fpdfapi/parser/cpdf_reference.h"
#include "core/fpdfdoc/cpdf_numbertree.h"
#include "core/fpdfdoc/cpdf_structelement.h"
#include "third_party/base/ptr_util.h"

namespace {

bool IsTagged(const CPDF_Document* pDoc) {
  const CPDF_Dictionary* pCatalog = pDoc->GetRoot();
  const CPDF_Dictionary* pMarkInfo = pCatalog->GetDictFor("MarkInfo");
  return pMarkInfo && pMarkInfo->GetIntegerFor("Marked");
}

}  // namespace

// static
std::unique_ptr<CPDF_StructTree> CPDF_StructTree::LoadPage(
    const CPDF_Document* pDoc,
    const CPDF_Dictionary* pPageDict) {
  if (!IsTagged(pDoc))
    return nullptr;

  auto pTree = pdfium::MakeUnique<CPDF_StructTree>(pDoc);
  pTree->LoadPageTree(pPageDict);
  return pTree;
}

CPDF_StructTree::CPDF_StructTree(const CPDF_Document* pDoc)
    : m_pTreeRoot(pDoc->GetRoot()->GetDictFor("StructTreeRoot")),
      m_pRoleMap(m_pTreeRoot ? m_pTreeRoot->GetDictFor("RoleMap") : nullptr) {}

CPDF_StructTree::~CPDF_StructTree() = default;

void CPDF_StructTree::LoadPageTree(const CPDF_Dictionary* pPageDict) {
  m_pPage = pPageDict;
  if (!m_pTreeRoot)
    return;

  const CPDF_Object* pKids = m_pTreeRoot->GetDirectObjectFor("K");
  if (!pKids)
    return;

  uint32_t dwKids = 0;
  if (pKids->IsDictionary())
    dwKids = 1;
  else if (const CPDF_Array* pArray = pKids->AsArray())
    dwKids = pArray->size();
  else
    return;

  m_Kids.clear();
  m_Kids.resize(dwKids);
  const CPDF_Dictionary* pParentTree = m_pTreeRoot->GetDictFor("ParentTree");
  if (!pParentTree)
    return;

  CPDF_NumberTree parent_tree(pParentTree);
  int parents_id = pPageDict->GetIntegerFor("StructParents", -1);
  if (parents_id < 0)
    return;

  const CPDF_Array* pParentArray = ToArray(parent_tree.LookupValue(parents_id));
  if (!pParentArray)
    return;

  StructElementMap element_map;
  for (size_t i = 0; i < pParentArray->size(); i++) {
    if (const CPDF_Dictionary* pParent = pParentArray->GetDictAt(i))
      AddPageNode(pParent, &element_map, 0);
  }
}

RetainPtr<CPDF_StructElement> CPDF_StructTree::AddPageNode(
    const CPDF_Dictionary* pDict,
    StructElementMap* map,
    int nLevel) {
  static constexpr int kStructTreeMaxRecursion = 32;
  if (nLevel > kStructTreeMaxRecursion)
    return nullptr;

  auto it = map->find(pDict);
  if (it != map->end())
    return it->second;

  auto pElement = pdfium::MakeRetain<CPDF_StructElement>(this, nullptr, pDict);
  (*map)[pDict] = pElement;
  const CPDF_Dictionary* pParent = pDict->GetDictFor("P");
  if (!pParent || pParent->GetStringFor("Type") == "StructTreeRoot") {
    if (!AddTopLevelNode(pDict, pElement))
      map->erase(pDict);
    return pElement;
  }

  RetainPtr<CPDF_StructElement> pParentElement =
      AddPageNode(pParent, map, nLevel + 1);
  bool bSave = false;
  for (CPDF_StructKid& kid : *pParentElement->GetKids()) {
    if (kid.m_Type == CPDF_StructKid::Element && kid.m_pDict == pDict) {
      kid.m_pElement = pElement;
      bSave = true;
    }
  }
  if (!bSave)
    map->erase(pDict);
  return pElement;
}

bool CPDF_StructTree::AddTopLevelNode(
    const CPDF_Dictionary* pDict,
    const RetainPtr<CPDF_StructElement>& pElement) {
  const CPDF_Object* pObj = m_pTreeRoot->GetDirectObjectFor("K");
  if (!pObj)
    return false;

  if (pObj->IsDictionary()) {
    if (pObj->GetObjNum() != pDict->GetObjNum())
      return false;
    m_Kids[0] = pElement;
  }

  const CPDF_Array* pTopKids = pObj->AsArray();
  if (!pTopKids)
    return true;

  bool bSave = false;
  for (size_t i = 0; i < pTopKids->size(); i++) {
    const CPDF_Reference* pKidRef = ToReference(pTopKids->GetObjectAt(i));
    if (pKidRef && pKidRef->GetRefObjNum() == pDict->GetObjNum()) {
      m_Kids[i] = pElement;
      bSave = true;
    }
  }
  return bSave;
}
