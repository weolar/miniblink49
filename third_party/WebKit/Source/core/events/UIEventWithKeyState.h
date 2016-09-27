/*
 * Copyright (C) 2001 Peter Kelly (pmk@post.com)
 * Copyright (C) 2001 Tobias Anton (anton@stud.fbi.fh-darmstadt.de)
 * Copyright (C) 2006 Samuel Weinig (sam.weinig@gmail.com)
 * Copyright (C) 2003, 2004, 2005, 2006, 2008 Apple Inc. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 *
 */

#ifndef UIEventWithKeyState_h
#define UIEventWithKeyState_h

#include "core/CoreExport.h"
#include "core/events/UIEvent.h"

namespace blink {

    class CORE_EXPORT UIEventWithKeyState : public UIEvent {
    public:
        bool ctrlKey() const { return m_ctrlKey; }
        bool shiftKey() const { return m_shiftKey; }
        bool altKey() const { return m_altKey; }
        bool metaKey() const { return m_metaKey; }

        // We ignore the new tab modifiers (ctrl or meta, depending on OS) set by JavaScript when processing events.
        // However, scripts running in isolated worlds (aka content scripts) are not subject to this restriction. Since it is possible that an event created by a content script is caught and recreated by the web page's script, we resort to a global flag.
        static bool newTabModifierSetFromIsolatedWorld() { return s_newTabModifierSetFromIsolatedWorld; }
        static void clearNewTabModifierSetFromIsolatedWorld() { s_newTabModifierSetFromIsolatedWorld = false; }
        static void didCreateEventInIsolatedWorld(bool ctrlKey, bool shiftKey, bool altKey, bool metaKey);

    protected:
        UIEventWithKeyState()
            : m_ctrlKey(false)
            , m_altKey(false)
            , m_shiftKey(false)
            , m_metaKey(false)
        {
        }

        UIEventWithKeyState(const AtomicString& type, bool canBubble, bool cancelable, PassRefPtrWillBeRawPtr<AbstractView> view,
            int detail, bool ctrlKey, bool altKey, bool shiftKey, bool metaKey, InputDevice* sourceDevice = nullptr)
            : UIEvent(type, canBubble, cancelable, view, detail, sourceDevice)
            , m_ctrlKey(ctrlKey)
            , m_altKey(altKey)
            , m_shiftKey(shiftKey)
            , m_metaKey(metaKey)
        {
        }

        // Expose these so init functions can set them.
        bool m_ctrlKey : 1;
        bool m_altKey : 1;
        bool m_shiftKey : 1;
        bool m_metaKey : 1;

    private:
        static bool s_newTabModifierSetFromIsolatedWorld;
    };

    UIEventWithKeyState* findEventWithKeyState(Event*);

} // namespace blink

#endif // UIEventWithKeyState_h
