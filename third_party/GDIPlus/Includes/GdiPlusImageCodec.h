/**************************************************************************\
*
* Copyright (c) 2000, Microsoft Corp.  All Rights Reserved.
*
* Module Name:
*
*   GdiplusImageCodec.h
*
* Abstract:
*
*   APIs for imaging codecs.
*
\**************************************************************************/

#ifndef _GDIPLUSIMAGECODEC_H
#define _GDIPLUSIMAGECODEC_H

//--------------------------------------------------------------------------
// Codec Management APIs
//--------------------------------------------------------------------------

inline Status 
GetImageDecodersSize(
    OUT UINT *numDecoders,
    OUT UINT *size)
{
    return DllExports::GdipGetImageDecodersSize(numDecoders, size);
}


inline Status 
GetImageDecoders(
    IN UINT numDecoders,
    IN UINT size,
    OUT ImageCodecInfo *decoders)
{
    return DllExports::GdipGetImageDecoders(numDecoders, size, decoders);
}


inline Status 
GetImageEncodersSize(
    OUT UINT *numEncoders, 
    OUT UINT *size)
{
    return DllExports::GdipGetImageEncodersSize(numEncoders, size);
}


inline Status 
GetImageEncoders(
    IN UINT numEncoders,
    IN UINT size,
    OUT ImageCodecInfo *encoders)
{
    return DllExports::GdipGetImageEncoders(numEncoders, size, encoders);
}

inline Status 
AddImageCodec(
    IN const ImageCodecInfo* codec)
{
    return DllExports::GdipAddImageCodec(codec);
}

inline Status 
RemoveImageCodec(
    IN const ImageCodecInfo* codec)
{
    return DllExports::GdipRemoveImageCodec(codec);
}

#endif  // _GDIPLUSIMAGECODEC_H
