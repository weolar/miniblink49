// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef EventNames_h
#define EventNames_h

#include "wtf/text/AtomicString.h"
#include "core/CoreExport.h"

// Generated from:
// - ..\..\..\..\..\..\out\Debug/gen/blink/core/EventInterfaces.in

namespace blink {
namespace EventNames {

CORE_EXPORT extern const WTF::AtomicString& FontFaceSetLoadEvent;
CORE_EXPORT extern const WTF::AtomicString& MediaQueryListEvent;
CORE_EXPORT extern const WTF::AtomicString& AnimationEvent;
CORE_EXPORT extern const WTF::AtomicString& ApplicationCacheErrorEvent;
CORE_EXPORT extern const WTF::AtomicString& AutocompleteErrorEvent;
CORE_EXPORT extern const WTF::AtomicString& BeforeUnloadEvent;
CORE_EXPORT extern const WTF::AtomicString& ClipboardEvent;
CORE_EXPORT extern const WTF::AtomicString& CompositionEvent;
CORE_EXPORT extern const WTF::AtomicString& CustomEvent;
CORE_EXPORT extern const WTF::AtomicString& ErrorEvent;
CORE_EXPORT extern const WTF::AtomicString& Event;
CORE_EXPORT extern const WTF::AtomicString& FocusEvent;
CORE_EXPORT extern const WTF::AtomicString& HashChangeEvent;
CORE_EXPORT extern const WTF::AtomicString& KeyboardEvent;
CORE_EXPORT extern const WTF::AtomicString& MessageEvent;
CORE_EXPORT extern const WTF::AtomicString& MouseEvent;
CORE_EXPORT extern const WTF::AtomicString& MutationEvent;
CORE_EXPORT extern const WTF::AtomicString& PageTransitionEvent;
CORE_EXPORT extern const WTF::AtomicString& PopStateEvent;
CORE_EXPORT extern const WTF::AtomicString& ProgressEvent;
CORE_EXPORT extern const WTF::AtomicString& ResourceProgressEvent;
CORE_EXPORT extern const WTF::AtomicString& SecurityPolicyViolationEvent;
CORE_EXPORT extern const WTF::AtomicString& TextEvent;
CORE_EXPORT extern const WTF::AtomicString& TouchEvent;
CORE_EXPORT extern const WTF::AtomicString& TransitionEvent;
CORE_EXPORT extern const WTF::AtomicString& UIEvent;
CORE_EXPORT extern const WTF::AtomicString& WheelEvent;
CORE_EXPORT extern const WTF::AtomicString& TrackEvent;
CORE_EXPORT extern const WTF::AtomicString& SVGZoomEvent;
CORE_EXPORT extern const WTF::AtomicString& XMLHttpRequestProgressEvent;
CORE_EXPORT extern const WTF::AtomicString& RelatedEvent;
CORE_EXPORT extern const WTF::AtomicString& PointerEvent;
CORE_EXPORT extern const WTF::AtomicString& MediaKeyEvent;
CORE_EXPORT extern const WTF::AtomicString& PromiseRejectionEvent;
CORE_EXPORT extern const WTF::AtomicString& AnimationPlayerEvent;

CORE_EXPORT void init();

} // EventNames
} // namespace blink

#endif
