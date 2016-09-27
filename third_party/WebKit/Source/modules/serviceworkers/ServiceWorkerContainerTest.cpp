
// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/serviceworkers/ServiceWorkerContainer.h"

#include "bindings/core/v8/Dictionary.h"
#include "bindings/core/v8/ScriptFunction.h"
#include "bindings/core/v8/ScriptPromise.h"
#include "bindings/core/v8/ScriptState.h"
#include "bindings/core/v8/V8DOMException.h"
#include "bindings/core/v8/V8GCController.h"
#include "core/dom/DOMException.h"
#include "core/dom/Document.h"
#include "core/dom/ExecutionContext.h"
#include "core/page/FocusController.h"
#include "core/testing/DummyPageHolder.h"
#include "modules/serviceworkers/ServiceWorkerContainerClient.h"
#include "platform/weborigin/KURL.h"
#include "platform/weborigin/SecurityOrigin.h"
#include "public/platform/WebServiceWorkerClientsInfo.h"
#include "public/platform/WebServiceWorkerProvider.h"
#include "public/platform/WebURL.h"
#include "wtf/OwnPtr.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/text/WTFString.h"
#include <gtest/gtest.h>
#include <v8.h>

namespace blink {
namespace {

// Promise-related test support.

struct StubScriptFunction {
public:
    StubScriptFunction()
        : m_callCount(0)
    {
    }

    // The returned ScriptFunction can outlive the StubScriptFunction,
    // but it should not be called after the StubScriptFunction dies.
    v8::Local<v8::Function> function(ScriptState* scriptState)
    {
        return ScriptFunctionImpl::createFunction(scriptState, *this);
    }

    size_t callCount() { return m_callCount; }
    ScriptValue arg() { return m_arg; }

private:
    size_t m_callCount;
    ScriptValue m_arg;

    class ScriptFunctionImpl : public ScriptFunction {
    public:
        static v8::Local<v8::Function> createFunction(ScriptState* scriptState, StubScriptFunction& owner)
        {
            ScriptFunctionImpl* self = new ScriptFunctionImpl(scriptState, owner);
            return self->bindToV8Function();
        }

    private:
        ScriptFunctionImpl(ScriptState* scriptState, StubScriptFunction& owner)
            : ScriptFunction(scriptState)
            , m_owner(owner)
        {
        }

        ScriptValue call(ScriptValue arg) override
        {
            m_owner.m_arg = arg;
            m_owner.m_callCount++;
            return ScriptValue();
        }

        StubScriptFunction& m_owner;
    };
};

class ScriptValueTest {
public:
    virtual ~ScriptValueTest() { }
    virtual void operator()(ScriptValue) const = 0;
};

// Runs microtasks and expects |promise| to be rejected. Calls
// |valueTest| with the value passed to |reject|, if any.
void expectRejected(ScriptState* scriptState, ScriptPromise& promise, const ScriptValueTest& valueTest)
{
    StubScriptFunction resolved, rejected;
    promise.then(resolved.function(scriptState), rejected.function(scriptState));
    promise.isolate()->RunMicrotasks();
    EXPECT_EQ(0ul, resolved.callCount());
    EXPECT_EQ(1ul, rejected.callCount());
    if (rejected.callCount())
        valueTest(rejected.arg());
}

// DOM-related test support.

// Matches a ScriptValue and a DOMException with a specific name and message.
class ExpectDOMException : public ScriptValueTest {
public:
    ExpectDOMException(const String& expectedName, const String& expectedMessage)
        : m_expectedName(expectedName)
        , m_expectedMessage(expectedMessage)
    {
    }

    ~ExpectDOMException() override { }

    void operator()(ScriptValue value) const override
    {
        DOMException* exception = V8DOMException::toImplWithTypeCheck(value.isolate(), value.v8Value());
        EXPECT_TRUE(exception) << "the value should be a DOMException";
        if (!exception)
            return;
        EXPECT_EQ(m_expectedName, exception->name());
        EXPECT_EQ(m_expectedMessage, exception->message());
    }

private:
    String m_expectedName;
    String m_expectedMessage;
};

// Service Worker-specific tests.

class NotReachedWebServiceWorkerProvider : public WebServiceWorkerProvider {
public:
    ~NotReachedWebServiceWorkerProvider() override { }

