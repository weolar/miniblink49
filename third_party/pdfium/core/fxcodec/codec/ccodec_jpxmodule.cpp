// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcodec/codec/ccodec_jpxmodule.h"

#include <algorithm>
#include <limits>
#include <memory>
#include <utility>
#include <vector>

#include "core/fpdfapi/page/cpdf_colorspace.h"
#include "core/fxcodec/codec/cjpx_decoder.h"
#include "core/fxcrt/fx_safe_types.h"
#include "third_party/base/ptr_util.h"
#include "third_party/base/stl_util.h"
#include "third_party/libopenjpeg20/openjpeg.h"
#include "third_party/libopenjpeg20/opj_malloc.h"

namespace {

void fx_ignore_callback(const char* msg, void* client_data) {}

opj_stream_t* fx_opj_stream_create_memory_stream(DecodeData* data) {
  if (!data || !data->src_data || data->src_size <= 0)
    return nullptr;

  opj_stream_t* stream = opj_stream_create(OPJ_J2K_STREAM_CHUNK_SIZE,
                                           /*p_is_input=*/OPJ_TRUE);
  if (!stream)
    return nullptr;

  opj_stream_set_user_data(stream, data, nullptr);
  opj_stream_set_user_data_length(stream, data->src_size);
  opj_stream_set_read_function(stream, opj_read_from_memory);
  opj_stream_set_skip_function(stream, opj_skip_from_memory);
  opj_stream_set_seek_function(stream, opj_seek_from_memory);
  return stream;
}

bool alloc_rgb(int** out_r, int** out_g, int** out_b, size_t size) {
  int* r = static_cast<int*>(opj_image_data_alloc(size));
  if (!r)
    return false;

  int* g = static_cast<int*>(opj_image_data_alloc(size));
  if (!g) {
    opj_image_data_free(r);
    return false;
  }

  int* b = static_cast<int*>(opj_image_data_alloc(size));
  if (!b) {
    opj_image_data_free(r);
    opj_image_data_free(g);
    return false;
  }

  *out_r = r;
  *out_g = g;
  *out_b = b;
  return true;
}

void sycc_to_rgb(int offset,
                 int upb,
                 int y,
                 int cb,
                 int cr,
                 int* out_r,
                 int* out_g,
                 int* out_b) {
  cb -= offset;
  cr -= offset;
  *out_r = pdfium::clamp(y + static_cast<int>(1.402 * cr), 0, upb);
  *out_g = pdfium::clamp(y - static_cast<int>(0.344 * cb + 0.714 * cr), 0, upb);
  *out_b = pdfium::clamp(y + static_cast<int>(1.772 * cb), 0, upb);
}

void sycc444_to_rgb(opj_image_t* img) {
  int prec = img->comps[0].prec;
  // If we shift 31 we're going to go negative, then things go bad.
  if (prec > 30)
    return;
  int offset = 1 << (prec - 1);
  int upb = (1 << prec) - 1;
  OPJ_UINT32 maxw =
      std::min({img->comps[0].w, img->comps[1].w, img->comps[2].w});
  OPJ_UINT32 maxh =
      std::min({img->comps[0].h, img->comps[1].h, img->comps[2].h});
  FX_SAFE_SIZE_T max_size = maxw;
  max_size *= maxh;
  max_size *= sizeof(int);
  if (!max_size.IsValid())
    return;

  const int* y = img->comps[0].data;
  const int* cb = img->comps[1].data;
  const int* cr = img->comps[2].data;
  if (!y || !cb || !cr)
    return;

  int* r;
  int* g;
  int* b;
  if (!alloc_rgb(&r, &g, &b, max_size.ValueOrDie()))
    return;

  int* d0 = r;
  int* d1 = g;
  int* d2 = b;
  max_size /= sizeof(int);
  for (size_t i = 0; i < max_size.ValueOrDie(); ++i) {
    sycc_to_rgb(offset, upb, *y++, *cb++, *cr++, r++, g++, b++);
  }
  opj_image_data_free(img->comps[0].data);
  opj_image_data_free(img->comps[1].data);
  opj_image_data_free(img->comps[2].data);
  img->comps[0].data = d0;
  img->comps[1].data = d1;
  img->comps[2].data = d2;
}

bool sycc420_422_size_is_valid(opj_image_t* img) {
  return img && img->comps[0].w != std::numeric_limits<OPJ_UINT32>::max() &&
         (img->comps[0].w + 1) / 2 == img->comps[1].w &&
         img->comps[1].w == img->comps[2].w &&
         img->comps[1].h == img->comps[2].h;
}

bool sycc420_size_is_valid(opj_image_t* img) {
  return sycc420_422_size_is_valid(img) &&
         img->comps[0].h != std::numeric_limits<OPJ_UINT32>::max() &&
         (img->comps[0].h + 1) / 2 == img->comps[1].h;
}

bool sycc422_size_is_valid(opj_image_t* img) {
  return sycc420_422_size_is_valid(img) && img->comps[0].h == img->comps[1].h;
}

void sycc422_to_rgb(opj_image_t* img) {
  if (!sycc422_size_is_valid(img))
    return;

  int prec = img->comps[0].prec;
  if (prec <= 0 || prec >= 32)
    return;

  int offset = 1 << (prec - 1);
  int upb = (1 << prec) - 1;
  OPJ_UINT32 maxw = img->comps[0].w;
  OPJ_UINT32 maxh = img->comps[0].h;
  FX_SAFE_SIZE_T max_size = maxw;
  max_size *= maxh;
  max_size *= sizeof(int);
  if (!max_size.IsValid())
    return;

  const int* y = img->comps[0].data;
  const int* cb = img->comps[1].data;
  const int* cr = img->comps[2].data;
  if (!y || !cb || !cr)
    return;

  int* r;
  int* g;
  int* b;
  if (!alloc_rgb(&r, &g, &b, max_size.ValueOrDie()))
    return;

  int* d0 = r;
  int* d1 = g;
  int* d2 = b;
  for (uint32_t i = 0; i < maxh; ++i) {
    OPJ_UINT32 j;
    for (j = 0; j < (maxw & ~static_cast<OPJ_UINT32>(1)); j += 2) {
      sycc_to_rgb(offset, upb, *y++, *cb, *cr, r++, g++, b++);
      sycc_to_rgb(offset, upb, *y++, *cb++, *cr++, r++, g++, b++);
    }
    if (j < maxw) {
      sycc_to_rgb(offset, upb, *y++, *cb++, *cr++, r++, g++, b++);
    }
  }
  opj_image_data_free(img->comps[0].data);
  opj_image_data_free(img->comps[1].data);
  opj_image_data_free(img->comps[2].data);
  img->comps[0].data = d0;
  img->comps[1].data = d1;
  img->comps[2].data = d2;
  img->comps[1].w = maxw;
  img->comps[1].h = maxh;
  img->comps[2].w = maxw;
  img->comps[2].h = maxh;
  img->comps[1].dx = img->comps[0].dx;
  img->comps[2].dx = img->comps[0].dx;
  img->comps[1].dy = img->comps[0].dy;
  img->comps[2].dy = img->comps[0].dy;
}

bool sycc420_must_extend_cbcr(OPJ_UINT32 y, OPJ_UINT32 cbcr) {
  return (y & 1) && (cbcr == y / 2);
}

bool is_sycc420(const opj_image_t* img) {
  return img->comps[0].dx == 1 && img->comps[0].dy == 1 &&
         img->comps[1].dx == 2 && img->comps[1].dy == 2 &&
         img->comps[2].dx == 2 && img->comps[2].dy == 2;
}

bool is_sycc422(const opj_image_t* img) {
  return img->comps[0].dx == 1 && img->comps[0].dy == 1 &&
         img->comps[1].dx == 2 && img->comps[1].dy == 1 &&
         img->comps[2].dx == 2 && img->comps[2].dy == 1;
}

bool is_sycc444(const opj_image_t* img) {
  return img->comps[0].dx == 1 && img->comps[0].dy == 1 &&
         img->comps[1].dx == 1 && img->comps[1].dy == 1 &&
         img->comps[2].dx == 1 && img->comps[2].dy == 1;
}

void color_sycc_to_rgb(opj_image_t* img) {
  if (img->numcomps < 3) {
    img->color_space = OPJ_CLRSPC_GRAY;
    return;
  }
  if (is_sycc420(img))
    sycc420_to_rgb(img);
  else if (is_sycc422(img))
    sycc422_to_rgb(img);
  else if (is_sycc444(img))
    sycc444_to_rgb(img);
  else
    return;

  img->color_space = OPJ_CLRSPC_SRGB;
}

}  // namespace

