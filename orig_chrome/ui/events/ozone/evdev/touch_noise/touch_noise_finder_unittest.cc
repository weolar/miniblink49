// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/events/ozone/evdev/touch_noise/touch_noise_finder.h"

#include <stddef.h>

#include <algorithm>

#include "base/logging.h"
#include "base/macros.h"
#include "base/memory/scoped_ptr.h"
#include "base/strings/stringprintf.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "ui/events/ozone/evdev/touch_evdev_types.h"
#include "ui/gfx/geometry/point_f.h"

namespace ui {

class TouchNoiseFinderTest : public testing::Test {
public:
    struct TouchEntry {
        int time_ms;
        size_t slot;
        bool touching;
        gfx::PointF location;
        bool expect_noise;
    };

    TouchNoiseFinderTest() { }
    ~TouchNoiseFinderTest() override { }

    bool FilterAndCheck(const TouchEntry entries[], size_t count)
    {
        std::vector<InProgressTouchEvdev> touches;
        size_t start_index = 0u;
        std::bitset<kNumTouchEvdevSlots> was_touching;
        for (size_t i = 0; i < count; ++i) {
            const TouchEntry& entry = entries[i];

            InProgressTouchEvdev touch;
            touch.x = entry.location.x();
            touch.y = entry.location.y();
            touch.tracking_id = entry.slot;
            touch.slot = entry.slot;
            touch.was_touching = was_touching.test(touch.slot);
            touch.touching = entry.touching;
            touches.push_back(touch);

            if (i == count - 1 || entry.time_ms != entries[i + 1].time_ms) {
                touch_noise_finder_->HandleTouches(
                    touches, base::TimeDelta::FromMilliseconds(entry.time_ms));

                for (size_t j = 0; j < touches.size(); ++j) {
                    bool expect_noise = entries[j + start_index].expect_noise;
                    size_t slot = touches[j].slot;
                    if (touch_noise_finder_->SlotHasNoise(slot) != expect_noise) {
                        LOG(ERROR) << base::StringPrintf(
                            "Incorrect filtering at %dms for slot %zu", entry.time_ms,
                            slot);
                        return false;
                    }
                }

                start_index = i + 1;
                touches.clear();
            }

            was_touching.set(entry.slot, entry.touching);
        }

        return true;
    }

private:
    // testing::Test:
    void SetUp() override
    {
        touch_noise_finder_.reset(new TouchNoiseFinder);
    }

    scoped_ptr<TouchNoiseFinder> touch_noise_finder_;

    DISALLOW_COPY_AND_ASSIGN(TouchNoiseFinderTest);
};

// Test that taps which are far apart in quick succession are considered noise.
TEST_F(TouchNoiseFinderTest, FarApartTaps)
{
    const TouchEntry kTestData[] = {
        { 10, 1, true, gfx::PointF(10, 10), false },
        { 20, 1, true, gfx::PointF(10, 11), false },
        { 30, 1, true, gfx::PointF(10, 12), false },
        { 30, 2, true, gfx::PointF(2500, 1000), true },
        { 40, 1, true, gfx::PointF(10, 13), true },
        { 40, 2, true, gfx::PointF(2500, 1001), true },
        { 50, 1, true, gfx::PointF(10, 14), true },
        { 50, 2, false, gfx::PointF(2500, 1002), true },
        { 60, 1, false, gfx::PointF(10, 15), true }
    };
    EXPECT_TRUE(FilterAndCheck(kTestData, arraysize(kTestData)));
}

// Test that taps which are far apart but do not occur in quick succession are
// not considered noise.
TEST_F(TouchNoiseFinderTest, FarApartTapsSlow)
{
    const TouchEntry kTestData[] = {
        { 1000, 1, true, gfx::PointF(10, 10), false },
        { 1500, 1, true, gfx::PointF(10, 11), false },
        { 2000, 1, true, gfx::PointF(10, 12), false },
        { 2500, 1, true, gfx::PointF(10, 13), false },
        { 2500, 2, true, gfx::PointF(2500, 1000), false },
        { 3000, 1, true, gfx::PointF(10, 14), false },
        { 3000, 2, false, gfx::PointF(2500, 1001), false },
        { 3500, 1, false, gfx::PointF(10, 15), false }
    };
    EXPECT_TRUE(FilterAndCheck(kTestData, arraysize(kTestData)));
}

// Test that touches which are horizontally aligned are considered noise.
TEST_F(TouchNoiseFinderTest, HorizontallyAligned)
{
    const TouchEntry kTestData[] = {
        { 10, 1, true, gfx::PointF(10, 10), false },
        { 20, 1, true, gfx::PointF(10, 10), false },
        { 20, 2, true, gfx::PointF(10, 25), true },
        { 30, 1, false, gfx::PointF(10, 10), false },
        { 30, 2, true, gfx::PointF(10, 25), true },
        { 40, 2, false, gfx::PointF(10, 25), true }
    };
    EXPECT_TRUE(FilterAndCheck(kTestData, arraysize(kTestData)));
}

// Test that touches in the same position are considered noise.
TEST_F(TouchNoiseFinderTest, SamePosition)
{
    const TouchEntry kTestData[] = {
        { 1000, 1, true, gfx::PointF(10, 10), false },
        { 1500, 1, false, gfx::PointF(10, 10), false },
        { 2000, 1, true, gfx::PointF(10, 10), false },
        { 2500, 1, false, gfx::PointF(10, 10), false },
        { 3000, 1, true, gfx::PointF(50, 50), false },
        { 3500, 1, true, gfx::PointF(50, 51), false },
        { 3500, 2, true, gfx::PointF(10, 10), true },
        { 4000, 1, false, gfx::PointF(50, 52), false },
        { 4000, 2, false, gfx::PointF(10, 10), true },
        { 4500, 1, true, gfx::PointF(10, 10), true },
        { 5000, 1, false, gfx::PointF(10, 10), true }
    };
    EXPECT_TRUE(FilterAndCheck(kTestData, arraysize(kTestData)));
}

// Test that a multi-second touch is considered noise.
TEST_F(TouchNoiseFinderTest, MultiSecondTouch)
{
    const TouchEntry kTestData[] = {
        { 1000, 1, true, gfx::PointF(10, 10), false },
        { 2000, 1, true, gfx::PointF(10, 11), false },
        { 3000, 1, true, gfx::PointF(10, 10), false },
        { 4000, 1, true, gfx::PointF(10, 11), true },
        { 5000, 1, true, gfx::PointF(10, 10), true },
        { 6000, 1, true, gfx::PointF(10, 11), true }
    };
    EXPECT_TRUE(FilterAndCheck(kTestData, arraysize(kTestData)));
}

} // namespace ui
