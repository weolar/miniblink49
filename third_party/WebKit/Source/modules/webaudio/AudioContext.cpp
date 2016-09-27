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
#include "modules/webaudio/AudioContext.h"

#include "bindings/core/v8/ExceptionMessages.h"
#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/ScriptPromiseResolver.h"
#include "bindings/core/v8/ScriptState.h"
#include "core/dom/DOMException.h"
#include "core/dom/Document.h"
#include "core/dom/ExceptionCode.h"
#include "core/dom/ExecutionContextTask.h"
#include "core/html/HTMLMediaElement.h"
#include "modules/mediastream/MediaStream.h"
#include "modules/webaudio/AnalyserNode.h"
#include "modules/webaudio/AudioBuffer.h"
#include "modules/webaudio/AudioBufferCallback.h"
#include "modules/webaudio/AudioBufferSourceNode.h"
#include "modules/webaudio/AudioListener.h"
#include "modules/webaudio/AudioNodeInput.h"
#include "modules/webaudio/AudioNodeOutput.h"
#include "modules/webaudio/BiquadFilterNode.h"
#include "modules/webaudio/ChannelMergerNode.h"
#include "modules/webaudio/ChannelSplitterNode.h"
#include "modules/webaudio/ConvolverNode.h"
#include "modules/webaudio/DefaultAudioDestinationNode.h"
#include "modules/webaudio/DelayNode.h"
#include "modules/webaudio/DynamicsCompressorNode.h"
#include "modules/webaudio/GainNode.h"
#include "modules/webaudio/MediaElementAudioSourceNode.h"
#include "modules/webaudio/MediaStreamAudioDestinationNode.h"
#include "modules/webaudio/MediaStreamAudioSourceNode.h"
#include "modules/webaudio/OfflineAudioCompletionEvent.h"
#include "modules/webaudio/OfflineAudioContext.h"
#include "modules/webaudio/OfflineAudioDestinationNode.h"
#include "modules/webaudio/OscillatorNode.h"
#include "modules/webaudio/PannerNode.h"
#include "modules/webaudio/PeriodicWave.h"
#include "modules/webaudio/ScriptProcessorNode.h"
#include "modules/webaudio/StereoPannerNode.h"
#include "modules/webaudio/WaveShaperNode.h"
#include "platform/ThreadSafeFunctional.h"
#include "public/platform/Platform.h"
#include "wtf/text/WTFString.h"

#if DEBUG_AUDIONODE_REFERENCES
#include <stdio.h>
#endif

