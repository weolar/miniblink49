// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcodec/codec/ccodec_progressivedecoder.h"

#include <algorithm>
#include <memory>
#include <utility>
#include <vector>

#include "core/fxcodec/codec/cfx_codec_memory.h"
#include "core/fxcodec/fx_codec.h"
#include "core/fxcrt/fx_stream.h"
#include "core/fxge/dib/cfx_dibitmap.h"
#include "core/fxge/fx_dib.h"
#include "third_party/base/logging.h"
#include "third_party/base/numerics/safe_math.h"
#include "third_party/base/ptr_util.h"

#define FXCODEC_BLOCK_SIZE 4096

namespace {

#ifdef PDF_ENABLE_XFA_PNG
#if _FX_OS_ == _FX_OS_MACOSX_
const double kPngGamma = 1.7;
#else   // _FX_OS_ == _FX_OS_MACOSX_
const double kPngGamma = 2.2;
#endif  // _FX_OS_ == _FX_OS_MACOSX_
#endif  // PDF_ENABLE_XFA_PNG

void RGB2BGR(uint8_t* buffer, int width = 1) {
  if (buffer && width > 0) {
    uint8_t temp;
    int i = 0;
    int j = 0;
    for (; i < width; i++, j += 3) {
      temp = buffer[j];
      buffer[j] = buffer[j + 2];
      buffer[j + 2] = temp;
    }
  }
}

}  // namespace

CCodec_ProgressiveDecoder::CFXCODEC_WeightTable::CFXCODEC_WeightTable() {}

CCodec_ProgressiveDecoder::CFXCODEC_WeightTable::~CFXCODEC_WeightTable() {}

void CCodec_ProgressiveDecoder::CFXCODEC_WeightTable::Calc(int dest_len,
                                                           int dest_min,
                                                           int dest_max,
                                                           int src_len,
                                                           int src_min,
                                                           int src_max) {
  double scale, base;
  scale = (float)src_len / (float)dest_len;
  if (dest_len < 0) {
    base = (float)(src_len);
  } else {
    base = 0.0f;
  }
  m_ItemSize =
      (int)(sizeof(int) * 2 + sizeof(int) * (ceil(fabs((float)scale)) + 1));
  m_DestMin = dest_min;
  m_pWeightTables.resize((dest_max - dest_min) * m_ItemSize + 4);
  if (fabs((float)scale) < 1.0f) {
    for (int dest_pixel = dest_min; dest_pixel < dest_max; dest_pixel++) {
      PixelWeight& pixel_weights = *GetPixelWeight(dest_pixel);
      double src_pos = dest_pixel * scale + scale / 2 + base;
      pixel_weights.m_SrcStart = (int)floor((float)src_pos - 1.0f / 2);
      pixel_weights.m_SrcEnd = (int)floor((float)src_pos + 1.0f / 2);
      if (pixel_weights.m_SrcStart < src_min) {
        pixel_weights.m_SrcStart = src_min;
      }
      if (pixel_weights.m_SrcEnd >= src_max) {
        pixel_weights.m_SrcEnd = src_max - 1;
      }
      if (pixel_weights.m_SrcStart == pixel_weights.m_SrcEnd) {
        pixel_weights.m_Weights[0] = 65536;
      } else {
        pixel_weights.m_Weights[1] = FXSYS_round(
            (float)(src_pos - pixel_weights.m_SrcStart - 1.0f / 2) * 65536);
        pixel_weights.m_Weights[0] = 65536 - pixel_weights.m_Weights[1];
      }
    }
    return;
  }
  for (int dest_pixel = dest_min; dest_pixel < dest_max; dest_pixel++) {
    PixelWeight& pixel_weights = *GetPixelWeight(dest_pixel);
    double src_start = dest_pixel * scale + base;
    double src_end = src_start + scale;
    int start_i, end_i;
    if (src_start < src_end) {
      start_i = (int)floor((float)src_start);
      end_i = (int)ceil((float)src_end);
    } else {
      start_i = (int)floor((float)src_end);
      end_i = (int)ceil((float)src_start);
    }
    if (start_i < src_min) {
      start_i = src_min;
    }
    if (end_i >= src_max) {
      end_i = src_max - 1;
    }
    if (start_i > end_i) {
      pixel_weights.m_SrcStart = start_i;
      pixel_weights.m_SrcEnd = start_i;
      continue;
    }
    pixel_weights.m_SrcStart = start_i;
    pixel_weights.m_SrcEnd = end_i;
    for (int j = start_i; j <= end_i; j++) {
      double dest_start = ((float)j - base) / scale;
      double dest_end = ((float)(j + 1) - base) / scale;
      if (dest_start > dest_end) {
        double temp = dest_start;
        dest_start = dest_end;
        dest_end = temp;
      }
      double area_start =
          dest_start > (float)(dest_pixel) ? dest_start : (float)(dest_pixel);
      double area_end = dest_end > (float)(dest_pixel + 1)
                            ? (float)(dest_pixel + 1)
                            : dest_end;
      double weight = area_start >= area_end ? 0.0f : area_end - area_start;
      if (weight == 0 && j == end_i) {
        pixel_weights.m_SrcEnd--;
        break;
      }
      pixel_weights.m_Weights[j - start_i] =
          FXSYS_round((float)(weight * 65536));
    }
  }
}

CCodec_ProgressiveDecoder::CFXCODEC_HorzTable::CFXCODEC_HorzTable() {}

CCodec_ProgressiveDecoder::CFXCODEC_HorzTable::~CFXCODEC_HorzTable() {}

void CCodec_ProgressiveDecoder::CFXCODEC_HorzTable::Calc(int dest_len,
                                                         int src_len) {
  double scale = (double)dest_len / (double)src_len;
  m_ItemSize = sizeof(int) * 4;
  int size = dest_len * m_ItemSize + 4;
  m_pWeightTables.resize(size, 0);
  if (scale > 1) {
    int pre_dest_col = 0;
    for (int src_col = 0; src_col < src_len; src_col++) {
      double dest_col_f = src_col * scale;
      int dest_col = FXSYS_round((float)dest_col_f);
      PixelWeight* pWeight = GetPixelWeight(dest_col);
      pWeight->m_SrcStart = pWeight->m_SrcEnd = src_col;
      pWeight->m_Weights[0] = 65536;
      pWeight->m_Weights[1] = 0;
      if (src_col == src_len - 1 && dest_col < dest_len - 1) {
        for (int dest_col_index = pre_dest_col + 1; dest_col_index < dest_len;
             dest_col_index++) {
          pWeight = GetPixelWeight(dest_col_index);
          pWeight->m_SrcStart = pWeight->m_SrcEnd = src_col;
          pWeight->m_Weights[0] = 65536;
          pWeight->m_Weights[1] = 0;
        }
        return;
      }
      int dest_col_len = dest_col - pre_dest_col;
      for (int dest_col_index = pre_dest_col + 1; dest_col_index < dest_col;
           dest_col_index++) {
        pWeight = GetPixelWeight(dest_col_index);
        pWeight->m_SrcStart = src_col - 1;
        pWeight->m_SrcEnd = src_col;
        pWeight->m_Weights[0] =
            FXSYS_round((float)(((float)dest_col - (float)dest_col_index) /
                                (float)dest_col_len * 65536));
        pWeight->m_Weights[1] = 65536 - pWeight->m_Weights[0];
      }
      pre_dest_col = dest_col;
    }
    return;
  }
  for (int dest_col = 0; dest_col < dest_len; dest_col++) {
    double src_col_f = dest_col / scale;
    int src_col = FXSYS_round((float)src_col_f);
    PixelWeight* pWeight = GetPixelWeight(dest_col);
    pWeight->m_SrcStart = pWeight->m_SrcEnd = src_col;
    pWeight->m_Weights[0] = 65536;
    pWeight->m_Weights[1] = 0;
  }
}

CCodec_ProgressiveDecoder::CFXCODEC_VertTable::CFXCODEC_VertTable() {}

CCodec_ProgressiveDecoder::CFXCODEC_VertTable::~CFXCODEC_VertTable() {}

void CCodec_ProgressiveDecoder::CFXCODEC_VertTable::Calc(int dest_len,
                                                         int src_len) {
  double scale = (double)dest_len / (double)src_len;
  m_ItemSize = sizeof(int) * 4;
  int size = dest_len * m_ItemSize + 4;
  m_pWeightTables.resize(size, 0);
  if (scale <= 1) {
    for (int dest_row = 0; dest_row < dest_len; dest_row++) {
      PixelWeight* pWeight = GetPixelWeight(dest_row);
      pWeight->m_SrcStart = dest_row;
      pWeight->m_SrcEnd = dest_row;
      pWeight->m_Weights[0] = 65536;
      pWeight->m_Weights[1] = 0;
    }
    return;
  }

  double step = 0.0;
  int src_row = 0;
  while (step < (double)dest_len) {
    int start_step = (int)step;
    step = scale * (++src_row);
    int end_step = (int)step;
    if (end_step >= dest_len) {
      end_step = dest_len;
      for (int dest_row = start_step; dest_row < end_step; dest_row++) {
        PixelWeight* pWeight = GetPixelWeight(dest_row);
        pWeight->m_SrcStart = start_step;
        pWeight->m_SrcEnd = start_step;
        pWeight->m_Weights[0] = 65536;
        pWeight->m_Weights[1] = 0;
      }
      return;
    }
    int length = end_step - start_step;
    {
      PixelWeight* pWeight = GetPixelWeight(start_step);
      pWeight->m_SrcStart = start_step;
      pWeight->m_SrcEnd = start_step;
      pWeight->m_Weights[0] = 65536;
      pWeight->m_Weights[1] = 0;
    }
    for (int dest_row = start_step + 1; dest_row < end_step; dest_row++) {
      PixelWeight* pWeight = GetPixelWeight(dest_row);
      pWeight->m_SrcStart = start_step;
      pWeight->m_SrcEnd = end_step;
      pWeight->m_Weights[0] =
          FXSYS_round((float)(end_step - dest_row) / (float)length * 65536);
      pWeight->m_Weights[1] = 65536 - pWeight->m_Weights[0];
    }
  }
}

CCodec_ProgressiveDecoder::CCodec_ProgressiveDecoder(
    CCodec_ModuleMgr* pCodecMgr)
    : m_pCodecMgr(pCodecMgr) {}

CCodec_ProgressiveDecoder::~CCodec_ProgressiveDecoder() {}

#ifdef PDF_ENABLE_XFA_PNG
bool CCodec_ProgressiveDecoder::PngReadHeader(int width,
                                              int height,
                                              int bpc,
                                              int pass,
                                              int* color_type,
                                              double* gamma) {
  if (!m_pDeviceBitmap) {
    m_SrcWidth = width;
    m_SrcHeight = height;
    m_SrcBPC = bpc;
    m_SrcPassNumber = pass;
    switch (*color_type) {
      case 0:
        m_SrcComponents = 1;
        break;
      case 4:
        m_SrcComponents = 2;
        break;
      case 2:
        m_SrcComponents = 3;
        break;
      case 3:
      case 6:
        m_SrcComponents = 4;
        break;
      default:
        m_SrcComponents = 0;
        break;
    }
    m_clipBox = FX_RECT(0, 0, width, height);
    return false;
  }
  FXDIB_Format format = m_pDeviceBitmap->GetFormat();
  switch (format) {
    case FXDIB_1bppMask:
    case FXDIB_1bppRgb:
      NOTREACHED();
      return false;
    case FXDIB_8bppMask:
    case FXDIB_8bppRgb:
      *color_type = 0;
      break;
    case FXDIB_Rgb:
      *color_type = 2;
      break;
    case FXDIB_Rgb32:
    case FXDIB_Argb:
      *color_type = 6;
      break;
    default:
      NOTREACHED();
      return false;
  }
  *gamma = kPngGamma;
  return true;
}

