/*
 * Copyright (C) 2011, Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY GOOGLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */

#include "config.h"
#include "modules/indexeddb/DOMWindowIndexedDatabase.h"

#include "core/dom/Document.h"
#include "core/frame/LocalDOMWindow.h"
#include "core/page/Page.h"
#include "modules/indexeddb/IDBFactory.h"

namespace blink {

DOMWindowIndexedDatabase::DOMWindowIndexedDatabase(LocalDOMWindow& window)
    : DOMWindowProperty(window.frame())
    , m_window(window)
{
}

DEFINE_EMPTY_DESTRUCTOR_WILL_BE_REMOVED(DOMWindowIndexedDatabase);

DEFINE_TRACE(DOMWindowIndexedDatabase)
{
    visitor->trace(m_idbFactory);
    WillBeHeapSupplement<LocalDOMWindow>::trace(visitor);
    DOMWindowProperty::trace(visitor);
}

const char* DOMWindowIndexedDatabase::supplementName()
{
    return "DOMWindowIndexedDatabase";
}

DOMWindowIndexedDatabase& DOMWindowIndexedDatabase::from(LocalDOMWindow& window)
{
    DOMWindowIndexedDatabase* supplement = static_cast<DOMWindowIndexedDatabase*>(WillBeHeapSupplement<LocalDOMWindow>::from(window, supplementName()));
    if (!supplement) {
        supplement = new DOMWindowIndexedDatabase(window);
        provideTo(window, supplementName(), adoptPtrWillBeNoop(supplement));
    }
    return *supplement;
}

void DOMWindowIndexedDatabase::willDestroyGlobalObjectInFrame()
{
    m_idbFactory = nullptr;
    DOMWindowProperty::willDestroyGlobalObjectInFrame();
}

void DOMWindowIndexedDatabase::willDetachGlobalObjectFromFrame()
{
    m_idbFactory = nullptr;
    DOMWindowProperty::willDetachGlobalObjectFromFrame();
}

IDBFactory* DOMWindowIndexedDatabase::indexedDB(DOMWindow& window)
{
    return from(toLocalDOMWindow(window)).indexedDB();
}

IDBFactory* DOMWindowIndexedDatabase::indexedDB()
{
    Document* document = m_window.document();
    if (!document)
        return nullptr;

    Page* page = document->page();
    if (!page)
        return nullptr;

    if (!m_window.isCurrentlyDisplayedInFrame())
        return nullptr;

    if (!m_idbFactory)
        m_idbFactory = IDBFactory::create(IndexedDBClient::create());
    return m_idbFactory.get();
}

} // namespace blink
