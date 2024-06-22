// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#include "MathMLNames.h"

#include "wtf/StaticConstructors.h"
#include "wtf/StdLibExtras.h"

namespace blink {
namespace MathMLNames {

using namespace blink;

DEFINE_GLOBAL(AtomicString, mathmlNamespaceURI)

// Tags

void* TagStorage[MathMLTagsCount * ((sizeof(MathMLQualifiedName) + sizeof(void *) - 1) / sizeof(void *))];
const MathMLQualifiedName& annotation_xmlTag = reinterpret_cast<MathMLQualifiedName*>(&TagStorage)[0];
const MathMLQualifiedName& malignmarkTag = reinterpret_cast<MathMLQualifiedName*>(&TagStorage)[1];
const MathMLQualifiedName& mathTag = reinterpret_cast<MathMLQualifiedName*>(&TagStorage)[2];
const MathMLQualifiedName& mglyphTag = reinterpret_cast<MathMLQualifiedName*>(&TagStorage)[3];
const MathMLQualifiedName& miTag = reinterpret_cast<MathMLQualifiedName*>(&TagStorage)[4];
const MathMLQualifiedName& mnTag = reinterpret_cast<MathMLQualifiedName*>(&TagStorage)[5];
const MathMLQualifiedName& moTag = reinterpret_cast<MathMLQualifiedName*>(&TagStorage)[6];
const MathMLQualifiedName& msTag = reinterpret_cast<MathMLQualifiedName*>(&TagStorage)[7];
const MathMLQualifiedName& mtextTag = reinterpret_cast<MathMLQualifiedName*>(&TagStorage)[8];


PassOwnPtr<const MathMLQualifiedName*[]> getMathMLTags()
{
    OwnPtr<const MathMLQualifiedName*[]> tags = adoptArrayPtr(new const MathMLQualifiedName*[MathMLTagsCount]);
    for (size_t i = 0; i < MathMLTagsCount; i++)
        tags[i] = reinterpret_cast<MathMLQualifiedName*>(&TagStorage) + i;
    return tags.release();
}

// Attributes

void* AttrStorage[MathMLAttrsCount * ((sizeof(QualifiedName) + sizeof(void *) - 1) / sizeof(void *))];

const QualifiedName& definitionURLAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[0];
const QualifiedName& encodingAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[1];

PassOwnPtr<const QualifiedName*[]> getMathMLAttrs()
{
    OwnPtr<const QualifiedName*[]> attrs = adoptArrayPtr(new const QualifiedName*[MathMLAttrsCount]);
    for (size_t i = 0; i < MathMLAttrsCount; i++)
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
    AtomicString mathmlNS("http://www.w3.org/1998/Math/MathML", AtomicString::ConstructFromLiteral);

    // Namespace
    new ((void*)&mathmlNamespaceURI) AtomicString(mathmlNS);
    static const NameEntry kNames[] = {
        { "annotation-xml", 3437210, 14, 1, 0 },
        { "definitionURL", 5781331, 13, 0, 1 },
        { "encoding", 641915, 8, 0, 1 },
        { "malignmark", 8634190, 10, 1, 0 },
        { "math", 8182406, 4, 1, 0 },
        { "mglyph", 3316076, 6, 1, 0 },
        { "mi", 16514040, 2, 1, 0 },
        { "mn", 3612898, 2, 1, 0 },
        { "mo", 16096753, 2, 1, 0 },
        { "ms", 5264856, 2, 1, 0 },
        { "mtext", 16756693, 5, 1, 0 },
    };

    size_t tag_i = 0;
    size_t attr_i = 0;
    for (size_t i = 0; i < WTF_ARRAY_LENGTH(kNames); i++) {
        StringImpl* stringImpl = StringImpl::createStatic(kNames[i].name, kNames[i].length, kNames[i].hash);
        if (kNames[i].isTag) {
            void* address = reinterpret_cast<MathMLQualifiedName*>(&TagStorage) + tag_i;
            QualifiedName::createStatic(address, stringImpl, mathmlNS);
            tag_i++;
        }

        if (!kNames[i].isAttr)
            continue;
        void* address = reinterpret_cast<QualifiedName*>(&AttrStorage) + attr_i;
        QualifiedName::createStatic(address, stringImpl);
        attr_i++;
    }
    ASSERT(tag_i == MathMLTagsCount);
    ASSERT(attr_i == MathMLAttrsCount);
}

} // MathML
} // namespace blink