namespace blink {

// Don't allow more than this number of simultaneous AudioContexts talking to hardware.
const unsigned MaxHardwareContexts = 6;
unsigned AudioContext::s_hardwareContextCount = 0;
unsigned AudioContext::s_contextId = 0;

AudioContext* AudioContext::create(Document& document, ExceptionState& exceptionState)
{
    ASSERT(isMainThread());
    if (s_hardwareContextCount >= MaxHardwareContexts) {
        exceptionState.throwDOMException(
            NotSupportedError,
            ExceptionMessages::indexExceedsMaximumBound(
                "number of hardware contexts",
                s_hardwareContextCount,
                MaxHardwareContexts));
        return nullptr;
    }

    AudioContext* audioContext = new AudioContext(&document);
    audioContext->suspendIfNeeded();
    return audioContext;
}

// Constructor for rendering to the audio hardware.
AudioContext::AudioContext(Document* document)
    : ActiveDOMObject(document)
    , m_isStopScheduled(false)
    , m_isCleared(false)
    , m_isInitialized(false)
    , m_destinationNode(nullptr)
    , m_isResolvingResumePromises(false)
    , m_connectionCount(0)
    , m_didInitializeContextGraphMutex(false)
    , m_deferredTaskHandler(DeferredTaskHandler::create())
    , m_isOfflineContext(false)
    , m_contextState(Suspended)
{
    m_didInitializeContextGraphMutex = true;
    m_destinationNode = DefaultAudioDestinationNode::create(this);

    initialize();
}

// Constructor for offline (non-realtime) rendering.
AudioContext::AudioContext(Document* document, unsigned numberOfChannels, size_t numberOfFrames, float sampleRate)
    : ActiveDOMObject(document)
    , m_isStopScheduled(false)
    , m_isCleared(false)
    , m_isInitialized(false)
    , m_destinationNode(nullptr)
    , m_isResolvingResumePromises(false)
    , m_connectionCount(0)
    , m_didInitializeContextGraphMutex(false)
    , m_deferredTaskHandler(DeferredTaskHandler::create())
    , m_isOfflineContext(true)
    , m_contextState(Suspended)
{
    m_didInitializeContextGraphMutex = true;
    // Create a new destination for offline rendering.
    m_renderTarget = AudioBuffer::create(numberOfChannels, numberOfFrames, sampleRate);
    if (m_renderTarget.get())
        m_destinationNode = OfflineAudioDestinationNode::create(this, m_renderTarget.get());

    initialize();
}

AudioContext::~AudioContext()
{
#if DEBUG_AUDIONODE_REFERENCES
    fprintf(stderr, "%p: AudioContext::~AudioContext(): %u\n", this, m_contextId);
#endif
    deferredTaskHandler().contextWillBeDestroyed();
    // AudioNodes keep a reference to their context, so there should be no way to be in the destructor if there are still AudioNodes around.
    ASSERT(!m_isInitialized);
    ASSERT(!m_activeSourceNodes.size());
    ASSERT(!m_finishedSourceHandlers.size());
    ASSERT(!m_isResolvingResumePromises);
    ASSERT(!m_resumeResolvers.size());
}

void AudioContext::initialize()
{
    if (isInitialized())
        return;

    FFTFrame::initialize();
    m_listener = AudioListener::create();

    if (m_destinationNode.get()) {
        m_destinationNode->handler().initialize();

        if (!isOfflineContext()) {
            // This starts the audio thread. The destination node's provideInput() method will now be called repeatedly to render audio.
            // Each time provideInput() is called, a portion of the audio stream is rendered. Let's call this time period a "render quantum".
            // NOTE: for now default AudioContext does not need an explicit startRendering() call from JavaScript.
            // We may want to consider requiring it for symmetry with OfflineAudioContext.
            startRendering();
            ++s_hardwareContextCount;
        }

        m_contextId = s_contextId++;
        m_isInitialized = true;
#if DEBUG_AUDIONODE_REFERENCES
        fprintf(stderr, "%p: AudioContext::AudioContext(): %u #%u\n",
            this, m_contextId, AudioContext::s_hardwareContextCount);
#endif
    }
}

void AudioContext::clear()
{
    m_destinationNode.clear();
    // The audio rendering thread is dead.  Nobody will schedule AudioHandler
    // deletion.  Let's do it ourselves.
    deferredTaskHandler().clearHandlersToBeDeleted();
    m_isCleared = true;
}

void AudioContext::uninitialize()
{
    ASSERT(isMainThread());

    if (!isInitialized())
        return;

    m_isInitialized = false;

    // This stops the audio thread and all audio rendering.
    if (m_destinationNode)
        m_destinationNode->handler().uninitialize();

    if (!isOfflineContext()) {
        ASSERT(s_hardwareContextCount);
        --s_hardwareContextCount;
    }

    // Get rid of the sources which may still be playing.
    releaseActiveSourceNodes();

    // Reject any pending resolvers before we go away.
    rejectPendingResolvers();

    // For an offline audio context, the completion event will set the state to closed.  For an
    // online context, we need to do it here.  We only want to set the closed state once.
    if (!isOfflineContext())
        setContextState(Closed);

    // Resolve the promise now, if any
    if (m_closeResolver)
        m_closeResolver->resolve();

    ASSERT(m_listener);
    m_listener->waitForHRTFDatabaseLoaderThreadCompletion();

    clear();
}

void AudioContext::stop()
{
    // Usually ExecutionContext calls stop twice.
    if (m_isStopScheduled)
        return;
    m_isStopScheduled = true;

    // Don't call uninitialize() immediately here because the ExecutionContext is in the middle
    // of dealing with all of its ActiveDOMObjects at this point. uninitialize() can de-reference other
    // ActiveDOMObjects so let's schedule uninitialize() to be called later.
    // FIXME: see if there's a more direct way to handle this issue.
    Platform::current()->mainThread()->postTask(FROM_HERE, bind(&AudioContext::uninitialize, this));
}

bool AudioContext::hasPendingActivity() const
{
    // There's no pending activity if the audio context has been cleared.
    return !m_isCleared;
}

void AudioContext::throwExceptionForClosedState(ExceptionState& exceptionState)
{
    exceptionState.throwDOMException(InvalidStateError, "AudioContext has been closed.");
}

AudioBuffer* AudioContext::createBuffer(unsigned numberOfChannels, size_t numberOfFrames, float sampleRate, ExceptionState& exceptionState)
{
    // It's ok to call createBuffer, even if the context is closed because the AudioBuffer doesn't
    // really "belong" to any particular context.

    return AudioBuffer::create(numberOfChannels, numberOfFrames, sampleRate, exceptionState);
}

void AudioContext::decodeAudioData(DOMArrayBuffer* audioData, AudioBufferCallback* successCallback, AudioBufferCallback* errorCallback, ExceptionState& exceptionState)
{
    if (isContextClosed()) {
        throwExceptionForClosedState(exceptionState);
        return;
    }

    if (!audioData) {
        exceptionState.throwDOMException(
            SyntaxError,
            "invalid ArrayBuffer for audioData.");
        return;
    }
    m_audioDecoder.decodeAsync(audioData, sampleRate(), successCallback, errorCallback);
}

AudioBufferSourceNode* AudioContext::createBufferSource(ExceptionState& exceptionState)
{
    ASSERT(isMainThread());

    if (isContextClosed()) {
        throwExceptionForClosedState(exceptionState);
        return nullptr;
    }

    AudioBufferSourceNode* node = AudioBufferSourceNode::create(*this, sampleRate());

    // Do not add a reference to this source node now. The reference will be added when start() is
    // called.

    return node;
}

MediaElementAudioSourceNode* AudioContext::createMediaElementSource(HTMLMediaElement* mediaElement, ExceptionState& exceptionState)
{
    ASSERT(isMainThread());

    if (isContextClosed()) {
        throwExceptionForClosedState(exceptionState);
        return nullptr;
    }

    if (!mediaElement) {
        exceptionState.throwDOMException(
            InvalidStateError,
            "invalid HTMLMedialElement.");
        return nullptr;
    }

    // First check if this media element already has a source node.
    if (mediaElement->audioSourceNode()) {
        exceptionState.throwDOMException(
            InvalidStateError,
            "HTMLMediaElement already connected previously to a different MediaElementSourceNode.");
        return nullptr;
    }

    MediaElementAudioSourceNode* node = MediaElementAudioSourceNode::create(*this, *mediaElement);

    mediaElement->setAudioSourceNode(node);

    notifySourceNodeStartedProcessing(node); // context keeps reference until node is disconnected
    return node;
}

MediaStreamAudioSourceNode* AudioContext::createMediaStreamSource(MediaStream* mediaStream, ExceptionState& exceptionState)
{
    ASSERT(isMainThread());

    if (isContextClosed()) {
        throwExceptionForClosedState(exceptionState);
        return nullptr;
    }

    if (!mediaStream) {
        exceptionState.throwDOMException(
            InvalidStateError,
            "invalid MediaStream source");
        return nullptr;
    }

    MediaStreamTrackVector audioTracks = mediaStream->getAudioTracks();
    if (audioTracks.isEmpty()) {
        exceptionState.throwDOMException(
            InvalidStateError,
            "MediaStream has no audio track");
        return nullptr;
    }

    // Use the first audio track in the media stream.
    MediaStreamTrack* audioTrack = audioTracks[0];
    OwnPtr<AudioSourceProvider> provider = audioTrack->createWebAudioSource();
    MediaStreamAudioSourceNode* node = MediaStreamAudioSourceNode::create(*this, *mediaStream, audioTrack, provider.release());

    // FIXME: Only stereo streams are supported right now. We should be able to accept multi-channel streams.
    node->setFormat(2, sampleRate());

    notifySourceNodeStartedProcessing(node); // context keeps reference until node is disconnected
    return node;
}

MediaStreamAudioDestinationNode* AudioContext::createMediaStreamDestination(ExceptionState& exceptionState)
{
    if (isContextClosed()) {
        throwExceptionForClosedState(exceptionState);
        return nullptr;
    }

    // Set number of output channels to stereo by default.
    return MediaStreamAudioDestinationNode::create(*this, 2);
}

ScriptProcessorNode* AudioContext::createScriptProcessor(ExceptionState& exceptionState)
{
    // Set number of input/output channels to stereo by default.
    return createScriptProcessor(0, 2, 2, exceptionState);
}

ScriptProcessorNode* AudioContext::createScriptProcessor(size_t bufferSize, ExceptionState& exceptionState)
{
    // Set number of input/output channels to stereo by default.
    return createScriptProcessor(bufferSize, 2, 2, exceptionState);
}

ScriptProcessorNode* AudioContext::createScriptProcessor(size_t bufferSize, size_t numberOfInputChannels, ExceptionState& exceptionState)
{
    // Set number of output channels to stereo by default.
    return createScriptProcessor(bufferSize, numberOfInputChannels, 2, exceptionState);
}

ScriptProcessorNode* AudioContext::createScriptProcessor(size_t bufferSize, size_t numberOfInputChannels, size_t numberOfOutputChannels, ExceptionState& exceptionState)
{
    ASSERT(isMainThread());

    if (isContextClosed()) {
        throwExceptionForClosedState(exceptionState);
        return nullptr;
    }

    ScriptProcessorNode* node = ScriptProcessorNode::create(*this, sampleRate(), bufferSize, numberOfInputChannels, numberOfOutputChannels);

    if (!node) {
        if (!numberOfInputChannels && !numberOfOutputChannels) {
            exceptionState.throwDOMException(
                IndexSizeError,
                "number of input channels and output channels cannot both be zero.");
        } else if (numberOfInputChannels > AudioContext::maxNumberOfChannels()) {
            exceptionState.throwDOMException(
                IndexSizeError,
                "number of input channels (" + String::number(numberOfInputChannels)
                + ") exceeds maximum ("
                + String::number(AudioContext::maxNumberOfChannels()) + ").");
        } else if (numberOfOutputChannels > AudioContext::maxNumberOfChannels()) {
            exceptionState.throwDOMException(
                IndexSizeError,
                "number of output channels (" + String::number(numberOfInputChannels)
                + ") exceeds maximum ("
                + String::number(AudioContext::maxNumberOfChannels()) + ").");
        } else {
            exceptionState.throwDOMException(
                IndexSizeError,
                "buffer size (" + String::number(bufferSize)
                + ") must be a power of two between 256 and 16384.");
        }
        return nullptr;
    }

    notifySourceNodeStartedProcessing(node); // context keeps reference until we stop making javascript rendering callbacks
    return node;
}

StereoPannerNode* AudioContext::createStereoPanner(ExceptionState& exceptionState)
{
    ASSERT(isMainThread());
    if (isContextClosed()) {
        throwExceptionForClosedState(exceptionState);
        return nullptr;
    }

    return StereoPannerNode::create(*this, sampleRate());
}

BiquadFilterNode* AudioContext::createBiquadFilter(ExceptionState& exceptionState)
{
    ASSERT(isMainThread());
    if (isContextClosed()) {
        throwExceptionForClosedState(exceptionState);
        return nullptr;
    }

    return BiquadFilterNode::create(*this, sampleRate());
}

WaveShaperNode* AudioContext::createWaveShaper(ExceptionState& exceptionState)
{
    ASSERT(isMainThread());
    if (isContextClosed()) {
        throwExceptionForClosedState(exceptionState);
        return nullptr;
    }

    return WaveShaperNode::create(*this);
}

PannerNode* AudioContext::createPanner(ExceptionState& exceptionState)
{
    ASSERT(isMainThread());
    if (isContextClosed()) {
        throwExceptionForClosedState(exceptionState);
        return nullptr;
    }

    return PannerNode::create(*this, sampleRate());
}

ConvolverNode* AudioContext::createConvolver(ExceptionState& exceptionState)
{
    ASSERT(isMainThread());
    if (isContextClosed()) {
        throwExceptionForClosedState(exceptionState);
        return nullptr;
    }

    return ConvolverNode::create(*this, sampleRate());
}

DynamicsCompressorNode* AudioContext::createDynamicsCompressor(ExceptionState& exceptionState)
{
    ASSERT(isMainThread());
    if (isContextClosed()) {
        throwExceptionForClosedState(exceptionState);
        return nullptr;
    }

    return DynamicsCompressorNode::create(*this, sampleRate());
}

AnalyserNode* AudioContext::createAnalyser(ExceptionState& exceptionState)
{
    ASSERT(isMainThread());
    if (isContextClosed()) {
        throwExceptionForClosedState(exceptionState);
        return nullptr;
    }

    return AnalyserNode::create(*this, sampleRate());
}

GainNode* AudioContext::createGain(ExceptionState& exceptionState)
{
    ASSERT(isMainThread());
    if (isContextClosed()) {
        throwExceptionForClosedState(exceptionState);
        return nullptr;
    }

    return GainNode::create(*this, sampleRate());
}

DelayNode* AudioContext::createDelay(ExceptionState& exceptionState)
{
    const double defaultMaxDelayTime = 1;
    return createDelay(defaultMaxDelayTime, exceptionState);
}

DelayNode* AudioContext::createDelay(double maxDelayTime, ExceptionState& exceptionState)
{
    ASSERT(isMainThread());
    if (isContextClosed()) {
        throwExceptionForClosedState(exceptionState);
        return nullptr;
    }

    return DelayNode::create(*this, sampleRate(), maxDelayTime, exceptionState);
}

ChannelSplitterNode* AudioContext::createChannelSplitter(ExceptionState& exceptionState)
{
    const unsigned ChannelSplitterDefaultNumberOfOutputs = 6;
    return createChannelSplitter(ChannelSplitterDefaultNumberOfOutputs, exceptionState);
}

ChannelSplitterNode* AudioContext::createChannelSplitter(size_t numberOfOutputs, ExceptionState& exceptionState)
{
    ASSERT(isMainThread());

    if (isContextClosed()) {
        throwExceptionForClosedState(exceptionState);
        return nullptr;
    }

    ChannelSplitterNode* node = ChannelSplitterNode::create(*this, sampleRate(), numberOfOutputs);

    if (!node) {
        exceptionState.throwDOMException(
            IndexSizeError,
            "number of outputs (" + String::number(numberOfOutputs)
            + ") must be between 1 and "
            + String::number(AudioContext::maxNumberOfChannels()) + ".");
        return nullptr;
    }

    return node;
}

ChannelMergerNode* AudioContext::createChannelMerger(ExceptionState& exceptionState)
{
    const unsigned ChannelMergerDefaultNumberOfInputs = 6;
    return createChannelMerger(ChannelMergerDefaultNumberOfInputs, exceptionState);
}

ChannelMergerNode* AudioContext::createChannelMerger(size_t numberOfInputs, ExceptionState& exceptionState)
{
    ASSERT(isMainThread());
    if (isContextClosed()) {
        throwExceptionForClosedState(exceptionState);
        return nullptr;
    }

    ChannelMergerNode* node = ChannelMergerNode::create(*this, sampleRate(), numberOfInputs);

    if (!node) {
        exceptionState.throwDOMException(
            IndexSizeError,
            ExceptionMessages::indexOutsideRange<size_t>(
                "number of inputs",
                numberOfInputs,
                1,
                ExceptionMessages::InclusiveBound,
                AudioContext::maxNumberOfChannels(),
                ExceptionMessages::InclusiveBound));
        return nullptr;
    }

    return node;
}

OscillatorNode* AudioContext::createOscillator(ExceptionState& exceptionState)
{
    ASSERT(isMainThread());
    if (isContextClosed()) {
        throwExceptionForClosedState(exceptionState);
        return nullptr;
    }

    OscillatorNode* node = OscillatorNode::create(*this, sampleRate());

    // Do not add a reference to this source node now. The reference will be added when start() is
    // called.

    return node;
}

PeriodicWave* AudioContext::createPeriodicWave(DOMFloat32Array* real, DOMFloat32Array* imag, ExceptionState& exceptionState)
{
    ASSERT(isMainThread());

    if (isContextClosed()) {
        throwExceptionForClosedState(exceptionState);
        return nullptr;
    }

    if (!real) {
        exceptionState.throwDOMException(
            SyntaxError,
            "invalid real array");
        return nullptr;
    }

    if (!imag) {
        exceptionState.throwDOMException(
            SyntaxError,
            "invalid imaginary array");
        return nullptr;
    }

    if (real->length() > PeriodicWave::kMaxPeriodicWaveArraySize) {
        exceptionState.throwDOMException(
            IndexSizeError,
            ExceptionMessages::indexOutsideRange(
                "length of the real part array",
                real->length(),
                1u,
                ExceptionMessages::InclusiveBound,
                PeriodicWave::kMaxPeriodicWaveArraySize,
                ExceptionMessages::InclusiveBound));
        return nullptr;
    }

    if (imag->length() > PeriodicWave::kMaxPeriodicWaveArraySize) {
        exceptionState.throwDOMException(
            IndexSizeError,
            ExceptionMessages::indexOutsideRange(
                "length of the imaginary part array",
                imag->length(),
                1u,
                ExceptionMessages::InclusiveBound,
                PeriodicWave::kMaxPeriodicWaveArraySize,
                ExceptionMessages::InclusiveBound));
        return nullptr;
    }

    if (real->length() != imag->length()) {
        exceptionState.throwDOMException(
            IndexSizeError,
            "length of real array (" + String::number(real->length())
            + ") and length of imaginary array (" +  String::number(imag->length())
            + ") must match.");
        return nullptr;
    }

    return PeriodicWave::create(sampleRate(), real, imag);
}

String AudioContext::state() const
{
    // These strings had better match the strings for AudioContextState in AudioContext.idl.
    switch (m_contextState) {
    case Suspended:
        return "suspended";
    case Running:
        return "running";
    case Closed:
        return "closed";
    }
    ASSERT_NOT_REACHED();
    return "";
}

void AudioContext::setContextState(AudioContextState newState)
{
    ASSERT(isMainThread());

    // Validate the transitions.  The valid transitions are Suspended->Running, Running->Suspended,
    // and anything->Closed.
    switch (newState) {
    case Suspended:
        ASSERT(m_contextState == Running);
        break;
    case Running:
        ASSERT(m_contextState == Suspended);
        break;
    case Closed:
        ASSERT(m_contextState != Closed);
        break;
    }

    if (newState == m_contextState) {
        // ASSERTs above failed; just return.
        return;
    }

    m_contextState = newState;

    // Notify context that state changed
    if (executionContext())
        executionContext()->postTask(FROM_HERE, createSameThreadTask(&AudioContext::notifyStateChange, this));
}

void AudioContext::notifyStateChange()
{
    dispatchEvent(Event::create(EventTypeNames::statechange));
}

ScriptPromise AudioContext::suspendContext(ScriptState* scriptState)
{
    ASSERT(isMainThread());
    AutoLocker locker(this);

    if (isOfflineContext()) {
        return ScriptPromise::rejectWithDOMException(
            scriptState,
            DOMException::create(
                InvalidAccessError,
                "cannot suspend an OfflineAudioContext"));
    }

    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);
    ScriptPromise promise = resolver->promise();

