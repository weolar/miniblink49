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

#ifndef WebGeolocationController_h
#define WebGeolocationController_h

#include "../platform/WebCommon.h"
#include "../platform/WebNonCopyable.h"

namespace blink {

class GeolocationController;
class WebGeolocationPosition;
class WebGeolocationError;

class WebGeolocationController : public WebNonCopyable {
public:
    BLINK_EXPORT void positionChanged(const WebGeolocationPosition&);
    BLINK_EXPORT void errorOccurred(const WebGeolocationError&);

#if BLINK_IMPLEMENTATION
    WebGeolocationController(GeolocationController* c)
        : m_private(c)
    {
    }

    GeolocationController* controller() const { return m_private; }
#endif

private:
    // No implementation for the default constructor. Declared private to ensure that no instances
    // can be created by the consumers of Chromium WebKit.
    WebGeolocationController();

    // This bare pointer is owned and kept alive by the frame of the
    // WebLocalFrame which creates this controller object.
    GeolocationController* m_private;
};

} // namespace blink

#endif // WebGeolocationController_h
