// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "SVGElementFactory.h"

#include "SVGNames.h"
#include "core/svg/SVGAElement.h"
#include "core/svg/SVGAnimateElement.h"
#include "core/svg/SVGAnimateMotionElement.h"
#include "core/svg/SVGAnimateTransformElement.h"
#include "core/svg/SVGCircleElement.h"
#include "core/svg/SVGClipPathElement.h"
#include "core/svg/SVGCursorElement.h"
#include "core/svg/SVGDefsElement.h"
#include "core/svg/SVGDescElement.h"
#include "core/svg/SVGDiscardElement.h"
#include "core/svg/SVGEllipseElement.h"
#include "core/svg/SVGFEBlendElement.h"
#include "core/svg/SVGFEColorMatrixElement.h"
#include "core/svg/SVGFEComponentTransferElement.h"
#include "core/svg/SVGFECompositeElement.h"
#include "core/svg/SVGFEConvolveMatrixElement.h"
#include "core/svg/SVGFEDiffuseLightingElement.h"
#include "core/svg/SVGFEDisplacementMapElement.h"
#include "core/svg/SVGFEDistantLightElement.h"
#include "core/svg/SVGFEDropShadowElement.h"
#include "core/svg/SVGFEFloodElement.h"
#include "core/svg/SVGFEFuncAElement.h"
#include "core/svg/SVGFEFuncBElement.h"
#include "core/svg/SVGFEFuncGElement.h"
#include "core/svg/SVGFEFuncRElement.h"
#include "core/svg/SVGFEGaussianBlurElement.h"
#include "core/svg/SVGFEImageElement.h"
#include "core/svg/SVGFEMergeElement.h"
#include "core/svg/SVGFEMergeNodeElement.h"
#include "core/svg/SVGFEMorphologyElement.h"
#include "core/svg/SVGFEOffsetElement.h"
#include "core/svg/SVGFEPointLightElement.h"
#include "core/svg/SVGFESpecularLightingElement.h"
#include "core/svg/SVGFESpotLightElement.h"
#include "core/svg/SVGFETileElement.h"
#include "core/svg/SVGFETurbulenceElement.h"
#include "core/svg/SVGFilterElement.h"
#include "core/svg/SVGForeignObjectElement.h"
#include "core/svg/SVGGElement.h"
#include "core/svg/SVGImageElement.h"
#include "core/svg/SVGLineElement.h"
#include "core/svg/SVGLinearGradientElement.h"
#include "core/svg/SVGMPathElement.h"
#include "core/svg/SVGMarkerElement.h"
#include "core/svg/SVGMaskElement.h"
#include "core/svg/SVGMetadataElement.h"
#include "core/svg/SVGPathElement.h"
#include "core/svg/SVGPatternElement.h"
#include "core/svg/SVGPolygonElement.h"
#include "core/svg/SVGPolylineElement.h"
#include "core/svg/SVGRadialGradientElement.h"
#include "core/svg/SVGRectElement.h"
#include "core/svg/SVGSVGElement.h"
#include "core/svg/SVGScriptElement.h"
#include "core/svg/SVGSetElement.h"
#include "core/svg/SVGStopElement.h"
#include "core/svg/SVGStyleElement.h"
#include "core/svg/SVGSwitchElement.h"
#include "core/svg/SVGSymbolElement.h"
#include "core/svg/SVGTSpanElement.h"
#include "core/svg/SVGTextElement.h"
#include "core/svg/SVGTextPathElement.h"
#include "core/svg/SVGTitleElement.h"
#include "core/svg/SVGUnknownElement.h"
#include "core/svg/SVGUseElement.h"
#include "core/svg/SVGViewElement.h"
#include "core/svg/SVGUnknownElement.h"
#include "core/dom/custom/CustomElement.h"
#include "core/dom/custom/CustomElementRegistrationContext.h"
#include "core/dom/Document.h"
#include "core/frame/Settings.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "wtf/HashMap.h"

namespace blink {

using namespace SVGNames;

typedef PassRefPtrWillBeRawPtr<SVGElement> (*ConstructorFunction)(
    Document&,
    bool createdByParser);

typedef HashMap<AtomicString, ConstructorFunction> FunctionMap;

static FunctionMap* g_constructors = 0;

static PassRefPtrWillBeRawPtr<SVGElement> aConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGAElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> animateConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGAnimateElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> animateMotionConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGAnimateMotionElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> animateTransformConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGAnimateTransformElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> circleConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGCircleElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> clipPathConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGClipPathElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> cursorConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGCursorElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> defsConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGDefsElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> descConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGDescElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> discardConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGDiscardElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> ellipseConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGEllipseElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> feBlendConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGFEBlendElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> feColorMatrixConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGFEColorMatrixElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> feComponentTransferConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGFEComponentTransferElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> feCompositeConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGFECompositeElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> feConvolveMatrixConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGFEConvolveMatrixElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> feDiffuseLightingConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGFEDiffuseLightingElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> feDisplacementMapConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGFEDisplacementMapElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> feDistantLightConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGFEDistantLightElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> feDropShadowConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGFEDropShadowElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> feFloodConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGFEFloodElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> feFuncAConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGFEFuncAElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> feFuncBConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGFEFuncBElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> feFuncGConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGFEFuncGElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> feFuncRConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGFEFuncRElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> feGaussianBlurConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGFEGaussianBlurElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> feImageConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGFEImageElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> feMergeConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGFEMergeElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> feMergeNodeConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGFEMergeNodeElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> feMorphologyConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGFEMorphologyElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> feOffsetConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGFEOffsetElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> fePointLightConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGFEPointLightElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> feSpecularLightingConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGFESpecularLightingElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> feSpotLightConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGFESpotLightElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> feTileConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGFETileElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> feTurbulenceConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGFETurbulenceElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> filterConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGFilterElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> foreignObjectConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGForeignObjectElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> gConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGGElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> imageConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGImageElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> lineConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGLineElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> linearGradientConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGLinearGradientElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> mpathConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGMPathElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> markerConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGMarkerElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> maskConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGMaskElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> metadataConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGMetadataElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> pathConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGPathElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> patternConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGPatternElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> polygonConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGPolygonElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> polylineConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGPolylineElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> radialGradientConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGRadialGradientElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> rectConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGRectElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> svgConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGSVGElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> setConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGSetElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> stopConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGStopElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> switchConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGSwitchElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> symbolConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGSymbolElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> tspanConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGTSpanElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> textConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGTextElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> textPathConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGTextPathElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> titleConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGTitleElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> useConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGUseElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> viewConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGViewElement::create(document);
}
static PassRefPtrWillBeRawPtr<SVGElement> scriptConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGScriptElement::create(document, createdByParser);
}
static PassRefPtrWillBeRawPtr<SVGElement> styleConstructor(
    Document& document,
    bool createdByParser)
{
    return SVGStyleElement::create(document, createdByParser);
}

