// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/cast/net/rtp/rtp_packetizer.h"

#include <string>

#include "base/big_endian.h"
#include "base/logging.h"
#include "media/cast/net/pacing/paced_sender.h"
#include "media/cast/net/rtp/rtp_defines.h"

namespace media {
namespace cast {

    RtpPacketizerConfig::RtpPacketizerConfig()
        : payload_type(-1)
        , max_payload_length(kMaxIpPacketSize - 28)
        , // Default is IP-v4/UDP.
        sequence_number(0)
        , ssrc(0)
    {
    }

    RtpPacketizerConfig::~RtpPacketizerConfig() { }

    RtpPacketizer::RtpPacketizer(PacedSender* const transport,
        PacketStorage* packet_storage,
        RtpPacketizerConfig rtp_packetizer_config)
        : config_(rtp_packetizer_config)
        , transport_(transport)
        , packet_storage_(packet_storage)
        , sequence_number_(config_.sequence_number)
        , rtp_timestamp_(0)
        , packet_id_(0)
        , send_packet_count_(0)
        , send_octet_count_(0)
    {
        DCHECK(transport) << "Invalid argument";
    }

    RtpPacketizer::~RtpPacketizer() { }

    uint16 RtpPacketizer::NextSequenceNumber()
    {
        ++sequence_number_;
        return sequence_number_ - 1;
    }

    void RtpPacketizer::SendFrameAsPackets(const EncodedFrame& frame)
    {
        uint16 rtp_header_length = kRtpHeaderLength + kCastHeaderLength;
        uint16 max_length = config_.max_payload_length - rtp_header_length - 1;
        rtp_timestamp_ = frame.rtp_timestamp;

        // Split the payload evenly (round number up).
        size_t num_packets = (frame.data.size() + max_length) / max_length;
        size_t payload_length = (frame.data.size() + num_packets) / num_packets;
        DCHECK_LE(payload_length, max_length) << "Invalid argument";

        SendPacketVector packets;

        size_t remaining_size = frame.data.size();
        std::string::const_iterator data_iter = frame.data.begin();

        uint8 num_extensions = 0;
        if (frame.new_playout_delay_ms)
            num_extensions++;
        DCHECK_LE(num_extensions, kCastExtensionCountmask);

        while (remaining_size > 0) {
            PacketRef packet(new base::RefCountedData<Packet>);

            if (remaining_size < payload_length) {
                payload_length = remaining_size;
            }
            remaining_size -= payload_length;
            BuildCommonRTPheader(
                &packet->data, remaining_size == 0, frame.rtp_timestamp);

            // Build Cast header.
            // TODO(miu): Should we always set the ref frame bit and the ref_frame_id?
            DCHECK_NE(frame.dependency, EncodedFrame::UNKNOWN_DEPENDENCY);
            uint8 byte0 = kCastReferenceFrameIdBitMask;
            if (frame.dependency == EncodedFrame::KEY)
                byte0 |= kCastKeyFrameBitMask;
            // Extensions only go on the first packet of the frame
            if (packet_id_ == 0)
                byte0 |= num_extensions;
            packet->data.push_back(byte0);
            packet->data.push_back(static_cast<uint8>(frame.frame_id));
            size_t start_size = packet->data.size();
            packet->data.resize(start_size + 4);
            base::BigEndianWriter big_endian_writer(
                reinterpret_cast<char*>(&(packet->data[start_size])), 4);
            big_endian_writer.WriteU16(packet_id_);
            big_endian_writer.WriteU16(static_cast<uint16>(num_packets - 1));
            packet->data.push_back(static_cast<uint8>(frame.referenced_frame_id));
            // Add extension details only on the first packet of the frame
            if (packet_id_ == 0 && frame.new_playout_delay_ms) {
                packet->data.push_back(kCastRtpExtensionAdaptiveLatency << 2);
                packet->data.push_back(2); // 2 bytes
                packet->data.push_back(
                    static_cast<uint8>(frame.new_playout_delay_ms >> 8));
                packet->data.push_back(
                    static_cast<uint8>(frame.new_playout_delay_ms));
            }

            // Copy payload data.
            packet->data.insert(packet->data.end(),
                data_iter,
                data_iter + payload_length);
            data_iter += payload_length;

            const PacketKey key = PacedPacketSender::MakePacketKey(frame.reference_time,
                config_.ssrc,
                packet_id_++);
            packets.push_back(make_pair(key, packet));

            // Update stats.
            ++send_packet_count_;
            send_octet_count_ += payload_length;
        }
        DCHECK(packet_id_ == num_packets) << "Invalid state";

        packet_storage_->StoreFrame(frame.frame_id, packets);

        // Send to network.
        transport_->SendPackets(packets);

        // Prepare for next frame.
        packet_id_ = 0;
    }

    void RtpPacketizer::BuildCommonRTPheader(Packet* packet,
        bool marker_bit,
        uint32 time_stamp)
    {
        packet->push_back(0x80);
        packet->push_back(static_cast<uint8>(config_.payload_type) | (marker_bit ? kRtpMarkerBitMask : 0));
        size_t start_size = packet->size();
        packet->resize(start_size + 10);
        base::BigEndianWriter big_endian_writer(
            reinterpret_cast<char*>(&((*packet)[start_size])), 10);
        big_endian_writer.WriteU16(sequence_number_);
        big_endian_writer.WriteU32(time_stamp);
        big_endian_writer.WriteU32(config_.ssrc);
        ++sequence_number_;
    }

} // namespace cast
} // namespace media
