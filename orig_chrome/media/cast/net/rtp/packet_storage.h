// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef MEDIA_CAST_NET_RTP_SENDER_PACKET_STORAGE_PACKET_STORAGE_H_
#define MEDIA_CAST_NET_RTP_SENDER_PACKET_STORAGE_PACKET_STORAGE_H_

#include <deque>

#include "base/basictypes.h"
#include "media/cast/net/pacing/paced_sender.h"

namespace media {
namespace cast {

    class PacketStorage {
    public:
        PacketStorage();
        virtual ~PacketStorage();

        // Store all of the packets for a frame.
        void StoreFrame(uint32 frame_id, const SendPacketVector& packets);

        // Release all of the packets for a frame.
        void ReleaseFrame(uint32 frame_id);

        // Returns a list of packets for a frame indexed by a 8-bits ID.
        // It is the lowest 8 bits of a frame ID.
        // Returns NULL if the frame cannot be found.
        const SendPacketVector* GetFrame8(uint8 frame_id_8bits) const;

        // Get the number of stored frames.
        size_t GetNumberOfStoredFrames() const;

    private:
        std::deque<SendPacketVector> frames_;
        uint32 first_frame_id_in_list_;

        // The number of frames whose packets have been released, but the entry in the
        // |frames_| queue has not yet been popped.
        size_t zombie_count_;

        DISALLOW_COPY_AND_ASSIGN(PacketStorage);
    };

} // namespace cast
} // namespace media

#endif // MEDIA_CAST_NET_RTP_SENDER_PACKET_STORAGE_PACKET_STORAGE_H_
