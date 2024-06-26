// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/page/cpdf_colorspace.h"

#include <algorithm>
#include <limits>
#include <memory>
#include <utility>
#include <vector>

#include "core/fpdfapi/cpdf_modulemgr.h"
#include "core/fpdfapi/page/cpdf_devicecs.h"
#include "core/fpdfapi/page/cpdf_docpagedata.h"
#include "core/fpdfapi/page/cpdf_function.h"
#include "core/fpdfapi/page/cpdf_iccprofile.h"
#include "core/fpdfapi/page/cpdf_pagemodule.h"
#include "core/fpdfapi/page/cpdf_pattern.h"
#include "core/fpdfapi/page/cpdf_patterncs.h"
#include "core/fpdfapi/parser/cpdf_array.h"
#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_document.h"
#include "core/fpdfapi/parser/cpdf_name.h"
#include "core/fpdfapi/parser/cpdf_object.h"
#include "core/fpdfapi/parser/cpdf_stream.h"
#include "core/fpdfapi/parser/cpdf_stream_acc.h"
#include "core/fpdfapi/parser/cpdf_string.h"
#include "core/fpdfdoc/cpdf_action.h"
#include "core/fxcodec/codec/ccodec_iccmodule.h"
#include "core/fxcodec/fx_codec.h"
#include "core/fxcrt/fx_memory.h"
#include "core/fxcrt/maybe_owned.h"
#include "third_party/base/stl_util.h"

namespace {

const uint8_t g_sRGBSamples1[] = {
    0,   3,   6,   10,  13,  15,  18,  20,  22,  23,  25,  27,  28,  30,  31,
    32,  34,  35,  36,  37,  38,  39,  40,  41,  42,  43,  44,  45,  46,  47,
    48,  49,  49,  50,  51,  52,  53,  53,  54,  55,  56,  56,  57,  58,  58,
    59,  60,  61,  61,  62,  62,  63,  64,  64,  65,  66,  66,  67,  67,  68,
    68,  69,  70,  70,  71,  71,  72,  72,  73,  73,  74,  74,  75,  76,  76,
    77,  77,  78,  78,  79,  79,  79,  80,  80,  81,  81,  82,  82,  83,  83,
    84,  84,  85,  85,  85,  86,  86,  87,  87,  88,  88,  88,  89,  89,  90,
    90,  91,  91,  91,  92,  92,  93,  93,  93,  94,  94,  95,  95,  95,  96,
    96,  97,  97,  97,  98,  98,  98,  99,  99,  99,  100, 100, 101, 101, 101,
    102, 102, 102, 103, 103, 103, 104, 104, 104, 105, 105, 106, 106, 106, 107,
    107, 107, 108, 108, 108, 109, 109, 109, 110, 110, 110, 110, 111, 111, 111,
    112, 112, 112, 113, 113, 113, 114, 114, 114, 115, 115, 115, 115, 116, 116,
    116, 117, 117, 117, 118, 118, 118, 118, 119, 119, 119, 120,
};

const uint8_t g_sRGBSamples2[] = {
    120, 121, 122, 124, 125, 126, 127, 128, 129, 130, 131, 132, 133, 134, 135,
    136, 137, 138, 139, 140, 141, 142, 143, 144, 145, 146, 147, 148, 148, 149,
    150, 151, 152, 153, 154, 155, 155, 156, 157, 158, 159, 159, 160, 161, 162,
    163, 163, 164, 165, 166, 167, 167, 168, 169, 170, 170, 171, 172, 173, 173,
    174, 175, 175, 176, 177, 178, 178, 179, 180, 180, 181, 182, 182, 183, 184,
    185, 185, 186, 187, 187, 188, 189, 189, 190, 190, 191, 192, 192, 193, 194,
    194, 195, 196, 196, 197, 197, 198, 199, 199, 200, 200, 201, 202, 202, 203,
    203, 204, 205, 205, 206, 206, 207, 208, 208, 209, 209, 210, 210, 211, 212,
    212, 213, 213, 214, 214, 215, 215, 216, 216, 217, 218, 218, 219, 219, 220,
    220, 221, 221, 222, 222, 223, 223, 224, 224, 225, 226, 226, 227, 227, 228,
    228, 229, 229, 230, 230, 231, 231, 232, 232, 233, 233, 234, 234, 235, 235,
    236, 236, 237, 237, 238, 238, 238, 239, 239, 240, 240, 241, 241, 242, 242,
    243, 243, 244, 244, 245, 245, 246, 246, 246, 247, 247, 248, 248, 249, 249,
    250, 250, 251, 251, 251, 252, 252, 253, 253, 254, 254, 255, 255,
};

constexpr size_t kBlackWhitePointCount = 3;

void GetDefaultBlackPoint(float* pPoints) {
  static constexpr float kDefaultValue = 0.0f;
  for (size_t i = 0; i < kBlackWhitePointCount; ++i)
    pPoints[i] = kDefaultValue;
}

void GetBlackPoint(const CPDF_Dictionary* pDict, float* pPoints) {
  const CPDF_Array* pParam = pDict->GetArrayFor("BlackPoint");
  if (!pParam || pParam->size() != kBlackWhitePointCount) {
    GetDefaultBlackPoint(pPoints);
    return;
  }

  // Check to make sure all values are non-negative.
  for (size_t i = 0; i < kBlackWhitePointCount; ++i) {
    pPoints[i] = pParam->GetNumberAt(i);
    if (pPoints[i] < 0) {
      GetDefaultBlackPoint(pPoints);
      return;
    }
  }
}

bool GetWhitePoint(const CPDF_Dictionary* pDict, float* pPoints) {
  const CPDF_Array* pParam = pDict->GetArrayFor("WhitePoint");
  if (!pParam || pParam->size() != kBlackWhitePointCount)
    return false;

  for (size_t i = 0; i < kBlackWhitePointCount; ++i)
    pPoints[i] = pParam->GetNumberAt(i);
  return pPoints[0] > 0.0f && pPoints[1] == 1.0f && pPoints[2] > 0.0f;
}

class CPDF_CalGray final : public CPDF_ColorSpace {
 public:
  explicit CPDF_CalGray(CPDF_Document* pDoc);
  ~CPDF_CalGray() override {}

  // CPDF_ColorSpace:
  uint32_t v_Load(CPDF_Document* pDoc,
                  const CPDF_Array* pArray,
                  std::set<const CPDF_Object*>* pVisited) override;
  bool GetRGB(const float* pBuf, float* R, float* G, float* B) const override;
  void TranslateImageLine(uint8_t* pDestBuf,
                          const uint8_t* pSrcBuf,
                          int pixels,
                          int image_width,
                          int image_height,
                          bool bTransMask) const override;

