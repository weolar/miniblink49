// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXBARCODE_PDF417_BC_PDF417BARCODEROW_H_
#define FXBARCODE_PDF417_BC_PDF417BARCODEROW_H_

#include <stdint.h>

#include <vector>

class CBC_BarcodeRow {
 public:
  explicit CBC_BarcodeRow(size_t width);
  virtual ~CBC_BarcodeRow();

  void addBar(bool black, int32_t width);
  std::vector<uint8_t>& getRow();

 private:
  std::vector<uint8_t> m_row;
  int32_t m_currentLocation;
};

#endif  // FXBARCODE_PDF417_BC_PDF417BARCODEROW_H_
