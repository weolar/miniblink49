// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/cmaps/cmap_int.h"

#include <algorithm>

#include "core/fpdfapi/cpdf_modulemgr.h"
#include "core/fpdfapi/font/cpdf_fontglobals.h"
#include "core/fpdfapi/page/cpdf_pagemodule.h"
#include "third_party/base/span.h"

namespace {

struct SingleCmap {
  uint16_t code;
  uint16_t cid;
};

struct RangeCmap {
  uint16_t low;
  uint16_t high;
  uint16_t cid;
};

const FXCMAP_CMap* FindNextCMap(const FXCMAP_CMap* pMap) {
  return pMap->m_UseOffset ? pMap + pMap->m_UseOffset : nullptr;
}

}  // namespace

const FXCMAP_CMap* FindEmbeddedCMap(const ByteString& bsName,
                                    int charset,
                                    int coding) {
  CPDF_FontGlobals* pFontGlobals =
      CPDF_ModuleMgr::Get()->GetPageModule()->GetFontGlobals();

  pdfium::span<const FXCMAP_CMap> pCMaps =
      pFontGlobals->GetEmbeddedCharset(charset);

  for (size_t i = 0; i < pCMaps.size(); i++) {
    if (bsName == pCMaps[i].m_Name)
      return &pCMaps[i];
  }
  return nullptr;
}

uint16_t CIDFromCharCode(const FXCMAP_CMap* pMap, uint32_t charcode) {
  ASSERT(pMap);
  const uint16_t loword = static_cast<uint16_t>(charcode);
  if (charcode >> 16) {
    while (pMap) {
      if (pMap->m_pDWordMap) {
        const FXCMAP_DWordCIDMap* begin = pMap->m_pDWordMap;
        const auto* end = begin + pMap->m_DWordCount;
        const auto* found = std::lower_bound(
            begin, end, charcode,
            [](const FXCMAP_DWordCIDMap& element, uint32_t charcode) {
              uint16_t hiword = static_cast<uint16_t>(charcode >> 16);
              if (element.m_HiWord != hiword)
                return element.m_HiWord < hiword;
              return element.m_LoWordHigh < static_cast<uint16_t>(charcode);
            });
        if (found != end && loword >= found->m_LoWordLow &&
            loword <= found->m_LoWordHigh) {
          return found->m_CID + loword - found->m_LoWordLow;
        }
      }
      pMap = FindNextCMap(pMap);
    }
    return 0;
  }

  while (pMap && pMap->m_pWordMap) {
    switch (pMap->m_WordMapType) {
      case FXCMAP_CMap::Single: {
        const auto* begin =
            reinterpret_cast<const SingleCmap*>(pMap->m_pWordMap);
        const auto* end = begin + pMap->m_WordCount;
        const auto* found = std::lower_bound(
            begin, end, loword, [](const SingleCmap& element, uint16_t code) {
              return element.code < code;
            });
        if (found != end && found->code == loword)
          return found->cid;
        break;
      }
      case FXCMAP_CMap::Range: {
        const auto* begin =
            reinterpret_cast<const RangeCmap*>(pMap->m_pWordMap);
        const auto* end = begin + pMap->m_WordCount;
        const auto* found = std::lower_bound(
            begin, end, loword, [](const RangeCmap& element, uint16_t code) {
              return element.high < code;
            });
        if (found != end && loword >= found->low && loword <= found->high)
          return found->cid + loword - found->low;
        break;
      }
      default: {
        NOTREACHED();
        break;
      }
    }
    pMap = FindNextCMap(pMap);
  }

  return 0;
}

uint32_t CharCodeFromCID(const FXCMAP_CMap* pMap, uint16_t cid) {
  // TODO(dsinclair): This should be checking both pMap->m_WordMap and
  // pMap->m_DWordMap. There was a second while() but it was never reached as
  // the first always returns. Investigate and determine how this should
  // really be working. (https://codereview.chromium.org/2235743003 removed the
  // second while loop.)
  ASSERT(pMap);
  while (pMap) {
    switch (pMap->m_WordMapType) {
      case FXCMAP_CMap::Single: {
        const auto* pCur =
            reinterpret_cast<const SingleCmap*>(pMap->m_pWordMap);
        const auto* pEnd = pCur + pMap->m_WordCount;
        while (pCur < pEnd) {
          if (pCur->cid == cid)
            return pCur->code;
          ++pCur;
        }
        break;
      }
      case FXCMAP_CMap::Range: {
        const auto* pCur = reinterpret_cast<const RangeCmap*>(pMap->m_pWordMap);
        const auto* pEnd = pCur + pMap->m_WordCount;
        while (pCur < pEnd) {
          if (cid >= pCur->cid && cid <= pCur->cid + pCur->high - pCur->low)
            return pCur->low + cid - pCur->cid;
          ++pCur;
        }
        break;
      }
      default: {
        NOTREACHED();
        break;
      }
    }
    pMap = FindNextCMap(pMap);
  }
  return 0;
}
