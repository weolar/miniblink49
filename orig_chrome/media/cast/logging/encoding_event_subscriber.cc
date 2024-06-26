// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/cast/logging/encoding_event_subscriber.h"

#include <algorithm>
#include <cstring>
#include <utility>

#include "base/logging.h"
#include "base/numerics/safe_conversions.h"
#include "media/cast/logging/proto/proto_utils.h"

using google::protobuf::RepeatedPtrField;
using media::cast::proto::AggregatedFrameEvent;
using media::cast::proto::AggregatedPacketEvent;
using media::cast::proto::BasePacketEvent;
using media::cast::proto::LogMetadata;

namespace {

// A size limit on maps to keep lookups fast.
const size_t kMaxMapSize = 200;

// The smallest (oredered by RTP timestamp) |kNumMapEntriesToTransfer| entries
// will be moved when the map size reaches |kMaxMapSize|.
// Must be smaller than |kMaxMapSize|.
const size_t kNumMapEntriesToTransfer = 100;

template <typename ProtoPtr>
bool IsRtpTimestampLessThan(const ProtoPtr& lhs, const ProtoPtr& rhs)
{
    return lhs->relative_rtp_timestamp() < rhs->relative_rtp_timestamp();
}

BasePacketEvent* GetNewBasePacketEvent(AggregatedPacketEvent* event_proto,
    int packet_id, int size)
{
    BasePacketEvent* base = event_proto->add_base_packet_event();
    base->set_packet_id(packet_id);
    base->set_size(size);
    return base;
}

}

namespace media {
namespace cast {

    EncodingEventSubscriber::EncodingEventSubscriber(
        EventMediaType event_media_type,
        size_t max_frames)
        : event_media_type_(event_media_type)
        , max_frames_(max_frames)
        , frame_event_storage_index_(0)
        , packet_event_storage_index_(0)
        , seen_first_rtp_timestamp_(false)
        , first_rtp_timestamp_(0u)
    {
    }

    EncodingEventSubscriber::~EncodingEventSubscriber()
    {
        DCHECK(thread_checker_.CalledOnValidThread());
    }

    void EncodingEventSubscriber::OnReceiveFrameEvent(
        const FrameEvent& frame_event)
    {
        DCHECK(thread_checker_.CalledOnValidThread());

        if (event_media_type_ != frame_event.media_type)
            return;

        RtpTimestamp relative_rtp_timestamp = GetRelativeRtpTimestamp(frame_event.rtp_timestamp);
        FrameEventMap::iterator it = frame_event_map_.find(relative_rtp_timestamp);
        linked_ptr<AggregatedFrameEvent> event_proto;

        // Look up existing entry. If not found, create a new entry and add to map.
        if (it == frame_event_map_.end()) {
            event_proto.reset(new AggregatedFrameEvent);
            event_proto->set_relative_rtp_timestamp(relative_rtp_timestamp);
            frame_event_map_.insert(
                std::make_pair(relative_rtp_timestamp, event_proto));
        } else {
            event_proto = it->second;
            if (event_proto->event_type_size() >= kMaxEventsPerProto) {
                DVLOG(2) << "Too many events in frame " << frame_event.rtp_timestamp
                         << ". Using new frame event proto.";
                AddFrameEventToStorage(event_proto);
                event_proto.reset(new AggregatedFrameEvent);
                event_proto->set_relative_rtp_timestamp(relative_rtp_timestamp);
                it->second = event_proto;
            }
        }

        event_proto->add_event_type(ToProtoEventType(frame_event.type));
        event_proto->add_event_timestamp_ms(
            (frame_event.timestamp - base::TimeTicks()).InMilliseconds());

        if (frame_event.type == FRAME_CAPTURE_END) {
            if (frame_event.media_type == VIDEO_EVENT && frame_event.width > 0 && frame_event.height > 0) {
                event_proto->set_width(frame_event.width);
                event_proto->set_height(frame_event.height);
            }
        } else if (frame_event.type == FRAME_ENCODED) {
            event_proto->set_encoded_frame_size(frame_event.size);
            if (frame_event.encoder_cpu_utilization >= 0.0) {
                event_proto->set_encoder_cpu_percent_utilized(base::saturated_cast<int32>(
                    frame_event.encoder_cpu_utilization * 100.0 + 0.5));
            }
            if (frame_event.idealized_bitrate_utilization >= 0.0) {
                event_proto->set_idealized_bitrate_percent_utilized(
                    base::saturated_cast<int32>(
                        frame_event.idealized_bitrate_utilization * 100.0 + 0.5));
            }
            if (frame_event.media_type == VIDEO_EVENT) {
                event_proto->set_key_frame(frame_event.key_frame);
                event_proto->set_target_bitrate(frame_event.target_bitrate);
            }
        } else if (frame_event.type == FRAME_PLAYOUT) {
            event_proto->set_delay_millis(frame_event.delay_delta.InMilliseconds());
        }

        if (frame_event_map_.size() > kMaxMapSize)
            TransferFrameEvents(kNumMapEntriesToTransfer);

        DCHECK(frame_event_map_.size() <= kMaxMapSize);
        DCHECK(frame_event_storage_.size() <= max_frames_);
    }

