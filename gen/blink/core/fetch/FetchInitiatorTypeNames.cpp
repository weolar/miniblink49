// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#include "FetchInitiatorTypeNames.h"

#include "wtf/StdLibExtras.h"

// Generated from:
// - fetch/FetchInitiatorTypeNames.in

namespace blink {
namespace FetchInitiatorTypeNames {

using namespace WTF;

const int kNameCount = 12;

void* NamesStorage[kNameCount * ((sizeof(AtomicString) + sizeof(void *) - 1) / sizeof(void *))];

const AtomicString& beacon = reinterpret_cast<AtomicString*>(&NamesStorage)[0];
const AtomicString& css = reinterpret_cast<AtomicString*>(&NamesStorage)[1];
const AtomicString& document = reinterpret_cast<AtomicString*>(&NamesStorage)[2];
const AtomicString& icon = reinterpret_cast<AtomicString*>(&NamesStorage)[3];
const AtomicString& internal = reinterpret_cast<AtomicString*>(&NamesStorage)[4];
const AtomicString& link = reinterpret_cast<AtomicString*>(&NamesStorage)[5];
const AtomicString& ping = reinterpret_cast<AtomicString*>(&NamesStorage)[6];
const AtomicString& processinginstruction = reinterpret_cast<AtomicString*>(&NamesStorage)[7];
const AtomicString& texttrack = reinterpret_cast<AtomicString*>(&NamesStorage)[8];
const AtomicString& violationreport = reinterpret_cast<AtomicString*>(&NamesStorage)[9];
const AtomicString& xml = reinterpret_cast<AtomicString*>(&NamesStorage)[10];
const AtomicString& xmlhttprequest = reinterpret_cast<AtomicString*>(&NamesStorage)[11];

void init()
{
    struct NameEntry {
        const char* name;
        unsigned hash;
        unsigned char length;
    };

    static const NameEntry kNames[] = {
        { "beacon", 12791408, 6 },
        { "css", 12506144, 3 },
        { "document", 13333587, 8 },
        { "icon", 12131262, 4 },
        { "internal", 15376144, 8 },
        { "link", 7010491, 4 },
        { "ping", 9193756, 4 },
        { "processinginstruction", 8687069, 21 },
        { "texttrack", 5772877, 9 },
        { "violationreport", 9570399, 15 },
        { "xml", 10322861, 3 },
        { "xmlhttprequest", 10847986, 14 },
    };

    for (size_t i = 0; i < WTF_ARRAY_LENGTH(kNames); i++) {
        StringImpl* stringImpl = StringImpl::createStatic(kNames[i].name, kNames[i].length, kNames[i].hash);
        void* address = reinterpret_cast<AtomicString*>(&NamesStorage) + i;
        new (address) AtomicString(stringImpl);
    }
}

} // FetchInitiatorTypeNames
} // namespace blink
