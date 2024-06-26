// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfdoc/cpdf_nametree.h"

#include <utility>
#include <vector>

#include "core/fpdfapi/parser/cpdf_array.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_document.h"
#include "core/fpdfapi/parser/cpdf_string.h"
#include "core/fpdfapi/parser/fpdf_parser_decode.h"

namespace {

constexpr int kNameTreeMaxRecursion = 32;

std::pair<WideString, WideString> GetNodeLimitsMaybeSwap(CPDF_Array* pLimits) {
  ASSERT(pLimits);
  WideString csLeft = pLimits->GetUnicodeTextAt(0);
  WideString csRight = pLimits->GetUnicodeTextAt(1);
  // If the lower limit is greater than the upper limit, swap them.
  if (csLeft.Compare(csRight) > 0) {
    pLimits->SetNewAt<CPDF_String>(0, csRight);
    pLimits->SetNewAt<CPDF_String>(1, csLeft);
    csLeft = pLimits->GetUnicodeTextAt(0);
    csRight = pLimits->GetUnicodeTextAt(1);
  }
  return {csLeft, csRight};
}

// Get the limit arrays that leaf array |pFind| is under in the tree with root
// |pNode|. |pLimits| will hold all the limit arrays from the leaf up to before
// the root. Return true if successful.
bool GetNodeAncestorsLimits(CPDF_Dictionary* pNode,
                            const CPDF_Array* pFind,
                            int nLevel,
                            std::vector<CPDF_Array*>* pLimits) {
  if (nLevel > kNameTreeMaxRecursion)
    return false;

  if (pNode->GetArrayFor("Names") == pFind) {
    pLimits->push_back(pNode->GetArrayFor("Limits"));
    return true;
  }

  CPDF_Array* pKids = pNode->GetArrayFor("Kids");
  if (!pKids)
    return false;

  for (size_t i = 0; i < pKids->size(); ++i) {
    CPDF_Dictionary* pKid = pKids->GetDictAt(i);
    if (!pKid)
      continue;

    if (GetNodeAncestorsLimits(pKid, pFind, nLevel + 1, pLimits)) {
      pLimits->push_back(pNode->GetArrayFor("Limits"));
      return true;
    }
  }
  return false;
}

// Upon the deletion of |csName| from leaf array |pFind|, update the ancestors
// of |pFind|. Specifically, the limits of |pFind|'s ancestors will be updated
// if needed, and any ancestors that are now empty will be removed.
bool UpdateNodesAndLimitsUponDeletion(CPDF_Dictionary* pNode,
                                      const CPDF_Array* pFind,
                                      const WideString& csName,
                                      int nLevel) {
  if (nLevel > kNameTreeMaxRecursion)
    return false;

  CPDF_Array* pLimits = pNode->GetArrayFor("Limits");
  WideString csLeft;
  WideString csRight;
  if (pLimits)
    std::tie(csLeft, csRight) = GetNodeLimitsMaybeSwap(pLimits);

  CPDF_Array* pNames = pNode->GetArrayFor("Names");
  if (pNames) {
    if (pNames != pFind)
      return false;
    if (pNames->IsEmpty() || !pLimits)
      return true;
    if (csLeft != csName && csRight != csName)
      return true;

    // Since |csName| defines |pNode|'s limits, we need to loop through the
    // names to find the new lower and upper limits.
    WideString csNewLeft = csRight;
    WideString csNewRight = csLeft;
    for (size_t i = 0; i < pNames->size() / 2; ++i) {
      WideString wsName = pNames->GetUnicodeTextAt(i * 2);
      if (wsName.Compare(csNewLeft) < 0)
        csNewLeft = wsName;
      if (wsName.Compare(csNewRight) > 0)
        csNewRight = wsName;
    }
    pLimits->SetNewAt<CPDF_String>(0, csNewLeft);
    pLimits->SetNewAt<CPDF_String>(1, csNewRight);
    return true;
  }

  CPDF_Array* pKids = pNode->GetArrayFor("Kids");
  if (!pKids)
    return false;

  // Loop through the kids to find the leaf array |pFind|.
  for (size_t i = 0; i < pKids->size(); ++i) {
    CPDF_Dictionary* pKid = pKids->GetDictAt(i);
    if (!pKid)
      continue;
    if (!UpdateNodesAndLimitsUponDeletion(pKid, pFind, csName, nLevel + 1))
      continue;

    // Remove this child node if it's empty.
    if ((pKid->KeyExist("Names") && pKid->GetArrayFor("Names")->IsEmpty()) ||
        (pKid->KeyExist("Kids") && pKid->GetArrayFor("Kids")->IsEmpty())) {
      pKids->RemoveAt(i);
    }
    if (pKids->IsEmpty() || !pLimits)
      return true;
    if (csLeft != csName && csRight != csName)
      return true;

    // Since |csName| defines |pNode|'s limits, we need to loop through the
    // kids to find the new lower and upper limits.
    WideString csNewLeft = csRight;
    WideString csNewRight = csLeft;
    for (size_t j = 0; j < pKids->size(); ++j) {
      CPDF_Array* pKidLimits = pKids->GetDictAt(j)->GetArrayFor("Limits");
      ASSERT(pKidLimits);
      if (pKidLimits->GetUnicodeTextAt(0).Compare(csNewLeft) < 0)
        csNewLeft = pKidLimits->GetUnicodeTextAt(0);
      if (pKidLimits->GetUnicodeTextAt(1).Compare(csNewRight) > 0)
        csNewRight = pKidLimits->GetUnicodeTextAt(1);
    }
    pLimits->SetNewAt<CPDF_String>(0, csNewLeft);
    pLimits->SetNewAt<CPDF_String>(1, csNewRight);
    return true;
  }
  return false;
}

// Search for |csName| in the tree with root |pNode|. If successful, return the
// value that |csName| points to; |nIndex| will be the index of |csName|,
// |ppFind| will be the leaf array that |csName| is found in, and |pFindIndex|
// will be the index of |csName| in |ppFind|. If |csName| is not found, |ppFind|
// will be the leaf array that |csName| should be added to, and |pFindIndex|
// will be the index that it should be added at.
CPDF_Object* SearchNameNodeByName(CPDF_Dictionary* pNode,
                                  const WideString& csName,
                                  int nLevel,
                                  size_t* nIndex,
                                  CPDF_Array** ppFind,
                                  int* pFindIndex) {
  if (nLevel > kNameTreeMaxRecursion)
    return nullptr;

  CPDF_Array* pLimits = pNode->GetArrayFor("Limits");
  CPDF_Array* pNames = pNode->GetArrayFor("Names");
  if (pLimits) {
    WideString csLeft;
    WideString csRight;
    std::tie(csLeft, csRight) = GetNodeLimitsMaybeSwap(pLimits);
    // Skip this node if the name to look for is smaller than its lower limit.
    if (csName.Compare(csLeft) < 0)
      return nullptr;

    // Skip this node if the name to look for is greater than its higher limit,
    // and the node itself is a leaf node.
    if (csName.Compare(csRight) > 0 && pNames) {
      if (ppFind)
        *ppFind = pNames;
      if (pFindIndex)
        *pFindIndex = pNames->size() / 2 - 1;

      return nullptr;
    }
  }

  // If the node is a leaf node, look for the name in its names array.
  if (pNames) {
    size_t dwCount = pNames->size() / 2;
    for (size_t i = 0; i < dwCount; i++) {
      WideString csValue = pNames->GetUnicodeTextAt(i * 2);
      int32_t iCompare = csValue.Compare(csName);
      if (iCompare > 0)
        break;
      if (ppFind)
        *ppFind = pNames;
      if (pFindIndex)
        *pFindIndex = i;
      if (iCompare < 0)
        continue;

      *nIndex += i;
      return pNames->GetDirectObjectAt(i * 2 + 1);
    }
    *nIndex += dwCount;
    return nullptr;
  }

  // Search through the node's children.
  CPDF_Array* pKids = pNode->GetArrayFor("Kids");
  if (!pKids)
    return nullptr;

  for (size_t i = 0; i < pKids->size(); i++) {
    CPDF_Dictionary* pKid = pKids->GetDictAt(i);
    if (!pKid)
      continue;

    CPDF_Object* pFound = SearchNameNodeByName(pKid, csName, nLevel + 1, nIndex,
                                               ppFind, pFindIndex);
    if (pFound)
      return pFound;
  }
  return nullptr;
}

// Get the key-value pair at |nIndex| in the tree with root |pNode|. If
// successful, return the value object; |csName| will be the key, |ppFind|
// will be the leaf array that this pair is in, and |pFindIndex| will be the
// index of the pair in |pFind|.
CPDF_Object* SearchNameNodeByIndex(CPDF_Dictionary* pNode,
                                   size_t nIndex,
                                   int nLevel,
                                   size_t* nCurIndex,
                                   WideString* csName,
                                   CPDF_Array** ppFind,
                                   int* pFindIndex) {
  if (nLevel > kNameTreeMaxRecursion)
    return nullptr;

  CPDF_Array* pNames = pNode->GetArrayFor("Names");
  if (pNames) {
    size_t nCount = pNames->size() / 2;
    if (nIndex >= *nCurIndex + nCount) {
      *nCurIndex += nCount;
      return nullptr;
    }
    if (ppFind)
      *ppFind = pNames;
    if (pFindIndex)
      *pFindIndex = nIndex - *nCurIndex;

    *csName = pNames->GetUnicodeTextAt((nIndex - *nCurIndex) * 2);
    return pNames->GetDirectObjectAt((nIndex - *nCurIndex) * 2 + 1);
  }

  CPDF_Array* pKids = pNode->GetArrayFor("Kids");
  if (!pKids)
    return nullptr;

  for (size_t i = 0; i < pKids->size(); i++) {
    CPDF_Dictionary* pKid = pKids->GetDictAt(i);
    if (!pKid)
      continue;
    CPDF_Object* pFound = SearchNameNodeByIndex(
        pKid, nIndex, nLevel + 1, nCurIndex, csName, ppFind, pFindIndex);
    if (pFound)
      return pFound;
  }
  return nullptr;
}

// Get the total number of key-value pairs in the tree with root |pNode|.
size_t CountNamesInternal(CPDF_Dictionary* pNode, int nLevel) {
  if (nLevel > kNameTreeMaxRecursion)
    return 0;

  CPDF_Array* pNames = pNode->GetArrayFor("Names");
  if (pNames)
    return pNames->size() / 2;

  CPDF_Array* pKids = pNode->GetArrayFor("Kids");
  if (!pKids)
    return 0;

  size_t nCount = 0;
  for (size_t i = 0; i < pKids->size(); i++) {
    CPDF_Dictionary* pKid = pKids->GetDictAt(i);
    if (!pKid)
      continue;

    nCount += CountNamesInternal(pKid, nLevel + 1);
  }
  return nCount;
}

}  // namespace

