// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_CAST_CAST_DEFINES_H_
#define MEDIA_CAST_CAST_DEFINES_H_

#include <stdint.h>

#include <cmath>
#include <map>
#include <set>

#include "base/basictypes.h"
#include "base/compiler_specific.h"
#include "base/logging.h"
#include "base/time/time.h"
#include "media/cast/net/cast_transport_config.h"

namespace media {
namespace cast {

    const uint32 kVideoFrequency = 90000;
    const int kMinSampleRateForEncoding = 8000;
    const uint32 kStartFrameId = UINT32_C(0xffffffff);

    // This is an important system-wide constant.  This limits how much history the
    // implementation must retain in order to process the acknowledgements of past
    // frames.
    // This value is carefully choosen such that it fits in the 8-bits range for
    // frame IDs. It is also less than half of the full 8-bits range such that we
    // can handle wrap around and compare two frame IDs.
    const int kMaxUnackedFrames = 120;

    const int64 kCastMessageUpdateIntervalMs = 33;
    const int64 kNackRepeatIntervalMs = 30;

    // Success/in-progress/failure status codes bubbled up to clients via
    // StatusChangeCallbacks.
    enum OperationalStatus {
        // Client should not send frames yet (sender), or should not expect to receive
        // frames yet (receiver).
        STATUS_UNINITIALIZED,

        // Client may now send or receive frames.
        STATUS_INITIALIZED,

        // Codec is being re-initialized.  Client may continue sending frames, but
        // some may be ignored/dropped until a transition back to STATUS_INITIALIZED.
        STATUS_CODEC_REINIT_PENDING,

        // Session has halted due to invalid configuration.
        STATUS_INVALID_CONFIGURATION,

        // Session has halted due to an unsupported codec.
        STATUS_UNSUPPORTED_CODEC,

        // Session has halted due to a codec initialization failure.  Note that this
        // can be reported after STATUS_INITIALIZED/STATUS_CODEC_REINIT_PENDING if the
        // codec was re-initialized during the session.
        STATUS_CODEC_INIT_FAILED,

        // Session has halted due to a codec runtime failure.
        STATUS_CODEC_RUNTIME_ERROR,
    };

    enum DefaultSettings {
        kDefaultAudioEncoderBitrate = 0, // This means "auto," and may mean VBR.
        kDefaultAudioSamplingRate = 48000,
        kDefaultMaxQp = 63,
        kDefaultMinQp = 4,
        kDefaultMaxFrameRate = 30,
        kDefaultNumberOfVideoBuffers = 1,
        kDefaultRtcpIntervalMs = 500,
        kDefaultRtpHistoryMs = 1000,
        kDefaultRtpMaxDelayMs = 100,
        kDefaultRtpAudioPayloadType = 127,
        kDefaultRtpVideoPayloadType = 96,
        kDefaultMinVideoBitRate = 50,
        kDefaultMaxVideoBitRate = 2000,
    };

    enum PacketType {
        kNewPacket,
        kNewPacketCompletingFrame,
        kDuplicatePacket,
        kTooOldPacket,
    };

    // kRtcpCastAllPacketsLost is used in PacketIDSet and
    // on the wire to mean that ALL packets for a particular
    // frame are lost.
    const uint16 kRtcpCastAllPacketsLost = 0xffff;

    // kRtcpCastLastPacket is used in PacketIDSet to ask for
    // the last packet of a frame to be retransmitted.
    const uint16 kRtcpCastLastPacket = 0xfffe;

    const size_t kMinLengthOfRtcp = 8;

    // Basic RTP header + cast header.
    const size_t kMinLengthOfRtp = 12 + 6;

    // Each uint16 represents one packet id within a cast frame.
    // Can also contain kRtcpCastAllPacketsLost and kRtcpCastLastPacket.
    typedef std::set<uint16> PacketIdSet;
    // Each uint8 represents one cast frame.
    typedef std::map<uint8, PacketIdSet> MissingFramesAndPacketsMap;

    // TODO(pwestin): Re-factor the functions bellow into a class with static
    // methods.

    // January 1970, in NTP seconds.
    // Network Time Protocol (NTP), which is in seconds relative to 0h UTC on
    // 1 January 1900.
    static const int64 kUnixEpochInNtpSeconds = INT64_C(2208988800);

    // Magic fractional unit. Used to convert time (in microseconds) to/from
    // fractional NTP seconds.
    static const double kMagicFractionalUnit = 4.294967296E3;

    // The maximum number of Cast receiver events to keep in history for the
    // purpose of sending the events through RTCP.
    // The number chosen should be more than the number of events that can be
    // stored in a RTCP packet.
    static const size_t kReceiverRtcpEventHistorySize = 512;

