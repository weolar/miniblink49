/*
 * Copyright (C) 2013 Google, Inc. All Rights Reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef XSSAuditorDelegate_h
#define XSSAuditorDelegate_h

#include "platform/heap/Handle.h"
#include "platform/weborigin/KURL.h"
#include "wtf/OwnPtr.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/Vector.h"
#include "wtf/text/TextPosition.h"
#include "wtf/text/WTFString.h"

namespace blink {

class Document;
class FormData;

class XSSInfo {
public:
    static PassOwnPtr<XSSInfo> create(const String& originalURL, bool didBlockEntirePage, bool didSendXSSProtectionHeader, bool didSendCSPHeader)
    {
        return adoptPtr(new XSSInfo(originalURL, didBlockEntirePage, didSendXSSProtectionHeader, didSendCSPHeader));
    }

    String buildConsoleError() const;
    bool isSafeToSendToAnotherThread() const;

    String m_originalURL;
    bool m_didBlockEntirePage;
    bool m_didSendXSSProtectionHeader;
    bool m_didSendCSPHeader;
    TextPosition m_textPosition;

private:
    XSSInfo(const String& originalURL, bool didBlockEntirePage, bool didSendXSSProtectionHeader, bool didSendCSPHeader)
        : m_originalURL(originalURL.isolatedCopy())
        , m_didBlockEntirePage(didBlockEntirePage)
        , m_didSendXSSProtectionHeader(didSendXSSProtectionHeader)
        , m_didSendCSPHeader(didSendCSPHeader)
    { }
};

class XSSAuditorDelegate final {
    DISALLOW_ALLOCATION();
    WTF_MAKE_NONCOPYABLE(XSSAuditorDelegate);
public:
    explicit XSSAuditorDelegate(Document*);
    DECLARE_TRACE();

    void didBlockScript(const XSSInfo&);
    void setReportURL(const KURL& url) { m_reportURL = url; }

private:
    PassRefPtr<FormData> generateViolationReport(const XSSInfo&);

    RawPtrWillBeMember<Document> m_document;
    bool m_didSendNotifications;
    KURL m_reportURL;
};

typedef Vector<OwnPtr<XSSInfo>> XSSInfoStream;

}

#endif
