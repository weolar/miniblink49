// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_INPUT_TOP_CONTROLS_MANAGER_H_
#define CC_INPUT_TOP_CONTROLS_MANAGER_H_

#include "base/memory/scoped_ptr.h"
#include "base/memory/weak_ptr.h"
#include "cc/input/top_controls_state.h"
#include "cc/layers/layer_impl.h"
#include "ui/gfx/geometry/size.h"
#include "ui/gfx/geometry/vector2d_f.h"

namespace base {
class TimeTicks;
}

namespace cc {

class KeyframedFloatAnimationCurve;
class LayerTreeImpl;
class TopControlsManagerClient;

// Manages the position of the top controls.
class CC_EXPORT TopControlsManager
    : public base::SupportsWeakPtr<TopControlsManager> {
public:
    enum AnimationDirection {
        NO_ANIMATION,
        SHOWING_CONTROLS,
        HIDING_CONTROLS
    };

    static scoped_ptr<TopControlsManager> Create(
        TopControlsManagerClient* client,
        float top_controls_show_threshold,
        float top_controls_hide_threshold);
    virtual ~TopControlsManager();

    float ControlsTopOffset() const;
    float ContentTopOffset() const;
    float TopControlsShownRatio() const;
    float TopControlsHeight() const;

    KeyframedFloatAnimationCurve* animation()
    {
        return top_controls_animation_.get();
    }
    AnimationDirection animation_direction() { return animation_direction_; }

    void UpdateTopControlsState(TopControlsState constraints,
        TopControlsState current,
        bool animate);

    void ScrollBegin();
    gfx::Vector2dF ScrollBy(const gfx::Vector2dF& pending_delta);
    void ScrollEnd();

    // The caller should ensure that |Pinch{Begin,End}| are called within
    // the scope of |Scroll{Begin,End}|.
    void PinchBegin();
    void PinchEnd();

    void MainThreadHasStoppedFlinging();

    gfx::Vector2dF Animate(base::TimeTicks monotonic_time);

protected:
    TopControlsManager(TopControlsManagerClient* client,
        float top_controls_show_threshold,
        float top_controls_hide_threshold);

private:
    void ResetAnimations();
    void SetupAnimation(AnimationDirection direction);
    void StartAnimationIfNecessary();
    bool IsAnimationCompleteAtTime(base::TimeTicks time);
    void ResetBaseline();

    TopControlsManagerClient* client_; // The client manages the lifecycle of
        // this.

    scoped_ptr<KeyframedFloatAnimationCurve> top_controls_animation_;
    AnimationDirection animation_direction_;
    TopControlsState permitted_state_;

    // Accumulated scroll delta since last baseline reset
    float accumulated_scroll_delta_;

    // Content offset when last baseline reset occurred
    float baseline_content_offset_;

    // The percent height of the visible top control such that it must be shown
    // when the user stops the scroll.
    float top_controls_show_threshold_;

    // The percent height of the visible top control such that it must be hidden
    // when the user stops the scroll.
    float top_controls_hide_threshold_;

    bool pinch_gesture_active_;

    DISALLOW_COPY_AND_ASSIGN(TopControlsManager);
};

} // namespace cc

#endif // CC_INPUT_TOP_CONTROLS_MANAGER_H_
