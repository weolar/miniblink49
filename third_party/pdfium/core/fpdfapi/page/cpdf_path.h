// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_PAGE_CPDF_PATH_H_
#define CORE_FPDFAPI_PAGE_CPDF_PATH_H_

#include <vector>

#include "core/fxcrt/fx_system.h"
#include "core/fxcrt/shared_copy_on_write.h"
#include "core/fxge/cfx_pathdata.h"

class CPDF_Path {
 public:
  CPDF_Path();
  CPDF_Path(const CPDF_Path& that);
  ~CPDF_Path();

  void Emplace() { m_Ref.Emplace(); }
  bool HasRef() const { return !!m_Ref; }

  const std::vector<FX_PATHPOINT>& GetPoints() const;
  void ClosePath();

  CFX_PointF GetPoint(int index) const;
  CFX_FloatRect GetBoundingBox() const;
  CFX_FloatRect GetBoundingBox(float line_width, float miter_limit) const;

  bool IsRect() const;
  void Transform(const CFX_Matrix& matrix);

  void Append(const CPDF_Path& other, const CFX_Matrix* pMatrix);
  void Append(const CFX_PathData* pData, const CFX_Matrix* pMatrix);
  // TODO(thestig): Switch to CFX_FloatRect.
  void AppendRect(float left, float bottom, float right, float top);
  void AppendPoint(const CFX_PointF& point, FXPT_TYPE type, bool close);

  // TODO(tsepez): Remove when all access thru this class.
  const CFX_PathData* GetObject() const { return m_Ref.GetObject(); }

 private:
  SharedCopyOnWrite<CFX_PathData> m_Ref;
};

#endif  // CORE_FPDFAPI_PAGE_CPDF_PATH_H_
