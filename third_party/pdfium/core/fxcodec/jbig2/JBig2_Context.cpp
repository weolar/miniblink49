// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcodec/jbig2/JBig2_Context.h"

#include <string.h>

#include <algorithm>
#include <limits>
#include <list>
#include <utility>
#include <vector>

#include "core/fpdfapi/parser/cpdf_stream.h"
#include "core/fpdfapi/parser/cpdf_stream_acc.h"
#include "core/fxcodec/jbig2/JBig2_ArithDecoder.h"
#include "core/fxcodec/jbig2/JBig2_BitStream.h"
#include "core/fxcodec/jbig2/JBig2_GrdProc.h"
#include "core/fxcodec/jbig2/JBig2_GrrdProc.h"
#include "core/fxcodec/jbig2/JBig2_HtrdProc.h"
#include "core/fxcodec/jbig2/JBig2_PddProc.h"
#include "core/fxcodec/jbig2/JBig2_SddProc.h"
#include "core/fxcodec/jbig2/JBig2_TrdProc.h"
#include "core/fxcrt/fx_safe_types.h"
#include "core/fxcrt/pauseindicator_iface.h"
#include "third_party/base/ptr_util.h"

namespace {

size_t GetHuffContextSize(uint8_t val) {
  return val == 0 ? 65536 : val == 1 ? 8192 : 1024;
}

size_t GetRefAggContextSize(bool val) {
  return val ? 1024 : 8192;
}

}  // namespace

// Implement a very small least recently used (LRU) cache. It is very
// common for a JBIG2 dictionary to span multiple pages in a PDF file,
// and we do not want to decode the same dictionary over and over
// again. We key off of the memory location of the dictionary. The
// list keeps track of the freshness of entries, with freshest ones
// at the front. Even a tiny cache size like 2 makes a dramatic
// difference for typical JBIG2 documents.
static const size_t kSymbolDictCacheMaxSize = 2;
static_assert(kSymbolDictCacheMaxSize > 0,
              "Symbol Dictionary Cache must have non-zero size");

CJBig2_Context::CJBig2_Context(const RetainPtr<CPDF_StreamAcc>& pGlobalStream,
                               const RetainPtr<CPDF_StreamAcc>& pSrcStream,
                               std::list<CJBig2_CachePair>* pSymbolDictCache,
                               bool bIsGlobal)
    : m_pStream(pdfium::MakeUnique<CJBig2_BitStream>(pSrcStream)),
      m_HuffmanTables(CJBig2_HuffmanTable::kNumHuffmanTables),
      m_nSegmentDecoded(0),
      m_bInPage(false),
      m_bBufSpecified(false),
      m_PauseStep(10),
      m_ProcessingStatus(FXCODEC_STATUS_FRAME_READY),
      m_dwOffset(0),
      m_pSymbolDictCache(pSymbolDictCache),
      m_bIsGlobal(bIsGlobal) {
  if (pGlobalStream && pGlobalStream->GetSize() > 0) {
    m_pGlobalContext = pdfium::MakeUnique<CJBig2_Context>(
        nullptr, pGlobalStream, pSymbolDictCache, true);
  }
}

CJBig2_Context::~CJBig2_Context() {}

JBig2_Result CJBig2_Context::DecodeSequential(PauseIndicatorIface* pPause) {
  if (m_pStream->getByteLeft() <= 0)
    return JBig2_Result::kEndReached;

  while (m_pStream->getByteLeft() >= JBIG2_MIN_SEGMENT_SIZE) {
    JBig2_Result nRet;
    if (!m_pSegment) {
      m_pSegment = pdfium::MakeUnique<CJBig2_Segment>();
      nRet = ParseSegmentHeader(m_pSegment.get());
      if (nRet != JBig2_Result::kSuccess) {
        m_pSegment.reset();
        return nRet;
      }
      m_dwOffset = m_pStream->getOffset();
    }
    nRet = ParseSegmentData(m_pSegment.get(), pPause);
    if (m_ProcessingStatus == FXCODEC_STATUS_DECODE_TOBECONTINUE) {
      m_ProcessingStatus = FXCODEC_STATUS_DECODE_TOBECONTINUE;
      m_PauseStep = 2;
      return JBig2_Result::kSuccess;
    }
    if (nRet == JBig2_Result::kEndReached) {
      m_pSegment.reset();
      return JBig2_Result::kSuccess;
    }
    if (nRet != JBig2_Result::kSuccess) {
      m_pSegment.reset();
      return nRet;
    }
    if (m_pSegment->m_dwData_length != 0xffffffff) {
      m_dwOffset += m_pSegment->m_dwData_length;
      if (!m_dwOffset.IsValid())
        return JBig2_Result::kFailure;

      m_pStream->setOffset(m_dwOffset.ValueOrDie());
    } else {
      m_pStream->offset(4);
    }
    m_SegmentList.push_back(std::move(m_pSegment));
    if (m_pStream->getByteLeft() > 0 && m_pPage && pPause &&
        pPause->NeedToPauseNow()) {
      m_ProcessingStatus = FXCODEC_STATUS_DECODE_TOBECONTINUE;
      m_PauseStep = 2;
      return JBig2_Result::kSuccess;
    }
  }
  return JBig2_Result::kSuccess;
}

JBig2_Result CJBig2_Context::DecodeRandomFirstPage(
    PauseIndicatorIface* pPause) {
  while (m_pStream->getByteLeft() > JBIG2_MIN_SEGMENT_SIZE) {
    auto pSegment = pdfium::MakeUnique<CJBig2_Segment>();
    JBig2_Result nRet = ParseSegmentHeader(pSegment.get());
    if (nRet != JBig2_Result::kSuccess)
      return nRet;

    if (pSegment->m_cFlags.s.type == 51)
      break;

    m_SegmentList.push_back(std::move(pSegment));
    if (pPause && pPause->NeedToPauseNow()) {
      m_PauseStep = 3;
      m_ProcessingStatus = FXCODEC_STATUS_DECODE_TOBECONTINUE;
      return JBig2_Result::kSuccess;
    }
  }
  m_nSegmentDecoded = 0;
  return DecodeRandom(pPause);
}

JBig2_Result CJBig2_Context::DecodeRandom(PauseIndicatorIface* pPause) {
  for (; m_nSegmentDecoded < m_SegmentList.size(); ++m_nSegmentDecoded) {
    JBig2_Result nRet =
        ParseSegmentData(m_SegmentList[m_nSegmentDecoded].get(), pPause);
    if (nRet == JBig2_Result::kEndReached)
      return JBig2_Result::kSuccess;

    if (nRet != JBig2_Result::kSuccess)
      return nRet;

    if (m_pPage && pPause && pPause->NeedToPauseNow()) {
      m_PauseStep = 4;
      m_ProcessingStatus = FXCODEC_STATUS_DECODE_TOBECONTINUE;
      return JBig2_Result::kSuccess;
    }
  }
  return JBig2_Result::kSuccess;
}

bool CJBig2_Context::GetFirstPage(uint8_t* pBuf,
                                  int32_t width,
                                  int32_t height,
                                  int32_t stride,
                                  PauseIndicatorIface* pPause) {
  if (m_pGlobalContext) {
    JBig2_Result nRet = m_pGlobalContext->DecodeSequential(pPause);
    if (nRet != JBig2_Result::kSuccess) {
      m_ProcessingStatus = FXCODEC_STATUS_ERROR;
      return nRet == JBig2_Result::kSuccess;
    }
  }
  m_PauseStep = 0;
  m_pPage = pdfium::MakeUnique<CJBig2_Image>(width, height, stride, pBuf);
  m_bBufSpecified = true;
  if (pPause && pPause->NeedToPauseNow()) {
    m_PauseStep = 1;
    m_ProcessingStatus = FXCODEC_STATUS_DECODE_TOBECONTINUE;
    return true;
  }
  return Continue(pPause);
}

