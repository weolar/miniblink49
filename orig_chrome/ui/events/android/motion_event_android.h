
// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_EVENTS_ANDROID_MOTION_EVENT_ANDROID_H_
#define UI_EVENTS_ANDROID_MOTION_EVENT_ANDROID_H_

#include <jni.h>
#include <stddef.h>
#include <stdint.h>

#include "base/android/scoped_java_ref.h"
#include "base/macros.h"
#include "base/memory/scoped_ptr.h"
#include "base/time/time.h"
#include "ui/events/events_export.h"
#include "ui/events/gesture_detection/motion_event.h"
#include "ui/gfx/geometry/point_f.h"

namespace ui {

// Implementation of ui::MotionEvent wrapping a native Android MotionEvent.
// All *input* coordinates are in device pixels (as with Android MotionEvent),
// while all *output* coordinates are in DIPs (as with WebTouchEvent).
class EVENTS_EXPORT MotionEventAndroid : public MotionEvent {
public:
    struct Pointer {
        Pointer(jint id,
            jfloat pos_x_pixels,
            jfloat pos_y_pixels,
            jfloat touch_major_pixels,
            jfloat touch_minor_pixels,
            jfloat orientation_rad,
            jfloat tilt_rad,
            jint tool_type);
        jint id;
        jfloat pos_x_pixels;
        jfloat pos_y_pixels;
        jfloat touch_major_pixels;
        jfloat touch_minor_pixels;
        jfloat orientation_rad;
        jfloat tilt_rad;
        jint tool_type;
    };

    // Forcing the caller to provide all cached values upon construction
    // eliminates the need to perform a JNI call to retrieve values individually.
    MotionEventAndroid(float pix_to_dip,
        JNIEnv* env,
        jobject event,
        jlong time_ms,
        jint android_action,
        jint pointer_count,
        jint history_size,
        jint action_index,
        jint android_button_state,
        jint meta_state,
        jfloat raw_offset_x_pixels,
        jfloat raw_offset_y_pixels,
        const Pointer& pointer0,
        const Pointer& pointer1);
    ~MotionEventAndroid() override;

    // ui::MotionEvent methods.
    uint32_t GetUniqueEventId() const override;
    Action GetAction() const override;
    int GetActionIndex() const override;
    size_t GetPointerCount() const override;
    int GetPointerId(size_t pointer_index) const override;
    float GetX(size_t pointer_index) const override;
    float GetY(size_t pointer_index) const override;
    float GetRawX(size_t pointer_index) const override;
    float GetRawY(size_t pointer_index) const override;
    float GetTouchMajor(size_t pointer_index) const override;
    float GetTouchMinor(size_t pointer_index) const override;
    float GetOrientation(size_t pointer_index) const override;
    float GetPressure(size_t pointer_index) const override;
    float GetTilt(size_t pointer_index) const override;
    base::TimeTicks GetEventTime() const override;
    size_t GetHistorySize() const override;
    base::TimeTicks GetHistoricalEventTime(
        size_t historical_index) const override;
    float GetHistoricalTouchMajor(size_t pointer_index,
        size_t historical_index) const override;
    float GetHistoricalX(size_t pointer_index,
        size_t historical_index) const override;
    float GetHistoricalY(size_t pointer_index,
        size_t historical_index) const override;
    ToolType GetToolType(size_t pointer_index) const override;
    int GetButtonState() const override;
    int GetFlags() const override;

    static bool RegisterMotionEventAndroid(JNIEnv* env);

private:
    struct CachedPointer;

    float ToDips(float pixels) const;
    CachedPointer FromAndroidPointer(const Pointer& pointer) const;

    // Cache pointer coords, id's and major lengths for the most common
    // touch-related scenarios, i.e., scrolling and pinching.  This prevents
    // redundant JNI fetches for the same bits.
    enum { MAX_POINTERS_TO_CACHE = 2 };

    // The Java reference to the underlying MotionEvent.
    base::android::ScopedJavaGlobalRef<jobject> event_;

    // Used to convert pixel coordinates from the Java-backed MotionEvent to
    // DIP coordinates cached/returned by the MotionEventAndroid.
    const float pix_to_dip_;

    const base::TimeTicks cached_time_;
    const Action cached_action_;
    const size_t cached_pointer_count_;
    const size_t cached_history_size_;
    const int cached_action_index_;
    const int cached_button_state_;
    const int cached_flags_;
    const gfx::Vector2dF cached_raw_position_offset_;
    struct CachedPointer {
        CachedPointer();
        int id;
        gfx::PointF position;
        float touch_major;
        float touch_minor;
        float orientation;
        float tilt;
        ToolType tool_type;
    } cached_pointers_[MAX_POINTERS_TO_CACHE];

    // A unique identifier for the Android motion event.
    const uint32_t unique_event_id_;

    DISALLOW_COPY_AND_ASSIGN(MotionEventAndroid);
};

} // namespace content

#endif // UI_EVENTS_ANDROID_MOTION_EVENT_ANDROID_H_
