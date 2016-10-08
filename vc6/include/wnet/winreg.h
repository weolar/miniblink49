/*++ BUILD Version: 0001    // Increment this if a change has global effects

Copyright (c) Microsoft Corporation.  All rights reserved.

Module Name:

    Winreg.h

Abstract:

    This module contains the function prototypes and constant, type and
    structure definitions for the Windows 32-Bit Registry API.

--*/

#ifndef _WINREG_
#define _WINREG_


#ifdef _MAC
#include <macwin32.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef WINVER
#define WINVER 0x0500   // version 5.0
#endif /* !WINVER */

//
// RRF - Registry Routine Flags (for RegGetValue)
//

#define RRF_RT_REG_NONE        0x00000001  // restrict type to REG_NONE      (other data types will not return ERROR_SUCCESS)
#define RRF_RT_REG_SZ          0x00000002  // restrict type to REG_SZ        (other data types will not return ERROR_SUCCESS) (automatically converts REG_EXPAND_SZ to REG_SZ unless RRF_NOEXPAND is specified)
#define RRF_RT_REG_EXPAND_SZ   0x00000004  // restrict type to REG_EXPAND_SZ (other data types will not return ERROR_SUCCESS) (must specify RRF_NOEXPAND or RegGetValue will fail with ERROR_INVALID_PARAMETER)
#define RRF_RT_REG_BINARY      0x00000008  // restrict type to REG_BINARY    (other data types will not return ERROR_SUCCESS)
#define RRF_RT_REG_DWORD       0x00000010  // restrict type to REG_DWORD     (other data types will not return ERROR_SUCCESS)
#define RRF_RT_REG_MULTI_SZ    0x00000020  // restrict type to REG_MULTI_SZ  (other data types will not return ERROR_SUCCESS)
#define RRF_RT_REG_QWORD       0x00000040  // restrict type to REG_QWORD     (other data types will not return ERROR_SUCCESS)

#define RRF_RT_DWORD           (RRF_RT_REG_BINARY | RRF_RT_REG_DWORD) // restrict type to *32-bit* RRF_RT_REG_BINARY or RRF_RT_REG_DWORD (other data types will not return ERROR_SUCCESS)
#define RRF_RT_QWORD           (RRF_RT_REG_BINARY | RRF_RT_REG_QWORD) // restrict type to *64-bit* RRF_RT_REG_BINARY or RRF_RT_REG_DWORD (other data types will not return ERROR_SUCCESS)
#define RRF_RT_ANY             0x0000ffff                             // no type restriction

#define RRF_NOEXPAND           0x10000000  // do not automatically expand environment strings if value is of type REG_EXPAND_SZ
#define RRF_ZEROONFAILURE      0x20000000  // if pvData is not NULL, set content to all zeros on failure

//
// Requested Key access mask type.
//

typedef ACCESS_MASK REGSAM;

//
// Reserved Key Handles.
//

#define HKEY_CLASSES_ROOT           (( HKEY ) (ULONG_PTR)((LONG)0x80000000) )
#define HKEY_CURRENT_USER           (( HKEY ) (ULONG_PTR)((LONG)0x80000001) )
#define HKEY_LOCAL_MACHINE          (( HKEY ) (ULONG_PTR)((LONG)0x80000002) )
#define HKEY_USERS                  (( HKEY ) (ULONG_PTR)((LONG)0x80000003) )
#define HKEY_PERFORMANCE_DATA       (( HKEY ) (ULONG_PTR)((LONG)0x80000004) )
#define HKEY_PERFORMANCE_TEXT       (( HKEY ) (ULONG_PTR)((LONG)0x80000050) )
#define HKEY_PERFORMANCE_NLSTEXT    (( HKEY ) (ULONG_PTR)((LONG)0x80000060) )
#if(WINVER >= 0x0400)
#define HKEY_CURRENT_CONFIG         (( HKEY ) (ULONG_PTR)((LONG)0x80000005) )
#define HKEY_DYN_DATA               (( HKEY ) (ULONG_PTR)((LONG)0x80000006) )


