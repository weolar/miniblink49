/**************************************************************************\
*
* Copyright (c) 1998-2000, Microsoft Corp.  All Rights Reserved.
*
* Module Name:
*
*   GdiplusEnums.h
*
* Abstract:
*
*   Various enumeration types
*
\**************************************************************************/

#ifndef _GDIPLUSENUMS_H
#define _GDIPLUSENUMS_H


//--------------------------------------------------------------------------
// Default bezier flattening tolerance in device pixels.
//--------------------------------------------------------------------------

const float FlatnessDefault = 1.0f/4.0f;

//--------------------------------------------------------------------------
// Graphics and Container State cookies
//--------------------------------------------------------------------------

typedef UINT     GraphicsState;
typedef UINT     GraphicsContainer;

//--------------------------------------------------------------------------
// Fill mode constants
//--------------------------------------------------------------------------

enum FillMode
{
    FillModeAlternate,        // 0
    FillModeWinding           // 1
};

//--------------------------------------------------------------------------
// Quality mode constants
//--------------------------------------------------------------------------

enum QualityMode
{
    QualityModeInvalid   = -1,
    QualityModeDefault   = 0,
    QualityModeLow       = 1, // for apps that need the best performance
    QualityModeHigh      = 2  // for apps that need the best rendering quality
};

//--------------------------------------------------------------------------
// Alpha compositing mode constants
//--------------------------------------------------------------------------

enum CompositingMode
{
    CompositingModeSourceOver,    // 0
    CompositingModeSourceCopy     // 1
};

//--------------------------------------------------------------------------
// Alpha compositing quality constants
//--------------------------------------------------------------------------

enum CompositingQuality
{
    CompositingQualityInvalid          = QualityModeInvalid,
    CompositingQualityDefault          = QualityModeDefault,
    CompositingQualityHighSpeed        = QualityModeLow,
    CompositingQualityHighQuality      = QualityModeHigh,
    CompositingQualityGammaCorrected,
    CompositingQualityAssumeLinear
};

//--------------------------------------------------------------------------
// Unit constants
//--------------------------------------------------------------------------

enum Unit
{
    UnitWorld,      // 0 -- World coordinate (non-physical unit)
    UnitDisplay,    // 1 -- Variable -- for PageTransform only
    UnitPixel,      // 2 -- Each unit is one device pixel.
    UnitPoint,      // 3 -- Each unit is a printer's point, or 1/72 inch.
    UnitInch,       // 4 -- Each unit is 1 inch.
    UnitDocument,   // 5 -- Each unit is 1/300 inch.
    UnitMillimeter  // 6 -- Each unit is 1 millimeter.
};

//--------------------------------------------------------------------------
// MetafileFrameUnit
//
// The frameRect for creating a metafile can be specified in any of these
// units.  There is an extra frame unit value (MetafileFrameUnitGdi) so
// that units can be supplied in the same units that GDI expects for
// frame rects -- these units are in .01 (1/100ths) millimeter units
// as defined by GDI.
//--------------------------------------------------------------------------
enum MetafileFrameUnit
{
    MetafileFrameUnitPixel      = UnitPixel,
    MetafileFrameUnitPoint      = UnitPoint,
    MetafileFrameUnitInch       = UnitInch,
    MetafileFrameUnitDocument   = UnitDocument,
    MetafileFrameUnitMillimeter = UnitMillimeter,
    MetafileFrameUnitGdi                        // GDI compatible .01 MM units
};

//--------------------------------------------------------------------------
// Coordinate space identifiers
//--------------------------------------------------------------------------

enum CoordinateSpace
{
    CoordinateSpaceWorld,     // 0
    CoordinateSpacePage,      // 1
    CoordinateSpaceDevice     // 2
};

//--------------------------------------------------------------------------
// Various wrap modes for brushes
//--------------------------------------------------------------------------

enum WrapMode
{
    WrapModeTile,        // 0
    WrapModeTileFlipX,   // 1
    WrapModeTileFlipY,   // 2
    WrapModeTileFlipXY,  // 3
    WrapModeClamp        // 4
};

//--------------------------------------------------------------------------
// Various hatch styles
//--------------------------------------------------------------------------

enum HatchStyle
{
    HatchStyleHorizontal,                   // 0
    HatchStyleVertical,                     // 1
    HatchStyleForwardDiagonal,              // 2
    HatchStyleBackwardDiagonal,             // 3
    HatchStyleCross,                        // 4
    HatchStyleDiagonalCross,                // 5
    HatchStyle05Percent,                    // 6
    HatchStyle10Percent,                    // 7
    HatchStyle20Percent,                    // 8
    HatchStyle25Percent,                    // 9
    HatchStyle30Percent,                    // 10
    HatchStyle40Percent,                    // 11
    HatchStyle50Percent,                    // 12
    HatchStyle60Percent,                    // 13
    HatchStyle70Percent,                    // 14
    HatchStyle75Percent,                    // 15
    HatchStyle80Percent,                    // 16
    HatchStyle90Percent,                    // 17
    HatchStyleLightDownwardDiagonal,        // 18
    HatchStyleLightUpwardDiagonal,          // 19
    HatchStyleDarkDownwardDiagonal,         // 20
    HatchStyleDarkUpwardDiagonal,           // 21
    HatchStyleWideDownwardDiagonal,         // 22
    HatchStyleWideUpwardDiagonal,           // 23
    HatchStyleLightVertical,                // 24
    HatchStyleLightHorizontal,              // 25
    HatchStyleNarrowVertical,               // 26
    HatchStyleNarrowHorizontal,             // 27
    HatchStyleDarkVertical,                 // 28
    HatchStyleDarkHorizontal,               // 29
    HatchStyleDashedDownwardDiagonal,       // 30
    HatchStyleDashedUpwardDiagonal,         // 31
    HatchStyleDashedHorizontal,             // 32
    HatchStyleDashedVertical,               // 33
    HatchStyleSmallConfetti,                // 34
    HatchStyleLargeConfetti,                // 35
    HatchStyleZigZag,                       // 36
    HatchStyleWave,                         // 37
    HatchStyleDiagonalBrick,                // 38
    HatchStyleHorizontalBrick,              // 39
    HatchStyleWeave,                        // 40
    HatchStylePlaid,                        // 41
    HatchStyleDivot,                        // 42
    HatchStyleDottedGrid,                   // 43
    HatchStyleDottedDiamond,                // 44
    HatchStyleShingle,                      // 45
    HatchStyleTrellis,                      // 46
    HatchStyleSphere,                       // 47
    HatchStyleSmallGrid,                    // 48
    HatchStyleSmallCheckerBoard,            // 49
    HatchStyleLargeCheckerBoard,            // 50
    HatchStyleOutlinedDiamond,              // 51
    HatchStyleSolidDiamond,                 // 52

    HatchStyleTotal,    // must be after all unique hatch styles

    HatchStyleLargeGrid = HatchStyleCross,  // 4  an alias for the cross style

    HatchStyleMin       = HatchStyleHorizontal,
    HatchStyleMax       = HatchStyleTotal - 1,
};

//--------------------------------------------------------------------------
// Dash style constants
//--------------------------------------------------------------------------

enum DashStyle
{
    DashStyleSolid,          // 0
    DashStyleDash,           // 1
    DashStyleDot,            // 2
    DashStyleDashDot,        // 3
    DashStyleDashDotDot,     // 4
    DashStyleCustom          // 5
};

