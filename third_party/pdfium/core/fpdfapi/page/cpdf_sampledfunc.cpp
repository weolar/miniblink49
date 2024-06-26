// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/page/cpdf_sampledfunc.h"

#include "core/fpdfapi/parser/cpdf_array.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fxcrt/cfx_bitstream.h"
#include "core/fxcrt/cfx_fixedbufgrow.h"
#include "core/fxcrt/fx_safe_types.h"
#include "third_party/base/stl_util.h"

namespace {

// See PDF Reference 1.7, page 170, table 3.36.
bool IsValidBitsPerSample(uint32_t x) {
  switch (x) {
    case 1:
    case 2:
    case 4:
    case 8:
    case 12:
    case 16:
    case 24:
    case 32:
      return true;
    default:
      return false;
  }
}

}  // namespace

CPDF_SampledFunc::CPDF_SampledFunc() : CPDF_Function(Type::kType0Sampled) {}

CPDF_SampledFunc::~CPDF_SampledFunc() {}

bool CPDF_SampledFunc::v_Init(const CPDF_Object* pObj,
                              std::set<const CPDF_Object*>* pVisited) {
  const CPDF_Stream* pStream = pObj->AsStream();
  if (!pStream)
    return false;

  const CPDF_Dictionary* pDict = pStream->GetDict();
  const CPDF_Array* pSize = pDict->GetArrayFor("Size");
  if (!pSize || pSize->IsEmpty())
    return false;

  m_nBitsPerSample = pDict->GetIntegerFor("BitsPerSample");
  if (!IsValidBitsPerSample(m_nBitsPerSample))
    return false;

  FX_SAFE_UINT32 nTotalSampleBits = m_nBitsPerSample;
  nTotalSampleBits *= m_nOutputs;
  const CPDF_Array* pEncode = pDict->GetArrayFor("Encode");
  m_EncodeInfo.resize(m_nInputs);
  for (uint32_t i = 0; i < m_nInputs; i++) {
    int size = pSize->GetIntegerAt(i);
    if (size <= 0)
      return false;

    m_EncodeInfo[i].sizes = size;
    nTotalSampleBits *= m_EncodeInfo[i].sizes;
    if (pEncode) {
      m_EncodeInfo[i].encode_min = pEncode->GetFloatAt(i * 2);
      m_EncodeInfo[i].encode_max = pEncode->GetFloatAt(i * 2 + 1);
    } else {
      m_EncodeInfo[i].encode_min = 0;
      m_EncodeInfo[i].encode_max =
          m_EncodeInfo[i].sizes == 1 ? 1 : m_EncodeInfo[i].sizes - 1;
    }
  }
  FX_SAFE_UINT32 nTotalSampleBytes = (nTotalSampleBits + 7) / 8;
  if (!nTotalSampleBytes.IsValid() || nTotalSampleBytes.ValueOrDie() == 0)
    return false;

  m_SampleMax = 0xffffffff >> (32 - m_nBitsPerSample);
  m_pSampleStream = pdfium::MakeRetain<CPDF_StreamAcc>(pStream);
  m_pSampleStream->LoadAllDataFiltered();
  if (nTotalSampleBytes.ValueOrDie() > m_pSampleStream->GetSize())
    return false;

  const CPDF_Array* pDecode = pDict->GetArrayFor("Decode");
  m_DecodeInfo.resize(m_nOutputs);
  for (uint32_t i = 0; i < m_nOutputs; i++) {
    if (pDecode) {
      m_DecodeInfo[i].decode_min = pDecode->GetFloatAt(2 * i);
      m_DecodeInfo[i].decode_max = pDecode->GetFloatAt(2 * i + 1);
    } else {
      m_DecodeInfo[i].decode_min = m_Ranges[i * 2];
      m_DecodeInfo[i].decode_max = m_Ranges[i * 2 + 1];
    }
  }
  return true;
}

bool CPDF_SampledFunc::v_Call(const float* inputs, float* results) const {
  int pos = 0;
  CFX_FixedBufGrow<float, 16> encoded_input_buf(m_nInputs);
  float* encoded_input = encoded_input_buf;
  CFX_FixedBufGrow<uint32_t, 32> int_buf(m_nInputs * 2);
  uint32_t* index = int_buf;
  uint32_t* blocksize = index + m_nInputs;
  for (uint32_t i = 0; i < m_nInputs; i++) {
    if (i == 0)
      blocksize[i] = 1;
    else
      blocksize[i] = blocksize[i - 1] * m_EncodeInfo[i - 1].sizes;
    encoded_input[i] =
        Interpolate(inputs[i], m_Domains[i * 2], m_Domains[i * 2 + 1],
                    m_EncodeInfo[i].encode_min, m_EncodeInfo[i].encode_max);
    index[i] = pdfium::clamp(static_cast<uint32_t>(encoded_input[i]), 0U,
                             m_EncodeInfo[i].sizes - 1);
    pos += index[i] * blocksize[i];
  }
  FX_SAFE_INT32 bits_to_output = m_nOutputs;
  bits_to_output *= m_nBitsPerSample;
  if (!bits_to_output.IsValid())
    return false;

  int bits_to_skip;
  {
    FX_SAFE_INT32 bitpos = pos;
    bitpos *= bits_to_output.ValueOrDie();
    bits_to_skip = bitpos.ValueOrDefault(-1);
    if (bits_to_skip < 0)
      return false;

    FX_SAFE_INT32 range_check = bitpos;
    range_check += bits_to_output.ValueOrDie();
    if (!range_check.IsValid())
      return false;
  }

  pdfium::span<const uint8_t> pSampleData = m_pSampleStream->GetSpan();
  if (pSampleData.empty())
    return false;

  CFX_BitStream bitstream(pSampleData);
  bitstream.SkipBits(bits_to_skip);
  for (uint32_t i = 0; i < m_nOutputs; ++i) {
    uint32_t sample = bitstream.GetBits(m_nBitsPerSample);
    float encoded = sample;
    for (uint32_t j = 0; j < m_nInputs; ++j) {
      if (index[j] == m_EncodeInfo[j].sizes - 1) {
        if (index[j] == 0)
          encoded = encoded_input[j] * sample;
      } else {
        FX_SAFE_INT32 bitpos2 = blocksize[j];
        bitpos2 += pos;
        bitpos2 *= m_nOutputs;
        bitpos2 += i;
        bitpos2 *= m_nBitsPerSample;
        int bits_to_skip2 = bitpos2.ValueOrDefault(-1);
        if (bits_to_skip2 < 0)
          return false;

        CFX_BitStream bitstream2(pSampleData);
        bitstream2.SkipBits(bits_to_skip2);
        float sample2 =
            static_cast<float>(bitstream2.GetBits(m_nBitsPerSample));
        encoded += (encoded_input[j] - index[j]) * (sample2 - sample);
      }
    }
    results[i] =
        Interpolate(encoded, 0, m_SampleMax, m_DecodeInfo[i].decode_min,
                    m_DecodeInfo[i].decode_max);
  }
  return true;
}