 private:
  static constexpr float kDefaultGamma = 1.0f;

  float m_Gamma = kDefaultGamma;
  float m_WhitePoint[kBlackWhitePointCount];
  float m_BlackPoint[kBlackWhitePointCount];
};

class CPDF_CalRGB final : public CPDF_ColorSpace {
 public:
  explicit CPDF_CalRGB(CPDF_Document* pDoc);
  ~CPDF_CalRGB() override {}

  uint32_t v_Load(CPDF_Document* pDoc,
                  const CPDF_Array* pArray,
                  std::set<const CPDF_Object*>* pVisited) override;

  bool GetRGB(const float* pBuf, float* R, float* G, float* B) const override;

  void TranslateImageLine(uint8_t* pDestBuf,
                          const uint8_t* pSrcBuf,
                          int pixels,
                          int image_width,
                          int image_height,
                          bool bTransMask) const override;

 private:
  static constexpr size_t kGammaCount = 3;
  static constexpr size_t kMatrixCount = 9;

  float m_WhitePoint[kBlackWhitePointCount];
  float m_BlackPoint[kBlackWhitePointCount];
  float m_Gamma[kGammaCount];
  float m_Matrix[kMatrixCount];
  bool m_bGamma = false;
  bool m_bMatrix = false;
};

class CPDF_LabCS final : public CPDF_ColorSpace {
 public:
  explicit CPDF_LabCS(CPDF_Document* pDoc);
  ~CPDF_LabCS() override {}

  uint32_t v_Load(CPDF_Document* pDoc,
                  const CPDF_Array* pArray,
                  std::set<const CPDF_Object*>* pVisited) override;

  void GetDefaultValue(int iComponent,
                       float* value,
                       float* min,
                       float* max) const override;
  bool GetRGB(const float* pBuf, float* R, float* G, float* B) const override;

  void TranslateImageLine(uint8_t* pDestBuf,
                          const uint8_t* pSrcBuf,
                          int pixels,
                          int image_width,
                          int image_height,
                          bool bTransMask) const override;

 private:
  static constexpr size_t kRangesCount = 4;

  float m_WhitePoint[kBlackWhitePointCount];
  float m_BlackPoint[kBlackWhitePointCount];
  float m_Ranges[kRangesCount];
};

class CPDF_ICCBasedCS final : public CPDF_ColorSpace {
 public:
  explicit CPDF_ICCBasedCS(CPDF_Document* pDoc);
  ~CPDF_ICCBasedCS() override;

  // CPDF_ColorSpace:
  uint32_t v_Load(CPDF_Document* pDoc,
                  const CPDF_Array* pArray,
                  std::set<const CPDF_Object*>* pVisited) override;
  bool GetRGB(const float* pBuf, float* R, float* G, float* B) const override;
  void EnableStdConversion(bool bEnabled) override;
  void TranslateImageLine(uint8_t* pDestBuf,
                          const uint8_t* pSrcBuf,
                          int pixels,
                          int image_width,
                          int image_height,
                          bool bTransMask) const override;
  bool IsNormal() const override;

 private:
  // If no valid ICC profile or using sRGB, try looking for an alternate.
  bool FindAlternateProfile(CPDF_Document* pDoc,
                            const CPDF_Dictionary* pDict,
                            std::set<const CPDF_Object*>* pVisited,
                            uint32_t nExpectedComponents);
  static CPDF_ColorSpace* GetStockAlternateProfile(uint32_t nComponents);
  static bool IsValidComponents(int32_t nComps);
  static std::vector<float> GetRanges(const CPDF_Dictionary* pDict,
                                      uint32_t nComponents);

  MaybeOwned<CPDF_ColorSpace> m_pAlterCS;
  RetainPtr<CPDF_IccProfile> m_pProfile;
  mutable std::vector<uint8_t> m_pCache;
  std::vector<float> m_pRanges;
};

class CPDF_IndexedCS final : public CPDF_ColorSpace {
 public:
  explicit CPDF_IndexedCS(CPDF_Document* pDoc);
  ~CPDF_IndexedCS() override;

  uint32_t v_Load(CPDF_Document* pDoc,
                  const CPDF_Array* pArray,
                  std::set<const CPDF_Object*>* pVisited) override;

  bool GetRGB(const float* pBuf, float* R, float* G, float* B) const override;

  void EnableStdConversion(bool bEnabled) override;

 private:
  CPDF_ColorSpace* m_pBaseCS = nullptr;
  UnownedPtr<const CPDF_CountedColorSpace> m_pCountedBaseCS;
  uint32_t m_nBaseComponents = 0;
  int m_MaxIndex = 0;
  ByteString m_Table;
  std::vector<float> m_pCompMinMax;
};

class CPDF_SeparationCS final : public CPDF_ColorSpace {
 public:
  explicit CPDF_SeparationCS(CPDF_Document* pDoc);
  ~CPDF_SeparationCS() override;

  // CPDF_ColorSpace:
  void GetDefaultValue(int iComponent,
                       float* value,
                       float* min,
                       float* max) const override;
  uint32_t v_Load(CPDF_Document* pDoc,
                  const CPDF_Array* pArray,
                  std::set<const CPDF_Object*>* pVisited) override;
  bool GetRGB(const float* pBuf, float* R, float* G, float* B) const override;
  void EnableStdConversion(bool bEnabled) override;

 private:
  std::unique_ptr<CPDF_ColorSpace> m_pAltCS;
  std::unique_ptr<const CPDF_Function> m_pFunc;
  enum { None, All, Colorant } m_Type;
};

class CPDF_DeviceNCS final : public CPDF_ColorSpace {
 public:
  explicit CPDF_DeviceNCS(CPDF_Document* pDoc);
  ~CPDF_DeviceNCS() override;

  // CPDF_ColorSpace:
  void GetDefaultValue(int iComponent,
                       float* value,
                       float* min,
                       float* max) const override;
  uint32_t v_Load(CPDF_Document* pDoc,
                  const CPDF_Array* pArray,
                  std::set<const CPDF_Object*>* pVisited) override;
  bool GetRGB(const float* pBuf, float* R, float* G, float* B) const override;
  void EnableStdConversion(bool bEnabled) override;