//--------------------------------------------------------------------------
// Dash cap constants
//--------------------------------------------------------------------------

enum DashCap
{
    DashCapFlat             = 0,
    DashCapRound            = 2,
    DashCapTriangle         = 3
};

//--------------------------------------------------------------------------
// Line cap constants (only the lowest 8 bits are used).
//--------------------------------------------------------------------------

enum LineCap
{
    LineCapFlat             = 0,
    LineCapSquare           = 1,
    LineCapRound            = 2,
    LineCapTriangle         = 3,

    LineCapNoAnchor         = 0x10, // corresponds to flat cap
    LineCapSquareAnchor     = 0x11, // corresponds to square cap
    LineCapRoundAnchor      = 0x12, // corresponds to round cap
    LineCapDiamondAnchor    = 0x13, // corresponds to triangle cap
    LineCapArrowAnchor      = 0x14, // no correspondence

    LineCapCustom           = 0xff, // custom cap

    LineCapAnchorMask       = 0xf0  // mask to check for anchor or not.
};

//--------------------------------------------------------------------------
// Custom Line cap type constants
//--------------------------------------------------------------------------

enum CustomLineCapType
{
    CustomLineCapTypeDefault         = 0,
    CustomLineCapTypeAdjustableArrow = 1
};

//--------------------------------------------------------------------------
// Line join constants
//--------------------------------------------------------------------------

enum LineJoin
{
    LineJoinMiter        = 0,
    LineJoinBevel        = 1,
    LineJoinRound        = 2,
    LineJoinMiterClipped = 3
};

//--------------------------------------------------------------------------
// Path point types (only the lowest 8 bits are used.)
//  The lowest 3 bits are interpreted as point type
//  The higher 5 bits are reserved for flags.
//--------------------------------------------------------------------------

enum PathPointType
{
    PathPointTypeStart           = 0,    // move
    PathPointTypeLine            = 1,    // line
    PathPointTypeBezier          = 3,    // default Beizer (= cubic Bezier)
    PathPointTypePathTypeMask    = 0x07, // type mask (lowest 3 bits).
    PathPointTypeDashMode        = 0x10, // currently in dash mode.
    PathPointTypePathMarker      = 0x20, // a marker for the path.
    PathPointTypeCloseSubpath    = 0x80, // closed flag

    // Path types used for advanced path.

    PathPointTypeBezier2    = 2,    // quadratic Beizer
    PathPointTypeBezier3    = 3,    // cubic Bezier
    PathPointTypeBezier4    = 4,    // quartic (4th order) Beizer
    PathPointTypeBezier5    = 5,    // quintic (5th order) Bezier
    PathPointTypeBezier6    = 6     // hexaic (6th order) Bezier
};


//--------------------------------------------------------------------------
// WarpMode constants
//--------------------------------------------------------------------------

enum WarpMode
{
    WarpModePerspective,    // 0
    WarpModeBilinear        // 1
};

//--------------------------------------------------------------------------
// LineGradient Mode
//--------------------------------------------------------------------------

enum LinearGradientMode
{
    LinearGradientModeHorizontal,         // 0
    LinearGradientModeVertical,           // 1
    LinearGradientModeForwardDiagonal,    // 2
    LinearGradientModeBackwardDiagonal    // 3
};

//--------------------------------------------------------------------------
// Region Comine Modes
//--------------------------------------------------------------------------

enum CombineMode
{
    CombineModeReplace,     // 0
    CombineModeIntersect,   // 1
    CombineModeUnion,       // 2
    CombineModeXor,         // 3
    CombineModeExclude,     // 4
    CombineModeComplement   // 5 (does exclude from)
};

//--------------------------------------------------------------------------
 // Image types
//--------------------------------------------------------------------------

enum ImageType
{
    ImageTypeUnknown,   // 0
    ImageTypeBitmap,    // 1
    ImageTypeMetafile   // 2
};

//--------------------------------------------------------------------------
// Interpolation modes
//--------------------------------------------------------------------------

enum InterpolationMode
{
    InterpolationModeInvalid          = QualityModeInvalid,
    InterpolationModeDefault          = QualityModeDefault,
    InterpolationModeLowQuality       = QualityModeLow,
    InterpolationModeHighQuality      = QualityModeHigh,
    InterpolationModeBilinear,
    InterpolationModeBicubic,
    InterpolationModeNearestNeighbor,
    InterpolationModeHighQualityBilinear,
    InterpolationModeHighQualityBicubic
};

//--------------------------------------------------------------------------
// Pen types
//--------------------------------------------------------------------------
enum PenAlignment
{
    PenAlignmentCenter       = 0,
    PenAlignmentInset        = 1,
    PenAlignmentOutset       = 2,
    PenAlignmentLeft         = 3,
    PenAlignmentRight        = 4
};

//--------------------------------------------------------------------------
// Brush types
//--------------------------------------------------------------------------

enum BrushType
{
   BrushTypeSolidColor       = 0,
   BrushTypeHatchFill        = 1,
   BrushTypeTextureFill      = 2,
   BrushTypePathGradient     = 3,
   BrushTypeLinearGradient   = 4
};

//--------------------------------------------------------------------------
// Pen's Fill types
//--------------------------------------------------------------------------

enum PenType
{
   PenTypeSolidColor       = BrushTypeSolidColor,
   PenTypeHatchFill        = BrushTypeHatchFill,
   PenTypeTextureFill      = BrushTypeTextureFill,
   PenTypePathGradient     = BrushTypePathGradient,
   PenTypeLinearGradient   = BrushTypeLinearGradient,
   PenTypeUnknown          = -1
};

//--------------------------------------------------------------------------
// Matrix Order
//--------------------------------------------------------------------------

enum MatrixOrder
{
    MatrixOrderPrepend    = 0,
    MatrixOrderAppend     = 1
};

//--------------------------------------------------------------------------
// Generic font families
//--------------------------------------------------------------------------

enum GenericFontFamily
{
    GenericFontFamilySerif,
    GenericFontFamilySansSerif,
    GenericFontFamilyMonospace

};

//--------------------------------------------------------------------------
// FontStyle: face types and common styles
//--------------------------------------------------------------------------

//  These should probably be flags

//  Must have:
//      Regular = 0
//      Bold = 1
//      Italic = 2
//      BoldItalic = 3

enum FontStyle
{
    FontStyleRegular    = 0,
    FontStyleBold       = 1,
    FontStyleItalic     = 2,
    FontStyleBoldItalic = 3,
    FontStyleUnderline  = 4,
    FontStyleStrikeout  = 8
};

//---------------------------------------------------------------------------
// Smoothing Mode
//---------------------------------------------------------------------------

enum SmoothingMode
{
    SmoothingModeInvalid     = QualityModeInvalid,
    SmoothingModeDefault     = QualityModeDefault,
    SmoothingModeHighSpeed   = QualityModeLow,
    SmoothingModeHighQuality = QualityModeHigh,
    SmoothingModeNone,
    SmoothingModeAntiAlias
};

//---------------------------------------------------------------------------
// Pixel Format Mode
//---------------------------------------------------------------------------

