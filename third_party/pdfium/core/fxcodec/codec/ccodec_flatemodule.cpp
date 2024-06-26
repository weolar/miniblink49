// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcodec/codec/ccodec_flatemodule.h"

#include <algorithm>
#include <limits>
#include <memory>
#include <utility>
#include <vector>

#include "core/fxcodec/codec/ccodec_scanlinedecoder.h"
#include "core/fxcodec/fx_codec.h"
#include "core/fxcrt/fx_extension.h"
#include "third_party/base/numerics/safe_conversions.h"
#include "third_party/base/ptr_util.h"
#include "third_party/base/span.h"

#if defined(USE_SYSTEM_ZLIB)
#include <zlib.h>
#else
#include "third_party/zlib/zlib.h"
#endif

extern "C" {

static void* my_alloc_func(void* opaque,
                           unsigned int items,
                           unsigned int size) {
  return FX_Alloc2D(uint8_t, items, size);
}

static void my_free_func(void* opaque, void* address) {
  FX_Free(address);
}

}  // extern "C"

namespace {

static constexpr uint32_t kMaxTotalOutSize = 1024 * 1024 * 1024;  // 1 GiB

uint32_t FlateGetPossiblyTruncatedTotalOut(z_stream* context) {
  return std::min(pdfium::base::saturated_cast<uint32_t>(context->total_out),
                  kMaxTotalOutSize);
}

uint32_t FlateGetPossiblyTruncatedTotalIn(z_stream* context) {
  return pdfium::base::saturated_cast<uint32_t>(context->total_in);
}

bool FlateCompress(unsigned char* dest_buf,
                   unsigned long* dest_size,
                   const unsigned char* src_buf,
                   uint32_t src_size) {
  return compress(dest_buf, dest_size, src_buf, src_size) == Z_OK;
}

z_stream* FlateInit() {
  z_stream* p = FX_Alloc(z_stream, 1);
  p->zalloc = my_alloc_func;
  p->zfree = my_free_func;
  inflateInit(p);
  return p;
}

void FlateInput(z_stream* context, pdfium::span<const uint8_t> src_buf) {
  context->next_in = const_cast<unsigned char*>(src_buf.data());
  context->avail_in = static_cast<uint32_t>(src_buf.size());
}

uint32_t FlateOutput(z_stream* context,
                     unsigned char* dest_buf,
                     uint32_t dest_size) {
  context->next_out = dest_buf;
  context->avail_out = dest_size;
  uint32_t pre_pos = FlateGetPossiblyTruncatedTotalOut(context);
  int ret = inflate(static_cast<z_stream*>(context), Z_SYNC_FLUSH);

  uint32_t post_pos = FlateGetPossiblyTruncatedTotalOut(context);
  ASSERT(post_pos >= pre_pos);

  uint32_t written = post_pos - pre_pos;
  if (written < dest_size)
    memset(dest_buf + written, '\0', dest_size - written);

  return ret;
}

uint32_t FlateGetAvailOut(z_stream* context) {
  return context->avail_out;
}

void FlateEnd(z_stream* context) {
  inflateEnd(context);
  FX_Free(context);
}

// For use with std::unique_ptr<z_stream>.
struct FlateDeleter {
  inline void operator()(z_stream* context) { FlateEnd(context); }
};

class CLZWDecoder {
 public:
  int Decode(uint8_t* output,
             uint32_t& outlen,
             const uint8_t* input,
             uint32_t& size,
             bool bEarlyChange);

 private:
  void AddCode(uint32_t prefix_code, uint8_t append_char);
  void DecodeString(uint32_t code);

