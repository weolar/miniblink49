// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebGLSync_h
#define WebGLSync_h

#include "modules/webgl/WebGLSharedObject.h"
#include "public/platform/WebGraphicsContext3D.h"

namespace blink {

class WebGL2RenderingContextBase;

class WebGLSync : public WebGLSharedObject {
    DEFINE_WRAPPERTYPEINFO();
public:
    ~WebGLSync() override;

    WGC3Dsync object() const { return m_object; }

protected:
    WebGLSync(WebGL2RenderingContextBase*, WGC3Dsync, GLenum objectType);

    bool hasObject() const override { return m_object != 0; }
    void deleteObjectImpl(WebGraphicsContext3D*) override;

    GLenum objectType() const { return m_objectType; }

private:
    bool isSync() const override { return true; }

    WGC3Dsync m_object;
    GLenum m_objectType;
};

} // namespace blink

#endif // WebGLSync_h
