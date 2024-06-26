// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCODEC_CODEC_CCODEC_PNGMODULE_H_
#define CORE_FXCODEC_CODEC_CCODEC_PNGMODULE_H_

#include <memory>

#include "core/fxcodec/codec/codec_module_iface.h"

class CCodec_PngModule final : public CodecModuleIface {
 public:
  class Delegate {
   public:
    virtual bool PngReadHeader(int width,
                               int height,
                               int bpc,
                               int pass,
                               int* color_type,
                               double* gamma) = 0;

    // Returns true on success. |pSrcBuf| will be set if this succeeds.
    // |pSrcBuf| does not take ownership of the buffer.
    virtual bool PngAskScanlineBuf(int line, uint8_t** pSrcBuf) = 0;

    virtual void PngFillScanlineBufCompleted(int pass, int line) = 0;
  };

  // CodecModuleIface:
  FX_FILESIZE GetAvailInput(Context* pContext) const override;
  bool Input(Context* pContext,
             RetainPtr<CFX_CodecMemory> codec_memory,
             CFX_DIBAttribute* pAttribute) override;

  std::unique_ptr<Context> Start(Delegate* pDelegate);
};

#endif  // CORE_FXCODEC_CODEC_CCODEC_PNGMODULE_H_
