// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fgas/layout/cfx_char.h"

#include <algorithm>

#include "core/fxcrt/fx_extension.h"

namespace {

#ifndef NDEBUG
constexpr int32_t kBidiMaxLevel = 61;
#endif  // NDEBUG

#undef PACK_NIBBLES
#define PACK_NIBBLES(hi, lo) \
  ((static_cast<uint32_t>(hi) << 4) + static_cast<uint32_t>(lo))

enum FX_BIDIWEAKSTATE : uint8_t {
  FX_BWSxa = 0,
  FX_BWSxr,
  FX_BWSxl,
  FX_BWSao,
  FX_BWSro,
  FX_BWSlo,
  FX_BWSrt,
  FX_BWSlt,
  FX_BWScn,
  FX_BWSra,
  FX_BWSre,
  FX_BWSla,
  FX_BWSle,
  FX_BWSac,
  FX_BWSrc,
  FX_BWSrs,
  FX_BWSlc,
  FX_BWSls,
  FX_BWSret,
  FX_BWSlet
};

// NOTE: Range of FX_BIDICLASS prevents encoding all possible values in this
// manner, but the ones used manage to fit. Except that I suspect that 0xF
// was intended to be used as a sentinel, even though it also means kRLE.
// TODO(tsepez): pick a better representation.
enum FX_BIDIWEAKACTION : uint16_t {
  FX_BWAIX = 0x100,
  FX_BWAXX = 0x0F,
  FX_BWAxxx = 0xFF,
  FX_BWAxIx = 0x100 + FX_BWAxxx,
  FX_BWAxxN = PACK_NIBBLES(0x0F, FX_BIDICLASS::kON),
  FX_BWAxxE = PACK_NIBBLES(0x0F, FX_BIDICLASS::kEN),
  FX_BWAxxA = PACK_NIBBLES(0x0F, FX_BIDICLASS::kAN),
  FX_BWAxxR = PACK_NIBBLES(0x0F, FX_BIDICLASS::kR),
  FX_BWAxxL = PACK_NIBBLES(0x0F, FX_BIDICLASS::kL),
  FX_BWANxx = PACK_NIBBLES(FX_BIDICLASS::kON, 0x0F),
  FX_BWAAxx = PACK_NIBBLES(FX_BIDICLASS::kAN, 0x0F),
  FX_BWAExE = PACK_NIBBLES(FX_BIDICLASS::kEN, FX_BIDICLASS::kEN),
  FX_BWANIx = 0x100 + PACK_NIBBLES(FX_BIDICLASS::kON, 0x0F),
  FX_BWANxN = PACK_NIBBLES(FX_BIDICLASS::kON, FX_BIDICLASS::kON),
  FX_BWANxR = PACK_NIBBLES(FX_BIDICLASS::kON, FX_BIDICLASS::kR),
  FX_BWANxE = PACK_NIBBLES(FX_BIDICLASS::kON, FX_BIDICLASS::kEN),
  FX_BWAAxA = PACK_NIBBLES(FX_BIDICLASS::kAN, FX_BIDICLASS::kAN),
  FX_BWANxL = PACK_NIBBLES(FX_BIDICLASS::kON, FX_BIDICLASS::kL),
  FX_BWALxL = PACK_NIBBLES(FX_BIDICLASS::kL, FX_BIDICLASS::kL),
  FX_BWAxIL = 0x100 + PACK_NIBBLES(0x0F, FX_BIDICLASS::kL),
  FX_BWAAxR = PACK_NIBBLES(FX_BIDICLASS::kAN, FX_BIDICLASS::kR),
  FX_BWALxx = PACK_NIBBLES(FX_BIDICLASS::kL, 0x0F),
};

enum FX_BIDINEUTRALSTATE : uint8_t {
  FX_BNSr = 0,
  FX_BNSl,
  FX_BNSrn,
  FX_BNSln,
  FX_BNSa,
  FX_BNSna
};

enum FX_BIDINEUTRALACTION : uint16_t {
  // For placeholders in table.
  FX_BNAZero = 0,

