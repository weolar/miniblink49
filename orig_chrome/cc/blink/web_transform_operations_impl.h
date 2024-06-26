// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_BLINK_WEB_TRANSFORM_OPERATIONS_IMPL_H_
#define CC_BLINK_WEB_TRANSFORM_OPERATIONS_IMPL_H_

#include "base/memory/scoped_ptr.h"
#include "cc/animation/transform_operations.h"
#include "cc/blink/cc_blink_export.h"
#include "third_party/WebKit/public/platform/WebTransformOperations.h"

namespace cc_blink {

class WebTransformOperationsImpl : public blink::WebTransformOperations {
public:
    CC_BLINK_EXPORT WebTransformOperationsImpl();
    ~WebTransformOperationsImpl() override;

    const cc::TransformOperations& AsTransformOperations() const;

    // Implementation of blink::WebTransformOperations methods
    bool canBlendWith(const blink::WebTransformOperations& other) const override;
    void appendTranslate(double x, double y, double z) override;
    void appendRotate(double x, double y, double z, double degrees) override;
    void appendScale(double x, double y, double z) override;
    void appendSkew(double x, double y) override;
    void appendPerspective(double depth) override;
    void appendMatrix(const SkMatrix44&) override;
    void appendIdentity() override;
    bool isIdentity() const override;

private:
    cc::TransformOperations transform_operations_;

    DISALLOW_COPY_AND_ASSIGN(WebTransformOperationsImpl);
};

} // namespace cc_blink

#endif // CC_BLINK_WEB_TRANSFORM_OPERATIONS_IMPL_H_
