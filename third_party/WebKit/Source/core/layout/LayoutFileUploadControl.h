/*
 * Copyright (C) 2006, 2007, 2009, 2012 Apple Inc. All rights reserved.
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

#ifndef LayoutFileUploadControl_h
#define LayoutFileUploadControl_h

#include "core/CoreExport.h"
#include "core/layout/LayoutBlockFlow.h"

namespace blink {

class HTMLInputElement;

// Each LayoutFileUploadControl contains a LayoutButton (for opening the file chooser), and
// sufficient space to draw a file icon and filename. The LayoutButton has a shadow node
// associated with it to receive click/hover events.

class CORE_EXPORT LayoutFileUploadControl final : public LayoutBlockFlow {
public:
    LayoutFileUploadControl(HTMLInputElement*);
    virtual ~LayoutFileUploadControl();

    virtual bool isOfType(LayoutObjectType type) const override { return type == LayoutObjectFileUploadControl || LayoutBlockFlow::isOfType(type); }

    String buttonValue();
    String fileTextValue() const;

    HTMLInputElement* uploadButton() const;
    int uploadButtonWidth();

    static const int afterButtonSpacing = 4;

    virtual const char* name() const override { return "LayoutFileUploadControl"; }

private:
    virtual void updateFromElement() override;
    virtual void computeIntrinsicLogicalWidths(LayoutUnit& minLogicalWidth, LayoutUnit& maxLogicalWidth) const override;
    virtual void computePreferredLogicalWidths() override;
    virtual void paintObject(const PaintInfo&, const LayoutPoint&) override;

    int maxFilenameWidth() const;

    virtual PositionWithAffinity positionForPoint(const LayoutPoint&) override;

    bool m_canReceiveDroppedFiles;
};

DEFINE_LAYOUT_OBJECT_TYPE_CASTS(LayoutFileUploadControl, isFileUploadControl());

} // namespace blink

#endif // LayoutFileUploadControl_h
