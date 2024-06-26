// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcrt/bytestring.h"

#include <stddef.h>

#include <algorithm>
#include <cctype>
#include <string>
#include <utility>

#include "core/fxcrt/cfx_utf8decoder.h"
#include "core/fxcrt/fx_codepage.h"
#include "core/fxcrt/fx_extension.h"
#include "core/fxcrt/fx_safe_types.h"
#include "core/fxcrt/string_pool_template.h"
#include "third_party/base/numerics/safe_math.h"
#include "third_party/base/span.h"
#include "third_party/base/stl_util.h"

template class fxcrt::StringDataTemplate<char>;
template class fxcrt::StringViewTemplate<char>;
template class fxcrt::StringPoolTemplate<ByteString>;
template struct std::hash<ByteString>;

namespace {

constexpr char kTrimChars[] = "\x09\x0a\x0b\x0c\x0d\x20";

const char* FX_strstr(const char* haystack,
                      int haystack_len,
                      const char* needle,
                      int needle_len) {
  if (needle_len > haystack_len || needle_len == 0) {
    return nullptr;
  }
  const char* end_ptr = haystack + haystack_len - needle_len;
  while (haystack <= end_ptr) {
    int i = 0;
    while (1) {
      if (haystack[i] != needle[i]) {
        break;
      }
      i++;
      if (i == needle_len) {
        return haystack;
      }
    }
    haystack++;
  }
  return nullptr;
}

}  // namespace

