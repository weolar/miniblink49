// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef InspectorAccessibilityAgent_h
#define InspectorAccessibilityAgent_h

#include "core/InspectorFrontend.h"
#include "core/inspector/InspectorBaseAgent.h"
#include "modules/ModulesExport.h"
#include "wtf/PassOwnPtr.h"

namespace blink {

class Page;

class MODULES_EXPORT InspectorAccessibilityAgent : public InspectorBaseAgent<InspectorAccessibilityAgent, InspectorFrontend::Accessibility>, public InspectorBackendDispatcher::AccessibilityCommandHandler {
    WTF_MAKE_NONCOPYABLE(InspectorAccessibilityAgent);
public:
    static PassOwnPtrWillBeRawPtr<InspectorAccessibilityAgent> create(Page* page)
    {
        return adoptPtrWillBeNoop(new InspectorAccessibilityAgent(page));
    }

    // Base agent methods.
    DECLARE_VIRTUAL_TRACE();

    // Protocol methods.
    void getAXNode(ErrorString*, int nodeId, RefPtr<TypeBuilder::Accessibility::AXNode>& accessibilityNode) override;

private:
    explicit InspectorAccessibilityAgent(Page*);

    RawPtrWillBeMember<Page> m_page;
};

}

#endif // InspectorAccessibilityAgent_h
