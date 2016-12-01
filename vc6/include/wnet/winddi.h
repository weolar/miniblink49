/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    winddi.h

Abstract:

    Private entry points, defines and types for Windows NT GDI device
    driver interface.

--*/

#ifndef _WINDDI_
#define _WINDDI_

#if defined(_ENGINE_EXPORT_)

#define ENGAPI

#else

#define ENGAPI DECLSPEC_IMPORT

#endif

//
// GUID definitions for use with kernel-mode GDI drivers
//

#ifndef _NO_DDRAWINT_NO_COM

#ifndef EXTERN_C
    #ifdef __cplusplus
        #define EXTERN_C    extern "C"
    #else // !defined(__cplusplus)
        #define EXTERN_C    extern
    #endif // !defined(__cplusplus)
#endif // !defined(EXTERN_C)

#ifndef DEFINE_GUID
#ifndef INITGUID
#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
    EXTERN_C const GUID FAR name
#else // !defined(INITGUID)

#define DEFINE_GUID(name, l, w1, w2, b1, b2, b3, b4, b5, b6, b7, b8) \
        EXTERN_C const GUID name \
                = { l, w1, w2, { b1, b2,  b3,  b4,  b5,  b6,  b7,  b8 } }
#endif // !defined(INITGUID)
#endif // !defined(DEFINE_GUID)

#ifndef DEFINE_GUIDEX
    #define DEFINE_GUIDEX(name) EXTERN_C const CDECL GUID name
#endif // !defined(DEFINE_GUIDEX)

#ifndef STATICGUIDOF
    #define STATICGUIDOF(guid) STATIC_##guid
#endif // !defined(STATICGUIDOF)

#ifndef GUID_DEFINED
    #define GUID_DEFINED
    typedef struct _GUID {
        ULONG   Data1;
        USHORT  Data2;
        USHORT  Data3;
        UCHAR   Data4[8];
    } GUID;
#endif // !defined(GUID_DEFINED)

#ifndef IsEqualGUID
    #define IsEqualGUID(guid1, guid2) \
        (!memcmp((guid1), (guid2), sizeof(GUID)))
#endif // !defined(IsEqualGUID)

#ifndef IsEqualIID
    #define IsEqualIID IsEqualGUID
#endif // !defined(IsEqualIID)

#endif // !defined(_NO_DDRAWINT_NO_COM)

//
// Standard DirectX HAL includes
//

#include <ddrawint.h>
#include <d3dnthal.h>

#ifdef __cplusplus
extern "C" {
#endif


//
// drivers and other components that include this should NOT include
// windows.h  They should be system conponents that only use GDI internals
// and therefore only include wingdi.h
//

#if defined(_X86_) && !defined(USERMODE_DRIVER) && !defined(BUILD_WOW6432)
//
// x86 does not support floating-point instruction in the kernel mode,
// the floating-point data would like be handled 32bits value as double words.
//
typedef DWORD FLOATL;
#else
//
// Any platform that has support for floats in the kernel
//
typedef FLOAT FLOATL;
#endif // _X86_

typedef LONG        PTRDIFF;
typedef PTRDIFF    *PPTRDIFF;
typedef LONG        FIX;
typedef FIX        *PFIX;
typedef ULONG       ROP4;
typedef ULONG       MIX;
typedef ULONG       HGLYPH;
typedef HGLYPH     *PHGLYPH;
#define HGLYPH_INVALID ((HGLYPH)-1)

typedef ULONG           IDENT;

//
// handles for font file and font context objects
//

typedef ULONG_PTR HFF;
typedef ULONG_PTR HFC;
#define HFF_INVALID ((HFF) 0)
#define HFC_INVALID ((HFC) 0)

#define FD_ERROR  0xFFFFFFFF
#define DDI_ERROR 0xFFFFFFFF

typedef struct _POINTE      /* pte  */
{
    FLOATL x;
    FLOATL y;
} POINTE, *PPOINTE;

typedef union _FLOAT_LONG
{
    FLOATL   e;
    LONG     l;
} FLOAT_LONG, *PFLOAT_LONG;

typedef struct  _POINTFIX
{
    FIX   x;
    FIX   y;
} POINTFIX, *PPOINTFIX;

typedef struct _RECTFX
{
    FIX   xLeft;
    FIX   yTop;
    FIX   xRight;
    FIX   yBottom;
} RECTFX, *PRECTFX;


DECLARE_HANDLE(HBM);
DECLARE_HANDLE(HDEV);
DECLARE_HANDLE(HSURF);
DECLARE_HANDLE(DHSURF);
DECLARE_HANDLE(DHPDEV);
DECLARE_HANDLE(HDRVOBJ);

#define LTOFX(x)            ((x)<<4)

#define FXTOL(x)            ((x)>>4)
#define FXTOLFLOOR(x)       ((x)>>4)
#define FXTOLCEILING(x)     ((x + 0x0F)>>4)
#define FXTOLROUND(x)       ((((x) >> 3) + 1) >> 1)

// context information

typedef struct _FD_XFORM {
    FLOATL eXX;
    FLOATL eXY;
    FLOATL eYX;
    FLOATL eYY;
} FD_XFORM, *PFD_XFORM;


typedef struct _FD_DEVICEMETRICS {       // devm
    FLONG  flRealizedType;
    POINTE pteBase;
    POINTE pteSide;
    LONG   lD;
    FIX    fxMaxAscender;
    FIX    fxMaxDescender;
    POINTL ptlUnderline1;
    POINTL ptlStrikeOut;
    POINTL ptlULThickness;
    POINTL ptlSOThickness;
    ULONG  cxMax;                      // max pel width of bitmaps

// the fields formerly in REALIZE_EXTRA as well as some new fields:

    ULONG cyMax;      // did not use to be here
    ULONG cjGlyphMax; // (cxMax + 7)/8 * cyMax, or at least it should be

    FD_XFORM  fdxQuantized;
    LONG      lNonLinearExtLeading;
    LONG      lNonLinearIntLeading;
    LONG      lNonLinearMaxCharWidth;
    LONG      lNonLinearAvgCharWidth;

// some new fields

    LONG      lMinA;
    LONG      lMinC;
    LONG      lMinD;

    LONG      alReserved[1]; // just in case we need it.

} FD_DEVICEMETRICS, *PFD_DEVICEMETRICS;

typedef struct _LIGATURE { /* lig */
        ULONG culSize;
        LPWSTR pwsz;
        ULONG chglyph;
        HGLYPH ahglyph[1];
} LIGATURE, *PLIGATURE;

typedef struct _FD_LIGATURE {
        ULONG culThis;
        ULONG ulType;
        ULONG cLigatures;
        LIGATURE alig[1];
} FD_LIGATURE;


// glyph handle must be 32 bit


// signed 16 bit integer type denoting number of FUnit's

typedef SHORT FWORD;

// point in the 32.32 bit precission

typedef struct _POINTQF    // ptq
{
    LARGE_INTEGER x;
    LARGE_INTEGER y;
} POINTQF, *PPOINTQF;

//. Structures


//     devm.flRealizedType flags

// FDM_TYPE_ZERO_BEARINGS           // all glyphs have zero a and c spaces

// the following two features refer to all glyphs in this font realization

// FDM_TYPE_CHAR_INC_EQUAL_BM_BASE  // base width == cx for horiz, == cy for vert.
// FDM_TYPE_MAXEXT_EQUAL_BM_SIDE    // side width == cy for horiz, == cx for vert.

#define FDM_TYPE_BM_SIDE_CONST          0x00000001
#define FDM_TYPE_MAXEXT_EQUAL_BM_SIDE   0x00000002
#define FDM_TYPE_CHAR_INC_EQUAL_BM_BASE 0x00000004
#define FDM_TYPE_ZERO_BEARINGS          0x00000008
#define FDM_TYPE_CONST_BEARINGS         0x00000010


// structures for describing a supported set of glyphs in a font

typedef struct _WCRUN {
    WCHAR   wcLow;        // lowest character in run  inclusive
    USHORT  cGlyphs;      // wcHighInclusive = wcLow + cGlyphs - 1;
    HGLYPH *phg;          // pointer to an array of cGlyphs HGLYPH's
} WCRUN, *PWCRUN;

// If phg is set to (HGLYPH *)NULL, for all wc's in this particular run
// the handle can be computed as simple zero extension:
//        HGLYPH hg = (HGLYPH) wc;
//
// If phg is not NULL, memory pointed to by phg, allocated by the driver,
// WILL NOT MOVE.


typedef struct _FD_GLYPHSET {
    ULONG    cjThis;           // size of this structure in butes
    FLONG    flAccel;          // accel flags, bits to be explained below
    ULONG    cGlyphsSupported; // sum over all wcrun's of wcrun.cGlyphs
    ULONG    cRuns;
    WCRUN    awcrun[1];        // an array of cRun WCRUN structures
} FD_GLYPHSET, *PFD_GLYPHSET;

typedef struct _FD_GLYPHATTR {
    ULONG    cjThis;           // size of this structure in butes
    ULONG    cGlyphs;
    ULONG    iMode;
    BYTE     aGlyphAttr[1];    // an array of byte
} FD_GLYPHATTR, *PFD_GLYPHATTR;

// If GS_UNICODE_HANDLES  bit is set,
// for ALL WCRUNS in this FD_GLYPHSET the handles are
// obtained by zero extending unicode code points of
// the corresponding supported glyphs, i.e. all gs.phg's are NULL

#define GS_UNICODE_HANDLES      0x00000001

// If GS_8BIT_HANDLES bit is set, all handles are in 0-255 range.
// This is just an ansi font then and we are really making up all
// the unicode stuff about this font.

#define GS_8BIT_HANDLES         0x00000002

// all handles fit in 16 bits.
// to 8 bit handles as it should.

#define GS_16BIT_HANDLES        0x00000004


// ligatures


typedef struct _FD_KERNINGPAIR {
    WCHAR  wcFirst;
    WCHAR  wcSecond;
    FWORD  fwdKern;
} FD_KERNINGPAIR;

// IFIMETRICS constants

#define FM_VERSION_NUMBER                   0x0

//
// IFIMETRICS::fsType flags
//
#define FM_TYPE_LICENSED                    0x2
#define FM_READONLY_EMBED                   0x4
#define FM_EDITABLE_EMBED                   0x8
#define FM_NO_EMBEDDING                     FM_TYPE_LICENSED

//
// IFIMETRICS::flInfo flags
//
#define FM_INFO_TECH_TRUETYPE               0x00000001
#define FM_INFO_TECH_BITMAP                 0x00000002
#define FM_INFO_TECH_STROKE                 0x00000004
#define FM_INFO_TECH_OUTLINE_NOT_TRUETYPE   0x00000008
#define FM_INFO_ARB_XFORMS                  0x00000010
#define FM_INFO_1BPP                        0x00000020
#define FM_INFO_4BPP                        0x00000040
#define FM_INFO_8BPP                        0x00000080
#define FM_INFO_16BPP                       0x00000100
#define FM_INFO_24BPP                       0x00000200
#define FM_INFO_32BPP                       0x00000400
#define FM_INFO_INTEGER_WIDTH               0x00000800
#define FM_INFO_CONSTANT_WIDTH              0x00001000
#define FM_INFO_NOT_CONTIGUOUS              0x00002000
#define FM_INFO_TECH_MM                     0x00004000
#define FM_INFO_RETURNS_OUTLINES            0x00008000
#define FM_INFO_RETURNS_STROKES             0x00010000
#define FM_INFO_RETURNS_BITMAPS             0x00020000
#define FM_INFO_DSIG                        0x00040000 // FM_INFO_UNICODE_COMPLIANT
#define FM_INFO_RIGHT_HANDED                0x00080000
#define FM_INFO_INTEGRAL_SCALING            0x00100000
#define FM_INFO_90DEGREE_ROTATIONS          0x00200000
#define FM_INFO_OPTICALLY_FIXED_PITCH       0x00400000
#define FM_INFO_DO_NOT_ENUMERATE            0x00800000
#define FM_INFO_ISOTROPIC_SCALING_ONLY      0x01000000
#define FM_INFO_ANISOTROPIC_SCALING_ONLY    0x02000000
#define FM_INFO_TECH_CFF                    0x04000000
#define FM_INFO_FAMILY_EQUIV                0x08000000
#define FM_INFO_DBCS_FIXED_PITCH            0x10000000
#define FM_INFO_NONNEGATIVE_AC              0x20000000
#define FM_INFO_IGNORE_TC_RA_ABLE           0x40000000
#define FM_INFO_TECH_TYPE1                  0x80000000

// max number of charsets supported in a tt font, 16 according to win95 guys

#define MAXCHARSETS 16

//
// IFIMETRICS::ulPanoseCulture
//
#define  FM_PANOSE_CULTURE_LATIN     0x0


//
// IFMETRICS::fsSelection flags
//
#define  FM_SEL_ITALIC          0x0001
#define  FM_SEL_UNDERSCORE      0x0002
#define  FM_SEL_NEGATIVE        0x0004
#define  FM_SEL_OUTLINED        0x0008
#define  FM_SEL_STRIKEOUT       0x0010
#define  FM_SEL_BOLD            0x0020
#define  FM_SEL_REGULAR         0x0040

//
// The FONTDIFF structure contains all of the fields that could
// possibly change under simulation
//
typedef struct _FONTDIFF {
    BYTE   jReserved1;      // 0x0
    BYTE   jReserved2;      // 0x1
    BYTE   jReserved3;      // 0x2
    BYTE   bWeight;         // 0x3  Panose Weight
    USHORT usWinWeight;     // 0x4
    FSHORT fsSelection;     // 0x6
    FWORD  fwdAveCharWidth; // 0x8
    FWORD  fwdMaxCharInc;   // 0xA
    POINTL ptlCaret;        // 0xC
} FONTDIFF;

typedef struct _FONTSIM {
    PTRDIFF  dpBold;       // offset from beginning of FONTSIM to FONTDIFF
    PTRDIFF  dpItalic;     // offset from beginning of FONTSIM to FONTDIFF
    PTRDIFF  dpBoldItalic; // offset from beginning of FONTSIM to FONTDIFF
} FONTSIM;


typedef struct _IFIMETRICS {
    ULONG    cjThis;           // includes attached information
    ULONG    cjIfiExtra;       // sizeof IFIEXTRA if any, formerly ulVersion
    PTRDIFF  dpwszFamilyName;
    PTRDIFF  dpwszStyleName;
    PTRDIFF  dpwszFaceName;
    PTRDIFF  dpwszUniqueName;
    PTRDIFF  dpFontSim;
    LONG     lEmbedId;
    LONG     lItalicAngle;
    LONG     lCharBias;

// dpCharSet field replaced alReserved[0].
// If the 3.51 pcl minidrivers are still to work on NT 4.0 this field must not
// move because they will have 0 at this position.

    PTRDIFF  dpCharSets;            // only used if > 1 charset supported
    BYTE     jWinCharSet;           // as in LOGFONT::lfCharSet
    BYTE     jWinPitchAndFamily;    // as in LOGFONT::lfPitchAndFamily
    USHORT   usWinWeight;           // as in LOGFONT::lfWeight
    ULONG    flInfo;                // see above
    USHORT   fsSelection;           // see above
    USHORT   fsType;                // see above
    FWORD    fwdUnitsPerEm;         // em height
    FWORD    fwdLowestPPEm;         // readable limit
    FWORD    fwdWinAscender;
    FWORD    fwdWinDescender;
    FWORD    fwdMacAscender;
    FWORD    fwdMacDescender;
    FWORD    fwdMacLineGap;
    FWORD    fwdTypoAscender;
    FWORD    fwdTypoDescender;
    FWORD    fwdTypoLineGap;
    FWORD    fwdAveCharWidth;
    FWORD    fwdMaxCharInc;
    FWORD    fwdCapHeight;
    FWORD    fwdXHeight;
    FWORD    fwdSubscriptXSize;
    FWORD    fwdSubscriptYSize;
    FWORD    fwdSubscriptXOffset;
    FWORD    fwdSubscriptYOffset;
    FWORD    fwdSuperscriptXSize;
    FWORD    fwdSuperscriptYSize;
    FWORD    fwdSuperscriptXOffset;
    FWORD    fwdSuperscriptYOffset;
    FWORD    fwdUnderscoreSize;
    FWORD    fwdUnderscorePosition;
    FWORD    fwdStrikeoutSize;
    FWORD    fwdStrikeoutPosition;
    BYTE     chFirstChar;           // for win 3.1 compatibility
    BYTE     chLastChar;            // for win 3.1 compatibility
    BYTE     chDefaultChar;         // for win 3.1 compatibility
    BYTE     chBreakChar;           // for win 3.1 compatibility
    WCHAR    wcFirstChar;           // lowest supported code in Unicode set
    WCHAR    wcLastChar;            // highest supported code in Unicode set
    WCHAR    wcDefaultChar;
    WCHAR    wcBreakChar;
    POINTL   ptlBaseline;           //
    POINTL   ptlAspect;             // designed aspect ratio (bitmaps)
    POINTL   ptlCaret;              // points along caret
    RECTL    rclFontBox;            // bounding box for all glyphs (font space)
    BYTE     achVendId[4];          // as per TrueType
    ULONG    cKerningPairs;
    ULONG    ulPanoseCulture;
    PANOSE   panose;

#if defined(_WIN64)

    //
    // IFIMETRICS must begin on a 64-bit boundary
    //

    PVOID    Align;

#endif

} IFIMETRICS, *PIFIMETRICS;


// rather than adding the fields of IFIEXTRA  to IFIMETRICS itself
// we add them as a separate structure. This structure, if present at all,
// lies below IFIMETRICS in memory.
// If IFIEXTRA is present at all, ifi.cjIfiExtra (formerly ulVersion)
// will contain size of IFIEXTRA including any reserved fields.
// That way ulVersion = 0 (NT 3.51 or less) printer minidrivers
// will work with NT 4.0.

typedef struct _IFIEXTRA
{
    ULONG    ulIdentifier;   // used for Type 1 fonts only
    PTRDIFF  dpFontSig;      // nontrivial for tt only, at least for now.
    ULONG    cig;            // maxp->numGlyphs, # of distinct glyph indicies
    PTRDIFF  dpDesignVector; // offset to design vector for mm instances
    PTRDIFF  dpAxesInfoW;    // offset to full axes info for base mm font
    ULONG    aulReserved[1]; // in case we need even more stuff in the future
} IFIEXTRA, *PIFIEXTRA;

#define SIZEOFDV(cAxes) (offsetof(DESIGNVECTOR,dvValues) + (cAxes)*sizeof(LONG))
#define SIZEOFAXIW(cAxes) (offsetof(AXESLISTW,axlAxisInfo) + (cAxes)*sizeof(AXISINFOW))
#define SIZEOFAXIA(cAxes) (offsetof(AXESLISTA,axlAxisInfo) + (cAxes)*sizeof(AXISINFOA))

/**************************************************************************\
 *
\**************************************************************************/

/* OpenGL DDI ExtEscape escape numbers (4352 - 4607) */

#define OPENGL_CMD      4352        /* for OpenGL ExtEscape */
#define OPENGL_GETINFO  4353        /* for OpenGL ExtEscape */
#define WNDOBJ_SETUP    4354        /* for live video ExtEscape */

/**************************************************************************\
* Display Driver version numbers
*
*   Note: DDI_DRIVER_VERSION has been removed.  Drivers must specify
*         what version of the DDI they support using one of the below
*         DDI_DRIVER_VERSION_Xxx macros.
*         DDI_DRIVER_VERSION_NT4 is equivalent to old DDI_DRIVER_VERSION.
\**************************************************************************/

#define DDI_DRIVER_VERSION_NT4      0x00020000
#define DDI_DRIVER_VERSION_SP3      0x00020003
#define DDI_DRIVER_VERSION_NT5      0x00030000
#define DDI_DRIVER_VERSION_NT5_01   0x00030100
#define DDI_DRIVER_VERSION_NT5_01_SP1 0x00030101

#define GDI_DRIVER_VERSION 0x4000   /* for NT version 4.0.00 */

typedef LONG_PTR (APIENTRY *PFN)();

typedef struct  _DRVFN  /* drvfn */
{
    ULONG   iFunc;
    PFN     pfn;
} DRVFN, *PDRVFN;

/* Required functions           */

#define INDEX_DrvEnablePDEV                      0L
#define INDEX_DrvCompletePDEV                    1L
#define INDEX_DrvDisablePDEV                     2L
#define INDEX_DrvEnableSurface                   3L
#define INDEX_DrvDisableSurface                  4L

/* Other functions              */

#define INDEX_DrvAssertMode                      5L
#define INDEX_DrvOffset                          6L     // Obsolete
#define INDEX_DrvResetPDEV                       7L
#define INDEX_DrvDisableDriver                   8L
#define INDEX_DrvCreateDeviceBitmap             10L
#define INDEX_DrvDeleteDeviceBitmap             11L
#define INDEX_DrvRealizeBrush                   12L
#define INDEX_DrvDitherColor                    13L
#define INDEX_DrvStrokePath                     14L
#define INDEX_DrvFillPath                       15L
#define INDEX_DrvStrokeAndFillPath              16L
#define INDEX_DrvPaint                          17L
#define INDEX_DrvBitBlt                         18L
#define INDEX_DrvCopyBits                       19L
#define INDEX_DrvStretchBlt                     20L
#define INDEX_DrvSetPalette                     22L
#define INDEX_DrvTextOut                        23L
#define INDEX_DrvEscape                         24L
#define INDEX_DrvDrawEscape                     25L
#define INDEX_DrvQueryFont                      26L
#define INDEX_DrvQueryFontTree                  27L
#define INDEX_DrvQueryFontData                  28L
#define INDEX_DrvSetPointerShape                29L
#define INDEX_DrvMovePointer                    30L
#define INDEX_DrvLineTo                         31L
#define INDEX_DrvSendPage                       32L
#define INDEX_DrvStartPage                      33L
#define INDEX_DrvEndDoc                         34L
#define INDEX_DrvStartDoc                       35L
#define INDEX_DrvGetGlyphMode                   37L
#define INDEX_DrvSynchronize                    38L
#define INDEX_DrvSaveScreenBits                 40L
#define INDEX_DrvGetModes                       41L
#define INDEX_DrvFree                           42L
#define INDEX_DrvDestroyFont                    43L
#define INDEX_DrvQueryFontCaps                  44L
#define INDEX_DrvLoadFontFile                   45L
#define INDEX_DrvUnloadFontFile                 46L
#define INDEX_DrvFontManagement                 47L
#define INDEX_DrvQueryTrueTypeTable             48L
#define INDEX_DrvQueryTrueTypeOutline           49L
#define INDEX_DrvGetTrueTypeFile                50L
#define INDEX_DrvQueryFontFile                  51L
#define INDEX_DrvMovePanning                    52L
#define INDEX_DrvQueryAdvanceWidths             53L
#define INDEX_DrvSetPixelFormat                 54L
#define INDEX_DrvDescribePixelFormat            55L
#define INDEX_DrvSwapBuffers                    56L
#define INDEX_DrvStartBanding                   57L
#define INDEX_DrvNextBand                       58L
#define INDEX_DrvGetDirectDrawInfo              59L
#define INDEX_DrvEnableDirectDraw               60L
#define INDEX_DrvDisableDirectDraw              61L
#define INDEX_DrvQuerySpoolType                 62L
#define INDEX_DrvIcmCreateColorTransform        64L
#define INDEX_DrvIcmDeleteColorTransform        65L
#define INDEX_DrvIcmCheckBitmapBits             66L
#define INDEX_DrvIcmSetDeviceGammaRamp          67L
#define INDEX_DrvGradientFill                   68L
#define INDEX_DrvStretchBltROP                  69L
#define INDEX_DrvPlgBlt                         70L
#define INDEX_DrvAlphaBlend                     71L
#define INDEX_DrvSynthesizeFont                 72L
#define INDEX_DrvGetSynthesizedFontFiles        73L
#define INDEX_DrvTransparentBlt                 74L
#define INDEX_DrvQueryPerBandInfo               75L
#define INDEX_DrvQueryDeviceSupport             76L

#define INDEX_DrvReserved1                      77L
#define INDEX_DrvReserved2                      78L
#define INDEX_DrvReserved3                      79L
#define INDEX_DrvReserved4                      80L
#define INDEX_DrvReserved5                      81L
#define INDEX_DrvReserved6                      82L
#define INDEX_DrvReserved7                      83L
#define INDEX_DrvReserved8                      84L

#define INDEX_DrvDeriveSurface                  85L
#define INDEX_DrvQueryGlyphAttrs                86L
#define INDEX_DrvNotify                         87L
#define INDEX_DrvSynchronizeSurface             88L
#define INDEX_DrvResetDevice                    89L
#define INDEX_DrvReserved9                      90L
#define INDEX_DrvReserved10                     91L
#define INDEX_DrvReserved11                     92L

/* The total number of dispatched functions */

#define INDEX_LAST                              93L

typedef struct  tagDRVENABLEDATA
{
    ULONG   iDriverVersion;
    ULONG   c;
    DRVFN  *pdrvfn;
} DRVENABLEDATA, *PDRVENABLEDATA;

typedef struct  tagDEVINFO
{
    FLONG       flGraphicsCaps;
    LOGFONTW    lfDefaultFont;
    LOGFONTW    lfAnsiVarFont;
    LOGFONTW    lfAnsiFixFont;
    ULONG       cFonts;
    ULONG       iDitherFormat;
    USHORT      cxDither;
    USHORT      cyDither;
    HPALETTE    hpalDefault;
    FLONG       flGraphicsCaps2;
} DEVINFO, *PDEVINFO;

// flGraphicsCaps flags for DEVINFO:

#define GCAPS_BEZIERS           0x00000001
#define GCAPS_GEOMETRICWIDE     0x00000002
#define GCAPS_ALTERNATEFILL     0x00000004
#define GCAPS_WINDINGFILL       0x00000008
#define GCAPS_HALFTONE          0x00000010
#define GCAPS_COLOR_DITHER      0x00000020
#define GCAPS_HORIZSTRIKE       0x00000040      // Obsolete
#define GCAPS_VERTSTRIKE        0x00000080      // Obsolete
#define GCAPS_OPAQUERECT        0x00000100
#define GCAPS_VECTORFONT        0x00000200
#define GCAPS_MONO_DITHER       0x00000400
#define GCAPS_ASYNCCHANGE       0x00000800      // Obsolete
#define GCAPS_ASYNCMOVE         0x00001000
#define GCAPS_DONTJOURNAL       0x00002000
#define GCAPS_DIRECTDRAW        0x00004000      // Obsolete
#define GCAPS_ARBRUSHOPAQUE     0x00008000
#define GCAPS_PANNING           0x00010000
#define GCAPS_HIGHRESTEXT       0x00040000
#define GCAPS_PALMANAGED        0x00080000
#define GCAPS_DITHERONREALIZE   0x00200000
#define GCAPS_NO64BITMEMACCESS  0x00400000      // Obsolete
#define GCAPS_FORCEDITHER       0x00800000
#define GCAPS_GRAY16            0x01000000
#define GCAPS_ICM               0x02000000
#define GCAPS_CMYKCOLOR         0x04000000
#define GCAPS_LAYERED           0x08000000
#define GCAPS_ARBRUSHTEXT       0x10000000
#define GCAPS_SCREENPRECISION   0x20000000
#define GCAPS_FONT_RASTERIZER   0x40000000      // the device has built in tt rasterizer
#define GCAPS_NUP               0x80000000

// flGraphicsCaps2 flags for DEVINFO:

#define GCAPS2_JPEGSRC          0x00000001
#define GCAPS2_xxxx             0x00000002
#define GCAPS2_PNGSRC           0x00000008
#define GCAPS2_CHANGEGAMMARAMP  0x00000010
#define GCAPS2_ALPHACURSOR      0x00000020
#define GCAPS2_SYNCFLUSH        0x00000040
#define GCAPS2_SYNCTIMER        0x00000080
#define GCAPS2_ICD_MULTIMON     0x00000100
#define GCAPS2_MOUSETRAILS      0x00000200
#define GCAPS2_RESERVED1        0x00000400

typedef struct  _LINEATTRS
{
    FLONG       fl;
    ULONG       iJoin;
    ULONG       iEndCap;
    FLOAT_LONG  elWidth;
    FLOATL      eMiterLimit;
    ULONG       cstyle;
    PFLOAT_LONG pstyle;
    FLOAT_LONG  elStyleState;
} LINEATTRS, *PLINEATTRS;

#define LA_GEOMETRIC        0x00000001
#define LA_ALTERNATE        0x00000002
#define LA_STARTGAP         0x00000004
#define LA_STYLED           0x00000008

#define JOIN_ROUND          0L
#define JOIN_BEVEL          1L
#define JOIN_MITER          2L

#define ENDCAP_ROUND        0L
#define ENDCAP_SQUARE       1L
#define ENDCAP_BUTT         2L

typedef LONG  LDECI4;

typedef struct _XFORML {
    FLOATL  eM11;
    FLOATL  eM12;
    FLOATL  eM21;
    FLOATL  eM22;
    FLOATL  eDx;
    FLOATL  eDy;
} XFORML, *PXFORML;

typedef struct _CIECHROMA
{
    LDECI4   x;
    LDECI4   y;
    LDECI4   Y;
}CIECHROMA;

typedef struct _COLORINFO
{
    CIECHROMA  Red;
    CIECHROMA  Green;
    CIECHROMA  Blue;
    CIECHROMA  Cyan;
    CIECHROMA  Magenta;
    CIECHROMA  Yellow;
    CIECHROMA  AlignmentWhite;

    LDECI4  RedGamma;
    LDECI4  GreenGamma;
    LDECI4  BlueGamma;

    LDECI4  MagentaInCyanDye;
    LDECI4  YellowInCyanDye;
    LDECI4  CyanInMagentaDye;
    LDECI4  YellowInMagentaDye;
    LDECI4  CyanInYellowDye;
    LDECI4  MagentaInYellowDye;
}COLORINFO, *PCOLORINFO;

// Allowed values for GDIINFO.ulPrimaryOrder.

#define PRIMARY_ORDER_ABC       0
#define PRIMARY_ORDER_ACB       1
#define PRIMARY_ORDER_BAC       2
#define PRIMARY_ORDER_BCA       3
#define PRIMARY_ORDER_CBA       4
#define PRIMARY_ORDER_CAB       5

// Allowed values for GDIINFO.ulHTPatternSize.

#define HT_PATSIZE_2x2          0
#define HT_PATSIZE_2x2_M        1
#define HT_PATSIZE_4x4          2
#define HT_PATSIZE_4x4_M        3
#define HT_PATSIZE_6x6          4
#define HT_PATSIZE_6x6_M        5
#define HT_PATSIZE_8x8          6
#define HT_PATSIZE_8x8_M        7
#define HT_PATSIZE_10x10        8
#define HT_PATSIZE_10x10_M      9
#define HT_PATSIZE_12x12        10
#define HT_PATSIZE_12x12_M      11
#define HT_PATSIZE_14x14        12
#define HT_PATSIZE_14x14_M      13
#define HT_PATSIZE_16x16        14
#define HT_PATSIZE_16x16_M      15
#define HT_PATSIZE_SUPERCELL    16
#define HT_PATSIZE_SUPERCELL_M  17
#define HT_PATSIZE_USER         18
#define HT_PATSIZE_MAX_INDEX    HT_PATSIZE_USER
#define HT_PATSIZE_DEFAULT      HT_PATSIZE_SUPERCELL_M

#define HT_USERPAT_CX_MIN       4
#define HT_USERPAT_CX_MAX       256
#define HT_USERPAT_CY_MIN       4
#define HT_USERPAT_CY_MAX       256


// Allowed values for GDIINFO.ulHTOutputFormat.

#define HT_FORMAT_1BPP          0
#define HT_FORMAT_4BPP          2
#define HT_FORMAT_4BPP_IRGB     3
#define HT_FORMAT_8BPP          4
#define HT_FORMAT_16BPP         5
#define HT_FORMAT_24BPP         6
#define HT_FORMAT_32BPP         7


// Allowed values for GDIINFO.flHTFlags.

#define HT_FLAG_SQUARE_DEVICE_PEL       0x00000001
#define HT_FLAG_HAS_BLACK_DYE           0x00000002
#define HT_FLAG_ADDITIVE_PRIMS          0x00000004
#define HT_FLAG_USE_8BPP_BITMASK        0x00000008
#define HT_FLAG_INK_HIGH_ABSORPTION     0x00000010
#define HT_FLAG_INK_ABSORPTION_INDICES  0x00000060
#define HT_FLAG_DO_DEVCLR_XFORM         0x00000080
#define HT_FLAG_OUTPUT_CMY              0x00000100
#define HT_FLAG_PRINT_DRAFT_MODE        0x00000200
#define HT_FLAG_INVERT_8BPP_BITMASK_IDX 0x00000400
#define HT_FLAG_8BPP_CMY332_MASK        0xFF000000

#define MAKE_CMYMASK_BYTE(c,m,y)    ((BYTE)(((BYTE)(c) & 0x07) << 5) |      \
                                     (BYTE)(((BYTE)(m) & 0x07) << 2) |      \
                                     (BYTE)((BYTE)(y) & 0x03))

