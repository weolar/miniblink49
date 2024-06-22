// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#include "EventModulesNames.h"

#include "wtf/StdLibExtras.h"

// Generated from:
// - ..\..\..\..\..\..\..\out\Debug/gen/blink/modules/EventModulesInterfaces.in

namespace blink {
namespace EventNames {

using namespace WTF;

const int kModulesNameCount = 34;

void* ModulesNamesStorage[kModulesNameCount * ((sizeof(AtomicString) + sizeof(void *) - 1) / sizeof(void *))];

const AtomicString& BeforeInstallPromptEvent = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[0];
const AtomicString& PeriodicSyncEvent = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[1];
const AtomicString& SyncEvent = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[2];
const AtomicString& DeviceLightEvent = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[3];
const AtomicString& DeviceMotionEvent = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[4];
const AtomicString& DeviceOrientationEvent = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[5];
const AtomicString& MediaEncryptedEvent = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[6];
const AtomicString& MediaKeyMessageEvent = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[7];
const AtomicString& GamepadEvent = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[8];
const AtomicString& GeofencingEvent = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[9];
const AtomicString& IDBVersionChangeEvent = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[10];
const AtomicString& MediaStreamEvent = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[11];
const AtomicString& MediaStreamTrackEvent = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[12];
const AtomicString& RTCDTMFToneChangeEvent = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[13];
const AtomicString& RTCDataChannelEvent = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[14];
const AtomicString& RTCIceCandidateEvent = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[15];
const AtomicString& CrossOriginConnectEvent = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[16];
const AtomicString& ServicePortConnectEvent = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[17];
const AtomicString& NotificationEvent = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[18];
const AtomicString& DefaultSessionStartEvent = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[19];
const AtomicString& PushEvent = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[20];
const AtomicString& ExtendableEvent = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[21];
const AtomicString& FetchEvent = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[22];
const AtomicString& ServiceWorkerMessageEvent = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[23];
const AtomicString& SpeechRecognitionError = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[24];
const AtomicString& SpeechRecognitionEvent = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[25];
const AtomicString& SpeechSynthesisEvent = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[26];
const AtomicString& StorageEvent = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[27];
#if ENABLE(WEB_AUDIO)
const AtomicString& AudioProcessingEvent = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[28];
#endif // ENABLE(WEB_AUDIO)
#if ENABLE(WEB_AUDIO)
const AtomicString& OfflineAudioCompletionEvent = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[29];
#endif // ENABLE(WEB_AUDIO)
const AtomicString& WebGLContextEvent = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[30];
const AtomicString& MIDIConnectionEvent = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[31];
const AtomicString& MIDIMessageEvent = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[32];
const AtomicString& CloseEvent = reinterpret_cast<AtomicString*>(&ModulesNamesStorage)[33];

void initModules()
{
    struct NameEntry {
        const char* name;
        unsigned hash;
        unsigned char length;
    };

    static const NameEntry kNames[] = {
        { "BeforeInstallPromptEvent", 4066404, 24 },
        { "PeriodicSyncEvent", 13473429, 17 },
        { "SyncEvent", 5657331, 9 },
        { "DeviceLightEvent", 8667494, 16 },
        { "DeviceMotionEvent", 12345260, 17 },
        { "DeviceOrientationEvent", 858735, 22 },
        { "MediaEncryptedEvent", 13703864, 19 },
        { "MediaKeyMessageEvent", 4827241, 20 },
        { "GamepadEvent", 11020890, 12 },
        { "GeofencingEvent", 14416555, 15 },
        { "IDBVersionChangeEvent", 7770749, 21 },
        { "MediaStreamEvent", 328267, 16 },
        { "MediaStreamTrackEvent", 13109664, 21 },
        { "RTCDTMFToneChangeEvent", 16457152, 22 },
        { "RTCDataChannelEvent", 1105547, 19 },
        { "RTCIceCandidateEvent", 6147283, 20 },
        { "CrossOriginConnectEvent", 7469206, 23 },
        { "ServicePortConnectEvent", 8636685, 23 },
        { "NotificationEvent", 8638709, 17 },
        { "DefaultSessionStartEvent", 15750974, 24 },
        { "PushEvent", 508183, 9 },
        { "ExtendableEvent", 2731257, 15 },
        { "FetchEvent", 6447122, 10 },
        { "ServiceWorkerMessageEvent", 10604454, 25 },
        { "SpeechRecognitionError", 13037355, 22 },
        { "SpeechRecognitionEvent", 6869292, 22 },
        { "SpeechSynthesisEvent", 17758, 20 },
        { "StorageEvent", 5246637, 12 },
        { "AudioProcessingEvent", 9607128, 20 },
        { "OfflineAudioCompletionEvent", 6072331, 27 },
        { "WebGLContextEvent", 8713913, 17 },
        { "MIDIConnectionEvent", 5515626, 19 },
        { "MIDIMessageEvent", 12787442, 16 },
        { "CloseEvent", 11722244, 10 },
    };

    for (size_t i = 0; i < WTF_ARRAY_LENGTH(kNames); i++) {
        StringImpl* stringImpl = StringImpl::createStatic(kNames[i].name, kNames[i].length, kNames[i].hash);
        void* address = reinterpret_cast<AtomicString*>(&ModulesNamesStorage) + i;
        new (address) AtomicString(stringImpl);
    }
}

} // EventNames
} // namespace blink