  uint32_t m_InPos;
  uint32_t m_OutPos;
  uint8_t* m_pOutput;
  const uint8_t* m_pInput;
  bool m_Early;
  uint32_t m_nCodes;
  uint32_t m_StackLen;
  int m_CodeLen;
  uint32_t m_CodeArray[5021];
  uint8_t m_DecodeStack[4000];
};

void CLZWDecoder::AddCode(uint32_t prefix_code, uint8_t append_char) {
  if (m_nCodes + m_Early == 4094) {
    return;
  }
  m_CodeArray[m_nCodes++] = (prefix_code << 16) | append_char;
  if (m_nCodes + m_Early == 512 - 258) {
    m_CodeLen = 10;
  } else if (m_nCodes + m_Early == 1024 - 258) {
    m_CodeLen = 11;
  } else if (m_nCodes + m_Early == 2048 - 258) {
    m_CodeLen = 12;
  }
}

void CLZWDecoder::DecodeString(uint32_t code) {
  while (1) {
    int index = code - 258;
    if (index < 0 || index >= (int)m_nCodes) {
      break;
    }
    uint32_t data = m_CodeArray[index];
    if (m_StackLen >= sizeof(m_DecodeStack)) {
      return;
    }
    m_DecodeStack[m_StackLen++] = (uint8_t)data;
    code = data >> 16;
  }
  if (m_StackLen >= sizeof(m_DecodeStack)) {
    return;
  }
  m_DecodeStack[m_StackLen++] = (uint8_t)code;
}

int CLZWDecoder::Decode(uint8_t* dest_buf,
                        uint32_t& dest_size,
                        const uint8_t* src_buf,
                        uint32_t& src_size,
                        bool bEarlyChange) {
  m_CodeLen = 9;
  m_InPos = 0;
  m_OutPos = 0;
  m_pInput = src_buf;
  m_pOutput = dest_buf;
  m_Early = bEarlyChange ? 1 : 0;
  m_nCodes = 0;
  uint32_t old_code = 0xFFFFFFFF;
  uint8_t last_char = 0;
  while (1) {
    if (m_InPos + m_CodeLen > src_size * 8) {
      break;
    }
    int byte_pos = m_InPos / 8;
    int bit_pos = m_InPos % 8, bit_left = m_CodeLen;
    uint32_t code = 0;
    if (bit_pos) {
      bit_left -= 8 - bit_pos;
      code = (m_pInput[byte_pos++] & ((1 << (8 - bit_pos)) - 1)) << bit_left;
    }
    if (bit_left < 8) {
      code |= m_pInput[byte_pos] >> (8 - bit_left);
    } else {
      bit_left -= 8;
      code |= m_pInput[byte_pos++] << bit_left;
      if (bit_left) {
        code |= m_pInput[byte_pos] >> (8 - bit_left);
      }
    }
    m_InPos += m_CodeLen;
    if (code == 257)
      break;
    if (code < 256) {
      if (m_OutPos == dest_size) {
        return -5;
      }
      if (m_pOutput) {
        m_pOutput[m_OutPos] = (uint8_t)code;
      }
      m_OutPos++;
      last_char = (uint8_t)code;
      if (old_code != 0xFFFFFFFF)
        AddCode(old_code, last_char);
      old_code = code;
    } else if (code == 256) {
      m_CodeLen = 9;
      m_nCodes = 0;
      old_code = 0xFFFFFFFF;
    } else {
      // Else 257 or greater.
      if (old_code == 0xFFFFFFFF)
        return 2;

      m_StackLen = 0;
      if (code >= m_nCodes + 258) {
        if (m_StackLen < sizeof(m_DecodeStack)) {
          m_DecodeStack[m_StackLen++] = last_char;
        }
        DecodeString(old_code);
      } else {
        DecodeString(code);
      }
      if (m_OutPos + m_StackLen > dest_size) {
        return -5;
      }
      if (m_pOutput) {
        for (uint32_t i = 0; i < m_StackLen; i++) {
          m_pOutput[m_OutPos + i] = m_DecodeStack[m_StackLen - i - 1];
        }
      }
      m_OutPos += m_StackLen;
      last_char = m_DecodeStack[m_StackLen - 1];
      if (old_code < 256) {
        AddCode(old_code, last_char);
      } else if (old_code - 258 >= m_nCodes) {
        dest_size = m_OutPos;
        src_size = (m_InPos + 7) / 8;
        return 0;
      } else {
        AddCode(old_code, last_char);
      }
      old_code = code;
    }
  }
  dest_size = m_OutPos;
  src_size = (m_InPos + 7) / 8;
  return 0;
}

uint8_t PathPredictor(int a, int b, int c) {
  int p = a + b - c;
  int pa = abs(p - a);
  int pb = abs(p - b);
  int pc = abs(p - c);
  if (pa <= pb && pa <= pc)
    return (uint8_t)a;
  if (pb <= pc)
    return (uint8_t)b;
  return (uint8_t)c;
}

void PNG_PredictLine(uint8_t* pDestData,
                     const uint8_t* pSrcData,
                     const uint8_t* pLastLine,
                     int bpc,
                     int nColors,
                     int nPixels) {
  const uint32_t row_size = CalculatePitch8(bpc, nColors, nPixels).ValueOrDie();
  const uint32_t BytesPerPixel = (bpc * nColors + 7) / 8;
  uint8_t tag = pSrcData[0];
  if (tag == 0) {
    memmove(pDestData, pSrcData + 1, row_size);
    return;
  }
  for (uint32_t byte = 0; byte < row_size; ++byte) {
    uint8_t raw_byte = pSrcData[byte + 1];
    switch (tag) {
      case 1: {
        uint8_t left = 0;
        if (byte >= BytesPerPixel) {
          left = pDestData[byte - BytesPerPixel];
        }
        pDestData[byte] = raw_byte + left;
        break;
      }
      case 2: {
        uint8_t up = 0;
        if (pLastLine) {
          up = pLastLine[byte];
        }
        pDestData[byte] = raw_byte + up;
        break;
      }
      case 3: {
        uint8_t left = 0;
        if (byte >= BytesPerPixel) {
          left = pDestData[byte - BytesPerPixel];
        }
        uint8_t up = 0;
        if (pLastLine) {
          up = pLastLine[byte];
        }
        pDestData[byte] = raw_byte + (up + left) / 2;
        break;
      }
      case 4: {
        uint8_t left = 0;
        if (byte >= BytesPerPixel) {
          left = pDestData[byte - BytesPerPixel];
        }
        uint8_t up = 0;
        if (pLastLine) {
          up = pLastLine[byte];
        }
        uint8_t upper_left = 0;
        if (byte >= BytesPerPixel && pLastLine) {
          upper_left = pLastLine[byte - BytesPerPixel];
        }
        pDestData[byte] = raw_byte + PathPredictor(left, up, upper_left);
        break;
      }
      default:
        pDestData[byte] = raw_byte;
        break;
    }
  }
}

bool PNG_Predictor(int Colors,
                   int BitsPerComponent,
                   int Columns,
                   std::unique_ptr<uint8_t, FxFreeDeleter>* data_buf,
                   uint32_t* data_size) {
  // TODO(thestig): Look into using CalculatePitch8() here.
  const int BytesPerPixel = (Colors * BitsPerComponent + 7) / 8;
  const int row_size = (Colors * BitsPerComponent * Columns + 7) / 8;
  if (row_size <= 0)
    return false;
  const int row_count = (*data_size + row_size) / (row_size + 1);
  if (row_count <= 0)
    return false;
  const int last_row_size = *data_size % (row_size + 1);
  std::unique_ptr<uint8_t, FxFreeDeleter> dest_buf(
      FX_Alloc2D(uint8_t, row_size, row_count));
  uint32_t byte_cnt = 0;
  uint8_t* pSrcData = data_buf->get();
  uint8_t* pDestData = dest_buf.get();
  for (int row = 0; row < row_count; row++) {
    uint8_t tag = pSrcData[0];
    byte_cnt++;
    if (tag == 0) {
      int move_size = row_size;
      if ((row + 1) * (move_size + 1) > static_cast<int>(*data_size)) {
        move_size = last_row_size - 1;
      }
      memcpy(pDestData, pSrcData + 1, move_size);
      pSrcData += move_size + 1;
      pDestData += move_size;
      byte_cnt += move_size;
      continue;
    }
    for (int byte = 0; byte < row_size && byte_cnt < *data_size;
         ++byte, ++byte_cnt) {
      uint8_t raw_byte = pSrcData[byte + 1];
      switch (tag) {
        case 1: {
          uint8_t left = 0;
          if (byte >= BytesPerPixel) {
            left = pDestData[byte - BytesPerPixel];
          }
          pDestData[byte] = raw_byte + left;
          break;
        }
        case 2: {
          uint8_t up = 0;
          if (row) {
            up = pDestData[byte - row_size];
          }
          pDestData[byte] = raw_byte + up;
          break;
        }
        case 3: {
          uint8_t left = 0;
          if (byte >= BytesPerPixel) {
            left = pDestData[byte - BytesPerPixel];
          }
          uint8_t up = 0;
          if (row) {
            up = pDestData[byte - row_size];
          }
          pDestData[byte] = raw_byte + (up + left) / 2;
          break;
        }
        case 4: {
          uint8_t left = 0;
          if (byte >= BytesPerPixel) {
            left = pDestData[byte - BytesPerPixel];
          }
          uint8_t up = 0;
          if (row) {
            up = pDestData[byte - row_size];
          }
          uint8_t upper_left = 0;
          if (byte >= BytesPerPixel && row) {
            upper_left = pDestData[byte - row_size - BytesPerPixel];
          }
          pDestData[byte] = raw_byte + PathPredictor(left, up, upper_left);
          break;
        }
        default:
          pDestData[byte] = raw_byte;
          break;
      }
    }
    pSrcData += row_size + 1;
    pDestData += row_size;
  }
  *data_buf = std::move(dest_buf);
  *data_size = row_size * row_count -
               (last_row_size > 0 ? (row_size + 1 - last_row_size) : 0);
  return true;
}

void TIFF_PredictLine(uint8_t* dest_buf,
                      uint32_t row_size,
                      int BitsPerComponent,
                      int Colors,
                      int Columns) {
  if (BitsPerComponent == 1) {
    int row_bits = std::min(BitsPerComponent * Colors * Columns,
                            pdfium::base::checked_cast<int>(row_size * 8));
    int index_pre = 0;
    int col_pre = 0;
    for (int i = 1; i < row_bits; i++) {
      int col = i % 8;
      int index = i / 8;
      if (((dest_buf[index] >> (7 - col)) & 1) ^
          ((dest_buf[index_pre] >> (7 - col_pre)) & 1)) {
        dest_buf[index] |= 1 << (7 - col);
      } else {
        dest_buf[index] &= ~(1 << (7 - col));
      }
      index_pre = index;
      col_pre = col;
    }
    return;
  }
  int BytesPerPixel = BitsPerComponent * Colors / 8;
  if (BitsPerComponent == 16) {
    for (uint32_t i = BytesPerPixel; i + 1 < row_size; i += 2) {
      uint16_t pixel =
          (dest_buf[i - BytesPerPixel] << 8) | dest_buf[i - BytesPerPixel + 1];
      pixel += (dest_buf[i] << 8) | dest_buf[i + 1];
      dest_buf[i] = pixel >> 8;
      dest_buf[i + 1] = (uint8_t)pixel;
    }
  } else {
    for (uint32_t i = BytesPerPixel; i < row_size; i++) {
      dest_buf[i] += dest_buf[i - BytesPerPixel];
    }
  }
}

bool TIFF_Predictor(int Colors,
                    int BitsPerComponent,
                    int Columns,
                    std::unique_ptr<uint8_t, FxFreeDeleter>* data_buf,
                    uint32_t* data_size) {
  int row_size = (Colors * BitsPerComponent * Columns + 7) / 8;
  if (row_size == 0)
    return false;
  const int row_count = (*data_size + row_size - 1) / row_size;
  const int last_row_size = *data_size % row_size;
  for (int row = 0; row < row_count; row++) {
    uint8_t* scan_line = data_buf->get() + row * row_size;
    if ((row + 1) * row_size > static_cast<int>(*data_size)) {
      row_size = last_row_size;
    }
    TIFF_PredictLine(scan_line, row_size, BitsPerComponent, Colors, Columns);
  }
  return true;
}

void FlateUncompress(pdfium::span<const uint8_t> src_buf,
                     uint32_t orig_size,
                     std::unique_ptr<uint8_t, FxFreeDeleter>* dest_buf,
                     uint32_t* dest_size,
                     uint32_t* offset) {
  dest_buf->reset();
  *dest_size = 0;

  std::unique_ptr<z_stream, FlateDeleter> context(FlateInit());
  if (!context)
    return;

  FlateInput(context.get(), src_buf);

  const uint32_t kMaxInitialAllocSize = 10000000;
  uint32_t guess_size = orig_size ? orig_size : src_buf.size() * 2;
  guess_size = std::min(guess_size, kMaxInitialAllocSize);

  uint32_t buf_size = guess_size;
  uint32_t last_buf_size = buf_size;
  std::unique_ptr<uint8_t, FxFreeDeleter> guess_buf(
      FX_Alloc(uint8_t, guess_size + 1));
  guess_buf.get()[guess_size] = '\0';

  std::vector<std::unique_ptr<uint8_t, FxFreeDeleter>> result_tmp_bufs;
  {
    std::unique_ptr<uint8_t, FxFreeDeleter> cur_buf = std::move(guess_buf);
    while (1) {
      uint32_t ret = FlateOutput(context.get(), cur_buf.get(), buf_size);
      uint32_t avail_buf_size = FlateGetAvailOut(context.get());
      if (ret != Z_OK || avail_buf_size != 0) {
        last_buf_size = buf_size - avail_buf_size;
        result_tmp_bufs.push_back(std::move(cur_buf));
        break;
      }
      result_tmp_bufs.push_back(std::move(cur_buf));
      cur_buf.reset(FX_Alloc(uint8_t, buf_size + 1));
      cur_buf.get()[buf_size] = '\0';
    }
  }

  // The TotalOut size returned from the library may not be big enough to
  // handle the content the library returns. We can only handle items
  // up to 4GB in size.
  *dest_size = FlateGetPossiblyTruncatedTotalOut(context.get());
  *offset = FlateGetPossiblyTruncatedTotalIn(context.get());
  if (result_tmp_bufs.size() == 1) {
    *dest_buf = std::move(result_tmp_bufs[0]);
    return;
  }

  std::unique_ptr<uint8_t, FxFreeDeleter> result_buf(
      FX_Alloc(uint8_t, *dest_size));
  uint32_t result_pos = 0;
  uint32_t remaining = *dest_size;
  for (size_t i = 0; i < result_tmp_bufs.size(); i++) {
    std::unique_ptr<uint8_t, FxFreeDeleter> tmp_buf =
        std::move(result_tmp_bufs[i]);
    uint32_t tmp_buf_size = buf_size;
    if (i == result_tmp_bufs.size() - 1)
      tmp_buf_size = last_buf_size;

    uint32_t cp_size = std::min(tmp_buf_size, remaining);
    memcpy(result_buf.get() + result_pos, tmp_buf.get(), cp_size);
    result_pos += cp_size;
    remaining -= cp_size;
  }
  *dest_buf = std::move(result_buf);
}

enum class PredictorType : uint8_t { kNone, kFlate, kPng };
static PredictorType GetPredictor(int predictor) {
  if (predictor >= 10)
    return PredictorType::kPng;
  if (predictor == 2)
    return PredictorType::kFlate;
  return PredictorType::kNone;
}

class CCodec_FlateScanlineDecoder : public CCodec_ScanlineDecoder {
 public:
  CCodec_FlateScanlineDecoder(pdfium::span<const uint8_t> src_buf,
                              int width,
                              int height,
                              int nComps,
                              int bpc);
  ~CCodec_FlateScanlineDecoder() override;

