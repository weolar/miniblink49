/*++

Copyright (c) 1996-1997 Microsoft Corporation

Module Name:

    prcomoem.h

Abstract:

    Interface declaration for Windows NT printer driver OEM plugins

--*/

#ifndef _PRCOMOEM_H_
#define _PRCOMOEM_H_

//
// This file has to be included after printoem.h. We also need to inlude
// objbase.h or comcat.h from sdk\inc.
//

//
// Each dll/exe must initialize the GUIDs once.If you are not using precompiled
// headers for the file(s) which initializes the GUIDs, define INITGUID before
// including objbase.h.
//

//
// Class ID for OEM rendering component. All OEM rendering plugin need to use this ID.
//
// {6d6abf26-9f38-11d1-882a-00c04fb961ec}
//

DEFINE_GUID(CLSID_OEMRENDER, 0x6d6abf26, 0x9f38, 0x11d1, 0x88, 0x2a, 0x00, 0xc0, 0x4f, 0xb9, 0x61, 0xec);

//
// Class ID for OEM UI component. All OEM UI plugin need to use this ID.
//
// {abce80d7-9f46-11d1-882a-00c04fb961ec}
//

DEFINE_GUID(CLSID_OEMUI, 0xabce80d7, 0x9f46, 0x11d1, 0x88, 0x2a, 0x00, 0xc0, 0x4f, 0xb9, 0x61, 0xec);

//
// Interface ID for IPrintOemCommon Interface
//
// {7f42285e-91d5-11d1-8820-00c04fb961ec}
//

DEFINE_GUID(IID_IPrintOemCommon, 0x7f42285e, 0x91d5, 0x11d1, 0x88, 0x20, 0x00, 0xc0, 0x4f, 0xb9, 0x61, 0xec);

//
// Interface ID for IPrintOemEngine Interface
//
// {63d17590-91d8-11d1-8820-00c04fb961ec}
//

DEFINE_GUID(IID_IPrintOemEngine, 0x63d17590, 0x91d8, 0x11d1, 0x88, 0x20, 0x00, 0xc0, 0x4f, 0xb9, 0x61, 0xec);

//
// Interface ID for IPrintOemUI Interface
//
// {C6A7A9D0-774C-11d1-947F-00A0C90640B8}
//

DEFINE_GUID(IID_IPrintOemUI, 0xc6a7a9d0, 0x774c, 0x11d1, 0x94, 0x7f, 0x0, 0xa0, 0xc9, 0x6, 0x40, 0xb8);

//
// Interface ID for IPrintOemUI2 Interface
//
// {292515F9-B54B-489B-9275-BAB56821395E}
//

DEFINE_GUID(IID_IPrintOemUI2, 0x292515f9, 0xb54b, 0x489b, 0x92, 0x75, 0xba, 0xb5, 0x68, 0x21, 0x39, 0x5e);

//
// Interface ID for IPrintOemDriverUI interface
//
// {92B05D50-78BC-11d1-9480-00A0C90640B8}
//

DEFINE_GUID(IID_IPrintOemDriverUI, 0x92b05d50, 0x78bc, 0x11d1, 0x94, 0x80, 0x0, 0xa0, 0xc9, 0x6, 0x40, 0xb8);

//
// Interface ID for IPrintCoreUI2 interface
//
// {085CCFCA-3ADF-4c9e-B491-D851A6EDC997}
//

DEFINE_GUID(IID_IPrintCoreUI2, 0x85ccfca, 0x3adf, 0x4c9e, 0xb4, 0x91, 0xd8, 0x51, 0xa6, 0xed, 0xc9, 0x97);

//
// Interface ID for IPrintOemPS Interface
//
// {688342b5-8e1a-11d1-881f-00c04fb961ec}
//

DEFINE_GUID(IID_IPrintOemPS, 0x688342b5, 0x8e1a, 0x11d1, 0x88, 0x1f, 0x00, 0xc0, 0x4f, 0xb9, 0x61, 0xec);

//
// Interface ID for IPrintOemPS2 Interface
//
// {BECF7F34-51B3-46c9-8A1C-18679BD21F36}
//

DEFINE_GUID(IID_IPrintOemPS2, 0xbecf7f34, 0x51b3, 0x46c9, 0x8a, 0x1c, 0x18, 0x67, 0x9b, 0xd2, 0x1f, 0x36);

//
// Interface ID for IPrintOemDriverPS interface
//
// {d90060c7-8e1a-11d1-881f-00c04fb961ec}
//

DEFINE_GUID(IID_IPrintOemDriverPS, 0xd90060c7, 0x8e1a, 0x11d1, 0x88, 0x1f, 0x00, 0xc0, 0x4f, 0xb9, 0x61, 0xec);

//
// Interface ID for IPrintCorePS2 interface
//
// {CDBB0B0B-A917-40d7-9FBF-483B3BE7EF22}

DEFINE_GUID(IID_IPrintCorePS2, 0xcdbb0b0b, 0xa917, 0x40d7, 0x9f, 0xbf, 0x48, 0x3b, 0x3b, 0xe7, 0xef, 0x22);

//
// Interface ID for IPrintOemUni Interface
//
// {D67EBBF0-78BF-11d1-9480-00A0C90640B8}
//

DEFINE_GUID(IID_IPrintOemUni, 0xd67ebbf0, 0x78bf, 0x11d1, 0x94, 0x80, 0x0, 0xa0, 0xc9, 0x6, 0x40, 0xb8);

//
// Interface ID for IPrintOemUni2 Interface
//
// {B91220AC-15CC-4e7a-A21E-9591F34D6F6C}
//

DEFINE_GUID(IID_IPrintOemUni2, 0xb91220ac, 0x15cc, 0x4e7a, 0xa2, 0x1e, 0x95, 0x91, 0xf3, 0x4d, 0x6f, 0x6c);

//
// Interface ID for IPrintOemDriverUni interface
//
// {D67EBBF1-78BF-11d1-9480-00A0C90640B8}
//

DEFINE_GUID(IID_IPrintOemDriverUni, 0xd67ebbf1, 0x78bf, 0x11d1, 0x94, 0x80, 0x0, 0xa0, 0xc9, 0x6, 0x40, 0xb8);

#undef IUnknown

#ifdef __cplusplus
extern "C" {
#endif

//
//****************************************************************************
//  IPrintOemCommon interface
//****************************************************************************
//

#undef INTERFACE
#define INTERFACE IPrintOemCommon
DECLARE_INTERFACE_(IPrintOemCommon, IUnknown)
{
    //
    // IUnknown methods
    //

    STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID* ppvObj) PURE;
    STDMETHOD_(ULONG, AddRef) (THIS) PURE;
    STDMETHOD_(ULONG, Release) (THIS) PURE;

    //
    // IPrintOemCommon methods
    //

    //
    // Method for getting OEM related information
    //

    STDMETHOD(GetInfo) (THIS_   DWORD   dwMode,
                                PVOID   pBuffer,
                                DWORD   cbSize,
                                PDWORD  pcbNeeded) PURE;
    //
    // Method for OEM private devmode handling
    //

    STDMETHOD(DevMode) (THIS_   DWORD       dwMode,
                                POEMDMPARAM pOemDMParam) PURE;
};

#ifndef KERNEL_MODE

//
// Definitions used by user interface module only.
// Make sure the macro KERNEL_MODE is not defined.
//

//
//****************************************************************************
//  IPrintOemUI interface
//****************************************************************************
//

