
/*****************************************************************************\
*                                                                             *
* shlwapi.h - Interface for the Windows light-weight utility APIs             *
*                                                                             *
* Version 1.0                                                                 *
*                                                                             *
* Copyright (c) Microsoft Corporation. All rights reserved.                   *
*                                                                             *
\*****************************************************************************/


#ifndef _INC_SHLWAPI
#define _INC_SHLWAPI

#ifndef NOSHLWAPI

#include <objbase.h>
#include <shtypes.h>


#ifndef _WINRESRC_
#ifndef _WIN32_IE
#define _WIN32_IE 0x0501
#else
#if (_WIN32_IE < 0x0400) && defined(_WIN32_WINNT) && (_WIN32_WINNT >= 0x0500)
#error _WIN32_IE setting conflicts with _WIN32_WINNT setting
#endif
#endif
#endif


#ifdef UNIX
typedef interface IInternetSecurityMgrSite IInternetSecurityMgrSite;
typedef interface IInternetSecurityManager IInternetSecurityManager;
typedef interface IInternetHostSecurityManager IInternetHostSecurityManager;
#endif

//
// Define API decoration for direct importing of DLL references.
//
#ifndef WINSHLWAPI
#if !defined(_SHLWAPI_)
#define LWSTDAPI          EXTERN_C DECLSPEC_IMPORT HRESULT STDAPICALLTYPE
#define LWSTDAPI_(type)   EXTERN_C DECLSPEC_IMPORT type STDAPICALLTYPE
#define LWSTDAPIV         EXTERN_C DECLSPEC_IMPORT HRESULT STDAPIVCALLTYPE
#define LWSTDAPIV_(type)  EXTERN_C DECLSPEC_IMPORT type STDAPIVCALLTYPE
#else
#define LWSTDAPI          STDAPI
#define LWSTDAPI_(type)   STDAPI_(type)
#define LWSTDAPIV         STDAPIV
#define LWSTDAPIV_(type)  STDAPIV_(type)
#endif
#endif // WINSHLWAPI

#ifdef _WIN32
#include <pshpack8.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

//
// Users of this header may define any number of these constants to avoid
// the definitions of each functional group.
//
//    NO_SHLWAPI_STRFCNS    String functions
//    NO_SHLWAPI_PATH       Path functions
//    NO_SHLWAPI_REG        Registry functions
//    NO_SHLWAPI_STREAM     Stream functions
//    NO_SHLWAPI_GDI        GDI helper functions

#ifndef NO_SHLWAPI_STRFCNS
//
//=============== String Routines ===================================
//

LWSTDAPI_(LPSTR)    StrChrA(LPCSTR lpStart, WORD wMatch);
LWSTDAPI_(LPWSTR)   StrChrW(LPCWSTR lpStart, WCHAR wMatch);
LWSTDAPI_(LPSTR)    StrChrIA(LPCSTR lpStart, WORD wMatch);
LWSTDAPI_(LPWSTR)   StrChrIW(LPCWSTR lpStart, WCHAR wMatch);
LWSTDAPI_(int)      StrCmpNA(LPCSTR lpStr1, LPCSTR lpStr2, int nChar);
LWSTDAPI_(int)      StrCmpNW(LPCWSTR lpStr1, LPCWSTR lpStr2, int nChar);
LWSTDAPI_(int)      StrCmpNIA(LPCSTR lpStr1, LPCSTR lpStr2, int nChar);
LWSTDAPI_(int)      StrCmpNIW(LPCWSTR lpStr1, LPCWSTR lpStr2, int nChar);
LWSTDAPI_(int)      StrCSpnA(LPCSTR lpStr, LPCSTR lpSet);
LWSTDAPI_(int)      StrCSpnW(LPCWSTR lpStr, LPCWSTR lpSet);
LWSTDAPI_(int)      StrCSpnIA(LPCSTR lpStr, LPCSTR lpSet);
LWSTDAPI_(int)      StrCSpnIW(LPCWSTR lpStr, LPCWSTR lpSet);
LWSTDAPI_(LPSTR)    StrDupA(LPCSTR lpSrch);
LWSTDAPI_(LPWSTR)   StrDupW(LPCWSTR lpSrch);
LWSTDAPI_(LPSTR)    StrFormatByteSizeA(DWORD dw, LPSTR szBuf, UINT uiBufSize);
LWSTDAPI_(LPSTR)    StrFormatByteSize64A(LONGLONG qdw, LPSTR szBuf, UINT uiBufSize);
LWSTDAPI_(LPWSTR)   StrFormatByteSizeW(LONGLONG qdw, LPWSTR szBuf, UINT uiBufSize);
LWSTDAPI_(LPWSTR)   StrFormatKBSizeW(LONGLONG qdw, LPWSTR szBuf, UINT uiBufSize);
LWSTDAPI_(LPSTR)    StrFormatKBSizeA(LONGLONG qdw, LPSTR szBuf, UINT uiBufSize);
LWSTDAPI_(int)      StrFromTimeIntervalA(LPSTR pszOut, UINT cchMax, DWORD dwTimeMS, int digits);
LWSTDAPI_(int)      StrFromTimeIntervalW(LPWSTR pszOut, UINT cchMax, DWORD dwTimeMS, int digits);
LWSTDAPI_(BOOL)     StrIsIntlEqualA(BOOL fCaseSens, LPCSTR lpString1, LPCSTR lpString2, int nChar);
LWSTDAPI_(BOOL)     StrIsIntlEqualW(BOOL fCaseSens, LPCWSTR lpString1, LPCWSTR lpString2, int nChar);
LWSTDAPI_(LPSTR)    StrNCatA(LPSTR psz1, LPCSTR psz2, int cchMax);
LWSTDAPI_(LPWSTR)   StrNCatW(LPWSTR psz1, LPCWSTR psz2, int cchMax);
LWSTDAPI_(LPSTR)    StrPBrkA(LPCSTR psz, LPCSTR pszSet);
LWSTDAPI_(LPWSTR)   StrPBrkW(LPCWSTR psz, LPCWSTR pszSet);
LWSTDAPI_(LPSTR)    StrRChrA(LPCSTR lpStart, LPCSTR lpEnd, WORD wMatch);
LWSTDAPI_(LPWSTR)   StrRChrW(LPCWSTR lpStart, LPCWSTR lpEnd, WCHAR wMatch);
LWSTDAPI_(LPSTR)    StrRChrIA(LPCSTR lpStart, LPCSTR lpEnd, WORD wMatch);
LWSTDAPI_(LPWSTR)   StrRChrIW(LPCWSTR lpStart, LPCWSTR lpEnd, WCHAR wMatch);
LWSTDAPI_(LPSTR)    StrRStrIA(LPCSTR lpSource, LPCSTR lpLast, LPCSTR lpSrch);
LWSTDAPI_(LPWSTR)   StrRStrIW(LPCWSTR lpSource, LPCWSTR lpLast, LPCWSTR lpSrch);
LWSTDAPI_(int)      StrSpnA(LPCSTR psz, LPCSTR pszSet);
LWSTDAPI_(int)      StrSpnW(LPCWSTR psz, LPCWSTR pszSet);
LWSTDAPI_(LPSTR)    StrStrA(LPCSTR lpFirst, LPCSTR lpSrch);
LWSTDAPI_(LPWSTR)   StrStrW(LPCWSTR lpFirst, LPCWSTR lpSrch);
LWSTDAPI_(LPSTR)    StrStrIA(LPCSTR lpFirst, LPCSTR lpSrch);
LWSTDAPI_(LPWSTR)   StrStrIW(LPCWSTR lpFirst, LPCWSTR lpSrch);
LWSTDAPI_(int)      StrToIntA(LPCSTR lpSrc);
LWSTDAPI_(int)      StrToIntW(LPCWSTR lpSrc);
LWSTDAPI_(BOOL)     StrToIntExA(LPCSTR pszString, DWORD dwFlags, int * piRet);
LWSTDAPI_(BOOL)     StrToIntExW(LPCWSTR pszString, DWORD dwFlags, int * piRet);
#if (_WIN32_IE >= 0x0600)
LWSTDAPI_(BOOL)     StrToInt64ExA(LPCSTR pszString, DWORD dwFlags, LONGLONG * pllRet);
LWSTDAPI_(BOOL)     StrToInt64ExW(LPCWSTR pszString, DWORD dwFlags, LONGLONG * pllRet);
#endif
LWSTDAPI_(BOOL)     StrTrimA(LPSTR psz, LPCSTR pszTrimChars);
LWSTDAPI_(BOOL)     StrTrimW(LPWSTR psz, LPCWSTR pszTrimChars);

LWSTDAPI_(LPWSTR)   StrCatW(LPWSTR psz1, LPCWSTR psz2);
LWSTDAPI_(int)      StrCmpW(LPCWSTR psz1, LPCWSTR psz2);
LWSTDAPI_(int)      StrCmpIW(LPCWSTR psz1, LPCWSTR psz2);
LWSTDAPI_(LPWSTR)   StrCpyW(LPWSTR psz1, LPCWSTR psz2);
LWSTDAPI_(LPWSTR)   StrCpyNW(LPWSTR psz1, LPCWSTR psz2, int cchMax);

LWSTDAPI_(LPWSTR)   StrCatBuffW(LPWSTR pszDest, LPCWSTR pszSrc, int cchDestBuffSize);
LWSTDAPI_(LPSTR)    StrCatBuffA(LPSTR pszDest, LPCSTR pszSrc, int cchDestBuffSize);

LWSTDAPI_(BOOL)     ChrCmpIA(WORD w1, WORD w2);
LWSTDAPI_(BOOL)     ChrCmpIW(WCHAR w1, WCHAR w2);

LWSTDAPI_(int)      wvnsprintfA(LPSTR lpOut, int cchLimitIn, LPCSTR lpFmt, va_list arglist);
LWSTDAPI_(int)      wvnsprintfW(LPWSTR lpOut, int cchLimitIn, LPCWSTR lpFmt, va_list arglist);
LWSTDAPIV_(int)     wnsprintfA(LPSTR lpOut, int cchLimitIn, LPCSTR lpFmt, ...);
LWSTDAPIV_(int)     wnsprintfW(LPWSTR lpOut, int cchLimitIn, LPCWSTR lpFmt, ...);

#define StrIntlEqNA( s1, s2, nChar) StrIsIntlEqualA( TRUE, s1, s2, nChar)
#define StrIntlEqNW( s1, s2, nChar) StrIsIntlEqualW( TRUE, s1, s2, nChar)
#define StrIntlEqNIA(s1, s2, nChar) StrIsIntlEqualA(FALSE, s1, s2, nChar)
#define StrIntlEqNIW(s1, s2, nChar) StrIsIntlEqualW(FALSE, s1, s2, nChar)

LWSTDAPI StrRetToStrA(STRRET *pstr, LPCITEMIDLIST pidl, LPSTR *ppsz);
LWSTDAPI StrRetToStrW(STRRET *pstr, LPCITEMIDLIST pidl, LPWSTR *ppsz);
#ifdef UNICODE
#define StrRetToStr  StrRetToStrW
#else
#define StrRetToStr  StrRetToStrA
#endif // !UNICODE
LWSTDAPI StrRetToBufA(STRRET *pstr, LPCITEMIDLIST pidl, LPSTR pszBuf, UINT cchBuf);
LWSTDAPI StrRetToBufW(STRRET *pstr, LPCITEMIDLIST pidl, LPWSTR pszBuf, UINT cchBuf);
#ifdef UNICODE
#define StrRetToBuf  StrRetToBufW
#else
#define StrRetToBuf  StrRetToBufA
#endif // !UNICODE
LWSTDAPI StrRetToBSTR(STRRET *pstr, LPCITEMIDLIST pidl, BSTR *pbstr);

// helper to duplicate a string using the task allocator

LWSTDAPI SHStrDupA(LPCSTR psz, WCHAR **ppwsz);
LWSTDAPI SHStrDupW(LPCWSTR psz, WCHAR **ppwsz);
#ifdef UNICODE
#define SHStrDup  SHStrDupW
#else
#define SHStrDup  SHStrDupA
#endif // !UNICODE

LWSTDAPI_(int) StrCmpLogicalW(LPCWSTR psz1, LPCWSTR psz2);
LWSTDAPI_(DWORD) StrCatChainW(LPWSTR pszDst, DWORD cchDst, DWORD ichAt, LPCWSTR pszSrc);

LWSTDAPI SHLoadIndirectString(LPCWSTR pszSource, LPWSTR pszOutBuf, UINT cchOutBuf, void **ppvReserved);

