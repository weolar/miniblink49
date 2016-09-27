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
#include "core/html/track/TextTrackCueList.h"

#include "wtf/StdLibExtras.h"

namespace blink {

TextTrackCueList::TextTrackCueList()
    : m_firstInvalidIndex(0)
{
}

DEFINE_EMPTY_DESTRUCTOR_WILL_BE_REMOVED(TextTrackCueList);

unsigned long TextTrackCueList::length() const
{
    return m_list.size();
}

TextTrackCue* TextTrackCueList::item(unsigned index) const
{
    if (index < m_list.size())
        return m_list[index].get();
    return nullptr;
}

TextTrackCue* TextTrackCueList::getCueById(const AtomicString& id) const
{
    for (size_t i = 0; i < m_list.size(); ++i) {
        if (m_list[i]->id() == id)
            return m_list[i].get();
    }
    return nullptr;
}

void TextTrackCueList::collectActiveCues(TextTrackCueList& activeCues) const
{
    activeCues.clear();
    for (auto& cue : m_list) {
        if (cue->isActive())
            activeCues.add(cue);
    }
}

bool TextTrackCueList::add(PassRefPtrWillBeRawPtr<TextTrackCue> cue)
{
    ASSERT(cue->startTime() >= 0);
    ASSERT(cue->endTime() >= 0);

    // Maintain text track cue order:
    // https://html.spec.whatwg.org/#text-track-cue-order
    size_t index = findInsertionIndex(cue.get());

    // FIXME: The cue should not exist in the list in the first place.
    if (!m_list.isEmpty() && (index > 0) && (m_list[index - 1].get() == cue.get()))
        return false;

    m_list.insert(index, cue);
    invalidateCueIndex(index);
    return true;
}

static bool cueIsBefore(const TextTrackCue* cue, PassRefPtrWillBeRawPtr<TextTrackCue> otherCue)
{
    if (cue->startTime() < otherCue->startTime())
        return true;

    return cue->startTime() == otherCue->startTime() && cue->endTime() > otherCue->endTime();
}

size_t TextTrackCueList::findInsertionIndex(const TextTrackCue* cueToInsert) const
{
    auto it = std::upper_bound(m_list.begin(), m_list.end(), cueToInsert, cueIsBefore);
    size_t index = safeCast<size_t>(it - m_list.begin());
    ASSERT_WITH_SECURITY_IMPLICATION(index <= m_list.size());
    return index;
}

bool TextTrackCueList::remove(TextTrackCue* cue)
{
    size_t index = m_list.find(cue);
    if (index == kNotFound)
        return false;

    m_list.remove(index);
    invalidateCueIndex(index);
    cue->invalidateCueIndex();
    return true;
}

void TextTrackCueList::updateCueIndex(TextTrackCue* cue)
{
    if (!remove(cue))
        return;
    add(cue);
}

void TextTrackCueList::clear()
{
    m_list.clear();
}

void TextTrackCueList::invalidateCueIndex(size_t index)
{
    // Store the smallest (first) index that we know has a cue that does not
    // meet the criteria:
    //   cueIndex(list[index-1]) + 1 == cueIndex(list[index]) [index > 0]
    // This is a stronger requirement than we need, but it's easier to maintain.
    // We can then check if a cue's index is valid by comparing it with
    // |m_firstInvalidIndex| - if it's strictly less it is valid.
    m_firstInvalidIndex = std::min(m_firstInvalidIndex, index);
}

void TextTrackCueList::validateCueIndexes()
{
    // Compute new index values for the cues starting at
    // |m_firstInvalidIndex|. If said index is beyond the end of the list, no
    // cues will need to be updated.
    for (size_t i = m_firstInvalidIndex; i < m_list.size(); ++i)
        m_list[i]->updateCueIndex(safeCast<unsigned>(i));
    m_firstInvalidIndex = m_list.size();
}

DEFINE_TRACE(TextTrackCueList)
{
    visitor->trace(m_list);
}

} // namespace blink