#undef INTERFACE
#define INTERFACE IPrintOemUI
DECLARE_INTERFACE_(IPrintOemUI, IPrintOemCommon)
{
    //
    // IUnknown methods
    //

    STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID* ppvObj)PURE;
    STDMETHOD_(ULONG, AddRef) (THIS)PURE;
    STDMETHOD_(ULONG, Release) (THIS)PURE;

    //
    // IPrintOemCommon methods
    //

    //
    // Method for getting OEM related information
    //

    STDMETHOD(GetInfo) (THIS_   DWORD   dwMode,
                                PVOID   pBuffer,
                                DWORD   cbSize,
                                PDWORD  pcbNeeded) PURE;
    //
    // Method for OEM private devmode handling
    //

    STDMETHOD(DevMode) (THIS_   DWORD       dwMode,
                                POEMDMPARAM pOemDMParam) PURE;

    //
    // IPrintOemUI methods
    //

    //
    // Method for publishing Driver interface.
    //

    STDMETHOD(PublishDriverInterface) (THIS_ IUnknown *pIUnknown) PURE;


    //
    // CommonUIProp
    //

    STDMETHOD(CommonUIProp) (THIS_
            DWORD  dwMode,
            POEMCUIPPARAM   pOemCUIPParam
            )PURE;

    //
    // DocumentPropertySheets
    //

    STDMETHOD(DocumentPropertySheets) (THIS_
            PPROPSHEETUI_INFO   pPSUIInfo,
            LPARAM              lParam
            )PURE;

    //
    // DevicePropertySheets
    //

    STDMETHOD(DevicePropertySheets) (THIS_
            PPROPSHEETUI_INFO   pPSUIInfo,
            LPARAM              lParam
            )PURE;


    //
    // DevQueryPrintEx
    //

    STDMETHOD(DevQueryPrintEx) (THIS_
            POEMUIOBJ               poemuiobj,
            PDEVQUERYPRINT_INFO     pDQPInfo,
            PDEVMODE                pPublicDM,
            PVOID                   pOEMDM
            )PURE;

    //
    // DeviceCapabilities
    //

    STDMETHOD(DeviceCapabilities) (THIS_
            POEMUIOBJ   poemuiobj,
            HANDLE      hPrinter,
            PWSTR       pDeviceName,
            WORD        wCapability,
            PVOID       pOutput,
            PDEVMODE    pPublicDM,
            PVOID       pOEMDM,
            DWORD       dwOld,
            DWORD       *dwResult
            )PURE;

    //
    // UpgradePrinter
    //

    STDMETHOD(UpgradePrinter) (THIS_
            DWORD   dwLevel,
            PBYTE   pDriverUpgradeInfo
            )PURE;

    //
    // PrinterEvent
    //

    STDMETHOD(PrinterEvent) (THIS_
            PWSTR   pPrinterName,
            INT     iDriverEvent,
            DWORD   dwFlags,
            LPARAM  lParam
            )PURE;

    //
    // DriverEvent
    //

    STDMETHOD(DriverEvent) (THIS_
            DWORD   dwDriverEvent,
            DWORD   dwLevel,
            LPBYTE  pDriverInfo,
            LPARAM  lParam
            )PURE;

    //
    // QueryColorProfile
    //

    STDMETHOD(QueryColorProfile) (THIS_
            HANDLE      hPrinter,
            POEMUIOBJ   poemuiobj,
            PDEVMODE    pPublicDM,
            PVOID       pOEMDM,
            ULONG       ulQueryMode,
            VOID       *pvProfileData,
            ULONG      *pcbProfileData,
            FLONG      *pflProfileData
            )PURE;

    //
    // FontInstallerDlgProc
    //

    STDMETHOD(FontInstallerDlgProc) (THIS_
            HWND    hWnd,
            UINT    usMsg,
            WPARAM  wParam,
            LPARAM  lParam
            )PURE;

    //
    // UpdateExternalFonts
    //

    STDMETHOD(UpdateExternalFonts) (THIS_
            HANDLE  hPrinter,
            HANDLE  hHeap,
            PWSTR   pwstrCartridges
           )PURE;
};


//
//****************************************************************************
//  IPrintOemUI2 interface
//****************************************************************************
//

#undef INTERFACE
#define INTERFACE IPrintOemUI2
DECLARE_INTERFACE_(IPrintOemUI2, IPrintOemUI)
{
    //
    // IUnknown methods
    //

    STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID* ppvObj)PURE;
    STDMETHOD_(ULONG, AddRef) (THIS)PURE;
    STDMETHOD_(ULONG, Release) (THIS)PURE;

    //
    // IPrintOemCommon methods
    //

    //
    // Method for getting OEM related information
    //

    STDMETHOD(GetInfo) (THIS_   DWORD   dwMode,
                                PVOID   pBuffer,
                                DWORD   cbSize,
                                PDWORD  pcbNeeded) PURE;
    //
    // Method for OEM private devmode handling
    //

    STDMETHOD(DevMode) (THIS_   DWORD       dwMode,
                                POEMDMPARAM pOemDMParam) PURE;

    //
    // IPrintOemUI methods
    //

    //
    // Method for publishing Driver interface.
    //

    STDMETHOD(PublishDriverInterface) (THIS_ IUnknown *pIUnknown) PURE;


    //
    // CommonUIProp
    //

    STDMETHOD(CommonUIProp) (THIS_
            DWORD  dwMode,
            POEMCUIPPARAM   pOemCUIPParam
            )PURE;

    //
    // DocumentPropertySheets
    //

    STDMETHOD(DocumentPropertySheets) (THIS_
            PPROPSHEETUI_INFO   pPSUIInfo,
            LPARAM              lParam
            )PURE;

    //
    // DevicePropertySheets
    //

    STDMETHOD(DevicePropertySheets) (THIS_
            PPROPSHEETUI_INFO   pPSUIInfo,
            LPARAM              lParam
            )PURE;


    //
    // DevQueryPrintEx
    //

    STDMETHOD(DevQueryPrintEx) (THIS_
            POEMUIOBJ               poemuiobj,
            PDEVQUERYPRINT_INFO     pDQPInfo,
            PDEVMODE                pPublicDM,
            PVOID                   pOEMDM
            )PURE;

    //
    // DeviceCapabilities
    //

    STDMETHOD(DeviceCapabilities) (THIS_
            POEMUIOBJ   poemuiobj,
            HANDLE      hPrinter,
            PWSTR       pDeviceName,
            WORD        wCapability,
            PVOID       pOutput,
            PDEVMODE    pPublicDM,
            PVOID       pOEMDM,
            DWORD       dwOld,
            DWORD       *dwResult
            )PURE;

    //
    // UpgradePrinter
    //

    STDMETHOD(UpgradePrinter) (THIS_
            DWORD   dwLevel,
            PBYTE   pDriverUpgradeInfo
            )PURE;

    //
    // PrinterEvent
    //

    STDMETHOD(PrinterEvent) (THIS_
            PWSTR   pPrinterName,
            INT     iDriverEvent,
            DWORD   dwFlags,
            LPARAM  lParam
            )PURE;

    //
    // DriverEvent
    //

    STDMETHOD(DriverEvent) (THIS_
            DWORD   dwDriverEvent,
            DWORD   dwLevel,
            LPBYTE  pDriverInfo,
            LPARAM  lParam
            )PURE;

    //
    // QueryColorProfile
    //

    STDMETHOD(QueryColorProfile) (THIS_
            HANDLE      hPrinter,
            POEMUIOBJ   poemuiobj,
            PDEVMODE    pPublicDM,
            PVOID       pOEMDM,
            ULONG       ulQueryMode,
            VOID       *pvProfileData,
            ULONG      *pcbProfileData,
            FLONG      *pflProfileData
            )PURE;

    //
    // FontInstallerDlgProc
    //

    STDMETHOD(FontInstallerDlgProc) (THIS_
            HWND    hWnd,
            UINT    usMsg,
            WPARAM  wParam,
            LPARAM  lParam
            )PURE;

    //
    // UpdateExternalFonts
    //

    STDMETHOD(UpdateExternalFonts) (THIS_
            HANDLE  hPrinter,
            HANDLE  hHeap,
            PWSTR   pwstrCartridges
           )PURE;

    //
    // IPrintOemUI2 methods
    //

    //
    // QueryJobAttributes
    //

    STDMETHOD(QueryJobAttributes)  (THIS_
            HANDLE      hPrinter,
            PDEVMODE    pDevmode,
            DWORD       dwLevel,
            LPBYTE      lpAttributeInfo
           )PURE;

    //
    // Hide Standard UI
    //

    STDMETHOD(HideStandardUI)  (THIS_
            DWORD       dwMode
           )PURE;

    //
    // DocumentEvent
    //

    STDMETHOD(DocumentEvent) (THIS_
            HANDLE      hPrinter,
            HDC         hdc,
            INT         iEsc,
            ULONG       cbIn,
            PVOID       pvIn,
            ULONG       cbOut,
            PVOID       pvOut,
            PINT        piResult
           )PURE;
};