#define MAKE_CMY332_MASK(c,m,y)     ((DWORD)(((DWORD)(c) & 0x07) << 29) |   \
                                     (DWORD)(((DWORD)(m) & 0x07) << 26) |   \
                                     (DWORD)(((DWORD)(y) & 0x03) << 24))


#define HT_FLAG_INK_ABSORPTION_IDX0     0x00000000
#define HT_FLAG_INK_ABSORPTION_IDX1     0x00000020
#define HT_FLAG_INK_ABSORPTION_IDX2     0x00000040
#define HT_FLAG_INK_ABSORPTION_IDX3     0x00000060

#define HT_FLAG_HIGHEST_INK_ABSORPTION  (HT_FLAG_INK_HIGH_ABSORPTION    |   \
                                         HT_FLAG_INK_ABSORPTION_IDX3)
#define HT_FLAG_HIGHER_INK_ABSORPTION   (HT_FLAG_INK_HIGH_ABSORPTION    |   \
                                         HT_FLAG_INK_ABSORPTION_IDX2)
#define HT_FLAG_HIGH_INK_ABSORPTION     (HT_FLAG_INK_HIGH_ABSORPTION    |   \
                                         HT_FLAG_INK_ABSORPTION_IDX1)
#define HT_FLAG_NORMAL_INK_ABSORPTION   HT_FLAG_INK_ABSORPTION_IDX0
#define HT_FLAG_LOW_INK_ABSORPTION      (HT_FLAG_INK_ABSORPTION_IDX1)
#define HT_FLAG_LOWER_INK_ABSORPTION    (HT_FLAG_INK_ABSORPTION_IDX2)
#define HT_FLAG_LOWEST_INK_ABSORPTION   (HT_FLAG_INK_ABSORPTION_IDX3)

// Setting/checking halftone 8bpp bitmask RGB mode

#define HT_BITMASKPALRGB                (DWORD)'0BGR'
#define HT_SET_BITMASKPAL2RGB(pPal)     (*((LPDWORD)(pPal)) = HT_BITMASKPALRGB)
#define HT_IS_BITMASKPALRGB(pPal)       (*((LPDWORD)(pPal)) == (DWORD)0)

// Allowed values for GDIINFO.ulPhysicalPixelCharacteristics

#define PPC_DEFAULT                        0x0
#define PPC_UNDEFINED                      0x1
#define PPC_RGB_ORDER_VERTICAL_STRIPES     0x2
#define PPC_BGR_ORDER_VERTICAL_STRIPES     0x3
#define PPC_RGB_ORDER_HORIZONTAL_STRIPES   0x4
#define PPC_BGR_ORDER_HORIZONTAL_STRIPES   0x5

// GDIINFO.ulPhysicalPixelGamma should be set either to the scaled
// gamma (x1000) of the physical pixel or one of the following
// values.  For example, a 2.2 gamma would be represented as 2200

#define PPG_DEFAULT                        0
#define PPG_SRGB                           1

