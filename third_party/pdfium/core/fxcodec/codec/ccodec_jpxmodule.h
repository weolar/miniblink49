// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCODEC_CODEC_CCODEC_JPXMODULE_H_
#define CORE_FXCODEC_CODEC_CCODEC_JPXMODULE_H_

#include <memory>
#include <vector>

#include "core/fxcrt/fx_system.h"
#include "third_party/base/span.h"

class CJPX_Decoder;
class CPDF_ColorSpace;

class CCodec_JpxModule {
 public:
  CCodec_JpxModule();
  ~CCodec_JpxModule();

  std::unique_ptr<CJPX_Decoder> CreateDecoder(
      pdfium::span<const uint8_t> src_span,
      CPDF_ColorSpace* cs);

  void GetImageInfo(CJPX_Decoder* pDecoder,
                    uint32_t* width,
                    uint32_t* height,
                    uint32_t* components);
  bool Decode(CJPX_Decoder* pDecoder,
              uint8_t* dest_data,
              uint32_t pitch,
              const std::vector<uint8_t>& offsets);
};

#endif  // CORE_FXCODEC_CODEC_CCODEC_JPXMODULE_H_
