// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXBARCODE_UTILS_H_
#define FXBARCODE_UTILS_H_

enum BCFORMAT {
  BCFORMAT_UNSPECIFY = -1,
  BCFORMAT_CODABAR,
  BCFORMAT_CODE_39,
  BCFORMAT_CODE_128,
  BCFORMAT_CODE_128B,
  BCFORMAT_CODE_128C,
  BCFORMAT_EAN_8,
  BCFORMAT_UPC_A,
  BCFORMAT_EAN_13,
  BCFORMAT_PDF_417,
  BCFORMAT_DATAMATRIX,
  BCFORMAT_QR_CODE
};

#endif  // FXBARCODE_UTILS_H_
