// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef InspectorEmulationAgent_h
#define InspectorEmulationAgent_h

#include "core/InspectorFrontend.h"
#include "core/inspector/InspectorBaseAgent.h"

namespace blink {

class WebViewImpl;

using ErrorString = String;

class InspectorEmulationAgent final : public InspectorBaseAgent<InspectorEmulationAgent, InspectorFrontend::Emulation>, public InspectorBackendDispatcher::EmulationCommandHandler {
    WTF_MAKE_NONCOPYABLE(InspectorEmulationAgent);
public:
    static PassOwnPtrWillBeRawPtr<InspectorEmulationAgent> create(WebViewImpl*);
    ~InspectorEmulationAgent() override;

    void viewportChanged();

    // InspectorBackendDispatcher::EmulationCommandHandler implementation.
    void resetScrollAndPageScaleFactor(ErrorString*) override;
    void setPageScaleFactor(ErrorString*, double pageScaleFactor) override;
    void setScriptExecutionDisabled(ErrorString*, bool) override;
    void setTouchEmulationEnabled(ErrorString*, bool enabled, const String* configuration) override;
    void setEmulatedMedia(ErrorString*, const String&) override;

    // InspectorBaseAgent overrides.
    void disable(ErrorString*) override;
    void restore() override;
    void discardAgent() override;
    void didCommitLoadForLocalFrame(LocalFrame*) override;

    DECLARE_VIRTUAL_TRACE();

private:
    explicit InspectorEmulationAgent(WebViewImpl*);

    WebViewImpl* m_webViewImpl;
};


} // namespace blink


#endif // !defined(InspectorEmulationAgent_h)