//
//****************************************************************************
//  IPrintOemDriverUI interface
//****************************************************************************
//

#undef INTERFACE
#define INTERFACE IPrintOemDriverUI
DECLARE_INTERFACE_(IPrintOemDriverUI, IUnknown)
{
    //
    // IUnknown methods
    //

    STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID* ppvObj)PURE;
    STDMETHOD_(ULONG, AddRef) (THIS)PURE;
    STDMETHOD_(ULONG, Release) (THIS)PURE;

    //
    // IPrintOemDriverUI methods
    //

    //
    // Helper function to get driver settings
    //

    STDMETHOD(DrvGetDriverSetting) (THIS_
                        PVOID   pci,
                        PCSTR   Feature,
                        PVOID   pOutput,
                        DWORD   cbSize,
                        PDWORD  pcbNeeded,
                        PDWORD  pdwOptionsReturned
                        )PURE;

    //
    // Helper function to allow OEM plugins upgrade private registry
    // settings. This function should be called only by OEM's UpgradePrinter()
    //

    STDMETHOD(DrvUpgradeRegistrySetting) (THIS_
                        HANDLE   hPrinter,
                        PCSTR    pFeature,
                        PCSTR    pOption
                        )PURE;

    //
    // Helper function to allow OEM plugins to update the driver UI
    // settings and show constraints. This function should be called only when
    // the UI is present.
    //

    STDMETHOD(DrvUpdateUISetting) (THIS_
                        PVOID    pci,
                        PVOID    pOptItem,
                        DWORD    dwPreviousSelection,
                        DWORD    dwMode
                        )PURE;
};

//
//****************************************************************************
//  IPrintCoreUI2 interface
//
//  This is the revised core driver helper interface OEM UI plugin can call.
//  It supercedes the old helper interface IPrintOemDriverUI.
//****************************************************************************
//

#undef INTERFACE
#define INTERFACE IPrintCoreUI2
DECLARE_INTERFACE_(IPrintCoreUI2, IPrintOemDriverUI)
{
    //
    // IUnknown methods
    //

    STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID* ppvObj)PURE;
    STDMETHOD_(ULONG, AddRef) (THIS)PURE;
    STDMETHOD_(ULONG, Release) (THIS)PURE;

    //
    // IPrintOemDriverUI methods
    //

    //
    // Helper function to get driver settings. This function is only supported
    // for UI plugins that do not fully replace core driver's standard UI.
    //

    STDMETHOD(DrvGetDriverSetting) (THIS_
                        PVOID   pci,
                        PCSTR   Feature,
                        PVOID   pOutput,
                        DWORD   cbSize,
                        PDWORD  pcbNeeded,
                        PDWORD  pdwOptionsReturned
                        )PURE;

    //
    // Helper function to allow OEM plugins upgrade private registry
    // settings. This function is supported for any UI plugins and should be
    // called only by OEM's UpgradePrinter.
    //

    STDMETHOD(DrvUpgradeRegistrySetting) (THIS_
                        HANDLE   hPrinter,
                        PCSTR    pFeature,
                        PCSTR    pOption
                        )PURE;

    //
    // Helper function to allow OEM plugins to update the driver UI settings.
    // This function is only supported for UI plugins that do not fully replace
    // core driver's standard UI. It should be called only when the UI is present.
    //

    STDMETHOD(DrvUpdateUISetting) (THIS_
                        PVOID    pci,
                        PVOID    pOptItem,
                        DWORD    dwPreviousSelection,
                        DWORD    dwMode
                        )PURE;

    //
    // IPrintCoreUI2 new methods
    //

    //
    // Following four helper functions are only supported for UI plugins that fully
    // replace core driver's standard UI. They should only be called by the UI plugin's
    // DocumentPropertySheets, DevicePropertySheets and their property sheet callback
    // functions.
    //
    // Helper function to retrieve driver's current setting as a list of
    // feature/option keyword pairs.
    //

    STDMETHOD(GetOptions) (THIS_
                           IN  POEMUIOBJ  poemuiobj,
                           IN  DWORD      dwFlags,
                           IN  PCSTR      pmszFeaturesRequested,
                           IN  DWORD      cbIn,
                           OUT PSTR       pmszFeatureOptionBuf,
                           IN  DWORD      cbSize,
                           OUT PDWORD     pcbNeeded) PURE;

    //
    // Helper function to change driver's setting using a list of feature/option
    // keyword pairs.
    //

    STDMETHOD(SetOptions) (THIS_
                           IN  POEMUIOBJ  poemuiobj,
                           IN  DWORD      dwFlags,
                           IN  PCSTR      pmszFeatureOptionBuf,
                           IN  DWORD      cbIn,
                           OUT PDWORD     pdwResult) PURE;

    //
    // Helper function to retrieve the option(s) of a given feature that are
    // constrained in driver's current setting.
    //

    STDMETHOD(EnumConstrainedOptions) (THIS_
                                       IN  POEMUIOBJ  poemuiobj,
                                       IN  DWORD      dwFlags,
                                       IN  PCSTR      pszFeatureKeyword,
                                       OUT PSTR       pmszConstrainedOptionList,
                                       IN  DWORD      cbSize,
                                       OUT PDWORD     pcbNeeded) PURE;

    //
    // Helper function to retrieve a list of feature/option keyword pairs from
    // driver's current setting that conflict with the given feature/option pair.
    //

    STDMETHOD(WhyConstrained) (THIS_
                               IN  POEMUIOBJ  poemuiobj,
                               IN  DWORD      dwFlags,
                               IN  PCSTR      pszFeatureKeyword,
                               IN  PCSTR      pszOptionKeyword,
                               OUT PSTR       pmszReasonList,
                               IN  DWORD      cbSize,
                               OUT PDWORD     pcbNeeded) PURE;

    //
    // Following five helper functions are supported for any UI plugins.
    //
    // Helper function to retrieve global attribute.
    //

    STDMETHOD(GetGlobalAttribute) (THIS_
                                   IN  POEMUIOBJ  poemuiobj,
                                   IN  DWORD      dwFlags,
                                   IN  PCSTR      pszAttribute,
                                   OUT PDWORD     pdwDataType,
                                   OUT PBYTE      pbData,
                                   IN  DWORD      cbSize,
                                   OUT PDWORD     pcbNeeded) PURE;


    //
    // Helper function to retrieve attribute of a given feature.
    //

    STDMETHOD(GetFeatureAttribute) (THIS_
                                    IN  POEMUIOBJ  poemuiobj,
                                    IN  DWORD      dwFlags,
                                    IN  PCSTR      pszFeatureKeyword,
                                    IN  PCSTR      pszAttribute,
                                    OUT PDWORD     pdwDataType,
                                    OUT PBYTE      pbData,
                                    IN  DWORD      cbSize,
                                    OUT PDWORD     pcbNeeded) PURE;

    //
    // Helper function to retrieve attribute of a given feature/option selection.
    //

    STDMETHOD(GetOptionAttribute) (THIS_
                                   IN  POEMUIOBJ  poemuiobj,
                                   IN  DWORD      dwFlags,
                                   IN  PCSTR      pszFeatureKeyword,
                                   IN  PCSTR      pszOptionKeyword,
                                   IN  PCSTR      pszAttribute,
                                   OUT PDWORD     pdwDataType,
                                   OUT PBYTE      pbData,
                                   IN  DWORD      cbSize,
                                   OUT PDWORD     pcbNeeded) PURE;

    //
    // Helper function to retrieve the list of feature keyword.
    //

    STDMETHOD(EnumFeatures) (THIS_
                             IN  POEMUIOBJ  poemuiobj,
                             IN  DWORD      dwFlags,
                             OUT PSTR       pmszFeatureList,
                             IN  DWORD      cbSize,
                             OUT PDWORD     pcbNeeded) PURE;

    //
    // Helper function to retrieve the list of options keyword of a given feature.
    //

    STDMETHOD(EnumOptions) (THIS_
                            IN  POEMUIOBJ  poemuiobj,
                            IN  DWORD      dwFlags,
                            IN  PCSTR      pszFeatureKeyword,
                            OUT PSTR       pmszOptionList,
                            IN  DWORD      cbSize,
                            OUT PDWORD     pcbNeeded) PURE;

    //
    // Helper function to query system simulation support
    //

    STDMETHOD(QuerySimulationSupport) (THIS_
                                       IN  HANDLE  hPrinter,
                                       IN  DWORD   dwLevel,
                                       OUT PBYTE   pCaps,
                                       IN  DWORD   cbSize,
                                       OUT PDWORD  pcbNeeded) PURE;
};

