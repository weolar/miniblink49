// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#include "InputTypeNames.h"

#include "wtf/StdLibExtras.h"

// Generated from:
// - html/forms/InputTypeNames.in

namespace blink {
namespace InputTypeNames {

using namespace WTF;

const int kNameCount = 23;

void* NamesStorage[kNameCount * ((sizeof(AtomicString) + sizeof(void *) - 1) / sizeof(void *))];

const AtomicString& button = reinterpret_cast<AtomicString*>(&NamesStorage)[0];
const AtomicString& checkbox = reinterpret_cast<AtomicString*>(&NamesStorage)[1];
const AtomicString& color = reinterpret_cast<AtomicString*>(&NamesStorage)[2];
const AtomicString& date = reinterpret_cast<AtomicString*>(&NamesStorage)[3];
const AtomicString& datetime = reinterpret_cast<AtomicString*>(&NamesStorage)[4];
const AtomicString& datetime_local = reinterpret_cast<AtomicString*>(&NamesStorage)[5];
const AtomicString& email = reinterpret_cast<AtomicString*>(&NamesStorage)[6];
const AtomicString& file = reinterpret_cast<AtomicString*>(&NamesStorage)[7];
const AtomicString& hidden = reinterpret_cast<AtomicString*>(&NamesStorage)[8];
const AtomicString& image = reinterpret_cast<AtomicString*>(&NamesStorage)[9];
const AtomicString& month = reinterpret_cast<AtomicString*>(&NamesStorage)[10];
const AtomicString& number = reinterpret_cast<AtomicString*>(&NamesStorage)[11];
const AtomicString& password = reinterpret_cast<AtomicString*>(&NamesStorage)[12];
const AtomicString& radio = reinterpret_cast<AtomicString*>(&NamesStorage)[13];
const AtomicString& range = reinterpret_cast<AtomicString*>(&NamesStorage)[14];
const AtomicString& reset = reinterpret_cast<AtomicString*>(&NamesStorage)[15];
const AtomicString& search = reinterpret_cast<AtomicString*>(&NamesStorage)[16];
const AtomicString& submit = reinterpret_cast<AtomicString*>(&NamesStorage)[17];
const AtomicString& tel = reinterpret_cast<AtomicString*>(&NamesStorage)[18];
const AtomicString& text = reinterpret_cast<AtomicString*>(&NamesStorage)[19];
const AtomicString& time = reinterpret_cast<AtomicString*>(&NamesStorage)[20];
const AtomicString& url = reinterpret_cast<AtomicString*>(&NamesStorage)[21];
const AtomicString& week = reinterpret_cast<AtomicString*>(&NamesStorage)[22];

void init()
{
    struct NameEntry {
        const char* name;
        unsigned hash;
        unsigned char length;
    };

    static const NameEntry kNames[] = {
        { "button", 679691, 6 },
        { "checkbox", 2704933, 8 },
        { "color", 2734929, 5 },
        { "date", 11043522, 4 },
        { "datetime", 12167257, 8 },
        { "datetime-local", 5474839, 14 },
        { "email", 13948917, 5 },
        { "file", 14168904, 4 },
        { "hidden", 12930326, 6 },
        { "image", 10287573, 5 },
        { "month", 12226123, 5 },
        { "number", 15609029, 6 },
        { "password", 8168283, 8 },
        { "radio", 6579434, 5 },
        { "range", 2862653, 5 },
        { "reset", 13674204, 5 },
        { "search", 6906057, 6 },
        { "submit", 12328646, 6 },
        { "tel", 1681717, 3 },
        { "text", 2784654, 4 },
        { "time", 3114886, 4 },
        { "url", 10560150, 3 },
        { "week", 14375271, 4 },
    };

    for (size_t i = 0; i < WTF_ARRAY_LENGTH(kNames); i++) {
        StringImpl* stringImpl = StringImpl::createStatic(kNames[i].name, kNames[i].length, kNames[i].hash);
        void* address = reinterpret_cast<AtomicString*>(&NamesStorage) + i;
        new (address) AtomicString(stringImpl);
    }
}

} // InputTypeNames
} // namespace blink
