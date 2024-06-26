// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <android/input.h>
#include <stddef.h>

#include "base/android/jni_android.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/events/android/motion_event_android.h"
#include "ui/events/event_constants.h"
#include "ui/events/test/motion_event_test_utils.h"

namespace ui {
class MotionEvent;

namespace {
    const float kPixToDip = 0.5f;

    int kAndroidActionDown = AMOTION_EVENT_ACTION_DOWN;
    int kAndroidActionPointerDown = AMOTION_EVENT_ACTION_POINTER_DOWN;
    int kAndroidAltKeyDown = AMETA_ALT_ON;

    // Corresponds to TOOL_TYPE_FINGER, see
    // developer.android.com/reference/android/view/MotionEvent.html
    //     #TOOL_TYPE_FINGER.
    int kAndroidToolTypeFinger = 1;

    // Corresponds to BUTTON_PRIMARY, see
    // developer.android.com/reference/android/view/MotionEvent.html#BUTTON_PRIMARY.
    int kAndroidButtonPrimary = 1;

} // namespace

// Note that these tests avoid creating a Java instance of the MotionEvent, as
// we're primarily testing caching behavior, and the code necessary to
// construct a Java-backed MotionEvent itself adds unnecessary complexity.
TEST(MotionEventAndroidTest, Constructor)
{
    int event_time_ms = 5;
    base::TimeTicks event_time = base::TimeTicks() + base::TimeDelta::FromMilliseconds(event_time_ms);
    MotionEventAndroid::Pointer p0(
        1, 13.7f, -7.13f, 5.3f, 1.2f, 0.1f, 0.2f, kAndroidToolTypeFinger);
    MotionEventAndroid::Pointer p1(
        2, -13.7f, 7.13f, 3.5f, 12.1f, -0.1f, -0.4f, kAndroidToolTypeFinger);
    float raw_offset = -3.f;
    int pointer_count = 2;
    int history_size = 0;
    int action_index = -1;
    MotionEventAndroid event(kPixToDip,
        base::android::AttachCurrentThread(),
        nullptr,
        event_time_ms,
        kAndroidActionDown,
        pointer_count,
        history_size,
        action_index,
        kAndroidButtonPrimary,
        kAndroidAltKeyDown,
        raw_offset,
        -raw_offset,
        p0,
        p1);

    EXPECT_EQ(MotionEvent::ACTION_DOWN, event.GetAction());
    EXPECT_EQ(event_time, event.GetEventTime());
    EXPECT_EQ(p0.pos_x_pixels * kPixToDip, event.GetX(0));
    EXPECT_EQ(p0.pos_y_pixels * kPixToDip, event.GetY(0));
    EXPECT_EQ(p1.pos_x_pixels * kPixToDip, event.GetX(1));
    EXPECT_EQ(p1.pos_y_pixels * kPixToDip, event.GetY(1));
    EXPECT_FLOAT_EQ((p0.pos_x_pixels + raw_offset) * kPixToDip, event.GetRawX(0));
    EXPECT_FLOAT_EQ((p0.pos_y_pixels - raw_offset) * kPixToDip, event.GetRawY(0));
    EXPECT_FLOAT_EQ((p1.pos_x_pixels + raw_offset) * kPixToDip, event.GetRawX(1));
    EXPECT_FLOAT_EQ((p1.pos_y_pixels - raw_offset) * kPixToDip, event.GetRawY(1));
    EXPECT_EQ(p0.touch_major_pixels * kPixToDip, event.GetTouchMajor(0));
    EXPECT_EQ(p1.touch_major_pixels * kPixToDip, event.GetTouchMajor(1));
    EXPECT_EQ(p0.touch_minor_pixels * kPixToDip, event.GetTouchMinor(0));
    EXPECT_EQ(p1.touch_minor_pixels * kPixToDip, event.GetTouchMinor(1));
    EXPECT_EQ(p0.orientation_rad, event.GetOrientation(0));
    EXPECT_EQ(p1.orientation_rad, event.GetOrientation(1));
    EXPECT_EQ(p0.tilt_rad, event.GetTilt(0));
    EXPECT_EQ(p1.tilt_rad, event.GetTilt(1));
    EXPECT_EQ(p0.id, event.GetPointerId(0));
    EXPECT_EQ(p1.id, event.GetPointerId(1));
    EXPECT_EQ(MotionEvent::TOOL_TYPE_FINGER, event.GetToolType(0));
    EXPECT_EQ(MotionEvent::TOOL_TYPE_FINGER, event.GetToolType(1));
    EXPECT_EQ(MotionEvent::BUTTON_PRIMARY, event.GetButtonState());
    EXPECT_EQ(ui::EF_ALT_DOWN, event.GetFlags());
    EXPECT_EQ(static_cast<size_t>(pointer_count), event.GetPointerCount());
    EXPECT_EQ(static_cast<size_t>(history_size), event.GetHistorySize());
}

TEST(MotionEventAndroidTest, Clone)
{
    const int pointer_count = 1;
    MotionEventAndroid::Pointer p0(
        1, 13.7f, -7.13f, 5.3f, 1.2f, 0.1f, 0.2f, kAndroidToolTypeFinger);
    MotionEventAndroid::Pointer p1(0, 0, 0, 0, 0, 0, 0, 0);
    MotionEventAndroid event(kPixToDip,
        base::android::AttachCurrentThread(),
        nullptr,
        0,
        kAndroidActionDown,
        pointer_count,
        0,
        0,
        0,
        0,
        0,
        0,
        p0,
        p1);

    scoped_ptr<MotionEvent> clone = event.Clone();
    EXPECT_EQ(ui::test::ToString(event), ui::test::ToString(*clone));
}

TEST(MotionEventAndroidTest, Cancel)
{
    const int event_time_ms = 5;
    const int pointer_count = 1;
    MotionEventAndroid::Pointer p0(
        1, 13.7f, -7.13f, 5.3f, 1.2f, 0.1f, 0.2f, kAndroidToolTypeFinger);
    MotionEventAndroid::Pointer p1(0, 0, 0, 0, 0, 0, 0, 0);
    MotionEventAndroid event(kPixToDip,
        base::android::AttachCurrentThread(),
        nullptr,
        event_time_ms,
        kAndroidActionDown,
        pointer_count,
        0,
        0,
        0,
        0,
        0,
        0,
        p0,
        p1);

    scoped_ptr<MotionEvent> cancel_event = event.Cancel();
    EXPECT_EQ(MotionEvent::ACTION_CANCEL, cancel_event->GetAction());
    EXPECT_EQ(
        base::TimeTicks() + base::TimeDelta::FromMilliseconds(event_time_ms),
        cancel_event->GetEventTime());
    EXPECT_EQ(p0.pos_x_pixels * kPixToDip, cancel_event->GetX(0));
    EXPECT_EQ(p0.pos_y_pixels * kPixToDip, cancel_event->GetY(0));
    EXPECT_EQ(static_cast<size_t>(pointer_count),
        cancel_event->GetPointerCount());
    EXPECT_EQ(0U, cancel_event->GetHistorySize());
}

TEST(MotionEventAndroidTest, InvalidOrientationsSanitized)
{
    int pointer_count = 2;
    float orientation0 = 1e10f;
    float orientation1 = std::numeric_limits<float>::quiet_NaN();
    MotionEventAndroid::Pointer p0(0, 0, 0, 0, 0, orientation0, 0, 0);
    MotionEventAndroid::Pointer p1(1, 0, 0, 0, 0, orientation1, 0, 0);
    MotionEventAndroid event(kPixToDip,
        base::android::AttachCurrentThread(),
        nullptr,
        0,
        kAndroidActionDown,
        pointer_count,
        0,
        0,
        0,
        0,
        0,
        0,
        p0,
        p1);

    EXPECT_EQ(0.f, event.GetOrientation(0));
    EXPECT_EQ(0.f, event.GetOrientation(1));
}

TEST(MotionEventAndroidTest, NonEmptyHistoryForNonMoveEventsSanitized)
{
    int pointer_count = 1;
    size_t history_size = 5;
    MotionEventAndroid::Pointer p0(0, 0, 0, 0, 0, 0, 0, 0);
    MotionEventAndroid::Pointer p1(0, 0, 0, 0, 0, 0, 0, 0);
    MotionEventAndroid event(kPixToDip,
        base::android::AttachCurrentThread(),
        nullptr,
        0,
        kAndroidActionDown,
        pointer_count,
        history_size,
        0,
        0,
        0,
        0,
        0,
        p0,
        p1);

    EXPECT_EQ(0U, event.GetHistorySize());
}

TEST(MotionEventAndroidTest, ActionIndexForPointerDown)
{
    MotionEventAndroid::Pointer p0(
        1, 13.7f, -7.13f, 5.3f, 1.2f, 0.1f, 0.2f, kAndroidToolTypeFinger);
    MotionEventAndroid::Pointer p1(
        2, -13.7f, 7.13f, 3.5f, 12.1f, -0.1f, -0.4f, kAndroidToolTypeFinger);
    int pointer_count = 2;
    int history_size = 0;
    int action_index = 1;
    MotionEventAndroid event(kPixToDip,
        base::android::AttachCurrentThread(),
        nullptr,
        0,
        kAndroidActionPointerDown,
        pointer_count,
        history_size,
        action_index,
        0,
        0,
        0,
        0,
        p0,
        p1);

    EXPECT_EQ(MotionEvent::ACTION_POINTER_DOWN, event.GetAction());
    EXPECT_EQ(action_index, event.GetActionIndex());
}

} // namespace content
