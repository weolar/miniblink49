// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_PAGE_CPDF_COLORSPACE_H_
#define CORE_FPDFAPI_PAGE_CPDF_COLORSPACE_H_

#include <memory>
#include <set>

#include "core/fpdfapi/page/cpdf_pattern.h"
#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/fx_system.h"
#include "core/fxcrt/unowned_ptr.h"

#define PDFCS_DEVICEGRAY 1
#define PDFCS_DEVICERGB 2
#define PDFCS_DEVICECMYK 3
#define PDFCS_CALGRAY 4
#define PDFCS_CALRGB 5
#define PDFCS_LAB 6
#define PDFCS_ICCBASED 7
#define PDFCS_SEPARATION 8
#define PDFCS_DEVICEN 9
#define PDFCS_INDEXED 10
#define PDFCS_PATTERN 11

class CPDF_Array;
class CPDF_Document;
class CPDF_Object;
class CPDF_PatternCS;

constexpr size_t kMaxPatternColorComps = 16;

struct PatternValue {
  CPDF_Pattern* m_pPattern;
  CPDF_CountedPattern* m_pCountedPattern;
  int m_nComps;
  float m_Comps[kMaxPatternColorComps];
};

class CPDF_ColorSpace {
 public:
  static CPDF_ColorSpace* GetStockCS(int Family);
  static CPDF_ColorSpace* ColorspaceFromName(const ByteString& name);
  static std::unique_ptr<CPDF_ColorSpace> Load(CPDF_Document* pDoc,
                                               CPDF_Object* pCSObj);
  static std::unique_ptr<CPDF_ColorSpace> Load(
      CPDF_Document* pDoc,
      const CPDF_Object* pCSObj,
      std::set<const CPDF_Object*>* pVisited);

  void Release();

  size_t GetBufSize() const;
  float* CreateBuf() const;

  // Should only be called if this colorspace is not a pattern.
  float* CreateBufAndSetDefaultColor() const;

  uint32_t CountComponents() const;
  int GetFamily() const { return m_Family; }
  bool IsSpecial() const {
    return GetFamily() == PDFCS_SEPARATION || GetFamily() == PDFCS_DEVICEN ||
           GetFamily() == PDFCS_INDEXED || GetFamily() == PDFCS_PATTERN;
  }

  virtual void GetDefaultValue(int iComponent,
                               float* value,
                               float* min,
                               float* max) const;

  virtual bool GetRGB(const float* pBuf,
                      float* R,
                      float* G,
                      float* B) const = 0;

  virtual void TranslateImageLine(uint8_t* dest_buf,
                                  const uint8_t* src_buf,
                                  int pixels,
                                  int image_width,
                                  int image_height,
                                  bool bTransMask) const;
  virtual void EnableStdConversion(bool bEnabled);

  virtual bool IsNormal() const;

  // Only call these 3 methods below if GetFamily() returns |PDFCS_PATTERN|.

  // Returns |this| as a CPDF_PatternCS* if |this| is a pattern.
  virtual CPDF_PatternCS* AsPatternCS();
  virtual const CPDF_PatternCS* AsPatternCS() const;

  // Use instead of GetRGB() for patterns.
  virtual bool GetPatternRGB(const PatternValue& value,
                             float* R,
                             float* G,
                             float* B) const;

  const CPDF_Array* GetArray() const { return m_pArray.Get(); }
  CPDF_Document* GetDocument() const { return m_pDocument.Get(); }

 protected:
  CPDF_ColorSpace(CPDF_Document* pDoc, int family);
  virtual ~CPDF_ColorSpace();

  // Returns the number of components, or 0 on failure.
  virtual uint32_t v_Load(CPDF_Document* pDoc,
                          const CPDF_Array* pArray,
                          std::set<const CPDF_Object*>* pVisited) = 0;

  // Stock colorspaces are not loaded normally. This initializes their
  // components count.
  void SetComponentsForStockCS(uint32_t nComponents);

  UnownedPtr<CPDF_Document> const m_pDocument;
  UnownedPtr<const CPDF_Array> m_pArray;
  const int m_Family;
  uint32_t m_dwStdConversion = 0;

 private:
  uint32_t m_nComponents = 0;
};
using CPDF_CountedColorSpace = CPDF_CountedObject<CPDF_ColorSpace>;

namespace std {

// Make std::unique_ptr<CPDF_ColorSpace> call Release() rather than
// simply deleting the object.
template <>
struct default_delete<CPDF_ColorSpace> {
  void operator()(CPDF_ColorSpace* pColorSpace) const {
    if (pColorSpace)
      pColorSpace->Release();
  }
};

}  // namespace std

#endif  // CORE_FPDFAPI_PAGE_CPDF_COLORSPACE_H_
