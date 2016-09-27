// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/storage/DOMWindowStorageController.h"

#include "core/dom/Document.h"
#include "core/events/Event.h"
#include "core/frame/LocalDOMWindow.h"
#include "core/page/Page.h"
#include "modules/storage/DOMWindowStorage.h"

namespace blink {

DOMWindowStorageController::DOMWindowStorageController(Document& document)
    : DOMWindowLifecycleObserver(document.domWindow())
    , m_document(document)
{
}

DEFINE_EMPTY_DESTRUCTOR_WILL_BE_REMOVED(DOMWindowStorageController);

DEFINE_TRACE(DOMWindowStorageController)
{
    visitor->trace(m_document);
    WillBeHeapSupplement<Document>::trace(visitor);
    DOMWindowLifecycleObserver::trace(visitor);
}

// static
const char* DOMWindowStorageController::supplementName()
{
    return "DOMWindowStorageController";
}

// static
DOMWindowStorageController& DOMWindowStorageController::from(Document& document)
{
    DOMWindowStorageController* controller = static_cast<DOMWindowStorageController*>(WillBeHeapSupplement<Document>::from(document, supplementName()));
    if (!controller) {
        controller = new DOMWindowStorageController(document);
        WillBeHeapSupplement<Document>::provideTo(document, supplementName(), adoptPtrWillBeNoop(controller));
    }
    return *controller;
}

void DOMWindowStorageController::didAddEventListener(LocalDOMWindow* window, const AtomicString& eventType)
{
    if (eventType == EventTypeNames::storage) {
        // Creating these blink::Storage objects informs the system that we'd like to receive
        // notifications about storage events that might be triggered in other processes. Rather
        // than subscribe to these notifications explicitly, we subscribe to them implicitly to
        // simplify the work done by the system.
        DOMWindowStorage::from(*window).localStorage(IGNORE_EXCEPTION);
        DOMWindowStorage::from(*window).sessionStorage(IGNORE_EXCEPTION);
    }
}

} // namespace blink
