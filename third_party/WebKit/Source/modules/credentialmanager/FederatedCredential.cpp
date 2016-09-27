// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/credentialmanager/FederatedCredential.h"

#include "bindings/core/v8/ExceptionState.h"
#include "modules/credentialmanager/FederatedCredentialData.h"
#include "platform/credentialmanager/PlatformFederatedCredential.h"
#include "public/platform/WebFederatedCredential.h"

namespace blink {

FederatedCredential* FederatedCredential::create(WebFederatedCredential* webFederatedCredential)
{
    return new FederatedCredential(webFederatedCredential);
}

FederatedCredential* FederatedCredential::create(const FederatedCredentialData& data, ExceptionState& exceptionState)
{
    KURL iconURL = parseStringAsURL(data.iconURL(), exceptionState);
    KURL providerURL = parseStringAsURL(data.provider(), exceptionState);
    if (exceptionState.hadException())
        return nullptr;
    return new FederatedCredential(data.id(), providerURL, data.name(), iconURL);
}

FederatedCredential::FederatedCredential(WebFederatedCredential* webFederatedCredential)
    : Credential(webFederatedCredential->platformCredential())
{
}

FederatedCredential::FederatedCredential(const String& id, const KURL& provider, const String& name, const KURL& icon)
    : Credential(PlatformFederatedCredential::create(id, provider, name, icon))
{
}

const KURL& FederatedCredential::provider() const
{
    return static_cast<PlatformFederatedCredential*>(m_platformCredential.get())->provider();
}

} // namespace blink
