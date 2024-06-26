// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_PARSER_CPDF_DICTIONARY_H_
#define CORE_FPDFAPI_PARSER_CPDF_DICTIONARY_H_

#include <map>
#include <memory>
#include <set>
#include <utility>
#include <vector>

#include "core/fpdfapi/parser/cpdf_object.h"
#include "core/fxcrt/fx_coordinates.h"
#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/string_pool_template.h"
#include "core/fxcrt/weak_ptr.h"
#include "third_party/base/logging.h"
#include "third_party/base/ptr_util.h"

class CPDF_IndirectObjectHolder;

class CPDF_Dictionary final : public CPDF_Object {
 public:
  using const_iterator =
      std::map<ByteString, std::unique_ptr<CPDF_Object>>::const_iterator;

  CPDF_Dictionary();
  explicit CPDF_Dictionary(const WeakPtr<ByteStringPool>& pPool);
  ~CPDF_Dictionary() override;

  // CPDF_Object:
  Type GetType() const override;
  std::unique_ptr<CPDF_Object> Clone() const override;
  CPDF_Dictionary* GetDict() override;
  const CPDF_Dictionary* GetDict() const override;
  bool IsDictionary() const override;
  CPDF_Dictionary* AsDictionary() override;
  const CPDF_Dictionary* AsDictionary() const override;
  bool WriteTo(IFX_ArchiveStream* archive,
               const CPDF_Encryptor* encryptor) const override;

  bool IsLocked() const { return !!m_LockCount; }

  size_t size() const { return m_Map.size(); }
  const CPDF_Object* GetObjectFor(const ByteString& key) const;
  CPDF_Object* GetObjectFor(const ByteString& key);
  const CPDF_Object* GetDirectObjectFor(const ByteString& key) const;
  CPDF_Object* GetDirectObjectFor(const ByteString& key);
  ByteString GetStringFor(const ByteString& key) const;
  ByteString GetStringFor(const ByteString& key,
                          const ByteString& default_str) const;
  WideString GetUnicodeTextFor(const ByteString& key) const;
  int GetIntegerFor(const ByteString& key) const;
  int GetIntegerFor(const ByteString& key, int default_int) const;
  bool GetBooleanFor(const ByteString& key, bool bDefault) const;
  float GetNumberFor(const ByteString& key) const;
  const CPDF_Dictionary* GetDictFor(const ByteString& key) const;
  CPDF_Dictionary* GetDictFor(const ByteString& key);
  const CPDF_Stream* GetStreamFor(const ByteString& key) const;
  CPDF_Stream* GetStreamFor(const ByteString& key);
  const CPDF_Array* GetArrayFor(const ByteString& key) const;
  CPDF_Array* GetArrayFor(const ByteString& key);
  CFX_FloatRect GetRectFor(const ByteString& key) const;
  CFX_Matrix GetMatrixFor(const ByteString& key) const;
  float GetFloatFor(const ByteString& key) const { return GetNumberFor(key); }

  bool KeyExist(const ByteString& key) const;
  std::vector<ByteString> GetKeys() const;

  // Set* functions invalidate iterators for the element with the key |key|.
  // Takes ownership of |pObj|, returns an unowned pointer to it.
  CPDF_Object* SetFor(const ByteString& key, std::unique_ptr<CPDF_Object> pObj);

  // Creates a new object owned by the dictionary and returns an unowned
  // pointer to it.
  template <typename T, typename... Args>
  typename std::enable_if<!CanInternStrings<T>::value, T*>::type SetNewFor(
      const ByteString& key,
      Args&&... args) {
    CHECK(!IsLocked());
    return static_cast<T*>(
        SetFor(key, pdfium::MakeUnique<T>(std::forward<Args>(args)...)));
  }
  template <typename T, typename... Args>
  typename std::enable_if<CanInternStrings<T>::value, T*>::type SetNewFor(
      const ByteString& key,
      Args&&... args) {
    CHECK(!IsLocked());
    return static_cast<T*>(SetFor(
        key, pdfium::MakeUnique<T>(m_pPool, std::forward<Args>(args)...)));
  }

  // Convenience functions to convert native objects to array form.
  void SetRectFor(const ByteString& key, const CFX_FloatRect& rect);
  void SetMatrixFor(const ByteString& key, const CFX_Matrix& matrix);

  void ConvertToIndirectObjectFor(const ByteString& key,
                                  CPDF_IndirectObjectHolder* pHolder);

  // Invalidates iterators for the element with the key |key|.
  std::unique_ptr<CPDF_Object> RemoveFor(const ByteString& key);

  // Invalidates iterators for the element with the key |oldkey|.
  void ReplaceKey(const ByteString& oldkey, const ByteString& newkey);

  WeakPtr<ByteStringPool> GetByteStringPool() const { return m_pPool; }

 private:
  friend class CPDF_DictionaryLocker;

  ByteString MaybeIntern(const ByteString& str);
  std::unique_ptr<CPDF_Object> CloneNonCyclic(
      bool bDirect,
      std::set<const CPDF_Object*>* visited) const override;

  mutable uint32_t m_LockCount = 0;
  WeakPtr<ByteStringPool> m_pPool;
  std::map<ByteString, std::unique_ptr<CPDF_Object>> m_Map;
};

class CPDF_DictionaryLocker {
 public:
  using const_iterator = CPDF_Dictionary::const_iterator;

  explicit CPDF_DictionaryLocker(const CPDF_Dictionary* pDictionary);
  ~CPDF_DictionaryLocker();

  const_iterator begin() const {
    CHECK(m_pDictionary->IsLocked());
    return m_pDictionary->m_Map.begin();
  }
  const_iterator end() const {
    CHECK(m_pDictionary->IsLocked());
    return m_pDictionary->m_Map.end();
  }

 private:
  UnownedPtr<const CPDF_Dictionary> const m_pDictionary;
};

inline CPDF_Dictionary* ToDictionary(CPDF_Object* obj) {
  return obj ? obj->AsDictionary() : nullptr;
}

inline const CPDF_Dictionary* ToDictionary(const CPDF_Object* obj) {
  return obj ? obj->AsDictionary() : nullptr;
}

inline std::unique_ptr<CPDF_Dictionary> ToDictionary(
    std::unique_ptr<CPDF_Object> obj) {
  CPDF_Dictionary* pDict = ToDictionary(obj.get());
  if (!pDict)
    return nullptr;
  obj.release();
  return std::unique_ptr<CPDF_Dictionary>(pDict);
}

#endif  // CORE_FPDFAPI_PARSER_CPDF_DICTIONARY_H_