#else   // KERNEL_MODE

//
// Definitions used by rendering module only.
// Make sure the macro KERNEL_MODE is defined.
//

//
//****************************************************************************
//  IPrintOemEngine interface
//****************************************************************************
//

#undef INTERFACE
#define INTERFACE IPrintOemEngine
DECLARE_INTERFACE_(IPrintOemEngine, IPrintOemCommon)
{
    //
    // IUnknown methods
    //

    STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID* ppvObj) PURE;
    STDMETHOD_(ULONG, AddRef) (THIS) PURE;
    STDMETHOD_(ULONG, Release) (THIS) PURE;

    //
    // IPrintOemCommon methods
    //

    //
    // Method for getting OEM related information
    //

    STDMETHOD(GetInfo) (THIS_   DWORD   dwMode,
                                PVOID   pBuffer,
                                DWORD   cbSize,
                                PDWORD  pcbNeeded) PURE;
    //
    // Method for OEM private devmode handling
    //

    STDMETHOD(DevMode) (THIS_   DWORD       dwMode,
                                POEMDMPARAM pOemDMParam) PURE;

    //
    // IPrintOemEngine methods
    //

    //
    // Method for OEM to specify DDI hook out
    //

    STDMETHOD(EnableDriver)  (THIS_   DWORD           DriverVersion,
                                      DWORD           cbSize,
                                      PDRVENABLEDATA  pded) PURE;

    //
    // Method to notify OEM plugin that it is no longer required
    //

    STDMETHOD(DisableDriver) (THIS) PURE;

    //
    // Method for OEM to contruct its own PDEV
    //

    STDMETHOD(EnablePDEV)    (THIS_   PDEVOBJ         pdevobj,
                                      PWSTR           pPrinterName,
                                      ULONG           cPatterns,
                                      HSURF          *phsurfPatterns,
                                      ULONG           cjGdiInfo,
                                      GDIINFO        *pGdiInfo,
                                      ULONG           cjDevInfo,
                                      DEVINFO        *pDevInfo,
                                      DRVENABLEDATA  *pded,
                                      OUT PDEVOEM    *pDevOem) PURE;

    //
    // Method for OEM to free any resource associated with its PDEV
    //

    STDMETHOD(DisablePDEV)   (THIS_   PDEVOBJ         pdevobj) PURE;

    //
    // Method for OEM to transfer from old PDEV to new PDEV
    //

    STDMETHOD(ResetPDEV)     (THIS_   PDEVOBJ         pdevobjOld,
                                      PDEVOBJ         pdevobjNew) PURE;
};

//
//****************************************************************************
//  IPrintOemPS interface
//****************************************************************************
//

#undef INTERFACE
#define INTERFACE IPrintOemPS
DECLARE_INTERFACE_(IPrintOemPS, IPrintOemEngine)
{
    //
    // IUnknown methods
    //

    STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID* ppvObj) PURE;
    STDMETHOD_(ULONG, AddRef) (THIS) PURE;
    STDMETHOD_(ULONG, Release) (THIS) PURE;

    //
    // IPrintOemCommon methods
    //

    //
    // Method for getting OEM related information
    //

    STDMETHOD(GetInfo) (THIS_   DWORD   dwMode,
                                PVOID   pBuffer,
                                DWORD   cbSize,
                                PDWORD  pcbNeeded) PURE;
    //
    // Method for OEM private devmode handling
    //

    STDMETHOD(DevMode) (THIS_   DWORD       dwMode,
                                POEMDMPARAM pOemDMParam) PURE;

    //
    // IPrintOemEngine methods
    //

    //
    // Method for OEM to specify DDI hook out
    //

    STDMETHOD(EnableDriver)  (THIS_   DWORD           DriverVersion,
                                      DWORD           cbSize,
                                      PDRVENABLEDATA  pded) PURE;

    //
    // Method to notify OEM plugin that it is no longer required
    //

    STDMETHOD(DisableDriver) (THIS) PURE;

    //
    // Method for OEM to construct its own PDEV
    //

    STDMETHOD(EnablePDEV)    (THIS_   PDEVOBJ         pdevobj,
                                      PWSTR           pPrinterName,
                                      ULONG           cPatterns,
                                      HSURF          *phsurfPatterns,
                                      ULONG           cjGdiInfo,
                                      GDIINFO        *pGdiInfo,
                                      ULONG           cjDevInfo,
                                      DEVINFO        *pDevInfo,
                                      DRVENABLEDATA  *pded,
                                      OUT PDEVOEM    *pDevOem) PURE;

    //
    // Method for OEM to free any resource associated with its PDEV
    //

    STDMETHOD(DisablePDEV)   (THIS_   PDEVOBJ         pdevobj) PURE;

    //
    // Method for OEM to transfer from old PDEV to new PDEV
    //

    STDMETHOD(ResetPDEV)     (THIS_   PDEVOBJ         pdevobjOld,
                                      PDEVOBJ         pdevobjNew) PURE;

    //
    // IPrintOemPS methods
    //

    //
    // Method for publishing Driver interface.
    //

    STDMETHOD(PublishDriverInterface)(THIS_  IUnknown *pIUnknown) PURE;

    //
    // Method for OEM to generate output at specific injection point
    //

    STDMETHOD(Command) (THIS_   PDEVOBJ     pdevobj,
                                DWORD       dwIndex,
                                PVOID       pData,
                                DWORD       cbSize,
                                OUT DWORD   *pdwResult) PURE;
};