bool CJBig2_Context::Continue(PauseIndicatorIface* pPause) {
  m_ProcessingStatus = FXCODEC_STATUS_DECODE_READY;
  JBig2_Result nRet = JBig2_Result::kSuccess;
  if (m_PauseStep <= 2) {
    nRet = DecodeSequential(pPause);
  } else if (m_PauseStep == 3) {
    nRet = DecodeRandomFirstPage(pPause);
  } else if (m_PauseStep == 4) {
    nRet = DecodeRandom(pPause);
  } else if (m_PauseStep == 5) {
    m_ProcessingStatus = FXCODEC_STATUS_DECODE_FINISH;
    return true;
  }
  if (m_ProcessingStatus == FXCODEC_STATUS_DECODE_TOBECONTINUE)
    return nRet == JBig2_Result::kSuccess;

  m_PauseStep = 5;
  if (!m_bBufSpecified && nRet == JBig2_Result::kSuccess) {
    m_ProcessingStatus = FXCODEC_STATUS_DECODE_FINISH;
    return true;
  }
  m_ProcessingStatus = nRet == JBig2_Result::kSuccess
                           ? FXCODEC_STATUS_DECODE_FINISH
                           : FXCODEC_STATUS_ERROR;
  return nRet == JBig2_Result::kSuccess;
}

CJBig2_Segment* CJBig2_Context::FindSegmentByNumber(uint32_t dwNumber) {
  if (m_pGlobalContext) {
    CJBig2_Segment* pSeg = m_pGlobalContext->FindSegmentByNumber(dwNumber);
    if (pSeg)
      return pSeg;
  }
  for (const auto& pSeg : m_SegmentList) {
    if (pSeg->m_dwNumber == dwNumber)
      return pSeg.get();
  }
  return nullptr;
}

CJBig2_Segment* CJBig2_Context::FindReferredTableSegmentByIndex(
    CJBig2_Segment* pSegment,
    int32_t nIndex) {
  static const uint8_t kTableType = 53;
  int32_t count = 0;
  for (int32_t i = 0; i < pSegment->m_nReferred_to_segment_count; ++i) {
    CJBig2_Segment* pSeg =
        FindSegmentByNumber(pSegment->m_Referred_to_segment_numbers[i]);
    if (pSeg && pSeg->m_cFlags.s.type == kTableType) {
      if (count == nIndex)
        return pSeg;
      ++count;
    }
  }
  return nullptr;
}

JBig2_Result CJBig2_Context::ParseSegmentHeader(CJBig2_Segment* pSegment) {
  if (m_pStream->readInteger(&pSegment->m_dwNumber) != 0 ||
      m_pStream->read1Byte(&pSegment->m_cFlags.c) != 0) {
    return JBig2_Result::kFailure;
  }

  uint8_t cTemp = m_pStream->getCurByte();
  if ((cTemp >> 5) == 7) {
    if (m_pStream->readInteger(
            (uint32_t*)&pSegment->m_nReferred_to_segment_count) != 0) {
      return JBig2_Result::kFailure;
    }
    pSegment->m_nReferred_to_segment_count &= 0x1fffffff;
    if (pSegment->m_nReferred_to_segment_count >
        JBIG2_MAX_REFERRED_SEGMENT_COUNT) {
      return JBig2_Result::kFailure;
    }
  } else {
    if (m_pStream->read1Byte(&cTemp) != 0)
      return JBig2_Result::kFailure;

    pSegment->m_nReferred_to_segment_count = cTemp >> 5;
  }
  uint8_t cSSize =
      pSegment->m_dwNumber > 65536 ? 4 : pSegment->m_dwNumber > 256 ? 2 : 1;
  uint8_t cPSize = pSegment->m_cFlags.s.page_association_size ? 4 : 1;
  if (pSegment->m_nReferred_to_segment_count) {
    pSegment->m_Referred_to_segment_numbers.resize(
        pSegment->m_nReferred_to_segment_count);
    for (int32_t i = 0; i < pSegment->m_nReferred_to_segment_count; ++i) {
      switch (cSSize) {
        case 1:
          if (m_pStream->read1Byte(&cTemp) != 0)
            return JBig2_Result::kFailure;

          pSegment->m_Referred_to_segment_numbers[i] = cTemp;
          break;
        case 2:
          uint16_t wTemp;
          if (m_pStream->readShortInteger(&wTemp) != 0)
            return JBig2_Result::kFailure;

          pSegment->m_Referred_to_segment_numbers[i] = wTemp;
          break;
        case 4:
          uint32_t dwTemp;
          if (m_pStream->readInteger(&dwTemp) != 0)
            return JBig2_Result::kFailure;

          pSegment->m_Referred_to_segment_numbers[i] = dwTemp;
          break;
      }
      if (pSegment->m_Referred_to_segment_numbers[i] >= pSegment->m_dwNumber)
        return JBig2_Result::kFailure;
    }
  }
  if (cPSize == 1) {
    if (m_pStream->read1Byte(&cTemp) != 0)
      return JBig2_Result::kFailure;
    pSegment->m_dwPage_association = cTemp;
  } else if (m_pStream->readInteger(&pSegment->m_dwPage_association) != 0) {
    return JBig2_Result::kFailure;
  }
  if (m_pStream->readInteger(&pSegment->m_dwData_length) != 0)
    return JBig2_Result::kFailure;

  pSegment->m_dwObjNum = m_pStream->getObjNum();
  pSegment->m_dwDataOffset = m_pStream->getOffset();
  pSegment->m_State = JBIG2_SEGMENT_DATA_UNPARSED;
  return JBig2_Result::kSuccess;
}

JBig2_Result CJBig2_Context::ParseSegmentData(CJBig2_Segment* pSegment,
                                              PauseIndicatorIface* pPause) {
  JBig2_Result ret = ProcessingParseSegmentData(pSegment, pPause);
  while (m_ProcessingStatus == FXCODEC_STATUS_DECODE_TOBECONTINUE &&
         m_pStream->getByteLeft() > 0) {
    ret = ProcessingParseSegmentData(pSegment, pPause);
  }
  return ret;
}