//============================================================================
//  HALFTONE INFORMATION
//============================================================================
//
// HT_FLAG_DO_DEVCLR_XFORM flag specified devices and/or drivers are required
// gdi halftone to do a device transform when ICM is off, the device transform
// take a input RGB value and map it to the device color space to produced
// perceived density same as input RGB color values.  This flag only valid if
// it is a printer device surface and its format is 16bpp or 24bpp.
//
// ** If devices are additive and/or device surface is 1bpp, 4bpp or 8bpp, then
//    gdi halftone will always performed device transform regardless the
//    setting of HT_FLAG_DO_DEVCLR_XFORM flag.
//
// HT_FLAG_8BPP_CMY332_MASK - define the bit mask for 8-bpp format halftone
// palette, this palette only used if a HT_FORMAT_8BPP is specified in
// flHTOutputFormat and HT_FLAG_USE_8BPP_BITMASK bit is set in the flHTFlags.
// if HT_FLAG_USE_8BPP_BITMAP is not set then a standard NT4.0 8-bpp format is
// assumed.  The format of 8BPP is set per PDEV, it cannot be changed after
// the pDEV is created.
//
// To set HT_FLAG_8BPP_CMY332_MASK, macro MAKE_CMY332_MASK() can be used.  When
// specified, the bit mask cover total of 8 bits area which describe the
// highest of level of CYAN, MAGENTA and YELLOW primary color, the maximum of
// Cyan is 3 bits (7 levels), Magenta is 3 bits (7 levels) and Yellow is 2 bit
// (3 levels).
//
//     CMYMask Meaning
//     ======= =======
//     0x6F    Cyan=3, Magenta=3, Yellow=3
//     0xFF    Cyan=7, Magenta=7, Yellow=3
//     0x25    Cyan=1, Magenta=1, Yellow=1, is same as using CMY 4bpp
//
//     SPECIAL
//     CMYMask Meagning
//     ======= =======
//     0x00    Gray scale 256 levels
//     0x01    5 Levels (0-4) each of Cyan, Magenta and yellow, 5^3=125 color
//     0x02    6 Levels (0-5) each of Cyan, Magenta and yellow, 6^3=216 color
//
// Any other invalid combinations (any of Cyan, Magenta, Yellow level bits is
// 0 (zero) then it will returned palette entries 0
//
// The Palette indics are arranged as CMY entries with CYAN at highest bit
// numbers and YELLOW at lowest bit number as shown below for palette indics
// bit's (8 bit) definition.
//
// Pallete Index BIT#:  7   6   5   4   3   2   1   0
//                      |       |   |       |   |   |
//                      +---C---+   +---M---+   +-Y-+
//                          |           |         |
//                          |           |        0x03
//                          |           |         +-- Yellow 0-3, Max=4 levels
//                          |          0x1c
//                          |           +-- Magenta 0-7, Max=8 levels
//                        0xe0
//                          +-- Cyan 0-7, Max=8 levels
//
//
// If a primray color level in an index is greater than the prmary color
// levels then it is eqaul to the maximum primary color level, for example
// if C=7 (0xe0), and Cyan level only 5 then level 6 and 7 are same as 5
//
// To retrieve the palette definition for 8bpp format, use
//
// LONG APIENTRY
// HT_Get8BPPMaskPalette(PPALETTEENTRY pPaletteEntry,
//                       BOOL          Use8BPPMaskPal,
//                       BYTE          CMYMask,
//                       USHORT        RedGamma,
//                       USHORT        GreenGamma,
//                       USHORT        BlueGamma);
//
// pPaletteEntry   - Pointer to the PALETTEENTRY, if NULL it return palette
//                   count needed,
//
//                   * See note below for the pPaletteEntry[0] special settings.
//
// Use8BPPmaskPal  - FALSE if a NT4.0 standard 8bpp palette requested, FALSE
//                   if a CMYMask 8bpp mask palette is needed.
//
// CMYMask         - CMY bit mask for CMY as defined above.  This must be the
//                   same primary level as defined in GDIInfo.flHTFlags.
//                   It should be same as ((GDIInfo.flHTFlags >> 24) & 0xFF).
//                   The CMYMask also can be specified using predefined macro
//                   MAKS_CMYMASK_BYTE(cLevel, mLevel, yLevel)
//
// RedGamma        - Red gamma if Use8BPPMaskPal is FALSE, Cyan gamma
//                   if Use8BPPMaskPal is TRUE
//
// GreenGamma      - Green gamma if Use8BPPMaskPal is FALSE, Magenta gamma
//                   if Use8BPPMaskPal is TRUE
//
// BlueGamma       - Blue gamma if Use8BPPMaskPal is FALSE, Yellow gamma
//                   if Use8BPPMaskPal is TRUE
//
//
// ***************************************************************************
// * SPECIAL NOTE for Windows NT version later than Windows 2000 Release     *
// ***************************************************************************
// Current version of Window NT (Post Windows 2000) will Overloading the
// pPaletteEntry in HT_Get8BPPMaskPalette(DoUseCMYMask) API to returned a
// inverted indices palette based on additive palette entries composition.
// Because Windows GDI ROP assume index 0 always black and last index always
// white without checking the palette entries. (Indices based ROPs rather color
// based)  This cause many ROPS got wrong result which has inverted output.
//
// To correct this GDI ROPs behavior, the POST windows 2000 version of GDI
// Halftone will supports a special CMY_INVERTED format. All new drivers should
// use this CMY_INVERTED method for future compabilities
//
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
// @   Following Steps are required for ALL POST Windows 2000 Drivers when    @
// @   using Window GDI Halftone 8bpp CMY332 Mask mode                        @
// @@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
//
// 1. Must set HT_FLAG_INVERT_8BPP_BITMASK_IDX flags
//
// 2. Must set pPaleteEntry[0] when calling HT_Get8BPPMaskPalette() with
//
//     pPaletteEntry[0].peRed   = 'R';
//     pPaletteEntry[0].peGreen = 'G';
//     pPaletteEntry[0].peBlue  = 'B';
//     pPaletteEntry[0].peFlags = '0';
//
//     The caller can use following supplied macro to set this for future
//     compabilities
//
//         HT_SET_BITMASKPAL2RGB(pPaletteEntry)
//
//     where pPaletteEntry is the pointer to the PALETTEENTRY that passed to
//     the HT_GET8BPPMaskPalette() fucntion call
//
// 3. Must Check the return Palette from HT_Get8BPPMaskPalette() using
//    following Macro
//
//         HT_IS_BITMASKPALRGB(pPaletteEntry)
//
//    where pPaletteEntry is the pointer to the PALETTEENTRY that passed to the
//    HT_GET8BPPMaskPalette() fucntion call,
//
//    If this macro return FALSE then the current version of GDI HALFTONE does
//    NOT support the CMY_INVERTED 8bpp bitmaask mode and it only supports CMY
//    mode.
//
//    If this macro return TRUE then the GDI HALFTONE DOES support the
//    CMY_INVERTED 8bpp bitmaask mode and the caller must using a translation
//    table to obtain final halftone surface bitmap 8bpp indices ink levels.
//
// 4. Behavior changes for GDI halftone that supports 8bpp CMY_INVERTED bitmask
//    mode, following is a list of changes of CMYMask mode passed to
//    the HT_Get8BPPMaskPalette()
//
//    CMYMask      CMY Mode Indices          CMY_INVERTED Mode Indices
//    =======  =========================   =============================
//         0         0: WHITE                    0: BLACK
//               1-254: Light->Dark Gray     1-254: Dark->Light Gray
//                 255: BLACK                  255: WHITE
//    -------------------------------------------------------------------
//         1         0: WHITE                 0-65: BLACK
//               1-123: 5^3 CMY color       66-188: 5^3 RGB color
//             124-255: BLACK              189-255: WHITE
//                                         127-128: Duplicate for XOR ROP
//                                                  (CMY Levels 2:2:2)
//    -------------------------------------------------------------------
//         2         0: WHITE                 0-20: BLACK
//               1-214: 6^3 CMY color       21-234: 6^3 RGB color
//             215-255: BLACK              235-255: WHITE
//    -------------------------------------------------------------------
//     3-255*        0: WHITE                    0: BLACK
//               1-254: CMY Color BitMask    1-254: Centered CxMxY BitMask*
//                 255: BLACK                  255: WHITE
//    ===================================================================
//
//     * For CMYMask mode 3-255, the valid combination must NOT have any
//       of Cyan, Magenta or Yellow ink level equal to 0.
//
//     * The reason for CMY_INVERTED mode that pading BLACK and WHITE entires
//       at both end and have all other color in the middle is to make sure
//       all 256 color palette entries are even distributed so that GDI ROPs
//       (raster operation) will work more correctly. This is because GDI ROPs
//       are based on the indices not color
//
//     * The CMY_INVERTED Mode has all non-black, non white indices centered
//       and even distributed within the total 256 palette indices.  For
//       example; if a CMY=333 levels then it has total 3x3x3=27 indices,
//       these 27 indices will be centered by packing 114 black indices at
//       begining and packing 114 white indices at end to ensure that ROP
//       will be correct rendered.
//
//       See following sample function of for how to generate these ink levels
//       and Windows 2000 CMY332 Index translation table
//
//
// 5. For CMYMask index mode 0 to 255, the caller can use following sample
//    function to genrate INKLEVELS translation table
//
//    The follwing structure and tables are examples of how to translate 8bpp
//    bitmask halftone bitmap indices to ink levels
//
//        typedef struct _INKLEVELS {
//             BYTE    Cyan;        // Cyan level from 0 to max
//             BYTE    Magenta;     // Magenta level from 0 to max
//             BYTE    Yellow;      // Yellow level from 0 to max
//             BYTE    CMY332Idx;   // Original windows 2000 CMY332 Index
//             } INKLEVELS, *PINKLEVELS;
//
//     To Compute a 8bpp translate table of INKLEVELS, following sample
//     function show how to genrate a INKLEVELS translate table for a valid
//     CMYMask range from 0 to 255.  It can be use to generate either Windows
//     2000 CMY Mode or new Post Windows 2000's CMY_INVERTED mode translation
//     table.  It also generate a windows 2000 CMY Mode CMY332Idx so caller
//     can map CMY_INVERTED new indices to old index for current existing
//     indices processing function.
//
//     Example Function that generate translate table for CMYMask 0 to 255,
//     the pInkLevels must pointed to a valid memory location of 256 INKLEVELS
//     entries, if return value is TRUE then it can be used to trnaslate 8bpp
//     indices to ink levels or mapp to the older CMY332 style indices.
//
//     
//     BOOL
//     GenerateInkLevels(
//         PINKLEVELS  pInkLevels,     // Pointer to 256 INKLEVELS table
//         BYTE        CMYMask,        // CMYMask mode
//         BOOL        CMYInverted     // TRUE for CMY_INVERTED mode
//         )
//     {
//         PINKLEVELS  pILDup;
//         PINKLEVELS  pILEnd;
//         INKLEVELS   InkLevels;
//         INT         Count;
//         INT         IdxInc;
//         INT         cC;
//         INT         cM;
//         INT         cY;
//         INT         xC;
//         INT         xM;
//         INT         xY;
//         INT         iC;
//         INT         iM;
//         INT         iY;
//         INT         mC;
//         INT         mM;
//
//
//         switch (CMYMask) {
//
//         case 0:
//
//             cC =
//             cM =
//             xC =
//             xM = 0;
//             cY =
//             xY = 255;
//             break;
//
//         case 1:
//         case 2:
//
//             cC =
//             cM =
//             cY =
//             xC =
//             xM =
//             xY = 3 + (INT)CMYMask;
//             break;
//
//         default:
//
//             cC = (INT)((CMYMask >> 5) & 0x07);
//             cM = (INT)((CMYMask >> 2) & 0x07);
//             cY = (INT)( CMYMask       & 0x03);
//             xC = 7;
//             xM = 7;
//             xY = 3;
//             break;
//         }
//
//         Count = (cC + 1) * (cM + 1) * (cY + 1);
//
//         if ((Count < 1) || (Count > 256)) {
//
//             return(FALSE);
//         }
//
//         InkLevels.Cyan      =
//         InkLevels.Magenta   =
//         InkLevels.Yellow    =
//         InkLevels.CMY332Idx = 0;
//         mC                  = (xM + 1) * (xY + 1);
//         mM                  = xY + 1;
//         pILDup              = NULL;
//
//         if (CMYInverted) {
//
//             //
//             // Move the pInkLevels to the first entry which center around
//             // 256 table entries, if we skip any then all entries skipped
//             // will be white (CMY levels all zeros).  Because this is
//             // CMY_INVERTED so entries start from back of the table and
//             // moving backward to the begining of the table
//             //
//
//             pILEnd      = pInkLevels - 1;
//             IdxInc      = ((256 - Count - (Count & 0x01)) / 2);
//             pInkLevels += 255;
//
//             while (IdxInc--) {
//
//                 *pInkLevels-- = InkLevels;
//             }
//
//             if (Count & 0x01) {
//
//                 //
//                 // If we have odd number of entries then we need to
//                 // duplicate the center one for correct XOR ROP to
//                 // operated correctly. The pILDup will always be index
//                 // 127, the duplication are indices 127, 128
//                 //
//
//                 pILDup = pInkLevels - (Count / 2) - 1;
//             }
//
//             //
//             // We running from end of table to the begining, because
//             // when in CMYInverted mode, the index 0 is black and index
//             // 255 is white.  Since we only generate 'Count' of index
//             // and place them at center, we will change xC, xM, xY max.
//             // index to same as cC, cM and cY.
//             //
//
//             IdxInc = -1;
//             xC     = cC;
//             xM     = cM;
//             xY     = cY;
//
//         } else {
//
//             IdxInc = 1;
//             pILEnd = pInkLevels + 256;
//         }
//
//         //
//         // At following, the composition of ink levels, index always
//         // from 0 CMY Ink levels (WHITE) to maximum ink levels (BLACK),
//         // the different with CMY_INVERTED mode is we compose it from
//         // index 255 to index 0 rather than from index 0 to 255
//         //
//
//         if (CMYMask) {
//
//             INT Idx332C;
//             INT Idx332M;
//
//             for (iC = 0, Idx332C = -mC; iC <= xC; iC++) {
//
//                 if (iC <= cC) {
//
//                     InkLevels.Cyan  = (BYTE)iC;
//                     Idx332C        += mC;
//                 }
//
//                 for (iM = 0, Idx332M = -mM; iM <= xM; iM++) {
//
//                     if (iM <= cM) {
//
//                         InkLevels.Magenta  = (BYTE)iM;
//                         Idx332M           += mM;
//                     }
//
//                     for (iY = 0; iY <= xY; iY++) {
//
//                         if (iY <= cY) {
//
//                             InkLevels.Yellow = (BYTE)iY;
//                         }
//
//                         InkLevels.CMY332Idx = (BYTE)(Idx332C + Idx332M) +
//                                               InkLevels.Yellow;
//                         *pInkLevels         = InkLevels;
//
//                         if ((pInkLevels += IdxInc) == pILDup) {
//
//                             *pInkLevels  = InkLevels;
//                             pInkLevels  += IdxInc;
//                         }
//                     }
//                 }
//             }
//
//             //
//             // Now if we need to pack black at other end of the
//             // translation table then do it here, Notice that InkLevels
//             // are at cC, cM and cY here and the CMY332Idx is at BLACK
//             //
//
//             while (pInkLevels != pILEnd) {
//
//                 *pInkLevels  = InkLevels;
//                 pInkLevels  += IdxInc;
//             }
//
//         } else {
//
//             //
//             // Gray Scale case
//             //
//
//             for (iC = 0; iC < 256; iC++, pInkLevels += IdxInc) {
//
//                 pInkLevels->Cyan      =
//                 pInkLevels->Magenta   =
//                 pInkLevels->Yellow    =
//                 pInkLevels->CMY332Idx = (BYTE)iC;
//             }
//         }
//
//         return(TRUE);
//     }
//
//
// 6. For CMYMask Mode 0 (Gray scale), the gray scale table just inverted
//    between CMY and CMY_INVERTED mode.
//
//     CMY mode:          0 to 255 gray scale from WHITE to BLACK increment,
//     CMY_INVERTED Mode: 0 to 255 gray scale from BLACK to WHITE increment.
//
//
// 7. For CMYMask Mode 1 and 2, the caller should use a translation table for
//    translate indices to CMY ink levels.
//
// 8. For CMYMode mode 3 to 255,
//
//    if in CMY Mode (Windows 2000) is specified then The final CMY ink levels
//    indices byte has following meanings
//
//         Bit     7 6 5 4 3 2 1 0
//                 |   | |   | | |
//                 +---+ +---+ +=+
//                   |     |    |
//                   |     |    +-- Yellow 0-3 (Max. 4 levels)
//                   |     |
//                   |     +-- Magenta 0-7 (Max. 8 levels)
//                   |
//                   +-- Cyan 0-7 (Max. 8 levels)
//
//
//     If a CMY_INVERTED mode is specified then caller must use a translation
//     table to convert a index to the ink levels, to generate this table,
//     please see above #5 description.
//
//============================================================================
//  END HALFTONE INFORMATION
//============================================================================

typedef struct _GDIINFO
{
    ULONG ulVersion;
    ULONG ulTechnology;
    ULONG ulHorzSize;
    ULONG ulVertSize;
    ULONG ulHorzRes;
    ULONG ulVertRes;
    ULONG cBitsPixel;
    ULONG cPlanes;
    ULONG ulNumColors;
    ULONG flRaster;
    ULONG ulLogPixelsX;
    ULONG ulLogPixelsY;
    ULONG flTextCaps;

    ULONG ulDACRed;
    ULONG ulDACGreen;
    ULONG ulDACBlue;

    ULONG ulAspectX;
    ULONG ulAspectY;
    ULONG ulAspectXY;

    LONG  xStyleStep;
    LONG  yStyleStep;
    LONG  denStyleStep;

    POINTL ptlPhysOffset;
    SIZEL  szlPhysSize;

    ULONG ulNumPalReg;

// These fields are for halftone initialization.

    COLORINFO ciDevice;
    ULONG     ulDevicePelsDPI;
    ULONG     ulPrimaryOrder;
    ULONG     ulHTPatternSize;
    ULONG     ulHTOutputFormat;
    ULONG     flHTFlags;

    ULONG ulVRefresh;
    ULONG ulBltAlignment;

    ULONG ulPanningHorzRes;
    ULONG ulPanningVertRes;
    ULONG xPanningAlignment;
    ULONG yPanningAlignment;

// The following fields are for user defined halftone dither patterns.  These
// fields are only checked if ulHTPatternSize is eqaul to HT_PATSIZE_USER.
//
// The user defined pHTPatA, pHTPatB, pHTPatC pointers correspond to the primary
// color order defined in ulPrimaryOrder as PRIMARY_ORDER_xxx.
//
// The size of halftone dither pattern must range from 4 to 256.  For each
// dither pattern, pHTPatA, pHTPatB, pHTPatC must point to a
// valid byte array of (cxHTPat x cyHTPat) size.  pHTPatA, pHTPatB and
// pHTPatC may point to the same dither pattern array.
//
// Each byte threshold within the dither pattern defines the additive
// intensity threshold of pixels.  A zero threshold value indicates the pixel
// location is ignored (always black), while 1 to 255 threshold values give the
// dither pattern 255 level of grays.

    ULONG   cxHTPat;    // cxHTPat must range from 4-256
    ULONG   cyHTPat;    // cyHTPat must range from 4-256
    LPBYTE  pHTPatA;    // for Primary Color Order A
    LPBYTE  pHTPatB;    // for Primary Color Order B
    LPBYTE  pHTPatC;    // for Primary Color Order C

// Shade and blend caps

    ULONG   flShadeBlend;

    ULONG   ulPhysicalPixelCharacteristics;
    ULONG   ulPhysicalPixelGamma;

} GDIINFO, *PGDIINFO;

/*
 * User objects
 */

typedef struct _BRUSHOBJ
{
    ULONG  iSolidColor;
    PVOID  pvRbrush;
    FLONG  flColorType;
} BRUSHOBJ;

//
// BRUSHOBJ::flColorType
//
#define BR_DEVICE_ICM    0x01
#define BR_HOST_ICM      0x02
#define BR_CMYKCOLOR     0x04
#define BR_ORIGCOLOR     0x08

typedef struct _CLIPOBJ
{
    ULONG   iUniq;
    RECTL   rclBounds;
    BYTE    iDComplexity;
    BYTE    iFComplexity;
    BYTE    iMode;
    BYTE    fjOptions;
} CLIPOBJ;

typedef struct _DRIVEROBJ DRIVEROBJ;

typedef BOOL (CALLBACK * FREEOBJPROC)(DRIVEROBJ *pDriverObj);

