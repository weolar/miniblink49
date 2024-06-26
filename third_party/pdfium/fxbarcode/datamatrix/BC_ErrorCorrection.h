// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXBARCODE_DATAMATRIX_BC_ERRORCORRECTION_H_
#define FXBARCODE_DATAMATRIX_BC_ERRORCORRECTION_H_

#include "core/fxcrt/widestring.h"

class CBC_SymbolInfo;

class CBC_ErrorCorrection {
 public:
  CBC_ErrorCorrection() = delete;
  ~CBC_ErrorCorrection() = delete;

  // Returns an empty string on failure.
  static WideString EncodeECC200(const WideString& codewords,
                                 const CBC_SymbolInfo* symbolInfo);
};

#endif  // FXBARCODE_DATAMATRIX_BC_ERRORCORRECTION_H_