//
// RegConnectRegistryEx supported flags
//
#define REG_SECURE_CONNECTION   1 

/*NOINC*/
#ifndef _PROVIDER_STRUCTS_DEFINED
#define _PROVIDER_STRUCTS_DEFINED

#define PROVIDER_KEEPS_VALUE_LENGTH 0x1
struct val_context {
    int valuelen;       // the total length of this value
    LPVOID value_context;   // provider's context
    LPVOID val_buff_ptr;    // where in the ouput buffer the value is.
};

typedef struct val_context FAR *PVALCONTEXT;

typedef struct pvalueA {           // Provider supplied value/context.
    LPSTR   pv_valuename;          // The value name pointer
    int pv_valuelen;
    LPVOID pv_value_context;
    DWORD pv_type;
}PVALUEA, FAR *PPVALUEA;
typedef struct pvalueW {           // Provider supplied value/context.
    LPWSTR  pv_valuename;          // The value name pointer
    int pv_valuelen;
    LPVOID pv_value_context;
    DWORD pv_type;
}PVALUEW, FAR *PPVALUEW;
#ifdef UNICODE
typedef PVALUEW PVALUE;
typedef PPVALUEW PPVALUE;
#else
typedef PVALUEA PVALUE;
typedef PPVALUEA PPVALUE;
#endif // UNICODE

typedef
DWORD _cdecl
QUERYHANDLER (LPVOID keycontext, PVALCONTEXT val_list, DWORD num_vals,
          LPVOID outputbuffer, DWORD FAR *total_outlen, DWORD input_blen);

typedef QUERYHANDLER FAR *PQUERYHANDLER;

typedef struct provider_info {
    PQUERYHANDLER pi_R0_1val;
    PQUERYHANDLER pi_R0_allvals;
    PQUERYHANDLER pi_R3_1val;
    PQUERYHANDLER pi_R3_allvals;
    DWORD pi_flags;    // capability flags (none defined yet).
    LPVOID pi_key_context;
}REG_PROVIDER;

typedef struct provider_info FAR *PPROVIDER;

typedef struct value_entA {
    LPSTR   ve_valuename;
    DWORD ve_valuelen;
    DWORD_PTR ve_valueptr;
    DWORD ve_type;
}VALENTA, FAR *PVALENTA;
typedef struct value_entW {
    LPWSTR  ve_valuename;
    DWORD ve_valuelen;
    DWORD_PTR ve_valueptr;
    DWORD ve_type;
}VALENTW, FAR *PVALENTW;
#ifdef UNICODE
typedef VALENTW VALENT;
typedef PVALENTW PVALENT;
#else
typedef VALENTA VALENT;
typedef PVALENTA PVALENT;
#endif // UNICODE

#endif // not(_PROVIDER_STRUCTS_DEFINED)
/*INC*/

#endif /* WINVER >= 0x0400 */

//
// Default values for parameters that do not exist in the Win 3.1
// compatible APIs.
//

#define WIN31_CLASS                 NULL

//
// API Prototypes.
//


WINADVAPI
LONG
APIENTRY
RegCloseKey (
    __in HKEY hKey
    );

WINADVAPI
LONG
APIENTRY
RegOverridePredefKey (
    __in HKEY hKey,
    __in_opt HKEY hNewHKey
    );

WINADVAPI
LONG
APIENTRY
RegOpenUserClassesRoot(
    __in HANDLE hToken,
    __reserved DWORD dwOptions,
    __in REGSAM samDesired,
    __out PHKEY  phkResult
    );

WINADVAPI
LONG
APIENTRY
RegOpenCurrentUser(
    __in REGSAM samDesired,
    __out PHKEY phkResult
    );

WINADVAPI
LONG
APIENTRY
RegDisablePredefinedCache(
    );

WINADVAPI
LONG
APIENTRY
RegConnectRegistryA (
    __in_opt LPCSTR lpMachineName,
    __in HKEY hKey,
    __out PHKEY phkResult
    );
