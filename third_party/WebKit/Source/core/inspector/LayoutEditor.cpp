// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/inspector/LayoutEditor.h"

#include "core/css/CSSComputedStyleDeclaration.h"
#include "core/css/CSSPrimitiveValue.h"
#include "core/frame/FrameView.h"
#include "core/inspector/InspectorCSSAgent.h"
#include "core/layout/LayoutBox.h"
#include "core/layout/LayoutInline.h"
#include "core/layout/LayoutObject.h"
#include "platform/JSONValues.h"

namespace blink {

namespace {

PassRefPtr<JSONObject> createAnchor(float x, float y, const String& propertyName, FloatPoint deltaVector, PassRefPtr<JSONObject> valueDescription)
{
    RefPtr<JSONObject> object = JSONObject::create();
    object->setNumber("x", x);
    object->setNumber("y", y);
    object->setString("propertyName", propertyName);

    RefPtr<JSONObject> deltaVectorJSON = JSONObject::create();
    deltaVectorJSON->setNumber("x", deltaVector.x());
    deltaVectorJSON->setNumber("y", deltaVector.y());
    object->setObject("deltaVector", deltaVectorJSON.release());
    object->setObject("propertyValue", valueDescription);
    return object.release();
}

void contentsQuadToViewport(const FrameView* view, FloatQuad& quad)
{
    quad.setP1(view->contentsToViewport(roundedIntPoint(quad.p1())));
    quad.setP2(view->contentsToViewport(roundedIntPoint(quad.p2())));
    quad.setP3(view->contentsToViewport(roundedIntPoint(quad.p3())));
    quad.setP4(view->contentsToViewport(roundedIntPoint(quad.p4())));
}

FloatPoint orthogonalVector(FloatPoint from, FloatPoint to, FloatPoint defaultVector)
{
    if (from == to)
        return defaultVector;

    return FloatPoint(to.y() - from.y(), from.x() - to.x());
}

} // namespace

LayoutEditor::LayoutEditor(InspectorCSSAgent* cssAgent)
    : m_node(nullptr)
    , m_cssAgent(cssAgent)
    , m_changingProperty(CSSPropertyInvalid)
    , m_propertyInitialValue(0)
{
}

DEFINE_TRACE(LayoutEditor)
{
    visitor->trace(m_node);
    visitor->trace(m_cssAgent);
}

void LayoutEditor::setNode(Node* node)
{
    m_node = node;
    m_changingProperty = CSSPropertyInvalid;
    m_propertyInitialValue = 0;
}

PassRefPtr<JSONObject> LayoutEditor::buildJSONInfo() const
{
    if (!m_node)
        return nullptr;

    LayoutObject* layoutObject = m_node->layoutObject();

    if (!layoutObject)
        return nullptr;

    FrameView* containingView = layoutObject->frameView();
    if (!containingView)
        return nullptr;
    if (!layoutObject->isBox() && !layoutObject->isLayoutInline())
        return nullptr;

    LayoutRect paddingBox;

    if (layoutObject->isBox()) {
        LayoutBox* layoutBox = toLayoutBox(layoutObject);

        // LayoutBox returns the "pure" content area box, exclusive of the scrollbars (if present), which also count towards the content area in CSS.
        const int verticalScrollbarWidth = layoutBox->verticalScrollbarWidth();
        const int horizontalScrollbarHeight = layoutBox->horizontalScrollbarHeight();

        paddingBox = layoutBox->paddingBoxRect();
        paddingBox.setWidth(paddingBox.width() + verticalScrollbarWidth);
        paddingBox.setHeight(paddingBox.height() + horizontalScrollbarHeight);
    } else {
        LayoutInline* layoutInline = toLayoutInline(layoutObject);
        LayoutRect borderBox = LayoutRect(layoutInline->linesBoundingBox());
        paddingBox = LayoutRect(borderBox.x() + layoutInline->borderLeft(), borderBox.y() + layoutInline->borderTop(),
            borderBox.width() - layoutInline->borderLeft() - layoutInline->borderRight(), borderBox.height() - layoutInline->borderTop() - layoutInline->borderBottom());
    }

    FloatQuad padding = layoutObject->localToAbsoluteQuad(FloatRect(paddingBox));
    contentsQuadToViewport(containingView, padding);

    float xLeft = (padding.p1().x() + padding.p4().x()) / 2;
    float yLeft = (padding.p1().y() + padding.p4().y()) / 2;
    FloatPoint orthoLeft = orthogonalVector(padding.p4(), padding.p1(), FloatPoint(-1, 0));

    float xRight = (padding.p2().x() + padding.p3().x()) / 2;
    float yRight = (padding.p2().y() + padding.p3().y()) / 2;
    FloatPoint orthoRight = orthogonalVector(padding.p2(), padding.p3(), FloatPoint(1, 0));

    RefPtr<JSONObject> object = JSONObject::create();
    RefPtr<JSONArray> anchors = JSONArray::create();

    RefPtr<JSONObject> paddingLeftDescription = createValueDescription("padding-left");
    if (paddingLeftDescription)
        anchors->pushObject(createAnchor(xLeft, yLeft, "padding-left", orthoLeft, paddingLeftDescription.release()));

    RefPtr<JSONObject> paddingRightDescription = createValueDescription("padding-right");
    if (paddingRightDescription)
        anchors->pushObject(createAnchor(xRight, yRight, "padding-right", orthoRight, paddingRightDescription.release()));

    object->setArray("anchors", anchors.release());
    return object.release();
}

RefPtrWillBeRawPtr<CSSPrimitiveValue> LayoutEditor::getPropertyCSSValue(CSSPropertyID property) const
{
    RefPtrWillBeRawPtr<CSSComputedStyleDeclaration> computedStyleInfo = CSSComputedStyleDeclaration::create(m_node, true);
    RefPtrWillBeRawPtr<CSSValue> cssValue = computedStyleInfo->getPropertyCSSValue(property);
    if (!cssValue->isPrimitiveValue())
        return nullptr;

    return toCSSPrimitiveValue(cssValue.get());
}

PassRefPtr<JSONObject> LayoutEditor::createValueDescription(const String& propertyName) const
{
    RefPtrWillBeRawPtr<CSSPrimitiveValue> cssValue = getPropertyCSSValue(cssPropertyID(propertyName));
    if (!cssValue)
        return nullptr;

    RefPtr<JSONObject> object = JSONObject::create();
    object->setNumber("value", cssValue->getFloatValue());
    object->setString("unit", "px");
    return object.release();
}

void LayoutEditor::overlayStartedPropertyChange(const String& anchorName)
{
    m_changingProperty = cssPropertyID(anchorName);
    if (!m_node || !m_changingProperty)
        return;

    RefPtrWillBeRawPtr<CSSPrimitiveValue> cssValue = getPropertyCSSValue(m_changingProperty);
    if (!cssValue) {
        m_changingProperty = CSSPropertyInvalid;
        return;
    }

    m_propertyInitialValue = cssValue->getFloatValue();
}

void LayoutEditor::overlayPropertyChanged(float cssDelta)
{
    if (m_changingProperty && m_node->isElementNode()) {
        String errorString;
        m_cssAgent->setCSSPropertyValue(&errorString, toElement(m_node.get()), m_changingProperty, String::number(cssDelta + m_propertyInitialValue) + "px");
    }
}

void LayoutEditor::overlayEndedPropertyChange()
{
    m_changingProperty = CSSPropertyInvalid;
    m_propertyInitialValue = 0;
}

} // namespace blink