OPJ_SIZE_T opj_read_from_memory(void* p_buffer,
                                OPJ_SIZE_T nb_bytes,
                                void* p_user_data) {
  DecodeData* srcData = static_cast<DecodeData*>(p_user_data);
  if (!srcData || !srcData->src_data || srcData->src_size == 0)
    return static_cast<OPJ_SIZE_T>(-1);

  // Reads at EOF return an error code.
  if (srcData->offset >= srcData->src_size)
    return static_cast<OPJ_SIZE_T>(-1);

  OPJ_SIZE_T bufferLength = srcData->src_size - srcData->offset;
  OPJ_SIZE_T readlength = nb_bytes < bufferLength ? nb_bytes : bufferLength;
  memcpy(p_buffer, &srcData->src_data[srcData->offset], readlength);
  srcData->offset += readlength;
  return readlength;
}

OPJ_OFF_T opj_skip_from_memory(OPJ_OFF_T nb_bytes, void* p_user_data) {
  DecodeData* srcData = static_cast<DecodeData*>(p_user_data);
  if (!srcData || !srcData->src_data || srcData->src_size == 0)
    return static_cast<OPJ_OFF_T>(-1);

  // Offsets are signed and may indicate a negative skip. Do not support this
  // because of the strange return convention where either bytes skipped or
  // -1 is returned. Following that convention, a successful relative seek of
  // -1 bytes would be required to to give the same result as the error case.
  if (nb_bytes < 0)
    return static_cast<OPJ_OFF_T>(-1);

  // FIXME: use std::make_unsigned<OPJ_OFF_T>::type once c++11 lib is OK'd.
  uint64_t unsignedNbBytes = static_cast<uint64_t>(nb_bytes);
  // Additionally, the offset may take us beyond the range of a size_t (e.g.
  // 32-bit platforms). If so, just clamp at EOF.
  if (unsignedNbBytes >
      std::numeric_limits<OPJ_SIZE_T>::max() - srcData->offset) {
    srcData->offset = srcData->src_size;
  } else {
    OPJ_SIZE_T checkedNbBytes = static_cast<OPJ_SIZE_T>(unsignedNbBytes);
    // Otherwise, mimic fseek() semantics to always succeed, even past EOF,
    // clamping at EOF.  We can get away with this since we don't actually
    // provide negative relative skips from beyond EOF back to inside the
    // data, which would be the only reason to need to know exactly how far
    // beyond EOF we are.
    srcData->offset =
        std::min(srcData->offset + checkedNbBytes, srcData->src_size);
  }
  return nb_bytes;
}

