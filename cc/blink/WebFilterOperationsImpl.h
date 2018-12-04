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
    virtual ~WebFilterOperationsImpl() override;

    WebFilterOperationsImpl(const blink::WebFilterOperations&);
    WebFilterOperationsImpl(const WebFilterOperationsImpl&);

    virtual void appendGrayscaleFilter(float amount) override;
    virtual void appendSepiaFilter(float amount) override;
    virtual void appendSaturateFilter(float amount) override;
    virtual void appendHueRotateFilter(float amount) override;
    virtual void appendInvertFilter(float amount) override;
    virtual void appendBrightnessFilter(float amount) override;
    virtual void appendContrastFilter(float amount) override;
    virtual void appendOpacityFilter(float amount) override;
    virtual void appendBlurFilter(float amount) override;
    virtual void appendDropShadowFilter(blink::WebPoint offset, float stdDeviation, blink::WebColor) override;
    virtual void appendColorMatrixFilter(SkScalar matrix[20]) override;
    virtual void appendZoomFilter(float amount, int inset) override;
    virtual void appendSaturatingBrightnessFilter(float amount) override;

    // This grabs a ref on the passed-in filter.
    virtual void appendReferenceFilter(SkImageFilter*)override;

    virtual void clear() override;
    virtual bool isEmpty() const override;

    const blink::FilterOperation& at(size_t i) const;
    size_t getSize() const;

protected:
    WTF::Vector<blink::FilterOperation*> m_operations;
};

} // cc_blink

#endif // WebFilterOperationsImpl_h
