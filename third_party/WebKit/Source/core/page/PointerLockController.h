/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. AND ITS CONTRIBUTORS ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE INC. OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef PointerLockController_h
#define PointerLockController_h

#include "core/CoreExport.h"
#include "platform/heap/Handle.h"
#include "wtf/RefPtr.h"
#include "wtf/text/AtomicString.h"

namespace blink {

class Element;
class Document;
class Page;
class PlatformMouseEvent;

class CORE_EXPORT PointerLockController final : public NoBaseWillBeGarbageCollected<PointerLockController> {
    WTF_MAKE_NONCOPYABLE(PointerLockController);
    WTF_MAKE_FAST_ALLOCATED_WILL_BE_REMOVED(PointerLockController);
public:
    static PassOwnPtrWillBeRawPtr<PointerLockController> create(Page*);

    void requestPointerLock(Element* target);
    void requestPointerUnlock();
    void elementRemoved(Element*);
    void documentDetached(Document*);
    bool lockPending() const;
    Element* element() const;

    void didAcquirePointerLock();
    void didNotAcquirePointerLock();
    void didLosePointerLock();
    void dispatchLockedMouseEvent(const PlatformMouseEvent&, const AtomicString& eventType);

    DECLARE_TRACE();

private:
    explicit PointerLockController(Page*);
    void clearElement();
    void enqueueEvent(const AtomicString& type, Element*);
    void enqueueEvent(const AtomicString& type, Document*);

    RawPtrWillBeMember<Page> m_page;
    bool m_lockPending;
    RefPtrWillBeMember<Element> m_element;
    RefPtrWillBeMember<Document> m_documentOfRemovedElementWhileWaitingForUnlock;
};

} // namespace blink

#endif // PointerLockController_h
