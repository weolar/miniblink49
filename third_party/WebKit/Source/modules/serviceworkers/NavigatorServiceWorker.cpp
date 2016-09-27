// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/serviceworkers/NavigatorServiceWorker.h"

#include "core/dom/Document.h"
#include "core/frame/LocalDOMWindow.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/Navigator.h"
#include "modules/serviceworkers/ServiceWorkerContainer.h"

namespace blink {

NavigatorServiceWorker::NavigatorServiceWorker(Navigator& navigator)
    : DOMWindowProperty(navigator.frame())
{
}

NavigatorServiceWorker::~NavigatorServiceWorker()
{
}

NavigatorServiceWorker* NavigatorServiceWorker::from(Document& document)
{
    if (!document.frame() || !document.frame()->domWindow())
        return nullptr;
    Navigator& navigator = *document.frame()->domWindow()->navigator();
    return &from(navigator);
}

NavigatorServiceWorker& NavigatorServiceWorker::from(Navigator& navigator)
{
    NavigatorServiceWorker* supplement = toNavigatorServiceWorker(navigator);
    if (!supplement) {
        supplement = new NavigatorServiceWorker(navigator);
        provideTo(navigator, supplementName(), supplement);
        if (navigator.frame() && navigator.frame()->securityContext()->securityOrigin()->canAccessServiceWorkers()) {
            // Initialize ServiceWorkerContainer too.
            supplement->serviceWorker(ASSERT_NO_EXCEPTION);
        }
    }
    return *supplement;
}

NavigatorServiceWorker* NavigatorServiceWorker::toNavigatorServiceWorker(Navigator& navigator)
{
    return static_cast<NavigatorServiceWorker*>(HeapSupplement<Navigator>::from(navigator, supplementName()));
}

const char* NavigatorServiceWorker::supplementName()
{
    return "NavigatorServiceWorker";
}

ServiceWorkerContainer* NavigatorServiceWorker::serviceWorker(Navigator& navigator, ExceptionState& exceptionState)
{
    return NavigatorServiceWorker::from(navigator).serviceWorker(exceptionState);
}

ServiceWorkerContainer* NavigatorServiceWorker::serviceWorker(ExceptionState& exceptionState)
{
    if (frame() && !frame()->securityContext()->securityOrigin()->canAccessServiceWorkers()) {
        if (frame()->securityContext()->isSandboxed(SandboxOrigin))
            exceptionState.throwSecurityError("Service worker is disabled because the context is sandboxed and lacks the 'allow-same-origin' flag.");
        else
            exceptionState.throwSecurityError("Access to service workers is denied in this document origin.");
        return nullptr;
    }
    if (!m_serviceWorker && frame()) {
        ASSERT(frame()->domWindow());
        m_serviceWorker = ServiceWorkerContainer::create(frame()->domWindow()->executionContext());
    }
    return m_serviceWorker.get();
}

void NavigatorServiceWorker::willDetachGlobalObjectFromFrame()
{
    if (m_serviceWorker) {
        m_serviceWorker->willBeDetachedFromFrame();
        m_serviceWorker = nullptr;
    }
}

DEFINE_TRACE(NavigatorServiceWorker)
{
    visitor->trace(m_serviceWorker);
    HeapSupplement<Navigator>::trace(visitor);
    DOMWindowProperty::trace(visitor);
}

} // namespace blink