bool CCodec_ProgressiveDecoder::PngAskScanlineBuf(int line, uint8_t** pSrcBuf) {
  RetainPtr<CFX_DIBitmap> pDIBitmap = m_pDeviceBitmap;
  if (!pDIBitmap) {
    NOTREACHED();
    return false;
  }
  if (line >= m_clipBox.top && line < m_clipBox.bottom) {
    double scale_y = static_cast<double>(m_sizeY) / m_clipBox.Height();
    int32_t row = (int32_t)((line - m_clipBox.top) * scale_y) + m_startY;
    const uint8_t* src_scan = pDIBitmap->GetScanline(row);
    uint8_t* dest_scan = m_pDecodeBuf.get();
    *pSrcBuf = m_pDecodeBuf.get();
    int32_t src_Bpp = pDIBitmap->GetBPP() >> 3;
    int32_t dest_Bpp = (m_SrcFormat & 0xff) >> 3;
    int32_t src_left = m_startX;
    int32_t dest_left = m_clipBox.left;
    src_scan += src_left * src_Bpp;
    dest_scan += dest_left * dest_Bpp;
    for (int32_t src_col = 0; src_col < m_sizeX; src_col++) {
      PixelWeight* pPixelWeights = m_WeightHorzOO.GetPixelWeight(src_col);
      if (pPixelWeights->m_SrcStart != pPixelWeights->m_SrcEnd) {
        continue;
      }
      switch (pDIBitmap->GetFormat()) {
        case FXDIB_1bppMask:
        case FXDIB_1bppRgb:
          NOTREACHED();
          return false;
        case FXDIB_8bppMask:
        case FXDIB_8bppRgb: {
          if (pDIBitmap->GetPalette()) {
            return false;
          }
          uint32_t dest_g = 0;
          dest_g += pPixelWeights->m_Weights[0] * src_scan[src_col];
          dest_scan[pPixelWeights->m_SrcStart] = (uint8_t)(dest_g >> 16);
        } break;
        case FXDIB_Rgb:
        case FXDIB_Rgb32: {
          uint32_t dest_b = 0;
          uint32_t dest_g = 0;
          uint32_t dest_r = 0;
          const uint8_t* p = src_scan + src_col * src_Bpp;
          dest_b += pPixelWeights->m_Weights[0] * (*p++);
          dest_g += pPixelWeights->m_Weights[0] * (*p++);
          dest_r += pPixelWeights->m_Weights[0] * (*p);
          uint8_t* pDes = &dest_scan[pPixelWeights->m_SrcStart * dest_Bpp];
          *pDes++ = (uint8_t)((dest_b) >> 16);
          *pDes++ = (uint8_t)((dest_g) >> 16);
          *pDes = (uint8_t)((dest_r) >> 16);
        } break;
        case FXDIB_Argb: {
          uint32_t dest_r = 0;
          uint32_t dest_g = 0;
          uint32_t dest_b = 0;
          const uint8_t* p = src_scan + src_col * src_Bpp;
          dest_b += pPixelWeights->m_Weights[0] * (*p++);
          dest_g += pPixelWeights->m_Weights[0] * (*p++);
          dest_r += pPixelWeights->m_Weights[0] * (*p++);
          uint8_t* pDes = &dest_scan[pPixelWeights->m_SrcStart * dest_Bpp];
          *pDes++ = (uint8_t)((dest_b) >> 16);
          *pDes++ = (uint8_t)((dest_g) >> 16);
          *pDes++ = (uint8_t)((dest_r) >> 16);
          *pDes = *p;
        } break;
        default:
          return false;
      }
    }
  }
  return true;
}

void CCodec_ProgressiveDecoder::PngFillScanlineBufCompleted(int pass,
                                                            int line) {
  RetainPtr<CFX_DIBitmap> pDIBitmap = m_pDeviceBitmap;
  ASSERT(pDIBitmap);
  int src_top = m_clipBox.top;
  int src_bottom = m_clipBox.bottom;
  int dest_top = m_startY;
  int src_height = m_clipBox.Height();
  int dest_height = m_sizeY;
  if (line >= src_top && line < src_bottom) {
    double scale_y = static_cast<double>(dest_height) / src_height;
    int src_row = line - src_top;
    int dest_row = (int)(src_row * scale_y) + dest_top;
    if (dest_row >= dest_top + dest_height) {
      return;
    }
    PngOneOneMapResampleHorz(pDIBitmap, dest_row, m_pDecodeBuf.get(),
                             m_SrcFormat);
    if (m_SrcPassNumber == 1 && scale_y > 1.0) {
      ResampleVert(pDIBitmap, scale_y, dest_row);
      return;
    }
    if (pass == 6 && scale_y > 1.0) {
      ResampleVert(pDIBitmap, scale_y, dest_row);
    }
  }
}
#endif  // PDF_ENABLE_XFA_PNG

#ifdef PDF_ENABLE_XFA_GIF
void CCodec_ProgressiveDecoder::GifRecordCurrentPosition(uint32_t& cur_pos) {
  uint32_t remain_size =
      m_pCodecMgr->GetGifModule()->GetAvailInput(m_pGifContext.get());
  cur_pos = m_offSet - remain_size;
}

bool CCodec_ProgressiveDecoder::GifInputRecordPositionBuf(
    uint32_t rcd_pos,
    const FX_RECT& img_rc,
    int32_t pal_num,
    CFX_GifPalette* pal_ptr,
    int32_t delay_time,
    bool user_input,
    int32_t trans_index,
    int32_t disposal_method,
    bool interlace) {
  m_offSet = rcd_pos;
  m_InvalidateGifBuffer = true;

  FXCODEC_STATUS error_status = FXCODEC_STATUS_ERROR;
  if (!GifReadMoreData(m_pCodecMgr->GetGifModule(), error_status)) {
    return false;
  }
  CFX_GifPalette* pPalette = nullptr;
  if (pal_num != 0 && pal_ptr) {
    pPalette = pal_ptr;
  } else {
    if (!m_pGifPalette)
      return false;
    pal_num = m_GifPltNumber;
    pPalette = m_pGifPalette;
  }
  if (!m_pSrcPalette)
    m_pSrcPalette.reset(FX_Alloc(FX_ARGB, pal_num));
  else if (pal_num > m_SrcPaletteNumber)
    m_pSrcPalette.reset(FX_Realloc(FX_ARGB, m_pSrcPalette.release(), pal_num));
  if (!m_pSrcPalette)
    return false;

  m_SrcPaletteNumber = pal_num;
  for (int i = 0; i < pal_num; i++) {
    m_pSrcPalette.get()[i] =
        ArgbEncode(0xff, pPalette[i].r, pPalette[i].g, pPalette[i].b);
  }
  m_GifTransIndex = trans_index;
  m_GifFrameRect = img_rc;
  m_SrcPassNumber = interlace ? 4 : 1;
  int32_t pal_index = m_GifBgIndex;
  RetainPtr<CFX_DIBitmap> pDevice = m_pDeviceBitmap;
  if (trans_index >= pal_num)
    trans_index = -1;
  if (trans_index != -1) {
    m_pSrcPalette.get()[trans_index] &= 0x00ffffff;
    if (pDevice->HasAlpha())
      pal_index = trans_index;
  }
  if (pal_index >= pal_num)
    return false;

  int startX = m_startX;
  int startY = m_startY;
  int sizeX = m_sizeX;
  int sizeY = m_sizeY;
  int Bpp = pDevice->GetBPP() / 8;
  FX_ARGB argb = m_pSrcPalette.get()[pal_index];
  for (int row = 0; row < sizeY; row++) {
    uint8_t* pScanline =
        pDevice->GetWritableScanline(row + startY) + startX * Bpp;
    switch (m_TransMethod) {
      case 3: {
        uint8_t gray =
            FXRGB2GRAY(FXARGB_R(argb), FXARGB_G(argb), FXARGB_B(argb));
        memset(pScanline, gray, sizeX);
        break;
      }
      case 8: {
        for (int col = 0; col < sizeX; col++) {
          *pScanline++ = FXARGB_B(argb);
          *pScanline++ = FXARGB_G(argb);
          *pScanline++ = FXARGB_R(argb);
          pScanline += Bpp - 3;
        }
        break;
      }
      case 12: {
        for (int col = 0; col < sizeX; col++) {
          FXARGB_SETDIB(pScanline, argb);
          pScanline += 4;
        }
        break;
      }
    }
  }
  return true;
}

void CCodec_ProgressiveDecoder::GifReadScanline(int32_t row_num,
                                                uint8_t* row_buf) {
  RetainPtr<CFX_DIBitmap> pDIBitmap = m_pDeviceBitmap;
  ASSERT(pDIBitmap);
  int32_t img_width = m_GifFrameRect.Width();
  if (!pDIBitmap->HasAlpha()) {
    uint8_t* byte_ptr = row_buf;
    for (int i = 0; i < img_width; i++) {
      if (*byte_ptr == m_GifTransIndex) {
        *byte_ptr = m_GifBgIndex;
      }
      byte_ptr++;
    }
  }
  int32_t pal_index = m_GifBgIndex;
  if (m_GifTransIndex != -1 && m_pDeviceBitmap->HasAlpha()) {
    pal_index = m_GifTransIndex;
  }
  memset(m_pDecodeBuf.get(), pal_index, m_SrcWidth);
  bool bLastPass = (row_num % 2) == 1;
  int32_t line = row_num + m_GifFrameRect.top;
  int32_t left = m_GifFrameRect.left;
  memcpy(m_pDecodeBuf.get() + left, row_buf, img_width);
  int src_top = m_clipBox.top;
  int src_bottom = m_clipBox.bottom;
  int dest_top = m_startY;
  int src_height = m_clipBox.Height();
  int dest_height = m_sizeY;
  if (line < src_top || line >= src_bottom)
    return;

  double scale_y = static_cast<double>(dest_height) / src_height;
  int src_row = line - src_top;
  int dest_row = (int)(src_row * scale_y) + dest_top;
  if (dest_row >= dest_top + dest_height)
    return;

  ReSampleScanline(pDIBitmap, dest_row, m_pDecodeBuf.get(), m_SrcFormat);
  if (scale_y > 1.0 && m_SrcPassNumber == 1) {
    ResampleVert(pDIBitmap, scale_y, dest_row);
    return;
  }
  if (scale_y <= 1.0)
    return;

  int dest_bottom = dest_top + m_sizeY;
  int dest_Bpp = pDIBitmap->GetBPP() >> 3;
  uint32_t dest_ScanOffet = m_startX * dest_Bpp;
  if (dest_row + (int)scale_y >= dest_bottom - 1) {
    const uint8_t* scan_src = pDIBitmap->GetScanline(dest_row) + dest_ScanOffet;
    int cur_row = dest_row;
    while (++cur_row < dest_bottom) {
      uint8_t* scan_des =
          pDIBitmap->GetWritableScanline(cur_row) + dest_ScanOffet;
      uint32_t size = m_sizeX * dest_Bpp;
      memmove(scan_des, scan_src, size);
    }
  }
  if (bLastPass)
    GifDoubleLineResampleVert(pDIBitmap, scale_y, dest_row);
}
#endif  // PDF_ENABLE_XFA_GIF

#ifdef PDF_ENABLE_XFA_BMP
bool CCodec_ProgressiveDecoder::BmpInputImagePositionBuf(uint32_t rcd_pos) {
  m_offSet = rcd_pos;
  FXCODEC_STATUS error_status = FXCODEC_STATUS_ERROR;
  return BmpReadMoreData(m_pCodecMgr->GetBmpModule(), m_pBmpContext.get(),
                         error_status);
}

void CCodec_ProgressiveDecoder::BmpReadScanline(
    uint32_t row_num,
    const std::vector<uint8_t>& row_buf) {
  RetainPtr<CFX_DIBitmap> pDIBitmap = m_pDeviceBitmap;
  ASSERT(pDIBitmap);
  std::copy(row_buf.begin(), row_buf.begin() + m_ScanlineSize,
            m_pDecodeBuf.get());
  int src_top = m_clipBox.top;
  int src_bottom = m_clipBox.bottom;
  int dest_top = m_startY;
  int src_height = m_clipBox.Height();
  int dest_height = m_sizeY;
  if ((src_top >= 0 && row_num < static_cast<uint32_t>(src_top)) ||
      src_bottom < 0 || row_num >= static_cast<uint32_t>(src_bottom)) {
    return;
  }

  double scale_y = static_cast<double>(dest_height) / src_height;
  int src_row = row_num - src_top;
  int dest_row = (int)(src_row * scale_y) + dest_top;
  if (dest_row >= dest_top + dest_height)
    return;

  ReSampleScanline(pDIBitmap, dest_row, m_pDecodeBuf.get(), m_SrcFormat);
  if (scale_y <= 1.0)
    return;

  if (m_BmpIsTopBottom) {
    ResampleVert(pDIBitmap, scale_y, dest_row);
    return;
  }
  ResampleVertBT(pDIBitmap, scale_y, dest_row);
}