WINADVAPI
LONG
APIENTRY
RegConnectRegistryW (
    __in_opt LPCWSTR lpMachineName,
    __in HKEY hKey,
    __out PHKEY phkResult
    );
#ifdef UNICODE
#define RegConnectRegistry  RegConnectRegistryW
#else
#define RegConnectRegistry  RegConnectRegistryA
#endif // !UNICODE

WINADVAPI
LONG
APIENTRY
RegConnectRegistryExA (
    __in_opt LPCSTR lpMachineName,
    __in HKEY hKey,
    __in ULONG Flags,
    __out PHKEY phkResult
    );
WINADVAPI
LONG
APIENTRY
RegConnectRegistryExW (
    __in_opt LPCWSTR lpMachineName,
    __in HKEY hKey,
    __in ULONG Flags,
    __out PHKEY phkResult
    );
#ifdef UNICODE
#define RegConnectRegistryEx  RegConnectRegistryExW
#else
#define RegConnectRegistryEx  RegConnectRegistryExA
#endif // !UNICODE

WINADVAPI
LONG
APIENTRY
RegCreateKeyA (
    __in HKEY hKey,
    __in_opt LPCSTR lpSubKey,
    __out PHKEY phkResult
    );
WINADVAPI
LONG
APIENTRY
RegCreateKeyW (
    __in HKEY hKey,
    __in_opt LPCWSTR lpSubKey,
    __out PHKEY phkResult
    );
#ifdef UNICODE
#define RegCreateKey  RegCreateKeyW
#else
#define RegCreateKey  RegCreateKeyA
#endif // !UNICODE

WINADVAPI
LONG
APIENTRY
RegCreateKeyExA (
    __in HKEY hKey,
    __in LPCSTR lpSubKey,
    __reserved DWORD Reserved,
    __in_opt LPSTR lpClass,
    __in DWORD dwOptions,
    __in REGSAM samDesired,
    __in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    __out PHKEY phkResult,
    __out_opt LPDWORD lpdwDisposition
    );
WINADVAPI
LONG
APIENTRY
RegCreateKeyExW (
    __in HKEY hKey,
    __in LPCWSTR lpSubKey,
    __reserved DWORD Reserved,
    __in_opt LPWSTR lpClass,
    __in DWORD dwOptions,
    __in REGSAM samDesired,
    __in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    __out PHKEY phkResult,
    __out_opt LPDWORD lpdwDisposition
    );
#ifdef UNICODE
#define RegCreateKeyEx  RegCreateKeyExW
#else
#define RegCreateKeyEx  RegCreateKeyExA
#endif // !UNICODE

WINADVAPI
LONG
APIENTRY
RegDeleteKeyA (
    __in HKEY hKey,
    __in LPCSTR lpSubKey
    );
WINADVAPI
LONG
APIENTRY
RegDeleteKeyW (
    __in HKEY hKey,
    __in LPCWSTR lpSubKey
    );
#ifdef UNICODE
#define RegDeleteKey  RegDeleteKeyW
#else
#define RegDeleteKey  RegDeleteKeyA
#endif // !UNICODE
    
WINADVAPI
LONG
APIENTRY
RegDeleteKeyExA (
    __in HKEY hKey,
    __in LPCSTR lpSubKey,
    __in REGSAM samDesired,
    __reserved DWORD Reserved
    );
WINADVAPI
LONG
APIENTRY
RegDeleteKeyExW (
    __in HKEY hKey,
    __in LPCWSTR lpSubKey,
    __in REGSAM samDesired,
    __reserved DWORD Reserved
    );
#ifdef UNICODE
#define RegDeleteKeyEx  RegDeleteKeyExW
#else
#define RegDeleteKeyEx  RegDeleteKeyExA
#endif // !UNICODE

WINADVAPI
LONG
APIENTRY
RegDisableReflectionKey (
    __in HKEY hBase
    );    

WINADVAPI
LONG
APIENTRY
RegEnableReflectionKey (
    __in HKEY hBase
    );    

WINADVAPI
LONG
APIENTRY
RegQueryReflectionKey (
    __in HKEY hBase,
    __out BOOL *bIsReflectionDisabled
    );    


