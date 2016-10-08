/***
*useoldio.h - force the use of the Microsoft "classic" iostream libraries.
*
*       Copyright (c) 1996-2001, Microsoft Corporation. All rights reserved.
*
*Purpose:
*       Generates default library directives for the old ("classic") IOSTREAM
*       libraries.  The exact name of the library specified in the directive
*       depends on the compiler switches (-ML, -MT, -MD, -MLd, -MTd, and -MDd).
*
*       This header file is only included by other header files.
*
*       [Public]
*
****/

#ifndef _USE_OLD_IOSTREAMS
#define _USE_OLD_IOSTREAMS
#if defined(DEPRECATE_IOSTREAMS)
/*
 * Warning C4995, '_OLD_IOSTREAMS_ARE_DEPRECATED' is a deprecated name, is 
 * being issued because the old I/O Streams headers iostreams.h et al will no
 * longer be supported from VC8.  Replace references such as #include 
 * <iostreams.h> with #include <iostreams>, using the new, more conformant, I/O
 * Streams headers.
 */

#pragma deprecated(_OLD_IOSTREAMS_ARE_DEPRECATED)
extern void _OLD_IOSTREAMS_ARE_DEPRECATED();
#endif  /* DEPRECATE_IOSTREAMS */
#ifdef  _MT
#ifdef  _DLL
#ifdef  _DEBUG
#pragma comment(lib,"msvcirtd")
#else   /* _DEBUG */
#pragma comment(lib,"msvcirt")
#endif  /* _DEBUG */

#else   /* _DLL */
#ifdef  _DEBUG
#pragma comment(lib,"libcimtd")
#else   /* _DEBUG */
#pragma comment(lib,"libcimt")
#endif  /* _DEBUG */
#endif  /* _DLL */

#else   /* _MT */
#ifdef  _DEBUG
#pragma comment(lib,"libcid")
#else   /* _DEBUG */
#pragma comment(lib,"libci")
#endif  /* _DEBUG */
#endif

#endif  /* _USE_OLD_IOSTREAMS */

