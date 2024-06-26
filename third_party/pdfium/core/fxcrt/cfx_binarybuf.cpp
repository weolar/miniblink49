// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcrt/cfx_binarybuf.h"

#include <algorithm>
#include <utility>

CFX_BinaryBuf::CFX_BinaryBuf() = default;

CFX_BinaryBuf::~CFX_BinaryBuf() = default;

void CFX_BinaryBuf::Delete(size_t start_index, size_t count) {
  if (!m_pBuffer || count > m_DataSize || start_index > m_DataSize - count)
    return;

  memmove(m_pBuffer.get() + start_index, m_pBuffer.get() + start_index + count,
          m_DataSize - start_index - count);
  m_DataSize -= count;
}

size_t CFX_BinaryBuf::GetLength() const {
  return m_DataSize;
}

void CFX_BinaryBuf::Clear() {
  m_DataSize = 0;
}

std::unique_ptr<uint8_t, FxFreeDeleter> CFX_BinaryBuf::DetachBuffer() {
  m_DataSize = 0;
  m_AllocSize = 0;
  return std::move(m_pBuffer);
}

void CFX_BinaryBuf::EstimateSize(size_t size) {
  if (m_AllocSize < size)
    ExpandBuf(size - m_DataSize);
}

void CFX_BinaryBuf::ExpandBuf(size_t add_size) {
  FX_SAFE_SIZE_T new_size = m_DataSize;
  new_size += add_size;
  if (m_AllocSize >= new_size.ValueOrDie())
    return;

  size_t alloc_step = std::max(static_cast<size_t>(128),
                               m_AllocStep ? m_AllocStep : m_AllocSize / 4);
  new_size += alloc_step - 1;  // Quantize, don't combine these lines.
  new_size /= alloc_step;
  new_size *= alloc_step;
  m_AllocSize = new_size.ValueOrDie();
  m_pBuffer.reset(m_pBuffer
                      ? FX_Realloc(uint8_t, m_pBuffer.release(), m_AllocSize)
                      : FX_Alloc(uint8_t, m_AllocSize));
}

void CFX_BinaryBuf::AppendBlock(const void* pBuf, size_t size) {
  if (size == 0)
    return;

  ExpandBuf(size);
  if (pBuf) {
    memcpy(m_pBuffer.get() + m_DataSize, pBuf, size);
  } else {
    memset(m_pBuffer.get() + m_DataSize, 0, size);
  }
  m_DataSize += size;
}
