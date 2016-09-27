/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
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

#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "bindings/core/v8/ScriptController.h"
#include "bindings/core/v8/SerializedScriptValue.h"
#include "bindings/core/v8/V8AbstractEventListener.h"
#include "bindings/core/v8/V8BindingForTesting.h"
#include "bindings/core/v8/V8Event.h"
#include "platform/testing/URLTestHelpers.h"
#include "public/platform/Platform.h"
#include "public/platform/WebUnitTestSupport.h"
#include "public/web/WebDOMCustomEvent.h"
#include "public/web/WebFrame.h"
#include "public/web/WebView.h"
#include "v8.h"
#include "web/WebLocalFrameImpl.h"
#include "web/WebViewImpl.h"
#include "web/tests/FrameTestHelpers.h"
#include <gtest/gtest.h>

namespace blink {

class TestListener : public V8AbstractEventListener {
public:
    bool operator==(const EventListener&) override
    {
        return true;
    }

    void handleEvent(ScriptState* scriptState, Event* event) override
    {
        EXPECT_EQ(event->type(), "blah");

        ScriptState::Scope scope(scriptState);
        v8::Local<v8::Context> context = scriptState->context();
        v8::Local<v8::Value> jsEvent = toV8(event, context->Global(), isolate());

        EXPECT_EQ(jsEvent->ToObject(context).ToLocalChecked()->Get(context, v8AtomicString(isolate(), "detail")).ToLocalChecked(), v8::Boolean::New(isolate(), true));
    }

    static PassRefPtr<TestListener> create(ScriptState* scriptState)
    {
        return adoptRef(new TestListener(scriptState));
    }

private:
    TestListener(ScriptState* scriptState)
        : V8AbstractEventListener(false, scriptState->world(), scriptState->isolate())
    {
    }

    v8::Local<v8::Value> callListenerFunction(ScriptState*, v8::Local<v8::Value>, Event*) override
    {
        ASSERT_NOT_REACHED();
        return v8::Local<v8::Value>();
    }
};

// Tests that a CustomEvent can be initialized with a 'detail' value that is a
// SerializedScriptValue, and that this value can be retrieved and read in an
// event listener. Initialization of a CustomEvent with a SerializedScriptValue
// is an internal API, so it cannot be tested with a LayoutTest.
//
// Triggers crash in GC. http://crbug.com/317669
TEST(CustomEventTest, InitWithSerializedScriptValue)
{
    const std::string baseURL = "http://www.test.com";
    const std::string path = "visible_iframe.html";

    URLTestHelpers::registerMockedURLFromBaseURL(WebString::fromUTF8(baseURL.c_str()), WebString::fromUTF8(path.c_str()));
    FrameTestHelpers::WebViewHelper webViewHelper;
    WebLocalFrameImpl* frame = toWebLocalFrameImpl(webViewHelper.initializeAndLoad(baseURL + path)->mainFrame());
    WebDOMEvent event = frame->frame()->document()->createEvent("CustomEvent", IGNORE_EXCEPTION);
    WebDOMCustomEvent customEvent = event.to<WebDOMCustomEvent>();

    v8::Isolate* isolate = toIsolate(frame->frame());
    V8TestingScope scope(isolate);
    customEvent.initCustomEvent("blah", false, false, WebSerializedScriptValue::serialize(v8::Boolean::New(isolate, true)));
    RefPtr<EventListener> listener = TestListener::create(scope.scriptState());
    frame->frame()->document()->addEventListener("blah", listener, false);
    frame->frame()->document()->dispatchEvent(event);

    Platform::current()->unitTestSupport()->unregisterAllMockedURLs();
}

} // namespace blink
