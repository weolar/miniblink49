/* yvals.h values header for Microsoft C/C++ */
#ifndef _YVALS
#define _YVALS
#include <use_ansi.h>

/* Define _CRTIMP */
#ifndef _CRTIMP
#ifdef	_DLL
#define _CRTIMP __declspec(dllimport)
#else	/* ndef _DLL */
#define _CRTIMP
#endif	/* _DLL */
#endif	/* _CRTIMP */

#ifdef  _MSC_VER
#pragma pack(push,8)
#endif  /* _MSC_VER */

 #pragma warning(disable: 4018 4114 4146 4244 4245)
 #pragma warning(disable: 4663 4664 4665)
 #pragma warning(disable: 4237 4284 4290 4514)
		/* NAMESPACE */
 #if defined(__cplusplus)
  #define _STD			std::
  #define _STD_BEGIN	namespace std {
  #define _STD_END		};
  #define _STD_USING
 #else
  #define _STD			::
  #define _STD_BEGIN
  #define _STD_END
 #endif /* __cplusplus */
_STD_BEGIN
		/* TYPE bool */
 #if defined(__cplusplus)
typedef bool _Bool;
 #endif /* __cplusplus */
		/* INTEGER PROPERTIES */
#define _MAX_EXP_DIG	8	/* for parsing numerics */
#define _MAX_INT_DIG	32
#define _MAX_SIG_DIG	36
		/* STDIO PROPERTIES */
#define _Filet _iobuf

#ifndef _FPOS_T_DEFINED
#define _FPOSOFF(fp)	((long)(fp))
#endif /* _FPOS_T_DEFINED */

		/* NAMING PROPERTIES */
 #if defined(__cplusplus)
  #define _C_LIB_DECL extern "C" {
  #define _END_C_LIB_DECL }
 #else
  #define _C_LIB_DECL
  #define _END_C_LIB_DECL
 #endif /* __cplusplus */
#define _CDECL
		// CLASS _Lockit
 #if defined(__cplusplus)
class _CRTIMP _Lockit
	{	// lock while object in existence
public:
  #ifdef _MT
   #define _LOCKIT(x)	lockit x
	_Lockit();
	~_Lockit();
  #else
   #define _LOCKIT(x)
	_Lockit()
		{}
	~_Lockit()
		{}
  #endif /* _MT */
	};
 #endif /* __cplusplus */
		/* MISCELLANEOUS MACROS */
#define _L(c)	L##c
#define _Mbstinit(x)	mbstate_t x = {0}
#define _MAX	_cpp_max
#define _MIN	_cpp_min
_STD_END
#ifdef  _MSC_VER
#pragma pack(pop)
#endif  /* _MSC_VER */

#endif /* _YVALS */

/*
 * Copyright (c) 1996 by P.J. Plauger.  ALL RIGHTS RESERVED. 
 * Consult your license regarding permissions and restrictions.
 */

