// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCODEC_BMP_FX_BMP_H_
#define CORE_FXCODEC_BMP_FX_BMP_H_

#include "core/fxcodec/codec/ccodec_bmpmodule.h"

#define BMP_WIDTHBYTES(width, bitCount) ((width * bitCount) + 31) / 32 * 4
#define BMP_PAL_ENCODE(a, r, g, b) \
  (((uint32_t)(a) << 24) | ((r) << 16) | ((g) << 8) | (b))
#define BMP_D_STATUS_HEADER 0x01
#define BMP_D_STATUS_PAL 0x02
#define BMP_D_STATUS_DATA_PRE 0x03
#define BMP_D_STATUS_DATA 0x04
#define BMP_D_STATUS_TAIL 0x00
#define BMP_SIGNATURE 0x4D42
#define BMP_PAL_OLD 1
#define RLE_MARKER 0
#define RLE_EOL 0
#define RLE_EOI 1
#define RLE_DELTA 2
#define BMP_RGB 0L
#define BMP_RLE8 1L
#define BMP_RLE4 2L
#define BMP_BITFIELDS 3L
// Limit width to (MAXINT32 - 31) / 32
#define BMP_MAX_WIDTH 67108863
#pragma pack(1)
struct BmpFileHeader {
  uint16_t bfType;
  uint32_t bfSize;
  uint16_t bfReserved1;
  uint16_t bfReserved2;
  uint32_t bfOffBits;
};

struct BmpCoreHeader {
  uint32_t bcSize;
  uint16_t bcWidth;
  uint16_t bcHeight;
  uint16_t bcPlanes;
  uint16_t bcBitCount;
};

struct BmpInfoHeader {
  uint32_t biSize;
  int32_t biWidth;
  int32_t biHeight;
  uint16_t biPlanes;
  uint16_t biBitCount;
  uint32_t biCompression;
  uint32_t biSizeImage;
  int32_t biXPelsPerMeter;
  int32_t biYPelsPerMeter;
  uint32_t biClrUsed;
  uint32_t biClrImportant;
};
#pragma pack()

#endif  // CORE_FXCODEC_BMP_FX_BMP_H_