  // CCodec_ScanlineDecoder:
  bool v_Rewind() override;
  uint8_t* v_GetNextLine() override;
  uint32_t GetSrcOffset() override;

 protected:
  std::unique_ptr<z_stream, FlateDeleter> m_pFlate;
  pdfium::span<const uint8_t> const m_SrcBuf;
  std::unique_ptr<uint8_t, FxFreeDeleter> const m_pScanline;
};

CCodec_FlateScanlineDecoder::CCodec_FlateScanlineDecoder(
    pdfium::span<const uint8_t> src_span,
    int width,
    int height,
    int nComps,
    int bpc)
    : CCodec_ScanlineDecoder(width,
                             height,
                             width,
                             height,
                             nComps,
                             bpc,
                             CalculatePitch8(bpc, nComps, width).ValueOrDie()),
      m_SrcBuf(src_span),
      m_pScanline(FX_Alloc(uint8_t, m_Pitch)) {}

CCodec_FlateScanlineDecoder::~CCodec_FlateScanlineDecoder() = default;

bool CCodec_FlateScanlineDecoder::v_Rewind() {
  m_pFlate.reset(FlateInit());
  if (!m_pFlate)
    return false;

  FlateInput(m_pFlate.get(), m_SrcBuf);
  return true;
}

uint8_t* CCodec_FlateScanlineDecoder::v_GetNextLine() {
  FlateOutput(m_pFlate.get(), m_pScanline.get(), m_Pitch);
  return m_pScanline.get();
}

uint32_t CCodec_FlateScanlineDecoder::GetSrcOffset() {
  return FlateGetPossiblyTruncatedTotalIn(m_pFlate.get());
}

class CCodec_FlatePredictorScanlineDecoder final
    : public CCodec_FlateScanlineDecoder {
 public:
  CCodec_FlatePredictorScanlineDecoder(pdfium::span<const uint8_t> src_buf,
                                       int width,
                                       int height,
                                       int comps,
                                       int bpc,
                                       PredictorType predictor,
                                       int Colors,
                                       int BitsPerComponent,
                                       int Columns);
  ~CCodec_FlatePredictorScanlineDecoder() override;

  // CCodec_ScanlineDecoder:
  bool v_Rewind() override;
  uint8_t* v_GetNextLine() override;

 private:
  void GetNextLineWithPredictedPitch();
  void GetNextLineWithoutPredictedPitch();

  const PredictorType m_Predictor;
  int m_Colors = 0;
  int m_BitsPerComponent = 0;
  int m_Columns = 0;
  uint32_t m_PredictPitch = 0;
  size_t m_LeftOver = 0;
  std::vector<uint8_t> m_LastLine;
  std::vector<uint8_t> m_PredictBuffer;
  std::vector<uint8_t> m_PredictRaw;
};

CCodec_FlatePredictorScanlineDecoder::CCodec_FlatePredictorScanlineDecoder(
    pdfium::span<const uint8_t> src_span,
    int width,
    int height,
    int comps,
    int bpc,
    PredictorType predictor,
    int Colors,
    int BitsPerComponent,
    int Columns)
    : CCodec_FlateScanlineDecoder(src_span, width, height, comps, bpc),
      m_Predictor(predictor) {
  ASSERT(m_Predictor != PredictorType::kNone);
  if (BitsPerComponent * Colors * Columns == 0) {
    BitsPerComponent = m_bpc;
    Colors = m_nComps;
    Columns = m_OrigWidth;
  }
  m_Colors = Colors;
  m_BitsPerComponent = BitsPerComponent;
  m_Columns = Columns;
  m_PredictPitch =
      CalculatePitch8(m_BitsPerComponent, m_Colors, m_Columns).ValueOrDie();
  m_LastLine.resize(m_PredictPitch);
  m_PredictBuffer.resize(m_PredictPitch);
  m_PredictRaw.resize(m_PredictPitch + 1);
}

CCodec_FlatePredictorScanlineDecoder::~CCodec_FlatePredictorScanlineDecoder() =
    default;

bool CCodec_FlatePredictorScanlineDecoder::v_Rewind() {
  if (!CCodec_FlateScanlineDecoder::v_Rewind())
    return false;

  m_LeftOver = 0;
  return true;
}

uint8_t* CCodec_FlatePredictorScanlineDecoder::v_GetNextLine() {
  if (m_Pitch == m_PredictPitch)
    GetNextLineWithPredictedPitch();
  else
    GetNextLineWithoutPredictedPitch();
  return m_pScanline.get();
}

void CCodec_FlatePredictorScanlineDecoder::GetNextLineWithPredictedPitch() {
  switch (m_Predictor) {
    case PredictorType::kPng:
      FlateOutput(m_pFlate.get(), m_PredictRaw.data(), m_PredictPitch + 1);
      PNG_PredictLine(m_pScanline.get(), m_PredictRaw.data(), m_LastLine.data(),
                      m_BitsPerComponent, m_Colors, m_Columns);
      memcpy(m_LastLine.data(), m_pScanline.get(), m_PredictPitch);
      break;
    case PredictorType::kFlate:
      FlateOutput(m_pFlate.get(), m_pScanline.get(), m_Pitch);
      TIFF_PredictLine(m_pScanline.get(), m_PredictPitch, m_bpc, m_nComps,
                       m_OutputWidth);
      break;
    default:
      NOTREACHED();
      break;
  }
}

void CCodec_FlatePredictorScanlineDecoder::GetNextLineWithoutPredictedPitch() {
  size_t bytes_to_go = m_Pitch;
  size_t read_leftover = m_LeftOver > bytes_to_go ? bytes_to_go : m_LeftOver;
  if (read_leftover) {
    memcpy(m_pScanline.get(), &m_PredictBuffer[m_PredictPitch - m_LeftOver],
           read_leftover);
    m_LeftOver -= read_leftover;
    bytes_to_go -= read_leftover;
  }
  while (bytes_to_go) {
    switch (m_Predictor) {
      case PredictorType::kPng:
        FlateOutput(m_pFlate.get(), m_PredictRaw.data(), m_PredictPitch + 1);
        PNG_PredictLine(m_PredictBuffer.data(), m_PredictRaw.data(),
                        m_LastLine.data(), m_BitsPerComponent, m_Colors,
                        m_Columns);
        memcpy(m_LastLine.data(), m_PredictBuffer.data(), m_PredictPitch);
        break;
      case PredictorType::kFlate:
        FlateOutput(m_pFlate.get(), m_PredictBuffer.data(), m_PredictPitch);
        TIFF_PredictLine(m_PredictBuffer.data(), m_PredictPitch,
                         m_BitsPerComponent, m_Colors, m_Columns);
        break;
      default:
        NOTREACHED();
        break;
    }
    size_t read_bytes =
        m_PredictPitch > bytes_to_go ? bytes_to_go : m_PredictPitch;
    memcpy(m_pScanline.get() + m_Pitch - bytes_to_go, m_PredictBuffer.data(),
           read_bytes);
    m_LeftOver += m_PredictPitch - read_bytes;
    bytes_to_go -= read_bytes;
  }
}

}  // namespace

