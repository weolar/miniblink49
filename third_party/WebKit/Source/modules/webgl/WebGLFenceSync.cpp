// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#include "modules/webgl/WebGLFenceSync.h"

#include "modules/webgl/WebGL2RenderingContextBase.h"

namespace blink {

PassRefPtrWillBeRawPtr<WebGLSync> WebGLFenceSync::create(WebGL2RenderingContextBase* ctx, GLenum condition, GLbitfield flags)
{
    return adoptRefWillBeNoop(new WebGLFenceSync(ctx, condition, flags));
}

WebGLFenceSync::~WebGLFenceSync()
{
}

WebGLFenceSync::WebGLFenceSync(WebGL2RenderingContextBase* ctx, GLenum condition, GLbitfield flags)
    : WebGLSync(ctx, ctx->webContext()->fenceSync(condition, flags), GL_SYNC_FENCE)
{
}

} // namespace blink
