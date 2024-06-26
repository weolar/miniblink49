// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXGE_DIB_CFX_DIBBASE_H_
#define CORE_FXGE_DIB_CFX_DIBBASE_H_

#include <memory>

#include "core/fxcrt/fx_coordinates.h"
#include "core/fxcrt/fx_memory.h"
#include "core/fxcrt/retain_ptr.h"
#include "core/fxge/fx_dib.h"

enum FXDIB_Channel {
  FXDIB_Red = 1,
  FXDIB_Green,
  FXDIB_Blue,
  FXDIB_Cyan,
  FXDIB_Magenta,
  FXDIB_Yellow,
  FXDIB_Black,
  FXDIB_Alpha
};

class CFX_ClipRgn;
class CFX_DIBitmap;
class PauseIndicatorIface;

// Base class for all Device-Indepenent Bitmaps.
class CFX_DIBBase : public Retainable {
 public:
  ~CFX_DIBBase() override;

  virtual uint8_t* GetBuffer() const;
  virtual const uint8_t* GetScanline(int line) const = 0;
  virtual bool SkipToScanline(int line, PauseIndicatorIface* pPause) const;
  virtual void DownSampleScanline(int line,
                                  uint8_t* dest_scan,
                                  int dest_bpp,
                                  int dest_width,
                                  bool bFlipX,
                                  int clip_left,
                                  int clip_width) const = 0;

  uint8_t* GetWritableScanline(int line) {
    return const_cast<uint8_t*>(GetScanline(line));
  }
  int GetWidth() const { return m_Width; }
  int GetHeight() const { return m_Height; }

  FXDIB_Format GetFormat() const {
    return static_cast<FXDIB_Format>(m_AlphaFlag * 0x100 + m_bpp);
  }
  uint32_t GetPitch() const { return m_Pitch; }
  uint32_t* GetPalette() const { return m_pPalette.get(); }
  int GetBPP() const { return m_bpp; }

  // TODO(thestig): Investigate this. Given the possible values of FXDIB_Format,
  // it feels as though this should be implemented as !!(m_AlphaFlag & 1) and
  // IsOpaqueImage() below should never be able to return true.
  bool IsAlphaMask() const { return m_AlphaFlag == 1; }
  bool HasAlpha() const { return !!(m_AlphaFlag & 2); }
  bool IsOpaqueImage() const { return !(m_AlphaFlag & 3); }
  bool IsCmykImage() const { return !!(m_AlphaFlag & 4); }

  int GetPaletteSize() const {
    return IsAlphaMask() ? 0 : (m_bpp == 1 ? 2 : (m_bpp == 8 ? 256 : 0));
  }

  uint32_t GetPaletteArgb(int index) const;
  void SetPaletteArgb(int index, uint32_t color);

  // Copies into internally-owned palette.
  void SetPalette(const uint32_t* pSrcPal);

  RetainPtr<CFX_DIBitmap> Clone(const FX_RECT* pClip) const;
  RetainPtr<CFX_DIBitmap> CloneConvert(FXDIB_Format format);
  RetainPtr<CFX_DIBitmap> StretchTo(int dest_width,
                                    int dest_height,
                                    const FXDIB_ResampleOptions& options,
                                    const FX_RECT* pClip);
  RetainPtr<CFX_DIBitmap> TransformTo(const CFX_Matrix& mtDest,
                                      int* left,
                                      int* top);
  RetainPtr<CFX_DIBitmap> SwapXY(bool bXFlip, bool bYFlip) const;
  RetainPtr<CFX_DIBitmap> FlipImage(bool bXFlip, bool bYFlip) const;

  RetainPtr<CFX_DIBitmap> CloneAlphaMask() const;

  // Copies into internally-owned mask.
  bool SetAlphaMask(const RetainPtr<CFX_DIBBase>& pAlphaMask,
                    const FX_RECT* pClip);

  bool GetOverlapRect(int& dest_left,
                      int& dest_top,
                      int& width,
                      int& height,
                      int src_width,
                      int src_height,
                      int& src_left,
                      int& src_top,
                      const CFX_ClipRgn* pClipRgn);

#if defined _SKIA_SUPPORT_ || defined _SKIA_SUPPORT_PATHS_
  void DebugVerifyBitmapIsPreMultiplied(void* buffer) const;
#endif

  RetainPtr<CFX_DIBitmap> m_pAlphaMask;

 protected:
  CFX_DIBBase();

  static bool ConvertBuffer(FXDIB_Format dest_format,
                            uint8_t* dest_buf,
                            int dest_pitch,
                            int width,
                            int height,
                            const RetainPtr<CFX_DIBBase>& pSrcBitmap,
                            int src_left,
                            int src_top,
                            std::unique_ptr<uint32_t, FxFreeDeleter>* pal);

  void BuildPalette();
  bool BuildAlphaMask();
  int FindPalette(uint32_t color) const;
  void GetPalette(uint32_t* pal, int alpha) const;

  int m_Width;
  int m_Height;
  int m_bpp;
  uint32_t m_AlphaFlag;
  uint32_t m_Pitch;
  // TODO(weili): Use std::vector for this.
  std::unique_ptr<uint32_t, FxFreeDeleter> m_pPalette;
};

#endif  // CORE_FXGE_DIB_CFX_DIBBASE_H_
