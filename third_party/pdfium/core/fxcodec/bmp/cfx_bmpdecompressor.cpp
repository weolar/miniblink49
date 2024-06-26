// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcodec/bmp/cfx_bmpdecompressor.h"

#include <algorithm>
#include <limits>
#include <utility>

#include "core/fxcodec/bmp/cfx_bmpcontext.h"
#include "core/fxcodec/codec/cfx_codec_memory.h"
#include "core/fxcrt/fx_safe_types.h"
#include "core/fxcrt/fx_system.h"
#include "third_party/base/logging.h"
#include "third_party/base/numerics/safe_math.h"

namespace {

const size_t kBmpCoreHeaderSize = 12;
const size_t kBmpInfoHeaderSize = 40;

static_assert(sizeof(BmpCoreHeader) == kBmpCoreHeaderSize,
              "BmpCoreHeader has wrong size");
static_assert(sizeof(BmpInfoHeader) == kBmpInfoHeaderSize,
              "BmpInfoHeader has wrong size");

uint8_t HalfRoundUp(uint8_t value) {
  uint16_t value16 = value;
  return static_cast<uint8_t>((value16 + 1) / 2);
}

}  // namespace

CFX_BmpDecompressor::CFX_BmpDecompressor()
    : context_ptr_(nullptr),
      header_offset_(0),
      width_(0),
      height_(0),
      compress_flag_(0),
      components_(0),
      src_row_bytes_(0),
      out_row_bytes_(0),
      bit_counts_(0),
      color_used_(0),
      imgTB_flag_(false),
      pal_num_(0),
      pal_type_(0),
      data_size_(0),
      img_data_offset_(0),
      img_ifh_size_(0),
      row_num_(0),
      col_num_(0),
      dpi_x_(0),
      dpi_y_(0),
      mask_red_(0),
      mask_green_(0),
      mask_blue_(0),
      decode_status_(BMP_D_STATUS_HEADER) {}

CFX_BmpDecompressor::~CFX_BmpDecompressor() {}

void CFX_BmpDecompressor::Error() {
  longjmp(jmpbuf_, 1);
}

void CFX_BmpDecompressor::ReadScanline(uint32_t row_num,
                                       const std::vector<uint8_t>& row_buf) {
  context_ptr_->m_pDelegate->BmpReadScanline(row_num, row_buf);
}

bool CFX_BmpDecompressor::GetDataPosition(uint32_t rcd_pos) {
  return context_ptr_->m_pDelegate->BmpInputImagePositionBuf(rcd_pos);
}

