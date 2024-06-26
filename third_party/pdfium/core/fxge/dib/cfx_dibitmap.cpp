// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxge/dib/cfx_dibitmap.h"

#include <limits>
#include <memory>
#include <utility>

#include "core/fxcodec/fx_codec.h"
#include "core/fxge/cfx_cliprgn.h"
#include "core/fxge/dib/cfx_scanlinecompositor.h"

namespace {

constexpr size_t kMaxOOMLimit = 12000000;
constexpr int8_t kChannelOffset[] = {0, 2, 1, 0, 0, 1, 2, 3, 3};

}  // namespace

CFX_DIBitmap::CFX_DIBitmap() {
  m_pPalette = nullptr;
#ifdef _SKIA_SUPPORT_PATHS_
  m_nFormat = Format::kCleared;
#endif
}

bool CFX_DIBitmap::Create(int width, int height, FXDIB_Format format) {
  return Create(width, height, format, nullptr, 0);
}

bool CFX_DIBitmap::Create(int width,
                          int height,
                          FXDIB_Format format,
                          uint8_t* pBuffer,
                          uint32_t pitch) {
  m_pBuffer = nullptr;
  m_bpp = static_cast<uint8_t>(format);
  m_AlphaFlag = static_cast<uint8_t>(format >> 8);
  m_Width = 0;
  m_Height = 0;
  m_Pitch = 0;

  uint32_t calculatedSize;
  if (!CalculatePitchAndSize(height, width, format, &pitch, &calculatedSize))
    return false;

  if (pBuffer) {
    m_pBuffer.Reset(pBuffer);
  } else {
    size_t bufferSize = calculatedSize + 4;
    if (bufferSize >= kMaxOOMLimit) {
      m_pBuffer = std::unique_ptr<uint8_t, FxFreeDeleter>(
          FX_TryAlloc(uint8_t, bufferSize));
      if (!m_pBuffer)
        return false;
    } else {
      m_pBuffer = std::unique_ptr<uint8_t, FxFreeDeleter>(
          FX_Alloc(uint8_t, bufferSize));
    }
  }
  m_Width = width;
  m_Height = height;
  m_Pitch = pitch;
  if (!HasAlpha() || format == FXDIB_Argb)
    return true;

  if (BuildAlphaMask())
    return true;

  if (pBuffer)
    return true;

  m_pBuffer = nullptr;
  m_Width = 0;
  m_Height = 0;
  m_Pitch = 0;
  return false;
}

bool CFX_DIBitmap::Copy(const RetainPtr<CFX_DIBBase>& pSrc) {
  if (m_pBuffer)
    return false;

  if (!Create(pSrc->GetWidth(), pSrc->GetHeight(), pSrc->GetFormat()))
    return false;

  SetPalette(pSrc->GetPalette());
  SetAlphaMask(pSrc->m_pAlphaMask, nullptr);
  for (int row = 0; row < pSrc->GetHeight(); row++)
    memcpy(m_pBuffer.Get() + row * m_Pitch, pSrc->GetScanline(row), m_Pitch);
  return true;
}

CFX_DIBitmap::~CFX_DIBitmap() {}

uint8_t* CFX_DIBitmap::GetBuffer() const {
  return m_pBuffer.Get();
}

const uint8_t* CFX_DIBitmap::GetScanline(int line) const {
  return m_pBuffer.Get() ? m_pBuffer.Get() + line * m_Pitch : nullptr;
}

void CFX_DIBitmap::TakeOver(RetainPtr<CFX_DIBitmap>&& pSrcBitmap) {
  m_pBuffer = std::move(pSrcBitmap->m_pBuffer);
  m_pPalette = std::move(pSrcBitmap->m_pPalette);
  m_pAlphaMask = pSrcBitmap->m_pAlphaMask;
  pSrcBitmap->m_pBuffer = nullptr;
  pSrcBitmap->m_pAlphaMask = nullptr;
  m_bpp = pSrcBitmap->m_bpp;
  m_AlphaFlag = pSrcBitmap->m_AlphaFlag;
  m_Width = pSrcBitmap->m_Width;
  m_Height = pSrcBitmap->m_Height;
  m_Pitch = pSrcBitmap->m_Pitch;
}

void CFX_DIBitmap::Clear(uint32_t color) {
  if (!m_pBuffer)
    return;

  uint8_t* pBuffer = m_pBuffer.Get();
  switch (GetFormat()) {
    case FXDIB_1bppMask:
      memset(pBuffer, (color & 0xff000000) ? 0xff : 0, m_Pitch * m_Height);
      break;
    case FXDIB_1bppRgb: {
      int index = FindPalette(color);
      memset(pBuffer, index ? 0xff : 0, m_Pitch * m_Height);
      break;
    }
    case FXDIB_8bppMask:
      memset(pBuffer, color >> 24, m_Pitch * m_Height);
      break;
    case FXDIB_8bppRgb: {
      int index = FindPalette(color);
      memset(pBuffer, index, m_Pitch * m_Height);
      break;
    }
    case FXDIB_Rgb:
    case FXDIB_Rgba: {
      int a;
      int r;
      int g;
      int b;
      std::tie(a, r, g, b) = ArgbDecode(color);
      if (r == g && g == b) {
        memset(pBuffer, r, m_Pitch * m_Height);
      } else {
        int byte_pos = 0;
        for (int col = 0; col < m_Width; col++) {
          pBuffer[byte_pos++] = b;
          pBuffer[byte_pos++] = g;
          pBuffer[byte_pos++] = r;
        }
        for (int row = 1; row < m_Height; row++) {
          memcpy(pBuffer + row * m_Pitch, pBuffer, m_Pitch);
        }
      }
      break;
    }
    case FXDIB_Rgb32:
    case FXDIB_Argb: {
      color = IsCmykImage() ? FXCMYK_TODIB(color) : FXARGB_TODIB(color);
#ifdef _SKIA_SUPPORT_
      if (FXDIB_Rgb32 == GetFormat() && !IsCmykImage())
        color |= 0xFF000000;
#endif
      for (int i = 0; i < m_Width; i++)
        reinterpret_cast<uint32_t*>(pBuffer)[i] = color;
      for (int row = 1; row < m_Height; row++)
        memcpy(pBuffer + row * m_Pitch, pBuffer, m_Pitch);
      break;
    }
    default:
      break;
  }
}

