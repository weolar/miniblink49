// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_BLINK_WEB_FILTER_OPERATIONS_IMPL_H_
#define CC_BLINK_WEB_FILTER_OPERATIONS_IMPL_H_

#include "cc/blink/cc_blink_export.h"
#include "cc/output/filter_operations.h"
#include "third_party/WebKit/public/platform/WebFilterOperations.h"

namespace cc_blink {

class WebFilterOperationsImpl : public blink::WebFilterOperations {
public:
    CC_BLINK_EXPORT WebFilterOperationsImpl();
    ~WebFilterOperationsImpl() override;

    const cc::FilterOperations& AsFilterOperations() const;

    // Implementation of blink::WebFilterOperations methods
    void appendGrayscaleFilter(float amount) override;
    void appendSepiaFilter(float amount) override;
    void appendSaturateFilter(float amount) override;
    void appendHueRotateFilter(float amount) override;
    void appendInvertFilter(float amount) override;
    void appendBrightnessFilter(float amount) override;
    void appendContrastFilter(float amount) override;
    void appendOpacityFilter(float amount) override;
    void appendBlurFilter(float amount) override;
    void appendDropShadowFilter(blink::WebPoint offset,
        float std_deviation,
        blink::WebColor color) override;
    void appendColorMatrixFilter(SkScalar matrix[20]) override;
    void appendZoomFilter(float amount, int inset) override;
    void appendSaturatingBrightnessFilter(float amount) override;
    void appendReferenceFilter(SkImageFilter* image_filter) override;

    void clear() override;
    bool isEmpty() const override;

private:
    cc::FilterOperations filter_operations_;

    DISALLOW_COPY_AND_ASSIGN(WebFilterOperationsImpl);
};

} // namespace cc_blink

#endif // CC_BLINK_WEB_FILTER_OPERATIONS_IMPL_H_
