// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/parser/cpdf_dictionary.h"

#include <set>
#include <utility>

#include "core/fpdfapi/parser/cpdf_array.h"
#include "core/fpdfapi/parser/cpdf_boolean.h"
#include "core/fpdfapi/parser/cpdf_crypto_handler.h"
#include "core/fpdfapi/parser/cpdf_name.h"
#include "core/fpdfapi/parser/cpdf_number.h"
#include "core/fpdfapi/parser/cpdf_reference.h"
#include "core/fpdfapi/parser/cpdf_stream.h"
#include "core/fpdfapi/parser/cpdf_string.h"
#include "core/fpdfapi/parser/fpdf_parser_utility.h"
#include "core/fxcrt/fx_stream.h"
#include "third_party/base/logging.h"
#include "third_party/base/ptr_util.h"
#include "third_party/base/stl_util.h"

CPDF_Dictionary::CPDF_Dictionary()
    : CPDF_Dictionary(WeakPtr<ByteStringPool>()) {}

CPDF_Dictionary::CPDF_Dictionary(const WeakPtr<ByteStringPool>& pPool)
    : m_pPool(pPool) {}

CPDF_Dictionary::~CPDF_Dictionary() {
  // Mark the object as deleted so that it will not be deleted again,
  // and break cyclic references.
  m_ObjNum = kInvalidObjNum;
  for (auto& it : m_Map) {
    if (it.second && it.second->GetObjNum() == kInvalidObjNum)
      it.second.release();
  }
}

CPDF_Object::Type CPDF_Dictionary::GetType() const {
  return kDictionary;
}

CPDF_Dictionary* CPDF_Dictionary::GetDict() {
  return this;
}

const CPDF_Dictionary* CPDF_Dictionary::GetDict() const {
  return this;
}

bool CPDF_Dictionary::IsDictionary() const {
  return true;
}

CPDF_Dictionary* CPDF_Dictionary::AsDictionary() {
  return this;
}

const CPDF_Dictionary* CPDF_Dictionary::AsDictionary() const {
  return this;
}

std::unique_ptr<CPDF_Object> CPDF_Dictionary::Clone() const {
  return CloneObjectNonCyclic(false);
}

std::unique_ptr<CPDF_Object> CPDF_Dictionary::CloneNonCyclic(
    bool bDirect,
    std::set<const CPDF_Object*>* pVisited) const {
  pVisited->insert(this);
  auto pCopy = pdfium::MakeUnique<CPDF_Dictionary>(m_pPool);
  CPDF_DictionaryLocker locker(this);
  for (const auto& it : locker) {
    if (!pdfium::ContainsKey(*pVisited, it.second.get())) {
      std::set<const CPDF_Object*> visited(*pVisited);
      if (auto obj = it.second->CloneNonCyclic(bDirect, &visited))
        pCopy->m_Map.insert(std::make_pair(it.first, std::move(obj)));
    }
  }
  return std::move(pCopy);
}

const CPDF_Object* CPDF_Dictionary::GetObjectFor(const ByteString& key) const {
  auto it = m_Map.find(key);
  return it != m_Map.end() ? it->second.get() : nullptr;
}

CPDF_Object* CPDF_Dictionary::GetObjectFor(const ByteString& key) {
  return const_cast<CPDF_Object*>(
      static_cast<const CPDF_Dictionary*>(this)->GetObjectFor(key));
}

const CPDF_Object* CPDF_Dictionary::GetDirectObjectFor(
    const ByteString& key) const {
  const CPDF_Object* p = GetObjectFor(key);
  return p ? p->GetDirect() : nullptr;
}

CPDF_Object* CPDF_Dictionary::GetDirectObjectFor(const ByteString& key) {
  return const_cast<CPDF_Object*>(
      static_cast<const CPDF_Dictionary*>(this)->GetDirectObjectFor(key));
}

