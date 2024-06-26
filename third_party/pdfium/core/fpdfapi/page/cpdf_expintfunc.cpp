// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/page/cpdf_expintfunc.h"

#include "core/fpdfapi/parser/cpdf_array.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_number.h"
#include "third_party/base/stl_util.h"

CPDF_ExpIntFunc::CPDF_ExpIntFunc()
    : CPDF_Function(Type::kType2ExpotentialInterpolation) {}

CPDF_ExpIntFunc::~CPDF_ExpIntFunc() = default;

bool CPDF_ExpIntFunc::v_Init(const CPDF_Object* pObj,
                             std::set<const CPDF_Object*>* pVisited) {
  const CPDF_Dictionary* pDict = pObj->GetDict();
  if (!pDict)
    return false;

  const CPDF_Number* pExponent = ToNumber(pDict->GetObjectFor("N"));
  if (!pExponent)
    return false;

  m_Exponent = pExponent->GetNumber();

  const CPDF_Array* pArray0 = pDict->GetArrayFor("C0");
  if (pArray0 && m_nOutputs == 0)
    m_nOutputs = pArray0->size();
  if (m_nOutputs == 0)
    m_nOutputs = 1;

  const CPDF_Array* pArray1 = pDict->GetArrayFor("C1");
  m_BeginValues = pdfium::Vector2D<float>(m_nOutputs, 2);
  m_EndValues = pdfium::Vector2D<float>(m_nOutputs, 2);
  for (uint32_t i = 0; i < m_nOutputs; i++) {
    m_BeginValues[i] = pArray0 ? pArray0->GetFloatAt(i) : 0.0f;
    m_EndValues[i] = pArray1 ? pArray1->GetFloatAt(i) : 1.0f;
  }

  FX_SAFE_UINT32 nOutputs = m_nOutputs;
  nOutputs *= m_nInputs;
  if (!nOutputs.IsValid())
    return false;

  m_nOrigOutputs = m_nOutputs;
  m_nOutputs = nOutputs.ValueOrDie();
  return true;
}

bool CPDF_ExpIntFunc::v_Call(const float* inputs, float* results) const {
  for (uint32_t i = 0; i < m_nInputs; i++) {
    for (uint32_t j = 0; j < m_nOrigOutputs; j++) {
      results[i * m_nOrigOutputs + j] =
          m_BeginValues[j] + FXSYS_pow(inputs[i], m_Exponent) *
                                 (m_EndValues[j] - m_BeginValues[j]);
    }
  }
  return true;
}
