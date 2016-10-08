/*++ BUILD Version: 0000     Increment this if a change has global effects

Copyright (c) Microsoft Corporation. All rights reserved.

Module Name:

    ntsecapi.h

Abstract:

    This module defines the Local Security Authority APIs.

Revision History:

--*/

#ifndef _NTSECAPI_
#define _NTSECAPI_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _NTDEF_
typedef LONG NTSTATUS, *PNTSTATUS;
#endif

#ifndef _NTLSA_IFS_
// begin_ntifs


//
// Security operation mode of the system is held in a control
// longword.
//

typedef ULONG  LSA_OPERATIONAL_MODE, *PLSA_OPERATIONAL_MODE;

// end_ntifs
#endif // _NTLSA_IFS_

//
// The flags in the security operational mode are defined
// as:
//
//    PasswordProtected - Some level of authentication (such as
//        a password) must be provided by users before they are
//        allowed to use the system.  Once set, this value will
//        not be cleared without re-booting the system.
//
//    IndividualAccounts - Each user must identify an account to
//        logon to.  This flag is only meaningful if the
//        PasswordProtected flag is also set.  If this flag is
//        not set and the PasswordProtected flag is set, then all
//        users may logon to the same account.  Once set, this value
//        will not be cleared without re-booting the system.
//
//    MandatoryAccess - Indicates the system is running in a mandatory
//        access control mode (e.g., B-level as defined by the U.S.A's
//        Department of Defense's "Orange Book").  This is not utilized
//        in the current release of NT.  This flag is only meaningful
//        if both the PasswordProtected and IndividualAccounts flags are
//        set.  Once set, this value will not be cleared without
//        re-booting the system.
//
//    LogFull - Indicates the system has been brought up in a mode in
//        which if must perform security auditing, but its audit log
//        is full.  This may (should) restrict the operations that
//        can occur until the audit log is made not-full again.  THIS
//        VALUE MAY BE CLEARED WHILE THE SYSTEM IS RUNNING (I.E., WITHOUT
//        REBOOTING).
//
// If the PasswordProtected flag is not set, then the system is running
// without security, and user interface should be adjusted appropriately.
//

#define LSA_MODE_PASSWORD_PROTECTED     (0x00000001L)
#define LSA_MODE_INDIVIDUAL_ACCOUNTS    (0x00000002L)
#define LSA_MODE_MANDATORY_ACCESS       (0x00000004L)
#define LSA_MODE_LOG_FULL               (0x00000008L)

#ifndef _NTLSA_IFS_
// begin_ntifs
//
// Used by a logon process to indicate what type of logon is being
// requested.
//

typedef enum _SECURITY_LOGON_TYPE {
    Interactive = 2,    // Interactively logged on (locally or remotely)
    Network,            // Accessing system via network
    Batch,              // Started via a batch queue
    Service,            // Service started by service controller
    Proxy,              // Proxy logon
    Unlock,             // Unlock workstation
    NetworkCleartext,   // Network logon with cleartext credentials
    NewCredentials,     // Clone caller, new default credentials
    RemoteInteractive,  // Remote, yet interactive. Terminal server
    CachedInteractive,  // Try cached credentials without hitting the net.
    CachedRemoteInteractive, // Same as RemoteInteractive, this is used internally for auditing purpose
    CachedUnlock        // Cached Unlock workstation
} SECURITY_LOGON_TYPE, *PSECURITY_LOGON_TYPE;

// end_ntifs
#endif // _NTLSA_IFS_

#ifndef _NTLSA_IFS_
// begin_ntifs

#ifndef _NTLSA_AUDIT_
#define _NTLSA_AUDIT_

/////////////////////////////////////////////////////////////////////////
//                                                                     //
// Data types related to Auditing                                      //
//                                                                     //
/////////////////////////////////////////////////////////////////////////


//
// The following enumerated type is used between the reference monitor and
// LSA in the generation of audit messages.  It is used to indicate the
// type of data being passed as a parameter from the reference monitor
// to LSA.  LSA is responsible for transforming the specified data type
// into a set of unicode strings that are added to the event record in
// the audit log.
//

typedef enum _SE_ADT_PARAMETER_TYPE {

    SeAdtParmTypeNone = 0,          //Produces 1 parameter
                                    //Received value:
                                    //
                                    //  None.
                                    //
                                    //Results in:
                                    //
                                    //  a unicode string containing "-".
                                    //
                                    //Note:  This is typically used to
                                    //       indicate that a parameter value
                                    //       was not available.
                                    //

    SeAdtParmTypeString,            //Produces 1 parameter.
                                    //Received Value:
                                    //
                                    //  Unicode String (variable length)
                                    //
                                    //Results in:
                                    //
                                    //  No transformation.  The string
                                    //  entered into the event record as
                                    //  received.
                                    //
                                    // The Address value of the audit info
                                    // should be a pointer to a UNICODE_STRING
                                    // structure.



    SeAdtParmTypeFileSpec,          //Produces 1 parameter.
                                    //Received value:
                                    //
                                    //  Unicode string containing a file or
                                    //  directory name.
                                    //
                                    //Results in:
                                    //
                                    //  Unicode string with the prefix of the
                                    //  file's path replaced by a drive letter
                                    //  if possible.
                                    //




    SeAdtParmTypeUlong,             //Produces 1 parameter
                                    //Received value:
                                    //
                                    //  Ulong
                                    //
                                    //Results in:
                                    //
                                    //  Unicode string representation of
                                    //  unsigned integer value.


    SeAdtParmTypeSid,               //Produces 1 parameter.
                                    //Received value:
                                    //
                                    //  SID (variable length)
                                    //
                                    //Results in:
                                    //
                                    //  String representation of SID
                                    //




    SeAdtParmTypeLogonId,           //Produces 3 parameters.
                                    //Received Value:
                                    //
                                    //  LUID (fixed length)
                                    //
                                    //Results in:
                                    //
                                    //  param 1: Username string
                                    //  param 2: domain name string
                                    //  param 3: Logon ID (Luid) string


    SeAdtParmTypeNoLogonId,         //Produces 3 parameters.
                                    //Received value:
                                    //
                                    //  None.
                                    //
                                    //Results in:
                                    //
                                    //  param 1: "-"
                                    //  param 2: "-"
                                    //  param 3: "-"
                                    //
                                    //Note:
                                    //
                                    //  This type is used when a logon ID
                                    //  is needed, but one is not available
                                    //  to pass.  For example, if an
                                    //  impersonation logon ID is expected
                                    //  but the subject is not impersonating
                                    //  anyone.
                                    //

    SeAdtParmTypeAccessMask,        //Produces 1 parameter with formatting.
                                    //Received value:
                                    //
                                    //  ACCESS_MASK followed by
                                    //  a Unicode string.  The unicode
                                    //  string contains the name of the
                                    //  type of object the access mask
                                    //  applies to.  The event's source
                                    //  further qualifies the object type.
                                    //
                                    //Results in:
                                    //
                                    //  formatted unicode string built to
                                    //  take advantage of the specified
                                    //  source's parameter message file.
                                    //
                                    //Note:
                                    //
                                    //  An access mask containing three
                                    //  access types for a Widget object
                                    //  type (defined by the Foozle source)
                                    //  might end up looking like:
                                    //
                                    //      %%1062\n\t\t%1066\n\t\t%%601
                                    //
                                    //  The %%numbers are signals to the
                                    //  event viewer to perform parameter
                                    //  substitution before display.
                                    //



    SeAdtParmTypePrivs,             //Produces 1 parameter with formatting.
                                    //Received value:
                                    //
                                    //  ??? Check with RobertRe and ScottBi
                                    //
                                    //Results in:
                                    //
                                    //  formatted unicode string similar to
                                    //  that for access types.  Each priv
                                    //  will be formatted to be displayed
                                    //  on its own line.  E.g.,
                                    //
                                    //      %%642\n\t\t%%651\n\t\t%%655
                                    //

    SeAdtParmTypeObjectTypes,       //Produces 10 parameters with formatting.
                                    //Received value:
                                    //
                                    // Produces a list a stringized GUIDS along
                                    // with information similar to that for
                                    // an access mask.

    SeAdtParmTypeHexUlong,          //Produces 1 parameter
                                    //Received value:
                                    //
                                    //  Ulong
                                    //
                                    //Results in:
                                    //
                                    //  Unicode string representation of
                                    //  unsigned integer value in hexadecimal.

    SeAdtParmTypePtr,               //Produces 1 parameter
                                    //Received value:
                                    //
                                    //  pointer
                                    //
                                    //Results in:
                                    //
                                    //  Unicode string representation of
                                    //  unsigned integer value in hexadecimal.

    SeAdtParmTypeTime,              //Produces 2 parameters
                                    //Received value:
                                    //
                                    //  LARGE_INTEGER
                                    //
                                    //Results in:
                                    //
                                    // Unicode string representation of
                                    // date and time.

                                    //
    SeAdtParmTypeGuid,              //Produces 1 parameter
                                    //Received value:
                                    //
                                    //  GUID pointer
                                    //
                                    //Results in:
                                    //
                                    // Unicode string representation of GUID
                                    // {xxxxxxxx-xxxx-xxxx-xxxx-xxxxxxxxxxxx}
                                    //

    SeAdtParmTypeLuid,              //
                                    //Produces 1 parameter
                                    //Received value:
                                    //
                                    // LUID
                                    //
                                    //Results in:
                                    //
                                    // Hex LUID
                                    //

    SeAdtParmTypeHexInt64,          //Produces 1 parameter
                                    //Received value:
                                    //
                                    //  64 bit integer
                                    //
                                    //Results in:
                                    //
                                    //  Unicode string representation of
                                    //  unsigned integer value in hexadecimal.

    SeAdtParmTypeStringList,        //Produces 1 parameter
                                    //Received value:
                                    //
                                    // ptr to LSAP_ADT_STRING_LIST
                                    //
                                    //Results in:
                                    //
                                    // Unicode string representation of
                                    // concatenation of the strings in the list

    SeAdtParmTypeSidList,           //Produces 1 parameter
                                    //Received value:
                                    //
                                    // ptr to LSAP_ADT_SID_LIST
                                    //
                                    //Results in:
                                    //
                                    // Unicode string representation of
                                    // concatenation of the SIDs in the list

    SeAdtParmTypeDuration,          //Produces 1 parameters
                                    //Received value:
                                    //
                                    //  LARGE_INTEGER
                                    //
                                    //Results in:
                                    //
                                    // Unicode string representation of
                                    // a duration.

    SeAdtParmTypeUserAccountControl,//Produces 3 parameters
                                    //Received value:
                                    //
                                    // old and new UserAccountControl values
                                    //
                                    //Results in:
                                    //
                                    // Unicode string representations of
                                    // the flags in UserAccountControl.
                                    // 1 - old value in hex
                                    // 2 - new value in hex
                                    // 3 - difference as strings

    SeAdtParmTypeNoUac,             //Produces 3 parameters
                                    //Received value:
                                    //
                                    // none
                                    //
                                    //Results in:
                                    //
                                    // Three dashes ('-') as unicode strings.

    SeAdtParmTypeMessage,           //Produces 1 Parameter
                                    //Received value:
                                    //
                                    //  ULONG (MessageNo from msobjs.mc)
                                    //
                                    //Results in:
                                    //
                                    // Unicode string representation of
                                    // %%MessageNo which the event viewer
                                    // will replace with the message string
                                    // from msobjs.mc

    SeAdtParmTypeDateTime,          //Produces 1 Parameter
                                    //Received value:
                                    //
                                    //  LARGE_INTEGER
                                    //
                                    //Results in:
                                    //
                                    // Unicode string representation of
                                    // date and time (in _one_ string).

    SeAdtParmTypeSockAddr           // Produces 2 parameters
                                    //
                                    // Received value:
                                    //
                                    // pointer to SOCKADDR_IN/SOCKADDR_IN6
                                    // structure
                                    //
                                    // Results in:
                                    //
                                    // param 1: IP address string
                                    // param 2: Port number string
                                    // 


} SE_ADT_PARAMETER_TYPE, *PSE_ADT_PARAMETER_TYPE;

#ifndef GUID_DEFINED
#include <guiddef.h>
#endif /* GUID_DEFINED */

typedef struct _SE_ADT_OBJECT_TYPE {
    GUID ObjectType;
    USHORT Flags;
#define SE_ADT_OBJECT_ONLY 0x1
    USHORT Level;
    ACCESS_MASK AccessMask;
} SE_ADT_OBJECT_TYPE, *PSE_ADT_OBJECT_TYPE;

typedef struct _SE_ADT_PARAMETER_ARRAY_ENTRY {

    SE_ADT_PARAMETER_TYPE Type;
    ULONG Length;
    ULONG_PTR Data[2];
    PVOID Address;

} SE_ADT_PARAMETER_ARRAY_ENTRY, *PSE_ADT_PARAMETER_ARRAY_ENTRY;

//
// Structure that will be passed between the Reference Monitor and LSA
// to transmit auditing information.
//

#define SE_MAX_AUDIT_PARAMETERS 32
#define SE_MAX_GENERIC_AUDIT_PARAMETERS 28

typedef struct _SE_ADT_PARAMETER_ARRAY {

    ULONG CategoryId;
    ULONG AuditId;
    ULONG ParameterCount;
    ULONG Length;
    USHORT Type;
    ULONG Flags;
    SE_ADT_PARAMETER_ARRAY_ENTRY Parameters[ SE_MAX_AUDIT_PARAMETERS ];

} SE_ADT_PARAMETER_ARRAY, *PSE_ADT_PARAMETER_ARRAY;

#define SE_ADT_PARAMETERS_SELF_RELATIVE     0x00000001

#endif // _NTLSA_AUDIT_

// end_ntifs
#endif // _NTLSA_IFS_

//
// Audit Event Categories
//
// The following are the built-in types or Categories of audit event.
// WARNING!  This structure is subject to expansion.  The user should not
// compute the number of elements of this type directly, but instead
// should obtain the count of elements by calling LsaQueryInformationPolicy()
// for the PolicyAuditEventsInformation class and extracting the count from
// the MaximumAuditEventCount field of the returned structure.
//

typedef enum _POLICY_AUDIT_EVENT_TYPE {

    AuditCategorySystem,
    AuditCategoryLogon,
    AuditCategoryObjectAccess,
    AuditCategoryPrivilegeUse,
    AuditCategoryDetailedTracking,
    AuditCategoryPolicyChange,
    AuditCategoryAccountManagement,
    AuditCategoryDirectoryServiceAccess,
    AuditCategoryAccountLogon

} POLICY_AUDIT_EVENT_TYPE, *PPOLICY_AUDIT_EVENT_TYPE;


//
// The following defines describe the auditing options for each
// event type
//

// Leave options specified for this event unchanged

#define POLICY_AUDIT_EVENT_UNCHANGED       (0x00000000L)