//
//****************************************************************************
//  IPrintOemPS2 interface
//****************************************************************************
//

#undef INTERFACE
#define INTERFACE IPrintOemPS2
DECLARE_INTERFACE_(IPrintOemPS2, IPrintOemPS)
{
    //
    // IUnknown methods
    //

    STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID* ppvObj) PURE;
    STDMETHOD_(ULONG, AddRef) (THIS) PURE;
    STDMETHOD_(ULONG, Release) (THIS) PURE;

    //
    // IPrintOemCommon methods
    //

    //
    // Method for getting OEM related information
    //

    STDMETHOD(GetInfo) (THIS_   DWORD   dwMode,
                                PVOID   pBuffer,
                                DWORD   cbSize,
                                PDWORD  pcbNeeded) PURE;
    //
    // Method for OEM private devmode handling
    //

    STDMETHOD(DevMode) (THIS_   DWORD       dwMode,
                                POEMDMPARAM pOemDMParam) PURE;

    //
    // IPrintOemEngine methods
    //

    //
    // Method for OEM to specify DDI hook out
    //

    STDMETHOD(EnableDriver)  (THIS_   DWORD           DriverVersion,
                                      DWORD           cbSize,
                                      PDRVENABLEDATA  pded) PURE;

    //
    // Method to notify OEM plugin that it is no longer required
    //

    STDMETHOD(DisableDriver) (THIS) PURE;

    //
    // Method for OEM to construct its own PDEV
    //

    STDMETHOD(EnablePDEV)    (THIS_   PDEVOBJ         pdevobj,
                                      PWSTR           pPrinterName,
                                      ULONG           cPatterns,
                                      HSURF          *phsurfPatterns,
                                      ULONG           cjGdiInfo,
                                      GDIINFO        *pGdiInfo,
                                      ULONG           cjDevInfo,
                                      DEVINFO        *pDevInfo,
                                      DRVENABLEDATA  *pded,
                                      OUT PDEVOEM    *pDevOem) PURE;

    //
    // Method for OEM to free any resource associated with its PDEV
    //

    STDMETHOD(DisablePDEV)   (THIS_   PDEVOBJ         pdevobj) PURE;

    //
    // Method for OEM to transfer from old PDEV to new PDEV
    //

    STDMETHOD(ResetPDEV)     (THIS_   PDEVOBJ         pdevobjOld,
                                      PDEVOBJ         pdevobjNew) PURE;

    //
    // IPrintOemPS methods
    //

    //
    // Method for publishing Driver interface.
    //

    STDMETHOD(PublishDriverInterface)(THIS_  IUnknown *pIUnknown) PURE;

    //
    // Method for OEM to generate output at specific injection point
    //

    STDMETHOD(Command) (THIS_   PDEVOBJ     pdevobj,
                                DWORD       dwIndex,
                                PVOID       pData,
                                DWORD       cbSize,
                                OUT DWORD   *pdwResult) PURE;

    //
    // IPrintOemPS2 methods
    //

    //
    // Method for plugin to hook out spooler's WritePrinter API so it
    // can get access to output data PostScript driver is generating
    //
    // At DrvEnablePDEV time, PostScript driver will call this function with
    // pdevobj = NULL, pBuf = NULL, cbBuffer = 0 to detect if the plugin
    // implements this function. Plugin should return S_OK to indicate it is
    // implementing this function, or return E_NOTIMPL otherwise.
    //
    // In pcbWritten, plugins should return the number of bytes written to the
    // spooler's WritePrinter function. Zero doesn't carry a special meaning,
    // errors must be reported through the returned HRESULT.
    //

    STDMETHOD(WritePrinter) (THIS_   PDEVOBJ    pdevobj,
                                     PVOID      pBuf,
                                     DWORD      cbBuffer,
                                     PDWORD     pcbWritten) PURE;

    //
    // Method for plugin to implement if it wants to be called to get the chance
    // to override some PDEV settings such as paper margins.
    // Plugins that recognize the adjustment type should return S_OK.
    // If the adjustment type is unrecognized, they should return S_FALSE
    // and not E_NOTIMPL, this code should be reserved for the COM meaning.
    // If the plugin fails the call, it should return E_FAIL.
    // The chain of plugins will be called until a plugin returns S_OK or
    // any failure code other than E_NOTIMPL, in other words, until the first
    // plugin that is designed to handle the adjustment is found.
    //

    STDMETHOD(GetPDEVAdjustment) (THIS_ PDEVOBJ    pdevobj,
                                        DWORD      dwAdjustType,
                                        PVOID      pBuf,
                                        DWORD      cbBuffer,
                                        OUT BOOL  *pbAdjustmentDone) PURE;
};

//
//****************************************************************************
//  IPrintOemDriverPS interface
//****************************************************************************
//

#undef INTERFACE
#define INTERFACE IPrintOemDriverPS
DECLARE_INTERFACE_(IPrintOemDriverPS, IUnknown)
{
    //
    // IUnknown methods
    //

    STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID* ppvObj) PURE;
    STDMETHOD_(ULONG, AddRef) (THIS) PURE;
    STDMETHOD_(ULONG, Release) (THIS) PURE;

    //
    // IPrintOemDriverPS methods
    //

    //
    // Method for OEM to get driver settings
    //

    STDMETHOD(DrvGetDriverSetting) (THIS_   PVOID   pdriverobj,
                                            PCSTR   Feature,
                                            PVOID   pOutput,
                                            DWORD   cbSize,
                                            PDWORD  pcbNeeded,
                                            PDWORD  pdwOptionsReturned) PURE;

    //
    // Method for OEM to write to spooler buffer
    //

    STDMETHOD(DrvWriteSpoolBuf)(THIS_       PDEVOBJ     pdevobj,
                                            PVOID       pBuffer,
                                            DWORD       cbSize,
                                            OUT DWORD   *pdwResult) PURE;
};

//
//****************************************************************************
//  IPrintCorePS2 interface
//
//  This is the revised core driver helper interface OEM PS render plugin can
//  call. It supercedes the old helper interface IPrintOemDriverPS.
//****************************************************************************
//

