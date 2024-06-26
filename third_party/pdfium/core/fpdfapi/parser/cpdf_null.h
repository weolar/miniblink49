// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_PARSER_CPDF_NULL_H_
#define CORE_FPDFAPI_PARSER_CPDF_NULL_H_

#include <memory>

#include "core/fpdfapi/parser/cpdf_object.h"

class CPDF_Null final : public CPDF_Object {
 public:
  CPDF_Null();

  // CPDF_Object.
  Type GetType() const override;
  std::unique_ptr<CPDF_Object> Clone() const override;
  bool WriteTo(IFX_ArchiveStream* archive,
               const CPDF_Encryptor* encryptor) const override;
  bool IsNull() const override;
};

#endif  // CORE_FPDFAPI_PARSER_CPDF_NULL_H_
