// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef HTMLMediaElementMediaSession_h
#define HTMLMediaElementMediaSession_h

#include "core/html/HTMLMediaElement.h"
#include "modules/ModulesExport.h"
#include "modules/mediasession/MediaSession.h"
#include "platform/Supplementable.h"
#include "platform/heap/Handle.h"

namespace blink {

// A supplement to HTMLMediaElement responsible for the integration of
// MediaSession with HTMLMediaElement.
class HTMLMediaElementMediaSession final : public NoBaseWillBeGarbageCollected<HTMLMediaElementMediaSession>, public WillBeHeapSupplement<HTMLMediaElement> {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(HTMLMediaElementMediaSession);
public:
    static MediaSession* session(HTMLMediaElement&);
    static void setSession(HTMLMediaElement&, MediaSession*);

    DECLARE_VIRTUAL_TRACE();

private:
    static HTMLMediaElementMediaSession& from(HTMLMediaElement&);
    static HTMLMediaElementMediaSession* fromIfExists(HTMLMediaElement&);
    static const char* supplementName();

    PersistentWillBeMember<MediaSession> m_session;
};

} // namespace blink

#endif // HTMLMediaElementMediaSession_h