OPJ_BOOL opj_seek_from_memory(OPJ_OFF_T nb_bytes, void* p_user_data) {
  DecodeData* srcData = static_cast<DecodeData*>(p_user_data);
  if (!srcData || !srcData->src_data || srcData->src_size == 0)
    return OPJ_FALSE;

  // Offsets are signed and may indicate a negative position, which would
  // be before the start of the file. Do not support this.
  if (nb_bytes < 0)
    return OPJ_FALSE;

  // FIXME: use std::make_unsigned<OPJ_OFF_T>::type once c++11 lib is OK'd.
  uint64_t unsignedNbBytes = static_cast<uint64_t>(nb_bytes);
  // Additionally, the offset may take us beyond the range of a size_t (e.g.
  // 32-bit platforms). If so, just clamp at EOF.
  if (unsignedNbBytes > std::numeric_limits<OPJ_SIZE_T>::max()) {
    srcData->offset = srcData->src_size;
  } else {
    OPJ_SIZE_T checkedNbBytes = static_cast<OPJ_SIZE_T>(nb_bytes);
    // Otherwise, mimic fseek() semantics to always succeed, even past EOF,
    // again clamping at EOF.
    srcData->offset = std::min(checkedNbBytes, srcData->src_size);
  }
  return OPJ_TRUE;
}

