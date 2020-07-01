/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef StyleBuilderConverter_h
#define StyleBuilderConverter_h

#include "core/css/CSSValue.h"
#include "core/css/CSSValueList.h"
#include "core/css/resolver/StyleResolverState.h"
#include "core/layout/LayoutView.h"
#include "core/style/QuotesData.h"
#include "core/style/ShadowList.h"
#include "core/style/StyleReflection.h"
#include "core/style/StyleScrollSnapData.h"
#include "core/style/TransformOrigin.h"
#include "platform/LengthSize.h"
#include "platform/fonts/FontDescription.h"
#include "platform/text/TabSize.h"

namespace blink {

class RotateTransformOperation;
class TranslateTransformOperation;
class ScaleTransformOperation;

// Note that we assume the parser only allows valid CSSValue types.
class StyleBuilderConverter {
public:
    static PassRefPtr<StyleReflection> convertBoxReflect(StyleResolverState&, CSSValue*);
    static AtomicString convertFragmentIdentifier(StyleResolverState&, CSSValue*);
    static Color convertColor(StyleResolverState&, CSSValue*, bool forVisitedLink = false);
    template <typename T> static T convertComputedLength(StyleResolverState&, CSSValue*);
    static LengthBox convertClip(StyleResolverState&, CSSValue*);
    template <typename T> static T convertFlags(StyleResolverState&, CSSValue*);
    static FontDescription::FamilyDescription convertFontFamily(StyleResolverState&, CSSValue*);
    static PassRefPtr<FontFeatureSettings> convertFontFeatureSettings(StyleResolverState&, CSSValue*);
    static FontDescription::Size convertFontSize(StyleResolverState&, CSSValue*);
    static float convertFontSizeAdjust(StyleResolverState&, CSSValue*);
    static FontWeight convertFontWeight(StyleResolverState&, CSSValue*);
    static FontDescription::VariantLigatures convertFontVariantLigatures(StyleResolverState&, CSSValue*);
    static EGlyphOrientation convertGlyphOrientation(StyleResolverState&, CSSValue*);
    static StyleSelfAlignmentData convertSelfOrDefaultAlignmentData(StyleResolverState&, CSSValue*);
    static StyleContentAlignmentData convertContentAlignmentData(StyleResolverState&, CSSValue*);
    static GridAutoFlow convertGridAutoFlow(StyleResolverState&, CSSValue*);
    static GridPosition convertGridPosition(StyleResolverState&, CSSValue*);
    static GridTrackSize convertGridTrackSize(StyleResolverState&, CSSValue*);
    template <typename T> static T convertLineWidth(StyleResolverState&, CSSValue*);
    static Length convertLength(const StyleResolverState&, CSSValue*);
    static UnzoomedLength convertUnzoomedLength(const StyleResolverState&, CSSValue*);
    static Length convertLengthOrAuto(const StyleResolverState&, CSSValue*);
    static Length convertLengthSizing(StyleResolverState&, CSSValue*);
    static Length convertLengthMaxSizing(StyleResolverState&, CSSValue*);
    static TabSize convertLengthOrTabSpaces(StyleResolverState&, CSSValue*);
    static LineBoxContain convertLineBoxContain(StyleResolverState&, CSSValue*);
    static Length convertLineHeight(StyleResolverState&, CSSValue*);
    static float convertNumberOrPercentage(StyleResolverState&, CSSValue*);
    static LengthPoint convertPosition(StyleResolverState&, CSSValue*);
    static float convertPerspective(StyleResolverState&, CSSValue*);
    static LengthPoint convertPerspectiveOrigin(StyleResolverState&, CSSValue*);
    static Length convertQuirkyLength(StyleResolverState&, CSSValue*);
    static PassRefPtr<QuotesData> convertQuotes(StyleResolverState&, CSSValue*);
    static LengthSize convertRadius(StyleResolverState&, CSSValue*);
    static EPaintOrder convertPaintOrder(StyleResolverState&, CSSValue*);
    static PassRefPtr<ShadowList> convertShadow(StyleResolverState&, CSSValue*);
    static PassRefPtr<ShapeValue> convertShapeValue(StyleResolverState&, CSSValue*);
    static float convertSpacing(StyleResolverState&, CSSValue*);
    template <CSSValueID IdForNone> static AtomicString convertString(StyleResolverState&, CSSValue*);
    static PassRefPtr<SVGDashArray> convertStrokeDasharray(StyleResolverState&, CSSValue*);
    static StyleColor convertStyleColor(StyleResolverState&, CSSValue*, bool forVisitedLink = false);
    static float convertTextStrokeWidth(StyleResolverState&, CSSValue*);
    static TransformOrigin convertTransformOrigin(StyleResolverState&, CSSValue*);

