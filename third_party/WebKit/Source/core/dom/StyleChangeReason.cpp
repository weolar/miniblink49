// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/dom/StyleChangeReason.h"

#include "platform/TraceEvent.h"
#include "wtf/MainThread.h"
#include "wtf/StaticConstructors.h"

namespace blink {

namespace StyleChangeReason {
const char ActiveStylesheetsUpdate[] = "ActiveStylesheetsUpdate";
const char Animation[] = "Animation";
const char Attribute[] = "Attribute";
const char CompositorProxy[] = "CompositorProxy";
const char ControlValue[] = "ControlValue";
const char Control[] = "Control";
const char DesignMode[] = "DesignMode";
const char Drag[] = "Drag";
const char FontSizeChange[] = "FontSizeChange";
const char Fonts[] = "Fonts";
const char FullScreen[] = "FullScreen";
const char Inline[] = "Inline";
const char InlineCSSStyleMutated[] = "Inline CSS style declaration was mutated";
const char Inspector[] = "Inspector";
const char Language[] = "Language";
const char LinkColorChange[] = "LinkColorChange";
const char PlatformColorChange[] = "PlatformColorChange";
const char PropagateInheritChangeToDistributedNodes[] = "PropagateInheritChangeToDistributedNodes";
const char PseudoClass[] = "PseudoClass";
const char SVGContainerSizeChange[] = "SVGContainerSizeChange";
const char SVGCursor[] = "SVGCursor";
const char SVGFilterLayerUpdate[] = "SVGFilterLayerUpdate";
const char Shadow[] = "Shadow";
const char SiblingSelector[] = "SiblingSelector";
const char StyleInvalidator[] = "StyleInvalidator";
const char StyleSheetChange[] = "StyleSheetChange";
const char Validate[] = "Validate";
const char ViewportUnits[] = "ViewportUnits";
const char VisitedLink[] = "VisitedLink";
const char VisuallyOrdered[] = "VisuallyOrdered";
const char WritingModeChange[] = "WritingModeChange";
const char Zoom[] = "Zoom";
} // namespace StyleChangeReasonForTracing

namespace StyleChangeExtraData {
DEFINE_GLOBAL(AtomicString, Active)
DEFINE_GLOBAL(AtomicString, Disabled)
DEFINE_GLOBAL(AtomicString, Focus)
DEFINE_GLOBAL(AtomicString, Hover)
DEFINE_GLOBAL(AtomicString, Past)
DEFINE_GLOBAL(AtomicString, Unresolved)

void init()
{
    ASSERT(isMainThread());

    new (NotNull, (void*)&Active) AtomicString(":active", AtomicString::ConstructFromLiteral);
    new (NotNull, (void*)&Disabled) AtomicString(":disabled", AtomicString::ConstructFromLiteral);
    new (NotNull, (void*)&Focus) AtomicString(":focus", AtomicString::ConstructFromLiteral);
    new (NotNull, (void*)&Hover) AtomicString(":hover", AtomicString::ConstructFromLiteral);
    new (NotNull, (void*)&Past) AtomicString(":past", AtomicString::ConstructFromLiteral);
    new (NotNull, (void*)&Unresolved) AtomicString(":unresolved", AtomicString::ConstructFromLiteral);
}

} // namespace StyleChangeExtraData

} // namespace blink
