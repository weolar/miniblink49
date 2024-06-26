// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CORE_FPDFAPI_PARSER_CPDF_OBJECT_AVAIL_H_
#define CORE_FPDFAPI_PARSER_CPDF_OBJECT_AVAIL_H_

#include <set>
#include <stack>

#include "core/fpdfapi/parser/cpdf_data_avail.h"
#include "core/fxcrt/maybe_owned.h"
#include "core/fxcrt/unowned_ptr.h"

class CPDF_Object;
class CPDF_Reference;
class CPDF_IndirectObjectHolder;
class CPDF_ReadValidator;

// Helper for check availability of object tree.
class CPDF_ObjectAvail {
 public:
  CPDF_ObjectAvail(CPDF_ReadValidator* validator,
                   CPDF_IndirectObjectHolder* holder,
                   const CPDF_Object* root);
  CPDF_ObjectAvail(CPDF_ReadValidator* validator,
                   CPDF_IndirectObjectHolder* holder,
                   uint32_t obj_num);
  virtual ~CPDF_ObjectAvail();

  CPDF_DataAvail::DocAvailStatus CheckAvail();

 protected:
  virtual bool ExcludeObject(const CPDF_Object* object) const;

 private:
  bool LoadRootObject();
  bool CheckObjects();
  bool AppendObjectSubRefs(const CPDF_Object* object,
                           std::stack<uint32_t>* refs) const;
  void CleanMemory();
  bool HasObjectParsed(uint32_t obj_num) const;

  UnownedPtr<CPDF_ReadValidator> validator_;
  UnownedPtr<CPDF_IndirectObjectHolder> holder_;
  MaybeOwned<const CPDF_Object> root_;
  std::set<uint32_t> parsed_objnums_;
  std::stack<uint32_t> non_parsed_objects_;
};

#endif  // CORE_FPDFAPI_PARSER_CPDF_OBJECT_AVAIL_H_
