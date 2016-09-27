/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef Canvas2DLayerBridge_h
#define Canvas2DLayerBridge_h

#include "platform/PlatformExport.h"
#include "platform/geometry/IntSize.h"
#include "platform/graphics/ImageBufferSurface.h"
#include "public/platform/WebExternalTextureLayer.h"
#include "public/platform/WebExternalTextureLayerClient.h"
#include "public/platform/WebExternalTextureMailbox.h"
#include "third_party/khronos/GLES2/gl2.h"
#include "third_party/skia/include/core/SkImage.h"
#include "third_party/skia/include/utils/SkDeferredCanvas.h"
#include "wtf/Deque.h"
#include "wtf/DoublyLinkedList.h"
#include "wtf/PassOwnPtr.h"
#include "wtf/RefCounted.h"
#include "wtf/RefPtr.h"

namespace blink {

class Canvas2DLayerBridgeTest;
class ImageBuffer;
class WebGraphicsContext3D;
class WebGraphicsContext3DProvider;

class PLATFORM_EXPORT Canvas2DLayerBridge : public WebExternalTextureLayerClient, public SkDeferredCanvas::NotificationClient, public DoublyLinkedListNode<Canvas2DLayerBridge>, public RefCounted<Canvas2DLayerBridge> {
    WTF_MAKE_NONCOPYABLE(Canvas2DLayerBridge);
public:
    static PassRefPtr<Canvas2DLayerBridge> create(const IntSize&, OpacityMode, int msaaSampleCount);

    ~Canvas2DLayerBridge() override;

    // WebExternalTextureLayerClient implementation.
    bool prepareMailbox(WebExternalTextureMailbox*, WebExternalBitmap*) override;
    void mailboxReleased(const WebExternalTextureMailbox&, bool lostResource) override;

    // SkDeferredCanvas::NotificationClient implementation
    void prepareForDraw() override;
    void storageAllocatedForRecordingChanged(size_t) override;
    void flushedDrawCommands() override;
    void skippedPendingDrawCommands() override;

    // ImageBufferSurface implementation
    void finalizeFrame(const FloatRect &dirtyRect);
    void willAccessPixels();
    SkCanvas* canvas() const { return m_canvas.get(); }
    bool checkSurfaceValid();
    bool restoreSurface();
    WebLayer* layer() const;
    bool isAccelerated() const { return true; }
    void setFilterQuality(SkFilterQuality);
    void setIsHidden(bool);
    void setImageBuffer(ImageBuffer* imageBuffer) { m_imageBuffer = imageBuffer; }
    void didDraw();

    // Methods used by Canvas2DLayerManager
    virtual size_t freeMemoryIfPossible(size_t); // virtual for mocking
    virtual void flush(); // virtual for mocking
    virtual size_t storageAllocatedForRecording(); // virtual for faking
    size_t bytesAllocated() const { return m_bytesAllocated; }
    void limitPendingFrames();
    void freeTransientResources();
    bool hasTransientResources() const;
    bool isHidden() { return m_isHidden; }

    void beginDestruction();

    PassRefPtr<SkImage> newImageSnapshot();

protected:
    Canvas2DLayerBridge(PassOwnPtr<WebGraphicsContext3DProvider>, PassOwnPtr<SkDeferredCanvas>, PassRefPtr<SkSurface>, int, OpacityMode);
    void setRateLimitingEnabled(bool);
    WebGraphicsContext3D* context();

    OwnPtr<SkDeferredCanvas> m_canvas;
    RefPtr<SkSurface> m_surface;
    OwnPtr<WebExternalTextureLayer> m_layer;
    OwnPtr<WebGraphicsContext3DProvider> m_contextProvider;
    ImageBuffer* m_imageBuffer;
    int m_msaaSampleCount;
    size_t m_bytesAllocated;
    bool m_didRecordDrawCommand;
    bool m_isSurfaceValid;
    int m_framesPending;
    int m_framesSinceMailboxRelease;
    bool m_destructionInProgress;
    bool m_rateLimitingEnabled;
    SkFilterQuality m_filterQuality;
    bool m_isHidden;

    friend class WTF::DoublyLinkedListNode<Canvas2DLayerBridge>;
    friend class Canvas2DLayerBridgeTest;
    Canvas2DLayerBridge* m_next;
    Canvas2DLayerBridge* m_prev;

    struct MailboxInfo {
        WebExternalTextureMailbox m_mailbox;
        RefPtr<SkImage> m_image;
        RefPtr<Canvas2DLayerBridge> m_parentLayerBridge;

        MailboxInfo(const MailboxInfo&);
        MailboxInfo() {}
    };

    uint32_t m_lastImageId;

    enum {
        // We should normally not have more that two active mailboxes at a time,
        // but sometime we may have three due to the async nature of mailbox handling.
        MaxActiveMailboxes = 3,
    };

    Deque<MailboxInfo, MaxActiveMailboxes> m_mailboxes;
    GLenum m_lastFilter;
    OpacityMode m_opacityMode;
};

} // namespace blink

#endif