#if (_WIN32_IE >= 0x0603)
LWSTDAPI_(BOOL) IsCharSpaceA(CHAR wch);
LWSTDAPI_(BOOL) IsCharSpaceW(WCHAR wch);
#ifdef UNICODE
#define IsCharSpace  IsCharSpaceW
#else
#define IsCharSpace  IsCharSpaceA
#endif // !UNICODE

LWSTDAPI_(int)  StrCmpCA(LPCSTR pszStr1, LPCSTR pszStr2);
LWSTDAPI_(int)  StrCmpCW(LPCWSTR pszStr1, LPCWSTR pszStr2);
#ifdef UNICODE
#define StrCmpC  StrCmpCW
#else
#define StrCmpC  StrCmpCA
#endif // !UNICODE

LWSTDAPI_(int)  StrCmpICA(LPCSTR pszStr1, LPCSTR pszStr2);
LWSTDAPI_(int)  StrCmpICW(LPCWSTR pszStr1, LPCWSTR pszStr2);
#ifdef UNICODE
#define StrCmpIC  StrCmpICW
#else
#define StrCmpIC  StrCmpICA
#endif // !UNICODE
#endif // (_WIN32_IE >= 0x0603)

#ifdef UNICODE
#define StrChr                  StrChrW
#define StrRChr                 StrRChrW
#define StrChrI                 StrChrIW
#define StrRChrI                StrRChrIW
#define StrCmpN                 StrCmpNW
#define StrCmpNI                StrCmpNIW
#define StrStr                  StrStrW
#define StrStrI                 StrStrIW
#define StrDup                  StrDupW
#define StrRStrI                StrRStrIW
#define StrCSpn                 StrCSpnW
#define StrCSpnI                StrCSpnIW
#define StrSpn                  StrSpnW
#define StrToInt                StrToIntW
#define StrPBrk                 StrPBrkW
#define StrToIntEx              StrToIntExW
#if (_WIN32_IE >= 0x0600)
#define StrToInt64Ex            StrToInt64ExW
#endif
#define StrFromTimeInterval     StrFromTimeIntervalW
#define StrIntlEqN              StrIntlEqNW
#define StrIntlEqNI             StrIntlEqNIW
#define StrFormatByteSize       StrFormatByteSizeW
#define StrFormatByteSize64     StrFormatByteSizeW
#define StrFormatKBSize         StrFormatKBSizeW
#define StrNCat                 StrNCatW
#define StrTrim                 StrTrimW
#define StrCatBuff              StrCatBuffW
#define ChrCmpI                 ChrCmpIW
#define wvnsprintf              wvnsprintfW
#define wnsprintf               wnsprintfW
#define StrIsIntlEqual          StrIsIntlEqualW


#else
#define StrChr                  StrChrA
#define StrRChr                 StrRChrA
#define StrChrI                 StrChrIA
#define StrRChrI                StrRChrIA
#define StrCmpN                 StrCmpNA
#define StrCmpNI                StrCmpNIA
#define StrStr                  StrStrA
#define StrStrI                 StrStrIA
#define StrDup                  StrDupA
#define StrRStrI                StrRStrIA
#define StrCSpn                 StrCSpnA
#define StrCSpnI                StrCSpnIA
#define StrSpn                  StrSpnA
#define StrToInt                StrToIntA
#define StrPBrk                 StrPBrkA
#define StrToIntEx              StrToIntExA
#if (_WIN32_IE >= 0x0600)
#define StrToInt64Ex            StrToInt64ExA
#endif
#define StrFromTimeInterval     StrFromTimeIntervalA
#define StrIntlEqN              StrIntlEqNA
#define StrIntlEqNI             StrIntlEqNIA
#define StrFormatByteSize       StrFormatByteSizeA
#define StrFormatByteSize64     StrFormatByteSize64A
#define StrFormatKBSize         StrFormatKBSizeA
#define StrNCat                 StrNCatA
#define StrTrim                 StrTrimA
#define StrCatBuff              StrCatBuffA
#define ChrCmpI                 ChrCmpIA
#define wvnsprintf              wvnsprintfA
#define wnsprintf               wnsprintfA
#define StrIsIntlEqual          StrIsIntlEqualA
#endif


// Backward compatible to NT's non-standard naming (strictly
// for comctl32)
//
LWSTDAPI_(BOOL)     IntlStrEqWorkerA(BOOL fCaseSens, LPCSTR lpString1, LPCSTR lpString2, int nChar);
LWSTDAPI_(BOOL)     IntlStrEqWorkerW(BOOL fCaseSens, LPCWSTR lpString1, LPCWSTR lpString2, int nChar);

#define IntlStrEqNA( s1, s2, nChar) IntlStrEqWorkerA( TRUE, s1, s2, nChar)
#define IntlStrEqNW( s1, s2, nChar) IntlStrEqWorkerW( TRUE, s1, s2, nChar)
#define IntlStrEqNIA(s1, s2, nChar) IntlStrEqWorkerA(FALSE, s1, s2, nChar)
#define IntlStrEqNIW(s1, s2, nChar) IntlStrEqWorkerW(FALSE, s1, s2, nChar)

#ifdef UNICODE
#define IntlStrEqN              IntlStrEqNW
#define IntlStrEqNI             IntlStrEqNIW
#else
#define IntlStrEqN              IntlStrEqNA
#define IntlStrEqNI             IntlStrEqNIA
#endif

#define SZ_CONTENTTYPE_HTMLA       "text/html"
#define SZ_CONTENTTYPE_HTMLW       L"text/html"
#define SZ_CONTENTTYPE_CDFA        "application/x-cdf"
#define SZ_CONTENTTYPE_CDFW        L"application/x-cdf"

#ifdef UNICODE
#define SZ_CONTENTTYPE_HTML     SZ_CONTENTTYPE_HTMLW
#define SZ_CONTENTTYPE_CDF      SZ_CONTENTTYPE_CDFW
#else
#define SZ_CONTENTTYPE_HTML     SZ_CONTENTTYPE_HTMLA
#define SZ_CONTENTTYPE_CDF      SZ_CONTENTTYPE_CDFA
#endif

#define PathIsHTMLFileA(pszPath)     PathIsContentTypeA(pszPath, SZ_CONTENTTYPE_HTMLA)
#define PathIsHTMLFileW(pszPath)     PathIsContentTypeW(pszPath, SZ_CONTENTTYPE_HTMLW)

// Flags for StrToIntEx
#define STIF_DEFAULT        0x00000000L
#define STIF_SUPPORT_HEX    0x00000001L


#define StrCatA                 lstrcatA
#define StrCmpA                 lstrcmpA
#define StrCmpIA                lstrcmpiA
#define StrCpyA                 lstrcpyA
#define StrCpyNA                lstrcpynA


#define StrToLong               StrToInt
#define StrNCmp                 StrCmpN
#define StrNCmpI                StrCmpNI
#define StrNCpy                 StrCpyN
#define StrCatN                 StrNCat

#ifdef UNICODE
#define StrCat                  StrCatW
#define StrCmp                  StrCmpW
#define StrCmpI                 StrCmpIW
#define StrCpy                  StrCpyW
#define StrCpyN                 StrCpyNW
#define StrCatBuff              StrCatBuffW
#else
#define StrCat                  lstrcatA
#define StrCmp                  lstrcmpA
#define StrCmpI                 lstrcmpiA
#define StrCpy                  lstrcpyA
#define StrCpyN                 lstrcpynA
#define StrCatBuff              StrCatBuffA
#endif


#endif //  NO_SHLWAPI_STRFCNS


#ifndef NO_SHLWAPI_PATH

//
//=============== Path Routines ===================================
//

LWSTDAPI_(LPSTR)  PathAddBackslashA(LPSTR pszPath);
LWSTDAPI_(LPWSTR)  PathAddBackslashW(LPWSTR pszPath);
#ifdef UNICODE
#define PathAddBackslash  PathAddBackslashW
#else
#define PathAddBackslash  PathAddBackslashA
#endif // !UNICODE
LWSTDAPI_(BOOL)     PathAddExtensionA(LPSTR pszPath, LPCSTR pszExt);
LWSTDAPI_(BOOL)     PathAddExtensionW(LPWSTR pszPath, LPCWSTR pszExt);
#ifdef UNICODE
#define PathAddExtension  PathAddExtensionW
#else
#define PathAddExtension  PathAddExtensionA
#endif // !UNICODE
LWSTDAPI_(BOOL)     PathAppendA(LPSTR pszPath, LPCSTR pMore);
LWSTDAPI_(BOOL)     PathAppendW(LPWSTR pszPath, LPCWSTR pMore);
LWSTDAPI_(LPSTR)  PathBuildRootA(LPSTR pszRoot, int iDrive);
LWSTDAPI_(LPWSTR)  PathBuildRootW(LPWSTR pszRoot, int iDrive);
#ifdef UNICODE
#define PathBuildRoot  PathBuildRootW
#else
#define PathBuildRoot  PathBuildRootA
#endif // !UNICODE
LWSTDAPI_(BOOL)     PathCanonicalizeA(LPSTR pszBuf, LPCSTR pszPath);
LWSTDAPI_(BOOL)     PathCanonicalizeW(LPWSTR pszBuf, LPCWSTR pszPath);
LWSTDAPI_(LPSTR)  PathCombineA(LPSTR pszDest, LPCSTR pszDir, LPCSTR pszFile);
LWSTDAPI_(LPWSTR)  PathCombineW(LPWSTR pszDest, LPCWSTR pszDir, LPCWSTR pszFile);
#ifdef UNICODE
#define PathCombine  PathCombineW
#else
#define PathCombine  PathCombineA
#endif // !UNICODE
LWSTDAPI_(BOOL)     PathCompactPathA(HDC hDC, LPSTR pszPath, UINT dx);
LWSTDAPI_(BOOL)     PathCompactPathW(HDC hDC, LPWSTR pszPath, UINT dx);
LWSTDAPI_(BOOL)     PathCompactPathExA(LPSTR pszOut, LPCSTR pszSrc, UINT cchMax, DWORD dwFlags);
LWSTDAPI_(BOOL)     PathCompactPathExW(LPWSTR pszOut, LPCWSTR pszSrc, UINT cchMax, DWORD dwFlags);
LWSTDAPI_(int)      PathCommonPrefixA(LPCSTR pszFile1, LPCSTR pszFile2, LPSTR achPath);
LWSTDAPI_(int)      PathCommonPrefixW(LPCWSTR pszFile1, LPCWSTR pszFile2, LPWSTR achPath);
LWSTDAPI_(BOOL)     PathFileExistsA(LPCSTR pszPath);
LWSTDAPI_(BOOL)     PathFileExistsW(LPCWSTR pszPath);
#ifdef UNICODE
#define PathFileExists  PathFileExistsW
#else
#define PathFileExists  PathFileExistsA
#endif // !UNICODE
LWSTDAPI_(LPSTR)  PathFindExtensionA(LPCSTR pszPath);
LWSTDAPI_(LPWSTR)  PathFindExtensionW(LPCWSTR pszPath);
#ifdef UNICODE
#define PathFindExtension  PathFindExtensionW
#else
#define PathFindExtension  PathFindExtensionA
#endif // !UNICODE
LWSTDAPI_(LPSTR)  PathFindFileNameA(LPCSTR pszPath);
LWSTDAPI_(LPWSTR)  PathFindFileNameW(LPCWSTR pszPath);
#ifdef UNICODE
#define PathFindFileName  PathFindFileNameW
#else
#define PathFindFileName  PathFindFileNameA
#endif // !UNICODE
LWSTDAPI_(LPSTR)  PathFindNextComponentA(LPCSTR pszPath);
LWSTDAPI_(LPWSTR)  PathFindNextComponentW(LPCWSTR pszPath);
#ifdef UNICODE
#define PathFindNextComponent  PathFindNextComponentW
#else
#define PathFindNextComponent  PathFindNextComponentA
#endif // !UNICODE
LWSTDAPI_(BOOL)     PathFindOnPathA(LPSTR pszPath, LPCSTR * ppszOtherDirs);
LWSTDAPI_(BOOL)     PathFindOnPathW(LPWSTR pszPath, LPCWSTR * ppszOtherDirs);
LWSTDAPI_(LPSTR)  PathGetArgsA(LPCSTR pszPath);
LWSTDAPI_(LPWSTR)  PathGetArgsW(LPCWSTR pszPath);
#ifdef UNICODE
#define PathGetArgs  PathGetArgsW
#else
#define PathGetArgs  PathGetArgsA
#endif // !UNICODE
LWSTDAPI_(LPCSTR) PathFindSuffixArrayA(LPCSTR pszPath, const LPCSTR *apszSuffix, int iArraySize);
LWSTDAPI_(LPCWSTR) PathFindSuffixArrayW(LPCWSTR pszPath, const LPCWSTR *apszSuffix, int iArraySize);
#ifdef UNICODE
#define PathFindSuffixArray  PathFindSuffixArrayW
#else
#define PathFindSuffixArray  PathFindSuffixArrayA
#endif // !UNICODE
LWSTDAPI_(BOOL)     PathIsLFNFileSpecA(LPCSTR lpName);
LWSTDAPI_(BOOL)     PathIsLFNFileSpecW(LPCWSTR lpName);
#ifdef UNICODE
#define PathIsLFNFileSpec  PathIsLFNFileSpecW
#else
#define PathIsLFNFileSpec  PathIsLFNFileSpecA
#endif // !UNICODE

