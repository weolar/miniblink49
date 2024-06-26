// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_CAST_CONGESTION_CONTROL_CONGESTION_CONTROL_H_
#define MEDIA_CAST_CONGESTION_CONTROL_CONGESTION_CONTROL_H_

#include "base/basictypes.h"
#include "base/memory/scoped_ptr.h"
#include "base/time/tick_clock.h"
#include "base/time/time.h"

namespace media {
namespace cast {

    class CongestionControl {
    public:
        virtual ~CongestionControl();

        // Called with latest measured rtt value.
        virtual void UpdateRtt(base::TimeDelta rtt) = 0;

        // Called with an updated target playout delay value.
        virtual void UpdateTargetPlayoutDelay(base::TimeDelta delay) = 0;

        // Called when an encoded frame is enqueued for transport.
        virtual void SendFrameToTransport(uint32 frame_id,
            size_t frame_size_in_bits,
            base::TimeTicks when)
            = 0;

        // Called when we receive an ACK for a frame.
        virtual void AckFrame(uint32 frame_id, base::TimeTicks when) = 0;

        // Returns the bitrate we should use for the next frame.  |soft_max_bitrate|
        // is a soft upper-bound applied to the computed target bitrate before the
        // hard upper- and lower-bounds are applied.
        virtual int GetBitrate(base::TimeTicks playout_time,
            base::TimeDelta playout_delay,
            int soft_max_bitrate)
            = 0;
    };

    CongestionControl* NewAdaptiveCongestionControl(
        base::TickClock* clock,
        int max_bitrate_configured,
        int min_bitrate_configured,
        double max_frame_rate);

    CongestionControl* NewFixedCongestionControl(int bitrate);

} // namespace cast
} // namespace media

#endif // MEDIA_CAST_CONGESTION_CONTROL_CONGESTION_CONTROL_H_
