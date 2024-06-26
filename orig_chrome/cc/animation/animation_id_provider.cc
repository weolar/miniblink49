// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/animation/animation_id_provider.h"
#include "base/atomic_sequence_num.h"

namespace cc {

base::StaticAtomicSequenceNumber g_next_animation_id;
base::StaticAtomicSequenceNumber g_next_group_id;
base::StaticAtomicSequenceNumber g_next_timeline_id;
base::StaticAtomicSequenceNumber g_next_player_id;

int AnimationIdProvider::NextAnimationId()
{
    // Animation IDs start from 1.
    return g_next_animation_id.GetNext() + 1;
}

int AnimationIdProvider::NextGroupId()
{
    // Animation group IDs start from 1.
    return g_next_group_id.GetNext() + 1;
}

int AnimationIdProvider::NextTimelineId()
{
    return g_next_timeline_id.GetNext() + 1;
}

int AnimationIdProvider::NextPlayerId()
{
    return g_next_player_id.GetNext() + 1;
}

} // namespace cc
