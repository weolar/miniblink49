// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_CMAPS_CMAP_INT_H_
#define CORE_FPDFAPI_CMAPS_CMAP_INT_H_

#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/fx_system.h"

struct FXCMAP_DWordCIDMap {
  uint16_t m_HiWord;
  uint16_t m_LoWordLow;
  uint16_t m_LoWordHigh;
  uint16_t m_CID;
};

struct FXCMAP_CMap {
  enum MapType : uint8_t { Single, Range };

  const char* m_Name;                     // Raw, POD struct.
  const uint16_t* m_pWordMap;             // Raw, POD struct.
  const FXCMAP_DWordCIDMap* m_pDWordMap;  // Raw, POD struct.
  uint16_t m_WordCount;
  uint16_t m_DWordCount;
  MapType m_WordMapType;
  int8_t m_UseOffset;
};

const FXCMAP_CMap* FindEmbeddedCMap(const ByteString& name,
                                    int charset,
                                    int coding);
uint16_t CIDFromCharCode(const FXCMAP_CMap* pMap, uint32_t charcode);
uint32_t CharCodeFromCID(const FXCMAP_CMap* pMap, uint16_t cid);

#endif  // CORE_FPDFAPI_CMAPS_CMAP_INT_H_
