// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef StyleChangeReason_h
#define StyleChangeReason_h

#include "core/dom/QualifiedName.h"
#include "wtf/NullPtr.h"
#include "wtf/PassRefPtr.h"
#include "wtf/text/AtomicString.h"
#include "wtf/text/WTFString.h"

namespace blink {

class QualifiedName;

namespace StyleChangeReason {
extern const char ActiveStylesheetsUpdate[];
extern const char Animation[];
extern const char Attribute[];
extern const char CompositorProxy[];
extern const char ControlValue[];
extern const char Control[];
extern const char DesignMode[];
extern const char Drag[];
extern const char FontSizeChange[];
extern const char Fonts[];
extern const char FullScreen[];
extern const char Inline[];
extern const char InlineCSSStyleMutated[];
extern const char Inspector[];
extern const char Language[];
extern const char LinkColorChange[];
extern const char PlatformColorChange[];
extern const char PropagateInheritChangeToDistributedNodes[];
extern const char PseudoClass[];
extern const char SVGContainerSizeChange[];
extern const char SVGCursor[];
extern const char SVGFilterLayerUpdate[];
extern const char Shadow[];
extern const char SiblingSelector[];
extern const char StyleInvalidator[];
extern const char StyleSheetChange[];
extern const char Validate[];
extern const char ViewportUnits[];
extern const char VisitedLink[];
extern const char VisuallyOrdered[];
extern const char WritingModeChange[];
extern const char Zoom[];
}
typedef const char StyleChangeReasonString[];

namespace StyleChangeExtraData {
extern const AtomicString& Active;
extern const AtomicString& Disabled;
extern const AtomicString& Focus;
extern const AtomicString& Hover;
extern const AtomicString& Past;
extern const AtomicString& Unresolved;

void init();
}

// |StyleChangeReasonForTracing| is used to trace the reason a
// |Node::setNeedsStyleRecalc| call was made to show it in DevTools or in
// about:tracing.
// |StyleChangeReasonForTracing| is strictly only for the tracing purpose as
// described above. Blink logic must not depend on this value.
class StyleChangeReasonForTracing {
public:
    static StyleChangeReasonForTracing create(StyleChangeReasonString reasonString)
    {
        return StyleChangeReasonForTracing(reasonString, nullAtom);
    }

    static StyleChangeReasonForTracing createWithExtraData(StyleChangeReasonString reasonString, const AtomicString& extraData)
    {
        return StyleChangeReasonForTracing(reasonString, extraData);
    }

    static StyleChangeReasonForTracing fromAttribute(const QualifiedName& attributeName)
    {
        return StyleChangeReasonForTracing(StyleChangeReason::Attribute, attributeName.localName());
    }

    String reasonString() const { return String(m_reason); }
    const AtomicString& extraData() const { return m_extraData; }

private:
    StyleChangeReasonForTracing(StyleChangeReasonString reasonString, const AtomicString& extraData)
        : m_reason(reasonString)
        , m_extraData(extraData)
    {
    }

    // disable comparisons
    void operator==(const StyleChangeReasonForTracing&) const { }
    void operator!=(const StyleChangeReasonForTracing&) const { }

    const char* m_reason;
    AtomicString m_extraData;
};

} // namespace blink

#endif // StyleChangeReason_h
