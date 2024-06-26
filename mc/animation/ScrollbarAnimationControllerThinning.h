// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_ANIMATION_SCROLLBAR_ANIMATION_CONTROLLER_THINNING_H_
#define CC_ANIMATION_SCROLLBAR_ANIMATION_CONTROLLER_THINNING_H_

#include "base/memory/scoped_ptr.h"
#include "cc/animation/scrollbar_animation_controller.h"
#include "cc/base/cc_export.h"

namespace cc {
class LayerImpl;

// Scrollbar animation that partially fades and thins after an idle delay,
// and reacts to mouse movements.
class CC_EXPORT ScrollbarAnimationControllerThinning
    : public ScrollbarAnimationController {
 public:
  static scoped_ptr<ScrollbarAnimationControllerThinning> Create(
      LayerImpl* scroll_layer,
      ScrollbarAnimationControllerClient* client,
      base::TimeDelta delay_before_starting,
      base::TimeDelta resize_delay_before_starting,
      base::TimeDelta duration);

  ~ScrollbarAnimationControllerThinning() override;

  void set_mouse_move_distance_for_test(float distance) {
    mouse_move_distance_to_trigger_animation_ = distance;
  }
  bool mouse_is_over_scrollbar() const { return mouse_is_over_scrollbar_; }
  bool mouse_is_near_scrollbar() const { return mouse_is_near_scrollbar_; }

  void DidScrollUpdate(bool on_resize) override;
  void DidMouseMoveOffScrollbar() override;
  void DidMouseMoveNear(float distance) override;

 protected:
  ScrollbarAnimationControllerThinning(
      LayerImpl* scroll_layer,
      ScrollbarAnimationControllerClient* client,
      base::TimeDelta delay_before_starting,
      base::TimeDelta resize_delay_before_starting,
      base::TimeDelta duration);

  void RunAnimationFrame(float progress) override;

 private:
  // Describes whether the current animation should INCREASE (darken / thicken)
  // a bar or DECREASE it (lighten / thin).
  enum AnimationChange {
    NONE,
    INCREASE,
    DECREASE
  };
  float OpacityAtAnimationProgress(float progress);
  float ThumbThicknessScaleAtAnimationProgress(float progress);
  float AdjustScale(float new_value,
                    float current_value,
                    AnimationChange animation_change);
  void ApplyOpacityAndThumbThicknessScale(float opacity,
                                          float thumb_thickness_scale);

  bool mouse_is_over_scrollbar_;
  bool mouse_is_near_scrollbar_;
  // Are we narrowing or thickening the bars.
  AnimationChange thickness_change_;
  // Are we darkening or lightening the bars.
  AnimationChange opacity_change_;
  // How close should the mouse be to the scrollbar before we thicken it.
  float mouse_move_distance_to_trigger_animation_;

  DISALLOW_COPY_AND_ASSIGN(ScrollbarAnimationControllerThinning);
};

}  // namespace cc

#endif  // CC_ANIMATION_SCROLLBAR_ANIMATION_CONTROLLER_THINNING_H_
