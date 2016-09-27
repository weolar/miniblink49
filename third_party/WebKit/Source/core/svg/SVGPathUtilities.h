/*
 * Copyright (C) Research In Motion Limited 2010, 2012. All rights reserved.
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

#ifndef SVGPathUtilities_h
#define SVGPathUtilities_h

#include "core/CoreExport.h"
#include "core/svg/SVGPathParser.h"
#include "platform/geometry/FloatPoint.h"
#include "wtf/text/WTFString.h"

namespace blink {

class Path;
class SVGPathByteStream;

// String/SVGPathByteStream -> Path
bool CORE_EXPORT buildPathFromString(const String&, Path&);
bool buildPathFromByteStream(const SVGPathByteStream&, Path&);

// String -> SVGPathByteStream
bool buildSVGPathByteStreamFromString(const String&, SVGPathByteStream&, PathParsingMode);

// SVGPathByteStream -> String
bool buildStringFromByteStream(const SVGPathByteStream&, String&, PathParsingMode);

bool addToSVGPathByteStream(SVGPathByteStream&, const SVGPathByteStream&, unsigned repeatCount = 1);

unsigned getSVGPathSegAtLengthFromSVGPathByteStream(const SVGPathByteStream&, float length);
float getTotalLengthOfSVGPathByteStream(const SVGPathByteStream&);
FloatPoint getPointAtLengthOfSVGPathByteStream(const SVGPathByteStream&, float length);

} // namespace blink

#endif // SVGPathUtilities_h
