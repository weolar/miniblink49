// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/webgl/WebGLContextAttributeHelpers.h"

#include "core/frame/Settings.h"

namespace blink {

WebGLContextAttributes toWebGLContextAttributes(const CanvasContextCreationAttributes& attrs)
{
    WebGLContextAttributes result;
    result.setAlpha(attrs.alpha());
    result.setDepth(attrs.depth());
    result.setStencil(attrs.stencil());
    result.setAntialias(attrs.antialias());
    result.setPremultipliedAlpha(attrs.premultipliedAlpha());
    result.setPreserveDrawingBuffer(attrs.preserveDrawingBuffer());
    result.setFailIfMajorPerformanceCaveat(attrs.failIfMajorPerformanceCaveat());
    return result;
}

WebGraphicsContext3D::Attributes toWebGraphicsContext3DAttributes(const WebGLContextAttributes& attrs, const WebString& topDocumentURL, Settings* settings, unsigned webGLVersion)
{
    WebGraphicsContext3D::Attributes result;
    result.alpha = attrs.alpha();
    result.depth = attrs.depth();
    result.stencil = attrs.stencil();
    result.antialias = attrs.antialias();
    if (attrs.antialias()) {
        if (settings && !settings->openGLMultisamplingEnabled())
            result.antialias = false;
    }
    result.premultipliedAlpha = attrs.premultipliedAlpha();
    result.failIfMajorPerformanceCaveat = attrs.failIfMajorPerformanceCaveat();

    result.noExtensions = true;
    result.shareResources = false;
    result.preferDiscreteGPU = true;

    result.topDocumentURL = topDocumentURL;

    result.webGL = true;
    result.webGLVersion = webGLVersion;
    return result;
}

} // namespace blink
