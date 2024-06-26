// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/events/ozone/evdev/device_event_dispatcher_evdev.h"

namespace ui {

KeyEventParams::KeyEventParams(int device_id,
    unsigned int code,
    bool down,
    bool suppress_auto_repeat,
    base::TimeDelta timestamp)
    : device_id(device_id)
    , code(code)
    , down(down)
    , suppress_auto_repeat(suppress_auto_repeat)
    , timestamp(timestamp)
{
}

KeyEventParams::KeyEventParams(const KeyEventParams& other) = default;

KeyEventParams::~KeyEventParams()
{
}

MouseMoveEventParams::MouseMoveEventParams(int device_id,
    const gfx::PointF& location,
    const PointerDetails& details,
    base::TimeDelta timestamp)
    : device_id(device_id)
    , location(location)
    , pointer_details(details)
    , timestamp(timestamp)
{
}

MouseMoveEventParams::MouseMoveEventParams(const MouseMoveEventParams& other) = default;

MouseMoveEventParams::~MouseMoveEventParams()
{
}

MouseButtonEventParams::MouseButtonEventParams(int device_id,
    const gfx::PointF& location,
    unsigned int button,
    bool down,
    bool allow_remap,
    const PointerDetails& details,
    base::TimeDelta timestamp)
    : device_id(device_id)
    , location(location)
    , button(button)
    , down(down)
    , allow_remap(allow_remap)
    , pointer_details(details)
    , timestamp(timestamp)
{
}

MouseButtonEventParams::MouseButtonEventParams(
    const MouseButtonEventParams& other)
    = default;

MouseButtonEventParams::~MouseButtonEventParams()
{
}

MouseWheelEventParams::MouseWheelEventParams(int device_id,
    const gfx::PointF& location,
    const gfx::Vector2d& delta,
    base::TimeDelta timestamp)
    : device_id(device_id)
    , location(location)
    , delta(delta)
    , timestamp(timestamp)
{
}

MouseWheelEventParams::MouseWheelEventParams(
    const MouseWheelEventParams& other)
    = default;

MouseWheelEventParams::~MouseWheelEventParams()
{
}

PinchEventParams::PinchEventParams(int device_id,
    EventType type,
    const gfx::PointF location,
    float scale,
    const base::TimeDelta timestamp)
    : device_id(device_id)
    , type(type)
    , location(location)
    , scale(scale)
    , timestamp(timestamp)
{
}

PinchEventParams::PinchEventParams(const PinchEventParams& other) = default;

PinchEventParams::~PinchEventParams()
{
}

ScrollEventParams::ScrollEventParams(int device_id,
    EventType type,
    const gfx::PointF location,
    const gfx::Vector2dF& delta,
    const gfx::Vector2dF& ordinal_delta,
    int finger_count,
    const base::TimeDelta timestamp)
    : device_id(device_id)
    , type(type)
    , location(location)
    , delta(delta)
    , ordinal_delta(ordinal_delta)
    , finger_count(finger_count)
    , timestamp(timestamp)
{
}

ScrollEventParams::ScrollEventParams(const ScrollEventParams& other) = default;

ScrollEventParams::~ScrollEventParams()
{
}

TouchEventParams::TouchEventParams(int device_id,
    int slot,
    EventType type,
    const gfx::PointF& location,
    const PointerDetails& details,
    const base::TimeDelta& timestamp)
    : device_id(device_id)
    , slot(slot)
    , type(type)
    , location(location)
    , pointer_details(details)
    , timestamp(timestamp)
{
}

TouchEventParams::TouchEventParams(const TouchEventParams& other) = default;

TouchEventParams::~TouchEventParams()
{
}

} // namspace ui
