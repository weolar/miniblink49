// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include <windows.h>

#include <objidl.h>

#include <algorithm>
#include <memory>
#include <sstream>
#include <utility>

#include "core/fxcrt/fx_system.h"
#include "core/fxge/cfx_gemodule.h"
#include "core/fxge/cfx_graphstatedata.h"
#include "core/fxge/cfx_pathdata.h"
#include "core/fxge/win32/cfx_windowsdib.h"
#include "core/fxge/win32/win32_int.h"

// Has to come before gdiplus.h
namespace Gdiplus {
using std::min;
using std::max;
}  // namespace Gdiplus

#include <gdiplus.h>  // NOLINT

namespace {

enum {
  FuncId_GdipCreatePath2,
  FuncId_GdipSetPenDashArray,
  FuncId_GdipSetPenLineJoin,
  FuncId_GdipCreateFromHDC,
  FuncId_GdipSetPageUnit,
  FuncId_GdipSetSmoothingMode,
  FuncId_GdipCreateSolidFill,
  FuncId_GdipFillPath,
  FuncId_GdipDeleteBrush,
  FuncId_GdipCreatePen1,
  FuncId_GdipSetPenMiterLimit,
  FuncId_GdipDrawPath,
  FuncId_GdipDeletePen,
  FuncId_GdipDeletePath,
  FuncId_GdipDeleteGraphics,
  FuncId_GdipCreateBitmapFromFileICM,
  FuncId_GdipCreateBitmapFromStreamICM,
  FuncId_GdipGetImageHeight,
  FuncId_GdipGetImageWidth,
  FuncId_GdipGetImagePixelFormat,
  FuncId_GdipBitmapLockBits,
  FuncId_GdipGetImagePaletteSize,
  FuncId_GdipGetImagePalette,
  FuncId_GdipBitmapUnlockBits,
  FuncId_GdipDisposeImage,
  FuncId_GdipCreateBitmapFromScan0,
  FuncId_GdipSetImagePalette,
  FuncId_GdipSetInterpolationMode,
  FuncId_GdipDrawImagePointsI,
  FuncId_GdiplusStartup,
  FuncId_GdipDrawLineI,
  FuncId_GdipCreatePath,
  FuncId_GdipSetPathFillMode,
  FuncId_GdipSetClipRegion,
  FuncId_GdipWidenPath,
  FuncId_GdipAddPathLine,
  FuncId_GdipAddPathRectangle,
  FuncId_GdipDeleteRegion,
  FuncId_GdipSetPenLineCap197819,
  FuncId_GdipSetPenDashOffset,
  FuncId_GdipCreateMatrix2,
  FuncId_GdipDeleteMatrix,
  FuncId_GdipSetWorldTransform,
  FuncId_GdipSetPixelOffsetMode,
};

LPCSTR g_GdipFuncNames[] = {
    "GdipCreatePath2",
    "GdipSetPenDashArray",
    "GdipSetPenLineJoin",
    "GdipCreateFromHDC",
    "GdipSetPageUnit",
    "GdipSetSmoothingMode",
    "GdipCreateSolidFill",
    "GdipFillPath",
    "GdipDeleteBrush",
    "GdipCreatePen1",
    "GdipSetPenMiterLimit",
    "GdipDrawPath",
    "GdipDeletePen",
    "GdipDeletePath",
    "GdipDeleteGraphics",
    "GdipCreateBitmapFromFileICM",
    "GdipCreateBitmapFromStreamICM",
    "GdipGetImageHeight",
    "GdipGetImageWidth",
    "GdipGetImagePixelFormat",
    "GdipBitmapLockBits",
    "GdipGetImagePaletteSize",
    "GdipGetImagePalette",
    "GdipBitmapUnlockBits",
    "GdipDisposeImage",
    "GdipCreateBitmapFromScan0",
    "GdipSetImagePalette",
    "GdipSetInterpolationMode",
    "GdipDrawImagePointsI",
    "GdiplusStartup",
    "GdipDrawLineI",
    "GdipCreatePath",
    "GdipSetPathFillMode",
    "GdipSetClipRegion",
    "GdipWidenPath",
    "GdipAddPathLine",
    "GdipAddPathRectangle",
    "GdipDeleteRegion",
    "GdipSetPenLineCap197819",
    "GdipSetPenDashOffset",
    "GdipCreateMatrix2",
    "GdipDeleteMatrix",
    "GdipSetWorldTransform",
    "GdipSetPixelOffsetMode",
};
static_assert(FX_ArraySize(g_GdipFuncNames) ==
                  static_cast<size_t>(FuncId_GdipSetPixelOffsetMode) + 1,
              "g_GdipFuncNames has wrong size");

typedef Gdiplus::GpStatus(WINGDIPAPI* FuncType_GdipCreatePath2)(
    GDIPCONST Gdiplus::GpPointF*,
    GDIPCONST BYTE*,
    INT,
    Gdiplus::GpFillMode,
    Gdiplus::GpPath** path);
typedef Gdiplus::GpStatus(WINGDIPAPI* FuncType_GdipSetPenDashArray)(
    Gdiplus::GpPen* pen,
    GDIPCONST Gdiplus::REAL* dash,
    INT count);
typedef Gdiplus::GpStatus(WINGDIPAPI* FuncType_GdipSetPenLineJoin)(
    Gdiplus::GpPen* pen,
    Gdiplus::GpLineJoin lineJoin);
typedef Gdiplus::GpStatus(WINGDIPAPI* FuncType_GdipCreateFromHDC)(
    HDC hdc,
    Gdiplus::GpGraphics** graphics);
typedef Gdiplus::GpStatus(WINGDIPAPI* FuncType_GdipSetPageUnit)(
    Gdiplus::GpGraphics* graphics,
    Gdiplus::GpUnit unit);
typedef Gdiplus::GpStatus(WINGDIPAPI* FuncType_GdipSetSmoothingMode)(
    Gdiplus::GpGraphics* graphics,
    Gdiplus::SmoothingMode smoothingMode);
typedef Gdiplus::GpStatus(WINGDIPAPI* FuncType_GdipCreateSolidFill)(
    Gdiplus::ARGB color,
    Gdiplus::GpSolidFill** brush);
typedef Gdiplus::GpStatus(WINGDIPAPI* FuncType_GdipFillPath)(
    Gdiplus::GpGraphics* graphics,
    Gdiplus::GpBrush* brush,
    Gdiplus::GpPath* path);
typedef Gdiplus::GpStatus(WINGDIPAPI* FuncType_GdipDeleteBrush)(
    Gdiplus::GpBrush* brush);
typedef Gdiplus::GpStatus(WINGDIPAPI* FuncType_GdipCreatePen1)(
    Gdiplus::ARGB color,
    Gdiplus::REAL width,
    Gdiplus::GpUnit unit,
    Gdiplus::GpPen** pen);
typedef Gdiplus::GpStatus(WINGDIPAPI* FuncType_GdipSetPenMiterLimit)(
    Gdiplus::GpPen* pen,
    Gdiplus::REAL miterLimit);
typedef Gdiplus::GpStatus(WINGDIPAPI* FuncType_GdipDrawPath)(
    Gdiplus::GpGraphics* graphics,
    Gdiplus::GpPen* pen,
    Gdiplus::GpPath* path);
typedef Gdiplus::GpStatus(WINGDIPAPI* FuncType_GdipDeletePen)(
    Gdiplus::GpPen* pen);
typedef Gdiplus::GpStatus(WINGDIPAPI* FuncType_GdipDeletePath)(
    Gdiplus::GpPath* path);
typedef Gdiplus::GpStatus(WINGDIPAPI* FuncType_GdipDeleteGraphics)(
    Gdiplus::GpGraphics* graphics);
typedef Gdiplus::GpStatus(WINGDIPAPI* FuncType_GdipCreateBitmapFromFileICM)(
    GDIPCONST WCHAR* filename,
    Gdiplus::GpBitmap** bitmap);
typedef Gdiplus::GpStatus(WINGDIPAPI* FuncType_GdipCreateBitmapFromStreamICM)(
    IStream* stream,
    Gdiplus::GpBitmap** bitmap);
typedef Gdiplus::GpStatus(WINGDIPAPI* FuncType_GdipGetImageWidth)(
    Gdiplus::GpImage* image,
    UINT* width);
typedef Gdiplus::GpStatus(WINGDIPAPI* FuncType_GdipGetImageHeight)(
    Gdiplus::GpImage* image,
    UINT* height);
typedef Gdiplus::GpStatus(WINGDIPAPI* FuncType_GdipGetImagePixelFormat)(
    Gdiplus::GpImage* image,
    Gdiplus::PixelFormat* format);
typedef Gdiplus::GpStatus(WINGDIPAPI* FuncType_GdipBitmapLockBits)(
    Gdiplus::GpBitmap* bitmap,
    GDIPCONST Gdiplus::GpRect* rect,
    UINT flags,
    Gdiplus::PixelFormat format,
    Gdiplus::BitmapData* lockedBitmapData);
typedef Gdiplus::GpStatus(WINGDIPAPI* FuncType_GdipGetImagePalette)(
    Gdiplus::GpImage* image,
    Gdiplus::ColorPalette* palette,
    INT size);
typedef Gdiplus::GpStatus(WINGDIPAPI* FuncType_GdipGetImagePaletteSize)(
    Gdiplus::GpImage* image,
    INT* size);
typedef Gdiplus::GpStatus(WINGDIPAPI* FuncType_GdipBitmapUnlockBits)(
    Gdiplus::GpBitmap* bitmap,
    Gdiplus::BitmapData* lockedBitmapData);
typedef Gdiplus::GpStatus(WINGDIPAPI* FuncType_GdipDisposeImage)(
    Gdiplus::GpImage* image);
typedef Gdiplus::GpStatus(WINGDIPAPI* FuncType_GdipCreateBitmapFromScan0)(
    INT width,
    INT height,
    INT stride,
    Gdiplus::PixelFormat format,
    BYTE* scan0,
    Gdiplus::GpBitmap** bitmap);
typedef Gdiplus::GpStatus(WINGDIPAPI* FuncType_GdipSetImagePalette)(
    Gdiplus::GpImage* image,
    GDIPCONST Gdiplus::ColorPalette* palette);
typedef Gdiplus::GpStatus(WINGDIPAPI* FuncType_GdipSetInterpolationMode)(
    Gdiplus::GpGraphics* graphics,
    Gdiplus::InterpolationMode interpolationMode);
typedef Gdiplus::GpStatus(WINGDIPAPI* FuncType_GdipDrawImagePointsI)(
    Gdiplus::GpGraphics* graphics,
    Gdiplus::GpImage* image,
    GDIPCONST Gdiplus::GpPoint* dstpoints,
    INT count);
typedef Gdiplus::Status(WINAPI* FuncType_GdiplusStartup)(
    OUT uintptr_t* token,
    const Gdiplus::GdiplusStartupInput* input,
    OUT Gdiplus::GdiplusStartupOutput* output);
typedef Gdiplus::GpStatus(WINGDIPAPI* FuncType_GdipDrawLineI)(
    Gdiplus::GpGraphics* graphics,
    Gdiplus::GpPen* pen,
    int x1,
    int y1,
    int x2,
    int y2);
typedef Gdiplus::GpStatus(WINGDIPAPI* FuncType_GdipCreatePath)(
    Gdiplus::GpFillMode brushMode,
    Gdiplus::GpPath** path);
typedef Gdiplus::GpStatus(WINGDIPAPI* FuncType_GdipSetPathFillMode)(
    Gdiplus::GpPath* path,
    Gdiplus::GpFillMode fillmode);
typedef Gdiplus::GpStatus(WINGDIPAPI* FuncType_GdipSetClipRegion)(
    Gdiplus::GpGraphics* graphics,
    Gdiplus::GpRegion* region,
    Gdiplus::CombineMode combineMode);
typedef Gdiplus::GpStatus(WINGDIPAPI* FuncType_GdipWidenPath)(
    Gdiplus::GpPath* nativePath,
    Gdiplus::GpPen* pen,
    Gdiplus::GpMatrix* matrix,
    Gdiplus::REAL flatness);
typedef Gdiplus::GpStatus(WINGDIPAPI* FuncType_GdipAddPathLine)(
    Gdiplus::GpPath* path,
    Gdiplus::REAL x1,
    Gdiplus::REAL y1,
    Gdiplus::REAL x2,
    Gdiplus::REAL y2);
typedef Gdiplus::GpStatus(WINGDIPAPI* FuncType_GdipAddPathRectangle)(
    Gdiplus::GpPath* path,
    Gdiplus::REAL x,
    Gdiplus::REAL y,
    Gdiplus::REAL width,
    Gdiplus::REAL height);
typedef Gdiplus::GpStatus(WINGDIPAPI* FuncType_GdipDeleteRegion)(
    Gdiplus::GpRegion* region);
typedef Gdiplus::GpStatus(WINGDIPAPI* FuncType_GdipSetPenLineCap197819)(
    Gdiplus::GpPen* pen,
    Gdiplus::GpLineCap startCap,
    Gdiplus::GpLineCap endCap,
    Gdiplus::GpDashCap dashCap);
typedef Gdiplus::GpStatus(WINGDIPAPI* FuncType_GdipSetPenDashOffset)(
    Gdiplus::GpPen* pen,
    Gdiplus::REAL offset);
typedef Gdiplus::GpStatus(WINGDIPAPI* FuncType_GdipCreateMatrix2)(
    Gdiplus::REAL m11,
    Gdiplus::REAL m12,
    Gdiplus::REAL m21,
    Gdiplus::REAL m22,
    Gdiplus::REAL dx,
    Gdiplus::REAL dy,
    Gdiplus::GpMatrix** matrix);
typedef Gdiplus::GpStatus(WINGDIPAPI* FuncType_GdipDeleteMatrix)(
    Gdiplus::GpMatrix* matrix);
typedef Gdiplus::GpStatus(WINGDIPAPI* FuncType_GdipSetWorldTransform)(
    Gdiplus::GpGraphics* graphics,
    Gdiplus::GpMatrix* matrix);
typedef Gdiplus::GpStatus(WINGDIPAPI* FuncType_GdipSetPixelOffsetMode)(
    Gdiplus::GpGraphics* graphics,
    Gdiplus::PixelOffsetMode pixelOffsetMode);
#define CallFunc(funcname)               \
  reinterpret_cast<FuncType_##funcname>( \
      GdiplusExt.m_Functions[FuncId_##funcname])

Gdiplus::GpFillMode GdiFillType2Gdip(int fill_type) {
  return fill_type == ALTERNATE ? Gdiplus::FillModeAlternate
                                : Gdiplus::FillModeWinding;
}

const CGdiplusExt& GetGdiplusExt() {
  auto* pData =
      reinterpret_cast<CWin32Platform*>(CFX_GEModule::Get()->GetPlatformData());
  return pData->m_GdiplusExt;
}

Gdiplus::GpBrush* GdipCreateBrushImpl(DWORD argb) {
  const CGdiplusExt& GdiplusExt = GetGdiplusExt();
  Gdiplus::GpSolidFill* solidBrush = nullptr;
  CallFunc(GdipCreateSolidFill)((Gdiplus::ARGB)argb, &solidBrush);
  return solidBrush;
}

void OutputImage(Gdiplus::GpGraphics* pGraphics,
                 const RetainPtr<CFX_DIBitmap>& pBitmap,
                 const FX_RECT* pSrcRect,
                 int dest_left,
                 int dest_top,
                 int dest_width,
                 int dest_height) {
  int src_width = pSrcRect->Width(), src_height = pSrcRect->Height();
  const CGdiplusExt& GdiplusExt = GetGdiplusExt();
  if (pBitmap->GetBPP() == 1 && (pSrcRect->left % 8)) {
    FX_RECT new_rect(0, 0, src_width, src_height);
    RetainPtr<CFX_DIBitmap> pCloned = pBitmap->Clone(pSrcRect);
    if (!pCloned)
      return;
    OutputImage(pGraphics, pCloned, &new_rect, dest_left, dest_top, dest_width,
                dest_height);
    return;
  }
  int src_pitch = pBitmap->GetPitch();
  uint8_t* scan0 = pBitmap->GetBuffer() + pSrcRect->top * src_pitch +
                   pBitmap->GetBPP() * pSrcRect->left / 8;
  Gdiplus::GpBitmap* bitmap = nullptr;
  switch (pBitmap->GetFormat()) {
    case FXDIB_Argb:
      CallFunc(GdipCreateBitmapFromScan0)(src_width, src_height, src_pitch,
                                          PixelFormat32bppARGB, scan0, &bitmap);
      break;
    case FXDIB_Rgb32:
      CallFunc(GdipCreateBitmapFromScan0)(src_width, src_height, src_pitch,
                                          PixelFormat32bppRGB, scan0, &bitmap);
      break;
    case FXDIB_Rgb:
      CallFunc(GdipCreateBitmapFromScan0)(src_width, src_height, src_pitch,
                                          PixelFormat24bppRGB, scan0, &bitmap);
      break;
    case FXDIB_8bppRgb: {
      CallFunc(GdipCreateBitmapFromScan0)(src_width, src_height, src_pitch,
                                          PixelFormat8bppIndexed, scan0,
                                          &bitmap);
      UINT pal[258];
      pal[0] = 0;
      pal[1] = 256;
      for (int i = 0; i < 256; i++)
        pal[i + 2] = pBitmap->GetPaletteArgb(i);
      CallFunc(GdipSetImagePalette)(bitmap, (Gdiplus::ColorPalette*)pal);
      break;
    }
    case FXDIB_1bppRgb: {
      CallFunc(GdipCreateBitmapFromScan0)(src_width, src_height, src_pitch,
                                          PixelFormat1bppIndexed, scan0,
                                          &bitmap);
      break;
    }
  }
  if (dest_height < 0) {
    dest_height--;
  }
  if (dest_width < 0) {
    dest_width--;
  }
  Gdiplus::Point destinationPoints[] = {
      Gdiplus::Point(dest_left, dest_top),
      Gdiplus::Point(dest_left + dest_width, dest_top),
      Gdiplus::Point(dest_left, dest_top + dest_height)};
  CallFunc(GdipDrawImagePointsI)(pGraphics, bitmap, destinationPoints, 3);
  CallFunc(GdipDisposeImage)(bitmap);
}

Gdiplus::GpPen* GdipCreatePenImpl(const CFX_GraphStateData* pGraphState,
                                  const CFX_Matrix* pMatrix,
                                  DWORD argb,
                                  bool bTextMode) {
  const CGdiplusExt& GdiplusExt = GetGdiplusExt();
  float width = pGraphState->m_LineWidth;
  if (!bTextMode) {
    float unit = pMatrix
                     ? 1.0f / ((pMatrix->GetXUnit() + pMatrix->GetYUnit()) / 2)
                     : 1.0f;
    width = std::max(width, unit);
  }
  Gdiplus::GpPen* pPen = nullptr;
  CallFunc(GdipCreatePen1)((Gdiplus::ARGB)argb, width, Gdiplus::UnitWorld,
                           &pPen);
  Gdiplus::LineCap lineCap = Gdiplus::LineCapFlat;
  Gdiplus::DashCap dashCap = Gdiplus::DashCapFlat;
  bool bDashExtend = false;
  switch (pGraphState->m_LineCap) {
    case CFX_GraphStateData::LineCapButt:
      lineCap = Gdiplus::LineCapFlat;
      break;
    case CFX_GraphStateData::LineCapRound:
      lineCap = Gdiplus::LineCapRound;
      dashCap = Gdiplus::DashCapRound;
      bDashExtend = true;
      break;
    case CFX_GraphStateData::LineCapSquare:
      lineCap = Gdiplus::LineCapSquare;
      bDashExtend = true;
      break;
  }
  CallFunc(GdipSetPenLineCap197819)(pPen, lineCap, lineCap, dashCap);
  Gdiplus::LineJoin lineJoin = Gdiplus::LineJoinMiterClipped;
  switch (pGraphState->m_LineJoin) {
    case CFX_GraphStateData::LineJoinMiter:
      lineJoin = Gdiplus::LineJoinMiterClipped;
      break;
    case CFX_GraphStateData::LineJoinRound:
      lineJoin = Gdiplus::LineJoinRound;
      break;
    case CFX_GraphStateData::LineJoinBevel:
      lineJoin = Gdiplus::LineJoinBevel;
      break;
  }
  CallFunc(GdipSetPenLineJoin)(pPen, lineJoin);
  if (!pGraphState->m_DashArray.empty()) {
    float* pDashArray =
        FX_Alloc(float, FxAlignToBoundary<2>(pGraphState->m_DashArray.size()));
    int nCount = 0;
    float on_leftover = 0, off_leftover = 0;
    for (size_t i = 0; i < pGraphState->m_DashArray.size(); i += 2) {
      float on_phase = pGraphState->m_DashArray[i];
      float off_phase;
      if (i == pGraphState->m_DashArray.size() - 1)
        off_phase = on_phase;
      else
        off_phase = pGraphState->m_DashArray[i + 1];
      on_phase /= width;
      off_phase /= width;
      if (on_phase + off_phase <= 0.00002f) {
        on_phase = 1.0f / 10;
        off_phase = 1.0f / 10;
      }
      if (bDashExtend) {
        if (off_phase < 1)
          off_phase = 0;
        else
          off_phase -= 1;
        on_phase += 1;
      }
      if (on_phase == 0 || off_phase == 0) {
        if (nCount == 0) {
          on_leftover += on_phase;
          off_leftover += off_phase;
        } else {
          pDashArray[nCount - 2] += on_phase;
          pDashArray[nCount - 1] += off_phase;
        }
      } else {
        pDashArray[nCount++] = on_phase + on_leftover;
        on_leftover = 0;
        pDashArray[nCount++] = off_phase + off_leftover;
        off_leftover = 0;
      }
    }
    CallFunc(GdipSetPenDashArray)(pPen, pDashArray, nCount);
    float phase = pGraphState->m_DashPhase;
    if (bDashExtend) {
      if (phase < 0.5f)
        phase = 0;
      else
        phase -= 0.5f;
    }
    CallFunc(GdipSetPenDashOffset)(pPen, phase);
    FX_Free(pDashArray);
    pDashArray = nullptr;
  }
  CallFunc(GdipSetPenMiterLimit)(pPen, pGraphState->m_MiterLimit);
  return pPen;
}

Optional<std::pair<size_t, size_t>> IsSmallTriangle(Gdiplus::PointF* points,
                                                    const CFX_Matrix* pMatrix) {
  size_t pairs[] = {1, 2, 0, 2, 0, 1};
  for (size_t i = 0; i < FX_ArraySize(pairs) / 2; i++) {
    size_t pair1 = pairs[i * 2];
    size_t pair2 = pairs[i * 2 + 1];

    CFX_PointF p1(points[pair1].X, points[pair1].Y);
    CFX_PointF p2(points[pair2].X, points[pair2].Y);
    if (pMatrix) {
      p1 = pMatrix->Transform(p1);
      p2 = pMatrix->Transform(p2);
    }

    CFX_PointF diff = p1 - p2;
    float distance_square = (diff.x * diff.x) + (diff.y * diff.y);
    if (distance_square < (1.0f * 2 + 1.0f / 4))
      return std::make_pair(i, pair1);
  }
  return {};
}

class GpStream final : public IStream {
 public:
  GpStream() : m_RefCount(1), m_ReadPos(0) {}
  ~GpStream() = default;

  // IUnknown
  HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid,
                                           void** ppvObject) override {
    if (iid == __uuidof(IUnknown) || iid == __uuidof(IStream) ||
        iid == __uuidof(ISequentialStream)) {
      *ppvObject = static_cast<IStream*>(this);
      AddRef();
      return S_OK;
    }
    return E_NOINTERFACE;
  }
  ULONG STDMETHODCALLTYPE AddRef() override {
    return (ULONG)InterlockedIncrement(&m_RefCount);
  }
  ULONG STDMETHODCALLTYPE Release() override {
    ULONG res = (ULONG)InterlockedDecrement(&m_RefCount);
    if (res == 0) {
      delete this;
    }
    return res;
  }

  // ISequentialStream
  HRESULT STDMETHODCALLTYPE Read(void* output,
                                 ULONG cb,
                                 ULONG* pcbRead) override {
    if (pcbRead)
      *pcbRead = 0;

    if (m_ReadPos >= m_InterStream.tellp())
      return HRESULT_FROM_WIN32(ERROR_END_OF_MEDIA);

    size_t bytes_left = m_InterStream.tellp() - m_ReadPos;
    size_t bytes_out =
        std::min(pdfium::base::checked_cast<size_t>(cb), bytes_left);
    memcpy(output, m_InterStream.str().c_str() + m_ReadPos, bytes_out);
    m_ReadPos += bytes_out;
    if (pcbRead)
      *pcbRead = (ULONG)bytes_out;

    return S_OK;
  }
  HRESULT STDMETHODCALLTYPE Write(const void* input,
                                  ULONG cb,
                                  ULONG* pcbWritten) override {
    if (cb <= 0) {
      if (pcbWritten)
        *pcbWritten = 0;
      return S_OK;
    }
    m_InterStream.write(reinterpret_cast<const char*>(input), cb);
    if (pcbWritten)
      *pcbWritten = cb;
    return S_OK;
  }

  // IStream
  HRESULT STDMETHODCALLTYPE SetSize(ULARGE_INTEGER) override {
    return E_NOTIMPL;
  }
  HRESULT STDMETHODCALLTYPE CopyTo(IStream*,
                                   ULARGE_INTEGER,
                                   ULARGE_INTEGER*,
                                   ULARGE_INTEGER*) override {
    return E_NOTIMPL;
  }
  HRESULT STDMETHODCALLTYPE Commit(DWORD) override { return E_NOTIMPL; }
  HRESULT STDMETHODCALLTYPE Revert() override { return E_NOTIMPL; }
  HRESULT STDMETHODCALLTYPE LockRegion(ULARGE_INTEGER,
                                       ULARGE_INTEGER,
                                       DWORD) override {
    return E_NOTIMPL;
  }
  HRESULT STDMETHODCALLTYPE UnlockRegion(ULARGE_INTEGER,
                                         ULARGE_INTEGER,
                                         DWORD) override {
    return E_NOTIMPL;
  }
  HRESULT STDMETHODCALLTYPE Clone(IStream** stream) override {
    return E_NOTIMPL;
  }
  HRESULT STDMETHODCALLTYPE Seek(LARGE_INTEGER liDistanceToMove,
                                 DWORD dwOrigin,
                                 ULARGE_INTEGER* lpNewFilePointer) override {
    std::streamoff start;
    std::streamoff new_read_position;
    switch (dwOrigin) {
      case STREAM_SEEK_SET:
        start = 0;
        break;
      case STREAM_SEEK_CUR:
        start = m_ReadPos;
        break;
      case STREAM_SEEK_END:
        if (m_InterStream.tellp() < 0)
          return STG_E_SEEKERROR;
        start = m_InterStream.tellp();
        break;
      default:
        return STG_E_INVALIDFUNCTION;
    }
    new_read_position = start + liDistanceToMove.QuadPart;
    if (new_read_position > m_InterStream.tellp())
      return STG_E_SEEKERROR;

    m_ReadPos = new_read_position;
    if (lpNewFilePointer)
      lpNewFilePointer->QuadPart = m_ReadPos;

    return S_OK;
  }
  HRESULT STDMETHODCALLTYPE Stat(STATSTG* pStatstg,
                                 DWORD grfStatFlag) override {
    if (!pStatstg)
      return STG_E_INVALIDFUNCTION;

    ZeroMemory(pStatstg, sizeof(STATSTG));

    if (m_InterStream.tellp() < 0)
      return STG_E_SEEKERROR;

    pStatstg->cbSize.QuadPart = m_InterStream.tellp();
    return S_OK;
  }

 private:
  LONG m_RefCount;
  std::streamoff m_ReadPos;
  std::ostringstream m_InterStream;
};

struct PREVIEW3_DIBITMAP {
  BITMAPINFO* pbmi;
  int Stride;
  LPBYTE pScan0;
  Gdiplus::GpBitmap* pBitmap;
  Gdiplus::BitmapData* pBitmapData;
  GpStream* pStream;
};

PREVIEW3_DIBITMAP* LoadDIBitmap(WINDIB_Open_Args_ args) {
  Gdiplus::GpBitmap* pBitmap;
  GpStream* pStream = nullptr;
  const CGdiplusExt& GdiplusExt = GetGdiplusExt();
  Gdiplus::Status status = Gdiplus::Ok;
  if (args.flags == WINDIB_OPEN_PATHNAME) {
    status = CallFunc(GdipCreateBitmapFromFileICM)(args.path_name, &pBitmap);
  } else {
    if (args.memory_size == 0 || !args.memory_base)
      return nullptr;

    pStream = new GpStream;
    pStream->Write(args.memory_base, (ULONG)args.memory_size, nullptr);
    status = CallFunc(GdipCreateBitmapFromStreamICM)(pStream, &pBitmap);
  }
  if (status != Gdiplus::Ok) {
    if (pStream)
      pStream->Release();

    return nullptr;
  }
  UINT height, width;
  CallFunc(GdipGetImageHeight)(pBitmap, &height);
  CallFunc(GdipGetImageWidth)(pBitmap, &width);
  Gdiplus::PixelFormat pixel_format;
  CallFunc(GdipGetImagePixelFormat)(pBitmap, &pixel_format);
  int info_size = sizeof(BITMAPINFOHEADER);
  int bpp = 24;
  int dest_pixel_format = PixelFormat24bppRGB;
  if (pixel_format == PixelFormat1bppIndexed) {
    info_size += 8;
    bpp = 1;
    dest_pixel_format = PixelFormat1bppIndexed;
  } else if (pixel_format == PixelFormat8bppIndexed) {
    info_size += 1024;
    bpp = 8;
    dest_pixel_format = PixelFormat8bppIndexed;
  } else if (pixel_format == PixelFormat32bppARGB) {
    bpp = 32;
    dest_pixel_format = PixelFormat32bppARGB;
  }
  LPBYTE buf = FX_Alloc(BYTE, info_size);
  BITMAPINFOHEADER* pbmih = (BITMAPINFOHEADER*)buf;
  pbmih->biBitCount = bpp;
  pbmih->biCompression = BI_RGB;
  pbmih->biHeight = -(int)height;
  pbmih->biPlanes = 1;
  pbmih->biWidth = width;
  Gdiplus::Rect rect(0, 0, width, height);
  Gdiplus::BitmapData* pBitmapData = FX_Alloc(Gdiplus::BitmapData, 1);
  CallFunc(GdipBitmapLockBits)(pBitmap, &rect, Gdiplus::ImageLockModeRead,
                               dest_pixel_format, pBitmapData);
  if (pixel_format == PixelFormat1bppIndexed ||
      pixel_format == PixelFormat8bppIndexed) {
    DWORD* ppal = (DWORD*)(buf + sizeof(BITMAPINFOHEADER));
    struct {
      UINT flags;
      UINT Count;
      DWORD Entries[256];
    } pal;
    int size = 0;
    CallFunc(GdipGetImagePaletteSize)(pBitmap, &size);
    CallFunc(GdipGetImagePalette)(pBitmap, (Gdiplus::ColorPalette*)&pal, size);
    int entries = pixel_format == PixelFormat1bppIndexed ? 2 : 256;
    for (int i = 0; i < entries; i++) {
      ppal[i] = pal.Entries[i] & 0x00ffffff;
    }
  }
  PREVIEW3_DIBITMAP* pInfo = FX_Alloc(PREVIEW3_DIBITMAP, 1);
  pInfo->pbmi = (BITMAPINFO*)buf;
  pInfo->pScan0 = (LPBYTE)pBitmapData->Scan0;
  pInfo->Stride = pBitmapData->Stride;
  pInfo->pBitmap = pBitmap;
  pInfo->pBitmapData = pBitmapData;
  pInfo->pStream = pStream;
  return pInfo;
}

void FreeDIBitmap(PREVIEW3_DIBITMAP* pInfo) {
  const CGdiplusExt& GdiplusExt = GetGdiplusExt();
  CallFunc(GdipBitmapUnlockBits)(pInfo->pBitmap, pInfo->pBitmapData);
  CallFunc(GdipDisposeImage)(pInfo->pBitmap);
  FX_Free(pInfo->pBitmapData);
  FX_Free((LPBYTE)pInfo->pbmi);
  if (pInfo->pStream)
    pInfo->pStream->Release();
  FX_Free(pInfo);
}

}  // namespace

