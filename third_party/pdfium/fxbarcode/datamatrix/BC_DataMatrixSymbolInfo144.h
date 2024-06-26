// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXBARCODE_DATAMATRIX_BC_DATAMATRIXSYMBOLINFO144_H_
#define FXBARCODE_DATAMATRIX_BC_DATAMATRIXSYMBOLINFO144_H_

#include "fxbarcode/datamatrix/BC_SymbolInfo.h"

class CBC_DataMatrixSymbolInfo144 final : public CBC_SymbolInfo {
 public:
  CBC_DataMatrixSymbolInfo144();
  ~CBC_DataMatrixSymbolInfo144() override;

  // CBC_SymbolInfo:
  size_t getInterleavedBlockCount() const override;
};

#endif  // FXBARCODE_DATAMATRIX_BC_DATAMATRIXSYMBOLINFO144_H_
