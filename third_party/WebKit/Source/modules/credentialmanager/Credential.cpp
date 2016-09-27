// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/credentialmanager/Credential.h"

#include "bindings/core/v8/ExceptionState.h"
#include "core/dom/ExceptionCode.h"

namespace blink {

Credential* Credential::create(const String& id, const String& name, const KURL& icon)
{
    return new Credential(id, name, icon);
}

Credential* Credential::create(const String& id, const String& name, const String& icon, ExceptionState& exceptionState)
{
    KURL iconURL = parseStringAsURL(icon, exceptionState);
    if (exceptionState.hadException())
        return nullptr;
    return new Credential(id, name, iconURL);
}

Credential::Credential(PlatformCredential* credential)
    : m_platformCredential(credential)
{
}

Credential::Credential(const String& id, const String& name, const KURL& icon)
    : m_platformCredential(PlatformCredential::create(id, name, icon))
{
}

KURL Credential::parseStringAsURL(const String& url, ExceptionState& exceptionState)
{
    if (url.isEmpty())
        return KURL();
    KURL parsedURL = KURL(KURL(), url);
    if (!parsedURL.isValid())
        exceptionState.throwDOMException(SyntaxError, "'" + url + "' is not a valid URL.");
    return parsedURL;
}

DEFINE_TRACE(Credential)
{
    visitor->trace(m_platformCredential);
}

} // namespace blink