void CCodec_ProgressiveDecoder::ResampleVertBT(
    const RetainPtr<CFX_DIBitmap>& pDeviceBitmap,
    double scale_y,
    int dest_row) {
  int dest_Bpp = pDeviceBitmap->GetBPP() >> 3;
  uint32_t dest_ScanOffet = m_startX * dest_Bpp;
  int dest_top = m_startY;
  int dest_bottom = m_startY + m_sizeY;
  pdfium::base::CheckedNumeric<int> check_dest_row_1 = dest_row;
  check_dest_row_1 += pdfium::base::checked_cast<int>(scale_y);
  int dest_row_1 = check_dest_row_1.ValueOrDie();
  if (dest_row_1 >= dest_bottom - 1) {
    const uint8_t* scan_src =
        pDeviceBitmap->GetScanline(dest_row) + dest_ScanOffet;
    while (++dest_row < dest_bottom) {
      uint8_t* scan_des =
          pDeviceBitmap->GetWritableScanline(dest_row) + dest_ScanOffet;
      uint32_t size = m_sizeX * dest_Bpp;
      memmove(scan_des, scan_src, size);
    }
    return;
  }
  for (; dest_row_1 > dest_row; dest_row_1--) {
    uint8_t* scan_des =
        pDeviceBitmap->GetWritableScanline(dest_row_1) + dest_ScanOffet;
    PixelWeight* pWeight = m_WeightVert.GetPixelWeight(dest_row_1 - dest_top);
    const uint8_t* scan_src1 =
        pDeviceBitmap->GetScanline(pWeight->m_SrcStart + dest_top) +
        dest_ScanOffet;
    const uint8_t* scan_src2 =
        pDeviceBitmap->GetScanline(pWeight->m_SrcEnd + dest_top) +
        dest_ScanOffet;
    for (int dest_col = 0; dest_col < m_sizeX; dest_col++) {
      switch (pDeviceBitmap->GetFormat()) {
        case FXDIB_Invalid:
        case FXDIB_1bppMask:
        case FXDIB_1bppRgb:
          return;
        case FXDIB_8bppMask:
        case FXDIB_8bppRgb: {
          if (pDeviceBitmap->GetPalette()) {
            return;
          }
          int dest_g = 0;
          dest_g += pWeight->m_Weights[0] * (*scan_src1++);
          dest_g += pWeight->m_Weights[1] * (*scan_src2++);
          *scan_des++ = (uint8_t)(dest_g >> 16);
        } break;
        case FXDIB_Rgb:
        case FXDIB_Rgb32: {
          uint32_t dest_b = 0;
          uint32_t dest_g = 0;
          uint32_t dest_r = 0;
          dest_b += pWeight->m_Weights[0] * (*scan_src1++);
          dest_g += pWeight->m_Weights[0] * (*scan_src1++);
          dest_r += pWeight->m_Weights[0] * (*scan_src1++);
          scan_src1 += dest_Bpp - 3;
          dest_b += pWeight->m_Weights[1] * (*scan_src2++);
          dest_g += pWeight->m_Weights[1] * (*scan_src2++);
          dest_r += pWeight->m_Weights[1] * (*scan_src2++);
          scan_src2 += dest_Bpp - 3;
          *scan_des++ = (uint8_t)((dest_b) >> 16);
          *scan_des++ = (uint8_t)((dest_g) >> 16);
          *scan_des++ = (uint8_t)((dest_r) >> 16);
          scan_des += dest_Bpp - 3;
        } break;
        case FXDIB_Argb: {
          uint32_t dest_a = 0;
          uint32_t dest_b = 0;
          uint32_t dest_g = 0;
          uint32_t dest_r = 0;
          dest_b += pWeight->m_Weights[0] * (*scan_src1++);
          dest_g += pWeight->m_Weights[0] * (*scan_src1++);
          dest_r += pWeight->m_Weights[0] * (*scan_src1++);
          dest_a += pWeight->m_Weights[0] * (*scan_src1++);
          dest_b += pWeight->m_Weights[1] * (*scan_src2++);
          dest_g += pWeight->m_Weights[1] * (*scan_src2++);
          dest_r += pWeight->m_Weights[1] * (*scan_src2++);
          dest_a += pWeight->m_Weights[1] * (*scan_src2++);
          *scan_des++ = (uint8_t)((dest_b) >> 16);
          *scan_des++ = (uint8_t)((dest_g) >> 16);
          *scan_des++ = (uint8_t)((dest_r) >> 16);
          *scan_des++ = (uint8_t)((dest_a) >> 16);
        } break;
        default:
          return;
      }
    }
  }
}

bool CCodec_ProgressiveDecoder::BmpDetectImageTypeInBuffer(
    CFX_DIBAttribute* pAttribute) {
  CCodec_BmpModule* pBmpModule = m_pCodecMgr->GetBmpModule();
  if (!pBmpModule) {
    m_status = FXCODEC_STATUS_ERR_MEMORY;
    return false;
  }

  std::unique_ptr<CodecModuleIface::Context> pBmpContext =
      pBmpModule->Start(this);
  pBmpModule->Input(pBmpContext.get(), m_pCodecMemory, nullptr);

  std::vector<uint32_t> palette;
  int32_t readResult = pBmpModule->ReadHeader(
      pBmpContext.get(), &m_SrcWidth, &m_SrcHeight, &m_BmpIsTopBottom,
      &m_SrcComponents, &m_SrcPaletteNumber, &palette, pAttribute);
  while (readResult == 2) {
    FXCODEC_STATUS error_status = FXCODEC_STATUS_ERR_FORMAT;
    if (!BmpReadMoreData(pBmpModule, pBmpContext.get(), error_status)) {
      m_status = error_status;
      return false;
    }
    readResult = pBmpModule->ReadHeader(
        pBmpContext.get(), &m_SrcWidth, &m_SrcHeight, &m_BmpIsTopBottom,
        &m_SrcComponents, &m_SrcPaletteNumber, &palette, pAttribute);
  }

  if (readResult != 1) {
    m_status = FXCODEC_STATUS_ERR_FORMAT;
    return false;
  }

  FXDIB_Format format = FXDIB_Invalid;
  switch (m_SrcComponents) {
    case 1:
      m_SrcFormat = FXCodec_8bppRgb;
      format = FXDIB_8bppRgb;
      break;
    case 3:
      m_SrcFormat = FXCodec_Rgb;
      format = FXDIB_Rgb;
      break;
    case 4:
      m_SrcFormat = FXCodec_Rgb32;
      format = FXDIB_Rgb32;
      break;
    default:
      m_status = FXCODEC_STATUS_ERR_FORMAT;
      return false;
  }

  uint32_t pitch = 0;
  uint32_t neededData = 0;
  if (!CFX_DIBitmap::CalculatePitchAndSize(m_SrcWidth, m_SrcHeight, format,
                                           &pitch, &neededData)) {
    m_status = FXCODEC_STATUS_ERR_FORMAT;
    return false;
  }

  uint32_t availableData = m_pCodecMemory->GetSize() > m_offSet
                               ? m_pCodecMemory->GetSize() - m_offSet
                               : 0;
  if (neededData > availableData) {
    m_status = FXCODEC_STATUS_ERR_FORMAT;
    return false;
  }

  m_SrcBPC = 8;
  m_clipBox = FX_RECT(0, 0, m_SrcWidth, m_SrcHeight);
  m_pBmpContext = std::move(pBmpContext);
  if (m_SrcPaletteNumber) {
    m_pSrcPalette.reset(FX_Alloc(FX_ARGB, m_SrcPaletteNumber));
    memcpy(m_pSrcPalette.get(), palette.data(),
           m_SrcPaletteNumber * sizeof(FX_ARGB));
  } else {
    m_pSrcPalette.reset();
  }
  return true;
}

bool CCodec_ProgressiveDecoder::BmpReadMoreData(
    CCodec_BmpModule* pBmpModule,
    CodecModuleIface::Context* pContext,
    FXCODEC_STATUS& err_status) {
  return ReadMoreData(pBmpModule, pContext, false, err_status);
}

FXCODEC_STATUS CCodec_ProgressiveDecoder::BmpStartDecode(
    const RetainPtr<CFX_DIBitmap>& pDIBitmap) {
  CCodec_BmpModule* pBmpModule = m_pCodecMgr->GetBmpModule();
  if (!pBmpModule) {
    m_pDeviceBitmap = nullptr;
    m_pFile = nullptr;
    m_status = FXCODEC_STATUS_ERR_MEMORY;
    return m_status;
  }
  GetTransMethod(m_pDeviceBitmap->GetFormat(), m_SrcFormat);
  m_ScanlineSize = FxAlignToBoundary<4>(m_SrcWidth * m_SrcComponents);
  m_pDecodeBuf.reset(FX_Alloc(uint8_t, m_ScanlineSize));
  m_WeightHorz.Calc(m_sizeX, 0, m_sizeX, m_clipBox.Width(), 0,
                    m_clipBox.Width());
  m_WeightVert.Calc(m_sizeY, m_clipBox.Height());
  m_status = FXCODEC_STATUS_DECODE_TOBECONTINUE;
  return m_status;
}

FXCODEC_STATUS CCodec_ProgressiveDecoder::BmpContinueDecode() {
  CCodec_BmpModule* pBmpModule = m_pCodecMgr->GetBmpModule();
  if (!pBmpModule) {
    m_status = FXCODEC_STATUS_ERR_MEMORY;
    return m_status;
  }
  while (true) {
    int32_t readRes = pBmpModule->LoadImage(m_pBmpContext.get());
    while (readRes == 2) {
      FXCODEC_STATUS error_status = FXCODEC_STATUS_DECODE_FINISH;
      if (!BmpReadMoreData(pBmpModule, m_pBmpContext.get(), error_status)) {
        m_pDeviceBitmap = nullptr;
        m_pFile = nullptr;
        m_status = error_status;
        return m_status;
      }
      readRes = pBmpModule->LoadImage(m_pBmpContext.get());
    }
    if (readRes == 1) {
      m_pDeviceBitmap = nullptr;
      m_pFile = nullptr;
      m_status = FXCODEC_STATUS_DECODE_FINISH;
      return m_status;
    }
    m_pDeviceBitmap = nullptr;
    m_pFile = nullptr;
    m_status = FXCODEC_STATUS_ERROR;
    return m_status;
  }
}
#endif  // PDF_ENABLE_XFA_BMP

#ifdef PDF_ENABLE_XFA_GIF
bool CCodec_ProgressiveDecoder::GifReadMoreData(CCodec_GifModule* pGifModule,
                                                FXCODEC_STATUS& err_status) {
  if (!ReadMoreData(pGifModule, m_pGifContext.get(), m_InvalidateGifBuffer,
                    err_status)) {
    return false;
  }
  m_InvalidateGifBuffer = false;
  return true;
}

bool CCodec_ProgressiveDecoder::GifDetectImageTypeInBuffer(
    CFX_DIBAttribute* pAttribute) {
  CCodec_GifModule* pGifModule = m_pCodecMgr->GetGifModule();
  if (!pGifModule) {
    m_status = FXCODEC_STATUS_ERR_MEMORY;
    return false;
  }
  m_pGifContext = pGifModule->Start(this);
  pGifModule->Input(m_pGifContext.get(), m_pCodecMemory, nullptr);
  m_SrcComponents = 1;
  CFX_GifDecodeStatus readResult = pGifModule->ReadHeader(
      m_pGifContext.get(), &m_SrcWidth, &m_SrcHeight, &m_GifPltNumber,
      &m_pGifPalette, &m_GifBgIndex, nullptr);
  while (readResult == CFX_GifDecodeStatus::Unfinished) {
    FXCODEC_STATUS error_status = FXCODEC_STATUS_ERR_FORMAT;
    if (!GifReadMoreData(pGifModule, error_status)) {
      m_pGifContext = nullptr;
      m_status = error_status;
      return false;
    }
    readResult = pGifModule->ReadHeader(m_pGifContext.get(), &m_SrcWidth,
                                        &m_SrcHeight, &m_GifPltNumber,
                                        &m_pGifPalette, &m_GifBgIndex, nullptr);
  }
  if (readResult == CFX_GifDecodeStatus::Success) {
    m_SrcBPC = 8;
    m_clipBox = FX_RECT(0, 0, m_SrcWidth, m_SrcHeight);
    return true;
  }
  m_pGifContext = nullptr;
  m_status = FXCODEC_STATUS_ERR_FORMAT;
  return false;
}

FXCODEC_STATUS CCodec_ProgressiveDecoder::GifStartDecode(
    const RetainPtr<CFX_DIBitmap>& pDIBitmap) {
  CCodec_GifModule* pGifModule = m_pCodecMgr->GetGifModule();
  if (!pGifModule) {
    m_pDeviceBitmap = nullptr;
    m_pFile = nullptr;
    m_status = FXCODEC_STATUS_ERR_MEMORY;
    return m_status;
  }
  m_SrcFormat = FXCodec_8bppRgb;
  GetTransMethod(m_pDeviceBitmap->GetFormat(), m_SrcFormat);
  int scanline_size = FxAlignToBoundary<4>(m_SrcWidth);
  m_pDecodeBuf.reset(FX_Alloc(uint8_t, scanline_size));
  m_WeightHorz.Calc(m_sizeX, 0, m_sizeX, m_clipBox.Width(), 0,
                    m_clipBox.Width());
  m_WeightVert.Calc(m_sizeY, m_clipBox.Height());
  m_FrameCur = 0;
  m_status = FXCODEC_STATUS_DECODE_TOBECONTINUE;
  return m_status;
}

