// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_PAGE_CPDF_TEXTOBJECT_H_
#define CORE_FPDFAPI_PAGE_CPDF_TEXTOBJECT_H_

#include <memory>
#include <vector>

#include "core/fpdfapi/page/cpdf_pageobject.h"
#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/fx_system.h"

class CPDF_TextObjectItem {
 public:
  CPDF_TextObjectItem();
  ~CPDF_TextObjectItem();

  uint32_t m_CharCode;
  CFX_PointF m_Origin;
};

class CPDF_TextObject final : public CPDF_PageObject {
 public:
  explicit CPDF_TextObject(int32_t content_stream);
  CPDF_TextObject();
  ~CPDF_TextObject() override;

  // CPDF_PageObject
  Type GetType() const override;
  void Transform(const CFX_Matrix& matrix) override;
  bool IsText() const override;
  CPDF_TextObject* AsText() override;
  const CPDF_TextObject* AsText() const override;

  std::unique_ptr<CPDF_TextObject> Clone() const;

  size_t CountItems() const;
  void GetItemInfo(size_t index, CPDF_TextObjectItem* pInfo) const;

  size_t CountChars() const;
  void GetCharInfo(size_t index, uint32_t* charcode, float* kerning) const;
  void GetCharInfo(size_t index, CPDF_TextObjectItem* pInfo) const;
  float GetCharWidth(uint32_t charcode) const;

  CFX_PointF GetPos() const { return m_Pos; }
  CFX_Matrix GetTextMatrix() const;
  CPDF_Font* GetFont() const;
  float GetFontSize() const;

  void SetText(const ByteString& text);
  void SetPosition(CFX_PointF pos) { m_Pos = pos; }
  void SetPosition(float x, float y);

  void RecalcPositionData();

  const std::vector<uint32_t>& GetCharCodes() const { return m_CharCodes; }
  const std::vector<float>& GetCharPositions() const { return m_CharPos; }

  void SetSegments(const ByteString* pStrs,
                   const std::vector<float>& kernings,
                   size_t nSegs);
  CFX_PointF CalcPositionData(float horz_scale);

 private:
  CFX_PointF m_Pos;
  std::vector<uint32_t> m_CharCodes;
  std::vector<float> m_CharPos;
};

#endif  // CORE_FPDFAPI_PAGE_CPDF_TEXTOBJECT_H_
