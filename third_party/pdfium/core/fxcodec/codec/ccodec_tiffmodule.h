// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCODEC_CODEC_CCODEC_TIFFMODULE_H_
#define CORE_FXCODEC_CODEC_CCODEC_TIFFMODULE_H_

#include <memory>

#include "core/fxcodec/codec/codec_module_iface.h"

class CFX_DIBAttribute;
class CFX_DIBitmap;
class IFX_SeekableReadStream;

class CCodec_TiffModule final : public CodecModuleIface {
 public:
  std::unique_ptr<Context> CreateDecoder(
      const RetainPtr<IFX_SeekableReadStream>& file_ptr);

  // CodecModuleIface:
  FX_FILESIZE GetAvailInput(Context* pContext) const override;
  bool Input(Context* pContext,
             RetainPtr<CFX_CodecMemory> codec_memory,
             CFX_DIBAttribute* pAttribute) override;

  bool LoadFrameInfo(Context* ctx,
                     int32_t frame,
                     int32_t* width,
                     int32_t* height,
                     int32_t* comps,
                     int32_t* bpc,
                     CFX_DIBAttribute* pAttribute);
  bool Decode(Context* ctx, const RetainPtr<CFX_DIBitmap>& pDIBitmap);
};

#endif  // CORE_FXCODEC_CODEC_CCODEC_TIFFMODULE_H_
