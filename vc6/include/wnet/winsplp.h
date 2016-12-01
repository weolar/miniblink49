/*++

Copyright (c) 1990  Microsoft Corporation

Module Name:

    WinSplp.h

Abstract:

    Internal Header file for Print APIs

Author:

Revision History:

--*/

#ifndef _WINSPLP_
#define _WINSPLP_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


#define PRINTER_NOTIFY_STATUS_ENDPOINT 1
#define PRINTER_NOTIFY_STATUS_POLL     2
#define PRINTER_NOTIFY_STATUS_INFO     4


#define ROUTER_UNKNOWN      0
#define ROUTER_SUCCESS      1
#define ROUTER_STOP_ROUTING 2

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

typedef struct _PRINTER_NOTIFY_INIT {
    DWORD Size;
    DWORD Reserved;
    DWORD PollTime;
} PRINTER_NOTIFY_INIT, *PPRINTER_NOTIFY_INIT, *LPPRINTER_NOTIFY_INIT;

typedef struct _SPLCLIENT_INFO_1{
    DWORD       dwSize;
    LPWSTR      pMachineName;
    LPWSTR      pUserName;
    DWORD       dwBuildNum;
    DWORD       dwMajorVersion;
    DWORD       dwMinorVersion;
    WORD        wProcessorArchitecture;
} SPLCLIENT_INFO_1, *PSPLCLIENT_INFO_1, *LPSPLCLIENT_INFO_1;

// This definition is used in the private spooler RPC interface (RpcSplOpenPrinter)
// The handle returned in the struct is the Server Side hPrinter which will used in
// making direct API calls from the client to the server side w/o the overhead of
// RPC. The performance boost is observed mainly in calls to Read/WritePrinter made from
// within the spooler (gdi32.dll during playback)
//
//
typedef struct _SPLCLIENT_INFO_2{

#ifdef _WIN64
    DWORD64 hSplPrinter;      // Server side handle to be used for direct calls
#else
    DWORD32 hSplPrinter;      // Server side handle to be used for direct calls
#endif

} SPLCLIENT_INFO_2, *PSPLCLIENT_INFO_2, *LPSPLCLIENT_INFO_2;