enum PixelOffsetMode
{
    PixelOffsetModeInvalid     = QualityModeInvalid,
    PixelOffsetModeDefault     = QualityModeDefault,
    PixelOffsetModeHighSpeed   = QualityModeLow,
    PixelOffsetModeHighQuality = QualityModeHigh,
    PixelOffsetModeNone,    // no pixel offset
    PixelOffsetModeHalf     // offset by -0.5, -0.5 for fast anti-alias perf
};

//---------------------------------------------------------------------------
// Text Rendering Hint
//---------------------------------------------------------------------------

enum TextRenderingHint
{
#ifdef DCR_USE_NEW_186764
    TextRenderingHintSystemDefault = 0,            // Glyph with system default rendering hint
    TextRenderingHintSingleBitPerPixelGridFit,     // Glyph bitmap with hinting
#else
    TextRenderingHintSingleBitPerPixelGridFit = 0, // Glyph bitmap with hinting
#endif // DCR_USE_NEW_186764
    TextRenderingHintSingleBitPerPixel,            // Glyph bitmap without hinting
    TextRenderingHintAntiAliasGridFit,             // Glyph anti-alias bitmap with hinting
    TextRenderingHintAntiAlias,                    // Glyph anti-alias bitmap without hinting
    TextRenderingHintClearTypeGridFit              // Glyph CT bitmap with hinting
};

//---------------------------------------------------------------------------
// Metafile Types
//---------------------------------------------------------------------------
enum MetafileType
{
    MetafileTypeInvalid,            // Invalid metafile
    MetafileTypeWmf,                // Standard WMF
    MetafileTypeWmfAldus,           // Aldus Placeable Metafile format
    MetafileTypeEmf,                // EMF (not EMF+)
    MetafileTypeEmfPlusOnly,        // EMF+ without dual, down-level records
    MetafileTypeEmfPlusDual         // EMF+ with dual, down-level records
};

// Specifies the type of EMF to record
enum EmfType
{
    EmfTypeEmfOnly     = MetafileTypeEmf,          // no EMF+, only EMF
    EmfTypeEmfPlusOnly = MetafileTypeEmfPlusOnly,  // no EMF, only EMF+
    EmfTypeEmfPlusDual = MetafileTypeEmfPlusDual   // both EMF+ and EMF
};

// All persistent objects must have a type listed here
enum ObjectType
{
    ObjectTypeInvalid,
    ObjectTypeBrush,
    ObjectTypePen,
    ObjectTypePath,
    ObjectTypeRegion,
    ObjectTypeImage,
    ObjectTypeFont,
    ObjectTypeStringFormat,
    ObjectTypeImageAttributes,
    ObjectTypeCustomLineCap,

    ObjectTypeMax = ObjectTypeCustomLineCap,
    ObjectTypeMin = ObjectTypeBrush
};

inline BOOL
ObjectTypeIsValid(
    ObjectType      type
    )
{
    return ((type >= ObjectTypeMin) && (type <= ObjectTypeMax));
}

//---------------------------------------------------------------------------
// EMF+ Records
//---------------------------------------------------------------------------

// We have to change the WMF record numbers so that they don't conflict with
// the EMF and EMF+ record numbers.
enum EmfPlusRecordType;
#define GDIP_EMFPLUS_RECORD_BASE        0x00004000
#define GDIP_WMF_RECORD_BASE            0x00010000
#define GDIP_WMF_RECORD_TO_EMFPLUS(n)   ((EmfPlusRecordType)((n) | GDIP_WMF_RECORD_BASE))
#define GDIP_EMFPLUS_RECORD_TO_WMF(n)   ((n) & (~GDIP_WMF_RECORD_BASE))
#define GDIP_IS_WMF_RECORDTYPE(n)       (((n) & GDIP_WMF_RECORD_BASE) != 0)

enum EmfPlusRecordType
{
   // Since we have to enumerate GDI records right along with GDI+ records,
   // we list all the GDI records here so that they can be part of the
   // same enumeration type which is used in the enumeration callback.