    void registerServiceWorker(const WebURL& pattern, const WebURL& scriptURL, WebServiceWorkerRegistrationCallbacks* callbacks) override
    {
        ADD_FAILURE() << "the provider should not be called to register a Service Worker";
        delete callbacks;
    }
};

class ServiceWorkerContainerTest : public ::testing::Test {
protected:
    ServiceWorkerContainerTest()
        : m_page(DummyPageHolder::create())
    {
    }

    ~ServiceWorkerContainerTest()
    {
        m_page.clear();
        V8GCController::collectGarbage(isolate());
    }

    ExecutionContext* executionContext() { return &(m_page->document()); }
    v8::Isolate* isolate() { return v8::Isolate::GetCurrent(); }
    ScriptState* scriptState() { return ScriptState::forMainWorld(m_page->document().frame()); }

    void provide(PassOwnPtr<WebServiceWorkerProvider> provider)
    {
        m_page->document().WillBeHeapSupplementable<Document>::provideSupplement(ServiceWorkerContainerClient::supplementName(), ServiceWorkerContainerClient::create(provider));
    }

    void setPageURL(const String& url)
    {
        // For URL completion.
        m_page->document().setURL(KURL(KURL(), url));

        // The basis for security checks.
        m_page->document().setSecurityOrigin(SecurityOrigin::createFromString(url));
    }

    void testRegisterRejected(const String& scriptURL, const String& scope, const ScriptValueTest& valueTest)
    {
        // When the registration is rejected, a register call must not reach
        // the provider.
        provide(adoptPtr(new NotReachedWebServiceWorkerProvider()));

        ServiceWorkerContainer* container = ServiceWorkerContainer::create(executionContext());
        ScriptState::Scope scriptScope(scriptState());
        RegistrationOptions options;
        options.setScope(scope);
        ScriptPromise promise = container->registerServiceWorker(scriptState(), scriptURL, options);
        expectRejected(scriptState(), promise, valueTest);

        container->willBeDetachedFromFrame();
    }

    void testGetRegistrationRejected(const String& documentURL, const ScriptValueTest& valueTest)
    {
        provide(adoptPtr(new NotReachedWebServiceWorkerProvider()));

        ServiceWorkerContainer* container = ServiceWorkerContainer::create(executionContext());
        ScriptState::Scope scriptScope(scriptState());
        ScriptPromise promise = container->getRegistration(scriptState(), documentURL);
        expectRejected(scriptState(), promise, valueTest);

        container->willBeDetachedFromFrame();
    }

private:
    OwnPtr<DummyPageHolder> m_page;
};

TEST_F(ServiceWorkerContainerTest, Register_NonSecureOriginIsRejected)
{
    setPageURL("http://www.example.com/");
    testRegisterRejected(
        "http://www.example.com/worker.js",
        "http://www.example.com/",
        ExpectDOMException("NotSupportedError", "Only secure origins are allowed (see: https://goo.gl/Y0ZkNV)."));
}

TEST_F(ServiceWorkerContainerTest, Register_CrossOriginScriptIsRejected)
{
    setPageURL("https://www.example.com");
    testRegisterRejected(
        "https://www.example.com:8080/", // Differs by port
        "https://www.example.com/",
        ExpectDOMException("SecurityError", "Failed to register a ServiceWorker: The origin of the provided scriptURL ('https://www.example.com:8080') does not match the current origin ('https://www.example.com')."));
}

TEST_F(ServiceWorkerContainerTest, Register_CrossOriginScopeIsRejected)
{
    setPageURL("https://www.example.com");
    testRegisterRejected(
        "https://www.example.com",
        "wss://www.example.com/", // Differs by protocol
        ExpectDOMException("SecurityError", "Failed to register a ServiceWorker: The origin of the provided scope ('wss://www.example.com') does not match the current origin ('https://www.example.com')."));
}

TEST_F(ServiceWorkerContainerTest, GetRegistration_NonSecureOriginIsRejected)
{
    setPageURL("http://www.example.com/");
    testGetRegistrationRejected(
        "http://www.example.com/",
        ExpectDOMException("NotSupportedError", "Only secure origins are allowed (see: https://goo.gl/Y0ZkNV)."));
}

TEST_F(ServiceWorkerContainerTest, GetRegistration_CrossOriginURLIsRejected)
{
    setPageURL("https://www.example.com/");
    testGetRegistrationRejected(
        "https://foo.example.com/", // Differs by host
        ExpectDOMException("SecurityError", "Failed to get a ServiceWorkerRegistration: The origin of the provided documentURL ('https://foo.example.com') does not match the current origin ('https://www.example.com')."));
}

class StubWebServiceWorkerProvider {
public:
    StubWebServiceWorkerProvider()
        : m_registerCallCount(0)
        , m_getRegistrationCallCount(0)
    {
    }

