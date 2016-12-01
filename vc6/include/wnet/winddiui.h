/*++

Copyright (c) 1990-2001  Microsoft Corporation

Module Name:

    WinDDIUI.h

Abstract:

    Header file for the UI portion of printer drivers.

Revision History:

--*/
#ifndef _WINDDIUI_
#define _WINDDIUI_

#include <compstui.h>

#ifdef __cplusplus
extern "C" {
#endif

//
// DrvDevicePropertySheets replace previous version of PrinterProperties
//

LONG WINAPI
DrvDevicePropertySheets(
    PPROPSHEETUI_INFO   pPSUIInfo,
    LPARAM              lParam
    );

typedef struct _DEVICEPROPERTYHEADER {
    WORD    cbSize;
    WORD    Flags;
    HANDLE  hPrinter;
    LPTSTR  pszPrinterName;
} DEVICEPROPERTYHEADER, *PDEVICEPROPERTYHEADER;

#define DPS_NOPERMISSION    0x0001


//
// For document properties replace DocumentProperties.
//
// Note: if pPSUIInfo is NULL then the call need not to display any dialog
//       boxes (Ignored the DC_PROMPT bit in the fMode, the lParam in this case
//       is a pointer to DOCUMENTPROPERTYHEADER
//

LONG WINAPI
DrvDocumentPropertySheets(
    PPROPSHEETUI_INFO   pPSUIInfo,
    LPARAM              lParam
    );

typedef struct _DOCUMENTPROPERTYHEADER {
    WORD        cbSize;
    WORD        Reserved;
    HANDLE      hPrinter;
    LPTSTR      pszPrinterName;
    PDEVMODE    pdmIn;
    PDEVMODE    pdmOut;
    DWORD       cbOut;
    DWORD       fMode;
} DOCUMENTPROPERTYHEADER, *PDOCUMENTPROPERTYHEADER;

#define DM_ADVANCED         0x10
#define DM_NOPERMISSION     0x20
#define DM_USER_DEFAULT     0x40


// Devmode conversion function used by GetPrinter and SetPrinter

BOOL WINAPI
DrvConvertDevMode(
    LPTSTR   pPrinterName,
    PDEVMODE pdmIn,
    PDEVMODE pdmOut,
    PLONG    pcbNeeded,
    DWORD    fMode
    );

#define CDM_CONVERT         0x01
#define CDM_CONVERT351      0x02
#define CDM_DRIVER_DEFAULT  0x04


//
// This is for DevQueryPrintEx()
//

typedef struct _DEVQUERYPRINT_INFO {
    WORD    cbSize;         // size of this structure in bytes
    WORD    Level;          // Level of this info, 1 for this version
    HANDLE  hPrinter;       // handle to the printer for the query
    DEVMODE *pDevMode;      // pointer to the DEVMODE for this job.
    LPTSTR  pszErrorStr;    // pointer to the error string buffer.
    DWORD   cchErrorStr;    // count characters of pwErrorStr passed.
    DWORD   cchNeeded;      // count characters of pwErrorStr needed.
    } DEVQUERYPRINT_INFO, *PDEVQUERYPRINT_INFO;

BOOL WINAPI
DevQueryPrintEx(
    PDEVQUERYPRINT_INFO pDQPInfo
    );

//
// This for the DrvUpgradePrinter
//

typedef struct _DRIVER_UPGRADE_INFO_1 {
    LPTSTR  pPrinterName;
    LPTSTR  pOldDriverDirectory;
} DRIVER_UPGRADE_INFO_1, *PDRIVER_UPGRADE_INFO_1;

typedef struct _DRIVER_UPGRADE_INFO_2 {
    LPTSTR   pPrinterName;
    LPTSTR   pOldDriverDirectory;
    DWORD    cVersion;
    LPTSTR   pName;
    LPTSTR   pEnvironment;
    LPTSTR   pDriverPath;
    LPTSTR   pDataFile;
    LPTSTR   pConfigFile;
    LPTSTR   pHelpFile;
    LPTSTR   pDependentFiles;
    LPTSTR   pMonitorName;
    LPTSTR   pDefaultDataType;
    LPTSTR   pszzPreviousNames;
} DRIVER_UPGRADE_INFO_2, *PDRIVER_UPGRADE_INFO_2;

BOOL WINAPI
DrvUpgradePrinter(
    DWORD   Level,
    LPBYTE  pDriverUpgradeInfo
    );

//
// DrvDocumentEvent
//
//
//  Defines and proto-types for hooking GDI printer management functions
//
//  return values: -1 means error, 0 means not supported function
//
//  CreateDCPre must return > 0 or none of the others will be called.
//
//
//  CREATEDCPRE
//      return failure from CreateDC if this fails, CREATEDCPOST not called
//      bIC - TRUE if came from CreateIC
//      output devmode - this is the devmode that actualy gets passed to the
//      server side driver.  Any data needed in EnablePDEV should be passed
//      as part of the DriverExtra.
//
//  CREATEDCPOST
//      return value is ignored
//      the hdc will be 0 if something failed since CREATEDCPRE
//      The input buffer contains a pointer to the devmode returned in the
//      CREATEDCPRE output buffer
//
//  RESETDCPRE
//      return failure from ResetDC if this fails, CREATEDCPOST not called
//
//  RESETDCPOST
//      return value is ignored
//
//  STARTDOCPRE
//      return failure form StartDoc if this fails, driver not called
//
//  STARTDOCPOST
//      return failure form StartDoc if this fails, driver already called.
//      AbortDoc() called.
//
//  STARTPAGE
//      return failure form EndPage if this fails, driver not called
//
//  ENDPAGE
//      return value is ignored, DrvEndPage always called
//
//  ENDDOCPRE
//      return value is ignored, DrvEndDoc always called
//
//  ENDDOCPOST
//      return value is ignored, DrvEndDoc has alreadybeen called
//
//  ABORTDOC
//      return value is ignored
//
//  DELETEDC
//      return value is ignored
//
//  EXTESCAPE
//      return value is ignored
//      The input buffer includes the ExtEscape escape value, size of input
//      buffer to ExtEscape and the input buffer passed in.
//      The output buffer is just the buffer that was passed to ExtEscape
//
//  DOCUMENTEVENT_SPOOLED
//      This flag is added to the iEsc value if the document is being spooled
//      to a metafile rather than going direct.  Note that if this bit is set
//
//

#define DOCUMENTEVENT_EVENT(iEsc) (LOWORD(iEsc))
#define DOCUMENTEVENT_FLAGS(iEsc) (HIWORD(iEsc))

typedef struct _DOCEVENT_FILTER {
    UINT    cbSize;
    UINT    cElementsAllocated;
    UINT    cElementsNeeded;
    UINT    cElementsReturned;
    DWORD   aDocEventCall[ANYSIZE_ARRAY];
} DOCEVENT_FILTER, *PDOCEVENT_FILTER;

//
// Add structures usded for each DocumentEvent calls
//

typedef struct _DOCEVENT_CREATEDCPRE {
    PWSTR       pszDriver;
    PWSTR       pszDevice;
    PDEVMODEW   pdm;
    BOOL        bIC;
} DOCEVENT_CREATEDCPRE, *PDCEVENT_CREATEDCPRE;

typedef struct _DOCEVENT_ESCAPE {
    int    iEscape;
    int    cjInput;
    PVOID  pvInData;
} DOCEVENT_ESCAPE, *PDOCEVENT_ESCAPE;

//
// Escape codes for DrvDocumentEvent
//

#define DOCUMENTEVENT_FIRST         1   // Inclusive lower bound
#define DOCUMENTEVENT_CREATEDCPRE   1   // in-pszDriver, pszDevice, pdm, bIC, out-ppdm
#define DOCUMENTEVENT_CREATEDCPOST  2   // in-ppdm
#define DOCUMENTEVENT_RESETDCPRE    3   // in-pszDriver, pszDevice, pdm, out-ppdm
#define DOCUMENTEVENT_RESETDCPOST   4   // in-ppdm
#define DOCUMENTEVENT_STARTDOC      5   // none
#define DOCUMENTEVENT_STARTDOCPRE   5   // none
#define DOCUMENTEVENT_STARTPAGE     6   // none
#define DOCUMENTEVENT_ENDPAGE       7   // none
#define DOCUMENTEVENT_ENDDOC        8   // none
#define DOCUMENTEVENT_ENDDOCPRE     8   // none
#define DOCUMENTEVENT_ABORTDOC      9   // none
#define DOCUMENTEVENT_DELETEDC     10   // none
#define DOCUMENTEVENT_ESCAPE       11   // in-iEsc, cjInBuf, inBuf, out-outBuf
#define DOCUMENTEVENT_ENDDOCPOST   12   // none
#define DOCUMENTEVENT_STARTDOCPOST 13   // none
#define DOCUMENTEVENT_QUERYFILTER  14   // none
#define DOCUMENTEVENT_LAST         15   // Non-inclusive upper bound

#define DOCUMENTEVENT_SPOOLED   0x10000

//
// Return values for DrvDocumentEvent
//

#define DOCUMENTEVENT_SUCCESS     1
#define DOCUMENTEVENT_UNSUPPORTED 0
#define DOCUMENTEVENT_FAILURE     -1

int WINAPI
DrvDocumentEvent(
    HANDLE  hPrinter,
    HDC     hdc,
    int     iEsc,
    ULONG   cbIn,
    PVOID   pvIn,
    ULONG   cbOut,
    PVOID   pvOut
);


//
// DrvPrinterEvent
//
//
//    DrvPrinterEvent are called by the print subsystem when events
//    happen that might be of interest to a printer driver
//    The only event which should be implemented in the driver
//    is PRITNER_EVENT_INITIALIZE so that default settings are created
//    for the printer.
//
// PRINTER_EVENT_CONFIGURATION_CHANGE
//        Reserve it for future use.Xerox is already using it.
//
// PRINTER_EVENT_ADD_CONNECTION
//        return value ignored
//        Called after a successful AddPrinterConnection API
//        in the context of the calling app
//        lParam NULL
//
// PRINTER_EVENT_DELETE_CONNECTION
//        return value ignored
//        Called Before DeletePrinterConnect API
//        in the context of the calling app
//        lParam NULL
//
// PRINTER_EVENT_INITIALIZE
//        Called when a printer is created for the driver to
//        initialize its registry settings
//        Called in the spooler process
//        lParam NULL
//
// PRINTER_EVENT_DELETE
//        Called when a printer is about to be deleted
//        Called in the spooler process
//        lParam NULL
//
// PRINTER_EVENT_CACHE_REFRESH
//        return value ignored
//        called in spooler process
//        No UI
//        called when spooler detects that something has
//        changed in the workstaion cache or when establishing
//        the cache.
//        allows driver to update any private cache data
//        ( such as font files etc. )
//
// PRINTER_EVENT_CACHE_DELETE
//        return value ignored
//        called in spooler process
//        No UI
//        called when spooler is deleting a cached printer
//        allows printer driver to delete anything it has
//        cached
//
// PRINTER_EVENT_ATTRIBUTES_CHANGED
//        return value ignored
//        No UI
//        Called when the printer attribute bits for a given
//        printer have changed. Allows the driver to respond
//        appropriately.
//        lParam is a pointer to a PRINTER_EVENT_ATTRIBUTES_INFO
//        structure.
//
// PRINTER_EVENT_FLAG_NO_UI
//        Do not bring up UI when this flag it ON
//

//
// DrvPrinterEvent DriverEvent code
//
#define PRINTER_EVENT_CONFIGURATION_CHANGE      0
#define PRINTER_EVENT_ADD_CONNECTION            1
#define PRINTER_EVENT_DELETE_CONNECTION         2
#define PRINTER_EVENT_INITIALIZE                3
#define PRINTER_EVENT_DELETE                    4
#define PRINTER_EVENT_CACHE_REFRESH             5
#define PRINTER_EVENT_CACHE_DELETE              6
#define PRINTER_EVENT_ATTRIBUTES_CHANGED        7

//
// DrvPrinterEvent Flags
//

#define PRINTER_EVENT_FLAG_NO_UI        0x00000001

//
// lParam of PRINTER_EVENT_ATTRIBUTES_CHANGED points to this structure.
//
typedef struct _PRINTER_EVENT_ATTRIBUTES_INFO {
    DWORD       cbSize;
    DWORD       dwOldAttributes;
    DWORD       dwNewAttributes;
} PRINTER_EVENT_ATTRIBUTES_INFO, *PPRINTER_EVENT_ATTRIBUTES_INFO;

BOOL WINAPI
DrvPrinterEvent(
    LPWSTR  pPrinterName,
    int     DriverEvent,
    DWORD   Flags,
    LPARAM  lParam
);

//
// DrvDriverEvent is called when any version of the printer driver is deleted.
//
#define DRIVER_EVENT_INITIALIZE        0x00000001
#define DRIVER_EVENT_DELETE            0x00000002

BOOL WINAPI
DrvDriverEvent(
    DWORD   dwDriverEvent,
    DWORD   dwLevel,
    LPBYTE  pDriverInfo,
    LPARAM  lParam
);

// Print processor capabilities for the driver.
#define BORDER_PRINT                   0x00000000        // default
#define NO_BORDER_PRINT                0x00000001

#define BOOKLET_PRINT                  0x00000002

#define NO_COLOR_OPTIMIZATION          0x00000000        // default
#define COLOR_OPTIMIZATION             0x00000001

typedef struct _ATTRIBUTE_INFO_1 {
    DWORD    dwJobNumberOfPagesPerSide;
    DWORD    dwDrvNumberOfPagesPerSide;
    DWORD    dwNupBorderFlags;
    DWORD    dwJobPageOrderFlags;
    DWORD    dwDrvPageOrderFlags;
    DWORD    dwJobNumberOfCopies;
    DWORD    dwDrvNumberOfCopies;
} ATTRIBUTE_INFO_1, *PATTRIBUTE_INFO_1;

typedef struct _ATTRIBUTE_INFO_2 {
    DWORD    dwJobNumberOfPagesPerSide;
    DWORD    dwDrvNumberOfPagesPerSide;
    DWORD    dwNupBorderFlags;
    DWORD    dwJobPageOrderFlags;
    DWORD    dwDrvPageOrderFlags;
    DWORD    dwJobNumberOfCopies;
    DWORD    dwDrvNumberOfCopies;
    DWORD    dwColorOptimization;           // Added for monochrome optimization
} ATTRIBUTE_INFO_2, *PATTRIBUTE_INFO_2;

#ifndef __ATTRIBUTE_INFO_3__
#define __ATTRIBUTE_INFO_3__
typedef struct _ATTRIBUTE_INFO_3 {
    DWORD    dwJobNumberOfPagesPerSide;
    DWORD    dwDrvNumberOfPagesPerSide;
    DWORD    dwNupBorderFlags;
    DWORD    dwJobPageOrderFlags;
    DWORD    dwDrvPageOrderFlags;
    DWORD    dwJobNumberOfCopies;
    DWORD    dwDrvNumberOfCopies;
    DWORD    dwColorOptimization;           // Added for monochrome optimization
    short    dmPrintQuality;                // Added for monochrome optimization
    short    dmYResolution;                 // Added for monochrome optimization
} ATTRIBUTE_INFO_3, *PATTRIBUTE_INFO_3;

#endif

//
// DrvQueryJobAttributes is called by the spooler(print processor) to get information
// about the printing options used with the job. These options include N-up and reverse
// order printing.
//
BOOL WINAPI
DrvQueryJobAttributes(
    HANDLE      hPrinter,
    PDEVMODE    pDevMode,
    DWORD       dwLevel,
    LPBYTE      lpAttributeInfo
);

//
// DrvQueryColorProfile is called by the GDI (graphics device interface) to get information
// about the default color profile for the given DEVMODE, used with ICM (image color
// management).
//
BOOL WINAPI
DrvQueryColorProfile(
    HANDLE      hPrinter,
    PDEVMODEW   pdevmode,
    ULONG       ulQueryMode,
    VOID       *pvProfileData,
    ULONG      *pcbProfileData,
    FLONG      *pflProfileData
);

// The value for ulQueryMode
#define QCP_DEVICEPROFILE   0x0000
#define QCP_SOURCEPROFILE   0x0001

// The flags for pflProfileData.
#define QCP_PROFILEMEMORY  0x0001 // The pvProfileData points the color profile data itself.
#define QCP_PROFILEDISK    0x0002 // The pvProfileData points the color profile file name in Unicode.

//
//  User Mode Printer Driver DLL,
//
//  Note on hPrinter passed into DrvSplStartDoc() and subsequent
//  DrvSplxxx calls
//
//
//  A. If you have DrvSplxxxx calls in separate DLL and link it with
//     spoolss.lib.
//
//      * The hPrinter will be valid for any call to the spooler, such as
//        WritePrinter(), GetPrinterData()
//
//      * To do this you must
//
//          1. Have separate DLL for all DrvSplxxx functions.
//          2. Put this DLL name into your dependency files (inf).
//          3. link to spoolss.lib rather than winspool.lib
//          4. Use SetPrinterData() with SPLPRINTER_USER_MODE_PRINTER_DRIVER
//             as key name, and this DLL name as data.
//          5. Call any spooler functions linked from spoolss.lib
//
//
//
//  B. If you have DrvSplxxx calls located in your printer driver UI DLL and
//     linked with winspool.lib
//
//      * The hPrinter is NOT valid for any spooler calls, such as
//        WritePrinter(), GetPrinterData() from within the DrvSplxxx driver
//        functions.
//
//      * To do any spooler call from inside of DrvSplxxxx function you must
//        do the following
//
//          1. hSpoolSS = LoadLibrary("spoolss.dll");
//          2. pfn = GetProcAddress("WritePrinter") or whatever the spooler
//             functions you wish to call
//          3. Call the pfn function pointer returned from GetProcAddress()
//          4. FreeLibrary(hSpoolSS);
//
//
//  The A method is recommended.
//
//
//  If a UserModePrinterDriver DLL is created the following routines are
//  required or optional
//
//  Required Routines
//      DrvSplStartDoc
//      DrvSplWritePrinter
//      DrvSplEndDoc
//      DrvSplClose
//
//
//  Optional Routines
//      DrvSplStart
//      DrvSplEndPage
//      DrvSplAbort
//
//


HANDLE WINAPI
DrvSplStartDoc(
    HANDLE  hPrinter,
    DWORD   JobId
);


BOOL WINAPI
DrvSplWritePrinter(
    HANDLE  hDriver,
    LPVOID  pBuf,
    DWORD   cbBuf,
    LPDWORD pcWritten
);

VOID WINAPI
DrvSplEndDoc(
    HANDLE  hDriver
);


VOID WINAPI
DrvSplClose(
    HANDLE  hDriver
);


BOOL WINAPI
DrvSplStartPage(
    HANDLE  hDriver
);

BOOL WINAPI
DrvSplEndPage(
    HANDLE  hDriver
);

VOID WINAPI
DrvSplAbort(
    HANDLE  hDriver
);

DWORD
DrvSplDeviceCaps(
    HANDLE      hPrinter,
    PWSTR       pszDeviceName,
    WORD        Capability,
    PVOID       pOutput,
    DWORD       cchBufSize,
    PDEVMODE    pDevmode
    );

DWORD
DrvDeviceCapabilities(
    HANDLE      hPrinter,
    PWSTR       pszDeviceName,
    WORD        Capability,
    PVOID       pOutput,
    PDEVMODE    pDevmode
    );

//
//  Printer Attribute
//  Use with SetPrinterData to define UMPD.DLL
//

#define SPLPRINTER_USER_MODE_PRINTER_DRIVER     TEXT("SPLUserModePrinterDriver")

#ifdef __cplusplus
}
#endif

#endif  /* !_WINDDIUI_ */