    WmfRecordTypeSetBkColor              = GDIP_WMF_RECORD_TO_EMFPLUS(META_SETBKCOLOR),
    WmfRecordTypeSetBkMode               = GDIP_WMF_RECORD_TO_EMFPLUS(META_SETBKMODE),
    WmfRecordTypeSetMapMode              = GDIP_WMF_RECORD_TO_EMFPLUS(META_SETMAPMODE),
    WmfRecordTypeSetROP2                 = GDIP_WMF_RECORD_TO_EMFPLUS(META_SETROP2),
    WmfRecordTypeSetRelAbs               = GDIP_WMF_RECORD_TO_EMFPLUS(META_SETRELABS),
    WmfRecordTypeSetPolyFillMode         = GDIP_WMF_RECORD_TO_EMFPLUS(META_SETPOLYFILLMODE),
    WmfRecordTypeSetStretchBltMode       = GDIP_WMF_RECORD_TO_EMFPLUS(META_SETSTRETCHBLTMODE),
    WmfRecordTypeSetTextCharExtra        = GDIP_WMF_RECORD_TO_EMFPLUS(META_SETTEXTCHAREXTRA),
    WmfRecordTypeSetTextColor            = GDIP_WMF_RECORD_TO_EMFPLUS(META_SETTEXTCOLOR),
    WmfRecordTypeSetTextJustification    = GDIP_WMF_RECORD_TO_EMFPLUS(META_SETTEXTJUSTIFICATION),
    WmfRecordTypeSetWindowOrg            = GDIP_WMF_RECORD_TO_EMFPLUS(META_SETWINDOWORG),
    WmfRecordTypeSetWindowExt            = GDIP_WMF_RECORD_TO_EMFPLUS(META_SETWINDOWEXT),
    WmfRecordTypeSetViewportOrg          = GDIP_WMF_RECORD_TO_EMFPLUS(META_SETVIEWPORTORG),
    WmfRecordTypeSetViewportExt          = GDIP_WMF_RECORD_TO_EMFPLUS(META_SETVIEWPORTEXT),
    WmfRecordTypeOffsetWindowOrg         = GDIP_WMF_RECORD_TO_EMFPLUS(META_OFFSETWINDOWORG),
    WmfRecordTypeScaleWindowExt          = GDIP_WMF_RECORD_TO_EMFPLUS(META_SCALEWINDOWEXT),
    WmfRecordTypeOffsetViewportOrg       = GDIP_WMF_RECORD_TO_EMFPLUS(META_OFFSETVIEWPORTORG),
    WmfRecordTypeScaleViewportExt        = GDIP_WMF_RECORD_TO_EMFPLUS(META_SCALEVIEWPORTEXT),
    WmfRecordTypeLineTo                  = GDIP_WMF_RECORD_TO_EMFPLUS(META_LINETO),
    WmfRecordTypeMoveTo                  = GDIP_WMF_RECORD_TO_EMFPLUS(META_MOVETO),
    WmfRecordTypeExcludeClipRect         = GDIP_WMF_RECORD_TO_EMFPLUS(META_EXCLUDECLIPRECT),
    WmfRecordTypeIntersectClipRect       = GDIP_WMF_RECORD_TO_EMFPLUS(META_INTERSECTCLIPRECT),
    WmfRecordTypeArc                     = GDIP_WMF_RECORD_TO_EMFPLUS(META_ARC),
    WmfRecordTypeEllipse                 = GDIP_WMF_RECORD_TO_EMFPLUS(META_ELLIPSE),
    WmfRecordTypeFloodFill               = GDIP_WMF_RECORD_TO_EMFPLUS(META_FLOODFILL),
    WmfRecordTypePie                     = GDIP_WMF_RECORD_TO_EMFPLUS(META_PIE),
    WmfRecordTypeRectangle               = GDIP_WMF_RECORD_TO_EMFPLUS(META_RECTANGLE),
    WmfRecordTypeRoundRect               = GDIP_WMF_RECORD_TO_EMFPLUS(META_ROUNDRECT),
    WmfRecordTypePatBlt                  = GDIP_WMF_RECORD_TO_EMFPLUS(META_PATBLT),
    WmfRecordTypeSaveDC                  = GDIP_WMF_RECORD_TO_EMFPLUS(META_SAVEDC),
    WmfRecordTypeSetPixel                = GDIP_WMF_RECORD_TO_EMFPLUS(META_SETPIXEL),
    WmfRecordTypeOffsetClipRgn           = GDIP_WMF_RECORD_TO_EMFPLUS(META_OFFSETCLIPRGN),
    WmfRecordTypeTextOut                 = GDIP_WMF_RECORD_TO_EMFPLUS(META_TEXTOUT),
    WmfRecordTypeBitBlt                  = GDIP_WMF_RECORD_TO_EMFPLUS(META_BITBLT),
    WmfRecordTypeStretchBlt              = GDIP_WMF_RECORD_TO_EMFPLUS(META_STRETCHBLT),
    WmfRecordTypePolygon                 = GDIP_WMF_RECORD_TO_EMFPLUS(META_POLYGON),
    WmfRecordTypePolyline                = GDIP_WMF_RECORD_TO_EMFPLUS(META_POLYLINE),
    WmfRecordTypeEscape                  = GDIP_WMF_RECORD_TO_EMFPLUS(META_ESCAPE),
    WmfRecordTypeRestoreDC               = GDIP_WMF_RECORD_TO_EMFPLUS(META_RESTOREDC),
    WmfRecordTypeFillRegion              = GDIP_WMF_RECORD_TO_EMFPLUS(META_FILLREGION),
    WmfRecordTypeFrameRegion             = GDIP_WMF_RECORD_TO_EMFPLUS(META_FRAMEREGION),
    WmfRecordTypeInvertRegion            = GDIP_WMF_RECORD_TO_EMFPLUS(META_INVERTREGION),
    WmfRecordTypePaintRegion             = GDIP_WMF_RECORD_TO_EMFPLUS(META_PAINTREGION),
    WmfRecordTypeSelectClipRegion        = GDIP_WMF_RECORD_TO_EMFPLUS(META_SELECTCLIPREGION),
    WmfRecordTypeSelectObject            = GDIP_WMF_RECORD_TO_EMFPLUS(META_SELECTOBJECT),
    WmfRecordTypeSetTextAlign            = GDIP_WMF_RECORD_TO_EMFPLUS(META_SETTEXTALIGN),
    WmfRecordTypeDrawText                = GDIP_WMF_RECORD_TO_EMFPLUS(0x062F),  // META_DRAWTEXT
    WmfRecordTypeChord                   = GDIP_WMF_RECORD_TO_EMFPLUS(META_CHORD),
    WmfRecordTypeSetMapperFlags          = GDIP_WMF_RECORD_TO_EMFPLUS(META_SETMAPPERFLAGS),
    WmfRecordTypeExtTextOut              = GDIP_WMF_RECORD_TO_EMFPLUS(META_EXTTEXTOUT),
    WmfRecordTypeSetDIBToDev             = GDIP_WMF_RECORD_TO_EMFPLUS(META_SETDIBTODEV),
    WmfRecordTypeSelectPalette           = GDIP_WMF_RECORD_TO_EMFPLUS(META_SELECTPALETTE),
    WmfRecordTypeRealizePalette          = GDIP_WMF_RECORD_TO_EMFPLUS(META_REALIZEPALETTE),
    WmfRecordTypeAnimatePalette          = GDIP_WMF_RECORD_TO_EMFPLUS(META_ANIMATEPALETTE),
    WmfRecordTypeSetPalEntries           = GDIP_WMF_RECORD_TO_EMFPLUS(META_SETPALENTRIES),
    WmfRecordTypePolyPolygon             = GDIP_WMF_RECORD_TO_EMFPLUS(META_POLYPOLYGON),
    WmfRecordTypeResizePalette           = GDIP_WMF_RECORD_TO_EMFPLUS(META_RESIZEPALETTE),
    WmfRecordTypeDIBBitBlt               = GDIP_WMF_RECORD_TO_EMFPLUS(META_DIBBITBLT),
    WmfRecordTypeDIBStretchBlt           = GDIP_WMF_RECORD_TO_EMFPLUS(META_DIBSTRETCHBLT),
    WmfRecordTypeDIBCreatePatternBrush   = GDIP_WMF_RECORD_TO_EMFPLUS(META_DIBCREATEPATTERNBRUSH),
    WmfRecordTypeStretchDIB              = GDIP_WMF_RECORD_TO_EMFPLUS(META_STRETCHDIB),
    WmfRecordTypeExtFloodFill            = GDIP_WMF_RECORD_TO_EMFPLUS(META_EXTFLOODFILL),
    WmfRecordTypeSetLayout               = GDIP_WMF_RECORD_TO_EMFPLUS(0x0149),  // META_SETLAYOUT
    WmfRecordTypeResetDC                 = GDIP_WMF_RECORD_TO_EMFPLUS(0x014C),  // META_RESETDC
    WmfRecordTypeStartDoc                = GDIP_WMF_RECORD_TO_EMFPLUS(0x014D),  // META_STARTDOC
    WmfRecordTypeStartPage               = GDIP_WMF_RECORD_TO_EMFPLUS(0x004F),  // META_STARTPAGE
    WmfRecordTypeEndPage                 = GDIP_WMF_RECORD_TO_EMFPLUS(0x0050),  // META_ENDPAGE
    WmfRecordTypeAbortDoc                = GDIP_WMF_RECORD_TO_EMFPLUS(0x0052),  // META_ABORTDOC
    WmfRecordTypeEndDoc                  = GDIP_WMF_RECORD_TO_EMFPLUS(0x005E),  // META_ENDDOC
    WmfRecordTypeDeleteObject            = GDIP_WMF_RECORD_TO_EMFPLUS(META_DELETEOBJECT),
    WmfRecordTypeCreatePalette           = GDIP_WMF_RECORD_TO_EMFPLUS(META_CREATEPALETTE),
    WmfRecordTypeCreateBrush             = GDIP_WMF_RECORD_TO_EMFPLUS(0x00F8),  // META_CREATEBRUSH
    WmfRecordTypeCreatePatternBrush      = GDIP_WMF_RECORD_TO_EMFPLUS(META_CREATEPATTERNBRUSH),
    WmfRecordTypeCreatePenIndirect       = GDIP_WMF_RECORD_TO_EMFPLUS(META_CREATEPENINDIRECT),
    WmfRecordTypeCreateFontIndirect      = GDIP_WMF_RECORD_TO_EMFPLUS(META_CREATEFONTINDIRECT),
    WmfRecordTypeCreateBrushIndirect     = GDIP_WMF_RECORD_TO_EMFPLUS(META_CREATEBRUSHINDIRECT),
    WmfRecordTypeCreateBitmapIndirect    = GDIP_WMF_RECORD_TO_EMFPLUS(0x02FD),  // META_CREATEBITMAPINDIRECT
    WmfRecordTypeCreateBitmap            = GDIP_WMF_RECORD_TO_EMFPLUS(0x06FE),  // META_CREATEBITMAP
    WmfRecordTypeCreateRegion            = GDIP_WMF_RECORD_TO_EMFPLUS(META_CREATEREGION),

