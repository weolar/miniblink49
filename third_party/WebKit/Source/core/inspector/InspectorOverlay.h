/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
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

#ifndef InspectorOverlay_h
#define InspectorOverlay_h

#include "core/CoreExport.h"
#include "core/inspector/InspectorHighlight.h"
#include "platform/geometry/FloatQuad.h"
#include "platform/heap/Handle.h"
#include "wtf/Forward.h"

namespace blink {

class LayoutEditor;
struct InspectorHighlightConfig;

class InspectorOverlay : public WillBeGarbageCollectedMixin {
public:
    virtual ~InspectorOverlay() { }

    virtual void update() = 0;
    virtual void setPausedInDebuggerMessage(const String*) = 0;
    virtual void setInspectModeEnabled(bool) = 0;
    virtual void hideHighlight() = 0;
    virtual void highlightNode(Node*, Node* eventTarget, const InspectorHighlightConfig&, bool omitTooltip) = 0;
    virtual void highlightQuad(PassOwnPtr<FloatQuad>, const InspectorHighlightConfig&) = 0;
    virtual void showAndHideViewSize(bool showGrid) = 0;
    virtual void suspendUpdates() = 0;
    virtual void resumeUpdates() = 0;
    virtual void clear() = 0;
    virtual void setLayoutEditor(PassOwnPtrWillBeRawPtr<LayoutEditor>) = 0;

    DEFINE_INLINE_VIRTUAL_TRACE() { }

    class CORE_EXPORT Listener : public WillBeGarbageCollectedMixin {
    public:
        virtual ~Listener() { }
        virtual void overlayResumed() = 0;
        virtual void overlaySteppedOver() = 0;
    };
    virtual void setListener(Listener*) = 0;
};

} // namespace blink


#endif // InspectorOverlay_h
