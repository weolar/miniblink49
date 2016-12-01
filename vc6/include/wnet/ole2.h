/*#!perl
MapHeaderToDll("ole2.h", "ole32.dll");
ActivateAroundFunctionCall("ole32.dll");
IgnoreFunction("CreateDataAdviseHolder"); # this function occurs in ole2.h and objbase.h
                                          # The wrapped one is in objbase.h
IgnoreFunction("OleBuildVersion"); # not documented
DeclareFunctionErrorValue("OleCreateMenuDescriptor", "NULL");
DeclareFunctionErrorValue("OleDuplicateData", "NULL");
DeclareFunctionErrorValue("OleGetIconOfFile", "NULL");
DeclareFunctionErrorValue("OleGetIconOfClass", "NULL");
DeclareFunctionErrorValue("OleMetafilePictFromIconAndLabel", "NULL");
IgnoreFunction("WlmOleCheckoutMacInterface"); # Macintosh stuff
IgnoreFunction("WlmOleCheckinMacInterface"); # Macintosh stuff
IgnoreFunction("WlmOleWrapMacInterface"); # Macintosh stuff
IgnoreFunction("WlmOleUnwrapMacInterface"); # Macintosh stuff
IgnoreFunction("WlmOleCheckoutWinInterface"); # Macintosh stuff
IgnoreFunction("WlmOleCheckinWinInterface"); # Macintosh stuff
IgnoreFunction("WlmOleWrapWinInterface"); # Macintosh stuff
IgnoreFunction("WlmOleUnwrapWinInterface"); # Macintosh stuff
IgnoreFunction("WlmOleVersion"); # Macintosh stuff
IgnoreFunction("WlmOleSetInPlaceWindow"); # Macintosh stuff
IgnoreFunction("WlmOleRegisterUserWrap"); # Macintosh stuff
*/

//+---------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (c) Microsoft Corporation. All rights reserved.
//
//  File:       OLE2.h
//  Contents:   Main OLE2 header; Defines Linking and Emmebbeding interfaces, and API's.
//              Also includes .h files for the compobj, and oleauto  subcomponents.
//
//----------------------------------------------------------------------------
#if !defined( _OLE2_H_ )
#define _OLE2_H_

#if _MSC_VER > 1000
#pragma once
#endif

#ifndef _MAC
// Set packing to 8
#include <pshpack8.h>

// Make 100% sure WIN32 is defined
#ifndef WIN32
#define WIN32    100  // 100 == NT version 1.0
#endif
#else //_MAC
#ifdef _WIN32
#include "macname1.h"
#endif // _WIN32
#endif //_MAC



// SET to remove _export from interface definitions


#include <winerror.h>

#ifdef _MAC
#if !defined(__MACPUB__)
#include <macpub.h>
#endif
#endif //_MAC

#include <objbase.h>
#include <oleauto.h>

// View OBJECT Error Codes

#define E_DRAW                  VIEW_E_DRAW

// IDataObject Error Codes
#define DATA_E_FORMATETC        DV_E_FORMATETC


#ifdef _MAC
/****** Standard Object Definitions *****************************************/

//#ifndef __COMPOBJ__
//#include <compobj.h>
//#endif



typedef enum {
	OLE_E_NOEXTENSION	= OLE_E_NOSTORAGE +1,	// no extension at startup
	OLE_E_VERSEXTENSION,						// extension has wrong version #
	OLE_E_IPBUSY,								// cannot get inplace resource
	OLE_E_NOT_FRONT_PROCESS,					// inplace container unexpectedly in background
	OLE_E_WRONG_MENU,							// holemenu != prev holemenu
	OLE_E_MENU_NOT_PATCHED,						// menubar is not hashed
	OLE_E_MENUID_NOT_HASHED,					// id was never hashed
	OLE_E_foo
} inplace_errors;


// DoDragDropMac()
typedef struct tagDRAG_FLAVORINFO {
	unsigned long	cfFormat;
	unsigned long	FlavorFlags;		// Flavor Flags as specified by the DragManager
	OLEBOOL			fAddData;			// If true, the data is added with the flavor
	long			reserved;			// must be NULL
} DRAG_FLAVORINFO;