int32_t CFX_BmpDecompressor::ReadHeader() {
  if (decode_status_ == BMP_D_STATUS_HEADER) {
    BmpFileHeader bmp_header;
    if (!ReadData(reinterpret_cast<uint8_t*>(&bmp_header),
                  sizeof(BmpFileHeader))) {
      return 2;
    }

    bmp_header.bfType =
        FXWORD_GET_LSBFIRST(reinterpret_cast<uint8_t*>(&bmp_header.bfType));
    bmp_header.bfOffBits =
        FXDWORD_GET_LSBFIRST(reinterpret_cast<uint8_t*>(&bmp_header.bfOffBits));
    data_size_ =
        FXDWORD_GET_LSBFIRST(reinterpret_cast<uint8_t*>(&bmp_header.bfSize));
    if (bmp_header.bfType != BMP_SIGNATURE) {
      Error();
      NOTREACHED();
    }

    if (!ReadData(reinterpret_cast<uint8_t*>(&img_ifh_size_),
                  sizeof(img_ifh_size_))) {
      return 2;
    }

    img_ifh_size_ =
        FXDWORD_GET_LSBFIRST(reinterpret_cast<uint8_t*>(&img_ifh_size_));
    pal_type_ = 0;
    switch (img_ifh_size_) {
      case kBmpCoreHeaderSize: {
        pal_type_ = 1;
        BmpCoreHeader bmp_core_header;
        if (!ReadData(reinterpret_cast<uint8_t*>(&bmp_core_header),
                      sizeof(BmpCoreHeader))) {
          return 2;
        }

        width_ = FXWORD_GET_LSBFIRST(
            reinterpret_cast<uint8_t*>(&bmp_core_header.bcWidth));
        height_ = FXWORD_GET_LSBFIRST(
            reinterpret_cast<uint8_t*>(&bmp_core_header.bcHeight));
        bit_counts_ = FXWORD_GET_LSBFIRST(
            reinterpret_cast<uint8_t*>(&bmp_core_header.bcBitCount));
        compress_flag_ = BMP_RGB;
        imgTB_flag_ = false;
      } break;
      case kBmpInfoHeaderSize: {
        BmpInfoHeader bmp_info_header;
        if (!ReadData(reinterpret_cast<uint8_t*>(&bmp_info_header),
                      sizeof(BmpInfoHeader))) {
          return 2;
        }

        width_ = FXDWORD_GET_LSBFIRST(
            reinterpret_cast<uint8_t*>(&bmp_info_header.biWidth));
        int32_t signed_height = FXDWORD_GET_LSBFIRST(
            reinterpret_cast<uint8_t*>(&bmp_info_header.biHeight));
        bit_counts_ = FXWORD_GET_LSBFIRST(
            reinterpret_cast<uint8_t*>(&bmp_info_header.biBitCount));
        compress_flag_ = FXDWORD_GET_LSBFIRST(
            reinterpret_cast<uint8_t*>(&bmp_info_header.biCompression));
        color_used_ = FXDWORD_GET_LSBFIRST(
            reinterpret_cast<uint8_t*>(&bmp_info_header.biClrUsed));
        dpi_x_ = static_cast<int32_t>(FXDWORD_GET_LSBFIRST(
            reinterpret_cast<uint8_t*>(&bmp_info_header.biXPelsPerMeter)));
        dpi_y_ = static_cast<int32_t>(FXDWORD_GET_LSBFIRST(
            reinterpret_cast<uint8_t*>(&bmp_info_header.biYPelsPerMeter)));
        SetHeight(signed_height);
      } break;
      default: {
        if (img_ifh_size_ <= sizeof(BmpInfoHeader)) {
          Error();
          NOTREACHED();
        }

        FX_SAFE_SIZE_T new_pos = input_buffer_->GetPosition();
        BmpInfoHeader bmp_info_header;
        if (!ReadData(reinterpret_cast<uint8_t*>(&bmp_info_header),
                      sizeof(bmp_info_header))) {
          return 2;
        }

        new_pos += img_ifh_size_;
        if (!new_pos.IsValid()) {
          Error();
          NOTREACHED();
        }

        if (!input_buffer_->Seek(new_pos.ValueOrDie()))
          return 2;

        uint16_t biPlanes;
        width_ = FXDWORD_GET_LSBFIRST(
            reinterpret_cast<uint8_t*>(&bmp_info_header.biWidth));
        int32_t signed_height = FXDWORD_GET_LSBFIRST(
            reinterpret_cast<uint8_t*>(&bmp_info_header.biHeight));
        bit_counts_ = FXWORD_GET_LSBFIRST(
            reinterpret_cast<uint8_t*>(&bmp_info_header.biBitCount));
        compress_flag_ = FXDWORD_GET_LSBFIRST(
            reinterpret_cast<uint8_t*>(&bmp_info_header.biCompression));
        color_used_ = FXDWORD_GET_LSBFIRST(
            reinterpret_cast<uint8_t*>(&bmp_info_header.biClrUsed));
        biPlanes = FXWORD_GET_LSBFIRST(
            reinterpret_cast<uint8_t*>(&bmp_info_header.biPlanes));
        dpi_x_ = FXDWORD_GET_LSBFIRST(
            reinterpret_cast<uint8_t*>(&bmp_info_header.biXPelsPerMeter));
        dpi_y_ = FXDWORD_GET_LSBFIRST(
            reinterpret_cast<uint8_t*>(&bmp_info_header.biYPelsPerMeter));
        SetHeight(signed_height);
        if (compress_flag_ != BMP_RGB || biPlanes != 1 || color_used_ != 0) {
          Error();
          NOTREACHED();
        }
      }
    }

    if (width_ > BMP_MAX_WIDTH || compress_flag_ > BMP_BITFIELDS) {
      Error();
      NOTREACHED();
    }

    switch (bit_counts_) {
      case 1:
      case 4:
      case 8:
      case 16:
      case 24: {
        if (color_used_ > 1U << bit_counts_) {
          Error();
          NOTREACHED();
        }
        break;
      }
      case 32:
        break;
      default:
        Error();
        NOTREACHED();
        break;
    }
    src_row_bytes_ = BMP_WIDTHBYTES(width_, bit_counts_);
    switch (bit_counts_) {
      case 1:
      case 4:
      case 8:
        out_row_bytes_ = BMP_WIDTHBYTES(width_, 8);
        components_ = 1;
        break;
      case 16:
      case 24:
        out_row_bytes_ = BMP_WIDTHBYTES(width_, 24);
        components_ = 3;
        break;
      case 32:
        out_row_bytes_ = src_row_bytes_;
        components_ = 4;
        break;
    }
    out_row_buffer_.clear();

    if (out_row_bytes_ <= 0) {
      Error();
      NOTREACHED();
    }

    out_row_buffer_.resize(out_row_bytes_);
    SaveDecodingStatus(BMP_D_STATUS_PAL);
  }
  if (decode_status_ == BMP_D_STATUS_PAL) {
    if (compress_flag_ == BMP_BITFIELDS) {
      if (bit_counts_ != 16 && bit_counts_ != 32) {
        Error();
        NOTREACHED();
      }

      uint32_t masks[3];
      if (!ReadData(reinterpret_cast<uint8_t*>(masks), sizeof(masks)))
        return 2;

      mask_red_ = FXDWORD_GET_LSBFIRST(reinterpret_cast<uint8_t*>(&masks[0]));
      mask_green_ = FXDWORD_GET_LSBFIRST(reinterpret_cast<uint8_t*>(&masks[1]));
      mask_blue_ = FXDWORD_GET_LSBFIRST(reinterpret_cast<uint8_t*>(&masks[2]));
      if (mask_red_ & mask_green_ || mask_red_ & mask_blue_ ||
          mask_green_ & mask_blue_) {
        Error();
        NOTREACHED();
      }
      header_offset_ = std::max(header_offset_, 26 + img_ifh_size_);
      SaveDecodingStatus(BMP_D_STATUS_DATA_PRE);
      return 1;
    } else if (bit_counts_ == 16) {
      mask_red_ = 0x7C00;
      mask_green_ = 0x03E0;
      mask_blue_ = 0x001F;
    }
    pal_num_ = 0;
    if (bit_counts_ < 16) {
      pal_num_ = 1 << bit_counts_;
      if (color_used_ != 0)
        pal_num_ = color_used_;
      uint32_t src_pal_size = pal_num_ * (pal_type_ ? 3 : 4);
      std::vector<uint8_t> src_pal(src_pal_size);
      uint8_t* src_pal_data = src_pal.data();
      if (!ReadData(src_pal_data, src_pal_size)) {
        return 2;
      }

      palette_.resize(pal_num_);
      int32_t src_pal_index = 0;
      if (pal_type_ == BMP_PAL_OLD) {
        while (src_pal_index < pal_num_) {
          palette_[src_pal_index++] = BMP_PAL_ENCODE(
              0x00, src_pal_data[2], src_pal_data[1], src_pal_data[0]);
          src_pal_data += 3;
        }
      } else {
        while (src_pal_index < pal_num_) {
          palette_[src_pal_index++] =
              BMP_PAL_ENCODE(src_pal_data[3], src_pal_data[2], src_pal_data[1],
                             src_pal_data[0]);
          src_pal_data += 4;
        }
      }
    }
    header_offset_ = std::max(
        header_offset_, 14 + img_ifh_size_ + pal_num_ * (pal_type_ ? 3 : 4));
    SaveDecodingStatus(BMP_D_STATUS_DATA_PRE);
  }
  return 1;
}

