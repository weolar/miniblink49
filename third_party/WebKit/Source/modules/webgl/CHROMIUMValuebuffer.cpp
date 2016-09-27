// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#include "modules/webgl/CHROMIUMValuebuffer.h"

#include "modules/webgl/WebGLRenderingContextBase.h"

namespace blink {

PassRefPtrWillBeRawPtr<CHROMIUMValuebuffer> CHROMIUMValuebuffer::create(WebGLRenderingContextBase* ctx)
{
    return adoptRefWillBeNoop(new CHROMIUMValuebuffer(ctx));
}

CHROMIUMValuebuffer::~CHROMIUMValuebuffer()
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

CHROMIUMValuebuffer::CHROMIUMValuebuffer(WebGLRenderingContextBase* ctx)
    : WebGLSharedPlatform3DObject(ctx)
    , m_hasEverBeenBound(false)
{
    setObject(ctx->webContext()->createValuebufferCHROMIUM());
}

void CHROMIUMValuebuffer::deleteObjectImpl(WebGraphicsContext3D* context3d)
{
    context3d->deleteValuebufferCHROMIUM(m_object);
    m_object = 0;
}

} // namespace blink
