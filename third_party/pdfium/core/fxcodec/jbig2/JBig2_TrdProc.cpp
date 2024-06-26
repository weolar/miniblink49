// Copyright 2015 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcodec/jbig2/JBig2_TrdProc.h"

#include <memory>

#include "core/fxcodec/jbig2/JBig2_ArithDecoder.h"
#include "core/fxcodec/jbig2/JBig2_ArithIntDecoder.h"
#include "core/fxcodec/jbig2/JBig2_GrrdProc.h"
#include "core/fxcodec/jbig2/JBig2_HuffmanDecoder.h"
#include "core/fxcrt/fx_safe_types.h"
#include "core/fxcrt/maybe_owned.h"
#include "third_party/base/optional.h"
#include "third_party/base/ptr_util.h"

namespace {

Optional<uint32_t> CheckTRDDimension(uint32_t dimension, int32_t delta) {
  FX_SAFE_UINT32 result = dimension;
  result += delta;
  if (!result.IsValid())
    return {};
  return {(uint32_t)result.ValueOrDie()};
}

Optional<int32_t> CheckTRDReferenceDimension(int32_t dimension,
                                             uint32_t shift,
                                             int32_t offset) {
  FX_SAFE_INT32 result = offset;
  result += dimension >> shift;
  if (!result.IsValid())
    return {};
  return {(int32_t)result.ValueOrDie()};
}

}  // namespace

JBig2IntDecoderState::JBig2IntDecoderState() = default;

JBig2IntDecoderState::~JBig2IntDecoderState() = default;

CJBig2_TRDProc::CJBig2_TRDProc() = default;

CJBig2_TRDProc::~CJBig2_TRDProc() = default;

