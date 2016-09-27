/*
 * Copyright (C) 2004, 2006, 2007 Apple Inc. All rights reserved.
 * Copyright (C) 2007 Alp Toker <alp@atoker.com>
 * Copyright (C) 2010 Torch Mobile (Beijing) Co. Ltd. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE COMPUTER, INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE COMPUTER, INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/html/HTMLCanvasElement.h"

#include "bindings/core/v8/ExceptionMessages.h"
#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/ScriptController.h"
#include "core/HTMLNames.h"
#include "core/dom/Document.h"
#include "core/dom/ExceptionCode.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/Settings.h"
#include "core/html/ImageData.h"
#include "core/html/canvas/CanvasContextCreationAttributes.h"
#include "core/html/canvas/CanvasFontCache.h"
#include "core/html/canvas/CanvasRenderingContext.h"
#include "core/html/canvas/CanvasRenderingContextFactory.h"
#include "core/layout/LayoutHTMLCanvas.h"
#include "core/paint/DeprecatedPaintLayer.h"
#include "platform/MIMETypeRegistry.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "platform/graphics/BitmapImage.h"
//#include "platform/graphics/Canvas2DImageBufferSurface.h"
#include "platform/graphics/ExpensiveCanvasHeuristicParameters.h"
#include "platform/graphics/ImageBuffer.h"
#include "platform/graphics/RecordingImageBufferSurface.h"
#include "platform/graphics/StaticBitmapImage.h"
#include "platform/graphics/UnacceleratedImageBufferSurface.h"
#include "platform/graphics/gpu/AcceleratedImageBufferSurface.h"
#include "platform/transforms/AffineTransform.h"
#include "public/platform/Platform.h"
#include <math.h>
#include <v8.h>

namespace blink {

using namespace HTMLNames;

namespace {

// These values come from the WhatWG spec.
const int DefaultWidth = 300;
const int DefaultHeight = 150;

// Firefox limits width/height to 32767 pixels, but slows down dramatically before it
// reaches that limit. We limit by area instead, giving us larger maximum dimensions,
// in exchange for a smaller maximum canvas size.
const int MaxCanvasArea = 32768 * 8192; // Maximum canvas area in CSS pixels

// In Skia, we will also limit width/height to 32767.
const int MaxSkiaDim = 32767; // Maximum width/height in CSS pixels.

bool canCreateImageBuffer(const IntSize& size)
{
    if (size.isEmpty())
        return false;
    if (size.width() * size.height() > MaxCanvasArea)
        return false;
    if (size.width() > MaxSkiaDim || size.height() > MaxSkiaDim)
        return false;
    return true;
}

PassRefPtr<Image> createTransparentImage(const IntSize& size)
{
    ASSERT(canCreateImageBuffer(size));
    SkBitmap bitmap;
    bitmap.allocN32Pixels(size.width(), size.height());
    bitmap.eraseColor(SK_ColorTRANSPARENT);
    return BitmapImage::create(bitmap);
}

} // namespace

DEFINE_EMPTY_DESTRUCTOR_WILL_BE_REMOVED(CanvasObserver);

inline HTMLCanvasElement::HTMLCanvasElement(Document& document)
    : HTMLElement(canvasTag, document)
    , DocumentVisibilityObserver(document)
    , m_size(DefaultWidth, DefaultHeight)
    , m_ignoreReset(false)
    , m_accelerationDisabled(false)
    , m_externallyAllocatedMemory(0)
    , m_originClean(true)
    , m_didFailToCreateImageBuffer(false)
    , m_imageBufferIsClear(false)
{
    setHasCustomStyleCallbacks();
}

DEFINE_NODE_FACTORY(HTMLCanvasElement)

HTMLCanvasElement::~HTMLCanvasElement()
{
    v8::Isolate::GetCurrent()->AdjustAmountOfExternalAllocatedMemory(-m_externallyAllocatedMemory);
#if !ENABLE(OILPAN)
    for (CanvasObserver* canvasObserver : m_observers)
        canvasObserver->canvasDestroyed(this);
    // Ensure these go away before the ImageBuffer.
    m_context.clear();
#endif
}

void HTMLCanvasElement::parseAttribute(const QualifiedName& name, const AtomicString& value)
{
    if (name == widthAttr || name == heightAttr)
        reset();
    HTMLElement::parseAttribute(name, value);
}

LayoutObject* HTMLCanvasElement::createLayoutObject(const ComputedStyle& style)
{
    LocalFrame* frame = document().frame();
    if (frame && frame->script().canExecuteScripts(NotAboutToExecuteScript))
        return new LayoutHTMLCanvas(this);
    return HTMLElement::createLayoutObject(style);
}

void HTMLCanvasElement::didRecalcStyle(StyleRecalcChange)
{
    SkFilterQuality filterQuality = ensureComputedStyle()->imageRendering() == ImageRenderingPixelated ? kNone_SkFilterQuality : kLow_SkFilterQuality;
    if (is3D()) {
        m_context->setFilterQuality(filterQuality);
        setNeedsCompositingUpdate();
    } else if (hasImageBuffer()) {
        m_imageBuffer->setFilterQuality(filterQuality);
    }
}

Node::InsertionNotificationRequest HTMLCanvasElement::insertedInto(ContainerNode* node)
{
    setIsInCanvasSubtree(true);
    return HTMLElement::insertedInto(node);
}

void HTMLCanvasElement::addObserver(CanvasObserver* observer)
{
    m_observers.add(observer);
}

void HTMLCanvasElement::removeObserver(CanvasObserver* observer)
{
    m_observers.remove(observer);
}

void HTMLCanvasElement::setHeight(int value)
{
    setIntegralAttribute(heightAttr, value);
}

void HTMLCanvasElement::setWidth(int value)
{
    setIntegralAttribute(widthAttr, value);
}

HTMLCanvasElement::ContextFactoryVector& HTMLCanvasElement::renderingContextFactories()
{
    ASSERT(isMainThread());
    DEFINE_STATIC_LOCAL(ContextFactoryVector, s_contextFactories, (CanvasRenderingContext::ContextTypeCount));
    return s_contextFactories;
}

CanvasRenderingContextFactory* HTMLCanvasElement::getRenderingContextFactory(int type)
{
    ASSERT(type < CanvasRenderingContext::ContextTypeCount);
    return renderingContextFactories()[type].get();
}

void HTMLCanvasElement::registerRenderingContextFactory(PassOwnPtr<CanvasRenderingContextFactory> renderingContextFactory)
{
    CanvasRenderingContext::ContextType type = renderingContextFactory->contextType();
    ASSERT(type < CanvasRenderingContext::ContextTypeCount);
    ASSERT(!renderingContextFactories()[type]);
    renderingContextFactories()[type] = renderingContextFactory;
}

ScriptValue HTMLCanvasElement::getContext(ScriptState* scriptState, const String& type, const CanvasContextCreationAttributes& attributes)
{
    CanvasRenderingContext* context = getCanvasRenderingContext(type, attributes);
    if (!context) {
        return ScriptValue::createNull(scriptState);
    }
    return ScriptValue(scriptState, toV8(context, scriptState->context()->Global(), scriptState->isolate()));
}

CanvasRenderingContext* HTMLCanvasElement::getCanvasRenderingContext(const String& type, const CanvasContextCreationAttributes& attributes)
{
    CanvasRenderingContext::ContextType contextType = CanvasRenderingContext::contextTypeFromId(type);

    // Unknown type.
    if (contextType == CanvasRenderingContext::ContextTypeCount)
        return nullptr;

    // Log the aliased context type used.
    if (!m_context)
        Platform::current()->histogramEnumeration("Canvas.ContextType", contextType, CanvasRenderingContext::ContextTypeCount);

    contextType = CanvasRenderingContext::resolveContextTypeAliases(contextType);

    CanvasRenderingContextFactory* factory = getRenderingContextFactory(contextType);
    if (!factory)
        return nullptr;

    // FIXME - The code depends on the context not going away once created, to prevent JS from
    // seeing a dangling pointer. So for now we will disallow the context from being changed
    // once it is created.
    if (m_context) {
        if (m_context->contextType() == contextType)
            return m_context.get();

        factory->onError(this, "Canvas has an existing context of a different type");
        return nullptr;
    }

    m_context = factory->create(this, attributes, document());
    if (!m_context)
        return nullptr;

    if (m_context->is3d()) {
        const ComputedStyle* style = ensureComputedStyle();
        if (style)
            m_context->setFilterQuality(style->imageRendering() == ImageRenderingPixelated ? kNone_SkFilterQuality : kLow_SkFilterQuality);
        updateExternallyAllocatedMemory();
    }
    setNeedsCompositingUpdate();

    return m_context.get();
}

bool HTMLCanvasElement::shouldBeDirectComposited() const
{
    return (m_context && m_context->isAccelerated()) || (hasImageBuffer() && buffer()->isExpensiveToPaint());
}

bool HTMLCanvasElement::isPaintable() const
{
    if (!m_context)
        return canCreateImageBuffer(size());
    return buffer();
}

void HTMLCanvasElement::didDraw(const FloatRect& rect)
{
    if (rect.isEmpty())
        return;
    m_imageBufferIsClear = false;
    clearCopiedImage();
    if (layoutObject())
        layoutObject()->setMayNeedPaintInvalidation();
    m_dirtyRect.unite(rect);
    if (m_context && m_context->is2d() && hasImageBuffer())
        buffer()->didDraw(rect);
    notifyObserversCanvasChanged(rect);
}

void HTMLCanvasElement::didFinalizeFrame()
{
    if (m_dirtyRect.isEmpty())
        return;

    // Propagate the m_dirtyRect accumulated so far to the compositor
    // before restarting with a blank dirty rect.
    FloatRect srcRect(0, 0, size().width(), size().height());
    m_dirtyRect.intersect(srcRect);
    LayoutBox* ro = layoutBox();
    // Canvas content updates do not need to be propagated as
    // paint invalidations if the canvas is accelerated, since
    // the canvas contents are sent separately through a texture layer.
    if (ro && (!m_context || !m_context->isAccelerated())) {
        LayoutRect mappedDirtyRect(enclosingIntRect(mapRect(m_dirtyRect, srcRect, ro->contentBoxRect())));
        // For querying DeprecatedPaintLayer::compositingState()
        // FIXME: is this invalidation using the correct compositing state?
        DisableCompositingQueryAsserts disabler;
        ro->invalidatePaintRectangle(mappedDirtyRect);
    }
    m_dirtyRect = FloatRect();
}

void HTMLCanvasElement::restoreCanvasMatrixClipStack()
{
    if (m_context)
        m_context->restoreCanvasMatrixClipStack();
}

void HTMLCanvasElement::doDeferredPaintInvalidation()
{
    ASSERT(!m_dirtyRect.isEmpty());
    if (is3D()) {
        didFinalizeFrame();
    } else {
        ASSERT(hasImageBuffer());
        FloatRect srcRect(0, 0, size().width(), size().height());
        m_dirtyRect.intersect(srcRect);
        LayoutBox* lb = layoutBox();
        if (lb) {
            FloatRect mappedDirtyRect = mapRect(m_dirtyRect, srcRect, lb->contentBoxRect());
            if (m_context->isAccelerated()) {
                // Accelerated 2D canvases need the dirty rect to be expressed relative to the
                // content box, as opposed to the layout box.
                mappedDirtyRect.move(-lb->contentBoxOffset());
            }
            m_imageBuffer->finalizeFrame(mappedDirtyRect);
        } else {
            m_imageBuffer->finalizeFrame(m_dirtyRect);
        }
    }
    ASSERT(m_dirtyRect.isEmpty());
}

void HTMLCanvasElement::notifyObserversCanvasChanged(const FloatRect& rect)
{
    for (CanvasObserver* canvasObserver : m_observers)
        canvasObserver->canvasChanged(this, rect);
}

void HTMLCanvasElement::reset()
{
    if (m_ignoreReset)
        return;

    m_dirtyRect = FloatRect();

    bool ok;
    bool hadImageBuffer = hasImageBuffer();

    int w = getAttribute(widthAttr).toInt(&ok);
    if (!ok || w < 0)
        w = DefaultWidth;

    int h = getAttribute(heightAttr).toInt(&ok);
    if (!ok || h < 0)
        h = DefaultHeight;

    if (m_context && m_context->is2d())
        m_context->reset();

    IntSize oldSize = size();
    IntSize newSize(w, h);

    // If the size of an existing buffer matches, we can just clear it instead of reallocating.
    // This optimization is only done for 2D canvases for now.
    if (hadImageBuffer && oldSize == newSize && m_context && m_context->is2d() && !buffer()->isRecording()) {
        if (!m_imageBufferIsClear) {
            m_imageBufferIsClear = true;
            m_context->clearRect(0, 0, width(), height());
        }
        return;
    }

    setSurfaceSize(newSize);

    if (m_context && m_context->is3d() && oldSize != size())
        m_context->reshape(width(), height());

    if (LayoutObject* layoutObject = this->layoutObject()) {
        if (layoutObject->isCanvas()) {
            if (oldSize != size()) {
                toLayoutHTMLCanvas(layoutObject)->canvasSizeChanged();
                if (layoutBox() && layoutBox()->hasAcceleratedCompositing())
                    layoutBox()->contentChanged(CanvasChanged);
            }
            if (hadImageBuffer)
                layoutObject->setShouldDoFullPaintInvalidation();
        }
    }

    for (CanvasObserver* canvasObserver : m_observers)
        canvasObserver->canvasResized(this);
}

bool HTMLCanvasElement::paintsIntoCanvasBuffer() const
{
    ASSERT(m_context);

    if (!m_context->isAccelerated())
        return true;

    if (layoutBox() && layoutBox()->hasAcceleratedCompositing())
        return false;

    return true;
}

void HTMLCanvasElement::paint(GraphicsContext* context, const LayoutRect& r)
{
    // FIXME: crbug.com/438240; there is a bug with the new CSS blending and compositing feature.
    if (!m_context)
        return;
    if (!paintsIntoCanvasBuffer() && !document().printing())
        return;

    m_context->paintRenderingResultsToCanvas(FrontBuffer);
    if (hasImageBuffer()) {
        if (!context->contextDisabled()) {
            SkXfermode::Mode compositeOperator = !m_context || m_context->hasAlpha() ? SkXfermode::kSrcOver_Mode : SkXfermode::kSrc_Mode;
            buffer()->draw(context, pixelSnappedIntRect(r), 0, compositeOperator);
        }
    } else {
        // When alpha is false, we should draw to opaque black.
        if (!m_context->hasAlpha())
            context->fillRect(FloatRect(r), Color(0, 0, 0));
    }

    if (is3D() && paintsIntoCanvasBuffer())
        m_context->markLayerComposited();
}

bool HTMLCanvasElement::is3D() const
{
    return m_context && m_context->is3d();
}

void HTMLCanvasElement::setSurfaceSize(const IntSize& size)
{
    m_size = size;
    m_didFailToCreateImageBuffer = false;
    discardImageBuffer();
    clearCopiedImage();
    if (m_context && m_context->is2d() && m_context->isContextLost()) {
        m_context->didSetSurfaceSize();
    }
}

String HTMLCanvasElement::toEncodingMimeType(const String& mimeType)
{
    String lowercaseMimeType = mimeType.lower();

    // FIXME: Make isSupportedImageMIMETypeForEncoding threadsafe (to allow this method to be used on a worker thread).
    if (mimeType.isNull() || !MIMETypeRegistry::isSupportedImageMIMETypeForEncoding(lowercaseMimeType))
        lowercaseMimeType = "image/png";

    return lowercaseMimeType;
}

const AtomicString HTMLCanvasElement::imageSourceURL() const
{
    return AtomicString(toDataURLInternal("image/png", 0, FrontBuffer));
}

String HTMLCanvasElement::toDataURLInternal(const String& mimeType, const double* quality, SourceDrawingBuffer sourceBuffer) const
{
    if (!isPaintable())
        return String("data:,");

    String encodingMimeType = toEncodingMimeType(mimeType);
    if (!m_context) {
        ImageData* imageData = ImageData::create(m_size);
        ScopedDisposal<ImageData> disposer(imageData);
        return ImageDataBuffer(imageData->size(), imageData->data()->data()).toDataURL(encodingMimeType, quality);
    }

    if (m_context->is3d()) {
        // Get non-premultiplied data because of inaccurate premultiplied alpha conversion of buffer()->toDataURL().
        ImageData* imageData = m_context->paintRenderingResultsToImageData(sourceBuffer);
        ScopedDisposal<ImageData> disposer(imageData);
        if (imageData)
            return ImageDataBuffer(imageData->size(), imageData->data()->data()).toDataURL(encodingMimeType, quality);
        m_context->paintRenderingResultsToCanvas(sourceBuffer);
    }

    return buffer()->toDataURL(encodingMimeType, quality);
}

String HTMLCanvasElement::toDataURL(const String& mimeType, const ScriptValue& qualityArgument, ExceptionState& exceptionState) const
{
    if (!originClean()) {
        exceptionState.throwSecurityError("Tainted canvases may not be exported.");
        return String();
    }
    double quality;
    double* qualityPtr = nullptr;
    if (!qualityArgument.isEmpty()) {
        v8::Local<v8::Value> v8Value = qualityArgument.v8Value();
        if (v8Value->IsNumber()) {
            quality = v8Value.As<v8::Number>()->Value();
            qualityPtr = &quality;
        }
    }
    return toDataURLInternal(mimeType, qualityPtr, BackBuffer);
}

SecurityOrigin* HTMLCanvasElement::securityOrigin() const
{
    return document().securityOrigin();
}

bool HTMLCanvasElement::originClean() const
{
    if (document().settings() && document().settings()->disableReadingFromCanvas())
        return false;
    return m_originClean;
}

bool HTMLCanvasElement::shouldAccelerate(const IntSize& size) const
{
    if (m_context && !m_context->is2d())
        return false;

    if (m_accelerationDisabled)
        return false;

    if (RuntimeEnabledFeatures::forceDisplayList2dCanvasEnabled())
        return false;

    Settings* settings = document().settings();
    if (!settings || !settings->accelerated2dCanvasEnabled())
        return false;

    int canvasPixelCount = size.width() * size.height();

    if (RuntimeEnabledFeatures::displayList2dCanvasEnabled()) {
#if 0
        // TODO(junov): re-enable this code once we solve the problem of recording
        // GPU-backed images to an SkPicture for cross-context rendering crbug.com/490328

        // If the compositor provides GPU acceleration to display list canvases, we
        // prefer that over direct acceleration.
        if (document().viewportDescription().matchesHeuristicsForGpuRasterization())
            return false;
#endif
        // If the GPU resources would be very expensive, prefer a display list.
        if (canvasPixelCount > ExpensiveCanvasHeuristicParameters::PreferDisplayListOverGpuSizeThreshold)
            return false;
    }

    // Do not use acceleration for small canvas.
    if (canvasPixelCount < settings->minimumAccelerated2dCanvasSize())
        return false;

    if (!Platform::current()->canAccelerate2dCanvas())
        return false;

    return true;
}

class UnacceleratedSurfaceFactory : public RecordingImageBufferFallbackSurfaceFactory {
public:
    virtual PassOwnPtr<ImageBufferSurface> createSurface(const IntSize& size, OpacityMode opacityMode)
    {
        return adoptPtr(new UnacceleratedImageBufferSurface(size, opacityMode));
    }

    virtual ~UnacceleratedSurfaceFactory() { }
};

bool HTMLCanvasElement::shouldUseDisplayList(const IntSize& deviceSize)
{
    if (RuntimeEnabledFeatures::forceDisplayList2dCanvasEnabled())
        return true;

    if (!RuntimeEnabledFeatures::displayList2dCanvasEnabled())
        return false;

    return true;
}

PassOwnPtr<ImageBufferSurface> HTMLCanvasElement::createImageBufferSurface(const IntSize& deviceSize, int* msaaSampleCount)
{
    OpacityMode opacityMode = !m_context || m_context->hasAlpha() ? NonOpaque : Opaque;

    *msaaSampleCount = 0;
    if (is3D()) {
        // If 3d, but the use of the canvas will be for non-accelerated content
        // (such as -webkit-canvas, then then make a non-accelerated
        // ImageBuffer. This means copying the internal Image will require a
        // pixel readback, but that is unavoidable in this case.
        // FIXME: Actually, avoid setting m_accelerationDisabled at all when
        // doing GPU-based rasterization.
#ifdef MINIBLINK_NOT_IMPLEMENTED
        if (m_accelerationDisabled)
            return adoptPtr(new UnacceleratedImageBufferSurface(deviceSize, opacityMode));
        return adoptPtr(new AcceleratedImageBufferSurface(deviceSize, opacityMode));
#endif // MINIBLINK_NOT_IMPLEMENTED
        notImplemented();
        return nullptr;
    }

    if (shouldAccelerate(deviceSize)) {
#ifdef MINIBLINK_NOT_IMPLEMENTED
        if (document().settings())
            *msaaSampleCount = document().settings()->accelerated2dCanvasMSAASampleCount();
        OwnPtr<ImageBufferSurface> surface = adoptPtr(new Canvas2DImageBufferSurface(deviceSize, opacityMode, *msaaSampleCount));
        if (surface->isValid())
            return surface.release();
#endif // MINIBLINK_NOT_IMPLEMENTED
        notImplemented();
        return nullptr;
    }

    OwnPtr<RecordingImageBufferFallbackSurfaceFactory> surfaceFactory = adoptPtr(new UnacceleratedSurfaceFactory());

    if (shouldUseDisplayList(deviceSize)) {
        OwnPtr<ImageBufferSurface> surface = adoptPtr(new RecordingImageBufferSurface(deviceSize, surfaceFactory.release(), opacityMode));
        if (surface->isValid())
            return surface.release();
        surfaceFactory = adoptPtr(new UnacceleratedSurfaceFactory()); // recreate because previous one was released
    }

    return surfaceFactory->createSurface(deviceSize, opacityMode);
}

void HTMLCanvasElement::createImageBuffer()
{
    createImageBufferInternal(nullptr);
    if (m_didFailToCreateImageBuffer && m_context->is2d())
        m_context->loseContext(CanvasRenderingContext::SyntheticLostContext);
}

void HTMLCanvasElement::createImageBufferInternal(PassOwnPtr<ImageBufferSurface> externalSurface)
{
    ASSERT(!m_imageBuffer);

    m_didFailToCreateImageBuffer = true;
    m_imageBufferIsClear = true;

    if (!canCreateImageBuffer(size()))
        return;

    int msaaSampleCount = 0;
    OwnPtr<ImageBufferSurface> surface;
    if (externalSurface) {
        surface = externalSurface;
    } else {
        surface = createImageBufferSurface(size(), &msaaSampleCount);
    }
    m_imageBuffer = ImageBuffer::create(surface.release());
    if (!m_imageBuffer)
        return;
    m_imageBuffer->setClient(this);

    document().updateLayoutTreeIfNeeded();
    const ComputedStyle* style = ensureComputedStyle();
    m_imageBuffer->setFilterQuality((style && (style->imageRendering() == ImageRenderingPixelated)) ? kNone_SkFilterQuality : kLow_SkFilterQuality);

    m_didFailToCreateImageBuffer = false;

    updateExternallyAllocatedMemory();

    if (is3D()) {
        // Early out for WebGL canvases
        return;
    }

    m_imageBuffer->setClient(this);
    // Enabling MSAA overrides a request to disable antialiasing. This is true regardless of whether the
    // rendering mode is accelerated or not. For consistency, we don't want to apply AA in accelerated
    // canvases but not in unaccelerated canvases.
    if (!msaaSampleCount && document().settings() && !document().settings()->antialiased2dCanvasEnabled())
        m_context->setShouldAntialias(false);

    if (m_context)
        setNeedsCompositingUpdate();
}

void HTMLCanvasElement::notifySurfaceInvalid()
{
    if (m_context && m_context->is2d())
        m_context->loseContext(CanvasRenderingContext::RealLostContext);
}

DEFINE_TRACE(HTMLCanvasElement)
{
#if ENABLE(OILPAN)
    visitor->trace(m_observers);
    visitor->trace(m_context);
#endif
    DocumentVisibilityObserver::trace(visitor);
    HTMLElement::trace(visitor);
}

void HTMLCanvasElement::updateExternallyAllocatedMemory() const
{
    int bufferCount = 0;
    if (m_imageBuffer) {
        bufferCount++;
        if (m_imageBuffer->isAccelerated()) {
            // The number of internal GPU buffers vary between one (stable
            // non-displayed state) and three (triple-buffered animations).
            // Adding 2 is a pessimistic but relevant estimate.
            // Note: These buffers might be allocated in GPU memory.
            bufferCount += 2;
        }
    }
    if (m_copiedImage)
        bufferCount++;

    // Four bytes per pixel per buffer.
    Checked<intptr_t, RecordOverflow> checkedExternallyAllocatedMemory = 4 * bufferCount;
    if (is3D())
        checkedExternallyAllocatedMemory += m_context->externallyAllocatedBytesPerPixel();

    checkedExternallyAllocatedMemory *= width();
    checkedExternallyAllocatedMemory *= height();
    intptr_t externallyAllocatedMemory;
    if (checkedExternallyAllocatedMemory.safeGet(externallyAllocatedMemory) == CheckedState::DidOverflow)
        externallyAllocatedMemory = std::numeric_limits<intptr_t>::max();

    // Subtracting two intptr_t that are known to be positive will never underflow.
    v8::Isolate::GetCurrent()->AdjustAmountOfExternalAllocatedMemory(externallyAllocatedMemory - m_externallyAllocatedMemory);
    m_externallyAllocatedMemory = externallyAllocatedMemory;
}

SkCanvas* HTMLCanvasElement::drawingCanvas() const
{
    return buffer() ? m_imageBuffer->canvas() : nullptr;
}

SkCanvas* HTMLCanvasElement::existingDrawingCanvas() const
{
    if (!hasImageBuffer())
        return nullptr;

    return m_imageBuffer->canvas();
}

ImageBuffer* HTMLCanvasElement::buffer() const
{
    ASSERT(m_context);
    if (!hasImageBuffer() && !m_didFailToCreateImageBuffer)
        const_cast<HTMLCanvasElement*>(this)->createImageBuffer();
    return m_imageBuffer.get();
}

void HTMLCanvasElement::createImageBufferUsingSurfaceForTesting(PassOwnPtr<ImageBufferSurface> surface)
{
    discardImageBuffer();
    setWidth(surface->size().width());
    setHeight(surface->size().height());
    createImageBufferInternal(surface);
}

void HTMLCanvasElement::ensureUnacceleratedImageBuffer()
{
    ASSERT(m_context);
    if ((hasImageBuffer() && !m_imageBuffer->isAccelerated()) || m_didFailToCreateImageBuffer)
        return;
    discardImageBuffer();
    OpacityMode opacityMode = m_context->hasAlpha() ? NonOpaque : Opaque;
    m_imageBuffer = ImageBuffer::create(size(), opacityMode);
    m_didFailToCreateImageBuffer = !m_imageBuffer;
}

PassRefPtr<Image> HTMLCanvasElement::copiedImage(SourceDrawingBuffer sourceBuffer) const
{
    if (!isPaintable())
        return nullptr;
    if (!m_context)
        return createTransparentImage(size());

    bool needToUpdate = !m_copiedImage;
    // The concept of SourceDrawingBuffer is valid on only WebGL.
    if (m_context->is3d())
        needToUpdate |= m_context->paintRenderingResultsToCanvas(sourceBuffer);
    if (needToUpdate && buffer()) {
        m_copiedImage = buffer()->copyImage(CopyBackingStore, Unscaled);
        updateExternallyAllocatedMemory();
    }
    return m_copiedImage;
}

void HTMLCanvasElement::discardImageBuffer()
{
    m_imageBuffer.clear();
    m_dirtyRect = FloatRect();
    updateExternallyAllocatedMemory();
}

void HTMLCanvasElement::clearCopiedImage()
{
    if (m_copiedImage) {
        m_copiedImage.clear();
        updateExternallyAllocatedMemory();
    }
}

AffineTransform HTMLCanvasElement::baseTransform() const
{
    ASSERT(hasImageBuffer() && !m_didFailToCreateImageBuffer);
    return m_imageBuffer->baseTransform();
}

void HTMLCanvasElement::didChangeVisibilityState(PageVisibilityState visibility)
{
    if (!m_context)
        return;
    bool hidden = visibility != PageVisibilityStateVisible;
    m_context->setIsHidden(hidden);
    if (hidden) {
        clearCopiedImage();
        if (is3D()) {
            discardImageBuffer();
        }
    }
}

void HTMLCanvasElement::didMoveToNewDocument(Document& oldDocument)
{
    setObservedDocument(document());
    if (m_context)
        m_context->didMoveToNewDocument(&document());
    HTMLElement::didMoveToNewDocument(oldDocument);
}

PassRefPtr<Image> HTMLCanvasElement::getSourceImageForCanvas(SourceImageMode mode, SourceImageStatus* status) const
{
    if (!width() || !height()) {
        *status = ZeroSizeCanvasSourceImageStatus;
        return nullptr;
    }

    if (!isPaintable()) {
        *status = InvalidSourceImageStatus;
        return nullptr;
    }

    if (!m_context) {
        *status = NormalSourceImageStatus;
        return createTransparentImage(size());
    }

    m_imageBuffer->willAccessPixels();

    if (m_context->is3d()) {
        m_context->paintRenderingResultsToCanvas(BackBuffer);
    }

    RefPtr<SkImage> image = buffer()->newImageSnapshot();
    if (image) {
        *status = NormalSourceImageStatus;
        return StaticBitmapImage::create(image.release());
    }

    *status = InvalidSourceImageStatus;
    return nullptr;
}

bool HTMLCanvasElement::wouldTaintOrigin(SecurityOrigin*) const
{
    return !originClean();
}

FloatSize HTMLCanvasElement::elementSize() const
{
    return FloatSize(width(), height());
}

bool HTMLCanvasElement::isOpaque() const
{
    return m_context && !m_context->hasAlpha();
}

} // blink
