// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/blink/web_nine_patch_layer_impl.h"

#include "base/command_line.h"
#include "cc/base/switches.h"
#include "cc/blink/web_layer_impl.h"
#include "cc/blink/web_layer_impl_fixed_bounds.h"
#include "cc/layers/nine_patch_layer.h"
#include "cc/layers/picture_image_layer.h"

namespace cc_blink {

WebNinePatchLayerImpl::WebNinePatchLayerImpl()
{
    layer_.reset(new WebLayerImpl(
        cc::NinePatchLayer::Create(WebLayerImpl::LayerSettings())));
}

WebNinePatchLayerImpl::~WebNinePatchLayerImpl()
{
}

blink::WebLayer* WebNinePatchLayerImpl::layer()
{
    return layer_.get();
}

void WebNinePatchLayerImpl::setBitmap(const SkBitmap& bitmap)
{
    cc::NinePatchLayer* nine_patch = static_cast<cc::NinePatchLayer*>(layer_->layer());
    nine_patch->SetBitmap(bitmap);
}

void WebNinePatchLayerImpl::setAperture(const blink::WebRect& aperture)
{
    cc::NinePatchLayer* nine_patch = static_cast<cc::NinePatchLayer*>(layer_->layer());
    nine_patch->SetAperture(gfx::Rect(aperture));
}

void WebNinePatchLayerImpl::setBorder(const blink::WebRect& border)
{
    cc::NinePatchLayer* nine_patch = static_cast<cc::NinePatchLayer*>(layer_->layer());
    nine_patch->SetBorder(gfx::Rect(border));
}

} // namespace cc_blink