typedef struct _PRINTPROVIDOR {

   BOOL (*fpOpenPrinter)(LPWSTR   pPrinterName,
                         LPHANDLE phPrinter,
                         LPPRINTER_DEFAULTS pDefault);

   BOOL (*fpSetJob)(HANDLE hPrinter,
                    DWORD JobId,
                    DWORD Level,
                    LPBYTE pJob,
                    DWORD Command);

   BOOL (*fpGetJob)(HANDLE   hPrinter,
                    DWORD    JobId,
                    DWORD    Level,
                    LPBYTE   pJob,
                    DWORD    cbBuf,
                    LPDWORD  pcbNeeded);

   BOOL (*fpEnumJobs)(HANDLE  hPrinter,
                      DWORD   FirstJob,
                      DWORD   NoJobs,
                      DWORD   Level,
                      LPBYTE  pJob,
                      DWORD   cbBuf,
                      LPDWORD pcbNeeded,
                      LPDWORD pcReturned);

   HANDLE (*fpAddPrinter)(LPWSTR  pName,
                          DWORD   Level,
                          LPBYTE  pPrinter);

   BOOL (*fpDeletePrinter)(HANDLE   hPrinter);

   BOOL (*fpSetPrinter)(HANDLE  hPrinter,
                        DWORD   Level,
                        LPBYTE  pPrinter,
                        DWORD   Command);

   BOOL (*fpGetPrinter)(HANDLE  hPrinter,
                        DWORD   Level,
                        LPBYTE  pPrinter,
                        DWORD   cbBuf,
                        LPDWORD pcbNeeded);

   BOOL (*fpEnumPrinters)(DWORD   Flags,
                          LPWSTR  Name,
                          DWORD   Level,
                          LPBYTE  pPrinterEnum,
                          DWORD   cbBuf,
                          LPDWORD pcbNeeded,
                          LPDWORD pcReturned);

   BOOL (*fpAddPrinterDriver)(LPWSTR  pName,
                              DWORD   Level,
                              LPBYTE  pDriverInfo);

   BOOL (*fpEnumPrinterDrivers)(LPWSTR  pName,
                                LPWSTR  pEnvironment,
                                DWORD   Level,
                                LPBYTE  pDriverInfo,
                                DWORD   cbBuf,
                                LPDWORD pcbNeeded,
                                LPDWORD pcReturned);

   BOOL (*fpGetPrinterDriver)(HANDLE  hPrinter,
                              LPWSTR  pEnvironment,
                              DWORD   Level,
                              LPBYTE  pDriverInfo,
                              DWORD   cbBuf,
                              LPDWORD pcbNeeded);

   BOOL (*fpGetPrinterDriverDirectory)(LPWSTR  pName,
                                       LPWSTR  pEnvironment,
                                       DWORD   Level,
                                       LPBYTE  pDriverDirectory,
                                       DWORD   cbBuf,
                                       LPDWORD pcbNeeded);

   BOOL (*fpDeletePrinterDriver)(LPWSTR   pName,
                                 LPWSTR   pEnvironment,
                                 LPWSTR   pDriverName);

   BOOL (*fpAddPrintProcessor)(LPWSTR  pName,
                               LPWSTR  pEnvironment,
                               LPWSTR  pPathName,
                               LPWSTR  pPrintProcessorName);

   BOOL (*fpEnumPrintProcessors)(LPWSTR  pName,
                                 LPWSTR  pEnvironment,
                                 DWORD   Level,
                                 LPBYTE  pPrintProcessorInfo,
                                 DWORD   cbBuf,
                                 LPDWORD pcbNeeded,
                                 LPDWORD pcReturned);

   BOOL (*fpGetPrintProcessorDirectory)(LPWSTR  pName,
                                        LPWSTR  pEnvironment,
                                        DWORD   Level,
                                        LPBYTE  pPrintProcessorInfo,
                                        DWORD   cbBuf,
                                        LPDWORD pcbNeeded);

   BOOL (*fpDeletePrintProcessor)(LPWSTR  pName,
                                  LPWSTR  pEnvironment,
                                  LPWSTR  pPrintProcessorName);

   BOOL (*fpEnumPrintProcessorDatatypes)(LPWSTR  pName,
                                         LPWSTR  pPrintProcessorName,
                                         DWORD   Level,
                                         LPBYTE  pDataypes,
                                         DWORD   cbBuf,
                                         LPDWORD pcbNeeded,
                                         LPDWORD pcReturned);

   DWORD (*fpStartDocPrinter)(HANDLE  hPrinter,
                             DWORD   Level,
                             LPBYTE  pDocInfo);

   BOOL (*fpStartPagePrinter)(HANDLE  hPrinter);

   BOOL (*fpWritePrinter)(HANDLE  hPrinter,
                          LPVOID  pBuf,
                          DWORD   cbBuf,
                          LPDWORD pcWritten);

   BOOL (*fpEndPagePrinter)(HANDLE   hPrinter);

   BOOL (*fpAbortPrinter)(HANDLE   hPrinter);

   BOOL (*fpReadPrinter)(HANDLE  hPrinter,
                         LPVOID  pBuf,
                         DWORD   cbBuf,
                         LPDWORD pNoBytesRead);

   BOOL (*fpEndDocPrinter)(HANDLE   hPrinter);

   BOOL (*fpAddJob)(HANDLE  hPrinter,
                    DWORD   Level,
                    LPBYTE  pData,
                    DWORD   cbBuf,
                    LPDWORD pcbNeeded);

   BOOL (*fpScheduleJob)(HANDLE  hPrinter,
                         DWORD   JobId);

   DWORD (*fpGetPrinterData)(HANDLE   hPrinter,
                             LPWSTR   pValueName,
                             LPDWORD  pType,
                             LPBYTE   pData,
                             DWORD    nSize,
                             LPDWORD  pcbNeeded);

   DWORD (*fpSetPrinterData)(HANDLE  hPrinter,
                             LPWSTR  pValueName,
                             DWORD   Type,
                             LPBYTE  pData,
                             DWORD   cbData);

   DWORD (*fpWaitForPrinterChange)(HANDLE hPrinter, DWORD Flags);

   BOOL (*fpClosePrinter)(HANDLE hPrinter);

   BOOL (*fpAddForm)(HANDLE  hPrinter,
                     DWORD   Level,
                     LPBYTE  pForm);

   BOOL (*fpDeleteForm)(HANDLE  hPrinter,
                        LPWSTR  pFormName);

   BOOL (*fpGetForm)(HANDLE  hPrinter,
                     LPWSTR  pFormName,
                     DWORD   Level,
                     LPBYTE  pForm,
                     DWORD   cbBuf,
                     LPDWORD pcbNeeded);

   BOOL (*fpSetForm)(HANDLE  hPrinter,
                     LPWSTR  pFormName,
                     DWORD   Level,
                     LPBYTE  pForm);

   BOOL (*fpEnumForms)(HANDLE  hPrinter,
                       DWORD   Level,
                       LPBYTE  pForm,
                       DWORD   cbBuf,
                       LPDWORD pcbNeeded,
                       LPDWORD pcReturned);

   BOOL (*fpEnumMonitors)(LPWSTR  pName,
                          DWORD   Level,
                          LPBYTE  pMonitors,
                          DWORD   cbBuf,
                          LPDWORD pcbNeeded,
                          LPDWORD pcReturned);

   BOOL (*fpEnumPorts)(LPWSTR  pName,
                       DWORD   Level,
                       LPBYTE  pPorts,
                       DWORD   cbBuf,
                       LPDWORD pcbNeeded,
                       LPDWORD pcReturned);

   BOOL (*fpAddPort)(LPWSTR  pName,
                     HWND    hWnd,
                     LPWSTR  pMonitorName);

   BOOL (*fpConfigurePort)(LPWSTR  pName,
                           HWND    hWnd,
                           LPWSTR  pPortName);

   BOOL (*fpDeletePort)(LPWSTR  pName,
                        HWND    hWnd,
                        LPWSTR  pPortName);

   HANDLE (*fpCreatePrinterIC)(HANDLE  hPrinter,
                               LPDEVMODEW   pDevMode);

   BOOL (*fpPlayGdiScriptOnPrinterIC)(HANDLE  hPrinterIC,
                                      LPBYTE  pIn,
                                      DWORD   cIn,
                                      LPBYTE  pOut,
                                      DWORD   cOut,
                                      DWORD   ul);

   BOOL (*fpDeletePrinterIC)(HANDLE  hPrinterIC);

   BOOL (*fpAddPrinterConnection)(LPWSTR  pName);

   BOOL (*fpDeletePrinterConnection)(LPWSTR pName);

   DWORD (*fpPrinterMessageBox)(HANDLE  hPrinter,
                                DWORD   Error,
                                HWND    hWnd,
                                LPWSTR  pText,
                                LPWSTR  pCaption,
                                DWORD   dwType);

   BOOL (*fpAddMonitor)(LPWSTR  pName,
                        DWORD   Level,
                        LPBYTE  pMonitorInfo);

   BOOL (*fpDeleteMonitor)(LPWSTR  pName,
                           LPWSTR  pEnvironment,
                           LPWSTR  pMonitorName);

   BOOL (*fpResetPrinter)(HANDLE hPrinter,
                          LPPRINTER_DEFAULTS pDefault);

   BOOL (*fpGetPrinterDriverEx)(HANDLE  hPrinter,
                              LPWSTR  pEnvironment,
                              DWORD   Level,
                              LPBYTE  pDriverInfo,
                              DWORD   cbBuf,
                              LPDWORD pcbNeeded,
                              DWORD   dwClientMajorVersion,
                              DWORD   dwClientMinorVersion,
                              PDWORD  pdwServerMajorVersion,
                              PDWORD  pdwServerMinorVersion);

   BOOL (*fpFindFirstPrinterChangeNotification)(
            HANDLE hPrinter,
            DWORD fdwFlags,
            DWORD fdwOptions,
            HANDLE hNotify,
            PDWORD pfdwStatus,
            PVOID pPrinterNotifyOptions,
            PVOID pPrinterNotifyInit);

   BOOL (*fpFindClosePrinterChangeNotification)(HANDLE hPrinter);


   BOOL (*fpAddPortEx)(LPWSTR   pName,
                       DWORD    Level,
                       LPBYTE   lpBuffer,
                       LPWSTR   lpMonitorName);

   BOOL (*fpShutDown)(LPVOID pvReserved);

   BOOL (*fpRefreshPrinterChangeNotification)(HANDLE hPrinter,
                                              DWORD  Reserved,
                                              PVOID pvReserved,
                                              PVOID pPrinterNotifyInfo);

   BOOL (*fpOpenPrinterEx)(LPWSTR     pPrinterName,
                           LPHANDLE   phPrinter,
                           LPPRINTER_DEFAULTS pDefault,
                           LPBYTE     pClientInfo,
                           DWORD      Level);

   HANDLE (*fpAddPrinterEx)(LPWSTR  pName,
                            DWORD   Level,
                            LPBYTE  pPrinter,
                            LPBYTE  pClientInfo,
                            DWORD   ClientInfoLevel);

   BOOL (*fpSetPort)(LPWSTR     pName,
                     LPWSTR     pPortName,
                     DWORD      Level,
                     LPBYTE     pPortInfo);

   DWORD (*fpEnumPrinterData)(HANDLE   hPrinter,
                              DWORD    dwIndex,
                              LPWSTR   pValueName,
                              DWORD    cbValueName,
                              LPDWORD  pcbValueName,
                              LPDWORD  pType,
                              LPBYTE   pData,
                              DWORD    cbData,
                              LPDWORD  pcbData);

   DWORD (*fpDeletePrinterData)(HANDLE   hPrinter,
                                LPWSTR   pValueName);

   DWORD (*fpClusterSplOpen)( LPCTSTR pszServer,
                              LPCTSTR pszResource,
                              PHANDLE phSpooler,
                              LPCTSTR pszName,
                              LPCTSTR pszAddress );


   DWORD (*fpClusterSplClose)( HANDLE hSpooler );

   DWORD (*fpClusterSplIsAlive)( HANDLE hSpooler );

   DWORD (*fpSetPrinterDataEx)(  HANDLE  hPrinter,
                                 LPCWSTR pKeyName,
                                 LPCWSTR pValueName,
                                 DWORD   Type,
                                 LPBYTE  pData,
                                 DWORD   cbData);

   DWORD (*fpGetPrinterDataEx)(HANDLE   hPrinter,
                               LPCWSTR  pKeyName,
                               LPCWSTR  pValueName,
                               LPDWORD  pType,
                               LPBYTE   pData,
                               DWORD    nSize,
                               LPDWORD  pcbNeeded);

   DWORD (*fpEnumPrinterDataEx)(  HANDLE  hPrinter,
                                  LPCWSTR pKeyName,
                                  LPBYTE  pEnumValues,
                                  DWORD   cbEnumValues,
                                  LPDWORD pcbEnumValues,
                                  LPDWORD pnEnumValues);

   DWORD (*fpEnumPrinterKey)( HANDLE   hPrinter,
                              LPCWSTR  pKeyName,
                              LPWSTR   pSubkey,
                              DWORD    cbSubkey,
                              LPDWORD  pcbSubkey);

   DWORD (*fpDeletePrinterDataEx)(  HANDLE  hPrinter,
                                    LPCWSTR pKeyName,
                                    LPCWSTR pValueName);

   DWORD (*fpDeletePrinterKey)(  HANDLE  hPrinter,
                                 LPCWSTR pKeyName);

   BOOL (*fpSeekPrinter)( HANDLE hPrinter,
                          LARGE_INTEGER liDistanceToMove,
                          PLARGE_INTEGER pliNewPointer,
                          DWORD dwMoveMethod,
                          BOOL bWrite );

   BOOL (*fpDeletePrinterDriverEx)(LPWSTR   pName,
                                   LPWSTR   pEnvironment,
                                   LPWSTR   pDriverName,
                                   DWORD    dwDeleteFlag,
                                   DWORD    dwVersionNum);

   BOOL (*fpAddPerMachineConnection)(LPCWSTR    pServer,
                                     LPCWSTR    pPrinterName,
                                     LPCWSTR    pPrintServer,
                                     LPCWSTR    pProvider);

   BOOL (*fpDeletePerMachineConnection)(LPCWSTR   pServer,
                                        LPCWSTR   pPrinterName);

   BOOL (*fpEnumPerMachineConnections)(LPCWSTR    pServer,
                                       LPBYTE     pPrinterEnum,
                                       DWORD      cbBuf,
                                       LPDWORD    pcbNeeded,
                                       LPDWORD    pcReturned);

   BOOL (*fpXcvData)(   HANDLE  hXcv,
                        LPCWSTR pszDataName,
                        PBYTE   pInputData,
                        DWORD   cbInputData,
                        PBYTE   pOutputData,
                        DWORD   cbOutputData,
                        PDWORD  pcbOutputNeeded,
                        PDWORD  pdwStatus
                        );

   BOOL (*fpAddPrinterDriverEx)(LPWSTR  pName,
                                DWORD   Level,
                                LPBYTE  pDriverInfo,
                                DWORD   dwFileCopyFlags);

   BOOL (*fpSplReadPrinter)(HANDLE hPrinter,
                            LPBYTE *pBuf,
                            DWORD  cbBuf);

   BOOL (*fpDriverUnloadComplete)(LPWSTR  pDriverFile);

   BOOL (*fpGetSpoolFileInfo)(HANDLE    hPrinter,
                              LPWSTR    *pSpoolDir,
                              LPHANDLE  phFile,
                              HANDLE    hSpoolerProcess,
                              HANDLE    hAppProcess);

   BOOL (*fpCommitSpoolData)(HANDLE  hPrinter,
                             DWORD   cbCommit);

   BOOL (*fpCloseSpoolFileHandle)(HANDLE  hPrinter);

   BOOL (*fpFlushPrinter)(HANDLE  hPrinter,
                          LPBYTE  pBuf,
                          DWORD   cbBuf,
                          LPDWORD pcWritten,
                          DWORD   cSleep);
   DWORD (*fpSendRecvBidiData)(HANDLE                    hPrinter,
                               LPCWSTR                   pAction,
                               PBIDI_REQUEST_CONTAINER   pReqData,
                               PBIDI_RESPONSE_CONTAINER* ppResData);

   BOOL (*fpAddDriverCatalog)(HANDLE  hPrinter,
                              DWORD   dwLevel,
                              VOID    *pvDriverInfCatInfo,
                              DWORD   dwCatalogCopyFlags);

} PRINTPROVIDOR, *LPPRINTPROVIDOR;

