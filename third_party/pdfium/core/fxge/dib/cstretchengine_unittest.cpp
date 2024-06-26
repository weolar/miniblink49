// Copyright 2017 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/fxge/dib/cstretchengine.h"

#include <memory>
#include <utility>

#include "core/fpdfapi/parser/cpdf_dictionary.h"
#include "core/fpdfapi/parser/cpdf_number.h"
#include "core/fpdfapi/parser/cpdf_stream.h"
#include "core/fpdfapi/render/cpdf_dibbase.h"
#include "core/fxge/fx_dib.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/base/ptr_util.h"

TEST(CStretchEngine, OverflowInCtor) {
  FX_RECT clip_rect;
  std::unique_ptr<CPDF_Dictionary> dict_obj =
      pdfium::MakeUnique<CPDF_Dictionary>();
  dict_obj->SetNewFor<CPDF_Number>("Width", 71000);
  dict_obj->SetNewFor<CPDF_Number>("Height", 12500);
  std::unique_ptr<CPDF_Stream> stream =
      pdfium::MakeUnique<CPDF_Stream>(nullptr, 0, std::move(dict_obj));
  auto dib_source = pdfium::MakeRetain<CPDF_DIBBase>();
  dib_source->Load(nullptr, stream.get());
  CStretchEngine engine(nullptr, FXDIB_8bppRgb, 500, 500, clip_rect, dib_source,
                        FXDIB_ResampleOptions());
  EXPECT_FALSE(engine.m_ResampleOptions.bInterpolateDownsample);
  EXPECT_TRUE(engine.m_ResampleOptions.bInterpolateBilinear);
  EXPECT_FALSE(engine.m_ResampleOptions.bInterpolateBicubic);
  EXPECT_FALSE(engine.m_ResampleOptions.bHalftone);
  EXPECT_FALSE(engine.m_ResampleOptions.bNoSmoothing);
  EXPECT_FALSE(engine.m_ResampleOptions.bLossy);
}
