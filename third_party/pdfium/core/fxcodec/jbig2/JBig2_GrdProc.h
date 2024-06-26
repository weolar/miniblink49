// Copyright 2015 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCODEC_JBIG2_JBIG2_GRDPROC_H_
#define CORE_FXCODEC_JBIG2_JBIG2_GRDPROC_H_

#include <memory>

#include "core/fxcodec/fx_codec_def.h"
#include "core/fxcrt/fx_coordinates.h"
#include "core/fxcrt/fx_system.h"
#include "core/fxcrt/unowned_ptr.h"

class CJBig2_ArithDecoder;
class CJBig2_BitStream;
class CJBig2_Image;
class JBig2ArithCtx;
class PauseIndicatorIface;

class CJBig2_GRDProc {
 public:
  class ProgressiveArithDecodeState {
   public:
    ProgressiveArithDecodeState();
    ~ProgressiveArithDecodeState();

    std::unique_ptr<CJBig2_Image>* pImage;
    UnownedPtr<CJBig2_ArithDecoder> pArithDecoder;
    UnownedPtr<JBig2ArithCtx> gbContext;
    UnownedPtr<PauseIndicatorIface> pPause;
  };

  CJBig2_GRDProc();
  ~CJBig2_GRDProc();

  std::unique_ptr<CJBig2_Image> DecodeArith(CJBig2_ArithDecoder* pArithDecoder,
                                            JBig2ArithCtx* gbContext);

  FXCODEC_STATUS StartDecodeArith(ProgressiveArithDecodeState* pState);
  FXCODEC_STATUS StartDecodeMMR(std::unique_ptr<CJBig2_Image>* pImage,
                                CJBig2_BitStream* pStream);
  FXCODEC_STATUS ContinueDecode(ProgressiveArithDecodeState* pState);
  const FX_RECT& GetReplaceRect() const { return m_ReplaceRect; }

  bool MMR;
  bool TPGDON;
  bool USESKIP;
  uint8_t GBTEMPLATE;
  uint32_t GBW;
  uint32_t GBH;
  UnownedPtr<CJBig2_Image> SKIP;
  int8_t GBAT[8];

 private:
  bool UseTemplate0Opt3() const;
  bool UseTemplate1Opt3() const;
  bool UseTemplate23Opt3() const;

  FXCODEC_STATUS ProgressiveDecodeArith(ProgressiveArithDecodeState* pState);
  FXCODEC_STATUS ProgressiveDecodeArithTemplate0Opt3(
      ProgressiveArithDecodeState* pState);
  FXCODEC_STATUS ProgressiveDecodeArithTemplate0Unopt(
      ProgressiveArithDecodeState* pState);
  FXCODEC_STATUS ProgressiveDecodeArithTemplate1Opt3(
      ProgressiveArithDecodeState* pState);
  FXCODEC_STATUS ProgressiveDecodeArithTemplate1Unopt(
      ProgressiveArithDecodeState* pState);
  FXCODEC_STATUS ProgressiveDecodeArithTemplate2Opt3(
      ProgressiveArithDecodeState* pState);
  FXCODEC_STATUS ProgressiveDecodeArithTemplate2Unopt(
      ProgressiveArithDecodeState* pState);
  FXCODEC_STATUS ProgressiveDecodeArithTemplate3Opt3(
      ProgressiveArithDecodeState* pState);
  FXCODEC_STATUS ProgressiveDecodeArithTemplate3Unopt(
      ProgressiveArithDecodeState* pState);

  std::unique_ptr<CJBig2_Image> DecodeArithOpt3(
      CJBig2_ArithDecoder* pArithDecoder,
      JBig2ArithCtx* gbContext,
      int OPT);
  std::unique_ptr<CJBig2_Image> DecodeArithTemplateUnopt(
      CJBig2_ArithDecoder* pArithDecoder,
      JBig2ArithCtx* gbContext,
      int UNOPT);
  std::unique_ptr<CJBig2_Image> DecodeArithTemplate3Opt3(
      CJBig2_ArithDecoder* pArithDecoder,
      JBig2ArithCtx* gbContext);
  std::unique_ptr<CJBig2_Image> DecodeArithTemplate3Unopt(
      CJBig2_ArithDecoder* pArithDecoder,
      JBig2ArithCtx* gbContext);

  uint32_t m_loopIndex = 0;
  uint8_t* m_pLine = nullptr;
  FXCODEC_STATUS m_ProssiveStatus;
  uint16_t m_DecodeType = 0;
  int m_LTP = 0;
  FX_RECT m_ReplaceRect;
};

#endif  // CORE_FXCODEC_JBIG2_JBIG2_GRDPROC_H_
