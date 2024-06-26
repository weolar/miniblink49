// Copyright 2015 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCODEC_JBIG2_JBIG2_PDDPROC_H_
#define CORE_FXCODEC_JBIG2_JBIG2_PDDPROC_H_

#include <memory>

#include "core/fxcrt/fx_system.h"

class CJBig2_ArithDecoder;
class CJBig2_BitStream;
class CJBig2_GRDProc;
class CJBig2_PatternDict;
class JBig2ArithCtx;
class PauseIndicatorIface;

class CJBig2_PDDProc {
 public:
  std::unique_ptr<CJBig2_PatternDict> DecodeArith(
      CJBig2_ArithDecoder* pArithDecoder,
      JBig2ArithCtx* gbContext,
      PauseIndicatorIface* pPause);

  std::unique_ptr<CJBig2_PatternDict> DecodeMMR(CJBig2_BitStream* pStream);

  bool HDMMR;
  uint8_t HDPW;
  uint8_t HDPH;
  uint32_t GRAYMAX;
  uint8_t HDTEMPLATE;

 private:
  std::unique_ptr<CJBig2_GRDProc> CreateGRDProc();
};

#endif  // CORE_FXCODEC_JBIG2_JBIG2_PDDPROC_H_
