// Copyright 2014 PDFium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// Original code copyright 2014 Foxit Software Inc. http://www.foxitsoftware.com

#include "core/fxcodec/gif/cfx_gif.h"

const char kGifSignature87[] = "GIF87a";
const char kGifSignature89[] = "GIF89a";

static_assert(sizeof(CFX_GifGlobalFlags) == 1,
              "CFX_GifGlobalFlags should have a size of 1");
static_assert(sizeof(CFX_GifLocalFlags) == 1,
              "CFX_GifLocalFlags should have a size of 1");
static_assert(sizeof(CFX_GifHeader) == 6,
              "CFX_GifHeader should have a size of 6");
static_assert(sizeof(CFX_GifLocalScreenDescriptor) == 7,
              "CFX_GifLocalScreenDescriptor should have a size of 7");
static_assert(sizeof(CFX_CFX_GifImageInfo) == 9,
              "CFX_CFX_GifImageInfo should have a size of 9");
static_assert(sizeof(CFX_GifControlExtensionFlags) == 1,
              "CFX_GifControlExtensionFlags should have a size of 1");
static_assert(sizeof(CFX_GifPlainTextExtension) == 13,
              "CFX_GifPlainTextExtension should have a size of 13");
static_assert(sizeof(CFX_GifGraphicControlExtension) == 5,
              "CFX_GifGraphicControlExtension should have a size of 5");
static_assert(sizeof(GifApplicationExtension) == 12,
              "GifApplicationExtension should have a size of 12");
static_assert(sizeof(CFX_GifPalette) == 3,
              "CFX_GifPalette should have a size of 3");

CFX_GifImage::CFX_GifImage() = default;

CFX_GifImage::~CFX_GifImage() = default;