CGdiplusExt::CGdiplusExt() {}

CGdiplusExt::~CGdiplusExt() {
//   FreeLibrary(m_GdiModule);
//   FreeLibrary(m_hModule);
}

void CGdiplusExt::Load() {
  static BOOL s_isLoaded = FALSE;
  if (s_isLoaded)
    return;
  s_isLoaded = TRUE; // TODO: Multi thread..

  char buf[MAX_PATH];
  GetSystemDirectoryA(buf, MAX_PATH);
  ByteString dllpath = buf;
  dllpath += "\\GDIPLUS.DLL";
  m_hModule = LoadLibraryA(dllpath.c_str());
  if (!m_hModule)
    return;

  m_Functions.resize(FX_ArraySize(g_GdipFuncNames));
  for (size_t i = 0; i < FX_ArraySize(g_GdipFuncNames); ++i) {
    m_Functions[i] = GetProcAddress(m_hModule, g_GdipFuncNames[i]);
    if (!m_Functions[i]) {
      m_hModule = nullptr;
      return;
    }
  }

  uintptr_t gdiplusToken;
  Gdiplus::GdiplusStartupInput gdiplusStartupInput;
  ((FuncType_GdiplusStartup)m_Functions[FuncId_GdiplusStartup])(
      &gdiplusToken, &gdiplusStartupInput, nullptr);
  m_GdiModule = LoadLibraryA("GDI32.DLL");
}

