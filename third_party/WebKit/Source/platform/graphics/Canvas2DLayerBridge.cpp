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

#include "config.h"

#include "platform/graphics/Canvas2DLayerBridge.h"

#include "GrContext.h"
#include "SkDevice.h"
#include "SkSurface.h"

#include "platform/TraceEvent.h"
#include "platform/graphics/Canvas2DLayerManager.h"
#include "platform/graphics/GraphicsLayer.h"
#include "platform/graphics/ImageBuffer.h"
#include "public/platform/Platform.h"
#include "public/platform/WebCompositorSupport.h"
#include "public/platform/WebGraphicsContext3D.h"
#include "public/platform/WebGraphicsContext3DProvider.h"
#include "third_party/skia/include/core/SkData.h"
#include "third_party/skia/include/core/SkSurface.h"
#include "wtf/RefCountedLeakCounter.h"

namespace {
enum {
    InvalidMailboxIndex = -1,
};

DEFINE_DEBUG_ONLY_GLOBAL(WTF::RefCountedLeakCounter, canvas2DLayerBridgeInstanceCounter, ("Canvas2DLayerBridge"));
}

namespace blink {

static PassRefPtr<SkSurface> createSkSurface(GrContext* gr, const IntSize& size, int msaaSampleCount, OpacityMode opacityMode)
{
    if (!gr)
        return nullptr;
    gr->resetContext();
    SkImageInfo info = SkImageInfo::MakeN32Premul(size.width(), size.height());
    SkSurfaceProps disableLCDProps(0, kUnknown_SkPixelGeometry);
    return adoptRef(SkSurface::NewRenderTarget(gr, SkSurface::kNo_Budgeted, info,  msaaSampleCount,
        Opaque == opacityMode ? 0 : &disableLCDProps));
}

PassRefPtr<Canvas2DLayerBridge> Canvas2DLayerBridge::create(const IntSize& size, OpacityMode opacityMode, int msaaSampleCount)
{
    TRACE_EVENT_INSTANT0("test_gpu", "Canvas2DLayerBridgeCreation", TRACE_EVENT_SCOPE_GLOBAL);
    OwnPtr<WebGraphicsContext3DProvider> contextProvider = adoptPtr(Platform::current()->createSharedOffscreenGraphicsContext3DProvider());
    if (!contextProvider)
        return nullptr;
    RefPtr<SkSurface> surface(createSkSurface(contextProvider->grContext(), size, msaaSampleCount, opacityMode));
    if (!surface)
        return nullptr;
    RefPtr<Canvas2DLayerBridge> layerBridge;
    OwnPtr<SkDeferredCanvas> canvas = adoptPtr(SkDeferredCanvas::Create(surface.get()));
    layerBridge = adoptRef(new Canvas2DLayerBridge(contextProvider.release(), canvas.release(), surface.release(), msaaSampleCount, opacityMode));
    return layerBridge.release();
}

Canvas2DLayerBridge::Canvas2DLayerBridge(PassOwnPtr<WebGraphicsContext3DProvider> contextProvider, PassOwnPtr<SkDeferredCanvas> canvas, PassRefPtr<SkSurface> surface, int msaaSampleCount, OpacityMode opacityMode)
    : m_canvas(canvas)
    , m_surface(surface)
    , m_contextProvider(contextProvider)
    , m_imageBuffer(0)
    , m_msaaSampleCount(msaaSampleCount)
    , m_bytesAllocated(0)
    , m_didRecordDrawCommand(false)
    , m_isSurfaceValid(true)
    , m_framesPending(0)
    , m_destructionInProgress(false)
    , m_rateLimitingEnabled(false)
    , m_filterQuality(kLow_SkFilterQuality)
    , m_isHidden(false)
    , m_next(0)
    , m_prev(0)
    , m_lastImageId(0)
    , m_lastFilter(GL_LINEAR)
    , m_opacityMode(opacityMode)
{
    ASSERT(m_canvas);
    ASSERT(m_surface);
    ASSERT(m_contextProvider);
    // Used by browser tests to detect the use of a Canvas2DLayerBridge.
    TRACE_EVENT_INSTANT0("test_gpu", "Canvas2DLayerBridgeCreation", TRACE_EVENT_SCOPE_GLOBAL);
    m_layer = adoptPtr(Platform::current()->compositorSupport()->createExternalTextureLayer(this));
    m_layer->setOpaque(opacityMode == Opaque);
    m_layer->setBlendBackgroundColor(opacityMode != Opaque);
    GraphicsLayer::registerContentsLayer(m_layer->layer());
    m_layer->setRateLimitContext(m_rateLimitingEnabled);
    m_layer->setNearestNeighbor(m_filterQuality == kNone_SkFilterQuality);
    m_canvas->setNotificationClient(this);
#ifndef NDEBUG
    canvas2DLayerBridgeInstanceCounter.increment();
#endif
}

Canvas2DLayerBridge::~Canvas2DLayerBridge()
{
    ASSERT(m_destructionInProgress);
    // TODO(junov): This can go back to a regular ASSERT once crbug.com/466793 is resolved.
    RELEASE_ASSERT(!Canvas2DLayerManager::get().isInList(this));
    if (m_canvas)
        m_canvas->setNotificationClient(nullptr);
    m_layer.clear();
    ASSERT(m_mailboxes.size() == 0);
#ifndef NDEBUG
    canvas2DLayerBridgeInstanceCounter.decrement();
#endif
}

void Canvas2DLayerBridge::beginDestruction()
{
    ASSERT(!m_destructionInProgress);
    setRateLimitingEnabled(false);
    m_canvas->silentFlush();
    m_imageBuffer = 0;
    freeTransientResources();
    setIsHidden(true);
    m_destructionInProgress = true;
    GraphicsLayer::unregisterContentsLayer(m_layer->layer());
    m_canvas->setNotificationClient(0);
    m_surface.clear();
    m_canvas.clear();
    m_layer->clearTexture();
    // Orphaning the layer is required to trigger the recration of a new layer
    // in the case where destruction is caused by a canvas resize. Test:
    // virtual/gpu/fast/canvas/canvas-resize-after-paint-without-layout.html
    m_layer->layer()->removeFromParent();
    // To anyone who ever hits this assert: Please update crbug.com/344666
    // with repro steps.
    ASSERT(!m_bytesAllocated);
}

void Canvas2DLayerBridge::setFilterQuality(SkFilterQuality filterQuality)
{
    ASSERT(!m_destructionInProgress);
    m_filterQuality = filterQuality;
    m_layer->setNearestNeighbor(m_filterQuality == kNone_SkFilterQuality);
}

void Canvas2DLayerBridge::setIsHidden(bool hidden)
{
    ASSERT(!m_destructionInProgress);
    bool newHiddenValue = hidden || m_destructionInProgress;
    if (m_isHidden == newHiddenValue)
        return;

    m_isHidden = newHiddenValue;
    if (isHidden()) {
        freeTransientResources();
    }
}

void Canvas2DLayerBridge::willAccessPixels()
{
    // A readback operation may alter the texture parameters, which may affect
    // the compositor's behavior. Therefore, we must trigger copy-on-write
    // even though we are not technically writing to the texture, only to its
    // parameters.
    if (m_isSurfaceValid)
        m_surface->notifyContentWillChange(SkSurface::kRetain_ContentChangeMode);
}

void Canvas2DLayerBridge::freeTransientResources()
{
    ASSERT(!m_destructionInProgress);
    if (!m_isSurfaceValid)
        return;
    flush();
    freeMemoryIfPossible(bytesAllocated());
    ASSERT(!hasTransientResources());
}

bool Canvas2DLayerBridge::hasTransientResources() const
{
    return !m_destructionInProgress && bytesAllocated();
}

void Canvas2DLayerBridge::limitPendingFrames()
{
    ASSERT(!m_destructionInProgress);
    if (isHidden()) {
        freeTransientResources();
        return;
    }
    if (m_didRecordDrawCommand) {
        m_framesPending++;
        m_didRecordDrawCommand = false;
        if (m_framesPending > 1) {
            // Turn on the rate limiter if this layer tends to accumulate a
            // non-discardable multi-frame backlog of draw commands.
            setRateLimitingEnabled(true);
        }
        if (m_rateLimitingEnabled) {
            flush();
        }
    }
}

void Canvas2DLayerBridge::prepareForDraw()
{
    ASSERT(!m_destructionInProgress);
    ASSERT(m_layer);
    if (!checkSurfaceValid()) {
        if (m_canvas) {
            // drop pending commands because there is no surface to draw to
            m_canvas->silentFlush();
        }
        return;
    }
}

void Canvas2DLayerBridge::storageAllocatedForRecordingChanged(size_t bytesAllocated)
{
    ASSERT(!m_destructionInProgress);
    intptr_t delta = (intptr_t)bytesAllocated - (intptr_t)m_bytesAllocated;
    m_bytesAllocated = bytesAllocated;
    Canvas2DLayerManager::get().layerTransientResourceAllocationChanged(this, delta);
}

size_t Canvas2DLayerBridge::storageAllocatedForRecording()
{
    return m_canvas->storageAllocatedForRecording();
}

void Canvas2DLayerBridge::flushedDrawCommands()
{
    ASSERT(!m_destructionInProgress);
    storageAllocatedForRecordingChanged(storageAllocatedForRecording());
    m_framesPending = 0;
}

void Canvas2DLayerBridge::skippedPendingDrawCommands()
{
    ASSERT(!m_destructionInProgress);
    // Stop triggering the rate limiter if SkDeferredCanvas is detecting
    // and optimizing overdraw.
    setRateLimitingEnabled(false);
    flushedDrawCommands();
}

void Canvas2DLayerBridge::setRateLimitingEnabled(bool enabled)
{
    ASSERT(!m_destructionInProgress);
    if (m_rateLimitingEnabled != enabled) {
        m_rateLimitingEnabled = enabled;
        m_layer->setRateLimitContext(m_rateLimitingEnabled);
    }
}

size_t Canvas2DLayerBridge::freeMemoryIfPossible(size_t bytesToFree)
{
    ASSERT(!m_destructionInProgress);
    size_t bytesFreed = m_canvas->freeMemoryIfPossible(bytesToFree);
    m_bytesAllocated -= bytesFreed;
    if (bytesFreed)
        Canvas2DLayerManager::get().layerTransientResourceAllocationChanged(this, -((intptr_t)bytesFreed));
    return bytesFreed;
}

void Canvas2DLayerBridge::flush()
{
    ASSERT(!m_destructionInProgress);
    if (m_canvas->hasPendingCommands()) {
        TRACE_EVENT0("cc", "Canvas2DLayerBridge::flush");
        m_canvas->flush();
    }
}

WebGraphicsContext3D* Canvas2DLayerBridge::context()
{
    // Check on m_layer is necessary because context() may be called during
    // the destruction of m_layer
    if (m_layer && !m_destructionInProgress)
        checkSurfaceValid(); // To ensure rate limiter is disabled if context is lost.
    return m_contextProvider ? m_contextProvider->context3d() : 0;
}

bool Canvas2DLayerBridge::checkSurfaceValid()
{
    ASSERT(!m_destructionInProgress);
    if (m_destructionInProgress || !m_isSurfaceValid)
        return false;
    if (m_contextProvider->context3d()->isContextLost()) {
        m_isSurfaceValid = false;
        m_surface.clear();
        for (auto mailboxInfo = m_mailboxes.begin(); mailboxInfo != m_mailboxes.end(); ++mailboxInfo) {
            if (mailboxInfo->m_image)
                mailboxInfo->m_image.clear();
        }
        if (m_imageBuffer)
            m_imageBuffer->notifySurfaceInvalid();
        setRateLimitingEnabled(false);
    }
    return m_isSurfaceValid;
}

bool Canvas2DLayerBridge::restoreSurface()
{
    ASSERT(!m_destructionInProgress);
    if (m_destructionInProgress)
        return false;
    ASSERT(m_layer && !m_isSurfaceValid);

    WebGraphicsContext3D* sharedContext = 0;
    m_layer->clearTexture();
    m_contextProvider = adoptPtr(Platform::current()->createSharedOffscreenGraphicsContext3DProvider());
    if (m_contextProvider)
        sharedContext = m_contextProvider->context3d();

    if (sharedContext && !sharedContext->isContextLost()) {
        SkISize skSize = m_canvas->getBaseLayerSize();
        IntSize size(skSize.width(), skSize.height());
        RefPtr<SkSurface> surface(createSkSurface(m_contextProvider->grContext(), size, m_msaaSampleCount, m_opacityMode));
        if (surface.get()) {
            m_surface = surface.release();
            m_canvas->setSurface(m_surface.get());
            m_isSurfaceValid = true;
            // FIXME: draw sad canvas picture into new buffer crbug.com/243842
        }
    }

    return m_isSurfaceValid;
}

bool Canvas2DLayerBridge::prepareMailbox(WebExternalTextureMailbox* outMailbox, WebExternalBitmap* bitmap)
{
    if (m_destructionInProgress) {
        // It can be hit in the following sequence.
        // 1. Canvas draws something.
        // 2. The compositor begins the frame.
        // 3. Javascript makes a context be lost.
        // 4. Here.
        return false;
    }
    if (bitmap) {
        // Using accelerated 2d canvas with software renderer, which
        // should only happen in tests that use fake graphics contexts
        // or in Android WebView in software mode. In this case, we do
        // not care about producing any results for this canvas.
        m_canvas->silentFlush();
        m_lastImageId = 0;
        return false;
    }
    if (!checkSurfaceValid())
        return false;

    WebGraphicsContext3D* webContext = context();

    // Release to skia textures that were previouosly released by the
    // compositor. We do this before acquiring the next snapshot in
    // order to cap maximum gpu memory consumption.
    flush();

    RefPtr<SkImage> image = adoptRef(m_canvas->newImageSnapshot());

    // Early exit if canvas was not drawn to since last prepareMailbox
    GLenum filter = m_filterQuality == kNone_SkFilterQuality ? GL_NEAREST : GL_LINEAR;
    if (image->uniqueID() == m_lastImageId && filter == m_lastFilter)
        return false;
    m_lastImageId = image->uniqueID();
    m_lastFilter = filter;

    {
        MailboxInfo tmp;
        tmp.m_image = image;
        tmp.m_parentLayerBridge = this;
        m_mailboxes.prepend(tmp);
    }
    MailboxInfo& mailboxInfo = m_mailboxes.first();

    mailboxInfo.m_mailbox.nearestNeighbor = filter == GL_NEAREST;

    GrContext* grContext = m_contextProvider->grContext();
    if (!grContext)
        return true; // for testing: skip gl stuff when using a mock graphics context.

    ASSERT(image->getTexture());

    // Because of texture sharing with the compositor, we must invalidate
    // the state cached in skia so that the deferred copy on write
    // in SkSurface_Gpu does not make any false assumptions.
    mailboxInfo.m_image->getTexture()->textureParamsModified();

    webContext->bindTexture(GL_TEXTURE_2D, mailboxInfo.m_image->getTexture()->getTextureHandle());
    webContext->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter);
    webContext->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filter);
    webContext->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    webContext->texParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Re-use the texture's existing mailbox, if there is one.
    if (image->getTexture()->getCustomData()) {
        ASSERT(image->getTexture()->getCustomData()->size() == sizeof(mailboxInfo.m_mailbox.name));
        memcpy(&mailboxInfo.m_mailbox.name[0], image->getTexture()->getCustomData()->data(), sizeof(mailboxInfo.m_mailbox.name));
    } else {
        context()->genMailboxCHROMIUM(mailboxInfo.m_mailbox.name);
        RefPtr<SkData> mailboxNameData = adoptRef(SkData::NewWithCopy(&mailboxInfo.m_mailbox.name[0], sizeof(mailboxInfo.m_mailbox.name)));
        image->getTexture()->setCustomData(mailboxNameData.get());
        webContext->produceTextureCHROMIUM(GL_TEXTURE_2D, mailboxInfo.m_mailbox.name);
    }

    if (isHidden()) {
        // With hidden canvases, we release the SkImage immediately because
        // there is no need for animations to be double buffered.
        mailboxInfo.m_image.clear();
    } else {
        webContext->flush();
        mailboxInfo.m_mailbox.syncPoint = webContext->insertSyncPoint();
    }
    webContext->bindTexture(GL_TEXTURE_2D, 0);
    // Because we are changing the texture binding without going through skia,
    // we must dirty the context.
    grContext->resetContext(kTextureBinding_GrGLBackendState);

    *outMailbox = mailboxInfo.m_mailbox;
    return true;
}

