/*++

Copyright (c) 1996-1997 Microsoft Corporation

Module Name:

    printoem.h

Abstract:

    Declarations for Windows NT printer driver OEM plugins

--*/


#ifndef _PRINTOEM_
#define _PRINTOEM_

#ifdef __cplusplus
extern "C" {
#endif

//
// Current OEM plugin interface version number
//

#define PRINTER_OEMINTF_VERSION 0x00010000

#define OEM_MODE_PUBLISHER      0x00000001

typedef struct _PUBLISHERINFO {

    DWORD dwMode;           // flags for publisher
    WORD  wMinoutlinePPEM;  // min size to download as Type1
    WORD  wMaxbitmapPPEM;   // max size to download as Type3

} PUBLISHERINFO, *PPUBLISHERINFO;

#define OEMGI_GETSIGNATURE        1
#define OEMGI_GETINTERFACEVERSION 2
#define OEMGI_GETVERSION          3
#define OEMGI_GETPUBLISHERINFO    4

//
// OEMGetInfo
//
BOOL APIENTRY
OEMGetInfo(
    IN  DWORD  dwMode,
    OUT PVOID  pBuffer,
    IN  DWORD  cbSize,
    OUT PDWORD pcbNeeded
    );

BOOL APIENTRY
OEMDriverDMS(
    PVOID    pDevObj,
    PVOID    pBuffer,
    DWORD    cbSize,
    PDWORD   pcbNeeded
    );

//
// OEMDevMode
//

#define OEMDM_SIZE     1
#define OEMDM_DEFAULT  2
#define OEMDM_CONVERT  3
#define OEMDM_MERGE    4

typedef struct _OEMDMPARAM {

    DWORD    cbSize;        // size of OEM_DEVMODEPARAM
    PVOID    pdriverobj;    // reference to driver data structure
    HANDLE   hPrinter;      // PRINTER handle
    HANDLE   hModule;       // OEM module handle
    PDEVMODE pPublicDMIn;   // public devmode in
    PDEVMODE pPublicDMOut;  // public devmode out
    PVOID    pOEMDMIn;      // OEM private devmode in
    PVOID    pOEMDMOut;     // OEM private devmode out
    DWORD    cbBufSize;     // output size of pOEMDMOut buffer

} OEMDMPARAM, *POEMDMPARAM;

typedef struct _OEM_DMEXTRAHEADER {

    DWORD   dwSize;         // size of OEM extra data
    DWORD   dwSignature;    // Unique OEM signature
    DWORD   dwVersion;      // OEM DLL version number

} OEM_DMEXTRAHEADER, *POEM_DMEXTRAHEADER;

//
// USERDATA for OPTITEM.UserData
//

typedef struct _USERDATA {

    DWORD       dwSize;                 // Size of this structure
    ULONG_PTR    dwItemID;               // XXX_ITEM or pointer to FEATURE
    PSTR        pKeyWordName;           // Keyword name
    DWORD       dwReserved[8];
} USERDATA, *PUSERDATA;


BOOL APIENTRY
OEMDevMode(
    DWORD       dwMode,
    POEMDMPARAM pOemDMParam
    );

//
// Callback function provided by the driver to
// allow OEM plugins access/set/update to driver private settings
//

typedef BOOL (APIENTRY *PFN_DrvGetDriverSetting)(
        PVOID   pdriverobj,
        PCSTR   Feature,
        PVOID   pOutput,
        DWORD   cbSize,
        PDWORD  pcbNeeded,
        PDWORD  pdwOptionsReturned
        );

//
// Callback function provided by the driver to
// allow OEM plugins upgrade private registry settings.
//

typedef BOOL (APIENTRY *PFN_DrvUpgradeRegistrySetting)(
        HANDLE   hPrinter,
        PCSTR    pFeature,
        PCSTR    pOption
        );

//
// Callback function provided by the driver UI to
// allow OEM plugins to update the driver UI settings and
// shows constraint. This function is called only when the UI is present.
//

typedef BOOL (APIENTRY *PFN_DrvUpdateUISetting)(
        PVOID    pdriverobj,
        PVOID    pOptItem,
        DWORD    dwPreviousSelection,
        DWORD    dwMode
        );



// Predefined feature indices used for accessing driver private settings

#define OEMGDS_MIN_DOCSTICKY        1
#define OEMGDS_PSDM_FLAGS           1       // DWORD - misc. flag bits
#define OEMGDS_PSDM_DIALECT         2       // INT - PS output option
#define OEMGDS_PSDM_TTDLFMT         3       // INT - TrueType font downloading option
#define OEMGDS_PSDM_NUP             4       // INT - N-up option
#define OEMGDS_PSDM_PSLEVEL         5       // INT - target language level
#define OEMGDS_PSDM_CUSTOMSIZE      6       // 5*DWORD - custom page size parameters

#define OEMGDS_UNIDM_GPDVER         0x4000  // WORD - GPD Version
#define OEMGDS_UNIDM_FLAGS          0x4001  // DWORD - misc flag bits

// Indices for private devmode fields - start at 0x4000

#define OEMGDS_MIN_PRINTERSTICKY    0x8000
#define OEMGDS_PRINTFLAGS           0x8000  // DWORD - misc. flag bits
#define OEMGDS_FREEMEM              0x8001  // DWORD - amount of VM, ps only
#define OEMGDS_JOBTIMEOUT           0x8002  // DWORD - job timeout, ps only
#define OEMGDS_WAITTIMEOUT          0x8003  // DWORD - wait timeout, ps only
#define OEMGDS_PROTOCOL             0x8004  // WORD - output protocol, ps only
#define OEMGDS_MINOUTLINE           0x8005  // WORD - min outline font size, ps only
#define OEMGDS_MAXBITMAP            0x8006  // WORD - max bitmap font size, ps only

#define OEMGDS_MAX                  0x10000


// dwType  flags for use with     STDMETHOD (DrvGetGPDData)
#define GPD_OEMCUSTOMDATA           1


/*******************************************************************************
 *
 * Definitions used by kernel-mode rendering module only:
 *  Make sure the macro KERNEL_MODE is defined and
 *  the header file winddi.h is included before this file.
 */

#ifdef KERNEL_MODE

//
// OEMEnableDriver
//

BOOL APIENTRY
OEMEnableDriver(
    DWORD           dwOemIntfVersion,
    DWORD           cbSize,
    PDRVENABLEDATA  pded
    );

typedef struct _DEVOBJ *PDEVOBJ;
typedef PVOID PDEVOEM;

typedef DWORD (APIENTRY *PFN_DrvWriteSpoolBuf)(
    PDEVOBJ pdevobj,
    PVOID   pBuffer,
    DWORD   cbSize
    );

typedef DWORD (APIENTRY *PFN_DrvWriteAbortBuf)(
    PDEVOBJ pdevobj,
    PVOID   pBuffer,
    DWORD   cbSize,
    DWORD       dwWait

    );

typedef INT (APIENTRY *PFN_DrvXMoveTo)(
    PDEVOBJ pdevobj,
    INT     x,
    DWORD   dwFlags
    );

typedef INT (APIENTRY *PFN_DrvYMoveTo)(
    PDEVOBJ pdevobj,
    INT     y,
    DWORD   dwFlags
    );

typedef BOOL (APIENTRY *PFN_DrvGetStandardVariable)(
    PDEVOBJ pdevobj,
    DWORD   dwIndex,
    PVOID   pBuffer,
    DWORD   cbSize,
    PDWORD  pcbNeeded
    );

typedef enum _STDVARIABLEINDEX{

        SVI_NUMDATABYTES,          // "NumOfDataBytes"
        SVI_WIDTHINBYTES,          // "RasterDataWidthInBytes"
        SVI_HEIGHTINPIXELS,        // "RasterDataHeightInPixels"
        SVI_COPIES,                // "NumOfCopies"
        SVI_PRINTDIRECTION,        // "PrintDirInCCDegrees"
        SVI_DESTX,                 // "DestX"
        SVI_DESTY,                 // "DestY"
        SVI_DESTXREL,              // "DestXRel"
        SVI_DESTYREL,              // "DestYRel"
        SVI_LINEFEEDSPACING,       // "LinefeedSpacing"
        SVI_RECTXSIZE,             // "RectXSize"
        SVI_RECTYSIZE,             // "RectYSize"
        SVI_GRAYPERCENT,           // "GrayPercentage"
        SVI_NEXTFONTID,            // "NextFontID"
        SVI_NEXTGLYPH,             // "NextGlyph"
        SVI_PHYSPAPERLENGTH,       // "PhysPaperLength"
        SVI_PHYSPAPERWIDTH,        // "PhysPaperWidth"
        SVI_FONTHEIGHT,            // "FontHeight"
        SVI_FONTWIDTH,             // "FontWidth"
        SVI_FONTMAXWIDTH,             // "FontMaxWidth"
        SVI_FONTBOLD,              // "FontBold"
        SVI_FONTITALIC,            // "FontItalic"
        SVI_FONTUNDERLINE,         // "FontUnderline"
        SVI_FONTSTRIKETHRU,        // "FontStrikeThru"
        SVI_CURRENTFONTID,         // "CurrentFontID"
        SVI_TEXTYRES,              // "TextYRes"
        SVI_TEXTXRES,              // "TextXRes"
        SVI_GRAPHICSYRES,              // "GraphicsYRes"
        SVI_GRAPHICSXRES,              // "GraphicsXRes"
        SVI_ROP3,                  // "Rop3"
        SVI_REDVALUE,              // "RedValue"
        SVI_GREENVALUE,            // "GreenValue"
        SVI_BLUEVALUE,             // "BlueValue"
        SVI_PALETTEINDEXTOPROGRAM, // "PaletteIndexToProgram"
        SVI_CURRENTPALETTEINDEX,   // "CurrentPaletteIndex"
        SVI_PATTERNBRUSH_TYPE,     // "PatternBrushType"
        SVI_PATTERNBRUSH_ID,       // "PatternBrushID"
        SVI_PATTERNBRUSH_SIZE,     // "PatternBrushSize"
        SVI_CURSORORIGINX,           //  "CursorOriginX"
        SVI_CURSORORIGINY,           //  "CursorOriginY"
                //  this is in MasterUnits and in the coordinates of the currently selected orientation.
                //  this value is defined as ImageableOrigin - CursorOrigin
        SVI_PAGENUMBER,  //  "PageNumber"
                //  this value tracks number of times DrvStartBand has been called since
                //  StartDoc.

        SVI_MAX             //  Just a placeholder do not use.
}STDVARIABLEINDEX;

typedef BOOL (APIENTRY *PFN_DrvUnidriverTextOut)(
    SURFOBJ    *pso,
    STROBJ     *pstro,
    FONTOBJ    *pfo,
    CLIPOBJ    *pco,
    RECTL      *prclExtra,
    RECTL      *prclOpaque,
    BRUSHOBJ   *pboFore,
    BRUSHOBJ   *pboOpaque,
    POINTL     *pptlBrushOrg,
    MIX         mix
    );

//
// bit fields defined for dwFlags
//
// Note:  The following Bit values are reserved for an internal use!
//  0x4000
//  0x8000
//
//
#define MV_UPDATE       0x0001
#define MV_RELATIVE     0x0002
#define MV_GRAPHICS     0x0004
#define MV_PHYSICAL     0x0008
#define MV_SENDXMOVECMD     0x0010
#define MV_SENDYMOVECMD     0x0020

typedef struct _DRVPROCS {

    PFN_DrvWriteSpoolBuf    DrvWriteSpoolBuf;   // common to both pscript and unidrv
    PFN_DrvXMoveTo          DrvXMoveTo;         // unidrv specific
    PFN_DrvYMoveTo          DrvYMoveTo;         // unidrv specific
    PFN_DrvGetDriverSetting DrvGetDriverSetting;// common to both pscript and unidrv
    PFN_DrvGetStandardVariable BGetStandardVariable; // unidrv specific
    PFN_DrvUnidriverTextOut    DrvUnidriverTextOut;  // unidrv specific
    PFN_DrvWriteAbortBuf    DrvWriteAbortBuf;   // unidrv specific

} DRVPROCS, *PDRVPROCS;

typedef struct _DEVOBJ {

    DWORD       dwSize;       // size of DEVOBJ structure
    PDEVOEM     pdevOEM;      // pointer to OEM's device data
    HANDLE      hEngine;      // GDI handle for current printer
    HANDLE      hPrinter;     // spooler handle for current printer
    HANDLE      hOEM;         // handle to OEM dll
    PDEVMODE    pPublicDM;    // public devmode
    PVOID       pOEMDM;       // OEM private devmode
    PDRVPROCS   pDrvProcs;    // pointer to kernel mode helper function table

} DEVOBJ;

//
// OEMDisableDriver
//

VOID APIENTRY
OEMDisableDriver(
    VOID
    );

//
// OEMEnablePDEV
//

PDEVOEM APIENTRY
OEMEnablePDEV(
    PDEVOBJ         pdevobj,
    PWSTR           pPrinterName,
    ULONG           cPatterns,
    HSURF          *phsurfPatterns,
    ULONG           cjGdiInfo,
    GDIINFO        *pGdiInfo,
    ULONG           cjDevInfo,
    DEVINFO        *pDevInfo,
    DRVENABLEDATA  *pded
    );

//
// OEMDisablePDEV
//

VOID APIENTRY
OEMDisablePDEV(
    PDEVOBJ pdevobj
    );

//
// OEMResetPDEV
//

BOOL APIENTRY
OEMResetPDEV(
    PDEVOBJ pdevobjOld,
    PDEVOBJ pdevobjNew
    );

//
// OEMCommand - PSCRIPT only
//

DWORD APIENTRY
OEMCommand(
    PDEVOBJ pdevobj,
    DWORD   dwIndex,
    PVOID   pData,
    DWORD   cbSize
    );

INT APIENTRY
OEMCommandCallback(
    PDEVOBJ         pdevobj,
    DWORD           dwCallbackID,
    DWORD           dwCount,
    PDWORD          pdwParams
    );

//
// OEMImageProcessing - UNIDRV only
//

typedef struct {
    DWORD dwSize;
    POINT ptOffset;
    PSTR  pHalftoneOption;
    BOOL  bBanding;
    BOOL  bBlankBand;
} IPPARAMS, *PIPPARAMS;

PBYTE APIENTRY
OEMImageProcessing(
    PDEVOBJ     pdevobj,
    PBYTE       pSrcBitmap,
    PBITMAPINFOHEADER pBitmapInfoHeader,
    PBYTE       pColorTable,
    DWORD       dwCallbackID,
    PIPPARAMS   pIPParams
    );

//
// OEMFilterGraphics - UNIDRV only
//

BOOL APIENTRY
OEMFilterGraphics(
    PDEVOBJ pdevobj,
    PBYTE   pBuf,
    DWORD   dwLen
    );

//
// OEMCompression - UNIDRV only
//
INT APIENTRY
OEMCompression(
    PDEVOBJ pdevobj,
    PBYTE   pInBuf,
    PBYTE   pOutBuf,
    DWORD   dwInLen,
    DWORD   dwOutLen
    );

//
// OEMHalftone - UNIDRV only
//

BOOL APIENTRY
OEMHalftonePattern(
    PDEVOBJ pdevobj,
    PBYTE   pHTPattern,
    DWORD   dwHTPatternX,
    DWORD   dwHTPatternY,
    DWORD   dwHTNumPatterns,
    DWORD   dwCallbackID,
    PBYTE   pResource,
    DWORD   dwResourceSize
    );

//
// OEMMemoryUsage - UNIDRV only
//

typedef struct {
    DWORD   dwFixedMemoryUsage;
    DWORD   dwPercentMemoryUsage;
    DWORD   dwMaxBandSize;
} OEMMEMORYUSAGE, *POEMMEMORYUSAGE;

VOID APIENTRY
OEMMemoryUsage(
    PDEVOBJ pdevobj,
    POEMMEMORYUSAGE pMemoryUsage
    );

//
// OEMTTYGetInfo - UNIDRV only
//
INT APIENTRY
OEMTTYGetInfo(
    PDEVOBJ pdevobj,
    DWORD   dwInfoIndex,
    PVOID   pOutputBuf,
    DWORD   dwSize,
    DWORD   *pcbcNeeded
    );

#define OEMTTY_INFO_MARGINS     1
#define OEMTTY_INFO_CODEPAGE    2
#define OEMTTY_INFO_NUM_UFMS    3
#define OEMTTY_INFO_UFM_IDS     4

//
// UNIDRV font callback
//

typedef BOOL (*PFNGETINFO)(struct _UNIFONTOBJ*, DWORD, PVOID, DWORD, PDWORD);

typedef struct _UNIFONTOBJ {
    ULONG       ulFontID;
    DWORD       dwFlags;     // General flags
    IFIMETRICS *pIFIMetrics; // Pointer to IFIMETRICS
    PFNGETINFO  pfnGetInfo;  // Pointer to UNIFONTOBJ_GetInfo callback
} UNIFONTOBJ, *PUNIFONTOBJ;

//
// UNIFONTOBJ.dwFlags
//

#define UFOFLAG_TTFONT               0x00000001
#define UFOFLAG_TTDOWNLOAD_BITMAP    0x00000002
#define UFOFLAG_TTDOWNLOAD_TTOUTLINE 0x00000004
#define UFOFLAG_TTOUTLINE_BOLD_SIM   0x00000008
#define UFOFLAG_TTOUTLINE_ITALIC_SIM 0x00000010
#define UFOFLAG_TTOUTLINE_VERTICAL   0x00000020
#define UFOFLAG_TTSUBSTITUTED        0x00000040

//
// UNIFONTOBJ callback ID
//

#define UFO_GETINFO_FONTOBJ     1
#define UFO_GETINFO_GLYPHSTRING 2
#define UFO_GETINFO_GLYPHBITMAP 3
#define UFO_GETINFO_GLYPHWIDTH  4
#define UFO_GETINFO_MEMORY      5
#define UFO_GETINFO_STDVARIABLE 6

//
// UFO_GETINFO_FONTOBJ callback structure
//

typedef struct _GETINFO_FONTOBJ {
    DWORD    dwSize;   // Size of this structure
    FONTOBJ *pFontObj; // Pointer to the FONTOBJ
} GETINFO_FONTOBJ, *PGETINFO_FONTOBJ;

//
// UFO_GETINFO_FONTOBJ callback structure
//

typedef struct _GETINFO_GLYPHSTRING {
    DWORD dwSize;    // Size of this structure
    DWORD dwCount;   // Count of glyphs in pGlyphIn
    DWORD dwTypeIn;  // Glyph type of pGlyphIn, TYPE_GLYPHID/TYPE_HANDLE.
    PVOID pGlyphIn;  // Pointer to the input glyph string
    DWORD dwTypeOut; // Glyph type of pGlyphOut, TYPE_UNICODE/TYPE_TRANSDATA.
    PVOID pGlyphOut; // Pointer to the output glyph string
    DWORD dwGlyphOutSize; // The size of pGlyphOut buffer
} GETINFO_GLYPHSTRING, *PGETINFO_GLYPHSTRING;

//
// UFO_GETINFO_GLYPHBITMAP
//

typedef struct _GETINFO_GLYPHBITMAP {
    DWORD       dwSize;    // Size of this structure
    HGLYPH      hGlyph;    // Glyph hangle passed in OEMDownloadCharGlyph
    GLYPHDATA *pGlyphData; // Pointer to the GLYPHDATA data structure
} GETINFO_GLYPHBITMAP, *PGETINFO_GLYPHBITMAP;

//
// UFO_GETINFO_GLYPHWIDTH
//

typedef struct _GETINFO_GLYPHWIDTH {
    DWORD dwSize;  // Size of this structure
    DWORD dwType;  // Type of glyph stirng in pGlyph, TYPE_GLYPHHANDLE/GLYPHID.
    DWORD dwCount; // Count of glyph in pGlyph
    PVOID pGlyph;  // Pointer to a glyph string
    PLONG plWidth; // Pointer to the buffer of width table.
                   // Minidriver has to prepare this.
} GETINFO_GLYPHWIDTH, *PGETINFO_GLYPHWIDTH;

//
// UFO_GETINFO_MEMORY
//

typedef struct _GETINFO_MEMORY {
    DWORD dwSize;
    DWORD dwRemainingMemory;
} GETINFO_MEMORY, PGETINFO_MEMROY;

//
// UFO_GETINFO_STDVARIABLE
//
// OEM DLL has to prepare all StdVar buffer and set ID in dwStdVarID.
//

typedef struct _GETINFO_STDVAR {
    DWORD dwSize;
    DWORD dwNumOfVariable;
    struct {
        DWORD dwStdVarID;
        LONG  lStdVariable;
    } StdVar[1];
} GETINFO_STDVAR, *PGETINFO_STDVAR;


#define FNT_INFO_PRINTDIRINCCDEGREES  0 // PrintDirInCCDegrees
#define FNT_INFO_GRAYPERCENTAGE       1 // GrayPercentage
#define FNT_INFO_NEXTFONTID           2 // NextfontID
#define FNT_INFO_NEXTGLYPH            3 // NextGlyph
#define FNT_INFO_FONTHEIGHT           4 // FontHeight
#define FNT_INFO_FONTWIDTH            5 // FontWidth
#define FNT_INFO_FONTBOLD             6 // FontBold
#define FNT_INFO_FONTITALIC           7 // FontItalic
#define FNT_INFO_FONTUNDERLINE        8 // FontUnderline
#define FNT_INFO_FONTSTRIKETHRU       9 // FontStrikeThru
#define FNT_INFO_CURRENTFONTID       10 // Current
#define FNT_INFO_TEXTYRES            11 // TextYRes
#define FNT_INFO_TEXTXRES            12 // TextXRes
#define FNT_INFO_FONTMAXWIDTH        13 // FontMaxWidth
#define FNT_INFO_MAX                 14

//
// OEMDownloadFontheader - UNIDRV only
//

DWORD APIENTRY
OEMDownloadFontHeader(
    PDEVOBJ     pdevobj,
    PUNIFONTOBJ pUFObj
    );

//
// OEMDownloadCharGlyph - UNIDRV only
//

DWORD APIENTRY
OEMDownloadCharGlyph(
    PDEVOBJ     pdevobj,
    PUNIFONTOBJ pUFObj,
    HGLYPH      hGlyph,
    PDWORD      pdwWidth
    );

//
// OEMTTDownloadMethod - UNIDRV only
//

DWORD APIENTRY
OEMTTDownloadMethod(
    PDEVOBJ     pdevobj,
    PUNIFONTOBJ pUFObj
    );

#define TTDOWNLOAD_DONTCARE  0
#define TTDOWNLOAD_GRAPHICS  1
#define TTDOWNLOAD_BITMAP    2
#define TTDOWNLOAD_TTOUTLINE 3

//
// OEMOutputCharStr - UNIDRV only
//

VOID APIENTRY
OEMOutputCharStr(
    PDEVOBJ     pdevobj,
    PUNIFONTOBJ pUFObj,
    DWORD       dwType,
    DWORD       dwCount,
    PVOID       pGlyph
    );

#define TYPE_UNICODE      1
#define TYPE_TRANSDATA    2
#define TYPE_GLYPHHANDLE  3
#define TYPE_GLYPHID      4

//
// OEMSendFontCmd - UNIDRV only
//

typedef struct _FINVOCATION {
    DWORD dwCount;    // Size of command
    PBYTE pubCommand; // Pointer to font selection command
} FINVOCATION, *PFINVOCATION;

VOID APIENTRY
OEMSendFontCmd(
    PDEVOBJ      pdevobj,
    PUNIFONTOBJ  pUFObj,
    PFINVOCATION pFInv
    );

//
// OEMTextOutAsBitmap - UNIDRV only
//
BOOL APIENTRY
OEMTextOutAsBitmap(
    SURFOBJ    *pso,
    STROBJ     *pstro,
    FONTOBJ    *pfo,
    CLIPOBJ    *pco,
    RECTL      *prclExtra,
    RECTL      *prclOpaque,
    BRUSHOBJ   *pboFore,
    BRUSHOBJ   *pboOpaque,
    POINTL     *pptlOrg,
    MIX         mix
    );

//
// OEMBitBlt
//

BOOL APIENTRY
OEMBitBlt(
    SURFOBJ        *psoTrg,
    SURFOBJ        *psoSrc,
    SURFOBJ        *psoMask,
    CLIPOBJ        *pco,
    XLATEOBJ       *pxlo,
    RECTL          *prclTrg,
    POINTL         *pptlSrc,
    POINTL         *pptlMask,
    BRUSHOBJ       *pbo,
    POINTL         *pptlBrush,
    ROP4            rop4
    );

//
// OEMStretchBlt
//

BOOL APIENTRY
OEMStretchBlt(
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

//
// OEMCopyBits
//

BOOL APIENTRY
OEMCopyBits(
    SURFOBJ        *psoDest,
    SURFOBJ        *psoSrc,
    CLIPOBJ        *pco,
    XLATEOBJ       *pxlo,
    RECTL          *prclDest,
    POINTL         *pptlSrc
    );

//
// OEMTextOut
//

BOOL APIENTRY
OEMTextOut(
    SURFOBJ    *pso,
    STROBJ     *pstro,
    FONTOBJ    *pfo,
    CLIPOBJ    *pco,
    RECTL      *prclExtra,
    RECTL      *prclOpaque,
    BRUSHOBJ   *pboFore,
    BRUSHOBJ   *pboOpaque,
    POINTL     *pptlOrg,
    MIX         mix
    );

//
// OEMStrokePath
//

BOOL APIENTRY
OEMStrokePath(
    SURFOBJ    *pso,
    PATHOBJ    *ppo,
    CLIPOBJ    *pco,
    XFORMOBJ   *pxo,
    BRUSHOBJ   *pbo,
    POINTL     *pptlBrushOrg,
    LINEATTRS  *plineattrs,
    MIX         mix
    );

//
// OEMFillPath
//

BOOL APIENTRY
OEMFillPath(
    SURFOBJ    *pso,
    PATHOBJ    *ppo,
    CLIPOBJ    *pco,
    BRUSHOBJ   *pbo,
    POINTL     *pptlBrushOrg,
    MIX         mix,
    FLONG       flOptions
    );

//
// OEMStrokeAndFillPath
//

BOOL APIENTRY
OEMStrokeAndFillPath(
    SURFOBJ    *pso,
    PATHOBJ    *ppo,
    CLIPOBJ    *pco,
    XFORMOBJ   *pxo,
    BRUSHOBJ   *pboStroke,
    LINEATTRS  *plineattrs,
    BRUSHOBJ   *pboFill,
    POINTL     *pptlBrushOrg,
    MIX         mixFill,
    FLONG       flOptions
    );

//
// OEMRealizeBrush
//

BOOL APIENTRY
OEMRealizeBrush(
    BRUSHOBJ   *pbo,
    SURFOBJ    *psoTarget,
    SURFOBJ    *psoPattern,
    SURFOBJ    *psoMask,
    XLATEOBJ   *pxlo,
    ULONG       iHatch
    );

//
// OEMStartPage
//

BOOL APIENTRY
OEMStartPage(
    SURFOBJ    *pso
    );

//
// OEMSendPage
//

BOOL APIENTRY
OEMSendPage(
    SURFOBJ    *pso
    );

//
// OEMEscape
//

ULONG APIENTRY
OEMEscape(
    SURFOBJ    *pso,
    ULONG       iEsc,
    ULONG       cjIn,
    PVOID       pvIn,
    ULONG       cjOut,
    PVOID       pvOut
    );

//
// OEMStartDoc
//

BOOL APIENTRY
OEMStartDoc(
    SURFOBJ    *pso,
    PWSTR       pwszDocName,
    DWORD       dwJobId
    );

//
// OEMEndDoc
//

BOOL APIENTRY
OEMEndDoc(
    SURFOBJ    *pso,
    FLONG       fl
    );

//
// OEMQueryFont
//

PIFIMETRICS APIENTRY
OEMQueryFont(
    DHPDEV      dhpdev,
    ULONG_PTR   iFile,
    ULONG       iFace,
    ULONG_PTR   *pid
    );

//
// OEMQueryFontTree
//

PVOID APIENTRY
OEMQueryFontTree(
    DHPDEV      dhpdev,
    ULONG_PTR   iFile,
    ULONG       iFace,
    ULONG       iMode,
    ULONG_PTR   *pid
    );

//
// OEMQueryFontData
//

LONG APIENTRY
OEMQueryFontData(
    DHPDEV      dhpdev,
    FONTOBJ    *pfo,
    ULONG       iMode,
    HGLYPH      hg,
    GLYPHDATA  *pgd,
    PVOID       pv,
    ULONG       cjSize
    );

//
// OEMQueryAdvanceWidths
//

BOOL APIENTRY
OEMQueryAdvanceWidths(
    DHPDEV      dhpdev,
    FONTOBJ    *pfo,
    ULONG       iMode,
    HGLYPH     *phg,
    PVOID       pvWidths,
    ULONG       cGlyphs
    );

//
// OEMFontManagement
//

ULONG APIENTRY
OEMFontManagement(
    SURFOBJ    *pso,
    FONTOBJ    *pfo,
    ULONG       iMode,
    ULONG       cjIn,
    PVOID       pvIn,
    ULONG       cjOut,
    PVOID       pvOut
    );

//
// OEMGetGlyphMode
//

ULONG APIENTRY
OEMGetGlyphMode(
    DHPDEV      dhpdev,
    FONTOBJ    *pfo
    );

BOOL APIENTRY
OEMNextBand(
    SURFOBJ *pso,
    POINTL *pptl
    );

BOOL APIENTRY
OEMStartBanding(
    SURFOBJ *pso,
    POINTL *pptl
    );

ULONG APIENTRY
OEMDitherColor(
    DHPDEV  dhpdev,
    ULONG   iMode,
    ULONG   rgbColor,
    ULONG  *pulDither
    );

BOOL APIENTRY
OEMPaint(
    SURFOBJ         *pso,
    CLIPOBJ         *pco,
    BRUSHOBJ        *pbo,
    POINTL          *pptlBrushOrg,
    MIX             mix
    );

BOOL APIENTRY
OEMLineTo(
    SURFOBJ    *pso,
    CLIPOBJ    *pco,
    BRUSHOBJ   *pbo,
    LONG        x1,
    LONG        y1,
    LONG        x2,
    LONG        y2,
    RECTL      *prclBounds,
    MIX         mix
    );

#ifndef WINNT_40

//
// OEMStretchBltROP
//

BOOL APIENTRY
OEMStretchBltROP(
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
    ROP4             rop4
    );

//
// OEMPlgBlt
//

BOOL APIENTRY
OEMPlgBlt(
    SURFOBJ         *psoDst,
    SURFOBJ         *psoSrc,
    SURFOBJ         *psoMask,
    CLIPOBJ         *pco,
    XLATEOBJ        *pxlo,
    COLORADJUSTMENT *pca,
    POINTL          *pptlBrushOrg,
    POINTFIX        *pptfixDest,
    RECTL           *prclSrc,
    POINTL          *pptlMask,
    ULONG           iMode
    );

//
// OEMAlphaBlend
//

BOOL APIENTRY
OEMAlphaBlend(
    SURFOBJ    *psoDest,
    SURFOBJ    *psoSrc,
    CLIPOBJ    *pco,
    XLATEOBJ   *pxlo,
    RECTL      *prclDest,
    RECTL      *prclSrc,
    BLENDOBJ   *pBlendObj
    );

//
// OEMGradientFill
//

BOOL APIENTRY
OEMGradientFill(
    SURFOBJ    *psoDest,
    CLIPOBJ    *pco,
    XLATEOBJ   *pxlo,
    TRIVERTEX  *pVertex,
    ULONG       nVertex,
    PVOID       pMesh,
    ULONG       nMesh,
    RECTL      *prclExtents,
    POINTL     *pptlDitherOrg,
    ULONG       ulMode
    );

//
// OEMIcmCreateTransform
//

HANDLE APIENTRY
OEMIcmCreateColorTransform(
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

//
// OEMIcmDeleteTransform
//

BOOL APIENTRY
OEMIcmDeleteColorTransform(
    DHPDEV dhpdev,
    HANDLE hcmXform
    );

//
// OEMQueryDeviceSupport
//

BOOL APIENTRY
OEMQueryDeviceSupport(
    SURFOBJ    *pso,
    XLATEOBJ   *pxlo,
    XFORMOBJ   *pxo,
    ULONG      iType,
    ULONG      cjIn,
    PVOID      pvIn,
    ULONG      cjOut,
    PVOID      pvOut
    );

//
// OEMTransparentBlt
//

BOOL APIENTRY
OEMTransparentBlt(
    SURFOBJ    *psoDst,
    SURFOBJ    *psoSrc,
    CLIPOBJ    *pco,
    XLATEOBJ   *pxlo,
    RECTL      *prclDst,
    RECTL      *prclSrc,
    ULONG      iTransColor,
    ULONG      ulReserved
    );

#endif // !WINNT_40

//
// Function prototype define's for the driver hooks.
//
// NOTE: These are only needed for NT4. For NT5, they are defined in winddi.h.
//

#ifdef WINNT_40

typedef BOOL (APIENTRY  *LPFNDLLINITIALIZE)(
    ULONG           ulReason
    );

typedef BOOL (APIENTRY *PFN_DrvBitBlt)(
    SURFOBJ        *psoTrg,
    SURFOBJ        *psoSrc,
    SURFOBJ        *psoMask,
    CLIPOBJ        *pco,
    XLATEOBJ       *pxlo,
    RECTL          *prclTrg,
    POINTL         *pptlSrc,
    POINTL         *pptlMask,
    BRUSHOBJ       *pbo,
    POINTL         *pptlBrush,
    ROP4            rop4
    );

typedef BOOL (APIENTRY *PFN_DrvStretchBlt)(
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

typedef BOOL (APIENTRY *PFN_DrvCopyBits)(
    SURFOBJ        *psoDest,
    SURFOBJ        *psoSrc,
    CLIPOBJ        *pco,
    XLATEOBJ       *pxlo,
    RECTL          *prclDest,
    POINTL         *pptlSrc
    );

typedef BOOL (APIENTRY *PFN_DrvTextOut)(
    SURFOBJ    *pso,
    STROBJ     *pstro,
    FONTOBJ    *pfo,
    CLIPOBJ    *pco,
    RECTL      *prclExtra,
    RECTL      *prclOpaque,
    BRUSHOBJ   *pboFore,
    BRUSHOBJ   *pboOpaque,
    POINTL     *pptlOrg,
    MIX         mix
    );

typedef BOOL (APIENTRY *PFN_DrvStrokePath)(
    SURFOBJ    *pso,
    PATHOBJ    *ppo,
    CLIPOBJ    *pco,
    XFORMOBJ   *pxo,
    BRUSHOBJ   *pbo,
    POINTL     *pptlBrushOrg,
    LINEATTRS  *plineattrs,
    MIX         mix
    );

typedef BOOL (APIENTRY *PFN_DrvFillPath)(
    SURFOBJ    *pso,
    PATHOBJ    *ppo,
    CLIPOBJ    *pco,
    BRUSHOBJ   *pbo,
    POINTL     *pptlBrushOrg,
    MIX         mix,
    FLONG       flOptions
    );

typedef BOOL (APIENTRY *PFN_DrvStrokeAndFillPath)(
    SURFOBJ    *pso,
    PATHOBJ    *ppo,
    CLIPOBJ    *pco,
    XFORMOBJ   *pxo,
    BRUSHOBJ   *pboStroke,
    LINEATTRS  *plineattrs,
    BRUSHOBJ   *pboFill,
    POINTL     *pptlBrushOrg,
    MIX         mixFill,
    FLONG       flOptions
    );

typedef BOOL (APIENTRY *PFN_DrvRealizeBrush)(
    BRUSHOBJ   *pbo,
    SURFOBJ    *psoTarget,
    SURFOBJ    *psoPattern,
    SURFOBJ    *psoMask,
    XLATEOBJ   *pxlo,
    ULONG       iHatch
    );

typedef BOOL (APIENTRY *PFN_DrvStartPage)(
    SURFOBJ    *pso
    );

typedef BOOL (APIENTRY *PFN_DrvSendPage)(
    SURFOBJ    *pso
    );

typedef ULONG (APIENTRY *PFN_DrvEscape)(
    SURFOBJ    *pso,
    ULONG       iEsc,
    ULONG       cjIn,
    PVOID       pvIn,
    ULONG       cjOut,
    PVOID       pvOut
    );

typedef BOOL (APIENTRY *PFN_DrvStartDoc)(
    SURFOBJ    *pso,
    PWSTR       pwszDocName,
    DWORD       dwJobId
    );

typedef BOOL (APIENTRY *PFN_DrvEndDoc)(
    SURFOBJ    *pso,
    FLONG       fl
    );

typedef PIFIMETRICS (APIENTRY *PFN_DrvQueryFont)(
    DHPDEV      dhpdev,
    ULONG_PTR    iFile,
    ULONG       iFace,
    ULONG_PTR   *pid
    );

typedef PVOID (APIENTRY *PFN_DrvQueryFontTree)(
    DHPDEV      dhpdev,
    ULONG_PTR    iFile,
    ULONG       iFace,
    ULONG       iMode,
    ULONG_PTR   *pid
    );

typedef LONG (APIENTRY *PFN_DrvQueryFontData)(
    DHPDEV      dhpdev,
    FONTOBJ    *pfo,
    ULONG       iMode,
    HGLYPH      hg,
    GLYPHDATA  *pgd,
    PVOID       pv,
    ULONG       cjSize
    );

typedef BOOL (APIENTRY *PFN_DrvQueryAdvanceWidths)(
    DHPDEV      dhpdev,
    FONTOBJ    *pfo,
    ULONG       iMode,
    HGLYPH     *phg,
    PVOID       pvWidths,
    ULONG       cGlyphs
    );

typedef ULONG (APIENTRY *PFN_DrvFontManagement)(
    SURFOBJ    *pso,
    FONTOBJ    *pfo,
    ULONG       iMode,
    ULONG       cjIn,
    PVOID       pvIn,
    ULONG       cjOut,
    PVOID       pvOut
    );

typedef ULONG (APIENTRY *PFN_DrvGetGlyphMode)(
    DHPDEV      dhpdev,
    FONTOBJ    *pfo
    );

typedef BOOL (APIENTRY *PFN_DrvNextBand)(
    SURFOBJ *pso,
    POINTL *pptl
    );

typedef BOOL (APIENTRY *PFN_DrvStartBanding)(
    SURFOBJ *pso,
    POINTL *pptl
    );

typedef ULONG (APIENTRY *PFN_DrvDitherColor)(
    DHPDEV  dhpdev,
    ULONG   iMode,
    ULONG   rgbColor,
    ULONG  *pulDither
    );

typedef BOOL (APIENTRY *PFN_DrvPaint)(
    SURFOBJ         *pso,
    CLIPOBJ         *pco,
    BRUSHOBJ        *pbo,
    POINTL          *pptlBrushOrg,
    MIX             mix
    );

typedef BOOL (APIENTRY *PFN_DrvLineTo)(
    SURFOBJ    *pso,
    CLIPOBJ    *pco,
    BRUSHOBJ   *pbo,
    LONG        x1,
    LONG        y1,
    LONG        x2,
    LONG        y2,
    RECTL      *prclBounds,
    MIX         mix
    );

#endif // WINNT_40

//
// for the IPrintOemPS2::GetPDEVAdjustment call
//
#define PDEV_ADJUST_PAPER_MARGIN_TYPE 1

typedef struct _PDEV_ADJUST_PAPER_MARGIN {

    RECTL   rcImageableArea;    // contains the imageable area in 0.001 mm units

} PDEV_ADJUST_PAPER_MARGIN;

#define PDEV_HOSTFONT_ENABLED_TYPE 2

typedef struct _PDEV_HOSTFONT_ENABLED {

    BOOL bHostfontEnabled;

} PDEV_HOSTFONT_ENABLED;

#define PDEV_USE_TRUE_COLOR_TYPE 3

typedef struct _PDEV_USE_TRUE_COLOR {

    BOOL bUseTrueColor;

} PDEV_USE_TRUE_COLOR;

#endif // KERNEL_MODE


/*******************************************************************************
 *
 * Definitions used by user-mode UI module only:
 *  Make sure the macro KERNEL_MODE is NOT defined and
 *  the header file winddiui.h is included before this file.
 *
 */

#ifndef KERNEL_MODE

//
// Reports capability of simulated features
//
typedef struct _SIMULATE_CAPS_1 {
    DWORD     dwLevel;
    DWORD     dwPageOrderFlags;         // Reverse page order
    DWORD     dwNumberOfCopies;         // Max number of copies
    DWORD     dwCollate;                // Collate support
    DWORD     dwNupOptions;             // The (1-base) bit set represents the N-up option available.
                                        // 0x0001 means 1-up
                                        // 0x0002 means 2-up
                                        // 0x0008 means 4-up
                                        // 0x812B means (1,2,4,6,9,16)
} SIMULATE_CAPS_1, *PSIMULATE_CAPS_1;

//
// Reference to driver data structure. This is passed to
// most of the OEM UI DLL entrypoints.
//

typedef struct _OEMUIPROCS {

    PFN_DrvGetDriverSetting DrvGetDriverSetting;
    PFN_DrvUpdateUISetting  DrvUpdateUISetting;

} OEMUIPROCS, *POEMUIPROCS;

typedef struct _OEMUIOBJ {

    DWORD       cbSize;             // size of this structure
    POEMUIPROCS pOemUIProcs;        // pointer to user mode helper function table

} OEMUIOBJ, *POEMUIOBJ;


//
// OEMCommonUIProp
//

typedef struct _OEMCUIPPARAM *POEMCUIPPARAM;
typedef LONG (APIENTRY *OEMCUIPCALLBACK)(PCPSUICBPARAM, POEMCUIPPARAM);

typedef struct _OEMCUIPPARAM {

    DWORD           cbSize;         // size of this structure
    POEMUIOBJ       poemuiobj;      // reference to driver data structure
    HANDLE          hPrinter;       // handle to the current printer
    PWSTR           pPrinterName;   // name of current printer
    HANDLE          hModule;        // instance handle to OEM DLL
    HANDLE          hOEMHeap;       // handle to the OEM memory heap
    PDEVMODE        pPublicDM;      // public devmode
    PVOID           pOEMDM;         // OEM private devmode
    DWORD           dwFlags;        // misc. flag bits
    POPTITEM        pDrvOptItems;   // pointer to driver items
    DWORD           cDrvOptItems;   // number of driver items
    POPTITEM        pOEMOptItems;   // pointer to OEM items
    DWORD           cOEMOptItems;   // number of OEM items
    PVOID           pOEMUserData;   // pointer to OEM private data
    OEMCUIPCALLBACK  OEMCUIPCallback; // address of callback function

} OEMCUIPPARAM;

BOOL APIENTRY
OEMCommonUIProp(
    DWORD           dwMode,
    POEMCUIPPARAM   pOemCUIPParam
    );

//
// OEMCommonUIProp dwMode parameter value
//
#define OEMCUIP_DOCPROP       1
#define OEMCUIP_PRNPROP       2

//
// OEMDocumentPropertySheets
//

LRESULT APIENTRY
OEMDocumentPropertySheets(
    PPROPSHEETUI_INFO pPSUIInfo,
    LPARAM            lParam
    );

//
// OEMDevicePropertySheets
//

LRESULT APIENTRY
OEMDevicePropertySheets(
    PPROPSHEETUI_INFO pPSUIInfo,
    LPARAM            lParam
    );

//
// pPSUIInfo->lParamInit is a pointer to _OEMUIPSPARAM structure defined below.
//
typedef struct _OEMUIPSPARAM {

    DWORD           cbSize;         // size of this structure
    POEMUIOBJ       poemuiobj;      // reference to driver data structure
    HANDLE          hPrinter;       // handle to the current printer
    PWSTR           pPrinterName;   // name of current printer
    HANDLE          hModule;        // instance handle to OEM DLL
    HANDLE          hOEMHeap;       // handle to the OEM memory heap
    PDEVMODE        pPublicDM;      // public devmode
    PVOID           pOEMDM;         // OEM private devmode
    PVOID           pOEMUserData;   // pointer to OEM private data
    DWORD           dwFlags;        // misc. flag bits
    PVOID           pOemEntry;

} OEMUIPSPARAM, *POEMUIPSPARAM;

//
// OEMDevQueryPrintEx
//

BOOL APIENTRY
OEMDevQueryPrintEx(
    POEMUIOBJ           poemuiobj,
    PDEVQUERYPRINT_INFO pDQPInfo,
    PDEVMODE            pPublicDM,
    PVOID               pOEMDM
    );

//
// OEMDeviceCapabilities
//

DWORD APIENTRY
OEMDeviceCapabilities(
    POEMUIOBJ   poemuiobj,
    HANDLE      hPrinter,
    PWSTR       pDeviceName,
    WORD        wCapability,
    PVOID       pOutput,
    PDEVMODE    pPublicDM,
    PVOID       pOEMDM,
    DWORD       dwLastResult
    );

//
// OEMUpgradePrinter
//

BOOL APIENTRY
OEMUpgradePrinter(
    DWORD   dwLevel,
    PBYTE   pDriverUpgradeInfo
    );

//
// OEMUpgradeRegistry
//

BOOL APIENTRY
OEMUpgradeRegistry(
    DWORD   dwLevel,
    PBYTE   pDriverUpgradeInfo,
    PFN_DrvUpgradeRegistrySetting pfnUpgrade
    );


//
// OEMPrinterEvent
//

BOOL APIENTRY
OEMPrinterEvent(
    PWSTR   pPrinterName,
    INT     iDriverEvent,
    DWORD   dwFlags,
    LPARAM  lParam
    );

//
// OEMDriverEvent
//

BOOL APIENTRY
OEMPDriverEvent(
    DWORD   dwDriverEvent,
    DWORD   dwLevel,
    LPBYTE  pDriverInfo,
    LPARAM  lParam
    );


//
// OEMQueryColorProfile
//

BOOL APIENTRY
OEMQueryColorProfile(
    HANDLE      hPrinter,
    POEMUIOBJ   poemuiobj,
    PDEVMODE    pPublicDM,
    PVOID       pOEMDM,
    ULONG       ulQueryMode,
    VOID       *pvProfileData,
    ULONG      *pcbProfileData,
    FLONG      *pflProfileData
    );

//
// Font Installer dialog proc
//

INT_PTR CALLBACK
OEMFontInstallerDlgProc(
    HWND    hWnd,
    UINT    usMsg,
    WPARAM  wParam,
    LPARAM  lParam
    );


BOOL CALLBACK
OEMUpdateExternalFonts(
    HANDLE  hPrinter,
    HANDLE  hHeap,
    PWSTR   pwstrCartridges
   );


#endif // !KERNEL_MODE

//
// Datatypes for attributes plugin can get by calling core driver's
// following helper functions:
//
// GetGlobalAttribute, GetFeatureAttribute, GetOptionAttribute
//

typedef enum _EATTRIBUTE_DATATYPE {

    kADT_UNKNOWN,
    kADT_BOOL,
    kADT_INT,
    kADT_LONG,
    kADT_DWORD,
    kADT_ASCII,              // NULL terminated ASCII string
    kADT_UNICODE,            // NULL terminated Unicode string
    kADT_BINARY,             // binary blob
    kADT_SIZE,
    kADT_RECT,
    kADT_CUSTOMSIZEPARAMS,   // array of CUSTOMSIZEPARAM structures

} EATTRIBUTE_DATATYPE;

//
// Data structure for storing information about PPD's *ParamCustomPageSize entries
//

#define CUSTOMPARAM_WIDTH        0
#define CUSTOMPARAM_HEIGHT       1
#define CUSTOMPARAM_WIDTHOFFSET  2
#define CUSTOMPARAM_HEIGHTOFFSET 3
#define CUSTOMPARAM_ORIENTATION  4
#define CUSTOMPARAM_MAX          5

typedef struct _CUSTOMSIZEPARAM {

    LONG    dwOrder;                // order value
    LONG    lMinVal;                // min value (in microns)
    LONG    lMaxVal;                // max value (in microns)

} CUSTOMSIZEPARAM, *PCUSTOMSIZEPARAM;

//
// constants for SetOptions helper function
//
// SetOptions flag
//

#define SETOPTIONS_FLAG_RESOLVE_CONFLICT       0x00000001
#define SETOPTIONS_FLAG_KEEP_CONFLICT          0x00000002

//
// SetOptions result code
//

#define SETOPTIONS_RESULT_NO_CONFLICT          0
#define SETOPTIONS_RESULT_CONFLICT_RESOLVED    1
#define SETOPTIONS_RESULT_CONFLICT_REMAINED    2

#ifdef __cplusplus
}
#endif

#endif  // !_PRINTOEM_


