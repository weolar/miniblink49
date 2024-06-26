// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#ifndef FPDFSDK_PWL_CPWL_ICON_H_
#define FPDFSDK_PWL_CPWL_ICON_H_

#include <memory>
#include <utility>

#include "core/fxcrt/fx_string.h"
#include "core/fxcrt/unowned_ptr.h"
#include "fpdfsdk/pwl/cpwl_wnd.h"

class CPWL_Icon final : public CPWL_Wnd {
 public:
  CPWL_Icon(const CreateParams& cp, std::unique_ptr<PrivateData> pAttachedData);
  ~CPWL_Icon() override;

  void SetIconFit(CPDF_IconFit* pIconFit) { m_pIconFit = pIconFit; }
  void SetPDFStream(CPDF_Stream* pStream) { m_pPDFStream = pStream; }

  // horizontal scale, vertical scale
  std::pair<float, float> GetScale();

  // x, y
  std::pair<float, float> GetImageOffset();

  CFX_Matrix GetImageMatrix();
  ByteString GetImageAlias();

 private:
  // left, bottom
  std::pair<float, float> GetIconPosition();

  // width, height
  std::pair<float, float> GetImageSize();

  UnownedPtr<CPDF_Stream> m_pPDFStream;
  UnownedPtr<CPDF_IconFit> m_pIconFit;
};

#endif  // FPDFSDK_PWL_CPWL_ICON_H_