namespace fxcrt {

static_assert(sizeof(ByteString) <= sizeof(char*),
              "Strings must not require more space than pointers");

#define FORCE_ANSI 0x10000
#define FORCE_UNICODE 0x20000
#define FORCE_INT64 0x40000

// static
ByteString ByteString::FormatInteger(int i) {
  char buf[32];
  FXSYS_snprintf(buf, sizeof(buf), "%d", i);
  return ByteString(buf);
}

// static
ByteString ByteString::FormatFloat(float d) {
  char buf[32];
  return ByteString(buf, FloatToString(d, buf));
}

// static
ByteString ByteString::FormatV(const char* pFormat, va_list argList) {
  va_list argListCopy;
  va_copy(argListCopy, argList);
  int nMaxLen = vsnprintf(nullptr, 0, pFormat, argListCopy);
  va_end(argListCopy);

  if (nMaxLen <= 0)
    return ByteString();

  ByteString ret;
  {
    // Span's lifetime must end before ReleaseBuffer() below.
    pdfium::span<char> buf = ret.GetBuffer(nMaxLen);

    // In the following two calls, there's always space in the buffer for
    // a terminating NUL that's not included in nMaxLen.
    memset(buf.data(), 0, nMaxLen + 1);
    va_copy(argListCopy, argList);
    vsnprintf(buf.data(), nMaxLen + 1, pFormat, argListCopy);
    va_end(argListCopy);
  }
  ret.ReleaseBuffer(ret.GetStringLength());
  return ret;
}

// static
ByteString ByteString::Format(const char* pFormat, ...) {
  va_list argList;
  va_start(argList, pFormat);
  ByteString ret = FormatV(pFormat, argList);
  va_end(argList);

  return ret;
}

ByteString::ByteString(const char* pStr, size_t nLen) {
  if (nLen)
    m_pData.Reset(StringData::Create(pStr, nLen));
}

ByteString::ByteString(const uint8_t* pStr, size_t nLen) {
  if (nLen)
    m_pData.Reset(
        StringData::Create(reinterpret_cast<const char*>(pStr), nLen));
}

ByteString::ByteString() {}

ByteString::ByteString(const ByteString& other) : m_pData(other.m_pData) {}

ByteString::ByteString(ByteString&& other) noexcept {
  m_pData.Swap(other.m_pData);
}

ByteString::ByteString(char ch) {
  m_pData.Reset(StringData::Create(1));
  m_pData->m_String[0] = ch;
}

ByteString::ByteString(const char* ptr)
    : ByteString(ptr, ptr ? strlen(ptr) : 0) {}

ByteString::ByteString(ByteStringView stringSrc) {
  if (!stringSrc.IsEmpty())
    m_pData.Reset(StringData::Create(stringSrc.unterminated_c_str(),
                                     stringSrc.GetLength()));
}

ByteString::ByteString(ByteStringView str1, ByteStringView str2) {
  FX_SAFE_SIZE_T nSafeLen = str1.GetLength();
  nSafeLen += str2.GetLength();

  size_t nNewLen = nSafeLen.ValueOrDie();
  if (nNewLen == 0)
    return;

  m_pData.Reset(StringData::Create(nNewLen));
  m_pData->CopyContents(str1.unterminated_c_str(), str1.GetLength());
  m_pData->CopyContentsAt(str1.GetLength(), str2.unterminated_c_str(),
                          str2.GetLength());
}

ByteString::ByteString(const std::initializer_list<ByteStringView>& list) {
  FX_SAFE_SIZE_T nSafeLen = 0;
  for (const auto& item : list)
    nSafeLen += item.GetLength();

  size_t nNewLen = nSafeLen.ValueOrDie();
  if (nNewLen == 0)
    return;

  m_pData.Reset(StringData::Create(nNewLen));

  size_t nOffset = 0;
  for (const auto& item : list) {
    m_pData->CopyContentsAt(nOffset, item.unterminated_c_str(),
                            item.GetLength());
    nOffset += item.GetLength();
  }
}

ByteString::ByteString(const std::ostringstream& outStream) {
  std::string str = outStream.str();
  if (str.length() > 0)
    m_pData.Reset(StringData::Create(str.c_str(), str.length()));
}

ByteString::~ByteString() {}

const ByteString& ByteString::operator=(const char* pStr) {
  if (!pStr || !pStr[0])
    clear();
  else
    AssignCopy(pStr, strlen(pStr));

  return *this;
}

const ByteString& ByteString::operator=(ByteStringView stringSrc) {
  if (stringSrc.IsEmpty())
    clear();
  else
    AssignCopy(stringSrc.unterminated_c_str(), stringSrc.GetLength());

  return *this;
}

const ByteString& ByteString::operator=(const ByteString& that) {
  if (m_pData != that.m_pData)
    m_pData = that.m_pData;

  return *this;
}

const ByteString& ByteString::operator=(ByteString&& that) {
  if (m_pData != that.m_pData)
    m_pData = std::move(that.m_pData);

  return *this;
}

const ByteString& ByteString::operator+=(const char* pStr) {
  if (pStr)
    Concat(pStr, strlen(pStr));

  return *this;
}

const ByteString& ByteString::operator+=(char ch) {
  Concat(&ch, 1);
  return *this;
}

const ByteString& ByteString::operator+=(const ByteString& str) {
  if (str.m_pData)
    Concat(str.m_pData->m_String, str.m_pData->m_nDataLength);

  return *this;
}

const ByteString& ByteString::operator+=(ByteStringView str) {
  if (!str.IsEmpty())
    Concat(str.unterminated_c_str(), str.GetLength());

  return *this;
}

bool ByteString::operator==(const char* ptr) const {
  if (!m_pData)
    return !ptr || !ptr[0];

  if (!ptr)
    return m_pData->m_nDataLength == 0;

  return strlen(ptr) == m_pData->m_nDataLength &&
         memcmp(ptr, m_pData->m_String, m_pData->m_nDataLength) == 0;
}

bool ByteString::operator==(ByteStringView str) const {
  if (!m_pData)
    return str.IsEmpty();

  return m_pData->m_nDataLength == str.GetLength() &&
         memcmp(m_pData->m_String, str.unterminated_c_str(), str.GetLength()) ==
             0;
}

bool ByteString::operator==(const ByteString& other) const {
  if (m_pData == other.m_pData)
    return true;

  if (IsEmpty())
    return other.IsEmpty();

  if (other.IsEmpty())
    return false;

  return other.m_pData->m_nDataLength == m_pData->m_nDataLength &&
         memcmp(other.m_pData->m_String, m_pData->m_String,
                m_pData->m_nDataLength) == 0;
}

bool ByteString::operator<(const char* ptr) const {
  if (!m_pData && !ptr)
    return false;
  if (c_str() == ptr)
    return false;

  size_t len = GetLength();
  size_t other_len = ptr ? strlen(ptr) : 0;
  int result = memcmp(c_str(), ptr, std::min(len, other_len));
  return result < 0 || (result == 0 && len < other_len);
}

bool ByteString::operator<(ByteStringView str) const {
  return Compare(str) < 0;
}

bool ByteString::operator<(const ByteString& other) const {
  if (m_pData == other.m_pData)
    return false;

  size_t len = GetLength();
  size_t other_len = other.GetLength();
  int result = memcmp(c_str(), other.c_str(), std::min(len, other_len));
  return result < 0 || (result == 0 && len < other_len);
}

bool ByteString::EqualNoCase(ByteStringView str) const {
  if (!m_pData)
    return str.IsEmpty();

  size_t len = str.GetLength();
  if (m_pData->m_nDataLength != len)
    return false;

  const uint8_t* pThis = (const uint8_t*)m_pData->m_String;
  const uint8_t* pThat = str.raw_str();
  for (size_t i = 0; i < len; i++) {
    if ((*pThis) != (*pThat)) {
      uint8_t bThis = tolower(*pThis);
      uint8_t bThat = tolower(*pThat);
      if (bThis != bThat)
        return false;
    }
    pThis++;
    pThat++;
  }
  return true;
}

void ByteString::AssignCopy(const char* pSrcData, size_t nSrcLen) {
  AllocBeforeWrite(nSrcLen);
  m_pData->CopyContents(pSrcData, nSrcLen);
  m_pData->m_nDataLength = nSrcLen;
}

void ByteString::ReallocBeforeWrite(size_t nNewLength) {
  if (m_pData && m_pData->CanOperateInPlace(nNewLength))
    return;

  if (nNewLength == 0) {
    clear();
    return;
  }

  RetainPtr<StringData> pNewData(StringData::Create(nNewLength));
  if (m_pData) {
    size_t nCopyLength = std::min(m_pData->m_nDataLength, nNewLength);
    pNewData->CopyContents(m_pData->m_String, nCopyLength);
    pNewData->m_nDataLength = nCopyLength;
  } else {
    pNewData->m_nDataLength = 0;
  }
  pNewData->m_String[pNewData->m_nDataLength] = 0;
  m_pData.Swap(pNewData);
}

void ByteString::AllocBeforeWrite(size_t nNewLength) {
  if (m_pData && m_pData->CanOperateInPlace(nNewLength))
    return;

  if (nNewLength == 0) {
    clear();
    return;
  }

  m_pData.Reset(StringData::Create(nNewLength));
}

void ByteString::ReleaseBuffer(size_t nNewLength) {
  if (!m_pData)
    return;

  nNewLength = std::min(nNewLength, m_pData->m_nAllocLength);
  if (nNewLength == 0) {
    clear();
    return;
  }

  ASSERT(m_pData->m_nRefs == 1);
  m_pData->m_nDataLength = nNewLength;
  m_pData->m_String[nNewLength] = 0;
  if (m_pData->m_nAllocLength - nNewLength >= 32) {
    // Over arbitrary threshold, so pay the price to relocate.  Force copy to
    // always occur by holding a second reference to the string.
    ByteString preserve(*this);
    ReallocBeforeWrite(nNewLength);
  }
}

void ByteString::Reserve(size_t len) {
  GetBuffer(len);
}

pdfium::span<char> ByteString::GetBuffer(size_t nMinBufLength) {
  if (!m_pData) {
    if (nMinBufLength == 0)
      return pdfium::span<char>();

    m_pData.Reset(StringData::Create(nMinBufLength));
    m_pData->m_nDataLength = 0;
    m_pData->m_String[0] = 0;
    return pdfium::span<char>(m_pData->m_String, m_pData->m_nAllocLength);
  }

  if (m_pData->CanOperateInPlace(nMinBufLength))
    return pdfium::span<char>(m_pData->m_String, m_pData->m_nAllocLength);

  nMinBufLength = std::max(nMinBufLength, m_pData->m_nDataLength);
  if (nMinBufLength == 0)
    return pdfium::span<char>();

  RetainPtr<StringData> pNewData(StringData::Create(nMinBufLength));
  pNewData->CopyContents(*m_pData);
  pNewData->m_nDataLength = m_pData->m_nDataLength;
  m_pData.Swap(pNewData);
  return pdfium::span<char>(m_pData->m_String, m_pData->m_nAllocLength);
}

size_t ByteString::Delete(size_t index, size_t count) {
  if (!m_pData)
    return 0;

  size_t old_length = m_pData->m_nDataLength;
  if (count == 0 || index != pdfium::clamp<size_t>(index, 0, old_length))
    return old_length;

  size_t removal_length = index + count;
  if (removal_length > old_length)
    return old_length;

  ReallocBeforeWrite(old_length);
  size_t chars_to_copy = old_length - removal_length + 1;
  memmove(m_pData->m_String + index, m_pData->m_String + removal_length,
          chars_to_copy);
  m_pData->m_nDataLength = old_length - count;
  return m_pData->m_nDataLength;
}

void ByteString::Concat(const char* pSrcData, size_t nSrcLen) {
  if (!pSrcData || nSrcLen == 0)
    return;

  if (!m_pData) {
    m_pData.Reset(StringData::Create(pSrcData, nSrcLen));
    return;
  }

  if (m_pData->CanOperateInPlace(m_pData->m_nDataLength + nSrcLen)) {
    m_pData->CopyContentsAt(m_pData->m_nDataLength, pSrcData, nSrcLen);
    m_pData->m_nDataLength += nSrcLen;
    return;
  }

  size_t nConcatLen = std::max(m_pData->m_nDataLength / 2, nSrcLen);
  RetainPtr<StringData> pNewData(
      StringData::Create(m_pData->m_nDataLength + nConcatLen));
  pNewData->CopyContents(*m_pData);
  pNewData->CopyContentsAt(m_pData->m_nDataLength, pSrcData, nSrcLen);
  pNewData->m_nDataLength = m_pData->m_nDataLength + nSrcLen;
  m_pData.Swap(pNewData);
}

intptr_t ByteString::ReferenceCountForTesting() const {
  return m_pData ? m_pData->m_nRefs : 0;
}

ByteString ByteString::Mid(size_t first, size_t count) const {
  if (!m_pData)
    return ByteString();

  if (!IsValidIndex(first))
    return ByteString();

  if (count == 0 || !IsValidLength(count))
    return ByteString();

  if (!IsValidIndex(first + count - 1))
    return ByteString();

  if (first == 0 && count == m_pData->m_nDataLength)
    return *this;

  ByteString dest;
  AllocCopy(dest, count, first);
  return dest;
}

ByteString ByteString::Left(size_t count) const {
  if (count == 0 || !IsValidLength(count))
    return ByteString();
  return Mid(0, count);
}

ByteString ByteString::Right(size_t count) const {
  if (count == 0 || !IsValidLength(count))
    return ByteString();
  return Mid(GetLength() - count, count);
}

void ByteString::AllocCopy(ByteString& dest,
                           size_t nCopyLen,
                           size_t nCopyIndex) const {
  if (nCopyLen == 0)
    return;

  RetainPtr<StringData> pNewData(
      StringData::Create(m_pData->m_String + nCopyIndex, nCopyLen));
  dest.m_pData.Swap(pNewData);
}

void ByteString::SetAt(size_t index, char c) {
  ASSERT(IsValidIndex(index));
  ReallocBeforeWrite(m_pData->m_nDataLength);
  m_pData->m_String[index] = c;
}

size_t ByteString::Insert(size_t location, char ch) {
  const size_t cur_length = GetLength();
  if (!IsValidLength(location))
    return cur_length;

  const size_t new_length = cur_length + 1;
  ReallocBeforeWrite(new_length);
  memmove(m_pData->m_String + location + 1, m_pData->m_String + location,
          new_length - location);
  m_pData->m_String[location] = ch;
  m_pData->m_nDataLength = new_length;
  return new_length;
}

Optional<size_t> ByteString::Find(char ch, size_t start) const {
  if (!m_pData)
    return Optional<size_t>();

  if (!IsValidIndex(start))
    return Optional<size_t>();

  const char* pStr = static_cast<const char*>(
      memchr(m_pData->m_String + start, ch, m_pData->m_nDataLength - start));
  return pStr ? Optional<size_t>(static_cast<size_t>(pStr - m_pData->m_String))
              : Optional<size_t>();
}

Optional<size_t> ByteString::Find(ByteStringView subStr, size_t start) const {
  if (!m_pData)
    return Optional<size_t>();

  if (!IsValidIndex(start))
    return Optional<size_t>();

  const char* pStr =
      FX_strstr(m_pData->m_String + start, m_pData->m_nDataLength - start,
                subStr.unterminated_c_str(), subStr.GetLength());
  return pStr ? Optional<size_t>(static_cast<size_t>(pStr - m_pData->m_String))
              : Optional<size_t>();
}

Optional<size_t> ByteString::ReverseFind(char ch) const {
  if (!m_pData)
    return Optional<size_t>();

  size_t nLength = m_pData->m_nDataLength;
  while (nLength--) {
    if (m_pData->m_String[nLength] == ch)
      return Optional<size_t>(nLength);
  }
  return Optional<size_t>();
}

void ByteString::MakeLower() {
  if (!m_pData)
    return;

  ReallocBeforeWrite(m_pData->m_nDataLength);
  FXSYS_strlwr(m_pData->m_String);
}

void ByteString::MakeUpper() {
  if (!m_pData)
    return;

  ReallocBeforeWrite(m_pData->m_nDataLength);
  FXSYS_strupr(m_pData->m_String);
}

size_t ByteString::Remove(char chRemove) {
  if (!m_pData || m_pData->m_nDataLength == 0)
    return 0;

  char* pstrSource = m_pData->m_String;
  char* pstrEnd = m_pData->m_String + m_pData->m_nDataLength;
  while (pstrSource < pstrEnd) {
    if (*pstrSource == chRemove)
      break;
    pstrSource++;
  }
  if (pstrSource == pstrEnd)
    return 0;

  ptrdiff_t copied = pstrSource - m_pData->m_String;
  ReallocBeforeWrite(m_pData->m_nDataLength);
  pstrSource = m_pData->m_String + copied;
  pstrEnd = m_pData->m_String + m_pData->m_nDataLength;

  char* pstrDest = pstrSource;
  while (pstrSource < pstrEnd) {
    if (*pstrSource != chRemove) {
      *pstrDest = *pstrSource;
      pstrDest++;
    }
    pstrSource++;
  }

  *pstrDest = 0;
  size_t nCount = static_cast<size_t>(pstrSource - pstrDest);
  m_pData->m_nDataLength -= nCount;
  return nCount;
}

size_t ByteString::Replace(ByteStringView pOld, ByteStringView pNew) {
  if (!m_pData || pOld.IsEmpty())
    return 0;

  size_t nSourceLen = pOld.GetLength();
  size_t nReplacementLen = pNew.GetLength();
  size_t nCount = 0;
  const char* pStart = m_pData->m_String;
  char* pEnd = m_pData->m_String + m_pData->m_nDataLength;
  while (1) {
    const char* pTarget = FX_strstr(pStart, static_cast<int>(pEnd - pStart),
                                    pOld.unterminated_c_str(), nSourceLen);
    if (!pTarget)
      break;

    nCount++;
    pStart = pTarget + nSourceLen;
  }
  if (nCount == 0)
    return 0;

  size_t nNewLength =
      m_pData->m_nDataLength + (nReplacementLen - nSourceLen) * nCount;

  if (nNewLength == 0) {
    clear();
    return nCount;
  }

  RetainPtr<StringData> pNewData(StringData::Create(nNewLength));
  pStart = m_pData->m_String;
  char* pDest = pNewData->m_String;
  for (size_t i = 0; i < nCount; i++) {
    const char* pTarget = FX_strstr(pStart, static_cast<int>(pEnd - pStart),
                                    pOld.unterminated_c_str(), nSourceLen);
    memcpy(pDest, pStart, pTarget - pStart);
    pDest += pTarget - pStart;
    memcpy(pDest, pNew.unterminated_c_str(), pNew.GetLength());
    pDest += pNew.GetLength();
    pStart = pTarget + nSourceLen;
  }
  memcpy(pDest, pStart, pEnd - pStart);
  m_pData.Swap(pNewData);
  return nCount;
}

int ByteString::Compare(ByteStringView str) const {
  if (!m_pData)
    return str.IsEmpty() ? 0 : -1;

  size_t this_len = m_pData->m_nDataLength;
  size_t that_len = str.GetLength();
  size_t min_len = std::min(this_len, that_len);
  int result = memcmp(m_pData->m_String, str.unterminated_c_str(), min_len);
  if (result != 0)
    return result;
  if (this_len == that_len)
    return 0;
  return this_len < that_len ? -1 : 1;
}

void ByteString::Trim() {
  TrimRight(kTrimChars);
  TrimLeft(kTrimChars);
}

void ByteString::Trim(char target) {
  ByteStringView targets(target);
  TrimRight(targets);
  TrimLeft(targets);
}

void ByteString::Trim(ByteStringView targets) {
  TrimRight(targets);
  TrimLeft(targets);
}

void ByteString::TrimLeft() {
  TrimLeft(kTrimChars);
}

void ByteString::TrimLeft(char target) {
  TrimLeft(ByteStringView(target));
}

void ByteString::TrimLeft(ByteStringView targets) {
  if (!m_pData || targets.IsEmpty())
    return;

  size_t len = GetLength();
  if (len == 0)
    return;

  size_t pos = 0;
  while (pos < len) {
    size_t i = 0;
    while (i < targets.GetLength() && targets[i] != m_pData->m_String[pos])
      i++;
    if (i == targets.GetLength())
      break;
    pos++;
  }
  if (pos) {
    ReallocBeforeWrite(len);
    size_t nDataLength = len - pos;
    memmove(m_pData->m_String, m_pData->m_String + pos,
            (nDataLength + 1) * sizeof(char));
    m_pData->m_nDataLength = nDataLength;
  }
}

void ByteString::TrimRight() {
  TrimRight(kTrimChars);
}

void ByteString::TrimRight(char target) {
  TrimRight(ByteStringView(target));
}

void ByteString::TrimRight(ByteStringView targets) {
  if (!m_pData || targets.IsEmpty())
    return;

  size_t pos = GetLength();
  if (pos == 0)
    return;

  while (pos) {
    size_t i = 0;
    while (i < targets.GetLength() && targets[i] != m_pData->m_String[pos - 1])
      i++;
    if (i == targets.GetLength())
      break;
    pos--;
  }
  if (pos < m_pData->m_nDataLength) {
    ReallocBeforeWrite(m_pData->m_nDataLength);
    m_pData->m_String[pos] = 0;
    m_pData->m_nDataLength = pos;
  }
}

std::ostream& operator<<(std::ostream& os, const ByteString& str) {
  return os.write(str.c_str(), str.GetLength());
}

std::ostream& operator<<(std::ostream& os, ByteStringView str) {
  return os.write(str.unterminated_c_str(), str.GetLength());
}

}  // namespace fxcrt

uint32_t FX_HashCode_GetA(ByteStringView str, bool bIgnoreCase) {
  uint32_t dwHashCode = 0;
  if (bIgnoreCase) {
    for (ByteStringView::UnsignedType c : str)
      dwHashCode = 31 * dwHashCode + tolower(c);
  } else {
    for (ByteStringView::UnsignedType c : str)
      dwHashCode = 31 * dwHashCode + c;
  }
  return dwHashCode;
}

uint32_t FX_HashCode_GetAsIfW(ByteStringView str, bool bIgnoreCase) {
  uint32_t dwHashCode = 0;
  if (bIgnoreCase) {
    for (ByteStringView::UnsignedType c : str)
      dwHashCode = 1313 * dwHashCode + FXSYS_towlower(c);
  } else {
    for (ByteStringView::UnsignedType c : str)
      dwHashCode = 1313 * dwHashCode + c;
  }
  return dwHashCode;
}
