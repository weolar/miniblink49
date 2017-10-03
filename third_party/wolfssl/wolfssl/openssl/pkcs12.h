/* pkcs12.h for openssl */

#include <wolfssl/wolfcrypt/pkcs12.h>

/* wolfCrypt level does not make use of ssl.h */
#define PKCS12         WC_PKCS12
#define PKCS12_new     wc_PKCS12_new
#define PKCS12_free    wc_PKCS12_free

/* wolfSSL level using structs from ssl.h and calls down to wolfCrypt */
#define d2i_PKCS12_bio wolfSSL_d2i_PKCS12_bio
#define PKCS12_parse   wolfSSL_PKCS12_parse
#define PKCS12_PBE_add wolfSSL_PKCS12_PBE_add