    void EncodingEventSubscriber::OnReceivePacketEvent(
        const PacketEvent& packet_event)
    {
        DCHECK(thread_checker_.CalledOnValidThread());

        if (event_media_type_ != packet_event.media_type)
            return;

        RtpTimestamp relative_rtp_timestamp = GetRelativeRtpTimestamp(packet_event.rtp_timestamp);
        PacketEventMap::iterator it = packet_event_map_.find(relative_rtp_timestamp);
        linked_ptr<AggregatedPacketEvent> event_proto;
        BasePacketEvent* base_packet_event_proto = NULL;

        // Look up existing entry. If not found, create a new entry and add to map.
        if (it == packet_event_map_.end()) {
            event_proto.reset(new AggregatedPacketEvent);
            event_proto->set_relative_rtp_timestamp(relative_rtp_timestamp);
            packet_event_map_.insert(
                std::make_pair(relative_rtp_timestamp, event_proto));
            base_packet_event_proto = GetNewBasePacketEvent(
                event_proto.get(), packet_event.packet_id, packet_event.size);
        } else {
            // Found existing entry, now look up existing BasePacketEvent using packet
            // ID. If not found, create a new entry and add to proto.
            event_proto = it->second;
            RepeatedPtrField<BasePacketEvent>* field = event_proto->mutable_base_packet_event();
            for (RepeatedPtrField<BasePacketEvent>::pointer_iterator base_it = field->pointer_begin();
                 base_it != field->pointer_end();
                 ++base_it) {
                if ((*base_it)->packet_id() == packet_event.packet_id) {
                    base_packet_event_proto = *base_it;
                    break;
                }
            }
            if (!base_packet_event_proto) {
                if (event_proto->base_packet_event_size() >= kMaxPacketsPerFrame) {
                    DVLOG(3) << "Too many packets in AggregatedPacketEvent "
                             << packet_event.rtp_timestamp << ". "
                             << "Using new packet event proto.";
                    AddPacketEventToStorage(event_proto);
                    event_proto.reset(new AggregatedPacketEvent);
                    event_proto->set_relative_rtp_timestamp(relative_rtp_timestamp);
                    it->second = event_proto;
                }

                base_packet_event_proto = GetNewBasePacketEvent(
                    event_proto.get(), packet_event.packet_id, packet_event.size);
            } else if (base_packet_event_proto->event_type_size() >= kMaxEventsPerProto) {
                DVLOG(3) << "Too many events in packet "
                         << packet_event.rtp_timestamp << ", "
                         << packet_event.packet_id << ". Using new packet event proto.";
                AddPacketEventToStorage(event_proto);
                event_proto.reset(new AggregatedPacketEvent);
                event_proto->set_relative_rtp_timestamp(relative_rtp_timestamp);
                it->second = event_proto;
                base_packet_event_proto = GetNewBasePacketEvent(
                    event_proto.get(), packet_event.packet_id, packet_event.size);
            }
        }

        base_packet_event_proto->add_event_type(
            ToProtoEventType(packet_event.type));
        base_packet_event_proto->add_event_timestamp_ms(
            (packet_event.timestamp - base::TimeTicks()).InMilliseconds());

        // |base_packet_event_proto| could have been created with a receiver event
        // which does not have the packet size and we would need to overwrite it when
        // we see a sender event, which does have the packet size.
        if (packet_event.size > 0) {
            base_packet_event_proto->set_size(packet_event.size);
        }

        if (packet_event_map_.size() > kMaxMapSize)
            TransferPacketEvents(kNumMapEntriesToTransfer);

        DCHECK(packet_event_map_.size() <= kMaxMapSize);
        DCHECK(packet_event_storage_.size() <= max_frames_);
    }

