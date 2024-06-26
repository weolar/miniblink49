// Copyright 2015 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcodec/jbig2/JBig2_SddProc.h"

#include <string.h>

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

#include "core/fxcodec/jbig2/JBig2_ArithIntDecoder.h"
#include "core/fxcodec/jbig2/JBig2_GrdProc.h"
#include "core/fxcodec/jbig2/JBig2_GrrdProc.h"
#include "core/fxcodec/jbig2/JBig2_HuffmanDecoder.h"
#include "core/fxcodec/jbig2/JBig2_HuffmanTable.h"
#include "core/fxcodec/jbig2/JBig2_SymbolDict.h"
#include "core/fxcodec/jbig2/JBig2_TrdProc.h"
#include "third_party/base/ptr_util.h"

CJBig2_SDDProc::CJBig2_SDDProc() = default;

CJBig2_SDDProc::~CJBig2_SDDProc() = default;

std::unique_ptr<CJBig2_SymbolDict> CJBig2_SDDProc::DecodeArith(
    CJBig2_ArithDecoder* pArithDecoder,
    std::vector<JBig2ArithCtx>* gbContext,
    std::vector<JBig2ArithCtx>* grContext) {
  std::vector<std::unique_ptr<CJBig2_Image>> SDNEWSYMS;
  uint32_t HCHEIGHT, NSYMSDECODED;
  int32_t HCDH;
  uint32_t SYMWIDTH, TOTWIDTH;
  int32_t DW;
  uint32_t I, J, REFAGGNINST;
  std::vector<bool> EXFLAGS;
  uint32_t EXINDEX;
  bool CUREXFLAG;
  uint32_t EXRUNLENGTH;
  uint32_t nTmp;
  uint32_t SBNUMSYMS;
  uint8_t SBSYMCODELEN;
  int32_t RDXI, RDYI;
  uint32_t num_ex_syms;
  // Pointers are not owned
  std::vector<CJBig2_Image*> SBSYMS;
  std::unique_ptr<CJBig2_ArithIaidDecoder> IAID;
  std::unique_ptr<CJBig2_SymbolDict> pDict;
  auto IADH = pdfium::MakeUnique<CJBig2_ArithIntDecoder>();
  auto IADW = pdfium::MakeUnique<CJBig2_ArithIntDecoder>();
  auto IAAI = pdfium::MakeUnique<CJBig2_ArithIntDecoder>();
  auto IARDX = pdfium::MakeUnique<CJBig2_ArithIntDecoder>();
  auto IARDY = pdfium::MakeUnique<CJBig2_ArithIntDecoder>();
  auto IAEX = pdfium::MakeUnique<CJBig2_ArithIntDecoder>();
  auto IADT = pdfium::MakeUnique<CJBig2_ArithIntDecoder>();
  auto IAFS = pdfium::MakeUnique<CJBig2_ArithIntDecoder>();
  auto IADS = pdfium::MakeUnique<CJBig2_ArithIntDecoder>();
  auto IAIT = pdfium::MakeUnique<CJBig2_ArithIntDecoder>();
  auto IARI = pdfium::MakeUnique<CJBig2_ArithIntDecoder>();
  auto IARDW = pdfium::MakeUnique<CJBig2_ArithIntDecoder>();
  auto IARDH = pdfium::MakeUnique<CJBig2_ArithIntDecoder>();
  nTmp = 0;
  while ((uint32_t)(1 << nTmp) < (SDNUMINSYMS + SDNUMNEWSYMS)) {
    nTmp++;
  }
  IAID = pdfium::MakeUnique<CJBig2_ArithIaidDecoder>((uint8_t)nTmp);
  SDNEWSYMS.resize(SDNUMNEWSYMS);

  HCHEIGHT = 0;
  NSYMSDECODED = 0;
  while (NSYMSDECODED < SDNUMNEWSYMS) {
    std::unique_ptr<CJBig2_Image> BS;
    IADH->Decode(pArithDecoder, &HCDH);
    HCHEIGHT = HCHEIGHT + HCDH;
    if ((int)HCHEIGHT < 0 || (int)HCHEIGHT > JBIG2_MAX_IMAGE_SIZE)
      return nullptr;

    SYMWIDTH = 0;
    TOTWIDTH = 0;
    for (;;) {
      if (!IADW->Decode(pArithDecoder, &DW))
        break;

      if (NSYMSDECODED >= SDNUMNEWSYMS)
        return nullptr;

      SYMWIDTH = SYMWIDTH + DW;
      if ((int)SYMWIDTH < 0 || (int)SYMWIDTH > JBIG2_MAX_IMAGE_SIZE)
        return nullptr;

      if (HCHEIGHT == 0 || SYMWIDTH == 0) {
        TOTWIDTH = TOTWIDTH + SYMWIDTH;
        SDNEWSYMS[NSYMSDECODED] = nullptr;
        NSYMSDECODED = NSYMSDECODED + 1;
        continue;
      }
      TOTWIDTH = TOTWIDTH + SYMWIDTH;
      if (SDREFAGG == 0) {
        auto pGRD = pdfium::MakeUnique<CJBig2_GRDProc>();
        pGRD->MMR = 0;
        pGRD->GBW = SYMWIDTH;
        pGRD->GBH = HCHEIGHT;
        pGRD->GBTEMPLATE = SDTEMPLATE;
        pGRD->TPGDON = 0;
        pGRD->USESKIP = 0;
        pGRD->GBAT[0] = SDAT[0];
        pGRD->GBAT[1] = SDAT[1];
        pGRD->GBAT[2] = SDAT[2];
        pGRD->GBAT[3] = SDAT[3];
        pGRD->GBAT[4] = SDAT[4];
        pGRD->GBAT[5] = SDAT[5];
        pGRD->GBAT[6] = SDAT[6];
        pGRD->GBAT[7] = SDAT[7];
        BS = pGRD->DecodeArith(pArithDecoder, gbContext->data());
        if (!BS)
          return nullptr;
      } else {
        IAAI->Decode(pArithDecoder, (int*)&REFAGGNINST);
        if (REFAGGNINST > 1) {
          // Huffman tables must not outlive |pDecoder|.
          auto SBHUFFFS = pdfium::MakeUnique<CJBig2_HuffmanTable>(6);
          auto SBHUFFDS = pdfium::MakeUnique<CJBig2_HuffmanTable>(8);
          auto SBHUFFDT = pdfium::MakeUnique<CJBig2_HuffmanTable>(11);
          auto SBHUFFRDW = pdfium::MakeUnique<CJBig2_HuffmanTable>(15);
          auto SBHUFFRDH = pdfium::MakeUnique<CJBig2_HuffmanTable>(15);
          auto SBHUFFRDX = pdfium::MakeUnique<CJBig2_HuffmanTable>(15);
          auto SBHUFFRDY = pdfium::MakeUnique<CJBig2_HuffmanTable>(15);
          auto SBHUFFRSIZE = pdfium::MakeUnique<CJBig2_HuffmanTable>(1);
          auto pDecoder = pdfium::MakeUnique<CJBig2_TRDProc>();
          pDecoder->SBHUFF = SDHUFF;
          pDecoder->SBREFINE = 1;
          pDecoder->SBW = SYMWIDTH;
          pDecoder->SBH = HCHEIGHT;
          pDecoder->SBNUMINSTANCES = REFAGGNINST;
          pDecoder->SBSTRIPS = 1;
          pDecoder->SBNUMSYMS = SDNUMINSYMS + NSYMSDECODED;
          SBNUMSYMS = pDecoder->SBNUMSYMS;
          nTmp = 0;
          while ((uint32_t)(1 << nTmp) < SBNUMSYMS) {
            nTmp++;
          }
          SBSYMCODELEN = (uint8_t)nTmp;
          pDecoder->SBSYMCODELEN = SBSYMCODELEN;
          SBSYMS.resize(SBNUMSYMS);
          std::copy(SDINSYMS, SDINSYMS + SDNUMINSYMS, SBSYMS.begin());
          for (size_t i = 0; i < NSYMSDECODED; ++i)
            SBSYMS[i + SDNUMINSYMS] = SDNEWSYMS[i].get();
          pDecoder->SBSYMS = SBSYMS.data();
          pDecoder->SBDEFPIXEL = 0;
          pDecoder->SBCOMBOP = JBIG2_COMPOSE_OR;
          pDecoder->TRANSPOSED = 0;
          pDecoder->REFCORNER = JBIG2_CORNER_TOPLEFT;
          pDecoder->SBDSOFFSET = 0;
          pDecoder->SBHUFFFS = SBHUFFFS.get();
          pDecoder->SBHUFFDS = SBHUFFDS.get();
          pDecoder->SBHUFFDT = SBHUFFDT.get();
          pDecoder->SBHUFFRDW = SBHUFFRDW.get();
          pDecoder->SBHUFFRDH = SBHUFFRDH.get();
          pDecoder->SBHUFFRDX = SBHUFFRDX.get();
          pDecoder->SBHUFFRDY = SBHUFFRDY.get();
          pDecoder->SBHUFFRSIZE = SBHUFFRSIZE.get();
          pDecoder->SBRTEMPLATE = SDRTEMPLATE;
          pDecoder->SBRAT[0] = SDRAT[0];
          pDecoder->SBRAT[1] = SDRAT[1];
          pDecoder->SBRAT[2] = SDRAT[2];
          pDecoder->SBRAT[3] = SDRAT[3];
          JBig2IntDecoderState ids;
          ids.IADT = IADT.get();
          ids.IAFS = IAFS.get();
          ids.IADS = IADS.get();
          ids.IAIT = IAIT.get();
          ids.IARI = IARI.get();
          ids.IARDW = IARDW.get();
          ids.IARDH = IARDH.get();
          ids.IARDX = IARDX.get();
          ids.IARDY = IARDY.get();
          ids.IAID = IAID.get();
          BS = pDecoder->DecodeArith(pArithDecoder, grContext->data(), &ids);
          if (!BS)
            return nullptr;
        } else if (REFAGGNINST == 1) {
          SBNUMSYMS = SDNUMINSYMS + NSYMSDECODED;
          uint32_t IDI;
          IAID->Decode(pArithDecoder, &IDI);
          IARDX->Decode(pArithDecoder, &RDXI);
          IARDY->Decode(pArithDecoder, &RDYI);
          if (IDI >= SBNUMSYMS)
            return nullptr;

          SBSYMS.resize(SBNUMSYMS);
          std::copy(SDINSYMS, SDINSYMS + SDNUMINSYMS, SBSYMS.begin());
          for (size_t i = 0; i < NSYMSDECODED; ++i)
            SBSYMS[i + SDNUMINSYMS] = SDNEWSYMS[i].get();
          if (!SBSYMS[IDI])
            return nullptr;

          auto pGRRD = pdfium::MakeUnique<CJBig2_GRRDProc>();
          pGRRD->GRW = SYMWIDTH;
          pGRRD->GRH = HCHEIGHT;
          pGRRD->GRTEMPLATE = SDRTEMPLATE;
          pGRRD->GRREFERENCE = SBSYMS[IDI];
          pGRRD->GRREFERENCEDX = RDXI;
          pGRRD->GRREFERENCEDY = RDYI;
          pGRRD->TPGRON = 0;
          pGRRD->GRAT[0] = SDRAT[0];
          pGRRD->GRAT[1] = SDRAT[1];
          pGRRD->GRAT[2] = SDRAT[2];
          pGRRD->GRAT[3] = SDRAT[3];
          BS = pGRRD->Decode(pArithDecoder, grContext->data());
          if (!BS)
            return nullptr;
        }
      }
      SDNEWSYMS[NSYMSDECODED] = std::move(BS);
      NSYMSDECODED = NSYMSDECODED + 1;
    }
  }
  EXINDEX = 0;
  CUREXFLAG = 0;
  EXFLAGS.resize(SDNUMINSYMS + SDNUMNEWSYMS);
  num_ex_syms = 0;
  while (EXINDEX < SDNUMINSYMS + SDNUMNEWSYMS) {
    IAEX->Decode(pArithDecoder, (int*)&EXRUNLENGTH);
    if (EXINDEX + EXRUNLENGTH > SDNUMINSYMS + SDNUMNEWSYMS)
      return nullptr;

    if (EXRUNLENGTH != 0) {
      for (I = EXINDEX; I < EXINDEX + EXRUNLENGTH; I++) {
        if (CUREXFLAG)
          num_ex_syms++;
        EXFLAGS[I] = CUREXFLAG;
      }
    }
    EXINDEX = EXINDEX + EXRUNLENGTH;
    CUREXFLAG = !CUREXFLAG;
  }
  if (num_ex_syms > SDNUMEXSYMS)
    return nullptr;

  pDict = pdfium::MakeUnique<CJBig2_SymbolDict>();
  I = J = 0;
  for (I = 0; I < SDNUMINSYMS + SDNUMNEWSYMS; I++) {
    if (!EXFLAGS[I] || J >= SDNUMEXSYMS)
      continue;
    if (I < SDNUMINSYMS) {
      pDict->AddImage(SDINSYMS[I]
                          ? pdfium::MakeUnique<CJBig2_Image>(*SDINSYMS[I])
                          : nullptr);
    } else {
      pDict->AddImage(std::move(SDNEWSYMS[I - SDNUMINSYMS]));
    }
    ++J;
  }
  return pDict;
}

