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

#ifndef FirstLetterPseudoElement_h
#define FirstLetterPseudoElement_h

#include "core/dom/Document.h"
#include "core/dom/PseudoElement.h"
#include "wtf/text/WTFString.h"

namespace blink {

class Element;
class LayoutObject;
class LayoutTextFragment;

class FirstLetterPseudoElement final : public PseudoElement {
    WTF_MAKE_NONCOPYABLE(FirstLetterPseudoElement);
public:
    static PassRefPtrWillBeRawPtr<FirstLetterPseudoElement> create(Element* parent)
    {
        return adoptRefWillBeNoop(new FirstLetterPseudoElement(parent));
    }

    ~FirstLetterPseudoElement() override;

    static LayoutObject* firstLetterTextLayoutObject(const Element&);
    static unsigned firstLetterLength(const String&);

    void setRemainingTextLayoutObject(LayoutTextFragment*);
    LayoutTextFragment* remainingTextLayoutObject() const { return m_remainingTextLayoutObject; }

    void updateTextFragments();

    void attach(const AttachContext& = AttachContext()) override;
    void detach(const AttachContext& = AttachContext()) override;

private:
    explicit FirstLetterPseudoElement(Element*);

    void didRecalcStyle(StyleRecalcChange) override;

    void attachFirstLetterTextLayoutObjects();
    ComputedStyle* styleForFirstLetter(LayoutObject*);

    LayoutTextFragment* m_remainingTextLayoutObject;
};

DEFINE_ELEMENT_TYPE_CASTS(FirstLetterPseudoElement, isFirstLetterPseudoElement());

} // namespace blink

#endif // FirstLetterPseudoElement_h
