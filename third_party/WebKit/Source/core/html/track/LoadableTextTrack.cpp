/*
 * Copyright (C) 2011 Google Inc.  All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/html/track/LoadableTextTrack.h"

#include "core/dom/ElementTraversal.h"
#include "core/html/HTMLTrackElement.h"
#include "core/html/track/vtt/VTTRegionList.h"

namespace blink {

LoadableTextTrack::LoadableTextTrack(HTMLTrackElement* track)
    : TextTrack(emptyAtom, emptyAtom, emptyAtom, emptyAtom, TrackElement)
    , m_trackElement(track)
{
}

LoadableTextTrack::~LoadableTextTrack()
{
#if !ENABLE(OILPAN)
    ASSERT(!m_trackElement);
#endif
}

#if !ENABLE(OILPAN)
void LoadableTextTrack::clearTrackElement()
{
    m_trackElement = nullptr;
}
#endif

bool LoadableTextTrack::isDefault() const
{
    ASSERT(m_trackElement);
    return m_trackElement->fastHasAttribute(HTMLNames::defaultAttr);
}

void LoadableTextTrack::setMode(const AtomicString& mode)
{
    TextTrack::setMode(mode);
#if !ENABLE(OILPAN)
    if (!m_trackElement)
        return;
#endif

    if (m_trackElement->readyState() == HTMLTrackElement::NONE)
        m_trackElement->scheduleLoad();
}

void LoadableTextTrack::addRegions(const WillBeHeapVector<RefPtrWillBeMember<VTTRegion>>& newRegions)
{
    for (size_t i = 0; i < newRegions.size(); ++i) {
        newRegions[i]->setTrack(this);
        regions()->add(newRegions[i]);
    }
}

size_t LoadableTextTrack::trackElementIndex()
{
    ASSERT(m_trackElement);
    ASSERT(m_trackElement->parentNode());

    size_t index = 0;
    for (HTMLTrackElement* track = Traversal<HTMLTrackElement>::firstChild(*m_trackElement->parentNode()); track; track = Traversal<HTMLTrackElement>::nextSibling(*track)) {
        if (!track->parentNode())
            continue;
        if (track == m_trackElement)
            return index;
        ++index;
    }
    ASSERT_NOT_REACHED();

    return 0;
}

DEFINE_TRACE(LoadableTextTrack)
{
    visitor->trace(m_trackElement);
    TextTrack::trace(visitor);
}

} // namespace blink