BOOL
InitializePrintProvidor(
   LPPRINTPROVIDOR  pPrintProvidor,
   DWORD    cbPrintProvidor,
   LPWSTR   pFullRegistryPath
);

typedef struct _PRINTPROCESSOROPENDATA {
    PDEVMODE  pDevMode;
    LPWSTR    pDatatype;
    LPWSTR    pParameters;
    LPWSTR    pDocumentName;
    DWORD     JobId;
    LPWSTR    pOutputFile;
    LPWSTR    pPrinterName;
} PRINTPROCESSOROPENDATA, *PPRINTPROCESSOROPENDATA, *LPPRINTPROCESSOROPENDATA;

HANDLE
OpenPrintProcessor(
    LPWSTR  pPrinterName,
    PPRINTPROCESSOROPENDATA pPrintProcessorOpenData
);

BOOL
PrintDocumentOnPrintProcessor(
    HANDLE  hPrintProcessor,
    LPWSTR  pDocumentName
);

BOOL
ClosePrintProcessor(
    HANDLE  hPrintProcessor
);

BOOL
ControlPrintProcessor(
    HANDLE  hPrintProcessor,
    DWORD   Command
);

DWORD
GetPrintProcessorCapabilities(
    LPTSTR   pValueName,
    DWORD    dwAttributes,
    LPBYTE   pData,
    DWORD    nSize,
    LPDWORD  pcbNeeded
);