ByteString CPDF_Dictionary::GetStringFor(const ByteString& key) const {
  const CPDF_Object* p = GetObjectFor(key);
  return p ? p->GetString() : ByteString();
}

WideString CPDF_Dictionary::GetUnicodeTextFor(const ByteString& key) const {
  const CPDF_Object* p = GetObjectFor(key);
  if (const CPDF_Reference* pRef = ToReference(p))
    p = pRef->GetDirect();
  return p ? p->GetUnicodeText() : WideString();
}

ByteString CPDF_Dictionary::GetStringFor(const ByteString& key,
                                         const ByteString& def) const {
  const CPDF_Object* p = GetObjectFor(key);
  return p ? p->GetString() : ByteString(def);
}

int CPDF_Dictionary::GetIntegerFor(const ByteString& key) const {
  const CPDF_Object* p = GetObjectFor(key);
  return p ? p->GetInteger() : 0;
}

int CPDF_Dictionary::GetIntegerFor(const ByteString& key, int def) const {
  const CPDF_Object* p = GetObjectFor(key);
  return p ? p->GetInteger() : def;
}

float CPDF_Dictionary::GetNumberFor(const ByteString& key) const {
  const CPDF_Object* p = GetObjectFor(key);
  return p ? p->GetNumber() : 0;
}

bool CPDF_Dictionary::GetBooleanFor(const ByteString& key,
                                    bool bDefault) const {
  const CPDF_Object* p = GetObjectFor(key);
  return ToBoolean(p) ? p->GetInteger() != 0 : bDefault;
}

const CPDF_Dictionary* CPDF_Dictionary::GetDictFor(
    const ByteString& key) const {
  const CPDF_Object* p = GetDirectObjectFor(key);
  if (!p)
    return nullptr;
  if (const CPDF_Dictionary* pDict = p->AsDictionary())
    return pDict;
  if (const CPDF_Stream* pStream = p->AsStream())
    return pStream->GetDict();
  return nullptr;
}

CPDF_Dictionary* CPDF_Dictionary::GetDictFor(const ByteString& key) {
  return const_cast<CPDF_Dictionary*>(
      static_cast<const CPDF_Dictionary*>(this)->GetDictFor(key));
}

const CPDF_Array* CPDF_Dictionary::GetArrayFor(const ByteString& key) const {
  return ToArray(GetDirectObjectFor(key));
}

CPDF_Array* CPDF_Dictionary::GetArrayFor(const ByteString& key) {
  return ToArray(GetDirectObjectFor(key));
}

const CPDF_Stream* CPDF_Dictionary::GetStreamFor(const ByteString& key) const {
  return ToStream(GetDirectObjectFor(key));
}

CPDF_Stream* CPDF_Dictionary::GetStreamFor(const ByteString& key) {
  return ToStream(GetDirectObjectFor(key));
}

CFX_FloatRect CPDF_Dictionary::GetRectFor(const ByteString& key) const {
  CFX_FloatRect rect;
  const CPDF_Array* pArray = GetArrayFor(key);
  if (pArray)
    rect = pArray->GetRect();
  return rect;
}

CFX_Matrix CPDF_Dictionary::GetMatrixFor(const ByteString& key) const {
  CFX_Matrix matrix;
  const CPDF_Array* pArray = GetArrayFor(key);
  if (pArray)
    matrix = pArray->GetMatrix();
  return matrix;
}

bool CPDF_Dictionary::KeyExist(const ByteString& key) const {
  return pdfium::ContainsKey(m_Map, key);
}

std::vector<ByteString> CPDF_Dictionary::GetKeys() const {
  std::vector<ByteString> result;
  CPDF_DictionaryLocker locker(this);
  for (const auto& item : locker)
    result.push_back(item.first);
  return result;
}

