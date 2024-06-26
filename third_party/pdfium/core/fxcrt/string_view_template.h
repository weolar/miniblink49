// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FXCRT_STRING_VIEW_TEMPLATE_H_
#define CORE_FXCRT_STRING_VIEW_TEMPLATE_H_

#include <algorithm>
#include <iterator>
#include <type_traits>
#include <vector>

#include "core/fxcrt/fx_system.h"
#include "third_party/base/optional.h"
#include "third_party/base/span.h"
#include "third_party/base/stl_util.h"

namespace fxcrt {

// An immutable string with caller-provided storage which must outlive the
// string itself. These are not necessarily nul-terminated, so that substring
// extraction (via the Mid(), Left(), and Right() methods) is copy-free.
//
// String view arguments should be passed by value, since they are small,
// rather than const-ref, even if they are not modified.
template <typename T>
class StringViewTemplate {
 public:
  using CharType = T;
  using UnsignedType = typename std::make_unsigned<CharType>::type;
  using const_iterator = const CharType*;
  using const_reverse_iterator = std::reverse_iterator<const_iterator>;

  constexpr StringViewTemplate() /*noexcept*/ = default;
  constexpr StringViewTemplate(const StringViewTemplate& src) /*noexcept*/ =
      default;

  // Deliberately implicit to avoid calling on every string literal.
  // NOLINTNEXTLINE(runtime/explicit)
  StringViewTemplate(const CharType* ptr) /*noexcept*/
      : m_Span(reinterpret_cast<const UnsignedType*>(ptr),
               ptr ? FXSYS_len(ptr) : 0) {}

  constexpr StringViewTemplate(const CharType* ptr, size_t len) /*noexcept*/
      : m_Span(reinterpret_cast<const UnsignedType*>(ptr), len) {}

  explicit constexpr StringViewTemplate(
      const pdfium::span<CharType>& other) /*noexcept*/
      : m_Span(reinterpret_cast<const UnsignedType*>(other.data()),
               other.size()) {}

  template <typename U = UnsignedType>
  constexpr StringViewTemplate(
      const UnsignedType* ptr,
      size_t size,
      typename std::enable_if<!std::is_same<U, CharType>::value>::type* =
          0) /*noexcept*/
      : m_Span(ptr, size) {}

  template <typename U = UnsignedType>
  StringViewTemplate(
      const pdfium::span<U> other,
      typename std::enable_if<!std::is_same<U, CharType>::value>::type* =
          0) /*noexcept*/
      : m_Span(other) {}

  // Deliberately implicit to avoid calling on every string literal.
  // |ch| must be an lvalue that outlives the StringViewTemplate.
  // NOLINTNEXTLINE(runtime/explicit)
  constexpr StringViewTemplate(CharType& ch) /*noexcept*/
      : m_Span(reinterpret_cast<const UnsignedType*>(&ch), 1) {}

  // Any changes to |vec| invalidate the string.
  explicit StringViewTemplate(const std::vector<UnsignedType>& vec) /*noexcept*/
      : m_Span(vec.size() ? vec.data() : nullptr, vec.size()) {}

  StringViewTemplate& operator=(const CharType* src) {
    m_Span = pdfium::span<const UnsignedType>(
        reinterpret_cast<const UnsignedType*>(src), src ? FXSYS_len(src) : 0);
    return *this;
  }

  StringViewTemplate& operator=(const StringViewTemplate& src) {
    m_Span = src.m_Span;
    return *this;
  }

  const_iterator begin() const {
    return reinterpret_cast<const_iterator>(m_Span.begin());
  }
  const_iterator end() const {
    return reinterpret_cast<const_iterator>(m_Span.end());
  }
  const_reverse_iterator rbegin() const {
    return const_reverse_iterator(end());
  }
  const_reverse_iterator rend() const {
    return const_reverse_iterator(begin());
  }

  bool operator==(const StringViewTemplate& other) const {
    return m_Span == other.m_Span;
  }
  bool operator==(const CharType* ptr) const {
    StringViewTemplate other(ptr);
    return *this == other;
  }
  bool operator!=(const CharType* ptr) const { return !(*this == ptr); }
  bool operator!=(const StringViewTemplate& other) const {
    return !(*this == other);
  }

  bool IsASCII() const {
    for (auto c : *this) {
      if (c <= 0 || c > 127)  // Questionable signedness of |c|.
        return false;
    }
    return true;
  }

  bool EqualsASCII(const StringViewTemplate<char>& that) const {
    size_t length = GetLength();
    if (length != that.GetLength())
      return false;

    for (size_t i = 0; i < length; ++i) {
      auto c = (*this)[i];
      if (c <= 0 || c > 127 || c != that[i])  // Questionable signedness of |c|.
        return false;
    }
    return true;
  }

