// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/parser/cpdf_reference.h"

#include "core/fpdfapi/parser/cpdf_indirect_object_holder.h"
#include "core/fxcrt/fx_stream.h"
#include "third_party/base/ptr_util.h"
#include "third_party/base/stl_util.h"

CPDF_Reference::CPDF_Reference(CPDF_IndirectObjectHolder* pDoc, uint32_t objnum)
    : m_pObjList(pDoc), m_RefObjNum(objnum) {}

CPDF_Reference::~CPDF_Reference() {}

CPDF_Object::Type CPDF_Reference::GetType() const {
  return kReference;
}

ByteString CPDF_Reference::GetString() const {
  const CPDF_Object* obj = SafeGetDirect();
  return obj ? obj->GetString() : ByteString();
}

float CPDF_Reference::GetNumber() const {
  const CPDF_Object* obj = SafeGetDirect();
  return obj ? obj->GetNumber() : 0;
}

int CPDF_Reference::GetInteger() const {
  const CPDF_Object* obj = SafeGetDirect();
  return obj ? obj->GetInteger() : 0;
}

CPDF_Dictionary* CPDF_Reference::GetDict() {
  CPDF_Object* obj = SafeGetDirect();
  return obj ? obj->GetDict() : nullptr;
}

const CPDF_Dictionary* CPDF_Reference::GetDict() const {
  const CPDF_Object* obj = SafeGetDirect();
  return obj ? obj->GetDict() : nullptr;
}

bool CPDF_Reference::IsReference() const {
  return true;
}

CPDF_Reference* CPDF_Reference::AsReference() {
  return this;
}

const CPDF_Reference* CPDF_Reference::AsReference() const {
  return this;
}

std::unique_ptr<CPDF_Object> CPDF_Reference::Clone() const {
  return CloneObjectNonCyclic(false);
}

std::unique_ptr<CPDF_Object> CPDF_Reference::CloneNonCyclic(
    bool bDirect,
    std::set<const CPDF_Object*>* pVisited) const {
  pVisited->insert(this);
  if (bDirect) {
    auto* pDirect = GetDirect();
    return pDirect && !pdfium::ContainsKey(*pVisited, pDirect)
               ? pDirect->CloneNonCyclic(true, pVisited)
               : nullptr;
  }
  return pdfium::MakeUnique<CPDF_Reference>(m_pObjList.Get(), m_RefObjNum);
}

CPDF_Object* CPDF_Reference::SafeGetDirect() {
  CPDF_Object* obj = GetDirect();
  return (obj && !obj->IsReference()) ? obj : nullptr;
}

const CPDF_Object* CPDF_Reference::SafeGetDirect() const {
  const CPDF_Object* obj = GetDirect();
  return (obj && !obj->IsReference()) ? obj : nullptr;
}

void CPDF_Reference::SetRef(CPDF_IndirectObjectHolder* pDoc, uint32_t objnum) {
  m_pObjList = pDoc;
  m_RefObjNum = objnum;
}

CPDF_Object* CPDF_Reference::GetDirect() {
  return m_pObjList ? m_pObjList->GetOrParseIndirectObject(m_RefObjNum)
                    : nullptr;
}

const CPDF_Object* CPDF_Reference::GetDirect() const {
  return m_pObjList ? m_pObjList->GetOrParseIndirectObject(m_RefObjNum)
                    : nullptr;
}

bool CPDF_Reference::WriteTo(IFX_ArchiveStream* archive,
                             const CPDF_Encryptor* encryptor) const {
  return archive->WriteString(" ") && archive->WriteDWord(GetRefObjNum()) &&
         archive->WriteString(" 0 R ");
}

std::unique_ptr<CPDF_Object> CPDF_Reference::MakeReference(
    CPDF_IndirectObjectHolder* holder) const {
  ASSERT(holder == m_pObjList.Get());
  // Do not allow reference to reference, just create other reference for same
  // object.
  return pdfium::MakeUnique<CPDF_Reference>(holder, GetRefObjNum());
}
