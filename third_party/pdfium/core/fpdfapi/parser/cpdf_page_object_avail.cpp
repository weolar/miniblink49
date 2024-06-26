// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/fpdfapi/parser/cpdf_page_object_avail.h"

#include "core/fpdfapi/parser/cpdf_dictionary.h"

CPDF_PageObjectAvail::~CPDF_PageObjectAvail() {}

bool CPDF_PageObjectAvail::ExcludeObject(const CPDF_Object* object) const {
  if (CPDF_ObjectAvail::ExcludeObject(object))
    return true;

  return object->IsDictionary() &&
         object->GetDict()->GetStringFor("Type") == "Page";
}
