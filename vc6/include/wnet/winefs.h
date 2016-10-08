//+---------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright (C) Microsoft Corporation, 1992-1999.
//
//  File:       winefs.h
//
//  Contents:   EFS Data and prototypes.
//
//----------------------------------------------------------------------------

#ifndef __WINEFS_H__
#define __WINEFS_H__

#if _MSC_VER > 1000
#pragma once
#endif


#ifdef __cplusplus
extern "C" {
#endif

//+---------------------------------------------------------------------------------/
//                                                                                  /
//                                                                                  /
//                          Data Structures                                         /
//                                                                                  /
//                                                                                  /
//----------------------------------------------------------------------------------/


#ifndef ALGIDDEF
#define ALGIDDEF
typedef unsigned int ALG_ID;
#endif

//
//  Encoded Certificate
//


typedef struct _CERTIFICATE_BLOB {

    DWORD   dwCertEncodingType;

#ifdef MIDL_PASS
    [range(0,32768)] 
#endif // MIDL_PASS

    DWORD   cbData;

#ifdef MIDL_PASS
    [size_is(cbData)]
#endif // MIDL_PASS
    PBYTE    pbData;

} EFS_CERTIFICATE_BLOB, *PEFS_CERTIFICATE_BLOB;


//
//  Certificate Hash
//

typedef struct _EFS_HASH_BLOB {

#ifdef MIDL_PASS
    [range(0,100)] 
#endif // MIDL_PASS
    DWORD   cbData;

#ifdef MIDL_PASS
    [size_is(cbData)]
#endif // MIDL_PASS
    PBYTE    pbData;

} EFS_HASH_BLOB, *PEFS_HASH_BLOB;


//
//  RPC blob
//

typedef struct _EFS_RPC_BLOB {

#ifdef MIDL_PASS
    [range(0,266240)] 
#endif // MIDL_PASS
    DWORD   cbData;

#ifdef MIDL_PASS
    [size_is(cbData)]
#endif // MIDL_PASS
    PBYTE    pbData;

} EFS_RPC_BLOB, *PEFS_RPC_BLOB;

typedef struct _EFS_KEY_INFO {

    DWORD   dwVersion;
    ULONG   Entropy;
    ALG_ID  Algorithm;
    ULONG   KeyLength;
    
} EFS_KEY_INFO, *PEFS_KEY_INFO;


//
// Input to add a user to an encrypted file
//


typedef struct _ENCRYPTION_CERTIFICATE {
    DWORD cbTotalLength;
    SID * pUserSid;
    PEFS_CERTIFICATE_BLOB pCertBlob;
} ENCRYPTION_CERTIFICATE, *PENCRYPTION_CERTIFICATE;

#define MAX_SID_SIZE 256


typedef struct _ENCRYPTION_CERTIFICATE_HASH {
    DWORD cbTotalLength;
    SID * pUserSid;
    PEFS_HASH_BLOB  pHash;

#ifdef MIDL_PASS
    [string]
#endif // MIDL_PASS
    LPWSTR lpDisplayInformation;

} ENCRYPTION_CERTIFICATE_HASH, *PENCRYPTION_CERTIFICATE_HASH;







typedef struct _ENCRYPTION_CERTIFICATE_HASH_LIST {
#ifdef MIDL_PASS
    [range(0,500)] 
#endif // MIDL_PASS
    DWORD nCert_Hash;
#ifdef MIDL_PASS
    [size_is(nCert_Hash)]
#endif // MIDL_PASS
     PENCRYPTION_CERTIFICATE_HASH * pUsers;
} ENCRYPTION_CERTIFICATE_HASH_LIST, *PENCRYPTION_CERTIFICATE_HASH_LIST;



typedef struct _ENCRYPTION_CERTIFICATE_LIST {    
#ifdef MIDL_PASS
    [range(0,500)] 
#endif // MIDL_PASS
    DWORD nUsers;
#ifdef MIDL_PASS
    [size_is(nUsers)]
#endif // MIDL_PASS
     PENCRYPTION_CERTIFICATE * pUsers;
} ENCRYPTION_CERTIFICATE_LIST, *PENCRYPTION_CERTIFICATE_LIST;




//+---------------------------------------------------------------------------------/
//                                                                                  /
//                                                                                  /
//                               Prototypes                                         /
//                                                                                  /
//                                                                                  /
//----------------------------------------------------------------------------------/


WINADVAPI
DWORD
WINAPI
QueryUsersOnEncryptedFile(
     __in LPCWSTR lpFileName,
     __out PENCRYPTION_CERTIFICATE_HASH_LIST * pUsers
    );


WINADVAPI
DWORD
WINAPI
QueryRecoveryAgentsOnEncryptedFile(
     __in LPCWSTR lpFileName,
     __out PENCRYPTION_CERTIFICATE_HASH_LIST * pRecoveryAgents
    );


WINADVAPI
DWORD
WINAPI
RemoveUsersFromEncryptedFile(
     __in LPCWSTR lpFileName,
     __in PENCRYPTION_CERTIFICATE_HASH_LIST pHashes
    );

WINADVAPI
DWORD
WINAPI
AddUsersToEncryptedFile(
     __in LPCWSTR lpFileName,
     __in PENCRYPTION_CERTIFICATE_LIST pUsers
    );

WINADVAPI
DWORD
WINAPI
SetUserFileEncryptionKey(
    __in_opt PENCRYPTION_CERTIFICATE pEncryptionCertificate
    );


WINADVAPI
VOID
WINAPI
FreeEncryptionCertificateHashList(
    __in_opt PENCRYPTION_CERTIFICATE_HASH_LIST pHashes
    );

WINADVAPI
BOOL
WINAPI
EncryptionDisable(
    __in LPCWSTR DirPath,
    __in BOOL Disable
    );


WINADVAPI
DWORD
WINAPI
DuplicateEncryptionInfoFile(
     __in LPCWSTR SrcFileName,
     __in LPCWSTR DstFileName, 
     __in DWORD dwCreationDistribution, 
     __in DWORD dwAttributes, 
     __in CONST LPSECURITY_ATTRIBUTES lpSecurityAttributes
     );

#ifdef __cplusplus
}       // Balance extern "C" above
#endif

#endif // __WINEFS_H__