JBig2_Result CJBig2_Context::ProcessingParseSegmentData(
    CJBig2_Segment* pSegment,
    PauseIndicatorIface* pPause) {
  switch (pSegment->m_cFlags.s.type) {
    case 0:
      return ParseSymbolDict(pSegment);
    case 4:
    case 6:
    case 7:
      if (!m_bInPage)
        return JBig2_Result::kFailure;
      return ParseTextRegion(pSegment);
    case 16:
      return ParsePatternDict(pSegment, pPause);
    case 20:
    case 22:
    case 23:
      if (!m_bInPage)
        return JBig2_Result::kFailure;
      return ParseHalftoneRegion(pSegment, pPause);
    case 36:
    case 38:
    case 39:
      if (!m_bInPage)
        return JBig2_Result::kFailure;
      return ParseGenericRegion(pSegment, pPause);
    case 40:
    case 42:
    case 43:
      if (!m_bInPage)
        return JBig2_Result::kFailure;
      return ParseGenericRefinementRegion(pSegment);
    case 48: {
      uint16_t wTemp;
      auto pPageInfo = pdfium::MakeUnique<JBig2PageInfo>();
      if (m_pStream->readInteger(&pPageInfo->m_dwWidth) != 0 ||
          m_pStream->readInteger(&pPageInfo->m_dwHeight) != 0 ||
          m_pStream->readInteger(&pPageInfo->m_dwResolutionX) != 0 ||
          m_pStream->readInteger(&pPageInfo->m_dwResolutionY) != 0 ||
          m_pStream->read1Byte(&pPageInfo->m_cFlags) != 0 ||
          m_pStream->readShortInteger(&wTemp) != 0) {
        return JBig2_Result::kFailure;
      }
      pPageInfo->m_bIsStriped = !!(wTemp & 0x8000);
      pPageInfo->m_wMaxStripeSize = wTemp & 0x7fff;
      bool bMaxHeight = (pPageInfo->m_dwHeight == 0xffffffff);
      if (bMaxHeight && pPageInfo->m_bIsStriped != true)
        pPageInfo->m_bIsStriped = true;

      if (!m_bBufSpecified) {
        uint32_t height =
            bMaxHeight ? pPageInfo->m_wMaxStripeSize : pPageInfo->m_dwHeight;
        m_pPage =
            pdfium::MakeUnique<CJBig2_Image>(pPageInfo->m_dwWidth, height);
      }

      if (!m_pPage->data()) {
        m_ProcessingStatus = FXCODEC_STATUS_ERROR;
        return JBig2_Result::kFailure;
      }

      m_pPage->Fill((pPageInfo->m_cFlags & 4) ? 1 : 0);
      m_PageInfoList.push_back(std::move(pPageInfo));
      m_bInPage = true;
    } break;
    case 49:
      m_bInPage = false;
      return JBig2_Result::kEndReached;
      break;
    case 50:
      m_pStream->offset(pSegment->m_dwData_length);
      break;
    case 51:
      return JBig2_Result::kEndReached;
    case 52:
      m_pStream->offset(pSegment->m_dwData_length);
      break;
    case 53:
      return ParseTable(pSegment);
    case 62:
      m_pStream->offset(pSegment->m_dwData_length);
      break;
    default:
      break;
  }
  return JBig2_Result::kSuccess;
}

JBig2_Result CJBig2_Context::ParseSymbolDict(CJBig2_Segment* pSegment) {
  uint16_t wFlags;
  if (m_pStream->readShortInteger(&wFlags) != 0)
    return JBig2_Result::kFailure;

  auto pSymbolDictDecoder = pdfium::MakeUnique<CJBig2_SDDProc>();
  pSymbolDictDecoder->SDHUFF = wFlags & 0x0001;
  pSymbolDictDecoder->SDREFAGG = (wFlags >> 1) & 0x0001;
  pSymbolDictDecoder->SDTEMPLATE = (wFlags >> 10) & 0x0003;
  pSymbolDictDecoder->SDRTEMPLATE = !!((wFlags >> 12) & 0x0003);
  if (pSymbolDictDecoder->SDHUFF == 0) {
    const uint32_t dwTemp = (pSymbolDictDecoder->SDTEMPLATE == 0) ? 8 : 2;
    for (uint32_t i = 0; i < dwTemp; ++i) {
      if (m_pStream->read1Byte((uint8_t*)&pSymbolDictDecoder->SDAT[i]) != 0)
        return JBig2_Result::kFailure;
    }
  }
  if (pSymbolDictDecoder->SDREFAGG == 1 && !pSymbolDictDecoder->SDRTEMPLATE) {
    for (int32_t i = 0; i < 4; ++i) {
      if (m_pStream->read1Byte((uint8_t*)&pSymbolDictDecoder->SDRAT[i]) != 0)
        return JBig2_Result::kFailure;
    }
  }
  if (m_pStream->readInteger(&pSymbolDictDecoder->SDNUMEXSYMS) != 0 ||
      m_pStream->readInteger(&pSymbolDictDecoder->SDNUMNEWSYMS) != 0) {
    return JBig2_Result::kFailure;
  }
  if (pSymbolDictDecoder->SDNUMEXSYMS > JBIG2_MAX_EXPORT_SYSMBOLS ||
      pSymbolDictDecoder->SDNUMNEWSYMS > JBIG2_MAX_NEW_SYSMBOLS) {
    return JBig2_Result::kFailure;
  }
  for (int32_t i = 0; i < pSegment->m_nReferred_to_segment_count; ++i) {
    if (!FindSegmentByNumber(pSegment->m_Referred_to_segment_numbers[i]))
      return JBig2_Result::kFailure;
  }
  CJBig2_Segment* pLRSeg = nullptr;
  pSymbolDictDecoder->SDNUMINSYMS = 0;
  for (int32_t i = 0; i < pSegment->m_nReferred_to_segment_count; ++i) {
    CJBig2_Segment* pSeg =
        FindSegmentByNumber(pSegment->m_Referred_to_segment_numbers[i]);
    if (pSeg->m_cFlags.s.type == 0) {
      pSymbolDictDecoder->SDNUMINSYMS += pSeg->m_SymbolDict->NumImages();
      pLRSeg = pSeg;
    }
  }

  std::unique_ptr<CJBig2_Image*, FxFreeDeleter> SDINSYMS;
  if (pSymbolDictDecoder->SDNUMINSYMS != 0) {
    SDINSYMS.reset(FX_Alloc(CJBig2_Image*, pSymbolDictDecoder->SDNUMINSYMS));
    uint32_t dwTemp = 0;
    for (int32_t i = 0; i < pSegment->m_nReferred_to_segment_count; ++i) {
      CJBig2_Segment* pSeg =
          FindSegmentByNumber(pSegment->m_Referred_to_segment_numbers[i]);
      if (pSeg->m_cFlags.s.type == 0) {
        const CJBig2_SymbolDict& dict = *pSeg->m_SymbolDict.get();
        for (size_t j = 0; j < dict.NumImages(); ++j)
          SDINSYMS.get()[dwTemp + j] = dict.GetImage(j);
        dwTemp += dict.NumImages();
      }
    }
  }
  pSymbolDictDecoder->SDINSYMS = SDINSYMS.get();

  uint8_t cSDHUFFDH = (wFlags >> 2) & 0x0003;
  uint8_t cSDHUFFDW = (wFlags >> 4) & 0x0003;
  if (pSymbolDictDecoder->SDHUFF == 1) {
    if (cSDHUFFDH == 2 || cSDHUFFDW == 2)
      return JBig2_Result::kFailure;

    int32_t nIndex = 0;
    if (cSDHUFFDH == 0) {
      pSymbolDictDecoder->SDHUFFDH = GetHuffmanTable(4);
    } else if (cSDHUFFDH == 1) {
      pSymbolDictDecoder->SDHUFFDH = GetHuffmanTable(5);
    } else {
      CJBig2_Segment* pSeg =
          FindReferredTableSegmentByIndex(pSegment, nIndex++);
      if (!pSeg)
        return JBig2_Result::kFailure;
      pSymbolDictDecoder->SDHUFFDH = pSeg->m_HuffmanTable.get();
    }
    if (cSDHUFFDW == 0) {
      pSymbolDictDecoder->SDHUFFDW = GetHuffmanTable(2);
    } else if (cSDHUFFDW == 1) {
      pSymbolDictDecoder->SDHUFFDW = GetHuffmanTable(3);
    } else {
      CJBig2_Segment* pSeg =
          FindReferredTableSegmentByIndex(pSegment, nIndex++);
      if (!pSeg)
        return JBig2_Result::kFailure;
      pSymbolDictDecoder->SDHUFFDW = pSeg->m_HuffmanTable.get();
    }
    uint8_t cSDHUFFBMSIZE = (wFlags >> 6) & 0x0001;
    if (cSDHUFFBMSIZE == 0) {
      pSymbolDictDecoder->SDHUFFBMSIZE = GetHuffmanTable(1);
    } else {
      CJBig2_Segment* pSeg =
          FindReferredTableSegmentByIndex(pSegment, nIndex++);
      if (!pSeg)
        return JBig2_Result::kFailure;
      pSymbolDictDecoder->SDHUFFBMSIZE = pSeg->m_HuffmanTable.get();
    }
    if (pSymbolDictDecoder->SDREFAGG == 1) {
      uint8_t cSDHUFFAGGINST = (wFlags >> 7) & 0x0001;
      if (cSDHUFFAGGINST == 0) {
        pSymbolDictDecoder->SDHUFFAGGINST = GetHuffmanTable(1);
      } else {
        CJBig2_Segment* pSeg =
            FindReferredTableSegmentByIndex(pSegment, nIndex++);
        if (!pSeg)
          return JBig2_Result::kFailure;
        pSymbolDictDecoder->SDHUFFAGGINST = pSeg->m_HuffmanTable.get();
      }
    }
  }

  const bool bUseGbContext = (pSymbolDictDecoder->SDHUFF == 0);
  const bool bUseGrContext = (pSymbolDictDecoder->SDREFAGG == 1);
  const size_t gbContextSize =
      GetHuffContextSize(pSymbolDictDecoder->SDTEMPLATE);
  const size_t grContextSize =
      GetRefAggContextSize(pSymbolDictDecoder->SDRTEMPLATE);
  std::vector<JBig2ArithCtx> gbContext;
  std::vector<JBig2ArithCtx> grContext;
  if ((wFlags & 0x0100) && pLRSeg) {
    if (bUseGbContext) {
      gbContext = pLRSeg->m_SymbolDict->GbContext();
      if (gbContext.size() != gbContextSize)
        return JBig2_Result::kFailure;
    }
    if (bUseGrContext) {
      grContext = pLRSeg->m_SymbolDict->GrContext();
      if (grContext.size() != grContextSize)
        return JBig2_Result::kFailure;
    }
  } else {
    if (bUseGbContext)
      gbContext.resize(gbContextSize);
    if (bUseGrContext)
      grContext.resize(grContextSize);
  }

  CJBig2_CacheKey key =
      CJBig2_CacheKey(pSegment->m_dwObjNum, pSegment->m_dwDataOffset);
  bool cache_hit = false;
  pSegment->m_nResultType = JBIG2_SYMBOL_DICT_POINTER;
  if (m_bIsGlobal && key.first != 0) {
    for (auto it = m_pSymbolDictCache->begin(); it != m_pSymbolDictCache->end();
         ++it) {
      if (it->first == key) {
        pSegment->m_SymbolDict = it->second->DeepCopy();
        m_pSymbolDictCache->push_front(
            CJBig2_CachePair(key, std::move(it->second)));
        m_pSymbolDictCache->erase(it);
        cache_hit = true;
        break;
      }
    }
  }
  if (!cache_hit) {
    if (bUseGbContext) {
      auto pArithDecoder =
          pdfium::MakeUnique<CJBig2_ArithDecoder>(m_pStream.get());
      pSegment->m_SymbolDict = pSymbolDictDecoder->DecodeArith(
          pArithDecoder.get(), &gbContext, &grContext);
      if (!pSegment->m_SymbolDict)
        return JBig2_Result::kFailure;

      m_pStream->alignByte();
      m_pStream->offset(2);
    } else {
      pSegment->m_SymbolDict = pSymbolDictDecoder->DecodeHuffman(
          m_pStream.get(), &gbContext, &grContext);
      if (!pSegment->m_SymbolDict)
        return JBig2_Result::kFailure;
      m_pStream->alignByte();
    }
    if (m_bIsGlobal) {
      std::unique_ptr<CJBig2_SymbolDict> value =
          pSegment->m_SymbolDict->DeepCopy();
      size_t size = m_pSymbolDictCache->size();
      while (size >= kSymbolDictCacheMaxSize) {
        m_pSymbolDictCache->pop_back();
        --size;
      }
      m_pSymbolDictCache->push_front(CJBig2_CachePair(key, std::move(value)));
    }
  }
  if (wFlags & 0x0200) {
    if (bUseGbContext)
      pSegment->m_SymbolDict->SetGbContext(std::move(gbContext));
    if (bUseGrContext)
      pSegment->m_SymbolDict->SetGrContext(std::move(grContext));
  }
  return JBig2_Result::kSuccess;
}

