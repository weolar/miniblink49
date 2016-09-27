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

#ifndef DOMMimeTypeArray_h
#define DOMMimeTypeArray_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "core/frame/DOMWindowProperty.h"
#include "modules/plugins/DOMMimeType.h"
#include "platform/heap/Handle.h"
#include "wtf/Forward.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefCounted.h"

namespace blink {

class LocalFrame;
class PluginData;

class DOMMimeTypeArray final : public GarbageCollectedFinalized<DOMMimeTypeArray>, public ScriptWrappable, public DOMWindowProperty {
    DEFINE_WRAPPERTYPEINFO();
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(DOMMimeTypeArray);
public:
    static DOMMimeTypeArray* create(LocalFrame* frame)
    {
        return new DOMMimeTypeArray(frame);
    }

    unsigned length() const;
    DOMMimeType* item(unsigned index);
    DOMMimeType* namedItem(const AtomicString& propertyName);

    DECLARE_VIRTUAL_TRACE();

private:
    explicit DOMMimeTypeArray(LocalFrame*);
    PluginData* getPluginData() const;
};

} // namespace blink

#endif // DOMMimeTypeArray_h
