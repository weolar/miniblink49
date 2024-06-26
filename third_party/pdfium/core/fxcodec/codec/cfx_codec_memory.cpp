// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/fxcodec/codec/cfx_codec_memory.h"

#include <algorithm>

CFX_CodecMemory::CFX_CodecMemory(size_t buffer_size)
    : buffer_(FX_Alloc(uint8_t, buffer_size)), size_(buffer_size) {}

CFX_CodecMemory::~CFX_CodecMemory() = default;

bool CFX_CodecMemory::Seek(size_t pos) {
  if (pos > size_)
    return false;

  pos_ = pos;
  return true;
}

size_t CFX_CodecMemory::ReadBlock(void* buffer, size_t size) {
  if (!buffer || !size || IsEOF())
    return 0;

  size_t bytes_to_read = std::min(size, size_ - pos_);
  memcpy(buffer, buffer_.get() + pos_, bytes_to_read);
  pos_ += bytes_to_read;
  return bytes_to_read;
}

bool CFX_CodecMemory::TryResize(size_t new_buffer_size) {
  uint8_t* pOldBuf = buffer_.release();
  uint8_t* pNewBuf = FX_TryRealloc(uint8_t, pOldBuf, new_buffer_size);
  if (new_buffer_size && !pNewBuf) {
    buffer_.reset(pOldBuf);
    return false;
  }
  buffer_.reset(pNewBuf);
  size_ = new_buffer_size;
  return true;
}

void CFX_CodecMemory::Consume(size_t consumed) {
  size_t unconsumed = size_ - consumed;
  memmove(buffer_.get(), buffer_.get() + consumed, unconsumed);
}