bool CFX_DIBitmap::TransferBitmap(int dest_left,
                                  int dest_top,
                                  int width,
                                  int height,
                                  const RetainPtr<CFX_DIBBase>& pSrcBitmap,
                                  int src_left,
                                  int src_top) {
  if (!m_pBuffer)
    return false;

  if (!GetOverlapRect(dest_left, dest_top, width, height,
                      pSrcBitmap->GetWidth(), pSrcBitmap->GetHeight(), src_left,
                      src_top, nullptr)) {
    return true;
  }

  FXDIB_Format dest_format = GetFormat();
  FXDIB_Format src_format = pSrcBitmap->GetFormat();
  if (dest_format != src_format) {
    return TransferWithUnequalFormats(dest_format, dest_left, dest_top, width,
                                      height, pSrcBitmap, src_left, src_top);
  }

  if (GetBPP() != 1) {
    TransferWithMultipleBPP(dest_left, dest_top, width, height, pSrcBitmap,
                            src_left, src_top);
    return true;
  }

  TransferEqualFormatsOneBPP(dest_left, dest_top, width, height, pSrcBitmap,
                             src_left, src_top);
  return true;
}

bool CFX_DIBitmap::TransferWithUnequalFormats(
    FXDIB_Format dest_format,
    int dest_left,
    int dest_top,
    int width,
    int height,
    const RetainPtr<CFX_DIBBase>& pSrcBitmap,
    int src_left,
    int src_top) {
  if (m_pPalette)
    return false;

  if (m_bpp == 8)
    dest_format = FXDIB_8bppMask;

  uint8_t* dest_buf =
      m_pBuffer.Get() + dest_top * m_Pitch + dest_left * GetBPP() / 8;
  std::unique_ptr<uint32_t, FxFreeDeleter> d_plt;
  if (!ConvertBuffer(dest_format, dest_buf, m_Pitch, width, height, pSrcBitmap,
                     src_left, src_top, &d_plt)) {
    return false;
  }
  return true;
}

void CFX_DIBitmap::TransferWithMultipleBPP(
    int dest_left,
    int dest_top,
    int width,
    int height,
    const RetainPtr<CFX_DIBBase>& pSrcBitmap,
    int src_left,
    int src_top) {
  int Bpp = GetBPP() / 8;
  for (int row = 0; row < height; ++row) {
    uint8_t* dest_scan =
        m_pBuffer.Get() + (dest_top + row) * m_Pitch + dest_left * Bpp;
    const uint8_t* src_scan =
        pSrcBitmap->GetScanline(src_top + row) + src_left * Bpp;
    memcpy(dest_scan, src_scan, width * Bpp);
  }
}

void CFX_DIBitmap::TransferEqualFormatsOneBPP(
    int dest_left,
    int dest_top,
    int width,
    int height,
    const RetainPtr<CFX_DIBBase>& pSrcBitmap,
    int src_left,
    int src_top) {
  for (int row = 0; row < height; ++row) {
    uint8_t* dest_scan = m_pBuffer.Get() + (dest_top + row) * m_Pitch;
    const uint8_t* src_scan = pSrcBitmap->GetScanline(src_top + row);
    for (int col = 0; col < width; ++col) {
      int src_idx = src_left + col;
      int dest_idx = dest_left + col;
      if (src_scan[(src_idx) / 8] & (1 << (7 - (src_idx) % 8)))
        dest_scan[(dest_idx) / 8] |= 1 << (7 - (dest_idx) % 8);
      else
        dest_scan[(dest_idx) / 8] &= ~(1 << (7 - (dest_idx) % 8));
    }
  }
}

bool CFX_DIBitmap::LoadChannelFromAlpha(
    FXDIB_Channel destChannel,
    const RetainPtr<CFX_DIBBase>& pSrcBitmap) {
  if (!m_pBuffer)
    return false;

  RetainPtr<CFX_DIBBase> pSrcClone = pSrcBitmap;
  if (!pSrcBitmap->HasAlpha() && !pSrcBitmap->IsAlphaMask())
    return false;

  if (pSrcBitmap->GetBPP() == 1) {
    pSrcClone = pSrcBitmap->CloneConvert(FXDIB_8bppMask);
    if (!pSrcClone)
      return false;
  }
  int srcOffset = pSrcBitmap->GetFormat() == FXDIB_Argb ? 3 : 0;
  int destOffset = 0;
  if (destChannel == FXDIB_Alpha) {
    if (IsAlphaMask()) {
      if (!ConvertFormat(FXDIB_8bppMask))
        return false;
    } else {
      if (!ConvertFormat(IsCmykImage() ? FXDIB_Cmyka : FXDIB_Argb))
        return false;

      if (GetFormat() == FXDIB_Argb)
        destOffset = 3;
    }
  } else {
    if (IsAlphaMask())
      return false;

    if (GetBPP() < 24) {
      if (HasAlpha()) {
        if (!ConvertFormat(IsCmykImage() ? FXDIB_Cmyka : FXDIB_Argb))
          return false;
#if _FX_PLATFORM_ == _FX_PLATFORM_APPLE_
      } else if (!ConvertFormat(IsCmykImage() ? FXDIB_Cmyk : FXDIB_Rgb32)) {
#else
      } else if (!ConvertFormat(IsCmykImage() ? FXDIB_Cmyk : FXDIB_Rgb)) {
#endif
        return false;
      }
    }
    destOffset = kChannelOffset[destChannel];
  }
  if (pSrcClone->m_pAlphaMask) {
    RetainPtr<CFX_DIBBase> pAlphaMask = pSrcClone->m_pAlphaMask;
    if (pSrcClone->GetWidth() != m_Width ||
        pSrcClone->GetHeight() != m_Height) {
      if (pAlphaMask) {
        pAlphaMask = pAlphaMask->StretchTo(m_Width, m_Height,
                                           FXDIB_ResampleOptions(), nullptr);
        if (!pAlphaMask)
          return false;
      }
    }
    pSrcClone = std::move(pAlphaMask);
    srcOffset = 0;
  } else if (pSrcClone->GetWidth() != m_Width ||
             pSrcClone->GetHeight() != m_Height) {
    RetainPtr<CFX_DIBitmap> pSrcMatched = pSrcClone->StretchTo(
        m_Width, m_Height, FXDIB_ResampleOptions(), nullptr);
    if (!pSrcMatched)
      return false;

    pSrcClone = std::move(pSrcMatched);
  }
  RetainPtr<CFX_DIBitmap> pDst(this);
  if (destChannel == FXDIB_Alpha && m_pAlphaMask) {
    pDst = m_pAlphaMask;
    destOffset = 0;
  }
  int srcBytes = pSrcClone->GetBPP() / 8;
  int destBytes = pDst->GetBPP() / 8;
  for (int row = 0; row < m_Height; row++) {
    uint8_t* dest_pos = pDst->GetWritableScanline(row) + destOffset;
    const uint8_t* src_pos = pSrcClone->GetScanline(row) + srcOffset;
    for (int col = 0; col < m_Width; col++) {
      *dest_pos = *src_pos;
      dest_pos += destBytes;
      src_pos += srcBytes;
    }
  }
  return true;
}

