/*
 * Copyright (C) 2002, 2003 The Karbon Developers
 * Copyright (C) 2006 Alexander Kellett <lypanov@kde.org>
 * Copyright (C) 2006, 2007 Rob Buis <buis@kde.org>
 * Copyright (C) 2007, 2009 Apple Inc. All rights reserved.
 * Copyright (C) Research In Motion Limited 2010. All rights reserved.
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
 */

#include "config.h"
#include "core/svg/SVGPathBuilder.h"

#include "core/svg/SVGPathSeg.h"
#include "platform/graphics/Path.h"

namespace blink {

void SVGPathBuilder::emitSegment(const PathSegmentData& segment)
{
    switch (segment.command) {
    case PathSegMoveToAbs:
        if (m_closed && !m_path.isEmpty())
            m_path.closeSubpath();
        m_path.moveTo(segment.targetPoint);
        m_closed = false;
        break;
    case PathSegLineToAbs:
        m_path.addLineTo(segment.targetPoint);
        break;
    case PathSegClosePath:
        m_path.closeSubpath();
        m_closed = true;
        break;
    case PathSegCurveToCubicAbs:
        m_path.addBezierCurveTo(segment.point1, segment.point2, segment.targetPoint);
        break;
    default:
        ASSERT_NOT_REACHED();
    }
}

}
