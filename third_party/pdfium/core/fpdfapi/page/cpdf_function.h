// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_PAGE_CPDF_FUNCTION_H_
#define CORE_FPDFAPI_PAGE_CPDF_FUNCTION_H_

#include <memory>
#include <set>
#include <vector>

class CPDF_ExpIntFunc;
class CPDF_Object;
class CPDF_SampledFunc;
class CPDF_StitchFunc;

class CPDF_Function {
 public:
  enum class Type {
    kTypeInvalid = -1,
    kType0Sampled = 0,
    kType2ExpotentialInterpolation = 2,
    kType3Stitching = 3,
    kType4PostScript = 4,
  };

  static std::unique_ptr<CPDF_Function> Load(const CPDF_Object* pFuncObj);
  static Type IntegerToFunctionType(int iType);

  virtual ~CPDF_Function();

  bool Call(const float* inputs,
            uint32_t ninputs,
            float* results,
            int* nresults) const;
  uint32_t CountInputs() const { return m_nInputs; }
  uint32_t CountOutputs() const { return m_nOutputs; }
  float GetDomain(int i) const { return m_Domains[i]; }
  float GetRange(int i) const { return m_Ranges[i]; }
  float Interpolate(float x,
                    float xmin,
                    float xmax,
                    float ymin,
                    float ymax) const;

  const CPDF_SampledFunc* ToSampledFunc() const;
  const CPDF_ExpIntFunc* ToExpIntFunc() const;
  const CPDF_StitchFunc* ToStitchFunc() const;

 protected:
  explicit CPDF_Function(Type type);

  static std::unique_ptr<CPDF_Function> Load(
      const CPDF_Object* pFuncObj,
      std::set<const CPDF_Object*>* pVisited);
  bool Init(const CPDF_Object* pObj, std::set<const CPDF_Object*>* pVisited);
  virtual bool v_Init(const CPDF_Object* pObj,
                      std::set<const CPDF_Object*>* pVisited) = 0;
  virtual bool v_Call(const float* inputs, float* results) const = 0;

  const Type m_Type;
  uint32_t m_nInputs;
  uint32_t m_nOutputs;
  std::vector<float> m_Domains;
  std::vector<float> m_Ranges;
};

#endif  // CORE_FPDFAPI_PAGE_CPDF_FUNCTION_H_