JBig2_Result CJBig2_Context::ParseTextRegion(CJBig2_Segment* pSegment) {
  uint16_t wFlags;
  JBig2RegionInfo ri;
  if (ParseRegionInfo(&ri) != JBig2_Result::kSuccess ||
      m_pStream->readShortInteger(&wFlags) != 0) {
    return JBig2_Result::kFailure;
  }
  if (!CJBig2_Image::IsValidImageSize(ri.width, ri.height))
    return JBig2_Result::kFailure;

  auto pTRD = pdfium::MakeUnique<CJBig2_TRDProc>();
  pTRD->SBW = ri.width;
  pTRD->SBH = ri.height;
  pTRD->SBHUFF = wFlags & 0x0001;
  pTRD->SBREFINE = (wFlags >> 1) & 0x0001;
  uint32_t dwTemp = (wFlags >> 2) & 0x0003;
  pTRD->SBSTRIPS = 1 << dwTemp;
  pTRD->REFCORNER = (JBig2Corner)((wFlags >> 4) & 0x0003);
  pTRD->TRANSPOSED = (wFlags >> 6) & 0x0001;
  pTRD->SBCOMBOP = (JBig2ComposeOp)((wFlags >> 7) & 0x0003);
  pTRD->SBDEFPIXEL = (wFlags >> 9) & 0x0001;
  pTRD->SBDSOFFSET = (wFlags >> 10) & 0x001f;
  if (pTRD->SBDSOFFSET >= 0x0010) {
    pTRD->SBDSOFFSET = pTRD->SBDSOFFSET - 0x0020;
  }
  pTRD->SBRTEMPLATE = !!((wFlags >> 15) & 0x0001);

  if (pTRD->SBHUFF == 1 && m_pStream->readShortInteger(&wFlags) != 0) {
    return JBig2_Result::kFailure;
  }
  if (pTRD->SBREFINE == 1 && !pTRD->SBRTEMPLATE) {
    for (int32_t i = 0; i < 4; ++i) {
      if (m_pStream->read1Byte((uint8_t*)&pTRD->SBRAT[i]) != 0)
        return JBig2_Result::kFailure;
    }
  }
  if (m_pStream->readInteger(&pTRD->SBNUMINSTANCES) != 0)
    return JBig2_Result::kFailure;

  // Assume each instance takes at least 0.25 bits when encoded. That means for
  // a stream of length N bytes, there can be at most 32N instances. This is a
  // conservative estimate just to sanitize the |SBNUMINSTANCES| value.
  // Use FX_SAFE_INT32 to be safe, though it should never overflow because PDFs
  // have a maximum size of roughly 11 GB.
  FX_SAFE_INT32 nMaxStripInstances = m_pStream->getLength();
  nMaxStripInstances *= 32;
  if (pTRD->SBNUMINSTANCES > nMaxStripInstances.ValueOrDie())
    return JBig2_Result::kFailure;

  for (int32_t i = 0; i < pSegment->m_nReferred_to_segment_count; ++i) {
    if (!FindSegmentByNumber(pSegment->m_Referred_to_segment_numbers[i]))
      return JBig2_Result::kFailure;
  }

  pTRD->SBNUMSYMS = 0;
  for (int32_t i = 0; i < pSegment->m_nReferred_to_segment_count; ++i) {
    CJBig2_Segment* pSeg =
        FindSegmentByNumber(pSegment->m_Referred_to_segment_numbers[i]);
    if (pSeg->m_cFlags.s.type == 0) {
      pTRD->SBNUMSYMS += pSeg->m_SymbolDict->NumImages();
    }
  }

  std::unique_ptr<CJBig2_Image*, FxFreeDeleter> SBSYMS;
  if (pTRD->SBNUMSYMS > 0) {
    SBSYMS.reset(FX_Alloc(CJBig2_Image*, pTRD->SBNUMSYMS));
    dwTemp = 0;
    for (int32_t i = 0; i < pSegment->m_nReferred_to_segment_count; ++i) {
      CJBig2_Segment* pSeg =
          FindSegmentByNumber(pSegment->m_Referred_to_segment_numbers[i]);
      if (pSeg->m_cFlags.s.type == 0) {
        const CJBig2_SymbolDict& dict = *pSeg->m_SymbolDict.get();
        for (size_t j = 0; j < dict.NumImages(); ++j)
          SBSYMS.get()[dwTemp + j] = dict.GetImage(j);
        dwTemp += dict.NumImages();
      }
    }
    pTRD->SBSYMS = SBSYMS.get();
  } else {
    pTRD->SBSYMS = nullptr;
  }

  if (pTRD->SBHUFF == 1) {
    std::vector<JBig2HuffmanCode> SBSYMCODES =
        DecodeSymbolIDHuffmanTable(pTRD->SBNUMSYMS);
    if (SBSYMCODES.empty())
      return JBig2_Result::kFailure;

    m_pStream->alignByte();
    pTRD->SBSYMCODES = std::move(SBSYMCODES);
  } else {
    dwTemp = 0;
    while ((uint32_t)(1 << dwTemp) < pTRD->SBNUMSYMS) {
      ++dwTemp;
    }
    pTRD->SBSYMCODELEN = (uint8_t)dwTemp;
  }

  if (pTRD->SBHUFF == 1) {
    uint8_t cSBHUFFFS = wFlags & 0x0003;
    uint8_t cSBHUFFDS = (wFlags >> 2) & 0x0003;
    uint8_t cSBHUFFDT = (wFlags >> 4) & 0x0003;
    uint8_t cSBHUFFRDW = (wFlags >> 6) & 0x0003;
    uint8_t cSBHUFFRDH = (wFlags >> 8) & 0x0003;
    uint8_t cSBHUFFRDX = (wFlags >> 10) & 0x0003;
    uint8_t cSBHUFFRDY = (wFlags >> 12) & 0x0003;
    uint8_t cSBHUFFRSIZE = (wFlags >> 14) & 0x0001;
    if (cSBHUFFFS == 2 || cSBHUFFRDW == 2 || cSBHUFFRDH == 2 ||
        cSBHUFFRDX == 2 || cSBHUFFRDY == 2) {
      return JBig2_Result::kFailure;
    }
    int32_t nIndex = 0;
    if (cSBHUFFFS == 0) {
      pTRD->SBHUFFFS = GetHuffmanTable(6);
    } else if (cSBHUFFFS == 1) {
      pTRD->SBHUFFFS = GetHuffmanTable(7);
    } else {
      CJBig2_Segment* pSeg =
          FindReferredTableSegmentByIndex(pSegment, nIndex++);
      if (!pSeg)
        return JBig2_Result::kFailure;
      pTRD->SBHUFFFS = pSeg->m_HuffmanTable.get();
    }
    if (cSBHUFFDS == 0) {
      pTRD->SBHUFFDS = GetHuffmanTable(8);
    } else if (cSBHUFFDS == 1) {
      pTRD->SBHUFFDS = GetHuffmanTable(9);
    } else if (cSBHUFFDS == 2) {
      pTRD->SBHUFFDS = GetHuffmanTable(10);
    } else {
      CJBig2_Segment* pSeg =
          FindReferredTableSegmentByIndex(pSegment, nIndex++);
      if (!pSeg)
        return JBig2_Result::kFailure;
      pTRD->SBHUFFDS = pSeg->m_HuffmanTable.get();
    }
    if (cSBHUFFDT == 0) {
      pTRD->SBHUFFDT = GetHuffmanTable(11);
    } else if (cSBHUFFDT == 1) {
      pTRD->SBHUFFDT = GetHuffmanTable(12);
    } else if (cSBHUFFDT == 2) {
      pTRD->SBHUFFDT = GetHuffmanTable(13);
    } else {
      CJBig2_Segment* pSeg =
          FindReferredTableSegmentByIndex(pSegment, nIndex++);
      if (!pSeg)
        return JBig2_Result::kFailure;
      pTRD->SBHUFFDT = pSeg->m_HuffmanTable.get();
    }
    if (cSBHUFFRDW == 0) {
      pTRD->SBHUFFRDW = GetHuffmanTable(14);
    } else if (cSBHUFFRDW == 1) {
      pTRD->SBHUFFRDW = GetHuffmanTable(15);
    } else {
      CJBig2_Segment* pSeg =
          FindReferredTableSegmentByIndex(pSegment, nIndex++);
      if (!pSeg)
        return JBig2_Result::kFailure;
      pTRD->SBHUFFRDW = pSeg->m_HuffmanTable.get();
    }
    if (cSBHUFFRDH == 0) {
      pTRD->SBHUFFRDH = GetHuffmanTable(14);
    } else if (cSBHUFFRDH == 1) {
      pTRD->SBHUFFRDH = GetHuffmanTable(15);
    } else {
      CJBig2_Segment* pSeg =
          FindReferredTableSegmentByIndex(pSegment, nIndex++);
      if (!pSeg)
        return JBig2_Result::kFailure;
      pTRD->SBHUFFRDH = pSeg->m_HuffmanTable.get();
    }
    if (cSBHUFFRDX == 0) {
      pTRD->SBHUFFRDX = GetHuffmanTable(14);
    } else if (cSBHUFFRDX == 1) {
      pTRD->SBHUFFRDX = GetHuffmanTable(15);
    } else {
      CJBig2_Segment* pSeg =
          FindReferredTableSegmentByIndex(pSegment, nIndex++);
      if (!pSeg)
        return JBig2_Result::kFailure;
      pTRD->SBHUFFRDX = pSeg->m_HuffmanTable.get();
    }
    if (cSBHUFFRDY == 0) {
      pTRD->SBHUFFRDY = GetHuffmanTable(14);
    } else if (cSBHUFFRDY == 1) {
      pTRD->SBHUFFRDY = GetHuffmanTable(15);
    } else {
      CJBig2_Segment* pSeg =
          FindReferredTableSegmentByIndex(pSegment, nIndex++);
      if (!pSeg)
        return JBig2_Result::kFailure;
      pTRD->SBHUFFRDY = pSeg->m_HuffmanTable.get();
    }
    if (cSBHUFFRSIZE == 0) {
      pTRD->SBHUFFRSIZE = GetHuffmanTable(1);
    } else {
      CJBig2_Segment* pSeg =
          FindReferredTableSegmentByIndex(pSegment, nIndex++);
      if (!pSeg)
        return JBig2_Result::kFailure;
      pTRD->SBHUFFRSIZE = pSeg->m_HuffmanTable.get();
    }
  }
  std::unique_ptr<JBig2ArithCtx, FxFreeDeleter> grContext;
  if (pTRD->SBREFINE == 1) {
    const size_t size = GetRefAggContextSize(pTRD->SBRTEMPLATE);
    grContext.reset(FX_Alloc(JBig2ArithCtx, size));
  }
  if (pTRD->SBHUFF == 0) {
    auto pArithDecoder =
        pdfium::MakeUnique<CJBig2_ArithDecoder>(m_pStream.get());
    pSegment->m_nResultType = JBIG2_IMAGE_POINTER;
    pSegment->m_Image =
        pTRD->DecodeArith(pArithDecoder.get(), grContext.get(), nullptr);
    if (!pSegment->m_Image)
      return JBig2_Result::kFailure;
    m_pStream->alignByte();
    m_pStream->offset(2);
  } else {
    pSegment->m_nResultType = JBIG2_IMAGE_POINTER;
    pSegment->m_Image = pTRD->DecodeHuffman(m_pStream.get(), grContext.get());
    if (!pSegment->m_Image)
      return JBig2_Result::kFailure;
    m_pStream->alignByte();
  }
  if (pSegment->m_cFlags.s.type != 4) {
    if (!m_bBufSpecified) {
      const auto& pPageInfo = m_PageInfoList.back();
      if ((pPageInfo->m_bIsStriped == 1) &&
          (ri.y + ri.height > m_pPage->height())) {
        m_pPage->Expand(ri.y + ri.height, (pPageInfo->m_cFlags & 4) ? 1 : 0);
      }
    }
    m_pPage->ComposeFrom(ri.x, ri.y, pSegment->m_Image.get(),
                         (JBig2ComposeOp)(ri.flags & 0x03));
    pSegment->m_Image.reset();
  }
  return JBig2_Result::kSuccess;
}

