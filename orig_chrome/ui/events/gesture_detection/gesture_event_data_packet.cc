// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/events/gesture_detection/gesture_event_data_packet.h"

#include "base/logging.h"
#include "ui/events/gesture_detection/motion_event.h"

namespace ui {
namespace {

    GestureEventDataPacket::GestureSource ToGestureSource(
        const ui::MotionEvent& event)
    {
        switch (event.GetAction()) {
        case ui::MotionEvent::ACTION_DOWN:
            return GestureEventDataPacket::TOUCH_SEQUENCE_START;
        case ui::MotionEvent::ACTION_UP:
            return GestureEventDataPacket::TOUCH_SEQUENCE_END;
        case ui::MotionEvent::ACTION_MOVE:
            return GestureEventDataPacket::TOUCH_MOVE;
        case ui::MotionEvent::ACTION_CANCEL:
            return GestureEventDataPacket::TOUCH_SEQUENCE_CANCEL;
        case ui::MotionEvent::ACTION_POINTER_DOWN:
            return GestureEventDataPacket::TOUCH_START;
        case ui::MotionEvent::ACTION_POINTER_UP:
            return GestureEventDataPacket::TOUCH_END;
        case ui::MotionEvent::ACTION_NONE:
            NOTREACHED();
            return GestureEventDataPacket::INVALID;
        };
        NOTREACHED();
        return GestureEventDataPacket::INVALID;
    }

} // namespace

GestureEventDataPacket::GestureEventDataPacket()
    : gesture_source_(UNDEFINED)
    , ack_state_(AckState::PENDING)
    , unique_touch_event_id_(0)
{
}

GestureEventDataPacket::GestureEventDataPacket(
    base::TimeTicks timestamp,
    GestureSource source,
    const gfx::PointF& touch_location,
    const gfx::PointF& raw_touch_location,
    uint32_t unique_touch_event_id)
    : timestamp_(timestamp)
    , touch_location_(touch_location)
    , raw_touch_location_(raw_touch_location)
    , gesture_source_(source)
    , ack_state_(AckState::PENDING)
    , unique_touch_event_id_(unique_touch_event_id)
{
    DCHECK_NE(gesture_source_, UNDEFINED);
}

GestureEventDataPacket::GestureEventDataPacket(
    const GestureEventDataPacket& other)
    : timestamp_(other.timestamp_)
    , gestures_(other.gestures_)
    , touch_location_(other.touch_location_)
    , raw_touch_location_(other.raw_touch_location_)
    , gesture_source_(other.gesture_source_)
    , ack_state_(AckState::PENDING)
    , unique_touch_event_id_(other.unique_touch_event_id_)
{
}

GestureEventDataPacket::~GestureEventDataPacket()
{
}

GestureEventDataPacket& GestureEventDataPacket::operator=(
    const GestureEventDataPacket& other)
{
    timestamp_ = other.timestamp_;
    gesture_source_ = other.gesture_source_;
    touch_location_ = other.touch_location_;
    raw_touch_location_ = other.raw_touch_location_;
    gestures_ = other.gestures_;
    ack_state_ = other.ack_state_;
    unique_touch_event_id_ = other.unique_touch_event_id_;
    return *this;
}

void GestureEventDataPacket::Push(const GestureEventData& gesture)
{
    DCHECK_NE(ET_UNKNOWN, gesture.type());
    gestures_->push_back(gesture);
}

GestureEventDataPacket GestureEventDataPacket::FromTouch(
    const ui::MotionEvent& touch)
{
    return GestureEventDataPacket(touch.GetEventTime(), ToGestureSource(touch),
        gfx::PointF(touch.GetX(), touch.GetY()),
        gfx::PointF(touch.GetRawX(), touch.GetRawY()),
        touch.GetUniqueEventId());
}

GestureEventDataPacket GestureEventDataPacket::FromTouchTimeout(
    const GestureEventData& gesture)
{
    GestureEventDataPacket packet(gesture.time, TOUCH_TIMEOUT,
        gfx::PointF(gesture.x, gesture.y),
        gfx::PointF(gesture.raw_x, gesture.raw_y), 0);
    packet.Push(gesture);
    return packet;
}

void GestureEventDataPacket::Ack(bool event_consumed)
{
    DCHECK_EQ(static_cast<int>(ack_state_), static_cast<int>(AckState::PENDING));
    ack_state_ = event_consumed ? AckState::CONSUMED : AckState::UNCONSUMED;
}

} // namespace ui