CPDF_Object* CPDF_Dictionary::SetFor(const ByteString& key,
                                     std::unique_ptr<CPDF_Object> pObj) {
  CHECK(!IsLocked());
  if (!pObj) {
    m_Map.erase(key);
    return nullptr;
  }
  ASSERT(pObj->IsInline());
  CPDF_Object* pRet = pObj.get();
  m_Map[MaybeIntern(key)] = std::move(pObj);
  return pRet;
}

void CPDF_Dictionary::ConvertToIndirectObjectFor(
    const ByteString& key,
    CPDF_IndirectObjectHolder* pHolder) {
  CHECK(!IsLocked());
  auto it = m_Map.find(key);
  if (it == m_Map.end() || it->second->IsReference())
    return;

  CPDF_Object* pObj = pHolder->AddIndirectObject(std::move(it->second));
  it->second = pObj->MakeReference(pHolder);
}

std::unique_ptr<CPDF_Object> CPDF_Dictionary::RemoveFor(const ByteString& key) {
  CHECK(!IsLocked());
  std::unique_ptr<CPDF_Object> result;
  auto it = m_Map.find(key);
  if (it != m_Map.end()) {
    result = std::move(it->second);
    m_Map.erase(it);
  }
  return result;
}

void CPDF_Dictionary::ReplaceKey(const ByteString& oldkey,
                                 const ByteString& newkey) {
  CHECK(!IsLocked());
  auto old_it = m_Map.find(oldkey);
  if (old_it == m_Map.end())
    return;

  auto new_it = m_Map.find(newkey);
  if (new_it == old_it)
    return;

  m_Map[MaybeIntern(newkey)] = std::move(old_it->second);
  m_Map.erase(old_it);
}

void CPDF_Dictionary::SetRectFor(const ByteString& key,
                                 const CFX_FloatRect& rect) {
  CPDF_Array* pArray = SetNewFor<CPDF_Array>(key);
  pArray->AddNew<CPDF_Number>(rect.left);
  pArray->AddNew<CPDF_Number>(rect.bottom);
  pArray->AddNew<CPDF_Number>(rect.right);
  pArray->AddNew<CPDF_Number>(rect.top);
}

void CPDF_Dictionary::SetMatrixFor(const ByteString& key,
                                   const CFX_Matrix& matrix) {
  CPDF_Array* pArray = SetNewFor<CPDF_Array>(key);
  pArray->AddNew<CPDF_Number>(matrix.a);
  pArray->AddNew<CPDF_Number>(matrix.b);
  pArray->AddNew<CPDF_Number>(matrix.c);
  pArray->AddNew<CPDF_Number>(matrix.d);
  pArray->AddNew<CPDF_Number>(matrix.e);
  pArray->AddNew<CPDF_Number>(matrix.f);
}

ByteString CPDF_Dictionary::MaybeIntern(const ByteString& str) {
  return m_pPool ? m_pPool->Intern(str) : str;
}

bool CPDF_Dictionary::WriteTo(IFX_ArchiveStream* archive,
                              const CPDF_Encryptor* encryptor) const {
  if (!archive->WriteString("<<"))
    return false;

  const bool is_signature = CPDF_CryptoHandler::IsSignatureDictionary(this);

  CPDF_DictionaryLocker locker(this);
  for (const auto& it : locker) {
    const ByteString& key = it.first;
    CPDF_Object* pValue = it.second.get();
    if (!archive->WriteString("/") ||
        !archive->WriteString(PDF_NameEncode(key).AsStringView())) {
      return false;
    }
    if (!pValue->WriteTo(archive, !is_signature || key != "Contents"
                                      ? encryptor
                                      : nullptr)) {
      return false;
    }
  }
  return archive->WriteString(">>");
}

CPDF_DictionaryLocker::CPDF_DictionaryLocker(const CPDF_Dictionary* pDictionary)
    : m_pDictionary(pDictionary) {
  m_pDictionary->m_LockCount++;
}

CPDF_DictionaryLocker::~CPDF_DictionaryLocker() {
  m_pDictionary->m_LockCount--;
}
