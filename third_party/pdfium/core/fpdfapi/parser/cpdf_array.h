// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_PARSER_CPDF_ARRAY_H_
#define CORE_FPDFAPI_PARSER_CPDF_ARRAY_H_

#include <memory>
#include <set>
#include <type_traits>
#include <utility>
#include <vector>

#include "core/fpdfapi/parser/cpdf_indirect_object_holder.h"
#include "core/fpdfapi/parser/cpdf_object.h"
#include "core/fxcrt/fx_coordinates.h"
#include "third_party/base/ptr_util.h"

class CPDF_Array final : public CPDF_Object {
 public:
  using const_iterator =
      std::vector<std::unique_ptr<CPDF_Object>>::const_iterator;

  CPDF_Array();
  explicit CPDF_Array(const WeakPtr<ByteStringPool>& pPool);
  ~CPDF_Array() override;

  // CPDF_Object:
  Type GetType() const override;
  std::unique_ptr<CPDF_Object> Clone() const override;
  bool IsArray() const override;
  CPDF_Array* AsArray() override;
  const CPDF_Array* AsArray() const override;
  bool WriteTo(IFX_ArchiveStream* archive,
               const CPDF_Encryptor* encryptor) const override;

  bool IsEmpty() const { return m_Objects.empty(); }
  size_t size() const { return m_Objects.size(); }
  CPDF_Object* GetObjectAt(size_t index);
  const CPDF_Object* GetObjectAt(size_t index) const;
  CPDF_Object* GetDirectObjectAt(size_t index);
  const CPDF_Object* GetDirectObjectAt(size_t index) const;
  ByteString GetStringAt(size_t index) const;
  WideString GetUnicodeTextAt(size_t index) const;
  int GetIntegerAt(size_t index) const;
  float GetNumberAt(size_t index) const;
  CPDF_Dictionary* GetDictAt(size_t index);
  const CPDF_Dictionary* GetDictAt(size_t index) const;
  CPDF_Stream* GetStreamAt(size_t index);
  const CPDF_Stream* GetStreamAt(size_t index) const;
  CPDF_Array* GetArrayAt(size_t index);
  const CPDF_Array* GetArrayAt(size_t index) const;
  float GetFloatAt(size_t index) const { return GetNumberAt(index); }
  CFX_Matrix GetMatrix() const;
  CFX_FloatRect GetRect() const;

  // Takes ownership of |pObj|, returns unowned pointer to it.
  CPDF_Object* Add(std::unique_ptr<CPDF_Object> pObj);
  CPDF_Object* SetAt(size_t index, std::unique_ptr<CPDF_Object> pObj);
  CPDF_Object* InsertAt(size_t index, std::unique_ptr<CPDF_Object> pObj);

  // Creates object owned by the array, returns unowned pointer to it.
  // We have special cases for objects that can intern strings from
  // a ByteStringPool.
  template <typename T, typename... Args>
  typename std::enable_if<!CanInternStrings<T>::value, T*>::type AddNew(
      Args&&... args) {
    return static_cast<T*>(
        Add(pdfium::MakeUnique<T>(std::forward<Args>(args)...)));
  }
  template <typename T, typename... Args>
  typename std::enable_if<CanInternStrings<T>::value, T*>::type AddNew(
      Args&&... args) {
    return static_cast<T*>(
        Add(pdfium::MakeUnique<T>(m_pPool, std::forward<Args>(args)...)));
  }
  template <typename T, typename... Args>
  typename std::enable_if<!CanInternStrings<T>::value, T*>::type SetNewAt(
      size_t index,
      Args&&... args) {
    return static_cast<T*>(
        SetAt(index, pdfium::MakeUnique<T>(std::forward<Args>(args)...)));
  }
  template <typename T, typename... Args>
  typename std::enable_if<CanInternStrings<T>::value, T*>::type SetNewAt(
      size_t index,
      Args&&... args) {
    return static_cast<T*>(SetAt(
        index, pdfium::MakeUnique<T>(m_pPool, std::forward<Args>(args)...)));
  }
  template <typename T, typename... Args>
  typename std::enable_if<!CanInternStrings<T>::value, T*>::type InsertNewAt(
      size_t index,
      Args&&... args) {
    return static_cast<T*>(
        InsertAt(index, pdfium::MakeUnique<T>(std::forward<Args>(args)...)));
  }
  template <typename T, typename... Args>
  typename std::enable_if<CanInternStrings<T>::value, T*>::type InsertNewAt(
      size_t index,
      Args&&... args) {
    return static_cast<T*>(InsertAt(
        index, pdfium::MakeUnique<T>(m_pPool, std::forward<Args>(args)...)));
  }

  void Clear();
  void RemoveAt(size_t index);
  void ConvertToIndirectObjectAt(size_t index, CPDF_IndirectObjectHolder* pDoc);
  bool IsLocked() const { return !!m_LockCount; }

 private:
  friend class CPDF_ArrayLocker;

  std::unique_ptr<CPDF_Object> CloneNonCyclic(
      bool bDirect,
      std::set<const CPDF_Object*>* pVisited) const override;

  std::vector<std::unique_ptr<CPDF_Object>> m_Objects;
  WeakPtr<ByteStringPool> m_pPool;
  mutable uint32_t m_LockCount = 0;
};

class CPDF_ArrayLocker {
 public:
  using const_iterator = CPDF_Array::const_iterator;

  explicit CPDF_ArrayLocker(const CPDF_Array* pArray);
  ~CPDF_ArrayLocker();

  const_iterator begin() const {
    CHECK(m_pArray->IsLocked());
    return m_pArray->m_Objects.begin();
  }
  const_iterator end() const {
    CHECK(m_pArray->IsLocked());
    return m_pArray->m_Objects.end();
  }

 private:
  UnownedPtr<const CPDF_Array> const m_pArray;
};

inline CPDF_Array* ToArray(CPDF_Object* obj) {
  return obj ? obj->AsArray() : nullptr;
}

inline const CPDF_Array* ToArray(const CPDF_Object* obj) {
  return obj ? obj->AsArray() : nullptr;
}

inline std::unique_ptr<CPDF_Array> ToArray(std::unique_ptr<CPDF_Object> obj) {
  CPDF_Array* pArray = ToArray(obj.get());
  if (!pArray)
    return nullptr;
  obj.release();
  return std::unique_ptr<CPDF_Array>(pArray);
}

#endif  // CORE_FPDFAPI_PARSER_CPDF_ARRAY_H_
