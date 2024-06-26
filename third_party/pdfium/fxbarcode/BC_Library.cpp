// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxbarcode/BC_Library.h"

#include <stdint.h>

#include "fxbarcode/common/reedsolomon/BC_ReedSolomonGF256.h"
#include "fxbarcode/datamatrix/BC_SymbolInfo.h"
#include "fxbarcode/qrcode/BC_QRCoderErrorCorrectionLevel.h"
#include "fxbarcode/qrcode/BC_QRCoderMode.h"
#include "fxbarcode/qrcode/BC_QRCoderVersion.h"

void BC_Library_Init() {
  CBC_QRCoderErrorCorrectionLevel::Initialize();
  CBC_QRCoderMode::Initialize();
  CBC_QRCoderVersion::Initialize();
  CBC_ReedSolomonGF256::Initialize();
  CBC_SymbolInfo::Initialize();
}

void BC_Library_Destroy() {
  CBC_QRCoderErrorCorrectionLevel::Finalize();
  CBC_QRCoderMode::Finalize();
  CBC_QRCoderVersion::Finalize();
  CBC_ReedSolomonGF256::Finalize();
  CBC_SymbolInfo::Finalize();
}
