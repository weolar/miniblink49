/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2000 Simon Hausmann <hausmann@kde.org>
 * Copyright (C) 2004, 2006, 2008, 2009 Apple Inc. All rights reserved.
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

#ifndef HTMLFrameElementBase_h
#define HTMLFrameElementBase_h

#include "core/html/HTMLFrameOwnerElement.h"
#include "platform/scroll/ScrollTypes.h"

namespace blink {

class HTMLFrameElementBase : public HTMLFrameOwnerElement {
public:
    ScrollbarMode scrollingMode() const final { return m_scrolling; }

    int marginWidth() const { return m_marginWidth; }
    int marginHeight() const { return m_marginHeight; }

    bool canContainRangeEndPoint() const final { return false; }

protected:
    HTMLFrameElementBase(const QualifiedName&, Document&);

    bool isURLAllowed() const;

    void parseAttribute(const QualifiedName&, const AtomicString&) override;
    InsertionNotificationRequest insertedInto(ContainerNode*) override;
    void didNotifySubtreeInsertionsToDocument() final;
    void attach(const AttachContext& = AttachContext()) override;

    // FIXME: Remove this method once we have input routing in the browser
    // process. See http://crbug.com/339659.
    void defaultEventHandler(Event*) override;

private:
    bool supportsFocus() const final;
    void setFocus(bool) final;

    bool isURLAttribute(const Attribute&) const final;
    bool hasLegalLinkAttribute(const QualifiedName&) const final;
    bool isHTMLContentAttribute(const Attribute&) const final;

    bool areAuthorShadowsAllowed() const final { return false; }

    void setLocation(const String&);
    void setNameAndOpenURL();
    void openURL(bool lockBackForwardList = true);

    AtomicString m_URL;
    AtomicString m_frameName;

    ScrollbarMode m_scrolling;

    int m_marginWidth;
    int m_marginHeight;
};

inline bool isHTMLFrameElementBase(const HTMLElement& element)
{
    return isHTMLFrameElement(element) || isHTMLIFrameElement(element);
}

DEFINE_HTMLELEMENT_TYPE_CASTS_WITH_FUNCTION(HTMLFrameElementBase);

} // namespace blink

#endif // HTMLFrameElementBase_h
