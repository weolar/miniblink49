// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#if ENABLE(WEB_AUDIO)
#include "modules/webaudio/StereoPannerNode.h"

#include "bindings/core/v8/ExceptionMessages.h"
#include "bindings/core/v8/ExceptionState.h"
#include "core/dom/ExceptionCode.h"
#include "core/dom/ExecutionContext.h"
#include "modules/webaudio/AudioContext.h"
#include "modules/webaudio/AudioNodeInput.h"
#include "modules/webaudio/AudioNodeOutput.h"
#include "platform/audio/StereoPanner.h"
#include "wtf/MathExtras.h"

namespace blink {

StereoPannerHandler::StereoPannerHandler(AudioNode& node, float sampleRate, AudioParamHandler& pan)
    : AudioHandler(NodeTypeStereoPanner, node, sampleRate)
    , m_pan(pan)
    , m_sampleAccuratePanValues(ProcessingSizeInFrames)
{
    addInput();
    addOutput(2);

    // The node-specific default mixing rules declare that StereoPannerNode
    // can handle mono to stereo and stereo to stereo conversion.
    m_channelCount = 2;
    m_channelCountMode = ClampedMax;
    m_channelInterpretation = AudioBus::Speakers;

    initialize();
}

PassRefPtr<StereoPannerHandler> StereoPannerHandler::create(AudioNode& node, float sampleRate, AudioParamHandler& pan)
{
    return adoptRef(new StereoPannerHandler(node, sampleRate, pan));
}

StereoPannerHandler::~StereoPannerHandler()
{
    uninitialize();
}

void StereoPannerHandler::process(size_t framesToProcess)
{
    AudioBus* outputBus = output(0).bus();

    if (!isInitialized() || !input(0).isConnected() || !m_stereoPanner.get()) {
        outputBus->zero();
        return;
    }

    AudioBus* inputBus = input(0).bus();
    if (!inputBus) {
        outputBus->zero();
        return;
    }

    if (m_pan->hasSampleAccurateValues()) {
        // Apply sample-accurate panning specified by AudioParam automation.
        ASSERT(framesToProcess <= m_sampleAccuratePanValues.size());
        if (framesToProcess <= m_sampleAccuratePanValues.size()) {
            float* panValues = m_sampleAccuratePanValues.data();
            m_pan->calculateSampleAccurateValues(panValues, framesToProcess);
            m_stereoPanner->panWithSampleAccurateValues(inputBus, outputBus, panValues, framesToProcess);
        }
    } else {
        m_stereoPanner->panToTargetValue(inputBus, outputBus, m_pan->value(), framesToProcess);
    }
}

void StereoPannerHandler::initialize()
{
    if (isInitialized())
        return;

    m_stereoPanner = Spatializer::create(Spatializer::PanningModelEqualPower, sampleRate());

    AudioHandler::initialize();
}

void StereoPannerHandler::setChannelCount(unsigned long channelCount, ExceptionState& exceptionState)
{
    ASSERT(isMainThread());
    AudioContext::AutoLocker locker(context());

    // A PannerNode only supports 1 or 2 channels
    if (channelCount > 0 && channelCount <= 2) {
        if (m_channelCount != channelCount) {
            m_channelCount = channelCount;
            if (m_channelCountMode != Max)
                updateChannelsForInputs();
        }
    } else {
        exceptionState.throwDOMException(
            NotSupportedError,
            ExceptionMessages::indexOutsideRange<unsigned long>(
                "channelCount",
                channelCount,
                1,
                ExceptionMessages::InclusiveBound,
                2,
                ExceptionMessages::InclusiveBound));
    }
}

void StereoPannerHandler::setChannelCountMode(const String& mode, ExceptionState& exceptionState)
{
    ASSERT(isMainThread());
    AudioContext::AutoLocker locker(context());

    ChannelCountMode oldMode = m_channelCountMode;

    if (mode == "clamped-max") {
        m_newChannelCountMode = ClampedMax;
    } else if (mode == "explicit") {
        m_newChannelCountMode = Explicit;
    } else if (mode == "max") {
        // This is not supported for a StereoPannerNode, which can only handle
        // 1 or 2 channels.
        exceptionState.throwDOMException(
            NotSupportedError,
                "StereoPanner: 'max' is not allowed");
        m_newChannelCountMode = oldMode;
    } else {
        // Do nothing for other invalid values.
        m_newChannelCountMode = oldMode;
    }

    if (m_newChannelCountMode != oldMode)
        context()->deferredTaskHandler().addChangedChannelCountMode(this);
}

// ----------------------------------------------------------------

StereoPannerNode::StereoPannerNode(AudioContext& context, float sampleRate)
    : AudioNode(context)
    , m_pan(AudioParam::create(context, 0))
{
    setHandler(StereoPannerHandler::create(*this, sampleRate, m_pan->handler()));
}

StereoPannerNode* StereoPannerNode::create(AudioContext& context, float sampleRate)
{
    return new StereoPannerNode(context, sampleRate);
}

DEFINE_TRACE(StereoPannerNode)
{
    visitor->trace(m_pan);
    AudioNode::trace(visitor);
}

AudioParam* StereoPannerNode::pan() const
{
    return m_pan;
}

} // namespace blink

#endif // ENABLE(WEB_AUDIO)
