// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_BLINK_WEB_NINE_PATCH_LAYER_IMPL_H_
#define CC_BLINK_WEB_NINE_PATCH_LAYER_IMPL_H_

#include "base/memory/scoped_ptr.h"
#include "cc/blink/cc_blink_export.h"
#include "third_party/WebKit/public/platform/WebNinePatchLayer.h"
#include "third_party/skia/include/core/SkBitmap.h"

namespace cc_blink {

class WebLayerImpl;

class WebNinePatchLayerImpl : public blink::WebNinePatchLayer {
public:
    CC_BLINK_EXPORT WebNinePatchLayerImpl();
    virtual ~WebNinePatchLayerImpl();

    // blink::WebNinePatchLayer implementation.
    virtual blink::WebLayer* layer();

    virtual void setBitmap(const SkBitmap& bitmap);
    virtual void setAperture(const blink::WebRect& aperture);
    virtual void setBorder(const blink::WebRect& border);

private:
    scoped_ptr<WebLayerImpl> layer_;

    DISALLOW_COPY_AND_ASSIGN(WebNinePatchLayerImpl);
};

} // namespace cc_blink

#endif // CC_BLINK_WEB_NINE_PATCH_LAYER_IMPL_H_
