// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#include "XMLNames.h"

#include "wtf/StaticConstructors.h"
#include "wtf/StdLibExtras.h"

namespace blink {
namespace XMLNames {

using namespace blink;

DEFINE_GLOBAL(AtomicString, xmlNamespaceURI)

// Attributes

void* AttrStorage[XMLAttrsCount * ((sizeof(QualifiedName) + sizeof(void *) - 1) / sizeof(void *))];

const QualifiedName& langAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[0];
const QualifiedName& spaceAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[1];

PassOwnPtr<const QualifiedName*[]> getXMLAttrs()
{
    OwnPtr<const QualifiedName*[]> attrs = adoptArrayPtr(new const QualifiedName*[XMLAttrsCount]);
    for (size_t i = 0; i < XMLAttrsCount; i++)
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
    AtomicString xmlNS("http://www.w3.org/XML/1998/namespace", AtomicString::ConstructFromLiteral);

    // Namespace
    new ((void*)&xmlNamespaceURI) AtomicString(xmlNS);
    static const NameEntry kNames[] = {
        { "lang", 3702417, 4, 0, 1 },
        { "space", 531440, 5, 0, 1 },
    };

    size_t attr_i = 0;
    for (size_t i = 0; i < WTF_ARRAY_LENGTH(kNames); i++) {
        StringImpl* stringImpl = StringImpl::createStatic(kNames[i].name, kNames[i].length, kNames[i].hash);
        void* address = reinterpret_cast<QualifiedName*>(&AttrStorage) + attr_i;
        QualifiedName::createStatic(address, stringImpl, xmlNS);
        attr_i++;
    }
    ASSERT(attr_i == XMLAttrsCount);
}

} // XML
} // namespace blink
