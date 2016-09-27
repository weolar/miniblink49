/*
    Copyright (C) 2004, 2005, 2007 Nikolas Zimmermann <zimmermann@kde.org>
                  2004, 2005 Rob Buis <buis@kde.org>
    Copyright (C) Research In Motion Limited 2010. All rights reserved.

    Based on khtml code by:
    Copyright (C) 2000-2003 Lars Knoll (knoll@kde.org)
              (C) 2000 Antti Koivisto (koivisto@kde.org)
              (C) 2000-2003 Dirk Mueller (mueller@kde.org)
              (C) 2002-2003 Apple Computer, Inc.

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.

    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.

    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
    Boston, MA 02110-1301, USA.
*/

#ifndef SVGComputedStyleDefs_h
#define SVGComputedStyleDefs_h

#include "core/CoreExport.h"
#include "platform/Length.h"
#include "platform/graphics/Color.h"
#include "wtf/OwnPtr.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/RefCounted.h"
#include "wtf/RefPtr.h"
#include "wtf/RefVector.h"
#include "wtf/text/WTFString.h"

namespace blink {

typedef RefVector<Length> SVGDashArray;

enum SVGPaintType {
    SVG_PAINTTYPE_RGBCOLOR,
    SVG_PAINTTYPE_NONE,
    SVG_PAINTTYPE_CURRENTCOLOR,
    SVG_PAINTTYPE_URI_NONE,
    SVG_PAINTTYPE_URI_CURRENTCOLOR,
    SVG_PAINTTYPE_URI_RGBCOLOR,
    SVG_PAINTTYPE_URI
};

enum EBaselineShift {
    BS_LENGTH, BS_SUB, BS_SUPER
};

enum ETextAnchor {
    TA_START, TA_MIDDLE, TA_END
};

enum EColorInterpolation {
    CI_AUTO, CI_SRGB, CI_LINEARRGB
};

enum EColorRendering {
    CR_AUTO, CR_OPTIMIZESPEED, CR_OPTIMIZEQUALITY
};
enum EShapeRendering {
    SR_AUTO, SR_OPTIMIZESPEED, SR_CRISPEDGES, SR_GEOMETRICPRECISION
};

enum SVGWritingMode {
    WM_LRTB, WM_LR, WM_RLTB, WM_RL, WM_TBRL, WM_TB
};

enum EGlyphOrientation {
    GO_0DEG, GO_90DEG, GO_180DEG, GO_270DEG, GO_AUTO
};

enum EAlignmentBaseline {
    AB_AUTO, AB_BASELINE, AB_BEFORE_EDGE, AB_TEXT_BEFORE_EDGE,
    AB_MIDDLE, AB_CENTRAL, AB_AFTER_EDGE, AB_TEXT_AFTER_EDGE,
    AB_IDEOGRAPHIC, AB_ALPHABETIC, AB_HANGING, AB_MATHEMATICAL
};

enum EDominantBaseline {
    DB_AUTO, DB_USE_SCRIPT, DB_NO_CHANGE, DB_RESET_SIZE,
    DB_IDEOGRAPHIC, DB_ALPHABETIC, DB_HANGING, DB_MATHEMATICAL,
    DB_CENTRAL, DB_MIDDLE, DB_TEXT_AFTER_EDGE, DB_TEXT_BEFORE_EDGE
};

enum EVectorEffect {
    VE_NONE,
    VE_NON_SCALING_STROKE
};

enum EBufferedRendering {
    BR_AUTO,
    BR_DYNAMIC,
    BR_STATIC
};

enum EMaskType {
    MT_LUMINANCE,
    MT_ALPHA
};

enum EPaintOrderType {
    PT_NONE    = 0,
    PT_FILL    = 1,
    PT_STROKE  = 2,
    PT_MARKERS = 3
};

const int kPaintOrderBitwidth = 2;
typedef unsigned EPaintOrder;
const unsigned PO_NORMAL = PT_FILL | PT_STROKE << 2 | PT_MARKERS << 4;

// Inherited/Non-Inherited Style Datastructures
class StyleFillData : public RefCounted<StyleFillData> {
public:
    static PassRefPtr<StyleFillData> create() { return adoptRef(new StyleFillData); }
    PassRefPtr<StyleFillData> copy() const { return adoptRef(new StyleFillData(*this)); }

    bool operator==(const StyleFillData&) const;
    bool operator!=(const StyleFillData& other) const
    {
        return !(*this == other);
    }

    float opacity;
    SVGPaintType paintType;
    Color paintColor;
    String paintUri;
    SVGPaintType visitedLinkPaintType;
    Color visitedLinkPaintColor;
    String visitedLinkPaintUri;

private:
    StyleFillData();
    StyleFillData(const StyleFillData&);
};

class UnzoomedLength {
public:
    explicit UnzoomedLength(const Length& length) : m_length(length) { }

