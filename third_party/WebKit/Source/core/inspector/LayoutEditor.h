// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef LayoutEditor_h
#define LayoutEditor_h

#include "core/CSSPropertyNames.h"
#include "core/CoreExport.h"
#include "core/dom/Node.h"
#include "core/inspector/InspectorOverlayHost.h"
#include "platform/heap/Handle.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/RefPtr.h"
#include "wtf/text/WTFString.h"

namespace blink {

class JSONObject;
class InspectorCSSAgent;
class CSSPrimitiveValue;

class CORE_EXPORT LayoutEditor final: public NoBaseWillBeGarbageCollectedFinalized<LayoutEditor>, public InspectorOverlayHost::LayoutEditorListener {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(LayoutEditor);
public:
    static PassOwnPtrWillBeRawPtr<LayoutEditor> create(InspectorCSSAgent* cssAgent)
    {
        return adoptPtrWillBeNoop(new LayoutEditor(cssAgent));
    }

    void setNode(Node*);
    PassRefPtr<JSONObject> buildJSONInfo() const;

    DECLARE_VIRTUAL_TRACE();

private:
    explicit LayoutEditor(InspectorCSSAgent*);
    RefPtrWillBeRawPtr<CSSPrimitiveValue> getPropertyCSSValue(CSSPropertyID) const;
    PassRefPtr<JSONObject> createValueDescription(const String&) const;

    // InspectorOverlayHost::LayoutEditorListener implementation.
    void overlayStartedPropertyChange(const String&) override;
    void overlayPropertyChanged(float) override;
    void overlayEndedPropertyChange() override;

    RefPtrWillBeMember<Node> m_node;
    RawPtrWillBeMember<InspectorCSSAgent> m_cssAgent;
    CSSPropertyID m_changingProperty;
    float m_propertyInitialValue;
};

} // namespace blink


#endif // !defined(LayoutEditor_h)
