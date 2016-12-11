
#ifndef GDIPlusReader_h
#define GDIPlusReader_h

#include <stdint.h>
#include "platform/image-decoders/ImageDecoder.h"

namespace Gdiplus {
class Bitmap;
}

namespace blink {

class PLATFORM_EXPORT GDIPlusReader {
public:
    GDIPlusReader(ImageDecoder* parent);
    ~GDIPlusReader();

    void setBuffer(ImageFrame* buffer) { m_buffer = buffer; }
    void setData(SharedBuffer* data) { m_data = data; }

    bool decode(bool onlySize);

    void setForceBitMaskAlpha() {}

private:
    void release();
    void decodeToBitmapByGDIPlus();

    // The decoder that owns us.
    ImageDecoder* m_parent;

    // The destination for the pixel data.
    ImageFrame* m_buffer;

    // The file to decode.
    RefPtr<SharedBuffer> m_data;

    Gdiplus::Bitmap* m_gdipBitmap;
    IStream* m_istream;
    HGLOBAL m_memHandle;
};

} // namespace blink

#endif