  bool EqualsASCIINoCase(const StringViewTemplate<char>& that) const {
    size_t length = GetLength();
    if (length != that.GetLength())
      return false;

    for (size_t i = 0; i < length; ++i) {
      auto c = (*this)[i];
      if (c <= 0 || c > 127 || tolower(c) != tolower(that[i]))
        return false;
    }
    return true;
  }

  uint32_t GetID() const {
    if (m_Span.size() == 0)
      return 0;

    uint32_t strid = 0;
    size_t size = std::min(static_cast<size_t>(4), m_Span.size());
    for (size_t i = 0; i < size; i++)
      strid = strid * 256 + m_Span[i];

    return strid << ((4 - size) * 8);
  }

  pdfium::span<const UnsignedType> span() const { return m_Span; }
  const UnsignedType* raw_str() const { return m_Span.data(); }
  const CharType* unterminated_c_str() const {
    return reinterpret_cast<const CharType*>(m_Span.data());
  }

  size_t GetLength() const { return m_Span.size(); }
  bool IsEmpty() const { return m_Span.empty(); }
  bool IsValidIndex(size_t index) const { return index < m_Span.size(); }
  bool IsValidLength(size_t length) const { return length <= m_Span.size(); }

  const UnsignedType& operator[](const size_t index) const {
    return m_Span[index];
  }

  UnsignedType First() const { return m_Span.size() ? m_Span[0] : 0; }
  UnsignedType Last() const {
    return m_Span.size() ? m_Span[m_Span.size() - 1] : 0;
  }

  const CharType CharAt(const size_t index) const {
    return static_cast<CharType>(m_Span[index]);
  }

  Optional<size_t> Find(CharType ch) const {
    const auto* found = reinterpret_cast<const UnsignedType*>(FXSYS_chr(
        reinterpret_cast<const CharType*>(m_Span.data()), ch, m_Span.size()));

    return found ? Optional<size_t>(found - m_Span.data()) : Optional<size_t>();
  }

  bool Contains(CharType ch) const { return Find(ch).has_value(); }

  StringViewTemplate Mid(size_t first, size_t count) const {
    if (!m_Span.data())
      return StringViewTemplate();

    if (!IsValidIndex(first))
      return StringViewTemplate();

    if (count == 0 || !IsValidLength(count))
      return StringViewTemplate();

    if (!IsValidIndex(first + count - 1))
      return StringViewTemplate();

    return StringViewTemplate(m_Span.data() + first, count);
  }

  StringViewTemplate Left(size_t count) const {
    if (count == 0 || !IsValidLength(count))
      return StringViewTemplate();
    return Mid(0, count);
  }

  StringViewTemplate Right(size_t count) const {
    if (count == 0 || !IsValidLength(count))
      return StringViewTemplate();
    return Mid(GetLength() - count, count);
  }

  StringViewTemplate TrimmedRight(CharType ch) const {
    if (IsEmpty())
      return StringViewTemplate();

    size_t pos = GetLength();
    while (pos && CharAt(pos - 1) == ch)
      pos--;

    if (pos == 0)
      return StringViewTemplate();

    return StringViewTemplate(m_Span.data(), pos);
  }

  bool operator<(const StringViewTemplate& that) const {
    int result =
        FXSYS_cmp(reinterpret_cast<const CharType*>(m_Span.data()),
                  reinterpret_cast<const CharType*>(that.m_Span.data()),
                  std::min(m_Span.size(), that.m_Span.size()));
    return result < 0 || (result == 0 && m_Span.size() < that.m_Span.size());
  }

  bool operator>(const StringViewTemplate& that) const {
    int result =
        FXSYS_cmp(reinterpret_cast<const CharType*>(m_Span.data()),
                  reinterpret_cast<const CharType*>(that.m_Span.data()),
                  std::min(m_Span.size(), that.m_Span.size()));
    return result > 0 || (result == 0 && m_Span.size() > that.m_Span.size());
  }

 protected:
  pdfium::span<const UnsignedType> m_Span;

 private:
  void* operator new(size_t) /*throw()*/ { return nullptr; }
};

template <typename T>
inline bool operator==(const T* lhs, const StringViewTemplate<T>& rhs) {
  return rhs == lhs;
}
template <typename T>
inline bool operator!=(const T* lhs, const StringViewTemplate<T>& rhs) {
  return rhs != lhs;
}
template <typename T>
inline bool operator<(const T* lhs, const StringViewTemplate<T>& rhs) {
  return rhs > lhs;
}

// Workaround for one of the cases external template classes are
// failing in GCC before version 7 with -O0
#if !defined(__GNUC__) || __GNUC__ >= 7
extern template class StringViewTemplate<char>;
extern template class StringViewTemplate<wchar_t>;
#endif

using ByteStringView = StringViewTemplate<char>;
using WideStringView = StringViewTemplate<wchar_t>;

}  // namespace fxcrt

using ByteStringView = fxcrt::ByteStringView;
using WideStringView = fxcrt::WideStringView;

#endif  // CORE_FXCRT_STRING_VIEW_TEMPLATE_H_