bool CFX_DIBitmap::LoadChannel(FXDIB_Channel destChannel, int value) {
  if (!m_pBuffer)
    return false;

  int destOffset;
  if (destChannel == FXDIB_Alpha) {
    if (IsAlphaMask()) {
      if (!ConvertFormat(FXDIB_8bppMask)) {
        return false;
      }
      destOffset = 0;
    } else {
      destOffset = 0;
      if (!ConvertFormat(IsCmykImage() ? FXDIB_Cmyka : FXDIB_Argb)) {
        return false;
      }
      if (GetFormat() == FXDIB_Argb) {
        destOffset = 3;
      }
    }
  } else {
    if (IsAlphaMask()) {
      return false;
    }
    if (GetBPP() < 24) {
      if (HasAlpha()) {
        if (!ConvertFormat(IsCmykImage() ? FXDIB_Cmyka : FXDIB_Argb)) {
          return false;
        }
#if _FX_PLATFORM_ == _FX_PLATFORM_APPLE_
      } else if (!ConvertFormat(IsCmykImage() ? FXDIB_Cmyk : FXDIB_Rgb)) {
#else
      } else if (!ConvertFormat(IsCmykImage() ? FXDIB_Cmyk : FXDIB_Rgb32)) {
#endif
        return false;
      }
    }
    destOffset = kChannelOffset[destChannel];
  }
  int Bpp = GetBPP() / 8;
  if (Bpp == 1) {
    memset(m_pBuffer.Get(), value, m_Height * m_Pitch);
    return true;
  }
  if (destChannel == FXDIB_Alpha && m_pAlphaMask) {
    memset(m_pAlphaMask->GetBuffer(), value,
           m_pAlphaMask->GetHeight() * m_pAlphaMask->GetPitch());
    return true;
  }
  for (int row = 0; row < m_Height; row++) {
    uint8_t* scan_line = m_pBuffer.Get() + row * m_Pitch + destOffset;
    for (int col = 0; col < m_Width; col++) {
      *scan_line = value;
      scan_line += Bpp;
    }
  }
  return true;
}

bool CFX_DIBitmap::MultiplyAlpha(const RetainPtr<CFX_DIBBase>& pSrcBitmap) {
  if (!m_pBuffer)
    return false;

  ASSERT(pSrcBitmap->IsAlphaMask());
  if (!pSrcBitmap->IsAlphaMask())
    return false;

  if (!IsAlphaMask() && !HasAlpha())
    return LoadChannelFromAlpha(FXDIB_Alpha, pSrcBitmap);

  RetainPtr<CFX_DIBitmap> pSrcClone = pSrcBitmap.As<CFX_DIBitmap>();
  if (pSrcBitmap->GetWidth() != m_Width ||
      pSrcBitmap->GetHeight() != m_Height) {
    pSrcClone = pSrcBitmap->StretchTo(m_Width, m_Height,
                                      FXDIB_ResampleOptions(), nullptr);
    if (!pSrcClone)
      return false;
  }
  if (IsAlphaMask()) {
    if (!ConvertFormat(FXDIB_8bppMask))
      return false;

    for (int row = 0; row < m_Height; row++) {
      uint8_t* dest_scan = m_pBuffer.Get() + m_Pitch * row;
      uint8_t* src_scan = pSrcClone->m_pBuffer.Get() + pSrcClone->m_Pitch * row;
      if (pSrcClone->GetBPP() == 1) {
        for (int col = 0; col < m_Width; col++) {
          if (!((1 << (7 - col % 8)) & src_scan[col / 8]))
            dest_scan[col] = 0;
        }
      } else {
        for (int col = 0; col < m_Width; col++) {
          *dest_scan = (*dest_scan) * src_scan[col] / 255;
          dest_scan++;
        }
      }
    }
  } else {
    if (GetFormat() == FXDIB_Argb) {
      if (pSrcClone->GetBPP() == 1)
        return false;

      for (int row = 0; row < m_Height; row++) {
        uint8_t* dest_scan = m_pBuffer.Get() + m_Pitch * row + 3;
        uint8_t* src_scan =
            pSrcClone->m_pBuffer.Get() + pSrcClone->m_Pitch * row;
        for (int col = 0; col < m_Width; col++) {
          *dest_scan = (*dest_scan) * src_scan[col] / 255;
          dest_scan += 4;
        }
      }
    } else {
      m_pAlphaMask->MultiplyAlpha(pSrcClone);
    }
  }
  return true;
}

