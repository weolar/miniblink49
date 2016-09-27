/*
 * Copyright (C) 2011 Google, Inc. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY GOOGLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef DOMWindowProperty_h
#define DOMWindowProperty_h

#include "core/CoreExport.h"
#include "platform/heap/Handle.h"

namespace blink {

class LocalDOMWindow;
class LocalFrame;

class CORE_EXPORT DOMWindowProperty : public WillBeGarbageCollectedMixin {
public:
    explicit DOMWindowProperty(LocalFrame*);

    virtual void willDestroyGlobalObjectInFrame();
    virtual void willDetachGlobalObjectFromFrame();

    LocalFrame* frame() const { return m_frame; }

    EAGERLY_FINALIZE_WILL_BE_REMOVED();
    DECLARE_VIRTUAL_TRACE();

protected:
    // TODO(Oilpan): when ~DOMWindowProperty is removed, check classes that derive
    // from it. Several will then be able to derive from GarbageCollected<> instead.
#if !ENABLE(OILPAN)
    virtual ~DOMWindowProperty();
#endif

    RawPtrWillBeMember<LocalFrame> m_frame;

#if !ENABLE(OILPAN)
private:
    LocalDOMWindow* m_associatedDOMWindow;
#endif
};

} // namespace blink

#endif // DOMWindowProperty_h
