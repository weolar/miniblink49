// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#if ENABLE(WEB_AUDIO)

#include "platform/audio/StereoPanner.h"

#include "platform/audio/AudioBus.h"
#include "platform/audio/AudioUtilities.h"
#include "wtf/MathExtras.h"
#include <algorithm>

// Use a 50ms smoothing / de-zippering time-constant.
const float SmoothingTimeConstant = 0.050f;

namespace blink {

// Implement equal-power panning algorithm for mono or stereo input.
// See: http://webaudio.github.io/web-audio-api/#panning-algorithm

StereoPanner::StereoPanner(float sampleRate) : Spatializer(PanningModelEqualPower)
    , m_isFirstRender(true)
    , m_pan(0.0)
{
    // Convert smoothing time (50ms) to a per-sample time value.
    m_smoothingConstant = AudioUtilities::discreteTimeConstantForSampleRate(SmoothingTimeConstant, sampleRate);
}

void StereoPanner::panWithSampleAccurateValues(const AudioBus* inputBus, AudioBus* outputBus, const float* panValues, size_t framesToProcess)
{
    bool isInputSafe = inputBus
        && (inputBus->numberOfChannels() == 1 || inputBus->numberOfChannels() == 2)
        && framesToProcess <= inputBus->length();
    ASSERT(isInputSafe);
    if (!isInputSafe)
        return;

    unsigned numberOfInputChannels = inputBus->numberOfChannels();

    bool isOutputSafe = outputBus
        && outputBus->numberOfChannels() == 2
        && framesToProcess <= outputBus->length();
    ASSERT(isOutputSafe);
    if (!isOutputSafe)
        return;

    const float* sourceL = inputBus->channel(0)->data();
    const float* sourceR = numberOfInputChannels > 1 ? inputBus->channel(1)->data() : sourceL;
    float* destinationL = outputBus->channelByType(AudioBus::ChannelLeft)->mutableData();
    float* destinationR = outputBus->channelByType(AudioBus::ChannelRight)->mutableData();

    if (!sourceL || !sourceR || !destinationL || !destinationR)
        return;

    double gainL, gainR, panRadian;

    int n = framesToProcess;

    if (numberOfInputChannels == 1) { // For mono source case.
        while (n--) {
            float inputL = *sourceL++;
            m_pan = clampTo(*panValues++, -1.0, 1.0);
            // Pan from left to right [-1; 1] will be normalized as [0; 1].
            panRadian = (m_pan * 0.5 + 0.5) * piOverTwoDouble;
            gainL = std::cos(panRadian);
            gainR = std::sin(panRadian);
            *destinationL++ = static_cast<float>(inputL * gainL);
            *destinationR++ = static_cast<float>(inputL * gainR);
        }
    } else { // For stereo source case.
        while (n--) {
            float inputL = *sourceL++;
            float inputR = *sourceR++;
            m_pan = clampTo(*panValues++, -1.0, 1.0);
            // Normalize [-1; 0] to [0; 1]. Do nothing when [0; 1].
            panRadian = (m_pan <= 0 ? m_pan + 1 : m_pan) * piOverTwoDouble;
            gainL = std::cos(panRadian);
            gainR = std::sin(panRadian);
            if (m_pan <= 0) {
                *destinationL++ = static_cast<float>(inputL + inputR * gainL);
                *destinationR++ = static_cast<float>(inputR * gainR);
            } else {
                *destinationL++ = static_cast<float>(inputL * gainL);
                *destinationR++ = static_cast<float>(inputR + inputL * gainR);
            }
        }
    }
}

void StereoPanner::panToTargetValue(const AudioBus* inputBus, AudioBus* outputBus, float panValue, size_t framesToProcess)
{
    bool isInputSafe = inputBus
        && (inputBus->numberOfChannels() == 1 || inputBus->numberOfChannels() == 2)
        && framesToProcess <= inputBus->length();
    ASSERT(isInputSafe);
    if (!isInputSafe)
        return;

    unsigned numberOfInputChannels = inputBus->numberOfChannels();

    bool isOutputSafe = outputBus
        && outputBus->numberOfChannels() == 2
        && framesToProcess <= outputBus->length();
    ASSERT(isOutputSafe);
    if (!isOutputSafe)
        return;

    const float* sourceL = inputBus->channel(0)->data();
    const float* sourceR = numberOfInputChannels > 1 ? inputBus->channel(1)->data() : sourceL;
    float* destinationL = outputBus->channelByType(AudioBus::ChannelLeft)->mutableData();
    float* destinationR = outputBus->channelByType(AudioBus::ChannelRight)->mutableData();

    if (!sourceL || !sourceR || !destinationL || !destinationR)
        return;

    float targetPan = clampTo(panValue, -1.0, 1.0);

    // Don't de-zipper on first render call.
    if (m_isFirstRender) {
        m_isFirstRender = false;
        m_pan = targetPan;
    }

    double gainL, gainR, panRadian;
    const double smoothingConstant = m_smoothingConstant;

    int n = framesToProcess;

    if (numberOfInputChannels == 1) { // For mono source case.
        while (n--) {
            float inputL = *sourceL++;
            m_pan += (targetPan - m_pan) * smoothingConstant;

            // Pan from left to right [-1; 1] will be normalized as [0; 1].
            panRadian = (m_pan * 0.5 + 0.5) * piOverTwoDouble;

            gainL = std::cos(panRadian);
            gainR = std::sin(panRadian);
            *destinationL++ = static_cast<float>(inputL * gainL);
            *destinationR++ = static_cast<float>(inputL * gainR);
        }
    } else { // For stereo source case.
        while (n--) {
            float inputL = *sourceL++;
            float inputR = *sourceR++;
            m_pan += (targetPan - m_pan) * smoothingConstant;

            // Normalize [-1; 0] to [0; 1] for the left pan position (<= 0), and
            // do nothing when [0; 1].
            panRadian = (m_pan <= 0 ? m_pan + 1 : m_pan) * piOverTwoDouble;

            gainL = std::cos(panRadian);
            gainR = std::sin(panRadian);

            // The pan value should be checked every sample when de-zippering.
            // See crbug.com/470559.
            if (m_pan <= 0) {
                // When [-1; 0], keep left channel intact and equal-power pan the
                // right channel only.
                *destinationL++ = static_cast<float>(inputL + inputR * gainL);
                *destinationR++ = static_cast<float>(inputR * gainR);
            } else {
                // When [0; 1], keep right channel intact and equal-power pan the
                // left channel only.
                *destinationL++ = static_cast<float>(inputL * gainL);
                *destinationR++ = static_cast<float>(inputR + inputL * gainR);
            }
        }
    }
}

} // namespace blink

#endif // ENABLE(WEB_AUDIO)
