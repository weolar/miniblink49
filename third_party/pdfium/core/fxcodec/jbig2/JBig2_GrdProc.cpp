// Copyright 2015 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcodec/jbig2/JBig2_GrdProc.h"

#include <functional>
#include <memory>
#include <utility>

#include "core/fxcodec/codec/ccodec_faxmodule.h"
#include "core/fxcodec/fx_codec.h"
#include "core/fxcodec/jbig2/JBig2_ArithDecoder.h"
#include "core/fxcodec/jbig2/JBig2_BitStream.h"
#include "core/fxcodec/jbig2/JBig2_Image.h"
#include "core/fxcrt/pauseindicator_iface.h"
#include "third_party/base/ptr_util.h"

namespace {

// TODO(npm): Name this constants better or merge some together.
constexpr uint16_t kOptConstant1[] = {0x9b25, 0x0795, 0x00e5};
constexpr uint16_t kOptConstant2[] = {0x0006, 0x0004, 0x0001};
constexpr uint16_t kOptConstant3[] = {0xf800, 0x1e00, 0x0380};
constexpr uint16_t kOptConstant4[] = {0x0000, 0x0001, 0x0003};
constexpr uint16_t kOptConstant5[] = {0x07f0, 0x01f8, 0x007c};
constexpr uint16_t kOptConstant6[] = {0x7bf7, 0x0efb, 0x01bd};
constexpr uint16_t kOptConstant7[] = {0x0800, 0x0200, 0x0080};
constexpr uint16_t kOptConstant8[] = {0x0010, 0x0008, 0x0004};
constexpr uint16_t kOptConstant9[] = {0x000c, 0x0009, 0x0007};
constexpr uint16_t kOptConstant10[] = {0x0007, 0x000f, 0x0007};
constexpr uint16_t kOptConstant11[] = {0x001f, 0x001f, 0x000f};
constexpr uint16_t kOptConstant12[] = {0x000f, 0x0007, 0x0003};

}  // namespace

CJBig2_GRDProc::ProgressiveArithDecodeState::ProgressiveArithDecodeState() =
    default;

CJBig2_GRDProc::ProgressiveArithDecodeState::~ProgressiveArithDecodeState() =
    default;

CJBig2_GRDProc::CJBig2_GRDProc() = default;

CJBig2_GRDProc::~CJBig2_GRDProc() = default;

bool CJBig2_GRDProc::UseTemplate0Opt3() const {
  return (GBAT[0] == 3) && (GBAT[1] == -1) && (GBAT[2] == -3) &&
         (GBAT[3] == -1) && (GBAT[4] == 2) && (GBAT[5] == -2) &&
         (GBAT[6] == -2) && (GBAT[7] == -2);
}

bool CJBig2_GRDProc::UseTemplate1Opt3() const {
  return (GBAT[0] == 3) && (GBAT[1] == -1);
}

bool CJBig2_GRDProc::UseTemplate23Opt3() const {
  return (GBAT[0] == 2) && (GBAT[1] == -1);
}

std::unique_ptr<CJBig2_Image> CJBig2_GRDProc::DecodeArith(
    CJBig2_ArithDecoder* pArithDecoder,
    JBig2ArithCtx* gbContext) {
  if (!CJBig2_Image::IsValidImageSize(GBW, GBH))
    return pdfium::MakeUnique<CJBig2_Image>(GBW, GBH);

  switch (GBTEMPLATE) {
    case 0:
      return UseTemplate0Opt3()
                 ? DecodeArithOpt3(pArithDecoder, gbContext, 0)
                 : DecodeArithTemplateUnopt(pArithDecoder, gbContext, 0);
    case 1:
      return UseTemplate1Opt3()
                 ? DecodeArithOpt3(pArithDecoder, gbContext, 1)
                 : DecodeArithTemplateUnopt(pArithDecoder, gbContext, 1);
    case 2:
      return UseTemplate23Opt3()
                 ? DecodeArithOpt3(pArithDecoder, gbContext, 2)
                 : DecodeArithTemplateUnopt(pArithDecoder, gbContext, 2);
    default:
      return UseTemplate23Opt3()
                 ? DecodeArithTemplate3Opt3(pArithDecoder, gbContext)
                 : DecodeArithTemplate3Unopt(pArithDecoder, gbContext);
  }
}

