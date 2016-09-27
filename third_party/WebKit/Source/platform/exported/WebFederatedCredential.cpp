// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "public/platform/WebFederatedCredential.h"

#include "platform/credentialmanager/PlatformFederatedCredential.h"

namespace blink {
WebFederatedCredential::WebFederatedCredential(const WebString& id, const WebURL& provider, const WebString& name, const WebURL& iconURL)
    : WebCredential(PlatformFederatedCredential::create(id, provider, name, iconURL))
{
}

void WebFederatedCredential::assign(const WebFederatedCredential& other)
{
    m_platformCredential = other.m_platformCredential;
}

WebURL WebFederatedCredential::provider() const
{
    return static_cast<PlatformFederatedCredential*>(m_platformCredential.get())->provider();
}

WebFederatedCredential::WebFederatedCredential(PlatformCredential* credential)
    : WebCredential(credential)
{
}

WebFederatedCredential& WebFederatedCredential::operator=(PlatformCredential* credential)
{
    m_platformCredential = credential;
    return *this;
}

} // namespace blink