#undef INTERFACE
#define INTERFACE IPrintCorePS2
DECLARE_INTERFACE_(IPrintCorePS2, IUnknown)
{
    //
    // IUnknown methods
    //

    STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID* ppvObj) PURE;
    STDMETHOD_(ULONG, AddRef) (THIS) PURE;
    STDMETHOD_(ULONG, Release) (THIS) PURE;

    //
    // IPrintCorePS2 methods
    //

    //
    // Method for OEM to write to spooler buffer.
    //

    STDMETHOD(DrvWriteSpoolBuf)(THIS_
                           IN  PDEVOBJ  pdevobj,
                           IN  PVOID    pBuffer,
                           IN  DWORD    cbSize,
                           OUT DWORD    *pdwResult) PURE;

    //
    // Helper function to retrieve driver's current setting as a list of
    // feature/option keyword pairs.
    //

    STDMETHOD(GetOptions) (THIS_
                           IN  PDEVOBJ  pdevobj,
                           IN  DWORD    dwFlags,
                           IN  PCSTR    pmszFeaturesRequested,
                           IN  DWORD    cbIn,
                           OUT PSTR     pmszFeatureOptionBuf,
                           IN  DWORD    cbSize,
                           OUT PDWORD   pcbNeeded) PURE;

    //
    // Helper function to retrieve global attribute.
    //

    STDMETHOD(GetGlobalAttribute) (THIS_
                                   IN  PDEVOBJ  pdevobj,
                                   IN  DWORD    dwFlags,
                                   IN  PCSTR    pszAttribute,
                                   OUT PDWORD   pdwDataType,
                                   OUT PBYTE    pbData,
                                   IN  DWORD    cbSize,
                                   OUT PDWORD   pcbNeeded) PURE;


    //
    // Helper function to retrieve attribute of a given feature.
    //

    STDMETHOD(GetFeatureAttribute) (THIS_
                                    IN  PDEVOBJ  pdevobj,
                                    IN  DWORD    dwFlags,
                                    IN  PCSTR    pszFeatureKeyword,
                                    IN  PCSTR    pszAttribute,
                                    OUT PDWORD   pdwDataType,
                                    OUT PBYTE    pbData,
                                    IN  DWORD    cbSize,
                                    OUT PDWORD   pcbNeeded) PURE;

    //
    // Helper function to retrieve attribute of a given feature/option selection.
    //

    STDMETHOD(GetOptionAttribute) (THIS_
                                   IN  PDEVOBJ  pdevobj,
                                   IN  DWORD    dwFlags,
                                   IN  PCSTR    pszFeatureKeyword,
                                   IN  PCSTR    pszOptionKeyword,
                                   IN  PCSTR    pszAttribute,
                                   OUT PDWORD   pdwDataType,
                                   OUT PBYTE    pbData,
                                   IN  DWORD    cbSize,
                                   OUT PDWORD   pcbNeeded) PURE;

    //
    // Helper function to retrieve the list of feature keyword.
    //

    STDMETHOD(EnumFeatures) (THIS_
                             IN  PDEVOBJ  pdevobj,
                             IN  DWORD    dwFlags,
                             OUT PSTR     pmszFeatureList,
                             IN  DWORD    cbSize,
                             OUT PDWORD   pcbNeeded) PURE;

    //
    // Helper function to retrieve the list of options keyword of a given feature.
    //

    STDMETHOD(EnumOptions) (THIS_
                            IN  PDEVOBJ  pdevobj,
                            IN  DWORD    dwFlags,
                            IN  PCSTR    pszFeatureKeyword,
                            OUT PSTR     pmszOptionList,
                            IN  DWORD    cbSize,
                            OUT PDWORD   pcbNeeded) PURE;
};

//
//****************************************************************************
//  IPrintOemUni interface
//****************************************************************************
//

#undef INTERFACE
#define INTERFACE IPrintOemUni
DECLARE_INTERFACE_(IPrintOemUni, IPrintOemEngine)
{
    //
    // IUnknown methods
    //

    STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID* ppvObj) PURE;
    STDMETHOD_(ULONG, AddRef) (THIS) PURE;
    STDMETHOD_(ULONG, Release) (THIS) PURE;

    //
    // IPrintOemCommon methods
    //

    //
    // Method for getting OEM related information
    //

    STDMETHOD(GetInfo) (THIS_   DWORD   dwMode,
                                PVOID   pBuffer,
                                DWORD   cbSize,
                                PDWORD  pcbNeeded) PURE;
    //
    // Method for OEM private devmode handling
    //

    STDMETHOD(DevMode) (THIS_   DWORD       dwMode,
                                POEMDMPARAM pOemDMParam) PURE;

    //
    // IPrintOemEngine methods
    //

    //
    // Method for OEM to specify DDI hook out
    //

    STDMETHOD(EnableDriver)  (THIS_   DWORD           DriverVersion,
                                      DWORD           cbSize,
                                      PDRVENABLEDATA  pded) PURE;

    //
    // Method to notify OEM plugin that it is no longer required
    //

    STDMETHOD(DisableDriver) (THIS) PURE;

    //
    // Method for OEM to construct its own PDEV
    //

    STDMETHOD(EnablePDEV)    (THIS_   PDEVOBJ         pdevobj,
                                      PWSTR           pPrinterName,
                                      ULONG           cPatterns,
                                      HSURF          *phsurfPatterns,
                                      ULONG           cjGdiInfo,
                                      GDIINFO        *pGdiInfo,
                                      ULONG           cjDevInfo,
                                      DEVINFO        *pDevInfo,
                                      DRVENABLEDATA  *pded,
                                      OUT PDEVOEM    *pDevOem) PURE;

    //
    // Method for OEM to free any resource associated with its PDEV
    //

    STDMETHOD(DisablePDEV)   (THIS_   PDEVOBJ         pdevobj) PURE;

    //
    // Method for OEM to transfer from old PDEV to new PDEV
    //

    STDMETHOD(ResetPDEV)     (THIS_   PDEVOBJ         pdevobjOld,
                                      PDEVOBJ         pdevobjNew) PURE;

    //
    // IPrintOemUni methods
    //

    //
    // Method for publishing Driver interface.
    //

    STDMETHOD(PublishDriverInterface)(THIS_ IUnknown *pIUnknown) PURE;

    //
    // Method for getting OEM implemented methods.
    // Returns S_OK if the given method is implemented.
    // Returns S_FALSE if the given method is not implemented.
    //
    //

    STDMETHOD(GetImplementedMethod) (THIS_  PSTR    pMethodName) PURE;

    //
    // DriverDMS
    //

    STDMETHOD(DriverDMS)(THIS_  PVOID   pDevObj,
                                PVOID   pBuffer,
                                DWORD   cbSize,
                                PDWORD  pcbNeeded) PURE;

    //
    // CommandCallback
    //

    STDMETHOD(CommandCallback)(THIS_    PDEVOBJ     pdevobj,
                                        DWORD       dwCallbackID,
                                        DWORD       dwCount,
                                        PDWORD      pdwParams,
                                        OUT INT     *piResult) PURE;


    //
    // ImageProcessing
    //

    STDMETHOD(ImageProcessing)(THIS_    PDEVOBJ             pdevobj,
                                        PBYTE               pSrcBitmap,
                                        PBITMAPINFOHEADER   pBitmapInfoHeader,
                                        PBYTE               pColorTable,
                                        DWORD               dwCallbackID,
                                        PIPPARAMS           pIPParams,
                                        OUT PBYTE           *ppbResult) PURE;

    //
    // FilterGraphics
    //

    STDMETHOD(FilterGraphics) (THIS_    PDEVOBJ     pdevobj,
                                        PBYTE       pBuf,
                                        DWORD       dwLen) PURE;

    //
    // Compression
    //

    STDMETHOD(Compression)(THIS_    PDEVOBJ     pdevobj,
                                    PBYTE       pInBuf,
                                    PBYTE       pOutBuf,
                                    DWORD       dwInLen,
                                    DWORD       dwOutLen,
                                    OUT INT     *piResult) PURE;

    //
    // HalftonePattern
    //

    STDMETHOD(HalftonePattern) (THIS_   PDEVOBJ     pdevobj,
                                        PBYTE       pHTPattern,
                                        DWORD       dwHTPatternX,
                                        DWORD       dwHTPatternY,
                                        DWORD       dwHTNumPatterns,
                                        DWORD       dwCallbackID,
                                        PBYTE       pResource,
                                        DWORD       dwResourceSize) PURE;

    //
    // MemoryUsage
    //

    STDMETHOD(MemoryUsage) (THIS_   PDEVOBJ         pdevobj,
                                    POEMMEMORYUSAGE pMemoryUsage) PURE;

    //
    // TTYGetInfo
    //

    STDMETHOD(TTYGetInfo)(THIS_     PDEVOBJ     pdevobj,
                                    DWORD       dwInfoIndex,
                                    PVOID       pOutputBuf,
                                    DWORD       dwSize,
                                    DWORD       *pcbcNeeded
                                    ) PURE;
    //
    // DownloadFontheader
    //

    STDMETHOD(DownloadFontHeader)(THIS_     PDEVOBJ     pdevobj,
                                            PUNIFONTOBJ pUFObj,
                                            OUT DWORD   *pdwResult) PURE;

    //
    // DownloadCharGlyph
    //

    STDMETHOD(DownloadCharGlyph)(THIS_      PDEVOBJ     pdevobj,
                                            PUNIFONTOBJ pUFObj,
                                            HGLYPH      hGlyph,
                                            PDWORD      pdwWidth,
                                            OUT DWORD   *pdwResult) PURE;


    //
    // TTDownloadMethod
    //

    STDMETHOD(TTDownloadMethod)(THIS_       PDEVOBJ     pdevobj,
                                            PUNIFONTOBJ pUFObj,
                                            OUT DWORD   *pdwResult) PURE;

    //
    // OutputCharStr
    //

    STDMETHOD(OutputCharStr)(THIS_      PDEVOBJ     pdevobj,
                                        PUNIFONTOBJ pUFObj,
                                        DWORD       dwType,
                                        DWORD       dwCount,
                                        PVOID       pGlyph) PURE;

    //
    // SendFontCmd
    //


    STDMETHOD(SendFontCmd)(THIS_    PDEVOBJ      pdevobj,
                                    PUNIFONTOBJ  pUFObj,
                                    PFINVOCATION pFInv) PURE;

    //
    // TextOutAsBitmap
    //

    STDMETHOD(TextOutAsBitmap)(THIS_        SURFOBJ    *pso,
                                            STROBJ     *pstro,
                                            FONTOBJ    *pfo,
                                            CLIPOBJ    *pco,
                                            RECTL      *prclExtra,
                                            RECTL      *prclOpaque,
                                            BRUSHOBJ   *pboFore,
                                            BRUSHOBJ   *pboOpaque,
                                            POINTL     *pptlOrg,
                                            MIX         mix) PURE;
};