LWSTDAPI_(UINT)     PathGetCharTypeA(UCHAR ch);
LWSTDAPI_(UINT)     PathGetCharTypeW(WCHAR ch);

// Return flags for PathGetCharType
#define GCT_INVALID             0x0000
#define GCT_LFNCHAR             0x0001
#define GCT_SHORTCHAR           0x0002
#define GCT_WILD                0x0004
#define GCT_SEPARATOR           0x0008

LWSTDAPI_(int)      PathGetDriveNumberA(LPCSTR pszPath);
LWSTDAPI_(int)      PathGetDriveNumberW(LPCWSTR pszPath);
#ifdef UNICODE
#define PathGetDriveNumber  PathGetDriveNumberW
#else
#define PathGetDriveNumber  PathGetDriveNumberA
#endif // !UNICODE
LWSTDAPI_(BOOL)     PathIsDirectoryA(LPCSTR pszPath);
LWSTDAPI_(BOOL)     PathIsDirectoryW(LPCWSTR pszPath);
#ifdef UNICODE
#define PathIsDirectory  PathIsDirectoryW
#else
#define PathIsDirectory  PathIsDirectoryA
#endif // !UNICODE
LWSTDAPI_(BOOL)     PathIsDirectoryEmptyA(LPCSTR pszPath);
LWSTDAPI_(BOOL)     PathIsDirectoryEmptyW(LPCWSTR pszPath);
#ifdef UNICODE
#define PathIsDirectoryEmpty  PathIsDirectoryEmptyW
#else
#define PathIsDirectoryEmpty  PathIsDirectoryEmptyA
#endif // !UNICODE
LWSTDAPI_(BOOL)     PathIsFileSpecA(LPCSTR pszPath);
LWSTDAPI_(BOOL)     PathIsFileSpecW(LPCWSTR pszPath);
#ifdef UNICODE
#define PathIsFileSpec  PathIsFileSpecW
#else
#define PathIsFileSpec  PathIsFileSpecA
#endif // !UNICODE
LWSTDAPI_(BOOL)     PathIsPrefixA(LPCSTR pszPrefix, LPCSTR pszPath);
LWSTDAPI_(BOOL)     PathIsPrefixW(LPCWSTR pszPrefix, LPCWSTR pszPath);
#ifdef UNICODE
#define PathIsPrefix  PathIsPrefixW
#else
#define PathIsPrefix  PathIsPrefixA
#endif // !UNICODE
LWSTDAPI_(BOOL)     PathIsRelativeA(LPCSTR pszPath);
LWSTDAPI_(BOOL)     PathIsRelativeW(LPCWSTR pszPath);
#ifdef UNICODE
#define PathIsRelative  PathIsRelativeW
#else
#define PathIsRelative  PathIsRelativeA
#endif // !UNICODE
LWSTDAPI_(BOOL)     PathIsRootA(LPCSTR pszPath);
LWSTDAPI_(BOOL)     PathIsRootW(LPCWSTR pszPath);
#ifdef UNICODE
#define PathIsRoot  PathIsRootW
#else
#define PathIsRoot  PathIsRootA
#endif // !UNICODE
LWSTDAPI_(BOOL)     PathIsSameRootA(LPCSTR pszPath1, LPCSTR pszPath2);
LWSTDAPI_(BOOL)     PathIsSameRootW(LPCWSTR pszPath1, LPCWSTR pszPath2);
#ifdef UNICODE
#define PathIsSameRoot  PathIsSameRootW
#else
#define PathIsSameRoot  PathIsSameRootA
#endif // !UNICODE
LWSTDAPI_(BOOL)     PathIsUNCA(LPCSTR pszPath);
LWSTDAPI_(BOOL)     PathIsUNCW(LPCWSTR pszPath);
#ifdef UNICODE
#define PathIsUNC  PathIsUNCW
#else
#define PathIsUNC  PathIsUNCA
#endif // !UNICODE
LWSTDAPI_(BOOL)     PathIsNetworkPathA(LPCSTR pszPath);
LWSTDAPI_(BOOL)     PathIsNetworkPathW(LPCWSTR pszPath);
#ifdef UNICODE
#define PathIsNetworkPath  PathIsNetworkPathW
#else
#define PathIsNetworkPath  PathIsNetworkPathA
#endif // !UNICODE
LWSTDAPI_(BOOL)     PathIsUNCServerA(LPCSTR pszPath);
LWSTDAPI_(BOOL)     PathIsUNCServerW(LPCWSTR pszPath);
#ifdef UNICODE
#define PathIsUNCServer  PathIsUNCServerW
#else
#define PathIsUNCServer  PathIsUNCServerA
#endif // !UNICODE
LWSTDAPI_(BOOL)     PathIsUNCServerShareA(LPCSTR pszPath);
LWSTDAPI_(BOOL)     PathIsUNCServerShareW(LPCWSTR pszPath);
#ifdef UNICODE
#define PathIsUNCServerShare  PathIsUNCServerShareW
#else
#define PathIsUNCServerShare  PathIsUNCServerShareA
#endif // !UNICODE
LWSTDAPI_(BOOL)     PathIsContentTypeA(LPCSTR pszPath, LPCSTR pszContentType);
LWSTDAPI_(BOOL)     PathIsContentTypeW(LPCWSTR pszPath, LPCWSTR pszContentType);
LWSTDAPI_(BOOL)     PathIsURLA(LPCSTR pszPath);
LWSTDAPI_(BOOL)     PathIsURLW(LPCWSTR pszPath);
#ifdef UNICODE
#define PathIsURL  PathIsURLW
#else
#define PathIsURL  PathIsURLA
#endif // !UNICODE
LWSTDAPI_(BOOL)     PathMakePrettyA(LPSTR pszPath);
LWSTDAPI_(BOOL)     PathMakePrettyW(LPWSTR pszPath);
LWSTDAPI_(BOOL)     PathMatchSpecA(LPCSTR pszFile, LPCSTR pszSpec);
LWSTDAPI_(BOOL)     PathMatchSpecW(LPCWSTR pszFile, LPCWSTR pszSpec);
LWSTDAPI_(int)      PathParseIconLocationA(LPSTR pszIconFile);
LWSTDAPI_(int)      PathParseIconLocationW(LPWSTR pszIconFile);
LWSTDAPI_(void)     PathQuoteSpacesA(LPSTR lpsz);
LWSTDAPI_(void)     PathQuoteSpacesW(LPWSTR lpsz);
LWSTDAPI_(BOOL)     PathRelativePathToA(LPSTR pszPath, LPCSTR pszFrom, DWORD dwAttrFrom, LPCSTR pszTo, DWORD dwAttrTo);
LWSTDAPI_(BOOL)     PathRelativePathToW(LPWSTR pszPath, LPCWSTR pszFrom, DWORD dwAttrFrom, LPCWSTR pszTo, DWORD dwAttrTo);
LWSTDAPI_(void)     PathRemoveArgsA(LPSTR pszPath);
LWSTDAPI_(void)     PathRemoveArgsW(LPWSTR pszPath);
LWSTDAPI_(LPSTR)  PathRemoveBackslashA(LPSTR pszPath);
LWSTDAPI_(LPWSTR)  PathRemoveBackslashW(LPWSTR pszPath);
#ifdef UNICODE
#define PathRemoveBackslash  PathRemoveBackslashW
#else
#define PathRemoveBackslash  PathRemoveBackslashA
#endif // !UNICODE
LWSTDAPI_(void)     PathRemoveBlanksA(LPSTR pszPath);
LWSTDAPI_(void)     PathRemoveBlanksW(LPWSTR pszPath);
LWSTDAPI_(void)     PathRemoveExtensionA(LPSTR pszPath);
LWSTDAPI_(void)     PathRemoveExtensionW(LPWSTR pszPath);
LWSTDAPI_(BOOL)     PathRemoveFileSpecA(LPSTR pszPath);
LWSTDAPI_(BOOL)     PathRemoveFileSpecW(LPWSTR pszPath);
LWSTDAPI_(BOOL)     PathRenameExtensionA(LPSTR pszPath, LPCSTR pszExt);
LWSTDAPI_(BOOL)     PathRenameExtensionW(LPWSTR pszPath, LPCWSTR pszExt);
LWSTDAPI_(BOOL)     PathSearchAndQualifyA(LPCSTR pszPath, LPSTR pszBuf, UINT cchBuf);
LWSTDAPI_(BOOL)     PathSearchAndQualifyW(LPCWSTR pszPath, LPWSTR pszBuf, UINT cchBuf);
LWSTDAPI_(void)     PathSetDlgItemPathA(HWND hDlg, int id, LPCSTR pszPath);
LWSTDAPI_(void)     PathSetDlgItemPathW(HWND hDlg, int id, LPCWSTR pszPath);
LWSTDAPI_(LPSTR)  PathSkipRootA(LPCSTR pszPath);
LWSTDAPI_(LPWSTR)  PathSkipRootW(LPCWSTR pszPath);
#ifdef UNICODE
#define PathSkipRoot  PathSkipRootW
#else
#define PathSkipRoot  PathSkipRootA
#endif // !UNICODE
LWSTDAPI_(void)     PathStripPathA(LPSTR pszPath);
LWSTDAPI_(void)     PathStripPathW(LPWSTR pszPath);
#ifdef UNICODE
#define PathStripPath  PathStripPathW
#else
#define PathStripPath  PathStripPathA
#endif // !UNICODE
LWSTDAPI_(BOOL)     PathStripToRootA(LPSTR pszPath);
LWSTDAPI_(BOOL)     PathStripToRootW(LPWSTR pszPath);
#ifdef UNICODE
#define PathStripToRoot  PathStripToRootW
#else
#define PathStripToRoot  PathStripToRootA
#endif // !UNICODE
LWSTDAPI_(void)     PathUnquoteSpacesA(LPSTR lpsz);
LWSTDAPI_(void)     PathUnquoteSpacesW(LPWSTR lpsz);
LWSTDAPI_(BOOL)     PathMakeSystemFolderA(LPCSTR pszPath);
LWSTDAPI_(BOOL)     PathMakeSystemFolderW(LPCWSTR pszPath);
#ifdef UNICODE
#define PathMakeSystemFolder  PathMakeSystemFolderW
#else
#define PathMakeSystemFolder  PathMakeSystemFolderA
#endif // !UNICODE
LWSTDAPI_(BOOL)     PathUnmakeSystemFolderA(LPCSTR pszPath);
LWSTDAPI_(BOOL)     PathUnmakeSystemFolderW(LPCWSTR pszPath);
#ifdef UNICODE
#define PathUnmakeSystemFolder  PathUnmakeSystemFolderW
#else
#define PathUnmakeSystemFolder  PathUnmakeSystemFolderA
#endif // !UNICODE
LWSTDAPI_(BOOL)     PathIsSystemFolderA(LPCSTR pszPath, DWORD dwAttrb);
LWSTDAPI_(BOOL)     PathIsSystemFolderW(LPCWSTR pszPath, DWORD dwAttrb);
#ifdef UNICODE
#define PathIsSystemFolder  PathIsSystemFolderW
#else
#define PathIsSystemFolder  PathIsSystemFolderA
#endif // !UNICODE
LWSTDAPI_(void)     PathUndecorateA(LPSTR pszPath);
LWSTDAPI_(void)     PathUndecorateW(LPWSTR pszPath);
#ifdef UNICODE
#define PathUndecorate  PathUndecorateW
#else
#define PathUndecorate  PathUndecorateA
#endif // !UNICODE
LWSTDAPI_(BOOL)     PathUnExpandEnvStringsA(LPCSTR pszPath, LPSTR pszBuf, UINT cchBuf);
LWSTDAPI_(BOOL)     PathUnExpandEnvStringsW(LPCWSTR pszPath, LPWSTR pszBuf, UINT cchBuf);
#ifdef UNICODE
#define PathUnExpandEnvStrings  PathUnExpandEnvStringsW
#else
#define PathUnExpandEnvStrings  PathUnExpandEnvStringsA
#endif // !UNICODE



