// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/credentialmanager/CredentialsContainer.h"

#include "bindings/core/v8/Dictionary.h"
#include "bindings/core/v8/ScriptPromise.h"
#include "bindings/core/v8/ScriptPromiseResolver.h"
#include "core/dom/DOMException.h"
#include "core/dom/ExceptionCode.h"
#include "core/dom/ExecutionContext.h"
#include "modules/credentialmanager/Credential.h"
#include "modules/credentialmanager/CredentialManagerClient.h"
#include "modules/credentialmanager/CredentialRequestOptions.h"
#include "modules/credentialmanager/FederatedCredential.h"
#include "modules/credentialmanager/FederatedCredentialRequestOptions.h"
#include "modules/credentialmanager/PasswordCredential.h"
#include "platform/weborigin/SecurityOrigin.h"
#include "public/platform/Platform.h"
#include "public/platform/WebCredential.h"
#include "public/platform/WebCredentialManagerClient.h"
#include "public/platform/WebCredentialManagerError.h"
#include "public/platform/WebFederatedCredential.h"
#include "public/platform/WebPasswordCredential.h"

namespace blink {

static void rejectDueToCredentialManagerError(PassRefPtrWillBeRawPtr<ScriptPromiseResolver> resolver, WebCredentialManagerError* reason)
{
    switch (reason->errorType) {
    case WebCredentialManagerError::ErrorTypeDisabled:
        resolver->reject(DOMException::create(InvalidStateError, "The credential manager is disabled."));
        break;
    case WebCredentialManagerError::ErrorTypeUnknown:
    default:
        resolver->reject(DOMException::create(NotReadableError, "An unknown error occured while talking to the credential manager."));
        break;
    }
}

class NotificationCallbacks : public WebCredentialManagerClient::NotificationCallbacks {
    WTF_MAKE_NONCOPYABLE(NotificationCallbacks);
public:
    explicit NotificationCallbacks(PassRefPtrWillBeRawPtr<ScriptPromiseResolver> resolver) : m_resolver(resolver) { }
    ~NotificationCallbacks() override { }

    void onSuccess() override
    {
        m_resolver->resolve();
    }

    void onError(WebCredentialManagerError* reason) override
    {
        rejectDueToCredentialManagerError(m_resolver, reason);
    }

private:
    const RefPtrWillBePersistent<ScriptPromiseResolver> m_resolver;
};

class RequestCallbacks : public WebCredentialManagerClient::RequestCallbacks {
    WTF_MAKE_NONCOPYABLE(RequestCallbacks);
public:
    explicit RequestCallbacks(PassRefPtrWillBeRawPtr<ScriptPromiseResolver> resolver) : m_resolver(resolver) { }
    ~RequestCallbacks() override { }

    void onSuccess(WebCredential* credential) override
    {
        if (!credential) {
            m_resolver->resolve();
            return;
        }

        ASSERT(credential->isPasswordCredential() || credential->isFederatedCredential());
        if (credential->isPasswordCredential())
            m_resolver->resolve(PasswordCredential::create(static_cast<WebPasswordCredential*>(credential)));
        else
            m_resolver->resolve(FederatedCredential::create(static_cast<WebFederatedCredential*>(credential)));
    }

    void onError(WebCredentialManagerError* reason) override
    {
        rejectDueToCredentialManagerError(m_resolver, reason);
    }

private:
    const RefPtrWillBePersistent<ScriptPromiseResolver> m_resolver;
};


CredentialsContainer* CredentialsContainer::create()
{
    return new CredentialsContainer();
}

CredentialsContainer::CredentialsContainer()
{
}

static bool checkBoilerplate(PassRefPtrWillBeRawPtr<ScriptPromiseResolver> resolver)
{
    CredentialManagerClient* client = CredentialManagerClient::from(resolver->scriptState()->executionContext());
    if (!client) {
        resolver->reject(DOMException::create(InvalidStateError, "Could not establish connection to the credential manager."));
        return false;
    }

    String errorMessage;
    if (!resolver->scriptState()->executionContext()->isPrivilegedContext(errorMessage)) {
        resolver->reject(DOMException::create(SecurityError, errorMessage));
        return false;
    }

    return true;
}

ScriptPromise CredentialsContainer::request(ScriptState* scriptState, const CredentialRequestOptions& options)
{
    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);
    ScriptPromise promise = resolver->promise();
    if (!checkBoilerplate(resolver))
        return promise;

    Vector<KURL> providers;
    if (options.hasFederated() && options.federated().hasProviders()) {
        for (const auto& string : options.federated().providers()) {
            KURL url = KURL(KURL(), string);
            if (url.isValid())
                providers.append(url);
        }
    }

    CredentialManagerClient::from(scriptState->executionContext())->dispatchRequest(options.suppressUI(), providers, new RequestCallbacks(resolver));
    return promise;
}

ScriptPromise CredentialsContainer::notifySignedIn(ScriptState* scriptState, Credential* credential)
{
    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);
    ScriptPromise promise = resolver->promise();
    if (!checkBoilerplate(resolver))
        return promise;

    CredentialManagerClient::from(scriptState->executionContext())->dispatchSignedIn(WebCredential::create(credential->platformCredential()), new NotificationCallbacks(resolver));
    return promise;
}

ScriptPromise CredentialsContainer::requireUserMediation(ScriptState* scriptState)
{
    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);
    ScriptPromise promise = resolver->promise();
    if (!checkBoilerplate(resolver))
        return promise;

    CredentialManagerClient::from(scriptState->executionContext())->dispatchRequireUserMediation(new NotificationCallbacks(resolver));
    return promise;
}

} // namespace blink