    bool isZero() const { return m_length.isZero(); }

    bool operator==(const UnzoomedLength& other) const { return m_length == other.m_length; }
    bool operator!=(const UnzoomedLength& other) const { return !operator==(other); }

    const Length& length() const { return m_length; }

private:
    Length m_length;
};

class CORE_EXPORT StyleStrokeData : public RefCounted<StyleStrokeData> {
public:
    static PassRefPtr<StyleStrokeData> create()
    {
        return adoptRef(new StyleStrokeData);
    }

    PassRefPtr<StyleStrokeData> copy() const
    {
        return adoptRef(new StyleStrokeData(*this));
    }

    bool operator==(const StyleStrokeData&) const;
    bool operator!=(const StyleStrokeData& other) const
    {
        return !(*this == other);
    }

    float opacity;
    float miterLimit;

    UnzoomedLength width;
    Length dashOffset;
    RefPtr<SVGDashArray> dashArray;

    SVGPaintType paintType;
    Color paintColor;
    String paintUri;
    SVGPaintType visitedLinkPaintType;
    Color visitedLinkPaintColor;
    String visitedLinkPaintUri;

private:
    StyleStrokeData();
    StyleStrokeData(const StyleStrokeData&);
};

class StyleStopData : public RefCounted<StyleStopData> {
public:
    static PassRefPtr<StyleStopData> create() { return adoptRef(new StyleStopData); }
    PassRefPtr<StyleStopData> copy() const { return adoptRef(new StyleStopData(*this)); }

    bool operator==(const StyleStopData&) const;
    bool operator!=(const StyleStopData& other) const
    {
        return !(*this == other);
    }

    float opacity;
    Color color;

private:
    StyleStopData();
    StyleStopData(const StyleStopData&);
};

// Note: the rule for this class is, *no inheritance* of these props
class CORE_EXPORT StyleMiscData : public RefCounted<StyleMiscData> {
public:
    static PassRefPtr<StyleMiscData> create() { return adoptRef(new StyleMiscData); }
    PassRefPtr<StyleMiscData> copy() const { return adoptRef(new StyleMiscData(*this)); }

    bool operator==(const StyleMiscData&) const;
    bool operator!=(const StyleMiscData& other) const
    {
        return !(*this == other);
    }

    Color floodColor;
    float floodOpacity;
    Color lightingColor;

    Length baselineShiftValue;

private:
    StyleMiscData();
    StyleMiscData(const StyleMiscData&);
};

// Non-inherited resources
class StyleResourceData : public RefCounted<StyleResourceData> {
public:
    static PassRefPtr<StyleResourceData> create() { return adoptRef(new StyleResourceData); }
    PassRefPtr<StyleResourceData> copy() const { return adoptRef(new StyleResourceData(*this)); }

    bool operator==(const StyleResourceData&) const;
    bool operator!=(const StyleResourceData& other) const
    {
        return !(*this == other);
    }

    AtomicString clipper;
    AtomicString filter;
    AtomicString masker;

private:
    StyleResourceData();
    StyleResourceData(const StyleResourceData&);
};

// Inherited resources
class StyleInheritedResourceData : public RefCounted<StyleInheritedResourceData> {
public:
    static PassRefPtr<StyleInheritedResourceData> create() { return adoptRef(new StyleInheritedResourceData); }
    PassRefPtr<StyleInheritedResourceData> copy() const { return adoptRef(new StyleInheritedResourceData(*this)); }

    bool operator==(const StyleInheritedResourceData&) const;
    bool operator!=(const StyleInheritedResourceData& other) const
    {
        return !(*this == other);
    }

    AtomicString markerStart;
    AtomicString markerMid;
    AtomicString markerEnd;

private:
    StyleInheritedResourceData();
    StyleInheritedResourceData(const StyleInheritedResourceData&);
};

// Positioning and sizing properties.
class StyleLayoutData : public RefCounted<StyleLayoutData> {
    public:
        static PassRefPtr<StyleLayoutData> create() { return adoptRef(new StyleLayoutData); }
        PassRefPtr<StyleLayoutData> copy() const;
        bool operator==(const StyleLayoutData&) const;
        bool operator!=(const StyleLayoutData& other) const
        {
            return !(*this == other);
        }
        Length cx;
        Length cy;
        Length x;
        Length y;
        Length r;
        Length rx;
        Length ry;
    private:
        StyleLayoutData();
        StyleLayoutData(const StyleLayoutData&);
    };

} // namespace blink

#endif // SVGComputedStyleDefs_h
