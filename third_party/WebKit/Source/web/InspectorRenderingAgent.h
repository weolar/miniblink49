// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef InspectorRenderingAgent_h
#define InspectorRenderingAgent_h

#include "core/InspectorFrontend.h"
#include "core/inspector/InspectorBaseAgent.h"

namespace blink {

class WebViewImpl;

using ErrorString = String;

class InspectorRenderingAgent final : public InspectorBaseAgent<InspectorRenderingAgent, InspectorFrontend::Rendering>, public InspectorBackendDispatcher::RenderingCommandHandler {
    WTF_MAKE_NONCOPYABLE(InspectorRenderingAgent);
public:
    static PassOwnPtrWillBeRawPtr<InspectorRenderingAgent> create(WebViewImpl*);

    // InspectorBackendDispatcher::PageCommandHandler implementation.
    void setShowPaintRects(ErrorString*, bool show) override;
    void setShowDebugBorders(ErrorString*, bool show) override;
    void setShowFPSCounter(ErrorString*, bool show) override;
    void setContinuousPaintingEnabled(ErrorString*, bool enabled) override;
    void setShowScrollBottleneckRects(ErrorString*, bool show) override;

    // InspectorBaseAgent overrides.
    void disable(ErrorString*) override;
    void restore() override;

    DECLARE_VIRTUAL_TRACE();

private:
    explicit InspectorRenderingAgent(WebViewImpl*);
    bool compositingEnabled(ErrorString*);

    WebViewImpl* m_webViewImpl;
};


} // namespace blink


#endif // !defined(InspectorRenderingAgent_h)
