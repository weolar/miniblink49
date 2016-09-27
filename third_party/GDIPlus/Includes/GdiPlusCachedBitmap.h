/**************************************************************************
*
* Copyright (c) 2000 Microsoft Corporation
*
* Module Name:
*
*   CachedBitmap class definition
*
* Abstract:
*
*   CachedBitmap is a representation of an accelerated drawing
*   that has restrictions on what operations are allowed in order
*   to accelerate the drawing to the destination.
*
**************************************************************************/

#ifndef _GDIPLUSCACHEDBITMAP_H
#define _GDIPLUSCACHEDBITMAP_H

/**************************************************************************
*
* Class Name:
*
*   CachedBitmap
*
* Abstract:
*
*   An object to store a bitmap prepared for rendering on a particular
*   Graphics object. The memory storage for the CachedBitmap is opaque
*   to the other Engine code, so the only operations supported are
*   initializing the data (with a bitmap) and using the graphics to
*   draw it on the screen with an integer offset.
*
*   Look for the class definition in GdiplusHeaders.h
*
* Created:
*
*   04/23/2000 asecchia
*      Created it.
*
**************************************************************************/
inline 
CachedBitmap::CachedBitmap(
    IN Bitmap *bitmap, 
    IN Graphics *graphics)
{
    nativeCachedBitmap = NULL;    

    lastResult = DllExports::GdipCreateCachedBitmap(
        (GpBitmap *)bitmap->nativeImage,
        graphics->nativeGraphics,
        &nativeCachedBitmap
    );
}

inline 
CachedBitmap::~CachedBitmap()
{
    DllExports::GdipDeleteCachedBitmap(nativeCachedBitmap);
}

inline Status 
CachedBitmap::GetLastStatus() const 
{
    Status lastStatus = lastResult;
    lastResult = Ok;    
    return (lastStatus);
}

#endif

