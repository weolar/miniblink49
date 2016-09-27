// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/credentialmanager/NavigatorCredentials.h"

#include "core/dom/Document.h"
#include "core/frame/LocalDOMWindow.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/Navigator.h"
#include "modules/credentialmanager/CredentialsContainer.h"

namespace blink {

NavigatorCredentials::NavigatorCredentials(Navigator& navigator)
    : DOMWindowProperty(navigator.frame())
{
}

NavigatorCredentials::~NavigatorCredentials()
{
}

NavigatorCredentials& NavigatorCredentials::from(Navigator& navigator)
{
    NavigatorCredentials* supplement = static_cast<NavigatorCredentials*>(HeapSupplement<Navigator>::from(navigator, supplementName()));
    if (!supplement) {
        supplement = new NavigatorCredentials(navigator);
        provideTo(navigator, supplementName(), supplement);
    }
    return *supplement;
}

const char* NavigatorCredentials::supplementName()
{
    return "NavigatorCredentials";
}

CredentialsContainer* NavigatorCredentials::credentials(Navigator& navigator)
{
    return NavigatorCredentials::from(navigator).credentials();
}

CredentialsContainer* NavigatorCredentials::credentials()
{
    if (!m_credentialsContainer && frame())
        m_credentialsContainer = CredentialsContainer::create();
    return m_credentialsContainer.get();
}

DEFINE_TRACE(NavigatorCredentials)
{
    visitor->trace(m_credentialsContainer);
    HeapSupplement<Navigator>::trace(visitor);
    DOMWindowProperty::trace(visitor);
}

} // namespace blink
