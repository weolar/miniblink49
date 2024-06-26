// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/parser/cpdf_indirect_object_holder.h"

#include <algorithm>
#include <utility>

#include "core/fpdfapi/parser/cpdf_object.h"
#include "core/fpdfapi/parser/cpdf_parser.h"
#include "third_party/base/logging.h"
#include "third_party/base/ptr_util.h"

namespace {

CPDF_Object* FilterInvalidObjNum(CPDF_Object* obj) {
  return obj && obj->GetObjNum() != CPDF_Object::kInvalidObjNum ? obj : nullptr;
}

}  // namespace

CPDF_IndirectObjectHolder::CPDF_IndirectObjectHolder()
    : m_LastObjNum(0),
      m_pByteStringPool(pdfium::MakeUnique<ByteStringPool>()) {}

CPDF_IndirectObjectHolder::~CPDF_IndirectObjectHolder() {
  m_pByteStringPool.DeleteObject();  // Make weak.
}

CPDF_Object* CPDF_IndirectObjectHolder::GetIndirectObject(
    uint32_t objnum) const {
  auto it = m_IndirectObjs.find(objnum);
  return (it != m_IndirectObjs.end()) ? FilterInvalidObjNum(it->second.get())
                                      : nullptr;
}

CPDF_Object* CPDF_IndirectObjectHolder::GetOrParseIndirectObject(
    uint32_t objnum) {
  if (objnum == 0 || objnum == CPDF_Object::kInvalidObjNum)
    return nullptr;

  // Add item anyway to prevent recursively parsing of same object.
  auto insert_result = m_IndirectObjs.insert(std::make_pair(objnum, nullptr));
  if (!insert_result.second)
    return FilterInvalidObjNum(insert_result.first->second.get());

  std::unique_ptr<CPDF_Object> pNewObj = ParseIndirectObject(objnum);
  if (!pNewObj) {
    m_IndirectObjs.erase(insert_result.first);
    return nullptr;
  }

  pNewObj->SetObjNum(objnum);
  m_LastObjNum = std::max(m_LastObjNum, objnum);
  insert_result.first->second = std::move(pNewObj);
  return insert_result.first->second.get();
}

std::unique_ptr<CPDF_Object> CPDF_IndirectObjectHolder::ParseIndirectObject(
    uint32_t objnum) {
  return nullptr;
}

CPDF_Object* CPDF_IndirectObjectHolder::AddIndirectObject(
    std::unique_ptr<CPDF_Object> pObj) {
  CHECK(!pObj->GetObjNum());
  pObj->SetObjNum(++m_LastObjNum);

  auto& obj_holder = m_IndirectObjs[m_LastObjNum];
  if (obj_holder)
    m_OrphanObjs.push_back(std::move(obj_holder));

  obj_holder = std::move(pObj);
  return obj_holder.get();
}

bool CPDF_IndirectObjectHolder::ReplaceIndirectObjectIfHigherGeneration(
    uint32_t objnum,
    std::unique_ptr<CPDF_Object> pObj) {
  ASSERT(objnum);
  if (!pObj || objnum == CPDF_Object::kInvalidObjNum)
    return false;

  auto& obj_holder = m_IndirectObjs[objnum];
  const CPDF_Object* old_object = FilterInvalidObjNum(obj_holder.get());
  if (old_object && pObj->GetGenNum() <= old_object->GetGenNum())
    return false;

  pObj->SetObjNum(objnum);
  AddOrphan(std::move(obj_holder));
  obj_holder = std::move(pObj);
  m_LastObjNum = std::max(m_LastObjNum, objnum);
  return true;
}

void CPDF_IndirectObjectHolder::DeleteIndirectObject(uint32_t objnum) {
  auto it = m_IndirectObjs.find(objnum);
  if (it == m_IndirectObjs.end() || !FilterInvalidObjNum(it->second.get()))
    return;

  m_IndirectObjs.erase(it);
}

void CPDF_IndirectObjectHolder::AddOrphan(
    std::unique_ptr<CPDF_Object> pObject) {
  if (pObject)
    m_OrphanObjs.push_back(std::move(pObject));
}