std::unique_ptr<CJBig2_Image> CJBig2_GRDProc::DecodeArithOpt3(
    CJBig2_ArithDecoder* pArithDecoder,
    JBig2ArithCtx* gbContext,
    int OPT) {
  auto GBREG = pdfium::MakeUnique<CJBig2_Image>(GBW, GBH);
  if (!GBREG->data())
    return nullptr;

  int LTP = 0;
  uint8_t* pLine = GBREG->data();
  int32_t nStride = GBREG->stride();
  int32_t nStride2 = nStride << 1;
  int32_t nLineBytes = ((GBW + 7) >> 3) - 1;
  int32_t nBitsLeft = GBW - (nLineBytes << 3);
  // TODO(npm): Why is the height only trimmed when OPT is 0?
  uint32_t height = OPT == 0 ? GBH & 0x7fffffff : GBH;
  for (uint32_t h = 0; h < height; ++h) {
    if (TPGDON) {
      if (pArithDecoder->IsComplete())
        return nullptr;

      LTP = LTP ^ pArithDecoder->Decode(&gbContext[kOptConstant1[OPT]]);
    }
    if (LTP) {
      GBREG->CopyLine(h, h - 1);
    } else {
      if (h > 1) {
        uint8_t* pLine1 = pLine - nStride2;
        uint8_t* pLine2 = pLine - nStride;
        uint32_t line1 = (*pLine1++) << kOptConstant2[OPT];
        uint32_t line2 = *pLine2++;
        uint32_t CONTEXT = (line1 & kOptConstant3[OPT]) |
                           ((line2 >> kOptConstant4[OPT]) & kOptConstant5[OPT]);
        for (int32_t cc = 0; cc < nLineBytes; ++cc) {
          line1 = (line1 << 8) | ((*pLine1++) << kOptConstant2[OPT]);
          line2 = (line2 << 8) | (*pLine2++);
          uint8_t cVal = 0;
          for (int32_t k = 7; k >= 0; --k) {
            if (pArithDecoder->IsComplete())
              return nullptr;

            int bVal = pArithDecoder->Decode(&gbContext[CONTEXT]);
            cVal |= bVal << k;
            CONTEXT =
                (((CONTEXT & kOptConstant6[OPT]) << 1) | bVal |
                 ((line1 >> k) & kOptConstant7[OPT]) |
                 ((line2 >> (k + kOptConstant4[OPT])) & kOptConstant8[OPT]));
          }
          pLine[cc] = cVal;
        }
        line1 <<= 8;
        line2 <<= 8;
        uint8_t cVal1 = 0;
        for (int32_t k = 0; k < nBitsLeft; ++k) {
          if (pArithDecoder->IsComplete())
            return nullptr;

          int bVal = pArithDecoder->Decode(&gbContext[CONTEXT]);
          cVal1 |= bVal << (7 - k);
          CONTEXT =
              (((CONTEXT & kOptConstant6[OPT]) << 1) | bVal |
               ((line1 >> (7 - k)) & kOptConstant7[OPT]) |
               ((line2 >> (7 + kOptConstant4[OPT] - k)) & kOptConstant8[OPT]));
        }
        pLine[nLineBytes] = cVal1;
      } else {
        uint8_t* pLine2 = pLine - nStride;
        uint32_t line2 = (h & 1) ? (*pLine2++) : 0;
        uint32_t CONTEXT = ((line2 >> kOptConstant4[OPT]) & kOptConstant5[OPT]);
        for (int32_t cc = 0; cc < nLineBytes; ++cc) {
          if (h & 1) {
            line2 = (line2 << 8) | (*pLine2++);
          }
          uint8_t cVal = 0;
          for (int32_t k = 7; k >= 0; --k) {
            if (pArithDecoder->IsComplete())
              return nullptr;

            int bVal = pArithDecoder->Decode(&gbContext[CONTEXT]);
            cVal |= bVal << k;
            CONTEXT =
                (((CONTEXT & kOptConstant6[OPT]) << 1) | bVal |
                 ((line2 >> (k + kOptConstant4[OPT])) & kOptConstant8[OPT]));
          }
          pLine[cc] = cVal;
        }
        line2 <<= 8;
        uint8_t cVal1 = 0;
        for (int32_t k = 0; k < nBitsLeft; ++k) {
          if (pArithDecoder->IsComplete())
            return nullptr;

          int bVal = pArithDecoder->Decode(&gbContext[CONTEXT]);
          cVal1 |= bVal << (7 - k);
          CONTEXT = (((CONTEXT & kOptConstant6[OPT]) << 1) | bVal |
                     (((line2 >> (7 + kOptConstant4[OPT] - k))) &
                      kOptConstant8[OPT]));
        }
        pLine[nLineBytes] = cVal1;
      }
    }
    pLine += nStride;
  }
  return GBREG;
}

std::unique_ptr<CJBig2_Image> CJBig2_GRDProc::DecodeArithTemplateUnopt(
    CJBig2_ArithDecoder* pArithDecoder,
    JBig2ArithCtx* gbContext,
    int UNOPT) {
  auto GBREG = pdfium::MakeUnique<CJBig2_Image>(GBW, GBH);
  if (!GBREG->data())
    return nullptr;

  GBREG->Fill(0);
  int LTP = 0;
  uint8_t MOD2 = UNOPT % 2;
  uint8_t DIV2 = UNOPT / 2;
  uint8_t SHIFT = 4 - UNOPT;
  for (uint32_t h = 0; h < GBH; h++) {
    if (TPGDON) {
      if (pArithDecoder->IsComplete())
        return nullptr;

      LTP = LTP ^ pArithDecoder->Decode(&gbContext[kOptConstant1[UNOPT]]);
    }
    if (LTP) {
      GBREG->CopyLine(h, h - 1);
      continue;
    }
    uint32_t line1 = GBREG->GetPixel(1 + MOD2, h - 2);
    line1 |= GBREG->GetPixel(MOD2, h - 2) << 1;
    if (UNOPT == 1)
      line1 |= GBREG->GetPixel(0, h - 2) << 2;
    uint32_t line2 = GBREG->GetPixel(2 - DIV2, h - 1);
    line2 |= GBREG->GetPixel(1 - DIV2, h - 1) << 1;
    if (UNOPT < 2)
      line2 |= GBREG->GetPixel(0, h - 1) << 2;
    uint32_t line3 = 0;
    for (uint32_t w = 0; w < GBW; w++) {
      int bVal = 0;
      if (!USESKIP || !SKIP->GetPixel(w, h)) {
        if (pArithDecoder->IsComplete())
          return nullptr;

        uint32_t CONTEXT = line3;
        CONTEXT |= GBREG->GetPixel(w + GBAT[0], h + GBAT[1]) << SHIFT;
        CONTEXT |= line2 << (SHIFT + 1);
        CONTEXT |= line1 << kOptConstant9[UNOPT];
        if (UNOPT == 0) {
          CONTEXT |= GBREG->GetPixel(w + GBAT[2], h + GBAT[3]) << 10;
          CONTEXT |= GBREG->GetPixel(w + GBAT[4], h + GBAT[5]) << 11;
          CONTEXT |= GBREG->GetPixel(w + GBAT[6], h + GBAT[7]) << 15;
        }
        bVal = pArithDecoder->Decode(&gbContext[CONTEXT]);
        if (bVal)
          GBREG->SetPixel(w, h, bVal);
      }
      line1 = ((line1 << 1) | GBREG->GetPixel(w + 2 + MOD2, h - 2)) &
              kOptConstant10[UNOPT];
      line2 = ((line2 << 1) | GBREG->GetPixel(w + 3 - DIV2, h - 1)) &
              kOptConstant11[UNOPT];
      line3 = ((line3 << 1) | bVal) & kOptConstant12[UNOPT];
    }
  }
  return GBREG;
}

