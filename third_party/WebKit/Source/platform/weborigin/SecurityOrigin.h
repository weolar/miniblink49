/*
 * Copyright (C) 2007, 2008 Apple Inc. All rights reserved.
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
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
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

#ifndef SecurityOrigin_h
#define SecurityOrigin_h

#include "platform/PlatformExport.h"
#include "wtf/ThreadSafeRefCounted.h"
#include "wtf/text/WTFString.h"

namespace blink {

class KURL;
class SecurityOriginCache;

class PLATFORM_EXPORT SecurityOrigin : public RefCounted<SecurityOrigin> {
public:
    static PassRefPtr<SecurityOrigin> create(const KURL&);
    static PassRefPtr<SecurityOrigin> createUnique();

    static PassRefPtr<SecurityOrigin> createFromString(const String&);
    static PassRefPtr<SecurityOrigin> create(const String& protocol, const String& host, int port);

    static void setCache(SecurityOriginCache*);

    // Some URL schemes use nested URLs for their security context. For example,
    // filesystem URLs look like the following:
    //
    //   filesystem:http://example.com/temporary/path/to/file.png
    //
    // We're supposed to use "http://example.com" as the origin.
    //
    // Generally, we add URL schemes to this list when WebKit support them. For
    // example, we don't include the "jar" scheme, even though Firefox
    // understands that "jar" uses an inner URL for it's security origin.
    static bool shouldUseInnerURL(const KURL&);
    static KURL extractInnerURL(const KURL&);

    // Create a deep copy of this SecurityOrigin. This method is useful
    // when marshalling a SecurityOrigin to another thread.
    PassRefPtr<SecurityOrigin> isolatedCopy() const;

    // Set the domain property of this security origin to newDomain. This
    // function does not check whether newDomain is a suffix of the current
    // domain. The caller is responsible for validating newDomain.
    void setDomainFromDOM(const String& newDomain);
    bool domainWasSetInDOM() const { return m_domainWasSetInDOM; }

    String protocol() const { return m_protocol; }
    String host() const { return m_host; }
    String domain() const { return m_domain; }
    unsigned short port() const { return m_port; }

    // Returns true if a given URL is secure, based either directly on its
    // own protocol, or, when relevant, on the protocol of its "inner URL"
    // Protocols like blob: and filesystem: fall into this latter category.
    static bool isSecure(const KURL&);

    // Returns true if this SecurityOrigin can script objects in the given
    // SecurityOrigin. For example, call this function before allowing
    // script from one security origin to read or write objects from
    // another SecurityOrigin.
    bool canAccess(const SecurityOrigin*) const;

    // Same as canAccess, except that it adds an additional check to make sure
    // that the SecurityOrigins have the same suborigin name. If you're not
    // familiar with Suborigins, you probably want canAccess() for now.
    // Suborigins is a spec in progress, and where it should be enforced is
    // still in flux. See https://crbug.com/336894 for more details.
    bool canAccessCheckSuborigins(const SecurityOrigin*) const;

    // Returns true if this SecurityOrigin can read content retrieved from
    // the given URL. For example, call this function before issuing
    // XMLHttpRequests.
    bool canRequest(const KURL&) const;

    // Same as canRequest, except that it adds an additional check to make sure
    // that the SecurityOrigin does not have a suborigin name. Like with
    // canAccessCheckSuborigins() above, if you're not familiar with
    // Suborigins, you probably want canRequest() for now. Suborigins is a spec
    // in progress, and where it should be enforced is still in flux. See
    // https://crbug.com/336894 for more details.
    bool canRequestNoSuborigin(const KURL&) const;

    // Returns true if drawing an image from this URL taints a canvas from
    // this security origin. For example, call this function before
    // drawing an image onto an HTML canvas element with the drawImage API.
    bool taintsCanvas(const KURL&) const;

    // Returns true if |document| can display content from the given URL (e.g.,
    // in an iframe or as an image). For example, web sites generally cannot
    // display content from the user's files system.
    bool canDisplay(const KURL&) const;

    // Returns true if the origin loads resources either from the local
    // machine or over the network from a
    // cryptographically-authenticated origin, as described in
    // https://w3c.github.io/webappsec/specs/powerfulfeatures/#is-origin-trustworthy.
    bool isPotentiallyTrustworthy(String& errorMessage) const;

    // Returns true if this SecurityOrigin can load local resources, such
    // as images, iframes, and style sheets, and can link to local URLs.
    // For example, call this function before creating an iframe to a
    // file:// URL.
    //
    // Note: A SecurityOrigin might be allowed to load local resources
    //       without being able to issue an XMLHttpRequest for a local URL.
    //       To determine whether the SecurityOrigin can issue an
    //       XMLHttpRequest for a URL, call canRequest(url).
    bool canLoadLocalResources() const { return m_canLoadLocalResources; }

    // Explicitly grant the ability to load local resources to this
    // SecurityOrigin.
    //
    // Note: This method exists only to support backwards compatibility
    //       with older versions of WebKit.
    void grantLoadLocalResources();

    // Explicitly grant the ability to access every other SecurityOrigin.
    //
    // WARNING: This is an extremely powerful ability. Use with caution!
    void grantUniversalAccess();

    bool canAccessDatabase() const { return !isUnique(); }
    bool canAccessLocalStorage() const { return !isUnique(); }
    bool canAccessSharedWorkers() const { return !isUnique(); }
    bool canAccessServiceWorkers() const { return !isUnique(); }
    bool canAccessCookies() const { return !isUnique(); }
    bool canAccessPasswordManager() const { return !isUnique(); }
    bool canAccessFileSystem() const { return !isUnique(); }
    bool canAccessCacheStorage() const { return !isUnique(); }

    // Technically, we should always allow access to sessionStorage, but we
    // currently don't handle creating a sessionStorage area for unique
    // origins.
    bool canAccessSessionStorage() const { return !isUnique(); }

    // The local SecurityOrigin is the most privileged SecurityOrigin.
    // The local SecurityOrigin can script any document, navigate to local
    // resources, and can set arbitrary headers on XMLHttpRequests.
    bool isLocal() const;

    // Returns true if the host is one of 127.0.0.1/8, ::1/128, or "localhost".
    bool isLocalhost() const;

    // The origin is a globally unique identifier assigned when the Document is
    // created. http://www.whatwg.org/specs/web-apps/current-work/#sandboxOrigin
    //
    // There's a subtle difference between a unique origin and an origin that
    // has the SandboxOrigin flag set. The latter implies the former, and, in
    // addition, the SandboxOrigin flag is inherited by iframes.
    bool isUnique() const { return m_isUnique; }

    // Assigns a suborigin namespace to the SecurityOrigin. addSuborigin() must
    // only ever be called once per SecurityOrigin(). If it is called on a
    // SecurityOrigin that has already had a suborigin assigned, it will hit a
    // RELEASE_ASSERT().
    void addSuborigin(const String&);
    bool hasSuborigin() const { return !m_suboriginName.isNull(); }
    const String& suboriginName() const { return m_suboriginName; }

    // Marks a file:// origin as being in a domain defined by its path.
    // FIXME 81578: The naming of this is confusing. Files with restricted access to other local files
    // still can have other privileges that can be remembered, thereby not making them unique.
    void enforceFilePathSeparation();

    // Convert this SecurityOrigin into a string. The string
    // representation of a SecurityOrigin is similar to a URL, except it
    // lacks a path component. The string representation does not encode
    // the value of the SecurityOrigin's domain property.
    //
    // When using the string value, it's important to remember that it might be
    // "null". This happens when this SecurityOrigin is unique. For example,
    // this SecurityOrigin might have come from a sandboxed iframe, the
    // SecurityOrigin might be empty, or we might have explicitly decided that
    // we shouldTreatURLSchemeAsNoAccess.
    String toString() const;
    AtomicString toAtomicString() const;

    // Similar to toString(), but does not take into account any factors that
    // could make the string return "null".
    String toRawString() const;
    AtomicString toRawAtomicString() const;

    // This method checks for equality, ignoring the value of document.domain
    // (and whether it was set) but considering the host. It is used for postMessage.
    bool isSameSchemeHostPort(const SecurityOrigin*) const;
    bool isSameSchemeHostPortAndSuborigin(const SecurityOrigin*) const;

    bool needsDatabaseIdentifierQuirkForFiles() const { return m_needsDatabaseIdentifierQuirkForFiles; }

    static const KURL& urlWithUniqueSecurityOrigin();

    // Transfer origin privileges from another security origin.
    // The following privileges are currently copied over:
    //
    //   - Grant universal access.
    //   - Grant loading of local resources.
    //   - Use path-based file:// origins.
    //
    // Note: It is dangerous to change the privileges of an origin
    // at any other time than during initialization.
    void transferPrivilegesFrom(const SecurityOrigin&);

private:
    // FIXME: After the merge with the Chromium repo, this should be refactored
    // to use FRIEND_TEST in base/gtest_prod_util.h.
    friend class SecurityOriginTest;
    friend class SecurityOriginTest_Suborigins_Test;
    friend class SecurityOriginTest_SuboriginsParsing_Test;
    friend class SecurityOriginTest_SuboriginsIsSameSchemeHostPortAndSuborigin_Test;

    SecurityOrigin();
    explicit SecurityOrigin(const KURL&);
    explicit SecurityOrigin(const SecurityOrigin*);

    // FIXME: Rename this function to something more semantic.
    bool passesFileCheck(const SecurityOrigin*) const;
    void buildRawString(StringBuilder&) const;

    static bool deserializeSuboriginAndHost(const String&, String&, String&);

    String m_protocol;
    String m_host;
    String m_domain;
    String m_suboriginName;
    unsigned short m_port;
    bool m_isUnique;
    bool m_universalAccess;
    bool m_domainWasSetInDOM;
    bool m_canLoadLocalResources;
    bool m_enforceFilePathSeparation;
    bool m_needsDatabaseIdentifierQuirkForFiles;
};

} // namespace blink

#endif // SecurityOrigin_h
