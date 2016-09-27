/*
 * Copyright (C) 2006, 2007, 2012 Apple Inc. All rights reserved.
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
 *
 */

#include "config.h"
#include "core/layout/LayoutFileUploadControl.h"

#include "core/HTMLNames.h"
#include "core/InputTypeNames.h"
#include "core/dom/shadow/ElementShadow.h"
#include "core/dom/shadow/ShadowRoot.h"
#include "core/editing/PositionWithAffinity.h"
#include "core/fileapi/FileList.h"
#include "core/html/HTMLInputElement.h"
#include "core/layout/LayoutButton.h"
#include "core/layout/LayoutTheme.h"
#include "core/layout/TextRunConstructor.h"
#include "core/paint/FileUploadControlPainter.h"
#include "platform/fonts/Font.h"
#include "platform/text/PlatformLocale.h"
#include "platform/text/TextRun.h"
#include <math.h>

namespace blink {

using namespace HTMLNames;

const int defaultWidthNumChars = 34;

LayoutFileUploadControl::LayoutFileUploadControl(HTMLInputElement* input)
    : LayoutBlockFlow(input)
    , m_canReceiveDroppedFiles(input->canReceiveDroppedFiles())
{
}

LayoutFileUploadControl::~LayoutFileUploadControl()
{
}

void LayoutFileUploadControl::updateFromElement()
{
    HTMLInputElement* input = toHTMLInputElement(node());
    ASSERT(input->type() == InputTypeNames::file);

    if (HTMLInputElement* button = uploadButton()) {
        bool newCanReceiveDroppedFilesState = input->canReceiveDroppedFiles();
        if (m_canReceiveDroppedFiles != newCanReceiveDroppedFilesState) {
            m_canReceiveDroppedFiles = newCanReceiveDroppedFilesState;
            button->setActive(newCanReceiveDroppedFilesState);
        }
    }

    // This only supports clearing out the files, but that's OK because for
    // security reasons that's the only change the DOM is allowed to make.
    FileList* files = input->files();
    ASSERT(files);
    if (files && files->isEmpty())
        setShouldDoFullPaintInvalidation();
}

int LayoutFileUploadControl::maxFilenameWidth() const
{
    int uploadButtonWidth = (uploadButton() && uploadButton()->layoutBox()) ? uploadButton()->layoutBox()->pixelSnappedWidth() : 0;
    return std::max(0, contentBoxRect().pixelSnappedWidth() - uploadButtonWidth - afterButtonSpacing);
}

void LayoutFileUploadControl::paintObject(const PaintInfo& paintInfo, const LayoutPoint& paintOffset)
{
    FileUploadControlPainter(*this).paintObject(paintInfo, paintOffset);
}

void LayoutFileUploadControl::computeIntrinsicLogicalWidths(LayoutUnit& minLogicalWidth, LayoutUnit& maxLogicalWidth) const
{
    // Figure out how big the filename space needs to be for a given number of characters
    // (using "0" as the nominal character).
    const UChar character = '0';
    const String characterAsString = String(&character, 1);
    const Font& font = style()->font();
    // FIXME: Remove the need for this const_cast by making constructTextRun take a const LayoutObject*.
    LayoutFileUploadControl* layoutObject = const_cast<LayoutFileUploadControl*>(this);
    float minDefaultLabelWidth = defaultWidthNumChars * font.width(constructTextRun(layoutObject, font, characterAsString, styleRef(), TextRun::AllowTrailingExpansion));

    const String label = toHTMLInputElement(node())->locale().queryString(WebLocalizedString::FileButtonNoFileSelectedLabel);
    float defaultLabelWidth = font.width(constructTextRun(layoutObject, font, label, styleRef(), TextRun::AllowTrailingExpansion));
    if (HTMLInputElement* button = uploadButton()) {
        if (LayoutObject* buttonLayoutObject = button->layoutObject())
            defaultLabelWidth += buttonLayoutObject->maxPreferredLogicalWidth() + afterButtonSpacing;
    }
    maxLogicalWidth = static_cast<int>(ceilf(std::max(minDefaultLabelWidth, defaultLabelWidth)));

    if (!style()->width().hasPercent())
        minLogicalWidth = maxLogicalWidth;
}

void LayoutFileUploadControl::computePreferredLogicalWidths()
{
    ASSERT(preferredLogicalWidthsDirty());

    m_minPreferredLogicalWidth = 0;
    m_maxPreferredLogicalWidth = 0;
    const ComputedStyle& styleToUse = styleRef();

    if (styleToUse.width().isFixed() && styleToUse.width().value() > 0)
        m_minPreferredLogicalWidth = m_maxPreferredLogicalWidth = adjustContentBoxLogicalWidthForBoxSizing(styleToUse.width().value());
    else
        computeIntrinsicLogicalWidths(m_minPreferredLogicalWidth, m_maxPreferredLogicalWidth);

    if (styleToUse.minWidth().isFixed() && styleToUse.minWidth().value() > 0) {
        m_maxPreferredLogicalWidth = std::max(m_maxPreferredLogicalWidth, adjustContentBoxLogicalWidthForBoxSizing(styleToUse.minWidth().value()));
        m_minPreferredLogicalWidth = std::max(m_minPreferredLogicalWidth, adjustContentBoxLogicalWidthForBoxSizing(styleToUse.minWidth().value()));
    }

    if (styleToUse.maxWidth().isFixed()) {
        m_maxPreferredLogicalWidth = std::min(m_maxPreferredLogicalWidth, adjustContentBoxLogicalWidthForBoxSizing(styleToUse.maxWidth().value()));
        m_minPreferredLogicalWidth = std::min(m_minPreferredLogicalWidth, adjustContentBoxLogicalWidthForBoxSizing(styleToUse.maxWidth().value()));
    }

    int toAdd = borderAndPaddingWidth();
    m_minPreferredLogicalWidth += toAdd;
    m_maxPreferredLogicalWidth += toAdd;

    clearPreferredLogicalWidthsDirty();
}

PositionWithAffinity LayoutFileUploadControl::positionForPoint(const LayoutPoint&)
{
    return PositionWithAffinity();
}

HTMLInputElement* LayoutFileUploadControl::uploadButton() const
{
    // FIXME: This should be on HTMLInputElement as an API like innerButtonElement().
    HTMLInputElement* input = toHTMLInputElement(node());
    Node* buttonNode = input->userAgentShadowRoot()->firstChild();
    return isHTMLInputElement(buttonNode) ? toHTMLInputElement(buttonNode) : 0;
}

String LayoutFileUploadControl::buttonValue()
{
    if (HTMLInputElement* button = uploadButton())
        return button->value();

    return String();
}

String LayoutFileUploadControl::fileTextValue() const
{
    HTMLInputElement* input = toHTMLInputElement(node());
    ASSERT(input->files());
    return LayoutTheme::theme().fileListNameForWidth(input->locale(), input->files(), style()->font(), maxFilenameWidth());
}

} // namespace blink
