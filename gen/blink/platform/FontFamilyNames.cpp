// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#include "FontFamilyNames.h"

#include "wtf/StdLibExtras.h"

// Generated from:
// - fonts/FontFamilyNames.in

namespace blink {
namespace FontFamilyNames {

using namespace WTF;

const int kNameCount = 7;

void* NamesStorage[kNameCount * ((sizeof(AtomicString) + sizeof(void *) - 1) / sizeof(void *))];

const AtomicString& webkit_cursive = reinterpret_cast<AtomicString*>(&NamesStorage)[0];
const AtomicString& webkit_fantasy = reinterpret_cast<AtomicString*>(&NamesStorage)[1];
const AtomicString& webkit_monospace = reinterpret_cast<AtomicString*>(&NamesStorage)[2];
const AtomicString& webkit_pictograph = reinterpret_cast<AtomicString*>(&NamesStorage)[3];
const AtomicString& webkit_sans_serif = reinterpret_cast<AtomicString*>(&NamesStorage)[4];
const AtomicString& webkit_serif = reinterpret_cast<AtomicString*>(&NamesStorage)[5];
const AtomicString& webkit_standard = reinterpret_cast<AtomicString*>(&NamesStorage)[6];

void init()
{
    struct NameEntry {
        const char* name;
        unsigned hash;
        unsigned char length;
    };

    static const NameEntry kNames[] = {
        { "-webkit-cursive", 7063387, 15 },
        { "-webkit-fantasy", 16384875, 15 },
        { "-webkit-monospace", 14315487, 17 },
        { "-webkit-pictograph", 7921289, 18 },
        { "-webkit-sans-serif", 1961926, 18 },
        { "-webkit-serif", 7070799, 13 },
        { "-webkit-standard", 6994621, 16 },
    };

    for (size_t i = 0; i < WTF_ARRAY_LENGTH(kNames); i++) {
        StringImpl* stringImpl = StringImpl::createStatic(kNames[i].name, kNames[i].length, kNames[i].hash);
        void* address = reinterpret_cast<AtomicString*>(&NamesStorage) + i;
        new (address) AtomicString(stringImpl);
    }
}

} // FontFamilyNames
} // namespace blink
