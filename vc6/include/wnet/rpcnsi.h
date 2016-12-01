/*++

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    rpcnsi.h

Abstract:

    This file contains the types and function definitions to use the
    Name Service Independent APIs.

--*/

#ifndef __RPCNSI_H__
#define __RPCNSI_H__

#if _MSC_VER > 1000
#pragma once
#endif

typedef void __RPC_FAR * RPC_NS_HANDLE;

#define RPC_C_NS_SYNTAX_DEFAULT 0
#define RPC_C_NS_SYNTAX_DCE 3

#define RPC_C_PROFILE_DEFAULT_ELT 0
#define RPC_C_PROFILE_ALL_ELT 1
#define RPC_C_PROFILE_ALL_ELTS RPC_C_PROFILE_ALL_ELT
#define RPC_C_PROFILE_MATCH_BY_IF 2
#define RPC_C_PROFILE_MATCH_BY_MBR 3
#define RPC_C_PROFILE_MATCH_BY_BOTH 4

#define RPC_C_NS_DEFAULT_EXP_AGE -1

/* Server APIs */

RPCNSAPI RPC_STATUS RPC_ENTRY
RpcNsBindingExportA(
    __in unsigned long EntryNameSyntax,
    __in_opt RPC_CSTR EntryName,
    __in_opt RPC_IF_HANDLE IfSpec,
    __in_opt RPC_BINDING_VECTOR __RPC_FAR *BindingVec,
    __in_opt UUID_VECTOR __RPC_FAR *ObjectUuidVec
    );


RPCNSAPI RPC_STATUS RPC_ENTRY
RpcNsBindingUnexportA(
    __in unsigned long EntryNameSyntax,
    __in_opt RPC_CSTR EntryName,
    __in_opt RPC_IF_HANDLE IfSpec,
    __in_opt UUID_VECTOR __RPC_FAR *ObjectUuidVec
    );

#ifdef RPC_UNICODE_SUPPORTED

RPCNSAPI RPC_STATUS RPC_ENTRY
RpcNsBindingExportW(
    __in unsigned long EntryNameSyntax,
    __in_opt RPC_WSTR EntryName,
    __in_opt RPC_IF_HANDLE IfSpec,
    __in_opt RPC_BINDING_VECTOR __RPC_FAR *BindingVec,
    __in_opt UUID_VECTOR __RPC_FAR *ObjectUuidVec
    );

RPCNSAPI RPC_STATUS RPC_ENTRY
RpcNsBindingUnexportW(
    __in unsigned long EntryNameSyntax,
    __in_opt RPC_WSTR EntryName,
    __in_opt RPC_IF_HANDLE IfSpec,
    __in_opt UUID_VECTOR __RPC_FAR *ObjectUuidVec
    );

#endif

/* Server PnP APIs */

RPC_STATUS RPC_ENTRY
RpcNsBindingExportPnPA(
    __in unsigned long EntryNameSyntax,
    __in_opt RPC_CSTR EntryName,
    __in_opt RPC_IF_HANDLE IfSpec,
    __in_opt UUID_VECTOR *ObjectVector
    );

RPC_STATUS RPC_ENTRY
RpcNsBindingUnexportPnPA(
    __in unsigned long EntryNameSyntax,
    __in_opt RPC_CSTR EntryName,
    __in_opt RPC_IF_HANDLE IfSpec,
    __in_opt UUID_VECTOR *ObjectVector
    );

#ifdef RPC_UNICODE_SUPPORTED

RPC_STATUS RPC_ENTRY
RpcNsBindingExportPnPW(
    __in unsigned long EntryNameSyntax,
    __in_opt RPC_WSTR EntryName,
    __in_opt RPC_IF_HANDLE IfSpec,
    __in_opt UUID_VECTOR *ObjectVector
    );

RPC_STATUS RPC_ENTRY
RpcNsBindingUnexportPnPW(
    __in unsigned long EntryNameSyntax,
    __in_opt RPC_WSTR EntryName,
    __in_opt RPC_IF_HANDLE IfSpec,
    __in_opt UUID_VECTOR *ObjectVector
    );

#endif

/* Client APIs */

RPCNSAPI RPC_STATUS RPC_ENTRY
RpcNsBindingLookupBeginA(
    __in unsigned long EntryNameSyntax,
    __in_opt RPC_CSTR EntryName,
    __in_opt RPC_IF_HANDLE IfSpec,
    __in_opt UUID __RPC_FAR *ObjUuid,
    __in unsigned long BindingMaxCount,
    __out RPC_NS_HANDLE __RPC_FAR *LookupContext
    );

