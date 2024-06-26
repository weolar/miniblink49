// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCRT_CFX_BINARYBUF_H_
#define CORE_FXCRT_CFX_BINARYBUF_H_

#include <memory>

#include "core/fxcrt/fx_memory.h"
#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/fx_system.h"

class CFX_BinaryBuf {
 public:
  CFX_BinaryBuf();
  virtual ~CFX_BinaryBuf();

  uint8_t* GetBuffer() const { return m_pBuffer.get(); }
  size_t GetSize() const { return m_DataSize; }
  virtual size_t GetLength() const;
  bool IsEmpty() const { return GetLength() == 0; }

  void Clear();
  void SetAllocStep(size_t step) { m_AllocStep = step; }
  void EstimateSize(size_t size);
  void AppendBlock(const void* pBuf, size_t size);
  void AppendString(const ByteString& str) {
    AppendBlock(str.c_str(), str.GetLength());
  }

  void AppendByte(uint8_t byte) {
    ExpandBuf(1);
    m_pBuffer.get()[m_DataSize++] = byte;
  }

  void Delete(size_t start_index, size_t count);

  // Releases ownership of |m_pBuffer| and returns it.
  std::unique_ptr<uint8_t, FxFreeDeleter> DetachBuffer();

 protected:
  void ExpandBuf(size_t size);

  size_t m_AllocStep = 0;
  size_t m_AllocSize = 0;
  size_t m_DataSize = 0;
  std::unique_ptr<uint8_t, FxFreeDeleter> m_pBuffer;
};

#endif  // CORE_FXCRT_CFX_BINARYBUF_H_
