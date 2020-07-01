#ifndef LDAP_CLIENT_DEFINED
#define LDAP_CLIENT_DEFINED


#define LDAP_API_INFO_VERSION     1
#define LDAP_API_VERSION          2004
#define LDAP_VERSION_MIN          2
#define LDAP_VERSION_MAX          3
#define LDAP_VENDOR_NAME          "Microsoft Corporation."
#define LDAP_VENDOR_NAME_W       L"Microsoft Corporation."
#define LDAP_VENDOR_VERSION       510

//
// The default version of the API is 2. If required, the user MUST set the
// version to 3 using the LDAP_OPT_VERSION option.
//

#define LDAP_VERSION1           1
#define LDAP_VERSION2           2
#define LDAP_VERSION3           3
#define LDAP_VERSION            LDAP_VERSION2


//
//  These are the values to pass to ldap_get/set_option :
//

#define LDAP_OPT_API_INFO           0x00
#define LDAP_OPT_DESC               0x01
#define LDAP_OPT_DEREF              0x02
#define LDAP_OPT_SIZELIMIT          0x03
#define LDAP_OPT_TIMELIMIT          0x04
#define LDAP_OPT_THREAD_FN_PTRS     0x05
#define LDAP_OPT_REBIND_FN          0x06
#define LDAP_OPT_REBIND_ARG         0x07
#define LDAP_OPT_REFERRALS          0x08
#define LDAP_OPT_RESTART            0x09

#define LDAP_OPT_SSL                0x0a
#define LDAP_OPT_IO_FN_PTRS         0x0b
#define LDAP_OPT_CACHE_FN_PTRS      0x0d
#define LDAP_OPT_CACHE_STRATEGY     0x0e
#define LDAP_OPT_CACHE_ENABLE       0x0f
#define LDAP_OPT_REFERRAL_HOP_LIMIT 0x10

#define LDAP_OPT_PROTOCOL_VERSION   0x11        // known by two names.
#define LDAP_OPT_VERSION            0x11
#define LDAP_OPT_API_FEATURE_INFO   0x15

//
// We'll make the error codes compatible with reference implementation
//

typedef enum {
    LDAP_SUCCESS = 0x00,
    LDAP_OPERATIONS_ERROR = 0x01,
    LDAP_PROTOCOL_ERROR = 0x02,
    LDAP_TIMELIMIT_EXCEEDED = 0x03,
    LDAP_SIZELIMIT_EXCEEDED = 0x04,
    LDAP_COMPARE_FALSE = 0x05,
    LDAP_COMPARE_TRUE = 0x06,
    LDAP_AUTH_METHOD_NOT_SUPPORTED = 0x07,
    LDAP_STRONG_AUTH_REQUIRED = 0x08,
    LDAP_REFERRAL_V2 = 0x09,
    LDAP_PARTIAL_RESULTS = 0x09,
    LDAP_REFERRAL = 0x0a,
    LDAP_ADMIN_LIMIT_EXCEEDED = 0x0b,
    LDAP_UNAVAILABLE_CRIT_EXTENSION = 0x0c,
    LDAP_CONFIDENTIALITY_REQUIRED = 0x0d,
    LDAP_SASL_BIND_IN_PROGRESS = 0x0e,

    LDAP_NO_SUCH_ATTRIBUTE = 0x10,
    LDAP_UNDEFINED_TYPE = 0x11,
    LDAP_INAPPROPRIATE_MATCHING = 0x12,
    LDAP_CONSTRAINT_VIOLATION = 0x13,
    LDAP_ATTRIBUTE_OR_VALUE_EXISTS = 0x14,
    LDAP_INVALID_SYNTAX = 0x15,

    LDAP_NO_SUCH_OBJECT = 0x20,
    LDAP_ALIAS_PROBLEM = 0x21,
    LDAP_INVALID_DN_SYNTAX = 0x22,
    LDAP_IS_LEAF = 0x23,
    LDAP_ALIAS_DEREF_PROBLEM = 0x24,

    LDAP_INAPPROPRIATE_AUTH = 0x30,
    LDAP_INVALID_CREDENTIALS = 0x31,
    LDAP_INSUFFICIENT_RIGHTS = 0x32,
    LDAP_BUSY = 0x33,
    LDAP_UNAVAILABLE = 0x34,
    LDAP_UNWILLING_TO_PERFORM = 0x35,
    LDAP_LOOP_DETECT = 0x36,
    LDAP_SORT_CONTROL_MISSING = 0x3C,
    LDAP_OFFSET_RANGE_ERROR = 0x3D,

    LDAP_NAMING_VIOLATION = 0x40,
    LDAP_OBJECT_CLASS_VIOLATION = 0x41,
    LDAP_NOT_ALLOWED_ON_NONLEAF = 0x42,
    LDAP_NOT_ALLOWED_ON_RDN = 0x43,
    LDAP_ALREADY_EXISTS = 0x44,
    LDAP_NO_OBJECT_CLASS_MODS = 0x45,
    LDAP_RESULTS_TOO_LARGE = 0x46,
    LDAP_AFFECTS_MULTIPLE_DSAS = 0x47,

    LDAP_VIRTUAL_LIST_VIEW_ERROR = 0x4c,

    LDAP_OTHER = 0x50,
    LDAP_SERVER_DOWN = 0x51,
    LDAP_LOCAL_ERROR = 0x52,
    LDAP_ENCODING_ERROR = 0x53,
    LDAP_DECODING_ERROR = 0x54,
    LDAP_TIMEOUT = 0x55,
    LDAP_AUTH_UNKNOWN = 0x56,
    LDAP_FILTER_ERROR = 0x57,
    LDAP_USER_CANCELLED = 0x58,
    LDAP_PARAM_ERROR = 0x59,
    LDAP_NO_MEMORY = 0x5a,
    LDAP_CONNECT_ERROR = 0x5b,
    LDAP_NOT_SUPPORTED = 0x5c,
    LDAP_NO_RESULTS_RETURNED = 0x5e,
    LDAP_CONTROL_NOT_FOUND = 0x5d,
    LDAP_MORE_RESULTS_TO_RETURN = 0x5f,

    LDAP_CLIENT_LOOP = 0x60,
    LDAP_REFERRAL_LIMIT_EXCEEDED = 0x61
} LDAP_RETCODE;


