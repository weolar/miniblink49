/*
 * Copyright (C) 2006 Apple Computer, Inc.
 * Copyright (C) 2009 Torch Mobile Inc. http://www.torchmobile.com/
 * Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies)
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

#ifndef HitTestRequest_h
#define HitTestRequest_h

#include "wtf/Assertions.h"

namespace blink {

class HitTestRequest {
public:
    enum RequestType {
        ReadOnly = 1 << 1,
        Active = 1 << 2,
        Move = 1 << 3,
        Release = 1 << 4,
        IgnoreClipping = 1 << 5,
        SVGClipContent = 1 << 6,
        TouchEvent = 1 << 7,
        AllowChildFrameContent = 1 << 8,
        ChildFrameHitTest = 1 << 9,
        IgnorePointerEventsNone = 1 << 10,
        // Collect a list of nodes instead of just one.
        // (This is for elementsFromPoint and rect-based tests).
        ListBased = 1 << 11,
        // When using list-based testing, this flag causes us to continue hit
        // testing after a hit has been found.
        PenetratingList = 1 << 12,
        AvoidCache = 1 << 13,
    };

    typedef unsigned HitTestRequestType;

    HitTestRequest(HitTestRequestType requestType)
        : m_requestType(requestType)
    {
        // Penetrating lists should also be list-based.
        ASSERT(!(requestType & PenetratingList) || (requestType & ListBased));
    }

    bool readOnly() const { return m_requestType & ReadOnly; }
    bool active() const { return m_requestType & Active; }
    bool move() const { return m_requestType & Move; }
    bool release() const { return m_requestType & Release; }
    bool ignoreClipping() const { return m_requestType & IgnoreClipping; }
    bool svgClipContent() const { return m_requestType & SVGClipContent; }
    bool touchEvent() const { return m_requestType & TouchEvent; }
    bool allowsChildFrameContent() const { return m_requestType & AllowChildFrameContent; }
    bool isChildFrameHitTest() const { return m_requestType & ChildFrameHitTest; }
    bool ignorePointerEventsNone() const { return m_requestType & IgnorePointerEventsNone; }
    bool listBased() const { return m_requestType & ListBased; }
    bool penetratingList() const { return m_requestType & PenetratingList; }
    bool avoidCache() const { return m_requestType & AvoidCache; }

    // Convenience functions
    bool touchMove() const { return move() && touchEvent(); }

    HitTestRequestType type() const { return m_requestType; }

    // The Cacheability bits don't affect hit testing computation.
    // TODO(dtapuska): These bits really shouldn't be fields on the HitTestRequest as
    // they don't influence the result; but rather are hints on the output as to what to do.
    // Perhaps move these fields to another enum ?
    static const HitTestRequestType CacheabilityBits = ReadOnly | Active | Move | Release | TouchEvent;
    bool equalForCacheability(const HitTestRequest& value) const
    {
        return (m_requestType | CacheabilityBits) == (value.m_requestType | CacheabilityBits);
    }

private:
    HitTestRequestType m_requestType;
};

} // namespace blink

#endif // HitTestRequest_h
