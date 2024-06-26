
#ifndef common_SimpleRsa_h
#define common_SimpleRsa_h

#include "common/PrimeNumber.h"
#include <windows.h>

namespace common {

inline int genGcd(int a, int b)
{
    int t;

    while (b) {
        t = a;
        a = b;
        b = t%b;
    }
    return a;
}

inline bool primeW(int a, int b)
{
    if (genGcd(a, b) == 1)
        return true;
    else
        return false;
}

inline int modInverse(int a, int r)
{
    int b = 1;
    while (((a*b) % r) != 1) {
        b++;
        if (b < 0) {
            return -1;
        }
    }
    return b;
}

inline bool isPrime(int i)
{
#if 0
    if (i <= 1)
        return false;

    for (int j = 2; j < i; j++) {
        if (i%j == 0)
            return false;
    }
#endif
    if (i < kPrimeNumSetSize) {
        for (int j = 0; j < kPrimeNumSetSize; ++j) {
            if (i == kPrimeNumSet[j])
                return true;
        }
    }
    
    OutputDebugStringA("isPrime fail !\n");
    return false;
}

void genSecretKey(int* p, int *q)
{
    do {
        *p = rand() % 50 + 1;
    } while (!isPrime(*p));

    do {
        *q = rand() % 50 + 1;
    } while (p == q || !isPrime(*q));

}

int getRandE(int r)
{
    int e = 2;
    while (e < 1 || e > r || !primeW(e, r)) {
        e++;
        if (e < 0) {
            //printf("error ,function can't find e ,and now e is negative number");
            return -1;
        }
    }
    return e;
}

inline int genRsa(int a, int b, int c)
{
    int aa = a, r = 1;
    b = b + 1;

    while (b != 1) {
        r = r*aa;
        r = r%c;
        b--;
    }
    return r;
}

//     p,q is two big prime numbers
//     N = pq
//     r is the Euler's function output , r = (p-1)*(q-1)
//     e is the public key
//     d is the private key

int kLicense_p = 29;
int kLicense_q = 37;
int kLicense_N = 1073;
int kLicense_publicKey = 5;
int kLicense_privateKey = 605;
int kLicense_r = 1008;

void initRsaConst()
{
    common::genSecretKey(&kLicense_p, &kLicense_q);
    kLicense_N = kLicense_p * kLicense_q;
    kLicense_r = (kLicense_p - 1)*(kLicense_q - 1);
    kLicense_publicKey = common::getRandE(kLicense_r);
    kLicense_privateKey = common::modInverse(kLicense_publicKey, kLicense_r);
}

}

#endif // common_SimpleRsa_h