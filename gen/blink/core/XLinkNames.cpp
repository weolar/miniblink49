// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#include "XLinkNames.h"

#include "wtf/StaticConstructors.h"
#include "wtf/StdLibExtras.h"

namespace blink {
namespace XLinkNames {

using namespace blink;

DEFINE_GLOBAL(AtomicString, xlinkNamespaceURI)

// Attributes

void* AttrStorage[XLinkAttrsCount * ((sizeof(QualifiedName) + sizeof(void *) - 1) / sizeof(void *))];

const QualifiedName& actuateAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[0];
const QualifiedName& arcroleAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[1];
const QualifiedName& hrefAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[2];
const QualifiedName& roleAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[3];
const QualifiedName& showAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[4];
const QualifiedName& titleAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[5];
const QualifiedName& typeAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[6];

PassOwnPtr<const QualifiedName*[]> getXLinkAttrs()
{
    OwnPtr<const QualifiedName*[]> attrs = adoptArrayPtr(new const QualifiedName*[XLinkAttrsCount]);
    for (size_t i = 0; i < XLinkAttrsCount; i++)
        attrs[i] = reinterpret_cast<QualifiedName*>(&AttrStorage) + i;
    return attrs.release();
}


void init()
{
    struct NameEntry {
        const char* name;
        unsigned hash;
        unsigned char length;
        unsigned char isTag;
        unsigned char isAttr;
    };

    // Use placement new to initialize the globals.
    AtomicString xlinkNS("http://www.w3.org/1999/xlink", AtomicString::ConstructFromLiteral);

    // Namespace
    new ((void*)&xlinkNamespaceURI) AtomicString(xlinkNS);
    static const NameEntry kNames[] = {
        { "actuate", 5578769, 7, 0, 1 },
        { "arcrole", 11561565, 7, 0, 1 },
        { "href", 5797448, 4, 0, 1 },
        { "role", 16084934, 4, 0, 1 },
        { "show", 3191658, 4, 0, 1 },
        { "title", 2337488, 5, 0, 1 },
        { "type", 1916283, 4, 0, 1 },
    };

    size_t attr_i = 0;
    for (size_t i = 0; i < WTF_ARRAY_LENGTH(kNames); i++) {
        StringImpl* stringImpl = StringImpl::createStatic(kNames[i].name, kNames[i].length, kNames[i].hash);
        void* address = reinterpret_cast<QualifiedName*>(&AttrStorage) + attr_i;
        QualifiedName::createStatic(address, stringImpl, xlinkNS);
        attr_i++;
    }
    ASSERT(attr_i == XLinkAttrsCount);
}

} // XLink
} // namespace blink