    EmfRecordTypeHeader                  = EMR_HEADER,
    EmfRecordTypePolyBezier              = EMR_POLYBEZIER,
    EmfRecordTypePolygon                 = EMR_POLYGON,
    EmfRecordTypePolyline                = EMR_POLYLINE,
    EmfRecordTypePolyBezierTo            = EMR_POLYBEZIERTO,
    EmfRecordTypePolyLineTo              = EMR_POLYLINETO,
    EmfRecordTypePolyPolyline            = EMR_POLYPOLYLINE,
    EmfRecordTypePolyPolygon             = EMR_POLYPOLYGON,
    EmfRecordTypeSetWindowExtEx          = EMR_SETWINDOWEXTEX,
    EmfRecordTypeSetWindowOrgEx          = EMR_SETWINDOWORGEX,
    EmfRecordTypeSetViewportExtEx        = EMR_SETVIEWPORTEXTEX,
    EmfRecordTypeSetViewportOrgEx        = EMR_SETVIEWPORTORGEX,
    EmfRecordTypeSetBrushOrgEx           = EMR_SETBRUSHORGEX,
    EmfRecordTypeEOF                     = EMR_EOF,
    EmfRecordTypeSetPixelV               = EMR_SETPIXELV,
    EmfRecordTypeSetMapperFlags          = EMR_SETMAPPERFLAGS,
    EmfRecordTypeSetMapMode              = EMR_SETMAPMODE,
    EmfRecordTypeSetBkMode               = EMR_SETBKMODE,
    EmfRecordTypeSetPolyFillMode         = EMR_SETPOLYFILLMODE,
    EmfRecordTypeSetROP2                 = EMR_SETROP2,
    EmfRecordTypeSetStretchBltMode       = EMR_SETSTRETCHBLTMODE,
    EmfRecordTypeSetTextAlign            = EMR_SETTEXTALIGN,
    EmfRecordTypeSetColorAdjustment      = EMR_SETCOLORADJUSTMENT,
    EmfRecordTypeSetTextColor            = EMR_SETTEXTCOLOR,
    EmfRecordTypeSetBkColor              = EMR_SETBKCOLOR,
    EmfRecordTypeOffsetClipRgn           = EMR_OFFSETCLIPRGN,
    EmfRecordTypeMoveToEx                = EMR_MOVETOEX,
    EmfRecordTypeSetMetaRgn              = EMR_SETMETARGN,
    EmfRecordTypeExcludeClipRect         = EMR_EXCLUDECLIPRECT,
    EmfRecordTypeIntersectClipRect       = EMR_INTERSECTCLIPRECT,
    EmfRecordTypeScaleViewportExtEx      = EMR_SCALEVIEWPORTEXTEX,
    EmfRecordTypeScaleWindowExtEx        = EMR_SCALEWINDOWEXTEX,
    EmfRecordTypeSaveDC                  = EMR_SAVEDC,
    EmfRecordTypeRestoreDC               = EMR_RESTOREDC,
    EmfRecordTypeSetWorldTransform       = EMR_SETWORLDTRANSFORM,
    EmfRecordTypeModifyWorldTransform    = EMR_MODIFYWORLDTRANSFORM,
    EmfRecordTypeSelectObject            = EMR_SELECTOBJECT,
    EmfRecordTypeCreatePen               = EMR_CREATEPEN,
    EmfRecordTypeCreateBrushIndirect     = EMR_CREATEBRUSHINDIRECT,
    EmfRecordTypeDeleteObject            = EMR_DELETEOBJECT,
    EmfRecordTypeAngleArc                = EMR_ANGLEARC,
    EmfRecordTypeEllipse                 = EMR_ELLIPSE,
    EmfRecordTypeRectangle               = EMR_RECTANGLE,
    EmfRecordTypeRoundRect               = EMR_ROUNDRECT,
    EmfRecordTypeArc                     = EMR_ARC,
    EmfRecordTypeChord                   = EMR_CHORD,
    EmfRecordTypePie                     = EMR_PIE,
    EmfRecordTypeSelectPalette           = EMR_SELECTPALETTE,
    EmfRecordTypeCreatePalette           = EMR_CREATEPALETTE,
    EmfRecordTypeSetPaletteEntries       = EMR_SETPALETTEENTRIES,
    EmfRecordTypeResizePalette           = EMR_RESIZEPALETTE,
    EmfRecordTypeRealizePalette          = EMR_REALIZEPALETTE,
    EmfRecordTypeExtFloodFill            = EMR_EXTFLOODFILL,
    EmfRecordTypeLineTo                  = EMR_LINETO,
    EmfRecordTypeArcTo                   = EMR_ARCTO,
    EmfRecordTypePolyDraw                = EMR_POLYDRAW,
    EmfRecordTypeSetArcDirection         = EMR_SETARCDIRECTION,
    EmfRecordTypeSetMiterLimit           = EMR_SETMITERLIMIT,
    EmfRecordTypeBeginPath               = EMR_BEGINPATH,
    EmfRecordTypeEndPath                 = EMR_ENDPATH,
    EmfRecordTypeCloseFigure             = EMR_CLOSEFIGURE,
    EmfRecordTypeFillPath                = EMR_FILLPATH,
    EmfRecordTypeStrokeAndFillPath       = EMR_STROKEANDFILLPATH,
    EmfRecordTypeStrokePath              = EMR_STROKEPATH,
    EmfRecordTypeFlattenPath             = EMR_FLATTENPATH,
    EmfRecordTypeWidenPath               = EMR_WIDENPATH,
    EmfRecordTypeSelectClipPath          = EMR_SELECTCLIPPATH,
    EmfRecordTypeAbortPath               = EMR_ABORTPATH,
    EmfRecordTypeReserved_069            = 69,  // Not Used
    EmfRecordTypeGdiComment              = EMR_GDICOMMENT,
    EmfRecordTypeFillRgn                 = EMR_FILLRGN,
    EmfRecordTypeFrameRgn                = EMR_FRAMERGN,
    EmfRecordTypeInvertRgn               = EMR_INVERTRGN,
    EmfRecordTypePaintRgn                = EMR_PAINTRGN,
    EmfRecordTypeExtSelectClipRgn        = EMR_EXTSELECTCLIPRGN,
    EmfRecordTypeBitBlt                  = EMR_BITBLT,
    EmfRecordTypeStretchBlt              = EMR_STRETCHBLT,
    EmfRecordTypeMaskBlt                 = EMR_MASKBLT,
    EmfRecordTypePlgBlt                  = EMR_PLGBLT,
    EmfRecordTypeSetDIBitsToDevice       = EMR_SETDIBITSTODEVICE,
    EmfRecordTypeStretchDIBits           = EMR_STRETCHDIBITS,
    EmfRecordTypeExtCreateFontIndirect   = EMR_EXTCREATEFONTINDIRECTW,
    EmfRecordTypeExtTextOutA             = EMR_EXTTEXTOUTA,
    EmfRecordTypeExtTextOutW             = EMR_EXTTEXTOUTW,
    EmfRecordTypePolyBezier16            = EMR_POLYBEZIER16,
    EmfRecordTypePolygon16               = EMR_POLYGON16,
    EmfRecordTypePolyline16              = EMR_POLYLINE16,
    EmfRecordTypePolyBezierTo16          = EMR_POLYBEZIERTO16,
    EmfRecordTypePolylineTo16            = EMR_POLYLINETO16,
    EmfRecordTypePolyPolyline16          = EMR_POLYPOLYLINE16,
    EmfRecordTypePolyPolygon16           = EMR_POLYPOLYGON16,
    EmfRecordTypePolyDraw16              = EMR_POLYDRAW16,
    EmfRecordTypeCreateMonoBrush         = EMR_CREATEMONOBRUSH,
    EmfRecordTypeCreateDIBPatternBrushPt = EMR_CREATEDIBPATTERNBRUSHPT,
    EmfRecordTypeExtCreatePen            = EMR_EXTCREATEPEN,
    EmfRecordTypePolyTextOutA            = EMR_POLYTEXTOUTA,
    EmfRecordTypePolyTextOutW            = EMR_POLYTEXTOUTW,
    EmfRecordTypeSetICMMode              = 98,  // EMR_SETICMMODE,
    EmfRecordTypeCreateColorSpace        = 99,  // EMR_CREATECOLORSPACE,
    EmfRecordTypeSetColorSpace           = 100, // EMR_SETCOLORSPACE,
    EmfRecordTypeDeleteColorSpace        = 101, // EMR_DELETECOLORSPACE,
    EmfRecordTypeGLSRecord               = 102, // EMR_GLSRECORD,
    EmfRecordTypeGLSBoundedRecord        = 103, // EMR_GLSBOUNDEDRECORD,
    EmfRecordTypePixelFormat             = 104, // EMR_PIXELFORMAT,
    EmfRecordTypeDrawEscape              = 105, // EMR_RESERVED_105,
    EmfRecordTypeExtEscape               = 106, // EMR_RESERVED_106,
    EmfRecordTypeStartDoc                = 107, // EMR_RESERVED_107,
    EmfRecordTypeSmallTextOut            = 108, // EMR_RESERVED_108,
    EmfRecordTypeForceUFIMapping         = 109, // EMR_RESERVED_109,
    EmfRecordTypeNamedEscape             = 110, // EMR_RESERVED_110,
    EmfRecordTypeColorCorrectPalette     = 111, // EMR_COLORCORRECTPALETTE,
    EmfRecordTypeSetICMProfileA          = 112, // EMR_SETICMPROFILEA,
    EmfRecordTypeSetICMProfileW          = 113, // EMR_SETICMPROFILEW,
    EmfRecordTypeAlphaBlend              = 114, // EMR_ALPHABLEND,
    EmfRecordTypeSetLayout               = 115, // EMR_SETLAYOUT,
    EmfRecordTypeTransparentBlt          = 116, // EMR_TRANSPARENTBLT,
    EmfRecordTypeReserved_117            = 117, // Not Used
    EmfRecordTypeGradientFill            = 118, // EMR_GRADIENTFILL,
    EmfRecordTypeSetLinkedUFIs           = 119, // EMR_RESERVED_119,
    EmfRecordTypeSetTextJustification    = 120, // EMR_RESERVED_120,
    EmfRecordTypeColorMatchToTargetW     = 121, // EMR_COLORMATCHTOTARGETW,
    EmfRecordTypeCreateColorSpaceW       = 122, // EMR_CREATECOLORSPACEW,
    EmfRecordTypeMax                     = 122,
    EmfRecordTypeMin                     = 1,

