// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#include "modules/webgl/WebGLSampler.h"

#include "modules/webgl/WebGL2RenderingContextBase.h"

namespace blink {

PassRefPtrWillBeRawPtr<WebGLSampler> WebGLSampler::create(WebGL2RenderingContextBase* ctx)
{
    return adoptRefWillBeNoop(new WebGLSampler(ctx));
}

WebGLSampler::~WebGLSampler()
{
    // Always call detach here to ensure that platform object deletion
    // happens with Oilpan enabled. It keeps the code regular to do it
    // with or without Oilpan enabled.
    //
    // See comment in WebGLBuffer's destructor for additional
    // information on why this is done for WebGLSharedObject-derived
    // objects.
    detachAndDeleteObject();
}

WebGLSampler::WebGLSampler(WebGL2RenderingContextBase* ctx)
    : WebGLSharedPlatform3DObject(ctx)
{
    setObject(ctx->webContext()->createSampler());
}

void WebGLSampler::deleteObjectImpl(WebGraphicsContext3D* context3d)
{
    context3d->deleteSampler(m_object);
    m_object = 0;
}

} // namespace blink
