/* asn.c
 *
 * Copyright (C) 2006-2016 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */


#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

/*
ASN Options:
 * NO_ASN_TIME: Disables time parts of the ASN code for systems without an RTC
    or wishing to save space.
 * IGNORE_NAME_CONSTRAINTS: Skip ASN name checks.
 * ASN_DUMP_OID: Allows dump of OID information for debugging.
 * RSA_DECODE_EXTRA: Decodes extra information in RSA public key.
 * WOLFSSL_CERT_GEN: Cert generation. Saves extra certificate info in GetName.
 * WOLFSSL_NO_OCSP_OPTIONAL_CERTS: Skip optional OCSP certs (responder issuer
    must still be trusted)
 * WOLFSSL_NO_TRUSTED_CERTS_VERIFY: Workaround for situation where entire cert
    chain is not loaded. This only matches on subject and public key and
    does not perform a PKI validation, so it is not a secure solution.
    Only enabled for OCSP.
 * WOLFSSL_NO_OCSP_ISSUER_CHECK: Can be defined for backwards compatibility to
    disable checking of OCSP subject hash with issuer hash.
*/

#ifndef NO_ASN

#ifdef HAVE_RTP_SYS
    #include "os.h"           /* dc_rtc_api needs    */
    #include "dc_rtc_api.h"   /* to get current time */
#endif

#include <wolfssl/wolfcrypt/asn.h>
#include <wolfssl/wolfcrypt/coding.h>
#include <wolfssl/wolfcrypt/md2.h>
#include <wolfssl/wolfcrypt/hmac.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/pwdbased.h>
#include <wolfssl/wolfcrypt/des3.h>
#include <wolfssl/wolfcrypt/logging.h>

#include <wolfssl/wolfcrypt/random.h>
#include <wolfssl/wolfcrypt/hash.h>
#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

#ifndef NO_RC4
    #include <wolfssl/wolfcrypt/arc4.h>
#endif

#ifdef HAVE_NTRU
    #include "libntruencrypt/ntru_crypto.h"
#endif

#if defined(WOLFSSL_SHA512) || defined(WOLFSSL_SHA384)
    #include <wolfssl/wolfcrypt/sha512.h>
#endif

#ifndef NO_SHA256
    #include <wolfssl/wolfcrypt/sha256.h>
#endif

#ifdef HAVE_ECC
    #include <wolfssl/wolfcrypt/ecc.h>
#endif

#ifdef HAVE_ED25519
    #include <wolfssl/wolfcrypt/ed25519.h>
#endif

#ifndef NO_RSA
    #include <wolfssl/wolfcrypt/rsa.h>
#endif

#ifdef WOLFSSL_DEBUG_ENCODING
    #if defined(FREESCALE_MQX) || defined(FREESCALE_KSDK_MQX)
        #if MQX_USE_IO_OLD
            #include <fio.h>
        #else
            #include <nio.h>
        #endif
    #else
        #include <stdio.h>
    #endif
#endif

#ifdef _MSC_VER
    /* 4996 warning to use MS extensions e.g., strcpy_s instead of XSTRNCPY */
    #pragma warning(disable: 4996)
#endif

#define ERROR_OUT(err, eLabel) { ret = (err); goto eLabel; }

#ifndef NO_ASN_TIME
#if defined(USER_TIME)
    /* Use our gmtime and time_t/struct tm types.
       Only needs seconds since EPOCH using XTIME function.
       time_t XTIME(time_t * timer) {}
    */
    #define WOLFSSL_GMTIME
    #define USE_WOLF_TM
    #define USE_WOLF_TIME_T

#elif defined(TIME_OVERRIDES)
    /* Override XTIME() and XGMTIME() functionality.
       Requires user to provide these functions:
        time_t XTIME(time_t * timer) {}
        struct tm* XGMTIME(const time_t* timer, struct tm* tmp) {}
    */
    #ifndef HAVE_TIME_T_TYPE
        #define USE_WOLF_TIME_T
    #endif
    #ifndef HAVE_TM_TYPE
        #define USE_WOLF_TM
    #endif
    #define NEED_TMP_TIME

#elif defined(HAVE_RTP_SYS)
    /* uses parital <time.h> structures */
    #define XTIME(tl)       (0)
    #define XGMTIME(c, t)   rtpsys_gmtime((c))

#elif defined(MICRIUM)
    #include <clk.h>
    #include <time.h>
    #define XTIME(t1)       micrium_time((t1))
    #define WOLFSSL_GMTIME

#elif defined(MICROCHIP_TCPIP_V5) || defined(MICROCHIP_TCPIP)
    #include <time.h>
    #define XTIME(t1)       pic32_time((t1))
    #define XGMTIME(c, t)   gmtime((c))

#elif defined(FREESCALE_MQX) || defined(FREESCALE_KSDK_MQX)
    #define XTIME(t1)       mqx_time((t1))
    #define HAVE_GMTIME_R

#elif defined(FREESCALE_KSDK_BM) || defined(FREESCALE_FREE_RTOS) || defined(FREESCALE_KSDK_FREERTOS)
    #include <time.h>
    #ifndef XTIME
        /*extern time_t ksdk_time(time_t* timer);*/
        #define XTIME(t1)   ksdk_time((t1))
    #endif
    #define XGMTIME(c, t)   gmtime((c))

#elif defined(WOLFSSL_ATMEL)
    #define XTIME(t1)       atmel_get_curr_time_and_date((t1))
    #define WOLFSSL_GMTIME
    #define USE_WOLF_TM
    #define USE_WOLF_TIME_T

#elif defined(IDIRECT_DEV_TIME)
    /*Gets the timestamp from cloak software owned by VT iDirect
    in place of time() from <time.h> */
    #include <time.h>
    #define XTIME(t1)       idirect_time((t1))
    #define XGMTIME(c, t)   gmtime((c))

#elif defined(_WIN32_WCE)
    #include <windows.h>
    #define XTIME(t1)       windows_time((t1))
    #define WOLFSSL_GMTIME
#else

    /* default */
    /* uses complete <time.h> facility */
    #include <time.h>
#endif


/* Map default time functions */
#if !defined(XTIME) && !defined(TIME_OVERRIDES) && !defined(USER_TIME)
    #define XTIME(tl)       time((tl))
#endif
#if !defined(XGMTIME) && !defined(TIME_OVERRIDES)
    #if defined(WOLFSSL_GMTIME) || !defined(HAVE_GMTIME_R)
        #define XGMTIME(c, t)   gmtime((c))
    #else
        #define XGMTIME(c, t)   gmtime_r((c), (t))
        #define NEED_TMP_TIME
    #endif
#endif
#if !defined(XVALIDATE_DATE) && !defined(HAVE_VALIDATE_DATE)
    #define USE_WOLF_VALIDDATE
    #define XVALIDATE_DATE(d, f, t) ValidateDate((d), (f), (t))
#endif

/* wolf struct tm and time_t */
#if defined(USE_WOLF_TM)
    struct tm {
        int  tm_sec;     /* seconds after the minute [0-60] */
        int  tm_min;     /* minutes after the hour [0-59] */
        int  tm_hour;    /* hours since midnight [0-23] */
        int  tm_mday;    /* day of the month [1-31] */
        int  tm_mon;     /* months since January [0-11] */
        int  tm_year;    /* years since 1900 */
        int  tm_wday;    /* days since Sunday [0-6] */
        int  tm_yday;    /* days since January 1 [0-365] */
        int  tm_isdst;   /* Daylight Savings Time flag */
        long tm_gmtoff;  /* offset from CUT in seconds */
        char *tm_zone;   /* timezone abbreviation */
    };
#endif /* USE_WOLF_TM */
#if defined(USE_WOLF_TIME_T)
    typedef long time_t;
#endif

/* forward declarations */
#if defined(USER_TIME)
    struct tm* gmtime(const time_t* timer);
    extern time_t XTIME(time_t * timer);

    #ifdef STACK_TRAP
        /* for stack trap tracking, don't call os gmtime on OS X/linux,
           uses a lot of stack spce */
        extern time_t time(time_t * timer);
        #define XTIME(tl)  time((tl))
    #endif /* STACK_TRAP */

#elif defined(TIME_OVERRIDES)
    extern time_t XTIME(time_t * timer);
    extern struct tm* XGMTIME(const time_t* timer, struct tm* tmp);
#elif defined(WOLFSSL_GMTIME)
    struct tm* gmtime(const time_t* timer);
#endif


#if defined(_WIN32_WCE)
time_t windows_time(time_t* timer)
{
    SYSTEMTIME     sysTime;
    FILETIME       fTime;
    ULARGE_INTEGER intTime;
    time_t         localTime;

    if (timer == NULL)
        timer = &localTime;

    GetSystemTime(&sysTime);
    SystemTimeToFileTime(&sysTime, &fTime);

    XMEMCPY(&intTime, &fTime, sizeof(FILETIME));
    /* subtract EPOCH */
    intTime.QuadPart -= 0x19db1ded53e8000;
    /* to secs */
    intTime.QuadPart /= 10000000;
    *timer = (time_t)intTime.QuadPart;

    return *timer;
}
#endif /*  _WIN32_WCE */

#if defined(WOLFSSL_GMTIME)
struct tm* gmtime(const time_t* timer)
{
    #define YEAR0          1900
    #define EPOCH_YEAR     1970
    #define SECS_DAY       (24L * 60L * 60L)
    #define LEAPYEAR(year) (!((year) % 4) && (((year) % 100) || !((year) %400)))
    #define YEARSIZE(year) (LEAPYEAR(year) ? 366 : 365)

    static const int _ytab[2][12] =
    {
        {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
        {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
    };

    static struct tm st_time;
    struct tm* ret = &st_time;
    time_t secs = *timer;
    unsigned long dayclock, dayno;
    int year = EPOCH_YEAR;

    dayclock = (unsigned long)secs % SECS_DAY;
    dayno    = (unsigned long)secs / SECS_DAY;

    ret->tm_sec  = (int) dayclock % 60;
    ret->tm_min  = (int)(dayclock % 3600) / 60;
    ret->tm_hour = (int) dayclock / 3600;
    ret->tm_wday = (int) (dayno + 4) % 7;        /* day 0 a Thursday */

    while(dayno >= (unsigned long)YEARSIZE(year)) {
        dayno -= YEARSIZE(year);
        year++;
    }

    ret->tm_year = year - YEAR0;
    ret->tm_yday = (int)dayno;
    ret->tm_mon  = 0;

    while(dayno >= (unsigned long)_ytab[LEAPYEAR(year)][ret->tm_mon]) {
        dayno -= _ytab[LEAPYEAR(year)][ret->tm_mon];
        ret->tm_mon++;
    }

    ret->tm_mday  = (int)++dayno;
    ret->tm_isdst = 0;

    return ret;
}
#endif /* WOLFSSL_GMTIME */


#if defined(HAVE_RTP_SYS)
#define YEAR0          1900

struct tm* rtpsys_gmtime(const time_t* timer)       /* has a gmtime() but hangs */
{
    static struct tm st_time;
    struct tm* ret = &st_time;

    DC_RTC_CALENDAR cal;
    dc_rtc_time_get(&cal, TRUE);

    ret->tm_year  = cal.year - YEAR0;       /* gm starts at 1900 */
    ret->tm_mon   = cal.month - 1;          /* gm starts at 0 */
    ret->tm_mday  = cal.day;
    ret->tm_hour  = cal.hour;
    ret->tm_min   = cal.minute;
    ret->tm_sec   = cal.second;

    return ret;
}

#endif /* HAVE_RTP_SYS */


#if defined(MICROCHIP_TCPIP_V5) || defined(MICROCHIP_TCPIP)

/*
 * time() is just a stub in Microchip libraries. We need our own
 * implementation. Use SNTP client to get seconds since epoch.
 */
time_t pic32_time(time_t* timer)
{
#ifdef MICROCHIP_TCPIP_V5
    DWORD sec = 0;
#else
    uint32_t sec = 0;
#endif
    time_t localTime;

    if (timer == NULL)
        timer = &localTime;

#ifdef MICROCHIP_MPLAB_HARMONY
    sec = TCPIP_SNTP_UTCSecondsGet();
#else
    sec = SNTPGetUTCSeconds();
#endif
    *timer = (time_t) sec;

    return *timer;
}

#endif /* MICROCHIP_TCPIP || MICROCHIP_TCPIP_V5 */


#if defined(MICRIUM)

time_t micrium_time(time_t* timer)
{
    CLK_TS_SEC sec;

    Clk_GetTS_Unix(&sec);

    return (time_t) sec;
}

#endif /* MICRIUM */


#if defined(FREESCALE_MQX) || defined(FREESCALE_KSDK_MQX)

time_t mqx_time(time_t* timer)
{
    time_t localTime;
    TIME_STRUCT time_s;

    if (timer == NULL)
        timer = &localTime;

    _time_get(&time_s);
    *timer = (time_t) time_s.SECONDS;

    return *timer;
}

#endif /* FREESCALE_MQX || FREESCALE_KSDK_MQX */


#if defined(WOLFSSL_TIRTOS)

time_t XTIME(time_t * timer)
{
    time_t sec = 0;

    sec = (time_t) Seconds_get();

    if (timer != NULL)
        *timer = sec;

    return sec;
}

#endif /* WOLFSSL_TIRTOS */


#if defined(WOLFSSL_XILINX)
#include "xrtcpsu.h"

time_t XTIME(time_t * timer)
{
    time_t sec = 0;
    XRtcPsu_Config* con;
    XRtcPsu         rtc;

    con = XRtcPsu_LookupConfig(XPAR_XRTCPSU_0_DEVICE_ID);
    if (con != NULL) {
        if (XRtcPsu_CfgInitialize(&rtc, con, con->BaseAddr) == XST_SUCCESS) {
            sec = (time_t)XRtcPsu_GetCurrentTime(&rtc);
        }
        else {
            WOLFSSL_MSG("Unable to initialize RTC");
        }
    }

    if (timer != NULL)
        *timer = sec;

    return sec;
}

#endif /* WOLFSSL_TIRTOS */


static INLINE word32 btoi(byte b)
{
    return (word32)(b - 0x30);
}


/* two byte date/time, add to value */
static INLINE void GetTime(int* value, const byte* date, int* idx)
{
    int i = *idx;

    *value += btoi(date[i++]) * 10;
    *value += btoi(date[i++]);

    *idx = i;
}


#if defined(IDIRECT_DEV_TIME)

extern time_t getTimestamp();

time_t idirect_time(time_t * timer)
{
    time_t sec = getTimestamp();

    if (timer != NULL)
        *timer = sec;

    return sec;
}

#endif /* IDIRECT_DEV_TIME */

#endif /* !NO_ASN_TIME */


WOLFSSL_LOCAL int GetLength(const byte* input, word32* inOutIdx, int* len,
                           word32 maxIdx)
{
    int     length = 0;
    word32  idx = *inOutIdx;
    byte    b;

    *len = 0;    /* default length */

    if ((idx + 1) > maxIdx) {   /* for first read */
        WOLFSSL_MSG("GetLength bad index on input");
        return BUFFER_E;
    }

    b = input[idx++];
    if (b >= ASN_LONG_LENGTH) {
        word32 bytes = b & 0x7F;

        if ((idx + bytes) > maxIdx) {   /* for reading bytes */
            WOLFSSL_MSG("GetLength bad long length");
            return BUFFER_E;
        }

        while (bytes--) {
            b = input[idx++];
            length = (length << 8) | b;
        }
    }
    else
        length = b;

    if ((idx + length) > maxIdx) {   /* for user of length */
        WOLFSSL_MSG("GetLength value exceeds buffer length");
        return BUFFER_E;
    }

    *inOutIdx = idx;
    if (length > 0)
        *len = length;

    return length;
}


/* Get the DER/BER encoding of an ASN.1 header.
 *
 * input     Buffer holding DER/BER encoded data.
 * tag       ASN.1 tag value expected in header.
 * inOutIdx  Current index into buffer to parse.
 * len       The number of bytes in the ASN.1 data.
 * maxIdx    Length of data in buffer.
 * returns BUFFER_E when there is not enough data to parse.
 *         ASN_PARSE_E when the expected tag is not found or length is invalid.
 *         Otherwise, the number of bytes in the ASN.1 data.
 */
static int GetASNHeader(const byte* input, byte tag, word32* inOutIdx, int* len,
                        word32 maxIdx)
{
    word32 idx = *inOutIdx;
    byte   b;
    int    length;

    if ((idx + 1) > maxIdx)
        return BUFFER_E;

    b = input[idx++];
    if (b != tag)
        return ASN_PARSE_E;

    if (GetLength(input, &idx, &length, maxIdx) < 0)
        return ASN_PARSE_E;

    *len      = length;
    *inOutIdx = idx;
    return length;
}

WOLFSSL_LOCAL int GetSequence(const byte* input, word32* inOutIdx, int* len,
                           word32 maxIdx)
{
    return GetASNHeader(input, ASN_SEQUENCE | ASN_CONSTRUCTED, inOutIdx, len,
                        maxIdx);
}


WOLFSSL_LOCAL int GetSet(const byte* input, word32* inOutIdx, int* len,
                        word32 maxIdx)
{
    return GetASNHeader(input, ASN_SET | ASN_CONSTRUCTED, inOutIdx, len,
                        maxIdx);
}

/* Get the DER/BER encoded ASN.1 NULL element.
 * Ensure that the all fields are as expected and move index past the element.
 *
 * input     Buffer holding DER/BER encoded data.
 * inOutIdx  Current index into buffer to parse.
 * maxIdx    Length of data in buffer.
 * returns BUFFER_E when there is not enough data to parse.
 *         ASN_TAG_NULL_E when the NULL tag is not found.
 *         ASN_EXPECT_0_E when the length is not zero.
 *         Otherwise, 0 to indicate success.
 */
static int GetASNNull(const byte* input, word32* inOutIdx, word32 maxIdx)
{
    word32 idx = *inOutIdx;
    byte   b;

    if ((idx + 2) > maxIdx)
        return BUFFER_E;

    b = input[idx++];
    if (b != ASN_TAG_NULL)
        return ASN_TAG_NULL_E;

    if (input[idx++] != 0)
        return ASN_EXPECT_0_E;

    *inOutIdx = idx;
    return 0;
}

/* Set the DER/BER encoding of the ASN.1 NULL element.
 *
 * output  Buffer to write into.
 * returns the number of bytes added to the buffer.
 */
static int SetASNNull(byte* output)
{
    output[0] = ASN_TAG_NULL;
    output[1] = 0;

    return 2;
}

/* Get the DER/BER encoding of an ASN.1 BOOLEAN.
 *
 * input     Buffer holding DER/BER encoded data.
 * inOutIdx  Current index into buffer to parse.
 * maxIdx    Length of data in buffer.
 * returns BUFFER_E when there is not enough data to parse.
 *         ASN_PARSE_E when the BOOLEAN tag is not found or length is not 1.
 *         Otherwise, 0 to indicate the value was false and 1 to indicate true.
 */
static int GetBoolean(const byte* input, word32* inOutIdx, word32 maxIdx)
{
    word32 idx = *inOutIdx;
    byte   b;

    if ((idx + 3) > maxIdx)
        return BUFFER_E;

    b = input[idx++];
    if (b != ASN_BOOLEAN)
        return ASN_PARSE_E;

    if (input[idx++] != 1)
        return ASN_PARSE_E;

    b = input[idx++] != 0;

    *inOutIdx = idx;
    return b;
}

#ifdef ASN1_SET_BOOLEAN
/* Set the DER/BER encoding of the ASN.1 NULL element.
 * Note: Function not required as yet.
 *
 * val     Boolean value to encode.
 * output  Buffer to write into.
 * returns the number of bytes added to the buffer.
 */
static int SetBoolean(int val, byte* output)
{
    output[0] = ASN_BOOLEAN;
    output[1] = 1;
    output[2] = val ? -1 : 0;

    return 3;
}
#endif

/* Get the DER/BER encoding of an ASN.1 OCTET_STRING header.
 *
 * input     Buffer holding DER/BER encoded data.
 * inOutIdx  Current index into buffer to parse.
 * len       The number of bytes in the ASN.1 data.
 * maxIdx    Length of data in buffer.
 * returns BUFFER_E when there is not enough data to parse.
 *         ASN_PARSE_E when the OCTET_STRING tag is not found or length is
 *         invalid.
 *         Otherwise, the number of bytes in the ASN.1 data.
 */
static int GetOctetString(const byte* input, word32* inOutIdx, int* len,
                          word32 maxIdx)
{
    return GetASNHeader(input, ASN_OCTET_STRING, inOutIdx, len, maxIdx);
}

/* Get the DER/BER encoding of an ASN.1 INTEGER header.
 * Removes the leading zero byte when found.
 *
 * input     Buffer holding DER/BER encoded data.
 * inOutIdx  Current index into buffer to parse.
 * len       The number of bytes in the ASN.1 data (excluding any leading zero).
 * maxIdx    Length of data in buffer.
 * returns BUFFER_E when there is not enough data to parse.
 *         ASN_PARSE_E when the INTEGER tag is not found, length is invalid,
 *         or invalid use of or missing leading zero.
 *         Otherwise, 0 to indicate success.
 */
static int GetASNInt(const byte* input, word32* inOutIdx, int* len,
                     word32 maxIdx)
{
    int    ret;

    ret = GetASNHeader(input, ASN_INTEGER, inOutIdx, len, maxIdx);
    if (ret < 0)
        return ret;

    if (*len > 0) {
        /* remove leading zero, unless there is only one 0x00 byte */
        if ((input[*inOutIdx] == 0x00) && (*len > 1)) {
            (*inOutIdx)++;
            (*len)--;

            if (*len > 0 && (input[*inOutIdx] & 0x80) == 0)
                return ASN_PARSE_E;
        }
        else if ((input[*inOutIdx] & 0x80) == 0x80)
            return ASN_PARSE_E;
    }

    return 0;
}

/* Get the DER/BER encoding of an ASN.1 INTEGER that has a value of no more than
 * 7 bits.
 *
 * input     Buffer holding DER/BER encoded data.
 * inOutIdx  Current index into buffer to parse.
 * maxIdx    Length of data in buffer.
 * returns BUFFER_E when there is not enough data to parse.
 *         ASN_PARSE_E when the INTEGER tag is not found or length is invalid.
 *         Otherwise, the 7-bit value.
 */
static int GetInteger7Bit(const byte* input, word32* inOutIdx, word32 maxIdx)
{
    word32 idx = *inOutIdx;
    byte   b;

    if ((idx + 3) > maxIdx)
        return BUFFER_E;

    if (input[idx++] != ASN_INTEGER)
        return ASN_PARSE_E;
    if (input[idx++] != 1)
        return ASN_PARSE_E;
    b = input[idx++];

    *inOutIdx = idx;
    return b;
}

#if !defined(NO_DSA) || defined(HAVE_ECC) || (!defined(NO_RSA) && (defined(WOLFSSL_CERT_GEN) || (defined(WOLFSSL_KEY_GEN) && !defined(HAVE_USER_RSA))))
/* Set the DER/BER encoding of the ASN.1 INTEGER header.
 *
 * len        Length of data to encode.
 * firstByte  First byte of data, most significant byte of integer, to encode.
 * output     Buffer to write into.
 * returns the number of bytes added to the buffer.
 */
static int SetASNInt(int len, byte firstByte, byte* output)
{
    word32 idx = 0;

    output[idx++] = ASN_INTEGER;
    if (firstByte & 0x80)
        len++;
    idx += SetLength(len, output + idx);
    if (firstByte & 0x80)
        output[idx++] = 0x00;

    return idx;
}
#endif

#if !defined(NO_DSA) || defined(HAVE_ECC) || defined(WOLFSSL_CERT_GEN) || (defined(WOLFSSL_KEY_GEN) && !defined(NO_RSA) && !defined(HAVE_USER_RSA))
/* Set the DER/BER encoding of the ASN.1 INTEGER element with an mp_int.
 * The number is assumed to be positive.
 *
 * n       Multi-precision integer to encode.
 * maxSz   Maximum size of the encoded integer.
 *         A negative value indicates no check of length requested.
 * output  Buffer to write into.
 * returns BUFFER_E when the data is too long for the buffer.
 *         MP_TO_E when encoding the integer fails.
 *         Otherwise, the number of bytes added to the buffer.
 */
static int SetASNIntMP(mp_int* n, int maxSz, byte* output)
{
    int idx = 0;
    int leadingBit;
    int length;
    int err;

    leadingBit = mp_leading_bit(n);
    length = mp_unsigned_bin_size(n);
    idx = SetASNInt(length, leadingBit ? 0x80 : 0x00, output);
    if (maxSz >= 0 && (idx + length) > maxSz)
        return BUFFER_E;

    err = mp_to_unsigned_bin(n, output + idx);
    if (err != MP_OKAY)
        return MP_TO_E;
    idx += length;

    return idx;
}
#endif

#if !defined(NO_RSA) && defined(HAVE_USER_RSA) && defined(WOLFSSL_CERT_GEN)
/* Set the DER/BER encoding of the ASN.1 INTEGER element with an mp_int from
 * an RSA key.
 * The number is assumed to be positive.
 *
 * n       Multi-precision integer to encode.
 * output  Buffer to write into.
 * returns BUFFER_E when the data is too long for the buffer.
 *         MP_TO_E when encoding the integer fails.
 *         Otherwise, the number of bytes added to the buffer.
 */
static int SetASNIntRSA(mp_int* n, byte* output)
{
    int idx = 0;
    int leadingBit;
    int length;
    int err;

    leadingBit = wc_Rsa_leading_bit(n);
    length = wc_Rsa_unsigned_bin_size(n);
    idx = SetASNInt(length, leadingBit ? 0x80 : 0x00, output);
    if ((idx + length) > MAX_RSA_INT_SZ)
        return BUFFER_E;

    err = wc_Rsa_to_unsigned_bin(n, output + idx, length);
    if (err != MP_OKAY)
        return MP_TO_E;
    idx += length;

    return idx;
}
#endif /* !NO_RSA && (WOLFSSL_CERT_GEN || (WOLFSSL_KEY_GEN &&
                                           !HAVE_USER_RSA))) */

/* Windows header clash for WinCE using GetVersion */
WOLFSSL_LOCAL int GetMyVersion(const byte* input, word32* inOutIdx,
                               int* version, word32 maxIdx)
{
    word32 idx = *inOutIdx;

    if ((idx + MIN_VERSION_SZ) > maxIdx)
        return ASN_PARSE_E;

    if (input[idx++] != ASN_INTEGER)
        return ASN_PARSE_E;

    if (input[idx++] != 0x01)
        return ASN_VERSION_E;

    *version  = input[idx++];
    *inOutIdx = idx;

    return *version;
}


#ifndef NO_PWDBASED
/* Get small count integer, 32 bits or less */
int GetShortInt(const byte* input, word32* inOutIdx, int* number, word32 maxIdx)
{
    word32 idx = *inOutIdx;
    word32 len;

    *number = 0;

    /* check for type and length bytes */
    if ((idx + 2) > maxIdx)
        return BUFFER_E;

    if (input[idx++] != ASN_INTEGER)
        return ASN_PARSE_E;

    len = input[idx++];
    if (len > 4)
        return ASN_PARSE_E;

    if (len + idx > maxIdx)
        return ASN_PARSE_E;

    while (len--) {
        *number  = *number << 8 | input[idx++];
    }

    *inOutIdx = idx;

    return *number;
}
#endif /* !NO_PWDBASED */

/* May not have one, not an error */
static int GetExplicitVersion(const byte* input, word32* inOutIdx, int* version,
                              word32 maxIdx)
{
    word32 idx = *inOutIdx;

    WOLFSSL_ENTER("GetExplicitVersion");

    if ((idx + 1) > maxIdx)
        return BUFFER_E;

    if (input[idx++] == (ASN_CONTEXT_SPECIFIC | ASN_CONSTRUCTED)) {
        *inOutIdx = ++idx;  /* skip header */
        return GetMyVersion(input, inOutIdx, version, maxIdx);
    }

    /* go back as is */
    *version = 0;

    return 0;
}

int GetInt(mp_int* mpi, const byte* input, word32* inOutIdx, word32 maxIdx)
{
    word32 idx = *inOutIdx;
    int    ret;
    int    length;

    ret = GetASNInt(input, &idx, &length, maxIdx);
    if (ret != 0)
        return ret;

    if (mp_init(mpi) != MP_OKAY)
        return MP_INIT_E;

    if (mp_read_unsigned_bin(mpi, (byte*)input + idx, length) != 0) {
        mp_clear(mpi);
        return ASN_GETINT_E;
    }

#ifdef HAVE_WOLF_BIGINT
    if (wc_bigint_from_unsigned_bin(&mpi->raw, input + idx, length) != 0) {
        mp_clear(mpi);
        return ASN_GETINT_E;
    }
#endif /* HAVE_WOLF_BIGINT */

    *inOutIdx = idx + length;

    return 0;
}

static int CheckBitString(const byte* input, word32* inOutIdx, int* len,
                          word32 maxIdx, int zeroBits, byte* unusedBits)
{
    word32 idx = *inOutIdx;
    int    length;
    byte   b;

    if ((idx + 1) > maxIdx)
        return BUFFER_E;

    if (input[idx++] != ASN_BIT_STRING)
        return ASN_BITSTR_E;

    if (GetLength(input, &idx, &length, maxIdx) < 0)
        return ASN_PARSE_E;

    /* extra sanity check that length is greater than 0 */
    if (length <= 0) {
        WOLFSSL_MSG("Error length was 0 in CheckBitString");
        return BUFFER_E;
    }

    if (idx + 1 > maxIdx) {
        WOLFSSL_MSG("Attempted buffer read larger than input buffer");
        return BUFFER_E;
    }

    b = input[idx];
    if (zeroBits && b != 0x00)
        return ASN_EXPECT_0_E;
    if (b >= 0x08)
        return ASN_PARSE_E;
    if (b != 0) {
        if ((byte)(input[idx + length - 1] << (8 - b)) != 0)
            return ASN_PARSE_E;
    }
    idx++;
    length--; /* length has been checked for greater than 0 */

    *inOutIdx = idx;
    if (len != NULL)
        *len = length;
    if (unusedBits != NULL)
        *unusedBits = b;

    return 0;
}

#if (!defined(NO_RSA) && (defined(WOLFSSL_CERT_GEN) || \
                          (defined(WOLFSSL_KEY_GEN) && \
                           !defined(HAVE_USER_RSA)))) || \
    (defined(HAVE_ECC) && (defined(WOLFSSL_CERT_GEN) || \
                           defined(WOLFSSL_KEY_GEN)))
/* Set the DER/BER encoding of the ASN.1 BIT_STRING header.
 *
 * len         Length of data to encode.
 * unusedBits  The number of unused bits in the last byte of data.
 *             That is, the number of least significant zero bits before a one.
 *             The last byte is the most-significant non-zero byte of a number.
 * output      Buffer to write into.
 * returns the number of bytes added to the buffer.
 */
static word32 SetBitString(word32 len, byte unusedBits, byte* output)
{
    word32 idx = 0;

    output[idx++] = ASN_BIT_STRING;
    idx += SetLength(len + 1, output + idx);
    output[idx++] = unusedBits;

    return idx;
}

#ifdef WOLFSSL_CERT_EXT
/* Set the DER/BER encoding of the ASN.1 BIT_STRING with a 16-bit value.
 *
 * val         16-bit value to encode.
 * output      Buffer to write into.
 * returns the number of bytes added to the buffer.
 */
static word32 SetBitString16Bit(word16 val, byte* output)
{
    word32 idx;
    int    len;
    byte   lastByte;
    byte   unusedBits = 0;

    if ((val >> 8) != 0) {
        len = 2;
        lastByte = (byte)(val >> 8);
    }
    else {
        len = 1;
        lastByte = (byte)val;
    }

    while (((lastByte >> unusedBits) & 0x01) == 0x00)
        unusedBits++;

    idx = SetBitString(len, unusedBits, output);
    output[idx++] = (byte)val;
    if (len > 1)
        output[idx++] = (byte)(val >> 8);

    return idx;
}
#endif /* WOLFSSL_CERT_EXT */
#endif /* !NO_RSA && (WOLFSSL_CERT_GEN || (WOLFSSL_KEY_GEN &&
                                           !HAVE_USER_RSA)) */



/* hashType */
static const byte hashMd2hOid[] = {42, 134, 72, 134, 247, 13, 2, 2};
static const byte hashMd5hOid[] = {42, 134, 72, 134, 247, 13, 2, 5};
static const byte hashSha1hOid[] = {43, 14, 3, 2, 26};
static const byte hashSha224hOid[] = {96, 134, 72, 1, 101, 3, 4, 2, 4};
static const byte hashSha256hOid[] = {96, 134, 72, 1, 101, 3, 4, 2, 1};
static const byte hashSha384hOid[] = {96, 134, 72, 1, 101, 3, 4, 2, 2};
static const byte hashSha512hOid[] = {96, 134, 72, 1, 101, 3, 4, 2, 3};

/* sigType */
#ifndef NO_DSA
    static const byte sigSha1wDsaOid[] = {42, 134, 72, 206, 56, 4, 3};
#endif /* NO_DSA */
#ifndef NO_RSA
    static const byte sigMd2wRsaOid[] = {42, 134, 72, 134, 247, 13, 1, 1, 2};
    static const byte sigMd5wRsaOid[] = {42, 134, 72, 134, 247, 13, 1, 1, 4};
    static const byte sigSha1wRsaOid[] = {42, 134, 72, 134, 247, 13, 1, 1, 5};
    static const byte sigSha224wRsaOid[] = {42, 134, 72, 134, 247, 13, 1, 1,14};
    static const byte sigSha256wRsaOid[] = {42, 134, 72, 134, 247, 13, 1, 1,11};
    static const byte sigSha384wRsaOid[] = {42, 134, 72, 134, 247, 13, 1, 1,12};
    static const byte sigSha512wRsaOid[] = {42, 134, 72, 134, 247, 13, 1, 1,13};
#endif /* NO_RSA */
#ifdef HAVE_ECC
    static const byte sigSha1wEcdsaOid[] = {42, 134, 72, 206, 61, 4, 1};
    static const byte sigSha224wEcdsaOid[] = {42, 134, 72, 206, 61, 4, 3, 1};
    static const byte sigSha256wEcdsaOid[] = {42, 134, 72, 206, 61, 4, 3, 2};
    static const byte sigSha384wEcdsaOid[] = {42, 134, 72, 206, 61, 4, 3, 3};
    static const byte sigSha512wEcdsaOid[] = {42, 134, 72, 206, 61, 4, 3, 4};
#endif /* HAVE_ECC */
#ifdef HAVE_ED25519
    static const byte sigEd25519Oid[] = {43, 101, 112};
#endif /* HAVE_ED25519 */

/* keyType */
#ifndef NO_DSA
    static const byte keyDsaOid[] = {42, 134, 72, 206, 56, 4, 1};
#endif /* NO_DSA */
#ifndef NO_RSA
    static const byte keyRsaOid[] = {42, 134, 72, 134, 247, 13, 1, 1, 1};
#endif /* NO_RSA */
#ifdef HAVE_NTRU
    static const byte keyNtruOid[] = {43, 6, 1, 4, 1, 193, 22, 1, 1, 1, 1};
#endif /* HAVE_NTRU */
#ifdef HAVE_ECC
    static const byte keyEcdsaOid[] = {42, 134, 72, 206, 61, 2, 1};
#endif /* HAVE_ECC */
#ifdef HAVE_ED25519
    static const byte keyEd25519Oid[] = {43, 101, 112};
#endif /* HAVE_ED25519 */

/* curveType */
#ifdef HAVE_ECC
    /* See "ecc_sets" table in ecc.c */
#endif /* HAVE_ECC */

/* blkType */
static const byte blkAes128CbcOid[] = {96, 134, 72, 1, 101, 3, 4, 1, 2};
static const byte blkAes192CbcOid[] = {96, 134, 72, 1, 101, 3, 4, 1, 22};
static const byte blkAes256CbcOid[] = {96, 134, 72, 1, 101, 3, 4, 1, 42};
static const byte blkDesCbcOid[]  = {43, 14, 3, 2, 7};
static const byte blkDes3CbcOid[] = {42, 134, 72, 134, 247, 13, 3, 7};

/* keyWrapType */
static const byte wrapAes128Oid[] = {96, 134, 72, 1, 101, 3, 4, 1, 5};
static const byte wrapAes192Oid[] = {96, 134, 72, 1, 101, 3, 4, 1, 25};
static const byte wrapAes256Oid[] = {96, 134, 72, 1, 101, 3, 4, 1, 45};

/* cmsKeyAgreeType */
static const byte dhSinglePass_stdDH_sha1kdf_Oid[]   =
                                          {43, 129, 5, 16, 134, 72, 63, 0, 2};
static const byte dhSinglePass_stdDH_sha224kdf_Oid[] = {43, 129, 4, 1, 11, 0};
static const byte dhSinglePass_stdDH_sha256kdf_Oid[] = {43, 129, 4, 1, 11, 1};
static const byte dhSinglePass_stdDH_sha384kdf_Oid[] = {43, 129, 4, 1, 11, 2};
static const byte dhSinglePass_stdDH_sha512kdf_Oid[] = {43, 129, 4, 1, 11, 3};

/* ocspType */
#ifdef HAVE_OCSP
    static const byte ocspBasicOid[] = {43, 6, 1, 5, 5, 7, 48, 1, 1};
    static const byte ocspNonceOid[] = {43, 6, 1, 5, 5, 7, 48, 1, 2};
#endif /* HAVE_OCSP */

/* certExtType */
static const byte extBasicCaOid[] = {85, 29, 19};
static const byte extAltNamesOid[] = {85, 29, 17};
static const byte extCrlDistOid[] = {85, 29, 31};
static const byte extAuthInfoOid[] = {43, 6, 1, 5, 5, 7, 1, 1};
static const byte extAuthKeyOid[] = {85, 29, 35};
static const byte extSubjKeyOid[] = {85, 29, 14};
static const byte extCertPolicyOid[] = {85, 29, 32};
static const byte extKeyUsageOid[] = {85, 29, 15};
static const byte extInhibitAnyOid[] = {85, 29, 54};
static const byte extExtKeyUsageOid[] = {85, 29, 37};
static const byte extNameConsOid[] = {85, 29, 30};

/* certAuthInfoType */
static const byte extAuthInfoOcspOid[] = {43, 6, 1, 5, 5, 7, 48, 1};
static const byte extAuthInfoCaIssuerOid[] = {43, 6, 1, 5, 5, 7, 48, 2};

/* certPolicyType */
static const byte extCertPolicyAnyOid[] = {85, 29, 32, 0};

/* certKeyUseType */
static const byte extAltNamesHwNameOid[] = {43, 6, 1, 5, 5, 7, 8, 4};

/* certKeyUseType */
static const byte extExtKeyUsageAnyOid[] = {85, 29, 37, 0};
static const byte extExtKeyUsageServerAuthOid[]   = {43, 6, 1, 5, 5, 7, 3, 1};
static const byte extExtKeyUsageClientAuthOid[]   = {43, 6, 1, 5, 5, 7, 3, 2};
static const byte extExtKeyUsageCodeSigningOid[]  = {43, 6, 1, 5, 5, 7, 3, 3};
static const byte extExtKeyUsageEmailProtectOid[] = {43, 6, 1, 5, 5, 7, 3, 4};
static const byte extExtKeyUsageTimestampOid[]    = {43, 6, 1, 5, 5, 7, 3, 8};
static const byte extExtKeyUsageOcspSignOid[]     = {43, 6, 1, 5, 5, 7, 3, 9};

/* kdfType */
static const byte pbkdf2Oid[] = {42, 134, 72, 134, 247, 13, 1, 5, 12};

static const byte* OidFromId(word32 id, word32 type, word32* oidSz)
{
    const byte* oid = NULL;

    *oidSz = 0;

    switch (type) {

        case oidHashType:
            switch (id) {
                case MD2h:
                    oid = hashMd2hOid;
                    *oidSz = sizeof(hashMd2hOid);
                    break;
                case MD5h:
                    oid = hashMd5hOid;
                    *oidSz = sizeof(hashMd5hOid);
                    break;
                case SHAh:
                    oid = hashSha1hOid;
                    *oidSz = sizeof(hashSha1hOid);
                    break;
                case SHA224h:
                    oid = hashSha224hOid;
                    *oidSz = sizeof(hashSha224hOid);
                    break;
                case SHA256h:
                    oid = hashSha256hOid;
                    *oidSz = sizeof(hashSha256hOid);
                    break;
                case SHA384h:
                    oid = hashSha384hOid;
                    *oidSz = sizeof(hashSha384hOid);
                    break;
                case SHA512h:
                    oid = hashSha512hOid;
                    *oidSz = sizeof(hashSha512hOid);
                    break;
            }
            break;

        case oidSigType:
            switch (id) {
                #ifndef NO_DSA
                case CTC_SHAwDSA:
                    oid = sigSha1wDsaOid;
                    *oidSz = sizeof(sigSha1wDsaOid);
                    break;
                #endif /* NO_DSA */
                #ifndef NO_RSA
                case CTC_MD2wRSA:
                    oid = sigMd2wRsaOid;
                    *oidSz = sizeof(sigMd2wRsaOid);
                    break;
                case CTC_MD5wRSA:
                    oid = sigMd5wRsaOid;
                    *oidSz = sizeof(sigMd5wRsaOid);
                    break;
                case CTC_SHAwRSA:
                    oid = sigSha1wRsaOid;
                    *oidSz = sizeof(sigSha1wRsaOid);
                    break;
                case CTC_SHA224wRSA:
                    oid = sigSha224wRsaOid;
                    *oidSz = sizeof(sigSha224wRsaOid);
                    break;
                case CTC_SHA256wRSA:
                    oid = sigSha256wRsaOid;
                    *oidSz = sizeof(sigSha256wRsaOid);
                    break;
                case CTC_SHA384wRSA:
                    oid = sigSha384wRsaOid;
                    *oidSz = sizeof(sigSha384wRsaOid);
                    break;
                case CTC_SHA512wRSA:
                    oid = sigSha512wRsaOid;
                    *oidSz = sizeof(sigSha512wRsaOid);
                    break;
                #endif /* NO_RSA */
                #ifdef HAVE_ECC
                case CTC_SHAwECDSA:
                    oid = sigSha1wEcdsaOid;
                    *oidSz = sizeof(sigSha1wEcdsaOid);
                    break;
                case CTC_SHA224wECDSA:
                    oid = sigSha224wEcdsaOid;
                    *oidSz = sizeof(sigSha224wEcdsaOid);
                    break;
                case CTC_SHA256wECDSA:
                    oid = sigSha256wEcdsaOid;
                    *oidSz = sizeof(sigSha256wEcdsaOid);
                    break;
                case CTC_SHA384wECDSA:
                    oid = sigSha384wEcdsaOid;
                    *oidSz = sizeof(sigSha384wEcdsaOid);
                    break;
                case CTC_SHA512wECDSA:
                    oid = sigSha512wEcdsaOid;
                    *oidSz = sizeof(sigSha512wEcdsaOid);
                    break;
                #endif /* HAVE_ECC */
                #ifdef HAVE_ED25519
                case CTC_ED25519:
                    oid = sigEd25519Oid;
                    *oidSz = sizeof(sigEd25519Oid);
                    break;
                #endif
                default:
                    break;
            }
            break;

        case oidKeyType:
            switch (id) {
                #ifndef NO_DSA
                case DSAk:
                    oid = keyDsaOid;
                    *oidSz = sizeof(keyDsaOid);
                    break;
                #endif /* NO_DSA */
                #ifndef NO_RSA
                case RSAk:
                    oid = keyRsaOid;
                    *oidSz = sizeof(keyRsaOid);
                    break;
                #endif /* NO_RSA */
                #ifdef HAVE_NTRU
                case NTRUk:
                    oid = keyNtruOid;
                    *oidSz = sizeof(keyNtruOid);
                    break;
                #endif /* HAVE_NTRU */
                #ifdef HAVE_ECC
                case ECDSAk:
                    oid = keyEcdsaOid;
                    *oidSz = sizeof(keyEcdsaOid);
                    break;
                #endif /* HAVE_ECC */
                #ifdef HAVE_ED25519
                case ED25519k:
                    oid = keyEd25519Oid;
                    *oidSz = sizeof(keyEd25519Oid);
                    break;
                #endif /* HAVE_ED25519 */
                default:
                    break;
            }
            break;

        #ifdef HAVE_ECC
        case oidCurveType:
            if (wc_ecc_get_oid(id, &oid, oidSz) < 0) {
                WOLFSSL_MSG("ECC OID not found");
            }
            break;
        #endif /* HAVE_ECC */

        case oidBlkType:
            switch (id) {
                case AES128CBCb:
                    oid = blkAes128CbcOid;
                    *oidSz = sizeof(blkAes128CbcOid);
                    break;
                case AES192CBCb:
                    oid = blkAes192CbcOid;
                    *oidSz = sizeof(blkAes192CbcOid);
                    break;
                case AES256CBCb:
                    oid = blkAes256CbcOid;
                    *oidSz = sizeof(blkAes256CbcOid);
                    break;
                case DESb:
                    oid = blkDesCbcOid;
                    *oidSz = sizeof(blkDesCbcOid);
                    break;
                case DES3b:
                    oid = blkDes3CbcOid;
                    *oidSz = sizeof(blkDes3CbcOid);
                    break;
            }
            break;

        #ifdef HAVE_OCSP
        case oidOcspType:
            switch (id) {
                case OCSP_BASIC_OID:
                    oid = ocspBasicOid;
                    *oidSz = sizeof(ocspBasicOid);
                    break;
                case OCSP_NONCE_OID:
                    oid = ocspNonceOid;
                    *oidSz = sizeof(ocspNonceOid);
                    break;
            }
            break;
        #endif /* HAVE_OCSP */

        case oidCertExtType:
            switch (id) {
                case BASIC_CA_OID:
                    oid = extBasicCaOid;
                    *oidSz = sizeof(extBasicCaOid);
                    break;
                case ALT_NAMES_OID:
                    oid = extAltNamesOid;
                    *oidSz = sizeof(extAltNamesOid);
                    break;
                case CRL_DIST_OID:
                    oid = extCrlDistOid;
                    *oidSz = sizeof(extCrlDistOid);
                    break;
                case AUTH_INFO_OID:
                    oid = extAuthInfoOid;
                    *oidSz = sizeof(extAuthInfoOid);
                    break;
                case AUTH_KEY_OID:
                    oid = extAuthKeyOid;
                    *oidSz = sizeof(extAuthKeyOid);
                    break;
                case SUBJ_KEY_OID:
                    oid = extSubjKeyOid;
                    *oidSz = sizeof(extSubjKeyOid);
                    break;
                case CERT_POLICY_OID:
                    oid = extCertPolicyOid;
                    *oidSz = sizeof(extCertPolicyOid);
                    break;
                case KEY_USAGE_OID:
                    oid = extKeyUsageOid;
                    *oidSz = sizeof(extKeyUsageOid);
                    break;
                case INHIBIT_ANY_OID:
                    oid = extInhibitAnyOid;
                    *oidSz = sizeof(extInhibitAnyOid);
                    break;
                case EXT_KEY_USAGE_OID:
                    oid = extExtKeyUsageOid;
                    *oidSz = sizeof(extExtKeyUsageOid);
                    break;
                case NAME_CONS_OID:
                    oid = extNameConsOid;
                    *oidSz = sizeof(extNameConsOid);
                    break;
            }
            break;

        case oidCertAuthInfoType:
            switch (id) {
                case AIA_OCSP_OID:
                    oid = extAuthInfoOcspOid;
                    *oidSz = sizeof(extAuthInfoOcspOid);
                    break;
                case AIA_CA_ISSUER_OID:
                    oid = extAuthInfoCaIssuerOid;
                    *oidSz = sizeof(extAuthInfoCaIssuerOid);
                    break;
            }
            break;

        case oidCertPolicyType:
            switch (id) {
                case CP_ANY_OID:
                    oid = extCertPolicyAnyOid;
                    *oidSz = sizeof(extCertPolicyAnyOid);
                    break;
            }
            break;

        case oidCertAltNameType:
            switch (id) {
                case HW_NAME_OID:
                    oid = extAltNamesHwNameOid;
                    *oidSz = sizeof(extAltNamesHwNameOid);
                    break;
            }
            break;

        case oidCertKeyUseType:
            switch (id) {
                case EKU_ANY_OID:
                    oid = extExtKeyUsageAnyOid;
                    *oidSz = sizeof(extExtKeyUsageAnyOid);
                    break;
                case EKU_SERVER_AUTH_OID:
                    oid = extExtKeyUsageServerAuthOid;
                    *oidSz = sizeof(extExtKeyUsageServerAuthOid);
                    break;
                case EKU_CLIENT_AUTH_OID:
                    oid = extExtKeyUsageClientAuthOid;
                    *oidSz = sizeof(extExtKeyUsageClientAuthOid);
                    break;
                case EKU_CODESIGNING_OID:
                    oid = extExtKeyUsageCodeSigningOid;
                    *oidSz = sizeof(extExtKeyUsageCodeSigningOid);
                    break;
                case EKU_EMAILPROTECT_OID:
                    oid = extExtKeyUsageEmailProtectOid;
                    *oidSz = sizeof(extExtKeyUsageEmailProtectOid);
                    break;
                case EKU_TIMESTAMP_OID:
                    oid = extExtKeyUsageTimestampOid;
                    *oidSz = sizeof(extExtKeyUsageTimestampOid);
                    break;
                case EKU_OCSP_SIGN_OID:
                    oid = extExtKeyUsageOcspSignOid;
                    *oidSz = sizeof(extExtKeyUsageOcspSignOid);
                    break;
            }
            break;

        case oidKdfType:
            switch (id) {
                case PBKDF2_OID:
                    oid = pbkdf2Oid;
                    *oidSz = sizeof(pbkdf2Oid);
                    break;
            }
            break;

        case oidKeyWrapType:
            switch (id) {
                case AES128_WRAP:
                    oid = wrapAes128Oid;
                    *oidSz = sizeof(wrapAes128Oid);
                    break;
                case AES192_WRAP:
                    oid = wrapAes192Oid;
                    *oidSz = sizeof(wrapAes192Oid);
                    break;
                case AES256_WRAP:
                    oid = wrapAes256Oid;
                    *oidSz = sizeof(wrapAes256Oid);
                    break;
            }
            break;

        case oidCmsKeyAgreeType:
            switch (id) {
                case dhSinglePass_stdDH_sha1kdf_scheme:
                    oid = dhSinglePass_stdDH_sha1kdf_Oid;
                    *oidSz = sizeof(dhSinglePass_stdDH_sha1kdf_Oid);
                    break;
                case dhSinglePass_stdDH_sha224kdf_scheme:
                    oid = dhSinglePass_stdDH_sha224kdf_Oid;
                    *oidSz = sizeof(dhSinglePass_stdDH_sha224kdf_Oid);
                    break;
                case dhSinglePass_stdDH_sha256kdf_scheme:
                    oid = dhSinglePass_stdDH_sha256kdf_Oid;
                    *oidSz = sizeof(dhSinglePass_stdDH_sha256kdf_Oid);
                    break;
                case dhSinglePass_stdDH_sha384kdf_scheme:
                    oid = dhSinglePass_stdDH_sha384kdf_Oid;
                    *oidSz = sizeof(dhSinglePass_stdDH_sha384kdf_Oid);
                    break;
                case dhSinglePass_stdDH_sha512kdf_scheme:
                    oid = dhSinglePass_stdDH_sha512kdf_Oid;
                    *oidSz = sizeof(dhSinglePass_stdDH_sha512kdf_Oid);
                    break;
            }
            break;

        case oidIgnoreType:
        default:
            break;
    }

    return oid;
}

#ifdef HAVE_OID_ENCODING
int EncodeObjectId(const word16* in, word32 inSz, byte* out, word32* outSz)
{
    int i, x, len;
    word32 d, t;

    /* check args */
    if (in == NULL || outSz == NULL) {
        return BAD_FUNC_ARG;
    }

    /* compute length of encoded OID */
    d = (in[0] * 40) + in[1];
    len = 0;
    for (i = 1; i < (int)inSz; i++) {
        x = 0;
        t = d;
        while (t) {
            x++;
            t >>= 1;
        }
        len += (x / 7) + ((x % 7) ? 1 : 0) + (d == 0 ? 1 : 0);

        if (i < (int)inSz - 1) {
            d = in[i + 1];
        }
    }

    if (out) {
        /* verify length */
        if ((int)*outSz < len) {
            return BUFFER_E; /* buffer provided is not large enough */
        }

        /* calc first byte */
        d = (in[0] * 40) + in[1];

        /* encode bytes */
        x = 0;
        for (i = 1; i < (int)inSz; i++) {
            if (d) {
                int y = x, z;
                byte mask = 0;
                while (d) {
                    out[x++] = (byte)((d & 0x7F) | mask);
                    d     >>= 7;
                    mask  |= 0x80;  /* upper bit is set on all but the last byte */
                }
                /* now swap bytes y...x-1 */
                z = x - 1;
                while (y < z) {
                    mask = out[y];
                    out[y] = out[z];
                    out[z] = mask;
                    ++y;
                    --z;
                }
            }
            else {
              out[x++] = 0x00; /* zero value */
            }

            /* next word */
            if (i < (int)inSz - 1) {
                d = in[i + 1];
            }
        }
    }

    /* return length */
    *outSz = len;

    return 0;
}
#endif /* HAVE_OID_ENCODING */

#ifdef HAVE_OID_DECODING
int DecodeObjectId(const byte* in, word32 inSz, word16* out, word32* outSz)
{
    int x = 0, y = 0;
    word32 t = 0;

    /* check args */
    if (in == NULL || outSz == NULL) {
        return BAD_FUNC_ARG;
    }

    /* decode bytes */
    while (inSz--) {
        t = (t << 7) | (in[x] & 0x7F);
        if (!(in[x] & 0x80)) {
            if (y >= (int)*outSz) {
                return BUFFER_E;
            }
            if (y == 0) {
                out[0] = (t / 40);
                out[1] = (t % 40);
                y = 2;
            }
            else {
                out[y++] = t;
            }
            t = 0; /* reset tmp */
        }
        x++;
    }

    /* return length */
    *outSz = y;

    return 0;
}
#endif /* HAVE_OID_DECODING */

/* Get the DER/BER encoding of an ASN.1 OBJECT_ID header.
 *
 * input     Buffer holding DER/BER encoded data.
 * inOutIdx  Current index into buffer to parse.
 * len       The number of bytes in the ASN.1 data.
 * maxIdx    Length of data in buffer.
 * returns BUFFER_E when there is not enough data to parse.
 *         ASN_OBJECt_ID_E when the OBJECT_ID tag is not found.
 *         ASN_PARSE_E when length is invalid.
 *         Otherwise, 0 to indicate success.
 */
static int GetASNObjectId(const byte* input, word32* inOutIdx, int* len,
                          word32 maxIdx)
{
    word32 idx = *inOutIdx;
    byte   b;
    int    length;

    if ((idx + 1) > maxIdx)
        return BUFFER_E;

    b = input[idx++];
    if (b != ASN_OBJECT_ID)
        return ASN_OBJECT_ID_E;

    if (GetLength(input, &idx, &length, maxIdx) < 0)
        return ASN_PARSE_E;

    *len = length;
    *inOutIdx = idx;
    return 0;
}

/* Set the DER/BER encoding of the ASN.1 OBJECT_ID header.
 *
 * len         Length of the OBJECT_ID data.
 * output      Buffer to write into.
 * returns the number of bytes added to the buffer.
 */
static int SetObjectId(int len, byte* output)
{
    int idx = 0;

    output[idx++] = ASN_OBJECT_ID;
    idx += SetLength(len, output + idx);

    return idx;
}

int GetObjectId(const byte* input, word32* inOutIdx, word32* oid,
                                  word32 oidType, word32 maxIdx)
{
    int    ret = 0, length;
    word32 idx = *inOutIdx;
#ifndef NO_VERIFY_OID
    word32 actualOidSz = 0;
    const byte* actualOid;
#endif /* NO_VERIFY_OID */

    (void)oidType;
    WOLFSSL_ENTER("GetObjectId()");
    *oid = 0;

    ret = GetASNObjectId(input, &idx, &length, maxIdx);
    if (ret != 0)
        return ret;

#ifndef NO_VERIFY_OID
    actualOid = &input[idx];
    if (length > 0)
        actualOidSz = (word32)length;
#endif /* NO_VERIFY_OID */

    while (length--) {
        /* odd HC08 compiler behavior here when input[idx++] */
        *oid += (word32)input[idx];
        idx++;
    }
    /* just sum it up for now */

    *inOutIdx = idx;

#ifndef NO_VERIFY_OID
    {
        const byte* checkOid = NULL;
        word32 checkOidSz;
    #ifdef ASN_DUMP_OID
        int i;
    #endif

        if (oidType != oidIgnoreType) {
            checkOid = OidFromId(*oid, oidType, &checkOidSz);

        #ifdef ASN_DUMP_OID
            /* support for dumping OID information */
            printf("OID (Type %d, Sz %d, Sum %d): ", oidType, actualOidSz, *oid);
            for (i=0; i<actualOidSz; i++) {
                printf("%d, ", actualOid[i]);
            }
            printf("\n");
            #ifdef HAVE_OID_DECODING
            {
                word16 decOid[16];
                word32 decOidSz = sizeof(decOid);
                ret = DecodeObjectId(actualOid, actualOidSz, decOid, &decOidSz);
                if (ret == 0) {
                    printf("  Decoded (Sz %d): ", decOidSz);
                    for (i=0; i<decOidSz; i++) {
                        printf("%d.", decOid[i]);
                    }
                    printf("\n");
                }
                else {
                    printf("DecodeObjectId failed: %d\n", ret);
                }
            }
            #endif /* HAVE_OID_DECODING */
        #endif /* ASN_DUMP_OID */

            if (checkOid != NULL &&
                (checkOidSz != actualOidSz ||
                    XMEMCMP(actualOid, checkOid, checkOidSz) != 0)) {
                WOLFSSL_MSG("OID Check Failed");
                return ASN_UNKNOWN_OID_E;
            }
        }
    }
#endif /* NO_VERIFY_OID */

    return ret;
}

static int SkipObjectId(const byte* input, word32* inOutIdx, word32 maxIdx)
{
    word32 idx = *inOutIdx;
    int    length;
    int ret;

    ret = GetASNObjectId(input, &idx, &length, maxIdx);
    if (ret != 0)
        return ret;

    idx += length;
    *inOutIdx = idx;

    return 0;
}

WOLFSSL_LOCAL int GetAlgoId(const byte* input, word32* inOutIdx, word32* oid,
                     word32 oidType, word32 maxIdx)
{
    int    length;
    word32 idx = *inOutIdx;
    int    ret;
    *oid = 0;

    WOLFSSL_ENTER("GetAlgoId");

    if (GetSequence(input, &idx, &length, maxIdx) < 0)
        return ASN_PARSE_E;

    if (GetObjectId(input, &idx, oid, oidType, maxIdx) < 0)
        return ASN_OBJECT_ID_E;

    /* could have NULL tag and 0 terminator, but may not */
    if (input[idx] == ASN_TAG_NULL) {
        ret = GetASNNull(input, &idx, maxIdx);
        if (ret != 0)
            return ret;
    }

    *inOutIdx = idx;

    return 0;
}

#ifndef NO_RSA

#ifndef HAVE_USER_RSA
int wc_RsaPrivateKeyDecode(const byte* input, word32* inOutIdx, RsaKey* key,
                        word32 inSz)
{
    int version, length;

    if (inOutIdx == NULL) {
        return BAD_FUNC_ARG;
    }
    if (GetSequence(input, inOutIdx, &length, inSz) < 0)
        return ASN_PARSE_E;

    if (GetMyVersion(input, inOutIdx, &version, inSz) < 0)
        return ASN_PARSE_E;

    key->type = RSA_PRIVATE;

    if (GetInt(&key->n,  input, inOutIdx, inSz) < 0 ||
        GetInt(&key->e,  input, inOutIdx, inSz) < 0 ||
        GetInt(&key->d,  input, inOutIdx, inSz) < 0 ||
        GetInt(&key->p,  input, inOutIdx, inSz) < 0 ||
        GetInt(&key->q,  input, inOutIdx, inSz) < 0 ||
        GetInt(&key->dP, input, inOutIdx, inSz) < 0 ||
        GetInt(&key->dQ, input, inOutIdx, inSz) < 0 ||
        GetInt(&key->u,  input, inOutIdx, inSz) < 0 )  return ASN_RSA_KEY_E;

#ifdef WOLFSSL_XILINX_CRYPT
    if (wc_InitRsaHw(key) != 0) {
        return BAD_STATE_E;
    }
#endif

    return 0;
}
#endif /* HAVE_USER_RSA */
#endif /* NO_RSA */

/* Remove PKCS8 header, place inOutIdx at beginning of traditional,
 * return traditional length on success, negative on error */
int ToTraditionalInline(const byte* input, word32* inOutIdx, word32 sz)
{
    word32 idx, oid;
    int    version, length;
    int    ret;

    if (input == NULL || inOutIdx == NULL)
        return BAD_FUNC_ARG;

    idx = *inOutIdx;

    if (GetSequence(input, &idx, &length, sz) < 0)
        return ASN_PARSE_E;

    if (GetMyVersion(input, &idx, &version, sz) < 0)
        return ASN_PARSE_E;

    if (GetAlgoId(input, &idx, &oid, oidKeyType, sz) < 0)
        return ASN_PARSE_E;

    if (input[idx] == ASN_OBJECT_ID) {
        if (SkipObjectId(input, &idx, sz) < 0)
            return ASN_PARSE_E;
    }

    ret = GetOctetString(input, &idx, &length, sz);
    if (ret < 0)
        return ret;

    *inOutIdx = idx;

    return length;
}

/* Remove PKCS8 header, move beginning of traditional to beginning of input */
int ToTraditional(byte* input, word32 sz)
{
    word32 inOutIdx = 0;
    int    length;

    if (input == NULL)
        return BAD_FUNC_ARG;

    length = ToTraditionalInline(input, &inOutIdx, sz);
    if (length < 0)
        return length;

    XMEMMOVE(input, input + inOutIdx, length);

    return length;
}


/* find beginning of traditional key inside PKCS#8 unencrypted buffer
 * return traditional length on success, with inOutIdx at beginning of
 * traditional
 * return negative on failure/error */
int wc_GetPkcs8TraditionalOffset(byte* input, word32* inOutIdx, word32 sz)
{
    int length;

    if (input == NULL || inOutIdx == NULL || (*inOutIdx > sz))
        return BAD_FUNC_ARG;

    length = ToTraditionalInline(input, inOutIdx, sz);

    return length;
}


/* PKCS#8 from RFC 5208
 * This function takes in a DER key and converts it to PKCS#8 format. Used
 * in creating PKCS#12 shrouded key bags.
 * Reverse of ToTraditional
 *
 * PrivateKeyInfo ::= SEQUENCE {
 *  version Version,
 *  privateKeyAlgorithm PrivateKeyAlgorithmIdentifier,
 *  privateKey          PrivateKey,
 *  attributes          optional
 *  }
 *  Version ::= INTEGER
 *  PrivateKeyAlgorithmIdentifier ::= AlgorithmIdentifier
 *  PrivateKey ::= OCTET STRING
 *
 * out      buffer to place result in
 * outSz    size of out buffer
 * key      buffer with DER key
 * keySz    size of key buffer
 * algoID   algorithm ID i.e. RSAk
 * curveOID ECC curve oid if used. Should be NULL for RSA keys.
 * oidSz    size of curve oid. Is set to 0 if curveOID is NULL.
 *
 * Returns the size of PKCS#8 placed into out. In error cases returns negative
 * values.
 */
int wc_CreatePKCS8Key(byte* out, word32* outSz, byte* key, word32 keySz,
        int algoID, const byte* curveOID, word32 oidSz)
{
        word32 keyIdx = 0;
        word32 tmpSz  = 0;
        word32 sz;


        /* If out is NULL then return the max size needed
         * + 2 for ASN_OBJECT_ID and ASN_OCTET_STRING tags */
        if (out == NULL && outSz != NULL) {
            *outSz = keySz + MAX_SEQ_SZ + MAX_VERSION_SZ + MAX_ALGO_SZ
                     + MAX_LENGTH_SZ + MAX_LENGTH_SZ + 2;

            if (curveOID != NULL)
                *outSz += oidSz + MAX_LENGTH_SZ + 1;

            WOLFSSL_MSG("Checking size of PKCS8");

            return LENGTH_ONLY_E;
        }

        WOLFSSL_ENTER("wc_CreatePKCS8Key()");

        if (key == NULL || out == NULL || outSz == NULL) {
            return BAD_FUNC_ARG;
        }

        /* check the buffer has enough room for largest possible size */
        if (curveOID != NULL) {
            if (*outSz < (keySz + MAX_SEQ_SZ + MAX_VERSION_SZ + MAX_ALGO_SZ
                   + MAX_LENGTH_SZ + MAX_LENGTH_SZ + 3 + oidSz + MAX_LENGTH_SZ))
                return BUFFER_E;
        }
        else {
            oidSz = 0; /* with no curveOID oid size must be 0 */
            if (*outSz < (keySz + MAX_SEQ_SZ + MAX_VERSION_SZ + MAX_ALGO_SZ
                      + MAX_LENGTH_SZ + MAX_LENGTH_SZ + 2))
                return BUFFER_E;
        }

        /* PrivateKeyInfo ::= SEQUENCE */
        keyIdx += MAX_SEQ_SZ; /* save room for sequence */

        /*  version Version
         *  no header information just INTEGER */
        sz = SetMyVersion(PKCS8v0, out + keyIdx, 0);
        tmpSz += sz; keyIdx += sz;

        /*  privateKeyAlgorithm PrivateKeyAlgorithmIdentifier */
        sz = 0; /* set sz to 0 and get privateKey oid buffer size needed */
        if (curveOID != NULL && oidSz > 0) {
            byte buf[MAX_LENGTH_SZ];
            sz = SetLength(oidSz, buf);
            sz += 1; /* plus one for ASN object id */
        }
        sz = SetAlgoID(algoID, out + keyIdx, oidKeyType, oidSz + sz);
        tmpSz += sz; keyIdx += sz;

        /*  privateKey          PrivateKey *
         * pkcs8 ecc uses slightly different format. Places curve oid in
         * buffer */
        if (curveOID != NULL && oidSz > 0) {
            sz = SetObjectId(oidSz, out + keyIdx);
            keyIdx += sz; tmpSz += sz;
            XMEMCPY(out + keyIdx, curveOID, oidSz);
            keyIdx += oidSz; tmpSz += oidSz;
        }

        sz = SetOctetString(keySz, out + keyIdx);
        keyIdx += sz; tmpSz += sz;
        XMEMCPY(out + keyIdx, key, keySz);
        tmpSz += keySz;

        /*  attributes          optional
         * No attributes currently added */

        /* rewind and add sequence */
        sz = SetSequence(tmpSz, out);
        XMEMMOVE(out + sz, out + MAX_SEQ_SZ, tmpSz);

        return tmpSz + sz;
}


/* check that the private key is a pair for the public key in certificate
 * return 1 (true) on match
 * return 0 or negative value on failure/error
 *
 * key   : buffer holding DER fromat key
 * keySz : size of key buffer
 * der   : a initialized and parsed DecodedCert holding a certificate */
int wc_CheckPrivateKey(byte* key, word32 keySz, DecodedCert* der)
{
    int ret;

    if (key == NULL || der == NULL) {
        return BAD_FUNC_ARG;
    }

    #if !defined(NO_RSA)
    /* test if RSA key */
    if (der->keyOID == RSAk) {
        RsaKey a, b;
        word32 keyIdx = 0;

        if ((ret = wc_InitRsaKey(&a, NULL)) < 0)
            return ret;
        if ((ret = wc_InitRsaKey(&b, NULL)) < 0) {
            wc_FreeRsaKey(&a);
            return ret;
        }
        if ((ret = wc_RsaPrivateKeyDecode(key, &keyIdx, &a, keySz)) == 0) {
            WOLFSSL_MSG("Checking RSA key pair");
            keyIdx = 0; /* reset to 0 for parsing public key */

            if ((ret = wc_RsaPublicKeyDecode(der->publicKey, &keyIdx, &b,
                                                       der->pubKeySize)) == 0) {
                /* limit for user RSA crypto because of RsaKey
                 * dereference. */
            #if defined(HAVE_USER_RSA)
                WOLFSSL_MSG("Cannot verify RSA pair with user RSA");
                ret = 1; /* return first RSA cert as match */
            #else
                /* both keys extracted successfully now check n and e
                 * values are the same. This is dereferencing RsaKey */
                if (mp_cmp(&(a.n), &(b.n)) != MP_EQ ||
                    mp_cmp(&(a.e), &(b.e)) != MP_EQ) {
                    ret = MP_CMP_E;
                }
                else
                    ret = 1;
            #endif
            }
        }
        wc_FreeRsaKey(&b);
        wc_FreeRsaKey(&a);
    }
    else
    #endif /* NO_RSA */

    #ifdef HAVE_ECC
    if (der->keyOID == ECDSAk) {
        word32  keyIdx = 0;
        ecc_key key_pair;

        if ((ret = wc_ecc_init(&key_pair)) < 0)
            return ret;
        if ((ret = wc_EccPrivateKeyDecode(key, &keyIdx, &key_pair,
                                                                 keySz)) == 0) {
            WOLFSSL_MSG("Checking ECC key pair");
            keyIdx = 0;
            if ((ret = wc_ecc_import_x963(der->publicKey, der->pubKeySize,
                                                             &key_pair)) == 0) {
                /* public and private extracted successfuly no check if is
                 * a pair and also do sanity checks on key. wc_ecc_check_key
                 * checks that private * base generator equals pubkey */
                if ((ret = wc_ecc_check_key(&key_pair)) == 0)
                    ret = 1;
            }
        }
        wc_ecc_free(&key_pair);
    }
    else
    #endif /* HAVE_ECC */

    #ifdef HAVE_ED25519
    if (der->keyOID == ED25519k) {
        word32  keyIdx = 0;
        ed25519_key key_pair;

        if ((ret = wc_ed25519_init(&key_pair)) < 0)
            return ret;
        if ((ret = wc_Ed25519PrivateKeyDecode(key, &keyIdx, &key_pair,
                                                                 keySz)) == 0) {
            WOLFSSL_MSG("Checking ED25519 key pair");
            keyIdx = 0;
            if ((ret = wc_ed25519_import_public(der->publicKey, der->pubKeySize,
                                                             &key_pair)) == 0) {
                /* public and private extracted successfuly no check if is
                 * a pair and also do sanity checks on key. wc_ecc_check_key
                 * checks that private * base generator equals pubkey */
                if ((ret = wc_ed25519_check_key(&key_pair)) == 0)
                    ret = 1;
            }
        }
        wc_ed25519_free(&key_pair);
    }
    else
    #endif
    {
        ret = 0;
    }

    return ret;
}

#ifndef NO_PWDBASED

/* Check To see if PKCS version algo is supported, set id if it is return 0
   < 0 on error */
static int CheckAlgo(int first, int second, int* id, int* version)
{
    *id      = ALGO_ID_E;
    *version = PKCS5;   /* default */

    if (first == 1) {
        switch (second) {
        case 1:
            *id = PBE_SHA1_RC4_128;
            *version = PKCS12v1;
            return 0;
        case 3:
            *id = PBE_SHA1_DES3;
            *version = PKCS12v1;
            return 0;
        default:
            return ALGO_ID_E;
        }
    }

    if (first != PKCS5)
        return ASN_INPUT_E;  /* VERSION ERROR */

    if (second == PBES2) {
        *version = PKCS5v2;
        return 0;
    }

    switch (second) {
    case 3:                   /* see RFC 2898 for ids */
        *id = PBE_MD5_DES;
        return 0;
    case 10:
        *id = PBE_SHA1_DES;
        return 0;
    default:
        return ALGO_ID_E;

    }
}


/* Check To see if PKCS v2 algo is supported, set id if it is return 0
   < 0 on error */
static int CheckAlgoV2(int oid, int* id)
{
    switch (oid) {
    case 69:
        *id = PBE_SHA1_DES;
        return 0;
    case 652:
        *id = PBE_SHA1_DES3;
        return 0;
    default:
        return ALGO_ID_E;

    }
}


/* Decrypt input in place from parameters based on id */
static int DecryptKey(const char* password, int passwordSz, byte* salt,
                      int saltSz, int iterations, int id, byte* input,
                      int length, int version, byte* cbcIv)
{
    int typeH;
    int derivedLen;
    int decryptionType;
    int ret = 0;
#ifdef WOLFSSL_SMALL_STACK
    byte* key;
#else
    byte key[MAX_KEY_SIZE];
#endif

    (void)input;
    (void)length;

    switch (id) {
        case PBE_MD5_DES:
            typeH = MD5;
            derivedLen = 16;           /* may need iv for v1.5 */
            decryptionType = DES_TYPE;
            break;

        case PBE_SHA1_DES:
            typeH = SHA;
            derivedLen = 16;           /* may need iv for v1.5 */
            decryptionType = DES_TYPE;
            break;

        case PBE_SHA1_DES3:
            typeH = SHA;
            derivedLen = 32;           /* may need iv for v1.5 */
            decryptionType = DES3_TYPE;
            break;

        case PBE_SHA1_RC4_128:
            typeH = SHA;
            derivedLen = 16;
            decryptionType = RC4_TYPE;
            break;

        default:
            return ALGO_ID_E;
    }

#ifdef WOLFSSL_SMALL_STACK
    key = (byte*)XMALLOC(MAX_KEY_SIZE, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (key == NULL)
        return MEMORY_E;
#endif

    if (version == PKCS5v2)
        ret = wc_PBKDF2(key, (byte*)password, passwordSz,
                        salt, saltSz, iterations, derivedLen, typeH);
#ifndef NO_SHA
    else if (version == PKCS5)
        ret = wc_PBKDF1(key, (byte*)password, passwordSz,
                        salt, saltSz, iterations, derivedLen, typeH);
#endif
    else if (version == PKCS12v1) {
        int  i, idx = 0;
        byte unicodePasswd[MAX_UNICODE_SZ];

        if ( (passwordSz * 2 + 2) > (int)sizeof(unicodePasswd)) {
#ifdef WOLFSSL_SMALL_STACK
            XFREE(key, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
            return UNICODE_SIZE_E;
        }

        for (i = 0; i < passwordSz; i++) {
            unicodePasswd[idx++] = 0x00;
            unicodePasswd[idx++] = (byte)password[i];
        }
        /* add trailing NULL */
        unicodePasswd[idx++] = 0x00;
        unicodePasswd[idx++] = 0x00;

        ret =  wc_PKCS12_PBKDF(key, unicodePasswd, idx, salt, saltSz,
                            iterations, derivedLen, typeH, 1);
        if (decryptionType != RC4_TYPE)
            ret += wc_PKCS12_PBKDF(cbcIv, unicodePasswd, idx, salt, saltSz,
                                iterations, 8, typeH, 2);
    }
    else {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(key, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return ALGO_ID_E;
    }

    if (ret != 0) {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(key, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return ret;
    }

    switch (decryptionType) {
#ifndef NO_DES3
        case DES_TYPE:
        {
            Des    dec;
            byte*  desIv = key + 8;

            if (version == PKCS5v2 || version == PKCS12v1)
                desIv = cbcIv;

            ret = wc_Des_SetKey(&dec, key, desIv, DES_DECRYPTION);
            if (ret != 0) {
#ifdef WOLFSSL_SMALL_STACK
                XFREE(key, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
                return ret;
            }

            wc_Des_CbcDecrypt(&dec, input, input, length);
            break;
        }

        case DES3_TYPE:
        {
            Des3   dec;
            byte*  desIv = key + 24;

            if (version == PKCS5v2 || version == PKCS12v1)
                desIv = cbcIv;

            ret = wc_Des3Init(&dec, NULL, INVALID_DEVID);
            if (ret != 0) {
#ifdef WOLFSSL_SMALL_STACK
                XFREE(key, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
                return ret;
            }
            ret = wc_Des3_SetKey(&dec, key, desIv, DES_DECRYPTION);
            if (ret != 0) {
#ifdef WOLFSSL_SMALL_STACK
                XFREE(key, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
                return ret;
            }
            ret = wc_Des3_CbcDecrypt(&dec, input, input, length);
            if (ret != 0) {
#ifdef WOLFSSL_SMALL_STACK
                XFREE(key, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
                return ret;
            }
            break;
        }
#endif
#ifndef NO_RC4
        case RC4_TYPE:
        {
            Arc4    dec;

            wc_Arc4SetKey(&dec, key, derivedLen);
            wc_Arc4Process(&dec, input, input, length);
            break;
        }
#endif

        default:
#ifdef WOLFSSL_SMALL_STACK
            XFREE(key, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
            return ALGO_ID_E;
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(key, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return 0;
}


int wc_GetKeyOID(byte* key, word32 keySz, const byte** curveOID, word32* oidSz,
        int* algoID, void* heap)
{
    word32 tmpIdx = 0;
#ifdef HAVE_ECC
    ecc_key ecc;
#endif
#ifndef NO_RSA
    RsaKey rsa;
#endif
#ifdef HAVE_ED25519
    ed25519_key ed25519;
#endif

    if (algoID == NULL) {
        return BAD_FUNC_ARG;
    }
    *algoID = 0;

#ifndef NO_RSA
    if (wc_InitRsaKey(&rsa, heap) == 0) {
        if (wc_RsaPrivateKeyDecode(key, &tmpIdx, &rsa, keySz) == 0) {
            *algoID = RSAk;
        }
        else {
            WOLFSSL_MSG("Not RSA DER key");
        }
        wc_FreeRsaKey(&rsa);
    }
    else {
        WOLFSSL_MSG("GetKeyOID wc_InitRsaKey failed");
    }
#endif /* NO_RSA */
#ifdef HAVE_ECC
    if (*algoID != RSAk) {
        tmpIdx = 0;
        if (wc_ecc_init_ex(&ecc, heap, INVALID_DEVID) == 0) {
            if (wc_EccPrivateKeyDecode(key, &tmpIdx, &ecc, keySz) == 0) {
                *algoID = ECDSAk;

                /* sanity check on arguments */
                if (curveOID == NULL || oidSz == NULL) {
                    WOLFSSL_MSG("Error getting ECC curve OID");
                    wc_ecc_free(&ecc);
                    return BAD_FUNC_ARG;
                }

                /* now find oid */
                if (wc_ecc_get_oid(ecc.dp->oidSum, curveOID, oidSz) < 0) {
                    WOLFSSL_MSG("Error getting ECC curve OID");
                    wc_ecc_free(&ecc);
                    return BAD_FUNC_ARG;
                }
            }
            else {
                WOLFSSL_MSG("Not ECC DER key either");
            }
            wc_ecc_free(&ecc);
        }
        else {
            WOLFSSL_MSG("GetKeyOID wc_ecc_init_ex failed");
        }
    }
#endif /* HAVE_ECC */
#ifdef HAVE_ED25519
    if (*algoID != RSAk && *algoID != ECDSAk) {
        if (wc_ed25519_init(&ed25519) == 0) {
            if (wc_Ed25519PrivateKeyDecode(key, &tmpIdx, &ed25519, keySz)
                                                                         == 0) {
                *algoID = ED25519k;
            }
            else {
                WOLFSSL_MSG("Not ED25519 DER key");
            }
            wc_ed25519_free(&ed25519);
        }
        else {
            WOLFSSL_MSG("GetKeyOID wc_ed25519_init failed");
        }
    }
#endif

    /* if flag is not set then is neither RSA or ECC key that could be
     * found */
    if (*algoID == 0) {
        WOLFSSL_MSG("Bad key DER or compile options");
        return BAD_FUNC_ARG;
    }

    (void)curveOID;
    (void)oidSz;

    return 1;
}


/* Remove Encrypted PKCS8 header, move beginning of traditional to beginning
   of input */
int ToTraditionalEnc(byte* input, word32 sz,const char* password,int passwordSz)
{
    word32 inOutIdx = 0, oid;
    int    ret = 0, first, second, length = 0, version, saltSz, id;
    int    iterations = 0;
#ifdef WOLFSSL_SMALL_STACK
    byte*  salt = NULL;
    byte*  cbcIv = NULL;
#else
    byte   salt[MAX_SALT_SIZE];
    byte   cbcIv[MAX_IV_SIZE];
#endif

    if (GetSequence(input, &inOutIdx, &length, sz) < 0) {
        ERROR_OUT(ASN_PARSE_E, exit_tte);
    }

    if (GetAlgoId(input, &inOutIdx, &oid, oidSigType, sz) < 0) {
        ERROR_OUT(ASN_PARSE_E, exit_tte);
    }

    first  = input[inOutIdx - 2];   /* PKCS version always 2nd to last byte */
    second = input[inOutIdx - 1];   /* version.algo, algo id last byte */

    if (CheckAlgo(first, second, &id, &version) < 0) {
        ERROR_OUT(ASN_INPUT_E, exit_tte); /* Algo ID error */
    }

    if (version == PKCS5v2) {
        if (GetSequence(input, &inOutIdx, &length, sz) < 0) {
            ERROR_OUT(ASN_PARSE_E, exit_tte);
        }

        if (GetAlgoId(input, &inOutIdx, &oid, oidKdfType, sz) < 0) {
            ERROR_OUT(ASN_PARSE_E, exit_tte);
        }

        if (oid != PBKDF2_OID) {
            ERROR_OUT(ASN_PARSE_E, exit_tte);
        }
    }

    if (GetSequence(input, &inOutIdx, &length, sz) <= 0) {
        ERROR_OUT(ASN_PARSE_E, exit_tte);
    }

    ret = GetOctetString(input, &inOutIdx, &saltSz, sz);
    if (ret < 0)
        goto exit_tte;

    if (saltSz > MAX_SALT_SIZE) {
        ERROR_OUT(ASN_PARSE_E, exit_tte);
    }

#ifdef WOLFSSL_SMALL_STACK
    salt = (byte*)XMALLOC(MAX_SALT_SIZE, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (salt == NULL) {
        ERROR_OUT(MEMORY_E, exit_tte);
    }
#endif

    XMEMCPY(salt, &input[inOutIdx], saltSz);
    inOutIdx += saltSz;

    if (GetShortInt(input, &inOutIdx, &iterations, sz) < 0) {
        ERROR_OUT(ASN_PARSE_E, exit_tte);
    }

#ifdef WOLFSSL_SMALL_STACK
    cbcIv = (byte*)XMALLOC(MAX_IV_SIZE, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (cbcIv == NULL) {
        ERROR_OUT(MEMORY_E, exit_tte);
    }
#endif

    if (version == PKCS5v2) {
        /* get encryption algo */
        /* JOHN: New type. Need a little more research. */
        if (GetAlgoId(input, &inOutIdx, &oid, oidBlkType, sz) < 0) {
            ERROR_OUT(ASN_PARSE_E, exit_tte);
        }

        if (CheckAlgoV2(oid, &id) < 0) {
            ERROR_OUT(ASN_PARSE_E, exit_tte); /* PKCS v2 algo id error */
        }

        ret = GetOctetString(input, &inOutIdx, &length, sz);
        if (ret < 0)
            goto exit_tte;

        if (length > MAX_IV_SIZE) {
            ERROR_OUT(ASN_PARSE_E, exit_tte);
        }

        XMEMCPY(cbcIv, &input[inOutIdx], length);
        inOutIdx += length;
    }

    ret = GetOctetString(input, &inOutIdx, &length, sz);
    if (ret < 0)
        goto exit_tte;

    ret = DecryptKey(password, passwordSz, salt, saltSz, iterations, id,
                                   input + inOutIdx, length, version, cbcIv);

exit_tte:
#ifdef WOLFSSL_SMALL_STACK
    XFREE(salt,  NULL, DYNAMIC_TYPE_TMP_BUFFER);
    XFREE(cbcIv, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    if (ret == 0) {
        XMEMMOVE(input, input + inOutIdx, length);
        ret = ToTraditional(input, length);
    }

    return ret;
}

/* decrypt PKCS */
int DecryptContent(byte* input, word32 sz,const char* password,int passwordSz)
{
    word32 inOutIdx = 0, oid;
    int    ret = 0;
    int    first, second, length = 0, version, saltSz, id;
    int    iterations = 0;
#ifdef WOLFSSL_SMALL_STACK
    byte*  salt = NULL;
    byte*  cbcIv = NULL;
#else
    byte   salt[MAX_SALT_SIZE];
    byte   cbcIv[MAX_IV_SIZE];
#endif

    if (GetAlgoId(input, &inOutIdx, &oid, oidSigType, sz) < 0) {
        ERROR_OUT(ASN_PARSE_E, exit_dc);
    }

    first  = input[inOutIdx - 2];   /* PKCS version always 2nd to last byte */
    second = input[inOutIdx - 1];   /* version.algo, algo id last byte */

    if (CheckAlgo(first, second, &id, &version) < 0) {
        ERROR_OUT(ASN_INPUT_E, exit_dc); /* Algo ID error */
    }

    if (version == PKCS5v2) {
        if (GetSequence(input, &inOutIdx, &length, sz) < 0) {
            ERROR_OUT(ASN_PARSE_E, exit_dc);
        }

        if (GetAlgoId(input, &inOutIdx, &oid, oidKdfType, sz) < 0) {
            ERROR_OUT(ASN_PARSE_E, exit_dc);
        }

        if (oid != PBKDF2_OID) {
            ERROR_OUT(ASN_PARSE_E, exit_dc);
        }
    }

    if (GetSequence(input, &inOutIdx, &length, sz) <= 0) {
        ERROR_OUT(ASN_PARSE_E, exit_dc);
    }

    ret = GetOctetString(input, &inOutIdx, &saltSz, sz);
    if (ret < 0)
        goto exit_dc;

    if (saltSz > MAX_SALT_SIZE) {
        ERROR_OUT(ASN_PARSE_E, exit_dc);
    }

#ifdef WOLFSSL_SMALL_STACK
    salt = (byte*)XMALLOC(MAX_SALT_SIZE, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (salt == NULL) {
        ERROR_OUT(MEMORY_E, exit_dc);
    }
#endif

    XMEMCPY(salt, &input[inOutIdx], saltSz);
    inOutIdx += saltSz;

    if (GetShortInt(input, &inOutIdx, &iterations, sz) < 0) {
        ERROR_OUT(ASN_PARSE_E, exit_dc);
    }

#ifdef WOLFSSL_SMALL_STACK
    cbcIv = (byte*)XMALLOC(MAX_IV_SIZE, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (cbcIv == NULL) {
        ERROR_OUT(MEMORY_E, exit_dc);
    }
#endif

    if (version == PKCS5v2) {
        /* get encryption algo */
        /* JOHN: New type. Need a little more research. */
        if (GetAlgoId(input, &inOutIdx, &oid, oidBlkType, sz) < 0) {
            ERROR_OUT(ASN_PARSE_E, exit_dc);
        }

        if (CheckAlgoV2(oid, &id) < 0) {
            ERROR_OUT(ASN_PARSE_E, exit_dc); /* PKCS v2 algo id error */
        }

        ret = GetOctetString(input, &inOutIdx, &length, sz);
        if (ret < 0)
            goto exit_dc;

        XMEMCPY(cbcIv, &input[inOutIdx], length);
        inOutIdx += length;
    }

    if (input[inOutIdx++] != (ASN_CONTEXT_SPECIFIC | 0)) {
        ERROR_OUT(ASN_PARSE_E, exit_dc);
    }

    if (GetLength(input, &inOutIdx, &length, sz) < 0) {
        ERROR_OUT(ASN_PARSE_E, exit_dc);
    }

    ret = DecryptKey(password, passwordSz, salt, saltSz, iterations, id,
                                   input + inOutIdx, length, version, cbcIv);

exit_dc:

#ifdef WOLFSSL_SMALL_STACK
    XFREE(salt,  NULL, DYNAMIC_TYPE_TMP_BUFFER);
    XFREE(cbcIv, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    if (ret == 0) {
        XMEMMOVE(input, input + inOutIdx, length);
        ret = length;
    }

    return ret;
}
#endif /* NO_PWDBASED */

#ifndef NO_RSA

#ifndef HAVE_USER_RSA
int wc_RsaPublicKeyDecode(const byte* input, word32* inOutIdx, RsaKey* key,
                       word32 inSz)
{
    int  length;
#if defined(OPENSSL_EXTRA) || defined(RSA_DECODE_EXTRA)
    byte b;
#endif
    int ret;

    if (input == NULL || inOutIdx == NULL || key == NULL)
        return BAD_FUNC_ARG;

    if (GetSequence(input, inOutIdx, &length, inSz) < 0)
        return ASN_PARSE_E;

    key->type = RSA_PUBLIC;

#if defined(OPENSSL_EXTRA) || defined(RSA_DECODE_EXTRA)
    if ((*inOutIdx + 1) > inSz)
        return BUFFER_E;

    b = input[*inOutIdx];
    if (b != ASN_INTEGER) {
        /* not from decoded cert, will have algo id, skip past */
        if (GetSequence(input, inOutIdx, &length, inSz) < 0)
            return ASN_PARSE_E;

        if (SkipObjectId(input, inOutIdx, inSz) < 0)
            return ASN_PARSE_E;

        /* Option NULL ASN.1 tag */
        if (input[*inOutIdx] == ASN_TAG_NULL) {
            ret = GetASNNull(input, inOutIdx, inSz);
            if (ret != 0)
                return ret;
        }

        /* should have bit tag length and seq next */
        ret = CheckBitString(input, inOutIdx, NULL, inSz, 1, NULL);
        if (ret != 0)
            return ret;

        if (GetSequence(input, inOutIdx, &length, inSz) < 0)
            return ASN_PARSE_E;
    }
#endif /* OPENSSL_EXTRA */

    if (GetInt(&key->n,  input, inOutIdx, inSz) < 0)
        return ASN_RSA_KEY_E;
    if (GetInt(&key->e,  input, inOutIdx, inSz) < 0) {
        mp_clear(&key->n);
        return ASN_RSA_KEY_E;
    }

    return 0;
}

/* import RSA public key elements (n, e) into RsaKey structure (key) */
int wc_RsaPublicKeyDecodeRaw(const byte* n, word32 nSz, const byte* e,
                             word32 eSz, RsaKey* key)
{
    if (n == NULL || e == NULL || key == NULL)
        return BAD_FUNC_ARG;

    key->type = RSA_PUBLIC;

    if (mp_init(&key->n) != MP_OKAY)
        return MP_INIT_E;

    if (mp_read_unsigned_bin(&key->n, n, nSz) != 0) {
        mp_clear(&key->n);
        return ASN_GETINT_E;
    }

    if (mp_init(&key->e) != MP_OKAY) {
        mp_clear(&key->n);
        return MP_INIT_E;
    }

    if (mp_read_unsigned_bin(&key->e, e, eSz) != 0) {
        mp_clear(&key->n);
        mp_clear(&key->e);
        return ASN_GETINT_E;
    }

    return 0;
}
#endif /* HAVE_USER_RSA */
#endif

#ifndef NO_DH

int wc_DhKeyDecode(const byte* input, word32* inOutIdx, DhKey* key, word32 inSz)
{
    int    length;

    if (GetSequence(input, inOutIdx, &length, inSz) < 0)
        return ASN_PARSE_E;

    if (GetInt(&key->p,  input, inOutIdx, inSz) < 0 ||
        GetInt(&key->g,  input, inOutIdx, inSz) < 0) {
        return ASN_DH_KEY_E;
    }

    return 0;
}


int wc_DhParamsLoad(const byte* input, word32 inSz, byte* p, word32* pInOutSz,
                 byte* g, word32* gInOutSz)
{
    word32 idx = 0;
    int    ret;
    int    length;

    if (GetSequence(input, &idx, &length, inSz) <= 0)
        return ASN_PARSE_E;

    ret = GetASNInt(input, &idx, &length, inSz);
    if (ret != 0)
        return ret;

    if (length <= (int)*pInOutSz) {
        XMEMCPY(p, &input[idx], length);
        *pInOutSz = length;
    }
    else {
        return BUFFER_E;
    }
    idx += length;

    ret = GetASNInt(input, &idx, &length, inSz);
    if (ret != 0)
        return ret;

    if (length <= (int)*gInOutSz) {
        XMEMCPY(g, &input[idx], length);
        *gInOutSz = length;
    }
    else {
        return BUFFER_E;
    }

    return 0;
}

#endif /* NO_DH */


#ifndef NO_DSA

int DsaPublicKeyDecode(const byte* input, word32* inOutIdx, DsaKey* key,
                        word32 inSz)
{
    int    length;

    if (input == NULL || inOutIdx == NULL || key == NULL) {
        return BAD_FUNC_ARG;
    }

    if (GetSequence(input, inOutIdx, &length, inSz) < 0)
        return ASN_PARSE_E;

    if (GetInt(&key->p,  input, inOutIdx, inSz) < 0 ||
        GetInt(&key->q,  input, inOutIdx, inSz) < 0 ||
        GetInt(&key->g,  input, inOutIdx, inSz) < 0 ||
        GetInt(&key->y,  input, inOutIdx, inSz) < 0 )
        return ASN_DH_KEY_E;

    key->type = DSA_PUBLIC;
    return 0;
}


int DsaPrivateKeyDecode(const byte* input, word32* inOutIdx, DsaKey* key,
                        word32 inSz)
{
    int    length, version;

    /* Sanity checks on input */
    if (input == NULL || inOutIdx == NULL || key == NULL) {
        return BAD_FUNC_ARG;
    }

    if (GetSequence(input, inOutIdx, &length, inSz) < 0)
        return ASN_PARSE_E;

    if (GetMyVersion(input, inOutIdx, &version, inSz) < 0)
        return ASN_PARSE_E;

    if (GetInt(&key->p,  input, inOutIdx, inSz) < 0 ||
        GetInt(&key->q,  input, inOutIdx, inSz) < 0 ||
        GetInt(&key->g,  input, inOutIdx, inSz) < 0 ||
        GetInt(&key->y,  input, inOutIdx, inSz) < 0 ||
        GetInt(&key->x,  input, inOutIdx, inSz) < 0 )
        return ASN_DH_KEY_E;

    key->type = DSA_PRIVATE;
    return 0;
}

static mp_int* GetDsaInt(DsaKey* key, int idx)
{
    if (idx == 0)
        return &key->p;
    if (idx == 1)
        return &key->q;
    if (idx == 2)
        return &key->g;
    if (idx == 3)
        return &key->y;
    if (idx == 4)
        return &key->x;

    return NULL;
}

/* Release Tmp DSA resources */
static INLINE void FreeTmpDsas(byte** tmps, void* heap)
{
    int i;

    for (i = 0; i < DSA_INTS; i++)
        XFREE(tmps[i], heap, DYNAMIC_TYPE_DSA);

    (void)heap;
}

/* Convert DsaKey key to DER format, write to output (inLen), return bytes
 written */
int wc_DsaKeyToDer(DsaKey* key, byte* output, word32 inLen)
{
    word32 seqSz, verSz, rawLen, intTotalLen = 0;
    word32 sizes[DSA_INTS];
    int    i, j, outLen, ret = 0, mpSz;

    byte  seq[MAX_SEQ_SZ];
    byte  ver[MAX_VERSION_SZ];
    byte* tmps[DSA_INTS];

    if (!key || !output)
        return BAD_FUNC_ARG;

    if (key->type != DSA_PRIVATE)
        return BAD_FUNC_ARG;

    for (i = 0; i < DSA_INTS; i++)
        tmps[i] = NULL;

    /* write all big ints from key to DER tmps */
    for (i = 0; i < DSA_INTS; i++) {
        mp_int* keyInt = GetDsaInt(key, i);

        rawLen = mp_unsigned_bin_size(keyInt) + 1;
        tmps[i] = (byte*)XMALLOC(rawLen + MAX_SEQ_SZ, key->heap,
                                                              DYNAMIC_TYPE_DSA);
        if (tmps[i] == NULL) {
            ret = MEMORY_E;
            break;
        }

        mpSz = SetASNIntMP(keyInt, -1, tmps[i]);
        if (mpSz < 0) {
            ret = mpSz;
            break;
        }
        intTotalLen += (sizes[i] = mpSz);
    }

    if (ret != 0) {
        FreeTmpDsas(tmps, key->heap);
        return ret;
    }

    /* make headers */
    verSz = SetMyVersion(0, ver, FALSE);
    seqSz = SetSequence(verSz + intTotalLen, seq);

    outLen = seqSz + verSz + intTotalLen;
    if (outLen > (int)inLen)
        return BAD_FUNC_ARG;

    /* write to output */
    XMEMCPY(output, seq, seqSz);
    j = seqSz;
    XMEMCPY(output + j, ver, verSz);
    j += verSz;

    for (i = 0; i < DSA_INTS; i++) {
        XMEMCPY(output + j, tmps[i], sizes[i]);
        j += sizes[i];
    }
    FreeTmpDsas(tmps, key->heap);

    return outLen;
}

#endif /* NO_DSA */


void InitDecodedCert(DecodedCert* cert, byte* source, word32 inSz, void* heap)
{
    if (cert != NULL) {
        XMEMSET(cert, 0, sizeof(DecodedCert));

        cert->subjectCNEnc    = CTC_UTF8;
        cert->issuer[0]       = '\0';
        cert->subject[0]      = '\0';
        cert->source          = source;  /* don't own */
        cert->maxIdx          = inSz;    /* can't go over this index */
        cert->heap            = heap;
    #ifdef WOLFSSL_CERT_GEN
        cert->subjectSNEnc    = CTC_UTF8;
        cert->subjectCEnc     = CTC_PRINTABLE;
        cert->subjectLEnc     = CTC_UTF8;
        cert->subjectSTEnc    = CTC_UTF8;
        cert->subjectOEnc     = CTC_UTF8;
        cert->subjectOUEnc    = CTC_UTF8;
    #endif /* WOLFSSL_CERT_GEN */

        InitSignatureCtx(&cert->sigCtx, heap, INVALID_DEVID);
    }
}


void FreeAltNames(DNS_entry* altNames, void* heap)
{
    (void)heap;

    while (altNames) {
        DNS_entry* tmp = altNames->next;

        XFREE(altNames->name, heap, DYNAMIC_TYPE_ALTNAME);
        XFREE(altNames,       heap, DYNAMIC_TYPE_ALTNAME);
        altNames = tmp;
    }
}

#ifndef IGNORE_NAME_CONSTRAINTS

void FreeNameSubtrees(Base_entry* names, void* heap)
{
    (void)heap;

    while (names) {
        Base_entry* tmp = names->next;

        XFREE(names->name, heap, DYNAMIC_TYPE_ALTNAME);
        XFREE(names,       heap, DYNAMIC_TYPE_ALTNAME);
        names = tmp;
    }
}

#endif /* IGNORE_NAME_CONSTRAINTS */

void FreeDecodedCert(DecodedCert* cert)
{
    if (cert->subjectCNStored == 1)
        XFREE(cert->subjectCN, cert->heap, DYNAMIC_TYPE_SUBJECT_CN);
    if (cert->pubKeyStored == 1)
        XFREE(cert->publicKey, cert->heap, DYNAMIC_TYPE_PUBLIC_KEY);
    if (cert->weOwnAltNames && cert->altNames)
        FreeAltNames(cert->altNames, cert->heap);
#ifndef IGNORE_NAME_CONSTRAINTS
    if (cert->altEmailNames)
        FreeAltNames(cert->altEmailNames, cert->heap);
    if (cert->permittedNames)
        FreeNameSubtrees(cert->permittedNames, cert->heap);
    if (cert->excludedNames)
        FreeNameSubtrees(cert->excludedNames, cert->heap);
#endif /* IGNORE_NAME_CONSTRAINTS */
#ifdef WOLFSSL_SEP
    XFREE(cert->deviceType, cert->heap, DYNAMIC_TYPE_X509_EXT);
    XFREE(cert->hwType, cert->heap, DYNAMIC_TYPE_X509_EXT);
    XFREE(cert->hwSerialNum, cert->heap, DYNAMIC_TYPE_X509_EXT);
#endif /* WOLFSSL_SEP */
#ifdef OPENSSL_EXTRA
    if (cert->issuerName.fullName != NULL)
        XFREE(cert->issuerName.fullName, cert->heap, DYNAMIC_TYPE_X509);
    if (cert->subjectName.fullName != NULL)
        XFREE(cert->subjectName.fullName, cert->heap, DYNAMIC_TYPE_X509);
#endif /* OPENSSL_EXTRA */
    FreeSignatureCtx(&cert->sigCtx);
}

static int GetCertHeader(DecodedCert* cert)
{
    int ret = 0, len;

    if (GetSequence(cert->source, &cert->srcIdx, &len, cert->maxIdx) < 0)
        return ASN_PARSE_E;

    cert->certBegin = cert->srcIdx;

    if (GetSequence(cert->source, &cert->srcIdx, &len, cert->maxIdx) < 0)
        return ASN_PARSE_E;
    cert->sigIndex = len + cert->srcIdx;

    if (GetExplicitVersion(cert->source, &cert->srcIdx, &cert->version,
                                                              cert->maxIdx) < 0)
        return ASN_PARSE_E;

    if (GetSerialNumber(cert->source, &cert->srcIdx, cert->serial,
                                        &cert->serialSz, cert->maxIdx) < 0)
        return ASN_PARSE_E;

    return ret;
}

#if !defined(NO_RSA)
/* Store Rsa Key, may save later, Dsa could use in future */
static int StoreRsaKey(DecodedCert* cert)
{
    int    length;
    word32 recvd = cert->srcIdx;

    if (GetSequence(cert->source, &cert->srcIdx, &length, cert->maxIdx) < 0)
        return ASN_PARSE_E;

    recvd = cert->srcIdx - recvd;
    length += recvd;

    while (recvd--)
       cert->srcIdx--;

    cert->pubKeySize = length;
    cert->publicKey = cert->source + cert->srcIdx;
    cert->srcIdx += length;

    return 0;
}
#endif /* !NO_RSA */

#ifdef HAVE_ECC

    /* return 0 on success if the ECC curve oid sum is supported */
    static int CheckCurve(word32 oid)
    {
        int ret = 0;
        word32 oidSz = 0;

        ret = wc_ecc_get_oid(oid, NULL, &oidSz);
        if (ret < 0 || oidSz <= 0) {
            WOLFSSL_MSG("CheckCurve not found");
            ret = ALGO_ID_E;
        }

        return ret;
    }

#endif /* HAVE_ECC */

static int GetKey(DecodedCert* cert)
{
    int length;
#ifdef HAVE_NTRU
    int tmpIdx = cert->srcIdx;
#endif

    if (GetSequence(cert->source, &cert->srcIdx, &length, cert->maxIdx) < 0)
        return ASN_PARSE_E;

    if (GetAlgoId(cert->source, &cert->srcIdx,
                  &cert->keyOID, oidKeyType, cert->maxIdx) < 0)
        return ASN_PARSE_E;

    switch (cert->keyOID) {
   #ifndef NO_RSA
        case RSAk:
        {
            int ret;
            ret = CheckBitString(cert->source, &cert->srcIdx, NULL,
                                 cert->maxIdx, 1, NULL);
            if (ret != 0)
                return ret;

            return StoreRsaKey(cert);
        }

    #endif /* NO_RSA */
    #ifdef HAVE_NTRU
        case NTRUk:
        {
            const byte* key = &cert->source[tmpIdx];
            byte*       next = (byte*)key;
            word16      keyLen;
            word32      rc;
            word32      remaining = cert->maxIdx - cert->srcIdx;
#ifdef WOLFSSL_SMALL_STACK
            byte*       keyBlob = NULL;
#else
            byte        keyBlob[MAX_NTRU_KEY_SZ];
#endif
            rc = ntru_crypto_ntru_encrypt_subjectPublicKeyInfo2PublicKey(key,
                                &keyLen, NULL, &next, &remaining);
            if (rc != NTRU_OK)
                return ASN_NTRU_KEY_E;
            if (keyLen > MAX_NTRU_KEY_SZ)
                return ASN_NTRU_KEY_E;

#ifdef WOLFSSL_SMALL_STACK
            keyBlob = (byte*)XMALLOC(MAX_NTRU_KEY_SZ, NULL,
                                     DYNAMIC_TYPE_TMP_BUFFER);
            if (keyBlob == NULL)
                return MEMORY_E;
#endif

            rc = ntru_crypto_ntru_encrypt_subjectPublicKeyInfo2PublicKey(key,
                                &keyLen, keyBlob, &next, &remaining);
            if (rc != NTRU_OK) {
#ifdef WOLFSSL_SMALL_STACK
                XFREE(keyBlob, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
                return ASN_NTRU_KEY_E;
            }

            if ( (next - key) < 0) {
#ifdef WOLFSSL_SMALL_STACK
                XFREE(keyBlob, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
                return ASN_NTRU_KEY_E;
            }

            cert->srcIdx = tmpIdx + (int)(next - key);

            cert->publicKey = (byte*) XMALLOC(keyLen, cert->heap,
                                              DYNAMIC_TYPE_PUBLIC_KEY);
            if (cert->publicKey == NULL) {
#ifdef WOLFSSL_SMALL_STACK
                XFREE(keyBlob, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
                return MEMORY_E;
            }
            XMEMCPY(cert->publicKey, keyBlob, keyLen);
            cert->pubKeyStored = 1;
            cert->pubKeySize   = keyLen;

#ifdef WOLFSSL_SMALL_STACK
            XFREE(keyBlob, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

            return 0;
        }
    #endif /* HAVE_NTRU */
    #ifdef HAVE_ECC
        case ECDSAk:
        {
            int ret;

            if (GetObjectId(cert->source, &cert->srcIdx,
                            &cert->pkCurveOID, oidCurveType, cert->maxIdx) < 0)
                return ASN_PARSE_E;

            if (CheckCurve(cert->pkCurveOID) < 0)
                return ECC_CURVE_OID_E;

            /* key header */
            ret = CheckBitString(cert->source, &cert->srcIdx, &length,
                                 cert->maxIdx, 1, NULL);
            if (ret != 0)
                return ret;

            cert->publicKey = (byte*)XMALLOC(length, cert->heap,
                                             DYNAMIC_TYPE_PUBLIC_KEY);
            if (cert->publicKey == NULL)
                return MEMORY_E;
            XMEMCPY(cert->publicKey, &cert->source[cert->srcIdx], length);
            cert->pubKeyStored = 1;
            cert->pubKeySize   = length;

            cert->srcIdx += length;

            return 0;
        }
    #endif /* HAVE_ECC */
    #ifdef HAVE_ED25519
        case ED25519k:
        {
            int ret;

            cert->pkCurveOID = ED25519k;

            ret = CheckBitString(cert->source, &cert->srcIdx, &length,
                                 cert->maxIdx, 1, NULL);
            if (ret != 0)
                return ret;

            cert->publicKey = (byte*) XMALLOC(length, cert->heap,
                                              DYNAMIC_TYPE_PUBLIC_KEY);
            if (cert->publicKey == NULL)
                return MEMORY_E;
            XMEMCPY(cert->publicKey, &cert->source[cert->srcIdx], length);
            cert->pubKeyStored = 1;
            cert->pubKeySize   = length;

            cert->srcIdx += length;

            return 0;
        }
    #endif /* HAVE_ED25519 */
        default:
            return ASN_UNKNOWN_OID_E;
    }
}

/* process NAME, either issuer or subject */
static int GetName(DecodedCert* cert, int nameType)
{
    int    length;  /* length of all distinguished names */
    int    dummy;
    int    ret;
    char*  full;
    byte*  hash;
    word32 idx;
    #ifdef OPENSSL_EXTRA
        DecodedName* dName =
                  (nameType == ISSUER) ? &cert->issuerName : &cert->subjectName;
    #endif /* OPENSSL_EXTRA */

    WOLFSSL_MSG("Getting Cert Name");

    if (nameType == ISSUER) {
        full = cert->issuer;
        hash = cert->issuerHash;
    }
    else {
        full = cert->subject;
        hash = cert->subjectHash;
    }

    if (cert->source[cert->srcIdx] == ASN_OBJECT_ID) {
        WOLFSSL_MSG("Trying optional prefix...");

        if (SkipObjectId(cert->source, &cert->srcIdx, cert->maxIdx) < 0)
            return ASN_PARSE_E;
        WOLFSSL_MSG("Got optional prefix");
    }

    /* For OCSP, RFC2560 section 4.1.1 states the issuer hash should be
     * calculated over the entire DER encoding of the Name field, including
     * the tag and length. */
    idx = cert->srcIdx;
    if (GetSequence(cert->source, &cert->srcIdx, &length, cert->maxIdx) < 0)
        return ASN_PARSE_E;

#ifdef NO_SHA
    ret = wc_Sha256Hash(&cert->source[idx], length + cert->srcIdx - idx, hash);
#else
    ret = wc_ShaHash(&cert->source[idx], length + cert->srcIdx - idx, hash);
#endif
    if (ret != 0)
        return ret;

    length += cert->srcIdx;
    idx = 0;

#ifdef HAVE_PKCS7
    /* store pointer to raw issuer */
    if (nameType == ISSUER) {
        cert->issuerRaw = &cert->source[cert->srcIdx];
        cert->issuerRawLen = length - cert->srcIdx;
    }
#endif
#ifndef IGNORE_NAME_CONSTRAINTS
    if (nameType == SUBJECT) {
        cert->subjectRaw = &cert->source[cert->srcIdx];
        cert->subjectRawLen = length - cert->srcIdx;
    }
#endif

    while (cert->srcIdx < (word32)length) {
        byte   b;
        byte   joint[2];
        byte   tooBig = FALSE;
        int    oidSz;

        if (GetSet(cert->source, &cert->srcIdx, &dummy, cert->maxIdx) < 0) {
            WOLFSSL_MSG("Cert name lacks set header, trying sequence");
        }

        if (GetSequence(cert->source, &cert->srcIdx, &dummy, cert->maxIdx) <= 0)
            return ASN_PARSE_E;

        ret = GetASNObjectId(cert->source, &cert->srcIdx, &oidSz, cert->maxIdx);
        if (ret != 0)
            return ret;

        /* make sure there is room for joint */
        if ((cert->srcIdx + sizeof(joint)) > cert->maxIdx)
            return ASN_PARSE_E;

        XMEMCPY(joint, &cert->source[cert->srcIdx], sizeof(joint));

        /* v1 name types */
        if (joint[0] == 0x55 && joint[1] == 0x04) {
            byte   id;
            byte   copy = FALSE;
            int    strLen;

            cert->srcIdx += 2;
            id = cert->source[cert->srcIdx++];
            b  = cert->source[cert->srcIdx++]; /* encoding */

            if (GetLength(cert->source, &cert->srcIdx, &strLen,
                          cert->maxIdx) < 0)
                return ASN_PARSE_E;

            if ( (strLen + 14) > (int)(ASN_NAME_MAX - idx)) {
                /* include biggest pre fix header too 4 = "/serialNumber=" */
                WOLFSSL_MSG("ASN Name too big, skipping");
                tooBig = TRUE;
            }

            if (id == ASN_COMMON_NAME) {
                if (nameType == SUBJECT) {
                    cert->subjectCN = (char *)&cert->source[cert->srcIdx];
                    cert->subjectCNLen = strLen;
                    cert->subjectCNEnc = b;
                }

                if (!tooBig) {
                    XMEMCPY(&full[idx], "/CN=", 4);
                    idx += 4;
                    copy = TRUE;
                }
                #ifdef OPENSSL_EXTRA
                    dName->cnIdx = cert->srcIdx;
                    dName->cnLen = strLen;
                #endif /* OPENSSL_EXTRA */
            }
            else if (id == ASN_SUR_NAME) {
                if (!tooBig) {
                    XMEMCPY(&full[idx], "/SN=", 4);
                    idx += 4;
                    copy = TRUE;
                }
                #ifdef WOLFSSL_CERT_GEN
                    if (nameType == SUBJECT) {
                        cert->subjectSN = (char*)&cert->source[cert->srcIdx];
                        cert->subjectSNLen = strLen;
                        cert->subjectSNEnc = b;
                    }
                #endif /* WOLFSSL_CERT_GEN */
                #ifdef OPENSSL_EXTRA
                    dName->snIdx = cert->srcIdx;
                    dName->snLen = strLen;
                #endif /* OPENSSL_EXTRA */
            }
            else if (id == ASN_COUNTRY_NAME) {
                if (!tooBig) {
                    XMEMCPY(&full[idx], "/C=", 3);
                    idx += 3;
                    copy = TRUE;
                }
                #ifdef WOLFSSL_CERT_GEN
                    if (nameType == SUBJECT) {
                        cert->subjectC = (char*)&cert->source[cert->srcIdx];
                        cert->subjectCLen = strLen;
                        cert->subjectCEnc = b;
                    }
                #endif /* WOLFSSL_CERT_GEN */
                #ifdef OPENSSL_EXTRA
                    dName->cIdx = cert->srcIdx;
                    dName->cLen = strLen;
                #endif /* OPENSSL_EXTRA */
            }
            else if (id == ASN_LOCALITY_NAME) {
                if (!tooBig) {
                    XMEMCPY(&full[idx], "/L=", 3);
                    idx += 3;
                    copy = TRUE;
                }
                #ifdef WOLFSSL_CERT_GEN
                    if (nameType == SUBJECT) {
                        cert->subjectL = (char*)&cert->source[cert->srcIdx];
                        cert->subjectLLen = strLen;
                        cert->subjectLEnc = b;
                    }
                #endif /* WOLFSSL_CERT_GEN */
                #ifdef OPENSSL_EXTRA
                    dName->lIdx = cert->srcIdx;
                    dName->lLen = strLen;
                #endif /* OPENSSL_EXTRA */
            }
            else if (id == ASN_STATE_NAME) {
                if (!tooBig) {
                    XMEMCPY(&full[idx], "/ST=", 4);
                    idx += 4;
                    copy = TRUE;
                }
                #ifdef WOLFSSL_CERT_GEN
                    if (nameType == SUBJECT) {
                        cert->subjectST = (char*)&cert->source[cert->srcIdx];
                        cert->subjectSTLen = strLen;
                        cert->subjectSTEnc = b;
                    }
                #endif /* WOLFSSL_CERT_GEN */
                #ifdef OPENSSL_EXTRA
                    dName->stIdx = cert->srcIdx;
                    dName->stLen = strLen;
                #endif /* OPENSSL_EXTRA */
            }
            else if (id == ASN_ORG_NAME) {
                if (!tooBig) {
                    XMEMCPY(&full[idx], "/O=", 3);
                    idx += 3;
                    copy = TRUE;
                }
                #ifdef WOLFSSL_CERT_GEN
                    if (nameType == SUBJECT) {
                        cert->subjectO = (char*)&cert->source[cert->srcIdx];
                        cert->subjectOLen = strLen;
                        cert->subjectOEnc = b;
                    }
                #endif /* WOLFSSL_CERT_GEN */
                #ifdef OPENSSL_EXTRA
                    dName->oIdx = cert->srcIdx;
                    dName->oLen = strLen;
                #endif /* OPENSSL_EXTRA */
            }
            else if (id == ASN_ORGUNIT_NAME) {
                if (!tooBig) {
                    XMEMCPY(&full[idx], "/OU=", 4);
                    idx += 4;
                    copy = TRUE;
                }
                #ifdef WOLFSSL_CERT_GEN
                    if (nameType == SUBJECT) {
                        cert->subjectOU = (char*)&cert->source[cert->srcIdx];
                        cert->subjectOULen = strLen;
                        cert->subjectOUEnc = b;
                    }
                #endif /* WOLFSSL_CERT_GEN */
                #ifdef OPENSSL_EXTRA
                    dName->ouIdx = cert->srcIdx;
                    dName->ouLen = strLen;
                #endif /* OPENSSL_EXTRA */
            }
            else if (id == ASN_SERIAL_NUMBER) {
                if (!tooBig) {
                   XMEMCPY(&full[idx], "/serialNumber=", 14);
                   idx += 14;
                   copy = TRUE;
                }
                #ifdef OPENSSL_EXTRA
                    dName->snIdx = cert->srcIdx;
                    dName->snLen = strLen;
                #endif /* OPENSSL_EXTRA */
            }

            if (copy && !tooBig) {
                XMEMCPY(&full[idx], &cert->source[cert->srcIdx], strLen);
                idx += strLen;
            }

            cert->srcIdx += strLen;
        }
        else {
            /* skip */
            byte email = FALSE;
            byte uid   = FALSE;
            int  adv;

            if (joint[0] == 0x2a && joint[1] == 0x86)  /* email id hdr */
                email = TRUE;

            if (joint[0] == 0x9  && joint[1] == 0x92)  /* uid id hdr */
                uid = TRUE;

            cert->srcIdx += oidSz + 1;

            if (GetLength(cert->source, &cert->srcIdx, &adv, cert->maxIdx) < 0)
                return ASN_PARSE_E;

            if (adv > (int)(ASN_NAME_MAX - idx)) {
                WOLFSSL_MSG("ASN name too big, skipping");
                tooBig = TRUE;
            }

            if (email) {
                if ( (14 + adv) > (int)(ASN_NAME_MAX - idx)) {
                    WOLFSSL_MSG("ASN name too big, skipping");
                    tooBig = TRUE;
                }
                if (!tooBig) {
                    XMEMCPY(&full[idx], "/emailAddress=", 14);
                    idx += 14;
                }

                #ifdef WOLFSSL_CERT_GEN
                    if (nameType == SUBJECT) {
                        cert->subjectEmail = (char*)&cert->source[cert->srcIdx];
                        cert->subjectEmailLen = adv;
                    }
                #endif /* WOLFSSL_CERT_GEN */
                #ifdef OPENSSL_EXTRA
                    dName->emailIdx = cert->srcIdx;
                    dName->emailLen = adv;
                #endif /* OPENSSL_EXTRA */
                #ifndef IGNORE_NAME_CONSTRAINTS
                    {
                        DNS_entry* emailName = NULL;

                        emailName = (DNS_entry*)XMALLOC(sizeof(DNS_entry),
                                              cert->heap, DYNAMIC_TYPE_ALTNAME);
                        if (emailName == NULL) {
                            WOLFSSL_MSG("\tOut of Memory");
                            return MEMORY_E;
                        }
                        emailName->name = (char*)XMALLOC(adv + 1,
                                              cert->heap, DYNAMIC_TYPE_ALTNAME);
                        if (emailName->name == NULL) {
                            WOLFSSL_MSG("\tOut of Memory");
                            XFREE(emailName, cert->heap, DYNAMIC_TYPE_ALTNAME);
                            return MEMORY_E;
                        }
                        XMEMCPY(emailName->name,
                                              &cert->source[cert->srcIdx], adv);
                        emailName->name[adv] = 0;

                        emailName->next = cert->altEmailNames;
                        cert->altEmailNames = emailName;
                    }
                #endif /* IGNORE_NAME_CONSTRAINTS */
                if (!tooBig) {
                    XMEMCPY(&full[idx], &cert->source[cert->srcIdx], adv);
                    idx += adv;
                }
            }

            if (uid) {
                if ( (5 + adv) > (int)(ASN_NAME_MAX - idx)) {
                    WOLFSSL_MSG("ASN name too big, skipping");
                    tooBig = TRUE;
                }
                if (!tooBig) {
                    XMEMCPY(&full[idx], "/UID=", 5);
                    idx += 5;

                    XMEMCPY(&full[idx], &cert->source[cert->srcIdx], adv);
                    idx += adv;
                }
                #ifdef OPENSSL_EXTRA
                    dName->uidIdx = cert->srcIdx;
                    dName->uidLen = adv;
                #endif /* OPENSSL_EXTRA */
            }

            cert->srcIdx += adv;
        }
    }
    full[idx++] = 0;

    #ifdef OPENSSL_EXTRA
    {
        int totalLen = 0;

        if (dName->cnLen != 0)
            totalLen += dName->cnLen + 4;
        if (dName->snLen != 0)
            totalLen += dName->snLen + 4;
        if (dName->cLen != 0)
            totalLen += dName->cLen + 3;
        if (dName->lLen != 0)
            totalLen += dName->lLen + 3;
        if (dName->stLen != 0)
            totalLen += dName->stLen + 4;
        if (dName->oLen != 0)
            totalLen += dName->oLen + 3;
        if (dName->ouLen != 0)
            totalLen += dName->ouLen + 4;
        if (dName->emailLen != 0)
            totalLen += dName->emailLen + 14;
        if (dName->uidLen != 0)
            totalLen += dName->uidLen + 5;
        if (dName->serialLen != 0)
            totalLen += dName->serialLen + 14;

        dName->fullName = (char*)XMALLOC(totalLen + 1, cert->heap,
                                                             DYNAMIC_TYPE_X509);
        if (dName->fullName != NULL) {
            idx = 0;

            if (dName->cnLen != 0) {
                dName->entryCount++;
                XMEMCPY(&dName->fullName[idx], "/CN=", 4);
                idx += 4;
                XMEMCPY(&dName->fullName[idx],
                                     &cert->source[dName->cnIdx], dName->cnLen);
                dName->cnIdx = idx;
                idx += dName->cnLen;
            }
            if (dName->snLen != 0) {
                dName->entryCount++;
                XMEMCPY(&dName->fullName[idx], "/SN=", 4);
                idx += 4;
                XMEMCPY(&dName->fullName[idx],
                                     &cert->source[dName->snIdx], dName->snLen);
                dName->snIdx = idx;
                idx += dName->snLen;
            }
            if (dName->cLen != 0) {
                dName->entryCount++;
                XMEMCPY(&dName->fullName[idx], "/C=", 3);
                idx += 3;
                XMEMCPY(&dName->fullName[idx],
                                       &cert->source[dName->cIdx], dName->cLen);
                dName->cIdx = idx;
                idx += dName->cLen;
            }
            if (dName->lLen != 0) {
                dName->entryCount++;
                XMEMCPY(&dName->fullName[idx], "/L=", 3);
                idx += 3;
                XMEMCPY(&dName->fullName[idx],
                                       &cert->source[dName->lIdx], dName->lLen);
                dName->lIdx = idx;
                idx += dName->lLen;
            }
            if (dName->stLen != 0) {
                dName->entryCount++;
                XMEMCPY(&dName->fullName[idx], "/ST=", 4);
                idx += 4;
                XMEMCPY(&dName->fullName[idx],
                                     &cert->source[dName->stIdx], dName->stLen);
                dName->stIdx = idx;
                idx += dName->stLen;
            }
            if (dName->oLen != 0) {
                dName->entryCount++;
                XMEMCPY(&dName->fullName[idx], "/O=", 3);
                idx += 3;
                XMEMCPY(&dName->fullName[idx],
                                       &cert->source[dName->oIdx], dName->oLen);
                dName->oIdx = idx;
                idx += dName->oLen;
            }
            if (dName->ouLen != 0) {
                dName->entryCount++;
                XMEMCPY(&dName->fullName[idx], "/OU=", 4);
                idx += 4;
                XMEMCPY(&dName->fullName[idx],
                                     &cert->source[dName->ouIdx], dName->ouLen);
                dName->ouIdx = idx;
                idx += dName->ouLen;
            }
            if (dName->emailLen != 0) {
                dName->entryCount++;
                XMEMCPY(&dName->fullName[idx], "/emailAddress=", 14);
                idx += 14;
                XMEMCPY(&dName->fullName[idx],
                               &cert->source[dName->emailIdx], dName->emailLen);
                dName->emailIdx = idx;
                idx += dName->emailLen;
            }
            if (dName->uidLen != 0) {
                dName->entryCount++;
                XMEMCPY(&dName->fullName[idx], "/UID=", 5);
                idx += 5;
                XMEMCPY(&dName->fullName[idx],
                                   &cert->source[dName->uidIdx], dName->uidLen);
                dName->uidIdx = idx;
                idx += dName->uidLen;
            }
            if (dName->serialLen != 0) {
                dName->entryCount++;
                XMEMCPY(&dName->fullName[idx], "/serialNumber=", 14);
                idx += 14;
                XMEMCPY(&dName->fullName[idx],
                             &cert->source[dName->serialIdx], dName->serialLen);
                dName->serialIdx = idx;
                idx += dName->serialLen;
            }
            dName->fullName[idx] = '\0';
            dName->fullNameLen = totalLen;
        }
    }
    #endif /* OPENSSL_EXTRA */

    return 0;
}


#ifndef NO_ASN_TIME
#if !defined(NO_TIME_H) && defined(USE_WOLF_VALIDDATE)

/* to the second */
static int DateGreaterThan(const struct tm* a, const struct tm* b)
{
    if (a->tm_year > b->tm_year)
        return 1;

    if (a->tm_year == b->tm_year && a->tm_mon > b->tm_mon)
        return 1;

    if (a->tm_year == b->tm_year && a->tm_mon == b->tm_mon &&
           a->tm_mday > b->tm_mday)
        return 1;

    if (a->tm_year == b->tm_year && a->tm_mon == b->tm_mon &&
        a->tm_mday == b->tm_mday && a->tm_hour > b->tm_hour)
        return 1;

    if (a->tm_year == b->tm_year && a->tm_mon == b->tm_mon &&
        a->tm_mday == b->tm_mday && a->tm_hour == b->tm_hour &&
        a->tm_min > b->tm_min)
        return 1;

    if (a->tm_year == b->tm_year && a->tm_mon == b->tm_mon &&
        a->tm_mday == b->tm_mday && a->tm_hour == b->tm_hour &&
        a->tm_min  == b->tm_min  && a->tm_sec > b->tm_sec)
        return 1;

    return 0; /* false */
}


static INLINE int DateLessThan(const struct tm* a, const struct tm* b)
{
    return DateGreaterThan(b,a);
}


#if defined(WOLFSSL_MYSQL_COMPATIBLE) || defined(WOLFSSL_NGINX) || defined(WOLFSSL_HAPROXY)
int GetTimeString(byte* date, int format, char* buf, int len)
{
    struct tm t;
    int idx = 0;

    if (!ExtractDate(date, format, &t, &idx)) {
        return 0;
    }

    if (date[idx] != 'Z') {
        WOLFSSL_MSG("UTCtime, not Zulu") ;
        return 0;
    }

    /* place month in buffer */
    buf[0] = '\0';
    switch(t.tm_mon) {
        case 0:  XSTRNCAT(buf, "Jan ", 4); break;
        case 1:  XSTRNCAT(buf, "Feb ", 4); break;
        case 2:  XSTRNCAT(buf, "Mar ", 4); break;
        case 3:  XSTRNCAT(buf, "Apr ", 4); break;
        case 4:  XSTRNCAT(buf, "May ", 4); break;
        case 5:  XSTRNCAT(buf, "Jun ", 4); break;
        case 6:  XSTRNCAT(buf, "Jul ", 4); break;
        case 7:  XSTRNCAT(buf, "Aug ", 4); break;
        case 8:  XSTRNCAT(buf, "Sep ", 4); break;
        case 9:  XSTRNCAT(buf, "Oct ", 4); break;
        case 10: XSTRNCAT(buf, "Nov ", 4); break;
        case 11: XSTRNCAT(buf, "Dec ", 4); break;
        default:
            return 0;

    }
    idx = 4; /* use idx now for char buffer */
    buf[idx] = ' ';

    XSNPRINTF(buf + idx, len - idx, "%2d %02d:%02d:%02d %d GMT",
              t.tm_mday, t.tm_hour, t.tm_min, t.tm_sec, t.tm_year + 1900);

    return 1;
}
#endif /* WOLFSSL_MYSQL_COMPATIBLE */

int ExtractDate(const unsigned char* date, unsigned char format,
                                                  struct tm* certTime, int* idx)
{
    XMEMSET(certTime, 0, sizeof(struct tm));

    if (format == ASN_UTC_TIME) {
        if (btoi(date[0]) >= 5)
            certTime->tm_year = 1900;
        else
            certTime->tm_year = 2000;
    }
    else  { /* format == GENERALIZED_TIME */
        certTime->tm_year += btoi(date[*idx]) * 1000; *idx = *idx + 1;
        certTime->tm_year += btoi(date[*idx]) * 100;  *idx = *idx + 1;
    }

    /* adjust tm_year, tm_mon */
    GetTime((int*)&certTime->tm_year, date, idx); certTime->tm_year -= 1900;
    GetTime((int*)&certTime->tm_mon,  date, idx); certTime->tm_mon  -= 1;
    GetTime((int*)&certTime->tm_mday, date, idx);
    GetTime((int*)&certTime->tm_hour, date, idx);
    GetTime((int*)&certTime->tm_min,  date, idx);
    GetTime((int*)&certTime->tm_sec,  date, idx);

    return 1;
}


/* like atoi but only use first byte */
/* Make sure before and after dates are valid */
int ValidateDate(const byte* date, byte format, int dateType)
{
    time_t ltime;
    struct tm  certTime;
    struct tm* localTime;
    struct tm* tmpTime = NULL;
    int    i = 0;
    int    timeDiff = 0 ;
    int    diffHH = 0 ; int diffMM = 0 ;
    int    diffSign = 0 ;

#if defined(NEED_TMP_TIME)
    struct tm tmpTimeStorage;
    tmpTime = &tmpTimeStorage;
#else
    (void)tmpTime;
#endif

    ltime = XTIME(0);

#ifdef WOLFSSL_BEFORE_DATE_CLOCK_SKEW
    if (dateType == BEFORE) {
        WOLFSSL_MSG("Skewing local time for before date check");
        ltime += WOLFSSL_BEFORE_DATE_CLOCK_SKEW;
    }
#endif

#ifdef WOLFSSL_AFTER_DATE_CLOCK_SKEW
    if (dateType == AFTER) {
        WOLFSSL_MSG("Skewing local time for after date check");
        ltime -= WOLFSSL_AFTER_DATE_CLOCK_SKEW;
    }
#endif

    if (!ExtractDate(date, format, &certTime, &i)) {
        WOLFSSL_MSG("Error extracting the date");
        return 0;
    }

    if ((date[i] == '+') || (date[i] == '-')) {
        WOLFSSL_MSG("Using time differential, not Zulu") ;
        diffSign = date[i++] == '+' ? 1 : -1 ;
        GetTime(&diffHH, date, &i);
        GetTime(&diffMM, date, &i);
        timeDiff = diffSign * (diffHH*60 + diffMM) * 60 ;
    } else if (date[i] != 'Z') {
        WOLFSSL_MSG("UTCtime, niether Zulu or time differential") ;
        return 0;
    }

    ltime -= (time_t)timeDiff ;
    localTime = XGMTIME(&ltime, tmpTime);

    if (localTime == NULL) {
        WOLFSSL_MSG("XGMTIME failed");
        return 0;
    }

    if (dateType == BEFORE) {
        if (DateLessThan(localTime, &certTime)) {
            WOLFSSL_MSG("Date BEFORE check failed");
            return 0;
        }
    }
    else {  /* dateType == AFTER */
        if (DateGreaterThan(localTime, &certTime)) {
            WOLFSSL_MSG("Date AFTER check failed");
            return 0;
        }
    }

    return 1;
}
#endif /* !NO_TIME_H && USE_WOLF_VALIDDATE */

int wc_GetTime(void* timePtr, word32 timeSize)
{
    time_t* ltime = (time_t*)timePtr;

    if (timePtr == NULL) {
        return BAD_FUNC_ARG;
    }

    if ((word32)sizeof(time_t) > timeSize) {
        return BUFFER_E;
    }

    *ltime = XTIME(0);

    return 0;
}

#endif /* !NO_ASN_TIME */

static int GetDate(DecodedCert* cert, int dateType)
{
    int    length;
    byte   date[MAX_DATE_SIZE];
    byte   b;
    word32 startIdx = 0;

    XMEMSET(date, 0, MAX_DATE_SIZE);

    if (dateType == BEFORE)
        cert->beforeDate = &cert->source[cert->srcIdx];
    else
        cert->afterDate = &cert->source[cert->srcIdx];
    startIdx = cert->srcIdx;

    b = cert->source[cert->srcIdx++];
    if (b != ASN_UTC_TIME && b != ASN_GENERALIZED_TIME)
        return ASN_TIME_E;

    if (GetLength(cert->source, &cert->srcIdx, &length, cert->maxIdx) < 0)
        return ASN_PARSE_E;

    if (length > MAX_DATE_SIZE || length < MIN_DATE_SIZE)
        return ASN_DATE_SZ_E;

    XMEMCPY(date, &cert->source[cert->srcIdx], length);
    cert->srcIdx += length;

    if (dateType == BEFORE)
        cert->beforeDateLen = cert->srcIdx - startIdx;
    else
        cert->afterDateLen  = cert->srcIdx - startIdx;

#ifndef NO_ASN_TIME
    if (!XVALIDATE_DATE(date, b, dateType)) {
        if (dateType == BEFORE)
            return ASN_BEFORE_DATE_E;
        else
            return ASN_AFTER_DATE_E;
    }
#endif

    return 0;
}

static int GetValidity(DecodedCert* cert, int verify)
{
    int length;
    int badDate = 0;

    if (GetSequence(cert->source, &cert->srcIdx, &length, cert->maxIdx) < 0)
        return ASN_PARSE_E;

    if (GetDate(cert, BEFORE) < 0 && verify != NO_VERIFY)
        badDate = ASN_BEFORE_DATE_E;           /* continue parsing */

    if (GetDate(cert, AFTER) < 0 && verify != NO_VERIFY)
        return ASN_AFTER_DATE_E;

    if (badDate != 0)
        return badDate;

    return 0;
}


int DecodeToKey(DecodedCert* cert, int verify)
{
    int badDate = 0;
    int ret;

    if ( (ret = GetCertHeader(cert)) < 0)
        return ret;

    WOLFSSL_MSG("Got Cert Header");

    if ( (ret = GetAlgoId(cert->source, &cert->srcIdx, &cert->signatureOID,
                          oidSigType, cert->maxIdx)) < 0)
        return ret;

    WOLFSSL_MSG("Got Algo ID");

    if ( (ret = GetName(cert, ISSUER)) < 0)
        return ret;

    if ( (ret = GetValidity(cert, verify)) < 0)
        badDate = ret;

    if ( (ret = GetName(cert, SUBJECT)) < 0)
        return ret;

    WOLFSSL_MSG("Got Subject Name");

    if ( (ret = GetKey(cert)) < 0)
        return ret;

    WOLFSSL_MSG("Got Key");

    if (badDate != 0)
        return badDate;

    return ret;
}

static int GetSignature(DecodedCert* cert)
{
    int length;
    int ret;
    ret = CheckBitString(cert->source, &cert->srcIdx, &length, cert->maxIdx, 1,
                         NULL);
    if (ret != 0)
        return ret;

    cert->sigLength = length;
    cert->signature = &cert->source[cert->srcIdx];
    cert->srcIdx += cert->sigLength;

    return 0;
}

static word32 SetOctetString8Bit(word32 len, byte* output)
{
    output[0] = ASN_OCTET_STRING;
    output[1] = (byte)len;
    return 2;
}

static word32 SetDigest(const byte* digest, word32 digSz, byte* output)
{
    word32 idx = SetOctetString8Bit(digSz, output);
    XMEMCPY(&output[idx], digest, digSz);

    return idx + digSz;
}


static word32 BytePrecision(word32 value)
{
    word32 i;
    for (i = sizeof(value); i; --i)
        if (value >> ((i - 1) * WOLFSSL_BIT_SIZE))
            break;

    return i;
}


WOLFSSL_LOCAL word32 SetLength(word32 length, byte* output)
{
    word32 i = 0, j;

    if (length < ASN_LONG_LENGTH)
        output[i++] = (byte)length;
    else {
        output[i++] = (byte)(BytePrecision(length) | ASN_LONG_LENGTH);

        for (j = BytePrecision(length); j; --j) {
            output[i] = (byte)(length >> ((j - 1) * WOLFSSL_BIT_SIZE));
            i++;
        }
    }

    return i;
}


WOLFSSL_LOCAL word32 SetSequence(word32 len, byte* output)
{
    output[0] = ASN_SEQUENCE | ASN_CONSTRUCTED;
    return SetLength(len, output + 1) + 1;
}

WOLFSSL_LOCAL word32 SetOctetString(word32 len, byte* output)
{
    output[0] = ASN_OCTET_STRING;
    return SetLength(len, output + 1) + 1;
}

/* Write a set header to output */
WOLFSSL_LOCAL word32 SetSet(word32 len, byte* output)
{
    output[0] = ASN_SET | ASN_CONSTRUCTED;
    return SetLength(len, output + 1) + 1;
}

WOLFSSL_LOCAL word32 SetImplicit(byte tag, byte number, word32 len, byte* output)
{

    output[0] = ((tag == ASN_SEQUENCE || tag == ASN_SET) ? ASN_CONSTRUCTED : 0)
                    | ASN_CONTEXT_SPECIFIC | number;
    return SetLength(len, output + 1) + 1;
}

WOLFSSL_LOCAL word32 SetExplicit(byte number, word32 len, byte* output)
{
    output[0] = ASN_CONSTRUCTED | ASN_CONTEXT_SPECIFIC | number;
    return SetLength(len, output + 1) + 1;
}


#if defined(HAVE_ECC) && (defined(WOLFSSL_CERT_GEN) || defined(WOLFSSL_KEY_GEN))

static int SetCurve(ecc_key* key, byte* output)
{
#ifdef HAVE_OID_ENCODING
    int ret;
#endif
    int idx = 0;
    word32 oidSz = 0;

    /* validate key */
    if (key == NULL || key->dp == NULL) {
        return BAD_FUNC_ARG;
    }

#ifdef HAVE_OID_ENCODING
    ret = EncodeObjectId(key->dp->oid, key->dp->oidSz, NULL, &oidSz);
    if (ret != 0) {
        return ret;
    }
#else
    oidSz = key->dp->oidSz;
#endif

    idx += SetObjectId(oidSz, output);

#ifdef HAVE_OID_ENCODING
    ret = EncodeObjectId(key->dp->oid, key->dp->oidSz, output+idx, &oidSz);
    if (ret != 0) {
        return ret;
    }
#else
    XMEMCPY(output+idx, key->dp->oid, oidSz);
#endif
    idx += oidSz;

    return idx;
}

#endif /* HAVE_ECC && WOLFSSL_CERT_GEN */


static INLINE int IsSigAlgoECDSA(int algoOID)
{
    /* ECDSA sigAlgo must not have ASN1 NULL parameters */
    if (algoOID == CTC_SHAwECDSA || algoOID == CTC_SHA256wECDSA ||
        algoOID == CTC_SHA384wECDSA || algoOID == CTC_SHA512wECDSA) {
        return 1;
    }

    return 0;
}

WOLFSSL_LOCAL word32 SetAlgoID(int algoOID, byte* output, int type, int curveSz)
{
    word32 tagSz, idSz, seqSz, algoSz = 0;
    const  byte* algoName = 0;
    byte   ID_Length[1 + MAX_LENGTH_SZ];
    byte   seqArray[MAX_SEQ_SZ + 1];  /* add object_id to end */

    tagSz = (type == oidHashType ||
             (type == oidSigType && !IsSigAlgoECDSA(algoOID) &&
                                         algoOID != ED25519k) ||
             (type == oidKeyType && algoOID == RSAk)) ? 2 : 0;

    algoName = OidFromId(algoOID, type, &algoSz);

    if (algoName == NULL) {
        WOLFSSL_MSG("Unknown Algorithm");
        return 0;
    }

    idSz  = SetObjectId(algoSz, ID_Length);
    seqSz = SetSequence(idSz + algoSz + tagSz + curveSz, seqArray);

    XMEMCPY(output, seqArray, seqSz);
    XMEMCPY(output + seqSz, ID_Length, idSz);
    XMEMCPY(output + seqSz + idSz, algoName, algoSz);
    if (tagSz == 2)
        SetASNNull(&output[seqSz + idSz + algoSz]);

    return seqSz + idSz + algoSz + tagSz;

}


word32 wc_EncodeSignature(byte* out, const byte* digest, word32 digSz,
                          int hashOID)
{
    byte digArray[MAX_ENCODED_DIG_SZ];
    byte algoArray[MAX_ALGO_SZ];
    byte seqArray[MAX_SEQ_SZ];
    word32 encDigSz, algoSz, seqSz;

    encDigSz = SetDigest(digest, digSz, digArray);
    algoSz   = SetAlgoID(hashOID, algoArray, oidHashType, 0);
    seqSz    = SetSequence(encDigSz + algoSz, seqArray);

    XMEMCPY(out, seqArray, seqSz);
    XMEMCPY(out + seqSz, algoArray, algoSz);
    XMEMCPY(out + seqSz + algoSz, digArray, encDigSz);

    return encDigSz + algoSz + seqSz;
}


int wc_GetCTC_HashOID(int type)
{
    switch (type) {
#ifdef WOLFSSL_MD2
        case MD2:
            return MD2h;
#endif
#ifndef NO_MD5
        case MD5:
            return MD5h;
#endif
#ifndef NO_SHA
        case SHA:
            return SHAh;
#endif
#ifdef WOLFSSL_SHA224
        case SHA224:
            return SHA224h;
#endif
#ifndef NO_SHA256
        case SHA256:
            return SHA256h;
#endif
#ifdef WOLFSSL_SHA384
        case SHA384:
            return SHA384h;
#endif
#ifdef WOLFSSL_SHA512
        case SHA512:
            return SHA512h;
#endif
        default:
            return 0;
    };
}

void InitSignatureCtx(SignatureCtx* sigCtx, void* heap, int devId)
{
    if (sigCtx) {
        XMEMSET(sigCtx, 0, sizeof(SignatureCtx));
        sigCtx->devId = devId;
        sigCtx->heap = heap;
    }
}

void FreeSignatureCtx(SignatureCtx* sigCtx)
{
    if (sigCtx == NULL)
        return;

    if (sigCtx->digest) {
        XFREE(sigCtx->digest, sigCtx->heap, DYNAMIC_TYPE_DIGEST);
        sigCtx->digest = NULL;
    }
#ifndef NO_RSA
    if (sigCtx->plain) {
        XFREE(sigCtx->plain, sigCtx->heap, DYNAMIC_TYPE_SIGNATURE);
        sigCtx->plain = NULL;
    }
#endif
    if (sigCtx->key.ptr) {
        switch (sigCtx->keyOID) {
        #ifndef NO_RSA
            case RSAk:
                wc_FreeRsaKey(sigCtx->key.rsa);
                XFREE(sigCtx->key.ptr, sigCtx->heap, DYNAMIC_TYPE_RSA);
                break;
        #endif /* !NO_RSA */
        #ifdef HAVE_ECC
            case ECDSAk:
                wc_ecc_free(sigCtx->key.ecc);
                XFREE(sigCtx->key.ecc, sigCtx->heap, DYNAMIC_TYPE_ECC);
                break;
        #endif /* HAVE_ECC */
        #ifdef HAVE_ED25519
            case ED25519k:
                wc_ed25519_free(sigCtx->key.ed25519);
                XFREE(sigCtx->key.ed25519, sigCtx->heap, DYNAMIC_TYPE_ED25519);
                break;
        #endif /* HAVE_ED25519 */
            default:
                break;
        } /* switch (keyOID) */
        sigCtx->key.ptr = NULL;
    }

    /* reset state, we are done */
    sigCtx->state = SIG_STATE_BEGIN;
}

static int HashForSignature(const byte* buf, word32 bufSz, word32 sigOID,
                            byte* digest, int* typeH, int* digestSz, int verify)
{
    int ret = 0;

    (void)verify;

    switch (sigOID) {
    #if defined(WOLFSSL_MD2)
        case CTC_MD2wRSA:
            if (!verify) {
                ret = HASH_TYPE_E;
                WOLFSSL_MSG("MD2 not supported for signing");
            }
            else if ((ret = wc_Md2Hash(buf, bufSz, digest)) == 0) {
                *typeH    = MD2h;
                *digestSz = MD2_DIGEST_SIZE;
            }
        break;
    #endif
    #ifndef NO_MD5
        case CTC_MD5wRSA:
            if ((ret = wc_Md5Hash(buf, bufSz, digest)) == 0) {
                *typeH    = MD5h;
                *digestSz = MD5_DIGEST_SIZE;
            }
            break;
    #endif
    #ifndef NO_SHA
        case CTC_SHAwRSA:
        case CTC_SHAwDSA:
        case CTC_SHAwECDSA:
            if ((ret = wc_ShaHash(buf, bufSz, digest)) == 0) {
                *typeH    = SHAh;
                *digestSz = SHA_DIGEST_SIZE;
            }
            break;
    #endif
    #ifdef WOLFSSL_SHA224
        case CTC_SHA224wRSA:
        case CTC_SHA224wECDSA:
            if ((ret = wc_Sha224Hash(buf, bufSz, digest)) == 0) {
                *typeH    = SHA224h;
                *digestSz = SHA224_DIGEST_SIZE;
            }
            break;
    #endif
    #ifndef NO_SHA256
        case CTC_SHA256wRSA:
        case CTC_SHA256wECDSA:
            if ((ret = wc_Sha256Hash(buf, bufSz, digest)) == 0) {
                *typeH    = SHA256h;
                *digestSz = SHA256_DIGEST_SIZE;
            }
            break;
    #endif
    #ifdef WOLFSSL_SHA384
        case CTC_SHA384wRSA:
        case CTC_SHA384wECDSA:
            if ((ret = wc_Sha384Hash(buf, bufSz, digest)) == 0) {
                *typeH    = SHA384h;
                *digestSz = SHA384_DIGEST_SIZE;
            }
            break;
    #endif
    #ifdef WOLFSSL_SHA512
        case CTC_SHA512wRSA:
        case CTC_SHA512wECDSA:
            if ((ret = wc_Sha512Hash(buf, bufSz, digest)) == 0) {
                *typeH    = SHA512h;
                *digestSz = SHA512_DIGEST_SIZE;
            }
            break;
    #endif
        case CTC_ED25519:
            /* Hashes done in signing operation.
             * Two dependent hashes with prefixes performed.
             */
            break;
        default:
            ret = HASH_TYPE_E;
            WOLFSSL_MSG("Hash for Signature has unsupported type");
    }

    return ret;
}

/* Return codes: 0=Success, Negative (see error-crypt.h), ASN_SIG_CONFIRM_E */
static int ConfirmSignature(SignatureCtx* sigCtx,
    const byte* buf, word32 bufSz,
    const byte* key, word32 keySz, word32 keyOID,
    const byte* sig, word32 sigSz, word32 sigOID)
{
    int ret = 0;

    if (sigCtx == NULL || buf == NULL || bufSz == 0 || key == NULL ||
        keySz == 0 || sig == NULL || sigSz == 0) {
        return BAD_FUNC_ARG;
    }

    (void)key;
    (void)keySz;
    (void)sig;
    (void)sigSz;

    WOLFSSL_ENTER("ConfirmSignature");

    switch (sigCtx->state) {
        case SIG_STATE_BEGIN:
        {
            sigCtx->digest = (byte*)XMALLOC(WC_MAX_DIGEST_SIZE, sigCtx->heap,
                                                    DYNAMIC_TYPE_DIGEST);
            if (sigCtx->digest == NULL) {
                ERROR_OUT(MEMORY_E, exit_cs);
            }

            sigCtx->state = SIG_STATE_HASH;
        } /* SIG_STATE_BEGIN */
        FALL_THROUGH;

        case SIG_STATE_HASH:
        {
            ret = HashForSignature(buf, bufSz, sigOID, sigCtx->digest,
                                   &sigCtx->typeH, &sigCtx->digestSz, 1);
            if (ret != 0) {
                goto exit_cs;
            }

            sigCtx->state = SIG_STATE_KEY;
        } /* SIG_STATE_HASH */
        FALL_THROUGH;

        case SIG_STATE_KEY:
        {
            sigCtx->keyOID = keyOID;

            switch (keyOID) {
            #ifndef NO_RSA
                case RSAk:
                {
                    word32 idx = 0;

                    sigCtx->key.rsa = (RsaKey*)XMALLOC(sizeof(RsaKey),
                                                sigCtx->heap, DYNAMIC_TYPE_RSA);
                    sigCtx->plain = (byte*)XMALLOC(MAX_ENCODED_SIG_SZ,
                                         sigCtx->heap, DYNAMIC_TYPE_SIGNATURE);
                    if (sigCtx->key.rsa == NULL || sigCtx->plain == NULL) {
                        ERROR_OUT(MEMORY_E, exit_cs);
                    }
                    if ((ret = wc_InitRsaKey_ex(sigCtx->key.rsa, sigCtx->heap,
                                                        sigCtx->devId)) != 0) {
                        goto exit_cs;
                    }
                    if (sigSz > MAX_ENCODED_SIG_SZ) {
                        WOLFSSL_MSG("Verify Signature is too big");
                        ERROR_OUT(BUFFER_E, exit_cs);
                    }
                    if ((ret = wc_RsaPublicKeyDecode(key, &idx, sigCtx->key.rsa,
                                                                 keySz)) != 0) {
                        WOLFSSL_MSG("ASN Key decode error RSA");
                        goto exit_cs;
                    }
                    XMEMCPY(sigCtx->plain, sig, sigSz);
                    sigCtx->out = NULL;

                #ifdef WOLFSSL_ASYNC_CRYPT
                    sigCtx->asyncDev = &sigCtx->key.rsa->asyncDev;
                #endif
                    break;
                }
            #endif /* !NO_RSA */
            #ifdef HAVE_ECC
                case ECDSAk:
                {
                    sigCtx->verify = 0;
                    sigCtx->key.ecc = (ecc_key*)XMALLOC(sizeof(ecc_key),
                                                sigCtx->heap, DYNAMIC_TYPE_ECC);
                    if (sigCtx->key.ecc == NULL) {
                        ERROR_OUT(MEMORY_E, exit_cs);
                    }
                    if ((ret = wc_ecc_init_ex(sigCtx->key.ecc, sigCtx->heap,
                                                          sigCtx->devId)) < 0) {
                        goto exit_cs;
                    }
                    if ((ret = wc_ecc_import_x963(key, keySz,
                                                        sigCtx->key.ecc)) < 0) {
                        WOLFSSL_MSG("ASN Key import error ECC");
                        goto exit_cs;
                    }
                #ifdef WOLFSSL_ASYNC_CRYPT
                    sigCtx->asyncDev = &sigCtx->key.ecc->asyncDev;
                #endif
                    break;
                }
            #endif /* HAVE_ECC */
            #ifdef HAVE_ED25519
                case ED25519k:
                {
                    sigCtx->verify = 0;
                    sigCtx->key.ed25519 = (ed25519_key*)XMALLOC(
                                              sizeof(ed25519_key), sigCtx->heap,
                                              DYNAMIC_TYPE_ED25519);
                    if (sigCtx->key.ed25519 == NULL) {
                        ERROR_OUT(MEMORY_E, exit_cs);
                    }
                    if ((ret = wc_ed25519_init(sigCtx->key.ed25519)) < 0) {
                        goto exit_cs;
                    }
                    if ((ret = wc_ed25519_import_public(key, keySz,
                                                    sigCtx->key.ed25519)) < 0) {
                        WOLFSSL_MSG("ASN Key import error ED25519");
                        goto exit_cs;
                    }
                #ifdef WOLFSSL_ASYNC_CRYPT
                    sigCtx->asyncDev = &sigCtx->key.ed25519->asyncDev;
                #endif
                    break;
                }
            #endif
                default:
                    WOLFSSL_MSG("Verify Key type unknown");
                    ret = ASN_UNKNOWN_OID_E;
                    break;
            } /* switch (keyOID) */

            if (ret != 0) {
                goto exit_cs;
            }

            sigCtx->state = SIG_STATE_DO;

        #ifdef WOLFSSL_ASYNC_CRYPT
            if (sigCtx->devId != INVALID_DEVID && sigCtx->asyncDev && sigCtx->asyncCtx) {
                /* make sure event is intialized */
                WOLF_EVENT* event = &sigCtx->asyncDev->event;
                ret = wolfAsync_EventInit(event, WOLF_EVENT_TYPE_ASYNC_WOLFSSL,
                    sigCtx->asyncCtx, WC_ASYNC_FLAG_CALL_AGAIN);
            }
        #endif
        } /* SIG_STATE_KEY */
        FALL_THROUGH;

        case SIG_STATE_DO:
        {
            switch (keyOID) {
            #ifndef NO_RSA
                case RSAk:
                {
                    ret = wc_RsaSSL_VerifyInline(sigCtx->plain, sigSz,
                                                &sigCtx->out, sigCtx->key.rsa);
                    break;
                }
            #endif /* !NO_RSA */
            #ifdef HAVE_ECC
                case ECDSAk:
                {
                    ret = wc_ecc_verify_hash(sig, sigSz, sigCtx->digest,
                        sigCtx->digestSz, &sigCtx->verify, sigCtx->key.ecc);
                    break;
                }
            #endif /* HAVE_ECC */
            #ifdef HAVE_ED25519
                case ED25519k:
                {
                    ret = wc_ed25519_verify_msg(sig, sigSz, buf, bufSz,
                                          &sigCtx->verify, sigCtx->key.ed25519);
                    break;
                }
            #endif
                default:
                    break;
            }  /* switch (keyOID) */

            if (ret < 0) {
                /* treat all non async RSA errors as ASN_SIG_CONFIRM_E */
                if (ret != WC_PENDING_E)
                    ret = ASN_SIG_CONFIRM_E;
                goto exit_cs;
            }

            sigCtx->state = SIG_STATE_CHECK;
        } /* SIG_STATE_DO */
        FALL_THROUGH;

        case SIG_STATE_CHECK:
        {
            switch (keyOID) {
            #ifndef NO_RSA
                case RSAk:
                {
                    int encodedSigSz, verifySz;
                #ifdef WOLFSSL_SMALL_STACK
                    byte* encodedSig = (byte*)XMALLOC(MAX_ENCODED_SIG_SZ,
                                        sigCtx->heap, DYNAMIC_TYPE_TMP_BUFFER);
                    if (encodedSig == NULL) {
                        ERROR_OUT(MEMORY_E, exit_cs);
                    }
                #else
                    byte encodedSig[MAX_ENCODED_SIG_SZ];
                #endif

                    verifySz = ret;

                    /* make sure we're right justified */
                    encodedSigSz = wc_EncodeSignature(encodedSig,
                            sigCtx->digest, sigCtx->digestSz, sigCtx->typeH);
                    if (encodedSigSz == verifySz &&
                        XMEMCMP(sigCtx->out, encodedSig, encodedSigSz) == 0) {
                        ret = 0;
                    }
                    else {
                        WOLFSSL_MSG("RSA SSL verify match encode error");
                        ret = ASN_SIG_CONFIRM_E;
                    }

                #ifdef WOLFSSL_SMALL_STACK
                    XFREE(encodedSig, sigCtx->heap, DYNAMIC_TYPE_TMP_BUFFER);
                #endif
                    break;
                }
            #endif /* NO_RSA */
            #ifdef HAVE_ECC
                case ECDSAk:
                {
                    if (sigCtx->verify == 1) {
                        ret = 0;
                    }
                    else {
                        WOLFSSL_MSG("ECC Verify didn't match");
                        ret = ASN_SIG_CONFIRM_E;
                    }
                    break;
                }
            #endif /* HAVE_ECC */
            #ifdef HAVE_ED25519
                case ED25519k:
                {
                    if (sigCtx->verify == 1) {
                        ret = 0;
                    }
                    else {
                        WOLFSSL_MSG("ED25519 Verify didn't match");
                        ret = ASN_SIG_CONFIRM_E;
                    }
                    break;
                }
            #endif /* HAVE_ED25519 */
                default:
                    break;
            }  /* switch (keyOID) */

            break;
        } /* SIG_STATE_CHECK */
    } /* switch (sigCtx->state) */

exit_cs:

    WOLFSSL_LEAVE("ConfirmSignature", ret);

    if (ret != WC_PENDING_E) {
        FreeSignatureCtx(sigCtx);
    }

    return ret;
}


#ifndef IGNORE_NAME_CONSTRAINTS

static int MatchBaseName(int type, const char* name, int nameSz,
                         const char* base, int baseSz)
{
    if (base == NULL || baseSz <= 0 || name == NULL || nameSz <= 0 ||
            name[0] == '.' || nameSz < baseSz ||
            (type != ASN_RFC822_TYPE && type != ASN_DNS_TYPE))
        return 0;

    /* If an email type, handle special cases where the base is only
     * a domain, or is an email address itself. */
    if (type == ASN_RFC822_TYPE) {
        const char* p = NULL;
        int count = 0;

        if (base[0] != '.') {
            p = base;
            count = 0;

            /* find the '@' in the base */
            while (*p != '@' && count < baseSz) {
                count++;
                p++;
            }

            /* No '@' in base, reset p to NULL */
            if (count >= baseSz)
                p = NULL;
        }

        if (p == NULL) {
            /* Base isn't an email address, it is a domain name,
             * wind the name forward one character past its '@'. */
            p = name;
            count = 0;
            while (*p != '@' && count < baseSz) {
                count++;
                p++;
            }

            if (count < baseSz && *p == '@') {
                name = p + 1;
                nameSz -= count + 1;
            }
        }
    }

    if ((type == ASN_DNS_TYPE || type == ASN_RFC822_TYPE) && base[0] == '.') {
        int szAdjust = nameSz - baseSz;
        name += szAdjust;
        nameSz -= szAdjust;
    }

    while (nameSz > 0) {
        if (XTOLOWER((unsigned char)*name++) !=
                                               XTOLOWER((unsigned char)*base++))
            return 0;
        nameSz--;
    }

    return 1;
}


static int ConfirmNameConstraints(Signer* signer, DecodedCert* cert)
{
    if (signer == NULL || cert == NULL)
        return 0;

    /* Check against the excluded list */
    if (signer->excludedNames) {
        Base_entry* base = signer->excludedNames;

        while (base != NULL) {
            switch (base->type) {
                case ASN_DNS_TYPE:
                {
                    DNS_entry* name = cert->altNames;
                    while (name != NULL) {
                        if (MatchBaseName(ASN_DNS_TYPE,
                                          name->name, (int)XSTRLEN(name->name),
                                          base->name, base->nameSz)) {
                            return 0;
                        }
                        name = name->next;
                    }
                    break;
                }
                case ASN_RFC822_TYPE:
                {
                    DNS_entry* name = cert->altEmailNames;
                    while (name != NULL) {
                        if (MatchBaseName(ASN_RFC822_TYPE,
                                          name->name, (int)XSTRLEN(name->name),
                                          base->name, base->nameSz)) {
                            return 0;
                        }
                        name = name->next;
                    }
                    break;
                }
                case ASN_DIR_TYPE:
                {
                    /* allow permitted dirName smaller than actual subject */
                    if (cert->subjectRawLen >= base->nameSz &&
                        XMEMCMP(cert->subjectRaw, base->name,
                                                        base->nameSz) == 0) {
                        return 0;
                    }
                    break;
                }
            }; /* switch */
            base = base->next;
        }
    }

    /* Check against the permitted list */
    if (signer->permittedNames != NULL) {
        int needDns = 0;
        int matchDns = 0;
        int needEmail = 0;
        int matchEmail = 0;
        int needDir = 0;
        int matchDir = 0;
        Base_entry* base = signer->permittedNames;

        while (base != NULL) {
            switch (base->type) {
                case ASN_DNS_TYPE:
                {
                    DNS_entry* name = cert->altNames;

                    if (name != NULL)
                        needDns = 1;

                    while (name != NULL) {
                        matchDns = MatchBaseName(ASN_DNS_TYPE,
                                          name->name, (int)XSTRLEN(name->name),
                                          base->name, base->nameSz);
                        name = name->next;
                    }
                    break;
                }
                case ASN_RFC822_TYPE:
                {
                    DNS_entry* name = cert->altEmailNames;

                    if (name != NULL)
                        needEmail = 1;

                    while (name != NULL) {
                        matchEmail = MatchBaseName(ASN_DNS_TYPE,
                                          name->name, (int)XSTRLEN(name->name),
                                          base->name, base->nameSz);
                        name = name->next;
                    }
                    break;
                }
                case ASN_DIR_TYPE:
                {
                    /* allow permitted dirName smaller than actual subject */
                    needDir = 1;
                    if (cert->subjectRaw != NULL &&
                        cert->subjectRawLen >= base->nameSz &&
                        XMEMCMP(cert->subjectRaw, base->name,
                                                        base->nameSz) == 0) {
                        matchDir = 1;
                    }
                    break;
                }
            } /* switch */
            base = base->next;
        }

        if ((needDns   && !matchDns) ||
            (needEmail && !matchEmail) ||
            (needDir   && !matchDir)) {
            return 0;
        }
    }

    return 1;
}

#endif /* IGNORE_NAME_CONSTRAINTS */

static int DecodeAltNames(byte* input, int sz, DecodedCert* cert)
{
    word32 idx = 0;
    int length = 0;

    WOLFSSL_ENTER("DecodeAltNames");

    if (GetSequence(input, &idx, &length, sz) < 0) {
        WOLFSSL_MSG("\tBad Sequence");
        return ASN_PARSE_E;
    }

    cert->weOwnAltNames = 1;

    while (length > 0) {
        byte b = input[idx++];

        length--;

        /* Save DNS Type names in the altNames list. */
        /* Save Other Type names in the cert's OidMap */
        if (b == (ASN_CONTEXT_SPECIFIC | ASN_DNS_TYPE)) {
            DNS_entry* dnsEntry;
            int strLen;
            word32 lenStartIdx = idx;

            if (GetLength(input, &idx, &strLen, sz) < 0) {
                WOLFSSL_MSG("\tfail: str length");
                return ASN_PARSE_E;
            }
            length -= (idx - lenStartIdx);

            dnsEntry = (DNS_entry*)XMALLOC(sizeof(DNS_entry), cert->heap,
                                        DYNAMIC_TYPE_ALTNAME);
            if (dnsEntry == NULL) {
                WOLFSSL_MSG("\tOut of Memory");
                return MEMORY_E;
            }

            dnsEntry->name = (char*)XMALLOC(strLen + 1, cert->heap,
                                         DYNAMIC_TYPE_ALTNAME);
            if (dnsEntry->name == NULL) {
                WOLFSSL_MSG("\tOut of Memory");
                XFREE(dnsEntry, cert->heap, DYNAMIC_TYPE_ALTNAME);
                return MEMORY_E;
            }

            XMEMCPY(dnsEntry->name, &input[idx], strLen);
            dnsEntry->name[strLen] = '\0';

            dnsEntry->next = cert->altNames;
            cert->altNames = dnsEntry;

            length -= strLen;
            idx    += strLen;
        }
    #ifndef IGNORE_NAME_CONSTRAINTS
        else if (b == (ASN_CONTEXT_SPECIFIC | ASN_RFC822_TYPE)) {
            DNS_entry* emailEntry;
            int strLen;
            word32 lenStartIdx = idx;

            if (GetLength(input, &idx, &strLen, sz) < 0) {
                WOLFSSL_MSG("\tfail: str length");
                return ASN_PARSE_E;
            }
            length -= (idx - lenStartIdx);

            emailEntry = (DNS_entry*)XMALLOC(sizeof(DNS_entry), cert->heap,
                                        DYNAMIC_TYPE_ALTNAME);
            if (emailEntry == NULL) {
                WOLFSSL_MSG("\tOut of Memory");
                return MEMORY_E;
            }

            emailEntry->name = (char*)XMALLOC(strLen + 1, cert->heap,
                                         DYNAMIC_TYPE_ALTNAME);
            if (emailEntry->name == NULL) {
                WOLFSSL_MSG("\tOut of Memory");
                XFREE(emailEntry, cert->heap, DYNAMIC_TYPE_ALTNAME);
                return MEMORY_E;
            }

            XMEMCPY(emailEntry->name, &input[idx], strLen);
            emailEntry->name[strLen] = '\0';

            emailEntry->next = cert->altEmailNames;
            cert->altEmailNames = emailEntry;

            length -= strLen;
            idx    += strLen;
        }
    #endif /* IGNORE_NAME_CONSTRAINTS */
    #ifdef WOLFSSL_SEP
        else if (b == (ASN_CONTEXT_SPECIFIC | ASN_CONSTRUCTED | ASN_OTHER_TYPE))
        {
            int strLen;
            word32 lenStartIdx = idx;
            word32 oid = 0;
            int    ret;

            if (GetLength(input, &idx, &strLen, sz) < 0) {
                WOLFSSL_MSG("\tfail: other name length");
                return ASN_PARSE_E;
            }
            /* Consume the rest of this sequence. */
            length -= (strLen + idx - lenStartIdx);

            if (GetObjectId(input, &idx, &oid, oidCertAltNameType, sz) < 0) {
                WOLFSSL_MSG("\tbad OID");
                return ASN_PARSE_E;
            }

            if (oid != HW_NAME_OID) {
                WOLFSSL_MSG("\tincorrect OID");
                return ASN_PARSE_E;
            }

            if (input[idx++] != (ASN_CONTEXT_SPECIFIC | ASN_CONSTRUCTED)) {
                WOLFSSL_MSG("\twrong type");
                return ASN_PARSE_E;
            }

            if (GetLength(input, &idx, &strLen, sz) < 0) {
                WOLFSSL_MSG("\tfail: str len");
                return ASN_PARSE_E;
            }

            if (GetSequence(input, &idx, &strLen, sz) < 0) {
                WOLFSSL_MSG("\tBad Sequence");
                return ASN_PARSE_E;
            }

            ret = GetASNObjectId(input, &idx, &strLen, sz);
            if (ret != 0) {
                WOLFSSL_MSG("\tbad OID");
                return ret;
            }

            cert->hwType = (byte*)XMALLOC(strLen, cert->heap,
                                          DYNAMIC_TYPE_X509_EXT);
            if (cert->hwType == NULL) {
                WOLFSSL_MSG("\tOut of Memory");
                return MEMORY_E;
            }

            XMEMCPY(cert->hwType, &input[idx], strLen);
            cert->hwTypeSz = strLen;
            idx += strLen;

            ret = GetOctetString(input, &idx, &strLen, sz);
            if (ret < 0)
                return ret;

            cert->hwSerialNum = (byte*)XMALLOC(strLen + 1, cert->heap,
                                               DYNAMIC_TYPE_X509_EXT);
            if (cert->hwSerialNum == NULL) {
                WOLFSSL_MSG("\tOut of Memory");
                return MEMORY_E;
            }

            XMEMCPY(cert->hwSerialNum, &input[idx], strLen);
            cert->hwSerialNum[strLen] = '\0';
            cert->hwSerialNumSz = strLen;
            idx += strLen;
        }
    #endif /* WOLFSSL_SEP */
        else {
            int strLen;
            word32 lenStartIdx = idx;

            WOLFSSL_MSG("\tUnsupported name type, skipping");

            if (GetLength(input, &idx, &strLen, sz) < 0) {
                WOLFSSL_MSG("\tfail: unsupported name length");
                return ASN_PARSE_E;
            }
            length -= (strLen + idx - lenStartIdx);
            idx += strLen;
        }
    }
    return 0;
}

static int DecodeBasicCaConstraint(byte* input, int sz, DecodedCert* cert)
{
    word32 idx = 0;
    int length = 0;
    int ret;

    WOLFSSL_ENTER("DecodeBasicCaConstraint");

    if (GetSequence(input, &idx, &length, sz) < 0) {
        WOLFSSL_MSG("\tfail: bad SEQUENCE");
        return ASN_PARSE_E;
    }

    if (length == 0)
        return 0;

    /* If the basic ca constraint is false, this extension may be named, but
     * left empty. So, if the length is 0, just return. */

    ret = GetBoolean(input, &idx, sz);
    if (ret < 0) {
        WOLFSSL_MSG("\tfail: constraint not valid BOOLEAN");
        return ret;
    }

    cert->isCA = (byte)ret;

    /* If there isn't any more data, return. */
    if (idx >= (word32)sz)
        return 0;

    ret = GetInteger7Bit(input, &idx, sz);
    if (ret < 0)
        return ret;

    cert->pathLength = (byte)ret;
    cert->pathLengthSet = 1;

    return 0;
}


#define CRLDP_FULL_NAME 0
    /* From RFC3280 SS4.2.1.14, Distribution Point Name*/
#define GENERALNAME_URI 6
    /* From RFC3280 SS4.2.1.7, GeneralName */

static int DecodeCrlDist(byte* input, int sz, DecodedCert* cert)
{
    word32 idx = 0;
    int length = 0;

    WOLFSSL_ENTER("DecodeCrlDist");

    /* Unwrap the list of Distribution Points*/
    if (GetSequence(input, &idx, &length, sz) < 0)
        return ASN_PARSE_E;

    /* Unwrap a single Distribution Point */
    if (GetSequence(input, &idx, &length, sz) < 0)
        return ASN_PARSE_E;

    /* The Distribution Point has three explicit optional members
     *  First check for a DistributionPointName
     */
    if (input[idx] == (ASN_CONSTRUCTED | ASN_CONTEXT_SPECIFIC | 0))
    {
        idx++;
        if (GetLength(input, &idx, &length, sz) < 0)
            return ASN_PARSE_E;

        if (input[idx] ==
                    (ASN_CONTEXT_SPECIFIC | ASN_CONSTRUCTED | CRLDP_FULL_NAME))
        {
            idx++;
            if (GetLength(input, &idx, &length, sz) < 0)
                return ASN_PARSE_E;

            if (input[idx] == (ASN_CONTEXT_SPECIFIC | GENERALNAME_URI))
            {
                idx++;
                if (GetLength(input, &idx, &length, sz) < 0)
                    return ASN_PARSE_E;

                cert->extCrlInfoSz = length;
                cert->extCrlInfo = input + idx;
                idx += length;
            }
            else
                /* This isn't a URI, skip it. */
                idx += length;
        }
        else {
            /* This isn't a FULLNAME, skip it. */
            idx += length;
        }
    }

    /* Check for reasonFlags */
    if (idx < (word32)sz &&
        input[idx] == (ASN_CONSTRUCTED | ASN_CONTEXT_SPECIFIC | 1))
    {
        idx++;
        if (GetLength(input, &idx, &length, sz) < 0)
            return ASN_PARSE_E;
        idx += length;
    }

    /* Check for cRLIssuer */
    if (idx < (word32)sz &&
        input[idx] == (ASN_CONSTRUCTED | ASN_CONTEXT_SPECIFIC | 2))
    {
        idx++;
        if (GetLength(input, &idx, &length, sz) < 0)
            return ASN_PARSE_E;
        idx += length;
    }

    if (idx < (word32)sz)
    {
        WOLFSSL_MSG("\tThere are more CRL Distribution Point records, "
                   "but we only use the first one.");
    }

    return 0;
}


static int DecodeAuthInfo(byte* input, int sz, DecodedCert* cert)
/*
 *  Read the first of the Authority Information Access records. If there are
 *  any issues, return without saving the record.
 */
{
    word32 idx = 0;
    int length = 0;
    byte b;
    word32 oid;

    WOLFSSL_ENTER("DecodeAuthInfo");

    /* Unwrap the list of AIAs */
    if (GetSequence(input, &idx, &length, sz) < 0)
        return ASN_PARSE_E;

    while (idx < (word32)sz) {
        /* Unwrap a single AIA */
        if (GetSequence(input, &idx, &length, sz) < 0)
            return ASN_PARSE_E;

        oid = 0;
        if (GetObjectId(input, &idx, &oid, oidCertAuthInfoType, sz) < 0)
            return ASN_PARSE_E;


        /* Only supporting URIs right now. */
        b = input[idx++];
        if (GetLength(input, &idx, &length, sz) < 0)
            return ASN_PARSE_E;

        if (b == (ASN_CONTEXT_SPECIFIC | GENERALNAME_URI) &&
            oid == AIA_OCSP_OID)
        {
            cert->extAuthInfoSz = length;
            cert->extAuthInfo = input + idx;
            break;
        }
        idx += length;
    }

    return 0;
}


static int DecodeAuthKeyId(byte* input, int sz, DecodedCert* cert)
{
    word32 idx = 0;
    int length = 0, ret = 0;

    WOLFSSL_ENTER("DecodeAuthKeyId");

    if (GetSequence(input, &idx, &length, sz) < 0) {
        WOLFSSL_MSG("\tfail: should be a SEQUENCE\n");
        return ASN_PARSE_E;
    }

    if (input[idx++] != (ASN_CONTEXT_SPECIFIC | 0)) {
        WOLFSSL_MSG("\tinfo: OPTIONAL item 0, not available\n");
        return 0;
    }

    if (GetLength(input, &idx, &length, sz) <= 0) {
        WOLFSSL_MSG("\tfail: extension data length");
        return ASN_PARSE_E;
    }

#ifdef OPENSSL_EXTRA
    cert->extAuthKeyIdSrc = &input[idx];
    cert->extAuthKeyIdSz = length;
#endif /* OPENSSL_EXTRA */

    if (length == KEYID_SIZE) {
        XMEMCPY(cert->extAuthKeyId, input + idx, length);
    }
    else {
    #ifdef NO_SHA
        ret = wc_Sha256Hash(input + idx, length, cert->extAuthKeyId);
    #else
        ret = wc_ShaHash(input + idx, length, cert->extAuthKeyId);
    #endif
    }

    return ret;
}


static int DecodeSubjKeyId(byte* input, int sz, DecodedCert* cert)
{
    word32 idx = 0;
    int length = 0, ret = 0;

    WOLFSSL_ENTER("DecodeSubjKeyId");

    if (sz <= 0)
        return ASN_PARSE_E;

    ret = GetOctetString(input, &idx, &length, sz);
    if (ret < 0)
        return ret;

    #ifdef OPENSSL_EXTRA
        cert->extSubjKeyIdSrc = &input[idx];
        cert->extSubjKeyIdSz = length;
    #endif /* OPENSSL_EXTRA */

    if (length == SIGNER_DIGEST_SIZE) {
        XMEMCPY(cert->extSubjKeyId, input + idx, length);
    }
    else {
    #ifdef NO_SHA
        ret = wc_Sha256Hash(input + idx, length, cert->extSubjKeyId);
    #else
        ret = wc_ShaHash(input + idx, length, cert->extSubjKeyId);
    #endif
    }

    return ret;
}


static int DecodeKeyUsage(byte* input, int sz, DecodedCert* cert)
{
    word32 idx = 0;
    int length;
    int ret;
    WOLFSSL_ENTER("DecodeKeyUsage");

    ret = CheckBitString(input, &idx, &length, sz, 0, NULL);
    if (ret != 0)
        return ret;

    cert->extKeyUsage = (word16)(input[idx]);
    if (length == 2)
        cert->extKeyUsage |= (word16)(input[idx+1] << 8);

    return 0;
}


static int DecodeExtKeyUsage(byte* input, int sz, DecodedCert* cert)
{
    word32 idx = 0, oid;
    int length;

    WOLFSSL_ENTER("DecodeExtKeyUsage");

    if (GetSequence(input, &idx, &length, sz) < 0) {
        WOLFSSL_MSG("\tfail: should be a SEQUENCE");
        return ASN_PARSE_E;
    }

#ifdef OPENSSL_EXTRA
    cert->extExtKeyUsageSrc = input + idx;
    cert->extExtKeyUsageSz = length;
#endif

    while (idx < (word32)sz) {
        if (GetObjectId(input, &idx, &oid, oidCertKeyUseType, sz) < 0)
            return ASN_PARSE_E;

        switch (oid) {
            case EKU_ANY_OID:
                cert->extExtKeyUsage |= EXTKEYUSE_ANY;
                break;
            case EKU_SERVER_AUTH_OID:
                cert->extExtKeyUsage |= EXTKEYUSE_SERVER_AUTH;
                break;
            case EKU_CLIENT_AUTH_OID:
                cert->extExtKeyUsage |= EXTKEYUSE_CLIENT_AUTH;
                break;
            case EKU_CODESIGNING_OID:
                cert->extExtKeyUsage |= EXTKEYUSE_CODESIGN;
                break;
            case EKU_EMAILPROTECT_OID:
                cert->extExtKeyUsage |= EXTKEYUSE_EMAILPROT;
                break;
            case EKU_TIMESTAMP_OID:
                cert->extExtKeyUsage |= EXTKEYUSE_TIMESTAMP;
                break;
            case EKU_OCSP_SIGN_OID:
                cert->extExtKeyUsage |= EXTKEYUSE_OCSP_SIGN;
                break;
        }

    #ifdef OPENSSL_EXTRA
        cert->extExtKeyUsageCount++;
    #endif
    }

    return 0;
}


#ifndef IGNORE_NAME_CONSTRAINTS
#define ASN_TYPE_MASK 0xF
static int DecodeSubtree(byte* input, int sz, Base_entry** head, void* heap)
{
    word32 idx = 0;

    (void)heap;

    while (idx < (word32)sz) {
        int seqLength, strLength;
        word32 nameIdx;
        byte b, bType;

        if (GetSequence(input, &idx, &seqLength, sz) < 0) {
            WOLFSSL_MSG("\tfail: should be a SEQUENCE");
            return ASN_PARSE_E;
        }
        nameIdx = idx;
        b = input[nameIdx++];

        if (GetLength(input, &nameIdx, &strLength, sz) <= 0) {
            WOLFSSL_MSG("\tinvalid length");
            return ASN_PARSE_E;
        }

        /* Get type, LSB 4-bits */
        bType = (b & ASN_TYPE_MASK);

        if (bType == ASN_DNS_TYPE || bType == ASN_RFC822_TYPE ||
                                                        bType == ASN_DIR_TYPE) {
            Base_entry* entry;

            /* if constructed has leading sequence */
            if (b & ASN_CONSTRUCTED) {
                if (GetSequence(input, &nameIdx, &strLength, sz) < 0) {
                    WOLFSSL_MSG("\tfail: constructed be a SEQUENCE");
                    return ASN_PARSE_E;
                }
            }

            entry = (Base_entry*)XMALLOC(sizeof(Base_entry), heap,
                                                          DYNAMIC_TYPE_ALTNAME);
            if (entry == NULL) {
                WOLFSSL_MSG("allocate error");
                return MEMORY_E;
            }

            entry->name = (char*)XMALLOC(strLength, heap, DYNAMIC_TYPE_ALTNAME);
            if (entry->name == NULL) {
                WOLFSSL_MSG("allocate error");
                XFREE(entry, heap, DYNAMIC_TYPE_ALTNAME);
                return MEMORY_E;
            }

            XMEMCPY(entry->name, &input[nameIdx], strLength);
            entry->nameSz = strLength;
            entry->type = bType;

            entry->next = *head;
            *head = entry;
        }

        idx += seqLength;
    }

    return 0;
}


static int DecodeNameConstraints(byte* input, int sz, DecodedCert* cert)
{
    word32 idx = 0;
    int length = 0;

    WOLFSSL_ENTER("DecodeNameConstraints");

    if (GetSequence(input, &idx, &length, sz) < 0) {
        WOLFSSL_MSG("\tfail: should be a SEQUENCE");
        return ASN_PARSE_E;
    }

    while (idx < (word32)sz) {
        byte b = input[idx++];
        Base_entry** subtree = NULL;

        if (GetLength(input, &idx, &length, sz) <= 0) {
            WOLFSSL_MSG("\tinvalid length");
            return ASN_PARSE_E;
        }

        if (b == (ASN_CONTEXT_SPECIFIC | ASN_CONSTRUCTED | 0))
            subtree = &cert->permittedNames;
        else if (b == (ASN_CONTEXT_SPECIFIC | ASN_CONSTRUCTED | 1))
            subtree = &cert->excludedNames;
        else {
            WOLFSSL_MSG("\tinvalid subtree");
            return ASN_PARSE_E;
        }

        DecodeSubtree(input + idx, length, subtree, cert->heap);

        idx += length;
    }

    return 0;
}
#endif /* IGNORE_NAME_CONSTRAINTS */


#if defined(WOLFSSL_CERT_EXT) && !defined(WOLFSSL_SEP)

static int Word32ToString(char* d, word32 number)
{
    int i = 0;

    if (d != NULL) {
        word32 order = 1000000000;
        word32 digit;

        if (number == 0) {
            d[i++] = '0';
        }
        else {
            while (order) {
                digit = number / order;
                if (i > 0 || digit != 0) {
                    d[i++] = (char)digit + '0';
                }
                if (digit != 0)
                    number %= digit * order;
                if (order > 1)
                    order /= 10;
                else
                    order = 0;
            }
        }
        d[i] = 0;
    }

    return i;
}


/* Decode ITU-T X.690 OID format to a string representation
 * return string length */
static int DecodePolicyOID(char *out, word32 outSz, byte *in, word32 inSz)
{
    word32 val, idx = 0, nb_bytes;
    size_t w_bytes = 0;

    if (out == NULL || in == NULL || outSz < 4 || inSz < 2)
        return BAD_FUNC_ARG;

    /* first two byte must be interpreted as : 40 * int1 + int2 */
    val = (word16)in[idx++];

    w_bytes = Word32ToString(out, val / 40);
    out[w_bytes++] = '.';
    w_bytes += Word32ToString(out+w_bytes, val % 40);

    while (idx < inSz) {
        /* init value */
        val = 0;
        nb_bytes = 0;

        /* check that output size is ok */
        if (w_bytes > (outSz - 3))
            return BUFFER_E;

        /* first bit is used to set if value is coded on 1 or multiple bytes */
        while ((in[idx+nb_bytes] & 0x80))
            nb_bytes++;

        if (!nb_bytes)
            val = (word32)(in[idx++] & 0x7f);
        else {
            word32 base = 1, tmp = nb_bytes;

            while (tmp != 0) {
                val += (word32)(in[idx+tmp] & 0x7f) * base;
                base *= 128;
                tmp--;
            }
            val += (word32)(in[idx++] & 0x7f) * base;

            idx += nb_bytes;
        }

        out[w_bytes++] = '.';
        w_bytes += Word32ToString(out+w_bytes, val);
    }

    return 0;
}
#endif /* WOLFSSL_CERT_EXT && !WOLFSSL_SEP */

#if defined(WOLFSSL_SEP) || defined(WOLFSSL_CERT_EXT)
    /* Reference: https://tools.ietf.org/html/rfc5280#section-4.2.1.4 */
    static int DecodeCertPolicy(byte* input, int sz, DecodedCert* cert)
    {
        word32 idx = 0;
        word32 oldIdx;
        int ret;
        int total_length = 0, policy_length = 0, length = 0;
    #if !defined(WOLFSSL_SEP) && defined(WOLFSSL_CERT_EXT) && \
        !defined(WOLFSSL_DUP_CERTPOL)
        int i;
    #endif

        WOLFSSL_ENTER("DecodeCertPolicy");

        if (GetSequence(input, &idx, &total_length, sz) < 0) {
            WOLFSSL_MSG("\tGet CertPolicy total seq failed");
            return ASN_PARSE_E;
        }

        /* Validate total length */
        if (total_length > (sz - (int)idx)) {
            WOLFSSL_MSG("\tCertPolicy length mismatch");
            return ASN_PARSE_E;
        }

        /* Unwrap certificatePolicies */
        do {
            if (GetSequence(input, &idx, &policy_length, sz) < 0) {
                WOLFSSL_MSG("\tGet CertPolicy seq failed");
                return ASN_PARSE_E;
            }

            oldIdx = idx;
            ret = GetASNObjectId(input, &idx, &length, sz);
            if (ret != 0)
                return ret;
            policy_length -= idx - oldIdx;

            if (length > 0) {
                /* Verify length won't overrun buffer */
                if (length > (sz - (int)idx)) {
                    WOLFSSL_MSG("\tCertPolicy length exceeds input buffer");
                    return ASN_PARSE_E;
                }

        #if defined(WOLFSSL_SEP)
                cert->deviceType = (byte*)XMALLOC(length, cert->heap,
                                                  DYNAMIC_TYPE_X509_EXT);
                if (cert->deviceType == NULL) {
                    WOLFSSL_MSG("\tCouldn't alloc memory for deviceType");
                    return MEMORY_E;
                }
                cert->deviceTypeSz = length;
                XMEMCPY(cert->deviceType, input + idx, length);
                break;
        #elif defined(WOLFSSL_CERT_EXT)
                /* decode cert policy */
                if (DecodePolicyOID(cert->extCertPolicies[cert->extCertPoliciesNb],
                                    MAX_CERTPOL_SZ, input + idx, length) != 0) {
                    WOLFSSL_MSG("\tCouldn't decode CertPolicy");
                    return ASN_PARSE_E;
                }
            #ifndef WOLFSSL_DUP_CERTPOL
                /* From RFC 5280 section 4.2.1.3 "A certificate policy OID MUST
                 * NOT appear more than once in a certificate policies
                 * extension". This is a sanity check for duplicates.
                 * extCertPolicies should only have OID values, additional
                 * qualifiers need to be stored in a seperate array. */
                for (i = 0; i < cert->extCertPoliciesNb; i++) {
                    if (XMEMCMP(cert->extCertPolicies[i],
                            cert->extCertPolicies[cert->extCertPoliciesNb],
                            MAX_CERTPOL_SZ) == 0) {
                            WOLFSSL_MSG("Duplicate policy OIDs not allowed");
                            WOLFSSL_MSG("Use WOLFSSL_DUP_CERTPOL if wanted");
                            return CERTPOLICIES_E;
                    }
                }
            #endif /* !WOLFSSL_DUP_CERTPOL */
                cert->extCertPoliciesNb++;
        #else
                WOLFSSL_LEAVE("DecodeCertPolicy : unsupported mode", 0);
                return 0;
        #endif
            }
            idx += policy_length;
        } while((int)idx < total_length
    #if defined(WOLFSSL_CERT_EXT)
            && cert->extCertPoliciesNb < MAX_CERTPOL_NB
    #endif
        );

        WOLFSSL_LEAVE("DecodeCertPolicy", 0);
        return 0;
    }
#endif /* WOLFSSL_SEP */

static int DecodeCertExtensions(DecodedCert* cert)
/*
 *  Processing the Certificate Extensions. This does not modify the current
 *  index. It is works starting with the recorded extensions pointer.
 */
{
    int ret;
    word32 idx = 0;
    int sz = cert->extensionsSz;
    byte* input = cert->extensions;
    int length;
    word32 oid;
    byte critical = 0;
    byte criticalFail = 0;

    WOLFSSL_ENTER("DecodeCertExtensions");

    if (input == NULL || sz == 0)
        return BAD_FUNC_ARG;

    if (input[idx++] != ASN_EXTENSIONS) {
        WOLFSSL_MSG("\tfail: should be an EXTENSIONS");
        return ASN_PARSE_E;
    }

    if (GetLength(input, &idx, &length, sz) < 0) {
        WOLFSSL_MSG("\tfail: invalid length");
        return ASN_PARSE_E;
    }

    if (GetSequence(input, &idx, &length, sz) < 0) {
        WOLFSSL_MSG("\tfail: should be a SEQUENCE (1)");
        return ASN_PARSE_E;
    }

    while (idx < (word32)sz) {
        if (GetSequence(input, &idx, &length, sz) < 0) {
            WOLFSSL_MSG("\tfail: should be a SEQUENCE");
            return ASN_PARSE_E;
        }

        oid = 0;
        if ((ret = GetObjectId(input, &idx, &oid, oidCertExtType, sz)) < 0) {
            WOLFSSL_MSG("\tfail: OBJECT ID");
            return ret;
        }

        /* check for critical flag */
        critical = 0;
        if (input[idx] == ASN_BOOLEAN) {
            ret = GetBoolean(input, &idx, sz);
            if (ret < 0) {
                WOLFSSL_MSG("\tfail: critical boolean");
                return ret;
            }

            critical = (byte)ret;
        }

        /* process the extension based on the OID */
        ret = GetOctetString(input, &idx, &length, sz);
        if (ret < 0) {
            WOLFSSL_MSG("\tfail: bad OCTET STRING");
            return ret;
        }

        switch (oid) {
            case BASIC_CA_OID:
                #ifdef OPENSSL_EXTRA
                    cert->extBasicConstSet = 1;
                    cert->extBasicConstCrit = critical;
                #endif
                if (DecodeBasicCaConstraint(&input[idx], length, cert) < 0)
                    return ASN_PARSE_E;
                break;

            case CRL_DIST_OID:
                #ifdef OPENSSL_EXTRA
                    cert->extCRLdistSet  = 1;
                    cert->extCRLdistCrit = critical;
                #endif
                if (DecodeCrlDist(&input[idx], length, cert) < 0)
                    return ASN_PARSE_E;
                break;

            case AUTH_INFO_OID:
                #ifdef OPENSSL_EXTRA
                    cert->extAuthInfoSet  = 1;
                    cert->extAuthInfoCrit = critical;
                #endif
                if (DecodeAuthInfo(&input[idx], length, cert) < 0)
                    return ASN_PARSE_E;
                break;

            case ALT_NAMES_OID:
                #ifdef OPENSSL_EXTRA
                    cert->extSubjAltNameSet = 1;
                    cert->extSubjAltNameCrit = critical;
                #endif
                if (DecodeAltNames(&input[idx], length, cert) < 0)
                    return ASN_PARSE_E;
                break;

            case AUTH_KEY_OID:
                cert->extAuthKeyIdSet = 1;
                #ifdef OPENSSL_EXTRA
                    cert->extAuthKeyIdCrit = critical;
                #endif
                if (DecodeAuthKeyId(&input[idx], length, cert) < 0)
                    return ASN_PARSE_E;
                break;

            case SUBJ_KEY_OID:
                cert->extSubjKeyIdSet = 1;
                #ifdef OPENSSL_EXTRA
                    cert->extSubjKeyIdCrit = critical;
                #endif
                #ifndef WOLFSSL_ALLOW_CRIT_SKID
                    /* This check is added due to RFC 5280 section 4.2.1.2
                     * stating that conforming CA's must mark this extension
                     * as non-critical. When parsing extensions check that
                     * certificate was made in compliance with this. */
                    if (critical) {
                        WOLFSSL_MSG("Critical Subject Key ID is not allowed");
                        WOLFSSL_MSG("Use macro WOLFSSL_ALLOW_CRIT_SKID if wanted");
                        return ASN_CRIT_EXT_E;
                    }
                #endif

                if (DecodeSubjKeyId(&input[idx], length, cert) < 0)
                    return ASN_PARSE_E;
                break;

            case CERT_POLICY_OID:
                #ifdef WOLFSSL_SEP
                    #ifdef OPENSSL_EXTRA
                        cert->extCertPolicySet = 1;
                        cert->extCertPolicyCrit = critical;
                    #endif
                #endif
                #if defined(WOLFSSL_SEP) || defined(WOLFSSL_CERT_EXT)
                    if (DecodeCertPolicy(&input[idx], length, cert) < 0) {
                        return ASN_PARSE_E;
                    }
                #else
                    WOLFSSL_MSG("Certificate Policy extension not supported yet.");
                #endif
                break;

            case KEY_USAGE_OID:
                cert->extKeyUsageSet = 1;
                #ifdef OPENSSL_EXTRA
                    cert->extKeyUsageCrit = critical;
                #endif
                if (DecodeKeyUsage(&input[idx], length, cert) < 0)
                    return ASN_PARSE_E;
                break;

            case EXT_KEY_USAGE_OID:
                cert->extExtKeyUsageSet = 1;
                #ifdef OPENSSL_EXTRA
                    cert->extExtKeyUsageCrit = critical;
                #endif
                if (DecodeExtKeyUsage(&input[idx], length, cert) < 0)
                    return ASN_PARSE_E;
                break;

            #ifndef IGNORE_NAME_CONSTRAINTS
            case NAME_CONS_OID:
                cert->extNameConstraintSet = 1;
                #ifdef OPENSSL_EXTRA
                    cert->extNameConstraintCrit = critical;
                #endif
                if (DecodeNameConstraints(&input[idx], length, cert) < 0)
                    return ASN_PARSE_E;
                break;
            #endif /* IGNORE_NAME_CONSTRAINTS */

            case INHIBIT_ANY_OID:
                WOLFSSL_MSG("Inhibit anyPolicy extension not supported yet.");
                break;

            default:
                /* While it is a failure to not support critical extensions,
                 * still parse the certificate ignoring the unsupported
                 * extension to allow caller to accept it with the verify
                 * callback. */
                if (critical)
                    criticalFail = 1;
                break;
        }
        idx += length;
    }

    return criticalFail ? ASN_CRIT_EXT_E : 0;
}

int ParseCert(DecodedCert* cert, int type, int verify, void* cm)
{
    int   ret;
    char* ptr;

    ret = ParseCertRelative(cert, type, verify, cm);
    if (ret < 0)
        return ret;

    if (cert->subjectCNLen > 0) {
        ptr = (char*) XMALLOC(cert->subjectCNLen + 1, cert->heap,
                              DYNAMIC_TYPE_SUBJECT_CN);
        if (ptr == NULL)
            return MEMORY_E;
        XMEMCPY(ptr, cert->subjectCN, cert->subjectCNLen);
        ptr[cert->subjectCNLen] = '\0';
        cert->subjectCN = ptr;
        cert->subjectCNStored = 1;
    }

    if (cert->keyOID == RSAk &&
                          cert->publicKey != NULL  && cert->pubKeySize > 0) {
        ptr = (char*) XMALLOC(cert->pubKeySize, cert->heap,
                              DYNAMIC_TYPE_PUBLIC_KEY);
        if (ptr == NULL)
            return MEMORY_E;
        XMEMCPY(ptr, cert->publicKey, cert->pubKeySize);
        cert->publicKey = (byte *)ptr;
        cert->pubKeyStored = 1;
    }

    return ret;
}

/* from SSL proper, for locking can't do find here anymore */
#ifdef __cplusplus
    extern "C" {
#endif
    WOLFSSL_LOCAL Signer* GetCA(void* signers, byte* hash);
    #ifndef NO_SKID
        WOLFSSL_LOCAL Signer* GetCAByName(void* signers, byte* hash);
    #endif
#ifdef __cplusplus
    }
#endif


#if defined(WOLFCRYPT_ONLY) || defined(NO_CERTS)

/* dummy functions, not using wolfSSL so don't need actual ones */
Signer* GetCA(void* signers, byte* hash)
{
    (void)hash;

    return (Signer*)signers;
}

#ifndef NO_SKID
Signer* GetCAByName(void* signers, byte* hash)
{
    (void)hash;

    return (Signer*)signers;
}
#endif /* NO_SKID */

#endif /* WOLFCRYPT_ONLY || NO_CERTS */

int ParseCertRelative(DecodedCert* cert, int type, int verify, void* cm)
{
    int    ret = 0;
    int    badDate = 0;
    int    criticalExt = 0;
    word32 confirmOID;

    if (cert == NULL) {
        return BAD_FUNC_ARG;
    }

    if (cert->sigCtx.state == SIG_STATE_BEGIN) {
        if ((ret = DecodeToKey(cert, verify)) < 0) {
            if (ret == ASN_BEFORE_DATE_E || ret == ASN_AFTER_DATE_E)
                badDate = ret;
            else
                return ret;
        }

        WOLFSSL_MSG("Parsed Past Key");

        if (cert->srcIdx < cert->sigIndex) {
        #ifndef ALLOW_V1_EXTENSIONS
            if (cert->version < 2) {
                WOLFSSL_MSG("\tv1 and v2 certs not allowed extensions");
                return ASN_VERSION_E;
            }
        #endif

            /* save extensions */
            cert->extensions    = &cert->source[cert->srcIdx];
            cert->extensionsSz  = cert->sigIndex - cert->srcIdx;
            cert->extensionsIdx = cert->srcIdx;   /* for potential later use */

            if ((ret = DecodeCertExtensions(cert)) < 0) {
                if (ret == ASN_CRIT_EXT_E)
                    criticalExt = ret;
                else
                    return ret;
            }

            /* advance past extensions */
            cert->srcIdx = cert->sigIndex;
        }

        if ((ret = GetAlgoId(cert->source, &cert->srcIdx, &confirmOID,
                             oidSigType, cert->maxIdx)) < 0)
            return ret;

        if ((ret = GetSignature(cert)) < 0)
            return ret;

        if (confirmOID != cert->signatureOID)
            return ASN_SIG_OID_E;

    #ifndef NO_SKID
        if (cert->extSubjKeyIdSet == 0 && cert->publicKey != NULL &&
                                                        cert->pubKeySize > 0) {
        #ifdef NO_SHA
            ret = wc_Sha256Hash(cert->publicKey, cert->pubKeySize,
                                                            cert->extSubjKeyId);
        #else
            ret = wc_ShaHash(cert->publicKey, cert->pubKeySize,
                                                            cert->extSubjKeyId);
        #endif /* NO_SHA */
            if (ret != 0)
                return ret;
        }
    #endif /* !NO_SKID */

        if (verify != NO_VERIFY && type != CA_TYPE && type != TRUSTED_PEER_TYPE) {
            cert->ca = NULL;
    #ifndef NO_SKID
            if (cert->extAuthKeyIdSet)
                cert->ca = GetCA(cm, cert->extAuthKeyId);
            if (cert->ca == NULL)
                cert->ca = GetCAByName(cm, cert->issuerHash);

            /* alternate lookup method using subject and match on public key */
        #ifdef WOLFSSL_NO_TRUSTED_CERTS_VERIFY
            if (cert->ca == NULL && verify == VERIFY_OCSP) {
                if (cert->extSubjKeyIdSet) {
                    cert->ca = GetCA(cm, cert->extSubjKeyId);
                }
                if (cert->ca == NULL) {
                    cert->ca = GetCAByName(cm, cert->subjectHash);
                }
                if (cert->ca) {
                    if ((cert->ca->pubKeySize == cert->pubKeySize) &&
                        (XMEMCMP(cert->ca->publicKey, cert->publicKey,
                                                cert->ca->pubKeySize) == 0)) {
                        ret = 0; /* success */
                        goto exit_pcr;
                    }
                }
            }
        #endif /* WOLFSSL_NO_TRUSTED_CERTS_VERIFY */
    #else
            cert->ca = GetCA(cm, cert->issuerHash);
    #endif /* !NO_SKID */

            WOLFSSL_MSG("About to verify certificate signature");
            if (cert->ca) {
                if (cert->isCA) {
                    if (cert->ca->pathLengthSet) {
                        if (cert->ca->pathLength == 0) {
                            WOLFSSL_MSG("CA with path length 0 signing a CA");
                            return ASN_PATHLEN_INV_E;
                        }
                        if (cert->pathLengthSet &&
                            cert->pathLength >= cert->ca->pathLength) {

                            WOLFSSL_MSG("CA signing CA with longer path length");
                            return ASN_PATHLEN_INV_E;
                        }
                    }
                }

        #ifdef HAVE_OCSP
                /* Need the CA's public key hash for OCSP */
            #ifdef NO_SHA
                ret = wc_Sha256Hash(cert->ca->publicKey, cert->ca->pubKeySize,
                                                            cert->issuerKeyHash);
            #else
                ret = wc_ShaHash(cert->ca->publicKey, cert->ca->pubKeySize,
                                                            cert->issuerKeyHash);
            #endif /* NO_SHA */
                if (ret != 0)
                    return ret;
        #endif /* HAVE_OCSP */
            }
        }
    }

    if (verify != NO_VERIFY && type != CA_TYPE && type != TRUSTED_PEER_TYPE) {
        if (cert->ca) {
            if (verify == VERIFY || verify == VERIFY_OCSP) {
                /* try to confirm/verify signature */
                if ((ret = ConfirmSignature(&cert->sigCtx,
                        cert->source + cert->certBegin,
                        cert->sigIndex - cert->certBegin,
                        cert->ca->publicKey, cert->ca->pubKeySize,
                        cert->ca->keyOID, cert->signature,
                        cert->sigLength, cert->signatureOID)) != 0) {
                    if (ret != WC_PENDING_E) {
                        WOLFSSL_MSG("Confirm signature failed");
                    }
                    return ret;
                }
            #ifndef IGNORE_NAME_CONSTRAINTS
                /* check that this cert's name is permitted by the signer's
                 * name constraints */
                if (!ConfirmNameConstraints(cert->ca, cert)) {
                    WOLFSSL_MSG("Confirm name constraint failed");
                    return ASN_NAME_INVALID_E;
                }
            #endif /* IGNORE_NAME_CONSTRAINTS */
            }
        }
        else {
            /* no signer */
            WOLFSSL_MSG("No CA signer to verify with");
            return ASN_NO_SIGNER_E;
        }
    }

#ifdef WOLFSSL_NO_TRUSTED_CERTS_VERIFY
exit_pcr:
#endif

    if (badDate != 0)
        return badDate;

    if (criticalExt != 0)
        return criticalExt;

    return ret;
}

/* Create and init an new signer */
Signer* MakeSigner(void* heap)
{
    Signer* signer = (Signer*) XMALLOC(sizeof(Signer), heap,
                                       DYNAMIC_TYPE_SIGNER);
    if (signer) {
        signer->pubKeySize = 0;
        signer->keyOID     = 0;
        signer->publicKey  = NULL;
        signer->nameLen    = 0;
        signer->name       = NULL;
    #ifndef IGNORE_NAME_CONSTRAINTS
        signer->permittedNames = NULL;
        signer->excludedNames = NULL;
    #endif /* IGNORE_NAME_CONSTRAINTS */
        signer->pathLengthSet = 0;
        signer->pathLength = 0;
        signer->next       = NULL;
    }
    (void)heap;

    return signer;
}


/* Free an individual signer */
void FreeSigner(Signer* signer, void* heap)
{
    XFREE(signer->name, heap, DYNAMIC_TYPE_SUBJECT_CN);
    XFREE(signer->publicKey, heap, DYNAMIC_TYPE_PUBLIC_KEY);
#ifndef IGNORE_NAME_CONSTRAINTS
    if (signer->permittedNames)
        FreeNameSubtrees(signer->permittedNames, heap);
    if (signer->excludedNames)
        FreeNameSubtrees(signer->excludedNames, heap);
#endif
    XFREE(signer, heap, DYNAMIC_TYPE_SIGNER);

    (void)heap;
}


/* Free the whole singer table with number of rows */
void FreeSignerTable(Signer** table, int rows, void* heap)
{
    int i;

    for (i = 0; i < rows; i++) {
        Signer* signer = table[i];
        while (signer) {
            Signer* next = signer->next;
            FreeSigner(signer, heap);
            signer = next;
        }
        table[i] = NULL;
    }
}

#ifdef WOLFSSL_TRUST_PEER_CERT
/* Free an individual trusted peer cert */
void FreeTrustedPeer(TrustedPeerCert* tp, void* heap)
{
    if (tp == NULL) {
        return;
    }

    if (tp->name) {
        XFREE(tp->name, heap, DYNAMIC_TYPE_SUBJECT_CN);
    }

    if (tp->sig) {
        XFREE(tp->sig, heap, DYNAMIC_TYPE_SIGNATURE);
    }
#ifndef IGNORE_NAME_CONSTRAINTS
    if (tp->permittedNames)
        FreeNameSubtrees(tp->permittedNames, heap);
    if (tp->excludedNames)
        FreeNameSubtrees(tp->excludedNames, heap);
#endif
    XFREE(tp, heap, DYNAMIC_TYPE_CERT);

    (void)heap;
}

/* Free the whole Trusted Peer linked list */
void FreeTrustedPeerTable(TrustedPeerCert** table, int rows, void* heap)
{
    int i;

    for (i = 0; i < rows; i++) {
        TrustedPeerCert* tp = table[i];
        while (tp) {
            TrustedPeerCert* next = tp->next;
            FreeTrustedPeer(tp, heap);
            tp = next;
        }
        table[i] = NULL;
    }
}
#endif /* WOLFSSL_TRUST_PEER_CERT */

WOLFSSL_LOCAL int SetMyVersion(word32 version, byte* output, int header)
{
    int i = 0;

    if (output == NULL)
        return BAD_FUNC_ARG;

    if (header) {
        output[i++] = ASN_CONTEXT_SPECIFIC | ASN_CONSTRUCTED;
        output[i++] = 3;
    }
    output[i++] = ASN_INTEGER;
    output[i++] = 0x01;
    output[i++] = (byte)version;

    return i;
}


WOLFSSL_LOCAL int SetSerialNumber(const byte* sn, word32 snSz, byte* output)
{
    int result = 0;

    WOLFSSL_ENTER("SetSerialNumber");

    if (sn == NULL || output == NULL)
        return BAD_FUNC_ARG;

    if (snSz <= EXTERNAL_SERIAL_SIZE) {
        output[0] = ASN_INTEGER;
        /* The serial number is always positive. When encoding the
         * INTEGER, if the MSB is 1, add a padding zero to keep the
         * number positive. */
        if (sn[0] & 0x80) {
            output[1] = (byte)snSz + 1;
            output[2] = 0;
            XMEMCPY(&output[3], sn, snSz);
            result = snSz + 3;
        }
        else {
            output[1] = (byte)snSz;
            XMEMCPY(&output[2], sn, snSz);
            result = snSz + 2;
        }
    }
    return result;
}

WOLFSSL_LOCAL int GetSerialNumber(const byte* input, word32* inOutIdx,
    byte* serial, int* serialSz, word32 maxIdx)
{
    int result = 0;
    int ret;

    WOLFSSL_ENTER("GetSerialNumber");

    if (serial == NULL || input == NULL || serialSz == NULL) {
        return BAD_FUNC_ARG;
    }

    /* First byte is ASN type */
    if ((*inOutIdx+1) > maxIdx) {
        WOLFSSL_MSG("Bad idx first");
        return BUFFER_E;
    }

    ret = GetASNInt(input, inOutIdx, serialSz, maxIdx);
    if (ret != 0)
        return ret;

    if (*serialSz > EXTERNAL_SERIAL_SIZE) {
        WOLFSSL_MSG("Serial size bad");
        return ASN_PARSE_E;
    }

    /* return serial */
    XMEMCPY(serial, &input[*inOutIdx], *serialSz);
    *inOutIdx += *serialSz;

    return result;
}



const char* BEGIN_CERT         = "-----BEGIN CERTIFICATE-----";
const char* END_CERT           = "-----END CERTIFICATE-----";
const char* BEGIN_CERT_REQ     = "-----BEGIN CERTIFICATE REQUEST-----";
const char* END_CERT_REQ       = "-----END CERTIFICATE REQUEST-----";
const char* BEGIN_DH_PARAM     = "-----BEGIN DH PARAMETERS-----";
const char* END_DH_PARAM       = "-----END DH PARAMETERS-----";
const char* BEGIN_DSA_PARAM    = "-----BEGIN DSA PARAMETERS-----";
const char* END_DSA_PARAM      = "-----END DSA PARAMETERS-----";
const char* BEGIN_X509_CRL     = "-----BEGIN X509 CRL-----";
const char* END_X509_CRL       = "-----END X509 CRL-----";
const char* BEGIN_RSA_PRIV     = "-----BEGIN RSA PRIVATE KEY-----";
const char* END_RSA_PRIV       = "-----END RSA PRIVATE KEY-----";
const char* BEGIN_PRIV_KEY     = "-----BEGIN PRIVATE KEY-----";
const char* END_PRIV_KEY       = "-----END PRIVATE KEY-----";
const char* BEGIN_ENC_PRIV_KEY = "-----BEGIN ENCRYPTED PRIVATE KEY-----";
const char* END_ENC_PRIV_KEY   = "-----END ENCRYPTED PRIVATE KEY-----";
const char* BEGIN_EC_PRIV      = "-----BEGIN EC PRIVATE KEY-----";
const char* END_EC_PRIV        = "-----END EC PRIVATE KEY-----";
const char* BEGIN_DSA_PRIV     = "-----BEGIN DSA PRIVATE KEY-----";
const char* END_DSA_PRIV       = "-----END DSA PRIVATE KEY-----";
const char* BEGIN_PUB_KEY      = "-----BEGIN PUBLIC KEY-----";
const char* END_PUB_KEY        = "-----END PUBLIC KEY-----";
const char* BEGIN_EDDSA_PRIV   = "-----BEGIN EDDSA PRIVATE KEY-----";
const char* END_EDDSA_PRIV     = "-----END EDDSA PRIVATE KEY-----";

#if defined(WOLFSSL_KEY_GEN) || defined(WOLFSSL_CERT_GEN) || defined(OPENSSL_EXTRA)

/* Used for compatibility API */
int wc_DerToPem(const byte* der, word32 derSz,
                byte* output, word32 outSz, int type)
{
    return wc_DerToPemEx(der, derSz, output, outSz, NULL, type);
}

/* convert der buffer to pem into output, can't do inplace, der and output
   need to be different */
int wc_DerToPemEx(const byte* der, word32 derSz, byte* output, word32 outSz,
             byte *cipher_info, int type)
{
#ifdef WOLFSSL_SMALL_STACK
    char* header = NULL;
    char* footer = NULL;
#else
    char header[40 + HEADER_ENCRYPTED_KEY_SIZE];
    char footer[40];
#endif

    int headerLen = 40 + HEADER_ENCRYPTED_KEY_SIZE;
    int footerLen = 40;
    int i;
    int err;
    int outLen;   /* return length or error */

    if (der == output)      /* no in place conversion */
        return BAD_FUNC_ARG;

#ifdef WOLFSSL_SMALL_STACK
    header = (char*)XMALLOC(headerLen, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (header == NULL)
        return MEMORY_E;

    footer = (char*)XMALLOC(footerLen, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (footer == NULL) {
        XFREE(header, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        return MEMORY_E;
    }
#endif
    if (type == CERT_TYPE) {
        XSTRNCPY(header, BEGIN_CERT, headerLen);
        XSTRNCAT(header, "\n", 1);

        XSTRNCPY(footer, END_CERT, footerLen);
        XSTRNCAT(footer, "\n", 1);
    }
    else if (type == PRIVATEKEY_TYPE) {
        XSTRNCPY(header, BEGIN_RSA_PRIV, headerLen);
        XSTRNCAT(header, "\n", 1);

        XSTRNCPY(footer, END_RSA_PRIV, footerLen);
        XSTRNCAT(footer, "\n", 1);
    }
#ifndef NO_DSA
    else if (type == DSA_PRIVATEKEY_TYPE) {
        XSTRNCPY(header, BEGIN_DSA_PRIV, headerLen);
        XSTRNCAT(header, "\n", 1);

        XSTRNCPY(footer, END_DSA_PRIV, footerLen);
        XSTRNCAT(footer, "\n", 1);
    }
#endif
#ifdef HAVE_ECC
    else if (type == ECC_PRIVATEKEY_TYPE) {
        XSTRNCPY(header, BEGIN_EC_PRIV, headerLen);
        XSTRNCAT(header, "\n", 1);

        XSTRNCPY(footer, END_EC_PRIV, footerLen);
        XSTRNCAT(footer, "\n", 1);
    }
#endif
#ifdef HAVE_ED25519
    else if (type == EDDSA_PRIVATEKEY_TYPE) {
        XSTRNCPY(header, BEGIN_EDDSA_PRIV, headerLen);
        XSTRNCAT(header, "\n", 1);

        XSTRNCPY(footer, END_EDDSA_PRIV, footerLen);
        XSTRNCAT(footer, "\n", 1);
    }
#endif
#ifdef WOLFSSL_CERT_REQ
    else if (type == CERTREQ_TYPE)
    {
        XSTRNCPY(header, BEGIN_CERT_REQ, headerLen);
        XSTRNCAT(header, "\n", 1);

        XSTRNCPY(footer, END_CERT_REQ, footerLen);
        XSTRNCAT(footer, "\n", 1);
    }
#endif
#ifdef HAVE_CRL
    else if (type == CRL_TYPE)
    {
        XSTRNCPY(header, BEGIN_X509_CRL, headerLen);
        XSTRNCAT(header, "\n", 1);

        XSTRNCPY(footer, END_X509_CRL, footerLen);
        XSTRNCAT(footer, "\n", 1);
    }
#endif
    else {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(header, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(footer, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return BAD_FUNC_ARG;
    }

    /* extra header information for encrypted key */
    if (cipher_info != NULL) {
        size_t cipherInfoStrLen = XSTRLEN((char*)cipher_info);
        if (cipherInfoStrLen > HEADER_ENCRYPTED_KEY_SIZE - (23+10+2))
            cipherInfoStrLen = HEADER_ENCRYPTED_KEY_SIZE - (23+10+2);

        XSTRNCAT(header, "Proc-Type: 4,ENCRYPTED\n", 23);
        XSTRNCAT(header, "DEK-Info: ", 10);
        XSTRNCAT(header, (char*)cipher_info, cipherInfoStrLen);
        XSTRNCAT(header, "\n\n", 2);
    }

    headerLen = (int)XSTRLEN(header);
    footerLen = (int)XSTRLEN(footer);

    /* if null output and 0 size passed in then return size needed */
    if (!output && outSz == 0) {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(header, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(footer, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        outLen = 0;
        if ((err = Base64_Encode(der, derSz, NULL, (word32*)&outLen))
                != LENGTH_ONLY_E) {
            return err;
        }
        return headerLen + footerLen + outLen;
    }

    if (!der || !output) {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(header, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(footer, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return BAD_FUNC_ARG;
    }

    /* don't even try if outSz too short */
    if (outSz < headerLen + footerLen + derSz) {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(header, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(footer, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return BAD_FUNC_ARG;
    }

    /* header */
    XMEMCPY(output, header, headerLen);
    i = headerLen;

#ifdef WOLFSSL_SMALL_STACK
    XFREE(header, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    /* body */
    outLen = outSz - (headerLen + footerLen);  /* input to Base64_Encode */
    if ( (err = Base64_Encode(der, derSz, output + i, (word32*)&outLen)) < 0) {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(footer, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return err;
    }
    i += outLen;

    /* footer */
    if ( (i + footerLen) > (int)outSz) {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(footer, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return BAD_FUNC_ARG;
    }
    XMEMCPY(output + i, footer, footerLen);

#ifdef WOLFSSL_SMALL_STACK
    XFREE(footer, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return outLen + headerLen + footerLen;
}

#endif /* WOLFSSL_KEY_GEN || WOLFSSL_CERT_GEN */

#if !defined(NO_RSA) && (defined(WOLFSSL_CERT_GEN) || (defined(WOLFSSL_KEY_GEN) && !defined(HAVE_USER_RSA)))
/* USER RSA ifdef portions used instead of refactor in consideration for
   possible fips build */
/* Write a public RSA key to output */
static int SetRsaPublicKey(byte* output, RsaKey* key,
                           int outLen, int with_header)
{
#ifdef WOLFSSL_SMALL_STACK
    byte* n = NULL;
    byte* e = NULL;
#else
    byte n[MAX_RSA_INT_SZ];
    byte e[MAX_RSA_E_SZ];
#endif
    byte seq[MAX_SEQ_SZ];
    byte bitString[1 + MAX_LENGTH_SZ + 1];
    int  nSz;
    int  eSz;
    int  seqSz;
    int  bitStringSz;
    int  idx;

    if (output == NULL || key == NULL || outLen < MAX_SEQ_SZ)
        return BAD_FUNC_ARG;

    /* n */
#ifdef WOLFSSL_SMALL_STACK
    n = (byte*)XMALLOC(MAX_RSA_INT_SZ, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (n == NULL)
        return MEMORY_E;
#endif

#ifdef HAVE_USER_RSA
    nSz = SetASNIntRSA(key->n, n);
#else
    nSz = SetASNIntMP(&key->n, MAX_RSA_INT_SZ, n);
#endif
    if (nSz < 0) {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(n, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return nSz;
    }

    /* e */
#ifdef WOLFSSL_SMALL_STACK
    e = (byte*)XMALLOC(MAX_RSA_E_SZ, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (e == NULL) {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(n, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return MEMORY_E;
    }
#endif

#ifdef HAVE_USER_RSA
    eSz = SetASNIntRSA(key->e, e);
#else
    eSz = SetASNIntMP(&key->e, MAX_RSA_INT_SZ, e);
#endif
    if (eSz < 0) {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(n, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(e, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return eSz;
    }

    seqSz  = SetSequence(nSz + eSz, seq);

    /* check output size */
    if ( (seqSz + nSz + eSz) > outLen) {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(n,    NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(e,    NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return BUFFER_E;
    }

    /* headers */
    if (with_header) {
        int  algoSz;
#ifdef WOLFSSL_SMALL_STACK
        byte* algo = NULL;

        algo = (byte*)XMALLOC(MAX_ALGO_SZ, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        if (algo == NULL) {
            XFREE(n, NULL, DYNAMIC_TYPE_TMP_BUFFER);
            XFREE(e, NULL, DYNAMIC_TYPE_TMP_BUFFER);
            return MEMORY_E;
        }
#else
        byte algo[MAX_ALGO_SZ];
#endif
        algoSz = SetAlgoID(RSAk, algo, oidKeyType, 0);
        bitStringSz  = SetBitString(seqSz + nSz + eSz, 0, bitString);

        idx = SetSequence(nSz + eSz + seqSz + bitStringSz + algoSz, output);

        /* check output size */
        if ( (idx + algoSz + bitStringSz + seqSz + nSz + eSz) > outLen) {
            #ifdef WOLFSSL_SMALL_STACK
                XFREE(n,    NULL, DYNAMIC_TYPE_TMP_BUFFER);
                XFREE(e,    NULL, DYNAMIC_TYPE_TMP_BUFFER);
                XFREE(algo, NULL, DYNAMIC_TYPE_TMP_BUFFER);
            #endif

            return BUFFER_E;
        }

        /* algo */
        XMEMCPY(output + idx, algo, algoSz);
        idx += algoSz;
        /* bit string */
        XMEMCPY(output + idx, bitString, bitStringSz);
        idx += bitStringSz;
#ifdef WOLFSSL_SMALL_STACK
        XFREE(algo, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
    }
    else
        idx = 0;

    /* seq */
    XMEMCPY(output + idx, seq, seqSz);
    idx += seqSz;
    /* n */
    XMEMCPY(output + idx, n, nSz);
    idx += nSz;
    /* e */
    XMEMCPY(output + idx, e, eSz);
    idx += eSz;

#ifdef WOLFSSL_SMALL_STACK
    XFREE(n,    NULL, DYNAMIC_TYPE_TMP_BUFFER);
    XFREE(e,    NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return idx;
}
#endif /* !NO_RSA && (WOLFSSL_CERT_GEN || (WOLFSSL_KEY_GEN &&
                                           !HAVE_USER_RSA))) */


#if defined(WOLFSSL_KEY_GEN) && !defined(NO_RSA) && !defined(HAVE_USER_RSA)


static mp_int* GetRsaInt(RsaKey* key, int idx)
{
    if (idx == 0)
        return &key->n;
    if (idx == 1)
        return &key->e;
    if (idx == 2)
        return &key->d;
    if (idx == 3)
        return &key->p;
    if (idx == 4)
        return &key->q;
    if (idx == 5)
        return &key->dP;
    if (idx == 6)
        return &key->dQ;
    if (idx == 7)
        return &key->u;

    return NULL;
}


/* Release Tmp RSA resources */
static INLINE void FreeTmpRsas(byte** tmps, void* heap)
{
    int i;

    (void)heap;

    for (i = 0; i < RSA_INTS; i++)
        XFREE(tmps[i], heap, DYNAMIC_TYPE_RSA);
}


/* Convert RsaKey key to DER format, write to output (inLen), return bytes
   written */
int wc_RsaKeyToDer(RsaKey* key, byte* output, word32 inLen)
{
    word32 seqSz, verSz, rawLen, intTotalLen = 0;
    word32 sizes[RSA_INTS];
    int    i, j, outLen, ret = 0, mpSz;

    byte  seq[MAX_SEQ_SZ];
    byte  ver[MAX_VERSION_SZ];
    byte* tmps[RSA_INTS];

    if (!key || !output)
        return BAD_FUNC_ARG;

    if (key->type != RSA_PRIVATE)
        return BAD_FUNC_ARG;

    for (i = 0; i < RSA_INTS; i++)
        tmps[i] = NULL;

    /* write all big ints from key to DER tmps */
    for (i = 0; i < RSA_INTS; i++) {
        mp_int* keyInt = GetRsaInt(key, i);

        rawLen = mp_unsigned_bin_size(keyInt) + 1;
        tmps[i] = (byte*)XMALLOC(rawLen + MAX_SEQ_SZ, key->heap,
                                 DYNAMIC_TYPE_RSA);
        if (tmps[i] == NULL) {
            ret = MEMORY_E;
            break;
        }

        mpSz = SetASNIntMP(keyInt, MAX_RSA_INT_SZ, tmps[i]);
        if (mpSz < 0) {
            ret = mpSz;
            break;
        }
        intTotalLen += (sizes[i] = mpSz);
    }

    if (ret != 0) {
        FreeTmpRsas(tmps, key->heap);
        return ret;
    }

    /* make headers */
    verSz = SetMyVersion(0, ver, FALSE);
    seqSz = SetSequence(verSz + intTotalLen, seq);

    outLen = seqSz + verSz + intTotalLen;
    if (outLen > (int)inLen)
        return BAD_FUNC_ARG;

    /* write to output */
    XMEMCPY(output, seq, seqSz);
    j = seqSz;
    XMEMCPY(output + j, ver, verSz);
    j += verSz;

    for (i = 0; i < RSA_INTS; i++) {
        XMEMCPY(output + j, tmps[i], sizes[i]);
        j += sizes[i];
    }
    FreeTmpRsas(tmps, key->heap);

    return outLen;
}


/* Convert Rsa Public key to DER format, write to output (inLen), return bytes
   written */
int wc_RsaKeyToPublicDer(RsaKey* key, byte* output, word32 inLen)
{
    return SetRsaPublicKey(output, key, inLen, 1);
}

#endif /* WOLFSSL_KEY_GEN && !NO_RSA && !HAVE_USER_RSA */


#if defined(WOLFSSL_CERT_GEN) && !defined(NO_RSA)

/* Initialize and Set Certificate defaults:
   version    = 3 (0x2)
   serial     = 0
   sigType    = SHA_WITH_RSA
   issuer     = blank
   daysValid  = 500
   selfSigned = 1 (true) use subject as issuer
   subject    = blank
*/
int wc_InitCert(Cert* cert)
{
    if (cert == NULL) {
        return BAD_FUNC_ARG;
    }

    cert->version    = 2;   /* version 3 is hex 2 */
    cert->sigType    = CTC_SHAwRSA;
    cert->daysValid  = 500;
    cert->selfSigned = 1;
    cert->isCA       = 0;
    cert->bodySz     = 0;
#ifdef WOLFSSL_ALT_NAMES
    cert->altNamesSz   = 0;
    cert->beforeDateSz = 0;
    cert->afterDateSz  = 0;
#endif
#ifdef WOLFSSL_CERT_EXT
    cert->skidSz = 0;
    cert->akidSz = 0;
    cert->keyUsage = 0;
    cert->extKeyUsage = 0;
    cert->certPoliciesNb = 0;
    XMEMSET(cert->akid, 0, CTC_MAX_AKID_SIZE);
    XMEMSET(cert->skid, 0, CTC_MAX_SKID_SIZE);
    XMEMSET(cert->certPolicies, 0, CTC_MAX_CERTPOL_NB*CTC_MAX_CERTPOL_SZ);
#endif
    cert->keyType    = RSA_KEY;
    XMEMSET(cert->serial, 0, CTC_SERIAL_SIZE);

    cert->issuer.country[0] = '\0';
    cert->issuer.countryEnc = CTC_PRINTABLE;
    cert->issuer.state[0] = '\0';
    cert->issuer.stateEnc = CTC_UTF8;
    cert->issuer.locality[0] = '\0';
    cert->issuer.localityEnc = CTC_UTF8;
    cert->issuer.sur[0] = '\0';
    cert->issuer.surEnc = CTC_UTF8;
    cert->issuer.org[0] = '\0';
    cert->issuer.orgEnc = CTC_UTF8;
    cert->issuer.unit[0] = '\0';
    cert->issuer.unitEnc = CTC_UTF8;
    cert->issuer.commonName[0] = '\0';
    cert->issuer.commonNameEnc = CTC_UTF8;
    cert->issuer.email[0] = '\0';

    cert->subject.country[0] = '\0';
    cert->subject.countryEnc = CTC_PRINTABLE;
    cert->subject.state[0] = '\0';
    cert->subject.stateEnc = CTC_UTF8;
    cert->subject.locality[0] = '\0';
    cert->subject.localityEnc = CTC_UTF8;
    cert->subject.sur[0] = '\0';
    cert->subject.surEnc = CTC_UTF8;
    cert->subject.org[0] = '\0';
    cert->subject.orgEnc = CTC_UTF8;
    cert->subject.unit[0] = '\0';
    cert->subject.unitEnc = CTC_UTF8;
    cert->subject.commonName[0] = '\0';
    cert->subject.commonNameEnc = CTC_UTF8;
    cert->subject.email[0] = '\0';

#ifdef WOLFSSL_CERT_REQ
    cert->challengePw[0] ='\0';
#endif
#ifdef WOLFSSL_HEAP_TEST
    cert->heap = (void*)WOLFSSL_HEAP_TEST;
#else
    cert->heap = NULL;
#endif

    return 0;
}


/* DER encoded x509 Certificate */
typedef struct DerCert {
    byte size[MAX_LENGTH_SZ];          /* length encoded */
    byte version[MAX_VERSION_SZ];      /* version encoded */
    byte serial[CTC_SERIAL_SIZE + MAX_LENGTH_SZ]; /* serial number encoded */
    byte sigAlgo[MAX_ALGO_SZ];         /* signature algo encoded */
    byte issuer[ASN_NAME_MAX];         /* issuer  encoded */
    byte subject[ASN_NAME_MAX];        /* subject encoded */
    byte validity[MAX_DATE_SIZE*2 + MAX_SEQ_SZ*2];  /* before and after dates */
    byte publicKey[MAX_PUBLIC_KEY_SZ]; /* rsa / ntru public key encoded */
    byte ca[MAX_CA_SZ];                /* basic constraint CA true size */
    byte extensions[MAX_EXTENSIONS_SZ]; /* all extensions */
#ifdef WOLFSSL_CERT_EXT
    byte skid[MAX_KID_SZ];             /* Subject Key Identifier extension */
    byte akid[MAX_KID_SZ];             /* Authority Key Identifier extension */
    byte keyUsage[MAX_KEYUSAGE_SZ];    /* Key Usage extension */
    byte extKeyUsage[MAX_EXTKEYUSAGE_SZ]; /* Extended Key Usage extension */
    byte certPolicies[MAX_CERTPOL_NB*MAX_CERTPOL_SZ]; /* Certificate Policies */
#endif
#ifdef WOLFSSL_CERT_REQ
    byte attrib[MAX_ATTRIB_SZ];        /* Cert req attributes encoded */
#endif
#ifdef WOLFSSL_ALT_NAMES
    byte altNames[CTC_MAX_ALT_SIZE];   /* Alternative Names encoded */
#endif
    int  sizeSz;                       /* encoded size length */
    int  versionSz;                    /* encoded version length */
    int  serialSz;                     /* encoded serial length */
    int  sigAlgoSz;                    /* encoded sig alog length */
    int  issuerSz;                     /* encoded issuer length */
    int  subjectSz;                    /* encoded subject length */
    int  validitySz;                   /* encoded validity length */
    int  publicKeySz;                  /* encoded public key length */
    int  caSz;                         /* encoded CA extension length */
#ifdef WOLFSSL_CERT_EXT
    int  skidSz;                       /* encoded SKID extension length */
    int  akidSz;                       /* encoded SKID extension length */
    int  keyUsageSz;                   /* encoded KeyUsage extension length */
    int  extKeyUsageSz;                /* encoded ExtendedKeyUsage extension length */
    int  certPoliciesSz;               /* encoded CertPolicies extension length*/
#endif
#ifdef WOLFSSL_ALT_NAMES
    int  altNamesSz;                   /* encoded AltNames extension length */
#endif
    int  extensionsSz;                 /* encoded extensions total length */
    int  total;                        /* total encoded lengths */
#ifdef WOLFSSL_CERT_REQ
    int  attribSz;
#endif
} DerCert;


#ifdef WOLFSSL_CERT_REQ

/* Write a set header to output */
static word32 SetUTF8String(word32 len, byte* output)
{
    output[0] = ASN_UTF8STRING;
    return SetLength(len, output + 1) + 1;
}

#endif /* WOLFSSL_CERT_REQ */


/* Write a serial number to output */
static int SetSerial(const byte* serial, byte* output)
{
    int length = 0;

    output[length++] = ASN_INTEGER;
    length += SetLength(CTC_SERIAL_SIZE, &output[length]);
    XMEMCPY(&output[length], serial, CTC_SERIAL_SIZE);

    return length + CTC_SERIAL_SIZE;
}

#endif /* defined(WOLFSSL_CERT_GEN) && !defined(NO_RSA) */
#if defined(HAVE_ECC) && (defined(WOLFSSL_CERT_GEN) || defined(WOLFSSL_KEY_GEN))

/* Write a public ECC key to output */
static int SetEccPublicKey(byte* output, ecc_key* key, int with_header)
{
    byte bitString[1 + MAX_LENGTH_SZ + 1];
    int  algoSz;
    int  curveSz;
    int  bitStringSz;
    int  idx;
    word32 pubSz = ECC_BUFSIZE;
#ifdef WOLFSSL_SMALL_STACK
    byte* algo = NULL;
    byte* curve = NULL;
    byte* pub = NULL;
#else
    byte algo[MAX_ALGO_SZ];
    byte curve[MAX_ALGO_SZ];
    byte pub[ECC_BUFSIZE];
#endif

#ifdef WOLFSSL_SMALL_STACK
    pub = (byte*)XMALLOC(ECC_BUFSIZE, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (pub == NULL)
        return MEMORY_E;
#endif

    int ret = wc_ecc_export_x963(key, pub, &pubSz);
    if (ret != 0) {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(pub, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return ret;
    }

    /* headers */
    if (with_header) {
#ifdef WOLFSSL_SMALL_STACK
        curve = (byte*)XMALLOC(MAX_ALGO_SZ, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        if (curve == NULL) {
            XFREE(pub, NULL, DYNAMIC_TYPE_TMP_BUFFER);
            return MEMORY_E;
        }
#endif
        curveSz = SetCurve(key, curve);
        if (curveSz <= 0) {
#ifdef WOLFSSL_SMALL_STACK
            XFREE(curve, NULL, DYNAMIC_TYPE_TMP_BUFFER);
            XFREE(pub,   NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
            return curveSz;
        }

#ifdef WOLFSSL_SMALL_STACK
        algo = (byte*)XMALLOC(MAX_ALGO_SZ, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        if (algo == NULL) {
            XFREE(curve, NULL, DYNAMIC_TYPE_TMP_BUFFER);
            XFREE(pub,   NULL, DYNAMIC_TYPE_TMP_BUFFER);
            return MEMORY_E;
        }
#endif
        algoSz  = SetAlgoID(ECDSAk, algo, oidKeyType, curveSz);

        bitStringSz = SetBitString(pubSz, 0, bitString);

        idx = SetSequence(pubSz + curveSz + bitStringSz + algoSz, output);
        /* algo */
        XMEMCPY(output + idx, algo, algoSz);
        idx += algoSz;
       /* curve */
        XMEMCPY(output + idx, curve, curveSz);
        idx += curveSz;
        /* bit string */
        XMEMCPY(output + idx, bitString, bitStringSz);
        idx += bitStringSz;
    }
    else
        idx = 0;

    /* pub */
    XMEMCPY(output + idx, pub, pubSz);
    idx += pubSz;

#ifdef WOLFSSL_SMALL_STACK
    if (with_header) {
        XFREE(algo,  NULL, DYNAMIC_TYPE_TMP_BUFFER);
        XFREE(curve, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }
    XFREE(pub,   NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return idx;
}


/* returns the size of buffer used, the public ECC key in DER format is stored
   in output buffer
   with_AlgCurve is a flag for when to include a header that has the Algorithm
   and Curve infromation */
int wc_EccPublicKeyToDer(ecc_key* key, byte* output, word32 inLen,
                                                              int with_AlgCurve)
{
    word32 infoSz = 0;
    word32 keySz  = 0;
    int ret;

    if (output == NULL || key == NULL) {
        return BAD_FUNC_ARG;
    }

    if (with_AlgCurve) {
        /* buffer space for algorithm/curve */
        infoSz += MAX_SEQ_SZ;
        infoSz += 2 * MAX_ALGO_SZ;

        /* buffer space for public key sequence */
        infoSz += MAX_SEQ_SZ;
        infoSz += TRAILING_ZERO;
    }

    if ((ret = wc_ecc_export_x963(key, NULL, &keySz)) != LENGTH_ONLY_E) {
        WOLFSSL_MSG("Error in getting ECC public key size");
        return ret;
    }

    if (inLen < keySz + infoSz) {
        return BUFFER_E;
    }

    return SetEccPublicKey(output, key, with_AlgCurve);
}
#endif /* HAVE_ECC && (WOLFSSL_CERT_GEN || WOLFSSL_KEY_GEN) */
#if defined(HAVE_ED25519) && (defined(WOLFSSL_CERT_GEN) || \
                              defined(WOLFSSL_KEY_GEN))

/* Write a public ECC key to output */
static int SetEd25519PublicKey(byte* output, ed25519_key* key, int with_header)
{
    byte bitString[1 + MAX_LENGTH_SZ + 1];
    int  algoSz;
    int  bitStringSz;
    int  idx;
    word32 pubSz = ED25519_PUB_KEY_SIZE;
#ifdef WOLFSSL_SMALL_STACK
    byte* algo = NULL;
    byte* pub = NULL;
#else
    byte algo[MAX_ALGO_SZ];
    byte pub[ED25519_PUB_KEY_SIZE];
#endif

#ifdef WOLFSSL_SMALL_STACK
    pub = (byte*)XMALLOC(ECC_BUFSIZE, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (pub == NULL)
        return MEMORY_E;
#endif

    int ret = wc_ed25519_export_public(key, pub, &pubSz);
    if (ret != 0) {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(pub, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return ret;
    }

    /* headers */
    if (with_header) {
#ifdef WOLFSSL_SMALL_STACK
        algo = (byte*)XMALLOC(MAX_ALGO_SZ, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        if (algo == NULL) {
            XFREE(pub,   NULL, DYNAMIC_TYPE_TMP_BUFFER);
            return MEMORY_E;
        }
#endif
        algoSz  = SetAlgoID(ED25519k, algo, oidKeyType, 0);

        bitStringSz = SetBitString(pubSz, 0, bitString);

        idx = SetSequence(pubSz + bitStringSz + algoSz, output);
        /* algo */
        XMEMCPY(output + idx, algo, algoSz);
        idx += algoSz;
        /* bit string */
        XMEMCPY(output + idx, bitString, bitStringSz);
        idx += bitStringSz;
    }
    else
        idx = 0;

    /* pub */
    XMEMCPY(output + idx, pub, pubSz);
    idx += pubSz;

#ifdef WOLFSSL_SMALL_STACK
    if (with_header) {
        XFREE(algo,  NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }
    XFREE(pub,   NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return idx;
}

int wc_Ed25519PublicKeyToDer(ed25519_key* key, byte* output, word32 inLen,
                                                                    int withAlg)
{
    word32 infoSz = 0;
    word32 keySz  = 0;
    int ret;

    if (output == NULL || key == NULL) {
        return BAD_FUNC_ARG;
    }

    if (withAlg) {
        /* buffer space for algorithm */
        infoSz += MAX_SEQ_SZ;
        infoSz += MAX_ALGO_SZ;

        /* buffer space for public key sequence */
        infoSz += MAX_SEQ_SZ;
        infoSz += TRAILING_ZERO;
    }

    if ((ret = wc_ed25519_export_public(key, output, &keySz)) != BUFFER_E) {
        WOLFSSL_MSG("Error in getting ECC public key size");
        return ret;
    }

    if (inLen < keySz + infoSz) {
        return BUFFER_E;
    }

    return SetEd25519PublicKey(output, key, withAlg);
}
#endif /* HAVE_ED25519 && (WOLFSSL_CERT_GEN || WOLFSSL_KEY_GEN) */
#if defined(WOLFSSL_CERT_GEN) && !defined(NO_RSA)

static INLINE byte itob(int number)
{
    return (byte)number + 0x30;
}


/* write time to output, format */
static void SetTime(struct tm* date, byte* output)
{
    int i = 0;

    output[i++] = itob((date->tm_year % 10000) / 1000);
    output[i++] = itob((date->tm_year % 1000)  /  100);
    output[i++] = itob((date->tm_year % 100)   /   10);
    output[i++] = itob( date->tm_year % 10);

    output[i++] = itob(date->tm_mon / 10);
    output[i++] = itob(date->tm_mon % 10);

    output[i++] = itob(date->tm_mday / 10);
    output[i++] = itob(date->tm_mday % 10);

    output[i++] = itob(date->tm_hour / 10);
    output[i++] = itob(date->tm_hour % 10);

    output[i++] = itob(date->tm_min / 10);
    output[i++] = itob(date->tm_min % 10);

    output[i++] = itob(date->tm_sec / 10);
    output[i++] = itob(date->tm_sec % 10);

    output[i] = 'Z';  /* Zulu profile */
}


#ifdef WOLFSSL_ALT_NAMES

/* Copy Dates from cert, return bytes written */
static int CopyValidity(byte* output, Cert* cert)
{
    int seqSz;

    WOLFSSL_ENTER("CopyValidity");

    /* headers and output */
    seqSz = SetSequence(cert->beforeDateSz + cert->afterDateSz, output);
    XMEMCPY(output + seqSz, cert->beforeDate, cert->beforeDateSz);
    XMEMCPY(output + seqSz + cert->beforeDateSz, cert->afterDate,
                                                 cert->afterDateSz);
    return seqSz + cert->beforeDateSz + cert->afterDateSz;
}

#endif


/* for systems where mktime() doesn't normalize fully */
static void RebuildTime(time_t* in, struct tm* out)
{
    #if defined(FREESCALE_MQX) || defined(FREESCALE_KSDK_MQX)
        out = localtime_r(in, out);
    #else
        (void)in;
        (void)out;
    #endif
}


/* Set Date validity from now until now + daysValid
 * return size in bytes written to output, 0 on error */
static int SetValidity(byte* output, int daysValid)
{
    byte before[MAX_DATE_SIZE];
    byte  after[MAX_DATE_SIZE];

    int beforeSz;
    int afterSz;
    int seqSz;

    time_t     ticks;
    time_t     normalTime;
    struct tm* now;
    struct tm* tmpTime = NULL;
    struct tm  local;

#if defined(NEED_TMP_TIME)
    /* for use with gmtime_r */
    struct tm tmpTimeStorage;
    tmpTime = &tmpTimeStorage;
#else
    (void)tmpTime;
#endif

    ticks = XTIME(0);
    now   = XGMTIME(&ticks, tmpTime);

    if (now == NULL) {
        WOLFSSL_MSG("XGMTIME failed");
        return 0;   /* error */
    }

    /* before now */
    local = *now;
    before[0] = ASN_GENERALIZED_TIME;
    beforeSz  = SetLength(ASN_GEN_TIME_SZ, before + 1) + 1;  /* gen tag */

    /* subtract 1 day for more compliance */
    local.tm_mday -= 1;
    normalTime = mktime(&local);
    RebuildTime(&normalTime, &local);

    /* adjust */
    local.tm_year += 1900;
    local.tm_mon  +=    1;

    SetTime(&local, before + beforeSz);
    beforeSz += ASN_GEN_TIME_SZ;

    /* after now + daysValid */
    local = *now;
    after[0] = ASN_GENERALIZED_TIME;
    afterSz  = SetLength(ASN_GEN_TIME_SZ, after + 1) + 1;  /* gen tag */

    /* add daysValid */
    local.tm_mday += daysValid;
    normalTime = mktime(&local);
    RebuildTime(&normalTime, &local);

    /* adjust */
    local.tm_year += 1900;
    local.tm_mon  +=    1;

    SetTime(&local, after + afterSz);
    afterSz += ASN_GEN_TIME_SZ;

    /* headers and output */
    seqSz = SetSequence(beforeSz + afterSz, output);
    XMEMCPY(output + seqSz, before, beforeSz);
    XMEMCPY(output + seqSz + beforeSz, after, afterSz);

    return seqSz + beforeSz + afterSz;
}


/* ASN Encoded Name field */
typedef struct EncodedName {
    int  nameLen;                /* actual string value length */
    int  totalLen;               /* total encoded length */
    int  type;                   /* type of name */
    int  used;                   /* are we actually using this one */
    byte encoded[CTC_NAME_SIZE * 2]; /* encoding */
} EncodedName;


/* Get Which Name from index */
static const char* GetOneName(CertName* name, int idx)
{
    switch (idx) {
    case 0:
       return name->country;

    case 1:
       return name->state;

    case 2:
       return name->locality;

    case 3:
       return name->sur;

    case 4:
       return name->org;

    case 5:
       return name->unit;

    case 6:
       return name->commonName;

    case 7:
       return name->email;

    default:
       return 0;
    }
}


/* Get Which Name Encoding from index */
static char GetNameType(CertName* name, int idx)
{
    switch (idx) {
    case 0:
       return name->countryEnc;

    case 1:
       return name->stateEnc;

    case 2:
       return name->localityEnc;

    case 3:
       return name->surEnc;

    case 4:
       return name->orgEnc;

    case 5:
       return name->unitEnc;

    case 6:
       return name->commonNameEnc;

    default:
       return 0;
    }
}


/* Get ASN Name from index */
static byte GetNameId(int idx)
{
    switch (idx) {
    case 0:
       return ASN_COUNTRY_NAME;

    case 1:
       return ASN_STATE_NAME;

    case 2:
       return ASN_LOCALITY_NAME;

    case 3:
       return ASN_SUR_NAME;

    case 4:
       return ASN_ORG_NAME;

    case 5:
       return ASN_ORGUNIT_NAME;

    case 6:
       return ASN_COMMON_NAME;

    case 7:
       /* email uses different id type */
       return 0;

    default:
       return 0;
    }
}

/*
 Extensions ::= SEQUENCE OF Extension

 Extension ::= SEQUENCE {
 extnId     OBJECT IDENTIFIER,
 critical   BOOLEAN DEFAULT FALSE,
 extnValue  OCTET STRING }
 */

/* encode all extensions, return total bytes written */
static int SetExtensions(byte* out, word32 outSz, int *IdxInOut,
                         const byte* ext, int extSz)
{
    if (out == NULL || IdxInOut == NULL || ext == NULL)
        return BAD_FUNC_ARG;

    if (outSz < (word32)(*IdxInOut+extSz))
        return BUFFER_E;

    XMEMCPY(&out[*IdxInOut], ext, extSz);  /* extensions */
    *IdxInOut += extSz;

    return *IdxInOut;
}

/* encode extensions header, return total bytes written */
static int SetExtensionsHeader(byte* out, word32 outSz, int extSz)
{
    byte sequence[MAX_SEQ_SZ];
    byte len[MAX_LENGTH_SZ];
    int seqSz, lenSz, idx = 0;

    if (out == NULL)
        return BAD_FUNC_ARG;

    if (outSz < 3)
        return BUFFER_E;

    seqSz = SetSequence(extSz, sequence);

    /* encode extensions length provided */
    lenSz = SetLength(extSz+seqSz, len);

    if (outSz < (word32)(lenSz+seqSz+1))
        return BUFFER_E;

    out[idx++] = ASN_EXTENSIONS; /* extensions id */
    XMEMCPY(&out[idx], len, lenSz);  /* length */
    idx += lenSz;

    XMEMCPY(&out[idx], sequence, seqSz);  /* sequence */
    idx += seqSz;

    return idx;
}


/* encode CA basic constraint true, return total bytes written */
static int SetCa(byte* out, word32 outSz)
{
    static const byte ca[] = { 0x30, 0x0c, 0x06, 0x03, 0x55, 0x1d, 0x13, 0x04,
                               0x05, 0x30, 0x03, 0x01, 0x01, 0xff };

    if (out == NULL)
        return BAD_FUNC_ARG;

    if (outSz < sizeof(ca))
        return BUFFER_E;

    XMEMCPY(out, ca, sizeof(ca));

    return (int)sizeof(ca);
}


#ifdef WOLFSSL_CERT_EXT
/* encode OID and associated value, return total bytes written */
static int SetOidValue(byte* out, word32 outSz, const byte *oid, word32 oidSz,
                       byte *in, word32 inSz)
{
    int idx = 0;

    if (out == NULL || oid == NULL || in == NULL)
        return BAD_FUNC_ARG;

    if (outSz < 3)
        return BUFFER_E;

    /* sequence,  + 1 => byte to put value size */
    idx = SetSequence(inSz + oidSz + 1, out);

    if ((idx + inSz + oidSz + 1) > outSz)
        return BUFFER_E;

    XMEMCPY(out+idx, oid, oidSz);
    idx += oidSz;
    out[idx++] = (byte)inSz;
    XMEMCPY(out+idx, in, inSz);

    return (idx+inSz);
}

/* encode Subject Key Identifier, return total bytes written
 * RFC5280 : non-critical */
static int SetSKID(byte* output, word32 outSz, const byte *input, word32 length)
{
    byte skid_len[1 + MAX_LENGTH_SZ];
    byte skid_enc_len[MAX_LENGTH_SZ];
    int idx = 0, skid_lenSz, skid_enc_lenSz;
    static const byte skid_oid[] = { 0x06, 0x03, 0x55, 0x1d, 0x0e, 0x04 };

    if (output == NULL || input == NULL)
        return BAD_FUNC_ARG;

    /* Octet String header */
    skid_lenSz = SetOctetString(length, skid_len);

    /* length of encoded value */
    skid_enc_lenSz = SetLength(length + skid_lenSz, skid_enc_len);

    if (outSz < 3)
        return BUFFER_E;

    idx = SetSequence(length + sizeof(skid_oid) + skid_lenSz + skid_enc_lenSz,
                      output);

    if ((length + sizeof(skid_oid) + skid_lenSz + skid_enc_lenSz) > outSz)
        return BUFFER_E;

    /* put oid */
    XMEMCPY(output+idx, skid_oid, sizeof(skid_oid));
    idx += sizeof(skid_oid);

    /* put encoded len */
    XMEMCPY(output+idx, skid_enc_len, skid_enc_lenSz);
    idx += skid_enc_lenSz;

    /* put octet header */
    XMEMCPY(output+idx, skid_len, skid_lenSz);
    idx += skid_lenSz;

    /* put value */
    XMEMCPY(output+idx, input, length);
    idx += length;

    return idx;
}

/* encode Authority Key Identifier, return total bytes written
 * RFC5280 : non-critical */
static int SetAKID(byte* output, word32 outSz,
                                         byte *input, word32 length, void* heap)
{
    byte    *enc_val;
    int     ret, enc_valSz;
    static const byte akid_oid[] = { 0x06, 0x03, 0x55, 0x1d, 0x23, 0x04 };
    static const byte akid_cs[] = { 0x80 };

    if (output == NULL || input == NULL)
        return BAD_FUNC_ARG;

    enc_valSz = length + 3 + sizeof(akid_cs);
    enc_val = (byte *)XMALLOC(enc_valSz, heap, DYNAMIC_TYPE_TMP_BUFFER);
    if (enc_val == NULL)
        return MEMORY_E;

    /* sequence for ContentSpec & value */
    ret = SetOidValue(enc_val, enc_valSz, akid_cs, sizeof(akid_cs),
                      input, length);
    if (ret > 0) {
        enc_valSz = ret;

        ret = SetOidValue(output, outSz, akid_oid, sizeof(akid_oid),
                          enc_val, enc_valSz);
    }

    XFREE(enc_val, heap, DYNAMIC_TYPE_TMP_BUFFER);
    return ret;
}

/* encode Key Usage, return total bytes written
 * RFC5280 : critical */
static int SetKeyUsage(byte* output, word32 outSz, word16 input)
{
    byte ku[5];
    int  idx;
    static const byte keyusage_oid[] = { 0x06, 0x03, 0x55, 0x1d, 0x0f,
                                         0x01, 0x01, 0xff, 0x04};
    if (output == NULL)
        return BAD_FUNC_ARG;

    idx = SetBitString16Bit(input, ku);
    return SetOidValue(output, outSz, keyusage_oid, sizeof(keyusage_oid),
                       ku, idx);
}

static int SetOjectIdValue(byte* output, word32 outSz, int* idx,
    const byte* oid, word32 oidSz)
{
    /* verify room */
    if (*idx + 2 + oidSz >= outSz)
        return ASN_PARSE_E;

    *idx += SetObjectId(oidSz, &output[*idx]);
    XMEMCPY(&output[*idx], oid, oidSz);
    *idx += oidSz;

    return 0;
}

/* encode Extended Key Usage (RFC 5280 4.2.1.12), return total bytes written */
static int SetExtKeyUsage(byte* output, word32 outSz, byte input)
{
    int idx = 0, oidListSz = 0, totalSz, ret = 0;
    static const byte extkeyusage_oid[] = { 0x06, 0x03, 0x55, 0x1d, 0x25 };

    if (output == NULL)
        return BAD_FUNC_ARG;

    /* Skip to OID List */
    totalSz = 2 + sizeof(extkeyusage_oid) + 4;
    idx = totalSz;

    /* Build OID List */
    /* If any set, then just use it */
    if (input & EXTKEYUSE_ANY) {
        ret |= SetOjectIdValue(output, outSz, &idx,
            extExtKeyUsageAnyOid, sizeof(extExtKeyUsageAnyOid));
    }
    else {
        if (input & EXTKEYUSE_SERVER_AUTH)
            ret |= SetOjectIdValue(output, outSz, &idx,
                extExtKeyUsageServerAuthOid, sizeof(extExtKeyUsageServerAuthOid));
        if (input & EXTKEYUSE_CLIENT_AUTH)
            ret |= SetOjectIdValue(output, outSz, &idx,
                extExtKeyUsageClientAuthOid, sizeof(extExtKeyUsageClientAuthOid));
        if (input & EXTKEYUSE_CODESIGN)
            ret |= SetOjectIdValue(output, outSz, &idx,
                extExtKeyUsageCodeSigningOid, sizeof(extExtKeyUsageCodeSigningOid));
        if (input & EXTKEYUSE_EMAILPROT)
            ret |= SetOjectIdValue(output, outSz, &idx,
                extExtKeyUsageEmailProtectOid, sizeof(extExtKeyUsageEmailProtectOid));
        if (input & EXTKEYUSE_TIMESTAMP)
            ret |= SetOjectIdValue(output, outSz, &idx,
                extExtKeyUsageTimestampOid, sizeof(extExtKeyUsageTimestampOid));
        if (input & EXTKEYUSE_OCSP_SIGN)
            ret |= SetOjectIdValue(output, outSz, &idx,
                extExtKeyUsageOcspSignOid, sizeof(extExtKeyUsageOcspSignOid));
    }
    if (ret != 0)
        return ASN_PARSE_E;

    /* Calculate Sizes */
    oidListSz = idx - totalSz;
    totalSz = idx - 2; /* exclude first seq/len (2) */

    /* 1. Seq + Total Len (2) */
    idx = SetSequence(totalSz, output);

    /* 2. Object ID (2) */
    XMEMCPY(&output[idx], extkeyusage_oid, sizeof(extkeyusage_oid));
    idx += sizeof(extkeyusage_oid);

    /* 3. Octect String (2) */
    idx += SetOctetString(totalSz - idx, &output[idx]);

    /* 4. Seq + OidListLen (2) */
    idx += SetSequence(oidListSz, &output[idx]);

    /* 5. Oid List (already set in-place above) */
    idx += oidListSz;

    return idx;
}

/* Encode OID string representation to ITU-T X.690 format */
static int EncodePolicyOID(byte *out, word32 *outSz, const char *in, void* heap)
{
    word32 val, idx = 0, nb_val;
    char *token, *str, *ptr;
    word32 len;

    if (out == NULL || outSz == NULL || *outSz < 2 || in == NULL)
        return BAD_FUNC_ARG;

    len = (word32)XSTRLEN(in);

    str = (char *)XMALLOC(len+1, heap, DYNAMIC_TYPE_TMP_BUFFER);
    if (str == NULL)
        return MEMORY_E;

    XSTRNCPY(str, in, len);
    str[len] = 0x00;

    nb_val = 0;

    /* parse value, and set corresponding Policy OID value */
    token = XSTRTOK(str, ".", &ptr);
    while (token != NULL)
    {
        val = (word32)atoi(token);

        if (nb_val == 0) {
            if (val > 2) {
                XFREE(str, heap, DYNAMIC_TYPE_TMP_BUFFER);
                return ASN_OBJECT_ID_E;
            }

            out[idx] = (byte)(40 * val);
        }
        else if (nb_val == 1) {
            if (val > 127) {
                XFREE(str, heap, DYNAMIC_TYPE_TMP_BUFFER);
                return ASN_OBJECT_ID_E;
            }

            if (idx > *outSz) {
                XFREE(str, heap, DYNAMIC_TYPE_TMP_BUFFER);
                return BUFFER_E;
            }

            out[idx++] += (byte)val;
        }
        else {
            word32  tb = 0, x;
            int     i = 0;
            byte    oid[MAX_OID_SZ];

            while (val >= 128) {
                x = val % 128;
                val /= 128;
                oid[i++] = (byte) (((tb++) ? 0x80 : 0) | x);
            }

            if ((idx+(word32)i) > *outSz) {
                XFREE(str, heap, DYNAMIC_TYPE_TMP_BUFFER);
                return BUFFER_E;
            }

            oid[i] = (byte) (((tb++) ? 0x80 : 0) | val);

            /* push value in the right order */
            while (i >= 0)
                out[idx++] = oid[i--];
        }

        token = XSTRTOK(NULL, ".", &ptr);
        nb_val++;
    }

    *outSz = idx;

    XFREE(str, heap, DYNAMIC_TYPE_TMP_BUFFER);
    return 0;
}

/* encode Certificate Policies, return total bytes written
 * each input value must be ITU-T X.690 formatted : a.b.c...
 * input must be an array of values with a NULL terminated for the latest
 * RFC5280 : non-critical */
static int SetCertificatePolicies(byte *output,
                                  word32 outputSz,
                                  char input[MAX_CERTPOL_NB][MAX_CERTPOL_SZ],
                                  word16 nb_certpol,
                                  void* heap)
{
    byte    oid[MAX_OID_SZ],
            der_oid[MAX_CERTPOL_NB][MAX_OID_SZ],
            out[MAX_CERTPOL_SZ];
    word32  oidSz;
    word32  outSz, i = 0, der_oidSz[MAX_CERTPOL_NB];
    int     ret;

    static const byte certpol_oid[] = { 0x06, 0x03, 0x55, 0x1d, 0x20, 0x04 };
    static const byte oid_oid[] = { 0x06 };

    if (output == NULL || input == NULL || nb_certpol > MAX_CERTPOL_NB)
        return BAD_FUNC_ARG;

    for (i = 0; i < nb_certpol; i++) {
        oidSz = sizeof(oid);
        XMEMSET(oid, 0, oidSz);

        ret = EncodePolicyOID(oid, &oidSz, input[i], heap);
        if (ret != 0)
            return ret;

        /* compute sequence value for the oid */
        ret = SetOidValue(der_oid[i], MAX_OID_SZ, oid_oid,
                          sizeof(oid_oid), oid, oidSz);
        if (ret <= 0)
            return ret;
        else
            der_oidSz[i] = (word32)ret;
    }

    /* concatenate oid, keep two byte for sequence/size of the created value */
    for (i = 0, outSz = 2; i < nb_certpol; i++) {
        XMEMCPY(out+outSz, der_oid[i], der_oidSz[i]);
        outSz += der_oidSz[i];
    }

    /* add sequence */
    ret = SetSequence(outSz-2, out);
    if (ret <= 0)
        return ret;

    /* add Policy OID to compute final value */
    return SetOidValue(output, outputSz, certpol_oid, sizeof(certpol_oid),
                      out, outSz);
}
#endif /* WOLFSSL_CERT_EXT */

#ifdef WOLFSSL_ALT_NAMES
/* encode Alternative Names, return total bytes written */
static int SetAltNames(byte *out, word32 outSz, byte *input, word32 length)
{
    if (out == NULL || input == NULL)
        return BAD_FUNC_ARG;

    if (outSz < length)
        return BUFFER_E;

    /* Alternative Names come from certificate or computed by
     * external function, so already encoded. Just copy value */
    XMEMCPY(out, input, length);
    return length;
}
#endif /* WOLFSL_ALT_NAMES */


/* encode CertName into output, return total bytes written */
int SetName(byte* output, word32 outputSz, CertName* name)
{
    int          totalBytes = 0, i, idx;
#ifdef WOLFSSL_SMALL_STACK
    EncodedName* names = NULL;
#else
    EncodedName  names[NAME_ENTRIES];
#endif

    if (output == NULL || name == NULL)
        return BAD_FUNC_ARG;

    if (outputSz < 3)
        return BUFFER_E;

#ifdef WOLFSSL_SMALL_STACK
    names = (EncodedName*)XMALLOC(sizeof(EncodedName) * NAME_ENTRIES, NULL,
                                                       DYNAMIC_TYPE_TMP_BUFFER);
    if (names == NULL)
        return MEMORY_E;
#endif

    for (i = 0; i < NAME_ENTRIES; i++) {
        const char* nameStr = GetOneName(name, i);
        if (nameStr) {
            /* bottom up */
            byte firstLen[1 + MAX_LENGTH_SZ];
            byte secondLen[MAX_LENGTH_SZ];
            byte sequence[MAX_SEQ_SZ];
            byte set[MAX_SET_SZ];

            int email = i == (NAME_ENTRIES - 1) ? 1 : 0;
            int strLen  = (int)XSTRLEN(nameStr);
            int thisLen = strLen;
            int firstSz, secondSz, seqSz, setSz;

            if (strLen == 0) { /* no user data for this item */
                names[i].used = 0;
                continue;
            }

            /* Restrict country code size */
            if (i == 0 && strLen != CTC_COUNTRY_SIZE) {
#ifdef WOLFSSL_SMALL_STACK
                XFREE(names, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
                return ASN_COUNTRY_SIZE_E;
            }

            secondSz = SetLength(strLen, secondLen);
            thisLen += secondSz;
            if (email) {
                thisLen += EMAIL_JOINT_LEN;
                thisLen ++;                               /* id type */
                firstSz  = SetObjectId(EMAIL_JOINT_LEN, firstLen);
            }
            else {
                thisLen++;                                 /* str type */
                thisLen++;                                 /* id  type */
                thisLen += JOINT_LEN;
                firstSz  = SetObjectId(JOINT_LEN + 1, firstLen);
            }
            thisLen += firstSz;

            seqSz = SetSequence(thisLen, sequence);
            thisLen += seqSz;
            setSz = SetSet(thisLen, set);
            thisLen += setSz;

            if (thisLen > (int)sizeof(names[i].encoded)) {
#ifdef WOLFSSL_SMALL_STACK
                XFREE(names, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
                return BUFFER_E;
            }

            /* store it */
            idx = 0;
            /* set */
            XMEMCPY(names[i].encoded, set, setSz);
            idx += setSz;
            /* seq */
            XMEMCPY(names[i].encoded + idx, sequence, seqSz);
            idx += seqSz;
            /* asn object id */
            XMEMCPY(names[i].encoded + idx, firstLen, firstSz);
            idx += firstSz;
            if (email) {
                const byte EMAIL_OID[] = { 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d,
                                           0x01, 0x09, 0x01, 0x16 };
                /* email joint id */
                XMEMCPY(names[i].encoded + idx, EMAIL_OID, sizeof(EMAIL_OID));
                idx += (int)sizeof(EMAIL_OID);
            }
            else {
                /* joint id */
                byte bType = GetNameId(i);
                names[i].encoded[idx++] = 0x55;
                names[i].encoded[idx++] = 0x04;
                /* id type */
                names[i].encoded[idx++] = bType;
                /* str type */
                names[i].encoded[idx++] = GetNameType(name, i);
            }
            /* second length */
            XMEMCPY(names[i].encoded + idx, secondLen, secondSz);
            idx += secondSz;
            /* str value */
            XMEMCPY(names[i].encoded + idx, nameStr, strLen);
            idx += strLen;

            totalBytes += idx;
            names[i].totalLen = idx;
            names[i].used = 1;
        }
        else
            names[i].used = 0;
    }

    /* header */
    idx = SetSequence(totalBytes, output);
    totalBytes += idx;
    if (totalBytes > ASN_NAME_MAX) {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(names, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return BUFFER_E;
    }

    for (i = 0; i < NAME_ENTRIES; i++) {
        if (names[i].used) {
            if (outputSz < (word32)(idx+names[i].totalLen)) {
#ifdef WOLFSSL_SMALL_STACK
                XFREE(names, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
                return BUFFER_E;
            }

            XMEMCPY(output + idx, names[i].encoded, names[i].totalLen);
            idx += names[i].totalLen;
        }
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(names, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return totalBytes;
}

/* encode info from cert into DER encoded format */
static int EncodeCert(Cert* cert, DerCert* der, RsaKey* rsaKey, ecc_key* eccKey,
                      WC_RNG* rng, const byte* ntruKey, word16 ntruSz,
                      ed25519_key* ed25519Key)
{
    int ret;

    (void)eccKey;
    (void)ntruKey;
    (void)ntruSz;
    (void)ed25519Key;

    if (cert == NULL || der == NULL || rng == NULL)
        return BAD_FUNC_ARG;

    /* init */
    XMEMSET(der, 0, sizeof(DerCert));

    /* version */
    der->versionSz = SetMyVersion(cert->version, der->version, TRUE);

    /* serial number */
    ret = wc_RNG_GenerateBlock(rng, cert->serial, CTC_SERIAL_SIZE);
    if (ret != 0)
        return ret;

    cert->serial[0] = 0x01;   /* ensure positive */
    der->serialSz  = SetSerial(cert->serial, der->serial);

    /* signature algo */
    der->sigAlgoSz = SetAlgoID(cert->sigType, der->sigAlgo, oidSigType, 0);
    if (der->sigAlgoSz <= 0)
        return ALGO_ID_E;

    /* public key */
    if (cert->keyType == RSA_KEY) {
        if (rsaKey == NULL)
            return PUBLIC_KEY_E;
        der->publicKeySz = SetRsaPublicKey(der->publicKey, rsaKey,
                                           sizeof(der->publicKey), 1);
        if (der->publicKeySz <= 0)
            return PUBLIC_KEY_E;
    }

#ifdef HAVE_ECC
    if (cert->keyType == ECC_KEY) {
        if (eccKey == NULL)
            return PUBLIC_KEY_E;
        der->publicKeySz = SetEccPublicKey(der->publicKey, eccKey, 1);
        if (der->publicKeySz <= 0)
            return PUBLIC_KEY_E;
    }
#endif /* HAVE_ECC */

#ifdef HAVE_ED25519
    if (cert->keyType == ED25519_KEY) {
        if (ed25519Key == NULL)
            return PUBLIC_KEY_E;
        der->publicKeySz = SetEd25519PublicKey(der->publicKey, ed25519Key, 1);
        if (der->publicKeySz <= 0)
            return PUBLIC_KEY_E;
    }
#endif

#ifdef HAVE_NTRU
    if (cert->keyType == NTRU_KEY) {
        word32 rc;
        word16 encodedSz;

        rc  = ntru_crypto_ntru_encrypt_publicKey2SubjectPublicKeyInfo( ntruSz,
                                                   ntruKey, &encodedSz, NULL);
        if (rc != NTRU_OK)
            return PUBLIC_KEY_E;
        if (encodedSz > MAX_PUBLIC_KEY_SZ)
            return PUBLIC_KEY_E;

        rc  = ntru_crypto_ntru_encrypt_publicKey2SubjectPublicKeyInfo( ntruSz,
                                         ntruKey, &encodedSz, der->publicKey);
        if (rc != NTRU_OK)
            return PUBLIC_KEY_E;

        der->publicKeySz = encodedSz;
    }
#endif /* HAVE_NTRU */

    der->validitySz = 0;
#ifdef WOLFSSL_ALT_NAMES
    /* date validity copy ? */
    if (cert->beforeDateSz && cert->afterDateSz) {
        der->validitySz = CopyValidity(der->validity, cert);
        if (der->validitySz <= 0)
            return DATE_E;
    }
#endif

    /* date validity */
    if (der->validitySz == 0) {
        der->validitySz = SetValidity(der->validity, cert->daysValid);
        if (der->validitySz <= 0)
            return DATE_E;
    }

    /* subject name */
    der->subjectSz = SetName(der->subject, sizeof(der->subject), &cert->subject);
    if (der->subjectSz <= 0)
        return SUBJECT_E;

    /* issuer name */
    der->issuerSz = SetName(der->issuer, sizeof(der->issuer), cert->selfSigned ?
             &cert->subject : &cert->issuer);
    if (der->issuerSz <= 0)
        return ISSUER_E;

    /* set the extensions */
    der->extensionsSz = 0;

    /* CA */
    if (cert->isCA) {
        der->caSz = SetCa(der->ca, sizeof(der->ca));
        if (der->caSz <= 0)
            return CA_TRUE_E;

        der->extensionsSz += der->caSz;
    }
    else
        der->caSz = 0;

#ifdef WOLFSSL_ALT_NAMES
    /* Alternative Name */
    if (cert->altNamesSz) {
        der->altNamesSz = SetAltNames(der->altNames, sizeof(der->altNames),
                                      cert->altNames, cert->altNamesSz);
        if (der->altNamesSz <= 0)
            return ALT_NAME_E;

        der->extensionsSz += der->altNamesSz;
    }
    else
        der->altNamesSz = 0;
#endif

#ifdef WOLFSSL_CERT_EXT
    /* SKID */
    if (cert->skidSz) {
        /* check the provided SKID size */
        if (cert->skidSz > (int)sizeof(der->skid))
            return SKID_E;

        /* Note: different skid buffers sizes for der (MAX_KID_SZ) and
            cert (CTC_MAX_SKID_SIZE). */
        der->skidSz = SetSKID(der->skid, sizeof(der->skid),
                              cert->skid, cert->skidSz);
        if (der->skidSz <= 0)
            return SKID_E;

        der->extensionsSz += der->skidSz;
    }
    else
        der->skidSz = 0;

    /* AKID */
    if (cert->akidSz) {
        /* check the provided AKID size */
        if (cert->akidSz > (int)sizeof(der->akid))
            return AKID_E;

        der->akidSz = SetAKID(der->akid, sizeof(der->akid),
                              cert->akid, cert->akidSz, cert->heap);
        if (der->akidSz <= 0)
            return AKID_E;

        der->extensionsSz += der->akidSz;
    }
    else
        der->akidSz = 0;

    /* Key Usage */
    if (cert->keyUsage != 0){
        der->keyUsageSz = SetKeyUsage(der->keyUsage, sizeof(der->keyUsage),
                                      cert->keyUsage);
        if (der->keyUsageSz <= 0)
            return KEYUSAGE_E;

        der->extensionsSz += der->keyUsageSz;
    }
    else
        der->keyUsageSz = 0;

    /* Extended Key Usage */
    if (cert->extKeyUsage != 0){
        der->extKeyUsageSz = SetExtKeyUsage(der->extKeyUsage,
                                sizeof(der->extKeyUsage), cert->extKeyUsage);
        if (der->extKeyUsageSz <= 0)
            return EXTKEYUSAGE_E;

        der->extensionsSz += der->extKeyUsageSz;
    }
    else
        der->extKeyUsageSz = 0;

    /* Certificate Policies */
    if (cert->certPoliciesNb != 0) {
        der->certPoliciesSz = SetCertificatePolicies(der->certPolicies,
                                                     sizeof(der->certPolicies),
                                                     cert->certPolicies,
                                                     cert->certPoliciesNb,
                                                     cert->heap);
        if (der->certPoliciesSz <= 0)
            return CERTPOLICIES_E;

        der->extensionsSz += der->certPoliciesSz;
    }
    else
        der->certPoliciesSz = 0;
#endif /* WOLFSSL_CERT_EXT */

    /* put extensions */
    if (der->extensionsSz > 0) {

        /* put the start of extensions sequence (ID, Size) */
        der->extensionsSz = SetExtensionsHeader(der->extensions,
                                                sizeof(der->extensions),
                                                der->extensionsSz);
        if (der->extensionsSz <= 0)
            return EXTENSIONS_E;

        /* put CA */
        if (der->caSz) {
            ret = SetExtensions(der->extensions, sizeof(der->extensions),
                                &der->extensionsSz,
                                der->ca, der->caSz);
            if (ret == 0)
                return EXTENSIONS_E;
        }

#ifdef WOLFSSL_ALT_NAMES
        /* put Alternative Names */
        if (der->altNamesSz) {
            ret = SetExtensions(der->extensions, sizeof(der->extensions),
                                &der->extensionsSz,
                                der->altNames, der->altNamesSz);
            if (ret <= 0)
                return EXTENSIONS_E;
        }
#endif

#ifdef WOLFSSL_CERT_EXT
        /* put SKID */
        if (der->skidSz) {
            ret = SetExtensions(der->extensions, sizeof(der->extensions),
                                &der->extensionsSz,
                                der->skid, der->skidSz);
            if (ret <= 0)
                return EXTENSIONS_E;
        }

        /* put AKID */
        if (der->akidSz) {
            ret = SetExtensions(der->extensions, sizeof(der->extensions),
                                &der->extensionsSz,
                                der->akid, der->akidSz);
            if (ret <= 0)
                return EXTENSIONS_E;
        }

        /* put KeyUsage */
        if (der->keyUsageSz) {
            ret = SetExtensions(der->extensions, sizeof(der->extensions),
                                &der->extensionsSz,
                                der->keyUsage, der->keyUsageSz);
            if (ret <= 0)
                return EXTENSIONS_E;
        }

        /* put ExtendedKeyUsage */
        if (der->extKeyUsageSz) {
            ret = SetExtensions(der->extensions, sizeof(der->extensions),
                                &der->extensionsSz,
                                der->extKeyUsage, der->extKeyUsageSz);
            if (ret <= 0)
                return EXTENSIONS_E;
        }

        /* put Certificate Policies */
        if (der->certPoliciesSz) {
            ret = SetExtensions(der->extensions, sizeof(der->extensions),
                                &der->extensionsSz,
                                der->certPolicies, der->certPoliciesSz);
            if (ret <= 0)
                return EXTENSIONS_E;
        }
#endif /* WOLFSSL_CERT_EXT */
    }

    der->total = der->versionSz + der->serialSz + der->sigAlgoSz +
        der->publicKeySz + der->validitySz + der->subjectSz + der->issuerSz +
        der->extensionsSz;

    return 0;
}


/* write DER encoded cert to buffer, size already checked */
static int WriteCertBody(DerCert* der, byte* buffer)
{
    int idx;

    /* signed part header */
    idx = SetSequence(der->total, buffer);
    /* version */
    XMEMCPY(buffer + idx, der->version, der->versionSz);
    idx += der->versionSz;
    /* serial */
    XMEMCPY(buffer + idx, der->serial, der->serialSz);
    idx += der->serialSz;
    /* sig algo */
    XMEMCPY(buffer + idx, der->sigAlgo, der->sigAlgoSz);
    idx += der->sigAlgoSz;
    /* issuer */
    XMEMCPY(buffer + idx, der->issuer, der->issuerSz);
    idx += der->issuerSz;
    /* validity */
    XMEMCPY(buffer + idx, der->validity, der->validitySz);
    idx += der->validitySz;
    /* subject */
    XMEMCPY(buffer + idx, der->subject, der->subjectSz);
    idx += der->subjectSz;
    /* public key */
    XMEMCPY(buffer + idx, der->publicKey, der->publicKeySz);
    idx += der->publicKeySz;
    if (der->extensionsSz) {
        /* extensions */
        XMEMCPY(buffer + idx, der->extensions, min(der->extensionsSz,
                                                   (int)sizeof(der->extensions)));
        idx += der->extensionsSz;
    }

    return idx;
}


/* Make RSA signature from buffer (sz), write to sig (sigSz) */
static int MakeSignature(CertSignCtx* certSignCtx, const byte* buffer, int sz,
    byte* sig, int sigSz, RsaKey* rsaKey, ecc_key* eccKey,
    ed25519_key* ed25519Key, WC_RNG* rng, int sigAlgoType, void* heap)
{
    int digestSz = 0, typeH = 0, ret = 0;

    (void)digestSz;
    (void)typeH;
    (void)buffer;
    (void)sz;
    (void)sig;
    (void)sigSz;
    (void)rsaKey;
    (void)eccKey;
    (void)ed25519Key;
    (void)rng;

    switch (certSignCtx->state) {
    case CERTSIGN_STATE_BEGIN:
    case CERTSIGN_STATE_DIGEST:

        certSignCtx->state = CERTSIGN_STATE_DIGEST;
        certSignCtx->digest = (byte*)XMALLOC(WC_MAX_DIGEST_SIZE, heap,
            DYNAMIC_TYPE_TMP_BUFFER);
        if (certSignCtx->digest == NULL) {
            ret = MEMORY_E; goto exit_ms;
        }

        ret = HashForSignature(buffer, sz, sigAlgoType, certSignCtx->digest,
                               &typeH, &digestSz, 0);
        /* set next state, since WC_PENDING rentry for these are not "call again" */
        certSignCtx->state = CERTSIGN_STATE_ENCODE;
        if (ret != 0) {
            goto exit_ms;
        }
        FALL_THROUGH;

    case CERTSIGN_STATE_ENCODE:
    #ifndef NO_RSA
        if (rsaKey) {
            certSignCtx->encSig = (byte*)XMALLOC(MAX_DER_DIGEST_SZ, heap,
                DYNAMIC_TYPE_TMP_BUFFER);
            if (certSignCtx->encSig == NULL) {
                ret = MEMORY_E; goto exit_ms;
            }

            /* signature */
            certSignCtx->encSigSz = wc_EncodeSignature(certSignCtx->encSig,
                                          certSignCtx->digest, digestSz, typeH);
        }
    #endif /* !NO_RSA */
        FALL_THROUGH;

    case CERTSIGN_STATE_DO:
        certSignCtx->state = CERTSIGN_STATE_DO;
        ret = ALGO_ID_E; /* default to error */

    #ifndef NO_RSA
        if (rsaKey) {
            /* signature */
            ret = wc_RsaSSL_Sign(certSignCtx->encSig, certSignCtx->encSigSz,
                                 sig, sigSz, rsaKey, rng);
        }
    #endif /* !NO_RSA */

    #ifdef HAVE_ECC
        if (!rsaKey && eccKey) {
            word32 outSz = sigSz;

            ret = wc_ecc_sign_hash(certSignCtx->digest, digestSz,
                                   sig, &outSz, rng, eccKey);
            if (ret == 0)
                ret = outSz;
        }
    #endif /* HAVE_ECC */

    #ifdef HAVE_ED25519
        if (!rsaKey && !eccKey && ed25519Key) {
            word32 outSz = sigSz;

            ret = wc_ed25519_sign_msg(buffer, sz, sig, &outSz, ed25519Key);
            if (ret == 0)
                ret = outSz;
        }
    #endif /* HAVE_ECC */
        break;
    }

exit_ms:

    if (ret == WC_PENDING_E) {
        return ret;
    }

#ifndef NO_RSA
    if (rsaKey) {
        XFREE(certSignCtx->encSig, heap, DYNAMIC_TYPE_TMP_BUFFER);
    }
#endif /* !NO_RSA */

    XFREE(certSignCtx->digest, heap, DYNAMIC_TYPE_TMP_BUFFER);
    certSignCtx->digest = NULL;

    /* reset state */
    certSignCtx->state = CERTSIGN_STATE_BEGIN;

    return ret;
}


/* add signature to end of buffer, size of buffer assumed checked, return
   new length */
static int AddSignature(byte* buffer, int bodySz, const byte* sig, int sigSz,
                        int sigAlgoType)
{
    byte seq[MAX_SEQ_SZ];
    int  idx = bodySz, seqSz;

    /* algo */
    idx += SetAlgoID(sigAlgoType, buffer + idx, oidSigType, 0);
    /* bit string */
    idx += SetBitString(sigSz, 0, buffer + idx);
    /* signature */
    XMEMCPY(buffer + idx, sig, sigSz);
    idx += sigSz;

    /* make room for overall header */
    seqSz = SetSequence(idx, seq);
    XMEMMOVE(buffer + seqSz, buffer, idx);
    XMEMCPY(buffer, seq, seqSz);

    return idx + seqSz;
}


/* Make an x509 Certificate v3 any key type from cert input, write to buffer */
static int MakeAnyCert(Cert* cert, byte* derBuffer, word32 derSz,
                       RsaKey* rsaKey, ecc_key* eccKey, WC_RNG* rng,
                       const byte* ntruKey, word16 ntruSz,
                       ed25519_key* ed25519Key)
{
    int ret;
#ifdef WOLFSSL_SMALL_STACK
    DerCert* der;
#else
    DerCert der[1];
#endif

    cert->keyType = eccKey ? ECC_KEY : (rsaKey ? RSA_KEY :
                                         (ed25519Key ? ED25519_KEY : NTRU_KEY));

#ifdef WOLFSSL_SMALL_STACK
    der = (DerCert*)XMALLOC(sizeof(DerCert), NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (der == NULL)
        return MEMORY_E;
#endif

    ret = EncodeCert(cert, der, rsaKey, eccKey, rng, ntruKey, ntruSz,
                     ed25519Key);
    if (ret == 0) {
        if (der->total + MAX_SEQ_SZ * 2 > (int)derSz)
            ret = BUFFER_E;
        else
            ret = cert->bodySz = WriteCertBody(der, derBuffer);
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(der, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return ret;
}


/* Make an x509 Certificate v3 RSA or ECC from cert input, write to buffer */
int wc_MakeCert_ex(Cert* cert, byte* derBuffer, word32 derSz, int keyType,
                   void* key, WC_RNG* rng)
{
    RsaKey* rsaKey = NULL;
    ecc_key* eccKey = NULL;
    ed25519_key* ed25519Key = NULL;

    if (keyType == RSA_TYPE)
        rsaKey = (RsaKey*)key;
    else if (keyType == ECC_TYPE)
        eccKey = (ecc_key*)key;
    else if (keyType == ED25519_TYPE)
        ed25519Key = (ed25519_key*)key;

    return MakeAnyCert(cert, derBuffer, derSz, rsaKey, eccKey, rng, NULL, 0,
                       ed25519Key);
}
/* Make an x509 Certificate v3 RSA or ECC from cert input, write to buffer */
int wc_MakeCert(Cert* cert, byte* derBuffer, word32 derSz, RsaKey* rsaKey,
             ecc_key* eccKey, WC_RNG* rng)
{
    return MakeAnyCert(cert, derBuffer, derSz, rsaKey, eccKey, rng, NULL, 0,
                       NULL);
}


#ifdef HAVE_NTRU

int wc_MakeNtruCert(Cert* cert, byte* derBuffer, word32 derSz,
                  const byte* ntruKey, word16 keySz, WC_RNG* rng)
{
    return MakeAnyCert(cert, derBuffer, derSz, NULL, NULL, rng, ntruKey, keySz);
}

#endif /* HAVE_NTRU */


#ifdef WOLFSSL_CERT_REQ

static int SetReqAttrib(byte* output, char* pw, int extSz)
{
    static const byte cpOid[] =
        { ASN_OBJECT_ID, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01,
                         0x09, 0x07 };
    static const byte erOid[] =
        { ASN_OBJECT_ID, 0x09, 0x2a, 0x86, 0x48, 0x86, 0xf7, 0x0d, 0x01,
                         0x09, 0x0e };

    int sz      = 0; /* overall size */
    int cpSz    = 0; /* Challenge Password section size */
    int cpSeqSz = 0;
    int cpSetSz = 0;
    int cpStrSz = 0;
    int pwSz    = 0;
    int erSz    = 0; /* Extension Request section size */
    int erSeqSz = 0;
    int erSetSz = 0;
    byte cpSeq[MAX_SEQ_SZ];
    byte cpSet[MAX_SET_SZ];
    byte cpStr[MAX_PRSTR_SZ];
    byte erSeq[MAX_SEQ_SZ];
    byte erSet[MAX_SET_SZ];

    output[0] = 0xa0;
    sz++;

    if (pw && pw[0]) {
        pwSz = (int)XSTRLEN(pw);
        cpStrSz = SetUTF8String(pwSz, cpStr);
        cpSetSz = SetSet(cpStrSz + pwSz, cpSet);
        cpSeqSz = SetSequence(sizeof(cpOid) + cpSetSz + cpStrSz + pwSz, cpSeq);
        cpSz = cpSeqSz + sizeof(cpOid) + cpSetSz + cpStrSz + pwSz;
    }

    if (extSz) {
        erSetSz = SetSet(extSz, erSet);
        erSeqSz = SetSequence(erSetSz + sizeof(erOid) + extSz, erSeq);
        erSz = extSz + erSetSz + erSeqSz + sizeof(erOid);
    }

    /* Put the pieces together. */
    sz += SetLength(cpSz + erSz, &output[sz]);

    if (cpSz) {
        XMEMCPY(&output[sz], cpSeq, cpSeqSz);
        sz += cpSeqSz;
        XMEMCPY(&output[sz], cpOid, sizeof(cpOid));
        sz += sizeof(cpOid);
        XMEMCPY(&output[sz], cpSet, cpSetSz);
        sz += cpSetSz;
        XMEMCPY(&output[sz], cpStr, cpStrSz);
        sz += cpStrSz;
        XMEMCPY(&output[sz], pw, pwSz);
        sz += pwSz;
    }

    if (erSz) {
        XMEMCPY(&output[sz], erSeq, erSeqSz);
        sz += erSeqSz;
        XMEMCPY(&output[sz], erOid, sizeof(erOid));
        sz += sizeof(erOid);
        XMEMCPY(&output[sz], erSet, erSetSz);
        sz += erSetSz;
        /* The actual extension data will be tacked onto the output later. */
    }

    return sz;
}


/* encode info from cert into DER encoded format */
static int EncodeCertReq(Cert* cert, DerCert* der, RsaKey* rsaKey,
                         ecc_key* eccKey, ed25519_key* ed25519Key)
{
    (void)eccKey;
    (void)ed25519Key;

    if (cert == NULL || der == NULL)
        return BAD_FUNC_ARG;

    /* init */
    XMEMSET(der, 0, sizeof(DerCert));

    /* version */
    der->versionSz = SetMyVersion(cert->version, der->version, FALSE);

    /* subject name */
    der->subjectSz = SetName(der->subject, sizeof(der->subject), &cert->subject);
    if (der->subjectSz <= 0)
        return SUBJECT_E;

    /* public key */
    if (cert->keyType == RSA_KEY) {
        if (rsaKey == NULL)
            return PUBLIC_KEY_E;
        der->publicKeySz = SetRsaPublicKey(der->publicKey, rsaKey,
                                           sizeof(der->publicKey), 1);
        if (der->publicKeySz <= 0)
            return PUBLIC_KEY_E;
    }

#ifdef HAVE_ECC
    if (cert->keyType == ECC_KEY) {
        if (eccKey == NULL)
            return PUBLIC_KEY_E;
        der->publicKeySz = SetEccPublicKey(der->publicKey, eccKey, 1);
        if (der->publicKeySz <= 0)
            return PUBLIC_KEY_E;
    }
#endif /* HAVE_ECC */

#ifdef HAVE_ED25519
    if (cert->keyType == ED25519_KEY) {
        if (ed25519Key == NULL)
            return PUBLIC_KEY_E;
        der->publicKeySz = SetEd25519PublicKey(der->publicKey, ed25519Key, 1);
        if (der->publicKeySz <= 0)
            return PUBLIC_KEY_E;
    }
#endif /* HAVE_ED25519 */

    /* set the extensions */
    der->extensionsSz = 0;

    /* CA */
    if (cert->isCA) {
        der->caSz = SetCa(der->ca, sizeof(der->ca));
        if (der->caSz <= 0)
            return CA_TRUE_E;

        der->extensionsSz += der->caSz;
    }
    else
        der->caSz = 0;

#ifdef WOLFSSL_CERT_EXT
    /* SKID */
    if (cert->skidSz) {
        /* check the provided SKID size */
        if (cert->skidSz > (int)sizeof(der->skid))
            return SKID_E;

        der->skidSz = SetSKID(der->skid, sizeof(der->skid),
                              cert->skid, cert->skidSz);
        if (der->skidSz <= 0)
            return SKID_E;

        der->extensionsSz += der->skidSz;
    }
    else
        der->skidSz = 0;

    /* Key Usage */
    if (cert->keyUsage != 0){
        der->keyUsageSz = SetKeyUsage(der->keyUsage, sizeof(der->keyUsage),
                                      cert->keyUsage);
        if (der->keyUsageSz <= 0)
            return KEYUSAGE_E;

        der->extensionsSz += der->keyUsageSz;
    }
    else
        der->keyUsageSz = 0;

    /* Extended Key Usage */
    if (cert->extKeyUsage != 0){
        der->extKeyUsageSz = SetExtKeyUsage(der->extKeyUsage,
                                sizeof(der->extKeyUsage), cert->extKeyUsage);
        if (der->extKeyUsageSz <= 0)
            return EXTKEYUSAGE_E;

        der->extensionsSz += der->extKeyUsageSz;
    }
    else
        der->extKeyUsageSz = 0;

#endif /* WOLFSSL_CERT_EXT */

    /* put extensions */
    if (der->extensionsSz > 0) {
        int ret;

        /* put the start of sequence (ID, Size) */
        der->extensionsSz = SetSequence(der->extensionsSz, der->extensions);
        if (der->extensionsSz <= 0)
            return EXTENSIONS_E;

        /* put CA */
        if (der->caSz) {
            ret = SetExtensions(der->extensions, sizeof(der->extensions),
                                &der->extensionsSz,
                                der->ca, der->caSz);
            if (ret <= 0)
                return EXTENSIONS_E;
        }

#ifdef WOLFSSL_CERT_EXT
        /* put SKID */
        if (der->skidSz) {
            ret = SetExtensions(der->extensions, sizeof(der->extensions),
                                &der->extensionsSz,
                                der->skid, der->skidSz);
            if (ret <= 0)
                return EXTENSIONS_E;
        }

        /* put AKID */
        if (der->akidSz) {
            ret = SetExtensions(der->extensions, sizeof(der->extensions),
                                &der->extensionsSz,
                                der->akid, der->akidSz);
            if (ret <= 0)
                return EXTENSIONS_E;
        }

        /* put KeyUsage */
        if (der->keyUsageSz) {
            ret = SetExtensions(der->extensions, sizeof(der->extensions),
                                &der->extensionsSz,
                                der->keyUsage, der->keyUsageSz);
            if (ret <= 0)
                return EXTENSIONS_E;
        }

        /* put ExtendedKeyUsage */
        if (der->extKeyUsageSz) {
            ret = SetExtensions(der->extensions, sizeof(der->extensions),
                                &der->extensionsSz,
                                der->extKeyUsage, der->extKeyUsageSz);
            if (ret <= 0)
                return EXTENSIONS_E;
        }

#endif /* WOLFSSL_CERT_EXT */
    }

    der->attribSz = SetReqAttrib(der->attrib,
                                 cert->challengePw, der->extensionsSz);
    if (der->attribSz <= 0)
        return REQ_ATTRIBUTE_E;

    der->total = der->versionSz + der->subjectSz + der->publicKeySz +
        der->extensionsSz + der->attribSz;

    return 0;
}


/* write DER encoded cert req to buffer, size already checked */
static int WriteCertReqBody(DerCert* der, byte* buffer)
{
    int idx;

    /* signed part header */
    idx = SetSequence(der->total, buffer);
    /* version */
    XMEMCPY(buffer + idx, der->version, der->versionSz);
    idx += der->versionSz;
    /* subject */
    XMEMCPY(buffer + idx, der->subject, der->subjectSz);
    idx += der->subjectSz;
    /* public key */
    XMEMCPY(buffer + idx, der->publicKey, der->publicKeySz);
    idx += der->publicKeySz;
    /* attributes */
    XMEMCPY(buffer + idx, der->attrib, der->attribSz);
    idx += der->attribSz;
    /* extensions */
    if (der->extensionsSz) {
        XMEMCPY(buffer + idx, der->extensions, min(der->extensionsSz,
                                               (int)sizeof(der->extensions)));
        idx += der->extensionsSz;
    }

    return idx;
}


static int MakeCertReq(Cert* cert, byte* derBuffer, word32 derSz,
                   RsaKey* rsaKey, ecc_key* eccKey, ed25519_key* ed25519Key)
{
    int ret;
#ifdef WOLFSSL_SMALL_STACK
    DerCert* der;
#else
    DerCert der[1];
#endif

    cert->keyType = eccKey ? ECC_KEY : (ed25519Key ? ED25519_KEY : RSA_KEY);

#ifdef WOLFSSL_SMALL_STACK
    der = (DerCert*)XMALLOC(sizeof(DerCert), NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (der == NULL)
        return MEMORY_E;
#endif

    ret = EncodeCertReq(cert, der, rsaKey, eccKey, ed25519Key);

    if (ret == 0) {
        if (der->total + MAX_SEQ_SZ * 2 > (int)derSz)
            ret = BUFFER_E;
        else
            ret = cert->bodySz = WriteCertReqBody(der, derBuffer);
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(der, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return ret;
}

int wc_MakeCertReq_ex(Cert* cert, byte* derBuffer, word32 derSz, int keyType,
                      void* key)
{
    RsaKey* rsaKey = NULL;
    ecc_key* eccKey = NULL;
    ed25519_key* ed25519Key = NULL;

    if (keyType == RSA_TYPE)
        rsaKey = (RsaKey*)key;
    else if (keyType == ECC_TYPE)
        eccKey = (ecc_key*)key;
    else if (keyType == ED25519_TYPE)
        ed25519Key = (ed25519_key*)key;

    return MakeCertReq(cert, derBuffer, derSz, rsaKey, eccKey, ed25519Key);
}

int wc_MakeCertReq(Cert* cert, byte* derBuffer, word32 derSz,
                   RsaKey* rsaKey, ecc_key* eccKey)
{
    return MakeCertReq(cert, derBuffer, derSz, rsaKey, eccKey, NULL);
}
#endif /* WOLFSSL_CERT_REQ */


static int SignCert(int requestSz, int sType, byte* buffer, word32 buffSz,
                    RsaKey* rsaKey, ecc_key* eccKey, ed25519_key* ed25519Key,
                    WC_RNG* rng)
{
    int sigSz = 0;
    void* heap = NULL;
    CertSignCtx* certSignCtx = NULL;
#ifndef WOLFSSL_ASYNC_CRYPT
    CertSignCtx  certSignCtx_lcl;
    certSignCtx = &certSignCtx_lcl;
    XMEMSET(certSignCtx, 0, sizeof(CertSignCtx));
#endif

    if (requestSz < 0)
        return requestSz;

    /* locate ctx */
    if (rsaKey) {
    #ifndef NO_RSA
    #ifdef WOLFSSL_ASYNC_CRYPT
        certSignCtx = &rsaKey->certSignCtx;
    #endif
        heap = rsaKey->heap;
    #else
        return NOT_COMPILED_IN;
    #endif /* NO_RSA */
    }
    else if (eccKey) {
    #ifdef HAVE_ECC
    #ifdef WOLFSSL_ASYNC_CRYPT
        certSignCtx = &eccKey->certSignCtx;
    #endif
        heap = eccKey->heap;
    #else
        return NOT_COMPILED_IN;
    #endif /* HAVE_ECC */
    }

#ifdef WOLFSSL_ASYNC_CRYPT
    if (certSignCtx == NULL) {
        return BAD_FUNC_ARG;
    }
#endif

    if (certSignCtx->sig == NULL) {
        certSignCtx->sig = (byte*)XMALLOC(MAX_ENCODED_SIG_SZ, heap,
            DYNAMIC_TYPE_TMP_BUFFER);
        if (certSignCtx->sig == NULL)
            return MEMORY_E;
    }

    sigSz = MakeSignature(certSignCtx, buffer, requestSz, certSignCtx->sig,
        MAX_ENCODED_SIG_SZ, rsaKey, eccKey, ed25519Key, rng, sType, heap);
    if (sigSz == WC_PENDING_E)
        return sigSz;

    if (sigSz >= 0) {
        if (requestSz + MAX_SEQ_SZ * 2 + sigSz > (int)buffSz)
            sigSz = BUFFER_E;
        else
            sigSz = AddSignature(buffer, requestSz, certSignCtx->sig, sigSz, sType);
    }

    XFREE(certSignCtx->sig, heap, DYNAMIC_TYPE_TMP_BUFFER);
    certSignCtx->sig = NULL;

    return sigSz;
}

int wc_SignCert_ex(int requestSz, int sType, byte* buffer, word32 buffSz,
                   int keyType, void* key, WC_RNG* rng)
{
    RsaKey* rsaKey = NULL;
    ecc_key* eccKey = NULL;
    ed25519_key* ed25519Key = NULL;

    if (keyType == RSA_TYPE)
        rsaKey = (RsaKey*)key;
    else if (keyType == ECC_TYPE)
        eccKey = (ecc_key*)key;
    else if (keyType == ED25519_TYPE)
        ed25519Key = (ed25519_key*)key;

    return SignCert(requestSz, sType, buffer, buffSz, rsaKey, eccKey,
                    ed25519Key, rng);
}

int wc_SignCert(int requestSz, int sType, byte* buffer, word32 buffSz,
                RsaKey* rsaKey, ecc_key* eccKey, WC_RNG* rng)
{
    return SignCert(requestSz, sType, buffer, buffSz, rsaKey, eccKey, NULL,
                    rng);
}

int wc_MakeSelfCert(Cert* cert, byte* buffer, word32 buffSz,
                    RsaKey* key, WC_RNG* rng)
{
    int ret;

    ret = wc_MakeCert(cert, buffer, buffSz, key, NULL, rng);
    if (ret < 0)
        return ret;

    return wc_SignCert(cert->bodySz, cert->sigType,
                       buffer, buffSz, key, NULL, rng);
}


#ifdef WOLFSSL_CERT_EXT

/* Set KID from RSA or ECC public key */
static int SetKeyIdFromPublicKey(Cert *cert, RsaKey *rsakey, ecc_key *eckey,
                                 byte *ntruKey, word16 ntruKeySz,
                                 ed25519_key* ed25519Key, int kid_type)
{
    byte    *buffer;
    int     bufferSz, ret;

#ifndef HAVE_NTRU
    (void)ntruKeySz;
#endif

    if (cert == NULL ||
        (rsakey == NULL && eckey == NULL && ntruKey == NULL &&
                                            ed25519Key == NULL) ||
        (rsakey != NULL && eckey != NULL) ||
        (rsakey != NULL && ntruKey != NULL) ||
        (ntruKey != NULL && eckey != NULL) ||
        (kid_type != SKID_TYPE && kid_type != AKID_TYPE))
        return BAD_FUNC_ARG;

    buffer = (byte *)XMALLOC(MAX_PUBLIC_KEY_SZ, cert->heap,
                                                       DYNAMIC_TYPE_TMP_BUFFER);
    if (buffer == NULL)
        return MEMORY_E;

    /* RSA public key */
    if (rsakey != NULL)
        bufferSz = SetRsaPublicKey(buffer, rsakey, MAX_PUBLIC_KEY_SZ, 0);
#ifdef HAVE_ECC
    /* ECC public key */
    else if (eckey != NULL)
        bufferSz = SetEccPublicKey(buffer, eckey, 0);
#endif /* HAVE_ECC */
#ifdef HAVE_NTRU
    /* NTRU public key */
    else if (ntruKey != NULL) {
        bufferSz = MAX_PUBLIC_KEY_SZ;
        ret = ntru_crypto_ntru_encrypt_publicKey2SubjectPublicKeyInfo(
                        ntruKeySz, ntruKey, (word16 *)(&bufferSz), buffer);
        if (ret != NTRU_OK)
            bufferSz = -1;
    }
#endif
#ifdef HAVE_ED25519
    /* ED25519 public key */
    else if (ed25519Key != NULL)
        bufferSz = SetEd25519PublicKey(buffer, ed25519Key, 0);
#endif /* HAVE_ECC */
    else
        bufferSz = -1;

    if (bufferSz <= 0) {
        XFREE(buffer, cert->heap, DYNAMIC_TYPE_TMP_BUFFER);
        return PUBLIC_KEY_E;
    }

    /* Compute SKID by hashing public key */
#ifdef NO_SHA
    if (kid_type == SKID_TYPE) {
        ret = wc_Sha256Hash(buffer, bufferSz, cert->skid);
        cert->skidSz = SHA256_DIGEST_SIZE;
    }
    else if (kid_type == AKID_TYPE) {
        ret = wc_Sha256Hash(buffer, bufferSz, cert->akid);
        cert->akidSz = SHA256_DIGEST_SIZE;
    }
    else
        ret = BAD_FUNC_ARG;
#else /* NO_SHA */
    if (kid_type == SKID_TYPE) {
        ret = wc_ShaHash(buffer, bufferSz, cert->skid);
        cert->skidSz = SHA_DIGEST_SIZE;
    }
    else if (kid_type == AKID_TYPE) {
        ret = wc_ShaHash(buffer, bufferSz, cert->akid);
        cert->akidSz = SHA_DIGEST_SIZE;
    }
    else
        ret = BAD_FUNC_ARG;
#endif /* NO_SHA */

    XFREE(buffer, cert->heap, DYNAMIC_TYPE_TMP_BUFFER);
    return ret;
}

int wc_SetSubjectKeyIdFromPublicKey_ex(Cert *cert, int keyType, void* key)
{
    RsaKey* rsaKey = NULL;
    ecc_key* eccKey = NULL;
    ed25519_key* ed25519Key = NULL;

    if (keyType == RSA_TYPE)
        rsaKey = (RsaKey*)key;
    else if (keyType == ECC_TYPE)
        eccKey = (ecc_key*)key;
    else if (keyType == ED25519_TYPE)
        ed25519Key = (ed25519_key*)key;

    return SetKeyIdFromPublicKey(cert, rsaKey, eccKey, NULL, 0, ed25519Key,
                                 SKID_TYPE);
}

/* Set SKID from RSA or ECC public key */
int wc_SetSubjectKeyIdFromPublicKey(Cert *cert, RsaKey *rsakey, ecc_key *eckey)
{
    return SetKeyIdFromPublicKey(cert, rsakey, eckey, NULL, 0, NULL, SKID_TYPE);
}

#ifdef HAVE_NTRU
/* Set SKID from NTRU public key */
int wc_SetSubjectKeyIdFromNtruPublicKey(Cert *cert,
                                        byte *ntruKey, word16 ntruKeySz)
{
    return SetKeyIdFromPublicKey(cert, NULL,NULL,ntruKey, ntruKeySz, NULL,
                                 SKID_TYPE);
}
#endif

int wc_SetAuthKeyIdFromPublicKey_ex(Cert *cert, int keyType, void* key)
{
    RsaKey* rsaKey = NULL;
    ecc_key* eccKey = NULL;
    ed25519_key* ed25519Key = NULL;

    if (keyType == RSA_TYPE)
        rsaKey = (RsaKey*)key;
    else if (keyType == ECC_TYPE)
        eccKey = (ecc_key*)key;
    else if (keyType == ED25519_TYPE)
        ed25519Key = (ed25519_key*)key;

    return SetKeyIdFromPublicKey(cert, rsaKey, eccKey, NULL, 0, ed25519Key,
                                 AKID_TYPE);
}

/* Set SKID from RSA or ECC public key */
int wc_SetAuthKeyIdFromPublicKey(Cert *cert, RsaKey *rsakey, ecc_key *eckey)
{
    return SetKeyIdFromPublicKey(cert, rsakey, eckey, NULL, 0, NULL, AKID_TYPE);
}


#ifndef NO_FILESYSTEM

/* Set SKID from public key file in PEM */
int wc_SetSubjectKeyId(Cert *cert, const char* file)
{
    int     ret, derSz;
    byte*   der;
    word32  idx;
    RsaKey  *rsakey = NULL;
    ecc_key *eckey = NULL;

    if (cert == NULL || file == NULL)
        return BAD_FUNC_ARG;

    der = (byte*)XMALLOC(MAX_PUBLIC_KEY_SZ, cert->heap, DYNAMIC_TYPE_CERT);
    if (der == NULL) {
        WOLFSSL_MSG("wc_SetSubjectKeyId memory Problem");
        return MEMORY_E;
    }

    derSz = wolfSSL_PemPubKeyToDer(file, der, MAX_PUBLIC_KEY_SZ);
    if (derSz <= 0)
    {
        XFREE(der, cert->heap, DYNAMIC_TYPE_CERT);
        return derSz;
    }

    /* Load PubKey in internal structure */
    rsakey = (RsaKey*) XMALLOC(sizeof(RsaKey), cert->heap, DYNAMIC_TYPE_RSA);
    if (rsakey == NULL) {
        XFREE(der, cert->heap, DYNAMIC_TYPE_CERT);
        return MEMORY_E;
    }

    if (wc_InitRsaKey(rsakey, cert->heap) != 0) {
        WOLFSSL_MSG("wc_InitRsaKey failure");
        XFREE(rsakey, cert->heap, DYNAMIC_TYPE_RSA);
        XFREE(der, cert->heap, DYNAMIC_TYPE_CERT);
        return MEMORY_E;
    }

    idx = 0;
    ret = wc_RsaPublicKeyDecode(der, &idx, rsakey, derSz);
    if (ret != 0) {
        WOLFSSL_MSG("wc_RsaPublicKeyDecode failed");
        wc_FreeRsaKey(rsakey);
        XFREE(rsakey, cert->heap, DYNAMIC_TYPE_RSA);
        rsakey = NULL;
#ifdef HAVE_ECC
        /* Check to load ecc public key */
        eckey = (ecc_key*) XMALLOC(sizeof(ecc_key), cert->heap,
                                                              DYNAMIC_TYPE_ECC);
        if (eckey == NULL) {
            XFREE(der, cert->heap, DYNAMIC_TYPE_CERT);
            return MEMORY_E;
        }

        if (wc_ecc_init(eckey) != 0) {
            WOLFSSL_MSG("wc_ecc_init failure");
            wc_ecc_free(eckey);
            XFREE(eckey, cert->heap, DYNAMIC_TYPE_ECC);
            XFREE(der, cert->heap, DYNAMIC_TYPE_CERT);
            return MEMORY_E;
        }

        idx = 0;
        ret = wc_EccPublicKeyDecode(der, &idx, eckey, derSz);
        if (ret != 0) {
            WOLFSSL_MSG("wc_EccPublicKeyDecode failed");
            XFREE(der, cert->heap, DYNAMIC_TYPE_CERT);
            wc_ecc_free(eckey);
            return PUBLIC_KEY_E;
        }
#else
        XFREE(der, cert->heap, DYNAMIC_TYPE_CERT);
        return PUBLIC_KEY_E;
#endif /* HAVE_ECC */
    }

    XFREE(der, cert->heap, DYNAMIC_TYPE_CERT);

    ret = wc_SetSubjectKeyIdFromPublicKey(cert, rsakey, eckey);

    wc_FreeRsaKey(rsakey);
    XFREE(rsakey, cert->heap, DYNAMIC_TYPE_RSA);
#ifdef HAVE_ECC
    wc_ecc_free(eckey);
    XFREE(eckey, cert->heap, DYNAMIC_TYPE_ECC);
#endif
    return ret;
}

#endif /* NO_FILESYSTEM */

/* Set AKID from certificate contains in buffer (DER encoded) */
int wc_SetAuthKeyIdFromCert(Cert *cert, const byte *der, int derSz)
{
    int ret;

#ifdef WOLFSSL_SMALL_STACK
    DecodedCert* decoded;
#else
    DecodedCert decoded[1];
#endif

    if (cert == NULL || der == NULL || derSz <= 0)
        return BAD_FUNC_ARG;

#ifdef WOLFSSL_SMALL_STACK
    decoded = (DecodedCert*)XMALLOC(sizeof(DecodedCert),
                                    NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (decoded == NULL)
        return MEMORY_E;
#endif

    /* decode certificate and get SKID that will be AKID of current cert */
    InitDecodedCert(decoded, (byte*)der, derSz, NULL);
    ret = ParseCert(decoded, CERT_TYPE, NO_VERIFY, 0);
    if (ret != 0) {
        FreeDecodedCert(decoded);
        #ifdef WOLFSSL_SMALL_STACK
            XFREE(decoded, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        #endif
        return ret;
    }

    /* Subject Key Id not found !! */
    if (decoded->extSubjKeyIdSet == 0) {
        FreeDecodedCert(decoded);
        #ifdef WOLFSSL_SMALL_STACK
            XFREE(decoded, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        #endif
        return ASN_NO_SKID;
    }

    /* SKID invalid size */
    if (sizeof(cert->akid) < sizeof(decoded->extSubjKeyId)) {
        FreeDecodedCert(decoded);
        #ifdef WOLFSSL_SMALL_STACK
            XFREE(decoded, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        #endif
        return MEMORY_E;
    }

    /* Put the SKID of CA to AKID of certificate */
    XMEMCPY(cert->akid, decoded->extSubjKeyId, KEYID_SIZE);
    cert->akidSz = KEYID_SIZE;

    FreeDecodedCert(decoded);
    #ifdef WOLFSSL_SMALL_STACK
        XFREE(decoded, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    #endif

    return 0;
}


#ifndef NO_FILESYSTEM

/* Set AKID from certificate file in PEM */
int wc_SetAuthKeyId(Cert *cert, const char* file)
{
    int         ret;
    int         derSz;
    byte*       der;

    if (cert == NULL || file == NULL)
        return BAD_FUNC_ARG;

    der = (byte*)XMALLOC(EIGHTK_BUF, cert->heap, DYNAMIC_TYPE_CERT);
    if (der == NULL) {
        WOLFSSL_MSG("wc_SetAuthKeyId OOF Problem");
        return MEMORY_E;
    }

    derSz = wolfSSL_PemCertToDer(file, der, EIGHTK_BUF);
    if (derSz <= 0)
    {
        XFREE(der, cert->heap, DYNAMIC_TYPE_CERT);
        return derSz;
    }

    ret = wc_SetAuthKeyIdFromCert(cert, der, derSz);
    XFREE(der, cert->heap, DYNAMIC_TYPE_CERT);

    return ret;
}

#endif /* NO_FILESYSTEM */

/* Set KeyUsage from human readable string */
int wc_SetKeyUsage(Cert *cert, const char *value)
{
    int ret = 0;
    char *token, *str, *ptr;
    word32 len;

    if (cert == NULL || value == NULL)
        return BAD_FUNC_ARG;

    cert->keyUsage = 0;

    str = (char*)XMALLOC(XSTRLEN(value)+1, cert->heap, DYNAMIC_TYPE_TMP_BUFFER);
    if (str == NULL)
        return MEMORY_E;

    XMEMSET(str, 0, XSTRLEN(value)+1);
    XSTRNCPY(str, value, XSTRLEN(value));

    /* parse value, and set corresponding Key Usage value */
    if ((token = XSTRTOK(str, ",", &ptr)) == NULL) {
        XFREE(str, cert->heap, DYNAMIC_TYPE_TMP_BUFFER);
        return KEYUSAGE_E;
    }
    while (token != NULL)
    {
        len = (word32)XSTRLEN(token);

        if (!XSTRNCASECMP(token, "digitalSignature", len))
            cert->keyUsage |= KEYUSE_DIGITAL_SIG;
        else if (!XSTRNCASECMP(token, "nonRepudiation", len) ||
                 !XSTRNCASECMP(token, "contentCommitment", len))
            cert->keyUsage |= KEYUSE_CONTENT_COMMIT;
        else if (!XSTRNCASECMP(token, "keyEncipherment", len))
            cert->keyUsage |= KEYUSE_KEY_ENCIPHER;
        else if (!XSTRNCASECMP(token, "dataEncipherment", len))
            cert->keyUsage |= KEYUSE_DATA_ENCIPHER;
        else if (!XSTRNCASECMP(token, "keyAgreement", len))
            cert->keyUsage |= KEYUSE_KEY_AGREE;
        else if (!XSTRNCASECMP(token, "keyCertSign", len))
            cert->keyUsage |= KEYUSE_KEY_CERT_SIGN;
        else if (!XSTRNCASECMP(token, "cRLSign", len))
            cert->keyUsage |= KEYUSE_CRL_SIGN;
        else if (!XSTRNCASECMP(token, "encipherOnly", len))
            cert->keyUsage |= KEYUSE_ENCIPHER_ONLY;
        else if (!XSTRNCASECMP(token, "decipherOnly", len))
            cert->keyUsage |= KEYUSE_DECIPHER_ONLY;
        else {
            ret = KEYUSAGE_E;
            break;
        }

        token = XSTRTOK(NULL, ",", &ptr);
    }

    XFREE(str, cert->heap, DYNAMIC_TYPE_TMP_BUFFER);
    return ret;
}

/* Set ExtendedKeyUsage from human readable string */
int wc_SetExtKeyUsage(Cert *cert, const char *value)
{
    int ret = 0;
    char *token, *str, *ptr;
    word32 len;

    if (cert == NULL || value == NULL)
        return BAD_FUNC_ARG;

    cert->extKeyUsage = 0;

    str = (char*)XMALLOC(XSTRLEN(value)+1, cert->heap, DYNAMIC_TYPE_TMP_BUFFER);
    if (str == NULL)
        return MEMORY_E;

    XMEMSET(str, 0, XSTRLEN(value)+1);
    XSTRNCPY(str, value, XSTRLEN(value));

    /* parse value, and set corresponding Key Usage value */
    if ((token = XSTRTOK(str, ",", &ptr)) == NULL) {
        XFREE(str, cert->heap, DYNAMIC_TYPE_TMP_BUFFER);
        return EXTKEYUSAGE_E;
    }

    while (token != NULL)
    {
        len = (word32)XSTRLEN(token);

        if (!XSTRNCASECMP(token, "any", len))
            cert->extKeyUsage |= EXTKEYUSE_ANY;
        else if (!XSTRNCASECMP(token, "serverAuth", len))
            cert->extKeyUsage |= EXTKEYUSE_SERVER_AUTH;
        else if (!XSTRNCASECMP(token, "clientAuth", len))
            cert->extKeyUsage |= EXTKEYUSE_CLIENT_AUTH;
        else if (!XSTRNCASECMP(token, "codeSigning", len))
            cert->extKeyUsage |= EXTKEYUSE_CODESIGN;
        else if (!XSTRNCASECMP(token, "emailProtection", len))
            cert->extKeyUsage |= EXTKEYUSE_EMAILPROT;
        else if (!XSTRNCASECMP(token, "timeStamping", len))
            cert->extKeyUsage |= EXTKEYUSE_TIMESTAMP;
        else if (!XSTRNCASECMP(token, "OCSPSigning", len))
            cert->extKeyUsage |= EXTKEYUSE_OCSP_SIGN;
        else {
            ret = EXTKEYUSAGE_E;
            break;
        }

        token = XSTRTOK(NULL, ",", &ptr);
    }

    XFREE(str, cert->heap, DYNAMIC_TYPE_TMP_BUFFER);
    return ret;
}
#endif /* WOLFSSL_CERT_EXT */


#ifdef WOLFSSL_ALT_NAMES

/* Set Alt Names from der cert, return 0 on success */
static int SetAltNamesFromCert(Cert* cert, const byte* der, int derSz)
{
    int ret;
#ifdef WOLFSSL_SMALL_STACK
    DecodedCert* decoded;
#else
    DecodedCert decoded[1];
#endif

    if (derSz < 0)
        return derSz;

#ifdef WOLFSSL_SMALL_STACK
    decoded = (DecodedCert*)XMALLOC(sizeof(DecodedCert), NULL,
                                                       DYNAMIC_TYPE_TMP_BUFFER);
    if (decoded == NULL)
        return MEMORY_E;
#endif

    InitDecodedCert(decoded, (byte*)der, derSz, NULL);
    ret = ParseCertRelative(decoded, CA_TYPE, NO_VERIFY, 0);

    if (ret < 0) {
        WOLFSSL_MSG("ParseCertRelative error");
    }
    else if (decoded->extensions) {
        byte   b;
        int    length;
        word32 maxExtensionsIdx;

        decoded->srcIdx = decoded->extensionsIdx;
        b = decoded->source[decoded->srcIdx++];

        if (b != ASN_EXTENSIONS) {
            ret = ASN_PARSE_E;
        }
        else if (GetLength(decoded->source, &decoded->srcIdx, &length,
                                                         decoded->maxIdx) < 0) {
            ret = ASN_PARSE_E;
        }
        else if (GetSequence(decoded->source, &decoded->srcIdx, &length,
                                                         decoded->maxIdx) < 0) {
            ret = ASN_PARSE_E;
        }
        else {
            maxExtensionsIdx = decoded->srcIdx + length;

            while (decoded->srcIdx < maxExtensionsIdx) {
                word32 oid;
                word32 startIdx = decoded->srcIdx;
                word32 tmpIdx;

                if (GetSequence(decoded->source, &decoded->srcIdx, &length,
                            decoded->maxIdx) < 0) {
                    ret = ASN_PARSE_E;
                    break;
                }

                tmpIdx = decoded->srcIdx;
                decoded->srcIdx = startIdx;

                if (GetAlgoId(decoded->source, &decoded->srcIdx, &oid,
                              oidCertExtType, decoded->maxIdx) < 0) {
                    ret = ASN_PARSE_E;
                    break;
                }

                if (oid == ALT_NAMES_OID) {
                    cert->altNamesSz = length + (tmpIdx - startIdx);

                    if (cert->altNamesSz < (int)sizeof(cert->altNames))
                        XMEMCPY(cert->altNames, &decoded->source[startIdx],
                                cert->altNamesSz);
                    else {
                        cert->altNamesSz = 0;
                        WOLFSSL_MSG("AltNames extensions too big");
                        ret = ALT_NAME_E;
                        break;
                    }
                }
                decoded->srcIdx = tmpIdx + length;
            }
        }
    }

    FreeDecodedCert(decoded);
#ifdef WOLFSSL_SMALL_STACK
    XFREE(decoded, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return ret < 0 ? ret : 0;
}


/* Set Dates from der cert, return 0 on success */
static int SetDatesFromCert(Cert* cert, const byte* der, int derSz)
{
    int ret;
#ifdef WOLFSSL_SMALL_STACK
    DecodedCert* decoded;
#else
    DecodedCert decoded[1];
#endif

    WOLFSSL_ENTER("SetDatesFromCert");
    if (derSz < 0)
        return derSz;

#ifdef WOLFSSL_SMALL_STACK
    decoded = (DecodedCert*)XMALLOC(sizeof(DecodedCert), NULL,
                                                       DYNAMIC_TYPE_TMP_BUFFER);
    if (decoded == NULL)
        return MEMORY_E;
#endif

    InitDecodedCert(decoded, (byte*)der, derSz, NULL);
    ret = ParseCertRelative(decoded, CA_TYPE, NO_VERIFY, 0);

    if (ret < 0) {
        WOLFSSL_MSG("ParseCertRelative error");
    }
    else if (decoded->beforeDate == NULL || decoded->afterDate == NULL) {
        WOLFSSL_MSG("Couldn't extract dates");
        ret = -1;
    }
    else if (decoded->beforeDateLen > MAX_DATE_SIZE ||
                                        decoded->afterDateLen > MAX_DATE_SIZE) {
        WOLFSSL_MSG("Bad date size");
        ret = -1;
    }
    else {
        XMEMCPY(cert->beforeDate, decoded->beforeDate, decoded->beforeDateLen);
        XMEMCPY(cert->afterDate,  decoded->afterDate,  decoded->afterDateLen);

        cert->beforeDateSz = decoded->beforeDateLen;
        cert->afterDateSz  = decoded->afterDateLen;
    }

    FreeDecodedCert(decoded);

#ifdef WOLFSSL_SMALL_STACK
    XFREE(decoded, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return ret < 0 ? ret : 0;
}


#endif /* WOLFSSL_ALT_NAMES && !NO_RSA */


/* Set cn name from der buffer, return 0 on success */
static int SetNameFromCert(CertName* cn, const byte* der, int derSz)
{
    int ret, sz;
#ifdef WOLFSSL_SMALL_STACK
    DecodedCert* decoded;
#else
    DecodedCert decoded[1];
#endif

    if (derSz < 0)
        return derSz;

#ifdef WOLFSSL_SMALL_STACK
    decoded = (DecodedCert*)XMALLOC(sizeof(DecodedCert), NULL,
                                                       DYNAMIC_TYPE_TMP_BUFFER);
    if (decoded == NULL)
        return MEMORY_E;
#endif

    InitDecodedCert(decoded, (byte*)der, derSz, NULL);
    ret = ParseCertRelative(decoded, CA_TYPE, NO_VERIFY, 0);

    if (ret < 0) {
        WOLFSSL_MSG("ParseCertRelative error");
    }
    else {
        if (decoded->subjectCN) {
            sz = (decoded->subjectCNLen < CTC_NAME_SIZE) ? decoded->subjectCNLen
                                                         : CTC_NAME_SIZE - 1;
            XSTRNCPY(cn->commonName, decoded->subjectCN, CTC_NAME_SIZE);
            cn->commonName[sz] = 0;
            cn->commonNameEnc = decoded->subjectCNEnc;
        }
        if (decoded->subjectC) {
            sz = (decoded->subjectCLen < CTC_NAME_SIZE) ? decoded->subjectCLen
                                                        : CTC_NAME_SIZE - 1;
            XSTRNCPY(cn->country, decoded->subjectC, CTC_NAME_SIZE);
            cn->country[sz] = 0;
            cn->countryEnc = decoded->subjectCEnc;
        }
        if (decoded->subjectST) {
            sz = (decoded->subjectSTLen < CTC_NAME_SIZE) ? decoded->subjectSTLen
                                                         : CTC_NAME_SIZE - 1;
            XSTRNCPY(cn->state, decoded->subjectST, CTC_NAME_SIZE);
            cn->state[sz] = 0;
            cn->stateEnc = decoded->subjectSTEnc;
        }
        if (decoded->subjectL) {
            sz = (decoded->subjectLLen < CTC_NAME_SIZE) ? decoded->subjectLLen
                                                        : CTC_NAME_SIZE - 1;
            XSTRNCPY(cn->locality, decoded->subjectL, CTC_NAME_SIZE);
            cn->locality[sz] = 0;
            cn->localityEnc = decoded->subjectLEnc;
        }
        if (decoded->subjectO) {
            sz = (decoded->subjectOLen < CTC_NAME_SIZE) ? decoded->subjectOLen
                                                        : CTC_NAME_SIZE - 1;
            XSTRNCPY(cn->org, decoded->subjectO, CTC_NAME_SIZE);
            cn->org[sz] = 0;
            cn->orgEnc = decoded->subjectOEnc;
        }
        if (decoded->subjectOU) {
            sz = (decoded->subjectOULen < CTC_NAME_SIZE) ? decoded->subjectOULen
                                                         : CTC_NAME_SIZE - 1;
            XSTRNCPY(cn->unit, decoded->subjectOU, CTC_NAME_SIZE);
            cn->unit[sz] = 0;
            cn->unitEnc = decoded->subjectOUEnc;
        }
        if (decoded->subjectSN) {
            sz = (decoded->subjectSNLen < CTC_NAME_SIZE) ? decoded->subjectSNLen
                                                         : CTC_NAME_SIZE - 1;
            XSTRNCPY(cn->sur, decoded->subjectSN, CTC_NAME_SIZE);
            cn->sur[sz] = 0;
            cn->surEnc = decoded->subjectSNEnc;
        }
        if (decoded->subjectEmail) {
            sz = (decoded->subjectEmailLen < CTC_NAME_SIZE)
               ?  decoded->subjectEmailLen : CTC_NAME_SIZE - 1;
            XSTRNCPY(cn->email, decoded->subjectEmail, CTC_NAME_SIZE);
            cn->email[sz] = 0;
        }
    }

    FreeDecodedCert(decoded);

#ifdef WOLFSSL_SMALL_STACK
    XFREE(decoded, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return ret < 0 ? ret : 0;
}


#ifndef NO_FILESYSTEM

/* Set cert issuer from issuerFile in PEM */
int wc_SetIssuer(Cert* cert, const char* issuerFile)
{
    int         ret;
    int         derSz;
    byte*       der = (byte*)XMALLOC(EIGHTK_BUF, cert->heap, DYNAMIC_TYPE_CERT);

    if (der == NULL) {
        WOLFSSL_MSG("wc_SetIssuer OOF Problem");
        return MEMORY_E;
    }
    derSz = wolfSSL_PemCertToDer(issuerFile, der, EIGHTK_BUF);
    cert->selfSigned = 0;
    ret = SetNameFromCert(&cert->issuer, der, derSz);
    XFREE(der, cert->heap, DYNAMIC_TYPE_CERT);

    return ret;
}


/* Set cert subject from subjectFile in PEM */
int wc_SetSubject(Cert* cert, const char* subjectFile)
{
    int         ret;
    int         derSz;
    byte*       der = (byte*)XMALLOC(EIGHTK_BUF, cert->heap, DYNAMIC_TYPE_CERT);

    if (der == NULL) {
        WOLFSSL_MSG("wc_SetSubject OOF Problem");
        return MEMORY_E;
    }
    derSz = wolfSSL_PemCertToDer(subjectFile, der, EIGHTK_BUF);
    ret = SetNameFromCert(&cert->subject, der, derSz);
    XFREE(der, cert->heap, DYNAMIC_TYPE_CERT);

    return ret;
}


#ifdef WOLFSSL_ALT_NAMES

/* Set atl names from file in PEM */
int wc_SetAltNames(Cert* cert, const char* file)
{
    int         ret;
    int         derSz;
    byte*       der = (byte*)XMALLOC(EIGHTK_BUF, cert->heap, DYNAMIC_TYPE_CERT);

    if (der == NULL) {
        WOLFSSL_MSG("wc_SetAltNames OOF Problem");
        return MEMORY_E;
    }
    derSz = wolfSSL_PemCertToDer(file, der, EIGHTK_BUF);
    ret = SetAltNamesFromCert(cert, der, derSz);
    XFREE(der, cert->heap, DYNAMIC_TYPE_CERT);

    return ret;
}

#endif /* WOLFSSL_ALT_NAMES */

#endif /* NO_FILESYSTEM */

/* Set cert issuer from DER buffer */
int wc_SetIssuerBuffer(Cert* cert, const byte* der, int derSz)
{
    cert->selfSigned = 0;
    return SetNameFromCert(&cert->issuer, der, derSz);
}


/* Set cert subject from DER buffer */
int wc_SetSubjectBuffer(Cert* cert, const byte* der, int derSz)
{
    return SetNameFromCert(&cert->subject, der, derSz);
}


#ifdef WOLFSSL_ALT_NAMES

/* Set cert alt names from DER buffer */
int wc_SetAltNamesBuffer(Cert* cert, const byte* der, int derSz)
{
    return SetAltNamesFromCert(cert, der, derSz);
}

/* Set cert dates from DER buffer */
int wc_SetDatesBuffer(Cert* cert, const byte* der, int derSz)
{
    return SetDatesFromCert(cert, der, derSz);
}

#endif /* WOLFSSL_ALT_NAMES */

#endif /* WOLFSSL_CERT_GEN */


#ifdef HAVE_ECC

/* Der Encode r & s ints into out, outLen is (in/out) size */
int StoreECC_DSA_Sig(byte* out, word32* outLen, mp_int* r, mp_int* s)
{
    word32 idx = 0;
    int    rSz;                           /* encoding size */
    int    sSz;
    word32 headerSz = 4;   /* 2*ASN_TAG + 2*LEN(ENUM) */

    /* If the leading bit on the INTEGER is a 1, add a leading zero */
    int rLeadingZero = mp_leading_bit(r);
    int sLeadingZero = mp_leading_bit(s);
    int rLen = mp_unsigned_bin_size(r);   /* big int size */
    int sLen = mp_unsigned_bin_size(s);

    if (*outLen < (rLen + rLeadingZero + sLen + sLeadingZero +
                   headerSz + 2))  /* SEQ_TAG + LEN(ENUM) */
        return BUFFER_E;

    idx = SetSequence(rLen + rLeadingZero + sLen+sLeadingZero + headerSz, out);

    /* store r */
    rSz = SetASNIntMP(r, -1, &out[idx]);
    if (rSz < 0)
        return rSz;
    idx += rSz;

    /* store s */
    sSz = SetASNIntMP(s, -1, &out[idx]);
    if (sSz < 0)
        return sSz;
    idx += sSz;

    *outLen = idx;

    return 0;
}


/* Der Decode ECC-DSA Signature, r & s stored as big ints */
int DecodeECC_DSA_Sig(const byte* sig, word32 sigLen, mp_int* r, mp_int* s)
{
    word32 idx = 0;
    int    len = 0;

    if (GetSequence(sig, &idx, &len, sigLen) < 0) {
        return ASN_ECC_KEY_E;
    }

    if ((word32)len > (sigLen - idx)) {
        return ASN_ECC_KEY_E;
    }

    if (GetInt(r, sig, &idx, sigLen) < 0) {
        return ASN_ECC_KEY_E;
    }

    if (GetInt(s, sig, &idx, sigLen) < 0) {
        return ASN_ECC_KEY_E;
    }

    return 0;
}


int wc_EccPrivateKeyDecode(const byte* input, word32* inOutIdx, ecc_key* key,
                        word32 inSz)
{
    word32 oidSum;
    int    version, length;
    int    privSz, pubSz = 0;
    byte   b;
    int    ret = 0;
    int    curve_id = ECC_CURVE_DEF;
#ifdef WOLFSSL_SMALL_STACK
    byte* priv;
    byte* pub;
#else
    byte priv[ECC_MAXSIZE+1];
    byte pub[2*(ECC_MAXSIZE+1)]; /* public key has two parts plus header */
#endif
    byte* pubData = NULL;

    if (input == NULL || inOutIdx == NULL || key == NULL || inSz == 0)
        return BAD_FUNC_ARG;

    if (GetSequence(input, inOutIdx, &length, inSz) < 0)
        return ASN_PARSE_E;

    if (GetMyVersion(input, inOutIdx, &version, inSz) < 0)
        return ASN_PARSE_E;

    b = input[*inOutIdx];
    *inOutIdx += 1;

    /* priv type */
    if (b != 4 && b != 6 && b != 7)
        return ASN_PARSE_E;

    if (GetLength(input, inOutIdx, &length, inSz) < 0)
        return ASN_PARSE_E;

    if (length > ECC_MAXSIZE)
        return BUFFER_E;

#ifdef WOLFSSL_SMALL_STACK
    priv = (byte*)XMALLOC(ECC_MAXSIZE+1, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (priv == NULL)
        return MEMORY_E;

    pub = (byte*)XMALLOC(2*(ECC_MAXSIZE+1), NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (pub == NULL) {
        XFREE(priv, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        return MEMORY_E;
    }
#endif

    /* priv key */
    privSz = length;
    XMEMCPY(priv, &input[*inOutIdx], privSz);
    *inOutIdx += length;

    if (ret == 0 && (*inOutIdx + 1) < inSz) {
        /* prefix 0, may have */
        b = input[*inOutIdx];
        if (b == ECC_PREFIX_0) {
            *inOutIdx += 1;

            if (GetLength(input, inOutIdx, &length, inSz) <= 0)
                ret = ASN_PARSE_E;
            else {
                ret = GetObjectId(input, inOutIdx, &oidSum, oidIgnoreType,
                                  inSz);
                if (ret == 0) {
                    if ((ret = CheckCurve(oidSum)) < 0)
                        ret = ECC_CURVE_OID_E;
                    else {
                        curve_id = ret;
                        ret = 0;
                    }
                }
            }
        }
    }

    if (ret == 0 && (*inOutIdx + 1) < inSz) {
        /* prefix 1 */
        b = input[*inOutIdx];
        *inOutIdx += 1;

        if (b != ECC_PREFIX_1) {
            ret = ASN_ECC_KEY_E;
        }
        else if (GetLength(input, inOutIdx, &length, inSz) <= 0) {
            ret = ASN_PARSE_E;
        }
        else {
            /* key header */
            ret = CheckBitString(input, inOutIdx, &length, inSz, 0, NULL);
            if (ret == 0) {
                /* pub key */
                pubSz = length;
                if (pubSz < 2*(ECC_MAXSIZE+1)) {
                    XMEMCPY(pub, &input[*inOutIdx], pubSz);
                    *inOutIdx += length;
                    pubData = pub;
                }
                else
                    ret = BUFFER_E;
            }
        }
    }

    if (ret == 0) {
        ret = wc_ecc_import_private_key_ex(priv, privSz, pubData, pubSz, key,
                                                                      curve_id);
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(priv, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    XFREE(pub,  NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    return ret;
}


int wc_EccPublicKeyDecode(const byte* input, word32* inOutIdx,
                          ecc_key* key, word32 inSz)
{
    int    length;
    int    ret;
#ifdef ECC_CHECK_PUBLIC_KEY_OID
    word32 oidSum;
#endif

    if (input == NULL || inOutIdx == NULL || key == NULL || inSz == 0)
        return BAD_FUNC_ARG;

    if (GetSequence(input, inOutIdx, &length, inSz) < 0)
        return ASN_PARSE_E;

    if (GetSequence(input, inOutIdx, &length, inSz) < 0)
        return ASN_PARSE_E;

    ret = SkipObjectId(input, inOutIdx, inSz);
    if (ret != 0)
        return ret;

    /* ecc params information */
#ifdef ECC_CHECK_PUBLIC_KEY_OID
    ret = GetObjectId(input, inOutIdx, &oidSum, oidIgnoreType, inSz);
    if (ret != 0)
        return ret;
    if (CheckCurve(oidSum) < 0)
        return ECC_CURVE_OID_E;
#else
    ret = SkipObjectId(input, inOutIdx, inSz);
    if (ret != 0)
        return ret;
#endif

    /* key header */
    ret = CheckBitString(input, inOutIdx, NULL, inSz, 1, NULL);
    if (ret != 0)
        return ret;

    /* This is the raw point data compressed or uncompressed. */
    if (wc_ecc_import_x963(input + *inOutIdx, inSz - *inOutIdx, key) != 0)
        return ASN_ECC_KEY_E;

    return 0;
}


#ifdef WOLFSSL_KEY_GEN

/* build DER formatted ECC key, include optional public key if requested,
 * return length on success, negative on error */
static int wc_BuildEccKeyDer(ecc_key* key, byte* output, word32 inLen,
                             int pubIn)
{
    byte   curve[MAX_ALGO_SZ+2];
    byte   ver[MAX_VERSION_SZ];
    byte   seq[MAX_SEQ_SZ];
    byte   *prv = NULL, *pub = NULL;
    int    ret, totalSz, curveSz, verSz;
    int    privHdrSz  = ASN_ECC_HEADER_SZ;
    int    pubHdrSz   = ASN_ECC_CONTEXT_SZ + ASN_ECC_HEADER_SZ;

    word32 idx = 0, prvidx = 0, pubidx = 0, curveidx = 0;
    word32 seqSz, privSz, pubSz = ECC_BUFSIZE;

    if (key == NULL || output == NULL || inLen == 0)
        return BAD_FUNC_ARG;

    /* curve */
    curve[curveidx++] = ECC_PREFIX_0;
    curveidx++ /* to put the size after computation */;
    curveSz = SetCurve(key, curve+curveidx);
    if (curveSz < 0)
        return curveSz;
    /* set computed size */
    curve[1] = (byte)curveSz;
    curveidx += curveSz;

    /* private */
    privSz = key->dp->size;
    prv = (byte*)XMALLOC(privSz + privHdrSz + MAX_SEQ_SZ,
                         key->heap, DYNAMIC_TYPE_TMP_BUFFER);
    if (prv == NULL) {
        return MEMORY_E;
    }
    prvidx += SetOctetString8Bit(key->dp->size, &prv[prvidx]);
    ret = wc_ecc_export_private_only(key, prv + prvidx, &privSz);
    if (ret < 0) {
        XFREE(prv, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
        return ret;
    }
    prvidx += privSz;

    /* pubIn */
    if (pubIn) {
        ret = wc_ecc_export_x963(key, NULL, &pubSz);
        if (ret != LENGTH_ONLY_E) {
            XFREE(prv, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
            return ret;
        }

        pub = (byte*)XMALLOC(pubSz + pubHdrSz + MAX_SEQ_SZ,
                             key->heap, DYNAMIC_TYPE_TMP_BUFFER);
        if (pub == NULL) {
            XFREE(prv, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
            return MEMORY_E;
        }

        pub[pubidx++] = ECC_PREFIX_1;
        if (pubSz > 128) /* leading zero + extra size byte */
            pubidx += SetLength(pubSz + ASN_ECC_CONTEXT_SZ + 2, pub+pubidx);
        else /* leading zero */
            pubidx += SetLength(pubSz + ASN_ECC_CONTEXT_SZ + 1, pub+pubidx);

        /* SetBitString adds leading zero */
        pubidx += SetBitString(pubSz, 0, pub + pubidx);
        ret = wc_ecc_export_x963(key, pub + pubidx, &pubSz);
        if (ret != 0) {
            XFREE(prv, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
            XFREE(pub, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
            return ret;
        }
        pubidx += pubSz;
    }

    /* make headers */
    verSz = SetMyVersion(1, ver, FALSE);
    seqSz = SetSequence(verSz + prvidx + pubidx + curveidx, seq);

    totalSz = prvidx + pubidx + curveidx + verSz + seqSz;
    if (totalSz > (int)inLen) {
        XFREE(prv, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
        if (pubIn) {
            XFREE(pub, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
        }
        return BAD_FUNC_ARG;
    }

    /* write out */
    /* seq */
    XMEMCPY(output + idx, seq, seqSz);
    idx = seqSz;

    /* ver */
    XMEMCPY(output + idx, ver, verSz);
    idx += verSz;

    /* private */
    XMEMCPY(output + idx, prv, prvidx);
    idx += prvidx;
    XFREE(prv, key->heap, DYNAMIC_TYPE_TMP_BUFFER);

    /* curve */
    XMEMCPY(output + idx, curve, curveidx);
    idx += curveidx;

    /* pubIn */
    if (pubIn) {
        XMEMCPY(output + idx, pub, pubidx);
        /* idx += pubidx;  not used after write, if more data remove comment */
        XFREE(pub, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
    }

    return totalSz;
}


/* Write a Private ecc key, including public to DER format,
 * length on success else < 0 */
int wc_EccKeyToDer(ecc_key* key, byte* output, word32 inLen)
{
    return wc_BuildEccKeyDer(key, output, inLen, 1);
}


/* Write only private ecc key to DER format,
 * length on success else < 0 */
int wc_EccPrivateKeyToDer(ecc_key* key, byte* output, word32 inLen)
{
    return wc_BuildEccKeyDer(key, output, inLen, 0);
}

#endif /* WOLFSSL_KEY_GEN */

#endif  /* HAVE_ECC */


#ifdef HAVE_ED25519

int wc_Ed25519PrivateKeyDecode(const byte* input, word32* inOutIdx,
                               ed25519_key* key, word32 inSz)
{
    word32      oid;
    int         ret, version, length, endKeyIdx, privSz, pubSz;
    const byte* priv;
    const byte* pub;

    if (input == NULL || inOutIdx == NULL || key == NULL || inSz == 0)
        return BAD_FUNC_ARG;

    if (GetSequence(input, inOutIdx, &length, inSz) < 0)
        return ASN_PARSE_E;
    endKeyIdx = *inOutIdx + length;

    if (GetMyVersion(input, inOutIdx, &version, inSz) < 0)
        return ASN_PARSE_E;
    if (version != 0) {
        WOLFSSL_MSG("Unrecognized version of ED25519 private key");
        return ASN_PARSE_E;
    }

    if (GetAlgoId(input, inOutIdx, &oid, oidKeyType, inSz) < 0)
        return ASN_PARSE_E;
    if (oid != ED25519k)
        return ASN_PARSE_E;

    if (GetOctetString(input, inOutIdx, &length, inSz) < 0)
        return ASN_PARSE_E;

    if (GetOctetString(input, inOutIdx, &privSz, inSz) < 0)
        return ASN_PARSE_E;
    priv = input + *inOutIdx;
    *inOutIdx += privSz;

    if (endKeyIdx == (int)*inOutIdx) {
        ret = wc_ed25519_import_private_only(priv, privSz, key);
    }
    else {
        if (GetASNHeader(input, ASN_CONTEXT_SPECIFIC | ASN_CONSTRUCTED | 1,
                         inOutIdx, &length, inSz) < 0) {
            return ASN_PARSE_E;
        }
        if (GetOctetString(input, inOutIdx, &pubSz, inSz) < 0)
            return ASN_PARSE_E;
        pub = input + *inOutIdx;
        *inOutIdx += pubSz;

        ret = wc_ed25519_import_private_key(priv, privSz, pub, pubSz, key);
    }
    if (ret == 0 && endKeyIdx != (int)*inOutIdx)
        return ASN_PARSE_E;

    return ret;
}


int wc_Ed25519PublicKeyDecode(const byte* input, word32* inOutIdx,
                              ed25519_key* key, word32 inSz)
{
    int    length;
    int    ret;
#ifdef ECC_CHECK_PUBLIC_KEY_OID
    word32 oidSum;
#endif

    if (input == NULL || inOutIdx == NULL || key == NULL || inSz == 0)
        return BAD_FUNC_ARG;

    if (GetSequence(input, inOutIdx, &length, inSz) < 0)
        return ASN_PARSE_E;

    if (GetSequence(input, inOutIdx, &length, inSz) < 0)
        return ASN_PARSE_E;

    ret = SkipObjectId(input, inOutIdx, inSz);
    if (ret != 0)
        return ret;

    /* key header */
    ret = CheckBitString(input, inOutIdx, NULL, inSz, 1, NULL);
    if (ret != 0)
        return ret;

    /* This is the raw point data compressed or uncompressed. */
    if (wc_ed25519_import_public(input + *inOutIdx, inSz - *inOutIdx, key) != 0)
        return ASN_ECC_KEY_E;

    return 0;
}


#ifdef WOLFSSL_KEY_GEN

/* build DER formatted ED25519 key,
 * return length on success, negative on error */
static int wc_BuildEd25519KeyDer(ed25519_key* key, byte* output, word32 inLen,
                                 int pubOut)
{
    byte   algoArray[MAX_ALGO_SZ];
    byte   ver[MAX_VERSION_SZ];
    byte   seq[MAX_SEQ_SZ];
    int    ret;
    word32 idx = 0, seqSz, verSz, algoSz, privSz, pubSz = 0;

    if (key == NULL || output == NULL || inLen == 0)
        return BAD_FUNC_ARG;

    if (pubOut)
        pubSz = 2 + 2 + ED25519_PUB_KEY_SIZE;
    privSz = 2 + 2 + ED25519_KEY_SIZE;
    algoSz = SetAlgoID(ED25519k, algoArray, oidKeyType, 0);
    verSz  = SetMyVersion(0, ver, FALSE);
    seqSz  = SetSequence(verSz + algoSz + privSz + pubSz, seq);

    if (seqSz + verSz + algoSz + privSz + pubSz > inLen)
        return BAD_FUNC_ARG;

    /* write out */
    /* seq */
    XMEMCPY(output + idx, seq, seqSz);
    idx = seqSz;
    /* ver */
    XMEMCPY(output + idx, ver, verSz);
    idx += verSz;
    /* algo */
    XMEMCPY(output + idx, algoArray, algoSz);
    idx += algoSz;
    /* privKey */
    idx += SetOctetString(2 + ED25519_KEY_SIZE, output + idx);
    idx += SetOctetString(ED25519_KEY_SIZE, output + idx);
    ret = wc_ed25519_export_private_only(key, output + idx, &privSz);
    if (ret != 0)
        return ret;
    idx += privSz;
    /* pubKey */
    if (pubOut) {
        idx += SetExplicit(1, 2 + ED25519_PUB_KEY_SIZE, output + idx);
        idx += SetOctetString(ED25519_KEY_SIZE, output + idx);
        ret = wc_ed25519_export_public(key, output + idx, &pubSz);
        if (ret != 0)
            return ret;
        idx += pubSz;
    }

    return idx;
}

/* Write a Private ecc key, including public to DER format,
 * length on success else < 0 */
int wc_Ed25519KeyToDer(ed25519_key* key, byte* output, word32 inLen)
{
    return wc_BuildEd25519KeyDer(key, output, inLen, 1);
}



/* Write only private ecc key to DER format,
 * length on success else < 0 */
int wc_Ed25519PrivateKeyToDer(ed25519_key* key, byte* output, word32 inLen)
{
    return wc_BuildEd25519KeyDer(key, output, inLen, 0);
}

#endif /* WOLFSSL_KEY_GEN */

#endif  /* HAVE_ED25519 */


#if defined(HAVE_OCSP) || defined(HAVE_CRL)

/* Get raw Date only, no processing, 0 on success */
static int GetBasicDate(const byte* source, word32* idx, byte* date,
                        byte* format, int maxIdx)
{
    int    length;

    WOLFSSL_ENTER("GetBasicDate");

    *format = source[*idx];
    *idx += 1;
    if (*format != ASN_UTC_TIME && *format != ASN_GENERALIZED_TIME)
        return ASN_TIME_E;

    if (GetLength(source, idx, &length, maxIdx) < 0)
        return ASN_PARSE_E;

    if (length > MAX_DATE_SIZE || length < MIN_DATE_SIZE)
        return ASN_DATE_SZ_E;

    XMEMCPY(date, &source[*idx], length);
    *idx += length;

    return 0;
}

#endif


#ifdef HAVE_OCSP

static int GetEnumerated(const byte* input, word32* inOutIdx, int *value)
{
    word32 idx = *inOutIdx;
    word32 len;

    WOLFSSL_ENTER("GetEnumerated");

    *value = 0;

    if (input[idx++] != ASN_ENUMERATED)
        return ASN_PARSE_E;

    len = input[idx++];
    if (len > 4)
        return ASN_PARSE_E;

    while (len--) {
        *value  = *value << 8 | input[idx++];
    }

    *inOutIdx = idx;

    return *value;
}


static int DecodeSingleResponse(byte* source,
                            word32* ioIndex, OcspResponse* resp, word32 size)
{
    word32 idx = *ioIndex, prevIndex, oid;
    int length, wrapperSz;
    CertStatus* cs = resp->status;
    int ret;

    WOLFSSL_ENTER("DecodeSingleResponse");

    /* Outer wrapper of the SEQUENCE OF Single Responses. */
    if (GetSequence(source, &idx, &wrapperSz, size) < 0)
        return ASN_PARSE_E;

    prevIndex = idx;

    /* When making a request, we only request one status on one certificate
     * at a time. There should only be one SingleResponse */

    /* Wrapper around the Single Response */
    if (GetSequence(source, &idx, &length, size) < 0)
        return ASN_PARSE_E;

    /* Wrapper around the CertID */
    if (GetSequence(source, &idx, &length, size) < 0)
        return ASN_PARSE_E;
    /* Skip the hash algorithm */
    if (GetAlgoId(source, &idx, &oid, oidIgnoreType, size) < 0)
        return ASN_PARSE_E;
    /* Save reference to the hash of CN */
    ret = GetOctetString(source, &idx, &length, size);
    if (ret < 0)
        return ret;
    resp->issuerHash = source + idx;
    idx += length;
    /* Save reference to the hash of the issuer public key */
    ret = GetOctetString(source, &idx, &length, size);
    if (ret < 0)
        return ret;
    resp->issuerKeyHash = source + idx;
    idx += length;

    /* Get serial number */
    if (GetSerialNumber(source, &idx, cs->serial, &cs->serialSz, size) < 0)
        return ASN_PARSE_E;

    /* CertStatus */
    switch (source[idx++])
    {
        case (ASN_CONTEXT_SPECIFIC | CERT_GOOD):
            cs->status = CERT_GOOD;
            idx++;
            break;
        case (ASN_CONTEXT_SPECIFIC | ASN_CONSTRUCTED | CERT_REVOKED):
            cs->status = CERT_REVOKED;
            if (GetLength(source, &idx, &length, size) < 0)
                return ASN_PARSE_E;
            idx += length;
            break;
        case (ASN_CONTEXT_SPECIFIC | CERT_UNKNOWN):
            cs->status = CERT_UNKNOWN;
            idx++;
            break;
        default:
            return ASN_PARSE_E;
    }

#if defined(WOLFSSL_NGINX) || defined(WOLFSSL_HAPROXY)
    cs->thisDateAsn = source + idx;
#endif
    if (GetBasicDate(source, &idx, cs->thisDate,
                                                &cs->thisDateFormat, size) < 0)
        return ASN_PARSE_E;

#ifndef NO_ASN_TIME
    if (!XVALIDATE_DATE(cs->thisDate, cs->thisDateFormat, BEFORE))
        return ASN_BEFORE_DATE_E;
#endif

    /* The following items are optional. Only check for them if there is more
     * unprocessed data in the singleResponse wrapper. */

    if (((int)(idx - prevIndex) < wrapperSz) &&
        (source[idx] == (ASN_CONSTRUCTED | ASN_CONTEXT_SPECIFIC | 0)))
    {
        idx++;
        if (GetLength(source, &idx, &length, size) < 0)
            return ASN_PARSE_E;
#if defined(WOLFSSL_NGINX) || defined(WOLFSSL_HAPROXY)
        cs->nextDateAsn = source + idx;
#endif
        if (GetBasicDate(source, &idx, cs->nextDate,
                                                &cs->nextDateFormat, size) < 0)
            return ASN_PARSE_E;

#ifndef NO_ASN_TIME
        if (!XVALIDATE_DATE(cs->nextDate, cs->nextDateFormat, AFTER))
            return ASN_AFTER_DATE_E;
#endif
    }
    if (((int)(idx - prevIndex) < wrapperSz) &&
        (source[idx] == (ASN_CONSTRUCTED | ASN_CONTEXT_SPECIFIC | 1)))
    {
        idx++;
        if (GetLength(source, &idx, &length, size) < 0)
            return ASN_PARSE_E;
        idx += length;
    }

    *ioIndex = idx;

    return 0;
}

static int DecodeOcspRespExtensions(byte* source,
                            word32* ioIndex, OcspResponse* resp, word32 sz)
{
    word32 idx = *ioIndex;
    int length;
    int ext_bound; /* boundary index for the sequence of extensions */
    word32 oid;
    int ret;

    WOLFSSL_ENTER("DecodeOcspRespExtensions");

    if ((idx + 1) > sz)
        return BUFFER_E;

    if (source[idx++] != (ASN_CONSTRUCTED | ASN_CONTEXT_SPECIFIC | 1))
        return ASN_PARSE_E;

    if (GetLength(source, &idx, &length, sz) < 0)
        return ASN_PARSE_E;

    if (GetSequence(source, &idx, &length, sz) < 0)
        return ASN_PARSE_E;

    ext_bound = idx + length;

    while (idx < (word32)ext_bound) {
        if (GetSequence(source, &idx, &length, sz) < 0) {
            WOLFSSL_MSG("\tfail: should be a SEQUENCE");
            return ASN_PARSE_E;
        }

        oid = 0;
        if (GetObjectId(source, &idx, &oid, oidOcspType, sz) < 0) {
            WOLFSSL_MSG("\tfail: OBJECT ID");
            return ASN_PARSE_E;
        }

        /* check for critical flag */
        if (source[idx] == ASN_BOOLEAN) {
            WOLFSSL_MSG("\tfound optional critical flag, moving past");
            ret = GetBoolean(source, &idx, sz);
            if (ret < 0)
                return ret;
        }

        ret = GetOctetString(source, &idx, &length, sz);
        if (ret < 0)
            return ret;

        if (oid == OCSP_NONCE_OID) {
            /* get data inside extra OCTET_STRING */
            ret = GetOctetString(source, &idx, &length, sz);
            if (ret < 0)
                return ret;

            resp->nonce = source + idx;
            resp->nonceSz = length;
        }

        idx += length;
    }

    *ioIndex = idx;
    return 0;
}


static int DecodeResponseData(byte* source,
                            word32* ioIndex, OcspResponse* resp, word32 size)
{
    word32 idx = *ioIndex, prev_idx;
    int length;
    int version;
    word32 responderId = 0;

    WOLFSSL_ENTER("DecodeResponseData");

    resp->response = source + idx;
    prev_idx = idx;
    if (GetSequence(source, &idx, &length, size) < 0)
        return ASN_PARSE_E;
    resp->responseSz = length + idx - prev_idx;

    /* Get version. It is an EXPLICIT[0] DEFAULT(0) value. If this
     * item isn't an EXPLICIT[0], then set version to zero and move
     * onto the next item.
     */
    if (source[idx] == (ASN_CONTEXT_SPECIFIC | ASN_CONSTRUCTED))
    {
        idx += 2; /* Eat the value and length */
        if (GetMyVersion(source, &idx, &version, size) < 0)
            return ASN_PARSE_E;
    } else
        version = 0;

    responderId = source[idx++];
    if ((responderId == (ASN_CONTEXT_SPECIFIC | ASN_CONSTRUCTED | 1)) ||
        (responderId == (ASN_CONTEXT_SPECIFIC | ASN_CONSTRUCTED | 2)))
    {
        if (GetLength(source, &idx, &length, size) < 0)
            return ASN_PARSE_E;
        idx += length;
    }
    else
        return ASN_PARSE_E;

    /* save pointer to the producedAt time */
    if (GetBasicDate(source, &idx, resp->producedDate,
                                        &resp->producedDateFormat, size) < 0)
        return ASN_PARSE_E;

    if (DecodeSingleResponse(source, &idx, resp, size) < 0)
        return ASN_PARSE_E;

    /*
     * Check the length of the ResponseData against the current index to
     * see if there are extensions, they are optional.
     */
    if (idx - prev_idx < resp->responseSz)
        if (DecodeOcspRespExtensions(source, &idx, resp, size) < 0)
            return ASN_PARSE_E;

    *ioIndex = idx;
    return 0;
}


#ifndef WOLFSSL_NO_OCSP_OPTIONAL_CERTS

static int DecodeCerts(byte* source,
                            word32* ioIndex, OcspResponse* resp, word32 size)
{
    word32 idx = *ioIndex;

    WOLFSSL_ENTER("DecodeCerts");

    if (source[idx++] == (ASN_CONSTRUCTED | ASN_CONTEXT_SPECIFIC))
    {
        int length;

        if (GetLength(source, &idx, &length, size) < 0)
            return ASN_PARSE_E;

        if (GetSequence(source, &idx, &length, size) < 0)
            return ASN_PARSE_E;

        resp->cert = source + idx;
        resp->certSz = length;

        idx += length;
    }
    *ioIndex = idx;
    return 0;
}

#endif /* WOLFSSL_NO_OCSP_OPTIONAL_CERTS */


static int DecodeBasicOcspResponse(byte* source, word32* ioIndex,
            OcspResponse* resp, word32 size, void* cm, void* heap, int noVerify)
{
    int    length;
    word32 idx = *ioIndex;
    word32 end_index;
    int    ret;
    int    sigLength;

    WOLFSSL_ENTER("DecodeBasicOcspResponse");
    (void)heap;

    if (GetSequence(source, &idx, &length, size) < 0)
        return ASN_PARSE_E;

    if (idx + length > size)
        return ASN_INPUT_E;
    end_index = idx + length;

    if (DecodeResponseData(source, &idx, resp, size) < 0)
        return ASN_PARSE_E;

    /* Get the signature algorithm */
    if (GetAlgoId(source, &idx, &resp->sigOID, oidSigType, size) < 0)
        return ASN_PARSE_E;

    ret = CheckBitString(source, &idx, &sigLength, size, 1, NULL);
    if (ret != 0)
        return ret;

    resp->sigSz = sigLength;
    resp->sig = source + idx;
    idx += sigLength;

    /*
     * Check the length of the BasicOcspResponse against the current index to
     * see if there are certificates, they are optional.
     */
#ifndef WOLFSSL_NO_OCSP_OPTIONAL_CERTS
    if (idx < end_index)
    {
        DecodedCert cert;

        if (DecodeCerts(source, &idx, resp, size) < 0)
            return ASN_PARSE_E;

        InitDecodedCert(&cert, resp->cert, resp->certSz, heap);

        /* Don't verify if we don't have access to Cert Manager. */
        ret = ParseCertRelative(&cert, CERT_TYPE,
                                noVerify ? NO_VERIFY : VERIFY_OCSP, cm);
        if (ret < 0) {
            WOLFSSL_MSG("\tOCSP Responder certificate parsing failed");
            FreeDecodedCert(&cert);
            return ret;
        }

#ifndef WOLFSSL_NO_OCSP_ISSUER_CHECK
        if ((cert.extExtKeyUsage & EXTKEYUSE_OCSP_SIGN) == 0) {
            if (XMEMCMP(cert.subjectHash,
                        resp->issuerHash, KEYID_SIZE) == 0) {
                WOLFSSL_MSG("\tOCSP Response signed by issuer");
            }
            else {
                WOLFSSL_MSG("\tOCSP Responder key usage check failed");

                FreeDecodedCert(&cert);
                return BAD_OCSP_RESPONDER;
            }
        }
#endif

        /* ConfirmSignature is blocking here */
        ret = ConfirmSignature(&cert.sigCtx,
            resp->response, resp->responseSz,
            cert.publicKey, cert.pubKeySize, cert.keyOID,
            resp->sig, resp->sigSz, resp->sigOID);
        FreeDecodedCert(&cert);

        if (ret != 0) {
            WOLFSSL_MSG("\tOCSP Confirm signature failed");
            return ASN_OCSP_CONFIRM_E;
        }
    }
    else
#endif /* WOLFSSL_NO_OCSP_OPTIONAL_CERTS */
    {
        Signer* ca = NULL;
        int sigValid = -1;

        #ifndef NO_SKID
            ca = GetCA(cm, resp->issuerKeyHash);
        #else
            ca = GetCA(cm, resp->issuerHash);
        #endif

        if (ca) {
            SignatureCtx sigCtx;
            InitSignatureCtx(&sigCtx, heap, INVALID_DEVID);

            /* ConfirmSignature is blocking here */
            sigValid = ConfirmSignature(&sigCtx, resp->response,
                resp->responseSz, ca->publicKey, ca->pubKeySize, ca->keyOID,
                                resp->sig, resp->sigSz, resp->sigOID);
        }
        if (ca == NULL || sigValid != 0) {
            WOLFSSL_MSG("\tOCSP Confirm signature failed");
            return ASN_OCSP_CONFIRM_E;
        }

        (void)noVerify;
    }

    *ioIndex = idx;
    return 0;
}


void InitOcspResponse(OcspResponse* resp, CertStatus* status,
                                                    byte* source, word32 inSz)
{
    WOLFSSL_ENTER("InitOcspResponse");

    XMEMSET(status, 0, sizeof(CertStatus));
    XMEMSET(resp,   0, sizeof(OcspResponse));

    resp->responseStatus = -1;
    resp->status         = status;
    resp->source         = source;
    resp->maxIdx         = inSz;
}


int OcspResponseDecode(OcspResponse* resp, void* cm, void* heap, int noVerify)
{
    int ret;
    int length = 0;
    word32 idx = 0;
    byte* source = resp->source;
    word32 size = resp->maxIdx;
    word32 oid;

    WOLFSSL_ENTER("OcspResponseDecode");

    /* peel the outer SEQUENCE wrapper */
    if (GetSequence(source, &idx, &length, size) < 0)
        return ASN_PARSE_E;

    /* First get the responseStatus, an ENUMERATED */
    if (GetEnumerated(source, &idx, &resp->responseStatus) < 0)
        return ASN_PARSE_E;

    if (resp->responseStatus != OCSP_SUCCESSFUL)
        return 0;

    /* Next is an EXPLICIT record called ResponseBytes, OPTIONAL */
    if (idx >= size)
        return ASN_INPUT_E;
    if (source[idx++] != (ASN_CONSTRUCTED | ASN_CONTEXT_SPECIFIC))
        return ASN_PARSE_E;
    if (GetLength(source, &idx, &length, size) < 0)
        return ASN_PARSE_E;

    /* Get the responseBytes SEQUENCE */
    if (GetSequence(source, &idx, &length, size) < 0)
        return ASN_PARSE_E;

    /* Check ObjectID for the resposeBytes */
    if (GetObjectId(source, &idx, &oid, oidOcspType, size) < 0)
        return ASN_PARSE_E;
    if (oid != OCSP_BASIC_OID)
        return ASN_PARSE_E;
    ret = GetOctetString(source, &idx, &length, size);
    if (ret < 0)
        return ret;

    ret = DecodeBasicOcspResponse(source, &idx, resp, size, cm, heap, noVerify);
    if (ret < 0)
        return ret;

    return 0;
}


word32 EncodeOcspRequestExtensions(OcspRequest* req, byte* output, word32 size)
{
    static const byte NonceObjId[] = { 0x2b, 0x06, 0x01, 0x05, 0x05, 0x07,
                                       0x30, 0x01, 0x02 };
    byte seqArray[5][MAX_SEQ_SZ];
    word32 seqSz[5], totalSz = (word32)sizeof(NonceObjId);

    WOLFSSL_ENTER("SetOcspReqExtensions");

    if (!req || !output || !req->nonceSz)
        return 0;

    totalSz += req->nonceSz;
    totalSz += seqSz[0] = SetOctetString(req->nonceSz, seqArray[0]);
    totalSz += seqSz[1] = SetOctetString(req->nonceSz + seqSz[0], seqArray[1]);
    totalSz += seqSz[2] = SetObjectId(sizeof(NonceObjId), seqArray[2]);
    totalSz += seqSz[3] = SetSequence(totalSz, seqArray[3]);
    totalSz += seqSz[4] = SetSequence(totalSz, seqArray[4]);

    if (totalSz > size)
        return 0;

    totalSz = 0;

    XMEMCPY(output + totalSz, seqArray[4], seqSz[4]);
    totalSz += seqSz[4];

    XMEMCPY(output + totalSz, seqArray[3], seqSz[3]);
    totalSz += seqSz[3];

    XMEMCPY(output + totalSz, seqArray[2], seqSz[2]);
    totalSz += seqSz[2];

    XMEMCPY(output + totalSz, NonceObjId, sizeof(NonceObjId));
    totalSz += (word32)sizeof(NonceObjId);

    XMEMCPY(output + totalSz, seqArray[1], seqSz[1]);
    totalSz += seqSz[1];

    XMEMCPY(output + totalSz, seqArray[0], seqSz[0]);
    totalSz += seqSz[0];

    XMEMCPY(output + totalSz, req->nonce, req->nonceSz);
    totalSz += req->nonceSz;

    return totalSz;
}


int EncodeOcspRequest(OcspRequest* req, byte* output, word32 size)
{
    byte seqArray[5][MAX_SEQ_SZ];
    /* The ASN.1 of the OCSP Request is an onion of sequences */
    byte algoArray[MAX_ALGO_SZ];
    byte issuerArray[MAX_ENCODED_DIG_SZ];
    byte issuerKeyArray[MAX_ENCODED_DIG_SZ];
    byte snArray[MAX_SN_SZ];
    byte extArray[MAX_OCSP_EXT_SZ];
    word32 seqSz[5], algoSz, issuerSz, issuerKeySz, snSz, extSz, totalSz;
    int i;

    WOLFSSL_ENTER("EncodeOcspRequest");

#ifdef NO_SHA
    algoSz = SetAlgoID(SHA256h, algoArray, oidHashType, 0);
#else
    algoSz = SetAlgoID(SHAh, algoArray, oidHashType, 0);
#endif

    issuerSz    = SetDigest(req->issuerHash,    KEYID_SIZE,    issuerArray);
    issuerKeySz = SetDigest(req->issuerKeyHash, KEYID_SIZE,    issuerKeyArray);
    snSz        = SetSerialNumber(req->serial,  req->serialSz, snArray);
    extSz       = 0;

    if (req->nonceSz) {
        /* TLS Extensions use this function too - put extensions after
         * ASN.1: Context Specific [2].
         */
        extSz = EncodeOcspRequestExtensions(req, extArray + 2,
                                            OCSP_NONCE_EXT_SZ);
        extSz += SetExplicit(2, extSz, extArray);
    }

    totalSz = algoSz + issuerSz + issuerKeySz + snSz;
    for (i = 4; i >= 0; i--) {
        seqSz[i] = SetSequence(totalSz, seqArray[i]);
        totalSz += seqSz[i];
        if (i == 2) totalSz += extSz;
    }

    if (output == NULL)
        return totalSz;
    if (totalSz > size)
        return BUFFER_E;

    totalSz = 0;
    for (i = 0; i < 5; i++) {
        XMEMCPY(output + totalSz, seqArray[i], seqSz[i]);
        totalSz += seqSz[i];
    }

    XMEMCPY(output + totalSz, algoArray, algoSz);
    totalSz += algoSz;

    XMEMCPY(output + totalSz, issuerArray, issuerSz);
    totalSz += issuerSz;

    XMEMCPY(output + totalSz, issuerKeyArray, issuerKeySz);
    totalSz += issuerKeySz;

    XMEMCPY(output + totalSz, snArray, snSz);
    totalSz += snSz;

    if (extSz != 0) {
        XMEMCPY(output + totalSz, extArray, extSz);
        totalSz += extSz;
    }

    return totalSz;
}


int InitOcspRequest(OcspRequest* req, DecodedCert* cert, byte useNonce,
                                                                     void* heap)
{
    int ret;

    WOLFSSL_ENTER("InitOcspRequest");

    if (req == NULL)
        return BAD_FUNC_ARG;

    ForceZero(req, sizeof(OcspRequest));
    req->heap = heap;

    if (cert) {
        XMEMCPY(req->issuerHash,    cert->issuerHash,    KEYID_SIZE);
        XMEMCPY(req->issuerKeyHash, cert->issuerKeyHash, KEYID_SIZE);

        req->serial = (byte*)XMALLOC(cert->serialSz, req->heap,
                                                     DYNAMIC_TYPE_OCSP_REQUEST);
        if (req->serial == NULL)
            return MEMORY_E;

        XMEMCPY(req->serial, cert->serial, cert->serialSz);
        req->serialSz = cert->serialSz;

        if (cert->extAuthInfoSz != 0 && cert->extAuthInfo != NULL) {
            req->url = (byte*)XMALLOC(cert->extAuthInfoSz, req->heap,
                                                     DYNAMIC_TYPE_OCSP_REQUEST);
            if (req->url == NULL) {
                XFREE(req->serial, req->heap, DYNAMIC_TYPE_OCSP);
                return MEMORY_E;
            }

            XMEMCPY(req->url, cert->extAuthInfo, cert->extAuthInfoSz);
            req->urlSz = cert->extAuthInfoSz;
        }
    }

    if (useNonce) {
        WC_RNG rng;

    #ifndef HAVE_FIPS
        ret = wc_InitRng_ex(&rng, req->heap, INVALID_DEVID);
    #else
        ret = wc_InitRng(&rng);
    #endif
        if (ret != 0) {
            WOLFSSL_MSG("\tCannot initialize RNG. Skipping the OSCP Nonce.");
        } else {
            if (wc_RNG_GenerateBlock(&rng, req->nonce, MAX_OCSP_NONCE_SZ) != 0)
                WOLFSSL_MSG("\tCannot run RNG. Skipping the OSCP Nonce.");
            else
                req->nonceSz = MAX_OCSP_NONCE_SZ;

            wc_FreeRng(&rng);
        }
    }

    return 0;
}

void FreeOcspRequest(OcspRequest* req)
{
    WOLFSSL_ENTER("FreeOcspRequest");

    if (req) {
        if (req->serial)
            XFREE(req->serial, req->heap, DYNAMIC_TYPE_OCSP_REQUEST);

        if (req->url)
            XFREE(req->url, req->heap, DYNAMIC_TYPE_OCSP_REQUEST);
    }
}


int CompareOcspReqResp(OcspRequest* req, OcspResponse* resp)
{
    int cmp;

    WOLFSSL_ENTER("CompareOcspReqResp");

    if (req == NULL)
    {
        WOLFSSL_MSG("\tReq missing");
        return -1;
    }

    if (resp == NULL)
    {
        WOLFSSL_MSG("\tResp missing");
        return 1;
    }

    /* Nonces are not critical. The responder may not necessarily add
     * the nonce to the response. */
    if (resp->nonceSz != 0) {
        cmp = req->nonceSz - resp->nonceSz;
        if (cmp != 0)
        {
            WOLFSSL_MSG("\tnonceSz mismatch");
            return cmp;
        }

        cmp = XMEMCMP(req->nonce, resp->nonce, req->nonceSz);
        if (cmp != 0)
        {
            WOLFSSL_MSG("\tnonce mismatch");
            return cmp;
        }
    }

    cmp = XMEMCMP(req->issuerHash, resp->issuerHash, KEYID_SIZE);
    if (cmp != 0)
    {
        WOLFSSL_MSG("\tissuerHash mismatch");
        return cmp;
    }

    cmp = XMEMCMP(req->issuerKeyHash, resp->issuerKeyHash, KEYID_SIZE);
    if (cmp != 0)
    {
        WOLFSSL_MSG("\tissuerKeyHash mismatch");
        return cmp;
    }

    cmp = req->serialSz - resp->status->serialSz;
    if (cmp != 0)
    {
        WOLFSSL_MSG("\tserialSz mismatch");
        return cmp;
    }

    cmp = XMEMCMP(req->serial, resp->status->serial, req->serialSz);
    if (cmp != 0)
    {
        WOLFSSL_MSG("\tserial mismatch");
        return cmp;
    }

    return 0;
}

#endif


/* store SHA hash of NAME */
WOLFSSL_LOCAL int GetNameHash(const byte* source, word32* idx, byte* hash,
                             int maxIdx)
{
    int    length;  /* length of all distinguished names */
    int    ret;
    word32 dummy;

    WOLFSSL_ENTER("GetNameHash");

    if (source[*idx] == ASN_OBJECT_ID) {
        WOLFSSL_MSG("Trying optional prefix...");

        if (GetLength(source, idx, &length, maxIdx) < 0)
            return ASN_PARSE_E;

        *idx += length;
        WOLFSSL_MSG("Got optional prefix");
    }

    /* For OCSP, RFC2560 section 4.1.1 states the issuer hash should be
     * calculated over the entire DER encoding of the Name field, including
     * the tag and length. */
    dummy = *idx;
    if (GetSequence(source, idx, &length, maxIdx) < 0)
        return ASN_PARSE_E;

#ifdef NO_SHA
    ret = wc_Sha256Hash(source + dummy, length + *idx - dummy, hash);
#else
    ret = wc_ShaHash(source + dummy, length + *idx - dummy, hash);
#endif

    *idx += length;

    return ret;
}


#ifdef HAVE_CRL

/* initialize decoded CRL */
void InitDecodedCRL(DecodedCRL* dcrl, void* heap)
{
    WOLFSSL_MSG("InitDecodedCRL");

    dcrl->certBegin    = 0;
    dcrl->sigIndex     = 0;
    dcrl->sigLength    = 0;
    dcrl->signatureOID = 0;
    dcrl->certs        = NULL;
    dcrl->totalCerts   = 0;
    dcrl->heap         = heap;
    #ifdef WOLFSSL_HEAP_TEST
        dcrl->heap = (void*)WOLFSSL_HEAP_TEST;
    #endif
}


/* free decoded CRL resources */
void FreeDecodedCRL(DecodedCRL* dcrl)
{
    RevokedCert* tmp = dcrl->certs;

    WOLFSSL_MSG("FreeDecodedCRL");

    while(tmp) {
        RevokedCert* next = tmp->next;
        XFREE(tmp, dcrl->heap, DYNAMIC_TYPE_REVOKED);
        tmp = next;
    }
}


/* Get Revoked Cert list, 0 on success */
static int GetRevoked(const byte* buff, word32* idx, DecodedCRL* dcrl,
                      int maxIdx)
{
    int    len;
    word32 end;
    byte   b;
    RevokedCert* rc;

    WOLFSSL_ENTER("GetRevoked");

    if (GetSequence(buff, idx, &len, maxIdx) < 0)
        return ASN_PARSE_E;

    end = *idx + len;

    rc = (RevokedCert*)XMALLOC(sizeof(RevokedCert), dcrl->heap,
                                                          DYNAMIC_TYPE_REVOKED);
    if (rc == NULL) {
        WOLFSSL_MSG("Alloc Revoked Cert failed");
        return MEMORY_E;
    }

    if (GetSerialNumber(buff, idx, rc->serialNumber, &rc->serialSz,
                                                                maxIdx) < 0) {
        XFREE(rc, dcrl->heap, DYNAMIC_TYPE_REVOKED);
        return ASN_PARSE_E;
    }

    /* add to list */
    rc->next = dcrl->certs;
    dcrl->certs = rc;
    dcrl->totalCerts++;


    /* get date */
    b = buff[*idx];
    *idx += 1;

    if (b != ASN_UTC_TIME && b != ASN_GENERALIZED_TIME) {
        WOLFSSL_MSG("Expecting Date");
        return ASN_PARSE_E;
    }

    if (GetLength(buff, idx, &len, maxIdx) < 0)
        return ASN_PARSE_E;

    /* skip for now */
    *idx += len;

    if (*idx != end)  /* skip extensions */
        *idx = end;

    return 0;
}


/* Get CRL Signature, 0 on success */
static int GetCRL_Signature(const byte* source, word32* idx, DecodedCRL* dcrl,
                            int maxIdx)
{
    int    length;
    int    ret;

    WOLFSSL_ENTER("GetCRL_Signature");

    ret = CheckBitString(source, idx, &length, maxIdx, 1, NULL);
    if (ret != 0)
        return ret;
    dcrl->sigLength = length;

    dcrl->signature = (byte*)&source[*idx];
    *idx += dcrl->sigLength;

    return 0;
}

int VerifyCRL_Signature(SignatureCtx* sigCtx, const byte* toBeSigned,
                        word32 tbsSz, const byte* signature, word32 sigSz,
                        word32 signatureOID, Signer *ca, void* heap)
{
    /* try to confirm/verify signature */
#ifndef IGNORE_KEY_EXTENSIONS
    if ((ca->keyUsage & KEYUSE_CRL_SIGN) == 0) {
        WOLFSSL_MSG("CA cannot sign CRLs");
        return ASN_CRL_NO_SIGNER_E;
    }
#endif /* IGNORE_KEY_EXTENSIONS */

    InitSignatureCtx(sigCtx, heap, INVALID_DEVID);
    if (ConfirmSignature(sigCtx, toBeSigned, tbsSz, ca->publicKey,
                         ca->pubKeySize, ca->keyOID, signature, sigSz,
                         signatureOID) != 0) {
        WOLFSSL_MSG("CRL Confirm signature failed");
        return ASN_CRL_CONFIRM_E;
    }

    return 0;
}

/* prase crl buffer into decoded state, 0 on success */
int ParseCRL(DecodedCRL* dcrl, const byte* buff, word32 sz, void* cm)
{
    int          version, len, doNextDate = 1;
    word32       oid, idx = 0, dateIdx;
    Signer*      ca = NULL;
    SignatureCtx sigCtx;

    WOLFSSL_MSG("ParseCRL");

    /* raw crl hash */
    /* hash here if needed for optimized comparisons
     * Sha     sha;
     * wc_InitSha(&sha);
     * wc_ShaUpdate(&sha, buff, sz);
     * wc_ShaFinal(&sha, dcrl->crlHash); */

    if (GetSequence(buff, &idx, &len, sz) < 0)
        return ASN_PARSE_E;

    dcrl->certBegin = idx;

    if (GetSequence(buff, &idx, &len, sz) < 0)
        return ASN_PARSE_E;
    dcrl->sigIndex = len + idx;

    /* may have version */
    if (buff[idx] == ASN_INTEGER) {
        if (GetMyVersion(buff, &idx, &version, sz) < 0)
            return ASN_PARSE_E;
    }

    if (GetAlgoId(buff, &idx, &oid, oidIgnoreType, sz) < 0)
        return ASN_PARSE_E;

    if (GetNameHash(buff, &idx, dcrl->issuerHash, sz) < 0)
        return ASN_PARSE_E;

    if (GetBasicDate(buff, &idx, dcrl->lastDate, &dcrl->lastDateFormat, sz) < 0)
        return ASN_PARSE_E;

    dateIdx = idx;

    if (GetBasicDate(buff, &idx, dcrl->nextDate, &dcrl->nextDateFormat, sz) < 0)
    {
#ifndef WOLFSSL_NO_CRL_NEXT_DATE
        (void)dateIdx;
        return ASN_PARSE_E;
#else
        dcrl->nextDateFormat = ASN_OTHER_TYPE;  /* skip flag */
        doNextDate = 0;
        idx = dateIdx;
#endif
    }

    if (doNextDate) {
#ifndef NO_ASN_TIME
        if (!XVALIDATE_DATE(dcrl->nextDate, dcrl->nextDateFormat, AFTER)) {
            WOLFSSL_MSG("CRL after date is no longer valid");
            return ASN_AFTER_DATE_E;
        }
#endif
    }

    if (idx != dcrl->sigIndex && buff[idx] != CRL_EXTENSIONS) {
        if (GetSequence(buff, &idx, &len, sz) < 0)
            return ASN_PARSE_E;

        len += idx;

        while (idx < (word32)len) {
            if (GetRevoked(buff, &idx, dcrl, sz) < 0)
                return ASN_PARSE_E;
        }
    }

    if (idx != dcrl->sigIndex)
        idx = dcrl->sigIndex;   /* skip extensions */

    if (GetAlgoId(buff, &idx, &dcrl->signatureOID, oidSigType, sz) < 0)
        return ASN_PARSE_E;

    if (GetCRL_Signature(buff, &idx, dcrl, sz) < 0)
        return ASN_PARSE_E;

    /* openssl doesn't add skid by default for CRLs cause firefox chokes
       we're not assuming it's available yet */
#if !defined(NO_SKID) && defined(CRL_SKID_READY)
    if (dcrl->extAuthKeyIdSet)
        ca = GetCA(cm, dcrl->extAuthKeyId);
    if (ca == NULL)
        ca = GetCAByName(cm, dcrl->issuerHash);
#else
    ca = GetCA(cm, dcrl->issuerHash);
#endif /* !NO_SKID && CRL_SKID_READY */
    WOLFSSL_MSG("About to verify CRL signature");

    if (ca == NULL) {
        WOLFSSL_MSG("Did NOT find CRL issuer CA");
        return ASN_CRL_NO_SIGNER_E;
    }

    WOLFSSL_MSG("Found CRL issuer CA");
    return VerifyCRL_Signature(&sigCtx, buff + dcrl->certBegin,
           dcrl->sigIndex - dcrl->certBegin, dcrl->signature, dcrl->sigLength,
           dcrl->signatureOID, ca, dcrl->heap);
}

#endif /* HAVE_CRL */

#undef ERROR_OUT

#endif /* !NO_ASN */

#ifdef WOLFSSL_SEP


#endif /* WOLFSSL_SEP */
