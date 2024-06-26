// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include <windows.h>

#include <algorithm>
#include <memory>
#include <vector>

#include "core/fxcrt/fx_system.h"
#include "core/fxge/cfx_font.h"
#include "core/fxge/cfx_windowsrenderdevice.h"
#include "core/fxge/dib/cfx_dibextractor.h"
#include "core/fxge/dib/cfx_dibitmap.h"
#include "core/fxge/dib/cfx_imagerenderer.h"
#include "core/fxge/dib/cstretchengine.h"
#include "core/fxge/fx_freetype.h"
#include "core/fxge/win32/cpsoutput.h"
#include "core/fxge/win32/win32_int.h"

#if defined(PDFIUM_PRINT_TEXT_WITH_GDI)
namespace {

class ScopedState {
 public:
  ScopedState(HDC hDC, HFONT hFont) : m_hDC(hDC) {
    m_iState = SaveDC(m_hDC);
    m_hFont = SelectObject(m_hDC, hFont);
  }

  ~ScopedState() {
    HGDIOBJ hFont = SelectObject(m_hDC, m_hFont);
    DeleteObject(hFont);
    RestoreDC(m_hDC, m_iState);
  }

 private:
  HDC m_hDC;
  HGDIOBJ m_hFont;
  int m_iState;

  ScopedState(const ScopedState&) = delete;
  void operator=(const ScopedState&) = delete;
};

}  // namespace

bool g_pdfium_print_text_with_gdi = false;

PDFiumEnsureTypefaceCharactersAccessible g_pdfium_typeface_accessible_func =
    nullptr;
#endif

CGdiPrinterDriver::CGdiPrinterDriver(HDC hDC)
    : CGdiDeviceDriver(hDC, FXDC_PRINTER),
      m_HorzSize(::GetDeviceCaps(m_hDC, HORZSIZE)),
      m_VertSize(::GetDeviceCaps(m_hDC, VERTSIZE)) {}

CGdiPrinterDriver::~CGdiPrinterDriver() {}

int CGdiPrinterDriver::GetDeviceCaps(int caps_id) const {
  if (caps_id == FXDC_HORZ_SIZE)
    return m_HorzSize;
  if (caps_id == FXDC_VERT_SIZE)
    return m_VertSize;
  return CGdiDeviceDriver::GetDeviceCaps(caps_id);
}

bool CGdiPrinterDriver::SetDIBits(const RetainPtr<CFX_DIBBase>& pSource,
                                  uint32_t color,
                                  const FX_RECT& src_rect,
                                  int left,
                                  int top,
                                  BlendMode blend_type) {
  if (pSource->IsAlphaMask()) {
    FX_RECT clip_rect(left, top, left + src_rect.Width(),
                      top + src_rect.Height());
    return StretchDIBits(pSource, color, left - src_rect.left,
                         top - src_rect.top, pSource->GetWidth(),
                         pSource->GetHeight(), &clip_rect,
                         FXDIB_ResampleOptions(), BlendMode::kNormal);
  }
  ASSERT(pSource);
  ASSERT(!pSource->IsAlphaMask());
  ASSERT(blend_type == BlendMode::kNormal);
  if (pSource->HasAlpha())
    return false;

  CFX_DIBExtractor temp(pSource);
  RetainPtr<CFX_DIBitmap> pBitmap = temp.GetBitmap();
  if (!pBitmap)
    return false;

  return GDI_SetDIBits(pBitmap, src_rect, left, top);
}

