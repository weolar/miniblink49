/*
 * Copyright (C) 2011 Google, Inc. All rights reserved.
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
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef ContentSecurityPolicy_h
#define ContentSecurityPolicy_h

#include "bindings/core/v8/ScriptState.h"
#include "core/CoreExport.h"
#include "core/dom/ExecutionContext.h"
#include "core/dom/SecurityContext.h"
#include "core/fetch/Resource.h"
#include "core/frame/ConsoleTypes.h"
#include "platform/network/ContentSecurityPolicyParsers.h"
#include "platform/network/HTTPParsers.h"
#include "platform/weborigin/ReferrerPolicy.h"
#include "wtf/HashSet.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefCounted.h"
#include "wtf/Vector.h"
#include "wtf/text/StringHash.h"
#include "wtf/text/TextPosition.h"
#include "wtf/text/WTFString.h"
#include <utility>

namespace WTF {
class OrdinalNumber;
}

namespace blink {

class ContentSecurityPolicyResponseHeaders;
class ConsoleMessage;
class CSPDirectiveList;
class CSPSource;
class Document;
class KURL;
class ResourceRequest;
class SecurityOrigin;

typedef int SandboxFlags;
typedef Vector<OwnPtr<CSPDirectiveList>> CSPDirectiveListVector;
typedef WillBePersistentHeapVector<RefPtrWillBeMember<ConsoleMessage>> ConsoleMessageVector;
typedef std::pair<String, ContentSecurityPolicyHeaderType> CSPHeaderAndType;

class CORE_EXPORT ContentSecurityPolicy : public RefCounted<ContentSecurityPolicy> {
    WTF_MAKE_FAST_ALLOCATED(ContentSecurityPolicy);
public:
    // CSP Level 1 Directives
    static const char ConnectSrc[];
    static const char DefaultSrc[];
    static const char FontSrc[];
    static const char FrameSrc[];
    static const char ImgSrc[];
    static const char MediaSrc[];
    static const char ObjectSrc[];
    static const char ReportURI[];
    static const char Sandbox[];
    static const char ScriptSrc[];
    static const char StyleSrc[];

    // CSP Level 2 Directives
    static const char BaseURI[];
    static const char ChildSrc[];
    static const char FormAction[];
    static const char FrameAncestors[];
    static const char PluginTypes[];
    static const char ReflectedXSS[];
    static const char Referrer[];

    // Manifest Directives (to be merged into CSP Level 2)
    // https://w3c.github.io/manifest/#content-security-policy
    static const char ManifestSrc[];

    // Mixed Content Directive
    // https://w3c.github.io/webappsec/specs/mixedcontent/#strict-mode
    static const char BlockAllMixedContent[];

    // https://w3c.github.io/webappsec/specs/upgrade/
    static const char UpgradeInsecureRequests[];

    // Suborigin Directive
    // https://metromoxie.github.io/webappsec/specs/suborigins/index.html
    static const char Suborigin[];

    enum ReportingStatus {
        SendReport,
        SuppressReport
    };

    // When a resource is loaded after a redirect, source paths are
    // ignored in the matching algorithm.
    enum RedirectStatus {
        DidRedirect,
        DidNotRedirect
    };

    enum ExceptionStatus {
        WillThrowException,
        WillNotThrowException
    };

    static PassRefPtr<ContentSecurityPolicy> create()
    {
        return adoptRef(new ContentSecurityPolicy());
    }
    ~ContentSecurityPolicy();

    void bindToExecutionContext(ExecutionContext*);
    void copyStateFrom(const ContentSecurityPolicy*);
    void copyPluginTypesFrom(const ContentSecurityPolicy*);

    void didReceiveHeaders(const ContentSecurityPolicyResponseHeaders&);
    void didReceiveHeader(const String&, ContentSecurityPolicyHeaderType, ContentSecurityPolicyHeaderSource);

    const PassOwnPtr<Vector<CSPHeaderAndType>> headers() const;

    bool allowJavaScriptURLs(const String& contextURL, const WTF::OrdinalNumber& contextLine, ReportingStatus = SendReport) const;
    bool allowInlineEventHandlers(const String& contextURL, const WTF::OrdinalNumber& contextLine, ReportingStatus = SendReport) const;
    bool allowInlineScript(const String& contextURL, const WTF::OrdinalNumber& contextLine, const String& scriptContent, ReportingStatus = SendReport) const;
    bool allowInlineStyle(const String& contextURL, const WTF::OrdinalNumber& contextLine, const String& styleContent, ReportingStatus = SendReport) const;
    // When the reporting status is |SendReport|, the |ExceptionStatus|
    // should indicate whether the caller will throw a JavaScript
    // exception in the event of a violation. When the caller will throw
    // an exception, ContentSecurityPolicy does not log a violation
    // message to the console because it would be redundant.
    bool allowEval(ScriptState* = nullptr, ReportingStatus = SendReport, ExceptionStatus = WillNotThrowException) const;
    bool allowPluginType(const String& type, const String& typeAttribute, const KURL&, ReportingStatus = SendReport) const;
    // Checks whether the plugin type should be allowed in the given
    // document; enforces the CSP rule that PluginDocuments inherit
    // plugin-types directives from the parent document.
    bool allowPluginTypeForDocument(const Document&, const String& type, const String& typeAttribute, const KURL&, ReportingStatus = SendReport) const;

    bool allowScriptFromSource(const KURL&, RedirectStatus = DidNotRedirect, ReportingStatus = SendReport) const;
    bool allowObjectFromSource(const KURL&, RedirectStatus = DidNotRedirect, ReportingStatus = SendReport) const;
    bool allowChildFrameFromSource(const KURL&, RedirectStatus = DidNotRedirect, ReportingStatus = SendReport) const;
    bool allowImageFromSource(const KURL&, RedirectStatus = DidNotRedirect, ReportingStatus = SendReport) const;
    bool allowStyleFromSource(const KURL&, RedirectStatus = DidNotRedirect, ReportingStatus = SendReport) const;
    bool allowFontFromSource(const KURL&, RedirectStatus = DidNotRedirect, ReportingStatus = SendReport) const;
    bool allowMediaFromSource(const KURL&, RedirectStatus = DidNotRedirect, ReportingStatus = SendReport) const;
    bool allowConnectToSource(const KURL&, RedirectStatus = DidNotRedirect, ReportingStatus = SendReport) const;
    bool allowFormAction(const KURL&, RedirectStatus = DidNotRedirect, ReportingStatus = SendReport) const;
    bool allowBaseURI(const KURL&, RedirectStatus = DidNotRedirect, ReportingStatus = SendReport) const;
    bool allowWorkerContextFromSource(const KURL&, RedirectStatus = DidNotRedirect, ReportingStatus = SendReport) const;

    bool allowManifestFromSource(const KURL&, RedirectStatus = DidNotRedirect, ReportingStatus = SendReport) const;

    // |allowAncestors| does not need to know whether the resource was a
    // result of a redirect. After a redirect, source paths are usually
    // ignored to stop a page from learning the path to which the
    // request was redirected, but this is not a concern for ancestors,
    // because a child frame can't manipulate the URL of a cross-origin
    // parent.
    bool allowAncestors(LocalFrame*, const KURL&, ReportingStatus = SendReport) const;

    // The nonce and hash allow functions are guaranteed to not have any side
    // effects, including reporting.
    // Nonce/Hash functions check all policies relating to use of a script/style
    // with the given nonce/hash and return true all CSP policies allow it.
    // If these return true, callers can then process the content or
    // issue a load and be safe disabling any further CSP checks.
    bool allowScriptWithNonce(const String& nonce) const;
    bool allowStyleWithNonce(const String& nonce) const;
    bool allowScriptWithHash(const String& source) const;
    bool allowStyleWithHash(const String& source) const;

    void usesScriptHashAlgorithms(uint8_t ContentSecurityPolicyHashAlgorithm);
    void usesStyleHashAlgorithms(uint8_t ContentSecurityPolicyHashAlgorithm);

    ReflectedXSSDisposition reflectedXSSDisposition() const;

    bool didSetReferrerPolicy() const;

    void setOverrideAllowInlineStyle(bool);
    void setOverrideURLForSelf(const KURL&);

    bool isActive() const;

    // If a frame is passed in, the message will be logged to its active document's console.
    // Otherwise, the message will be logged to this object's |m_executionContext|.
    void logToConsole(PassRefPtrWillBeRawPtr<ConsoleMessage>, LocalFrame* = nullptr);

    void reportDirectiveAsSourceExpression(const String& directiveName, const String& sourceExpression);
    void reportDuplicateDirective(const String&);
    void reportInvalidDirectiveValueCharacter(const String& directiveName, const String& value);
    void reportInvalidPathCharacter(const String& directiveName, const String& value, const char);
    void reportInvalidPluginTypes(const String&);
    void reportInvalidSandboxFlags(const String&);
    void reportInvalidSuboriginFlags(const String&);
    void reportInvalidSourceExpression(const String& directiveName, const String& source);
    void reportInvalidReflectedXSS(const String&);
    void reportMissingReportURI(const String&);
    void reportUnsupportedDirective(const String&);
    void reportInvalidInReportOnly(const String&);
    void reportInvalidReferrer(const String&);
    void reportReportOnlyInMeta(const String&);
    void reportMetaOutsideHead(const String&);
    void reportSuboriginInMeta(const String&);
    void reportValueForEmptyDirective(const String& directiveName, const String& value);

    // If a frame is passed in, the report will be sent using it as a context. If no frame is
    // passed in, the report will be sent via this object's |m_executionContext| (or dropped
    // on the floor if no such context is available).
    void reportViolation(const String& directiveText, const String& effectiveDirective, const String& consoleMessage, const KURL& blockedURL, const Vector<String>& reportEndpoints, const String& header, LocalFrame* = nullptr);

    void reportBlockedScriptExecutionToInspector(const String& directiveText) const;

    const KURL url() const;
    void enforceSandboxFlags(SandboxFlags);
    void enforceSuborigin(const String&);
    void enforceStrictMixedContentChecking();
    String evalDisabledErrorMessage() const;

    void setInsecureRequestsPolicy(SecurityContext::InsecureRequestsPolicy);
    SecurityContext::InsecureRequestsPolicy insecureRequestsPolicy() const { return m_insecureRequestsPolicy; }

    bool urlMatchesSelf(const KURL&) const;
    bool protocolMatchesSelf(const KURL&) const;
    bool selfMatchesInnerURL() const;

    bool experimentalFeaturesEnabled() const;

    bool shouldSendCSPHeader(Resource::Type) const;

    static bool shouldBypassMainWorld(const ExecutionContext*);

    static bool isDirectiveName(const String&);

    // These functions are used to debug using ResourceContext to apply
    // CSP directives instead of Resource::Type, by checking that the
    // ResourceContext is as expected. See crbug.com/474412
    static bool isScriptResource(const ResourceRequest&);
    static bool isStyleResource(const ResourceRequest&);
    static bool isImageResource(const ResourceRequest&);
    static bool isFontResource(const ResourceRequest&);
    static bool isMediaResource(const ResourceRequest&);

private:
    ContentSecurityPolicy();

    void applyPolicySideEffectsToExecutionContext();

    Document* document() const;
    SecurityOrigin* securityOrigin() const;
    KURL completeURL(const String&) const;

    void logToConsole(const String& message, MessageLevel = ErrorMessageLevel);
    void addPolicyFromHeaderValue(const String&, ContentSecurityPolicyHeaderType, ContentSecurityPolicyHeaderSource);

    bool shouldSendViolationReport(const String&) const;
    void didSendViolationReport(const String&);

    ExecutionContext* m_executionContext;
    bool m_overrideInlineStyleAllowed;
    CSPDirectiveListVector m_policies;
    ConsoleMessageVector m_consoleMessages;

    HashSet<unsigned, AlreadyHashed> m_violationReportsSent;

    // We put the hash functions used on the policy object so that we only need
    // to calculate a hash once and then distribute it to all of the directives
    // for validation.
    uint8_t m_scriptHashAlgorithmsUsed;
    uint8_t m_styleHashAlgorithmsUsed;

    // State flags used to configure the environment after parsing a policy.
    SandboxFlags m_sandboxMask;
    String m_suboriginName;
    bool m_enforceStrictMixedContentChecking;
    ReferrerPolicy m_referrerPolicy;
    String m_disableEvalErrorMessage;
    SecurityContext::InsecureRequestsPolicy m_insecureRequestsPolicy;

    OwnPtr<CSPSource> m_selfSource;
    String m_selfProtocol;
};

}

#endif
