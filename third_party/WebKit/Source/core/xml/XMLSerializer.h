/*
 *  Copyright (C) 2003, 2006 Apple Computer, Inc.
 *  Copyright (C) 2006 Samuel Weinig (sam@webkit.org)
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

#ifndef XMLSerializer_h
#define XMLSerializer_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "platform/heap/Handle.h"

namespace blink {

class Node;

class XMLSerializer final : public GarbageCollected<XMLSerializer>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    static XMLSerializer* create()
    {
        return new XMLSerializer;
    }

    String serializeToString(Node*);

    DEFINE_INLINE_TRACE() { }

private:
    XMLSerializer() { }
};

} // namespace blink

#endif // XMLSerializer_h
