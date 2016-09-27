// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef Presentation_h
#define Presentation_h

#include "bindings/core/v8/ScriptPromise.h"
#include "core/events/EventTarget.h"
#include "core/frame/DOMWindowProperty.h"
#include "modules/presentation/PresentationSession.h"
#include "platform/heap/Handle.h"
#include "platform/heap/Heap.h"

namespace WTF {
class String;
} // namespace WTF

namespace blink {

class LocalFrame;
class PresentationController;
class ScriptState;
class WebPresentationSessionClient;
enum class WebPresentationSessionState;

// Implements the main entry point of the Presentation API corresponding to the Presentation.idl
// See https://w3c.github.io/presentation-api/#navigatorpresentation for details.
class Presentation final
    : public RefCountedGarbageCollectedEventTargetWithInlineData<Presentation>
    , public DOMWindowProperty {
    REFCOUNTED_GARBAGE_COLLECTED_EVENT_TARGET(Presentation);
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(Presentation);
    DEFINE_WRAPPERTYPEINFO();
public:
    static Presentation* create(LocalFrame*);
    ~Presentation() override;

    // EventTarget implementation.
    const AtomicString& interfaceName() const override;
    ExecutionContext* executionContext() const override;

    DECLARE_VIRTUAL_TRACE();

    PresentationSession* session() const;

    ScriptPromise startSession(ScriptState*, const String& presentationUrl);
    ScriptPromise joinSession(ScriptState*, const String& presentationUrl, const String& presentationId);
    ScriptPromise getAvailability(ScriptState*, const String& presentationUrl);

    DEFINE_ATTRIBUTE_EVENT_LISTENER(defaultsessionstart);

    // Called when the |defaultsessionstart| event needs to be fired.
    void didStartDefaultSession(PresentationSession*);

    // Called when the |onstatechange| event needs to be fired to the right session.
    void didChangeSessionState(WebPresentationSessionClient*, WebPresentationSessionState);

    // Called when the |onmessage| event needs to be fired to the right session.
    void didReceiveSessionTextMessage(WebPresentationSessionClient*, const String& message);
    void didReceiveSessionBinaryMessage(WebPresentationSessionClient*, const uint8_t* data, size_t length);

    // Adds a session to the open sessions list.
    void registerSession(PresentationSession*);

private:
    explicit Presentation(LocalFrame*);

    // Returns the session that matches the WebPresentationSessionClient or null.
    PresentationSession* findSession(WebPresentationSessionClient*);

    // The session object provided to the presentation page. Not supported.
    Member<PresentationSession> m_session;

    // The sessions opened for this frame.
    HeapHashSet<Member<PresentationSession>> m_openSessions;
};

} // namespace blink

#endif // Presentation_h