bool CFX_DIBitmap::MultiplyAlpha(int alpha) {
  if (!m_pBuffer)
    return false;

  switch (GetFormat()) {
    case FXDIB_1bppMask:
      if (!ConvertFormat(FXDIB_8bppMask)) {
        return false;
      }
      MultiplyAlpha(alpha);
      break;
    case FXDIB_8bppMask: {
      for (int row = 0; row < m_Height; row++) {
        uint8_t* scan_line = m_pBuffer.Get() + row * m_Pitch;
        for (int col = 0; col < m_Width; col++) {
          scan_line[col] = scan_line[col] * alpha / 255;
        }
      }
      break;
    }
    case FXDIB_Argb: {
      for (int row = 0; row < m_Height; row++) {
        uint8_t* scan_line = m_pBuffer.Get() + row * m_Pitch + 3;
        for (int col = 0; col < m_Width; col++) {
          *scan_line = (*scan_line) * alpha / 255;
          scan_line += 4;
        }
      }
      break;
    }
    default:
      if (HasAlpha()) {
        m_pAlphaMask->MultiplyAlpha(alpha);
      } else if (IsCmykImage()) {
        if (!ConvertFormat((FXDIB_Format)(GetFormat() | 0x0200))) {
          return false;
        }
        m_pAlphaMask->MultiplyAlpha(alpha);
      } else {
        if (!ConvertFormat(FXDIB_Argb)) {
          return false;
        }
        MultiplyAlpha(alpha);
      }
      break;
  }
  return true;
}

uint32_t CFX_DIBitmap::GetPixel(int x, int y) const {
  if (!m_pBuffer)
    return 0;

  uint8_t* pos = m_pBuffer.Get() + y * m_Pitch + x * GetBPP() / 8;
  switch (GetFormat()) {
    case FXDIB_1bppMask: {
      if ((*pos) & (1 << (7 - x % 8))) {
        return 0xff000000;
      }
      return 0;
    }
    case FXDIB_1bppRgb: {
      if ((*pos) & (1 << (7 - x % 8))) {
        return m_pPalette ? m_pPalette.get()[1] : 0xffffffff;
      }
      return m_pPalette ? m_pPalette.get()[0] : 0xff000000;
    }
    case FXDIB_8bppMask:
      return (*pos) << 24;
    case FXDIB_8bppRgb:
      return m_pPalette ? m_pPalette.get()[*pos]
                        : (0xff000000 | ((*pos) * 0x10101));
    case FXDIB_Rgb:
    case FXDIB_Rgba:
    case FXDIB_Rgb32:
      return FXARGB_GETDIB(pos) | 0xff000000;
    case FXDIB_Argb:
      return FXARGB_GETDIB(pos);
    default:
      break;
  }
  return 0;
}

void CFX_DIBitmap::SetPixel(int x, int y, uint32_t color) {
  if (!m_pBuffer)
    return;

  if (x < 0 || x >= m_Width || y < 0 || y >= m_Height)
    return;

  uint8_t* pos = m_pBuffer.Get() + y * m_Pitch + x * GetBPP() / 8;
  switch (GetFormat()) {
    case FXDIB_1bppMask:
      if (color >> 24) {
        *pos |= 1 << (7 - x % 8);
      } else {
        *pos &= ~(1 << (7 - x % 8));
      }
      break;
    case FXDIB_1bppRgb:
      if (m_pPalette) {
        if (color == m_pPalette.get()[1]) {
          *pos |= 1 << (7 - x % 8);
        } else {
          *pos &= ~(1 << (7 - x % 8));
        }
      } else {
        if (color == 0xffffffff) {
          *pos |= 1 << (7 - x % 8);
        } else {
          *pos &= ~(1 << (7 - x % 8));
        }
      }
      break;
    case FXDIB_8bppMask:
      *pos = (uint8_t)(color >> 24);
      break;
    case FXDIB_8bppRgb: {
      if (m_pPalette) {
        for (int i = 0; i < 256; i++) {
          if (m_pPalette.get()[i] == color) {
            *pos = (uint8_t)i;
            return;
          }
        }
        *pos = 0;
      } else {
        *pos = FXRGB2GRAY(FXARGB_R(color), FXARGB_G(color), FXARGB_B(color));
      }
      break;
    }
    case FXDIB_Rgb:
    case FXDIB_Rgb32: {
      int alpha = FXARGB_A(color);
      pos[0] = (FXARGB_B(color) * alpha + pos[0] * (255 - alpha)) / 255;
      pos[1] = (FXARGB_G(color) * alpha + pos[1] * (255 - alpha)) / 255;
      pos[2] = (FXARGB_R(color) * alpha + pos[2] * (255 - alpha)) / 255;
      break;
    }
    case FXDIB_Rgba: {
      pos[0] = FXARGB_B(color);
      pos[1] = FXARGB_G(color);
      pos[2] = FXARGB_R(color);
      break;
    }
    case FXDIB_Argb:
      FXARGB_SETDIB(pos, color);
      break;
    default:
      break;
  }
}

