// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_PAGE_CPDF_EXPINTFUNC_H_
#define CORE_FPDFAPI_PAGE_CPDF_EXPINTFUNC_H_

#include <set>
#include <vector>

#include "core/fpdfapi/page/cpdf_function.h"

class CPDF_ExpIntFunc final : public CPDF_Function {
 public:
  CPDF_ExpIntFunc();
  ~CPDF_ExpIntFunc() override;

  // CPDF_Function
  bool v_Init(const CPDF_Object* pObj,
              std::set<const CPDF_Object*>* pVisited) override;
  bool v_Call(const float* inputs, float* results) const override;

  uint32_t m_nOrigOutputs = 0;
  float m_Exponent = 0.0f;
  std::vector<float> m_BeginValues;
  std::vector<float> m_EndValues;
};

#endif  // CORE_FPDFAPI_PAGE_CPDF_EXPINTFUNC_H_
