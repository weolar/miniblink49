/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2001 Dirk Mueller (mueller@kde.org)
 *           (C) 2006 Alexey Proskuryakov (ap@webkit.org)
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2012 Apple Inc. All rights reserved.
 * Copyright (C) 2008, 2009 Torch Mobile Inc. All rights reserved. (http://www.torchmobile.com/)
 * Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies)
 * Copyright (C) 2013 Google Inc. All rights reserved.
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

#ifndef Fullscreen_h
#define Fullscreen_h

#include "core/CoreExport.h"
#include "core/dom/Document.h"
#include "core/dom/DocumentLifecycleObserver.h"
#include "core/dom/Element.h"
#include "platform/Supplementable.h"
#include "platform/Timer.h"
#include "platform/geometry/LayoutRect.h"
#include "wtf/Deque.h"
#include "wtf/RefPtr.h"
#include "wtf/Vector.h"

namespace blink {

class LayoutFullScreen;
class ComputedStyle;

class CORE_EXPORT Fullscreen final
    : public NoBaseWillBeGarbageCollectedFinalized<Fullscreen>
    , public WillBeHeapSupplement<Document>
    , public DocumentLifecycleObserver {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(Fullscreen);
public:
    virtual ~Fullscreen();
    static const char* supplementName();
    static Fullscreen& from(Document&);
    static Fullscreen* fromIfExists(Document&);
    static Element* fullscreenElementFrom(Document&);
    static Element* currentFullScreenElementFrom(Document&);
    static bool isFullScreen(Document&);
    static bool isActiveFullScreenElement(const Element&);

    enum RequestType {
        UnprefixedRequest, // Element.requestFullscreen()
        PrefixedRequest, // Element.webkitRequestFullscreen() and webkitRequestFullScreen()
        PrefixedVideoRequest, // HTMLVideoElement.webkitEnterFullscreen() and webkitEnterFullScreen()
    };

    void requestFullscreen(Element&, RequestType);
    static void fullyExitFullscreen(Document&);
    void exitFullscreen();

    static bool fullscreenEnabled(Document&);
    Element* fullscreenElement() const { return !m_fullScreenElementStack.isEmpty() ? m_fullScreenElementStack.last().first.get() : 0; }

    void didEnterFullScreenForElement(Element*);
    void didExitFullScreenForElement(Element*);

    void setFullScreenLayoutObject(LayoutFullScreen*);
    LayoutFullScreen* fullScreenLayoutObject() const { return m_fullScreenLayoutObject; }
    void fullScreenLayoutObjectDestroyed();

    void elementRemoved(Element&);

    // Mozilla API
    Element* webkitCurrentFullScreenElement() const { return m_fullScreenElement.get(); }

    void documentWasDetached() override;
#if !ENABLE(OILPAN)
    void documentWasDisposed() override;
#endif

    DECLARE_VIRTUAL_TRACE();

private:
    static Fullscreen* fromIfExistsSlow(Document&);

    explicit Fullscreen(Document&);

    Document* document();

    void clearFullscreenElementStack();
    void popFullscreenElementStack();
    void pushFullscreenElementStack(Element&, RequestType);

    void enqueueChangeEvent(Document&, RequestType);
    void enqueueErrorEvent(Element&, RequestType);
    void eventQueueTimerFired(Timer<Fullscreen>*);

    RefPtrWillBeMember<Element> m_fullScreenElement;
    WillBeHeapVector<std::pair<RefPtrWillBeMember<Element>, RequestType>> m_fullScreenElementStack;
    LayoutFullScreen* m_fullScreenLayoutObject;
    Timer<Fullscreen> m_eventQueueTimer;
    WillBeHeapDeque<RefPtrWillBeMember<Event>> m_eventQueue;
    LayoutRect m_savedPlaceholderFrameRect;
    RefPtr<ComputedStyle> m_savedPlaceholderComputedStyle;
};

inline bool Fullscreen::isActiveFullScreenElement(const Element& element)
{
    Fullscreen* fullscreen = fromIfExists(element.document());
    if (!fullscreen)
        return false;
    return fullscreen->webkitCurrentFullScreenElement() == &element;
}

inline Fullscreen* Fullscreen::fromIfExists(Document& document)
{
    if (!document.hasFullscreenSupplement())
        return 0;
    return fromIfExistsSlow(document);
}

} // namespace blink

#endif // Fullscreen_h
