// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef FXBARCODE_ONED_BC_ONEDEANWRITER_H_
#define FXBARCODE_ONED_BC_ONEDEANWRITER_H_

#include "fxbarcode/oned/BC_OneDimWriter.h"

class CBC_OneDimEANWriter : public CBC_OneDimWriter {
 public:
  CBC_OneDimEANWriter();
  ~CBC_OneDimEANWriter() override;

  virtual void InitEANWriter();
  virtual int32_t CalcChecksum(const ByteString& contents) = 0;
};

#endif  // FXBARCODE_ONED_BC_ONEDEANWRITER_H_
