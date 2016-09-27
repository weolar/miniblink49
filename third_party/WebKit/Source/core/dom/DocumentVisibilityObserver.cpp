// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/dom/DocumentVisibilityObserver.h"

#include "core/dom/Document.h"

namespace blink {

DocumentVisibilityObserver::DocumentVisibilityObserver(Document& document)
    : m_document(nullptr)
{
    registerObserver(document);
}

DocumentVisibilityObserver::~DocumentVisibilityObserver()
{
#if !ENABLE(OILPAN)
    unregisterObserver();
#endif
}

DEFINE_TRACE(DocumentVisibilityObserver)
{
    visitor->trace(m_document);
}

void DocumentVisibilityObserver::unregisterObserver()
{
    if (m_document) {
        m_document->unregisterVisibilityObserver(this);
        m_document = nullptr;
    }
}

void DocumentVisibilityObserver::registerObserver(Document& document)
{
    ASSERT(!m_document);
    m_document = &document;
    if (m_document)
        m_document->registerVisibilityObserver(this);
}

void DocumentVisibilityObserver::setObservedDocument(Document& document)
{
    unregisterObserver();
    registerObserver(document);
}

} // namespace blink