WINADVAPI
LONG
APIENTRY
RegDeleteValueA (
    __in HKEY hKey,
    __in_opt LPCSTR lpValueName
    );
WINADVAPI
LONG
APIENTRY
RegDeleteValueW (
    __in HKEY hKey,
    __in_opt LPCWSTR lpValueName
    );
#ifdef UNICODE
#define RegDeleteValue  RegDeleteValueW
#else
#define RegDeleteValue  RegDeleteValueA
#endif // !UNICODE

WINADVAPI
LONG
APIENTRY
RegEnumKeyA (
    __in HKEY hKey,
    __in DWORD dwIndex,
    __out_ecount_opt(cchName) LPSTR lpName,
    __in DWORD cchName
    );
WINADVAPI
LONG
APIENTRY
RegEnumKeyW (
    __in HKEY hKey,
    __in DWORD dwIndex,
    __out_ecount_opt(cchName) LPWSTR lpName,
    __in DWORD cchName
    );
#ifdef UNICODE
#define RegEnumKey  RegEnumKeyW
#else
#define RegEnumKey  RegEnumKeyA
#endif // !UNICODE

WINADVAPI
LONG
APIENTRY
RegEnumKeyExA (
    __in HKEY hKey,
    __in DWORD dwIndex,
    __out_ecount_opt(*lpcchName) LPSTR lpName,
    __inout LPDWORD lpcchName,
    __reserved LPDWORD lpReserved,
    __inout_ecount_opt(*lpcchClass) LPSTR lpClass,
    __inout_opt LPDWORD lpcchClass,
    __out_opt PFILETIME lpftLastWriteTime
    );
WINADVAPI
LONG
APIENTRY
RegEnumKeyExW (
    __in HKEY hKey,
    __in DWORD dwIndex,
    __out_ecount_opt(*lpcchName) LPWSTR lpName,
    __inout LPDWORD lpcchName,
    __reserved LPDWORD lpReserved,
    __inout_ecount_opt(*lpcchClass) LPWSTR lpClass,
    __inout_opt LPDWORD lpcchClass,
    __out_opt PFILETIME lpftLastWriteTime
    );
#ifdef UNICODE
#define RegEnumKeyEx  RegEnumKeyExW
#else
#define RegEnumKeyEx  RegEnumKeyExA
#endif // !UNICODE

WINADVAPI
LONG
APIENTRY
RegEnumValueA (
    __in HKEY hKey,
    __in DWORD dwIndex,
    __out_ecount_opt(*lpcchValueName) LPSTR lpValueName,
    __inout LPDWORD lpcchValueName,
    __reserved LPDWORD lpReserved,
    __out_opt LPDWORD lpType,
    __out_bcount_opt(*lpcbData) LPBYTE lpData,
    __inout_opt LPDWORD lpcbData
    );
WINADVAPI
LONG
APIENTRY
RegEnumValueW (
    __in HKEY hKey,
    __in DWORD dwIndex,
    __out_ecount_opt(*lpcchValueName) LPWSTR lpValueName,
    __inout LPDWORD lpcchValueName,
    __reserved LPDWORD lpReserved,
    __out_opt LPDWORD lpType,
    __out_bcount_opt(*lpcbData) LPBYTE lpData,
    __inout_opt LPDWORD lpcbData
    );
#ifdef UNICODE
#define RegEnumValue  RegEnumValueW
#else
#define RegEnumValue  RegEnumValueA
#endif // !UNICODE

WINADVAPI
LONG
APIENTRY
RegFlushKey (
    __in HKEY hKey
    );

WINADVAPI
LONG
APIENTRY
RegGetKeySecurity (
    __in HKEY hKey,
    __in SECURITY_INFORMATION SecurityInformation,
    __out_bcount_opt(*lpcbSecurityDescriptor) PSECURITY_DESCRIPTOR pSecurityDescriptor,
    __inout LPDWORD lpcbSecurityDescriptor
    );

