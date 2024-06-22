// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#include "EventTargetModulesNames.h"

#include "wtf/StdLibExtras.h"

// Generated from:
// - ../../modules/EventTargetModulesFactory.in

namespace blink {
namespace EventTargetNames {

using namespace WTF;

const int kModulesNameCount = 39;

void* ModulesNamesStorage[kModulesNameCount * ((sizeof(AtomicString) + sizeof(void *) - 1) / sizeof(void *))];

const AtomicString& BatteryManager = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[0];
const AtomicString& CompositorWorker = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[1];
const AtomicString& CompositorWorkerGlobalScope = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[2];
const AtomicString& MediaKeySession = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[3];
const AtomicString& FileWriter = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[4];
const AtomicString& IDBDatabase = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[5];
const AtomicString& IDBOpenDBRequest = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[6];
const AtomicString& IDBRequest = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[7];
const AtomicString& IDBTransaction = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[8];
const AtomicString& MediaSource = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[9];
const AtomicString& SourceBuffer = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[10];
const AtomicString& SourceBufferList = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[11];
const AtomicString& MediaStream = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[12];
const AtomicString& MediaStreamTrack = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[13];
const AtomicString& RTCDTMFSender = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[14];
const AtomicString& RTCDataChannel = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[15];
const AtomicString& RTCPeerConnection = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[16];
const AtomicString& ServicePortCollection = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[17];
const AtomicString& NetworkInformation = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[18];
const AtomicString& Notification = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[19];
const AtomicString& PermissionStatus = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[20];
const AtomicString& Presentation = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[21];
const AtomicString& PresentationAvailability = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[22];
const AtomicString& PresentationSession = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[23];
const AtomicString& ScreenOrientation = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[24];
const AtomicString& ServiceWorker = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[25];
const AtomicString& ServiceWorkerContainer = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[26];
const AtomicString& ServiceWorkerGlobalScope = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[27];
const AtomicString& ServiceWorkerRegistration = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[28];
const AtomicString& StashedPortCollection = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[29];
const AtomicString& SpeechRecognition = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[30];
const AtomicString& SpeechSynthesis = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[31];
const AtomicString& SpeechSynthesisUtterance = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[32];
#if ENABLE(WEB_AUDIO)
const AtomicString& AudioContext = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[33];
#endif // ENABLE(WEB_AUDIO)
#if ENABLE(WEB_AUDIO)
const AtomicString& AudioNode = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[34];
#endif // ENABLE(WEB_AUDIO)
const AtomicString& MIDIAccess = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[35];
const AtomicString& MIDIInput = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[36];
const AtomicString& MIDIPort = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[37];
const AtomicString& DOMWebSocket = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[38];

void initModules()
{
    struct NameEntry {
        const char* name;
        unsigned hash;
        unsigned char length;
    };

    static const NameEntry kNames[] = {
        { "BatteryManager", 5049459, 14 },
        { "CompositorWorker", 9970284, 16 },
        { "CompositorWorkerGlobalScope", 7405197, 27 },
        { "MediaKeySession", 1701501, 15 },
        { "FileWriter", 15152482, 10 },
        { "IDBDatabase", 1126017, 11 },
        { "IDBOpenDBRequest", 8388818, 16 },
        { "IDBRequest", 16217380, 10 },
        { "IDBTransaction", 13326789, 14 },
        { "MediaSource", 10009800, 11 },
        { "SourceBuffer", 3244577, 12 },
        { "SourceBufferList", 5225283, 16 },
        { "MediaStream", 5095066, 11 },
        { "MediaStreamTrack", 16623886, 16 },
        { "RTCDTMFSender", 7965945, 13 },
        { "RTCDataChannel", 16002100, 14 },
        { "RTCPeerConnection", 11238437, 17 },
        { "ServicePortCollection", 1521644, 21 },
        { "NetworkInformation", 3171974, 18 },
        { "Notification", 7262241, 12 },
        { "PermissionStatus", 4818911, 16 },
        { "Presentation", 15117440, 12 },
        { "PresentationAvailability", 10735288, 24 },
        { "PresentationSession", 16306778, 19 },
        { "ScreenOrientation", 16006315, 17 },
        { "ServiceWorker", 3155984, 13 },
        { "ServiceWorkerContainer", 6814814, 22 },
        { "ServiceWorkerGlobalScope", 16474009, 24 },
        { "ServiceWorkerRegistration", 11774198, 25 },
        { "StashedPortCollection", 9678830, 21 },
        { "SpeechRecognition", 1914212, 17 },
        { "SpeechSynthesis", 9951205, 15 },
        { "SpeechSynthesisUtterance", 12730322, 24 },
        { "AudioContext", 5207992, 12 },
        { "AudioNode", 5081561, 9 },
        { "MIDIAccess", 13252796, 10 },
        { "MIDIInput", 13226940, 9 },
        { "MIDIPort", 15710269, 8 },
        { "DOMWebSocket", 15572381, 12 },
    };

    for (size_t i = 0; i < WTF_ARRAY_LENGTH(kNames); i++) {
        StringImpl* stringImpl = StringImpl::createStatic(kNames[i].name, kNames[i].length, kNames[i].hash);
        void* address = reinterpret_cast<AtomicString*>(&ModulesNamesStorage) + i;
        new (address) AtomicString(stringImpl);
    }
}

} // EventTargetNames
} // namespace blink
