/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
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

#include "config.h"
#include "public/web/WebDOMActivityLogger.h"

#include "bindings/core/v8/V8Binding.h"
#include "bindings/core/v8/V8DOMActivityLogger.h"
#include "core/dom/Document.h"
#include "core/frame/LocalDOMWindow.h"
#include "wtf/PassRefPtr.h"
#include "wtf/text/WTFString.h"

namespace blink {

class DOMActivityLoggerContainer : public V8DOMActivityLogger {
public:
    explicit DOMActivityLoggerContainer(PassOwnPtr<WebDOMActivityLogger> logger)
        : m_domActivityLogger(logger)
    {
    }

    void logGetter(const String& apiName) override
    {
        m_domActivityLogger->logGetter(WebString(apiName), getURL(), getTitle());
    }

    void logSetter(const String& apiName, const v8::Local<v8::Value>& newValue) override
    {
        m_domActivityLogger->logSetter(WebString(apiName), newValue, getURL(), getTitle());
    }

    void logMethod(const String& apiName, int argc, const v8::Local<v8::Value>* argv) override
    {
        m_domActivityLogger->logMethod(WebString(apiName), argc, argv, getURL(), getTitle());
    }

    void logEvent(const String& eventName, int argc, const String* argv) override
    {
        Vector<WebString> webStringArgv;
        for (int i = 0; i < argc; i++)
            webStringArgv.append(argv[i]);
        m_domActivityLogger->logEvent(WebString(eventName), argc, webStringArgv.data(), getURL(), getTitle());
    }

private:
    WebURL getURL()
    {
        if (Document* document = currentDOMWindow(v8::Isolate::GetCurrent())->document())
            return WebURL(document->url());
        return WebURL();
    }

    WebString getTitle()
    {
        if (Document* document = currentDOMWindow(v8::Isolate::GetCurrent())->document())
            return WebString(document->title());
        return WebString();
    }

    OwnPtr<WebDOMActivityLogger> m_domActivityLogger;
};

bool hasDOMActivityLogger(int worldId, const WebString& extensionId)
{
    return V8DOMActivityLogger::activityLogger(worldId, extensionId);
}

void setDOMActivityLogger(int worldId, const WebString& extensionId, WebDOMActivityLogger* logger)
{
    ASSERT(logger);
    V8DOMActivityLogger::setActivityLogger(worldId, extensionId, adoptPtr(new DOMActivityLoggerContainer(adoptPtr(logger))));
}

} // namespace blink
