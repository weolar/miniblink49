// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "fxbarcode/oned/BC_OnedEANChecksum.h"

#include "core/fxcrt/fx_extension.h"

int32_t EANCalcChecksum(const ByteString& contents) {
  int32_t odd = 0;
  int32_t even = 0;
  size_t parity = 1;
  for (size_t i = contents.GetLength(); i > 0; i--) {
    if (parity % 2)
      odd += FXSYS_DecimalCharToInt(contents[i - 1]);
    else
      even += FXSYS_DecimalCharToInt(contents[i - 1]);
    parity++;
  }
  return (10 - (odd * 3 + even) % 10) % 10;
}
