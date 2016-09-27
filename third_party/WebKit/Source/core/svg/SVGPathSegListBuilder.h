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

#ifndef SVGPathSegListBuilder_h
#define SVGPathSegListBuilder_h

#include "core/svg/SVGPathConsumer.h"
#include "core/svg/SVGPathSegList.h"
#include "platform/heap/Handle.h"

namespace blink {

class SVGPathElement;

class SVGPathSegListBuilder final : public SVGPathConsumer {
public:
    SVGPathSegListBuilder(SVGPathElement*, PassRefPtrWillBeRawPtr<SVGPathSegList>);

private:
    void incrementPathSegmentCount() override { }
    bool continueConsuming() override { return true; }

    void emitSegment(const PathSegmentData&) override;

    RawPtrWillBeMember<SVGPathElement> m_pathElement;
    RefPtrWillBeMember<SVGPathSegList> m_pathSegList;
};

} // namespace blink

#endif // SVGPathSegListBuilder_h
