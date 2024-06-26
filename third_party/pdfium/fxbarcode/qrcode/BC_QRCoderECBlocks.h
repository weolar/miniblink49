// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXBARCODE_QRCODE_BC_QRCODERECBLOCKS_H_
#define FXBARCODE_QRCODE_BC_QRCODERECBLOCKS_H_

#include <stdint.h>

struct CBC_QRCoderECBlockData;

class CBC_QRCoderECBlocks {
 public:
  explicit CBC_QRCoderECBlocks(const CBC_QRCoderECBlockData& data);
  ~CBC_QRCoderECBlocks();

  int32_t GetNumBlocks() const;
  int32_t GetTotalECCodeWords() const;
  int32_t GetTotalDataCodeWords() const;

 private:
  int32_t GetECCodeWordsPerBlock() const;

  const CBC_QRCoderECBlockData& m_data;
};

#endif  // FXBARCODE_QRCODE_BC_QRCODERECBLOCKS_H_