std::unique_ptr<CCodec_ScanlineDecoder> CCodec_FlateModule::CreateDecoder(
    pdfium::span<const uint8_t> src_span,
    int width,
    int height,
    int nComps,
    int bpc,
    int predictor,
    int Colors,
    int BitsPerComponent,
    int Columns) {
  PredictorType predictor_type = GetPredictor(predictor);
  if (predictor_type == PredictorType::kNone) {
    return pdfium::MakeUnique<CCodec_FlateScanlineDecoder>(src_span, width,
                                                           height, nComps, bpc);
  }
  return pdfium::MakeUnique<CCodec_FlatePredictorScanlineDecoder>(
      src_span, width, height, nComps, bpc, predictor_type, Colors,
      BitsPerComponent, Columns);
}

uint32_t CCodec_FlateModule::FlateOrLZWDecode(
    bool bLZW,
    pdfium::span<const uint8_t> src_span,
    bool bEarlyChange,
    int predictor,
    int Colors,
    int BitsPerComponent,
    int Columns,
    uint32_t estimated_size,
    std::unique_ptr<uint8_t, FxFreeDeleter>* dest_buf,
    uint32_t* dest_size) {
  dest_buf->reset();
  uint32_t offset = 0;
  PredictorType predictor_type = GetPredictor(predictor);

  if (bLZW) {
    auto decoder = pdfium::MakeUnique<CLZWDecoder>();
    *dest_size = 0xFFFFFFFF;
    offset = src_span.size();
    int err = decoder->Decode(nullptr, *dest_size, src_span.data(), offset,
                              bEarlyChange);
    if (err || *dest_size == 0 || *dest_size + 1 < *dest_size)
      return FX_INVALID_OFFSET;

    decoder = pdfium::MakeUnique<CLZWDecoder>();
    dest_buf->reset(FX_Alloc(uint8_t, *dest_size + 1));
    uint8_t* dest_buf_ptr = dest_buf->get();
    dest_buf_ptr[*dest_size] = '\0';
    decoder->Decode(dest_buf_ptr, *dest_size, src_span.data(), offset,
                    bEarlyChange);
  } else {
    FlateUncompress(src_span, estimated_size, dest_buf, dest_size, &offset);
  }

  bool ret = false;
  switch (predictor_type) {
    case PredictorType::kNone:
      return offset;
    case PredictorType::kPng:
      ret =
          PNG_Predictor(Colors, BitsPerComponent, Columns, dest_buf, dest_size);
      break;
    case PredictorType::kFlate:
      ret = TIFF_Predictor(Colors, BitsPerComponent, Columns, dest_buf,
                           dest_size);
      break;
    default:
      NOTREACHED();
      break;
  }
  return ret ? offset : FX_INVALID_OFFSET;
}

bool CCodec_FlateModule::Encode(
    const uint8_t* src_buf,
    uint32_t src_size,
    std::unique_ptr<uint8_t, FxFreeDeleter>* dest_buf,
    uint32_t* dest_size) {
  *dest_size = src_size + src_size / 1000 + 12;
  dest_buf->reset(FX_Alloc(uint8_t, *dest_size));
  unsigned long temp_size = *dest_size;
  if (!FlateCompress(dest_buf->get(), &temp_size, src_buf, src_size))
    return false;

  *dest_size = (uint32_t)temp_size;
  return true;
}
