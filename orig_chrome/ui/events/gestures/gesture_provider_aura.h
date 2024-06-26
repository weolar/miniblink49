// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef UI_EVENTS_GESTURE_DETECTION_UI_GESTURE_PROVIDER_H_
#define UI_EVENTS_GESTURE_DETECTION_UI_GESTURE_PROVIDER_H_

#include <stdint.h>

#include "base/macros.h"
#include "base/memory/scoped_vector.h"
#include "ui/events/event.h"
#include "ui/events/events_export.h"
#include "ui/events/gesture_detection/filtered_gesture_provider.h"
#include "ui/events/gesture_detection/gesture_event_data_packet.h"
#include "ui/events/gesture_detection/touch_disposition_gesture_filter.h"
#include "ui/events/gestures/motion_event_aura.h"

namespace ui {

class GestureProviderAura;

class EVENTS_EXPORT GestureProviderAuraClient {
public:
    virtual ~GestureProviderAuraClient() { }
    virtual void OnGestureEvent(GestureConsumer* consumer,
        GestureEvent* event)
        = 0;
};

// Provides gesture detection and dispatch given a sequence of touch events
// and touch event acks.
class EVENTS_EXPORT GestureProviderAura : public GestureProviderClient {
public:
    GestureProviderAura(GestureConsumer* consumer,
        GestureProviderAuraClient* client);
    ~GestureProviderAura() override;

    void set_gesture_consumer(GestureConsumer* consumer)
    {
        gesture_consumer_ = consumer;
    }

    bool OnTouchEvent(TouchEvent* event);
    void OnTouchEventAck(uint32_t unique_event_id, bool event_consumed);
    const MotionEventAura& pointer_state() { return pointer_state_; }
    ScopedVector<GestureEvent>* GetAndResetPendingGestures();

    // GestureProviderClient implementation
    void OnGestureEvent(const GestureEventData& gesture) override;

private:
    GestureProviderAuraClient* client_;
    MotionEventAura pointer_state_;
    FilteredGestureProvider filtered_gesture_provider_;

    bool handling_event_;
    ScopedVector<GestureEvent> pending_gestures_;

    // |gesture_consumer_| must outlive this object.
    GestureConsumer* gesture_consumer_;

    DISALLOW_COPY_AND_ASSIGN(GestureProviderAura);
};

} // namespace ui

#endif // UI_EVENTS_GESTURE_DETECTION_UI_GESTURE_PROVIDER_H_
