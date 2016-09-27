/*
    Copyright (C) 2000 Harri Porten (porten@kde.org)
    Copyright (C) 2000 Daniel Molkentin (molkentin@kde.org)
    Copyright (C) 2000 Stefan Schimanski (schimmi@kde.org)
    Copyright (C) 2003, 2004, 2005, 2006, 2007 Apple Inc. All Rights Reserved.
    Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#include "config.h"
#include "platform/plugins/PluginData.h"

#include "platform/plugins/PluginListBuilder.h"
#include "public/platform/Platform.h"

namespace blink {

class PluginCache {
public:
    PluginCache() : m_loaded(false), m_refresh(false) { }
    ~PluginCache() { reset(false); }

    void reset(bool refresh)
    {
        m_plugins.clear();
        m_loaded = false;
        m_refresh = refresh;
    }

    const Vector<PluginInfo>& plugins()
    {
        if (!m_loaded) {
            PluginListBuilder builder(&m_plugins);
            Platform::current()->getPluginList(m_refresh, &builder);
            m_loaded = true;
            m_refresh = false;
        }
        return m_plugins;
    }

private:
    Vector<PluginInfo> m_plugins;
    bool m_loaded;
    bool m_refresh;
};

static PluginCache& pluginCache()
{
    DEFINE_STATIC_LOCAL(PluginCache, cache, ());
    return cache;
}

PluginData::PluginData(const Page* page)
{
    initPlugins(page);

    for (unsigned i = 0; i < m_plugins.size(); ++i) {
        const PluginInfo& plugin = m_plugins[i];
        for (unsigned j = 0; j < plugin.mimes.size(); ++j) {
            m_mimes.append(plugin.mimes[j]);
            m_mimePluginIndices.append(i);
        }
    }
}

bool PluginData::supportsMimeType(const String& mimeType) const
{
    for (unsigned i = 0; i < m_mimes.size(); ++i)
        if (m_mimes[i].type == mimeType)
            return true;
    return false;
}

const PluginInfo* PluginData::pluginInfoForMimeType(const String& mimeType) const
{
    for (unsigned i = 0; i < m_mimes.size(); ++i) {
        const MimeClassInfo& info = m_mimes[i];

        if (info.type == mimeType)
            return &m_plugins[m_mimePluginIndices[i]];
    }

    return 0;
}

String PluginData::pluginNameForMimeType(const String& mimeType) const
{
    if (const PluginInfo* info = pluginInfoForMimeType(mimeType))
        return info->name;
    return String();
}

void PluginData::initPlugins(const Page*)
{
    const Vector<PluginInfo>& plugins = pluginCache().plugins();
    for (size_t i = 0; i < plugins.size(); ++i)
        m_plugins.append(plugins[i]);
}

void PluginData::refresh()
{
    pluginCache().reset(true);
    pluginCache().plugins(); // Force the plugins to be reloaded now.
}

String getPluginMimeTypeFromExtension(const String& extension)
{
    const Vector<PluginInfo>& plugins = pluginCache().plugins();
    for (size_t i = 0; i < plugins.size(); ++i) {
        for (size_t j = 0; j < plugins[i].mimes.size(); ++j) {
            const MimeClassInfo& mime = plugins[i].mimes[j];
            const Vector<String>& extensions = mime.extensions;
            for (size_t k = 0; k < extensions.size(); ++k) {
                if (extension == extensions[k])
                    return mime.type;
            }
        }
    }
    return String();
}

} // namespace blink
