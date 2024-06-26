// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef THIRD_PARTY_BASE_STL_UTIL_H_
#define THIRD_PARTY_BASE_STL_UTIL_H_

#include <algorithm>
#include <iterator>
#include <memory>
#include <set>
#include <vector>

#include "third_party/base/numerics/safe_conversions.h"
#include "third_party/base/numerics/safe_math.h"

namespace pdfium {

// C++11 implementation of C++17's std::size():
// http://en.cppreference.com/w/cpp/iterator/size
template <typename Container>
constexpr auto size(const Container& c) -> decltype(c.size()) {
  return c.size();
}

template <typename T, size_t N>
constexpr size_t size(const T (&array)[N]) noexcept {
  return N;
}

// Test to see if a set, map, hash_set or hash_map contains a particular key.
// Returns true if the key is in the collection.
template <typename Collection, typename Key>
bool ContainsKey(const Collection& collection, const Key& key) {
  return collection.find(key) != collection.end();
}

// Test to see if a collection like a vector contains a particular value.
// Returns true if the value is in the collection.
template <typename Collection, typename Value>
bool ContainsValue(const Collection& collection, const Value& value) {
  return std::find(std::begin(collection), std::end(collection), value) !=
         std::end(collection);
}

// Means of generating a key for searching STL collections of std::unique_ptr
// that avoids the side effect of deleting the pointer.
template <class T>
class FakeUniquePtr : public std::unique_ptr<T> {
 public:
  using std::unique_ptr<T>::unique_ptr;
  ~FakeUniquePtr() { std::unique_ptr<T>::release(); }
};

// Convenience routine for "int-fected" code, so that the stl collection
// size_t size() method return values will be checked.
template <typename ResultType, typename Collection>
ResultType CollectionSize(const Collection& collection) {
  return pdfium::base::checked_cast<ResultType>(collection.size());
}

// Convenience routine for "int-fected" code, to handle signed indicies. The
// compiler can deduce the type, making this more convenient than the above.
template <typename IndexType, typename Collection>
bool IndexInBounds(const Collection& collection, IndexType index) {
  return index >= 0 && index < CollectionSize<IndexType>(collection);
}

// Track the addition of an object to a set, removing it automatically when
// the ScopedSetInsertion goes out of scope.
template <typename T>
class ScopedSetInsertion {
 public:
  ScopedSetInsertion(std::set<T>* org_set, T elem)
      : m_Set(org_set), m_Entry(elem) {
    m_Set->insert(m_Entry);
  }
  ~ScopedSetInsertion() { m_Set->erase(m_Entry); }

 private:
  std::set<T>* const m_Set;
  const T m_Entry;
};

// std::clamp(), some day.
template <class T>
constexpr const T& clamp(const T& v, const T& lo, const T& hi) {
  return std::min(std::max(v, lo), hi);
}

// Safely allocate a 1-dim vector big enough for |w| by |h| or die.
template <typename T>
std::vector<T> Vector2D(size_t w, size_t h) {
  pdfium::base::CheckedNumeric<size_t> safe_size = w;
  safe_size *= h;
  return std::vector<T>(safe_size.ValueOrDie());
}

}  // namespace pdfium

#endif  // THIRD_PARTY_BASE_STL_UTIL_H_