BOOL
GetJobAttributes(
    LPWSTR            pPrinterName,
    LPDEVMODEW        pDevmode,
    PATTRIBUTE_INFO_3 pAttributeInfo
    );

BOOL
InitializeMonitor(
    LPWSTR  pRegistryRoot
);

BOOL
OpenPort(
    LPWSTR  pName,
    PHANDLE pHandle
);

BOOL
WritePort(
    HANDLE  hPort,
    LPBYTE  pBuffer,
    DWORD   cbBuf,
    LPDWORD pcbWritten
);

BOOL
ReadPort(
    HANDLE hPort,
    LPBYTE pBuffer,
    DWORD  cbBuffer,
    LPDWORD pcbRead
);

BOOL
ClosePort(
    HANDLE  hPort
);


BOOL
XcvOpenPort(
    LPCWSTR pszObject,
    ACCESS_MASK GrantedAccess,
    PHANDLE phXcv
);

DWORD
XcvDataPort(
    HANDLE  hXcv,
    LPCWSTR pszDataName,
    PBYTE   pInputData,
    DWORD   cbInputData,
    PBYTE   pOutputData,
    DWORD   cbOutputData,
    PDWORD  pcbOutputNeeded
);

BOOL
XcvClosePort(
    HANDLE  hXcv
);