 private:
  std::unique_ptr<CPDF_ColorSpace> m_pAltCS;
  std::unique_ptr<const CPDF_Function> m_pFunc;
};

class Vector_3by1 {
 public:
  Vector_3by1() : a(0.0f), b(0.0f), c(0.0f) {}

  Vector_3by1(float a1, float b1, float c1) : a(a1), b(b1), c(c1) {}

  float a;
  float b;
  float c;
};

class Matrix_3by3 {
 public:
  Matrix_3by3()
      : a(0.0f),
        b(0.0f),
        c(0.0f),
        d(0.0f),
        e(0.0f),
        f(0.0f),
        g(0.0f),
        h(0.0f),
        i(0.0f) {}

  Matrix_3by3(float a1,
              float b1,
              float c1,
              float d1,
              float e1,
              float f1,
              float g1,
              float h1,
              float i1)
      : a(a1), b(b1), c(c1), d(d1), e(e1), f(f1), g(g1), h(h1), i(i1) {}

  Matrix_3by3 Inverse() {
    float det = a * (e * i - f * h) - b * (i * d - f * g) + c * (d * h - e * g);
    if (fabs(det) < std::numeric_limits<float>::epsilon())
      return Matrix_3by3();

    return Matrix_3by3(
        (e * i - f * h) / det, -(b * i - c * h) / det, (b * f - c * e) / det,
        -(d * i - f * g) / det, (a * i - c * g) / det, -(a * f - c * d) / det,
        (d * h - e * g) / det, -(a * h - b * g) / det, (a * e - b * d) / det);
  }

  Matrix_3by3 Multiply(const Matrix_3by3& m) {
    return Matrix_3by3(a * m.a + b * m.d + c * m.g, a * m.b + b * m.e + c * m.h,
                       a * m.c + b * m.f + c * m.i, d * m.a + e * m.d + f * m.g,
                       d * m.b + e * m.e + f * m.h, d * m.c + e * m.f + f * m.i,
                       g * m.a + h * m.d + i * m.g, g * m.b + h * m.e + i * m.h,
                       g * m.c + h * m.f + i * m.i);
  }

  Vector_3by1 TransformVector(const Vector_3by1& v) {
    return Vector_3by1(a * v.a + b * v.b + c * v.c, d * v.a + e * v.b + f * v.c,
                       g * v.a + h * v.b + i * v.c);
  }

  float a;
  float b;
  float c;
  float d;
  float e;
  float f;
  float g;
  float h;
  float i;
};

float RGB_Conversion(float colorComponent) {
  colorComponent = pdfium::clamp(colorComponent, 0.0f, 1.0f);
  int scale = std::max(static_cast<int>(colorComponent * 1023), 0);
  if (scale < 192)
    return g_sRGBSamples1[scale] / 255.0f;
  return g_sRGBSamples2[scale / 4 - 48] / 255.0f;
}

void XYZ_to_sRGB(float X, float Y, float Z, float* R, float* G, float* B) {
  float R1 = 3.2410f * X - 1.5374f * Y - 0.4986f * Z;
  float G1 = -0.9692f * X + 1.8760f * Y + 0.0416f * Z;
  float B1 = 0.0556f * X - 0.2040f * Y + 1.0570f * Z;

  *R = RGB_Conversion(R1);
  *G = RGB_Conversion(G1);
  *B = RGB_Conversion(B1);
}

void XYZ_to_sRGB_WhitePoint(float X,
                            float Y,
                            float Z,
                            float Xw,
                            float Yw,
                            float Zw,
                            float* R,
                            float* G,
                            float* B) {
  // The following RGB_xyz is based on
  // sRGB value {Rx,Ry}={0.64, 0.33}, {Gx,Gy}={0.30, 0.60}, {Bx,By}={0.15, 0.06}

  constexpr float Rx = 0.64f;
  constexpr float Ry = 0.33f;
  constexpr float Gx = 0.30f;
  constexpr float Gy = 0.60f;
  constexpr float Bx = 0.15f;
  constexpr float By = 0.06f;
  Matrix_3by3 RGB_xyz(Rx, Gx, Bx, Ry, Gy, By, 1 - Rx - Ry, 1 - Gx - Gy,
                      1 - Bx - By);
  Vector_3by1 whitePoint(Xw, Yw, Zw);
  Vector_3by1 XYZ(X, Y, Z);

  Vector_3by1 RGB_Sum_XYZ = RGB_xyz.Inverse().TransformVector(whitePoint);
  Matrix_3by3 RGB_SUM_XYZ_DIAG(RGB_Sum_XYZ.a, 0, 0, 0, RGB_Sum_XYZ.b, 0, 0, 0,
                               RGB_Sum_XYZ.c);
  Matrix_3by3 M = RGB_xyz.Multiply(RGB_SUM_XYZ_DIAG);
  Vector_3by1 RGB = M.Inverse().TransformVector(XYZ);

  *R = RGB_Conversion(RGB.a);
  *G = RGB_Conversion(RGB.b);
  *B = RGB_Conversion(RGB.c);
}

}  // namespace

// static
CPDF_ColorSpace* CPDF_ColorSpace::ColorspaceFromName(const ByteString& name) {
  if (name == "DeviceRGB" || name == "RGB")
    return CPDF_ColorSpace::GetStockCS(PDFCS_DEVICERGB);
  if (name == "DeviceGray" || name == "G")
    return CPDF_ColorSpace::GetStockCS(PDFCS_DEVICEGRAY);
  if (name == "DeviceCMYK" || name == "CMYK")
    return CPDF_ColorSpace::GetStockCS(PDFCS_DEVICECMYK);
  if (name == "Pattern")
    return CPDF_ColorSpace::GetStockCS(PDFCS_PATTERN);
  return nullptr;
}

// static
CPDF_ColorSpace* CPDF_ColorSpace::GetStockCS(int family) {
  return CPDF_ModuleMgr::Get()->GetPageModule()->GetStockCS(family);
}

// static
std::unique_ptr<CPDF_ColorSpace> CPDF_ColorSpace::Load(CPDF_Document* pDoc,
                                                       CPDF_Object* pObj) {
  std::set<const CPDF_Object*> visited;
  return Load(pDoc, pObj, &visited);
}

