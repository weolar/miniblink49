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

namespace blink {

ArchiveResourceCollection::ArchiveResourceCollection()
{
}

ArchiveResourceCollection::~ArchiveResourceCollection()
{
}

#ifdef MINIBLINK_NOT_MHTML
ArchiveResource::~ArchiveResource() {}
#endif

void ArchiveResourceCollection::addAllResources(MHTMLArchive* archive)
{
#ifndef MINIBLINK_NOT_MHTML
    ASSERT(archive);
    if (!archive)
        return;

    const MHTMLArchive::SubArchiveResources& subresources = archive->subresources();
    for (MHTMLArchive::SubArchiveResources::const_iterator iterator = subresources.begin(); iterator != subresources.end(); ++iterator)
        m_subresources.set((*iterator)->url(), iterator->get());

    const MHTMLArchive::SubFrameArchives& subframes = archive->subframeArchives();
    for (MHTMLArchive::SubFrameArchives::const_iterator iterator = subframes.begin(); iterator != subframes.end(); ++iterator) {
        RefPtrWillBeRawPtr<MHTMLArchive> archive = *iterator;
        ASSERT(archive->mainResource());

        const String& frameName = archive->mainResource()->frameName();
        if (!frameName.isNull()) {
            m_subframes.set(frameName, archive.get());
        } else {
            // In the MHTML case, frames don't have a name so we use the URL instead.
            String url = archive->mainResource()->url().string();
            if (url.isNull())
                url = "";
            m_subframes.set(url, archive.get());
        }
    }
#endif // MINIBLINK_NOT_MHTML
}

// FIXME: Adding a resource directly to a DocumentLoader/ArchiveResourceCollection seems like bad design, but is API some apps rely on.
// Can we change the design in a manner that will let us deprecate that API without reducing functionality of those apps?
void ArchiveResourceCollection::addResource(PassRefPtrWillBeRawPtr<ArchiveResource> resource)
{
    ASSERT(resource);
    if (!resource)
        return;

    const KURL& url = resource->url(); // get before passing PassRefPtr (which sets it to 0)
    m_subresources.set(url, resource);
}

ArchiveResource* ArchiveResourceCollection::archiveResourceForURL(const KURL& url)
{
    ArchiveResource* resource = m_subresources.get(url);
    if (!resource)
        return 0;

    return resource;
}

#ifndef MINIBLINK_NOT_MHTML
PassRefPtrWillBeRawPtr<MHTMLArchive> ArchiveResourceCollection::popSubframeArchive(const String& frameName, const KURL& url)
{

    RefPtrWillBeRawPtr<MHTMLArchive> archive = m_subframes.take(frameName);
    if (archive)
        return archive.release();

    return m_subframes.take(url.string());
}
#endif // MINIBLINK_NOT_MHTML


DEFINE_TRACE(ArchiveResourceCollection)
{
#if ENABLE(OILPAN)
    visitor->trace(m_subresources);
#ifndef MINIBLINK_NOT_MHTML
    visitor->trace(m_subframes);
#endif // MINIBLINK_NOT_MHTML
#endif
}

}
