// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_BLINK_CONTEXT_PROVIDER_WEB_CONTEXT_H_
#define CC_BLINK_CONTEXT_PROVIDER_WEB_CONTEXT_H_

#include "cc/output/context_provider.h"

namespace blink {
class WebGraphicsContext3D;
}

namespace cc_blink {

class ContextProviderWebContext : public cc::ContextProvider {
public:
    virtual blink::WebGraphicsContext3D* WebContext3D() = 0;

protected:
    ~ContextProviderWebContext() override { }
};

} // namespace cc_blink

#endif // CC_BLINK_CONTEXT_PROVIDER_WEB_CONTEXT_H_