// Audit successful occurrences of events of this type

#define POLICY_AUDIT_EVENT_SUCCESS         (0x00000001L)

// Audit failed attempts to cause an event of this type to occur

#define POLICY_AUDIT_EVENT_FAILURE         (0x00000002L)

#define POLICY_AUDIT_EVENT_NONE            (0x00000004L)

// Mask of valid event auditing options

#define POLICY_AUDIT_EVENT_MASK \
    (POLICY_AUDIT_EVENT_SUCCESS | \
     POLICY_AUDIT_EVENT_FAILURE | \
     POLICY_AUDIT_EVENT_UNCHANGED | \
     POLICY_AUDIT_EVENT_NONE)


#ifdef _NTDEF_
// begin_ntifs
typedef UNICODE_STRING LSA_UNICODE_STRING, *PLSA_UNICODE_STRING;
typedef STRING LSA_STRING, *PLSA_STRING;
typedef OBJECT_ATTRIBUTES LSA_OBJECT_ATTRIBUTES, *PLSA_OBJECT_ATTRIBUTES;
// end_ntifs
#else // _NTDEF_

#ifndef IN
#define IN
#endif

#ifndef OUT
#define OUT
#endif

#ifndef OPTIONAL
#define OPTIONAL
#endif


typedef struct _LSA_UNICODE_STRING {
    USHORT Length;
    USHORT MaximumLength;
#ifdef MIDL_PASS
    [size_is(MaximumLength/2), length_is(Length/2)]
#endif // MIDL_PASS
    PWSTR  Buffer;
} LSA_UNICODE_STRING, *PLSA_UNICODE_STRING;

typedef struct _LSA_STRING {
    USHORT Length;
    USHORT MaximumLength;
    PCHAR Buffer;
} LSA_STRING, *PLSA_STRING;

typedef struct _LSA_OBJECT_ATTRIBUTES {
    ULONG Length;
    HANDLE RootDirectory;
    PLSA_UNICODE_STRING ObjectName;
    ULONG Attributes;
    PVOID SecurityDescriptor;        // Points to type SECURITY_DESCRIPTOR
    PVOID SecurityQualityOfService;  // Points to type SECURITY_QUALITY_OF_SERVICE
} LSA_OBJECT_ATTRIBUTES, *PLSA_OBJECT_ATTRIBUTES;



#endif // _NTDEF_

//
// Macro for determining whether an API succeeded.
//

#define LSA_SUCCESS(Error) ((LONG)(Error) >= 0)

#ifndef _NTLSA_IFS_
// begin_ntifs

NTSTATUS
NTAPI
LsaRegisterLogonProcess (
    __in PLSA_STRING LogonProcessName,
    __out PHANDLE LsaHandle,
    __out PLSA_OPERATIONAL_MODE SecurityMode
    );


NTSTATUS
NTAPI
LsaLogonUser (
    __in HANDLE LsaHandle,
    __in PLSA_STRING OriginName,
    __in SECURITY_LOGON_TYPE LogonType,
    __in ULONG AuthenticationPackage,
    __in_bcount(AuthenticationInformationLength) PVOID AuthenticationInformation,
    __in ULONG AuthenticationInformationLength,
    __in_opt PTOKEN_GROUPS LocalGroups,
    __in PTOKEN_SOURCE SourceContext,
    __out PVOID *ProfileBuffer,
    __out PULONG ProfileBufferLength,
    __out PLUID LogonId,
    __out PHANDLE Token,
    __out PQUOTA_LIMITS Quotas,
    __out PNTSTATUS SubStatus
    );


// end_ntifs

NTSTATUS
NTAPI
LsaLookupAuthenticationPackage (
    __in HANDLE LsaHandle,
    __in PLSA_STRING PackageName,
    __out PULONG AuthenticationPackage
    );

// begin_ntifs

NTSTATUS
NTAPI
LsaFreeReturnBuffer (
    __in PVOID Buffer
    );

// end_ntifs

NTSTATUS
NTAPI
LsaCallAuthenticationPackage (
    __in HANDLE LsaHandle,
    __in ULONG AuthenticationPackage,
    __in_bcount(SubmitBufferLength) PVOID ProtocolSubmitBuffer,
    __in ULONG SubmitBufferLength,
    __out_opt PVOID *ProtocolReturnBuffer,
    __out_opt PULONG ReturnBufferLength,
    __out_opt PNTSTATUS ProtocolStatus
    );


NTSTATUS
NTAPI
LsaDeregisterLogonProcess (
    __in HANDLE LsaHandle
    );

NTSTATUS
NTAPI
LsaConnectUntrusted (
    __out PHANDLE LsaHandle
    );

#endif // _NTLSA_IFS_


////////////////////////////////////////////////////////////////////////////
//                                                                        //
// Local Security Policy Administration API datatypes and defines         //
//                                                                        //
////////////////////////////////////////////////////////////////////////////

//
// Access types for the Policy object
//

#define POLICY_VIEW_LOCAL_INFORMATION              0x00000001L
#define POLICY_VIEW_AUDIT_INFORMATION              0x00000002L
#define POLICY_GET_PRIVATE_INFORMATION             0x00000004L
#define POLICY_TRUST_ADMIN                         0x00000008L
#define POLICY_CREATE_ACCOUNT                      0x00000010L
#define POLICY_CREATE_SECRET                       0x00000020L
#define POLICY_CREATE_PRIVILEGE                    0x00000040L
#define POLICY_SET_DEFAULT_QUOTA_LIMITS            0x00000080L
#define POLICY_SET_AUDIT_REQUIREMENTS              0x00000100L
#define POLICY_AUDIT_LOG_ADMIN                     0x00000200L
#define POLICY_SERVER_ADMIN                        0x00000400L
#define POLICY_LOOKUP_NAMES                        0x00000800L
#define POLICY_NOTIFICATION                        0x00001000L

#define POLICY_ALL_ACCESS     (STANDARD_RIGHTS_REQUIRED         |\
                               POLICY_VIEW_LOCAL_INFORMATION    |\
                               POLICY_VIEW_AUDIT_INFORMATION    |\
                               POLICY_GET_PRIVATE_INFORMATION   |\
                               POLICY_TRUST_ADMIN               |\
                               POLICY_CREATE_ACCOUNT            |\
                               POLICY_CREATE_SECRET             |\
                               POLICY_CREATE_PRIVILEGE          |\
                               POLICY_SET_DEFAULT_QUOTA_LIMITS  |\
                               POLICY_SET_AUDIT_REQUIREMENTS    |\
                               POLICY_AUDIT_LOG_ADMIN           |\
                               POLICY_SERVER_ADMIN              |\
                               POLICY_LOOKUP_NAMES)


#define POLICY_READ           (STANDARD_RIGHTS_READ             |\
                               POLICY_VIEW_AUDIT_INFORMATION    |\
                               POLICY_GET_PRIVATE_INFORMATION)

#define POLICY_WRITE          (STANDARD_RIGHTS_WRITE            |\
                               POLICY_TRUST_ADMIN               |\
                               POLICY_CREATE_ACCOUNT            |\
                               POLICY_CREATE_SECRET             |\
                               POLICY_CREATE_PRIVILEGE          |\
                               POLICY_SET_DEFAULT_QUOTA_LIMITS  |\
                               POLICY_SET_AUDIT_REQUIREMENTS    |\
                               POLICY_AUDIT_LOG_ADMIN           |\
                               POLICY_SERVER_ADMIN)

#define POLICY_EXECUTE        (STANDARD_RIGHTS_EXECUTE          |\
                               POLICY_VIEW_LOCAL_INFORMATION    |\
                               POLICY_LOOKUP_NAMES)


//
// Policy object specific data types.
//

//
// The following data type is used to identify a domain
//

typedef struct _LSA_TRUST_INFORMATION {

    LSA_UNICODE_STRING Name;
    PSID Sid;

} LSA_TRUST_INFORMATION, *PLSA_TRUST_INFORMATION;

// where members have the following usage:
//
//     Name - The name of the domain.
//
//     Sid - A pointer to the Sid of the Domain
//

//
// The following data type is used in name and SID lookup services to
// describe the domains referenced in the lookup operation.
//

typedef struct _LSA_REFERENCED_DOMAIN_LIST {

    ULONG Entries;
    PLSA_TRUST_INFORMATION Domains;

} LSA_REFERENCED_DOMAIN_LIST, *PLSA_REFERENCED_DOMAIN_LIST;

// where members have the following usage:
//
//     Entries - Is a count of the number of domains described in the
//         Domains array.
//
//     Domains - Is a pointer to an array of Entries LSA_TRUST_INFORMATION data
//         structures.
//


//
// The following data type is used in name to SID lookup services to describe
// the domains referenced in the lookup operation.
//

typedef struct _LSA_TRANSLATED_SID {

    SID_NAME_USE Use;
    ULONG RelativeId;
    LONG DomainIndex;

} LSA_TRANSLATED_SID, *PLSA_TRANSLATED_SID;

// where members have the following usage:
//
//     Use - identifies the use of the SID.  If this value is SidUnknown or
//         SidInvalid, then the remainder of the record is not set and
//         should be ignored.
//
//     RelativeId - Contains the relative ID of the translated SID.  The
//         remainder of the SID (the prefix) is obtained using the
//         DomainIndex field.
//
//     DomainIndex - Is the index of an entry in a related
//         LSA_REFERENCED_DOMAIN_LIST data structure describing the
//         domain in which the account was found.
//
//         If there is no corresponding reference domain for an entry, then
//         this field will contain a negative value.
//

typedef struct _LSA_TRANSLATED_SID2 {

    SID_NAME_USE Use;
    PSID         Sid;
    LONG         DomainIndex;
    ULONG        Flags;

} LSA_TRANSLATED_SID2, *PLSA_TRANSLATED_SID2;

// where members have the following usage:
//
//     Use - identifies the use of the SID.  If this value is SidUnknown or
//         SidInvalid, then the remainder of the record is not set and
//         should be ignored.
//
//     Sid - Contains the complete Sid of the tranlated SID
//
//     DomainIndex - Is the index of an entry in a related
//         LSA_REFERENCED_DOMAIN_LIST data structure describing the
//         domain in which the account was found.
//
//         If there is no corresponding reference domain for an entry, then
//         this field will contain a negative value.
//

//
// The following data type is used in SID to name lookup services to
// describe the domains referenced in the lookup operation.
//

typedef struct _LSA_TRANSLATED_NAME {

    SID_NAME_USE Use;
    LSA_UNICODE_STRING Name;
    LONG DomainIndex;

} LSA_TRANSLATED_NAME, *PLSA_TRANSLATED_NAME;

// where the members have the following usage:
//
//     Use - Identifies the use of the name.  If this value is SidUnknown
//         or SidInvalid, then the remainder of the record is not set and
//         should be ignored.  If this value is SidWellKnownGroup then the
//         Name field is invalid, but the DomainIndex field is not.
//
//     Name - Contains the isolated name of the translated SID.
//
//     DomainIndex - Is the index of an entry in a related
//         LSA_REFERENCED_DOMAIN_LIST data structure describing the domain
//         in which the account was found.
//
//         If there is no corresponding reference domain for an entry, then
//         this field will contain a negative value.
//


//
// The following data type is used to represent the role of the LSA
// server (primary or backup).
//

typedef enum _POLICY_LSA_SERVER_ROLE {

    PolicyServerRoleBackup = 2,
    PolicyServerRolePrimary

} POLICY_LSA_SERVER_ROLE, *PPOLICY_LSA_SERVER_ROLE;

//
// The following data type is used to specify the auditing options for
// an Audit Event Type.
//

typedef ULONG POLICY_AUDIT_EVENT_OPTIONS, *PPOLICY_AUDIT_EVENT_OPTIONS;

// where the following flags can be set:
//
//     POLICY_AUDIT_EVENT_UNCHANGED - Leave existing auditing options
//         unchanged for events of this type.  This flag is only used for
//         set operations.  If this flag is set, then all other flags
//         are ignored.
//
//     POLICY_AUDIT_EVENT_NONE - Cancel all auditing options for events
//         of this type.  If this flag is set, the success/failure flags
//         are ignored.
//
//     POLICY_AUDIT_EVENT_SUCCESS - When auditing is enabled, audit all
//         successful occurrences of events of the given type.
//
//     POLICY_AUDIT_EVENT_FAILURE - When auditing is enabled, audit all
//         unsuccessful occurrences of events of the given type.
//




//
// The following data type defines the classes of Policy Information
// that may be queried/set.
//

typedef enum _POLICY_INFORMATION_CLASS {

    PolicyAuditLogInformation = 1,
    PolicyAuditEventsInformation,
    PolicyPrimaryDomainInformation,
    PolicyPdAccountInformation,
    PolicyAccountDomainInformation,
    PolicyLsaServerRoleInformation,
    PolicyReplicaSourceInformation,
    PolicyDefaultQuotaInformation,
    PolicyModificationInformation,
    PolicyAuditFullSetInformation,
    PolicyAuditFullQueryInformation,
    PolicyDnsDomainInformation,
    PolicyDnsDomainInformationInt

} POLICY_INFORMATION_CLASS, *PPOLICY_INFORMATION_CLASS;


//
// The following data type corresponds to the PolicyAuditLogInformation
// information class.  It is used to represent information relating to
// the Audit Log.
//
// This structure may be used in both query and set operations.  However,
// when used in set operations, some fields are ignored.
//

typedef struct _POLICY_AUDIT_LOG_INFO {

    ULONG AuditLogPercentFull;
    ULONG MaximumLogSize;
    LARGE_INTEGER AuditRetentionPeriod;
    BOOLEAN AuditLogFullShutdownInProgress;
    LARGE_INTEGER TimeToShutdown;
    ULONG NextAuditRecordId;

} POLICY_AUDIT_LOG_INFO, *PPOLICY_AUDIT_LOG_INFO;

// where the members have the following usage:
//
//     AuditLogPercentFull - Indicates the percentage of the Audit Log
//         currently being used.
//
//     MaximumLogSize - Specifies the maximum size of the Audit Log in
//         kilobytes.
//
//     AuditRetentionPeriod - Indicates the length of time that Audit
//         Records are to be retained.  Audit Records are discardable
//         if their timestamp predates the current time minus the
//         retention period.
//
//     AuditLogFullShutdownInProgress - Indicates whether or not a system
//         shutdown is being initiated due to the security Audit Log becoming
//         full.  This condition will only occur if the system is configured
//         to shutdown when the log becomes full.
//
//         TRUE indicates that a shutdown is in progress
//         FALSE indicates that a shutdown is not in progress.
//
//         Once a shutdown has been initiated, this flag will be set to
//         TRUE.  If an administrator is able to currect the situation
//         before the shutdown becomes irreversible, then this flag will
//         be reset to false.
//
//         This field is ignored for set operations.
//
//     TimeToShutdown - If the AuditLogFullShutdownInProgress flag is set,
//         then this field contains the time left before the shutdown
//         becomes irreversible.
//
//         This field is ignored for set operations.
//


