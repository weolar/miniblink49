/*
 * Copyright (C) 2011 Apple Inc. All rights reserved.
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

#ifndef AXSpinButton_h
#define AXSpinButton_h

#include "core/html/shadow/SpinButtonElement.h"
#include "modules/accessibility/AXMockObject.h"


namespace blink {

class AXObjectCacheImpl;

class AXSpinButton final : public AXMockObject {
public:
    static PassRefPtrWillBeRawPtr<AXSpinButton> create(AXObjectCacheImpl&);
    ~AXSpinButton() override;
    DECLARE_VIRTUAL_TRACE();

    void setSpinButtonElement(SpinButtonElement* spinButton) { m_spinButtonElement = spinButton; }
    void step(int amount);

private:
    explicit AXSpinButton(AXObjectCacheImpl&);

    AccessibilityRole roleValue() const override;
    bool isSpinButton() const override { return true; }
    bool isNativeSpinButton() const override { return true; }
    void addChildren() override;
    LayoutRect elementRect() const override;
    void detach() override;
    void detachFromParent() override;

    RawPtrWillBeMember<SpinButtonElement> m_spinButtonElement;
};

class AXSpinButtonPart final : public AXMockObject {
public:
    static PassRefPtrWillBeRawPtr<AXSpinButtonPart> create(AXObjectCacheImpl&);
    ~AXSpinButtonPart() override { }

    bool isIncrementor() const { return m_isIncrementor; }
    void setIsIncrementor(bool value) { m_isIncrementor = value; }

private:
    explicit AXSpinButtonPart(AXObjectCacheImpl&);
    bool m_isIncrementor : 1;

    bool press() const override;
    AccessibilityRole roleValue() const override { return ButtonRole; }
    bool isSpinButtonPart() const override { return true; }
    LayoutRect elementRect() const override;
};

DEFINE_AX_OBJECT_TYPE_CASTS(AXSpinButton, isNativeSpinButton());
DEFINE_AX_OBJECT_TYPE_CASTS(AXSpinButtonPart, isSpinButtonPart());

} // namespace blink

#endif // AXSpinButton_h
