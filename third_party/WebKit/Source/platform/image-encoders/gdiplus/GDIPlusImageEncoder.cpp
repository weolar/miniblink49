/*
* Copyright (c) 2010, Google Inc. All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions are
* met:
*
*     * Redistributions of source code must retain the above copyright
* notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above
* copyright notice, this list of conditions and the following disclaimer
* in the documentation and/or other materials provided with the
* distribution.
*     * Neither the name of Google Inc. nor the names of its
* contributors may be used to endorse or promote products derived from
* this software without specific prior written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
* A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
* OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
* DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
* THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
* (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
* OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "config.h"
#include <windows.h>

#undef min
#undef max
#include <algorithm>

using std::max;
using std::min;
#include <gdiplus.h>

#include "platform/image-encoders/gdiplus/GDIPlusImageEncoder.h"

#include "SkBitmap.h"
#include "platform/graphics/ImageBuffer.h"
#include "platform/graphics/GDIPlusInit.h"
#include "skia/include/core/SkImageEncoder.h"
#include "skia/include/core/SkData.h"
#include "skia/include/core/SkStream.h"

namespace blink {

bool GDIPlusImageEncoder::encode(const SkBitmap& bitmap, EncoderType type, Vector<unsigned char>* output)
{
    SkAutoLockPixels bitmapLock(bitmap);

    if (bitmap.colorType() != kN32_SkColorType || !bitmap.getPixels())
        return false; // Only support 32 bit/pixel skia bitmaps.

    if (bitmap.width() * bitmap.height() > 2024 * 2024)
        return false;

    //return encodePixels(IntSize(bitmap.width(), bitmap.height()), static_cast<unsigned char*>(bitmap.getPixels()), true, output);
    initGDIPlusClsids();

    const CLSID* clsid = &s_pngClsid;
    if (JPG == type)
        clsid = &s_jpgClsid;

    bool ok = false;
    IStream* pIStream = nullptr;
    Gdiplus::Bitmap* imageBitmap = nullptr;
    SkData* encodedData = nullptr;
    HGLOBAL hMem = NULL;
    SkImageEncoder* encoder = nullptr;

    BITMAPINFO bmi = {0};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = bitmap.width();
    bmi.bmiHeader.biHeight = -bitmap.height();
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biCompression = BI_RGB;
    bmi.bmiHeader.biBitCount = 32;
    
    do {
        encoder = CreateARGBImageEncoder();
        encodedData = encoder->encodeData(bitmap, 1);
        if (!encodedData)
            break;

        int nWidthBytes = bmi.bmiHeader.biWidth * 4;
        int nSize = nWidthBytes * (-bmi.bmiHeader.biHeight);
        int encodedDataSize = encodedData->size();
        if (encodedDataSize != nSize)
            break;

        imageBitmap = Gdiplus::Bitmap::FromBITMAPINFO(&bmi, (void *)encodedData->data());
        if (!imageBitmap)
            break;
        HRESULT hr = ::CreateStreamOnHGlobal(NULL, true, &pIStream);
        if (S_OK != hr)
            break;        
        Gdiplus::Status status = imageBitmap->Save(pIStream, clsid, NULL);

        LARGE_INTEGER liTemp = { 0 };
        pIStream->Seek(liTemp, STREAM_SEEK_SET, NULL);
        DWORD dwSize = 0;
        STATSTG stats = { 0 };
        pIStream->Stat(&stats, 0);
        if (0 == stats.cbSize.QuadPart || stats.cbSize.QuadPart > 2024 * 2024)
            break;
        dwSize = (DWORD)stats.cbSize.QuadPart;

        output->resize(dwSize);

        ULONG readSize = 0;
        hr = pIStream->Read(output->data(), dwSize, &readSize);
        ASSERT(readSize == dwSize);
        ASSERT(S_OK == hr);
        
        ok = S_OK == hr;
    } while (false);

    if (hMem && INVALID_HANDLE_VALUE == hMem) {
        ::GlobalUnlock(hMem);
        ::GlobalFree(hMem);
    }

    if (encoder)
        delete encoder;

    if (pIStream)
        pIStream->Release();

    if (imageBitmap)
        delete imageBitmap;

    if (encodedData)
        encodedData->unref();

    return ok;    
}

bool GDIPlusImageEncoder::encode(const ImageDataBuffer& imageData, EncoderType type, Vector<unsigned char>* output)
{
    return false;
}

} // namespace blink
