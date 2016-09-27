/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2007 David Smith (catfish.man@gmail.com)
 * Copyright (C) 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010, 2011 Apple Inc. All rights reserved.
 * Copyright (C) Research In Motion Limited 2010. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "config.h"
#include "core/dom/FirstLetterPseudoElement.h"

#include "core/dom/Element.h"
#include "core/layout/GeneratedChildren.h"
#include "core/layout/LayoutObject.h"
#include "core/layout/LayoutObjectInlines.h"
#include "core/layout/LayoutText.h"
#include "core/layout/LayoutTextFragment.h"
#include "wtf/TemporaryChange.h"
#include "wtf/text/WTFString.h"
#ifdef MINIBLINK_NOT_IMPLEMENTED
#include "wtf/text/icu/UnicodeIcu.h"
#endif // MINIBLINK_NOT_IMPLEMENTED
#include "wtf/text/qt4/UnicodeQt4.h"

namespace blink {

using namespace WTF;
using namespace Unicode;

// CSS 2.1 http://www.w3.org/TR/CSS21/selector.html#first-letter
// "Punctuation (i.e, characters defined in Unicode [UNICODE] in the "open" (Ps), "close" (Pe),
// "initial" (Pi). "final" (Pf) and "other" (Po) punctuation classes), that precedes or follows the first letter should be included"
static inline bool isPunctuationForFirstLetter(UChar c)
{
    CharCategory charCategory = category(c);
    return charCategory == Punctuation_Open
        || charCategory == Punctuation_Close
        || charCategory == Punctuation_InitialQuote
        || charCategory == Punctuation_FinalQuote
        || charCategory == Punctuation_Other;
}

static inline bool isSpaceForFirstLetter(UChar c)
{
    return isSpaceOrNewline(c) || c == noBreakSpaceCharacter;
}

unsigned FirstLetterPseudoElement::firstLetterLength(const String& text)
{
    unsigned length = 0;
    unsigned textLength = text.length();

    if (textLength == 0)
        return length;

    // Account for leading spaces first.
    while (length < textLength && isSpaceForFirstLetter(text[length]))
        length++;
    // Now account for leading punctuation.
    while (length < textLength && isPunctuationForFirstLetter(text[length]))
        length++;

    // Bail if we didn't find a letter before the end of the text or before a space.
    if (isSpaceForFirstLetter(text[length]) || length == textLength)
        return 0;

    // Account the next character for first letter.
    length++;

    // Keep looking for allowed punctuation for the :first-letter.
    for (; length < textLength; ++length) {
        UChar c = text[length];
        if (!isPunctuationForFirstLetter(c))
            break;
    }
    return length;
}

// Once we see any of these layoutObjects we can stop looking for first-letter as
// they signal the end of the first line of text.
static bool isInvalidFirstLetterLayoutObject(const LayoutObject* obj)
{
    return (obj->isBR() || (obj->isText() && toLayoutText(obj)->isWordBreak()));
}

LayoutObject* FirstLetterPseudoElement::firstLetterTextLayoutObject(const Element& element)
{
    LayoutObject* parentLayoutObject = 0;

    // If we are looking at a first letter element then we need to find the
    // first letter text layoutObject from the parent node, and not ourselves.
    if (element.isFirstLetterPseudoElement())
        parentLayoutObject = element.parentOrShadowHostElement()->layoutObject();
    else
        parentLayoutObject = element.layoutObject();

    if (!parentLayoutObject
        || !parentLayoutObject->style()->hasPseudoStyle(FIRST_LETTER)
        || !canHaveGeneratedChildren(*parentLayoutObject)
        || !(parentLayoutObject->isLayoutBlockFlow() || parentLayoutObject->isLayoutButton()))
        return nullptr;

    // Drill down into our children and look for our first text child.
    LayoutObject* firstLetterTextLayoutObject = parentLayoutObject->slowFirstChild();
    while (firstLetterTextLayoutObject) {
        // This can be called when the first letter layoutObject is already in the tree. We do not
        // want to consider that layoutObject for our text layoutObject so we go to the sibling (which is
        // the LayoutTextFragment for the remaining text).
        if (firstLetterTextLayoutObject->style() && firstLetterTextLayoutObject->style()->styleType() == FIRST_LETTER) {
            firstLetterTextLayoutObject = firstLetterTextLayoutObject->nextSibling();
        } else if (firstLetterTextLayoutObject->isText()) {
            // FIXME: If there is leading punctuation in a different LayoutText than
            // the first letter, we'll not apply the correct style to it.
            RefPtr<StringImpl> str = toLayoutText(firstLetterTextLayoutObject)->isTextFragment() ?
                toLayoutTextFragment(firstLetterTextLayoutObject)->completeText() :
                toLayoutText(firstLetterTextLayoutObject)->originalText();
            if (firstLetterLength(str.get()) || isInvalidFirstLetterLayoutObject(firstLetterTextLayoutObject))
                break;
            firstLetterTextLayoutObject = firstLetterTextLayoutObject->nextSibling();
        } else if (firstLetterTextLayoutObject->isListMarker()) {
            firstLetterTextLayoutObject = firstLetterTextLayoutObject->nextSibling();
        } else if (firstLetterTextLayoutObject->isFloatingOrOutOfFlowPositioned()) {
            if (firstLetterTextLayoutObject->style()->styleType() == FIRST_LETTER) {
                firstLetterTextLayoutObject = firstLetterTextLayoutObject->slowFirstChild();
                break;
            }
            firstLetterTextLayoutObject = firstLetterTextLayoutObject->nextSibling();
        } else if (firstLetterTextLayoutObject->isReplaced() || firstLetterTextLayoutObject->isLayoutButton()
            || firstLetterTextLayoutObject->isMenuList()) {
            return nullptr;
        } else if (firstLetterTextLayoutObject->isFlexibleBoxIncludingDeprecated() || firstLetterTextLayoutObject->isLayoutGrid()) {
            firstLetterTextLayoutObject = firstLetterTextLayoutObject->nextSibling();
        } else if (!firstLetterTextLayoutObject->isInline()
            && firstLetterTextLayoutObject->style()->hasPseudoStyle(FIRST_LETTER)
            && canHaveGeneratedChildren(*firstLetterTextLayoutObject)) {
            // There is a layoutObject further down the tree which has FIRST_LETTER set. When that node
            // is attached we will handle setting up the first letter then.
            return nullptr;
        } else {
            firstLetterTextLayoutObject = firstLetterTextLayoutObject->slowFirstChild();
        }
    }

    // No first letter text to display, we're done.
    // FIXME: This black-list of disallowed LayoutText subclasses is fragile. crbug.com/422336.
    // Should counter be on this list? What about LayoutTextFragment?
    if (!firstLetterTextLayoutObject || !firstLetterTextLayoutObject->isText() || isInvalidFirstLetterLayoutObject(firstLetterTextLayoutObject))
        return nullptr;

    return firstLetterTextLayoutObject;
}

FirstLetterPseudoElement::FirstLetterPseudoElement(Element* parent)
    : PseudoElement(parent, FIRST_LETTER)
    , m_remainingTextLayoutObject(nullptr)
{
}

FirstLetterPseudoElement::~FirstLetterPseudoElement()
{
}

void FirstLetterPseudoElement::updateTextFragments()
{
    String oldText =  m_remainingTextLayoutObject->completeText();
    ASSERT(oldText.impl());

    unsigned length = FirstLetterPseudoElement::firstLetterLength(oldText);
    m_remainingTextLayoutObject->setTextFragment(oldText.impl()->substring(length, oldText.length()), length, oldText.length() - length);
    m_remainingTextLayoutObject->dirtyLineBoxes();

    for (auto child = layoutObject()->slowFirstChild(); child; child = child->nextSibling()) {
        if (!child->isText() || !toLayoutText(child)->isTextFragment())
            continue;
        LayoutTextFragment* childFragment = toLayoutTextFragment(child);
        if (childFragment->firstLetterPseudoElement() != this)
            continue;

        childFragment->setTextFragment(oldText.impl()->substring(0, length), 0, length);
        childFragment->dirtyLineBoxes();

        // Make sure the first-letter layoutObject is set to require a layout as it
        // needs to re-create the line boxes. The remaining text layoutObject
        // will be marked by the LayoutText::setText.
        childFragment->setNeedsLayoutAndPrefWidthsRecalc(LayoutInvalidationReason::TextChanged);
        break;
    }
}

void FirstLetterPseudoElement::setRemainingTextLayoutObject(LayoutTextFragment* fragment)
{
    // The text fragment we get our content from is being destroyed. We need
    // to tell our parent element to recalcStyle so we can get cleaned up
    // as well.
    if (!fragment)
        setNeedsStyleRecalc(LocalStyleChange, StyleChangeReasonForTracing::create(StyleChangeReason::PseudoClass));

    m_remainingTextLayoutObject = fragment;
}

void FirstLetterPseudoElement::attach(const AttachContext& context)
{
    PseudoElement::attach(context);
    attachFirstLetterTextLayoutObjects();
}

void FirstLetterPseudoElement::detach(const AttachContext& context)
{
    if (m_remainingTextLayoutObject) {
        if (m_remainingTextLayoutObject->node() && document().isActive()) {
            Text* textNode = toText(m_remainingTextLayoutObject->node());
            m_remainingTextLayoutObject->setTextFragment(textNode->dataImpl(), 0, textNode->dataImpl()->length());
        }
        m_remainingTextLayoutObject->setFirstLetterPseudoElement(nullptr);
        m_remainingTextLayoutObject->setIsRemainingTextLayoutObject(false);
    }
    m_remainingTextLayoutObject = nullptr;

    PseudoElement::detach(context);
}

ComputedStyle* FirstLetterPseudoElement::styleForFirstLetter(LayoutObject* layoutObjectContainer)
{
    ASSERT(layoutObjectContainer);

    LayoutObject* styleContainer = parentOrShadowHostElement()->layoutObject();
    ASSERT(styleContainer);

    // We always force the pseudo style to recompute as the first-letter style
    // computed by the style container may not have taken the layoutObjects styles
    // into account.
    styleContainer->mutableStyle()->removeCachedPseudoStyle(FIRST_LETTER);

    ComputedStyle* pseudoStyle = styleContainer->getCachedPseudoStyle(FIRST_LETTER, layoutObjectContainer->firstLineStyle());
    ASSERT(pseudoStyle);

    return pseudoStyle;
}

void FirstLetterPseudoElement::attachFirstLetterTextLayoutObjects()
{
    LayoutObject* nextLayoutObject = FirstLetterPseudoElement::firstLetterTextLayoutObject(*this);
    ASSERT(nextLayoutObject);
    ASSERT(nextLayoutObject->isText());

    // The original string is going to be either a generated content string or a DOM node's
    // string. We want the original string before it got transformed in case first-letter has
    // no text-transform or a different text-transform applied to it.
    String oldText = toLayoutText(nextLayoutObject)->isTextFragment() ? toLayoutTextFragment(nextLayoutObject)->completeText() : toLayoutText(nextLayoutObject)->originalText();
    ASSERT(oldText.impl());

    ComputedStyle* pseudoStyle = styleForFirstLetter(nextLayoutObject->parent());
    layoutObject()->setStyle(pseudoStyle);

    // FIXME: This would already have been calculated in firstLetterLayoutObject. Can we pass the length through?
    unsigned length = FirstLetterPseudoElement::firstLetterLength(oldText);

    // Construct a text fragment for the text after the first letter.
    // This text fragment might be empty.
    LayoutTextFragment* remainingText =
        new LayoutTextFragment(nextLayoutObject->node() ? nextLayoutObject->node() : &nextLayoutObject->document(), oldText.impl(), length, oldText.length() - length);
    remainingText->setFirstLetterPseudoElement(this);
    remainingText->setIsRemainingTextLayoutObject(true);
    remainingText->setStyle(nextLayoutObject->mutableStyle());

    if (remainingText->node())
        remainingText->node()->setLayoutObject(remainingText);

    m_remainingTextLayoutObject = remainingText;

    LayoutObject* nextSibling = layoutObject()->nextSibling();
    layoutObject()->parent()->addChild(remainingText, nextSibling);

    // Construct text fragment for the first letter.
    LayoutTextFragment* letter = new LayoutTextFragment(&nextLayoutObject->document(), oldText.impl(), 0, length);
    letter->setFirstLetterPseudoElement(this);
    letter->setStyle(pseudoStyle);
    layoutObject()->addChild(letter);

    nextLayoutObject->destroy();
}

void FirstLetterPseudoElement::didRecalcStyle(StyleRecalcChange)
{
    if (!layoutObject())
        return;

    // The layoutObjects inside pseudo elements are anonymous so they don't get notified of recalcStyle and must have
    // the style propagated downward manually similar to LayoutObject::propagateStyleToAnonymousChildren.
    LayoutObject* layoutObject = this->layoutObject();
    for (LayoutObject* child = layoutObject->nextInPreOrder(layoutObject); child; child = child->nextInPreOrder(layoutObject)) {
        // We need to re-calculate the correct style for the first letter element
        // and then apply that to the container and the text fragment inside.
        if (child->style()->styleType() == FIRST_LETTER && m_remainingTextLayoutObject) {
            if (ComputedStyle* pseudoStyle = styleForFirstLetter(m_remainingTextLayoutObject->parent()))
                child->setPseudoStyle(pseudoStyle);
            continue;
        }

        // We only manage the style for the generated content items.
        if (!child->isText() && !child->isQuote() && !child->isImage())
            continue;

        child->setPseudoStyle(layoutObject->mutableStyle());
    }
}

} // namespace blink