BOOL
AddPortUI
(
    PCWSTR  pszServer,
    HWND    hWnd,
    PCWSTR  pszMonitorNameIn,
    PWSTR  *ppszPortNameOut
);

BOOL
ConfigurePortUI
(
    PCWSTR pszServer,
    HWND   hWnd,
    PCWSTR pszPortName
);

BOOL
DeletePortUI
(
    PCWSTR pszServer,
    HWND   hWnd,
    PCWSTR pszPortName
);


BOOL
SplDeleteSpoolerPortStart(
    PCWSTR pPortName
);

BOOL
SplDeleteSpoolerPortEnd(
    PCWSTR pName,
    BOOL   bDeletePort
);

typedef struct _MONITORREG {

    DWORD cbSize;

    LONG
    (WINAPI *fpCreateKey)(
        HANDLE hcKey,
        LPCTSTR pszSubKey,
        DWORD dwOptions,
        REGSAM samDesired,
        PSECURITY_ATTRIBUTES pSecurityAttributes,
        PHANDLE phckResult,
        PDWORD pdwDisposition,
        HANDLE hSpooler
        );

    LONG
    (WINAPI *fpOpenKey)(
        HANDLE hcKey,
        LPCTSTR pszSubKey,
        REGSAM samDesired,
        PHANDLE phkResult,
        HANDLE hSpooler
        );

    LONG
    (WINAPI *fpCloseKey)(
        HANDLE hcKey,
        HANDLE hSpooler
        );

    LONG
    (WINAPI *fpDeleteKey)(
        HANDLE hcKey,
        LPCTSTR pszSubKey,
        HANDLE hSpooler
        );

    LONG
    (WINAPI *fpEnumKey)(
        HANDLE hcKey,
        DWORD dwIndex,
        LPTSTR pszName,
        PDWORD pcchName,
        PFILETIME pftLastWriteTime,
        HANDLE hSpooler
        );

    LONG
    (WINAPI *fpQueryInfoKey)(
        HANDLE hcKey,
        PDWORD pcSubKeys,
        PDWORD pcbKey,
        PDWORD pcValues,
        PDWORD pcbValue,
        PDWORD pcbData,
        PDWORD pcbSecurityDescriptor,
        PFILETIME pftLastWriteTime,
        HANDLE hSpooler
        );

    LONG
    (WINAPI *fpSetValue)(
        HANDLE hcKey,
        LPCTSTR pszValue,
        DWORD dwType,
        const BYTE* pData,
        DWORD cbData,
        HANDLE hSpooler
        );

    LONG
    (WINAPI *fpDeleteValue)(
        HANDLE hcKey,
        LPCTSTR pszValue,
        HANDLE hSpooler
        );

    LONG
    (WINAPI *fpEnumValue)(
        HANDLE hcKey,
        DWORD dwIndex,
        LPTSTR pszValue,
        PDWORD pcbValue,
        PDWORD pType,
        PBYTE pData,
        PDWORD pcbData,
        HANDLE hSpooler
        );

    LONG
    (WINAPI *fpQueryValue)(
        HANDLE hcKey,
        LPCTSTR pszValue,
        PDWORD pType,
        PBYTE pData,
        PDWORD pcbData,
        HANDLE hSpooler
        );

} MONITORREG, *PMONITORREG;


typedef struct _MONITORINIT {
    DWORD cbSize;
    HANDLE hSpooler;
    HANDLE hckRegistryRoot;
    PMONITORREG pMonitorReg;
    BOOL bLocal;
    LPCWSTR pszServerName;
} MONITORINIT, *PMONITORINIT;


