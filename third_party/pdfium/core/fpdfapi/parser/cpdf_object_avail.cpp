// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/fpdfapi/parser/cpdf_object_avail.h"

#include <utility>

#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_indirect_object_holder.h"
#include "core/fpdfapi/parser/cpdf_object_walker.h"
#include "core/fpdfapi/parser/cpdf_read_validator.h"
#include "core/fpdfapi/parser/cpdf_reference.h"
#include "third_party/base/ptr_util.h"

CPDF_ObjectAvail::CPDF_ObjectAvail(CPDF_ReadValidator* validator,
                                   CPDF_IndirectObjectHolder* holder,
                                   const CPDF_Object* root)
    : validator_(validator), holder_(holder), root_(root) {
  ASSERT(validator_);
  ASSERT(holder);
  ASSERT(root_);
  if (!root_->IsInline())
    parsed_objnums_.insert(root_->GetObjNum());
}

CPDF_ObjectAvail::CPDF_ObjectAvail(CPDF_ReadValidator* validator,
                                   CPDF_IndirectObjectHolder* holder,
                                   uint32_t obj_num)
    : validator_(validator),
      holder_(holder),
      root_(pdfium::MakeUnique<CPDF_Reference>(holder, obj_num)) {
  ASSERT(validator_);
  ASSERT(holder);
}

CPDF_ObjectAvail::~CPDF_ObjectAvail() {}

CPDF_DataAvail::DocAvailStatus CPDF_ObjectAvail::CheckAvail() {
  if (!LoadRootObject())
    return CPDF_DataAvail::DocAvailStatus::DataNotAvailable;

  if (CheckObjects()) {
    CleanMemory();
    return CPDF_DataAvail::DocAvailStatus::DataAvailable;
  }
  return CPDF_DataAvail::DocAvailStatus::DataNotAvailable;
}

bool CPDF_ObjectAvail::LoadRootObject() {
  if (!non_parsed_objects_.empty())
    return true;

  while (root_ && root_->IsReference()) {
    const uint32_t ref_obj_num = root_->AsReference()->GetRefObjNum();
    if (HasObjectParsed(ref_obj_num)) {
      root_ = nullptr;
      return true;
    }

    const CPDF_ReadValidator::Session parse_session(validator_.Get());
    const CPDF_Object* direct = holder_->GetOrParseIndirectObject(ref_obj_num);
    if (validator_->has_read_problems())
      return false;

    parsed_objnums_.insert(ref_obj_num);
    root_ = direct;
  }
  std::stack<uint32_t> non_parsed_objects_in_root;
  if (AppendObjectSubRefs(root_.Get(), &non_parsed_objects_in_root)) {
    non_parsed_objects_ = std::move(non_parsed_objects_in_root);
    return true;
  }
  return false;
}

bool CPDF_ObjectAvail::CheckObjects() {
  std::stack<uint32_t> objects_to_check = std::move(non_parsed_objects_);
  std::set<uint32_t> checked_objects;
  while (!objects_to_check.empty()) {
    const uint32_t obj_num = objects_to_check.top();
    objects_to_check.pop();

    if (HasObjectParsed(obj_num))
      continue;

    if (!checked_objects.insert(obj_num).second)
      continue;

    const CPDF_ReadValidator::Session parse_session(validator_.Get());
    const CPDF_Object* direct = holder_->GetOrParseIndirectObject(obj_num);
    if (direct == root_.Get())
      continue;

    if (validator_->has_read_problems() ||
        !AppendObjectSubRefs(direct, &objects_to_check)) {
      non_parsed_objects_.push(obj_num);
      continue;
    }
    parsed_objnums_.insert(obj_num);
  }
  return non_parsed_objects_.empty();
}

bool CPDF_ObjectAvail::AppendObjectSubRefs(const CPDF_Object* object,
                                           std::stack<uint32_t>* refs) const {
  ASSERT(refs);
  if (!object)
    return true;

  CPDF_ObjectWalker walker(object);
  while (const CPDF_Object* obj = walker.GetNext()) {
    const CPDF_ReadValidator::Session parse_session(validator_.Get());

    // Skip if this object if it's an inlined root, the parent object or
    // explicitily excluded.
    const bool skip = (walker.GetParent() && obj == root_.Get()) ||
                      walker.dictionary_key() == "Parent" ||
                      (obj != root_.Get() && ExcludeObject(obj));

    // We need to parse the object before we can do the exclusion check.
    // This is because the exclusion check may check against a referenced
    // field of the object which we need to make sure is loaded.
    if (validator_->has_read_problems())
      return false;

    if (skip) {
      walker.SkipWalkIntoCurrentObject();
      continue;
    }

    if (obj->IsReference())
      refs->push(obj->AsReference()->GetRefObjNum());
  }
  return true;
}

void CPDF_ObjectAvail::CleanMemory() {
  root_.Reset();
  parsed_objnums_.clear();
}

bool CPDF_ObjectAvail::ExcludeObject(const CPDF_Object* object) const {
  return false;
}

bool CPDF_ObjectAvail::HasObjectParsed(uint32_t obj_num) const {
  return parsed_objnums_.count(obj_num) > 0;
}