typedef struct _DRIVEROBJ
{
    PVOID       pvObj;
    FREEOBJPROC pFreeProc;
    HDEV        hdev;
    DHPDEV      dhpdev;
} DRIVEROBJ;

typedef struct _FONTOBJ
{
    ULONG      iUniq;
    ULONG      iFace;
    ULONG      cxMax;
    FLONG      flFontType;
    ULONG_PTR   iTTUniq;
    ULONG_PTR   iFile;
    SIZE       sizLogResPpi;
    ULONG      ulStyleSize;
    PVOID      pvConsumer;
    PVOID      pvProducer;
} FONTOBJ;

typedef struct _BLENDOBJ
{
    BLENDFUNCTION BlendFunction;
}BLENDOBJ,*PBLENDOBJ;

typedef BYTE GAMMA_TABLES[2][256];

//
// FONTOBJ::flFontType
//
#define FO_TYPE_RASTER   RASTER_FONTTYPE     /* 0x1 */
#define FO_TYPE_DEVICE   DEVICE_FONTTYPE     /* 0x2 */
#define FO_TYPE_TRUETYPE TRUETYPE_FONTTYPE   /* 0x4 */
#define FO_TYPE_OPENTYPE OPENTYPE_FONTTYPE   /* 0X8 */

#define FO_SIM_BOLD      0x00002000
#define FO_SIM_ITALIC    0x00004000
#define FO_EM_HEIGHT     0x00008000
#define FO_GRAY16        0x00010000          /* [1] */
#define FO_NOGRAY16      0x00020000          /* [1] */
#define FO_NOHINTS       0x00040000          /* [3] */
#define FO_NO_CHOICE     0x00080000          /* [3] */

// new accelerators so that printer drivers  do not need to look to ifimetrics

#define FO_CFF            0x00100000
#define FO_POSTSCRIPT     0x00200000
#define FO_MULTIPLEMASTER 0x00400000
#define FO_VERT_FACE      0x00800000
#define FO_DBCS_FONT      0X01000000

// cleartype flags for horizontally or vertically striped LCD screen

#define FO_NOCLEARTYPE    0x02000000
#define FO_CLEARTYPE_X    0x10000000
#define FO_CLEARTYPE_Y    0x20000000
#define FO_CLEARTYPENATURAL_X    0x40000000

/**************************************************************************\
*
*   [1]
*
*   If the FO_GRAY16 flag is set then the bitmaps of the font
*   are 4-bit per pixel blending (alpha) values. A value of zero
*   means that the the resulting pixel should be equal to the
*   background color. If the value of the alpha value is k != 0
*   then the resulting pixel must be:
*
*       c0 = background color
*       c1 = foreground color
*       b  = blending value = (k+1)/16  // {k = 1,2,..,15}
*       b  = 0 (k = 0)
*       d0 = gamma[c0], d1 = gamma[c1]  // luminance components
*       d = (1 - b)*d0 + b*d1           // blended luminance
*       c = lambda[d]                   // blended device voltage
*
*   where gamma[] takes a color component from application space
*   to CIE space and labmda[] takes a color from CIE space to
*   device color space
*
*   GDI will set this bit if it request a font be gray scaled
*   to 16 values then GDI will set FO_GRAY16 upon entry to
*   DrvQueryFontData().  If the font driver cannot (or will
*   not) grayscale a particular realization of a font then the
*   font provider will zero out FO_GRAY16  and set FO_NOGRAY16
*   to inform GDI that
*   the gray scaling request cannot (or should not) be
*   satisfied.
*
*   [2]
*
*   The FO_NOHINTS indicates that hints were not used in the formation
*   of the glyph images. GDI will set this bit to request that hinting
*   be supressed. The font provider will set this bit accroding to the
*   rendering scheme that it used in generating the glyph image.
*
*   [3]
*
*   The FO_NO_CHOICE flag indicates that the flags FO_GRAY16 and
*   FO_NOHINTS must be obeyed if at all possible.
*
\**************************************************************************/

typedef struct _PALOBJ
{
    ULONG   ulReserved;
} PALOBJ;

typedef struct _PATHOBJ
{
    FLONG   fl;
    ULONG   cCurves;
} PATHOBJ;

typedef struct _SURFOBJ
{
    DHSURF  dhsurf;
    HSURF   hsurf;
    DHPDEV  dhpdev;
    HDEV    hdev;
    SIZEL   sizlBitmap;
    ULONG   cjBits;
    PVOID   pvBits;
    PVOID   pvScan0;
    LONG    lDelta;
    ULONG   iUniq;
    ULONG   iBitmapFormat;
    USHORT  iType;
    USHORT  fjBitmap;
} SURFOBJ;

typedef struct _WNDOBJ
{
    CLIPOBJ  coClient;
    PVOID    pvConsumer;
    RECTL    rclClient;
    SURFOBJ *psoOwner;
} WNDOBJ, *PWNDOBJ;

typedef struct _XFORMOBJ
{
    ULONG ulReserved;
} XFORMOBJ;

typedef struct _XLATEOBJ
{
    ULONG   iUniq;
    FLONG   flXlate;
    USHORT  iSrcType;               // Obsolete
    USHORT  iDstType;               // Obsolete
    ULONG   cEntries;
    ULONG  *pulXlate;
} XLATEOBJ;

/*
 * BRUSHOBJ callbacks
 */

ENGAPI
PVOID APIENTRY BRUSHOBJ_pvAllocRbrush(
    BRUSHOBJ *pbo,
    ULONG     cj
    );

ENGAPI
PVOID APIENTRY BRUSHOBJ_pvGetRbrush(
    BRUSHOBJ *pbo
    );

ENGAPI
ULONG APIENTRY BRUSHOBJ_ulGetBrushColor(
    BRUSHOBJ *pbo
    );

ENGAPI
HANDLE APIENTRY BRUSHOBJ_hGetColorTransform(
    BRUSHOBJ *pbo
    );

/*
 * CLIPOBJ callbacks
 */

#define DC_TRIVIAL      0
#define DC_RECT         1
#define DC_COMPLEX      3

#define FC_RECT         1
#define FC_RECT4        2
#define FC_COMPLEX      3

#define TC_RECTANGLES   0
#define TC_PATHOBJ      2

#define OC_BANK_CLIP    1       // Obsolete

#define CT_RECTANGLES   0L

#define CD_RIGHTDOWN    0L
#define CD_LEFTDOWN     1L
#define CD_RIGHTUP      2L
#define CD_LEFTUP       3L
#define CD_ANY          4L

#define CD_LEFTWARDS    1L
#define CD_UPWARDS      2L

typedef struct _ENUMRECTS
{
    ULONG       c;
    RECTL       arcl[1];
} ENUMRECTS;

ENGAPI
ULONG APIENTRY CLIPOBJ_cEnumStart(
    CLIPOBJ *pco,
    BOOL     bAll,
    ULONG    iType,
    ULONG    iDirection,
    ULONG    cLimit
    );

ENGAPI
BOOL APIENTRY CLIPOBJ_bEnum(
    CLIPOBJ *pco,
    ULONG    cj,
    ULONG   *pul
    );

PATHOBJ* APIENTRY CLIPOBJ_ppoGetPath(
    CLIPOBJ* pco
    );

/*
 *   FONTOBJ callbacks
 */

typedef struct _GLYPHBITS
{
    POINTL      ptlOrigin;
    SIZEL       sizlBitmap;
    BYTE        aj[1];
} GLYPHBITS;

#define FO_HGLYPHS          0L
#define FO_GLYPHBITS        1L
#define FO_PATHOBJ          2L

#define FD_NEGATIVE_FONT    1L

#define FO_DEVICE_FONT      1L
#define FO_OUTLINE_CAPABLE  2L

typedef union _GLYPHDEF
{
    GLYPHBITS  *pgb;
    PATHOBJ    *ppo;
} GLYPHDEF;

typedef struct _GLYPHPOS    /* gp */
{
    HGLYPH      hg;
    GLYPHDEF   *pgdf;
    POINTL      ptl;
} GLYPHPOS,*PGLYPHPOS;


// individual glyph data

// r is a unit vector along the baseline in device coordinates.
// s is a unit vector in the ascent direction in device coordinates.
// A, B, and C, are simple tranforms of the notional space versions into
// (28.4) device coordinates.  The dot products of those vectors with r
// are recorded here.  Note that the high words of ptqD are also 28.4
// device coordinates.  The low words provide extra accuracy.

// THE STRUCTURE DIFFERS IN ORDERING FROM NT 3.51 VERSION OF THE STRUCTURE.
// ptqD has been moved to the bottom.
// This requires only recompile of all the drivers.

typedef struct _GLYPHDATA {
        GLYPHDEF gdf;               // pointer to GLYPHBITS or to PATHOBJ
        HGLYPH   hg;                // glyhp handle
        FIX      fxD;               // Character increment amount: D*r.
        FIX      fxA;               // Prebearing amount: A*r.
        FIX      fxAB;              // Advancing edge of character: (A+B)*r.
        FIX      fxInkTop;          // Baseline to inkbox top along s.
        FIX      fxInkBottom;       // Baseline to inkbox bottom along s.
        RECTL    rclInk;            // Ink box with sides parallel to x,y axes
        POINTQF  ptqD;              // Character increment vector: D=A+B+C.
} GLYPHDATA;


// flAccel flags for STROBJ

// SO_FLAG_DEFAULT_PLACEMENT // defult inc vectors used to position chars
// SO_HORIZONTAL             // "left to right" or "right to left"
// SO_VERTICAL               // "top to bottom" or "bottom to top"
// SO_REVERSED               // set if horiz & "right to left" or if vert &  "bottom to top"
// SO_ZERO_BEARINGS          // all glyphs have zero a and c spaces
// SO_CHAR_INC_EQUAL_BM_BASE // base == cx for horiz, == cy for vert.
// SO_MAXEXT_EQUAL_BM_SIDE   // side == cy for horiz, == cx for vert.

// do not substitute device font for tt font even if device font sub table
// tells the driver this should be done

// SO_DO_NOT_SUBSTITUTE_DEVICE_FONT

#define SO_FLAG_DEFAULT_PLACEMENT        0x00000001
#define SO_HORIZONTAL                    0x00000002
#define SO_VERTICAL                      0x00000004
#define SO_REVERSED                      0x00000008
#define SO_ZERO_BEARINGS                 0x00000010
#define SO_CHAR_INC_EQUAL_BM_BASE        0x00000020
#define SO_MAXEXT_EQUAL_BM_SIDE          0x00000040
#define SO_DO_NOT_SUBSTITUTE_DEVICE_FONT 0x00000080
#define SO_GLYPHINDEX_TEXTOUT            0x00000100
#define SO_ESC_NOT_ORIENT                0x00000200
#define SO_DXDY                          0x00000400
#define SO_CHARACTER_EXTRA               0x00000800
#define SO_BREAK_EXTRA                   0x00001000

typedef struct _STROBJ
{
    ULONG     cGlyphs;     // # of glyphs to render
    FLONG     flAccel;     // accel flags
    ULONG     ulCharInc;   // non-zero only if fixed pitch font, equal to advanced width.
    RECTL     rclBkGround; // bk ground  rect of the string in device coords
    GLYPHPOS *pgp;         // If non-NULL then has all glyphs.
    LPWSTR    pwszOrg;     // pointer to original unicode string.
} STROBJ;

typedef struct _FONTINFO /* fi */
{
    ULONG   cjThis;
    FLONG   flCaps;
    ULONG   cGlyphsSupported;
    ULONG   cjMaxGlyph1;
    ULONG   cjMaxGlyph4;
    ULONG   cjMaxGlyph8;
    ULONG   cjMaxGlyph32;
} FONTINFO, *PFONTINFO;

ULONG APIENTRY FONTOBJ_cGetAllGlyphHandles(
    FONTOBJ *pfo,
    HGLYPH  *phg
    );

VOID APIENTRY FONTOBJ_vGetInfo(
    FONTOBJ  *pfo,
    ULONG     cjSize,
    FONTINFO *pfi
    );

ULONG APIENTRY FONTOBJ_cGetGlyphs(
    FONTOBJ *pfo,
    ULONG    iMode,
    ULONG    cGlyph,
    HGLYPH  *phg,
    PVOID   *ppvGlyph
    );

XFORMOBJ * APIENTRY FONTOBJ_pxoGetXform(
    FONTOBJ *pfo
    );

IFIMETRICS * APIENTRY FONTOBJ_pifi(
    FONTOBJ *pfo
    );

FD_GLYPHSET * APIENTRY FONTOBJ_pfdg(
    FONTOBJ *pfo
    );

PVOID APIENTRY FONTOBJ_pvTrueTypeFontFile(
    FONTOBJ *pfo,
    ULONG   *pcjFile
    );

PBYTE APIENTRY FONTOBJ_pjOpenTypeTablePointer (
    FONTOBJ *pfo,
    ULONG    ulTag,
    ULONG   *pcjTable
    );

LPWSTR APIENTRY FONTOBJ_pwszFontFilePaths (
    FONTOBJ *pfo,
    ULONG   *pcwc
    );

// for now only one mode is defined for glyph attributes

#define FO_ATTR_MODE_ROTATE         1

PFD_GLYPHATTR APIENTRY FONTOBJ_pQueryGlyphAttrs(
    FONTOBJ       *pfo,
    ULONG          iMode
    );

/*
 * PALOBJ callbacks
 */

#define PAL_INDEXED       0x00000001
#define PAL_BITFIELDS     0x00000002
#define PAL_RGB           0x00000004
#define PAL_BGR           0x00000008
#define PAL_CMYK          0x00000010

ULONG APIENTRY PALOBJ_cGetColors(
    PALOBJ *ppalo,
    ULONG   iStart,
    ULONG   cColors,
    ULONG  *pulColors
    );

/*
 * PATHOBJ callbacks
 */

#define PO_BEZIERS          0x00000001
#define PO_ELLIPSE          0x00000002
#define PO_ALL_INTEGERS     0x00000004
#define PO_ENUM_AS_INTEGERS 0x00000008

#define PD_BEGINSUBPATH   0x00000001
#define PD_ENDSUBPATH     0x00000002
#define PD_RESETSTYLE     0x00000004
#define PD_CLOSEFIGURE    0x00000008
#define PD_BEZIERS        0x00000010
#define PD_ALL           (PD_BEGINSUBPATH | \
                          PD_ENDSUBPATH   | \
                          PD_RESETSTYLE   | \
                          PD_CLOSEFIGURE  | \
                          PD_BEZIERS)

typedef struct  _PATHDATA
{
    FLONG    flags;
    ULONG    count;
    POINTFIX *pptfx;
} PATHDATA, *PPATHDATA;

typedef struct  _RUN
{
    LONG    iStart;
    LONG    iStop;
} RUN, *PRUN;

typedef struct  _CLIPLINE
{
    POINTFIX ptfxA;
    POINTFIX ptfxB;
    LONG    lStyleState;
    ULONG   c;
    RUN     arun[1];
} CLIPLINE, *PCLIPLINE;

ENGAPI
VOID APIENTRY PATHOBJ_vEnumStart(
    PATHOBJ *ppo
    );

ENGAPI
BOOL APIENTRY PATHOBJ_bEnum(
    PATHOBJ  *ppo,
    PATHDATA *ppd
    );

ENGAPI
VOID APIENTRY PATHOBJ_vEnumStartClipLines(
    PATHOBJ   *ppo,
    CLIPOBJ   *pco,
    SURFOBJ   *pso,
    LINEATTRS *pla
    );

ENGAPI
BOOL APIENTRY PATHOBJ_bEnumClipLines(
    PATHOBJ  *ppo,
    ULONG     cb,
    CLIPLINE *pcl
    );

ENGAPI
BOOL APIENTRY PATHOBJ_bMoveTo(
    PATHOBJ    *ppo,
    POINTFIX    ptfx
    );

ENGAPI
BOOL APIENTRY PATHOBJ_bPolyLineTo(
    PATHOBJ   *ppo,
    POINTFIX  *pptfx,
    ULONG      cptfx
    );

ENGAPI
BOOL APIENTRY PATHOBJ_bPolyBezierTo(
    PATHOBJ   *ppo,
    POINTFIX  *pptfx,
    ULONG      cptfx
    );

ENGAPI
BOOL APIENTRY PATHOBJ_bCloseFigure(
    PATHOBJ *ppo
    );

ENGAPI
VOID APIENTRY PATHOBJ_vGetBounds(
    PATHOBJ *ppo,
    PRECTFX prectfx
    );

/*
 * STROBJ callbacks
 */

ENGAPI
VOID APIENTRY STROBJ_vEnumStart(
    STROBJ *pstro
    );

ENGAPI
BOOL APIENTRY STROBJ_bEnum(
    STROBJ    *pstro,
    ULONG     *pc,
    PGLYPHPOS *ppgpos
    );

ENGAPI
BOOL APIENTRY STROBJ_bEnumPositionsOnly(
    STROBJ    *pstro,
    ULONG     *pc,
    PGLYPHPOS *ppgpos
    );

ENGAPI
DWORD APIENTRY STROBJ_dwGetCodePage(
    STROBJ  *pstro
    );

ENGAPI
FIX APIENTRY STROBJ_fxCharacterExtra(
    STROBJ  *pstro
    );

ENGAPI
FIX APIENTRY STROBJ_fxBreakExtra(
    STROBJ  *pstro
    );

ENGAPI
BOOL APIENTRY STROBJ_bGetAdvanceWidths(
    STROBJ   *pso,
    ULONG     iFirst,
    ULONG     c,
    POINTQF  *pptqD
    );


#define SGI_EXTRASPACE 0

/*
 * SURFOBJ callbacks
 */

#define STYPE_BITMAP    0L
#define STYPE_DEVICE    1L
#define STYPE_DEVBITMAP 3L

#define BMF_1BPP       1L
#define BMF_4BPP       2L
#define BMF_8BPP       3L
#define BMF_16BPP      4L
#define BMF_24BPP      5L
#define BMF_32BPP      6L
#define BMF_4RLE       7L
#define BMF_8RLE       8L
#define BMF_JPEG       9L
#define BMF_PNG       10L

#define BMF_TOPDOWN    0x0001
#define BMF_NOZEROINIT 0x0002
#define BMF_DONTCACHE  0x0004
#define BMF_USERMEM    0x0008
#define BMF_KMSECTION  0x0010
#define BMF_NOTSYSMEM  0x0020
#define BMF_WINDOW_BLT 0x0040
#define BMF_UMPDMEM    0x0080
#define BMF_RESERVED   0xFF00

/*
 * XFORMOBJ callbacks
 */

#define GX_IDENTITY     0L
#define GX_OFFSET       1L
#define GX_SCALE        2L
#define GX_GENERAL      3L

#define XF_LTOL         0L
#define XF_INV_LTOL     1L
#define XF_LTOFX        2L
#define XF_INV_FXTOL    3L

ENGAPI
ULONG APIENTRY XFORMOBJ_iGetXform(
    XFORMOBJ *pxo,
    XFORML   *pxform
    );