    static bool convertGridTrackList(CSSValue*, Vector<GridTrackSize>&, NamedGridLinesMap&, OrderedNamedGridLines&, StyleResolverState&);
    static void createImplicitNamedGridLinesFromGridArea(const NamedGridAreaMap&, NamedGridLinesMap&, GridTrackSizingDirection);
    static void convertOrderedNamedGridLinesMapToNamedGridLinesMap(const OrderedNamedGridLines&, NamedGridLinesMap&);

    static ScrollSnapPoints convertSnapPoints(StyleResolverState&, CSSValue*);
    static Vector<LengthPoint> convertSnapCoordinates(StyleResolverState&, CSSValue*);
    static LengthPoint convertSnapDestination(StyleResolverState&, CSSValue*);
    static PassRefPtr<TranslateTransformOperation> convertTranslate(StyleResolverState&, CSSValue*);
    static PassRefPtr<RotateTransformOperation> convertRotate(StyleResolverState&, CSSValue*);
    static PassRefPtr<ScaleTransformOperation> convertScale(StyleResolverState&, CSSValue*);
};

template <typename T>
T StyleBuilderConverter::convertComputedLength(StyleResolverState& state, CSSValue* value)
{
    return toCSSPrimitiveValue(value)->computeLength<T>(state.cssToLengthConversionData());
}

template <typename T>
T StyleBuilderConverter::convertFlags(StyleResolverState& state, CSSValue* value)
{
    T flags = static_cast<T>(0);
    if (value->isPrimitiveValue() && toCSSPrimitiveValue(value)->getValueID() == CSSValueNone)
        return flags;
    for (auto& flagValue : toCSSValueList(*value))
        flags |= toCSSPrimitiveValue(*flagValue);
    return flags;
}

template <typename T>
T StyleBuilderConverter::convertLineWidth(StyleResolverState& state, CSSValue* value)
{
    CSSPrimitiveValue* primitiveValue = toCSSPrimitiveValue(value);
    CSSValueID valueID = primitiveValue->getValueID();
    if (valueID == CSSValueThin)
        return 1;
    if (valueID == CSSValueMedium)
        return 3;
    if (valueID == CSSValueThick)
        return 5;
    if (valueID == CSSValueInvalid) {
        // Any original result that was >= 1 should not be allowed to fall below 1.
        // This keeps border lines from vanishing.

        // https://chromium.googlesource.com/chromium/src/+/f73426be35a4b02a6d029407db459a87bc5e4688
        // FIXME: We are moving to use the full page zoom implementation to handle
        // high-dpi.  In that case specyfing a border-width of less than 1px would
        // result in a border that is one device pixel thick.  With this change that
        // would instead be rounded up to 2 device pixels.  Consider clamping it to
        // device pixels or zoom adjusted CSS pixels instead of raw CSS pixels.
        // Reference crbug.com/485650 and crbug.com/382483
        double result = primitiveValue->computeLength<double>(state.cssToLengthConversionData());
        if (result > 0.0 && result < 1.0)
            return 1.0;
        return clampTo<T>(roundForImpreciseConversion<T>(result), defaultMinimumForClamp<T>(), defaultMaximumForClamp<T>());
    }
    ASSERT_NOT_REACHED();
    return 0;
}

template <CSSValueID IdForNone>
AtomicString StyleBuilderConverter::convertString(StyleResolverState&, CSSValue* value)
{
    CSSPrimitiveValue* primitiveValue = toCSSPrimitiveValue(value);
    if (primitiveValue->getValueID() == IdForNone)
        return nullAtom;
    return AtomicString(primitiveValue->getStringValue());
}

} // namespace blink

#endif
