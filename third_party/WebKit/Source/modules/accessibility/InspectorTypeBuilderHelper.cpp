// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/accessibility/InspectorTypeBuilderHelper.h"

#include "core/InspectorTypeBuilder.h"
#include "core/dom/DOMNodeIds.h"
#include "modules/accessibility/AXObject.h"
#include "modules/accessibility/AXObjectCacheImpl.h"

namespace blink {

using TypeBuilder::Accessibility::AXRelatedNode;

PassRefPtr<AXProperty> createProperty(String name, PassRefPtr<AXValue> value)
{
    RefPtr<AXProperty> property = AXProperty::create().setName(name).setValue(value);
    return property;
}

PassRefPtr<AXProperty> createProperty(AXGlobalStates::Enum name, PassRefPtr<AXValue> value)
{
    return createProperty(TypeBuilder::getEnumConstantValue(name), value);
}

PassRefPtr<AXProperty> createProperty(AXLiveRegionAttributes::Enum name, PassRefPtr<AXValue> value)
{
    return createProperty(TypeBuilder::getEnumConstantValue(name), value);
}

PassRefPtr<AXProperty> createProperty(AXRelationshipAttributes::Enum name, PassRefPtr<AXValue> value)
{
    return createProperty(TypeBuilder::getEnumConstantValue(name), value);
}

PassRefPtr<AXProperty> createProperty(AXWidgetAttributes::Enum name, PassRefPtr<AXValue> value)
{
    return createProperty(TypeBuilder::getEnumConstantValue(name), value);
}

PassRefPtr<AXProperty> createProperty(AXWidgetStates::Enum name, PassRefPtr<AXValue> value)
{
    return createProperty(TypeBuilder::getEnumConstantValue(name), value);
}

String ignoredReasonName(AXIgnoredReason reason)
{
    switch (reason) {
    case AXActiveModalDialog:
        return "activeModalDialog";
    case AXAncestorDisallowsChild:
        return "ancestorDisallowsChild";
    case AXAncestorIsLeafNode:
        return "ancestorIsLeafNode";
    case AXAriaHidden:
        return "ariaHidden";
    case AXAriaHiddenRoot:
        return "ariaHiddenRoot";
    case AXEmptyAlt:
        return "emptyAlt";
    case AXEmptyText:
        return "emptyText";
    case AXInert:
        return "inert";
    case AXInheritsPresentation:
        return "inheritsPresentation";
    case AXLabelContainer:
        return "labelContainer";
    case AXLabelFor:
        return "labelFor";
    case AXNotRendered:
        return "notRendered";
    case AXNotVisible:
        return "notVisible";
    case AXPresentationalRole:
        return "presentationalRole";
    case AXProbablyPresentational:
        return "probablyPresentational";
    case AXStaticTextUsedAsNameFor:
        return "staticTextUsedAsNameFor";
    case AXUninteresting:
        return "uninteresting";
    }
    ASSERT_NOT_REACHED();
    return "";
}

PassRefPtr<AXProperty> createProperty(IgnoredReason reason)
{
    if (reason.relatedObject)
        return createProperty(ignoredReasonName(reason.reason), createRelatedNodeValue(reason.relatedObject));
    return createProperty(ignoredReasonName(reason.reason), createBooleanValue(true));
}

PassRefPtr<AXValue> createValue(String value, AXValueType::Enum type)
{
    RefPtr<AXValue> axValue = AXValue::create().setType(type);
    axValue->setValue(JSONString::create(value));
    return axValue;
}

PassRefPtr<AXValue> createValue(int value, AXValueType::Enum type)
{
    RefPtr<AXValue> axValue = AXValue::create().setType(type);
    axValue->setValue(JSONBasicValue::create(value));
    return axValue;
}

PassRefPtr<AXValue> createValue(float value, AXValueType::Enum type)
{
    RefPtr<AXValue> axValue = AXValue::create().setType(type);
    axValue->setValue(JSONBasicValue::create(value));
    return axValue;
}

PassRefPtr<AXValue> createBooleanValue(bool value, AXValueType::Enum type)
{
    RefPtr<AXValue> axValue = AXValue::create().setType(type);
    axValue->setValue(JSONBasicValue::create(value));
    return axValue;
}

PassRefPtr<AXRelatedNode> relatedNodeForAXObject(const AXObject* axObject)
{
    Node* node = axObject->node();
    if (!node)
        return PassRefPtr<AXRelatedNode>();
    int backendNodeId = DOMNodeIds::idForNode(node);
    if (!backendNodeId)
        return PassRefPtr<AXRelatedNode>();
    RefPtr<AXRelatedNode> relatedNode = AXRelatedNode::create().setBackendNodeId(backendNodeId);
    if (!node->isElementNode())
        return relatedNode;

    Element* element = toElement(node);
    const AtomicString& idref = element->getIdAttribute();
    if (!idref.isEmpty())
        relatedNode->setIdref(idref);
    return relatedNode;
}

PassRefPtr<AXValue> createRelatedNodeValue(const AXObject* axObject)
{
    RefPtr<AXValue> axValue = AXValue::create().setType(AXValueType::Idref);
    RefPtr<AXRelatedNode> relatedNode = relatedNodeForAXObject(axObject);
    axValue->setRelatedNodeValue(relatedNode);
    return axValue;
}

PassRefPtr<AXValue> createRelatedNodeListValue(AXObject::AccessibilityChildrenVector axObjects)
{
    RefPtr<TypeBuilder::Array<AXRelatedNode>> relatedNodes = TypeBuilder::Array<AXRelatedNode>::create();
    for (unsigned i = 0; i < axObjects.size(); i++) {
        if (RefPtr<AXRelatedNode> relatedNode = relatedNodeForAXObject(axObjects[i].get()))
            relatedNodes->addItem(relatedNode);
    }
    RefPtr<AXValue> axValue = AXValue::create().setType(AXValueType::IdrefList);
    axValue->setRelatedNodeArrayValue(relatedNodes);
    return axValue;
}

} // namespace blink