JBig2_Result CJBig2_Context::ParsePatternDict(CJBig2_Segment* pSegment,
                                              PauseIndicatorIface* pPause) {
  uint8_t cFlags;
  auto pPDD = pdfium::MakeUnique<CJBig2_PDDProc>();
  if (m_pStream->read1Byte(&cFlags) != 0 ||
      m_pStream->read1Byte(&pPDD->HDPW) != 0 ||
      m_pStream->read1Byte(&pPDD->HDPH) != 0 ||
      m_pStream->readInteger(&pPDD->GRAYMAX) != 0) {
    return JBig2_Result::kFailure;
  }
  if (pPDD->GRAYMAX > JBIG2_MAX_PATTERN_INDEX)
    return JBig2_Result::kFailure;

  pPDD->HDMMR = cFlags & 0x01;
  pPDD->HDTEMPLATE = (cFlags >> 1) & 0x03;
  pSegment->m_nResultType = JBIG2_PATTERN_DICT_POINTER;
  if (pPDD->HDMMR == 0) {
    const size_t size = GetHuffContextSize(pPDD->HDTEMPLATE);
    std::unique_ptr<JBig2ArithCtx, FxFreeDeleter> gbContext(
        FX_Alloc(JBig2ArithCtx, size));
    auto pArithDecoder =
        pdfium::MakeUnique<CJBig2_ArithDecoder>(m_pStream.get());
    pSegment->m_PatternDict =
        pPDD->DecodeArith(pArithDecoder.get(), gbContext.get(), pPause);
    if (!pSegment->m_PatternDict)
      return JBig2_Result::kFailure;

    m_pStream->alignByte();
    m_pStream->offset(2);
  } else {
    pSegment->m_PatternDict = pPDD->DecodeMMR(m_pStream.get());
    if (!pSegment->m_PatternDict)
      return JBig2_Result::kFailure;
    m_pStream->alignByte();
  }
  return JBig2_Result::kSuccess;
}

