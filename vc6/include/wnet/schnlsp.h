
#ifndef schnlsp_h
#define schnlsp_h

#ifndef LDAP_AUTH_OTHERKIND
#define LDAP_AUTH_OTHERKIND             0x86L

// The SICILY type covers package negotiation to MSN servers.
// Each of the supported types can also be specified without
// doing the package negotiation, assuming the caller knows
// what the server supports.

#define LDAP_AUTH_SICILY                (LDAP_AUTH_OTHERKIND | 0x0200)

#define LDAP_AUTH_MSN                   (LDAP_AUTH_OTHERKIND | 0x0800)
#define LDAP_AUTH_NTLM                  (LDAP_AUTH_OTHERKIND | 0x1000)
#define LDAP_AUTH_DPA                   (LDAP_AUTH_OTHERKIND | 0x2000)

// This will cause the client to use the GSSAPI negotiation
// package to determine the most appropriate authentication type.
// This type should be used when talking to NT5.

#define LDAP_AUTH_NEGOTIATE             (LDAP_AUTH_OTHERKIND | 0x0400)

#define LDAP_AUTH_DIGEST                (LDAP_AUTH_OTHERKIND | 0x4000)

#endif // LDAP_AUTH_OTHERKIND

#endif // schnlsp_h