void CFX_DIBitmap::DownSampleScanline(int line,
                                      uint8_t* dest_scan,
                                      int dest_bpp,
                                      int dest_width,
                                      bool bFlipX,
                                      int clip_left,
                                      int clip_width) const {
  if (!m_pBuffer)
    return;

  int src_Bpp = m_bpp / 8;
  uint8_t* scanline = m_pBuffer.Get() + line * m_Pitch;
  if (src_Bpp == 0) {
    for (int i = 0; i < clip_width; i++) {
      uint32_t dest_x = clip_left + i;
      uint32_t src_x = dest_x * m_Width / dest_width;
      if (bFlipX) {
        src_x = m_Width - src_x - 1;
      }
      src_x %= m_Width;
      dest_scan[i] = (scanline[src_x / 8] & (1 << (7 - src_x % 8))) ? 255 : 0;
    }
  } else if (src_Bpp == 1) {
    for (int i = 0; i < clip_width; i++) {
      uint32_t dest_x = clip_left + i;
      uint32_t src_x = dest_x * m_Width / dest_width;
      if (bFlipX) {
        src_x = m_Width - src_x - 1;
      }
      src_x %= m_Width;
      int dest_pos = i;
      if (m_pPalette) {
        if (!IsCmykImage()) {
          dest_pos *= 3;
          FX_ARGB argb = m_pPalette.get()[scanline[src_x]];
          dest_scan[dest_pos] = FXARGB_B(argb);
          dest_scan[dest_pos + 1] = FXARGB_G(argb);
          dest_scan[dest_pos + 2] = FXARGB_R(argb);
        } else {
          dest_pos *= 4;
          FX_CMYK cmyk = m_pPalette.get()[scanline[src_x]];
          dest_scan[dest_pos] = FXSYS_GetCValue(cmyk);
          dest_scan[dest_pos + 1] = FXSYS_GetMValue(cmyk);
          dest_scan[dest_pos + 2] = FXSYS_GetYValue(cmyk);
          dest_scan[dest_pos + 3] = FXSYS_GetKValue(cmyk);
        }
      } else {
        dest_scan[dest_pos] = scanline[src_x];
      }
    }
  } else {
    for (int i = 0; i < clip_width; i++) {
      uint32_t dest_x = clip_left + i;
      uint32_t src_x =
          bFlipX ? (m_Width - dest_x * m_Width / dest_width - 1) * src_Bpp
                 : (dest_x * m_Width / dest_width) * src_Bpp;
      src_x %= m_Width * src_Bpp;
      int dest_pos = i * src_Bpp;
      for (int b = 0; b < src_Bpp; b++) {
        dest_scan[dest_pos + b] = scanline[src_x + b];
      }
    }
  }
}

void CFX_DIBitmap::ConvertBGRColorScale(uint32_t forecolor,
                                        uint32_t backcolor) {
  int fr = FXSYS_GetRValue(forecolor);
  int fg = FXSYS_GetGValue(forecolor);
  int fb = FXSYS_GetBValue(forecolor);
  int br = FXSYS_GetRValue(backcolor);
  int bg = FXSYS_GetGValue(backcolor);
  int bb = FXSYS_GetBValue(backcolor);
  if (m_bpp <= 8) {
    if (forecolor == 0 && backcolor == 0xffffff && !m_pPalette)
      return;
    if (!m_pPalette)
      BuildPalette();
    int size = 1 << m_bpp;
    for (int i = 0; i < size; ++i) {
      int gray = FXRGB2GRAY(FXARGB_R(m_pPalette.get()[i]),
                            FXARGB_G(m_pPalette.get()[i]),
                            FXARGB_B(m_pPalette.get()[i]));
      m_pPalette.get()[i] =
          ArgbEncode(0xff, br + (fr - br) * gray / 255,
                     bg + (fg - bg) * gray / 255, bb + (fb - bb) * gray / 255);
    }
    return;
  }
  if (forecolor == 0 && backcolor == 0xffffff) {
    for (int row = 0; row < m_Height; ++row) {
      uint8_t* scanline = m_pBuffer.Get() + row * m_Pitch;
      int gap = m_bpp / 8 - 2;
      for (int col = 0; col < m_Width; ++col) {
        int gray = FXRGB2GRAY(scanline[2], scanline[1], scanline[0]);
        *scanline++ = gray;
        *scanline++ = gray;
        *scanline = gray;
        scanline += gap;
      }
    }
    return;
  }
  for (int row = 0; row < m_Height; ++row) {
    uint8_t* scanline = m_pBuffer.Get() + row * m_Pitch;
    int gap = m_bpp / 8 - 2;
    for (int col = 0; col < m_Width; ++col) {
      int gray = FXRGB2GRAY(scanline[2], scanline[1], scanline[0]);
      *scanline++ = bb + (fb - bb) * gray / 255;
      *scanline++ = bg + (fg - bg) * gray / 255;
      *scanline = br + (fr - br) * gray / 255;
      scanline += gap;
    }
  }
}

void CFX_DIBitmap::ConvertCMYKColorScale(uint32_t forecolor,
                                         uint32_t backcolor) {
  int fc = FXSYS_GetCValue(forecolor);
  int fm = FXSYS_GetMValue(forecolor);
  int fy = FXSYS_GetYValue(forecolor);
  int fk = FXSYS_GetKValue(forecolor);
  int bc = FXSYS_GetCValue(backcolor);
  int bm = FXSYS_GetMValue(backcolor);
  int by = FXSYS_GetYValue(backcolor);
  int bk = FXSYS_GetKValue(backcolor);
  if (m_bpp <= 8) {
    if (forecolor == 0xff && backcolor == 0 && !m_pPalette)
      return;
    if (!m_pPalette)
      BuildPalette();
    int size = 1 << m_bpp;
    for (int i = 0; i < size; ++i) {
      uint8_t r;
      uint8_t g;
      uint8_t b;
      std::tie(r, g, b) =
          AdobeCMYK_to_sRGB1(FXSYS_GetCValue(m_pPalette.get()[i]),
                             FXSYS_GetMValue(m_pPalette.get()[i]),
                             FXSYS_GetYValue(m_pPalette.get()[i]),
                             FXSYS_GetKValue(m_pPalette.get()[i]));
      int gray = 255 - FXRGB2GRAY(r, g, b);
      m_pPalette.get()[i] =
          CmykEncode(bc + (fc - bc) * gray / 255, bm + (fm - bm) * gray / 255,
                     by + (fy - by) * gray / 255, bk + (fk - bk) * gray / 255);
    }
    return;
  }
  if (forecolor == 0xff && backcolor == 0x00) {
    for (int row = 0; row < m_Height; ++row) {
      uint8_t* scanline = m_pBuffer.Get() + row * m_Pitch;
      for (int col = 0; col < m_Width; ++col) {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        std::tie(r, g, b) = AdobeCMYK_to_sRGB1(scanline[0], scanline[1],
                                               scanline[2], scanline[3]);
        *scanline++ = 0;
        *scanline++ = 0;
        *scanline++ = 0;
        *scanline++ = 255 - FXRGB2GRAY(r, g, b);
      }
    }
    return;
  }
  for (int row = 0; row < m_Height; ++row) {
    uint8_t* scanline = m_pBuffer.Get() + row * m_Pitch;
    for (int col = 0; col < m_Width; ++col) {
      uint8_t r;
      uint8_t g;
      uint8_t b;
      std::tie(r, g, b) = AdobeCMYK_to_sRGB1(scanline[0], scanline[1],
                                             scanline[2], scanline[3]);
      int gray = 255 - FXRGB2GRAY(r, g, b);
      *scanline++ = bc + (fc - bc) * gray / 255;
      *scanline++ = bm + (fm - bm) * gray / 255;
      *scanline++ = by + (fy - by) * gray / 255;
      *scanline++ = bk + (fk - bk) * gray / 255;
    }
  }
}

