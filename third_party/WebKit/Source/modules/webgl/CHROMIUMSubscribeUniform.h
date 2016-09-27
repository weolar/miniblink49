// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROMIUMSubscribeUniform_h
#define CHROMIUMSubscribeUniform_h

#include "modules/webgl/WebGLExtension.h"
#include "wtf/PassRefPtr.h"

namespace blink {

class CHROMIUMValuebuffer;

class CHROMIUMSubscribeUniform final : public WebGLExtension {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<CHROMIUMSubscribeUniform> create(WebGLRenderingContextBase*);
    static bool supported(WebGLRenderingContextBase*);
    static const char* extensionName();

    ~CHROMIUMSubscribeUniform() override;
    WebGLExtensionName name() const override;

    PassRefPtrWillBeRawPtr<CHROMIUMValuebuffer> createValuebufferCHROMIUM();
    void deleteValuebufferCHROMIUM(CHROMIUMValuebuffer*);
    GLboolean isValuebufferCHROMIUM(CHROMIUMValuebuffer*);
    void bindValuebufferCHROMIUM(GLenum target, CHROMIUMValuebuffer*);

    void subscribeValueCHROMIUM(GLenum target, GLenum subscription);
    void populateSubscribedValuesCHROMIUM(GLenum target);
    void uniformValuebufferCHROMIUM(const WebGLUniformLocation*, GLenum target, GLenum subscription);

private:
    explicit CHROMIUMSubscribeUniform(WebGLRenderingContextBase*);
};

} // namespace blink

#endif // CHROMIUMSubscribeUniform_h
