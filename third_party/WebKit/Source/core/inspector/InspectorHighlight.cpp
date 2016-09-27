// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/inspector/InspectorHighlight.h"

#include "core/dom/ClientRect.h"
#include "core/dom/PseudoElement.h"
#include "core/frame/FrameView.h"
#include "core/layout/LayoutBox.h"
#include "core/layout/LayoutInline.h"
#include "core/layout/LayoutObject.h"
#include "core/layout/shapes/ShapeOutsideInfo.h"
#include "core/style/ComputedStyleConstants.h"
#include "platform/graphics/Path.h"

namespace blink {

namespace {

class PathBuilder {
    WTF_MAKE_NONCOPYABLE(PathBuilder);
public:
    PathBuilder() : m_path(TypeBuilder::Array<JSONValue>::create()) { }
    virtual ~PathBuilder() { }

    PassRefPtr<TypeBuilder::Array<JSONValue>> path() const { return m_path; }
    void appendPath(const Path& path)
    {
        path.apply(this, &PathBuilder::appendPathElement);
    }

protected:
    virtual FloatPoint translatePoint(const FloatPoint& point) { return point; }

private:
    static void appendPathElement(void* pathBuilder, const PathElement* pathElement)
    {
        static_cast<PathBuilder*>(pathBuilder)->appendPathElement(pathElement);
    }

    void appendPathElement(const PathElement*);
    void appendPathCommandAndPoints(const char* command, const FloatPoint points[], size_t length);

    RefPtr<TypeBuilder::Array<JSONValue>> m_path;
};

void PathBuilder::appendPathCommandAndPoints(const char* command, const FloatPoint points[], size_t length)
{
    m_path->addItem(JSONString::create(command));
    for (size_t i = 0; i < length; i++) {
        FloatPoint point = translatePoint(points[i]);
        m_path->addItem(JSONBasicValue::create(point.x()));
        m_path->addItem(JSONBasicValue::create(point.y()));
    }
}

void PathBuilder::appendPathElement(const PathElement* pathElement)
{
    switch (pathElement->type) {
    // The points member will contain 1 value.
    case PathElementMoveToPoint:
        appendPathCommandAndPoints("M", pathElement->points, 1);
        break;
    // The points member will contain 1 value.
    case PathElementAddLineToPoint:
        appendPathCommandAndPoints("L", pathElement->points, 1);
        break;
    // The points member will contain 3 values.
    case PathElementAddCurveToPoint:
        appendPathCommandAndPoints("C", pathElement->points, 3);
        break;
    // The points member will contain 2 values.
    case PathElementAddQuadCurveToPoint:
        appendPathCommandAndPoints("Q", pathElement->points, 2);
        break;
    // The points member will contain no values.
    case PathElementCloseSubpath:
        appendPathCommandAndPoints("Z", 0, 0);
        break;
    }
}

class ShapePathBuilder : public PathBuilder {
public:
    ShapePathBuilder(FrameView& view, LayoutObject& layoutObject, const ShapeOutsideInfo& shapeOutsideInfo)
        : m_view(view)
        , m_layoutObject(layoutObject)
        , m_shapeOutsideInfo(shapeOutsideInfo) { }