WINADVAPI
LONG
APIENTRY
RegLoadKeyA (
    __in HKEY    hKey,
    __in_opt LPCSTR  lpSubKey,
    __in LPCSTR  lpFile
    );
WINADVAPI
LONG
APIENTRY
RegLoadKeyW (
    __in HKEY    hKey,
    __in_opt LPCWSTR  lpSubKey,
    __in LPCWSTR  lpFile
    );
#ifdef UNICODE
#define RegLoadKey  RegLoadKeyW
#else
#define RegLoadKey  RegLoadKeyA
#endif // !UNICODE

WINADVAPI
LONG
APIENTRY
RegNotifyChangeKeyValue (
    __in HKEY hKey,
    __in BOOL bWatchSubtree,
    __in DWORD dwNotifyFilter,
    __in_opt HANDLE hEvent,
    __in BOOL fAsynchronous
    );

WINADVAPI
LONG
APIENTRY
RegOpenKeyA (
    __in HKEY hKey,
    __in_opt LPCSTR lpSubKey,
    __out PHKEY phkResult
    );
WINADVAPI
LONG
APIENTRY
RegOpenKeyW (
    __in HKEY hKey,
    __in_opt LPCWSTR lpSubKey,
    __out PHKEY phkResult
    );
#ifdef UNICODE
#define RegOpenKey  RegOpenKeyW
#else
#define RegOpenKey  RegOpenKeyA
#endif // !UNICODE

WINADVAPI
LONG
APIENTRY
RegOpenKeyExA (
    __in HKEY hKey,
    __in_opt LPCSTR lpSubKey,
    __reserved DWORD ulOptions,
    __in REGSAM samDesired,
    __out PHKEY phkResult
    );
WINADVAPI
LONG
APIENTRY
RegOpenKeyExW (
    __in HKEY hKey,
    __in_opt LPCWSTR lpSubKey,
    __reserved DWORD ulOptions,
    __in REGSAM samDesired,
    __out PHKEY phkResult
    );
#ifdef UNICODE
#define RegOpenKeyEx  RegOpenKeyExW
#else
#define RegOpenKeyEx  RegOpenKeyExA
#endif // !UNICODE

WINADVAPI
LONG
APIENTRY
RegQueryInfoKeyA (
    __in HKEY hKey,
    __out_ecount_opt(*lpcchClass) LPSTR lpClass,
    __inout_opt LPDWORD lpcchClass,
    __reserved LPDWORD lpReserved,
    __out_opt LPDWORD lpcSubKeys,
    __out_opt LPDWORD lpcbMaxSubKeyLen,
    __out_opt LPDWORD lpcbMaxClassLen,
    __out_opt LPDWORD lpcValues,
    __out_opt LPDWORD lpcbMaxValueNameLen,
    __out_opt LPDWORD lpcbMaxValueLen,
    __out_opt LPDWORD lpcbSecurityDescriptor,
    __out_opt PFILETIME lpftLastWriteTime
    );
WINADVAPI
LONG
APIENTRY
RegQueryInfoKeyW (
    __in HKEY hKey,
    __out_ecount_opt(*lpcchClass) LPWSTR lpClass,
    __inout_opt LPDWORD lpcchClass,
    __reserved LPDWORD lpReserved,
    __out_opt LPDWORD lpcSubKeys,
    __out_opt LPDWORD lpcbMaxSubKeyLen,
    __out_opt LPDWORD lpcbMaxClassLen,
    __out_opt LPDWORD lpcValues,
    __out_opt LPDWORD lpcbMaxValueNameLen,
    __out_opt LPDWORD lpcbMaxValueLen,
    __out_opt LPDWORD lpcbSecurityDescriptor,
    __out_opt PFILETIME lpftLastWriteTime
    );
#ifdef UNICODE
#define RegQueryInfoKey  RegQueryInfoKeyW
#else
#define RegQueryInfoKey  RegQueryInfoKeyA
#endif // !UNICODE

WINADVAPI
LONG
APIENTRY
RegQueryValueA (
    __in HKEY hKey,
    __in_opt LPCSTR lpSubKey,
    __out_bcount_opt(*lpcbData) LPSTR lpData,
    __inout_opt PLONG lpcbData
    );
