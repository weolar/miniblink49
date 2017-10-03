/* asn1.h for openssl */

#ifndef WOLFSSL_ASN1_H_
#define WOLFSSL_ASN1_H_
struct WOLFSSL_ASN1_BIT_STRING {
    int length;
    int type;
    char* data;
    long flags;
};

struct WOLFSSL_ASN1_STRING {
    int length;
    int type;
    char* data;
    long flags;
};

#endif /* WOLFSSL_ASN1_H_ */
