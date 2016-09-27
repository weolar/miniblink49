// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/webaudio/ConvolverNode.h"

#include "core/testing/DummyPageHolder.h"
#include "modules/webaudio/OfflineAudioContext.h"
#include <gtest/gtest.h>

namespace blink {

TEST(ConvolverNodeTest, ReverbLifetime)
{
    OwnPtr<DummyPageHolder> page = DummyPageHolder::create();
    OfflineAudioContext* context = OfflineAudioContext::create(&page->document(), 2, 1, 48000, ASSERT_NO_EXCEPTION);
    ConvolverNode* node = context->createConvolver(ASSERT_NO_EXCEPTION);
    ConvolverHandler& handler = node->convolverHandler();
    EXPECT_FALSE(handler.m_reverb);
    node->setBuffer(AudioBuffer::create(2, 1, 48000), ASSERT_NO_EXCEPTION);
    EXPECT_TRUE(handler.m_reverb);
    AudioContext::AutoLocker locker(context);
    handler.dispose();
    // m_reverb should live after dispose() because an audio thread is using it.
    EXPECT_TRUE(handler.m_reverb);
}

} // namespace blink
