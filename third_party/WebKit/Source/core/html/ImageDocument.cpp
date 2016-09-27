/*
 * Copyright (C) 2006, 2007, 2008, 2010 Apple Inc. All rights reserved.
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
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "core/html/ImageDocument.h"

#include "bindings/core/v8/ExceptionStatePlaceholder.h"
#include "core/HTMLNames.h"
#include "core/dom/RawDataDocumentParser.h"
#include "core/events/EventListener.h"
#include "core/events/MouseEvent.h"
#include "core/fetch/ImageResource.h"
#include "core/frame/FrameHost.h"
#include "core/frame/FrameView.h"
#include "core/frame/LocalDOMWindow.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/PinchViewport.h"
#include "core/frame/Settings.h"
#include "core/html/HTMLBodyElement.h"
#include "core/html/HTMLHeadElement.h"
#include "core/html/HTMLHtmlElement.h"
#include "core/html/HTMLImageElement.h"
#include "core/html/HTMLMetaElement.h"
#include "core/loader/DocumentLoader.h"
#include "core/loader/FrameLoader.h"
#include "core/loader/FrameLoaderClient.h"
#include "wtf/text/StringBuilder.h"
#include <limits>

using namespace std;

namespace blink {

using namespace HTMLNames;

class ImageEventListener : public EventListener {
public:
    static PassRefPtr<ImageEventListener> create(ImageDocument* document) { return adoptRef(new ImageEventListener(document)); }
    static const ImageEventListener* cast(const EventListener* listener)
    {
        return listener->type() == ImageEventListenerType
            ? static_cast<const ImageEventListener*>(listener)
            : 0;
    }

    virtual bool operator==(const EventListener& other);

private:
    ImageEventListener(ImageDocument* document)
        : EventListener(ImageEventListenerType)
        , m_doc(document)
    {
    }

    virtual void handleEvent(ExecutionContext*, Event*);

    ImageDocument* m_doc;
};

class ImageDocumentParser : public RawDataDocumentParser {
public:
    static PassRefPtrWillBeRawPtr<ImageDocumentParser> create(ImageDocument* document)
    {
        return adoptRefWillBeNoop(new ImageDocumentParser(document));
    }

    ImageDocument* document() const
    {
        return toImageDocument(RawDataDocumentParser::document());
    }

private:
    ImageDocumentParser(ImageDocument* document)
        : RawDataDocumentParser(document)
    {
    }

    void appendBytes(const char*, size_t) override;
    virtual void finish();
};

// --------

static float pageZoomFactor(const Document* document)
{
    LocalFrame* frame = document->frame();
    return frame ? frame->pageZoomFactor() : 1;
}

static String imageTitle(const String& filename, const IntSize& size)
{
    StringBuilder result;
    result.append(filename);
    result.appendLiteral(" (");
    // FIXME: Localize numbers. Safari/OSX shows localized numbers with group
    // separaters. For example, "1,920x1,080".
    result.appendNumber(size.width());
    result.append(static_cast<UChar>(0xD7)); // U+00D7 (multiplication sign)
    result.appendNumber(size.height());
    result.append(')');
    return result.toString();
}

void ImageDocumentParser::appendBytes(const char* data, size_t length)
{
    if (!length)
        return;

    LocalFrame* frame = document()->frame();
    Settings* settings = frame->settings();
    if (!frame->loader().client()->allowImage(!settings || settings->imagesEnabled(), document()->url()))
        return;

    if (document()->cachedImage()) {
        RELEASE_ASSERT(length <= std::numeric_limits<unsigned>::max());
        document()->cachedImage()->appendData(data, length);
    }

    if (!document())
        return;

    // Make sure the image layoutObject gets created because we need the layoutObject
    // to read the aspect ratio. See crbug.com/320244
    document()->updateLayoutTreeIfNeeded();
    document()->imageUpdated();
}

void ImageDocumentParser::finish()
{
    if (!isStopped() && document()->imageElement() && document()->cachedImage()) {
        ImageResource* cachedImage = document()->cachedImage();
        cachedImage->finish();
        cachedImage->setResponse(document()->frame()->loader().documentLoader()->response());

        // Report the natural image size in the page title, regardless of zoom level.
        // At a zoom level of 1 the image is guaranteed to have an integer size.
        IntSize size = flooredIntSize(cachedImage->imageSizeForLayoutObject(document()->imageElement()->layoutObject(), 1.0f));
        if (size.width()) {
            // Compute the title, we use the decoded filename of the resource, falling
            // back on the (decoded) hostname if there is no path.
            String fileName = decodeURLEscapeSequences(document()->url().lastPathComponent());
            if (fileName.isEmpty())
                fileName = document()->url().host();
            document()->setTitle(imageTitle(fileName, size));
        }

        document()->imageUpdated();
    }

    if (document())
        document()->finishedParsing();
}

// --------

ImageDocument::ImageDocument(const DocumentInit& initializer)
    : HTMLDocument(initializer, ImageDocumentClass)
    , m_imageElement(nullptr)
    , m_imageSizeIsKnown(false)
    , m_didShrinkImage(false)
    , m_shouldShrinkImage(shouldShrinkToFit())
    , m_shrinkToFitMode(frame()->settings()->viewportEnabled() ? Viewport : Desktop)
{
    setCompatibilityMode(QuirksMode);
    lockCompatibilityMode();
}

PassRefPtrWillBeRawPtr<DocumentParser> ImageDocument::createParser()
{
    return ImageDocumentParser::create(this);
}

void ImageDocument::createDocumentStructure(bool loadingMultipartContent)
{
    RefPtrWillBeRawPtr<HTMLHtmlElement> rootElement = HTMLHtmlElement::create(*this);
    appendChild(rootElement);
    rootElement->insertedByParser();

    if (frame())
        frame()->loader().dispatchDocumentElementAvailable();
    // Normally, ImageDocument creates an HTMLImageElement that doesn't actually load
    // anything, and the ImageDocument routes the main resource data into the HTMLImageElement's
    // ImageResource. However, the main resource pipeline doesn't know how to handle multipart content.
    // For multipart content, we instead stop streaming data through the main resource and re-request
    // the data directly.
    if (loadingMultipartContent)
        loader()->stopLoading();

    RefPtrWillBeRawPtr<HTMLHeadElement> head = HTMLHeadElement::create(*this);
    RefPtrWillBeRawPtr<HTMLMetaElement> meta = HTMLMetaElement::create(*this);
    meta->setAttribute(nameAttr, "viewport");
    meta->setAttribute(contentAttr, "width=device-width, minimum-scale=0.1");
    head->appendChild(meta);

    RefPtrWillBeRawPtr<HTMLBodyElement> body = HTMLBodyElement::create(*this);
    body->setAttribute(styleAttr, "margin: 0px;");

    m_imageElement = HTMLImageElement::create(*this);
    m_imageElement->setAttribute(styleAttr, "-webkit-user-select: none");
    // If the image is multipart, we neglect to mention to the HTMLImageElement that it's in an
    // ImageDocument, so that it requests the image normally.
    if (!loadingMultipartContent)
        m_imageElement->setLoadingImageDocument();
    m_imageElement->setSrc(url().string());
    body->appendChild(m_imageElement.get());

    if (shouldShrinkToFit()) {
        // Add event listeners
        RefPtr<EventListener> listener = ImageEventListener::create(this);
        if (LocalDOMWindow* domWindow = this->domWindow())
            domWindow->addEventListener("resize", listener, false);
        if (m_shrinkToFitMode == Desktop)
            m_imageElement->addEventListener("click", listener.release(), false);
    }

    rootElement->appendChild(head);
    rootElement->appendChild(body);
    if (loadingMultipartContent)
        finishedParsing();
}

float ImageDocument::scale() const
{
    if (!m_imageElement || m_imageElement->document() != this)
        return 1.0f;

    FrameView* view = frame()->view();
    if (!view)
        return 1;

    ASSERT(m_imageElement->cachedImage());
    LayoutSize imageSize = m_imageElement->cachedImage()->imageSizeForLayoutObject(m_imageElement->layoutObject(), pageZoomFactor(this));
    LayoutSize windowSize = LayoutSize(view->width(), view->height());

    float widthScale = windowSize.width().toFloat() / imageSize.width().toFloat();
    float heightScale = windowSize.height().toFloat() / imageSize.height().toFloat();

    return min(widthScale, heightScale);
}

void ImageDocument::resizeImageToFit(ScaleType type)
{
    if (!m_imageElement || m_imageElement->document() != this || (pageZoomFactor(this) > 1 && type == ScaleOnlyUnzoomedDocument))
        return;

    ASSERT(m_imageElement->cachedImage());
    LayoutSize imageSize = m_imageElement->cachedImage()->imageSizeForLayoutObject(m_imageElement->layoutObject(), pageZoomFactor(this));

    float scale = this->scale();
    m_imageElement->setWidth(static_cast<int>(imageSize.width() * scale));
    m_imageElement->setHeight(static_cast<int>(imageSize.height() * scale));

    m_imageElement->setInlineStyleProperty(CSSPropertyCursor, CSSValueZoomIn);
}

void ImageDocument::imageClicked(int x, int y)
{
    ASSERT(m_shrinkToFitMode == Desktop);

    if (!m_imageSizeIsKnown || imageFitsInWindow())
        return;

    m_shouldShrinkImage = !m_shouldShrinkImage;

    if (m_shouldShrinkImage) {
        windowSizeChanged(ScaleZoomedDocument);
    } else {
        restoreImageSize(ScaleZoomedDocument);

        updateLayout();

        double scale = this->scale();

        double scrollX = x / scale - static_cast<double>(frame()->view()->width()) / 2;
        double scrollY = y / scale - static_cast<double>(frame()->view()->height()) / 2;

        frame()->view()->setScrollPosition(DoublePoint(scrollX, scrollY), ProgrammaticScroll);
    }
}

void ImageDocument::imageUpdated()
{
    ASSERT(m_imageElement);

    if (m_imageSizeIsKnown)
        return;

    if (!m_imageElement->cachedImage() || m_imageElement->cachedImage()->imageSizeForLayoutObject(m_imageElement->layoutObject(), pageZoomFactor(this)).isEmpty())
        return;

    m_imageSizeIsKnown = true;

    if (shouldShrinkToFit()) {
        // Force resizing of the image
        windowSizeChanged(ScaleOnlyUnzoomedDocument);
    }

    // Update layout as soon as image size is known. This enables large image files to render progressively or to animate.
    updateLayout();
}

void ImageDocument::restoreImageSize(ScaleType type)
{
    ASSERT(m_shrinkToFitMode == Desktop);

    if (!m_imageElement || !m_imageSizeIsKnown || m_imageElement->document() != this || (pageZoomFactor(this) < 1 && type == ScaleOnlyUnzoomedDocument))
        return;

    ASSERT(m_imageElement->cachedImage());
    LayoutSize imageSize = m_imageElement->cachedImage()->imageSizeForLayoutObject(m_imageElement->layoutObject(), 1.0f);
    m_imageElement->setWidth(imageSize.width());
    m_imageElement->setHeight(imageSize.height());

    if (imageFitsInWindow())
        m_imageElement->removeInlineStyleProperty(CSSPropertyCursor);
    else
        m_imageElement->setInlineStyleProperty(CSSPropertyCursor, CSSValueZoomOut);

    m_didShrinkImage = false;
}

bool ImageDocument::imageFitsInWindow() const
{
    ASSERT(m_shrinkToFitMode == Desktop);

    if (!m_imageElement || m_imageElement->document() != this)
        return true;

    FrameView* view = frame()->view();
    if (!view)
        return true;

    ASSERT(m_imageElement->cachedImage());
    LayoutSize imageSize = m_imageElement->cachedImage()->imageSizeForLayoutObject(m_imageElement->layoutObject(), pageZoomFactor(this));
    LayoutSize windowSize = LayoutSize(view->width(), view->height());

    return imageSize.width() <= windowSize.width() && imageSize.height() <= windowSize.height();
}

void ImageDocument::windowSizeChanged(ScaleType type)
{
    if (!m_imageElement || !m_imageSizeIsKnown || m_imageElement->document() != this)
        return;

    if (m_shrinkToFitMode == Viewport) {
        // For huge images, minimum-scale=0.1 is still too big on small screens.
        // Set max-width so that the image will shrink to fit the width of the screen when
        // the scale is minimum.
        // Don't shrink height to fit because we use width=device-width in viewport meta tag,
        // and expect a full-width reading mode for normal-width-huge-height images.
        int viewportWidth = frame()->host()->pinchViewport().size().width();
        m_imageElement->setInlineStyleProperty(CSSPropertyMaxWidth, viewportWidth * 10, CSSPrimitiveValue::CSS_PX);
        return;
    }

    bool fitsInWindow = imageFitsInWindow();

    // If the image has been explicitly zoomed in, restore the cursor if the image fits
    // and set it to a zoom out cursor if the image doesn't fit
    if (!m_shouldShrinkImage) {
        if (fitsInWindow)
            m_imageElement->removeInlineStyleProperty(CSSPropertyCursor);
        else
            m_imageElement->setInlineStyleProperty(CSSPropertyCursor, CSSValueZoomOut);
        return;
    }

    if (m_didShrinkImage) {
        // If the window has been resized so that the image fits, restore the image size
        // otherwise update the restored image size.
        if (fitsInWindow)
            restoreImageSize(type);
        else
            resizeImageToFit(type);
    } else {
        // If the image isn't resized but needs to be, then resize it.
        if (!fitsInWindow) {
            resizeImageToFit(type);
            m_didShrinkImage = true;
        }
    }
}

ImageResource* ImageDocument::cachedImage()
{
    bool loadingMultipartContent = loader() && loader()->loadingMultipartContent();
    if (!m_imageElement)
        createDocumentStructure(loadingMultipartContent);

    return loadingMultipartContent ? nullptr : m_imageElement->cachedImage();
}

bool ImageDocument::shouldShrinkToFit() const
{
    return frame()->isMainFrame();
}

#if !ENABLE(OILPAN)
void ImageDocument::dispose()
{
    m_imageElement = nullptr;
    HTMLDocument::dispose();
}
#endif

DEFINE_TRACE(ImageDocument)
{
    visitor->trace(m_imageElement);
    HTMLDocument::trace(visitor);
}

// --------

void ImageEventListener::handleEvent(ExecutionContext*, Event* event)
{
    if (event->type() == EventTypeNames::resize) {
        m_doc->windowSizeChanged(ImageDocument::ScaleOnlyUnzoomedDocument);
    } else if (event->type() == EventTypeNames::click && event->isMouseEvent()) {
        MouseEvent* mouseEvent = toMouseEvent(event);
        m_doc->imageClicked(mouseEvent->x(), mouseEvent->y());
    }
}

bool ImageEventListener::operator==(const EventListener& listener)
{
    if (const ImageEventListener* imageEventListener = ImageEventListener::cast(&listener))
        return m_doc == imageEventListener->m_doc;
    return false;
}

}
