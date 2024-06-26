// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/page/cpdf_function.h"

#include <vector>

#include "core/fpdfapi/page/cpdf_expintfunc.h"
#include "core/fpdfapi/page/cpdf_psfunc.h"
#include "core/fpdfapi/page/cpdf_sampledfunc.h"
#include "core/fpdfapi/page/cpdf_stitchfunc.h"
#include "core/fpdfapi/parser/cpdf_array.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_stream.h"
#include "third_party/base/ptr_util.h"
#include "third_party/base/stl_util.h"

// static
std::unique_ptr<CPDF_Function> CPDF_Function::Load(
    const CPDF_Object* pFuncObj) {
  std::set<const CPDF_Object*> visited;
  return Load(pFuncObj, &visited);
}

// static
std::unique_ptr<CPDF_Function> CPDF_Function::Load(
    const CPDF_Object* pFuncObj,
    std::set<const CPDF_Object*>* pVisited) {
  if (!pFuncObj)
    return nullptr;

  if (pdfium::ContainsKey(*pVisited, pFuncObj))
    return nullptr;
  pdfium::ScopedSetInsertion<const CPDF_Object*> insertion(pVisited, pFuncObj);

  int iType = -1;
  if (const CPDF_Stream* pStream = pFuncObj->AsStream())
    iType = pStream->GetDict()->GetIntegerFor("FunctionType");
  else if (const CPDF_Dictionary* pDict = pFuncObj->AsDictionary())
    iType = pDict->GetIntegerFor("FunctionType");

  std::unique_ptr<CPDF_Function> pFunc;
  Type type = IntegerToFunctionType(iType);
  if (type == Type::kType0Sampled)
    pFunc = pdfium::MakeUnique<CPDF_SampledFunc>();
  else if (type == Type::kType2ExpotentialInterpolation)
    pFunc = pdfium::MakeUnique<CPDF_ExpIntFunc>();
  else if (type == Type::kType3Stitching)
    pFunc = pdfium::MakeUnique<CPDF_StitchFunc>();
  else if (type == Type::kType4PostScript)
    pFunc = pdfium::MakeUnique<CPDF_PSFunc>();

  if (!pFunc || !pFunc->Init(pFuncObj, pVisited))
    return nullptr;

  return pFunc;
}

// static
CPDF_Function::Type CPDF_Function::IntegerToFunctionType(int iType) {
  switch (iType) {
    case 0:
    case 2:
    case 3:
    case 4:
      return static_cast<Type>(iType);
    default:
      return Type::kTypeInvalid;
  }
}

CPDF_Function::CPDF_Function(Type type) : m_Type(type) {}

CPDF_Function::~CPDF_Function() = default;

bool CPDF_Function::Init(const CPDF_Object* pObj,
                         std::set<const CPDF_Object*>* pVisited) {
  const CPDF_Stream* pStream = pObj->AsStream();
  const CPDF_Dictionary* pDict =
      pStream ? pStream->GetDict() : pObj->AsDictionary();

  const CPDF_Array* pDomains = pDict->GetArrayFor("Domain");
  if (!pDomains)
    return false;

  m_nInputs = pDomains->size() / 2;
  if (m_nInputs == 0)
    return false;

  size_t nInputs = m_nInputs * 2;
  m_Domains = std::vector<float>(nInputs);
  for (size_t i = 0; i < nInputs; ++i)
    m_Domains[i] = pDomains->GetFloatAt(i);

  const CPDF_Array* pRanges = pDict->GetArrayFor("Range");
  m_nOutputs = pRanges ? pRanges->size() / 2 : 0;

  // Ranges are required for type 0 and type 4 functions. A non-zero
  // |m_nOutputs| here implied Ranges meets the requirements.
  bool bRangeRequired =
      m_Type == Type::kType0Sampled || m_Type == Type::kType4PostScript;
  if (bRangeRequired && m_nOutputs == 0)
    return false;

  if (m_nOutputs > 0) {
    size_t nOutputs = m_nOutputs * 2;
    m_Ranges = std::vector<float>(nOutputs);
    for (size_t i = 0; i < nOutputs; ++i)
      m_Ranges[i] = pRanges->GetFloatAt(i);
  }

  uint32_t old_outputs = m_nOutputs;
  if (!v_Init(pObj, pVisited))
    return false;

  if (!m_Ranges.empty() && m_nOutputs > old_outputs) {
    FX_SAFE_SIZE_T nOutputs = m_nOutputs;
    nOutputs *= 2;
    m_Ranges.resize(nOutputs.ValueOrDie());
  }
  return true;
}

bool CPDF_Function::Call(const float* inputs,
                         uint32_t ninputs,
                         float* results,
                         int* nresults) const {
  if (m_nInputs != ninputs)
    return false;

  *nresults = m_nOutputs;
  std::vector<float> clamped_inputs(m_nInputs);
  for (uint32_t i = 0; i < m_nInputs; i++) {
    clamped_inputs[i] =
        pdfium::clamp(inputs[i], m_Domains[i * 2], m_Domains[i * 2 + 1]);
  }
  if (!v_Call(clamped_inputs.data(), results))
    return false;

  if (m_Ranges.empty())
    return true;

  for (uint32_t i = 0; i < m_nOutputs; i++) {
    results[i] =
        pdfium::clamp(results[i], m_Ranges[i * 2], m_Ranges[i * 2 + 1]);
  }
  return true;
}

// See PDF Reference 1.7, page 170.
float CPDF_Function::Interpolate(float x,
                                 float xmin,
                                 float xmax,
                                 float ymin,
                                 float ymax) const {
  float divisor = xmax - xmin;
  return ymin + (divisor ? (x - xmin) * (ymax - ymin) / divisor : 0);
}

const CPDF_SampledFunc* CPDF_Function::ToSampledFunc() const {
  return m_Type == Type::kType0Sampled
             ? static_cast<const CPDF_SampledFunc*>(this)
             : nullptr;
}

const CPDF_ExpIntFunc* CPDF_Function::ToExpIntFunc() const {
  return m_Type == Type::kType2ExpotentialInterpolation
             ? static_cast<const CPDF_ExpIntFunc*>(this)
             : nullptr;
}

const CPDF_StitchFunc* CPDF_Function::ToStitchFunc() const {
  return m_Type == Type::kType3Stitching
             ? static_cast<const CPDF_StitchFunc*>(this)
             : nullptr;
}
