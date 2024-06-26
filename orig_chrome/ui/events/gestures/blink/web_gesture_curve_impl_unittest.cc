// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/events/gestures/blink/web_gesture_curve_impl.h"

#include "base/memory/scoped_ptr.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/WebKit/public/platform/WebFloatSize.h"
#include "third_party/WebKit/public/platform/WebGestureCurve.h"
#include "third_party/WebKit/public/platform/WebGestureCurveTarget.h"
#include "ui/events/gestures/fling_curve.h"

using blink::WebFloatSize;
using blink::WebGestureCurve;
using blink::WebGestureCurveTarget;

namespace ui {
namespace {

    class MockGestureCurveTarget : public WebGestureCurveTarget {
    public:
        bool scrollBy(const WebFloatSize& delta,
            const WebFloatSize& velocity) override
        {
            cumulative_delta_.width += delta.width;
            cumulative_delta_.height += delta.height;
            current_velocity_ = velocity;
            return true;
        }

        const WebFloatSize& cumulative_delta() const { return cumulative_delta_; }
        const WebFloatSize& current_velocity() const { return current_velocity_; }

    private:
        WebFloatSize cumulative_delta_;
        WebFloatSize current_velocity_;
    };

} // namespace anonymous

TEST(WebGestureCurveImplTest, Basic)
{
    gfx::Vector2dF velocity(5000, 0);
    gfx::Vector2dF offset;
    base::TimeTicks time;
    auto curve = WebGestureCurveImpl::CreateFromUICurveForTesting(
        scoped_ptr<ui::GestureCurve>(new ui::FlingCurve(velocity, time)), offset);

    // coded into the create call above.
    MockGestureCurveTarget target;
    EXPECT_TRUE(curve->apply(0, &target));
    EXPECT_TRUE(curve->apply(0.25, &target));
    EXPECT_NEAR(target.current_velocity().width, 1878, 1);
    EXPECT_EQ(target.current_velocity().height, 0);
    EXPECT_GT(target.cumulative_delta().width, 0);
    EXPECT_TRUE(curve->apply(0.45, &target)); // Use non-uniform tick spacing.

    // Ensure fling persists even if successive timestamps are identical.
    gfx::Vector2dF cumulative_delta = target.cumulative_delta();
    gfx::Vector2dF current_velocity = target.current_velocity();
    EXPECT_TRUE(curve->apply(0.45, &target));
    EXPECT_EQ(cumulative_delta, gfx::Vector2dF(target.cumulative_delta()));
    EXPECT_EQ(current_velocity, gfx::Vector2dF(target.current_velocity()));

    EXPECT_TRUE(curve->apply(0.75, &target));
    EXPECT_FALSE(curve->apply(1.5, &target));
    EXPECT_NEAR(target.cumulative_delta().width, 1193, 1);
    EXPECT_EQ(target.cumulative_delta().height, 0);
    EXPECT_EQ(target.current_velocity().width, 0);
    EXPECT_EQ(target.current_velocity().height, 0);
}

} // namespace ui
