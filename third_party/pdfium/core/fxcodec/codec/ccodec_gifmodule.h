// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCODEC_CODEC_CCODEC_GIFMODULE_H_
#define CORE_FXCODEC_CODEC_CCODEC_GIFMODULE_H_

#include <memory>
#include <utility>

#include "core/fxcodec/codec/codec_module_iface.h"
#include "core/fxcodec/gif/cfx_gif.h"
#include "core/fxcrt/fx_coordinates.h"

class CFX_DIBAttribute;

class CCodec_GifModule final : public CodecModuleIface {
 public:
  class Delegate {
   public:
    virtual void GifRecordCurrentPosition(uint32_t& cur_pos) = 0;
    virtual bool GifInputRecordPositionBuf(uint32_t rcd_pos,
                                           const FX_RECT& img_rc,
                                           int32_t pal_num,
                                           CFX_GifPalette* pal_ptr,
                                           int32_t delay_time,
                                           bool user_input,
                                           int32_t trans_index,
                                           int32_t disposal_method,
                                           bool interlace) = 0;
    virtual void GifReadScanline(int32_t row_num, uint8_t* row_buf) = 0;
  };

  CCodec_GifModule();
  ~CCodec_GifModule() override;

  // CodecModuleIface:
  FX_FILESIZE GetAvailInput(Context* context) const override;
  bool Input(Context* context,
             RetainPtr<CFX_CodecMemory> codec_memory,
             CFX_DIBAttribute* pAttribute) override;

  std::unique_ptr<Context> Start(Delegate* pDelegate);
  CFX_GifDecodeStatus ReadHeader(Context* context,
                                 int* width,
                                 int* height,
                                 int* pal_num,
                                 CFX_GifPalette** pal_pp,
                                 int* bg_index,
                                 CFX_DIBAttribute* pAttribute);
  std::pair<CFX_GifDecodeStatus, size_t> LoadFrameInfo(Context* context);
  CFX_GifDecodeStatus LoadFrame(Context* context,
                                size_t frame_num,
                                CFX_DIBAttribute* pAttribute);
};

#endif  // CORE_FXCODEC_CODEC_CCODEC_GIFMODULE_H_
