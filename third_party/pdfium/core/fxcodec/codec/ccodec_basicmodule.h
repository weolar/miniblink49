// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCODEC_CODEC_CCODEC_BASICMODULE_H_
#define CORE_FXCODEC_CODEC_CCODEC_BASICMODULE_H_

#include <memory>

#include "core/fxcrt/fx_memory.h"
#include "core/fxcrt/fx_system.h"
#include "third_party/base/span.h"

class CCodec_ScanlineDecoder;

class CCodec_BasicModule {
 public:
  std::unique_ptr<CCodec_ScanlineDecoder> CreateRunLengthDecoder(
      pdfium::span<const uint8_t> src_buf,
      int width,
      int height,
      int nComps,
      int bpc);

  bool RunLengthEncode(pdfium::span<const uint8_t> src_buf,
                       std::unique_ptr<uint8_t, FxFreeDeleter>* dest_buf,
                       uint32_t* dest_size);

  bool A85Encode(pdfium::span<const uint8_t> src_buf,
                 std::unique_ptr<uint8_t, FxFreeDeleter>* dest_buf,
                 uint32_t* dest_size);
};

#endif  // CORE_FXCODEC_CODEC_CCODEC_BASICMODULE_H_