void sycc420_to_rgb(opj_image_t* img) {
  if (!sycc420_size_is_valid(img))
    return;

  OPJ_UINT32 prec = img->comps[0].prec;
  if (!prec)
    return;

  OPJ_UINT32 offset = 1 << (prec - 1);
  OPJ_UINT32 upb = (1 << prec) - 1;
  OPJ_UINT32 yw = img->comps[0].w;
  OPJ_UINT32 yh = img->comps[0].h;
  OPJ_UINT32 cbw = img->comps[1].w;
  OPJ_UINT32 cbh = img->comps[1].h;
  OPJ_UINT32 crw = img->comps[2].w;
  bool extw = sycc420_must_extend_cbcr(yw, cbw);
  bool exth = sycc420_must_extend_cbcr(yh, cbh);
  FX_SAFE_UINT32 safeSize = yw;
  safeSize *= yh;
  safeSize *= sizeof(int);
  if (!safeSize.IsValid())
    return;

  int* r;
  int* g;
  int* b;
  if (!alloc_rgb(&r, &g, &b, safeSize.ValueOrDie()))
    return;

  int* d0 = r;
  int* d1 = g;
  int* d2 = b;
  const int* y = img->comps[0].data;
  const int* cb = img->comps[1].data;
  const int* cr = img->comps[2].data;
  if (!y || !cb || !cr)
    return;

  const int* ny = nullptr;
  int* nr = nullptr;
  int* ng = nullptr;
  int* nb = nullptr;
  OPJ_UINT32 i = 0;
  OPJ_UINT32 j = 0;
  for (i = 0; i < (yh & ~(OPJ_UINT32)1); i += 2) {
    ny = y + yw;
    nr = r + yw;
    ng = g + yw;
    nb = b + yw;
    for (j = 0; j < (yw & ~(OPJ_UINT32)1); j += 2) {
      sycc_to_rgb(offset, upb, *y, *cb, *cr, r, g, b);
      ++y;
      ++r;
      ++g;
      ++b;
      sycc_to_rgb(offset, upb, *y, *cb, *cr, r, g, b);
      ++y;
      ++r;
      ++g;
      ++b;
      sycc_to_rgb(offset, upb, *ny, *cb, *cr, nr, ng, nb);
      ++ny;
      ++nr;
      ++ng;
      ++nb;
      sycc_to_rgb(offset, upb, *ny, *cb, *cr, nr, ng, nb);
      ++ny;
      ++nr;
      ++ng;
      ++nb;
      ++cb;
      ++cr;
    }
    if (j < yw) {
      if (extw) {
        --cb;
        --cr;
      }
      sycc_to_rgb(offset, upb, *y, *cb, *cr, r, g, b);
      ++y;
      ++r;
      ++g;
      ++b;
      sycc_to_rgb(offset, upb, *ny, *cb, *cr, nr, ng, nb);
      ++ny;
      ++nr;
      ++ng;
      ++nb;
      ++cb;
      ++cr;
    }
    y += yw;
    r += yw;
    g += yw;
    b += yw;
  }
  if (i < yh) {
    if (exth) {
      cb -= cbw;
      cr -= crw;
    }
    for (j = 0; j < (yw & ~(OPJ_UINT32)1); j += 2) {
      sycc_to_rgb(offset, upb, *y, *cb, *cr, r, g, b);
      ++y;
      ++r;
      ++g;
      ++b;
      sycc_to_rgb(offset, upb, *y, *cb, *cr, r, g, b);
      ++y;
      ++r;
      ++g;
      ++b;
      ++cb;
      ++cr;
    }
    if (j < yw) {
      if (extw) {
        --cb;
        --cr;
      }
      sycc_to_rgb(offset, upb, *y, *cb, *cr, r, g, b);
    }
  }

  opj_image_data_free(img->comps[0].data);
  opj_image_data_free(img->comps[1].data);
  opj_image_data_free(img->comps[2].data);
  img->comps[0].data = d0;
  img->comps[1].data = d1;
  img->comps[2].data = d2;
  img->comps[1].w = yw;
  img->comps[1].h = yh;
  img->comps[2].w = yw;
  img->comps[2].h = yh;
  img->comps[1].dx = img->comps[0].dx;
  img->comps[2].dx = img->comps[0].dx;
  img->comps[1].dy = img->comps[0].dy;
  img->comps[2].dy = img->comps[0].dy;
}

CJPX_Decoder::CJPX_Decoder(CPDF_ColorSpace* cs)
    : m_Image(nullptr),
      m_Codec(nullptr),
      m_DecodeData(nullptr),
      m_Stream(nullptr),
      m_ColorSpace(cs) {}