    if (m_contextState == Closed) {
        resolver->reject(
            DOMException::create(InvalidStateError, "Cannot suspend a context that has been closed"));
    } else {
        // Stop rendering now.
        if (m_destinationNode)
            stopRendering();

        // Since we don't have any way of knowing when the hardware actually stops, we'll just
        // resolve the promise now.
        resolver->resolve();
    }

    return promise;
}

ScriptPromise AudioContext::resumeContext(ScriptState* scriptState)
{
    ASSERT(isMainThread());
    AutoLocker locker(this);

    if (isOfflineContext()) {
        return ScriptPromise::rejectWithDOMException(
            scriptState,
            DOMException::create(
                InvalidAccessError,
                "cannot resume an OfflineAudioContext"));
    }

    if (isContextClosed()) {
        return ScriptPromise::rejectWithDOMException(
            scriptState,
            DOMException::create(
                InvalidAccessError,
                "cannot resume a closed AudioContext"));
    }

    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);
    ScriptPromise promise = resolver->promise();

    // Restart the destination node to pull on the audio graph.
    if (m_destinationNode)
        startRendering();

    // Save the resolver which will get resolved when the destination node starts pulling on the
    // graph again.
    m_resumeResolvers.append(resolver);

    return promise;
}

void AudioContext::notifySourceNodeFinishedProcessing(AudioHandler* handler)
{
    ASSERT(isAudioThread());
    m_finishedSourceHandlers.append(handler);
}