JBig2_Result CJBig2_Context::ParseHalftoneRegion(CJBig2_Segment* pSegment,
                                                 PauseIndicatorIface* pPause) {
  uint8_t cFlags;
  JBig2RegionInfo ri;
  auto pHRD = pdfium::MakeUnique<CJBig2_HTRDProc>();
  if (ParseRegionInfo(&ri) != JBig2_Result::kSuccess ||
      m_pStream->read1Byte(&cFlags) != 0 ||
      m_pStream->readInteger(&pHRD->HGW) != 0 ||
      m_pStream->readInteger(&pHRD->HGH) != 0 ||
      m_pStream->readInteger((uint32_t*)&pHRD->HGX) != 0 ||
      m_pStream->readInteger((uint32_t*)&pHRD->HGY) != 0 ||
      m_pStream->readShortInteger(&pHRD->HRX) != 0 ||
      m_pStream->readShortInteger(&pHRD->HRY) != 0) {
    return JBig2_Result::kFailure;
  }

  if (!CJBig2_Image::IsValidImageSize(pHRD->HGW, pHRD->HGH))
    return JBig2_Result::kFailure;

  if (!CJBig2_Image::IsValidImageSize(ri.width, ri.height))
    return JBig2_Result::kFailure;

  pHRD->HBW = ri.width;
  pHRD->HBH = ri.height;
  pHRD->HMMR = cFlags & 0x01;
  pHRD->HTEMPLATE = (cFlags >> 1) & 0x03;
  pHRD->HENABLESKIP = (cFlags >> 3) & 0x01;
  pHRD->HCOMBOP = (JBig2ComposeOp)((cFlags >> 4) & 0x07);
  pHRD->HDEFPIXEL = (cFlags >> 7) & 0x01;
  if (pSegment->m_nReferred_to_segment_count != 1)
    return JBig2_Result::kFailure;

  CJBig2_Segment* pSeg =
      FindSegmentByNumber(pSegment->m_Referred_to_segment_numbers[0]);
  if (!pSeg || (pSeg->m_cFlags.s.type != 16))
    return JBig2_Result::kFailure;

  const CJBig2_PatternDict* pPatternDict = pSeg->m_PatternDict.get();
  if (!pPatternDict || (pPatternDict->NUMPATS == 0))
    return JBig2_Result::kFailure;

  pHRD->HNUMPATS = pPatternDict->NUMPATS;
  pHRD->HPATS = &pPatternDict->HDPATS;
  pHRD->HPW = pPatternDict->HDPATS[0]->width();
  pHRD->HPH = pPatternDict->HDPATS[0]->height();
  pSegment->m_nResultType = JBIG2_IMAGE_POINTER;
  if (pHRD->HMMR == 0) {
    const size_t size = GetHuffContextSize(pHRD->HTEMPLATE);
    std::unique_ptr<JBig2ArithCtx, FxFreeDeleter> gbContext(
        FX_Alloc(JBig2ArithCtx, size));
    auto pArithDecoder =
        pdfium::MakeUnique<CJBig2_ArithDecoder>(m_pStream.get());
    pSegment->m_Image =
        pHRD->DecodeArith(pArithDecoder.get(), gbContext.get(), pPause);
    if (!pSegment->m_Image)
      return JBig2_Result::kFailure;

    m_pStream->alignByte();
    m_pStream->offset(2);
  } else {
    pSegment->m_Image = pHRD->DecodeMMR(m_pStream.get());
    if (!pSegment->m_Image)
      return JBig2_Result::kFailure;
    m_pStream->alignByte();
  }
  if (pSegment->m_cFlags.s.type != 20) {
    if (!m_bBufSpecified) {
      const auto& pPageInfo = m_PageInfoList.back();
      if (pPageInfo->m_bIsStriped == 1 &&
          ri.y + ri.height > m_pPage->height()) {
        m_pPage->Expand(ri.y + ri.height, (pPageInfo->m_cFlags & 4) ? 1 : 0);
      }
    }
    m_pPage->ComposeFrom(ri.x, ri.y, pSegment->m_Image.get(),
                         (JBig2ComposeOp)(ri.flags & 0x03));
    pSegment->m_Image.reset();
  }
  return JBig2_Result::kSuccess;
}

