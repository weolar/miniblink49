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

#ifndef AudioBasicProcessorHandler_h
#define AudioBasicProcessorHandler_h

#include "modules/ModulesExport.h"
#include "modules/webaudio/AudioNode.h"
#include "wtf/Forward.h"

namespace blink {

class AudioNodeInput;
class AudioProcessor;

// AudioBasicProcessorHandler is an AudioHandler with one input and one output
// where the input and output have the same number of channels.
class MODULES_EXPORT AudioBasicProcessorHandler : public AudioHandler {
public:
    static PassRefPtr<AudioBasicProcessorHandler> create(NodeType, AudioNode&, float sampleRate, PassOwnPtr<AudioProcessor>);
    ~AudioBasicProcessorHandler() override;

    // AudioHandler
    void process(size_t framesToProcess) final;
    void pullInputs(size_t framesToProcess) final;
    void initialize() final;
    void uninitialize() final;

    // Called in the main thread when the number of channels for the input may have changed.
    void checkNumberOfChannelsForInput(AudioNodeInput*) final;

    // Returns the number of channels for both the input and the output.
    unsigned numberOfChannels();
    AudioProcessor* processor() { return m_processor.get(); }

private:
    AudioBasicProcessorHandler(NodeType, AudioNode&, float sampleRate, PassOwnPtr<AudioProcessor>);
    double tailTime() const final;
    double latencyTime() const final;

    OwnPtr<AudioProcessor> m_processor;
};

} // namespace blink

#endif // AudioBasicProcessorHandler_h
