/*
 * Copyright (C) 1997 Martin Jones (mjones@kde.org)
 *           (C) 1997 Torben Weis (weis@kde.org)
 *           (C) 1998 Waldo Bastian (bastian@kde.org)
 *           (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 * Copyright (C) 2003, 2004, 2005, 2006, 2008 Apple Inc. All rights reserved.
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

#ifndef HTMLTableElement_h
#define HTMLTableElement_h

#include "core/CoreExport.h"
#include "core/html/HTMLElement.h"

namespace blink {

class ExceptionState;
class HTMLCollection;
class HTMLTableCaptionElement;
class HTMLTableRowsCollection;
class HTMLTableSectionElement;

class CORE_EXPORT HTMLTableElement final : public HTMLElement {
    DEFINE_WRAPPERTYPEINFO();
public:
    DECLARE_NODE_FACTORY(HTMLTableElement);

    HTMLTableCaptionElement* caption() const;
    void setCaption(PassRefPtrWillBeRawPtr<HTMLTableCaptionElement>, ExceptionState&);

    HTMLTableSectionElement* tHead() const;
    void setTHead(PassRefPtrWillBeRawPtr<HTMLTableSectionElement>, ExceptionState&);

    HTMLTableSectionElement* tFoot() const;
    void setTFoot(PassRefPtrWillBeRawPtr<HTMLTableSectionElement>, ExceptionState&);

    PassRefPtrWillBeRawPtr<HTMLElement> createTHead();
    void deleteTHead();
    PassRefPtrWillBeRawPtr<HTMLElement> createTFoot();
    void deleteTFoot();
    PassRefPtrWillBeRawPtr<HTMLElement> createTBody();
    PassRefPtrWillBeRawPtr<HTMLElement> createCaption();
    void deleteCaption();
    PassRefPtrWillBeRawPtr<HTMLElement> insertRow(int index, ExceptionState&);
    void deleteRow(int index, ExceptionState&);

    PassRefPtrWillBeRawPtr<HTMLTableRowsCollection> rows();
    PassRefPtrWillBeRawPtr<HTMLCollection> tBodies();

    const AtomicString& rules() const;
    const AtomicString& summary() const;

    const StylePropertySet* additionalCellStyle();
    const StylePropertySet* additionalGroupStyle(bool rows);

    DECLARE_VIRTUAL_TRACE();

private:
    explicit HTMLTableElement(Document&);
    ~HTMLTableElement();

    void parseAttribute(const QualifiedName&, const AtomicString&) override;
    bool isPresentationAttribute(const QualifiedName&) const override;
    void collectStyleForPresentationAttribute(const QualifiedName&, const AtomicString&, MutableStylePropertySet*) override;
    bool isURLAttribute(const Attribute&) const override;
    bool hasLegalLinkAttribute(const QualifiedName&) const override;
    const QualifiedName& subResourceAttributeName() const override;

    // Used to obtain either a solid or outset border decl and to deal with the frame and rules attributes.
    const StylePropertySet* additionalPresentationAttributeStyle() override;

    enum TableRules { UnsetRules, NoneRules, GroupsRules, RowsRules, ColsRules, AllRules };
    enum CellBorders { NoBorders, SolidBorders, InsetBorders, SolidBordersColsOnly, SolidBordersRowsOnly };

    CellBorders cellBorders() const;

    PassRefPtrWillBeRawPtr<StylePropertySet> createSharedCellStyle();

    HTMLTableSectionElement* lastBody() const;

    void setNeedsTableStyleRecalc() const;

    bool m_borderAttr;          // Sets a precise border width and creates an outset border for the table and for its cells.
    bool m_borderColorAttr;     // Overrides the outset border and makes it solid for the table and cells instead.
    bool m_frameAttr;           // Implies a thin border width if no border is set and then a certain set of solid/hidden borders based off the value.
    TableRules m_rulesAttr;     // Implies a thin border width, a collapsing border model, and all borders on the table becoming set to hidden (if frame/border
                                // are present, to none otherwise).

    unsigned short m_padding;
    RefPtrWillBeMember<StylePropertySet> m_sharedCellStyle;
};

} // namespace blink

#endif // HTMLTableElement_h
