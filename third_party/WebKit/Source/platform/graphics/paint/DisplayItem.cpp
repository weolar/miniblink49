// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "platform/graphics/paint/DisplayItem.h"

namespace blink {

struct SameSizeAsDisplayItem {
    virtual ~SameSizeAsDisplayItem() { } // Allocate vtable pointer.
    void* pointers[2];
    int ints[2]; // Make sure other fields are packed into two ints.
#ifndef NDEBUG
    WTF::String m_debugString;
#endif
};
static_assert(sizeof(DisplayItem) == sizeof(SameSizeAsDisplayItem), "DisplayItem should stay small");

#ifndef NDEBUG

static WTF::String paintPhaseAsDebugString(int paintPhase)
{
    // Must be kept in sync with PaintPhase.
    switch (paintPhase) {
    case 0: return "PaintPhaseBlockBackground";
    case 1: return "PaintPhaseChildBlockBackground";
    case 2: return "PaintPhaseChildBlockBackgrounds";
    case 3: return "PaintPhaseFloat";
    case 4: return "PaintPhaseForeground";
    case 5: return "PaintPhaseOutline";
    case 6: return "PaintPhaseChildOutlines";
    case 7: return "PaintPhaseSelfOutline";
    case 8: return "PaintPhaseSelection";
    case 9: return "PaintPhaseCollapsedTableBorders";
    case 10: return "PaintPhaseTextClip";
    case 11: return "PaintPhaseMask";
    case DisplayItem::PaintPhaseMax: return "PaintPhaseClippingMask";
    default:
        ASSERT_NOT_REACHED();
        return "Unknown";
    }
}

#define PAINT_PHASE_BASED_DEBUG_STRINGS(Category) \
    if (type >= DisplayItem::Category##PaintPhaseFirst && type <= DisplayItem::Category##PaintPhaseLast) \
        return #Category + paintPhaseAsDebugString(type - DisplayItem::Category##PaintPhaseFirst);

#define DEBUG_STRING_CASE(DisplayItemName) \
    case DisplayItem::DisplayItemName: return #DisplayItemName

#define DEFAULT_CASE default: ASSERT_NOT_REACHED(); return "Unknown"

static WTF::String specialDrawingTypeAsDebugString(DisplayItem::Type type)
{
    switch (type) {
        DEBUG_STRING_CASE(BoxDecorationBackground);
        DEBUG_STRING_CASE(Caret);
        DEBUG_STRING_CASE(ColumnRules);
        DEBUG_STRING_CASE(DebugRedFill);
        DEBUG_STRING_CASE(DragImage);
        DEBUG_STRING_CASE(SVGImage);
        DEBUG_STRING_CASE(LinkHighlight);
        DEBUG_STRING_CASE(PageOverlay);
        DEBUG_STRING_CASE(PageWidgetDelegateBackgroundFallback);
        DEBUG_STRING_CASE(PopupContainerBorder);
        DEBUG_STRING_CASE(PopupListBoxBackground);
        DEBUG_STRING_CASE(PopupListBoxRow);
        DEBUG_STRING_CASE(PrintedContentBackground);
        DEBUG_STRING_CASE(PrintedContentLineBoundary);
        DEBUG_STRING_CASE(PrintedContentPDFURLRect);
        DEBUG_STRING_CASE(Resizer);
        DEBUG_STRING_CASE(SVGClip);
        DEBUG_STRING_CASE(SVGFilter);
        DEBUG_STRING_CASE(SVGMask);
        DEBUG_STRING_CASE(ScrollbarBackButtonEnd);
        DEBUG_STRING_CASE(ScrollbarBackButtonStart);
        DEBUG_STRING_CASE(ScrollbarBackground);
        DEBUG_STRING_CASE(ScrollbarBackTrack);
        DEBUG_STRING_CASE(ScrollbarCorner);
        DEBUG_STRING_CASE(ScrollbarForwardButtonEnd);
        DEBUG_STRING_CASE(ScrollbarForwardButtonStart);
        DEBUG_STRING_CASE(ScrollbarForwardTrack);
        DEBUG_STRING_CASE(ScrollbarHorizontal);
        DEBUG_STRING_CASE(ScrollbarThumb);
        DEBUG_STRING_CASE(ScrollbarTickmarks);
        DEBUG_STRING_CASE(ScrollbarTrackBackground);
        DEBUG_STRING_CASE(ScrollbarVertical);
        DEBUG_STRING_CASE(SelectionGap);
        DEBUG_STRING_CASE(SelectionTint);
        DEBUG_STRING_CASE(TableCellBackgroundFromSelfPaintingRow);
        DEBUG_STRING_CASE(VideoBitmap);
        DEBUG_STRING_CASE(WebPlugin);
        DEBUG_STRING_CASE(WebFont);

        DEFAULT_CASE;
    }
}

static WTF::String drawingTypeAsDebugString(DisplayItem::Type type)
{
    PAINT_PHASE_BASED_DEBUG_STRINGS(Drawing);
    return "Drawing" + specialDrawingTypeAsDebugString(type);
}

static WTF::String clipTypeAsDebugString(DisplayItem::Type type)
{
    PAINT_PHASE_BASED_DEBUG_STRINGS(ClipBox);
    PAINT_PHASE_BASED_DEBUG_STRINGS(ClipColumnBounds);
    PAINT_PHASE_BASED_DEBUG_STRINGS(ClipLayerFragment);

    switch (type) {
        DEBUG_STRING_CASE(ClipFileUploadControlRect);
        DEBUG_STRING_CASE(ClipFrameToVisibleContentRect);
        DEBUG_STRING_CASE(ClipFrameScrollbars);
        DEBUG_STRING_CASE(ClipLayerBackground);
        DEBUG_STRING_CASE(ClipLayerColumnBounds);
        DEBUG_STRING_CASE(ClipLayerFilter);
        DEBUG_STRING_CASE(ClipLayerForeground);
        DEBUG_STRING_CASE(ClipLayerParent);
        DEBUG_STRING_CASE(ClipLayerOverflowControls);
        DEBUG_STRING_CASE(ClipNodeImage);
        DEBUG_STRING_CASE(ClipPopupListBoxFrame);
        DEBUG_STRING_CASE(ClipSelectionImage);
        DEBUG_STRING_CASE(PageWidgetDelegateClip);
        DEBUG_STRING_CASE(TransparencyClip);
        DEBUG_STRING_CASE(ClipPrintedPage);
        DEFAULT_CASE;
    }
}

static String transform3DTypeAsDebugString(DisplayItem::Type type)
{
    switch (type) {
        DEBUG_STRING_CASE(Transform3DElementTransform);
        DEFAULT_CASE;
    }
}

WTF::String DisplayItem::typeAsDebugString(Type type)
{
    if (isDrawingType(type))
        return drawingTypeAsDebugString(type);
    if (isCachedType(type))
        return "Cached" + drawingTypeAsDebugString(cachedTypeToDrawingType(type));
    if (isClipType(type))
        return clipTypeAsDebugString(type);
    if (isEndClipType(type))
        return "End" + clipTypeAsDebugString(endClipTypeToClipType(type));

    if (type == UninitializedType)
        return "UninitializedType";

    PAINT_PHASE_BASED_DEBUG_STRINGS(FloatClip);
    if (isEndFloatClipType(type))
        return "End" + typeAsDebugString(endFloatClipTypeToFloatClipType(type));

    PAINT_PHASE_BASED_DEBUG_STRINGS(Scroll);
    if (isEndScrollType(type))
        return "End" + typeAsDebugString(endScrollTypeToScrollType(type));

    if (isTransform3DType(type))
        return transform3DTypeAsDebugString(type);
    if (isEndTransform3DType(type))
        return "End" + transform3DTypeAsDebugString(endTransform3DTypeToTransform3DType(type));

    PAINT_PHASE_BASED_DEBUG_STRINGS(SubtreeCached);
    PAINT_PHASE_BASED_DEBUG_STRINGS(BeginSubtree);
    PAINT_PHASE_BASED_DEBUG_STRINGS(EndSubtree);

    if (type == UninitializedType)
        return "UninitializedType";

    switch (type) {
        DEBUG_STRING_CASE(BeginFilter);
        DEBUG_STRING_CASE(EndFilter);
        DEBUG_STRING_CASE(BeginCompositing);
        DEBUG_STRING_CASE(EndCompositing);
        DEBUG_STRING_CASE(BeginTransform);
        DEBUG_STRING_CASE(EndTransform);
        DEBUG_STRING_CASE(BeginClipPath);
        DEBUG_STRING_CASE(EndClipPath);
        DEBUG_STRING_CASE(BeginFixedPosition);
        DEBUG_STRING_CASE(EndFixedPosition);
        DEBUG_STRING_CASE(BeginFixedPositionContainer);
        DEBUG_STRING_CASE(EndFixedPositionContainer);
        DEFAULT_CASE;
    }
}

WTF::String DisplayItem::asDebugString() const
{
    WTF::StringBuilder stringBuilder;
    stringBuilder.append('{');
    dumpPropertiesAsDebugString(stringBuilder);
    stringBuilder.append('}');
    return stringBuilder.toString();
}

void DisplayItem::dumpPropertiesAsDebugString(WTF::StringBuilder& stringBuilder) const
{
    stringBuilder.append(String::format("client: \"%p", client()));
    if (!clientDebugString().isEmpty()) {
        stringBuilder.append(' ');
        stringBuilder.append(clientDebugString());
    }
    stringBuilder.append("\", type: \"");
    stringBuilder.append(typeAsDebugString(type()));
    stringBuilder.append('"');
    if (m_skippedCache)
        stringBuilder.append(", skippedCache: true");
    if (m_scopeContainer)
        stringBuilder.append(String::format(", scope: \"%p,%d\"", m_scopeContainer, m_scopeId));
}

#endif

} // namespace blink
