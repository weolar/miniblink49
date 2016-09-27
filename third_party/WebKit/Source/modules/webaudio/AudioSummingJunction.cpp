/*
 * Copyright (C) 2012, Google Inc. All rights reserved.
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
#include "modules/webaudio/AudioSummingJunction.h"

#include "modules/webaudio/AudioContext.h"
#include "modules/webaudio/AudioNodeOutput.h"
#include <algorithm>

namespace blink {

AudioSummingJunction::AudioSummingJunction(DeferredTaskHandler& handler)
    : m_deferredTaskHandler(handler)
    , m_renderingStateNeedUpdating(false)
{
}

AudioSummingJunction::~AudioSummingJunction()
{
    deferredTaskHandler().removeMarkedSummingJunction(this);
}

void AudioSummingJunction::changedOutputs()
{
    ASSERT(deferredTaskHandler().isGraphOwner());
    if (!m_renderingStateNeedUpdating) {
        deferredTaskHandler().markSummingJunctionDirty(this);
        m_renderingStateNeedUpdating = true;
    }
}

void AudioSummingJunction::updateRenderingState()
{
    ASSERT(deferredTaskHandler().isAudioThread());
    ASSERT(deferredTaskHandler().isGraphOwner());
    if (m_renderingStateNeedUpdating) {
        // Copy from m_outputs to m_renderingOutputs.
        m_renderingOutputs.resize(m_outputs.size());
        unsigned j = 0;
        for (AudioNodeOutput* output : m_outputs) {
            m_renderingOutputs[j++] = output;
            output->updateRenderingState();
        }

        didUpdate();

        m_renderingStateNeedUpdating = false;
    }
}

} // namespace blink

#endif // ENABLE(WEB_AUDIO)
