/*
* Copyright (C) 2013 Google Inc.  All rights reserved.
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

#ifndef RasterResouce_h
#define RasterResouce_h

#include "third_party/WebKit/Source/platform/geometry/FloatRect.h"
#include "third_party/WebKit/Source/platform/geometry/IntSize.h"
#include "cc/raster/SkBitmapRefWrap.h"
#include "cc/raster/RasterTaskClient.h"

class SkBitmap;
class SkPicture;

namespace cc {

class RasterResouce {
public:
    RasterResouce(SkBitmapRefWrap* bitmapWrap, RecursiveMutex* mutex, SkPicture* picture, blink::FloatRect layerRect, RasterTaskClient* client)
        : m_bitmapWrap(bitmapWrap)
        , m_mutex(mutex)
        , m_picture(picture)
        , m_layerRect(layerRect)
        , m_client(client)
    {
    }

    ~RasterResouce()
    {
        delete m_picture;
        m_bitmapWrap->deref();
        m_client->rasterTaskFinish();
    }

    bool willBeDelete()
    {
        return m_client->willBeDelete();
    }

    SkBitmap* bitmap() { return m_bitmapWrap->get(); }
    RecursiveMutex* mutex(){ return m_mutex; }
    SkPicture* picture(){ return m_picture; }
    blink::FloatRect layerRect() { return m_layerRect; }
    blink::IntSize bitmapSize() { return blink::IntSize(m_bitmapWrap->get()->width(), m_bitmapWrap->get()->height()); }

private:
    cc::SkBitmapRefWrap* m_bitmapWrap;
    RecursiveMutex* m_mutex;
    SkPicture* m_picture;
    blink::FloatRect m_layerRect;
    RasterTaskClient* m_client;
};

} // cc

#endif // RasterResouce_h