    // That is the END of the GDI EMF records.

    // Now we start the list of EMF+ records.  We leave quite
    // a bit of room here for the addition of any new GDI
    // records that may be added later.

    EmfPlusRecordTypeInvalid = GDIP_EMFPLUS_RECORD_BASE,
    EmfPlusRecordTypeHeader,
    EmfPlusRecordTypeEndOfFile,

    EmfPlusRecordTypeComment,

    EmfPlusRecordTypeGetDC,    // the application grabbed the metafile dc

    EmfPlusRecordTypeMultiFormatStart,
    EmfPlusRecordTypeMultiFormatSection,
    EmfPlusRecordTypeMultiFormatEnd,

    // For all persistent objects
    EmfPlusRecordTypeObject, // brush,pen,path,region,image,font,string-format

    // Drawing Records
    EmfPlusRecordTypeClear,
    EmfPlusRecordTypeFillRects,
    EmfPlusRecordTypeDrawRects,
    EmfPlusRecordTypeFillPolygon,
    EmfPlusRecordTypeDrawLines,
    EmfPlusRecordTypeFillEllipse,
    EmfPlusRecordTypeDrawEllipse,
    EmfPlusRecordTypeFillPie,
    EmfPlusRecordTypeDrawPie,
    EmfPlusRecordTypeDrawArc,
    EmfPlusRecordTypeFillRegion,
    EmfPlusRecordTypeFillPath,
    EmfPlusRecordTypeDrawPath,
    EmfPlusRecordTypeFillClosedCurve,
    EmfPlusRecordTypeDrawClosedCurve,
    EmfPlusRecordTypeDrawCurve,
    EmfPlusRecordTypeDrawBeziers,
    EmfPlusRecordTypeDrawImage,
    EmfPlusRecordTypeDrawImagePoints,
    EmfPlusRecordTypeDrawString,

    // Graphics State Records
    EmfPlusRecordTypeSetRenderingOrigin,
    EmfPlusRecordTypeSetAntiAliasMode,
    EmfPlusRecordTypeSetTextRenderingHint,
#ifdef DCR_USE_NEW_188922
   EmfPlusRecordTypeSetTextContrast,
#else
   EmfPlusRecordTypeSetGammaValue,
#endif // DCR_USE_NEW_188922
    EmfPlusRecordTypeSetInterpolationMode,
    EmfPlusRecordTypeSetPixelOffsetMode,
    EmfPlusRecordTypeSetCompositingMode,
    EmfPlusRecordTypeSetCompositingQuality,
    EmfPlusRecordTypeSave,
    EmfPlusRecordTypeRestore,
    EmfPlusRecordTypeBeginContainer,
    EmfPlusRecordTypeBeginContainerNoParams,
    EmfPlusRecordTypeEndContainer,
    EmfPlusRecordTypeSetWorldTransform,
    EmfPlusRecordTypeResetWorldTransform,
    EmfPlusRecordTypeMultiplyWorldTransform,
    EmfPlusRecordTypeTranslateWorldTransform,
    EmfPlusRecordTypeScaleWorldTransform,
    EmfPlusRecordTypeRotateWorldTransform,
    EmfPlusRecordTypeSetPageTransform,
    EmfPlusRecordTypeResetClip,
    EmfPlusRecordTypeSetClipRect,
    EmfPlusRecordTypeSetClipPath,
    EmfPlusRecordTypeSetClipRegion,
    EmfPlusRecordTypeOffsetClip,