#ifdef UNICODE
#define PathAppend              PathAppendW
#define PathCanonicalize        PathCanonicalizeW
#define PathCompactPath         PathCompactPathW
#define PathCompactPathEx       PathCompactPathExW
#define PathCommonPrefix        PathCommonPrefixW
#define PathFindOnPath          PathFindOnPathW
#define PathGetCharType         PathGetCharTypeW
#define PathIsContentType       PathIsContentTypeW
#define PathIsHTMLFile          PathIsHTMLFileW
#define PathMakePretty          PathMakePrettyW
#define PathMatchSpec           PathMatchSpecW
#define PathParseIconLocation   PathParseIconLocationW
#define PathQuoteSpaces         PathQuoteSpacesW
#define PathRelativePathTo      PathRelativePathToW
#define PathRemoveArgs          PathRemoveArgsW
#define PathRemoveBlanks        PathRemoveBlanksW
#define PathRemoveExtension     PathRemoveExtensionW
#define PathRemoveFileSpec      PathRemoveFileSpecW
#define PathRenameExtension     PathRenameExtensionW
#define PathSearchAndQualify    PathSearchAndQualifyW
#define PathSetDlgItemPath      PathSetDlgItemPathW
#define PathUnquoteSpaces       PathUnquoteSpacesW
#else
#define PathAppend              PathAppendA
#define PathCanonicalize        PathCanonicalizeA
#define PathCompactPath         PathCompactPathA
#define PathCompactPathEx       PathCompactPathExA
#define PathCommonPrefix        PathCommonPrefixA
#define PathFindOnPath          PathFindOnPathA
#define PathGetCharType         PathGetCharTypeA
#define PathIsContentType       PathIsContentTypeA
#define PathIsHTMLFile          PathIsHTMLFileA
#define PathMakePretty          PathMakePrettyA
#define PathMatchSpec           PathMatchSpecA
#define PathParseIconLocation   PathParseIconLocationA
#define PathQuoteSpaces         PathQuoteSpacesA
#define PathRelativePathTo      PathRelativePathToA
#define PathRemoveArgs          PathRemoveArgsA
#define PathRemoveBlanks        PathRemoveBlanksA
#define PathRemoveExtension     PathRemoveExtensionA
#define PathRemoveFileSpec      PathRemoveFileSpecA
#define PathRenameExtension     PathRenameExtensionA
#define PathSearchAndQualify    PathSearchAndQualifyA
#define PathSetDlgItemPath      PathSetDlgItemPathA
#define PathUnquoteSpaces       PathUnquoteSpacesA
#endif

typedef enum {
    URL_SCHEME_INVALID     = -1,
    URL_SCHEME_UNKNOWN     =  0,
    URL_SCHEME_FTP,
    URL_SCHEME_HTTP,
    URL_SCHEME_GOPHER,
    URL_SCHEME_MAILTO,
    URL_SCHEME_NEWS,
    URL_SCHEME_NNTP,
    URL_SCHEME_TELNET,
    URL_SCHEME_WAIS,
    URL_SCHEME_FILE,
    URL_SCHEME_MK,
    URL_SCHEME_HTTPS,
    URL_SCHEME_SHELL,
    URL_SCHEME_SNEWS,
    URL_SCHEME_LOCAL,
    URL_SCHEME_JAVASCRIPT,
    URL_SCHEME_VBSCRIPT,
    URL_SCHEME_ABOUT,
    URL_SCHEME_RES,
    URL_SCHEME_MSSHELLROOTED,
    URL_SCHEME_MSSHELLIDLIST,
    URL_SCHEME_MSHELP,
    URL_SCHEME_MAXVALUE
} URL_SCHEME;

typedef enum {
    URL_PART_NONE       = 0,
    URL_PART_SCHEME     = 1,
    URL_PART_HOSTNAME,
    URL_PART_USERNAME,
    URL_PART_PASSWORD,
    URL_PART_PORT,
    URL_PART_QUERY,
} URL_PART;

typedef enum {
    URLIS_URL,
    URLIS_OPAQUE,
    URLIS_NOHISTORY,
    URLIS_FILEURL,
    URLIS_APPLIABLE,
    URLIS_DIRECTORY,
    URLIS_HASQUERY,
} URLIS;

#define URL_UNESCAPE                    0x10000000
#define URL_ESCAPE_UNSAFE               0x20000000
#define URL_PLUGGABLE_PROTOCOL          0x40000000
#define URL_WININET_COMPATIBILITY       0x80000000
#define URL_DONT_ESCAPE_EXTRA_INFO      0x02000000
#define URL_DONT_UNESCAPE_EXTRA_INFO    URL_DONT_ESCAPE_EXTRA_INFO
#define URL_BROWSER_MODE                URL_DONT_ESCAPE_EXTRA_INFO
#define URL_ESCAPE_SPACES_ONLY          0x04000000
#define URL_DONT_SIMPLIFY               0x08000000
#define URL_NO_META                     URL_DONT_SIMPLIFY
#define URL_UNESCAPE_INPLACE            0x00100000
#define URL_CONVERT_IF_DOSPATH          0x00200000
#define URL_UNESCAPE_HIGH_ANSI_ONLY     0x00400000
#define URL_INTERNAL_PATH               0x00800000  // Will escape #'s in paths
#define URL_FILE_USE_PATHURL            0x00010000
#define URL_DONT_UNESCAPE               0x00020000  // Do not unescape the path/url at all
#define URL_ESCAPE_PERCENT              0x00001000
#define URL_ESCAPE_SEGMENT_ONLY         0x00002000  // Treat the entire URL param as one URL segment.

#define URL_PARTFLAG_KEEPSCHEME         0x00000001

#define URL_APPLY_DEFAULT               0x00000001
#define URL_APPLY_GUESSSCHEME           0x00000002
#define URL_APPLY_GUESSFILE             0x00000004
#define URL_APPLY_FORCEAPPLY            0x00000008


LWSTDAPI_(int)          UrlCompareA(LPCSTR psz1, LPCSTR psz2, BOOL fIgnoreSlash);
LWSTDAPI_(int)          UrlCompareW(LPCWSTR psz1, LPCWSTR psz2, BOOL fIgnoreSlash);
LWSTDAPI                UrlCombineA(LPCSTR pszBase, LPCSTR pszRelative, LPSTR pszCombined, LPDWORD pcchCombined, DWORD dwFlags);
LWSTDAPI                UrlCombineW(LPCWSTR pszBase, LPCWSTR pszRelative, LPWSTR pszCombined, LPDWORD pcchCombined, DWORD dwFlags);
LWSTDAPI                UrlCanonicalizeA(LPCSTR pszUrl, LPSTR pszCanonicalized, LPDWORD pcchCanonicalized, DWORD dwFlags);
LWSTDAPI                UrlCanonicalizeW(LPCWSTR pszUrl, LPWSTR pszCanonicalized, LPDWORD pcchCanonicalized, DWORD dwFlags);
LWSTDAPI_(BOOL)         UrlIsOpaqueA(LPCSTR pszURL);
LWSTDAPI_(BOOL)         UrlIsOpaqueW(LPCWSTR pszURL);
LWSTDAPI_(BOOL)         UrlIsNoHistoryA(LPCSTR pszURL);
LWSTDAPI_(BOOL)         UrlIsNoHistoryW(LPCWSTR pszURL);
#define                 UrlIsFileUrlA(pszURL) UrlIsA(pszURL, URLIS_FILEURL)
#define                 UrlIsFileUrlW(pszURL) UrlIsW(pszURL, URLIS_FILEURL)
LWSTDAPI_(BOOL)         UrlIsA(LPCSTR pszUrl, URLIS UrlIs);
LWSTDAPI_(BOOL)         UrlIsW(LPCWSTR pszUrl, URLIS UrlIs);
LWSTDAPI_(LPCSTR)       UrlGetLocationA(LPCSTR psz1);
LWSTDAPI_(LPCWSTR)      UrlGetLocationW(LPCWSTR psz1);
LWSTDAPI                UrlUnescapeA(LPSTR pszUrl, LPSTR pszUnescaped, LPDWORD pcchUnescaped, DWORD dwFlags);
LWSTDAPI                UrlUnescapeW(LPWSTR pszUrl, LPWSTR pszUnescaped, LPDWORD pcchUnescaped, DWORD dwFlags);
LWSTDAPI                UrlEscapeA(LPCSTR pszUrl, LPSTR pszEscaped, LPDWORD pcchEscaped, DWORD dwFlags);
LWSTDAPI                UrlEscapeW(LPCWSTR pszUrl, LPWSTR pszEscaped, LPDWORD pcchEscaped, DWORD dwFlags);
LWSTDAPI                UrlCreateFromPathA(LPCSTR pszPath, LPSTR pszUrl, LPDWORD pcchUrl, DWORD dwFlags);
LWSTDAPI                UrlCreateFromPathW(LPCWSTR pszPath, LPWSTR pszUrl, LPDWORD pcchUrl, DWORD dwFlags);
LWSTDAPI                PathCreateFromUrlA(LPCSTR pszUrl, LPSTR pszPath, LPDWORD pcchPath, DWORD dwFlags);
LWSTDAPI                PathCreateFromUrlW(LPCWSTR pszUrl, LPWSTR pszPath, LPDWORD pcchPath, DWORD dwFlags);
LWSTDAPI                UrlHashA(LPCSTR pszUrl, LPBYTE pbHash, DWORD cbHash);
LWSTDAPI                UrlHashW(LPCWSTR pszUrl, LPBYTE pbHash, DWORD cbHash);
LWSTDAPI                UrlGetPartW(LPCWSTR pszIn, LPWSTR pszOut, LPDWORD pcchOut, DWORD dwPart, DWORD dwFlags);
LWSTDAPI                UrlGetPartA(LPCSTR pszIn, LPSTR pszOut, LPDWORD pcchOut, DWORD dwPart, DWORD dwFlags);
LWSTDAPI                UrlApplySchemeA(LPCSTR pszIn, LPSTR pszOut, LPDWORD pcchOut, DWORD dwFlags);
LWSTDAPI                UrlApplySchemeW(LPCWSTR pszIn, LPWSTR pszOut, LPDWORD pcchOut, DWORD dwFlags);
LWSTDAPI                HashData(LPBYTE pbData, DWORD cbData, LPBYTE pbHash, DWORD cbHash);



#ifdef UNICODE
#define UrlCompare              UrlCompareW
#define UrlCombine              UrlCombineW
#define UrlCanonicalize         UrlCanonicalizeW
#define UrlIsOpaque             UrlIsOpaqueW
#define UrlIsFileUrl            UrlIsFileUrlW
#define UrlGetLocation          UrlGetLocationW
#define UrlUnescape             UrlUnescapeW
#define UrlEscape               UrlEscapeW
#define UrlCreateFromPath       UrlCreateFromPathW
#define PathCreateFromUrl       PathCreateFromUrlW
#define UrlHash                 UrlHashW
#define UrlGetPart              UrlGetPartW
#define UrlApplyScheme          UrlApplySchemeW
#define UrlIs                   UrlIsW


#else //!UNICODE
#define UrlCompare              UrlCompareA
#define UrlCombine              UrlCombineA
#define UrlCanonicalize         UrlCanonicalizeA
#define UrlIsOpaque             UrlIsOpaqueA
#define UrlIsFileUrl            UrlIsFileUrlA
#define UrlGetLocation          UrlGetLocationA
#define UrlUnescape             UrlUnescapeA
#define UrlEscape               UrlEscapeA
#define UrlCreateFromPath       UrlCreateFromPathA
#define PathCreateFromUrl       PathCreateFromUrlA
#define UrlHash                 UrlHashA
#define UrlGetPart              UrlGetPartA
#define UrlApplyScheme          UrlApplySchemeA
#define UrlIs                   UrlIsA


#endif //UNICODE

#define UrlEscapeSpaces(pszUrl, pszEscaped, pcchEscaped)        UrlCanonicalize(pszUrl, pszEscaped, pcchEscaped, URL_ESCAPE_SPACES_ONLY |URL_DONT_ESCAPE_EXTRA_INFO )
#define UrlUnescapeInPlace(pszUrl, dwFlags)                     UrlUnescape(pszUrl, NULL, NULL, dwFlags | URL_UNESCAPE_INPLACE)



