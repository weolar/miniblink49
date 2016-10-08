//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
//  Copyright (C) Microsoft Corporation, 1996 - 1999
//
//  File:       wintrust.h
//
//  Contents:   Microsoft Internet Security Trust Provider Model
//
//  History:    31-May-1997 pberkman   created
//
//--------------------------------------------------------------------------

#ifndef WINTRUST_H
#define WINTRUST_H

#if _MSC_VER > 1000
#pragma once
#endif

#include    <wincrypt.h>

#ifdef __cplusplus
extern "C"
{
#endif


#ifndef WIN_CERT_REVISION_1_0   // there were duplicate definitions in winbase.h
#   define  WT_DEFINE_ALL_APIS
#else
#   undef   WT_DEFINE_ALL_APIS
#endif


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
//      Client definitions, typedefs, and prototypes
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

#include <pshpack8.h>

//////////////////////////////////////////////////////////////////////////////
//
// WINTRUST_DATA Structure
//----------------------------------------------------------------------------
//  Used when calling WinVerifyTrust to pass necessary information into
//  the Providers.
//
typedef struct _WINTRUST_DATA
{
    DWORD           cbStruct;                   // = sizeof(WINTRUST_DATA)

    LPVOID          pPolicyCallbackData;        // optional: used to pass data between the app and policy
    LPVOID          pSIPClientData;             // optional: used to pass data between the app and SIP.

    DWORD           dwUIChoice;                 // required: UI choice.  One of the following.
#                       define      WTD_UI_ALL              1
#                       define      WTD_UI_NONE             2
#                       define      WTD_UI_NOBAD            3
#                       define      WTD_UI_NOGOOD           4

    DWORD           fdwRevocationChecks;        // required: certificate revocation check options
#                       define      WTD_REVOKE_NONE         0x00000000
#                       define      WTD_REVOKE_WHOLECHAIN   0x00000001

    DWORD           dwUnionChoice;              // required: which structure is being passed in?
#                       define      WTD_CHOICE_FILE         1
#                       define      WTD_CHOICE_CATALOG      2
#                       define      WTD_CHOICE_BLOB         3
#                       define      WTD_CHOICE_SIGNER       4
#                       define      WTD_CHOICE_CERT         5
    union
    {
        struct WINTRUST_FILE_INFO_      *pFile;         // individual file
        struct WINTRUST_CATALOG_INFO_   *pCatalog;      // member of a Catalog File
        struct WINTRUST_BLOB_INFO_      *pBlob;         // memory blob
        struct WINTRUST_SGNR_INFO_      *pSgnr;         // signer structure only
        struct WINTRUST_CERT_INFO_      *pCert;
    };

    DWORD           dwStateAction;                      // optional (Catalog File Processing)
#                       define      WTD_STATEACTION_IGNORE           0x00000000
#                       define      WTD_STATEACTION_VERIFY           0x00000001
#                       define      WTD_STATEACTION_CLOSE            0x00000002
#                       define      WTD_STATEACTION_AUTO_CACHE       0x00000003
#                       define      WTD_STATEACTION_AUTO_CACHE_FLUSH 0x00000004

    HANDLE          hWVTStateData;                      // optional (Catalog File Processing)

    WCHAR           *pwszURLReference;          // optional: (future) used to determine zone.

    // 17-Feb-1998 philh: added
    DWORD           dwProvFlags;
#       define WTD_PROV_FLAGS_MASK                      0x0000FFFF
#       define WTD_USE_IE4_TRUST_FLAG                   0x00000001
#       define WTD_NO_IE4_CHAIN_FLAG                    0x00000002
#       define WTD_NO_POLICY_USAGE_FLAG                 0x00000004
#       define WTD_REVOCATION_CHECK_NONE                0x00000010
#       define WTD_REVOCATION_CHECK_END_CERT            0x00000020
#       define WTD_REVOCATION_CHECK_CHAIN               0x00000040
#       define WTD_REVOCATION_CHECK_CHAIN_EXCLUDE_ROOT  0x00000080
#       define WTD_SAFER_FLAG                           0x00000100
#       define WTD_HASH_ONLY_FLAG                       0x00000200
#       define WTD_USE_DEFAULT_OSVER_CHECK              0x00000400
#       define WTD_LIFETIME_SIGNING_FLAG                0x00000800
#       define WTD_CACHE_ONLY_URL_RETRIEVAL             0x00001000 // affects CRL retrieval and AIA retrieval

    // 07-Jan-2004 tonyschr: added
    DWORD           dwUIContext;                // optional: used to determine action text in UI
#       define WTD_UICONTEXT_EXECUTE                    0
#       define WTD_UICONTEXT_INSTALL                    1
} WINTRUST_DATA, *PWINTRUST_DATA;

//////////////////////////////////////////////////////////////////////////////
//
// WINTRUST_FILE_INFO Structure
//----------------------------------------------------------------------------
//  Used when calling WinVerifyTrust against an individual file.
//
typedef struct WINTRUST_FILE_INFO_
{
    DWORD           cbStruct;                   // = sizeof(WINTRUST_FILE_INFO)

    LPCWSTR         pcwszFilePath;              // required, file name to be verified
    HANDLE          hFile;                      // optional, open handle to pcwszFilePath

    // 09-Dec-1997 pberkman: added
    GUID            *pgKnownSubject;            // optional: fill if the subject type is known.

} WINTRUST_FILE_INFO, *PWINTRUST_FILE_INFO;

//////////////////////////////////////////////////////////////////////////////
//
// WINTRUST_CATALOG_INFO Structure
//----------------------------------------------------------------------------
//  Used when calling WinVerifyTrust against a member of a Microsoft Catalog
//  file.
//
typedef struct WINTRUST_CATALOG_INFO_
{
    DWORD           cbStruct;               // = sizeof(WINTRUST_CATALOG_INFO)

    DWORD           dwCatalogVersion;       // optional: Catalog version number
    LPCWSTR         pcwszCatalogFilePath;   // required: path/name to Catalog file

    LPCWSTR         pcwszMemberTag;         // required: tag to member in Catalog
    LPCWSTR         pcwszMemberFilePath;    // required: path/name to member file
    HANDLE          hMemberFile;            // optional: open handle to pcwszMemberFilePath

    // 30-Oct-1997 pberkman: added
    BYTE            *pbCalculatedFileHash;  // optional: pass in the calculated hash
    DWORD           cbCalculatedFileHash;   // optional: pass in the count bytes of the calc hash

    // 15-Jan-1998 pberkman: added
    PCCTL_CONTEXT   pcCatalogContext;       // optional: pass in to use instead of CatalogFilePath.

} WINTRUST_CATALOG_INFO, *PWINTRUST_CATALOG_INFO;

//////////////////////////////////////////////////////////////////////////////
//
// WINTRUST_BLOB_INFO Structure
//----------------------------------------------------------------------------
//  Used when calling WinVerifyTrust against a memory blob.
//
typedef struct WINTRUST_BLOB_INFO_
{
    DWORD           cbStruct;               // = sizeof(WINTRUST_BLOB_INFO)

    GUID            gSubject;               // SIP to load

    LPCWSTR         pcwszDisplayName;       // display name of object

    DWORD           cbMemObject;
    BYTE            *pbMemObject;

    DWORD           cbMemSignedMsg;
    BYTE            *pbMemSignedMsg;

} WINTRUST_BLOB_INFO, *PWINTRUST_BLOB_INFO;

//////////////////////////////////////////////////////////////////////////////
//
// WINTRUST_SGNR_INFO Structure
//----------------------------------------------------------------------------
//  Used when calling WinVerifyTrust against a CMSG_SIGNER_INFO Structure
//
typedef struct WINTRUST_SGNR_INFO_
{
    DWORD           cbStruct;               // = sizeof(WINTRUST_SGNR_INFO)

    LPCWSTR         pcwszDisplayName;       // name of the "thing" the pbMem is pointing to.

    CMSG_SIGNER_INFO *psSignerInfo;

    DWORD           chStores;               // number of stores in pahStores
    HCERTSTORE      *pahStores;             // array of stores to add to internal list

} WINTRUST_SGNR_INFO, *PWINTRUST_SGNR_INFO;

//////////////////////////////////////////////////////////////////////////////
//
// WINTRUST_CERT_INFO Structure
//----------------------------------------------------------------------------
//  Used when calling WinVerifyTrust against a CERT_CONTEXT Structure
//
typedef struct WINTRUST_CERT_INFO_
{
    DWORD           cbStruct;               // = sizeof(WINTRUST_CERT_INFO)

    LPCWSTR         pcwszDisplayName;       // display name

    CERT_CONTEXT    *psCertContext;

    DWORD           chStores;               // number of stores in pahStores
    HCERTSTORE      *pahStores;             // array of stores to add to internal list

    // 17-Nov-1997 pberkman: added
    DWORD           dwFlags;
#                       define      WTCI_DONT_OPEN_STORES   0x00000001  // only open dummy "root" all other are in pahStores.
#                       define      WTCI_OPEN_ONLY_ROOT     0x00000002

    // 26-Nov-1997 pberkman: added
    FILETIME        *psftVerifyAsOf;        // if not null, each cert will be validated as of this time.

} WINTRUST_CERT_INFO, *PWINTRUST_CERT_INFO;

#include <poppack.h>


//////////////////////////////////////////////////////////////////////////////
//
// WinVerifyTrust
//----------------------------------------------------------------------------
//  Exported from WINTRUST.DLL.
//  Call this function to verify the trust based on a digital signer.
//
//  pWVTData points to a WINTRUST_DATA data structure.
//
//  WTD_SAFER_FLAG should be set in WINTRUST_DATA's dwProvFlags to enable
//  the following semantics for the WINTRUST_ACTION_GENERIC_VERIFY_V2
//  policy provider specified in pgActionID:
//   - return TRUST_E_NOSIGNATURE if the subject isn't signed, has an
//     invalid signature or unable to find the signer certificate.
//     UI will never be displayed when not signed.
//   - ignore NO_CHECK revocation errors. Otherwise, continue to return
//     CERT_E_REVOCATION_FAILURE.
//   - search the code hash and publisher databases for the WTD_UI_NONE
//     dwUIChoice case. The default is to only search these databases when
//     UI has been enabled or user trust has been disabled.
//
//
//  Returns:
//          ERROR_SUCCESS               If the trust is authenticated or
//                                      if the user accepted the risk.
//
//          TRUST_E_PROVIDER_UNKNOWN    there was an error loading one of the
//                                      required Providers.
//
//          all error codes passed back are based on the Policy Provider used.
//
//  The following errors are returned when the
//  WINTRUST_ACTION_GENERIC_VERIFY_V2 policy provider is specified in
//  pgActionID:
//
//    TRUST_E_NOSIGNATURE (when WTD_SAFER_FLAG is set in dwProvFlags)
//      The subject isn't signed, has an invalid signature or unable
//      to find the signer certificate. All signature verification
//      errors will map to this error. Basically all errors except for
//      publisher or timestamp certificate verification.
//
//      Call GetLastError() to get the underlying reason for not having
//      a valid signature.
//
//      The following LastErrors indicate that the file doesn't have a
//      signature: TRUST_E_NOSIGNATURE, TRUST_E_SUBJECT_FORM_UNKNOWN or
//      TRUST_E_PROVIDER_UNKNOWN.
//
//      UI will never be displayed for this case.
//
//    TRUST_E_EXPLICIT_DISTRUST
//      Returned if the hash representing the subject is trusted as
//      AUTHZLEVELID_DISALLOWED or the publisher is in the "Disallowed"
//      store. Also returned if the publisher certificate is revoked.
//
//      UI will never be displayed for this case.
//
//    ERROR_SUCCESS
//      No UI unless noted below.
//
//      Returned for the following:
//       - Hash representing the subject is trusted as
//         AUTHZLEVELID_FULLYTRUSTED
//       - The publisher certificate exists in the
//         "TrustedPublisher" store and there weren't any verification errors.
//       - UI was enabled and the user clicked "Yes" when asked
//         to install and run the signed subject.
//       - UI was disabled. No publisher or timestamp chain error.
//
//    TRUST_E_SUBJECT_NOT_TRUSTED
//      UI was enabled and the the user clicked "No" when asked to install
//      and run the signed subject.
//
//    CRYPT_E_SECURITY_SETTINGS
//      The subject hash or publisher wasn't explicitly trusted and
//      user trust wasn't allowed in the safer authenticode flags.
//      No UI will be displayed for this case.
//
//      The subject is signed and its signature successfully
//      verified.
//
//    Any publisher or timestamp chain error. If WTD_SAFER_FLAG wasn't set in
//    dwProvFlags, any signed code verification error.
//
extern LONG WINAPI WinVerifyTrust(HWND hwnd, GUID *pgActionID,
                                  LPVOID pWVTData);


//////////////////////////////////////////////////////////////////////////////
//
// WinVerifyTrustEx
//----------------------------------------------------------------------------
//      *** DO NOT USE ***
//
//
extern HRESULT WINAPI WinVerifyTrustEx(HWND hwnd, GUID *pgActionID, 
                                       WINTRUST_DATA *pWinTrustData);


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//
//      Trust, Policy, and UI Provider definitions, typedefs, and prototypes
//
//  Model:
//      A client wishing to validate trust through WinVerifyTrust will
//      select an appropriate Action ID guid for the call.
//      This guid is defined by each Policy Provider and represents the
//      functions called based on the policy for the given object.
//
//      In this model, the Policy Provider determines which style of UI
//      will be shown to the user (this only applies to style, the
//      determination of whether UI is displayed is set by the calling client
//      in the UI flags member of WINTRUST_DATA).
//
//      Since the function entry points are common (same return value and
//      parameters), it allows Policy Provider developers to take advantage
//      of existing, generic, code to fill the CRYPT_PROVIDER_DATA structure.
//
//      This also allows the developer to simply add the specific policy they
//      need, then, call the generic Policy Provider - if appropriate.
//
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


//////////////////////////////////////////////////////////////////////////////
//
// Wintrust Policy Flags
//----------------------------------------------------------------------------
//  These are set during install and can be modified by the user
//  through various means.  The SETREG.EXE utility (found in the Authenticode
//  Tools Pack) will select/deselect each of them.
//
#define WTPF_TRUSTTEST              0x00000020  // trust any "TEST" certificate
#define WTPF_TESTCANBEVALID         0x00000080
#define WTPF_IGNOREEXPIRATION       0x00000100  // Use expiration date
#define WTPF_IGNOREREVOKATION       0x00000200  // Do revocation check
#define WTPF_OFFLINEOK_IND          0x00000400  // off-line is ok individual certs
#define WTPF_OFFLINEOK_COM          0x00000800  // off-line is ok commercial certs
#define WTPF_OFFLINEOKNBU_IND       0x00001000  // off-line is ok individual certs, no bad ui
#define WTPF_OFFLINEOKNBU_COM       0x00002000  // off-line is ok commercial certs, no bad ui
#define WTPF_VERIFY_V1_OFF          0x00010000  // turn verify of v1 certs off
#define WTPF_IGNOREREVOCATIONONTS   0x00020000  // ignore TimeStamp revocation checks
#define WTPF_ALLOWONLYPERTRUST      0x00040000  // allow only items in personal trust db.

//////////////////////////////////////////////////////////////////////////////
//
// WintrustGetRegPolicyFlags
//----------------------------------------------------------------------------
//  This API call is exported from WINTRUST.DLL and is the recommended method
//  of retrieving the DWORD representing the Policy Flags.
//
extern void WINAPI      WintrustGetRegPolicyFlags(DWORD *pdwPolicyFlags);

//////////////////////////////////////////////////////////////////////////////
//
// WintrustSetRegPolicyFlags
//----------------------------------------------------------------------------
//  This API call is exported from WINTRUST.DLL and is the recommended method
//  of setting the DWORD representing the Policy Flags.  MAKE SURE to call
//  WintrustGetRegPolicyFlags to get the current value and or/and the value
//  you need then call the set the flags.
//
extern BOOL WINAPI      WintrustSetRegPolicyFlags(DWORD dwPolicyFlags);



//////////////////////////////////////////////////////////////////////////////
//
// Trust Provider "Step" Error defines
//----------------------------------------------------------------------------
//  Each "step" of the Trust process has an error "slot" associated with it.
//  If an error occurs, the "step" will assign its result to this "slot".  These
//  errors can be any valid WINERROR.H HRESULT code.
//

    //
    //  step errors 0 through 20 are reserved for Authenticode specific.  If
    //  you are not calling any of the SOFTPUB.DLL (Authenticode) providers, you
    //  may use these as needed.
    //
#define TRUSTERROR_STEP_WVTPARAMS                   0
#define TRUSTERROR_STEP_FILEIO                      2
#define TRUSTERROR_STEP_SIP                         3
#define TRUSTERROR_STEP_SIPSUBJINFO                 5
#define TRUSTERROR_STEP_CATALOGFILE                 6
#define TRUSTERROR_STEP_CERTSTORE                   7
#define TRUSTERROR_STEP_MESSAGE                     8
#define TRUSTERROR_STEP_MSG_SIGNERCOUNT             9
#define TRUSTERROR_STEP_MSG_INNERCNTTYPE            10
#define TRUSTERROR_STEP_MSG_INNERCNT                11
#define TRUSTERROR_STEP_MSG_STORE                   12
#define TRUSTERROR_STEP_MSG_SIGNERINFO              13
#define TRUSTERROR_STEP_MSG_SIGNERCERT              14
#define TRUSTERROR_STEP_MSG_CERTCHAIN               15
#define TRUSTERROR_STEP_MSG_COUNTERSIGINFO          16
#define TRUSTERROR_STEP_MSG_COUNTERSIGCERT          17
#define TRUSTERROR_STEP_VERIFY_MSGHASH              18
#define TRUSTERROR_STEP_VERIFY_MSGINDIRECTDATA      19

    //
    //  step errors 30 through 37 are reserved for the ending error code for each
    //  entry point in the Trust Model.
    //
#define TRUSTERROR_STEP_FINAL_WVTINIT               30
#define TRUSTERROR_STEP_FINAL_INITPROV              31
#define TRUSTERROR_STEP_FINAL_OBJPROV               32
#define TRUSTERROR_STEP_FINAL_SIGPROV               33
#define TRUSTERROR_STEP_FINAL_CERTPROV              34
#define TRUSTERROR_STEP_FINAL_CERTCHKPROV           35
#define TRUSTERROR_STEP_FINAL_POLICYPROV            36
#define TRUSTERROR_STEP_FINAL_UIPROV                37

#define TRUSTERROR_MAX_STEPS                        38

//////////////////////////////////////////////////////////////////////////////
//
//  allocation and free function prototypes
//----------------------------------------------------------------------------
//
typedef void        *(*PFN_CPD_MEM_ALLOC)(IN DWORD cbSize);
typedef void        (*PFN_CPD_MEM_FREE)(IN void *pvMem2Free);

typedef BOOL        (*PFN_CPD_ADD_STORE)(IN struct _CRYPT_PROVIDER_DATA *pProvData,
                                         IN HCERTSTORE hStore2Add);

typedef BOOL        (*PFN_CPD_ADD_SGNR)(IN          struct _CRYPT_PROVIDER_DATA *pProvData,
                                        IN          BOOL fCounterSigner,
                                        IN OPTIONAL DWORD idxSigner,
                                        IN          struct _CRYPT_PROVIDER_SGNR *pSgnr2Add);

typedef BOOL        (*PFN_CPD_ADD_CERT)(IN          struct _CRYPT_PROVIDER_DATA *pProvData,
                                        IN          DWORD idxSigner,
                                        IN          BOOL fCounterSigner,
                                        IN OPTIONAL DWORD idxCounterSigner,
                                        IN          PCCERT_CONTEXT pCert2Add);

typedef BOOL        (*PFN_CPD_ADD_PRIVDATA)(IN struct _CRYPT_PROVIDER_DATA *pProvData,
                                            IN struct _CRYPT_PROVIDER_PRIVDATA *pPrivData2Add);

//////////////////////////////////////////////////////////////////////////////
//
//  Provider function prototypes
//----------------------------------------------------------------------------
//

//
//  entry point for the object provider
//
typedef HRESULT     (*PFN_PROVIDER_INIT_CALL)(IN OUT struct _CRYPT_PROVIDER_DATA *pProvData);

//
//  entry point for the object provider
//
typedef HRESULT     (*PFN_PROVIDER_OBJTRUST_CALL)(IN OUT struct _CRYPT_PROVIDER_DATA *pProvData);

//
//  entry point for the Signature Provider
//
typedef HRESULT     (*PFN_PROVIDER_SIGTRUST_CALL)(IN OUT struct _CRYPT_PROVIDER_DATA *pProvData);

//
//  entry point for the Certificate Provider
//
typedef HRESULT     (*PFN_PROVIDER_CERTTRUST_CALL)(IN OUT struct _CRYPT_PROVIDER_DATA *pProvData);

//
//  entry point for the Policy Provider's final call (from the trust provider)
//
typedef HRESULT     (*PFN_PROVIDER_FINALPOLICY_CALL)(IN OUT struct _CRYPT_PROVIDER_DATA *pProvData);

//
//  entry point for the Policy Provider's "dump structure" call
//
typedef HRESULT     (*PFN_PROVIDER_TESTFINALPOLICY_CALL)(IN OUT struct _CRYPT_PROVIDER_DATA *pProvData);

//
//  entry point for the Policy Provider's clean up routine for any PRIVDATA allocated
//
typedef HRESULT     (*PFN_PROVIDER_CLEANUP_CALL)(IN OUT struct _CRYPT_PROVIDER_DATA *pProvData);

//
//  entry point for the Policy Provider's Cert Check call.  This will return
//  true if the Trust Provider is to continue building the certificate chain.
//  If the PP returns FALSE, it is assumed that we have reached a "TRUSTED",
//  self-signed, root.  it is also the CertCheck's responsibility to set the
//  fTrustedRoot flag in the certificate structure.
//
typedef BOOL        (*PFN_PROVIDER_CERTCHKPOLICY_CALL)( IN          struct _CRYPT_PROVIDER_DATA *pProvData,
                                                        IN          DWORD idxSigner,
                                                        IN          BOOL fCounterSignerChain,
                                                        IN OPTIONAL DWORD idxCounterSigner);


#define WVT_OFFSETOF(t,f)   ((ULONG)((ULONG_PTR)(&((t*)0)->f)))

#define WVT_ISINSTRUCT(structtypedef, structpassedsize, member) \
                    ((WVT_OFFSETOF(structtypedef, member) < structpassedsize) ? TRUE : FALSE)


#define WVT_IS_CBSTRUCT_GT_MEMBEROFFSET(structtypedef, structpassedsize, member) \
                    WVT_ISINSTRUCT(structtypedef, structpassedsize, member)

#include <pshpack8.h>

//////////////////////////////////////////////////////////////////////////////
//
// CRYPT_PROVIDER_DATA Structure
//----------------------------------------------------------------------------
//  Used to pass information between WinVerifyTrust and all of the Provider
//  calls.
//
//  IMPORTANT:  1.  All dynamically allocated members MUST use the allocation
//                  and Add2 functions provided.
//
typedef struct _CRYPT_PROVIDER_DATA
{
    DWORD                               cbStruct;               // = sizeof(TRUST_PROVIDER_DATA) (set in WVT)

    WINTRUST_DATA                       *pWintrustData;         // NOT verified (set in WVT)
    BOOL                                fOpenedFile;            // the provider opened the file handle (if applicable)
    HWND                                hWndParent;             // if passed in, else, Desktop hWnd (set in WVT).
    GUID                                *pgActionID;            // represents the Provider combination (set in WVT).

    HCRYPTPROV                          hProv;                  // set to NULL to let CryptoAPI to assign.

    DWORD                               dwError;                // error if a low-level, system error was encountered

    DWORD                               dwRegSecuritySettings;  // ie security settings (set in WVT)
    DWORD                               dwRegPolicySettings;    // setreg settings (set in WVT)

    struct _CRYPT_PROVIDER_FUNCTIONS    *psPfns;                // set in WVT.

    DWORD                               cdwTrustStepErrors;     // set in WVT.
    DWORD                               *padwTrustStepErrors;   // allocated in WVT.  filled in WVT & Trust Provider

    DWORD                               chStores;               // number of stores in pahStores (root set in WVT)
    HCERTSTORE                          *pahStores;             // array of known stores (root set in WVT) root is ALWAYS #0!!!

    DWORD                               dwEncoding;             // message encoding type (set in WVT and Signature Prov)
    HCRYPTMSG                           hMsg;                   // set in Signature Prov.

    DWORD                               csSigners;              // use Add2 and Get functions!
    struct _CRYPT_PROVIDER_SGNR         *pasSigners;            // use Add2 and Get functions!

    DWORD                               csProvPrivData;         // use Add2 and Get functions!
    struct _CRYPT_PROVIDER_PRIVDATA     *pasProvPrivData;       // use Add2 and Get functions!

    DWORD                               dwSubjectChoice;
#                       define              CPD_CHOICE_SIP          1

    union
    {
        struct _PROVDATA_SIP            *pPDSip;
    };

    char                                *pszUsageOID;           // set in Init Provider

    // 03-Oct-1997 pberkman: added
    BOOL                                fRecallWithState;       // state was maintained for Catalog Files.

    // 10-Nov-1997 pberkman: added
    FILETIME                            sftSystemTime;

    // 16-Jan-1998 pberkman: added
    char                                *pszCTLSignerUsageOID;

    // 17-Feb-1998 philh: added
    // LOWORD intialized from WINTRUST_DATA's dwProvFlags.
    DWORD                               dwProvFlags;
#       define CPD_USE_NT5_CHAIN_FLAG                   0x80000000
#       define CPD_REVOCATION_CHECK_NONE                0x00010000
#       define CPD_REVOCATION_CHECK_END_CERT            0x00020000
#       define CPD_REVOCATION_CHECK_CHAIN               0x00040000
#       define CPD_REVOCATION_CHECK_CHAIN_EXCLUDE_ROOT  0x00080000

    // 24-Feb-1998 philh: added
    DWORD                               dwFinalError;

    // 20-May-1998 KeithV: added
    PCERT_USAGE_MATCH                                   pRequestUsage;

    // 02-Aug-2000 philh: added
    DWORD                               dwTrustPubSettings;

    // 02-Sep-2003 tonyschr: added    
    DWORD                               dwUIStateFlags;
#       define CPD_UISTATE_MODE_PROMPT                  0x00000000
#       define CPD_UISTATE_MODE_BLOCK                   0x00000001
#       define CPD_UISTATE_MODE_ALLOW                   0x00000002
#       define CPD_UISTATE_MODE_MASK                    0x00000003

} CRYPT_PROVIDER_DATA, *PCRYPT_PROVIDER_DATA;

//////////////////////////////////////////////////////////////////////////////
//
// CRYPT_PROVIDER_FUNCTIONS structure
//----------------------------------------------------------------------------
//
typedef struct _CRYPT_PROVIDER_FUNCTIONS
{
    DWORD                               cbStruct;

    PFN_CPD_MEM_ALLOC                   pfnAlloc;               // set in WVT
    PFN_CPD_MEM_FREE                    pfnFree;                // set in WVT

    PFN_CPD_ADD_STORE                   pfnAddStore2Chain;      // call to add a store to the chain.
    PFN_CPD_ADD_SGNR                    pfnAddSgnr2Chain;       // call to add a sgnr struct to a msg struct sgnr chain
    PFN_CPD_ADD_CERT                    pfnAddCert2Chain;       // call to add a cert struct to a sgnr struct cert chain
    PFN_CPD_ADD_PRIVDATA                pfnAddPrivData2Chain;   // call to add provider private data to struct.

    PFN_PROVIDER_INIT_CALL              pfnInitialize;          // initialize Policy data.
    PFN_PROVIDER_OBJTRUST_CALL          pfnObjectTrust;         // build info up to the signer info(s).
    PFN_PROVIDER_SIGTRUST_CALL          pfnSignatureTrust;      // build info to the signing cert
    PFN_PROVIDER_CERTTRUST_CALL         pfnCertificateTrust;    // build the chain
    PFN_PROVIDER_FINALPOLICY_CALL       pfnFinalPolicy;         // final call to policy
    PFN_PROVIDER_CERTCHKPOLICY_CALL     pfnCertCheckPolicy;     // check each cert will building chain
    PFN_PROVIDER_TESTFINALPOLICY_CALL   pfnTestFinalPolicy;     // dump structures to a file (or whatever the policy chooses)

    struct _CRYPT_PROVUI_FUNCS          *psUIpfns;

                    // 23-Jul-1997 pberkman: added
    PFN_PROVIDER_CLEANUP_CALL           pfnCleanupPolicy;       // PRIVDATA cleanup routine.

} CRYPT_PROVIDER_FUNCTIONS, *PCRYPT_PROVIDER_FUNCTIONS;

//////////////////////////////////////////////////////////////////////////////
//
// CRYPT_PROVUI_FUNCS structure
//----------------------------------------------------------------------------
//

typedef BOOL        (*PFN_PROVUI_CALL)(IN HWND hWndSecurityDialog, IN struct _CRYPT_PROVIDER_DATA *pProvData);

typedef struct _CRYPT_PROVUI_FUNCS
{
    DWORD                               cbStruct;

    struct _CRYPT_PROVUI_DATA           *psUIData;

    PFN_PROVUI_CALL                     pfnOnMoreInfoClick;
    PFN_PROVUI_CALL                     pfnOnMoreInfoClickDefault;

    PFN_PROVUI_CALL                     pfnOnAdvancedClick;
    PFN_PROVUI_CALL                     pfnOnAdvancedClickDefault;

} CRYPT_PROVUI_FUNCS, *PCRYPT_PROVUI_FUNCS;

//////////////////////////////////////////////////////////////////////////////
//
// CRYPT_PROVUI_DATA
//----------------------------------------------------------------------------
//
typedef struct _CRYPT_PROVUI_DATA
{
    DWORD                               cbStruct;

    DWORD                               dwFinalError;

    WCHAR                               *pYesButtonText;        // default: "&Yes"
    WCHAR                               *pNoButtonText;         // default: "&No"
    WCHAR                               *pMoreInfoButtonText;   // default: "&More Info"
    WCHAR                               *pAdvancedLinkText;     // default: <none>

    // 15-Sep-1997 pberkman: added
        // good: default:
                // "Do you want to install and run ""%1"" signed on %2 and distributed by:"
    WCHAR                               *pCopyActionText;
        // good no time stamp: default:
                // "Do you want to install and run ""%1"" signed on an unknown date/time and distributed by:"
    WCHAR                               *pCopyActionTextNoTS;
        // bad: default:
                // "Do you want to install and run ""%1""?"
    WCHAR                               *pCopyActionTextNotSigned;


} CRYPT_PROVUI_DATA, *PCRYPT_PROVUI_DATA;

//////////////////////////////////////////////////////////////////////////////
//
// CRYPT_PROVIDER_SGNR structure
//----------------------------------------------------------------------------
//  After the Signature Provider is finished there will be zero to many of these
//  filled out.  One for each signer of the message.  Also, there will be zero
//  to many of these filled out inside this structure.  One for each counter
//  signer of the signer.
//
//  IMPORTANT:  1.  All dynamically allocated members MUST use allocation
//                  and Add2 functions provided.
//
typedef struct _CRYPT_PROVIDER_SGNR
{
    DWORD                               cbStruct;

    FILETIME                            sftVerifyAsOf;      // either today's filetime or the timestamps

    DWORD                               csCertChain;        // use Add2 and Get functions!
    struct _CRYPT_PROVIDER_CERT         *pasCertChain;      // use Add2 and Get functions!

    DWORD                               dwSignerType;       // set if known by policy
#                                           define  SGNR_TYPE_TIMESTAMP     0x00000010

    CMSG_SIGNER_INFO                    *psSigner;          // must use the pfnAlloc allocator!

    DWORD                               dwError;            // error encounted while building/verifying the signer.

    DWORD                               csCounterSigners;   // use Add2 and Get functions!
    struct _CRYPT_PROVIDER_SGNR         *pasCounterSigners; // use Add2 and Get functions!

    // 11-Feb-1998 philh: added
    PCCERT_CHAIN_CONTEXT                pChainContext;

} CRYPT_PROVIDER_SGNR, *PCRYPT_PROVIDER_SGNR;

//////////////////////////////////////////////////////////////////////////////
//
// CRYPT_PROVIDER_CERT structure
//----------------------------------------------------------------------------
//  After the Signature and Certificate Providers are finished there will
//  be zero to many of these filled out in the CRYPT_PROVIDER_SGNR
//  structure.  One for each certificate in the chain.
//
//
typedef struct _CRYPT_PROVIDER_CERT
{
    DWORD                               cbStruct;

    PCCERT_CONTEXT                      pCert;              // must have its own ref-count!

    BOOL                                fCommercial;
    BOOL                                fTrustedRoot;       // certchk policy should set this.
    BOOL                                fSelfSigned;        // set in cert provider

    BOOL                                fTestCert;          // certchk policy will set

    DWORD                               dwRevokedReason;

    DWORD                               dwConfidence;       // set in the Certificate Provider
#                                           define  CERT_CONFIDENCE_SIG             0x10000000  // this cert
#                                           define  CERT_CONFIDENCE_TIME            0x01000000  // issuer cert
#                                           define  CERT_CONFIDENCE_TIMENEST        0x00100000  // this cert
#                                           define  CERT_CONFIDENCE_AUTHIDEXT       0x00010000  // this cert
#                                           define  CERT_CONFIDENCE_HYGIENE         0x00001000  // this cert
#                                           define  CERT_CONFIDENCE_HIGHEST         0x11111000

    DWORD                               dwError;

    CTL_CONTEXT                         *pTrustListContext;

    // 16-Jan-1998 pberkman: added
    BOOL                                fTrustListSignerCert;

    // 25-Feb-1998 philh: added
    //
    // The following two are only applicable to Self Signed certificates
    // residing in a CTL.
    PCCTL_CONTEXT                       pCtlContext;
    DWORD                               dwCtlError;

    // 12-Mar-1998 philh: added
    BOOL                                fIsCyclic;

    // 12-Oct-2000 DSIE: added
    PCERT_CHAIN_ELEMENT                 pChainElement;
} CRYPT_PROVIDER_CERT, *PCRYPT_PROVIDER_CERT;

//////////////////////////////////////////////////////////////////////////////
//
// CRYPT_PROVIDER_PRIVDATA structure
//----------------------------------------------------------------------------
//  This structure is to allow Policy Provider functions to share
//  POLICY SPECIFIC data between Policy Functions.
//  The Policy must use the pfnAddPrivateData2Chain function and
//  must free any data within the member before the Final Policy returns
//  to WVT.
//  To allow multiple providers to use this feature, each provider that
//  uses this member must set the provider ID to it's Action ID so that
//  the provider can find its data and ignore any other.
//
typedef struct _CRYPT_PROVIDER_PRIVDATA
{
    DWORD                               cbStruct;

    GUID                                gProviderID;

    DWORD                               cbProvData;
    void                                *pvProvData;

} CRYPT_PROVIDER_PRIVDATA, *PCRYPT_PROVIDER_PRIVDATA;

//////////////////////////////////////////////////////////////////////////////
//
// PROVDATA_SIP
//----------------------------------------------------------------------------
//
typedef struct _PROVDATA_SIP
{
    DWORD                               cbStruct;               // = sizeof(PROVDATA_SIP)

    GUID                                gSubject;               // subject guid of file/member file. (set in Sig Prov)

    struct SIP_DISPATCH_INFO_           *pSip;                  // set in Sig Prov - defined in sipbase.h
    struct SIP_DISPATCH_INFO_           *pCATSip;               // set in Sig Prov - defined in sipbase.h
    struct SIP_SUBJECTINFO_             *psSipSubjectInfo;      // set in Sig Prov - defined in sipbase.h
    struct SIP_SUBJECTINFO_             *psSipCATSubjectInfo;   // set in Sig Prov - defined in sipbase.h
    struct SIP_INDIRECT_DATA_           *psIndirectData;        // set in Sig Prov - defined in sipbase.h

} PROVDATA_SIP, *PPROVDATA_SIP;

//////////////////////////////////////////////////////////////////////////////
//
// structures used to register action IDs
//----------------------------------------------------------------------------
//
#define WT_CURRENT_VERSION                  0x00000200

typedef struct _CRYPT_TRUST_REG_ENTRY
{
    DWORD                               cbStruct;

    WCHAR                               *pwszDLLName;
    WCHAR                               *pwszFunctionName;  // no more than WT_MAX_FUNC_NAME!

} CRYPT_TRUST_REG_ENTRY, *PCRYPT_TRUST_REG_ENTRY;

typedef struct _CRYPT_REGISTER_ACTIONID
{
    DWORD                               cbStruct;

    CRYPT_TRUST_REG_ENTRY               sInitProvider;
    CRYPT_TRUST_REG_ENTRY               sObjectProvider;
    CRYPT_TRUST_REG_ENTRY               sSignatureProvider;
    CRYPT_TRUST_REG_ENTRY               sCertificateProvider;
    CRYPT_TRUST_REG_ENTRY               sCertificatePolicyProvider;
    CRYPT_TRUST_REG_ENTRY               sFinalPolicyProvider;
    CRYPT_TRUST_REG_ENTRY               sTestPolicyProvider;

                    // 23-Jul-1997 pberkman: added
    CRYPT_TRUST_REG_ENTRY               sCleanupProvider;

} CRYPT_REGISTER_ACTIONID, *PCRYPT_REGISTER_ACTIONID;

typedef BOOL (*PFN_ALLOCANDFILLDEFUSAGE)(IN const char *pszUsageOID,
                                         IN struct _CRYPT_PROVIDER_DEFUSAGE *psDefUsage);
typedef BOOL (*PFN_FREEDEFUSAGE)(IN const char *pszUsageOID,
                                 IN struct _CRYPT_PROVIDER_DEFUSAGE *psDefUsage);

typedef struct _CRYPT_PROVIDER_REGDEFUSAGE
{
    DWORD                   cbStruct;   // = sizeof CRYPT_PROVIDER_REGDEFUSAGE

    GUID                    *pgActionID;

    WCHAR                   *pwszDllName;
    char                    *pwszLoadCallbackDataFunctionName;
    char                    *pwszFreeCallbackDataFunctionName;

} CRYPT_PROVIDER_REGDEFUSAGE, *PCRYPT_PROVIDER_REGDEFUSAGE;

typedef struct _CRYPT_PROVIDER_DEFUSAGE
{
    DWORD                   cbStruct;               // = sizeof CRYPT_PROVIDER_DEFUSAGE

    GUID                    gActionID;            // ActionID of provider

    LPVOID                  pDefPolicyCallbackData; // normally filled in WINTRUST_DATA
    LPVOID                  pDefSIPClientData;      // normally filled in WINTRUST_DATA

} CRYPT_PROVIDER_DEFUSAGE, *PCRYPT_PROVIDER_DEFUSAGE;

#include <poppack.h>

//////////////////////////////////////////////////////////////////////////////
//
// WINTRUST.DLL Provider defines
//----------------------------------------------------------------------------
//  The following are definitions of the Microsoft Generic Cert Provider
//
#define WT_PROVIDER_DLL_NAME                L"WINTRUST.DLL"
#define WT_PROVIDER_CERTTRUST_FUNCTION      L"WintrustCertificateTrust"

//////////////////////////////////////////////////////////////////////////////
//
// WintrustAddActionID
//----------------------------------------------------------------------------
//  Adds a new Provider combination to the users'
//  system.  Creates all necessary registry entries, etc.  This should be done
//  during the Policy Provider's DllRegisterServer.
//
//  *** THE ONLY ONE WHO SHOULD CALL THIS IS THE POLICY PROVIDER ***
//
// Returns:
//      TRUE:                           No fatal errors
//      FALSE:                          Errors occured.  See GetLastError()
//
extern BOOL WINAPI  WintrustAddActionID(IN GUID *pgActionID,
                                        IN DWORD fdwFlags,
                                        IN CRYPT_REGISTER_ACTIONID *psProvInfo);

// By default, WintrustAddActionID doesn't return registry errors.
// Set this flag to return registry errors. If FALSE is returned,
// LastError is set.
#define WT_ADD_ACTION_ID_RET_RESULT_FLAG    0x1


//////////////////////////////////////////////////////////////////////////////
//
// WintrustRemoveActionID
//----------------------------------------------------------------------------
//  Removes the Provider action combination from the users'
//  system.
//
// Returns:
//      TRUE:                           No fatal errors
//      FALSE:                          Errors occured.  See GetLastError()
//
extern BOOL WINAPI  WintrustRemoveActionID(IN GUID *pgActionID);

//////////////////////////////////////////////////////////////////////////////
//
// WintrustLoadFunctionPointers
//----------------------------------------------------------------------------
//  Retrieves the function entry points based on the Action ID given.
//
// Returns:
//      TRUE                            success.
//      FALSE                           fail.
//
extern BOOL WINAPI WintrustLoadFunctionPointers(GUID *pgActionID, CRYPT_PROVIDER_FUNCTIONS *pPfns);


//////////////////////////////////////////////////////////////////////////////
//
// WintrustAddDefaultForUsage
//----------------------------------------------------------------------------
//  Sets the default Action ID for the usage.  If the provider uses this
//  function, and the provider requires any of the "callback" data in
//  WINTRUST_DATA to be filled out, it MUST completely fill out the
//  CRYPT_PROVIDER_REGDEFUSAGE structure.
//
// Returns:
//      TRUE                            success.
//      FALSE                           fail.
//
extern BOOL WINAPI              WintrustAddDefaultForUsage(IN const char *pszUsageOID,
                                                           IN CRYPT_PROVIDER_REGDEFUSAGE *psDefUsage);

//////////////////////////////////////////////////////////////////////////////
//
// WintrustGetDefaultForUsage
//----------------------------------------------------------------------------
//  Retrieves the Action ID and default callback data for the specified usage
//
//  this function must be called again with dwAction set to FREE to deallocate
//
//
// Returns:
//      TRUE                            success.
//      FALSE                           fail.
//
#define                             DWACTION_ALLOCANDFILL           1
#define                             DWACTION_FREE                   2
extern BOOL WINAPI              WintrustGetDefaultForUsage(IN DWORD dwAction,
                                                           IN const char *pszUsageOID,
                                                           IN OUT CRYPT_PROVIDER_DEFUSAGE *psUsage);

extern CRYPT_PROVIDER_SGNR * WINAPI     WTHelperGetProvSignerFromChain(CRYPT_PROVIDER_DATA *pProvData,
                                                                       DWORD idxSigner,
                                                                       BOOL fCounterSigner,
                                                                       DWORD idxCounterSigner);
extern CRYPT_PROVIDER_CERT * WINAPI     WTHelperGetProvCertFromChain(CRYPT_PROVIDER_SGNR *pSgnr,
                                                                     DWORD idxCert);

extern CRYPT_PROVIDER_DATA * WINAPI     WTHelperProvDataFromStateData(HANDLE hStateData);

extern CRYPT_PROVIDER_PRIVDATA * WINAPI WTHelperGetProvPrivateDataFromChain(CRYPT_PROVIDER_DATA *pProvData,
                                                                            GUID *pgProviderID);
extern BOOL WINAPI                      WTHelperCertIsSelfSigned(DWORD dwEncoding, CERT_INFO *pCert);

extern HRESULT WINAPI                   WTHelperCertCheckValidSignature(CRYPT_PROVIDER_DATA *pProvData);


//////////////////////////////////////////////////////////////////////////////
//
// Supported ASN structures contained in WINTRUST.DLL
//----------------------------------------------------------------------------
//
#include <pshpack8.h>

//
//  CTL Trusted CA Lists
//
#define szOID_TRUSTED_CODESIGNING_CA_LIST   "1.3.6.1.4.1.311.2.2.1"
#define szOID_TRUSTED_CLIENT_AUTH_CA_LIST   "1.3.6.1.4.1.311.2.2.2"
#define szOID_TRUSTED_SERVER_AUTH_CA_LIST   "1.3.6.1.4.1.311.2.2.3"

//
//  encode/decode OID defines
//
#define SPC_COMMON_NAME_OBJID               szOID_COMMON_NAME
#define SPC_TIME_STAMP_REQUEST_OBJID        "1.3.6.1.4.1.311.3.2.1"
#define SPC_INDIRECT_DATA_OBJID             "1.3.6.1.4.1.311.2.1.4"
#define SPC_SP_AGENCY_INFO_OBJID            "1.3.6.1.4.1.311.2.1.10"
#define SPC_STATEMENT_TYPE_OBJID            "1.3.6.1.4.1.311.2.1.11"
#define SPC_SP_OPUS_INFO_OBJID              "1.3.6.1.4.1.311.2.1.12"
#define SPC_CERT_EXTENSIONS_OBJID           "1.3.6.1.4.1.311.2.1.14"
#define SPC_PE_IMAGE_DATA_OBJID             "1.3.6.1.4.1.311.2.1.15"
#define SPC_RAW_FILE_DATA_OBJID             "1.3.6.1.4.1.311.2.1.18"
#define SPC_STRUCTURED_STORAGE_DATA_OBJID   "1.3.6.1.4.1.311.2.1.19"
#define SPC_JAVA_CLASS_DATA_OBJID           "1.3.6.1.4.1.311.2.1.20"
#define SPC_INDIVIDUAL_SP_KEY_PURPOSE_OBJID "1.3.6.1.4.1.311.2.1.21"
#define SPC_COMMERCIAL_SP_KEY_PURPOSE_OBJID "1.3.6.1.4.1.311.2.1.22"
#define SPC_CAB_DATA_OBJID                  "1.3.6.1.4.1.311.2.1.25"
#define SPC_GLUE_RDN_OBJID                  "1.3.6.1.4.1.311.2.1.25"    // obsolete!
#define SPC_MINIMAL_CRITERIA_OBJID          "1.3.6.1.4.1.311.2.1.26"
#define SPC_FINANCIAL_CRITERIA_OBJID        "1.3.6.1.4.1.311.2.1.27"
#define SPC_LINK_OBJID                      "1.3.6.1.4.1.311.2.1.28"
#define SPC_SIGINFO_OBJID                   "1.3.6.1.4.1.311.2.1.30"

#define CAT_NAMEVALUE_OBJID                 "1.3.6.1.4.1.311.12.2.1"
#define CAT_MEMBERINFO_OBJID                "1.3.6.1.4.1.311.12.2.2"


//
//  encode/decode internal defines
//
#define SPC_SP_AGENCY_INFO_STRUCT           ((LPCSTR) 2000)
#define SPC_MINIMAL_CRITERIA_STRUCT         ((LPCSTR) 2001)
#define SPC_FINANCIAL_CRITERIA_STRUCT       ((LPCSTR) 2002)
#define SPC_INDIRECT_DATA_CONTENT_STRUCT    ((LPCSTR) 2003)
#define SPC_PE_IMAGE_DATA_STRUCT            ((LPCSTR) 2004)
#define SPC_LINK_STRUCT                     ((LPCSTR) 2005)
#define SPC_STATEMENT_TYPE_STRUCT           ((LPCSTR) 2006)
#define SPC_SP_OPUS_INFO_STRUCT             ((LPCSTR) 2007)
#define SPC_CAB_DATA_STRUCT                 ((LPCSTR) 2008)
#define SPC_JAVA_CLASS_DATA_STRUCT          ((LPCSTR) 2009)

#define SPC_SIGINFO_STRUCT                  ((LPCSTR) 2130)

#define CAT_NAMEVALUE_STRUCT                ((LPCSTR) 2221)
#define CAT_MEMBERINFO_STRUCT               ((LPCSTR) 2222)


#define SPC_UUID_LENGTH     16
typedef BYTE SPC_UUID[SPC_UUID_LENGTH];

typedef struct _SPC_SERIALIZED_OBJECT
{
    SPC_UUID            ClassId;
    CRYPT_DATA_BLOB     SerializedData;

} SPC_SERIALIZED_OBJECT, *PSPC_SERIALIZED_OBJECT;

typedef struct SPC_SIGINFO_
{
    DWORD       dwSipVersion;
    GUID        gSIPGuid;
    DWORD       dwReserved1;
    DWORD       dwReserved2;
    DWORD       dwReserved3;
    DWORD       dwReserved4;
    DWORD       dwReserved5;

} SPC_SIGINFO, *PSPC_SIGINFO;

typedef struct SPC_LINK_
{
    DWORD dwLinkChoice;
#               define          SPC_URL_LINK_CHOICE         1
#               define          SPC_MONIKER_LINK_CHOICE     2
#               define          SPC_FILE_LINK_CHOICE        3

    union
    {
        LPWSTR                  pwszUrl;
        SPC_SERIALIZED_OBJECT   Moniker;
        LPWSTR                  pwszFile;
    };

} SPC_LINK, *PSPC_LINK;

typedef struct _SPC_PE_IMAGE_DATA
{
    CRYPT_BIT_BLOB            Flags;
    PSPC_LINK                 pFile;

} SPC_PE_IMAGE_DATA, *PSPC_PE_IMAGE_DATA;

typedef struct _SPC_INDIRECT_DATA_CONTENT
{
    CRYPT_ATTRIBUTE_TYPE_VALUE  Data;
    CRYPT_ALGORITHM_IDENTIFIER  DigestAlgorithm;
    CRYPT_HASH_BLOB             Digest;

} SPC_INDIRECT_DATA_CONTENT, *PSPC_INDIRECT_DATA_CONTENT;

typedef struct _SPC_FINANCIAL_CRITERIA
{
    BOOL                        fFinancialInfoAvailable;
    BOOL                        fMeetsCriteria;

} SPC_FINANCIAL_CRITERIA, *PSPC_FINANCIAL_CRITERIA;

typedef struct _SPC_IMAGE
{
    struct SPC_LINK_            *pImageLink;
    CRYPT_DATA_BLOB             Bitmap;
    CRYPT_DATA_BLOB             Metafile;
    CRYPT_DATA_BLOB             EnhancedMetafile;
    CRYPT_DATA_BLOB             GifFile;

} SPC_IMAGE, *PSPC_IMAGE;

typedef struct _SPC_SP_AGENCY_INFO
{
    struct SPC_LINK_            *pPolicyInformation;
    LPWSTR                      pwszPolicyDisplayText;
    PSPC_IMAGE                  pLogoImage;
    struct SPC_LINK_            *pLogoLink;

} SPC_SP_AGENCY_INFO, *PSPC_SP_AGENCY_INFO;

typedef struct _SPC_STATEMENT_TYPE
{
    DWORD                       cKeyPurposeId;
    LPSTR                       *rgpszKeyPurposeId;     // pszObjId

} SPC_STATEMENT_TYPE, *PSPC_STATEMENT_TYPE;

typedef struct _SPC_SP_OPUS_INFO
{
    LPCWSTR                     pwszProgramName;
    struct SPC_LINK_            *pMoreInfo;
    struct SPC_LINK_            *pPublisherInfo;

} SPC_SP_OPUS_INFO, *PSPC_SP_OPUS_INFO;

typedef struct _CAT_NAMEVALUE
{
    LPWSTR          pwszTag;
    DWORD           fdwFlags;
    CRYPT_DATA_BLOB Value;

} CAT_NAMEVALUE, *PCAT_NAMEVALUE;

typedef struct _CAT_MEMBERINFO
{
    LPWSTR          pwszSubjGuid;
    DWORD           dwCertVersion;

} CAT_MEMBERINFO, *PCAT_MEMBERINFO;

#include <poppack.h>



//////////////////////////////////////////////////////////////////////////////////
//
//  support for old calling convention: *** DO NOT USE ***
//
#ifdef WT_DEFINE_ALL_APIS

typedef struct _WIN_CERTIFICATE
{
    DWORD       dwLength;
    WORD        wRevision;
    WORD        wCertificateType;   // WIN_CERT_TYPE_xxx
    BYTE        bCertificate[ANYSIZE_ARRAY];

} WIN_CERTIFICATE, *LPWIN_CERTIFICATE;

#define WIN_CERT_REVISION_1_0               (0x0100)
#define WIN_CERT_REVISION_2_0               (0x0200)

#define WIN_CERT_TYPE_X509                  (0x0001)   // bCertificate contains an X.509 Certificate
#define WIN_CERT_TYPE_PKCS_SIGNED_DATA      (0x0002)   // bCertificate contains a PKCS SignedData structure
#define WIN_CERT_TYPE_RESERVED_1            (0x0003)   // Reserved
#define WIN_CERT_TYPE_TS_STACK_SIGNED       (0x0004)   // Terminal Server Protocol Stack Certificate signing


typedef LPVOID WIN_TRUST_SUBJECT;

typedef struct _WIN_TRUST_ACTDATA_CONTEXT_WITH_SUBJECT
{
    HANDLE            hClientToken;
    GUID *            SubjectType;
    WIN_TRUST_SUBJECT Subject;

} WIN_TRUST_ACTDATA_CONTEXT_WITH_SUBJECT, *LPWIN_TRUST_ACTDATA_CONTEXT_WITH_SUBJECT ;


typedef struct _WIN_TRUST_ACTDATA_SUBJECT_ONLY
{
    GUID *            SubjectType;
    WIN_TRUST_SUBJECT Subject;

} WIN_TRUST_ACTDATA_SUBJECT_ONLY, *LPWIN_TRUST_ACTDATA_SUBJECT_ONLY;

/* RawFile == 959dc450-8d9e-11cf-8736-00aa00a485eb */
#define WIN_TRUST_SUBJTYPE_RAW_FILE                              \
            { 0x959dc450,                                        \
              0x8d9e,                                            \
              0x11cf,                                            \
              {0x87, 0x36, 0x00, 0xaa, 0x00, 0xa4, 0x85, 0xeb}   \
            }

/* PeImage == 43c9a1e0-8da0-11cf-8736-00aa00a485eb */
#define WIN_TRUST_SUBJTYPE_PE_IMAGE                              \
            { 0x43c9a1e0,                                        \
              0x8da0,                                            \
              0x11cf,                                            \
              {0x87, 0x36, 0x00, 0xaa, 0x00, 0xa4, 0x85, 0xeb}   \
            }


/* JavaClass = 08ad3990-8da1-11cf-8736-00aa00a485eb */
#define WIN_TRUST_SUBJTYPE_JAVA_CLASS                            \
            { 0x08ad3990,                                        \
              0x8da1,                                            \
              0x11cf,                                            \
              {0x87, 0x36, 0x00, 0xaa, 0x00, 0xa4, 0x85, 0xeb}   \
            }
/* Cabinet = d17c5374-a392-11cf-9df5-00aa00c184e0 */
#define WIN_TRUST_SUBJTYPE_CABINET                               \
            { 0xd17c5374,                                        \
              0xa392,                                            \
              0x11cf,                                            \
              { 0x9d, 0xf5, 0x0, 0xaa, 0x0, 0xc1, 0x84, 0xe0 }   \
            }

typedef struct _WIN_TRUST_SUBJECT_FILE
{
    HANDLE  hFile;
    LPCWSTR lpPath;

} WIN_TRUST_SUBJECT_FILE, *LPWIN_TRUST_SUBJECT_FILE;

#define WIN_TRUST_SUBJTYPE_RAW_FILEEX                            \
            { 0x6f458110,                                        \
              0xc2f1,                                            \
              0x11cf,                                            \
              { 0x8a, 0x69, 0x0, 0xaa, 0x0, 0x6c, 0x37, 0x6 }    \
            }

#define WIN_TRUST_SUBJTYPE_PE_IMAGEEX                            \
            { 0x6f458111,                                        \
              0xc2f1,                                            \
              0x11cf,                                            \
              { 0x8a, 0x69, 0x0, 0xaa, 0x0, 0x6c, 0x37, 0x6 }    \
            }

#define WIN_TRUST_SUBJTYPE_JAVA_CLASSEX                          \
            { 0x6f458113,                                        \
              0xc2f1,                                            \
              0x11cf,                                            \
              { 0x8a, 0x69, 0x0, 0xaa, 0x0, 0x6c, 0x37, 0x6 }    \
            }

#define WIN_TRUST_SUBJTYPE_CABINETEX                             \
            { 0x6f458114,                                        \
              0xc2f1,                                            \
              0x11cf,                                            \
              { 0x8a, 0x69, 0x0, 0xaa, 0x0, 0x6c, 0x37, 0x6 }    \
            }

typedef struct _WIN_TRUST_SUBJECT_FILE_AND_DISPLAY
{
    HANDLE  hFile;              // handle to the open file if you got it
    LPCWSTR lpPath;             // the path to open if you don't
    LPCWSTR lpDisplayName;      // (optional) display name to show to user

} WIN_TRUST_SUBJECT_FILE_AND_DISPLAY, *LPWIN_TRUST_SUBJECT_FILE_AND_DISPLAY;

/* OleStorage == c257e740-8da0-11cf-8736-00aa00a485eb */
#define WIN_TRUST_SUBJTYPE_OLE_STORAGE                           \
            { 0xc257e740,                                        \
              0x8da0,                                            \
              0x11cf,                                            \
              {0x87, 0x36, 0x00, 0xaa, 0x00, 0xa4, 0x85, 0xeb}   \
            }


/* TrustedPublisher == 66426730-8da1-11cf-8736-00aa00a485eb */
#define WIN_SPUB_ACTION_TRUSTED_PUBLISHER                        \
            { 0x66426730,                                        \
              0x8da1,                                            \
              0x11cf,                                            \
              {0x87, 0x36, 0x00, 0xaa, 0x00, 0xa4, 0x85, 0xeb}   \
            }

/* NtActivateImage == 8bc96b00-8da1-11cf-8736-00aa00a485eb */
#define     WIN_SPUB_ACTION_NT_ACTIVATE_IMAGE                    \
            { 0x8bc96b00,                                        \
              0x8da1,                                            \
              0x11cf,                                            \
              {0x87, 0x36, 0x00, 0xaa, 0x00, 0xa4, 0x85, 0xeb}   \
            }

/* PublishedSoftware == 64b9d180-8da2-11cf-8736-00aa00a485eb */
#define WIN_SPUB_ACTION_PUBLISHED_SOFTWARE                       \
            { 0x64b9d180,                                        \
              0x8da2,                                            \
              0x11cf,                                            \
              {0x87, 0x36, 0x00, 0xaa, 0x00, 0xa4, 0x85, 0xeb}   \
            }

typedef struct _WIN_SPUB_TRUSTED_PUBLISHER_DATA
{
    HANDLE            hClientToken;
    LPWIN_CERTIFICATE lpCertificate;

} WIN_SPUB_TRUSTED_PUBLISHER_DATA, *LPWIN_SPUB_TRUSTED_PUBLISHER_DATA;

#endif



#ifdef __cplusplus
}
#endif

#endif // WINTRUST_H

