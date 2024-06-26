// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCRT_STRING_DATA_TEMPLATE_H_
#define CORE_FXCRT_STRING_DATA_TEMPLATE_H_

#include "core/fxcrt/fx_memory.h"
#include "core/fxcrt/fx_system.h"
#include "third_party/base/numerics/safe_math.h"

namespace fxcrt {

template <typename CharType>
class StringDataTemplate {
 public:
  static StringDataTemplate* Create(size_t nLen) {
    ASSERT(nLen > 0);

    // Calculate space needed for the fixed portion of the struct plus the
    // NUL char that is not included in |m_nAllocLength|.
    int overhead = offsetof(StringDataTemplate, m_String) + sizeof(CharType);
    pdfium::base::CheckedNumeric<size_t> nSize = nLen;
    nSize *= sizeof(CharType);
    nSize += overhead;

    // Now round to an 8-byte boundary. We'd expect that this is the minimum
    // granularity of any of the underlying allocators, so there may be cases
    // where we can save a re-alloc when adding a few characters to a string
    // by using this otherwise wasted space.
    nSize += 7;
    nSize &= ~7;
    size_t totalSize = nSize.ValueOrDie();
    size_t usableLen = (totalSize - overhead) / sizeof(CharType);
    ASSERT(usableLen >= nLen);

    void* pData = GetStringPartitionAllocator().root()->Alloc(
        totalSize, "StringDataTemplate");
    return new (pData) StringDataTemplate(nLen, usableLen);
  }

  static StringDataTemplate* Create(const CharType* pStr, size_t nLen) {
    StringDataTemplate* result = Create(nLen);
    result->CopyContents(pStr, nLen);
    return result;
  }

  void Retain() { ++m_nRefs; }
  void Release() {
    if (--m_nRefs <= 0)
      GetStringPartitionAllocator().root()->Free(this);
  }

  bool CanOperateInPlace(size_t nTotalLen) const {
    return m_nRefs <= 1 && nTotalLen <= m_nAllocLength;
  }

  void CopyContents(const StringDataTemplate& other) {
    ASSERT(other.m_nDataLength <= m_nAllocLength);
    memcpy(m_String, other.m_String,
           (other.m_nDataLength + 1) * sizeof(CharType));
  }

  void CopyContents(const CharType* pStr, size_t nLen) {
    ASSERT(nLen >= 0);
    ASSERT(nLen <= m_nAllocLength);

    memcpy(m_String, pStr, nLen * sizeof(CharType));
    m_String[nLen] = 0;
  }

  void CopyContentsAt(size_t offset, const CharType* pStr, size_t nLen) {
    ASSERT(offset >= 0);
    ASSERT(nLen >= 0);
    ASSERT(offset + nLen <= m_nAllocLength);

    memcpy(m_String + offset, pStr, nLen * sizeof(CharType));
    m_String[offset + nLen] = 0;
  }

  // To ensure ref counts do not overflow, consider the worst possible case:
  // the entire address space contains nothing but pointers to this object.
  // Since the count increments with each new pointer, the largest value is
  // the number of pointers that can fit into the address space. The size of
  // the address space itself is a good upper bound on it.
  intptr_t m_nRefs;

  // These lengths are in terms of number of characters, not bytes, and do not
  // include the terminating NUL character, but the underlying buffer is sized
  // to be capable of holding it.
  size_t m_nDataLength;
  size_t m_nAllocLength;

  // Not really 1, variable size.
  CharType m_String[1];

 private:
  StringDataTemplate(size_t dataLen, size_t allocLen)
      : m_nRefs(0), m_nDataLength(dataLen), m_nAllocLength(allocLen) {
    ASSERT(dataLen >= 0);
    ASSERT(dataLen <= allocLen);
    m_String[dataLen] = 0;
  }

  ~StringDataTemplate() = delete;
};

extern template class StringDataTemplate<char>;
extern template class StringDataTemplate<wchar_t>;

}  // namespace fxcrt

using fxcrt::StringDataTemplate;

#endif  // CORE_FXCRT_STRING_DATA_TEMPLATE_H_
