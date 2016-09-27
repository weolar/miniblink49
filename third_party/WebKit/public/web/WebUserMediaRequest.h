/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
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

#ifndef WebUserMediaRequest_h
#define WebUserMediaRequest_h

#include "public/platform/WebCommon.h"
#include "public/platform/WebPrivatePtr.h"
#include "public/platform/WebSecurityOrigin.h"
#include "public/platform/WebString.h"

namespace blink {

class UserMediaRequest;
class WebDocument;
class WebMediaConstraints;
class WebMediaStream;

class WebUserMediaRequest {
public:
    WebUserMediaRequest() { }
    WebUserMediaRequest(const WebUserMediaRequest& request) { assign(request); }
    ~WebUserMediaRequest() { reset(); }

    WebUserMediaRequest& operator=(const WebUserMediaRequest& other)
    {
        assign(other);
        return *this;
    }

    BLINK_EXPORT void reset();
    bool isNull() const { return m_private.isNull(); }
    BLINK_EXPORT bool equals(const WebUserMediaRequest&) const;
    BLINK_EXPORT void assign(const WebUserMediaRequest&);

    BLINK_EXPORT bool audio() const;
    BLINK_EXPORT bool video() const;
    BLINK_EXPORT WebMediaConstraints audioConstraints() const;
    BLINK_EXPORT WebMediaConstraints videoConstraints() const;

    BLINK_EXPORT WebSecurityOrigin securityOrigin() const;
    BLINK_EXPORT WebDocument ownerDocument() const;

    BLINK_EXPORT void requestSucceeded(const WebMediaStream&);

    BLINK_EXPORT void requestDenied(const WebString& description = WebString());
    BLINK_EXPORT void requestFailedConstraint(const WebString& constraintName, const WebString& description = WebString());
    BLINK_EXPORT void requestFailedUASpecific(const WebString& name, const WebString& constraintName = WebString(), const WebString& description = WebString());

    // DEPRECATED
    BLINK_EXPORT void requestFailed(const WebString& description = WebString()) { requestDenied(description); }

#if BLINK_IMPLEMENTATION
    WebUserMediaRequest(UserMediaRequest*);
    operator UserMediaRequest*() const;
#endif

private:
    WebPrivatePtr<UserMediaRequest> m_private;
};

inline bool operator==(const WebUserMediaRequest& a, const WebUserMediaRequest& b)
{
    return a.equals(b);
}

} // namespace blink

#endif // WebUserMediaRequest_h
