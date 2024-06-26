// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "xfa/fxgraphics/cxfa_graphics.h"

#include <memory>

#include "core/fxge/cfx_defaultrenderdevice.h"
#include "core/fxge/cfx_renderdevice.h"
#include "core/fxge/cfx_unicodeencoding.h"
#include "core/fxge/dib/cfx_dibitmap.h"
#include "third_party/base/ptr_util.h"
#include "xfa/fxgraphics/cxfa_gecolor.h"
#include "xfa/fxgraphics/cxfa_gepath.h"
#include "xfa/fxgraphics/cxfa_gepattern.h"
#include "xfa/fxgraphics/cxfa_geshading.h"

namespace {

struct FX_HATCHDATA {
  int32_t width;
  int32_t height;
  uint8_t maskBits[64];
};

const FX_HATCHDATA kHatchBitmapData[] = {
    {16,  // Horizontal
     16,
     {
         0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff,
         0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
         0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
     }},
    {16,  // Vertical
     16,
     {
         0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x00,
         0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80,
         0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80,
         0x80, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00,
         0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x00,
         0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00,
     }},
    {16,  // ForwardDiagonal
     16,
     {
         0x80, 0x80, 0x00, 0x00, 0x40, 0x40, 0x00, 0x00, 0x20, 0x20, 0x00,
         0x00, 0x10, 0x10, 0x00, 0x00, 0x08, 0x08, 0x00, 0x00, 0x04, 0x04,
         0x00, 0x00, 0x02, 0x02, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00, 0x80,
         0x80, 0x00, 0x00, 0x40, 0x40, 0x00, 0x00, 0x20, 0x20, 0x00, 0x00,
         0x10, 0x10, 0x00, 0x00, 0x08, 0x08, 0x00, 0x00, 0x04, 0x04, 0x00,
         0x00, 0x02, 0x02, 0x00, 0x00, 0x01, 0x01, 0x00, 0x00,
     }},
    {16,  // BackwardDiagonal
     16,
     {
         0x01, 0x01, 0x00, 0x00, 0x02, 0x02, 0x00, 0x00, 0x04, 0x04, 0x00,
         0x00, 0x08, 0x08, 0x00, 0x00, 0x10, 0x10, 0x00, 0x00, 0x20, 0x20,
         0x00, 0x00, 0x40, 0x40, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x01,
         0x01, 0x00, 0x00, 0x02, 0x02, 0x00, 0x00, 0x04, 0x04, 0x00, 0x00,
         0x08, 0x08, 0x00, 0x00, 0x10, 0x10, 0x00, 0x00, 0x20, 0x20, 0x00,
         0x00, 0x40, 0x40, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00,
     }},
    {16,  // Cross
     16,
     {
         0xff, 0xff, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x00,
         0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80,
         0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0xff,
         0xff, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00,
         0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x00,
         0x00, 0x80, 0x80, 0x00, 0x00, 0x80, 0x80, 0x00, 0x00,
     }},
    {16,  // DiagonalCross
     16,
     {
         0x81, 0x81, 0x00, 0x00, 0x42, 0x42, 0x00, 0x00, 0x24, 0x24, 0x00,
         0x00, 0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x24, 0x24,
         0x00, 0x00, 0x42, 0x42, 0x00, 0x00, 0x81, 0x81, 0x00, 0x00, 0x81,
         0x81, 0x00, 0x00, 0x42, 0x42, 0x00, 0x00, 0x24, 0x24, 0x00, 0x00,
         0x18, 0x18, 0x00, 0x00, 0x18, 0x18, 0x00, 0x00, 0x24, 0x24, 0x00,
         0x00, 0x42, 0x42, 0x00, 0x00, 0x81, 0x81, 0x00, 0x00,
     }},
};

const FX_HATCHDATA kHatchPlaceHolder = {
    0,
    0,
    {
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    }};

const FX_HATCHDATA& GetHatchBitmapData(size_t index) {
  return index < FX_ArraySize(kHatchBitmapData) ? kHatchBitmapData[index]
                                                : kHatchPlaceHolder;
}

}  // namespace

CXFA_Graphics::CXFA_Graphics(CFX_RenderDevice* renderDevice)
    : m_renderDevice(renderDevice) {
  ASSERT(m_renderDevice);
}