#endif //_MAC


// Common stuff gleamed from OLE.2,

/* verbs */
#define OLEIVERB_PRIMARY            (0L)
#define OLEIVERB_SHOW               (-1L)
#define OLEIVERB_OPEN               (-2L)
#define OLEIVERB_HIDE               (-3L)
#define OLEIVERB_UIACTIVATE         (-4L)
#define OLEIVERB_INPLACEACTIVATE    (-5L)
#define OLEIVERB_DISCARDUNDOSTATE   (-6L)

// for OleCreateEmbeddingHelper flags; roles in low word; options in high word
#define EMBDHLP_INPROC_HANDLER   0x0000L
#define EMBDHLP_INPROC_SERVER    0x0001L
#define EMBDHLP_CREATENOW    0x00000000L
#define EMBDHLP_DELAYCREATE  0x00010000L

/* extended create function flags */
#define OLECREATE_LEAVERUNNING	0x00000001

/* pull in the MIDL generated header */

#include <oleidl.h>


#ifdef _MAC
typedef struct tagOleMBarRec
{
        long reserved;
} OleMBarRec, *OleMBarPtr, **OleMBarHandle;

#define noAxis  3               // indicates no grow or size in any direction
// see mac #defines noConstraint, hAxisOnly, vAxisOnly

#endif //_MAC



/****** DV APIs ***********************************************************/


/* This function is declared in objbase.h and ole2.h.
   IsolationAware support is via objbase.h.
*/
#if    !defined(ISOLATION_AWARE_ENABLED) \
    || !ISOLATION_AWARE_ENABLED \
    || !defined(_OBJBASE_H_) \
    || !defined(CreateDataAdviseHolder)
WINOLEAPI CreateDataAdviseHolder(OUT LPDATAADVISEHOLDER FAR* ppDAHolder);
#endif


/****** OLE API Prototypes ************************************************/

WINOLEAPI_(DWORD) OleBuildVersion( VOID );

/* helper functions */
WINOLEAPI ReadClassStg(IN LPSTORAGE pStg, OUT CLSID FAR* pclsid);
WINOLEAPI WriteClassStg(IN LPSTORAGE pStg, IN REFCLSID rclsid);
WINOLEAPI ReadClassStm(IN LPSTREAM pStm, OUT CLSID FAR* pclsid);
WINOLEAPI WriteClassStm(IN LPSTREAM pStm, IN REFCLSID rclsid);
WINOLEAPI WriteFmtUserTypeStg (IN LPSTORAGE pstg, IN CLIPFORMAT cf, IN LPOLESTR lpszUserType);
WINOLEAPI ReadFmtUserTypeStg (IN LPSTORAGE pstg, OUT CLIPFORMAT FAR* pcf, OUT LPOLESTR FAR* lplpszUserType);


/* init/term */

WINOLEAPI OleInitialize(IN LPVOID pvReserved);
WINOLEAPI_(void) OleUninitialize(void);


/* APIs to query whether (Embedded/Linked) object can be created from
   the data object */

WINOLEAPI  OleQueryLinkFromData(IN LPDATAOBJECT pSrcDataObject);
WINOLEAPI  OleQueryCreateFromData(IN LPDATAOBJECT pSrcDataObject);


/* Object creation APIs */

WINOLEAPI  OleCreate(IN REFCLSID rclsid, IN REFIID riid, IN DWORD renderopt,
                IN LPFORMATETC pFormatEtc, IN LPOLECLIENTSITE pClientSite,
                IN LPSTORAGE pStg, OUT LPVOID FAR* ppvObj);

WINOLEAPI  OleCreateEx(IN REFCLSID rclsid, IN REFIID riid, IN DWORD dwFlags,
                IN DWORD renderopt, IN ULONG cFormats, IN DWORD* rgAdvf,
                IN LPFORMATETC rgFormatEtc, IN IAdviseSink FAR* lpAdviseSink,
                OUT DWORD FAR* rgdwConnection, IN LPOLECLIENTSITE pClientSite,
                IN LPSTORAGE pStg, OUT LPVOID FAR* ppvObj);

