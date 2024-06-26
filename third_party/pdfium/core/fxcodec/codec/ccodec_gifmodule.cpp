// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcodec/codec/ccodec_gifmodule.h"

#include "core/fxcodec/codec/cfx_codec_memory.h"
#include "core/fxcodec/codec/codec_int.h"
#include "core/fxcodec/fx_codec.h"
#include "core/fxcodec/gif/cfx_gif.h"
#include "core/fxcodec/gif/cfx_gifcontext.h"
#include "core/fxge/fx_dib.h"
#include "third_party/base/ptr_util.h"

CCodec_GifModule::CCodec_GifModule() {}

CCodec_GifModule::~CCodec_GifModule() {}

std::unique_ptr<CodecModuleIface::Context> CCodec_GifModule::Start(
    Delegate* pDelegate) {
  return pdfium::MakeUnique<CFX_GifContext>(this, pDelegate);
}

CFX_GifDecodeStatus CCodec_GifModule::ReadHeader(Context* pContext,
                                                 int* width,
                                                 int* height,
                                                 int* pal_num,
                                                 CFX_GifPalette** pal_pp,
                                                 int* bg_index,
                                                 CFX_DIBAttribute* pAttribute) {
  auto* context = static_cast<CFX_GifContext*>(pContext);
  CFX_GifDecodeStatus ret = context->ReadHeader();
  if (ret != CFX_GifDecodeStatus::Success)
    return ret;

  *width = context->width_;
  *height = context->height_;
  *pal_num = (2 << context->global_pal_exp_);
  *pal_pp = context->global_palette_.empty() ? nullptr
                                             : context->global_palette_.data();
  *bg_index = context->bc_index_;
  return CFX_GifDecodeStatus::Success;
}

std::pair<CFX_GifDecodeStatus, size_t> CCodec_GifModule::LoadFrameInfo(
    Context* pContext) {
  auto* context = static_cast<CFX_GifContext*>(pContext);
  CFX_GifDecodeStatus ret = context->GetFrame();
  if (ret != CFX_GifDecodeStatus::Success)
    return {ret, 0};
  return {CFX_GifDecodeStatus::Success, context->GetFrameNum()};
}

CFX_GifDecodeStatus CCodec_GifModule::LoadFrame(Context* pContext,
                                                size_t frame_num,
                                                CFX_DIBAttribute* pAttribute) {
  auto* context = static_cast<CFX_GifContext*>(pContext);
  CFX_GifDecodeStatus ret = context->LoadFrame(frame_num);
  if (ret != CFX_GifDecodeStatus::Success)
    return ret;

  if (pAttribute)
    pAttribute->m_fAspectRatio = context->pixel_aspect_;

  return CFX_GifDecodeStatus::Success;
}

FX_FILESIZE CCodec_GifModule::GetAvailInput(Context* pContext) const {
  return static_cast<CFX_GifContext*>(pContext)->GetAvailInput();
}

bool CCodec_GifModule::Input(Context* pContext,
                             RetainPtr<CFX_CodecMemory> codec_memory,
                             CFX_DIBAttribute*) {
  auto* ctx = static_cast<CFX_GifContext*>(pContext);
  ctx->SetInputBuffer(std::move(codec_memory));
  return true;
}
