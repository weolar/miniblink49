// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCODEC_CODEC_CCODEC_JPEGMODULE_H_
#define CORE_FXCODEC_CODEC_CCODEC_JPEGMODULE_H_

#include <csetjmp>
#include <memory>

#include "core/fxcodec/codec/codec_module_iface.h"
#include "third_party/base/span.h"

class CCodec_ScanlineDecoder;
class CFX_DIBAttribute;
class CFX_DIBBase;

class CCodec_JpegModule final : public CodecModuleIface {
 public:
  std::unique_ptr<CCodec_ScanlineDecoder> CreateDecoder(
      pdfium::span<const uint8_t> src_buf,
      int width,
      int height,
      int nComps,
      bool ColorTransform);

  // CodecModuleIface:
  FX_FILESIZE GetAvailInput(Context* pContext) const override;
  bool Input(Context* pContext,
             RetainPtr<CFX_CodecMemory> codec_memory,
             CFX_DIBAttribute* pAttribute) override;

  jmp_buf* GetJumpMark(Context* pContext);
  bool LoadInfo(pdfium::span<const uint8_t> src_span,
                int* width,
                int* height,
                int* num_components,
                int* bits_per_components,
                bool* color_transform);

  std::unique_ptr<Context> Start();

#if 1 // def PDF_ENABLE_XFA
  int ReadHeader(Context* pContext,
                 int* width,
                 int* height,
                 int* nComps,
                 CFX_DIBAttribute* pAttribute);
#endif  // PDF_ENABLE_XFA

  bool StartScanline(Context* pContext, int down_scale);
  bool ReadScanline(Context* pContext, uint8_t* dest_buf);

#if _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_
  static bool JpegEncode(const RetainPtr<CFX_DIBBase>& pSource,
                         uint8_t** dest_buf,
                         size_t* dest_size);
#endif  // _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_
};

#endif  // CORE_FXCODEC_CODEC_CCODEC_JPEGMODULE_H_
