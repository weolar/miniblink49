/*
 * Copyright (C) 2011, Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
 * DAMAGE.
 */

#ifndef NavigatorGamepad_h
#define NavigatorGamepad_h

#include "core/frame/DOMWindowLifecycleObserver.h"
#include "core/frame/DOMWindowProperty.h"
#include "core/frame/PlatformEventController.h"
#include "modules/ModulesExport.h"
#include "platform/AsyncMethodRunner.h"
#include "platform/Supplementable.h"
#include "platform/heap/Handle.h"
#include "public/platform/WebGamepads.h"

namespace blink {

class Document;
class Gamepad;
class GamepadList;
class Navigator;

class MODULES_EXPORT NavigatorGamepad final : public GarbageCollectedFinalized<NavigatorGamepad>, public HeapSupplement<Navigator>, public DOMWindowProperty, public PlatformEventController, public DOMWindowLifecycleObserver {
    USING_GARBAGE_COLLECTED_MIXIN(NavigatorGamepad);
public:
    static NavigatorGamepad* from(Document&);
    static NavigatorGamepad& from(Navigator&);
    ~NavigatorGamepad() override;

    static GamepadList* getGamepads(Navigator&);
    GamepadList* gamepads();

    DECLARE_VIRTUAL_TRACE();

    void didConnectOrDisconnectGamepad(unsigned index, const WebGamepad&, bool connected);

private:
    explicit NavigatorGamepad(LocalFrame*);

    static const char* supplementName();

    void dispatchOneEvent();
    void didRemoveGamepadEventListeners();
    bool startUpdatingIfAttached();

    // DOMWindowProperty
    void willDestroyGlobalObjectInFrame() override;
    void willDetachGlobalObjectFromFrame() override;

    // PlatformEventController
    void registerWithDispatcher() override;
    void unregisterWithDispatcher() override;
    bool hasLastData() override;
    void didUpdateData() override;
    void pageVisibilityChanged() override;

    // DOMWindowLifecycleObserver
    void didAddEventListener(LocalDOMWindow*, const AtomicString&) override;
    void didRemoveEventListener(LocalDOMWindow*, const AtomicString&) override;
    void didRemoveAllEventListeners(LocalDOMWindow*) override;

    Member<GamepadList> m_gamepads;
    HeapDeque<Member<Gamepad>> m_pendingEvents;
    AsyncMethodRunner<NavigatorGamepad> m_dispatchOneEventRunner;
};

} // namespace blink

#endif // NavigatorGamepad_h