std::unique_ptr<CJBig2_Image> CJBig2_GRDProc::DecodeArithTemplate3Opt3(
    CJBig2_ArithDecoder* pArithDecoder,
    JBig2ArithCtx* gbContext) {
  auto GBREG = pdfium::MakeUnique<CJBig2_Image>(GBW, GBH);
  if (!GBREG->data())
    return nullptr;

  int LTP = 0;
  uint8_t* pLine = GBREG->data();
  int32_t nStride = GBREG->stride();
  int32_t nLineBytes = ((GBW + 7) >> 3) - 1;
  int32_t nBitsLeft = GBW - (nLineBytes << 3);

  for (uint32_t h = 0; h < GBH; h++) {
    if (TPGDON) {
      if (pArithDecoder->IsComplete())
        return nullptr;

      LTP = LTP ^ pArithDecoder->Decode(&gbContext[0x0195]);
    }

    if (LTP) {
      GBREG->CopyLine(h, h - 1);
    } else {
      if (h > 0) {
        uint8_t* pLine1 = pLine - nStride;
        uint32_t line1 = *pLine1++;
        uint32_t CONTEXT = (line1 >> 1) & 0x03f0;
        for (int32_t cc = 0; cc < nLineBytes; cc++) {
          line1 = (line1 << 8) | (*pLine1++);
          uint8_t cVal = 0;
          for (int32_t k = 7; k >= 0; k--) {
            if (pArithDecoder->IsComplete())
              return nullptr;

            int bVal = pArithDecoder->Decode(&gbContext[CONTEXT]);
            cVal |= bVal << k;
            CONTEXT = ((CONTEXT & 0x01f7) << 1) | bVal |
                      ((line1 >> (k + 1)) & 0x0010);
          }
          pLine[cc] = cVal;
        }
        line1 <<= 8;
        uint8_t cVal1 = 0;
        for (int32_t k = 0; k < nBitsLeft; k++) {
          if (pArithDecoder->IsComplete())
            return nullptr;

          int bVal = pArithDecoder->Decode(&gbContext[CONTEXT]);
          cVal1 |= bVal << (7 - k);
          CONTEXT =
              ((CONTEXT & 0x01f7) << 1) | bVal | ((line1 >> (8 - k)) & 0x0010);
        }
        pLine[nLineBytes] = cVal1;
      } else {
        uint32_t CONTEXT = 0;
        for (int32_t cc = 0; cc < nLineBytes; cc++) {
          uint8_t cVal = 0;
          for (int32_t k = 7; k >= 0; k--) {
            if (pArithDecoder->IsComplete())
              return nullptr;

            int bVal = pArithDecoder->Decode(&gbContext[CONTEXT]);
            cVal |= bVal << k;
            CONTEXT = ((CONTEXT & 0x01f7) << 1) | bVal;
          }
          pLine[cc] = cVal;
        }
        uint8_t cVal1 = 0;
        for (int32_t k = 0; k < nBitsLeft; k++) {
          if (pArithDecoder->IsComplete())
            return nullptr;

          int bVal = pArithDecoder->Decode(&gbContext[CONTEXT]);
          cVal1 |= bVal << (7 - k);
          CONTEXT = ((CONTEXT & 0x01f7) << 1) | bVal;
        }
        pLine[nLineBytes] = cVal1;
      }
    }
    pLine += nStride;
  }
  return GBREG;
}

std::unique_ptr<CJBig2_Image> CJBig2_GRDProc::DecodeArithTemplate3Unopt(
    CJBig2_ArithDecoder* pArithDecoder,
    JBig2ArithCtx* gbContext) {
  auto GBREG = pdfium::MakeUnique<CJBig2_Image>(GBW, GBH);
  if (!GBREG->data())
    return nullptr;

  GBREG->Fill(0);
  int LTP = 0;
  for (uint32_t h = 0; h < GBH; h++) {
    if (TPGDON) {
      if (pArithDecoder->IsComplete())
        return nullptr;

      LTP = LTP ^ pArithDecoder->Decode(&gbContext[0x0195]);
    }
    if (LTP == 1) {
      GBREG->CopyLine(h, h - 1);
    } else {
      uint32_t line1 = GBREG->GetPixel(1, h - 1);
      line1 |= GBREG->GetPixel(0, h - 1) << 1;
      uint32_t line2 = 0;
      for (uint32_t w = 0; w < GBW; w++) {
        int bVal;
        if (USESKIP && SKIP->GetPixel(w, h)) {
          bVal = 0;
        } else {
          uint32_t CONTEXT = line2;
          CONTEXT |= GBREG->GetPixel(w + GBAT[0], h + GBAT[1]) << 4;
          CONTEXT |= line1 << 5;
          if (pArithDecoder->IsComplete())
            return nullptr;

          bVal = pArithDecoder->Decode(&gbContext[CONTEXT]);
        }
        if (bVal) {
          GBREG->SetPixel(w, h, bVal);
        }
        line1 = ((line1 << 1) | GBREG->GetPixel(w + 2, h - 1)) & 0x1f;
        line2 = ((line2 << 1) | bVal) & 0x0f;
      }
    }
  }
  return GBREG;
}

FXCODEC_STATUS CJBig2_GRDProc::StartDecodeArith(
    ProgressiveArithDecodeState* pState) {
  if (!CJBig2_Image::IsValidImageSize(GBW, GBH)) {
    m_ProssiveStatus = FXCODEC_STATUS_DECODE_FINISH;
    return FXCODEC_STATUS_DECODE_FINISH;
  }
  m_ProssiveStatus = FXCODEC_STATUS_DECODE_READY;
  std::unique_ptr<CJBig2_Image>* pImage = pState->pImage;
  if (!*pImage)
    *pImage = pdfium::MakeUnique<CJBig2_Image>(GBW, GBH);
  if (!(*pImage)->data()) {
    *pImage = nullptr;
    m_ProssiveStatus = FXCODEC_STATUS_ERROR;
    return FXCODEC_STATUS_ERROR;
  }
  pImage->get()->Fill(0);
  m_DecodeType = 1;
  m_LTP = 0;
  m_pLine = nullptr;
  m_loopIndex = 0;
  return ProgressiveDecodeArith(pState);
}