WINOLEAPI  OleCreateFromData(IN LPDATAOBJECT pSrcDataObj, IN REFIID riid,
                IN DWORD renderopt, IN LPFORMATETC pFormatEtc,
                IN LPOLECLIENTSITE pClientSite, IN LPSTORAGE pStg,
                OUT LPVOID FAR* ppvObj);

WINOLEAPI  OleCreateFromDataEx(IN LPDATAOBJECT pSrcDataObj, IN REFIID riid,
                IN DWORD dwFlags, IN DWORD renderopt, IN ULONG cFormats, IN DWORD* rgAdvf,
                IN LPFORMATETC rgFormatEtc, IN IAdviseSink FAR* lpAdviseSink,
                OUT DWORD FAR* rgdwConnection, IN LPOLECLIENTSITE pClientSite,
                IN LPSTORAGE pStg, OUT LPVOID FAR* ppvObj);

WINOLEAPI  OleCreateLinkFromData(IN LPDATAOBJECT pSrcDataObj, IN REFIID riid,
                IN DWORD renderopt, IN LPFORMATETC pFormatEtc,
                IN LPOLECLIENTSITE pClientSite, IN LPSTORAGE pStg,
                OUT LPVOID FAR* ppvObj);

WINOLEAPI  OleCreateLinkFromDataEx(IN LPDATAOBJECT pSrcDataObj, IN REFIID riid,
                IN DWORD dwFlags, IN DWORD renderopt, IN ULONG cFormats, IN DWORD* rgAdvf,
                IN LPFORMATETC rgFormatEtc, IN IAdviseSink FAR* lpAdviseSink,
                OUT IN DWORD FAR* rgdwConnection, IN LPOLECLIENTSITE pClientSite,
                IN LPSTORAGE pStg, OUT LPVOID FAR* ppvObj);

WINOLEAPI  OleCreateStaticFromData(IN LPDATAOBJECT pSrcDataObj, IN REFIID iid,
                IN DWORD renderopt, IN LPFORMATETC pFormatEtc,
                IN LPOLECLIENTSITE pClientSite, IN LPSTORAGE pStg,
                OUT LPVOID FAR* ppvObj);


WINOLEAPI  OleCreateLink(IN LPMONIKER pmkLinkSrc, IN REFIID riid,
            IN DWORD renderopt, IN LPFORMATETC lpFormatEtc,
            IN LPOLECLIENTSITE pClientSite, IN LPSTORAGE pStg, OUT LPVOID FAR* ppvObj);

WINOLEAPI  OleCreateLinkEx(IN LPMONIKER pmkLinkSrc, IN REFIID riid,
            IN DWORD dwFlags, IN DWORD renderopt, IN ULONG cFormats, IN DWORD* rgAdvf,
            IN LPFORMATETC rgFormatEtc, IN IAdviseSink FAR* lpAdviseSink,
            OUT DWORD FAR* rgdwConnection, IN LPOLECLIENTSITE pClientSite,
            IN LPSTORAGE pStg, OUT LPVOID FAR* ppvObj);

WINOLEAPI  OleCreateLinkToFile(IN LPCOLESTR lpszFileName, IN REFIID riid,
            IN DWORD renderopt, IN LPFORMATETC lpFormatEtc,
            IN LPOLECLIENTSITE pClientSite, IN LPSTORAGE pStg, OUT LPVOID FAR* ppvObj);

WINOLEAPI  OleCreateLinkToFileEx(IN LPCOLESTR lpszFileName, IN REFIID riid,
            IN DWORD dwFlags, IN DWORD renderopt, IN ULONG cFormats, IN DWORD* rgAdvf,
            IN LPFORMATETC rgFormatEtc, IN IAdviseSink FAR* lpAdviseSink,
            OUT DWORD FAR* rgdwConnection, IN LPOLECLIENTSITE pClientSite,
            IN LPSTORAGE pStg, OUT LPVOID FAR* ppvObj);

