// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CSPDirectiveList_h
#define CSPDirectiveList_h

#include "core/fetch/Resource.h"
#include "core/frame/csp/ContentSecurityPolicy.h"
#include "core/frame/csp/MediaListDirective.h"
#include "core/frame/csp/SourceListDirective.h"
#include "platform/network/ContentSecurityPolicyParsers.h"
#include "platform/network/HTTPParsers.h"
#include "platform/weborigin/KURL.h"
#include "platform/weborigin/ReferrerPolicy.h"
#include "wtf/OwnPtr.h"
#include "wtf/Vector.h"
#include "wtf/text/WTFString.h"

namespace blink {

class ContentSecurityPolicy;

class CSPDirectiveList {
    WTF_MAKE_FAST_ALLOCATED(CSPDirectiveList);
    WTF_MAKE_NONCOPYABLE(CSPDirectiveList);
public:
    static PassOwnPtr<CSPDirectiveList> create(ContentSecurityPolicy*, const UChar* begin, const UChar* end, ContentSecurityPolicyHeaderType, ContentSecurityPolicyHeaderSource);

    void parse(const UChar* begin, const UChar* end);

    const String& header() const { return m_header; }
    ContentSecurityPolicyHeaderType headerType() const { return m_headerType; }
    ContentSecurityPolicyHeaderSource headerSource() const { return m_headerSource; }

    bool allowJavaScriptURLs(const String& contextURL, const WTF::OrdinalNumber& contextLine, ContentSecurityPolicy::ReportingStatus) const;
    bool allowInlineEventHandlers(const String& contextURL, const WTF::OrdinalNumber& contextLine, ContentSecurityPolicy::ReportingStatus) const;
    bool allowInlineScript(const String& contextURL, const WTF::OrdinalNumber& contextLine, ContentSecurityPolicy::ReportingStatus, const String& scriptContent) const;
    bool allowInlineStyle(const String& contextURL, const WTF::OrdinalNumber& contextLine, ContentSecurityPolicy::ReportingStatus, const String& styleContent) const;
    bool allowEval(ScriptState*, ContentSecurityPolicy::ReportingStatus, ContentSecurityPolicy::ExceptionStatus = ContentSecurityPolicy::WillNotThrowException) const;
    bool allowPluginType(const String& type, const String& typeAttribute, const KURL&, ContentSecurityPolicy::ReportingStatus) const;

    bool allowScriptFromSource(const KURL&, ContentSecurityPolicy::RedirectStatus, ContentSecurityPolicy::ReportingStatus) const;
    bool allowObjectFromSource(const KURL&, ContentSecurityPolicy::RedirectStatus, ContentSecurityPolicy::ReportingStatus) const;
    bool allowChildFrameFromSource(const KURL&, ContentSecurityPolicy::RedirectStatus, ContentSecurityPolicy::ReportingStatus) const;
    bool allowImageFromSource(const KURL&, ContentSecurityPolicy::RedirectStatus, ContentSecurityPolicy::ReportingStatus) const;
    bool allowStyleFromSource(const KURL&, ContentSecurityPolicy::RedirectStatus, ContentSecurityPolicy::ReportingStatus) const;
    bool allowFontFromSource(const KURL&, ContentSecurityPolicy::RedirectStatus, ContentSecurityPolicy::ReportingStatus) const;
    bool allowMediaFromSource(const KURL&, ContentSecurityPolicy::RedirectStatus, ContentSecurityPolicy::ReportingStatus) const;
    bool allowManifestFromSource(const KURL&, ContentSecurityPolicy::RedirectStatus, ContentSecurityPolicy::ReportingStatus) const;
    bool allowConnectToSource(const KURL&, ContentSecurityPolicy::RedirectStatus, ContentSecurityPolicy::ReportingStatus) const;
    bool allowFormAction(const KURL&, ContentSecurityPolicy::RedirectStatus, ContentSecurityPolicy::ReportingStatus) const;
    bool allowBaseURI(const KURL&, ContentSecurityPolicy::RedirectStatus, ContentSecurityPolicy::ReportingStatus) const;
    bool allowChildContextFromSource(const KURL&, ContentSecurityPolicy::RedirectStatus, ContentSecurityPolicy::ReportingStatus) const;
    // |allowAncestors| does not need to know whether the resource was a
    // result of a redirect. After a redirect, source paths are usually
    // ignored to stop a page from learning the path to which the
    // request was redirected, but this is not a concern for ancestors,
    // because a child frame can't manipulate the URL of a cross-origin
    // parent.
    bool allowAncestors(LocalFrame*, const KURL&, ContentSecurityPolicy::ReportingStatus) const;
    bool allowScriptNonce(const String&) const;
    bool allowStyleNonce(const String&) const;
    bool allowScriptHash(const CSPHashValue&) const;
    bool allowStyleHash(const CSPHashValue&) const;

    const String& evalDisabledErrorMessage() const { return m_evalDisabledErrorMessage; }
    ReflectedXSSDisposition reflectedXSSDisposition() const { return m_reflectedXSSDisposition; }
    ReferrerPolicy referrerPolicy() const { return m_referrerPolicy; }
    bool didSetReferrerPolicy() const { return m_didSetReferrerPolicy; }
    bool isReportOnly() const { return m_reportOnly; }
    const Vector<String>& reportEndpoints() const { return m_reportEndpoints; }