void Canvas2DLayerBridge::mailboxReleased(const WebExternalTextureMailbox& mailbox, bool lostResource)
{
    bool contextLost = !m_isSurfaceValid || m_contextProvider->context3d()->isContextLost();
    ASSERT(m_mailboxes.last().m_parentLayerBridge.get() == this);

    // Mailboxes are typically released in FIFO order, so we iterate
    // from the end of m_mailboxes.
    auto releasedMailboxInfo = m_mailboxes.end();
    auto firstMailbox = m_mailboxes.begin();

    while (true) {
        --releasedMailboxInfo;
        if (nameEquals(releasedMailboxInfo->m_mailbox, mailbox)) {
            break;
        }
        if (releasedMailboxInfo == firstMailbox) {
            // Reached last entry without finding a match, should never happen.
            // FIXME: This used to be an ASSERT, and was (temporarily?) changed to a
            // CRASH to facilitate the investigation of crbug.com/443898.
            CRASH();
        }
    }

    if (!contextLost) {
        // Invalidate texture state in case the compositor altered it since the copy-on-write.
        if (releasedMailboxInfo->m_image) {
            if (mailbox.syncPoint) {
                context()->waitSyncPoint(mailbox.syncPoint);
            }
            GrTexture* texture = releasedMailboxInfo->m_image->getTexture();
            if (texture) {
                if (lostResource) {
                    texture->abandon();
                } else {
                    texture->textureParamsModified();
                }
            }
        }
    }

    RefPtr<Canvas2DLayerBridge> selfRef;
    if (m_destructionInProgress) {
        // To avoid memory use after free, take a scoped self-reference
        // to postpone destruction until the end of this function.
        selfRef = this;
    }

    // The destruction of 'releasedMailboxInfo' will:
    // 1) Release the self reference held by the mailboxInfo, which may trigger
    //    the self-destruction of this Canvas2DLayerBridge
    // 2) Release the SkImage, which will return the texture to skia's scratch
    //    texture pool.
    m_mailboxes.remove(releasedMailboxInfo);

    Canvas2DLayerManager::get().layerTransientResourceAllocationChanged(this);
}

WebLayer* Canvas2DLayerBridge::layer() const
{
    ASSERT(!m_destructionInProgress);
    ASSERT(m_layer);
    return m_layer->layer();
}

void Canvas2DLayerBridge::didDraw()
{
    Canvas2DLayerManager::get().layerDidDraw(this);
}

void Canvas2DLayerBridge::finalizeFrame(const FloatRect &dirtyRect)
{
    ASSERT(!m_destructionInProgress);
    m_layer->layer()->invalidateRect(enclosingIntRect(dirtyRect));
    m_didRecordDrawCommand = true;
}

PassRefPtr<SkImage> Canvas2DLayerBridge::newImageSnapshot()
{
    if (!checkSurfaceValid())
        return nullptr;
    return adoptRef(m_canvas->newImageSnapshot());
}

Canvas2DLayerBridge::MailboxInfo::MailboxInfo(const MailboxInfo& other) {
    memcpy(&m_mailbox, &other.m_mailbox, sizeof(m_mailbox));
    m_image = other.m_image;
    m_parentLayerBridge = other.m_parentLayerBridge;
}

} // namespace blink