//
// The following data type corresponds to the PolicyAuditEventsInformation
// information class.  It is used to represent information relating to
// the audit requirements.
//

typedef struct _POLICY_AUDIT_EVENTS_INFO {

    BOOLEAN AuditingMode;
    PPOLICY_AUDIT_EVENT_OPTIONS EventAuditingOptions;
    ULONG MaximumAuditEventCount;

} POLICY_AUDIT_EVENTS_INFO, *PPOLICY_AUDIT_EVENTS_INFO;

// where the members have the following usage:
//
//     AuditingMode - A Boolean variable specifying the Auditing Mode value.
//         This value is interpreted as follows:
//
//         TRUE - Auditing is to be enabled (set operations) or is enabled
//             (query operations).  Audit Records will be generated according
//             to the Event Auditing Options in effect (see the
//             EventAuditingOptions field.
//
//         FALSE - Auditing is to be disabled (set operations) or is
//             disabled (query operations).  No Audit Records will be
//             generated.  Note that for set operations the Event Auditing
//             Options in effect will still be updated as specified by the
//             EventAuditingOptions field whether Auditing is enabled or
//             disabled.
//
//    EventAuditingOptions - Pointer to an array of Auditing Options
//        indexed by Audit Event Type.
//
//    MaximumAuditEventCount - Specifiesa count of the number of Audit
//        Event Types specified by the EventAuditingOptions parameter.  If
//        this count is less than the number of Audit Event Types supported
//        by the system, the Auditing Options for Event Types with IDs
//        higher than (MaximumAuditEventCount + 1) are left unchanged.
//


//
// The following structure corresponds to the PolicyAccountDomainInformation
// information class.
//

typedef struct _POLICY_ACCOUNT_DOMAIN_INFO {

    LSA_UNICODE_STRING DomainName;
    PSID DomainSid;

} POLICY_ACCOUNT_DOMAIN_INFO, *PPOLICY_ACCOUNT_DOMAIN_INFO;

// where the members have the following usage:
//
//     DomainName - Is the name of the domain
//
//     DomainSid - Is the Sid of the domain
//


//
// The following structure corresponds to the PolicyPrimaryDomainInformation
// information class.
//

typedef struct _POLICY_PRIMARY_DOMAIN_INFO {

    LSA_UNICODE_STRING Name;
    PSID Sid;

} POLICY_PRIMARY_DOMAIN_INFO, *PPOLICY_PRIMARY_DOMAIN_INFO;

// where the members have the following usage:
//
//     Name - Is the name of the domain
//
//     Sid - Is the Sid of the domain
//


//
// The following structure corresponds to the PolicyDnsDomainInformation
// information class
//

typedef struct _POLICY_DNS_DOMAIN_INFO
{
    LSA_UNICODE_STRING Name;
    LSA_UNICODE_STRING DnsDomainName;
    LSA_UNICODE_STRING DnsForestName;
    GUID DomainGuid;
    PSID Sid;

} POLICY_DNS_DOMAIN_INFO, *PPOLICY_DNS_DOMAIN_INFO;

// where the members have the following usage:
//
//      Name - Is the name of the Domain
//
//      DnsDomainName - Is the DNS name of the domain
//
//      DnsForestName - Is the DNS forest name of the domain
//
//      DomainGuid - Is the GUID of the domain
//
//      Sid - Is the Sid of the domain


//
// The following structure corresponds to the PolicyPdAccountInformation
// information class.  This structure may be used in Query operations
// only.
//

typedef struct _POLICY_PD_ACCOUNT_INFO {

    LSA_UNICODE_STRING Name;

} POLICY_PD_ACCOUNT_INFO, *PPOLICY_PD_ACCOUNT_INFO;

// where the members have the following usage:
//
//     Name - Is the name of an account in the domain that should be used
//         for authentication and name/ID lookup requests.
//


//
// The following structure corresponds to the PolicyLsaServerRoleInformation
// information class.
//

typedef struct _POLICY_LSA_SERVER_ROLE_INFO {

    POLICY_LSA_SERVER_ROLE LsaServerRole;

} POLICY_LSA_SERVER_ROLE_INFO, *PPOLICY_LSA_SERVER_ROLE_INFO;

// where the fields have the following usage:
//
// TBS
//


//
// The following structure corresponds to the PolicyReplicaSourceInformation
// information class.
//

typedef struct _POLICY_REPLICA_SOURCE_INFO {

    LSA_UNICODE_STRING ReplicaSource;
    LSA_UNICODE_STRING ReplicaAccountName;

} POLICY_REPLICA_SOURCE_INFO, *PPOLICY_REPLICA_SOURCE_INFO;


//
// The following structure corresponds to the PolicyDefaultQuotaInformation
// information class.
//

typedef struct _POLICY_DEFAULT_QUOTA_INFO {

    QUOTA_LIMITS QuotaLimits;

} POLICY_DEFAULT_QUOTA_INFO, *PPOLICY_DEFAULT_QUOTA_INFO;


//
// The following structure corresponds to the PolicyModificationInformation
// information class.
//

typedef struct _POLICY_MODIFICATION_INFO {

    LARGE_INTEGER ModifiedId;
    LARGE_INTEGER DatabaseCreationTime;

} POLICY_MODIFICATION_INFO, *PPOLICY_MODIFICATION_INFO;

// where the members have the following usage:
//
//     ModifiedId - Is a 64-bit unsigned integer that is incremented each
//         time anything in the LSA database is modified.  This value is
//         only modified on Primary Domain Controllers.
//
//     DatabaseCreationTime - Is the date/time that the LSA Database was
//         created.  On Backup Domain Controllers, this value is replicated
//         from the Primary Domain Controller.
//

//
// The following structure type corresponds to the PolicyAuditFullSetInformation
// Information Class.
//

typedef struct _POLICY_AUDIT_FULL_SET_INFO {

    BOOLEAN ShutDownOnFull;

} POLICY_AUDIT_FULL_SET_INFO, *PPOLICY_AUDIT_FULL_SET_INFO;

//
// The following structure type corresponds to the PolicyAuditFullQueryInformation
// Information Class.
//

typedef struct _POLICY_AUDIT_FULL_QUERY_INFO {

    BOOLEAN ShutDownOnFull;
    BOOLEAN LogIsFull;

} POLICY_AUDIT_FULL_QUERY_INFO, *PPOLICY_AUDIT_FULL_QUERY_INFO;

//
// The following data type defines the classes of Policy Information
// that may be queried/set that has domain wide effect.
//

typedef enum _POLICY_DOMAIN_INFORMATION_CLASS {

//  PolicyDomainQualityOfServiceInformation, // value was used in W2K; no longer supported
    PolicyDomainEfsInformation = 2,
    PolicyDomainKerberosTicketInformation

} POLICY_DOMAIN_INFORMATION_CLASS, *PPOLICY_DOMAIN_INFORMATION_CLASS;

//
// The following structure corresponds to the PolicyEfsInformation
// information class
//

typedef struct _POLICY_DOMAIN_EFS_INFO {

    ULONG   InfoLength;
    PUCHAR  EfsBlob;

} POLICY_DOMAIN_EFS_INFO, *PPOLICY_DOMAIN_EFS_INFO;

//
// where the members have the following usage:
//
//      InfoLength - Length of the EFS Information blob
//
//      EfsBlob - Efs blob data
//


//
// The following structure corresponds to the PolicyDomainKerberosTicketInformation
// information class
//

#define POLICY_KERBEROS_VALIDATE_CLIENT 0x00000080


typedef struct _POLICY_DOMAIN_KERBEROS_TICKET_INFO {

    ULONG AuthenticationOptions;
    LARGE_INTEGER MaxServiceTicketAge;
    LARGE_INTEGER MaxTicketAge;
    LARGE_INTEGER MaxRenewAge;
    LARGE_INTEGER MaxClockSkew;
    LARGE_INTEGER Reserved;
} POLICY_DOMAIN_KERBEROS_TICKET_INFO, *PPOLICY_DOMAIN_KERBEROS_TICKET_INFO;

//
// where the members have the following usage
//
//      AuthenticationOptions -- allowed ticket options (POLICY_KERBEROS_* flags )
//
//      MaxServiceTicketAge   -- Maximum lifetime for a service ticket
//
//      MaxTicketAge -- Maximum lifetime for the initial ticket
//
//      MaxRenewAge -- Maximum cumulative age a renewable ticket can be with
//                     requring authentication
//
//      MaxClockSkew -- Maximum tolerance for synchronization of computer clocks
//
//      Reserved   --  Reserved


//
// The following data type defines the classes of Policy Information / Policy Domain Information
// that may be used to request notification
//

typedef enum _POLICY_NOTIFICATION_INFORMATION_CLASS {

    PolicyNotifyAuditEventsInformation = 1,
    PolicyNotifyAccountDomainInformation,
    PolicyNotifyServerRoleInformation,
    PolicyNotifyDnsDomainInformation,
    PolicyNotifyDomainEfsInformation,
    PolicyNotifyDomainKerberosTicketInformation,
    PolicyNotifyMachineAccountPasswordInformation

} POLICY_NOTIFICATION_INFORMATION_CLASS, *PPOLICY_NOTIFICATION_INFORMATION_CLASS;


//
// LSA RPC Context Handle (Opaque form).  Note that a Context Handle is
// always a pointer type unlike regular handles.
//

typedef PVOID LSA_HANDLE, *PLSA_HANDLE;


//
// Trusted Domain Object specific data types
//

//
// This data type defines the following information classes that may be
// queried or set.
//

typedef enum _TRUSTED_INFORMATION_CLASS {

    TrustedDomainNameInformation = 1,
    TrustedControllersInformation,
    TrustedPosixOffsetInformation,
    TrustedPasswordInformation,
    TrustedDomainInformationBasic,
    TrustedDomainInformationEx,
    TrustedDomainAuthInformation,
    TrustedDomainFullInformation,
    TrustedDomainAuthInformationInternal,
    TrustedDomainFullInformationInternal,
    TrustedDomainInformationEx2Internal,
    TrustedDomainFullInformation2Internal,

} TRUSTED_INFORMATION_CLASS, *PTRUSTED_INFORMATION_CLASS;

//
// The following data type corresponds to the TrustedDomainNameInformation
// information class.
//

typedef struct _TRUSTED_DOMAIN_NAME_INFO {

    LSA_UNICODE_STRING Name;

} TRUSTED_DOMAIN_NAME_INFO, *PTRUSTED_DOMAIN_NAME_INFO;

// where members have the following meaning:
//
// Name - The name of the Trusted Domain.
//

//
// The following data type corresponds to the TrustedControllersInformation
// information class.
//

typedef struct _TRUSTED_CONTROLLERS_INFO {

    ULONG Entries;
    PLSA_UNICODE_STRING Names;

} TRUSTED_CONTROLLERS_INFO, *PTRUSTED_CONTROLLERS_INFO;

// where members have the following meaning:
//
// Entries - Indicate how mamy entries there are in the Names array.
//
// Names - Pointer to an array of LSA_UNICODE_STRING structures containing the
//     names of domain controllers of the domain.  This information may not
//     be accurate and should be used only as a hint.  The order of this
//     list is considered significant and will be maintained.
//
//     By convention, the first name in this list is assumed to be the
//     Primary Domain Controller of the domain.  If the Primary Domain
//     Controller is not known, the first name should be set to the NULL
//     string.
//


//
// The following data type corresponds to the TrustedPosixOffsetInformation
// information class.
//

typedef struct _TRUSTED_POSIX_OFFSET_INFO {

    ULONG Offset;

} TRUSTED_POSIX_OFFSET_INFO, *PTRUSTED_POSIX_OFFSET_INFO;

// where members have the following meaning:
//
// Offset - Is an offset to use for the generation of Posix user and group
//     IDs from SIDs.  The Posix ID corresponding to any particular SID is
//     generated by adding the RID of that SID to the Offset of the SID's
//     corresponding TrustedDomain object.
//

//
// The following data type corresponds to the TrustedPasswordInformation
// information class.
//

typedef struct _TRUSTED_PASSWORD_INFO {
    LSA_UNICODE_STRING Password;
    LSA_UNICODE_STRING OldPassword;
} TRUSTED_PASSWORD_INFO, *PTRUSTED_PASSWORD_INFO;


typedef  LSA_TRUST_INFORMATION TRUSTED_DOMAIN_INFORMATION_BASIC;

typedef PLSA_TRUST_INFORMATION PTRUSTED_DOMAIN_INFORMATION_BASIC;

//
// Direction of the trust
//
#define TRUST_DIRECTION_DISABLED        0x00000000
#define TRUST_DIRECTION_INBOUND         0x00000001
#define TRUST_DIRECTION_OUTBOUND        0x00000002
#define TRUST_DIRECTION_BIDIRECTIONAL   (TRUST_DIRECTION_INBOUND | TRUST_DIRECTION_OUTBOUND)

#define TRUST_TYPE_DOWNLEVEL            0x00000001  // NT4 and before
#define TRUST_TYPE_UPLEVEL              0x00000002  // NT5
#define TRUST_TYPE_MIT                  0x00000003  // Trust with a MIT Kerberos realm
// #define TRUST_TYPE_DCE                  0x00000004  // Trust with a DCE realm
// Levels 0x5 - 0x000FFFFF reserved for future use
// Provider specific trust levels are from 0x00100000 to 0xFFF00000

#define TRUST_ATTRIBUTE_NON_TRANSITIVE     0x00000001  // Disallow transitivity
#define TRUST_ATTRIBUTE_UPLEVEL_ONLY       0x00000002  // Trust link only valid for uplevel client
#define TRUST_ATTRIBUTE_QUARANTINED_DOMAIN 0x00000004  // Used to quarantine domains
#define TRUST_ATTRIBUTE_FOREST_TRANSITIVE  0x00000008  // This link may contain forest trust information
#define TRUST_ATTRIBUTE_CROSS_ORGANIZATION 0x00000010  // This trust is to a domain/forest which is not part of this enterprise
#define TRUST_ATTRIBUTE_WITHIN_FOREST      0x00000020  // Trust is internal to this forest
#define TRUST_ATTRIBUTE_TREAT_AS_EXTERNAL  0x00000040  // Trust is to be treated as external for trust boundary purposes
#define TRUST_ATTRIBUTE_TRUST_USES_RC4_ENCRYPTION     0x00000080 // MIT trust with RC4
// Trust attributes 0x00000040 through 0x00200000 are reserved for future use
// Trust attributes 0x00400000 through 0x00800000 were used previously (up to W2K) and should not be re-used
// Trust attributes 0x01000000 through 0x80000000 are reserved for user
#define TRUST_ATTRIBUTES_VALID          0xFF03FFFF
#define TRUST_ATTRIBUTES_USER           0xFF000000