WINOLEAPI  OleCreateFromFile(IN REFCLSID rclsid, IN LPCOLESTR lpszFileName, IN REFIID riid,
            IN DWORD renderopt, IN LPFORMATETC lpFormatEtc,
            IN LPOLECLIENTSITE pClientSite, IN LPSTORAGE pStg, OUT LPVOID FAR* ppvObj);

WINOLEAPI  OleCreateFromFileEx(IN REFCLSID rclsid, IN LPCOLESTR lpszFileName, IN REFIID riid,
            IN DWORD dwFlags, IN DWORD renderopt, IN ULONG cFormats, IN DWORD* rgAdvf,
            IN LPFORMATETC rgFormatEtc, IN IAdviseSink FAR* lpAdviseSink,
            OUT DWORD FAR* rgdwConnection, IN LPOLECLIENTSITE pClientSite,
            IN LPSTORAGE pStg, OUT LPVOID FAR* ppvObj);

WINOLEAPI  OleLoad(IN LPSTORAGE pStg, IN REFIID riid, IN LPOLECLIENTSITE pClientSite,
            OUT LPVOID FAR* ppvObj);

WINOLEAPI  OleSave(IN LPPERSISTSTORAGE pPS, IN LPSTORAGE pStg, IN BOOL fSameAsLoad);

WINOLEAPI  OleLoadFromStream( IN LPSTREAM pStm, IN REFIID iidInterface, OUT LPVOID FAR* ppvObj);
WINOLEAPI  OleSaveToStream( IN LPPERSISTSTREAM pPStm, IN LPSTREAM pStm );


WINOLEAPI  OleSetContainedObject(IN LPUNKNOWN pUnknown, IN BOOL fContained);
WINOLEAPI  OleNoteObjectVisible(IN LPUNKNOWN pUnknown, IN BOOL fVisible);


/* Drag/Drop APIs */

WINOLEAPI  RegisterDragDrop(IN HWND hwnd, IN LPDROPTARGET pDropTarget);
WINOLEAPI  RevokeDragDrop(IN HWND hwnd);
WINOLEAPI  DoDragDrop(IN LPDATAOBJECT pDataObj, IN LPDROPSOURCE pDropSource,
            IN DWORD dwOKEffects, OUT LPDWORD pdwEffect);

/* Clipboard APIs */

WINOLEAPI  OleSetClipboard(IN LPDATAOBJECT pDataObj);
WINOLEAPI  OleGetClipboard(OUT LPDATAOBJECT FAR* ppDataObj);
WINOLEAPI  OleFlushClipboard(void);
WINOLEAPI  OleIsCurrentClipboard(IN LPDATAOBJECT pDataObj);


/* InPlace Editing APIs */

WINOLEAPI_(HOLEMENU)   OleCreateMenuDescriptor (IN HMENU hmenuCombined,
                                IN LPOLEMENUGROUPWIDTHS lpMenuWidths);
WINOLEAPI              OleSetMenuDescriptor (IN HOLEMENU holemenu, IN HWND hwndFrame,
                                IN HWND hwndActiveObject,
                                IN LPOLEINPLACEFRAME lpFrame,
                                IN LPOLEINPLACEACTIVEOBJECT lpActiveObj);
WINOLEAPI              OleDestroyMenuDescriptor (IN HOLEMENU holemenu);

WINOLEAPI              OleTranslateAccelerator (IN LPOLEINPLACEFRAME lpFrame,
                            IN LPOLEINPLACEFRAMEINFO lpFrameInfo, IN LPMSG lpmsg);


/* Helper APIs */
WINOLEAPI_(HANDLE) OleDuplicateData (IN HANDLE hSrc, IN CLIPFORMAT cfFormat,
                        IN UINT uiFlags);

WINOLEAPI          OleDraw (IN LPUNKNOWN pUnknown, IN DWORD dwAspect, IN HDC hdcDraw,
                    IN LPCRECT lprcBounds);

