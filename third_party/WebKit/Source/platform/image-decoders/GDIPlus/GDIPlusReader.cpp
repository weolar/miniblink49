
#include "config.h"
#include <windows.h>

#include "platform/image-decoders/GDIPlus/GDIPlusReader.h"
#include "platform/graphics/GDIPlusInit.h"

#undef min
#undef max
using std::max;
using std::min;
#include <gdiplus.h>

namespace blink {

GDIPlusReader::GDIPlusReader(ImageDecoder* parent)
    : m_parent(parent)
    , m_buffer(0)
    , m_gdipBitmap(nullptr)
    , m_istream(nullptr)
    , m_memHandle(nullptr)
{

}

GDIPlusReader::~GDIPlusReader()
{
    release();
}

void GDIPlusReader::release()
{
    if (m_gdipBitmap)
        delete m_gdipBitmap;
    m_gdipBitmap = nullptr;

    if (m_memHandle)
        ::GlobalFree(m_memHandle);
    m_memHandle = nullptr;

    if (m_istream)
        m_istream->Release();
    m_istream = nullptr;
}

static void fillbitmap(Gdiplus::Bitmap* gdipBitmap, ImageFrame* buffer)
{
    UINT w = gdipBitmap->GetWidth();
    UINT h = gdipBitmap->GetHeight();

    Gdiplus::Rect rect(0, 0, w, h);
    Gdiplus::BitmapData lockedBitmapData;
    gdipBitmap->LockBits(
#if _DEBUG
       &
#elif NDEBUG

#else USING_VC6RT
	
#endif
        rect, Gdiplus::ImageLockModeRead, PixelFormat32bppARGB, &lockedBitmapData);

    unsigned int *pData = reinterpret_cast<unsigned int *>(lockedBitmapData.Scan0);
    if (!pData)
        return;
    
    int stride = lockedBitmapData.Stride;

    UINT hasAlpha = 0xff000000;
    for (UINT x = 0; x < w; ++x) {
        for (UINT y = 0; y < h; ++y) {
            UINT color = pData[y * stride / 4 + x]; // color= 0xAARRGGBB
            hasAlpha &= color;
            UINT blue = GetRValue(color);
            UINT green = GetGValue(color);
            UINT red = GetBValue(color);
            UINT alpha = (LOBYTE((color) >> 24));
            buffer->setRGBA(x, y, red, green, blue, alpha);
        }
    }

    buffer->setHasAlpha(!hasAlpha);
    
    gdipBitmap->UnlockBits(&lockedBitmapData);
}

void GDIPlusReader::decodeToBitmapByGDIPlus()
{
    initGDIPlusClsids();

    release();

    m_memHandle = ::GlobalAlloc(GMEM_FIXED, m_data->size());
    BYTE* pMem = (BYTE*)::GlobalLock(m_memHandle);
    memcpy(pMem, m_data->data(), m_data->size());
    ::GlobalUnlock(m_memHandle);

    ::CreateStreamOnHGlobal(m_memHandle, FALSE, &m_istream);

    m_gdipBitmap = Gdiplus::Bitmap::FromStream(m_istream);
}

bool GDIPlusReader::decode(bool onlySize)
{
    decodeToBitmapByGDIPlus();
    if (!m_gdipBitmap)
        return false;

    int width = m_gdipBitmap->GetWidth();
    int height = m_gdipBitmap->GetHeight();

    // Set our size.
    if (0 == width || 0 == height)
        return false;
    
    if (!m_parent->setSize(width, height))
        return false;

    if (onlySize)
        return true;

    if (!m_buffer)
        return false;

    if (0 == width || 0 == height) {
        m_buffer->setStatus(ImageFrame::FramePartial);
        return false;
    }
    
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
