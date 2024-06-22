// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#include "EventTargetNames.h"

#include "wtf/StdLibExtras.h"

// Generated from:
// - events/EventTargetFactory.in

namespace blink {
namespace EventTargetNames {

using namespace WTF;

const int kNameCount = 23;

void* NamesStorage[kNameCount * ((sizeof(AtomicString) + sizeof(void *) - 1) / sizeof(void *))];

const AtomicString& AnimationPlayer = reinterpret_cast<AtomicString*>(&NamesStorage)[0];
const AtomicString& FontFaceSet = reinterpret_cast<AtomicString*>(&NamesStorage)[1];
const AtomicString& MediaQueryList = reinterpret_cast<AtomicString*>(&NamesStorage)[2];
const AtomicString& MessagePort = reinterpret_cast<AtomicString*>(&NamesStorage)[3];
const AtomicString& Node = reinterpret_cast<AtomicString*>(&NamesStorage)[4];
const AtomicString& FileReader = reinterpret_cast<AtomicString*>(&NamesStorage)[5];
const AtomicString& DOMWindow = reinterpret_cast<AtomicString*>(&NamesStorage)[6];
const AtomicString& MediaController = reinterpret_cast<AtomicString*>(&NamesStorage)[7];
const AtomicString& InputMethodContext = reinterpret_cast<AtomicString*>(&NamesStorage)[8];
const AtomicString& AudioTrackList = reinterpret_cast<AtomicString*>(&NamesStorage)[9];
const AtomicString& TextTrack = reinterpret_cast<AtomicString*>(&NamesStorage)[10];
const AtomicString& TextTrackCue = reinterpret_cast<AtomicString*>(&NamesStorage)[11];
const AtomicString& TextTrackList = reinterpret_cast<AtomicString*>(&NamesStorage)[12];
const AtomicString& VideoTrackList = reinterpret_cast<AtomicString*>(&NamesStorage)[13];
const AtomicString& ApplicationCache = reinterpret_cast<AtomicString*>(&NamesStorage)[14];
const AtomicString& EventSource = reinterpret_cast<AtomicString*>(&NamesStorage)[15];
const AtomicString& Performance = reinterpret_cast<AtomicString*>(&NamesStorage)[16];
const AtomicString& DedicatedWorkerGlobalScope = reinterpret_cast<AtomicString*>(&NamesStorage)[17];
const AtomicString& SharedWorker = reinterpret_cast<AtomicString*>(&NamesStorage)[18];
const AtomicString& SharedWorkerGlobalScope = reinterpret_cast<AtomicString*>(&NamesStorage)[19];
const AtomicString& Worker = reinterpret_cast<AtomicString*>(&NamesStorage)[20];
const AtomicString& XMLHttpRequest = reinterpret_cast<AtomicString*>(&NamesStorage)[21];
const AtomicString& XMLHttpRequestUpload = reinterpret_cast<AtomicString*>(&NamesStorage)[22];

void init()
{
    struct NameEntry {
        const char* name;
        unsigned hash;
        unsigned char length;
    };

    static const NameEntry kNames[] = {
        { "AnimationPlayer", 8316901, 15 },
        { "FontFaceSet", 15992392, 11 },
        { "MediaQueryList", 4446762, 14 },
        { "MessagePort", 7675160, 11 },
        { "Node", 7481749, 4 },
        { "FileReader", 1122872, 10 },
        { "DOMWindow", 5154002, 9 },
        { "MediaController", 5103076, 15 },
        { "InputMethodContext", 6735798, 18 },
        { "AudioTrackList", 4289573, 14 },
        { "TextTrack", 6141630, 9 },
        { "TextTrackCue", 12570487, 12 },
        { "TextTrackList", 15622086, 13 },
        { "VideoTrackList", 12436801, 14 },
        { "ApplicationCache", 14896278, 16 },
        { "EventSource", 10057717, 11 },
        { "Performance", 16508941, 11 },
        { "DedicatedWorkerGlobalScope", 16739179, 26 },
        { "SharedWorker", 5095866, 12 },
        { "SharedWorkerGlobalScope", 14997459, 23 },
        { "Worker", 14821068, 6 },
        { "XMLHttpRequest", 5139743, 14 },
        { "XMLHttpRequestUpload", 16716896, 20 },
    };

    for (size_t i = 0; i < WTF_ARRAY_LENGTH(kNames); i++) {
        StringImpl* stringImpl = StringImpl::createStatic(kNames[i].name, kNames[i].length, kNames[i].hash);
        void* address = reinterpret_cast<AtomicString*>(&NamesStorage) + i;
        new (address) AtomicString(stringImpl);
    }
}

} // EventTargetNames
} // namespace blink