bool CGdiplusExt::StretchDIBits(HDC hDC,
                                const RetainPtr<CFX_DIBitmap>& pBitmap,
                                int dest_left,
                                int dest_top,
                                int dest_width,
                                int dest_height,
                                const FX_RECT* pClipRect,
                                const FXDIB_ResampleOptions& options) {
  Gdiplus::GpGraphics* pGraphics;
  const CGdiplusExt& GdiplusExt = GetGdiplusExt();
  CallFunc(GdipCreateFromHDC)(hDC, &pGraphics);
  CallFunc(GdipSetPageUnit)(pGraphics, Gdiplus::UnitPixel);
  if (options.bNoSmoothing) {
    CallFunc(GdipSetInterpolationMode)(
        pGraphics, Gdiplus::InterpolationModeNearestNeighbor);
  } else if (pBitmap->GetWidth() > abs(dest_width) / 2 ||
             pBitmap->GetHeight() > abs(dest_height) / 2) {
    CallFunc(GdipSetInterpolationMode)(pGraphics,
                                       Gdiplus::InterpolationModeHighQuality);
  } else {
    CallFunc(GdipSetInterpolationMode)(pGraphics,
                                       Gdiplus::InterpolationModeBilinear);
  }
  FX_RECT src_rect(0, 0, pBitmap->GetWidth(), pBitmap->GetHeight());
  OutputImage(pGraphics, pBitmap, &src_rect, dest_left, dest_top, dest_width,
              dest_height);
  CallFunc(GdipDeleteGraphics)(pGraphics);
  CallFunc(GdipDeleteGraphics)(pGraphics);
  return true;
}

