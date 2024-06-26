// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "mc/base/Tween.h"

#include <math.h>
#include <stdint.h>

#include <algorithm>

// #include "base/logging.h"
// #include "base/numerics/safe_conversions.h"
// #include "build/build_config.h"
// #include "ui/gfx/geometry/cubic_bezier.h"
// #include "ui/gfx/geometry/safe_integer_conversions.h"
#include "mc/base/CubicBezier.h"
#include "third_party/WebKit/Source/platform/geometry/IntRect.h"

#if defined(OS_WIN)
#include <float.h>
#endif

namespace mc {

// static
double Tween::calculateValue(Tween::Type type, double state) 
{
    ASSERT(state >= 0);
    ASSERT(state <= 1);

    switch (type) {
    case EASE_IN:
        return pow(state, 2);

    case EASE_IN_2:
        return pow(state, 4);

    case EASE_IN_OUT:
        if (state < 0.5)
            return pow(state * 2, 2) / 2.0;
        return 1.0 - (pow((state - 1.0) * 2, 2) / 2.0);

    case FAST_IN_OUT:
        return (pow(state - 0.5, 3) + 0.125) / 0.25;

    case LINEAR:
        return state;

    case EASE_OUT_SNAP:
        state = 0.95 * (1.0 - pow(1.0 - state, 2));
        return state;

    case EASE_OUT:
        return 1.0 - pow(1.0 - state, 2);

    case SMOOTH_IN_OUT:
        return sin(state);

    case FAST_OUT_SLOW_IN:
        return CubicBezier(0.4, 0, 0.2, 1).solve(state);

    case LINEAR_OUT_SLOW_IN:
        return CubicBezier(0, 0, .2, 1).solve(state);

    case FAST_OUT_LINEAR_IN:
        return CubicBezier(0.4, 0, 1, 1).solve(state);

    case ZERO:
        return 0;
    }

    DebugBreak();
    return state;
}

namespace {

inline int toRoundedInt(float value) {
    float rounded;
    if (value >= 0.0f)
        rounded = std::floor(value + 0.5f);
    else
        rounded = std::ceil(value - 0.5f);
    return (int)(rounded);
}

uint8_t floatToColorByte(float f) 
{
    return (uint8_t)(toRoundedInt(f * 255.f));
}

uint8_t blendColorComponents(uint8_t start, uint8_t target, float start_alpha, float target_alpha, float blended_alpha, double progress)
{
    // Since progress can be outside [0, 1], blending can produce a value outside [0, 255].
    float blended_premultiplied = Tween::floatValueBetween(progress, start / 255.f * start_alpha, target / 255.f * target_alpha);
    return floatToColorByte(blended_premultiplied / blended_alpha);
}

}  // namespace

   // static
SkColor Tween::colorValueBetween(double value, SkColor start, SkColor target)
{
    float start_a = SkColorGetA(start) / 255.f;
    float target_a = SkColorGetA(target) / 255.f;
    float blended_a = floatValueBetween(value, start_a, target_a);
    if (blended_a <= 0.f)
        return SkColorSetARGB(0, 0, 0, 0);
    blended_a = std::min(blended_a, 1.f);

    uint8_t blended_r = blendColorComponents(SkColorGetR(start), SkColorGetR(target), start_a, target_a, blended_a, value);
    uint8_t blended_g = blendColorComponents(SkColorGetG(start), SkColorGetG(target), start_a, target_a, blended_a, value);
    uint8_t blended_b = blendColorComponents(SkColorGetB(start), SkColorGetB(target), start_a, target_a, blended_a, value);

    return SkColorSetARGB(floatToColorByte(blended_a), blended_r, blended_g, blended_b);
}

// static
double Tween::doubleValueBetween(double value, double start, double target) {
    return start + (target - start) * value;
}

// static
float Tween::floatValueBetween(double value, float start, float target) {
    return static_cast<float>(start + (target - start) * value);
}

// static
int Tween::intValueBetween(double value, int start, int target) {
    if (start == target)
        return start;
    double delta = static_cast<double>(target - start);
    if (delta < 0)
        delta--;
    else
        delta++;
#if defined(OS_WIN)
    return start + static_cast<int>(value * _nextafter(delta, 0));
#else
    return start + static_cast<int>(value * nextafter(delta, 0));
#endif
}

inline int toFlooredInt(double value) {
    return (int)(std::floor(value));
}

// static
int Tween::linearIntValueBetween(double value, int start, int target) {
    // NOTE: Do not use ToRoundedInt()!  See comments on function declaration.
    return toFlooredInt(0.5 + doubleValueBetween(value, start, target));
}

// static
blink::IntRect Tween::rectValueBetween(double value, const blink::IntRect& start_bounds, const blink::IntRect& target_bounds)
{
    return blink::IntRect(
        linearIntValueBetween(value, start_bounds.x(), target_bounds.x()),
        linearIntValueBetween(value, start_bounds.y(), target_bounds.y()),
        linearIntValueBetween(value, start_bounds.width(), target_bounds.width()),
        linearIntValueBetween(value, start_bounds.height(), target_bounds.height()));
}

// static
// gfx::Transform Tween::transformValueBetween(double value, const gfx::Transform& start_transform, const gfx::Transform& end_transform) 
// {
//     if (value >= 1.0)
//         return end_transform;
//     if (value <= 0.0)
//         return start_transform;
// 
//     gfx::Transform to_return = end_transform;
//     to_return.Blend(start_transform, value);
// 
//     return to_return;
// }

}  // namespace gfx
