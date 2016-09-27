// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef EXTShaderTextureLOD_h
#define EXTShaderTextureLOD_h

#include "modules/webgl/WebGLExtension.h"
#include "wtf/PassRefPtr.h"

namespace blink {

class EXTShaderTextureLOD final : public WebGLExtension {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<EXTShaderTextureLOD> create(WebGLRenderingContextBase*);
    static bool supported(WebGLRenderingContextBase*);
    static const char* extensionName();

    ~EXTShaderTextureLOD() override;
    WebGLExtensionName name() const override;

private:
    explicit EXTShaderTextureLOD(WebGLRenderingContextBase*);
};

} // namespace blink

#endif // EXTShaderTextureLOD_h