#endif //  NO_SHLWAPI_PATH



#ifndef NO_SHLWAPI_REG
//
//=============== Registry Routines ===================================
//


// SHDeleteEmptyKey mimics RegDeleteKey as it behaves on NT.
// SHDeleteKey mimics RegDeleteKey as it behaves on Win95.

LWSTDAPI_(DWORD)    SHDeleteEmptyKeyA(HKEY hkey, LPCSTR pszSubKey);
LWSTDAPI_(DWORD)    SHDeleteEmptyKeyW(HKEY hkey, LPCWSTR pszSubKey);
#ifdef UNICODE
#define SHDeleteEmptyKey  SHDeleteEmptyKeyW
#else
#define SHDeleteEmptyKey  SHDeleteEmptyKeyA
#endif // !UNICODE
LWSTDAPI_(DWORD)    SHDeleteKeyA(HKEY hkey, LPCSTR pszSubKey);
LWSTDAPI_(DWORD)    SHDeleteKeyW(HKEY hkey, LPCWSTR pszSubKey);
#ifdef UNICODE
#define SHDeleteKey  SHDeleteKeyW
#else
#define SHDeleteKey  SHDeleteKeyA
#endif // !UNICODE
LWSTDAPI_(HKEY)     SHRegDuplicateHKey(HKEY hkey);


// These functions open the key, get/set/delete the value, then close
// the key.

LWSTDAPI_(DWORD)    SHDeleteValueA(HKEY hkey, LPCSTR pszSubKey, LPCSTR pszValue);
LWSTDAPI_(DWORD)    SHDeleteValueW(HKEY hkey, LPCWSTR pszSubKey, LPCWSTR pszValue);
#ifdef UNICODE
#define SHDeleteValue  SHDeleteValueW
#else
#define SHDeleteValue  SHDeleteValueA
#endif // !UNICODE
LWSTDAPI_(DWORD)    SHGetValueA(HKEY hkey, LPCSTR pszSubKey, LPCSTR pszValue, DWORD *pdwType, void *pvData, DWORD *pcbData);
LWSTDAPI_(DWORD)    SHGetValueW(HKEY hkey, LPCWSTR pszSubKey, LPCWSTR pszValue, DWORD *pdwType, void *pvData, DWORD *pcbData);
#ifdef UNICODE
#define SHGetValue  SHGetValueW
#else
#define SHGetValue  SHGetValueA
#endif // !UNICODE
LWSTDAPI_(DWORD)    SHSetValueA(HKEY hkey, LPCSTR pszSubKey, LPCSTR pszValue, DWORD dwType, LPCVOID pvData, DWORD cbData);
LWSTDAPI_(DWORD)    SHSetValueW(HKEY hkey, LPCWSTR pszSubKey, LPCWSTR pszValue, DWORD dwType, LPCVOID pvData, DWORD cbData);
#ifdef UNICODE
#define SHSetValue  SHSetValueW
#else
#define SHSetValue  SHSetValueA
#endif // !UNICODE


#if (_WIN32_IE >= 0x0602)
//
// SRRF - Shell Registry Routine Flags (for SHRegGetValue)
//

typedef DWORD SRRF;

#define SRRF_RT_REG_NONE        0x00000001  // restrict type to REG_NONE      (other data types will not return ERROR_SUCCESS)
#define SRRF_RT_REG_SZ          0x00000002  // restrict type to REG_SZ        (other data types will not return ERROR_SUCCESS)
#define SRRF_RT_REG_EXPAND_SZ   0x00000004  // restrict type to REG_EXPAND_SZ (other data types will not return ERROR_SUCCESS)
#define SRRF_RT_REG_BINARY      0x00000008  // restrict type to REG_BINARY    (other data types will not return ERROR_SUCCESS)
#define SRRF_RT_REG_DWORD       0x00000010  // restrict type to REG_DWORD     (other data types will not return ERROR_SUCCESS)
#define SRRF_RT_REG_MULTI_SZ    0x00000020  // restrict type to REG_MULTI_SZ  (other data types will not return ERROR_SUCCESS)
#define SRRF_RT_REG_QWORD       0x00000040  // restrict type to REG_QWORD     (other data types will not return ERROR_SUCCESS)

#define SRRF_RT_DWORD           (SRRF_RT_REG_BINARY | SRRF_RT_REG_DWORD) // restrict type to *32-bit* SRRF_RT_REG_BINARY or SRRF_RT_REG_DWORD (other data types will not return ERROR_SUCCESS)
#define SRRF_RT_QWORD           (SRRF_RT_REG_BINARY | SRRF_RT_REG_QWORD) // restrict type to *64-bit* SRRF_RT_REG_BINARY or SRRF_RT_REG_DWORD (other data types will not return ERROR_SUCCESS)
#define SRRF_RT_ANY             0x0000ffff                               // no type restriction

#define SRRF_RM_ANY             0x00000000  // no mode restriction (default is to allow any mode)
#define SRRF_RM_NORMAL          0x00010000  // restrict system startup mode to "normal boot"               (other startup modes will not return ERROR_SUCCESS)
#define SRRF_RM_SAFE            0x00020000  // restrict system startup mode to "safe mode"                 (other startup modes will not return ERROR_SUCCESS)
#define SRRF_RM_SAFENETWORK     0x00040000  // restrict system startup mode to "safe mode with networking" (other startup modes will not return ERROR_SUCCESS)

#define SRRF_NOEXPAND           0x10000000  // do not automatically expand environment strings if value is of type REG_EXPAND_SZ
#define SRRF_ZEROONFAILURE      0x20000000  // if pvData is not NULL, set content to all zeros on failure

// Function:
//
//  SHRegGetValue()
//
// Purpose:
//
//  Gets a registry value.  SHRegGetValue() provides the following benefits:
//
//  - data type checking
//  - boot mode checking
//  - auto-expansion of REG_EXPAND_SZ data
//  - guaranteed NULL termination of REG_SZ, REG_EXPAND_SZ, REG_MULTI_SZ data
//
// Parameters:
//
//  hkey        - handle to a currently open key.
//
//  pszSubKey   - pointer to a null-terminated string specifying the relative
//                path from hkey to one of its subkeys from which the data is
//                to be retrieved.  this will be opened with KEY_READ sam.
//
//                Note1: pszSubKey can be NULL or "".  In either of these two
//                       cases, the data is retrieved from the hkey itself.
//                Note2: *** PERF ***
//                       If pszSubKey is not NULL or "", the subkey will be
//                       automatically be opened and closed by this routine
//                       in order to obtain the data.  If you are retrieving
//                       multiple values from the same subkey, it is better
//                       for perf to open the subkey via RegOpenKeyEx() prior
//                       to calling this method, and using this opened key as
//                       hkey with pszSubKey set to NULL.
//
//  pszValue    - pointer to a null-terminated string specifying the name of
//                the value to query for data
//
//                Note1: pszValue can be NULL or "".  In either of these two
//                       cases, the data is retrieved from the unnamed or
//                       default value.
//
//  dwFlags     - bitwise or of SRRF_ flags, which cannot be 0:  at least one
//                type restriction must be specified (SRRF_RT_...), or if any
//                type is desired then SRRF_RT_ANY can be specified
//
//                Note1: SRRF_RT_ANY will allow any data type to be returned.
//                Note2: The following two type restrictions have special
//                       handling semantics:
//
//                         SRRF_RT_DWORD == SRRF_RT_REG_BINARY | SRRF_RT_REG_DWORD
//                         SRRF_RT_QWORD == SRRF_RT_REG_BINARY | SRRF_RT_REG_QWORD
//
//                       If either of these are specified, with no other type
//                       restrictions, then in the prior case the restriction
//                       will limit "valid" returned data to either REG_DWORD
//                       or 32-bit REG_BINARY data, and in the latter case
//                       the restriction will limit "valid" returned data to
//                       either REG_QWORD or 64-bit REG_BINARY.
//
//  pdwType     - pointer to a dword which receives a code indicating the
//                type of data stored in the specified value
//
//                Note1: pdwType can be NULL if no type information is wanted
//                Note2: If pdwType is not NULL, and the SRRF_NOEXPAND flag
//                       has not been set, data types of REG_EXPAND_SZ will
//                       be returned as REG_SZ since they are automatically
//                       expanded in this method.
//
//  pvData      - pointer to a buffer that receives the value's data
//
//                Note1: pvData can be NULL if the data is not required.
//                       pvData is usually NULL if doing either a simple
//                       existence test, or if interested in the size only.
//                Note2: *** PERF ***
//                       Reference 'perf' note for pcbData.
//
//  pcbData     - when pvData is NULL:
//                  optional pointer to a dword that receives a size in bytes
//                  which would be sufficient to hold the registry data (note
//                  this size is not guaranteed to be exact, merely sufficient)
//                when pvData is not NULL:
//                  required pointer to a dword that specifies the size in
//                  bytes of the buffer pointed to by the pvData parameter
//                  and receives a size in bytes of:
//                  a) the number of bytes read into pvData on ERROR_SUCCESS
//                     (note this size is guaranteed to be exact)
//                  b) the number of bytes which would be sufficient to hold
//                     the registry data on ERROR_MORE_DATA -- pvData was of
//                     insufficient size (note this size is not guaranteed to
//                     be exact, merely sufficient)
//
//                Note1: pcbData can be NULL only if pvData is NULL.
//                Note2: *** PERF ***
//                       The potential for an 'extra' call to the registry to
//                       read (or re-read) in the data exists when the data
//                       type is REG_EXPAND_SZ and the SRRF_NOEXPAND flag has
//                       not been set.  The following conditions will result
//                       in this 'extra' read operation:
//                       i)  when pvData is NULL and pcbData is not NULL
//                           we must read in the data from the registry
//                           anyway in order to obtain the string and perform
//                           an expand on it to obtain and return the total
//                           required size in pcbData
//                       ii) when pvData is not NULL but is of insufficient
//                           size we must re-read in the data from the
//                           registry in order to obtain the entire string
//                           and perform an expand on it to obtain and return
//                           the total required size in pcbData
//
// Remarks:
//
//  The key identified by hkey must have been opened with KEY_QUERY_VALUE
//  access.  If pszSubKey is not NULL or "", it must be able to be opened
//  with KEY_QUERY_VALUE access in the current calling context.
//
//  If the data type is REG_SZ, REG_EXPAND_SZ or REG_MULTI_SZ then any
//  returned data is guaranteed to take into account proper null termination.
//  For example:  if pcbData is not NULL, its returned size will include the
//  bytes for a null terminator  if pvData is not NULL, its returned data
//  will be properly null terminated.
//
//  If the data type is REG_EXPAND_SZ, then unless the SRRF_NOEXPAND flag
//  is set the data will be automatically expanded prior to being returned.
//  For example:  if pdwType is not NULL, its returned type will be changed
//  to REG_SZ,  if pcbData is not NULL, its returned size will include the
//  bytes for a properly expanded string.  if pvData is not NULL, its
//  returned data will be the expanded version of the string.
//
//  Reference MSDN documentation for RegQueryValueEx() for more information
//  of the behaviour when pdwType, pvData, and/or pcbData are equal to NULL.
//
// Return Values:
//
//  If the function succeeds, the return value is ERROR_SUCCESS and all out
//  parameters requested (pdwType, pvData, pcbData) are valid.
//
//  If the function fails due to insufficient space in a provided non-NULL
//  pvData, the return value is ERROR_MORE_DATA and only pdwType and pcbData
//  can contain valid data.  The content of pvData in this case is undefined.
//
// Examples:
//
//  1) read REG_SZ (or REG_EXPAND_SZ as REG_SZ) "string" data from the (default) value of an open hkey
//
//      TCHAR szData[128]
//      DWORD cbData = sizeof(pszData)
//      if (ERROR_SUCCESS == SHRegGetValue(hkey, NULL, NULL, SRRF_RT_REG_SZ, NULL, szData, &cbData))
//      {
//          // use sz (successful read)
//      }
//
//  2) read REG_SZ (or REG_EXPAND_SZ as REG_SZ) "string" data of unknown size from the "MyValue" value of an open hkey
//
//      DWORD cbData
//      if (ERROR_SUCCESS == SHRegGetValue(hkey, NULL, TEXT("MyValue"), SRRF_RT_REG_SZ, NULL, NULL, &cbData))
//      {
//          TCHAR *pszData = new TCHAR[cbData/sizeof(TCHAR)]
//          if (pszData)
//          {
//              if (ERROR_SUCCESS == SHRegGetValue(hkey, NULL, TEXT("MyValue"), SRRF_RT_REG_SZ, NULL, pszData, &cbData))
//              {
//                  // use pszData (successful read)
//              }
//              delete[] pszData
//          }
//      }
//
//  3) read "dword" data from the "MyValue" value of the "MySubKey" subkey of an open hkey
//
//      DWORD dwData
//      DWORD cbData = sizeof(dwData)
//      if (ERROR_SUCCESS == SHRegGetValue(hkey, TEXT("MySubKey"), TEXT("MyValue"), SRRF_RT_REG_DWORD, NULL, &dwData, &cbData))
//      {
//          // use dwData (successful read)
//      }
//
//  4) read "dword" data from the "MyValue" value of the "MySubKey" subkey of an open hkey (32-bit binary data also ok)
//
//      DWORD dwData
//      DWORD cbData = sizeof(dwData)
//      if (ERROR_SUCCESS == SHRegGetValue(hkey, TEXT("MySubKey"), TEXT("MyValue"), SRRF_RT_DWORD, NULL, &dwData, &cbData))
//      {
//          // use dwData (successful read)
//      }
//
//  5) determine existence of "MyValue" value of an open hkey
//
//      BOOL bExists = ERROR_SUCCESS == SHRegGetValue(hkey, NULL, TEXT("MyValue"), SRRF_RT_ANY, NULL, NULL, NULL)