// static
std::unique_ptr<CPDF_ColorSpace> CPDF_ColorSpace::Load(
    CPDF_Document* pDoc,
    const CPDF_Object* pObj,
    std::set<const CPDF_Object*>* pVisited) {
  if (!pObj)
    return nullptr;

  if (pdfium::ContainsKey(*pVisited, pObj))
    return nullptr;

  pdfium::ScopedSetInsertion<const CPDF_Object*> insertion(pVisited, pObj);

  if (pObj->IsName())
    return pdfium::WrapUnique(ColorspaceFromName(pObj->GetString()));

  if (const CPDF_Stream* pStream = pObj->AsStream()) {
    const CPDF_Dictionary* pDict = pStream->GetDict();
    if (!pDict)
      return nullptr;

    CPDF_DictionaryLocker locker(pDict);
    for (const auto& it : locker) {
      std::unique_ptr<CPDF_ColorSpace> pRet;
      CPDF_Object* pValue = it.second.get();
      if (ToName(pValue))
        pRet.reset(ColorspaceFromName(pValue->GetString()));
      if (pRet)
        return pRet;
    }
    return nullptr;
  }

  const CPDF_Array* pArray = pObj->AsArray();
  if (!pArray || pArray->IsEmpty())
    return nullptr;

  const CPDF_Object* pFamilyObj = pArray->GetDirectObjectAt(0);
  if (!pFamilyObj)
    return nullptr;

  ByteString familyname = pFamilyObj->GetString();
  if (pArray->size() == 1)
    return pdfium::WrapUnique(ColorspaceFromName(familyname));

  std::unique_ptr<CPDF_ColorSpace> pCS;
  switch (familyname.GetID()) {
    case FXBSTR_ID('C', 'a', 'l', 'G'):
      pCS.reset(new CPDF_CalGray(pDoc));
      break;
    case FXBSTR_ID('C', 'a', 'l', 'R'):
      pCS.reset(new CPDF_CalRGB(pDoc));
      break;
    case FXBSTR_ID('L', 'a', 'b', 0):
      pCS.reset(new CPDF_LabCS(pDoc));
      break;
    case FXBSTR_ID('I', 'C', 'C', 'B'):
      pCS.reset(new CPDF_ICCBasedCS(pDoc));
      break;
    case FXBSTR_ID('I', 'n', 'd', 'e'):
    case FXBSTR_ID('I', 0, 0, 0):
      pCS.reset(new CPDF_IndexedCS(pDoc));
      break;
    case FXBSTR_ID('S', 'e', 'p', 'a'):
      pCS.reset(new CPDF_SeparationCS(pDoc));
      break;
    case FXBSTR_ID('D', 'e', 'v', 'i'):
      pCS.reset(new CPDF_DeviceNCS(pDoc));
      break;
    case FXBSTR_ID('P', 'a', 't', 't'):
      pCS.reset(new CPDF_PatternCS(pDoc));
      break;
    default:
      return nullptr;
  }
  pCS->m_pArray = pArray;
  pCS->m_nComponents = pCS->v_Load(pDoc, pArray, pVisited);
  if (pCS->m_nComponents == 0)
    return nullptr;

  return pCS;
}

void CPDF_ColorSpace::Release() {
  if (this == GetStockCS(PDFCS_DEVICERGB) ||
      this == GetStockCS(PDFCS_DEVICEGRAY) ||
      this == GetStockCS(PDFCS_DEVICECMYK) ||
      this == GetStockCS(PDFCS_PATTERN)) {
    return;
  }
  delete this;
}

size_t CPDF_ColorSpace::GetBufSize() const {
  if (m_Family == PDFCS_PATTERN)
    return sizeof(PatternValue);
  return m_nComponents * sizeof(float);
}

float* CPDF_ColorSpace::CreateBuf() const {
  return reinterpret_cast<float*>(FX_Alloc(uint8_t, GetBufSize()));
}

float* CPDF_ColorSpace::CreateBufAndSetDefaultColor() const {
  ASSERT(m_Family != PDFCS_PATTERN);

  float* buf = CreateBuf();
  float min;
  float max;
  for (uint32_t i = 0; i < m_nComponents; i++)
    GetDefaultValue(i, &buf[i], &min, &max);
  return buf;
}

uint32_t CPDF_ColorSpace::CountComponents() const {
  return m_nComponents;
}

void CPDF_ColorSpace::GetDefaultValue(int iComponent,
                                      float* value,
                                      float* min,
                                      float* max) const {
  *value = 0.0f;
  *min = 0.0f;
  *max = 1.0f;
}

void CPDF_ColorSpace::TranslateImageLine(uint8_t* dest_buf,
                                         const uint8_t* src_buf,
                                         int pixels,
                                         int image_width,
                                         int image_height,
                                         bool bTransMask) const {
  std::vector<float> src(m_nComponents);
  float R;
  float G;
  float B;
  const int divisor = m_Family != PDFCS_INDEXED ? 255 : 1;
  for (int i = 0; i < pixels; i++) {
    for (uint32_t j = 0; j < m_nComponents; j++)
      src[j] = static_cast<float>(*src_buf++) / divisor;
    GetRGB(src.data(), &R, &G, &B);
    *dest_buf++ = static_cast<int32_t>(B * 255);
    *dest_buf++ = static_cast<int32_t>(G * 255);
    *dest_buf++ = static_cast<int32_t>(R * 255);
  }
}

void CPDF_ColorSpace::EnableStdConversion(bool bEnabled) {
  if (bEnabled)
    m_dwStdConversion++;
  else if (m_dwStdConversion)
    m_dwStdConversion--;
}

bool CPDF_ColorSpace::IsNormal() const {
  return GetFamily() == PDFCS_DEVICEGRAY || GetFamily() == PDFCS_DEVICERGB ||
         GetFamily() == PDFCS_DEVICECMYK || GetFamily() == PDFCS_CALGRAY ||
         GetFamily() == PDFCS_CALRGB;
}

CPDF_PatternCS* CPDF_ColorSpace::AsPatternCS() {
  NOTREACHED();
  return nullptr;
}

const CPDF_PatternCS* CPDF_ColorSpace::AsPatternCS() const {
  NOTREACHED();
  return nullptr;
}

bool CPDF_ColorSpace::GetPatternRGB(const PatternValue& value,
                                    float* R,
                                    float* G,
                                    float* B) const {
  NOTREACHED();
  return false;
}

CPDF_ColorSpace::CPDF_ColorSpace(CPDF_Document* pDoc, int family)
    : m_pDocument(pDoc), m_Family(family) {}

CPDF_ColorSpace::~CPDF_ColorSpace() {}

void CPDF_ColorSpace::SetComponentsForStockCS(uint32_t nComponents) {
  ASSERT(!m_pDocument);  // Stock colorspace is not associated with a document.
  m_nComponents = nComponents;
}