ENGAPI
BOOL APIENTRY XFORMOBJ_bApplyXform(
    XFORMOBJ *pxo,
    ULONG     iMode,
    ULONG     cPoints,
    PVOID     pvIn,
    PVOID     pvOut
    );

DECLSPEC_DEPRECATED_DDK
ENGAPI
HANDLE APIENTRY XFORMOBJ_cmGetTransform(
    XFORMOBJ *pxo
    );

/*
 * XLATEOBJ callbacks
 */

#define XO_TRIVIAL      0x00000001
#define XO_TABLE        0x00000002
#define XO_TO_MONO      0x00000004
#define XO_FROM_CMYK    0x00000008
#define XO_DEVICE_ICM   0x00000010 // ICM on Device
#define XO_HOST_ICM     0x00000020 // ICM on Engine/Apps

#define XO_SRCPALETTE    1
#define XO_DESTPALETTE   2
#define XO_DESTDCPALETTE 3
#define XO_SRCBITFIELDS  4
#define XO_DESTBITFIELDS 5

ENGAPI
ULONG APIENTRY XLATEOBJ_iXlate(
    XLATEOBJ *pxlo,
    ULONG iColor
    );

ENGAPI
ULONG * APIENTRY XLATEOBJ_piVector(
    XLATEOBJ *pxlo
    );

ENGAPI
ULONG APIENTRY XLATEOBJ_cGetPalette(
    XLATEOBJ *pxlo,
    ULONG     iPal,
    ULONG     cPal,
    ULONG    *pPal
    );

ENGAPI
HANDLE APIENTRY XLATEOBJ_hGetColorTransform(
    XLATEOBJ *pxlo
    );

/*
 * Engine callbacks - error logging
 */

#ifdef USERMODE_DRIVER

#define EngGetLastError     GetLastError
#define EngSetLastError     SetLastError

#else // !USERMODE_DRIVER

ENGAPI
VOID APIENTRY EngSetLastError(ULONG);

ENGAPI
ULONG APIENTRY EngGetLastError();

#endif // !USERMODE_DRIVER

/*
 * Engine callbacks - Surfaces
 */

#define HOOK_BITBLT                     0x00000001
#define HOOK_STRETCHBLT                 0x00000002
#define HOOK_PLGBLT                     0x00000004
#define HOOK_TEXTOUT                    0x00000008
#define HOOK_PAINT                      0x00000010      // Obsolete
#define HOOK_STROKEPATH                 0x00000020
#define HOOK_FILLPATH                   0x00000040
#define HOOK_STROKEANDFILLPATH          0x00000080
#define HOOK_LINETO                     0x00000100
#define HOOK_COPYBITS                   0x00000400
#define HOOK_MOVEPANNING                0x00000800      // Obsolete
#define HOOK_SYNCHRONIZE                0x00001000
#define HOOK_STRETCHBLTROP              0x00002000
#define HOOK_SYNCHRONIZEACCESS          0x00004000      // Obsolete
#define HOOK_TRANSPARENTBLT             0x00008000
#define HOOK_ALPHABLEND                 0x00010000
#define HOOK_GRADIENTFILL               0x00020000
#define HOOK_FLAGS                      0x0003b5ff

ENGAPI
HBITMAP APIENTRY EngCreateBitmap(
    SIZEL sizl,
    LONG  lWidth,
    ULONG iFormat,
    FLONG fl,
    PVOID pvBits
    );

ENGAPI
HSURF APIENTRY EngCreateDeviceSurface(
    DHSURF dhsurf,
    SIZEL sizl,
    ULONG iFormatCompat
    );

ENGAPI
HBITMAP APIENTRY EngCreateDeviceBitmap(
    DHSURF dhsurf,
    SIZEL sizl,
    ULONG iFormatCompat
    );

ENGAPI
BOOL APIENTRY EngDeleteSurface(
    HSURF hsurf
    );

ENGAPI
SURFOBJ * APIENTRY EngLockSurface(
    HSURF hsurf
    );

ENGAPI
VOID APIENTRY EngUnlockSurface(
    SURFOBJ *pso
    );

ENGAPI
BOOL APIENTRY EngEraseSurface(
    SURFOBJ *pso,
    RECTL   *prcl,
    ULONG    iColor
    );

ENGAPI
BOOL APIENTRY EngAssociateSurface(
    HSURF hsurf,
    HDEV  hdev,
    FLONG flHooks
    );

#define MS_NOTSYSTEMMEMORY  0x0001
#define MS_SHAREDACCESS     0x0002

ENGAPI
BOOL APIENTRY EngModifySurface(
    HSURF   hsurf,
    HDEV    hdev,
    FLONG   flHooks,
    FLONG   flSurface,
    DHSURF  dhsurf,
    VOID*   pvScan0,
    LONG    lDelta,
    VOID*   pvReserved
    );

ENGAPI
BOOL APIENTRY EngMarkBandingSurface(
    HSURF hsurf
    );

ENGAPI
BOOL APIENTRY EngCheckAbort(
    SURFOBJ *pso
    );

/*
 * Engine callbacks - Paths
 */

ENGAPI
PATHOBJ * APIENTRY EngCreatePath();

ENGAPI
VOID APIENTRY EngDeletePath(
    PATHOBJ *ppo
    );

/*
 * Engine callbacks - Palettes
 */

ENGAPI
HPALETTE APIENTRY EngCreatePalette(
    ULONG  iMode,
    ULONG  cColors,
    ULONG *pulColors,
    FLONG  flRed,
    FLONG  flGreen,
    FLONG  flBlue
    );

ENGAPI
ULONG APIENTRY EngQueryPalette(
    HPALETTE    hpal,
    ULONG      *piMode,
    ULONG       cColors,
    ULONG      *pulColors);

ENGAPI
BOOL APIENTRY EngDeletePalette(
    HPALETTE hpal
    );

/*
 * Engine callbacks - Clipping
 */

ENGAPI
CLIPOBJ * APIENTRY EngCreateClip();

ENGAPI
VOID APIENTRY EngDeleteClip(
    CLIPOBJ *pco
    );

/*
 * Function prototypes
 */

//
// User-mode printer driver information-query entrypoint
//

BOOL APIENTRY
APIENTRY
DrvQueryDriverInfo(
    DWORD   dwMode,
    PVOID   pBuffer,
    DWORD   cbBuf,
    PDWORD  pcbNeeded
    );

#define DRVQUERY_USERMODE   1


// These are the only EXPORTED functions for ANY driver

BOOL APIENTRY DrvEnableDriver(
    ULONG          iEngineVersion,
    ULONG          cj,
    DRVENABLEDATA *pded
    );

/*
 * Driver functions
 */

VOID APIENTRY  DrvDisableDriver();

DHPDEV APIENTRY DrvEnablePDEV(
    DEVMODEW *pdm,
    LPWSTR    pwszLogAddress,
    ULONG     cPat,
    HSURF    *phsurfPatterns,
    ULONG     cjCaps,
    ULONG    *pdevcaps,
    ULONG     cjDevInfo,
    DEVINFO  *pdi,
    HDEV      hdev,
    LPWSTR    pwszDeviceName,
    HANDLE    hDriver
    );

#define HS_DDI_MAX 6

ULONG APIENTRY DrvResetDevice(
    DHPDEV dhpdev,
    PVOID Reserved
    );

#define DRD_SUCCESS         0
#define DRD_ERROR           1

BOOL APIENTRY DrvResetPDEV(
    DHPDEV dhpdevOld,
    DHPDEV dhpdevNew
    );

VOID APIENTRY DrvCompletePDEV(
    DHPDEV dhpdev,
    HDEV hdev
    );

BOOL APIENTRY DrvOffset(        // Obsolete
    SURFOBJ* pso,
    LONG x,
    LONG y,
    FLONG flReserved
    );

HSURF APIENTRY DrvEnableSurface(
    DHPDEV dhpdev
    );

VOID APIENTRY DrvSynchronize(
    DHPDEV dhpdev,
    RECTL *prcl
    );

VOID APIENTRY DrvDisableSurface(
    DHPDEV dhpdev
    );

VOID APIENTRY DrvDisablePDEV(
    DHPDEV dhpdev
    );

/* DrvSaveScreenBits - iMode definitions */

#define SS_SAVE    0
#define SS_RESTORE 1
#define SS_FREE    2

ULONG_PTR APIENTRY DrvSaveScreenBits(
    SURFOBJ  *pso,
    ULONG    iMode,
    ULONG_PTR ident,
    RECTL    *prcl
    );

/*
 * Desktops
 */

BOOL APIENTRY DrvAssertMode(
    DHPDEV dhpdev,
    BOOL   bEnable
    );

ULONG APIENTRY DrvGetModes(
    HANDLE    hDriver,
    ULONG     cjSize,
    DEVMODEW *pdm
    );

DECLSPEC_DEPRECATED_DDK
VOID APIENTRY DrvMovePanning(
    LONG    x,
    LONG    y,
    FLONG   fl
    );

BOOL APIENTRY DrvPlgBlt(
    SURFOBJ         *psoTrg,
    SURFOBJ         *psoSrc,
    SURFOBJ         *psoMsk,
    CLIPOBJ         *pco,
    XLATEOBJ        *pxlo,
    COLORADJUSTMENT *pca,
    POINTL          *pptlBrushOrg,
    POINTFIX        *pptfx,
    RECTL           *prcl,
    POINTL          *pptl,
    ULONG            iMode
    );


/*
 * Bitmaps
 */

HBITMAP APIENTRY DrvCreateDeviceBitmap(
    DHPDEV dhpdev,
    SIZEL  sizl,
    ULONG  iFormat
    );

VOID APIENTRY DrvDeleteDeviceBitmap(
    DHSURF dhsurf
    );

/*
 * Palettes
 */

BOOL APIENTRY DrvSetPalette(
    DHPDEV  dhpdev,
    PALOBJ *ppalo,
    FLONG   fl,
    ULONG   iStart,
    ULONG   cColors
    );

/*
 * Brushes
 */

#define DM_DEFAULT    0x00000001
#define DM_MONOCHROME 0x00000002

#define DCR_SOLID       0
#define DCR_DRIVER      1
#define DCR_HALFTONE    2

ULONG APIENTRY DrvDitherColor(
    DHPDEV dhpdev,
    ULONG  iMode,
    ULONG  rgb,
    ULONG *pul
    );

BOOL APIENTRY DrvRealizeBrush(
    BRUSHOBJ *pbo,
    SURFOBJ  *psoTarget,
    SURFOBJ  *psoPattern,
    SURFOBJ  *psoMask,
    XLATEOBJ *pxlo,
    ULONG    iHatch
    );

#define RB_DITHERCOLOR 0x80000000L


/*
 * Fonts
 */

PIFIMETRICS APIENTRY DrvQueryFont(
    DHPDEV    dhpdev,
    ULONG_PTR  iFile,
    ULONG     iFace,
    ULONG_PTR *pid
    );

// #define QFT_UNICODE     0L
#define QFT_LIGATURES       1L
#define QFT_KERNPAIRS       2L
#define QFT_GLYPHSET        3L

PVOID APIENTRY DrvQueryFontTree(
    DHPDEV    dhpdev,
    ULONG_PTR  iFile,
    ULONG     iFace,
    ULONG     iMode,
    ULONG_PTR *pid
    );

#define QFD_GLYPHANDBITMAP    1L
#define QFD_GLYPHANDOUTLINE   2L
#define QFD_MAXEXTENTS        3L
#define QFD_TT_GLYPHANDBITMAP 4L
#define QFD_TT_GRAY1_BITMAP   5L
#define QFD_TT_GRAY2_BITMAP   6L
#define QFD_TT_GRAY4_BITMAP   8L
#define QFD_TT_GRAY8_BITMAP   9L

#define QFD_TT_MONO_BITMAP QFD_TT_GRAY1_BITMAP

LONG APIENTRY DrvQueryFontData(
    DHPDEV      dhpdev,
    FONTOBJ    *pfo,
    ULONG       iMode,
    HGLYPH      hg,
    GLYPHDATA  *pgd,
    PVOID       pv,
    ULONG       cjSize
    );

VOID APIENTRY DrvFree(
PVOID   pv,
ULONG_PTR id);

VOID APIENTRY DrvDestroyFont(
FONTOBJ *pfo);

// Capability flags for DrvQueryCaps.

#define QC_OUTLINES             0x00000001
#define QC_1BIT                 0x00000002
#define QC_4BIT                 0x00000004

//
// This is a mask of the capabilites of a font provider that can return more
// than just glyph metrics (i.e., bitmaps and/or outlines).  If a driver has
// one or more of these capabilities, then it is FONT DRIVER.
//
// Drivers should only set individual bits. GDI will check if any are turned on
// using this define.
//

#define QC_FONTDRIVERCAPS   ( QC_OUTLINES | QC_1BIT | QC_4BIT )

LONG APIENTRY DrvQueryFontCaps(
    ULONG   culCaps,
    ULONG  *pulCaps
    );

// fStatus is a new flag NT 5.0

#define FF_SIGNATURE_VERIFIED 0x1
#define FF_IGNORED_SIGNATURE  0x2

ULONG_PTR APIENTRY DrvLoadFontFile(
    ULONG     cFiles,  // number of font files associated with this font
    ULONG_PTR  *piFile,  // handles for individual files, cFiles of them
    PVOID     *ppvView, // array of cFiles views
    ULONG     *pcjView, // array of their sizes
    DESIGNVECTOR *pdv, // only non null for mm instances
    ULONG     ulLangID,
    ULONG     ulFastCheckSum
    );

BOOL APIENTRY DrvUnloadFontFile(
    ULONG_PTR   iFile
    );

LONG APIENTRY DrvQueryTrueTypeTable(
    ULONG_PTR   iFile,
    ULONG      ulFont,
    ULONG      ulTag,
    PTRDIFF    dpStart,
    ULONG      cjBuf,
    BYTE       *pjBuf,
    PBYTE      *ppjTable,
    ULONG      *pcjTable
    );

BOOL APIENTRY DrvQueryAdvanceWidths(
    DHPDEV   dhpdev,
    FONTOBJ *pfo,
    ULONG    iMode,
    HGLYPH  *phg,
    PVOID    pvWidths,
    ULONG    cGlyphs
    );

// Values for iMode

#define QAW_GETWIDTHS       0
#define QAW_GETEASYWIDTHS   1

// values for bMetricsOnly. even though declared as BOOL
// by adding TTO_QUBICS, this is becoming a flag field.
// For versions of NT 4.0 and earlier, this value is always
// set to zero by GDI.

#define TTO_METRICS_ONLY 1
#define TTO_QUBICS       2
#define TTO_UNHINTED     4

LONG APIENTRY DrvQueryTrueTypeOutline(
    DHPDEV      dhpdev,
    FONTOBJ    *pfo,
    HGLYPH      hglyph,
    BOOL        bMetricsOnly,
    GLYPHDATA  *pgldt,
    ULONG       cjBuf,
    TTPOLYGONHEADER *ppoly
    );

PVOID APIENTRY DrvGetTrueTypeFile (
    ULONG_PTR   iFile,
    ULONG      *pcj
    );

// values for ulMode:

#define QFF_DESCRIPTION     1L
#define QFF_NUMFACES        2L

LONG APIENTRY DrvQueryFontFile(
    ULONG_PTR   iFile,
    ULONG      ulMode,
    ULONG      cjBuf,
    ULONG      *pulBuf
    );

/*
 * BitBlt
 */

BOOL APIENTRY DrvBitBlt(
    SURFOBJ  *psoTrg,
    SURFOBJ  *psoSrc,
    SURFOBJ  *psoMask,
    CLIPOBJ  *pco,
    XLATEOBJ *pxlo,
    RECTL    *prclTrg,
    POINTL   *pptlSrc,
    POINTL   *pptlMask,
    BRUSHOBJ *pbo,
    POINTL   *pptlBrush,
    ROP4      rop4
    );

BOOL APIENTRY DrvStretchBlt(
    SURFOBJ         *psoDest,
    SURFOBJ         *psoSrc,
    SURFOBJ         *psoMask,
    CLIPOBJ         *pco,
    XLATEOBJ        *pxlo,
    COLORADJUSTMENT *pca,
    POINTL          *pptlHTOrg,
    RECTL           *prclDest,
    RECTL           *prclSrc,
    POINTL          *pptlMask,
    ULONG            iMode
    );

BOOL APIENTRY DrvStretchBltROP(
    SURFOBJ         *psoDest,
    SURFOBJ         *psoSrc,
    SURFOBJ         *psoMask,
    CLIPOBJ         *pco,
    XLATEOBJ        *pxlo,
    COLORADJUSTMENT *pca,
    POINTL          *pptlHTOrg,
    RECTL           *prclDest,
    RECTL           *prclSrc,
    POINTL          *pptlMask,
    ULONG            iMode,
    BRUSHOBJ        *pbo,
    DWORD            rop4
    );

BOOL APIENTRY DrvAlphaBlend(
    SURFOBJ       *psoDest,
    SURFOBJ       *psoSrc,
    CLIPOBJ       *pco,
    XLATEOBJ      *pxlo,
    RECTL         *prclDest,
    RECTL         *prclSrc,
    BLENDOBJ      *pBlendObj
    );

BOOL APIENTRY DrvGradientFill(
    SURFOBJ         *psoDest,
    CLIPOBJ         *pco,
    XLATEOBJ        *pxlo,
    TRIVERTEX       *pVertex,
    ULONG            nVertex,
    PVOID            pMesh,
    ULONG            nMesh,
    RECTL           *prclExtents,
    POINTL          *pptlDitherOrg,
    ULONG            ulMode
    );

BOOL APIENTRY DrvTransparentBlt(
    SURFOBJ    *psoDst,
    SURFOBJ    *psoSrc,
    CLIPOBJ    *pco,
    XLATEOBJ   *pxlo,
    RECTL      *prclDst,
    RECTL      *prclSrc,
    ULONG      iTransColor,
    ULONG      ulReserved
);

BOOL APIENTRY DrvCopyBits(
    SURFOBJ  *psoDest,
    SURFOBJ  *psoSrc,
    CLIPOBJ  *pco,
    XLATEOBJ *pxlo,
    RECTL    *prclDest,
    POINTL   *pptlSrc
    );

/*
 * Text Output
 */

BOOL APIENTRY DrvTextOut(
    SURFOBJ  *pso,
    STROBJ   *pstro,
    FONTOBJ  *pfo,
    CLIPOBJ  *pco,
    RECTL    *prclExtra,        // Obsolete, always NULL
    RECTL    *prclOpaque,
    BRUSHOBJ *pboFore,
    BRUSHOBJ *pboOpaque,
    POINTL   *pptlOrg,
    MIX       mix
    );

/*
 * Graphics Output
 */

BOOL APIENTRY DrvLineTo(
    SURFOBJ   *pso,
    CLIPOBJ   *pco,
    BRUSHOBJ  *pbo,
    LONG       x1,
    LONG       y1,
    LONG       x2,
    LONG       y2,
    RECTL     *prclBounds,
    MIX        mix
    );

