/* bn.h for openssl */


#ifndef WOLFSSL_BN_H_
#define WOLFSSL_BN_H_

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/integer.h>

#ifdef __cplusplus
    extern "C" {
#endif

typedef struct WOLFSSL_BIGNUM {
    int   neg;              /* openssh deference */
    void* internal;         /* our big num */
} WOLFSSL_BIGNUM;


#define WOLFSSL_BN_ULONG mp_digit

typedef struct WOLFSSL_BN_CTX WOLFSSL_BN_CTX;
typedef struct WOLFSSL_BN_GENCB WOLFSSL_BN_GENCB;

WOLFSSL_API WOLFSSL_BN_CTX* wolfSSL_BN_CTX_new(void);
WOLFSSL_API void           wolfSSL_BN_CTX_init(WOLFSSL_BN_CTX*);
WOLFSSL_API void           wolfSSL_BN_CTX_free(WOLFSSL_BN_CTX*);

WOLFSSL_API WOLFSSL_BIGNUM* wolfSSL_BN_new(void);
WOLFSSL_API void           wolfSSL_BN_free(WOLFSSL_BIGNUM*);
WOLFSSL_API void           wolfSSL_BN_clear_free(WOLFSSL_BIGNUM*);


WOLFSSL_API int wolfSSL_BN_sub(WOLFSSL_BIGNUM*, const WOLFSSL_BIGNUM*,
                             const WOLFSSL_BIGNUM*);
WOLFSSL_API int wolfSSL_BN_mod(WOLFSSL_BIGNUM*, const WOLFSSL_BIGNUM*,
                             const WOLFSSL_BIGNUM*, const WOLFSSL_BN_CTX*);
WOLFSSL_API int wolfSSL_BN_mod_exp(WOLFSSL_BIGNUM *r, const WOLFSSL_BIGNUM *a,
        const WOLFSSL_BIGNUM *p, const WOLFSSL_BIGNUM *m, WOLFSSL_BN_CTX *ctx);
WOLFSSL_API const WOLFSSL_BIGNUM* wolfSSL_BN_value_one(void);


WOLFSSL_API int wolfSSL_BN_num_bytes(const WOLFSSL_BIGNUM*);
WOLFSSL_API int wolfSSL_BN_num_bits(const WOLFSSL_BIGNUM*);

WOLFSSL_API int wolfSSL_BN_is_zero(const WOLFSSL_BIGNUM*);
WOLFSSL_API int wolfSSL_BN_is_one(const WOLFSSL_BIGNUM*);
WOLFSSL_API int wolfSSL_BN_is_odd(const WOLFSSL_BIGNUM*);

WOLFSSL_API int wolfSSL_BN_cmp(const WOLFSSL_BIGNUM*, const WOLFSSL_BIGNUM*);

WOLFSSL_API int wolfSSL_BN_bn2bin(const WOLFSSL_BIGNUM*, unsigned char*);
WOLFSSL_API WOLFSSL_BIGNUM* wolfSSL_BN_bin2bn(const unsigned char*, int len,
                                WOLFSSL_BIGNUM* ret);

WOLFSSL_API int wolfSSL_mask_bits(WOLFSSL_BIGNUM*, int n);

WOLFSSL_API int wolfSSL_BN_rand(WOLFSSL_BIGNUM*, int bits, int top, int bottom);
WOLFSSL_API int wolfSSL_BN_is_bit_set(const WOLFSSL_BIGNUM*, int n);
WOLFSSL_API int wolfSSL_BN_hex2bn(WOLFSSL_BIGNUM**, const char* str);

WOLFSSL_API WOLFSSL_BIGNUM* wolfSSL_BN_dup(const WOLFSSL_BIGNUM*);
WOLFSSL_API WOLFSSL_BIGNUM* wolfSSL_BN_copy(WOLFSSL_BIGNUM*,
                                            const WOLFSSL_BIGNUM*);

WOLFSSL_API int   wolfSSL_BN_dec2bn(WOLFSSL_BIGNUM**, const char* str);
WOLFSSL_API char* wolfSSL_BN_bn2dec(const WOLFSSL_BIGNUM*);

WOLFSSL_API int wolfSSL_BN_lshift(WOLFSSL_BIGNUM*, const WOLFSSL_BIGNUM*, int);
WOLFSSL_API int wolfSSL_BN_add_word(WOLFSSL_BIGNUM*, WOLFSSL_BN_ULONG);
WOLFSSL_API int wolfSSL_BN_set_bit(WOLFSSL_BIGNUM*, int);
WOLFSSL_API int wolfSSL_BN_set_word(WOLFSSL_BIGNUM*, WOLFSSL_BN_ULONG);

WOLFSSL_API int wolfSSL_BN_add(WOLFSSL_BIGNUM*, WOLFSSL_BIGNUM*,
                               WOLFSSL_BIGNUM*);
WOLFSSL_API char *wolfSSL_BN_bn2hex(const WOLFSSL_BIGNUM*);
WOLFSSL_API int wolfSSL_BN_is_prime_ex(const WOLFSSL_BIGNUM*, int,
                                       WOLFSSL_BN_CTX*, WOLFSSL_BN_GENCB*);
WOLFSSL_API WOLFSSL_BN_ULONG wolfSSL_BN_mod_word(const WOLFSSL_BIGNUM*,
                                                 WOLFSSL_BN_ULONG);
#if !defined(NO_FILESYSTEM) && !defined(NO_STDIO_FILESYSTEM)
    WOLFSSL_API int wolfSSL_BN_print_fp(FILE*, const WOLFSSL_BIGNUM*);
#endif
WOLFSSL_API int wolfSSL_BN_rshift(WOLFSSL_BIGNUM*, const WOLFSSL_BIGNUM*, int);
WOLFSSL_API WOLFSSL_BIGNUM *wolfSSL_BN_CTX_get(WOLFSSL_BN_CTX *ctx);
WOLFSSL_API void wolfSSL_BN_CTX_start(WOLFSSL_BN_CTX *ctx);

typedef WOLFSSL_BIGNUM BIGNUM;
typedef WOLFSSL_BN_CTX BN_CTX;
typedef WOLFSSL_BN_GENCB BN_GENCB;

#define BN_CTX_new        wolfSSL_BN_CTX_new
#define BN_CTX_init       wolfSSL_BN_CTX_init
#define BN_CTX_free       wolfSSL_BN_CTX_free

#define BN_new        wolfSSL_BN_new
#define BN_free       wolfSSL_BN_free
#define BN_clear_free wolfSSL_BN_clear_free

#define BN_num_bytes wolfSSL_BN_num_bytes
#define BN_num_bits  wolfSSL_BN_num_bits

#define BN_is_zero  wolfSSL_BN_is_zero
#define BN_is_one   wolfSSL_BN_is_one
#define BN_is_odd   wolfSSL_BN_is_odd

#define BN_cmp    wolfSSL_BN_cmp

#define BN_bn2bin  wolfSSL_BN_bn2bin
#define BN_bin2bn  wolfSSL_BN_bin2bn

#define BN_mod       wolfSSL_BN_mod
#define BN_mod_exp   wolfSSL_BN_mod_exp
#define BN_sub       wolfSSL_BN_sub
#define BN_value_one wolfSSL_BN_value_one

#define BN_mask_bits wolfSSL_mask_bits

#define BN_rand       wolfSSL_BN_rand
#define BN_is_bit_set wolfSSL_BN_is_bit_set
#define BN_hex2bn     wolfSSL_BN_hex2bn

#define BN_dup  wolfSSL_BN_dup
#define BN_copy wolfSSL_BN_copy

#define BN_set_word wolfSSL_BN_set_word

#define BN_dec2bn wolfSSL_BN_dec2bn
#define BN_bn2dec wolfSSL_BN_bn2dec
#define BN_bn2hex wolfSSL_BN_bn2hex

#define BN_lshift wolfSSL_BN_lshift
#define BN_add_word wolfSSL_BN_add_word
#define BN_add wolfSSL_BN_add
#define BN_set_word wolfSSL_BN_set_word
#define BN_set_bit wolfSSL_BN_set_bit


#define BN_is_prime_ex wolfSSL_BN_is_prime_ex
#define BN_print_fp wolfSSL_BN_print_fp
#define BN_rshift wolfSSL_BN_rshift
#define BN_mod_word wolfSSL_BN_mod_word

#define BN_CTX_get wolfSSL_BN_CTX_get
#define BN_CTX_start wolfSSL_BN_CTX_start

#ifdef __cplusplus
    }  /* extern "C" */
#endif


#endif /* WOLFSSL__H_ */
