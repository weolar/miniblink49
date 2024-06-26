// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXBARCODE_PDF417_BC_PDF417WRITER_H_
#define FXBARCODE_PDF417_BC_PDF417WRITER_H_

#include <vector>

#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/fx_system.h"
#include "fxbarcode/BC_TwoDimWriter.h"

class CBC_PDF417Writer final : public CBC_TwoDimWriter {
 public:
  CBC_PDF417Writer();
  ~CBC_PDF417Writer() override;

  std::vector<uint8_t> Encode(WideStringView contents,
                              int32_t* pOutWidth,
                              int32_t* pOutHeight);

  // CBC_TwoDimWriter
  bool SetErrorCorrectionLevel(int32_t level) override;

 private:
  void RotateArray(std::vector<uint8_t>* bitarray,
                   int32_t width,
                   int32_t height);
};

#endif  // FXBARCODE_PDF417_BC_PDF417WRITER_H_
