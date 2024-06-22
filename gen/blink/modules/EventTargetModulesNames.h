// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef EventTargetModulesNames_h
#define EventTargetModulesNames_h

#include "core/EventTargetNames.h"
#include "platform/PlatformExport.h"

// Generated from:
// - ../../modules/EventTargetModulesFactory.in

namespace blink {
namespace EventTargetNames {

extern const WTF::AtomicString& BatteryManager;
extern const WTF::AtomicString& CompositorWorker;
extern const WTF::AtomicString& CompositorWorkerGlobalScope;
extern const WTF::AtomicString& MediaKeySession;
extern const WTF::AtomicString& FileWriter;
extern const WTF::AtomicString& IDBDatabase;
extern const WTF::AtomicString& IDBOpenDBRequest;
extern const WTF::AtomicString& IDBRequest;
extern const WTF::AtomicString& IDBTransaction;
extern const WTF::AtomicString& MediaSource;
extern const WTF::AtomicString& SourceBuffer;
extern const WTF::AtomicString& SourceBufferList;
extern const WTF::AtomicString& MediaStream;
extern const WTF::AtomicString& MediaStreamTrack;
extern const WTF::AtomicString& RTCDTMFSender;
extern const WTF::AtomicString& RTCDataChannel;
extern const WTF::AtomicString& RTCPeerConnection;
extern const WTF::AtomicString& ServicePortCollection;
extern const WTF::AtomicString& NetworkInformation;
extern const WTF::AtomicString& Notification;
extern const WTF::AtomicString& PermissionStatus;
extern const WTF::AtomicString& Presentation;
extern const WTF::AtomicString& PresentationAvailability;
extern const WTF::AtomicString& PresentationSession;
extern const WTF::AtomicString& ScreenOrientation;
extern const WTF::AtomicString& ServiceWorker;
extern const WTF::AtomicString& ServiceWorkerContainer;
extern const WTF::AtomicString& ServiceWorkerGlobalScope;
extern const WTF::AtomicString& ServiceWorkerRegistration;
extern const WTF::AtomicString& StashedPortCollection;
extern const WTF::AtomicString& SpeechRecognition;
extern const WTF::AtomicString& SpeechSynthesis;
extern const WTF::AtomicString& SpeechSynthesisUtterance;
extern const WTF::AtomicString& MIDIAccess;
extern const WTF::AtomicString& MIDIInput;
extern const WTF::AtomicString& MIDIPort;
extern const WTF::AtomicString& DOMWebSocket;
#if ENABLE(WEB_AUDIO)
extern const WTF::AtomicString& AudioContext;
#endif // ENABLE(WEB_AUDIO)
#if ENABLE(WEB_AUDIO)
extern const WTF::AtomicString& AudioNode;
#endif // ENABLE(WEB_AUDIO)

void initModules();

} // EventTargetNames
} // namespace blink

#endif
