/*
 * Copyright (C) 2011 Ericsson AB. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer
 *    in the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name of Ericsson nor the names of its contributors
 *    may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef UserMediaClient_h
#define UserMediaClient_h

#include "modules/ModulesExport.h"
#include "modules/mediastream/MediaDevicesRequest.h"
#include "modules/mediastream/UserMediaRequest.h"
#include "platform/mediastream/MediaStreamTrackSourcesRequest.h"

namespace blink {

class LocalFrame;

class UserMediaClient {
public:
    virtual void requestUserMedia(UserMediaRequest*) = 0;
    virtual void cancelUserMediaRequest(UserMediaRequest*) = 0;
    virtual void requestMediaDevices(MediaDevicesRequest*) = 0;
    virtual void cancelMediaDevicesRequest(MediaDevicesRequest*) = 0;
    virtual void requestSources(MediaStreamTrackSourcesRequest*) = 0;

protected:
    virtual ~UserMediaClient() { }
};

MODULES_EXPORT void provideUserMediaTo(LocalFrame&, UserMediaClient*);

} // namespace blink

#endif // UserMediaClient_h