LWSTDAPI_(LONG)     SHRegGetValueA(HKEY hkey, LPCSTR pszSubKey, LPCSTR pszValue, SRRF dwFlags, DWORD *pdwType, void *pvData, DWORD *pcbData);
LWSTDAPI_(LONG)     SHRegGetValueW(HKEY hkey, LPCWSTR pszSubKey, LPCWSTR pszValue, SRRF dwFlags, DWORD *pdwType, void *pvData, DWORD *pcbData);
#ifdef UNICODE
#define SHRegGetValue  SHRegGetValueW
#else
#define SHRegGetValue  SHRegGetValueA
#endif // !UNICODE
#endif // (_WIN32_IE >= 0x0602)

// These functions behave just like RegQueryValueEx(), except if the data
// type is REG_SZ, REG_EXPAND_SZ or REG_MULTI_SZ then the string is
// guaranteed to be properly null terminated.
//
// Additionally, if the data type is REG_EXPAND_SZ these functions will
// go ahead and expand out the string, and "massage" the returned *pdwType
// to be REG_SZ.

LWSTDAPI_(DWORD)    SHQueryValueExA(HKEY hkey, LPCSTR pszValue, DWORD *pdwReserved, DWORD *pdwType, void *pvData, DWORD *pcbData);
LWSTDAPI_(DWORD)    SHQueryValueExW(HKEY hkey, LPCWSTR pszValue, DWORD *pdwReserved, DWORD *pdwType, void *pvData, DWORD *pcbData);
#ifdef UNICODE
#define SHQueryValueEx  SHQueryValueExW
#else
#define SHQueryValueEx  SHQueryValueExA
#endif // !UNICODE

// Enumeration functions support.

LWSTDAPI_(LONG)     SHEnumKeyExA(HKEY hkey, DWORD dwIndex, LPSTR pszName, LPDWORD pcchName);
LWSTDAPI_(LONG)     SHEnumKeyExW(HKEY hkey, DWORD dwIndex, LPWSTR pszName, LPDWORD pcchName);
LWSTDAPI_(LONG)     SHEnumValueA(HKEY hkey, DWORD dwIndex, LPSTR pszValueName, LPDWORD pcchValueName, LPDWORD pdwType, void *pvData, LPDWORD pcbData);
LWSTDAPI_(LONG)     SHEnumValueW(HKEY hkey, DWORD dwIndex, LPWSTR pszValueName, LPDWORD pcchValueName, LPDWORD pdwType, void *pvData, LPDWORD pcbData);
LWSTDAPI_(LONG)     SHQueryInfoKeyA(HKEY hkey, LPDWORD pcSubKeys, LPDWORD pcchMaxSubKeyLen, LPDWORD pcValues, LPDWORD pcchMaxValueNameLen);
LWSTDAPI_(LONG)     SHQueryInfoKeyW(HKEY hkey, LPDWORD pcSubKeys, LPDWORD pcchMaxSubKeyLen, LPDWORD pcValues, LPDWORD pcchMaxValueNameLen);

// recursive key copy
LWSTDAPI_(DWORD)     SHCopyKeyA(HKEY hkeySrc, LPCSTR   szSrcSubKey, HKEY hkeyDest, DWORD fReserved);
LWSTDAPI_(DWORD)     SHCopyKeyW(HKEY hkeySrc, LPCWSTR wszSrcSubKey, HKEY hkeyDest, DWORD fReserved);

// Getting and setting file system paths with environment variables

LWSTDAPI_(DWORD)    SHRegGetPathA(HKEY hKey, LPCSTR pcszSubKey, LPCSTR pcszValue, LPSTR pszPath, DWORD dwFlags);
LWSTDAPI_(DWORD)    SHRegGetPathW(HKEY hKey, LPCWSTR pcszSubKey, LPCWSTR pcszValue, LPWSTR pszPath, DWORD dwFlags);
LWSTDAPI_(DWORD)    SHRegSetPathA(HKEY hKey, LPCSTR pcszSubKey, LPCSTR pcszValue, LPCSTR pcszPath, DWORD dwFlags);
LWSTDAPI_(DWORD)    SHRegSetPathW(HKEY hKey, LPCWSTR pcszSubKey, LPCWSTR pcszValue, LPCWSTR pcszPath, DWORD dwFlags);

#ifdef UNICODE
#define SHEnumKeyEx           SHEnumKeyExW
#define SHEnumValue           SHEnumValueW
#define SHQueryInfoKey        SHQueryInfoKeyW
#define SHCopyKey             SHCopyKeyW
#define SHRegGetPath          SHRegGetPathW
#define SHRegSetPath          SHRegSetPathW
#else
#define SHEnumKeyEx           SHEnumKeyExA
#define SHEnumValue           SHEnumValueA
#define SHQueryInfoKey        SHQueryInfoKeyA
#define SHCopyKey             SHCopyKeyA
#define SHRegGetPath          SHRegGetPathA
#define SHRegSetPath          SHRegSetPathA
#endif


//////////////////////////////////////////////
// User Specific Registry Access Functions
//////////////////////////////////////////////

//
// Type definitions.
//

typedef enum
{
    SHREGDEL_DEFAULT = 0x00000000,       // Delete's HKCU, or HKLM if HKCU is not found.
    SHREGDEL_HKCU    = 0x00000001,       // Delete HKCU only
    SHREGDEL_HKLM    = 0x00000010,       // Delete HKLM only.
    SHREGDEL_BOTH    = 0x00000011,       // Delete both HKCU and HKLM.
} SHREGDEL_FLAGS;

typedef enum
{
    SHREGENUM_DEFAULT = 0x00000000,       // Enumerates HKCU or HKLM if not found.
    SHREGENUM_HKCU    = 0x00000001,       // Enumerates HKCU only
    SHREGENUM_HKLM    = 0x00000010,       // Enumerates HKLM only.
    SHREGENUM_BOTH    = 0x00000011,       // Enumerates both HKCU and HKLM without duplicates.
                                          // This option is NYI.
} SHREGENUM_FLAGS;

#define     SHREGSET_HKCU           0x00000001       // Write to HKCU if empty.
#define     SHREGSET_FORCE_HKCU     0x00000002       // Write to HKCU.
#define     SHREGSET_HKLM           0x00000004       // Write to HKLM if empty.
#define     SHREGSET_FORCE_HKLM     0x00000008       // Write to HKLM.
#define     SHREGSET_DEFAULT        (SHREGSET_FORCE_HKCU | SHREGSET_HKLM)          // Default is SHREGSET_FORCE_HKCU | SHREGSET_HKLM.

typedef HANDLE HUSKEY;  // HUSKEY is a Handle to a User Specific KEY.
typedef HUSKEY *PHUSKEY;

LWSTDAPI_(LONG)        SHRegCreateUSKeyA(LPCSTR pszPath, REGSAM samDesired, HUSKEY hRelativeUSKey, PHUSKEY phNewUSKey, DWORD dwFlags);
LWSTDAPI_(LONG)        SHRegCreateUSKeyW(LPCWSTR pwzPath, REGSAM samDesired, HUSKEY hRelativeUSKey, PHUSKEY phNewUSKey, DWORD dwFlags);
LWSTDAPI_(LONG)        SHRegOpenUSKeyA(LPCSTR pszPath, REGSAM samDesired, HUSKEY hRelativeUSKey, PHUSKEY phNewUSKey, BOOL fIgnoreHKCU);
LWSTDAPI_(LONG)        SHRegOpenUSKeyW(LPCWSTR pwzPath, REGSAM samDesired, HUSKEY hRelativeUSKey, PHUSKEY phNewUSKey, BOOL fIgnoreHKCU);
LWSTDAPI_(LONG)        SHRegQueryUSValueA(HUSKEY hUSKey, LPCSTR pszValue, LPDWORD pdwType, void *pvData, LPDWORD pcbData, BOOL fIgnoreHKCU, void *pvDefaultData, DWORD dwDefaultDataSize);
LWSTDAPI_(LONG)        SHRegQueryUSValueW(HUSKEY hUSKey, LPCWSTR pwzValue, LPDWORD pdwType, void *pvData, LPDWORD pcbData, BOOL fIgnoreHKCU, void *pvDefaultData, DWORD dwDefaultDataSize);
LWSTDAPI_(LONG)        SHRegWriteUSValueA(HUSKEY hUSKey, LPCSTR pszValue, DWORD dwType, const void *pvData, DWORD cbData, DWORD dwFlags);
LWSTDAPI_(LONG)        SHRegWriteUSValueW(HUSKEY hUSKey, LPCWSTR pwzValue, DWORD dwType, const void *pvData, DWORD cbData, DWORD dwFlags);
LWSTDAPI_(LONG)        SHRegDeleteUSValueA(HUSKEY hUSKey, LPCSTR pszValue, SHREGDEL_FLAGS delRegFlags);
LWSTDAPI_(LONG)        SHRegDeleteEmptyUSKeyW(HUSKEY hUSKey, LPCWSTR pwzSubKey, SHREGDEL_FLAGS delRegFlags);
LWSTDAPI_(LONG)        SHRegDeleteEmptyUSKeyA(HUSKEY hUSKey, LPCSTR pszSubKey, SHREGDEL_FLAGS delRegFlags);
LWSTDAPI_(LONG)        SHRegDeleteUSValueW(HUSKEY hUSKey, LPCWSTR pwzValue, SHREGDEL_FLAGS delRegFlags);
LWSTDAPI_(LONG)        SHRegEnumUSKeyA(HUSKEY hUSKey, DWORD dwIndex, LPSTR pszName, LPDWORD pcchName, SHREGENUM_FLAGS enumRegFlags);
LWSTDAPI_(LONG)        SHRegEnumUSKeyW(HUSKEY hUSKey, DWORD dwIndex, LPWSTR pwzName, LPDWORD pcchName, SHREGENUM_FLAGS enumRegFlags);
LWSTDAPI_(LONG)        SHRegEnumUSValueA(HUSKEY hUSkey, DWORD dwIndex, LPSTR pszValueName, LPDWORD pcchValueName, LPDWORD pdwType, void *pvData, LPDWORD pcbData, SHREGENUM_FLAGS enumRegFlags);
LWSTDAPI_(LONG)        SHRegEnumUSValueW(HUSKEY hUSkey, DWORD dwIndex, LPWSTR pszValueName, LPDWORD pcchValueName, LPDWORD pdwType, void *pvData, LPDWORD pcbData, SHREGENUM_FLAGS enumRegFlags);
LWSTDAPI_(LONG)        SHRegQueryInfoUSKeyA(HUSKEY hUSKey, LPDWORD pcSubKeys, LPDWORD pcchMaxSubKeyLen, LPDWORD pcValues, LPDWORD pcchMaxValueNameLen, SHREGENUM_FLAGS enumRegFlags);
LWSTDAPI_(LONG)        SHRegQueryInfoUSKeyW(HUSKEY hUSKey, LPDWORD pcSubKeys, LPDWORD pcchMaxSubKeyLen, LPDWORD pcValues, LPDWORD pcchMaxValueNameLen, SHREGENUM_FLAGS enumRegFlags);
LWSTDAPI_(LONG)        SHRegCloseUSKey(HUSKEY hUSKey);


