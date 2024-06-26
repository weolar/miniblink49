// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_BLINK_WEB_LAYER_IMPL_FIXED_BOUNDS_H_
#define CC_BLINK_WEB_LAYER_IMPL_FIXED_BOUNDS_H_

#include "cc/blink/web_layer_impl.h"
#include "ui/gfx/geometry/size.h"
#include "ui/gfx/transform.h"

namespace cc_blink {

// A special implementation of WebLayerImpl for layers that its contents
// need to be automatically scaled when the bounds changes. The compositor
// can efficiently handle the bounds change of such layers if the bounds
// is fixed to a given value and the change of bounds are converted to
// transformation scales.
class WebLayerImplFixedBounds : public WebLayerImpl {
public:
    CC_BLINK_EXPORT WebLayerImplFixedBounds();
    CC_BLINK_EXPORT explicit WebLayerImplFixedBounds(
        scoped_refptr<cc::Layer> layer);
    ~WebLayerImplFixedBounds() override;

    // WebLayerImpl overrides.
    void invalidateRect(const blink::WebRect& rect) override;
    void setTransformOrigin(
        const blink::WebFloatPoint3D& transform_origin) override;
    void setBounds(const blink::WebSize& bounds) override;
    blink::WebSize bounds() const override;
    void setTransform(const SkMatrix44& transform) override;
    SkMatrix44 transform() const override;

    CC_BLINK_EXPORT void SetFixedBounds(gfx::Size bounds);

protected:
    void SetTransformInternal(const gfx::Transform& transform);
    void UpdateLayerBoundsAndTransform();

    gfx::Transform original_transform_;
    gfx::Size original_bounds_;
    gfx::Size fixed_bounds_;

private:
    DISALLOW_COPY_AND_ASSIGN(WebLayerImplFixedBounds);
};

} // namespace cc_blink

#endif // CC_BLINK_WEB_LAYER_IMPL_FIXED_BOUNDS_H_
