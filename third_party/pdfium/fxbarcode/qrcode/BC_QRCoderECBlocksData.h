// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXBARCODE_QRCODE_BC_QRCODERECBLOCKSDATA_H_
#define FXBARCODE_QRCODE_BC_QRCODERECBLOCKSDATA_H_

#include <stdint.h>

struct CBC_QRCoderECBlockData {
  uint8_t ecCodeWordsPerBlock;
  uint8_t count1;
  uint8_t dataCodeWords1;
  uint8_t count2;
  uint8_t dataCodeWords2;
};

extern const CBC_QRCoderECBlockData g_ECBData[40][4];

#endif  // FXBARCODE_QRCODE_BC_QRCODERECBLOCKSDATA_H_