    inline bool IsNewerFrameId(uint32 frame_id, uint32 prev_frame_id)
    {
        return (frame_id != prev_frame_id) && static_cast<uint32>(frame_id - prev_frame_id) < 0x80000000;
    }

    inline bool IsNewerRtpTimestamp(uint32 timestamp, uint32 prev_timestamp)
    {
        return (timestamp != prev_timestamp) && static_cast<uint32>(timestamp - prev_timestamp) < 0x80000000;
    }

    inline bool IsOlderFrameId(uint32 frame_id, uint32 prev_frame_id)
    {
        return (frame_id == prev_frame_id) || IsNewerFrameId(prev_frame_id, frame_id);
    }

    inline bool IsNewerPacketId(uint16 packet_id, uint16 prev_packet_id)
    {
        return (packet_id != prev_packet_id) && static_cast<uint16>(packet_id - prev_packet_id) < 0x8000;
    }

    inline bool IsNewerSequenceNumber(uint16 sequence_number,
        uint16 prev_sequence_number)
    {
        // Same function as IsNewerPacketId just different data and name.
        return IsNewerPacketId(sequence_number, prev_sequence_number);
    }

    // Create a NTP diff from seconds and fractions of seconds; delay_fraction is
    // fractions of a second where 0x80000000 is half a second.
    inline uint32 ConvertToNtpDiff(uint32 delay_seconds, uint32 delay_fraction)
    {
        return ((delay_seconds & 0x0000FFFF) << 16) + ((delay_fraction & 0xFFFF0000) >> 16);
    }

    inline base::TimeDelta ConvertFromNtpDiff(uint32 ntp_delay)
    {
        uint32 delay_ms = (ntp_delay & 0x0000ffff) * 1000;
        delay_ms >>= 16;
        delay_ms += ((ntp_delay & 0xffff0000) >> 16) * 1000;
        return base::TimeDelta::FromMilliseconds(delay_ms);
    }

    inline void ConvertTimeToFractions(int64 ntp_time_us,
        uint32* seconds,
        uint32* fractions)
    {
        DCHECK_GE(ntp_time_us, 0) << "Time must NOT be negative";
        const int64 seconds_component = ntp_time_us / base::Time::kMicrosecondsPerSecond;
        // NTP time will overflow in the year 2036.  Also, make sure unit tests don't
        // regress and use an origin past the year 2036.  If this overflows here, the
        // inverse calculation fails to compute the correct TimeTicks value, throwing
        // off the entire system.
        DCHECK_LT(seconds_component, INT64_C(4263431296))
            << "One year left to fix the NTP year 2036 wrap-around issue!";
        *seconds = static_cast<uint32>(seconds_component);
        *fractions = static_cast<uint32>(
            (ntp_time_us % base::Time::kMicrosecondsPerSecond) * kMagicFractionalUnit);
    }

    inline void ConvertTimeTicksToNtp(const base::TimeTicks& time,
        uint32* ntp_seconds,
        uint32* ntp_fractions)
    {
        base::TimeDelta elapsed_since_unix_epoch = time - base::TimeTicks::UnixEpoch();

        int64 ntp_time_us = elapsed_since_unix_epoch.InMicroseconds() + (kUnixEpochInNtpSeconds * base::Time::kMicrosecondsPerSecond);

        ConvertTimeToFractions(ntp_time_us, ntp_seconds, ntp_fractions);
    }

    inline base::TimeTicks ConvertNtpToTimeTicks(uint32 ntp_seconds,
        uint32 ntp_fractions)
    {
        // We need to ceil() here because the calculation of |fractions| in
        // ConvertTimeToFractions() effectively does a floor().
        int64 ntp_time_us = ntp_seconds * base::Time::kMicrosecondsPerSecond + static_cast<int64>(std::ceil(ntp_fractions / kMagicFractionalUnit));

        base::TimeDelta elapsed_since_unix_epoch = base::TimeDelta::FromMicroseconds(
            ntp_time_us - (kUnixEpochInNtpSeconds * base::Time::kMicrosecondsPerSecond));
        return base::TimeTicks::UnixEpoch() + elapsed_since_unix_epoch;
    }

    inline base::TimeDelta RtpDeltaToTimeDelta(int64 rtp_delta, int rtp_timebase)
    {
        DCHECK_GT(rtp_timebase, 0);
        return rtp_delta * base::TimeDelta::FromSeconds(1) / rtp_timebase;
    }

    inline int64 TimeDeltaToRtpDelta(base::TimeDelta delta, int rtp_timebase)
    {
        DCHECK_GT(rtp_timebase, 0);
        return delta * rtp_timebase / base::TimeDelta::FromSeconds(1);
    }

} // namespace cast
} // namespace media

#endif // MEDIA_CAST_CAST_DEFINES_H_