//
//****************************************************************************
//  IPrintOemUni2 interface
//****************************************************************************
//

#undef INTERFACE
#define INTERFACE IPrintOemUni2
DECLARE_INTERFACE_(IPrintOemUni2, IPrintOemUni)
{
    //
    // IUnknown methods
    //

    STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID* ppvObj) PURE;
    STDMETHOD_(ULONG, AddRef) (THIS) PURE;
    STDMETHOD_(ULONG, Release) (THIS) PURE;

    //
    // IPrintOemCommon methods
    //

    //
    // Method for getting OEM related information
    //

    STDMETHOD(GetInfo) (THIS_   DWORD   dwMode,
                                PVOID   pBuffer,
                                DWORD   cbSize,
                                PDWORD  pcbNeeded) PURE;
    //
    // Method for OEM private devmode handling
    //

    STDMETHOD(DevMode) (THIS_   DWORD       dwMode,
                                POEMDMPARAM pOemDMParam) PURE;

    //
    // IPrintOemEngine methods
    //

    //
    // Method for OEM to specify DDI hook out
    //

    STDMETHOD(EnableDriver)  (THIS_   DWORD           DriverVersion,
                                      DWORD           cbSize,
                                      PDRVENABLEDATA  pded) PURE;

    //
    // Method to notify OEM plugin that it is no longer required
    //

    STDMETHOD(DisableDriver) (THIS) PURE;

    //
    // Method for OEM to construct its own PDEV
    //

    STDMETHOD(EnablePDEV)    (THIS_   PDEVOBJ         pdevobj,
                                      PWSTR           pPrinterName,
                                      ULONG           cPatterns,
                                      HSURF          *phsurfPatterns,
                                      ULONG           cjGdiInfo,
                                      GDIINFO        *pGdiInfo,
                                      ULONG           cjDevInfo,
                                      DEVINFO        *pDevInfo,
                                      DRVENABLEDATA  *pded,
                                      OUT PDEVOEM    *pDevOem) PURE;

    //
    // Method for OEM to free any resource associated with its PDEV
    //

    STDMETHOD(DisablePDEV)   (THIS_   PDEVOBJ         pdevobj) PURE;

    //
    // Method for OEM to transfer from old PDEV to new PDEV
    //

    STDMETHOD(ResetPDEV)     (THIS_   PDEVOBJ         pdevobjOld,
                                      PDEVOBJ         pdevobjNew) PURE;

    //
    // IPrintOemUni methods
    //

    //
    // Method for publishing Driver interface.
    //

    STDMETHOD(PublishDriverInterface)(THIS_ IUnknown *pIUnknown) PURE;

    //
    // Method for getting OEM implemented methods.
    // Returns S_OK if the given method is implemented.
    // Returns S_FALSE if the given method is not implemented.
    //
    //

    STDMETHOD(GetImplementedMethod) (THIS_  PSTR    pMethodName) PURE;

    //
    // DriverDMS
    //

    STDMETHOD(DriverDMS)(THIS_  PVOID   pDevObj,
                                PVOID   pBuffer,
                                DWORD   cbSize,
                                PDWORD  pcbNeeded) PURE;

    //
    // CommandCallback
    //

    STDMETHOD(CommandCallback)(THIS_    PDEVOBJ     pdevobj,
                                        DWORD       dwCallbackID,
                                        DWORD       dwCount,
                                        PDWORD      pdwParams,
                                        OUT INT     *piResult) PURE;


    //
    // ImageProcessing
    //

    STDMETHOD(ImageProcessing)(THIS_    PDEVOBJ             pdevobj,
                                        PBYTE               pSrcBitmap,
                                        PBITMAPINFOHEADER   pBitmapInfoHeader,
                                        PBYTE               pColorTable,
                                        DWORD               dwCallbackID,
                                        PIPPARAMS           pIPParams,
                                        OUT PBYTE           *ppbResult) PURE;

    //
    // FilterGraphics
    //

    STDMETHOD(FilterGraphics) (THIS_    PDEVOBJ     pdevobj,
                                        PBYTE       pBuf,
                                        DWORD       dwLen) PURE;

    //
    // Compression
    //

    STDMETHOD(Compression)(THIS_    PDEVOBJ     pdevobj,
                                    PBYTE       pInBuf,
                                    PBYTE       pOutBuf,
                                    DWORD       dwInLen,
                                    DWORD       dwOutLen,
                                    OUT INT     *piResult) PURE;

    //
    // HalftonePattern
    //

    STDMETHOD(HalftonePattern) (THIS_   PDEVOBJ     pdevobj,
                                        PBYTE       pHTPattern,
                                        DWORD       dwHTPatternX,
                                        DWORD       dwHTPatternY,
                                        DWORD       dwHTNumPatterns,
                                        DWORD       dwCallbackID,
                                        PBYTE       pResource,
                                        DWORD       dwResourceSize) PURE;

    //
    // MemoryUsage
    //

    STDMETHOD(MemoryUsage) (THIS_   PDEVOBJ         pdevobj,
                                    POEMMEMORYUSAGE pMemoryUsage) PURE;

    //
    // TTYGetInfo
    //

    STDMETHOD(TTYGetInfo)(THIS_     PDEVOBJ     pdevobj,
                                    DWORD       dwInfoIndex,
                                    PVOID       pOutputBuf,
                                    DWORD       dwSize,
                                    DWORD       *pcbcNeeded
                                    ) PURE;
    //
    // DownloadFontheader
    //

    STDMETHOD(DownloadFontHeader)(THIS_     PDEVOBJ     pdevobj,
                                            PUNIFONTOBJ pUFObj,
                                            OUT DWORD   *pdwResult) PURE;

    //
    // DownloadCharGlyph
    //

    STDMETHOD(DownloadCharGlyph)(THIS_      PDEVOBJ     pdevobj,
                                            PUNIFONTOBJ pUFObj,
                                            HGLYPH      hGlyph,
                                            PDWORD      pdwWidth,
                                            OUT DWORD   *pdwResult) PURE;


    //
    // TTDownloadMethod
    //

    STDMETHOD(TTDownloadMethod)(THIS_       PDEVOBJ     pdevobj,
                                            PUNIFONTOBJ pUFObj,
                                            OUT DWORD   *pdwResult) PURE;

    //
    // OutputCharStr
    //

    STDMETHOD(OutputCharStr)(THIS_      PDEVOBJ     pdevobj,
                                        PUNIFONTOBJ pUFObj,
                                        DWORD       dwType,
                                        DWORD       dwCount,
                                        PVOID       pGlyph) PURE;

    //
    // SendFontCmd
    //


    STDMETHOD(SendFontCmd)(THIS_    PDEVOBJ      pdevobj,
                                    PUNIFONTOBJ  pUFObj,
                                    PFINVOCATION pFInv) PURE;

    //
    // TextOutAsBitmap
    //

    STDMETHOD(TextOutAsBitmap)(THIS_        SURFOBJ    *pso,
                                            STROBJ     *pstro,
                                            FONTOBJ    *pfo,
                                            CLIPOBJ    *pco,
                                            RECTL      *prclExtra,
                                            RECTL      *prclOpaque,
                                            BRUSHOBJ   *pboFore,
                                            BRUSHOBJ   *pboOpaque,
                                            POINTL     *pptlOrg,
                                            MIX         mix) PURE;

    //
    // IPrintOemUni2 methods
    //

    //
    // Method for plugin to hook out spooler's WritePrinter API so it
    // can get access to output data Universal driver is generating
    //
    // At DrvEnablePDEV time, Universal driver will call this function with
    // pdevobj = NULL, pBuf = NULL, cbBuffer = 0 to detect if the plugin
    // implements this function. Plugin should return S_OK to indicate it is
    // implementing this function, or return E_NOTIMPL otherwise.
    //
    // In pcbWritten, plugins should return the number of bytes written to the
    // spooler's WritePrinter function. Zero doesn't carry a special meaning,
    // errors must be reported through the returned HRESULT.
    //

    STDMETHOD(WritePrinter) (THIS_   PDEVOBJ    pdevobj,
                                     PVOID      pBuf,
                                     DWORD      cbBuffer,
                                     PDWORD     pcbWritten) PURE;
};


