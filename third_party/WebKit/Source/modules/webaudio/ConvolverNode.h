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

#ifndef ConvolverNode_h
#define ConvolverNode_h

#include "modules/ModulesExport.h"
#include "modules/webaudio/AudioNode.h"
#include "wtf/OwnPtr.h"
#include "wtf/RefPtr.h"
#include "wtf/ThreadingPrimitives.h"

namespace blink {

class AudioBuffer;
class ExceptionState;
class Reverb;

class MODULES_EXPORT ConvolverHandler final : public AudioHandler {
public:
    static PassRefPtr<ConvolverHandler> create(AudioNode&, float sampleRate);
    ~ConvolverHandler() override;

    // AudioHandler
    void process(size_t framesToProcess) override;

    // Impulse responses
    void setBuffer(AudioBuffer*, ExceptionState&);
    AudioBuffer* buffer();

    bool normalize() const { return m_normalize; }
    void setNormalize(bool normalize) { m_normalize = normalize; }

private:
    ConvolverHandler(AudioNode&, float sampleRate);
    double tailTime() const override;
    double latencyTime() const override;

    OwnPtr<Reverb> m_reverb;
    // This Persistent doesn't make a reference cycle including the owner
    // ConvolverNode.
    Persistent<AudioBuffer> m_buffer;

    // This synchronizes dynamic changes to the convolution impulse response with process().
    mutable Mutex m_processLock;

    // Normalize the impulse response or not. Must default to true.
    bool m_normalize;

    // TODO(tkent): Use FRIEND_TEST macro provided by gtest_prod.h
    friend class ConvolverNodeTest_ReverbLifetime_Test;
};

class MODULES_EXPORT ConvolverNode final : public AudioNode {
    DEFINE_WRAPPERTYPEINFO();
public:
    static ConvolverNode* create(AudioContext&, float sampleRate);

    AudioBuffer* buffer() const;
    void setBuffer(AudioBuffer*, ExceptionState&);
    bool normalize() const;
    void setNormalize(bool);

private:
    ConvolverNode(AudioContext&, float sampleRate);
    ConvolverHandler& convolverHandler() const;

    // TODO(tkent): Use FRIEND_TEST macro provided by gtest_prod.h
    friend class ConvolverNodeTest_ReverbLifetime_Test;
};

} // namespace blink

#endif // ConvolverNode_h