// These calls are equal to an SHRegOpenUSKey, SHRegQueryUSValue, and then a SHRegCloseUSKey.
LWSTDAPI_(LONG)        SHRegGetUSValueA(LPCSTR pszSubKey, LPCSTR pszValue, LPDWORD pdwType, void * pvData, LPDWORD pcbData, BOOL fIgnoreHKCU, void *pvDefaultData, DWORD dwDefaultDataSize);
LWSTDAPI_(LONG)        SHRegGetUSValueW(LPCWSTR pwzSubKey, LPCWSTR pwzValue, LPDWORD pdwType, void * pvData, LPDWORD pcbData, BOOL fIgnoreHKCU, void *pvDefaultData, DWORD dwDefaultDataSize);
LWSTDAPI_(LONG)        SHRegSetUSValueA(LPCSTR pszSubKey, LPCSTR pszValue, DWORD dwType, const void *pvData, DWORD cbData, DWORD dwFlags);
LWSTDAPI_(LONG)        SHRegSetUSValueW(LPCWSTR pwzSubKey, LPCWSTR pwzValue, DWORD dwType, const void *pvData, DWORD cbData, DWORD dwFlags);
LWSTDAPI_(int)         SHRegGetIntW(HKEY hk, LPCWSTR pwzKey, int iDefault);

#ifdef UNICODE
#define SHRegCreateUSKey        SHRegCreateUSKeyW
#define SHRegOpenUSKey          SHRegOpenUSKeyW
#define SHRegQueryUSValue       SHRegQueryUSValueW
#define SHRegWriteUSValue       SHRegWriteUSValueW
#define SHRegDeleteUSValue      SHRegDeleteUSValueW
#define SHRegDeleteEmptyUSKey   SHRegDeleteEmptyUSKeyW
#define SHRegEnumUSKey          SHRegEnumUSKeyW
#define SHRegEnumUSValue        SHRegEnumUSValueW
#define SHRegQueryInfoUSKey     SHRegQueryInfoUSKeyW
#define SHRegGetUSValue         SHRegGetUSValueW
#define SHRegSetUSValue         SHRegSetUSValueW
#define SHRegGetInt             SHRegGetIntW
#else
#define SHRegCreateUSKey        SHRegCreateUSKeyA
#define SHRegOpenUSKey          SHRegOpenUSKeyA
#define SHRegQueryUSValue       SHRegQueryUSValueA
#define SHRegWriteUSValue       SHRegWriteUSValueA
#define SHRegDeleteUSValue      SHRegDeleteUSValueA
#define SHRegDeleteEmptyUSKey   SHRegDeleteEmptyUSKeyA
#define SHRegEnumUSKey          SHRegEnumUSKeyA
#define SHRegEnumUSValue        SHRegEnumUSValueA
#define SHRegQueryInfoUSKey     SHRegQueryInfoUSKeyA
#define SHRegGetUSValue         SHRegGetUSValueA
#define SHRegSetUSValue         SHRegSetUSValueA
#endif

LWSTDAPI_(BOOL) SHRegGetBoolUSValueA(LPCSTR pszSubKey, LPCSTR pszValue, BOOL fIgnoreHKCU, BOOL fDefault);
LWSTDAPI_(BOOL) SHRegGetBoolUSValueW(LPCWSTR pszSubKey, LPCWSTR pszValue, BOOL fIgnoreHKCU, BOOL fDefault);

#ifdef UNICODE
#define SHRegGetBoolUSValue SHRegGetBoolUSValueW
#else
#define SHRegGetBoolUSValue SHRegGetBoolUSValueA
#endif

//
//  Association APIs
//
//  these APIs are to assist in accessing the data in HKCR
//  getting the Command strings and exe paths
//  for different verbs and extensions are simplified this way
//

enum {
    ASSOCF_INIT_NOREMAPCLSID           = 0x00000001,  //  do not remap clsids to progids
    ASSOCF_INIT_BYEXENAME              = 0x00000002,  //  executable is being passed in
    ASSOCF_OPEN_BYEXENAME              = 0x00000002,  //  executable is being passed in
    ASSOCF_INIT_DEFAULTTOSTAR          = 0x00000004,  //  treat "*" as the BaseClass
    ASSOCF_INIT_DEFAULTTOFOLDER        = 0x00000008,  //  treat "Folder" as the BaseClass
    ASSOCF_NOUSERSETTINGS              = 0x00000010,  //  dont use HKCU
    ASSOCF_NOTRUNCATE                  = 0x00000020,  //  dont truncate the return string
    ASSOCF_VERIFY                      = 0x00000040,  //  verify data is accurate (DISK HITS)
    ASSOCF_REMAPRUNDLL                 = 0x00000080,  //  actually gets info about rundlls target if applicable
    ASSOCF_NOFIXUPS                    = 0x00000100,  //  attempt to fix errors if found
    ASSOCF_IGNOREBASECLASS             = 0x00000200,  //  dont recurse into the baseclass
};

typedef DWORD ASSOCF;


typedef enum {
    ASSOCSTR_COMMAND      = 1,  //  shell\verb\command string
    ASSOCSTR_EXECUTABLE,        //  the executable part of command string
    ASSOCSTR_FRIENDLYDOCNAME,   //  friendly name of the document type
    ASSOCSTR_FRIENDLYAPPNAME,   //  friendly name of executable
    ASSOCSTR_NOOPEN,            //  noopen value
    ASSOCSTR_SHELLNEWVALUE,     //  query values under the shellnew key
    ASSOCSTR_DDECOMMAND,        //  template for DDE commands
    ASSOCSTR_DDEIFEXEC,         //  DDECOMMAND to use if just create a process
    ASSOCSTR_DDEAPPLICATION,    //  Application name in DDE broadcast
    ASSOCSTR_DDETOPIC,          //  Topic Name in DDE broadcast
    ASSOCSTR_INFOTIP,           //  info tip for an item, or list of properties to create info tip from
    ASSOCSTR_QUICKTIP,          //  same as ASSOCSTR_INFOTIP, except, this list contains only quickly retrievable properties
    ASSOCSTR_TILEINFO,          //  similar to ASSOCSTR_INFOTIP - lists important properties for tileview
    ASSOCSTR_CONTENTTYPE,       //  MIME Content type
    ASSOCSTR_DEFAULTICON,       //  Default icon source
    ASSOCSTR_SHELLEXTENSION,    //  Guid string pointing to the Shellex\Shellextensionhandler value.
    ASSOCSTR_MAX                //  last item in enum...
} ASSOCSTR;

typedef enum {
    ASSOCKEY_SHELLEXECCLASS = 1,  //  the key that should be passed to ShellExec(hkeyClass)
    ASSOCKEY_APP,                 //  the "Application" key for the association
    ASSOCKEY_CLASS,               //  the progid or class key
    ASSOCKEY_BASECLASS,           //  the BaseClass key
    ASSOCKEY_MAX                  //  last item in enum...
} ASSOCKEY;

typedef enum {
    ASSOCDATA_MSIDESCRIPTOR = 1,  //  Component Descriptor to pass to MSI APIs
    ASSOCDATA_NOACTIVATEHANDLER,  //  restrict attempts to activate window
    ASSOCDATA_QUERYCLASSSTORE,    //  should check with the NT Class Store
    ASSOCDATA_HASPERUSERASSOC,    //  defaults to user specified association
    ASSOCDATA_EDITFLAGS,          //  Edit flags.
    ASSOCDATA_VALUE,              //  use pszExtra as the Value name
    ASSOCDATA_MAX
} ASSOCDATA;

typedef enum {
    ASSOCENUM_NONE
} ASSOCENUM;

#undef INTERFACE
#define INTERFACE IQueryAssociations

DECLARE_INTERFACE_( IQueryAssociations, IUnknown )
{
    // IUnknown methods
    STDMETHOD (QueryInterface)(THIS_ REFIID riid, void **ppv) PURE;
    STDMETHOD_(ULONG, AddRef) ( THIS ) PURE;
    STDMETHOD_(ULONG, Release) ( THIS ) PURE;

    // IQueryAssociations methods
    STDMETHOD (Init)(THIS_ ASSOCF flags, LPCWSTR pszAssoc, HKEY hkProgid, HWND hwnd) PURE;
    STDMETHOD (GetString)(THIS_ ASSOCF flags, ASSOCSTR str, LPCWSTR pszExtra, LPWSTR pszOut, DWORD *pcchOut) PURE;
    STDMETHOD (GetKey)(THIS_ ASSOCF flags, ASSOCKEY key, LPCWSTR pszExtra, HKEY *phkeyOut) PURE;
    STDMETHOD (GetData)(THIS_ ASSOCF flags, ASSOCDATA data, LPCWSTR pszExtra, LPVOID pvOut, DWORD *pcbOut) PURE;
    STDMETHOD (GetEnum)(THIS_ ASSOCF flags, ASSOCENUM assocenum, LPCWSTR pszExtra, REFIID riid, LPVOID *ppvOut) PURE;
};


LWSTDAPI AssocCreate(CLSID clsid, REFIID riid, LPVOID *ppv);

//  wrappers for the interface
LWSTDAPI AssocQueryStringA(ASSOCF flags, ASSOCSTR str, LPCSTR pszAssoc, LPCSTR pszExtra, LPSTR pszOut, DWORD *pcchOut);
//  wrappers for the interface
LWSTDAPI AssocQueryStringW(ASSOCF flags, ASSOCSTR str, LPCWSTR pszAssoc, LPCWSTR pszExtra, LPWSTR pszOut, DWORD *pcchOut);
#ifdef UNICODE
#define AssocQueryString  AssocQueryStringW
#else
#define AssocQueryString  AssocQueryStringA
#endif // !UNICODE
LWSTDAPI AssocQueryStringByKeyA(ASSOCF flags, ASSOCSTR str, HKEY hkAssoc, LPCSTR pszExtra, LPSTR pszOut, DWORD *pcchOut);
LWSTDAPI AssocQueryStringByKeyW(ASSOCF flags, ASSOCSTR str, HKEY hkAssoc, LPCWSTR pszExtra, LPWSTR pszOut, DWORD *pcchOut);
#ifdef UNICODE
#define AssocQueryStringByKey  AssocQueryStringByKeyW
#else
#define AssocQueryStringByKey  AssocQueryStringByKeyA
#endif // !UNICODE
LWSTDAPI AssocQueryKeyA(ASSOCF flags, ASSOCKEY key, LPCSTR pszAssoc, LPCSTR pszExtra, HKEY *phkeyOut);
LWSTDAPI AssocQueryKeyW(ASSOCF flags, ASSOCKEY key, LPCWSTR pszAssoc, LPCWSTR pszExtra, HKEY *phkeyOut);
#ifdef UNICODE
#define AssocQueryKey  AssocQueryKeyW
#else
#define AssocQueryKey  AssocQueryKeyA
#endif // !UNICODE

#if (_WIN32_IE >= 0x0601)
//  AssocIsDangerous() checks a file type to determine whether it is "Dangerous"
//      this maps to the IE download dialog's forcing a prompt to open or save.
//      dangerous file types should be handled more carefully than other file types.
//
//  Parameter:  pszAssoc - type to check.  may be an extension or progid.  (".exe" or "exefile" would both be valid)
//
//  Returns: TRUE if the file type is dangerous.
//
//  NOTES:
//
//      this API first checks a hardcoded list of known dangerous types.
//      then it checks the editflags for the file type looking for the FTA_AlwaysUnsafe bit.
//      then it checks Safer policies.
//      
LWSTDAPI_(BOOL) AssocIsDangerous(LPCWSTR pszAssoc);
#endif // (_WIN32_IE >= 0x0601)

#if (_WIN32_IE >= 0x0603)
//  PERCEIVED types:
typedef enum {
    PERCEIVED_TYPE_CUSTOM          = -3,
    PERCEIVED_TYPE_UNSPECIFIED     = -2,
    PERCEIVED_TYPE_FOLDER          = -1,
    PERCEIVED_TYPE_UNKNOWN         = 0,
    PERCEIVED_TYPE_TEXT,
    PERCEIVED_TYPE_IMAGE,
    PERCEIVED_TYPE_AUDIO,
    PERCEIVED_TYPE_VIDEO,
    PERCEIVED_TYPE_COMPRESSED,
    PERCEIVED_TYPE_DOCUMENT,
    PERCEIVED_TYPE_SYSTEM,
    PERCEIVED_TYPE_APPLICATION
} PERCEIVED;