CPDF_NameTree::CPDF_NameTree(CPDF_Dictionary* pRoot) : m_pRoot(pRoot) {}

CPDF_NameTree::CPDF_NameTree(CPDF_Document* pDoc, const ByteString& category) {
  CPDF_Dictionary* pRoot = pDoc->GetRoot();
  if (!pRoot)
    return;

  CPDF_Dictionary* pNames = pRoot->GetDictFor("Names");
  if (!pNames)
    return;

  m_pRoot = pNames->GetDictFor(category);
}

CPDF_NameTree::~CPDF_NameTree() {}

size_t CPDF_NameTree::GetCount() const {
  return m_pRoot ? CountNamesInternal(m_pRoot.Get(), 0) : 0;
}

int CPDF_NameTree::GetIndex(const WideString& csName) const {
  if (!m_pRoot)
    return -1;

  size_t nIndex = 0;
  if (!SearchNameNodeByName(m_pRoot.Get(), csName, 0, &nIndex, nullptr,
                            nullptr)) {
    return -1;
  }
  return nIndex;
}

bool CPDF_NameTree::AddValueAndName(std::unique_ptr<CPDF_Object> pObj,
                                    const WideString& name) {
  if (!m_pRoot)
    return false;

  size_t nIndex = 0;
  CPDF_Array* pFind = nullptr;
  int nFindIndex = -1;
  // Fail if the tree already contains this name or if the tree is too deep.
  if (SearchNameNodeByName(m_pRoot.Get(), name, 0, &nIndex, &pFind,
                           &nFindIndex)) {
    return false;
  }

  // If the returned |pFind| is a nullptr, then |name| is smaller than all
  // existing entries in the tree, and we did not find a leaf array to place
  // |name| into. We instead will find the leftmost leaf array in which to place
  // |name| and |pObj|.
  if (!pFind) {
    size_t nCurIndex = 0;
    WideString csName;
    SearchNameNodeByIndex(m_pRoot.Get(), 0, 0, &nCurIndex, &csName, &pFind,
                          nullptr);
  }
  ASSERT(pFind);

  // Insert the name and the object into the leaf array found. Note that the
  // insertion position is right after the key-value pair returned by |index|.
  size_t nNameIndex = (nFindIndex + 1) * 2;
  size_t nValueIndex = nNameIndex + 1;
  pFind->InsertNewAt<CPDF_String>(nNameIndex, name);
  pFind->InsertAt(nValueIndex, std::move(pObj));

  // Expand the limits that the newly added name is under, if the name falls
  // outside of the limits of its leaf array or any arrays above it.
  std::vector<CPDF_Array*> pLimits;
  GetNodeAncestorsLimits(m_pRoot.Get(), pFind, 0, &pLimits);
  for (auto* pLimit : pLimits) {
    if (!pLimit)
      continue;

    if (name.Compare(pLimit->GetUnicodeTextAt(0)) < 0)
      pLimit->SetNewAt<CPDF_String>(0, name);

    if (name.Compare(pLimit->GetUnicodeTextAt(1)) > 0)
      pLimit->SetNewAt<CPDF_String>(1, name);
  }
  return true;
}

