#ifndef _WIN_USER_SETTINGS_H_
#define _WIN_USER_SETTINGS_H_

/* Verify this is Windows */
#ifndef _WIN32
#error This user_settings.h header is only designed for Windows
#endif

/* Configurations */
#if defined(HAVE_FIPS)
/* FIPS */
#define OPENSSL_EXTRA
#define HAVE_THREAD_LS
#define WOLFSSL_KEY_GEN
#define HAVE_AESGCM
#define HAVE_HASHDRBG
#define WOLFSSL_SHA384
#define WOLFSSL_SHA512
#define NO_PSK
#define NO_HC128
#define NO_RC4
#define NO_RABBIT
#define NO_DSA
#define NO_MD4
#else
/* Enables blinding mode, to prevent timing attacks */
#define WC_RSA_BLINDING

#if defined(WOLFSSL_LIB)
/* The lib */
#define OPENSSL_EXTRA
#define WOLFSSL_RIPEMD
#define WOLFSSL_SHA512
#define NO_PSK
#define HAVE_EXTENDED_MASTER
#define WOLFSSL_SNIFFER
#define HAVE_TLS_EXTENSIONS
#define HAVE_SECURE_RENEGOTIATION
#else
/* The servers and clients */
#define OPENSSL_EXTRA
#define NO_PSK
#endif
#endif /* HAVE_FIPS */

#endif /* _WIN_USER_SETTINGS_H_ */
