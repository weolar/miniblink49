// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdint.h>

#include "base/macros.h"
#include "ui/events/gestures/gesture_recognizer.h"

namespace ui {

namespace {

    // Stub implementation of GestureRecognizer for Mac. Currently only serves to
    // provide a no-op implementation of TransferEventsTo().
    class GestureRecognizerImplMac : public GestureRecognizer {
    public:
        GestureRecognizerImplMac() { }
        ~GestureRecognizerImplMac() override { }

    private:
        bool ProcessTouchEventPreDispatch(TouchEvent* event,
            GestureConsumer* consumer) override
        {
            return false;
        }

        Gestures* AckTouchEvent(uint32_t unique_event_id,
            ui::EventResult result,
            GestureConsumer* consumer) override
        {
            return NULL;
        }
        bool CleanupStateForConsumer(GestureConsumer* consumer) override
        {
            return false;
        }
        GestureConsumer* GetTouchLockedTarget(const TouchEvent& event) override
        {
            return NULL;
        }
        GestureConsumer* GetTargetForLocation(const gfx::PointF& location,
            int source_device_id) override
        {
            return NULL;
        }
        void CancelActiveTouchesExcept(GestureConsumer* not_cancelled) override { }
        void TransferEventsTo(GestureConsumer* current_consumer,
            GestureConsumer* new_consumer) override { }
        bool GetLastTouchPointForTarget(GestureConsumer* consumer,
            gfx::PointF* point) override
        {
            return false;
        }
        bool CancelActiveTouches(GestureConsumer* consumer) override { return false; }
        void AddGestureEventHelper(GestureEventHelper* helper) override { }
        void RemoveGestureEventHelper(GestureEventHelper* helper) override { }

        DISALLOW_COPY_AND_ASSIGN(GestureRecognizerImplMac);
    };

} // namespace

// static
GestureRecognizer* GestureRecognizer::Get()
{
    CR_DEFINE_STATIC_LOCAL(GestureRecognizerImplMac, instance, ());
    return &instance;
}

} // namespace ui
