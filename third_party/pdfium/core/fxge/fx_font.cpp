// Copyright 2018 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "core/fxge/fx_font.h"

#include "core/fxge/dib/cfx_dibitmap.h"
#include "core/fxge/dib/cfx_imagestretcher.h"

CFX_GlyphBitmap::CFX_GlyphBitmap()
    : m_pBitmap(pdfium::MakeRetain<CFX_DIBitmap>()) {}

CFX_GlyphBitmap::~CFX_GlyphBitmap() = default;
