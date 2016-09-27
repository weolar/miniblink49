/*
 * Copyright (C) 2006, 2007, 2009 Apple Inc. All rights reserved.
 * Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "config.h"
#include "core/page/ScopedPageLoadDeferrer.h"

#include "core/dom/Document.h"
#include "core/frame/LocalFrame.h"
#include "core/loader/FrameLoader.h"
#include "core/page/Page.h"
#include "public/platform/Platform.h"
#include "public/platform/WebScheduler.h"
#include "wtf/HashSet.h"

namespace blink {

ScopedPageLoadDeferrer::ScopedPageLoadDeferrer(Page* exclusion) : m_detached(false)
{
    const HashSet<Page*>& pages = Page::ordinaryPages();
    for (const Page* page : pages) {
        if (page == exclusion || page->defersLoading())
            continue;

        if (page->mainFrame()->isLocalFrame()) {
            m_deferredFrames.append(page->deprecatedLocalMainFrame());

            // Ensure that we notify the client if the initial empty document is accessed before
            // showing anything modal, to prevent spoofs while the modal window or sheet is visible.
            page->deprecatedLocalMainFrame()->loader().notifyIfInitialDocumentAccessed();
        }
    }

    size_t count = m_deferredFrames.size();
    for (size_t i = 0; i < count; ++i) {
        if (Page* page = m_deferredFrames[i]->page())
            page->setDefersLoading(true);
    }
    Platform::current()->currentThread()->scheduler()->suspendTimerQueue();
}

void ScopedPageLoadDeferrer::detach()
{
    if (m_detached)
        return;

    for (size_t i = 0; i < m_deferredFrames.size(); ++i) {
        if (Page* page = m_deferredFrames[i]->page())
            page->setDefersLoading(false);
    }

    Platform::current()->currentThread()->scheduler()->resumeTimerQueue();
    m_detached = true;
}

#if ENABLE(OILPAN)
void ScopedPageLoadDeferrer::dispose()
{
    detach();
    m_deferredFrames.clear();
}
#endif

ScopedPageLoadDeferrer::~ScopedPageLoadDeferrer()
{
    detach();
}

DEFINE_TRACE(ScopedPageLoadDeferrer)
{
#if ENABLE(OILPAN)
    visitor->trace(m_deferredFrames);
#endif
}

} // namespace blink