CPDF_CalGray::CPDF_CalGray(CPDF_Document* pDoc)
    : CPDF_ColorSpace(pDoc, PDFCS_CALGRAY) {}

uint32_t CPDF_CalGray::v_Load(CPDF_Document* pDoc,
                              const CPDF_Array* pArray,
                              std::set<const CPDF_Object*>* pVisited) {
  const CPDF_Dictionary* pDict = pArray->GetDictAt(1);
  if (!pDict)
    return 0;

  if (!GetWhitePoint(pDict, m_WhitePoint))
    return 0;

  GetBlackPoint(pDict, m_BlackPoint);

  m_Gamma = pDict->GetNumberFor("Gamma");
  if (m_Gamma == 0)
    m_Gamma = kDefaultGamma;
  return 1;
}

bool CPDF_CalGray::GetRGB(const float* pBuf,
                          float* R,
                          float* G,
                          float* B) const {
  *R = *pBuf;
  *G = *pBuf;
  *B = *pBuf;
  return true;
}

void CPDF_CalGray::TranslateImageLine(uint8_t* pDestBuf,
                                      const uint8_t* pSrcBuf,
                                      int pixels,
                                      int image_width,
                                      int image_height,
                                      bool bTransMask) const {
  for (int i = 0; i < pixels; i++) {
    *pDestBuf++ = pSrcBuf[i];
    *pDestBuf++ = pSrcBuf[i];
    *pDestBuf++ = pSrcBuf[i];
  }
}

CPDF_CalRGB::CPDF_CalRGB(CPDF_Document* pDoc)
    : CPDF_ColorSpace(pDoc, PDFCS_CALRGB) {}

uint32_t CPDF_CalRGB::v_Load(CPDF_Document* pDoc,
                             const CPDF_Array* pArray,
                             std::set<const CPDF_Object*>* pVisited) {
  const CPDF_Dictionary* pDict = pArray->GetDictAt(1);
  if (!pDict)
    return 0;

  if (!GetWhitePoint(pDict, m_WhitePoint))
    return 0;

  GetBlackPoint(pDict, m_BlackPoint);

  const CPDF_Array* pParam = pDict->GetArrayFor("Gamma");
  if (pParam) {
    m_bGamma = true;
    for (size_t i = 0; i < FX_ArraySize(m_Gamma); ++i)
      m_Gamma[i] = pParam->GetNumberAt(i);
  }

  pParam = pDict->GetArrayFor("Matrix");
  if (pParam) {
    m_bMatrix = true;
    for (size_t i = 0; i < FX_ArraySize(m_Matrix); ++i)
      m_Matrix[i] = pParam->GetNumberAt(i);
  }
  return 3;
}

bool CPDF_CalRGB::GetRGB(const float* pBuf,
                         float* R,
                         float* G,
                         float* B) const {
  float A_ = pBuf[0];
  float B_ = pBuf[1];
  float C_ = pBuf[2];
  if (m_bGamma) {
    A_ = FXSYS_pow(A_, m_Gamma[0]);
    B_ = FXSYS_pow(B_, m_Gamma[1]);
    C_ = FXSYS_pow(C_, m_Gamma[2]);
  }

  float X;
  float Y;
  float Z;
  if (m_bMatrix) {
    X = m_Matrix[0] * A_ + m_Matrix[3] * B_ + m_Matrix[6] * C_;
    Y = m_Matrix[1] * A_ + m_Matrix[4] * B_ + m_Matrix[7] * C_;
    Z = m_Matrix[2] * A_ + m_Matrix[5] * B_ + m_Matrix[8] * C_;
  } else {
    X = A_;
    Y = B_;
    Z = C_;
  }
  XYZ_to_sRGB_WhitePoint(X, Y, Z, m_WhitePoint[0], m_WhitePoint[1],
                         m_WhitePoint[2], R, G, B);
  return true;
}

void CPDF_CalRGB::TranslateImageLine(uint8_t* pDestBuf,
                                     const uint8_t* pSrcBuf,
                                     int pixels,
                                     int image_width,
                                     int image_height,
                                     bool bTransMask) const {
  if (bTransMask) {
    float Cal[3];
    float R;
    float G;
    float B;
    for (int i = 0; i < pixels; i++) {
      Cal[0] = static_cast<float>(pSrcBuf[2]) / 255;
      Cal[1] = static_cast<float>(pSrcBuf[1]) / 255;
      Cal[2] = static_cast<float>(pSrcBuf[0]) / 255;
      GetRGB(Cal, &R, &G, &B);
      pDestBuf[0] = FXSYS_round(B * 255);
      pDestBuf[1] = FXSYS_round(G * 255);
      pDestBuf[2] = FXSYS_round(R * 255);
      pSrcBuf += 3;
      pDestBuf += 3;
    }
  }
  ReverseRGB(pDestBuf, pSrcBuf, pixels);
}

CPDF_LabCS::CPDF_LabCS(CPDF_Document* pDoc)
    : CPDF_ColorSpace(pDoc, PDFCS_LAB) {}

void CPDF_LabCS::GetDefaultValue(int iComponent,
                                 float* value,
                                 float* min,
                                 float* max) const {
  ASSERT(iComponent < 3);
  if (iComponent == 0) {
    *min = 0.0f;
    *max = 100 * 1.0f;
    *value = 0.0f;
    return;
  }

  *min = m_Ranges[iComponent * 2 - 2];
  *max = m_Ranges[iComponent * 2 - 1];
  *value = pdfium::clamp(0.0f, *min, *max);
}

uint32_t CPDF_LabCS::v_Load(CPDF_Document* pDoc,
                            const CPDF_Array* pArray,
                            std::set<const CPDF_Object*>* pVisited) {
  const CPDF_Dictionary* pDict = pArray->GetDictAt(1);
  if (!pDict)
    return 0;

  if (!GetWhitePoint(pDict, m_WhitePoint))
    return 0;

  GetBlackPoint(pDict, m_BlackPoint);

  const CPDF_Array* pParam = pDict->GetArrayFor("Range");
  static constexpr float kDefaultRanges[kRangesCount] = {-100.0f, 100.0f,
                                                         -100.0f, 100.0f};
  static_assert(FX_ArraySize(kDefaultRanges) == FX_ArraySize(m_Ranges),
                "Range size mismatch");
  for (size_t i = 0; i < FX_ArraySize(kDefaultRanges); ++i)
    m_Ranges[i] = pParam ? pParam->GetNumberAt(i) : kDefaultRanges[i];
  return 3;
}

