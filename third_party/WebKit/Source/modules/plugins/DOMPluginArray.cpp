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
#include "modules/plugins/DOMPluginArray.h"

#include "core/frame/LocalFrame.h"
#include "core/page/Page.h"
#include "platform/plugins/PluginData.h"
#include "wtf/Vector.h"
#include "wtf/text/AtomicString.h"

namespace blink {

DOMPluginArray::DOMPluginArray(LocalFrame* frame)
    : DOMWindowProperty(frame)
{
}

DEFINE_TRACE(DOMPluginArray)
{
    DOMWindowProperty::trace(visitor);
}

unsigned DOMPluginArray::length() const
{
    PluginData* data = pluginData();
    if (!data)
        return 0;
    return data->plugins().size();
}

DOMPlugin* DOMPluginArray::item(unsigned index)
{
    PluginData* data = pluginData();
    if (!data)
        return nullptr;
    const Vector<PluginInfo>& plugins = data->plugins();
    if (index >= plugins.size())
        return nullptr;
    return DOMPlugin::create(data, m_frame, index);
}

DOMPlugin* DOMPluginArray::namedItem(const AtomicString& propertyName)
{
    PluginData* data = pluginData();
    if (!data)
        return nullptr;
    const Vector<PluginInfo>& plugins = data->plugins();
    for (unsigned i = 0; i < plugins.size(); ++i) {
        if (plugins[i].name == propertyName)
            return DOMPlugin::create(data, m_frame, i);
    }
    return nullptr;
}

void DOMPluginArray::refresh(bool reload)
{
    if (!m_frame)
        return;
    Page::refreshPlugins();
    if (reload)
        m_frame->reload(FrameLoadTypeReload, ClientRedirect);
}

PluginData* DOMPluginArray::pluginData() const
{
    if (!m_frame)
        return nullptr;
    Page* page = m_frame->page();
    if (!page)
        return nullptr;
    return page->pluginData();
}

} // namespace blink
