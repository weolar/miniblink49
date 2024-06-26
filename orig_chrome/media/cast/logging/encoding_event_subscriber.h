// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_CAST_LOGGING_ENCODING_EVENT_SUBSCRIBER_H_
#define MEDIA_CAST_LOGGING_ENCODING_EVENT_SUBSCRIBER_H_

#include <map>

#include "base/memory/linked_ptr.h"
#include "base/threading/thread_checker.h"
#include "media/cast/logging/logging_defines.h"
#include "media/cast/logging/proto/raw_events.pb.h"
#include "media/cast/logging/raw_event_subscriber.h"

namespace media {
namespace cast {

    // Number of packets per frame recorded by the subscriber.
    // Once the max number of packets has been reached, a new aggregated proto
    // will be created.
    static const int kMaxPacketsPerFrame = 64;
    // Number of events per proto recorded by the subscriber.
    // Once the max number of events has been reached, a new aggregated proto
    // will be created.
    static const int kMaxEventsPerProto = 16;

    typedef std::vector<linked_ptr<media::cast::proto::AggregatedFrameEvent>>
        FrameEventList;
    typedef std::vector<linked_ptr<media::cast::proto::AggregatedPacketEvent>>
        PacketEventList;

    // A RawEventSubscriber implementation that subscribes to events,
    // encodes them in protocol buffer format, and aggregates them into a more
    // compact structure. Aggregation is per-frame, and uses a map with RTP
    // timestamp as key. Periodically, old entries in the map will be transferred
    // to a storage vector. This helps keep the size of the map small and
    // lookup times fast. The storage itself is a circular buffer that will
    // overwrite old entries once it has reached the size configured by user.
    class EncodingEventSubscriber : public RawEventSubscriber {
    public:
        // |event_media_type|: The subscriber will only process events that
        // corresponds to this type.
        // |max_frames|: How many events to keep in the frame / packet storage.
        // This helps keep memory usage bounded.
        // Every time one of |OnReceive[Frame,Packet]Event()| is
        // called, it will check if the respective map size has exceeded |max_frames|.
        // If so, it will remove the oldest aggregated entry (ordered by RTP
        // timestamp).
        EncodingEventSubscriber(EventMediaType event_media_type, size_t max_frames);

        ~EncodingEventSubscriber() final;

        // RawReventSubscriber implementations.
        void OnReceiveFrameEvent(const FrameEvent& frame_event) final;
        void OnReceivePacketEvent(const PacketEvent& packet_event) final;

        // Assigns frame events and packet events received so far to |frame_events|
        // and |packet_events| and resets the internal state.
        // In addition, assign metadata associated with these events to |metadata|.
        // The protos in |frame_events| and |packets_events| are sorted in
        // ascending RTP timestamp order.
        void GetEventsAndReset(media::cast::proto::LogMetadata* metadata,
            FrameEventList* frame_events,
            PacketEventList* packet_events);

    private:
        typedef std::map<RtpTimestamp,
            linked_ptr<media::cast::proto::AggregatedFrameEvent>>
            FrameEventMap;
        typedef std::map<RtpTimestamp,
            linked_ptr<media::cast::proto::AggregatedPacketEvent>>
            PacketEventMap;

        // Transfer up to |max_num_entries| smallest entries from |frame_event_map_|
        // to |frame_event_storage_|. This helps keep size of |frame_event_map_| small
        // and lookup speed fast.
        void TransferFrameEvents(size_t max_num_entries);
        // See above.
        void TransferPacketEvents(size_t max_num_entries);

        void AddFrameEventToStorage(
            const linked_ptr<media::cast::proto::AggregatedFrameEvent>&
                frame_event_proto);
        void AddPacketEventToStorage(
            const linked_ptr<media::cast::proto::AggregatedPacketEvent>&
                packet_event_proto);

        // Returns the difference between |rtp_timestamp| and |first_rtp_timestamp_|.
        // Sets |first_rtp_timestamp_| if it is not already set.
        RtpTimestamp GetRelativeRtpTimestamp(RtpTimestamp rtp_timestamp);

        // Clears the maps and first RTP timestamp seen.
        void Reset();

        const EventMediaType event_media_type_;
        const size_t max_frames_;

        FrameEventMap frame_event_map_;
        FrameEventList frame_event_storage_;
        int frame_event_storage_index_;

        PacketEventMap packet_event_map_;
        PacketEventList packet_event_storage_;
        int packet_event_storage_index_;

        // All functions must be called on the main thread.
        base::ThreadChecker thread_checker_;

        // Set to true on first event encountered after a |Reset()|.
        bool seen_first_rtp_timestamp_;

        // Set to RTP timestamp of first event encountered after a |Reset()|.
        RtpTimestamp first_rtp_timestamp_;

        DISALLOW_COPY_AND_ASSIGN(EncodingEventSubscriber);
    };

} // namespace cast
} // namespace media

#endif // MEDIA_CAST_LOGGING_ENCODING_EVENT_SUBSCRIBER_H_
