// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROMIUMValuebuffer_h
#define CHROMIUMValuebuffer_h

#include "modules/webgl/WebGLSharedPlatform3DObject.h"
#include "wtf/PassRefPtr.h"

namespace blink {

class CHROMIUMValuebuffer final : public WebGLSharedPlatform3DObject {
    DEFINE_WRAPPERTYPEINFO();
public:
    ~CHROMIUMValuebuffer() override;

    static PassRefPtrWillBeRawPtr<CHROMIUMValuebuffer> create(WebGLRenderingContextBase*);

    bool hasEverBeenBound() const { return m_hasEverBeenBound; }

    void setHasEverBeenBound() { m_hasEverBeenBound = true; }

protected:
    explicit CHROMIUMValuebuffer(WebGLRenderingContextBase*);

    void deleteObjectImpl(WebGraphicsContext3D*) override;

private:

    bool m_hasEverBeenBound;
};

} // namespace blink

#endif // CHROMIUMValuebuffer_h