    // Used to copy plugin-types into a plugin document in a nested
    // browsing context.
    bool hasPluginTypes() const { return !!m_pluginTypes; }
    const String& pluginTypesText() const;

    bool shouldSendCSPHeader(Resource::Type) const;

private:
    CSPDirectiveList(ContentSecurityPolicy*, ContentSecurityPolicyHeaderType, ContentSecurityPolicyHeaderSource);

    bool parseDirective(const UChar* begin, const UChar* end, String& name, String& value);
    void parseReportURI(const String& name, const String& value);
    void parsePluginTypes(const String& name, const String& value);
    void parseReflectedXSS(const String& name, const String& value);
    void parseReferrer(const String& name, const String& value);
    String parseSuboriginName(const String& policy);
    void addDirective(const String& name, const String& value);
    void applySandboxPolicy(const String& name, const String& sandboxPolicy);
    void applySuboriginPolicy(const String& name, const String& suboriginPolicy);
    void enforceStrictMixedContentChecking(const String& name, const String& value);
    void enableInsecureRequestsUpgrade(const String& name, const String& value);

    template <class CSPDirectiveType>
    void setCSPDirective(const String& name, const String& value, OwnPtr<CSPDirectiveType>&);

    SourceListDirective* operativeDirective(SourceListDirective*) const;
    SourceListDirective* operativeDirective(SourceListDirective*, SourceListDirective* override) const;
    void reportViolation(const String& directiveText, const String& effectiveDirective, const String& consoleMessage, const KURL& blockedURL) const;
    void reportViolationWithFrame(const String& directiveText, const String& effectiveDirective, const String& consoleMessage, const KURL& blockedURL, LocalFrame*) const;
    void reportViolationWithLocation(const String& directiveText, const String& effectiveDirective, const String& consoleMessage, const KURL& blockedURL, const String& contextURL, const WTF::OrdinalNumber& contextLine) const;
    void reportViolationWithState(const String& directiveText, const String& effectiveDirective, const String& message, const KURL& blockedURL, ScriptState*, const ContentSecurityPolicy::ExceptionStatus) const;

    bool checkEval(SourceListDirective*) const;
    bool checkInline(SourceListDirective*) const;
    bool checkNonce(SourceListDirective*, const String&) const;
    bool checkHash(SourceListDirective*, const CSPHashValue&) const;
    bool checkSource(SourceListDirective*, const KURL&, ContentSecurityPolicy::RedirectStatus) const;
    bool checkMediaType(MediaListDirective*, const String& type, const String& typeAttribute) const;
    bool checkAncestors(SourceListDirective*, LocalFrame*) const;

    void setEvalDisabledErrorMessage(const String& errorMessage) { m_evalDisabledErrorMessage = errorMessage; }

    bool checkEvalAndReportViolation(SourceListDirective*, const String& consoleMessage, ScriptState*, ContentSecurityPolicy::ExceptionStatus = ContentSecurityPolicy::WillNotThrowException) const;
    bool checkInlineAndReportViolation(SourceListDirective*, const String& consoleMessage, const String& contextURL, const WTF::OrdinalNumber& contextLine, bool isScript, const String& hashValue) const;

    bool checkSourceAndReportViolation(SourceListDirective*, const KURL&, const String& effectiveDirective, ContentSecurityPolicy::RedirectStatus) const;
    bool checkMediaTypeAndReportViolation(MediaListDirective*, const String& type, const String& typeAttribute, const String& consoleMessage) const;
    bool checkAncestorsAndReportViolation(SourceListDirective*, LocalFrame*, const KURL&) const;

    bool denyIfEnforcingPolicy() const { return m_reportOnly; }

    ContentSecurityPolicy* m_policy;

    String m_header;
    ContentSecurityPolicyHeaderType m_headerType;
    ContentSecurityPolicyHeaderSource m_headerSource;

    bool m_reportOnly;
    bool m_hasSandboxPolicy;
    bool m_hasSuboriginPolicy;
    ReflectedXSSDisposition m_reflectedXSSDisposition;

    bool m_didSetReferrerPolicy;
    ReferrerPolicy m_referrerPolicy;

    bool m_strictMixedContentCheckingEnforced;

    bool m_upgradeInsecureRequests;

    OwnPtr<MediaListDirective> m_pluginTypes;
    OwnPtr<SourceListDirective> m_baseURI;
    OwnPtr<SourceListDirective> m_childSrc;
    OwnPtr<SourceListDirective> m_connectSrc;
    OwnPtr<SourceListDirective> m_defaultSrc;
    OwnPtr<SourceListDirective> m_fontSrc;
    OwnPtr<SourceListDirective> m_formAction;
    OwnPtr<SourceListDirective> m_frameAncestors;
    OwnPtr<SourceListDirective> m_frameSrc;
    OwnPtr<SourceListDirective> m_imgSrc;
    OwnPtr<SourceListDirective> m_mediaSrc;
    OwnPtr<SourceListDirective> m_manifestSrc;
    OwnPtr<SourceListDirective> m_objectSrc;
    OwnPtr<SourceListDirective> m_scriptSrc;
    OwnPtr<SourceListDirective> m_styleSrc;

    Vector<String> m_reportEndpoints;

    String m_evalDisabledErrorMessage;
};


} // namespace

#endif