FXCODEC_STATUS CCodec_ProgressiveDecoder::GifContinueDecode() {
  CCodec_GifModule* pGifModule = m_pCodecMgr->GetGifModule();
  if (!pGifModule) {
    m_pDeviceBitmap = nullptr;
    m_pFile = nullptr;
    m_status = FXCODEC_STATUS_ERR_MEMORY;
    return m_status;
  }

  CFX_GifDecodeStatus readRes =
      pGifModule->LoadFrame(m_pGifContext.get(), m_FrameCur, nullptr);
  while (readRes == CFX_GifDecodeStatus::Unfinished) {
    FXCODEC_STATUS error_status = FXCODEC_STATUS_DECODE_FINISH;
    if (!GifReadMoreData(pGifModule, error_status)) {
      m_pDeviceBitmap = nullptr;
      m_pFile = nullptr;
      m_status = error_status;
      return m_status;
    }
    readRes = pGifModule->LoadFrame(m_pGifContext.get(), m_FrameCur, nullptr);
  }

  if (readRes == CFX_GifDecodeStatus::Success) {
    m_pDeviceBitmap = nullptr;
    m_pFile = nullptr;
    m_status = FXCODEC_STATUS_DECODE_FINISH;
    return m_status;
  }

  m_pDeviceBitmap = nullptr;
  m_pFile = nullptr;
  m_status = FXCODEC_STATUS_ERROR;
  return m_status;
}

void CCodec_ProgressiveDecoder::GifDoubleLineResampleVert(
    const RetainPtr<CFX_DIBitmap>& pDeviceBitmap,
    double scale_y,
    int dest_row) {
  int dest_Bpp = pDeviceBitmap->GetBPP() >> 3;
  uint32_t dest_ScanOffet = m_startX * dest_Bpp;
  int dest_top = m_startY;
  pdfium::base::CheckedNumeric<double> scale_y2 = scale_y;
  scale_y2 *= 2;
  pdfium::base::CheckedNumeric<int> check_dest_row_1 = dest_row;
  check_dest_row_1 -= scale_y2.ValueOrDie();
  int dest_row_1 = check_dest_row_1.ValueOrDie();
  dest_row_1 = std::max(dest_row_1, dest_top);
  for (; dest_row_1 < dest_row; dest_row_1++) {
    uint8_t* scan_des =
        pDeviceBitmap->GetWritableScanline(dest_row_1) + dest_ScanOffet;
    PixelWeight* pWeight = m_WeightVert.GetPixelWeight(dest_row_1 - dest_top);
    const uint8_t* scan_src1 =
        pDeviceBitmap->GetScanline(pWeight->m_SrcStart + dest_top) +
        dest_ScanOffet;
    const uint8_t* scan_src2 =
        pDeviceBitmap->GetScanline(pWeight->m_SrcEnd + dest_top) +
        dest_ScanOffet;
    for (int dest_col = 0; dest_col < m_sizeX; dest_col++) {
      switch (pDeviceBitmap->GetFormat()) {
        case FXDIB_Invalid:
        case FXDIB_1bppMask:
        case FXDIB_1bppRgb:
          return;
        case FXDIB_8bppMask:
        case FXDIB_8bppRgb: {
          if (pDeviceBitmap->GetPalette()) {
            return;
          }
          int dest_g = 0;
          dest_g += pWeight->m_Weights[0] * (*scan_src1++);
          dest_g += pWeight->m_Weights[1] * (*scan_src2++);
          *scan_des++ = (uint8_t)(dest_g >> 16);
        } break;
        case FXDIB_Rgb:
        case FXDIB_Rgb32: {
          uint32_t dest_b = 0;
          uint32_t dest_g = 0;
          uint32_t dest_r = 0;
          dest_b += pWeight->m_Weights[0] * (*scan_src1++);
          dest_g += pWeight->m_Weights[0] * (*scan_src1++);
          dest_r += pWeight->m_Weights[0] * (*scan_src1++);
          scan_src1 += dest_Bpp - 3;
          dest_b += pWeight->m_Weights[1] * (*scan_src2++);
          dest_g += pWeight->m_Weights[1] * (*scan_src2++);
          dest_r += pWeight->m_Weights[1] * (*scan_src2++);
          scan_src2 += dest_Bpp - 3;
          *scan_des++ = (uint8_t)((dest_b) >> 16);
          *scan_des++ = (uint8_t)((dest_g) >> 16);
          *scan_des++ = (uint8_t)((dest_r) >> 16);
          scan_des += dest_Bpp - 3;
        } break;
        case FXDIB_Argb: {
          uint32_t dest_a = 0;
          uint32_t dest_b = 0;
          uint32_t dest_g = 0;
          uint32_t dest_r = 0;
          dest_b += pWeight->m_Weights[0] * (*scan_src1++);
          dest_g += pWeight->m_Weights[0] * (*scan_src1++);
          dest_r += pWeight->m_Weights[0] * (*scan_src1++);
          dest_a += pWeight->m_Weights[0] * (*scan_src1++);
          dest_b += pWeight->m_Weights[1] * (*scan_src2++);
          dest_g += pWeight->m_Weights[1] * (*scan_src2++);
          dest_r += pWeight->m_Weights[1] * (*scan_src2++);
          dest_a += pWeight->m_Weights[1] * (*scan_src2++);
          *scan_des++ = (uint8_t)((dest_b) >> 16);
          *scan_des++ = (uint8_t)((dest_g) >> 16);
          *scan_des++ = (uint8_t)((dest_r) >> 16);
          *scan_des++ = (uint8_t)((dest_a) >> 16);
        } break;
        default:
          return;
      }
    }
  }
  int dest_bottom = dest_top + m_sizeY - 1;
  if (dest_row + (int)(2 * scale_y) >= dest_bottom &&
      dest_row + (int)scale_y < dest_bottom) {
    GifDoubleLineResampleVert(pDeviceBitmap, scale_y, dest_row + (int)scale_y);
  }
}
#endif  // PDF_ENABLE_XFA_GIF

bool CCodec_ProgressiveDecoder::JpegReadMoreData(CCodec_JpegModule* pJpegModule,
                                                 FXCODEC_STATUS& err_status) {
  return ReadMoreData(pJpegModule, m_pJpegContext.get(), false, err_status);
}

bool CCodec_ProgressiveDecoder::JpegDetectImageTypeInBuffer(
    CFX_DIBAttribute* pAttribute) {
  CCodec_JpegModule* pJpegModule = m_pCodecMgr->GetJpegModule();
  m_pJpegContext = pJpegModule->Start();
  if (!m_pJpegContext) {
    m_status = FXCODEC_STATUS_ERR_MEMORY;
    return false;
  }
  pJpegModule->Input(m_pJpegContext.get(), m_pCodecMemory, nullptr);

  // Setting jump marker before calling ReadHeader, since a longjmp to
  // the marker indicates a fatal error.
  if (setjmp(*pJpegModule->GetJumpMark(m_pJpegContext.get())) == -1) {
    m_pJpegContext.reset();
    m_status = FXCODEC_STATUS_ERR_FORMAT;
    return false;
  }

  int32_t readResult =
      pJpegModule->ReadHeader(m_pJpegContext.get(), &m_SrcWidth, &m_SrcHeight,
                              &m_SrcComponents, pAttribute);
  while (readResult == 2) {
    FXCODEC_STATUS error_status = FXCODEC_STATUS_ERR_FORMAT;
    if (!JpegReadMoreData(pJpegModule, error_status)) {
      m_status = error_status;
      return false;
    }
    readResult =
        pJpegModule->ReadHeader(m_pJpegContext.get(), &m_SrcWidth, &m_SrcHeight,
                                &m_SrcComponents, pAttribute);
  }
  if (!readResult) {
    m_SrcBPC = 8;
    m_clipBox = FX_RECT(0, 0, m_SrcWidth, m_SrcHeight);
    return true;
  }
  m_pJpegContext.reset();
  m_status = FXCODEC_STATUS_ERR_FORMAT;
  return false;
}

FXCODEC_STATUS CCodec_ProgressiveDecoder::JpegStartDecode(
    const RetainPtr<CFX_DIBitmap>& pDIBitmap) {
  CCodec_JpegModule* pJpegModule = m_pCodecMgr->GetJpegModule();
  int down_scale = 1;
  GetDownScale(down_scale);
  // Setting jump marker before calling StartScanLine, since a longjmp to
  // the marker indicates a fatal error.
  if (setjmp(*pJpegModule->GetJumpMark(m_pJpegContext.get())) == -1) {
    m_pJpegContext.reset();
    m_status = FXCODEC_STATUS_ERROR;
    return FXCODEC_STATUS_ERROR;
  }

  bool startStatus =
      pJpegModule->StartScanline(m_pJpegContext.get(), down_scale);
  while (!startStatus) {
    FXCODEC_STATUS error_status = FXCODEC_STATUS_ERROR;
    if (!JpegReadMoreData(pJpegModule, error_status)) {
      m_pDeviceBitmap = nullptr;
      m_pFile = nullptr;
      m_status = error_status;
      return m_status;
    }

    startStatus = pJpegModule->StartScanline(m_pJpegContext.get(), down_scale);
  }
  int scanline_size = (m_SrcWidth + down_scale - 1) / down_scale;
  scanline_size = FxAlignToBoundary<4>(scanline_size * m_SrcComponents);
  m_pDecodeBuf.reset(FX_Alloc(uint8_t, scanline_size));
  m_WeightHorz.Calc(m_sizeX, 0, m_sizeX, m_clipBox.Width(), 0,
                    m_clipBox.Width());
  m_WeightVert.Calc(m_sizeY, m_clipBox.Height());
  switch (m_SrcComponents) {
    case 1:
      m_SrcFormat = FXCodec_8bppGray;
      break;
    case 3:
      m_SrcFormat = FXCodec_Rgb;
      break;
    case 4:
      m_SrcFormat = FXCodec_Cmyk;
      break;
  }
  GetTransMethod(pDIBitmap->GetFormat(), m_SrcFormat);
  m_status = FXCODEC_STATUS_DECODE_TOBECONTINUE;
  return m_status;
}

FXCODEC_STATUS CCodec_ProgressiveDecoder::JpegContinueDecode() {
  CCodec_JpegModule* pJpegModule = m_pCodecMgr->GetJpegModule();
  // Setting jump marker before calling ReadScanLine, since a longjmp to
  // the marker indicates a fatal error.
  if (setjmp(*pJpegModule->GetJumpMark(m_pJpegContext.get())) == -1) {
    m_pJpegContext.reset();
    m_status = FXCODEC_STATUS_ERROR;
    return FXCODEC_STATUS_ERROR;
  }

  while (true) {
    bool readRes =
        pJpegModule->ReadScanline(m_pJpegContext.get(), m_pDecodeBuf.get());
    while (!readRes) {
      FXCODEC_STATUS error_status = FXCODEC_STATUS_DECODE_FINISH;
      if (!JpegReadMoreData(pJpegModule, error_status)) {
        m_pDeviceBitmap = nullptr;
        m_pFile = nullptr;
        m_status = error_status;
        return m_status;
      }
      readRes =
          pJpegModule->ReadScanline(m_pJpegContext.get(), m_pDecodeBuf.get());
    }
    if (m_SrcFormat == FXCodec_Rgb) {
      int src_Bpp = (m_SrcFormat & 0xff) >> 3;
      RGB2BGR(m_pDecodeBuf.get() + m_clipBox.left * src_Bpp, m_clipBox.Width());
    }
    if (m_SrcRow >= m_clipBox.bottom) {
      m_pDeviceBitmap = nullptr;
      m_pFile = nullptr;
      m_status = FXCODEC_STATUS_DECODE_FINISH;
      return m_status;
    }
    Resample(m_pDeviceBitmap, m_SrcRow, m_pDecodeBuf.get(), m_SrcFormat);
    m_SrcRow++;
  }
}

