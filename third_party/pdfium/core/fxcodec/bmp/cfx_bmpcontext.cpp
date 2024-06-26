// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcodec/bmp/cfx_bmpcontext.h"

CFX_BmpContext::CFX_BmpContext(CCodec_BmpModule* pModule,
                               CCodec_BmpModule::Delegate* pDelegate)
    : m_pModule(pModule), m_pDelegate(pDelegate) {}

CFX_BmpContext::~CFX_BmpContext() {}