bool CGdiPrinterDriver::StretchDIBits(const RetainPtr<CFX_DIBBase>& pSource,
                                      uint32_t color,
                                      int dest_left,
                                      int dest_top,
                                      int dest_width,
                                      int dest_height,
                                      const FX_RECT* pClipRect,
                                      const FXDIB_ResampleOptions& options,
                                      BlendMode blend_type) {
  if (pSource->IsAlphaMask()) {
    int alpha = FXARGB_A(color);
    if (pSource->GetBPP() != 1 || alpha != 255)
      return false;

    if (dest_width < 0 || dest_height < 0) {
      RetainPtr<CFX_DIBitmap> pFlipped =
          pSource->FlipImage(dest_width < 0, dest_height < 0);
      if (!pFlipped)
        return false;

      if (dest_width < 0)
        dest_left += dest_width;
      if (dest_height < 0)
        dest_top += dest_height;

      return GDI_StretchBitMask(pFlipped, dest_left, dest_top, abs(dest_width),
                                abs(dest_height), color);
    }

    CFX_DIBExtractor temp(pSource);
    RetainPtr<CFX_DIBitmap> pBitmap = temp.GetBitmap();
    if (!pBitmap)
      return false;
    return GDI_StretchBitMask(pBitmap, dest_left, dest_top, dest_width,
                              dest_height, color);
  }

  if (pSource->HasAlpha())
    return false;

  if (dest_width < 0 || dest_height < 0) {
    RetainPtr<CFX_DIBitmap> pFlipped =
        pSource->FlipImage(dest_width < 0, dest_height < 0);
    if (!pFlipped)
      return false;

    if (dest_width < 0)
      dest_left += dest_width;
    if (dest_height < 0)
      dest_top += dest_height;

    return GDI_StretchDIBits(pFlipped, dest_left, dest_top, abs(dest_width),
                             abs(dest_height), options);
  }

  CFX_DIBExtractor temp(pSource);
  RetainPtr<CFX_DIBitmap> pBitmap = temp.GetBitmap();
  if (!pBitmap)
    return false;
  return GDI_StretchDIBits(pBitmap, dest_left, dest_top, dest_width,
                           dest_height, options);
}

bool CGdiPrinterDriver::StartDIBits(const RetainPtr<CFX_DIBBase>& pSource,
                                    int bitmap_alpha,
                                    uint32_t color,
                                    const CFX_Matrix& matrix,
                                    const FXDIB_ResampleOptions& options,
                                    std::unique_ptr<CFX_ImageRenderer>* handle,
                                    BlendMode blend_type) {
  if (bitmap_alpha < 255 || pSource->HasAlpha() ||
      (pSource->IsAlphaMask() && (pSource->GetBPP() != 1))) {
    return false;
  }
  CFX_FloatRect unit_rect = matrix.GetUnitRect();
  FX_RECT full_rect = unit_rect.GetOuterRect();
  if (fabs(matrix.b) < 0.5f && matrix.a != 0 && fabs(matrix.c) < 0.5f &&
      matrix.d != 0) {
    bool bFlipX = matrix.a < 0;
    bool bFlipY = matrix.d > 0;
    return StretchDIBits(pSource, color,
                         bFlipX ? full_rect.right : full_rect.left,
                         bFlipY ? full_rect.bottom : full_rect.top,
                         bFlipX ? -full_rect.Width() : full_rect.Width(),
                         bFlipY ? -full_rect.Height() : full_rect.Height(),
                         nullptr, FXDIB_ResampleOptions(), blend_type);
  }
  if (fabs(matrix.a) >= 0.5f || fabs(matrix.d) >= 0.5f)
    return false;

  RetainPtr<CFX_DIBitmap> pTransformed =
      pSource->SwapXY(matrix.c > 0, matrix.b < 0);
  if (!pTransformed)
    return false;

  return StretchDIBits(pTransformed, color, full_rect.left, full_rect.top,
                       full_rect.Width(), full_rect.Height(), nullptr,
                       FXDIB_ResampleOptions(), blend_type);
}

