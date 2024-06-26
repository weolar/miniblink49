// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_PAGE_CPDF_COLORSTATE_H_
#define CORE_FPDFAPI_PAGE_CPDF_COLORSTATE_H_

#include <vector>

#include "core/fpdfapi/page/cpdf_color.h"
#include "core/fxcrt/fx_system.h"
#include "core/fxcrt/shared_copy_on_write.h"
#include "core/fxge/fx_dib.h"

class CPDF_Color;
class CPDF_ColorSpace;
class CPDF_Pattern;

class CPDF_ColorState {
 public:
  CPDF_ColorState();
  CPDF_ColorState(const CPDF_ColorState& that);
  ~CPDF_ColorState();

  void Emplace();
  void SetDefault();

  FX_COLORREF GetFillColorRef() const;
  void SetFillColorRef(FX_COLORREF colorref);

  FX_COLORREF GetStrokeColorRef() const;
  void SetStrokeColorRef(FX_COLORREF colorref);

  const CPDF_Color* GetFillColor() const;
  CPDF_Color* GetMutableFillColor();
  bool HasFillColor() const;

  const CPDF_Color* GetStrokeColor() const;
  CPDF_Color* GetMutableStrokeColor();
  bool HasStrokeColor() const;

  void SetFillColor(CPDF_ColorSpace* pCS, const std::vector<float>& values);
  void SetStrokeColor(CPDF_ColorSpace* pCS, const std::vector<float>& values);
  void SetFillPattern(CPDF_Pattern* pattern, const std::vector<float>& values);
  void SetStrokePattern(CPDF_Pattern* pattern,
                        const std::vector<float>& values);

  bool HasRef() const { return !!m_Ref; }

 private:
  class ColorData final : public Retainable {
   public:
    ColorData();
    ColorData(const ColorData& src);
    ~ColorData() override;

    void SetDefault();

    FX_COLORREF m_FillColorRef = 0;
    FX_COLORREF m_StrokeColorRef = 0;
    CPDF_Color m_FillColor;
    CPDF_Color m_StrokeColor;
  };

  void SetColor(CPDF_ColorSpace* pCS,
                const std::vector<float>& values,
                CPDF_Color* color,
                FX_COLORREF* colorref);
  void SetPattern(CPDF_Pattern* pPattern,
                  const std::vector<float>& values,
                  CPDF_Color* color,
                  FX_COLORREF* colorref);

  SharedCopyOnWrite<ColorData> m_Ref;
};

#endif  // CORE_FPDFAPI_PAGE_CPDF_COLORSTATE_H_
