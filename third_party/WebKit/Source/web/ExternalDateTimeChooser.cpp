/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#include "config.h"
#if !ENABLE(INPUT_MULTIPLE_FIELDS_UI)
#include "web/ExternalDateTimeChooser.h"

#include "core/InputTypeNames.h"
#include "core/html/forms/DateTimeChooserClient.h"
#include "public/web/WebDateTimeChooserCompletion.h"
#include "public/web/WebDateTimeChooserParams.h"
#include "public/web/WebViewClient.h"
#include "web/ChromeClientImpl.h"
#include "wtf/text/AtomicString.h"

namespace blink {

class WebDateTimeChooserCompletionImpl : public WebDateTimeChooserCompletion {
public:
    WebDateTimeChooserCompletionImpl(ExternalDateTimeChooser* chooser)
        : m_chooser(chooser)
    {
    }

private:
    void didChooseValue(const WebString& value) override
    {
        m_chooser->didChooseValue(value);
        delete this;
    }

    void didChooseValue(double value) override
    {
        m_chooser->didChooseValue(value);
        delete this;
    }

    void didCancelChooser() override
    {
        m_chooser->didCancelChooser();
        delete this;
    }

    RefPtr<ExternalDateTimeChooser> m_chooser;
};

ExternalDateTimeChooser::~ExternalDateTimeChooser()
{
}

ExternalDateTimeChooser::ExternalDateTimeChooser(DateTimeChooserClient* client)
    : m_client(client)
{
    ASSERT(client);
}

PassRefPtr<ExternalDateTimeChooser> ExternalDateTimeChooser::create(ChromeClientImpl* chromeClient, WebViewClient* webViewClient, DateTimeChooserClient* client, const DateTimeChooserParameters& parameters)
{
    ASSERT(chromeClient);
    RefPtr<ExternalDateTimeChooser> chooser = adoptRef(new ExternalDateTimeChooser(client));
    if (!chooser->openDateTimeChooser(chromeClient, webViewClient, parameters))
        chooser.clear();
    return chooser.release();
}


static WebDateTimeInputType toWebDateTimeInputType(const AtomicString& source)
{
    if (source == InputTypeNames::date)
        return WebDateTimeInputTypeDate;
    if (source == InputTypeNames::datetime)
        return WebDateTimeInputTypeDateTime;
    if (source == InputTypeNames::datetime_local)
        return WebDateTimeInputTypeDateTimeLocal;
    if (source == InputTypeNames::month)
        return WebDateTimeInputTypeMonth;
    if (source == InputTypeNames::time)
        return WebDateTimeInputTypeTime;
    if (source == InputTypeNames::week)
        return WebDateTimeInputTypeWeek;
    return WebDateTimeInputTypeNone;
}

bool ExternalDateTimeChooser::openDateTimeChooser(ChromeClientImpl* chromeClient, WebViewClient* webViewClient, const DateTimeChooserParameters& parameters)
{
    if (!webViewClient)
        return false;

    WebDateTimeChooserParams webParams;
    webParams.type = toWebDateTimeInputType(parameters.type);
    webParams.anchorRectInScreen = parameters.anchorRectInScreen;
    webParams.currentValue = parameters.currentValue;
    webParams.doubleValue = parameters.doubleValue;
    webParams.suggestions = parameters.suggestions;
    webParams.minimum = parameters.minimum;
    webParams.maximum = parameters.maximum;
    webParams.step = parameters.step;
    webParams.stepBase = parameters.stepBase;
    webParams.isRequired = parameters.required;
    webParams.isAnchorElementRTL = parameters.isAnchorElementRTL;

    WebDateTimeChooserCompletion* completion = new WebDateTimeChooserCompletionImpl(this);
    if (webViewClient->openDateTimeChooser(webParams, completion))
        return true;
    // We can't open a chooser. Calling
    // WebDateTimeChooserCompletionImpl::didCancelChooser to delete the
    // WebDateTimeChooserCompletionImpl object and deref this.
    completion->didCancelChooser();
    return false;
}

void ExternalDateTimeChooser::didChooseValue(const WebString& value)
{
    if (m_client)
        m_client->didChooseValue(value);
    // didChooseValue might run JavaScript code, and endChooser() might be
    // called. However DateTimeChooserCompletionImpl still has one reference to
    // this object.
    if (m_client)
        m_client->didEndChooser();
}

void ExternalDateTimeChooser::didChooseValue(double value)
{
    if (m_client)
        m_client->didChooseValue(value);
    // didChooseValue might run JavaScript code, and endChooser() might be
    // called. However DateTimeChooserCompletionImpl still has one reference to
    // this object.
    if (m_client)
        m_client->didEndChooser();
}

void ExternalDateTimeChooser::didCancelChooser()
{
    if (m_client)
        m_client->didEndChooser();
}

void ExternalDateTimeChooser::endChooser()
{
    DateTimeChooserClient* client = m_client;
    m_client = 0;
    client->didEndChooser();
}

AXObject* ExternalDateTimeChooser::rootAXObject()
{
    return 0;
}

} // namespace blink

#endif