CXFA_Graphics::~CXFA_Graphics() = default;

void CXFA_Graphics::SaveGraphState() {
  m_renderDevice->SaveState();
  m_infoStack.push_back(pdfium::MakeUnique<TInfo>(m_info));
}

void CXFA_Graphics::RestoreGraphState() {
  m_renderDevice->RestoreState(false);
  if (m_infoStack.empty())
    return;

  m_info = *m_infoStack.back();
  m_infoStack.pop_back();
  return;
}

void CXFA_Graphics::SetLineCap(CFX_GraphStateData::LineCap lineCap) {
  m_info.graphState.m_LineCap = lineCap;
}

void CXFA_Graphics::SetLineDash(float dashPhase,
                                const float* dashArray,
                                size_t dashCount) {
  ASSERT(dashArray);
  ASSERT(dashCount);

  float scale = m_info.isActOnDash ? m_info.graphState.m_LineWidth : 1.0;
  m_info.graphState.m_DashPhase = dashPhase;
  m_info.graphState.m_DashArray.resize(dashCount);
  for (size_t i = 0; i < dashCount; i++)
    m_info.graphState.m_DashArray[i] = dashArray[i] * scale;
}

void CXFA_Graphics::SetSolidLineDash() {
  m_info.graphState.m_DashArray.clear();
}

void CXFA_Graphics::SetLineWidth(float lineWidth) {
  m_info.graphState.m_LineWidth = lineWidth;
}

void CXFA_Graphics::EnableActOnDash() {
  m_info.isActOnDash = true;
}

void CXFA_Graphics::SetStrokeColor(const CXFA_GEColor& color) {
  m_info.strokeColor = color;
}

void CXFA_Graphics::SetFillColor(const CXFA_GEColor& color) {
    m_info.fillColor = color;
}

void CXFA_Graphics::StrokePath(CXFA_GEPath* path, const CFX_Matrix* matrix) {
  if (path)
    RenderDeviceStrokePath(path, matrix);
}

void CXFA_Graphics::FillPath(CXFA_GEPath* path,
                             FX_FillMode fillMode,
                             const CFX_Matrix* matrix) {
  if (path)
    RenderDeviceFillPath(path, fillMode, matrix);
}

void CXFA_Graphics::ConcatMatrix(const CFX_Matrix* matrix) {
  if (matrix)
    m_info.CTM.Concat(*matrix);
}

const CFX_Matrix* CXFA_Graphics::GetMatrix() const {
  return &m_info.CTM;
}

CFX_RectF CXFA_Graphics::GetClipRect() const {
  FX_RECT r = m_renderDevice->GetClipBox();
  return CFX_RectF(r.left, r.top, r.Width(), r.Height());
}

void CXFA_Graphics::SetClipRect(const CFX_RectF& rect) {
  m_renderDevice->SetClip_Rect(
      FX_RECT(FXSYS_round(rect.left), FXSYS_round(rect.top),
              FXSYS_round(rect.right()), FXSYS_round(rect.bottom())));
}

CFX_RenderDevice* CXFA_Graphics::GetRenderDevice() {
  return m_renderDevice;
}

void CXFA_Graphics::RenderDeviceStrokePath(const CXFA_GEPath* path,
                                           const CFX_Matrix* matrix) {
  if (m_info.strokeColor.GetType() != CXFA_GEColor::Solid)
    return;

  CFX_Matrix m = m_info.CTM;
  if (matrix)
    m.Concat(*matrix);

  m_renderDevice->DrawPath(path->GetPathData(), &m, &m_info.graphState, 0x0,
                           m_info.strokeColor.GetArgb(), 0);
}

void CXFA_Graphics::RenderDeviceFillPath(const CXFA_GEPath* path,
                                         FX_FillMode fillMode,
                                         const CFX_Matrix* matrix) {
  CFX_Matrix m = m_info.CTM;
  if (matrix)
    m.Concat(*matrix);

  switch (m_info.fillColor.GetType()) {
    case CXFA_GEColor::Solid:
      m_renderDevice->DrawPath(path->GetPathData(), &m, &m_info.graphState,
                               m_info.fillColor.GetArgb(), 0x0, fillMode);
      return;
    case CXFA_GEColor::Pattern:
      FillPathWithPattern(path, fillMode, m);
      return;
    case CXFA_GEColor::Shading:
      FillPathWithShading(path, fillMode, m);
      return;
    default:
      return;
  }
}

