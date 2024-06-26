// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fpdfapi/render/cpdf_renderoptions.h"

namespace {

constexpr uint32_t kCacheSizeLimitBytes = 100 * 1024 * 1024;

}  // namespace

CPDF_RenderOptions::Options::Options() = default;

CPDF_RenderOptions::Options::Options(const CPDF_RenderOptions::Options& rhs) =
    default;

CPDF_RenderOptions::CPDF_RenderOptions() {
  // TODO(thestig): Make constexpr to initialize |m_Options| once C++14 is
  // available.
  m_Options.bClearType = true;
}

CPDF_RenderOptions::CPDF_RenderOptions(const CPDF_RenderOptions& rhs) = default;

CPDF_RenderOptions::~CPDF_RenderOptions() = default;

FX_ARGB CPDF_RenderOptions::TranslateColor(FX_ARGB argb) const {
  if (ColorModeIs(kNormal))
    return argb;
  if (ColorModeIs(kAlpha))
    return argb;

  int a;
  int r;
  int g;
  int b;
  std::tie(a, r, g, b) = ArgbDecode(argb);
  int gray = FXRGB2GRAY(r, g, b);
  return ArgbEncode(a, gray, gray, gray);
}

uint32_t CPDF_RenderOptions::GetCacheSizeLimit() const {
  return kCacheSizeLimitBytes;
}