    void EncodingEventSubscriber::GetEventsAndReset(LogMetadata* metadata,
        FrameEventList* frame_events, PacketEventList* packet_events)
    {
        DCHECK(thread_checker_.CalledOnValidThread());

        // Flush all events.
        TransferFrameEvents(frame_event_map_.size());
        TransferPacketEvents(packet_event_map_.size());
        std::sort(frame_event_storage_.begin(), frame_event_storage_.end(),
            &IsRtpTimestampLessThan<linked_ptr<AggregatedFrameEvent>>);
        std::sort(packet_event_storage_.begin(), packet_event_storage_.end(),
            &IsRtpTimestampLessThan<linked_ptr<AggregatedPacketEvent>>);

        metadata->set_is_audio(event_media_type_ == AUDIO_EVENT);
        metadata->set_first_rtp_timestamp(first_rtp_timestamp_);
        metadata->set_num_frame_events(frame_event_storage_.size());
        metadata->set_num_packet_events(packet_event_storage_.size());
        metadata->set_reference_timestamp_ms_at_unix_epoch(
            (base::TimeTicks::UnixEpoch() - base::TimeTicks()).InMilliseconds());
        frame_events->swap(frame_event_storage_);
        packet_events->swap(packet_event_storage_);
        Reset();
    }

    void EncodingEventSubscriber::TransferFrameEvents(size_t max_num_entries)
    {
        DCHECK(frame_event_map_.size() >= max_num_entries);

        FrameEventMap::iterator it = frame_event_map_.begin();
        for (size_t i = 0;
             i < max_num_entries && it != frame_event_map_.end();
             i++, ++it) {
            AddFrameEventToStorage(it->second);
        }

        frame_event_map_.erase(frame_event_map_.begin(), it);
    }

    void EncodingEventSubscriber::TransferPacketEvents(size_t max_num_entries)
    {
        PacketEventMap::iterator it = packet_event_map_.begin();
        for (size_t i = 0;
             i < max_num_entries && it != packet_event_map_.end();
             i++, ++it) {
            AddPacketEventToStorage(it->second);
        }

        packet_event_map_.erase(packet_event_map_.begin(), it);
    }

    void EncodingEventSubscriber::AddFrameEventToStorage(
        const linked_ptr<AggregatedFrameEvent>& frame_event_proto)
    {
        if (frame_event_storage_.size() >= max_frames_) {
            frame_event_storage_[frame_event_storage_index_] = frame_event_proto;
        } else {
            frame_event_storage_.push_back(frame_event_proto);
        }

        frame_event_storage_index_ = (frame_event_storage_index_ + 1) % max_frames_;
    }

    void EncodingEventSubscriber::AddPacketEventToStorage(
        const linked_ptr<AggregatedPacketEvent>& packet_event_proto)
    {
        if (packet_event_storage_.size() >= max_frames_)
            packet_event_storage_[packet_event_storage_index_] = packet_event_proto;
        else
            packet_event_storage_.push_back(packet_event_proto);

        packet_event_storage_index_ = (packet_event_storage_index_ + 1) % max_frames_;
    }

    RtpTimestamp EncodingEventSubscriber::GetRelativeRtpTimestamp(
        RtpTimestamp rtp_timestamp)
    {
        if (!seen_first_rtp_timestamp_) {
            seen_first_rtp_timestamp_ = true;
            first_rtp_timestamp_ = rtp_timestamp;
        }

        return rtp_timestamp - first_rtp_timestamp_;
    }

    void EncodingEventSubscriber::Reset()
    {
        frame_event_map_.clear();
        frame_event_storage_.clear();
        frame_event_storage_index_ = 0;
        packet_event_map_.clear();
        packet_event_storage_.clear();
        packet_event_storage_index_ = 0;
        seen_first_rtp_timestamp_ = false;
        first_rtp_timestamp_ = 0u;
    }

} // namespace cast
} // namespace media
