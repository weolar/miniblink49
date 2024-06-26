// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXBARCODE_PDF417_BC_PDF417_H_
#define FXBARCODE_PDF417_BC_PDF417_H_

#include <memory>
#include <vector>

#include "core/fxcrt/fx_string.h"

class CBC_BarcodeRow;
class CBC_BarcodeMatrix;

class CBC_PDF417 {
 public:
  CBC_PDF417();
  ~CBC_PDF417();

  CBC_BarcodeMatrix* getBarcodeMatrix();
  bool GenerateBarcodeLogic(WideStringView msg, int32_t errorCorrectionLevel);
  void setDimensions(int32_t maxCols,
                     int32_t minCols,
                     int32_t maxRows,
                     int32_t minRows);

 private:
  static const int32_t START_PATTERN = 0x1fea8;
  static const int32_t STOP_PATTERN = 0x3fa29;
  static constexpr float PREFERRED_RATIO = 3.0f;
  static constexpr float DEFAULT_MODULE_WIDTH = 0.357f;
  static constexpr float HEIGHT = 2.0f;

  static int32_t calculateNumberOfRows(int32_t m, int32_t k, int32_t c);
  static int32_t getNumberOfPadCodewords(int32_t m,
                                         int32_t k,
                                         int32_t c,
                                         int32_t r);
  static void encodeChar(int32_t pattern, int32_t len, CBC_BarcodeRow* logic);
  void encodeLowLevel(WideString fullCodewords,
                      int32_t c,
                      int32_t r,
                      int32_t errorCorrectionLevel,
                      CBC_BarcodeMatrix* logic);
  std::vector<int32_t> determineDimensions(
      size_t sourceCodeWords,
      int32_t errorCorrectionCodeWords) const;

  std::unique_ptr<CBC_BarcodeMatrix> m_barcodeMatrix;
  int32_t m_minCols = 1;
  int32_t m_maxCols = 30;
  int32_t m_minRows = 3;
  int32_t m_maxRows = 90;
};

#endif  // FXBARCODE_PDF417_BC_PDF417_H_
