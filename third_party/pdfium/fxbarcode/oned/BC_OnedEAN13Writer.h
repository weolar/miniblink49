// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXBARCODE_ONED_BC_ONEDEAN13WRITER_H_
#define FXBARCODE_ONED_BC_ONEDEAN13WRITER_H_

#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/fx_system.h"
#include "fxbarcode/oned/BC_OnedEANWriter.h"

class CFX_DIBitmap;
class CFX_RenderDevice;

class CBC_OnedEAN13Writer final : public CBC_OneDimEANWriter {
 public:
  CBC_OnedEAN13Writer();
  ~CBC_OnedEAN13Writer() override;

  // CBC_OneDimEANWriter:
  uint8_t* EncodeWithHint(const ByteString& contents,
                          BCFORMAT format,
                          int32_t& outWidth,
                          int32_t& outHeight,
                          int32_t hints) override;
  uint8_t* EncodeImpl(const ByteString& contents, int32_t& outLength) override;
  bool CheckContentValidity(WideStringView contents) override;
  WideString FilterContents(WideStringView contents) override;
  int32_t CalcChecksum(const ByteString& contents) override;

 private:
  bool ShowChars(WideStringView contents,
                 CFX_RenderDevice* device,
                 const CFX_Matrix* matrix,
                 int32_t barWidth,
                 int32_t multiple) override;

  int32_t m_codeWidth;
};

#endif  // FXBARCODE_ONED_BC_ONEDEAN13WRITER_H_
