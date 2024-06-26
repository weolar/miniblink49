
#ifndef electron_common_IconUtil_h
#define electron_common_IconUtil_h

#include "node/nodeblink.h"
#include "node/src/node.h"
#include "common/api/ApiNativeImage.h"

namespace IconUtil {

void InitializeBitmapHeader(BITMAPV5HEADER* header, int width, int height)
{
    DCHECK(header);
    memset(header, 0, sizeof(BITMAPV5HEADER));
    header->bV5Size = sizeof(BITMAPV5HEADER);

    // Note that icons are created using top-down DIBs so we must negate the
    // value used for the icon's height.
    header->bV5Width = width;
    header->bV5Height = -height;
    header->bV5Planes = 1;
    header->bV5Compression = BI_RGB;

    // Initializing the bitmap format to 32 bit ARGB.
    header->bV5BitCount = 32;
    header->bV5RedMask = 0x00FF0000;
    header->bV5GreenMask = 0x0000FF00;
    header->bV5BlueMask = 0x000000FF;
    header->bV5AlphaMask = 0xFF000000;

    // Use the system color space.  The default value is LCS_CALIBRATED_RGB, which
    // causes us to crash if we don't specify the approprite gammas, etc.  See
    // <http://msdn.microsoft.com/en-us/library/ms536531(VS.85).aspx> and
    // <http://b/1283121>.
    header->bV5CSType = LCS_WINDOWS_COLOR_SPACE;

    // Use a valid value for bV5Intent as 0 is not a valid one.
    // <http://msdn.microsoft.com/en-us/library/dd183381(VS.85).aspx>
    header->bV5Intent = LCS_GM_IMAGES;
}

bool PixelsHaveAlpha(const uint32_t* pixels, size_t num_pixels)
{
    for (const uint32_t* end = pixels + num_pixels; pixels != end; ++pixels) {
        if ((*pixels & 0xff000000) != 0)
            return true;
    }

    return false;
}

struct ScopedICONINFO : ICONINFO {
    ScopedICONINFO()
    {
        hbmColor = NULL;
        hbmMask = NULL;
    }

    ~ScopedICONINFO()
    {
        if (hbmColor)
            ::DeleteObject(hbmColor);
        if (hbmMask)
            ::DeleteObject(hbmMask);
    }
};

// G:\electron_5_0_3\src\ui\gfx\icon_util.cc
v8::Local<v8::Object> CreateSkBitmapFromHICONHelper(v8::Isolate* isolate, HICON icon, const SIZE& s)
{
//     DCHECK(icon);
//     DCHECK(!s.IsEmpty());

    // Now we should create a DIB so that we can use ::DrawIconEx in order to obtain the icon's image.
    BITMAPV5HEADER h;
    InitializeBitmapHeader(&h, s.cx, s.cy);
    HDC hdc = ::GetDC(NULL);
    uint32_t* bits;
    HBITMAP dib = ::CreateDIBSection(hdc, reinterpret_cast<BITMAPINFO*>(&h), DIB_RGB_COLORS, reinterpret_cast<void**>(&bits), NULL, 0);
    //DCHECK(dib);
    HDC dib_dc = CreateCompatibleDC(hdc);
    ::ReleaseDC(NULL, hdc);
    //DCHECK(dib_dc);
    HGDIOBJ old_obj = ::SelectObject(dib_dc, dib);

    // Windows icons are defined using two different masks. The XOR mask, which
    // represents the icon image and an AND mask which is a monochrome bitmap
    // which indicates the transparency of each pixel.
    //
    // To make things more complex, the icon image itself can be an ARGB bitmap
    // and therefore contain an alpha channel which specifies the transparency
    // for each pixel. Unfortunately, there is no easy way to determine whether
    // or not a bitmap has an alpha channel and therefore constructing the bitmap
    // for the icon is nothing but straightforward.
    //
    // The idea is to read the AND mask but use it only if we know for sure that
    // the icon image does not have an alpha channel. The only way to tell if the
    // bitmap has an alpha channel is by looking through the pixels and checking
    // whether there are non-zero alpha bytes.
    //
    // We start by drawing the AND mask into our DIB.
    size_t num_pixels = s.cx * s.cy;
    memset(bits, 0, num_pixels * 4);
    ::DrawIconEx(dib_dc, 0, 0, icon, s.cx, s.cy, 0, NULL, DI_MASK);

    // Capture boolean opacity. We may not use it if we find out the bitmap has
    // an alpha channel.
    std::unique_ptr<bool[]> opaque(new bool[num_pixels]);
    for (size_t i = 0; i < num_pixels; ++i)
        opaque[i] = !bits[i];

    // Then draw the image itself which is really the XOR mask.
    memset(bits, 0, num_pixels * 4);
    ::DrawIconEx(dib_dc, 0, 0, icon, s.cx, s.cy, 0, NULL, DI_NORMAL);

    // Finding out whether the bitmap has an alpha channel.
    bool bitmap_has_alpha_channel = PixelsHaveAlpha(static_cast<const uint32_t*>(bits), num_pixels);

    // If the bitmap does not have an alpha channel, we need to build it using
    // the previously captured AND mask. Otherwise, we are done.
    if (!bitmap_has_alpha_channel) {
        uint32_t* p = static_cast<uint32_t*>(bits);
        for (size_t i = 0; i < num_pixels; ++p, ++i) {
            //DCHECK_EQ((*p & 0xff000000), 0u);
            if (opaque[i])
                *p |= 0xff000000;
            else
                *p &= 0x00ffffff;
        }
    }

    v8::Local<v8::Object> ret = atom::NativeImage::createFromBITMAPINFO(isolate, (BITMAPINFO*)(&h), bits);

    ::SelectObject(dib_dc, old_obj);
    ::DeleteObject(dib);
    ::DeleteDC(dib_dc);

    return ret;
}

v8::Local<v8::Object> CreateSkBitmapFromHICON(v8::Isolate* isolate, HICON icon)
{
    // We start with validating parameters.
    if (!icon)
        return v8::Local<v8::Object>();

    ScopedICONINFO icon_info;
    BITMAP bitmap_info = { 0 };

    if (!::GetIconInfo(icon, &icon_info))
        return v8::Local<v8::Object>();

    if (!::GetObject(icon_info.hbmMask, sizeof(bitmap_info), &bitmap_info))
        return v8::Local<v8::Object>();

    // For non-color cursors, the mask contains both an AND and an XOR mask and
    // the height includes both. Thus, the mask width is the same as image width,
    // but we need to divide mask height by 2 to get the image height.
    const int height = bitmap_info.bmHeight / (icon_info.hbmColor ? 1 : 2);
    SIZE icon_size = { bitmap_info.bmWidth, height };
    return CreateSkBitmapFromHICONHelper(isolate, icon, icon_size);
}

}

#endif // electron_common_IconUtil_h