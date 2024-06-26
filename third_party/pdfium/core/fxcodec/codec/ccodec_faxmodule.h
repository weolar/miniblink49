// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCODEC_CODEC_CCODEC_FAXMODULE_H_
#define CORE_FXCODEC_CODEC_CCODEC_FAXMODULE_H_

#include <memory>

#include "core/fxcrt/fx_memory.h"
#include "core/fxcrt/fx_system.h"
#include "third_party/base/span.h"

class CCodec_ScanlineDecoder;

class CCodec_FaxModule {
 public:
  std::unique_ptr<CCodec_ScanlineDecoder> CreateDecoder(
      pdfium::span<const uint8_t> src_buf,
      int width,
      int height,
      int K,
      bool EndOfLine,
      bool EncodedByteAlign,
      bool BlackIs1,
      int Columns,
      int Rows);

#if _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_
  static void FaxEncode(const uint8_t* src_buf,
                        int width,
                        int height,
                        int pitch,
                        std::unique_ptr<uint8_t, FxFreeDeleter>* dest_buf,
                        uint32_t* dest_size);
#endif  // _FX_PLATFORM_ == _FX_PLATFORM_WINDOWS_

  // Return the ending bit position.
  static int FaxG4Decode(const uint8_t* src_buf,
                         uint32_t src_size,
                         int starting_bitpos,
                         int width,
                         int height,
                         int pitch,
                         uint8_t* dest_buf);
};

#endif  // CORE_FXCODEC_CODEC_CCODEC_FAXMODULE_H_
