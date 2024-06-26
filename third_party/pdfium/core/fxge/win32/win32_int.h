// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXGE_WIN32_WIN32_INT_H_
#define CORE_FXGE_WIN32_WIN32_INT_H_

#include <windows.h>

#include <memory>
#include <vector>

#include "core/fxcrt/retain_ptr.h"
#include "core/fxge/cfx_pathdata.h"
#include "core/fxge/cfx_windowsrenderdevice.h"
#include "core/fxge/renderdevicedriver_iface.h"
#include "core/fxge/win32/cfx_psrenderer.h"
#include "core/fxge/win32/cpsoutput.h"

class CFX_ImageRenderer;
class FXTEXT_CHARPOS;
struct WINDIB_Open_Args_;

RetainPtr<CFX_DIBitmap> FX_WindowsDIB_LoadFromBuf(BITMAPINFO* pbmi,
                                                  LPVOID pData,
                                                  bool bAlpha);
class CGdiplusExt {
 public:
  CGdiplusExt();
  ~CGdiplusExt();

  void Load();
  bool IsAvailable() { return !!m_hModule; }
  bool StretchDIBits(HDC hDC,
                     const RetainPtr<CFX_DIBitmap>& pBitmap,
                     int dest_left,
                     int dest_top,
                     int dest_width,
                     int dest_height,
                     const FX_RECT* pClipRect,
                     const FXDIB_ResampleOptions& options);
  bool DrawPath(HDC hDC,
                const CFX_PathData* pPathData,
                const CFX_Matrix* pObject2Device,
                const CFX_GraphStateData* pGraphState,
                uint32_t fill_argb,
                uint32_t stroke_argb,
                int fill_mode);

  RetainPtr<CFX_DIBitmap> LoadDIBitmap(WINDIB_Open_Args_ args);

  std::vector<FARPROC> m_Functions;

 protected:
  HMODULE m_hModule = nullptr;
  HMODULE m_GdiModule = nullptr;
};

class CWin32Platform {
 public:
  bool m_bHalfTone;
  CGdiplusExt m_GdiplusExt;
};

class CGdiDeviceDriver : public RenderDeviceDriverIface {
 protected:
  CGdiDeviceDriver(HDC hDC, int device_class);
  ~CGdiDeviceDriver() override;

  // RenderDeviceDriverIface
  int GetDeviceCaps(int caps_id) const override;
  void SaveState() override;
  void RestoreState(bool bKeepSaved) override;
  bool SetClip_PathFill(const CFX_PathData* pPathData,
                        const CFX_Matrix* pObject2Device,
                        int fill_mode) override;
  bool SetClip_PathStroke(const CFX_PathData* pPathData,
                          const CFX_Matrix* pObject2Device,
                          const CFX_GraphStateData* pGraphState) override;
  bool DrawPath(const CFX_PathData* pPathData,
                const CFX_Matrix* pObject2Device,
                const CFX_GraphStateData* pGraphState,
                uint32_t fill_color,
                uint32_t stroke_color,
                int fill_mode,
                BlendMode blend_type) override;
  bool FillRectWithBlend(const FX_RECT& rect,
                         uint32_t fill_color,
                         BlendMode blend_type) override;
  bool DrawCosmeticLine(const CFX_PointF& ptMoveTo,
                        const CFX_PointF& ptLineTo,
                        uint32_t color,
                        BlendMode blend_type) override;
  bool GetClipBox(FX_RECT* pRect) override;

  void DrawLine(float x1, float y1, float x2, float y2);

  bool GDI_SetDIBits(const RetainPtr<CFX_DIBitmap>& pBitmap,
                     const FX_RECT& src_rect,
                     int left,
                     int top);
  bool GDI_StretchDIBits(const RetainPtr<CFX_DIBitmap>& pBitmap,
                         int dest_left,
                         int dest_top,
                         int dest_width,
                         int dest_height,
                         const FXDIB_ResampleOptions& options);
  bool GDI_StretchBitMask(const RetainPtr<CFX_DIBitmap>& pBitmap,
                          int dest_left,
                          int dest_top,
                          int dest_width,
                          int dest_height,
                          uint32_t bitmap_color);

