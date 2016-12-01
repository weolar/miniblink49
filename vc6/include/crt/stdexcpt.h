/***
*stdexcpt.h - User include file for standard exception classes
*
*       Copyright (c) Microsoft Corporation. All rights reserved.
*
*Purpose:
*       This file is the previous location of the standard exception class
*       definitions, now found in the standard header <exception>.
*
*       [Public]
*
****/

#if     _MSC_VER > 1000
#pragma once
#endif

#ifndef _INC_STDEXCPT
#define _INC_STDEXCPT

#if     !defined(_WIN32)
#error ERROR: Only Win32 target supported!
#endif


#ifdef  __cplusplus

#include <exception>

#endif  /* __cplusplus */
#endif  /* _INC_STDEXCPT */

