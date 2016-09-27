/*
 *  Copyright (C) 2003, 2006 Apple Computer, Inc.
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

#ifndef DOMParser_h
#define DOMParser_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "platform/heap/Handle.h"
#include "wtf/Forward.h"

namespace blink {

class Document;

class DOMParser final : public GarbageCollectedFinalized<DOMParser>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    static DOMParser* create(Document& document)
    {
        return new DOMParser(document);
    }

    PassRefPtrWillBeRawPtr<Document> parseFromString(const String&, const String& type);

    DECLARE_TRACE();

private:
    explicit DOMParser(Document&);

    WeakPtrWillBeWeakMember<Document> m_contextDocument;
};

} // namespace blink

#endif // DOMParser_h