bool CFX_BmpDecompressor::ValidateFlag() const {
  switch (compress_flag_) {
    case BMP_RGB:
    case BMP_BITFIELDS:
    case BMP_RLE8:
    case BMP_RLE4:
      return true;
    default:
      return false;
  }
}

int32_t CFX_BmpDecompressor::DecodeImage() {
  if (decode_status_ == BMP_D_STATUS_DATA_PRE) {
    input_buffer_->Seek(0);
    if (!GetDataPosition(header_offset_)) {
      decode_status_ = BMP_D_STATUS_TAIL;
      Error();
      NOTREACHED();
    }

    row_num_ = 0;
    SaveDecodingStatus(BMP_D_STATUS_DATA);
  }
  if (decode_status_ != BMP_D_STATUS_DATA || !ValidateFlag()) {
    Error();
    NOTREACHED();
  }

  switch (compress_flag_) {
    case BMP_RGB:
    case BMP_BITFIELDS:
      return DecodeRGB();
    case BMP_RLE8:
      return DecodeRLE8();
    case BMP_RLE4:
      return DecodeRLE4();
    default:
      return 0;
  }
}

bool CFX_BmpDecompressor::ValidateColorIndex(uint8_t val) {
  if (val >= pal_num_) {
    Error();
    NOTREACHED();
  }

  return true;
}