WINADVAPI
LONG
APIENTRY
RegQueryValueW (
    __in HKEY hKey,
    __in_opt LPCWSTR lpSubKey,
    __out_bcount_opt(*lpcbData) LPWSTR lpData,
    __inout_opt PLONG lpcbData
    );
#ifdef UNICODE
#define RegQueryValue  RegQueryValueW
#else
#define RegQueryValue  RegQueryValueA
#endif // !UNICODE

#if(WINVER >= 0x0400)
WINADVAPI
LONG
APIENTRY
RegQueryMultipleValuesA (
    __in HKEY hKey,
    __out_ecount(num_vals) PVALENTA val_list,
    __in DWORD num_vals,
    __out_bcount_opt(*ldwTotsize) LPSTR lpValueBuf,
    __inout_opt LPDWORD ldwTotsize
    );
WINADVAPI
LONG
APIENTRY
RegQueryMultipleValuesW (
    __in HKEY hKey,
    __out_ecount(num_vals) PVALENTW val_list,
    __in DWORD num_vals,
    __out_bcount_opt(*ldwTotsize) LPWSTR lpValueBuf,
    __inout_opt LPDWORD ldwTotsize
    );
#ifdef UNICODE
#define RegQueryMultipleValues  RegQueryMultipleValuesW
#else
#define RegQueryMultipleValues  RegQueryMultipleValuesA
#endif // !UNICODE
#endif /* WINVER >= 0x0400 */

WINADVAPI
LONG
APIENTRY
RegQueryValueExA (
    __in HKEY hKey,
    __in_opt LPCSTR lpValueName,
    __reserved LPDWORD lpReserved,
    __out_opt LPDWORD lpType,
    __out_bcount_opt(*lpcbData) LPBYTE lpData,
    __inout_opt LPDWORD lpcbData
    );
WINADVAPI
LONG
APIENTRY
RegQueryValueExW (
    __in HKEY hKey,
    __in_opt LPCWSTR lpValueName,
    __reserved LPDWORD lpReserved,
    __out_opt LPDWORD lpType,
    __out_bcount_opt(*lpcbData) LPBYTE lpData,
    __inout_opt LPDWORD lpcbData
    );
#ifdef UNICODE
#define RegQueryValueEx  RegQueryValueExW
#else
#define RegQueryValueEx  RegQueryValueExA
#endif // !UNICODE

WINADVAPI
LONG
APIENTRY
RegReplaceKeyA (
    __in HKEY hKey,
    __in_opt LPCSTR lpSubKey,
    __in LPCSTR lpNewFile,
    __in LPCSTR lpOldFile
    );
WINADVAPI
LONG
APIENTRY
RegReplaceKeyW (
    __in HKEY hKey,
    __in_opt LPCWSTR lpSubKey,
    __in LPCWSTR lpNewFile,
    __in LPCWSTR lpOldFile
    );
#ifdef UNICODE
#define RegReplaceKey  RegReplaceKeyW
#else
#define RegReplaceKey  RegReplaceKeyA
#endif // !UNICODE

WINADVAPI
LONG
APIENTRY
RegRestoreKeyA (
    __in HKEY hKey,
    __in LPCSTR lpFile,
    __in DWORD dwFlags
    );
WINADVAPI
LONG
APIENTRY
RegRestoreKeyW (
    __in HKEY hKey,
    __in LPCWSTR lpFile,
    __in DWORD dwFlags
    );
#ifdef UNICODE
#define RegRestoreKey  RegRestoreKeyW
#else
#define RegRestoreKey  RegRestoreKeyA
#endif // !UNICODE

WINADVAPI
LONG
APIENTRY
RegSaveKeyA (
    __in HKEY hKey,
    __in LPCSTR lpFile,
    __in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes
    );
WINADVAPI
LONG
APIENTRY
RegSaveKeyW (
    __in HKEY hKey,
    __in LPCWSTR lpFile,
    __in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes
    );
