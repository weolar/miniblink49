// Copyright (c) 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "third_party/WebKit/Source/wtf/Atomics.h"
#include "mc/animation/AnimationIdProvider.h"

namespace mc {

int g_next_animation_id = 0;
int g_next_group_id = 0;
int g_next_timeline_id = 0;
int g_next_player_id = 0;

int AnimationIdProvider::nextAnimationId() 
{
    // Animation IDs start from 1.
    return atomicIncrement(&g_next_animation_id) + 1;
}

int AnimationIdProvider::nextGroupId()
{
    // Animation group IDs start from 1.
    return atomicIncrement(&g_next_group_id) + 1;
}

int AnimationIdProvider::nextTimelineId()
{
    return atomicIncrement(&g_next_timeline_id) + 1;
}

int AnimationIdProvider::nextPlayerId()
{
    return atomicIncrement(&g_next_player_id) + 1;
}

}  // namespace cc
