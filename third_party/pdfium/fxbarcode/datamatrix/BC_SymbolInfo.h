// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXBARCODE_DATAMATRIX_BC_SYMBOLINFO_H_
#define FXBARCODE_DATAMATRIX_BC_SYMBOLINFO_H_

#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/fx_system.h"

class CBC_SymbolInfo {
 public:
  CBC_SymbolInfo(size_t dataCapacity,
                 size_t errorCodewords,
                 int32_t matrixWidth,
                 int32_t matrixHeight,
                 int32_t dataRegions);
  virtual ~CBC_SymbolInfo();

  static void Initialize();
  static void Finalize();
  static void overrideSymbolSet(CBC_SymbolInfo* override);
  static const CBC_SymbolInfo* Lookup(size_t iDataCodewords,
                                      bool bAllowRectangular);

  int32_t getSymbolDataWidth() const;
  int32_t getSymbolDataHeight() const;
  int32_t getSymbolWidth() const;
  int32_t getSymbolHeight() const;
  size_t getCodewordCount() const;
  virtual size_t getInterleavedBlockCount() const;
  size_t getDataLengthForInterleavedBlock() const;
  size_t getErrorLengthForInterleavedBlock() const;

  size_t dataCapacity() const { return m_dataCapacity; }
  size_t errorCodewords() const { return m_errorCodewords; }
  int32_t matrixWidth() const { return m_matrixWidth; }
  int32_t matrixHeight() const { return m_matrixHeight; }

 protected:
  CBC_SymbolInfo(size_t dataCapacity,
                 size_t errorCodewords,
                 int32_t matrixWidth,
                 int32_t matrixHeight,
                 int32_t dataRegions,
                 size_t rsBlockData,
                 size_t rsBlockError);

 private:
  int32_t getHorizontalDataRegions() const;
  int32_t getVerticalDataRegions() const;

  const bool m_rectangular;
  const size_t m_dataCapacity;
  const size_t m_errorCodewords;
  const int32_t m_matrixWidth;
  const int32_t m_matrixHeight;
  const int32_t m_dataRegions;
  const size_t m_rsBlockData;
  const size_t m_rsBlockError;
};

#endif  // FXBARCODE_DATAMATRIX_BC_SYMBOLINFO_H_
