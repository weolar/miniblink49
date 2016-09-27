/*
 * Copyright (C) 2008, 2010 Apple Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 * 3.  Neither the name of Apple Computer, Inc. ("Apple") nor the names of
 *     its contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef Location_h
#define Location_h

#include "bindings/core/v8/ScriptValue.h"
#include "bindings/core/v8/ScriptWrappable.h"
#include "core/CoreExport.h"
#include "core/dom/DOMStringList.h"
#include "core/frame/DOMWindowProperty.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefCounted.h"
#include "wtf/text/WTFString.h"

namespace blink {

class LocalDOMWindow;
class ExceptionState;
class Frame;
class KURL;

// This class corresponds to the JS Location API, which is the only DOM API besides Window that is operable
// in a RemoteFrame. Rather than making DOMWindowProperty support RemoteFrames and generating a lot
// code churn, Location is implemented as a one-off with some custom lifetime management code. Namely,
// it needs a manual call to reset() from DOMWindow::reset() to ensure it doesn't retain a stale Frame pointer.
class CORE_EXPORT Location final : public RefCountedWillBeGarbageCollected<Location>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<Location> create(Frame* frame)
    {
        return adoptRefWillBeNoop(new Location(frame));
    }

    Frame* frame() const { return m_frame.get(); }
    void reset() { m_frame = nullptr; }

    void setHref(LocalDOMWindow* callingWindow, LocalDOMWindow* enteredWindow, const String&);
    String href() const;

    void assign(LocalDOMWindow* callingWindow, LocalDOMWindow* enteredWindow, const String&);
    void replace(LocalDOMWindow* callingWindow, LocalDOMWindow* enteredWindow, const String&);
    void reload(LocalDOMWindow* callingWindow);

    void setProtocol(LocalDOMWindow* callingWindow, LocalDOMWindow* enteredWindow, const String&, ExceptionState&);
    String protocol() const;
    void setHost(LocalDOMWindow* callingWindow, LocalDOMWindow* enteredWindow, const String&);
    String host() const;
    void setHostname(LocalDOMWindow* callingWindow, LocalDOMWindow* enteredWindow, const String&);
    String hostname() const;
    void setPort(LocalDOMWindow* callingWindow, LocalDOMWindow* enteredWindow, const String&);
    String port() const;
    void setPathname(LocalDOMWindow* callingWindow, LocalDOMWindow* enteredWindow, const String&);
    String pathname() const;
    void setSearch(LocalDOMWindow* callingWindow, LocalDOMWindow* enteredWindow, const String&);
    String search() const;
    void setHash(LocalDOMWindow* callingWindow, LocalDOMWindow* enteredWindow, const String&);
    String hash() const;
    String origin() const;

    PassRefPtrWillBeRawPtr<DOMStringList> ancestorOrigins() const;

    // Just return the |this| object the way the normal valueOf function on the Object prototype would.
    // The valueOf function is only added to make sure that it cannot be overwritten on location
    // objects, since that would provide a hook to change the string conversion behavior of location objects.
    ScriptValue valueOf(const ScriptValue& thisObject) { return thisObject; }

    DECLARE_VIRTUAL_TRACE();

private:
    explicit Location(Frame*);

    enum class SetLocation { Normal, ReplaceThisFrame };
    void setLocation(const String&, LocalDOMWindow* callingWindow, LocalDOMWindow* enteredWindow, SetLocation = SetLocation::Normal);

    const KURL& url() const;

    RawPtrWillBeMember<Frame> m_frame;
};

} // namespace blink

#endif // Location_h
