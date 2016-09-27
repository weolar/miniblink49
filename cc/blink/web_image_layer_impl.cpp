// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/blink/web_image_layer_impl.h"

#include "cc/blink/web_layer_impl.h"
#include "cc/blink/web_layer_impl_fixed_bounds.h"
#include "cc/layers/picture_image_layer.h"
#include "third_party/skia/include/core/SkImage.h"

namespace cc_blink {

WebImageLayerImpl::WebImageLayerImpl() {
  layer_.reset(new WebLayerImplFixedBounds(
      cc::PictureImageLayer::Create(WebLayerImpl::LayerSettings())));
}

WebImageLayerImpl::~WebImageLayerImpl() {
}

blink::WebLayer* WebImageLayerImpl::layer() {
  return layer_.get();
}

void WebImageLayerImpl::setImage(const SkImage* image) {
  skia::RefPtr<const SkImage> imageRef = skia::SharePtr(image);
  static_cast<cc::PictureImageLayer*>(layer_->layer())
      ->SetImage(imageRef.Pass());
  static_cast<WebLayerImplFixedBounds*>(layer_.get())
      ->SetFixedBounds(gfx::Size(image->width(), image->height()));
}

void WebImageLayerImpl::setNearestNeighbor(bool nearest_neighbor) {
  static_cast<cc::PictureImageLayer*>(layer_->layer())
      ->SetNearestNeighbor(nearest_neighbor);
}

}  // namespace cc_blink
