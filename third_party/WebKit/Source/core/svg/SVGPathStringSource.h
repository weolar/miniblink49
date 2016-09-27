/*
 * Copyright (C) Research In Motion Limited 2010. All rights reserved.
 * Copyright (C) 2013 Apple Inc. All rights reserved.
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

#ifndef SVGPathStringSource_h
#define SVGPathStringSource_h

#include "core/svg/SVGPathSource.h"
#include "wtf/text/WTFString.h"

namespace blink {

class SVGPathStringSource final : public SVGPathSource {
public:
    explicit SVGPathStringSource(const String&);

private:
    bool hasMoreData() const override;
    SVGPathSegType peekSegmentType() override;
    PathSegmentData parseSegment() override;

    void eatWhitespace();
    float parseNumberWithError();
    bool parseArcFlagWithError();

    String m_string;
    bool m_is8BitSource;
    bool m_seenError;

    union {
        const LChar* m_character8;
        const UChar* m_character16;
    } m_current;
    union {
        const LChar* m_character8;
        const UChar* m_character16;
    } m_end;

    SVGPathSegType m_previousCommand;
};

} // namespace blink

#endif // SVGPathStringSource_h
