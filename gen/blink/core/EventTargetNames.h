// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef EventTargetNames_h
#define EventTargetNames_h

#include "wtf/text/AtomicString.h"
#include "core/CoreExport.h"

// Generated from:
// - events/EventTargetFactory.in

namespace blink {
namespace EventTargetNames {

CORE_EXPORT extern const WTF::AtomicString& AnimationPlayer;
CORE_EXPORT extern const WTF::AtomicString& FontFaceSet;
CORE_EXPORT extern const WTF::AtomicString& MediaQueryList;
CORE_EXPORT extern const WTF::AtomicString& MessagePort;
CORE_EXPORT extern const WTF::AtomicString& Node;
CORE_EXPORT extern const WTF::AtomicString& FileReader;
CORE_EXPORT extern const WTF::AtomicString& MediaController;
CORE_EXPORT extern const WTF::AtomicString& InputMethodContext;
CORE_EXPORT extern const WTF::AtomicString& AudioTrackList;
CORE_EXPORT extern const WTF::AtomicString& TextTrack;
CORE_EXPORT extern const WTF::AtomicString& TextTrackCue;
CORE_EXPORT extern const WTF::AtomicString& TextTrackList;
CORE_EXPORT extern const WTF::AtomicString& VideoTrackList;
CORE_EXPORT extern const WTF::AtomicString& ApplicationCache;
CORE_EXPORT extern const WTF::AtomicString& EventSource;
CORE_EXPORT extern const WTF::AtomicString& Performance;
CORE_EXPORT extern const WTF::AtomicString& DedicatedWorkerGlobalScope;
CORE_EXPORT extern const WTF::AtomicString& SharedWorker;
CORE_EXPORT extern const WTF::AtomicString& SharedWorkerGlobalScope;
CORE_EXPORT extern const WTF::AtomicString& Worker;
CORE_EXPORT extern const WTF::AtomicString& XMLHttpRequest;
CORE_EXPORT extern const WTF::AtomicString& XMLHttpRequestUpload;
CORE_EXPORT extern const WTF::AtomicString& DOMWindow;

CORE_EXPORT void init();

} // EventTargetNames
} // namespace blink

#endif