    static PassRefPtr<TypeBuilder::Array<JSONValue>> buildPath(FrameView& view, LayoutObject& layoutObject, const ShapeOutsideInfo& shapeOutsideInfo, const Path& path)
    {
        ShapePathBuilder builder(view, layoutObject, shapeOutsideInfo);
        builder.appendPath(path);
        return builder.path();
    }

protected:
    virtual FloatPoint translatePoint(const FloatPoint& point)
    {
        FloatPoint layoutObjectPoint = m_shapeOutsideInfo.shapeToLayoutObjectPoint(point);
        return m_view.contentsToViewport(roundedIntPoint(m_layoutObject.localToAbsolute(layoutObjectPoint)));
    }

private:
    FrameView& m_view;
    LayoutObject& m_layoutObject;
    const ShapeOutsideInfo& m_shapeOutsideInfo;
};


PassRefPtr<TypeBuilder::Array<double>> buildArrayForQuad(const FloatQuad& quad)
{
    RefPtr<TypeBuilder::Array<double>> array = TypeBuilder::Array<double>::create();
    array->addItem(quad.p1().x());
    array->addItem(quad.p1().y());
    array->addItem(quad.p2().x());
    array->addItem(quad.p2().y());
    array->addItem(quad.p3().x());
    array->addItem(quad.p3().y());
    array->addItem(quad.p4().x());
    array->addItem(quad.p4().y());
    return array.release();
}

Path quadToPath(const FloatQuad& quad)
{
    Path quadPath;
    quadPath.moveTo(quad.p1());
    quadPath.addLineTo(quad.p2());
    quadPath.addLineTo(quad.p3());
    quadPath.addLineTo(quad.p4());
    quadPath.closeSubpath();
    return quadPath;
}

void contentsQuadToViewport(const FrameView* view, FloatQuad& quad)
{
    quad.setP1(view->contentsToViewport(roundedIntPoint(quad.p1())));
    quad.setP2(view->contentsToViewport(roundedIntPoint(quad.p2())));
    quad.setP3(view->contentsToViewport(roundedIntPoint(quad.p3())));
    quad.setP4(view->contentsToViewport(roundedIntPoint(quad.p4())));
}

const ShapeOutsideInfo* shapeOutsideInfoForNode(Node* node, Shape::DisplayPaths* paths, FloatQuad* bounds)
{
    LayoutObject* layoutObject = node->layoutObject();
    if (!layoutObject || !layoutObject->isBox() || !toLayoutBox(layoutObject)->shapeOutsideInfo())
        return nullptr;

    FrameView* containingView = node->document().view();
    LayoutBox* layoutBox = toLayoutBox(layoutObject);
    const ShapeOutsideInfo* shapeOutsideInfo = layoutBox->shapeOutsideInfo();

    shapeOutsideInfo->computedShape().buildDisplayPaths(*paths);

    LayoutRect shapeBounds = shapeOutsideInfo->computedShapePhysicalBoundingBox();
    *bounds = layoutBox->localToAbsoluteQuad(FloatRect(shapeBounds));
    contentsQuadToViewport(containingView, *bounds);

    return shapeOutsideInfo;
}

bool buildNodeQuads(LayoutObject* layoutObject, FloatQuad* content, FloatQuad* padding, FloatQuad* border, FloatQuad* margin)
{
    FrameView* containingView = layoutObject->frameView();
    if (!containingView)
        return false;
    if (!layoutObject->isBox() && !layoutObject->isLayoutInline())
        return false;

    LayoutRect contentBox;
    LayoutRect paddingBox;
    LayoutRect borderBox;
    LayoutRect marginBox;

    if (layoutObject->isBox()) {
        LayoutBox* layoutBox = toLayoutBox(layoutObject);

        // LayoutBox returns the "pure" content area box, exclusive of the scrollbars (if present), which also count towards the content area in CSS.
        const int verticalScrollbarWidth = layoutBox->verticalScrollbarWidth();
        const int horizontalScrollbarHeight = layoutBox->horizontalScrollbarHeight();
        contentBox = layoutBox->contentBoxRect();
        contentBox.setWidth(contentBox.width() + verticalScrollbarWidth);
        contentBox.setHeight(contentBox.height() + horizontalScrollbarHeight);

        paddingBox = layoutBox->paddingBoxRect();
        paddingBox.setWidth(paddingBox.width() + verticalScrollbarWidth);
        paddingBox.setHeight(paddingBox.height() + horizontalScrollbarHeight);

        borderBox = layoutBox->borderBoxRect();

        marginBox = LayoutRect(borderBox.x() - layoutBox->marginLeft(), borderBox.y() - layoutBox->marginTop(),
            borderBox.width() + layoutBox->marginWidth(), borderBox.height() + layoutBox->marginHeight());
    } else {
        LayoutInline* layoutInline = toLayoutInline(layoutObject);

        // LayoutInline's bounding box includes paddings and borders, excludes margins.
        borderBox = LayoutRect(layoutInline->linesBoundingBox());
        paddingBox = LayoutRect(borderBox.x() + layoutInline->borderLeft(), borderBox.y() + layoutInline->borderTop(),
            borderBox.width() - layoutInline->borderLeft() - layoutInline->borderRight(), borderBox.height() - layoutInline->borderTop() - layoutInline->borderBottom());
        contentBox = LayoutRect(paddingBox.x() + layoutInline->paddingLeft(), paddingBox.y() + layoutInline->paddingTop(),
            paddingBox.width() - layoutInline->paddingLeft() - layoutInline->paddingRight(), paddingBox.height() - layoutInline->paddingTop() - layoutInline->paddingBottom());
        // Ignore marginTop and marginBottom for inlines.
        marginBox = LayoutRect(borderBox.x() - layoutInline->marginLeft(), borderBox.y(),
            borderBox.width() + layoutInline->marginWidth(), borderBox.height());
    }

    *content = layoutObject->localToAbsoluteQuad(FloatRect(contentBox));
    *padding = layoutObject->localToAbsoluteQuad(FloatRect(paddingBox));
    *border = layoutObject->localToAbsoluteQuad(FloatRect(borderBox));
    *margin = layoutObject->localToAbsoluteQuad(FloatRect(marginBox));

    contentsQuadToViewport(containingView, *content);
    contentsQuadToViewport(containingView, *padding);
    contentsQuadToViewport(containingView, *border);
    contentsQuadToViewport(containingView, *margin);

    return true;
}

PassRefPtr<JSONObject> buildElementInfo(Element* element)
{
    RefPtr<JSONObject> elementInfo = JSONObject::create();
    Element* realElement = element;
    PseudoElement* pseudoElement = nullptr;
    if (element->isPseudoElement()) {
        pseudoElement = toPseudoElement(element);
        realElement = element->parentOrShadowHostElement();
    }
    bool isXHTML = realElement->document().isXHTMLDocument();
    elementInfo->setString("tagName", isXHTML ? realElement->nodeName() : realElement->nodeName().lower());
    elementInfo->setString("idValue", realElement->getIdAttribute());
    StringBuilder classNames;
    if (realElement->hasClass() && realElement->isStyledElement()) {
        HashSet<AtomicString> usedClassNames;
        const SpaceSplitString& classNamesString = realElement->classNames();
        size_t classNameCount = classNamesString.size();
        for (size_t i = 0; i < classNameCount; ++i) {
            const AtomicString& className = classNamesString[i];
            if (!usedClassNames.add(className).isNewEntry)
                continue;
            classNames.append('.');
            classNames.append(className);
        }
    }
    if (pseudoElement) {
        if (pseudoElement->pseudoId() == BEFORE)
            classNames.appendLiteral("::before");
        else if (pseudoElement->pseudoId() == AFTER)
            classNames.appendLiteral("::after");
    }
    if (!classNames.isEmpty())
        elementInfo->setString("className", classNames.toString());

    LayoutObject* layoutObject = element->layoutObject();
    FrameView* containingView = element->document().view();
    if (!layoutObject || !containingView)
        return elementInfo;

    // layoutObject the getBoundingClientRect() data in the tooltip
    // to be consistent with the rulers (see http://crbug.com/262338).
    ClientRect* boundingBox = element->getBoundingClientRect();
    elementInfo->setString("nodeWidth", String::number(boundingBox->width()));
    elementInfo->setString("nodeHeight", String::number(boundingBox->height()));

    return elementInfo;
}

} // namespace

InspectorHighlight::InspectorHighlight()
    : m_highlightPaths(JSONArray::create())
    , m_showRulers(false)
    , m_showExtensionLines(false)
{
}

InspectorHighlightConfig::InspectorHighlightConfig()
    : showInfo(false)
    , showRulers(false)
    , showExtensionLines(false)
    , showLayoutEditor(false)
{
}

InspectorHighlight::InspectorHighlight(Node* node, const InspectorHighlightConfig& highlightConfig, bool appendElementInfo)
    : m_highlightPaths(JSONArray::create())
    , m_showRulers(highlightConfig.showRulers)
    , m_showExtensionLines(highlightConfig.showExtensionLines)
{
    appendPathsForShapeOutside(node, highlightConfig);
    appendNodeHighlight(node, highlightConfig);
    if (appendElementInfo && node->isElementNode())
        m_elementInfo = buildElementInfo(toElement(node));
}

InspectorHighlight::~InspectorHighlight()
{
}

void InspectorHighlight::appendQuad(const FloatQuad& quad, const Color& fillColor, const Color& outlineColor)
{
    Path path = quadToPath(quad);
    PathBuilder builder;
    builder.appendPath(path);
    appendPath(builder.path(), fillColor, outlineColor);
}

void InspectorHighlight::appendPath(PassRefPtr<JSONArrayBase> path, const Color& fillColor, const Color& outlineColor)
{
    RefPtr<JSONObject> object = JSONObject::create();
    object->setValue("path", path);
    object->setString("fillColor", fillColor.serialized());
    if (outlineColor != Color::transparent)
        object->setString("outlineColor", outlineColor.serialized());
    m_highlightPaths->pushObject(object.release());
}

void InspectorHighlight::appendEventTargetQuads(Node* eventTargetNode, const InspectorHighlightConfig& highlightConfig)
{
    if (eventTargetNode->layoutObject()) {
        FloatQuad border, unused;
        if (buildNodeQuads(eventTargetNode->layoutObject(), &unused, &unused, &border, &unused))
            appendQuad(border, highlightConfig.eventTarget);
    }
}

void InspectorHighlight::appendPathsForShapeOutside(Node* node, const InspectorHighlightConfig& config)
{
    Shape::DisplayPaths paths;
    FloatQuad boundsQuad;

    const ShapeOutsideInfo* shapeOutsideInfo = shapeOutsideInfoForNode(node, &paths, &boundsQuad);
    if (!shapeOutsideInfo)
        return;

    if (!paths.shape.length()) {
        appendQuad(boundsQuad, config.shape);
        return;
    }

    appendPath(ShapePathBuilder::buildPath(*node->document().view(), *node->layoutObject(), *shapeOutsideInfo, paths.shape), config.shape, Color::transparent);
    if (paths.marginShape.length())
        appendPath(ShapePathBuilder::buildPath(*node->document().view(), *node->layoutObject(), *shapeOutsideInfo, paths.marginShape), config.shapeMargin, Color::transparent);
}

void InspectorHighlight::appendNodeHighlight(Node* node, const InspectorHighlightConfig& highlightConfig)
{
    LayoutObject* layoutObject = node->layoutObject();
    if (!layoutObject)
        return;

    // LayoutSVGRoot should be highlighted through the isBox() code path, all other SVG elements should just dump their absoluteQuads().
    if (layoutObject->node() && layoutObject->node()->isSVGElement() && !layoutObject->isSVGRoot()) {
        Vector<FloatQuad> quads;
        layoutObject->absoluteQuads(quads);
        FrameView* containingView = layoutObject->frameView();
        for (size_t i = 0; i < quads.size(); ++i) {
            if (containingView)
                contentsQuadToViewport(containingView, quads[i]);
            appendQuad(quads[i], highlightConfig.content, highlightConfig.contentOutline);
        }
        return;
    }

    FloatQuad content, padding, border, margin;
    if (!buildNodeQuads(layoutObject, &content, &padding, &border, &margin))
        return;
    appendQuad(content, highlightConfig.content, highlightConfig.contentOutline);
    appendQuad(padding, highlightConfig.padding);
    appendQuad(border, highlightConfig.border);
    appendQuad(margin, highlightConfig.margin);
}

PassRefPtr<JSONObject> InspectorHighlight::asJSONObject() const
{
    RefPtr<JSONObject> object = JSONObject::create();
    object->setArray("paths", m_highlightPaths);
    object->setBoolean("showRulers", m_showRulers);
    object->setBoolean("showExtensionLines", m_showExtensionLines);
    if (m_elementInfo)
        object->setObject("elementInfo", m_elementInfo);
    return object.release();
}

// static
bool InspectorHighlight::getBoxModel(Node* node, RefPtr<TypeBuilder::DOM::BoxModel>& model)
{
    LayoutObject* layoutObject = node->layoutObject();
    FrameView* view = node->document().view();
    if (!layoutObject || !view)
        return false;

    FloatQuad content, padding, border, margin;
    if (!buildNodeQuads(node->layoutObject(), &content, &padding, &border, &margin))
        return false;

    IntRect boundingBox = view->contentsToRootFrame(layoutObject->absoluteBoundingBoxRect());
    LayoutBoxModelObject* modelObject = layoutObject->isBoxModelObject() ? toLayoutBoxModelObject(layoutObject) : nullptr;

    model = TypeBuilder::DOM::BoxModel::create()
        .setContent(buildArrayForQuad(content))
        .setPadding(buildArrayForQuad(padding))
        .setBorder(buildArrayForQuad(border))
        .setMargin(buildArrayForQuad(margin))
        .setWidth(modelObject ? adjustForAbsoluteZoom(modelObject->pixelSnappedOffsetWidth(), modelObject) : boundingBox.width())
        .setHeight(modelObject ? adjustForAbsoluteZoom(modelObject->pixelSnappedOffsetHeight(), modelObject) : boundingBox.height());

    Shape::DisplayPaths paths;
    FloatQuad boundsQuad;
    if (const ShapeOutsideInfo* shapeOutsideInfo = shapeOutsideInfoForNode(node, &paths, &boundsQuad)) {
        RefPtr<TypeBuilder::DOM::ShapeOutsideInfo> shapeTypeBuilder = TypeBuilder::DOM::ShapeOutsideInfo::create()
            .setBounds(buildArrayForQuad(boundsQuad))
            .setShape(ShapePathBuilder::buildPath(*view, *layoutObject, *shapeOutsideInfo, paths.shape))
            .setMarginShape(ShapePathBuilder::buildPath(*view, *layoutObject, *shapeOutsideInfo, paths.marginShape));
        model->setShapeOutside(shapeTypeBuilder);
    }

    return true;
}

// static
InspectorHighlightConfig InspectorHighlight::defaultConfig()
{
    InspectorHighlightConfig config;
    config.content = Color(255, 0, 0, 0);
    config.contentOutline = Color(128, 0, 0, 0);
    config.padding = Color(0, 255, 0, 0);
    config.border = Color(0, 0, 255, 0);
    config.margin = Color(255, 255, 255, 0);
    config.eventTarget = Color(128, 128, 128, 0);
    config.shape = Color(0, 0, 0, 0);
    config.shapeMargin = Color(128, 128, 128, 0);
    config.showInfo = true;
    config.showRulers = true;
    config.showExtensionLines = true;
    config.showLayoutEditor = false;
    return config;
}

} // namespace blink
