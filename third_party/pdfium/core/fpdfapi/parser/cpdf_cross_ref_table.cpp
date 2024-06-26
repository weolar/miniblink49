// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/fpdfapi/parser/cpdf_cross_ref_table.h"

#include <utility>
#include <vector>

#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_parser.h"
#include "third_party/base/stl_util.h"

// static
std::unique_ptr<CPDF_CrossRefTable> CPDF_CrossRefTable::MergeUp(
    std::unique_ptr<CPDF_CrossRefTable> current,
    std::unique_ptr<CPDF_CrossRefTable> top) {
  if (!current)
    return top;

  if (!top)
    return current;

  current->Update(std::move(top));
  return current;
}

CPDF_CrossRefTable::CPDF_CrossRefTable() = default;

CPDF_CrossRefTable::CPDF_CrossRefTable(std::unique_ptr<CPDF_Dictionary> trailer)
    : trailer_(std::move(trailer)) {}

CPDF_CrossRefTable::~CPDF_CrossRefTable() = default;

void CPDF_CrossRefTable::AddCompressed(uint32_t obj_num,
                                       uint32_t archive_obj_num) {
  if (obj_num >= CPDF_Parser::kMaxObjectNumber ||
      archive_obj_num >= CPDF_Parser::kMaxObjectNumber) {
    NOTREACHED();
    return;
  }

  auto& info = objects_info_[obj_num];
  if (info.gennum > 0)
    return;

  if (info.type == ObjectType::kObjStream)
    return;

  info.type = ObjectType::kCompressed;
  info.archive_obj_num = archive_obj_num;
  info.gennum = 0;

  objects_info_[archive_obj_num].type = ObjectType::kObjStream;
}

void CPDF_CrossRefTable::AddNormal(uint32_t obj_num,
                                   uint16_t gen_num,
                                   FX_FILESIZE pos) {
  if (obj_num >= CPDF_Parser::kMaxObjectNumber) {
    NOTREACHED();
    return;
  }

  auto& info = objects_info_[obj_num];
  if (info.gennum > gen_num)
    return;

  if (info.type == ObjectType::kCompressed && gen_num == 0)
    return;

  if (info.type != ObjectType::kObjStream)
    info.type = ObjectType::kNormal;

  info.gennum = gen_num;
  info.pos = pos;
}

void CPDF_CrossRefTable::SetFree(uint32_t obj_num) {
  if (obj_num >= CPDF_Parser::kMaxObjectNumber) {
    NOTREACHED();
    return;
  }

  auto& info = objects_info_[obj_num];
  info.type = ObjectType::kFree;
  info.gennum = 0xFFFF;
  info.pos = 0;
}

void CPDF_CrossRefTable::SetTrailer(std::unique_ptr<CPDF_Dictionary> trailer) {
  trailer_ = std::move(trailer);
}

const CPDF_CrossRefTable::ObjectInfo* CPDF_CrossRefTable::GetObjectInfo(
    uint32_t obj_num) const {
  const auto it = objects_info_.find(obj_num);
  return it != objects_info_.end() ? &it->second : nullptr;
}

void CPDF_CrossRefTable::Update(
    std::unique_ptr<CPDF_CrossRefTable> new_cross_ref) {
  UpdateInfo(std::move(new_cross_ref->objects_info_));
  UpdateTrailer(std::move(new_cross_ref->trailer_));
}

void CPDF_CrossRefTable::ShrinkObjectMap(uint32_t objnum) {
  if (objnum == 0) {
    objects_info_.clear();
    return;
  }

  objects_info_.erase(objects_info_.lower_bound(objnum), objects_info_.end());

  if (!pdfium::ContainsKey(objects_info_, objnum - 1))
    objects_info_[objnum - 1].pos = 0;
}

void CPDF_CrossRefTable::UpdateInfo(
    std::map<uint32_t, ObjectInfo>&& new_objects_info) {
  auto cur_it = objects_info_.begin();
  auto new_it = new_objects_info.begin();
  while (cur_it != objects_info_.end() && new_it != new_objects_info.end()) {
    if (cur_it->first == new_it->first) {
      if (cur_it->second.type == ObjectType::kObjStream &&
          new_it->second.type == ObjectType::kNormal) {
        new_it->second.type = ObjectType::kObjStream;
      }
      ++cur_it;
      ++new_it;
    } else if (cur_it->first < new_it->first) {
      new_objects_info.insert(new_it, *cur_it);
      ++cur_it;
    } else {
      new_it = new_objects_info.lower_bound(cur_it->first);
    }
  }
  for (; cur_it != objects_info_.end(); ++cur_it) {
    new_objects_info.insert(new_objects_info.end(), *cur_it);
  }
  objects_info_ = std::move(new_objects_info);
}

void CPDF_CrossRefTable::UpdateTrailer(
    std::unique_ptr<CPDF_Dictionary> new_trailer) {
  if (!new_trailer)
    return;

  if (!trailer_) {
    trailer_ = std::move(new_trailer);
    return;
  }

  new_trailer->SetFor("XRefStm", trailer_->RemoveFor("XRefStm"));
  new_trailer->SetFor("Prev", trailer_->RemoveFor("Prev"));

  for (const auto& key : new_trailer->GetKeys())
    trailer_->SetFor(key, new_trailer->RemoveFor(key));
}