#ifdef UNICODE
#define RegSaveKey  RegSaveKeyW
#else
#define RegSaveKey  RegSaveKeyA
#endif // !UNICODE

WINADVAPI
LONG
APIENTRY
RegSetKeySecurity (
    __in HKEY hKey,
    __in SECURITY_INFORMATION SecurityInformation,
    __in PSECURITY_DESCRIPTOR pSecurityDescriptor
    );

WINADVAPI
LONG
APIENTRY
RegSetValueA (
    __in HKEY hKey,
    __in_opt LPCSTR lpSubKey,
    __in DWORD dwType,
    __in_bcount_opt(cbData) LPCSTR lpData,
    __in DWORD cbData
    );
WINADVAPI
LONG
APIENTRY
RegSetValueW (
    __in HKEY hKey,
    __in_opt LPCWSTR lpSubKey,
    __in DWORD dwType,
    __in_bcount_opt(cbData) LPCWSTR lpData,
    __in DWORD cbData
    );
#ifdef UNICODE
#define RegSetValue  RegSetValueW
#else
#define RegSetValue  RegSetValueA
#endif // !UNICODE


WINADVAPI
LONG
APIENTRY
RegSetValueExA (
    __in HKEY hKey,
    __in_opt LPCSTR lpValueName,
    __reserved DWORD Reserved,
    __in DWORD dwType,
    __in_bcount_opt(cbData) CONST BYTE* lpData,
    __in DWORD cbData
    );
WINADVAPI
LONG
APIENTRY
RegSetValueExW (
    __in HKEY hKey,
    __in_opt LPCWSTR lpValueName,
    __reserved DWORD Reserved,
    __in DWORD dwType,
    __in_bcount_opt(cbData) CONST BYTE* lpData,
    __in DWORD cbData
    );
#ifdef UNICODE
#define RegSetValueEx  RegSetValueExW
#else
#define RegSetValueEx  RegSetValueExA
#endif // !UNICODE

WINADVAPI
LONG
APIENTRY
RegUnLoadKeyA (
    __in HKEY    hKey,
    __in_opt LPCSTR lpSubKey
    );
WINADVAPI
LONG
APIENTRY
RegUnLoadKeyW (
    __in HKEY    hKey,
    __in_opt LPCWSTR lpSubKey
    );
#ifdef UNICODE
#define RegUnLoadKey  RegUnLoadKeyW
#else
#define RegUnLoadKey  RegUnLoadKeyA
#endif // !UNICODE

WINADVAPI
LONG
APIENTRY
RegGetValueA (
    __in HKEY    hkey,
    __in_opt LPCSTR  lpSubKey,
    __in_opt LPCSTR  lpValue,
    __in_opt DWORD    dwFlags,
    __out_opt LPDWORD pdwType,
    __out_bcount_opt(*pcbData) PVOID   pvData,
    __inout_opt LPDWORD pcbData 
    );
WINADVAPI
LONG
APIENTRY
RegGetValueW (
    __in HKEY    hkey,
    __in_opt LPCWSTR  lpSubKey,
    __in_opt LPCWSTR  lpValue,
    __in_opt DWORD    dwFlags,
    __out_opt LPDWORD pdwType,
    __out_bcount_opt(*pcbData) PVOID   pvData,
    __inout_opt LPDWORD pcbData 
    );
#ifdef UNICODE
#define RegGetValue  RegGetValueW
#else
#define RegGetValue  RegGetValueA
#endif // !UNICODE

//
// Remoteable System Shutdown APIs
//

WINADVAPI
BOOL
APIENTRY
InitiateSystemShutdownA(
    __in_opt LPSTR lpMachineName,
    __in_opt LPSTR lpMessage,
    __in DWORD dwTimeout,
    __in BOOL bForceAppsClosed,
    __in BOOL bRebootAfterShutdown
    );
WINADVAPI
BOOL
APIENTRY
InitiateSystemShutdownW(
    __in_opt LPWSTR lpMachineName,
    __in_opt LPWSTR lpMessage,
    __in DWORD dwTimeout,
    __in BOOL bForceAppsClosed,
    __in BOOL bRebootAfterShutdown
    );
