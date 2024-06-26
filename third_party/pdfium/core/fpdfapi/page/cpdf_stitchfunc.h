// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_PAGE_CPDF_STITCHFUNC_H_
#define CORE_FPDFAPI_PAGE_CPDF_STITCHFUNC_H_

#include <memory>
#include <set>
#include <vector>

#include "core/fpdfapi/page/cpdf_function.h"

class CPDF_StitchFunc final : public CPDF_Function {
 public:
  CPDF_StitchFunc();
  ~CPDF_StitchFunc() override;

  // CPDF_Function
  bool v_Init(const CPDF_Object* pObj,
              std::set<const CPDF_Object*>* pVisited) override;
  bool v_Call(const float* inputs, float* results) const override;

  const std::vector<std::unique_ptr<CPDF_Function>>& GetSubFunctions() const {
    return m_pSubFunctions;
  }
  float GetBound(size_t i) const { return m_bounds[i]; }

 private:
  std::vector<std::unique_ptr<CPDF_Function>> m_pSubFunctions;
  std::vector<float> m_bounds;
  std::vector<float> m_encode;
};

#endif  // CORE_FPDFAPI_PAGE_CPDF_STITCHFUNC_H_
