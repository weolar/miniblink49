// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/dom/IncrementLoadEventDelayCount.h"

#include "core/dom/Document.h"

#ifndef NDEBUG
#include <set>
std::set<void*>* g_activatingIncrementLoadEventDelayCount = nullptr;
#endif

namespace blink {

PassOwnPtr<IncrementLoadEventDelayCount> IncrementLoadEventDelayCount::create(Document& document, void* objectForDebug)
{
    return adoptPtr(new IncrementLoadEventDelayCount(document, objectForDebug));
}

IncrementLoadEventDelayCount::IncrementLoadEventDelayCount(Document& document, void* objectForDebug)
    : m_document(&document)
{
    document.incrementLoadEventDelayCount();
#ifndef NDEBUG
    if (!g_activatingIncrementLoadEventDelayCount)
        g_activatingIncrementLoadEventDelayCount = new std::set<void*>();
    g_activatingIncrementLoadEventDelayCount->insert(this);
#endif
    m_objectForDebug = objectForDebug;
}

IncrementLoadEventDelayCount::~IncrementLoadEventDelayCount()
{
    m_document->decrementLoadEventDelayCount();
#ifndef NDEBUG
    g_activatingIncrementLoadEventDelayCount->erase(this);
#endif
}

void IncrementLoadEventDelayCount::documentChanged(Document& newDocument)
{
    newDocument.incrementLoadEventDelayCount();
    m_document->decrementLoadEventDelayCount();
    m_document = &newDocument;
}
}
