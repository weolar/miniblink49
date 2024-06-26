// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfdoc/cpdf_bookmarktree.h"

#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_document.h"

CPDF_BookmarkTree::CPDF_BookmarkTree(CPDF_Document* pDoc) : m_pDocument(pDoc) {}

CPDF_BookmarkTree::~CPDF_BookmarkTree() = default;

CPDF_Bookmark CPDF_BookmarkTree::GetFirstChild(
    const CPDF_Bookmark& parent) const {
  const CPDF_Dictionary* pParentDict = parent.GetDict();
  if (pParentDict)
    return CPDF_Bookmark(pParentDict->GetDictFor("First"));

  const CPDF_Dictionary* pRoot = m_pDocument->GetRoot();
  if (!pRoot)
    return CPDF_Bookmark();

  const CPDF_Dictionary* pOutlines = pRoot->GetDictFor("Outlines");
  return pOutlines ? CPDF_Bookmark(pOutlines->GetDictFor("First"))
                   : CPDF_Bookmark();
}

CPDF_Bookmark CPDF_BookmarkTree::GetNextSibling(
    const CPDF_Bookmark& bookmark) const {
  const CPDF_Dictionary* pDict = bookmark.GetDict();
  if (!pDict)
    return CPDF_Bookmark();

  const CPDF_Dictionary* pNext = pDict->GetDictFor("Next");
  return pNext == pDict ? CPDF_Bookmark() : CPDF_Bookmark(pNext);
}