void AudioContext::releaseFinishedSourceNodes()
{
    ASSERT(isGraphOwner());
    ASSERT(isAudioThread());
    for (AudioHandler* handler : m_finishedSourceHandlers) {
        for (unsigned i = 0; i < m_activeSourceNodes.size(); ++i) {
            if (handler == &m_activeSourceNodes[i]->handler()) {
                handler->breakConnection();
                m_activeSourceNodes.remove(i);
                break;
            }
        }
    }

    m_finishedSourceHandlers.clear();
}

void AudioContext::notifySourceNodeStartedProcessing(AudioNode* node)
{
    ASSERT(isMainThread());
    AutoLocker locker(this);

    m_activeSourceNodes.append(node);
    node->handler().makeConnection();
}

void AudioContext::releaseActiveSourceNodes()
{
    ASSERT(isMainThread());
    for (auto& sourceNode : m_activeSourceNodes)
        sourceNode->handler().breakConnection();

    m_activeSourceNodes.clear();
}

void AudioContext::handleStoppableSourceNodes()
{
    ASSERT(isGraphOwner());

    // Find AudioBufferSourceNodes to see if we can stop playing them.
    for (AudioNode* node : m_activeSourceNodes) {
        if (node->handler().nodeType() == AudioHandler::NodeTypeAudioBufferSource) {
            AudioBufferSourceNode* sourceNode = static_cast<AudioBufferSourceNode*>(node);
            sourceNode->audioBufferSourceHandler().handleStoppableSourceNode();
        }
    }
}

