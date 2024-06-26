// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/font/cpdf_cmapmanager.h"

#include <utility>

#include "core/fpdfapi/font/cpdf_cid2unicodemap.h"
#include "core/fpdfapi/font/cpdf_cmap.h"
#include "third_party/base/ptr_util.h"

CPDF_CMapManager::CPDF_CMapManager() {}

CPDF_CMapManager::~CPDF_CMapManager() {}

RetainPtr<CPDF_CMap> CPDF_CMapManager::GetPredefinedCMap(
    const ByteString& name) {
  auto it = m_CMaps.find(name);
  if (it != m_CMaps.end())
    return it->second;

  RetainPtr<CPDF_CMap> pCMap = LoadPredefinedCMap(name);
  if (!name.IsEmpty())
    m_CMaps[name] = pCMap;

  return pCMap;
}

RetainPtr<CPDF_CMap> CPDF_CMapManager::LoadPredefinedCMap(ByteString name) {
  if (!name.IsEmpty() && name[0] == '/')
    name = name.Right(name.GetLength() - 1);

  auto pCMap = pdfium::MakeRetain<CPDF_CMap>();
  pCMap->LoadPredefined(this, name);
  return pCMap;
}

CPDF_CID2UnicodeMap* CPDF_CMapManager::GetCID2UnicodeMap(CIDSet charset) {
  if (!m_CID2UnicodeMaps[charset])
    m_CID2UnicodeMaps[charset] = LoadCID2UnicodeMap(charset);

  return m_CID2UnicodeMaps[charset].get();
}

std::unique_ptr<CPDF_CID2UnicodeMap> CPDF_CMapManager::LoadCID2UnicodeMap(
    CIDSet charset) {
  auto pMap = pdfium::MakeUnique<CPDF_CID2UnicodeMap>();
  pMap->Load(this, charset);
  return pMap;
}
