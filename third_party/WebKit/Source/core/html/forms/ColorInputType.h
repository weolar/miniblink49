/*
 * Copyright (C) 2010 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef ColorInputType_h
#define ColorInputType_h

#include "core/html/forms/BaseClickableWithKeyInputType.h"
#include "core/html/forms/ColorChooserClient.h"

namespace blink {

class ColorChooser;

class ColorInputType final : public BaseClickableWithKeyInputType, public ColorChooserClient {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(ColorInputType);
public:
    static PassRefPtrWillBeRawPtr<InputType> create(HTMLInputElement&);
    ~ColorInputType() override;
    DECLARE_VIRTUAL_TRACE();

    // ColorChooserClient implementation.
    void didChooseColor(const Color&) override;
    void didEndChooser() override;
    Element& ownerElement() const override;
    IntRect elementRectRelativeToViewport() const override;
    Color currentColor() override;
    bool shouldShowSuggestions() const override;
    Vector<ColorSuggestion> suggestions() const override;
    ColorChooserClient* colorChooserClient() override;

private:
    ColorInputType(HTMLInputElement& element) : BaseClickableWithKeyInputType(element) { }
    void countUsage() override;
    const AtomicString& formControlType() const override;
    bool supportsRequired() const override;
    String fallbackValue() const override;
    String sanitizeValue(const String&) const override;
    void createShadowSubtree() override;
    void setValue(const String&, bool valueChanged, TextFieldEventBehavior) override;
    void handleDOMActivateEvent(Event*) override;
    void closePopupView() override;
    bool shouldRespectListAttribute() override;
    bool typeMismatchFor(const String&) const override;
    void warnIfValueIsInvalid(const String&) const override;
    void updateView() override;
    AXObject* popupRootAXObject() override;

    Color valueAsColor() const;
    void endColorChooser();
    HTMLElement* shadowColorSwatch() const;

    OwnPtrWillBeMember<ColorChooser> m_chooser;
};

} // namespace blink

#endif // ColorInputType_h