#define PERCEIVEDFLAG_UNDEFINED     0x0000
#define PERCEIVEDFLAG_SOFTCODED     0x0001
#define PERCEIVEDFLAG_HARDCODED     0x0002
#define PERCEIVEDFLAG_NATIVESUPPORT 0x0004
#define PERCEIVEDFLAG_GDIPLUS       0x0010
#define PERCEIVEDFLAG_WMSDK         0x0020
#define PERCEIVEDFLAG_ZIPFOLDER     0x0040

typedef DWORD PERCEIVEDFLAG;

LWSTDAPI AssocGetPerceivedType(__in LPCWSTR pszExt, __out PERCEIVED *ptype, __out PERCEIVEDFLAG *pflag, __out_opt LPWSTR *ppszType);
#endif  // (_WIN32_IE >= 0x0603)



#endif //  NO_SHLWAPI_REG



#ifndef NO_SHLWAPI_STREAM
//
//=============== Stream Routines ===================================
//

LWSTDAPI_(struct IStream *) SHOpenRegStreamA(HKEY hkey, LPCSTR pszSubkey, LPCSTR pszValue, DWORD grfMode);
LWSTDAPI_(struct IStream *) SHOpenRegStreamW(HKEY hkey, LPCWSTR pszSubkey, LPCWSTR pszValue, DWORD grfMode);
#ifdef UNICODE
#define SHOpenRegStream  SHOpenRegStreamW
#else
#define SHOpenRegStream  SHOpenRegStreamA
#endif // !UNICODE
LWSTDAPI_(struct IStream *) SHOpenRegStream2A(HKEY hkey, LPCSTR pszSubkey, LPCSTR pszValue, DWORD grfMode);
LWSTDAPI_(struct IStream *) SHOpenRegStream2W(HKEY hkey, LPCWSTR pszSubkey, LPCWSTR pszValue, DWORD grfMode);
#ifdef UNICODE
#define SHOpenRegStream2  SHOpenRegStream2W
#else
#define SHOpenRegStream2  SHOpenRegStream2A
#endif // !UNICODE
// New code always wants new implementation...
#undef SHOpenRegStream
#define SHOpenRegStream SHOpenRegStream2

LWSTDAPI SHCreateStreamOnFileA(LPCSTR pszFile, DWORD grfMode, struct IStream **ppstm);
LWSTDAPI SHCreateStreamOnFileW(LPCWSTR pszFile, DWORD grfMode, struct IStream **ppstm);
#ifdef UNICODE
#define SHCreateStreamOnFile  SHCreateStreamOnFileW
#else
#define SHCreateStreamOnFile  SHCreateStreamOnFileA
#endif // !UNICODE

#if (_WIN32_IE >= 0x0600)

LWSTDAPI SHCreateStreamOnFileEx(LPCWSTR pszFile, DWORD grfMode, DWORD dwAttributes, BOOL fCreate, struct IStream * pstmTemplate, struct IStream **ppstm);

#endif // (_WIN32_IE >= 0x0600)



#endif // NO_SHLWAPI_STREAM



#ifndef NO_SHLWAPI_HTTP
//
//=============== HTTP helper Routines ===================================
//

#if (_WIN32_IE >= 0x0603)
LWSTDAPI   GetAcceptLanguagesA(LPSTR psz, DWORD* pcch);
LWSTDAPI   GetAcceptLanguagesW(LPWSTR psz, DWORD* pcch);
#ifdef UNICODE
#define GetAcceptLanguages  GetAcceptLanguagesW
#else
#define GetAcceptLanguages  GetAcceptLanguagesA
#endif // !UNICODE
#endif  // (_WIN32_IE >= 0x0603)

#endif // NO_SHLWAPI_HTTP


#if (_WIN32_IE >= 0x0601)
#define SHGVSPB_PERUSER             0x00000001 // must have one of PERUSER or ALLUSERS
#define SHGVSPB_ALLUSERS            0x00000002
#define SHGVSPB_PERFOLDER           0x00000004 // must have one of PERFOLDER ALLFOLDERS or INHERIT
#define SHGVSPB_ALLFOLDERS          0x00000008
#define SHGVSPB_INHERIT             0x00000010
#define SHGVSPB_ROAM                0x00000020 // modifies the above
#define SHGVSPB_NOAUTODEFAULTS      0x80000000 // turns off read delegation to more general property bags

#define SHGVSPB_FOLDER              (SHGVSPB_PERUSER | SHGVSPB_PERFOLDER)
#define SHGVSPB_FOLDERNODEFAULTS    (SHGVSPB_PERUSER | SHGVSPB_PERFOLDER | SHGVSPB_NOAUTODEFAULTS)
#define SHGVSPB_USERDEFAULTS        (SHGVSPB_PERUSER | SHGVSPB_ALLFOLDERS)
#define SHGVSPB_GLOBALDEAFAULTS     (SHGVSPB_ALLUSERS | SHGVSPB_ALLFOLDERS)

LWSTDAPI SHGetViewStatePropertyBag(LPCITEMIDLIST pidl, LPCWSTR pszBagName, DWORD dwFlags, REFIID riid, void** ppv);
#endif // (_WIN32_IE >= 0x0601)


// Shared memory apis

#if (_WIN32_IE >= 0x0603)
LWSTDAPI_(HANDLE)   SHAllocShared(const void *pvData, DWORD dwSize, DWORD dwProcessId);
LWSTDAPI_(BOOL)     SHFreeShared(HANDLE hData,DWORD dwProcessId);
LWSTDAPI_(void *)   SHLockShared(HANDLE hData, DWORD dwProcessId);
LWSTDAPI_(BOOL)     SHUnlockShared(void *pvData);
#endif // _WIN32_IE >= 0x0603


#if (_WIN32_IE >= 0x0500)

// SHAutoComplete
//      hwndEdit - HWND of editbox, ComboBox or ComboBoxEx.
//      dwFlags - Flags to indicate what to AutoAppend or AutoSuggest for the editbox.
//
// WARNING:
//    Caller needs to have called CoInitialize() or OleInitialize()
//    and cannot call CoUninit/OleUninit until after
//    WM_DESTROY on hwndEdit.
//
//  dwFlags values:
#define SHACF_DEFAULT                   0x00000000  // Currently (SHACF_FILESYSTEM | SHACF_URLALL)
#define SHACF_FILESYSTEM                0x00000001  // This includes the File System as well as the rest of the shell (Desktop\My Computer\Control Panel\)
#define SHACF_URLALL                    (SHACF_URLHISTORY | SHACF_URLMRU)
#define SHACF_URLHISTORY                0x00000002  // URLs in the User's History
#define SHACF_URLMRU                    0x00000004  // URLs in the User's Recently Used list.
#define SHACF_USETAB                    0x00000008  // Use the tab to move thru the autocomplete possibilities instead of to the next dialog/window control.
#define SHACF_FILESYS_ONLY              0x00000010  // This includes the File System

#if (_WIN32_IE >= 0x0600)
#define SHACF_FILESYS_DIRS              0x00000020  // Same as SHACF_FILESYS_ONLY except it only includes directories, UNC servers, and UNC server shares.
#endif // (_WIN32_IE >= 0x0600)

#define SHACF_AUTOSUGGEST_FORCE_ON      0x10000000  // Ignore the registry default and force the feature on.
#define SHACF_AUTOSUGGEST_FORCE_OFF     0x20000000  // Ignore the registry default and force the feature off.
#define SHACF_AUTOAPPEND_FORCE_ON       0x40000000  // Ignore the registry default and force the feature on. (Also know as AutoComplete)
#define SHACF_AUTOAPPEND_FORCE_OFF      0x80000000  // Ignore the registry default and force the feature off. (Also know as AutoComplete)

LWSTDAPI SHAutoComplete(HWND hwndEdit, DWORD dwFlags);

LWSTDAPI SHSetThreadRef(IUnknown *punk);
LWSTDAPI SHGetThreadRef(IUnknown **ppunk);

LWSTDAPI_(BOOL) SHSkipJunction(struct IBindCtx* pbc, const CLSID *pclsid);
#endif // (_WIN32_IE >= 0x0500)

#if (_WIN32_IE >= 0x0603)
LWSTDAPI SHCreateThreadRef(LONG *pcRef, IUnknown **ppunk);
#endif // (_WIN32_IE >= 0x0603)

#define CTF_INSIST          0x00000001      // SHCreateThread() dwFlags - call pfnThreadProc synchronously if CreateThread() fails
#define CTF_THREAD_REF      0x00000002      // hold a reference to the creating thread
#define CTF_PROCESS_REF     0x00000004      // hold a reference to the creating process
#define CTF_COINIT          0x00000008      // init COM for the created thread
#define CTF_FREELIBANDEXIT  0x00000010      // hold a ref to the DLL and call FreeLibraryAndExitThread() when done
#define CTF_REF_COUNTED     0x00000020      // thread supports ref counting via SHGetThreadRef() or CTF_THREAD_REF so that child threads can keep this thread alive
#define CTF_WAIT_ALLOWCOM   0x00000040      // while waiting for pfnCallback, allow COM marshaling to the blocked calling thread

LWSTDAPI_(BOOL) SHCreateThread(LPTHREAD_START_ROUTINE pfnThreadProc, void *pData, DWORD dwFlags, LPTHREAD_START_ROUTINE pfnCallback);

LWSTDAPI SHReleaseThreadRef(); // release a CTF_THREAD_REF reference earlier than the return of pfnThreadProc

#ifndef NO_SHLWAPI_GDI
//
//====== GDI helper functions  ================================================
//

LWSTDAPI_(HPALETTE) SHCreateShellPalette(HDC hdc);

#if (_WIN32_IE >= 0x0500)

LWSTDAPI_(void)     ColorRGBToHLS(COLORREF clrRGB, WORD* pwHue, WORD* pwLuminance, WORD* pwSaturation);
LWSTDAPI_(COLORREF) ColorHLSToRGB(WORD wHue, WORD wLuminance, WORD wSaturation);
LWSTDAPI_(COLORREF) ColorAdjustLuma(COLORREF clrRGB, int n, BOOL fScale);


#endif  // _WIN32_IE >= 0x0500

#endif // NO_SHLWAPI_GDI


//
//====== DllGetVersion  =======================================================
//

typedef struct _DLLVERSIONINFO
{
    DWORD cbSize;
    DWORD dwMajorVersion;                   // Major version
    DWORD dwMinorVersion;                   // Minor version
    DWORD dwBuildNumber;                    // Build number
    DWORD dwPlatformID;                     // DLLVER_PLATFORM_*
} DLLVERSIONINFO;

// Platform IDs for DLLVERSIONINFO
#define DLLVER_PLATFORM_WINDOWS         0x00000001      // Windows 95
#define DLLVER_PLATFORM_NT              0x00000002      // Windows NT

#if (_WIN32_IE >= 0x0501)

typedef struct _DLLVERSIONINFO2
{
    DLLVERSIONINFO info1;
    DWORD dwFlags;                          // No flags currently defined
    ULONGLONG ullVersion;                   // Encoded as:
                                            // Major 0xFFFF 0000 0000 0000
                                            // Minor 0x0000 FFFF 0000 0000
                                            // Build 0x0000 0000 FFFF 0000
                                            // QFE   0x0000 0000 0000 FFFF
} DLLVERSIONINFO2;

#define DLLVER_MAJOR_MASK                    0xFFFF000000000000
#define DLLVER_MINOR_MASK                    0x0000FFFF00000000
#define DLLVER_BUILD_MASK                    0x00000000FFFF0000
#define DLLVER_QFE_MASK                      0x000000000000FFFF

#endif

#define MAKEDLLVERULL(major, minor, build, qfe) \
        (((ULONGLONG)(major) << 48) |        \
         ((ULONGLONG)(minor) << 32) |        \
         ((ULONGLONG)(build) << 16) |        \
         ((ULONGLONG)(  qfe) <<  0))

//
// The caller should always GetProcAddress("DllGetVersion"), not
// implicitly link to it.
//

typedef HRESULT (CALLBACK* DLLGETVERSIONPROC)(DLLVERSIONINFO *);

// DllInstall (to be implemented by self-installing DLLs)
STDAPI DllInstall(BOOL bInstall, LPCWSTR pszCmdLine);


#if (_WIN32_IE >= 0x0602)

// Function to see if Internet Explorer Enhanced Security Configuration is active for the current user
LWSTDAPI_(BOOL) IsInternetESCEnabled();
#endif // (_WIN32_IE >= 0x0602)


#ifdef __cplusplus
}
#endif

#ifdef _WIN32
#include <poppack.h>
#endif

#endif


#endif  // _INC_SHLWAPI