bool CPDF_LabCS::GetRGB(const float* pBuf, float* R, float* G, float* B) const {
  float Lstar = pBuf[0];
  float astar = pBuf[1];
  float bstar = pBuf[2];
  float M = (Lstar + 16.0f) / 116.0f;
  float L = M + astar / 500.0f;
  float N = M - bstar / 200.0f;
  float X;
  float Y;
  float Z;
  if (L < 0.2069f)
    X = 0.957f * 0.12842f * (L - 0.1379f);
  else
    X = 0.957f * L * L * L;

  if (M < 0.2069f)
    Y = 0.12842f * (M - 0.1379f);
  else
    Y = M * M * M;

  if (N < 0.2069f)
    Z = 1.0889f * 0.12842f * (N - 0.1379f);
  else
    Z = 1.0889f * N * N * N;

  XYZ_to_sRGB(X, Y, Z, R, G, B);
  return true;
}

void CPDF_LabCS::TranslateImageLine(uint8_t* pDestBuf,
                                    const uint8_t* pSrcBuf,
                                    int pixels,
                                    int image_width,
                                    int image_height,
                                    bool bTransMask) const {
  for (int i = 0; i < pixels; i++) {
    float lab[3];
    lab[0] = pSrcBuf[0] * 100 / 255.0f;
    lab[1] = pSrcBuf[1] - 128;
    lab[2] = pSrcBuf[2] - 128;

    float R;
    float G;
    float B;
    GetRGB(lab, &R, &G, &B);
    pDestBuf[0] = static_cast<int32_t>(B * 255);
    pDestBuf[1] = static_cast<int32_t>(G * 255);
    pDestBuf[2] = static_cast<int32_t>(R * 255);
    pDestBuf += 3;
    pSrcBuf += 3;
  }
}

CPDF_ICCBasedCS::CPDF_ICCBasedCS(CPDF_Document* pDoc)
    : CPDF_ColorSpace(pDoc, PDFCS_ICCBASED) {}

CPDF_ICCBasedCS::~CPDF_ICCBasedCS() {
  if (m_pProfile && m_pDocument) {
    const CPDF_Stream* pStream = m_pProfile->GetStream();
    m_pProfile.Reset();  // Give up our reference first.
    auto* pPageData = m_pDocument->GetPageData();
    if (pPageData)
      pPageData->MaybePurgeIccProfile(pStream);
  }
}

uint32_t CPDF_ICCBasedCS::v_Load(CPDF_Document* pDoc,
                                 const CPDF_Array* pArray,
                                 std::set<const CPDF_Object*>* pVisited) {
  const CPDF_Stream* pStream = pArray->GetStreamAt(1);
  if (!pStream)
    return 0;

  // The PDF 1.7 spec says the number of components must be valid. While some
  // PDF viewers tolerate invalid values, Acrobat does not, so be consistent
  // with Acrobat and reject bad values.
  const CPDF_Dictionary* pDict = pStream->GetDict();
  int32_t nDictComponents = pDict ? pDict->GetIntegerFor("N") : 0;
  if (!IsValidComponents(nDictComponents))
    return 0;

  uint32_t nComponents = static_cast<uint32_t>(nDictComponents);
  m_pProfile = pDoc->LoadIccProfile(pStream);
  if (!m_pProfile)
    return 0;

  // The PDF 1.7 spec also says the number of components in the ICC profile
  // must match the N value. However, that assumes the viewer actually
  // understands the ICC profile.
  // If the valid ICC profile has a mismatch, fail.
  if (m_pProfile->IsValid() && m_pProfile->GetComponents() != nComponents)
    return 0;

  // If PDFium does not understand the ICC profile format at all, or if it's
  // SRGB, a profile PDFium recognizes but does not support well, then try the
  // alternate profile.
  if (!m_pProfile->IsSupported() &&
      !FindAlternateProfile(pDoc, pDict, pVisited, nComponents)) {
    // If there is no alternate profile, use a stock profile as mentioned in
    // the PDF 1.7 spec in table 4.16 in the "Alternate" key description.
    ASSERT(!m_pAlterCS);
    m_pAlterCS = GetStockAlternateProfile(nComponents);
  }

  m_pRanges = GetRanges(pDict, nComponents);
  return nComponents;
}

bool CPDF_ICCBasedCS::GetRGB(const float* pBuf,
                             float* R,
                             float* G,
                             float* B) const {
  ASSERT(m_pProfile);
  if (m_pProfile->IsSRGB()) {
    *R = pBuf[0];
    *G = pBuf[1];
    *B = pBuf[2];
    return true;
  }
  if (m_pProfile->transform()) {
    float rgb[3];
    CCodec_IccModule* pIccModule = CPDF_ModuleMgr::Get()->GetIccModule();
    pIccModule->SetComponents(CountComponents());
    pIccModule->Translate(m_pProfile->transform(), pBuf, rgb);
    *R = rgb[0];
    *G = rgb[1];
    *B = rgb[2];
    return true;
  }

  if (m_pAlterCS)
    return m_pAlterCS->GetRGB(pBuf, R, G, B);

  *R = 0.0f;
  *G = 0.0f;
  *B = 0.0f;
  return true;
}

void CPDF_ICCBasedCS::EnableStdConversion(bool bEnabled) {
  CPDF_ColorSpace::EnableStdConversion(bEnabled);
  if (m_pAlterCS)
    m_pAlterCS->EnableStdConversion(bEnabled);
}