bool CPDF_NameTree::DeleteValueAndName(int nIndex) {
  if (!m_pRoot)
    return false;

  size_t nCurIndex = 0;
  WideString csName;
  CPDF_Array* pFind = nullptr;
  int nFindIndex = -1;
  // Fail if the tree does not contain |nIndex|.
  if (!SearchNameNodeByIndex(m_pRoot.Get(), nIndex, 0, &nCurIndex, &csName,
                             &pFind, &nFindIndex)) {
    return false;
  }

  // Remove the name and the object from the leaf array |pFind|.
  pFind->RemoveAt(nFindIndex * 2);
  pFind->RemoveAt(nFindIndex * 2);

  // Delete empty nodes and update the limits of |pFind|'s ancestors as needed.
  UpdateNodesAndLimitsUponDeletion(m_pRoot.Get(), pFind, csName, 0);
  return true;
}

CPDF_Object* CPDF_NameTree::LookupValueAndName(int nIndex,
                                               WideString* csName) const {
  csName->clear();
  if (!m_pRoot)
    return nullptr;

  size_t nCurIndex = 0;
  return SearchNameNodeByIndex(m_pRoot.Get(), nIndex, 0, &nCurIndex, csName,
                               nullptr, nullptr);
}

CPDF_Object* CPDF_NameTree::LookupValue(const WideString& csName) const {
  if (!m_pRoot)
    return nullptr;

  size_t nIndex = 0;
  return SearchNameNodeByName(m_pRoot.Get(), csName, 0, &nIndex, nullptr,
                              nullptr);
}

CPDF_Array* CPDF_NameTree::LookupNamedDest(CPDF_Document* pDoc,
                                           const WideString& sName) {
  CPDF_Object* pValue = LookupValue(sName);
  if (!pValue) {
    CPDF_Dictionary* pDests = pDoc->GetRoot()->GetDictFor("Dests");
    if (!pDests)
      return nullptr;
    pValue = pDests->GetDirectObjectFor(PDF_EncodeText(sName));
  }
  if (!pValue)
    return nullptr;
  if (CPDF_Array* pArray = pValue->AsArray())
    return pArray;
  if (CPDF_Dictionary* pDict = pValue->AsDictionary())
    return pDict->GetArrayFor("D");
  return nullptr;
}