#ifdef PDF_ENABLE_XFA_PNG
void CCodec_ProgressiveDecoder::PngOneOneMapResampleHorz(
    const RetainPtr<CFX_DIBitmap>& pDeviceBitmap,
    int32_t dest_line,
    uint8_t* src_scan,
    FXCodec_Format src_format) {
  uint8_t* dest_scan = pDeviceBitmap->GetWritableScanline(dest_line);
  int32_t src_Bpp = (m_SrcFormat & 0xff) >> 3;
  int32_t dest_Bpp = pDeviceBitmap->GetBPP() >> 3;
  int32_t src_left = m_clipBox.left;
  int32_t dest_left = m_startX;
  src_scan += src_left * src_Bpp;
  dest_scan += dest_left * dest_Bpp;
  for (int32_t dest_col = 0; dest_col < m_sizeX; dest_col++) {
    PixelWeight* pPixelWeights = m_WeightHorzOO.GetPixelWeight(dest_col);
    switch (pDeviceBitmap->GetFormat()) {
      case FXDIB_1bppMask:
      case FXDIB_1bppRgb:
        NOTREACHED();
        return;
      case FXDIB_8bppMask:
      case FXDIB_8bppRgb: {
        if (pDeviceBitmap->GetPalette()) {
          return;
        }
        uint32_t dest_g = 0;
        dest_g +=
            pPixelWeights->m_Weights[0] * src_scan[pPixelWeights->m_SrcStart];
        dest_g +=
            pPixelWeights->m_Weights[1] * src_scan[pPixelWeights->m_SrcEnd];
        *dest_scan++ = (uint8_t)(dest_g >> 16);
      } break;
      case FXDIB_Rgb:
      case FXDIB_Rgb32: {
        uint32_t dest_b = 0;
        uint32_t dest_g = 0;
        uint32_t dest_r = 0;
        const uint8_t* p = src_scan;
        p = src_scan + pPixelWeights->m_SrcStart * src_Bpp;
        dest_b += pPixelWeights->m_Weights[0] * (*p++);
        dest_g += pPixelWeights->m_Weights[0] * (*p++);
        dest_r += pPixelWeights->m_Weights[0] * (*p);
        p = src_scan + pPixelWeights->m_SrcEnd * src_Bpp;
        dest_b += pPixelWeights->m_Weights[1] * (*p++);
        dest_g += pPixelWeights->m_Weights[1] * (*p++);
        dest_r += pPixelWeights->m_Weights[1] * (*p);
        *dest_scan++ = (uint8_t)((dest_b) >> 16);
        *dest_scan++ = (uint8_t)((dest_g) >> 16);
        *dest_scan++ = (uint8_t)((dest_r) >> 16);
        dest_scan += dest_Bpp - 3;
      } break;
      case FXDIB_Argb: {
        uint32_t dest_a = 0;
        uint32_t dest_b = 0;
        uint32_t dest_g = 0;
        uint32_t dest_r = 0;
        const uint8_t* p = src_scan;
        p = src_scan + pPixelWeights->m_SrcStart * src_Bpp;
        dest_b += pPixelWeights->m_Weights[0] * (*p++);
        dest_g += pPixelWeights->m_Weights[0] * (*p++);
        dest_r += pPixelWeights->m_Weights[0] * (*p++);
        dest_a += pPixelWeights->m_Weights[0] * (*p);
        p = src_scan + pPixelWeights->m_SrcEnd * src_Bpp;
        dest_b += pPixelWeights->m_Weights[1] * (*p++);
        dest_g += pPixelWeights->m_Weights[1] * (*p++);
        dest_r += pPixelWeights->m_Weights[1] * (*p++);
        dest_a += pPixelWeights->m_Weights[1] * (*p);
        *dest_scan++ = (uint8_t)((dest_b) >> 16);
        *dest_scan++ = (uint8_t)((dest_g) >> 16);
        *dest_scan++ = (uint8_t)((dest_r) >> 16);
        *dest_scan++ = (uint8_t)((dest_a) >> 16);
      } break;
      default:
        return;
    }
  }
}

bool CCodec_ProgressiveDecoder::PngDetectImageTypeInBuffer(
    CFX_DIBAttribute* pAttribute) {
  CCodec_PngModule* pPngModule = m_pCodecMgr->GetPngModule();
  if (!pPngModule) {
    m_status = FXCODEC_STATUS_ERR_MEMORY;
    return false;
  }
  m_pPngContext = pPngModule->Start(this);
  if (!m_pPngContext) {
    m_status = FXCODEC_STATUS_ERR_MEMORY;
    return false;
  }
  while (pPngModule->Input(m_pPngContext.get(), m_pCodecMemory, pAttribute)) {
    uint32_t remain_size = static_cast<uint32_t>(m_pFile->GetSize()) - m_offSet;
    uint32_t input_size =
        remain_size > FXCODEC_BLOCK_SIZE ? FXCODEC_BLOCK_SIZE : remain_size;
    if (input_size == 0) {
      m_pPngContext.reset();
      m_status = FXCODEC_STATUS_ERR_FORMAT;
      return false;
    }
    if (m_pCodecMemory && input_size > m_pCodecMemory->GetSize())
      m_pCodecMemory = pdfium::MakeRetain<CFX_CodecMemory>(input_size);

    if (!m_pFile->ReadBlockAtOffset(m_pCodecMemory->GetBuffer(), m_offSet,
                                    input_size)) {
      m_status = FXCODEC_STATUS_ERR_READ;
      return false;
    }
    m_offSet += input_size;
  }
  m_pPngContext.reset();
  if (m_SrcPassNumber == 0) {
    m_status = FXCODEC_STATUS_ERR_FORMAT;
    return false;
  }
  return true;
}

FXCODEC_STATUS CCodec_ProgressiveDecoder::PngStartDecode(
    const RetainPtr<CFX_DIBitmap>& pDIBitmap) {
  CCodec_PngModule* pPngModule = m_pCodecMgr->GetPngModule();
  if (!pPngModule) {
    m_pDeviceBitmap = nullptr;
    m_pFile = nullptr;
    m_status = FXCODEC_STATUS_ERR_MEMORY;
    return m_status;
  }
  m_pPngContext = pPngModule->Start(this);
  if (!m_pPngContext) {
    m_pDeviceBitmap = nullptr;
    m_pFile = nullptr;
    m_status = FXCODEC_STATUS_ERR_MEMORY;
    return m_status;
  }
  m_offSet = 0;
  switch (m_pDeviceBitmap->GetFormat()) {
    case FXDIB_8bppMask:
    case FXDIB_8bppRgb:
      m_SrcComponents = 1;
      m_SrcFormat = FXCodec_8bppGray;
      break;
    case FXDIB_Rgb:
      m_SrcComponents = 3;
      m_SrcFormat = FXCodec_Rgb;
      break;
    case FXDIB_Rgb32:
    case FXDIB_Argb:
      m_SrcComponents = 4;
      m_SrcFormat = FXCodec_Argb;
      break;
    default: {
      m_pDeviceBitmap = nullptr;
      m_pFile = nullptr;
      m_status = FXCODEC_STATUS_ERR_PARAMS;
      return m_status;
    }
  }
  GetTransMethod(m_pDeviceBitmap->GetFormat(), m_SrcFormat);
  int scanline_size = FxAlignToBoundary<4>(m_SrcWidth * m_SrcComponents);
  m_pDecodeBuf.reset(FX_Alloc(uint8_t, scanline_size));
  m_WeightHorzOO.Calc(m_sizeX, m_clipBox.Width());
  m_WeightVert.Calc(m_sizeY, m_clipBox.Height());
  m_status = FXCODEC_STATUS_DECODE_TOBECONTINUE;
  return m_status;
}

FXCODEC_STATUS CCodec_ProgressiveDecoder::PngContinueDecode() {
  CCodec_PngModule* pPngModule = m_pCodecMgr->GetPngModule();
  if (!pPngModule) {
    m_status = FXCODEC_STATUS_ERR_MEMORY;
    return m_status;
  }
  while (true) {
    uint32_t remain_size = (uint32_t)m_pFile->GetSize() - m_offSet;
    uint32_t input_size =
        remain_size > FXCODEC_BLOCK_SIZE ? FXCODEC_BLOCK_SIZE : remain_size;
    if (input_size == 0) {
      m_pPngContext.reset();
      m_pDeviceBitmap = nullptr;
      m_pFile = nullptr;
      m_status = FXCODEC_STATUS_DECODE_FINISH;
      return m_status;
    }
    if (m_pCodecMemory && input_size > m_pCodecMemory->GetSize())
      m_pCodecMemory = pdfium::MakeRetain<CFX_CodecMemory>(input_size);

    bool bResult = m_pFile->ReadBlockAtOffset(m_pCodecMemory->GetBuffer(),
                                              m_offSet, input_size);
    if (!bResult) {
      m_pDeviceBitmap = nullptr;
      m_pFile = nullptr;
      m_status = FXCODEC_STATUS_ERR_READ;
      return m_status;
    }
    m_offSet += input_size;
    bResult = pPngModule->Input(m_pPngContext.get(), m_pCodecMemory, nullptr);
    if (!bResult) {
      m_pDeviceBitmap = nullptr;
      m_pFile = nullptr;
      m_status = FXCODEC_STATUS_ERROR;
      return m_status;
    }
  }
}
#endif  // PDF_ENABLE_XFA_PNG

#ifdef PDF_ENABLE_XFA_TIFF
bool CCodec_ProgressiveDecoder::TiffDetectImageTypeFromFile(
    CFX_DIBAttribute* pAttribute) {
  CCodec_TiffModule* pTiffModule = m_pCodecMgr->GetTiffModule();
  if (!pTiffModule) {
    m_status = FXCODEC_STATUS_ERR_FORMAT;
    return false;
  }
  m_pTiffContext = pTiffModule->CreateDecoder(m_pFile);
  if (!m_pTiffContext) {
    m_status = FXCODEC_STATUS_ERR_FORMAT;
    return false;
  }
  int32_t dummy_bpc;
  bool ret = pTiffModule->LoadFrameInfo(m_pTiffContext.get(), 0, &m_SrcWidth,
                                        &m_SrcHeight, &m_SrcComponents,
                                        &dummy_bpc, pAttribute);
  m_SrcComponents = 4;
  m_clipBox = FX_RECT(0, 0, m_SrcWidth, m_SrcHeight);
  if (!ret) {
    m_pTiffContext.reset();
    m_status = FXCODEC_STATUS_ERR_FORMAT;
    return false;
  }
  return true;
}