std::unique_ptr<CJBig2_Image> CJBig2_TRDProc::DecodeHuffman(
    CJBig2_BitStream* pStream,
    JBig2ArithCtx* grContext) {
  auto SBREG = pdfium::MakeUnique<CJBig2_Image>(SBW, SBH);
  if (!SBREG->data())
    return nullptr;

  SBREG->Fill(SBDEFPIXEL);
  int32_t INITIAL_STRIPT;
  auto pHuffmanDecoder = pdfium::MakeUnique<CJBig2_HuffmanDecoder>(pStream);
  if (pHuffmanDecoder->DecodeAValue(SBHUFFDT.Get(), &INITIAL_STRIPT) != 0)
    return nullptr;

  FX_SAFE_INT32 STRIPT = INITIAL_STRIPT;
  STRIPT *= SBSTRIPS;
  STRIPT = -STRIPT;
  FX_SAFE_INT32 FIRSTS = 0;
  uint32_t NINSTANCES = 0;
  while (NINSTANCES < SBNUMINSTANCES) {
    int32_t INITIAL_DT;
    if (pHuffmanDecoder->DecodeAValue(SBHUFFDT.Get(), &INITIAL_DT) != 0)
      return nullptr;

    FX_SAFE_INT32 DT = INITIAL_DT;
    DT *= SBSTRIPS;
    STRIPT += DT;
    bool bFirst = true;
    FX_SAFE_INT32 CURS = 0;
    for (;;) {
      if (bFirst) {
        int32_t DFS;
        if (pHuffmanDecoder->DecodeAValue(SBHUFFFS.Get(), &DFS) != 0)
          return nullptr;

        FIRSTS += DFS;
        CURS = FIRSTS;
        bFirst = false;
      } else {
        int32_t IDS;
        int32_t nVal = pHuffmanDecoder->DecodeAValue(SBHUFFDS.Get(), &IDS);
        if (nVal == JBIG2_OOB)
          break;

        if (nVal != 0)
          return nullptr;

        CURS += IDS;
        CURS += SBDSOFFSET;
      }
      uint8_t CURT = 0;
      if (SBSTRIPS != 1) {
        uint32_t nTmp = 1;
        while (static_cast<uint32_t>(1 << nTmp) < SBSTRIPS)
          ++nTmp;
        int32_t nVal;
        if (pStream->readNBits(nTmp, &nVal) != 0)
          return nullptr;

        CURT = nVal;
      }
      FX_SAFE_INT32 SAFE_TI = STRIPT + CURT;
      if (!SAFE_TI.IsValid())
        return nullptr;

      int32_t TI = SAFE_TI.ValueOrDie();
      FX_SAFE_INT32 nSafeVal = 0;
      int32_t nBits = 0;
      uint32_t IDI;
      for (;;) {
        uint32_t nTmp;
        if (pStream->read1Bit(&nTmp) != 0)
          return nullptr;

        nSafeVal <<= 1;
        if (!nSafeVal.IsValid())
          return nullptr;

        nSafeVal |= nTmp;
        ++nBits;
        const int32_t nVal = nSafeVal.ValueOrDie();
        for (IDI = 0; IDI < SBNUMSYMS; ++IDI) {
          if (nBits == SBSYMCODES[IDI].codelen && nVal == SBSYMCODES[IDI].code)
            break;
        }
        if (IDI < SBNUMSYMS)
          break;
      }
      bool RI = 0;
      if (SBREFINE != 0 && pStream->read1Bit(&RI) != 0)
        return nullptr;

      MaybeOwned<CJBig2_Image> IBI;
      if (RI == 0) {
        IBI = SBSYMS[IDI];
      } else {
        int32_t RDWI;
        int32_t RDHI;
        int32_t RDXI;
        int32_t RDYI;
        int32_t HUFFRSIZE;
        if ((pHuffmanDecoder->DecodeAValue(SBHUFFRDW.Get(), &RDWI) != 0) ||
            (pHuffmanDecoder->DecodeAValue(SBHUFFRDH.Get(), &RDHI) != 0) ||
            (pHuffmanDecoder->DecodeAValue(SBHUFFRDX.Get(), &RDXI) != 0) ||
            (pHuffmanDecoder->DecodeAValue(SBHUFFRDY.Get(), &RDYI) != 0) ||
            (pHuffmanDecoder->DecodeAValue(SBHUFFRSIZE.Get(), &HUFFRSIZE) !=
             0)) {
          return nullptr;
        }
        pStream->alignByte();
        uint32_t nTmp = pStream->getOffset();
        CJBig2_Image* IBOI = SBSYMS[IDI];
        if (!IBOI)
          return nullptr;

        Optional<uint32_t> WOI = CheckTRDDimension(IBOI->width(), RDWI);
        Optional<uint32_t> HOI = CheckTRDDimension(IBOI->height(), RDHI);
        if (!WOI || !HOI)
          return nullptr;

        Optional<int32_t> GRREFERENCEDX =
            CheckTRDReferenceDimension(RDWI, 2, RDXI);
        Optional<int32_t> GRREFERENCEDY =
            CheckTRDReferenceDimension(RDHI, 2, RDYI);
        if (!GRREFERENCEDX || !GRREFERENCEDY)
          return nullptr;

        auto pGRRD = pdfium::MakeUnique<CJBig2_GRRDProc>();
        pGRRD->GRW = WOI.value();
        pGRRD->GRH = HOI.value();
        pGRRD->GRTEMPLATE = SBRTEMPLATE;
        pGRRD->GRREFERENCE = IBOI;
        pGRRD->GRREFERENCEDX = GRREFERENCEDX.value();
        pGRRD->GRREFERENCEDY = GRREFERENCEDY.value();
        pGRRD->TPGRON = 0;
        pGRRD->GRAT[0] = SBRAT[0];
        pGRRD->GRAT[1] = SBRAT[1];
        pGRRD->GRAT[2] = SBRAT[2];
        pGRRD->GRAT[3] = SBRAT[3];

        auto pArithDecoder = pdfium::MakeUnique<CJBig2_ArithDecoder>(pStream);
        IBI = pGRRD->Decode(pArithDecoder.get(), grContext);
        if (!IBI)
          return nullptr;

        pStream->alignByte();
        pStream->offset(2);
        if (static_cast<uint32_t>(HUFFRSIZE) != (pStream->getOffset() - nTmp))
          return nullptr;
      }
      if (!IBI)
        continue;

      uint32_t WI = IBI->width();
      uint32_t HI = IBI->height();
      if (TRANSPOSED == 0 && ((REFCORNER == JBIG2_CORNER_TOPRIGHT) ||
                              (REFCORNER == JBIG2_CORNER_BOTTOMRIGHT))) {
        CURS += WI - 1;
      } else if (TRANSPOSED == 1 && ((REFCORNER == JBIG2_CORNER_BOTTOMLEFT) ||
                                     (REFCORNER == JBIG2_CORNER_BOTTOMRIGHT))) {
        CURS += HI - 1;
      }
      if (!CURS.IsValid())
        return nullptr;

      int32_t SI = CURS.ValueOrDie();
      ComposeData compose = GetComposeData(SI, TI, WI, HI);
      IBI.Get()->ComposeTo(SBREG.get(), compose.x, compose.y, SBCOMBOP);
      if (compose.increment)
        CURS += compose.increment;
      ++NINSTANCES;
    }
  }
  return SBREG;
}

