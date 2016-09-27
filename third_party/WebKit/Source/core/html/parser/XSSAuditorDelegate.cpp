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

#include "config.h"
#include "core/html/parser/XSSAuditorDelegate.h"

#include "core/dom/Document.h"
#include "core/frame/LocalFrame.h"
#include "core/inspector/ConsoleMessage.h"
#include "core/loader/DocumentLoader.h"
#include "core/loader/FrameLoader.h"
#include "core/loader/FrameLoaderClient.h"
#include "core/loader/PingLoader.h"
#include "platform/JSONValues.h"
#include "platform/network/FormData.h"
#include "platform/weborigin/SecurityOrigin.h"
#include "wtf/text/StringBuilder.h"

namespace blink {

String XSSInfo::buildConsoleError() const
{
    StringBuilder message;
    message.appendLiteral("The XSS Auditor ");
    message.append(m_didBlockEntirePage ? "blocked access to" : "refused to execute a script in");
    message.appendLiteral(" '");
    message.append(m_originalURL);
    message.appendLiteral("' because ");
    message.append(m_didBlockEntirePage ? "the source code of a script" : "its source code");
    message.appendLiteral(" was found within the request.");

    if (m_didSendCSPHeader)
        message.appendLiteral(" The server sent a 'Content-Security-Policy' header requesting this behavior.");
    else if (m_didSendXSSProtectionHeader)
        message.appendLiteral(" The server sent an 'X-XSS-Protection' header requesting this behavior.");
    else
        message.appendLiteral(" The auditor was enabled as the server sent neither an 'X-XSS-Protection' nor 'Content-Security-Policy' header.");

    return message.toString();
}

bool XSSInfo::isSafeToSendToAnotherThread() const
{
    return m_originalURL.isSafeToSendToAnotherThread();
}

XSSAuditorDelegate::XSSAuditorDelegate(Document* document)
    : m_document(document)
    , m_didSendNotifications(false)
{
    ASSERT(isMainThread());
    ASSERT(m_document);
}

DEFINE_TRACE(XSSAuditorDelegate)
{
    visitor->trace(m_document);
}

PassRefPtr<FormData> XSSAuditorDelegate::generateViolationReport(const XSSInfo& xssInfo)
{
    ASSERT(isMainThread());

    FrameLoader& frameLoader = m_document->frame()->loader();
    String httpBody;
    if (frameLoader.documentLoader()) {
        if (FormData* formData = frameLoader.documentLoader()->originalRequest().httpBody())
            httpBody = formData->flattenToString();
    }

    RefPtr<JSONObject> reportDetails = JSONObject::create();
    reportDetails->setString("request-url", xssInfo.m_originalURL);
    reportDetails->setString("request-body", httpBody);

    RefPtr<JSONObject> reportObject = JSONObject::create();
    reportObject->setObject("xss-report", reportDetails.release());

    return FormData::create(reportObject->toJSONString().utf8().data());
}

void XSSAuditorDelegate::didBlockScript(const XSSInfo& xssInfo)
{
    ASSERT(isMainThread());

    m_document->addConsoleMessage(ConsoleMessage::create(JSMessageSource, ErrorMessageLevel, xssInfo.buildConsoleError()));

    // stopAllLoaders can detach the LocalFrame, so protect it.
    RefPtrWillBeRawPtr<LocalFrame> protect(m_document->frame());
    FrameLoader& frameLoader = m_document->frame()->loader();
    if (xssInfo.m_didBlockEntirePage)
        frameLoader.stopAllLoaders();

    if (!m_didSendNotifications) {
        m_didSendNotifications = true;

        frameLoader.client()->didDetectXSS(m_document->url(), xssInfo.m_didBlockEntirePage);

        if (!m_reportURL.isEmpty())
            PingLoader::sendViolationReport(m_document->frame(), m_reportURL, generateViolationReport(xssInfo), PingLoader::XSSAuditorViolationReport);
    }

    if (xssInfo.m_didBlockEntirePage)
        m_document->frame()->navigationScheduler().schedulePageBlock(m_document);
}

} // namespace blink
