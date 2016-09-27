/*
 *  Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
 *  Copyright (C) 2008 Apple Inc. All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "config.h"
#include "modules/plugins/DOMMimeTypeArray.h"

#include "core/frame/LocalFrame.h"
#include "core/page/Page.h"
#include "platform/plugins/PluginData.h"
#include "wtf/Vector.h"
#include "wtf/text/AtomicString.h"

namespace blink {

DOMMimeTypeArray::DOMMimeTypeArray(LocalFrame* frame)
    : DOMWindowProperty(frame)
{
}

DEFINE_TRACE(DOMMimeTypeArray)
{
    DOMWindowProperty::trace(visitor);
}

unsigned DOMMimeTypeArray::length() const
{
    PluginData* data = getPluginData();
    if (!data)
        return 0;
    return data->mimes().size();
}

DOMMimeType* DOMMimeTypeArray::item(unsigned index)
{
    PluginData* data = getPluginData();
    if (!data)
        return nullptr;
    const Vector<MimeClassInfo>& mimes = data->mimes();
    if (index >= mimes.size())
        return nullptr;
    return DOMMimeType::create(data, m_frame, index);
}

DOMMimeType* DOMMimeTypeArray::namedItem(const AtomicString& propertyName)
{
    PluginData* data = getPluginData();
    if (!data)
        return nullptr;
    const Vector<MimeClassInfo>& mimes = data->mimes();
    for (unsigned i = 0; i < mimes.size(); ++i) {
        if (mimes[i].type == propertyName)
            return DOMMimeType::create(data, m_frame, i);
    }
    return nullptr;
}

PluginData* DOMMimeTypeArray::getPluginData() const
{
    if (!m_frame)
        return nullptr;
    Page* p = m_frame->page();
    if (!p)
        return nullptr;
    return p->pluginData();
}

} // namespace blink
