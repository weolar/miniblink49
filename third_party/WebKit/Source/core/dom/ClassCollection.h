/*
 * Copyright (C) 2007 Apple Inc. All rights reserved.
 * Copyright (C) 2007 David Smith (catfish.man@gmail.com)
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

#ifndef ClassCollection_h
#define ClassCollection_h

#include "core/dom/Element.h"
#include "core/dom/SpaceSplitString.h"
#include "core/html/HTMLCollection.h"

namespace blink {

class ClassCollection final : public HTMLCollection {
public:
    // classNames argument is an AtomicString because it is common for Elements to share the same class names.
    // It is also used to construct a SpaceSplitString (m_classNames) and its constructor requires an AtomicString.
    static PassRefPtrWillBeRawPtr<ClassCollection> create(ContainerNode& rootNode, CollectionType type, const AtomicString& classNames)
    {
        ASSERT_UNUSED(type, type == ClassCollectionType);
        return adoptRefWillBeNoop(new ClassCollection(rootNode, classNames));
    }

    ~ClassCollection() override;

    bool elementMatches(const Element&) const;

private:
    ClassCollection(ContainerNode& rootNode, const AtomicString& classNames);

    SpaceSplitString m_classNames;
    AtomicString m_originalClassNames;
};

DEFINE_TYPE_CASTS(ClassCollection, LiveNodeListBase, collection, collection->type() == ClassCollectionType, collection.type() == ClassCollectionType);

inline bool ClassCollection::elementMatches(const Element& testElement) const
{
    if (!testElement.hasClass())
        return false;
    if (!m_classNames.size())
        return false;
    return testElement.classNames().containsAll(m_classNames);
}

} // namespace blink

#endif // ClassCollection_h
