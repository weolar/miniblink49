/*
 * Copyright (C) 2013, Google Inc. All rights reserved.
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

#ifndef AXInlineTextBox_h
#define AXInlineTextBox_h

#include "core/layout/line/AbstractInlineTextBox.h"
#include "modules/accessibility/AXObject.h"

namespace blink {

class AXObjectCacheImpl;

class AXInlineTextBox final : public AXObject {

private:
    AXInlineTextBox(PassRefPtr<AbstractInlineTextBox>, AXObjectCacheImpl&);

public:
    static PassRefPtrWillBeRawPtr<AXInlineTextBox> create(PassRefPtr<AbstractInlineTextBox>, AXObjectCacheImpl&);

    void init() override;
    void detach() override;

    void setInlineTextBox(AbstractInlineTextBox* inlineTextBox) { m_inlineTextBox = inlineTextBox; }

    AccessibilityRole roleValue() const override { return InlineTextBoxRole; }
    String stringValue() const override;
    void textCharacterOffsets(Vector<int>&) const override;
    void wordBoundaries(Vector<PlainTextRange>& words) const override;
    LayoutRect elementRect() const override;
    AXObject* computeParent() const override;
    AccessibilityTextDirection textDirection() const override;
    AXObject* nextOnLine() const override;
    AXObject* previousOnLine() const override;

private:
    RefPtr<AbstractInlineTextBox> m_inlineTextBox;

    bool computeAccessibilityIsIgnored(IgnoredReasons* = nullptr) const override;
};

} // namespace blink

#endif // AXInlineTextBox_h