FXCODEC_STATUS CCodec_ProgressiveDecoder::TiffContinueDecode() {
  CCodec_TiffModule* pTiffModule = m_pCodecMgr->GetTiffModule();
  if (!pTiffModule) {
    m_status = FXCODEC_STATUS_ERR_MEMORY;
    return m_status;
  }
  bool ret = false;
  if (m_pDeviceBitmap->GetBPP() == 32 &&
      m_pDeviceBitmap->GetWidth() == m_SrcWidth && m_SrcWidth == m_sizeX &&
      m_pDeviceBitmap->GetHeight() == m_SrcHeight && m_SrcHeight == m_sizeY &&
      m_startX == 0 && m_startY == 0 && m_clipBox.left == 0 &&
      m_clipBox.top == 0 && m_clipBox.right == m_SrcWidth &&
      m_clipBox.bottom == m_SrcHeight) {
    ret = pTiffModule->Decode(m_pTiffContext.get(), m_pDeviceBitmap);
    m_pDeviceBitmap = nullptr;
    m_pFile = nullptr;
    if (!ret) {
      m_status = FXCODEC_STATUS_ERROR;
      return m_status;
    }
    m_status = FXCODEC_STATUS_DECODE_FINISH;
    return m_status;
  }

  auto pDIBitmap = pdfium::MakeRetain<CFX_DIBitmap>();
  pDIBitmap->Create(m_SrcWidth, m_SrcHeight, FXDIB_Argb);
  if (!pDIBitmap->GetBuffer()) {
    m_pDeviceBitmap = nullptr;
    m_pFile = nullptr;
    m_status = FXCODEC_STATUS_ERR_MEMORY;
    return m_status;
  }
  ret = pTiffModule->Decode(m_pTiffContext.get(), pDIBitmap);
  if (!ret) {
    m_pDeviceBitmap = nullptr;
    m_pFile = nullptr;
    m_status = FXCODEC_STATUS_ERROR;
    return m_status;
  }
  RetainPtr<CFX_DIBitmap> pClipBitmap =
      (m_clipBox.left == 0 && m_clipBox.top == 0 &&
       m_clipBox.right == m_SrcWidth && m_clipBox.bottom == m_SrcHeight)
          ? pDIBitmap
          : pDIBitmap->Clone(&m_clipBox);
  if (!pClipBitmap) {
    m_pDeviceBitmap = nullptr;
    m_pFile = nullptr;
    m_status = FXCODEC_STATUS_ERR_MEMORY;
    return m_status;
  }
  RetainPtr<CFX_DIBitmap> pFormatBitmap;
  switch (m_pDeviceBitmap->GetFormat()) {
    case FXDIB_8bppRgb:
      pFormatBitmap = pdfium::MakeRetain<CFX_DIBitmap>();
      pFormatBitmap->Create(pClipBitmap->GetWidth(), pClipBitmap->GetHeight(),
                            FXDIB_8bppRgb);
      break;
    case FXDIB_8bppMask:
      pFormatBitmap = pdfium::MakeRetain<CFX_DIBitmap>();
      pFormatBitmap->Create(pClipBitmap->GetWidth(), pClipBitmap->GetHeight(),
                            FXDIB_8bppMask);
      break;
    case FXDIB_Rgb:
      pFormatBitmap = pdfium::MakeRetain<CFX_DIBitmap>();
      pFormatBitmap->Create(pClipBitmap->GetWidth(), pClipBitmap->GetHeight(),
                            FXDIB_Rgb);
      break;
    case FXDIB_Rgb32:
      pFormatBitmap = pdfium::MakeRetain<CFX_DIBitmap>();
      pFormatBitmap->Create(pClipBitmap->GetWidth(), pClipBitmap->GetHeight(),
                            FXDIB_Rgb32);
      break;
    case FXDIB_Argb:
      pFormatBitmap = pClipBitmap;
      break;
    default:
      break;
  }
  switch (m_pDeviceBitmap->GetFormat()) {
    case FXDIB_8bppRgb:
    case FXDIB_8bppMask: {
      for (int32_t row = 0; row < pClipBitmap->GetHeight(); row++) {
        const uint8_t* src_line = pClipBitmap->GetScanline(row);
        uint8_t* dest_line = pFormatBitmap->GetWritableScanline(row);
        for (int32_t col = 0; col < pClipBitmap->GetWidth(); col++) {
          uint8_t _a = 255 - src_line[3];
          uint8_t b = (src_line[0] * src_line[3] + 0xFF * _a) / 255;
          uint8_t g = (src_line[1] * src_line[3] + 0xFF * _a) / 255;
          uint8_t r = (src_line[2] * src_line[3] + 0xFF * _a) / 255;
          *dest_line++ = FXRGB2GRAY(r, g, b);
          src_line += 4;
        }
      }
    } break;
    case FXDIB_Rgb:
    case FXDIB_Rgb32: {
      int32_t desBpp = (m_pDeviceBitmap->GetFormat() == FXDIB_Rgb) ? 3 : 4;
      for (int32_t row = 0; row < pClipBitmap->GetHeight(); row++) {
        const uint8_t* src_line = pClipBitmap->GetScanline(row);
        uint8_t* dest_line = pFormatBitmap->GetWritableScanline(row);
        for (int32_t col = 0; col < pClipBitmap->GetWidth(); col++) {
          uint8_t _a = 255 - src_line[3];
          uint8_t b = (src_line[0] * src_line[3] + 0xFF * _a) / 255;
          uint8_t g = (src_line[1] * src_line[3] + 0xFF * _a) / 255;
          uint8_t r = (src_line[2] * src_line[3] + 0xFF * _a) / 255;
          *dest_line++ = b;
          *dest_line++ = g;
          *dest_line++ = r;
          dest_line += desBpp - 3;
          src_line += 4;
        }
      }
    } break;
    default:
      break;
  }
  if (!pFormatBitmap) {
    m_pDeviceBitmap = nullptr;
    m_pFile = nullptr;
    m_status = FXCODEC_STATUS_ERR_MEMORY;
    return m_status;
  }
  RetainPtr<CFX_DIBitmap> pStrechBitmap = pFormatBitmap->StretchTo(
      m_sizeX, m_sizeY, kBilinearInterpolation, nullptr);
  pFormatBitmap = nullptr;
  if (!pStrechBitmap) {
    m_pDeviceBitmap = nullptr;
    m_pFile = nullptr;
    m_status = FXCODEC_STATUS_ERR_MEMORY;
    return m_status;
  }
  m_pDeviceBitmap->TransferBitmap(m_startX, m_startY, m_sizeX, m_sizeY,
                                  pStrechBitmap, 0, 0);
  m_pDeviceBitmap = nullptr;
  m_pFile = nullptr;
  m_status = FXCODEC_STATUS_DECODE_FINISH;
  return m_status;
}
#endif  // PDF_ENABLE_XFA_TIFF

bool CCodec_ProgressiveDecoder::DetectImageType(FXCODEC_IMAGE_TYPE imageType,
                                                CFX_DIBAttribute* pAttribute) {
#ifdef PDF_ENABLE_XFA_TIFF
  if (imageType == FXCODEC_IMAGE_TIFF)
    return TiffDetectImageTypeFromFile(pAttribute);
#endif  // PDF_ENABLE_XFA_TIFF

  size_t size = std::min<size_t>(m_pFile->GetSize(), FXCODEC_BLOCK_SIZE);
  m_pCodecMemory = pdfium::MakeRetain<CFX_CodecMemory>(size);
  m_offSet = 0;
  if (!m_pFile->ReadBlockAtOffset(m_pCodecMemory->GetBuffer(), m_offSet,
                                  size)) {
    m_status = FXCODEC_STATUS_ERR_READ;
    return false;
  }
  m_offSet += size;

  if (imageType == FXCODEC_IMAGE_JPG)
    return JpegDetectImageTypeInBuffer(pAttribute);

#ifdef PDF_ENABLE_XFA_BMP
  if (imageType == FXCODEC_IMAGE_BMP)
    return BmpDetectImageTypeInBuffer(pAttribute);
#endif  // PDF_ENABLE_XFA_BMP

#ifdef PDF_ENABLE_XFA_GIF
  if (imageType == FXCODEC_IMAGE_GIF)
    return GifDetectImageTypeInBuffer(pAttribute);
#endif  // PDF_ENABLE_XFA_GIF

#ifdef PDF_ENABLE_XFA_PNG
  if (imageType == FXCODEC_IMAGE_PNG)
    return PngDetectImageTypeInBuffer(pAttribute);
#endif  // PDF_ENABLE_XFA_PNG

  m_status = FXCODEC_STATUS_ERR_FORMAT;
  return false;
}

bool CCodec_ProgressiveDecoder::ReadMoreData(
    CodecModuleIface* pModule,
    CodecModuleIface::Context* pContext,
    bool invalidate_buffer,
    FXCODEC_STATUS& err_status) {
  // Check for EOF.
  if (m_offSet >= static_cast<uint32_t>(m_pFile->GetSize()))
    return false;

  // Try to get whatever remains.
  uint32_t dwBytesToFetchFromFile = m_pFile->GetSize() - m_offSet;

  // Figure out if the codec stopped processing midway through the buffer.
  size_t dwUnconsumed = 0;
  if (!invalidate_buffer) {
    FX_SAFE_SIZE_T avail_input = pModule->GetAvailInput(pContext);
    if (!avail_input.IsValid())
      return false;
    dwUnconsumed = avail_input.ValueOrDie();
  }

  if (dwUnconsumed == m_pCodecMemory->GetSize()) {
    // Codec couldn't make any progress against the bytes in the buffer.
    // Increase the buffer size so that there might be enough contiguous
    // bytes to allow whatever operation is having difficulty to succeed.
    dwBytesToFetchFromFile =
        std::min<uint32_t>(dwBytesToFetchFromFile, FXCODEC_BLOCK_SIZE);
    size_t dwNewSize = m_pCodecMemory->GetSize() + dwBytesToFetchFromFile;
    if (!m_pCodecMemory->TryResize(dwNewSize)) {
      err_status = FXCODEC_STATUS_ERR_MEMORY;
      return false;
    }
  } else {
    size_t dwConsumed = m_pCodecMemory->GetSize() - dwUnconsumed;
    m_pCodecMemory->Consume(dwConsumed);
    dwBytesToFetchFromFile =
        std::min<uint32_t>(dwBytesToFetchFromFile, dwConsumed);
  }

  // Append new data past the bytes not yet processed by the codec.
  if (!m_pFile->ReadBlockAtOffset(m_pCodecMemory->GetBuffer() + dwUnconsumed,
                                  m_offSet, dwBytesToFetchFromFile)) {
    err_status = FXCODEC_STATUS_ERR_READ;
    return false;
  }
  m_offSet += dwBytesToFetchFromFile;
  return pModule->Input(pContext, m_pCodecMemory, nullptr);
}

FXCODEC_STATUS CCodec_ProgressiveDecoder::LoadImageInfo(
    const RetainPtr<IFX_SeekableReadStream>& pFile,
    FXCODEC_IMAGE_TYPE imageType,
    CFX_DIBAttribute* pAttribute,
    bool bSkipImageTypeCheck) {
  switch (m_status) {
    case FXCODEC_STATUS_FRAME_READY:
    case FXCODEC_STATUS_FRAME_TOBECONTINUE:
    case FXCODEC_STATUS_DECODE_READY:
    case FXCODEC_STATUS_DECODE_TOBECONTINUE:
      return FXCODEC_STATUS_ERROR;
    default:
      break;
  }
  if (!pFile) {
    m_status = FXCODEC_STATUS_ERR_PARAMS;
    m_pFile = nullptr;
    return m_status;
  }
  m_pFile = pFile;
  m_offSet = 0;
  m_SrcWidth = m_SrcHeight = 0;
  m_SrcComponents = m_SrcBPC = 0;
  m_clipBox = FX_RECT(0, 0, 0, 0);
  m_startX = m_startY = 0;
  m_sizeX = m_sizeY = 0;
  m_SrcPassNumber = 0;
  if (imageType != FXCODEC_IMAGE_UNKNOWN &&
      DetectImageType(imageType, pAttribute)) {
    m_imageType = imageType;
    m_status = FXCODEC_STATUS_FRAME_READY;
    return m_status;
  }
  // If we got here then the image data does not match the requested decoder.
  // If we're skipping the type check then bail out at this point and return
  // the failed status.
  if (bSkipImageTypeCheck)
    return m_status;

  for (int type = FXCODEC_IMAGE_UNKNOWN + 1; type < FXCODEC_IMAGE_MAX; type++) {
    if (DetectImageType(static_cast<FXCODEC_IMAGE_TYPE>(type), pAttribute)) {
      m_imageType = static_cast<FXCODEC_IMAGE_TYPE>(type);
      m_status = FXCODEC_STATUS_FRAME_READY;
      return m_status;
    }
  }
  m_status = FXCODEC_STATUS_ERR_FORMAT;
  m_pFile = nullptr;
  return m_status;
}

void CCodec_ProgressiveDecoder::SetClipBox(FX_RECT* clip) {
  if (m_status != FXCODEC_STATUS_FRAME_READY)
    return;

  if (clip->IsEmpty()) {
    m_clipBox = FX_RECT(0, 0, 0, 0);
    return;
  }
  clip->left = std::max(clip->left, 0);
  clip->right = std::min(clip->right, m_SrcWidth);
  clip->top = std::max(clip->top, 0);
  clip->bottom = std::min(clip->bottom, m_SrcHeight);
  if (clip->IsEmpty()) {
    m_clipBox = FX_RECT(0, 0, 0, 0);
    return;
  }
  m_clipBox = *clip;
}

void CCodec_ProgressiveDecoder::GetDownScale(int& down_scale) {
  down_scale = 1;
  int ratio_w = m_clipBox.Width() / m_sizeX;
  int ratio_h = m_clipBox.Height() / m_sizeY;
  int ratio = (ratio_w > ratio_h) ? ratio_h : ratio_w;
  if (ratio >= 8) {
    down_scale = 8;
  } else if (ratio >= 4) {
    down_scale = 4;
  } else if (ratio >= 2) {
    down_scale = 2;
  }
  m_clipBox.left /= down_scale;
  m_clipBox.right /= down_scale;
  m_clipBox.top /= down_scale;
  m_clipBox.bottom /= down_scale;
  if (m_clipBox.right == m_clipBox.left) {
    m_clipBox.right = m_clipBox.left + 1;
  }
  if (m_clipBox.bottom == m_clipBox.top) {
    m_clipBox.bottom = m_clipBox.top + 1;
  }
}

