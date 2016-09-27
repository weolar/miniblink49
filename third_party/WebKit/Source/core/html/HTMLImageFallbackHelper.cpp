// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/html/HTMLImageFallbackHelper.h"

#include "core/HTMLNames.h"
#include "core/InputTypeNames.h"
#include "core/dom/ElementRareData.h"
#include "core/dom/Text.h"
#include "core/dom/shadow/ShadowRoot.h"
#include "core/fetch/ImageResource.h"
#include "core/html/FormDataList.h"
#include "core/html/HTMLDivElement.h"
#include "core/html/HTMLElement.h"
#include "core/html/HTMLImageElement.h"
#include "core/html/HTMLImageLoader.h"
#include "core/html/HTMLInputElement.h"
#include "core/html/HTMLStyleElement.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/text/StringBuilder.h"

namespace blink {

using namespace HTMLNames;

static bool noImageSourceSpecified(const Element& element)
{
    bool noSrcSpecified = !element.hasAttribute(srcAttr) || element.getAttribute(srcAttr).isNull() || element.getAttribute(srcAttr).isEmpty();
    bool noSrcsetSpecified = !element.hasAttribute(srcsetAttr) || element.getAttribute(srcsetAttr).isNull() || element.getAttribute(srcsetAttr).isEmpty();
    return noSrcSpecified && noSrcsetSpecified;
}

void HTMLImageFallbackHelper::createAltTextShadowTree(Element& element)
{
    ShadowRoot& root = element.ensureUserAgentShadowRoot();

    RefPtrWillBeRawPtr<HTMLDivElement> container = HTMLDivElement::create(element.document());
    root.appendChild(container);
    container->setAttribute(idAttr, AtomicString("alttext-container", AtomicString::ConstructFromLiteral));
    container->setInlineStyleProperty(CSSPropertyOverflow, CSSValueHidden);
    container->setInlineStyleProperty(CSSPropertyBorderWidth, 1, CSSPrimitiveValue::CSS_PX);
    container->setInlineStyleProperty(CSSPropertyBorderStyle, CSSValueSolid);
    container->setInlineStyleProperty(CSSPropertyBorderColor, CSSValueSilver);
    container->setInlineStyleProperty(CSSPropertyDisplay, CSSValueInlineBlock);
    container->setInlineStyleProperty(CSSPropertyBoxSizing, CSSValueBorderBox);
    container->setInlineStyleProperty(CSSPropertyPadding, 1, CSSPrimitiveValue::CSS_PX);

    RefPtrWillBeRawPtr<HTMLImageElement> brokenImage = HTMLImageElement::create(element.document());
    container->appendChild(brokenImage);
    brokenImage->setIsFallbackImage();
    brokenImage->setAttribute(idAttr, AtomicString("alttext-image", AtomicString::ConstructFromLiteral));
    brokenImage->setAttribute(widthAttr, AtomicString("16", AtomicString::ConstructFromLiteral));
    brokenImage->setAttribute(heightAttr, AtomicString("16", AtomicString::ConstructFromLiteral));
    brokenImage->setAttribute(alignAttr, AtomicString("left", AtomicString::ConstructFromLiteral));
    brokenImage->setInlineStyleProperty(CSSPropertyMargin, 0, CSSPrimitiveValue::CSS_PX);

    RefPtrWillBeRawPtr<HTMLDivElement> altText = HTMLDivElement::create(element.document());
    container->appendChild(altText);
    altText->setAttribute(idAttr, AtomicString("alttext", AtomicString::ConstructFromLiteral));
    altText->setInlineStyleProperty(CSSPropertyOverflow, CSSValueHidden);
    altText->setInlineStyleProperty(CSSPropertyDisplay, CSSValueBlock);

    RefPtrWillBeRawPtr<Text> text = Text::create(element.document(), toHTMLElement(element).altText());
    altText->appendChild(text);
}

PassRefPtr<ComputedStyle> HTMLImageFallbackHelper::customStyleForAltText(Element& element, PassRefPtr<ComputedStyle> newStyle)
{
    // If we have an author shadow root or have not created the UA shadow root yet, bail early. We can't
    // use ensureUserAgentShadowRoot() here because that would alter the DOM tree during style recalc.
    if (element.shadowRoot() || !element.userAgentShadowRoot())
        return newStyle;

    Element* placeHolder = element.userAgentShadowRoot()->getElementById("alttext-container");
    Element* brokenImage = element.userAgentShadowRoot()->getElementById("alttext-image");
    // Input elements have a UA shadow root of their own. We may not have replaced it with fallback content yet.
    if (!placeHolder || !brokenImage)
        return newStyle;


    if (element.document().inQuirksMode()) {
        // Mimic the behaviour of the image host by setting symmetric dimensions if only one dimension is specified.
        if (newStyle->width().isSpecifiedOrIntrinsic() && newStyle->height().isAuto())
            newStyle->setHeight(newStyle->width());
        else if (newStyle->height().isSpecifiedOrIntrinsic() && newStyle->width().isAuto())
            newStyle->setWidth(newStyle->height());
        if (newStyle->width().isSpecifiedOrIntrinsic() && newStyle->height().isSpecifiedOrIntrinsic()) {
            placeHolder->setInlineStyleProperty(CSSPropertyVerticalAlign, CSSValueBaseline);
        }
    }

    // If the image has specified dimensions allow the alt-text container expand to fill them.
    if (newStyle->width().isSpecifiedOrIntrinsic() && newStyle->height().isSpecifiedOrIntrinsic()) {
        placeHolder->setInlineStyleProperty(CSSPropertyWidth, 100, CSSPrimitiveValue::CSS_PERCENTAGE);
        placeHolder->setInlineStyleProperty(CSSPropertyHeight, 100, CSSPrimitiveValue::CSS_PERCENTAGE);
    }

    // Make sure the broken image icon appears on the appropriate side of the image for the element's writing direction.
    brokenImage->setInlineStyleProperty(CSSPropertyFloat, AtomicString(newStyle->direction() == LTR ? "left" : "right"));

    // This is an <img> with no attributes, so don't display anything.
    if (noImageSourceSpecified(element) && !newStyle->width().isSpecifiedOrIntrinsic() && !newStyle->height().isSpecifiedOrIntrinsic() && toHTMLElement(element).altText().isEmpty())
        newStyle->setDisplay(NONE);

    // This preserves legacy behaviour originally defined when alt-text was managed by LayoutImage.
    if (noImageSourceSpecified(element))
        brokenImage->setInlineStyleProperty(CSSPropertyDisplay, CSSValueNone);
    else
        brokenImage->setInlineStyleProperty(CSSPropertyDisplay, CSSValueInline);

    return newStyle;
}

} // namespace blink
