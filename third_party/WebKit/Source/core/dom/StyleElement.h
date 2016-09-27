/*
 * Copyright (C) 2006, 2007 Rob Buis
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

#ifndef StyleElement_h
#define StyleElement_h

#include "core/css/CSSStyleSheet.h"
#include "wtf/text/TextPosition.h"

namespace blink {

class ContainerNode;
class Document;
class Element;

class StyleElement : public WillBeGarbageCollectedMixin {
public:
    StyleElement(Document*, bool createdByParser);
    virtual ~StyleElement();
    DECLARE_VIRTUAL_TRACE();

protected:
    enum ProcessingResult {
        ProcessingSuccessful,
        ProcessingFatalError
    };

    virtual const AtomicString& type() const = 0;
    virtual const AtomicString& media() const = 0;

    CSSStyleSheet* sheet() const { return m_sheet.get(); }

    bool isLoading() const;
    bool sheetLoaded(Document&);
    void startLoadingDynamicSheet(Document&);

    void insertedInto(Element*, ContainerNode* insertionPoint);
    void removedFrom(Element*, ContainerNode* insertionPoint);
    void clearDocumentData(Document&, Element*);
    ProcessingResult processStyleSheet(Document&, Element*);
    ProcessingResult childrenChanged(Element*);
    ProcessingResult finishParsingChildren(Element*);

    RefPtrWillBeMember<CSSStyleSheet> m_sheet;

private:
    ProcessingResult createSheet(Element*, const String& text = String());
    ProcessingResult process(Element*);
    void clearSheet(Element* ownerElement = 0);

    bool m_createdByParser : 1;
    bool m_loading : 1;
    bool m_registeredAsCandidate : 1;
    TextPosition m_startPosition;
};

}

#endif
