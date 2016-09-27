/*
 * Copyright (C) 2008 Apple Inc. All rights reserved.
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

#ifndef AXListBoxOption_h
#define AXListBoxOption_h

#include "core/html/HTMLElement.h"
#include "modules/accessibility/AXLayoutObject.h"
#include "wtf/Forward.h"

namespace blink {

class AXObjectCacheImpl;
class HTMLSelectElement;

class AXListBoxOption final : public AXLayoutObject {

private:
    AXListBoxOption(LayoutObject*, AXObjectCacheImpl&);

public:
    static PassRefPtrWillBeRawPtr<AXListBoxOption> create(LayoutObject*, AXObjectCacheImpl&);
    ~AXListBoxOption() override;

    bool isAXListBoxOption() const override { return true; }
    AccessibilityRole determineAccessibilityRole() final;
    bool isSelected() const override;
    bool isEnabled() const override;
    bool isSelectedOptionActive() const override;
    void setSelected(bool) override;
    bool canSetSelectedAttribute() const override;
    String stringValue() const override;
    String deprecatedTitle(TextUnderElementMode) const override { return String(); }

private:
    bool canHaveChildren() const override { return false; }
    bool computeAccessibilityIsIgnored(IgnoredReasons* = nullptr) const override;

    HTMLSelectElement* listBoxOptionParentNode() const;
    int listBoxOptionIndex() const;
    AXObject* listBoxOptionAXObject(HTMLElement*) const;
    bool isParentPresentationalRole() const;
};

DEFINE_AX_OBJECT_TYPE_CASTS(AXListBoxOption, isAXListBoxOption());

} // namespace blink

#endif // AXListBoxOption_h
