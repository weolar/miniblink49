// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebGLQuery_h
#define WebGLQuery_h

#include "modules/webgl/WebGLSharedPlatform3DObject.h"
#include "wtf/PassRefPtr.h"

namespace blink {

class WebGL2RenderingContextBase;

class WebGLQuery : public WebGLSharedPlatform3DObject {
    DEFINE_WRAPPERTYPEINFO();
public:
    ~WebGLQuery() override;

    static PassRefPtrWillBeRawPtr<WebGLQuery> create(WebGL2RenderingContextBase*);

    void setTarget(GLenum);
    GLenum getTarget() const { return m_target; }

protected:
    explicit WebGLQuery(WebGL2RenderingContextBase*);

    void deleteObjectImpl(WebGraphicsContext3D*) override;

private:
    bool isQuery() const override { return true; }

    GLenum m_target;
};

} // namespace blink

#endif // WebGLQuery_h