int32_t CFX_BmpDecompressor::DecodeRGB() {
  std::vector<uint8_t> dest_buf(src_row_bytes_);
  while (row_num_ < height_) {
    size_t idx = 0;
    if (!ReadData(dest_buf.data(), src_row_bytes_))
      return 2;

    SaveDecodingStatus(BMP_D_STATUS_DATA);
    switch (bit_counts_) {
      case 1: {
        for (uint32_t col = 0; col < width_; ++col)
          out_row_buffer_[idx++] =
              dest_buf[col >> 3] & (0x80 >> (col % 8)) ? 0x01 : 0x00;
      } break;
      case 4: {
        for (uint32_t col = 0; col < width_; ++col) {
          out_row_buffer_[idx++] = (col & 0x01)
                                       ? (dest_buf[col >> 1] & 0x0F)
                                       : ((dest_buf[col >> 1] & 0xF0) >> 4);
        }
      } break;
      case 16: {
        uint16_t* buf = reinterpret_cast<uint16_t*>(dest_buf.data());
        uint8_t blue_bits = 0;
        uint8_t green_bits = 0;
        uint8_t red_bits = 0;
        for (int32_t i = 0; i < 16; i++) {
          if ((mask_blue_ >> i) & 0x01)
            blue_bits++;
          if ((mask_green_ >> i) & 0x01)
            green_bits++;
          if ((mask_red_ >> i) & 0x01)
            red_bits++;
        }
        green_bits += blue_bits;
        red_bits += green_bits;
        if (blue_bits > 8 || green_bits < 8 || red_bits < 8)
          return 2;
        blue_bits = 8 - blue_bits;
        green_bits -= 8;
        red_bits -= 8;
        for (uint32_t col = 0; col < width_; ++col) {
          *buf = FXWORD_GET_LSBFIRST(reinterpret_cast<uint8_t*>(buf));
          out_row_buffer_[idx++] =
              static_cast<uint8_t>((*buf & mask_blue_) << blue_bits);
          out_row_buffer_[idx++] =
              static_cast<uint8_t>((*buf & mask_green_) >> green_bits);
          out_row_buffer_[idx++] =
              static_cast<uint8_t>((*buf++ & mask_red_) >> red_bits);
        }
      } break;
      case 8:
      case 24:
      case 32:
        uint8_t* dest_buf_data = dest_buf.data();
        std::copy(dest_buf_data, dest_buf_data + src_row_bytes_,
                  out_row_buffer_.begin());
        idx += src_row_bytes_;
        break;
    }
    for (uint8_t byte : out_row_buffer_) {
      if (!ValidateColorIndex(byte))
        return 0;
    }
    ReadScanline(imgTB_flag_ ? row_num_++ : (height_ - 1 - row_num_++),
                 out_row_buffer_);
  }
  SaveDecodingStatus(BMP_D_STATUS_TAIL);
  return 1;
}