bool CFX_DIBitmap::ConvertColorScale(uint32_t forecolor, uint32_t backcolor) {
  ASSERT(!IsAlphaMask());
  if (!m_pBuffer || IsAlphaMask())
    return false;

  if (IsCmykImage())
    ConvertCMYKColorScale(forecolor, backcolor);
  else
    ConvertBGRColorScale(forecolor, backcolor);
  return true;
}

// static
bool CFX_DIBitmap::CalculatePitchAndSize(int height,
                                         int width,
                                         FXDIB_Format format,
                                         uint32_t* pitch,
                                         uint32_t* size) {
  if (width <= 0 || height <= 0)
    return false;

  int bpp = GetBppFromFormat(format);
  if (!bpp)
    return false;

  if ((INT_MAX - 31) / width < bpp)
    return false;

  if (!*pitch)
    *pitch = static_cast<uint32_t>((width * bpp + 31) / 32 * 4);

  if ((1 << 30) / *pitch < static_cast<uint32_t>(height))
    return false;

  *size = *pitch * static_cast<uint32_t>(height);
  return true;
}

bool CFX_DIBitmap::CompositeBitmap(int dest_left,
                                   int dest_top,
                                   int width,
                                   int height,
                                   const RetainPtr<CFX_DIBBase>& pSrcBitmap,
                                   int src_left,
                                   int src_top,
                                   BlendMode blend_type,
                                   const CFX_ClipRgn* pClipRgn,
                                   bool bRgbByteOrder) {
  if (!m_pBuffer)
    return false;

  ASSERT(!pSrcBitmap->IsAlphaMask());
  ASSERT(m_bpp >= 8);
  if (pSrcBitmap->IsAlphaMask() || m_bpp < 8) {
    return false;
  }
  if (!GetOverlapRect(dest_left, dest_top, width, height,
                      pSrcBitmap->GetWidth(), pSrcBitmap->GetHeight(), src_left,
                      src_top, pClipRgn)) {
    return true;
  }

  RetainPtr<CFX_DIBitmap> pClipMask;
  FX_RECT clip_box;
  if (pClipRgn && pClipRgn->GetType() != CFX_ClipRgn::RectI) {
    ASSERT(pClipRgn->GetType() == CFX_ClipRgn::MaskF);
    pClipMask = pClipRgn->GetMask();
    clip_box = pClipRgn->GetBox();
  }
  CFX_ScanlineCompositor compositor;
  if (!compositor.Init(GetFormat(), pSrcBitmap->GetFormat(), width,
                       pSrcBitmap->GetPalette(), 0, blend_type,
                       pClipMask != nullptr, bRgbByteOrder, 0)) {
    return false;
  }
  int dest_Bpp = m_bpp / 8;
  int src_Bpp = pSrcBitmap->GetBPP() / 8;
  bool bRgb = src_Bpp > 1 && !pSrcBitmap->IsCmykImage();
  RetainPtr<CFX_DIBitmap> pSrcAlphaMask = pSrcBitmap->m_pAlphaMask;
  for (int row = 0; row < height; row++) {
    uint8_t* dest_scan =
        m_pBuffer.Get() + (dest_top + row) * m_Pitch + dest_left * dest_Bpp;
    const uint8_t* src_scan =
        pSrcBitmap->GetScanline(src_top + row) + src_left * src_Bpp;
    const uint8_t* src_scan_extra_alpha =
        pSrcAlphaMask ? pSrcAlphaMask->GetScanline(src_top + row) + src_left
                      : nullptr;
    uint8_t* dst_scan_extra_alpha =
        m_pAlphaMask
            ? m_pAlphaMask->GetWritableScanline(dest_top + row) + dest_left
            : nullptr;
    const uint8_t* clip_scan = nullptr;
    if (pClipMask) {
      clip_scan = pClipMask->m_pBuffer.Get() +
                  (dest_top + row - clip_box.top) * pClipMask->m_Pitch +
                  (dest_left - clip_box.left);
    }
    if (bRgb) {
      compositor.CompositeRgbBitmapLine(dest_scan, src_scan, width, clip_scan,
                                        src_scan_extra_alpha,
                                        dst_scan_extra_alpha);
    } else {
      compositor.CompositePalBitmapLine(dest_scan, src_scan, src_left, width,
                                        clip_scan, src_scan_extra_alpha,
                                        dst_scan_extra_alpha);
    }
  }
  return true;
}

