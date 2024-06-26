// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCODEC_GIF_CFX_LZWDECOMPRESSOR_H_
#define CORE_FXCODEC_GIF_CFX_LZWDECOMPRESSOR_H_

#include <memory>
#include <vector>

#include "core/fxcodec/gif/cfx_gif.h"

class CFX_LZWDecompressor {
 public:
  struct CodeEntry {
    uint16_t prefix;
    uint8_t suffix;
  };

  // Returns nullptr on error
  static std::unique_ptr<CFX_LZWDecompressor> Create(uint8_t color_exp,
                                                     uint8_t code_exp);
  ~CFX_LZWDecompressor();

  CFX_GifDecodeStatus Decode(const uint8_t* src_buf,
                             uint32_t src_size,
                             uint8_t* dest_buf,
                             uint32_t* dest_size);

  // Used by unittests, should not be called in production code.
  uint32_t ExtractDataForTest(uint8_t* dest_buf, uint32_t dest_size) {
    return ExtractData(dest_buf, dest_size);
  }

  std::vector<uint8_t>* DecompressedForTest() { return &decompressed_; }
  size_t* DecompressedNextForTest() { return &decompressed_next_; }

 private:
  CFX_LZWDecompressor(uint8_t color_exp, uint8_t code_exp);
  void ClearTable();
  void AddCode(uint16_t prefix_code, uint8_t append_char);
  bool DecodeString(uint16_t code);
  uint32_t ExtractData(uint8_t* dest_buf, uint32_t dest_size);

  uint8_t code_size_;
  uint8_t code_size_cur_;
  uint16_t code_color_end_;
  uint16_t code_clear_;
  uint16_t code_end_;
  uint16_t code_next_;
  uint8_t code_first_;
  std::vector<uint8_t> decompressed_;
  size_t decompressed_next_;
  uint16_t code_old_;
  const uint8_t* next_in_;
  uint32_t avail_in_;
  uint8_t bits_left_;
  uint32_t code_store_;
  CodeEntry code_table_[GIF_MAX_LZW_CODE];
};

#endif  // CORE_FXCODEC_GIF_CFX_LZWDECOMPRESSOR_H_
