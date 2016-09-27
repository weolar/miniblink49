// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#include "modules/webgl/CHROMIUMSubscribeUniform.h"

#include "modules/webgl/CHROMIUMValuebuffer.h"

namespace blink {

CHROMIUMSubscribeUniform::CHROMIUMSubscribeUniform(WebGLRenderingContextBase* context)
    : WebGLExtension(context)
{
    context->extensionsUtil()->ensureExtensionEnabled("GL_CHROMIUM_subscribe_uniform");
}

CHROMIUMSubscribeUniform::~CHROMIUMSubscribeUniform()
{
}

WebGLExtensionName CHROMIUMSubscribeUniform::name() const
{
    return CHROMIUMSubscribeUniformName;
}

PassRefPtrWillBeRawPtr<CHROMIUMSubscribeUniform> CHROMIUMSubscribeUniform::create(WebGLRenderingContextBase* context)
{
    return adoptRefWillBeNoop(new CHROMIUMSubscribeUniform(context));
}

bool CHROMIUMSubscribeUniform::supported(WebGLRenderingContextBase* context)
{
    return context->extensionsUtil()->supportsExtension("GL_CHROMIUM_subscribe_uniform");
}

const char* CHROMIUMSubscribeUniform::extensionName()
{
    return "CHROMIUM_subscribe_uniform";
}

PassRefPtrWillBeRawPtr<CHROMIUMValuebuffer> CHROMIUMSubscribeUniform::createValuebufferCHROMIUM()
{
    WebGLExtensionScopedContext scoped(this);
    if (scoped.isLost())
        return nullptr;

    return scoped.context()->createValuebufferCHROMIUM();
}

void CHROMIUMSubscribeUniform::deleteValuebufferCHROMIUM(CHROMIUMValuebuffer* valuebuffer)
{
    WebGLExtensionScopedContext scoped(this);
    if (scoped.isLost() || !valuebuffer)
        return;

    scoped.context()->deleteValuebufferCHROMIUM(valuebuffer);
}

GLboolean CHROMIUMSubscribeUniform::isValuebufferCHROMIUM(CHROMIUMValuebuffer* valuebuffer)
{
    WebGLExtensionScopedContext scoped(this);
    if (scoped.isLost() || !valuebuffer)
        return 0;

    return scoped.context()->isValuebufferCHROMIUM(valuebuffer);
}

void CHROMIUMSubscribeUniform::bindValuebufferCHROMIUM(GLenum target, CHROMIUMValuebuffer* valuebuffer)
{
    WebGLExtensionScopedContext scoped(this);
    if (scoped.isLost() || !valuebuffer)
        return;

    scoped.context()->bindValuebufferCHROMIUM(target, valuebuffer);
}

void CHROMIUMSubscribeUniform::subscribeValueCHROMIUM(GLenum target, GLenum subscription)
{
    WebGLExtensionScopedContext scoped(this);
    if (scoped.isLost())
        return;

    scoped.context()->subscribeValueCHROMIUM(target, subscription);
}

void CHROMIUMSubscribeUniform::populateSubscribedValuesCHROMIUM(GLenum target)
{
    WebGLExtensionScopedContext scoped(this);
    if (scoped.isLost())
        return;

    scoped.context()->populateSubscribedValuesCHROMIUM(target);
}

void CHROMIUMSubscribeUniform::uniformValuebufferCHROMIUM(const WebGLUniformLocation* location, GLenum target, GLenum subscription)
{
    WebGLExtensionScopedContext scoped(this);
    if (scoped.isLost() || !location)
        return;

    scoped.context()->uniformValuebufferCHROMIUM(location, target, subscription);
}

} // namespace blink
