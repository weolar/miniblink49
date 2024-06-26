// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcodec/jbig2/JBig2_HuffmanDecoder.h"

#include "core/fxcodec/jbig2/JBig2_Define.h"
#include "core/fxcrt/fx_safe_types.h"

CJBig2_HuffmanDecoder::CJBig2_HuffmanDecoder(CJBig2_BitStream* pStream)
    : m_pStream(pStream) {}

CJBig2_HuffmanDecoder::~CJBig2_HuffmanDecoder() {}

int CJBig2_HuffmanDecoder::DecodeAValue(const CJBig2_HuffmanTable* pTable,
                                        int* nResult) {
  FX_SAFE_INT32 nSafeVal = 0;
  int nBits = 0;
  while (1) {
    uint32_t nTmp;
    if (m_pStream->read1Bit(&nTmp) == -1)
      break;

    nSafeVal <<= 1;
    if (!nSafeVal.IsValid())
      break;

    nSafeVal |= nTmp;
    ++nBits;
    const int32_t nVal = nSafeVal.ValueOrDie();
    for (uint32_t i = 0; i < pTable->Size(); ++i) {
      const JBig2HuffmanCode& code = pTable->GetCODES()[i];
      if (code.codelen != nBits || code.code != nVal)
        continue;

      if (pTable->IsHTOOB() && i == pTable->Size() - 1)
        return JBIG2_OOB;

      if (m_pStream->readNBits(pTable->GetRANGELEN()[i], &nTmp) == -1)
        return -1;

      uint32_t offset = pTable->IsHTOOB() ? 3 : 2;
      if (i == pTable->Size() - offset)
        *nResult = pTable->GetRANGELOW()[i] - nTmp;
      else
        *nResult = pTable->GetRANGELOW()[i] + nTmp;
      return 0;
    }
  }
  return -1;
}
