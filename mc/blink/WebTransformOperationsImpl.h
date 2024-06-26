// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef mc_blink_WebTransformOperationsImpl_h
#define mc_blink_WebTransformOperationsImpl_h

#include "mc/animation/TransformOperations.h"
#include "third_party/WebKit/public/platform/WebTransformOperations.h"
#include "third_party/WebKit/Source/wtf/OwnPtr.h"
#include "third_party/WebKit/Source/wtf/PassOwnPtr.h"

namespace mc_blink {

class WebTransformOperationsImpl : public blink::WebTransformOperations {
public:
    WebTransformOperationsImpl();
    ~WebTransformOperationsImpl() override;

    const mc::TransformOperations& asTransformOperations() const;

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
    mc::TransformOperations m_transformOperations;
};

}  // namespace mc_blink

#endif  // mc_blink_WebTransformOperationsImpl_h
