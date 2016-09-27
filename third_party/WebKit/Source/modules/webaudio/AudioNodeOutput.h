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

#ifndef AudioNodeOutput_h
#define AudioNodeOutput_h

#include "modules/webaudio/AudioNode.h"
#include "modules/webaudio/AudioParam.h"
#include "platform/audio/AudioBus.h"
#include "wtf/HashSet.h"
#include "wtf/RefPtr.h"

namespace blink {

class AudioNodeInput;

// AudioNodeOutput represents a single output for an AudioNode.
// It may be connected to one or more AudioNodeInputs.
class AudioNodeOutput final {
public:
    // It's OK to pass 0 for numberOfChannels in which case
    // setNumberOfChannels() must be called later on.
    static PassOwnPtr<AudioNodeOutput> create(AudioHandler*, unsigned numberOfChannels);
    void dispose();

    // Causes our AudioNode to process if it hasn't already for this render quantum.
    // It returns the bus containing the processed audio for this output, returning inPlaceBus if in-place processing was possible.
    // Called from context's audio thread.
    AudioBus* pull(AudioBus* inPlaceBus, size_t framesToProcess);

    // bus() will contain the rendered audio after pull() is called for each rendering time quantum.
    // Called from context's audio thread.
    AudioBus* bus() const;

    // renderingFanOutCount() is the number of AudioNodeInputs that we're connected to during rendering.
    // Unlike fanOutCount() it will not change during the course of a render quantum.
    unsigned renderingFanOutCount() const;

    // Must be called with the context's graph lock.
    void disconnectAll();

    // Disconnect a specific input or AudioParam.
    void disconnectInput(AudioNodeInput &);
    void disconnectAudioParam(AudioParamHandler&);

    void setNumberOfChannels(unsigned);
    unsigned numberOfChannels() const { return m_numberOfChannels; }
    bool isChannelCountKnown() const { return numberOfChannels() > 0; }

    bool isConnected() { return fanOutCount() > 0 || paramFanOutCount() > 0; }

    // Probe if the output node is connected with a certain input or AudioParam
    bool isConnectedToInput(AudioNodeInput &);
    bool isConnectedToAudioParam(AudioParamHandler&);

    // Disable/Enable happens when there are still JavaScript references to a node, but it has otherwise "finished" its work.
    // For example, when a note has finished playing.  It is kept around, because it may be played again at a later time.
    // They must be called with the context's graph lock.
    void disable();
    void enable();

    // updateRenderingState() is called in the audio thread at the start or end of the render quantum to handle any recent changes to the graph state.
    // It must be called with the context's graph lock.
    void updateRenderingState();

private:
    AudioNodeOutput(AudioHandler*, unsigned numberOfChannels);
    // Can be called from any thread.
    AudioHandler& handler() const { return m_handler; }
    DeferredTaskHandler& deferredTaskHandler() const { return m_handler.context()->deferredTaskHandler(); }

    // This reference is safe because the AudioHandler owns this AudioNodeOutput
    // object.
    AudioHandler& m_handler;

    friend class AudioNodeInput;
    friend class AudioParamHandler;

    // These are called from AudioNodeInput.
    // They must be called with the context's graph lock.
    void addInput(AudioNodeInput&);
    void removeInput(AudioNodeInput&);
    void addParam(AudioParamHandler&);
    void removeParam(AudioParamHandler&);

    // fanOutCount() is the number of AudioNodeInputs that we're connected to.
    // This method should not be called in audio thread rendering code, instead renderingFanOutCount() should be used.
    // It must be called with the context's graph lock.
    unsigned fanOutCount();

    // Similar to fanOutCount(), paramFanOutCount() is the number of AudioParams that we're connected to.
    // This method should not be called in audio thread rendering code, instead renderingParamFanOutCount() should be used.
    // It must be called with the context's graph lock.
    unsigned paramFanOutCount();

    // Must be called with the context's graph lock.
    void disconnectAllInputs();
    void disconnectAllParams();

    // updateInternalBus() updates m_internalBus appropriately for the number of channels.
    // It is called in the constructor or in the audio thread with the context's graph lock.
    void updateInternalBus();

    // Announce to any nodes we're connected to that we changed our channel count for its input.
    // It must be called in the audio thread with the context's graph lock.
    void propagateChannelCount();

    // updateNumberOfChannels() is called in the audio thread at the start or end of the render quantum to pick up channel changes.
    // It must be called with the context's graph lock.
    void updateNumberOfChannels();

    // m_numberOfChannels will only be changed in the audio thread.
    // The main thread sets m_desiredNumberOfChannels which will later get picked up in the audio thread in updateNumberOfChannels().
    unsigned m_numberOfChannels;
    unsigned m_desiredNumberOfChannels;

    // m_internalBus and m_inPlaceBus must only be changed in the audio thread with the context's graph lock (or constructor).
    RefPtr<AudioBus> m_internalBus;
    RefPtr<AudioBus> m_inPlaceBus;
    // If m_isInPlace is true, use m_inPlaceBus as the valid AudioBus; If false, use the default m_internalBus.
    bool m_isInPlace;

    // This HashSet holds connection references. We must call
    // AudioNode::makeConnection when we add an AudioNodeInput to this, and must
    // call AudioNode::breakConnection() when we remove an AudioNodeInput from
    // this.
    HashSet<AudioNodeInput*> m_inputs;
    bool m_isEnabled;

#if ENABLE(ASSERT)
    bool m_didCallDispose;
#endif

    // For the purposes of rendering, keeps track of the number of inputs and AudioParams we're connected to.
    // These value should only be changed at the very start or end of the rendering quantum.
    unsigned m_renderingFanOutCount;
    unsigned m_renderingParamFanOutCount;

    // This collection of raw pointers is safe because they are retained by
    // AudioParam objects retained by m_connectedParams of the owner AudioNode.
    HashSet<AudioParamHandler*> m_params;
};

} // namespace blink

#endif // AudioNodeOutput_h