int32_t CFX_BmpDecompressor::DecodeRLE8() {
  uint8_t first_part;
  col_num_ = 0;
  while (true) {
    if (!ReadData(&first_part, sizeof(first_part)))
      return 2;

    switch (first_part) {
      case RLE_MARKER: {
        if (!ReadData(&first_part, sizeof(first_part)))
          return 2;

        switch (first_part) {
          case RLE_EOL: {
            if (row_num_ >= height_) {
              SaveDecodingStatus(BMP_D_STATUS_TAIL);
              Error();
              NOTREACHED();
            }

            ReadScanline(imgTB_flag_ ? row_num_++ : (height_ - 1 - row_num_++),
                         out_row_buffer_);
            col_num_ = 0;
            std::fill(out_row_buffer_.begin(), out_row_buffer_.end(), 0);
            SaveDecodingStatus(BMP_D_STATUS_DATA);
            continue;
          }
          case RLE_EOI: {
            if (row_num_ < height_) {
              ReadScanline(
                  imgTB_flag_ ? row_num_++ : (height_ - 1 - row_num_++),
                  out_row_buffer_);
            }
            SaveDecodingStatus(BMP_D_STATUS_TAIL);
            return 1;
          }
          case RLE_DELTA: {
            uint8_t delta[2];
            if (!ReadData(delta, sizeof(delta)))
              return 2;

            col_num_ += delta[0];
            size_t bmp_row_num__next = row_num_ + delta[1];
            if (col_num_ >= out_row_bytes_ || bmp_row_num__next >= height_) {
              Error();
              NOTREACHED();
            }

            while (row_num_ < bmp_row_num__next) {
              std::fill(out_row_buffer_.begin(), out_row_buffer_.end(), 0);
              ReadScanline(
                  imgTB_flag_ ? row_num_++ : (height_ - 1 - row_num_++),
                  out_row_buffer_);
            }
          } break;
          default: {
            int32_t avail_size = out_row_bytes_ - col_num_;
            if (!avail_size || static_cast<int32_t>(first_part) > avail_size) {
              Error();
              NOTREACHED();
            }

            size_t second_part_size =
                first_part & 1 ? first_part + 1 : first_part;
            std::vector<uint8_t> second_part(second_part_size);
            uint8_t* second_part_data = second_part.data();
            if (!ReadData(second_part_data, second_part_size))
              return 2;

            std::copy(second_part_data, second_part_data + first_part,
                      out_row_buffer_.begin() + col_num_);
            for (size_t i = col_num_; i < col_num_ + first_part; ++i) {
              if (!ValidateColorIndex(out_row_buffer_[i]))
                return 0;
            }
            col_num_ += first_part;
          }
        }
      } break;
      default: {
        int32_t avail_size = out_row_bytes_ - col_num_;
        if (!avail_size || static_cast<int32_t>(first_part) > avail_size) {
          Error();
          NOTREACHED();
        }

        uint8_t second_part;
        if (!ReadData(&second_part, sizeof(second_part)))
          return 2;

        std::fill(out_row_buffer_.begin() + col_num_,
                  out_row_buffer_.begin() + col_num_ + first_part, second_part);
        if (!ValidateColorIndex(out_row_buffer_[col_num_]))
          return 0;
        col_num_ += first_part;
      }
    }
  }
  Error();
  NOTREACHED();
}