    // New record types must be added here (at the end) -- do not add above,
    // since that will invalidate previous metafiles!
    EmfPlusRecordTypeDrawDriverString,

    // Have this here so you don't need to keep changing the value of
    // EmfPlusRecordTypeMax every time you add a new record.

    EmfPlusRecordTotal,

    EmfPlusRecordTypeMax = EmfPlusRecordTotal-1,
    EmfPlusRecordTypeMin = EmfPlusRecordTypeHeader,
};

//---------------------------------------------------------------------------
// StringFormatFlags
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
// String format flags
//
//  DirectionRightToLeft          - For horizontal text, the reading order is
//                                  right to left. This value is called
//                                  the base embedding level by the Unicode
//                                  bidirectional engine.
//                                  For vertical text, columns are read from
//                                  right to left.
//                                  By default, horizontal or vertical text is
//                                  read from left to right.
//
//  DirectionVertical             - Individual lines of text are vertical. In
//                                  each line, characters progress from top to
//                                  bottom.
//                                  By default, lines of text are horizontal,
//                                  each new line below the previous line.
//
//  NoFitBlackBox                 - Allows parts of glyphs to overhang the
//                                  bounding rectangle.
//                                  By default glyphs are first aligned
//                                  inside the margines, then any glyphs which
//                                  still overhang the bounding box are
//                                  repositioned to avoid any overhang.
//                                  For example when an italic
//                                  lower case letter f in a font such as
//                                  Garamond is aligned at the far left of a
//                                  rectangle, the lower part of the f will
//                                  reach slightly further left than the left
//                                  edge of the rectangle. Setting this flag
//                                  will ensure the character aligns visually
//                                  with the lines above and below, but may
//                                  cause some pixels outside the formatting
//                                  rectangle to be clipped or painted.
//
#ifndef DCR_USE_NEW_137252
//  NumberContextArabic           - Causes any initial numeric in the string to
//                                  be analysed for bidirection layout as if
//                                  it was preceeded by Arabic text.
//
//  DisableKashidaJustification   - Arabic text will not be justified by the
//                                  insertion of kashidas (i.e. extending the
//                                  joining line between characters). Instead
//                                  Arabic script will be justified by the
//                                  widening of the whitespace between words.
//
#endif
//  DisplayFormatControl          - Causes control characters such as the
//                                  left-to-right mark to be shown in the
//                                  output with a representative glyph.
//
#ifndef DCR_USE_NEW_137252
//  DisableKerning                - Disables Truetype and OpenType kerning.
//
//  DisableLigatures              - Disables Truetype and OpenType ligatures.
//
//  LayoutLegacyBidi              - Causes the bidirection algorithm to use
//                                  slightly different classifications for
//                                  '+', '-' and '/' that make their layout
//                                  much closer to that expected by files
//                                  generated in Windows or by Windows
//                                  applications.
//
//  NoChanges                     - A text imager created with this flag set
//                                  does not support those APIs that change
//                                  it's contents or formatting, but for most
//                                  simple text will be significantly faster in
//                                  performing measurement and drawing
//                                  functions.
//
#endif
//  NoFontFallback                - Disables fallback to alternate fonts for
//                                  characters not supported in the requested
//                                  font. Any missing characters will be
//                                  be displayed with the fonts missing glyph,
//                                  usually an open square.
//
//  NoWrap                        - Disables wrapping of text between lines
//                                  when formatting within a rectangle.
//                                  NoWrap is implied when a point is passed
//                                  instead of a rectangle, or when the
//                                  specified rectangle has a zero line length.
//
//  NoClip                        - By default text is clipped to the
//                                  formatting rectangle. Setting NoClip
//                                  allows overhanging pixels to affect the
//                                  device outside the formatting rectangle.
//                                  Pixels at the end of the line may be
//                                  affected if the glyphs overhang their
//                                  cells, and either the NoFitBlackBox flag
//                                  has been set, or the glyph extends to far
//                                  to be fitted.
//                                  Pixels above/before the first line or
//                                  below/after the last line may be affected
//                                  if the glyphs extend beyond their cell
//                                  ascent / descent. This can occur rarely
//                                  with unusual diacritic mark combinations.

//---------------------------------------------------------------------------

enum StringFormatFlags
{
    StringFormatFlagsDirectionRightToLeft        = 0x00000001,
    StringFormatFlagsDirectionVertical           = 0x00000002,
    StringFormatFlagsNoFitBlackBox               = 0x00000004,
#ifndef DCR_USE_NEW_137252
    StringFormatFlagsNumberContextArabic         = 0x00000008,
    StringFormatFlagsDisableKashidaJustification = 0x00000010,
#endif
    StringFormatFlagsDisplayFormatControl        = 0x00000020,
#ifndef DCR_USE_NEW_137252
    StringFormatFlagsDisableKerning              = 0x00000040,
    StringFormatFlagsDisableLigatures            = 0x00000080,
    StringFormatFlagsLayoutLegacyBidi            = 0x00000100,
    StringFormatFlagsNoChanges                   = 0x00000200,
#endif
    StringFormatFlagsNoFontFallback              = 0x00000400,
    StringFormatFlagsMeasureTrailingSpaces       = 0x00000800,
    StringFormatFlagsNoWrap                      = 0x00001000,
    StringFormatFlagsLineLimit                   = 0x00002000,

    StringFormatFlagsNoClip                      = 0x00004000
};

//---------------------------------------------------------------------------
// StringTrimming
//---------------------------------------------------------------------------

enum StringTrimming {
    StringTrimmingNone              = 0,
    StringTrimmingCharacter         = 1,
    StringTrimmingWord              = 2,
    StringTrimmingEllipsisCharacter = 3,
    StringTrimmingEllipsisWord      = 4,
    StringTrimmingEllipsisPath      = 5
};

#ifndef DCR_USE_NEW_137252
//---------------------------------------------------------------------------
// String units
//
// String units are like length units in CSS, they may be absolute, or
// they may be relative to a font size.
//
//---------------------------------------------------------------------------

enum StringUnit {
    StringUnitWorld        = UnitWorld,
    StringUnitDisplay      = UnitDisplay,
    StringUnitPixel        = UnitPixel,
    StringUnitPoint        = UnitPoint,
    StringUnitInch         = UnitInch,
    StringUnitDocument     = UnitDocument,
    StringUnitMillimeter   = UnitMillimeter,
    StringUnitEm           = 32
};
#endif

#ifndef DCR_USE_NEW_152154
//---------------------------------------------------------------------------
// Line spacing flags
//---------------------------------------------------------------------------

enum LineSpacing {
    LineSpacingWorld                = UnitWorld,
    LineSpacingDisplay              = UnitDisplay,
    LineSpacingPixel                = UnitPixel,
    LineSpacingPoint                = UnitPoint,
    LineSpacingInch                 = UnitInch,
    LineSpacingDocument             = UnitDocument,
    LineSpacingMillimeter           = UnitMillimeter,

    LineSpacingRecommended          = 32,
    LineSpacingAtLeast              = 33,
    LineSpacingAtLeastMultiple      = 34,
    LineSpacingCell                 = 35,
    LineSpacingCellAtLeast          = 36,
    LineSpacingCellAtLeastMultiple  = 37
};