#ifdef RPC_UNICODE_SUPPORTED

RPCNSAPI RPC_STATUS RPC_ENTRY
RpcNsBindingLookupBeginW(
    __in unsigned long EntryNameSyntax,
    __in_opt RPC_WSTR EntryName,
    __in_opt RPC_IF_HANDLE IfSpec,
    __in_opt UUID __RPC_FAR *ObjUuid,
    __in unsigned long BindingMaxCount,
    __out RPC_NS_HANDLE __RPC_FAR *LookupContext
    );
#endif

RPCNSAPI RPC_STATUS RPC_ENTRY
RpcNsBindingLookupNext(
    IN  RPC_NS_HANDLE LookupContext,
    OUT RPC_BINDING_VECTOR __RPC_FAR * __RPC_FAR * BindingVec
    );

RPCNSAPI RPC_STATUS RPC_ENTRY
RpcNsBindingLookupDone(
    IN OUT RPC_NS_HANDLE __RPC_FAR * LookupContext
    );

/* Group APIs */

RPCNSAPI RPC_STATUS RPC_ENTRY
RpcNsGroupDeleteA(
    __in unsigned long GroupNameSyntax,
    __in_opt RPC_CSTR GroupName
    );

RPCNSAPI RPC_STATUS RPC_ENTRY
RpcNsGroupMbrAddA(
    __in unsigned long GroupNameSyntax,
    __in RPC_CSTR GroupName,
    __in unsigned long MemberNameSyntax,
    __in RPC_CSTR MemberName
    );

RPCNSAPI RPC_STATUS RPC_ENTRY
RpcNsGroupMbrRemoveA(
    __in unsigned long GroupNameSyntax,
    __in RPC_CSTR GroupName,
    __in unsigned long MemberNameSyntax,
    __in RPC_CSTR MemberName
    );

RPCNSAPI RPC_STATUS RPC_ENTRY
RpcNsGroupMbrInqBeginA(
    __in unsigned long GroupNameSyntax,
    __in RPC_CSTR GroupName,
    __in unsigned long MemberNameSyntax,
    __out RPC_NS_HANDLE __RPC_FAR *InquiryContext
    );

RPCNSAPI RPC_STATUS RPC_ENTRY
RpcNsGroupMbrInqNextA(
    __inout RPC_NS_HANDLE InquiryContext,
    __deref_out RPC_CSTR __RPC_FAR *MemberName
    );

#ifdef RPC_UNICODE_SUPPORTED

RPCNSAPI RPC_STATUS RPC_ENTRY
RpcNsGroupDeleteW(
    __in unsigned long GroupNameSyntax,
    __in_opt RPC_WSTR GroupName
    );

RPCNSAPI RPC_STATUS RPC_ENTRY
RpcNsGroupMbrAddW(
    __in unsigned long GroupNameSyntax,
    __in RPC_WSTR GroupName,
    __in unsigned long MemberNameSyntax,
    __in RPC_WSTR MemberName
    );

RPCNSAPI RPC_STATUS RPC_ENTRY
RpcNsGroupMbrRemoveW(
    __in unsigned long GroupNameSyntax,
    __in RPC_WSTR GroupName,
    __in unsigned long MemberNameSyntax,
    __in RPC_WSTR MemberName
    );

RPCNSAPI RPC_STATUS RPC_ENTRY
RpcNsGroupMbrInqBeginW(
    __in unsigned long GroupNameSyntax,
    __in RPC_WSTR GroupName,
    __in unsigned long MemberNameSyntax,
    __out RPC_NS_HANDLE __RPC_FAR *InquiryContext
    );

RPCNSAPI RPC_STATUS RPC_ENTRY
RpcNsGroupMbrInqNextW(
    __inout RPC_NS_HANDLE InquiryContext,
    __deref_out RPC_WSTR __RPC_FAR *MemberName
    );

#endif

RPCNSAPI RPC_STATUS RPC_ENTRY
RpcNsGroupMbrInqDone(
    IN OUT RPC_NS_HANDLE __RPC_FAR * InquiryContext
    );

/* Profile APIs */

RPCNSAPI RPC_STATUS RPC_ENTRY
RpcNsProfileDeleteA(
    __in unsigned long ProfileNameSyntax,
    __in RPC_CSTR ProfileName
    );

RPCNSAPI RPC_STATUS RPC_ENTRY
RpcNsProfileEltAddA(
    __in unsigned long ProfileNameSyntax,
    __in RPC_CSTR ProfileName,
    __in_opt RPC_IF_ID __RPC_FAR *IfId,
    __in unsigned long MemberNameSyntax,
    __in RPC_CSTR MemberName,
    __in unsigned long Priority,
    __in_opt RPC_CSTR Annotation
    );