bool CGdiPrinterDriver::DrawDeviceText(int nChars,
                                       const FXTEXT_CHARPOS* pCharPos,
                                       CFX_Font* pFont,
                                       const CFX_Matrix* pObject2Device,
                                       float font_size,
                                       uint32_t color) {
#if defined(PDFIUM_PRINT_TEXT_WITH_GDI)
  if (!g_pdfium_print_text_with_gdi)
    return false;

  if (nChars < 1 || !pFont || !pFont->IsEmbedded() || !pFont->IsTTFont())
    return false;

  // Scale factor used to minimize the kerning problems caused by rounding
  // errors below. Value chosen based on the title of https://crbug.com/18383
  const double kScaleFactor = 10;

  // Font
  //
  // Note that |pFont| has the actual font to render with embedded within, but
  // but unfortunately AddFontMemResourceEx() does not seem to cooperate.
  // Loading font data to memory seems to work, but then enumerating the fonts
  // fails to find it. This requires more investigation. In the meanwhile,
  // assume the printing is happening on the machine that generated the PDF, so
  // the embedded font, if not a web font, is available through GDI anyway.
  // TODO(thestig): Figure out why AddFontMemResourceEx() does not work.
  // Generalize this method to work for all PDFs with embedded fonts.
  // In sandboxed environments, font loading may not work at all, so this may be
  // the best possible effort.
  LOGFONT lf = {};
  lf.lfHeight = -font_size * kScaleFactor;
  lf.lfWeight = pFont->IsBold() ? FW_BOLD : FW_NORMAL;
  lf.lfItalic = pFont->IsItalic();
  lf.lfCharSet = DEFAULT_CHARSET;

  const WideString wsName =
      WideString::FromUTF8(pFont->GetFaceName().AsStringView());
  size_t iNameLen =
      std::min(wsName.GetLength(), static_cast<size_t>(LF_FACESIZE - 1));
  memcpy(lf.lfFaceName, wsName.c_str(), sizeof(lf.lfFaceName[0]) * iNameLen);
  lf.lfFaceName[iNameLen] = 0;

  HFONT hFont = CreateFontIndirect(&lf);
  if (!hFont)
    return false;

  ScopedState state(m_hDC, hFont);
  size_t nTextMetricSize = GetOutlineTextMetrics(m_hDC, 0, nullptr);
  if (nTextMetricSize == 0) {
    // Give up and fail if there is no way to get the font to try again.
    if (!g_pdfium_typeface_accessible_func)
      return false;

    // Try to get the font. Any letter will do.
    g_pdfium_typeface_accessible_func(&lf, L"A", 1);
    nTextMetricSize = GetOutlineTextMetrics(m_hDC, 0, nullptr);
    if (nTextMetricSize == 0)
      return false;
  }

  std::vector<BYTE> buf(nTextMetricSize);
  OUTLINETEXTMETRIC* pTextMetric =
      reinterpret_cast<OUTLINETEXTMETRIC*>(buf.data());
  if (GetOutlineTextMetrics(m_hDC, nTextMetricSize, pTextMetric) == 0)
    return false;

  // If the selected font is not the requested font, then bail out. This can
  // happen with web fonts, for example.
  wchar_t* wsSelectedName = reinterpret_cast<wchar_t*>(
      buf.data() + reinterpret_cast<size_t>(pTextMetric->otmpFaceName));
  if (wsName != wsSelectedName)
    return false;

  // Transforms
  SetGraphicsMode(m_hDC, GM_ADVANCED);
  XFORM xform;
  xform.eM11 = pObject2Device->a / kScaleFactor;
  xform.eM12 = pObject2Device->b / kScaleFactor;
  xform.eM21 = -pObject2Device->c / kScaleFactor;
  xform.eM22 = -pObject2Device->d / kScaleFactor;
  xform.eDx = pObject2Device->e;
  xform.eDy = pObject2Device->f;
  ModifyWorldTransform(m_hDC, &xform, MWT_LEFTMULTIPLY);

  // Color
  FX_COLORREF colorref = ArgbToColorRef(color);
  SetTextColor(m_hDC, colorref);
  SetBkMode(m_hDC, TRANSPARENT);

  // Text
  WideString wsText;
  std::vector<INT> spacing(nChars);
  float fPreviousOriginX = 0;
  for (int i = 0; i < nChars; ++i) {
    // Only works with PDFs from Skia's PDF generator. Cannot handle arbitrary
    // values from PDFs.
    const FXTEXT_CHARPOS& charpos = pCharPos[i];
    ASSERT(charpos.m_AdjustMatrix[0] == 0);
    ASSERT(charpos.m_AdjustMatrix[1] == 0);
    ASSERT(charpos.m_AdjustMatrix[2] == 0);
    ASSERT(charpos.m_AdjustMatrix[3] == 0);
    ASSERT(charpos.m_Origin.y == 0);

    // Round the spacing to the nearest integer, but keep track of the rounding
    // error for calculating the next spacing value.
    float fOriginX = charpos.m_Origin.x * kScaleFactor;
    float fPixelSpacing = fOriginX - fPreviousOriginX;
    spacing[i] = FXSYS_round(fPixelSpacing);
    fPreviousOriginX = fOriginX - (fPixelSpacing - spacing[i]);

    wsText += charpos.m_GlyphIndex;
  }

  // Draw
  SetTextAlign(m_hDC, TA_LEFT | TA_BASELINE);
  if (ExtTextOutW(m_hDC, 0, 0, ETO_GLYPH_INDEX, nullptr, wsText.c_str(), nChars,
                  nChars > 1 ? &spacing[1] : nullptr)) {
    return true;
  }

  // Give up and fail if there is no way to get the font to try again.
  if (!g_pdfium_typeface_accessible_func)
    return false;

  // Try to get the font and draw again.
  g_pdfium_typeface_accessible_func(&lf, wsText.c_str(), nChars);
  return !!ExtTextOutW(m_hDC, 0, 0, ETO_GLYPH_INDEX, nullptr, wsText.c_str(),
                       nChars, nChars > 1 ? &spacing[1] : nullptr);
#else
  return false;
#endif
}

