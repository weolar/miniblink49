// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/blink/web_filter_operations_impl.h"

#include "base/basictypes.h"
#include "skia/ext/refptr.h"
#include "third_party/WebKit/public/platform/WebColor.h"
#include "third_party/WebKit/public/platform/WebPoint.h"
#include "third_party/skia/include/core/SkScalar.h"

namespace cc_blink {

WebFilterOperationsImpl::WebFilterOperationsImpl()
{
}

WebFilterOperationsImpl::~WebFilterOperationsImpl()
{
}

const cc::FilterOperations& WebFilterOperationsImpl::AsFilterOperations()
    const
{
    return filter_operations_;
}

void WebFilterOperationsImpl::appendGrayscaleFilter(float amount)
{
    filter_operations_.Append(cc::FilterOperation::CreateGrayscaleFilter(amount));
}

void WebFilterOperationsImpl::appendSepiaFilter(float amount)
{
    filter_operations_.Append(cc::FilterOperation::CreateSepiaFilter(amount));
}

void WebFilterOperationsImpl::appendSaturateFilter(float amount)
{
    filter_operations_.Append(cc::FilterOperation::CreateSaturateFilter(amount));
}

void WebFilterOperationsImpl::appendHueRotateFilter(float amount)
{
    filter_operations_.Append(cc::FilterOperation::CreateHueRotateFilter(amount));
}

void WebFilterOperationsImpl::appendInvertFilter(float amount)
{
    filter_operations_.Append(cc::FilterOperation::CreateInvertFilter(amount));
}

void WebFilterOperationsImpl::appendBrightnessFilter(float amount)
{
    filter_operations_.Append(
        cc::FilterOperation::CreateBrightnessFilter(amount));
}

void WebFilterOperationsImpl::appendContrastFilter(float amount)
{
    filter_operations_.Append(cc::FilterOperation::CreateContrastFilter(amount));
}

void WebFilterOperationsImpl::appendOpacityFilter(float amount)
{
    filter_operations_.Append(cc::FilterOperation::CreateOpacityFilter(amount));
}

void WebFilterOperationsImpl::appendBlurFilter(float amount)
{
    filter_operations_.Append(cc::FilterOperation::CreateBlurFilter(amount));
}

void WebFilterOperationsImpl::appendDropShadowFilter(blink::WebPoint offset,
    float std_deviation,
    blink::WebColor color)
{
    filter_operations_.Append(cc::FilterOperation::CreateDropShadowFilter(
        offset, std_deviation, color));
}

void WebFilterOperationsImpl::appendColorMatrixFilter(SkScalar matrix[20])
{
    filter_operations_.Append(
        cc::FilterOperation::CreateColorMatrixFilter(matrix));
}

void WebFilterOperationsImpl::appendZoomFilter(float amount, int inset)
{
    filter_operations_.Append(
        cc::FilterOperation::CreateZoomFilter(amount, inset));
}

void WebFilterOperationsImpl::appendSaturatingBrightnessFilter(float amount)
{
    filter_operations_.Append(
        cc::FilterOperation::CreateSaturatingBrightnessFilter(amount));
}

void WebFilterOperationsImpl::appendReferenceFilter(
    SkImageFilter* image_filter)
{
    filter_operations_.Append(
        cc::FilterOperation::CreateReferenceFilter(skia::SharePtr(image_filter)));
}

void WebFilterOperationsImpl::clear()
{
    filter_operations_.Clear();
}

bool WebFilterOperationsImpl::isEmpty() const
{
    return filter_operations_.IsEmpty();
}

} // namespace cc_blink