typedef struct _MONITOR
{
    BOOL (WINAPI *pfnEnumPorts)
    (
    LPWSTR   pName,
    DWORD   Level,
    LPBYTE  pPorts,
    DWORD   cbBuf,
    LPDWORD pcbNeeded,
    LPDWORD pcReturned
    );

    BOOL (WINAPI *pfnOpenPort)
    (
    LPWSTR  pName,
    PHANDLE pHandle
    );

    BOOL (WINAPI *pfnOpenPortEx)
    (
    LPWSTR  pPortName,
    LPWSTR  pPrinterName,
    PHANDLE pHandle,
    struct _MONITOR FAR *pMonitor
    );


    BOOL (WINAPI *pfnStartDocPort)
    (
    HANDLE  hPort,
    LPWSTR  pPrinterName,
    DWORD   JobId,
    DWORD   Level,
    LPBYTE  pDocInfo
    );

    BOOL (WINAPI *pfnWritePort)
    (
    HANDLE  hPort,
    LPBYTE  pBuffer,
    DWORD   cbBuf,
    LPDWORD pcbWritten
    );

    BOOL (WINAPI *pfnReadPort)
    (
    HANDLE hPort,
    LPBYTE pBuffer,
    DWORD  cbBuffer,
    LPDWORD pcbRead
    );

    BOOL (WINAPI *pfnEndDocPort)
    (
    HANDLE   hPort
    );

    BOOL (WINAPI *pfnClosePort)
    (
        HANDLE  hPort
    );

    BOOL (WINAPI *pfnAddPort)
    (
    LPWSTR   pName,
    HWND    hWnd,
    LPWSTR   pMonitorName
    );

    BOOL (WINAPI *pfnAddPortEx)
    (
    LPWSTR   pName,
    DWORD    Level,
    LPBYTE   lpBuffer,
    LPWSTR   lpMonitorName
    );

    BOOL (WINAPI *pfnConfigurePort)
    (
    LPWSTR   pName,
    HWND  hWnd,
    LPWSTR pPortName
    );

    BOOL (WINAPI *pfnDeletePort)
    (
    LPWSTR  pName,
    HWND    hWnd,
    LPWSTR  pPortName
    );

    BOOL (WINAPI *pfnGetPrinterDataFromPort)
    (
    HANDLE  hPort,
    DWORD   ControlID,
    LPWSTR  pValueName,
    LPWSTR  lpInBuffer,
    DWORD   cbInBuffer,
    LPWSTR  lpOutBuffer,
    DWORD   cbOutBuffer,
    LPDWORD lpcbReturned
    );

    BOOL (WINAPI *pfnSetPortTimeOuts)
    (
    HANDLE  hPort,
    LPCOMMTIMEOUTS lpCTO,
    DWORD   reserved    // must be set to 0
    );

    BOOL (WINAPI *pfnXcvOpenPort)
    (
    LPCWSTR pszObject,
    ACCESS_MASK GrantedAccess,
    PHANDLE phXcv
    );

    DWORD (WINAPI *pfnXcvDataPort)
    (
    HANDLE  hXcv,
    LPCWSTR pszDataName,
    PBYTE   pInputData,
    DWORD   cbInputData,
    PBYTE   pOutputData,
    DWORD   cbOutputData,
    PDWORD  pcbOutputNeeded
    );

    BOOL (WINAPI *pfnXcvClosePort)
    (
    HANDLE  hXcv
    );


} MONITOR, FAR *LPMONITOR;

typedef struct _MONITOREX
{
    DWORD       dwMonitorSize;
    MONITOR     Monitor;

} MONITOREX, FAR *LPMONITOREX;


