/*
 * Copyright (C) 2008, 2010 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef ArchiveResource_h
#define ArchiveResource_h

#include "platform/SharedBuffer.h"
#include "platform/heap/Handle.h"
#include "platform/network/ResourceResponse.h"
#include "platform/weborigin/KURL.h"
#include "wtf/RefCounted.h"
#include "wtf/RefPtr.h"

namespace blink {

class PLATFORM_EXPORT ArchiveResource final : public RefCountedWillBeGarbageCollectedFinalized<ArchiveResource> {
public:
    static PassRefPtrWillBeRawPtr<ArchiveResource> create(
        PassRefPtr<SharedBuffer>,
        const KURL&,
        const String& contentId,
        const AtomicString& mimeType,
        const AtomicString& textEncoding,
        const String& frameName,
        const ResourceResponse& = ResourceResponse());

    ~ArchiveResource();

    const KURL& url() const { return m_url; }
    const String& contentID() const { return m_contentID; }
    const ResourceResponse& response() const { return m_response; }
    SharedBuffer* data() const { return m_data.get(); }
    const AtomicString& mimeType() const { return m_mimeType; }
    const AtomicString& textEncoding() const { return m_textEncoding; }
    const String& frameName() const { return m_frameName; }

    DEFINE_INLINE_TRACE() { }
    
private:
    ArchiveResource(
        PassRefPtr<SharedBuffer>,
        const KURL&,
        const String& contentID,
        const AtomicString& mimeType,
        const AtomicString& textEncoding,
        const String& frameName,
        const ResourceResponse&);

    KURL m_url;
    String m_contentID;
    ResourceResponse m_response;
    RefPtr<SharedBuffer> m_data;
    AtomicString m_mimeType;
    AtomicString m_textEncoding;
    String m_frameName;
};

}

#endif // ArchiveResource_h
