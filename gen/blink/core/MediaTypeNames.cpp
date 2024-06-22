// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#include "MediaTypeNames.h"

#include "wtf/StdLibExtras.h"

// Generated from:
// - css/MediaTypeNames.in

namespace blink {
namespace MediaTypeNames {

using namespace WTF;

const int kNameCount = 10;

void* NamesStorage[kNameCount * ((sizeof(AtomicString) + sizeof(void *) - 1) / sizeof(void *))];

const AtomicString& all = reinterpret_cast<AtomicString*>(&NamesStorage)[0];
const AtomicString& braille = reinterpret_cast<AtomicString*>(&NamesStorage)[1];
const AtomicString& embossed = reinterpret_cast<AtomicString*>(&NamesStorage)[2];
const AtomicString& handheld = reinterpret_cast<AtomicString*>(&NamesStorage)[3];
const AtomicString& print = reinterpret_cast<AtomicString*>(&NamesStorage)[4];
const AtomicString& projection = reinterpret_cast<AtomicString*>(&NamesStorage)[5];
const AtomicString& screen = reinterpret_cast<AtomicString*>(&NamesStorage)[6];
const AtomicString& speech = reinterpret_cast<AtomicString*>(&NamesStorage)[7];
const AtomicString& tty = reinterpret_cast<AtomicString*>(&NamesStorage)[8];
const AtomicString& tv = reinterpret_cast<AtomicString*>(&NamesStorage)[9];

void init()
{
    struct NameEntry {
        const char* name;
        unsigned hash;
        unsigned char length;
    };

    static const NameEntry kNames[] = {
        { "all", 10505778, 3 },
        { "braille", 15044340, 7 },
        { "embossed", 2246272, 8 },
        { "handheld", 12743954, 8 },
        { "print", 10456730, 5 },
        { "projection", 1089243, 10 },
        { "screen", 13980269, 6 },
        { "speech", 4509099, 6 },
        { "tty", 13331747, 3 },
        { "tv", 14748810, 2 },
    };

    for (size_t i = 0; i < WTF_ARRAY_LENGTH(kNames); i++) {
        StringImpl* stringImpl = StringImpl::createStatic(kNames[i].name, kNames[i].length, kNames[i].hash);
        void* address = reinterpret_cast<AtomicString*>(&NamesStorage) + i;
        new (address) AtomicString(stringImpl);
    }
}

} // MediaTypeNames
} // namespace blink