JBig2_Result CJBig2_Context::ParseGenericRegion(CJBig2_Segment* pSegment,
                                                PauseIndicatorIface* pPause) {
  if (!m_pGRD) {
    auto pGRD = pdfium::MakeUnique<CJBig2_GRDProc>();
    uint8_t cFlags;
    if (ParseRegionInfo(&m_ri) != JBig2_Result::kSuccess ||
        m_pStream->read1Byte(&cFlags) != 0) {
      return JBig2_Result::kFailure;
    }
    if (m_ri.height < 0 || m_ri.width < 0)
      return JBig2_Result::kFailure;
    pGRD->GBW = m_ri.width;
    pGRD->GBH = m_ri.height;
    pGRD->MMR = cFlags & 0x01;
    pGRD->GBTEMPLATE = (cFlags >> 1) & 0x03;
    pGRD->TPGDON = (cFlags >> 3) & 0x01;
    if (pGRD->MMR == 0) {
      if (pGRD->GBTEMPLATE == 0) {
        for (int32_t i = 0; i < 8; ++i) {
          if (m_pStream->read1Byte((uint8_t*)&pGRD->GBAT[i]) != 0)
            return JBig2_Result::kFailure;
        }
      } else {
        for (int32_t i = 0; i < 2; ++i) {
          if (m_pStream->read1Byte((uint8_t*)&pGRD->GBAT[i]) != 0)
            return JBig2_Result::kFailure;
        }
      }
    }
    pGRD->USESKIP = 0;
    m_pGRD = std::move(pGRD);
  }
  pSegment->m_nResultType = JBIG2_IMAGE_POINTER;
  if (m_pGRD->MMR == 0) {
    if (m_gbContext.empty())
      m_gbContext.resize(GetHuffContextSize(m_pGRD->GBTEMPLATE));

    bool bStart = !m_pArithDecoder;
    if (bStart) {
      m_pArithDecoder =
          pdfium::MakeUnique<CJBig2_ArithDecoder>(m_pStream.get());
    }
    {
      // |state.gbContext| can't exist when m_gbContext.clear() called below.
      CJBig2_GRDProc::ProgressiveArithDecodeState state;
      state.pImage = &pSegment->m_Image;
      state.pArithDecoder = m_pArithDecoder.get();
      state.gbContext = m_gbContext.data();
      state.pPause = pPause;
      m_ProcessingStatus = bStart ? m_pGRD->StartDecodeArith(&state)
                                  : m_pGRD->ContinueDecode(&state);
      if (m_ProcessingStatus == FXCODEC_STATUS_DECODE_TOBECONTINUE) {
        if (pSegment->m_cFlags.s.type != 36) {
          if (!m_bBufSpecified) {
            const auto& pPageInfo = m_PageInfoList.back();
            if ((pPageInfo->m_bIsStriped == 1) &&
                (m_ri.y + m_ri.height > m_pPage->height())) {
              m_pPage->Expand(m_ri.y + m_ri.height,
                              (pPageInfo->m_cFlags & 4) ? 1 : 0);
            }
          }
          const FX_RECT& rect = m_pGRD->GetReplaceRect();
          m_pPage->ComposeFromWithRect(m_ri.x + rect.left, m_ri.y + rect.top,
                                       pSegment->m_Image.get(), rect,
                                       (JBig2ComposeOp)(m_ri.flags & 0x03));
        }
        return JBig2_Result::kSuccess;
      }
    }
    m_pArithDecoder.reset();
    m_gbContext.clear();
    if (!pSegment->m_Image) {
      m_ProcessingStatus = FXCODEC_STATUS_ERROR;
      m_pGRD.reset();
      return JBig2_Result::kFailure;
    }
    m_pStream->alignByte();
    m_pStream->offset(2);
  } else {
    m_pGRD->StartDecodeMMR(&pSegment->m_Image, m_pStream.get());
    if (!pSegment->m_Image) {
      m_pGRD.reset();
      return JBig2_Result::kFailure;
    }
    m_pStream->alignByte();
  }
  if (pSegment->m_cFlags.s.type != 36) {
    if (!m_bBufSpecified) {
      JBig2PageInfo* pPageInfo = m_PageInfoList.back().get();
      if ((pPageInfo->m_bIsStriped == 1) &&
          (m_ri.y + m_ri.height > m_pPage->height())) {
        m_pPage->Expand(m_ri.y + m_ri.height,
                        (pPageInfo->m_cFlags & 4) ? 1 : 0);
      }
    }
    const FX_RECT& rect = m_pGRD->GetReplaceRect();
    m_pPage->ComposeFromWithRect(m_ri.x + rect.left, m_ri.y + rect.top,
                                 pSegment->m_Image.get(), rect,
                                 (JBig2ComposeOp)(m_ri.flags & 0x03));
    pSegment->m_Image.reset();
  }
  m_pGRD.reset();
  return JBig2_Result::kSuccess;
}

JBig2_Result CJBig2_Context::ParseGenericRefinementRegion(
    CJBig2_Segment* pSegment) {
  JBig2RegionInfo ri;
  uint8_t cFlags;
  if (ParseRegionInfo(&ri) != JBig2_Result::kSuccess ||
      m_pStream->read1Byte(&cFlags) != 0) {
    return JBig2_Result::kFailure;
  }
  if (!CJBig2_Image::IsValidImageSize(ri.width, ri.height))
    return JBig2_Result::kFailure;

  auto pGRRD = pdfium::MakeUnique<CJBig2_GRRDProc>();
  pGRRD->GRW = ri.width;
  pGRRD->GRH = ri.height;
  pGRRD->GRTEMPLATE = !!(cFlags & 0x01);
  pGRRD->TPGRON = (cFlags >> 1) & 0x01;
  if (!pGRRD->GRTEMPLATE) {
    for (int32_t i = 0; i < 4; ++i) {
      if (m_pStream->read1Byte((uint8_t*)&pGRRD->GRAT[i]) != 0)
        return JBig2_Result::kFailure;
    }
  }
  CJBig2_Segment* pSeg = nullptr;
  if (pSegment->m_nReferred_to_segment_count > 0) {
    int32_t i;
    for (i = 0; i < pSegment->m_nReferred_to_segment_count; ++i) {
      pSeg = FindSegmentByNumber(pSegment->m_Referred_to_segment_numbers[0]);
      if (!pSeg)
        return JBig2_Result::kFailure;

      if (pSeg->m_cFlags.s.type == 4 || pSeg->m_cFlags.s.type == 20 ||
          pSeg->m_cFlags.s.type == 36 || pSeg->m_cFlags.s.type == 40) {
        break;
      }
    }
    if (i >= pSegment->m_nReferred_to_segment_count)
      return JBig2_Result::kFailure;

    pGRRD->GRREFERENCE = pSeg->m_Image.get();
  } else {
    pGRRD->GRREFERENCE = m_pPage.get();
  }
  pGRRD->GRREFERENCEDX = 0;
  pGRRD->GRREFERENCEDY = 0;
  const size_t size = GetRefAggContextSize(pGRRD->GRTEMPLATE);
  std::unique_ptr<JBig2ArithCtx, FxFreeDeleter> grContext(
      FX_Alloc(JBig2ArithCtx, size));
  auto pArithDecoder = pdfium::MakeUnique<CJBig2_ArithDecoder>(m_pStream.get());
  pSegment->m_nResultType = JBIG2_IMAGE_POINTER;
  pSegment->m_Image = pGRRD->Decode(pArithDecoder.get(), grContext.get());
  if (!pSegment->m_Image)
    return JBig2_Result::kFailure;

  m_pStream->alignByte();
  m_pStream->offset(2);
  if (pSegment->m_cFlags.s.type != 40) {
    if (!m_bBufSpecified) {
      JBig2PageInfo* pPageInfo = m_PageInfoList.back().get();
      if ((pPageInfo->m_bIsStriped == 1) &&
          (ri.y + ri.height > m_pPage->height())) {
        m_pPage->Expand(ri.y + ri.height, (pPageInfo->m_cFlags & 4) ? 1 : 0);
      }
    }
    m_pPage->ComposeFrom(ri.x, ri.y, pSegment->m_Image.get(),
                         (JBig2ComposeOp)(ri.flags & 0x03));
    pSegment->m_Image.reset();
  }
  return JBig2_Result::kSuccess;
}

