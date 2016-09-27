// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/webaudio/DynamicsCompressorNode.h"

#include "core/testing/DummyPageHolder.h"
#include "modules/webaudio/OfflineAudioContext.h"
#include <gtest/gtest.h>

namespace blink {

TEST(DynamicsCompressorNodeTest, ProcessorLifetime)
{
    OwnPtr<DummyPageHolder> page = DummyPageHolder::create();
    OfflineAudioContext* context = OfflineAudioContext::create(&page->document(), 2, 1, 48000, ASSERT_NO_EXCEPTION);
    DynamicsCompressorNode* node = context->createDynamicsCompressor(ASSERT_NO_EXCEPTION);
    DynamicsCompressorHandler& handler = node->dynamicsCompressorHandler();
    EXPECT_TRUE(handler.m_dynamicsCompressor);
    AudioContext::AutoLocker locker(context);
    handler.dispose();
    // m_dynamicsCompressor should live after dispose() because an audio thread
    // is using it.
    EXPECT_TRUE(handler.m_dynamicsCompressor);
}

} // namespace blink
