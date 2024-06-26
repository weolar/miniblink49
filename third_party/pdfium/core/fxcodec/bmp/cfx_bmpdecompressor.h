// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCODEC_BMP_CFX_BMPDECOMPRESSOR_H_
#define CORE_FXCODEC_BMP_CFX_BMPDECOMPRESSOR_H_

#include <setjmp.h>

#include <vector>

#include "core/fxcodec/bmp/fx_bmp.h"

class CFX_BmpContext;
class CFX_CodecMemory;

class CFX_BmpDecompressor {
 public:
  CFX_BmpDecompressor();
  ~CFX_BmpDecompressor();

  void Error();
  int32_t DecodeImage();
  int32_t ReadHeader();
  void SetInputBuffer(RetainPtr<CFX_CodecMemory> codec_memory);
  FX_FILESIZE GetAvailInput() const;

  jmp_buf jmpbuf_;
  CFX_BmpContext* context_ptr_;
  std::vector<uint8_t> out_row_buffer_;
  std::vector<uint32_t> palette_;
  uint32_t header_offset_;
  uint32_t width_;
  uint32_t height_;
  uint32_t compress_flag_;
  int32_t components_;
  size_t src_row_bytes_;
  size_t out_row_bytes_;
  uint16_t bit_counts_;
  uint32_t color_used_;
  bool imgTB_flag_;
  int32_t pal_num_;
  int32_t pal_type_;
  uint32_t data_size_;
  uint32_t img_data_offset_;
  uint32_t img_ifh_size_;
  size_t row_num_;
  size_t col_num_;
  int32_t dpi_x_;
  int32_t dpi_y_;
  uint32_t mask_red_;
  uint32_t mask_green_;
  uint32_t mask_blue_;
  int32_t decode_status_;

 private:
  bool GetDataPosition(uint32_t cur_pos);
  void ReadScanline(uint32_t row_num, const std::vector<uint8_t>& row_buf);
  int32_t DecodeRGB();
  int32_t DecodeRLE8();
  int32_t DecodeRLE4();
  bool ReadData(uint8_t* destination, uint32_t size);
  void SaveDecodingStatus(int32_t status);
  bool ValidateColorIndex(uint8_t val);
  bool ValidateFlag() const;
  void SetHeight(int32_t signed_height);

  RetainPtr<CFX_CodecMemory> input_buffer_;
};

#endif  // CORE_FXCODEC_BMP_CFX_BMPDECOMPRESSOR_H_
