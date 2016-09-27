// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#include "modules/webgl/EXTsRGB.h"

#include "modules/webgl/WebGLRenderingContextBase.h"

namespace blink {

EXTsRGB::EXTsRGB(WebGLRenderingContextBase* context)
    : WebGLExtension(context)
{
    context->extensionsUtil()->ensureExtensionEnabled("GL_EXT_sRGB");
}

EXTsRGB::~EXTsRGB()
{
}

WebGLExtensionName EXTsRGB::name() const
{
    return EXTsRGBName;
}

PassRefPtrWillBeRawPtr<EXTsRGB> EXTsRGB::create(WebGLRenderingContextBase* context)
{
    return adoptRefWillBeNoop(new EXTsRGB(context));
}

bool EXTsRGB::supported(WebGLRenderingContextBase* context)
{
    Extensions3DUtil* extensionsUtil = context->extensionsUtil();
    return extensionsUtil->supportsExtension("GL_EXT_sRGB");
}

const char* EXTsRGB::extensionName()
{
    return "EXT_sRGB";
}

} // namespace blink