typedef struct _MONITOR2
{
    DWORD cbSize;
    BOOL (WINAPI *pfnEnumPorts)
    (
    HANDLE  hMonitor,
    LPWSTR  pName,
    DWORD   Level,
    LPBYTE  pPorts,
    DWORD   cbBuf,
    LPDWORD pcbNeeded,
    LPDWORD pcReturned
    );

    BOOL (WINAPI *pfnOpenPort)
    (
    HANDLE  hMonitor,
    LPWSTR  pName,
    PHANDLE pHandle
    );

    BOOL (WINAPI *pfnOpenPortEx)
    (
    HANDLE  hMonitor,
    HANDLE  hMonitorPort,
    LPWSTR  pPortName,
    LPWSTR  pPrinterName,
    PHANDLE pHandle,
    struct _MONITOR2 FAR *pMonitor2
    );


    BOOL (WINAPI *pfnStartDocPort)
    (
    HANDLE  hPort,
    LPWSTR  pPrinterName,
    DWORD   JobId,
    DWORD   Level,
    LPBYTE  pDocInfo
    );

    BOOL (WINAPI *pfnWritePort)
    (
    HANDLE  hPort,
    LPBYTE  pBuffer,
    DWORD   cbBuf,
    LPDWORD pcbWritten
    );

    BOOL (WINAPI *pfnReadPort)
    (
    HANDLE hPort,
    LPBYTE pBuffer,
    DWORD  cbBuffer,
    LPDWORD pcbRead
    );

    BOOL (WINAPI *pfnEndDocPort)
    (
    HANDLE   hPort
    );

    BOOL (WINAPI *pfnClosePort)
    (
    HANDLE  hPort
    );

    BOOL (WINAPI *pfnAddPort)
    (
    HANDLE   hMonitor,
    LPWSTR   pName,
    HWND     hWnd,
    LPWSTR   pMonitorName
    );

    BOOL (WINAPI *pfnAddPortEx)
    (
    HANDLE   hMonitor,
    LPWSTR   pName,
    DWORD    Level,
    LPBYTE   lpBuffer,
    LPWSTR   lpMonitorName
    );

    BOOL (WINAPI *pfnConfigurePort)
    (
    HANDLE  hMonitor,
    LPWSTR  pName,
    HWND    hWnd,
    LPWSTR  pPortName
    );

    BOOL (WINAPI *pfnDeletePort)
    (
    HANDLE  hMonitor,
    LPWSTR  pName,
    HWND    hWnd,
    LPWSTR  pPortName
    );

    BOOL (WINAPI *pfnGetPrinterDataFromPort)
    (
    HANDLE  hPort,
    DWORD   ControlID,
    LPWSTR  pValueName,
    LPWSTR  lpInBuffer,
    DWORD   cbInBuffer,
    LPWSTR  lpOutBuffer,
    DWORD   cbOutBuffer,
    LPDWORD lpcbReturned
    );

    BOOL (WINAPI *pfnSetPortTimeOuts)
    (
    HANDLE  hPort,
    LPCOMMTIMEOUTS lpCTO,
    DWORD   reserved    // must be set to 0
    );

    BOOL (WINAPI *pfnXcvOpenPort)
    (
    HANDLE  hMonitor,
    LPCWSTR pszObject,
    ACCESS_MASK GrantedAccess,
    PHANDLE phXcv
    );

    DWORD (WINAPI *pfnXcvDataPort)
    (
    HANDLE  hXcv,
    LPCWSTR pszDataName,
    PBYTE   pInputData,
    DWORD   cbInputData,
    PBYTE   pOutputData,
    DWORD   cbOutputData,
    PDWORD  pcbOutputNeeded
    );

    BOOL (WINAPI *pfnXcvClosePort)
    (
    HANDLE  hXcv
    );

    VOID (WINAPI *pfnShutdown)
    (
    HANDLE hMonitor
    );

    DWORD (WINAPI *pfnSendRecvBidiDataFromPort)
    (
    HANDLE                    hPort,
    DWORD                     dwAccessBit,
    LPCWSTR                   pAction,
    PBIDI_REQUEST_CONTAINER   pReqData,
    PBIDI_RESPONSE_CONTAINER* ppResData
    );

} MONITOR2, *PMONITOR2, FAR *LPMONITOR2;

#define MONITOR2_SIZE_WIN2K ( sizeof(DWORD) + (sizeof(PVOID)*18) )

typedef struct _MONITORUI
{
    DWORD   dwMonitorUISize;

    BOOL (WINAPI *pfnAddPortUI)
    (
        PCWSTR pszServer,
        HWND   hWnd,
        PCWSTR pszMonitorNameIn,
        PWSTR  *ppszPortNameOut
    );

    BOOL (WINAPI *pfnConfigurePortUI)
    (
        PCWSTR  pName,
        HWND    hWnd,
        PCWSTR  pPortName
    );

    BOOL (WINAPI *pfnDeletePortUI)
    (
        PCWSTR pszServer,
        HWND   hWnd,
        PCWSTR pszPortName
    );

} MONITORUI, FAR *PMONITORUI;


HANDLE
CreatePrinterIC(
    HANDLE  hPrinter,
    LPDEVMODEW  pDevMode
);

BOOL
PlayGdiScriptOnPrinterIC(
    HANDLE  hPrinterIC,
    LPBYTE  pIn,
    DWORD   cIn,
    LPBYTE  pOut,
    DWORD   cOut,
    DWORD   ul
);

BOOL
DeletePrinterIC(
    HANDLE  hPrinterIC
);

BOOL
DevQueryPrint(
    HANDLE      hPrinter,
    LPDEVMODE   pDevMode,
    DWORD      *pResID
);

HANDLE
RevertToPrinterSelf(
    VOID
);

BOOL
ImpersonatePrinterClient(
    HANDLE  hToken
);

BOOL
ReplyPrinterChangeNotification(
    HANDLE hNotify,
    DWORD fdwFlags,
    PDWORD pdwResult,
    PVOID pPrinterNotifyInfo
);

BOOL
PartialReplyPrinterChangeNotification(
    HANDLE hNotify,
    PPRINTER_NOTIFY_INFO_DATA pInfoDataSrc
);

PPRINTER_NOTIFY_INFO
RouterAllocPrinterNotifyInfo(
    DWORD cPrinterNotifyInfoData
);

BOOL
RouterFreePrinterNotifyInfo(
    PPRINTER_NOTIFY_INFO pInfo
);


PBIDI_RESPONSE_CONTAINER
RouterAllocBidiResponseContainer(
    DWORD Count
);

PVOID
RouterAllocBidiMem (
    IN size_t NumBytes
);

VOID
RouterFreeBidiMem (
    IN PVOID pMemPointer
);



#define PRINTER_NOTIFY_INFO_DATA_COMPACT 1

