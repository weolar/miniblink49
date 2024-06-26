// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CORE_FPDFAPI_PARSER_CPDF_PAGE_OBJECT_AVAIL_H_
#define CORE_FPDFAPI_PARSER_CPDF_PAGE_OBJECT_AVAIL_H_

#include "core/fpdfapi/parser/cpdf_object_avail.h"

// Helper for check availability of page's object tree.
// Exclude references to pages.
class CPDF_PageObjectAvail final : public CPDF_ObjectAvail {
 public:
  using CPDF_ObjectAvail::CPDF_ObjectAvail;
  ~CPDF_PageObjectAvail() override;

 private:
  bool ExcludeObject(const CPDF_Object* object) const override;
};

#endif  // CORE_FPDFAPI_PARSER_CPDF_PAGE_OBJECT_AVAIL_H_
