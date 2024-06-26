// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef CORE_FPDFAPI_RENDER_CPDF_TYPE3GLYPHS_H_
#define CORE_FPDFAPI_RENDER_CPDF_TYPE3GLYPHS_H_

#include <map>
#include <memory>
#include <utility>
#include <vector>

#include "core/fxcrt/fx_system.h"

class CFX_GlyphBitmap;

class CPDF_Type3Glyphs {
 public:
  CPDF_Type3Glyphs();
  ~CPDF_Type3Glyphs();

  // Returns a pair of integers (top_line, bottom_line).
  std::pair<int, int> AdjustBlue(float top, float bottom);

  std::map<uint32_t, std::unique_ptr<CFX_GlyphBitmap>> m_GlyphMap;

 private:
  std::vector<int> m_TopBlue;
  std::vector<int> m_BottomBlue;
};

#endif  // CORE_FPDFAPI_RENDER_CPDF_TYPE3GLYPHS_H_
