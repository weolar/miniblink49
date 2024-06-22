// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#include "IndexedDBNames.h"

#include "wtf/StdLibExtras.h"

// Generated from:
// - indexeddb/IndexedDBNames.in

namespace blink {
namespace IndexedDBNames {

using namespace WTF;

const int kNameCount = 11;

void* NamesStorage[kNameCount * ((sizeof(AtomicString) + sizeof(void *) - 1) / sizeof(void *))];

const AtomicString& done = reinterpret_cast<AtomicString*>(&NamesStorage)[0];
const AtomicString& next = reinterpret_cast<AtomicString*>(&NamesStorage)[1];
const AtomicString& nextunique = reinterpret_cast<AtomicString*>(&NamesStorage)[2];
const AtomicString& none = reinterpret_cast<AtomicString*>(&NamesStorage)[3];
const AtomicString& pending = reinterpret_cast<AtomicString*>(&NamesStorage)[4];
const AtomicString& prev = reinterpret_cast<AtomicString*>(&NamesStorage)[5];
const AtomicString& prevunique = reinterpret_cast<AtomicString*>(&NamesStorage)[6];
const AtomicString& readonly = reinterpret_cast<AtomicString*>(&NamesStorage)[7];
const AtomicString& readwrite = reinterpret_cast<AtomicString*>(&NamesStorage)[8];
const AtomicString& total = reinterpret_cast<AtomicString*>(&NamesStorage)[9];
const AtomicString& versionchange = reinterpret_cast<AtomicString*>(&NamesStorage)[10];

void init()
{
    struct NameEntry {
        const char* name;
        unsigned hash;
        unsigned char length;
    };

    static const NameEntry kNames[] = {
        { "done", 11685723, 4 },
        { "next", 13682113, 4 },
        { "nextunique", 15395501, 10 },
        { "none", 3454253, 4 },
        { "pending", 1181072, 7 },
        { "prev", 6676561, 4 },
        { "prevunique", 7206471, 10 },
        { "readonly", 4471832, 8 },
        { "readwrite", 15868712, 9 },
        { "total", 1584834, 5 },
        { "versionchange", 1334380, 13 },
    };

    for (size_t i = 0; i < WTF_ARRAY_LENGTH(kNames); i++) {
        StringImpl* stringImpl = StringImpl::createStatic(kNames[i].name, kNames[i].length, kNames[i].hash);
        void* address = reinterpret_cast<AtomicString*>(&NamesStorage) + i;
        new (address) AtomicString(stringImpl);
    }
}

} // IndexedDBNames
} // namespace blink
