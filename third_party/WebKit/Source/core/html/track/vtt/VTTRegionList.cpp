/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
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
#include "core/html/track/vtt/VTTRegionList.h"

namespace blink {

VTTRegionList::VTTRegionList()
{
}

unsigned long VTTRegionList::length() const
{
    return m_list.size();
}

VTTRegion* VTTRegionList::item(unsigned index) const
{
    if (index < m_list.size())
        return m_list[index].get();

    return nullptr;
}

VTTRegion* VTTRegionList::getRegionById(const String& id) const
{
    if (id.isEmpty())
        return nullptr;

    for (size_t i = 0; i < m_list.size(); ++i) {
        if (m_list[i]->id() == id)
            return m_list[i].get();
    }

    return nullptr;
}

void VTTRegionList::add(PassRefPtrWillBeRawPtr<VTTRegion> region)
{
    m_list.append(region);
}

bool VTTRegionList::remove(VTTRegion* region)
{
    size_t index = m_list.find(region);
    if (index == kNotFound)
        return false;

    m_list.remove(index);
    return true;
}

DEFINE_TRACE(VTTRegionList)
{
    visitor->trace(m_list);
}

} // namespace blink