void CXFA_Graphics::FillPathWithPattern(const CXFA_GEPath* path,
                                        FX_FillMode fillMode,
                                        const CFX_Matrix& matrix) {
  RetainPtr<CFX_DIBitmap> bitmap = m_renderDevice->GetBitmap();
  int32_t width = bitmap->GetWidth();
  int32_t height = bitmap->GetHeight();
  auto bmp = pdfium::MakeRetain<CFX_DIBitmap>();
  bmp->Create(width, height, FXDIB_Argb);
  m_renderDevice->GetDIBits(bmp, 0, 0);

  FX_HatchStyle hatchStyle = m_info.fillColor.GetPattern()->m_hatchStyle;
  const FX_HATCHDATA& data =
      GetHatchBitmapData(static_cast<size_t>(hatchStyle));

  auto mask = pdfium::MakeRetain<CFX_DIBitmap>();
  mask->Create(data.width, data.height, FXDIB_1bppMask);
  memcpy(mask->GetBuffer(), data.maskBits, mask->GetPitch() * data.height);
  const CFX_FloatRect rectf =
      matrix.TransformRect(path->GetPathData()->GetBoundingBox());
  const FX_RECT rect = rectf.ToRoundedFxRect();

  CFX_DefaultRenderDevice device;
  device.Attach(bmp, false, nullptr, false);
  device.FillRect(rect, m_info.fillColor.GetPattern()->m_backArgb);
  for (int32_t j = rect.bottom; j < rect.top; j += mask->GetHeight()) {
    for (int32_t i = rect.left; i < rect.right; i += mask->GetWidth())
      device.SetBitMask(mask, i, j, m_info.fillColor.GetPattern()->m_foreArgb);
  }
  CFX_RenderDevice::StateRestorer restorer(m_renderDevice);
  m_renderDevice->SetClip_PathFill(path->GetPathData(), &matrix, fillMode);
  SetDIBitsWithMatrix(bmp, CFX_Matrix());
}