RPCNSAPI RPC_STATUS RPC_ENTRY
RpcNsProfileEltRemoveA(
    __in unsigned long ProfileNameSyntax,
    __in RPC_CSTR ProfileName,
    __in_opt RPC_IF_ID __RPC_FAR *IfId,
    __in unsigned long MemberNameSyntax,
    __in RPC_CSTR MemberName
    );

RPCNSAPI RPC_STATUS RPC_ENTRY
RpcNsProfileEltInqBeginA(
    __in unsigned long ProfileNameSyntax,
    __in RPC_CSTR ProfileName,
    __in unsigned long InquiryType,
    __in_opt RPC_IF_ID __RPC_FAR *IfId,
    __in unsigned long VersOption,
    __in unsigned long MemberNameSyntax,
    __in_opt RPC_CSTR MemberName,
    __out RPC_NS_HANDLE __RPC_FAR *InquiryContext
    );

RPCNSAPI RPC_STATUS RPC_ENTRY
RpcNsProfileEltInqNextA(
    __in RPC_NS_HANDLE InquiryContext,
    __out_opt RPC_IF_ID __RPC_FAR *IfId,
    __out RPC_CSTR __RPC_FAR *MemberName,
    __out unsigned long __RPC_FAR *Priority,
    __out RPC_CSTR __RPC_FAR *Annotation
    );

#ifdef RPC_UNICODE_SUPPORTED

RPCNSAPI RPC_STATUS RPC_ENTRY
RpcNsProfileDeleteW(
    __in unsigned long ProfileNameSyntax,
    __in RPC_WSTR ProfileName
    );

RPCNSAPI RPC_STATUS RPC_ENTRY
RpcNsProfileEltAddW(
    __in unsigned long ProfileNameSyntax,
    __in RPC_WSTR ProfileName,
    __in_opt RPC_IF_ID __RPC_FAR *IfId,
    __in unsigned long MemberNameSyntax,
    __in RPC_WSTR MemberName,
    __in unsigned long Priority,
    __in_opt RPC_WSTR Annotation
    );

RPCNSAPI RPC_STATUS RPC_ENTRY
RpcNsProfileEltRemoveW(
    __in unsigned long ProfileNameSyntax,
    __in RPC_WSTR ProfileName,
    __in_opt RPC_IF_ID __RPC_FAR *IfId,
    __in unsigned long MemberNameSyntax,
    __in RPC_WSTR MemberName
    );

RPCNSAPI RPC_STATUS RPC_ENTRY
RpcNsProfileEltInqBeginW(
    __in unsigned long ProfileNameSyntax,
    __in RPC_WSTR ProfileName,
    __in unsigned long InquiryType,
    __in_opt RPC_IF_ID __RPC_FAR *IfId,
    __in unsigned long VersOption,
    __in unsigned long MemberNameSyntax,
    __in_opt RPC_WSTR MemberName,
    __out RPC_NS_HANDLE __RPC_FAR *InquiryContext
    );

RPCNSAPI RPC_STATUS RPC_ENTRY
RpcNsProfileEltInqNextW(
    __in RPC_NS_HANDLE InquiryContext,
    __out_opt RPC_IF_ID __RPC_FAR *IfId,
    __out RPC_WSTR __RPC_FAR *MemberName,
    __out unsigned long __RPC_FAR *Priority,
    __out RPC_WSTR __RPC_FAR *Annotation
    );

#endif

RPCNSAPI RPC_STATUS RPC_ENTRY
RpcNsProfileEltInqDone(
    IN OUT RPC_NS_HANDLE __RPC_FAR * InquiryContext
    );

/* Entry object APIs */

RPCNSAPI RPC_STATUS RPC_ENTRY
RpcNsEntryObjectInqBeginA(
    __in unsigned long EntryNameSyntax,
    __in RPC_CSTR EntryName,
    __out RPC_NS_HANDLE __RPC_FAR *InquiryContext
    );

#ifdef RPC_UNICODE_SUPPORTED

RPCNSAPI RPC_STATUS RPC_ENTRY
RpcNsEntryObjectInqBeginW(
    __in unsigned long EntryNameSyntax,
    __in RPC_WSTR EntryName,
    __out RPC_NS_HANDLE __RPC_FAR *InquiryContext
    );

#endif