/// The following methods of linespacing are relative to the font size
//
// =========== Method ===========  =============== Relative to ===============
//
// LineSpacingRecommended          recommended line spacing specified by font
// LineSpacingAtLeast              max(recommended, tallest glyph cell)
// LineSpacingAtLeastMultiple      smallest multiple of recommended big enough
//                                 for all glyph cells on the line
// LineSpacingCell                 cell height
// LineSpacingCellAtLeast          max(font cell height, tallest glyph cell)
// LineSpacingCellAtLeastMultiple  smallest multiple of cell height big enough
//                                 for all glyph cells on the line
#endif


//---------------------------------------------------------------------------
// National language digit substitution
//---------------------------------------------------------------------------

enum StringDigitSubstitute
{
    StringDigitSubstituteUser        = 0,  // As NLS setting
    StringDigitSubstituteNone        = 1,
    StringDigitSubstituteNational    = 2,
    StringDigitSubstituteTraditional = 3
};

//---------------------------------------------------------------------------
// Hotkey prefix interpretation
//---------------------------------------------------------------------------

enum HotkeyPrefix
{
    HotkeyPrefixNone        = 0,
    HotkeyPrefixShow        = 1,
    HotkeyPrefixHide        = 2
};

//---------------------------------------------------------------------------
// Text alignment flags
//---------------------------------------------------------------------------

enum StringAlignment
{
    // Left edge for left-to-right text,
    // right for right-to-left text,
    // and top for vertical
    StringAlignmentNear   = 0,
    StringAlignmentCenter = 1,
    StringAlignmentFar    = 2
};

//---------------------------------------------------------------------------
// DriverStringOptions
//---------------------------------------------------------------------------

enum DriverStringOptions
{
    DriverStringOptionsCmapLookup             = 1,
    DriverStringOptionsVertical               = 2,
    DriverStringOptionsRealizedAdvance        = 4,
#ifndef DCR_USE_NEW_137252
    DriverStringOptionsCompensateResolution   = 8
#endif
};

//---------------------------------------------------------------------------
// Flush Intention flags
//---------------------------------------------------------------------------

enum FlushIntention
{
    FlushIntentionFlush = 0,        // Flush all batched rendering operations
    FlushIntentionSync = 1          // Flush all batched rendering operations
                                    // and wait for them to complete
};

#ifndef DCR_USE_NEW_175866

//---------------------------------------------------------------------------
// Window Change Notification types
//---------------------------------------------------------------------------

enum WindowNotifyEnum
{
   WindowNotifyEnumEnable = 0,
   WindowNotifyEnumDisable,
   WindowNotifyEnumPalette,
   WindowNotifyEnumDisplay,
   WindowNotifyEnumSysColor
};

#endif

//---------------------------------------------------------------------------
// Image encoder parameter related types
//---------------------------------------------------------------------------

#ifdef DCR_USE_NEW_145804
enum EncoderParameterValueType
{
    EncoderParameterValueTypeByte           = 1,    // 8-bit unsigned int
    EncoderParameterValueTypeASCII          = 2,    // 8-bit byte containing one 7-bit ASCII
                                                    // code. NULL terminated.
    EncoderParameterValueTypeShort          = 3,    // 16-bit unsigned int
    EncoderParameterValueTypeLong           = 4,    // 32-bit unsigned int
    EncoderParameterValueTypeRational       = 5,    // Two Longs. The first Long is the
                                                    // numerator, the second Long expresses the
                                                    // denomintor.
    EncoderParameterValueTypeLongRange      = 6,    // Two longs which specify a range of
                                                    // integer values. The first Long specifies
                                                    // the lower end and the second one
                                                    // specifies the higher end. All values
                                                    // are inclusive at both ends
    EncoderParameterValueTypeUndefined      = 7,    // 8-bit byte that can take any value
                                                    // depending on field definition
    EncoderParameterValueTypeRationalRange  = 8     // Two Rationals. The first Rational
                                                    // specifies the lower end and the second
                                                    // specifies the higher end. All values
                                                    // are inclusive at both ends
};
#else
enum ValueType
{
    ValueTypeByte           = 1,    // 8-bit unsigned int
    ValueTypeASCII          = 2,    // 8-bit byte containing one 7-bit ASCII
                                    // code. NULL terminated.
    ValueTypeShort          = 3,    // 16-bit unsigned int
    ValueTypeLong           = 4,    // 32-bit unsigned int
    ValueTypeRational       = 5,    // Two Longs. The first Long is the
                                    // numerator, the second Long expresses the
                                    // denomintor.
    ValueTypeLongRange      = 6,    // Two longs which specify a range of
                                    // integer values. The first Long specifies
                                    // the lower end and the second one
                                    // specifies the higher end. All values
                                    // are inclusive at both ends
    ValueTypeUndefined      = 7,    // 8-bit byte that can take any value
                                    // depending on field definition
    ValueTypeRationalRange  = 8     // Two Rationals. The first Rational
                                    // specifies the lower end and the second
                                    // specifies the higher end. All values
                                    // are inclusive at both ends
};
#endif

//---------------------------------------------------------------------------
// Image encoder value types
//---------------------------------------------------------------------------

enum EncoderValue
{
    EncoderValueColorTypeCMYK,
    EncoderValueColorTypeYCCK,
    EncoderValueCompressionLZW,
    EncoderValueCompressionCCITT3,
    EncoderValueCompressionCCITT4,
    EncoderValueCompressionRle,
    EncoderValueCompressionNone,
    EncoderValueScanMethodInterlaced,
    EncoderValueScanMethodNonInterlaced,
    EncoderValueVersionGif87,
    EncoderValueVersionGif89,
    EncoderValueRenderProgressive,
    EncoderValueRenderNonProgressive,
    EncoderValueTransformRotate90,
    EncoderValueTransformRotate180,
    EncoderValueTransformRotate270,
    EncoderValueTransformFlipHorizontal,
    EncoderValueTransformFlipVertical,
    #ifdef DCR_USE_NEW_140861
    EncoderValueMultiFrame,
    #else
    EncodeValueMultiFrame,
    #endif
    EncoderValueLastFrame,
    EncoderValueFlush,
    #ifdef DCR_USE_NEW_140861
    EncoderValueFrameDimensionTime,
    EncoderValueFrameDimensionResolution,
    EncoderValueFrameDimensionPage
    #else
    EncodeValueFrameDimensionTime,
    EncodeValueFrameDimensionResolution,
    EncodeValueFrameDimensionPage
    #endif
};

//---------------------------------------------------------------------------
// Graphics layout values (support for Middle East localization)
//---------------------------------------------------------------------------

enum GraphicsLayout
{
    GraphicsLayoutNormal,
    GraphicsLayoutMirrored,
    GraphicsLayoutMirroredIgnoreImages,
    GraphicsLayoutMirroredForceImages
};

//---------------------------------------------------------------------------
// Image layout values (support for Middle East localization)
//---------------------------------------------------------------------------

enum ImageLayout
{
    ImageLayoutNormal,
    ImageLayoutIgnoreMirrored
};

enum EmfToWmfBitsFlags
{
    EmfToWmfBitsFlagsDefault    = 0x00000000,
    EmfToWmfBitsFlagsEmbedEmf   = 0x00000001,
    EmfToWmfBitsFlagsIncludeAPM = 0x00000002,
    EmfToWmfBitsFlagsNoXORClip  = 0x00000004
};

#endif // !_GDIPLUSENUMS_H
