/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
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

#ifndef WebSecurityOrigin_h
#define WebSecurityOrigin_h

// TODO(tasak): WebSecurityOrigin should be in public/platform.
// However, we could not move this file soon, because
// content depends on public/web/WebSecurityOrigin. So firstly
// (1) fix and create another WebSecurityOrigin.h in public/platform,
// and copy the WebSecurityOrigin.h here.
// (2) fix content side to use public/platform/WebSecurityOrigin after
// blink roll.
// (3) remove this public/web/WebSecurityOrigin.h.

#include "public/platform/WebCommon.h"

#if INSIDE_BLINK
#include "wtf/PassRefPtr.h"
#endif

namespace blink {

class SecurityOrigin;
class WebSecurityOriginPrivate;
class WebString;
class WebURL;

class WebSecurityOrigin {
public:
    ~WebSecurityOrigin() { reset(); }

    WebSecurityOrigin() : m_private(0) { }
    WebSecurityOrigin(const WebSecurityOrigin& s) : m_private(0) { assign(s); }
    WebSecurityOrigin& operator=(const WebSecurityOrigin& s)
    {
        assign(s);
        return *this;
    }

    BLINK_PLATFORM_EXPORT static WebSecurityOrigin createFromDatabaseIdentifier(const WebString& databaseIdentifier);
    BLINK_PLATFORM_EXPORT static WebSecurityOrigin createFromString(const WebString&);
    BLINK_PLATFORM_EXPORT static WebSecurityOrigin create(const WebURL&);

    BLINK_PLATFORM_EXPORT void reset();
    BLINK_PLATFORM_EXPORT void assign(const WebSecurityOrigin&);

    bool isNull() const { return !m_private; }

    BLINK_PLATFORM_EXPORT WebString protocol() const;
    BLINK_PLATFORM_EXPORT WebString host() const;
    BLINK_PLATFORM_EXPORT unsigned short port() const;

    // A unique WebSecurityOrigin is the least privileged WebSecurityOrigin.
    BLINK_PLATFORM_EXPORT bool isUnique() const;

    // Returns true if this WebSecurityOrigin can script objects in the given
    // SecurityOrigin. For example, call this function before allowing
    // script from one security origin to read or write objects from
    // another SecurityOrigin.
    BLINK_PLATFORM_EXPORT bool canAccess(const WebSecurityOrigin&) const;

    // Returns true if this WebSecurityOrigin can read content retrieved from
    // the given URL. For example, call this function before allowing script
    // from a given security origin to receive contents from a given URL.
    BLINK_PLATFORM_EXPORT bool canRequest(const WebURL&) const;

    // Returns true if the origin loads resources either from the local
    // machine or over the network from a
    // cryptographically-authenticated origin, as described in
    // https://w3c.github.io/webappsec/specs/powerfulfeatures/#is-origin-trustworthy.
    BLINK_PLATFORM_EXPORT bool isPotentiallyTrustworthy(WebString& errorMessage) const;

    // Returns a string representation of the WebSecurityOrigin.  The empty
    // WebSecurityOrigin is represented by "null".  The representation of a
    // non-empty WebSecurityOrigin resembles a standard URL.
    BLINK_PLATFORM_EXPORT WebString toString() const;

    // Returns a string representation of this WebSecurityOrigin that can
    // be used as a file.  Should be used in storage APIs only.
    BLINK_PLATFORM_EXPORT WebString databaseIdentifier() const;

    // Returns true if this WebSecurityOrigin can access usernames and
    // passwords stored in password manager.
    BLINK_PLATFORM_EXPORT bool canAccessPasswordManager() const;

    // Allows this WebSecurityOrigin access to local resources.
    BLINK_PLATFORM_EXPORT void grantLoadLocalResources() const;

#if INSIDE_BLINK
    BLINK_PLATFORM_EXPORT WebSecurityOrigin(const WTF::PassRefPtr<SecurityOrigin>&);
    BLINK_PLATFORM_EXPORT WebSecurityOrigin& operator=(const WTF::PassRefPtr<SecurityOrigin>&);
    BLINK_PLATFORM_EXPORT operator WTF::PassRefPtr<SecurityOrigin>() const;
    BLINK_PLATFORM_EXPORT SecurityOrigin* get() const;
#endif

private:
    void assign(WebSecurityOriginPrivate*);
    WebSecurityOriginPrivate* m_private;
};

} // namespace blink

#endif