void CCodec_ProgressiveDecoder::GetTransMethod(FXDIB_Format dest_format,
                                               FXCodec_Format src_format) {
  switch (dest_format) {
    case FXDIB_1bppMask:
    case FXDIB_1bppRgb: {
      switch (src_format) {
        case FXCodec_1bppGray:
          m_TransMethod = 0;
          break;
        default:
          m_TransMethod = -1;
      }
    } break;
    case FXDIB_8bppMask:
    case FXDIB_8bppRgb: {
      switch (src_format) {
        case FXCodec_1bppGray:
          m_TransMethod = 1;
          break;
        case FXCodec_8bppGray:
          m_TransMethod = 2;
          break;
        case FXCodec_1bppRgb:
        case FXCodec_8bppRgb:
          m_TransMethod = 3;
          break;
        case FXCodec_Rgb:
        case FXCodec_Rgb32:
        case FXCodec_Argb:
          m_TransMethod = 4;
          break;
        case FXCodec_Cmyk:
          m_TransMethod = 5;
          break;
        default:
          m_TransMethod = -1;
      }
    } break;
    case FXDIB_Rgb: {
      switch (src_format) {
        case FXCodec_1bppGray:
          m_TransMethod = 6;
          break;
        case FXCodec_8bppGray:
          m_TransMethod = 7;
          break;
        case FXCodec_1bppRgb:
        case FXCodec_8bppRgb:
          m_TransMethod = 8;
          break;
        case FXCodec_Rgb:
        case FXCodec_Rgb32:
        case FXCodec_Argb:
          m_TransMethod = 9;
          break;
        case FXCodec_Cmyk:
          m_TransMethod = 10;
          break;
        default:
          m_TransMethod = -1;
      }
    } break;
    case FXDIB_Rgb32:
    case FXDIB_Argb: {
      switch (src_format) {
        case FXCodec_1bppGray:
          m_TransMethod = 6;
          break;
        case FXCodec_8bppGray:
          m_TransMethod = 7;
          break;
        case FXCodec_1bppRgb:
        case FXCodec_8bppRgb:
          if (dest_format == FXDIB_Argb) {
            m_TransMethod = 12;
          } else {
            m_TransMethod = 8;
          }
          break;
        case FXCodec_Rgb:
        case FXCodec_Rgb32:
          m_TransMethod = 9;
          break;
        case FXCodec_Cmyk:
          m_TransMethod = 10;
          break;
        case FXCodec_Argb:
          m_TransMethod = 11;
          break;
        default:
          m_TransMethod = -1;
      }
    } break;
    default:
      m_TransMethod = -1;
  }
}

void CCodec_ProgressiveDecoder::ReSampleScanline(
    const RetainPtr<CFX_DIBitmap>& pDeviceBitmap,
    int dest_line,
    uint8_t* src_scan,
    FXCodec_Format src_format) {
  int src_left = m_clipBox.left;
  int dest_left = m_startX;
  uint8_t* dest_scan =
      pDeviceBitmap->GetBuffer() + dest_line * pDeviceBitmap->GetPitch();
  int src_bytes_per_pixel = (src_format & 0xff) / 8;
  int dest_bytes_per_pixel = pDeviceBitmap->GetBPP() / 8;
  src_scan += src_left * src_bytes_per_pixel;
  dest_scan += dest_left * dest_bytes_per_pixel;
  for (int dest_col = 0; dest_col < m_sizeX; dest_col++) {
    PixelWeight* pPixelWeights = m_WeightHorz.GetPixelWeight(dest_col);
    switch (m_TransMethod) {
      case -1:
        return;
      case 0:
        return;
      case 1:
        return;
      case 2: {
        uint32_t dest_g = 0;
        for (int j = pPixelWeights->m_SrcStart; j <= pPixelWeights->m_SrcEnd;
             j++) {
          int pixel_weight =
              pPixelWeights->m_Weights[j - pPixelWeights->m_SrcStart];
          dest_g += pixel_weight * src_scan[j];
        }
        *dest_scan++ = (uint8_t)(dest_g >> 16);
      } break;
      case 3: {
        int dest_r = 0;
        int dest_g = 0;
        int dest_b = 0;
        for (int j = pPixelWeights->m_SrcStart; j <= pPixelWeights->m_SrcEnd;
             j++) {
          int pixel_weight =
              pPixelWeights->m_Weights[j - pPixelWeights->m_SrcStart];
          unsigned long argb = m_pSrcPalette.get()[src_scan[j]];
          dest_r += pixel_weight * (uint8_t)(argb >> 16);
          dest_g += pixel_weight * (uint8_t)(argb >> 8);
          dest_b += pixel_weight * (uint8_t)argb;
        }
        *dest_scan++ =
            (uint8_t)FXRGB2GRAY((dest_r >> 16), (dest_g >> 16), (dest_b >> 16));
      } break;
      case 4: {
        uint32_t dest_b = 0;
        uint32_t dest_g = 0;
        uint32_t dest_r = 0;
        for (int j = pPixelWeights->m_SrcStart; j <= pPixelWeights->m_SrcEnd;
             j++) {
          int pixel_weight =
              pPixelWeights->m_Weights[j - pPixelWeights->m_SrcStart];
          const uint8_t* src_pixel = src_scan + j * src_bytes_per_pixel;
          dest_b += pixel_weight * (*src_pixel++);
          dest_g += pixel_weight * (*src_pixel++);
          dest_r += pixel_weight * (*src_pixel);
        }
        *dest_scan++ =
            (uint8_t)FXRGB2GRAY((dest_r >> 16), (dest_g >> 16), (dest_b >> 16));
      } break;
      case 5: {
        uint32_t dest_b = 0;
        uint32_t dest_g = 0;
        uint32_t dest_r = 0;
        for (int j = pPixelWeights->m_SrcStart; j <= pPixelWeights->m_SrcEnd;
             j++) {
          int pixel_weight =
              pPixelWeights->m_Weights[j - pPixelWeights->m_SrcStart];
          const uint8_t* src_pixel = src_scan + j * src_bytes_per_pixel;
          uint8_t src_b = 0;
          uint8_t src_g = 0;
          uint8_t src_r = 0;
          std::tie(src_r, src_g, src_b) =
              AdobeCMYK_to_sRGB1(255 - src_pixel[0], 255 - src_pixel[1],
                                 255 - src_pixel[2], 255 - src_pixel[3]);
          dest_b += pixel_weight * src_b;
          dest_g += pixel_weight * src_g;
          dest_r += pixel_weight * src_r;
        }
        *dest_scan++ =
            (uint8_t)FXRGB2GRAY((dest_r >> 16), (dest_g >> 16), (dest_b >> 16));
      } break;
      case 6:
        return;
      case 7: {
        uint32_t dest_g = 0;
        for (int j = pPixelWeights->m_SrcStart; j <= pPixelWeights->m_SrcEnd;
             j++) {
          int pixel_weight =
              pPixelWeights->m_Weights[j - pPixelWeights->m_SrcStart];
          dest_g += pixel_weight * src_scan[j];
        }
        memset(dest_scan, (uint8_t)(dest_g >> 16), 3);
        dest_scan += dest_bytes_per_pixel;
      } break;
      case 8: {
        int dest_r = 0;
        int dest_g = 0;
        int dest_b = 0;
        for (int j = pPixelWeights->m_SrcStart; j <= pPixelWeights->m_SrcEnd;
             j++) {
          int pixel_weight =
              pPixelWeights->m_Weights[j - pPixelWeights->m_SrcStart];
          unsigned long argb = m_pSrcPalette.get()[src_scan[j]];
          dest_r += pixel_weight * (uint8_t)(argb >> 16);
          dest_g += pixel_weight * (uint8_t)(argb >> 8);
          dest_b += pixel_weight * (uint8_t)argb;
        }
        *dest_scan++ = (uint8_t)((dest_b) >> 16);
        *dest_scan++ = (uint8_t)((dest_g) >> 16);
        *dest_scan++ = (uint8_t)((dest_r) >> 16);
        dest_scan += dest_bytes_per_pixel - 3;
      } break;
      case 12: {
#ifdef PDF_ENABLE_XFA_BMP
        if (m_pBmpContext) {
          int dest_r = 0;
          int dest_g = 0;
          int dest_b = 0;
          for (int j = pPixelWeights->m_SrcStart; j <= pPixelWeights->m_SrcEnd;
               j++) {
            int pixel_weight =
                pPixelWeights->m_Weights[j - pPixelWeights->m_SrcStart];
            unsigned long argb = m_pSrcPalette.get()[src_scan[j]];
            dest_r += pixel_weight * (uint8_t)(argb >> 16);
            dest_g += pixel_weight * (uint8_t)(argb >> 8);
            dest_b += pixel_weight * (uint8_t)argb;
          }
          *dest_scan++ = (uint8_t)((dest_b) >> 16);
          *dest_scan++ = (uint8_t)((dest_g) >> 16);
          *dest_scan++ = (uint8_t)((dest_r) >> 16);
          *dest_scan++ = 0xFF;
          break;
        }
#endif  // PDF_ENABLE_XFA_BMP
        int dest_a = 0;
        int dest_r = 0;
        int dest_g = 0;
        int dest_b = 0;
        for (int j = pPixelWeights->m_SrcStart; j <= pPixelWeights->m_SrcEnd;
             j++) {
          int pixel_weight =
              pPixelWeights->m_Weights[j - pPixelWeights->m_SrcStart];
          unsigned long argb = m_pSrcPalette.get()[src_scan[j]];
          dest_a += pixel_weight * (uint8_t)(argb >> 24);
          dest_r += pixel_weight * (uint8_t)(argb >> 16);
          dest_g += pixel_weight * (uint8_t)(argb >> 8);
          dest_b += pixel_weight * (uint8_t)argb;
        }
        *dest_scan++ = (uint8_t)((dest_b) >> 16);
        *dest_scan++ = (uint8_t)((dest_g) >> 16);
        *dest_scan++ = (uint8_t)((dest_r) >> 16);
        *dest_scan++ = (uint8_t)((dest_a) >> 16);
      } break;
      case 9: {
        uint32_t dest_b = 0;
        uint32_t dest_g = 0;
        uint32_t dest_r = 0;
        for (int j = pPixelWeights->m_SrcStart; j <= pPixelWeights->m_SrcEnd;
             j++) {
          int pixel_weight =
              pPixelWeights->m_Weights[j - pPixelWeights->m_SrcStart];
          const uint8_t* src_pixel = src_scan + j * src_bytes_per_pixel;
          dest_b += pixel_weight * (*src_pixel++);
          dest_g += pixel_weight * (*src_pixel++);
          dest_r += pixel_weight * (*src_pixel);
        }
        *dest_scan++ = (uint8_t)((dest_b) >> 16);
        *dest_scan++ = (uint8_t)((dest_g) >> 16);
        *dest_scan++ = (uint8_t)((dest_r) >> 16);
        dest_scan += dest_bytes_per_pixel - 3;
      } break;
      case 10: {
        uint32_t dest_b = 0;
        uint32_t dest_g = 0;
        uint32_t dest_r = 0;
        for (int j = pPixelWeights->m_SrcStart; j <= pPixelWeights->m_SrcEnd;
             j++) {
          int pixel_weight =
              pPixelWeights->m_Weights[j - pPixelWeights->m_SrcStart];
          const uint8_t* src_pixel = src_scan + j * src_bytes_per_pixel;
          uint8_t src_b = 0;
          uint8_t src_g = 0;
          uint8_t src_r = 0;
          std::tie(src_r, src_g, src_b) =
              AdobeCMYK_to_sRGB1(255 - src_pixel[0], 255 - src_pixel[1],
                                 255 - src_pixel[2], 255 - src_pixel[3]);
          dest_b += pixel_weight * src_b;
          dest_g += pixel_weight * src_g;
          dest_r += pixel_weight * src_r;
        }
        *dest_scan++ = (uint8_t)((dest_b) >> 16);
        *dest_scan++ = (uint8_t)((dest_g) >> 16);
        *dest_scan++ = (uint8_t)((dest_r) >> 16);
        dest_scan += dest_bytes_per_pixel - 3;
      } break;
      case 11: {
        uint32_t dest_alpha = 0;
        uint32_t dest_r = 0;
        uint32_t dest_g = 0;
        uint32_t dest_b = 0;
        for (int j = pPixelWeights->m_SrcStart; j <= pPixelWeights->m_SrcEnd;
             j++) {
          int pixel_weight =
              pPixelWeights->m_Weights[j - pPixelWeights->m_SrcStart];
          const uint8_t* src_pixel = src_scan + j * src_bytes_per_pixel;
          pixel_weight = pixel_weight * src_pixel[3] / 255;
          dest_b += pixel_weight * (*src_pixel++);
          dest_g += pixel_weight * (*src_pixel++);
          dest_r += pixel_weight * (*src_pixel);
          dest_alpha += pixel_weight;
        }
        *dest_scan++ = (uint8_t)((dest_b) >> 16);
        *dest_scan++ = (uint8_t)((dest_g) >> 16);
        *dest_scan++ = (uint8_t)((dest_r) >> 16);
        *dest_scan++ = (uint8_t)((dest_alpha * 255) >> 16);
      } break;
      default:
        return;
    }
  }
}