void AudioContext::handlePreRenderTasks()
{
    ASSERT(isAudioThread());

    // At the beginning of every render quantum, try to update the internal rendering graph state (from main thread changes).
    // It's OK if the tryLock() fails, we'll just take slightly longer to pick up the changes.
    if (tryLock()) {
        deferredTaskHandler().handleDeferredTasks();

        resolvePromisesForResume();

        // Check to see if source nodes can be stopped because the end time has passed.
        handleStoppableSourceNodes();

        unlock();
    }
}

void AudioContext::handlePostRenderTasks()
{
    ASSERT(isAudioThread());

    // Must use a tryLock() here too.  Don't worry, the lock will very rarely be contended and this method is called frequently.
    // The worst that can happen is that there will be some nodes which will take slightly longer than usual to be deleted or removed
    // from the render graph (in which case they'll render silence).
    if (tryLock()) {
        // Take care of AudioNode tasks where the tryLock() failed previously.
        deferredTaskHandler().breakConnections();

        // Dynamically clean up nodes which are no longer needed.
        releaseFinishedSourceNodes();

        deferredTaskHandler().handleDeferredTasks();
        deferredTaskHandler().requestToDeleteHandlersOnMainThread();

        unlock();
    }
}

void AudioContext::resolvePromisesForResumeOnMainThread()
{
    ASSERT(isMainThread());
    AutoLocker locker(this);

    for (auto& resolver : m_resumeResolvers) {
        if (m_contextState == Closed) {
            resolver->reject(
                DOMException::create(InvalidStateError, "Cannot resume a context that has been closed"));
        } else {
            resolver->resolve();
        }
    }

    m_resumeResolvers.clear();
    m_isResolvingResumePromises = false;
}

