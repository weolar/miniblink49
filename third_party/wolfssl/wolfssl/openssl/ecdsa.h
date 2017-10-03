/* ecdsa.h for openssl */

#ifndef WOLFSSL_ECDSA_H_
#define WOLFSSL_ECDSA_H_

#include <wolfssl/openssl/bn.h>


#ifdef __cplusplus
extern "C" {
#endif

#ifndef WOLFSSL_ECDSA_TYPE_DEFINED /* guard on redeclaration */
typedef struct WOLFSSL_ECDSA_SIG      WOLFSSL_ECDSA_SIG;
#define WOLFSSL_ECDSA_TYPE_DEFINED
#endif

typedef WOLFSSL_ECDSA_SIG             ECDSA_SIG;

struct WOLFSSL_ECDSA_SIG {
    WOLFSSL_BIGNUM *r;
    WOLFSSL_BIGNUM *s;
};

WOLFSSL_API void wolfSSL_ECDSA_SIG_free(WOLFSSL_ECDSA_SIG *sig);
WOLFSSL_API WOLFSSL_ECDSA_SIG *wolfSSL_ECDSA_SIG_new(void);
WOLFSSL_API WOLFSSL_ECDSA_SIG *wolfSSL_ECDSA_do_sign(const unsigned char *dgst,
                                                     int dgst_len,
                                                     WOLFSSL_EC_KEY *eckey);
WOLFSSL_API int wolfSSL_ECDSA_do_verify(const unsigned char *dgst,
                                        int dgst_len,
                                        const WOLFSSL_ECDSA_SIG *sig,
                                        WOLFSSL_EC_KEY *eckey);

#define ECDSA_SIG_free wolfSSL_ECDSA_SIG_free
#define ECDSA_SIG_new wolfSSL_ECDSA_SIG_new
#define ECDSA_do_sign wolfSSL_ECDSA_do_sign
#define ECDSA_do_verify wolfSSL_ECDSA_do_verify

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* header */

