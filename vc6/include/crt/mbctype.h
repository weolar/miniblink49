/***
*mbctype.h - MBCS character conversion macros
*
*       Copyright (c) Microsoft Corporation.  All rights reserved.
*
*Purpose:
*       Defines macros for MBCS character classification/conversion.
*
*       [Public]
*
****/

#if     _MSC_VER > 1000
#pragma once
#endif

#ifndef _INC_MBCTYPE
#define _INC_MBCTYPE

#if     !defined(_WIN32)
#error ERROR: Only Win32 target supported!
#endif


/* include the standard ctype.h header file */

#include <ctype.h>

#ifdef  __cplusplus
extern "C" {
#endif


/* Define _CRTIMP */

#ifndef _CRTIMP
#ifdef  _DLL
#define _CRTIMP __declspec(dllimport)
#else   /* ndef _DLL */
#define _CRTIMP
#endif  /* _DLL */
#endif  /* _CRTIMP */


/* Define __cdecl for non-Microsoft compilers */

#if     (!defined(_MSC_VER) && !defined(__cdecl))
#define __cdecl
#endif


/*
 * MBCS - Multi-Byte Character Set
 */

/*
 * This declaration allows the user access the _mbctype[] look-up array.
 */
_CRTIMP extern unsigned char _mbctype[];
_CRTIMP extern unsigned char _mbcasemap[];


/* bit masks for MBCS character types */

#define _MS     0x01    /* MBCS single-byte symbol */
#define _MP     0x02    /* MBCS punct */
#define _M1     0x04    /* MBCS 1st (lead) byte */
#define _M2     0x08    /* MBCS 2nd byte*/

#define _SBUP   0x10    /* SBCS upper char */
#define _SBLOW  0x20    /* SBCS lower char */

/* byte types  */

#define _MBC_SINGLE     0       /* valid single byte char */
#define _MBC_LEAD       1       /* lead byte */
#define _MBC_TRAIL      2       /* trailing byte */
#define _MBC_ILLEGAL    (-1)    /* illegal byte */

#define _KANJI_CP   932

/* _setmbcp parameter defines */
#define _MB_CP_SBCS     0
#define _MB_CP_OEM      -2
#define _MB_CP_ANSI     -3
#define _MB_CP_LOCALE   -4


#ifndef _MBCTYPE_DEFINED

/* MB control routines */

_CRTIMP int __cdecl _setmbcp(int);
_CRTIMP int __cdecl _getmbcp(void);


/* MBCS character classification function prototypes */


/* byte routines */
_CRTIMP int __cdecl _ismbbkalnum( unsigned int );
_CRTIMP int __cdecl _ismbbkana( unsigned int );
_CRTIMP int __cdecl _ismbbkpunct( unsigned int );
_CRTIMP int __cdecl _ismbbkprint( unsigned int );
_CRTIMP int __cdecl _ismbbalpha( unsigned int );
_CRTIMP int __cdecl _ismbbpunct( unsigned int );
_CRTIMP int __cdecl _ismbbalnum( unsigned int );
_CRTIMP int __cdecl _ismbbprint( unsigned int );
_CRTIMP int __cdecl _ismbbgraph( unsigned int );

#ifndef _MBLEADTRAIL_DEFINED
_CRTIMP int __cdecl _ismbblead( unsigned int );
_CRTIMP int __cdecl _ismbbtrail( unsigned int );
_CRTIMP int __cdecl _ismbslead( const unsigned char *, const unsigned char *);
_CRTIMP int __cdecl _ismbstrail( const unsigned char *, const unsigned char *);
#define _MBLEADTRAIL_DEFINED
#endif

#define _MBCTYPE_DEFINED
#endif

/*
 * char byte classification macros
 */

#define _ismbbkalnum(_c)    ((_mbctype+1)[(unsigned char)(_c)] & _MS)
#define _ismbbkprint(_c)    ((_mbctype+1)[(unsigned char)(_c)] & (_MS|_MP))
#define _ismbbkpunct(_c)    ((_mbctype+1)[(unsigned char)(_c)] & _MP)

#define _ismbbalnum(_c) (((_pctype)[(unsigned char)(_c)] & (_ALPHA|_DIGIT))||_ismbbkalnum(_c))
#define _ismbbalpha(_c) (((_pctype)[(unsigned char)(_c)] & (_ALPHA))||_ismbbkalnum(_c))
#define _ismbbgraph(_c) (((_pctype)[(unsigned char)(_c)] & (_PUNCT|_ALPHA|_DIGIT))||_ismbbkprint(_c))
#define _ismbbprint(_c) (((_pctype)[(unsigned char)(_c)] & (_BLANK|_PUNCT|_ALPHA|_DIGIT))||_ismbbkprint(_c))
#define _ismbbpunct(_c) (((_pctype)[(unsigned char)(_c)] & _PUNCT)||_ismbbkpunct(_c))

#define _ismbblead(_c)  ((_mbctype+1)[(unsigned char)(_c)] & _M1)
#define _ismbbtrail(_c) ((_mbctype+1)[(unsigned char)(_c)] & _M2)

#define _ismbbkana(_c)  ((_mbctype+1)[(unsigned char)(_c)] & (_MS|_MP))


#ifdef  __cplusplus
}
#endif

#endif  /* _INC_MBCTYPE */

