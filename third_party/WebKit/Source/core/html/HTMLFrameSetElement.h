/*
 * Copyright (C) 1999 Lars Knoll (knoll@kde.org)
 *           (C) 1999 Antti Koivisto (koivisto@kde.org)
 *           (C) 2000 Simon Hausmann <hausmann@kde.org>
 * Copyright (C) 2004, 2006, 2009, 2010 Apple Inc. All rights reserved.
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

#ifndef HTMLFrameSetElement_h
#define HTMLFrameSetElement_h

#include "core/dom/Document.h"
#include "core/html/HTMLDimension.h"
#include "core/html/HTMLElement.h"

namespace blink {

class HTMLFrameSetElement final : public HTMLElement {
    DEFINE_WRAPPERTYPEINFO();
public:
    DECLARE_NODE_FACTORY(HTMLFrameSetElement);

    bool hasFrameBorder() const { return m_frameborder; }
    bool noResize() const { return m_noresize; }

    size_t totalRows() const { return std::max<size_t>(1, m_rowLengths.size()); }
    size_t totalCols() const { return std::max<size_t>(1, m_colLengths.size()); }
    int border() const { return hasFrameBorder() ? m_border : 0; }

    bool hasBorderColor() const { return m_borderColorSet; }

    const Vector<HTMLDimension>& rowLengths() const { return m_rowLengths; }
    const Vector<HTMLDimension>& colLengths() const { return m_colLengths; }

    LocalDOMWindow* anonymousNamedGetter(const AtomicString&);

    DEFINE_WINDOW_ATTRIBUTE_EVENT_LISTENER(blur);
    DEFINE_WINDOW_ATTRIBUTE_EVENT_LISTENER(error);
    DEFINE_WINDOW_ATTRIBUTE_EVENT_LISTENER(focus);
    DEFINE_WINDOW_ATTRIBUTE_EVENT_LISTENER(load);
    DEFINE_WINDOW_ATTRIBUTE_EVENT_LISTENER(resize);
    DEFINE_WINDOW_ATTRIBUTE_EVENT_LISTENER(scroll);
    DEFINE_WINDOW_ATTRIBUTE_EVENT_LISTENER(orientationchange);

private:
    explicit HTMLFrameSetElement(Document&);

    void parseAttribute(const QualifiedName&, const AtomicString&) override;
    bool isPresentationAttribute(const QualifiedName&) const override;
    void collectStyleForPresentationAttribute(const QualifiedName&, const AtomicString&, MutableStylePropertySet*) override;

    void attach(const AttachContext& = AttachContext()) override;
    bool layoutObjectIsNeeded(const ComputedStyle&) override;
    LayoutObject* createLayoutObject(const ComputedStyle&) override;

    void defaultEventHandler(Event*) override;

    InsertionNotificationRequest insertedInto(ContainerNode*) override;
    void willRecalcStyle(StyleRecalcChange) override;

    Vector<HTMLDimension> m_rowLengths;
    Vector<HTMLDimension> m_colLengths;

    int m_border;
    bool m_borderSet;

    bool m_borderColorSet;

    bool m_frameborder;
    bool m_frameborderSet;
    bool m_noresize;
};

} // namespace blink

#endif // HTMLFrameSetElement_h