BOOL APIENTRY DrvStrokePath(
    SURFOBJ   *pso,
    PATHOBJ   *ppo,
    CLIPOBJ   *pco,
    XFORMOBJ  *pxo,
    BRUSHOBJ  *pbo,
    POINTL    *pptlBrushOrg,
    LINEATTRS *plineattrs,
    MIX        mix
    );

#define FP_ALTERNATEMODE    1L
#define FP_WINDINGMODE      2L

BOOL APIENTRY DrvFillPath(
    SURFOBJ  *pso,
    PATHOBJ  *ppo,
    CLIPOBJ  *pco,
    BRUSHOBJ *pbo,
    POINTL   *pptlBrushOrg,
    MIX       mix,
    FLONG     flOptions
    );

BOOL APIENTRY DrvStrokeAndFillPath(
    SURFOBJ   *pso,
    PATHOBJ   *ppo,
    CLIPOBJ   *pco,
    XFORMOBJ  *pxo,
    BRUSHOBJ  *pboStroke,
    LINEATTRS *plineattrs,
    BRUSHOBJ  *pboFill,
    POINTL    *pptlBrushOrg,
    MIX        mixFill,
    FLONG      flOptions
    );

BOOL APIENTRY DrvPaint(
    SURFOBJ  *pso,
    CLIPOBJ  *pco,
    BRUSHOBJ *pbo,
    POINTL   *pptlBrushOrg,
    MIX       mix
    );

/*
 * Pointers
 */

#define SPS_ERROR               0
#define SPS_DECLINE             1
#define SPS_ACCEPT_NOEXCLUDE    2
#define SPS_ACCEPT_EXCLUDE      3               // Obsolete
#define SPS_ACCEPT_SYNCHRONOUS  4

#define SPS_CHANGE          0x00000001L
#define SPS_ASYNCCHANGE     0x00000002L           // Obsolete
#define SPS_ANIMATESTART    0x00000004L
#define SPS_ANIMATEUPDATE   0x00000008L
#define SPS_ALPHA           0x00000010L
#define SPS_LENGTHMASK      0x00000F00L
#define SPS_FREQMASK        0x000FF000L


ULONG APIENTRY DrvSetPointerShape(
    SURFOBJ  *pso,
    SURFOBJ  *psoMask,
    SURFOBJ  *psoColor,
    XLATEOBJ *pxlo,
    LONG      xHot,
    LONG      yHot,
    LONG      x,
    LONG      y,
    RECTL    *prcl,
    FLONG     fl
    );

VOID APIENTRY DrvMovePointer(
    SURFOBJ  *pso,
    LONG      x,
    LONG      y,
    RECTL    *prcl
    );

/*
 * Printing
 */

BOOL APIENTRY DrvSendPage(
    SURFOBJ *pso
    );

BOOL APIENTRY DrvStartPage(
    SURFOBJ *pso
    );

ULONG APIENTRY DrvEscape(
    SURFOBJ *pso,
    ULONG    iEsc,
    ULONG    cjIn,
    PVOID    pvIn,
    ULONG    cjOut,
    PVOID    pvOut
    );

BOOL  APIENTRY DrvStartDoc(
    SURFOBJ *pso,
    LPWSTR   pwszDocName,
    DWORD    dwJobId
    );

#define ED_ABORTDOC    1

BOOL APIENTRY DrvEndDoc(
    SURFOBJ *pso,
    FLONG fl
    );

DECLSPEC_DEPRECATED_DDK
BOOL APIENTRY DrvQuerySpoolType(
    DHPDEV,
    LPWSTR
    );

ULONG APIENTRY DrvDrawEscape(
    SURFOBJ *pso,
    ULONG    iEsc,
    CLIPOBJ *pco,
    RECTL   *prcl,
    ULONG    cjIn,
    PVOID    pvIn
    );

ULONG APIENTRY DrvGetGlyphMode(
    DHPDEV,
    FONTOBJ *
    );

ULONG APIENTRY DrvFontManagement(
    SURFOBJ *pso,
    FONTOBJ *pfo,
    ULONG    iMode,
    ULONG    cjIn,
    PVOID    pvIn,
    ULONG    cjOut,
    PVOID    pvOut
    );

BOOL APIENTRY DrvStartBanding(
    SURFOBJ *pso,
    POINTL *pptl
    );

BOOL APIENTRY DrvNextBand(
    SURFOBJ *pso,
    POINTL *pptl
    );

typedef struct _PERBANDINFO
{
    BOOL  bRepeatThisBand;
    SIZEL szlBand;
    ULONG ulHorzRes;
    ULONG ulVertRes;
} PERBANDINFO, *PPERBANDINFO;

ULONG APIENTRY DrvQueryPerBandInfo(
    SURFOBJ *pso,
    PERBANDINFO *pbi
    );

/*
 * DirectDraw
 */

BOOL APIENTRY DrvEnableDirectDraw(
    DHPDEV                  dhpdev,
    DD_CALLBACKS           *pCallBacks,
    DD_SURFACECALLBACKS    *pSurfaceCallBacks,
    DD_PALETTECALLBACKS    *pPaletteCallBacks
    );

VOID APIENTRY DrvDisableDirectDraw(
    DHPDEV  dhpdev
    );

BOOL APIENTRY DrvGetDirectDrawInfo(
    DHPDEV        dhpdev,
    DD_HALINFO   *pHalInfo,
    DWORD        *pdwNumHeaps,
    VIDEOMEMORY  *pvmList,
    DWORD        *pdwNumFourCCCodes,
    DWORD        *pdwFourCC
    );

HBITMAP APIENTRY DrvDeriveSurface(
    DD_DIRECTDRAW_GLOBAL   *pDirectDraw,
    DD_SURFACE_LOCAL       *pSurface
    );

/*
 * ICM (Image Color Matching)
 */

HANDLE APIENTRY DrvIcmCreateColorTransform(
    DHPDEV           dhpdev,
    LPLOGCOLORSPACEW pLogColorSpace,
    PVOID            pvSourceProfile,
    ULONG            cjSourceProfile,
    PVOID            pvDestProfile,
    ULONG            cjDestProfile,
    PVOID            pvTargetProfile,
    ULONG            cjTargetProfile,
    DWORD            dwReserved
    );

BOOL APIENTRY DrvIcmDeleteColorTransform(
    DHPDEV dhpdev,
    HANDLE hcmXform
    );

BOOL APIENTRY DrvIcmCheckBitmapBits(
    DHPDEV   dhpdev,
    HANDLE   hColorTransform,
    SURFOBJ *pso,
    PBYTE    paResults
    );

typedef struct _GAMMARAMP {
    WORD Red[256];
    WORD Green[256];
    WORD Blue[256];
} GAMMARAMP, *PGAMMARAMP;

BOOL APIENTRY DrvIcmSetDeviceGammaRamp(
    DHPDEV  dhpdev,
    ULONG   iFormat,
    LPVOID  lpRamp
    );

#define IGRF_RGB_256BYTES  0 /* Obsolate */
#define IGRF_RGB_256WORDS  1 /* Same GammaRampArray format as Win95/98 DDI */

/*
 * Query miscellaneous driver support
 */

BOOL APIENTRY DrvQueryDeviceSupport(
    SURFOBJ *pso,
    XLATEOBJ *pxlo,
    XFORMOBJ *pxo,
    ULONG iType,
    ULONG cjIn,
    PVOID pvIn,
    ULONG cjOut,
    PVOID pvOut);

#define QDS_CHECKJPEGFORMAT     0
#define QDS_CHECKPNGFORMAT      1

VOID APIENTRY DrvSynchronizeSurface(
    SURFOBJ *pso,
    RECTL   *prcl,
    FLONG    fl
);

#define DSS_TIMER_EVENT        0x0001
#define DSS_FLUSH_EVENT        0x0002

VOID APIENTRY DrvNotify(
    SURFOBJ *pso,
    ULONG    iType,
    PVOID    pvData);

#define DN_ACCELERATION_LEVEL     1
#define DN_DEVICE_ORIGIN          2
#define DN_SLEEP_MODE             3
#define DN_DRAWING_BEGIN          4

/*
 * Engine callbacks - tracking clip region changes
 */

#define WOC_RGN_CLIENT_DELTA    0x0001
#define WOC_RGN_CLIENT          0x0002
#define WOC_RGN_SURFACE_DELTA   0x0004
#define WOC_RGN_SURFACE         0x0008
#define WOC_CHANGED             0x0010
#define WOC_DELETE              0x0020
#define WOC_DRAWN               0x0040
#define WOC_SPRITE_OVERLAP      0x0080
#define WOC_SPRITE_NO_OVERLAP   0x0100

typedef VOID (CALLBACK * WNDOBJCHANGEPROC)(WNDOBJ *pwo, FLONG fl);

#define WO_RGN_CLIENT_DELTA     0x0001
#define WO_RGN_CLIENT           0x0002
#define WO_RGN_SURFACE_DELTA    0x0004
#define WO_RGN_SURFACE          0x0008
#define WO_RGN_UPDATE_ALL       0x0010
#define WO_RGN_WINDOW           0x0020
#define WO_DRAW_NOTIFY          0x0040
#define WO_SPRITE_NOTIFY        0x0080
#define WO_RGN_DESKTOP_COORD    0x0100

ENGAPI
WNDOBJ * APIENTRY EngCreateWnd(
    SURFOBJ         *pso,
    HWND             hwnd,
    WNDOBJCHANGEPROC pfn,
    FLONG            fl,
    int              iPixelFormat
    );

ENGAPI
VOID APIENTRY EngDeleteWnd(
    WNDOBJ  *pwo
    );

ENGAPI
ULONG APIENTRY WNDOBJ_cEnumStart(
    WNDOBJ  *pwo,
    ULONG    iType,
    ULONG    iDirection,
    ULONG    cLimit
    );

ENGAPI
BOOL APIENTRY WNDOBJ_bEnum(
    WNDOBJ  *pwo,
    ULONG    cj,
    ULONG   *pul
    );

ENGAPI
VOID APIENTRY WNDOBJ_vSetConsumer(
    WNDOBJ  *pwo,
    PVOID    pvConsumer
    );

/*
 * Engine callback - hung device notification
 */

#define EHN_RESTORED        0
#define EHN_ERROR           1

ENGAPI
ULONG APIENTRY EngHangNotification(
    HDEV hdev,
    PVOID Reserved
    );

/*
 * Engine callbacks - tracking driver managed resources
 */

ENGAPI
HDRVOBJ APIENTRY EngCreateDriverObj(
    PVOID pvObj,
    FREEOBJPROC pFreeObjProc,
    HDEV hdev
    );

ENGAPI
BOOL APIENTRY EngDeleteDriverObj(
    HDRVOBJ hdo,
    BOOL bCallBack,
    BOOL bLocked
    );

ENGAPI
DRIVEROBJ* APIENTRY EngLockDriverObj(
    HDRVOBJ hdo
    );

ENGAPI
BOOL APIENTRY EngUnlockDriverObj(
    HDRVOBJ hdo
    );

/*
 * Engine callback - return current process handle.
 */

ENGAPI
HANDLE APIENTRY EngGetProcessHandle();

/*
 * Engine callback - return current thread id
 */

ENGAPI
HANDLE APIENTRY EngGetCurrentThreadId();

/*
 * Engine callback - return current process id
 */

ENGAPI
HANDLE APIENTRY EngGetCurrentProcessId();

/*
 * Pixel formats
 */

BOOL APIENTRY DrvSetPixelFormat(
    SURFOBJ *pso,
    LONG     iPixelFormat,
    HWND     hwnd
    );

LONG APIENTRY DrvDescribePixelFormat(
    DHPDEV   dhpdev,
    LONG     iPixelFormat,
    ULONG    cjpfd,
    PIXELFORMATDESCRIPTOR *ppfd
    );

/*
 * Swap buffers
 */

BOOL APIENTRY DrvSwapBuffers(
    SURFOBJ *pso,
    WNDOBJ  *pwo
    );

/*
 * Function prototypes - Engine Simulations
 */

ENGAPI
BOOL APIENTRY EngBitBlt(
    SURFOBJ  *psoTrg,
    SURFOBJ  *psoSrc,
    SURFOBJ  *psoMask,
    CLIPOBJ  *pco,
    XLATEOBJ *pxlo,
    RECTL    *prclTrg,
    POINTL   *pptlSrc,
    POINTL   *pptlMask,
    BRUSHOBJ *pbo,
    POINTL   *pptlBrush,
    ROP4      rop4
    );

ENGAPI
BOOL APIENTRY EngLineTo(
    SURFOBJ   *pso,
    CLIPOBJ   *pco,
    BRUSHOBJ  *pbo,
    LONG       x1,
    LONG       y1,
    LONG       x2,
    LONG       y2,
    RECTL     *prclBounds,
    MIX        mix
    );

ENGAPI
BOOL APIENTRY EngStretchBlt(
    SURFOBJ         *psoDest,
    SURFOBJ         *psoSrc,
    SURFOBJ         *psoMask,
    CLIPOBJ         *pco,
    XLATEOBJ        *pxlo,
    COLORADJUSTMENT *pca,
    POINTL          *pptlHTOrg,
    RECTL           *prclDest,
    RECTL           *prclSrc,
    POINTL          *pptlMask,
    ULONG            iMode
    );

ENGAPI
BOOL APIENTRY EngStretchBltROP(
    SURFOBJ         *psoDest,
    SURFOBJ         *psoSrc,
    SURFOBJ         *psoMask,
    CLIPOBJ         *pco,
    XLATEOBJ        *pxlo,
    COLORADJUSTMENT *pca,
    POINTL          *pptlHTOrg,
    RECTL           *prclDest,
    RECTL           *prclSrc,
    POINTL          *pptlMask,
    ULONG            iMode,
    BRUSHOBJ        *pbo,
    DWORD            rop4
    );

ENGAPI
BOOL APIENTRY EngAlphaBlend(
    SURFOBJ       *psoDest,
    SURFOBJ       *psoSrc,
    CLIPOBJ       *pco,
    XLATEOBJ      *pxlo,
    RECTL         *prclDest,
    RECTL         *prclSrc,
    BLENDOBJ      *pBlendObj
    );

ENGAPI
BOOL APIENTRY EngGradientFill(
    SURFOBJ         *psoDest,
    CLIPOBJ         *pco,
    XLATEOBJ        *pxlo,
    TRIVERTEX       *pVertex,
    ULONG            nVertex,
    PVOID            pMesh,
    ULONG            nMesh,
    RECTL           *prclExtents,
    POINTL          *pptlDitherOrg,
    ULONG            ulMode
    );

ENGAPI
BOOL APIENTRY EngTransparentBlt(
    SURFOBJ    *psoDst,
    SURFOBJ    *psoSrc,
    CLIPOBJ    *pco,
    XLATEOBJ   *pxlo,
    RECTL      *prclDst,
    RECTL      *prclSrc,
    ULONG      iTransColor,
    ULONG      ulReserved
    );

ENGAPI
BOOL APIENTRY EngTextOut(
    SURFOBJ  *pso,
    STROBJ   *pstro,
    FONTOBJ  *pfo,
    CLIPOBJ  *pco,
    RECTL    *prclExtra,
    RECTL    *prclOpaque,
    BRUSHOBJ *pboFore,
    BRUSHOBJ *pboOpaque,
    POINTL   *pptlOrg,
    MIX       mix
    );

ENGAPI
BOOL APIENTRY EngStrokePath(
    SURFOBJ   *pso,
    PATHOBJ   *ppo,
    CLIPOBJ   *pco,
    XFORMOBJ  *pxo,
    BRUSHOBJ  *pbo,
    POINTL    *pptlBrushOrg,
    LINEATTRS *plineattrs,
    MIX        mix
    );

ENGAPI
BOOL APIENTRY EngFillPath(
    SURFOBJ  *pso,
    PATHOBJ  *ppo,
    CLIPOBJ  *pco,
    BRUSHOBJ *pbo,
    POINTL   *pptlBrushOrg,
    MIX       mix,
    FLONG     flOptions
    );

ENGAPI
BOOL APIENTRY EngStrokeAndFillPath(
    SURFOBJ   *pso,
    PATHOBJ   *ppo,
    CLIPOBJ   *pco,
    XFORMOBJ  *pxo,
    BRUSHOBJ  *pboStroke,
    LINEATTRS *plineattrs,
    BRUSHOBJ  *pboFill,
    POINTL    *pptlBrushOrg,
    MIX        mixFill,
    FLONG      flOptions
    );

ENGAPI
BOOL APIENTRY EngPaint(
    SURFOBJ  *pso,
    CLIPOBJ  *pco,
    BRUSHOBJ *pbo,
    POINTL   *pptlBrushOrg,
    MIX       mix
    );

ENGAPI
BOOL APIENTRY EngCopyBits(
    SURFOBJ  *psoDest,
    SURFOBJ  *psoSrc,
    CLIPOBJ  *pco,
    XLATEOBJ *pxlo,
    RECTL    *prclDest,
    POINTL   *pptlSrc
    );

ENGAPI
ULONG APIENTRY EngSetPointerShape(
    SURFOBJ  *pso,
    SURFOBJ  *psoMask,
    SURFOBJ  *psoColor,
    XLATEOBJ *pxlo,
    LONG      xHot,
    LONG      yHot,
    LONG      x,
    LONG      y,
    RECTL    *prcl,
    FLONG     fl
    );

ENGAPI
VOID APIENTRY EngMovePointer(
    SURFOBJ  *pso,
    LONG      x,
    LONG      y,
    RECTL    *prcl
    );

ENGAPI
BOOL APIENTRY EngPlgBlt(
    SURFOBJ         *psoTrg,
    SURFOBJ         *psoSrc,
    SURFOBJ         *psoMsk,
    CLIPOBJ         *pco,
    XLATEOBJ        *pxlo,
    COLORADJUSTMENT *pca,
    POINTL          *pptlBrushOrg,
    POINTFIX        *pptfx,
    RECTL           *prcl,
    POINTL          *pptl,
    ULONG            iMode
    );

ENGAPI
ULONG APIENTRY EngDitherColor(
    HDEV   hdev,
    ULONG  iMode,
    ULONG  rgb,
    ULONG *pul
    );

//
// Sprite control
//

#define ECS_TEARDOWN    0x0001
#define ECS_REDRAW      0x0002

ENGAPI
BOOL APIENTRY EngControlSprites(
    WNDOBJ  *pwo,
    FLONG    fl
    );


//
// Halftone releated APIs
//

LONG APIENTRY HT_ComputeRGBGammaTable(
    USHORT  GammaTableEntries,
    USHORT  GammaTableType,
    USHORT  RedGamma,
    USHORT  GreenGamma,
    USHORT  BlueGamma,
    LPBYTE  pGammaTable
    );

LONG APIENTRY HT_Get8BPPFormatPalette(
    LPPALETTEENTRY  pPaletteEntry,
    USHORT          RedGamma,
    USHORT          GreenGamma,
    USHORT          BlueGamma
    );