FXCODEC_STATUS CJBig2_GRDProc::ProgressiveDecodeArith(
    ProgressiveArithDecodeState* pState) {
  int iline = m_loopIndex;

  using DecodeFunction = std::function<FXCODEC_STATUS(
      CJBig2_GRDProc&, ProgressiveArithDecodeState*)>;
  DecodeFunction func;
  switch (GBTEMPLATE) {
    case 0:
      func = UseTemplate0Opt3()
                 ? &CJBig2_GRDProc::ProgressiveDecodeArithTemplate0Opt3
                 : &CJBig2_GRDProc::ProgressiveDecodeArithTemplate0Unopt;
      break;
    case 1:
      func = UseTemplate1Opt3()
                 ? &CJBig2_GRDProc::ProgressiveDecodeArithTemplate1Opt3
                 : &CJBig2_GRDProc::ProgressiveDecodeArithTemplate1Unopt;
      break;
    case 2:
      func = UseTemplate23Opt3()
                 ? &CJBig2_GRDProc::ProgressiveDecodeArithTemplate2Opt3
                 : &CJBig2_GRDProc::ProgressiveDecodeArithTemplate2Unopt;
      break;
    default:
      func = UseTemplate23Opt3()
                 ? &CJBig2_GRDProc::ProgressiveDecodeArithTemplate3Opt3
                 : &CJBig2_GRDProc::ProgressiveDecodeArithTemplate3Unopt;
      break;
  }
  CJBig2_Image* pImage = pState->pImage->get();
  m_ProssiveStatus = func(*this, pState);
  m_ReplaceRect.left = 0;
  m_ReplaceRect.right = pImage->width();
  m_ReplaceRect.top = iline;
  m_ReplaceRect.bottom = m_loopIndex;
  if (m_ProssiveStatus == FXCODEC_STATUS_DECODE_FINISH)
    m_loopIndex = 0;

  return m_ProssiveStatus;
}

FXCODEC_STATUS CJBig2_GRDProc::StartDecodeMMR(
    std::unique_ptr<CJBig2_Image>* pImage,
    CJBig2_BitStream* pStream) {
  auto image = pdfium::MakeUnique<CJBig2_Image>(GBW, GBH);
  if (!image->data()) {
    *pImage = nullptr;
    m_ProssiveStatus = FXCODEC_STATUS_ERROR;
    return m_ProssiveStatus;
  }
  int bitpos = static_cast<int>(pStream->getBitPos());
  bitpos = CCodec_FaxModule::FaxG4Decode(pStream->getBuf(),
                                         pStream->getLength(), bitpos, GBW, GBH,
                                         image->stride(), image->data());
  pStream->setBitPos(bitpos);
  for (uint32_t i = 0; i < image->stride() * GBH; ++i)
    image->data()[i] = ~image->data()[i];
  m_ProssiveStatus = FXCODEC_STATUS_DECODE_FINISH;
  *pImage = std::move(image);
  return m_ProssiveStatus;
}

FXCODEC_STATUS CJBig2_GRDProc::ContinueDecode(
    ProgressiveArithDecodeState* pState) {
  if (m_ProssiveStatus != FXCODEC_STATUS_DECODE_TOBECONTINUE)
    return m_ProssiveStatus;

  if (m_DecodeType != 1) {
    m_ProssiveStatus = FXCODEC_STATUS_ERROR;
    return m_ProssiveStatus;
  }
  return ProgressiveDecodeArith(pState);
}

FXCODEC_STATUS CJBig2_GRDProc::ProgressiveDecodeArithTemplate0Opt3(
    ProgressiveArithDecodeState* pState) {
  CJBig2_Image* pImage = pState->pImage->get();
  JBig2ArithCtx* gbContext = pState->gbContext.Get();
  CJBig2_ArithDecoder* pArithDecoder = pState->pArithDecoder.Get();
  if (!m_pLine)
    m_pLine = pImage->data();
  int32_t nStride = pImage->stride();
  int32_t nStride2 = nStride << 1;
  int32_t nLineBytes = ((GBW + 7) >> 3) - 1;
  int32_t nBitsLeft = GBW - (nLineBytes << 3);
  uint32_t height = GBH & 0x7fffffff;

  for (; m_loopIndex < height; m_loopIndex++) {
    if (TPGDON) {
      if (pArithDecoder->IsComplete())
        return FXCODEC_STATUS_ERROR;

      m_LTP = m_LTP ^ pArithDecoder->Decode(&gbContext[0x9b25]);
    }
    if (m_LTP) {
      pImage->CopyLine(m_loopIndex, m_loopIndex - 1);
    } else {
      if (m_loopIndex > 1) {
        uint8_t* pLine1 = m_pLine - nStride2;
        uint8_t* pLine2 = m_pLine - nStride;
        uint32_t line1 = (*pLine1++) << 6;
        uint32_t line2 = *pLine2++;
        uint32_t CONTEXT = ((line1 & 0xf800) | (line2 & 0x07f0));
        for (int32_t cc = 0; cc < nLineBytes; cc++) {
          line1 = (line1 << 8) | ((*pLine1++) << 6);
          line2 = (line2 << 8) | (*pLine2++);
          uint8_t cVal = 0;
          for (int32_t k = 7; k >= 0; k--) {
            if (pArithDecoder->IsComplete())
              return FXCODEC_STATUS_ERROR;

            int bVal = pArithDecoder->Decode(&gbContext[CONTEXT]);
            cVal |= bVal << k;
            CONTEXT = (((CONTEXT & 0x7bf7) << 1) | bVal |
                       ((line1 >> k) & 0x0800) | ((line2 >> k) & 0x0010));
          }
          m_pLine[cc] = cVal;
        }
        line1 <<= 8;
        line2 <<= 8;
        uint8_t cVal1 = 0;
        for (int32_t k = 0; k < nBitsLeft; k++) {
          if (pArithDecoder->IsComplete())
            return FXCODEC_STATUS_ERROR;

          int bVal = pArithDecoder->Decode(&gbContext[CONTEXT]);
          cVal1 |= bVal << (7 - k);
          CONTEXT =
              (((CONTEXT & 0x7bf7) << 1) | bVal |
               ((line1 >> (7 - k)) & 0x0800) | ((line2 >> (7 - k)) & 0x0010));
        }
        m_pLine[nLineBytes] = cVal1;
      } else {
        uint8_t* pLine2 = m_pLine - nStride;
        uint32_t line2 = (m_loopIndex & 1) ? (*pLine2++) : 0;
        uint32_t CONTEXT = (line2 & 0x07f0);
        for (int32_t cc = 0; cc < nLineBytes; cc++) {
          if (m_loopIndex & 1) {
            line2 = (line2 << 8) | (*pLine2++);
          }
          uint8_t cVal = 0;
          for (int32_t k = 7; k >= 0; k--) {
            if (pArithDecoder->IsComplete())
              return FXCODEC_STATUS_ERROR;

            int bVal = pArithDecoder->Decode(&gbContext[CONTEXT]);
            cVal |= bVal << k;
            CONTEXT =
                (((CONTEXT & 0x7bf7) << 1) | bVal | ((line2 >> k) & 0x0010));
          }
          m_pLine[cc] = cVal;
        }
        line2 <<= 8;
        uint8_t cVal1 = 0;
        for (int32_t k = 0; k < nBitsLeft; k++) {
          if (pArithDecoder->IsComplete())
            return FXCODEC_STATUS_ERROR;

          int bVal = pArithDecoder->Decode(&gbContext[CONTEXT]);
          cVal1 |= bVal << (7 - k);
          CONTEXT = (((CONTEXT & 0x7bf7) << 1) | bVal |
                     ((line2 >> (7 - k)) & 0x0010));
        }
        m_pLine[nLineBytes] = cVal1;
      }
    }
    m_pLine += nStride;
    if (pState->pPause && pState->pPause->NeedToPauseNow()) {
      m_loopIndex++;
      m_ProssiveStatus = FXCODEC_STATUS_DECODE_TOBECONTINUE;
      return FXCODEC_STATUS_DECODE_TOBECONTINUE;
    }
  }
  m_ProssiveStatus = FXCODEC_STATUS_DECODE_FINISH;
  return FXCODEC_STATUS_DECODE_FINISH;
}

