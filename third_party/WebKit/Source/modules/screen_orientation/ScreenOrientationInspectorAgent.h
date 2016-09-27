// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ScreenOrientationInspectorAgent_h
#define ScreenOrientationInspectorAgent_h

#include "core/InspectorFrontend.h"
#include "core/inspector/InspectorBaseAgent.h"
#include "modules/ModulesExport.h"
#include "wtf/text/WTFString.h"

namespace blink {

class ScreenOrientationController;

typedef String ErrorString;

class MODULES_EXPORT ScreenOrientationInspectorAgent final : public InspectorBaseAgent<ScreenOrientationInspectorAgent, InspectorFrontend::ScreenOrientation>, public InspectorBackendDispatcher::ScreenOrientationCommandHandler {
    WTF_MAKE_NONCOPYABLE(ScreenOrientationInspectorAgent);
public:
    static PassOwnPtrWillBeRawPtr<ScreenOrientationInspectorAgent> create(LocalFrame&);

    ~ScreenOrientationInspectorAgent() override;

    // Protocol methods.
    void setScreenOrientationOverride(ErrorString*, int, const String&) override;
    void clearScreenOrientationOverride(ErrorString*) override;

    // InspectorBaseAgent overrides.
    void disable(ErrorString*) override;
    void restore() override;

private:
    explicit ScreenOrientationInspectorAgent(LocalFrame&);
    LocalFrame& m_frame;
};

} // namespace blink


#endif // !defined(ScreenOrientationInspectorAgent_h)
