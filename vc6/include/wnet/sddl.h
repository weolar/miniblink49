/*++

Copyright (c) 1997-1998  Microsoft Corporation

Module Name:

    sddl.w

Abstract:

    This module defines the support and conversions routines necessary for SDDL.

Revision History:

--*/

#ifndef __SDDL_H__
#define __SDDL_H__


#ifdef __cplusplus
extern "C" {
#endif

//
// SDDL Version information
//
#define SDDL_REVISION_1     1
#define SDDL_REVISION       SDDL_REVISION_1

//
// SDDL Component tags
//
#define SDDL_OWNER                          TEXT("O")       // Owner tag
#define SDDL_GROUP                          TEXT("G")       // Group tag
#define SDDL_DACL                           TEXT("D")       // DACL tag
#define SDDL_SACL                           TEXT("S")       // SACL tag

//
// SDDL Security descriptor controls
//
#define SDDL_PROTECTED                      TEXT("P")       // DACL or SACL Protected
#define SDDL_AUTO_INHERIT_REQ               TEXT("AR")      // Auto inherit request
#define SDDL_AUTO_INHERITED                 TEXT("AI")      // DACL/SACL are auto inherited

//
// SDDL Ace types
//
#define SDDL_ACCESS_ALLOWED                 TEXT("A")   // Access allowed
#define SDDL_ACCESS_DENIED                  TEXT("D")   // Access denied
#define SDDL_OBJECT_ACCESS_ALLOWED          TEXT("OA")  // Object access allowed
#define SDDL_OBJECT_ACCESS_DENIED           TEXT("OD")  // Object access denied
#define SDDL_AUDIT                          TEXT("AU")  // Audit
#define SDDL_ALARM                          TEXT("AL")  // Alarm
#define SDDL_OBJECT_AUDIT                   TEXT("OU")  // Object audit
#define SDDL_OBJECT_ALARM                   TEXT("OL")  // Object alarm

//
// SDDL Ace flags
//
#define SDDL_CONTAINER_INHERIT              TEXT("CI")  // Container inherit
#define SDDL_OBJECT_INHERIT                 TEXT("OI")  // Object inherit
#define SDDL_NO_PROPAGATE                   TEXT("NP")  // Inherit no propagate
#define SDDL_INHERIT_ONLY                   TEXT("IO")  // Inherit only
#define SDDL_INHERITED                      TEXT("ID")  // Inherited
#define SDDL_AUDIT_SUCCESS                  TEXT("SA")  // Audit success
#define SDDL_AUDIT_FAILURE                  TEXT("FA")  // Audit failure


//
// SDDL Rights
//
#define SDDL_READ_PROPERTY                  TEXT("RP")
#define SDDL_WRITE_PROPERTY                 TEXT("WP")
#define SDDL_CREATE_CHILD                   TEXT("CC")
#define SDDL_DELETE_CHILD                   TEXT("DC")
#define SDDL_LIST_CHILDREN                  TEXT("LC")
#define SDDL_SELF_WRITE                     TEXT("SW")
#define SDDL_LIST_OBJECT                    TEXT("LO")
#define SDDL_DELETE_TREE                    TEXT("DT")
#define SDDL_CONTROL_ACCESS                 TEXT("CR")
#define SDDL_READ_CONTROL                   TEXT("RC")
#define SDDL_WRITE_DAC                      TEXT("WD")
#define SDDL_WRITE_OWNER                    TEXT("WO")
#define SDDL_STANDARD_DELETE                TEXT("SD")
#define SDDL_GENERIC_ALL                    TEXT("GA")
#define SDDL_GENERIC_READ                   TEXT("GR")
#define SDDL_GENERIC_WRITE                  TEXT("GW")
#define SDDL_GENERIC_EXECUTE                TEXT("GX")
#define SDDL_FILE_ALL                       TEXT("FA")
#define SDDL_FILE_READ                      TEXT("FR")
#define SDDL_FILE_WRITE                     TEXT("FW")
#define SDDL_FILE_EXECUTE                   TEXT("FX")
#define SDDL_KEY_ALL                        TEXT("KA")
#define SDDL_KEY_READ                       TEXT("KR")
#define SDDL_KEY_WRITE                      TEXT("KW")
#define SDDL_KEY_EXECUTE                    TEXT("KX")

//
// SDDL User alias max size
//      - currently, upto two supported eg. "DA"
//      - modify this if more WCHARs need to be there in future e.g. "DAX"
//

#define SDDL_ALIAS_SIZE                     2

//
// SDDL User aliases
//
#define SDDL_DOMAIN_ADMINISTRATORS          TEXT("DA")      // Domain admins
#define SDDL_DOMAIN_GUESTS                  TEXT("DG")      // Domain guests
#define SDDL_DOMAIN_USERS                   TEXT("DU")      // Domain users
#define SDDL_ENTERPRISE_DOMAIN_CONTROLLERS  TEXT("ED")      // Enterprise domain controllers
#define SDDL_DOMAIN_DOMAIN_CONTROLLERS      TEXT("DD")      // Domain domain controllers
#define SDDL_DOMAIN_COMPUTERS               TEXT("DC")      // Domain computers
#define SDDL_BUILTIN_ADMINISTRATORS         TEXT("BA")      // Builtin (local ) administrators
#define SDDL_BUILTIN_GUESTS                 TEXT("BG")      // Builtin (local ) guests
#define SDDL_BUILTIN_USERS                  TEXT("BU")      // Builtin (local ) users
#define SDDL_LOCAL_ADMIN                    TEXT("LA")      // Local administrator account
#define SDDL_LOCAL_GUEST                    TEXT("LG")      // Local group account
#define SDDL_ACCOUNT_OPERATORS              TEXT("AO")      // Account operators
#define SDDL_BACKUP_OPERATORS               TEXT("BO")      // Backup operators
#define SDDL_PRINTER_OPERATORS              TEXT("PO")      // Printer operators
#define SDDL_SERVER_OPERATORS               TEXT("SO")      // Server operators
#define SDDL_AUTHENTICATED_USERS            TEXT("AU")      // Authenticated users
#define SDDL_PERSONAL_SELF                  TEXT("PS")      // Personal self
#define SDDL_CREATOR_OWNER                  TEXT("CO")      // Creator owner
#define SDDL_CREATOR_GROUP                  TEXT("CG")      // Creator group
#define SDDL_LOCAL_SYSTEM                   TEXT("SY")      // Local system
#define SDDL_POWER_USERS                    TEXT("PU")      // Power users
#define SDDL_EVERYONE                       TEXT("WD")      // Everyone ( World )
#define SDDL_REPLICATOR                     TEXT("RE")      // Replicator
#define SDDL_INTERACTIVE                    TEXT("IU")      // Interactive logon user
#define SDDL_NETWORK                        TEXT("NU")      // Nework logon user
#define SDDL_SERVICE                        TEXT("SU")      // Service logon user
#define SDDL_RESTRICTED_CODE                TEXT("RC")      // Restricted code
#define SDDL_ANONYMOUS                      TEXT("AN")      // Anonymous Logon
#define SDDL_SCHEMA_ADMINISTRATORS          TEXT("SA")      // Schema Administrators
#define SDDL_CERT_SERV_ADMINISTRATORS       TEXT("CA")      // Certificate Server Administrators
#define SDDL_RAS_SERVERS                    TEXT("RS")      // RAS servers group
#define SDDL_ENTERPRISE_ADMINS              TEXT("EA")      // Enterprise administrators
#define SDDL_GROUP_POLICY_ADMINS            TEXT("PA")      // Group Policy administrators
#define SDDL_ALIAS_PREW2KCOMPACC            TEXT("RU")      // alias to allow previous windows 2000
#define SDDL_LOCAL_SERVICE                  TEXT("LS")      // Local service account (for services)
#define SDDL_NETWORK_SERVICE                TEXT("NS")      // Network service account (for services)
#define SDDL_REMOTE_DESKTOP                 TEXT("RD")      // Remote desktop users (for terminal server)
#define SDDL_NETWORK_CONFIGURATION_OPS      TEXT("NO")      // Network configuration operators ( to manage configuration of networking features)
#define SDDL_PERFMON_USERS                  TEXT("MU")      // Performance Monitor Users
#define SDDL_PERFLOG_USERS                  TEXT("LU")      // Performance Log Users



//
// SDDL Seperators - character version
//
#define SDDL_SEPERATORC                     TEXT(';')
#define SDDL_DELIMINATORC                   TEXT(':')
#define SDDL_ACE_BEGINC                     TEXT('(')
#define SDDL_ACE_ENDC                       TEXT(')')

//
// SDDL Seperators - string version
//
#define SDDL_SEPERATOR                     TEXT(";")
#define SDDL_DELIMINATOR                   TEXT(":")
#define SDDL_ACE_BEGIN                     TEXT("(")
#define SDDL_ACE_END                       TEXT(")")

#if !defined(_NTDDK_)

#if(_WIN32_WINNT >= 0x0500)

WINADVAPI
BOOL
WINAPI
ConvertSidToStringSidA(
    IN  PSID     Sid,
    OUT LPSTR  *StringSid
    );
WINADVAPI
BOOL
WINAPI
ConvertSidToStringSidW(
    IN  PSID     Sid,
    OUT LPWSTR  *StringSid
    );
#ifdef UNICODE
#define ConvertSidToStringSid  ConvertSidToStringSidW
#else
#define ConvertSidToStringSid  ConvertSidToStringSidA
#endif // !UNICODE

WINADVAPI
BOOL
WINAPI
ConvertStringSidToSidA(
    IN LPCSTR   StringSid,
    OUT PSID   *Sid
    );
WINADVAPI
BOOL
WINAPI
ConvertStringSidToSidW(
    IN LPCWSTR   StringSid,
    OUT PSID   *Sid
    );
#ifdef UNICODE
#define ConvertStringSidToSid  ConvertStringSidToSidW
#else
#define ConvertStringSidToSid  ConvertStringSidToSidA
#endif // !UNICODE

WINADVAPI
BOOL
WINAPI
ConvertStringSecurityDescriptorToSecurityDescriptorA(
    IN  LPCSTR StringSecurityDescriptor,
    IN  DWORD StringSDRevision,
    OUT PSECURITY_DESCRIPTOR  *SecurityDescriptor,
    OUT PULONG  SecurityDescriptorSize OPTIONAL
    );
WINADVAPI
BOOL
WINAPI
ConvertStringSecurityDescriptorToSecurityDescriptorW(
    IN  LPCWSTR StringSecurityDescriptor,
    IN  DWORD StringSDRevision,
    OUT PSECURITY_DESCRIPTOR  *SecurityDescriptor,
    OUT PULONG  SecurityDescriptorSize OPTIONAL
    );
#ifdef UNICODE
#define ConvertStringSecurityDescriptorToSecurityDescriptor  ConvertStringSecurityDescriptorToSecurityDescriptorW
#else
#define ConvertStringSecurityDescriptorToSecurityDescriptor  ConvertStringSecurityDescriptorToSecurityDescriptorA
#endif // !UNICODE


WINADVAPI
BOOL
WINAPI
ConvertSecurityDescriptorToStringSecurityDescriptorA(
    IN  PSECURITY_DESCRIPTOR  SecurityDescriptor,
    IN  DWORD RequestedStringSDRevision,
    IN  SECURITY_INFORMATION SecurityInformation,
    OUT LPSTR  *StringSecurityDescriptor OPTIONAL,
    OUT PULONG StringSecurityDescriptorLen OPTIONAL
    );
WINADVAPI
BOOL
WINAPI
ConvertSecurityDescriptorToStringSecurityDescriptorW(
    IN  PSECURITY_DESCRIPTOR  SecurityDescriptor,
    IN  DWORD RequestedStringSDRevision,
    IN  SECURITY_INFORMATION SecurityInformation,
    OUT LPWSTR  *StringSecurityDescriptor OPTIONAL,
    OUT PULONG StringSecurityDescriptorLen OPTIONAL
    );
#ifdef UNICODE
#define ConvertSecurityDescriptorToStringSecurityDescriptor  ConvertSecurityDescriptorToStringSecurityDescriptorW
#else
#define ConvertSecurityDescriptorToStringSecurityDescriptor  ConvertSecurityDescriptorToStringSecurityDescriptorA
#endif // !UNICODE

#endif /* _WIN32_WINNT >=  0x0500 */

#endif /* !defined(_NTDDK_) */


#ifdef __cplusplus
}
#endif

#endif  // endif __SDDL_H__



