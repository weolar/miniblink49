// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/render/cpdf_type3glyphs.h"

#include <algorithm>
#include <map>

#include "core/fxge/fx_font.h"

namespace {

constexpr int kType3MaxBlues = 16;

int AdjustBlueHelper(float pos, std::vector<int>* blues) {
  float min_distance = 1000000.0f;
  int closest_pos = -1;
  for (int i = 0; i < static_cast<int>(blues->size()); ++i) {
    float distance = fabs(pos - static_cast<float>(blues->at(i)));
    if (distance < std::min(0.8f, min_distance)) {
      min_distance = distance;
      closest_pos = i;
    }
  }
  if (closest_pos >= 0)
    return blues->at(closest_pos);
  int new_pos = FXSYS_round(pos);
  if (blues->size() < kType3MaxBlues)
    blues->push_back(new_pos);
  return new_pos;
}

}  // namespace

CPDF_Type3Glyphs::CPDF_Type3Glyphs() {}

CPDF_Type3Glyphs::~CPDF_Type3Glyphs() {}

std::pair<int, int> CPDF_Type3Glyphs::AdjustBlue(float top, float bottom) {
  return std::make_pair(AdjustBlueHelper(top, &m_TopBlue),
                        AdjustBlueHelper(bottom, &m_BottomBlue));
}