LONG APIENTRY HT_Get8BPPMaskPalette(
    LPPALETTEENTRY  pPaletteEntry,
    BOOL            Use8BPPMaskPal,
    BYTE            CMYMask,
    USHORT          RedGamma,
    USHORT          GreenGamma,
    USHORT          BlueGamma
    );

typedef struct _DEVHTINFO {
    DWORD       HTFlags;
    DWORD       HTPatternSize;
    DWORD       DevPelsDPI;
    COLORINFO   ColorInfo;
    } DEVHTINFO, *PDEVHTINFO;

#define DEVHTADJF_COLOR_DEVICE      0x00000001
#define DEVHTADJF_ADDITIVE_DEVICE   0x00000002

typedef struct _DEVHTADJDATA {
    DWORD       DeviceFlags;
    DWORD       DeviceXDPI;
    DWORD       DeviceYDPI;
    PDEVHTINFO  pDefHTInfo;
    PDEVHTINFO  pAdjHTInfo;
    } DEVHTADJDATA, *PDEVHTADJDATA;

LONG
APIENTRY
HTUI_DeviceColorAdjustment(
    LPSTR           pDeviceName,
    PDEVHTADJDATA   pDevHTAdjData
    );


//
// General support APIS
//

ENGAPI
VOID APIENTRY EngDebugBreak(
    VOID
    );

ENGAPI
VOID APIENTRY EngBugCheckEx(
    IN ULONG BugCheckCode,
    IN ULONG_PTR P1,
    IN ULONG_PTR P2,
    IN ULONG_PTR P3,
    IN ULONG_PTR P4
    );

ENGAPI
VOID APIENTRY EngDebugPrint(
    PCHAR StandardPrefix,
    PCHAR DebugMessage,
    va_list ap
    );

ENGAPI
VOID APIENTRY EngQueryPerformanceCounter(
    LONGLONG  *pPerformanceCount
    );

ENGAPI
VOID APIENTRY EngQueryPerformanceFrequency(
    LONGLONG  *pFrequency
    );

ENGAPI
BOOL APIENTRY EngSetPointerTag(
    HDEV       hdev,
    SURFOBJ   *psoMask,
    SURFOBJ   *psoColor,
    XLATEOBJ  *pxlo,
    FLONG      fl
    );

//
// Kernel mode memory operations
//

#define FL_ZERO_MEMORY      0x00000001
#define FL_NONPAGED_MEMORY  0x00000002

#ifdef USERMODE_DRIVER

#define EngAllocMem(flags, cj, tag) ((PVOID) GlobalAlloc(((flags) & FL_ZERO_MEMORY) ? GPTR : GMEM_FIXED, cj))
#define EngFreeMem(p)               GlobalFree((HGLOBAL) (p))
#define EngAllocUserMem(cj, tag)    ((PVOID) GlobalAlloc(GMEM_FIXED,cj))
#define EngFreeUserMem(p)           GlobalFree((HGLOBAL) (p))
#define EngAllocPrivateUserMem( psl, cj, tag) ((PVOID) GlobalAlloc(GMEM_FIXED,cj))
#define EngFreePrivateUserMem( psl, p)        GlobalFree((HGLOBAL) (p))
#define EngMulDiv                   MulDiv

#else // !USERMODE_DRIVER

ENGAPI
PVOID APIENTRY EngAllocMem(
    ULONG Flags,
    ULONG MemSize,
    ULONG Tag
    );

ENGAPI
VOID APIENTRY EngFreeMem(
    PVOID Mem
    );

ENGAPI
PVOID APIENTRY EngAllocUserMem(
    SIZE_T cj,
    ULONG tag
    );

ENGAPI
VOID APIENTRY EngFreeUserMem(
    PVOID pv
    );

ENGAPI
PVOID APIENTRY EngAllocPrivateUserMem(
    PDD_SURFACE_LOCAL psl,
    SIZE_T cj,
    ULONG tag
    );

ENGAPI
VOID
EngFreePrivateUserMem(
    PDD_SURFACE_LOCAL psl,
    PVOID pv
    );

ENGAPI
HRESULT
EngDxIoctl(
    ULONG ulIoctl,
    PVOID pBuffer,
    ULONG ulBufferSize
    );

ENGAPI
int APIENTRY EngMulDiv(
    int a,
    int b,
    int c
    );


#endif // !USERMODE_DRIVER

//
// User mode memory Operations
//

ENGAPI
VOID APIENTRY EngProbeForRead(
    PVOID Address,
    SIZE_T Length,
    ULONG Alignment
    );

ENGAPI
VOID APIENTRY EngProbeForReadAndWrite(
    PVOID Address,
    SIZE_T Length,
    ULONG Alignment
    );

ENGAPI
HANDLE APIENTRY EngSecureMem(
    PVOID Address,
    ULONG Length
    );

ENGAPI
VOID APIENTRY EngUnsecureMem(
    HANDLE hSecure
    );

ENGAPI
DWORD APIENTRY EngDeviceIoControl(
    HANDLE hDevice,
    DWORD dwIoControlCode,
    LPVOID lpInBuffer,
    DWORD nInBufferSize,
    LPVOID lpOutBuffer,
    DWORD nOutBufferSize,
    LPDWORD lpBytesReturned
    );

//
// Loading drivers and gettings entry points from them
//

#ifdef USERMODE_DRIVER

#define EngLoadImage(filename)               ((HANDLE) LoadLibraryW(filename))
#define EngFindImageProcAddress(h, procname) ((PVOID) GetProcAddress(h, procname))
#define EngUnloadImage(h)                    FreeLibrary((HMODULE) (h))

#else // !USERMODE_DRIVER

ENGAPI
HANDLE APIENTRY EngLoadImage(
    LPWSTR pwszDriver
    );

ENGAPI
PVOID APIENTRY EngFindImageProcAddress(
    HANDLE hModule,
    LPSTR lpProcName
    );

ENGAPI
VOID APIENTRY EngUnloadImage(
    HANDLE hModule
    );

ENGAPI
PVOID APIENTRY EngMapModule(
    HANDLE h,
    PULONG pSize
    );

#endif // !USERMODE_DRIVER


//
// callback for extra PDEV information
//

ENGAPI
LPWSTR APIENTRY EngGetPrinterDataFileName(
    HDEV hdev
    );

ENGAPI
LPWSTR APIENTRY EngGetDriverName(
    HDEV hdev
    );

typedef struct _TYPE1_FONT
{
    HANDLE  hPFM;
    HANDLE  hPFB;
    ULONG   ulIdentifier;
} TYPE1_FONT;

ENGAPI
BOOL APIENTRY EngGetType1FontList(
    HDEV            hdev,
    TYPE1_FONT      *pType1Buffer,
    ULONG           cjType1Buffer,
    PULONG          pulLocalFonts,
    PULONG          pulRemoteFonts,
    LARGE_INTEGER   *pLastModified
    );

//
// Manipulating resource sections
//

ENGAPI
HANDLE APIENTRY EngLoadModule(
    LPWSTR pwsz
    );

ENGAPI
PVOID APIENTRY EngFindResource(
    HANDLE h,
    int    iName,
    int    iType,
    PULONG pulSize
    );

ENGAPI
VOID APIENTRY EngFreeModule(
    HANDLE h
    );

ENGAPI
BOOL
APIENTRY
EngDeleteFile (
    LPWSTR  pwszFileName
    );

ENGAPI
PVOID
APIENTRY
EngMapFile(
    LPWSTR      pwsz,
    ULONG       cjSize,
    ULONG_PTR  *piFile
    );

ENGAPI
BOOL
APIENTRY
EngUnmapFile (
     ULONG_PTR iFile
     );

//
// FontFile Callbacks
//

ENGAPI
VOID APIENTRY EngUnmapFontFile(
    ULONG_PTR iFile
    );

ENGAPI
BOOL APIENTRY EngMapFontFile(
    ULONG_PTR  iFile,
    PULONG *ppjBuf,
    ULONG  *pcjBuf
    );

ENGAPI
VOID APIENTRY EngUnmapFontFileFD(
    ULONG_PTR iFile
    );

ENGAPI
BOOL APIENTRY EngMapFontFileFD(
    ULONG_PTR  iFile,
    PULONG *ppjBuf,
    ULONG  *pcjBuf
    );

ENGAPI
BOOL APIENTRY EngLpkInstalled();

//
// Semaphores
//

DECLARE_HANDLE(HSEMAPHORE);

ENGAPI
HSEMAPHORE APIENTRY EngCreateSemaphore(
    VOID
    );

ENGAPI
VOID APIENTRY EngAcquireSemaphore(
    HSEMAPHORE hsem
    );

ENGAPI
VOID APIENTRY EngReleaseSemaphore(
    HSEMAPHORE hsem
    );

ENGAPI
VOID APIENTRY EngDeleteSemaphore(
    HSEMAPHORE hsem
    );

ENGAPI
BOOL APIENTRY EngIsSemaphoreOwned(
    HSEMAPHORE hsem
    );

ENGAPI
BOOL APIENTRY EngIsSemaphoreOwnedByCurrentThread(
    HSEMAPHORE hsem
    );

//
// Semaphore wrapper with a reference count for thread-safe creation and
// destruction.  Data must be created zero-filled.
//
// Use EngAcquire/ReleaseSemaphore on hsem for access protection.
//

typedef struct _ENGSAFESEMAPHORE
{
    HSEMAPHORE hsem;
    LONG lCount;
} ENGSAFESEMAPHORE;

ENGAPI
BOOL APIENTRY EngInitializeSafeSemaphore(
    ENGSAFESEMAPHORE *pssem
    );

ENGAPI
VOID APIENTRY EngDeleteSafeSemaphore(
    ENGSAFESEMAPHORE *pssem
    );

ENGAPI
VOID APIENTRY EngMultiByteToUnicodeN(
    LPWSTR UnicodeString,
    ULONG MaxBytesInUnicodeString,
    PULONG BytesInUnicodeString,
    PCHAR MultiByteString,
    ULONG BytesInMultiByteString
    );

ENGAPI
VOID APIENTRY EngUnicodeToMultiByteN(
    PCHAR MultiByteString,
    ULONG MaxBytesInMultiByteString,
    PULONG BytesInMultiByteString,
    PWSTR UnicodeString,
    ULONG BytesInUnicodeString
    );

// for the spooler

#ifdef USERMODE_DRIVER

#define EngWritePrinter     WritePrinter
#define EngGetPrinter       GetPrinter
#define EngGetPrinterDriver GetPrinterDriver
#define EngGetPrinterData   GetPrinterData
#define EngSetPrinterData   SetPrinterData
#define EngEnumForms        EnumForms
#define EngGetForm          GetForm

#else // !USERMODE_DRIVER

ENGAPI
BOOL APIENTRY EngGetPrinterDriver(
    HANDLE  hPrinter,
    LPWSTR  pEnvironment,
    DWORD   dwLevel,
    BYTE   *lpbDrvInfo,
    DWORD   cbBuf,
    DWORD  *pcbNeeded
    );

ENGAPI
DWORD APIENTRY EngGetPrinterData(
    HANDLE   hPrinter,
    LPWSTR    pValueName,
    LPDWORD  pType,
    LPBYTE   pData,
    DWORD    nSize,
    LPDWORD  pcbNeeded
    );

ENGAPI
DWORD APIENTRY EngSetPrinterData(
    HANDLE   hPrinter,
    LPWSTR   pType,
    DWORD    dwType,
    LPBYTE   lpbPrinterData,
    DWORD    cjPrinterData
    );

ENGAPI
BOOL APIENTRY EngGetForm(
    HANDLE  hPrinter,
    LPWSTR   pFormName,
    DWORD   Level,
    LPBYTE  pForm,
    DWORD   cbBuf,
    LPDWORD pcbNeeded
    );

ENGAPI
BOOL APIENTRY EngWritePrinter(
    HANDLE  hPrinter,
    LPVOID  pBuf,
    DWORD   cbBuf,
    LPDWORD pcWritten
    );

ENGAPI
BOOL APIENTRY EngGetPrinter(
    HANDLE  hPrinter,
    DWORD   dwLevel,
    LPBYTE  pPrinter,
    DWORD   cbBuf,
    LPDWORD pcbNeeded
    );

ENGAPI
BOOL APIENTRY EngEnumForms(
    HANDLE  hPrinter,
    DWORD   Level,
    LPBYTE  pForm,
    DWORD   cbBuf,
    LPDWORD pcbNeeded,
    LPDWORD pcReturned
    );

#endif // !USERMODE_DRIVER


#if defined(_X86_) && !defined(USERMODE_DRIVER)

    typedef struct _FLOATOBJ
    {
        ULONG ul1;
        ULONG ul2;
    } FLOATOBJ, *PFLOATOBJ;

    VOID APIENTRY FLOATOBJ_SetFloat(PFLOATOBJ,FLOATL);
    VOID APIENTRY FLOATOBJ_SetLong(PFLOATOBJ,LONG);

    LONG APIENTRY FLOATOBJ_GetFloat(PFLOATOBJ);
    LONG APIENTRY FLOATOBJ_GetLong(PFLOATOBJ);

    VOID APIENTRY FLOATOBJ_AddFloat(PFLOATOBJ,FLOATL);
    VOID APIENTRY FLOATOBJ_AddLong(PFLOATOBJ,LONG);
    VOID APIENTRY FLOATOBJ_Add(PFLOATOBJ,PFLOATOBJ);

    VOID APIENTRY FLOATOBJ_SubFloat(PFLOATOBJ,FLOATL);
    VOID APIENTRY FLOATOBJ_SubLong(PFLOATOBJ,LONG);
    VOID APIENTRY FLOATOBJ_Sub(PFLOATOBJ,PFLOATOBJ);

    VOID APIENTRY FLOATOBJ_MulFloat(PFLOATOBJ,FLOATL);
    VOID APIENTRY FLOATOBJ_MulLong(PFLOATOBJ,LONG);
    VOID APIENTRY FLOATOBJ_Mul(PFLOATOBJ,PFLOATOBJ);

    VOID APIENTRY FLOATOBJ_DivFloat(PFLOATOBJ,FLOATL);
    VOID APIENTRY FLOATOBJ_DivLong(PFLOATOBJ,LONG);
    VOID APIENTRY FLOATOBJ_Div(PFLOATOBJ,PFLOATOBJ);

    VOID APIENTRY FLOATOBJ_Neg(PFLOATOBJ);

    BOOL APIENTRY FLOATOBJ_EqualLong(PFLOATOBJ,LONG);
    BOOL APIENTRY FLOATOBJ_GreaterThanLong(PFLOATOBJ,LONG);
    BOOL APIENTRY FLOATOBJ_LessThanLong(PFLOATOBJ,LONG);

    BOOL APIENTRY FLOATOBJ_Equal(PFLOATOBJ,PFLOATOBJ);
    BOOL APIENTRY FLOATOBJ_GreaterThan(PFLOATOBJ,PFLOATOBJ);
    BOOL APIENTRY FLOATOBJ_LessThan(PFLOATOBJ,PFLOATOBJ);

#else

    // any platform that has support for floats in the kernel

    typedef FLOAT FLOATOBJ;
    typedef FLOAT *PFLOATOBJ;

    #define   FLOATOBJ_SetFloat(pf,f)       {*(pf) = (f);           }
    #define   FLOATOBJ_SetLong(pf,l)        {*(pf) = (FLOAT)(l);    }

    #define   FLOATOBJ_GetFloat(pf)         *((PULONG)pf)
    #define   FLOATOBJ_GetLong(pf)          (LONG)*(pf)

    #define   FLOATOBJ_AddFloat(pf,f)       {*(pf) += f;            }
    #define   FLOATOBJ_AddLong(pf,l)        {*(pf) += (LONG)(l);    }
    #define   FLOATOBJ_Add(pf,pf1)          {*(pf) += *(pf1);       }

    #define   FLOATOBJ_SubFloat(pf,f)       {*(pf) -= f;            }
    #define   FLOATOBJ_SubLong(pf,l)        {*(pf) -= (LONG)(l);    }
    #define   FLOATOBJ_Sub(pf,pf1)          {*(pf) -= *(pf1);       }

    #define   FLOATOBJ_MulFloat(pf,f)       {*(pf) *= f;            }
    #define   FLOATOBJ_MulLong(pf,l)        {*(pf) *= (LONG)(l);    }
    #define   FLOATOBJ_Mul(pf,pf1)          {*(pf) *= *(pf1);       }

    #define   FLOATOBJ_DivFloat(pf,f)       {*(pf) /= f;            }
    #define   FLOATOBJ_DivLong(pf,l)        {*(pf) /= (LONG)(l);    }
    #define   FLOATOBJ_Div(pf,pf1)          {*(pf) /= *(pf1);       }

    #define   FLOATOBJ_Neg(pf)              {*(pf) = -*(pf);        }

    #define   FLOATOBJ_EqualLong(pf,l)          (*(pf) == (FLOAT)(l))
    #define   FLOATOBJ_GreaterThanLong(pf,l)    (*(pf) >  (FLOAT)(l))
    #define   FLOATOBJ_LessThanLong(pf,l)       (*(pf) <  (FLOAT)(l))

    #define   FLOATOBJ_Equal(pf,pf1)            (*(pf) == *(pf1))
    #define   FLOATOBJ_GreaterThan(pf,pf1)      (*(pf) >  *(pf1))
    #define   FLOATOBJ_LessThan(pf,pf1)         (*(pf) <  *(pf1))

#endif // _FLOATOBJ_

#if defined(USERMODE_DRIVER)

typedef XFORML FLOATOBJ_XFORM ;
typedef XFORML *PFLOATOBJ_XFORM;
typedef XFORML FAR *LPFLOATOBJ_XFORM;

#define XFORMOBJ_iGetFloatObjXform XFORMOBJ_iGetXform

#else

typedef struct  tagFLOATOBJ_XFORM
{
    FLOATOBJ eM11;
    FLOATOBJ eM12;
    FLOATOBJ eM21;
    FLOATOBJ eM22;
    FLOATOBJ eDx;
    FLOATOBJ eDy;
} FLOATOBJ_XFORM, *PFLOATOBJ_XFORM, FAR *LPFLOATOBJ_XFORM;

ULONG APIENTRY XFORMOBJ_iGetFloatObjXform(
    XFORMOBJ *pxo,
    FLOATOBJ_XFORM * pfxo
    );

#endif


// SORT specific defines

typedef int (__cdecl *SORTCOMP)(const void *pv1, const void *pv2);

ENGAPI
VOID APIENTRY EngSort(
    PBYTE pjBuf,
    ULONG c,
    ULONG cjElem,
    SORTCOMP pfnComp
    );

typedef struct _ENG_TIME_FIELDS {
    USHORT usYear;        // range [1601...]
    USHORT usMonth;       // range [1..12]
    USHORT usDay;         // range [1..31]
    USHORT usHour;        // range [0..23]
    USHORT usMinute;      // range [0..59]
    USHORT usSecond;      // range [0..59]
    USHORT usMilliseconds;// range [0..999]
    USHORT usWeekday;     // range [0..6] == [Sunday..Saturday]
} ENG_TIME_FIELDS, *PENG_TIME_FIELDS;

