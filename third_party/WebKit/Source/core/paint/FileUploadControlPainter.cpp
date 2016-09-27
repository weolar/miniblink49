// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/paint/FileUploadControlPainter.h"

#include "core/layout/LayoutButton.h"
#include "core/layout/LayoutFileUploadControl.h"
#include "core/layout/TextRunConstructor.h"
#include "core/paint/LayoutObjectDrawingRecorder.h"
#include "core/paint/PaintInfo.h"
#include "platform/graphics/paint/ClipRecorder.h"
#include "wtf/Optional.h"

namespace blink {

const int buttonShadowHeight = 2;

void FileUploadControlPainter::paintObject(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    if (m_layoutFileUploadControl.style()->visibility() != VISIBLE)
        return;

    // Push a clip.
    Optional<ClipRecorder> clipRecorder;
    if (paintInfo.phase == PaintPhaseForeground || paintInfo.phase == PaintPhaseChildBlockBackgrounds) {
        IntRect clipRect = enclosingIntRect(LayoutRect(
            LayoutPoint(paintOffset.x() + m_layoutFileUploadControl.borderLeft(), paintOffset.y() + m_layoutFileUploadControl.borderTop()),
            m_layoutFileUploadControl.size() + LayoutSize(0, -m_layoutFileUploadControl.borderWidth() + buttonShadowHeight)));
        if (clipRect.isEmpty())
            return;
        clipRecorder.emplace(*paintInfo.context, m_layoutFileUploadControl, DisplayItem::ClipFileUploadControlRect, LayoutRect(clipRect));
    }

    if (paintInfo.phase == PaintPhaseForeground && !LayoutObjectDrawingRecorder::useCachedDrawingIfPossible(*paintInfo.context, m_layoutFileUploadControl, paintInfo.phase)) {
        const String& displayedFilename = m_layoutFileUploadControl.fileTextValue();
        const Font& font = m_layoutFileUploadControl.style()->font();
        TextRun textRun = constructTextRun(&m_layoutFileUploadControl, font, displayedFilename, m_layoutFileUploadControl.styleRef(), RespectDirection | RespectDirectionOverride);
        textRun.setExpansionBehavior(TextRun::AllowTrailingExpansion);

        // Determine where the filename should be placed
        LayoutUnit contentLeft = paintOffset.x() + m_layoutFileUploadControl.borderLeft() + m_layoutFileUploadControl.paddingLeft();
        Node* button = m_layoutFileUploadControl.uploadButton();
        if (!button)
            return;

        int buttonWidth = (button && button->layoutBox()) ? button->layoutBox()->pixelSnappedWidth() : 0;
        LayoutUnit buttonAndSpacingWidth = buttonWidth + LayoutFileUploadControl::afterButtonSpacing;
        float textWidth = font.width(textRun);
        LayoutUnit textX;
        if (m_layoutFileUploadControl.style()->isLeftToRightDirection())
            textX = contentLeft + buttonAndSpacingWidth;
        else
            textX = contentLeft + m_layoutFileUploadControl.contentWidth() - buttonAndSpacingWidth - textWidth;

        LayoutUnit textY = 0;
        // We want to match the button's baseline
        // FIXME: Make this work with transforms.
        if (LayoutButton* buttonLayoutObject = toLayoutButton(button->layoutObject()))
            textY = paintOffset.y() + m_layoutFileUploadControl.borderTop() + m_layoutFileUploadControl.paddingTop() + buttonLayoutObject->baselinePosition(AlphabeticBaseline, true, HorizontalLine, PositionOnContainingLine);
        else
            textY = m_layoutFileUploadControl.baselinePosition(AlphabeticBaseline, true, HorizontalLine, PositionOnContainingLine);
        TextRunPaintInfo textRunPaintInfo(textRun);
        // FIXME: Shouldn't these offsets be rounded? crbug.com/350474
        textRunPaintInfo.bounds = FloatRect(textX.toFloat(), textY.toFloat() - m_layoutFileUploadControl.style()->fontMetrics().ascent(),
            textWidth, m_layoutFileUploadControl.style()->fontMetrics().height());

        // Draw the filename.
        LayoutObjectDrawingRecorder recorder(*paintInfo.context, m_layoutFileUploadControl, paintInfo.phase, textRunPaintInfo.bounds);
        paintInfo.context->setFillColor(m_layoutFileUploadControl.resolveColor(CSSPropertyColor));
        paintInfo.context->drawBidiText(font, textRunPaintInfo, FloatPoint(roundToInt(textX), roundToInt(textY)));
    }

    // Paint the children.
    m_layoutFileUploadControl.LayoutBlockFlow::paintObject(paintInfo, paintOffset);
}

} // namespace blink
