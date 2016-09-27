/*
 * Copyright (C) 2004, 2005 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006, 2007 Rob Buis <buis@kde.org>
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

#ifndef SVGStyleElement_h
#define SVGStyleElement_h

#include "core/SVGNames.h"
#include "core/dom/StyleElement.h"
#include "core/events/EventSender.h"
#include "core/svg/SVGElement.h"
#include "platform/heap/Handle.h"

namespace blink {

typedef EventSender<SVGStyleElement> SVGStyleEventSender;

class SVGStyleElement final : public SVGElement
                            , public StyleElement {
    DEFINE_WRAPPERTYPEINFO();
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(SVGStyleElement);
public:
    static PassRefPtrWillBeRawPtr<SVGStyleElement> create(Document&, bool createdByParser);
    ~SVGStyleElement() override;

    using StyleElement::sheet;

    bool disabled() const;
    void setDisabled(bool);

    const AtomicString& type() const override;
    void setType(const AtomicString&);

    const AtomicString& media() const override;
    void setMedia(const AtomicString&);

    String title() const override;
    void setTitle(const AtomicString&);

    void dispatchPendingEvent(SVGStyleEventSender*);

    DECLARE_VIRTUAL_TRACE();

private:
    SVGStyleElement(Document&, bool createdByParser);

    void parseAttribute(const QualifiedName&, const AtomicString&) override;
    InsertionNotificationRequest insertedInto(ContainerNode*) override;
    void didNotifySubtreeInsertionsToDocument() override;
    void removedFrom(ContainerNode*) override;
    void childrenChanged(const ChildrenChange&) override;

    void finishParsingChildren() override;
    bool layoutObjectIsNeeded(const ComputedStyle&) override { return false; }

    bool sheetLoaded() override { return StyleElement::sheetLoaded(document()); }
    void notifyLoadedSheetAndAllCriticalSubresources(LoadedSheetErrorStatus) override;
    void startLoadingDynamicSheet() override { StyleElement::startLoadingDynamicSheet(document()); }
};

} // namespace blink

#endif // SVGStyleElement_h