void AudioContext::resolvePromisesForResume()
{
    // This runs inside the AudioContext's lock when handling pre-render tasks.
    ASSERT(isAudioThread());
    ASSERT(isGraphOwner());

    // Resolve any pending promises created by resume(). Only do this if we haven't already started
    // resolving these promises. This gets called very often and it takes some time to resolve the
    // promises in the main thread.
    if (!m_isResolvingResumePromises && m_resumeResolvers.size() > 0) {
        m_isResolvingResumePromises = true;
        Platform::current()->mainThread()->postTask(FROM_HERE, threadSafeBind(&AudioContext::resolvePromisesForResumeOnMainThread, this));
    }
}

void AudioContext::rejectPendingResolvers()
{
    ASSERT(isMainThread());

    // Audio context is closing down so reject any resume promises that are still pending.

    for (auto& resolver : m_resumeResolvers) {
        resolver->reject(DOMException::create(InvalidStateError, "Audio context is going away"));
    }
    m_resumeResolvers.clear();
    m_isResolvingResumePromises = false;
}

const AtomicString& AudioContext::interfaceName() const
{
    return EventTargetNames::AudioContext;
}

ExecutionContext* AudioContext::executionContext() const
{
    return m_isStopScheduled ? 0 : ActiveDOMObject::executionContext();
}