    // Creates a WebServiceWorkerProvider. This can outlive the
    // StubWebServiceWorkerProvider, but |registerServiceWorker| and
    // other methods must not be called after the
    // StubWebServiceWorkerProvider dies.
    PassOwnPtr<WebServiceWorkerProvider> provider()
    {
        return adoptPtr(new WebServiceWorkerProviderImpl(*this));
    }

    size_t registerCallCount() { return m_registerCallCount; }
    const WebURL& registerScope() { return m_registerScope; }
    const WebURL& registerScriptURL() { return m_registerScriptURL; }
    size_t getRegistrationCallCount() { return m_getRegistrationCallCount; }
    const WebURL& getRegistrationURL() { return m_getRegistrationURL; }

private:
    class WebServiceWorkerProviderImpl : public WebServiceWorkerProvider {
    public:
        WebServiceWorkerProviderImpl(StubWebServiceWorkerProvider& owner)
            : m_owner(owner)
        {
        }

        ~WebServiceWorkerProviderImpl() override { }

        void registerServiceWorker(const WebURL& pattern, const WebURL& scriptURL, WebServiceWorkerRegistrationCallbacks* callbacks) override
        {
            m_owner.m_registerCallCount++;
            m_owner.m_registerScope = pattern;
            m_owner.m_registerScriptURL = scriptURL;
            m_registrationCallbacksToDelete.append(adoptPtr(callbacks));
        }

        void getRegistration(const WebURL& documentURL, WebServiceWorkerGetRegistrationCallbacks* callbacks) override
        {
            m_owner.m_getRegistrationCallCount++;
            m_owner.m_getRegistrationURL = documentURL;
            m_getRegistrationCallbacksToDelete.append(adoptPtr(callbacks));
        }

    private:
        StubWebServiceWorkerProvider& m_owner;
        Vector<OwnPtr<WebServiceWorkerRegistrationCallbacks>> m_registrationCallbacksToDelete;
        Vector<OwnPtr<WebServiceWorkerGetRegistrationCallbacks>> m_getRegistrationCallbacksToDelete;
    };

private:
    size_t m_registerCallCount;
    WebURL m_registerScope;
    WebURL m_registerScriptURL;
    size_t m_getRegistrationCallCount;
    WebURL m_getRegistrationURL;
};

TEST_F(ServiceWorkerContainerTest, RegisterUnregister_NonHttpsSecureOriginDelegatesToProvider)
{
    setPageURL("http://localhost/x/index.html");

    StubWebServiceWorkerProvider stubProvider;
    provide(stubProvider.provider());

    ServiceWorkerContainer* container = ServiceWorkerContainer::create(executionContext());

    // register
    {
        ScriptState::Scope scriptScope(scriptState());
        RegistrationOptions options;
        options.setScope("y/");
        container->registerServiceWorker(scriptState(), "/x/y/worker.js", options);

        EXPECT_EQ(1ul, stubProvider.registerCallCount());
        EXPECT_EQ(WebURL(KURL(KURL(), "http://localhost/x/y/")), stubProvider.registerScope());
        EXPECT_EQ(WebURL(KURL(KURL(), "http://localhost/x/y/worker.js")), stubProvider.registerScriptURL());
    }

    container->willBeDetachedFromFrame();
}

TEST_F(ServiceWorkerContainerTest, GetRegistration_OmittedDocumentURLDefaultsToPageURL)
{
    setPageURL("http://localhost/x/index.html");

    StubWebServiceWorkerProvider stubProvider;
    provide(stubProvider.provider());

    ServiceWorkerContainer* container = ServiceWorkerContainer::create(executionContext());

    {
        ScriptState::Scope scriptScope(scriptState());
        container->getRegistration(scriptState(), "");
        EXPECT_EQ(1ul, stubProvider.getRegistrationCallCount());
        EXPECT_EQ(WebURL(KURL(KURL(), "http://localhost/x/index.html")), stubProvider.getRegistrationURL());
    }

    container->willBeDetachedFromFrame();
}

} // namespace
} // namespace blink
