/*
 * Copyright (C) 2004, 2005, 2008 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2007 Rob Buis <buis@kde.org>
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

#ifndef SVGScriptElement_h
#define SVGScriptElement_h

#include "core/SVGNames.h"
#include "core/dom/ScriptLoaderClient.h"
#include "core/svg/SVGAnimatedBoolean.h"
#include "core/svg/SVGAnimatedString.h"
#include "core/svg/SVGElement.h"
#include "core/svg/SVGURIReference.h"
#include "platform/heap/Handle.h"

namespace blink {

class ScriptLoader;

class SVGScriptElement final
    : public SVGElement
    , public SVGURIReference
    , public ScriptLoaderClient {
    DEFINE_WRAPPERTYPEINFO();
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(SVGScriptElement);
public:
    static PassRefPtrWillBeRawPtr<SVGScriptElement> create(Document&, bool wasInsertedByParser);

    ScriptLoader* loader() const { return m_loader.get(); }

#if ENABLE(ASSERT)
    bool isAnimatableAttribute(const QualifiedName&) const override;
#endif

    DECLARE_VIRTUAL_TRACE();

private:
    SVGScriptElement(Document&, bool wasInsertedByParser, bool alreadyStarted);

    void parseAttribute(const QualifiedName&, const AtomicString&) override;
    InsertionNotificationRequest insertedInto(ContainerNode*) override;
    void didNotifySubtreeInsertionsToDocument() override;
    void childrenChanged(const ChildrenChange&) override;
    void didMoveToNewDocument(Document& oldDocument) override;

    void svgAttributeChanged(const QualifiedName&) override;
    bool isURLAttribute(const Attribute&) const override;
    bool isStructurallyExternal() const override { return hasSourceAttribute(); }
    void finishParsingChildren() override;

    bool haveLoadedRequiredResources() override;

    String sourceAttributeValue() const override;
    String charsetAttributeValue() const override;
    String typeAttributeValue() const override;
    String languageAttributeValue() const override;
    String forAttributeValue() const override;
    String eventAttributeValue() const override;
    bool asyncAttributeValue() const override;
    bool deferAttributeValue() const override;
    bool hasSourceAttribute() const override;

    void dispatchLoadEvent() override;

    PassRefPtrWillBeRawPtr<Element> cloneElementWithoutAttributesAndChildren() override;
    bool layoutObjectIsNeeded(const ComputedStyle&) override { return false; }

    OwnPtrWillBeMember<ScriptLoader> m_loader;
};

} // namespace blink

#endif // SVGScriptElement_h
