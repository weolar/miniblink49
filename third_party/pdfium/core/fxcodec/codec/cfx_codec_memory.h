// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CORE_FXCODEC_CODEC_CFX_CODEC_MEMORY_H_
#define CORE_FXCODEC_CODEC_CFX_CODEC_MEMORY_H_

#include <memory>

#include "core/fxcrt/fx_memory.h"
#include "core/fxcrt/retain_ptr.h"
#include "third_party/base/span.h"

class CFX_CodecMemory final : public Retainable {
 public:
  template <typename T, typename... Args>
  friend RetainPtr<T> pdfium::MakeRetain(Args&&... args);

  pdfium::span<uint8_t> GetSpan() { return {buffer_.get(), size_}; }
  uint8_t* GetBuffer() { return buffer_.get(); }
  size_t GetSize() const { return size_; }
  size_t GetPosition() const { return pos_; }
  bool IsEOF() const { return pos_ >= size_; }
  size_t ReadBlock(void* buffer, size_t size);

  // Sets the cursor position to |pos| if possible.
  bool Seek(size_t pos);

  // Try to change the size of the buffer, keep the old one on failure.
  bool TryResize(size_t new_buffer_size);

  // Schlep the bytes down the buffer.
  void Consume(size_t consumed);

 private:
  explicit CFX_CodecMemory(size_t buffer_size);
  ~CFX_CodecMemory() override;

  std::unique_ptr<uint8_t, FxFreeDeleter> buffer_;
  size_t size_ = 0;
  size_t pos_ = 0;
};

#endif  // CORE_FXCODEC_CODEC_CFX_CODEC_MEMORY_H_
