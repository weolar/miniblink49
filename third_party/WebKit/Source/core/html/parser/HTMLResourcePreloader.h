/*
 * Copyright (C) 2013 Google Inc. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef HTMLResourcePreloader_h
#define HTMLResourcePreloader_h

#include "core/fetch/FetchRequest.h"
#include "core/fetch/Resource.h"
#include "core/html/parser/PreloadRequest.h"
#include "core/html/parser/ResourcePreloader.h"
#include "wtf/CurrentTime.h"
#include "wtf/text/TextPosition.h"

namespace blink {

class HTMLResourcePreloader final : public NoBaseWillBeGarbageCollected<HTMLResourcePreloader>, public ResourcePreloader {
    WTF_MAKE_NONCOPYABLE(HTMLResourcePreloader); WTF_MAKE_FAST_ALLOCATED_WILL_BE_REMOVED(HTMLResourcePreloader);
public:
    static PassOwnPtrWillBeRawPtr<HTMLResourcePreloader> create(Document&);
    DECLARE_TRACE();

protected:
    void preload(PassOwnPtr<PreloadRequest>) override;

private:
    explicit HTMLResourcePreloader(Document&);

    RawPtrWillBeMember<Document> m_document;
};

}

#endif
