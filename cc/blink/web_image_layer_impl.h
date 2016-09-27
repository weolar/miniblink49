// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_BLINK_WEB_IMAGE_LAYER_IMPL_H_
#define CC_BLINK_WEB_IMAGE_LAYER_IMPL_H_

#include "base/memory/scoped_ptr.h"
#include "cc/blink/cc_blink_export.h"
#include "third_party/WebKit/public/platform/WebImageLayer.h"

namespace cc_blink {

class WebLayerImpl;

class WebImageLayerImpl : public blink::WebImageLayer {
 public:
  CC_BLINK_EXPORT WebImageLayerImpl();
  ~WebImageLayerImpl() override;

  // blink::WebImageLayer implementation.
  blink::WebLayer* layer() override;
  void setImage(const SkImage* image) override;
  void setNearestNeighbor(bool nearest_neighbor) override;

 private:
  scoped_ptr<WebLayerImpl> layer_;

  DISALLOW_COPY_AND_ASSIGN(WebImageLayerImpl);
};

}  // namespace cc_blink

#endif  // CC_BLINK_WEB_IMAGE_LAYER_IMPL_H_
