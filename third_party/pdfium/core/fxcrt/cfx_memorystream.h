// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCRT_CFX_MEMORYSTREAM_H_
#define CORE_FXCRT_CFX_MEMORYSTREAM_H_

#include <memory>

#include "core/fxcrt/fx_memory.h"
#include "core/fxcrt/fx_stream.h"
#include "core/fxcrt/retain_ptr.h"

class CFX_MemoryStream final : public IFX_SeekableStream {
 public:
  template <typename T, typename... Args>
  friend RetainPtr<T> pdfium::MakeRetain(Args&&... args);

  // IFX_SeekableStream
  FX_FILESIZE GetSize() override;
  FX_FILESIZE GetPosition() override;
  bool IsEOF() override;
  bool ReadBlockAtOffset(void* buffer,
                         FX_FILESIZE offset,
                         size_t size) override;
  size_t ReadBlock(void* buffer, size_t size) override;
  bool WriteBlockAtOffset(const void* buffer,
                          FX_FILESIZE offset,
                          size_t size) override;
  bool Flush() override;

  const uint8_t* GetBuffer() const { return m_data.get(); }

 private:
  CFX_MemoryStream();
  CFX_MemoryStream(std::unique_ptr<uint8_t, FxFreeDeleter> pBuffer,
                   size_t nSize);
  ~CFX_MemoryStream() override;

  std::unique_ptr<uint8_t, FxFreeDeleter> m_data;
  size_t m_nTotalSize;
  size_t m_nCurSize;
  size_t m_nCurPos = 0;
};

#endif  // CORE_FXCRT_CFX_MEMORYSTREAM_H_
