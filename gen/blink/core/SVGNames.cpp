// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#include "SVGNames.h"

#include "wtf/StaticConstructors.h"
#include "wtf/StdLibExtras.h"

namespace blink {
namespace SVGNames {

using namespace blink;

DEFINE_GLOBAL(AtomicString, svgNamespaceURI)

// Tags

void* TagStorage[SVGTagsCount * ((sizeof(SVGQualifiedName) + sizeof(void *) - 1) / sizeof(void *))];
const SVGQualifiedName& aTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[0];
const SVGQualifiedName& animateTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[1];
const SVGQualifiedName& animateColorTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[2];
const SVGQualifiedName& animateMotionTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[3];
const SVGQualifiedName& animateTransformTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[4];
const SVGQualifiedName& circleTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[5];
const SVGQualifiedName& clipPathTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[6];
const SVGQualifiedName& cursorTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[7];
const SVGQualifiedName& defsTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[8];
const SVGQualifiedName& descTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[9];
const SVGQualifiedName& discardTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[10];
const SVGQualifiedName& ellipseTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[11];
const SVGQualifiedName& feBlendTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[12];
const SVGQualifiedName& feColorMatrixTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[13];
const SVGQualifiedName& feComponentTransferTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[14];
const SVGQualifiedName& feCompositeTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[15];
const SVGQualifiedName& feConvolveMatrixTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[16];
const SVGQualifiedName& feDiffuseLightingTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[17];
const SVGQualifiedName& feDisplacementMapTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[18];
const SVGQualifiedName& feDistantLightTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[19];
const SVGQualifiedName& feDropShadowTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[20];
const SVGQualifiedName& feFloodTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[21];
const SVGQualifiedName& feFuncATag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[22];
const SVGQualifiedName& feFuncBTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[23];
const SVGQualifiedName& feFuncGTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[24];
const SVGQualifiedName& feFuncRTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[25];
const SVGQualifiedName& feGaussianBlurTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[26];
const SVGQualifiedName& feImageTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[27];
const SVGQualifiedName& feMergeTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[28];
const SVGQualifiedName& feMergeNodeTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[29];
const SVGQualifiedName& feMorphologyTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[30];
const SVGQualifiedName& feOffsetTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[31];
const SVGQualifiedName& fePointLightTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[32];
const SVGQualifiedName& feSpecularLightingTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[33];
const SVGQualifiedName& feSpotLightTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[34];
const SVGQualifiedName& feTileTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[35];
const SVGQualifiedName& feTurbulenceTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[36];
const SVGQualifiedName& filterTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[37];
const SVGQualifiedName& foreignObjectTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[38];
const SVGQualifiedName& gTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[39];
const SVGQualifiedName& imageTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[40];
const SVGQualifiedName& lineTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[41];
const SVGQualifiedName& linearGradientTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[42];
const SVGQualifiedName& markerTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[43];
const SVGQualifiedName& maskTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[44];
const SVGQualifiedName& metadataTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[45];
const SVGQualifiedName& mpathTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[46];
const SVGQualifiedName& pathTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[47];
const SVGQualifiedName& patternTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[48];
const SVGQualifiedName& polygonTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[49];
const SVGQualifiedName& polylineTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[50];
const SVGQualifiedName& radialGradientTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[51];
const SVGQualifiedName& rectTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[52];
const SVGQualifiedName& scriptTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[53];
const SVGQualifiedName& setTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[54];
const SVGQualifiedName& stopTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[55];
const SVGQualifiedName& styleTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[56];
const SVGQualifiedName& svgTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[57];
const SVGQualifiedName& switchTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[58];
const SVGQualifiedName& symbolTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[59];
const SVGQualifiedName& textTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[60];
const SVGQualifiedName& textPathTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[61];
const SVGQualifiedName& titleTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[62];
const SVGQualifiedName& tspanTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[63];
const SVGQualifiedName& useTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[64];
const SVGQualifiedName& viewTag = reinterpret_cast<SVGQualifiedName*>(&TagStorage)[65];


PassOwnPtr<const SVGQualifiedName*[]> getSVGTags()
{
    OwnPtr<const SVGQualifiedName*[]> tags = adoptArrayPtr(new const SVGQualifiedName*[SVGTagsCount]);
    for (size_t i = 0; i < SVGTagsCount; i++)
        tags[i] = reinterpret_cast<SVGQualifiedName*>(&TagStorage) + i;
    return tags.release();
}

// Attributes

void* AttrStorage[SVGAttrsCount * ((sizeof(QualifiedName) + sizeof(void *) - 1) / sizeof(void *))];

const QualifiedName& accent_heightAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[0];
const QualifiedName& accumulateAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[1];
const QualifiedName& additiveAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[2];
const QualifiedName& alignment_baselineAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[3];
const QualifiedName& alphabeticAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[4];
const QualifiedName& amplitudeAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[5];
const QualifiedName& animateAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[6];
const QualifiedName& arabic_formAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[7];
const QualifiedName& ascentAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[8];
const QualifiedName& attributeNameAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[9];
const QualifiedName& attributeTypeAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[10];
const QualifiedName& azimuthAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[11];
const QualifiedName& baseFrequencyAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[12];
const QualifiedName& baseProfileAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[13];
const QualifiedName& baseline_shiftAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[14];
const QualifiedName& bboxAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[15];
const QualifiedName& beginAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[16];
const QualifiedName& biasAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[17];
const QualifiedName& buffered_renderingAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[18];
const QualifiedName& byAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[19];
const QualifiedName& calcModeAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[20];
const QualifiedName& cap_heightAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[21];
const QualifiedName& clipAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[22];
const QualifiedName& clip_pathAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[23];
const QualifiedName& clip_ruleAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[24];
const QualifiedName& clipPathUnitsAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[25];
const QualifiedName& colorAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[26];
const QualifiedName& color_interpolationAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[27];
const QualifiedName& color_interpolation_filtersAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[28];
const QualifiedName& color_renderingAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[29];
const QualifiedName& cursorAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[30];
const QualifiedName& cxAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[31];
const QualifiedName& cyAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[32];
const QualifiedName& dAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[33];
const QualifiedName& descentAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[34];
const QualifiedName& diffuseConstantAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[35];
const QualifiedName& directionAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[36];
const QualifiedName& displayAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[37];
const QualifiedName& divisorAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[38];
const QualifiedName& dominant_baselineAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[39];
const QualifiedName& durAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[40];
const QualifiedName& dxAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[41];
const QualifiedName& dyAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[42];
const QualifiedName& edgeModeAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[43];
const QualifiedName& elevationAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[44];
const QualifiedName& enable_backgroundAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[45];
const QualifiedName& endAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[46];
const QualifiedName& exponentAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[47];
const QualifiedName& fillAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[48];
const QualifiedName& fill_opacityAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[49];
const QualifiedName& fill_ruleAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[50];
const QualifiedName& filterAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[51];
const QualifiedName& filterUnitsAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[52];
const QualifiedName& flood_colorAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[53];
const QualifiedName& flood_opacityAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[54];
const QualifiedName& font_familyAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[55];
const QualifiedName& font_sizeAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[56];
const QualifiedName& font_size_adjustAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[57];
const QualifiedName& font_stretchAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[58];
const QualifiedName& font_styleAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[59];
const QualifiedName& font_variantAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[60];
const QualifiedName& font_weightAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[61];
const QualifiedName& formatAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[62];
const QualifiedName& frAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[63];
const QualifiedName& fromAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[64];
const QualifiedName& fxAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[65];
const QualifiedName& fyAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[66];
const QualifiedName& g1Attr = reinterpret_cast<QualifiedName*>(&AttrStorage)[67];
const QualifiedName& g2Attr = reinterpret_cast<QualifiedName*>(&AttrStorage)[68];
const QualifiedName& glyph_nameAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[69];
const QualifiedName& glyph_orientation_horizontalAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[70];
const QualifiedName& glyph_orientation_verticalAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[71];
const QualifiedName& glyphRefAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[72];
const QualifiedName& gradientTransformAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[73];
const QualifiedName& gradientUnitsAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[74];
const QualifiedName& hangingAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[75];
const QualifiedName& heightAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[76];
const QualifiedName& horiz_adv_xAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[77];
const QualifiedName& horiz_origin_xAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[78];
const QualifiedName& horiz_origin_yAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[79];
const QualifiedName& ideographicAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[80];
const QualifiedName& image_renderingAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[81];
const QualifiedName& inAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[82];
const QualifiedName& in2Attr = reinterpret_cast<QualifiedName*>(&AttrStorage)[83];
const QualifiedName& interceptAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[84];
const QualifiedName& kAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[85];
const QualifiedName& k1Attr = reinterpret_cast<QualifiedName*>(&AttrStorage)[86];
const QualifiedName& k2Attr = reinterpret_cast<QualifiedName*>(&AttrStorage)[87];
const QualifiedName& k3Attr = reinterpret_cast<QualifiedName*>(&AttrStorage)[88];
const QualifiedName& k4Attr = reinterpret_cast<QualifiedName*>(&AttrStorage)[89];
const QualifiedName& kernelMatrixAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[90];
const QualifiedName& kernelUnitLengthAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[91];
const QualifiedName& keyPointsAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[92];
const QualifiedName& keySplinesAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[93];
const QualifiedName& keyTimesAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[94];
const QualifiedName& langAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[95];
const QualifiedName& lengthAdjustAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[96];
const QualifiedName& letter_spacingAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[97];
const QualifiedName& lighting_colorAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[98];
const QualifiedName& limitingConeAngleAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[99];
const QualifiedName& localAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[100];
const QualifiedName& marker_endAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[101];
const QualifiedName& marker_midAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[102];
const QualifiedName& marker_startAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[103];
const QualifiedName& markerHeightAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[104];
const QualifiedName& markerUnitsAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[105];
const QualifiedName& markerWidthAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[106];
const QualifiedName& maskAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[107];
const QualifiedName& mask_typeAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[108];
const QualifiedName& maskContentUnitsAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[109];
const QualifiedName& maskUnitsAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[110];
const QualifiedName& mathematicalAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[111];
const QualifiedName& maxAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[112];
const QualifiedName& mediaAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[113];
const QualifiedName& methodAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[114];
const QualifiedName& minAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[115];
const QualifiedName& modeAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[116];
const QualifiedName& nameAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[117];
const QualifiedName& numOctavesAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[118];
const QualifiedName& offsetAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[119];
const QualifiedName& onactivateAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[120];
const QualifiedName& onbeginAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[121];
const QualifiedName& onendAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[122];
const QualifiedName& onfocusinAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[123];
const QualifiedName& onfocusoutAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[124];
const QualifiedName& onrepeatAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[125];
const QualifiedName& onzoomAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[126];
const QualifiedName& opacityAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[127];
const QualifiedName& operatorAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[128];
const QualifiedName& orderAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[129];
const QualifiedName& orientAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[130];
const QualifiedName& orientationAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[131];
const QualifiedName& originAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[132];
const QualifiedName& overflowAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[133];
const QualifiedName& overline_positionAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[134];
const QualifiedName& overline_thicknessAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[135];
const QualifiedName& paint_orderAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[136];
const QualifiedName& panose_1Attr = reinterpret_cast<QualifiedName*>(&AttrStorage)[137];
const QualifiedName& pathAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[138];
const QualifiedName& pathLengthAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[139];
const QualifiedName& patternContentUnitsAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[140];
const QualifiedName& patternTransformAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[141];
const QualifiedName& patternUnitsAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[142];
const QualifiedName& pointer_eventsAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[143];
const QualifiedName& pointsAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[144];
const QualifiedName& pointsAtXAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[145];
const QualifiedName& pointsAtYAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[146];
const QualifiedName& pointsAtZAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[147];
const QualifiedName& preserveAlphaAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[148];
const QualifiedName& preserveAspectRatioAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[149];
const QualifiedName& primitiveUnitsAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[150];
const QualifiedName& rAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[151];
const QualifiedName& radiusAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[152];
const QualifiedName& refXAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[153];
const QualifiedName& refYAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[154];
const QualifiedName& rendering_intentAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[155];
const QualifiedName& repeatCountAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[156];
const QualifiedName& repeatDurAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[157];
const QualifiedName& requiredExtensionsAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[158];
const QualifiedName& requiredFeaturesAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[159];
const QualifiedName& restartAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[160];
const QualifiedName& resultAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[161];
const QualifiedName& rotateAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[162];
const QualifiedName& rxAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[163];
const QualifiedName& ryAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[164];
const QualifiedName& scaleAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[165];
const QualifiedName& seedAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[166];
const QualifiedName& shape_renderingAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[167];
const QualifiedName& slopeAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[168];
const QualifiedName& spacingAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[169];
const QualifiedName& specularConstantAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[170];
const QualifiedName& specularExponentAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[171];
const QualifiedName& spreadMethodAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[172];
const QualifiedName& startOffsetAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[173];
const QualifiedName& stdDeviationAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[174];
const QualifiedName& stemhAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[175];
const QualifiedName& stemvAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[176];
const QualifiedName& stitchTilesAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[177];
const QualifiedName& stop_colorAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[178];
const QualifiedName& stop_opacityAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[179];
const QualifiedName& strikethrough_positionAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[180];
const QualifiedName& strikethrough_thicknessAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[181];
const QualifiedName& strokeAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[182];
const QualifiedName& stroke_dasharrayAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[183];
const QualifiedName& stroke_dashoffsetAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[184];
const QualifiedName& stroke_linecapAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[185];
const QualifiedName& stroke_linejoinAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[186];
const QualifiedName& stroke_miterlimitAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[187];
const QualifiedName& stroke_opacityAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[188];
const QualifiedName& stroke_widthAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[189];
const QualifiedName& styleAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[190];
const QualifiedName& surfaceScaleAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[191];
const QualifiedName& systemLanguageAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[192];
const QualifiedName& tableValuesAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[193];
const QualifiedName& targetAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[194];
const QualifiedName& targetXAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[195];
const QualifiedName& targetYAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[196];
const QualifiedName& text_anchorAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[197];
const QualifiedName& text_decorationAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[198];
const QualifiedName& text_renderingAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[199];
const QualifiedName& textLengthAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[200];
const QualifiedName& titleAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[201];
const QualifiedName& toAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[202];
const QualifiedName& transformAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[203];
const QualifiedName& transform_originAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[204];
const QualifiedName& typeAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[205];
const QualifiedName& u1Attr = reinterpret_cast<QualifiedName*>(&AttrStorage)[206];
const QualifiedName& u2Attr = reinterpret_cast<QualifiedName*>(&AttrStorage)[207];
const QualifiedName& underline_positionAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[208];
const QualifiedName& underline_thicknessAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[209];
const QualifiedName& unicodeAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[210];
const QualifiedName& unicode_bidiAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[211];
const QualifiedName& unicode_rangeAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[212];
const QualifiedName& units_per_emAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[213];
const QualifiedName& v_alphabeticAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[214];
const QualifiedName& v_hangingAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[215];
const QualifiedName& v_ideographicAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[216];
const QualifiedName& v_mathematicalAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[217];
const QualifiedName& valuesAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[218];
const QualifiedName& vector_effectAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[219];
const QualifiedName& versionAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[220];
const QualifiedName& vert_adv_yAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[221];
const QualifiedName& vert_origin_xAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[222];
const QualifiedName& vert_origin_yAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[223];
const QualifiedName& viewBoxAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[224];
const QualifiedName& viewTargetAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[225];
const QualifiedName& visibilityAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[226];
const QualifiedName& widthAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[227];
const QualifiedName& widthsAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[228];
const QualifiedName& word_spacingAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[229];
const QualifiedName& writing_modeAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[230];
const QualifiedName& xAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[231];
const QualifiedName& x_heightAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[232];
const QualifiedName& x1Attr = reinterpret_cast<QualifiedName*>(&AttrStorage)[233];
const QualifiedName& x2Attr = reinterpret_cast<QualifiedName*>(&AttrStorage)[234];
const QualifiedName& xChannelSelectorAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[235];
const QualifiedName& yAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[236];
const QualifiedName& y1Attr = reinterpret_cast<QualifiedName*>(&AttrStorage)[237];
const QualifiedName& y2Attr = reinterpret_cast<QualifiedName*>(&AttrStorage)[238];
const QualifiedName& yChannelSelectorAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[239];
const QualifiedName& zAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[240];
const QualifiedName& zoomAndPanAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[241];
const QualifiedName& hrefAttr = reinterpret_cast<QualifiedName*>(&AttrStorage)[242];

PassOwnPtr<const QualifiedName*[]> getSVGAttrs()
{
    OwnPtr<const QualifiedName*[]> attrs = adoptArrayPtr(new const QualifiedName*[SVGAttrsCount]);
    for (size_t i = 0; i < SVGAttrsCount; i++)
        attrs[i] = reinterpret_cast<QualifiedName*>(&AttrStorage) + i;
    return attrs.release();
}


void init()
{
    struct NameEntry {
        const char* name;
        unsigned hash;
        unsigned char length;
        unsigned char isTag;
        unsigned char isAttr;
    };

    // Use placement new to initialize the globals.
    AtomicString svgNS("http://www.w3.org/2000/svg", AtomicString::ConstructFromLiteral);

    // Namespace
    new ((void*)&svgNamespaceURI) AtomicString(svgNS);
    static const NameEntry kNames[] = {
        { "a", 9778235, 1, 1, 0 },
        { "accent-height", 7818218, 13, 0, 1 },
        { "accumulate", 12470039, 10, 0, 1 },
        { "additive", 2986601, 8, 0, 1 },
        { "alignment-baseline", 15363520, 18, 0, 1 },
        { "alphabetic", 5333598, 10, 0, 1 },
        { "amplitude", 4611145, 9, 0, 1 },
        { "animate", 15142871, 7, 1, 1 },
        { "animateColor", 8168430, 12, 1, 0 },
        { "animateMotion", 9626227, 13, 1, 0 },
        { "animateTransform", 8940358, 16, 1, 0 },
        { "arabic-form", 11127318, 11, 0, 1 },
        { "ascent", 10886646, 6, 0, 1 },
        { "attributeName", 8469069, 13, 0, 1 },
        { "attributeType", 4267467, 13, 0, 1 },
        { "azimuth", 11445731, 7, 0, 1 },
        { "baseFrequency", 5843500, 13, 0, 1 },
        { "baseProfile", 6136015, 11, 0, 1 },
        { "baseline-shift", 738627, 14, 0, 1 },
        { "bbox", 3471533, 4, 0, 1 },
        { "begin", 12626174, 5, 0, 1 },
        { "bias", 16339666, 4, 0, 1 },
        { "buffered-rendering", 711842, 18, 0, 1 },
        { "by", 2718574, 2, 0, 1 },
        { "calcMode", 13855136, 8, 0, 1 },
        { "cap-height", 2435412, 10, 0, 1 },
        { "circle", 1709685, 6, 1, 0 },
        { "clip", 15009294, 4, 0, 1 },
        { "clip-path", 15117952, 9, 0, 1 },
        { "clip-rule", 9308965, 9, 0, 1 },
        { "clipPath", 4241194, 8, 1, 0 },
        { "clipPathUnits", 6700353, 13, 0, 1 },
        { "color", 2734929, 5, 0, 1 },
        { "color-interpolation", 5191234, 19, 0, 1 },
        { "color-interpolation-filters", 10987393, 27, 0, 1 },
        { "color-rendering", 10166753, 15, 0, 1 },
        { "cursor", 1987604, 6, 1, 1 },
        { "cx", 4988833, 2, 0, 1 },
        { "cy", 1446155, 2, 0, 1 },
        { "d", 2286069, 1, 0, 1 },
        { "defs", 3666553, 4, 1, 0 },
        { "desc", 8557401, 4, 1, 0 },
        { "descent", 1127408, 7, 0, 1 },
        { "diffuseConstant", 2992919, 15, 0, 1 },
        { "direction", 16092904, 9, 0, 1 },
        { "discard", 12039290, 7, 1, 0 },
        { "display", 16245385, 7, 0, 1 },
        { "divisor", 5380631, 7, 0, 1 },
        { "dominant-baseline", 8742937, 17, 0, 1 },
        { "dur", 1381072, 3, 0, 1 },
        { "dx", 4552890, 2, 0, 1 },
        { "dy", 7600873, 2, 0, 1 },
        { "edgeMode", 8996806, 8, 0, 1 },
        { "elevation", 4496523, 9, 0, 1 },
        { "ellipse", 15372068, 7, 1, 0 },
        { "enable-background", 7604741, 17, 0, 1 },
        { "end", 1590106, 3, 0, 1 },
        { "exponent", 7735438, 8, 0, 1 },
        { "feBlend", 11501676, 7, 1, 0 },
        { "feColorMatrix", 3169415, 13, 1, 0 },
        { "feComponentTransfer", 3518927, 19, 1, 0 },
        { "feComposite", 12541773, 11, 1, 0 },
        { "feConvolveMatrix", 14493860, 16, 1, 0 },
        { "feDiffuseLighting", 3352565, 17, 1, 0 },
        { "feDisplacementMap", 5905267, 17, 1, 0 },
        { "feDistantLight", 747025, 14, 1, 0 },
        { "feDropShadow", 10767402, 12, 1, 0 },
        { "feFlood", 6640775, 7, 1, 0 },
        { "feFuncA", 4019930, 7, 1, 0 },
        { "feFuncB", 15012662, 7, 1, 0 },
        { "feFuncG", 4786906, 7, 1, 0 },
        { "feFuncR", 95219, 7, 1, 0 },
        { "feGaussianBlur", 11103364, 14, 1, 0 },
        { "feImage", 7842896, 7, 1, 0 },
        { "feMerge", 1453606, 7, 1, 0 },
        { "feMergeNode", 15500549, 11, 1, 0 },
        { "feMorphology", 8041573, 12, 1, 0 },
        { "feOffset", 10253256, 8, 1, 0 },
        { "fePointLight", 15054408, 12, 1, 0 },
        { "feSpecularLighting", 10866110, 18, 1, 0 },
        { "feSpotLight", 3344980, 11, 1, 0 },
        { "feTile", 2220132, 6, 1, 0 },
        { "feTurbulence", 14661844, 12, 1, 0 },
        { "fill", 14906492, 4, 0, 1 },
        { "fill-opacity", 1178205, 12, 0, 1 },
        { "fill-rule", 11672563, 9, 0, 1 },
        { "filter", 16731886, 6, 1, 1 },
        { "filterUnits", 11167090, 11, 0, 1 },
        { "flood-color", 9861123, 11, 0, 1 },
        { "flood-opacity", 12898900, 13, 0, 1 },
        { "font-family", 11614204, 11, 0, 1 },
        { "font-size", 3230817, 9, 0, 1 },
        { "font-size-adjust", 7342273, 16, 0, 1 },
        { "font-stretch", 8655366, 12, 0, 1 },
        { "font-style", 6471658, 10, 0, 1 },
        { "font-variant", 7877027, 12, 0, 1 },
        { "font-weight", 7792151, 11, 0, 1 },
        { "foreignObject", 15048399, 13, 1, 0 },
        { "format", 15152429, 6, 0, 1 },
        { "fr", 9385464, 2, 0, 1 },
        { "from", 16135834, 4, 0, 1 },
        { "fx", 15324007, 2, 0, 1 },
        { "fy", 15642507, 2, 0, 1 },
        { "g", 16436612, 1, 1, 0 },
        { "g1", 11608154, 2, 0, 1 },
        { "g2", 4017113, 2, 0, 1 },
        { "glyph-name", 16482391, 10, 0, 1 },
        { "glyph-orientation-horizontal", 9213710, 28, 0, 1 },
        { "glyph-orientation-vertical", 5038937, 26, 0, 1 },
        { "glyphRef", 7998179, 8, 0, 1 },
        { "gradientTransform", 7347535, 17, 0, 1 },
        { "gradientUnits", 1404814, 13, 0, 1 },
        { "hanging", 10850364, 7, 0, 1 },
        { "height", 6697833, 6, 0, 1 },
        { "horiz-adv-x", 5218312, 11, 0, 1 },
        { "horiz-origin-x", 7598127, 14, 0, 1 },
        { "horiz-origin-y", 14834332, 14, 0, 1 },
        { "ideographic", 4061484, 11, 0, 1 },
        { "image", 10287573, 5, 1, 0 },
        { "image-rendering", 16430331, 15, 0, 1 },
        { "in", 5937979, 2, 0, 1 },
        { "in2", 14108186, 3, 0, 1 },
        { "intercept", 1705558, 9, 0, 1 },
        { "k", 2339254, 1, 0, 1 },
        { "k1", 6632531, 2, 0, 1 },
        { "k2", 14802177, 2, 0, 1 },
        { "k3", 5328286, 2, 0, 1 },
        { "k4", 1699474, 2, 0, 1 },
        { "kernelMatrix", 12629737, 12, 0, 1 },
        { "kernelUnitLength", 12011177, 16, 0, 1 },
        { "keyPoints", 4334432, 9, 0, 1 },
        { "keySplines", 14518292, 10, 0, 1 },
        { "keyTimes", 16684339, 8, 0, 1 },
        { "lang", 3702417, 4, 0, 1 },
        { "lengthAdjust", 13438957, 12, 0, 1 },
        { "letter-spacing", 15248365, 14, 0, 1 },
        { "lighting-color", 8113130, 14, 0, 1 },
        { "limitingConeAngle", 1376504, 17, 0, 1 },
        { "line", 15272783, 4, 1, 0 },
        { "linearGradient", 1246858, 14, 1, 0 },
        { "local", 9423053, 5, 0, 1 },
        { "marker", 15384222, 6, 1, 0 },
        { "marker-end", 5221081, 10, 0, 1 },
        { "marker-mid", 14289583, 10, 0, 1 },
        { "marker-start", 980150, 12, 0, 1 },
        { "markerHeight", 3243627, 12, 0, 1 },
        { "markerUnits", 126910, 11, 0, 1 },
        { "markerWidth", 2347631, 11, 0, 1 },
        { "mask", 10325940, 4, 1, 1 },
        { "mask-type", 1273039, 9, 0, 1 },
        { "maskContentUnits", 6357138, 16, 0, 1 },
        { "maskUnits", 13353827, 9, 0, 1 },
        { "mathematical", 10197022, 12, 0, 1 },
        { "max", 3449166, 3, 0, 1 },
        { "media", 13905581, 5, 0, 1 },
        { "metadata", 2268346, 8, 1, 0 },
        { "method", 14679268, 6, 0, 1 },
        { "min", 2196278, 3, 0, 1 },
        { "mode", 13319651, 4, 0, 1 },
        { "mpath", 9277366, 5, 1, 0 },
        { "name", 8774809, 4, 0, 1 },
        { "numOctaves", 6742044, 10, 0, 1 },
        { "offset", 1221606, 6, 0, 1 },
        { "onactivate", 3026152, 10, 0, 1 },
        { "onbegin", 12049348, 7, 0, 1 },
        { "onend", 811239, 5, 0, 1 },
        { "onfocusin", 2970475, 9, 0, 1 },
        { "onfocusout", 4459422, 10, 0, 1 },
        { "onrepeat", 6681969, 8, 0, 1 },
        { "onzoom", 1226916, 6, 0, 1 },
        { "opacity", 12231101, 7, 0, 1 },
        { "operator", 16379917, 8, 0, 1 },
        { "order", 15651464, 5, 0, 1 },
        { "orient", 5635487, 6, 0, 1 },
        { "orientation", 10339552, 11, 0, 1 },
        { "origin", 2361284, 6, 0, 1 },
        { "overflow", 13155215, 8, 0, 1 },
        { "overline-position", 13556011, 17, 0, 1 },
        { "overline-thickness", 16671138, 18, 0, 1 },
        { "paint-order", 3304976, 11, 0, 1 },
        { "panose-1", 641618, 8, 0, 1 },
        { "path", 14212236, 4, 1, 1 },
        { "pathLength", 691978, 10, 0, 1 },
        { "pattern", 16283150, 7, 1, 0 },
        { "patternContentUnits", 3102758, 19, 0, 1 },
        { "patternTransform", 15581536, 16, 0, 1 },
        { "patternUnits", 12052704, 12, 0, 1 },
        { "pointer-events", 9363889, 14, 0, 1 },
        { "points", 12718235, 6, 0, 1 },
        { "pointsAtX", 16510410, 9, 0, 1 },
        { "pointsAtY", 10912372, 9, 0, 1 },
        { "pointsAtZ", 13666400, 9, 0, 1 },
        { "polygon", 1875665, 7, 1, 0 },
        { "polyline", 9966402, 8, 1, 0 },
        { "preserveAlpha", 16654065, 13, 0, 1 },
        { "preserveAspectRatio", 15032206, 19, 0, 1 },
        { "primitiveUnits", 12203325, 14, 0, 1 },
        { "r", 9755863, 1, 0, 1 },
        { "radialGradient", 4356263, 14, 1, 0 },
        { "radius", 2794824, 6, 0, 1 },
        { "rect", 15193729, 4, 1, 0 },
        { "refX", 11984993, 4, 0, 1 },
        { "refY", 14103698, 4, 0, 1 },
        { "rendering-intent", 10528535, 16, 0, 1 },
        { "repeatCount", 10019402, 11, 0, 1 },
        { "repeatDur", 8664793, 9, 0, 1 },
        { "requiredExtensions", 4208057, 18, 0, 1 },
        { "requiredFeatures", 11389123, 16, 0, 1 },
        { "restart", 2504772, 7, 0, 1 },
        { "result", 15954886, 6, 0, 1 },
        { "rotate", 16211843, 6, 0, 1 },
        { "rx", 5132163, 2, 0, 1 },
        { "ry", 14820197, 2, 0, 1 },
        { "scale", 7900465, 5, 0, 1 },
        { "script", 7137273, 6, 1, 0 },
        { "seed", 15054065, 4, 0, 1 },
        { "set", 7291079, 3, 1, 0 },
        { "shape-rendering", 7310465, 15, 0, 1 },
        { "slope", 8800286, 5, 0, 1 },
        { "spacing", 11396576, 7, 0, 1 },
        { "specularConstant", 15821133, 16, 0, 1 },
        { "specularExponent", 12340909, 16, 0, 1 },
        { "spreadMethod", 7294005, 12, 0, 1 },
        { "startOffset", 16206982, 11, 0, 1 },
        { "stdDeviation", 9412814, 12, 0, 1 },
        { "stemh", 1638345, 5, 0, 1 },
        { "stemv", 15263686, 5, 0, 1 },
        { "stitchTiles", 14130215, 11, 0, 1 },
        { "stop", 16220638, 4, 1, 0 },
        { "stop-color", 4601095, 10, 0, 1 },
        { "stop-opacity", 15542229, 12, 0, 1 },
        { "strikethrough-position", 14866568, 22, 0, 1 },
        { "strikethrough-thickness", 4458024, 23, 0, 1 },
        { "stroke", 13630306, 6, 0, 1 },
        { "stroke-dasharray", 13918191, 16, 0, 1 },
        { "stroke-dashoffset", 12491343, 17, 0, 1 },
        { "stroke-linecap", 2304011, 14, 0, 1 },
        { "stroke-linejoin", 14943542, 15, 0, 1 },
        { "stroke-miterlimit", 3565435, 17, 0, 1 },
        { "stroke-opacity", 16354193, 14, 0, 1 },
        { "stroke-width", 15643309, 12, 0, 1 },
        { "style", 10993676, 5, 1, 1 },
        { "surfaceScale", 3931981, 12, 0, 1 },
        { "svg", 8018441, 3, 1, 0 },
        { "switch", 1240910, 6, 1, 0 },
        { "symbol", 9487027, 6, 1, 0 },
        { "systemLanguage", 16769792, 14, 0, 1 },
        { "tableValues", 3989232, 11, 0, 1 },
        { "target", 1752822, 6, 0, 1 },
        { "targetX", 15131166, 7, 0, 1 },
        { "targetY", 9495385, 7, 0, 1 },
        { "text", 2784654, 4, 1, 0 },
        { "text-anchor", 10988461, 11, 0, 1 },
        { "text-decoration", 8931788, 15, 0, 1 },
        { "text-rendering", 5473995, 14, 0, 1 },
        { "textLength", 10317958, 10, 0, 1 },
        { "textPath", 14264041, 8, 1, 0 },
        { "title", 2337488, 5, 1, 1 },
        { "to", 8637605, 2, 0, 1 },
        { "transform", 12016467, 9, 0, 1 },
        { "transform-origin", 11461635, 16, 0, 1 },
        { "tspan", 1121562, 5, 1, 0 },
        { "type", 1916283, 4, 0, 1 },
        { "u1", 8892538, 2, 0, 1 },
        { "u2", 3297321, 2, 0, 1 },
        { "underline-position", 16729143, 18, 0, 1 },
        { "underline-thickness", 8311364, 19, 0, 1 },
        { "unicode", 12294694, 7, 0, 1 },
        { "unicode-bidi", 16168075, 12, 0, 1 },
        { "unicode-range", 8777099, 13, 0, 1 },
        { "units-per-em", 14229671, 12, 0, 1 },
        { "use", 1434542, 3, 1, 0 },
        { "v-alphabetic", 4753593, 12, 0, 1 },
        { "v-hanging", 6170815, 9, 0, 1 },
        { "v-ideographic", 12768143, 13, 0, 1 },
        { "v-mathematical", 6401279, 14, 0, 1 },
        { "values", 6531200, 6, 0, 1 },
        { "vector-effect", 16211112, 13, 0, 1 },
        { "version", 8128447, 7, 0, 1 },
        { "vert-adv-y", 12348807, 10, 0, 1 },
        { "vert-origin-x", 7838799, 13, 0, 1 },
        { "vert-origin-y", 13566707, 13, 0, 1 },
        { "view", 9558954, 4, 1, 0 },
        { "viewBox", 12063430, 7, 0, 1 },
        { "viewTarget", 7658893, 10, 0, 1 },
        { "visibility", 7049701, 10, 0, 1 },
        { "width", 12902275, 5, 0, 1 },
        { "widths", 12497930, 6, 0, 1 },
        { "word-spacing", 965879, 12, 0, 1 },
        { "writing-mode", 3659809, 12, 0, 1 },
        { "x", 15484113, 1, 0, 1 },
        { "x-height", 7361931, 8, 0, 1 },
        { "x1", 9037550, 2, 0, 1 },
        { "x2", 14272341, 2, 0, 1 },
        { "xChannelSelector", 11124924, 16, 0, 1 },
        { "y", 4984204, 1, 0, 1 },
        { "y1", 9285315, 2, 0, 1 },
        { "y2", 1776935, 2, 0, 1 },
        { "yChannelSelector", 12848041, 16, 0, 1 },
        { "z", 9090206, 1, 0, 1 },
        { "zoomAndPan", 16695696, 10, 0, 1 },
        { "href", 5797448, 4, 0, 1 },
    };

    size_t tag_i = 0;
    size_t attr_i = 0;
    for (size_t i = 0; i < WTF_ARRAY_LENGTH(kNames); i++) {
        StringImpl* stringImpl = StringImpl::createStatic(kNames[i].name, kNames[i].length, kNames[i].hash);
        if (kNames[i].isTag) {
            void* address = reinterpret_cast<SVGQualifiedName*>(&TagStorage) + tag_i;
            QualifiedName::createStatic(address, stringImpl, svgNS);
            tag_i++;
        }

        if (!kNames[i].isAttr)
            continue;
        void* address = reinterpret_cast<QualifiedName*>(&AttrStorage) + attr_i;
        QualifiedName::createStatic(address, stringImpl);
        attr_i++;
    }
    ASSERT(tag_i == SVGTagsCount);
    ASSERT(attr_i == SVGAttrsCount);
}

} // SVG
} // namespace blink
