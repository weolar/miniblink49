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
#include "modules/accessibility/AXTableHeaderContainer.h"

#include "modules/accessibility/AXObjectCacheImpl.h"

namespace blink {

AXTableHeaderContainer::AXTableHeaderContainer(AXObjectCacheImpl& axObjectCache)
    : AXMockObject(axObjectCache)
{
}

AXTableHeaderContainer::~AXTableHeaderContainer()
{
}

PassRefPtrWillBeRawPtr<AXTableHeaderContainer> AXTableHeaderContainer::create(AXObjectCacheImpl& axObjectCache)
{
    return adoptRefWillBeNoop(new AXTableHeaderContainer(axObjectCache));
}

LayoutRect AXTableHeaderContainer::elementRect() const
{
    // this will be filled in when addChildren is called
    return m_headerRect;
}

bool AXTableHeaderContainer::computeAccessibilityIsIgnored(IgnoredReasons* ignoredReasons) const
{
    if (!m_parent)
        return true;

    if (!m_parent->accessibilityIsIgnored())
        return false;

    if (ignoredReasons)
        m_parent->computeAccessibilityIsIgnored(ignoredReasons);

    return true;
}

void AXTableHeaderContainer::addChildren()
{
    ASSERT(!m_haveChildren);

    m_haveChildren = true;
    if (!m_parent || !m_parent->isAXTable())
        return;

    toAXTable(m_parent)->columnHeaders(m_children);

    unsigned length = m_children.size();
    for (unsigned k = 0; k < length; ++k)
        m_headerRect.unite(m_children[k]->elementRect());
}

} // namespace blink