void AudioContext::startRendering()
{
    // This is called for both online and offline contexts.
    ASSERT(isMainThread());
    ASSERT(m_destinationNode);

    if (m_contextState == Suspended) {
        destination()->audioDestinationHandler().startRendering();
        setContextState(Running);
    }
}

void AudioContext::stopRendering()
{
    ASSERT(isMainThread());
    ASSERT(m_destinationNode);
    ASSERT(!isOfflineContext());

    if (m_contextState == Running) {
        destination()->audioDestinationHandler().stopRendering();
        setContextState(Suspended);
        deferredTaskHandler().clearHandlersToBeDeleted();
    }
}

void AudioContext::fireCompletionEvent()
{
    ASSERT(isMainThread());
    if (!isMainThread())
        return;

    AudioBuffer* renderedBuffer = m_renderTarget.get();

    // For an offline context, we set the state to closed here so that the oncomplete handler sees
    // that the context has been closed.
    setContextState(Closed);

    ASSERT(renderedBuffer);
    if (!renderedBuffer)
        return;

    // Avoid firing the event if the document has already gone away.
    if (executionContext()) {
        // Call the offline rendering completion event listener and resolve the promise too.
        dispatchEvent(OfflineAudioCompletionEvent::create(renderedBuffer));
        m_offlineResolver->resolve(renderedBuffer);
    }
}