BOOL
AppendPrinterNotifyInfoData(
    PPRINTER_NOTIFY_INFO pInfoDest,
    PPRINTER_NOTIFY_INFO_DATA pInfoDataSrc,
    DWORD fdwFlags
);


DWORD
CallRouterFindFirstPrinterChangeNotification(
    HANDLE hPrinter,
    DWORD fdwFlags,
    DWORD fdwOptions,
    HANDLE hNotify,
    PVOID pvReserved);

BOOL
ProvidorFindFirstPrinterChangeNotification(
    HANDLE hPrinter,
    DWORD fdwFlags,
    DWORD fdwOptions,
    HANDLE hNotify,
    PVOID pvReserved0,
    PVOID pvReserved1);

BOOL
ProvidorFindClosePrinterChangeNotification(
    HANDLE hPrinter);



BOOL
SpoolerFindFirstPrinterChangeNotification(
    HANDLE hPrinter,
    DWORD fdwFlags,
    DWORD fdwOptions,
    PHANDLE phEvent,
    PVOID pPrinterNotifyOptions,
    PVOID pvReserved);

BOOL
SpoolerFindNextPrinterChangeNotification(
    HANDLE hPrinter,
    LPDWORD pfdwChange,
    PVOID pvReserved0,
    PVOID ppPrinterNotifyInfo);

VOID
SpoolerFreePrinterNotifyInfo(
    PPRINTER_NOTIFY_INFO pInfo);

BOOL
SpoolerFindClosePrinterChangeNotification(
    HANDLE hPrinter);

LPMONITOR2
WINAPI
InitializePrintMonitor2(
    PMONITORINIT pMonitorInit,
    PHANDLE phMonitor
);

BOOL
WINAPI
InitializeMonitorEx(
    LPWSTR      pRegistryRoot,
    LPMONITOR   pMonitor
);

LPMONITOREX
WINAPI
InitializePrintMonitor(
    LPWSTR      pRegistryRoot
);

PMONITORUI
WINAPI
InitializePrintMonitorUI(
    VOID
);


//
//  The following is added for new point-and-print support which allows
//  specific files to be associated with a print queue (instead of a printer
//  driver) using SetPrinterDataEx under the key "CopyFiles"
//
#define COPYFILE_EVENT_SET_PRINTER_DATAEX           1
#define COPYFILE_EVENT_DELETE_PRINTER               2
#define COPYFILE_EVENT_ADD_PRINTER_CONNECTION       3
#define COPYFILE_EVENT_DELETE_PRINTER_CONNECTION    4
#define COPYFILE_EVENT_FILES_CHANGED                5


BOOL
WINAPI
SpoolerCopyFileEvent(
    LPWSTR  pszPrinterName,
    LPWSTR  pszKey,
    DWORD   dwCopyFileEvent
    );

#define COPYFILE_FLAG_CLIENT_SPOOLER            0x00000001
#define COPYFILE_FLAG_SERVER_SPOOLER            0x00000002


DWORD
WINAPI
GenerateCopyFilePaths(
    IN      LPCWSTR     pszPrinterName,
    IN      LPCWSTR     pszDirectory,
    IN      LPBYTE      pSplClientInfo,
    IN      DWORD       dwLevel,
    IN OUT  LPWSTR      pszSourceDir,
    IN OUT  LPDWORD     pcchSourceDirSize,
    IN OUT  LPWSTR      pszTargetDir,
    IN OUT  LPDWORD     pcchTargetDirSize,
    IN      DWORD       dwFlags
    );


typedef enum {
    kMessageBox = 0    
} UI_TYPE;

typedef struct {
    DWORD       cbSize;     // sizeof(MESSAGEBOX_PARAMS)
    LPWSTR      pTitle;     // Pointer to a null-terminated string for the title bar of the message box.
    LPWSTR      pMessage;   // Pointer to a null-terminated string containing the message to display. 	
    DWORD       Style;      // Specifies the contents and behavior of the message box
    DWORD       dwTimeout;  // If bWait is TRUE, Timeout specifies the time, in seconds, that the function waits for the user's response.
    BOOL        bWait;      // If TRUE, SplPromptUIInUsersSession does not return until the user responds or the time-out interval elapses. 
                            // If Timeout is zero, SplPromptUIInUsersSession doesn't return until the user responds. 
                            // If FALSE, the function returns immediately and pResponse returns IDASYNC. 
 
} MESSAGEBOX_PARAMS, *PMESSAGEBOX_PARAMS;

typedef struct {

    UI_TYPE  UIType;

    union {

        MESSAGEBOX_PARAMS   MessageBoxParams;
    };
	
} SHOWUIPARAMS, *PSHOWUIPARAMS;

BOOL
SplPromptUIInUsersSession(
    IN  HANDLE          hPrinter,
    IN  DWORD           JobId,
    IN  PSHOWUIPARAMS   pUIParams,
    OUT DWORD           *pResponse
);

DWORD
SplIsSessionZero(
    IN  HANDLE  hPrinter,
    IN  DWORD   JobId,
    OUT BOOL    *pIsSessionZero
);

#ifdef __cplusplus
}                   /* End of extern "C" { */
#endif              /* __cplusplus */

#endif // _WINSPLP_