CPSPrinterDriver::CPSPrinterDriver(HDC hDC,
                                   WindowsPrintMode mode,
                                   bool bCmykOutput)
    : m_hDC(hDC), m_bCmykOutput(bCmykOutput) {
  // |mode| should be PostScript.
  ASSERT(mode == WindowsPrintMode::kModePostScript2 ||
         mode == WindowsPrintMode::kModePostScript3 ||
         mode == WindowsPrintMode::kModePostScript2PassThrough ||
         mode == WindowsPrintMode::kModePostScript3PassThrough);
  int pslevel = (mode == WindowsPrintMode::kModePostScript2 ||
                 mode == WindowsPrintMode::kModePostScript2PassThrough)
                    ? 2
                    : 3;
  CPSOutput::OutputMode output_mode =
      (mode == WindowsPrintMode::kModePostScript2 ||
       mode == WindowsPrintMode::kModePostScript3)
          ? CPSOutput::OutputMode::kGdiComment
          : CPSOutput::OutputMode::kExtEscape;

  m_HorzSize = ::GetDeviceCaps(m_hDC, HORZSIZE);
  m_VertSize = ::GetDeviceCaps(m_hDC, VERTSIZE);
  m_Width = ::GetDeviceCaps(m_hDC, HORZRES);
  m_Height = ::GetDeviceCaps(m_hDC, VERTRES);
  m_nBitsPerPixel = ::GetDeviceCaps(m_hDC, BITSPIXEL);

  m_PSRenderer.Init(pdfium::MakeRetain<CPSOutput>(m_hDC, output_mode), pslevel,
                    m_Width, m_Height, bCmykOutput);
  HRGN hRgn = ::CreateRectRgn(0, 0, 1, 1);
  int ret = ::GetClipRgn(hDC, hRgn);
  if (ret == 1) {
    ret = ::GetRegionData(hRgn, 0, NULL);
    if (ret) {
      RGNDATA* pData = reinterpret_cast<RGNDATA*>(FX_Alloc(uint8_t, ret));
      ret = ::GetRegionData(hRgn, ret, pData);
      if (ret) {
        CFX_PathData path;
        for (uint32_t i = 0; i < pData->rdh.nCount; i++) {
          RECT* pRect =
              reinterpret_cast<RECT*>(pData->Buffer + pData->rdh.nRgnSize * i);
          path.AppendRect(static_cast<float>(pRect->left),
                          static_cast<float>(pRect->bottom),
                          static_cast<float>(pRect->right),
                          static_cast<float>(pRect->top));
        }
        m_PSRenderer.SetClip_PathFill(&path, nullptr, FXFILL_WINDING);
      }
      FX_Free(pData);
    }
  }
  ::DeleteObject(hRgn);
}

CPSPrinterDriver::~CPSPrinterDriver() {
  EndRendering();
}

int CPSPrinterDriver::GetDeviceCaps(int caps_id) const {
  switch (caps_id) {
    case FXDC_DEVICE_CLASS:
      return FXDC_PRINTER;
    case FXDC_PIXEL_WIDTH:
      return m_Width;
    case FXDC_PIXEL_HEIGHT:
      return m_Height;
    case FXDC_BITS_PIXEL:
      return m_nBitsPerPixel;
    case FXDC_RENDER_CAPS:
      return m_bCmykOutput ? FXRC_BIT_MASK | FXRC_CMYK_OUTPUT : FXRC_BIT_MASK;
    case FXDC_HORZ_SIZE:
      return m_HorzSize;
    case FXDC_VERT_SIZE:
      return m_VertSize;
  }
  return 0;
}

bool CPSPrinterDriver::StartRendering() {
  return m_PSRenderer.StartRendering();
}

void CPSPrinterDriver::EndRendering() {
  m_PSRenderer.EndRendering();
}

void CPSPrinterDriver::SaveState() {
  m_PSRenderer.SaveState();
}

void CPSPrinterDriver::RestoreState(bool bKeepSaved) {
  m_PSRenderer.RestoreState(bKeepSaved);
}