FXCODEC_STATUS CJBig2_GRDProc::ProgressiveDecodeArithTemplate0Unopt(
    ProgressiveArithDecodeState* pState) {
  CJBig2_Image* pImage = pState->pImage->get();
  JBig2ArithCtx* gbContext = pState->gbContext.Get();
  CJBig2_ArithDecoder* pArithDecoder = pState->pArithDecoder.Get();
  for (; m_loopIndex < GBH; m_loopIndex++) {
    if (TPGDON) {
      if (pArithDecoder->IsComplete())
        return FXCODEC_STATUS_ERROR;

      m_LTP = m_LTP ^ pArithDecoder->Decode(&gbContext[0x9b25]);
    }
    if (m_LTP) {
      pImage->CopyLine(m_loopIndex, m_loopIndex - 1);
    } else {
      uint32_t line1 = pImage->GetPixel(1, m_loopIndex - 2);
      line1 |= pImage->GetPixel(0, m_loopIndex - 2) << 1;
      uint32_t line2 = pImage->GetPixel(2, m_loopIndex - 1);
      line2 |= pImage->GetPixel(1, m_loopIndex - 1) << 1;
      line2 |= pImage->GetPixel(0, m_loopIndex - 1) << 2;
      uint32_t line3 = 0;
      for (uint32_t w = 0; w < GBW; w++) {
        int bVal;
        if (USESKIP && SKIP->GetPixel(w, m_loopIndex)) {
          bVal = 0;
        } else {
          uint32_t CONTEXT = line3;
          CONTEXT |= pImage->GetPixel(w + GBAT[0], m_loopIndex + GBAT[1]) << 4;
          CONTEXT |= line2 << 5;
          CONTEXT |= pImage->GetPixel(w + GBAT[2], m_loopIndex + GBAT[3]) << 10;
          CONTEXT |= pImage->GetPixel(w + GBAT[4], m_loopIndex + GBAT[5]) << 11;
          CONTEXT |= line1 << 12;
          CONTEXT |= pImage->GetPixel(w + GBAT[6], m_loopIndex + GBAT[7]) << 15;
          if (pArithDecoder->IsComplete())
            return FXCODEC_STATUS_ERROR;

          bVal = pArithDecoder->Decode(&gbContext[CONTEXT]);
        }
        if (bVal) {
          pImage->SetPixel(w, m_loopIndex, bVal);
        }
        line1 =
            ((line1 << 1) | pImage->GetPixel(w + 2, m_loopIndex - 2)) & 0x07;
        line2 =
            ((line2 << 1) | pImage->GetPixel(w + 3, m_loopIndex - 1)) & 0x1f;
        line3 = ((line3 << 1) | bVal) & 0x0f;
      }
    }
    if (pState->pPause && pState->pPause->NeedToPauseNow()) {
      m_loopIndex++;
      m_ProssiveStatus = FXCODEC_STATUS_DECODE_TOBECONTINUE;
      return FXCODEC_STATUS_DECODE_TOBECONTINUE;
    }
  }
  m_ProssiveStatus = FXCODEC_STATUS_DECODE_FINISH;
  return FXCODEC_STATUS_DECODE_FINISH;
}

