/*
 * Copyright (C) 2010 Apple Inc. All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef AXMenuListOption_h
#define AXMenuListOption_h

#include "core/html/HTMLOptionElement.h"
#include "modules/accessibility/AXMockObject.h"

namespace blink {

class AXObjectCacheImpl;

class AXMenuListOption final : public AXMockObject {
public:
    static PassRefPtrWillBeRawPtr<AXMenuListOption> create(HTMLOptionElement* element, AXObjectCacheImpl& axObjectCache) { return adoptRefWillBeNoop(new AXMenuListOption(element, axObjectCache)); }
    ~AXMenuListOption() override;

private:
    AXMenuListOption(HTMLOptionElement*, AXObjectCacheImpl&);
    DECLARE_VIRTUAL_TRACE();

    bool isMenuListOption() const override { return true; }

    Node* node() const override { return m_element; }
    void detach() override;
    bool isDetached() const override { return !m_element; }
    AccessibilityRole roleValue() const override { return MenuListOptionRole; }
    bool canHaveChildren() const override { return false; }

    Element* actionElement() const override;
    bool isEnabled() const override;
    bool isVisible() const override;
    bool isOffScreen() const override;
    bool isSelected() const override;
    void setSelected(bool) override;
    bool canSetSelectedAttribute() const override;
    LayoutRect elementRect() const override;
    String stringValue() const override;
    bool computeAccessibilityIsIgnored(IgnoredReasons* = nullptr) const override;

    RawPtrWillBeMember<HTMLOptionElement> m_element;
};

DEFINE_AX_OBJECT_TYPE_CASTS(AXMenuListOption, isMenuListOption());

} // namespace blink

#endif // AXMenuListOption_h