typedef struct _TRUSTED_DOMAIN_INFORMATION_EX {

    LSA_UNICODE_STRING Name;
    LSA_UNICODE_STRING FlatName;
    PSID  Sid;
    ULONG TrustDirection;
    ULONG TrustType;
    ULONG TrustAttributes;

} TRUSTED_DOMAIN_INFORMATION_EX, *PTRUSTED_DOMAIN_INFORMATION_EX;

typedef struct _TRUSTED_DOMAIN_INFORMATION_EX2 {

    LSA_UNICODE_STRING Name;
    LSA_UNICODE_STRING FlatName;
    PSID  Sid;
    ULONG TrustDirection;
    ULONG TrustType;
    ULONG TrustAttributes;
    ULONG ForestTrustLength;
#ifdef MIDL_PASS
    [size_is( ForestTrustLength )]
#endif
    PUCHAR ForestTrustInfo;

} TRUSTED_DOMAIN_INFORMATION_EX2, *PTRUSTED_DOMAIN_INFORMATION_EX2;

//
// Type of authentication information
//
#define TRUST_AUTH_TYPE_NONE    0   // Ignore this entry
#define TRUST_AUTH_TYPE_NT4OWF  1   // NT4 OWF password
#define TRUST_AUTH_TYPE_CLEAR   2   // Cleartext password
#define TRUST_AUTH_TYPE_VERSION 3   // Cleartext password version number

typedef struct _LSA_AUTH_INFORMATION {

    LARGE_INTEGER LastUpdateTime;
    ULONG AuthType;
    ULONG AuthInfoLength;
    PUCHAR AuthInfo;
} LSA_AUTH_INFORMATION, *PLSA_AUTH_INFORMATION;

typedef struct _TRUSTED_DOMAIN_AUTH_INFORMATION {

    ULONG IncomingAuthInfos;
    PLSA_AUTH_INFORMATION   IncomingAuthenticationInformation;
    PLSA_AUTH_INFORMATION   IncomingPreviousAuthenticationInformation;
    ULONG OutgoingAuthInfos;
    PLSA_AUTH_INFORMATION   OutgoingAuthenticationInformation;
    PLSA_AUTH_INFORMATION   OutgoingPreviousAuthenticationInformation;

} TRUSTED_DOMAIN_AUTH_INFORMATION, *PTRUSTED_DOMAIN_AUTH_INFORMATION;

typedef struct _TRUSTED_DOMAIN_FULL_INFORMATION {

    TRUSTED_DOMAIN_INFORMATION_EX   Information;
    TRUSTED_POSIX_OFFSET_INFO       PosixOffset;
    TRUSTED_DOMAIN_AUTH_INFORMATION AuthInformation;

} TRUSTED_DOMAIN_FULL_INFORMATION, *PTRUSTED_DOMAIN_FULL_INFORMATION;

typedef struct _TRUSTED_DOMAIN_FULL_INFORMATION2 {

    TRUSTED_DOMAIN_INFORMATION_EX2  Information;
    TRUSTED_POSIX_OFFSET_INFO       PosixOffset;
    TRUSTED_DOMAIN_AUTH_INFORMATION AuthInformation;

} TRUSTED_DOMAIN_FULL_INFORMATION2, *PTRUSTED_DOMAIN_FULL_INFORMATION2;

typedef enum {

    ForestTrustTopLevelName,
    ForestTrustTopLevelNameEx,
    ForestTrustDomainInfo,
    ForestTrustRecordTypeLast = ForestTrustDomainInfo

} LSA_FOREST_TRUST_RECORD_TYPE;

//
// Bottom 16 bits of the flags are reserved for disablement reasons
//

#define LSA_FTRECORD_DISABLED_REASONS            ( 0x0000FFFFL )

//
// Reasons for a top-level name forest trust record to be disabled
//

#define LSA_TLN_DISABLED_NEW                     ( 0x00000001L )
#define LSA_TLN_DISABLED_ADMIN                   ( 0x00000002L )
#define LSA_TLN_DISABLED_CONFLICT                ( 0x00000004L )

//
// Reasons for a domain information forest trust record to be disabled
//

#define LSA_SID_DISABLED_ADMIN                   ( 0x00000001L )
#define LSA_SID_DISABLED_CONFLICT                ( 0x00000002L )
#define LSA_NB_DISABLED_ADMIN                    ( 0x00000004L )
#define LSA_NB_DISABLED_CONFLICT                 ( 0x00000008L )

typedef struct _LSA_FOREST_TRUST_DOMAIN_INFO {

#ifdef MIDL_PASS
    PISID Sid;
#else
    PSID Sid;
#endif
    LSA_UNICODE_STRING DnsName;
    LSA_UNICODE_STRING NetbiosName;

} LSA_FOREST_TRUST_DOMAIN_INFO, *PLSA_FOREST_TRUST_DOMAIN_INFO;


//
//  To prevent huge data to be passed in, we should put a limit on LSA_FOREST_TRUST_BINARY_DATA.
//      128K is large enough that can't be reached in the near future, and small enough not to
//      cause memory problems.

#define MAX_FOREST_TRUST_BINARY_DATA_SIZE ( 128 * 1024 )

typedef struct _LSA_FOREST_TRUST_BINARY_DATA {

#ifdef MIDL_PASS
    [range(0, MAX_FOREST_TRUST_BINARY_DATA_SIZE)] ULONG Length;
    [size_is( Length )] PUCHAR Buffer;
#else
    ULONG Length;
    PUCHAR Buffer;
#endif

} LSA_FOREST_TRUST_BINARY_DATA, *PLSA_FOREST_TRUST_BINARY_DATA;

typedef struct _LSA_FOREST_TRUST_RECORD {

    ULONG Flags;
    LSA_FOREST_TRUST_RECORD_TYPE ForestTrustType; // type of record
    LARGE_INTEGER Time;

#ifdef MIDL_PASS
    [switch_type( LSA_FOREST_TRUST_RECORD_TYPE ), switch_is( ForestTrustType )]
#endif

    union {                                       // actual data

#ifdef MIDL_PASS
        [case( ForestTrustTopLevelName,
               ForestTrustTopLevelNameEx )] LSA_UNICODE_STRING TopLevelName;
        [case( ForestTrustDomainInfo )] LSA_FOREST_TRUST_DOMAIN_INFO DomainInfo;
        [default] LSA_FOREST_TRUST_BINARY_DATA Data;
#else
        LSA_UNICODE_STRING TopLevelName;
        LSA_FOREST_TRUST_DOMAIN_INFO DomainInfo;
        LSA_FOREST_TRUST_BINARY_DATA Data;        // used for unrecognized types
#endif
    } ForestTrustData;

} LSA_FOREST_TRUST_RECORD, *PLSA_FOREST_TRUST_RECORD;

//
// To prevent forest trust blobs of large size, number of records must be
// smaller than MAX_RECORDS_IN_FOREST_TRUST_INFO
//

#define MAX_RECORDS_IN_FOREST_TRUST_INFO 4000

typedef struct _LSA_FOREST_TRUST_INFORMATION {

#ifdef MIDL_PASS
    [range(0, MAX_RECORDS_IN_FOREST_TRUST_INFO)] ULONG RecordCount;
    [size_is( RecordCount )] PLSA_FOREST_TRUST_RECORD * Entries;
#else
    ULONG RecordCount;
    PLSA_FOREST_TRUST_RECORD * Entries;
#endif

} LSA_FOREST_TRUST_INFORMATION, *PLSA_FOREST_TRUST_INFORMATION;

typedef enum {

    CollisionTdo,
    CollisionXref,
    CollisionOther

} LSA_FOREST_TRUST_COLLISION_RECORD_TYPE;

typedef struct _LSA_FOREST_TRUST_COLLISION_RECORD {

    ULONG Index;
    LSA_FOREST_TRUST_COLLISION_RECORD_TYPE Type;
    ULONG Flags;
    LSA_UNICODE_STRING Name;

} LSA_FOREST_TRUST_COLLISION_RECORD, *PLSA_FOREST_TRUST_COLLISION_RECORD;

typedef struct _LSA_FOREST_TRUST_COLLISION_INFORMATION {

    ULONG RecordCount;
#ifdef MIDL_PASS
    [size_is( RecordCount )]
#endif
    PLSA_FOREST_TRUST_COLLISION_RECORD * Entries;

} LSA_FOREST_TRUST_COLLISION_INFORMATION, *PLSA_FOREST_TRUST_COLLISION_INFORMATION;


//
// LSA Enumeration Context
//

typedef ULONG LSA_ENUMERATION_HANDLE, *PLSA_ENUMERATION_HANDLE;

//
// LSA Enumeration Information
//

typedef struct _LSA_ENUMERATION_INFORMATION {

    PSID Sid;

} LSA_ENUMERATION_INFORMATION, *PLSA_ENUMERATION_INFORMATION;


////////////////////////////////////////////////////////////////////////////
//                                                                        //
// Local Security Policy - Miscellaneous API function prototypes          //
//                                                                        //
////////////////////////////////////////////////////////////////////////////


NTSTATUS
NTAPI
LsaFreeMemory(
    __in_opt PVOID Buffer
    );

NTSTATUS
NTAPI
LsaClose(
    __in LSA_HANDLE ObjectHandle
    );


typedef struct _SECURITY_LOGON_SESSION_DATA {
    ULONG               Size ;
    LUID                LogonId ;
    LSA_UNICODE_STRING  UserName ;
    LSA_UNICODE_STRING  LogonDomain ;
    LSA_UNICODE_STRING  AuthenticationPackage ;
    ULONG               LogonType ;
    ULONG               Session ;
    PSID                Sid ;
    LARGE_INTEGER       LogonTime ;

    //
    // new for whistler:
    //

    LSA_UNICODE_STRING  LogonServer ;
    LSA_UNICODE_STRING  DnsDomainName ;
    LSA_UNICODE_STRING  Upn ;
} SECURITY_LOGON_SESSION_DATA, * PSECURITY_LOGON_SESSION_DATA ;

NTSTATUS
NTAPI
LsaEnumerateLogonSessions(
    __out PULONG LogonSessionCount,
    __out PLUID * LogonSessionList
    );

NTSTATUS
NTAPI
LsaGetLogonSessionData(
    __in PLUID LogonId,
    __out PSECURITY_LOGON_SESSION_DATA * ppLogonSessionData
    );

NTSTATUS
NTAPI
LsaOpenPolicy(
    __in_opt PLSA_UNICODE_STRING SystemName,
    __in PLSA_OBJECT_ATTRIBUTES ObjectAttributes,
    __in ACCESS_MASK DesiredAccess,
    __out PLSA_HANDLE PolicyHandle
    );


NTSTATUS
NTAPI
LsaQueryInformationPolicy(
    __in LSA_HANDLE PolicyHandle,
    __in POLICY_INFORMATION_CLASS InformationClass,
    __out PVOID *Buffer
    );

NTSTATUS
NTAPI
LsaSetInformationPolicy(
    __in LSA_HANDLE PolicyHandle,
    __in POLICY_INFORMATION_CLASS InformationClass,
    __in PVOID Buffer
    );

NTSTATUS
NTAPI
LsaQueryDomainInformationPolicy(
    __in LSA_HANDLE PolicyHandle,
    __in POLICY_DOMAIN_INFORMATION_CLASS InformationClass,
    __out PVOID *Buffer
    );

NTSTATUS
NTAPI
LsaSetDomainInformationPolicy(
    __in LSA_HANDLE PolicyHandle,
    __in POLICY_DOMAIN_INFORMATION_CLASS InformationClass,
    __in_opt PVOID Buffer
    );


NTSTATUS
NTAPI
LsaRegisterPolicyChangeNotification(
    __in POLICY_NOTIFICATION_INFORMATION_CLASS InformationClass,
    __in HANDLE  NotificationEventHandle
    );

NTSTATUS
NTAPI
LsaUnregisterPolicyChangeNotification(
    __in POLICY_NOTIFICATION_INFORMATION_CLASS InformationClass,
    __in HANDLE  NotificationEventHandle
    );



NTSTATUS
NTAPI
LsaEnumerateTrustedDomains(
    __in LSA_HANDLE PolicyHandle,
    __inout PLSA_ENUMERATION_HANDLE EnumerationContext,
    __out PVOID *Buffer,
    __in ULONG PreferedMaximumLength,
    __out PULONG CountReturned
    );


NTSTATUS
NTAPI
LsaLookupNames(
    __in LSA_HANDLE PolicyHandle,
    __in ULONG Count,
    __in PLSA_UNICODE_STRING Names,
    __out PLSA_REFERENCED_DOMAIN_LIST *ReferencedDomains,
    __out PLSA_TRANSLATED_SID *Sids
    );

NTSTATUS
NTAPI
LsaLookupNames2(
    __in LSA_HANDLE PolicyHandle,
    __in ULONG Flags, // Reserved
    __in ULONG Count,
    __in PLSA_UNICODE_STRING Names,
    __out PLSA_REFERENCED_DOMAIN_LIST *ReferencedDomains,
    __out PLSA_TRANSLATED_SID2 *Sids
    );

NTSTATUS
NTAPI
LsaLookupSids(
    __in LSA_HANDLE PolicyHandle,
    __in ULONG Count,
    __in PSID *Sids,
    __out PLSA_REFERENCED_DOMAIN_LIST *ReferencedDomains,
    __out PLSA_TRANSLATED_NAME *Names
    );



#define SE_INTERACTIVE_LOGON_NAME           TEXT("SeInteractiveLogonRight")
#define SE_NETWORK_LOGON_NAME               TEXT("SeNetworkLogonRight")
#define SE_BATCH_LOGON_NAME                 TEXT("SeBatchLogonRight")
#define SE_SERVICE_LOGON_NAME               TEXT("SeServiceLogonRight")
#define SE_DENY_INTERACTIVE_LOGON_NAME      TEXT("SeDenyInteractiveLogonRight")
#define SE_DENY_NETWORK_LOGON_NAME          TEXT("SeDenyNetworkLogonRight")
#define SE_DENY_BATCH_LOGON_NAME            TEXT("SeDenyBatchLogonRight")
#define SE_DENY_SERVICE_LOGON_NAME          TEXT("SeDenyServiceLogonRight")
#define SE_REMOTE_INTERACTIVE_LOGON_NAME    TEXT("SeRemoteInteractiveLogonRight")
#define SE_DENY_REMOTE_INTERACTIVE_LOGON_NAME TEXT("SeDenyRemoteInteractiveLogonRight")

//
// This new API returns all the accounts with a certain privilege
//

NTSTATUS
NTAPI
LsaEnumerateAccountsWithUserRight(
    __in LSA_HANDLE PolicyHandle,
    __in_opt PLSA_UNICODE_STRING UserRight,
    __out PVOID *Buffer,
    __out PULONG CountReturned
    );

//
// These new APIs differ by taking a SID instead of requiring the caller
// to open the account first and passing in an account handle
//