FXCODEC_STATUS CJBig2_GRDProc::ProgressiveDecodeArithTemplate1Opt3(
    ProgressiveArithDecodeState* pState) {
  CJBig2_Image* pImage = pState->pImage->get();
  JBig2ArithCtx* gbContext = pState->gbContext.Get();
  CJBig2_ArithDecoder* pArithDecoder = pState->pArithDecoder.Get();
  if (!m_pLine)
    m_pLine = pImage->data();
  int32_t nStride = pImage->stride();
  int32_t nStride2 = nStride << 1;
  int32_t nLineBytes = ((GBW + 7) >> 3) - 1;
  int32_t nBitsLeft = GBW - (nLineBytes << 3);
  for (; m_loopIndex < GBH; m_loopIndex++) {
    if (TPGDON) {
      if (pArithDecoder->IsComplete())
        return FXCODEC_STATUS_ERROR;

      m_LTP = m_LTP ^ pArithDecoder->Decode(&gbContext[0x0795]);
    }
    if (m_LTP) {
      pImage->CopyLine(m_loopIndex, m_loopIndex - 1);
    } else {
      if (m_loopIndex > 1) {
        uint8_t* pLine1 = m_pLine - nStride2;
        uint8_t* pLine2 = m_pLine - nStride;
        uint32_t line1 = (*pLine1++) << 4;
        uint32_t line2 = *pLine2++;
        uint32_t CONTEXT = (line1 & 0x1e00) | ((line2 >> 1) & 0x01f8);
        for (int32_t cc = 0; cc < nLineBytes; cc++) {
          line1 = (line1 << 8) | ((*pLine1++) << 4);
          line2 = (line2 << 8) | (*pLine2++);
          uint8_t cVal = 0;
          for (int32_t k = 7; k >= 0; k--) {
            if (pArithDecoder->IsComplete())
              return FXCODEC_STATUS_ERROR;

            int bVal = pArithDecoder->Decode(&gbContext[CONTEXT]);
            cVal |= bVal << k;
            CONTEXT = ((CONTEXT & 0x0efb) << 1) | bVal |
                      ((line1 >> k) & 0x0200) | ((line2 >> (k + 1)) & 0x0008);
          }
          m_pLine[cc] = cVal;
        }
        line1 <<= 8;
        line2 <<= 8;
        uint8_t cVal1 = 0;
        for (int32_t k = 0; k < nBitsLeft; k++) {
          if (pArithDecoder->IsComplete())
            return FXCODEC_STATUS_ERROR;

          int bVal = pArithDecoder->Decode(&gbContext[CONTEXT]);
          cVal1 |= bVal << (7 - k);
          CONTEXT = ((CONTEXT & 0x0efb) << 1) | bVal |
                    ((line1 >> (7 - k)) & 0x0200) |
                    ((line2 >> (8 - k)) & 0x0008);
        }
        m_pLine[nLineBytes] = cVal1;
      } else {
        uint8_t* pLine2 = m_pLine - nStride;
        uint32_t line2 = (m_loopIndex & 1) ? (*pLine2++) : 0;
        uint32_t CONTEXT = (line2 >> 1) & 0x01f8;
        for (int32_t cc = 0; cc < nLineBytes; cc++) {
          if (m_loopIndex & 1) {
            line2 = (line2 << 8) | (*pLine2++);
          }
          uint8_t cVal = 0;
          for (int32_t k = 7; k >= 0; k--) {
            if (pArithDecoder->IsComplete())
              return FXCODEC_STATUS_ERROR;

            int bVal = pArithDecoder->Decode(&gbContext[CONTEXT]);
            cVal |= bVal << k;
            CONTEXT = ((CONTEXT & 0x0efb) << 1) | bVal |
                      ((line2 >> (k + 1)) & 0x0008);
          }
          m_pLine[cc] = cVal;
        }
        line2 <<= 8;
        uint8_t cVal1 = 0;
        for (int32_t k = 0; k < nBitsLeft; k++) {
          if (pArithDecoder->IsComplete())
            return FXCODEC_STATUS_ERROR;

          int bVal = pArithDecoder->Decode(&gbContext[CONTEXT]);
          cVal1 |= bVal << (7 - k);
          CONTEXT =
              ((CONTEXT & 0x0efb) << 1) | bVal | ((line2 >> (8 - k)) & 0x0008);
        }
        m_pLine[nLineBytes] = cVal1;
      }
    }
    m_pLine += nStride;
    if (pState->pPause && pState->pPause->NeedToPauseNow()) {
      m_loopIndex++;
      m_ProssiveStatus = FXCODEC_STATUS_DECODE_TOBECONTINUE;
      return FXCODEC_STATUS_DECODE_TOBECONTINUE;
    }
  }
  m_ProssiveStatus = FXCODEC_STATUS_DECODE_FINISH;
  return FXCODEC_STATUS_DECODE_FINISH;
}

FXCODEC_STATUS CJBig2_GRDProc::ProgressiveDecodeArithTemplate1Unopt(
    ProgressiveArithDecodeState* pState) {
  CJBig2_Image* pImage = pState->pImage->get();
  JBig2ArithCtx* gbContext = pState->gbContext.Get();
  CJBig2_ArithDecoder* pArithDecoder = pState->pArithDecoder.Get();
  for (uint32_t h = 0; h < GBH; h++) {
    if (TPGDON) {
      if (pArithDecoder->IsComplete())
        return FXCODEC_STATUS_ERROR;

      m_LTP = m_LTP ^ pArithDecoder->Decode(&gbContext[0x0795]);
    }
    if (m_LTP) {
      pImage->CopyLine(h, h - 1);
    } else {
      uint32_t line1 = pImage->GetPixel(2, h - 2);
      line1 |= pImage->GetPixel(1, h - 2) << 1;
      line1 |= pImage->GetPixel(0, h - 2) << 2;
      uint32_t line2 = pImage->GetPixel(2, h - 1);
      line2 |= pImage->GetPixel(1, h - 1) << 1;
      line2 |= pImage->GetPixel(0, h - 1) << 2;
      uint32_t line3 = 0;
      for (uint32_t w = 0; w < GBW; w++) {
        int bVal;
        if (USESKIP && SKIP->GetPixel(w, h)) {
          bVal = 0;
        } else {
          uint32_t CONTEXT = line3;
          CONTEXT |= pImage->GetPixel(w + GBAT[0], h + GBAT[1]) << 3;
          CONTEXT |= line2 << 4;
          CONTEXT |= line1 << 9;
          if (pArithDecoder->IsComplete())
            return FXCODEC_STATUS_ERROR;

          bVal = pArithDecoder->Decode(&gbContext[CONTEXT]);
        }
        if (bVal) {
          pImage->SetPixel(w, h, bVal);
        }
        line1 = ((line1 << 1) | pImage->GetPixel(w + 3, h - 2)) & 0x0f;
        line2 = ((line2 << 1) | pImage->GetPixel(w + 3, h - 1)) & 0x1f;
        line3 = ((line3 << 1) | bVal) & 0x07;
      }
    }
    if (pState->pPause && pState->pPause->NeedToPauseNow()) {
      m_loopIndex++;
      m_ProssiveStatus = FXCODEC_STATUS_DECODE_TOBECONTINUE;
      return FXCODEC_STATUS_DECODE_TOBECONTINUE;
    }
  }
  m_ProssiveStatus = FXCODEC_STATUS_DECODE_FINISH;
  return FXCODEC_STATUS_DECODE_FINISH;
}

FXCODEC_STATUS CJBig2_GRDProc::ProgressiveDecodeArithTemplate2Opt3(
    ProgressiveArithDecodeState* pState) {
  CJBig2_Image* pImage = pState->pImage->get();
  JBig2ArithCtx* gbContext = pState->gbContext.Get();
  CJBig2_ArithDecoder* pArithDecoder = pState->pArithDecoder.Get();
  if (!m_pLine)
    m_pLine = pImage->data();
  int32_t nStride = pImage->stride();
  int32_t nStride2 = nStride << 1;
  int32_t nLineBytes = ((GBW + 7) >> 3) - 1;
  int32_t nBitsLeft = GBW - (nLineBytes << 3);
  for (; m_loopIndex < GBH; m_loopIndex++) {
    if (TPGDON) {
      if (pArithDecoder->IsComplete())
        return FXCODEC_STATUS_ERROR;

      m_LTP = m_LTP ^ pArithDecoder->Decode(&gbContext[0x00e5]);
    }
    if (m_LTP) {
      pImage->CopyLine(m_loopIndex, m_loopIndex - 1);
    } else {
      if (m_loopIndex > 1) {
        uint8_t* pLine1 = m_pLine - nStride2;
        uint8_t* pLine2 = m_pLine - nStride;
        uint32_t line1 = (*pLine1++) << 1;
        uint32_t line2 = *pLine2++;
        uint32_t CONTEXT = (line1 & 0x0380) | ((line2 >> 3) & 0x007c);
        for (int32_t cc = 0; cc < nLineBytes; cc++) {
          line1 = (line1 << 8) | ((*pLine1++) << 1);
          line2 = (line2 << 8) | (*pLine2++);
          uint8_t cVal = 0;
          for (int32_t k = 7; k >= 0; k--) {
            if (pArithDecoder->IsComplete())
              return FXCODEC_STATUS_ERROR;

            int bVal = pArithDecoder->Decode(&gbContext[CONTEXT]);
            cVal |= bVal << k;
            CONTEXT = ((CONTEXT & 0x01bd) << 1) | bVal |
                      ((line1 >> k) & 0x0080) | ((line2 >> (k + 3)) & 0x0004);
          }
          m_pLine[cc] = cVal;
        }
        line1 <<= 8;
        line2 <<= 8;
        uint8_t cVal1 = 0;
        for (int32_t k = 0; k < nBitsLeft; k++) {
          if (pArithDecoder->IsComplete())
            return FXCODEC_STATUS_ERROR;

          int bVal = pArithDecoder->Decode(&gbContext[CONTEXT]);
          cVal1 |= bVal << (7 - k);
          CONTEXT = ((CONTEXT & 0x01bd) << 1) | bVal |
                    ((line1 >> (7 - k)) & 0x0080) |
                    ((line2 >> (10 - k)) & 0x0004);
        }
        m_pLine[nLineBytes] = cVal1;
      } else {
        uint8_t* pLine2 = m_pLine - nStride;
        uint32_t line2 = (m_loopIndex & 1) ? (*pLine2++) : 0;
        uint32_t CONTEXT = (line2 >> 3) & 0x007c;
        for (int32_t cc = 0; cc < nLineBytes; cc++) {
          if (m_loopIndex & 1) {
            line2 = (line2 << 8) | (*pLine2++);
          }
          uint8_t cVal = 0;
          for (int32_t k = 7; k >= 0; k--) {
            if (pArithDecoder->IsComplete())
              return FXCODEC_STATUS_ERROR;

            int bVal = pArithDecoder->Decode(&gbContext[CONTEXT]);
            cVal |= bVal << k;
            CONTEXT = ((CONTEXT & 0x01bd) << 1) | bVal |
                      ((line2 >> (k + 3)) & 0x0004);
          }
          m_pLine[cc] = cVal;
        }
        line2 <<= 8;
        uint8_t cVal1 = 0;
        for (int32_t k = 0; k < nBitsLeft; k++) {
          if (pArithDecoder->IsComplete())
            return FXCODEC_STATUS_ERROR;

          int bVal = pArithDecoder->Decode(&gbContext[CONTEXT]);
          cVal1 |= bVal << (7 - k);
          CONTEXT = ((CONTEXT & 0x01bd) << 1) | bVal |
                    (((line2 >> (10 - k))) & 0x0004);
        }
        m_pLine[nLineBytes] = cVal1;
      }
    }
    m_pLine += nStride;
    if (pState->pPause && m_loopIndex % 50 == 0 &&
        pState->pPause->NeedToPauseNow()) {
      m_loopIndex++;
      m_ProssiveStatus = FXCODEC_STATUS_DECODE_TOBECONTINUE;
      return FXCODEC_STATUS_DECODE_TOBECONTINUE;
    }
  }
  m_ProssiveStatus = FXCODEC_STATUS_DECODE_FINISH;
  return FXCODEC_STATUS_DECODE_FINISH;
}

FXCODEC_STATUS CJBig2_GRDProc::ProgressiveDecodeArithTemplate2Unopt(
    ProgressiveArithDecodeState* pState) {
  CJBig2_Image* pImage = pState->pImage->get();
  JBig2ArithCtx* gbContext = pState->gbContext.Get();
  CJBig2_ArithDecoder* pArithDecoder = pState->pArithDecoder.Get();
  for (; m_loopIndex < GBH; m_loopIndex++) {
    if (TPGDON) {
      if (pArithDecoder->IsComplete())
        return FXCODEC_STATUS_ERROR;

      m_LTP = m_LTP ^ pArithDecoder->Decode(&gbContext[0x00e5]);
    }
    if (m_LTP) {
      pImage->CopyLine(m_loopIndex, m_loopIndex - 1);
    } else {
      uint32_t line1 = pImage->GetPixel(1, m_loopIndex - 2);
      line1 |= pImage->GetPixel(0, m_loopIndex - 2) << 1;
      uint32_t line2 = pImage->GetPixel(1, m_loopIndex - 1);
      line2 |= pImage->GetPixel(0, m_loopIndex - 1) << 1;
      uint32_t line3 = 0;
      for (uint32_t w = 0; w < GBW; w++) {
        int bVal;
        if (USESKIP && SKIP->GetPixel(w, m_loopIndex)) {
          bVal = 0;
        } else {
          uint32_t CONTEXT = line3;
          CONTEXT |= pImage->GetPixel(w + GBAT[0], m_loopIndex + GBAT[1]) << 2;
          CONTEXT |= line2 << 3;
          CONTEXT |= line1 << 7;
          if (pArithDecoder->IsComplete())
            return FXCODEC_STATUS_ERROR;

          bVal = pArithDecoder->Decode(&gbContext[CONTEXT]);
        }
        if (bVal) {
          pImage->SetPixel(w, m_loopIndex, bVal);
        }
        line1 =
            ((line1 << 1) | pImage->GetPixel(w + 2, m_loopIndex - 2)) & 0x07;
        line2 =
            ((line2 << 1) | pImage->GetPixel(w + 2, m_loopIndex - 1)) & 0x0f;
        line3 = ((line3 << 1) | bVal) & 0x03;
      }
    }
    if (pState->pPause && pState->pPause->NeedToPauseNow()) {
      m_loopIndex++;
      m_ProssiveStatus = FXCODEC_STATUS_DECODE_TOBECONTINUE;
      return FXCODEC_STATUS_DECODE_TOBECONTINUE;
    }
  }
  m_ProssiveStatus = FXCODEC_STATUS_DECODE_FINISH;
  return FXCODEC_STATUS_DECODE_FINISH;
}