CJPX_Decoder::~CJPX_Decoder() {
  if (m_Codec)
    opj_destroy_codec(m_Codec.Release());
  if (m_Stream)
    opj_stream_destroy(m_Stream.Release());
  if (m_Image)
    opj_image_destroy(m_Image.Release());
}

bool CJPX_Decoder::Init(pdfium::span<const uint8_t> src_data) {
  static const unsigned char szJP2Header[] = {
      0x00, 0x00, 0x00, 0x0c, 0x6a, 0x50, 0x20, 0x20, 0x0d, 0x0a, 0x87, 0x0a};
  if (src_data.empty() || src_data.size() < sizeof(szJP2Header))
    return false;

  m_Image = nullptr;
  m_SrcData = src_data;
  m_DecodeData =
      pdfium::MakeUnique<DecodeData>(src_data.data(), src_data.size());
  m_Stream = fx_opj_stream_create_memory_stream(m_DecodeData.get());
  if (!m_Stream)
    return false;

  opj_set_default_decoder_parameters(&m_Parameters);
  m_Parameters.decod_format = 0;
  m_Parameters.cod_format = 3;
  if (memcmp(m_SrcData.data(), szJP2Header, sizeof(szJP2Header)) == 0) {
    m_Codec = opj_create_decompress(OPJ_CODEC_JP2);
    m_Parameters.decod_format = 1;
  } else {
    m_Codec = opj_create_decompress(OPJ_CODEC_J2K);
  }
  if (!m_Codec)
    return false;

  if (m_ColorSpace && m_ColorSpace->GetFamily() == PDFCS_INDEXED)
    m_Parameters.flags |= OPJ_DPARAMETERS_IGNORE_PCLR_CMAP_CDEF_FLAG;
  opj_set_info_handler(m_Codec.Get(), fx_ignore_callback, nullptr);
  opj_set_warning_handler(m_Codec.Get(), fx_ignore_callback, nullptr);
  opj_set_error_handler(m_Codec.Get(), fx_ignore_callback, nullptr);
  if (!opj_setup_decoder(m_Codec.Get(), &m_Parameters))
    return false;

  m_Image = nullptr;
  opj_image_t* pTempImage = nullptr;
  if (!opj_read_header(m_Stream.Get(), m_Codec.Get(), &pTempImage))
    return false;

  m_Image = pTempImage;
  m_Image->pdfium_use_colorspace = !!m_ColorSpace;
  return true;
}

bool CJPX_Decoder::StartDecode() {
  if (!m_Parameters.nb_tile_to_decode) {
    if (!opj_set_decode_area(m_Codec.Get(), m_Image.Get(), m_Parameters.DA_x0,
                             m_Parameters.DA_y0, m_Parameters.DA_x1,
                             m_Parameters.DA_y1)) {
      opj_image_destroy(m_Image.Release());
      return false;
    }
    if (!(opj_decode(m_Codec.Get(), m_Stream.Get(), m_Image.Get()) &&
          opj_end_decompress(m_Codec.Get(), m_Stream.Get()))) {
      opj_image_destroy(m_Image.Release());
      return false;
    }
  } else if (!opj_get_decoded_tile(m_Codec.Get(), m_Stream.Get(), m_Image.Get(),
                                   m_Parameters.tile_index)) {
    return false;
  }

  opj_stream_destroy(m_Stream.Release());
  if (m_Image->color_space != OPJ_CLRSPC_SYCC && m_Image->numcomps == 3 &&
      m_Image->comps[0].dx == m_Image->comps[0].dy &&
      m_Image->comps[1].dx != 1) {
    m_Image->color_space = OPJ_CLRSPC_SYCC;
  } else if (m_Image->numcomps <= 2) {
    m_Image->color_space = OPJ_CLRSPC_GRAY;
  }
  if (m_Image->color_space == OPJ_CLRSPC_SYCC)
    color_sycc_to_rgb(m_Image.Get());

  if (m_Image->icc_profile_buf) {
    // TODO(palmer): Using |opj_free| here resolves the crash described in
    // https://crbug.com/737033, but ultimately we need to harmonize the
    // memory allocation strategy across OpenJPEG and its PDFium callers.
    opj_free(m_Image->icc_profile_buf);
    m_Image->icc_profile_buf = nullptr;
    m_Image->icc_profile_len = 0;
  }
  return true;
}

