/*
 * Copyright (C) 1997 Martin Jones (mjones@kde.org)
 *           (C) 1997 Torben Weis (weis@kde.org)
 *           (C) 1998 Waldo Bastian (bastian@kde.org)
 *           (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 * Copyright (C) 2003, 2004, 2005, 2006, 2007, 2010 Apple Inc. All rights reserved.
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

#ifndef HTMLTableRowElement_h
#define HTMLTableRowElement_h

#include "core/CoreExport.h"
#include "core/html/HTMLTablePartElement.h"

namespace blink {

class ExceptionState;

class CORE_EXPORT HTMLTableRowElement final : public HTMLTablePartElement {
    DEFINE_WRAPPERTYPEINFO();
public:
    DECLARE_NODE_FACTORY(HTMLTableRowElement);

    int rowIndex() const;

    int sectionRowIndex() const;

    PassRefPtrWillBeRawPtr<HTMLElement> insertCell(int index, ExceptionState&);
    void deleteCell(int index, ExceptionState&);

    PassRefPtrWillBeRawPtr<HTMLCollection> cells();

private:
    explicit HTMLTableRowElement(Document&);

    bool hasLegalLinkAttribute(const QualifiedName&) const override;
    const QualifiedName& subResourceAttributeName() const override;
};

} // namespace blink

#endif // HTMLTableRowElement_h
