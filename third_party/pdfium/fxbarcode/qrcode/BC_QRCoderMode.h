// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXBARCODE_QRCODE_BC_QRCODERMODE_H_
#define FXBARCODE_QRCODE_BC_QRCODERMODE_H_

#include <stdint.h>

#include <vector>

class CBC_QRCoderMode {
 public:
  virtual ~CBC_QRCoderMode();

  static void Initialize();
  static void Finalize();

  int32_t GetCharacterCountBits(int32_t number) const;
  int32_t GetBits() const;

  static CBC_QRCoderMode* sBYTE;
  static CBC_QRCoderMode* sNUMERIC;
  static CBC_QRCoderMode* sALPHANUMERIC;
  static CBC_QRCoderMode* sKANJI;
  static CBC_QRCoderMode* sECI;
  static CBC_QRCoderMode* sGBK;
  static CBC_QRCoderMode* sTERMINATOR;
  static CBC_QRCoderMode* sFNC1_FIRST_POSITION;
  static CBC_QRCoderMode* sFNC1_SECOND_POSITION;
  static CBC_QRCoderMode* sSTRUCTURED_APPEND;

 private:
  CBC_QRCoderMode(std::vector<int32_t> charCountBits, int32_t bits);

  std::vector<int32_t> m_characterCountBitsForVersions;
  const int32_t m_bits;
};

#endif  // FXBARCODE_QRCODE_BC_QRCODERMODE_H_