//
//****************************************************************************
//  IPrintOemDriverUni interface
//****************************************************************************
//

#undef INTERFACE
#define INTERFACE IPrintOemDriverUni
DECLARE_INTERFACE_(IPrintOemDriverUni, IUnknown)
{
    //
    // IUnknown methods
    //

    STDMETHOD(QueryInterface) (THIS_ REFIID riid, LPVOID* ppvObj) PURE;
    STDMETHOD_(ULONG, AddRef) (THIS) PURE;
    STDMETHOD_(ULONG, Release) (THIS) PURE;

    //
    // IPrintOemDriverUni methods
    //

    //
    // Function to get driver settings
    //

    STDMETHOD(DrvGetDriverSetting) (THIS_   PVOID   pdriverobj,
                                            PCSTR   Feature,
                                            PVOID   pOutput,
                                            DWORD   cbSize,
                                            PDWORD  pcbNeeded,
                                            PDWORD  pdwOptionsReturned) PURE;

    //
    // Common to both Unidrv & Pscript
    //

    STDMETHOD(DrvWriteSpoolBuf)(THIS_       PDEVOBJ     pdevobj,
                                            PVOID       pBuffer,
                                            DWORD       cbSize,
                                            OUT DWORD   *pdwResult) PURE;

    //
    // Unidrv specific XMoveTo and YMoveTo. Returns E_NOT_IMPL in Pscript
    //

    STDMETHOD(DrvXMoveTo)(THIS_     PDEVOBJ     pdevobj,
                                    INT         x,
                                    DWORD       dwFlags,
                                    OUT INT     *piResult) PURE;

    STDMETHOD(DrvYMoveTo)(THIS_     PDEVOBJ     pdevobj,
                                    INT         y,
                                    DWORD       dwFlags,
                                    OUT INT     *piResult) PURE;
    //
    // Unidrv specific. To get the standard variable value.
    //

    STDMETHOD(DrvGetStandardVariable)(THIS_     PDEVOBJ     pdevobj,
                                                DWORD       dwIndex,
                                                PVOID       pBuffer,
                                                DWORD       cbSize,
                                                PDWORD      pcbNeeded) PURE;

    //
    // Unidrv specific.  To Provide OEM plugins access to GPD data.
    //

    STDMETHOD (DrvGetGPDData)(THIS_  PDEVOBJ     pdevobj,
                                     DWORD       dwType,     // Type of the data
                                     PVOID         pInputData,   // reserved. Should be set to 0
                                     PVOID          pBuffer,     // Caller allocated Buffer to be copied
                                     DWORD       cbSize,     // Size of the buffer
                                     PDWORD      pcbNeeded   // New Size of the buffer if needed.
                             ) PURE;


    //
    // Unidrv specific. To do the TextOut.
    //

    STDMETHOD(DrvUniTextOut)(THIS_    SURFOBJ    *pso,
                                      STROBJ     *pstro,
                                      FONTOBJ    *pfo,
                                      CLIPOBJ    *pco,
                                      RECTL      *prclExtra,
                                      RECTL      *prclOpaque,
                                      BRUSHOBJ   *pboFore,
                                      BRUSHOBJ   *pboOpaque,
                                      POINTL     *pptlBrushOrg,
                                      MIX         mix) PURE;

    //
    //   Warning!!!  new method!!  must place at end of
    //   interface - else major incompatibility with previous oem plugins
    //

    STDMETHOD(DrvWriteAbortBuf)(THIS_       PDEVOBJ     pdevobj,
                                            PVOID       pBuffer,
                                            DWORD       cbSize,
                                            DWORD       dwWait  //  pause data transmission for this many millisecs.
                               ) PURE;
};

#endif  // !KERNEL_MODE

#ifdef __cplusplus
}
#endif

#endif  // !_PRCOMOEM_H_


