// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_PAGE_CPDF_SAMPLEDFUNC_H_
#define CORE_FPDFAPI_PAGE_CPDF_SAMPLEDFUNC_H_

#include <set>
#include <vector>

#include "core/fpdfapi/page/cpdf_function.h"
#include "core/fpdfapi/parser/cpdf_stream_acc.h"
#include "core/fxcrt/retain_ptr.h"

class CPDF_SampledFunc final : public CPDF_Function {
 public:
  struct SampleEncodeInfo {
    float encode_max;
    float encode_min;
    uint32_t sizes;
  };

  struct SampleDecodeInfo {
    float decode_max;
    float decode_min;
  };

  CPDF_SampledFunc();
  ~CPDF_SampledFunc() override;

  // CPDF_Function
  bool v_Init(const CPDF_Object* pObj,
              std::set<const CPDF_Object*>* pVisited) override;
  bool v_Call(const float* inputs, float* results) const override;

  const std::vector<SampleEncodeInfo>& GetEncodeInfo() const {
    return m_EncodeInfo;
  }
  uint32_t GetBitsPerSample() const { return m_nBitsPerSample; }
  RetainPtr<CPDF_StreamAcc> GetSampleStream() const { return m_pSampleStream; }

 private:
  std::vector<SampleEncodeInfo> m_EncodeInfo;
  std::vector<SampleDecodeInfo> m_DecodeInfo;
  uint32_t m_nBitsPerSample;
  uint32_t m_SampleMax;
  RetainPtr<CPDF_StreamAcc> m_pSampleStream;
};

#endif  // CORE_FPDFAPI_PAGE_CPDF_SAMPLEDFUNC_H_
