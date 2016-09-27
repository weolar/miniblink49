// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef StereoPannerNode_h
#define StereoPannerNode_h

#include "modules/webaudio/AudioNode.h"
#include "modules/webaudio/AudioParam.h"
#include "platform/audio/AudioBus.h"
#include "platform/audio/Spatializer.h"

namespace blink {

// StereoPannerNode is an AudioNode with one input and one output. It is
// specifically designed for equal-power stereo panning.
class StereoPannerHandler final : public AudioHandler {
public:
    static PassRefPtr<StereoPannerHandler> create(AudioNode&, float sampleRate, AudioParamHandler& pan);
    ~StereoPannerHandler() override;

    void process(size_t framesToProcess) override;
    void initialize() override;

    void setChannelCount(unsigned long, ExceptionState&) final;
    void setChannelCountMode(const String&, ExceptionState&) final;

private:
    StereoPannerHandler(AudioNode&, float sampleRate, AudioParamHandler& pan);

    OwnPtr<Spatializer> m_stereoPanner;
    RefPtr<AudioParamHandler> m_pan;

    AudioFloatArray m_sampleAccuratePanValues;

    // TODO(tkent): Use FRIEND_TEST macro provided by gtest_prod.h
    friend class StereoPannerNodeTest_StereoPannerLifetime_Test;
};

class StereoPannerNode final : public AudioNode {
    DEFINE_WRAPPERTYPEINFO();
public:
    static StereoPannerNode* create(AudioContext&, float sampleRate);
    DECLARE_VIRTUAL_TRACE();

    AudioParam* pan() const;

private:
    StereoPannerNode(AudioContext&, float sampleRate);

    Member<AudioParam> m_pan;
};

} // namespace blink

#endif // StereoPannerNode_h
