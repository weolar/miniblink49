/*
 * Copyright (C) 2004, 2005, 2006, 2008 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006 Rob Buis <buis@kde.org>
 * Copyright (C) 2009, 2014 Apple Inc. All rights reserved.
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

#ifndef SVGElement_h
#define SVGElement_h

#include "core/CoreExport.h"
#include "core/SVGNames.h"
#include "core/dom/Element.h"
#include "core/svg/SVGParsingError.h"
#include "core/svg/properties/SVGPropertyInfo.h"
#include "platform/heap/Handle.h"
#include "wtf/HashMap.h"
#include "wtf/OwnPtr.h"

namespace blink {

class AffineTransform;
class CSSCursorImageValue;
class Document;
class SVGAnimatedPropertyBase;
class SubtreeLayoutScope;
class SVGAnimatedString;
class SVGCursorElement;
class SVGDocumentExtensions;
class SVGElement;
class SVGElementRareData;
class SVGFitToViewBox;
class SVGLength;
class SVGSVGElement;
class SVGUseElement;

typedef WillBeHeapHashSet<RawPtrWillBeMember<SVGElement>> SVGElementSet;

class CORE_EXPORT SVGElement : public Element {
    DEFINE_WRAPPERTYPEINFO();
public:
    ~SVGElement() override;
    void attach(const AttachContext&) override;
    void detach(const AttachContext&) override;

    short tabIndex() const override;
    bool supportsFocus() const override { return false; }

    bool isOutermostSVGSVGElement() const;

    bool hasTagName(const SVGQualifiedName& name) const { return hasLocalName(name.localName()); }

    String title() const override;
    bool hasRelativeLengths() const { return !m_elementsWithRelativeLengths.isEmpty(); }
    static bool isAnimatableCSSProperty(const QualifiedName&);

    enum CTMScope {
        NearestViewportScope, // Used by SVGGraphicsElement::getCTM()
        ScreenScope, // Used by SVGGraphicsElement::getScreenCTM()
        AncestorScope // Used by SVGSVGElement::get{Enclosure|Intersection}List()
    };
    virtual AffineTransform localCoordinateSpaceTransform(CTMScope) const;
    virtual bool needsPendingResourceHandling() const { return true; }

    bool instanceUpdatesBlocked() const;
    void setInstanceUpdatesBlocked(bool);

    SVGSVGElement* ownerSVGElement() const;
    SVGElement* viewportElement() const;

    SVGDocumentExtensions& accessDocumentSVGExtensions();

    virtual bool isSVGGeometryElement() const { return false; }
    virtual bool isSVGGraphicsElement() const { return false; }
    virtual bool isFilterEffect() const { return false; }
    virtual bool isTextContent() const { return false; }
    virtual bool isTextPositioning() const { return false; }
    virtual bool isStructurallyExternal() const { return false; }

    // For SVGTests
    virtual bool isValid() const { return true; }

    virtual void svgAttributeChanged(const QualifiedName&);

    PassRefPtrWillBeRawPtr<SVGAnimatedPropertyBase> propertyFromAttribute(const QualifiedName& attributeName);
    static AnimatedPropertyType animatedPropertyTypeForCSSAttribute(const QualifiedName& attributeName);

    void sendSVGLoadEventToSelfAndAncestorChainIfPossible();
    bool sendSVGLoadEventIfPossible();

    virtual AffineTransform* animateMotionTransform() { return nullptr; }

    void invalidateSVGAttributes() { ensureUniqueElementData().m_animatedSVGAttributesAreDirty = true; }
    void invalidateSVGPresentationAttributeStyle() { ensureUniqueElementData().m_presentationAttributeStyleIsDirty = true; }
    void addSVGLengthPropertyToPresentationAttributeStyle(MutableStylePropertySet*, CSSPropertyID, SVGLength&);

    const WillBeHeapHashSet<RawPtrWillBeWeakMember<SVGElement>>& instancesForElement() const;
    void mapInstanceToElement(SVGElement*);
    void removeInstanceMapping(SVGElement*);

    bool getBoundingBox(FloatRect&);

    void setCursorElement(SVGCursorElement*);
    void setCursorImageValue(CSSCursorImageValue*);

#if !ENABLE(OILPAN)
    void cursorElementRemoved();
    void cursorImageValueRemoved();
#endif

    SVGElement* correspondingElement();
    void setCorrespondingElement(SVGElement*);
    SVGUseElement* correspondingUseElement() const;

    void synchronizeAnimatedSVGAttribute(const QualifiedName&) const;

    PassRefPtr<ComputedStyle> customStyleForLayoutObject() final;

#if ENABLE(ASSERT)
    virtual bool isAnimatableAttribute(const QualifiedName&) const;
#endif

    MutableStylePropertySet* animatedSMILStyleProperties() const;
    MutableStylePropertySet* ensureAnimatedSMILStyleProperties();
    void setUseOverrideComputedStyle(bool);

    virtual bool haveLoadedRequiredResources();

    bool addEventListener(const AtomicString& eventType, PassRefPtr<EventListener>, bool useCapture = false) final;
    bool removeEventListener(const AtomicString& eventType, PassRefPtr<EventListener>, bool useCapture = false) final;

    void invalidateRelativeLengthClients(SubtreeLayoutScope* = 0);

    void addToPropertyMap(PassRefPtrWillBeRawPtr<SVGAnimatedPropertyBase>);

    SVGAnimatedString* className() { return m_className.get(); }

    bool inUseShadowTree() const;

    SVGElementSet* setOfIncomingReferences() const;
    void addReferenceTo(SVGElement*);
    void rebuildAllIncomingReferences();
    void removeAllIncomingReferences();
    void removeAllOutgoingReferences();

    class InvalidationGuard {
        STACK_ALLOCATED();
        WTF_MAKE_NONCOPYABLE(InvalidationGuard);
    public:
        InvalidationGuard(SVGElement* element) : m_element(element) { }
        ~InvalidationGuard() { m_element->invalidateInstances(); }

    private:
        RawPtrWillBeMember<SVGElement> m_element;
    };

    class InstanceUpdateBlocker {
        STACK_ALLOCATED();
        WTF_MAKE_NONCOPYABLE(InstanceUpdateBlocker);
    public:
        InstanceUpdateBlocker(SVGElement* targetElement);
        ~InstanceUpdateBlocker();

    private:
        RawPtrWillBeMember<SVGElement> m_targetElement;
    };

    void invalidateInstances();

    DECLARE_VIRTUAL_TRACE();

    static const AtomicString& eventParameterName();

    bool isPresentationAttribute(const QualifiedName&) const override;
    virtual bool isPresentationAttributeWithSVGDOM(const QualifiedName&) const { return false; }

protected:
    SVGElement(const QualifiedName&, Document&, ConstructionType = CreateSVGElement);

    void parseAttribute(const QualifiedName&, const AtomicString&) override;

    void attributeChanged(const QualifiedName&, const AtomicString&, AttributeModificationReason = ModifiedDirectly) override;

    void collectStyleForPresentationAttribute(const QualifiedName&, const AtomicString&, MutableStylePropertySet*) override;

    InsertionNotificationRequest insertedInto(ContainerNode*) override;
    void removedFrom(ContainerNode*) override;
    void childrenChanged(const ChildrenChange&) override;

    static CSSPropertyID cssPropertyIdForSVGAttributeName(const QualifiedName&);
    void updateRelativeLengthsInformation() { updateRelativeLengthsInformation(selfHasRelativeLengths(), this); }
    void updateRelativeLengthsInformation(bool hasRelativeLengths, SVGElement*);
    static void markForLayoutAndParentResourceInvalidation(LayoutObject*);

    virtual bool selfHasRelativeLengths() const { return false; }

    SVGElementRareData* ensureSVGRareData();
    inline bool hasSVGRareData() const { return m_SVGRareData; }
    inline SVGElementRareData* svgRareData() const
    {
        ASSERT(m_SVGRareData);
        return m_SVGRareData.get();
    }

    // SVGFitToViewBox::parseAttribute uses reportAttributeParsingError.
    friend class SVGFitToViewBox;
    void reportAttributeParsingError(SVGParsingError, const QualifiedName&, const AtomicString&);
    bool hasFocusEventListeners() const;

private:
    bool isSVGElement() const = delete; // This will catch anyone doing an unnecessary check.
    bool isStyledElement() const = delete; // This will catch anyone doing an unnecessary check.

    const ComputedStyle* ensureComputedStyle(PseudoId = NOPSEUDO);
    const ComputedStyle* virtualEnsureComputedStyle(PseudoId pseudoElementSpecifier = NOPSEUDO) final { return ensureComputedStyle(pseudoElementSpecifier); }
    void willRecalcStyle(StyleRecalcChange) override;

    void buildPendingResourcesIfNeeded();

    WillBeHeapHashSet<RawPtrWillBeWeakMember<SVGElement>> m_elementsWithRelativeLengths;

    typedef WillBeHeapHashMap<QualifiedName, RefPtrWillBeMember<SVGAnimatedPropertyBase>> AttributeToPropertyMap;
    AttributeToPropertyMap m_attributeToPropertyMap;

#if ENABLE(ASSERT)
    bool m_inRelativeLengthClientsInvalidation;
#endif

    OwnPtrWillBeMember<SVGElementRareData> m_SVGRareData;
    RefPtrWillBeMember<SVGAnimatedString> m_className;
};

struct SVGAttributeHashTranslator {
    static unsigned hash(const QualifiedName& key)
    {
        if (key.hasPrefix()) {
            QualifiedNameComponents components = { nullAtom.impl(), key.localName().impl(), key.namespaceURI().impl() };
            return hashComponents(components);
        }
        return DefaultHash<QualifiedName>::Hash::hash(key);
    }
    static bool equal(const QualifiedName& a, const QualifiedName& b) { return a.matches(b); }
};

DEFINE_ELEMENT_TYPE_CASTS(SVGElement, isSVGElement());

template <typename T> bool isElementOfType(const SVGElement&);
template <> inline bool isElementOfType<const SVGElement>(const SVGElement&) { return true; }

inline bool Node::hasTagName(const SVGQualifiedName& name) const
{
    return isSVGElement() && toSVGElement(*this).hasTagName(name);
}

// This requires isSVG*Element(const SVGElement&).
#define DEFINE_SVGELEMENT_TYPE_CASTS_WITH_FUNCTION(thisType) \
    inline bool is##thisType(const thisType* element); \
    inline bool is##thisType(const thisType& element); \
    inline bool is##thisType(const SVGElement* element) { return element && is##thisType(*element); } \
    inline bool is##thisType(const Node& node) { return node.isSVGElement() ? is##thisType(toSVGElement(node)) : false; } \
    inline bool is##thisType(const Node* node) { return node && is##thisType(*node); } \
    template<typename T> inline bool is##thisType(const PassRefPtrWillBeRawPtr<T>& node) { return is##thisType(node.get()); } \
    template<typename T> inline bool is##thisType(const RefPtrWillBeMember<T>& node) { return is##thisType(node.get()); } \
    template <> inline bool isElementOfType<const thisType>(const SVGElement& element) { return is##thisType(element); } \
    DEFINE_ELEMENT_TYPE_CASTS_WITH_FUNCTION(thisType)

} // namespace blink

#include "core/SVGElementTypeHelpers.h"

#endif // SVGElement_h