NTSTATUS
NTAPI
LsaEnumerateAccountRights(
    __in LSA_HANDLE PolicyHandle,
    __in PSID AccountSid,
    __deref_out_ecount(*CountOfRights) PLSA_UNICODE_STRING *UserRights,
    __out PULONG CountOfRights
    );

NTSTATUS
NTAPI
LsaAddAccountRights(
    __in LSA_HANDLE PolicyHandle,
    __in PSID AccountSid,
    __in_ecount(CountOfRights) PLSA_UNICODE_STRING UserRights,
    __in ULONG CountOfRights
    );

NTSTATUS
NTAPI
LsaRemoveAccountRights(
    __in LSA_HANDLE PolicyHandle,
    __in PSID AccountSid,
    __in BOOLEAN AllRights,
    __in_ecount_opt(CountOfRights) PLSA_UNICODE_STRING UserRights,
    __in ULONG CountOfRights
    );


///////////////////////////////////////////////////////////////////////////////
//                                                                           //
// Local Security Policy - Trusted Domain Object API function prototypes     //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

NTSTATUS
NTAPI
LsaOpenTrustedDomainByName(
    __in LSA_HANDLE PolicyHandle,
    __in PLSA_UNICODE_STRING TrustedDomainName,
    __in ACCESS_MASK DesiredAccess,
    __out PLSA_HANDLE TrustedDomainHandle
    );


NTSTATUS
NTAPI
LsaQueryTrustedDomainInfo(
    __in LSA_HANDLE PolicyHandle,
    __in PSID TrustedDomainSid,
    __in TRUSTED_INFORMATION_CLASS InformationClass,
    __out PVOID *Buffer
    );

NTSTATUS
NTAPI
LsaSetTrustedDomainInformation(
    __in LSA_HANDLE PolicyHandle,
    __in PSID TrustedDomainSid,
    __in TRUSTED_INFORMATION_CLASS InformationClass,
    __in PVOID Buffer
    );

NTSTATUS
NTAPI
LsaDeleteTrustedDomain(
    __in LSA_HANDLE PolicyHandle,
    __in PSID TrustedDomainSid
    );

NTSTATUS
NTAPI
LsaQueryTrustedDomainInfoByName(
    __in LSA_HANDLE PolicyHandle,
    __in PLSA_UNICODE_STRING TrustedDomainName,
    __in TRUSTED_INFORMATION_CLASS InformationClass,
    __out PVOID *Buffer
    );

NTSTATUS
NTAPI
LsaSetTrustedDomainInfoByName(
    __in LSA_HANDLE PolicyHandle,
    __in PLSA_UNICODE_STRING TrustedDomainName,
    __in TRUSTED_INFORMATION_CLASS InformationClass,
    __in PVOID Buffer
    );

NTSTATUS
NTAPI
LsaEnumerateTrustedDomainsEx(
    __in LSA_HANDLE PolicyHandle,
    __inout PLSA_ENUMERATION_HANDLE EnumerationContext,
    __out PVOID *Buffer,
    __in ULONG PreferedMaximumLength,
    __out PULONG CountReturned
    );

NTSTATUS
NTAPI
LsaCreateTrustedDomainEx(
    __in LSA_HANDLE PolicyHandle,
    __in PTRUSTED_DOMAIN_INFORMATION_EX TrustedDomainInformation,
    __in PTRUSTED_DOMAIN_AUTH_INFORMATION AuthenticationInformation,
    __in ACCESS_MASK DesiredAccess,
    __out PLSA_HANDLE TrustedDomainHandle
    );

NTSTATUS
NTAPI
LsaQueryForestTrustInformation(
    __in LSA_HANDLE PolicyHandle,
    __in PLSA_UNICODE_STRING TrustedDomainName,
    __out PLSA_FOREST_TRUST_INFORMATION * ForestTrustInfo
    );

NTSTATUS
NTAPI
LsaSetForestTrustInformation(
    __in LSA_HANDLE PolicyHandle,
    __in PLSA_UNICODE_STRING TrustedDomainName,
    __in PLSA_FOREST_TRUST_INFORMATION ForestTrustInfo,
    __in BOOLEAN CheckOnly,
    __out PLSA_FOREST_TRUST_COLLISION_INFORMATION * CollisionInfo
    );

// #define TESTING_MATCHING_ROUTINE

#ifdef TESTING_MATCHING_ROUTINE

NTSTATUS
NTAPI
LsaForestTrustFindMatch(
    __in LSA_HANDLE PolicyHandle,
    __in ULONG Type,
    __in PLSA_UNICODE_STRING Name,
    __out PLSA_UNICODE_STRING * Match
    );

#endif

//
// This API sets the workstation password (equivalent of setting/getting
// the SSI_SECRET_NAME secret)
//

NTSTATUS
NTAPI
LsaStorePrivateData(
    __in LSA_HANDLE PolicyHandle,
    __in PLSA_UNICODE_STRING KeyName,
    __in_opt PLSA_UNICODE_STRING PrivateData
    );

NTSTATUS
NTAPI
LsaRetrievePrivateData(
    __in LSA_HANDLE PolicyHandle,
    __in PLSA_UNICODE_STRING KeyName,
    __out PLSA_UNICODE_STRING * PrivateData
    );


ULONG
NTAPI
LsaNtStatusToWinError(
    __in NTSTATUS Status
    );


//
// Define a symbol so we can tell if ntifs.h has been included.
//

// begin_ntifs
#ifndef _NTLSA_IFS_
#define _NTLSA_IFS_
#endif
// end_ntifs


//
// SPNEGO package stuff
//

enum NEGOTIATE_MESSAGES {
    NegEnumPackagePrefixes = 0,
    NegGetCallerName = 1,
    NegCallPackageMax
} ;

#define NEGOTIATE_MAX_PREFIX    32

typedef struct _NEGOTIATE_PACKAGE_PREFIX {
    ULONG_PTR   PackageId ;
    PVOID       PackageDataA ;
    PVOID       PackageDataW ;
    ULONG_PTR   PrefixLen ;
    UCHAR       Prefix[ NEGOTIATE_MAX_PREFIX ];
} NEGOTIATE_PACKAGE_PREFIX, * PNEGOTIATE_PACKAGE_PREFIX ;

typedef struct _NEGOTIATE_PACKAGE_PREFIXES {
    ULONG       MessageType ;
    ULONG       PrefixCount ;
    ULONG       Offset ;        // Offset to array of _PREFIX above
    ULONG       Pad ;           // Align structure for 64-bit
} NEGOTIATE_PACKAGE_PREFIXES, *PNEGOTIATE_PACKAGE_PREFIXES ;

typedef struct _NEGOTIATE_CALLER_NAME_REQUEST {
    ULONG       MessageType ;
    LUID        LogonId ;
} NEGOTIATE_CALLER_NAME_REQUEST, *PNEGOTIATE_CALLER_NAME_REQUEST ;

typedef struct _NEGOTIATE_CALLER_NAME_RESPONSE {
    ULONG       MessageType ;
    PWSTR       CallerName ;
} NEGOTIATE_CALLER_NAME_RESPONSE, * PNEGOTIATE_CALLER_NAME_RESPONSE ;

#ifndef _NTDEF_
typedef LSA_UNICODE_STRING UNICODE_STRING, *PUNICODE_STRING;
typedef LSA_STRING STRING, *PSTRING ;
#endif

#ifndef _DOMAIN_PASSWORD_INFORMATION_DEFINED
#define _DOMAIN_PASSWORD_INFORMATION_DEFINED
typedef struct _DOMAIN_PASSWORD_INFORMATION {
    USHORT MinPasswordLength;
    USHORT PasswordHistoryLength;
    ULONG PasswordProperties;
#if defined(MIDL_PASS)
    OLD_LARGE_INTEGER MaxPasswordAge;
    OLD_LARGE_INTEGER MinPasswordAge;
#else
    LARGE_INTEGER MaxPasswordAge;
    LARGE_INTEGER MinPasswordAge;
#endif
} DOMAIN_PASSWORD_INFORMATION, *PDOMAIN_PASSWORD_INFORMATION;
#endif 

//
// PasswordProperties flags
//

#define DOMAIN_PASSWORD_COMPLEX             0x00000001L
#define DOMAIN_PASSWORD_NO_ANON_CHANGE      0x00000002L
#define DOMAIN_PASSWORD_NO_CLEAR_CHANGE     0x00000004L
#define DOMAIN_LOCKOUT_ADMINS               0x00000008L
#define DOMAIN_PASSWORD_STORE_CLEARTEXT     0x00000010L
#define DOMAIN_REFUSE_PASSWORD_CHANGE       0x00000020L



#ifndef _PASSWORD_NOTIFICATION_DEFINED
#define _PASSWORD_NOTIFICATION_DEFINED
typedef NTSTATUS (*PSAM_PASSWORD_NOTIFICATION_ROUTINE) (
    PUNICODE_STRING UserName,
    ULONG RelativeId,
    PUNICODE_STRING NewPassword
);

#define SAM_PASSWORD_CHANGE_NOTIFY_ROUTINE  "PasswordChangeNotify"

typedef BOOLEAN (*PSAM_INIT_NOTIFICATION_ROUTINE) (
);

#define SAM_INIT_NOTIFICATION_ROUTINE  "InitializeChangeNotify"

#define SAM_PASSWORD_FILTER_ROUTINE  "PasswordFilter"

typedef BOOLEAN (*PSAM_PASSWORD_FILTER_ROUTINE) (
    IN PUNICODE_STRING  AccountName,
    IN PUNICODE_STRING  FullName,
    IN PUNICODE_STRING Password,
    IN BOOLEAN SetOperation
    );


#endif // _PASSWORD_NOTIFICATION_DEFINED


/////////////////////////////////////////////////////////////////////////
//                                                                     //
// Name of the MSV1_0 authentication package                           //
//                                                                     //
/////////////////////////////////////////////////////////////////////////

#define MSV1_0_PACKAGE_NAME     "MICROSOFT_AUTHENTICATION_PACKAGE_V1_0"
#define MSV1_0_PACKAGE_NAMEW    L"MICROSOFT_AUTHENTICATION_PACKAGE_V1_0"
#define MSV1_0_PACKAGE_NAMEW_LENGTH sizeof(MSV1_0_PACKAGE_NAMEW) - sizeof(WCHAR)

//
// Location of MSV authentication package data
//
#define MSV1_0_SUBAUTHENTICATION_KEY "SYSTEM\\CurrentControlSet\\Control\\Lsa\\MSV1_0"
#define MSV1_0_SUBAUTHENTICATION_VALUE "Auth"


/////////////////////////////////////////////////////////////////////////
//                                                                     //
// Widely used MSV1_0 data types                                       //
//                                                                     //
/////////////////////////////////////////////////////////////////////////



///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//       LOGON      Related Data Structures
//
//                                                                           //
///////////////////////////////////////////////////////////////////////////////

//
// When a LsaLogonUser() call is dispatched to the MsV1_0 authentication
// package, the beginning of the AuthenticationInformation buffer is
// cast to a MSV1_0_LOGON_SUBMIT_TYPE to determine the type of logon
// being requested.  Similarly, upon return, the type of profile buffer
// can be determined by typecasting it to a MSV_1_0_PROFILE_BUFFER_TYPE.
//

//
//  MSV1.0 LsaLogonUser() submission message types.
//

typedef enum _MSV1_0_LOGON_SUBMIT_TYPE {
    MsV1_0InteractiveLogon = 2,
    MsV1_0Lm20Logon,
    MsV1_0NetworkLogon,
    MsV1_0SubAuthLogon,
    MsV1_0WorkstationUnlockLogon = 7
} MSV1_0_LOGON_SUBMIT_TYPE, *PMSV1_0_LOGON_SUBMIT_TYPE;


//
//  MSV1.0 LsaLogonUser() profile buffer types.
//

typedef enum _MSV1_0_PROFILE_BUFFER_TYPE {
    MsV1_0InteractiveProfile = 2,
    MsV1_0Lm20LogonProfile,
    MsV1_0SmartCardProfile
} MSV1_0_PROFILE_BUFFER_TYPE, *PMSV1_0_PROFILE_BUFFER_TYPE;

//
// MsV1_0InteractiveLogon
//
// The AuthenticationInformation buffer of an LsaLogonUser() call to
// perform an interactive logon contains the following data structure:
//

typedef struct _MSV1_0_INTERACTIVE_LOGON {
    MSV1_0_LOGON_SUBMIT_TYPE MessageType;
    UNICODE_STRING LogonDomainName;
    UNICODE_STRING UserName;
    UNICODE_STRING Password;
} MSV1_0_INTERACTIVE_LOGON, *PMSV1_0_INTERACTIVE_LOGON;

//
// Where:
//
//     MessageType - Contains the type of logon being requested.  This
//         field must be set to MsV1_0InteractiveLogon.
//
//     UserName - Is a string representing the user's account name.  The
//         name may be up to 255 characters long.  The name is treated case
//         insensitive.
//
//     Password - Is a string containing the user's cleartext password.
//         The password may be up to 255 characters long and contain any
//         UNICODE value.
//
//


//
// The ProfileBuffer returned upon a successful logon of this type
// contains the following data structure:
//

typedef struct _MSV1_0_INTERACTIVE_PROFILE {
    MSV1_0_PROFILE_BUFFER_TYPE MessageType;
    USHORT LogonCount;
    USHORT BadPasswordCount;
    LARGE_INTEGER LogonTime;
    LARGE_INTEGER LogoffTime;
    LARGE_INTEGER KickOffTime;
    LARGE_INTEGER PasswordLastSet;
    LARGE_INTEGER PasswordCanChange;
    LARGE_INTEGER PasswordMustChange;
    UNICODE_STRING LogonScript;
    UNICODE_STRING HomeDirectory;
    UNICODE_STRING FullName;
    UNICODE_STRING ProfilePath;
    UNICODE_STRING HomeDirectoryDrive;
    UNICODE_STRING LogonServer;
    ULONG UserFlags;
} MSV1_0_INTERACTIVE_PROFILE, *PMSV1_0_INTERACTIVE_PROFILE;

//
// where:
//
//     MessageType - Identifies the type of profile data being returned.
//         Contains the type of logon being requested.  This field must
//         be set to MsV1_0InteractiveProfile.
//
//     LogonCount - Number of times the user is currently logged on.
//
//     BadPasswordCount - Number of times a bad password was applied to
//         the account since last successful logon.
//
//     LogonTime - Time when user last logged on.  This is an absolute
//         format NT standard time value.
//
//     LogoffTime - Time when user should log off.  This is an absolute
//         format NT standard time value.
//
//     KickOffTime - Time when system should force user logoff.  This is
//         an absolute format NT standard time value.
//
//     PasswordLastChanged - Time and date the password was last
//         changed.  This is an absolute format NT standard time
//         value.
//
//     PasswordCanChange - Time and date when the user can change the
//         password.  This is an absolute format NT time value.  To
//         prevent a password from ever changing, set this field to a
//         date very far into the future.
//
//     PasswordMustChange - Time and date when the user must change the
//         password.  If the user can never change the password, this
//         field is undefined.  This is an absolute format NT time
//         value.
//
//     LogonScript - The (relative) path to the account's logon
//         script.
//
//     HomeDirectory - The home directory for the user.
//


