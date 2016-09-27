/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#if ENABLE(WEB_AUDIO)
#include "modules/webaudio/ChannelMergerNode.h"

#include "bindings/core/v8/ExceptionMessages.h"
#include "bindings/core/v8/ExceptionState.h"
#include "core/dom/ExceptionCode.h"
#include "core/dom/ExecutionContext.h"
#include "modules/webaudio/AudioContext.h"
#include "modules/webaudio/AudioNodeInput.h"
#include "modules/webaudio/AudioNodeOutput.h"


namespace blink {

ChannelMergerHandler::ChannelMergerHandler(AudioNode& node, float sampleRate, unsigned numberOfInputs)
    : AudioHandler(NodeTypeChannelMerger, node, sampleRate)
{
    // These properties are fixed for the node and cannot be changed by user.
    m_channelCount = 1;
    m_channelCountMode = Explicit;

    // Create the requested number of inputs.
    for (unsigned i = 0; i < numberOfInputs; ++i)
        addInput();

    // Create the output with the requested number of channels.
    addOutput(numberOfInputs);

    initialize();
}

PassRefPtr<ChannelMergerHandler> ChannelMergerHandler::create(AudioNode& node, float sampleRate, unsigned numberOfInputs)
{
    return adoptRef(new ChannelMergerHandler(node, sampleRate, numberOfInputs));
}

void ChannelMergerHandler::process(size_t framesToProcess)
{
    AudioNodeOutput& output = this->output(0);
    ASSERT_UNUSED(framesToProcess, framesToProcess == output.bus()->length());

    unsigned numberOfOutputChannels = output.numberOfChannels();
    ASSERT(numberOfInputs() == numberOfOutputChannels);

    // Merge multiple inputs into one output.
    for (unsigned i = 0; i < numberOfOutputChannels; ++i) {
        AudioNodeInput& input = this->input(i);
        ASSERT(input.numberOfChannels() == 1);
        AudioChannel* outputChannel = output.bus()->channel(i);
        if (input.isConnected()) {

            // The mixing rules will be applied so multiple channels are down-
            // mixed to mono (when the mixing rule is defined). Note that only
            // the first channel will be taken for the undefined input channel
            // layout.
            //
            // See: http://webaudio.github.io/web-audio-api/#channel-up-mixing-and-down-mixing
            AudioChannel* inputChannel = input.bus()->channel(0);
            outputChannel->copyFrom(inputChannel);

        } else {
            // If input is unconnected, fill zeros in the channel.
            outputChannel->zero();
        }
    }
}

void ChannelMergerHandler::setChannelCount(unsigned long channelCount, ExceptionState& exceptionState)
{
    ASSERT(isMainThread());
    AudioContext::AutoLocker locker(context());

    // channelCount must be 1.
    if (channelCount != 1) {
        exceptionState.throwDOMException(
            InvalidStateError,
            "ChannelMerger: channelCount cannot be changed from 1");
    }
}

void ChannelMergerHandler::setChannelCountMode(const String& mode, ExceptionState& exceptionState)
{
    ASSERT(isMainThread());
    AudioContext::AutoLocker locker(context());

    // channcelCountMode must be 'explicit'.
    if (mode != "explicit") {
        exceptionState.throwDOMException(
            InvalidStateError,
            "ChannelMerger: channelCountMode cannot be changed from 'explicit'");
    }
}

// ----------------------------------------------------------------

ChannelMergerNode::ChannelMergerNode(AudioContext& context, float sampleRate, unsigned numberOfInputs)
    : AudioNode(context)
{
    setHandler(ChannelMergerHandler::create(*this, sampleRate, numberOfInputs));
}

ChannelMergerNode* ChannelMergerNode::create(AudioContext& context, float sampleRate, unsigned numberOfInputs)
{
    if (!numberOfInputs || numberOfInputs > AudioContext::maxNumberOfChannels())
        return nullptr;
    return new ChannelMergerNode(context, sampleRate, numberOfInputs);
}

} // namespace blink

#endif // ENABLE(WEB_AUDIO)
