// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxge/cfx_gemodule.h"

#include <memory>
#include <utility>

#include "core/fxge/android/cfpf_skiadevicemodule.h"
#include "core/fxge/android/cfx_androidfontinfo.h"
#include "core/fxge/cfx_fontmgr.h"
#include "third_party/base/ptr_util.h"

void CFX_GEModule::InitPlatform() {
  CFPF_SkiaDeviceModule* pDeviceModule = CFPF_GetSkiaDeviceModule();
  if (!pDeviceModule)
    return;

  CFPF_SkiaFontMgr* pFontMgr = pDeviceModule->GetFontMgr();
  if (pFontMgr) {
    auto pFontInfo = pdfium::MakeUnique<CFX_AndroidFontInfo>();
    pFontInfo->Init(pFontMgr);
    m_pFontMgr->SetSystemFontInfo(std::move(pFontInfo));
  }
  m_pPlatformData = pDeviceModule;
}

void CFX_GEModule::DestroyPlatform() {
  if (m_pPlatformData)
    static_cast<CFPF_SkiaDeviceModule*>(m_pPlatformData)->Destroy();
}