void CPDF_ICCBasedCS::TranslateImageLine(uint8_t* pDestBuf,
                                         const uint8_t* pSrcBuf,
                                         int pixels,
                                         int image_width,
                                         int image_height,
                                         bool bTransMask) const {
  if (m_pProfile->IsSRGB()) {
    ReverseRGB(pDestBuf, pSrcBuf, pixels);
    return;
  }
  if (!m_pProfile->transform()) {
    if (m_pAlterCS) {
      m_pAlterCS->TranslateImageLine(pDestBuf, pSrcBuf, pixels, image_width,
                                     image_height, false);
    }
    return;
  }

  // |nMaxColors| will not overflow since |nComponents| is limited in size.
  const uint32_t nComponents = CountComponents();
  ASSERT(IsValidComponents(nComponents));
  int nMaxColors = 1;
  for (uint32_t i = 0; i < nComponents; i++)
    nMaxColors *= 52;

  bool bTranslate = nComponents > 3;
  if (!bTranslate) {
    FX_SAFE_INT32 nPixelCount = image_width;
    nPixelCount *= image_height;
    if (nPixelCount.IsValid())
      bTranslate = nPixelCount.ValueOrDie() < nMaxColors * 3 / 2;
  }
  if (bTranslate) {
    CPDF_ModuleMgr::Get()->GetIccModule()->TranslateScanline(
        m_pProfile->transform(), pDestBuf, pSrcBuf, pixels);
    return;
  }

  if (m_pCache.empty()) {
    m_pCache = pdfium::Vector2D<uint8_t>(nMaxColors, 3);
    auto temp_src = pdfium::Vector2D<uint8_t>(nMaxColors, nComponents);
    size_t src_index = 0;
    for (int i = 0; i < nMaxColors; i++) {
      uint32_t color = i;
      uint32_t order = nMaxColors / 52;
      for (uint32_t c = 0; c < nComponents; c++) {
        temp_src[src_index++] = static_cast<uint8_t>(color / order * 5);
        color %= order;
        order /= 52;
      }
    }
    CPDF_ModuleMgr::Get()->GetIccModule()->TranslateScanline(
        m_pProfile->transform(), m_pCache.data(), temp_src.data(), nMaxColors);
  }
  for (int i = 0; i < pixels; i++) {
    int index = 0;
    for (uint32_t c = 0; c < nComponents; c++) {
      index = index * 52 + (*pSrcBuf) / 5;
      pSrcBuf++;
    }
    index *= 3;
    *pDestBuf++ = m_pCache[index];
    *pDestBuf++ = m_pCache[index + 1];
    *pDestBuf++ = m_pCache[index + 2];
  }
}

bool CPDF_ICCBasedCS::IsNormal() const {
  if (m_pProfile->IsSRGB())
    return true;
  if (m_pProfile->transform())
    return m_pProfile->transform()->IsNormal();
  if (m_pAlterCS)
    return m_pAlterCS->IsNormal();
  return false;
}

bool CPDF_ICCBasedCS::FindAlternateProfile(
    CPDF_Document* pDoc,
    const CPDF_Dictionary* pDict,
    std::set<const CPDF_Object*>* pVisited,
    uint32_t nExpectedComponents) {
  const CPDF_Object* pAlterCSObj = pDict->GetDirectObjectFor("Alternate");
  if (!pAlterCSObj)
    return false;

  auto pAlterCS = CPDF_ColorSpace::Load(pDoc, pAlterCSObj, pVisited);
  if (!pAlterCS)
    return false;

  if (pAlterCS->GetFamily() == PDFCS_PATTERN)
    return false;

  if (pAlterCS->CountComponents() != nExpectedComponents)
    return false;

  m_pAlterCS = std::move(pAlterCS);
  return true;
}

// static
CPDF_ColorSpace* CPDF_ICCBasedCS::GetStockAlternateProfile(
    uint32_t nComponents) {
  if (nComponents == 1)
    return GetStockCS(PDFCS_DEVICEGRAY);
  if (nComponents == 3)
    return GetStockCS(PDFCS_DEVICERGB);
  if (nComponents == 4)
    return GetStockCS(PDFCS_DEVICECMYK);
  NOTREACHED();
  return nullptr;
}

// static
bool CPDF_ICCBasedCS::IsValidComponents(int32_t nComps) {
  return nComps == 1 || nComps == 3 || nComps == 4;
}

// static
std::vector<float> CPDF_ICCBasedCS::GetRanges(const CPDF_Dictionary* pDict,
                                              uint32_t nComponents) {
  ASSERT(IsValidComponents(nComponents));

  std::vector<float> ranges;
  ranges.reserve(nComponents * 2);
  const CPDF_Array* pRanges = pDict->GetArrayFor("Range");
  if (pRanges) {
    for (uint32_t i = 0; i < nComponents * 2; i++) {
      ranges.push_back(pRanges->GetNumberAt(i));
    }
  } else {
    for (uint32_t i = 0; i < nComponents; i++) {
      ranges.push_back(0.0f);
      ranges.push_back(1.0f);
    }
  }
  return ranges;
}

CPDF_IndexedCS::CPDF_IndexedCS(CPDF_Document* pDoc)
    : CPDF_ColorSpace(pDoc, PDFCS_INDEXED) {}

CPDF_IndexedCS::~CPDF_IndexedCS() {
  const CPDF_ColorSpace* pCS =
      m_pCountedBaseCS ? m_pCountedBaseCS->get() : nullptr;
  if (pCS && m_pDocument) {
    auto* pPageData = m_pDocument->GetPageData();
    if (pPageData)
      pPageData->ReleaseColorSpace(pCS->GetArray());
  }
}

uint32_t CPDF_IndexedCS::v_Load(CPDF_Document* pDoc,
                                const CPDF_Array* pArray,
                                std::set<const CPDF_Object*>* pVisited) {
  if (pArray->size() < 4)
    return 0;

  const CPDF_Object* pBaseObj = pArray->GetDirectObjectAt(1);
  if (pBaseObj == m_pArray)
    return 0;

  CPDF_DocPageData* pDocPageData = pDoc->GetPageData();
  m_pBaseCS = pDocPageData->GetColorSpaceGuarded(pBaseObj, nullptr, pVisited);
  if (!m_pBaseCS)
    return 0;

  // The base color space cannot be a Pattern or Indexed space, according to the
  // PDF 1.7 spec, page 263.
  int family = m_pBaseCS->GetFamily();
  if (family == PDFCS_INDEXED || family == PDFCS_PATTERN)
    return 0;

  m_pCountedBaseCS = pDocPageData->FindColorSpacePtr(m_pBaseCS->GetArray());
  m_nBaseComponents = m_pBaseCS->CountComponents();
  m_pCompMinMax = pdfium::Vector2D<float>(m_nBaseComponents, 2);
  float defvalue;
  for (uint32_t i = 0; i < m_nBaseComponents; i++) {
    m_pBaseCS->GetDefaultValue(i, &defvalue, &m_pCompMinMax[i * 2],
                               &m_pCompMinMax[i * 2 + 1]);
    m_pCompMinMax[i * 2 + 1] -= m_pCompMinMax[i * 2];
  }
  m_MaxIndex = pArray->GetIntegerAt(2);

  const CPDF_Object* pTableObj = pArray->GetDirectObjectAt(3);
  if (!pTableObj)
    return 0;

  if (const CPDF_String* pString = pTableObj->AsString()) {
    m_Table = pString->GetString();
  } else if (const CPDF_Stream* pStream = pTableObj->AsStream()) {
    auto pAcc = pdfium::MakeRetain<CPDF_StreamAcc>(pStream);
    pAcc->LoadAllDataFiltered();
    m_Table = ByteStringView(pAcc->GetData(), pAcc->GetSize());
  }
  return 1;
}