RPCNSAPI RPC_STATUS RPC_ENTRY
RpcNsEntryObjectInqNext(
    IN  RPC_NS_HANDLE InquiryContext,
    OUT UUID __RPC_FAR * ObjUuid
    );

RPCNSAPI RPC_STATUS RPC_ENTRY
RpcNsEntryObjectInqDone(
    IN OUT RPC_NS_HANDLE __RPC_FAR * InquiryContext
    );

/* Management and MISC APIs */

RPCNSAPI RPC_STATUS RPC_ENTRY
RpcNsEntryExpandNameA(
    __in unsigned long EntryNameSyntax,
    __in RPC_CSTR EntryName,
    __out RPC_CSTR __RPC_FAR *ExpandedName
    );

RPCNSAPI RPC_STATUS RPC_ENTRY
RpcNsMgmtBindingUnexportA(
    __in unsigned long EntryNameSyntax,
    __in RPC_CSTR EntryName,
    __in_opt RPC_IF_ID __RPC_FAR *IfId,
    __in unsigned long VersOption,
    __in_opt UUID_VECTOR __RPC_FAR *ObjectUuidVec
    );

RPCNSAPI RPC_STATUS RPC_ENTRY
RpcNsMgmtEntryCreateA(
    __in unsigned long EntryNameSyntax,
    __in RPC_CSTR EntryName
    );

RPCNSAPI RPC_STATUS RPC_ENTRY
RpcNsMgmtEntryDeleteA(
    __in unsigned long EntryNameSyntax,
    __in RPC_CSTR EntryName
    );

RPCNSAPI RPC_STATUS RPC_ENTRY
RpcNsMgmtEntryInqIfIdsA(
    __in unsigned long EntryNameSyntax,
    __in RPC_CSTR EntryName,
    __out RPC_IF_ID_VECTOR __RPC_FAR * __RPC_FAR *IfIdVec
    );

RPCNSAPI RPC_STATUS RPC_ENTRY
RpcNsMgmtHandleSetExpAge(
    IN RPC_NS_HANDLE NsHandle,
    IN unsigned long ExpirationAge
    );

RPCNSAPI RPC_STATUS RPC_ENTRY
RpcNsMgmtInqExpAge(
    OUT unsigned long __RPC_FAR * ExpirationAge
    );

RPCNSAPI RPC_STATUS RPC_ENTRY
RpcNsMgmtSetExpAge(
    IN unsigned long ExpirationAge
    );

#ifdef RPC_UNICODE_SUPPORTED

RPCNSAPI RPC_STATUS RPC_ENTRY
RpcNsEntryExpandNameW(
    __in unsigned long EntryNameSyntax,
    __in RPC_WSTR EntryName,
    __out RPC_WSTR __RPC_FAR *ExpandedName
    );

RPCNSAPI RPC_STATUS RPC_ENTRY
RpcNsMgmtBindingUnexportW(
    __in unsigned long EntryNameSyntax,
    __in RPC_WSTR EntryName,
    __in_opt RPC_IF_ID __RPC_FAR *IfId,
    __in unsigned long VersOption,
    __in_opt UUID_VECTOR __RPC_FAR *ObjectUuidVec
    );

RPCNSAPI RPC_STATUS RPC_ENTRY
RpcNsMgmtEntryCreateW(
    __in unsigned long EntryNameSyntax,
    __in RPC_WSTR EntryName
    );

RPCNSAPI RPC_STATUS RPC_ENTRY
RpcNsMgmtEntryDeleteW(
    __in unsigned long EntryNameSyntax,
    __in RPC_WSTR EntryName
    );

RPCNSAPI RPC_STATUS RPC_ENTRY
RpcNsMgmtEntryInqIfIdsW(
    __in unsigned long EntryNameSyntax,
    __in RPC_WSTR EntryName,
    __out RPC_IF_ID_VECTOR __RPC_FAR * __RPC_FAR *IfIdVec
    );

#endif

/* Client API's implemented in wrappers. */

RPCNSAPI RPC_STATUS RPC_ENTRY
RpcNsBindingImportBeginA(
    __in unsigned long EntryNameSyntax,
    __in_opt RPC_CSTR EntryName,
    __in_opt RPC_IF_HANDLE IfSpec,
    __in_opt UUID __RPC_FAR *ObjUuid,
    __out RPC_NS_HANDLE __RPC_FAR *ImportContext
    );

#ifdef RPC_UNICODE_SUPPORTED

RPCNSAPI RPC_STATUS RPC_ENTRY
RpcNsBindingImportBeginW(
    __in unsigned long EntryNameSyntax,
    __in_opt RPC_WSTR EntryName,
    __in_opt RPC_IF_HANDLE IfSpec,
    __in_opt UUID __RPC_FAR *ObjUuid,
    __out RPC_NS_HANDLE __RPC_FAR *ImportContext
    );