FXCODEC_STATUS CJBig2_GRDProc::ProgressiveDecodeArithTemplate3Opt3(
    ProgressiveArithDecodeState* pState) {
  CJBig2_Image* pImage = pState->pImage->get();
  JBig2ArithCtx* gbContext = pState->gbContext.Get();
  CJBig2_ArithDecoder* pArithDecoder = pState->pArithDecoder.Get();
  if (!m_pLine)
    m_pLine = pImage->data();
  int32_t nStride = pImage->stride();
  int32_t nLineBytes = ((GBW + 7) >> 3) - 1;
  int32_t nBitsLeft = GBW - (nLineBytes << 3);
  for (; m_loopIndex < GBH; m_loopIndex++) {
    if (TPGDON) {
      if (pArithDecoder->IsComplete())
        return FXCODEC_STATUS_ERROR;

      m_LTP = m_LTP ^ pArithDecoder->Decode(&gbContext[0x0195]);
    }
    if (m_LTP) {
      pImage->CopyLine(m_loopIndex, m_loopIndex - 1);
    } else {
      if (m_loopIndex > 0) {
        uint8_t* pLine1 = m_pLine - nStride;
        uint32_t line1 = *pLine1++;
        uint32_t CONTEXT = (line1 >> 1) & 0x03f0;
        for (int32_t cc = 0; cc < nLineBytes; cc++) {
          line1 = (line1 << 8) | (*pLine1++);
          uint8_t cVal = 0;
          for (int32_t k = 7; k >= 0; k--) {
            if (pArithDecoder->IsComplete())
              return FXCODEC_STATUS_ERROR;

            int bVal = pArithDecoder->Decode(&gbContext[CONTEXT]);
            cVal |= bVal << k;
            CONTEXT = ((CONTEXT & 0x01f7) << 1) | bVal |
                      ((line1 >> (k + 1)) & 0x0010);
          }
          m_pLine[cc] = cVal;
        }
        line1 <<= 8;
        uint8_t cVal1 = 0;
        for (int32_t k = 0; k < nBitsLeft; k++) {
          if (pArithDecoder->IsComplete())
            return FXCODEC_STATUS_ERROR;

          int bVal = pArithDecoder->Decode(&gbContext[CONTEXT]);
          cVal1 |= bVal << (7 - k);
          CONTEXT =
              ((CONTEXT & 0x01f7) << 1) | bVal | ((line1 >> (8 - k)) & 0x0010);
        }
        m_pLine[nLineBytes] = cVal1;
      } else {
        uint32_t CONTEXT = 0;
        for (int32_t cc = 0; cc < nLineBytes; cc++) {
          uint8_t cVal = 0;
          for (int32_t k = 7; k >= 0; k--) {
            if (pArithDecoder->IsComplete())
              return FXCODEC_STATUS_ERROR;

            int bVal = pArithDecoder->Decode(&gbContext[CONTEXT]);
            cVal |= bVal << k;
            CONTEXT = ((CONTEXT & 0x01f7) << 1) | bVal;
          }
          m_pLine[cc] = cVal;
        }
        uint8_t cVal1 = 0;
        for (int32_t k = 0; k < nBitsLeft; k++) {
          if (pArithDecoder->IsComplete())
            return FXCODEC_STATUS_ERROR;

          int bVal = pArithDecoder->Decode(&gbContext[CONTEXT]);
          cVal1 |= bVal << (7 - k);
          CONTEXT = ((CONTEXT & 0x01f7) << 1) | bVal;
        }
        m_pLine[nLineBytes] = cVal1;
      }
    }
    m_pLine += nStride;
    if (pState->pPause && pState->pPause->NeedToPauseNow()) {
      m_loopIndex++;
      m_ProssiveStatus = FXCODEC_STATUS_DECODE_TOBECONTINUE;
      return FXCODEC_STATUS_DECODE_TOBECONTINUE;
    }
  }
  m_ProssiveStatus = FXCODEC_STATUS_DECODE_FINISH;
  return FXCODEC_STATUS_DECODE_FINISH;
}

FXCODEC_STATUS CJBig2_GRDProc::ProgressiveDecodeArithTemplate3Unopt(
    ProgressiveArithDecodeState* pState) {
  CJBig2_Image* pImage = pState->pImage->get();
  JBig2ArithCtx* gbContext = pState->gbContext.Get();
  CJBig2_ArithDecoder* pArithDecoder = pState->pArithDecoder.Get();
  for (; m_loopIndex < GBH; m_loopIndex++) {
    if (TPGDON) {
      if (pArithDecoder->IsComplete())
        return FXCODEC_STATUS_ERROR;

      m_LTP = m_LTP ^ pArithDecoder->Decode(&gbContext[0x0195]);
    }
    if (m_LTP) {
      pImage->CopyLine(m_loopIndex, m_loopIndex - 1);
    } else {
      uint32_t line1 = pImage->GetPixel(1, m_loopIndex - 1);
      line1 |= pImage->GetPixel(0, m_loopIndex - 1) << 1;
      uint32_t line2 = 0;
      for (uint32_t w = 0; w < GBW; w++) {
        int bVal;
        if (USESKIP && SKIP->GetPixel(w, m_loopIndex)) {
          bVal = 0;
        } else {
          uint32_t CONTEXT = line2;
          CONTEXT |= pImage->GetPixel(w + GBAT[0], m_loopIndex + GBAT[1]) << 4;
          CONTEXT |= line1 << 5;
          if (pArithDecoder->IsComplete())
            return FXCODEC_STATUS_ERROR;

          bVal = pArithDecoder->Decode(&gbContext[CONTEXT]);
        }
        if (bVal) {
          pImage->SetPixel(w, m_loopIndex, bVal);
        }
        line1 =
            ((line1 << 1) | pImage->GetPixel(w + 2, m_loopIndex - 1)) & 0x1f;
        line2 = ((line2 << 1) | bVal) & 0x0f;
      }
    }
    if (pState->pPause && pState->pPause->NeedToPauseNow()) {
      m_loopIndex++;
      m_ProssiveStatus = FXCODEC_STATUS_DECODE_TOBECONTINUE;
      return FXCODEC_STATUS_DECODE_TOBECONTINUE;
    }
  }
  m_ProssiveStatus = FXCODEC_STATUS_DECODE_FINISH;
  return FXCODEC_STATUS_DECODE_FINISH;
}
