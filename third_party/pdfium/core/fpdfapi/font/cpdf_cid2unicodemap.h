// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_FONT_CPDF_CID2UNICODEMAP_H_
#define CORE_FPDFAPI_FONT_CPDF_CID2UNICODEMAP_H_

#include "core/fpdfapi/font/cpdf_cidfont.h"
#include "third_party/base/span.h"

class CPDF_CMapManager;

class CPDF_CID2UnicodeMap {
 public:
  CPDF_CID2UnicodeMap();
  ~CPDF_CID2UnicodeMap();

  bool IsLoaded() const;
  wchar_t UnicodeFromCID(uint16_t CID) const;
  void Load(CPDF_CMapManager* pMgr, CIDSet charset);

 private:
  CIDSet m_Charset;
  pdfium::span<const uint16_t> m_pEmbeddedMap;
};

#endif  // CORE_FPDFAPI_FONT_CPDF_CID2UNICODEMAP_H_
