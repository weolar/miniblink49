#include "config.h"
#include "StyleBuilderFunctions.h"

#include "CSSValueKeywords.h"
#include "core/css/BasicShapeFunctions.h"
#include "core/css/CSSContentDistributionValue.h"
#include "core/css/CSSPrimitiveValueMappings.h"
#include "core/css/Pair.h"
#include "core/css/resolver/StyleResolverState.h"


namespace blink {

void StyleBuilderFunctions::applyInitialCSSPropertyVectorEffect(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setVectorEffect(SVGComputedStyle::initialVectorEffect());
}

void StyleBuilderFunctions::applyInheritCSSPropertyVectorEffect(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setVectorEffect(state.parentStyle()->svgStyle().vectorEffect());
}

void StyleBuilderFunctions::applyValueCSSPropertyVectorEffect(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle().setVectorEffect(static_cast<EVectorEffect>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyFillRule(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setFillRule(SVGComputedStyle::initialFillRule());
}

void StyleBuilderFunctions::applyInheritCSSPropertyFillRule(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setFillRule(state.parentStyle()->svgStyle().fillRule());
}

void StyleBuilderFunctions::applyValueCSSPropertyFillRule(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle().setFillRule(static_cast<WindRule>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitLineBoxContain(StyleResolverState& state)
{
    state.style()->setLineBoxContain(ComputedStyle::initialLineBoxContain());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitLineBoxContain(StyleResolverState& state)
{
    state.style()->setLineBoxContain(state.parentStyle()->lineBoxContain());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitLineBoxContain(StyleResolverState& state, CSSValue* value)
{
    state.style()->setLineBoxContain(StyleBuilderConverter::convertLineBoxContain(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyCx(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setCx(SVGComputedStyle::initialCx());
}

void StyleBuilderFunctions::applyInheritCSSPropertyCx(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setCx(state.parentStyle()->svgStyle().cx());
}

void StyleBuilderFunctions::applyValueCSSPropertyCx(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle().setCx(StyleBuilderConverter::convertLength(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyCy(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setCy(SVGComputedStyle::initialCy());
}

void StyleBuilderFunctions::applyInheritCSSPropertyCy(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setCy(state.parentStyle()->svgStyle().cy());
}

void StyleBuilderFunctions::applyValueCSSPropertyCy(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle().setCy(StyleBuilderConverter::convertLength(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyShapeOutside(StyleResolverState& state)
{
    state.style()->setShapeOutside(ComputedStyle::initialShapeOutside());
}

void StyleBuilderFunctions::applyInheritCSSPropertyShapeOutside(StyleResolverState& state)
{
    state.style()->setShapeOutside(state.parentStyle()->shapeOutside());
}

void StyleBuilderFunctions::applyValueCSSPropertyShapeOutside(StyleResolverState& state, CSSValue* value)
{
    state.style()->setShapeOutside(StyleBuilderConverter::convertShapeValue(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitHyphenateCharacter(StyleResolverState& state)
{
    state.style()->setHyphenationString(ComputedStyle::initialHyphenationString());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitHyphenateCharacter(StyleResolverState& state)
{
    state.style()->setHyphenationString(state.parentStyle()->hyphenationString());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitHyphenateCharacter(StyleResolverState& state, CSSValue* value)
{
    state.style()->setHyphenationString(StyleBuilderConverter::convertString<CSSValueAuto>(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitBoxPack(StyleResolverState& state)
{
    state.style()->setBoxPack(ComputedStyle::initialBoxPack());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitBoxPack(StyleResolverState& state)
{
    state.style()->setBoxPack(state.parentStyle()->boxPack());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitBoxPack(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBoxPack(static_cast<EBoxPack>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitMarginBottomCollapse(StyleResolverState& state)
{
    state.style()->setMarginAfterCollapse(ComputedStyle::initialMarginAfterCollapse());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitMarginBottomCollapse(StyleResolverState& state)
{
    state.style()->setMarginAfterCollapse(state.parentStyle()->marginAfterCollapse());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitMarginBottomCollapse(StyleResolverState& state, CSSValue* value)
{
    state.style()->setMarginAfterCollapse(static_cast<EMarginCollapse>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyOutlineWidth(StyleResolverState& state)
{
    state.style()->setOutlineWidth(ComputedStyle::initialOutlineWidth());
}

void StyleBuilderFunctions::applyInheritCSSPropertyOutlineWidth(StyleResolverState& state)
{
    state.style()->setOutlineWidth(state.parentStyle()->outlineWidth());
}

void StyleBuilderFunctions::applyValueCSSPropertyOutlineWidth(StyleResolverState& state, CSSValue* value)
{
    state.style()->setOutlineWidth(StyleBuilderConverter::convertLineWidth<unsigned short>(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyStrokeOpacity(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setStrokeOpacity(SVGComputedStyle::initialStrokeOpacity());
}

void StyleBuilderFunctions::applyInheritCSSPropertyStrokeOpacity(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setStrokeOpacity(state.parentStyle()->svgStyle().strokeOpacity());
}

void StyleBuilderFunctions::applyValueCSSPropertyStrokeOpacity(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle().setStrokeOpacity(StyleBuilderConverter::convertNumberOrPercentage(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyFontFamily(StyleResolverState& state)
{
    state.fontBuilder().setFamilyDescription(FontBuilder::initialFamilyDescription());
}

void StyleBuilderFunctions::applyInheritCSSPropertyFontFamily(StyleResolverState& state)
{
    state.fontBuilder().setFamilyDescription(state.parentFontDescription().familyDescription());
}

void StyleBuilderFunctions::applyValueCSSPropertyFontFamily(StyleResolverState& state, CSSValue* value)
{
    state.fontBuilder().setFamilyDescription(StyleBuilderConverter::convertFontFamily(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitBoxFlex(StyleResolverState& state)
{
    state.style()->setBoxFlex(ComputedStyle::initialBoxFlex());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitBoxFlex(StyleResolverState& state)
{
    state.style()->setBoxFlex(state.parentStyle()->boxFlex());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitBoxFlex(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBoxFlex(static_cast<float>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitAppearance(StyleResolverState& state)
{
    state.style()->setAppearance(ComputedStyle::initialAppearance());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitAppearance(StyleResolverState& state)
{
    state.style()->setAppearance(state.parentStyle()->appearance());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitAppearance(StyleResolverState& state, CSSValue* value)
{
    state.style()->setAppearance(static_cast<ControlPart>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyBorderBottomStyle(StyleResolverState& state)
{
    state.style()->setBorderBottomStyle(ComputedStyle::initialBorderStyle());
}

void StyleBuilderFunctions::applyInheritCSSPropertyBorderBottomStyle(StyleResolverState& state)
{
    state.style()->setBorderBottomStyle(state.parentStyle()->borderBottomStyle());
}

void StyleBuilderFunctions::applyValueCSSPropertyBorderBottomStyle(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBorderBottomStyle(static_cast<EBorderStyle>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyHeight(StyleResolverState& state)
{
    state.style()->setHeight(ComputedStyle::initialSize());
}

void StyleBuilderFunctions::applyInheritCSSPropertyHeight(StyleResolverState& state)
{
    state.style()->setHeight(state.parentStyle()->height());
}

void StyleBuilderFunctions::applyValueCSSPropertyHeight(StyleResolverState& state, CSSValue* value)
{
    state.style()->setHeight(StyleBuilderConverter::convertLengthSizing(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyPaintOrder(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setPaintOrder(SVGComputedStyle::initialPaintOrder());
}

void StyleBuilderFunctions::applyInheritCSSPropertyPaintOrder(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setPaintOrder(state.parentStyle()->svgStyle().paintOrder());
}

void StyleBuilderFunctions::applyValueCSSPropertyPaintOrder(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle().setPaintOrder(StyleBuilderConverter::convertPaintOrder(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyTextRendering(StyleResolverState& state)
{
    state.fontBuilder().setTextRendering(FontBuilder::initialTextRendering());
}

void StyleBuilderFunctions::applyInheritCSSPropertyTextRendering(StyleResolverState& state)
{
    state.fontBuilder().setTextRendering(state.parentFontDescription().textRendering());
}

void StyleBuilderFunctions::applyValueCSSPropertyTextRendering(StyleResolverState& state, CSSValue* value)
{
    state.fontBuilder().setTextRendering(static_cast<TextRenderingMode>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitColumnBreakAfter(StyleResolverState& state)
{
    state.style()->setColumnBreakAfter(ComputedStyle::initialPageBreak());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitColumnBreakAfter(StyleResolverState& state)
{
    state.style()->setColumnBreakAfter(state.parentStyle()->columnBreakAfter());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitColumnBreakAfter(StyleResolverState& state, CSSValue* value)
{
    state.style()->setColumnBreakAfter(static_cast<EPageBreak>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyBorderBottomLeftRadius(StyleResolverState& state)
{
    state.style()->setBorderBottomLeftRadius(ComputedStyle::initialBorderRadius());
}

void StyleBuilderFunctions::applyInheritCSSPropertyBorderBottomLeftRadius(StyleResolverState& state)
{
    state.style()->setBorderBottomLeftRadius(state.parentStyle()->borderBottomLeftRadius());
}

void StyleBuilderFunctions::applyValueCSSPropertyBorderBottomLeftRadius(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBorderBottomLeftRadius(StyleBuilderConverter::convertRadius(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyColorInterpolation(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setColorInterpolation(SVGComputedStyle::initialColorInterpolation());
}

void StyleBuilderFunctions::applyInheritCSSPropertyColorInterpolation(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setColorInterpolation(state.parentStyle()->svgStyle().colorInterpolation());
}

void StyleBuilderFunctions::applyValueCSSPropertyColorInterpolation(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle().setColorInterpolation(static_cast<EColorInterpolation>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyTextDecorationLine(StyleResolverState& state)
{
    state.style()->setTextDecoration(ComputedStyle::initialTextDecoration());
}

void StyleBuilderFunctions::applyInheritCSSPropertyTextDecorationLine(StyleResolverState& state)
{
    state.style()->setTextDecoration(state.parentStyle()->textDecoration());
}

void StyleBuilderFunctions::applyValueCSSPropertyTextDecorationLine(StyleResolverState& state, CSSValue* value)
{
    state.style()->setTextDecoration(StyleBuilderConverter::convertFlags<TextDecoration>(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitColumnSpan(StyleResolverState& state)
{
    state.style()->setColumnSpan(ComputedStyle::initialColumnSpan());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitColumnSpan(StyleResolverState& state)
{
    state.style()->setColumnSpan(state.parentStyle()->columnSpan());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitColumnSpan(StyleResolverState& state, CSSValue* value)
{
    state.style()->setColumnSpan(static_cast<ColumnSpan>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyShapeMargin(StyleResolverState& state)
{
    state.style()->setShapeMargin(ComputedStyle::initialShapeMargin());
}

void StyleBuilderFunctions::applyInheritCSSPropertyShapeMargin(StyleResolverState& state)
{
    state.style()->setShapeMargin(state.parentStyle()->shapeMargin());
}

void StyleBuilderFunctions::applyValueCSSPropertyShapeMargin(StyleResolverState& state, CSSValue* value)
{
    state.style()->setShapeMargin(StyleBuilderConverter::convertLength(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyScrollSnapType(StyleResolverState& state)
{
    state.style()->setScrollSnapType(ComputedStyle::initialScrollSnapType());
}

void StyleBuilderFunctions::applyInheritCSSPropertyScrollSnapType(StyleResolverState& state)
{
    state.style()->setScrollSnapType(state.parentStyle()->scrollSnapType());
}

void StyleBuilderFunctions::applyValueCSSPropertyScrollSnapType(StyleResolverState& state, CSSValue* value)
{
    state.style()->setScrollSnapType(static_cast<ScrollSnapType>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyImageRendering(StyleResolverState& state)
{
    state.style()->setImageRendering(ComputedStyle::initialImageRendering());
}

void StyleBuilderFunctions::applyInheritCSSPropertyImageRendering(StyleResolverState& state)
{
    state.style()->setImageRendering(state.parentStyle()->imageRendering());
}

void StyleBuilderFunctions::applyValueCSSPropertyImageRendering(StyleResolverState& state, CSSValue* value)
{
    state.style()->setImageRendering(static_cast<EImageRendering>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitUserSelect(StyleResolverState& state)
{
    state.style()->setUserSelect(ComputedStyle::initialUserSelect());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitUserSelect(StyleResolverState& state)
{
    state.style()->setUserSelect(state.parentStyle()->userSelect());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitUserSelect(StyleResolverState& state, CSSValue* value)
{
    state.style()->setUserSelect(static_cast<EUserSelect>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyPaddingBottom(StyleResolverState& state)
{
    state.style()->setPaddingBottom(ComputedStyle::initialPadding());
}

void StyleBuilderFunctions::applyInheritCSSPropertyPaddingBottom(StyleResolverState& state)
{
    state.style()->setPaddingBottom(state.parentStyle()->paddingBottom());
}

void StyleBuilderFunctions::applyValueCSSPropertyPaddingBottom(StyleResolverState& state, CSSValue* value)
{
    state.style()->setPaddingBottom(StyleBuilderConverter::convertLength(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyQuotes(StyleResolverState& state)
{
    state.style()->setQuotes(ComputedStyle::initialQuotes());
}

void StyleBuilderFunctions::applyInheritCSSPropertyQuotes(StyleResolverState& state)
{
    state.style()->setQuotes(state.parentStyle()->quotes());
}

void StyleBuilderFunctions::applyValueCSSPropertyQuotes(StyleResolverState& state, CSSValue* value)
{
    state.style()->setQuotes(StyleBuilderConverter::convertQuotes(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyGridAutoRows(StyleResolverState& state)
{
    state.style()->setGridAutoRows(ComputedStyle::initialGridAutoRows());
}

void StyleBuilderFunctions::applyInheritCSSPropertyGridAutoRows(StyleResolverState& state)
{
    state.style()->setGridAutoRows(state.parentStyle()->gridAutoRows());
}

void StyleBuilderFunctions::applyValueCSSPropertyGridAutoRows(StyleResolverState& state, CSSValue* value)
{
    state.style()->setGridAutoRows(StyleBuilderConverter::convertGridTrackSize(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyClipPath(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setClipperResource(SVGComputedStyle::initialClipperResource());
}

void StyleBuilderFunctions::applyInheritCSSPropertyClipPath(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setClipperResource(state.parentStyle()->svgStyle().clipperResource());
}

void StyleBuilderFunctions::applyValueCSSPropertyClipPath(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle().setClipperResource(StyleBuilderConverter::convertFragmentIdentifier(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyObjectFit(StyleResolverState& state)
{
    state.style()->setObjectFit(ComputedStyle::initialObjectFit());
}

void StyleBuilderFunctions::applyInheritCSSPropertyObjectFit(StyleResolverState& state)
{
    state.style()->setObjectFit(state.parentStyle()->objectFit());
}

void StyleBuilderFunctions::applyValueCSSPropertyObjectFit(StyleResolverState& state, CSSValue* value)
{
    state.style()->setObjectFit(static_cast<ObjectFit>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyStopOpacity(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setStopOpacity(SVGComputedStyle::initialStopOpacity());
}

void StyleBuilderFunctions::applyInheritCSSPropertyStopOpacity(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setStopOpacity(state.parentStyle()->svgStyle().stopOpacity());
}

void StyleBuilderFunctions::applyValueCSSPropertyStopOpacity(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle().setStopOpacity(StyleBuilderConverter::convertNumberOrPercentage(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitBoxReflect(StyleResolverState& state)
{
    state.style()->setBoxReflect(ComputedStyle::initialBoxReflect());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitBoxReflect(StyleResolverState& state)
{
    state.style()->setBoxReflect(state.parentStyle()->boxReflect());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitBoxReflect(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBoxReflect(StyleBuilderConverter::convertBoxReflect(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyBorderTopRightRadius(StyleResolverState& state)
{
    state.style()->setBorderTopRightRadius(ComputedStyle::initialBorderRadius());
}

void StyleBuilderFunctions::applyInheritCSSPropertyBorderTopRightRadius(StyleResolverState& state)
{
    state.style()->setBorderTopRightRadius(state.parentStyle()->borderTopRightRadius());
}

void StyleBuilderFunctions::applyValueCSSPropertyBorderTopRightRadius(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBorderTopRightRadius(StyleBuilderConverter::convertRadius(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitBoxDirection(StyleResolverState& state)
{
    state.style()->setBoxDirection(ComputedStyle::initialBoxDirection());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitBoxDirection(StyleResolverState& state)
{
    state.style()->setBoxDirection(state.parentStyle()->boxDirection());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitBoxDirection(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBoxDirection(static_cast<EBoxDirection>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitUserModify(StyleResolverState& state)
{
    state.style()->setUserModify(ComputedStyle::initialUserModify());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitUserModify(StyleResolverState& state)
{
    state.style()->setUserModify(state.parentStyle()->userModify());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitUserModify(StyleResolverState& state, CSSValue* value)
{
    state.style()->setUserModify(static_cast<EUserModify>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitBoxDecorationBreak(StyleResolverState& state)
{
    state.style()->setBoxDecorationBreak(ComputedStyle::initialBoxDecorationBreak());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitBoxDecorationBreak(StyleResolverState& state)
{
    state.style()->setBoxDecorationBreak(state.parentStyle()->boxDecorationBreak());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitBoxDecorationBreak(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBoxDecorationBreak(static_cast<EBoxDecorationBreak>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyFontKerning(StyleResolverState& state)
{
    state.fontBuilder().setKerning(FontBuilder::initialKerning());
}

void StyleBuilderFunctions::applyInheritCSSPropertyFontKerning(StyleResolverState& state)
{
    state.fontBuilder().setKerning(state.parentFontDescription().kerning());
}

void StyleBuilderFunctions::applyValueCSSPropertyFontKerning(StyleResolverState& state, CSSValue* value)
{
    state.fontBuilder().setKerning(static_cast<FontDescription::Kerning>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyFontWeight(StyleResolverState& state)
{
    state.fontBuilder().setWeight(FontBuilder::initialWeight());
}

void StyleBuilderFunctions::applyInheritCSSPropertyFontWeight(StyleResolverState& state)
{
    state.fontBuilder().setWeight(state.parentFontDescription().weight());
}

void StyleBuilderFunctions::applyValueCSSPropertyFontWeight(StyleResolverState& state, CSSValue* value)
{
    state.fontBuilder().setWeight(StyleBuilderConverter::convertFontWeight(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyShapeImageThreshold(StyleResolverState& state)
{
    state.style()->setShapeImageThreshold(ComputedStyle::initialShapeImageThreshold());
}

void StyleBuilderFunctions::applyInheritCSSPropertyShapeImageThreshold(StyleResolverState& state)
{
    state.style()->setShapeImageThreshold(state.parentStyle()->shapeImageThreshold());
}

void StyleBuilderFunctions::applyValueCSSPropertyShapeImageThreshold(StyleResolverState& state, CSSValue* value)
{
    state.style()->setShapeImageThreshold(static_cast<float>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyRy(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setRy(SVGComputedStyle::initialRy());
}

void StyleBuilderFunctions::applyInheritCSSPropertyRy(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setRy(state.parentStyle()->svgStyle().ry());
}

void StyleBuilderFunctions::applyValueCSSPropertyRy(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle().setRy(StyleBuilderConverter::convertLength(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyRx(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setRx(SVGComputedStyle::initialRx());
}

void StyleBuilderFunctions::applyInheritCSSPropertyRx(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setRx(state.parentStyle()->svgStyle().rx());
}

void StyleBuilderFunctions::applyValueCSSPropertyRx(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle().setRx(StyleBuilderConverter::convertLength(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyMarginRight(StyleResolverState& state)
{
    state.style()->setMarginRight(ComputedStyle::initialMargin());
}

void StyleBuilderFunctions::applyInheritCSSPropertyMarginRight(StyleResolverState& state)
{
    state.style()->setMarginRight(state.parentStyle()->marginRight());
}

void StyleBuilderFunctions::applyValueCSSPropertyMarginRight(StyleResolverState& state, CSSValue* value)
{
    state.style()->setMarginRight(StyleBuilderConverter::convertQuirkyLength(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitFontSmoothing(StyleResolverState& state)
{
    state.fontBuilder().setFontSmoothing(FontBuilder::initialFontSmoothing());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitFontSmoothing(StyleResolverState& state)
{
    state.fontBuilder().setFontSmoothing(state.parentFontDescription().fontSmoothing());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitFontSmoothing(StyleResolverState& state, CSSValue* value)
{
    state.fontBuilder().setFontSmoothing(static_cast<FontSmoothingMode>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyPageBreakBefore(StyleResolverState& state)
{
    state.style()->setPageBreakBefore(ComputedStyle::initialPageBreak());
}

void StyleBuilderFunctions::applyInheritCSSPropertyPageBreakBefore(StyleResolverState& state)
{
    state.style()->setPageBreakBefore(state.parentStyle()->pageBreakBefore());
}

void StyleBuilderFunctions::applyValueCSSPropertyPageBreakBefore(StyleResolverState& state, CSSValue* value)
{
    state.style()->setPageBreakBefore(static_cast<EPageBreak>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitFontFeatureSettings(StyleResolverState& state)
{
    state.fontBuilder().setFeatureSettings(FontBuilder::initialFeatureSettings());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitFontFeatureSettings(StyleResolverState& state)
{
    state.fontBuilder().setFeatureSettings(state.parentFontDescription().featureSettings());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitFontFeatureSettings(StyleResolverState& state, CSSValue* value)
{
    state.fontBuilder().setFeatureSettings(StyleBuilderConverter::convertFontFeatureSettings(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyOpacity(StyleResolverState& state)
{
    state.style()->setOpacity(ComputedStyle::initialOpacity());
}

void StyleBuilderFunctions::applyInheritCSSPropertyOpacity(StyleResolverState& state)
{
    state.style()->setOpacity(state.parentStyle()->opacity());
}

void StyleBuilderFunctions::applyValueCSSPropertyOpacity(StyleResolverState& state, CSSValue* value)
{
    state.style()->setOpacity(static_cast<float>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyDominantBaseline(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setDominantBaseline(SVGComputedStyle::initialDominantBaseline());
}

void StyleBuilderFunctions::applyInheritCSSPropertyDominantBaseline(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setDominantBaseline(state.parentStyle()->svgStyle().dominantBaseline());
}

void StyleBuilderFunctions::applyValueCSSPropertyDominantBaseline(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle().setDominantBaseline(static_cast<EDominantBaseline>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyAlignSelf(StyleResolverState& state)
{
    state.style()->setAlignSelf(ComputedStyle::initialSelfAlignment());
}

void StyleBuilderFunctions::applyInheritCSSPropertyAlignSelf(StyleResolverState& state)
{
    state.style()->setAlignSelf(state.parentStyle()->alignSelf());
}

void StyleBuilderFunctions::applyValueCSSPropertyAlignSelf(StyleResolverState& state, CSSValue* value)
{
    state.style()->setAlignSelf(StyleBuilderConverter::convertSelfOrDefaultAlignmentData(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyClear(StyleResolverState& state)
{
    state.style()->setClear(ComputedStyle::initialClear());
}

void StyleBuilderFunctions::applyInheritCSSPropertyClear(StyleResolverState& state)
{
    state.style()->setClear(state.parentStyle()->clear());
}

void StyleBuilderFunctions::applyValueCSSPropertyClear(StyleResolverState& state, CSSValue* value)
{
    state.style()->setClear(static_cast<EClear>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitColumnBreakInside(StyleResolverState& state)
{
    state.style()->setColumnBreakInside(ComputedStyle::initialPageBreak());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitColumnBreakInside(StyleResolverState& state)
{
    state.style()->setColumnBreakInside(state.parentStyle()->columnBreakInside());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitColumnBreakInside(StyleResolverState& state, CSSValue* value)
{
    state.style()->setColumnBreakInside(static_cast<EPageBreak>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitMaskBoxImageSource(StyleResolverState& state)
{
    state.style()->setMaskBoxImageSource(ComputedStyle::initialMaskBoxImageSource());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitMaskBoxImageSource(StyleResolverState& state)
{
    state.style()->setMaskBoxImageSource(state.parentStyle()->maskBoxImageSource());
}

void StyleBuilderFunctions::applyInitialCSSPropertyTransformStyle(StyleResolverState& state)
{
    state.style()->setTransformStyle3D(ComputedStyle::initialTransformStyle3D());
}

void StyleBuilderFunctions::applyInheritCSSPropertyTransformStyle(StyleResolverState& state)
{
    state.style()->setTransformStyle3D(state.parentStyle()->transformStyle3D());
}

void StyleBuilderFunctions::applyValueCSSPropertyTransformStyle(StyleResolverState& state, CSSValue* value)
{
    state.style()->setTransformStyle3D(static_cast<ETransformStyle3D>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitBorderHorizontalSpacing(StyleResolverState& state)
{
    state.style()->setHorizontalBorderSpacing(ComputedStyle::initialHorizontalBorderSpacing());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitBorderHorizontalSpacing(StyleResolverState& state)
{
    state.style()->setHorizontalBorderSpacing(state.parentStyle()->horizontalBorderSpacing());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitBorderHorizontalSpacing(StyleResolverState& state, CSSValue* value)
{
    state.style()->setHorizontalBorderSpacing(StyleBuilderConverter::convertComputedLength<short>(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitTapHighlightColor(StyleResolverState& state)
{
    state.style()->setTapHighlightColor(ComputedStyle::initialTapHighlightColor());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitTapHighlightColor(StyleResolverState& state)
{
    state.style()->setTapHighlightColor(state.parentStyle()->tapHighlightColor());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitTapHighlightColor(StyleResolverState& state, CSSValue* value)
{
    state.style()->setTapHighlightColor(StyleBuilderConverter::convertColor(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyCaptionSide(StyleResolverState& state)
{
    state.style()->setCaptionSide(ComputedStyle::initialCaptionSide());
}

void StyleBuilderFunctions::applyInheritCSSPropertyCaptionSide(StyleResolverState& state)
{
    state.style()->setCaptionSide(state.parentStyle()->captionSide());
}

void StyleBuilderFunctions::applyValueCSSPropertyCaptionSide(StyleResolverState& state, CSSValue* value)
{
    state.style()->setCaptionSide(static_cast<ECaptionSide>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitPrintColorAdjust(StyleResolverState& state)
{
    state.style()->setPrintColorAdjust(ComputedStyle::initialPrintColorAdjust());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitPrintColorAdjust(StyleResolverState& state)
{
    state.style()->setPrintColorAdjust(state.parentStyle()->printColorAdjust());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitPrintColorAdjust(StyleResolverState& state, CSSValue* value)
{
    state.style()->setPrintColorAdjust(static_cast<PrintColorAdjust>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyScrollBlocksOn(StyleResolverState& state)
{
    state.style()->setScrollBlocksOn(ComputedStyle::initialScrollBlocksOn());
}

void StyleBuilderFunctions::applyInheritCSSPropertyScrollBlocksOn(StyleResolverState& state)
{
    state.style()->setScrollBlocksOn(state.parentStyle()->scrollBlocksOn());
}

void StyleBuilderFunctions::applyValueCSSPropertyScrollBlocksOn(StyleResolverState& state, CSSValue* value)
{
    state.style()->setScrollBlocksOn(StyleBuilderConverter::convertFlags<WebScrollBlocksOn>(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyStrokeDasharray(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setStrokeDashArray(SVGComputedStyle::initialStrokeDashArray());
}

void StyleBuilderFunctions::applyInheritCSSPropertyStrokeDasharray(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setStrokeDashArray(state.parentStyle()->svgStyle().strokeDashArray());
}

void StyleBuilderFunctions::applyValueCSSPropertyStrokeDasharray(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle().setStrokeDashArray(StyleBuilderConverter::convertStrokeDasharray(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyFlexBasis(StyleResolverState& state)
{
    state.style()->setFlexBasis(ComputedStyle::initialFlexBasis());
}

void StyleBuilderFunctions::applyInheritCSSPropertyFlexBasis(StyleResolverState& state)
{
    state.style()->setFlexBasis(state.parentStyle()->flexBasis());
}

void StyleBuilderFunctions::applyValueCSSPropertyFlexBasis(StyleResolverState& state, CSSValue* value)
{
    state.style()->setFlexBasis(StyleBuilderConverter::convertLengthOrAuto(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyLetterSpacing(StyleResolverState& state)
{
    state.style()->setLetterSpacing(ComputedStyle::initialLetterWordSpacing());
}

void StyleBuilderFunctions::applyInheritCSSPropertyLetterSpacing(StyleResolverState& state)
{
    state.style()->setLetterSpacing(state.parentStyle()->letterSpacing());
}

void StyleBuilderFunctions::applyValueCSSPropertyLetterSpacing(StyleResolverState& state, CSSValue* value)
{
    state.style()->setLetterSpacing(StyleBuilderConverter::convertSpacing(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitRubyPosition(StyleResolverState& state)
{
    state.style()->setRubyPosition(ComputedStyle::initialRubyPosition());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitRubyPosition(StyleResolverState& state)
{
    state.style()->setRubyPosition(state.parentStyle()->rubyPosition());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitRubyPosition(StyleResolverState& state, CSSValue* value)
{
    state.style()->setRubyPosition(static_cast<RubyPosition>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitTransformOriginZ(StyleResolverState& state)
{
    state.style()->setTransformOriginZ(ComputedStyle::initialTransformOriginZ());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitTransformOriginZ(StyleResolverState& state)
{
    state.style()->setTransformOriginZ(state.parentStyle()->transformOriginZ());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitTransformOriginZ(StyleResolverState& state, CSSValue* value)
{
    state.style()->setTransformOriginZ(StyleBuilderConverter::convertComputedLength<float>(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitTransformOriginY(StyleResolverState& state)
{
    state.style()->setTransformOriginY(ComputedStyle::initialTransformOriginY());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitTransformOriginY(StyleResolverState& state)
{
    state.style()->setTransformOriginY(state.parentStyle()->transformOriginY());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitTransformOriginY(StyleResolverState& state, CSSValue* value)
{
    state.style()->setTransformOriginY(StyleBuilderConverter::convertLength(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitTransformOriginX(StyleResolverState& state)
{
    state.style()->setTransformOriginX(ComputedStyle::initialTransformOriginX());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitTransformOriginX(StyleResolverState& state)
{
    state.style()->setTransformOriginX(state.parentStyle()->transformOriginX());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitTransformOriginX(StyleResolverState& state, CSSValue* value)
{
    state.style()->setTransformOriginX(StyleBuilderConverter::convertLength(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyTransform(StyleResolverState& state)
{
    state.style()->setTransform(ComputedStyle::initialTransform());
}

void StyleBuilderFunctions::applyInheritCSSPropertyTransform(StyleResolverState& state)
{
    state.style()->setTransform(state.parentStyle()->transform());
}

void StyleBuilderFunctions::applyInitialCSSPropertyScrollBehavior(StyleResolverState& state)
{
    state.style()->setScrollBehavior(ComputedStyle::initialScrollBehavior());
}

void StyleBuilderFunctions::applyInheritCSSPropertyScrollBehavior(StyleResolverState& state)
{
    state.style()->setScrollBehavior(state.parentStyle()->scrollBehavior());
}

void StyleBuilderFunctions::applyValueCSSPropertyScrollBehavior(StyleResolverState& state, CSSValue* value)
{
    state.style()->setScrollBehavior(static_cast<ScrollBehavior>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyGridAutoFlow(StyleResolverState& state)
{
    state.style()->setGridAutoFlow(ComputedStyle::initialGridAutoFlow());
}

void StyleBuilderFunctions::applyInheritCSSPropertyGridAutoFlow(StyleResolverState& state)
{
    state.style()->setGridAutoFlow(state.parentStyle()->gridAutoFlow());
}

void StyleBuilderFunctions::applyValueCSSPropertyGridAutoFlow(StyleResolverState& state, CSSValue* value)
{
    state.style()->setGridAutoFlow(StyleBuilderConverter::convertGridAutoFlow(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyStopColor(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setStopColor(SVGComputedStyle::initialStopColor());
}

void StyleBuilderFunctions::applyInheritCSSPropertyStopColor(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setStopColor(state.parentStyle()->svgStyle().stopColor());
}

void StyleBuilderFunctions::applyValueCSSPropertyStopColor(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle().setStopColor(StyleBuilderConverter::convertColor(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitLineClamp(StyleResolverState& state)
{
    state.style()->setLineClamp(ComputedStyle::initialLineClamp());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitLineClamp(StyleResolverState& state)
{
    state.style()->setLineClamp(state.parentStyle()->lineClamp());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitLineClamp(StyleResolverState& state, CSSValue* value)
{
    state.style()->setLineClamp(static_cast<LineClampValue>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyJustifySelf(StyleResolverState& state)
{
    state.style()->setJustifySelf(ComputedStyle::initialSelfAlignment());
}

void StyleBuilderFunctions::applyInheritCSSPropertyJustifySelf(StyleResolverState& state)
{
    state.style()->setJustifySelf(state.parentStyle()->justifySelf());
}

void StyleBuilderFunctions::applyValueCSSPropertyJustifySelf(StyleResolverState& state, CSSValue* value)
{
    state.style()->setJustifySelf(StyleBuilderConverter::convertSelfOrDefaultAlignmentData(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyTextJustify(StyleResolverState& state)
{
    state.style()->setTextJustify(ComputedStyle::initialTextJustify());
}

void StyleBuilderFunctions::applyInheritCSSPropertyTextJustify(StyleResolverState& state)
{
    state.style()->setTextJustify(state.parentStyle()->textJustify());
}

void StyleBuilderFunctions::applyValueCSSPropertyTextJustify(StyleResolverState& state, CSSValue* value)
{
    state.style()->setTextJustify(static_cast<TextJustify>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyPerspectiveOrigin(StyleResolverState& state)
{
    state.style()->setPerspectiveOrigin(ComputedStyle::initialPerspectiveOrigin());
}

void StyleBuilderFunctions::applyInheritCSSPropertyPerspectiveOrigin(StyleResolverState& state)
{
    state.style()->setPerspectiveOrigin(state.parentStyle()->perspectiveOrigin());
}

void StyleBuilderFunctions::applyValueCSSPropertyPerspectiveOrigin(StyleResolverState& state, CSSValue* value)
{
    state.style()->setPerspectiveOrigin(StyleBuilderConverter::convertPerspectiveOrigin(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyTextAnchor(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setTextAnchor(SVGComputedStyle::initialTextAnchor());
}

void StyleBuilderFunctions::applyInheritCSSPropertyTextAnchor(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setTextAnchor(state.parentStyle()->svgStyle().textAnchor());
}

void StyleBuilderFunctions::applyValueCSSPropertyTextAnchor(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle().setTextAnchor(static_cast<ETextAnchor>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyFontStyle(StyleResolverState& state)
{
    state.fontBuilder().setStyle(FontBuilder::initialStyle());
}

void StyleBuilderFunctions::applyInheritCSSPropertyFontStyle(StyleResolverState& state)
{
    state.fontBuilder().setStyle(state.parentFontDescription().style());
}

void StyleBuilderFunctions::applyValueCSSPropertyFontStyle(StyleResolverState& state, CSSValue* value)
{
    state.fontBuilder().setStyle(static_cast<FontStyle>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyBorderBottomRightRadius(StyleResolverState& state)
{
    state.style()->setBorderBottomRightRadius(ComputedStyle::initialBorderRadius());
}

void StyleBuilderFunctions::applyInheritCSSPropertyBorderBottomRightRadius(StyleResolverState& state)
{
    state.style()->setBorderBottomRightRadius(state.parentStyle()->borderBottomRightRadius());
}

void StyleBuilderFunctions::applyValueCSSPropertyBorderBottomRightRadius(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBorderBottomRightRadius(StyleBuilderConverter::convertRadius(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyBorderRightWidth(StyleResolverState& state)
{
    state.style()->setBorderRightWidth(ComputedStyle::initialBorderWidth());
}

void StyleBuilderFunctions::applyInheritCSSPropertyBorderRightWidth(StyleResolverState& state)
{
    state.style()->setBorderRightWidth(state.parentStyle()->borderRightWidth());
}

void StyleBuilderFunctions::applyValueCSSPropertyBorderRightWidth(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBorderRightWidth(StyleBuilderConverter::convertLineWidth<unsigned>(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyBorderTopLeftRadius(StyleResolverState& state)
{
    state.style()->setBorderTopLeftRadius(ComputedStyle::initialBorderRadius());
}

void StyleBuilderFunctions::applyInheritCSSPropertyBorderTopLeftRadius(StyleResolverState& state)
{
    state.style()->setBorderTopLeftRadius(state.parentStyle()->borderTopLeftRadius());
}

void StyleBuilderFunctions::applyValueCSSPropertyBorderTopLeftRadius(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBorderTopLeftRadius(StyleBuilderConverter::convertRadius(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyFontVariant(StyleResolverState& state)
{
    state.fontBuilder().setVariant(FontBuilder::initialVariant());
}

void StyleBuilderFunctions::applyInheritCSSPropertyFontVariant(StyleResolverState& state)
{
    state.fontBuilder().setVariant(state.parentFontDescription().variant());
}

void StyleBuilderFunctions::applyValueCSSPropertyFontVariant(StyleResolverState& state, CSSValue* value)
{
    state.fontBuilder().setVariant(static_cast<FontVariant>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWritingMode(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setWritingMode(SVGComputedStyle::initialWritingMode());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWritingMode(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setWritingMode(state.parentStyle()->svgStyle().writingMode());
}

void StyleBuilderFunctions::applyValueCSSPropertyWritingMode(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle().setWritingMode(static_cast<SVGWritingMode>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitTextSecurity(StyleResolverState& state)
{
    state.style()->setTextSecurity(ComputedStyle::initialTextSecurity());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitTextSecurity(StyleResolverState& state)
{
    state.style()->setTextSecurity(state.parentStyle()->textSecurity());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitTextSecurity(StyleResolverState& state, CSSValue* value)
{
    state.style()->setTextSecurity(static_cast<ETextSecurity>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyBorderLeftWidth(StyleResolverState& state)
{
    state.style()->setBorderLeftWidth(ComputedStyle::initialBorderWidth());
}

void StyleBuilderFunctions::applyInheritCSSPropertyBorderLeftWidth(StyleResolverState& state)
{
    state.style()->setBorderLeftWidth(state.parentStyle()->borderLeftWidth());
}

void StyleBuilderFunctions::applyValueCSSPropertyBorderLeftWidth(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBorderLeftWidth(StyleBuilderConverter::convertLineWidth<unsigned>(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitLineBreak(StyleResolverState& state)
{
    state.style()->setLineBreak(ComputedStyle::initialLineBreak());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitLineBreak(StyleResolverState& state)
{
    state.style()->setLineBreak(state.parentStyle()->lineBreak());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitLineBreak(StyleResolverState& state, CSSValue* value)
{
    state.style()->setLineBreak(static_cast<LineBreak>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyIsolation(StyleResolverState& state)
{
    state.style()->setIsolation(ComputedStyle::initialIsolation());
}

void StyleBuilderFunctions::applyInheritCSSPropertyIsolation(StyleResolverState& state)
{
    state.style()->setIsolation(state.parentStyle()->isolation());
}

void StyleBuilderFunctions::applyValueCSSPropertyIsolation(StyleResolverState& state, CSSValue* value)
{
    state.style()->setIsolation(static_cast<EIsolation>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyGlyphOrientationHorizontal(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setGlyphOrientationHorizontal(SVGComputedStyle::initialGlyphOrientationHorizontal());
}

void StyleBuilderFunctions::applyInheritCSSPropertyGlyphOrientationHorizontal(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setGlyphOrientationHorizontal(state.parentStyle()->svgStyle().glyphOrientationHorizontal());
}

void StyleBuilderFunctions::applyValueCSSPropertyGlyphOrientationHorizontal(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle().setGlyphOrientationHorizontal(StyleBuilderConverter::convertGlyphOrientation(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyFillOpacity(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setFillOpacity(SVGComputedStyle::initialFillOpacity());
}

void StyleBuilderFunctions::applyInheritCSSPropertyFillOpacity(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setFillOpacity(state.parentStyle()->svgStyle().fillOpacity());
}

void StyleBuilderFunctions::applyValueCSSPropertyFillOpacity(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle().setFillOpacity(StyleBuilderConverter::convertNumberOrPercentage(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyBorderTopWidth(StyleResolverState& state)
{
    state.style()->setBorderTopWidth(ComputedStyle::initialBorderWidth());
}

void StyleBuilderFunctions::applyInheritCSSPropertyBorderTopWidth(StyleResolverState& state)
{
    state.style()->setBorderTopWidth(state.parentStyle()->borderTopWidth());
}

void StyleBuilderFunctions::applyValueCSSPropertyBorderTopWidth(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBorderTopWidth(StyleBuilderConverter::convertLineWidth<unsigned>(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyBottom(StyleResolverState& state)
{
    state.style()->setBottom(ComputedStyle::initialOffset());
}

void StyleBuilderFunctions::applyInheritCSSPropertyBottom(StyleResolverState& state)
{
    state.style()->setBottom(state.parentStyle()->bottom());
}

void StyleBuilderFunctions::applyValueCSSPropertyBottom(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBottom(StyleBuilderConverter::convertLengthOrAuto(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyBorderCollapse(StyleResolverState& state)
{
    state.style()->setBorderCollapse(ComputedStyle::initialBorderCollapse());
}

void StyleBuilderFunctions::applyInheritCSSPropertyBorderCollapse(StyleResolverState& state)
{
    state.style()->setBorderCollapse(state.parentStyle()->borderCollapse());
}

void StyleBuilderFunctions::applyValueCSSPropertyBorderCollapse(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBorderCollapse(static_cast<EBorderCollapse>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyTop(StyleResolverState& state)
{
    state.style()->setTop(ComputedStyle::initialOffset());
}

void StyleBuilderFunctions::applyInheritCSSPropertyTop(StyleResolverState& state)
{
    state.style()->setTop(state.parentStyle()->top());
}

void StyleBuilderFunctions::applyValueCSSPropertyTop(StyleResolverState& state, CSSValue* value)
{
    state.style()->setTop(StyleBuilderConverter::convertLengthOrAuto(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyScrollSnapPointsX(StyleResolverState& state)
{
    state.style()->setScrollSnapPointsX(ComputedStyle::initialScrollSnapPointsX());
}

void StyleBuilderFunctions::applyInheritCSSPropertyScrollSnapPointsX(StyleResolverState& state)
{
    state.style()->setScrollSnapPointsX(state.parentStyle()->scrollSnapPointsX());
}

void StyleBuilderFunctions::applyValueCSSPropertyScrollSnapPointsX(StyleResolverState& state, CSSValue* value)
{
    state.style()->setScrollSnapPointsX(StyleBuilderConverter::convertSnapPoints(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyScrollSnapPointsY(StyleResolverState& state)
{
    state.style()->setScrollSnapPointsY(ComputedStyle::initialScrollSnapPointsY());
}

void StyleBuilderFunctions::applyInheritCSSPropertyScrollSnapPointsY(StyleResolverState& state)
{
    state.style()->setScrollSnapPointsY(state.parentStyle()->scrollSnapPointsY());
}

void StyleBuilderFunctions::applyValueCSSPropertyScrollSnapPointsY(StyleResolverState& state, CSSValue* value)
{
    state.style()->setScrollSnapPointsY(StyleBuilderConverter::convertSnapPoints(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyUnicodeBidi(StyleResolverState& state)
{
    state.style()->setUnicodeBidi(ComputedStyle::initialUnicodeBidi());
}

void StyleBuilderFunctions::applyInheritCSSPropertyUnicodeBidi(StyleResolverState& state)
{
    state.style()->setUnicodeBidi(state.parentStyle()->unicodeBidi());
}

void StyleBuilderFunctions::applyValueCSSPropertyUnicodeBidi(StyleResolverState& state, CSSValue* value)
{
    state.style()->setUnicodeBidi(static_cast<EUnicodeBidi>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitRtlOrdering(StyleResolverState& state)
{
    state.style()->setRTLOrdering(ComputedStyle::initialRTLOrdering());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitRtlOrdering(StyleResolverState& state)
{
    state.style()->setRTLOrdering(state.parentStyle()->rtlOrdering());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitRtlOrdering(StyleResolverState& state, CSSValue* value)
{
    state.style()->setRTLOrdering(static_cast<Order>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyFloat(StyleResolverState& state)
{
    state.style()->setFloating(ComputedStyle::initialFloating());
}

void StyleBuilderFunctions::applyInheritCSSPropertyFloat(StyleResolverState& state)
{
    state.style()->setFloating(state.parentStyle()->floating());
}

void StyleBuilderFunctions::applyValueCSSPropertyFloat(StyleResolverState& state, CSSValue* value)
{
    state.style()->setFloating(static_cast<EFloat>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWordWrap(StyleResolverState& state)
{
    state.style()->setOverflowWrap(ComputedStyle::initialOverflowWrap());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWordWrap(StyleResolverState& state)
{
    state.style()->setOverflowWrap(state.parentStyle()->overflowWrap());
}

void StyleBuilderFunctions::applyValueCSSPropertyWordWrap(StyleResolverState& state, CSSValue* value)
{
    state.style()->setOverflowWrap(static_cast<EOverflowWrap>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyTransformOrigin(StyleResolverState& state)
{
    state.style()->setTransformOrigin(ComputedStyle::initialTransformOrigin());
}

void StyleBuilderFunctions::applyInheritCSSPropertyTransformOrigin(StyleResolverState& state)
{
    state.style()->setTransformOrigin(state.parentStyle()->transformOrigin());
}

void StyleBuilderFunctions::applyValueCSSPropertyTransformOrigin(StyleResolverState& state, CSSValue* value)
{
    state.style()->setTransformOrigin(StyleBuilderConverter::convertTransformOrigin(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyMarginTop(StyleResolverState& state)
{
    state.style()->setMarginTop(ComputedStyle::initialMargin());
}

void StyleBuilderFunctions::applyInheritCSSPropertyMarginTop(StyleResolverState& state)
{
    state.style()->setMarginTop(state.parentStyle()->marginTop());
}

void StyleBuilderFunctions::applyValueCSSPropertyMarginTop(StyleResolverState& state, CSSValue* value)
{
    state.style()->setMarginTop(StyleBuilderConverter::convertQuirkyLength(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyMaxWidth(StyleResolverState& state)
{
    state.style()->setMaxWidth(ComputedStyle::initialMaxSize());
}

void StyleBuilderFunctions::applyInheritCSSPropertyMaxWidth(StyleResolverState& state)
{
    state.style()->setMaxWidth(state.parentStyle()->maxWidth());
}

void StyleBuilderFunctions::applyValueCSSPropertyMaxWidth(StyleResolverState& state, CSSValue* value)
{
    state.style()->setMaxWidth(StyleBuilderConverter::convertLengthMaxSizing(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyTextShadow(StyleResolverState& state)
{
    state.style()->setTextShadow(ComputedStyle::initialTextShadow());
}

void StyleBuilderFunctions::applyInheritCSSPropertyTextShadow(StyleResolverState& state)
{
    state.style()->setTextShadow(state.parentStyle()->textShadow());
}

void StyleBuilderFunctions::applyValueCSSPropertyTextShadow(StyleResolverState& state, CSSValue* value)
{
    state.style()->setTextShadow(StyleBuilderConverter::convertShadow(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyPaddingRight(StyleResolverState& state)
{
    state.style()->setPaddingRight(ComputedStyle::initialPadding());
}

void StyleBuilderFunctions::applyInheritCSSPropertyPaddingRight(StyleResolverState& state)
{
    state.style()->setPaddingRight(state.parentStyle()->paddingRight());
}

void StyleBuilderFunctions::applyValueCSSPropertyPaddingRight(StyleResolverState& state, CSSValue* value)
{
    state.style()->setPaddingRight(StyleBuilderConverter::convertLength(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitBoxOrdinalGroup(StyleResolverState& state)
{
    state.style()->setBoxOrdinalGroup(ComputedStyle::initialBoxOrdinalGroup());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitBoxOrdinalGroup(StyleResolverState& state)
{
    state.style()->setBoxOrdinalGroup(state.parentStyle()->boxOrdinalGroup());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitBoxOrdinalGroup(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBoxOrdinalGroup(static_cast<unsigned int>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyColumnFill(StyleResolverState& state)
{
    state.style()->setColumnFill(ComputedStyle::initialColumnFill());
}

void StyleBuilderFunctions::applyInheritCSSPropertyColumnFill(StyleResolverState& state)
{
    state.style()->setColumnFill(state.parentStyle()->columnFill());
}

void StyleBuilderFunctions::applyValueCSSPropertyColumnFill(StyleResolverState& state, CSSValue* value)
{
    state.style()->setColumnFill(static_cast<ColumnFill>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyOverflowX(StyleResolverState& state)
{
    state.style()->setOverflowX(ComputedStyle::initialOverflowX());
}

void StyleBuilderFunctions::applyInheritCSSPropertyOverflowX(StyleResolverState& state)
{
    state.style()->setOverflowX(state.parentStyle()->overflowX());
}

void StyleBuilderFunctions::applyValueCSSPropertyOverflowX(StyleResolverState& state, CSSValue* value)
{
    state.style()->setOverflowX(static_cast<EOverflow>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyOverflowY(StyleResolverState& state)
{
    state.style()->setOverflowY(ComputedStyle::initialOverflowY());
}

void StyleBuilderFunctions::applyInheritCSSPropertyOverflowY(StyleResolverState& state)
{
    state.style()->setOverflowY(state.parentStyle()->overflowY());
}

void StyleBuilderFunctions::applyValueCSSPropertyOverflowY(StyleResolverState& state, CSSValue* value)
{
    state.style()->setOverflowY(static_cast<EOverflow>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyPerspective(StyleResolverState& state)
{
    state.style()->setPerspective(ComputedStyle::initialPerspective());
}

void StyleBuilderFunctions::applyInheritCSSPropertyPerspective(StyleResolverState& state)
{
    state.style()->setPerspective(state.parentStyle()->perspective());
}

void StyleBuilderFunctions::applyValueCSSPropertyPerspective(StyleResolverState& state, CSSValue* value)
{
    state.style()->setPerspective(StyleBuilderConverter::convertPerspective(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyLineHeight(StyleResolverState& state)
{
    state.style()->setLineHeight(ComputedStyle::initialLineHeight());
}

void StyleBuilderFunctions::applyInheritCSSPropertyLineHeight(StyleResolverState& state)
{
    state.style()->setLineHeight(state.parentStyle()->specifiedLineHeight());
}

void StyleBuilderFunctions::applyValueCSSPropertyLineHeight(StyleResolverState& state, CSSValue* value)
{
    state.style()->setLineHeight(StyleBuilderConverter::convertLineHeight(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyOrder(StyleResolverState& state)
{
    state.style()->setOrder(ComputedStyle::initialOrder());
}

void StyleBuilderFunctions::applyInheritCSSPropertyOrder(StyleResolverState& state)
{
    state.style()->setOrder(state.parentStyle()->order());
}

void StyleBuilderFunctions::applyValueCSSPropertyOrder(StyleResolverState& state, CSSValue* value)
{
    state.style()->setOrder(static_cast<int>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitBoxAlign(StyleResolverState& state)
{
    state.style()->setBoxAlign(ComputedStyle::initialBoxAlign());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitBoxAlign(StyleResolverState& state)
{
    state.style()->setBoxAlign(state.parentStyle()->boxAlign());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitBoxAlign(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBoxAlign(static_cast<EBoxAlignment>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyScale(StyleResolverState& state)
{
    state.style()->setScale(ComputedStyle::initialScale());
}

void StyleBuilderFunctions::applyInheritCSSPropertyScale(StyleResolverState& state)
{
    state.style()->setScale(state.parentStyle()->scale());
}

void StyleBuilderFunctions::applyValueCSSPropertyScale(StyleResolverState& state, CSSValue* value)
{
    state.style()->setScale(StyleBuilderConverter::convertScale(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyGridAutoColumns(StyleResolverState& state)
{
    state.style()->setGridAutoColumns(ComputedStyle::initialGridAutoColumns());
}

void StyleBuilderFunctions::applyInheritCSSPropertyGridAutoColumns(StyleResolverState& state)
{
    state.style()->setGridAutoColumns(state.parentStyle()->gridAutoColumns());
}

void StyleBuilderFunctions::applyValueCSSPropertyGridAutoColumns(StyleResolverState& state, CSSValue* value)
{
    state.style()->setGridAutoColumns(StyleBuilderConverter::convertGridTrackSize(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyGridRowStart(StyleResolverState& state)
{
    state.style()->setGridRowStart(ComputedStyle::initialGridRowStart());
}

void StyleBuilderFunctions::applyInheritCSSPropertyGridRowStart(StyleResolverState& state)
{
    state.style()->setGridRowStart(state.parentStyle()->gridRowStart());
}

void StyleBuilderFunctions::applyValueCSSPropertyGridRowStart(StyleResolverState& state, CSSValue* value)
{
    state.style()->setGridRowStart(StyleBuilderConverter::convertGridPosition(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitTextStrokeWidth(StyleResolverState& state)
{
    state.style()->setTextStrokeWidth(ComputedStyle::initialTextStrokeWidth());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitTextStrokeWidth(StyleResolverState& state)
{
    state.style()->setTextStrokeWidth(state.parentStyle()->textStrokeWidth());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitTextStrokeWidth(StyleResolverState& state, CSSValue* value)
{
    state.style()->setTextStrokeWidth(StyleBuilderConverter::convertTextStrokeWidth(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyStrokeDashoffset(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setStrokeDashOffset(SVGComputedStyle::initialStrokeDashOffset());
}

void StyleBuilderFunctions::applyInheritCSSPropertyStrokeDashoffset(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setStrokeDashOffset(state.parentStyle()->svgStyle().strokeDashOffset());
}

void StyleBuilderFunctions::applyValueCSSPropertyStrokeDashoffset(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle().setStrokeDashOffset(StyleBuilderConverter::convertLength(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyPosition(StyleResolverState& state)
{
    state.style()->setPosition(ComputedStyle::initialPosition());
}

void StyleBuilderFunctions::applyInheritCSSPropertyPosition(StyleResolverState& state)
{
    state.style()->setPosition(state.parentStyle()->position());
}

void StyleBuilderFunctions::applyValueCSSPropertyPosition(StyleResolverState& state, CSSValue* value)
{
    state.style()->setPosition(static_cast<EPosition>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyGlyphOrientationVertical(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setGlyphOrientationVertical(SVGComputedStyle::initialGlyphOrientationVertical());
}

void StyleBuilderFunctions::applyInheritCSSPropertyGlyphOrientationVertical(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setGlyphOrientationVertical(state.parentStyle()->svgStyle().glyphOrientationVertical());
}

void StyleBuilderFunctions::applyInitialCSSPropertyFontStretch(StyleResolverState& state)
{
    state.fontBuilder().setStretch(FontBuilder::initialStretch());
}

void StyleBuilderFunctions::applyInheritCSSPropertyFontStretch(StyleResolverState& state)
{
    state.fontBuilder().setStretch(state.parentFontDescription().stretch());
}

void StyleBuilderFunctions::applyValueCSSPropertyFontStretch(StyleResolverState& state, CSSValue* value)
{
    state.fontBuilder().setStretch(static_cast<FontStretch>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyMixBlendMode(StyleResolverState& state)
{
    state.style()->setBlendMode(ComputedStyle::initialBlendMode());
}

void StyleBuilderFunctions::applyInheritCSSPropertyMixBlendMode(StyleResolverState& state)
{
    state.style()->setBlendMode(state.parentStyle()->blendMode());
}

void StyleBuilderFunctions::applyValueCSSPropertyMixBlendMode(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBlendMode(static_cast<blink::WebBlendMode>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyBaselineShift(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setBaselineShift(SVGComputedStyle::initialBaselineShift());
}

void StyleBuilderFunctions::applyInitialCSSPropertyPaddingLeft(StyleResolverState& state)
{
    state.style()->setPaddingLeft(ComputedStyle::initialPadding());
}

void StyleBuilderFunctions::applyInheritCSSPropertyPaddingLeft(StyleResolverState& state)
{
    state.style()->setPaddingLeft(state.parentStyle()->paddingLeft());
}

void StyleBuilderFunctions::applyValueCSSPropertyPaddingLeft(StyleResolverState& state, CSSValue* value)
{
    state.style()->setPaddingLeft(StyleBuilderConverter::convertLength(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWhiteSpace(StyleResolverState& state)
{
    state.style()->setWhiteSpace(ComputedStyle::initialWhiteSpace());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWhiteSpace(StyleResolverState& state)
{
    state.style()->setWhiteSpace(state.parentStyle()->whiteSpace());
}

void StyleBuilderFunctions::applyValueCSSPropertyWhiteSpace(StyleResolverState& state, CSSValue* value)
{
    state.style()->setWhiteSpace(static_cast<EWhiteSpace>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyOverflowWrap(StyleResolverState& state)
{
    state.style()->setOverflowWrap(ComputedStyle::initialOverflowWrap());
}

void StyleBuilderFunctions::applyInheritCSSPropertyOverflowWrap(StyleResolverState& state)
{
    state.style()->setOverflowWrap(state.parentStyle()->overflowWrap());
}

void StyleBuilderFunctions::applyValueCSSPropertyOverflowWrap(StyleResolverState& state, CSSValue* value)
{
    state.style()->setOverflowWrap(static_cast<EOverflowWrap>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyVerticalAlign(StyleResolverState& state)
{
    state.style()->setVerticalAlign(ComputedStyle::initialVerticalAlign());
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitLocale(StyleResolverState& state)
{
    state.style()->setLocale(ComputedStyle::initialLocale());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitLocale(StyleResolverState& state)
{
    state.style()->setLocale(state.parentStyle()->locale());
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitMarginAfterCollapse(StyleResolverState& state)
{
    state.style()->setMarginAfterCollapse(ComputedStyle::initialMarginAfterCollapse());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitMarginAfterCollapse(StyleResolverState& state)
{
    state.style()->setMarginAfterCollapse(state.parentStyle()->marginAfterCollapse());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitMarginAfterCollapse(StyleResolverState& state, CSSValue* value)
{
    state.style()->setMarginAfterCollapse(static_cast<EMarginCollapse>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitTextEmphasisPosition(StyleResolverState& state)
{
    state.style()->setTextEmphasisPosition(ComputedStyle::initialTextEmphasisPosition());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitTextEmphasisPosition(StyleResolverState& state)
{
    state.style()->setTextEmphasisPosition(state.parentStyle()->textEmphasisPosition());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitTextEmphasisPosition(StyleResolverState& state, CSSValue* value)
{
    state.style()->setTextEmphasisPosition(static_cast<TextEmphasisPosition>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWordSpacing(StyleResolverState& state)
{
    state.style()->setWordSpacing(ComputedStyle::initialLetterWordSpacing());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWordSpacing(StyleResolverState& state)
{
    state.style()->setWordSpacing(state.parentStyle()->wordSpacing());
}

void StyleBuilderFunctions::applyValueCSSPropertyWordSpacing(StyleResolverState& state, CSSValue* value)
{
    state.style()->setWordSpacing(StyleBuilderConverter::convertSpacing(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyPageBreakAfter(StyleResolverState& state)
{
    state.style()->setPageBreakAfter(ComputedStyle::initialPageBreak());
}

void StyleBuilderFunctions::applyInheritCSSPropertyPageBreakAfter(StyleResolverState& state)
{
    state.style()->setPageBreakAfter(state.parentStyle()->pageBreakAfter());
}

void StyleBuilderFunctions::applyValueCSSPropertyPageBreakAfter(StyleResolverState& state, CSSValue* value)
{
    state.style()->setPageBreakAfter(static_cast<EPageBreak>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyMarkerEnd(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setMarkerEndResource(SVGComputedStyle::initialMarkerEndResource());
}

void StyleBuilderFunctions::applyInheritCSSPropertyMarkerEnd(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setMarkerEndResource(state.parentStyle()->svgStyle().markerEndResource());
}

void StyleBuilderFunctions::applyValueCSSPropertyMarkerEnd(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle().setMarkerEndResource(StyleBuilderConverter::convertFragmentIdentifier(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitBoxLines(StyleResolverState& state)
{
    state.style()->setBoxLines(ComputedStyle::initialBoxLines());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitBoxLines(StyleResolverState& state)
{
    state.style()->setBoxLines(state.parentStyle()->boxLines());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitBoxLines(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBoxLines(static_cast<EBoxLines>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyTableLayout(StyleResolverState& state)
{
    state.style()->setTableLayout(ComputedStyle::initialTableLayout());
}

void StyleBuilderFunctions::applyInheritCSSPropertyTableLayout(StyleResolverState& state)
{
    state.style()->setTableLayout(state.parentStyle()->tableLayout());
}

void StyleBuilderFunctions::applyValueCSSPropertyTableLayout(StyleResolverState& state, CSSValue* value)
{
    state.style()->setTableLayout(static_cast<ETableLayout>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyBorderRightStyle(StyleResolverState& state)
{
    state.style()->setBorderRightStyle(ComputedStyle::initialBorderStyle());
}

void StyleBuilderFunctions::applyInheritCSSPropertyBorderRightStyle(StyleResolverState& state)
{
    state.style()->setBorderRightStyle(state.parentStyle()->borderRightStyle());
}

void StyleBuilderFunctions::applyValueCSSPropertyBorderRightStyle(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBorderRightStyle(static_cast<EBorderStyle>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyGridRowEnd(StyleResolverState& state)
{
    state.style()->setGridRowEnd(ComputedStyle::initialGridRowEnd());
}

void StyleBuilderFunctions::applyInheritCSSPropertyGridRowEnd(StyleResolverState& state)
{
    state.style()->setGridRowEnd(state.parentStyle()->gridRowEnd());
}

void StyleBuilderFunctions::applyValueCSSPropertyGridRowEnd(StyleResolverState& state, CSSValue* value)
{
    state.style()->setGridRowEnd(StyleBuilderConverter::convertGridPosition(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyTextUnderlinePosition(StyleResolverState& state)
{
    state.style()->setTextUnderlinePosition(ComputedStyle::initialTextUnderlinePosition());
}

void StyleBuilderFunctions::applyInheritCSSPropertyTextUnderlinePosition(StyleResolverState& state)
{
    state.style()->setTextUnderlinePosition(state.parentStyle()->textUnderlinePosition());
}

void StyleBuilderFunctions::applyValueCSSPropertyTextUnderlinePosition(StyleResolverState& state, CSSValue* value)
{
    state.style()->setTextUnderlinePosition(static_cast<TextUnderlinePosition>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyBackfaceVisibility(StyleResolverState& state)
{
    state.style()->setBackfaceVisibility(ComputedStyle::initialBackfaceVisibility());
}

void StyleBuilderFunctions::applyInheritCSSPropertyBackfaceVisibility(StyleResolverState& state)
{
    state.style()->setBackfaceVisibility(state.parentStyle()->backfaceVisibility());
}

void StyleBuilderFunctions::applyValueCSSPropertyBackfaceVisibility(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBackfaceVisibility(static_cast<EBackfaceVisibility>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyLeft(StyleResolverState& state)
{
    state.style()->setLeft(ComputedStyle::initialOffset());
}

void StyleBuilderFunctions::applyInheritCSSPropertyLeft(StyleResolverState& state)
{
    state.style()->setLeft(state.parentStyle()->left());
}

void StyleBuilderFunctions::applyValueCSSPropertyLeft(StyleResolverState& state, CSSValue* value)
{
    state.style()->setLeft(StyleBuilderConverter::convertLengthOrAuto(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWidth(StyleResolverState& state)
{
    state.style()->setWidth(ComputedStyle::initialSize());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWidth(StyleResolverState& state)
{
    state.style()->setWidth(state.parentStyle()->width());
}

void StyleBuilderFunctions::applyValueCSSPropertyWidth(StyleResolverState& state, CSSValue* value)
{
    state.style()->setWidth(StyleBuilderConverter::convertLengthSizing(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyTouchAction(StyleResolverState& state)
{
    state.style()->setTouchAction(ComputedStyle::initialTouchAction());
}

void StyleBuilderFunctions::applyInheritCSSPropertyTouchAction(StyleResolverState& state)
{
    state.style()->setTouchAction(state.parentStyle()->touchAction());
}

void StyleBuilderFunctions::applyValueCSSPropertyTouchAction(StyleResolverState& state, CSSValue* value)
{
    state.style()->setTouchAction(StyleBuilderConverter::convertFlags<TouchAction>(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitClipPath(StyleResolverState& state)
{
    state.style()->setClipPath(ComputedStyle::initialClipPath());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitClipPath(StyleResolverState& state)
{
    state.style()->setClipPath(state.parentStyle()->clipPath());
}

void StyleBuilderFunctions::applyInitialCSSPropertyStrokeMiterlimit(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setStrokeMiterLimit(SVGComputedStyle::initialStrokeMiterLimit());
}

void StyleBuilderFunctions::applyInheritCSSPropertyStrokeMiterlimit(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setStrokeMiterLimit(state.parentStyle()->svgStyle().strokeMiterLimit());
}

void StyleBuilderFunctions::applyValueCSSPropertyStrokeMiterlimit(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle().setStrokeMiterLimit(static_cast<float>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyGridColumnStart(StyleResolverState& state)
{
    state.style()->setGridColumnStart(ComputedStyle::initialGridColumnStart());
}

void StyleBuilderFunctions::applyInheritCSSPropertyGridColumnStart(StyleResolverState& state)
{
    state.style()->setGridColumnStart(state.parentStyle()->gridColumnStart());
}

void StyleBuilderFunctions::applyValueCSSPropertyGridColumnStart(StyleResolverState& state, CSSValue* value)
{
    state.style()->setGridColumnStart(StyleBuilderConverter::convertGridPosition(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyMarginBottom(StyleResolverState& state)
{
    state.style()->setMarginBottom(ComputedStyle::initialMargin());
}

void StyleBuilderFunctions::applyInheritCSSPropertyMarginBottom(StyleResolverState& state)
{
    state.style()->setMarginBottom(state.parentStyle()->marginBottom());
}

void StyleBuilderFunctions::applyValueCSSPropertyMarginBottom(StyleResolverState& state, CSSValue* value)
{
    state.style()->setMarginBottom(StyleBuilderConverter::convertQuirkyLength(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyListStylePosition(StyleResolverState& state)
{
    state.style()->setListStylePosition(ComputedStyle::initialListStylePosition());
}

void StyleBuilderFunctions::applyInheritCSSPropertyListStylePosition(StyleResolverState& state)
{
    state.style()->setListStylePosition(state.parentStyle()->listStylePosition());
}

void StyleBuilderFunctions::applyValueCSSPropertyListStylePosition(StyleResolverState& state, CSSValue* value)
{
    state.style()->setListStylePosition(static_cast<EListStylePosition>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitFilter(StyleResolverState& state)
{
    state.style()->setFilter(ComputedStyle::initialFilter());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitFilter(StyleResolverState& state)
{
    state.style()->setFilter(state.parentStyle()->filter());
}

void StyleBuilderFunctions::applyInitialCSSPropertyOutlineOffset(StyleResolverState& state)
{
    state.style()->setOutlineOffset(ComputedStyle::initialOutlineOffset());
}

void StyleBuilderFunctions::applyInheritCSSPropertyOutlineOffset(StyleResolverState& state)
{
    state.style()->setOutlineOffset(state.parentStyle()->outlineOffset());
}

void StyleBuilderFunctions::applyValueCSSPropertyOutlineOffset(StyleResolverState& state, CSSValue* value)
{
    state.style()->setOutlineOffset(StyleBuilderConverter::convertComputedLength<int>(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyShapeRendering(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setShapeRendering(SVGComputedStyle::initialShapeRendering());
}

void StyleBuilderFunctions::applyInheritCSSPropertyShapeRendering(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setShapeRendering(state.parentStyle()->svgStyle().shapeRendering());
}

void StyleBuilderFunctions::applyValueCSSPropertyShapeRendering(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle().setShapeRendering(static_cast<EShapeRendering>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyJustifyItems(StyleResolverState& state)
{
    state.style()->setJustifyItems(ComputedStyle::initialSelfAlignment());
}

void StyleBuilderFunctions::applyInheritCSSPropertyJustifyItems(StyleResolverState& state)
{
    state.style()->setJustifyItems(state.parentStyle()->justifyItems());
}

void StyleBuilderFunctions::applyValueCSSPropertyJustifyItems(StyleResolverState& state, CSSValue* value)
{
    state.style()->setJustifyItems(StyleBuilderConverter::convertSelfOrDefaultAlignmentData(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyRotate(StyleResolverState& state)
{
    state.style()->setRotate(ComputedStyle::initialRotate());
}

void StyleBuilderFunctions::applyInheritCSSPropertyRotate(StyleResolverState& state)
{
    state.style()->setRotate(state.parentStyle()->rotate());
}

void StyleBuilderFunctions::applyValueCSSPropertyRotate(StyleResolverState& state, CSSValue* value)
{
    state.style()->setRotate(StyleBuilderConverter::convertRotate(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyColorRendering(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setColorRendering(SVGComputedStyle::initialColorRendering());
}

void StyleBuilderFunctions::applyInheritCSSPropertyColorRendering(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setColorRendering(state.parentStyle()->svgStyle().colorRendering());
}

void StyleBuilderFunctions::applyValueCSSPropertyColorRendering(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle().setColorRendering(static_cast<EColorRendering>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyStrokeLinejoin(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setJoinStyle(SVGComputedStyle::initialJoinStyle());
}

void StyleBuilderFunctions::applyInheritCSSPropertyStrokeLinejoin(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setJoinStyle(state.parentStyle()->svgStyle().joinStyle());
}

void StyleBuilderFunctions::applyValueCSSPropertyStrokeLinejoin(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle().setJoinStyle(static_cast<LineJoin>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyFlexGrow(StyleResolverState& state)
{
    state.style()->setFlexGrow(ComputedStyle::initialFlexGrow());
}

void StyleBuilderFunctions::applyInheritCSSPropertyFlexGrow(StyleResolverState& state)
{
    state.style()->setFlexGrow(state.parentStyle()->flexGrow());
}

void StyleBuilderFunctions::applyValueCSSPropertyFlexGrow(StyleResolverState& state, CSSValue* value)
{
    state.style()->setFlexGrow(static_cast<float>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyMask(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setMaskerResource(SVGComputedStyle::initialMaskerResource());
}

void StyleBuilderFunctions::applyInheritCSSPropertyMask(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setMaskerResource(state.parentStyle()->svgStyle().maskerResource());
}

void StyleBuilderFunctions::applyValueCSSPropertyMask(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle().setMaskerResource(StyleBuilderConverter::convertFragmentIdentifier(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitMarginBeforeCollapse(StyleResolverState& state)
{
    state.style()->setMarginBeforeCollapse(ComputedStyle::initialMarginBeforeCollapse());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitMarginBeforeCollapse(StyleResolverState& state)
{
    state.style()->setMarginBeforeCollapse(state.parentStyle()->marginBeforeCollapse());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitMarginBeforeCollapse(StyleResolverState& state, CSSValue* value)
{
    state.style()->setMarginBeforeCollapse(static_cast<EMarginCollapse>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyPointerEvents(StyleResolverState& state)
{
    state.style()->setPointerEvents(ComputedStyle::initialPointerEvents());
}

void StyleBuilderFunctions::applyInheritCSSPropertyPointerEvents(StyleResolverState& state)
{
    state.style()->setPointerEvents(state.parentStyle()->pointerEvents());
}

void StyleBuilderFunctions::applyValueCSSPropertyPointerEvents(StyleResolverState& state, CSSValue* value)
{
    state.style()->setPointerEvents(static_cast<EPointerEvents>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitBoxOrient(StyleResolverState& state)
{
    state.style()->setBoxOrient(ComputedStyle::initialBoxOrient());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitBoxOrient(StyleResolverState& state)
{
    state.style()->setBoxOrient(state.parentStyle()->boxOrient());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitBoxOrient(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBoxOrient(static_cast<EBoxOrient>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyStrokeLinecap(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setCapStyle(SVGComputedStyle::initialCapStyle());
}

void StyleBuilderFunctions::applyInheritCSSPropertyStrokeLinecap(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setCapStyle(state.parentStyle()->svgStyle().capStyle());
}

void StyleBuilderFunctions::applyValueCSSPropertyStrokeLinecap(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle().setCapStyle(static_cast<LineCap>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyBorderLeftStyle(StyleResolverState& state)
{
    state.style()->setBorderLeftStyle(ComputedStyle::initialBorderStyle());
}

void StyleBuilderFunctions::applyInheritCSSPropertyBorderLeftStyle(StyleResolverState& state)
{
    state.style()->setBorderLeftStyle(state.parentStyle()->borderLeftStyle());
}

void StyleBuilderFunctions::applyValueCSSPropertyBorderLeftStyle(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBorderLeftStyle(static_cast<EBorderStyle>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitColumnRuleWidth(StyleResolverState& state)
{
    state.style()->setColumnRuleWidth(ComputedStyle::initialColumnRuleWidth());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitColumnRuleWidth(StyleResolverState& state)
{
    state.style()->setColumnRuleWidth(state.parentStyle()->columnRuleWidth());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitColumnRuleWidth(StyleResolverState& state, CSSValue* value)
{
    state.style()->setColumnRuleWidth(StyleBuilderConverter::convertLineWidth<unsigned short>(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitUserDrag(StyleResolverState& state)
{
    state.style()->setUserDrag(ComputedStyle::initialUserDrag());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitUserDrag(StyleResolverState& state)
{
    state.style()->setUserDrag(state.parentStyle()->userDrag());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitUserDrag(StyleResolverState& state, CSSValue* value)
{
    state.style()->setUserDrag(static_cast<EUserDrag>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyFontVariantLigatures(StyleResolverState& state)
{
    state.fontBuilder().setVariantLigatures(FontBuilder::initialVariantLigatures());
}

void StyleBuilderFunctions::applyInheritCSSPropertyFontVariantLigatures(StyleResolverState& state)
{
    state.fontBuilder().setVariantLigatures(state.parentFontDescription().variantLigatures());
}

void StyleBuilderFunctions::applyValueCSSPropertyFontVariantLigatures(StyleResolverState& state, CSSValue* value)
{
    state.fontBuilder().setVariantLigatures(StyleBuilderConverter::convertFontVariantLigatures(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyMarkerMid(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setMarkerMidResource(SVGComputedStyle::initialMarkerMidResource());
}

void StyleBuilderFunctions::applyInheritCSSPropertyMarkerMid(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setMarkerMidResource(state.parentStyle()->svgStyle().markerMidResource());
}

void StyleBuilderFunctions::applyValueCSSPropertyMarkerMid(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle().setMarkerMidResource(StyleBuilderConverter::convertFragmentIdentifier(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitTextCombine(StyleResolverState& state)
{
    state.style()->setTextCombine(ComputedStyle::initialTextCombine());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitTextCombine(StyleResolverState& state)
{
    state.style()->setTextCombine(state.parentStyle()->textCombine());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitTextCombine(StyleResolverState& state, CSSValue* value)
{
    state.style()->setTextCombine(static_cast<TextCombine>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyX(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setX(SVGComputedStyle::initialX());
}

void StyleBuilderFunctions::applyInheritCSSPropertyX(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setX(state.parentStyle()->svgStyle().x());
}

void StyleBuilderFunctions::applyValueCSSPropertyX(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle().setX(StyleBuilderConverter::convertLength(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyY(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setY(SVGComputedStyle::initialY());
}

void StyleBuilderFunctions::applyInheritCSSPropertyY(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setY(state.parentStyle()->svgStyle().y());
}

void StyleBuilderFunctions::applyValueCSSPropertyY(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle().setY(StyleBuilderConverter::convertLength(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyFilter(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setFilterResource(SVGComputedStyle::initialFilterResource());
}

void StyleBuilderFunctions::applyInheritCSSPropertyFilter(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setFilterResource(state.parentStyle()->svgStyle().filterResource());
}

void StyleBuilderFunctions::applyValueCSSPropertyFilter(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle().setFilterResource(StyleBuilderConverter::convertFragmentIdentifier(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyR(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setR(SVGComputedStyle::initialR());
}

void StyleBuilderFunctions::applyInheritCSSPropertyR(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setR(state.parentStyle()->svgStyle().r());
}

void StyleBuilderFunctions::applyValueCSSPropertyR(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle().setR(StyleBuilderConverter::convertLength(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyResize(StyleResolverState& state)
{
    state.style()->setResize(ComputedStyle::initialResize());
}

void StyleBuilderFunctions::applyInheritCSSPropertyResize(StyleResolverState& state)
{
    state.style()->setResize(state.parentStyle()->resize());
}

void StyleBuilderFunctions::applyInitialCSSPropertyFontSizeAdjust(StyleResolverState& state)
{
    state.fontBuilder().setSizeAdjust(FontBuilder::initialSizeAdjust());
}

void StyleBuilderFunctions::applyInheritCSSPropertyFontSizeAdjust(StyleResolverState& state)
{
    state.fontBuilder().setSizeAdjust(state.parentFontDescription().sizeAdjust());
}

void StyleBuilderFunctions::applyValueCSSPropertyFontSizeAdjust(StyleResolverState& state, CSSValue* value)
{
    state.fontBuilder().setSizeAdjust(StyleBuilderConverter::convertFontSizeAdjust(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyFlexShrink(StyleResolverState& state)
{
    state.style()->setFlexShrink(ComputedStyle::initialFlexShrink());
}

void StyleBuilderFunctions::applyInheritCSSPropertyFlexShrink(StyleResolverState& state)
{
    state.style()->setFlexShrink(state.parentStyle()->flexShrink());
}

void StyleBuilderFunctions::applyValueCSSPropertyFlexShrink(StyleResolverState& state, CSSValue* value)
{
    state.style()->setFlexShrink(static_cast<float>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitTextOrientation(StyleResolverState& state)
{
    state.style()->setTextOrientation(ComputedStyle::initialTextOrientation());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitTextOrientation(StyleResolverState& state)
{
    state.style()->setTextOrientation(state.parentStyle()->textOrientation());
}

void StyleBuilderFunctions::applyInitialCSSPropertyColorInterpolationFilters(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setColorInterpolationFilters(SVGComputedStyle::initialColorInterpolationFilters());
}

void StyleBuilderFunctions::applyInheritCSSPropertyColorInterpolationFilters(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setColorInterpolationFilters(state.parentStyle()->svgStyle().colorInterpolationFilters());
}

void StyleBuilderFunctions::applyValueCSSPropertyColorInterpolationFilters(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle().setColorInterpolationFilters(static_cast<EColorInterpolation>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitColumnBreakBefore(StyleResolverState& state)
{
    state.style()->setColumnBreakBefore(ComputedStyle::initialPageBreak());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitColumnBreakBefore(StyleResolverState& state)
{
    state.style()->setColumnBreakBefore(state.parentStyle()->columnBreakBefore());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitColumnBreakBefore(StyleResolverState& state, CSSValue* value)
{
    state.style()->setColumnBreakBefore(static_cast<EPageBreak>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyTextTransform(StyleResolverState& state)
{
    state.style()->setTextTransform(ComputedStyle::initialTextTransform());
}

void StyleBuilderFunctions::applyInheritCSSPropertyTextTransform(StyleResolverState& state)
{
    state.style()->setTextTransform(state.parentStyle()->textTransform());
}

void StyleBuilderFunctions::applyValueCSSPropertyTextTransform(StyleResolverState& state, CSSValue* value)
{
    state.style()->setTextTransform(static_cast<ETextTransform>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyRight(StyleResolverState& state)
{
    state.style()->setRight(ComputedStyle::initialOffset());
}

void StyleBuilderFunctions::applyInheritCSSPropertyRight(StyleResolverState& state)
{
    state.style()->setRight(state.parentStyle()->right());
}

void StyleBuilderFunctions::applyValueCSSPropertyRight(StyleResolverState& state, CSSValue* value)
{
    state.style()->setRight(StyleBuilderConverter::convertLengthOrAuto(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyGridColumnEnd(StyleResolverState& state)
{
    state.style()->setGridColumnEnd(ComputedStyle::initialGridColumnEnd());
}

void StyleBuilderFunctions::applyInheritCSSPropertyGridColumnEnd(StyleResolverState& state)
{
    state.style()->setGridColumnEnd(state.parentStyle()->gridColumnEnd());
}

void StyleBuilderFunctions::applyValueCSSPropertyGridColumnEnd(StyleResolverState& state, CSSValue* value)
{
    state.style()->setGridColumnEnd(StyleBuilderConverter::convertGridPosition(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyScrollSnapCoordinate(StyleResolverState& state)
{
    state.style()->setScrollSnapCoordinate(ComputedStyle::initialScrollSnapCoordinate());
}

void StyleBuilderFunctions::applyInheritCSSPropertyScrollSnapCoordinate(StyleResolverState& state)
{
    state.style()->setScrollSnapCoordinate(state.parentStyle()->scrollSnapCoordinate());
}

void StyleBuilderFunctions::applyValueCSSPropertyScrollSnapCoordinate(StyleResolverState& state, CSSValue* value)
{
    state.style()->setScrollSnapCoordinate(StyleBuilderConverter::convertSnapCoordinates(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyAlignContent(StyleResolverState& state)
{
    state.style()->setAlignContent(ComputedStyle::initialContentAlignment());
}

void StyleBuilderFunctions::applyInheritCSSPropertyAlignContent(StyleResolverState& state)
{
    state.style()->setAlignContent(state.parentStyle()->alignContent());
}

void StyleBuilderFunctions::applyValueCSSPropertyAlignContent(StyleResolverState& state, CSSValue* value)
{
    state.style()->setAlignContent(StyleBuilderConverter::convertContentAlignmentData(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyDirection(StyleResolverState& state)
{
    state.style()->setDirection(ComputedStyle::initialDirection());
}

void StyleBuilderFunctions::applyInheritCSSPropertyDirection(StyleResolverState& state)
{
    state.style()->setDirection(state.parentStyle()->direction());
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitBorderImage(StyleResolverState& state)
{
    state.style()->setBorderImage(ComputedStyle::initialNinePieceImage());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitBorderImage(StyleResolverState& state)
{
    state.style()->setBorderImage(state.parentStyle()->borderImage());
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitBorderVerticalSpacing(StyleResolverState& state)
{
    state.style()->setVerticalBorderSpacing(ComputedStyle::initialVerticalBorderSpacing());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitBorderVerticalSpacing(StyleResolverState& state)
{
    state.style()->setVerticalBorderSpacing(state.parentStyle()->verticalBorderSpacing());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitBorderVerticalSpacing(StyleResolverState& state, CSSValue* value)
{
    state.style()->setVerticalBorderSpacing(StyleBuilderConverter::convertComputedLength<short>(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyMarginLeft(StyleResolverState& state)
{
    state.style()->setMarginLeft(ComputedStyle::initialMargin());
}

void StyleBuilderFunctions::applyInheritCSSPropertyMarginLeft(StyleResolverState& state)
{
    state.style()->setMarginLeft(state.parentStyle()->marginLeft());
}

void StyleBuilderFunctions::applyValueCSSPropertyMarginLeft(StyleResolverState& state, CSSValue* value)
{
    state.style()->setMarginLeft(StyleBuilderConverter::convertQuirkyLength(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyFlexWrap(StyleResolverState& state)
{
    state.style()->setFlexWrap(ComputedStyle::initialFlexWrap());
}

void StyleBuilderFunctions::applyInheritCSSPropertyFlexWrap(StyleResolverState& state)
{
    state.style()->setFlexWrap(state.parentStyle()->flexWrap());
}

void StyleBuilderFunctions::applyValueCSSPropertyFlexWrap(StyleResolverState& state, CSSValue* value)
{
    state.style()->setFlexWrap(static_cast<EFlexWrap>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyMinHeight(StyleResolverState& state)
{
    state.style()->setMinHeight(ComputedStyle::initialMinSize());
}

void StyleBuilderFunctions::applyInheritCSSPropertyMinHeight(StyleResolverState& state)
{
    state.style()->setMinHeight(state.parentStyle()->minHeight());
}

void StyleBuilderFunctions::applyValueCSSPropertyMinHeight(StyleResolverState& state, CSSValue* value)
{
    state.style()->setMinHeight(StyleBuilderConverter::convertLengthSizing(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyFloodColor(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setFloodColor(SVGComputedStyle::initialFloodColor());
}

void StyleBuilderFunctions::applyInheritCSSPropertyFloodColor(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setFloodColor(state.parentStyle()->svgStyle().floodColor());
}

void StyleBuilderFunctions::applyValueCSSPropertyFloodColor(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle().setFloodColor(StyleBuilderConverter::convertColor(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyMaxHeight(StyleResolverState& state)
{
    state.style()->setMaxHeight(ComputedStyle::initialMaxSize());
}

void StyleBuilderFunctions::applyInheritCSSPropertyMaxHeight(StyleResolverState& state)
{
    state.style()->setMaxHeight(state.parentStyle()->maxHeight());
}

void StyleBuilderFunctions::applyValueCSSPropertyMaxHeight(StyleResolverState& state, CSSValue* value)
{
    state.style()->setMaxHeight(StyleBuilderConverter::convertLengthMaxSizing(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyBufferedRendering(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setBufferedRendering(SVGComputedStyle::initialBufferedRendering());
}

void StyleBuilderFunctions::applyInheritCSSPropertyBufferedRendering(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setBufferedRendering(state.parentStyle()->svgStyle().bufferedRendering());
}

void StyleBuilderFunctions::applyValueCSSPropertyBufferedRendering(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle().setBufferedRendering(static_cast<EBufferedRendering>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitWritingMode(StyleResolverState& state)
{
    state.style()->setWritingMode(ComputedStyle::initialWritingMode());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitWritingMode(StyleResolverState& state)
{
    state.style()->setWritingMode(state.parentStyle()->writingMode());
}

void StyleBuilderFunctions::applyInitialCSSPropertyAlignmentBaseline(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setAlignmentBaseline(SVGComputedStyle::initialAlignmentBaseline());
}

void StyleBuilderFunctions::applyInheritCSSPropertyAlignmentBaseline(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setAlignmentBaseline(state.parentStyle()->svgStyle().alignmentBaseline());
}

void StyleBuilderFunctions::applyValueCSSPropertyAlignmentBaseline(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle().setAlignmentBaseline(static_cast<EAlignmentBaseline>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyMinWidth(StyleResolverState& state)
{
    state.style()->setMinWidth(ComputedStyle::initialMinSize());
}

void StyleBuilderFunctions::applyInheritCSSPropertyMinWidth(StyleResolverState& state)
{
    state.style()->setMinWidth(state.parentStyle()->minWidth());
}

void StyleBuilderFunctions::applyValueCSSPropertyMinWidth(StyleResolverState& state, CSSValue* value)
{
    state.style()->setMinWidth(StyleBuilderConverter::convertLengthSizing(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyMaskType(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setMaskType(SVGComputedStyle::initialMaskType());
}

void StyleBuilderFunctions::applyInheritCSSPropertyMaskType(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setMaskType(state.parentStyle()->svgStyle().maskType());
}

void StyleBuilderFunctions::applyValueCSSPropertyMaskType(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle().setMaskType(static_cast<EMaskType>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitColumnRuleStyle(StyleResolverState& state)
{
    state.style()->setColumnRuleStyle(ComputedStyle::initialBorderStyle());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitColumnRuleStyle(StyleResolverState& state)
{
    state.style()->setColumnRuleStyle(state.parentStyle()->columnRuleStyle());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitColumnRuleStyle(StyleResolverState& state, CSSValue* value)
{
    state.style()->setColumnRuleStyle(static_cast<EBorderStyle>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitBoxFlexGroup(StyleResolverState& state)
{
    state.style()->setBoxFlexGroup(ComputedStyle::initialBoxFlexGroup());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitBoxFlexGroup(StyleResolverState& state)
{
    state.style()->setBoxFlexGroup(state.parentStyle()->boxFlexGroup());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitBoxFlexGroup(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBoxFlexGroup(static_cast<unsigned int>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyTextAlignLast(StyleResolverState& state)
{
    state.style()->setTextAlignLast(ComputedStyle::initialTextAlignLast());
}

void StyleBuilderFunctions::applyInheritCSSPropertyTextAlignLast(StyleResolverState& state)
{
    state.style()->setTextAlignLast(state.parentStyle()->textAlignLast());
}

void StyleBuilderFunctions::applyValueCSSPropertyTextAlignLast(StyleResolverState& state, CSSValue* value)
{
    state.style()->setTextAlignLast(static_cast<TextAlignLast>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitMarginTopCollapse(StyleResolverState& state)
{
    state.style()->setMarginBeforeCollapse(ComputedStyle::initialMarginBeforeCollapse());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitMarginTopCollapse(StyleResolverState& state)
{
    state.style()->setMarginBeforeCollapse(state.parentStyle()->marginBeforeCollapse());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitMarginTopCollapse(StyleResolverState& state, CSSValue* value)
{
    state.style()->setMarginBeforeCollapse(static_cast<EMarginCollapse>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyTabSize(StyleResolverState& state)
{
    state.style()->setTabSize(ComputedStyle::initialTabSize());
}

void StyleBuilderFunctions::applyInheritCSSPropertyTabSize(StyleResolverState& state)
{
    state.style()->setTabSize(state.parentStyle()->tabSize());
}

void StyleBuilderFunctions::applyValueCSSPropertyTabSize(StyleResolverState& state, CSSValue* value)
{
    state.style()->setTabSize(StyleBuilderConverter::convertLengthOrTabSpaces(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyBorderBottomWidth(StyleResolverState& state)
{
    state.style()->setBorderBottomWidth(ComputedStyle::initialBorderWidth());
}

void StyleBuilderFunctions::applyInheritCSSPropertyBorderBottomWidth(StyleResolverState& state)
{
    state.style()->setBorderBottomWidth(state.parentStyle()->borderBottomWidth());
}

void StyleBuilderFunctions::applyValueCSSPropertyBorderBottomWidth(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBorderBottomWidth(StyleBuilderConverter::convertLineWidth<unsigned>(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitHighlight(StyleResolverState& state)
{
    state.style()->setHighlight(ComputedStyle::initialHighlight());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitHighlight(StyleResolverState& state)
{
    state.style()->setHighlight(state.parentStyle()->highlight());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitHighlight(StyleResolverState& state, CSSValue* value)
{
    state.style()->setHighlight(StyleBuilderConverter::convertString<CSSValueNone>(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyEmptyCells(StyleResolverState& state)
{
    state.style()->setEmptyCells(ComputedStyle::initialEmptyCells());
}

void StyleBuilderFunctions::applyInheritCSSPropertyEmptyCells(StyleResolverState& state)
{
    state.style()->setEmptyCells(state.parentStyle()->emptyCells());
}

void StyleBuilderFunctions::applyValueCSSPropertyEmptyCells(StyleResolverState& state, CSSValue* value)
{
    state.style()->setEmptyCells(static_cast<EEmptyCell>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyBorderImageSource(StyleResolverState& state)
{
    state.style()->setBorderImageSource(ComputedStyle::initialBorderImageSource());
}

void StyleBuilderFunctions::applyInheritCSSPropertyBorderImageSource(StyleResolverState& state)
{
    state.style()->setBorderImageSource(state.parentStyle()->borderImageSource());
}

void StyleBuilderFunctions::applyInitialCSSPropertyMotionOffset(StyleResolverState& state)
{
    state.style()->setMotionOffset(ComputedStyle::initialMotionOffset());
}

void StyleBuilderFunctions::applyInheritCSSPropertyMotionOffset(StyleResolverState& state)
{
    state.style()->setMotionOffset(state.parentStyle()->motionOffset());
}

void StyleBuilderFunctions::applyValueCSSPropertyMotionOffset(StyleResolverState& state, CSSValue* value)
{
    state.style()->setMotionOffset(StyleBuilderConverter::convertLength(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyTextOverflow(StyleResolverState& state)
{
    state.style()->setTextOverflow(ComputedStyle::initialTextOverflow());
}

void StyleBuilderFunctions::applyInheritCSSPropertyTextOverflow(StyleResolverState& state)
{
    state.style()->setTextOverflow(state.parentStyle()->textOverflow());
}

void StyleBuilderFunctions::applyValueCSSPropertyTextOverflow(StyleResolverState& state, CSSValue* value)
{
    state.style()->setTextOverflow(static_cast<TextOverflow>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyBoxSizing(StyleResolverState& state)
{
    state.style()->setBoxSizing(ComputedStyle::initialBoxSizing());
}

void StyleBuilderFunctions::applyInheritCSSPropertyBoxSizing(StyleResolverState& state)
{
    state.style()->setBoxSizing(state.parentStyle()->boxSizing());
}

void StyleBuilderFunctions::applyValueCSSPropertyBoxSizing(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBoxSizing(static_cast<EBoxSizing>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyStrokeWidth(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setStrokeWidth(SVGComputedStyle::initialStrokeWidth());
}

void StyleBuilderFunctions::applyInheritCSSPropertyStrokeWidth(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setStrokeWidth(state.parentStyle()->svgStyle().strokeWidth());
}

void StyleBuilderFunctions::applyValueCSSPropertyStrokeWidth(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle().setStrokeWidth(StyleBuilderConverter::convertUnzoomedLength(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyMarkerStart(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setMarkerStartResource(SVGComputedStyle::initialMarkerStartResource());
}

void StyleBuilderFunctions::applyInheritCSSPropertyMarkerStart(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setMarkerStartResource(state.parentStyle()->svgStyle().markerStartResource());
}

void StyleBuilderFunctions::applyValueCSSPropertyMarkerStart(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle().setMarkerStartResource(StyleBuilderConverter::convertFragmentIdentifier(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyTextDecorationStyle(StyleResolverState& state)
{
    state.style()->setTextDecorationStyle(ComputedStyle::initialTextDecorationStyle());
}

void StyleBuilderFunctions::applyInheritCSSPropertyTextDecorationStyle(StyleResolverState& state)
{
    state.style()->setTextDecorationStyle(state.parentStyle()->textDecorationStyle());
}

void StyleBuilderFunctions::applyValueCSSPropertyTextDecorationStyle(StyleResolverState& state, CSSValue* value)
{
    state.style()->setTextDecorationStyle(static_cast<TextDecorationStyle>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyTranslate(StyleResolverState& state)
{
    state.style()->setTranslate(ComputedStyle::initialTranslate());
}

void StyleBuilderFunctions::applyInheritCSSPropertyTranslate(StyleResolverState& state)
{
    state.style()->setTranslate(state.parentStyle()->translate());
}

void StyleBuilderFunctions::applyValueCSSPropertyTranslate(StyleResolverState& state, CSSValue* value)
{
    state.style()->setTranslate(StyleBuilderConverter::convertTranslate(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyPaddingTop(StyleResolverState& state)
{
    state.style()->setPaddingTop(ComputedStyle::initialPadding());
}

void StyleBuilderFunctions::applyInheritCSSPropertyPaddingTop(StyleResolverState& state)
{
    state.style()->setPaddingTop(state.parentStyle()->paddingTop());
}

void StyleBuilderFunctions::applyValueCSSPropertyPaddingTop(StyleResolverState& state, CSSValue* value)
{
    state.style()->setPaddingTop(StyleBuilderConverter::convertLength(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyDisplay(StyleResolverState& state)
{
    state.style()->setDisplay(ComputedStyle::initialDisplay());
}

void StyleBuilderFunctions::applyInheritCSSPropertyDisplay(StyleResolverState& state)
{
    state.style()->setDisplay(state.parentStyle()->display());
}

void StyleBuilderFunctions::applyValueCSSPropertyDisplay(StyleResolverState& state, CSSValue* value)
{
    state.style()->setDisplay(static_cast<EDisplay>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWordBreak(StyleResolverState& state)
{
    state.style()->setWordBreak(ComputedStyle::initialWordBreak());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWordBreak(StyleResolverState& state)
{
    state.style()->setWordBreak(state.parentStyle()->wordBreak());
}

void StyleBuilderFunctions::applyValueCSSPropertyWordBreak(StyleResolverState& state, CSSValue* value)
{
    state.style()->setWordBreak(static_cast<EWordBreak>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyBorderTopStyle(StyleResolverState& state)
{
    state.style()->setBorderTopStyle(ComputedStyle::initialBorderStyle());
}

void StyleBuilderFunctions::applyInheritCSSPropertyBorderTopStyle(StyleResolverState& state)
{
    state.style()->setBorderTopStyle(state.parentStyle()->borderTopStyle());
}

void StyleBuilderFunctions::applyValueCSSPropertyBorderTopStyle(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBorderTopStyle(static_cast<EBorderStyle>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyJustifyContent(StyleResolverState& state)
{
    state.style()->setJustifyContent(ComputedStyle::initialContentAlignment());
}

void StyleBuilderFunctions::applyInheritCSSPropertyJustifyContent(StyleResolverState& state)
{
    state.style()->setJustifyContent(state.parentStyle()->justifyContent());
}

void StyleBuilderFunctions::applyValueCSSPropertyJustifyContent(StyleResolverState& state, CSSValue* value)
{
    state.style()->setJustifyContent(StyleBuilderConverter::convertContentAlignmentData(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyBoxShadow(StyleResolverState& state)
{
    state.style()->setBoxShadow(ComputedStyle::initialBoxShadow());
}

void StyleBuilderFunctions::applyInheritCSSPropertyBoxShadow(StyleResolverState& state)
{
    state.style()->setBoxShadow(state.parentStyle()->boxShadow());
}

void StyleBuilderFunctions::applyValueCSSPropertyBoxShadow(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBoxShadow(StyleBuilderConverter::convertShadow(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertySpeak(StyleResolverState& state)
{
    state.style()->setSpeak(ComputedStyle::initialSpeak());
}

void StyleBuilderFunctions::applyInheritCSSPropertySpeak(StyleResolverState& state)
{
    state.style()->setSpeak(state.parentStyle()->speak());
}

void StyleBuilderFunctions::applyValueCSSPropertySpeak(StyleResolverState& state, CSSValue* value)
{
    state.style()->setSpeak(static_cast<ESpeak>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyListStyleImage(StyleResolverState& state)
{
    state.style()->setListStyleImage(ComputedStyle::initialListStyleImage());
}

void StyleBuilderFunctions::applyInheritCSSPropertyListStyleImage(StyleResolverState& state)
{
    state.style()->setListStyleImage(state.parentStyle()->listStyleImage());
}

void StyleBuilderFunctions::applyInitialCSSPropertyFloodOpacity(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setFloodOpacity(SVGComputedStyle::initialFloodOpacity());
}

void StyleBuilderFunctions::applyInheritCSSPropertyFloodOpacity(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setFloodOpacity(state.parentStyle()->svgStyle().floodOpacity());
}

void StyleBuilderFunctions::applyValueCSSPropertyFloodOpacity(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle().setFloodOpacity(StyleBuilderConverter::convertNumberOrPercentage(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyClipRule(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setClipRule(SVGComputedStyle::initialClipRule());
}

void StyleBuilderFunctions::applyInheritCSSPropertyClipRule(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setClipRule(state.parentStyle()->svgStyle().clipRule());
}

void StyleBuilderFunctions::applyValueCSSPropertyClipRule(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle().setClipRule(static_cast<WindRule>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyFlexDirection(StyleResolverState& state)
{
    state.style()->setFlexDirection(ComputedStyle::initialFlexDirection());
}

void StyleBuilderFunctions::applyInheritCSSPropertyFlexDirection(StyleResolverState& state)
{
    state.style()->setFlexDirection(state.parentStyle()->flexDirection());
}

void StyleBuilderFunctions::applyValueCSSPropertyFlexDirection(StyleResolverState& state, CSSValue* value)
{
    state.style()->setFlexDirection(static_cast<EFlexDirection>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyLightingColor(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setLightingColor(SVGComputedStyle::initialLightingColor());
}

void StyleBuilderFunctions::applyInheritCSSPropertyLightingColor(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setLightingColor(state.parentStyle()->svgStyle().lightingColor());
}

void StyleBuilderFunctions::applyValueCSSPropertyLightingColor(StyleResolverState& state, CSSValue* value)
{
    state.style()->accessSVGStyle().setLightingColor(StyleBuilderConverter::convertColor(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyPageBreakInside(StyleResolverState& state)
{
    state.style()->setPageBreakInside(ComputedStyle::initialPageBreak());
}

void StyleBuilderFunctions::applyInheritCSSPropertyPageBreakInside(StyleResolverState& state)
{
    state.style()->setPageBreakInside(state.parentStyle()->pageBreakInside());
}

void StyleBuilderFunctions::applyValueCSSPropertyPageBreakInside(StyleResolverState& state, CSSValue* value)
{
    state.style()->setPageBreakInside(static_cast<EPageBreak>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyListStyleType(StyleResolverState& state)
{
    state.style()->setListStyleType(ComputedStyle::initialListStyleType());
}

void StyleBuilderFunctions::applyInheritCSSPropertyListStyleType(StyleResolverState& state)
{
    state.style()->setListStyleType(state.parentStyle()->listStyleType());
}

void StyleBuilderFunctions::applyValueCSSPropertyListStyleType(StyleResolverState& state, CSSValue* value)
{
    state.style()->setListStyleType(static_cast<EListStyleType>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyTextAlign(StyleResolverState& state)
{
    state.style()->setTextAlign(ComputedStyle::initialTextAlign());
}

void StyleBuilderFunctions::applyInheritCSSPropertyTextAlign(StyleResolverState& state)
{
    state.style()->setTextAlign(state.parentStyle()->textAlign());
}

void StyleBuilderFunctions::applyInitialCSSPropertyAlignItems(StyleResolverState& state)
{
    state.style()->setAlignItems(ComputedStyle::initialSelfAlignment());
}

void StyleBuilderFunctions::applyInheritCSSPropertyAlignItems(StyleResolverState& state)
{
    state.style()->setAlignItems(state.parentStyle()->alignItems());
}

void StyleBuilderFunctions::applyValueCSSPropertyAlignItems(StyleResolverState& state, CSSValue* value)
{
    state.style()->setAlignItems(StyleBuilderConverter::convertSelfOrDefaultAlignmentData(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyObjectPosition(StyleResolverState& state)
{
    state.style()->setObjectPosition(ComputedStyle::initialObjectPosition());
}

void StyleBuilderFunctions::applyInheritCSSPropertyObjectPosition(StyleResolverState& state)
{
    state.style()->setObjectPosition(state.parentStyle()->objectPosition());
}

void StyleBuilderFunctions::applyValueCSSPropertyObjectPosition(StyleResolverState& state, CSSValue* value)
{
    state.style()->setObjectPosition(StyleBuilderConverter::convertPosition(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyScrollSnapDestination(StyleResolverState& state)
{
    state.style()->setScrollSnapDestination(ComputedStyle::initialScrollSnapDestination());
}

void StyleBuilderFunctions::applyInheritCSSPropertyScrollSnapDestination(StyleResolverState& state)
{
    state.style()->setScrollSnapDestination(state.parentStyle()->scrollSnapDestination());
}

void StyleBuilderFunctions::applyValueCSSPropertyScrollSnapDestination(StyleResolverState& state, CSSValue* value)
{
    state.style()->setScrollSnapDestination(StyleBuilderConverter::convertPosition(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitPerspectiveOriginY(StyleResolverState& state)
{
    state.style()->setPerspectiveOriginY(ComputedStyle::initialPerspectiveOriginY());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitPerspectiveOriginY(StyleResolverState& state)
{
    state.style()->setPerspectiveOriginY(state.parentStyle()->perspectiveOriginY());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitPerspectiveOriginY(StyleResolverState& state, CSSValue* value)
{
    state.style()->setPerspectiveOriginY(StyleBuilderConverter::convertLength(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitPerspectiveOriginX(StyleResolverState& state)
{
    state.style()->setPerspectiveOriginX(ComputedStyle::initialPerspectiveOriginX());
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitPerspectiveOriginX(StyleResolverState& state)
{
    state.style()->setPerspectiveOriginX(state.parentStyle()->perspectiveOriginX());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitPerspectiveOriginX(StyleResolverState& state, CSSValue* value)
{
    state.style()->setPerspectiveOriginX(StyleBuilderConverter::convertLength(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyFontSize(StyleResolverState& state)
{
    state.fontBuilder().setSize(FontBuilder::initialSize());
}

void StyleBuilderFunctions::applyInheritCSSPropertyFontSize(StyleResolverState& state)
{
    state.fontBuilder().setSize(state.parentFontDescription().size());
}

void StyleBuilderFunctions::applyValueCSSPropertyFontSize(StyleResolverState& state, CSSValue* value)
{
    state.fontBuilder().setSize(StyleBuilderConverter::convertFontSize(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyVisibility(StyleResolverState& state)
{
    state.style()->setVisibility(ComputedStyle::initialVisibility());
}

void StyleBuilderFunctions::applyInheritCSSPropertyVisibility(StyleResolverState& state)
{
    state.style()->setVisibility(state.parentStyle()->visibility());
}

void StyleBuilderFunctions::applyValueCSSPropertyVisibility(StyleResolverState& state, CSSValue* value)
{
    state.style()->setVisibility(static_cast<EVisibility>(*toCSSPrimitiveValue(value)));
}


void StyleBuilderFunctions::applyInitialCSSPropertyAnimationDelay(StyleResolverState& state)
{
    CSSAnimationData& data = state.style()->accessAnimations();
    data.delayList().clear();
    data.delayList().append(CSSAnimationData::initialDelay());
}

void StyleBuilderFunctions::applyInheritCSSPropertyAnimationDelay(StyleResolverState& state)
{
    const CSSAnimationData* parentData = state.parentStyle()->animations();
    if (!parentData)
        applyInitialCSSPropertyAnimationDelay(state);
    else
        state.style()->accessAnimations().delayList() = parentData->delayList();
}

void StyleBuilderFunctions::applyValueCSSPropertyAnimationDelay(StyleResolverState& state, CSSValue* value)
{
    CSSAnimationData& data = state.style()->accessAnimations();
    data.delayList().clear();
    for (auto& listValue : toCSSValueList(*value))
        data.delayList().append(CSSToStyleMap::mapAnimationDelay(listValue.get()));
}

void StyleBuilderFunctions::applyInitialCSSPropertyAnimationDirection(StyleResolverState& state)
{
    CSSAnimationData& data = state.style()->accessAnimations();
    data.directionList().clear();
    data.directionList().append(CSSAnimationData::initialDirection());
}

void StyleBuilderFunctions::applyInheritCSSPropertyAnimationDirection(StyleResolverState& state)
{
    const CSSAnimationData* parentData = state.parentStyle()->animations();
    if (!parentData)
        applyInitialCSSPropertyAnimationDirection(state);
    else
        state.style()->accessAnimations().directionList() = parentData->directionList();
}

void StyleBuilderFunctions::applyValueCSSPropertyAnimationDirection(StyleResolverState& state, CSSValue* value)
{
    CSSAnimationData& data = state.style()->accessAnimations();
    data.directionList().clear();
    for (auto& listValue : toCSSValueList(*value))
        data.directionList().append(CSSToStyleMap::mapAnimationDirection(listValue.get()));
}

void StyleBuilderFunctions::applyInitialCSSPropertyAnimationDuration(StyleResolverState& state)
{
    CSSAnimationData& data = state.style()->accessAnimations();
    data.durationList().clear();
    data.durationList().append(CSSAnimationData::initialDuration());
}

void StyleBuilderFunctions::applyInheritCSSPropertyAnimationDuration(StyleResolverState& state)
{
    const CSSAnimationData* parentData = state.parentStyle()->animations();
    if (!parentData)
        applyInitialCSSPropertyAnimationDuration(state);
    else
        state.style()->accessAnimations().durationList() = parentData->durationList();
}

void StyleBuilderFunctions::applyValueCSSPropertyAnimationDuration(StyleResolverState& state, CSSValue* value)
{
    CSSAnimationData& data = state.style()->accessAnimations();
    data.durationList().clear();
    for (auto& listValue : toCSSValueList(*value))
        data.durationList().append(CSSToStyleMap::mapAnimationDuration(listValue.get()));
}

void StyleBuilderFunctions::applyInitialCSSPropertyAnimationFillMode(StyleResolverState& state)
{
    CSSAnimationData& data = state.style()->accessAnimations();
    data.fillModeList().clear();
    data.fillModeList().append(CSSAnimationData::initialFillMode());
}

void StyleBuilderFunctions::applyInheritCSSPropertyAnimationFillMode(StyleResolverState& state)
{
    const CSSAnimationData* parentData = state.parentStyle()->animations();
    if (!parentData)
        applyInitialCSSPropertyAnimationFillMode(state);
    else
        state.style()->accessAnimations().fillModeList() = parentData->fillModeList();
}

void StyleBuilderFunctions::applyValueCSSPropertyAnimationFillMode(StyleResolverState& state, CSSValue* value)
{
    CSSAnimationData& data = state.style()->accessAnimations();
    data.fillModeList().clear();
    for (auto& listValue : toCSSValueList(*value))
        data.fillModeList().append(CSSToStyleMap::mapAnimationFillMode(listValue.get()));
}

void StyleBuilderFunctions::applyInitialCSSPropertyAnimationIterationCount(StyleResolverState& state)
{
    CSSAnimationData& data = state.style()->accessAnimations();
    data.iterationCountList().clear();
    data.iterationCountList().append(CSSAnimationData::initialIterationCount());
}

void StyleBuilderFunctions::applyInheritCSSPropertyAnimationIterationCount(StyleResolverState& state)
{
    const CSSAnimationData* parentData = state.parentStyle()->animations();
    if (!parentData)
        applyInitialCSSPropertyAnimationIterationCount(state);
    else
        state.style()->accessAnimations().iterationCountList() = parentData->iterationCountList();
}

void StyleBuilderFunctions::applyValueCSSPropertyAnimationIterationCount(StyleResolverState& state, CSSValue* value)
{
    CSSAnimationData& data = state.style()->accessAnimations();
    data.iterationCountList().clear();
    for (auto& listValue : toCSSValueList(*value))
        data.iterationCountList().append(CSSToStyleMap::mapAnimationIterationCount(listValue.get()));
}

void StyleBuilderFunctions::applyInitialCSSPropertyAnimationName(StyleResolverState& state)
{
    CSSAnimationData& data = state.style()->accessAnimations();
    data.nameList().clear();
    data.nameList().append(CSSAnimationData::initialName());
}

void StyleBuilderFunctions::applyInheritCSSPropertyAnimationName(StyleResolverState& state)
{
    const CSSAnimationData* parentData = state.parentStyle()->animations();
    if (!parentData)
        applyInitialCSSPropertyAnimationName(state);
    else
        state.style()->accessAnimations().nameList() = parentData->nameList();
}

void StyleBuilderFunctions::applyValueCSSPropertyAnimationName(StyleResolverState& state, CSSValue* value)
{
    CSSAnimationData& data = state.style()->accessAnimations();
    data.nameList().clear();
    for (auto& listValue : toCSSValueList(*value))
        data.nameList().append(CSSToStyleMap::mapAnimationName(listValue.get()));
}

void StyleBuilderFunctions::applyInitialCSSPropertyAnimationPlayState(StyleResolverState& state)
{
    CSSAnimationData& data = state.style()->accessAnimations();
    data.playStateList().clear();
    data.playStateList().append(CSSAnimationData::initialPlayState());
}

void StyleBuilderFunctions::applyInheritCSSPropertyAnimationPlayState(StyleResolverState& state)
{
    const CSSAnimationData* parentData = state.parentStyle()->animations();
    if (!parentData)
        applyInitialCSSPropertyAnimationPlayState(state);
    else
        state.style()->accessAnimations().playStateList() = parentData->playStateList();
}

void StyleBuilderFunctions::applyValueCSSPropertyAnimationPlayState(StyleResolverState& state, CSSValue* value)
{
    CSSAnimationData& data = state.style()->accessAnimations();
    data.playStateList().clear();
    for (auto& listValue : toCSSValueList(*value))
        data.playStateList().append(CSSToStyleMap::mapAnimationPlayState(listValue.get()));
}

void StyleBuilderFunctions::applyInitialCSSPropertyAnimationTimingFunction(StyleResolverState& state)
{
    CSSAnimationData& data = state.style()->accessAnimations();
    data.timingFunctionList().clear();
    data.timingFunctionList().append(CSSAnimationData::initialTimingFunction());
}

void StyleBuilderFunctions::applyInheritCSSPropertyAnimationTimingFunction(StyleResolverState& state)
{
    const CSSAnimationData* parentData = state.parentStyle()->animations();
    if (!parentData)
        applyInitialCSSPropertyAnimationTimingFunction(state);
    else
        state.style()->accessAnimations().timingFunctionList() = parentData->timingFunctionList();
}

void StyleBuilderFunctions::applyValueCSSPropertyAnimationTimingFunction(StyleResolverState& state, CSSValue* value)
{
    CSSAnimationData& data = state.style()->accessAnimations();
    data.timingFunctionList().clear();
    for (auto& listValue : toCSSValueList(*value))
        data.timingFunctionList().append(CSSToStyleMap::mapAnimationTimingFunction(listValue.get()));
}

void StyleBuilderFunctions::applyInitialCSSPropertyTransitionDelay(StyleResolverState& state)
{
    CSSTransitionData& data = state.style()->accessTransitions();
    data.delayList().clear();
    data.delayList().append(CSSTransitionData::initialDelay());
}

void StyleBuilderFunctions::applyInheritCSSPropertyTransitionDelay(StyleResolverState& state)
{
    const CSSTransitionData* parentData = state.parentStyle()->transitions();
    if (!parentData)
        applyInitialCSSPropertyTransitionDelay(state);
    else
        state.style()->accessTransitions().delayList() = parentData->delayList();
}

void StyleBuilderFunctions::applyValueCSSPropertyTransitionDelay(StyleResolverState& state, CSSValue* value)
{
    CSSTransitionData& data = state.style()->accessTransitions();
    data.delayList().clear();
    for (auto& listValue : toCSSValueList(*value))
        data.delayList().append(CSSToStyleMap::mapAnimationDelay(listValue.get()));
}

void StyleBuilderFunctions::applyInitialCSSPropertyTransitionDuration(StyleResolverState& state)
{
    CSSTransitionData& data = state.style()->accessTransitions();
    data.durationList().clear();
    data.durationList().append(CSSTransitionData::initialDuration());
}

void StyleBuilderFunctions::applyInheritCSSPropertyTransitionDuration(StyleResolverState& state)
{
    const CSSTransitionData* parentData = state.parentStyle()->transitions();
    if (!parentData)
        applyInitialCSSPropertyTransitionDuration(state);
    else
        state.style()->accessTransitions().durationList() = parentData->durationList();
}

void StyleBuilderFunctions::applyValueCSSPropertyTransitionDuration(StyleResolverState& state, CSSValue* value)
{
    CSSTransitionData& data = state.style()->accessTransitions();
    data.durationList().clear();
    for (auto& listValue : toCSSValueList(*value))
        data.durationList().append(CSSToStyleMap::mapAnimationDuration(listValue.get()));
}

void StyleBuilderFunctions::applyInitialCSSPropertyTransitionProperty(StyleResolverState& state)
{
    CSSTransitionData& data = state.style()->accessTransitions();
    data.propertyList().clear();
    data.propertyList().append(CSSTransitionData::initialProperty());
}

void StyleBuilderFunctions::applyInheritCSSPropertyTransitionProperty(StyleResolverState& state)
{
    const CSSTransitionData* parentData = state.parentStyle()->transitions();
    if (!parentData)
        applyInitialCSSPropertyTransitionProperty(state);
    else
        state.style()->accessTransitions().propertyList() = parentData->propertyList();
}

void StyleBuilderFunctions::applyValueCSSPropertyTransitionProperty(StyleResolverState& state, CSSValue* value)
{
    CSSTransitionData& data = state.style()->accessTransitions();
    data.propertyList().clear();
    for (auto& listValue : toCSSValueList(*value))
        data.propertyList().append(CSSToStyleMap::mapAnimationProperty(listValue.get()));
}

void StyleBuilderFunctions::applyInitialCSSPropertyTransitionTimingFunction(StyleResolverState& state)
{
    CSSTransitionData& data = state.style()->accessTransitions();
    data.timingFunctionList().clear();
    data.timingFunctionList().append(CSSTransitionData::initialTimingFunction());
}

void StyleBuilderFunctions::applyInheritCSSPropertyTransitionTimingFunction(StyleResolverState& state)
{
    const CSSTransitionData* parentData = state.parentStyle()->transitions();
    if (!parentData)
        applyInitialCSSPropertyTransitionTimingFunction(state);
    else
        state.style()->accessTransitions().timingFunctionList() = parentData->timingFunctionList();
}

void StyleBuilderFunctions::applyValueCSSPropertyTransitionTimingFunction(StyleResolverState& state, CSSValue* value)
{
    CSSTransitionData& data = state.style()->accessTransitions();
    data.timingFunctionList().clear();
    for (auto& listValue : toCSSValueList(*value))
        data.timingFunctionList().append(CSSToStyleMap::mapAnimationTimingFunction(listValue.get()));
}


void StyleBuilderFunctions::applyInitialCSSPropertyClip(StyleResolverState& state)
{
    state.style()->setHasAutoClip();
}

void StyleBuilderFunctions::applyInheritCSSPropertyClip(StyleResolverState& state)
{
    if (state.parentStyle()->hasAutoClip())
        state.style()->setHasAutoClip();
    else
        state.style()->setClip(state.parentStyle()->clip());
}

void StyleBuilderFunctions::applyValueCSSPropertyClip(StyleResolverState& state, CSSValue* value)
{
    if (!value->isPrimitiveValue())
        return;

    CSSPrimitiveValue* primitiveValue = toCSSPrimitiveValue(value);
    if (primitiveValue->getValueID() == CSSValueAuto)
        state.style()->setHasAutoClip();
    else
        state.style()->setClip(StyleBuilderConverter::convertClip(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyOrphans(StyleResolverState& state)
{
    state.style()->setHasAutoOrphans();
}

void StyleBuilderFunctions::applyInheritCSSPropertyOrphans(StyleResolverState& state)
{
    if (state.parentStyle()->hasAutoOrphans())
        state.style()->setHasAutoOrphans();
    else
        state.style()->setOrphans(state.parentStyle()->orphans());
}

void StyleBuilderFunctions::applyValueCSSPropertyOrphans(StyleResolverState& state, CSSValue* value)
{
    if (!value->isPrimitiveValue())
        return;

    CSSPrimitiveValue* primitiveValue = toCSSPrimitiveValue(value);
    if (primitiveValue->getValueID() == CSSValueAuto)
        state.style()->setHasAutoOrphans();
    else
        state.style()->setOrphans(static_cast<short>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitColumnCount(StyleResolverState& state)
{
    state.style()->setHasAutoColumnCount();
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitColumnCount(StyleResolverState& state)
{
    if (state.parentStyle()->hasAutoColumnCount())
        state.style()->setHasAutoColumnCount();
    else
        state.style()->setColumnCount(state.parentStyle()->columnCount());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitColumnCount(StyleResolverState& state, CSSValue* value)
{
    if (!value->isPrimitiveValue())
        return;

    CSSPrimitiveValue* primitiveValue = toCSSPrimitiveValue(value);
    if (primitiveValue->getValueID() == CSSValueAuto)
        state.style()->setHasAutoColumnCount();
    else
        state.style()->setColumnCount(static_cast<unsigned short>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitColumnGap(StyleResolverState& state)
{
    state.style()->setHasNormalColumnGap();
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitColumnGap(StyleResolverState& state)
{
    if (state.parentStyle()->hasNormalColumnGap())
        state.style()->setHasNormalColumnGap();
    else
        state.style()->setColumnGap(state.parentStyle()->columnGap());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitColumnGap(StyleResolverState& state, CSSValue* value)
{
    if (!value->isPrimitiveValue())
        return;

    CSSPrimitiveValue* primitiveValue = toCSSPrimitiveValue(value);
    if (primitiveValue->getValueID() == CSSValueNormal)
        state.style()->setHasNormalColumnGap();
    else
        state.style()->setColumnGap(StyleBuilderConverter::convertComputedLength<float>(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitColumnWidth(StyleResolverState& state)
{
    state.style()->setHasAutoColumnWidth();
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitColumnWidth(StyleResolverState& state)
{
    if (state.parentStyle()->hasAutoColumnWidth())
        state.style()->setHasAutoColumnWidth();
    else
        state.style()->setColumnWidth(state.parentStyle()->columnWidth());
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitColumnWidth(StyleResolverState& state, CSSValue* value)
{
    if (!value->isPrimitiveValue())
        return;

    CSSPrimitiveValue* primitiveValue = toCSSPrimitiveValue(value);
    if (primitiveValue->getValueID() == CSSValueAuto)
        state.style()->setHasAutoColumnWidth();
    else
        state.style()->setColumnWidth(StyleBuilderConverter::convertComputedLength<float>(state, value));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWidows(StyleResolverState& state)
{
    state.style()->setHasAutoWidows();
}

void StyleBuilderFunctions::applyInheritCSSPropertyWidows(StyleResolverState& state)
{
    if (state.parentStyle()->hasAutoWidows())
        state.style()->setHasAutoWidows();
    else
        state.style()->setWidows(state.parentStyle()->widows());
}

void StyleBuilderFunctions::applyValueCSSPropertyWidows(StyleResolverState& state, CSSValue* value)
{
    if (!value->isPrimitiveValue())
        return;

    CSSPrimitiveValue* primitiveValue = toCSSPrimitiveValue(value);
    if (primitiveValue->getValueID() == CSSValueAuto)
        state.style()->setHasAutoWidows();
    else
        state.style()->setWidows(static_cast<short>(*toCSSPrimitiveValue(value)));
}

void StyleBuilderFunctions::applyInitialCSSPropertyZIndex(StyleResolverState& state)
{
    state.style()->setHasAutoZIndex();
}

void StyleBuilderFunctions::applyInheritCSSPropertyZIndex(StyleResolverState& state)
{
    if (state.parentStyle()->hasAutoZIndex())
        state.style()->setHasAutoZIndex();
    else
        state.style()->setZIndex(state.parentStyle()->zIndex());
}

void StyleBuilderFunctions::applyValueCSSPropertyZIndex(StyleResolverState& state, CSSValue* value)
{
    if (!value->isPrimitiveValue())
        return;

    CSSPrimitiveValue* primitiveValue = toCSSPrimitiveValue(value);
    if (primitiveValue->getValueID() == CSSValueAuto)
        state.style()->setHasAutoZIndex();
    else
        state.style()->setZIndex(static_cast<int>(*toCSSPrimitiveValue(value)));
}


static bool lengthMatchesAllSides(const LengthBox& lengthBox, const Length& length)
{
    return (lengthBox.left() == length
        && lengthBox.right() == length
        && lengthBox.top() == length
        && lengthBox.bottom() == length);
}

static bool borderImageLengthMatchesAllSides(const BorderImageLengthBox& borderImageLengthBox, const BorderImageLength& borderImageLength)
{
    return (borderImageLengthBox.left() == borderImageLength
        && borderImageLengthBox.right() == borderImageLength
        && borderImageLengthBox.top() == borderImageLength
        && borderImageLengthBox.bottom() == borderImageLength);
}

void StyleBuilderFunctions::applyInitialCSSPropertyBorderImageOutset(StyleResolverState& state)
{
    const NinePieceImage& currentImage = state.style()->borderImage();
    if (borderImageLengthMatchesAllSides(currentImage.outset(), BorderImageLength(Length(0, Fixed))))
        return;

    NinePieceImage image(currentImage);
    image.setOutset(Length(0, Fixed));
    state.style()->setBorderImage(image);
}

void StyleBuilderFunctions::applyInheritCSSPropertyBorderImageOutset(StyleResolverState& state)
{
    NinePieceImage image(state.style()->borderImage());
    image.copyOutsetFrom(state.parentStyle()->borderImage());
    state.style()->setBorderImage(image);
}

void StyleBuilderFunctions::applyValueCSSPropertyBorderImageOutset(StyleResolverState& state, CSSValue* value)
{
    NinePieceImage image(state.style()->borderImage());
    image.setOutset(CSSToStyleMap::mapNinePieceImageQuad(state, value));
    state.style()->setBorderImage(image);
}

void StyleBuilderFunctions::applyInitialCSSPropertyBorderImageRepeat(StyleResolverState& state)
{
    const NinePieceImage& currentImage = state.style()->borderImage();
    if (currentImage.horizontalRule() == StretchImageRule && currentImage.verticalRule() == StretchImageRule)
        return;

    NinePieceImage image(currentImage);
    image.setHorizontalRule(StretchImageRule);
    image.setVerticalRule(StretchImageRule);
    state.style()->setBorderImage(image);
}

void StyleBuilderFunctions::applyInheritCSSPropertyBorderImageRepeat(StyleResolverState& state)
{
    NinePieceImage image(state.style()->borderImage());
    image.copyRepeatFrom(state.parentStyle()->borderImage());
    state.style()->setBorderImage(image);
}

void StyleBuilderFunctions::applyValueCSSPropertyBorderImageRepeat(StyleResolverState& state, CSSValue* value)
{
    NinePieceImage image(state.style()->borderImage());
    CSSToStyleMap::mapNinePieceImageRepeat(state, value, image);
    state.style()->setBorderImage(image);
}

void StyleBuilderFunctions::applyInitialCSSPropertyBorderImageSlice(StyleResolverState& state)
{
    const NinePieceImage& currentImage = state.style()->borderImage();
    if (currentImage.fill() == false && lengthMatchesAllSides(currentImage.imageSlices(), Length(100, Percent)))
        return;

    NinePieceImage image(currentImage);
    image.setImageSlices(LengthBox(Length(100, Percent), Length(100, Percent), Length(100, Percent), Length(100, Percent)));
    image.setFill(false);
    state.style()->setBorderImage(image);
}

void StyleBuilderFunctions::applyInheritCSSPropertyBorderImageSlice(StyleResolverState& state)
{
    NinePieceImage image(state.style()->borderImage());
    image.copyImageSlicesFrom(state.parentStyle()->borderImage());
    state.style()->setBorderImage(image);
}

void StyleBuilderFunctions::applyValueCSSPropertyBorderImageSlice(StyleResolverState& state, CSSValue* value)
{
    NinePieceImage image(state.style()->borderImage());
    CSSToStyleMap::mapNinePieceImageSlice(state, value, image);
    state.style()->setBorderImage(image);
}

void StyleBuilderFunctions::applyInitialCSSPropertyBorderImageWidth(StyleResolverState& state)
{
    const NinePieceImage& currentImage = state.style()->borderImage();
    if (borderImageLengthMatchesAllSides(currentImage.borderSlices(), BorderImageLength(1.0)))
        return;

    NinePieceImage image(currentImage);
    image.setBorderSlices(1.0);
    state.style()->setBorderImage(image);
}

void StyleBuilderFunctions::applyInheritCSSPropertyBorderImageWidth(StyleResolverState& state)
{
    NinePieceImage image(state.style()->borderImage());
    image.copyBorderSlicesFrom(state.parentStyle()->borderImage());
    state.style()->setBorderImage(image);
}

void StyleBuilderFunctions::applyValueCSSPropertyBorderImageWidth(StyleResolverState& state, CSSValue* value)
{
    NinePieceImage image(state.style()->borderImage());
    image.setBorderSlices(CSSToStyleMap::mapNinePieceImageQuad(state, value));
    state.style()->setBorderImage(image);
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitMaskBoxImageOutset(StyleResolverState& state)
{
    const NinePieceImage& currentImage = state.style()->maskBoxImage();
    if (borderImageLengthMatchesAllSides(currentImage.outset(), BorderImageLength(Length(0, Fixed))))
        return;

    NinePieceImage image(currentImage);
    image.setOutset(Length(0, Fixed));
    state.style()->setMaskBoxImage(image);
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitMaskBoxImageOutset(StyleResolverState& state)
{
    NinePieceImage image(state.style()->maskBoxImage());
    image.copyOutsetFrom(state.parentStyle()->maskBoxImage());
    state.style()->setMaskBoxImage(image);
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitMaskBoxImageOutset(StyleResolverState& state, CSSValue* value)
{
    NinePieceImage image(state.style()->maskBoxImage());
    image.setOutset(CSSToStyleMap::mapNinePieceImageQuad(state, value));
    state.style()->setMaskBoxImage(image);
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitMaskBoxImageRepeat(StyleResolverState& state)
{
    const NinePieceImage& currentImage = state.style()->maskBoxImage();
    if (currentImage.horizontalRule() == StretchImageRule && currentImage.verticalRule() == StretchImageRule)
        return;

    NinePieceImage image(currentImage);
    image.setHorizontalRule(StretchImageRule);
    image.setVerticalRule(StretchImageRule);
    state.style()->setMaskBoxImage(image);
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitMaskBoxImageRepeat(StyleResolverState& state)
{
    NinePieceImage image(state.style()->maskBoxImage());
    image.copyRepeatFrom(state.parentStyle()->maskBoxImage());
    state.style()->setMaskBoxImage(image);
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitMaskBoxImageRepeat(StyleResolverState& state, CSSValue* value)
{
    NinePieceImage image(state.style()->maskBoxImage());
    CSSToStyleMap::mapNinePieceImageRepeat(state, value, image);
    state.style()->setMaskBoxImage(image);
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitMaskBoxImageSlice(StyleResolverState& state)
{
    const NinePieceImage& currentImage = state.style()->maskBoxImage();
    // Masks have a different initial value for slices. Preserve the value of 0 for backwards compatibility.
    if (currentImage.fill() == true && lengthMatchesAllSides(currentImage.imageSlices(), Length(0, Fixed)))
        return;

    NinePieceImage image(currentImage);
    image.setImageSlices(LengthBox(Length(0, Fixed), Length(0, Fixed), Length(0, Fixed), Length(0, Fixed)));
    image.setFill(true);
    state.style()->setMaskBoxImage(image);
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitMaskBoxImageSlice(StyleResolverState& state)
{
    NinePieceImage image(state.style()->maskBoxImage());
    image.copyImageSlicesFrom(state.parentStyle()->maskBoxImage());
    state.style()->setMaskBoxImage(image);
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitMaskBoxImageSlice(StyleResolverState& state, CSSValue* value)
{
    NinePieceImage image(state.style()->maskBoxImage());
    CSSToStyleMap::mapNinePieceImageSlice(state, value, image);
    state.style()->setMaskBoxImage(image);
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitMaskBoxImageWidth(StyleResolverState& state)
{
    const NinePieceImage& currentImage = state.style()->maskBoxImage();
    // Masks have a different initial value for widths. Preserve the value of 'auto' for backwards compatibility.
    if (borderImageLengthMatchesAllSides(currentImage.borderSlices(), BorderImageLength(Length(Auto))))
        return;

    NinePieceImage image(currentImage);
    image.setBorderSlices(Length(Auto));
    state.style()->setMaskBoxImage(image);
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitMaskBoxImageWidth(StyleResolverState& state)
{
    NinePieceImage image(state.style()->maskBoxImage());
    image.copyBorderSlicesFrom(state.parentStyle()->maskBoxImage());
    state.style()->setMaskBoxImage(image);
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitMaskBoxImageWidth(StyleResolverState& state, CSSValue* value)
{
    NinePieceImage image(state.style()->maskBoxImage());
    image.setBorderSlices(CSSToStyleMap::mapNinePieceImageQuad(state, value));
    state.style()->setMaskBoxImage(image);
}


void StyleBuilderFunctions::applyValueCSSPropertyBorderImageSource(StyleResolverState& state, CSSValue* value)
{
    state.style()->setBorderImageSource(state.styleImage(CSSPropertyBorderImageSource, value));
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitMaskBoxImageSource(StyleResolverState& state, CSSValue* value)
{
    state.style()->setMaskBoxImageSource(state.styleImage(CSSPropertyWebkitMaskBoxImageSource, value));
}


void StyleBuilderFunctions::applyInitialCSSPropertyBackgroundColor(StyleResolverState& state)
{
    StyleColor color = ComputedStyle::initialBackgroundColor();
    if (state.applyPropertyToRegularStyle())
        state.style()->setBackgroundColor(color);
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkBackgroundColor(color);
}

void StyleBuilderFunctions::applyInheritCSSPropertyBackgroundColor(StyleResolverState& state)
{
    // Visited link style can never explicitly inherit from parent visited link style so no separate getters are needed.
    StyleColor color = state.parentStyle()->backgroundColor();
    if (state.applyPropertyToRegularStyle())
        state.style()->setBackgroundColor(color);
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkBackgroundColor(color);
}

void StyleBuilderFunctions::applyValueCSSPropertyBackgroundColor(StyleResolverState& state, CSSValue* value)
{
    if (state.applyPropertyToRegularStyle())
        state.style()->setBackgroundColor(StyleBuilderConverter::convertStyleColor(state, value));
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkBackgroundColor(StyleBuilderConverter::convertStyleColor(state, value, true));
}

void StyleBuilderFunctions::applyInitialCSSPropertyBorderBottomColor(StyleResolverState& state)
{
    StyleColor color = StyleColor::currentColor();
    if (state.applyPropertyToRegularStyle())
        state.style()->setBorderBottomColor(color);
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkBorderBottomColor(color);
}

void StyleBuilderFunctions::applyInheritCSSPropertyBorderBottomColor(StyleResolverState& state)
{
    // Visited link style can never explicitly inherit from parent visited link style so no separate getters are needed.
    StyleColor color = state.parentStyle()->borderBottomColor();
    if (state.applyPropertyToRegularStyle())
        state.style()->setBorderBottomColor(color);
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkBorderBottomColor(color);
}

void StyleBuilderFunctions::applyValueCSSPropertyBorderBottomColor(StyleResolverState& state, CSSValue* value)
{
    if (state.applyPropertyToRegularStyle())
        state.style()->setBorderBottomColor(StyleBuilderConverter::convertStyleColor(state, value));
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkBorderBottomColor(StyleBuilderConverter::convertStyleColor(state, value, true));
}

void StyleBuilderFunctions::applyInitialCSSPropertyBorderLeftColor(StyleResolverState& state)
{
    StyleColor color = StyleColor::currentColor();
    if (state.applyPropertyToRegularStyle())
        state.style()->setBorderLeftColor(color);
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkBorderLeftColor(color);
}

void StyleBuilderFunctions::applyInheritCSSPropertyBorderLeftColor(StyleResolverState& state)
{
    // Visited link style can never explicitly inherit from parent visited link style so no separate getters are needed.
    StyleColor color = state.parentStyle()->borderLeftColor();
    if (state.applyPropertyToRegularStyle())
        state.style()->setBorderLeftColor(color);
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkBorderLeftColor(color);
}

void StyleBuilderFunctions::applyValueCSSPropertyBorderLeftColor(StyleResolverState& state, CSSValue* value)
{
    if (state.applyPropertyToRegularStyle())
        state.style()->setBorderLeftColor(StyleBuilderConverter::convertStyleColor(state, value));
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkBorderLeftColor(StyleBuilderConverter::convertStyleColor(state, value, true));
}

void StyleBuilderFunctions::applyInitialCSSPropertyBorderRightColor(StyleResolverState& state)
{
    StyleColor color = StyleColor::currentColor();
    if (state.applyPropertyToRegularStyle())
        state.style()->setBorderRightColor(color);
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkBorderRightColor(color);
}

void StyleBuilderFunctions::applyInheritCSSPropertyBorderRightColor(StyleResolverState& state)
{
    // Visited link style can never explicitly inherit from parent visited link style so no separate getters are needed.
    StyleColor color = state.parentStyle()->borderRightColor();
    if (state.applyPropertyToRegularStyle())
        state.style()->setBorderRightColor(color);
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkBorderRightColor(color);
}

void StyleBuilderFunctions::applyValueCSSPropertyBorderRightColor(StyleResolverState& state, CSSValue* value)
{
    if (state.applyPropertyToRegularStyle())
        state.style()->setBorderRightColor(StyleBuilderConverter::convertStyleColor(state, value));
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkBorderRightColor(StyleBuilderConverter::convertStyleColor(state, value, true));
}

void StyleBuilderFunctions::applyInitialCSSPropertyBorderTopColor(StyleResolverState& state)
{
    StyleColor color = StyleColor::currentColor();
    if (state.applyPropertyToRegularStyle())
        state.style()->setBorderTopColor(color);
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkBorderTopColor(color);
}

void StyleBuilderFunctions::applyInheritCSSPropertyBorderTopColor(StyleResolverState& state)
{
    // Visited link style can never explicitly inherit from parent visited link style so no separate getters are needed.
    StyleColor color = state.parentStyle()->borderTopColor();
    if (state.applyPropertyToRegularStyle())
        state.style()->setBorderTopColor(color);
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkBorderTopColor(color);
}

void StyleBuilderFunctions::applyValueCSSPropertyBorderTopColor(StyleResolverState& state, CSSValue* value)
{
    if (state.applyPropertyToRegularStyle())
        state.style()->setBorderTopColor(StyleBuilderConverter::convertStyleColor(state, value));
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkBorderTopColor(StyleBuilderConverter::convertStyleColor(state, value, true));
}

void StyleBuilderFunctions::applyInitialCSSPropertyOutlineColor(StyleResolverState& state)
{
    StyleColor color = StyleColor::currentColor();
    if (state.applyPropertyToRegularStyle())
        state.style()->setOutlineColor(color);
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkOutlineColor(color);
}

void StyleBuilderFunctions::applyInheritCSSPropertyOutlineColor(StyleResolverState& state)
{
    // Visited link style can never explicitly inherit from parent visited link style so no separate getters are needed.
    StyleColor color = state.parentStyle()->outlineColor();
    if (state.applyPropertyToRegularStyle())
        state.style()->setOutlineColor(color);
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkOutlineColor(color);
}

void StyleBuilderFunctions::applyValueCSSPropertyOutlineColor(StyleResolverState& state, CSSValue* value)
{
    if (state.applyPropertyToRegularStyle())
        state.style()->setOutlineColor(StyleBuilderConverter::convertStyleColor(state, value));
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkOutlineColor(StyleBuilderConverter::convertStyleColor(state, value, true));
}

void StyleBuilderFunctions::applyInitialCSSPropertyTextDecorationColor(StyleResolverState& state)
{
    StyleColor color = StyleColor::currentColor();
    if (state.applyPropertyToRegularStyle())
        state.style()->setTextDecorationColor(color);
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkTextDecorationColor(color);
}

void StyleBuilderFunctions::applyInheritCSSPropertyTextDecorationColor(StyleResolverState& state)
{
    // Visited link style can never explicitly inherit from parent visited link style so no separate getters are needed.
    StyleColor color = state.parentStyle()->textDecorationColor();
    if (state.applyPropertyToRegularStyle())
        state.style()->setTextDecorationColor(color);
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkTextDecorationColor(color);
}

void StyleBuilderFunctions::applyValueCSSPropertyTextDecorationColor(StyleResolverState& state, CSSValue* value)
{
    if (state.applyPropertyToRegularStyle())
        state.style()->setTextDecorationColor(StyleBuilderConverter::convertStyleColor(state, value));
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkTextDecorationColor(StyleBuilderConverter::convertStyleColor(state, value, true));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitColumnRuleColor(StyleResolverState& state)
{
    StyleColor color = StyleColor::currentColor();
    if (state.applyPropertyToRegularStyle())
        state.style()->setColumnRuleColor(color);
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkColumnRuleColor(color);
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitColumnRuleColor(StyleResolverState& state)
{
    // Visited link style can never explicitly inherit from parent visited link style so no separate getters are needed.
    StyleColor color = state.parentStyle()->columnRuleColor();
    if (state.applyPropertyToRegularStyle())
        state.style()->setColumnRuleColor(color);
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkColumnRuleColor(color);
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitColumnRuleColor(StyleResolverState& state, CSSValue* value)
{
    if (state.applyPropertyToRegularStyle())
        state.style()->setColumnRuleColor(StyleBuilderConverter::convertStyleColor(state, value));
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkColumnRuleColor(StyleBuilderConverter::convertStyleColor(state, value, true));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitTextEmphasisColor(StyleResolverState& state)
{
    StyleColor color = StyleColor::currentColor();
    if (state.applyPropertyToRegularStyle())
        state.style()->setTextEmphasisColor(color);
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkTextEmphasisColor(color);
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitTextEmphasisColor(StyleResolverState& state)
{
    // Visited link style can never explicitly inherit from parent visited link style so no separate getters are needed.
    StyleColor color = state.parentStyle()->textEmphasisColor();
    if (state.applyPropertyToRegularStyle())
        state.style()->setTextEmphasisColor(color);
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkTextEmphasisColor(color);
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitTextEmphasisColor(StyleResolverState& state, CSSValue* value)
{
    if (state.applyPropertyToRegularStyle())
        state.style()->setTextEmphasisColor(StyleBuilderConverter::convertStyleColor(state, value));
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkTextEmphasisColor(StyleBuilderConverter::convertStyleColor(state, value, true));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitTextFillColor(StyleResolverState& state)
{
    StyleColor color = StyleColor::currentColor();
    if (state.applyPropertyToRegularStyle())
        state.style()->setTextFillColor(color);
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkTextFillColor(color);
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitTextFillColor(StyleResolverState& state)
{
    // Visited link style can never explicitly inherit from parent visited link style so no separate getters are needed.
    StyleColor color = state.parentStyle()->textFillColor();
    if (state.applyPropertyToRegularStyle())
        state.style()->setTextFillColor(color);
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkTextFillColor(color);
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitTextFillColor(StyleResolverState& state, CSSValue* value)
{
    if (state.applyPropertyToRegularStyle())
        state.style()->setTextFillColor(StyleBuilderConverter::convertStyleColor(state, value));
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkTextFillColor(StyleBuilderConverter::convertStyleColor(state, value, true));
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitTextStrokeColor(StyleResolverState& state)
{
    StyleColor color = StyleColor::currentColor();
    if (state.applyPropertyToRegularStyle())
        state.style()->setTextStrokeColor(color);
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkTextStrokeColor(color);
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitTextStrokeColor(StyleResolverState& state)
{
    // Visited link style can never explicitly inherit from parent visited link style so no separate getters are needed.
    StyleColor color = state.parentStyle()->textStrokeColor();
    if (state.applyPropertyToRegularStyle())
        state.style()->setTextStrokeColor(color);
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkTextStrokeColor(color);
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitTextStrokeColor(StyleResolverState& state, CSSValue* value)
{
    if (state.applyPropertyToRegularStyle())
        state.style()->setTextStrokeColor(StyleBuilderConverter::convertStyleColor(state, value));
    if (state.applyPropertyToVisitedLinkStyle())
        state.style()->setVisitedLinkTextStrokeColor(StyleBuilderConverter::convertStyleColor(state, value, true));
}


void StyleBuilderFunctions::applyInitialCSSPropertyCounterIncrement(StyleResolverState& state) {
    state.style()->clearIncrementDirectives();
}

void StyleBuilderFunctions::applyInheritCSSPropertyCounterIncrement(StyleResolverState& state)
{
    const CounterDirectiveMap* parentMap = state.parentStyle()->counterDirectives();
    if (!parentMap)
        return;

    CounterDirectiveMap& map = state.style()->accessCounterDirectives();
    ASSERT(!parentMap->isEmpty());

    typedef CounterDirectiveMap::const_iterator Iterator;
    Iterator end = parentMap->end();
    for (Iterator it = parentMap->begin(); it != end; ++it) {
        CounterDirectives& directives = map.add(it->key, CounterDirectives()).storedValue->value;
        directives.inheritIncrement(it->value);
    }
}

void StyleBuilderFunctions::applyValueCSSPropertyCounterIncrement(StyleResolverState& state, CSSValue* value)
{
    state.style()->clearIncrementDirectives();

    if (!value->isValueList()) {
        ASSERT(value->isPrimitiveValue() && toCSSPrimitiveValue(value)->getValueID() == CSSValueNone);
        return;
    }

    CounterDirectiveMap& map = state.style()->accessCounterDirectives();

    CSSValueList* list = toCSSValueList(value);

    int length = list ? list->length() : 0;
    for (int i = 0; i < length; ++i) {
        CSSValue* currValue = list->item(i);
        if (!currValue->isPrimitiveValue())
            continue;

        Pair* pair = toCSSPrimitiveValue(currValue)->getPairValue();
        if (!pair || !pair->first() || !pair->second())
            continue;

        AtomicString identifier(pair->first()->getStringValue());
        int value = pair->second()->getIntValue();
        CounterDirectives& directives = map.add(identifier, CounterDirectives()).storedValue->value;
        directives.addIncrementValue(value);
    }
    ASSERT(!map.isEmpty());
}

void StyleBuilderFunctions::applyInitialCSSPropertyCounterReset(StyleResolverState& state) {
    state.style()->clearResetDirectives();
}

void StyleBuilderFunctions::applyInheritCSSPropertyCounterReset(StyleResolverState& state)
{
    const CounterDirectiveMap* parentMap = state.parentStyle()->counterDirectives();
    if (!parentMap)
        return;

    CounterDirectiveMap& map = state.style()->accessCounterDirectives();
    ASSERT(!parentMap->isEmpty());

    typedef CounterDirectiveMap::const_iterator Iterator;
    Iterator end = parentMap->end();
    for (Iterator it = parentMap->begin(); it != end; ++it) {
        CounterDirectives& directives = map.add(it->key, CounterDirectives()).storedValue->value;
        directives.inheritReset(it->value);
    }
}

void StyleBuilderFunctions::applyValueCSSPropertyCounterReset(StyleResolverState& state, CSSValue* value)
{
    state.style()->clearResetDirectives();

    if (!value->isValueList()) {
        ASSERT(value->isPrimitiveValue() && toCSSPrimitiveValue(value)->getValueID() == CSSValueNone);
        return;
    }

    CounterDirectiveMap& map = state.style()->accessCounterDirectives();

    CSSValueList* list = toCSSValueList(value);

    int length = list ? list->length() : 0;
    for (int i = 0; i < length; ++i) {
        CSSValue* currValue = list->item(i);
        if (!currValue->isPrimitiveValue())
            continue;

        Pair* pair = toCSSPrimitiveValue(currValue)->getPairValue();
        if (!pair || !pair->first() || !pair->second())
            continue;

        AtomicString identifier(pair->first()->getStringValue());
        int value = pair->second()->getIntValue();
        CounterDirectives& directives = map.add(identifier, CounterDirectives()).storedValue->value;
        directives.setResetValue(value);
    }
    ASSERT(!map.isEmpty());
}


void StyleBuilderFunctions::applyInitialCSSPropertyBackgroundAttachment(StyleResolverState& state)
{
    FillLayer* currChild = &state.style()->accessBackgroundLayers();
    currChild->setAttachment(FillLayer::initialFillAttachment(BackgroundFillLayer));
    for (currChild = currChild->next(); currChild; currChild = currChild->next())
        currChild->clearAttachment();
}

void StyleBuilderFunctions::applyInheritCSSPropertyBackgroundAttachment(StyleResolverState& state)
{
    FillLayer* currChild = &state.style()->accessBackgroundLayers();
    FillLayer* prevChild = 0;
    const FillLayer* currParent = &state.parentStyle()->backgroundLayers();
    while (currParent && currParent->isAttachmentSet()) {
        if (!currChild)
            currChild = prevChild->ensureNext();
        currChild->setAttachment(currParent->attachment());
        prevChild = currChild;
        currChild = prevChild->next();
        currParent = currParent->next();
    }

    while (currChild) {
        /* Reset any remaining layers to not have the property set. */
        currChild->clearAttachment();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyValueCSSPropertyBackgroundAttachment(StyleResolverState& state, CSSValue* value)
{
    FillLayer* currChild = &state.style()->accessBackgroundLayers();
    FillLayer* prevChild = 0;
    if (value->isValueList() && !value->isImageSetValue()) {
        /* Walk each value and put it into a layer, creating new layers as needed. */
        CSSValueList* valueList = toCSSValueList(value);
        for (unsigned int i = 0; i < valueList->length(); i++) {
            if (!currChild)
                currChild = prevChild->ensureNext();
            CSSToStyleMap::mapFillAttachment(state, currChild, valueList->item(i));
            prevChild = currChild;
            currChild = currChild->next();
        }
    } else {
        CSSToStyleMap::mapFillAttachment(state, currChild, value);
        currChild = currChild->next();
    }
    while (currChild) {
        /* Reset all remaining layers to not have the property set. */
        currChild->clearAttachment();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyInitialCSSPropertyBackgroundBlendMode(StyleResolverState& state)
{
    FillLayer* currChild = &state.style()->accessBackgroundLayers();
    currChild->setBlendMode(FillLayer::initialFillBlendMode(BackgroundFillLayer));
    for (currChild = currChild->next(); currChild; currChild = currChild->next())
        currChild->clearBlendMode();
}

void StyleBuilderFunctions::applyInheritCSSPropertyBackgroundBlendMode(StyleResolverState& state)
{
    FillLayer* currChild = &state.style()->accessBackgroundLayers();
    FillLayer* prevChild = 0;
    const FillLayer* currParent = &state.parentStyle()->backgroundLayers();
    while (currParent && currParent->isBlendModeSet()) {
        if (!currChild)
            currChild = prevChild->ensureNext();
        currChild->setBlendMode(currParent->blendMode());
        prevChild = currChild;
        currChild = prevChild->next();
        currParent = currParent->next();
    }

    while (currChild) {
        /* Reset any remaining layers to not have the property set. */
        currChild->clearBlendMode();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyValueCSSPropertyBackgroundBlendMode(StyleResolverState& state, CSSValue* value)
{
    FillLayer* currChild = &state.style()->accessBackgroundLayers();
    FillLayer* prevChild = 0;
    if (value->isValueList() && !value->isImageSetValue()) {
        /* Walk each value and put it into a layer, creating new layers as needed. */
        CSSValueList* valueList = toCSSValueList(value);
        for (unsigned int i = 0; i < valueList->length(); i++) {
            if (!currChild)
                currChild = prevChild->ensureNext();
            CSSToStyleMap::mapFillBlendMode(state, currChild, valueList->item(i));
            prevChild = currChild;
            currChild = currChild->next();
        }
    } else {
        CSSToStyleMap::mapFillBlendMode(state, currChild, value);
        currChild = currChild->next();
    }
    while (currChild) {
        /* Reset all remaining layers to not have the property set. */
        currChild->clearBlendMode();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyInitialCSSPropertyBackgroundClip(StyleResolverState& state)
{
    FillLayer* currChild = &state.style()->accessBackgroundLayers();
    currChild->setClip(FillLayer::initialFillClip(BackgroundFillLayer));
    for (currChild = currChild->next(); currChild; currChild = currChild->next())
        currChild->clearClip();
}

void StyleBuilderFunctions::applyInheritCSSPropertyBackgroundClip(StyleResolverState& state)
{
    FillLayer* currChild = &state.style()->accessBackgroundLayers();
    FillLayer* prevChild = 0;
    const FillLayer* currParent = &state.parentStyle()->backgroundLayers();
    while (currParent && currParent->isClipSet()) {
        if (!currChild)
            currChild = prevChild->ensureNext();
        currChild->setClip(currParent->clip());
        prevChild = currChild;
        currChild = prevChild->next();
        currParent = currParent->next();
    }

    while (currChild) {
        /* Reset any remaining layers to not have the property set. */
        currChild->clearClip();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyValueCSSPropertyBackgroundClip(StyleResolverState& state, CSSValue* value)
{
    FillLayer* currChild = &state.style()->accessBackgroundLayers();
    FillLayer* prevChild = 0;
    if (value->isValueList() && !value->isImageSetValue()) {
        /* Walk each value and put it into a layer, creating new layers as needed. */
        CSSValueList* valueList = toCSSValueList(value);
        for (unsigned int i = 0; i < valueList->length(); i++) {
            if (!currChild)
                currChild = prevChild->ensureNext();
            CSSToStyleMap::mapFillClip(state, currChild, valueList->item(i));
            prevChild = currChild;
            currChild = currChild->next();
        }
    } else {
        CSSToStyleMap::mapFillClip(state, currChild, value);
        currChild = currChild->next();
    }
    while (currChild) {
        /* Reset all remaining layers to not have the property set. */
        currChild->clearClip();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyInitialCSSPropertyBackgroundImage(StyleResolverState& state)
{
    FillLayer* currChild = &state.style()->accessBackgroundLayers();
    currChild->setImage(FillLayer::initialFillImage(BackgroundFillLayer));
    for (currChild = currChild->next(); currChild; currChild = currChild->next())
        currChild->clearImage();
}

void StyleBuilderFunctions::applyInheritCSSPropertyBackgroundImage(StyleResolverState& state)
{
    FillLayer* currChild = &state.style()->accessBackgroundLayers();
    FillLayer* prevChild = 0;
    const FillLayer* currParent = &state.parentStyle()->backgroundLayers();
    while (currParent && currParent->isImageSet()) {
        if (!currChild)
            currChild = prevChild->ensureNext();
        currChild->setImage(currParent->image());
        prevChild = currChild;
        currChild = prevChild->next();
        currParent = currParent->next();
    }

    while (currChild) {
        /* Reset any remaining layers to not have the property set. */
        currChild->clearImage();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyValueCSSPropertyBackgroundImage(StyleResolverState& state, CSSValue* value)
{
    FillLayer* currChild = &state.style()->accessBackgroundLayers();
    FillLayer* prevChild = 0;
    if (value->isValueList() && !value->isImageSetValue()) {
        /* Walk each value and put it into a layer, creating new layers as needed. */
        CSSValueList* valueList = toCSSValueList(value);
        for (unsigned int i = 0; i < valueList->length(); i++) {
            if (!currChild)
                currChild = prevChild->ensureNext();
            CSSToStyleMap::mapFillImage(state, currChild, valueList->item(i));
            prevChild = currChild;
            currChild = currChild->next();
        }
    } else {
        CSSToStyleMap::mapFillImage(state, currChild, value);
        currChild = currChild->next();
    }
    while (currChild) {
        /* Reset all remaining layers to not have the property set. */
        currChild->clearImage();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyInitialCSSPropertyBackgroundOrigin(StyleResolverState& state)
{
    FillLayer* currChild = &state.style()->accessBackgroundLayers();
    currChild->setOrigin(FillLayer::initialFillOrigin(BackgroundFillLayer));
    for (currChild = currChild->next(); currChild; currChild = currChild->next())
        currChild->clearOrigin();
}

void StyleBuilderFunctions::applyInheritCSSPropertyBackgroundOrigin(StyleResolverState& state)
{
    FillLayer* currChild = &state.style()->accessBackgroundLayers();
    FillLayer* prevChild = 0;
    const FillLayer* currParent = &state.parentStyle()->backgroundLayers();
    while (currParent && currParent->isOriginSet()) {
        if (!currChild)
            currChild = prevChild->ensureNext();
        currChild->setOrigin(currParent->origin());
        prevChild = currChild;
        currChild = prevChild->next();
        currParent = currParent->next();
    }

    while (currChild) {
        /* Reset any remaining layers to not have the property set. */
        currChild->clearOrigin();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyValueCSSPropertyBackgroundOrigin(StyleResolverState& state, CSSValue* value)
{
    FillLayer* currChild = &state.style()->accessBackgroundLayers();
    FillLayer* prevChild = 0;
    if (value->isValueList() && !value->isImageSetValue()) {
        /* Walk each value and put it into a layer, creating new layers as needed. */
        CSSValueList* valueList = toCSSValueList(value);
        for (unsigned int i = 0; i < valueList->length(); i++) {
            if (!currChild)
                currChild = prevChild->ensureNext();
            CSSToStyleMap::mapFillOrigin(state, currChild, valueList->item(i));
            prevChild = currChild;
            currChild = currChild->next();
        }
    } else {
        CSSToStyleMap::mapFillOrigin(state, currChild, value);
        currChild = currChild->next();
    }
    while (currChild) {
        /* Reset all remaining layers to not have the property set. */
        currChild->clearOrigin();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyInitialCSSPropertyBackgroundPositionX(StyleResolverState& state)
{
    FillLayer* currChild = &state.style()->accessBackgroundLayers();
    currChild->setXPosition(FillLayer::initialFillXPosition(BackgroundFillLayer));
    for (currChild = currChild->next(); currChild; currChild = currChild->next())
        currChild->clearXPosition();
}

void StyleBuilderFunctions::applyInheritCSSPropertyBackgroundPositionX(StyleResolverState& state)
{
    FillLayer* currChild = &state.style()->accessBackgroundLayers();
    FillLayer* prevChild = 0;
    const FillLayer* currParent = &state.parentStyle()->backgroundLayers();
    while (currParent && currParent->isXPositionSet()) {
        if (!currChild)
            currChild = prevChild->ensureNext();
        currChild->setXPosition(currParent->xPosition());
        prevChild = currChild;
        currChild = prevChild->next();
        currParent = currParent->next();
    }

    while (currChild) {
        /* Reset any remaining layers to not have the property set. */
        currChild->clearXPosition();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyValueCSSPropertyBackgroundPositionX(StyleResolverState& state, CSSValue* value)
{
    FillLayer* currChild = &state.style()->accessBackgroundLayers();
    FillLayer* prevChild = 0;
    if (value->isValueList() && !value->isImageSetValue()) {
        /* Walk each value and put it into a layer, creating new layers as needed. */
        CSSValueList* valueList = toCSSValueList(value);
        for (unsigned int i = 0; i < valueList->length(); i++) {
            if (!currChild)
                currChild = prevChild->ensureNext();
            CSSToStyleMap::mapFillXPosition(state, currChild, valueList->item(i));
            prevChild = currChild;
            currChild = currChild->next();
        }
    } else {
        CSSToStyleMap::mapFillXPosition(state, currChild, value);
        currChild = currChild->next();
    }
    while (currChild) {
        /* Reset all remaining layers to not have the property set. */
        currChild->clearXPosition();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyInitialCSSPropertyBackgroundPositionY(StyleResolverState& state)
{
    FillLayer* currChild = &state.style()->accessBackgroundLayers();
    currChild->setYPosition(FillLayer::initialFillYPosition(BackgroundFillLayer));
    for (currChild = currChild->next(); currChild; currChild = currChild->next())
        currChild->clearYPosition();
}

void StyleBuilderFunctions::applyInheritCSSPropertyBackgroundPositionY(StyleResolverState& state)
{
    FillLayer* currChild = &state.style()->accessBackgroundLayers();
    FillLayer* prevChild = 0;
    const FillLayer* currParent = &state.parentStyle()->backgroundLayers();
    while (currParent && currParent->isYPositionSet()) {
        if (!currChild)
            currChild = prevChild->ensureNext();
        currChild->setYPosition(currParent->yPosition());
        prevChild = currChild;
        currChild = prevChild->next();
        currParent = currParent->next();
    }

    while (currChild) {
        /* Reset any remaining layers to not have the property set. */
        currChild->clearYPosition();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyValueCSSPropertyBackgroundPositionY(StyleResolverState& state, CSSValue* value)
{
    FillLayer* currChild = &state.style()->accessBackgroundLayers();
    FillLayer* prevChild = 0;
    if (value->isValueList() && !value->isImageSetValue()) {
        /* Walk each value and put it into a layer, creating new layers as needed. */
        CSSValueList* valueList = toCSSValueList(value);
        for (unsigned int i = 0; i < valueList->length(); i++) {
            if (!currChild)
                currChild = prevChild->ensureNext();
            CSSToStyleMap::mapFillYPosition(state, currChild, valueList->item(i));
            prevChild = currChild;
            currChild = currChild->next();
        }
    } else {
        CSSToStyleMap::mapFillYPosition(state, currChild, value);
        currChild = currChild->next();
    }
    while (currChild) {
        /* Reset all remaining layers to not have the property set. */
        currChild->clearYPosition();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyInitialCSSPropertyBackgroundRepeatX(StyleResolverState& state)
{
    FillLayer* currChild = &state.style()->accessBackgroundLayers();
    currChild->setRepeatX(FillLayer::initialFillRepeatX(BackgroundFillLayer));
    for (currChild = currChild->next(); currChild; currChild = currChild->next())
        currChild->clearRepeatX();
}

void StyleBuilderFunctions::applyInheritCSSPropertyBackgroundRepeatX(StyleResolverState& state)
{
    FillLayer* currChild = &state.style()->accessBackgroundLayers();
    FillLayer* prevChild = 0;
    const FillLayer* currParent = &state.parentStyle()->backgroundLayers();
    while (currParent && currParent->isRepeatXSet()) {
        if (!currChild)
            currChild = prevChild->ensureNext();
        currChild->setRepeatX(currParent->repeatX());
        prevChild = currChild;
        currChild = prevChild->next();
        currParent = currParent->next();
    }

    while (currChild) {
        /* Reset any remaining layers to not have the property set. */
        currChild->clearRepeatX();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyValueCSSPropertyBackgroundRepeatX(StyleResolverState& state, CSSValue* value)
{
    FillLayer* currChild = &state.style()->accessBackgroundLayers();
    FillLayer* prevChild = 0;
    if (value->isValueList() && !value->isImageSetValue()) {
        /* Walk each value and put it into a layer, creating new layers as needed. */
        CSSValueList* valueList = toCSSValueList(value);
        for (unsigned int i = 0; i < valueList->length(); i++) {
            if (!currChild)
                currChild = prevChild->ensureNext();
            CSSToStyleMap::mapFillRepeatX(state, currChild, valueList->item(i));
            prevChild = currChild;
            currChild = currChild->next();
        }
    } else {
        CSSToStyleMap::mapFillRepeatX(state, currChild, value);
        currChild = currChild->next();
    }
    while (currChild) {
        /* Reset all remaining layers to not have the property set. */
        currChild->clearRepeatX();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyInitialCSSPropertyBackgroundRepeatY(StyleResolverState& state)
{
    FillLayer* currChild = &state.style()->accessBackgroundLayers();
    currChild->setRepeatY(FillLayer::initialFillRepeatY(BackgroundFillLayer));
    for (currChild = currChild->next(); currChild; currChild = currChild->next())
        currChild->clearRepeatY();
}

void StyleBuilderFunctions::applyInheritCSSPropertyBackgroundRepeatY(StyleResolverState& state)
{
    FillLayer* currChild = &state.style()->accessBackgroundLayers();
    FillLayer* prevChild = 0;
    const FillLayer* currParent = &state.parentStyle()->backgroundLayers();
    while (currParent && currParent->isRepeatYSet()) {
        if (!currChild)
            currChild = prevChild->ensureNext();
        currChild->setRepeatY(currParent->repeatY());
        prevChild = currChild;
        currChild = prevChild->next();
        currParent = currParent->next();
    }

    while (currChild) {
        /* Reset any remaining layers to not have the property set. */
        currChild->clearRepeatY();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyValueCSSPropertyBackgroundRepeatY(StyleResolverState& state, CSSValue* value)
{
    FillLayer* currChild = &state.style()->accessBackgroundLayers();
    FillLayer* prevChild = 0;
    if (value->isValueList() && !value->isImageSetValue()) {
        /* Walk each value and put it into a layer, creating new layers as needed. */
        CSSValueList* valueList = toCSSValueList(value);
        for (unsigned int i = 0; i < valueList->length(); i++) {
            if (!currChild)
                currChild = prevChild->ensureNext();
            CSSToStyleMap::mapFillRepeatY(state, currChild, valueList->item(i));
            prevChild = currChild;
            currChild = currChild->next();
        }
    } else {
        CSSToStyleMap::mapFillRepeatY(state, currChild, value);
        currChild = currChild->next();
    }
    while (currChild) {
        /* Reset all remaining layers to not have the property set. */
        currChild->clearRepeatY();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyInitialCSSPropertyBackgroundSize(StyleResolverState& state)
{
    FillLayer* currChild = &state.style()->accessBackgroundLayers();
    currChild->setSize(FillLayer::initialFillSize(BackgroundFillLayer));
    for (currChild = currChild->next(); currChild; currChild = currChild->next())
        currChild->clearSize();
}

void StyleBuilderFunctions::applyInheritCSSPropertyBackgroundSize(StyleResolverState& state)
{
    FillLayer* currChild = &state.style()->accessBackgroundLayers();
    FillLayer* prevChild = 0;
    const FillLayer* currParent = &state.parentStyle()->backgroundLayers();
    while (currParent && currParent->isSizeSet()) {
        if (!currChild)
            currChild = prevChild->ensureNext();
        currChild->setSize(currParent->size());
        prevChild = currChild;
        currChild = prevChild->next();
        currParent = currParent->next();
    }

    while (currChild) {
        /* Reset any remaining layers to not have the property set. */
        currChild->clearSize();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyValueCSSPropertyBackgroundSize(StyleResolverState& state, CSSValue* value)
{
    FillLayer* currChild = &state.style()->accessBackgroundLayers();
    FillLayer* prevChild = 0;
    if (value->isValueList() && !value->isImageSetValue()) {
        /* Walk each value and put it into a layer, creating new layers as needed. */
        CSSValueList* valueList = toCSSValueList(value);
        for (unsigned int i = 0; i < valueList->length(); i++) {
            if (!currChild)
                currChild = prevChild->ensureNext();
            CSSToStyleMap::mapFillSize(state, currChild, valueList->item(i));
            prevChild = currChild;
            currChild = currChild->next();
        }
    } else {
        CSSToStyleMap::mapFillSize(state, currChild, value);
        currChild = currChild->next();
    }
    while (currChild) {
        /* Reset all remaining layers to not have the property set. */
        currChild->clearSize();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyInitialCSSPropertyMaskSourceType(StyleResolverState& state)
{
    FillLayer* currChild = &state.style()->accessMaskLayers();
    currChild->setMaskSourceType(FillLayer::initialFillMaskSourceType(MaskFillLayer));
    for (currChild = currChild->next(); currChild; currChild = currChild->next())
        currChild->clearMaskSourceType();
}

void StyleBuilderFunctions::applyInheritCSSPropertyMaskSourceType(StyleResolverState& state)
{
    FillLayer* currChild = &state.style()->accessMaskLayers();
    FillLayer* prevChild = 0;
    const FillLayer* currParent = &state.parentStyle()->maskLayers();
    while (currParent && currParent->isMaskSourceTypeSet()) {
        if (!currChild)
            currChild = prevChild->ensureNext();
        currChild->setMaskSourceType(currParent->maskSourceType());
        prevChild = currChild;
        currChild = prevChild->next();
        currParent = currParent->next();
    }

    while (currChild) {
        /* Reset any remaining layers to not have the property set. */
        currChild->clearMaskSourceType();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyValueCSSPropertyMaskSourceType(StyleResolverState& state, CSSValue* value)
{
    FillLayer* currChild = &state.style()->accessMaskLayers();
    FillLayer* prevChild = 0;
    if (value->isValueList() && !value->isImageSetValue()) {
        /* Walk each value and put it into a layer, creating new layers as needed. */
        CSSValueList* valueList = toCSSValueList(value);
        for (unsigned int i = 0; i < valueList->length(); i++) {
            if (!currChild)
                currChild = prevChild->ensureNext();
            CSSToStyleMap::mapFillMaskSourceType(state, currChild, valueList->item(i));
            prevChild = currChild;
            currChild = currChild->next();
        }
    } else {
        CSSToStyleMap::mapFillMaskSourceType(state, currChild, value);
        currChild = currChild->next();
    }
    while (currChild) {
        /* Reset all remaining layers to not have the property set. */
        currChild->clearMaskSourceType();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitBackgroundComposite(StyleResolverState& state)
{
    FillLayer* currChild = &state.style()->accessBackgroundLayers();
    currChild->setComposite(FillLayer::initialFillComposite(BackgroundFillLayer));
    for (currChild = currChild->next(); currChild; currChild = currChild->next())
        currChild->clearComposite();
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitBackgroundComposite(StyleResolverState& state)
{
    FillLayer* currChild = &state.style()->accessBackgroundLayers();
    FillLayer* prevChild = 0;
    const FillLayer* currParent = &state.parentStyle()->backgroundLayers();
    while (currParent && currParent->isCompositeSet()) {
        if (!currChild)
            currChild = prevChild->ensureNext();
        currChild->setComposite(currParent->composite());
        prevChild = currChild;
        currChild = prevChild->next();
        currParent = currParent->next();
    }

    while (currChild) {
        /* Reset any remaining layers to not have the property set. */
        currChild->clearComposite();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitBackgroundComposite(StyleResolverState& state, CSSValue* value)
{
    FillLayer* currChild = &state.style()->accessBackgroundLayers();
    FillLayer* prevChild = 0;
    if (value->isValueList() && !value->isImageSetValue()) {
        /* Walk each value and put it into a layer, creating new layers as needed. */
        CSSValueList* valueList = toCSSValueList(value);
        for (unsigned int i = 0; i < valueList->length(); i++) {
            if (!currChild)
                currChild = prevChild->ensureNext();
            CSSToStyleMap::mapFillComposite(state, currChild, valueList->item(i));
            prevChild = currChild;
            currChild = currChild->next();
        }
    } else {
        CSSToStyleMap::mapFillComposite(state, currChild, value);
        currChild = currChild->next();
    }
    while (currChild) {
        /* Reset all remaining layers to not have the property set. */
        currChild->clearComposite();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitMaskClip(StyleResolverState& state)
{
    FillLayer* currChild = &state.style()->accessMaskLayers();
    currChild->setClip(FillLayer::initialFillClip(MaskFillLayer));
    for (currChild = currChild->next(); currChild; currChild = currChild->next())
        currChild->clearClip();
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitMaskClip(StyleResolverState& state)
{
    FillLayer* currChild = &state.style()->accessMaskLayers();
    FillLayer* prevChild = 0;
    const FillLayer* currParent = &state.parentStyle()->maskLayers();
    while (currParent && currParent->isClipSet()) {
        if (!currChild)
            currChild = prevChild->ensureNext();
        currChild->setClip(currParent->clip());
        prevChild = currChild;
        currChild = prevChild->next();
        currParent = currParent->next();
    }

    while (currChild) {
        /* Reset any remaining layers to not have the property set. */
        currChild->clearClip();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitMaskClip(StyleResolverState& state, CSSValue* value)
{
    FillLayer* currChild = &state.style()->accessMaskLayers();
    FillLayer* prevChild = 0;
    if (value->isValueList() && !value->isImageSetValue()) {
        /* Walk each value and put it into a layer, creating new layers as needed. */
        CSSValueList* valueList = toCSSValueList(value);
        for (unsigned int i = 0; i < valueList->length(); i++) {
            if (!currChild)
                currChild = prevChild->ensureNext();
            CSSToStyleMap::mapFillClip(state, currChild, valueList->item(i));
            prevChild = currChild;
            currChild = currChild->next();
        }
    } else {
        CSSToStyleMap::mapFillClip(state, currChild, value);
        currChild = currChild->next();
    }
    while (currChild) {
        /* Reset all remaining layers to not have the property set. */
        currChild->clearClip();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitMaskComposite(StyleResolverState& state)
{
    FillLayer* currChild = &state.style()->accessMaskLayers();
    currChild->setComposite(FillLayer::initialFillComposite(MaskFillLayer));
    for (currChild = currChild->next(); currChild; currChild = currChild->next())
        currChild->clearComposite();
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitMaskComposite(StyleResolverState& state)
{
    FillLayer* currChild = &state.style()->accessMaskLayers();
    FillLayer* prevChild = 0;
    const FillLayer* currParent = &state.parentStyle()->maskLayers();
    while (currParent && currParent->isCompositeSet()) {
        if (!currChild)
            currChild = prevChild->ensureNext();
        currChild->setComposite(currParent->composite());
        prevChild = currChild;
        currChild = prevChild->next();
        currParent = currParent->next();
    }

    while (currChild) {
        /* Reset any remaining layers to not have the property set. */
        currChild->clearComposite();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitMaskComposite(StyleResolverState& state, CSSValue* value)
{
    FillLayer* currChild = &state.style()->accessMaskLayers();
    FillLayer* prevChild = 0;
    if (value->isValueList() && !value->isImageSetValue()) {
        /* Walk each value and put it into a layer, creating new layers as needed. */
        CSSValueList* valueList = toCSSValueList(value);
        for (unsigned int i = 0; i < valueList->length(); i++) {
            if (!currChild)
                currChild = prevChild->ensureNext();
            CSSToStyleMap::mapFillComposite(state, currChild, valueList->item(i));
            prevChild = currChild;
            currChild = currChild->next();
        }
    } else {
        CSSToStyleMap::mapFillComposite(state, currChild, value);
        currChild = currChild->next();
    }
    while (currChild) {
        /* Reset all remaining layers to not have the property set. */
        currChild->clearComposite();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitMaskImage(StyleResolverState& state)
{
    FillLayer* currChild = &state.style()->accessMaskLayers();
    currChild->setImage(FillLayer::initialFillImage(MaskFillLayer));
    for (currChild = currChild->next(); currChild; currChild = currChild->next())
        currChild->clearImage();
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitMaskImage(StyleResolverState& state)
{
    FillLayer* currChild = &state.style()->accessMaskLayers();
    FillLayer* prevChild = 0;
    const FillLayer* currParent = &state.parentStyle()->maskLayers();
    while (currParent && currParent->isImageSet()) {
        if (!currChild)
            currChild = prevChild->ensureNext();
        currChild->setImage(currParent->image());
        prevChild = currChild;
        currChild = prevChild->next();
        currParent = currParent->next();
    }

    while (currChild) {
        /* Reset any remaining layers to not have the property set. */
        currChild->clearImage();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitMaskImage(StyleResolverState& state, CSSValue* value)
{
    FillLayer* currChild = &state.style()->accessMaskLayers();
    FillLayer* prevChild = 0;
    if (value->isValueList() && !value->isImageSetValue()) {
        /* Walk each value and put it into a layer, creating new layers as needed. */
        CSSValueList* valueList = toCSSValueList(value);
        for (unsigned int i = 0; i < valueList->length(); i++) {
            if (!currChild)
                currChild = prevChild->ensureNext();
            CSSToStyleMap::mapFillImage(state, currChild, valueList->item(i));
            prevChild = currChild;
            currChild = currChild->next();
        }
    } else {
        CSSToStyleMap::mapFillImage(state, currChild, value);
        currChild = currChild->next();
    }
    while (currChild) {
        /* Reset all remaining layers to not have the property set. */
        currChild->clearImage();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitMaskOrigin(StyleResolverState& state)
{
    FillLayer* currChild = &state.style()->accessMaskLayers();
    currChild->setOrigin(FillLayer::initialFillOrigin(MaskFillLayer));
    for (currChild = currChild->next(); currChild; currChild = currChild->next())
        currChild->clearOrigin();
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitMaskOrigin(StyleResolverState& state)
{
    FillLayer* currChild = &state.style()->accessMaskLayers();
    FillLayer* prevChild = 0;
    const FillLayer* currParent = &state.parentStyle()->maskLayers();
    while (currParent && currParent->isOriginSet()) {
        if (!currChild)
            currChild = prevChild->ensureNext();
        currChild->setOrigin(currParent->origin());
        prevChild = currChild;
        currChild = prevChild->next();
        currParent = currParent->next();
    }

    while (currChild) {
        /* Reset any remaining layers to not have the property set. */
        currChild->clearOrigin();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitMaskOrigin(StyleResolverState& state, CSSValue* value)
{
    FillLayer* currChild = &state.style()->accessMaskLayers();
    FillLayer* prevChild = 0;
    if (value->isValueList() && !value->isImageSetValue()) {
        /* Walk each value and put it into a layer, creating new layers as needed. */
        CSSValueList* valueList = toCSSValueList(value);
        for (unsigned int i = 0; i < valueList->length(); i++) {
            if (!currChild)
                currChild = prevChild->ensureNext();
            CSSToStyleMap::mapFillOrigin(state, currChild, valueList->item(i));
            prevChild = currChild;
            currChild = currChild->next();
        }
    } else {
        CSSToStyleMap::mapFillOrigin(state, currChild, value);
        currChild = currChild->next();
    }
    while (currChild) {
        /* Reset all remaining layers to not have the property set. */
        currChild->clearOrigin();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitMaskPositionX(StyleResolverState& state)
{
    FillLayer* currChild = &state.style()->accessMaskLayers();
    currChild->setXPosition(FillLayer::initialFillXPosition(MaskFillLayer));
    for (currChild = currChild->next(); currChild; currChild = currChild->next())
        currChild->clearXPosition();
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitMaskPositionX(StyleResolverState& state)
{
    FillLayer* currChild = &state.style()->accessMaskLayers();
    FillLayer* prevChild = 0;
    const FillLayer* currParent = &state.parentStyle()->maskLayers();
    while (currParent && currParent->isXPositionSet()) {
        if (!currChild)
            currChild = prevChild->ensureNext();
        currChild->setXPosition(currParent->xPosition());
        prevChild = currChild;
        currChild = prevChild->next();
        currParent = currParent->next();
    }

    while (currChild) {
        /* Reset any remaining layers to not have the property set. */
        currChild->clearXPosition();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitMaskPositionX(StyleResolverState& state, CSSValue* value)
{
    FillLayer* currChild = &state.style()->accessMaskLayers();
    FillLayer* prevChild = 0;
    if (value->isValueList() && !value->isImageSetValue()) {
        /* Walk each value and put it into a layer, creating new layers as needed. */
        CSSValueList* valueList = toCSSValueList(value);
        for (unsigned int i = 0; i < valueList->length(); i++) {
            if (!currChild)
                currChild = prevChild->ensureNext();
            CSSToStyleMap::mapFillXPosition(state, currChild, valueList->item(i));
            prevChild = currChild;
            currChild = currChild->next();
        }
    } else {
        CSSToStyleMap::mapFillXPosition(state, currChild, value);
        currChild = currChild->next();
    }
    while (currChild) {
        /* Reset all remaining layers to not have the property set. */
        currChild->clearXPosition();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitMaskPositionY(StyleResolverState& state)
{
    FillLayer* currChild = &state.style()->accessMaskLayers();
    currChild->setYPosition(FillLayer::initialFillYPosition(MaskFillLayer));
    for (currChild = currChild->next(); currChild; currChild = currChild->next())
        currChild->clearYPosition();
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitMaskPositionY(StyleResolverState& state)
{
    FillLayer* currChild = &state.style()->accessMaskLayers();
    FillLayer* prevChild = 0;
    const FillLayer* currParent = &state.parentStyle()->maskLayers();
    while (currParent && currParent->isYPositionSet()) {
        if (!currChild)
            currChild = prevChild->ensureNext();
        currChild->setYPosition(currParent->yPosition());
        prevChild = currChild;
        currChild = prevChild->next();
        currParent = currParent->next();
    }

    while (currChild) {
        /* Reset any remaining layers to not have the property set. */
        currChild->clearYPosition();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitMaskPositionY(StyleResolverState& state, CSSValue* value)
{
    FillLayer* currChild = &state.style()->accessMaskLayers();
    FillLayer* prevChild = 0;
    if (value->isValueList() && !value->isImageSetValue()) {
        /* Walk each value and put it into a layer, creating new layers as needed. */
        CSSValueList* valueList = toCSSValueList(value);
        for (unsigned int i = 0; i < valueList->length(); i++) {
            if (!currChild)
                currChild = prevChild->ensureNext();
            CSSToStyleMap::mapFillYPosition(state, currChild, valueList->item(i));
            prevChild = currChild;
            currChild = currChild->next();
        }
    } else {
        CSSToStyleMap::mapFillYPosition(state, currChild, value);
        currChild = currChild->next();
    }
    while (currChild) {
        /* Reset all remaining layers to not have the property set. */
        currChild->clearYPosition();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitMaskRepeatX(StyleResolverState& state)
{
    FillLayer* currChild = &state.style()->accessMaskLayers();
    currChild->setRepeatX(FillLayer::initialFillRepeatX(MaskFillLayer));
    for (currChild = currChild->next(); currChild; currChild = currChild->next())
        currChild->clearRepeatX();
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitMaskRepeatX(StyleResolverState& state)
{
    FillLayer* currChild = &state.style()->accessMaskLayers();
    FillLayer* prevChild = 0;
    const FillLayer* currParent = &state.parentStyle()->maskLayers();
    while (currParent && currParent->isRepeatXSet()) {
        if (!currChild)
            currChild = prevChild->ensureNext();
        currChild->setRepeatX(currParent->repeatX());
        prevChild = currChild;
        currChild = prevChild->next();
        currParent = currParent->next();
    }

    while (currChild) {
        /* Reset any remaining layers to not have the property set. */
        currChild->clearRepeatX();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitMaskRepeatX(StyleResolverState& state, CSSValue* value)
{
    FillLayer* currChild = &state.style()->accessMaskLayers();
    FillLayer* prevChild = 0;
    if (value->isValueList() && !value->isImageSetValue()) {
        /* Walk each value and put it into a layer, creating new layers as needed. */
        CSSValueList* valueList = toCSSValueList(value);
        for (unsigned int i = 0; i < valueList->length(); i++) {
            if (!currChild)
                currChild = prevChild->ensureNext();
            CSSToStyleMap::mapFillRepeatX(state, currChild, valueList->item(i));
            prevChild = currChild;
            currChild = currChild->next();
        }
    } else {
        CSSToStyleMap::mapFillRepeatX(state, currChild, value);
        currChild = currChild->next();
    }
    while (currChild) {
        /* Reset all remaining layers to not have the property set. */
        currChild->clearRepeatX();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitMaskRepeatY(StyleResolverState& state)
{
    FillLayer* currChild = &state.style()->accessMaskLayers();
    currChild->setRepeatY(FillLayer::initialFillRepeatY(MaskFillLayer));
    for (currChild = currChild->next(); currChild; currChild = currChild->next())
        currChild->clearRepeatY();
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitMaskRepeatY(StyleResolverState& state)
{
    FillLayer* currChild = &state.style()->accessMaskLayers();
    FillLayer* prevChild = 0;
    const FillLayer* currParent = &state.parentStyle()->maskLayers();
    while (currParent && currParent->isRepeatYSet()) {
        if (!currChild)
            currChild = prevChild->ensureNext();
        currChild->setRepeatY(currParent->repeatY());
        prevChild = currChild;
        currChild = prevChild->next();
        currParent = currParent->next();
    }

    while (currChild) {
        /* Reset any remaining layers to not have the property set. */
        currChild->clearRepeatY();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitMaskRepeatY(StyleResolverState& state, CSSValue* value)
{
    FillLayer* currChild = &state.style()->accessMaskLayers();
    FillLayer* prevChild = 0;
    if (value->isValueList() && !value->isImageSetValue()) {
        /* Walk each value and put it into a layer, creating new layers as needed. */
        CSSValueList* valueList = toCSSValueList(value);
        for (unsigned int i = 0; i < valueList->length(); i++) {
            if (!currChild)
                currChild = prevChild->ensureNext();
            CSSToStyleMap::mapFillRepeatY(state, currChild, valueList->item(i));
            prevChild = currChild;
            currChild = currChild->next();
        }
    } else {
        CSSToStyleMap::mapFillRepeatY(state, currChild, value);
        currChild = currChild->next();
    }
    while (currChild) {
        /* Reset all remaining layers to not have the property set. */
        currChild->clearRepeatY();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyInitialCSSPropertyWebkitMaskSize(StyleResolverState& state)
{
    FillLayer* currChild = &state.style()->accessMaskLayers();
    currChild->setSize(FillLayer::initialFillSize(MaskFillLayer));
    for (currChild = currChild->next(); currChild; currChild = currChild->next())
        currChild->clearSize();
}

void StyleBuilderFunctions::applyInheritCSSPropertyWebkitMaskSize(StyleResolverState& state)
{
    FillLayer* currChild = &state.style()->accessMaskLayers();
    FillLayer* prevChild = 0;
    const FillLayer* currParent = &state.parentStyle()->maskLayers();
    while (currParent && currParent->isSizeSet()) {
        if (!currChild)
            currChild = prevChild->ensureNext();
        currChild->setSize(currParent->size());
        prevChild = currChild;
        currChild = prevChild->next();
        currParent = currParent->next();
    }

    while (currChild) {
        /* Reset any remaining layers to not have the property set. */
        currChild->clearSize();
        currChild = currChild->next();
    }
}

void StyleBuilderFunctions::applyValueCSSPropertyWebkitMaskSize(StyleResolverState& state, CSSValue* value)
{
    FillLayer* currChild = &state.style()->accessMaskLayers();
    FillLayer* prevChild = 0;
    if (value->isValueList() && !value->isImageSetValue()) {
        /* Walk each value and put it into a layer, creating new layers as needed. */
        CSSValueList* valueList = toCSSValueList(value);
        for (unsigned int i = 0; i < valueList->length(); i++) {
            if (!currChild)
                currChild = prevChild->ensureNext();
            CSSToStyleMap::mapFillSize(state, currChild, valueList->item(i));
            prevChild = currChild;
            currChild = currChild->next();
        }
    } else {
        CSSToStyleMap::mapFillSize(state, currChild, value);
        currChild = currChild->next();
    }
    while (currChild) {
        /* Reset all remaining layers to not have the property set. */
        currChild->clearSize();
        currChild = currChild->next();
    }
}


void StyleBuilderFunctions::applyInitialCSSPropertyGridTemplateColumns(StyleResolverState& state)
{
    state.style()->setGridTemplateColumns(ComputedStyle::initialGridTemplateColumns());
    state.style()->setNamedGridColumnLines(ComputedStyle::initialNamedGridColumnLines());
    state.style()->setOrderedNamedGridColumnLines(ComputedStyle::initialOrderedNamedGridColumnLines());
}

void StyleBuilderFunctions::applyInheritCSSPropertyGridTemplateColumns(StyleResolverState& state)
{
    state.style()->setGridTemplateColumns(state.parentStyle()->gridTemplateColumns());
    state.style()->setNamedGridColumnLines(state.parentStyle()->namedGridColumnLines());
    state.style()->setOrderedNamedGridColumnLines(state.parentStyle()->orderedNamedGridColumnLines());
}

void StyleBuilderFunctions::applyValueCSSPropertyGridTemplateColumns(StyleResolverState& state, CSSValue* value)
{
    Vector<GridTrackSize> trackSizes;
    NamedGridLinesMap namedGridLines;
    OrderedNamedGridLines orderedNamedGridLines;
    if (!StyleBuilderConverter::convertGridTrackList(value, trackSizes, namedGridLines, orderedNamedGridLines, state))
        return;
    const NamedGridAreaMap& namedGridAreas = state.style()->namedGridArea();
    if (!namedGridAreas.isEmpty())
        StyleBuilderConverter::createImplicitNamedGridLinesFromGridArea(namedGridAreas, namedGridLines, ForColumns);
    state.style()->setGridTemplateColumns(trackSizes);
    state.style()->setNamedGridColumnLines(namedGridLines);
    state.style()->setOrderedNamedGridColumnLines(orderedNamedGridLines);
}

void StyleBuilderFunctions::applyInitialCSSPropertyGridTemplateRows(StyleResolverState& state)
{
    state.style()->setGridTemplateRows(ComputedStyle::initialGridTemplateRows());
    state.style()->setNamedGridRowLines(ComputedStyle::initialNamedGridRowLines());
    state.style()->setOrderedNamedGridRowLines(ComputedStyle::initialOrderedNamedGridRowLines());
}

void StyleBuilderFunctions::applyInheritCSSPropertyGridTemplateRows(StyleResolverState& state)
{
    state.style()->setGridTemplateRows(state.parentStyle()->gridTemplateRows());
    state.style()->setNamedGridRowLines(state.parentStyle()->namedGridRowLines());
    state.style()->setOrderedNamedGridRowLines(state.parentStyle()->orderedNamedGridRowLines());
}

void StyleBuilderFunctions::applyValueCSSPropertyGridTemplateRows(StyleResolverState& state, CSSValue* value)
{
    Vector<GridTrackSize> trackSizes;
    NamedGridLinesMap namedGridLines;
    OrderedNamedGridLines orderedNamedGridLines;
    if (!StyleBuilderConverter::convertGridTrackList(value, trackSizes, namedGridLines, orderedNamedGridLines, state))
        return;
    const NamedGridAreaMap& namedGridAreas = state.style()->namedGridArea();
    if (!namedGridAreas.isEmpty())
        StyleBuilderConverter::createImplicitNamedGridLinesFromGridArea(namedGridAreas, namedGridLines, ForRows);
    state.style()->setGridTemplateRows(trackSizes);
    state.style()->setNamedGridRowLines(namedGridLines);
    state.style()->setOrderedNamedGridRowLines(orderedNamedGridLines);
}


void StyleBuilderFunctions::applyInitialCSSPropertyFill(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setFillPaint(
        SVGComputedStyle::initialFillPaintType(),
        SVGComputedStyle::initialFillPaintColor(),
        SVGComputedStyle::initialFillPaintUri(),
        state.applyPropertyToRegularStyle(),
        state.applyPropertyToVisitedLinkStyle());
}

void StyleBuilderFunctions::applyInheritCSSPropertyFill(StyleResolverState& state)
{
    const SVGComputedStyle& svgParentStyle = state.parentStyle()->svgStyle();
    state.style()->accessSVGStyle().setFillPaint(
        svgParentStyle.fillPaintType(),
        svgParentStyle.fillPaintColor(),
        svgParentStyle.fillPaintUri(),
        state.applyPropertyToRegularStyle(),
        state.applyPropertyToVisitedLinkStyle());
}

void StyleBuilderFunctions::applyValueCSSPropertyFill(StyleResolverState& state, CSSValue* value)
{
    String url;
    if (value->isValueList()) {
        CSSValueList* list = toCSSValueList(value);
        ASSERT(list->length() > 1);

        if (!list->item(0)->isPrimitiveValue())
            return;

        CSSPrimitiveValue* primitiveValue = toCSSPrimitiveValue(list->item(0));
        if (!primitiveValue->isURI())
            return;

        url = primitiveValue->getStringValue();
        value = list->item(1);
    }
    if (value->isPrimitiveValue()) {
        CSSPrimitiveValue* primitiveValue = toCSSPrimitiveValue(value);
        Color color;
        SVGPaintType paintType = SVG_PAINTTYPE_RGBCOLOR;
        if (primitiveValue->getValueID() == CSSValueNone) {
            paintType = url.isEmpty() ? SVG_PAINTTYPE_NONE : SVG_PAINTTYPE_URI_NONE;
        } else if (primitiveValue->isURI()) {
            paintType = SVG_PAINTTYPE_URI;
            url = primitiveValue->getStringValue();
        } else if (primitiveValue->getValueID() == CSSValueCurrentcolor) {
            color = state.style()->color();
            paintType = url.isEmpty() ? SVG_PAINTTYPE_CURRENTCOLOR : SVG_PAINTTYPE_URI_CURRENTCOLOR;
        } else {
            color = StyleBuilderConverter::convertColor(state, primitiveValue);
            paintType = url.isEmpty() ? SVG_PAINTTYPE_RGBCOLOR : SVG_PAINTTYPE_URI_RGBCOLOR;
        }
        state.style()->accessSVGStyle().setFillPaint(paintType, color, url,
            state.applyPropertyToRegularStyle(),
            state.applyPropertyToVisitedLinkStyle());
    }
}

void StyleBuilderFunctions::applyInitialCSSPropertyStroke(StyleResolverState& state)
{
    state.style()->accessSVGStyle().setStrokePaint(
        SVGComputedStyle::initialStrokePaintType(),
        SVGComputedStyle::initialStrokePaintColor(),
        SVGComputedStyle::initialStrokePaintUri(),
        state.applyPropertyToRegularStyle(),
        state.applyPropertyToVisitedLinkStyle());
}

void StyleBuilderFunctions::applyInheritCSSPropertyStroke(StyleResolverState& state)
{
    const SVGComputedStyle& svgParentStyle = state.parentStyle()->svgStyle();
    state.style()->accessSVGStyle().setStrokePaint(
        svgParentStyle.strokePaintType(),
        svgParentStyle.strokePaintColor(),
        svgParentStyle.strokePaintUri(),
        state.applyPropertyToRegularStyle(),
        state.applyPropertyToVisitedLinkStyle());
}

void StyleBuilderFunctions::applyValueCSSPropertyStroke(StyleResolverState& state, CSSValue* value)
{
    String url;
    if (value->isValueList()) {
        CSSValueList* list = toCSSValueList(value);
        ASSERT(list->length() > 1);

        if (!list->item(0)->isPrimitiveValue())
            return;

        CSSPrimitiveValue* primitiveValue = toCSSPrimitiveValue(list->item(0));
        if (!primitiveValue->isURI())
            return;

        url = primitiveValue->getStringValue();
        value = list->item(1);
    }
    if (value->isPrimitiveValue()) {
        CSSPrimitiveValue* primitiveValue = toCSSPrimitiveValue(value);
        Color color;
        SVGPaintType paintType = SVG_PAINTTYPE_RGBCOLOR;
        if (primitiveValue->getValueID() == CSSValueNone) {
            paintType = url.isEmpty() ? SVG_PAINTTYPE_NONE : SVG_PAINTTYPE_URI_NONE;
        } else if (primitiveValue->isURI()) {
            paintType = SVG_PAINTTYPE_URI;
            url = primitiveValue->getStringValue();
        } else if (primitiveValue->getValueID() == CSSValueCurrentcolor) {
            color = state.style()->color();
            paintType = url.isEmpty() ? SVG_PAINTTYPE_CURRENTCOLOR : SVG_PAINTTYPE_URI_CURRENTCOLOR;
        } else {
            color = StyleBuilderConverter::convertColor(state, primitiveValue);
            paintType = url.isEmpty() ? SVG_PAINTTYPE_RGBCOLOR : SVG_PAINTTYPE_URI_RGBCOLOR;
        }
        state.style()->accessSVGStyle().setStrokePaint(paintType, color, url,
            state.applyPropertyToRegularStyle(),
            state.applyPropertyToVisitedLinkStyle());
    }
}

} // namespace blink
