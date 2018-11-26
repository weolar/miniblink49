/*
 * Copyright (C) 2008 Apple Inc. All rights reserved.
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

#include "config.h"
#include "platform/mhtml/ArchiveResourceCollection.h"

#include "platform/mhtml/ArchiveResource.h"
#include "platform/mhtml/MHTMLArchive.h"
#include "platform/weborigin/KURL.h"
#include "wtf/text/StringBuilder.h"
#include "wtf/text/WTFString.h"

namespace blink {

namespace {

// Converts |contentID| taken from a Content-ID MIME header
// into URI using "cid" scheme.  Based primarily on an example
// from rfc2557 in section 9.5, but also based on more
// normative parts of specs like:
// - rfc2557 - MHTML - section 8.3 - "Use of the Content-ID header and CID URLs"
// - rfc1738 - URL - section 4 (reserved scheme names;  includes "cid")
// - rfc2387 - multipart/related - section 3.4 - "Syntax" (cid := msg-id)
// - rfc0822 - msg-id = "<" addr-spec ">"; addr-spec = local-part "@" domain
KURL convertContentIDToURI(const String& contentID)
{
    if (contentID.length() <= 2)
        return KURL();

    if (!contentID.startsWith('<') || !contentID.endsWith('>'))
        return KURL();

    StringBuilder uriBuilder;
    uriBuilder.append("cid:");
    uriBuilder.append(contentID, 1, contentID.length() - 2);
    return KURL(KURL(), uriBuilder.toString());
}

}

ArchiveResourceCollection::ArchiveResourceCollection()
{
}

ArchiveResourceCollection::~ArchiveResourceCollection()
{
}

void ArchiveResourceCollection::addAllResources(MHTMLArchive* archive)
{
    ASSERT(archive);
    if (!archive)
        return;

    const MHTMLArchive::SubArchiveResources& subresources = archive->subresources();
    for (const RefPtrWillBeMember<ArchiveResource>& subresource : subresources) {
        addResource(*subresource);
    }

    const MHTMLArchive::SubFrameArchives& subframes = archive->subframeArchives();
    for (const RefPtrWillBeMember<MHTMLArchive>& archive : subframes) {
        ASSERT(archive->mainResource());

        const String& frameName = archive->mainResource()->frameName();
        if (!frameName.isNull()) {
            m_subframes.set(frameName, archive.get());
        } else {
            // In the MHTML case, frames don't have a name so we use the URL instead.
            m_subframes.set(archive->mainResource()->url().string(), archive.get());
        }

        KURL cidURI = convertContentIDToURI(archive->mainResource()->contentID());
        if (cidURI.isValid())
            m_subframes.set(cidURI, archive.get());
    }
}

// FIXME: Adding a resource directly to a DocumentLoader/ArchiveResourceCollection seems like bad design, but is API some apps rely on.
// Can we change the design in a manner that will let us deprecate that API without reducing functionality of those apps?
void ArchiveResourceCollection::addResource(ArchiveResource& resource)
{
    const KURL& url = resource.url();
    m_subresources.set(url, &resource);

    KURL cidURI = convertContentIDToURI(resource.contentID());
    if (cidURI.isValid())
        m_subresources.set(cidURI, &resource);
}

ArchiveResource* ArchiveResourceCollection::archiveResourceForURL(const KURL& url)
{
    ArchiveResource* resource = m_subresources.get(url);
    if (!resource)
        return 0;

    return resource;
}

PassRefPtrWillBeRawPtr<MHTMLArchive> ArchiveResourceCollection::popSubframeArchive(const String& frameName, const KURL& url)
{
    RefPtrWillBeRawPtr<MHTMLArchive> archive = m_subframes.take(frameName);
    if (archive)
        return archive.release();

    return m_subframes.take(url.string());
}

DEFINE_TRACE(ArchiveResourceCollection)
{
#if ENABLE(OILPAN)
    visitor->trace(m_subresources);
    visitor->trace(m_subframes);
#endif
}

}
