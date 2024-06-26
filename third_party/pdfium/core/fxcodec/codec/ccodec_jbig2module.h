// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCODEC_CODEC_CCODEC_JBIG2MODULE_H_
#define CORE_FXCODEC_CODEC_CCODEC_JBIG2MODULE_H_

#include <memory>

#include "core/fxcodec/fx_codec_def.h"
#include "core/fxcrt/retain_ptr.h"

class CJBig2_Context;
class CJBig2_Image;
class CPDF_StreamAcc;
class PauseIndicatorIface;
class JBig2_DocumentContext;

class CCodec_Jbig2Context {
 public:
  CCodec_Jbig2Context();
  ~CCodec_Jbig2Context();

  uint32_t m_width;
  uint32_t m_height;
  RetainPtr<CPDF_StreamAcc> m_pGlobalStream;
  RetainPtr<CPDF_StreamAcc> m_pSrcStream;
  uint8_t* m_dest_buf;
  uint32_t m_dest_pitch;
  std::unique_ptr<CJBig2_Context> m_pContext;
};

class CCodec_Jbig2Module {
 public:
  CCodec_Jbig2Module() {}
  ~CCodec_Jbig2Module();

  FXCODEC_STATUS StartDecode(
      CCodec_Jbig2Context* pJbig2Context,
      std::unique_ptr<JBig2_DocumentContext>* pContextHolder,
      uint32_t width,
      uint32_t height,
      const RetainPtr<CPDF_StreamAcc>& src_stream,
      const RetainPtr<CPDF_StreamAcc>& global_stream,
      uint8_t* dest_buf,
      uint32_t dest_pitch,
      PauseIndicatorIface* pPause);
  FXCODEC_STATUS ContinueDecode(CCodec_Jbig2Context* pJbig2Context,
                                PauseIndicatorIface* pPause);

 private:
  FXCODEC_STATUS Decode(CCodec_Jbig2Context* pJbig2Context,
                        bool decode_success);
};

#endif  // CORE_FXCODEC_CODEC_CCODEC_JBIG2MODULE_H_
