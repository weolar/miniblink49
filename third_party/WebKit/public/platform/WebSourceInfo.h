/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
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

#ifndef WebSourceInfo_h
#define WebSourceInfo_h

#include "WebCommon.h"
#include "WebNonCopyable.h"
#include "WebPrivatePtr.h"
#include "WebString.h"

namespace blink {

class WebSourceInfoPrivate;

class WebSourceInfo {
public:
    enum SourceKind {
        SourceKindNone,
        SourceKindAudio,
        SourceKindVideo
    };

    enum VideoFacingMode {
        VideoFacingModeNone,
        VideoFacingModeUser,
        VideoFacingModeEnvironment
    };

    WebSourceInfo() { }
    WebSourceInfo(const WebSourceInfo& other) { assign(other); }
    ~WebSourceInfo() { reset(); }

    WebSourceInfo& operator=(const WebSourceInfo& other)
    {
        assign(other);
        return *this;
    }

    BLINK_PLATFORM_EXPORT void assign(const WebSourceInfo&);

    BLINK_PLATFORM_EXPORT void initialize(const WebString& id, SourceKind, const WebString& label, VideoFacingMode);
    BLINK_PLATFORM_EXPORT void reset();
    bool isNull() const { return m_private.isNull(); }

    BLINK_PLATFORM_EXPORT WebString id() const;
    BLINK_PLATFORM_EXPORT SourceKind kind() const;
    BLINK_PLATFORM_EXPORT WebString label() const;
    BLINK_PLATFORM_EXPORT VideoFacingMode facing() const;

private:
    WebPrivatePtr<WebSourceInfoPrivate> m_private;
};

} // namespace blink

#endif // WebSourceInfo_h
