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
#include "modules/accessibility/AXList.h"

#include "core/html/HTMLUListElement.h"
#include "core/layout/LayoutObject.h"
#include "modules/accessibility/AXObjectCacheImpl.h"

namespace blink {

using namespace HTMLNames;

AXList::AXList(LayoutObject* layoutObject, AXObjectCacheImpl& axObjectCache)
    : AXLayoutObject(layoutObject, axObjectCache)
{
}

AXList::~AXList()
{
}

PassRefPtrWillBeRawPtr<AXList> AXList::create(LayoutObject* layoutObject, AXObjectCacheImpl& axObjectCache)
{
    return adoptRefWillBeNoop(new AXList(layoutObject, axObjectCache));
}

bool AXList::computeAccessibilityIsIgnored(IgnoredReasons* ignoredReasons) const
{
    return accessibilityIsIgnoredByDefault(ignoredReasons);
}

bool AXList::isDescriptionList() const
{
    if (!m_layoutObject)
        return false;

    Node* node = m_layoutObject->node();
    return node && node->hasTagName(dlTag);
}

AccessibilityRole AXList::roleValue() const
{
    if (isDescriptionList())
        return DescriptionListRole;

    return ListRole;
}
} // namespace blink