bool CPDF_IndexedCS::GetRGB(const float* pBuf,
                            float* R,
                            float* G,
                            float* B) const {
  int32_t index = static_cast<int32_t>(*pBuf);
  if (index < 0 || index > m_MaxIndex)
    return false;

  if (m_nBaseComponents) {
    FX_SAFE_SIZE_T length = index;
    length += 1;
    length *= m_nBaseComponents;
    if (!length.IsValid() || length.ValueOrDie() > m_Table.GetLength()) {
      *R = 0;
      *G = 0;
      *B = 0;
      return false;
    }
  }
  std::vector<float> comps(m_nBaseComponents);
  const uint8_t* pTable = m_Table.raw_str();
  for (uint32_t i = 0; i < m_nBaseComponents; ++i) {
    comps[i] =
        m_pCompMinMax[i * 2] +
        m_pCompMinMax[i * 2 + 1] * pTable[index * m_nBaseComponents + i] / 255;
  }
  ASSERT(m_nBaseComponents == m_pBaseCS->CountComponents());
  return m_pBaseCS->GetRGB(comps.data(), R, G, B);
}

void CPDF_IndexedCS::EnableStdConversion(bool bEnabled) {
  CPDF_ColorSpace::EnableStdConversion(bEnabled);
  if (m_pBaseCS)
    m_pBaseCS->EnableStdConversion(bEnabled);
}

CPDF_SeparationCS::CPDF_SeparationCS(CPDF_Document* pDoc)
    : CPDF_ColorSpace(pDoc, PDFCS_SEPARATION) {}

CPDF_SeparationCS::~CPDF_SeparationCS() {}

void CPDF_SeparationCS::GetDefaultValue(int iComponent,
                                        float* value,
                                        float* min,
                                        float* max) const {
  *value = 1.0f;
  *min = 0;
  *max = 1.0f;
}

uint32_t CPDF_SeparationCS::v_Load(CPDF_Document* pDoc,
                                   const CPDF_Array* pArray,
                                   std::set<const CPDF_Object*>* pVisited) {
  ByteString name = pArray->GetStringAt(1);
  if (name == "None") {
    m_Type = None;
    return 1;
  }

  m_Type = Colorant;
  const CPDF_Object* pAltCS = pArray->GetDirectObjectAt(2);
  if (pAltCS == m_pArray)
    return 0;

  m_pAltCS = Load(pDoc, pAltCS, pVisited);
  if (!m_pAltCS)
    return 0;

  if (m_pAltCS->IsSpecial())
    return 0;

  const CPDF_Object* pFuncObj = pArray->GetDirectObjectAt(3);
  if (pFuncObj && !pFuncObj->IsName()) {
    auto pFunc = CPDF_Function::Load(pFuncObj);
    if (pFunc && pFunc->CountOutputs() >= m_pAltCS->CountComponents())
      m_pFunc = std::move(pFunc);
  }
  return 1;
}

bool CPDF_SeparationCS::GetRGB(const float* pBuf,
                               float* R,
                               float* G,
                               float* B) const {
  if (m_Type == None)
    return false;

  if (!m_pFunc) {
    if (!m_pAltCS)
      return false;

    int nComps = m_pAltCS->CountComponents();
    std::vector<float> results(nComps);
    for (int i = 0; i < nComps; i++)
      results[i] = *pBuf;
    return m_pAltCS->GetRGB(results.data(), R, G, B);
  }

  // Using at least 16 elements due to the call m_pAltCS->GetRGB() below.
  std::vector<float> results(std::max(m_pFunc->CountOutputs(), 16u));
  int nresults = 0;
  if (!m_pFunc->Call(pBuf, 1, results.data(), &nresults) || nresults == 0)
    return false;

  if (m_pAltCS)
    return m_pAltCS->GetRGB(results.data(), R, G, B);

  R = 0;
  G = 0;
  B = 0;
  return false;
}

void CPDF_SeparationCS::EnableStdConversion(bool bEnabled) {
  CPDF_ColorSpace::EnableStdConversion(bEnabled);
  if (m_pAltCS)
    m_pAltCS->EnableStdConversion(bEnabled);
}

CPDF_DeviceNCS::CPDF_DeviceNCS(CPDF_Document* pDoc)
    : CPDF_ColorSpace(pDoc, PDFCS_DEVICEN) {}

CPDF_DeviceNCS::~CPDF_DeviceNCS() {}

void CPDF_DeviceNCS::GetDefaultValue(int iComponent,
                                     float* value,
                                     float* min,
                                     float* max) const {
  *value = 1.0f;
  *min = 0;
  *max = 1.0f;
}

uint32_t CPDF_DeviceNCS::v_Load(CPDF_Document* pDoc,
                                const CPDF_Array* pArray,
                                std::set<const CPDF_Object*>* pVisited) {
  const CPDF_Array* pObj = ToArray(pArray->GetDirectObjectAt(1));
  if (!pObj)
    return 0;

  const CPDF_Object* pAltCS = pArray->GetDirectObjectAt(2);
  if (!pAltCS || pAltCS == m_pArray)
    return 0;

  m_pAltCS = Load(pDoc, pAltCS, pVisited);
  m_pFunc = CPDF_Function::Load(pArray->GetDirectObjectAt(3));
  if (!m_pAltCS || !m_pFunc)
    return 0;

  if (m_pAltCS->IsSpecial())
    return 0;

  if (m_pFunc->CountOutputs() < m_pAltCS->CountComponents())
    return 0;

  return pObj->size();
}

bool CPDF_DeviceNCS::GetRGB(const float* pBuf,
                            float* R,
                            float* G,
                            float* B) const {
  if (!m_pFunc)
    return false;

  // Using at least 16 elements due to the call m_pAltCS->GetRGB() below.
  std::vector<float> results(std::max(m_pFunc->CountOutputs(), 16u));
  int nresults = 0;
  if (!m_pFunc->Call(pBuf, CountComponents(), results.data(), &nresults) ||
      nresults == 0) {
    return false;
  }

  return m_pAltCS->GetRGB(results.data(), R, G, B);
}

void CPDF_DeviceNCS::EnableStdConversion(bool bEnabled) {
  CPDF_ColorSpace::EnableStdConversion(bEnabled);
  if (m_pAltCS) {
    m_pAltCS->EnableStdConversion(bEnabled);
  }
}
