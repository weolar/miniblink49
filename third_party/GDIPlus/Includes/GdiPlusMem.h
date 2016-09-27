/**************************************************************************\
*
* Copyright (c) 1998-2000, Microsoft Corp.  All Rights Reserved.
*
* Module Name:
*
*   GdiplusMem.h
*
* Abstract:
*
*   Flat GDI+ Memory Allocators - header file
*
\**************************************************************************/

// TODO: this file style needs to be made internally consistent with the way
//       it handles breaking the long argument lists across multiple lines

#ifndef _GDIPLUSMEM_H
#define _GDIPLUSMEM_H

#define WINGDIPAPI __stdcall

// currently, only C++ wrapper API's force const.

#ifdef _GDIPLUS_H
#define GDIPCONST const
#else
#define GDIPCONST
#endif

#ifdef __cplusplus
extern "C" {
#endif

//----------------------------------------------------------------------------
// Memory Allocation APIs
//----------------------------------------------------------------------------

void* WINGDIPAPI
GdipAlloc(size_t size);

void WINGDIPAPI
GdipFree(void* ptr);

#ifdef __cplusplus
}
#endif

#endif // !_GDIPLUSMEM_H
