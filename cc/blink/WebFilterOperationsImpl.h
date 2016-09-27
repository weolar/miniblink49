// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebFilterOperationsImpl_h
#define WebFilterOperationsImpl_h

#include "third_party/WebKit/public/platform/WebFilterOperations.h"
#include "platform/graphics/filters/FilterOperation.h"
#include <WTF/Vector.h>

namespace cc_blink {

class WebFilterOperationsImpl : public blink::WebFilterOperations {
public:
    WebFilterOperationsImpl();
    virtual ~WebFilterOperationsImpl() OVERRIDE;

    virtual void appendGrayscaleFilter(float amount) OVERRIDE;
    virtual void appendSepiaFilter(float amount) OVERRIDE;
    virtual void appendSaturateFilter(float amount) OVERRIDE;
    virtual void appendHueRotateFilter(float amount) OVERRIDE;
    virtual void appendInvertFilter(float amount) OVERRIDE;
    virtual void appendBrightnessFilter(float amount) OVERRIDE;
    virtual void appendContrastFilter(float amount) OVERRIDE;
    virtual void appendOpacityFilter(float amount) OVERRIDE;
    virtual void appendBlurFilter(float amount) OVERRIDE;
    virtual void appendDropShadowFilter(blink::WebPoint offset, float stdDeviation, blink::WebColor) OVERRIDE;
    virtual void appendColorMatrixFilter(SkScalar matrix[20]) OVERRIDE;
    virtual void appendZoomFilter(float amount, int inset) OVERRIDE;
    virtual void appendSaturatingBrightnessFilter(float amount) OVERRIDE;

    // This grabs a ref on the passed-in filter.
    virtual void appendReferenceFilter(SkImageFilter*)OVERRIDE;

    virtual void clear() OVERRIDE;
    virtual bool isEmpty() const OVERRIDE;

protected:
    WTF::Vector<blink::FilterOperation::OperationType> m_operationTypes;
};

} // cc_blink

#endif // WebFilterOperationsImpl_h
