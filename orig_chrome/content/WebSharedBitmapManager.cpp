
#include "orig_chrome/content/WebSharedBitmapManager.h"
#include "ui/gfx/geometry/size.h"

namespace content {

WebSharedBitmapManager::WebSharedBitmapManager()
{
}

WebSharedBitmapManager::~WebSharedBitmapManager()
{
}

class SharedMemoryBitmap : public cc::SharedBitmap {
public:
    SharedMemoryBitmap(uint8* pixels, const cc::SharedBitmapId& id)
        : cc::SharedBitmap(pixels, id)
    {
    }

    ~SharedMemoryBitmap()
    {
        free(pixels());
    }
};

scoped_ptr<cc::SharedBitmap> WebSharedBitmapManager::AllocateSharedBitmap(const gfx::Size& size)
{
    size_t memorySize;
    if (!cc::SharedBitmap::SizeInBytes(size, &memorySize))
        return scoped_ptr<cc::SharedBitmap>();

    uint8* memory = static_cast<uint8*>(malloc(memorySize));
    cc::SharedBitmapId id = cc::SharedBitmap::GenerateId();
    return scoped_ptr<cc::SharedBitmap>(new SharedMemoryBitmap(memory, id));
}

scoped_ptr<cc::SharedBitmap> WebSharedBitmapManager::GetSharedBitmapFromId(const gfx::Size& size, const cc::SharedBitmapId&)
{
    DebugBreak();
    return scoped_ptr<cc::SharedBitmap>();
}

} // content
