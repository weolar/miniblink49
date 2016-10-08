/***
*use_ansi.h - pragmas for ANSI Standard C++ libraries
*
*	Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This header is intended to force the use of the appropriate ANSI
*       Standard C++ libraries whenever it is included.
*
*       [Public]
*
****/

#ifndef _USE_ANSI_CPP
#define _USE_ANSI_CPP

#ifdef _MT
#ifdef _DLL
#ifdef _DEBUG
#pragma comment(lib,"msvcprtd")
#else	// _DEBUG
#pragma comment(lib,"msvcprt")
#endif	// _DEBUG

#else	// _DLL
#ifdef _DEBUG
#pragma comment(lib,"libcpmtd")
#else	// _DEBUG
#pragma comment(lib,"libcpmt")
#endif	// _DEBUG
#endif	// _DLL

#else	// _MT
#ifdef _DEBUG
#pragma comment(lib,"libcpd")
#else	// _DEBUG
#pragma comment(lib,"libcp")
#endif	// _DEBUG
#endif

#endif	// _USE_ANSI_CPP