WINOLEAPI          OleRun(IN LPUNKNOWN pUnknown);
WINOLEAPI_(BOOL)   OleIsRunning(IN LPOLEOBJECT pObject);
WINOLEAPI          OleLockRunning(IN LPUNKNOWN pUnknown, IN BOOL fLock, IN BOOL fLastUnlockCloses);
WINOLEAPI_(void)   ReleaseStgMedium(IN LPSTGMEDIUM);
WINOLEAPI          CreateOleAdviseHolder(OUT LPOLEADVISEHOLDER FAR* ppOAHolder);

WINOLEAPI          OleCreateDefaultHandler(IN REFCLSID clsid, IN LPUNKNOWN pUnkOuter,
                    IN REFIID riid, OUT LPVOID FAR* lplpObj);

WINOLEAPI          OleCreateEmbeddingHelper(IN REFCLSID clsid, IN LPUNKNOWN pUnkOuter,
                    IN DWORD flags, IN LPCLASSFACTORY pCF,
                    IN REFIID riid, OUT LPVOID FAR* lplpObj);

WINOLEAPI_(BOOL)   IsAccelerator(IN HACCEL hAccel, IN int cAccelEntries, IN LPMSG lpMsg,
                                        OUT WORD FAR* lpwCmd);
/* Icon extraction Helper APIs */

WINOLEAPI_(HGLOBAL) OleGetIconOfFile(IN LPOLESTR lpszPath, IN BOOL fUseFileAsLabel);

WINOLEAPI_(HGLOBAL) OleGetIconOfClass(IN REFCLSID rclsid,     IN LPOLESTR lpszLabel,
                                        IN BOOL fUseTypeAsLabel);

WINOLEAPI_(HGLOBAL) OleMetafilePictFromIconAndLabel(IN HICON hIcon, IN LPOLESTR lpszLabel,
                                        IN LPOLESTR lpszSourceFile, IN UINT iIconIndex);



/* Registration Database Helper APIs */

WINOLEAPI                  OleRegGetUserType (IN REFCLSID clsid, IN DWORD dwFormOfType,
                                        OUT LPOLESTR FAR* pszUserType);

WINOLEAPI                  OleRegGetMiscStatus     (IN REFCLSID clsid, IN DWORD dwAspect,
                                        OUT DWORD FAR* pdwStatus);

WINOLEAPI                  OleRegEnumFormatEtc     (IN REFCLSID clsid, IN DWORD dwDirection,
                                        OUT LPENUMFORMATETC FAR* ppenum);

WINOLEAPI                  OleRegEnumVerbs (IN REFCLSID clsid, OUT LPENUMOLEVERB FAR* ppenum);

#ifdef _MAC
/* WlmOLE helper APIs */

WINOLEAPI WlmOleCheckoutMacInterface(LPUNKNOWN pUnk, LPVOID* ppv);
WINOLEAPI WlmOleCheckinMacInterface(LPUNKNOWN pUnk);
WINOLEAPI WlmOleWrapMacInterface(LPUNKNOWN pUnk, REFIID riid, LPVOID* ppv);
WINOLEAPI WlmOleUnwrapMacInterface(LPVOID pv);

WINOLEAPI WlmOleCheckoutWinInterface(LPVOID pUnk, LPUNKNOWN* ppv);
WINOLEAPI WlmOleCheckinWinInterface(LPVOID pUnk);
WINOLEAPI WlmOleWrapWinInterface(LPVOID pUnk, REFIID riid, LPUNKNOWN* ppv);
WINOLEAPI WlmOleUnwrapWinInterface(LPVOID pv);

WINOLEAPI_(void) WlmOleVersion(void);
WINOLEAPI_(void) WlmOleSetInPlaceWindow(HWND hwnd);

typedef HRESULT (STDAPICALLTYPE* OLEWRAPPROC) (REFIID riid, LPVOID* ppvWin, LPVOID* ppvMac);
WINOLEAPI WlmOleRegisterUserWrap(OLEWRAPPROC procNew, OLEWRAPPROC* pprocOld);
#endif


