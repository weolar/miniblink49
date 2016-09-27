/*
    Copyright (C) 2008 Nokia Corporation and/or its subsidiary(-ies)
    Copyright (C) 2008 Apple Inc. All rights reserved.

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

#ifndef DOMPluginArray_h
#define DOMPluginArray_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "core/frame/DOMWindowProperty.h"
#include "modules/plugins/DOMPlugin.h"
#include "platform/heap/Handle.h"
#include "wtf/Forward.h"

namespace blink {

class LocalFrame;
class PluginData;

class DOMPluginArray final : public GarbageCollectedFinalized<DOMPluginArray>, public ScriptWrappable, public DOMWindowProperty {
    DEFINE_WRAPPERTYPEINFO();
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(DOMPluginArray);
public:
    static DOMPluginArray* create(LocalFrame* frame)
    {
        return new DOMPluginArray(frame);
    }

    unsigned length() const;
    DOMPlugin* item(unsigned index);
    DOMPlugin* namedItem(const AtomicString& propertyName);

    void refresh(bool reload);

    DECLARE_VIRTUAL_TRACE();

private:
    explicit DOMPluginArray(LocalFrame*);
    PluginData* pluginData() const;
};

} // namespace blink

#endif // DOMPluginArray_h