int32_t CFX_BmpDecompressor::DecodeRLE4() {
  uint8_t first_part;
  col_num_ = 0;
  while (true) {
    if (!ReadData(&first_part, sizeof(first_part)))
      return 2;

    switch (first_part) {
      case RLE_MARKER: {
        if (!ReadData(&first_part, sizeof(first_part))) {
          return 2;
        }
        switch (first_part) {
          case RLE_EOL: {
            if (row_num_ >= height_) {
              SaveDecodingStatus(BMP_D_STATUS_TAIL);
              Error();
              NOTREACHED();
            }

            ReadScanline(imgTB_flag_ ? row_num_++ : (height_ - 1 - row_num_++),
                         out_row_buffer_);
            col_num_ = 0;
            std::fill(out_row_buffer_.begin(), out_row_buffer_.end(), 0);
            SaveDecodingStatus(BMP_D_STATUS_DATA);
            continue;
          }
          case RLE_EOI: {
            if (row_num_ < height_) {
              ReadScanline(
                  imgTB_flag_ ? row_num_++ : (height_ - 1 - row_num_++),
                  out_row_buffer_);
            }
            SaveDecodingStatus(BMP_D_STATUS_TAIL);
            return 1;
          }
          case RLE_DELTA: {
            uint8_t delta[2];
            if (!ReadData(delta, sizeof(delta)))
              return 2;

            col_num_ += delta[0];
            size_t bmp_row_num__next = row_num_ + delta[1];
            if (col_num_ >= out_row_bytes_ || bmp_row_num__next >= height_) {
              Error();
              NOTREACHED();
            }

            while (row_num_ < bmp_row_num__next) {
              std::fill(out_row_buffer_.begin(), out_row_buffer_.end(), 0);
              ReadScanline(
                  imgTB_flag_ ? row_num_++ : (height_ - 1 - row_num_++),
                  out_row_buffer_);
            }
          } break;
          default: {
            int32_t avail_size = out_row_bytes_ - col_num_;
            if (!avail_size) {
              Error();
              NOTREACHED();
            }
            uint8_t size = HalfRoundUp(first_part);
            if (static_cast<int32_t>(first_part) > avail_size) {
              if (size + (col_num_ >> 1) > src_row_bytes_) {
                Error();
                NOTREACHED();
              }

              first_part = avail_size - 1;
            }
            size_t second_part_size = size & 1 ? size + 1 : size;
            std::vector<uint8_t> second_part(second_part_size);
            uint8_t* second_part_data = second_part.data();
            if (!ReadData(second_part_data, second_part_size))
              return 2;

            for (uint8_t i = 0; i < first_part; i++) {
              uint8_t color = (i & 0x01) ? (*second_part_data++ & 0x0F)
                                         : (*second_part_data & 0xF0) >> 4;
              if (!ValidateColorIndex(color))
                return 0;

              out_row_buffer_[col_num_++] = color;
            }
          }
        }
      } break;
      default: {
        int32_t avail_size = out_row_bytes_ - col_num_;
        if (!avail_size) {
          Error();
          NOTREACHED();
        }

        if (static_cast<int32_t>(first_part) > avail_size) {
          uint8_t size = HalfRoundUp(first_part);
          if (size + (col_num_ >> 1) > src_row_bytes_) {
            Error();
            NOTREACHED();
          }

          first_part = avail_size - 1;
        }
        uint8_t second_part;
        if (!ReadData(&second_part, sizeof(second_part)))
          return 2;

        for (uint8_t i = 0; i < first_part; i++) {
          uint8_t second_byte = second_part;
          second_byte =
              i & 0x01 ? (second_byte & 0x0F) : (second_byte & 0xF0) >> 4;
          if (!ValidateColorIndex(second_byte))
            return 0;

          out_row_buffer_[col_num_++] = second_byte;
        }
      }
    }
  }
  Error();
  NOTREACHED();
}

bool CFX_BmpDecompressor::ReadData(uint8_t* destination, uint32_t size) {
  return input_buffer_ && input_buffer_->ReadBlock(destination, size) == size;
}

void CFX_BmpDecompressor::SaveDecodingStatus(int32_t status) {
  decode_status_ = status;
}

void CFX_BmpDecompressor::SetInputBuffer(
    RetainPtr<CFX_CodecMemory> codec_memory) {
  input_buffer_ = std::move(codec_memory);
}

FX_FILESIZE CFX_BmpDecompressor::GetAvailInput() const {
  if (!input_buffer_)
    return 0;

  return input_buffer_->GetSize() - input_buffer_->GetPosition();
}

void CFX_BmpDecompressor::SetHeight(int32_t signed_height) {
  if (signed_height >= 0) {
    height_ = signed_height;
    return;
  }
  if (signed_height == std::numeric_limits<int>::min()) {
    Error();
    NOTREACHED();
  }
  height_ = -signed_height;
  imgTB_flag_ = true;
}
