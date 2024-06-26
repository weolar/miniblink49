// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <algorithm>
#include <utility>

#include "base/logging.h"
#include "base/time/tick_clock.h"
#include "media/cast/logging/receiver_time_offset_estimator_impl.h"

namespace media {
namespace cast {

    ReceiverTimeOffsetEstimatorImpl::BoundCalculator::BoundCalculator()
        : has_bound_(false)
    {
    }
    ReceiverTimeOffsetEstimatorImpl::BoundCalculator::~BoundCalculator() { }

    void ReceiverTimeOffsetEstimatorImpl::BoundCalculator::SetSent(
        uint32 rtp,
        uint32 packet_id,
        bool audio,
        base::TimeTicks t)
    {
        uint64 key = (static_cast<uint64>(rtp) << 32) | (packet_id << 1) | static_cast<uint64>(audio);
        events_[key].first = t;
        CheckUpdate(key);
    }

    void ReceiverTimeOffsetEstimatorImpl::BoundCalculator::SetReceived(
        uint32 rtp,
        uint16 packet_id,
        bool audio,
        base::TimeTicks t)
    {
        uint64 key = (static_cast<uint64>(rtp) << 32) | (packet_id << 1) | static_cast<uint64>(audio);
        events_[key].second = t;
        CheckUpdate(key);
    }

    void ReceiverTimeOffsetEstimatorImpl::BoundCalculator::UpdateBound(
        base::TimeTicks sent, base::TimeTicks received)
    {
        base::TimeDelta delta = received - sent;
        if (has_bound_) {
            if (delta < bound_) {
                bound_ = delta;
            } else {
                bound_ += (delta - bound_) / kClockDriftSpeed;
            }
        } else {
            bound_ = delta;
        }
        has_bound_ = true;
    }

    void ReceiverTimeOffsetEstimatorImpl::BoundCalculator::CheckUpdate(
        uint64 key)
    {
        const TimeTickPair& ticks = events_[key];
        if (!ticks.first.is_null() && !ticks.second.is_null()) {
            UpdateBound(ticks.first, ticks.second);
            events_.erase(key);
            return;
        }

        if (events_.size() > kMaxEventTimesMapSize) {
            EventMap::iterator i = ModMapOldest(&events_);
            if (i != events_.end()) {
                events_.erase(i);
            }
        }
    }

    ReceiverTimeOffsetEstimatorImpl::ReceiverTimeOffsetEstimatorImpl()
    {
    }

    ReceiverTimeOffsetEstimatorImpl::~ReceiverTimeOffsetEstimatorImpl()
    {
        DCHECK(thread_checker_.CalledOnValidThread());
    }

    void ReceiverTimeOffsetEstimatorImpl::OnReceiveFrameEvent(
        const FrameEvent& frame_event)
    {
        DCHECK(thread_checker_.CalledOnValidThread());
        switch (frame_event.type) {
        case FRAME_ACK_SENT:
            lower_bound_.SetSent(frame_event.rtp_timestamp,
                0,
                frame_event.media_type == AUDIO_EVENT,
                frame_event.timestamp);
            break;
        case FRAME_ACK_RECEIVED:
            lower_bound_.SetReceived(frame_event.rtp_timestamp,
                0,
                frame_event.media_type == AUDIO_EVENT,
                frame_event.timestamp);
            break;
        default:
            // Ignored
            break;
        }
    }

    bool ReceiverTimeOffsetEstimatorImpl::GetReceiverOffsetBounds(
        base::TimeDelta* lower_bound,
        base::TimeDelta* upper_bound)
    {
        if (!lower_bound_.has_bound() || !upper_bound_.has_bound())
            return false;

        *lower_bound = -lower_bound_.bound();
        *upper_bound = upper_bound_.bound();

        // Sanitize the output, we don't want the upper bound to be
        // lower than the lower bound, make them the same.
        if (upper_bound < lower_bound) {
            lower_bound += (lower_bound - upper_bound) / 2;
            upper_bound = lower_bound;
        }
        return true;
    }

    void ReceiverTimeOffsetEstimatorImpl::OnReceivePacketEvent(
        const PacketEvent& packet_event)
    {
        DCHECK(thread_checker_.CalledOnValidThread());
        switch (packet_event.type) {
        case PACKET_SENT_TO_NETWORK:
            upper_bound_.SetSent(packet_event.rtp_timestamp,
                packet_event.packet_id,
                packet_event.media_type == AUDIO_EVENT,
                packet_event.timestamp);
            break;
        case PACKET_RECEIVED:
            upper_bound_.SetReceived(packet_event.rtp_timestamp,
                packet_event.packet_id,
                packet_event.media_type == AUDIO_EVENT,
                packet_event.timestamp);
            break;
        default:
            // Ignored
            break;
        }
    }

} // namespace cast
} // namespace media
