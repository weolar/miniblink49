/*
 * (C) 1999 Lars Knoll (knoll@kde.org)
 * (C) 2000 Dirk Mueller (mueller@kde.org)
 * Copyright (C) 2004, 2005, 2006, 2007 Apple Inc. All rights reserved.
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
 *
 */

#ifndef LayoutTextFragment_h
#define LayoutTextFragment_h

#include "core/layout/LayoutText.h"

namespace blink {

class FirstLetterPseudoElement;

// Used to represent a text substring of an element, e.g., for text runs that are split because of
// first letter and that must therefore have different styles (and positions in the layout tree).
// We cache offsets so that text transformations can be applied in such a way that we can recover
// the original unaltered string from our corresponding DOM node.
class LayoutTextFragment final : public LayoutText {
public:
    LayoutTextFragment(Node*, StringImpl*, int startOffset, int length);
    LayoutTextFragment(Node*, StringImpl*);
    virtual ~LayoutTextFragment();

    virtual bool isTextFragment() const override { return true; }

    virtual bool canBeSelectionLeaf() const override { return node() && node()->hasEditableStyle(); }

    unsigned start() const { return m_start; }
    unsigned end() const { return m_end; }

    virtual unsigned textStartOffset() const override { return start(); }

    void setContentString(StringImpl*);
    StringImpl* contentString() const { return m_contentString.get(); }
    // The complete text is all of the text in the associated DOM text node.
    PassRefPtr<StringImpl> completeText() const;
    // The fragment text is the text which will be used by this LayoutTextFragment. For
    // things like first-letter this may differ from the completeText as we maybe using
    // only a portion of the text nodes content.

    virtual PassRefPtr<StringImpl> originalText() const override;

    virtual void setText(PassRefPtr<StringImpl>, bool force = false) override;
    void setTextFragment(PassRefPtr<StringImpl>, unsigned start, unsigned length);

    virtual void transformText() override;

    // FIXME: Rename to LayoutTextFragment
    virtual const char* name() const override { return "LayoutTextFragment"; }

    void setFirstLetterPseudoElement(FirstLetterPseudoElement* element) { m_firstLetterPseudoElement = element; }
    FirstLetterPseudoElement* firstLetterPseudoElement() const { return m_firstLetterPseudoElement; }

    void setIsRemainingTextLayoutObject(bool isRemainingText) { m_isRemainingTextLayoutObject = isRemainingText; }
    bool isRemainingTextLayoutObject() const { return m_isRemainingTextLayoutObject; }

protected:
    virtual void willBeDestroyed() override;

private:
    LayoutBlock* blockForAccompanyingFirstLetter() const;
    virtual UChar previousCharacter() const override;

    Text* associatedTextNode() const;
    void updateHitTestResult(HitTestResult&, const LayoutPoint&) override;

    unsigned m_start;
    unsigned m_end;
    bool m_isRemainingTextLayoutObject;
    RefPtr<StringImpl> m_contentString;
    // Reference back to FirstLetterPseudoElement; cleared by FirstLetterPseudoElement::detach() if
    // it goes away first.
    FirstLetterPseudoElement* m_firstLetterPseudoElement;
};

DEFINE_TYPE_CASTS(LayoutTextFragment, LayoutObject, object, toLayoutText(object)->isTextFragment(), toLayoutText(object).isTextFragment());

} // namespace blink

#endif // LayoutTextFragment_h