//
// MsV1_0Lm20Logon and MsV1_0NetworkLogon
//
// The AuthenticationInformation buffer of an LsaLogonUser() call to
// perform an network logon contains the following data structure:
//
// MsV1_0NetworkLogon logon differs from MsV1_0Lm20Logon in that the
// ParameterControl field exists.
//

#define MSV1_0_CHALLENGE_LENGTH 8
#define MSV1_0_USER_SESSION_KEY_LENGTH 16
#define MSV1_0_LANMAN_SESSION_KEY_LENGTH 8



//
// Values for ParameterControl.
//

#define MSV1_0_CLEARTEXT_PASSWORD_ALLOWED    0x02
#define MSV1_0_UPDATE_LOGON_STATISTICS       0x04
#define MSV1_0_RETURN_USER_PARAMETERS        0x08
#define MSV1_0_DONT_TRY_GUEST_ACCOUNT        0x10
#define MSV1_0_ALLOW_SERVER_TRUST_ACCOUNT    0x20
#define MSV1_0_RETURN_PASSWORD_EXPIRY        0x40
// this next flag says that CaseInsensitiveChallengeResponse
//  (aka LmResponse) contains a client challenge in the first 8 bytes
#define MSV1_0_USE_CLIENT_CHALLENGE          0x80
#define MSV1_0_TRY_GUEST_ACCOUNT_ONLY        0x100
#define MSV1_0_RETURN_PROFILE_PATH           0x200
#define MSV1_0_TRY_SPECIFIED_DOMAIN_ONLY     0x400
#define MSV1_0_ALLOW_WORKSTATION_TRUST_ACCOUNT 0x800
#define MSV1_0_DISABLE_PERSONAL_FALLBACK     0x00001000
#define MSV1_0_ALLOW_FORCE_GUEST             0x00002000
#define MSV1_0_CLEARTEXT_PASSWORD_SUPPLIED   0x00004000
#define MSV1_0_USE_DOMAIN_FOR_ROUTING_ONLY   0x00008000
#define MSV1_0_SUBAUTHENTICATION_DLL_EX      0x00100000
#define MSV1_0_ALLOW_MSVCHAPV2               0x00010000

//
// The high order byte is a value indicating the SubAuthentication DLL.
//  Zero indicates no SubAuthentication DLL.
//
#define MSV1_0_SUBAUTHENTICATION_DLL         0xFF000000
#define MSV1_0_SUBAUTHENTICATION_DLL_SHIFT   24
#define MSV1_0_MNS_LOGON                     0x01000000

//
// This is the list of subauthentication dlls used in MS
//

#define MSV1_0_SUBAUTHENTICATION_DLL_RAS     2
#define MSV1_0_SUBAUTHENTICATION_DLL_IIS     132

typedef struct _MSV1_0_LM20_LOGON {
    MSV1_0_LOGON_SUBMIT_TYPE MessageType;
    UNICODE_STRING LogonDomainName;
    UNICODE_STRING UserName;
    UNICODE_STRING Workstation;
    UCHAR ChallengeToClient[MSV1_0_CHALLENGE_LENGTH];
    STRING CaseSensitiveChallengeResponse;
    STRING CaseInsensitiveChallengeResponse;
    ULONG ParameterControl;
} MSV1_0_LM20_LOGON, * PMSV1_0_LM20_LOGON;

//
// NT 5.0 SubAuth dlls can use this struct
//

typedef struct _MSV1_0_SUBAUTH_LOGON{
    MSV1_0_LOGON_SUBMIT_TYPE MessageType;
    UNICODE_STRING LogonDomainName;
    UNICODE_STRING UserName;
    UNICODE_STRING Workstation;
    UCHAR ChallengeToClient[MSV1_0_CHALLENGE_LENGTH];
    STRING AuthenticationInfo1;
    STRING AuthenticationInfo2;
    ULONG ParameterControl;
    ULONG SubAuthPackageId;
} MSV1_0_SUBAUTH_LOGON, * PMSV1_0_SUBAUTH_LOGON;


//
// Values for UserFlags.
//

#define LOGON_GUEST                 0x01
#define LOGON_NOENCRYPTION          0x02
#define LOGON_CACHED_ACCOUNT        0x04
#define LOGON_USED_LM_PASSWORD      0x08
#define LOGON_EXTRA_SIDS            0x20
#define LOGON_SUBAUTH_SESSION_KEY   0x40
#define LOGON_SERVER_TRUST_ACCOUNT  0x80
#define LOGON_NTLMV2_ENABLED        0x100       // says DC understands NTLMv2
#define LOGON_RESOURCE_GROUPS       0x200
#define LOGON_PROFILE_PATH_RETURNED 0x400

//
// The high order byte is reserved for return by SubAuthentication DLLs.
//

#define MSV1_0_SUBAUTHENTICATION_FLAGS 0xFF000000

// Values returned by the MSV1_0_MNS_LOGON SubAuthentication DLL
#define LOGON_GRACE_LOGON              0x01000000

typedef struct _MSV1_0_LM20_LOGON_PROFILE {
    MSV1_0_PROFILE_BUFFER_TYPE MessageType;
    LARGE_INTEGER KickOffTime;
    LARGE_INTEGER LogoffTime;
    ULONG UserFlags;
    UCHAR UserSessionKey[MSV1_0_USER_SESSION_KEY_LENGTH];
    UNICODE_STRING LogonDomainName;
    UCHAR LanmanSessionKey[MSV1_0_LANMAN_SESSION_KEY_LENGTH];
    UNICODE_STRING LogonServer;
    UNICODE_STRING UserParameters;
} MSV1_0_LM20_LOGON_PROFILE, * PMSV1_0_LM20_LOGON_PROFILE;


//
// Supplemental credentials structure used for passing credentials into
// MSV1_0 from other packages
//

#define MSV1_0_OWF_PASSWORD_LENGTH 16
#define MSV1_0_CRED_LM_PRESENT 0x1
#define MSV1_0_CRED_NT_PRESENT 0x2
#define MSV1_0_CRED_VERSION 0

typedef struct _MSV1_0_SUPPLEMENTAL_CREDENTIAL {
    ULONG Version;
    ULONG Flags;
    UCHAR LmPassword[MSV1_0_OWF_PASSWORD_LENGTH];
    UCHAR NtPassword[MSV1_0_OWF_PASSWORD_LENGTH];
} MSV1_0_SUPPLEMENTAL_CREDENTIAL, *PMSV1_0_SUPPLEMENTAL_CREDENTIAL;


//
// NTLM3 definitions.
//

#define MSV1_0_NTLM3_RESPONSE_LENGTH 16
#define MSV1_0_NTLM3_OWF_LENGTH 16

//
// this is the longest amount of time we'll allow challenge response
// pairs to be used. Note that this also has to allow for worst case clock skew
//
#define MSV1_0_MAX_NTLM3_LIFE 129600     // 36 hours (in seconds)
#define MSV1_0_MAX_AVL_SIZE 64000

//
// MsvAvFlags bit values
//

#define MSV1_0_AV_FLAG_FORCE_GUEST  0x00000001


// this is an MSV1_0 private data structure, defining the layout of an NTLM3 response, as sent by a
//  client in the NtChallengeResponse field of the NETLOGON_NETWORK_INFO structure. If can be differentiated
//  from an old style NT response by its length. This is crude, but it needs to pass through servers and
//  the servers' DCs that do not understand NTLM3 but that are willing to pass longer responses.
typedef struct _MSV1_0_NTLM3_RESPONSE {
    UCHAR Response[MSV1_0_NTLM3_RESPONSE_LENGTH]; // hash of OWF of password with all the following fields
    UCHAR RespType;     // id number of response; current is 1
    UCHAR HiRespType;   // highest id number understood by client
    USHORT Flags;       // reserved; must be sent as zero at this version
    ULONG MsgWord;      // 32 bit message from client to server (for use by auth protocol)
    ULONGLONG TimeStamp;    // time stamp when client generated response -- NT system time, quad part
    UCHAR ChallengeFromClient[MSV1_0_CHALLENGE_LENGTH];
    ULONG AvPairsOff;   // offset to start of AvPairs (to allow future expansion)
    UCHAR Buffer[1];    // start of buffer with AV pairs (or future stuff -- so use the offset)
} MSV1_0_NTLM3_RESPONSE, *PMSV1_0_NTLM3_RESPONSE;

#define MSV1_0_NTLM3_INPUT_LENGTH (sizeof(MSV1_0_NTLM3_RESPONSE) - MSV1_0_NTLM3_RESPONSE_LENGTH)
#define MSV1_0_NTLM3_MIN_NT_RESPONSE_LENGTH RTL_SIZEOF_THROUGH_FIELD(MSV1_0_NTLM3_RESPONSE, AvPairsOff)

typedef enum {
    MsvAvEOL,                 // end of list
    MsvAvNbComputerName,      // server's computer name -- NetBIOS
    MsvAvNbDomainName,        // server's domain name -- NetBIOS
    MsvAvDnsComputerName,     // server's computer name -- DNS
    MsvAvDnsDomainName,       // server's domain name -- DNS
    MsvAvDnsTreeName,         // server's tree name -- DNS
    MsvAvFlags                // server's extended flags -- DWORD mask
} MSV1_0_AVID;

typedef struct  _MSV1_0_AV_PAIR {
    USHORT AvId;
    USHORT AvLen;
    // Data is treated as byte array following structure
} MSV1_0_AV_PAIR, *PMSV1_0_AV_PAIR;



///////////////////////////////////////////////////////////////////////////////
//                                                                           //
//       CALL PACKAGE Related Data Structures                                //
//                                                                           //
///////////////////////////////////////////////////////////////////////////////


//
//  MSV1.0 LsaCallAuthenticationPackage() submission and response
//  message types.
//

typedef enum _MSV1_0_PROTOCOL_MESSAGE_TYPE {
    MsV1_0Lm20ChallengeRequest = 0,          // Both submission and response
    MsV1_0Lm20GetChallengeResponse,          // Both submission and response
    MsV1_0EnumerateUsers,                    // Both submission and response
    MsV1_0GetUserInfo,                       // Both submission and response
    MsV1_0ReLogonUsers,                      // Submission only
    MsV1_0ChangePassword,                    // Both submission and response
    MsV1_0ChangeCachedPassword,              // Both submission and response
    MsV1_0GenericPassthrough,                // Both submission and response
    MsV1_0CacheLogon,                        // Submission only, no response
    MsV1_0SubAuth,                           // Both submission and response
    MsV1_0DeriveCredential,                  // Both submission and response
    MsV1_0CacheLookup,                       // Both submission and response
    MsV1_0SetProcessOption,                  // Submission only, no response
} MSV1_0_PROTOCOL_MESSAGE_TYPE, *PMSV1_0_PROTOCOL_MESSAGE_TYPE;


typedef struct _MSV1_0_CHANGEPASSWORD_REQUEST {
    MSV1_0_PROTOCOL_MESSAGE_TYPE MessageType;
    UNICODE_STRING DomainName;
    UNICODE_STRING AccountName;
    UNICODE_STRING OldPassword;
    UNICODE_STRING NewPassword;
    BOOLEAN        Impersonating;
} MSV1_0_CHANGEPASSWORD_REQUEST, *PMSV1_0_CHANGEPASSWORD_REQUEST;

typedef struct _MSV1_0_CHANGEPASSWORD_RESPONSE {
    MSV1_0_PROTOCOL_MESSAGE_TYPE MessageType;
    BOOLEAN PasswordInfoValid;
    DOMAIN_PASSWORD_INFORMATION DomainPasswordInfo;
} MSV1_0_CHANGEPASSWORD_RESPONSE, *PMSV1_0_CHANGEPASSWORD_RESPONSE;


//
// MsV1_0GenericPassthrough - for remoting a CallPackage to
// a domain controller on the specified domain
//

typedef struct _MSV1_0_PASSTHROUGH_REQUEST {
    MSV1_0_PROTOCOL_MESSAGE_TYPE MessageType;
    UNICODE_STRING DomainName;
    UNICODE_STRING PackageName;
    ULONG DataLength;
    PUCHAR LogonData;
    ULONG Pad ;
} MSV1_0_PASSTHROUGH_REQUEST, *PMSV1_0_PASSTHROUGH_REQUEST;

typedef struct _MSV1_0_PASSTHROUGH_RESPONSE {
    MSV1_0_PROTOCOL_MESSAGE_TYPE MessageType;
    ULONG Pad;
    ULONG DataLength;
    PUCHAR ValidationData;
} MSV1_0_PASSTHROUGH_RESPONSE, *PMSV1_0_PASSTHROUGH_RESPONSE;


//
// MsV1_0SubAuthInfo submit buffer and response - for submitting a buffer to a
// specified Subauthentication Package during an LsaCallAuthenticationPackage().
// If this Subauthentication is to be done locally, then package this message
// in LsaCallAuthenticationPackage(). If this SubAuthentication needs to be done
// on the domain controller, then call LsaCallauthenticationPackage with the
// message type being MsV1_0GenericPassThrough and the LogonData in this struct
// should be a PMSV1_0_SUBAUTH_REQUEST
//

typedef struct _MSV1_0_SUBAUTH_REQUEST{
    MSV1_0_PROTOCOL_MESSAGE_TYPE MessageType;
    ULONG SubAuthPackageId;
    ULONG SubAuthInfoLength;
    PUCHAR SubAuthSubmitBuffer;
} MSV1_0_SUBAUTH_REQUEST, *PMSV1_0_SUBAUTH_REQUEST;

typedef struct _MSV1_0_SUBAUTH_RESPONSE{
    MSV1_0_PROTOCOL_MESSAGE_TYPE MessageType;
    ULONG SubAuthInfoLength;
    PUCHAR SubAuthReturnBuffer;
} MSV1_0_SUBAUTH_RESPONSE, *PMSV1_0_SUBAUTH_RESPONSE;


#define RtlGenRandom                    SystemFunction036
#define RtlEncryptMemory                SystemFunction040
#define RtlDecryptMemory                SystemFunction041

BOOLEAN
RtlGenRandom(
    __out_bcount(RandomBufferLength) PVOID RandomBuffer,
    __in ULONG RandomBufferLength
    );

//
// The buffer passed into RtlEncryptMemory and RtlDecryptMemory
// must be a multiple of this length.
//

#define RTL_ENCRYPT_MEMORY_SIZE             8

//
// Allow Encrypt/Decrypt across process boundaries.
// eg: encrypted buffer passed across LPC to another process which calls RtlDecryptMemory.
//

