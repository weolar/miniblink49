// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "public/platform/WebPasswordCredential.h"

#include "platform/credentialmanager/PlatformPasswordCredential.h"

namespace blink {
WebPasswordCredential::WebPasswordCredential(const WebString& id, const WebString& password, const WebString& name, const WebURL& iconURL)
    : WebCredential(PlatformPasswordCredential::create(id, password, name, iconURL))
{
}

void WebPasswordCredential::assign(const WebPasswordCredential& other)
{
    m_platformCredential = other.m_platformCredential;
}

WebString WebPasswordCredential::password() const
{
    return static_cast<PlatformPasswordCredential*>(m_platformCredential.get())->password();
}

WebPasswordCredential::WebPasswordCredential(PlatformCredential* credential)
    : WebCredential(credential)
{
}

WebPasswordCredential& WebPasswordCredential::operator=(PlatformCredential* credential)
{
    m_platformCredential = credential;
    return *this;
}

} // namespace blink


