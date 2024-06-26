#ifndef WebSharedBitmapManager_h
#define WebSharedBitmapManager_h

#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "base/memory/shared_memory.h"
#include "cc/resources/shared_bitmap_manager.h"

namespace content {

class WebSharedBitmapManager : public cc::SharedBitmapManager {
public:
    WebSharedBitmapManager();
    virtual ~WebSharedBitmapManager();

    virtual scoped_ptr<cc::SharedBitmap> AllocateSharedBitmap(const gfx::Size& size) OVERRIDE;
    virtual scoped_ptr<cc::SharedBitmap> GetSharedBitmapFromId(const gfx::Size&, const cc::SharedBitmapId&) OVERRIDE;
    //virtual scoped_ptr<cc::SharedBitmap> GetBitmapForSharedMemory(base::SharedMemory* mem) OVERRIDE;

private:
    DISALLOW_COPY_AND_ASSIGN(WebSharedBitmapManager);
};

} // content

#endif // WebSharedBitmapManager_h