// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FXBARCODE_BC_TWODIMWRITER_H_
#define FXBARCODE_BC_TWODIMWRITER_H_

#include <memory>
#include <vector>

#include "core/fxcrt/fx_coordinates.h"
#include "fxbarcode/BC_Writer.h"

class CBC_CommonBitMatrix;
class CFX_RenderDevice;

class CBC_TwoDimWriter : public CBC_Writer {
 public:
  explicit CBC_TwoDimWriter(bool bFixedSize);
  ~CBC_TwoDimWriter() override;

  bool RenderResult(const std::vector<uint8_t>& code,
                    int32_t codeWidth,
                    int32_t codeHeight);
  void RenderDeviceResult(CFX_RenderDevice* device, const CFX_Matrix* matrix);

  int32_t error_correction_level() const { return m_iCorrectionLevel; }

 protected:
  void set_error_correction_level(int32_t level) { m_iCorrectionLevel = level; }

 private:
  std::unique_ptr<CBC_CommonBitMatrix> m_output;
  int32_t m_multiX;
  int32_t m_multiY;
  int32_t m_leftPadding;
  int32_t m_topPadding;
  int32_t m_inputWidth;
  int32_t m_inputHeight;
  int32_t m_outputWidth;
  int32_t m_outputHeight;
  int32_t m_iCorrectionLevel = 1;
  const bool m_bFixedSize;
};

#endif  // FXBARCODE_BC_TWODIMWRITER_H_