void CXFA_Graphics::FillPathWithShading(const CXFA_GEPath* path,
                                        FX_FillMode fillMode,
                                        const CFX_Matrix& matrix) {
  RetainPtr<CFX_DIBitmap> bitmap = m_renderDevice->GetBitmap();
  int32_t width = bitmap->GetWidth();
  int32_t height = bitmap->GetHeight();
  float start_x = m_info.fillColor.GetShading()->m_beginPoint.x;
  float start_y = m_info.fillColor.GetShading()->m_beginPoint.y;
  float end_x = m_info.fillColor.GetShading()->m_endPoint.x;
  float end_y = m_info.fillColor.GetShading()->m_endPoint.y;
  auto bmp = pdfium::MakeRetain<CFX_DIBitmap>();
  bmp->Create(width, height, FXDIB_Argb);
  m_renderDevice->GetDIBits(bmp, 0, 0);
  int32_t pitch = bmp->GetPitch();
  bool result = false;
  switch (m_info.fillColor.GetShading()->m_type) {
    case FX_SHADING_Axial: {
      float x_span = end_x - start_x;
      float y_span = end_y - start_y;
      float axis_len_square = (x_span * x_span) + (y_span * y_span);
      for (int32_t row = 0; row < height; row++) {
        uint32_t* dib_buf = (uint32_t*)(bmp->GetBuffer() + row * pitch);
        for (int32_t column = 0; column < width; column++) {
          float x = (float)(column);
          float y = (float)(row);
          float scale = (((x - start_x) * x_span) + ((y - start_y) * y_span)) /
                        axis_len_square;
          if (scale < 0) {
            if (!m_info.fillColor.GetShading()->m_isExtendedBegin) {
              continue;
            }
            scale = 0;
          } else if (scale > 1.0f) {
            if (!m_info.fillColor.GetShading()->m_isExtendedEnd) {
              continue;
            }
            scale = 1.0f;
          }
          int32_t index = (int32_t)(scale * (FX_SHADING_Steps - 1));
          dib_buf[column] = m_info.fillColor.GetShading()->m_argbArray[index];
        }
      }
      result = true;
      break;
    }
    case FX_SHADING_Radial: {
      float start_r = m_info.fillColor.GetShading()->m_beginRadius;
      float end_r = m_info.fillColor.GetShading()->m_endRadius;
      float a = ((start_x - end_x) * (start_x - end_x)) +
                ((start_y - end_y) * (start_y - end_y)) -
                ((start_r - end_r) * (start_r - end_r));
      for (int32_t row = 0; row < height; row++) {
        uint32_t* dib_buf = (uint32_t*)(bmp->GetBuffer() + row * pitch);
        for (int32_t column = 0; column < width; column++) {
          float x = (float)(column);
          float y = (float)(row);
          float b = -2 * (((x - start_x) * (end_x - start_x)) +
                          ((y - start_y) * (end_y - start_y)) +
                          (start_r * (end_r - start_r)));
          float c = ((x - start_x) * (x - start_x)) +
                    ((y - start_y) * (y - start_y)) - (start_r * start_r);
          float s;
          if (a == 0) {
            s = -c / b;
          } else {
            float b2_4ac = (b * b) - 4 * (a * c);
            if (b2_4ac < 0) {
              continue;
            }
            float root = (sqrt(b2_4ac));
            float s1, s2;
            if (a > 0) {
              s1 = (-b - root) / (2 * a);
              s2 = (-b + root) / (2 * a);
            } else {
              s2 = (-b - root) / (2 * a);
              s1 = (-b + root) / (2 * a);
            }
            if (s2 <= 1.0f || m_info.fillColor.GetShading()->m_isExtendedEnd) {
              s = (s2);
            } else {
              s = (s1);
            }
            if ((start_r) + s * (end_r - start_r) < 0) {
              continue;
            }
          }
          if (s < 0) {
            if (!m_info.fillColor.GetShading()->m_isExtendedBegin) {
              continue;
            }
            s = 0;
          }
          if (s > 1.0f) {
            if (!m_info.fillColor.GetShading()->m_isExtendedEnd) {
              continue;
            }
            s = 1.0f;
          }
          int index = (int32_t)(s * (FX_SHADING_Steps - 1));
          dib_buf[column] = m_info.fillColor.GetShading()->m_argbArray[index];
        }
      }
      result = true;
      break;
    }
    default: {
      result = false;
      break;
    }
  }
  if (result) {
    CFX_RenderDevice::StateRestorer restorer(m_renderDevice);
    m_renderDevice->SetClip_PathFill(path->GetPathData(), &matrix, fillMode);
    SetDIBitsWithMatrix(bmp, matrix);
  }
}

void CXFA_Graphics::SetDIBitsWithMatrix(const RetainPtr<CFX_DIBBase>& source,
                                        const CFX_Matrix& matrix) {
  if (matrix.IsIdentity()) {
    m_renderDevice->SetDIBits(source, 0, 0);
  } else {
    CFX_Matrix m((float)source->GetWidth(), 0, 0, (float)source->GetHeight(), 0,
                 0);
    m.Concat(matrix);
    int32_t left;
    int32_t top;
    RetainPtr<CFX_DIBitmap> bmp1 = source->FlipImage(false, true);
    RetainPtr<CFX_DIBitmap> bmp2 = bmp1->TransformTo(m, &left, &top);
    m_renderDevice->SetDIBits(bmp2, left, top);
  }
}

CXFA_Graphics::TInfo::TInfo()
    : isActOnDash(false), strokeColor(nullptr), fillColor(nullptr) {}

CXFA_Graphics::TInfo::TInfo(const TInfo& info)
    : graphState(info.graphState),
      CTM(info.CTM),
      isActOnDash(info.isActOnDash),
      strokeColor(info.strokeColor),
      fillColor(info.fillColor) {}

CXFA_Graphics::TInfo& CXFA_Graphics::TInfo::operator=(const TInfo& other) {
  graphState = other.graphState;
  CTM = other.CTM;
  isActOnDash = other.isActOnDash;
  strokeColor = other.strokeColor;
  fillColor = other.fillColor;
  return *this;
}