#ifdef UNICODE
#define InitiateSystemShutdown  InitiateSystemShutdownW
#else
#define InitiateSystemShutdown  InitiateSystemShutdownA
#endif // !UNICODE


WINADVAPI
BOOL
APIENTRY
AbortSystemShutdownA(
    __in_opt LPSTR lpMachineName
    );
WINADVAPI
BOOL
APIENTRY
AbortSystemShutdownW(
    __in_opt LPWSTR lpMachineName
    );
#ifdef UNICODE
#define AbortSystemShutdown  AbortSystemShutdownW
#else
#define AbortSystemShutdown  AbortSystemShutdownA
#endif // !UNICODE

//
// defines for InitiateSystemShutdownEx reason codes
//

#include <reason.h>             // get the public reasons
//
// Then for Historical reasons support some old symbols, internal only

#define REASON_SWINSTALL    SHTDN_REASON_MAJOR_SOFTWARE|SHTDN_REASON_MINOR_INSTALLATION
#define REASON_HWINSTALL    SHTDN_REASON_MAJOR_HARDWARE|SHTDN_REASON_MINOR_INSTALLATION
#define REASON_SERVICEHANG  SHTDN_REASON_MAJOR_SOFTWARE|SHTDN_REASON_MINOR_HUNG
#define REASON_UNSTABLE     SHTDN_REASON_MAJOR_SYSTEM|SHTDN_REASON_MINOR_UNSTABLE
#define REASON_SWHWRECONF   SHTDN_REASON_MAJOR_SOFTWARE|SHTDN_REASON_MINOR_RECONFIG
#define REASON_OTHER        SHTDN_REASON_MAJOR_OTHER|SHTDN_REASON_MINOR_OTHER
#define REASON_UNKNOWN      SHTDN_REASON_UNKNOWN
#define REASON_LEGACY_API   SHTDN_REASON_LEGACY_API
#define REASON_PLANNED_FLAG SHTDN_REASON_FLAG_PLANNED

//
// MAX Shutdown TimeOut == 10 Years in seconds
//
#define MAX_SHUTDOWN_TIMEOUT (10*365*24*60*60)  	

WINADVAPI
BOOL
APIENTRY
InitiateSystemShutdownExA(
    __in_opt LPSTR lpMachineName,
    __in_opt LPSTR lpMessage,
    __in DWORD dwTimeout,
    __in BOOL bForceAppsClosed,
    __in BOOL bRebootAfterShutdown,
    __in DWORD dwReason
    );
WINADVAPI
BOOL
APIENTRY
InitiateSystemShutdownExW(
    __in_opt LPWSTR lpMachineName,
    __in_opt LPWSTR lpMessage,
    __in DWORD dwTimeout,
    __in BOOL bForceAppsClosed,
    __in BOOL bRebootAfterShutdown,
    __in DWORD dwReason
    );
#ifdef UNICODE
#define InitiateSystemShutdownEx  InitiateSystemShutdownExW
#else
#define InitiateSystemShutdownEx  InitiateSystemShutdownExA
#endif // !UNICODE


WINADVAPI
LONG
APIENTRY
RegSaveKeyExA (
    __in HKEY hKey,
    __in LPCSTR lpFile,
    __in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    __in DWORD Flags
    );
WINADVAPI
LONG
APIENTRY
RegSaveKeyExW (
    __in HKEY hKey,
    __in LPCWSTR lpFile,
    __in_opt LPSECURITY_ATTRIBUTES lpSecurityAttributes,
    __in DWORD Flags
    );
#ifdef UNICODE
#define RegSaveKeyEx  RegSaveKeyExW
#else
#define RegSaveKeyEx  RegSaveKeyExA
#endif // !UNICODE

WINADVAPI
LONG
APIENTRY
Wow64Win32ApiEntry (
    __in DWORD dwFuncNumber,
    __in DWORD dwFlag,
    __in DWORD dwRes
    );

#ifdef __cplusplus
}
#endif


#endif // _WINREG_


