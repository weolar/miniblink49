
#include "config.h"
#include <windows.h>

#include "platform/image-decoders/GDIPlus/GDIPlusReader.h"

#undef min
#undef max
using std::max;
using std::min;
#include <gdiplus.h>

namespace blink {

GDIPlusReader::GDIPlusReader(ImageDecoder* parent, Gdiplus::Bitmap* gdipBitmap)
    : m_parent(parent)
    , m_buffer(0)
    , m_gdipBitmap(gdipBitmap)
{

}

GDIPlusReader::~GDIPlusReader()
{
    delete m_gdipBitmap;
    m_gdipBitmap = nullptr;
}

static void fillbitmap(Gdiplus::Bitmap* gdipBitmap, ImageFrame* buffer)
{
    UINT w = gdipBitmap->GetWidth();
    UINT h = gdipBitmap->GetHeight();

    Gdiplus::Rect rect(0, 0, w, h);
    Gdiplus::BitmapData lockedBitmapData;
    gdipBitmap->LockBits(
#if USING_VC6RT != 1
        &
#endif
        rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &lockedBitmapData);

    unsigned int *pData = reinterpret_cast<unsigned int *>(lockedBitmapData.Scan0);
    int stride = lockedBitmapData.Stride;

    UINT hasAlpha = 0xff000000;
    for (UINT x = 0; x < w; ++x) {
        for (UINT y = 0; y < h; ++y) {
            UINT color = pData[y * stride / 4 + x]; // color= 0xAARRGGBB
            hasAlpha &= color;
            UINT red = GetRValue(color);
            UINT green = GetGValue(color);
            UINT blue = GetBValue(color);
            UINT alpha = (LOBYTE((color) >> 24));
            buffer->setRGBA(x, y, red, green, blue, alpha);
        }
    }

    buffer->setHasAlpha(!hasAlpha);
    
    gdipBitmap->UnlockBits(&lockedBitmapData);
}

bool GDIPlusReader::decodeBMP(bool onlySize)
{
    // Set our size.
    if (!m_parent->setSize(m_gdipBitmap->GetWidth(), m_gdipBitmap->GetHeight()))
        return false;

    if (onlySize)
        return true;

    // Initialize the framebuffer if needed.
    ASSERT(m_buffer);  // Parent should set this before asking us to decode!
    if (m_buffer->status() == ImageFrame::FrameEmpty) {
        if (!m_buffer->setSize(m_parent->size().width(), m_parent->size().height()))
            return m_parent->setFailed(); // Unable to allocate.
        m_buffer->setStatus(ImageFrame::FramePartial);
        // setSize() calls eraseARGB(), which resets the alpha flag, so we force
        // it back to false here.  We'll set it true below in all cases where
        // these 0s could actually show through.
        m_buffer->setHasAlpha(false);

        // For BMPs, the frame always fills the entire image.
        m_buffer->setOriginalFrameRect(IntRect(IntPoint(), m_parent->size()));
    }

    fillbitmap(m_gdipBitmap, m_buffer);
    m_buffer->setStatus(ImageFrame::FrameComplete);

    return true;
}

} // blink