std::unique_ptr<CJBig2_SymbolDict> CJBig2_SDDProc::DecodeHuffman(
    CJBig2_BitStream* pStream,
    std::vector<JBig2ArithCtx>* gbContext,
    std::vector<JBig2ArithCtx>* grContext) {
  std::vector<std::unique_ptr<CJBig2_Image>> SDNEWSYMS;
  std::vector<uint32_t> SDNEWSYMWIDTHS;
  uint32_t HCHEIGHT, NSYMSDECODED;
  int32_t HCDH;
  uint32_t SYMWIDTH, TOTWIDTH, HCFIRSTSYM;
  int32_t DW;
  uint32_t I, J, REFAGGNINST;
  std::vector<bool> EXFLAGS;
  uint32_t EXINDEX;
  bool CUREXFLAG;
  uint32_t EXRUNLENGTH;
  int32_t nVal;
  uint32_t nTmp;
  uint32_t SBNUMSYMS;
  uint8_t SBSYMCODELEN;
  uint32_t IDI;
  int32_t RDXI, RDYI;
  uint32_t BMSIZE;
  uint32_t stride;
  uint32_t num_ex_syms;
  // Pointers are not owned
  std::vector<CJBig2_Image*> SBSYMS;
  auto pHuffmanDecoder = pdfium::MakeUnique<CJBig2_HuffmanDecoder>(pStream);
  SDNEWSYMS.resize(SDNUMNEWSYMS);
  if (SDREFAGG == 0)
    SDNEWSYMWIDTHS.resize(SDNUMNEWSYMS);
  auto pDict = pdfium::MakeUnique<CJBig2_SymbolDict>();
  std::unique_ptr<CJBig2_HuffmanTable> pTable;

  HCHEIGHT = 0;
  NSYMSDECODED = 0;
  std::unique_ptr<CJBig2_Image> BS;
  while (NSYMSDECODED < SDNUMNEWSYMS) {
    if (pHuffmanDecoder->DecodeAValue(SDHUFFDH.Get(), &HCDH) != 0)
      return nullptr;

    HCHEIGHT = HCHEIGHT + HCDH;
    if ((int)HCHEIGHT < 0 || (int)HCHEIGHT > JBIG2_MAX_IMAGE_SIZE)
      return nullptr;

    SYMWIDTH = 0;
    TOTWIDTH = 0;
    HCFIRSTSYM = NSYMSDECODED;
    for (;;) {
      nVal = pHuffmanDecoder->DecodeAValue(SDHUFFDW.Get(), &DW);
      if (nVal == JBIG2_OOB)
        break;
      if (nVal != 0)
        return nullptr;
      if (NSYMSDECODED >= SDNUMNEWSYMS)
        return nullptr;

      SYMWIDTH = SYMWIDTH + DW;
      if ((int)SYMWIDTH < 0 || (int)SYMWIDTH > JBIG2_MAX_IMAGE_SIZE)
        return nullptr;
      if (HCHEIGHT == 0 || SYMWIDTH == 0) {
        TOTWIDTH = TOTWIDTH + SYMWIDTH;
        SDNEWSYMS[NSYMSDECODED] = nullptr;
        NSYMSDECODED = NSYMSDECODED + 1;
        continue;
      }
      TOTWIDTH = TOTWIDTH + SYMWIDTH;
      if (SDREFAGG == 1) {
        if (pHuffmanDecoder->DecodeAValue(SDHUFFAGGINST.Get(),
                                          (int*)&REFAGGNINST) != 0) {
          return nullptr;
        }
        BS = nullptr;
        if (REFAGGNINST > 1) {
          // Huffman tables must outlive |pDecoder|.
          auto SBHUFFFS = pdfium::MakeUnique<CJBig2_HuffmanTable>(6);
          auto SBHUFFDS = pdfium::MakeUnique<CJBig2_HuffmanTable>(8);
          auto SBHUFFDT = pdfium::MakeUnique<CJBig2_HuffmanTable>(11);
          auto SBHUFFRDW = pdfium::MakeUnique<CJBig2_HuffmanTable>(15);
          auto SBHUFFRDH = pdfium::MakeUnique<CJBig2_HuffmanTable>(15);
          auto SBHUFFRDX = pdfium::MakeUnique<CJBig2_HuffmanTable>(15);
          auto SBHUFFRDY = pdfium::MakeUnique<CJBig2_HuffmanTable>(15);
          auto SBHUFFRSIZE = pdfium::MakeUnique<CJBig2_HuffmanTable>(1);
          auto pDecoder = pdfium::MakeUnique<CJBig2_TRDProc>();
          pDecoder->SBHUFF = SDHUFF;
          pDecoder->SBREFINE = 1;
          pDecoder->SBW = SYMWIDTH;
          pDecoder->SBH = HCHEIGHT;
          pDecoder->SBNUMINSTANCES = REFAGGNINST;
          pDecoder->SBSTRIPS = 1;
          pDecoder->SBNUMSYMS = SDNUMINSYMS + NSYMSDECODED;
          SBNUMSYMS = pDecoder->SBNUMSYMS;
          std::vector<JBig2HuffmanCode> SBSYMCODES(SBNUMSYMS);
          nTmp = 1;
          while (static_cast<uint32_t>(1 << nTmp) < SBNUMSYMS)
            ++nTmp;
          for (I = 0; I < SBNUMSYMS; ++I) {
            SBSYMCODES[I].codelen = nTmp;
            SBSYMCODES[I].code = I;
          }
          pDecoder->SBSYMCODES = std::move(SBSYMCODES);
          SBSYMS.resize(SBNUMSYMS);
          std::copy(SDINSYMS, SDINSYMS + SDNUMINSYMS, SBSYMS.begin());
          for (size_t i = 0; i < NSYMSDECODED; ++i)
            SBSYMS[i + SDNUMINSYMS] = SDNEWSYMS[i].get();
          pDecoder->SBSYMS = SBSYMS.data();
          pDecoder->SBDEFPIXEL = 0;
          pDecoder->SBCOMBOP = JBIG2_COMPOSE_OR;
          pDecoder->TRANSPOSED = 0;
          pDecoder->REFCORNER = JBIG2_CORNER_TOPLEFT;
          pDecoder->SBDSOFFSET = 0;
          pDecoder->SBHUFFFS = SBHUFFFS.get();
          pDecoder->SBHUFFDS = SBHUFFDS.get();
          pDecoder->SBHUFFDT = SBHUFFDT.get();
          pDecoder->SBHUFFRDW = SBHUFFRDW.get();
          pDecoder->SBHUFFRDH = SBHUFFRDH.get();
          pDecoder->SBHUFFRDX = SBHUFFRDX.get();
          pDecoder->SBHUFFRDY = SBHUFFRDY.get();
          pDecoder->SBHUFFRSIZE = SBHUFFRSIZE.get();
          pDecoder->SBRTEMPLATE = SDRTEMPLATE;
          pDecoder->SBRAT[0] = SDRAT[0];
          pDecoder->SBRAT[1] = SDRAT[1];
          pDecoder->SBRAT[2] = SDRAT[2];
          pDecoder->SBRAT[3] = SDRAT[3];
          BS = pDecoder->DecodeHuffman(pStream, grContext->data());
          if (!BS)
            return nullptr;

        } else if (REFAGGNINST == 1) {
          SBNUMSYMS = SDNUMINSYMS + SDNUMNEWSYMS;
          nTmp = 1;
          while ((uint32_t)(1 << nTmp) < SBNUMSYMS) {
            nTmp++;
          }
          SBSYMCODELEN = (uint8_t)nTmp;
          uint32_t uVal = 0;
          for (;;) {
            if (pStream->read1Bit(&nTmp) != 0)
              return nullptr;

            uVal = (uVal << 1) | nTmp;
            if (uVal >= SBNUMSYMS)
              return nullptr;

            IDI = SBSYMCODELEN == 0 ? uVal : SBNUMSYMS;
            if (IDI < SBNUMSYMS)
              break;
          }
          auto SBHUFFRDX = pdfium::MakeUnique<CJBig2_HuffmanTable>(15);
          auto SBHUFFRSIZE = pdfium::MakeUnique<CJBig2_HuffmanTable>(1);
          if ((pHuffmanDecoder->DecodeAValue(SBHUFFRDX.get(), &RDXI) != 0) ||
              (pHuffmanDecoder->DecodeAValue(SBHUFFRDX.get(), &RDYI) != 0) ||
              (pHuffmanDecoder->DecodeAValue(SBHUFFRSIZE.get(), &nVal) != 0)) {
            return nullptr;
          }
          pStream->alignByte();
          nTmp = pStream->getOffset();
          SBSYMS.resize(SBNUMSYMS);
          std::copy(SDINSYMS, SDINSYMS + SDNUMINSYMS, SBSYMS.begin());
          for (size_t i = 0; i < NSYMSDECODED; ++i)
            SBSYMS[i + SDNUMINSYMS] = SDNEWSYMS[i].get();
          auto pGRRD = pdfium::MakeUnique<CJBig2_GRRDProc>();
          pGRRD->GRW = SYMWIDTH;
          pGRRD->GRH = HCHEIGHT;
          pGRRD->GRTEMPLATE = SDRTEMPLATE;
          pGRRD->GRREFERENCE = SBSYMS[IDI];
          pGRRD->GRREFERENCEDX = RDXI;
          pGRRD->GRREFERENCEDY = RDYI;
          pGRRD->TPGRON = 0;
          pGRRD->GRAT[0] = SDRAT[0];
          pGRRD->GRAT[1] = SDRAT[1];
          pGRRD->GRAT[2] = SDRAT[2];
          pGRRD->GRAT[3] = SDRAT[3];
          auto pArithDecoder = pdfium::MakeUnique<CJBig2_ArithDecoder>(pStream);
          BS = pGRRD->Decode(pArithDecoder.get(), grContext->data());
          if (!BS)
            return nullptr;

          pStream->alignByte();
          pStream->offset(2);
          if ((uint32_t)nVal != (pStream->getOffset() - nTmp))
            return nullptr;
        }
        SDNEWSYMS[NSYMSDECODED] = std::move(BS);
      }
      if (SDREFAGG == 0)
        SDNEWSYMWIDTHS[NSYMSDECODED] = SYMWIDTH;
      NSYMSDECODED = NSYMSDECODED + 1;
    }
    if (SDREFAGG == 0) {
      if (pHuffmanDecoder->DecodeAValue(SDHUFFBMSIZE.Get(),
                                        (int32_t*)&BMSIZE) != 0) {
        return nullptr;
      }
      pStream->alignByte();
      std::unique_ptr<CJBig2_Image> BHC;
      if (BMSIZE == 0) {
        stride = (TOTWIDTH + 7) >> 3;
        if (pStream->getByteLeft() >= stride * HCHEIGHT) {
          BHC = pdfium::MakeUnique<CJBig2_Image>(TOTWIDTH, HCHEIGHT);
          for (I = 0; I < HCHEIGHT; I++) {
            memcpy(BHC->data() + I * BHC->stride(), pStream->getPointer(),
                   stride);
            pStream->offset(stride);
          }
        } else {
          return nullptr;
        }
      } else {
        auto pGRD = pdfium::MakeUnique<CJBig2_GRDProc>();
        pGRD->MMR = 1;
        pGRD->GBW = TOTWIDTH;
        pGRD->GBH = HCHEIGHT;
        pGRD->StartDecodeMMR(&BHC, pStream);
        pStream->alignByte();
      }
      nTmp = 0;
      if (!BHC)
        continue;

      for (I = HCFIRSTSYM; I < NSYMSDECODED; ++I) {
        SDNEWSYMS[I] = BHC->SubImage(nTmp, 0, SDNEWSYMWIDTHS[I], HCHEIGHT);
        nTmp += SDNEWSYMWIDTHS[I];
      }
    }
  }
  EXINDEX = 0;
  CUREXFLAG = 0;
  pTable = pdfium::MakeUnique<CJBig2_HuffmanTable>(1);
  EXFLAGS.resize(SDNUMINSYMS + SDNUMNEWSYMS);
  num_ex_syms = 0;
  while (EXINDEX < SDNUMINSYMS + SDNUMNEWSYMS) {
    if (pHuffmanDecoder->DecodeAValue(pTable.get(), (int*)&EXRUNLENGTH) != 0)
      return nullptr;

    if (EXINDEX + EXRUNLENGTH > SDNUMINSYMS + SDNUMNEWSYMS)
      return nullptr;

    if (EXRUNLENGTH != 0) {
      for (I = EXINDEX; I < EXINDEX + EXRUNLENGTH; ++I) {
        if (CUREXFLAG)
          num_ex_syms++;

        EXFLAGS[I] = CUREXFLAG;
      }
    }
    EXINDEX = EXINDEX + EXRUNLENGTH;
    CUREXFLAG = !CUREXFLAG;
  }
  if (num_ex_syms > SDNUMEXSYMS)
    return nullptr;

  I = J = 0;
  for (I = 0; I < SDNUMINSYMS + SDNUMNEWSYMS; ++I) {
    if (!EXFLAGS[I] || J >= SDNUMEXSYMS)
      continue;
    if (I < SDNUMINSYMS) {
      pDict->AddImage(SDINSYMS[I]
                          ? pdfium::MakeUnique<CJBig2_Image>(*SDINSYMS[I])
                          : nullptr);
    } else {
      pDict->AddImage(std::move(SDNEWSYMS[I - SDNUMINSYMS]));
    }
    ++J;
  }
  return pDict;
}