void CCodec_ProgressiveDecoder::ResampleVert(
    const RetainPtr<CFX_DIBitmap>& pDeviceBitmap,
    double scale_y,
    int dest_row) {
  int dest_Bpp = pDeviceBitmap->GetBPP() >> 3;
  uint32_t dest_ScanOffet = m_startX * dest_Bpp;
  int dest_top = m_startY;
  pdfium::base::CheckedNumeric<int> check_dest_row_1 = dest_row;
  check_dest_row_1 -= pdfium::base::checked_cast<int>(scale_y);
  int dest_row_1 = check_dest_row_1.ValueOrDie();
  if (dest_row_1 < dest_top) {
    int dest_bottom = dest_top + m_sizeY;
    if (dest_row + (int)scale_y >= dest_bottom - 1) {
      const uint8_t* scan_src =
          pDeviceBitmap->GetScanline(dest_row) + dest_ScanOffet;
      while (++dest_row < dest_bottom) {
        uint8_t* scan_des =
            pDeviceBitmap->GetWritableScanline(dest_row) + dest_ScanOffet;
        uint32_t size = m_sizeX * dest_Bpp;
        memmove(scan_des, scan_src, size);
      }
    }
    return;
  }
  for (; dest_row_1 < dest_row; dest_row_1++) {
    uint8_t* scan_des =
        pDeviceBitmap->GetWritableScanline(dest_row_1) + dest_ScanOffet;
    PixelWeight* pWeight = m_WeightVert.GetPixelWeight(dest_row_1 - dest_top);
    const uint8_t* scan_src1 =
        pDeviceBitmap->GetScanline(pWeight->m_SrcStart + dest_top) +
        dest_ScanOffet;
    const uint8_t* scan_src2 =
        pDeviceBitmap->GetScanline(pWeight->m_SrcEnd + dest_top) +
        dest_ScanOffet;
    for (int dest_col = 0; dest_col < m_sizeX; dest_col++) {
      switch (pDeviceBitmap->GetFormat()) {
        case FXDIB_Invalid:
        case FXDIB_1bppMask:
        case FXDIB_1bppRgb:
          return;
        case FXDIB_8bppMask:
        case FXDIB_8bppRgb: {
          if (pDeviceBitmap->GetPalette()) {
            return;
          }
          int dest_g = 0;
          dest_g += pWeight->m_Weights[0] * (*scan_src1++);
          dest_g += pWeight->m_Weights[1] * (*scan_src2++);
          *scan_des++ = (uint8_t)(dest_g >> 16);
        } break;
        case FXDIB_Rgb:
        case FXDIB_Rgb32: {
          uint32_t dest_b = 0;
          uint32_t dest_g = 0;
          uint32_t dest_r = 0;
          dest_b += pWeight->m_Weights[0] * (*scan_src1++);
          dest_g += pWeight->m_Weights[0] * (*scan_src1++);
          dest_r += pWeight->m_Weights[0] * (*scan_src1++);
          scan_src1 += dest_Bpp - 3;
          dest_b += pWeight->m_Weights[1] * (*scan_src2++);
          dest_g += pWeight->m_Weights[1] * (*scan_src2++);
          dest_r += pWeight->m_Weights[1] * (*scan_src2++);
          scan_src2 += dest_Bpp - 3;
          *scan_des++ = (uint8_t)((dest_b) >> 16);
          *scan_des++ = (uint8_t)((dest_g) >> 16);
          *scan_des++ = (uint8_t)((dest_r) >> 16);
          scan_des += dest_Bpp - 3;
        } break;
        case FXDIB_Argb: {
          uint32_t dest_a = 0;
          uint32_t dest_b = 0;
          uint32_t dest_g = 0;
          uint32_t dest_r = 0;
          dest_b += pWeight->m_Weights[0] * (*scan_src1++);
          dest_g += pWeight->m_Weights[0] * (*scan_src1++);
          dest_r += pWeight->m_Weights[0] * (*scan_src1++);
          dest_a += pWeight->m_Weights[0] * (*scan_src1++);
          dest_b += pWeight->m_Weights[1] * (*scan_src2++);
          dest_g += pWeight->m_Weights[1] * (*scan_src2++);
          dest_r += pWeight->m_Weights[1] * (*scan_src2++);
          dest_a += pWeight->m_Weights[1] * (*scan_src2++);
          *scan_des++ = (uint8_t)((dest_b) >> 16);
          *scan_des++ = (uint8_t)((dest_g) >> 16);
          *scan_des++ = (uint8_t)((dest_r) >> 16);
          *scan_des++ = (uint8_t)((dest_a) >> 16);
        } break;
        default:
          return;
      }
    }
  }
  int dest_bottom = dest_top + m_sizeY;
  if (dest_row + (int)scale_y >= dest_bottom - 1) {
    const uint8_t* scan_src =
        pDeviceBitmap->GetScanline(dest_row) + dest_ScanOffet;
    while (++dest_row < dest_bottom) {
      uint8_t* scan_des =
          pDeviceBitmap->GetWritableScanline(dest_row) + dest_ScanOffet;
      uint32_t size = m_sizeX * dest_Bpp;
      memmove(scan_des, scan_src, size);
    }
  }
}

void CCodec_ProgressiveDecoder::Resample(
    const RetainPtr<CFX_DIBitmap>& pDeviceBitmap,
    int32_t src_line,
    uint8_t* src_scan,
    FXCodec_Format src_format) {
  int src_top = m_clipBox.top;
  int dest_top = m_startY;
  int src_height = m_clipBox.Height();
  int dest_height = m_sizeY;
  if (src_line >= src_top) {
    double scale_y = static_cast<double>(dest_height) / src_height;
    int src_row = src_line - src_top;
    int dest_row = (int)(src_row * scale_y) + dest_top;
    if (dest_row >= dest_top + dest_height)
      return;

    ReSampleScanline(pDeviceBitmap, dest_row, m_pDecodeBuf.get(), src_format);
    if (scale_y > 1.0)
      ResampleVert(pDeviceBitmap, scale_y, dest_row);
  }
}

std::pair<FXCODEC_STATUS, size_t> CCodec_ProgressiveDecoder::GetFrames() {
  if (!(m_status == FXCODEC_STATUS_FRAME_READY ||
        m_status == FXCODEC_STATUS_FRAME_TOBECONTINUE)) {
    return {FXCODEC_STATUS_ERROR, 0};
  }

  switch (m_imageType) {
#ifdef PDF_ENABLE_XFA_BMP
    case FXCODEC_IMAGE_BMP:
#endif  // PDF_ENABLE_XFA_BMP
    case FXCODEC_IMAGE_JPG:
#ifdef PDF_ENABLE_XFA_PNG
    case FXCODEC_IMAGE_PNG:
#endif  // PDF_ENABLE_XFA_PNG
#ifdef PDF_ENABLE_XFA_TIFF
    case FXCODEC_IMAGE_TIFF:
#endif  // PDF_ENABLE_XFA_TIFF
      m_FrameNumber = 1;
      m_status = FXCODEC_STATUS_DECODE_READY;
      return {m_status, 1};
#ifdef PDF_ENABLE_XFA_GIF
    case FXCODEC_IMAGE_GIF: {
      CCodec_GifModule* pGifModule = m_pCodecMgr->GetGifModule();
      if (!pGifModule) {
        m_status = FXCODEC_STATUS_ERR_MEMORY;
        return {m_status, 0};
      }
      while (true) {
        CFX_GifDecodeStatus readResult;
        std::tie(readResult, m_FrameNumber) =
            pGifModule->LoadFrameInfo(m_pGifContext.get());
        while (readResult == CFX_GifDecodeStatus::Unfinished) {
          FXCODEC_STATUS error_status = FXCODEC_STATUS_ERR_READ;
          if (!GifReadMoreData(pGifModule, error_status))
            return {error_status, 0};

          std::tie(readResult, m_FrameNumber) =
              pGifModule->LoadFrameInfo(m_pGifContext.get());
        }
        if (readResult == CFX_GifDecodeStatus::Success) {
          m_status = FXCODEC_STATUS_DECODE_READY;
          return {m_status, m_FrameNumber};
        }
        m_pGifContext = nullptr;
        m_status = FXCODEC_STATUS_ERROR;
        return {m_status, 0};
      }
    }
#endif  // PDF_ENABLE_XFA_GIF
    default:
      return {FXCODEC_STATUS_ERROR, 0};
  }
}

FXCODEC_STATUS CCodec_ProgressiveDecoder::StartDecode(
    const RetainPtr<CFX_DIBitmap>& pDIBitmap,
    int start_x,
    int start_y,
    int size_x,
    int size_y) {
  if (m_status != FXCODEC_STATUS_DECODE_READY)
    return FXCODEC_STATUS_ERROR;

  if (!pDIBitmap || pDIBitmap->GetBPP() < 8 || m_FrameNumber == 0)
    return FXCODEC_STATUS_ERR_PARAMS;

  m_pDeviceBitmap = pDIBitmap;
  if (m_clipBox.IsEmpty())
    return FXCODEC_STATUS_ERR_PARAMS;
  if (size_x <= 0 || size_x > 65535 || size_y <= 0 || size_y > 65535)
    return FXCODEC_STATUS_ERR_PARAMS;

  FX_RECT device_rc =
      FX_RECT(start_x, start_y, start_x + size_x, start_y + size_y);
  int32_t out_range_x = device_rc.right - pDIBitmap->GetWidth();
  int32_t out_range_y = device_rc.bottom - pDIBitmap->GetHeight();
  device_rc.Intersect(
      FX_RECT(0, 0, pDIBitmap->GetWidth(), pDIBitmap->GetHeight()));
  if (device_rc.IsEmpty())
    return FXCODEC_STATUS_ERR_PARAMS;

  m_startX = device_rc.left;
  m_startY = device_rc.top;
  m_sizeX = device_rc.Width();
  m_sizeY = device_rc.Height();
  m_FrameCur = 0;
  if (start_x < 0 || out_range_x > 0) {
    float scaleX = (float)m_clipBox.Width() / (float)size_x;
    if (start_x < 0) {
      m_clipBox.left -= (int32_t)ceil((float)start_x * scaleX);
    }
    if (out_range_x > 0) {
      m_clipBox.right -= (int32_t)floor((float)out_range_x * scaleX);
    }
  }
  if (start_y < 0 || out_range_y > 0) {
    float scaleY = (float)m_clipBox.Height() / (float)size_y;
    if (start_y < 0) {
      m_clipBox.top -= (int32_t)ceil((float)start_y * scaleY);
    }
    if (out_range_y > 0) {
      m_clipBox.bottom -= (int32_t)floor((float)out_range_y * scaleY);
    }
  }
  if (m_clipBox.IsEmpty()) {
    return FXCODEC_STATUS_ERR_PARAMS;
  }
  switch (m_imageType) {
#ifdef PDF_ENABLE_XFA_BMP
    case FXCODEC_IMAGE_BMP:
      return BmpStartDecode(pDIBitmap);
#endif  // PDF_ENABLE_XFA_BMP
#ifdef PDF_ENABLE_XFA_GIF
    case FXCODEC_IMAGE_GIF:
      return GifStartDecode(pDIBitmap);
#endif  // PDF_ENABLE_XFA_GIF
    case FXCODEC_IMAGE_JPG:
      return JpegStartDecode(pDIBitmap);
#ifdef PDF_ENABLE_XFA_PNG
    case FXCODEC_IMAGE_PNG:
      return PngStartDecode(pDIBitmap);
#endif  // PDF_ENABLE_XFA_PNG
#ifdef PDF_ENABLE_XFA_TIFF
    case FXCODEC_IMAGE_TIFF:
      m_status = FXCODEC_STATUS_DECODE_TOBECONTINUE;
      return m_status;
#endif  // PDF_ENABLE_XFA_TIFF
    default:
      return FXCODEC_STATUS_ERROR;
  }
}

FXCODEC_STATUS CCodec_ProgressiveDecoder::ContinueDecode() {
  if (m_status != FXCODEC_STATUS_DECODE_TOBECONTINUE)
    return FXCODEC_STATUS_ERROR;

  switch (m_imageType) {
    case FXCODEC_IMAGE_JPG:
      return JpegContinueDecode();
#ifdef PDF_ENABLE_XFA_BMP
    case FXCODEC_IMAGE_BMP:
      return BmpContinueDecode();
#endif  // PDF_ENABLE_XFA_BMP
#ifdef PDF_ENABLE_XFA_GIF
    case FXCODEC_IMAGE_GIF:
      return GifContinueDecode();
#endif  // PDF_ENABLE_XFA_GIF
#ifdef PDF_ENABLE_XFA_PNG
    case FXCODEC_IMAGE_PNG:
      return PngContinueDecode();
#endif  // PDF_ENABLE_XFA_PNG
#ifdef PDF_ENABLE_XFA_TIFF
    case FXCODEC_IMAGE_TIFF:
      return TiffContinueDecode();
#endif  // PDF_ENABLE_XFA_TIFF
    default:
      return FXCODEC_STATUS_ERROR;
  }
}

std::unique_ptr<CCodec_ProgressiveDecoder>
CCodec_ModuleMgr::CreateProgressiveDecoder() {
  return pdfium::MakeUnique<CCodec_ProgressiveDecoder>(this);
}