  HDC m_hDC;
  bool m_bMetafileDCType;
  int m_Width;
  int m_Height;
  int m_nBitsPerPixel;
  int m_DeviceClass;
  int m_RenderCaps;
};

class CGdiDisplayDriver final : public CGdiDeviceDriver {
 public:
  explicit CGdiDisplayDriver(HDC hDC);
  ~CGdiDisplayDriver() override;

 private:
  bool GetDIBits(const RetainPtr<CFX_DIBitmap>& pBitmap,
                 int left,
                 int top) override;
  bool SetDIBits(const RetainPtr<CFX_DIBBase>& pBitmap,
                 uint32_t color,
                 const FX_RECT& src_rect,
                 int left,
                 int top,
                 BlendMode blend_type) override;
  bool StretchDIBits(const RetainPtr<CFX_DIBBase>& pBitmap,
                     uint32_t color,
                     int dest_left,
                     int dest_top,
                     int dest_width,
                     int dest_height,
                     const FX_RECT* pClipRect,
                     const FXDIB_ResampleOptions& options,
                     BlendMode blend_type) override;
  bool StartDIBits(const RetainPtr<CFX_DIBBase>& pBitmap,
                   int bitmap_alpha,
                   uint32_t color,
                   const CFX_Matrix& matrix,
                   const FXDIB_ResampleOptions& options,
                   std::unique_ptr<CFX_ImageRenderer>* handle,
                   BlendMode blend_type) override;
  bool UseFoxitStretchEngine(const RetainPtr<CFX_DIBBase>& pSource,
                             uint32_t color,
                             int dest_left,
                             int dest_top,
                             int dest_width,
                             int dest_height,
                             const FX_RECT* pClipRect,
                             const FXDIB_ResampleOptions& options);
};

class CGdiPrinterDriver final : public CGdiDeviceDriver {
 public:
  explicit CGdiPrinterDriver(HDC hDC);
  ~CGdiPrinterDriver() override;

 private:
  int GetDeviceCaps(int caps_id) const override;
  bool SetDIBits(const RetainPtr<CFX_DIBBase>& pBitmap,
                 uint32_t color,
                 const FX_RECT& src_rect,
                 int left,
                 int top,
                 BlendMode blend_type) override;
  bool StretchDIBits(const RetainPtr<CFX_DIBBase>& pBitmap,
                     uint32_t color,
                     int dest_left,
                     int dest_top,
                     int dest_width,
                     int dest_height,
                     const FX_RECT* pClipRect,
                     const FXDIB_ResampleOptions& options,
                     BlendMode blend_type) override;
  bool StartDIBits(const RetainPtr<CFX_DIBBase>& pBitmap,
                   int bitmap_alpha,
                   uint32_t color,
                   const CFX_Matrix& matrix,
                   const FXDIB_ResampleOptions& options,
                   std::unique_ptr<CFX_ImageRenderer>* handle,
                   BlendMode blend_type) override;
  bool DrawDeviceText(int nChars,
                      const FXTEXT_CHARPOS* pCharPos,
                      CFX_Font* pFont,
                      const CFX_Matrix* pObject2Device,
                      float font_size,
                      uint32_t color) override;

  const int m_HorzSize;
  const int m_VertSize;
};

class CPSPrinterDriver final : public RenderDeviceDriverIface {
 public:
  CPSPrinterDriver(HDC hDC, WindowsPrintMode mode, bool bCmykOutput);
  ~CPSPrinterDriver() override;

