/*
 * Copyright (C) 2010, Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#if ENABLE(WEB_AUDIO)
#include "modules/webaudio/AudioBufferSourceNode.h"

#include "bindings/core/v8/ExceptionMessages.h"
#include "bindings/core/v8/ExceptionState.h"
#include "core/dom/ExceptionCode.h"
#include "core/frame/UseCounter.h"
#include "modules/webaudio/AudioContext.h"
#include "modules/webaudio/AudioNodeOutput.h"
#include "platform/FloatConversion.h"
#include "platform/audio/AudioUtilities.h"
#include "wtf/MainThread.h"
#include "wtf/MathExtras.h"
#include <algorithm>

namespace blink {

const double DefaultGrainDuration = 0.020; // 20ms

// Arbitrary upper limit on playback rate.
// Higher than expected rates can be useful when playing back oversampled buffers
// to minimize linear interpolation aliasing.
const double MaxRate = 1024;

// Number of extra frames to use when determining if a source node can be stopped.  This should be
// at least one rendering quantum, but we add one more quantum for good measure.  This doesn't need
// to be extra precise, just more than one rendering quantum.  See |handleStoppableSourceNode()|.
// FIXME: Expose the rendering quantum somehow instead of hardwiring a value here.
const int kExtraStopFrames = 256;

AudioBufferSourceHandler::AudioBufferSourceHandler(AudioNode& node, float sampleRate, AudioParamHandler& playbackRate, AudioParamHandler& detune)
    : AudioScheduledSourceHandler(NodeTypeAudioBufferSource, node, sampleRate)
    , m_buffer(nullptr)
    , m_playbackRate(playbackRate)
    , m_detune(detune)
    , m_isLooping(false)
    , m_loopStart(0)
    , m_loopEnd(0)
    , m_virtualReadIndex(0)
    , m_isGrain(false)
    , m_grainOffset(0.0)
    , m_grainDuration(DefaultGrainDuration)
    , m_minPlaybackRate(1.0)
{
    // Default to mono. A call to setBuffer() will set the number of output
    // channels to that of the buffer.
    addOutput(1);

    initialize();
}

PassRefPtr<AudioBufferSourceHandler> AudioBufferSourceHandler::create(AudioNode& node, float sampleRate, AudioParamHandler& playbackRate, AudioParamHandler& detune)
{
    return adoptRef(new AudioBufferSourceHandler(node, sampleRate, playbackRate, detune));
}

AudioBufferSourceHandler::~AudioBufferSourceHandler()
{
    clearPannerNode();
    uninitialize();
}

void AudioBufferSourceHandler::process(size_t framesToProcess)
{
    AudioBus* outputBus = output(0).bus();

    if (!isInitialized()) {
        outputBus->zero();
        return;
    }

    // The audio thread can't block on this lock, so we call tryLock() instead.
    MutexTryLocker tryLocker(m_processLock);
    if (tryLocker.locked()) {
        if (!buffer()) {
            outputBus->zero();
            return;
        }

        // After calling setBuffer() with a buffer having a different number of channels, there can in rare cases be a slight delay
        // before the output bus is updated to the new number of channels because of use of tryLocks() in the context's updating system.
        // In this case, if the the buffer has just been changed and we're not quite ready yet, then just output silence.
        if (numberOfChannels() != buffer()->numberOfChannels()) {
            outputBus->zero();
            return;
        }

        size_t quantumFrameOffset;
        size_t bufferFramesToProcess;

        updateSchedulingInfo(framesToProcess, outputBus, quantumFrameOffset, bufferFramesToProcess);

        if (!bufferFramesToProcess) {
            outputBus->zero();
            return;
        }

        for (unsigned i = 0; i < outputBus->numberOfChannels(); ++i)
            m_destinationChannels[i] = outputBus->channel(i)->mutableData();

        // Render by reading directly from the buffer.
        if (!renderFromBuffer(outputBus, quantumFrameOffset, bufferFramesToProcess)) {
            outputBus->zero();
            return;
        }

        outputBus->clearSilentFlag();
    } else {
        // Too bad - the tryLock() failed.  We must be in the middle of changing buffers and were already outputting silence anyway.
        outputBus->zero();
    }
}

// Returns true if we're finished.
bool AudioBufferSourceHandler::renderSilenceAndFinishIfNotLooping(AudioBus*, unsigned index, size_t framesToProcess)
{
    if (!loop()) {
        // If we're not looping, then stop playing when we get to the end.

        if (framesToProcess > 0) {
            // We're not looping and we've reached the end of the sample data, but we still need to provide more output,
            // so generate silence for the remaining.
            for (unsigned i = 0; i < numberOfChannels(); ++i)
                memset(m_destinationChannels[i] + index, 0, sizeof(float) * framesToProcess);
        }

        finish();
        return true;
    }
    return false;
}

bool AudioBufferSourceHandler::renderFromBuffer(AudioBus* bus, unsigned destinationFrameOffset, size_t numberOfFrames)
{
    ASSERT(context()->isAudioThread());

    // Basic sanity checking
    ASSERT(bus);
    ASSERT(buffer());
    if (!bus || !buffer())
        return false;

    unsigned numberOfChannels = this->numberOfChannels();
    unsigned busNumberOfChannels = bus->numberOfChannels();

    bool channelCountGood = numberOfChannels && numberOfChannels == busNumberOfChannels;
    ASSERT(channelCountGood);
    if (!channelCountGood)
        return false;

    // Sanity check destinationFrameOffset, numberOfFrames.
    size_t destinationLength = bus->length();

    bool isLengthGood = destinationLength <= 4096 && numberOfFrames <= 4096;
    ASSERT(isLengthGood);
    if (!isLengthGood)
        return false;

    bool isOffsetGood = destinationFrameOffset <= destinationLength && destinationFrameOffset + numberOfFrames <= destinationLength;
    ASSERT(isOffsetGood);
    if (!isOffsetGood)
        return false;

    // Potentially zero out initial frames leading up to the offset.
    if (destinationFrameOffset) {
        for (unsigned i = 0; i < numberOfChannels; ++i)
            memset(m_destinationChannels[i], 0, sizeof(float) * destinationFrameOffset);
    }

    // Offset the pointers to the correct offset frame.
    unsigned writeIndex = destinationFrameOffset;

    size_t bufferLength = buffer()->length();
    double bufferSampleRate = buffer()->sampleRate();

    // Avoid converting from time to sample-frames twice by computing
    // the grain end time first before computing the sample frame.
    unsigned endFrame = m_isGrain ? AudioUtilities::timeToSampleFrame(m_grainOffset + m_grainDuration, bufferSampleRate) : bufferLength;

    // This is a HACK to allow for HRTF tail-time - avoids glitch at end.
    // FIXME: implement tailTime for each AudioNode for a more general solution to this problem.
    // https://bugs.webkit.org/show_bug.cgi?id=77224
    if (m_isGrain)
        endFrame += 512;

    // Do some sanity checking.
    if (endFrame > bufferLength)
        endFrame = bufferLength;

    // If the .loop attribute is true, then values of m_loopStart == 0 && m_loopEnd == 0 implies
    // that we should use the entire buffer as the loop, otherwise use the loop values in m_loopStart and m_loopEnd.
    double virtualEndFrame = endFrame;
    double virtualDeltaFrames = endFrame;

    if (loop() && (m_loopStart || m_loopEnd) && m_loopStart >= 0 && m_loopEnd > 0 && m_loopStart < m_loopEnd) {
        // Convert from seconds to sample-frames.
        double loopStartFrame = m_loopStart * buffer()->sampleRate();
        double loopEndFrame = m_loopEnd * buffer()->sampleRate();

        virtualEndFrame = std::min(loopEndFrame, virtualEndFrame);
        virtualDeltaFrames = virtualEndFrame - loopStartFrame;
    }

    // If we're looping and the offset (virtualReadIndex) is past the end of the loop, wrap back to
    // the beginning of the loop. For other cases, nothing needs to be done.
    if (loop() && m_virtualReadIndex >= virtualEndFrame)
        m_virtualReadIndex = (m_loopStart < 0) ? 0 : (m_loopStart * buffer()->sampleRate());

    double computedPlaybackRate = computePlaybackRate();

    // Sanity check that our playback rate isn't larger than the loop size.
    if (computedPlaybackRate > virtualDeltaFrames)
        return false;

    // Get local copy.
    double virtualReadIndex = m_virtualReadIndex;

    // Render loop - reading from the source buffer to the destination using linear interpolation.
    int framesToProcess = numberOfFrames;

    const float** sourceChannels = m_sourceChannels.get();
    float** destinationChannels = m_destinationChannels.get();

    ASSERT(virtualReadIndex >= 0);
    ASSERT(virtualDeltaFrames >= 0);
    ASSERT(virtualEndFrame >= 0);

    // Optimize for the very common case of playing back with computedPlaybackRate == 1.
    // We can avoid the linear interpolation.
    if (computedPlaybackRate == 1 && virtualReadIndex == floor(virtualReadIndex)
        && virtualDeltaFrames == floor(virtualDeltaFrames)
        && virtualEndFrame == floor(virtualEndFrame)) {
        unsigned readIndex = static_cast<unsigned>(virtualReadIndex);
        unsigned deltaFrames = static_cast<unsigned>(virtualDeltaFrames);
        endFrame = static_cast<unsigned>(virtualEndFrame);
        while (framesToProcess > 0) {
            int framesToEnd = endFrame - readIndex;
            int framesThisTime = std::min(framesToProcess, framesToEnd);
            framesThisTime = std::max(0, framesThisTime);

            for (unsigned i = 0; i < numberOfChannels; ++i)
                memcpy(destinationChannels[i] + writeIndex, sourceChannels[i] + readIndex, sizeof(float) * framesThisTime);

            writeIndex += framesThisTime;
            readIndex += framesThisTime;
            framesToProcess -= framesThisTime;

            // It can happen that framesThisTime is 0. Assert that we will actually exit the loop in
            // this case.  framesThisTime is 0 only if readIndex >= endFrame;
            ASSERT(framesThisTime ? true : readIndex >= endFrame);

            // Wrap-around.
            if (readIndex >= endFrame) {
                readIndex -= deltaFrames;
                if (renderSilenceAndFinishIfNotLooping(bus, writeIndex, framesToProcess))
                    break;
            }
        }
        virtualReadIndex = readIndex;
    } else {
        while (framesToProcess--) {
            unsigned readIndex = static_cast<unsigned>(virtualReadIndex);
            double interpolationFactor = virtualReadIndex - readIndex;

            // For linear interpolation we need the next sample-frame too.
            unsigned readIndex2 = readIndex + 1;
            if (readIndex2 >= bufferLength) {
                if (loop()) {
                    // Make sure to wrap around at the end of the buffer.
                    readIndex2 = static_cast<unsigned>(virtualReadIndex + 1 - virtualDeltaFrames);
                } else {
                    readIndex2 = readIndex;
                }
            }

            // Final sanity check on buffer access.
            // FIXME: as an optimization, try to get rid of this inner-loop check and put assertions and guards before the loop.
            if (readIndex >= bufferLength || readIndex2 >= bufferLength)
                break;

            // Linear interpolation.
            for (unsigned i = 0; i < numberOfChannels; ++i) {
                float* destination = destinationChannels[i];
                const float* source = sourceChannels[i];

                double sample1 = source[readIndex];
                double sample2 = source[readIndex2];
                double sample = (1.0 - interpolationFactor) * sample1 + interpolationFactor * sample2;

                destination[writeIndex] = narrowPrecisionToFloat(sample);
            }
            writeIndex++;

            virtualReadIndex += computedPlaybackRate;

            // Wrap-around, retaining sub-sample position since virtualReadIndex is floating-point.
            if (virtualReadIndex >= virtualEndFrame) {
                virtualReadIndex -= virtualDeltaFrames;
                if (renderSilenceAndFinishIfNotLooping(bus, writeIndex, framesToProcess))
                    break;
            }
        }
    }

    bus->clearSilentFlag();

    m_virtualReadIndex = virtualReadIndex;

    return true;
}


void AudioBufferSourceHandler::setBuffer(AudioBuffer* buffer, ExceptionState& exceptionState)
{
    ASSERT(isMainThread());

    if (m_buffer) {
        exceptionState.throwDOMException(
            InvalidStateError,
            "Cannot set buffer after it has been already been set");
        return;
    }

    // The context must be locked since changing the buffer can re-configure the number of channels that are output.
    AudioContext::AutoLocker contextLocker(context());

    // This synchronizes with process().
    MutexLocker processLocker(m_processLock);

    if (buffer) {
        // Do any necesssary re-configuration to the buffer's number of channels.
        unsigned numberOfChannels = buffer->numberOfChannels();

        // This should not be possible since AudioBuffers can't be created with too many channels
        // either.
        if (numberOfChannels > AudioContext::maxNumberOfChannels()) {
            exceptionState.throwDOMException(
                NotSupportedError,
                ExceptionMessages::indexOutsideRange(
                    "number of input channels",
                    numberOfChannels,
                    1u,
                    ExceptionMessages::InclusiveBound,
                    AudioContext::maxNumberOfChannels(),
                    ExceptionMessages::InclusiveBound));
            return;
        }

        output(0).setNumberOfChannels(numberOfChannels);

        m_sourceChannels = adoptArrayPtr(new const float* [numberOfChannels]);
        m_destinationChannels = adoptArrayPtr(new float* [numberOfChannels]);

        for (unsigned i = 0; i < numberOfChannels; ++i)
            m_sourceChannels[i] = buffer->getChannelData(i)->data();

        // If this is a grain (as set by a previous call to start()), validate the grain parameters
        // now since it wasn't validated when start was called (because there was no buffer then).
        if (m_isGrain)
            clampGrainParameters(buffer);
    }

    m_virtualReadIndex = 0;
    m_buffer = buffer;
}

unsigned AudioBufferSourceHandler::numberOfChannels()
{
    return output(0).numberOfChannels();
}

void AudioBufferSourceHandler::clampGrainParameters(const AudioBuffer* buffer)
{
    ASSERT(buffer);

    // We have a buffer so we can clip the offset and duration to lie within the buffer.
    double bufferDuration = buffer->duration();

    m_grainOffset = clampTo(m_grainOffset, 0.0, bufferDuration);

    // If the duration was not explicitly given, use the buffer duration to set the grain
    // duration. Otherwise, we want to use the user-specified value, of course.
    if (!m_isDurationGiven)
        m_grainDuration = bufferDuration - m_grainOffset;

    if (m_isDurationGiven && loop()) {
        // We're looping a grain with a grain duration specified. Schedule the loop to stop after
        // grainDuration seconds after starting, possibly running the loop multiple times if
        // grainDuration is larger than the buffer duration. The net effect is as if the user called
        // stop(when + grainDuration).
        m_grainDuration = clampTo(m_grainDuration, 0.0, std::numeric_limits<double>::infinity());
        m_endTime = m_startTime + m_grainDuration;
    } else {
        m_grainDuration = clampTo(m_grainDuration, 0.0, bufferDuration - m_grainOffset);
    }

    // We call timeToSampleFrame here since at playbackRate == 1 we don't want to go through
    // linear interpolation at a sub-sample position since it will degrade the quality. When
    // aligned to the sample-frame the playback will be identical to the PCM data stored in the
    // buffer. Since playbackRate == 1 is very common, it's worth considering quality.
    m_virtualReadIndex = AudioUtilities::timeToSampleFrame(m_grainOffset, buffer->sampleRate());
}

void AudioBufferSourceHandler::start(double when, ExceptionState& exceptionState)
{
    AudioScheduledSourceHandler::start(when, exceptionState);
}

void AudioBufferSourceHandler::start(double when, double grainOffset, ExceptionState& exceptionState)
{
    startSource(when, grainOffset, buffer() ? buffer()->duration() : 0, false, exceptionState);
}

void AudioBufferSourceHandler::start(double when, double grainOffset, double grainDuration, ExceptionState& exceptionState)
{
    startSource(when, grainOffset, grainDuration, true, exceptionState);
}

void AudioBufferSourceHandler::startSource(double when, double grainOffset, double grainDuration, bool isDurationGiven, ExceptionState& exceptionState)
{
    ASSERT(isMainThread());

    if (m_playbackState != UNSCHEDULED_STATE) {
        exceptionState.throwDOMException(
            InvalidStateError,
            "cannot call start more than once.");
        return;
    }

    if (when < 0) {
        exceptionState.throwDOMException(
            InvalidStateError,
            ExceptionMessages::indexExceedsMinimumBound(
                "start time",
                when,
                0.0));
        return;
    }

    if (grainOffset < 0) {
        exceptionState.throwDOMException(
            InvalidStateError,
            ExceptionMessages::indexExceedsMinimumBound(
                "offset",
                grainOffset,
                0.0));
        return;
    }

    if (grainDuration < 0) {
        exceptionState.throwDOMException(
            InvalidStateError,
            ExceptionMessages::indexExceedsMinimumBound(
                "duration",
                grainDuration,
                0.0));
        return;
    }

    m_isDurationGiven = isDurationGiven;
    m_isGrain = true;
    m_grainOffset = grainOffset;
    m_grainDuration = grainDuration;

    // The node is started. Add a reference to keep us alive so that audio
    // will eventually get played even if Javascript should drop all references
    // to this node. The reference will get dropped when the source has finished
    // playing.
    context()->notifySourceNodeStartedProcessing(node());

    // If |when| < currentTime, the source must start now according to the spec.
    // So just set startTime to currentTime in this case to start the source now.
    m_startTime = std::max(when, context()->currentTime());

    if (buffer())
        clampGrainParameters(buffer());

    m_playbackState = SCHEDULED_STATE;
}

double AudioBufferSourceHandler::computePlaybackRate()
{
    double dopplerRate = 1;
    if (m_pannerNode)
        dopplerRate = m_pannerNode->dopplerRate();

    // Incorporate buffer's sample-rate versus AudioContext's sample-rate.
    // Normally it's not an issue because buffers are loaded at the
    // AudioContext's sample-rate, but we can handle it in any case.
    double sampleRateFactor = 1.0;
    if (buffer()) {
        // Use doubles to compute this to full accuracy.
        sampleRateFactor = buffer()->sampleRate() / static_cast<double>(sampleRate());
    }

    // Use finalValue() to incorporate changes of AudioParamTimeline and
    // AudioSummingJunction from m_playbackRate AudioParam.
    double basePlaybackRate = m_playbackRate->finalValue();

    double finalPlaybackRate = dopplerRate * sampleRateFactor * basePlaybackRate;

    // Take the detune value into account for the final playback rate.
    finalPlaybackRate *= pow(2, m_detune->finalValue() / 1200);

    // Sanity check the total rate.  It's very important that the resampler not
    // get any bad rate values.
    finalPlaybackRate = clampTo(finalPlaybackRate, 0.0, MaxRate);

    bool isPlaybackRateValid = !std::isnan(finalPlaybackRate) && !std::isinf(finalPlaybackRate);
    ASSERT(isPlaybackRateValid);

    if (!isPlaybackRateValid)
        finalPlaybackRate = 1.0;

    // Record the minimum playback rate for use by handleStoppableSourceNode.
    m_minPlaybackRate = std::min(finalPlaybackRate, m_minPlaybackRate);

    return finalPlaybackRate;
}

bool AudioBufferSourceHandler::propagatesSilence() const
{
    return !isPlayingOrScheduled() || hasFinished() || !m_buffer;
}

void AudioBufferSourceHandler::setPannerNode(PannerHandler* pannerNode)
{
    if (m_pannerNode != pannerNode && !hasFinished()) {
        RefPtr<PannerHandler> oldPannerNode(m_pannerNode.release());
        m_pannerNode = pannerNode;
        if (pannerNode)
            pannerNode->makeConnection();
        if (oldPannerNode)
            oldPannerNode->breakConnection();
    }
}

void AudioBufferSourceHandler::clearPannerNode()
{
    if (m_pannerNode) {
        m_pannerNode->breakConnection();
        m_pannerNode.clear();
    }
}

void AudioBufferSourceHandler::handleStoppableSourceNode()
{
    // If the source node is not looping, and we have a buffer, we can determine when the source
    // would stop playing.  This is intended to handle the (uncommon) scenario where start() has
    // been called but is never connected to the destination (directly or indirectly).  By stopping
    // the node, the node can be collected.  Otherwise, the node will never get collected, leaking
    // memory.
    if (!loop() && buffer() && isPlayingOrScheduled() && m_minPlaybackRate > 0) {
        // Adjust the duration to include the playback rate. Only need to account for rate < 1
        // which makes the sound last longer.  For rate >= 1, the source stops sooner, but that's
        // ok.
        double actualDuration = buffer()->duration() / m_minPlaybackRate;

        double stopTime = m_startTime + actualDuration;

        // See crbug.com/478301. If a source node is started via start(), the source may not start
        // at that time but one quantum (128 frames) later.  But we compute the stop time based on
        // the start time and the duration, so we end up stopping one quantum early.  Thus, add a
        // little extra time; we just need to stop the source sometime after it should have stopped
        // if it hadn't already.  We don't need to be super precise on when to stop.
        double extraStopTime = kExtraStopFrames / static_cast<double>(context()->sampleRate());

        stopTime += extraStopTime;
        if (context()->currentTime() > stopTime) {
            // The context time has passed the time when the source nodes should have stopped
            // playing. Stop the node now and deref it. (But don't run the onEnded event because the
            // source never actually played.)
            finishWithoutOnEnded();
        }
    }
}

void AudioBufferSourceHandler::finish()
{
    clearPannerNode();
    ASSERT(!m_pannerNode);
    AudioScheduledSourceHandler::finish();
}

// ----------------------------------------------------------------
AudioBufferSourceNode::AudioBufferSourceNode(AudioContext& context, float sampleRate)
    : AudioScheduledSourceNode(context)
    , m_playbackRate(AudioParam::create(context, 1.0))
    , m_detune(AudioParam::create(context, 0.0))
{
    setHandler(AudioBufferSourceHandler::create(*this, sampleRate, m_playbackRate->handler(), m_detune->handler()));
}

AudioBufferSourceNode* AudioBufferSourceNode::create(AudioContext& context, float sampleRate)
{
    return new AudioBufferSourceNode(context, sampleRate);
}

DEFINE_TRACE(AudioBufferSourceNode)
{
    visitor->trace(m_playbackRate);
    visitor->trace(m_detune);
    AudioScheduledSourceNode::trace(visitor);
}

AudioBufferSourceHandler& AudioBufferSourceNode::audioBufferSourceHandler() const
{
    return static_cast<AudioBufferSourceHandler&>(handler());
}

AudioBuffer* AudioBufferSourceNode::buffer() const
{
    return audioBufferSourceHandler().buffer();
}

void AudioBufferSourceNode::setBuffer(AudioBuffer* newBuffer, ExceptionState& exceptionState)
{
    audioBufferSourceHandler().setBuffer(newBuffer, exceptionState);
}

AudioParam* AudioBufferSourceNode::playbackRate() const
{
    return m_playbackRate;
}

AudioParam* AudioBufferSourceNode::detune() const
{
    return m_detune;
}

bool AudioBufferSourceNode::loop() const
{
    return audioBufferSourceHandler().loop();
}

void AudioBufferSourceNode::setLoop(bool loop)
{
    audioBufferSourceHandler().setLoop(loop);
}

double AudioBufferSourceNode::loopStart() const
{
    return audioBufferSourceHandler().loopStart();
}

void AudioBufferSourceNode::setLoopStart(double loopStart)
{
    audioBufferSourceHandler().setLoopStart(loopStart);
}

double AudioBufferSourceNode::loopEnd() const
{
    return audioBufferSourceHandler().loopEnd();
}

void AudioBufferSourceNode::setLoopEnd(double loopEnd)
{
    audioBufferSourceHandler().setLoopEnd(loopEnd);
}

void AudioBufferSourceNode::start(ExceptionState& exceptionState)
{
    audioBufferSourceHandler().start(0, exceptionState);
}

void AudioBufferSourceNode::start(double when, ExceptionState& exceptionState)
{
    audioBufferSourceHandler().start(when, exceptionState);
}

void AudioBufferSourceNode::start(double when, double grainOffset, ExceptionState& exceptionState)
{
    audioBufferSourceHandler().start(when, grainOffset, exceptionState);
}

void AudioBufferSourceNode::start(double when, double grainOffset, double grainDuration, ExceptionState& exceptionState)
{
    audioBufferSourceHandler().start(when, grainOffset, grainDuration, exceptionState);
}

} // namespace blink

#endif // ENABLE(WEB_AUDIO)