#endif

RPCNSAPI RPC_STATUS RPC_ENTRY
RpcNsBindingImportNext(
    IN RPC_NS_HANDLE ImportContext,
    OUT RPC_BINDING_HANDLE  __RPC_FAR * Binding
    );

RPCNSAPI RPC_STATUS RPC_ENTRY
RpcNsBindingImportDone(
    IN OUT RPC_NS_HANDLE __RPC_FAR * ImportContext
    );

RPCNSAPI RPC_STATUS RPC_ENTRY
RpcNsBindingSelect(
    IN OUT RPC_BINDING_VECTOR __RPC_FAR * BindingVec,
    OUT RPC_BINDING_HANDLE  __RPC_FAR * Binding
    );

#ifdef UNICODE

#define RpcNsBindingLookupBegin RpcNsBindingLookupBeginW
#define RpcNsBindingImportBegin RpcNsBindingImportBeginW
#define RpcNsBindingExport RpcNsBindingExportW
#define RpcNsBindingUnexport RpcNsBindingUnexportW
#define RpcNsGroupDelete RpcNsGroupDeleteW
#define RpcNsGroupMbrAdd RpcNsGroupMbrAddW
#define RpcNsGroupMbrRemove RpcNsGroupMbrRemoveW
#define RpcNsGroupMbrInqBegin RpcNsGroupMbrInqBeginW
#define RpcNsGroupMbrInqNext RpcNsGroupMbrInqNextW
#define RpcNsEntryExpandName RpcNsEntryExpandNameW
#define RpcNsEntryObjectInqBegin RpcNsEntryObjectInqBeginW
#define RpcNsMgmtBindingUnexport RpcNsMgmtBindingUnexportW
#define RpcNsMgmtEntryCreate RpcNsMgmtEntryCreateW
#define RpcNsMgmtEntryDelete RpcNsMgmtEntryDeleteW
#define RpcNsMgmtEntryInqIfIds RpcNsMgmtEntryInqIfIdsW
#define RpcNsProfileDelete RpcNsProfileDeleteW
#define RpcNsProfileEltAdd RpcNsProfileEltAddW
#define RpcNsProfileEltRemove RpcNsProfileEltRemoveW
#define RpcNsProfileEltInqBegin RpcNsProfileEltInqBeginW
#define RpcNsProfileEltInqNext RpcNsProfileEltInqNextW
#define RpcNsBindingExportPnP RpcNsBindingExportPnPW
#define RpcNsBindingUnexportPnP RpcNsBindingUnexportPnPW

#else

#define RpcNsBindingLookupBegin RpcNsBindingLookupBeginA
#define RpcNsBindingImportBegin RpcNsBindingImportBeginA
#define RpcNsBindingExport RpcNsBindingExportA
#define RpcNsBindingUnexport RpcNsBindingUnexportA
#define RpcNsGroupDelete RpcNsGroupDeleteA
#define RpcNsGroupMbrAdd RpcNsGroupMbrAddA
#define RpcNsGroupMbrRemove RpcNsGroupMbrRemoveA
#define RpcNsGroupMbrInqBegin RpcNsGroupMbrInqBeginA
#define RpcNsGroupMbrInqNext RpcNsGroupMbrInqNextA
#define RpcNsEntryExpandName RpcNsEntryExpandNameA
#define RpcNsEntryObjectInqBegin RpcNsEntryObjectInqBeginA
#define RpcNsMgmtBindingUnexport RpcNsMgmtBindingUnexportA
#define RpcNsMgmtEntryCreate RpcNsMgmtEntryCreateA
#define RpcNsMgmtEntryDelete RpcNsMgmtEntryDeleteA
#define RpcNsMgmtEntryInqIfIds RpcNsMgmtEntryInqIfIdsA
#define RpcNsProfileDelete RpcNsProfileDeleteA
#define RpcNsProfileEltAdd RpcNsProfileEltAddA
#define RpcNsProfileEltRemove RpcNsProfileEltRemoveA
#define RpcNsProfileEltInqBegin RpcNsProfileEltInqBeginA
#define RpcNsProfileEltInqNext RpcNsProfileEltInqNextA
#define RpcNsBindingExportPnP RpcNsBindingExportPnPA
#define RpcNsBindingUnexportPnP RpcNsBindingUnexportPnPA
#endif /* UNICODE */

#endif /* __RPCNSI_H__ */