/* OLE 1.0 conversion APIS */

/***** OLE 1.0 OLESTREAM declarations *************************************/

typedef struct _OLESTREAM FAR*  LPOLESTREAM;

typedef struct _OLESTREAMVTBL
{
    DWORD (CALLBACK* Get)(LPOLESTREAM, void FAR*, DWORD);
    DWORD (CALLBACK* Put)(LPOLESTREAM, const void FAR*, DWORD);
} OLESTREAMVTBL;
typedef  OLESTREAMVTBL FAR*  LPOLESTREAMVTBL;

typedef struct _OLESTREAM
{
    LPOLESTREAMVTBL lpstbl;
} OLESTREAM;


WINOLEAPI OleConvertOLESTREAMToIStorage
    (IN LPOLESTREAM                lpolestream,
    OUT LPSTORAGE                   pstg,
    IN const DVTARGETDEVICE FAR*   ptd);

WINOLEAPI OleConvertIStorageToOLESTREAM
    (IN LPSTORAGE      pstg,
    OUT LPOLESTREAM     lpolestream);


/* Storage Utility APIs */
WINOLEAPI GetHGlobalFromILockBytes (IN LPLOCKBYTES plkbyt, OUT HGLOBAL FAR* phglobal);
WINOLEAPI CreateILockBytesOnHGlobal (IN HGLOBAL hGlobal, IN BOOL fDeleteOnRelease,
                                    OUT LPLOCKBYTES FAR* pplkbyt);

WINOLEAPI GetHGlobalFromStream (IN LPSTREAM pstm, OUT HGLOBAL FAR* phglobal);
WINOLEAPI CreateStreamOnHGlobal (IN HGLOBAL hGlobal, IN BOOL fDeleteOnRelease,
                                OUT LPSTREAM FAR* ppstm);


/* ConvertTo APIS */

WINOLEAPI OleDoAutoConvert(IN LPSTORAGE pStg, OUT LPCLSID pClsidNew);
WINOLEAPI OleGetAutoConvert(IN REFCLSID clsidOld, OUT LPCLSID pClsidNew);
WINOLEAPI OleSetAutoConvert(IN REFCLSID clsidOld, IN REFCLSID clsidNew);
WINOLEAPI GetConvertStg(IN LPSTORAGE pStg);
WINOLEAPI SetConvertStg(IN LPSTORAGE pStg, IN BOOL fConvert);


WINOLEAPI OleConvertIStorageToOLESTREAMEx
    (IN LPSTORAGE          pstg,
                                    // Presentation data to OLESTREAM
     IN CLIPFORMAT         cfFormat,   //      format
     IN LONG               lWidth,     //      width
     IN LONG               lHeight,    //      height
     IN DWORD              dwSize,     //      size in bytes
     IN LPSTGMEDIUM        pmedium,    //      bits
     OUT LPOLESTREAM        polestm);

WINOLEAPI OleConvertOLESTREAMToIStorageEx
    (IN LPOLESTREAM        polestm,
     OUT LPSTORAGE          pstg,
                                    // Presentation data from OLESTREAM
     OUT CLIPFORMAT FAR*    pcfFormat,  //      format
     OUT LONG FAR*          plwWidth,   //      width
     OUT LONG FAR*          plHeight,   //      height
     OUT DWORD FAR*         pdwSize,    //      size in bytes
     OUT LPSTGMEDIUM        pmedium);   //      bits

#ifndef _MAC
#ifndef RC_INVOKED
#include <poppack.h>
#endif // RC_INVOKED

#else // _MAC

#ifndef __MACAPI__
#include <macapi.h>
#endif

#ifdef _WIN32
#include "macname2.h"
#endif // _WIN32

#endif // _MAC

//d308927f-ca94-4c96-8d87-2692a3dadff6
/*#!perl
SetInsertionPoint("ole2.h", "d308927f-ca94-4c96-8d87-2692a3dadff6");
*/

#endif     // __OLE2_H__

