

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 6.00.0366 */
/* Compiler settings for shobjidl.idl:
    Oicf, W1, Zp8, env=Win32 (32b run)
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
//@@MIDL_FILE_HEADING(  )

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif // __RPCNDR_H_VERSION__

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __shobjidl_h__
#define __shobjidl_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

/* Forward Declarations */ 

#ifndef __IPersistFolder_FWD_DEFINED__
#define __IPersistFolder_FWD_DEFINED__
typedef interface IPersistFolder IPersistFolder;
#endif 	/* __IPersistFolder_FWD_DEFINED__ */


#ifndef __IPersistFolder2_FWD_DEFINED__
#define __IPersistFolder2_FWD_DEFINED__
typedef interface IPersistFolder2 IPersistFolder2;
#endif 	/* __IPersistFolder2_FWD_DEFINED__ */


#ifndef __IPersistIDList_FWD_DEFINED__
#define __IPersistIDList_FWD_DEFINED__
typedef interface IPersistIDList IPersistIDList;
#endif 	/* __IPersistIDList_FWD_DEFINED__ */


#ifndef __IEnumIDList_FWD_DEFINED__
#define __IEnumIDList_FWD_DEFINED__
typedef interface IEnumIDList IEnumIDList;
#endif 	/* __IEnumIDList_FWD_DEFINED__ */


#ifndef __IShellFolder_FWD_DEFINED__
#define __IShellFolder_FWD_DEFINED__
typedef interface IShellFolder IShellFolder;
#endif 	/* __IShellFolder_FWD_DEFINED__ */


#ifndef __IEnumExtraSearch_FWD_DEFINED__
#define __IEnumExtraSearch_FWD_DEFINED__
typedef interface IEnumExtraSearch IEnumExtraSearch;
#endif 	/* __IEnumExtraSearch_FWD_DEFINED__ */


#ifndef __IShellFolder2_FWD_DEFINED__
#define __IShellFolder2_FWD_DEFINED__
typedef interface IShellFolder2 IShellFolder2;
#endif 	/* __IShellFolder2_FWD_DEFINED__ */


#ifndef __IShellView_FWD_DEFINED__
#define __IShellView_FWD_DEFINED__
typedef interface IShellView IShellView;
#endif 	/* __IShellView_FWD_DEFINED__ */


#ifndef __IShellView2_FWD_DEFINED__
#define __IShellView2_FWD_DEFINED__
typedef interface IShellView2 IShellView2;
#endif 	/* __IShellView2_FWD_DEFINED__ */


#ifndef __IFolderView_FWD_DEFINED__
#define __IFolderView_FWD_DEFINED__
typedef interface IFolderView IFolderView;
#endif 	/* __IFolderView_FWD_DEFINED__ */


#ifndef __IFolderFilterSite_FWD_DEFINED__
#define __IFolderFilterSite_FWD_DEFINED__
typedef interface IFolderFilterSite IFolderFilterSite;
#endif 	/* __IFolderFilterSite_FWD_DEFINED__ */


#ifndef __IFolderFilter_FWD_DEFINED__
#define __IFolderFilter_FWD_DEFINED__
typedef interface IFolderFilter IFolderFilter;
#endif 	/* __IFolderFilter_FWD_DEFINED__ */


#ifndef __IShellBrowser_FWD_DEFINED__
#define __IShellBrowser_FWD_DEFINED__
typedef interface IShellBrowser IShellBrowser;
#endif 	/* __IShellBrowser_FWD_DEFINED__ */


#ifndef __IProfferService_FWD_DEFINED__
#define __IProfferService_FWD_DEFINED__
typedef interface IProfferService IProfferService;
#endif 	/* __IProfferService_FWD_DEFINED__ */


#ifndef __IPropertyUI_FWD_DEFINED__
#define __IPropertyUI_FWD_DEFINED__
typedef interface IPropertyUI IPropertyUI;
#endif 	/* __IPropertyUI_FWD_DEFINED__ */


#ifndef __ICategoryProvider_FWD_DEFINED__
#define __ICategoryProvider_FWD_DEFINED__
typedef interface ICategoryProvider ICategoryProvider;
#endif 	/* __ICategoryProvider_FWD_DEFINED__ */


#ifndef __ICategorizer_FWD_DEFINED__
#define __ICategorizer_FWD_DEFINED__
typedef interface ICategorizer ICategorizer;
#endif 	/* __ICategorizer_FWD_DEFINED__ */


#ifndef __IShellLinkA_FWD_DEFINED__
#define __IShellLinkA_FWD_DEFINED__
typedef interface IShellLinkA IShellLinkA;
#endif 	/* __IShellLinkA_FWD_DEFINED__ */


#ifndef __IShellLinkW_FWD_DEFINED__
#define __IShellLinkW_FWD_DEFINED__
typedef interface IShellLinkW IShellLinkW;
#endif 	/* __IShellLinkW_FWD_DEFINED__ */


#ifndef __IActionProgressDialog_FWD_DEFINED__
#define __IActionProgressDialog_FWD_DEFINED__
typedef interface IActionProgressDialog IActionProgressDialog;
#endif 	/* __IActionProgressDialog_FWD_DEFINED__ */


#ifndef __IHWEventHandler_FWD_DEFINED__
#define __IHWEventHandler_FWD_DEFINED__
typedef interface IHWEventHandler IHWEventHandler;
#endif 	/* __IHWEventHandler_FWD_DEFINED__ */


#ifndef __IQueryCancelAutoPlay_FWD_DEFINED__
#define __IQueryCancelAutoPlay_FWD_DEFINED__
typedef interface IQueryCancelAutoPlay IQueryCancelAutoPlay;
#endif 	/* __IQueryCancelAutoPlay_FWD_DEFINED__ */


#ifndef __IActionProgress_FWD_DEFINED__
#define __IActionProgress_FWD_DEFINED__
typedef interface IActionProgress IActionProgress;
#endif 	/* __IActionProgress_FWD_DEFINED__ */


#ifndef __IShellExtInit_FWD_DEFINED__
#define __IShellExtInit_FWD_DEFINED__
typedef interface IShellExtInit IShellExtInit;
#endif 	/* __IShellExtInit_FWD_DEFINED__ */


#ifndef __IShellPropSheetExt_FWD_DEFINED__
#define __IShellPropSheetExt_FWD_DEFINED__
typedef interface IShellPropSheetExt IShellPropSheetExt;
#endif 	/* __IShellPropSheetExt_FWD_DEFINED__ */


#ifndef __IRemoteComputer_FWD_DEFINED__
#define __IRemoteComputer_FWD_DEFINED__
typedef interface IRemoteComputer IRemoteComputer;
#endif 	/* __IRemoteComputer_FWD_DEFINED__ */


#ifndef __IQueryContinue_FWD_DEFINED__
#define __IQueryContinue_FWD_DEFINED__
typedef interface IQueryContinue IQueryContinue;
#endif 	/* __IQueryContinue_FWD_DEFINED__ */


#ifndef __IUserNotification_FWD_DEFINED__
#define __IUserNotification_FWD_DEFINED__
typedef interface IUserNotification IUserNotification;
#endif 	/* __IUserNotification_FWD_DEFINED__ */


#ifndef __IItemNameLimits_FWD_DEFINED__
#define __IItemNameLimits_FWD_DEFINED__
typedef interface IItemNameLimits IItemNameLimits;
#endif 	/* __IItemNameLimits_FWD_DEFINED__ */


#ifndef __INetCrawler_FWD_DEFINED__
#define __INetCrawler_FWD_DEFINED__
typedef interface INetCrawler INetCrawler;
#endif 	/* __INetCrawler_FWD_DEFINED__ */


#ifndef __IExtractImage_FWD_DEFINED__
#define __IExtractImage_FWD_DEFINED__
typedef interface IExtractImage IExtractImage;
#endif 	/* __IExtractImage_FWD_DEFINED__ */


#ifndef __IExtractImage2_FWD_DEFINED__
#define __IExtractImage2_FWD_DEFINED__
typedef interface IExtractImage2 IExtractImage2;
#endif 	/* __IExtractImage2_FWD_DEFINED__ */


#ifndef __IUserEventTimerCallback_FWD_DEFINED__
#define __IUserEventTimerCallback_FWD_DEFINED__
typedef interface IUserEventTimerCallback IUserEventTimerCallback;
#endif 	/* __IUserEventTimerCallback_FWD_DEFINED__ */


#ifndef __IUserEventTimer_FWD_DEFINED__
#define __IUserEventTimer_FWD_DEFINED__
typedef interface IUserEventTimer IUserEventTimer;
#endif 	/* __IUserEventTimer_FWD_DEFINED__ */


#ifndef __IDockingWindow_FWD_DEFINED__
#define __IDockingWindow_FWD_DEFINED__
typedef interface IDockingWindow IDockingWindow;
#endif 	/* __IDockingWindow_FWD_DEFINED__ */


#ifndef __IDeskBand_FWD_DEFINED__
#define __IDeskBand_FWD_DEFINED__
typedef interface IDeskBand IDeskBand;
#endif 	/* __IDeskBand_FWD_DEFINED__ */


#ifndef __ITaskbarList_FWD_DEFINED__
#define __ITaskbarList_FWD_DEFINED__
typedef interface ITaskbarList ITaskbarList;
#endif 	/* __ITaskbarList_FWD_DEFINED__ */


#ifndef __ITaskbarList2_FWD_DEFINED__
#define __ITaskbarList2_FWD_DEFINED__
typedef interface ITaskbarList2 ITaskbarList2;
#endif 	/* __ITaskbarList2_FWD_DEFINED__ */


#ifndef __ICDBurn_FWD_DEFINED__
#define __ICDBurn_FWD_DEFINED__
typedef interface ICDBurn ICDBurn;
#endif 	/* __ICDBurn_FWD_DEFINED__ */


#ifndef __IWizardSite_FWD_DEFINED__
#define __IWizardSite_FWD_DEFINED__
typedef interface IWizardSite IWizardSite;
#endif 	/* __IWizardSite_FWD_DEFINED__ */


#ifndef __IWizardExtension_FWD_DEFINED__
#define __IWizardExtension_FWD_DEFINED__
typedef interface IWizardExtension IWizardExtension;
#endif 	/* __IWizardExtension_FWD_DEFINED__ */


#ifndef __IWebWizardExtension_FWD_DEFINED__
#define __IWebWizardExtension_FWD_DEFINED__
typedef interface IWebWizardExtension IWebWizardExtension;
#endif 	/* __IWebWizardExtension_FWD_DEFINED__ */


#ifndef __IPublishingWizard_FWD_DEFINED__
#define __IPublishingWizard_FWD_DEFINED__
typedef interface IPublishingWizard IPublishingWizard;
#endif 	/* __IPublishingWizard_FWD_DEFINED__ */


#ifndef __IFolderViewHost_FWD_DEFINED__
#define __IFolderViewHost_FWD_DEFINED__
typedef interface IFolderViewHost IFolderViewHost;
#endif 	/* __IFolderViewHost_FWD_DEFINED__ */


#ifndef __IAutoCompleteDropDown_FWD_DEFINED__
#define __IAutoCompleteDropDown_FWD_DEFINED__
typedef interface IAutoCompleteDropDown IAutoCompleteDropDown;
#endif 	/* __IAutoCompleteDropDown_FWD_DEFINED__ */


#ifndef __IModalWindow_FWD_DEFINED__
#define __IModalWindow_FWD_DEFINED__
typedef interface IModalWindow IModalWindow;
#endif 	/* __IModalWindow_FWD_DEFINED__ */


#ifndef __IPassportWizard_FWD_DEFINED__
#define __IPassportWizard_FWD_DEFINED__
typedef interface IPassportWizard IPassportWizard;
#endif 	/* __IPassportWizard_FWD_DEFINED__ */


#ifndef __ICDBurnExt_FWD_DEFINED__
#define __ICDBurnExt_FWD_DEFINED__
typedef interface ICDBurnExt ICDBurnExt;
#endif 	/* __ICDBurnExt_FWD_DEFINED__ */


#ifndef __IDVGetEnum_FWD_DEFINED__
#define __IDVGetEnum_FWD_DEFINED__
typedef interface IDVGetEnum IDVGetEnum;
#endif 	/* __IDVGetEnum_FWD_DEFINED__ */


#ifndef __IInsertItem_FWD_DEFINED__
#define __IInsertItem_FWD_DEFINED__
typedef interface IInsertItem IInsertItem;
#endif 	/* __IInsertItem_FWD_DEFINED__ */


#ifndef __IDeskBar_FWD_DEFINED__
#define __IDeskBar_FWD_DEFINED__
typedef interface IDeskBar IDeskBar;
#endif 	/* __IDeskBar_FWD_DEFINED__ */


#ifndef __IMenuBand_FWD_DEFINED__
#define __IMenuBand_FWD_DEFINED__
typedef interface IMenuBand IMenuBand;
#endif 	/* __IMenuBand_FWD_DEFINED__ */


#ifndef __IFolderBandPriv_FWD_DEFINED__
#define __IFolderBandPriv_FWD_DEFINED__
typedef interface IFolderBandPriv IFolderBandPriv;
#endif 	/* __IFolderBandPriv_FWD_DEFINED__ */


#ifndef __IBandSite_FWD_DEFINED__
#define __IBandSite_FWD_DEFINED__
typedef interface IBandSite IBandSite;
#endif 	/* __IBandSite_FWD_DEFINED__ */


#ifndef __INamespaceWalkCB_FWD_DEFINED__
#define __INamespaceWalkCB_FWD_DEFINED__
typedef interface INamespaceWalkCB INamespaceWalkCB;
#endif 	/* __INamespaceWalkCB_FWD_DEFINED__ */


#ifndef __INamespaceWalk_FWD_DEFINED__
#define __INamespaceWalk_FWD_DEFINED__
typedef interface INamespaceWalk INamespaceWalk;
#endif 	/* __INamespaceWalk_FWD_DEFINED__ */


#ifndef __IRegTreeItem_FWD_DEFINED__
#define __IRegTreeItem_FWD_DEFINED__
typedef interface IRegTreeItem IRegTreeItem;
#endif 	/* __IRegTreeItem_FWD_DEFINED__ */


#ifndef __IMenuPopup_FWD_DEFINED__
#define __IMenuPopup_FWD_DEFINED__
typedef interface IMenuPopup IMenuPopup;
#endif 	/* __IMenuPopup_FWD_DEFINED__ */


#ifndef __IShellItem_FWD_DEFINED__
#define __IShellItem_FWD_DEFINED__
typedef interface IShellItem IShellItem;
#endif 	/* __IShellItem_FWD_DEFINED__ */


#ifndef __IImageRecompress_FWD_DEFINED__
#define __IImageRecompress_FWD_DEFINED__
typedef interface IImageRecompress IImageRecompress;
#endif 	/* __IImageRecompress_FWD_DEFINED__ */


#ifndef __IDefViewSafety_FWD_DEFINED__
#define __IDefViewSafety_FWD_DEFINED__
typedef interface IDefViewSafety IDefViewSafety;
#endif 	/* __IDefViewSafety_FWD_DEFINED__ */


#ifndef __IContextMenuSite_FWD_DEFINED__
#define __IContextMenuSite_FWD_DEFINED__
typedef interface IContextMenuSite IContextMenuSite;
#endif 	/* __IContextMenuSite_FWD_DEFINED__ */


#ifndef __IDelegateFolder_FWD_DEFINED__
#define __IDelegateFolder_FWD_DEFINED__
typedef interface IDelegateFolder IDelegateFolder;
#endif 	/* __IDelegateFolder_FWD_DEFINED__ */


#ifndef __IBrowserFrameOptions_FWD_DEFINED__
#define __IBrowserFrameOptions_FWD_DEFINED__
typedef interface IBrowserFrameOptions IBrowserFrameOptions;
#endif 	/* __IBrowserFrameOptions_FWD_DEFINED__ */


#ifndef __INewWindowManager_FWD_DEFINED__
#define __INewWindowManager_FWD_DEFINED__
typedef interface INewWindowManager INewWindowManager;
#endif 	/* __INewWindowManager_FWD_DEFINED__ */


#ifndef __IShellMenuCallback_FWD_DEFINED__
#define __IShellMenuCallback_FWD_DEFINED__
typedef interface IShellMenuCallback IShellMenuCallback;
#endif 	/* __IShellMenuCallback_FWD_DEFINED__ */


#ifndef __IAttachmentExecute_FWD_DEFINED__
#define __IAttachmentExecute_FWD_DEFINED__
typedef interface IAttachmentExecute IAttachmentExecute;
#endif 	/* __IAttachmentExecute_FWD_DEFINED__ */


#ifndef __IShellMenu_FWD_DEFINED__
#define __IShellMenu_FWD_DEFINED__
typedef interface IShellMenu IShellMenu;
#endif 	/* __IShellMenu_FWD_DEFINED__ */


#ifndef __QueryCancelAutoPlay_FWD_DEFINED__
#define __QueryCancelAutoPlay_FWD_DEFINED__

#ifdef __cplusplus
typedef class QueryCancelAutoPlay QueryCancelAutoPlay;
#else
typedef struct QueryCancelAutoPlay QueryCancelAutoPlay;
#endif /* __cplusplus */

#endif 	/* __QueryCancelAutoPlay_FWD_DEFINED__ */


#ifndef __DriveSizeCategorizer_FWD_DEFINED__
#define __DriveSizeCategorizer_FWD_DEFINED__

#ifdef __cplusplus
typedef class DriveSizeCategorizer DriveSizeCategorizer;
#else
typedef struct DriveSizeCategorizer DriveSizeCategorizer;
#endif /* __cplusplus */

#endif 	/* __DriveSizeCategorizer_FWD_DEFINED__ */


#ifndef __DriveTypeCategorizer_FWD_DEFINED__
#define __DriveTypeCategorizer_FWD_DEFINED__

#ifdef __cplusplus
typedef class DriveTypeCategorizer DriveTypeCategorizer;
#else
typedef struct DriveTypeCategorizer DriveTypeCategorizer;
#endif /* __cplusplus */

#endif 	/* __DriveTypeCategorizer_FWD_DEFINED__ */


#ifndef __FreeSpaceCategorizer_FWD_DEFINED__
#define __FreeSpaceCategorizer_FWD_DEFINED__

#ifdef __cplusplus
typedef class FreeSpaceCategorizer FreeSpaceCategorizer;
#else
typedef struct FreeSpaceCategorizer FreeSpaceCategorizer;
#endif /* __cplusplus */

#endif 	/* __FreeSpaceCategorizer_FWD_DEFINED__ */


#ifndef __TimeCategorizer_FWD_DEFINED__
#define __TimeCategorizer_FWD_DEFINED__

#ifdef __cplusplus
typedef class TimeCategorizer TimeCategorizer;
#else
typedef struct TimeCategorizer TimeCategorizer;
#endif /* __cplusplus */

#endif 	/* __TimeCategorizer_FWD_DEFINED__ */


#ifndef __SizeCategorizer_FWD_DEFINED__
#define __SizeCategorizer_FWD_DEFINED__

#ifdef __cplusplus
typedef class SizeCategorizer SizeCategorizer;
#else
typedef struct SizeCategorizer SizeCategorizer;
#endif /* __cplusplus */

#endif 	/* __SizeCategorizer_FWD_DEFINED__ */


#ifndef __AlphabeticalCategorizer_FWD_DEFINED__
#define __AlphabeticalCategorizer_FWD_DEFINED__

#ifdef __cplusplus
typedef class AlphabeticalCategorizer AlphabeticalCategorizer;
#else
typedef struct AlphabeticalCategorizer AlphabeticalCategorizer;
#endif /* __cplusplus */

#endif 	/* __AlphabeticalCategorizer_FWD_DEFINED__ */


#ifndef __MergedCategorizer_FWD_DEFINED__
#define __MergedCategorizer_FWD_DEFINED__

#ifdef __cplusplus
typedef class MergedCategorizer MergedCategorizer;
#else
typedef struct MergedCategorizer MergedCategorizer;
#endif /* __cplusplus */

#endif 	/* __MergedCategorizer_FWD_DEFINED__ */


#ifndef __ImageProperties_FWD_DEFINED__
#define __ImageProperties_FWD_DEFINED__

#ifdef __cplusplus
typedef class ImageProperties ImageProperties;
#else
typedef struct ImageProperties ImageProperties;
#endif /* __cplusplus */

#endif 	/* __ImageProperties_FWD_DEFINED__ */


#ifndef __PropertiesUI_FWD_DEFINED__
#define __PropertiesUI_FWD_DEFINED__

#ifdef __cplusplus
typedef class PropertiesUI PropertiesUI;
#else
typedef struct PropertiesUI PropertiesUI;
#endif /* __cplusplus */

#endif 	/* __PropertiesUI_FWD_DEFINED__ */


#ifndef __UserNotification_FWD_DEFINED__
#define __UserNotification_FWD_DEFINED__

#ifdef __cplusplus
typedef class UserNotification UserNotification;
#else
typedef struct UserNotification UserNotification;
#endif /* __cplusplus */

#endif 	/* __UserNotification_FWD_DEFINED__ */


#ifndef __UserEventTimerCallback_FWD_DEFINED__
#define __UserEventTimerCallback_FWD_DEFINED__

#ifdef __cplusplus
typedef class UserEventTimerCallback UserEventTimerCallback;
#else
typedef struct UserEventTimerCallback UserEventTimerCallback;
#endif /* __cplusplus */

#endif 	/* __UserEventTimerCallback_FWD_DEFINED__ */


#ifndef __UserEventTimer_FWD_DEFINED__
#define __UserEventTimer_FWD_DEFINED__

#ifdef __cplusplus
typedef class UserEventTimer UserEventTimer;
#else
typedef struct UserEventTimer UserEventTimer;
#endif /* __cplusplus */

#endif 	/* __UserEventTimer_FWD_DEFINED__ */


#ifndef __NetCrawler_FWD_DEFINED__
#define __NetCrawler_FWD_DEFINED__

#ifdef __cplusplus
typedef class NetCrawler NetCrawler;
#else
typedef struct NetCrawler NetCrawler;
#endif /* __cplusplus */

#endif 	/* __NetCrawler_FWD_DEFINED__ */


#ifndef __CDBurn_FWD_DEFINED__
#define __CDBurn_FWD_DEFINED__

#ifdef __cplusplus
typedef class CDBurn CDBurn;
#else
typedef struct CDBurn CDBurn;
#endif /* __cplusplus */

#endif 	/* __CDBurn_FWD_DEFINED__ */


#ifndef __TaskbarList_FWD_DEFINED__
#define __TaskbarList_FWD_DEFINED__

#ifdef __cplusplus
typedef class TaskbarList TaskbarList;
#else
typedef struct TaskbarList TaskbarList;
#endif /* __cplusplus */

#endif 	/* __TaskbarList_FWD_DEFINED__ */


#ifndef __WebWizardHost_FWD_DEFINED__
#define __WebWizardHost_FWD_DEFINED__

#ifdef __cplusplus
typedef class WebWizardHost WebWizardHost;
#else
typedef struct WebWizardHost WebWizardHost;
#endif /* __cplusplus */

#endif 	/* __WebWizardHost_FWD_DEFINED__ */


#ifndef __PublishDropTarget_FWD_DEFINED__
#define __PublishDropTarget_FWD_DEFINED__

#ifdef __cplusplus
typedef class PublishDropTarget PublishDropTarget;
#else
typedef struct PublishDropTarget PublishDropTarget;
#endif /* __cplusplus */

#endif 	/* __PublishDropTarget_FWD_DEFINED__ */


#ifndef __PublishingWizard_FWD_DEFINED__
#define __PublishingWizard_FWD_DEFINED__

#ifdef __cplusplus
typedef class PublishingWizard PublishingWizard;
#else
typedef struct PublishingWizard PublishingWizard;
#endif /* __cplusplus */

#endif 	/* __PublishingWizard_FWD_DEFINED__ */


#ifndef __InternetPrintOrdering_FWD_DEFINED__
#define __InternetPrintOrdering_FWD_DEFINED__

#ifdef __cplusplus
typedef class InternetPrintOrdering InternetPrintOrdering;
#else
typedef struct InternetPrintOrdering InternetPrintOrdering;
#endif /* __cplusplus */

#endif 	/* __InternetPrintOrdering_FWD_DEFINED__ */


#ifndef __FolderViewHost_FWD_DEFINED__
#define __FolderViewHost_FWD_DEFINED__

#ifdef __cplusplus
typedef class FolderViewHost FolderViewHost;
#else
typedef struct FolderViewHost FolderViewHost;
#endif /* __cplusplus */

#endif 	/* __FolderViewHost_FWD_DEFINED__ */


#ifndef __NamespaceWalker_FWD_DEFINED__
#define __NamespaceWalker_FWD_DEFINED__

#ifdef __cplusplus
typedef class NamespaceWalker NamespaceWalker;
#else
typedef struct NamespaceWalker NamespaceWalker;
#endif /* __cplusplus */

#endif 	/* __NamespaceWalker_FWD_DEFINED__ */


#ifndef __ImageRecompress_FWD_DEFINED__
#define __ImageRecompress_FWD_DEFINED__

#ifdef __cplusplus
typedef class ImageRecompress ImageRecompress;
#else
typedef struct ImageRecompress ImageRecompress;
#endif /* __cplusplus */

#endif 	/* __ImageRecompress_FWD_DEFINED__ */


#ifndef __TrayBandSiteService_FWD_DEFINED__
#define __TrayBandSiteService_FWD_DEFINED__

#ifdef __cplusplus
typedef class TrayBandSiteService TrayBandSiteService;
#else
typedef struct TrayBandSiteService TrayBandSiteService;
#endif /* __cplusplus */

#endif 	/* __TrayBandSiteService_FWD_DEFINED__ */


#ifndef __PassportWizard_FWD_DEFINED__
#define __PassportWizard_FWD_DEFINED__

#ifdef __cplusplus
typedef class PassportWizard PassportWizard;
#else
typedef struct PassportWizard PassportWizard;
#endif /* __cplusplus */

#endif 	/* __PassportWizard_FWD_DEFINED__ */


#ifndef __AttachmentServices_FWD_DEFINED__
#define __AttachmentServices_FWD_DEFINED__

#ifdef __cplusplus
typedef class AttachmentServices AttachmentServices;
#else
typedef struct AttachmentServices AttachmentServices;
#endif /* __cplusplus */

#endif 	/* __AttachmentServices_FWD_DEFINED__ */


/* header files for imported files */
#include "objidl.h"
#include "oleidl.h"
#include "oaidl.h"
#include "shtypes.h"
#include "servprov.h"
#include "comcat.h"
#include "propidl.h"
#include "prsht.h"
#include "msxml.h"

#ifdef __cplusplus
extern "C"{
#endif 

void * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void * ); 

/* interface __MIDL_itf_shobjidl_0000 */
/* [local] */ 

#ifndef _WINRESRC_
#ifndef _WIN32_IE
#define _WIN32_IE 0x0501
#else
#if (_WIN32_IE < 0x0400) && defined(_WIN32_WINNT) && (_WIN32_WINNT >= 0x0500)
#error _WIN32_IE setting conflicts with _WIN32_WINNT setting
#endif
#endif
#endif


extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0000_v0_0_s_ifspec;

#ifndef __IPersistFolder_INTERFACE_DEFINED__
#define __IPersistFolder_INTERFACE_DEFINED__

/* interface IPersistFolder */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IPersistFolder;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("000214EA-0000-0000-C000-000000000046")
    IPersistFolder : public IPersist
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Initialize( 
            /* [in] */ LPCITEMIDLIST pidl) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPersistFolderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPersistFolder * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPersistFolder * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPersistFolder * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetClassID )( 
            IPersistFolder * This,
            /* [out] */ CLSID *pClassID);
        
        HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            IPersistFolder * This,
            /* [in] */ LPCITEMIDLIST pidl);
        
        END_INTERFACE
    } IPersistFolderVtbl;

    interface IPersistFolder
    {
        CONST_VTBL struct IPersistFolderVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPersistFolder_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IPersistFolder_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IPersistFolder_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IPersistFolder_GetClassID(This,pClassID)	\
    (This)->lpVtbl -> GetClassID(This,pClassID)


#define IPersistFolder_Initialize(This,pidl)	\
    (This)->lpVtbl -> Initialize(This,pidl)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IPersistFolder_Initialize_Proxy( 
    IPersistFolder * This,
    /* [in] */ LPCITEMIDLIST pidl);


void __RPC_STUB IPersistFolder_Initialize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IPersistFolder_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0196 */
/* [local] */ 

typedef IPersistFolder *LPPERSISTFOLDER;

#if (_WIN32_IE >= 0x0400)


extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0196_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0196_v0_0_s_ifspec;

#ifndef __IPersistFolder2_INTERFACE_DEFINED__
#define __IPersistFolder2_INTERFACE_DEFINED__

/* interface IPersistFolder2 */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IPersistFolder2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1AC3D9F0-175C-11d1-95BE-00609797EA4F")
    IPersistFolder2 : public IPersistFolder
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetCurFolder( 
            /* [out] */ LPITEMIDLIST *ppidl) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPersistFolder2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPersistFolder2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPersistFolder2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPersistFolder2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetClassID )( 
            IPersistFolder2 * This,
            /* [out] */ CLSID *pClassID);
        
        HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            IPersistFolder2 * This,
            /* [in] */ LPCITEMIDLIST pidl);
        
        HRESULT ( STDMETHODCALLTYPE *GetCurFolder )( 
            IPersistFolder2 * This,
            /* [out] */ LPITEMIDLIST *ppidl);
        
        END_INTERFACE
    } IPersistFolder2Vtbl;

    interface IPersistFolder2
    {
        CONST_VTBL struct IPersistFolder2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPersistFolder2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IPersistFolder2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IPersistFolder2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IPersistFolder2_GetClassID(This,pClassID)	\
    (This)->lpVtbl -> GetClassID(This,pClassID)


#define IPersistFolder2_Initialize(This,pidl)	\
    (This)->lpVtbl -> Initialize(This,pidl)


#define IPersistFolder2_GetCurFolder(This,ppidl)	\
    (This)->lpVtbl -> GetCurFolder(This,ppidl)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IPersistFolder2_GetCurFolder_Proxy( 
    IPersistFolder2 * This,
    /* [out] */ LPITEMIDLIST *ppidl);


void __RPC_STUB IPersistFolder2_GetCurFolder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IPersistFolder2_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0197 */
/* [local] */ 

typedef IPersistFolder2 *LPPERSISTFOLDER2;

#endif


extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0197_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0197_v0_0_s_ifspec;

#ifndef __IPersistIDList_INTERFACE_DEFINED__
#define __IPersistIDList_INTERFACE_DEFINED__

/* interface IPersistIDList */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IPersistIDList;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1079acfc-29bd-11d3-8e0d-00c04f6837d5")
    IPersistIDList : public IPersist
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetIDList( 
            /* [in] */ LPCITEMIDLIST pidl) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetIDList( 
            /* [out] */ LPITEMIDLIST *ppidl) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPersistIDListVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPersistIDList * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPersistIDList * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPersistIDList * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetClassID )( 
            IPersistIDList * This,
            /* [out] */ CLSID *pClassID);
        
        HRESULT ( STDMETHODCALLTYPE *SetIDList )( 
            IPersistIDList * This,
            /* [in] */ LPCITEMIDLIST pidl);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDList )( 
            IPersistIDList * This,
            /* [out] */ LPITEMIDLIST *ppidl);
        
        END_INTERFACE
    } IPersistIDListVtbl;

    interface IPersistIDList
    {
        CONST_VTBL struct IPersistIDListVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPersistIDList_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IPersistIDList_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IPersistIDList_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IPersistIDList_GetClassID(This,pClassID)	\
    (This)->lpVtbl -> GetClassID(This,pClassID)


#define IPersistIDList_SetIDList(This,pidl)	\
    (This)->lpVtbl -> SetIDList(This,pidl)

#define IPersistIDList_GetIDList(This,ppidl)	\
    (This)->lpVtbl -> GetIDList(This,ppidl)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IPersistIDList_SetIDList_Proxy( 
    IPersistIDList * This,
    /* [in] */ LPCITEMIDLIST pidl);


void __RPC_STUB IPersistIDList_SetIDList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IPersistIDList_GetIDList_Proxy( 
    IPersistIDList * This,
    /* [out] */ LPITEMIDLIST *ppidl);


void __RPC_STUB IPersistIDList_GetIDList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IPersistIDList_INTERFACE_DEFINED__ */


#ifndef __IEnumIDList_INTERFACE_DEFINED__
#define __IEnumIDList_INTERFACE_DEFINED__

/* interface IEnumIDList */
/* [unique][object][uuid][helpstring] */ 


EXTERN_C const IID IID_IEnumIDList;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("000214F2-0000-0000-C000-000000000046")
    IEnumIDList : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ LPITEMIDLIST *rgelt,
            /* [out] */ ULONG *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumIDList **ppenum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumIDListVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumIDList * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumIDList * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumIDList * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumIDList * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ LPITEMIDLIST *rgelt,
            /* [out] */ ULONG *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumIDList * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumIDList * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumIDList * This,
            /* [out] */ IEnumIDList **ppenum);
        
        END_INTERFACE
    } IEnumIDListVtbl;

    interface IEnumIDList
    {
        CONST_VTBL struct IEnumIDListVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumIDList_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumIDList_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumIDList_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumIDList_Next(This,celt,rgelt,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#define IEnumIDList_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumIDList_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumIDList_Clone(This,ppenum)	\
    (This)->lpVtbl -> Clone(This,ppenum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumIDList_Next_Proxy( 
    IEnumIDList * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ LPITEMIDLIST *rgelt,
    /* [out] */ ULONG *pceltFetched);


void __RPC_STUB IEnumIDList_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumIDList_Skip_Proxy( 
    IEnumIDList * This,
    /* [in] */ ULONG celt);


void __RPC_STUB IEnumIDList_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumIDList_Reset_Proxy( 
    IEnumIDList * This);


void __RPC_STUB IEnumIDList_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumIDList_Clone_Proxy( 
    IEnumIDList * This,
    /* [out] */ IEnumIDList **ppenum);


void __RPC_STUB IEnumIDList_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumIDList_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0199 */
/* [local] */ 

typedef IEnumIDList *LPENUMIDLIST;

typedef enum tagSHGDN
{
    SHGDN_NORMAL             = 0x0000,  // default (display purpose)
    SHGDN_INFOLDER           = 0x0001,  // displayed under a folder (relative)
    SHGDN_FOREDITING         = 0x1000,  // for in-place editing
    SHGDN_FORADDRESSBAR      = 0x4000,  // UI friendly parsing name (remove ugly stuff)
    SHGDN_FORPARSING         = 0x8000,  // parsing name for ParseDisplayName()
} SHGNO;
typedef DWORD SHGDNF;

typedef enum tagSHCONTF
{
    SHCONTF_FOLDERS             = 0x0020,   // only want folders enumerated (SFGAO_FOLDER)
    SHCONTF_NONFOLDERS          = 0x0040,   // include non folders
    SHCONTF_INCLUDEHIDDEN       = 0x0080,   // show items normally hidden
    SHCONTF_INIT_ON_FIRST_NEXT  = 0x0100,   // allow EnumObject() to return before validating enum
    SHCONTF_NETPRINTERSRCH      = 0x0200,   // hint that client is looking for printers
    SHCONTF_SHAREABLE           = 0x0400,   // hint that client is looking sharable resources (remote shares)
    SHCONTF_STORAGE             = 0x0800,   // include all items with accessible storage and their ancestors
} __SHCONTF__;
typedef DWORD SHCONTF;

#define SHCIDS_ALLFIELDS        0x80000000L
#define SHCIDS_CANONICALONLY    0x10000000L
#define SHCIDS_BITMASK          0xFFFF0000L
#define SHCIDS_COLUMNMASK       0x0000FFFFL
#define SFGAO_CANCOPY           DROPEFFECT_COPY // Objects can be copied    (0x1)
#define SFGAO_CANMOVE           DROPEFFECT_MOVE // Objects can be moved     (0x2)
#define SFGAO_CANLINK           DROPEFFECT_LINK // Objects can be linked    (0x4)
#define SFGAO_STORAGE           0x00000008L     // supports BindToObject(IID_IStorage)
#define SFGAO_CANRENAME         0x00000010L     // Objects can be renamed
#define SFGAO_CANDELETE         0x00000020L     // Objects can be deleted
#define SFGAO_HASPROPSHEET      0x00000040L     // Objects have property sheets
#define SFGAO_DROPTARGET        0x00000100L     // Objects are drop target
#define SFGAO_CAPABILITYMASK    0x00000177L
#define SFGAO_ENCRYPTED         0x00002000L     // object is encrypted (use alt color)
#define SFGAO_ISSLOW            0x00004000L     // 'slow' object
#define SFGAO_GHOSTED           0x00008000L     // ghosted icon
#define SFGAO_LINK              0x00010000L     // Shortcut (link)
#define SFGAO_SHARE             0x00020000L     // shared
#define SFGAO_READONLY          0x00040000L     // read-only
#define SFGAO_HIDDEN            0x00080000L     // hidden object
#define SFGAO_DISPLAYATTRMASK   0x000FC000L
#define SFGAO_FILESYSANCESTOR   0x10000000L     // may contain children with SFGAO_FILESYSTEM
#define SFGAO_FOLDER            0x20000000L     // support BindToObject(IID_IShellFolder)
#define SFGAO_FILESYSTEM        0x40000000L     // is a win32 file system object (file/folder/root)
#define SFGAO_HASSUBFOLDER      0x80000000L     // may contain children with SFGAO_FOLDER
#define SFGAO_CONTENTSMASK      0x80000000L
#define SFGAO_VALIDATE          0x01000000L     // invalidate cached information
#define SFGAO_REMOVABLE         0x02000000L     // is this removeable media?
#define SFGAO_COMPRESSED        0x04000000L     // Object is compressed (use alt color)
#define SFGAO_BROWSABLE         0x08000000L     // supports IShellFolder, but only implements CreateViewObject() (non-folder view)
#define SFGAO_NONENUMERATED     0x00100000L     // is a non-enumerated object
#define SFGAO_NEWCONTENT        0x00200000L     // should show bold in explorer tree
#define SFGAO_CANMONIKER        0x00400000L     // defunct
#define SFGAO_HASSTORAGE        0x00400000L     // defunct
#define SFGAO_STREAM            0x00400000L     // supports BindToObject(IID_IStream)
#define SFGAO_STORAGEANCESTOR   0x00800000L     // may contain children with SFGAO_STORAGE or SFGAO_STREAM
#define SFGAO_STORAGECAPMASK    0x70C50008L     // for determining storage capabilities, ie for open/save semantics
typedef ULONG SFGAOF;

#define STR_SKIP_BINDING_CLSID      L"Skip Binding CLSID"
#define STR_PARSE_PREFER_FOLDER_BROWSING     L"Parse Prefer Folder Browsing"
#define STR_DONT_PARSE_RELATIVE              L"Don't Parse Relative"
#define STR_PARSE_TRANSLATE_ALIASES          L"Parse Translate Aliases"
#define STR_PARSE_SHELL_PROTOCOL_TO_FILE_OBJECTS     L"Parse Shell Protocol To File Objects"



extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0199_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0199_v0_0_s_ifspec;

#ifndef __IShellFolder_INTERFACE_DEFINED__
#define __IShellFolder_INTERFACE_DEFINED__

/* interface IShellFolder */
/* [unique][object][uuid][helpstring] */ 


EXTERN_C const IID IID_IShellFolder;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("000214E6-0000-0000-C000-000000000046")
    IShellFolder : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE ParseDisplayName( 
            /* [in] */ HWND hwnd,
            /* [in] */ LPBC pbc,
            /* [string][in] */ LPOLESTR pszDisplayName,
            /* [out] */ ULONG *pchEaten,
            /* [out] */ LPITEMIDLIST *ppidl,
            /* [unique][out][in] */ ULONG *pdwAttributes) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumObjects( 
            /* [in] */ HWND hwnd,
            /* [in] */ SHCONTF grfFlags,
            /* [out] */ IEnumIDList **ppenumIDList) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE BindToObject( 
            /* [in] */ LPCITEMIDLIST pidl,
            /* [in] */ LPBC pbc,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppv) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE BindToStorage( 
            /* [in] */ LPCITEMIDLIST pidl,
            /* [in] */ LPBC pbc,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppv) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CompareIDs( 
            /* [in] */ LPARAM lParam,
            /* [in] */ LPCITEMIDLIST pidl1,
            /* [in] */ LPCITEMIDLIST pidl2) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateViewObject( 
            /* [in] */ HWND hwndOwner,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppv) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAttributesOf( 
            /* [in] */ UINT cidl,
            /* [size_is][in] */ LPCITEMIDLIST *apidl,
            /* [out][in] */ SFGAOF *rgfInOut) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetUIObjectOf( 
            /* [in] */ HWND hwndOwner,
            /* [in] */ UINT cidl,
            /* [size_is][in] */ LPCITEMIDLIST *apidl,
            /* [in] */ REFIID riid,
            /* [unique][out][in] */ UINT *rgfReserved,
            /* [iid_is][out] */ void **ppv) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDisplayNameOf( 
            /* [in] */ LPCITEMIDLIST pidl,
            /* [in] */ SHGDNF uFlags,
            /* [out] */ STRRET *pName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetNameOf( 
            /* [in] */ HWND hwnd,
            /* [in] */ LPCITEMIDLIST pidl,
            /* [string][in] */ LPCOLESTR pszName,
            /* [in] */ SHGDNF uFlags,
            /* [out] */ LPITEMIDLIST *ppidlOut) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IShellFolderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IShellFolder * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IShellFolder * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IShellFolder * This);
        
        HRESULT ( STDMETHODCALLTYPE *ParseDisplayName )( 
            IShellFolder * This,
            /* [in] */ HWND hwnd,
            /* [in] */ LPBC pbc,
            /* [string][in] */ LPOLESTR pszDisplayName,
            /* [out] */ ULONG *pchEaten,
            /* [out] */ LPITEMIDLIST *ppidl,
            /* [unique][out][in] */ ULONG *pdwAttributes);
        
        HRESULT ( STDMETHODCALLTYPE *EnumObjects )( 
            IShellFolder * This,
            /* [in] */ HWND hwnd,
            /* [in] */ SHCONTF grfFlags,
            /* [out] */ IEnumIDList **ppenumIDList);
        
        HRESULT ( STDMETHODCALLTYPE *BindToObject )( 
            IShellFolder * This,
            /* [in] */ LPCITEMIDLIST pidl,
            /* [in] */ LPBC pbc,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppv);
        
        HRESULT ( STDMETHODCALLTYPE *BindToStorage )( 
            IShellFolder * This,
            /* [in] */ LPCITEMIDLIST pidl,
            /* [in] */ LPBC pbc,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppv);
        
        HRESULT ( STDMETHODCALLTYPE *CompareIDs )( 
            IShellFolder * This,
            /* [in] */ LPARAM lParam,
            /* [in] */ LPCITEMIDLIST pidl1,
            /* [in] */ LPCITEMIDLIST pidl2);
        
        HRESULT ( STDMETHODCALLTYPE *CreateViewObject )( 
            IShellFolder * This,
            /* [in] */ HWND hwndOwner,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppv);
        
        HRESULT ( STDMETHODCALLTYPE *GetAttributesOf )( 
            IShellFolder * This,
            /* [in] */ UINT cidl,
            /* [size_is][in] */ LPCITEMIDLIST *apidl,
            /* [out][in] */ SFGAOF *rgfInOut);
        
        HRESULT ( STDMETHODCALLTYPE *GetUIObjectOf )( 
            IShellFolder * This,
            /* [in] */ HWND hwndOwner,
            /* [in] */ UINT cidl,
            /* [size_is][in] */ LPCITEMIDLIST *apidl,
            /* [in] */ REFIID riid,
            /* [unique][out][in] */ UINT *rgfReserved,
            /* [iid_is][out] */ void **ppv);
        
        HRESULT ( STDMETHODCALLTYPE *GetDisplayNameOf )( 
            IShellFolder * This,
            /* [in] */ LPCITEMIDLIST pidl,
            /* [in] */ SHGDNF uFlags,
            /* [out] */ STRRET *pName);
        
        HRESULT ( STDMETHODCALLTYPE *SetNameOf )( 
            IShellFolder * This,
            /* [in] */ HWND hwnd,
            /* [in] */ LPCITEMIDLIST pidl,
            /* [string][in] */ LPCOLESTR pszName,
            /* [in] */ SHGDNF uFlags,
            /* [out] */ LPITEMIDLIST *ppidlOut);
        
        END_INTERFACE
    } IShellFolderVtbl;

    interface IShellFolder
    {
        CONST_VTBL struct IShellFolderVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IShellFolder_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IShellFolder_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IShellFolder_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IShellFolder_ParseDisplayName(This,hwnd,pbc,pszDisplayName,pchEaten,ppidl,pdwAttributes)	\
    (This)->lpVtbl -> ParseDisplayName(This,hwnd,pbc,pszDisplayName,pchEaten,ppidl,pdwAttributes)

#define IShellFolder_EnumObjects(This,hwnd,grfFlags,ppenumIDList)	\
    (This)->lpVtbl -> EnumObjects(This,hwnd,grfFlags,ppenumIDList)

#define IShellFolder_BindToObject(This,pidl,pbc,riid,ppv)	\
    (This)->lpVtbl -> BindToObject(This,pidl,pbc,riid,ppv)

#define IShellFolder_BindToStorage(This,pidl,pbc,riid,ppv)	\
    (This)->lpVtbl -> BindToStorage(This,pidl,pbc,riid,ppv)

#define IShellFolder_CompareIDs(This,lParam,pidl1,pidl2)	\
    (This)->lpVtbl -> CompareIDs(This,lParam,pidl1,pidl2)

#define IShellFolder_CreateViewObject(This,hwndOwner,riid,ppv)	\
    (This)->lpVtbl -> CreateViewObject(This,hwndOwner,riid,ppv)

#define IShellFolder_GetAttributesOf(This,cidl,apidl,rgfInOut)	\
    (This)->lpVtbl -> GetAttributesOf(This,cidl,apidl,rgfInOut)

#define IShellFolder_GetUIObjectOf(This,hwndOwner,cidl,apidl,riid,rgfReserved,ppv)	\
    (This)->lpVtbl -> GetUIObjectOf(This,hwndOwner,cidl,apidl,riid,rgfReserved,ppv)

#define IShellFolder_GetDisplayNameOf(This,pidl,uFlags,pName)	\
    (This)->lpVtbl -> GetDisplayNameOf(This,pidl,uFlags,pName)

#define IShellFolder_SetNameOf(This,hwnd,pidl,pszName,uFlags,ppidlOut)	\
    (This)->lpVtbl -> SetNameOf(This,hwnd,pidl,pszName,uFlags,ppidlOut)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IShellFolder_ParseDisplayName_Proxy( 
    IShellFolder * This,
    /* [in] */ HWND hwnd,
    /* [in] */ LPBC pbc,
    /* [string][in] */ LPOLESTR pszDisplayName,
    /* [out] */ ULONG *pchEaten,
    /* [out] */ LPITEMIDLIST *ppidl,
    /* [unique][out][in] */ ULONG *pdwAttributes);


void __RPC_STUB IShellFolder_ParseDisplayName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellFolder_EnumObjects_Proxy( 
    IShellFolder * This,
    /* [in] */ HWND hwnd,
    /* [in] */ SHCONTF grfFlags,
    /* [out] */ IEnumIDList **ppenumIDList);


void __RPC_STUB IShellFolder_EnumObjects_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellFolder_BindToObject_Proxy( 
    IShellFolder * This,
    /* [in] */ LPCITEMIDLIST pidl,
    /* [in] */ LPBC pbc,
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ void **ppv);


void __RPC_STUB IShellFolder_BindToObject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellFolder_BindToStorage_Proxy( 
    IShellFolder * This,
    /* [in] */ LPCITEMIDLIST pidl,
    /* [in] */ LPBC pbc,
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ void **ppv);


void __RPC_STUB IShellFolder_BindToStorage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellFolder_CompareIDs_Proxy( 
    IShellFolder * This,
    /* [in] */ LPARAM lParam,
    /* [in] */ LPCITEMIDLIST pidl1,
    /* [in] */ LPCITEMIDLIST pidl2);


void __RPC_STUB IShellFolder_CompareIDs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellFolder_CreateViewObject_Proxy( 
    IShellFolder * This,
    /* [in] */ HWND hwndOwner,
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ void **ppv);


void __RPC_STUB IShellFolder_CreateViewObject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellFolder_GetAttributesOf_Proxy( 
    IShellFolder * This,
    /* [in] */ UINT cidl,
    /* [size_is][in] */ LPCITEMIDLIST *apidl,
    /* [out][in] */ SFGAOF *rgfInOut);


void __RPC_STUB IShellFolder_GetAttributesOf_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellFolder_GetUIObjectOf_Proxy( 
    IShellFolder * This,
    /* [in] */ HWND hwndOwner,
    /* [in] */ UINT cidl,
    /* [size_is][in] */ LPCITEMIDLIST *apidl,
    /* [in] */ REFIID riid,
    /* [unique][out][in] */ UINT *rgfReserved,
    /* [iid_is][out] */ void **ppv);


void __RPC_STUB IShellFolder_GetUIObjectOf_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellFolder_GetDisplayNameOf_Proxy( 
    IShellFolder * This,
    /* [in] */ LPCITEMIDLIST pidl,
    /* [in] */ SHGDNF uFlags,
    /* [out] */ STRRET *pName);


void __RPC_STUB IShellFolder_GetDisplayNameOf_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellFolder_SetNameOf_Proxy( 
    IShellFolder * This,
    /* [in] */ HWND hwnd,
    /* [in] */ LPCITEMIDLIST pidl,
    /* [string][in] */ LPCOLESTR pszName,
    /* [in] */ SHGDNF uFlags,
    /* [out] */ LPITEMIDLIST *ppidlOut);


void __RPC_STUB IShellFolder_SetNameOf_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IShellFolder_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0200 */
/* [local] */ 

typedef IShellFolder *LPSHELLFOLDER;

#if (_WIN32_IE >= 0x0500)
typedef struct tagEXTRASEARCH
    {
    GUID guidSearch;
    WCHAR wszFriendlyName[ 80 ];
    WCHAR wszUrl[ 2084 ];
    } 	EXTRASEARCH;

typedef struct tagEXTRASEARCH *LPEXTRASEARCH;

typedef struct IEnumExtraSearch *LPENUMEXTRASEARCH;




extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0200_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0200_v0_0_s_ifspec;

#ifndef __IEnumExtraSearch_INTERFACE_DEFINED__
#define __IEnumExtraSearch_INTERFACE_DEFINED__

/* interface IEnumExtraSearch */
/* [unique][object][uuid][helpstring] */ 


EXTERN_C const IID IID_IEnumExtraSearch;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0E700BE1-9DB6-11d1-A1CE-00C04FD75D13")
    IEnumExtraSearch : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Next( 
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ EXTRASEARCH *rgelt,
            /* [out] */ ULONG *pceltFetched) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Skip( 
            /* [in] */ ULONG celt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Reset( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Clone( 
            /* [out] */ IEnumExtraSearch **ppenum) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IEnumExtraSearchVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IEnumExtraSearch * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IEnumExtraSearch * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IEnumExtraSearch * This);
        
        HRESULT ( STDMETHODCALLTYPE *Next )( 
            IEnumExtraSearch * This,
            /* [in] */ ULONG celt,
            /* [length_is][size_is][out] */ EXTRASEARCH *rgelt,
            /* [out] */ ULONG *pceltFetched);
        
        HRESULT ( STDMETHODCALLTYPE *Skip )( 
            IEnumExtraSearch * This,
            /* [in] */ ULONG celt);
        
        HRESULT ( STDMETHODCALLTYPE *Reset )( 
            IEnumExtraSearch * This);
        
        HRESULT ( STDMETHODCALLTYPE *Clone )( 
            IEnumExtraSearch * This,
            /* [out] */ IEnumExtraSearch **ppenum);
        
        END_INTERFACE
    } IEnumExtraSearchVtbl;

    interface IEnumExtraSearch
    {
        CONST_VTBL struct IEnumExtraSearchVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IEnumExtraSearch_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IEnumExtraSearch_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IEnumExtraSearch_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IEnumExtraSearch_Next(This,celt,rgelt,pceltFetched)	\
    (This)->lpVtbl -> Next(This,celt,rgelt,pceltFetched)

#define IEnumExtraSearch_Skip(This,celt)	\
    (This)->lpVtbl -> Skip(This,celt)

#define IEnumExtraSearch_Reset(This)	\
    (This)->lpVtbl -> Reset(This)

#define IEnumExtraSearch_Clone(This,ppenum)	\
    (This)->lpVtbl -> Clone(This,ppenum)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IEnumExtraSearch_Next_Proxy( 
    IEnumExtraSearch * This,
    /* [in] */ ULONG celt,
    /* [length_is][size_is][out] */ EXTRASEARCH *rgelt,
    /* [out] */ ULONG *pceltFetched);


void __RPC_STUB IEnumExtraSearch_Next_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumExtraSearch_Skip_Proxy( 
    IEnumExtraSearch * This,
    /* [in] */ ULONG celt);


void __RPC_STUB IEnumExtraSearch_Skip_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumExtraSearch_Reset_Proxy( 
    IEnumExtraSearch * This);


void __RPC_STUB IEnumExtraSearch_Reset_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IEnumExtraSearch_Clone_Proxy( 
    IEnumExtraSearch * This,
    /* [out] */ IEnumExtraSearch **ppenum);


void __RPC_STUB IEnumExtraSearch_Clone_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IEnumExtraSearch_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0201 */
/* [local] */ 

typedef /* [public][v1_enum] */ 
enum __MIDL___MIDL_itf_shobjidl_0201_0001
    {	SHCOLSTATE_TYPE_STR	= 0x1,
	SHCOLSTATE_TYPE_INT	= 0x2,
	SHCOLSTATE_TYPE_DATE	= 0x3,
	SHCOLSTATE_TYPEMASK	= 0xf,
	SHCOLSTATE_ONBYDEFAULT	= 0x10,
	SHCOLSTATE_SLOW	= 0x20,
	SHCOLSTATE_EXTENDED	= 0x40,
	SHCOLSTATE_SECONDARYUI	= 0x80,
	SHCOLSTATE_HIDDEN	= 0x100,
	SHCOLSTATE_PREFER_VARCMP	= 0x200
    } 	SHCOLSTATE;

typedef DWORD SHCOLSTATEF;

typedef /* [public][public][public][public][public][public][public] */ struct __MIDL___MIDL_itf_shobjidl_0201_0002
    {
    GUID fmtid;
    DWORD pid;
    } 	SHCOLUMNID;

typedef struct __MIDL___MIDL_itf_shobjidl_0201_0002 *LPSHCOLUMNID;

typedef const SHCOLUMNID *LPCSHCOLUMNID;



extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0201_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0201_v0_0_s_ifspec;

#ifndef __IShellFolder2_INTERFACE_DEFINED__
#define __IShellFolder2_INTERFACE_DEFINED__

/* interface IShellFolder2 */
/* [unique][object][uuid][helpstring] */ 


EXTERN_C const IID IID_IShellFolder2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("93F2F68C-1D1B-11d3-A30E-00C04F79ABD1")
    IShellFolder2 : public IShellFolder
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetDefaultSearchGUID( 
            /* [out] */ GUID *pguid) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumSearches( 
            /* [out] */ IEnumExtraSearch **ppenum) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDefaultColumn( 
            /* [in] */ DWORD dwRes,
            /* [out] */ ULONG *pSort,
            /* [out] */ ULONG *pDisplay) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDefaultColumnState( 
            /* [in] */ UINT iColumn,
            /* [out] */ SHCOLSTATEF *pcsFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDetailsEx( 
            /* [in] */ LPCITEMIDLIST pidl,
            /* [in] */ const SHCOLUMNID *pscid,
            /* [out] */ VARIANT *pv) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDetailsOf( 
            /* [in] */ LPCITEMIDLIST pidl,
            /* [in] */ UINT iColumn,
            /* [out] */ SHELLDETAILS *psd) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE MapColumnToSCID( 
            /* [in] */ UINT iColumn,
            /* [in] */ SHCOLUMNID *pscid) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IShellFolder2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IShellFolder2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IShellFolder2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IShellFolder2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *ParseDisplayName )( 
            IShellFolder2 * This,
            /* [in] */ HWND hwnd,
            /* [in] */ LPBC pbc,
            /* [string][in] */ LPOLESTR pszDisplayName,
            /* [out] */ ULONG *pchEaten,
            /* [out] */ LPITEMIDLIST *ppidl,
            /* [unique][out][in] */ ULONG *pdwAttributes);
        
        HRESULT ( STDMETHODCALLTYPE *EnumObjects )( 
            IShellFolder2 * This,
            /* [in] */ HWND hwnd,
            /* [in] */ SHCONTF grfFlags,
            /* [out] */ IEnumIDList **ppenumIDList);
        
        HRESULT ( STDMETHODCALLTYPE *BindToObject )( 
            IShellFolder2 * This,
            /* [in] */ LPCITEMIDLIST pidl,
            /* [in] */ LPBC pbc,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppv);
        
        HRESULT ( STDMETHODCALLTYPE *BindToStorage )( 
            IShellFolder2 * This,
            /* [in] */ LPCITEMIDLIST pidl,
            /* [in] */ LPBC pbc,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppv);
        
        HRESULT ( STDMETHODCALLTYPE *CompareIDs )( 
            IShellFolder2 * This,
            /* [in] */ LPARAM lParam,
            /* [in] */ LPCITEMIDLIST pidl1,
            /* [in] */ LPCITEMIDLIST pidl2);
        
        HRESULT ( STDMETHODCALLTYPE *CreateViewObject )( 
            IShellFolder2 * This,
            /* [in] */ HWND hwndOwner,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppv);
        
        HRESULT ( STDMETHODCALLTYPE *GetAttributesOf )( 
            IShellFolder2 * This,
            /* [in] */ UINT cidl,
            /* [size_is][in] */ LPCITEMIDLIST *apidl,
            /* [out][in] */ SFGAOF *rgfInOut);
        
        HRESULT ( STDMETHODCALLTYPE *GetUIObjectOf )( 
            IShellFolder2 * This,
            /* [in] */ HWND hwndOwner,
            /* [in] */ UINT cidl,
            /* [size_is][in] */ LPCITEMIDLIST *apidl,
            /* [in] */ REFIID riid,
            /* [unique][out][in] */ UINT *rgfReserved,
            /* [iid_is][out] */ void **ppv);
        
        HRESULT ( STDMETHODCALLTYPE *GetDisplayNameOf )( 
            IShellFolder2 * This,
            /* [in] */ LPCITEMIDLIST pidl,
            /* [in] */ SHGDNF uFlags,
            /* [out] */ STRRET *pName);
        
        HRESULT ( STDMETHODCALLTYPE *SetNameOf )( 
            IShellFolder2 * This,
            /* [in] */ HWND hwnd,
            /* [in] */ LPCITEMIDLIST pidl,
            /* [string][in] */ LPCOLESTR pszName,
            /* [in] */ SHGDNF uFlags,
            /* [out] */ LPITEMIDLIST *ppidlOut);
        
        HRESULT ( STDMETHODCALLTYPE *GetDefaultSearchGUID )( 
            IShellFolder2 * This,
            /* [out] */ GUID *pguid);
        
        HRESULT ( STDMETHODCALLTYPE *EnumSearches )( 
            IShellFolder2 * This,
            /* [out] */ IEnumExtraSearch **ppenum);
        
        HRESULT ( STDMETHODCALLTYPE *GetDefaultColumn )( 
            IShellFolder2 * This,
            /* [in] */ DWORD dwRes,
            /* [out] */ ULONG *pSort,
            /* [out] */ ULONG *pDisplay);
        
        HRESULT ( STDMETHODCALLTYPE *GetDefaultColumnState )( 
            IShellFolder2 * This,
            /* [in] */ UINT iColumn,
            /* [out] */ SHCOLSTATEF *pcsFlags);
        
        HRESULT ( STDMETHODCALLTYPE *GetDetailsEx )( 
            IShellFolder2 * This,
            /* [in] */ LPCITEMIDLIST pidl,
            /* [in] */ const SHCOLUMNID *pscid,
            /* [out] */ VARIANT *pv);
        
        HRESULT ( STDMETHODCALLTYPE *GetDetailsOf )( 
            IShellFolder2 * This,
            /* [in] */ LPCITEMIDLIST pidl,
            /* [in] */ UINT iColumn,
            /* [out] */ SHELLDETAILS *psd);
        
        HRESULT ( STDMETHODCALLTYPE *MapColumnToSCID )( 
            IShellFolder2 * This,
            /* [in] */ UINT iColumn,
            /* [in] */ SHCOLUMNID *pscid);
        
        END_INTERFACE
    } IShellFolder2Vtbl;

    interface IShellFolder2
    {
        CONST_VTBL struct IShellFolder2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IShellFolder2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IShellFolder2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IShellFolder2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IShellFolder2_ParseDisplayName(This,hwnd,pbc,pszDisplayName,pchEaten,ppidl,pdwAttributes)	\
    (This)->lpVtbl -> ParseDisplayName(This,hwnd,pbc,pszDisplayName,pchEaten,ppidl,pdwAttributes)

#define IShellFolder2_EnumObjects(This,hwnd,grfFlags,ppenumIDList)	\
    (This)->lpVtbl -> EnumObjects(This,hwnd,grfFlags,ppenumIDList)

#define IShellFolder2_BindToObject(This,pidl,pbc,riid,ppv)	\
    (This)->lpVtbl -> BindToObject(This,pidl,pbc,riid,ppv)

#define IShellFolder2_BindToStorage(This,pidl,pbc,riid,ppv)	\
    (This)->lpVtbl -> BindToStorage(This,pidl,pbc,riid,ppv)

#define IShellFolder2_CompareIDs(This,lParam,pidl1,pidl2)	\
    (This)->lpVtbl -> CompareIDs(This,lParam,pidl1,pidl2)

#define IShellFolder2_CreateViewObject(This,hwndOwner,riid,ppv)	\
    (This)->lpVtbl -> CreateViewObject(This,hwndOwner,riid,ppv)

#define IShellFolder2_GetAttributesOf(This,cidl,apidl,rgfInOut)	\
    (This)->lpVtbl -> GetAttributesOf(This,cidl,apidl,rgfInOut)

#define IShellFolder2_GetUIObjectOf(This,hwndOwner,cidl,apidl,riid,rgfReserved,ppv)	\
    (This)->lpVtbl -> GetUIObjectOf(This,hwndOwner,cidl,apidl,riid,rgfReserved,ppv)

#define IShellFolder2_GetDisplayNameOf(This,pidl,uFlags,pName)	\
    (This)->lpVtbl -> GetDisplayNameOf(This,pidl,uFlags,pName)

#define IShellFolder2_SetNameOf(This,hwnd,pidl,pszName,uFlags,ppidlOut)	\
    (This)->lpVtbl -> SetNameOf(This,hwnd,pidl,pszName,uFlags,ppidlOut)


#define IShellFolder2_GetDefaultSearchGUID(This,pguid)	\
    (This)->lpVtbl -> GetDefaultSearchGUID(This,pguid)

#define IShellFolder2_EnumSearches(This,ppenum)	\
    (This)->lpVtbl -> EnumSearches(This,ppenum)

#define IShellFolder2_GetDefaultColumn(This,dwRes,pSort,pDisplay)	\
    (This)->lpVtbl -> GetDefaultColumn(This,dwRes,pSort,pDisplay)

#define IShellFolder2_GetDefaultColumnState(This,iColumn,pcsFlags)	\
    (This)->lpVtbl -> GetDefaultColumnState(This,iColumn,pcsFlags)

#define IShellFolder2_GetDetailsEx(This,pidl,pscid,pv)	\
    (This)->lpVtbl -> GetDetailsEx(This,pidl,pscid,pv)

#define IShellFolder2_GetDetailsOf(This,pidl,iColumn,psd)	\
    (This)->lpVtbl -> GetDetailsOf(This,pidl,iColumn,psd)

#define IShellFolder2_MapColumnToSCID(This,iColumn,pscid)	\
    (This)->lpVtbl -> MapColumnToSCID(This,iColumn,pscid)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IShellFolder2_GetDefaultSearchGUID_Proxy( 
    IShellFolder2 * This,
    /* [out] */ GUID *pguid);


void __RPC_STUB IShellFolder2_GetDefaultSearchGUID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellFolder2_EnumSearches_Proxy( 
    IShellFolder2 * This,
    /* [out] */ IEnumExtraSearch **ppenum);


void __RPC_STUB IShellFolder2_EnumSearches_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellFolder2_GetDefaultColumn_Proxy( 
    IShellFolder2 * This,
    /* [in] */ DWORD dwRes,
    /* [out] */ ULONG *pSort,
    /* [out] */ ULONG *pDisplay);


void __RPC_STUB IShellFolder2_GetDefaultColumn_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellFolder2_GetDefaultColumnState_Proxy( 
    IShellFolder2 * This,
    /* [in] */ UINT iColumn,
    /* [out] */ SHCOLSTATEF *pcsFlags);


void __RPC_STUB IShellFolder2_GetDefaultColumnState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellFolder2_GetDetailsEx_Proxy( 
    IShellFolder2 * This,
    /* [in] */ LPCITEMIDLIST pidl,
    /* [in] */ const SHCOLUMNID *pscid,
    /* [out] */ VARIANT *pv);


void __RPC_STUB IShellFolder2_GetDetailsEx_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellFolder2_GetDetailsOf_Proxy( 
    IShellFolder2 * This,
    /* [in] */ LPCITEMIDLIST pidl,
    /* [in] */ UINT iColumn,
    /* [out] */ SHELLDETAILS *psd);


void __RPC_STUB IShellFolder2_GetDetailsOf_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellFolder2_MapColumnToSCID_Proxy( 
    IShellFolder2 * This,
    /* [in] */ UINT iColumn,
    /* [in] */ SHCOLUMNID *pscid);


void __RPC_STUB IShellFolder2_MapColumnToSCID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IShellFolder2_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0202 */
/* [local] */ 

#endif // _WIN32_IE >= 0x0500)
typedef char *LPVIEWSETTINGS;

typedef /* [public][v1_enum] */ 
enum __MIDL___MIDL_itf_shobjidl_0202_0001
    {	FWF_AUTOARRANGE	= 0x1,
	FWF_ABBREVIATEDNAMES	= 0x2,
	FWF_SNAPTOGRID	= 0x4,
	FWF_OWNERDATA	= 0x8,
	FWF_BESTFITWINDOW	= 0x10,
	FWF_DESKTOP	= 0x20,
	FWF_SINGLESEL	= 0x40,
	FWF_NOSUBFOLDERS	= 0x80,
	FWF_TRANSPARENT	= 0x100,
	FWF_NOCLIENTEDGE	= 0x200,
	FWF_NOSCROLL	= 0x400,
	FWF_ALIGNLEFT	= 0x800,
	FWF_NOICONS	= 0x1000,
	FWF_SHOWSELALWAYS	= 0x2000,
	FWF_NOVISIBLE	= 0x4000,
	FWF_SINGLECLICKACTIVATE	= 0x8000,
	FWF_NOWEBVIEW	= 0x10000,
	FWF_HIDEFILENAMES	= 0x20000,
	FWF_CHECKSELECT	= 0x40000
    } 	FOLDERFLAGS;

typedef /* [public][v1_enum] */ 
enum __MIDL___MIDL_itf_shobjidl_0202_0002
    {	FVM_FIRST	= 1,
	FVM_ICON	= 1,
	FVM_SMALLICON	= 2,
	FVM_LIST	= 3,
	FVM_DETAILS	= 4,
	FVM_THUMBNAIL	= 5,
	FVM_TILE	= 6,
	FVM_THUMBSTRIP	= 7,
	FVM_LAST	= 7
    } 	FOLDERVIEWMODE;

typedef /* [public][public][public][public][public][public][public][public][public] */ struct __MIDL___MIDL_itf_shobjidl_0202_0003
    {
    UINT ViewMode;
    UINT fFlags;
    } 	FOLDERSETTINGS;

typedef FOLDERSETTINGS *LPFOLDERSETTINGS;

typedef const FOLDERSETTINGS *LPCFOLDERSETTINGS;

typedef FOLDERSETTINGS *PFOLDERSETTINGS;

#define SVSI_DESELECT       0x00000000
#define SVSI_SELECT         0x00000001
#define SVSI_EDIT           0x00000003  // includes select
#define SVSI_DESELECTOTHERS 0x00000004
#define SVSI_ENSUREVISIBLE  0x00000008
#define SVSI_FOCUSED        0x00000010
#define SVSI_TRANSLATEPT    0x00000020
#define SVSI_SELECTIONMARK  0x00000040
#define SVSI_POSITIONITEM   0x00000080
#define SVSI_CHECK          0x00000100
#define SVSI_NOSTATECHANGE  0x80000000
typedef UINT SVSIF;

typedef /* [public][v1_enum] */ 
enum __MIDL___MIDL_itf_shobjidl_0202_0004
    {	SVGIO_BACKGROUND	= 0,
	SVGIO_SELECTION	= 0x1,
	SVGIO_ALLVIEW	= 0x2,
	SVGIO_CHECKED	= 0x3,
	SVGIO_TYPE_MASK	= 0xf,
	SVGIO_FLAG_VIEWORDER	= 0x80000000
    } 	SVGIO;

typedef /* [public][v1_enum] */ 
enum __MIDL___MIDL_itf_shobjidl_0202_0005
    {	SVUIA_DEACTIVATE	= 0,
	SVUIA_ACTIVATE_NOFOCUS	= 1,
	SVUIA_ACTIVATE_FOCUS	= 2,
	SVUIA_INPLACEACTIVATE	= 3
    } 	SVUIA_STATUS;

#ifdef _FIX_ENABLEMODELESS_CONFLICT
#define    EnableModeless EnableModelessSV
#endif
#ifdef _NEVER_
typedef LPARAM LPFNSVADDPROPSHEETPAGE;

#else //!_NEVER_
#include <prsht.h>
typedef LPFNADDPROPSHEETPAGE LPFNSVADDPROPSHEETPAGE;
#endif //_NEVER_



extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0202_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0202_v0_0_s_ifspec;

#ifndef __IShellView_INTERFACE_DEFINED__
#define __IShellView_INTERFACE_DEFINED__

/* interface IShellView */
/* [unique][object][uuid][helpstring] */ 

typedef IShellView *LPSHELLVIEW;


EXTERN_C const IID IID_IShellView;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("000214E3-0000-0000-C000-000000000046")
    IShellView : public IOleWindow
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE TranslateAccelerator( 
            /* [in] */ MSG *pmsg) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnableModeless( 
            /* [in] */ BOOL fEnable) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UIActivate( 
            /* [in] */ UINT uState) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Refresh( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateViewWindow( 
            /* [in] */ IShellView *psvPrevious,
            /* [in] */ LPCFOLDERSETTINGS pfs,
            /* [in] */ IShellBrowser *psb,
            /* [out] */ RECT *prcView,
            /* [out] */ HWND *phWnd) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DestroyViewWindow( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCurrentInfo( 
            /* [out] */ LPFOLDERSETTINGS pfs) = 0;
        
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE AddPropertySheetPages( 
            /* [in] */ DWORD dwReserved,
            /* [in] */ LPFNSVADDPROPSHEETPAGE pfn,
            /* [in] */ LPARAM lparam) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SaveViewState( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SelectItem( 
            /* [in] */ LPCITEMIDLIST pidlItem,
            /* [in] */ SVSIF uFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetItemObject( 
            /* [in] */ UINT uItem,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppv) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IShellViewVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IShellView * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IShellView * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IShellView * This);
        
        /* [input_sync] */ HRESULT ( STDMETHODCALLTYPE *GetWindow )( 
            IShellView * This,
            /* [out] */ HWND *phwnd);
        
        HRESULT ( STDMETHODCALLTYPE *ContextSensitiveHelp )( 
            IShellView * This,
            /* [in] */ BOOL fEnterMode);
        
        HRESULT ( STDMETHODCALLTYPE *TranslateAccelerator )( 
            IShellView * This,
            /* [in] */ MSG *pmsg);
        
        HRESULT ( STDMETHODCALLTYPE *EnableModeless )( 
            IShellView * This,
            /* [in] */ BOOL fEnable);
        
        HRESULT ( STDMETHODCALLTYPE *UIActivate )( 
            IShellView * This,
            /* [in] */ UINT uState);
        
        HRESULT ( STDMETHODCALLTYPE *Refresh )( 
            IShellView * This);
        
        HRESULT ( STDMETHODCALLTYPE *CreateViewWindow )( 
            IShellView * This,
            /* [in] */ IShellView *psvPrevious,
            /* [in] */ LPCFOLDERSETTINGS pfs,
            /* [in] */ IShellBrowser *psb,
            /* [out] */ RECT *prcView,
            /* [out] */ HWND *phWnd);
        
        HRESULT ( STDMETHODCALLTYPE *DestroyViewWindow )( 
            IShellView * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetCurrentInfo )( 
            IShellView * This,
            /* [out] */ LPFOLDERSETTINGS pfs);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *AddPropertySheetPages )( 
            IShellView * This,
            /* [in] */ DWORD dwReserved,
            /* [in] */ LPFNSVADDPROPSHEETPAGE pfn,
            /* [in] */ LPARAM lparam);
        
        HRESULT ( STDMETHODCALLTYPE *SaveViewState )( 
            IShellView * This);
        
        HRESULT ( STDMETHODCALLTYPE *SelectItem )( 
            IShellView * This,
            /* [in] */ LPCITEMIDLIST pidlItem,
            /* [in] */ SVSIF uFlags);
        
        HRESULT ( STDMETHODCALLTYPE *GetItemObject )( 
            IShellView * This,
            /* [in] */ UINT uItem,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppv);
        
        END_INTERFACE
    } IShellViewVtbl;

    interface IShellView
    {
        CONST_VTBL struct IShellViewVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IShellView_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IShellView_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IShellView_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IShellView_GetWindow(This,phwnd)	\
    (This)->lpVtbl -> GetWindow(This,phwnd)

#define IShellView_ContextSensitiveHelp(This,fEnterMode)	\
    (This)->lpVtbl -> ContextSensitiveHelp(This,fEnterMode)


#define IShellView_TranslateAccelerator(This,pmsg)	\
    (This)->lpVtbl -> TranslateAccelerator(This,pmsg)

#define IShellView_EnableModeless(This,fEnable)	\
    (This)->lpVtbl -> EnableModeless(This,fEnable)

#define IShellView_UIActivate(This,uState)	\
    (This)->lpVtbl -> UIActivate(This,uState)

#define IShellView_Refresh(This)	\
    (This)->lpVtbl -> Refresh(This)

#define IShellView_CreateViewWindow(This,psvPrevious,pfs,psb,prcView,phWnd)	\
    (This)->lpVtbl -> CreateViewWindow(This,psvPrevious,pfs,psb,prcView,phWnd)

#define IShellView_DestroyViewWindow(This)	\
    (This)->lpVtbl -> DestroyViewWindow(This)

#define IShellView_GetCurrentInfo(This,pfs)	\
    (This)->lpVtbl -> GetCurrentInfo(This,pfs)

#define IShellView_AddPropertySheetPages(This,dwReserved,pfn,lparam)	\
    (This)->lpVtbl -> AddPropertySheetPages(This,dwReserved,pfn,lparam)

#define IShellView_SaveViewState(This)	\
    (This)->lpVtbl -> SaveViewState(This)

#define IShellView_SelectItem(This,pidlItem,uFlags)	\
    (This)->lpVtbl -> SelectItem(This,pidlItem,uFlags)

#define IShellView_GetItemObject(This,uItem,riid,ppv)	\
    (This)->lpVtbl -> GetItemObject(This,uItem,riid,ppv)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IShellView_TranslateAccelerator_Proxy( 
    IShellView * This,
    /* [in] */ MSG *pmsg);


void __RPC_STUB IShellView_TranslateAccelerator_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellView_EnableModeless_Proxy( 
    IShellView * This,
    /* [in] */ BOOL fEnable);


void __RPC_STUB IShellView_EnableModeless_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellView_UIActivate_Proxy( 
    IShellView * This,
    /* [in] */ UINT uState);


void __RPC_STUB IShellView_UIActivate_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellView_Refresh_Proxy( 
    IShellView * This);


void __RPC_STUB IShellView_Refresh_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellView_CreateViewWindow_Proxy( 
    IShellView * This,
    /* [in] */ IShellView *psvPrevious,
    /* [in] */ LPCFOLDERSETTINGS pfs,
    /* [in] */ IShellBrowser *psb,
    /* [out] */ RECT *prcView,
    /* [out] */ HWND *phWnd);


void __RPC_STUB IShellView_CreateViewWindow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellView_DestroyViewWindow_Proxy( 
    IShellView * This);


void __RPC_STUB IShellView_DestroyViewWindow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellView_GetCurrentInfo_Proxy( 
    IShellView * This,
    /* [out] */ LPFOLDERSETTINGS pfs);


void __RPC_STUB IShellView_GetCurrentInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [local] */ HRESULT STDMETHODCALLTYPE IShellView_AddPropertySheetPages_Proxy( 
    IShellView * This,
    /* [in] */ DWORD dwReserved,
    /* [in] */ LPFNSVADDPROPSHEETPAGE pfn,
    /* [in] */ LPARAM lparam);


void __RPC_STUB IShellView_AddPropertySheetPages_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellView_SaveViewState_Proxy( 
    IShellView * This);


void __RPC_STUB IShellView_SaveViewState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellView_SelectItem_Proxy( 
    IShellView * This,
    /* [in] */ LPCITEMIDLIST pidlItem,
    /* [in] */ SVSIF uFlags);


void __RPC_STUB IShellView_SelectItem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellView_GetItemObject_Proxy( 
    IShellView * This,
    /* [in] */ UINT uItem,
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ void **ppv);


void __RPC_STUB IShellView_GetItemObject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IShellView_INTERFACE_DEFINED__ */


#ifndef __IShellView2_INTERFACE_DEFINED__
#define __IShellView2_INTERFACE_DEFINED__

/* interface IShellView2 */
/* [unique][object][uuid][helpstring] */ 

typedef GUID SHELLVIEWID;

#define SV2GV_CURRENTVIEW ((UINT)-1)
#define SV2GV_DEFAULTVIEW ((UINT)-2)
#include <pshpack8.h>
typedef struct _SV2CVW2_PARAMS
    {
    DWORD cbSize;
    IShellView *psvPrev;
    LPCFOLDERSETTINGS pfs;
    IShellBrowser *psbOwner;
    RECT *prcView;
    const SHELLVIEWID *pvid;
    HWND hwndView;
    } 	SV2CVW2_PARAMS;

typedef struct _SV2CVW2_PARAMS *LPSV2CVW2_PARAMS;

#include <poppack.h>

EXTERN_C const IID IID_IShellView2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("88E39E80-3578-11CF-AE69-08002B2E1262")
    IShellView2 : public IShellView
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetView( 
            /* [out][in] */ SHELLVIEWID *pvid,
            /* [in] */ ULONG uView) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateViewWindow2( 
            /* [in] */ LPSV2CVW2_PARAMS lpParams) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE HandleRename( 
            /* [in] */ LPCITEMIDLIST pidlNew) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SelectAndPositionItem( 
            /* [in] */ LPCITEMIDLIST pidlItem,
            /* [in] */ UINT uFlags,
            /* [in] */ POINT *ppt) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IShellView2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IShellView2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IShellView2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IShellView2 * This);
        
        /* [input_sync] */ HRESULT ( STDMETHODCALLTYPE *GetWindow )( 
            IShellView2 * This,
            /* [out] */ HWND *phwnd);
        
        HRESULT ( STDMETHODCALLTYPE *ContextSensitiveHelp )( 
            IShellView2 * This,
            /* [in] */ BOOL fEnterMode);
        
        HRESULT ( STDMETHODCALLTYPE *TranslateAccelerator )( 
            IShellView2 * This,
            /* [in] */ MSG *pmsg);
        
        HRESULT ( STDMETHODCALLTYPE *EnableModeless )( 
            IShellView2 * This,
            /* [in] */ BOOL fEnable);
        
        HRESULT ( STDMETHODCALLTYPE *UIActivate )( 
            IShellView2 * This,
            /* [in] */ UINT uState);
        
        HRESULT ( STDMETHODCALLTYPE *Refresh )( 
            IShellView2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *CreateViewWindow )( 
            IShellView2 * This,
            /* [in] */ IShellView *psvPrevious,
            /* [in] */ LPCFOLDERSETTINGS pfs,
            /* [in] */ IShellBrowser *psb,
            /* [out] */ RECT *prcView,
            /* [out] */ HWND *phWnd);
        
        HRESULT ( STDMETHODCALLTYPE *DestroyViewWindow )( 
            IShellView2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetCurrentInfo )( 
            IShellView2 * This,
            /* [out] */ LPFOLDERSETTINGS pfs);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *AddPropertySheetPages )( 
            IShellView2 * This,
            /* [in] */ DWORD dwReserved,
            /* [in] */ LPFNSVADDPROPSHEETPAGE pfn,
            /* [in] */ LPARAM lparam);
        
        HRESULT ( STDMETHODCALLTYPE *SaveViewState )( 
            IShellView2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *SelectItem )( 
            IShellView2 * This,
            /* [in] */ LPCITEMIDLIST pidlItem,
            /* [in] */ SVSIF uFlags);
        
        HRESULT ( STDMETHODCALLTYPE *GetItemObject )( 
            IShellView2 * This,
            /* [in] */ UINT uItem,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppv);
        
        HRESULT ( STDMETHODCALLTYPE *GetView )( 
            IShellView2 * This,
            /* [out][in] */ SHELLVIEWID *pvid,
            /* [in] */ ULONG uView);
        
        HRESULT ( STDMETHODCALLTYPE *CreateViewWindow2 )( 
            IShellView2 * This,
            /* [in] */ LPSV2CVW2_PARAMS lpParams);
        
        HRESULT ( STDMETHODCALLTYPE *HandleRename )( 
            IShellView2 * This,
            /* [in] */ LPCITEMIDLIST pidlNew);
        
        HRESULT ( STDMETHODCALLTYPE *SelectAndPositionItem )( 
            IShellView2 * This,
            /* [in] */ LPCITEMIDLIST pidlItem,
            /* [in] */ UINT uFlags,
            /* [in] */ POINT *ppt);
        
        END_INTERFACE
    } IShellView2Vtbl;

    interface IShellView2
    {
        CONST_VTBL struct IShellView2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IShellView2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IShellView2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IShellView2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IShellView2_GetWindow(This,phwnd)	\
    (This)->lpVtbl -> GetWindow(This,phwnd)

#define IShellView2_ContextSensitiveHelp(This,fEnterMode)	\
    (This)->lpVtbl -> ContextSensitiveHelp(This,fEnterMode)


#define IShellView2_TranslateAccelerator(This,pmsg)	\
    (This)->lpVtbl -> TranslateAccelerator(This,pmsg)

#define IShellView2_EnableModeless(This,fEnable)	\
    (This)->lpVtbl -> EnableModeless(This,fEnable)

#define IShellView2_UIActivate(This,uState)	\
    (This)->lpVtbl -> UIActivate(This,uState)

#define IShellView2_Refresh(This)	\
    (This)->lpVtbl -> Refresh(This)

#define IShellView2_CreateViewWindow(This,psvPrevious,pfs,psb,prcView,phWnd)	\
    (This)->lpVtbl -> CreateViewWindow(This,psvPrevious,pfs,psb,prcView,phWnd)

#define IShellView2_DestroyViewWindow(This)	\
    (This)->lpVtbl -> DestroyViewWindow(This)

#define IShellView2_GetCurrentInfo(This,pfs)	\
    (This)->lpVtbl -> GetCurrentInfo(This,pfs)

#define IShellView2_AddPropertySheetPages(This,dwReserved,pfn,lparam)	\
    (This)->lpVtbl -> AddPropertySheetPages(This,dwReserved,pfn,lparam)

#define IShellView2_SaveViewState(This)	\
    (This)->lpVtbl -> SaveViewState(This)

#define IShellView2_SelectItem(This,pidlItem,uFlags)	\
    (This)->lpVtbl -> SelectItem(This,pidlItem,uFlags)

#define IShellView2_GetItemObject(This,uItem,riid,ppv)	\
    (This)->lpVtbl -> GetItemObject(This,uItem,riid,ppv)


#define IShellView2_GetView(This,pvid,uView)	\
    (This)->lpVtbl -> GetView(This,pvid,uView)

#define IShellView2_CreateViewWindow2(This,lpParams)	\
    (This)->lpVtbl -> CreateViewWindow2(This,lpParams)

#define IShellView2_HandleRename(This,pidlNew)	\
    (This)->lpVtbl -> HandleRename(This,pidlNew)

#define IShellView2_SelectAndPositionItem(This,pidlItem,uFlags,ppt)	\
    (This)->lpVtbl -> SelectAndPositionItem(This,pidlItem,uFlags,ppt)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IShellView2_GetView_Proxy( 
    IShellView2 * This,
    /* [out][in] */ SHELLVIEWID *pvid,
    /* [in] */ ULONG uView);


void __RPC_STUB IShellView2_GetView_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellView2_CreateViewWindow2_Proxy( 
    IShellView2 * This,
    /* [in] */ LPSV2CVW2_PARAMS lpParams);


void __RPC_STUB IShellView2_CreateViewWindow2_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellView2_HandleRename_Proxy( 
    IShellView2 * This,
    /* [in] */ LPCITEMIDLIST pidlNew);


void __RPC_STUB IShellView2_HandleRename_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellView2_SelectAndPositionItem_Proxy( 
    IShellView2 * This,
    /* [in] */ LPCITEMIDLIST pidlItem,
    /* [in] */ UINT uFlags,
    /* [in] */ POINT *ppt);


void __RPC_STUB IShellView2_SelectAndPositionItem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IShellView2_INTERFACE_DEFINED__ */


#ifndef __IFolderView_INTERFACE_DEFINED__
#define __IFolderView_INTERFACE_DEFINED__

/* interface IFolderView */
/* [unique][object][uuid][helpstring] */ 


EXTERN_C const IID IID_IFolderView;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("cde725b0-ccc9-4519-917e-325d72fab4ce")
    IFolderView : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetCurrentViewMode( 
            /* [out][in] */ UINT *pViewMode) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetCurrentViewMode( 
            /* [in] */ UINT ViewMode) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetFolder( 
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppv) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Item( 
            /* [in] */ int iItemIndex,
            /* [out] */ LPITEMIDLIST *ppidl) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ItemCount( 
            /* [in] */ UINT uFlags,
            /* [out] */ int *pcItems) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Items( 
            /* [in] */ UINT uFlags,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppv) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSelectionMarkedItem( 
            /* [out] */ int *piItem) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetFocusedItem( 
            /* [out] */ int *piItem) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetItemPosition( 
            /* [in] */ LPCITEMIDLIST pidl,
            /* [out] */ POINT *ppt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetSpacing( 
            /* [full][out][in] */ POINT *ppt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDefaultSpacing( 
            /* [out] */ POINT *ppt) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAutoArrange( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SelectItem( 
            /* [in] */ int iItem,
            /* [in] */ DWORD dwFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SelectAndPositionItems( 
            /* [in] */ UINT cidl,
            /* [size_is][in] */ LPCITEMIDLIST *apidl,
            /* [size_is][full][in] */ POINT *apt,
            /* [in] */ DWORD dwFlags) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IFolderViewVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IFolderView * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IFolderView * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IFolderView * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetCurrentViewMode )( 
            IFolderView * This,
            /* [out][in] */ UINT *pViewMode);
        
        HRESULT ( STDMETHODCALLTYPE *SetCurrentViewMode )( 
            IFolderView * This,
            /* [in] */ UINT ViewMode);
        
        HRESULT ( STDMETHODCALLTYPE *GetFolder )( 
            IFolderView * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppv);
        
        HRESULT ( STDMETHODCALLTYPE *Item )( 
            IFolderView * This,
            /* [in] */ int iItemIndex,
            /* [out] */ LPITEMIDLIST *ppidl);
        
        HRESULT ( STDMETHODCALLTYPE *ItemCount )( 
            IFolderView * This,
            /* [in] */ UINT uFlags,
            /* [out] */ int *pcItems);
        
        HRESULT ( STDMETHODCALLTYPE *Items )( 
            IFolderView * This,
            /* [in] */ UINT uFlags,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppv);
        
        HRESULT ( STDMETHODCALLTYPE *GetSelectionMarkedItem )( 
            IFolderView * This,
            /* [out] */ int *piItem);
        
        HRESULT ( STDMETHODCALLTYPE *GetFocusedItem )( 
            IFolderView * This,
            /* [out] */ int *piItem);
        
        HRESULT ( STDMETHODCALLTYPE *GetItemPosition )( 
            IFolderView * This,
            /* [in] */ LPCITEMIDLIST pidl,
            /* [out] */ POINT *ppt);
        
        HRESULT ( STDMETHODCALLTYPE *GetSpacing )( 
            IFolderView * This,
            /* [full][out][in] */ POINT *ppt);
        
        HRESULT ( STDMETHODCALLTYPE *GetDefaultSpacing )( 
            IFolderView * This,
            /* [out] */ POINT *ppt);
        
        HRESULT ( STDMETHODCALLTYPE *GetAutoArrange )( 
            IFolderView * This);
        
        HRESULT ( STDMETHODCALLTYPE *SelectItem )( 
            IFolderView * This,
            /* [in] */ int iItem,
            /* [in] */ DWORD dwFlags);
        
        HRESULT ( STDMETHODCALLTYPE *SelectAndPositionItems )( 
            IFolderView * This,
            /* [in] */ UINT cidl,
            /* [size_is][in] */ LPCITEMIDLIST *apidl,
            /* [size_is][full][in] */ POINT *apt,
            /* [in] */ DWORD dwFlags);
        
        END_INTERFACE
    } IFolderViewVtbl;

    interface IFolderView
    {
        CONST_VTBL struct IFolderViewVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFolderView_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IFolderView_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IFolderView_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IFolderView_GetCurrentViewMode(This,pViewMode)	\
    (This)->lpVtbl -> GetCurrentViewMode(This,pViewMode)

#define IFolderView_SetCurrentViewMode(This,ViewMode)	\
    (This)->lpVtbl -> SetCurrentViewMode(This,ViewMode)

#define IFolderView_GetFolder(This,riid,ppv)	\
    (This)->lpVtbl -> GetFolder(This,riid,ppv)

#define IFolderView_Item(This,iItemIndex,ppidl)	\
    (This)->lpVtbl -> Item(This,iItemIndex,ppidl)

#define IFolderView_ItemCount(This,uFlags,pcItems)	\
    (This)->lpVtbl -> ItemCount(This,uFlags,pcItems)

#define IFolderView_Items(This,uFlags,riid,ppv)	\
    (This)->lpVtbl -> Items(This,uFlags,riid,ppv)

#define IFolderView_GetSelectionMarkedItem(This,piItem)	\
    (This)->lpVtbl -> GetSelectionMarkedItem(This,piItem)

#define IFolderView_GetFocusedItem(This,piItem)	\
    (This)->lpVtbl -> GetFocusedItem(This,piItem)

#define IFolderView_GetItemPosition(This,pidl,ppt)	\
    (This)->lpVtbl -> GetItemPosition(This,pidl,ppt)

#define IFolderView_GetSpacing(This,ppt)	\
    (This)->lpVtbl -> GetSpacing(This,ppt)

#define IFolderView_GetDefaultSpacing(This,ppt)	\
    (This)->lpVtbl -> GetDefaultSpacing(This,ppt)

#define IFolderView_GetAutoArrange(This)	\
    (This)->lpVtbl -> GetAutoArrange(This)

#define IFolderView_SelectItem(This,iItem,dwFlags)	\
    (This)->lpVtbl -> SelectItem(This,iItem,dwFlags)

#define IFolderView_SelectAndPositionItems(This,cidl,apidl,apt,dwFlags)	\
    (This)->lpVtbl -> SelectAndPositionItems(This,cidl,apidl,apt,dwFlags)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IFolderView_GetCurrentViewMode_Proxy( 
    IFolderView * This,
    /* [out][in] */ UINT *pViewMode);


void __RPC_STUB IFolderView_GetCurrentViewMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IFolderView_SetCurrentViewMode_Proxy( 
    IFolderView * This,
    /* [in] */ UINT ViewMode);


void __RPC_STUB IFolderView_SetCurrentViewMode_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IFolderView_GetFolder_Proxy( 
    IFolderView * This,
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ void **ppv);


void __RPC_STUB IFolderView_GetFolder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IFolderView_Item_Proxy( 
    IFolderView * This,
    /* [in] */ int iItemIndex,
    /* [out] */ LPITEMIDLIST *ppidl);


void __RPC_STUB IFolderView_Item_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IFolderView_ItemCount_Proxy( 
    IFolderView * This,
    /* [in] */ UINT uFlags,
    /* [out] */ int *pcItems);


void __RPC_STUB IFolderView_ItemCount_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IFolderView_Items_Proxy( 
    IFolderView * This,
    /* [in] */ UINT uFlags,
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ void **ppv);


void __RPC_STUB IFolderView_Items_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IFolderView_GetSelectionMarkedItem_Proxy( 
    IFolderView * This,
    /* [out] */ int *piItem);


void __RPC_STUB IFolderView_GetSelectionMarkedItem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IFolderView_GetFocusedItem_Proxy( 
    IFolderView * This,
    /* [out] */ int *piItem);


void __RPC_STUB IFolderView_GetFocusedItem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IFolderView_GetItemPosition_Proxy( 
    IFolderView * This,
    /* [in] */ LPCITEMIDLIST pidl,
    /* [out] */ POINT *ppt);


void __RPC_STUB IFolderView_GetItemPosition_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IFolderView_GetSpacing_Proxy( 
    IFolderView * This,
    /* [full][out][in] */ POINT *ppt);


void __RPC_STUB IFolderView_GetSpacing_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IFolderView_GetDefaultSpacing_Proxy( 
    IFolderView * This,
    /* [out] */ POINT *ppt);


void __RPC_STUB IFolderView_GetDefaultSpacing_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IFolderView_GetAutoArrange_Proxy( 
    IFolderView * This);


void __RPC_STUB IFolderView_GetAutoArrange_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IFolderView_SelectItem_Proxy( 
    IFolderView * This,
    /* [in] */ int iItem,
    /* [in] */ DWORD dwFlags);


void __RPC_STUB IFolderView_SelectItem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IFolderView_SelectAndPositionItems_Proxy( 
    IFolderView * This,
    /* [in] */ UINT cidl,
    /* [size_is][in] */ LPCITEMIDLIST *apidl,
    /* [size_is][full][in] */ POINT *apt,
    /* [in] */ DWORD dwFlags);


void __RPC_STUB IFolderView_SelectAndPositionItems_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IFolderView_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0205 */
/* [local] */ 

#define SID_SFolderView IID_IFolderView    // folder view, usuaally IFolderView
#ifdef _FIX_ENABLEMODELESS_CONFLICT
#undef    EnableModeless 
#endif


extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0205_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0205_v0_0_s_ifspec;

#ifndef __IFolderFilterSite_INTERFACE_DEFINED__
#define __IFolderFilterSite_INTERFACE_DEFINED__

/* interface IFolderFilterSite */
/* [unique][object][uuid][helpstring] */ 


EXTERN_C const IID IID_IFolderFilterSite;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C0A651F5-B48B-11d2-B5ED-006097C686F6")
    IFolderFilterSite : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetFilter( 
            /* [in] */ IUnknown *punk) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IFolderFilterSiteVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IFolderFilterSite * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IFolderFilterSite * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IFolderFilterSite * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetFilter )( 
            IFolderFilterSite * This,
            /* [in] */ IUnknown *punk);
        
        END_INTERFACE
    } IFolderFilterSiteVtbl;

    interface IFolderFilterSite
    {
        CONST_VTBL struct IFolderFilterSiteVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFolderFilterSite_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IFolderFilterSite_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IFolderFilterSite_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IFolderFilterSite_SetFilter(This,punk)	\
    (This)->lpVtbl -> SetFilter(This,punk)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IFolderFilterSite_SetFilter_Proxy( 
    IFolderFilterSite * This,
    /* [in] */ IUnknown *punk);


void __RPC_STUB IFolderFilterSite_SetFilter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IFolderFilterSite_INTERFACE_DEFINED__ */


#ifndef __IFolderFilter_INTERFACE_DEFINED__
#define __IFolderFilter_INTERFACE_DEFINED__

/* interface IFolderFilter */
/* [unique][object][uuid][helpstring] */ 


EXTERN_C const IID IID_IFolderFilter;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9CC22886-DC8E-11d2-B1D0-00C04F8EEB3E")
    IFolderFilter : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE ShouldShow( 
            /* [in] */ IShellFolder *psf,
            /* [in] */ LPCITEMIDLIST pidlFolder,
            /* [in] */ LPCITEMIDLIST pidlItem) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetEnumFlags( 
            /* [in] */ IShellFolder *psf,
            /* [in] */ LPCITEMIDLIST pidlFolder,
            /* [in] */ HWND *phwnd,
            /* [out] */ DWORD *pgrfFlags) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IFolderFilterVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IFolderFilter * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IFolderFilter * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IFolderFilter * This);
        
        HRESULT ( STDMETHODCALLTYPE *ShouldShow )( 
            IFolderFilter * This,
            /* [in] */ IShellFolder *psf,
            /* [in] */ LPCITEMIDLIST pidlFolder,
            /* [in] */ LPCITEMIDLIST pidlItem);
        
        HRESULT ( STDMETHODCALLTYPE *GetEnumFlags )( 
            IFolderFilter * This,
            /* [in] */ IShellFolder *psf,
            /* [in] */ LPCITEMIDLIST pidlFolder,
            /* [in] */ HWND *phwnd,
            /* [out] */ DWORD *pgrfFlags);
        
        END_INTERFACE
    } IFolderFilterVtbl;

    interface IFolderFilter
    {
        CONST_VTBL struct IFolderFilterVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFolderFilter_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IFolderFilter_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IFolderFilter_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IFolderFilter_ShouldShow(This,psf,pidlFolder,pidlItem)	\
    (This)->lpVtbl -> ShouldShow(This,psf,pidlFolder,pidlItem)

#define IFolderFilter_GetEnumFlags(This,psf,pidlFolder,phwnd,pgrfFlags)	\
    (This)->lpVtbl -> GetEnumFlags(This,psf,pidlFolder,phwnd,pgrfFlags)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IFolderFilter_ShouldShow_Proxy( 
    IFolderFilter * This,
    /* [in] */ IShellFolder *psf,
    /* [in] */ LPCITEMIDLIST pidlFolder,
    /* [in] */ LPCITEMIDLIST pidlItem);


void __RPC_STUB IFolderFilter_ShouldShow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IFolderFilter_GetEnumFlags_Proxy( 
    IFolderFilter * This,
    /* [in] */ IShellFolder *psf,
    /* [in] */ LPCITEMIDLIST pidlFolder,
    /* [in] */ HWND *phwnd,
    /* [out] */ DWORD *pgrfFlags);


void __RPC_STUB IFolderFilter_GetEnumFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IFolderFilter_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0207 */
/* [local] */ 

#define SBSP_DEFBROWSER         0x0000
#define SBSP_SAMEBROWSER        0x0001
#define SBSP_NEWBROWSER         0x0002
#define SBSP_DEFMODE            0x0000
#define SBSP_OPENMODE           0x0010
#define SBSP_EXPLOREMODE        0x0020
#define SBSP_HELPMODE           0x0040 // IEUNIX : Help window uses this.
#define SBSP_NOTRANSFERHIST     0x0080
#define SBSP_ABSOLUTE           0x0000
#define SBSP_RELATIVE           0x1000
#define SBSP_PARENT             0x2000
#define SBSP_NAVIGATEBACK       0x4000
#define SBSP_NAVIGATEFORWARD    0x8000
#define SBSP_ALLOW_AUTONAVIGATE 0x10000
#define SBSP_CALLERUNTRUSTED      0x00800000
#define SBSP_TRUSTFIRSTDOWNLOAD   0x01000000
#define SBSP_UNTRUSTEDFORDOWNLOAD 0x02000000
#define SBSP_NOAUTOSELECT         0x04000000
#define SBSP_WRITENOHISTORY       0x08000000
#define SBSP_TRUSTEDFORACTIVEX    0x10000000
#define SBSP_REDIRECT                     0x40000000
#define SBSP_INITIATEDBYHLINKFRAME        0x80000000
#define FCW_STATUS      0x0001
#define FCW_TOOLBAR     0x0002
#define FCW_TREE        0x0003
#define FCW_INTERNETBAR 0x0006
#define FCW_PROGRESS    0x0008
#define FCT_MERGE       0x0001
#define FCT_CONFIGABLE  0x0002
#define FCT_ADDTOEND    0x0004
#ifdef _NEVER_
typedef LPARAM LPTBBUTTONSB;

#else //!_NEVER_
#include <commctrl.h>
typedef LPTBBUTTON LPTBBUTTONSB;
#endif //_NEVER_


extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0207_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0207_v0_0_s_ifspec;

#ifndef __IShellBrowser_INTERFACE_DEFINED__
#define __IShellBrowser_INTERFACE_DEFINED__

/* interface IShellBrowser */
/* [unique][object][uuid][helpstring] */ 


EXTERN_C const IID IID_IShellBrowser;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("000214E2-0000-0000-C000-000000000046")
    IShellBrowser : public IOleWindow
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE InsertMenusSB( 
            /* [in] */ HMENU hmenuShared,
            /* [out][in] */ LPOLEMENUGROUPWIDTHS lpMenuWidths) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetMenuSB( 
            /* [in] */ HMENU hmenuShared,
            /* [in] */ HOLEMENU holemenuRes,
            /* [in] */ HWND hwndActiveObject) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveMenusSB( 
            /* [in] */ HMENU hmenuShared) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetStatusTextSB( 
            /* [unique][in] */ LPCOLESTR pszStatusText) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnableModelessSB( 
            /* [in] */ BOOL fEnable) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE TranslateAcceleratorSB( 
            /* [in] */ MSG *pmsg,
            /* [in] */ WORD wID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE BrowseObject( 
            /* [in] */ LPCITEMIDLIST pidl,
            /* [in] */ UINT wFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetViewStateStream( 
            /* [in] */ DWORD grfMode,
            /* [out] */ IStream **ppStrm) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetControlWindow( 
            /* [in] */ UINT id,
            /* [out] */ HWND *phwnd) = 0;
        
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE SendControlMsg( 
            /* [in] */ UINT id,
            /* [in] */ UINT uMsg,
            /* [in] */ WPARAM wParam,
            /* [in] */ LPARAM lParam,
            /* [in] */ LRESULT *pret) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE QueryActiveShellView( 
            /* [out] */ IShellView **ppshv) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnViewWindowActive( 
            /* [in] */ IShellView *pshv) = 0;
        
        virtual /* [local] */ HRESULT STDMETHODCALLTYPE SetToolbarItems( 
            /* [in] */ LPTBBUTTONSB lpButtons,
            /* [in] */ UINT nButtons,
            /* [in] */ UINT uFlags) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IShellBrowserVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IShellBrowser * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IShellBrowser * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IShellBrowser * This);
        
        /* [input_sync] */ HRESULT ( STDMETHODCALLTYPE *GetWindow )( 
            IShellBrowser * This,
            /* [out] */ HWND *phwnd);
        
        HRESULT ( STDMETHODCALLTYPE *ContextSensitiveHelp )( 
            IShellBrowser * This,
            /* [in] */ BOOL fEnterMode);
        
        HRESULT ( STDMETHODCALLTYPE *InsertMenusSB )( 
            IShellBrowser * This,
            /* [in] */ HMENU hmenuShared,
            /* [out][in] */ LPOLEMENUGROUPWIDTHS lpMenuWidths);
        
        HRESULT ( STDMETHODCALLTYPE *SetMenuSB )( 
            IShellBrowser * This,
            /* [in] */ HMENU hmenuShared,
            /* [in] */ HOLEMENU holemenuRes,
            /* [in] */ HWND hwndActiveObject);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveMenusSB )( 
            IShellBrowser * This,
            /* [in] */ HMENU hmenuShared);
        
        HRESULT ( STDMETHODCALLTYPE *SetStatusTextSB )( 
            IShellBrowser * This,
            /* [unique][in] */ LPCOLESTR pszStatusText);
        
        HRESULT ( STDMETHODCALLTYPE *EnableModelessSB )( 
            IShellBrowser * This,
            /* [in] */ BOOL fEnable);
        
        HRESULT ( STDMETHODCALLTYPE *TranslateAcceleratorSB )( 
            IShellBrowser * This,
            /* [in] */ MSG *pmsg,
            /* [in] */ WORD wID);
        
        HRESULT ( STDMETHODCALLTYPE *BrowseObject )( 
            IShellBrowser * This,
            /* [in] */ LPCITEMIDLIST pidl,
            /* [in] */ UINT wFlags);
        
        HRESULT ( STDMETHODCALLTYPE *GetViewStateStream )( 
            IShellBrowser * This,
            /* [in] */ DWORD grfMode,
            /* [out] */ IStream **ppStrm);
        
        HRESULT ( STDMETHODCALLTYPE *GetControlWindow )( 
            IShellBrowser * This,
            /* [in] */ UINT id,
            /* [out] */ HWND *phwnd);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *SendControlMsg )( 
            IShellBrowser * This,
            /* [in] */ UINT id,
            /* [in] */ UINT uMsg,
            /* [in] */ WPARAM wParam,
            /* [in] */ LPARAM lParam,
            /* [in] */ LRESULT *pret);
        
        HRESULT ( STDMETHODCALLTYPE *QueryActiveShellView )( 
            IShellBrowser * This,
            /* [out] */ IShellView **ppshv);
        
        HRESULT ( STDMETHODCALLTYPE *OnViewWindowActive )( 
            IShellBrowser * This,
            /* [in] */ IShellView *pshv);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *SetToolbarItems )( 
            IShellBrowser * This,
            /* [in] */ LPTBBUTTONSB lpButtons,
            /* [in] */ UINT nButtons,
            /* [in] */ UINT uFlags);
        
        END_INTERFACE
    } IShellBrowserVtbl;

    interface IShellBrowser
    {
        CONST_VTBL struct IShellBrowserVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IShellBrowser_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IShellBrowser_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IShellBrowser_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IShellBrowser_GetWindow(This,phwnd)	\
    (This)->lpVtbl -> GetWindow(This,phwnd)

#define IShellBrowser_ContextSensitiveHelp(This,fEnterMode)	\
    (This)->lpVtbl -> ContextSensitiveHelp(This,fEnterMode)


#define IShellBrowser_InsertMenusSB(This,hmenuShared,lpMenuWidths)	\
    (This)->lpVtbl -> InsertMenusSB(This,hmenuShared,lpMenuWidths)

#define IShellBrowser_SetMenuSB(This,hmenuShared,holemenuRes,hwndActiveObject)	\
    (This)->lpVtbl -> SetMenuSB(This,hmenuShared,holemenuRes,hwndActiveObject)

#define IShellBrowser_RemoveMenusSB(This,hmenuShared)	\
    (This)->lpVtbl -> RemoveMenusSB(This,hmenuShared)

#define IShellBrowser_SetStatusTextSB(This,pszStatusText)	\
    (This)->lpVtbl -> SetStatusTextSB(This,pszStatusText)

#define IShellBrowser_EnableModelessSB(This,fEnable)	\
    (This)->lpVtbl -> EnableModelessSB(This,fEnable)

#define IShellBrowser_TranslateAcceleratorSB(This,pmsg,wID)	\
    (This)->lpVtbl -> TranslateAcceleratorSB(This,pmsg,wID)

#define IShellBrowser_BrowseObject(This,pidl,wFlags)	\
    (This)->lpVtbl -> BrowseObject(This,pidl,wFlags)

#define IShellBrowser_GetViewStateStream(This,grfMode,ppStrm)	\
    (This)->lpVtbl -> GetViewStateStream(This,grfMode,ppStrm)

#define IShellBrowser_GetControlWindow(This,id,phwnd)	\
    (This)->lpVtbl -> GetControlWindow(This,id,phwnd)

#define IShellBrowser_SendControlMsg(This,id,uMsg,wParam,lParam,pret)	\
    (This)->lpVtbl -> SendControlMsg(This,id,uMsg,wParam,lParam,pret)

#define IShellBrowser_QueryActiveShellView(This,ppshv)	\
    (This)->lpVtbl -> QueryActiveShellView(This,ppshv)

#define IShellBrowser_OnViewWindowActive(This,pshv)	\
    (This)->lpVtbl -> OnViewWindowActive(This,pshv)

#define IShellBrowser_SetToolbarItems(This,lpButtons,nButtons,uFlags)	\
    (This)->lpVtbl -> SetToolbarItems(This,lpButtons,nButtons,uFlags)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IShellBrowser_InsertMenusSB_Proxy( 
    IShellBrowser * This,
    /* [in] */ HMENU hmenuShared,
    /* [out][in] */ LPOLEMENUGROUPWIDTHS lpMenuWidths);


void __RPC_STUB IShellBrowser_InsertMenusSB_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellBrowser_SetMenuSB_Proxy( 
    IShellBrowser * This,
    /* [in] */ HMENU hmenuShared,
    /* [in] */ HOLEMENU holemenuRes,
    /* [in] */ HWND hwndActiveObject);


void __RPC_STUB IShellBrowser_SetMenuSB_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellBrowser_RemoveMenusSB_Proxy( 
    IShellBrowser * This,
    /* [in] */ HMENU hmenuShared);


void __RPC_STUB IShellBrowser_RemoveMenusSB_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellBrowser_SetStatusTextSB_Proxy( 
    IShellBrowser * This,
    /* [unique][in] */ LPCOLESTR pszStatusText);


void __RPC_STUB IShellBrowser_SetStatusTextSB_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellBrowser_EnableModelessSB_Proxy( 
    IShellBrowser * This,
    /* [in] */ BOOL fEnable);


void __RPC_STUB IShellBrowser_EnableModelessSB_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellBrowser_TranslateAcceleratorSB_Proxy( 
    IShellBrowser * This,
    /* [in] */ MSG *pmsg,
    /* [in] */ WORD wID);


void __RPC_STUB IShellBrowser_TranslateAcceleratorSB_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellBrowser_BrowseObject_Proxy( 
    IShellBrowser * This,
    /* [in] */ LPCITEMIDLIST pidl,
    /* [in] */ UINT wFlags);


void __RPC_STUB IShellBrowser_BrowseObject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellBrowser_GetViewStateStream_Proxy( 
    IShellBrowser * This,
    /* [in] */ DWORD grfMode,
    /* [out] */ IStream **ppStrm);


void __RPC_STUB IShellBrowser_GetViewStateStream_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellBrowser_GetControlWindow_Proxy( 
    IShellBrowser * This,
    /* [in] */ UINT id,
    /* [out] */ HWND *phwnd);


void __RPC_STUB IShellBrowser_GetControlWindow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [local] */ HRESULT STDMETHODCALLTYPE IShellBrowser_SendControlMsg_Proxy( 
    IShellBrowser * This,
    /* [in] */ UINT id,
    /* [in] */ UINT uMsg,
    /* [in] */ WPARAM wParam,
    /* [in] */ LPARAM lParam,
    /* [in] */ LRESULT *pret);


void __RPC_STUB IShellBrowser_SendControlMsg_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellBrowser_QueryActiveShellView_Proxy( 
    IShellBrowser * This,
    /* [out] */ IShellView **ppshv);


void __RPC_STUB IShellBrowser_QueryActiveShellView_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellBrowser_OnViewWindowActive_Proxy( 
    IShellBrowser * This,
    /* [in] */ IShellView *pshv);


void __RPC_STUB IShellBrowser_OnViewWindowActive_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [local] */ HRESULT STDMETHODCALLTYPE IShellBrowser_SetToolbarItems_Proxy( 
    IShellBrowser * This,
    /* [in] */ LPTBBUTTONSB lpButtons,
    /* [in] */ UINT nButtons,
    /* [in] */ UINT uFlags);


void __RPC_STUB IShellBrowser_SetToolbarItems_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IShellBrowser_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0208 */
/* [local] */ 

typedef IShellBrowser *LPSHELLBROWSER;



extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0208_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0208_v0_0_s_ifspec;

#ifndef __IProfferService_INTERFACE_DEFINED__
#define __IProfferService_INTERFACE_DEFINED__

/* interface IProfferService */
/* [unique][object][uuid][helpstring] */ 


EXTERN_C const IID IID_IProfferService;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("cb728b20-f786-11ce-92ad-00aa00a74cd0")
    IProfferService : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE ProfferService( 
            /* [in] */ REFGUID rguidService,
            /* [in] */ IServiceProvider *psp,
            /* [out] */ DWORD *pdwCookie) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RevokeService( 
            /* [in] */ DWORD dwCookie) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IProfferServiceVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IProfferService * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IProfferService * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IProfferService * This);
        
        HRESULT ( STDMETHODCALLTYPE *ProfferService )( 
            IProfferService * This,
            /* [in] */ REFGUID rguidService,
            /* [in] */ IServiceProvider *psp,
            /* [out] */ DWORD *pdwCookie);
        
        HRESULT ( STDMETHODCALLTYPE *RevokeService )( 
            IProfferService * This,
            /* [in] */ DWORD dwCookie);
        
        END_INTERFACE
    } IProfferServiceVtbl;

    interface IProfferService
    {
        CONST_VTBL struct IProfferServiceVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IProfferService_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IProfferService_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IProfferService_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IProfferService_ProfferService(This,rguidService,psp,pdwCookie)	\
    (This)->lpVtbl -> ProfferService(This,rguidService,psp,pdwCookie)

#define IProfferService_RevokeService(This,dwCookie)	\
    (This)->lpVtbl -> RevokeService(This,dwCookie)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IProfferService_ProfferService_Proxy( 
    IProfferService * This,
    /* [in] */ REFGUID rguidService,
    /* [in] */ IServiceProvider *psp,
    /* [out] */ DWORD *pdwCookie);


void __RPC_STUB IProfferService_ProfferService_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IProfferService_RevokeService_Proxy( 
    IProfferService * This,
    /* [in] */ DWORD dwCookie);


void __RPC_STUB IProfferService_RevokeService_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IProfferService_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0209 */
/* [local] */ 

#define SID_SProfferService IID_IProfferService    // nearest service that you can proffer to
typedef /* [public][public][v1_enum] */ 
enum __MIDL___MIDL_itf_shobjidl_0209_0001
    {	PUIFNF_DEFAULT	= 0,
	PUIFNF_MNEMONIC	= 0x1
    } 	PROPERTYUI_NAME_FLAGS;

typedef /* [public][public][v1_enum] */ 
enum __MIDL___MIDL_itf_shobjidl_0209_0002
    {	PUIF_DEFAULT	= 0,
	PUIF_RIGHTALIGN	= 0x1,
	PUIF_NOLABELININFOTIP	= 0x2
    } 	PROPERTYUI_FLAGS;

typedef /* [public][public][v1_enum] */ 
enum __MIDL___MIDL_itf_shobjidl_0209_0003
    {	PUIFFDF_DEFAULT	= 0,
	PUIFFDF_RIGHTTOLEFT	= 0x1,
	PUIFFDF_SHORTFORMAT	= 0x2,
	PUIFFDF_NOTIME	= 0x4,
	PUIFFDF_FRIENDLYDATE	= 0x8,
	PUIFFDF_NOUNITS	= 0x10
    } 	PROPERTYUI_FORMAT_FLAGS;



extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0209_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0209_v0_0_s_ifspec;

#ifndef __IPropertyUI_INTERFACE_DEFINED__
#define __IPropertyUI_INTERFACE_DEFINED__

/* interface IPropertyUI */
/* [unique][object][uuid][helpstring] */ 


EXTERN_C const IID IID_IPropertyUI;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("757a7d9f-919a-4118-99d7-dbb208c8cc66")
    IPropertyUI : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE ParsePropertyName( 
            /* [in] */ LPCWSTR pszName,
            /* [out] */ FMTID *pfmtid,
            /* [out] */ PROPID *ppid,
            /* [out][in] */ ULONG *pchEaten) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCannonicalName( 
            /* [in] */ REFFMTID fmtid,
            /* [in] */ PROPID pid,
            /* [size_is][out] */ LPWSTR pwszText,
            /* [in] */ DWORD cchText) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDisplayName( 
            /* [in] */ REFFMTID fmtid,
            /* [in] */ PROPID pid,
            /* [in] */ PROPERTYUI_NAME_FLAGS flags,
            /* [size_is][out] */ LPWSTR pwszText,
            /* [in] */ DWORD cchText) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetPropertyDescription( 
            /* [in] */ REFFMTID fmtid,
            /* [in] */ PROPID pid,
            /* [size_is][out] */ LPWSTR pwszText,
            /* [in] */ DWORD cchText) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDefaultWidth( 
            /* [in] */ REFFMTID fmtid,
            /* [in] */ PROPID pid,
            /* [out] */ ULONG *pcxChars) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetFlags( 
            /* [in] */ REFFMTID fmtid,
            /* [in] */ PROPID pid,
            /* [out] */ PROPERTYUI_FLAGS *pFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE FormatForDisplay( 
            /* [in] */ REFFMTID fmtid,
            /* [in] */ PROPID pid,
            /* [in] */ const PROPVARIANT *pvar,
            /* [in] */ PROPERTYUI_FORMAT_FLAGS flags,
            /* [size_is][out] */ LPWSTR pwszText,
            /* [in] */ DWORD cchText) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetHelpInfo( 
            /* [in] */ REFFMTID fmtid,
            /* [in] */ PROPID pid,
            /* [size_is][out] */ LPWSTR pwszHelpFile,
            /* [in] */ DWORD cch,
            /* [out] */ UINT *puHelpID) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPropertyUIVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPropertyUI * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPropertyUI * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPropertyUI * This);
        
        HRESULT ( STDMETHODCALLTYPE *ParsePropertyName )( 
            IPropertyUI * This,
            /* [in] */ LPCWSTR pszName,
            /* [out] */ FMTID *pfmtid,
            /* [out] */ PROPID *ppid,
            /* [out][in] */ ULONG *pchEaten);
        
        HRESULT ( STDMETHODCALLTYPE *GetCannonicalName )( 
            IPropertyUI * This,
            /* [in] */ REFFMTID fmtid,
            /* [in] */ PROPID pid,
            /* [size_is][out] */ LPWSTR pwszText,
            /* [in] */ DWORD cchText);
        
        HRESULT ( STDMETHODCALLTYPE *GetDisplayName )( 
            IPropertyUI * This,
            /* [in] */ REFFMTID fmtid,
            /* [in] */ PROPID pid,
            /* [in] */ PROPERTYUI_NAME_FLAGS flags,
            /* [size_is][out] */ LPWSTR pwszText,
            /* [in] */ DWORD cchText);
        
        HRESULT ( STDMETHODCALLTYPE *GetPropertyDescription )( 
            IPropertyUI * This,
            /* [in] */ REFFMTID fmtid,
            /* [in] */ PROPID pid,
            /* [size_is][out] */ LPWSTR pwszText,
            /* [in] */ DWORD cchText);
        
        HRESULT ( STDMETHODCALLTYPE *GetDefaultWidth )( 
            IPropertyUI * This,
            /* [in] */ REFFMTID fmtid,
            /* [in] */ PROPID pid,
            /* [out] */ ULONG *pcxChars);
        
        HRESULT ( STDMETHODCALLTYPE *GetFlags )( 
            IPropertyUI * This,
            /* [in] */ REFFMTID fmtid,
            /* [in] */ PROPID pid,
            /* [out] */ PROPERTYUI_FLAGS *pFlags);
        
        HRESULT ( STDMETHODCALLTYPE *FormatForDisplay )( 
            IPropertyUI * This,
            /* [in] */ REFFMTID fmtid,
            /* [in] */ PROPID pid,
            /* [in] */ const PROPVARIANT *pvar,
            /* [in] */ PROPERTYUI_FORMAT_FLAGS flags,
            /* [size_is][out] */ LPWSTR pwszText,
            /* [in] */ DWORD cchText);
        
        HRESULT ( STDMETHODCALLTYPE *GetHelpInfo )( 
            IPropertyUI * This,
            /* [in] */ REFFMTID fmtid,
            /* [in] */ PROPID pid,
            /* [size_is][out] */ LPWSTR pwszHelpFile,
            /* [in] */ DWORD cch,
            /* [out] */ UINT *puHelpID);
        
        END_INTERFACE
    } IPropertyUIVtbl;

    interface IPropertyUI
    {
        CONST_VTBL struct IPropertyUIVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPropertyUI_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IPropertyUI_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IPropertyUI_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IPropertyUI_ParsePropertyName(This,pszName,pfmtid,ppid,pchEaten)	\
    (This)->lpVtbl -> ParsePropertyName(This,pszName,pfmtid,ppid,pchEaten)

#define IPropertyUI_GetCannonicalName(This,fmtid,pid,pwszText,cchText)	\
    (This)->lpVtbl -> GetCannonicalName(This,fmtid,pid,pwszText,cchText)

#define IPropertyUI_GetDisplayName(This,fmtid,pid,flags,pwszText,cchText)	\
    (This)->lpVtbl -> GetDisplayName(This,fmtid,pid,flags,pwszText,cchText)

#define IPropertyUI_GetPropertyDescription(This,fmtid,pid,pwszText,cchText)	\
    (This)->lpVtbl -> GetPropertyDescription(This,fmtid,pid,pwszText,cchText)

#define IPropertyUI_GetDefaultWidth(This,fmtid,pid,pcxChars)	\
    (This)->lpVtbl -> GetDefaultWidth(This,fmtid,pid,pcxChars)

#define IPropertyUI_GetFlags(This,fmtid,pid,pFlags)	\
    (This)->lpVtbl -> GetFlags(This,fmtid,pid,pFlags)

#define IPropertyUI_FormatForDisplay(This,fmtid,pid,pvar,flags,pwszText,cchText)	\
    (This)->lpVtbl -> FormatForDisplay(This,fmtid,pid,pvar,flags,pwszText,cchText)

#define IPropertyUI_GetHelpInfo(This,fmtid,pid,pwszHelpFile,cch,puHelpID)	\
    (This)->lpVtbl -> GetHelpInfo(This,fmtid,pid,pwszHelpFile,cch,puHelpID)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IPropertyUI_ParsePropertyName_Proxy( 
    IPropertyUI * This,
    /* [in] */ LPCWSTR pszName,
    /* [out] */ FMTID *pfmtid,
    /* [out] */ PROPID *ppid,
    /* [out][in] */ ULONG *pchEaten);


void __RPC_STUB IPropertyUI_ParsePropertyName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IPropertyUI_GetCannonicalName_Proxy( 
    IPropertyUI * This,
    /* [in] */ REFFMTID fmtid,
    /* [in] */ PROPID pid,
    /* [size_is][out] */ LPWSTR pwszText,
    /* [in] */ DWORD cchText);


void __RPC_STUB IPropertyUI_GetCannonicalName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IPropertyUI_GetDisplayName_Proxy( 
    IPropertyUI * This,
    /* [in] */ REFFMTID fmtid,
    /* [in] */ PROPID pid,
    /* [in] */ PROPERTYUI_NAME_FLAGS flags,
    /* [size_is][out] */ LPWSTR pwszText,
    /* [in] */ DWORD cchText);


void __RPC_STUB IPropertyUI_GetDisplayName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IPropertyUI_GetPropertyDescription_Proxy( 
    IPropertyUI * This,
    /* [in] */ REFFMTID fmtid,
    /* [in] */ PROPID pid,
    /* [size_is][out] */ LPWSTR pwszText,
    /* [in] */ DWORD cchText);


void __RPC_STUB IPropertyUI_GetPropertyDescription_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IPropertyUI_GetDefaultWidth_Proxy( 
    IPropertyUI * This,
    /* [in] */ REFFMTID fmtid,
    /* [in] */ PROPID pid,
    /* [out] */ ULONG *pcxChars);


void __RPC_STUB IPropertyUI_GetDefaultWidth_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IPropertyUI_GetFlags_Proxy( 
    IPropertyUI * This,
    /* [in] */ REFFMTID fmtid,
    /* [in] */ PROPID pid,
    /* [out] */ PROPERTYUI_FLAGS *pFlags);


void __RPC_STUB IPropertyUI_GetFlags_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IPropertyUI_FormatForDisplay_Proxy( 
    IPropertyUI * This,
    /* [in] */ REFFMTID fmtid,
    /* [in] */ PROPID pid,
    /* [in] */ const PROPVARIANT *pvar,
    /* [in] */ PROPERTYUI_FORMAT_FLAGS flags,
    /* [size_is][out] */ LPWSTR pwszText,
    /* [in] */ DWORD cchText);


void __RPC_STUB IPropertyUI_FormatForDisplay_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IPropertyUI_GetHelpInfo_Proxy( 
    IPropertyUI * This,
    /* [in] */ REFFMTID fmtid,
    /* [in] */ PROPID pid,
    /* [size_is][out] */ LPWSTR pwszHelpFile,
    /* [in] */ DWORD cch,
    /* [out] */ UINT *puHelpID);


void __RPC_STUB IPropertyUI_GetHelpInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IPropertyUI_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0210 */
/* [local] */ 

#if (_WIN32_IE >= 0x0500)


extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0210_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0210_v0_0_s_ifspec;

#ifndef __ICategoryProvider_INTERFACE_DEFINED__
#define __ICategoryProvider_INTERFACE_DEFINED__

/* interface ICategoryProvider */
/* [unique][object][uuid][helpstring] */ 


EXTERN_C const IID IID_ICategoryProvider;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9af64809-5864-4c26-a720-c1f78c086ee3")
    ICategoryProvider : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE CanCategorizeOnSCID( 
            SHCOLUMNID *pscid) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetDefaultCategory( 
            GUID *pguid,
            SHCOLUMNID *pscid) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetCategoryForSCID( 
            SHCOLUMNID *pscid,
            GUID *pguid) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE EnumCategories( 
            IEnumGUID **penum) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetCategoryName( 
            GUID *pguid,
            LPWSTR pszName,
            UINT cch) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE CreateCategory( 
            GUID *pguid,
            REFIID riid,
            /* [iid_is] */ void **ppv) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICategoryProviderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICategoryProvider * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICategoryProvider * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICategoryProvider * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *CanCategorizeOnSCID )( 
            ICategoryProvider * This,
            SHCOLUMNID *pscid);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetDefaultCategory )( 
            ICategoryProvider * This,
            GUID *pguid,
            SHCOLUMNID *pscid);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetCategoryForSCID )( 
            ICategoryProvider * This,
            SHCOLUMNID *pscid,
            GUID *pguid);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *EnumCategories )( 
            ICategoryProvider * This,
            IEnumGUID **penum);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetCategoryName )( 
            ICategoryProvider * This,
            GUID *pguid,
            LPWSTR pszName,
            UINT cch);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *CreateCategory )( 
            ICategoryProvider * This,
            GUID *pguid,
            REFIID riid,
            /* [iid_is] */ void **ppv);
        
        END_INTERFACE
    } ICategoryProviderVtbl;

    interface ICategoryProvider
    {
        CONST_VTBL struct ICategoryProviderVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICategoryProvider_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICategoryProvider_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICategoryProvider_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICategoryProvider_CanCategorizeOnSCID(This,pscid)	\
    (This)->lpVtbl -> CanCategorizeOnSCID(This,pscid)

#define ICategoryProvider_GetDefaultCategory(This,pguid,pscid)	\
    (This)->lpVtbl -> GetDefaultCategory(This,pguid,pscid)

#define ICategoryProvider_GetCategoryForSCID(This,pscid,pguid)	\
    (This)->lpVtbl -> GetCategoryForSCID(This,pscid,pguid)

#define ICategoryProvider_EnumCategories(This,penum)	\
    (This)->lpVtbl -> EnumCategories(This,penum)

#define ICategoryProvider_GetCategoryName(This,pguid,pszName,cch)	\
    (This)->lpVtbl -> GetCategoryName(This,pguid,pszName,cch)

#define ICategoryProvider_CreateCategory(This,pguid,riid,ppv)	\
    (This)->lpVtbl -> CreateCategory(This,pguid,riid,ppv)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE ICategoryProvider_CanCategorizeOnSCID_Proxy( 
    ICategoryProvider * This,
    SHCOLUMNID *pscid);


void __RPC_STUB ICategoryProvider_CanCategorizeOnSCID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE ICategoryProvider_GetDefaultCategory_Proxy( 
    ICategoryProvider * This,
    GUID *pguid,
    SHCOLUMNID *pscid);


void __RPC_STUB ICategoryProvider_GetDefaultCategory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE ICategoryProvider_GetCategoryForSCID_Proxy( 
    ICategoryProvider * This,
    SHCOLUMNID *pscid,
    GUID *pguid);


void __RPC_STUB ICategoryProvider_GetCategoryForSCID_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE ICategoryProvider_EnumCategories_Proxy( 
    ICategoryProvider * This,
    IEnumGUID **penum);


void __RPC_STUB ICategoryProvider_EnumCategories_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE ICategoryProvider_GetCategoryName_Proxy( 
    ICategoryProvider * This,
    GUID *pguid,
    LPWSTR pszName,
    UINT cch);


void __RPC_STUB ICategoryProvider_GetCategoryName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE ICategoryProvider_CreateCategory_Proxy( 
    ICategoryProvider * This,
    GUID *pguid,
    REFIID riid,
    /* [iid_is] */ void **ppv);


void __RPC_STUB ICategoryProvider_CreateCategory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICategoryProvider_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0211 */
/* [local] */ 

typedef /* [public][public][public][v1_enum] */ 
enum __MIDL___MIDL_itf_shobjidl_0211_0001
    {	CATINFO_NORMAL	= 0,
	CATINFO_COLLAPSED	= 0x1,
	CATINFO_HIDDEN	= 0x2
    } 	CATEGORYINFO_FLAGS;

typedef /* [public][public][v1_enum] */ 
enum __MIDL___MIDL_itf_shobjidl_0211_0002
    {	CATSORT_DEFAULT	= 0,
	CATSORT_NAME	= 0x1
    } 	CATSORT_FLAGS;

typedef /* [public][public] */ struct __MIDL___MIDL_itf_shobjidl_0211_0003
    {
    CATEGORYINFO_FLAGS cif;
    WCHAR wszName[ 260 ];
    } 	CATEGORY_INFO;



extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0211_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0211_v0_0_s_ifspec;

#ifndef __ICategorizer_INTERFACE_DEFINED__
#define __ICategorizer_INTERFACE_DEFINED__

/* interface ICategorizer */
/* [unique][object][uuid][helpstring] */ 


EXTERN_C const IID IID_ICategorizer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("a3b14589-9174-49a8-89a3-06a1ae2b9ba7")
    ICategorizer : public IUnknown
    {
    public:
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetDescription( 
            LPWSTR pszDesc,
            UINT cch) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetCategory( 
            /* [in] */ UINT cidl,
            /* [size_is][in] */ LPCITEMIDLIST *apidl,
            /* [size_is][out][in] */ DWORD *rgCategoryIds) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE GetCategoryInfo( 
            DWORD dwCategoryId,
            CATEGORY_INFO *pci) = 0;
        
        virtual /* [helpstring] */ HRESULT STDMETHODCALLTYPE CompareCategory( 
            CATSORT_FLAGS csfFlags,
            DWORD dwCategoryId1,
            DWORD dwCategoryId2) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICategorizerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICategorizer * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICategorizer * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICategorizer * This);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetDescription )( 
            ICategorizer * This,
            LPWSTR pszDesc,
            UINT cch);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetCategory )( 
            ICategorizer * This,
            /* [in] */ UINT cidl,
            /* [size_is][in] */ LPCITEMIDLIST *apidl,
            /* [size_is][out][in] */ DWORD *rgCategoryIds);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *GetCategoryInfo )( 
            ICategorizer * This,
            DWORD dwCategoryId,
            CATEGORY_INFO *pci);
        
        /* [helpstring] */ HRESULT ( STDMETHODCALLTYPE *CompareCategory )( 
            ICategorizer * This,
            CATSORT_FLAGS csfFlags,
            DWORD dwCategoryId1,
            DWORD dwCategoryId2);
        
        END_INTERFACE
    } ICategorizerVtbl;

    interface ICategorizer
    {
        CONST_VTBL struct ICategorizerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICategorizer_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICategorizer_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICategorizer_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICategorizer_GetDescription(This,pszDesc,cch)	\
    (This)->lpVtbl -> GetDescription(This,pszDesc,cch)

#define ICategorizer_GetCategory(This,cidl,apidl,rgCategoryIds)	\
    (This)->lpVtbl -> GetCategory(This,cidl,apidl,rgCategoryIds)

#define ICategorizer_GetCategoryInfo(This,dwCategoryId,pci)	\
    (This)->lpVtbl -> GetCategoryInfo(This,dwCategoryId,pci)

#define ICategorizer_CompareCategory(This,csfFlags,dwCategoryId1,dwCategoryId2)	\
    (This)->lpVtbl -> CompareCategory(This,csfFlags,dwCategoryId1,dwCategoryId2)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring] */ HRESULT STDMETHODCALLTYPE ICategorizer_GetDescription_Proxy( 
    ICategorizer * This,
    LPWSTR pszDesc,
    UINT cch);


void __RPC_STUB ICategorizer_GetDescription_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE ICategorizer_GetCategory_Proxy( 
    ICategorizer * This,
    /* [in] */ UINT cidl,
    /* [size_is][in] */ LPCITEMIDLIST *apidl,
    /* [size_is][out][in] */ DWORD *rgCategoryIds);


void __RPC_STUB ICategorizer_GetCategory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE ICategorizer_GetCategoryInfo_Proxy( 
    ICategorizer * This,
    DWORD dwCategoryId,
    CATEGORY_INFO *pci);


void __RPC_STUB ICategorizer_GetCategoryInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring] */ HRESULT STDMETHODCALLTYPE ICategorizer_CompareCategory_Proxy( 
    ICategorizer * This,
    CATSORT_FLAGS csfFlags,
    DWORD dwCategoryId1,
    DWORD dwCategoryId2);


void __RPC_STUB ICategorizer_CompareCategory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICategorizer_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0212 */
/* [local] */ 

#endif // (_WIN32_IE >= 0x0500)
#ifdef UNICODE
#define IShellLink      IShellLinkW
#else
#define IShellLink      IShellLinkA
#endif
typedef /* [public] */ 
enum __MIDL___MIDL_itf_shobjidl_0212_0001
    {	SLR_NO_UI	= 0x1,
	SLR_ANY_MATCH	= 0x2,
	SLR_UPDATE	= 0x4,
	SLR_NOUPDATE	= 0x8,
	SLR_NOSEARCH	= 0x10,
	SLR_NOTRACK	= 0x20,
	SLR_NOLINKINFO	= 0x40,
	SLR_INVOKE_MSI	= 0x80,
	SLR_NO_UI_WITH_MSG_PUMP	= 0x101
    } 	SLR_FLAGS;

typedef /* [public] */ 
enum __MIDL___MIDL_itf_shobjidl_0212_0002
    {	SLGP_SHORTPATH	= 0x1,
	SLGP_UNCPRIORITY	= 0x2,
	SLGP_RAWPATH	= 0x4
    } 	SLGP_FLAGS;



extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0212_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0212_v0_0_s_ifspec;

#ifndef __IShellLinkA_INTERFACE_DEFINED__
#define __IShellLinkA_INTERFACE_DEFINED__

/* interface IShellLinkA */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IShellLinkA;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("000214EE-0000-0000-C000-000000000046")
    IShellLinkA : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetPath( 
            /* [size_is][out] */ LPSTR pszFile,
            /* [in] */ int cch,
            /* [full][out][in] */ WIN32_FIND_DATAA *pfd,
            /* [in] */ DWORD fFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetIDList( 
            /* [out] */ LPITEMIDLIST *ppidl) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetIDList( 
            /* [in] */ LPCITEMIDLIST pidl) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDescription( 
            /* [size_is][out] */ LPSTR pszName,
            /* [in] */ int cch) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetDescription( 
            /* [in] */ LPCSTR pszName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetWorkingDirectory( 
            /* [size_is][out] */ LPSTR pszDir,
            /* [in] */ int cch) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetWorkingDirectory( 
            /* [in] */ LPCSTR pszDir) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetArguments( 
            /* [size_is][out] */ LPSTR pszArgs,
            /* [in] */ int cch) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetArguments( 
            /* [in] */ LPCSTR pszArgs) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetHotkey( 
            /* [out] */ WORD *pwHotkey) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetHotkey( 
            /* [in] */ WORD wHotkey) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetShowCmd( 
            /* [out] */ int *piShowCmd) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetShowCmd( 
            /* [in] */ int iShowCmd) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetIconLocation( 
            /* [size_is][out] */ LPSTR pszIconPath,
            /* [in] */ int cch,
            /* [out] */ int *piIcon) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetIconLocation( 
            /* [in] */ LPCSTR pszIconPath,
            /* [in] */ int iIcon) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetRelativePath( 
            /* [in] */ LPCSTR pszPathRel,
            /* [in] */ DWORD dwReserved) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Resolve( 
            /* [in] */ HWND hwnd,
            /* [in] */ DWORD fFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetPath( 
            /* [in] */ LPCSTR pszFile) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IShellLinkAVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IShellLinkA * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IShellLinkA * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IShellLinkA * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetPath )( 
            IShellLinkA * This,
            /* [size_is][out] */ LPSTR pszFile,
            /* [in] */ int cch,
            /* [full][out][in] */ WIN32_FIND_DATAA *pfd,
            /* [in] */ DWORD fFlags);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDList )( 
            IShellLinkA * This,
            /* [out] */ LPITEMIDLIST *ppidl);
        
        HRESULT ( STDMETHODCALLTYPE *SetIDList )( 
            IShellLinkA * This,
            /* [in] */ LPCITEMIDLIST pidl);
        
        HRESULT ( STDMETHODCALLTYPE *GetDescription )( 
            IShellLinkA * This,
            /* [size_is][out] */ LPSTR pszName,
            /* [in] */ int cch);
        
        HRESULT ( STDMETHODCALLTYPE *SetDescription )( 
            IShellLinkA * This,
            /* [in] */ LPCSTR pszName);
        
        HRESULT ( STDMETHODCALLTYPE *GetWorkingDirectory )( 
            IShellLinkA * This,
            /* [size_is][out] */ LPSTR pszDir,
            /* [in] */ int cch);
        
        HRESULT ( STDMETHODCALLTYPE *SetWorkingDirectory )( 
            IShellLinkA * This,
            /* [in] */ LPCSTR pszDir);
        
        HRESULT ( STDMETHODCALLTYPE *GetArguments )( 
            IShellLinkA * This,
            /* [size_is][out] */ LPSTR pszArgs,
            /* [in] */ int cch);
        
        HRESULT ( STDMETHODCALLTYPE *SetArguments )( 
            IShellLinkA * This,
            /* [in] */ LPCSTR pszArgs);
        
        HRESULT ( STDMETHODCALLTYPE *GetHotkey )( 
            IShellLinkA * This,
            /* [out] */ WORD *pwHotkey);
        
        HRESULT ( STDMETHODCALLTYPE *SetHotkey )( 
            IShellLinkA * This,
            /* [in] */ WORD wHotkey);
        
        HRESULT ( STDMETHODCALLTYPE *GetShowCmd )( 
            IShellLinkA * This,
            /* [out] */ int *piShowCmd);
        
        HRESULT ( STDMETHODCALLTYPE *SetShowCmd )( 
            IShellLinkA * This,
            /* [in] */ int iShowCmd);
        
        HRESULT ( STDMETHODCALLTYPE *GetIconLocation )( 
            IShellLinkA * This,
            /* [size_is][out] */ LPSTR pszIconPath,
            /* [in] */ int cch,
            /* [out] */ int *piIcon);
        
        HRESULT ( STDMETHODCALLTYPE *SetIconLocation )( 
            IShellLinkA * This,
            /* [in] */ LPCSTR pszIconPath,
            /* [in] */ int iIcon);
        
        HRESULT ( STDMETHODCALLTYPE *SetRelativePath )( 
            IShellLinkA * This,
            /* [in] */ LPCSTR pszPathRel,
            /* [in] */ DWORD dwReserved);
        
        HRESULT ( STDMETHODCALLTYPE *Resolve )( 
            IShellLinkA * This,
            /* [in] */ HWND hwnd,
            /* [in] */ DWORD fFlags);
        
        HRESULT ( STDMETHODCALLTYPE *SetPath )( 
            IShellLinkA * This,
            /* [in] */ LPCSTR pszFile);
        
        END_INTERFACE
    } IShellLinkAVtbl;

    interface IShellLinkA
    {
        CONST_VTBL struct IShellLinkAVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IShellLinkA_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IShellLinkA_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IShellLinkA_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IShellLinkA_GetPath(This,pszFile,cch,pfd,fFlags)	\
    (This)->lpVtbl -> GetPath(This,pszFile,cch,pfd,fFlags)

#define IShellLinkA_GetIDList(This,ppidl)	\
    (This)->lpVtbl -> GetIDList(This,ppidl)

#define IShellLinkA_SetIDList(This,pidl)	\
    (This)->lpVtbl -> SetIDList(This,pidl)

#define IShellLinkA_GetDescription(This,pszName,cch)	\
    (This)->lpVtbl -> GetDescription(This,pszName,cch)

#define IShellLinkA_SetDescription(This,pszName)	\
    (This)->lpVtbl -> SetDescription(This,pszName)

#define IShellLinkA_GetWorkingDirectory(This,pszDir,cch)	\
    (This)->lpVtbl -> GetWorkingDirectory(This,pszDir,cch)

#define IShellLinkA_SetWorkingDirectory(This,pszDir)	\
    (This)->lpVtbl -> SetWorkingDirectory(This,pszDir)

#define IShellLinkA_GetArguments(This,pszArgs,cch)	\
    (This)->lpVtbl -> GetArguments(This,pszArgs,cch)

#define IShellLinkA_SetArguments(This,pszArgs)	\
    (This)->lpVtbl -> SetArguments(This,pszArgs)

#define IShellLinkA_GetHotkey(This,pwHotkey)	\
    (This)->lpVtbl -> GetHotkey(This,pwHotkey)

#define IShellLinkA_SetHotkey(This,wHotkey)	\
    (This)->lpVtbl -> SetHotkey(This,wHotkey)

#define IShellLinkA_GetShowCmd(This,piShowCmd)	\
    (This)->lpVtbl -> GetShowCmd(This,piShowCmd)

#define IShellLinkA_SetShowCmd(This,iShowCmd)	\
    (This)->lpVtbl -> SetShowCmd(This,iShowCmd)

#define IShellLinkA_GetIconLocation(This,pszIconPath,cch,piIcon)	\
    (This)->lpVtbl -> GetIconLocation(This,pszIconPath,cch,piIcon)

#define IShellLinkA_SetIconLocation(This,pszIconPath,iIcon)	\
    (This)->lpVtbl -> SetIconLocation(This,pszIconPath,iIcon)

#define IShellLinkA_SetRelativePath(This,pszPathRel,dwReserved)	\
    (This)->lpVtbl -> SetRelativePath(This,pszPathRel,dwReserved)

#define IShellLinkA_Resolve(This,hwnd,fFlags)	\
    (This)->lpVtbl -> Resolve(This,hwnd,fFlags)

#define IShellLinkA_SetPath(This,pszFile)	\
    (This)->lpVtbl -> SetPath(This,pszFile)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IShellLinkA_GetPath_Proxy( 
    IShellLinkA * This,
    /* [size_is][out] */ LPSTR pszFile,
    /* [in] */ int cch,
    /* [full][out][in] */ WIN32_FIND_DATAA *pfd,
    /* [in] */ DWORD fFlags);


void __RPC_STUB IShellLinkA_GetPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkA_GetIDList_Proxy( 
    IShellLinkA * This,
    /* [out] */ LPITEMIDLIST *ppidl);


void __RPC_STUB IShellLinkA_GetIDList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkA_SetIDList_Proxy( 
    IShellLinkA * This,
    /* [in] */ LPCITEMIDLIST pidl);


void __RPC_STUB IShellLinkA_SetIDList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkA_GetDescription_Proxy( 
    IShellLinkA * This,
    /* [size_is][out] */ LPSTR pszName,
    /* [in] */ int cch);


void __RPC_STUB IShellLinkA_GetDescription_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkA_SetDescription_Proxy( 
    IShellLinkA * This,
    /* [in] */ LPCSTR pszName);


void __RPC_STUB IShellLinkA_SetDescription_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkA_GetWorkingDirectory_Proxy( 
    IShellLinkA * This,
    /* [size_is][out] */ LPSTR pszDir,
    /* [in] */ int cch);


void __RPC_STUB IShellLinkA_GetWorkingDirectory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkA_SetWorkingDirectory_Proxy( 
    IShellLinkA * This,
    /* [in] */ LPCSTR pszDir);


void __RPC_STUB IShellLinkA_SetWorkingDirectory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkA_GetArguments_Proxy( 
    IShellLinkA * This,
    /* [size_is][out] */ LPSTR pszArgs,
    /* [in] */ int cch);


void __RPC_STUB IShellLinkA_GetArguments_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkA_SetArguments_Proxy( 
    IShellLinkA * This,
    /* [in] */ LPCSTR pszArgs);


void __RPC_STUB IShellLinkA_SetArguments_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkA_GetHotkey_Proxy( 
    IShellLinkA * This,
    /* [out] */ WORD *pwHotkey);


void __RPC_STUB IShellLinkA_GetHotkey_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkA_SetHotkey_Proxy( 
    IShellLinkA * This,
    /* [in] */ WORD wHotkey);


void __RPC_STUB IShellLinkA_SetHotkey_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkA_GetShowCmd_Proxy( 
    IShellLinkA * This,
    /* [out] */ int *piShowCmd);


void __RPC_STUB IShellLinkA_GetShowCmd_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkA_SetShowCmd_Proxy( 
    IShellLinkA * This,
    /* [in] */ int iShowCmd);


void __RPC_STUB IShellLinkA_SetShowCmd_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkA_GetIconLocation_Proxy( 
    IShellLinkA * This,
    /* [size_is][out] */ LPSTR pszIconPath,
    /* [in] */ int cch,
    /* [out] */ int *piIcon);


void __RPC_STUB IShellLinkA_GetIconLocation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkA_SetIconLocation_Proxy( 
    IShellLinkA * This,
    /* [in] */ LPCSTR pszIconPath,
    /* [in] */ int iIcon);


void __RPC_STUB IShellLinkA_SetIconLocation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkA_SetRelativePath_Proxy( 
    IShellLinkA * This,
    /* [in] */ LPCSTR pszPathRel,
    /* [in] */ DWORD dwReserved);


void __RPC_STUB IShellLinkA_SetRelativePath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkA_Resolve_Proxy( 
    IShellLinkA * This,
    /* [in] */ HWND hwnd,
    /* [in] */ DWORD fFlags);


void __RPC_STUB IShellLinkA_Resolve_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkA_SetPath_Proxy( 
    IShellLinkA * This,
    /* [in] */ LPCSTR pszFile);


void __RPC_STUB IShellLinkA_SetPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IShellLinkA_INTERFACE_DEFINED__ */


#ifndef __IShellLinkW_INTERFACE_DEFINED__
#define __IShellLinkW_INTERFACE_DEFINED__

/* interface IShellLinkW */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IShellLinkW;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("000214F9-0000-0000-C000-000000000046")
    IShellLinkW : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetPath( 
            /* [size_is][out] */ LPWSTR pszFile,
            /* [in] */ int cch,
            /* [full][out][in] */ WIN32_FIND_DATAW *pfd,
            /* [in] */ DWORD fFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetIDList( 
            /* [out] */ LPITEMIDLIST *ppidl) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetIDList( 
            /* [in] */ LPCITEMIDLIST pidl) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDescription( 
            /* [size_is][out] */ LPWSTR pszName,
            int cch) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetDescription( 
            /* [in] */ LPCWSTR pszName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetWorkingDirectory( 
            /* [size_is][out] */ LPWSTR pszDir,
            int cch) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetWorkingDirectory( 
            /* [in] */ LPCWSTR pszDir) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetArguments( 
            /* [size_is][out] */ LPWSTR pszArgs,
            int cch) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetArguments( 
            /* [in] */ LPCWSTR pszArgs) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetHotkey( 
            /* [out] */ WORD *pwHotkey) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetHotkey( 
            /* [in] */ WORD wHotkey) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetShowCmd( 
            /* [out] */ int *piShowCmd) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetShowCmd( 
            /* [in] */ int iShowCmd) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetIconLocation( 
            /* [size_is][out] */ LPWSTR pszIconPath,
            /* [in] */ int cch,
            /* [out] */ int *piIcon) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetIconLocation( 
            /* [in] */ LPCWSTR pszIconPath,
            /* [in] */ int iIcon) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetRelativePath( 
            /* [in] */ LPCWSTR pszPathRel,
            /* [in] */ DWORD dwReserved) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Resolve( 
            /* [in] */ HWND hwnd,
            /* [in] */ DWORD fFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetPath( 
            /* [in] */ LPCWSTR pszFile) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IShellLinkWVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IShellLinkW * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IShellLinkW * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IShellLinkW * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetPath )( 
            IShellLinkW * This,
            /* [size_is][out] */ LPWSTR pszFile,
            /* [in] */ int cch,
            /* [full][out][in] */ WIN32_FIND_DATAW *pfd,
            /* [in] */ DWORD fFlags);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDList )( 
            IShellLinkW * This,
            /* [out] */ LPITEMIDLIST *ppidl);
        
        HRESULT ( STDMETHODCALLTYPE *SetIDList )( 
            IShellLinkW * This,
            /* [in] */ LPCITEMIDLIST pidl);
        
        HRESULT ( STDMETHODCALLTYPE *GetDescription )( 
            IShellLinkW * This,
            /* [size_is][out] */ LPWSTR pszName,
            int cch);
        
        HRESULT ( STDMETHODCALLTYPE *SetDescription )( 
            IShellLinkW * This,
            /* [in] */ LPCWSTR pszName);
        
        HRESULT ( STDMETHODCALLTYPE *GetWorkingDirectory )( 
            IShellLinkW * This,
            /* [size_is][out] */ LPWSTR pszDir,
            int cch);
        
        HRESULT ( STDMETHODCALLTYPE *SetWorkingDirectory )( 
            IShellLinkW * This,
            /* [in] */ LPCWSTR pszDir);
        
        HRESULT ( STDMETHODCALLTYPE *GetArguments )( 
            IShellLinkW * This,
            /* [size_is][out] */ LPWSTR pszArgs,
            int cch);
        
        HRESULT ( STDMETHODCALLTYPE *SetArguments )( 
            IShellLinkW * This,
            /* [in] */ LPCWSTR pszArgs);
        
        HRESULT ( STDMETHODCALLTYPE *GetHotkey )( 
            IShellLinkW * This,
            /* [out] */ WORD *pwHotkey);
        
        HRESULT ( STDMETHODCALLTYPE *SetHotkey )( 
            IShellLinkW * This,
            /* [in] */ WORD wHotkey);
        
        HRESULT ( STDMETHODCALLTYPE *GetShowCmd )( 
            IShellLinkW * This,
            /* [out] */ int *piShowCmd);
        
        HRESULT ( STDMETHODCALLTYPE *SetShowCmd )( 
            IShellLinkW * This,
            /* [in] */ int iShowCmd);
        
        HRESULT ( STDMETHODCALLTYPE *GetIconLocation )( 
            IShellLinkW * This,
            /* [size_is][out] */ LPWSTR pszIconPath,
            /* [in] */ int cch,
            /* [out] */ int *piIcon);
        
        HRESULT ( STDMETHODCALLTYPE *SetIconLocation )( 
            IShellLinkW * This,
            /* [in] */ LPCWSTR pszIconPath,
            /* [in] */ int iIcon);
        
        HRESULT ( STDMETHODCALLTYPE *SetRelativePath )( 
            IShellLinkW * This,
            /* [in] */ LPCWSTR pszPathRel,
            /* [in] */ DWORD dwReserved);
        
        HRESULT ( STDMETHODCALLTYPE *Resolve )( 
            IShellLinkW * This,
            /* [in] */ HWND hwnd,
            /* [in] */ DWORD fFlags);
        
        HRESULT ( STDMETHODCALLTYPE *SetPath )( 
            IShellLinkW * This,
            /* [in] */ LPCWSTR pszFile);
        
        END_INTERFACE
    } IShellLinkWVtbl;

    interface IShellLinkW
    {
        CONST_VTBL struct IShellLinkWVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IShellLinkW_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IShellLinkW_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IShellLinkW_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IShellLinkW_GetPath(This,pszFile,cch,pfd,fFlags)	\
    (This)->lpVtbl -> GetPath(This,pszFile,cch,pfd,fFlags)

#define IShellLinkW_GetIDList(This,ppidl)	\
    (This)->lpVtbl -> GetIDList(This,ppidl)

#define IShellLinkW_SetIDList(This,pidl)	\
    (This)->lpVtbl -> SetIDList(This,pidl)

#define IShellLinkW_GetDescription(This,pszName,cch)	\
    (This)->lpVtbl -> GetDescription(This,pszName,cch)

#define IShellLinkW_SetDescription(This,pszName)	\
    (This)->lpVtbl -> SetDescription(This,pszName)

#define IShellLinkW_GetWorkingDirectory(This,pszDir,cch)	\
    (This)->lpVtbl -> GetWorkingDirectory(This,pszDir,cch)

#define IShellLinkW_SetWorkingDirectory(This,pszDir)	\
    (This)->lpVtbl -> SetWorkingDirectory(This,pszDir)

#define IShellLinkW_GetArguments(This,pszArgs,cch)	\
    (This)->lpVtbl -> GetArguments(This,pszArgs,cch)

#define IShellLinkW_SetArguments(This,pszArgs)	\
    (This)->lpVtbl -> SetArguments(This,pszArgs)

#define IShellLinkW_GetHotkey(This,pwHotkey)	\
    (This)->lpVtbl -> GetHotkey(This,pwHotkey)

#define IShellLinkW_SetHotkey(This,wHotkey)	\
    (This)->lpVtbl -> SetHotkey(This,wHotkey)

#define IShellLinkW_GetShowCmd(This,piShowCmd)	\
    (This)->lpVtbl -> GetShowCmd(This,piShowCmd)

#define IShellLinkW_SetShowCmd(This,iShowCmd)	\
    (This)->lpVtbl -> SetShowCmd(This,iShowCmd)

#define IShellLinkW_GetIconLocation(This,pszIconPath,cch,piIcon)	\
    (This)->lpVtbl -> GetIconLocation(This,pszIconPath,cch,piIcon)

#define IShellLinkW_SetIconLocation(This,pszIconPath,iIcon)	\
    (This)->lpVtbl -> SetIconLocation(This,pszIconPath,iIcon)

#define IShellLinkW_SetRelativePath(This,pszPathRel,dwReserved)	\
    (This)->lpVtbl -> SetRelativePath(This,pszPathRel,dwReserved)

#define IShellLinkW_Resolve(This,hwnd,fFlags)	\
    (This)->lpVtbl -> Resolve(This,hwnd,fFlags)

#define IShellLinkW_SetPath(This,pszFile)	\
    (This)->lpVtbl -> SetPath(This,pszFile)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IShellLinkW_GetPath_Proxy( 
    IShellLinkW * This,
    /* [size_is][out] */ LPWSTR pszFile,
    /* [in] */ int cch,
    /* [full][out][in] */ WIN32_FIND_DATAW *pfd,
    /* [in] */ DWORD fFlags);


void __RPC_STUB IShellLinkW_GetPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkW_GetIDList_Proxy( 
    IShellLinkW * This,
    /* [out] */ LPITEMIDLIST *ppidl);


void __RPC_STUB IShellLinkW_GetIDList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkW_SetIDList_Proxy( 
    IShellLinkW * This,
    /* [in] */ LPCITEMIDLIST pidl);


void __RPC_STUB IShellLinkW_SetIDList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkW_GetDescription_Proxy( 
    IShellLinkW * This,
    /* [size_is][out] */ LPWSTR pszName,
    int cch);


void __RPC_STUB IShellLinkW_GetDescription_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkW_SetDescription_Proxy( 
    IShellLinkW * This,
    /* [in] */ LPCWSTR pszName);


void __RPC_STUB IShellLinkW_SetDescription_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkW_GetWorkingDirectory_Proxy( 
    IShellLinkW * This,
    /* [size_is][out] */ LPWSTR pszDir,
    int cch);


void __RPC_STUB IShellLinkW_GetWorkingDirectory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkW_SetWorkingDirectory_Proxy( 
    IShellLinkW * This,
    /* [in] */ LPCWSTR pszDir);


void __RPC_STUB IShellLinkW_SetWorkingDirectory_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkW_GetArguments_Proxy( 
    IShellLinkW * This,
    /* [size_is][out] */ LPWSTR pszArgs,
    int cch);


void __RPC_STUB IShellLinkW_GetArguments_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkW_SetArguments_Proxy( 
    IShellLinkW * This,
    /* [in] */ LPCWSTR pszArgs);


void __RPC_STUB IShellLinkW_SetArguments_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkW_GetHotkey_Proxy( 
    IShellLinkW * This,
    /* [out] */ WORD *pwHotkey);


void __RPC_STUB IShellLinkW_GetHotkey_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkW_SetHotkey_Proxy( 
    IShellLinkW * This,
    /* [in] */ WORD wHotkey);


void __RPC_STUB IShellLinkW_SetHotkey_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkW_GetShowCmd_Proxy( 
    IShellLinkW * This,
    /* [out] */ int *piShowCmd);


void __RPC_STUB IShellLinkW_GetShowCmd_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkW_SetShowCmd_Proxy( 
    IShellLinkW * This,
    /* [in] */ int iShowCmd);


void __RPC_STUB IShellLinkW_SetShowCmd_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkW_GetIconLocation_Proxy( 
    IShellLinkW * This,
    /* [size_is][out] */ LPWSTR pszIconPath,
    /* [in] */ int cch,
    /* [out] */ int *piIcon);


void __RPC_STUB IShellLinkW_GetIconLocation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkW_SetIconLocation_Proxy( 
    IShellLinkW * This,
    /* [in] */ LPCWSTR pszIconPath,
    /* [in] */ int iIcon);


void __RPC_STUB IShellLinkW_SetIconLocation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkW_SetRelativePath_Proxy( 
    IShellLinkW * This,
    /* [in] */ LPCWSTR pszPathRel,
    /* [in] */ DWORD dwReserved);


void __RPC_STUB IShellLinkW_SetRelativePath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkW_Resolve_Proxy( 
    IShellLinkW * This,
    /* [in] */ HWND hwnd,
    /* [in] */ DWORD fFlags);


void __RPC_STUB IShellLinkW_Resolve_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellLinkW_SetPath_Proxy( 
    IShellLinkW * This,
    /* [in] */ LPCWSTR pszFile);


void __RPC_STUB IShellLinkW_SetPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IShellLinkW_INTERFACE_DEFINED__ */


#ifndef __IActionProgressDialog_INTERFACE_DEFINED__
#define __IActionProgressDialog_INTERFACE_DEFINED__

/* interface IActionProgressDialog */
/* [unique][helpstring][uuid][object] */ 

/* [v1_enum] */ 
enum __MIDL_IActionProgressDialog_0001
    {	SPINITF_NORMAL	= 0,
	SPINITF_MODAL	= 0x1,
	SPINITF_NOMINIMIZE	= 0x8
    } ;
typedef DWORD SPINITF;


EXTERN_C const IID IID_IActionProgressDialog;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("49ff1172-eadc-446d-9285-156453a6431c")
    IActionProgressDialog : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Initialize( 
            /* [in] */ SPINITF flags,
            /* [string][in] */ LPCWSTR pszTitle,
            /* [string][in] */ LPCWSTR pszCancel) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Stop( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IActionProgressDialogVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IActionProgressDialog * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IActionProgressDialog * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IActionProgressDialog * This);
        
        HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            IActionProgressDialog * This,
            /* [in] */ SPINITF flags,
            /* [string][in] */ LPCWSTR pszTitle,
            /* [string][in] */ LPCWSTR pszCancel);
        
        HRESULT ( STDMETHODCALLTYPE *Stop )( 
            IActionProgressDialog * This);
        
        END_INTERFACE
    } IActionProgressDialogVtbl;

    interface IActionProgressDialog
    {
        CONST_VTBL struct IActionProgressDialogVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IActionProgressDialog_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IActionProgressDialog_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IActionProgressDialog_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IActionProgressDialog_Initialize(This,flags,pszTitle,pszCancel)	\
    (This)->lpVtbl -> Initialize(This,flags,pszTitle,pszCancel)

#define IActionProgressDialog_Stop(This)	\
    (This)->lpVtbl -> Stop(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IActionProgressDialog_Initialize_Proxy( 
    IActionProgressDialog * This,
    /* [in] */ SPINITF flags,
    /* [string][in] */ LPCWSTR pszTitle,
    /* [string][in] */ LPCWSTR pszCancel);


void __RPC_STUB IActionProgressDialog_Initialize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IActionProgressDialog_Stop_Proxy( 
    IActionProgressDialog * This);


void __RPC_STUB IActionProgressDialog_Stop_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IActionProgressDialog_INTERFACE_DEFINED__ */


#ifndef __IHWEventHandler_INTERFACE_DEFINED__
#define __IHWEventHandler_INTERFACE_DEFINED__

/* interface IHWEventHandler */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IHWEventHandler;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("C1FB73D0-EC3A-4ba2-B512-8CDB9187B6D1")
    IHWEventHandler : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Initialize( 
            /* [string][in] */ LPCWSTR pszParams) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE HandleEvent( 
            /* [string][in] */ LPCWSTR pszDeviceID,
            /* [string][in] */ LPCWSTR pszAltDeviceID,
            /* [string][in] */ LPCWSTR pszEventType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE HandleEventWithContent( 
            /* [string][in] */ LPCWSTR pszDeviceID,
            /* [string][in] */ LPCWSTR pszAltDeviceID,
            /* [string][in] */ LPCWSTR pszEventType,
            /* [string][in] */ LPCWSTR pszContentTypeHandler,
            /* [in] */ IDataObject *pdataobject) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IHWEventHandlerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IHWEventHandler * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IHWEventHandler * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IHWEventHandler * This);
        
        HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            IHWEventHandler * This,
            /* [string][in] */ LPCWSTR pszParams);
        
        HRESULT ( STDMETHODCALLTYPE *HandleEvent )( 
            IHWEventHandler * This,
            /* [string][in] */ LPCWSTR pszDeviceID,
            /* [string][in] */ LPCWSTR pszAltDeviceID,
            /* [string][in] */ LPCWSTR pszEventType);
        
        HRESULT ( STDMETHODCALLTYPE *HandleEventWithContent )( 
            IHWEventHandler * This,
            /* [string][in] */ LPCWSTR pszDeviceID,
            /* [string][in] */ LPCWSTR pszAltDeviceID,
            /* [string][in] */ LPCWSTR pszEventType,
            /* [string][in] */ LPCWSTR pszContentTypeHandler,
            /* [in] */ IDataObject *pdataobject);
        
        END_INTERFACE
    } IHWEventHandlerVtbl;

    interface IHWEventHandler
    {
        CONST_VTBL struct IHWEventHandlerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IHWEventHandler_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IHWEventHandler_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IHWEventHandler_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IHWEventHandler_Initialize(This,pszParams)	\
    (This)->lpVtbl -> Initialize(This,pszParams)

#define IHWEventHandler_HandleEvent(This,pszDeviceID,pszAltDeviceID,pszEventType)	\
    (This)->lpVtbl -> HandleEvent(This,pszDeviceID,pszAltDeviceID,pszEventType)

#define IHWEventHandler_HandleEventWithContent(This,pszDeviceID,pszAltDeviceID,pszEventType,pszContentTypeHandler,pdataobject)	\
    (This)->lpVtbl -> HandleEventWithContent(This,pszDeviceID,pszAltDeviceID,pszEventType,pszContentTypeHandler,pdataobject)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IHWEventHandler_Initialize_Proxy( 
    IHWEventHandler * This,
    /* [string][in] */ LPCWSTR pszParams);


void __RPC_STUB IHWEventHandler_Initialize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IHWEventHandler_HandleEvent_Proxy( 
    IHWEventHandler * This,
    /* [string][in] */ LPCWSTR pszDeviceID,
    /* [string][in] */ LPCWSTR pszAltDeviceID,
    /* [string][in] */ LPCWSTR pszEventType);


void __RPC_STUB IHWEventHandler_HandleEvent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IHWEventHandler_HandleEventWithContent_Proxy( 
    IHWEventHandler * This,
    /* [string][in] */ LPCWSTR pszDeviceID,
    /* [string][in] */ LPCWSTR pszAltDeviceID,
    /* [string][in] */ LPCWSTR pszEventType,
    /* [string][in] */ LPCWSTR pszContentTypeHandler,
    /* [in] */ IDataObject *pdataobject);


void __RPC_STUB IHWEventHandler_HandleEventWithContent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IHWEventHandler_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0216 */
/* [local] */ 

#define ARCONTENT_AUTORUNINF         0x00000002
#define ARCONTENT_AUDIOCD         0x00000004
#define ARCONTENT_DVDMOVIE        0x00000008
#define ARCONTENT_BLANKCD         0x00000010
#define ARCONTENT_BLANKDVD        0x00000020
#define ARCONTENT_UNKNOWNCONTENT     0x00000040
#define ARCONTENT_AUTOPLAYPIX     0x00000080
#define ARCONTENT_AUTOPLAYMUSIC      0x00000100
#define ARCONTENT_AUTOPLAYVIDEO      0x00000200


extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0216_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0216_v0_0_s_ifspec;

#ifndef __IQueryCancelAutoPlay_INTERFACE_DEFINED__
#define __IQueryCancelAutoPlay_INTERFACE_DEFINED__

/* interface IQueryCancelAutoPlay */
/* [unique][helpstring][uuid][object] */ 


EXTERN_C const IID IID_IQueryCancelAutoPlay;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("DDEFE873-6997-4e68-BE26-39B633ADBE12")
    IQueryCancelAutoPlay : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE AllowAutoPlay( 
            /* [string][in] */ LPCWSTR pszPath,
            /* [in] */ DWORD dwContentType,
            /* [string][in] */ LPCWSTR pszLabel,
            /* [in] */ DWORD dwSerialNumber) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IQueryCancelAutoPlayVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IQueryCancelAutoPlay * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IQueryCancelAutoPlay * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IQueryCancelAutoPlay * This);
        
        HRESULT ( STDMETHODCALLTYPE *AllowAutoPlay )( 
            IQueryCancelAutoPlay * This,
            /* [string][in] */ LPCWSTR pszPath,
            /* [in] */ DWORD dwContentType,
            /* [string][in] */ LPCWSTR pszLabel,
            /* [in] */ DWORD dwSerialNumber);
        
        END_INTERFACE
    } IQueryCancelAutoPlayVtbl;

    interface IQueryCancelAutoPlay
    {
        CONST_VTBL struct IQueryCancelAutoPlayVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IQueryCancelAutoPlay_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IQueryCancelAutoPlay_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IQueryCancelAutoPlay_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IQueryCancelAutoPlay_AllowAutoPlay(This,pszPath,dwContentType,pszLabel,dwSerialNumber)	\
    (This)->lpVtbl -> AllowAutoPlay(This,pszPath,dwContentType,pszLabel,dwSerialNumber)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IQueryCancelAutoPlay_AllowAutoPlay_Proxy( 
    IQueryCancelAutoPlay * This,
    /* [string][in] */ LPCWSTR pszPath,
    /* [in] */ DWORD dwContentType,
    /* [string][in] */ LPCWSTR pszLabel,
    /* [in] */ DWORD dwSerialNumber);


void __RPC_STUB IQueryCancelAutoPlay_AllowAutoPlay_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IQueryCancelAutoPlay_INTERFACE_DEFINED__ */


#ifndef __IActionProgress_INTERFACE_DEFINED__
#define __IActionProgress_INTERFACE_DEFINED__

/* interface IActionProgress */
/* [unique][helpstring][uuid][object] */ 

/* [v1_enum] */ 
enum __MIDL_IActionProgress_0001
    {	SPBEGINF_NORMAL	= 0,
	SPBEGINF_AUTOTIME	= 0x2,
	SPBEGINF_NOPROGRESSBAR	= 0x10,
	SPBEGINF_MARQUEEPROGRESS	= 0x20
    } ;
typedef DWORD SPBEGINF;

typedef /* [v1_enum] */ 
enum _SPACTION
    {	SPACTION_NONE	= 0,
	SPACTION_MOVING	= SPACTION_NONE + 1,
	SPACTION_COPYING	= SPACTION_MOVING + 1,
	SPACTION_RECYCLING	= SPACTION_COPYING + 1,
	SPACTION_APPLYINGATTRIBS	= SPACTION_RECYCLING + 1,
	SPACTION_DOWNLOADING	= SPACTION_APPLYINGATTRIBS + 1,
	SPACTION_SEARCHING_INTERNET	= SPACTION_DOWNLOADING + 1,
	SPACTION_CALCULATING	= SPACTION_SEARCHING_INTERNET + 1,
	SPACTION_UPLOADING	= SPACTION_CALCULATING + 1,
	SPACTION_SEARCHING_FILES	= SPACTION_UPLOADING + 1
    } 	SPACTION;

typedef /* [v1_enum] */ 
enum _SPTEXT
    {	SPTEXT_ACTIONDESCRIPTION	= 1,
	SPTEXT_ACTIONDETAIL	= SPTEXT_ACTIONDESCRIPTION + 1
    } 	SPTEXT;


EXTERN_C const IID IID_IActionProgress;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("49ff1173-eadc-446d-9285-156453a6431c")
    IActionProgress : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Begin( 
            /* [in] */ SPACTION action,
            /* [in] */ SPBEGINF flags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UpdateProgress( 
            /* [in] */ ULONGLONG ulCompleted,
            /* [in] */ ULONGLONG ulTotal) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UpdateText( 
            /* [in] */ SPTEXT sptext,
            /* [string][in] */ LPCWSTR pszText,
            /* [in] */ BOOL fMayCompact) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE QueryCancel( 
            /* [out] */ BOOL *pfCancelled) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ResetCancel( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE End( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IActionProgressVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IActionProgress * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IActionProgress * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IActionProgress * This);
        
        HRESULT ( STDMETHODCALLTYPE *Begin )( 
            IActionProgress * This,
            /* [in] */ SPACTION action,
            /* [in] */ SPBEGINF flags);
        
        HRESULT ( STDMETHODCALLTYPE *UpdateProgress )( 
            IActionProgress * This,
            /* [in] */ ULONGLONG ulCompleted,
            /* [in] */ ULONGLONG ulTotal);
        
        HRESULT ( STDMETHODCALLTYPE *UpdateText )( 
            IActionProgress * This,
            /* [in] */ SPTEXT sptext,
            /* [string][in] */ LPCWSTR pszText,
            /* [in] */ BOOL fMayCompact);
        
        HRESULT ( STDMETHODCALLTYPE *QueryCancel )( 
            IActionProgress * This,
            /* [out] */ BOOL *pfCancelled);
        
        HRESULT ( STDMETHODCALLTYPE *ResetCancel )( 
            IActionProgress * This);
        
        HRESULT ( STDMETHODCALLTYPE *End )( 
            IActionProgress * This);
        
        END_INTERFACE
    } IActionProgressVtbl;

    interface IActionProgress
    {
        CONST_VTBL struct IActionProgressVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IActionProgress_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IActionProgress_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IActionProgress_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IActionProgress_Begin(This,action,flags)	\
    (This)->lpVtbl -> Begin(This,action,flags)

#define IActionProgress_UpdateProgress(This,ulCompleted,ulTotal)	\
    (This)->lpVtbl -> UpdateProgress(This,ulCompleted,ulTotal)

#define IActionProgress_UpdateText(This,sptext,pszText,fMayCompact)	\
    (This)->lpVtbl -> UpdateText(This,sptext,pszText,fMayCompact)

#define IActionProgress_QueryCancel(This,pfCancelled)	\
    (This)->lpVtbl -> QueryCancel(This,pfCancelled)

#define IActionProgress_ResetCancel(This)	\
    (This)->lpVtbl -> ResetCancel(This)

#define IActionProgress_End(This)	\
    (This)->lpVtbl -> End(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IActionProgress_Begin_Proxy( 
    IActionProgress * This,
    /* [in] */ SPACTION action,
    /* [in] */ SPBEGINF flags);


void __RPC_STUB IActionProgress_Begin_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IActionProgress_UpdateProgress_Proxy( 
    IActionProgress * This,
    /* [in] */ ULONGLONG ulCompleted,
    /* [in] */ ULONGLONG ulTotal);


void __RPC_STUB IActionProgress_UpdateProgress_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IActionProgress_UpdateText_Proxy( 
    IActionProgress * This,
    /* [in] */ SPTEXT sptext,
    /* [string][in] */ LPCWSTR pszText,
    /* [in] */ BOOL fMayCompact);


void __RPC_STUB IActionProgress_UpdateText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IActionProgress_QueryCancel_Proxy( 
    IActionProgress * This,
    /* [out] */ BOOL *pfCancelled);


void __RPC_STUB IActionProgress_QueryCancel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IActionProgress_ResetCancel_Proxy( 
    IActionProgress * This);


void __RPC_STUB IActionProgress_ResetCancel_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IActionProgress_End_Proxy( 
    IActionProgress * This);


void __RPC_STUB IActionProgress_End_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IActionProgress_INTERFACE_DEFINED__ */


#ifndef __IShellExtInit_INTERFACE_DEFINED__
#define __IShellExtInit_INTERFACE_DEFINED__

/* interface IShellExtInit */
/* [unique][local][object][uuid] */ 


EXTERN_C const IID IID_IShellExtInit;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("000214E8-0000-0000-C000-000000000046")
    IShellExtInit : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Initialize( 
            /* [in] */ LPCITEMIDLIST pidlFolder,
            /* [in] */ IDataObject *pdtobj,
            /* [in] */ HKEY hkeyProgID) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IShellExtInitVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IShellExtInit * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IShellExtInit * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IShellExtInit * This);
        
        HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            IShellExtInit * This,
            /* [in] */ LPCITEMIDLIST pidlFolder,
            /* [in] */ IDataObject *pdtobj,
            /* [in] */ HKEY hkeyProgID);
        
        END_INTERFACE
    } IShellExtInitVtbl;

    interface IShellExtInit
    {
        CONST_VTBL struct IShellExtInitVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IShellExtInit_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IShellExtInit_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IShellExtInit_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IShellExtInit_Initialize(This,pidlFolder,pdtobj,hkeyProgID)	\
    (This)->lpVtbl -> Initialize(This,pidlFolder,pdtobj,hkeyProgID)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IShellExtInit_Initialize_Proxy( 
    IShellExtInit * This,
    /* [in] */ LPCITEMIDLIST pidlFolder,
    /* [in] */ IDataObject *pdtobj,
    /* [in] */ HKEY hkeyProgID);


void __RPC_STUB IShellExtInit_Initialize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IShellExtInit_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0219 */
/* [local] */ 

typedef IShellExtInit *LPSHELLEXTINIT;



extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0219_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0219_v0_0_s_ifspec;

#ifndef __IShellPropSheetExt_INTERFACE_DEFINED__
#define __IShellPropSheetExt_INTERFACE_DEFINED__

/* interface IShellPropSheetExt */
/* [unique][local][object][uuid] */ 


enum __MIDL_IShellPropSheetExt_0001
    {	EXPPS_FILETYPES	= 0x1
    } ;
typedef UINT EXPPS;


EXTERN_C const IID IID_IShellPropSheetExt;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("000214E9-0000-0000-C000-000000000046")
    IShellPropSheetExt : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE AddPages( 
            /* [in] */ LPFNSVADDPROPSHEETPAGE pfnAddPage,
            /* [in] */ LPARAM lParam) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ReplacePage( 
            /* [in] */ EXPPS uPageID,
            /* [in] */ LPFNSVADDPROPSHEETPAGE pfnReplaceWith,
            /* [in] */ LPARAM lParam) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IShellPropSheetExtVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IShellPropSheetExt * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IShellPropSheetExt * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IShellPropSheetExt * This);
        
        HRESULT ( STDMETHODCALLTYPE *AddPages )( 
            IShellPropSheetExt * This,
            /* [in] */ LPFNSVADDPROPSHEETPAGE pfnAddPage,
            /* [in] */ LPARAM lParam);
        
        HRESULT ( STDMETHODCALLTYPE *ReplacePage )( 
            IShellPropSheetExt * This,
            /* [in] */ EXPPS uPageID,
            /* [in] */ LPFNSVADDPROPSHEETPAGE pfnReplaceWith,
            /* [in] */ LPARAM lParam);
        
        END_INTERFACE
    } IShellPropSheetExtVtbl;

    interface IShellPropSheetExt
    {
        CONST_VTBL struct IShellPropSheetExtVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IShellPropSheetExt_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IShellPropSheetExt_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IShellPropSheetExt_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IShellPropSheetExt_AddPages(This,pfnAddPage,lParam)	\
    (This)->lpVtbl -> AddPages(This,pfnAddPage,lParam)

#define IShellPropSheetExt_ReplacePage(This,uPageID,pfnReplaceWith,lParam)	\
    (This)->lpVtbl -> ReplacePage(This,uPageID,pfnReplaceWith,lParam)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IShellPropSheetExt_AddPages_Proxy( 
    IShellPropSheetExt * This,
    /* [in] */ LPFNSVADDPROPSHEETPAGE pfnAddPage,
    /* [in] */ LPARAM lParam);


void __RPC_STUB IShellPropSheetExt_AddPages_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellPropSheetExt_ReplacePage_Proxy( 
    IShellPropSheetExt * This,
    /* [in] */ EXPPS uPageID,
    /* [in] */ LPFNSVADDPROPSHEETPAGE pfnReplaceWith,
    /* [in] */ LPARAM lParam);


void __RPC_STUB IShellPropSheetExt_ReplacePage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IShellPropSheetExt_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0220 */
/* [local] */ 

typedef IShellPropSheetExt *LPSHELLPROPSHEETEXT;



extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0220_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0220_v0_0_s_ifspec;

#ifndef __IRemoteComputer_INTERFACE_DEFINED__
#define __IRemoteComputer_INTERFACE_DEFINED__

/* interface IRemoteComputer */
/* [unique][object][uuid][helpstring] */ 


EXTERN_C const IID IID_IRemoteComputer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("000214FE-0000-0000-C000-000000000046")
    IRemoteComputer : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Initialize( 
            /* [in] */ LPCWSTR pszMachine,
            /* [in] */ BOOL bEnumerating) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IRemoteComputerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IRemoteComputer * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IRemoteComputer * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IRemoteComputer * This);
        
        HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            IRemoteComputer * This,
            /* [in] */ LPCWSTR pszMachine,
            /* [in] */ BOOL bEnumerating);
        
        END_INTERFACE
    } IRemoteComputerVtbl;

    interface IRemoteComputer
    {
        CONST_VTBL struct IRemoteComputerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IRemoteComputer_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IRemoteComputer_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IRemoteComputer_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IRemoteComputer_Initialize(This,pszMachine,bEnumerating)	\
    (This)->lpVtbl -> Initialize(This,pszMachine,bEnumerating)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IRemoteComputer_Initialize_Proxy( 
    IRemoteComputer * This,
    /* [in] */ LPCWSTR pszMachine,
    /* [in] */ BOOL bEnumerating);


void __RPC_STUB IRemoteComputer_Initialize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IRemoteComputer_INTERFACE_DEFINED__ */


#ifndef __IQueryContinue_INTERFACE_DEFINED__
#define __IQueryContinue_INTERFACE_DEFINED__

/* interface IQueryContinue */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IQueryContinue;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("7307055c-b24a-486b-9f25-163e597a28a9")
    IQueryContinue : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE QueryContinue( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IQueryContinueVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IQueryContinue * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IQueryContinue * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IQueryContinue * This);
        
        HRESULT ( STDMETHODCALLTYPE *QueryContinue )( 
            IQueryContinue * This);
        
        END_INTERFACE
    } IQueryContinueVtbl;

    interface IQueryContinue
    {
        CONST_VTBL struct IQueryContinueVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IQueryContinue_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IQueryContinue_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IQueryContinue_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IQueryContinue_QueryContinue(This)	\
    (This)->lpVtbl -> QueryContinue(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IQueryContinue_QueryContinue_Proxy( 
    IQueryContinue * This);


void __RPC_STUB IQueryContinue_QueryContinue_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IQueryContinue_INTERFACE_DEFINED__ */


#ifndef __IUserNotification_INTERFACE_DEFINED__
#define __IUserNotification_INTERFACE_DEFINED__

/* interface IUserNotification */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IUserNotification;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("ba9711ba-5893-4787-a7e1-41277151550b")
    IUserNotification : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetBalloonInfo( 
            /* [string][in] */ LPCWSTR pszTitle,
            /* [string][in] */ LPCWSTR pszText,
            /* [in] */ DWORD dwInfoFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetBalloonRetry( 
            /* [in] */ DWORD dwShowTime,
            /* [in] */ DWORD dwInterval,
            /* [in] */ UINT cRetryCount) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetIconInfo( 
            /* [in] */ HICON hIcon,
            /* [string][in] */ LPCWSTR pszToolTip) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Show( 
            /* [in] */ IQueryContinue *pqc,
            /* [in] */ DWORD dwContinuePollInterval) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE PlaySound( 
            /* [string][in] */ LPCWSTR pszSoundName) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUserNotificationVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUserNotification * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUserNotification * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUserNotification * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetBalloonInfo )( 
            IUserNotification * This,
            /* [string][in] */ LPCWSTR pszTitle,
            /* [string][in] */ LPCWSTR pszText,
            /* [in] */ DWORD dwInfoFlags);
        
        HRESULT ( STDMETHODCALLTYPE *SetBalloonRetry )( 
            IUserNotification * This,
            /* [in] */ DWORD dwShowTime,
            /* [in] */ DWORD dwInterval,
            /* [in] */ UINT cRetryCount);
        
        HRESULT ( STDMETHODCALLTYPE *SetIconInfo )( 
            IUserNotification * This,
            /* [in] */ HICON hIcon,
            /* [string][in] */ LPCWSTR pszToolTip);
        
        HRESULT ( STDMETHODCALLTYPE *Show )( 
            IUserNotification * This,
            /* [in] */ IQueryContinue *pqc,
            /* [in] */ DWORD dwContinuePollInterval);
        
        HRESULT ( STDMETHODCALLTYPE *PlaySound )( 
            IUserNotification * This,
            /* [string][in] */ LPCWSTR pszSoundName);
        
        END_INTERFACE
    } IUserNotificationVtbl;

    interface IUserNotification
    {
        CONST_VTBL struct IUserNotificationVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUserNotification_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IUserNotification_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IUserNotification_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IUserNotification_SetBalloonInfo(This,pszTitle,pszText,dwInfoFlags)	\
    (This)->lpVtbl -> SetBalloonInfo(This,pszTitle,pszText,dwInfoFlags)

#define IUserNotification_SetBalloonRetry(This,dwShowTime,dwInterval,cRetryCount)	\
    (This)->lpVtbl -> SetBalloonRetry(This,dwShowTime,dwInterval,cRetryCount)

#define IUserNotification_SetIconInfo(This,hIcon,pszToolTip)	\
    (This)->lpVtbl -> SetIconInfo(This,hIcon,pszToolTip)

#define IUserNotification_Show(This,pqc,dwContinuePollInterval)	\
    (This)->lpVtbl -> Show(This,pqc,dwContinuePollInterval)

#define IUserNotification_PlaySound(This,pszSoundName)	\
    (This)->lpVtbl -> PlaySound(This,pszSoundName)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IUserNotification_SetBalloonInfo_Proxy( 
    IUserNotification * This,
    /* [string][in] */ LPCWSTR pszTitle,
    /* [string][in] */ LPCWSTR pszText,
    /* [in] */ DWORD dwInfoFlags);


void __RPC_STUB IUserNotification_SetBalloonInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUserNotification_SetBalloonRetry_Proxy( 
    IUserNotification * This,
    /* [in] */ DWORD dwShowTime,
    /* [in] */ DWORD dwInterval,
    /* [in] */ UINT cRetryCount);


void __RPC_STUB IUserNotification_SetBalloonRetry_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUserNotification_SetIconInfo_Proxy( 
    IUserNotification * This,
    /* [in] */ HICON hIcon,
    /* [string][in] */ LPCWSTR pszToolTip);


void __RPC_STUB IUserNotification_SetIconInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUserNotification_Show_Proxy( 
    IUserNotification * This,
    /* [in] */ IQueryContinue *pqc,
    /* [in] */ DWORD dwContinuePollInterval);


void __RPC_STUB IUserNotification_Show_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUserNotification_PlaySound_Proxy( 
    IUserNotification * This,
    /* [string][in] */ LPCWSTR pszSoundName);


void __RPC_STUB IUserNotification_PlaySound_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IUserNotification_INTERFACE_DEFINED__ */


#ifndef __IItemNameLimits_INTERFACE_DEFINED__
#define __IItemNameLimits_INTERFACE_DEFINED__

/* interface IItemNameLimits */
/* [object][uuid] */ 


EXTERN_C const IID IID_IItemNameLimits;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1df0d7f1-b267-4d28-8b10-12e23202a5c4")
    IItemNameLimits : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetValidCharacters( 
            /* [out] */ LPWSTR *ppwszValidChars,
            /* [out] */ LPWSTR *ppwszInvalidChars) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMaxLength( 
            /* [in] */ LPCWSTR pszName,
            /* [out] */ int *piMaxNameLen) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IItemNameLimitsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IItemNameLimits * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IItemNameLimits * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IItemNameLimits * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetValidCharacters )( 
            IItemNameLimits * This,
            /* [out] */ LPWSTR *ppwszValidChars,
            /* [out] */ LPWSTR *ppwszInvalidChars);
        
        HRESULT ( STDMETHODCALLTYPE *GetMaxLength )( 
            IItemNameLimits * This,
            /* [in] */ LPCWSTR pszName,
            /* [out] */ int *piMaxNameLen);
        
        END_INTERFACE
    } IItemNameLimitsVtbl;

    interface IItemNameLimits
    {
        CONST_VTBL struct IItemNameLimitsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IItemNameLimits_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IItemNameLimits_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IItemNameLimits_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IItemNameLimits_GetValidCharacters(This,ppwszValidChars,ppwszInvalidChars)	\
    (This)->lpVtbl -> GetValidCharacters(This,ppwszValidChars,ppwszInvalidChars)

#define IItemNameLimits_GetMaxLength(This,pszName,piMaxNameLen)	\
    (This)->lpVtbl -> GetMaxLength(This,pszName,piMaxNameLen)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IItemNameLimits_GetValidCharacters_Proxy( 
    IItemNameLimits * This,
    /* [out] */ LPWSTR *ppwszValidChars,
    /* [out] */ LPWSTR *ppwszInvalidChars);


void __RPC_STUB IItemNameLimits_GetValidCharacters_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IItemNameLimits_GetMaxLength_Proxy( 
    IItemNameLimits * This,
    /* [in] */ LPCWSTR pszName,
    /* [out] */ int *piMaxNameLen);


void __RPC_STUB IItemNameLimits_GetMaxLength_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IItemNameLimits_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0224 */
/* [local] */ 

#define SNCF_REFRESHLIST 0x00000001  // refresh the list (eg. from F5 or opening a folder)


extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0224_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0224_v0_0_s_ifspec;

#ifndef __INetCrawler_INTERFACE_DEFINED__
#define __INetCrawler_INTERFACE_DEFINED__

/* interface INetCrawler */
/* [unique][object][uuid][helpstring] */ 


EXTERN_C const IID IID_INetCrawler;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("49c929ee-a1b7-4c58-b539-e63be392b6f3")
    INetCrawler : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Update( 
            /* [in] */ DWORD dwFlags) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INetCrawlerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INetCrawler * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INetCrawler * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INetCrawler * This);
        
        HRESULT ( STDMETHODCALLTYPE *Update )( 
            INetCrawler * This,
            /* [in] */ DWORD dwFlags);
        
        END_INTERFACE
    } INetCrawlerVtbl;

    interface INetCrawler
    {
        CONST_VTBL struct INetCrawlerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INetCrawler_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INetCrawler_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INetCrawler_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INetCrawler_Update(This,dwFlags)	\
    (This)->lpVtbl -> Update(This,dwFlags)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE INetCrawler_Update_Proxy( 
    INetCrawler * This,
    /* [in] */ DWORD dwFlags);


void __RPC_STUB INetCrawler_Update_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INetCrawler_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0225 */
/* [local] */ 

#if (_WIN32_IE >= 0x0400)
#define IEI_PRIORITY_MAX        ITSAT_MAX_PRIORITY
#define IEI_PRIORITY_MIN        ITSAT_MIN_PRIORITY
#define IEIT_PRIORITY_NORMAL     ITSAT_DEFAULT_PRIORITY
#define IEIFLAG_ASYNC       0x0001      // ask the extractor if it supports ASYNC extract (free threaded)
#define IEIFLAG_CACHE       0x0002      // returned from the extractor if it does NOT cache the thumbnail
#define IEIFLAG_ASPECT      0x0004      // passed to the extractor to beg it to render to the aspect ratio of the supplied rect
#define IEIFLAG_OFFLINE     0x0008      // if the extractor shouldn't hit the net to get any content neede for the rendering
#define IEIFLAG_GLEAM       0x0010      // does the image have a gleam ? this will be returned if it does
#define IEIFLAG_SCREEN      0x0020      // render as if for the screen  (this is exlusive with IEIFLAG_ASPECT )
#define IEIFLAG_ORIGSIZE    0x0040      // render to the approx size passed, but crop if neccessary
#define IEIFLAG_NOSTAMP     0x0080      // returned from the extractor if it does NOT want an icon stamp on the thumbnail
#define IEIFLAG_NOBORDER    0x0100      // returned from the extractor if it does NOT want an a border around the thumbnail
#define IEIFLAG_QUALITY     0x0200      // passed to the Extract method to indicate that a slower, higher quality image is desired, re-compute the thumbnail
#define IEIFLAG_REFRESH     0x0400      // returned from the extractor if it would like to have Refresh Thumbnail available


extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0225_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0225_v0_0_s_ifspec;

#ifndef __IExtractImage_INTERFACE_DEFINED__
#define __IExtractImage_INTERFACE_DEFINED__

/* interface IExtractImage */
/* [unique][object][uuid][helpstring] */ 


EXTERN_C const IID IID_IExtractImage;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("BB2E617C-0920-11d1-9A0B-00C04FC2D6C1")
    IExtractImage : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetLocation( 
            /* [size_is][out] */ LPWSTR pszPathBuffer,
            /* [in] */ DWORD cch,
            /* [unique][out][in] */ DWORD *pdwPriority,
            /* [in] */ const SIZE *prgSize,
            /* [in] */ DWORD dwRecClrDepth,
            /* [out][in] */ DWORD *pdwFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Extract( 
            /* [out] */ HBITMAP *phBmpThumbnail) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IExtractImageVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IExtractImage * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IExtractImage * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IExtractImage * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetLocation )( 
            IExtractImage * This,
            /* [size_is][out] */ LPWSTR pszPathBuffer,
            /* [in] */ DWORD cch,
            /* [unique][out][in] */ DWORD *pdwPriority,
            /* [in] */ const SIZE *prgSize,
            /* [in] */ DWORD dwRecClrDepth,
            /* [out][in] */ DWORD *pdwFlags);
        
        HRESULT ( STDMETHODCALLTYPE *Extract )( 
            IExtractImage * This,
            /* [out] */ HBITMAP *phBmpThumbnail);
        
        END_INTERFACE
    } IExtractImageVtbl;

    interface IExtractImage
    {
        CONST_VTBL struct IExtractImageVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IExtractImage_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IExtractImage_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IExtractImage_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IExtractImage_GetLocation(This,pszPathBuffer,cch,pdwPriority,prgSize,dwRecClrDepth,pdwFlags)	\
    (This)->lpVtbl -> GetLocation(This,pszPathBuffer,cch,pdwPriority,prgSize,dwRecClrDepth,pdwFlags)

#define IExtractImage_Extract(This,phBmpThumbnail)	\
    (This)->lpVtbl -> Extract(This,phBmpThumbnail)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IExtractImage_GetLocation_Proxy( 
    IExtractImage * This,
    /* [size_is][out] */ LPWSTR pszPathBuffer,
    /* [in] */ DWORD cch,
    /* [unique][out][in] */ DWORD *pdwPriority,
    /* [in] */ const SIZE *prgSize,
    /* [in] */ DWORD dwRecClrDepth,
    /* [out][in] */ DWORD *pdwFlags);


void __RPC_STUB IExtractImage_GetLocation_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IExtractImage_Extract_Proxy( 
    IExtractImage * This,
    /* [out] */ HBITMAP *phBmpThumbnail);


void __RPC_STUB IExtractImage_Extract_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IExtractImage_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0226 */
/* [local] */ 

typedef IExtractImage *LPEXTRACTIMAGE;

#endif
#if (_WIN32_IE >= 0x0500)


extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0226_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0226_v0_0_s_ifspec;

#ifndef __IExtractImage2_INTERFACE_DEFINED__
#define __IExtractImage2_INTERFACE_DEFINED__

/* interface IExtractImage2 */
/* [unique][object][uuid][helpstring] */ 


EXTERN_C const IID IID_IExtractImage2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("953BB1EE-93B4-11d1-98A3-00C04FB687DA")
    IExtractImage2 : public IExtractImage
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetDateStamp( 
            /* [out] */ FILETIME *pDateStamp) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IExtractImage2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IExtractImage2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IExtractImage2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IExtractImage2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetLocation )( 
            IExtractImage2 * This,
            /* [size_is][out] */ LPWSTR pszPathBuffer,
            /* [in] */ DWORD cch,
            /* [unique][out][in] */ DWORD *pdwPriority,
            /* [in] */ const SIZE *prgSize,
            /* [in] */ DWORD dwRecClrDepth,
            /* [out][in] */ DWORD *pdwFlags);
        
        HRESULT ( STDMETHODCALLTYPE *Extract )( 
            IExtractImage2 * This,
            /* [out] */ HBITMAP *phBmpThumbnail);
        
        HRESULT ( STDMETHODCALLTYPE *GetDateStamp )( 
            IExtractImage2 * This,
            /* [out] */ FILETIME *pDateStamp);
        
        END_INTERFACE
    } IExtractImage2Vtbl;

    interface IExtractImage2
    {
        CONST_VTBL struct IExtractImage2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IExtractImage2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IExtractImage2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IExtractImage2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IExtractImage2_GetLocation(This,pszPathBuffer,cch,pdwPriority,prgSize,dwRecClrDepth,pdwFlags)	\
    (This)->lpVtbl -> GetLocation(This,pszPathBuffer,cch,pdwPriority,prgSize,dwRecClrDepth,pdwFlags)

#define IExtractImage2_Extract(This,phBmpThumbnail)	\
    (This)->lpVtbl -> Extract(This,phBmpThumbnail)


#define IExtractImage2_GetDateStamp(This,pDateStamp)	\
    (This)->lpVtbl -> GetDateStamp(This,pDateStamp)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IExtractImage2_GetDateStamp_Proxy( 
    IExtractImage2 * This,
    /* [out] */ FILETIME *pDateStamp);


void __RPC_STUB IExtractImage2_GetDateStamp_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IExtractImage2_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0227 */
/* [local] */ 

typedef IExtractImage2 *LPEXTRACTIMAGE2;

#endif



extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0227_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0227_v0_0_s_ifspec;

#ifndef __IUserEventTimerCallback_INTERFACE_DEFINED__
#define __IUserEventTimerCallback_INTERFACE_DEFINED__

/* interface IUserEventTimerCallback */
/* [unique][object][uuid][helpstring] */ 


EXTERN_C const IID IID_IUserEventTimerCallback;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("e9ead8e6-2a25-410e-9b58-a9fbef1dd1a2")
    IUserEventTimerCallback : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE UserEventTimerProc( 
            /* [in] */ ULONG uUserEventTimerID,
            /* [in] */ UINT uTimerElapse) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUserEventTimerCallbackVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUserEventTimerCallback * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUserEventTimerCallback * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUserEventTimerCallback * This);
        
        HRESULT ( STDMETHODCALLTYPE *UserEventTimerProc )( 
            IUserEventTimerCallback * This,
            /* [in] */ ULONG uUserEventTimerID,
            /* [in] */ UINT uTimerElapse);
        
        END_INTERFACE
    } IUserEventTimerCallbackVtbl;

    interface IUserEventTimerCallback
    {
        CONST_VTBL struct IUserEventTimerCallbackVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUserEventTimerCallback_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IUserEventTimerCallback_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IUserEventTimerCallback_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IUserEventTimerCallback_UserEventTimerProc(This,uUserEventTimerID,uTimerElapse)	\
    (This)->lpVtbl -> UserEventTimerProc(This,uUserEventTimerID,uTimerElapse)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IUserEventTimerCallback_UserEventTimerProc_Proxy( 
    IUserEventTimerCallback * This,
    /* [in] */ ULONG uUserEventTimerID,
    /* [in] */ UINT uTimerElapse);


void __RPC_STUB IUserEventTimerCallback_UserEventTimerProc_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IUserEventTimerCallback_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0228 */
/* [local] */ 




extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0228_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0228_v0_0_s_ifspec;

#ifndef __IUserEventTimer_INTERFACE_DEFINED__
#define __IUserEventTimer_INTERFACE_DEFINED__

/* interface IUserEventTimer */
/* [unique][object][uuid][helpstring] */ 


EXTERN_C const IID IID_IUserEventTimer;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0F504B94-6E42-42E6-99E0-E20FAFE52AB4")
    IUserEventTimer : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetUserEventTimer( 
            /* [in] */ HWND hWnd,
            /* [in] */ UINT uCallbackMessage,
            /* [in] */ UINT uTimerElapse,
            /* [in] */ IUserEventTimerCallback *pUserEventTimerCallback,
            /* [out][in] */ ULONG *puUserEventTimerID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE KillUserEventTimer( 
            /* [in] */ HWND hWnd,
            /* [in] */ ULONG uUserEventTimerID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetUserEventTimerElapsed( 
            /* [in] */ HWND hWnd,
            /* [in] */ ULONG uUserEventTimerID,
            /* [out] */ UINT *puTimerElapsed) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE InitTimerTickInterval( 
            /* [in] */ UINT uTimerTickIntervalMs) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IUserEventTimerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IUserEventTimer * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IUserEventTimer * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IUserEventTimer * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetUserEventTimer )( 
            IUserEventTimer * This,
            /* [in] */ HWND hWnd,
            /* [in] */ UINT uCallbackMessage,
            /* [in] */ UINT uTimerElapse,
            /* [in] */ IUserEventTimerCallback *pUserEventTimerCallback,
            /* [out][in] */ ULONG *puUserEventTimerID);
        
        HRESULT ( STDMETHODCALLTYPE *KillUserEventTimer )( 
            IUserEventTimer * This,
            /* [in] */ HWND hWnd,
            /* [in] */ ULONG uUserEventTimerID);
        
        HRESULT ( STDMETHODCALLTYPE *GetUserEventTimerElapsed )( 
            IUserEventTimer * This,
            /* [in] */ HWND hWnd,
            /* [in] */ ULONG uUserEventTimerID,
            /* [out] */ UINT *puTimerElapsed);
        
        HRESULT ( STDMETHODCALLTYPE *InitTimerTickInterval )( 
            IUserEventTimer * This,
            /* [in] */ UINT uTimerTickIntervalMs);
        
        END_INTERFACE
    } IUserEventTimerVtbl;

    interface IUserEventTimer
    {
        CONST_VTBL struct IUserEventTimerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IUserEventTimer_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IUserEventTimer_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IUserEventTimer_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IUserEventTimer_SetUserEventTimer(This,hWnd,uCallbackMessage,uTimerElapse,pUserEventTimerCallback,puUserEventTimerID)	\
    (This)->lpVtbl -> SetUserEventTimer(This,hWnd,uCallbackMessage,uTimerElapse,pUserEventTimerCallback,puUserEventTimerID)

#define IUserEventTimer_KillUserEventTimer(This,hWnd,uUserEventTimerID)	\
    (This)->lpVtbl -> KillUserEventTimer(This,hWnd,uUserEventTimerID)

#define IUserEventTimer_GetUserEventTimerElapsed(This,hWnd,uUserEventTimerID,puTimerElapsed)	\
    (This)->lpVtbl -> GetUserEventTimerElapsed(This,hWnd,uUserEventTimerID,puTimerElapsed)

#define IUserEventTimer_InitTimerTickInterval(This,uTimerTickIntervalMs)	\
    (This)->lpVtbl -> InitTimerTickInterval(This,uTimerTickIntervalMs)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IUserEventTimer_SetUserEventTimer_Proxy( 
    IUserEventTimer * This,
    /* [in] */ HWND hWnd,
    /* [in] */ UINT uCallbackMessage,
    /* [in] */ UINT uTimerElapse,
    /* [in] */ IUserEventTimerCallback *pUserEventTimerCallback,
    /* [out][in] */ ULONG *puUserEventTimerID);


void __RPC_STUB IUserEventTimer_SetUserEventTimer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUserEventTimer_KillUserEventTimer_Proxy( 
    IUserEventTimer * This,
    /* [in] */ HWND hWnd,
    /* [in] */ ULONG uUserEventTimerID);


void __RPC_STUB IUserEventTimer_KillUserEventTimer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUserEventTimer_GetUserEventTimerElapsed_Proxy( 
    IUserEventTimer * This,
    /* [in] */ HWND hWnd,
    /* [in] */ ULONG uUserEventTimerID,
    /* [out] */ UINT *puTimerElapsed);


void __RPC_STUB IUserEventTimer_GetUserEventTimerElapsed_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IUserEventTimer_InitTimerTickInterval_Proxy( 
    IUserEventTimer * This,
    /* [in] */ UINT uTimerTickIntervalMs);


void __RPC_STUB IUserEventTimer_InitTimerTickInterval_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IUserEventTimer_INTERFACE_DEFINED__ */


#ifndef __IDockingWindow_INTERFACE_DEFINED__
#define __IDockingWindow_INTERFACE_DEFINED__

/* interface IDockingWindow */
/* [object][uuid] */ 


EXTERN_C const IID IID_IDockingWindow;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("012dd920-7b26-11d0-8ca9-00a0c92dbfe8")
    IDockingWindow : public IOleWindow
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE ShowDW( 
            /* [in] */ BOOL fShow) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CloseDW( 
            /* [in] */ DWORD dwReserved) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ResizeBorderDW( 
            /* [in] */ LPCRECT prcBorder,
            /* [in] */ IUnknown *punkToolbarSite,
            /* [in] */ BOOL fReserved) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDockingWindowVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDockingWindow * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDockingWindow * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDockingWindow * This);
        
        /* [input_sync] */ HRESULT ( STDMETHODCALLTYPE *GetWindow )( 
            IDockingWindow * This,
            /* [out] */ HWND *phwnd);
        
        HRESULT ( STDMETHODCALLTYPE *ContextSensitiveHelp )( 
            IDockingWindow * This,
            /* [in] */ BOOL fEnterMode);
        
        HRESULT ( STDMETHODCALLTYPE *ShowDW )( 
            IDockingWindow * This,
            /* [in] */ BOOL fShow);
        
        HRESULT ( STDMETHODCALLTYPE *CloseDW )( 
            IDockingWindow * This,
            /* [in] */ DWORD dwReserved);
        
        HRESULT ( STDMETHODCALLTYPE *ResizeBorderDW )( 
            IDockingWindow * This,
            /* [in] */ LPCRECT prcBorder,
            /* [in] */ IUnknown *punkToolbarSite,
            /* [in] */ BOOL fReserved);
        
        END_INTERFACE
    } IDockingWindowVtbl;

    interface IDockingWindow
    {
        CONST_VTBL struct IDockingWindowVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDockingWindow_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDockingWindow_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDockingWindow_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDockingWindow_GetWindow(This,phwnd)	\
    (This)->lpVtbl -> GetWindow(This,phwnd)

#define IDockingWindow_ContextSensitiveHelp(This,fEnterMode)	\
    (This)->lpVtbl -> ContextSensitiveHelp(This,fEnterMode)


#define IDockingWindow_ShowDW(This,fShow)	\
    (This)->lpVtbl -> ShowDW(This,fShow)

#define IDockingWindow_CloseDW(This,dwReserved)	\
    (This)->lpVtbl -> CloseDW(This,dwReserved)

#define IDockingWindow_ResizeBorderDW(This,prcBorder,punkToolbarSite,fReserved)	\
    (This)->lpVtbl -> ResizeBorderDW(This,prcBorder,punkToolbarSite,fReserved)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDockingWindow_ShowDW_Proxy( 
    IDockingWindow * This,
    /* [in] */ BOOL fShow);


void __RPC_STUB IDockingWindow_ShowDW_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDockingWindow_CloseDW_Proxy( 
    IDockingWindow * This,
    /* [in] */ DWORD dwReserved);


void __RPC_STUB IDockingWindow_CloseDW_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDockingWindow_ResizeBorderDW_Proxy( 
    IDockingWindow * This,
    /* [in] */ LPCRECT prcBorder,
    /* [in] */ IUnknown *punkToolbarSite,
    /* [in] */ BOOL fReserved);


void __RPC_STUB IDockingWindow_ResizeBorderDW_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDockingWindow_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0230 */
/* [local] */ 

#define DBIM_MINSIZE    0x0001
#define DBIM_MAXSIZE    0x0002
#define DBIM_INTEGRAL   0x0004
#define DBIM_ACTUAL     0x0008
#define DBIM_TITLE      0x0010
#define DBIM_MODEFLAGS  0x0020
#define DBIM_BKCOLOR    0x0040
#include <pshpack8.h>
typedef /* [public][public] */ struct __MIDL___MIDL_itf_shobjidl_0230_0001
    {
    DWORD dwMask;
    POINTL ptMinSize;
    POINTL ptMaxSize;
    POINTL ptIntegral;
    POINTL ptActual;
    WCHAR wszTitle[ 256 ];
    DWORD dwModeFlags;
    COLORREF crBkgnd;
    } 	DESKBANDINFO;

#include <poppack.h>
#define DBIMF_NORMAL            0x0000
#define DBIMF_FIXED             0x0001
#define DBIMF_FIXEDBMP          0x0004   // a fixed background bitmap (if supported)
#define DBIMF_VARIABLEHEIGHT    0x0008
#define DBIMF_UNDELETEABLE      0x0010
#define DBIMF_DEBOSSED          0x0020
#define DBIMF_BKCOLOR           0x0040
#define DBIMF_USECHEVRON        0x0080
#define DBIMF_BREAK             0x0100
#define DBIMF_ADDTOFRONT        0x0200
#define DBIMF_TOPALIGN          0x0400
#define DBIF_VIEWMODE_NORMAL         0x0000
#define DBIF_VIEWMODE_VERTICAL       0x0001
#define DBIF_VIEWMODE_FLOATING       0x0002
#define DBIF_VIEWMODE_TRANSPARENT    0x0004

enum __MIDL___MIDL_itf_shobjidl_0230_0002
    {	DBID_BANDINFOCHANGED	= 0,
	DBID_SHOWONLY	= 1,
	DBID_MAXIMIZEBAND	= 2,
	DBID_PUSHCHEVRON	= 3,
	DBID_DELAYINIT	= 4,
	DBID_FINISHINIT	= 5,
	DBID_SETWINDOWTHEME	= 6,
	DBID_PERMITAUTOHIDE	= 7
    } ;
#define DBPC_SELECTFIRST    (DWORD)-1
#define DBPC_SELECTLAST     (DWORD)-2
#define CGID_DeskBand IID_IDeskBand


extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0230_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0230_v0_0_s_ifspec;

#ifndef __IDeskBand_INTERFACE_DEFINED__
#define __IDeskBand_INTERFACE_DEFINED__

/* interface IDeskBand */
/* [object][uuid] */ 


EXTERN_C const IID IID_IDeskBand;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("EB0FE172-1A3A-11D0-89B3-00A0C90A90AC")
    IDeskBand : public IDockingWindow
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetBandInfo( 
            /* [in] */ DWORD dwBandID,
            /* [in] */ DWORD dwViewMode,
            /* [out][in] */ DESKBANDINFO *pdbi) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDeskBandVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDeskBand * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDeskBand * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDeskBand * This);
        
        /* [input_sync] */ HRESULT ( STDMETHODCALLTYPE *GetWindow )( 
            IDeskBand * This,
            /* [out] */ HWND *phwnd);
        
        HRESULT ( STDMETHODCALLTYPE *ContextSensitiveHelp )( 
            IDeskBand * This,
            /* [in] */ BOOL fEnterMode);
        
        HRESULT ( STDMETHODCALLTYPE *ShowDW )( 
            IDeskBand * This,
            /* [in] */ BOOL fShow);
        
        HRESULT ( STDMETHODCALLTYPE *CloseDW )( 
            IDeskBand * This,
            /* [in] */ DWORD dwReserved);
        
        HRESULT ( STDMETHODCALLTYPE *ResizeBorderDW )( 
            IDeskBand * This,
            /* [in] */ LPCRECT prcBorder,
            /* [in] */ IUnknown *punkToolbarSite,
            /* [in] */ BOOL fReserved);
        
        HRESULT ( STDMETHODCALLTYPE *GetBandInfo )( 
            IDeskBand * This,
            /* [in] */ DWORD dwBandID,
            /* [in] */ DWORD dwViewMode,
            /* [out][in] */ DESKBANDINFO *pdbi);
        
        END_INTERFACE
    } IDeskBandVtbl;

    interface IDeskBand
    {
        CONST_VTBL struct IDeskBandVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDeskBand_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDeskBand_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDeskBand_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDeskBand_GetWindow(This,phwnd)	\
    (This)->lpVtbl -> GetWindow(This,phwnd)

#define IDeskBand_ContextSensitiveHelp(This,fEnterMode)	\
    (This)->lpVtbl -> ContextSensitiveHelp(This,fEnterMode)


#define IDeskBand_ShowDW(This,fShow)	\
    (This)->lpVtbl -> ShowDW(This,fShow)

#define IDeskBand_CloseDW(This,dwReserved)	\
    (This)->lpVtbl -> CloseDW(This,dwReserved)

#define IDeskBand_ResizeBorderDW(This,prcBorder,punkToolbarSite,fReserved)	\
    (This)->lpVtbl -> ResizeBorderDW(This,prcBorder,punkToolbarSite,fReserved)


#define IDeskBand_GetBandInfo(This,dwBandID,dwViewMode,pdbi)	\
    (This)->lpVtbl -> GetBandInfo(This,dwBandID,dwViewMode,pdbi)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDeskBand_GetBandInfo_Proxy( 
    IDeskBand * This,
    /* [in] */ DWORD dwBandID,
    /* [in] */ DWORD dwViewMode,
    /* [out][in] */ DESKBANDINFO *pdbi);


void __RPC_STUB IDeskBand_GetBandInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDeskBand_INTERFACE_DEFINED__ */


#ifndef __ITaskbarList_INTERFACE_DEFINED__
#define __ITaskbarList_INTERFACE_DEFINED__

/* interface ITaskbarList */
/* [object][uuid] */ 


EXTERN_C const IID IID_ITaskbarList;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("56FDF342-FD6D-11d0-958A-006097C9A090")
    ITaskbarList : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE HrInit( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddTab( 
            /* [in] */ HWND hwnd) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DeleteTab( 
            /* [in] */ HWND hwnd) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ActivateTab( 
            /* [in] */ HWND hwnd) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetActiveAlt( 
            /* [in] */ HWND hwnd) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ITaskbarListVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ITaskbarList * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ITaskbarList * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ITaskbarList * This);
        
        HRESULT ( STDMETHODCALLTYPE *HrInit )( 
            ITaskbarList * This);
        
        HRESULT ( STDMETHODCALLTYPE *AddTab )( 
            ITaskbarList * This,
            /* [in] */ HWND hwnd);
        
        HRESULT ( STDMETHODCALLTYPE *DeleteTab )( 
            ITaskbarList * This,
            /* [in] */ HWND hwnd);
        
        HRESULT ( STDMETHODCALLTYPE *ActivateTab )( 
            ITaskbarList * This,
            /* [in] */ HWND hwnd);
        
        HRESULT ( STDMETHODCALLTYPE *SetActiveAlt )( 
            ITaskbarList * This,
            /* [in] */ HWND hwnd);
        
        END_INTERFACE
    } ITaskbarListVtbl;

    interface ITaskbarList
    {
        CONST_VTBL struct ITaskbarListVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITaskbarList_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ITaskbarList_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ITaskbarList_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ITaskbarList_HrInit(This)	\
    (This)->lpVtbl -> HrInit(This)

#define ITaskbarList_AddTab(This,hwnd)	\
    (This)->lpVtbl -> AddTab(This,hwnd)

#define ITaskbarList_DeleteTab(This,hwnd)	\
    (This)->lpVtbl -> DeleteTab(This,hwnd)

#define ITaskbarList_ActivateTab(This,hwnd)	\
    (This)->lpVtbl -> ActivateTab(This,hwnd)

#define ITaskbarList_SetActiveAlt(This,hwnd)	\
    (This)->lpVtbl -> SetActiveAlt(This,hwnd)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ITaskbarList_HrInit_Proxy( 
    ITaskbarList * This);


void __RPC_STUB ITaskbarList_HrInit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ITaskbarList_AddTab_Proxy( 
    ITaskbarList * This,
    /* [in] */ HWND hwnd);


void __RPC_STUB ITaskbarList_AddTab_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ITaskbarList_DeleteTab_Proxy( 
    ITaskbarList * This,
    /* [in] */ HWND hwnd);


void __RPC_STUB ITaskbarList_DeleteTab_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ITaskbarList_ActivateTab_Proxy( 
    ITaskbarList * This,
    /* [in] */ HWND hwnd);


void __RPC_STUB ITaskbarList_ActivateTab_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ITaskbarList_SetActiveAlt_Proxy( 
    ITaskbarList * This,
    /* [in] */ HWND hwnd);


void __RPC_STUB ITaskbarList_SetActiveAlt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ITaskbarList_INTERFACE_DEFINED__ */


#ifndef __ITaskbarList2_INTERFACE_DEFINED__
#define __ITaskbarList2_INTERFACE_DEFINED__

/* interface ITaskbarList2 */
/* [object][uuid] */ 


EXTERN_C const IID IID_ITaskbarList2;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("602D4995-B13A-429b-A66E-1935E44F4317")
    ITaskbarList2 : public ITaskbarList
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE MarkFullscreenWindow( 
            /* [in] */ HWND hwnd,
            /* [in] */ BOOL fFullscreen) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ITaskbarList2Vtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ITaskbarList2 * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ITaskbarList2 * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ITaskbarList2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *HrInit )( 
            ITaskbarList2 * This);
        
        HRESULT ( STDMETHODCALLTYPE *AddTab )( 
            ITaskbarList2 * This,
            /* [in] */ HWND hwnd);
        
        HRESULT ( STDMETHODCALLTYPE *DeleteTab )( 
            ITaskbarList2 * This,
            /* [in] */ HWND hwnd);
        
        HRESULT ( STDMETHODCALLTYPE *ActivateTab )( 
            ITaskbarList2 * This,
            /* [in] */ HWND hwnd);
        
        HRESULT ( STDMETHODCALLTYPE *SetActiveAlt )( 
            ITaskbarList2 * This,
            /* [in] */ HWND hwnd);
        
        HRESULT ( STDMETHODCALLTYPE *MarkFullscreenWindow )( 
            ITaskbarList2 * This,
            /* [in] */ HWND hwnd,
            /* [in] */ BOOL fFullscreen);
        
        END_INTERFACE
    } ITaskbarList2Vtbl;

    interface ITaskbarList2
    {
        CONST_VTBL struct ITaskbarList2Vtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ITaskbarList2_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ITaskbarList2_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ITaskbarList2_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ITaskbarList2_HrInit(This)	\
    (This)->lpVtbl -> HrInit(This)

#define ITaskbarList2_AddTab(This,hwnd)	\
    (This)->lpVtbl -> AddTab(This,hwnd)

#define ITaskbarList2_DeleteTab(This,hwnd)	\
    (This)->lpVtbl -> DeleteTab(This,hwnd)

#define ITaskbarList2_ActivateTab(This,hwnd)	\
    (This)->lpVtbl -> ActivateTab(This,hwnd)

#define ITaskbarList2_SetActiveAlt(This,hwnd)	\
    (This)->lpVtbl -> SetActiveAlt(This,hwnd)


#define ITaskbarList2_MarkFullscreenWindow(This,hwnd,fFullscreen)	\
    (This)->lpVtbl -> MarkFullscreenWindow(This,hwnd,fFullscreen)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ITaskbarList2_MarkFullscreenWindow_Proxy( 
    ITaskbarList2 * This,
    /* [in] */ HWND hwnd,
    /* [in] */ BOOL fFullscreen);


void __RPC_STUB ITaskbarList2_MarkFullscreenWindow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ITaskbarList2_INTERFACE_DEFINED__ */


#ifndef __ICDBurn_INTERFACE_DEFINED__
#define __ICDBurn_INTERFACE_DEFINED__

/* interface ICDBurn */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_ICDBurn;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3d73a659-e5d0-4d42-afc0-5121ba425c8d")
    ICDBurn : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetRecorderDriveLetter( 
            /* [size_is][out] */ LPWSTR pszDrive,
            /* [in] */ UINT cch) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Burn( 
            /* [in] */ HWND hwnd) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE HasRecordableDrive( 
            /* [out] */ BOOL *pfHasRecorder) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICDBurnVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICDBurn * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICDBurn * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICDBurn * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetRecorderDriveLetter )( 
            ICDBurn * This,
            /* [size_is][out] */ LPWSTR pszDrive,
            /* [in] */ UINT cch);
        
        HRESULT ( STDMETHODCALLTYPE *Burn )( 
            ICDBurn * This,
            /* [in] */ HWND hwnd);
        
        HRESULT ( STDMETHODCALLTYPE *HasRecordableDrive )( 
            ICDBurn * This,
            /* [out] */ BOOL *pfHasRecorder);
        
        END_INTERFACE
    } ICDBurnVtbl;

    interface ICDBurn
    {
        CONST_VTBL struct ICDBurnVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICDBurn_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICDBurn_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICDBurn_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICDBurn_GetRecorderDriveLetter(This,pszDrive,cch)	\
    (This)->lpVtbl -> GetRecorderDriveLetter(This,pszDrive,cch)

#define ICDBurn_Burn(This,hwnd)	\
    (This)->lpVtbl -> Burn(This,hwnd)

#define ICDBurn_HasRecordableDrive(This,pfHasRecorder)	\
    (This)->lpVtbl -> HasRecordableDrive(This,pfHasRecorder)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ICDBurn_GetRecorderDriveLetter_Proxy( 
    ICDBurn * This,
    /* [size_is][out] */ LPWSTR pszDrive,
    /* [in] */ UINT cch);


void __RPC_STUB ICDBurn_GetRecorderDriveLetter_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICDBurn_Burn_Proxy( 
    ICDBurn * This,
    /* [in] */ HWND hwnd);


void __RPC_STUB ICDBurn_Burn_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICDBurn_HasRecordableDrive_Proxy( 
    ICDBurn * This,
    /* [out] */ BOOL *pfHasRecorder);


void __RPC_STUB ICDBurn_HasRecordableDrive_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICDBurn_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0234 */
/* [local] */ 

#define IDD_WIZEXTN_FIRST    0x5000
#define IDD_WIZEXTN_LAST     0x5100


extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0234_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0234_v0_0_s_ifspec;

#ifndef __IWizardSite_INTERFACE_DEFINED__
#define __IWizardSite_INTERFACE_DEFINED__

/* interface IWizardSite */
/* [object][local][helpstring][uuid] */ 


EXTERN_C const IID IID_IWizardSite;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("88960f5b-422f-4e7b-8013-73415381c3c3")
    IWizardSite : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetPreviousPage( 
            /* [out] */ HPROPSHEETPAGE *phpage) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetNextPage( 
            /* [out] */ HPROPSHEETPAGE *phpage) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetCancelledPage( 
            /* [out] */ HPROPSHEETPAGE *phpage) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWizardSiteVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWizardSite * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWizardSite * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWizardSite * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetPreviousPage )( 
            IWizardSite * This,
            /* [out] */ HPROPSHEETPAGE *phpage);
        
        HRESULT ( STDMETHODCALLTYPE *GetNextPage )( 
            IWizardSite * This,
            /* [out] */ HPROPSHEETPAGE *phpage);
        
        HRESULT ( STDMETHODCALLTYPE *GetCancelledPage )( 
            IWizardSite * This,
            /* [out] */ HPROPSHEETPAGE *phpage);
        
        END_INTERFACE
    } IWizardSiteVtbl;

    interface IWizardSite
    {
        CONST_VTBL struct IWizardSiteVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWizardSite_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWizardSite_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWizardSite_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWizardSite_GetPreviousPage(This,phpage)	\
    (This)->lpVtbl -> GetPreviousPage(This,phpage)

#define IWizardSite_GetNextPage(This,phpage)	\
    (This)->lpVtbl -> GetNextPage(This,phpage)

#define IWizardSite_GetCancelledPage(This,phpage)	\
    (This)->lpVtbl -> GetCancelledPage(This,phpage)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IWizardSite_GetPreviousPage_Proxy( 
    IWizardSite * This,
    /* [out] */ HPROPSHEETPAGE *phpage);


void __RPC_STUB IWizardSite_GetPreviousPage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IWizardSite_GetNextPage_Proxy( 
    IWizardSite * This,
    /* [out] */ HPROPSHEETPAGE *phpage);


void __RPC_STUB IWizardSite_GetNextPage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IWizardSite_GetCancelledPage_Proxy( 
    IWizardSite * This,
    /* [out] */ HPROPSHEETPAGE *phpage);


void __RPC_STUB IWizardSite_GetCancelledPage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWizardSite_INTERFACE_DEFINED__ */


#ifndef __IWizardExtension_INTERFACE_DEFINED__
#define __IWizardExtension_INTERFACE_DEFINED__

/* interface IWizardExtension */
/* [object][local][helpstring][uuid] */ 


EXTERN_C const IID IID_IWizardExtension;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("c02ea696-86cc-491e-9b23-74394a0444a8")
    IWizardExtension : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE AddPages( 
            /* [out][in] */ HPROPSHEETPAGE *aPages,
            /* [in] */ UINT cPages,
            /* [out] */ UINT *pnPagesAdded) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetFirstPage( 
            /* [out] */ HPROPSHEETPAGE *phpage) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetLastPage( 
            /* [out] */ HPROPSHEETPAGE *phpage) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWizardExtensionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWizardExtension * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWizardExtension * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWizardExtension * This);
        
        HRESULT ( STDMETHODCALLTYPE *AddPages )( 
            IWizardExtension * This,
            /* [out][in] */ HPROPSHEETPAGE *aPages,
            /* [in] */ UINT cPages,
            /* [out] */ UINT *pnPagesAdded);
        
        HRESULT ( STDMETHODCALLTYPE *GetFirstPage )( 
            IWizardExtension * This,
            /* [out] */ HPROPSHEETPAGE *phpage);
        
        HRESULT ( STDMETHODCALLTYPE *GetLastPage )( 
            IWizardExtension * This,
            /* [out] */ HPROPSHEETPAGE *phpage);
        
        END_INTERFACE
    } IWizardExtensionVtbl;

    interface IWizardExtension
    {
        CONST_VTBL struct IWizardExtensionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWizardExtension_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWizardExtension_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWizardExtension_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWizardExtension_AddPages(This,aPages,cPages,pnPagesAdded)	\
    (This)->lpVtbl -> AddPages(This,aPages,cPages,pnPagesAdded)

#define IWizardExtension_GetFirstPage(This,phpage)	\
    (This)->lpVtbl -> GetFirstPage(This,phpage)

#define IWizardExtension_GetLastPage(This,phpage)	\
    (This)->lpVtbl -> GetLastPage(This,phpage)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IWizardExtension_AddPages_Proxy( 
    IWizardExtension * This,
    /* [out][in] */ HPROPSHEETPAGE *aPages,
    /* [in] */ UINT cPages,
    /* [out] */ UINT *pnPagesAdded);


void __RPC_STUB IWizardExtension_AddPages_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IWizardExtension_GetFirstPage_Proxy( 
    IWizardExtension * This,
    /* [out] */ HPROPSHEETPAGE *phpage);


void __RPC_STUB IWizardExtension_GetFirstPage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IWizardExtension_GetLastPage_Proxy( 
    IWizardExtension * This,
    /* [out] */ HPROPSHEETPAGE *phpage);


void __RPC_STUB IWizardExtension_GetLastPage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWizardExtension_INTERFACE_DEFINED__ */


#ifndef __IWebWizardExtension_INTERFACE_DEFINED__
#define __IWebWizardExtension_INTERFACE_DEFINED__

/* interface IWebWizardExtension */
/* [unique][object][uuid][helpstring] */ 


EXTERN_C const IID IID_IWebWizardExtension;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0e6b3f66-98d1-48c0-a222-fbde74e2fbc5")
    IWebWizardExtension : public IWizardExtension
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetInitialURL( 
            /* [string][in] */ LPCWSTR pszURL) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetErrorURL( 
            /* [string][in] */ LPCWSTR pszErrorURL) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IWebWizardExtensionVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IWebWizardExtension * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IWebWizardExtension * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IWebWizardExtension * This);
        
        HRESULT ( STDMETHODCALLTYPE *AddPages )( 
            IWebWizardExtension * This,
            /* [out][in] */ HPROPSHEETPAGE *aPages,
            /* [in] */ UINT cPages,
            /* [out] */ UINT *pnPagesAdded);
        
        HRESULT ( STDMETHODCALLTYPE *GetFirstPage )( 
            IWebWizardExtension * This,
            /* [out] */ HPROPSHEETPAGE *phpage);
        
        HRESULT ( STDMETHODCALLTYPE *GetLastPage )( 
            IWebWizardExtension * This,
            /* [out] */ HPROPSHEETPAGE *phpage);
        
        HRESULT ( STDMETHODCALLTYPE *SetInitialURL )( 
            IWebWizardExtension * This,
            /* [string][in] */ LPCWSTR pszURL);
        
        HRESULT ( STDMETHODCALLTYPE *SetErrorURL )( 
            IWebWizardExtension * This,
            /* [string][in] */ LPCWSTR pszErrorURL);
        
        END_INTERFACE
    } IWebWizardExtensionVtbl;

    interface IWebWizardExtension
    {
        CONST_VTBL struct IWebWizardExtensionVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IWebWizardExtension_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IWebWizardExtension_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IWebWizardExtension_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IWebWizardExtension_AddPages(This,aPages,cPages,pnPagesAdded)	\
    (This)->lpVtbl -> AddPages(This,aPages,cPages,pnPagesAdded)

#define IWebWizardExtension_GetFirstPage(This,phpage)	\
    (This)->lpVtbl -> GetFirstPage(This,phpage)

#define IWebWizardExtension_GetLastPage(This,phpage)	\
    (This)->lpVtbl -> GetLastPage(This,phpage)


#define IWebWizardExtension_SetInitialURL(This,pszURL)	\
    (This)->lpVtbl -> SetInitialURL(This,pszURL)

#define IWebWizardExtension_SetErrorURL(This,pszErrorURL)	\
    (This)->lpVtbl -> SetErrorURL(This,pszErrorURL)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IWebWizardExtension_SetInitialURL_Proxy( 
    IWebWizardExtension * This,
    /* [string][in] */ LPCWSTR pszURL);


void __RPC_STUB IWebWizardExtension_SetInitialURL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IWebWizardExtension_SetErrorURL_Proxy( 
    IWebWizardExtension * This,
    /* [string][in] */ LPCWSTR pszErrorURL);


void __RPC_STUB IWebWizardExtension_SetErrorURL_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IWebWizardExtension_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0237 */
/* [local] */ 

#define SID_WebWizardHost IID_IWebWizardExtension
#define SHPWHF_NORECOMPRESS             0x00000001  // don't allow/prompt for recompress of streams
#define SHPWHF_NONETPLACECREATE         0x00000002  // don't create a network place when transfer is complete
#define SHPWHF_NOFILESELECTOR           0x00000004  // don't show the file selector
#define SHPWHF_VALIDATEVIAWEBFOLDERS    0x00010000  // enable web folders to validate network places (ANP support)


extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0237_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0237_v0_0_s_ifspec;

#ifndef __IPublishingWizard_INTERFACE_DEFINED__
#define __IPublishingWizard_INTERFACE_DEFINED__

/* interface IPublishingWizard */
/* [unique][object][uuid][helpstring] */ 


EXTERN_C const IID IID_IPublishingWizard;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("aa9198bb-ccec-472d-beed-19a4f6733f7a")
    IPublishingWizard : public IWizardExtension
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Initialize( 
            /* [in] */ IDataObject *pdo,
            /* [in] */ DWORD dwOptions,
            /* [string][in] */ LPCWSTR pszServiceProvider) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetTransferManifest( 
            /* [out] */ HRESULT *phrFromTransfer,
            /* [out] */ IXMLDOMDocument **pdocManifest) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPublishingWizardVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPublishingWizard * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPublishingWizard * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPublishingWizard * This);
        
        HRESULT ( STDMETHODCALLTYPE *AddPages )( 
            IPublishingWizard * This,
            /* [out][in] */ HPROPSHEETPAGE *aPages,
            /* [in] */ UINT cPages,
            /* [out] */ UINT *pnPagesAdded);
        
        HRESULT ( STDMETHODCALLTYPE *GetFirstPage )( 
            IPublishingWizard * This,
            /* [out] */ HPROPSHEETPAGE *phpage);
        
        HRESULT ( STDMETHODCALLTYPE *GetLastPage )( 
            IPublishingWizard * This,
            /* [out] */ HPROPSHEETPAGE *phpage);
        
        HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            IPublishingWizard * This,
            /* [in] */ IDataObject *pdo,
            /* [in] */ DWORD dwOptions,
            /* [string][in] */ LPCWSTR pszServiceProvider);
        
        HRESULT ( STDMETHODCALLTYPE *GetTransferManifest )( 
            IPublishingWizard * This,
            /* [out] */ HRESULT *phrFromTransfer,
            /* [out] */ IXMLDOMDocument **pdocManifest);
        
        END_INTERFACE
    } IPublishingWizardVtbl;

    interface IPublishingWizard
    {
        CONST_VTBL struct IPublishingWizardVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPublishingWizard_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IPublishingWizard_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IPublishingWizard_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IPublishingWizard_AddPages(This,aPages,cPages,pnPagesAdded)	\
    (This)->lpVtbl -> AddPages(This,aPages,cPages,pnPagesAdded)

#define IPublishingWizard_GetFirstPage(This,phpage)	\
    (This)->lpVtbl -> GetFirstPage(This,phpage)

#define IPublishingWizard_GetLastPage(This,phpage)	\
    (This)->lpVtbl -> GetLastPage(This,phpage)


#define IPublishingWizard_Initialize(This,pdo,dwOptions,pszServiceProvider)	\
    (This)->lpVtbl -> Initialize(This,pdo,dwOptions,pszServiceProvider)

#define IPublishingWizard_GetTransferManifest(This,phrFromTransfer,pdocManifest)	\
    (This)->lpVtbl -> GetTransferManifest(This,phrFromTransfer,pdocManifest)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IPublishingWizard_Initialize_Proxy( 
    IPublishingWizard * This,
    /* [in] */ IDataObject *pdo,
    /* [in] */ DWORD dwOptions,
    /* [string][in] */ LPCWSTR pszServiceProvider);


void __RPC_STUB IPublishingWizard_Initialize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IPublishingWizard_GetTransferManifest_Proxy( 
    IPublishingWizard * This,
    /* [out] */ HRESULT *phrFromTransfer,
    /* [out] */ IXMLDOMDocument **pdocManifest);


void __RPC_STUB IPublishingWizard_GetTransferManifest_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IPublishingWizard_INTERFACE_DEFINED__ */


#ifndef __IFolderViewHost_INTERFACE_DEFINED__
#define __IFolderViewHost_INTERFACE_DEFINED__

/* interface IFolderViewHost */
/* [object][local][helpstring][uuid] */ 


EXTERN_C const IID IID_IFolderViewHost;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("1ea58f02-d55a-411d-b09e-9e65ac21605b")
    IFolderViewHost : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Initialize( 
            /* [in] */ HWND hwndParent,
            /* [in] */ IDataObject *pdo,
            /* [in] */ RECT *prc) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IFolderViewHostVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IFolderViewHost * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IFolderViewHost * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IFolderViewHost * This);
        
        HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            IFolderViewHost * This,
            /* [in] */ HWND hwndParent,
            /* [in] */ IDataObject *pdo,
            /* [in] */ RECT *prc);
        
        END_INTERFACE
    } IFolderViewHostVtbl;

    interface IFolderViewHost
    {
        CONST_VTBL struct IFolderViewHostVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFolderViewHost_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IFolderViewHost_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IFolderViewHost_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IFolderViewHost_Initialize(This,hwndParent,pdo,prc)	\
    (This)->lpVtbl -> Initialize(This,hwndParent,pdo,prc)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IFolderViewHost_Initialize_Proxy( 
    IFolderViewHost * This,
    /* [in] */ HWND hwndParent,
    /* [in] */ IDataObject *pdo,
    /* [in] */ RECT *prc);


void __RPC_STUB IFolderViewHost_Initialize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IFolderViewHost_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0239 */
/* [local] */ 

#define ACDD_VISIBLE        0x0001


extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0239_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0239_v0_0_s_ifspec;

#ifndef __IAutoCompleteDropDown_INTERFACE_DEFINED__
#define __IAutoCompleteDropDown_INTERFACE_DEFINED__

/* interface IAutoCompleteDropDown */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IAutoCompleteDropDown;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("3CD141F4-3C6A-11d2-BCAA-00C04FD929DB")
    IAutoCompleteDropDown : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetDropDownStatus( 
            /* [out] */ DWORD *pdwFlags,
            /* [string][out] */ LPWSTR *ppwszString) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ResetEnumerator( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAutoCompleteDropDownVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAutoCompleteDropDown * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAutoCompleteDropDown * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAutoCompleteDropDown * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetDropDownStatus )( 
            IAutoCompleteDropDown * This,
            /* [out] */ DWORD *pdwFlags,
            /* [string][out] */ LPWSTR *ppwszString);
        
        HRESULT ( STDMETHODCALLTYPE *ResetEnumerator )( 
            IAutoCompleteDropDown * This);
        
        END_INTERFACE
    } IAutoCompleteDropDownVtbl;

    interface IAutoCompleteDropDown
    {
        CONST_VTBL struct IAutoCompleteDropDownVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAutoCompleteDropDown_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAutoCompleteDropDown_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAutoCompleteDropDown_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAutoCompleteDropDown_GetDropDownStatus(This,pdwFlags,ppwszString)	\
    (This)->lpVtbl -> GetDropDownStatus(This,pdwFlags,ppwszString)

#define IAutoCompleteDropDown_ResetEnumerator(This)	\
    (This)->lpVtbl -> ResetEnumerator(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IAutoCompleteDropDown_GetDropDownStatus_Proxy( 
    IAutoCompleteDropDown * This,
    /* [out] */ DWORD *pdwFlags,
    /* [string][out] */ LPWSTR *ppwszString);


void __RPC_STUB IAutoCompleteDropDown_GetDropDownStatus_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IAutoCompleteDropDown_ResetEnumerator_Proxy( 
    IAutoCompleteDropDown * This);


void __RPC_STUB IAutoCompleteDropDown_ResetEnumerator_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAutoCompleteDropDown_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0240 */
/* [local] */ 

#define PPW_LAUNCHEDBYUSER       0x00000001      // The wizard was launch explicitly by the user, not on demand by the key manager


extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0240_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0240_v0_0_s_ifspec;

#ifndef __IModalWindow_INTERFACE_DEFINED__
#define __IModalWindow_INTERFACE_DEFINED__

/* interface IModalWindow */
/* [unique][object][uuid][helpstring] */ 


EXTERN_C const IID IID_IModalWindow;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("b4db1657-70d7-485e-8e3e-6fcb5a5c1802")
    IModalWindow : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Show( 
            /* [in] */ HWND hwndParent) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IModalWindowVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IModalWindow * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IModalWindow * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IModalWindow * This);
        
        HRESULT ( STDMETHODCALLTYPE *Show )( 
            IModalWindow * This,
            /* [in] */ HWND hwndParent);
        
        END_INTERFACE
    } IModalWindowVtbl;

    interface IModalWindow
    {
        CONST_VTBL struct IModalWindowVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IModalWindow_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IModalWindow_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IModalWindow_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IModalWindow_Show(This,hwndParent)	\
    (This)->lpVtbl -> Show(This,hwndParent)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IModalWindow_Show_Proxy( 
    IModalWindow * This,
    /* [in] */ HWND hwndParent);


void __RPC_STUB IModalWindow_Show_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IModalWindow_INTERFACE_DEFINED__ */

//////////////////////////////////////////////////////////////////////////

typedef /* [v1_enum] */
enum FDAP {
    FDAP_BOTTOM = 0,
    FDAP_TOP = 1
} 	FDAP;

enum _FILEOPENDIALOGOPTIONS {
    FOS_OVERWRITEPROMPT = 0x2,
    FOS_STRICTFILETYPES = 0x4,
    FOS_NOCHANGEDIR = 0x8,
    FOS_PICKFOLDERS = 0x20,
    FOS_FORCEFILESYSTEM = 0x40,
    FOS_ALLNONSTORAGEITEMS = 0x80,
    FOS_NOVALIDATE = 0x100,
    FOS_ALLOWMULTISELECT = 0x200,
    FOS_PATHMUSTEXIST = 0x800,
    FOS_FILEMUSTEXIST = 0x1000,
    FOS_CREATEPROMPT = 0x2000,
    FOS_SHAREAWARE = 0x4000,
    FOS_NOREADONLYRETURN = 0x8000,
    FOS_NOTESTFILECREATE = 0x10000,
    FOS_HIDEMRUPLACES = 0x20000,
    FOS_HIDEPINNEDPLACES = 0x40000,
    FOS_NODEREFERENCELINKS = 0x100000,
    FOS_DONTADDTORECENT = 0x2000000,
    FOS_FORCESHOWHIDDEN = 0x10000000,
    FOS_DEFAULTNOMINIMODE = 0x20000000,
    FOS_FORCEPREVIEWPANEON = 0x40000000
};
typedef DWORD FILEOPENDIALOGOPTIONS;

enum _TRANSFER_SOURCE_FLAGS {
    TSF_NORMAL = 0,
    TSF_FAIL_EXIST = 0,
    TSF_RENAME_EXIST = 0x1,
    TSF_OVERWRITE_EXIST = 0x2,
    TSF_ALLOW_DECRYPTION = 0x4,
    TSF_NO_SECURITY = 0x8,
    TSF_COPY_CREATION_TIME = 0x10,
    TSF_COPY_WRITE_TIME = 0x20,
    TSF_USE_FULL_ACCESS = 0x40,
    TSF_DELETE_RECYCLE_IF_POSSIBLE = 0x80,
    TSF_COPY_HARD_LINK = 0x100,
    TSF_COPY_LOCALIZED_NAME = 0x200,
    TSF_MOVE_AS_COPY_DELETE = 0x400,
    TSF_SUSPEND_SHELLEVENTS = 0x800
};
typedef DWORD TRANSFER_SOURCE_FLAGS;

#define FOF_NO_UI (FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NOCONFIRMMKDIR) // don't display any UI at all
#define FOFX_SHOWELEVATIONPROMPT 0x00040000 

#ifdef __cplusplus

MIDL_INTERFACE("04b0f1a7-9490-44bc-96e1-4296a31252e2")
IFileOperationProgressSink : public IUnknown
{
public:
    virtual HRESULT STDMETHODCALLTYPE StartOperations(void) = 0;

    virtual HRESULT STDMETHODCALLTYPE FinishOperations(
        /* [in] */ HRESULT hrResult) = 0;

    virtual HRESULT STDMETHODCALLTYPE PreRenameItem(
        /* [in] */ DWORD dwFlags,
        /* [in] */ IShellItem *psiItem,
        /* [string][unique][in] */ LPCWSTR pszNewName) = 0;

    virtual HRESULT STDMETHODCALLTYPE PostRenameItem(
        /* [in] */ DWORD dwFlags,
        /* [in] */ IShellItem *psiItem,
        /* [string][in] */ LPCWSTR pszNewName,
        /* [in] */ HRESULT hrRename,
        /* [in] */ IShellItem *psiNewlyCreated) = 0;

    virtual HRESULT STDMETHODCALLTYPE PreMoveItem(
        /* [in] */ DWORD dwFlags,
        /* [in] */ IShellItem *psiItem,
        /* [in] */ IShellItem *psiDestinationFolder,
        /* [string][unique][in] */ LPCWSTR pszNewName) = 0;

    virtual HRESULT STDMETHODCALLTYPE PostMoveItem(
        /* [in] */ DWORD dwFlags,
        /* [in] */ IShellItem *psiItem,
        /* [in] */ IShellItem *psiDestinationFolder,
        /* [string][unique][in] */ LPCWSTR pszNewName,
        /* [in] */ HRESULT hrMove,
        /* [in] */ IShellItem *psiNewlyCreated) = 0;

    virtual HRESULT STDMETHODCALLTYPE PreCopyItem(
        /* [in] */ DWORD dwFlags,
        /* [in] */ IShellItem *psiItem,
        /* [in] */ IShellItem *psiDestinationFolder,
        /* [string][unique][in] */ LPCWSTR pszNewName) = 0;

    virtual HRESULT STDMETHODCALLTYPE PostCopyItem(
        /* [in] */ DWORD dwFlags,
        /* [in] */ IShellItem *psiItem,
        /* [in] */ IShellItem *psiDestinationFolder,
        /* [string][unique][in] */ LPCWSTR pszNewName,
        /* [in] */ HRESULT hrCopy,
        /* [in] */ IShellItem *psiNewlyCreated) = 0;

    virtual HRESULT STDMETHODCALLTYPE PreDeleteItem(
        /* [in] */ DWORD dwFlags,
        /* [in] */ IShellItem *psiItem) = 0;

    virtual HRESULT STDMETHODCALLTYPE PostDeleteItem(
        /* [in] */ DWORD dwFlags,
        /* [in] */ IShellItem *psiItem,
        /* [in] */ HRESULT hrDelete,
        /* [in] */ IShellItem *psiNewlyCreated) = 0;

    virtual HRESULT STDMETHODCALLTYPE PreNewItem(
        /* [in] */ DWORD dwFlags,
        /* [in] */ IShellItem *psiDestinationFolder,
        /* [string][unique][in] */ LPCWSTR pszNewName) = 0;

    virtual HRESULT STDMETHODCALLTYPE PostNewItem(
        /* [in] */ DWORD dwFlags,
        /* [in] */ IShellItem *psiDestinationFolder,
        /* [string][unique][in] */ LPCWSTR pszNewName,
        /* [string][unique][in] */ LPCWSTR pszTemplateName,
        /* [in] */ DWORD dwFileAttributes,
        /* [in] */ HRESULT hrNew,
        /* [in] */ IShellItem *psiNewItem) = 0;

    virtual HRESULT STDMETHODCALLTYPE UpdateProgress(
        /* [in] */ UINT iWorkTotal,
        /* [in] */ UINT iWorkSoFar) = 0;

    virtual HRESULT STDMETHODCALLTYPE ResetTimer(void) = 0;

    virtual HRESULT STDMETHODCALLTYPE PauseTimer(void) = 0;

    virtual HRESULT STDMETHODCALLTYPE ResumeTimer(void) = 0;
};

MIDL_INTERFACE("947aab5f-0a5c-4c13-b4d6-4bf7836fc9f8")
IFileOperation : public IUnknown{
    public:
        virtual HRESULT STDMETHODCALLTYPE Advise(
            /* [in] */ IFileOperationProgressSink *pfops,
            /* [out] */ DWORD *pdwCookie) = 0;

        virtual HRESULT STDMETHODCALLTYPE Unadvise(
            /* [in] */ DWORD dwCookie) = 0;

        virtual HRESULT STDMETHODCALLTYPE SetOperationFlags(
            /* [in] */ DWORD dwOperationFlags) = 0;

        virtual HRESULT STDMETHODCALLTYPE SetProgressMessage(
            /* [string][in] */ LPCWSTR pszMessage) = 0;

        virtual HRESULT STDMETHODCALLTYPE SetProgressDialog(
            /* [in] */ /*IOperationsProgressDialog*/void *popd) = 0;

        virtual HRESULT STDMETHODCALLTYPE SetProperties(
            /* [in] */ /*IPropertyChangeArray*/void *pproparray) = 0;

        virtual HRESULT STDMETHODCALLTYPE SetOwnerWindow(
            /* [in] */ HWND hwndOwner) = 0;

        virtual HRESULT STDMETHODCALLTYPE ApplyPropertiesToItem(
            /* [in] */ IShellItem *psiItem) = 0;

        virtual HRESULT STDMETHODCALLTYPE ApplyPropertiesToItems(
            /* [in] */ IUnknown *punkItems) = 0;

        virtual HRESULT STDMETHODCALLTYPE RenameItem(
            /* [in] */ IShellItem *psiItem,
            /* [string][in] */ LPCWSTR pszNewName,
            /* [unique][in] */ IFileOperationProgressSink *pfopsItem) = 0;

        virtual HRESULT STDMETHODCALLTYPE RenameItems(
            /* [in] */ IUnknown *pUnkItems,
            /* [string][in] */ LPCWSTR pszNewName) = 0;

        virtual HRESULT STDMETHODCALLTYPE MoveItem(
            /* [in] */ IShellItem *psiItem,
            /* [in] */ IShellItem *psiDestinationFolder,
            /* [string][unique][in] */ LPCWSTR pszNewName,
            /* [unique][in] */ IFileOperationProgressSink *pfopsItem) = 0;

        virtual HRESULT STDMETHODCALLTYPE MoveItems(
            /* [in] */ IUnknown *punkItems,
            /* [in] */ IShellItem *psiDestinationFolder) = 0;

        virtual HRESULT STDMETHODCALLTYPE CopyItem(
            /* [in] */ IShellItem *psiItem,
            /* [in] */ IShellItem *psiDestinationFolder,
            /* [string][unique][in] */ LPCWSTR pszCopyName,
            /* [unique][in] */ IFileOperationProgressSink *pfopsItem) = 0;

        virtual HRESULT STDMETHODCALLTYPE CopyItems(
            /* [in] */ IUnknown *punkItems,
            /* [in] */ IShellItem *psiDestinationFolder) = 0;

        virtual HRESULT STDMETHODCALLTYPE DeleteItem(
            /* [in] */ IShellItem *psiItem,
            /* [unique][in] */ IFileOperationProgressSink *pfopsItem) = 0;

        virtual HRESULT STDMETHODCALLTYPE DeleteItems(
            /* [in] */ IUnknown *punkItems) = 0;

        virtual HRESULT STDMETHODCALLTYPE NewItem(
            /* [in] */ IShellItem *psiDestinationFolder,
            /* [in] */ DWORD dwFileAttributes,
            /* [string][unique][in] */ LPCWSTR pszName,
            /* [string][unique][in] */ LPCWSTR pszTemplateName,
            /* [unique][in] */ IFileOperationProgressSink *pfopsItem) = 0;

        virtual HRESULT STDMETHODCALLTYPE PerformOperations(void) = 0;

        virtual HRESULT STDMETHODCALLTYPE GetAnyOperationsAborted(
            /* [out] */ BOOL *pfAnyOperationsAborted) = 0;

};

MIDL_INTERFACE("42f85136-db7e-439c-85f1-e4075d135fc8")
IFileDialog : public IModalWindow
{
public:
    virtual HRESULT STDMETHODCALLTYPE SetFileTypes(
        /* [in] */ UINT cFileTypes,
        /* [size_is][in] */ const /*COMDLG_FILTERSPEC*/void *rgFilterSpec) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetFileTypeIndex(
        /* [in] */ UINT iFileType) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetFileTypeIndex(
        /* [out] */ UINT *piFileType) = 0;

    virtual HRESULT STDMETHODCALLTYPE Advise(
        /* [in] */ /*IFileDialogEvents*/void *pfde,
        /* [out] */ DWORD *pdwCookie) = 0;

    virtual HRESULT STDMETHODCALLTYPE Unadvise(
        /* [in] */ DWORD dwCookie) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetOptions(
        /* [in] */ FILEOPENDIALOGOPTIONS fos) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetOptions(
        /* [out] */ FILEOPENDIALOGOPTIONS *pfos) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetDefaultFolder(
        /* [in] */ IShellItem *psi) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetFolder(
        /* [in] */ IShellItem *psi) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetFolder(
        /* [out] */ IShellItem **ppsi) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetCurrentSelection(
        /* [out] */ IShellItem **ppsi) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetFileName(
        /* [string][in] */ LPCWSTR pszName) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetFileName(
        /* [string][out] */ LPWSTR *pszName) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetTitle(
        /* [string][in] */ LPCWSTR pszTitle) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetOkButtonLabel(
        /* [string][in] */ LPCWSTR pszText) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetFileNameLabel(
        /* [string][in] */ LPCWSTR pszLabel) = 0;

    virtual HRESULT STDMETHODCALLTYPE GetResult(
        /* [out] */ IShellItem **ppsi) = 0;

    virtual HRESULT STDMETHODCALLTYPE AddPlace(
        /* [in] */ IShellItem *psi,
        /* [in] */ FDAP fdap) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetDefaultExtension(
        /* [string][in] */ LPCWSTR pszDefaultExtension) = 0;

    virtual HRESULT STDMETHODCALLTYPE Close(
        /* [in] */ HRESULT hr) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetClientGuid(
        /* [in] */ REFGUID guid) = 0;

    virtual HRESULT STDMETHODCALLTYPE ClearClientData(void) = 0;

    virtual HRESULT STDMETHODCALLTYPE SetFilter(
        /* [in] */ /*IShellItemFilter*/void *pFilter) = 0;

};
#endif // __cplusplus
//////////////////////////////////////////////////////////////////////////


#ifndef __IPassportWizard_INTERFACE_DEFINED__
#define __IPassportWizard_INTERFACE_DEFINED__

/* interface IPassportWizard */
/* [unique][object][uuid][helpstring] */ 


EXTERN_C const IID IID_IPassportWizard;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("a09db586-9180-41ac-9114-460a7f362b76")
    IPassportWizard : public IModalWindow
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetOptions( 
            /* [in] */ DWORD dwOptions) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IPassportWizardVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IPassportWizard * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IPassportWizard * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IPassportWizard * This);
        
        HRESULT ( STDMETHODCALLTYPE *Show )( 
            IPassportWizard * This,
            /* [in] */ HWND hwndParent);
        
        HRESULT ( STDMETHODCALLTYPE *SetOptions )( 
            IPassportWizard * This,
            /* [in] */ DWORD dwOptions);
        
        END_INTERFACE
    } IPassportWizardVtbl;

    interface IPassportWizard
    {
        CONST_VTBL struct IPassportWizardVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IPassportWizard_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IPassportWizard_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IPassportWizard_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IPassportWizard_Show(This,hwndParent)	\
    (This)->lpVtbl -> Show(This,hwndParent)


#define IPassportWizard_SetOptions(This,dwOptions)	\
    (This)->lpVtbl -> SetOptions(This,dwOptions)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IPassportWizard_SetOptions_Proxy( 
    IPassportWizard * This,
    /* [in] */ DWORD dwOptions);


void __RPC_STUB IPassportWizard_SetOptions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IPassportWizard_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0241 */
/* [local] */ 

#define PROPSTR_EXTENSIONCOMPLETIONSTATE L"ExtensionCompletionState"

enum __MIDL___MIDL_itf_shobjidl_0241_0001
    {	CDBE_RET_DEFAULT	= 0,
	CDBE_RET_DONTRUNOTHEREXTS	= 0x1,
	CDBE_RET_STOPWIZARD	= 0x2
    } ;
#define SID_CDWizardHost IID_ICDBurnExt


extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0241_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0241_v0_0_s_ifspec;

#ifndef __ICDBurnExt_INTERFACE_DEFINED__
#define __ICDBurnExt_INTERFACE_DEFINED__

/* interface ICDBurnExt */
/* [object][helpstring][uuid] */ 


enum __MIDL_ICDBurnExt_0002
    {	CDBE_TYPE_MUSIC	= 0x1,
	CDBE_TYPE_DATA	= 0x2,
	CDBE_TYPE_ALL	= 0xffffffff
    } ;

EXTERN_C const IID IID_ICDBurnExt;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("2271dcca-74fc-4414-8fb7-c56b05ace2d7")
    ICDBurnExt : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetSupportedActionTypes( 
            /* [out] */ DWORD *pdwActions) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICDBurnExtVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICDBurnExt * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICDBurnExt * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICDBurnExt * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetSupportedActionTypes )( 
            ICDBurnExt * This,
            /* [out] */ DWORD *pdwActions);
        
        END_INTERFACE
    } ICDBurnExtVtbl;

    interface ICDBurnExt
    {
        CONST_VTBL struct ICDBurnExtVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICDBurnExt_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICDBurnExt_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICDBurnExt_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICDBurnExt_GetSupportedActionTypes(This,pdwActions)	\
    (This)->lpVtbl -> GetSupportedActionTypes(This,pdwActions)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ICDBurnExt_GetSupportedActionTypes_Proxy( 
    ICDBurnExt * This,
    /* [out] */ DWORD *pdwActions);


void __RPC_STUB ICDBurnExt_GetSupportedActionTypes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICDBurnExt_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0242 */
/* [local] */ 

typedef void ( *PFDVENUMREADYBALLBACK )( 
    LPVOID pvData);



extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0242_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0242_v0_0_s_ifspec;

#ifndef __IDVGetEnum_INTERFACE_DEFINED__
#define __IDVGetEnum_INTERFACE_DEFINED__

/* interface IDVGetEnum */
/* [object][local][helpstring][uuid] */ 


EXTERN_C const IID IID_IDVGetEnum;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("70F55181-5FEA-4900-B6B8-7343CB0A348C")
    IDVGetEnum : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetEnumReadyCallback( 
            /* [in] */ PFDVENUMREADYBALLBACK pfn,
            /* [in] */ LPVOID pvData) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateEnumIDListFromContents( 
            /* [in] */ LPCITEMIDLIST pidlFolder,
            /* [in] */ DWORD dwEnumFlags,
            /* [out] */ IEnumIDList **ppEnumIDList) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDVGetEnumVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDVGetEnum * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDVGetEnum * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDVGetEnum * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetEnumReadyCallback )( 
            IDVGetEnum * This,
            /* [in] */ PFDVENUMREADYBALLBACK pfn,
            /* [in] */ LPVOID pvData);
        
        HRESULT ( STDMETHODCALLTYPE *CreateEnumIDListFromContents )( 
            IDVGetEnum * This,
            /* [in] */ LPCITEMIDLIST pidlFolder,
            /* [in] */ DWORD dwEnumFlags,
            /* [out] */ IEnumIDList **ppEnumIDList);
        
        END_INTERFACE
    } IDVGetEnumVtbl;

    interface IDVGetEnum
    {
        CONST_VTBL struct IDVGetEnumVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDVGetEnum_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDVGetEnum_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDVGetEnum_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDVGetEnum_SetEnumReadyCallback(This,pfn,pvData)	\
    (This)->lpVtbl -> SetEnumReadyCallback(This,pfn,pvData)

#define IDVGetEnum_CreateEnumIDListFromContents(This,pidlFolder,dwEnumFlags,ppEnumIDList)	\
    (This)->lpVtbl -> CreateEnumIDListFromContents(This,pidlFolder,dwEnumFlags,ppEnumIDList)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDVGetEnum_SetEnumReadyCallback_Proxy( 
    IDVGetEnum * This,
    /* [in] */ PFDVENUMREADYBALLBACK pfn,
    /* [in] */ LPVOID pvData);


void __RPC_STUB IDVGetEnum_SetEnumReadyCallback_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDVGetEnum_CreateEnumIDListFromContents_Proxy( 
    IDVGetEnum * This,
    /* [in] */ LPCITEMIDLIST pidlFolder,
    /* [in] */ DWORD dwEnumFlags,
    /* [out] */ IEnumIDList **ppEnumIDList);


void __RPC_STUB IDVGetEnum_CreateEnumIDListFromContents_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDVGetEnum_INTERFACE_DEFINED__ */


#ifndef __IInsertItem_INTERFACE_DEFINED__
#define __IInsertItem_INTERFACE_DEFINED__

/* interface IInsertItem */
/* [local][object][uuid][helpstring] */ 


EXTERN_C const IID IID_IInsertItem;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D2B57227-3D23-4b95-93C0-492BD454C356")
    IInsertItem : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE InsertItem( 
            /* [in] */ LPCITEMIDLIST pidl) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IInsertItemVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IInsertItem * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IInsertItem * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IInsertItem * This);
        
        HRESULT ( STDMETHODCALLTYPE *InsertItem )( 
            IInsertItem * This,
            /* [in] */ LPCITEMIDLIST pidl);
        
        END_INTERFACE
    } IInsertItemVtbl;

    interface IInsertItem
    {
        CONST_VTBL struct IInsertItemVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IInsertItem_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IInsertItem_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IInsertItem_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IInsertItem_InsertItem(This,pidl)	\
    (This)->lpVtbl -> InsertItem(This,pidl)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IInsertItem_InsertItem_Proxy( 
    IInsertItem * This,
    /* [in] */ LPCITEMIDLIST pidl);


void __RPC_STUB IInsertItem_InsertItem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IInsertItem_INTERFACE_DEFINED__ */


#ifndef __IDeskBar_INTERFACE_DEFINED__
#define __IDeskBar_INTERFACE_DEFINED__

/* interface IDeskBar */
/* [local][unique][object][uuid] */ 


EXTERN_C const IID IID_IDeskBar;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("EB0FE173-1A3A-11D0-89B3-00A0C90A90AC")
    IDeskBar : public IOleWindow
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetClient( 
            /* [in] */ IUnknown *punkClient) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetClient( 
            /* [out] */ IUnknown **ppunkClient) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnPosRectChangeDB( 
            /* [in] */ LPRECT prc) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDeskBarVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDeskBar * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDeskBar * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDeskBar * This);
        
        /* [input_sync] */ HRESULT ( STDMETHODCALLTYPE *GetWindow )( 
            IDeskBar * This,
            /* [out] */ HWND *phwnd);
        
        HRESULT ( STDMETHODCALLTYPE *ContextSensitiveHelp )( 
            IDeskBar * This,
            /* [in] */ BOOL fEnterMode);
        
        HRESULT ( STDMETHODCALLTYPE *SetClient )( 
            IDeskBar * This,
            /* [in] */ IUnknown *punkClient);
        
        HRESULT ( STDMETHODCALLTYPE *GetClient )( 
            IDeskBar * This,
            /* [out] */ IUnknown **ppunkClient);
        
        HRESULT ( STDMETHODCALLTYPE *OnPosRectChangeDB )( 
            IDeskBar * This,
            /* [in] */ LPRECT prc);
        
        END_INTERFACE
    } IDeskBarVtbl;

    interface IDeskBar
    {
        CONST_VTBL struct IDeskBarVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDeskBar_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDeskBar_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDeskBar_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDeskBar_GetWindow(This,phwnd)	\
    (This)->lpVtbl -> GetWindow(This,phwnd)

#define IDeskBar_ContextSensitiveHelp(This,fEnterMode)	\
    (This)->lpVtbl -> ContextSensitiveHelp(This,fEnterMode)


#define IDeskBar_SetClient(This,punkClient)	\
    (This)->lpVtbl -> SetClient(This,punkClient)

#define IDeskBar_GetClient(This,ppunkClient)	\
    (This)->lpVtbl -> GetClient(This,ppunkClient)

#define IDeskBar_OnPosRectChangeDB(This,prc)	\
    (This)->lpVtbl -> OnPosRectChangeDB(This,prc)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDeskBar_SetClient_Proxy( 
    IDeskBar * This,
    /* [in] */ IUnknown *punkClient);


void __RPC_STUB IDeskBar_SetClient_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDeskBar_GetClient_Proxy( 
    IDeskBar * This,
    /* [out] */ IUnknown **ppunkClient);


void __RPC_STUB IDeskBar_GetClient_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IDeskBar_OnPosRectChangeDB_Proxy( 
    IDeskBar * This,
    /* [in] */ LPRECT prc);


void __RPC_STUB IDeskBar_OnPosRectChangeDB_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDeskBar_INTERFACE_DEFINED__ */


#ifndef __IMenuBand_INTERFACE_DEFINED__
#define __IMenuBand_INTERFACE_DEFINED__

/* interface IMenuBand */
/* [local][unique][object][uuid] */ 


enum __MIDL_IMenuBand_0001
    {	MBHANDCID_PIDLSELECT	= 0
    } ;

EXTERN_C const IID IID_IMenuBand;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("568804CD-CBD7-11d0-9816-00C04FD91972")
    IMenuBand : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE IsMenuMessage( 
            /* [in] */ MSG *pmsg) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE TranslateMenuMessage( 
            /* [out][in] */ MSG *pmsg,
            /* [out] */ LRESULT *plRet) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMenuBandVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMenuBand * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMenuBand * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMenuBand * This);
        
        HRESULT ( STDMETHODCALLTYPE *IsMenuMessage )( 
            IMenuBand * This,
            /* [in] */ MSG *pmsg);
        
        HRESULT ( STDMETHODCALLTYPE *TranslateMenuMessage )( 
            IMenuBand * This,
            /* [out][in] */ MSG *pmsg,
            /* [out] */ LRESULT *plRet);
        
        END_INTERFACE
    } IMenuBandVtbl;

    interface IMenuBand
    {
        CONST_VTBL struct IMenuBandVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMenuBand_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMenuBand_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMenuBand_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMenuBand_IsMenuMessage(This,pmsg)	\
    (This)->lpVtbl -> IsMenuMessage(This,pmsg)

#define IMenuBand_TranslateMenuMessage(This,pmsg,plRet)	\
    (This)->lpVtbl -> TranslateMenuMessage(This,pmsg,plRet)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IMenuBand_IsMenuMessage_Proxy( 
    IMenuBand * This,
    /* [in] */ MSG *pmsg);


void __RPC_STUB IMenuBand_IsMenuMessage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMenuBand_TranslateMenuMessage_Proxy( 
    IMenuBand * This,
    /* [out][in] */ MSG *pmsg,
    /* [out] */ LRESULT *plRet);


void __RPC_STUB IMenuBand_TranslateMenuMessage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMenuBand_INTERFACE_DEFINED__ */


#ifndef __IFolderBandPriv_INTERFACE_DEFINED__
#define __IFolderBandPriv_INTERFACE_DEFINED__

/* interface IFolderBandPriv */
/* [unique][uuid][object] */ 


EXTERN_C const IID IID_IFolderBandPriv;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("47c01f95-e185-412c-b5c5-4f27df965aea")
    IFolderBandPriv : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetCascade( 
            /* [in] */ BOOL f) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetAccelerators( 
            /* [in] */ BOOL f) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetNoIcons( 
            /* [in] */ BOOL f) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetNoText( 
            /* [in] */ BOOL f) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IFolderBandPrivVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IFolderBandPriv * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IFolderBandPriv * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IFolderBandPriv * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetCascade )( 
            IFolderBandPriv * This,
            /* [in] */ BOOL f);
        
        HRESULT ( STDMETHODCALLTYPE *SetAccelerators )( 
            IFolderBandPriv * This,
            /* [in] */ BOOL f);
        
        HRESULT ( STDMETHODCALLTYPE *SetNoIcons )( 
            IFolderBandPriv * This,
            /* [in] */ BOOL f);
        
        HRESULT ( STDMETHODCALLTYPE *SetNoText )( 
            IFolderBandPriv * This,
            /* [in] */ BOOL f);
        
        END_INTERFACE
    } IFolderBandPrivVtbl;

    interface IFolderBandPriv
    {
        CONST_VTBL struct IFolderBandPrivVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IFolderBandPriv_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IFolderBandPriv_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IFolderBandPriv_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IFolderBandPriv_SetCascade(This,f)	\
    (This)->lpVtbl -> SetCascade(This,f)

#define IFolderBandPriv_SetAccelerators(This,f)	\
    (This)->lpVtbl -> SetAccelerators(This,f)

#define IFolderBandPriv_SetNoIcons(This,f)	\
    (This)->lpVtbl -> SetNoIcons(This,f)

#define IFolderBandPriv_SetNoText(This,f)	\
    (This)->lpVtbl -> SetNoText(This,f)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IFolderBandPriv_SetCascade_Proxy( 
    IFolderBandPriv * This,
    /* [in] */ BOOL f);


void __RPC_STUB IFolderBandPriv_SetCascade_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IFolderBandPriv_SetAccelerators_Proxy( 
    IFolderBandPriv * This,
    /* [in] */ BOOL f);


void __RPC_STUB IFolderBandPriv_SetAccelerators_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IFolderBandPriv_SetNoIcons_Proxy( 
    IFolderBandPriv * This,
    /* [in] */ BOOL f);


void __RPC_STUB IFolderBandPriv_SetNoIcons_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IFolderBandPriv_SetNoText_Proxy( 
    IFolderBandPriv * This,
    /* [in] */ BOOL f);


void __RPC_STUB IFolderBandPriv_SetNoText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IFolderBandPriv_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0247 */
/* [local] */ 

#if _WIN32_IE >= 0x0400
#include <pshpack8.h>
typedef struct tagBANDSITEINFO
    {
    DWORD dwMask;
    DWORD dwState;
    DWORD dwStyle;
    } 	BANDSITEINFO;

#include <poppack.h>

enum __MIDL___MIDL_itf_shobjidl_0247_0001
    {	BSID_BANDADDED	= 0,
	BSID_BANDREMOVED	= BSID_BANDADDED + 1
    } ;
#define BSIM_STATE          0x00000001
#define BSIM_STYLE          0x00000002
#define BSSF_VISIBLE        0x00000001
#define BSSF_NOTITLE        0x00000002
#define BSSF_UNDELETEABLE   0x00001000
#define BSIS_AUTOGRIPPER    0x00000000
#define BSIS_NOGRIPPER      0x00000001
#define BSIS_ALWAYSGRIPPER  0x00000002
#define BSIS_LEFTALIGN      0x00000004
#define BSIS_SINGLECLICK    0x00000008
#define BSIS_NOCONTEXTMENU  0x00000010
#define BSIS_NODROPTARGET   0x00000020
#define BSIS_NOCAPTION      0x00000040
#define BSIS_PREFERNOLINEBREAK   0x00000080
#define BSIS_LOCKED         0x00000100
#define SID_SBandSite IID_IBandSite
#define CGID_BandSite IID_IBandSite


extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0247_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0247_v0_0_s_ifspec;

#ifndef __IBandSite_INTERFACE_DEFINED__
#define __IBandSite_INTERFACE_DEFINED__

/* interface IBandSite */
/* [uuid][object] */ 


EXTERN_C const IID IID_IBandSite;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4CF504B0-DE96-11D0-8B3F-00A0C911E8E5")
    IBandSite : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE AddBand( 
            /* [in] */ IUnknown *punk) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnumBands( 
            /* [in] */ UINT uBand,
            /* [out] */ DWORD *pdwBandID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE QueryBand( 
            /* [in] */ DWORD dwBandID,
            /* [out] */ IDeskBand **ppstb,
            /* [out] */ DWORD *pdwState,
            /* [size_is][out] */ LPWSTR pszName,
            /* [in] */ int cchName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetBandState( 
            /* [in] */ DWORD dwBandID,
            /* [in] */ DWORD dwMask,
            /* [in] */ DWORD dwState) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE RemoveBand( 
            /* [in] */ DWORD dwBandID) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetBandObject( 
            /* [in] */ DWORD dwBandID,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppv) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetBandSiteInfo( 
            /* [in] */ const BANDSITEINFO *pbsinfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetBandSiteInfo( 
            /* [out][in] */ BANDSITEINFO *pbsinfo) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IBandSiteVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IBandSite * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IBandSite * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IBandSite * This);
        
        HRESULT ( STDMETHODCALLTYPE *AddBand )( 
            IBandSite * This,
            /* [in] */ IUnknown *punk);
        
        HRESULT ( STDMETHODCALLTYPE *EnumBands )( 
            IBandSite * This,
            /* [in] */ UINT uBand,
            /* [out] */ DWORD *pdwBandID);
        
        HRESULT ( STDMETHODCALLTYPE *QueryBand )( 
            IBandSite * This,
            /* [in] */ DWORD dwBandID,
            /* [out] */ IDeskBand **ppstb,
            /* [out] */ DWORD *pdwState,
            /* [size_is][out] */ LPWSTR pszName,
            /* [in] */ int cchName);
        
        HRESULT ( STDMETHODCALLTYPE *SetBandState )( 
            IBandSite * This,
            /* [in] */ DWORD dwBandID,
            /* [in] */ DWORD dwMask,
            /* [in] */ DWORD dwState);
        
        HRESULT ( STDMETHODCALLTYPE *RemoveBand )( 
            IBandSite * This,
            /* [in] */ DWORD dwBandID);
        
        HRESULT ( STDMETHODCALLTYPE *GetBandObject )( 
            IBandSite * This,
            /* [in] */ DWORD dwBandID,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppv);
        
        HRESULT ( STDMETHODCALLTYPE *SetBandSiteInfo )( 
            IBandSite * This,
            /* [in] */ const BANDSITEINFO *pbsinfo);
        
        HRESULT ( STDMETHODCALLTYPE *GetBandSiteInfo )( 
            IBandSite * This,
            /* [out][in] */ BANDSITEINFO *pbsinfo);
        
        END_INTERFACE
    } IBandSiteVtbl;

    interface IBandSite
    {
        CONST_VTBL struct IBandSiteVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBandSite_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IBandSite_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IBandSite_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IBandSite_AddBand(This,punk)	\
    (This)->lpVtbl -> AddBand(This,punk)

#define IBandSite_EnumBands(This,uBand,pdwBandID)	\
    (This)->lpVtbl -> EnumBands(This,uBand,pdwBandID)

#define IBandSite_QueryBand(This,dwBandID,ppstb,pdwState,pszName,cchName)	\
    (This)->lpVtbl -> QueryBand(This,dwBandID,ppstb,pdwState,pszName,cchName)

#define IBandSite_SetBandState(This,dwBandID,dwMask,dwState)	\
    (This)->lpVtbl -> SetBandState(This,dwBandID,dwMask,dwState)

#define IBandSite_RemoveBand(This,dwBandID)	\
    (This)->lpVtbl -> RemoveBand(This,dwBandID)

#define IBandSite_GetBandObject(This,dwBandID,riid,ppv)	\
    (This)->lpVtbl -> GetBandObject(This,dwBandID,riid,ppv)

#define IBandSite_SetBandSiteInfo(This,pbsinfo)	\
    (This)->lpVtbl -> SetBandSiteInfo(This,pbsinfo)

#define IBandSite_GetBandSiteInfo(This,pbsinfo)	\
    (This)->lpVtbl -> GetBandSiteInfo(This,pbsinfo)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IBandSite_AddBand_Proxy( 
    IBandSite * This,
    /* [in] */ IUnknown *punk);


void __RPC_STUB IBandSite_AddBand_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBandSite_EnumBands_Proxy( 
    IBandSite * This,
    /* [in] */ UINT uBand,
    /* [out] */ DWORD *pdwBandID);


void __RPC_STUB IBandSite_EnumBands_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBandSite_QueryBand_Proxy( 
    IBandSite * This,
    /* [in] */ DWORD dwBandID,
    /* [out] */ IDeskBand **ppstb,
    /* [out] */ DWORD *pdwState,
    /* [size_is][out] */ LPWSTR pszName,
    /* [in] */ int cchName);


void __RPC_STUB IBandSite_QueryBand_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBandSite_SetBandState_Proxy( 
    IBandSite * This,
    /* [in] */ DWORD dwBandID,
    /* [in] */ DWORD dwMask,
    /* [in] */ DWORD dwState);


void __RPC_STUB IBandSite_SetBandState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBandSite_RemoveBand_Proxy( 
    IBandSite * This,
    /* [in] */ DWORD dwBandID);


void __RPC_STUB IBandSite_RemoveBand_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBandSite_GetBandObject_Proxy( 
    IBandSite * This,
    /* [in] */ DWORD dwBandID,
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ void **ppv);


void __RPC_STUB IBandSite_GetBandObject_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBandSite_SetBandSiteInfo_Proxy( 
    IBandSite * This,
    /* [in] */ const BANDSITEINFO *pbsinfo);


void __RPC_STUB IBandSite_SetBandSiteInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IBandSite_GetBandSiteInfo_Proxy( 
    IBandSite * This,
    /* [out][in] */ BANDSITEINFO *pbsinfo);


void __RPC_STUB IBandSite_GetBandSiteInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IBandSite_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0248 */
/* [local] */ 

#endif // _WIN32_IE >= 0x0400


extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0248_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0248_v0_0_s_ifspec;

#ifndef __INamespaceWalkCB_INTERFACE_DEFINED__
#define __INamespaceWalkCB_INTERFACE_DEFINED__

/* interface INamespaceWalkCB */
/* [object][helpstring][uuid] */ 


EXTERN_C const IID IID_INamespaceWalkCB;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("d92995f8-cf5e-4a76-bf59-ead39ea2b97e")
    INamespaceWalkCB : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE FoundItem( 
            /* [in] */ IShellFolder *psf,
            /* [in] */ LPCITEMIDLIST pidl) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE EnterFolder( 
            /* [in] */ IShellFolder *psf,
            /* [in] */ LPCITEMIDLIST pidl) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE LeaveFolder( 
            /* [in] */ IShellFolder *psf,
            /* [in] */ LPCITEMIDLIST pidl) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE InitializeProgressDialog( 
            /* [string][out] */ LPWSTR *ppszTitle,
            /* [string][out] */ LPWSTR *ppszCancel) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INamespaceWalkCBVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INamespaceWalkCB * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INamespaceWalkCB * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INamespaceWalkCB * This);
        
        HRESULT ( STDMETHODCALLTYPE *FoundItem )( 
            INamespaceWalkCB * This,
            /* [in] */ IShellFolder *psf,
            /* [in] */ LPCITEMIDLIST pidl);
        
        HRESULT ( STDMETHODCALLTYPE *EnterFolder )( 
            INamespaceWalkCB * This,
            /* [in] */ IShellFolder *psf,
            /* [in] */ LPCITEMIDLIST pidl);
        
        HRESULT ( STDMETHODCALLTYPE *LeaveFolder )( 
            INamespaceWalkCB * This,
            /* [in] */ IShellFolder *psf,
            /* [in] */ LPCITEMIDLIST pidl);
        
        HRESULT ( STDMETHODCALLTYPE *InitializeProgressDialog )( 
            INamespaceWalkCB * This,
            /* [string][out] */ LPWSTR *ppszTitle,
            /* [string][out] */ LPWSTR *ppszCancel);
        
        END_INTERFACE
    } INamespaceWalkCBVtbl;

    interface INamespaceWalkCB
    {
        CONST_VTBL struct INamespaceWalkCBVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INamespaceWalkCB_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INamespaceWalkCB_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INamespaceWalkCB_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INamespaceWalkCB_FoundItem(This,psf,pidl)	\
    (This)->lpVtbl -> FoundItem(This,psf,pidl)

#define INamespaceWalkCB_EnterFolder(This,psf,pidl)	\
    (This)->lpVtbl -> EnterFolder(This,psf,pidl)

#define INamespaceWalkCB_LeaveFolder(This,psf,pidl)	\
    (This)->lpVtbl -> LeaveFolder(This,psf,pidl)

#define INamespaceWalkCB_InitializeProgressDialog(This,ppszTitle,ppszCancel)	\
    (This)->lpVtbl -> InitializeProgressDialog(This,ppszTitle,ppszCancel)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE INamespaceWalkCB_FoundItem_Proxy( 
    INamespaceWalkCB * This,
    /* [in] */ IShellFolder *psf,
    /* [in] */ LPCITEMIDLIST pidl);


void __RPC_STUB INamespaceWalkCB_FoundItem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INamespaceWalkCB_EnterFolder_Proxy( 
    INamespaceWalkCB * This,
    /* [in] */ IShellFolder *psf,
    /* [in] */ LPCITEMIDLIST pidl);


void __RPC_STUB INamespaceWalkCB_EnterFolder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INamespaceWalkCB_LeaveFolder_Proxy( 
    INamespaceWalkCB * This,
    /* [in] */ IShellFolder *psf,
    /* [in] */ LPCITEMIDLIST pidl);


void __RPC_STUB INamespaceWalkCB_LeaveFolder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INamespaceWalkCB_InitializeProgressDialog_Proxy( 
    INamespaceWalkCB * This,
    /* [string][out] */ LPWSTR *ppszTitle,
    /* [string][out] */ LPWSTR *ppszCancel);


void __RPC_STUB INamespaceWalkCB_InitializeProgressDialog_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INamespaceWalkCB_INTERFACE_DEFINED__ */


#ifndef __INamespaceWalk_INTERFACE_DEFINED__
#define __INamespaceWalk_INTERFACE_DEFINED__

/* interface INamespaceWalk */
/* [object][helpstring][uuid] */ 


enum __MIDL_INamespaceWalk_0001
    {	NSWF_NONE_IMPLIES_ALL	= 0x1,
	NSWF_ONE_IMPLIES_ALL	= 0x2,
	NSWF_DONT_TRAVERSE_LINKS	= 0x4,
	NSWF_DONT_ACCUMULATE_RESULT	= 0x8,
	NSWF_TRAVERSE_STREAM_JUNCTIONS	= 0x10,
	NSWF_FILESYSTEM_ONLY	= 0x20,
	NSWF_SHOW_PROGRESS	= 0x40,
	NSWF_FLAG_VIEWORDER	= 0x80,
	NSWF_IGNORE_AUTOPLAY_HIDA	= 0x100
    } ;

EXTERN_C const IID IID_INamespaceWalk;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("57ced8a7-3f4a-432c-9350-30f24483f74f")
    INamespaceWalk : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Walk( 
            /* [in] */ IUnknown *punkToWalk,
            /* [in] */ DWORD dwFlags,
            /* [in] */ int cDepth,
            /* [in] */ INamespaceWalkCB *pnswcb) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetIDArrayResult( 
            /* [out] */ UINT *pcItems,
            /* [length_is][size_is][out] */ LPITEMIDLIST **pppidl) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INamespaceWalkVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INamespaceWalk * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INamespaceWalk * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INamespaceWalk * This);
        
        HRESULT ( STDMETHODCALLTYPE *Walk )( 
            INamespaceWalk * This,
            /* [in] */ IUnknown *punkToWalk,
            /* [in] */ DWORD dwFlags,
            /* [in] */ int cDepth,
            /* [in] */ INamespaceWalkCB *pnswcb);
        
        HRESULT ( STDMETHODCALLTYPE *GetIDArrayResult )( 
            INamespaceWalk * This,
            /* [out] */ UINT *pcItems,
            /* [length_is][size_is][out] */ LPITEMIDLIST **pppidl);
        
        END_INTERFACE
    } INamespaceWalkVtbl;

    interface INamespaceWalk
    {
        CONST_VTBL struct INamespaceWalkVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INamespaceWalk_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INamespaceWalk_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INamespaceWalk_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INamespaceWalk_Walk(This,punkToWalk,dwFlags,cDepth,pnswcb)	\
    (This)->lpVtbl -> Walk(This,punkToWalk,dwFlags,cDepth,pnswcb)

#define INamespaceWalk_GetIDArrayResult(This,pcItems,pppidl)	\
    (This)->lpVtbl -> GetIDArrayResult(This,pcItems,pppidl)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE INamespaceWalk_Walk_Proxy( 
    INamespaceWalk * This,
    /* [in] */ IUnknown *punkToWalk,
    /* [in] */ DWORD dwFlags,
    /* [in] */ int cDepth,
    /* [in] */ INamespaceWalkCB *pnswcb);


void __RPC_STUB INamespaceWalk_Walk_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE INamespaceWalk_GetIDArrayResult_Proxy( 
    INamespaceWalk * This,
    /* [out] */ UINT *pcItems,
    /* [length_is][size_is][out] */ LPITEMIDLIST **pppidl);


void __RPC_STUB INamespaceWalk_GetIDArrayResult_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INamespaceWalk_INTERFACE_DEFINED__ */


#ifndef __IRegTreeItem_INTERFACE_DEFINED__
#define __IRegTreeItem_INTERFACE_DEFINED__

/* interface IRegTreeItem */
/* [object][local][helpstring][uuid] */ 


EXTERN_C const IID IID_IRegTreeItem;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("A9521922-0812-4d44-9EC3-7FD38C726F3D")
    IRegTreeItem : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetCheckState( 
            /* [out] */ BOOL *pbCheck) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetCheckState( 
            /* [in] */ BOOL bCheck) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IRegTreeItemVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IRegTreeItem * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IRegTreeItem * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IRegTreeItem * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetCheckState )( 
            IRegTreeItem * This,
            /* [out] */ BOOL *pbCheck);
        
        HRESULT ( STDMETHODCALLTYPE *SetCheckState )( 
            IRegTreeItem * This,
            /* [in] */ BOOL bCheck);
        
        END_INTERFACE
    } IRegTreeItemVtbl;

    interface IRegTreeItem
    {
        CONST_VTBL struct IRegTreeItemVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IRegTreeItem_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IRegTreeItem_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IRegTreeItem_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IRegTreeItem_GetCheckState(This,pbCheck)	\
    (This)->lpVtbl -> GetCheckState(This,pbCheck)

#define IRegTreeItem_SetCheckState(This,bCheck)	\
    (This)->lpVtbl -> SetCheckState(This,bCheck)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IRegTreeItem_GetCheckState_Proxy( 
    IRegTreeItem * This,
    /* [out] */ BOOL *pbCheck);


void __RPC_STUB IRegTreeItem_GetCheckState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IRegTreeItem_SetCheckState_Proxy( 
    IRegTreeItem * This,
    /* [in] */ BOOL bCheck);


void __RPC_STUB IRegTreeItem_SetCheckState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IRegTreeItem_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0251 */
/* [local] */ 


enum __MIDL___MIDL_itf_shobjidl_0251_0001
    {	MPOS_EXECUTE	= 0,
	MPOS_FULLCANCEL	= MPOS_EXECUTE + 1,
	MPOS_CANCELLEVEL	= MPOS_FULLCANCEL + 1,
	MPOS_SELECTLEFT	= MPOS_CANCELLEVEL + 1,
	MPOS_SELECTRIGHT	= MPOS_SELECTLEFT + 1,
	MPOS_CHILDTRACKING	= MPOS_SELECTRIGHT + 1
    } ;

enum __MIDL___MIDL_itf_shobjidl_0251_0002
    {	MPPF_SETFOCUS	= 0x1,
	MPPF_INITIALSELECT	= 0x2,
	MPPF_NOANIMATE	= 0x4,
	MPPF_KEYBOARD	= 0x10,
	MPPF_REPOSITION	= 0x20,
	MPPF_FORCEZORDER	= 0x40,
	MPPF_FINALSELECT	= 0x80,
	MPPF_TOP	= 0x20000000,
	MPPF_LEFT	= 0x40000000,
	MPPF_RIGHT	= 0x60000000,
	MPPF_BOTTOM	= 0x80000000,
	MPPF_POS_MASK	= 0xe0000000
    } ;


extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0251_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0251_v0_0_s_ifspec;

#ifndef __IMenuPopup_INTERFACE_DEFINED__
#define __IMenuPopup_INTERFACE_DEFINED__

/* interface IMenuPopup */
/* [local][unique][object][uuid] */ 


EXTERN_C const IID IID_IMenuPopup;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D1E7AFEB-6A2E-11d0-8C78-00C04FD918B4")
    IMenuPopup : public IDeskBar
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Popup( 
            /* [in] */ POINTL *ppt,
            /* [in] */ RECTL *prcExclude,
            DWORD dwFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnSelect( 
            DWORD dwSelectType) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetSubMenu( 
            /* [in] */ IMenuPopup *pmp,
            BOOL fSet) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IMenuPopupVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IMenuPopup * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IMenuPopup * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IMenuPopup * This);
        
        /* [input_sync] */ HRESULT ( STDMETHODCALLTYPE *GetWindow )( 
            IMenuPopup * This,
            /* [out] */ HWND *phwnd);
        
        HRESULT ( STDMETHODCALLTYPE *ContextSensitiveHelp )( 
            IMenuPopup * This,
            /* [in] */ BOOL fEnterMode);
        
        HRESULT ( STDMETHODCALLTYPE *SetClient )( 
            IMenuPopup * This,
            /* [in] */ IUnknown *punkClient);
        
        HRESULT ( STDMETHODCALLTYPE *GetClient )( 
            IMenuPopup * This,
            /* [out] */ IUnknown **ppunkClient);
        
        HRESULT ( STDMETHODCALLTYPE *OnPosRectChangeDB )( 
            IMenuPopup * This,
            /* [in] */ LPRECT prc);
        
        HRESULT ( STDMETHODCALLTYPE *Popup )( 
            IMenuPopup * This,
            /* [in] */ POINTL *ppt,
            /* [in] */ RECTL *prcExclude,
            DWORD dwFlags);
        
        HRESULT ( STDMETHODCALLTYPE *OnSelect )( 
            IMenuPopup * This,
            DWORD dwSelectType);
        
        HRESULT ( STDMETHODCALLTYPE *SetSubMenu )( 
            IMenuPopup * This,
            /* [in] */ IMenuPopup *pmp,
            BOOL fSet);
        
        END_INTERFACE
    } IMenuPopupVtbl;

    interface IMenuPopup
    {
        CONST_VTBL struct IMenuPopupVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IMenuPopup_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IMenuPopup_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IMenuPopup_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IMenuPopup_GetWindow(This,phwnd)	\
    (This)->lpVtbl -> GetWindow(This,phwnd)

#define IMenuPopup_ContextSensitiveHelp(This,fEnterMode)	\
    (This)->lpVtbl -> ContextSensitiveHelp(This,fEnterMode)


#define IMenuPopup_SetClient(This,punkClient)	\
    (This)->lpVtbl -> SetClient(This,punkClient)

#define IMenuPopup_GetClient(This,ppunkClient)	\
    (This)->lpVtbl -> GetClient(This,ppunkClient)

#define IMenuPopup_OnPosRectChangeDB(This,prc)	\
    (This)->lpVtbl -> OnPosRectChangeDB(This,prc)


#define IMenuPopup_Popup(This,ppt,prcExclude,dwFlags)	\
    (This)->lpVtbl -> Popup(This,ppt,prcExclude,dwFlags)

#define IMenuPopup_OnSelect(This,dwSelectType)	\
    (This)->lpVtbl -> OnSelect(This,dwSelectType)

#define IMenuPopup_SetSubMenu(This,pmp,fSet)	\
    (This)->lpVtbl -> SetSubMenu(This,pmp,fSet)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IMenuPopup_Popup_Proxy( 
    IMenuPopup * This,
    /* [in] */ POINTL *ppt,
    /* [in] */ RECTL *prcExclude,
    DWORD dwFlags);


void __RPC_STUB IMenuPopup_Popup_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMenuPopup_OnSelect_Proxy( 
    IMenuPopup * This,
    DWORD dwSelectType);


void __RPC_STUB IMenuPopup_OnSelect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IMenuPopup_SetSubMenu_Proxy( 
    IMenuPopup * This,
    /* [in] */ IMenuPopup *pmp,
    BOOL fSet);


void __RPC_STUB IMenuPopup_SetSubMenu_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IMenuPopup_INTERFACE_DEFINED__ */


#ifndef __IShellItem_INTERFACE_DEFINED__
#define __IShellItem_INTERFACE_DEFINED__

/* interface IShellItem */
/* [unique][object][uuid][helpstring] */ 

typedef /* [public][public][v1_enum] */ 
enum __MIDL_IShellItem_0001
    {	SIGDN_NORMALDISPLAY	= 0,
	SIGDN_PARENTRELATIVEPARSING	= 0x80018001,
	SIGDN_PARENTRELATIVEFORADDRESSBAR	= 0x8001c001,
	SIGDN_DESKTOPABSOLUTEPARSING	= 0x80028000,
	SIGDN_PARENTRELATIVEEDITING	= 0x80031001,
	SIGDN_DESKTOPABSOLUTEEDITING	= 0x8004c000,
	SIGDN_FILESYSPATH	= 0x80058000,
	SIGDN_URL	= 0x80068000
    } 	SIGDN;

/* [v1_enum] */ 
enum __MIDL_IShellItem_0002
    {	SICHINT_DISPLAY	= 0,
	SICHINT_ALLFIELDS	= 0x80000000,
	SICHINT_CANONICAL	= 0x10000000
    } ;
typedef DWORD SICHINTF;


EXTERN_C const IID IID_IShellItem;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("43826d1e-e718-42ee-bc55-a1e261c37bfe")
    IShellItem : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE BindToHandler( 
            /* [in] */ IBindCtx *pbc,
            /* [in] */ REFGUID rbhid,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvOut) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetParent( 
            /* [out] */ IShellItem **ppsi) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetDisplayName( 
            /* [in] */ SIGDN sigdnName,
            /* [string][out] */ LPOLESTR *ppszName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetAttributes( 
            /* [in] */ SFGAOF sfgaoMask,
            /* [out] */ SFGAOF *psfgaoAttribs) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Compare( 
            /* [in] */ IShellItem *psi,
            /* [in] */ SICHINTF hint,
            /* [out] */ int *piOrder) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IShellItemVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IShellItem * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IShellItem * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IShellItem * This);
        
        HRESULT ( STDMETHODCALLTYPE *BindToHandler )( 
            IShellItem * This,
            /* [in] */ IBindCtx *pbc,
            /* [in] */ REFGUID rbhid,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvOut);
        
        HRESULT ( STDMETHODCALLTYPE *GetParent )( 
            IShellItem * This,
            /* [out] */ IShellItem **ppsi);
        
        HRESULT ( STDMETHODCALLTYPE *GetDisplayName )( 
            IShellItem * This,
            /* [in] */ SIGDN sigdnName,
            /* [string][out] */ LPOLESTR *ppszName);
        
        HRESULT ( STDMETHODCALLTYPE *GetAttributes )( 
            IShellItem * This,
            /* [in] */ SFGAOF sfgaoMask,
            /* [out] */ SFGAOF *psfgaoAttribs);
        
        HRESULT ( STDMETHODCALLTYPE *Compare )( 
            IShellItem * This,
            /* [in] */ IShellItem *psi,
            /* [in] */ SICHINTF hint,
            /* [out] */ int *piOrder);
        
        END_INTERFACE
    } IShellItemVtbl;

    interface IShellItem
    {
        CONST_VTBL struct IShellItemVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IShellItem_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IShellItem_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IShellItem_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IShellItem_BindToHandler(This,pbc,rbhid,riid,ppvOut)	\
    (This)->lpVtbl -> BindToHandler(This,pbc,rbhid,riid,ppvOut)

#define IShellItem_GetParent(This,ppsi)	\
    (This)->lpVtbl -> GetParent(This,ppsi)

#define IShellItem_GetDisplayName(This,sigdnName,ppszName)	\
    (This)->lpVtbl -> GetDisplayName(This,sigdnName,ppszName)

#define IShellItem_GetAttributes(This,sfgaoMask,psfgaoAttribs)	\
    (This)->lpVtbl -> GetAttributes(This,sfgaoMask,psfgaoAttribs)

#define IShellItem_Compare(This,psi,hint,piOrder)	\
    (This)->lpVtbl -> Compare(This,psi,hint,piOrder)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IShellItem_BindToHandler_Proxy( 
    IShellItem * This,
    /* [in] */ IBindCtx *pbc,
    /* [in] */ REFGUID rbhid,
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ void **ppvOut);


void __RPC_STUB IShellItem_BindToHandler_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellItem_GetParent_Proxy( 
    IShellItem * This,
    /* [out] */ IShellItem **ppsi);


void __RPC_STUB IShellItem_GetParent_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellItem_GetDisplayName_Proxy( 
    IShellItem * This,
    /* [in] */ SIGDN sigdnName,
    /* [string][out] */ LPOLESTR *ppszName);


void __RPC_STUB IShellItem_GetDisplayName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellItem_GetAttributes_Proxy( 
    IShellItem * This,
    /* [in] */ SFGAOF sfgaoMask,
    /* [out] */ SFGAOF *psfgaoAttribs);


void __RPC_STUB IShellItem_GetAttributes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellItem_Compare_Proxy( 
    IShellItem * This,
    /* [in] */ IShellItem *psi,
    /* [in] */ SICHINTF hint,
    /* [out] */ int *piOrder);


void __RPC_STUB IShellItem_Compare_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IShellItem_INTERFACE_DEFINED__ */


#ifndef __IImageRecompress_INTERFACE_DEFINED__
#define __IImageRecompress_INTERFACE_DEFINED__

/* interface IImageRecompress */
/* [object][unique][helpstring][uuid] */ 


EXTERN_C const IID IID_IImageRecompress;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("505f1513-6b3e-4892-a272-59f8889a4d3e")
    IImageRecompress : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE RecompressImage( 
            IShellItem *psi,
            int cx,
            int cy,
            int iQuality,
            IStorage *pstg,
            IStream **ppstrmOut) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IImageRecompressVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IImageRecompress * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IImageRecompress * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IImageRecompress * This);
        
        HRESULT ( STDMETHODCALLTYPE *RecompressImage )( 
            IImageRecompress * This,
            IShellItem *psi,
            int cx,
            int cy,
            int iQuality,
            IStorage *pstg,
            IStream **ppstrmOut);
        
        END_INTERFACE
    } IImageRecompressVtbl;

    interface IImageRecompress
    {
        CONST_VTBL struct IImageRecompressVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IImageRecompress_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IImageRecompress_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IImageRecompress_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IImageRecompress_RecompressImage(This,psi,cx,cy,iQuality,pstg,ppstrmOut)	\
    (This)->lpVtbl -> RecompressImage(This,psi,cx,cy,iQuality,pstg,ppstrmOut)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IImageRecompress_RecompressImage_Proxy( 
    IImageRecompress * This,
    IShellItem *psi,
    int cx,
    int cy,
    int iQuality,
    IStorage *pstg,
    IStream **ppstrmOut);


void __RPC_STUB IImageRecompress_RecompressImage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IImageRecompress_INTERFACE_DEFINED__ */


#ifndef __IDefViewSafety_INTERFACE_DEFINED__
#define __IDefViewSafety_INTERFACE_DEFINED__

/* interface IDefViewSafety */
/* [unique][object][uuid][helpstring] */ 


EXTERN_C const IID IID_IDefViewSafety;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("9A93B3FB-4E75-4c74-871A-2CDA667F39A5")
    IDefViewSafety : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE IsSafePage( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDefViewSafetyVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDefViewSafety * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDefViewSafety * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDefViewSafety * This);
        
        HRESULT ( STDMETHODCALLTYPE *IsSafePage )( 
            IDefViewSafety * This);
        
        END_INTERFACE
    } IDefViewSafetyVtbl;

    interface IDefViewSafety
    {
        CONST_VTBL struct IDefViewSafetyVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDefViewSafety_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDefViewSafety_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDefViewSafety_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDefViewSafety_IsSafePage(This)	\
    (This)->lpVtbl -> IsSafePage(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDefViewSafety_IsSafePage_Proxy( 
    IDefViewSafety * This);


void __RPC_STUB IDefViewSafety_IsSafePage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDefViewSafety_INTERFACE_DEFINED__ */


#ifndef __IContextMenuSite_INTERFACE_DEFINED__
#define __IContextMenuSite_INTERFACE_DEFINED__

/* interface IContextMenuSite */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_IContextMenuSite;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("0811AEBE-0B87-4C54-9E72-548CF649016B")
    IContextMenuSite : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE DoContextMenuPopup( 
            /* [in] */ IUnknown *punkContextMenu,
            /* [in] */ UINT fFlags,
            /* [in] */ POINT pt) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IContextMenuSiteVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IContextMenuSite * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IContextMenuSite * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IContextMenuSite * This);
        
        HRESULT ( STDMETHODCALLTYPE *DoContextMenuPopup )( 
            IContextMenuSite * This,
            /* [in] */ IUnknown *punkContextMenu,
            /* [in] */ UINT fFlags,
            /* [in] */ POINT pt);
        
        END_INTERFACE
    } IContextMenuSiteVtbl;

    interface IContextMenuSite
    {
        CONST_VTBL struct IContextMenuSiteVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IContextMenuSite_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IContextMenuSite_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IContextMenuSite_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IContextMenuSite_DoContextMenuPopup(This,punkContextMenu,fFlags,pt)	\
    (This)->lpVtbl -> DoContextMenuPopup(This,punkContextMenu,fFlags,pt)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IContextMenuSite_DoContextMenuPopup_Proxy( 
    IContextMenuSite * This,
    /* [in] */ IUnknown *punkContextMenu,
    /* [in] */ UINT fFlags,
    /* [in] */ POINT pt);


void __RPC_STUB IContextMenuSite_DoContextMenuPopup_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IContextMenuSite_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0257 */
/* [local] */ 

#include <pshpack1.h>
typedef struct tagDELEGATEITEMID
    {
    WORD cbSize;
    WORD wOuter;
    WORD cbInner;
    BYTE rgb[ 1 ];
    } 	DELEGATEITEMID;

#include <poppack.h>
typedef const UNALIGNED DELEGATEITEMID *PCDELEGATEITEMID;
typedef UNALIGNED DELEGATEITEMID *PDELEGATEITEMID;


extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0257_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0257_v0_0_s_ifspec;

#ifndef __IDelegateFolder_INTERFACE_DEFINED__
#define __IDelegateFolder_INTERFACE_DEFINED__

/* interface IDelegateFolder */
/* [unique][object][uuid][helpstring] */ 


EXTERN_C const IID IID_IDelegateFolder;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("ADD8BA80-002B-11D0-8F0F-00C04FD7D062")
    IDelegateFolder : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetItemAlloc( 
            /* [in] */ IMalloc *pmalloc) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IDelegateFolderVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IDelegateFolder * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IDelegateFolder * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IDelegateFolder * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetItemAlloc )( 
            IDelegateFolder * This,
            /* [in] */ IMalloc *pmalloc);
        
        END_INTERFACE
    } IDelegateFolderVtbl;

    interface IDelegateFolder
    {
        CONST_VTBL struct IDelegateFolderVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IDelegateFolder_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IDelegateFolder_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IDelegateFolder_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IDelegateFolder_SetItemAlloc(This,pmalloc)	\
    (This)->lpVtbl -> SetItemAlloc(This,pmalloc)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IDelegateFolder_SetItemAlloc_Proxy( 
    IDelegateFolder * This,
    /* [in] */ IMalloc *pmalloc);


void __RPC_STUB IDelegateFolder_SetItemAlloc_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IDelegateFolder_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0258 */
/* [local] */ 

// INTERFACE: IBrowserFrameOptions
//
// This interface was implemented so a browser or host can ask a ShellView/ShelNameSpace what
// kind of 'Behavior' is appropriate for that view.
//
//    IBrowserFrameOptions::GetBrowserOptions()
//       dwMask is the logical OR of bits to look for.  pdwOptions is not optional and
//       it's return value will always equal or will be a subset of dwMask.
//       If the function succeeds, the return value must be S_OK and pdwOptions needs to be filled in.
//       If the function fails, pdwOptions needs to be filled in with BFO_NONE.
//


extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0258_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0258_v0_0_s_ifspec;

#ifndef __IBrowserFrameOptions_INTERFACE_DEFINED__
#define __IBrowserFrameOptions_INTERFACE_DEFINED__

/* interface IBrowserFrameOptions */
/* [local][object][uuid] */ 

typedef /* [unique] */ IBrowserFrameOptions *LPBROWSERFRAMEOPTIONS;


enum __MIDL_IBrowserFrameOptions_0001
    {	BFO_NONE	= 0,
	BFO_BROWSER_PERSIST_SETTINGS	= 0x1,
	BFO_RENAME_FOLDER_OPTIONS_TOINTERNET	= 0x2,
	BFO_BOTH_OPTIONS	= 0x4,
	BIF_PREFER_INTERNET_SHORTCUT	= 0x8,
	BFO_BROWSE_NO_IN_NEW_PROCESS	= 0x10,
	BFO_ENABLE_HYPERLINK_TRACKING	= 0x20,
	BFO_USE_IE_OFFLINE_SUPPORT	= 0x40,
	BFO_SUBSTITUE_INTERNET_START_PAGE	= 0x80,
	BFO_USE_IE_LOGOBANDING	= 0x100,
	BFO_ADD_IE_TOCAPTIONBAR	= 0x200,
	BFO_USE_DIALUP_REF	= 0x400,
	BFO_USE_IE_TOOLBAR	= 0x800,
	BFO_NO_PARENT_FOLDER_SUPPORT	= 0x1000,
	BFO_NO_REOPEN_NEXT_RESTART	= 0x2000,
	BFO_GO_HOME_PAGE	= 0x4000,
	BFO_PREFER_IEPROCESS	= 0x8000,
	BFO_SHOW_NAVIGATION_CANCELLED	= 0x10000,
	BFO_USE_IE_STATUSBAR	= 0x20000,
	BFO_QUERY_ALL	= 0xffffffff
    } ;
typedef DWORD BROWSERFRAMEOPTIONS;


EXTERN_C const IID IID_IBrowserFrameOptions;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("10DF43C8-1DBE-11d3-8B34-006097DF5BD4")
    IBrowserFrameOptions : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE GetFrameOptions( 
            /* [in] */ BROWSERFRAMEOPTIONS dwMask,
            /* [out] */ BROWSERFRAMEOPTIONS *pdwOptions) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IBrowserFrameOptionsVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IBrowserFrameOptions * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IBrowserFrameOptions * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IBrowserFrameOptions * This);
        
        HRESULT ( STDMETHODCALLTYPE *GetFrameOptions )( 
            IBrowserFrameOptions * This,
            /* [in] */ BROWSERFRAMEOPTIONS dwMask,
            /* [out] */ BROWSERFRAMEOPTIONS *pdwOptions);
        
        END_INTERFACE
    } IBrowserFrameOptionsVtbl;

    interface IBrowserFrameOptions
    {
        CONST_VTBL struct IBrowserFrameOptionsVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IBrowserFrameOptions_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IBrowserFrameOptions_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IBrowserFrameOptions_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IBrowserFrameOptions_GetFrameOptions(This,dwMask,pdwOptions)	\
    (This)->lpVtbl -> GetFrameOptions(This,dwMask,pdwOptions)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IBrowserFrameOptions_GetFrameOptions_Proxy( 
    IBrowserFrameOptions * This,
    /* [in] */ BROWSERFRAMEOPTIONS dwMask,
    /* [out] */ BROWSERFRAMEOPTIONS *pdwOptions);


void __RPC_STUB IBrowserFrameOptions_GetFrameOptions_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IBrowserFrameOptions_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0259 */
/* [local] */ 

typedef 
enum tagNWMF
    {	NWMF_UNLOADING	= 0x1,
	NWMF_USERINITED	= 0x2,
	NWMF_FIRST	= 0x4,
	NWMF_OVERRIDEKEY	= 0x8,
	NWMF_SHOWHELP	= 0x10,
	NWMF_HTMLDIALOG	= 0x20,
	NWMF_FROMDIALOGCHILD	= 0x40,
	NWMF_USERREQUESTED	= 0x80,
	NWMF_USERALLOWED	= 0x100
    } 	NWMF;

#define SID_SNewWindowManager IID_INewWindowManager


extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0259_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0259_v0_0_s_ifspec;

#ifndef __INewWindowManager_INTERFACE_DEFINED__
#define __INewWindowManager_INTERFACE_DEFINED__

/* interface INewWindowManager */
/* [unique][object][uuid] */ 


EXTERN_C const IID IID_INewWindowManager;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("D2BC4C84-3F72-4a52-A604-7BCBF3982CBB")
    INewWindowManager : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE EvaluateNewWindow( 
            /* [in] */ LPCWSTR pszUrl,
            /* [in] */ LPCWSTR pszName,
            /* [in] */ LPCWSTR pszUrlContext,
            /* [in] */ LPCWSTR pszFeatures,
            /* [in] */ BOOL fReplace,
            /* [in] */ DWORD dwFlags,
            /* [in] */ DWORD dwUserActionTime) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct INewWindowManagerVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            INewWindowManager * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            INewWindowManager * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            INewWindowManager * This);
        
        HRESULT ( STDMETHODCALLTYPE *EvaluateNewWindow )( 
            INewWindowManager * This,
            /* [in] */ LPCWSTR pszUrl,
            /* [in] */ LPCWSTR pszName,
            /* [in] */ LPCWSTR pszUrlContext,
            /* [in] */ LPCWSTR pszFeatures,
            /* [in] */ BOOL fReplace,
            /* [in] */ DWORD dwFlags,
            /* [in] */ DWORD dwUserActionTime);
        
        END_INTERFACE
    } INewWindowManagerVtbl;

    interface INewWindowManager
    {
        CONST_VTBL struct INewWindowManagerVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define INewWindowManager_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define INewWindowManager_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define INewWindowManager_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define INewWindowManager_EvaluateNewWindow(This,pszUrl,pszName,pszUrlContext,pszFeatures,fReplace,dwFlags,dwUserActionTime)	\
    (This)->lpVtbl -> EvaluateNewWindow(This,pszUrl,pszName,pszUrlContext,pszFeatures,fReplace,dwFlags,dwUserActionTime)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE INewWindowManager_EvaluateNewWindow_Proxy( 
    INewWindowManager * This,
    /* [in] */ LPCWSTR pszUrl,
    /* [in] */ LPCWSTR pszName,
    /* [in] */ LPCWSTR pszUrlContext,
    /* [in] */ LPCWSTR pszFeatures,
    /* [in] */ BOOL fReplace,
    /* [in] */ DWORD dwFlags,
    /* [in] */ DWORD dwUserActionTime);


void __RPC_STUB INewWindowManager_EvaluateNewWindow_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __INewWindowManager_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0260 */
/* [local] */ 

#include <pshpack8.h>
typedef struct tagSMDATA
    {
    DWORD dwMask;
    DWORD dwFlags;
    HMENU hmenu;
    HWND hwnd;
    UINT uId;
    UINT uIdParent;
    UINT uIdAncestor;
    IUnknown *punk;
    LPITEMIDLIST pidlFolder;
    LPITEMIDLIST pidlItem;
    IShellFolder *psf;
    void *pvUserData;
    } 	SMDATA;

typedef struct tagSMDATA *LPSMDATA;

// Mask
#define SMDM_SHELLFOLDER               0x00000001  // This is for an item in the band
#define SMDM_HMENU                     0x00000002  // This is for the Band itself
#define SMDM_TOOLBAR                   0x00000004  // Plain toolbar, not associated with a shell folder or hmenu
// Flags (bitmask)
typedef struct tagSMINFO
    {
    DWORD dwMask;
    DWORD dwType;
    DWORD dwFlags;
    int iIcon;
    } 	SMINFO;

typedef struct tagSMINFO *PSMINFO;

typedef struct tagSHCSCHANGENOTIFYSTRUCT
    {
    LONG lEvent;
    LPCITEMIDLIST pidl1;
    LPCITEMIDLIST pidl2;
    } 	SMCSHCHANGENOTIFYSTRUCT;

typedef struct tagSHCSCHANGENOTIFYSTRUCT *PSMCSHCHANGENOTIFYSTRUCT;

#include <poppack.h>

enum __MIDL___MIDL_itf_shobjidl_0260_0001
    {	SMIM_TYPE	= 0x1,
	SMIM_FLAGS	= 0x2,
	SMIM_ICON	= 0x4
    } ;

enum __MIDL___MIDL_itf_shobjidl_0260_0002
    {	SMIT_SEPARATOR	= 0x1,
	SMIT_STRING	= 0x2
    } ;

enum __MIDL___MIDL_itf_shobjidl_0260_0003
    {	SMIF_ICON	= 0x1,
	SMIF_ACCELERATOR	= 0x2,
	SMIF_DROPTARGET	= 0x4,
	SMIF_SUBMENU	= 0x8,
	SMIF_CHECKED	= 0x20,
	SMIF_DROPCASCADE	= 0x40,
	SMIF_HIDDEN	= 0x80,
	SMIF_DISABLED	= 0x100,
	SMIF_TRACKPOPUP	= 0x200,
	SMIF_DEMOTED	= 0x400,
	SMIF_ALTSTATE	= 0x800,
	SMIF_DRAGNDROP	= 0x1000,
	SMIF_NEW	= 0x2000
    } ;
#define SMC_INITMENU            0x00000001  // The callback is called to init a menuband
#define SMC_CREATE              0x00000002
#define SMC_EXITMENU            0x00000003  // The callback is called when menu is collapsing
#define SMC_GETINFO             0x00000005  // The callback is called to return DWORD values
#define SMC_GETSFINFO           0x00000006  // The callback is called to return DWORD values
#define SMC_GETOBJECT           0x00000007  // The callback is called to get some object
#define SMC_GETSFOBJECT         0x00000008  // The callback is called to get some object
#define SMC_SFEXEC              0x00000009  // The callback is called to execute an shell folder item
#define SMC_SFSELECTITEM        0x0000000A  // The callback is called when an item is selected
#define SMC_REFRESH             0x00000010  // Menus have completely refreshed. Reset your state.
#define SMC_DEMOTE              0x00000011  // Demote an item
#define SMC_PROMOTE             0x00000012  // Promote an item, wParam = SMINV_* flag
#define SMC_DEFAULTICON         0x00000016  // Returns Default icon location in wParam, index in lParam
#define SMC_NEWITEM             0x00000017  // Notifies item is not in the order stream.
#define SMC_CHEVRONEXPAND       0x00000019  // Notifies of a expansion via the chevron
#define SMC_DISPLAYCHEVRONTIP   0x0000002A  // S_OK display, S_FALSE not.
#define SMC_SETSFOBJECT         0x0000002D  // Called to save the passed object
#define SMC_SHCHANGENOTIFY      0x0000002E  // Called when a Change notify is received. lParam points to SMCSHCHANGENOTIFYSTRUCT
#define SMC_CHEVRONGETTIP       0x0000002F  // Called to get the chevron tip text. wParam = Tip title, Lparam = TipText Both MAX_PATH
#define SMC_SFDDRESTRICTED      0x00000030  // Called requesting if it's ok to drop. wParam = IDropTarget.


extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0260_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0260_v0_0_s_ifspec;

#ifndef __IShellMenuCallback_INTERFACE_DEFINED__
#define __IShellMenuCallback_INTERFACE_DEFINED__

/* interface IShellMenuCallback */
/* [local][unique][object][uuid] */ 


EXTERN_C const IID IID_IShellMenuCallback;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("4CA300A1-9B8D-11d1-8B22-00C04FD918D0")
    IShellMenuCallback : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE CallbackSM( 
            /* [out][in] */ LPSMDATA psmd,
            UINT uMsg,
            WPARAM wParam,
            LPARAM lParam) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IShellMenuCallbackVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IShellMenuCallback * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IShellMenuCallback * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IShellMenuCallback * This);
        
        HRESULT ( STDMETHODCALLTYPE *CallbackSM )( 
            IShellMenuCallback * This,
            /* [out][in] */ LPSMDATA psmd,
            UINT uMsg,
            WPARAM wParam,
            LPARAM lParam);
        
        END_INTERFACE
    } IShellMenuCallbackVtbl;

    interface IShellMenuCallback
    {
        CONST_VTBL struct IShellMenuCallbackVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IShellMenuCallback_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IShellMenuCallback_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IShellMenuCallback_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IShellMenuCallback_CallbackSM(This,psmd,uMsg,wParam,lParam)	\
    (This)->lpVtbl -> CallbackSM(This,psmd,uMsg,wParam,lParam)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IShellMenuCallback_CallbackSM_Proxy( 
    IShellMenuCallback * This,
    /* [out][in] */ LPSMDATA psmd,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam);


void __RPC_STUB IShellMenuCallback_CallbackSM_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IShellMenuCallback_INTERFACE_DEFINED__ */


#ifndef __IAttachmentExecute_INTERFACE_DEFINED__
#define __IAttachmentExecute_INTERFACE_DEFINED__

/* interface IAttachmentExecute */
/* [unique][local][uuid][object] */ 

typedef 
enum tagATTACHMENT_PROMPT
    {	ATTACHMENT_PROMPT_NONE	= 0,
	ATTACHMENT_PROMPT_SAVE	= 0x1,
	ATTACHMENT_PROMPT_EXEC	= 0x2,
	ATTACHMENT_PROMPT_EXEC_OR_SAVE	= 0x3
    } 	ATTACHMENT_PROMPT;

typedef 
enum tagATTACHMENT_ACTION
    {	ATTACHMENT_ACTION_CANCEL	= 0,
	ATTACHMENT_ACTION_SAVE	= 0x1,
	ATTACHMENT_ACTION_EXEC	= 0x2
    } 	ATTACHMENT_ACTION;


EXTERN_C const IID IID_IAttachmentExecute;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("73db1241-1e85-4581-8e4f-a81e1d0f8c57")
    IAttachmentExecute : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SetClientTitle( 
            /* [string][in] */ LPCWSTR pszTitle) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetClientGuid( 
            /* [in] */ REFGUID guid) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetLocalPath( 
            /* [string][in] */ LPCWSTR pszLocalPath) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetFileName( 
            /* [string][in] */ LPCWSTR pszFileName) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetSource( 
            /* [string][in] */ LPCWSTR pszSource) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetReferrer( 
            /* [string][in] */ LPCWSTR pszReferrer) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CheckPolicy( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Prompt( 
            /* [in] */ HWND hwnd,
            /* [in] */ ATTACHMENT_PROMPT prompt,
            /* [out] */ ATTACHMENT_ACTION *paction) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Save( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Execute( 
            /* [in] */ HWND hwnd,
            /* [string][in] */ LPCWSTR pszVerb,
            HANDLE *phProcess) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SaveWithUI( 
            HWND hwnd) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ClearClientState( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IAttachmentExecuteVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IAttachmentExecute * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IAttachmentExecute * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IAttachmentExecute * This);
        
        HRESULT ( STDMETHODCALLTYPE *SetClientTitle )( 
            IAttachmentExecute * This,
            /* [string][in] */ LPCWSTR pszTitle);
        
        HRESULT ( STDMETHODCALLTYPE *SetClientGuid )( 
            IAttachmentExecute * This,
            /* [in] */ REFGUID guid);
        
        HRESULT ( STDMETHODCALLTYPE *SetLocalPath )( 
            IAttachmentExecute * This,
            /* [string][in] */ LPCWSTR pszLocalPath);
        
        HRESULT ( STDMETHODCALLTYPE *SetFileName )( 
            IAttachmentExecute * This,
            /* [string][in] */ LPCWSTR pszFileName);
        
        HRESULT ( STDMETHODCALLTYPE *SetSource )( 
            IAttachmentExecute * This,
            /* [string][in] */ LPCWSTR pszSource);
        
        HRESULT ( STDMETHODCALLTYPE *SetReferrer )( 
            IAttachmentExecute * This,
            /* [string][in] */ LPCWSTR pszReferrer);
        
        HRESULT ( STDMETHODCALLTYPE *CheckPolicy )( 
            IAttachmentExecute * This);
        
        HRESULT ( STDMETHODCALLTYPE *Prompt )( 
            IAttachmentExecute * This,
            /* [in] */ HWND hwnd,
            /* [in] */ ATTACHMENT_PROMPT prompt,
            /* [out] */ ATTACHMENT_ACTION *paction);
        
        HRESULT ( STDMETHODCALLTYPE *Save )( 
            IAttachmentExecute * This);
        
        HRESULT ( STDMETHODCALLTYPE *Execute )( 
            IAttachmentExecute * This,
            /* [in] */ HWND hwnd,
            /* [string][in] */ LPCWSTR pszVerb,
            HANDLE *phProcess);
        
        HRESULT ( STDMETHODCALLTYPE *SaveWithUI )( 
            IAttachmentExecute * This,
            HWND hwnd);
        
        HRESULT ( STDMETHODCALLTYPE *ClearClientState )( 
            IAttachmentExecute * This);
        
        END_INTERFACE
    } IAttachmentExecuteVtbl;

    interface IAttachmentExecute
    {
        CONST_VTBL struct IAttachmentExecuteVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IAttachmentExecute_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IAttachmentExecute_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IAttachmentExecute_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IAttachmentExecute_SetClientTitle(This,pszTitle)	\
    (This)->lpVtbl -> SetClientTitle(This,pszTitle)

#define IAttachmentExecute_SetClientGuid(This,guid)	\
    (This)->lpVtbl -> SetClientGuid(This,guid)

#define IAttachmentExecute_SetLocalPath(This,pszLocalPath)	\
    (This)->lpVtbl -> SetLocalPath(This,pszLocalPath)

#define IAttachmentExecute_SetFileName(This,pszFileName)	\
    (This)->lpVtbl -> SetFileName(This,pszFileName)

#define IAttachmentExecute_SetSource(This,pszSource)	\
    (This)->lpVtbl -> SetSource(This,pszSource)

#define IAttachmentExecute_SetReferrer(This,pszReferrer)	\
    (This)->lpVtbl -> SetReferrer(This,pszReferrer)

#define IAttachmentExecute_CheckPolicy(This)	\
    (This)->lpVtbl -> CheckPolicy(This)

#define IAttachmentExecute_Prompt(This,hwnd,prompt,paction)	\
    (This)->lpVtbl -> Prompt(This,hwnd,prompt,paction)

#define IAttachmentExecute_Save(This)	\
    (This)->lpVtbl -> Save(This)

#define IAttachmentExecute_Execute(This,hwnd,pszVerb,phProcess)	\
    (This)->lpVtbl -> Execute(This,hwnd,pszVerb,phProcess)

#define IAttachmentExecute_SaveWithUI(This,hwnd)	\
    (This)->lpVtbl -> SaveWithUI(This,hwnd)

#define IAttachmentExecute_ClearClientState(This)	\
    (This)->lpVtbl -> ClearClientState(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IAttachmentExecute_SetClientTitle_Proxy( 
    IAttachmentExecute * This,
    /* [string][in] */ LPCWSTR pszTitle);


void __RPC_STUB IAttachmentExecute_SetClientTitle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IAttachmentExecute_SetClientGuid_Proxy( 
    IAttachmentExecute * This,
    /* [in] */ REFGUID guid);


void __RPC_STUB IAttachmentExecute_SetClientGuid_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IAttachmentExecute_SetLocalPath_Proxy( 
    IAttachmentExecute * This,
    /* [string][in] */ LPCWSTR pszLocalPath);


void __RPC_STUB IAttachmentExecute_SetLocalPath_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IAttachmentExecute_SetFileName_Proxy( 
    IAttachmentExecute * This,
    /* [string][in] */ LPCWSTR pszFileName);


void __RPC_STUB IAttachmentExecute_SetFileName_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IAttachmentExecute_SetSource_Proxy( 
    IAttachmentExecute * This,
    /* [string][in] */ LPCWSTR pszSource);


void __RPC_STUB IAttachmentExecute_SetSource_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IAttachmentExecute_SetReferrer_Proxy( 
    IAttachmentExecute * This,
    /* [string][in] */ LPCWSTR pszReferrer);


void __RPC_STUB IAttachmentExecute_SetReferrer_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IAttachmentExecute_CheckPolicy_Proxy( 
    IAttachmentExecute * This);


void __RPC_STUB IAttachmentExecute_CheckPolicy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IAttachmentExecute_Prompt_Proxy( 
    IAttachmentExecute * This,
    /* [in] */ HWND hwnd,
    /* [in] */ ATTACHMENT_PROMPT prompt,
    /* [out] */ ATTACHMENT_ACTION *paction);


void __RPC_STUB IAttachmentExecute_Prompt_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IAttachmentExecute_Save_Proxy( 
    IAttachmentExecute * This);


void __RPC_STUB IAttachmentExecute_Save_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IAttachmentExecute_Execute_Proxy( 
    IAttachmentExecute * This,
    /* [in] */ HWND hwnd,
    /* [string][in] */ LPCWSTR pszVerb,
    HANDLE *phProcess);


void __RPC_STUB IAttachmentExecute_Execute_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IAttachmentExecute_SaveWithUI_Proxy( 
    IAttachmentExecute * This,
    HWND hwnd);


void __RPC_STUB IAttachmentExecute_SaveWithUI_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IAttachmentExecute_ClearClientState_Proxy( 
    IAttachmentExecute * This);


void __RPC_STUB IAttachmentExecute_ClearClientState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IAttachmentExecute_INTERFACE_DEFINED__ */


/* interface __MIDL_itf_shobjidl_0262 */
/* [local] */ 

#define SMINIT_DEFAULT              0x00000000  // No Options
#define SMINIT_RESTRICT_DRAGDROP    0x00000002  // Don't allow Drag and Drop
#define SMINIT_TOPLEVEL             0x00000004  // This is the top band.
#define SMINIT_CACHED               0x00000010
#define SMINIT_VERTICAL             0x10000000  // This is a vertical menu
#define SMINIT_HORIZONTAL           0x20000000  // This is a horizontal menu    (does not inherit)
#define ANCESTORDEFAULT      (UINT)-1
#define SMSET_TOP                   0x10000000    // Bias this namespace to the top of the menu
#define SMSET_BOTTOM                0x20000000    // Bias this namespace to the bottom of the menu
#define SMSET_DONTOWN               0x00000001    // The Menuband doesn't own the non-ref counted object
#define SMINV_REFRESH        0x00000001
#define SMINV_ID             0x00000008


extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0262_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0262_v0_0_s_ifspec;

#ifndef __IShellMenu_INTERFACE_DEFINED__
#define __IShellMenu_INTERFACE_DEFINED__

/* interface IShellMenu */
/* [local][unique][object][uuid] */ 


EXTERN_C const IID IID_IShellMenu;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("EE1F7637-E138-11d1-8379-00C04FD918D0")
    IShellMenu : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Initialize( 
            /* [in] */ IShellMenuCallback *psmc,
            UINT uId,
            UINT uIdAncestor,
            DWORD dwFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMenuInfo( 
            /* [out] */ IShellMenuCallback **ppsmc,
            /* [out] */ UINT *puId,
            /* [out] */ UINT *puIdAncestor,
            /* [out] */ DWORD *pdwFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetShellFolder( 
            IShellFolder *psf,
            /* [in] */ LPCITEMIDLIST pidlFolder,
            HKEY hKey,
            DWORD dwFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetShellFolder( 
            /* [out] */ DWORD *pdwFlags,
            /* [out] */ LPITEMIDLIST *ppidl,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppv) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetMenu( 
            /* [in] */ HMENU hmenu,
            /* [in] */ HWND hwnd,
            DWORD dwFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetMenu( 
            /* [out] */ HMENU *phmenu,
            /* [out] */ HWND *phwnd,
            /* [out] */ DWORD *pdwFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE InvalidateItem( 
            /* [in] */ LPSMDATA psmd,
            DWORD dwFlags) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE GetState( 
            /* [out] */ LPSMDATA psmd) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetMenuToolbar( 
            /* [in] */ IUnknown *punk,
            DWORD dwFlags) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IShellMenuVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            IShellMenu * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppvObject);
        
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            IShellMenu * This);
        
        ULONG ( STDMETHODCALLTYPE *Release )( 
            IShellMenu * This);
        
        HRESULT ( STDMETHODCALLTYPE *Initialize )( 
            IShellMenu * This,
            /* [in] */ IShellMenuCallback *psmc,
            UINT uId,
            UINT uIdAncestor,
            DWORD dwFlags);
        
        HRESULT ( STDMETHODCALLTYPE *GetMenuInfo )( 
            IShellMenu * This,
            /* [out] */ IShellMenuCallback **ppsmc,
            /* [out] */ UINT *puId,
            /* [out] */ UINT *puIdAncestor,
            /* [out] */ DWORD *pdwFlags);
        
        HRESULT ( STDMETHODCALLTYPE *SetShellFolder )( 
            IShellMenu * This,
            IShellFolder *psf,
            /* [in] */ LPCITEMIDLIST pidlFolder,
            HKEY hKey,
            DWORD dwFlags);
        
        HRESULT ( STDMETHODCALLTYPE *GetShellFolder )( 
            IShellMenu * This,
            /* [out] */ DWORD *pdwFlags,
            /* [out] */ LPITEMIDLIST *ppidl,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void **ppv);
        
        HRESULT ( STDMETHODCALLTYPE *SetMenu )( 
            IShellMenu * This,
            /* [in] */ HMENU hmenu,
            /* [in] */ HWND hwnd,
            DWORD dwFlags);
        
        HRESULT ( STDMETHODCALLTYPE *GetMenu )( 
            IShellMenu * This,
            /* [out] */ HMENU *phmenu,
            /* [out] */ HWND *phwnd,
            /* [out] */ DWORD *pdwFlags);
        
        HRESULT ( STDMETHODCALLTYPE *InvalidateItem )( 
            IShellMenu * This,
            /* [in] */ LPSMDATA psmd,
            DWORD dwFlags);
        
        HRESULT ( STDMETHODCALLTYPE *GetState )( 
            IShellMenu * This,
            /* [out] */ LPSMDATA psmd);
        
        HRESULT ( STDMETHODCALLTYPE *SetMenuToolbar )( 
            IShellMenu * This,
            /* [in] */ IUnknown *punk,
            DWORD dwFlags);
        
        END_INTERFACE
    } IShellMenuVtbl;

    interface IShellMenu
    {
        CONST_VTBL struct IShellMenuVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IShellMenu_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IShellMenu_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IShellMenu_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IShellMenu_Initialize(This,psmc,uId,uIdAncestor,dwFlags)	\
    (This)->lpVtbl -> Initialize(This,psmc,uId,uIdAncestor,dwFlags)

#define IShellMenu_GetMenuInfo(This,ppsmc,puId,puIdAncestor,pdwFlags)	\
    (This)->lpVtbl -> GetMenuInfo(This,ppsmc,puId,puIdAncestor,pdwFlags)

#define IShellMenu_SetShellFolder(This,psf,pidlFolder,hKey,dwFlags)	\
    (This)->lpVtbl -> SetShellFolder(This,psf,pidlFolder,hKey,dwFlags)

#define IShellMenu_GetShellFolder(This,pdwFlags,ppidl,riid,ppv)	\
    (This)->lpVtbl -> GetShellFolder(This,pdwFlags,ppidl,riid,ppv)

#define IShellMenu_SetMenu(This,hmenu,hwnd,dwFlags)	\
    (This)->lpVtbl -> SetMenu(This,hmenu,hwnd,dwFlags)

#define IShellMenu_GetMenu(This,phmenu,phwnd,pdwFlags)	\
    (This)->lpVtbl -> GetMenu(This,phmenu,phwnd,pdwFlags)

#define IShellMenu_InvalidateItem(This,psmd,dwFlags)	\
    (This)->lpVtbl -> InvalidateItem(This,psmd,dwFlags)

#define IShellMenu_GetState(This,psmd)	\
    (This)->lpVtbl -> GetState(This,psmd)

#define IShellMenu_SetMenuToolbar(This,punk,dwFlags)	\
    (This)->lpVtbl -> SetMenuToolbar(This,punk,dwFlags)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE IShellMenu_Initialize_Proxy( 
    IShellMenu * This,
    /* [in] */ IShellMenuCallback *psmc,
    UINT uId,
    UINT uIdAncestor,
    DWORD dwFlags);


void __RPC_STUB IShellMenu_Initialize_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellMenu_GetMenuInfo_Proxy( 
    IShellMenu * This,
    /* [out] */ IShellMenuCallback **ppsmc,
    /* [out] */ UINT *puId,
    /* [out] */ UINT *puIdAncestor,
    /* [out] */ DWORD *pdwFlags);


void __RPC_STUB IShellMenu_GetMenuInfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellMenu_SetShellFolder_Proxy( 
    IShellMenu * This,
    IShellFolder *psf,
    /* [in] */ LPCITEMIDLIST pidlFolder,
    HKEY hKey,
    DWORD dwFlags);


void __RPC_STUB IShellMenu_SetShellFolder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellMenu_GetShellFolder_Proxy( 
    IShellMenu * This,
    /* [out] */ DWORD *pdwFlags,
    /* [out] */ LPITEMIDLIST *ppidl,
    /* [in] */ REFIID riid,
    /* [iid_is][out] */ void **ppv);


void __RPC_STUB IShellMenu_GetShellFolder_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellMenu_SetMenu_Proxy( 
    IShellMenu * This,
    /* [in] */ HMENU hmenu,
    /* [in] */ HWND hwnd,
    DWORD dwFlags);


void __RPC_STUB IShellMenu_SetMenu_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellMenu_GetMenu_Proxy( 
    IShellMenu * This,
    /* [out] */ HMENU *phmenu,
    /* [out] */ HWND *phwnd,
    /* [out] */ DWORD *pdwFlags);


void __RPC_STUB IShellMenu_GetMenu_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellMenu_InvalidateItem_Proxy( 
    IShellMenu * This,
    /* [in] */ LPSMDATA psmd,
    DWORD dwFlags);


void __RPC_STUB IShellMenu_InvalidateItem_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellMenu_GetState_Proxy( 
    IShellMenu * This,
    /* [out] */ LPSMDATA psmd);


void __RPC_STUB IShellMenu_GetState_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE IShellMenu_SetMenuToolbar_Proxy( 
    IShellMenu * This,
    /* [in] */ IUnknown *punk,
    DWORD dwFlags);


void __RPC_STUB IShellMenu_SetMenuToolbar_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IShellMenu_INTERFACE_DEFINED__ */



#ifndef __ShellObjects_LIBRARY_DEFINED__
#define __ShellObjects_LIBRARY_DEFINED__

/* library ShellObjects */
/* [version][lcid][helpstring][uuid] */ 

#define SID_PublishingWizard CLSID_PublishingWizard

EXTERN_C const IID LIBID_ShellObjects;

EXTERN_C const CLSID CLSID_QueryCancelAutoPlay;

#ifdef __cplusplus

class DECLSPEC_UUID("331F1768-05A9-4ddd-B86E-DAE34DDC998A")
QueryCancelAutoPlay;
#endif

EXTERN_C const CLSID CLSID_DriveSizeCategorizer;

#ifdef __cplusplus

class DECLSPEC_UUID("94357B53-CA29-4b78-83AE-E8FE7409134F")
DriveSizeCategorizer;
#endif

EXTERN_C const CLSID CLSID_DriveTypeCategorizer;

#ifdef __cplusplus

class DECLSPEC_UUID("B0A8F3CF-4333-4bab-8873-1CCB1CADA48B")
DriveTypeCategorizer;
#endif

EXTERN_C const CLSID CLSID_FreeSpaceCategorizer;

#ifdef __cplusplus

class DECLSPEC_UUID("B5607793-24AC-44c7-82E2-831726AA6CB7")
FreeSpaceCategorizer;
#endif

EXTERN_C const CLSID CLSID_TimeCategorizer;

#ifdef __cplusplus

class DECLSPEC_UUID("3bb4118f-ddfd-4d30-a348-9fb5d6bf1afe")
TimeCategorizer;
#endif

EXTERN_C const CLSID CLSID_SizeCategorizer;

#ifdef __cplusplus

class DECLSPEC_UUID("55d7b852-f6d1-42f2-aa75-8728a1b2d264")
SizeCategorizer;
#endif

EXTERN_C const CLSID CLSID_AlphabeticalCategorizer;

#ifdef __cplusplus

class DECLSPEC_UUID("3c2654c6-7372-4f6b-b310-55d6128f49d2")
AlphabeticalCategorizer;
#endif

EXTERN_C const CLSID CLSID_MergedCategorizer;

#ifdef __cplusplus

class DECLSPEC_UUID("8e827c11-33e7-4bc1-b242-8cd9a1c2b304")
MergedCategorizer;
#endif

EXTERN_C const CLSID CLSID_ImageProperties;

#ifdef __cplusplus

class DECLSPEC_UUID("7ab770c7-0e23-4d7a-8aa2-19bfad479829")
ImageProperties;
#endif

EXTERN_C const CLSID CLSID_PropertiesUI;

#ifdef __cplusplus

class DECLSPEC_UUID("d912f8cf-0396-4915-884e-fb425d32943b")
PropertiesUI;
#endif

EXTERN_C const CLSID CLSID_UserNotification;

#ifdef __cplusplus

class DECLSPEC_UUID("0010890e-8789-413c-adbc-48f5b511b3af")
UserNotification;
#endif

EXTERN_C const CLSID CLSID_UserEventTimerCallback;

#ifdef __cplusplus

class DECLSPEC_UUID("15fffd13-5140-41b8-b89a-c8d5759cd2b2")
UserEventTimerCallback;
#endif

EXTERN_C const CLSID CLSID_UserEventTimer;

#ifdef __cplusplus

class DECLSPEC_UUID("864A1288-354C-4D19-9D68-C2742BB14997")
UserEventTimer;
#endif

EXTERN_C const CLSID CLSID_NetCrawler;

#ifdef __cplusplus

class DECLSPEC_UUID("601ac3dc-786a-4eb0-bf40-ee3521e70bfb")
NetCrawler;
#endif

EXTERN_C const CLSID CLSID_CDBurn;

#ifdef __cplusplus

class DECLSPEC_UUID("fbeb8a05-beee-4442-804e-409d6c4515e9")
CDBurn;
#endif

EXTERN_C const CLSID CLSID_TaskbarList;

#ifdef __cplusplus

class DECLSPEC_UUID("56FDF344-FD6D-11d0-958A-006097C9A090")
TaskbarList;
#endif

EXTERN_C const CLSID CLSID_WebWizardHost;

#ifdef __cplusplus

class DECLSPEC_UUID("c827f149-55c1-4d28-935e-57e47caed973")
WebWizardHost;
#endif

EXTERN_C const CLSID CLSID_PublishDropTarget;

#ifdef __cplusplus

class DECLSPEC_UUID("CC6EEFFB-43F6-46c5-9619-51D571967F7D")
PublishDropTarget;
#endif

EXTERN_C const CLSID CLSID_PublishingWizard;

#ifdef __cplusplus

class DECLSPEC_UUID("6b33163c-76a5-4b6c-bf21-45de9cd503a1")
PublishingWizard;
#endif

EXTERN_C const CLSID CLSID_InternetPrintOrdering;

#ifdef __cplusplus

class DECLSPEC_UUID("add36aa8-751a-4579-a266-d66f5202ccbb")
InternetPrintOrdering;
#endif

EXTERN_C const CLSID CLSID_FolderViewHost;

#ifdef __cplusplus

class DECLSPEC_UUID("20b1cb23-6968-4eb9-b7d4-a66d00d07cee")
FolderViewHost;
#endif

EXTERN_C const CLSID CLSID_NamespaceWalker;

#ifdef __cplusplus

class DECLSPEC_UUID("72eb61e0-8672-4303-9175-f2e4c68b2e7c")
NamespaceWalker;
#endif

EXTERN_C const CLSID CLSID_ImageRecompress;

#ifdef __cplusplus

class DECLSPEC_UUID("6e33091c-d2f8-4740-b55e-2e11d1477a2c")
ImageRecompress;
#endif

EXTERN_C const CLSID CLSID_TrayBandSiteService;

#ifdef __cplusplus

class DECLSPEC_UUID("F60AD0A0-E5E1-45cb-B51A-E15B9F8B2934")
TrayBandSiteService;
#endif

EXTERN_C const CLSID CLSID_PassportWizard;

#ifdef __cplusplus

class DECLSPEC_UUID("58f1f272-9240-4f51-b6d4-fd63d1618591")
PassportWizard;
#endif

EXTERN_C const CLSID CLSID_AttachmentServices;

#ifdef __cplusplus

class DECLSPEC_UUID("4125dd96-e03a-4103-8f70-e0597d803b9c")
AttachmentServices;
#endif
#endif /* __ShellObjects_LIBRARY_DEFINED__ */

/* interface __MIDL_itf_shobjidl_0263 */
/* [local] */ 


extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0263_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_shobjidl_0263_v0_0_s_ifspec;

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  BSTR_UserSize(     unsigned long *, unsigned long            , BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserMarshal(  unsigned long *, unsigned char *, BSTR * ); 
unsigned char * __RPC_USER  BSTR_UserUnmarshal(unsigned long *, unsigned char *, BSTR * ); 
void                      __RPC_USER  BSTR_UserFree(     unsigned long *, BSTR * ); 

unsigned long             __RPC_USER  HBITMAP_UserSize(     unsigned long *, unsigned long            , HBITMAP * ); 
unsigned char * __RPC_USER  HBITMAP_UserMarshal(  unsigned long *, unsigned char *, HBITMAP * ); 
unsigned char * __RPC_USER  HBITMAP_UserUnmarshal(unsigned long *, unsigned char *, HBITMAP * ); 
void                      __RPC_USER  HBITMAP_UserFree(     unsigned long *, HBITMAP * ); 

unsigned long             __RPC_USER  HGLOBAL_UserSize(     unsigned long *, unsigned long            , HGLOBAL * ); 
unsigned char * __RPC_USER  HGLOBAL_UserMarshal(  unsigned long *, unsigned char *, HGLOBAL * ); 
unsigned char * __RPC_USER  HGLOBAL_UserUnmarshal(unsigned long *, unsigned char *, HGLOBAL * ); 
void                      __RPC_USER  HGLOBAL_UserFree(     unsigned long *, HGLOBAL * ); 

unsigned long             __RPC_USER  HICON_UserSize(     unsigned long *, unsigned long            , HICON * ); 
unsigned char * __RPC_USER  HICON_UserMarshal(  unsigned long *, unsigned char *, HICON * ); 
unsigned char * __RPC_USER  HICON_UserUnmarshal(unsigned long *, unsigned char *, HICON * ); 
void                      __RPC_USER  HICON_UserFree(     unsigned long *, HICON * ); 

unsigned long             __RPC_USER  HMENU_UserSize(     unsigned long *, unsigned long            , HMENU * ); 
unsigned char * __RPC_USER  HMENU_UserMarshal(  unsigned long *, unsigned char *, HMENU * ); 
unsigned char * __RPC_USER  HMENU_UserUnmarshal(unsigned long *, unsigned char *, HMENU * ); 
void                      __RPC_USER  HMENU_UserFree(     unsigned long *, HMENU * ); 

unsigned long             __RPC_USER  HWND_UserSize(     unsigned long *, unsigned long            , HWND * ); 
unsigned char * __RPC_USER  HWND_UserMarshal(  unsigned long *, unsigned char *, HWND * ); 
unsigned char * __RPC_USER  HWND_UserUnmarshal(unsigned long *, unsigned char *, HWND * ); 
void                      __RPC_USER  HWND_UserFree(     unsigned long *, HWND * ); 

unsigned long             __RPC_USER  LPCITEMIDLIST_UserSize(     unsigned long *, unsigned long            , LPCITEMIDLIST * ); 
unsigned char * __RPC_USER  LPCITEMIDLIST_UserMarshal(  unsigned long *, unsigned char *, LPCITEMIDLIST * ); 
unsigned char * __RPC_USER  LPCITEMIDLIST_UserUnmarshal(unsigned long *, unsigned char *, LPCITEMIDLIST * ); 
void                      __RPC_USER  LPCITEMIDLIST_UserFree(     unsigned long *, LPCITEMIDLIST * ); 

unsigned long             __RPC_USER  LPITEMIDLIST_UserSize(     unsigned long *, unsigned long            , LPITEMIDLIST * ); 
unsigned char * __RPC_USER  LPITEMIDLIST_UserMarshal(  unsigned long *, unsigned char *, LPITEMIDLIST * ); 
unsigned char * __RPC_USER  LPITEMIDLIST_UserUnmarshal(unsigned long *, unsigned char *, LPITEMIDLIST * ); 
void                      __RPC_USER  LPITEMIDLIST_UserFree(     unsigned long *, LPITEMIDLIST * ); 

unsigned long             __RPC_USER  LPSAFEARRAY_UserSize(     unsigned long *, unsigned long            , LPSAFEARRAY * ); 
unsigned char * __RPC_USER  LPSAFEARRAY_UserMarshal(  unsigned long *, unsigned char *, LPSAFEARRAY * ); 
unsigned char * __RPC_USER  LPSAFEARRAY_UserUnmarshal(unsigned long *, unsigned char *, LPSAFEARRAY * ); 
void                      __RPC_USER  LPSAFEARRAY_UserFree(     unsigned long *, LPSAFEARRAY * ); 

unsigned long             __RPC_USER  VARIANT_UserSize(     unsigned long *, unsigned long            , VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserMarshal(  unsigned long *, unsigned char *, VARIANT * ); 
unsigned char * __RPC_USER  VARIANT_UserUnmarshal(unsigned long *, unsigned char *, VARIANT * ); 
void                      __RPC_USER  VARIANT_UserFree(     unsigned long *, VARIANT * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif




