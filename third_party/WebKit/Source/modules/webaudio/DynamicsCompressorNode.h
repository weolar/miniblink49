/*
 * Copyright (C) 2011, Google Inc. All rights reserved.
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

#ifndef DynamicsCompressorNode_h
#define DynamicsCompressorNode_h

#include "modules/ModulesExport.h"
#include "modules/webaudio/AudioNode.h"
#include "modules/webaudio/AudioParam.h"
#include "wtf/OwnPtr.h"

namespace blink {

class DynamicsCompressor;

class MODULES_EXPORT DynamicsCompressorHandler final : public AudioHandler {
public:
    static PassRefPtr<DynamicsCompressorHandler> create(AudioNode&, float sampleRate, AudioParamHandler& threshold, AudioParamHandler& knee, AudioParamHandler& ratio, AudioParamHandler& reduction, AudioParamHandler& attack, AudioParamHandler& release);
    ~DynamicsCompressorHandler();

    // AudioHandler
    void process(size_t framesToProcess) override;
    void initialize() override;
    void clearInternalStateWhenDisabled() override;

private:
    DynamicsCompressorHandler(AudioNode&, float sampleRate, AudioParamHandler& threshold, AudioParamHandler& knee, AudioParamHandler& ratio, AudioParamHandler& reduction, AudioParamHandler& attack, AudioParamHandler& release);
    double tailTime() const override;
    double latencyTime() const override;

    OwnPtr<DynamicsCompressor> m_dynamicsCompressor;
    RefPtr<AudioParamHandler> m_threshold;
    RefPtr<AudioParamHandler> m_knee;
    RefPtr<AudioParamHandler> m_ratio;
    RefPtr<AudioParamHandler> m_reduction;
    RefPtr<AudioParamHandler> m_attack;
    RefPtr<AudioParamHandler> m_release;

    // TODO(tkent): Use FRIEND_TEST macro provided by gtest_prod.h
    friend class DynamicsCompressorNodeTest_ProcessorLifetime_Test;
};

class MODULES_EXPORT DynamicsCompressorNode final : public AudioNode {
    DEFINE_WRAPPERTYPEINFO();
public:
    static DynamicsCompressorNode* create(AudioContext&, float sampleRate);
    DECLARE_VIRTUAL_TRACE();

    AudioParam* threshold() const;
    AudioParam* knee() const;
    AudioParam* ratio() const;
    AudioParam* reduction() const;
    AudioParam* attack() const;
    AudioParam* release() const;

private:
    DynamicsCompressorNode(AudioContext&, float sampleRate);
    DynamicsCompressorHandler& dynamicsCompressorHandler() const;

    Member<AudioParam> m_threshold;
    Member<AudioParam> m_knee;
    Member<AudioParam> m_ratio;
    Member<AudioParam> m_reduction;
    Member<AudioParam> m_attack;
    Member<AudioParam> m_release;

    // TODO(tkent): Use FRIEND_TEST macro provided by gtest_prod.h
    friend class DynamicsCompressorNodeTest_ProcessorLifetime_Test;
};

} // namespace blink

#endif // DynamicsCompressorNode_h