void CJPX_Decoder::GetInfo(uint32_t* width,
                           uint32_t* height,
                           uint32_t* components) {
  *width = m_Image->x1;
  *height = m_Image->y1;
  *components = m_Image->numcomps;
}

bool CJPX_Decoder::Decode(uint8_t* dest_buf,
                          uint32_t pitch,
                          const std::vector<uint8_t>& offsets) {
  if (m_Image->comps[0].w != m_Image->x1 || m_Image->comps[0].h != m_Image->y1)
    return false;

  if (pitch<(m_Image->comps[0].w * 8 * m_Image->numcomps + 31)>> 5 << 2) {
    return false;
  }

  memset(dest_buf, 0xff, m_Image->y1 * pitch);
  std::vector<uint8_t*> channel_bufs(m_Image->numcomps);
  std::vector<int> adjust_comps(m_Image->numcomps);
  for (uint32_t i = 0; i < m_Image->numcomps; i++) {
    channel_bufs[i] = dest_buf + offsets[i];
    adjust_comps[i] = m_Image->comps[i].prec - 8;
    if (i > 0) {
      if (m_Image->comps[i].dx != m_Image->comps[i - 1].dx ||
          m_Image->comps[i].dy != m_Image->comps[i - 1].dy ||
          m_Image->comps[i].prec != m_Image->comps[i - 1].prec) {
        return false;
      }
    }
  }
  uint32_t width = m_Image->comps[0].w;
  uint32_t height = m_Image->comps[0].h;
  for (uint32_t channel = 0; channel < m_Image->numcomps; ++channel) {
    uint8_t* pChannel = channel_bufs[channel];
    if (adjust_comps[channel] < 0) {
      for (uint32_t row = 0; row < height; ++row) {
        uint8_t* pScanline = pChannel + row * pitch;
        for (uint32_t col = 0; col < width; ++col) {
          uint8_t* pPixel = pScanline + col * m_Image->numcomps;
          if (!m_Image->comps[channel].data)
            continue;

          int src = m_Image->comps[channel].data[row * width + col];
          src += m_Image->comps[channel].sgnd
                     ? 1 << (m_Image->comps[channel].prec - 1)
                     : 0;
          if (adjust_comps[channel] > 0) {
            *pPixel = 0;
          } else {
            *pPixel = static_cast<uint8_t>(src << -adjust_comps[channel]);
          }
        }
      }
    } else {
      for (uint32_t row = 0; row < height; ++row) {
        uint8_t* pScanline = pChannel + row * pitch;
        for (uint32_t col = 0; col < width; ++col) {
          uint8_t* pPixel = pScanline + col * m_Image->numcomps;
          if (!m_Image->comps[channel].data)
            continue;

          int src = m_Image->comps[channel].data[row * width + col];
          src += m_Image->comps[channel].sgnd
                     ? 1 << (m_Image->comps[channel].prec - 1)
                     : 0;
          if (adjust_comps[channel] - 1 < 0) {
            *pPixel = static_cast<uint8_t>((src >> adjust_comps[channel]));
          } else {
            int tmpPixel = (src >> adjust_comps[channel]) +
                           ((src >> (adjust_comps[channel] - 1)) % 2);
            tmpPixel = pdfium::clamp(tmpPixel, 0, 255);
            *pPixel = static_cast<uint8_t>(tmpPixel);
          }
        }
      }
    }
  }
  return true;
}

CCodec_JpxModule::CCodec_JpxModule() {}

CCodec_JpxModule::~CCodec_JpxModule() {}

std::unique_ptr<CJPX_Decoder> CCodec_JpxModule::CreateDecoder(
    pdfium::span<const uint8_t> src_span,
    CPDF_ColorSpace* cs) {
  auto decoder = pdfium::MakeUnique<CJPX_Decoder>(cs);
  if (!decoder->Init(src_span))
    return nullptr;

  return decoder;
}

void CCodec_JpxModule::GetImageInfo(CJPX_Decoder* pDecoder,
                                    uint32_t* width,
                                    uint32_t* height,
                                    uint32_t* components) {
  pDecoder->GetInfo(width, height, components);
}

bool CCodec_JpxModule::Decode(CJPX_Decoder* pDecoder,
                              uint8_t* dest_data,
                              uint32_t pitch,
                              const std::vector<uint8_t>& offsets) {
  return pDecoder->Decode(dest_data, pitch, offsets);
}
