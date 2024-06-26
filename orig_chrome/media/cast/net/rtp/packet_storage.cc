// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "media/cast/net/rtp/packet_storage.h"

#include "base/logging.h"
#include "media/cast/cast_defines.h"

namespace media {
namespace cast {

    PacketStorage::PacketStorage()
        : first_frame_id_in_list_(0)
        , zombie_count_(0)
    {
    }

    PacketStorage::~PacketStorage()
    {
    }

    size_t PacketStorage::GetNumberOfStoredFrames() const
    {
        return frames_.size() - zombie_count_;
    }

    void PacketStorage::StoreFrame(uint32 frame_id,
        const SendPacketVector& packets)
    {
        if (packets.empty()) {
            NOTREACHED();
            return;
        }

        if (frames_.empty()) {
            first_frame_id_in_list_ = frame_id;
        } else {
            // Make sure frame IDs are consecutive.
            DCHECK_EQ(first_frame_id_in_list_ + static_cast<uint32>(frames_.size()),
                frame_id);
            // Make sure we aren't being asked to store more frames than the system's
            // design limit.
            DCHECK_LT(frames_.size(), static_cast<size_t>(kMaxUnackedFrames));
        }

        // Save new frame to the end of the list.
        frames_.push_back(packets);
    }

    void PacketStorage::ReleaseFrame(uint32 frame_id)
    {
        const uint32 offset = frame_id - first_frame_id_in_list_;
        if (static_cast<int32>(offset) < 0 || offset >= frames_.size() || frames_[offset].empty()) {
            return;
        }

        frames_[offset].clear();
        ++zombie_count_;

        while (!frames_.empty() && frames_.front().empty()) {
            DCHECK_GT(zombie_count_, 0u);
            --zombie_count_;
            frames_.pop_front();
            ++first_frame_id_in_list_;
        }
    }

    const SendPacketVector* PacketStorage::GetFrame8(uint8 frame_id_8bits) const
    {
        // The requested frame ID has only 8-bits so convert the first frame ID
        // in list to match.
        uint8 index_8bits = first_frame_id_in_list_ & 0xFF;
        index_8bits = frame_id_8bits - index_8bits;
        if (index_8bits >= frames_.size())
            return NULL;
        const SendPacketVector& packets = frames_[index_8bits];
        return packets.empty() ? NULL : &packets;
    }

} // namespace cast
} // namespace media