DEFINE_TRACE(AudioContext)
{
    visitor->trace(m_closeResolver);
    visitor->trace(m_offlineResolver);
    visitor->trace(m_renderTarget);
    visitor->trace(m_destinationNode);
    visitor->trace(m_listener);
    // trace() can be called in AudioContext constructor, and
    // m_contextGraphMutex might be unavailable.
    if (m_didInitializeContextGraphMutex) {
        AutoLocker lock(this);
        visitor->trace(m_activeSourceNodes);
    } else {
        visitor->trace(m_activeSourceNodes);
    }
    visitor->trace(m_resumeResolvers);
    RefCountedGarbageCollectedEventTargetWithInlineData<AudioContext>::trace(visitor);
    ActiveDOMObject::trace(visitor);
}

SecurityOrigin* AudioContext::securityOrigin() const
{
    if (executionContext())
        return executionContext()->securityOrigin();

    return nullptr;
}

ScriptPromise AudioContext::closeContext(ScriptState* scriptState)
{
    if (isOfflineContext()) {
        return ScriptPromise::rejectWithDOMException(
            scriptState,
            DOMException::create(InvalidAccessError, "cannot close an OfflineAudioContext."));
    }

    if (isContextClosed()) {
        // We've already closed the context previously, but it hasn't yet been resolved, so just
        // create a new promise and reject it.
        return ScriptPromise::rejectWithDOMException(
            scriptState,
            DOMException::create(InvalidStateError,
                "Cannot close a context that is being closed or has already been closed."));
    }

    m_closeResolver = ScriptPromiseResolver::create(scriptState);
    ScriptPromise promise = m_closeResolver->promise();

    // Stop the audio context. This will stop the destination node from pulling audio anymore. And
    // since we have disconnected the destination from the audio graph, and thus has no references,
    // the destination node can GCed if JS has no references. stop() will also resolve the Promise
    // created here.
    stop();

    return promise;
}

} // namespace blink

#endif // ENABLE(WEB_AUDIO)