  // Other values.
  FX_BNAnL = PACK_NIBBLES(0, FX_BIDICLASS::kL),
  FX_BNAEn = PACK_NIBBLES(FX_BIDICLASS::kAN, 0),
  FX_BNARn = PACK_NIBBLES(FX_BIDICLASS::kR, 0),
  FX_BNALn = PACK_NIBBLES(FX_BIDICLASS::kL, 0),
  FX_BNAIn = FX_BWAIX,
  FX_BNALnL = PACK_NIBBLES(FX_BIDICLASS::kL, FX_BIDICLASS::kL),
};
#undef PACK_NIBBLES

const FX_BIDICLASS kNTypes[] = {
    FX_BIDICLASS::kN,   FX_BIDICLASS::kL,   FX_BIDICLASS::kR,
    FX_BIDICLASS::kAN,  FX_BIDICLASS::kEN,  FX_BIDICLASS::kAL,
    FX_BIDICLASS::kNSM, FX_BIDICLASS::kCS,  FX_BIDICLASS::kES,
    FX_BIDICLASS::kET,  FX_BIDICLASS::kBN,  FX_BIDICLASS::kBN,
    FX_BIDICLASS::kN,   FX_BIDICLASS::kB,   FX_BIDICLASS::kRLO,
    FX_BIDICLASS::kRLE, FX_BIDICLASS::kLRO, FX_BIDICLASS::kLRE,
    FX_BIDICLASS::kPDF, FX_BIDICLASS::kON,
};

const FX_BIDIWEAKSTATE kWeakStates[20][10] = {
    {FX_BWSao, FX_BWSxl, FX_BWSxr, FX_BWScn, FX_BWScn, FX_BWSxa, FX_BWSxa,
     FX_BWSao, FX_BWSao, FX_BWSao},
    {FX_BWSro, FX_BWSxl, FX_BWSxr, FX_BWSra, FX_BWSre, FX_BWSxa, FX_BWSxr,
     FX_BWSro, FX_BWSro, FX_BWSrt},
    {FX_BWSlo, FX_BWSxl, FX_BWSxr, FX_BWSla, FX_BWSle, FX_BWSxa, FX_BWSxl,
     FX_BWSlo, FX_BWSlo, FX_BWSlt},
    {FX_BWSao, FX_BWSxl, FX_BWSxr, FX_BWScn, FX_BWScn, FX_BWSxa, FX_BWSao,
     FX_BWSao, FX_BWSao, FX_BWSao},
    {FX_BWSro, FX_BWSxl, FX_BWSxr, FX_BWSra, FX_BWSre, FX_BWSxa, FX_BWSro,
     FX_BWSro, FX_BWSro, FX_BWSrt},
    {FX_BWSlo, FX_BWSxl, FX_BWSxr, FX_BWSla, FX_BWSle, FX_BWSxa, FX_BWSlo,
     FX_BWSlo, FX_BWSlo, FX_BWSlt},
    {FX_BWSro, FX_BWSxl, FX_BWSxr, FX_BWSra, FX_BWSre, FX_BWSxa, FX_BWSrt,
     FX_BWSro, FX_BWSro, FX_BWSrt},
    {FX_BWSlo, FX_BWSxl, FX_BWSxr, FX_BWSla, FX_BWSle, FX_BWSxa, FX_BWSlt,
     FX_BWSlo, FX_BWSlo, FX_BWSlt},
    {FX_BWSao, FX_BWSxl, FX_BWSxr, FX_BWScn, FX_BWScn, FX_BWSxa, FX_BWScn,
     FX_BWSac, FX_BWSao, FX_BWSao},
    {FX_BWSro, FX_BWSxl, FX_BWSxr, FX_BWSra, FX_BWSre, FX_BWSxa, FX_BWSra,
     FX_BWSrc, FX_BWSro, FX_BWSrt},
    {FX_BWSro, FX_BWSxl, FX_BWSxr, FX_BWSra, FX_BWSre, FX_BWSxa, FX_BWSre,
     FX_BWSrs, FX_BWSrs, FX_BWSret},
    {FX_BWSlo, FX_BWSxl, FX_BWSxr, FX_BWSla, FX_BWSle, FX_BWSxa, FX_BWSla,
     FX_BWSlc, FX_BWSlo, FX_BWSlt},
    {FX_BWSlo, FX_BWSxl, FX_BWSxr, FX_BWSla, FX_BWSle, FX_BWSxa, FX_BWSle,
     FX_BWSls, FX_BWSls, FX_BWSlet},
    {FX_BWSao, FX_BWSxl, FX_BWSxr, FX_BWScn, FX_BWScn, FX_BWSxa, FX_BWSao,
     FX_BWSao, FX_BWSao, FX_BWSao},
    {FX_BWSro, FX_BWSxl, FX_BWSxr, FX_BWSra, FX_BWSre, FX_BWSxa, FX_BWSro,
     FX_BWSro, FX_BWSro, FX_BWSrt},
    {FX_BWSro, FX_BWSxl, FX_BWSxr, FX_BWSra, FX_BWSre, FX_BWSxa, FX_BWSro,
     FX_BWSro, FX_BWSro, FX_BWSrt},
    {FX_BWSlo, FX_BWSxl, FX_BWSxr, FX_BWSla, FX_BWSle, FX_BWSxa, FX_BWSlo,
     FX_BWSlo, FX_BWSlo, FX_BWSlt},
    {FX_BWSlo, FX_BWSxl, FX_BWSxr, FX_BWSla, FX_BWSle, FX_BWSxa, FX_BWSlo,
     FX_BWSlo, FX_BWSlo, FX_BWSlt},
    {FX_BWSro, FX_BWSxl, FX_BWSxr, FX_BWSra, FX_BWSre, FX_BWSxa, FX_BWSret,
     FX_BWSro, FX_BWSro, FX_BWSret},
    {FX_BWSlo, FX_BWSxl, FX_BWSxr, FX_BWSla, FX_BWSle, FX_BWSxa, FX_BWSlet,
     FX_BWSlo, FX_BWSlo, FX_BWSlet},
};

const FX_BIDIWEAKACTION kWeakActions[20][10] = {
    {FX_BWAxxx, FX_BWAxxx, FX_BWAxxx, FX_BWAxxx, FX_BWAxxA, FX_BWAxxR,
     FX_BWAxxR, FX_BWAxxN, FX_BWAxxN, FX_BWAxxN},
    {FX_BWAxxx, FX_BWAxxx, FX_BWAxxx, FX_BWAxxx, FX_BWAxxE, FX_BWAxxR,
     FX_BWAxxR, FX_BWAxxN, FX_BWAxxN, FX_BWAxIx},
    {FX_BWAxxx, FX_BWAxxx, FX_BWAxxx, FX_BWAxxx, FX_BWAxxL, FX_BWAxxR,
     FX_BWAxxL, FX_BWAxxN, FX_BWAxxN, FX_BWAxIx},
    {FX_BWAxxx, FX_BWAxxx, FX_BWAxxx, FX_BWAxxx, FX_BWAxxA, FX_BWAxxR,
     FX_BWAxxN, FX_BWAxxN, FX_BWAxxN, FX_BWAxxN},
    {FX_BWAxxx, FX_BWAxxx, FX_BWAxxx, FX_BWAxxx, FX_BWAxxE, FX_BWAxxR,
     FX_BWAxxN, FX_BWAxxN, FX_BWAxxN, FX_BWAxIx},
    {FX_BWAxxx, FX_BWAxxx, FX_BWAxxx, FX_BWAxxx, FX_BWAxxL, FX_BWAxxR,
     FX_BWAxxN, FX_BWAxxN, FX_BWAxxN, FX_BWAxIx},
    {FX_BWANxx, FX_BWANxx, FX_BWANxx, FX_BWANxx, FX_BWAExE, FX_BWANxR,
     FX_BWAxIx, FX_BWANxN, FX_BWANxN, FX_BWAxIx},
    {FX_BWANxx, FX_BWANxx, FX_BWANxx, FX_BWANxx, FX_BWALxL, FX_BWANxR,
     FX_BWAxIx, FX_BWANxN, FX_BWANxN, FX_BWAxIx},
    {FX_BWAxxx, FX_BWAxxx, FX_BWAxxx, FX_BWAxxx, FX_BWAxxA, FX_BWAxxR,
     FX_BWAxxA, FX_BWAxIx, FX_BWAxxN, FX_BWAxxN},
    {FX_BWAxxx, FX_BWAxxx, FX_BWAxxx, FX_BWAxxx, FX_BWAxxE, FX_BWAxxR,
     FX_BWAxxA, FX_BWAxIx, FX_BWAxxN, FX_BWAxIx},
    {FX_BWAxxx, FX_BWAxxx, FX_BWAxxx, FX_BWAxxx, FX_BWAxxE, FX_BWAxxR,
     FX_BWAxxE, FX_BWAxIx, FX_BWAxIx, FX_BWAxxE},
    {FX_BWAxxx, FX_BWAxxx, FX_BWAxxx, FX_BWAxxx, FX_BWAxxL, FX_BWAxxR,
     FX_BWAxxA, FX_BWAxIx, FX_BWAxxN, FX_BWAxIx},
    {FX_BWAxxx, FX_BWAxxx, FX_BWAxxx, FX_BWAxxx, FX_BWAxxL, FX_BWAxxR,
     FX_BWAxxL, FX_BWAxIx, FX_BWAxIx, FX_BWAxxL},
    {FX_BWANxx, FX_BWANxx, FX_BWANxx, FX_BWAAxx, FX_BWAAxA, FX_BWANxR,
     FX_BWANxN, FX_BWANxN, FX_BWANxN, FX_BWANxN},
    {FX_BWANxx, FX_BWANxx, FX_BWANxx, FX_BWAAxx, FX_BWANxE, FX_BWANxR,
     FX_BWANxN, FX_BWANxN, FX_BWANxN, FX_BWANIx},
    {FX_BWANxx, FX_BWANxx, FX_BWANxx, FX_BWANxx, FX_BWAExE, FX_BWANxR,
     FX_BWANxN, FX_BWANxN, FX_BWANxN, FX_BWANIx},
    {FX_BWANxx, FX_BWANxx, FX_BWANxx, FX_BWAAxx, FX_BWANxL, FX_BWANxR,
     FX_BWANxN, FX_BWANxN, FX_BWANxN, FX_BWANIx},
    {FX_BWANxx, FX_BWANxx, FX_BWANxx, FX_BWANxx, FX_BWALxL, FX_BWANxR,
     FX_BWANxN, FX_BWANxN, FX_BWANxN, FX_BWANIx},
    {FX_BWAxxx, FX_BWAxxx, FX_BWAxxx, FX_BWAxxx, FX_BWAxxE, FX_BWAxxR,
     FX_BWAxxE, FX_BWAxxN, FX_BWAxxN, FX_BWAxxE},
    {FX_BWAxxx, FX_BWAxxx, FX_BWAxxx, FX_BWAxxx, FX_BWAxxL, FX_BWAxxR,
     FX_BWAxxL, FX_BWAxxN, FX_BWAxxN, FX_BWAxxL},
};

const FX_BIDINEUTRALSTATE kNeutralStates[6][5] = {
    {FX_BNSrn, FX_BNSl, FX_BNSr, FX_BNSr, FX_BNSr},
    {FX_BNSln, FX_BNSl, FX_BNSr, FX_BNSa, FX_BNSl},
    {FX_BNSrn, FX_BNSl, FX_BNSr, FX_BNSr, FX_BNSr},
    {FX_BNSln, FX_BNSl, FX_BNSr, FX_BNSa, FX_BNSl},
    {FX_BNSna, FX_BNSl, FX_BNSr, FX_BNSa, FX_BNSl},
    {FX_BNSna, FX_BNSl, FX_BNSr, FX_BNSa, FX_BNSl},
};

const FX_BIDINEUTRALACTION kNeutralActions[6][5] = {
    {FX_BNAIn, FX_BNAZero, FX_BNAZero, FX_BNAZero, FX_BNAZero},
    {FX_BNAIn, FX_BNAZero, FX_BNAZero, FX_BNAZero, FX_BNAnL},
    {FX_BNAIn, FX_BNAEn, FX_BNARn, FX_BNARn, FX_BNARn},
    {FX_BNAIn, FX_BNALn, FX_BNAEn, FX_BNAEn, FX_BNALnL},
    {FX_BNAIn, FX_BNAZero, FX_BNAZero, FX_BNAZero, FX_BNAnL},
    {FX_BNAIn, FX_BNAEn, FX_BNARn, FX_BNARn, FX_BNAEn},
};

const uint8_t kAddLevel[2][4] = {
    {0, 1, 2, 2},
    {1, 0, 1, 1},
};

FX_BIDICLASS Direction(int32_t val) {
  return FX_IsOdd(val) ? FX_BIDICLASS::kR : FX_BIDICLASS::kL;
}

FX_BIDICLASS GetDeferredType(int32_t val) {
  return static_cast<FX_BIDICLASS>((val >> 4) & 0x0F);
}

FX_BIDICLASS GetResolvedType(int32_t val) {
  return static_cast<FX_BIDICLASS>(val & 0x0F);
}

FX_BIDICLASS GetDeferredNeutrals(int32_t iAction, int32_t iLevel) {
  FX_BIDICLASS eClass = GetDeferredType(iAction);
  return eClass == FX_BIDICLASS::kAN ? Direction(iLevel) : eClass;
}

FX_BIDICLASS GetResolvedNeutrals(int32_t iAction) {
  return GetResolvedType(iAction);
}

FX_BIDIWEAKSTATE GetWeakState(FX_BIDIWEAKSTATE eState, FX_BIDICLASS eClass) {
  ASSERT(static_cast<size_t>(eState) < FX_ArraySize(kWeakStates));
  ASSERT(static_cast<size_t>(eClass) < FX_ArraySize(kWeakStates[0]));
  return kWeakStates[static_cast<size_t>(eState)][static_cast<size_t>(eClass)];
}

FX_BIDIWEAKACTION GetWeakAction(FX_BIDIWEAKSTATE eState, FX_BIDICLASS eClass) {
  ASSERT(static_cast<size_t>(eState) < FX_ArraySize(kWeakActions));
  ASSERT(static_cast<size_t>(eClass) < FX_ArraySize(kWeakActions[0]));
  return kWeakActions[static_cast<size_t>(eState)][static_cast<size_t>(eClass)];
}

FX_BIDINEUTRALSTATE GetNeutralState(FX_BIDINEUTRALSTATE eState,
                                    FX_BIDICLASS eClass) {
  ASSERT(static_cast<size_t>(eState) < FX_ArraySize(kNeutralStates));
  ASSERT(static_cast<size_t>(eClass) < FX_ArraySize(kNeutralStates[0]));
  return kNeutralStates[static_cast<size_t>(eState)]
                       [static_cast<size_t>(eClass)];
}

FX_BIDINEUTRALACTION GetNeutralAction(FX_BIDINEUTRALSTATE eState,
                                      FX_BIDICLASS eClass) {
  ASSERT(static_cast<size_t>(eState) < FX_ArraySize(kNeutralActions));
  ASSERT(static_cast<size_t>(eClass) < FX_ArraySize(kNeutralActions[0]));
  return kNeutralActions[static_cast<size_t>(eState)]
                        [static_cast<size_t>(eClass)];
}

void ReverseString(std::vector<CFX_Char>* chars, size_t iStart, size_t iCount) {
  ASSERT(pdfium::IndexInBounds(*chars, iStart));
  ASSERT(iStart + iCount <= chars->size());

  std::reverse(chars->begin() + iStart, chars->begin() + iStart + iCount);
}

void SetDeferredRunClass(std::vector<CFX_Char>* chars,
                         size_t iStart,
                         size_t iCount,
                         FX_BIDICLASS eValue) {
  ASSERT(iStart <= chars->size());
  ASSERT(iStart >= iCount);

  size_t iLast = iStart - iCount;
  for (size_t i = iStart; i > iLast; --i)
    (*chars)[i - 1].m_iBidiClass = eValue;
}

void SetDeferredRunLevel(std::vector<CFX_Char>* chars,
                         size_t iStart,
                         size_t iCount,
                         int32_t iValue) {
  ASSERT(iStart <= chars->size());
  ASSERT(iStart >= iCount);

  size_t iLast = iStart - iCount;
  for (size_t i = iStart; i > iLast; --i)
    (*chars)[i - 1].m_iBidiLevel = static_cast<int16_t>(iValue);
}

void Classify(std::vector<CFX_Char>* chars, size_t iCount) {
  for (size_t i = 0; i < iCount; ++i) {
    CFX_Char& cur = (*chars)[i];
    cur.m_iBidiClass = FX_GetBidiClass(cur.char_code());
  }
}

void ClassifyWithTransform(std::vector<CFX_Char>* chars, size_t iCount) {
  for (size_t i = 0; i < iCount; ++i) {
    CFX_Char& cur = (*chars)[i];
    cur.m_iBidiClass =
        kNTypes[static_cast<size_t>(FX_GetBidiClass(cur.char_code()))];
  }
}

void ResolveExplicit(std::vector<CFX_Char>* chars, size_t iCount) {
  for (size_t i = 0; i < iCount; ++i)
    (*chars)[i].m_iBidiLevel = 0;
}

void ResolveWeak(std::vector<CFX_Char>* chars, size_t iCount) {
  if (iCount <= 1)
    return;
  --iCount;

  int32_t iLevelCur = 0;
  size_t iNum = 0;
  FX_BIDIWEAKSTATE eState = FX_BWSxl;
  FX_BIDICLASS eClsCur;
  FX_BIDICLASS eClsRun;
  FX_BIDICLASS eClsNew;
  size_t i = 0;
  for (; i <= iCount; ++i) {
    CFX_Char* pTC = &(*chars)[i];
    eClsCur = pTC->m_iBidiClass;
    if (eClsCur == FX_BIDICLASS::kBN) {
      pTC->m_iBidiLevel = (int16_t)iLevelCur;
      if (i == iCount && iLevelCur != 0) {
        eClsCur = Direction(iLevelCur);
        pTC->m_iBidiClass = eClsCur;
      } else if (i < iCount) {
        CFX_Char* pTCNext = &(*chars)[i + 1];
        int32_t iLevelNext, iLevelNew;
        eClsNew = pTCNext->m_iBidiClass;
        iLevelNext = pTCNext->m_iBidiLevel;
        if (eClsNew != FX_BIDICLASS::kBN && iLevelCur != iLevelNext) {
          iLevelNew = std::max(iLevelNext, iLevelCur);
          pTC->m_iBidiLevel = static_cast<int16_t>(iLevelNew);
          eClsCur = Direction(iLevelNew);
          pTC->m_iBidiClass = eClsCur;
          iLevelCur = iLevelNext;
        } else {
          if (iNum > 0)
            ++iNum;
          continue;
        }
      } else {
        if (iNum > 0)
          ++iNum;
        continue;
      }
    }
    if (eClsCur > FX_BIDICLASS::kBN)
      continue;

    FX_BIDIWEAKACTION eAction = GetWeakAction(eState, eClsCur);
    eClsRun = GetDeferredType(eAction);
    if (eClsRun != static_cast<FX_BIDICLASS>(0xF) && iNum > 0) {
      SetDeferredRunClass(chars, i, iNum, eClsRun);
      iNum = 0;
    }
    eClsNew = GetResolvedType(eAction);
    if (eClsNew != static_cast<FX_BIDICLASS>(0xF))
      pTC->m_iBidiClass = eClsNew;
    if (FX_BWAIX & eAction)
      ++iNum;

    eState = GetWeakState(eState, eClsCur);
  }
  if (iNum == 0)
    return;

  eClsCur = Direction(0);
  eClsRun = GetDeferredType(GetWeakAction(eState, eClsCur));
  if (eClsRun != static_cast<FX_BIDICLASS>(0xF))
    SetDeferredRunClass(chars, i, iNum, eClsRun);
}

void ResolveNeutrals(std::vector<CFX_Char>* chars, size_t iCount) {
  if (iCount <= 1)
    return;
  --iCount;

  CFX_Char* pTC;
  int32_t iLevel = 0;
  size_t i = 0;
  size_t iNum = 0;
  FX_BIDINEUTRALSTATE eState = FX_BNSl;
  FX_BIDICLASS eClsCur;
  FX_BIDICLASS eClsRun;
  FX_BIDICLASS eClsNew;
  for (; i <= iCount; ++i) {
    pTC = &(*chars)[i];
    eClsCur = pTC->m_iBidiClass;
    if (eClsCur == FX_BIDICLASS::kBN) {
      if (iNum)
        ++iNum;
      continue;
    }
    if (eClsCur >= FX_BIDICLASS::kAL)
      continue;

    FX_BIDINEUTRALACTION eAction = GetNeutralAction(eState, eClsCur);
    eClsRun = GetDeferredNeutrals(eAction, iLevel);
    if (eClsRun != FX_BIDICLASS::kN && iNum > 0) {
      SetDeferredRunClass(chars, i, iNum, eClsRun);
      iNum = 0;
    }

    eClsNew = GetResolvedNeutrals(eAction);
    if (eClsNew != FX_BIDICLASS::kN)
      pTC->m_iBidiClass = eClsNew;
    if (FX_BNAIn & eAction)
      ++iNum;

    eState = GetNeutralState(eState, eClsCur);
    iLevel = pTC->m_iBidiLevel;
  }
  if (iNum == 0)
    return;

  eClsCur = Direction(iLevel);
  eClsRun = GetDeferredNeutrals(GetNeutralAction(eState, eClsCur), iLevel);
  if (eClsRun != FX_BIDICLASS::kN)
    SetDeferredRunClass(chars, i, iNum, eClsRun);
}

void ResolveImplicit(std::vector<CFX_Char>* chars, size_t iCount) {
  for (size_t i = 0; i < iCount; ++i) {
    FX_BIDICLASS eCls = (*chars)[i].m_iBidiClass;
    if (eCls == FX_BIDICLASS::kBN || eCls <= FX_BIDICLASS::kON ||
        eCls >= FX_BIDICLASS::kAL) {
      continue;
    }
    (*chars)[i].m_iBidiLevel += kAddLevel[FX_IsOdd((*chars)[i].m_iBidiLevel)]
                                         [static_cast<size_t>(eCls) - 1];
  }
}

void ResolveWhitespace(std::vector<CFX_Char>* chars, size_t iCount) {
  if (iCount <= 1)
    return;
  iCount--;

  int32_t iLevel = 0;
  size_t i = 0;
  size_t iNum = 0;
  for (; i <= iCount; ++i) {
    switch (static_cast<FX_BIDICLASS>((*chars)[i].m_iBidiClass)) {
      case FX_BIDICLASS::kWS:
        ++iNum;
        break;
      case FX_BIDICLASS::kRLE:
      case FX_BIDICLASS::kLRE:
      case FX_BIDICLASS::kLRO:
      case FX_BIDICLASS::kRLO:
      case FX_BIDICLASS::kPDF:
      case FX_BIDICLASS::kBN:
        (*chars)[i].m_iBidiLevel = static_cast<int16_t>(iLevel);
        ++iNum;
        break;
      case FX_BIDICLASS::kS:
      case FX_BIDICLASS::kB:
        if (iNum > 0)
          SetDeferredRunLevel(chars, i, iNum, 0);

        (*chars)[i].m_iBidiLevel = 0;
        iNum = 0;
        break;
      default:
        iNum = 0;
        break;
    }
    iLevel = (*chars)[i].m_iBidiLevel;
  }
  if (iNum > 0)
    SetDeferredRunLevel(chars, i, iNum, 0);
}

size_t ReorderLevel(std::vector<CFX_Char>* chars,
                    size_t iCount,
                    int32_t iBaseLevel,
                    size_t iStart,
                    bool bReverse) {
  ASSERT(iBaseLevel >= 0);
  ASSERT(iBaseLevel <= kBidiMaxLevel);
  ASSERT(iStart < iCount);

  if (iCount < 1)
    return 0;

  bReverse = bReverse || FX_IsOdd(iBaseLevel);
  size_t i = iStart;
  for (; i < iCount; ++i) {
    int32_t iLevel = (*chars)[i].m_iBidiLevel;
    if (iLevel == iBaseLevel)
      continue;
    if (iLevel < iBaseLevel)
      break;

    i += ReorderLevel(chars, iCount, iBaseLevel + 1, i, bReverse) - 1;
  }

  size_t iNum = i - iStart;
  if (bReverse && iNum > 1)
    ReverseString(chars, iStart, iNum);

  return iNum;
}

void Reorder(std::vector<CFX_Char>* chars, size_t iCount) {
  for (size_t i = 0; i < iCount;)
    i += ReorderLevel(chars, iCount, 0, i, false);
}

void Position(std::vector<CFX_Char>* chars, size_t iCount) {
  for (size_t i = 0; i < iCount; ++i) {
    if ((*chars)[i].m_iBidiPos > iCount)
      continue;

    (*chars)[(*chars)[i].m_iBidiPos].m_iBidiOrder = i;
  }
}

}  // namespace

// static
void CFX_Char::BidiLine(std::vector<CFX_Char>* chars, size_t iCount) {
  ASSERT(iCount <= chars->size());
  if (iCount < 2)
    return;

  ClassifyWithTransform(chars, iCount);
  ResolveExplicit(chars, iCount);
  ResolveWeak(chars, iCount);
  ResolveNeutrals(chars, iCount);
  ResolveImplicit(chars, iCount);
  Classify(chars, iCount);
  ResolveWhitespace(chars, iCount);
  Reorder(chars, iCount);
  Position(chars, iCount);
}

CFX_Char::CFX_Char(uint16_t wCharCode) : CFX_Char(wCharCode, 100, 100) {}

CFX_Char::CFX_Char(uint16_t wCharCode,
                   int32_t iHorizontalScale,
                   int32_t iVerticalScale)
    : m_wCharCode(wCharCode),
      m_iHorizontalScale(iHorizontalScale),
      m_iVerticalScale(iVerticalScale) {}

CFX_Char::CFX_Char(const CFX_Char& other) = default;

CFX_Char::~CFX_Char() = default;

FX_CHARTYPE CFX_Char::GetCharType() const {
  return FX_GetCharType(m_wCharCode);
}