#if !defined(_WIN64)
#pragma pack(push, 4)
#endif

typedef struct ldap {

    struct {

        UINT_PTR sb_sd;

        UCHAR Reserved1[(10 * sizeof(ULONG)) + 1];

        ULONG_PTR sb_naddr;   // notzero implies CLDAP available

        UCHAR Reserved2[(6 * sizeof(ULONG))];

    } ld_sb;

    //
    //  Following parameters MAY match up to reference implementation of LDAP
    //

    PCHAR   ld_host;
    ULONG   ld_version;
    UCHAR   ld_lberoptions;

    //
    //  Safe to assume that these parameters are in same location as
    //  reference implementation of LDAP API.
    //

    ULONG   ld_deref;

    ULONG   ld_timelimit;
    ULONG   ld_sizelimit;

    ULONG   ld_errno;
    PCHAR   ld_matched;
    PCHAR   ld_error;
    ULONG   ld_msgid;

    UCHAR Reserved3[(6 * sizeof(ULONG)) + 1];

    //
    //  Following parameters may match up to reference implementation of LDAP API.
    //

    ULONG   ld_cldaptries;
    ULONG   ld_cldaptimeout;
    ULONG   ld_refhoplimit;
    ULONG   ld_options;

} LDAP, *PLDAP;

typedef struct ldapmsg {

    ULONG lm_msgid;             // message number for given connection
    ULONG lm_msgtype;           // message type of the form LDAP_RES_xxx

    PVOID lm_ber;               // ber form of message

    struct ldapmsg *lm_chain;   // pointer to next result value
    struct ldapmsg *lm_next;    // pointer to next message
    ULONG lm_time;

    //
    //  new fields below not in reference implementation
    //

    PLDAP   Connection;         // connection from which we received response
    PVOID   Request;            // owning request (opaque structure)
    ULONG   lm_returncode;      // server's return code
    USHORT  lm_referral;        // index of referral within ref table
    BOOLEAN lm_chased;          // has referral been chased already?
    BOOLEAN lm_eom;             // is this the last entry for this message?
    BOOLEAN ConnectionReferenced; // is the Connection still valid?

} LDAPMessage, *PLDAPMessage;



#if !defined(_WIN64)
#pragma pack(pop)
#endif

typedef struct berelement {
    PCHAR   opaque;     // this is an opaque structure used just for
                        // compatibility with reference implementation
} BerElement;
#define NULLBER ((BerElement *) 0)

typedef struct berval {
    ULONG  bv_len;
    PCHAR  bv_val;
} LDAP_BERVAL, *PLDAP_BERVAL, BERVAL, *PBERVAL, BerValue;


//
//  Synchronous and asynch search routines.
//
//  filter follows RFC 1960 with the addition that '(' ')' '*' ' ' '\' and
//   '\0' are all escaped with '\'
//
// Scope of search.  This corresponds to the "scope" parameter on search

#define LDAP_SCOPE_BASE         0x00
#define LDAP_SCOPE_ONELEVEL     0x01
#define LDAP_SCOPE_SUBTREE      0x02

#endif