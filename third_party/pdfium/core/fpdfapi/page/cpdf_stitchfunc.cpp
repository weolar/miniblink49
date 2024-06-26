// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/page/cpdf_stitchfunc.h"

#include <utility>

#include "core/fpdfapi/parser/cpdf_array.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"

namespace {

constexpr uint32_t kRequiredNumInputs = 1;

}  // namespace

CPDF_StitchFunc::CPDF_StitchFunc() : CPDF_Function(Type::kType3Stitching) {}

CPDF_StitchFunc::~CPDF_StitchFunc() {}

bool CPDF_StitchFunc::v_Init(const CPDF_Object* pObj,
                             std::set<const CPDF_Object*>* pVisited) {
  if (m_nInputs != kRequiredNumInputs)
    return false;

  const CPDF_Dictionary* pDict = pObj->GetDict();
  if (!pDict)
    return false;

  const CPDF_Array* pFunctionsArray = pDict->GetArrayFor("Functions");
  if (!pFunctionsArray)
    return false;

  const CPDF_Array* pBoundsArray = pDict->GetArrayFor("Bounds");
  if (!pBoundsArray)
    return false;

  const CPDF_Array* pEncodeArray = pDict->GetArrayFor("Encode");
  if (!pEncodeArray)
    return false;

  const uint32_t nSubs = pFunctionsArray->size();
  if (nSubs == 0)
    return false;

  // Check array sizes. The checks are slightly relaxed to allow the "Bounds"
  // and "Encode" arrays to have more than the required number of elements.
  {
    if (pBoundsArray->size() < nSubs - 1)
      return false;

    FX_SAFE_UINT32 nExpectedEncodeSize = nSubs;
    nExpectedEncodeSize *= 2;
    if (!nExpectedEncodeSize.IsValid())
      return false;

    if (pEncodeArray->size() < nExpectedEncodeSize.ValueOrDie())
      return false;
  }

  // Check sub-functions.
  {
    Optional<uint32_t> nOutputs;
    for (uint32_t i = 0; i < nSubs; ++i) {
      const CPDF_Object* pSub = pFunctionsArray->GetDirectObjectAt(i);
      if (pSub == pObj)
        return false;

      std::unique_ptr<CPDF_Function> pFunc(CPDF_Function::Load(pSub, pVisited));
      if (!pFunc)
        return false;

      // Check that the input dimensionality is 1, and that all output
      // dimensionalities are the same.
      if (pFunc->CountInputs() != kRequiredNumInputs)
        return false;

      uint32_t nFuncOutputs = pFunc->CountOutputs();
      if (nFuncOutputs == 0)
        return false;

      if (nOutputs) {
        if (nFuncOutputs != *nOutputs)
          return false;
      } else {
        nOutputs = nFuncOutputs;
      }

      m_pSubFunctions.push_back(std::move(pFunc));
    }
    m_nOutputs = *nOutputs;
  }

  m_bounds.reserve(nSubs + 1);
  m_bounds.push_back(m_Domains[0]);
  for (uint32_t i = 0; i < nSubs - 1; i++)
    m_bounds.push_back(pBoundsArray->GetFloatAt(i));
  m_bounds.push_back(m_Domains[1]);

  m_encode.reserve(nSubs * 2);
  for (uint32_t i = 0; i < nSubs * 2; i++)
    m_encode.push_back(pEncodeArray->GetFloatAt(i));
  return true;
}

bool CPDF_StitchFunc::v_Call(const float* inputs, float* results) const {
  float input = inputs[0];
  size_t i;
  for (i = 0; i < m_pSubFunctions.size() - 1; i++) {
    if (input < m_bounds[i + 1])
      break;
  }
  input = Interpolate(input, m_bounds[i], m_bounds[i + 1], m_encode[i * 2],
                      m_encode[i * 2 + 1]);
  int nresults;
  return m_pSubFunctions[i]->Call(&input, kRequiredNumInputs, results,
                                  &nresults);
}
