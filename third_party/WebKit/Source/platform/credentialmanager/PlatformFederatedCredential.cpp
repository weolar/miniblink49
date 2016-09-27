// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "platform/credentialmanager/PlatformFederatedCredential.h"

namespace blink {

PlatformFederatedCredential* PlatformFederatedCredential::create(const String& id, const KURL& provider, const String& name, const KURL& iconURL)
{
    return new PlatformFederatedCredential(id, provider, name, iconURL);
}

PlatformFederatedCredential::PlatformFederatedCredential(const String& id, const KURL& provider, const String& name, const KURL& iconURL)
    : PlatformCredential(id, name, iconURL)
    , m_provider(provider)
{
    setType("federated");
}

PlatformFederatedCredential::~PlatformFederatedCredential()
{
}

} // namespace blink