ENGAPI
VOID APIENTRY EngQueryLocalTime(
    PENG_TIME_FIELDS
    );

ENGAPI
FD_GLYPHSET* APIENTRY EngComputeGlyphSet(
    INT nCodePage,
    INT nFirstChar,
    INT cChars
    );

ENGAPI
INT APIENTRY EngMultiByteToWideChar(
    UINT CodePage,
    LPWSTR WideCharString,
    INT BytesInWideCharString,
    LPSTR MultiByteString,
    INT BytesInMultiByteString
    );

ENGAPI
INT APIENTRY EngWideCharToMultiByte(
    UINT CodePage,
    LPWSTR WideCharString,
    INT BytesInWideCharString,
    LPSTR MultiByteString,
    INT BytesInMultiByteString
    );

ENGAPI
VOID APIENTRY EngGetCurrentCodePage(
    PUSHORT OemCodePage,
    PUSHORT AnsiCodePage
    );

ENGAPI
HANDLE APIENTRY EngLoadModuleForWrite(
    LPWSTR pwsz,
    ULONG  cjSizeOfModule
    );

ENGAPI
LARGE_INTEGER APIENTRY EngQueryFileTimeStamp (
    LPWSTR  pwsz
    );

ENGAPI
BOOL APIENTRY EngGetFileChangeTime(
    HANDLE          h,
    LARGE_INTEGER   *pChangeTime
    );

ENGAPI
BOOL APIENTRY EngGetFilePath(
    IN  HANDLE h ,
    OUT WCHAR (*pDest)[MAX_PATH+1]
    );

#if defined(_M_AMD64)

__forceinline
ULONG
EngSaveFloatingPointState (
    VOID   *pBuffer,
    ULONG   cjBufferSize
    )
{
    return ((((pBuffer) == NULL) || ((cjBufferSize) == 0)) ? 8 : TRUE);
}

__forceinline
BOOL
EngRestoreFloatingPointState (
    VOID   *pBuffer
    )

{
    return TRUE;
}

#else

ENGAPI
ULONG APIENTRY EngSaveFloatingPointState(
    VOID   *pBuffer,
    ULONG   cjBufferSize
    );

ENGAPI
BOOL APIENTRY EngRestoreFloatingPointState(
    VOID   *pBuffer
    );

#endif

//
// DirectDraw surface locking
//

ENGAPI
PDD_SURFACE_LOCAL APIENTRY EngLockDirectDrawSurface(
        HANDLE hSurface
        );

ENGAPI
BOOL APIENTRY EngUnlockDirectDrawSurface(
        PDD_SURFACE_LOCAL pSurface
        );

//
//  Engine Event support.
//

//
//  Opaque type for event objects.
//

typedef struct _ENG_EVENT *PEVENT;

ENGAPI
BOOL APIENTRY EngDeleteEvent(
    IN  PEVENT  pEvent
    );

ENGAPI
BOOL APIENTRY EngCreateEvent(
    OUT PEVENT *ppEvent
    );

ENGAPI
BOOL APIENTRY EngUnmapEvent(
    IN  PEVENT pEvent
    );

ENGAPI
PEVENT APIENTRY EngMapEvent(
    IN  HDEV            hDev,
    IN  HANDLE          hUserObject,
    IN  PVOID           Reserved1,
    IN  PVOID           Reserved2,
    IN  PVOID           Reserved3
    );

ENGAPI
BOOL APIENTRY EngWaitForSingleObject(
    IN  PEVENT          pEvent,
    IN  PLARGE_INTEGER  pTimeOut
    );

ENGAPI
LONG APIENTRY EngSetEvent(
    IN PEVENT pEvent
    );

ENGAPI
VOID APIENTRY
EngClearEvent (
IN PEVENT pEvent
);

ENGAPI
LONG APIENTRY
EngReadStateEvent (
IN PEVENT pEvent
);


//
// Querying of system attributes.
//

typedef enum _ENG_SYSTEM_ATTRIBUTE {
    EngProcessorFeature = 1,
    EngNumberOfProcessors,
    EngOptimumAvailableUserMemory,
    EngOptimumAvailableSystemMemory,
} ENG_SYSTEM_ATTRIBUTE;

#define QSA_MMX   0x00000100  // MMX
#define QSA_SSE   0x00002000  // SIMD
#define QSA_3DNOW 0x00004000  // 3DNow

ENGAPI
BOOL APIENTRY
EngQuerySystemAttribute(
    ENG_SYSTEM_ATTRIBUTE CapNum,
    PDWORD pCapability);

#define ENG_FNT_CACHE_READ_FAULT    0x1
#define ENG_FNT_CACHE_WRITE_FAULT   0x2

ENGAPI
PVOID APIENTRY  EngFntCacheLookUp(ULONG FastCheckSum, ULONG * pulSize);

ENGAPI
PVOID APIENTRY  EngFntCacheAlloc(ULONG FastCheckSum, ULONG ulSize);

ENGAPI
VOID  APIENTRY  EngFntCacheFault(ULONG ulFastCheckSum, ULONG iFaultMode);

typedef enum _ENG_DEVICE_ATTRIBUTE {
    QDA_RESERVED = 0,
    QDA_ACCELERATION_LEVEL = 1
} ENG_DEVICE_ATTRIBUTE;

ENGAPI
BOOL APIENTRY
EngQueryDeviceAttribute(
    HDEV                    hdev,
    ENG_DEVICE_ATTRIBUTE    devAttr,
    VOID *                  pvIn,
    ULONG                   ulInSize,
    VOID *                  pvOUt,
    ULONG                   ulOutSize);

typedef struct
{
   DWORD nSize;
   HDC   hdc;
   PBYTE pvEMF;
   PBYTE pvCurrentRecord;
} EMFINFO, *PEMFINFO;

DECLSPEC_DEPRECATED_DDK
BOOL APIENTRY
EngQueryEMFInfo(
    HDEV              hdev,
    EMFINFO           *pEMFInfo);

//
// EngProcessorFeature
//

//
// EngNumberOfProcessors
//
// Number of active processors in the machine.
//

//
// EngOptimumAvailableUserMemory
//
// Optimum amount of user-mode memory available to avoid paging.
// Returns the number of bytes for the optimum allocation size
// Memory is allocated via EngAllocUserMem.
//

//
// EngOptimumAvailableSystemMemory
//
// Optimum amount of system memory available to avoid paging.
// Returns the number of bytes for the optimum allocation size.
// Memory is allocated via EngAllocMem.
//

//
// DDI entrypoint function prototypes
//

typedef BOOL   (APIENTRY *PFN_DrvEnableDriver)(ULONG,ULONG,PDRVENABLEDATA);
typedef DHPDEV (APIENTRY *PFN_DrvEnablePDEV) (PDEVMODEW,LPWSTR,ULONG,HSURF*,ULONG,GDIINFO*,ULONG,PDEVINFO,HDEV,LPWSTR,HANDLE);
typedef VOID   (APIENTRY *PFN_DrvCompletePDEV)(DHPDEV,HDEV);
typedef ULONG  (APIENTRY *PFN_DrvResetDevice)(DHPDEV,PVOID);
typedef VOID   (APIENTRY *PFN_DrvDisablePDEV)(DHPDEV);
typedef VOID   (APIENTRY *PFN_DrvSynchronize)(DHPDEV,RECTL *);
typedef HSURF  (APIENTRY *PFN_DrvEnableSurface)(DHPDEV);
typedef VOID   (APIENTRY *PFN_DrvDisableDriver)(VOID);
typedef VOID   (APIENTRY *PFN_DrvDisableSurface)(DHPDEV);
typedef BOOL   (APIENTRY *PFN_DrvAssertMode)(DHPDEV, BOOL);
typedef BOOL   (APIENTRY *PFN_DrvTextOut)(SURFOBJ *,STROBJ *,FONTOBJ *,CLIPOBJ *,RECTL *,RECTL *,BRUSHOBJ *,BRUSHOBJ *,POINTL *,MIX);
typedef BOOL   (APIENTRY *PFN_DrvStretchBlt)(SURFOBJ *,SURFOBJ *,SURFOBJ *,CLIPOBJ *,XLATEOBJ *,COLORADJUSTMENT *,POINTL *,RECTL *,RECTL *,POINTL *,ULONG);
typedef BOOL   (APIENTRY *PFN_DrvStretchBltROP)(SURFOBJ *,SURFOBJ *,SURFOBJ *,CLIPOBJ *,XLATEOBJ *,COLORADJUSTMENT *,POINTL *,RECTL *,RECTL *,POINTL *,ULONG, BRUSHOBJ *,ROP4);
typedef BOOL   (APIENTRY *PFN_DrvTransparentBlt)(SURFOBJ *, SURFOBJ *, CLIPOBJ *, XLATEOBJ *, RECTL *, RECTL *, ULONG, ULONG);
typedef BOOL   (APIENTRY *PFN_DrvPlgBlt)(SURFOBJ *,SURFOBJ *,SURFOBJ *,CLIPOBJ *,XLATEOBJ *,COLORADJUSTMENT *,POINTL *,POINTFIX *,RECTL *,POINTL *,ULONG);
typedef BOOL   (APIENTRY *PFN_DrvBitBlt)(SURFOBJ *,SURFOBJ *,SURFOBJ *,CLIPOBJ *,XLATEOBJ *,RECTL *,POINTL *,POINTL *,BRUSHOBJ *,POINTL *,ROP4);
typedef BOOL   (APIENTRY *PFN_DrvRealizeBrush)(BRUSHOBJ *,SURFOBJ *,SURFOBJ *,SURFOBJ *,XLATEOBJ *,ULONG);
typedef BOOL   (APIENTRY *PFN_DrvCopyBits)(SURFOBJ *,SURFOBJ *,CLIPOBJ *,XLATEOBJ *,RECTL *,POINTL *);
typedef ULONG  (APIENTRY *PFN_DrvDitherColor)(DHPDEV, ULONG, ULONG, ULONG *);
typedef HBITMAP (APIENTRY *PFN_DrvCreateDeviceBitmap)(DHPDEV, SIZEL, ULONG);
typedef VOID   (APIENTRY *PFN_DrvDeleteDeviceBitmap)(DHSURF);
typedef BOOL   (APIENTRY *PFN_DrvSetPalette)(DHPDEV, PALOBJ *, FLONG, ULONG, ULONG);
typedef ULONG  (APIENTRY *PFN_DrvEscape)(SURFOBJ *, ULONG, ULONG, PVOID, ULONG, PVOID);
typedef ULONG  (APIENTRY *PFN_DrvDrawEscape)(SURFOBJ *, ULONG, CLIPOBJ *, RECTL *, ULONG, PVOID);
typedef PIFIMETRICS (APIENTRY *PFN_DrvQueryFont)(DHPDEV, ULONG_PTR, ULONG, ULONG_PTR *);
typedef PVOID  (APIENTRY *PFN_DrvQueryFontTree)(DHPDEV, ULONG_PTR, ULONG, ULONG, ULONG_PTR *);
typedef LONG   (APIENTRY *PFN_DrvQueryFontData)(DHPDEV, FONTOBJ *, ULONG, HGLYPH, GLYPHDATA *, PVOID, ULONG);
typedef VOID   (APIENTRY *PFN_DrvFree)(PVOID, ULONG_PTR);
typedef VOID   (APIENTRY *PFN_DrvDestroyFont)(FONTOBJ *);
typedef LONG   (APIENTRY *PFN_DrvQueryFontCaps)(ULONG, ULONG *);
typedef HFF    (APIENTRY *PFN_DrvLoadFontFile)(ULONG, ULONG_PTR *, PVOID *, ULONG *, DESIGNVECTOR *, ULONG, ULONG);
typedef BOOL   (APIENTRY *PFN_DrvUnloadFontFile)(ULONG_PTR);
typedef ULONG  (APIENTRY *PFN_DrvSetPointerShape)(SURFOBJ *, SURFOBJ *, SURFOBJ *,XLATEOBJ *,LONG,LONG,LONG,LONG,RECTL *,FLONG);
typedef VOID   (APIENTRY *PFN_DrvMovePointer)(SURFOBJ *pso,LONG x,LONG y,RECTL *prcl);
typedef BOOL   (APIENTRY *PFN_DrvSendPage)(SURFOBJ *);
typedef BOOL   (APIENTRY *PFN_DrvStartPage)(SURFOBJ *pso);
typedef BOOL   (APIENTRY *PFN_DrvStartDoc)(SURFOBJ *pso, LPWSTR pwszDocName, DWORD dwJobId);
typedef BOOL   (APIENTRY *PFN_DrvEndDoc)(SURFOBJ *pso, FLONG fl);
typedef BOOL   (APIENTRY *PFN_DrvQuerySpoolType)(DHPDEV dhpdev, LPWSTR pwchType);
typedef BOOL   (APIENTRY *PFN_DrvLineTo)(SURFOBJ *,CLIPOBJ *,BRUSHOBJ *,LONG,LONG,LONG,LONG,RECTL *,MIX);
typedef BOOL   (APIENTRY *PFN_DrvStrokePath)(SURFOBJ *,PATHOBJ *,CLIPOBJ *,XFORMOBJ *,BRUSHOBJ *,POINTL *,LINEATTRS *,MIX);
typedef BOOL   (APIENTRY *PFN_DrvFillPath)(SURFOBJ *,PATHOBJ *,CLIPOBJ *,BRUSHOBJ *,POINTL *,MIX,FLONG);
typedef BOOL   (APIENTRY *PFN_DrvStrokeAndFillPath)(SURFOBJ *,PATHOBJ *,CLIPOBJ *,XFORMOBJ *,BRUSHOBJ *,LINEATTRS *,BRUSHOBJ *,POINTL *,MIX,FLONG);
typedef BOOL   (APIENTRY *PFN_DrvPaint)(SURFOBJ *,CLIPOBJ *,BRUSHOBJ *,POINTL *,MIX);
typedef ULONG  (APIENTRY *PFN_DrvGetGlyphMode)(DHPDEV dhpdev,FONTOBJ *pfo);
typedef BOOL   (APIENTRY *PFN_DrvResetPDEV)(DHPDEV dhpdevOld, DHPDEV dhpdevNew);
typedef ULONG_PTR  (APIENTRY *PFN_DrvSaveScreenBits)(SURFOBJ *, ULONG, ULONG_PTR, RECTL *);
typedef ULONG  (APIENTRY *PFN_DrvGetModes)(HANDLE, ULONG, DEVMODEW *);
typedef LONG   (APIENTRY *PFN_DrvQueryTrueTypeTable)(ULONG_PTR, ULONG, ULONG, PTRDIFF, ULONG, BYTE *, PBYTE *, ULONG *);
typedef LONG   (APIENTRY *PFN_DrvQueryTrueTypeSection)(ULONG, ULONG, ULONG, HANDLE *, PTRDIFF *);
typedef LONG   (APIENTRY *PFN_DrvQueryTrueTypeOutline)(DHPDEV, FONTOBJ *, HGLYPH, BOOL, GLYPHDATA *, ULONG, TTPOLYGONHEADER *);
typedef PVOID  (APIENTRY *PFN_DrvGetTrueTypeFile)(ULONG_PTR, ULONG *);
typedef LONG   (APIENTRY *PFN_DrvQueryFontFile)(ULONG_PTR, ULONG, ULONG, ULONG *);
typedef PFD_GLYPHATTR   (APIENTRY *PFN_DrvQueryGlyphAttrs)(FONTOBJ *, ULONG );
typedef BOOL   (APIENTRY *PFN_DrvQueryAdvanceWidths)(DHPDEV,FONTOBJ *,ULONG,HGLYPH *,PVOID,ULONG);
typedef ULONG  (APIENTRY *PFN_DrvFontManagement)(SURFOBJ *,FONTOBJ *,ULONG,ULONG,PVOID,ULONG,PVOID);
typedef BOOL   (APIENTRY *PFN_DrvSetPixelFormat)(SURFOBJ *,LONG,HWND);
typedef LONG   (APIENTRY *PFN_DrvDescribePixelFormat)(DHPDEV,LONG,ULONG,PIXELFORMATDESCRIPTOR *);
typedef BOOL   (APIENTRY *PFN_DrvSwapBuffers)(SURFOBJ *, WNDOBJ *);
typedef BOOL   (APIENTRY *PFN_DrvStartBanding)(SURFOBJ *, POINTL *ppointl);
typedef BOOL   (APIENTRY *PFN_DrvNextBand)(SURFOBJ *, POINTL *ppointl);
typedef BOOL   (APIENTRY *PFN_DrvQueryPerBandInfo)(SURFOBJ *,PERBANDINFO *);
typedef BOOL   (APIENTRY *PFN_DrvEnableDirectDraw)(DHPDEV, DD_CALLBACKS *,DD_SURFACECALLBACKS *, DD_PALETTECALLBACKS *);
typedef VOID   (APIENTRY *PFN_DrvDisableDirectDraw)(DHPDEV);
typedef BOOL   (APIENTRY *PFN_DrvGetDirectDrawInfo)(DHPDEV, DD_HALINFO *, DWORD *, VIDEOMEMORY *, DWORD *, DWORD *);
typedef HANDLE (APIENTRY *PFN_DrvIcmCreateColorTransform)(DHPDEV,LPLOGCOLORSPACEW,LPVOID,ULONG,LPVOID,ULONG,LPVOID,ULONG,DWORD);
typedef BOOL   (APIENTRY *PFN_DrvIcmDeleteColorTransform)(DHPDEV,HANDLE);
typedef BOOL   (APIENTRY *PFN_DrvIcmCheckBitmapBits)(DHPDEV,HANDLE,SURFOBJ *,PBYTE);
typedef BOOL   (APIENTRY *PFN_DrvIcmSetDeviceGammaRamp)(DHPDEV,ULONG,LPVOID);
typedef BOOL   (APIENTRY *PFN_DrvAlphaBlend)(SURFOBJ*,SURFOBJ*,CLIPOBJ*,XLATEOBJ*,PRECTL,PRECTL,BLENDOBJ *);
typedef BOOL   (APIENTRY *PFN_DrvGradientFill)(SURFOBJ*,CLIPOBJ*,XLATEOBJ*,TRIVERTEX*,ULONG,PVOID,ULONG,RECTL *,POINTL *,ULONG);
typedef BOOL   (APIENTRY *PFN_DrvQueryDeviceSupport)(SURFOBJ*,XLATEOBJ*,XFORMOBJ*,ULONG,ULONG,PVOID,ULONG,PVOID);
typedef HBITMAP (APIENTRY *PFN_DrvDeriveSurface)(DD_DIRECTDRAW_GLOBAL*,DD_SURFACE_LOCAL*);
typedef VOID   (APIENTRY *PFN_DrvSynchronizeSurface)(SURFOBJ*, RECTL*, FLONG);
typedef VOID   (APIENTRY *PFN_DrvNotify)(SURFOBJ*, ULONG, PVOID);

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  //  _WINDDI_

