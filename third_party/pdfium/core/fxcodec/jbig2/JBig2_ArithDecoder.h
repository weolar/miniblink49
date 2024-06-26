// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCODEC_JBIG2_JBIG2_ARITHDECODER_H_
#define CORE_FXCODEC_JBIG2_JBIG2_ARITHDECODER_H_

#include <stdint.h>

#include "core/fxcrt/unowned_ptr.h"

class CJBig2_BitStream;
struct JBig2ArithQe;

class JBig2ArithCtx {
 public:
  struct JBig2ArithQe {
    uint16_t Qe;
    uint8_t NMPS;
    uint8_t NLPS;
    bool bSwitch;
  };

  JBig2ArithCtx();

  int DecodeNLPS(const JBig2ArithQe& qe);
  int DecodeNMPS(const JBig2ArithQe& qe);

  unsigned int MPS() const { return m_MPS ? 1 : 0; }
  unsigned int I() const { return m_I; }

 private:
  bool m_MPS = 0;
  unsigned int m_I = 0;
};

class CJBig2_ArithDecoder {
 public:
  explicit CJBig2_ArithDecoder(CJBig2_BitStream* pStream);
  ~CJBig2_ArithDecoder();

  int Decode(JBig2ArithCtx* pCX);

  bool IsComplete() const { return m_Complete; }

 private:
  void BYTEIN();
  void ReadValueA();

  bool m_Complete;
  bool m_FinishedStream;
  uint8_t m_B;
  unsigned int m_C;
  unsigned int m_A;
  unsigned int m_CT;
  UnownedPtr<CJBig2_BitStream> const m_pStream;
};

#endif  // CORE_FXCODEC_JBIG2_JBIG2_ARITHDECODER_H_