bool CFX_DIBitmap::CompositeMask(int dest_left,
                                 int dest_top,
                                 int width,
                                 int height,
                                 const RetainPtr<CFX_DIBBase>& pMask,
                                 uint32_t color,
                                 int src_left,
                                 int src_top,
                                 BlendMode blend_type,
                                 const CFX_ClipRgn* pClipRgn,
                                 bool bRgbByteOrder,
                                 int alpha_flag) {
  if (!m_pBuffer)
    return false;

  ASSERT(pMask->IsAlphaMask());
  ASSERT(m_bpp >= 8);
  if (!pMask->IsAlphaMask() || m_bpp < 8)
    return false;

  if (!GetOverlapRect(dest_left, dest_top, width, height, pMask->GetWidth(),
                      pMask->GetHeight(), src_left, src_top, pClipRgn)) {
    return true;
  }

  int src_alpha =
      (uint8_t)(alpha_flag >> 8) ? (alpha_flag & 0xff) : FXARGB_A(color);
  if (src_alpha == 0)
    return true;

  RetainPtr<CFX_DIBitmap> pClipMask;
  FX_RECT clip_box;
  if (pClipRgn && pClipRgn->GetType() != CFX_ClipRgn::RectI) {
    ASSERT(pClipRgn->GetType() == CFX_ClipRgn::MaskF);
    pClipMask = pClipRgn->GetMask();
    clip_box = pClipRgn->GetBox();
  }
  int src_bpp = pMask->GetBPP();
  int Bpp = GetBPP() / 8;
  CFX_ScanlineCompositor compositor;
  if (!compositor.Init(GetFormat(), pMask->GetFormat(), width, nullptr, color,
                       blend_type, pClipMask != nullptr, bRgbByteOrder,
                       alpha_flag)) {
    return false;
  }
  for (int row = 0; row < height; row++) {
    uint8_t* dest_scan =
        m_pBuffer.Get() + (dest_top + row) * m_Pitch + dest_left * Bpp;
    const uint8_t* src_scan = pMask->GetScanline(src_top + row);
    uint8_t* dst_scan_extra_alpha =
        m_pAlphaMask
            ? m_pAlphaMask->GetWritableScanline(dest_top + row) + dest_left
            : nullptr;
    const uint8_t* clip_scan = nullptr;
    if (pClipMask) {
      clip_scan = pClipMask->m_pBuffer.Get() +
                  (dest_top + row - clip_box.top) * pClipMask->m_Pitch +
                  (dest_left - clip_box.left);
    }
    if (src_bpp == 1) {
      compositor.CompositeBitMaskLine(dest_scan, src_scan, src_left, width,
                                      clip_scan, dst_scan_extra_alpha);
    } else {
      compositor.CompositeByteMaskLine(dest_scan, src_scan + src_left, width,
                                       clip_scan, dst_scan_extra_alpha);
    }
  }
  return true;
}

bool CFX_DIBitmap::CompositeRect(int left,
                                 int top,
                                 int width,
                                 int height,
                                 uint32_t color,
                                 int alpha_flag) {
  if (!m_pBuffer)
    return false;

  int src_alpha = (alpha_flag >> 8) ? (alpha_flag & 0xff) : FXARGB_A(color);
  if (src_alpha == 0)
    return true;

  FX_RECT rect(left, top, left + width, top + height);
  rect.Intersect(0, 0, m_Width, m_Height);
  if (rect.IsEmpty())
    return true;

  width = rect.Width();
  uint32_t dst_color;
  if (alpha_flag >> 8)
    dst_color = FXCMYK_TODIB(color);
  else
    dst_color = FXARGB_TODIB(color);
  uint8_t* color_p = (uint8_t*)&dst_color;
  if (m_bpp == 8) {
    uint8_t gray = 255;
    if (!IsAlphaMask()) {
      if (alpha_flag >> 8) {
        uint8_t r;
        uint8_t g;
        uint8_t b;
        std::tie(r, g, b) =
            AdobeCMYK_to_sRGB1(color_p[0], color_p[1], color_p[2], color_p[3]);
        gray = FXRGB2GRAY(r, g, b);
      } else {
        gray = (uint8_t)FXRGB2GRAY((int)color_p[2], color_p[1], color_p[0]);
      }
      if (IsCmykImage()) {
        gray = ~gray;
      }
    }
    for (int row = rect.top; row < rect.bottom; row++) {
      uint8_t* dest_scan = m_pBuffer.Get() + row * m_Pitch + rect.left;
      if (src_alpha == 255) {
        memset(dest_scan, gray, width);
      } else {
        for (int col = 0; col < width; col++) {
          *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, gray, src_alpha);
          dest_scan++;
        }
      }
    }
    return true;
  }
  if (m_bpp == 1) {
    ASSERT(!IsCmykImage());
    ASSERT(static_cast<uint8_t>(alpha_flag >> 8) == 0);

    int left_shift = rect.left % 8;
    int right_shift = rect.right % 8;
    int new_width = rect.right / 8 - rect.left / 8;
    int index = 0;
    if (m_pPalette) {
      for (int i = 0; i < 2; i++) {
        if (m_pPalette.get()[i] == color)
          index = i;
      }
    } else {
      index = (static_cast<uint8_t>(color) == 0xff) ? 1 : 0;
    }
    for (int row = rect.top; row < rect.bottom; row++) {
      uint8_t* dest_scan_top = GetWritableScanline(row) + rect.left / 8;
      uint8_t* dest_scan_top_r = GetWritableScanline(row) + rect.right / 8;
      uint8_t left_flag = *dest_scan_top & (255 << (8 - left_shift));
      uint8_t right_flag = *dest_scan_top_r & (255 >> right_shift);
      if (new_width) {
        memset(dest_scan_top + 1, index ? 255 : 0, new_width - 1);
        if (!index) {
          *dest_scan_top &= left_flag;
          *dest_scan_top_r &= right_flag;
        } else {
          *dest_scan_top |= ~left_flag;
          *dest_scan_top_r |= ~right_flag;
        }
      } else {
        if (!index) {
          *dest_scan_top &= left_flag | right_flag;
        } else {
          *dest_scan_top |= ~(left_flag | right_flag);
        }
      }
    }
    return true;
  }
  ASSERT(m_bpp >= 24);
  if (m_bpp < 24 || (!(alpha_flag >> 8) && IsCmykImage()))
    return false;
  if (alpha_flag >> 8 && !IsCmykImage()) {
    std::tie(color_p[2], color_p[1], color_p[0]) =
        AdobeCMYK_to_sRGB1(FXSYS_GetCValue(color), FXSYS_GetMValue(color),
                           FXSYS_GetYValue(color), FXSYS_GetKValue(color));
  }
  if (!IsCmykImage())
    color_p[3] = static_cast<uint8_t>(src_alpha);
  int Bpp = m_bpp / 8;
  bool bAlpha = HasAlpha();
  bool bArgb = GetFormat() == FXDIB_Argb;
  if (src_alpha == 255) {
    for (int row = rect.top; row < rect.bottom; row++) {
      uint8_t* dest_scan = m_pBuffer.Get() + row * m_Pitch + rect.left * Bpp;
      uint8_t* dest_scan_alpha =
          m_pAlphaMask ? m_pAlphaMask->GetWritableScanline(row) + rect.left
                       : nullptr;
      if (dest_scan_alpha) {
        memset(dest_scan_alpha, 0xff, width);
      }
      if (Bpp == 4) {
        uint32_t* scan = (uint32_t*)dest_scan;
        for (int col = 0; col < width; col++) {
          *scan++ = dst_color;
        }
      } else {
        for (int col = 0; col < width; col++) {
          *dest_scan++ = color_p[0];
          *dest_scan++ = color_p[1];
          *dest_scan++ = color_p[2];
        }
      }
    }
    return true;
  }
  for (int row = rect.top; row < rect.bottom; row++) {
    uint8_t* dest_scan = m_pBuffer.Get() + row * m_Pitch + rect.left * Bpp;
    if (bAlpha) {
      if (bArgb) {
        for (int col = 0; col < width; col++) {
          uint8_t back_alpha = dest_scan[3];
          if (back_alpha == 0) {
            FXARGB_SETDIB(dest_scan, ArgbEncode(src_alpha, color_p[2],
                                                color_p[1], color_p[0]));
            dest_scan += 4;
            continue;
          }
          uint8_t dest_alpha =
              back_alpha + src_alpha - back_alpha * src_alpha / 255;
          int alpha_ratio = src_alpha * 255 / dest_alpha;
          *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, color_p[0], alpha_ratio);
          dest_scan++;
          *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, color_p[1], alpha_ratio);
          dest_scan++;
          *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, color_p[2], alpha_ratio);
          dest_scan++;
          *dest_scan++ = dest_alpha;
        }
      } else {
        uint8_t* dest_scan_alpha =
            m_pAlphaMask->GetWritableScanline(row) + rect.left;
        for (int col = 0; col < width; col++) {
          uint8_t back_alpha = *dest_scan_alpha;
          if (back_alpha == 0) {
            *dest_scan_alpha++ = src_alpha;
            memcpy(dest_scan, color_p, Bpp);
            dest_scan += Bpp;
            continue;
          }
          uint8_t dest_alpha =
              back_alpha + src_alpha - back_alpha * src_alpha / 255;
          *dest_scan_alpha++ = dest_alpha;
          int alpha_ratio = src_alpha * 255 / dest_alpha;
          for (int comps = 0; comps < Bpp; comps++) {
            *dest_scan =
                FXDIB_ALPHA_MERGE(*dest_scan, color_p[comps], alpha_ratio);
            dest_scan++;
          }
        }
      }
    } else {
      for (int col = 0; col < width; col++) {
        for (int comps = 0; comps < Bpp; comps++) {
          if (comps == 3) {
            *dest_scan++ = 255;
            continue;
          }
          *dest_scan = FXDIB_ALPHA_MERGE(*dest_scan, color_p[comps], src_alpha);
          dest_scan++;
        }
      }
    }
  }
  return true;
}