struct CreateSVGFunctionMapData {
  const QualifiedName& tag;
  ConstructorFunction func;
};

static void createSVGFunctionMap()
{
    ASSERT(!g_constructors);
    g_constructors = new FunctionMap;
    // Empty array initializer lists are illegal [dcl.init.aggr] and will not
    // compile in MSVC. If tags list is empty, add check to skip this.
    static const CreateSVGFunctionMapData data[] = {
        { aTag, aConstructor },
        { animateTag, animateConstructor },
        { animateMotionTag, animateMotionConstructor },
        { animateTransformTag, animateTransformConstructor },
        { circleTag, circleConstructor },
        { clipPathTag, clipPathConstructor },
        { cursorTag, cursorConstructor },
        { defsTag, defsConstructor },
        { descTag, descConstructor },
        { discardTag, discardConstructor },
        { ellipseTag, ellipseConstructor },
        { feBlendTag, feBlendConstructor },
        { feColorMatrixTag, feColorMatrixConstructor },
        { feComponentTransferTag, feComponentTransferConstructor },
        { feCompositeTag, feCompositeConstructor },
        { feConvolveMatrixTag, feConvolveMatrixConstructor },
        { feDiffuseLightingTag, feDiffuseLightingConstructor },
        { feDisplacementMapTag, feDisplacementMapConstructor },
        { feDistantLightTag, feDistantLightConstructor },
        { feDropShadowTag, feDropShadowConstructor },
        { feFloodTag, feFloodConstructor },
        { feFuncATag, feFuncAConstructor },
        { feFuncBTag, feFuncBConstructor },
        { feFuncGTag, feFuncGConstructor },
        { feFuncRTag, feFuncRConstructor },
        { feGaussianBlurTag, feGaussianBlurConstructor },
        { feImageTag, feImageConstructor },
        { feMergeTag, feMergeConstructor },
        { feMergeNodeTag, feMergeNodeConstructor },
        { feMorphologyTag, feMorphologyConstructor },
        { feOffsetTag, feOffsetConstructor },
        { fePointLightTag, fePointLightConstructor },
        { feSpecularLightingTag, feSpecularLightingConstructor },
        { feSpotLightTag, feSpotLightConstructor },
        { feTileTag, feTileConstructor },
        { feTurbulenceTag, feTurbulenceConstructor },
        { filterTag, filterConstructor },
        { foreignObjectTag, foreignObjectConstructor },
        { gTag, gConstructor },
        { imageTag, imageConstructor },
        { lineTag, lineConstructor },
        { linearGradientTag, linearGradientConstructor },
        { mpathTag, mpathConstructor },
        { markerTag, markerConstructor },
        { maskTag, maskConstructor },
        { metadataTag, metadataConstructor },
        { pathTag, pathConstructor },
        { patternTag, patternConstructor },
        { polygonTag, polygonConstructor },
        { polylineTag, polylineConstructor },
        { radialGradientTag, radialGradientConstructor },
        { rectTag, rectConstructor },
        { svgTag, svgConstructor },
        { setTag, setConstructor },
        { stopTag, stopConstructor },
        { switchTag, switchConstructor },
        { symbolTag, symbolConstructor },
        { tspanTag, tspanConstructor },
        { textTag, textConstructor },
        { textPathTag, textPathConstructor },
        { titleTag, titleConstructor },
        { useTag, useConstructor },
        { viewTag, viewConstructor },
        { scriptTag, scriptConstructor },
        { styleTag, styleConstructor },
    };
    for (size_t i = 0; i < WTF_ARRAY_LENGTH(data); i++)
        g_constructors->set(data[i].tag.localName(), data[i].func);
}

PassRefPtrWillBeRawPtr<SVGElement> SVGElementFactory::createSVGElement(
    const AtomicString& localName,
    Document& document,
    bool createdByParser)
{
    if (!g_constructors)
        createSVGFunctionMap();
    if (ConstructorFunction function = g_constructors->get(localName))
        return function(document, createdByParser);

#ifdef MINIBLINK_NOT_IMPLEMENTED
    if (document.registrationContext() && CustomElement::isValidName(localName)) {
        RefPtrWillBeRawPtr<Element> element = document.registrationContext()->createCustomTagElement(document, QualifiedName(nullAtom, localName, svgNamespaceURI));
        ASSERT_WITH_SECURITY_IMPLICATION(element->isSVGElement());
        return static_pointer_cast<SVGElement>(element.release());
    }
#endif // MINIBLINK_NOT_IMPLEMENTED

    return SVGUnknownElement::create(QualifiedName(nullAtom, localName, svgNamespaceURI), document);
}

} // namespace blink
