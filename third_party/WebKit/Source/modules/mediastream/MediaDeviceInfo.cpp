/*
 * Copyright (C) 2014 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY GOOGLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL GOOGLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "modules/mediastream/MediaDeviceInfo.h"

#include "wtf/text/WTFString.h"

namespace blink {

MediaDeviceInfo* MediaDeviceInfo::create(const WebMediaDeviceInfo& webMediaDeviceInfo)
{
    ASSERT(!webMediaDeviceInfo.isNull());
    return new MediaDeviceInfo(webMediaDeviceInfo);
}

MediaDeviceInfo::MediaDeviceInfo(const WebMediaDeviceInfo& webMediaDeviceInfo)
    : m_webMediaDeviceInfo(webMediaDeviceInfo)
{
}

String MediaDeviceInfo::deviceId() const
{
    return m_webMediaDeviceInfo.deviceId();
}

String MediaDeviceInfo::kind() const
{
    switch (m_webMediaDeviceInfo.kind()) {
    case WebMediaDeviceInfo::MediaDeviceKindAudioInput:
        return "audioinput";
    case WebMediaDeviceInfo::MediaDeviceKindAudioOutput:
        return "audiooutput";
    case WebMediaDeviceInfo::MediaDeviceKindVideoInput:
        return "videoinput";
    }

    ASSERT_NOT_REACHED();
    return String();
}

String MediaDeviceInfo::label() const
{
    return m_webMediaDeviceInfo.label();
}

String MediaDeviceInfo::groupId() const
{
    return m_webMediaDeviceInfo.groupId();
}

} // namespace blink