#define RTL_ENCRYPT_OPTION_CROSS_PROCESS    0x01

//
// Allow Encrypt/Decrypt across callers with same LogonId.
// eg: encrypted buffer passed across LPC to another process which calls RtlDecryptMemory whilst impersonating.
//

#define RTL_ENCRYPT_OPTION_SAME_LOGON       0x02

NTSTATUS
RtlEncryptMemory(
    __inout_bcount(MemorySize) PVOID Memory,
    __in ULONG MemorySize,
    __in ULONG OptionFlags
    );

NTSTATUS
RtlDecryptMemory(
    __inout_bcount(MemorySize) PVOID Memory,
    __in ULONG MemorySize,
    __in ULONG OptionFlags
    );
    

// Revision of the Kerberos Protocol.  MS uses Version 5, Revision 6

#define KERBEROS_VERSION    5
#define KERBEROS_REVISION   6



// Encryption Types:
// These encryption types are supported by the default MS KERBSUPP DLL
// as crypto systems.  Values over 127 are local values, and may be changed
// without notice.

#define KERB_ETYPE_NULL             0
#define KERB_ETYPE_DES_CBC_CRC      1
#define KERB_ETYPE_DES_CBC_MD4      2
#define KERB_ETYPE_DES_CBC_MD5      3


#define KERB_ETYPE_RC4_MD4          -128    // FFFFFF80
#define KERB_ETYPE_RC4_PLAIN2       -129
#define KERB_ETYPE_RC4_LM           -130
#define KERB_ETYPE_RC4_SHA          -131
#define KERB_ETYPE_DES_PLAIN        -132
#define KERB_ETYPE_RC4_HMAC_OLD     -133    // FFFFFF7B
#define KERB_ETYPE_RC4_PLAIN_OLD    -134
#define KERB_ETYPE_RC4_HMAC_OLD_EXP -135
#define KERB_ETYPE_RC4_PLAIN_OLD_EXP -136
#define KERB_ETYPE_RC4_PLAIN        -140
#define KERB_ETYPE_RC4_PLAIN_EXP    -141

//
// Pkinit encryption types
//


#define KERB_ETYPE_DSA_SHA1_CMS                             9
#define KERB_ETYPE_RSA_MD5_CMS                              10
#define KERB_ETYPE_RSA_SHA1_CMS                             11
#define KERB_ETYPE_RC2_CBC_ENV                              12
#define KERB_ETYPE_RSA_ENV                                  13
#define KERB_ETYPE_RSA_ES_OEAP_ENV                          14
#define KERB_ETYPE_DES_EDE3_CBC_ENV                         15


//
// Deprecated
//

#define KERB_ETYPE_DSA_SIGN                                8
#define KERB_ETYPE_RSA_PRIV                                9
#define KERB_ETYPE_RSA_PUB                                 10
#define KERB_ETYPE_RSA_PUB_MD5                             11
#define KERB_ETYPE_RSA_PUB_SHA1                            12
#define KERB_ETYPE_PKCS7_PUB                               13

//
// Unsupported but defined types
//

#define KERB_ETYPE_DES3_CBC_MD5                             5
#define KERB_ETYPE_DES3_CBC_SHA1                            7
#define KERB_ETYPE_DES3_CBC_SHA1_KD                        16

//
// In use types
//

#define KERB_ETYPE_DES_CBC_MD5_NT                          20
#define KERB_ETYPE_RC4_HMAC_NT                             23
#define KERB_ETYPE_RC4_HMAC_NT_EXP                         24

// Checksum algorithms.
// These algorithms are keyed internally for our use.

#define KERB_CHECKSUM_NONE  0
#define KERB_CHECKSUM_CRC32         1
#define KERB_CHECKSUM_MD4           2
#define KERB_CHECKSUM_KRB_DES_MAC   4
#define KERB_CHECKSUM_KRB_DES_MAC_K 5
#define KERB_CHECKSUM_MD5           7
#define KERB_CHECKSUM_MD5_DES       8


#define KERB_CHECKSUM_LM            -130
#define KERB_CHECKSUM_SHA1          -131
#define KERB_CHECKSUM_REAL_CRC32    -132
#define KERB_CHECKSUM_DES_MAC       -133
#define KERB_CHECKSUM_DES_MAC_MD5   -134
#define KERB_CHECKSUM_MD25          -135
#define KERB_CHECKSUM_RC4_MD5       -136
#define KERB_CHECKSUM_MD5_HMAC      -137                // used by netlogon
#define KERB_CHECKSUM_HMAC_MD5      -138                // used by Kerberos

#define AUTH_REQ_ALLOW_FORWARDABLE      0x00000001
#define AUTH_REQ_ALLOW_PROXIABLE        0x00000002
#define AUTH_REQ_ALLOW_POSTDATE         0x00000004
#define AUTH_REQ_ALLOW_RENEWABLE        0x00000008
#define AUTH_REQ_ALLOW_NOADDRESS        0x00000010
#define AUTH_REQ_ALLOW_ENC_TKT_IN_SKEY  0x00000020
#define AUTH_REQ_ALLOW_VALIDATE         0x00000040
#define AUTH_REQ_VALIDATE_CLIENT        0x00000080
#define AUTH_REQ_OK_AS_DELEGATE         0x00000100
#define AUTH_REQ_PREAUTH_REQUIRED       0x00000200
#define AUTH_REQ_TRANSITIVE_TRUST       0x00000400
#define AUTH_REQ_ALLOW_S4U_DELEGATE     0x00000800


#define AUTH_REQ_PER_USER_FLAGS         (AUTH_REQ_ALLOW_FORWARDABLE | \
                                         AUTH_REQ_ALLOW_PROXIABLE | \
                                         AUTH_REQ_ALLOW_POSTDATE | \
                                         AUTH_REQ_ALLOW_RENEWABLE | \
                                         AUTH_REQ_ALLOW_VALIDATE )
//
// Ticket Flags:
//

#define KERB_TICKET_FLAGS_reserved          0x80000000
#define KERB_TICKET_FLAGS_forwardable       0x40000000
#define KERB_TICKET_FLAGS_forwarded         0x20000000
#define KERB_TICKET_FLAGS_proxiable         0x10000000
#define KERB_TICKET_FLAGS_proxy             0x08000000
#define KERB_TICKET_FLAGS_may_postdate      0x04000000
#define KERB_TICKET_FLAGS_postdated         0x02000000
#define KERB_TICKET_FLAGS_invalid           0x01000000
#define KERB_TICKET_FLAGS_renewable         0x00800000
#define KERB_TICKET_FLAGS_initial           0x00400000
#define KERB_TICKET_FLAGS_pre_authent       0x00200000
#define KERB_TICKET_FLAGS_hw_authent        0x00100000
#define KERB_TICKET_FLAGS_ok_as_delegate    0x00040000
#define KERB_TICKET_FLAGS_name_canonicalize 0x00010000
#define KERB_TICKET_FLAGS_reserved1         0x00000001




//
// Name types
//

#define KRB_NT_UNKNOWN   0                // Name type not known
#define KRB_NT_PRINCIPAL 1                // Just the name of the principal as in DCE, or for users
#define KRB_NT_PRINCIPAL_AND_ID -131      // Name of the principal and its SID.
#define KRB_NT_SRV_INST  2                // Service and other unique instance (krbtgt)
#define KRB_NT_SRV_INST_AND_ID -132       // SPN and SID
#define KRB_NT_SRV_HST   3                // Service with host name as instance (telnet, rcommands)
#define KRB_NT_SRV_XHST  4                // Service with host as remaining components
#define KRB_NT_UID       5                // Unique ID
#define KRB_NT_ENTERPRISE_PRINCIPAL 10    // UPN **ONLY**
#define KRB_NT_ENT_PRINCIPAL_AND_ID -130  // UPN and SID

//
// MS extensions, negative according to the RFC
//

#define KRB_NT_MS_PRINCIPAL         -128        // NT4 style name

#define KRB_NT_MS_PRINCIPAL_AND_ID  -129        // nt4 style name with sid

#define KERB_IS_MS_PRINCIPAL(_x_) (((_x_) <= KRB_NT_MS_PRINCIPAL) || ((_x_) >= KRB_NT_ENTERPRISE_PRINCIPAL))


#ifndef MICROSOFT_KERBEROS_NAME_A

#define MICROSOFT_KERBEROS_NAME_A   "Kerberos"
#define MICROSOFT_KERBEROS_NAME_W   L"Kerberos"
#ifdef WIN32_CHICAGO
#define MICROSOFT_KERBEROS_NAME MICROSOFT_KERBEROS_NAME_A
#else
#define MICROSOFT_KERBEROS_NAME MICROSOFT_KERBEROS_NAME_W
#endif // WIN32_CHICAGO
#endif // MICROSOFT_KERBEROS_NAME_A


/////////////////////////////////////////////////////////////////////////
//
// Quality of protection parameters for MakeSignature / EncryptMessage
//
/////////////////////////////////////////////////////////////////////////

//
// This flag indicates to EncryptMessage that the message is not to actually
// be encrypted, but a header/trailer are to be produced.
//

#define KERB_WRAP_NO_ENCRYPT 0x80000001

/////////////////////////////////////////////////////////////////////////
//
// LsaLogonUser parameters
//
/////////////////////////////////////////////////////////////////////////

typedef enum _KERB_LOGON_SUBMIT_TYPE {
    KerbInteractiveLogon = 2,
    KerbSmartCardLogon = 6,
    KerbWorkstationUnlockLogon = 7,
    KerbSmartCardUnlockLogon = 8,
    KerbProxyLogon = 9,
    KerbTicketLogon = 10,
    KerbTicketUnlockLogon = 11,
    KerbS4ULogon = 12
} KERB_LOGON_SUBMIT_TYPE, *PKERB_LOGON_SUBMIT_TYPE;


typedef struct _KERB_INTERACTIVE_LOGON {
    KERB_LOGON_SUBMIT_TYPE MessageType;
    UNICODE_STRING LogonDomainName;
    UNICODE_STRING UserName;
    UNICODE_STRING Password;
} KERB_INTERACTIVE_LOGON, *PKERB_INTERACTIVE_LOGON;


typedef struct _KERB_INTERACTIVE_UNLOCK_LOGON {
    KERB_INTERACTIVE_LOGON Logon;
    LUID LogonId;
} KERB_INTERACTIVE_UNLOCK_LOGON, *PKERB_INTERACTIVE_UNLOCK_LOGON;

typedef struct _KERB_SMART_CARD_LOGON {
    KERB_LOGON_SUBMIT_TYPE MessageType;
    UNICODE_STRING Pin;
    ULONG CspDataLength;
    PUCHAR CspData;
} KERB_SMART_CARD_LOGON, *PKERB_SMART_CARD_LOGON;

typedef struct _KERB_SMART_CARD_UNLOCK_LOGON {
    KERB_SMART_CARD_LOGON Logon;
    LUID LogonId;
} KERB_SMART_CARD_UNLOCK_LOGON, *PKERB_SMART_CARD_UNLOCK_LOGON;

//
// Structure used for a ticket-only logon
//

typedef struct _KERB_TICKET_LOGON {
    KERB_LOGON_SUBMIT_TYPE MessageType;
    ULONG Flags;
    ULONG ServiceTicketLength;
    ULONG TicketGrantingTicketLength;
    PUCHAR ServiceTicket;               // REQUIRED: Service ticket "host"
    PUCHAR TicketGrantingTicket;        // OPTIONAL: User's encdoded in a KERB_CRED message, encrypted with session key from service ticket
} KERB_TICKET_LOGON, *PKERB_TICKET_LOGON;

//
// Flags for the ticket logon flags field
//

#define KERB_LOGON_FLAG_ALLOW_EXPIRED_TICKET 0x1

typedef struct _KERB_TICKET_UNLOCK_LOGON {
    KERB_TICKET_LOGON Logon;
    LUID LogonId;
} KERB_TICKET_UNLOCK_LOGON, *PKERB_TICKET_UNLOCK_LOGON;

//
//  Used for S4U Client requests
//
//
typedef struct _KERB_S4U_LOGON {
    KERB_LOGON_SUBMIT_TYPE MessageType;
    ULONG Flags;
    UNICODE_STRING ClientUpn;   // REQUIRED: UPN for client
    UNICODE_STRING ClientRealm; // Optional: Client Realm, if known
} KERB_S4U_LOGON, *PKERB_S4U_LOGON;


//
// Use the same profile structure as MSV1_0
//
typedef enum _KERB_PROFILE_BUFFER_TYPE {
    KerbInteractiveProfile = 2,
    KerbSmartCardProfile = 4,
    KerbTicketProfile = 6
} KERB_PROFILE_BUFFER_TYPE, *PKERB_PROFILE_BUFFER_TYPE;


typedef struct _KERB_INTERACTIVE_PROFILE {
    KERB_PROFILE_BUFFER_TYPE MessageType;
    USHORT LogonCount;
    USHORT BadPasswordCount;
    LARGE_INTEGER LogonTime;
    LARGE_INTEGER LogoffTime;
    LARGE_INTEGER KickOffTime;
    LARGE_INTEGER PasswordLastSet;
    LARGE_INTEGER PasswordCanChange;
    LARGE_INTEGER PasswordMustChange;
    UNICODE_STRING LogonScript;
    UNICODE_STRING HomeDirectory;
    UNICODE_STRING FullName;
    UNICODE_STRING ProfilePath;
    UNICODE_STRING HomeDirectoryDrive;
    UNICODE_STRING LogonServer;
    ULONG UserFlags;
} KERB_INTERACTIVE_PROFILE, *PKERB_INTERACTIVE_PROFILE;


//
// For smart card, we return a smart card profile, which is an interactive
// profile plus a certificate
//

typedef struct _KERB_SMART_CARD_PROFILE {
    KERB_INTERACTIVE_PROFILE Profile;
    ULONG CertificateSize;
    PUCHAR CertificateData;
} KERB_SMART_CARD_PROFILE, *PKERB_SMART_CARD_PROFILE;


//
// For a ticket logon profile, we return the session key from the ticket
//

typedef struct KERB_CRYPTO_KEY {
    LONG KeyType;
    ULONG Length;
    PUCHAR Value;
} KERB_CRYPTO_KEY, *PKERB_CRYPTO_KEY;

typedef struct _KERB_TICKET_PROFILE {
    KERB_INTERACTIVE_PROFILE Profile;
    KERB_CRYPTO_KEY SessionKey;
} KERB_TICKET_PROFILE, *PKERB_TICKET_PROFILE;


