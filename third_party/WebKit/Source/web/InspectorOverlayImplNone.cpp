/*
* Copyright (C) 2011 weolar Inc. All rights reserved.
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

#include "config.h"
#include "web/InspectorOverlayImpl.h"
#include "third_party/WebKit/Source/core/page/Page.h"
#include "third_party/WebKit/Source/core/inspector/LayoutEditor.h"
#include "third_party/WebKit/Source/core/loader/EmptyClients.h"

namespace blink {
InspectorOverlayImpl::InspectorOverlayImpl(WebViewImpl* webViewImpl)
{

}

InspectorOverlayImpl::~InspectorOverlayImpl()
{
}

void InspectorOverlayImpl::onTimer(Timer<InspectorOverlayImpl>*)
{
}

bool InspectorOverlayImpl::handleInputEvent(const WebInputEvent& inputEvent)
{
    return false;
}

void InspectorOverlayImpl::update()
{
}

void InspectorOverlayImpl::setPausedInDebuggerMessage(const String* message)
{
}

void InspectorOverlayImpl::setInspectModeEnabled(bool enabled)
{
}

void InspectorOverlayImpl::hideHighlight()
{
}

void InspectorOverlayImpl::highlightNode(Node* node, Node* eventTarget, const InspectorHighlightConfig& highlightConfig, bool omitTooltip)
{
}

void InspectorOverlayImpl::highlightQuad(PassOwnPtr<FloatQuad> quad, const InspectorHighlightConfig& highlightConfig)
{
}

void InspectorOverlayImpl::showAndHideViewSize(bool showGrid)
{
}

bool InspectorOverlayImpl::isEmpty()
{
    return true;
}

void InspectorOverlayImpl::suspendUpdates()
{
}

void InspectorOverlayImpl::resumeUpdates()
{
}

void InspectorOverlayImpl::clear()
{

}

void InspectorOverlayImpl::setLayoutEditor(PassOwnPtrWillBeRawPtr<LayoutEditor> layoutEditor)
{
}


void InspectorOverlayImpl::overlayResumed()
{
}

void InspectorOverlayImpl::overlaySteppedOver()
{
}

void InspectorOverlayImpl::paintPageOverlay(WebGraphicsContext* context, const WebSize& webViewSize)
{
}

DEFINE_TRACE(InspectorOverlayImpl)
{
    InspectorOverlay::trace(visitor);
}

namespace {

    class InspectorOverlayChromeClient final : public EmptyChromeClient {
    public:
        InspectorOverlayChromeClient(ChromeClient& client, InspectorOverlayImpl* overlay)
            : m_client(client)
            , m_overlay(overlay)
        { }

        void setCursor(const Cursor& cursor) override
        {
            m_client.setCursor(cursor);
        }

        void setToolTip(const String& tooltip, TextDirection direction) override
        {
            m_client.setToolTip(tooltip, direction);
        }

        void invalidateRect(const IntRect&) override
        {
            m_overlay->invalidate();
        }

    private:
        ChromeClient& m_client;
        InspectorOverlayImpl* m_overlay;
    };

    class InspectorOverlayStub : public NoBaseWillBeGarbageCollectedFinalized<InspectorOverlayStub>, public InspectorOverlay {
        WTF_MAKE_FAST_ALLOCATED_WILL_BE_REMOVED(InspectorOverlayStub);
        WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(InspectorOverlayStub);
    public:
        InspectorOverlayStub() { }
        DECLARE_TRACE();

        // InspectorOverlay implementation.
        void update() override { }
        void setPausedInDebuggerMessage(const String*) override { }
        void setInspectModeEnabled(bool) override { }
        void hideHighlight() override { }
        void highlightNode(Node*, Node* eventTarget, const InspectorHighlightConfig&, bool omitTooltip) override { }
        void highlightQuad(PassOwnPtr<FloatQuad>, const InspectorHighlightConfig&) override { }
        void showAndHideViewSize(bool showGrid) override { }
        void setListener(InspectorOverlay::Listener* listener) override { }
        void suspendUpdates() override { }
        void resumeUpdates() override { }
        void clear() override { }
        void setLayoutEditor(PassOwnPtrWillBeRawPtr<LayoutEditor>) override { }
    };

    DEFINE_TRACE(InspectorOverlayStub)
    {
        InspectorOverlay::trace(visitor);
    }

} // anonymous namespace

// static
PassOwnPtrWillBeRawPtr<InspectorOverlay> InspectorOverlayImpl::createEmpty()
{
    return adoptPtrWillBeNoop(new InspectorOverlayStub());
}

} // namespace blink