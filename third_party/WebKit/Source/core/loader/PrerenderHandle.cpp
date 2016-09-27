/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/loader/PrerenderHandle.h"

#include "core/dom/Document.h"
#include "core/frame/LocalFrame.h"
#include "core/loader/FrameLoader.h"
#include "core/loader/PrerendererClient.h"
#include "platform/Prerender.h"
#include "platform/weborigin/ReferrerPolicy.h"
#include "platform/weborigin/SecurityPolicy.h"

namespace blink {

// static
PassOwnPtrWillBeRawPtr<PrerenderHandle> PrerenderHandle::create(Document& document, PrerenderClient* client, const KURL& url, const unsigned prerenderRelTypes)
{
    // Prerenders are unlike requests in most ways (for instance, they pass down fragments, and they don't return data),
    // but they do have referrers.
    if (!document.frame())
        return nullptr;

    RefPtr<Prerender> prerender = Prerender::create(client, url, prerenderRelTypes, SecurityPolicy::generateReferrer(document.referrerPolicy(), url, document.outgoingReferrer()));

    PrerendererClient* prerendererClient = PrerendererClient::from(document.page());
    if (prerendererClient)
        prerendererClient->willAddPrerender(prerender.get());
    prerender->add();

    return adoptPtrWillBeNoop(new PrerenderHandle(document, prerender.release()));
}

PrerenderHandle::PrerenderHandle(Document& document, PassRefPtr<Prerender> prerender)
    : DocumentLifecycleObserver(&document)
    , m_prerender(prerender)
{
}

PrerenderHandle::~PrerenderHandle()
{
    if (m_prerender)
        detach();
}

void PrerenderHandle::cancel()
{
    // Avoid both abandoning and canceling the same prerender. In the abandon case, the LinkLoader cancels the
    // PrerenderHandle as the Document is destroyed, even through the DocumentLifecycleObserver has already abandoned
    // it.
    if (!m_prerender)
        return;
    m_prerender->cancel();
    detach();
}

const KURL& PrerenderHandle::url() const
{
    return m_prerender->url();
}

void PrerenderHandle::documentWasDetached()
{
#if ENABLE(OILPAN)
    // In Oilpan, a PrerenderHandle is not removed from
    // LifecycleNotifier::m_observers until a next GC is triggered.
    // Thus documentWasDetached() can be called for a PrerenderHandle
    // that is already canceled (and thus detached). In that case,
    // we should not detach the PrerenderHandle again, so we need this check.
    if (!m_prerender)
        return;
#else
    ASSERT(m_prerender);
#endif
    m_prerender->abandon();
    detach();
}

void PrerenderHandle::detach()
{
    m_prerender->removeClient();
    m_prerender.clear();
}

DEFINE_TRACE(PrerenderHandle)
{
    DocumentLifecycleObserver::trace(visitor);
}

}