std::unique_ptr<CJBig2_Image> CJBig2_TRDProc::DecodeArith(
    CJBig2_ArithDecoder* pArithDecoder,
    JBig2ArithCtx* grContext,
    JBig2IntDecoderState* pIDS) {
  auto SBREG = pdfium::MakeUnique<CJBig2_Image>(SBW, SBH);
  if (!SBREG->data())
    return nullptr;

  MaybeOwned<CJBig2_ArithIntDecoder> pIADT;
  if (pIDS)
    pIADT = pIDS->IADT;
  else
    pIADT = pdfium::MakeUnique<CJBig2_ArithIntDecoder>();
  int32_t INITIAL_STRIPT;
  if (!pIADT->Decode(pArithDecoder, &INITIAL_STRIPT))
    return nullptr;

  MaybeOwned<CJBig2_ArithIntDecoder> pIAFS;
  MaybeOwned<CJBig2_ArithIntDecoder> pIADS;
  MaybeOwned<CJBig2_ArithIntDecoder> pIAIT;
  MaybeOwned<CJBig2_ArithIntDecoder> pIARI;
  MaybeOwned<CJBig2_ArithIntDecoder> pIARDW;
  MaybeOwned<CJBig2_ArithIntDecoder> pIARDH;
  MaybeOwned<CJBig2_ArithIntDecoder> pIARDX;
  MaybeOwned<CJBig2_ArithIntDecoder> pIARDY;
  MaybeOwned<CJBig2_ArithIaidDecoder> pIAID;
  if (pIDS) {
    pIAFS = pIDS->IAFS;
    pIADS = pIDS->IADS;
    pIAIT = pIDS->IAIT;
    pIARI = pIDS->IARI;
    pIARDW = pIDS->IARDW;
    pIARDH = pIDS->IARDH;
    pIARDX = pIDS->IARDX;
    pIARDY = pIDS->IARDY;
    pIAID = pIDS->IAID;
  } else {
    pIAFS = pdfium::MakeUnique<CJBig2_ArithIntDecoder>();
    pIADS = pdfium::MakeUnique<CJBig2_ArithIntDecoder>();
    pIAIT = pdfium::MakeUnique<CJBig2_ArithIntDecoder>();
    pIARI = pdfium::MakeUnique<CJBig2_ArithIntDecoder>();
    pIARDW = pdfium::MakeUnique<CJBig2_ArithIntDecoder>();
    pIARDH = pdfium::MakeUnique<CJBig2_ArithIntDecoder>();
    pIARDX = pdfium::MakeUnique<CJBig2_ArithIntDecoder>();
    pIARDY = pdfium::MakeUnique<CJBig2_ArithIntDecoder>();
    pIAID = pdfium::MakeUnique<CJBig2_ArithIaidDecoder>(SBSYMCODELEN);
  }

  SBREG->Fill(SBDEFPIXEL);

  FX_SAFE_INT32 STRIPT = INITIAL_STRIPT;
  STRIPT *= SBSTRIPS;
  STRIPT = -STRIPT;
  FX_SAFE_INT32 FIRSTS = 0;
  uint32_t NINSTANCES = 0;
  while (NINSTANCES < SBNUMINSTANCES) {
    FX_SAFE_INT32 CURS = 0;
    int32_t INITIAL_DT;
    if (!pIADT->Decode(pArithDecoder, &INITIAL_DT))
      return nullptr;

    FX_SAFE_INT32 DT = INITIAL_DT;
    DT *= SBSTRIPS;
    STRIPT += DT;
    bool bFirst = true;
    for (;;) {
      if (bFirst) {
        int32_t DFS;
        pIAFS->Decode(pArithDecoder, &DFS);
        FIRSTS += DFS;
        CURS = FIRSTS;
        bFirst = false;
      } else {
        int32_t IDS;
        if (!pIADS->Decode(pArithDecoder, &IDS))
          break;

        CURS += IDS;
        CURS += SBDSOFFSET;
      }
      if (NINSTANCES >= SBNUMINSTANCES)
        break;

      int CURT = 0;
      if (SBSTRIPS != 1)
        pIAIT->Decode(pArithDecoder, &CURT);

      FX_SAFE_INT32 SAFE_TI = STRIPT + CURT;
      if (!SAFE_TI.IsValid())
        return nullptr;

      int32_t TI = SAFE_TI.ValueOrDie();
      uint32_t IDI;
      pIAID->Decode(pArithDecoder, &IDI);
      if (IDI >= SBNUMSYMS)
        return nullptr;

      int RI;
      if (SBREFINE == 0)
        RI = 0;
      else
        pIARI->Decode(pArithDecoder, &RI);

      MaybeOwned<CJBig2_Image> pIBI;
      if (RI == 0) {
        pIBI = SBSYMS[IDI];
      } else {
        int32_t RDWI;
        int32_t RDHI;
        int32_t RDXI;
        int32_t RDYI;
        pIARDW->Decode(pArithDecoder, &RDWI);
        pIARDH->Decode(pArithDecoder, &RDHI);
        pIARDX->Decode(pArithDecoder, &RDXI);
        pIARDY->Decode(pArithDecoder, &RDYI);
        CJBig2_Image* IBOI = SBSYMS[IDI];
        if (!IBOI)
          return nullptr;

        Optional<uint32_t> WOI = CheckTRDDimension(IBOI->width(), RDWI);
        Optional<uint32_t> HOI = CheckTRDDimension(IBOI->height(), RDHI);
        if (!WOI || !HOI)
          return nullptr;

        Optional<int32_t> GRREFERENCEDX =
            CheckTRDReferenceDimension(RDWI, 1, RDXI);
        Optional<int32_t> GRREFERENCEDY =
            CheckTRDReferenceDimension(RDHI, 1, RDYI);
        if (!GRREFERENCEDX || !GRREFERENCEDY)
          return nullptr;

        auto pGRRD = pdfium::MakeUnique<CJBig2_GRRDProc>();
        pGRRD->GRW = WOI.value();
        pGRRD->GRH = HOI.value();
        pGRRD->GRTEMPLATE = SBRTEMPLATE;
        pGRRD->GRREFERENCE = IBOI;
        pGRRD->GRREFERENCEDX = GRREFERENCEDX.value();
        pGRRD->GRREFERENCEDY = GRREFERENCEDY.value();
        pGRRD->TPGRON = 0;
        pGRRD->GRAT[0] = SBRAT[0];
        pGRRD->GRAT[1] = SBRAT[1];
        pGRRD->GRAT[2] = SBRAT[2];
        pGRRD->GRAT[3] = SBRAT[3];
        pIBI = pGRRD->Decode(pArithDecoder, grContext);
      }
      if (!pIBI)
        return nullptr;

      uint32_t WI = pIBI->width();
      uint32_t HI = pIBI->height();
      if (TRANSPOSED == 0 && ((REFCORNER == JBIG2_CORNER_TOPRIGHT) ||
                              (REFCORNER == JBIG2_CORNER_BOTTOMRIGHT))) {
        CURS += WI - 1;
      } else if (TRANSPOSED == 1 && ((REFCORNER == JBIG2_CORNER_BOTTOMLEFT) ||
                                     (REFCORNER == JBIG2_CORNER_BOTTOMRIGHT))) {
        CURS += HI - 1;
      }
      if (!CURS.IsValid())
        return nullptr;

      int32_t SI = CURS.ValueOrDie();
      ComposeData compose = GetComposeData(SI, TI, WI, HI);
      pIBI.Get()->ComposeTo(SBREG.get(), compose.x, compose.y, SBCOMBOP);
      if (compose.increment)
        CURS += compose.increment;
      ++NINSTANCES;
    }
  }
  return SBREG;
}

