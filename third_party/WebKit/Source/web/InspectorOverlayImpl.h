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

#ifndef InspectorOverlayImpl_h
#define InspectorOverlayImpl_h

#include "core/InspectorTypeBuilder.h"
#include "core/inspector/InspectorOverlay.h"
#include "core/inspector/InspectorOverlayHost.h"
#include "platform/Timer.h"
#include "platform/geometry/FloatQuad.h"
#include "platform/geometry/LayoutRect.h"
#include "platform/graphics/Color.h"
#include "platform/heap/Handle.h"
#include "public/web/WebInputEvent.h"
#include "public/web/WebPageOverlay.h"
#include "wtf/OwnPtr.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/RefPtr.h"
#include "wtf/text/WTFString.h"

namespace blink {

class Color;
class EmptyChromeClient;
class LocalFrame;
class GraphicsContext;
class JSONValue;
class Node;
class Page;
class WebViewImpl;
class LayoutEditor;

class InspectorOverlayImpl final : public NoBaseWillBeGarbageCollectedFinalized<InspectorOverlayImpl>, public InspectorOverlay, public WebPageOverlay, public InspectorOverlayHost::DebuggerListener {
    WTF_MAKE_FAST_ALLOCATED_WILL_BE_REMOVED(InspectorOverlayImpl);
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(InspectorOverlayImpl);
public:
    // FIXME: remove this once overlay works for non-main frame.
    static PassOwnPtrWillBeRawPtr<InspectorOverlay> createEmpty();

    static PassOwnPtrWillBeRawPtr<InspectorOverlayImpl> create(WebViewImpl* webViewImpl)
    {
        return adoptPtrWillBeNoop(new InspectorOverlayImpl(webViewImpl));
    }

    ~InspectorOverlayImpl() override;
    DECLARE_TRACE();

    // InspectorOverlay implementation.
    void update() override;
    void setPausedInDebuggerMessage(const String*) override;
    void setInspectModeEnabled(bool) override;
    void hideHighlight() override;
    void highlightNode(Node*, Node* eventTarget, const InspectorHighlightConfig&, bool omitTooltip) override;
    void highlightQuad(PassOwnPtr<FloatQuad>, const InspectorHighlightConfig&) override;
    void showAndHideViewSize(bool showGrid) override;
    void setListener(InspectorOverlay::Listener* listener) override 
    {
#ifndef MINIBLINK_NO_DEVTOOLS
        m_listener = listener; 
#endif // MINIBLINK_NO_DEVTOOLS
    }
    void suspendUpdates() override;
    void resumeUpdates() override;
    void clear() override;
    void setLayoutEditor(PassOwnPtrWillBeRawPtr<LayoutEditor>) override;

    bool handleInputEvent(const WebInputEvent&);
    void invalidate();
private:
    explicit InspectorOverlayImpl(WebViewImpl*);

    // InspectorOverlayHost::DebuggerListener implementation.
    void overlayResumed() override;
    void overlaySteppedOver() override;

    // WebPageOverlay implementation.
    void paintPageOverlay(WebGraphicsContext*, const WebSize& webViewSize) override;

    bool isEmpty();
    void drawNodeHighlight();
    void drawQuadHighlight();
    void drawPausedInDebuggerMessage();
    void drawViewSize();

    Page* overlayPage();
    LocalFrame* overlayMainFrame();
    void reset(const IntSize& viewportSize, const IntPoint& documentScrollOffset);
    void evaluateInOverlay(const String& method, const String& argument);
    void evaluateInOverlay(const String& method, PassRefPtr<JSONValue> argument);
    void onTimer(Timer<InspectorOverlayImpl>*);
#ifndef MINIBLINK_NO_DEVTOOLS
    WebViewImpl* m_webViewImpl;
    String m_pausedInDebuggerMessage;
    bool m_inspectModeEnabled;
    RefPtrWillBeMember<Node> m_highlightNode;
    RefPtrWillBeMember<Node> m_eventTargetNode;
    InspectorHighlightConfig m_nodeHighlightConfig;
    OwnPtr<FloatQuad> m_highlightQuad;
    OwnPtrWillBeMember<Page> m_overlayPage;
    OwnPtr<EmptyChromeClient> m_overlayChromeClient;
    RefPtrWillBeMember<InspectorOverlayHost> m_overlayHost;
    InspectorHighlightConfig m_quadHighlightConfig;
    bool m_drawViewSize;
    bool m_drawViewSizeWithGrid;
    bool m_omitTooltip;
    Timer<InspectorOverlayImpl> m_timer;
    int m_suspendCount;
    bool m_updating;
    RawPtrWillBeMember<InspectorOverlay::Listener> m_listener;
    OwnPtrWillBeMember<LayoutEditor> m_layoutEditor;
#endif // MINIBLINK_NO_DEVTOOLS
};

} // namespace blink


#endif // InspectorOverlayImpl_h