bool CGdiplusExt::DrawPath(HDC hDC,
                           const CFX_PathData* pPathData,
                           const CFX_Matrix* pObject2Device,
                           const CFX_GraphStateData* pGraphState,
                           uint32_t fill_argb,
                           uint32_t stroke_argb,
                           int fill_mode) {
  auto& pPoints = pPathData->GetPoints();
  if (pPoints.empty())
    return true;

  Gdiplus::GpGraphics* pGraphics = nullptr;
  const CGdiplusExt& GdiplusExt = GetGdiplusExt();
  CallFunc(GdipCreateFromHDC)(hDC, &pGraphics);
  CallFunc(GdipSetPageUnit)(pGraphics, Gdiplus::UnitPixel);
  CallFunc(GdipSetPixelOffsetMode)(pGraphics, Gdiplus::PixelOffsetModeHalf);
  Gdiplus::GpMatrix* pMatrix = nullptr;
  if (pObject2Device) {
    CallFunc(GdipCreateMatrix2)(pObject2Device->a, pObject2Device->b,
                                pObject2Device->c, pObject2Device->d,
                                pObject2Device->e, pObject2Device->f, &pMatrix);
    CallFunc(GdipSetWorldTransform)(pGraphics, pMatrix);
  }
  Gdiplus::PointF* points = FX_Alloc(Gdiplus::PointF, pPoints.size());
  BYTE* types = FX_Alloc(BYTE, pPoints.size());
  int nSubPathes = 0;
  bool bSubClose = false;
  int pos_subclose = 0;
  bool bSmooth = false;
  int startpoint = 0;
  for (size_t i = 0; i < pPoints.size(); i++) {
    points[i].X = pPoints[i].m_Point.x;
    points[i].Y = pPoints[i].m_Point.y;

    CFX_PointF pos = pPoints[i].m_Point;
    if (pObject2Device)
      pos = pObject2Device->Transform(pos);

    if (pos.x > 50000 * 1.0f)
      points[i].X = 50000 * 1.0f;
    if (pos.x < -50000 * 1.0f)
      points[i].X = -50000 * 1.0f;
    if (pos.y > 50000 * 1.0f)
      points[i].Y = 50000 * 1.0f;
    if (pos.y < -50000 * 1.0f)
      points[i].Y = -50000 * 1.0f;

    FXPT_TYPE point_type = pPoints[i].m_Type;
    if (point_type == FXPT_TYPE::MoveTo) {
      types[i] = Gdiplus::PathPointTypeStart;
      nSubPathes++;
      bSubClose = false;
      startpoint = i;
    } else if (point_type == FXPT_TYPE::LineTo) {
      types[i] = Gdiplus::PathPointTypeLine;
      if (pPoints[i - 1].IsTypeAndOpen(FXPT_TYPE::MoveTo) &&
          (i == pPoints.size() - 1 ||
           pPoints[i + 1].IsTypeAndOpen(FXPT_TYPE::MoveTo)) &&
          points[i].Y == points[i - 1].Y && points[i].X == points[i - 1].X) {
        points[i].X += 0.01f;
        continue;
      }
      if (!bSmooth && points[i].X != points[i - 1].X &&
          points[i].Y != points[i - 1].Y)
        bSmooth = true;
    } else if (point_type == FXPT_TYPE::BezierTo) {
      types[i] = Gdiplus::PathPointTypeBezier;
      bSmooth = true;
    }
    if (pPoints[i].m_CloseFigure) {
      if (bSubClose)
        types[pos_subclose] &= ~Gdiplus::PathPointTypeCloseSubpath;
      else
        bSubClose = true;
      pos_subclose = i;
      types[i] |= Gdiplus::PathPointTypeCloseSubpath;
      if (!bSmooth && points[i].X != points[startpoint].X &&
          points[i].Y != points[startpoint].Y)
        bSmooth = true;
    }
  }
  if (fill_mode & FXFILL_NOPATHSMOOTH) {
    bSmooth = false;
    CallFunc(GdipSetSmoothingMode)(pGraphics, Gdiplus::SmoothingModeNone);
  } else if (!(fill_mode & FXFILL_FULLCOVER)) {
    if (!bSmooth && (fill_mode & 3))
      bSmooth = true;

    if (bSmooth || (pGraphState && pGraphState->m_LineWidth > 2)) {
      CallFunc(GdipSetSmoothingMode)(pGraphics,
                                     Gdiplus::SmoothingModeAntiAlias);
    }
  }
  int new_fill_mode = fill_mode & 3;
  if (pPoints.size() == 4 && !pGraphState) {
    auto indices = IsSmallTriangle(points, pObject2Device);
    if (indices.has_value()) {
      size_t v1;
      size_t v2;
      std::tie(v1, v2) = indices.value();
      Gdiplus::GpPen* pPen = nullptr;
      CallFunc(GdipCreatePen1)(fill_argb, 1.0f, Gdiplus::UnitPixel, &pPen);
      CallFunc(GdipDrawLineI)(
          pGraphics, pPen, FXSYS_round(points[v1].X), FXSYS_round(points[v1].Y),
          FXSYS_round(points[v2].X), FXSYS_round(points[v2].Y));
      CallFunc(GdipDeletePen)(pPen);
      return true;
    }
  }
  Gdiplus::GpPath* pGpPath = nullptr;
  CallFunc(GdipCreatePath2)(points, types, pPoints.size(),
                            GdiFillType2Gdip(new_fill_mode), &pGpPath);
  if (!pGpPath) {
    if (pMatrix)
      CallFunc(GdipDeleteMatrix)(pMatrix);

    FX_Free(points);
    FX_Free(types);
    CallFunc(GdipDeleteGraphics)(pGraphics);
    return false;
  }
  if (new_fill_mode) {
    Gdiplus::GpBrush* pBrush = GdipCreateBrushImpl(fill_argb);
    CallFunc(GdipSetPathFillMode)(pGpPath, GdiFillType2Gdip(new_fill_mode));
    CallFunc(GdipFillPath)(pGraphics, pBrush, pGpPath);
    CallFunc(GdipDeleteBrush)(pBrush);
  }
  if (pGraphState && stroke_argb) {
    Gdiplus::GpPen* pPen =
        GdipCreatePenImpl(pGraphState, pObject2Device, stroke_argb,
                          !!(fill_mode & FX_STROKE_TEXT_MODE));
    if (nSubPathes == 1) {
      CallFunc(GdipDrawPath)(pGraphics, pPen, pGpPath);
    } else {
      int iStart = 0;
      for (size_t i = 0; i < pPoints.size(); i++) {
        if (i == pPoints.size() - 1 ||
            types[i + 1] == Gdiplus::PathPointTypeStart) {
          Gdiplus::GpPath* pSubPath;
          CallFunc(GdipCreatePath2)(points + iStart, types + iStart,
                                    i - iStart + 1,
                                    GdiFillType2Gdip(new_fill_mode), &pSubPath);
          iStart = i + 1;
          CallFunc(GdipDrawPath)(pGraphics, pPen, pSubPath);
          CallFunc(GdipDeletePath)(pSubPath);
        }
      }
    }
    CallFunc(GdipDeletePen)(pPen);
  }
  if (pMatrix)
    CallFunc(GdipDeleteMatrix)(pMatrix);
  FX_Free(points);
  FX_Free(types);
  CallFunc(GdipDeletePath)(pGpPath);
  CallFunc(GdipDeleteGraphics)(pGraphics);
  return true;
}

RetainPtr<CFX_DIBitmap> CGdiplusExt::LoadDIBitmap(WINDIB_Open_Args_ args) {
  PREVIEW3_DIBITMAP* pInfo = ::LoadDIBitmap(args);
  if (!pInfo)
    return nullptr;

  int height = abs(pInfo->pbmi->bmiHeader.biHeight);
  int width = pInfo->pbmi->bmiHeader.biWidth;
  int dest_pitch = (width * pInfo->pbmi->bmiHeader.biBitCount + 31) / 32 * 4;
  LPBYTE pData = FX_Alloc2D(BYTE, dest_pitch, height);
  if (dest_pitch == pInfo->Stride) {
    memcpy(pData, pInfo->pScan0, dest_pitch * height);
  } else {
    for (int i = 0; i < height; i++) {
      memcpy(pData + dest_pitch * i, pInfo->pScan0 + pInfo->Stride * i,
             dest_pitch);
    }
  }
  RetainPtr<CFX_DIBitmap> pDIBitmap = FX_WindowsDIB_LoadFromBuf(
      pInfo->pbmi, pData, pInfo->pbmi->bmiHeader.biBitCount == 32);
  FX_Free(pData);
  FreeDIBitmap(pInfo);
  return pDIBitmap;
}
