// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/audio_output_devices/HTMLMediaElementAudioOutputDevice.h"

#include "bindings/core/v8/ScriptPromiseResolver.h"
#include "bindings/core/v8/ScriptState.h"
#include "core/dom/ExecutionContext.h"
#include "modules/audio_output_devices/SetSinkIdCallbacks.h"
#include "platform/Logging.h"

namespace blink {

HTMLMediaElementAudioOutputDevice::HTMLMediaElementAudioOutputDevice()
    : m_sinkId("")
{
}

String HTMLMediaElementAudioOutputDevice::sinkId(HTMLMediaElement& element)
{
    HTMLMediaElementAudioOutputDevice& aodElement = HTMLMediaElementAudioOutputDevice::from(element);
    return aodElement.m_sinkId;
}

void HTMLMediaElementAudioOutputDevice::setSinkId(const String& sinkId)
{
    m_sinkId = sinkId;
}

ScriptPromise HTMLMediaElementAudioOutputDevice::setSinkId(ScriptState* scriptState, HTMLMediaElement& element, const String& sinkId)
{
    WebMediaPlayer* webMediaPlayer = element.webMediaPlayer();
    if (!webMediaPlayer)
        return ScriptPromise::rejectWithDOMException(scriptState, DOMException::create(AbortError, "No media player available"));

    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);
    webMediaPlayer->setSinkId(sinkId, new SetSinkIdCallbacks(resolver, element, sinkId));
    return resolver->promise();
}

const char* HTMLMediaElementAudioOutputDevice::supplementName()
{
    return "HTMLMediaElementAudioOutputDevice";
}

HTMLMediaElementAudioOutputDevice& HTMLMediaElementAudioOutputDevice::from(HTMLMediaElement& element)
{
    HTMLMediaElementAudioOutputDevice* supplement = static_cast<HTMLMediaElementAudioOutputDevice*>(WillBeHeapSupplement<HTMLMediaElement>::from(element, supplementName()));
    if (!supplement) {
        supplement = new HTMLMediaElementAudioOutputDevice();
        provideTo(element, supplementName(), adoptPtrWillBeNoop(supplement));
    }
    return *supplement;
}

DEFINE_TRACE(HTMLMediaElementAudioOutputDevice)
{
    WillBeHeapSupplement<HTMLMediaElement>::trace(visitor);
}

} // namespace blink