 private:
  // RenderDeviceDriverIface
  int GetDeviceCaps(int caps_id) const override;
  bool StartRendering() override;
  void EndRendering() override;
  void SaveState() override;
  void RestoreState(bool bKeepSaved) override;
  bool SetClip_PathFill(const CFX_PathData* pPathData,
                        const CFX_Matrix* pObject2Device,
                        int fill_mode) override;
  bool SetClip_PathStroke(const CFX_PathData* pPathData,
                          const CFX_Matrix* pObject2Device,
                          const CFX_GraphStateData* pGraphState) override;
  bool DrawPath(const CFX_PathData* pPathData,
                const CFX_Matrix* pObject2Device,
                const CFX_GraphStateData* pGraphState,
                uint32_t fill_color,
                uint32_t stroke_color,
                int fill_mode,
                BlendMode blend_type) override;
  bool GetClipBox(FX_RECT* pRect) override;
  bool SetDIBits(const RetainPtr<CFX_DIBBase>& pBitmap,
                 uint32_t color,
                 const FX_RECT& src_rect,
                 int left,
                 int top,
                 BlendMode blend_type) override;
  bool StretchDIBits(const RetainPtr<CFX_DIBBase>& pBitmap,
                     uint32_t color,
                     int dest_left,
                     int dest_top,
                     int dest_width,
                     int dest_height,
                     const FX_RECT* pClipRect,
                     const FXDIB_ResampleOptions& options,
                     BlendMode blend_type) override;
  bool StartDIBits(const RetainPtr<CFX_DIBBase>& pBitmap,
                   int bitmap_alpha,
                   uint32_t color,
                   const CFX_Matrix& matrix,
                   const FXDIB_ResampleOptions& options,
                   std::unique_ptr<CFX_ImageRenderer>* handle,
                   BlendMode blend_type) override;
  bool DrawDeviceText(int nChars,
                      const FXTEXT_CHARPOS* pCharPos,
                      CFX_Font* pFont,
                      const CFX_Matrix* pObject2Device,
                      float font_size,
                      uint32_t color) override;

  HDC m_hDC;
  const bool m_bCmykOutput;
  int m_Width;
  int m_Height;
  int m_nBitsPerPixel;
  int m_HorzSize;
  int m_VertSize;
  CFX_PSRenderer m_PSRenderer;
};

class CTextOnlyPrinterDriver final : public RenderDeviceDriverIface {
 public:
  explicit CTextOnlyPrinterDriver(HDC hDC);
  ~CTextOnlyPrinterDriver() override;

 private:
  // RenderDeviceDriverIface
  int GetDeviceCaps(int caps_id) const override;
  void SaveState() override{};
  void RestoreState(bool bKeepSaved) override{};
  bool SetClip_PathFill(const CFX_PathData* pPathData,
                        const CFX_Matrix* pObject2Device,
                        int fill_mode) override;
  bool SetClip_PathStroke(const CFX_PathData* pPathData,
                          const CFX_Matrix* pObject2Device,
                          const CFX_GraphStateData* pGraphState) override;
  bool DrawPath(const CFX_PathData* pPathData,
                const CFX_Matrix* pObject2Device,
                const CFX_GraphStateData* pGraphState,
                uint32_t fill_color,
                uint32_t stroke_color,
                int fill_mode,
                BlendMode blend_type) override;
  bool GetClipBox(FX_RECT* pRect) override;
  bool SetDIBits(const RetainPtr<CFX_DIBBase>& pBitmap,
                 uint32_t color,
                 const FX_RECT& src_rect,
                 int left,
                 int top,
                 BlendMode blend_type) override;
  bool StretchDIBits(const RetainPtr<CFX_DIBBase>& pBitmap,
                     uint32_t color,
                     int dest_left,
                     int dest_top,
                     int dest_width,
                     int dest_height,
                     const FX_RECT* pClipRect,
                     const FXDIB_ResampleOptions& options,
                     BlendMode blend_type) override;
  bool StartDIBits(const RetainPtr<CFX_DIBBase>& pBitmap,
                   int bitmap_alpha,
                   uint32_t color,
                   const CFX_Matrix& matrix,
                   const FXDIB_ResampleOptions& options,
                   std::unique_ptr<CFX_ImageRenderer>* handle,
                   BlendMode blend_type) override;
  bool DrawDeviceText(int nChars,
                      const FXTEXT_CHARPOS* pCharPos,
                      CFX_Font* pFont,
                      const CFX_Matrix* pObject2Device,
                      float font_size,
                      uint32_t color) override;

  HDC m_hDC;
  const int m_Width;
  const int m_Height;
  int m_nBitsPerPixel;
  const int m_HorzSize;
  const int m_VertSize;
  float m_OriginY;
  bool m_SetOrigin;
};
#endif  // CORE_FXGE_WIN32_WIN32_INT_H_