CJBig2_TRDProc::ComposeData CJBig2_TRDProc::GetComposeData(int32_t SI,
                                                           int32_t TI,
                                                           uint32_t WI,
                                                           uint32_t HI) const {
  ComposeData results;
  if (TRANSPOSED == 0) {
    switch (REFCORNER) {
      case JBIG2_CORNER_TOPLEFT:
        results.x = SI;
        results.y = TI;
        results.increment = WI - 1;
        break;
      case JBIG2_CORNER_TOPRIGHT:
        results.x = SI - WI + 1;
        results.y = TI;
        break;
      case JBIG2_CORNER_BOTTOMLEFT:
        results.x = SI;
        results.y = TI - HI + 1;
        results.increment = WI - 1;
        break;
      case JBIG2_CORNER_BOTTOMRIGHT:
        results.x = SI - WI + 1;
        results.y = TI - HI + 1;
        break;
    }
  } else {
    switch (REFCORNER) {
      case JBIG2_CORNER_TOPLEFT:
        results.x = TI;
        results.y = SI;
        results.increment = HI - 1;
        break;
      case JBIG2_CORNER_TOPRIGHT:
        results.x = TI - WI + 1;
        results.y = SI;
        results.increment = HI - 1;
        break;
      case JBIG2_CORNER_BOTTOMLEFT:
        results.x = TI;
        results.y = SI - HI + 1;
        break;
      case JBIG2_CORNER_BOTTOMRIGHT:
        results.x = TI - WI + 1;
        results.y = SI - HI + 1;
        break;
    }
  }
  return results;
}