bool CFX_DIBitmap::ConvertFormat(FXDIB_Format dest_format) {
  FXDIB_Format src_format = GetFormat();
  if (dest_format == src_format)
    return true;

  if (dest_format == FXDIB_8bppMask && src_format == FXDIB_8bppRgb &&
      !m_pPalette) {
    m_AlphaFlag = 1;
    return true;
  }
  if (dest_format == FXDIB_Argb && src_format == FXDIB_Rgb32) {
    m_AlphaFlag = 2;
    for (int row = 0; row < m_Height; row++) {
      uint8_t* scanline = m_pBuffer.Get() + row * m_Pitch + 3;
      for (int col = 0; col < m_Width; col++) {
        *scanline = 0xff;
        scanline += 4;
      }
    }
    return true;
  }
  int dest_bpp = GetBppFromFormat(dest_format);
  int dest_pitch = (dest_bpp * m_Width + 31) / 32 * 4;
  std::unique_ptr<uint8_t, FxFreeDeleter> dest_buf(
      FX_TryAlloc(uint8_t, dest_pitch * m_Height + 4));
  if (!dest_buf)
    return false;

  RetainPtr<CFX_DIBitmap> pAlphaMask;
  if (dest_format == FXDIB_Argb) {
    memset(dest_buf.get(), 0xff, dest_pitch * m_Height + 4);
    if (m_pAlphaMask) {
      for (int row = 0; row < m_Height; row++) {
        uint8_t* pDstScanline = dest_buf.get() + row * dest_pitch + 3;
        const uint8_t* pSrcScanline = m_pAlphaMask->GetScanline(row);
        for (int col = 0; col < m_Width; col++) {
          *pDstScanline = *pSrcScanline++;
          pDstScanline += 4;
        }
      }
    }
  } else if (dest_format & 0x0200) {
    if (src_format == FXDIB_Argb) {
      pAlphaMask = CloneAlphaMask();
      if (!pAlphaMask)
        return false;
    } else {
      if (!m_pAlphaMask) {
        if (!BuildAlphaMask())
          return false;
        pAlphaMask = std::move(m_pAlphaMask);
      } else {
        pAlphaMask = m_pAlphaMask;
      }
    }
  }
  bool ret = false;
  RetainPtr<CFX_DIBBase> holder(this);
  std::unique_ptr<uint32_t, FxFreeDeleter> pal_8bpp;
  ret = ConvertBuffer(dest_format, dest_buf.get(), dest_pitch, m_Width,
                      m_Height, holder, 0, 0, &pal_8bpp);
  if (!ret)
    return false;

  m_pAlphaMask = pAlphaMask;
  m_pPalette = std::move(pal_8bpp);
  m_pBuffer = std::move(dest_buf);
  m_bpp = static_cast<uint8_t>(dest_format);
  m_AlphaFlag = static_cast<uint8_t>(dest_format >> 8);
  m_Pitch = dest_pitch;
  return true;
}
