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

#ifndef PrerenderHandle_h
#define PrerenderHandle_h

#include "core/dom/DocumentLifecycleObserver.h"
#include "platform/heap/Handle.h"
#include "platform/weborigin/KURL.h"
#include "wtf/Noncopyable.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/PassRefPtr.h"

namespace blink {

class Document;
class Prerender;
class PrerenderClient;

class PrerenderHandle final : public NoBaseWillBeGarbageCollectedFinalized<PrerenderHandle>, public DocumentLifecycleObserver {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(PrerenderHandle);
    WTF_MAKE_NONCOPYABLE(PrerenderHandle);
public:
    static PassOwnPtrWillBeRawPtr<PrerenderHandle> create(Document&, PrerenderClient*, const KURL&, unsigned prerenderRelTypes);

    virtual ~PrerenderHandle();

    void cancel();
    const KURL& url() const;

    // From DocumentLifecycleObserver:
    void documentWasDetached() override;

    DECLARE_VIRTUAL_TRACE();

private:
    PrerenderHandle(Document&, PassRefPtr<Prerender>);

    void detach();

    RefPtr<Prerender> m_prerender;
};

}

#endif // PrerenderHandle_h