typedef enum _KERB_PROTOCOL_MESSAGE_TYPE {
    KerbDebugRequestMessage = 0,
    KerbQueryTicketCacheMessage,
    KerbChangeMachinePasswordMessage,
    KerbVerifyPacMessage,
    KerbRetrieveTicketMessage,
    KerbUpdateAddressesMessage,
    KerbPurgeTicketCacheMessage,
    KerbChangePasswordMessage,
    KerbRetrieveEncodedTicketMessage,
    KerbDecryptDataMessage,
    KerbAddBindingCacheEntryMessage,
    KerbSetPasswordMessage,
    KerbSetPasswordExMessage,
    KerbVerifyCredentialsMessage,
    KerbQueryTicketCacheExMessage,
    KerbPurgeTicketCacheExMessage,
    KerbRefreshSmartcardCredentialsMessage,
    KerbAddExtraCredentialsMessage,
    KerbQuerySupplementalCredentialsMessage,
    KerbTransferCredentialsMessage,
    KerbQueryTicketCacheEx2Message,   
} KERB_PROTOCOL_MESSAGE_TYPE, *PKERB_PROTOCOL_MESSAGE_TYPE;


//
// Used both for retrieving tickets and for querying ticket cache
//

typedef struct _KERB_QUERY_TKT_CACHE_REQUEST {
    KERB_PROTOCOL_MESSAGE_TYPE MessageType;
    LUID LogonId;
} KERB_QUERY_TKT_CACHE_REQUEST, *PKERB_QUERY_TKT_CACHE_REQUEST;


typedef struct _KERB_TICKET_CACHE_INFO {
    UNICODE_STRING ServerName;
    UNICODE_STRING RealmName;
    LARGE_INTEGER StartTime;
    LARGE_INTEGER EndTime;
    LARGE_INTEGER RenewTime;
    LONG EncryptionType;
    ULONG TicketFlags;
} KERB_TICKET_CACHE_INFO, *PKERB_TICKET_CACHE_INFO;


typedef struct _KERB_TICKET_CACHE_INFO_EX {
    UNICODE_STRING ClientName;
    UNICODE_STRING ClientRealm;
    UNICODE_STRING ServerName;
    UNICODE_STRING ServerRealm;
    LARGE_INTEGER StartTime;
    LARGE_INTEGER EndTime;
    LARGE_INTEGER RenewTime;
    LONG EncryptionType;
    ULONG TicketFlags;
} KERB_TICKET_CACHE_INFO_EX, *PKERB_TICKET_CACHE_INFO_EX;

typedef struct _KERB_TICKET_CACHE_INFO_EX2 {
    UNICODE_STRING ClientName;
    UNICODE_STRING ClientRealm;
    UNICODE_STRING ServerName;
    UNICODE_STRING ServerRealm;
    LARGE_INTEGER StartTime;
    LARGE_INTEGER EndTime;
    LARGE_INTEGER RenewTime;
    LONG EncryptionType;
    ULONG TicketFlags;
    
    //
    // the following are new in KERB_TICKET_CACHE_INFO_EX2 
    //
    
    ULONG SessionKeyType;
} KERB_TICKET_CACHE_INFO_EX2, *PKERB_TICKET_CACHE_INFO_EX2;

typedef struct _KERB_QUERY_TKT_CACHE_RESPONSE {
    KERB_PROTOCOL_MESSAGE_TYPE MessageType;
    ULONG CountOfTickets;
    KERB_TICKET_CACHE_INFO Tickets[ANYSIZE_ARRAY];
} KERB_QUERY_TKT_CACHE_RESPONSE, *PKERB_QUERY_TKT_CACHE_RESPONSE;

typedef struct _KERB_QUERY_TKT_CACHE_EX_RESPONSE {
    KERB_PROTOCOL_MESSAGE_TYPE MessageType;
    ULONG CountOfTickets;
    KERB_TICKET_CACHE_INFO_EX Tickets[ANYSIZE_ARRAY];
} KERB_QUERY_TKT_CACHE_EX_RESPONSE, *PKERB_QUERY_TKT_CACHE_EX_RESPONSE;

typedef struct _KERB_QUERY_TKT_CACHE_EX2_RESPONSE {
    KERB_PROTOCOL_MESSAGE_TYPE MessageType;
    ULONG CountOfTickets;
    KERB_TICKET_CACHE_INFO_EX2 Tickets[ANYSIZE_ARRAY];
} KERB_QUERY_TKT_CACHE_EX2_RESPONSE, *PKERB_QUERY_TKT_CACHE_EX2_RESPONSE;

//
// Types for retrieving encoded ticket from the cache
//

#ifndef __SECHANDLE_DEFINED__
typedef struct _SecHandle
{
    ULONG_PTR dwLower ;
    ULONG_PTR dwUpper ;
} SecHandle, * PSecHandle ;

#define __SECHANDLE_DEFINED__
#endif // __SECHANDLE_DEFINED__

// Ticket Flags
#define KERB_USE_DEFAULT_TICKET_FLAGS       0x0

// CacheOptions
#define KERB_RETRIEVE_TICKET_DEFAULT           0x0
#define KERB_RETRIEVE_TICKET_DONT_USE_CACHE    0x1
#define KERB_RETRIEVE_TICKET_USE_CACHE_ONLY    0x2
#define KERB_RETRIEVE_TICKET_USE_CREDHANDLE    0x4
#define KERB_RETRIEVE_TICKET_AS_KERB_CRED      0x8
#define KERB_RETRIEVE_TICKET_WITH_SEC_CRED    0x10
#define KERB_RETRIEVE_TICKET_CACHE_TICKET     0x20

// Encryption Type options
#define KERB_ETYPE_DEFAULT 0x0 // don't specify etype in tkt req.

typedef struct _KERB_AUTH_DATA {
    ULONG Type;
    ULONG Length;
    PUCHAR Data;
} KERB_AUTH_DATA, *PKERB_AUTH_DATA;


typedef struct _KERB_NET_ADDRESS {
    ULONG Family;
    ULONG Length;
    PCHAR Address;
} KERB_NET_ADDRESS, *PKERB_NET_ADDRESS;


typedef struct _KERB_NET_ADDRESSES {
    ULONG Number;
    KERB_NET_ADDRESS Addresses[ANYSIZE_ARRAY];
} KERB_NET_ADDRESSES, *PKERB_NET_ADDRESSES;

//
// Types for the information about a ticket
//

typedef struct _KERB_EXTERNAL_NAME {
    SHORT NameType;
    USHORT NameCount;
    UNICODE_STRING Names[ANYSIZE_ARRAY];
} KERB_EXTERNAL_NAME, *PKERB_EXTERNAL_NAME;


typedef struct _KERB_EXTERNAL_TICKET {
    PKERB_EXTERNAL_NAME ServiceName;
    PKERB_EXTERNAL_NAME TargetName;
    PKERB_EXTERNAL_NAME ClientName;
    UNICODE_STRING DomainName;
    UNICODE_STRING TargetDomainName;
    UNICODE_STRING AltTargetDomainName;  // contains ClientDomainName
    KERB_CRYPTO_KEY SessionKey;
    ULONG TicketFlags;
    ULONG Flags;
    LARGE_INTEGER KeyExpirationTime;
    LARGE_INTEGER StartTime;
    LARGE_INTEGER EndTime;
    LARGE_INTEGER RenewUntil;
    LARGE_INTEGER TimeSkew;
    ULONG EncodedTicketSize;
    PUCHAR EncodedTicket;
} KERB_EXTERNAL_TICKET, *PKERB_EXTERNAL_TICKET;

typedef struct _KERB_RETRIEVE_TKT_REQUEST {
    KERB_PROTOCOL_MESSAGE_TYPE MessageType;
    LUID LogonId;
    UNICODE_STRING TargetName;
    ULONG TicketFlags;
    ULONG CacheOptions;
    LONG EncryptionType;
    SecHandle CredentialsHandle;
} KERB_RETRIEVE_TKT_REQUEST, *PKERB_RETRIEVE_TKT_REQUEST;

typedef struct _KERB_RETRIEVE_TKT_RESPONSE {
    KERB_EXTERNAL_TICKET Ticket;
} KERB_RETRIEVE_TKT_RESPONSE, *PKERB_RETRIEVE_TKT_RESPONSE;

//
// Used to purge entries from the ticket cache
//

typedef struct _KERB_PURGE_TKT_CACHE_REQUEST {
    KERB_PROTOCOL_MESSAGE_TYPE MessageType;
    LUID LogonId;
    UNICODE_STRING ServerName;
    UNICODE_STRING RealmName;
} KERB_PURGE_TKT_CACHE_REQUEST, *PKERB_PURGE_TKT_CACHE_REQUEST;

//
// Flags for purge requests
//

#define KERB_PURGE_ALL_TICKETS 1

typedef struct _KERB_PURGE_TKT_CACHE_EX_REQUEST {
    KERB_PROTOCOL_MESSAGE_TYPE MessageType;
    LUID LogonId;
    ULONG Flags;
    KERB_TICKET_CACHE_INFO_EX TicketTemplate;
} KERB_PURGE_TKT_CACHE_EX_REQUEST, *PKERB_PURGE_TKT_CACHE_EX_REQUEST;  


//
// KerbChangePassword
//
// KerbChangePassword changes the password on the KDC account plus
//  the password cache and logon credentials if applicable.
//
//

typedef struct _KERB_CHANGEPASSWORD_REQUEST {
    KERB_PROTOCOL_MESSAGE_TYPE MessageType;
    UNICODE_STRING DomainName;
    UNICODE_STRING AccountName;
    UNICODE_STRING OldPassword;
    UNICODE_STRING NewPassword;
    BOOLEAN        Impersonating;
} KERB_CHANGEPASSWORD_REQUEST, *PKERB_CHANGEPASSWORD_REQUEST;



//
// KerbSetPassword
//
// KerbSetPassword changes the password on the KDC account plus
//  the password cache and logon credentials if applicable.
//
//
   
typedef struct _KERB_SETPASSWORD_REQUEST {
    KERB_PROTOCOL_MESSAGE_TYPE MessageType;
    LUID LogonId;
    SecHandle CredentialsHandle;
    ULONG Flags;
    UNICODE_STRING DomainName;
    UNICODE_STRING AccountName;
    UNICODE_STRING Password;
} KERB_SETPASSWORD_REQUEST, *PKERB_SETPASSWORD_REQUEST;


typedef struct _KERB_SETPASSWORD_EX_REQUEST {
    KERB_PROTOCOL_MESSAGE_TYPE MessageType;
    LUID LogonId;
    SecHandle CredentialsHandle;
    ULONG Flags;
    UNICODE_STRING AccountRealm;
    UNICODE_STRING AccountName;
    UNICODE_STRING Password;
    UNICODE_STRING ClientRealm;
    UNICODE_STRING ClientName;
    BOOLEAN        Impersonating;
    UNICODE_STRING KdcAddress;
    ULONG          KdcAddressType;
 } KERB_SETPASSWORD_EX_REQUEST, *PKERB_SETPASSWORD_EX_REQUEST;

                                                                   
#define DS_UNKNOWN_ADDRESS_TYPE         0 // anything *but* IP
#define KERB_SETPASS_USE_LOGONID        1
#define KERB_SETPASS_USE_CREDHANDLE     2


typedef struct _KERB_DECRYPT_REQUEST {
    KERB_PROTOCOL_MESSAGE_TYPE MessageType;
    LUID LogonId;
    ULONG Flags;
    LONG CryptoType;
    LONG KeyUsage;
    KERB_CRYPTO_KEY Key;        // optional
    ULONG EncryptedDataSize;
    ULONG InitialVectorSize;
    PUCHAR InitialVector;
    PUCHAR EncryptedData;
} KERB_DECRYPT_REQUEST, *PKERB_DECRYPT_REQUEST;

//
// If set, use the primary key from the current logon session of the one provided in the LogonId field.
// Otherwise, use the Key in the KERB_DECRYPT_MESSAGE.

#define KERB_DECRYPT_FLAG_DEFAULT_KEY   0x00000001


typedef struct _KERB_DECRYPT_RESPONSE  {
        UCHAR DecryptedData[ANYSIZE_ARRAY];
} KERB_DECRYPT_RESPONSE, *PKERB_DECRYPT_RESPONSE;


//
// Request structure for adding a binding cache entry. TCB privilege
// is required for this operation.
//

typedef struct _KERB_ADD_BINDING_CACHE_ENTRY_REQUEST {
    KERB_PROTOCOL_MESSAGE_TYPE MessageType;
    UNICODE_STRING RealmName;
    UNICODE_STRING KdcAddress;
    ULONG AddressType;                  //dsgetdc.h DS_NETBIOS_ADDRESS||DS_INET_ADDRESS
} KERB_ADD_BINDING_CACHE_ENTRY_REQUEST, *PKERB_ADD_BINDING_CACHE_ENTRY_REQUEST;

                       
//
// Request structure for reacquiring smartcard credentials for a 
// given LUID.
// Requires TCB.
//
typedef struct _KERB_REFRESH_SCCRED_REQUEST {
    KERB_PROTOCOL_MESSAGE_TYPE MessageType;
    UNICODE_STRING             CredentialBlob;   // optional
    LUID                       LogonId;
    ULONG                      Flags;
} KERB_REFRESH_SCCRED_REQUEST, *PKERB_REFRESH_SCCRED_REQUEST;

//
// Flags for KERB_REFRESH_SCCRED_REQUEST
//
//      KERB_REFRESH_SCCRED_RELEASE
//      Release the smartcard handle for LUID
//
//      KERB_REFRESH_SCCRED_GETTGT
//      Use the certificate hash in the blob to get a TGT for the logon 
//      session.
//
#define KERB_REFRESH_SCCRED_RELEASE             0x0  
#define KERB_REFRESH_SCCRED_GETTGT              0x1  

//
// Request structure for adding extra Server credentials to a given
// logon session.  Only applicable during AcceptSecurityContext, and
// requires TCB to alter "other" creds
//

typedef struct _KERB_ADD_CREDENTIALS_REQUEST {
    KERB_PROTOCOL_MESSAGE_TYPE MessageType;
    UNICODE_STRING UserName;
    UNICODE_STRING DomainName;
    UNICODE_STRING Password;
    LUID  LogonId; // optional
    ULONG Flags;
} KERB_ADD_CREDENTIALS_REQUEST, *PKERB_ADD_CREDENTIALS_REQUEST;



#define KERB_REQUEST_ADD_CREDENTIAL     1
#define KERB_REQUEST_REPLACE_CREDENTIAL 2
#define KERB_REQUEST_REMOVE_CREDENTIAL  4

//
// Request structure for transferring credentials between 2 luids.
// Requires TCB.
//

typedef struct _KERB_TRANSFER_CRED_REQUEST {
    KERB_PROTOCOL_MESSAGE_TYPE MessageType;
    LUID                       OriginLogonId;
    LUID                       DestinationLogonId;
    ULONG                      Flags;
} KERB_TRANSFER_CRED_REQUEST, *PKERB_TRANSFER_CRED_REQUEST;



#ifdef __cplusplus
}
#endif

#endif /* _NTSECAPI_ */