bool CPSPrinterDriver::SetClip_PathFill(const CFX_PathData* pPathData,
                                        const CFX_Matrix* pObject2Device,
                                        int fill_mode) {
  m_PSRenderer.SetClip_PathFill(pPathData, pObject2Device, fill_mode);
  return true;
}

bool CPSPrinterDriver::SetClip_PathStroke(
    const CFX_PathData* pPathData,
    const CFX_Matrix* pObject2Device,
    const CFX_GraphStateData* pGraphState) {
  m_PSRenderer.SetClip_PathStroke(pPathData, pObject2Device, pGraphState);
  return true;
}

bool CPSPrinterDriver::DrawPath(const CFX_PathData* pPathData,
                                const CFX_Matrix* pObject2Device,
                                const CFX_GraphStateData* pGraphState,
                                FX_ARGB fill_color,
                                FX_ARGB stroke_color,
                                int fill_mode,
                                BlendMode blend_type) {
  if (blend_type != BlendMode::kNormal) {
    return false;
  }
  return m_PSRenderer.DrawPath(pPathData, pObject2Device, pGraphState,
                               fill_color, stroke_color, fill_mode & 3);
}

bool CPSPrinterDriver::GetClipBox(FX_RECT* pRect) {
  *pRect = m_PSRenderer.GetClipBox();
  return true;
}

bool CPSPrinterDriver::SetDIBits(const RetainPtr<CFX_DIBBase>& pBitmap,
                                 uint32_t color,
                                 const FX_RECT& src_rect,
                                 int left,
                                 int top,
                                 BlendMode blend_type) {
  if (blend_type != BlendMode::kNormal)
    return false;
  return m_PSRenderer.SetDIBits(pBitmap, color, left, top);
}

bool CPSPrinterDriver::StretchDIBits(const RetainPtr<CFX_DIBBase>& pBitmap,
                                     uint32_t color,
                                     int dest_left,
                                     int dest_top,
                                     int dest_width,
                                     int dest_height,
                                     const FX_RECT* pClipRect,
                                     const FXDIB_ResampleOptions& options,
                                     BlendMode blend_type) {
  if (blend_type != BlendMode::kNormal)
    return false;
  return m_PSRenderer.StretchDIBits(pBitmap, color, dest_left, dest_top,
                                    dest_width, dest_height, options);
}

bool CPSPrinterDriver::StartDIBits(const RetainPtr<CFX_DIBBase>& pBitmap,
                                   int bitmap_alpha,
                                   uint32_t color,
                                   const CFX_Matrix& matrix,
                                   const FXDIB_ResampleOptions& options,
                                   std::unique_ptr<CFX_ImageRenderer>* handle,
                                   BlendMode blend_type) {
  if (blend_type != BlendMode::kNormal)
    return false;

  if (bitmap_alpha < 255)
    return false;

  *handle = nullptr;
  return m_PSRenderer.DrawDIBits(pBitmap, color, matrix, options);
}

bool CPSPrinterDriver::DrawDeviceText(int nChars,
                                      const FXTEXT_CHARPOS* pCharPos,
                                      CFX_Font* pFont,
                                      const CFX_Matrix* pObject2Device,
                                      float font_size,
                                      uint32_t color) {
  return m_PSRenderer.DrawText(nChars, pCharPos, pFont, pObject2Device,
                               font_size, color);
}

CTextOnlyPrinterDriver::CTextOnlyPrinterDriver(HDC hDC)
    : m_hDC(hDC),
      m_Width(INT_MAX),
      m_Height(INT_MAX),
      m_HorzSize(INT_MAX),
      m_VertSize(INT_MAX),
      m_OriginY(0.0f),
      m_SetOrigin(false) {
  m_nBitsPerPixel = ::GetDeviceCaps(m_hDC, BITSPIXEL);
}

CTextOnlyPrinterDriver::~CTextOnlyPrinterDriver() {
  EndRendering();
}

int CTextOnlyPrinterDriver::GetDeviceCaps(int caps_id) const {
  switch (caps_id) {
    case FXDC_DEVICE_CLASS:
      return FXDC_PRINTER;
    case FXDC_PIXEL_WIDTH:
      return m_Width;
    case FXDC_PIXEL_HEIGHT:
      return m_Height;
    case FXDC_BITS_PIXEL:
      return m_nBitsPerPixel;
    case FXDC_RENDER_CAPS:
      return 0;
    case FXDC_HORZ_SIZE:
      return m_HorzSize;
    case FXDC_VERT_SIZE:
      return m_VertSize;
  }
  return 0;
}

