// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebGLSharedPlatform3DObject_h
#define WebGLSharedPlatform3DObject_h

#include "modules/webgl/WebGLSharedObject.h"
#include "wtf/PassRefPtr.h"

namespace blink {

class WebGLRenderingContextBase;

class WebGLSharedPlatform3DObject : public WebGLSharedObject {
public:
    ~WebGLSharedPlatform3DObject() override;

    Platform3DObject object() const { return m_object; }
    void setObject(Platform3DObject);

protected:
    explicit WebGLSharedPlatform3DObject(WebGLRenderingContextBase*);

    bool hasObject() const override;

    Platform3DObject m_object;
};

} // namespace blink

#endif // WebGLSharedPlatform3DObject_h
