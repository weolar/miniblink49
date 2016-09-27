// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/webaudio/AudioBasicProcessorHandler.h"

#include "core/testing/DummyPageHolder.h"
#include "modules/webaudio/OfflineAudioContext.h"
#include "platform/audio/AudioProcessor.h"
#include <gtest/gtest.h>

namespace blink {

class MockAudioProcessor final : public AudioProcessor {
public:
    MockAudioProcessor() : AudioProcessor(48000, 2) { }
    void initialize() override { m_initialized = true; }
    void uninitialize() override { m_initialized = false; }
    void process(const AudioBus*, AudioBus*, size_t) override { }
    void reset() override { }
    void setNumberOfChannels(unsigned) override { }
    unsigned numberOfChannels() const override { return m_numberOfChannels; }
    double tailTime() const override { return 0; }
    double latencyTime() const override { return 0; }
};

class MockProcessorNode final : public AudioNode {
public:
    MockProcessorNode(AudioContext& context)
        : AudioNode(context)
    {
        setHandler(AudioBasicProcessorHandler::create(AudioHandler::NodeTypeWaveShaper, *this, 48000, adoptPtr(new MockAudioProcessor())));
        handler().initialize();
    }
};

TEST(AudioBasicProcessorHandlerTest, ProcessorFinalization)
{
    OwnPtr<DummyPageHolder> page = DummyPageHolder::create();
    OfflineAudioContext* context = OfflineAudioContext::create(&page->document(), 2, 1, 48000, ASSERT_NO_EXCEPTION);
    MockProcessorNode* node = new MockProcessorNode(*context);
    AudioBasicProcessorHandler& handler = static_cast<AudioBasicProcessorHandler&>(node->handler());
    EXPECT_TRUE(handler.processor());
    EXPECT_TRUE(handler.processor()->isInitialized());
    AudioContext::AutoLocker locker(context);
    handler.dispose();
    // The AudioProcessor should live after dispose() and should not be
    // finalized because an audio thread is using it.
    EXPECT_TRUE(handler.processor());
    EXPECT_TRUE(handler.processor()->isInitialized());
}

} // namespace blink
