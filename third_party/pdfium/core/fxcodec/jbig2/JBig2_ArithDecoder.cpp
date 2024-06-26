// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcodec/jbig2/JBig2_ArithDecoder.h"

#include "core/fxcodec/jbig2/JBig2_BitStream.h"
#include "core/fxcrt/fx_memory.h"

namespace {

const JBig2ArithCtx::JBig2ArithQe kQeTable[] = {
    // Stupid hack to keep clang-format from reformatting this badly.
    {0x5601, 1, 1, true},    {0x3401, 2, 6, false},   {0x1801, 3, 9, false},
    {0x0AC1, 4, 12, false},  {0x0521, 5, 29, false},  {0x0221, 38, 33, false},
    {0x5601, 7, 6, true},    {0x5401, 8, 14, false},  {0x4801, 9, 14, false},
    {0x3801, 10, 14, false}, {0x3001, 11, 17, false}, {0x2401, 12, 18, false},
    {0x1C01, 13, 20, false}, {0x1601, 29, 21, false}, {0x5601, 15, 14, true},
    {0x5401, 16, 14, false}, {0x5101, 17, 15, false}, {0x4801, 18, 16, false},
    {0x3801, 19, 17, false}, {0x3401, 20, 18, false}, {0x3001, 21, 19, false},
    {0x2801, 22, 19, false}, {0x2401, 23, 20, false}, {0x2201, 24, 21, false},
    {0x1C01, 25, 22, false}, {0x1801, 26, 23, false}, {0x1601, 27, 24, false},
    {0x1401, 28, 25, false}, {0x1201, 29, 26, false}, {0x1101, 30, 27, false},
    {0x0AC1, 31, 28, false}, {0x09C1, 32, 29, false}, {0x08A1, 33, 30, false},
    {0x0521, 34, 31, false}, {0x0441, 35, 32, false}, {0x02A1, 36, 33, false},
    {0x0221, 37, 34, false}, {0x0141, 38, 35, false}, {0x0111, 39, 36, false},
    {0x0085, 40, 37, false}, {0x0049, 41, 38, false}, {0x0025, 42, 39, false},
    {0x0015, 43, 40, false}, {0x0009, 44, 41, false}, {0x0005, 45, 42, false},
    {0x0001, 45, 43, false}, {0x5601, 46, 46, false}};

const unsigned int kDefaultAValue = 0x8000;

}  // namespace

JBig2ArithCtx::JBig2ArithCtx() = default;

int JBig2ArithCtx::DecodeNLPS(const JBig2ArithQe& qe) {
  bool D = !m_MPS;
  if (qe.bSwitch)
    m_MPS = !m_MPS;
  m_I = qe.NLPS;
  ASSERT(m_I < FX_ArraySize(kQeTable));
  return D;
}

int JBig2ArithCtx::DecodeNMPS(const JBig2ArithQe& qe) {
  m_I = qe.NMPS;
  ASSERT(m_I < FX_ArraySize(kQeTable));
  return MPS();
}

CJBig2_ArithDecoder::CJBig2_ArithDecoder(CJBig2_BitStream* pStream)
    : m_Complete(false), m_FinishedStream(false), m_pStream(pStream) {
  m_B = m_pStream->getCurByte_arith();
  m_C = (m_B ^ 0xff) << 16;
  BYTEIN();
  m_C = m_C << 7;
  m_CT = m_CT - 7;
  m_A = kDefaultAValue;
}

CJBig2_ArithDecoder::~CJBig2_ArithDecoder() {}

int CJBig2_ArithDecoder::Decode(JBig2ArithCtx* pCX) {
  ASSERT(pCX);
  ASSERT(pCX->I() < FX_ArraySize(kQeTable));

  const JBig2ArithCtx::JBig2ArithQe& qe = kQeTable[pCX->I()];
  m_A -= qe.Qe;
  if ((m_C >> 16) < m_A) {
    if (m_A & kDefaultAValue)
      return pCX->MPS();

    const int D = m_A < qe.Qe ? pCX->DecodeNLPS(qe) : pCX->DecodeNMPS(qe);
    ReadValueA();
    return D;
  }

  m_C -= m_A << 16;
  const int D = m_A < qe.Qe ? pCX->DecodeNMPS(qe) : pCX->DecodeNLPS(qe);
  m_A = qe.Qe;
  ReadValueA();
  return D;
}

void CJBig2_ArithDecoder::BYTEIN() {
  unsigned char B1;
  if (m_B == 0xff) {
    B1 = m_pStream->getNextByte_arith();
    if (B1 > 0x8f) {
      m_CT = 8;
      // If we are here, it means that we have finished decoding data (see JBIG2
      // spec, Section E.3.4). If we arrive here a second time, we're looping,
      // so complete decoding.
      if (m_FinishedStream)
        m_Complete = true;
      m_FinishedStream = true;
    } else {
      m_pStream->incByteIdx();
      m_B = B1;
      m_C = m_C + 0xfe00 - (m_B << 9);
      m_CT = 7;
    }
  } else {
    m_pStream->incByteIdx();
    m_B = m_pStream->getCurByte_arith();
    m_C = m_C + 0xff00 - (m_B << 8);
    m_CT = 8;
  }

  if (!m_pStream->IsInBounds())
    m_Complete = true;
}

void CJBig2_ArithDecoder::ReadValueA() {
  do {
    if (m_CT == 0)
      BYTEIN();
    m_A <<= 1;
    m_C <<= 1;
    --m_CT;
  } while ((m_A & kDefaultAValue) == 0);
}
