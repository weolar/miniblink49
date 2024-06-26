// Copyright 2016 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxge/android/cfpf_skiadevicemodule.h"

#include <utility>

#include "core/fxge/android/cfpf_skiafontmgr.h"
#include "third_party/base/ptr_util.h"

namespace {

CFPF_SkiaDeviceModule* gs_pPFModule = nullptr;

}  // namespace

CFPF_SkiaDeviceModule* CFPF_GetSkiaDeviceModule() {
  if (!gs_pPFModule)
    gs_pPFModule = new CFPF_SkiaDeviceModule;
  return gs_pPFModule;
}

CFPF_SkiaDeviceModule::CFPF_SkiaDeviceModule() {}

CFPF_SkiaDeviceModule::~CFPF_SkiaDeviceModule() {}

void CFPF_SkiaDeviceModule::Destroy() {
  delete gs_pPFModule;
  gs_pPFModule = nullptr;
}

CFPF_SkiaFontMgr* CFPF_SkiaDeviceModule::GetFontMgr() {
  if (!m_pFontMgr) {
    auto pNewMgr = pdfium::MakeUnique<CFPF_SkiaFontMgr>();
    if (!pNewMgr->InitFTLibrary())
      return nullptr;
    m_pFontMgr = std::move(pNewMgr);
  }
  return m_pFontMgr.get();
}
