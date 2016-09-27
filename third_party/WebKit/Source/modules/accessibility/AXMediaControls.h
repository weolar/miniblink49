/*
 * Copyright (C) 2009 Apple Inc. All rights reserved.
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


#ifndef AXMediaControls_h
#define AXMediaControls_h

#include "core/html/shadow/MediaControlElements.h"
#include "modules/accessibility/AXSlider.h"

namespace blink {

class AXObjectCacheImpl;

class AccessibilityMediaControl : public AXLayoutObject {

public:
    static PassRefPtrWillBeRawPtr<AXObject> create(LayoutObject*, AXObjectCacheImpl&);
    ~AccessibilityMediaControl() override { }

    AccessibilityRole roleValue() const override;

    String deprecatedTitle(TextUnderElementMode) const final;
    String deprecatedAccessibilityDescription() const override;
    String deprecatedHelpText() const override;

protected:
    AccessibilityMediaControl(LayoutObject*, AXObjectCacheImpl&);
    MediaControlElementType controlType() const;
    bool computeAccessibilityIsIgnored(IgnoredReasons* = nullptr) const override;
};


class AccessibilityMediaTimeline final : public AXSlider {

public:
    static PassRefPtrWillBeRawPtr<AXObject> create(LayoutObject*, AXObjectCacheImpl&);
    ~AccessibilityMediaTimeline() override { }

    String deprecatedHelpText() const override;
    String valueDescription() const override;
    const AtomicString& getAttribute(const QualifiedName& attribute) const;

private:
    AccessibilityMediaTimeline(LayoutObject*, AXObjectCacheImpl&);
};


class AXMediaControlsContainer final : public AccessibilityMediaControl {

public:
    static PassRefPtrWillBeRawPtr<AXObject> create(LayoutObject*, AXObjectCacheImpl&);
    ~AXMediaControlsContainer() override { }

    AccessibilityRole roleValue() const override { return ToolbarRole; }

    String deprecatedHelpText() const override;
    String deprecatedAccessibilityDescription() const override;

private:
    AXMediaControlsContainer(LayoutObject*, AXObjectCacheImpl&);
    bool controllingVideoElement() const;
    bool computeAccessibilityIsIgnored(IgnoredReasons* = nullptr) const override;
};


class AccessibilityMediaTimeDisplay final : public AccessibilityMediaControl {

public:
    static PassRefPtrWillBeRawPtr<AXObject> create(LayoutObject*, AXObjectCacheImpl&);
    ~AccessibilityMediaTimeDisplay() override { }

    AccessibilityRole roleValue() const override { return StaticTextRole; }

    String stringValue() const override;
    String deprecatedAccessibilityDescription() const override;

private:
    AccessibilityMediaTimeDisplay(LayoutObject*, AXObjectCacheImpl&);
    bool computeAccessibilityIsIgnored(IgnoredReasons* = nullptr) const override;
};


} // namespace blink

#endif // AXMediaControls_h
