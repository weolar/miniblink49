// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_PAGE_CPDF_PATHOBJECT_H_
#define CORE_FPDFAPI_PAGE_CPDF_PATHOBJECT_H_

#include "core/fpdfapi/page/cpdf_pageobject.h"
#include "core/fpdfapi/page/cpdf_path.h"
#include "core/fxcrt/fx_coordinates.h"
#include "core/fxcrt/fx_system.h"

class CPDF_PathObject final : public CPDF_PageObject {
 public:
  explicit CPDF_PathObject(int32_t content_stream);
  CPDF_PathObject();
  ~CPDF_PathObject() override;

  // CPDF_PageObject
  Type GetType() const override;
  void Transform(const CFX_Matrix& maxtrix) override;
  bool IsPath() const override;
  CPDF_PathObject* AsPath() override;
  const CPDF_PathObject* AsPath() const override;

  void CalcBoundingBox();

  bool stroke() const { return m_bStroke; }
  void set_stroke(bool stroke) { m_bStroke = stroke; }

  int filltype() const { return m_FillType; }
  void set_filltype(int filltype) { m_FillType = filltype; }

  CPDF_Path& path() { return m_Path; }
  const CPDF_Path& path() const { return m_Path; }

  const CFX_Matrix& matrix() const { return m_Matrix; }
  void set_matrix(const CFX_Matrix& matrix) { m_Matrix = matrix; }

 private:
  bool m_bStroke = false;
  int m_FillType = 0;
  CPDF_Path m_Path;
  CFX_Matrix m_Matrix;
};

#endif  // CORE_FPDFAPI_PAGE_CPDF_PATHOBJECT_H_