JBig2_Result CJBig2_Context::ParseTable(CJBig2_Segment* pSegment) {
  pSegment->m_nResultType = JBIG2_HUFFMAN_TABLE_POINTER;
  pSegment->m_HuffmanTable.reset();
  auto pHuff = pdfium::MakeUnique<CJBig2_HuffmanTable>(m_pStream.get());
  if (!pHuff->IsOK())
    return JBig2_Result::kFailure;

  pSegment->m_HuffmanTable = std::move(pHuff);
  m_pStream->alignByte();
  return JBig2_Result::kSuccess;
}

JBig2_Result CJBig2_Context::ParseRegionInfo(JBig2RegionInfo* pRI) {
  if (m_pStream->readInteger((uint32_t*)&pRI->width) != 0 ||
      m_pStream->readInteger((uint32_t*)&pRI->height) != 0 ||
      m_pStream->readInteger((uint32_t*)&pRI->x) != 0 ||
      m_pStream->readInteger((uint32_t*)&pRI->y) != 0 ||
      m_pStream->read1Byte(&pRI->flags) != 0) {
    return JBig2_Result::kFailure;
  }
  return JBig2_Result::kSuccess;
}

std::vector<JBig2HuffmanCode> CJBig2_Context::DecodeSymbolIDHuffmanTable(
    uint32_t SBNUMSYMS) {
  const size_t kRunCodesSize = 35;
  JBig2HuffmanCode huffman_codes[kRunCodesSize];
  for (size_t i = 0; i < kRunCodesSize; ++i) {
    if (m_pStream->readNBits(4, &huffman_codes[i].codelen) != 0)
      return std::vector<JBig2HuffmanCode>();
  }
  if (!HuffmanAssignCode(huffman_codes, kRunCodesSize))
    return std::vector<JBig2HuffmanCode>();

  std::vector<JBig2HuffmanCode> SBSYMCODES(SBNUMSYMS);
  int32_t run = 0;
  int32_t i = 0;
  while (i < static_cast<int>(SBNUMSYMS)) {
    size_t j;
    FX_SAFE_INT32 nSafeVal = 0;
    int32_t nBits = 0;
    uint32_t nTemp;
    while (true) {
      if (m_pStream->read1Bit(&nTemp) != 0)
        return std::vector<JBig2HuffmanCode>();

      nSafeVal <<= 1;
      if (!nSafeVal.IsValid())
        return std::vector<JBig2HuffmanCode>();

      nSafeVal |= nTemp;
      ++nBits;
      const int32_t nVal = nSafeVal.ValueOrDie();
      for (j = 0; j < kRunCodesSize; ++j) {
        if (nBits == huffman_codes[j].codelen && nVal == huffman_codes[j].code)
          break;
      }
      if (j < kRunCodesSize)
        break;
    }
    int32_t runcode = static_cast<int32_t>(j);
    if (runcode < 32) {
      SBSYMCODES[i].codelen = runcode;
      run = 0;
    } else if (runcode == 32) {
      if (m_pStream->readNBits(2, &nTemp) != 0)
        return std::vector<JBig2HuffmanCode>();
      run = nTemp + 3;
    } else if (runcode == 33) {
      if (m_pStream->readNBits(3, &nTemp) != 0)
        return std::vector<JBig2HuffmanCode>();
      run = nTemp + 3;
    } else if (runcode == 34) {
      if (m_pStream->readNBits(7, &nTemp) != 0)
        return std::vector<JBig2HuffmanCode>();
      run = nTemp + 11;
    }
    if (run > 0) {
      if (i + run > (int)SBNUMSYMS)
        return std::vector<JBig2HuffmanCode>();
      for (int32_t k = 0; k < run; ++k) {
        if (runcode == 32 && i > 0)
          SBSYMCODES[i + k].codelen = SBSYMCODES[i - 1].codelen;
        else
          SBSYMCODES[i + k].codelen = 0;
      }
      i += run;
    } else {
      ++i;
    }
  }
  if (!HuffmanAssignCode(SBSYMCODES.data(), SBNUMSYMS))
    return std::vector<JBig2HuffmanCode>();
  return SBSYMCODES;
}

const CJBig2_HuffmanTable* CJBig2_Context::GetHuffmanTable(size_t idx) {
  ASSERT(idx > 0);
  ASSERT(idx < CJBig2_HuffmanTable::kNumHuffmanTables);
  if (!m_HuffmanTables[idx].get())
    m_HuffmanTables[idx] = pdfium::MakeUnique<CJBig2_HuffmanTable>(idx);
  return m_HuffmanTables[idx].get();
}

// static
bool CJBig2_Context::HuffmanAssignCode(JBig2HuffmanCode* SBSYMCODES,
                                       uint32_t NTEMP) {
  int LENMAX = 0;
  for (uint32_t i = 0; i < NTEMP; ++i)
    LENMAX = std::max(SBSYMCODES[i].codelen, LENMAX);

  std::vector<int> LENCOUNT(LENMAX + 1);
  std::vector<int> FIRSTCODE(LENMAX + 1);
  for (uint32_t i = 0; i < NTEMP; ++i)
    ++LENCOUNT[SBSYMCODES[i].codelen];
  LENCOUNT[0] = 0;

  for (int i = 1; i <= LENMAX; ++i) {
    pdfium::base::CheckedNumeric<int> shifted = FIRSTCODE[i - 1];
    shifted += LENCOUNT[i - 1];
    shifted <<= 1;
    if (!shifted.IsValid())
      return false;

    FIRSTCODE[i] = shifted.ValueOrDie();
    int CURCODE = FIRSTCODE[i];
    for (uint32_t j = 0; j < NTEMP; ++j) {
      if (SBSYMCODES[j].codelen == i)
        SBSYMCODES[j].code = CURCODE++;
    }
  }
  return true;
}
