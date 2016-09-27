/*
 * Copyright (C) 2010, 2011 Nokia Corporation and/or its subsidiary(-ies)
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

#include "config.h"
#include "core/html/HTMLDetailsElement.h"

#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "core/CSSPropertyNames.h"
#include "core/CSSValueKeywords.h"
#include "core/HTMLNames.h"
#include "core/dom/ElementTraversal.h"
#include "core/dom/Text.h"
#include "core/dom/shadow/ShadowRoot.h"
#include "core/events/Event.h"
#include "core/events/EventSender.h"
#include "core/frame/UseCounter.h"
#include "core/html/HTMLContentElement.h"
#include "core/html/HTMLDivElement.h"
#include "core/html/HTMLSummaryElement.h"
#include "core/html/shadow/DetailsMarkerControl.h"
#include "core/html/shadow/ShadowElementNames.h"
#include "core/layout/LayoutBlockFlow.h"
#include "platform/text/PlatformLocale.h"

namespace blink {

using namespace HTMLNames;

class FirstSummarySelectFilter final : public HTMLContentSelectFilter {
public:
    virtual ~FirstSummarySelectFilter() { }

    static PassOwnPtrWillBeRawPtr<FirstSummarySelectFilter> create()
    {
        return adoptPtrWillBeNoop(new FirstSummarySelectFilter());
    }

    bool canSelectNode(const WillBeHeapVector<RawPtrWillBeMember<Node>, 32>& siblings, int nth) const override
    {
        if (!siblings[nth]->hasTagName(HTMLNames::summaryTag))
            return false;
        for (int i = nth - 1; i >= 0; --i) {
            if (siblings[i]->hasTagName(HTMLNames::summaryTag))
                return false;
        }
        return true;
    }

    DEFINE_INLINE_VIRTUAL_TRACE()
    {
        HTMLContentSelectFilter::trace(visitor);
    }

private:
    FirstSummarySelectFilter() { }
};

static DetailsEventSender& detailsToggleEventSender()
{
    DEFINE_STATIC_LOCAL(DetailsEventSender, sharedToggleEventSender, (EventTypeNames::toggle));
    return sharedToggleEventSender;
}

PassRefPtrWillBeRawPtr<HTMLDetailsElement> HTMLDetailsElement::create(Document& document)
{
    RefPtrWillBeRawPtr<HTMLDetailsElement> details = adoptRefWillBeNoop(new HTMLDetailsElement(document));
    details->ensureUserAgentShadowRoot();
    return details.release();
}

HTMLDetailsElement::HTMLDetailsElement(Document& document)
    : HTMLElement(detailsTag, document)
    , m_isOpen(false)
{
    UseCounter::count(document, UseCounter::DetailsElement);
}

HTMLDetailsElement::~HTMLDetailsElement()
{
    detailsToggleEventSender().cancelEvent(this);
}

void HTMLDetailsElement::dispatchPendingEvent(DetailsEventSender* eventSender)
{
    ASSERT_UNUSED(eventSender, eventSender == &detailsToggleEventSender());
    dispatchEvent(Event::create(EventTypeNames::toggle));
}


LayoutObject* HTMLDetailsElement::createLayoutObject(const ComputedStyle&)
{
    return new LayoutBlockFlow(this);
}

void HTMLDetailsElement::didAddUserAgentShadowRoot(ShadowRoot& root)
{
    RefPtrWillBeRawPtr<HTMLSummaryElement> defaultSummary = HTMLSummaryElement::create(document());
    defaultSummary->appendChild(Text::create(document(), locale().queryString(WebLocalizedString::DetailsLabel)));

    RefPtrWillBeRawPtr<HTMLContentElement> summary = HTMLContentElement::create(document(), FirstSummarySelectFilter::create());
    summary->setIdAttribute(ShadowElementNames::detailsSummary());
    summary->appendChild(defaultSummary);
    root.appendChild(summary.release());

    RefPtrWillBeRawPtr<HTMLDivElement> content = HTMLDivElement::create(document());
    content->setIdAttribute(ShadowElementNames::detailsContent());
    content->appendChild(HTMLContentElement::create(document()));
    content->setInlineStyleProperty(CSSPropertyDisplay, CSSValueNone);
    root.appendChild(content.release());
}

Element* HTMLDetailsElement::findMainSummary() const
{
    if (HTMLSummaryElement* summary = Traversal<HTMLSummaryElement>::firstChild(*this))
        return summary;

    HTMLContentElement* content = toHTMLContentElement(userAgentShadowRoot()->firstChild());
    ASSERT(content->firstChild() && isHTMLSummaryElement(*content->firstChild()));
    return toElement(content->firstChild());
}

void HTMLDetailsElement::parseAttribute(const QualifiedName& name, const AtomicString& value)
{
    if (name == openAttr) {
        bool oldValue = m_isOpen;
        m_isOpen = !value.isNull();
        if (m_isOpen == oldValue)
            return;

        // Dispatch toggle event asynchronously.
        detailsToggleEventSender().cancelEvent(this);
        detailsToggleEventSender().dispatchEventSoon(this);

        Element* content = ensureUserAgentShadowRoot().getElementById(ShadowElementNames::detailsContent());
        ASSERT(content);
        if (m_isOpen)
            content->removeInlineStyleProperty(CSSPropertyDisplay);
        else
            content->setInlineStyleProperty(CSSPropertyDisplay, CSSValueNone);

        // Invalidate the LayoutDetailsMarker in order to turn the arrow signifying if the
        // details element is open or closed.
        Element* summary = findMainSummary();
        ASSERT(summary);

        Element* control = toHTMLSummaryElement(summary)->markerControl();
        if (control && control->layoutObject())
            control->layoutObject()->setShouldDoFullPaintInvalidation();

        return;
    }
    HTMLElement::parseAttribute(name, value);
}

void HTMLDetailsElement::toggleOpen()
{
    setAttribute(openAttr, m_isOpen ? nullAtom : emptyAtom);
}

bool HTMLDetailsElement::isInteractiveContent() const
{
    return true;
}

}
