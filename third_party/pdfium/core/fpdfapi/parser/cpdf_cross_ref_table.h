// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CORE_FPDFAPI_PARSER_CPDF_CROSS_REF_TABLE_H_
#define CORE_FPDFAPI_PARSER_CPDF_CROSS_REF_TABLE_H_

#include <map>
#include <memory>

#include "core/fxcrt/fx_system.h"

class CPDF_Dictionary;

class CPDF_CrossRefTable {
 public:
  enum class ObjectType : uint8_t {
    kFree = 0x00,
    kNormal = 0x01,
    kNotCompressed = kNormal,
    kCompressed = 0x02,
    kObjStream = 0xFF,
    kNull = kObjStream,
  };

  struct ObjectInfo {
    ObjectInfo() : pos(0), type(ObjectType::kFree), gennum(0) {}
    // if type is ObjectType::kCompressed the archive_obj_num should be used.
    // if type is ObjectType::kNotCompressed the pos should be used.
    // In other cases its are unused.
    union {
      FX_FILESIZE pos;
      uint32_t archive_obj_num;
    };
    ObjectType type;
    uint16_t gennum;
  };

  // Merge cross reference tables.  Apply top on current.
  static std::unique_ptr<CPDF_CrossRefTable> MergeUp(
      std::unique_ptr<CPDF_CrossRefTable> current,
      std::unique_ptr<CPDF_CrossRefTable> top);

  CPDF_CrossRefTable();
  explicit CPDF_CrossRefTable(std::unique_ptr<CPDF_Dictionary> trailer);
  ~CPDF_CrossRefTable();

  void AddCompressed(uint32_t obj_num, uint32_t archive_obj_num);
  void AddNormal(uint32_t obj_num, uint16_t gen_num, FX_FILESIZE pos);
  void SetFree(uint32_t obj_num);

  const CPDF_Dictionary* trailer() const { return trailer_.get(); }
  void SetTrailer(std::unique_ptr<CPDF_Dictionary> trailer);

  const ObjectInfo* GetObjectInfo(uint32_t obj_num) const;

  const std::map<uint32_t, ObjectInfo>& objects_info() const {
    return objects_info_;
  }

  void Update(std::unique_ptr<CPDF_CrossRefTable> new_cross_ref);

  void ShrinkObjectMap(uint32_t objnum);

 private:
  void UpdateInfo(std::map<uint32_t, ObjectInfo>&& new_objects_info);
  void UpdateTrailer(std::unique_ptr<CPDF_Dictionary> new_trailer);

  std::unique_ptr<CPDF_Dictionary> trailer_;
  std::map<uint32_t, ObjectInfo> objects_info_;
};

#endif  // CORE_FPDFAPI_PARSER_CPDF_CROSS_REF_TABLE_H_
