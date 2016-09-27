// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#include "modules/webgl/WebGLSync.h"

#include "modules/webgl/WebGL2RenderingContextBase.h"

namespace blink {

WebGLSync::~WebGLSync()
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

WebGLSync::WebGLSync(WebGL2RenderingContextBase* ctx, WGC3Dsync object, GLenum objectType)
    : WebGLSharedObject(ctx)
    , m_object(object)
    , m_objectType(objectType)
{
}

void WebGLSync::deleteObjectImpl(WebGraphicsContext3D* context3d)
{
    context3d->deleteSync(m_object);
    m_object = 0;
}

} // namespace blink
