/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
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

#ifndef InspectorOverlayHost_h
#define InspectorOverlayHost_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "core/CoreExport.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefCounted.h"

namespace blink {

class CORE_EXPORT InspectorOverlayHost final : public RefCountedWillBeGarbageCollectedFinalized<InspectorOverlayHost>, public ScriptWrappable {
    DEFINE_WRAPPERTYPEINFO();
public:
    static PassRefPtrWillBeRawPtr<InspectorOverlayHost> create()
    {
        return adoptRefWillBeNoop(new InspectorOverlayHost());
    }
    ~InspectorOverlayHost();
    DECLARE_TRACE();

    void resume();
    void stepOver();
    void startPropertyChange(const String&);
    void changeProperty(float delta);
    void endPropertyChange();

    class DebuggerListener : public WillBeGarbageCollectedMixin {
    public:
        virtual ~DebuggerListener() { }
        virtual void overlayResumed() = 0;
        virtual void overlaySteppedOver() = 0;
    };
    void setDebuggerListener(DebuggerListener* listener) { m_debuggerListener = listener; }

    class CORE_EXPORT LayoutEditorListener : public WillBeGarbageCollectedMixin {
    public:
        virtual ~LayoutEditorListener() { }
        virtual void overlayStartedPropertyChange(const String&) = 0;
        virtual void overlayPropertyChanged(float cssDelta) = 0;
        virtual void overlayEndedPropertyChange() = 0;
    };
    void setLayoutEditorListener(LayoutEditorListener* listener) { m_layoutEditorListener = listener; }

private:
    InspectorOverlayHost();

    RawPtrWillBeMember<DebuggerListener> m_debuggerListener;
    RawPtrWillBeMember<LayoutEditorListener> m_layoutEditorListener;

};

} // namespace blink

#endif // InspectorOverlayHost_h
