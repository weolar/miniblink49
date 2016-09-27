/*
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

#ifndef DOMPlugin_h
#define DOMPlugin_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "core/frame/LocalFrameLifecycleObserver.h"
#include "modules/plugins/DOMMimeType.h"
#include "platform/heap/Handle.h"
#include "wtf/Forward.h"
#include "wtf/RefPtr.h"

namespace blink {

class PluginData;

class DOMPlugin final : public GarbageCollectedFinalized<DOMPlugin>, public ScriptWrappable, public LocalFrameLifecycleObserver {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(DOMPlugin);
    DEFINE_WRAPPERTYPEINFO();
public:
    static DOMPlugin* create(PluginData* pluginData, LocalFrame* frame, unsigned index)
    {
        return new DOMPlugin(pluginData, frame, index);
    }
    virtual ~DOMPlugin();

    String name() const;
    String filename() const;
    String description() const;

    unsigned length() const;

    DOMMimeType* item(unsigned index);
    DOMMimeType* namedItem(const AtomicString& propertyName);

    DECLARE_VIRTUAL_TRACE();

private:
    DOMPlugin(PluginData*, LocalFrame*, unsigned index);

    const PluginInfo& pluginInfo() const { return m_pluginData->plugins()[m_index]; }

    RefPtr<PluginData> m_pluginData;
    unsigned m_index;
};

} // namespace blink

#endif // DOMPlugin_h