bool CTextOnlyPrinterDriver::SetClip_PathFill(const CFX_PathData* pPathData,
                                              const CFX_Matrix* pObject2Device,
                                              int fill_mode) {
  return true;
}

bool CTextOnlyPrinterDriver::SetClip_PathStroke(
    const CFX_PathData* pPathData,
    const CFX_Matrix* pObject2Device,
    const CFX_GraphStateData* pGraphState) {
  return false;
}

bool CTextOnlyPrinterDriver::DrawPath(const CFX_PathData* pPathData,
                                      const CFX_Matrix* pObject2Device,
                                      const CFX_GraphStateData* pGraphState,
                                      uint32_t fill_color,
                                      uint32_t stroke_color,
                                      int fill_mode,
                                      BlendMode blend_type) {
  return false;
}

bool CTextOnlyPrinterDriver::SetDIBits(const RetainPtr<CFX_DIBBase>& pBitmap,
                                       uint32_t color,
                                       const FX_RECT& src_rect,
                                       int left,
                                       int top,
                                       BlendMode blend_type) {
  return false;
}

bool CTextOnlyPrinterDriver::GetClipBox(FX_RECT* pRect) {
  pRect->left = 0;
  pRect->right = m_Width;
  pRect->top = 0;
  pRect->bottom = m_Height;
  return true;
}

bool CTextOnlyPrinterDriver::StretchDIBits(
    const RetainPtr<CFX_DIBBase>& pBitmap,
    uint32_t color,
    int dest_left,
    int dest_top,
    int dest_width,
    int dest_height,
    const FX_RECT* pClipRect,
    const FXDIB_ResampleOptions& options,
    BlendMode blend_type) {
  return false;
}

bool CTextOnlyPrinterDriver::StartDIBits(
    const RetainPtr<CFX_DIBBase>& pBitmap,
    int bitmap_alpha,
    uint32_t color,
    const CFX_Matrix& matrix,
    const FXDIB_ResampleOptions& options,
    std::unique_ptr<CFX_ImageRenderer>* handle,
    BlendMode blend_type) {
  return false;
}

bool CTextOnlyPrinterDriver::DrawDeviceText(int nChars,
                                            const FXTEXT_CHARPOS* pCharPos,
                                            CFX_Font* pFont,
                                            const CFX_Matrix* pObject2Device,
                                            float font_size,
                                            uint32_t color) {
  if (g_pdfium_print_mode != 1)
    return false;
  if (nChars < 1 || !pFont || !pFont->IsEmbedded() || !pFont->IsTTFont())
    return false;

  // Scale factor used to minimize the kerning problems caused by rounding
  // errors below. Value chosen based on the title of https://crbug.com/18383
  const double kScaleFactor = 10;

  WideString wsText;
  int totalLength = nChars;

  // Detect new lines and add clrf characters (since this is Windows only).
  // These characters are removed by SkPDF, but the new line information is
  // preserved in the text location. clrf characters seem to be ignored by
  // label printers that use this driver.
  if (m_SetOrigin &&
      FXSYS_round(m_OriginY) != FXSYS_round(pObject2Device->f * kScaleFactor)) {
    wsText += L"\r\n";
    totalLength += 2;
  }
  m_OriginY = pObject2Device->f * kScaleFactor;
  m_SetOrigin = true;

  // Text
  for (int i = 0; i < nChars; ++i) {
    // Only works with PDFs from Skia's PDF generator. Cannot handle arbitrary
    // values from PDFs.
    const FXTEXT_CHARPOS& charpos = pCharPos[i];
    ASSERT(charpos.m_AdjustMatrix[0] == 0);
    ASSERT(charpos.m_AdjustMatrix[1] == 0);
    ASSERT(charpos.m_AdjustMatrix[2] == 0);
    ASSERT(charpos.m_AdjustMatrix[3] == 0);
    ASSERT(charpos.m_Origin.y == 0);

    wsText += charpos.m_Unicode;
  }
  size_t len = totalLength;
  ByteString text = wsText.ToDefANSI();
  while (len > 0) {
    char buffer[1026];
    size_t send_len = std::min(len, static_cast<size_t>(1024));
    *(reinterpret_cast<uint16_t*>(buffer)) = send_len;
    memcpy(buffer + 2, text.c_str(), send_len);
    ::GdiComment(m_hDC, send_len + 2, reinterpret_cast<const BYTE*>(buffer));
    len -= send_len;
    text.Right(len);
  }
  return true;
}
