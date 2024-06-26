// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "fxbarcode/BC_TwoDimWriter.h"

#include <algorithm>

#include "core/fxge/cfx_graphstatedata.h"
#include "core/fxge/cfx_pathdata.h"
#include "core/fxge/cfx_renderdevice.h"
#include "fxbarcode/BC_Writer.h"
#include "fxbarcode/common/BC_CommonBitMatrix.h"
#include "third_party/base/numerics/safe_math.h"
#include "third_party/base/ptr_util.h"

CBC_TwoDimWriter::CBC_TwoDimWriter(bool bFixedSize)
    : m_bFixedSize(bFixedSize) {}

CBC_TwoDimWriter::~CBC_TwoDimWriter() = default;

bool CBC_TwoDimWriter::RenderResult(const std::vector<uint8_t>& code,
                                    int32_t codeWidth,
                                    int32_t codeHeight) {
  if (code.empty())
    return false;

  m_inputWidth = codeWidth;
  m_inputHeight = codeHeight;
  int32_t tempWidth = m_inputWidth + 2;
  int32_t tempHeight = m_inputHeight + 2;
  float moduleHSize = std::min(m_ModuleWidth, m_ModuleHeight);
  moduleHSize = std::min(moduleHSize, 8.0f);
  moduleHSize = std::max(moduleHSize, 1.0f);
  pdfium::base::CheckedNumeric<int32_t> scaledWidth = tempWidth;
  pdfium::base::CheckedNumeric<int32_t> scaledHeight = tempHeight;
  scaledWidth *= moduleHSize;
  scaledHeight *= moduleHSize;
  m_outputWidth = scaledWidth.ValueOrDie();
  m_outputHeight = scaledHeight.ValueOrDie();

  if (m_bFixedSize) {
    if (m_Width < m_outputWidth || m_Height < m_outputHeight) {
      return false;
    }
  } else {
    if (m_Width > m_outputWidth || m_Height > m_outputHeight) {
      int32_t width_factor = static_cast<int32_t>(
          floor(static_cast<float>(m_Width) / m_outputWidth));
      int32_t height_factor = static_cast<int32_t>(
          floor(static_cast<float>(m_Height) / m_outputHeight));
      width_factor = std::max(width_factor, 1);
      height_factor = std::max(height_factor, 1);

      m_outputWidth *= width_factor;
      m_outputHeight *= height_factor;
    }
  }
  m_multiX =
      static_cast<int32_t>(ceil(static_cast<float>(m_outputWidth) / tempWidth));
  m_multiY = static_cast<int32_t>(
      ceil(static_cast<float>(m_outputHeight) / tempHeight));
  if (m_bFixedSize) {
    m_multiX = std::min(m_multiX, m_multiY);
    m_multiY = m_multiX;
  }

  m_leftPadding = std::max((m_Width - m_outputWidth) / 2, 0);
  m_topPadding = std::max((m_Height - m_outputHeight) / 2, 0);

  m_output = pdfium::MakeUnique<CBC_CommonBitMatrix>();
  m_output->Init(m_inputWidth, m_inputHeight);

  for (int32_t y = 0; y < m_inputHeight; ++y) {
    for (int32_t x = 0; x < m_inputWidth; ++x) {
      if (code[x + y * m_inputWidth] == 1)
        m_output->Set(x, y);
    }
  }
  return true;
}

void CBC_TwoDimWriter::RenderDeviceResult(CFX_RenderDevice* device,
                                          const CFX_Matrix* matrix) {
  ASSERT(m_output);

  CFX_GraphStateData stateData;
  CFX_PathData path;
  path.AppendRect(0, 0, m_Width, m_Height);
  device->DrawPath(&path, matrix, &stateData, m_backgroundColor,
                   m_backgroundColor, FXFILL_ALTERNATE);
  int32_t leftPos = m_leftPadding;
  int32_t topPos = m_topPadding;

  CFX_Matrix matri = *matrix;
  if (m_Width < m_outputWidth && m_Height < m_outputHeight) {
    CFX_Matrix matriScale(static_cast<float>(m_Width) / m_outputWidth, 0.0, 0.0,
                          static_cast<float>(m_Height) / m_outputHeight, 0.0,
                          0.0);
    matriScale.Concat(*matrix);
    matri = matriScale;
  }

  CFX_GraphStateData data;
  for (int32_t x = 0; x < m_inputWidth; x++) {
    for (int32_t y = 0; y < m_inputHeight; y++) {
      if (m_output->Get(x, y)) {
        // In the output, each module is shifted by 1 due to the one module
        // padding added to create quiet areas.
        int start_x_output = x + 1;
        int end_x_output = x + 2;
        int start_y_output = y + 1;
        int end_y_output = y + 2;

        CFX_PathData rect;
        rect.AppendRect(leftPos + start_x_output * m_multiX,
                        topPos + start_y_output * m_multiY,
                        leftPos + end_x_output * m_multiX,
                        topPos + end_y_output * m_multiY);
        device->DrawPath(&rect, &matri, &data, m_barColor, 0, FXFILL_WINDING);
      }
    }
  }
}
