/*
 * Copyright (C) 2007, 2008, 2009, 2010 Apple Inc. All rights reserved.
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
#include "core/html/HTMLVideoElement.h"

#include "bindings/core/v8/ExceptionState.h"
#include "core/CSSPropertyNames.h"
#include "core/HTMLNames.h"
#include "core/dom/Attribute.h"
#include "core/dom/Document.h"
#include "core/dom/ExceptionCode.h"
#include "core/dom/shadow/ShadowRoot.h"
#include "core/frame/Settings.h"
#include "core/html/parser/HTMLParserIdioms.h"
#include "core/layout/LayoutImage.h"
#include "core/layout/LayoutVideo.h"
#include "platform/UserGestureIndicator.h"
#include "platform/graphics/GraphicsContext.h"
#include "platform/graphics/ImageBuffer.h"
#include "platform/graphics/gpu/Extensions3DUtil.h"
#include "public/platform/WebCanvas.h"
#include "public/platform/WebGraphicsContext3D.h"
#include "public/platform/WebSize.h"
#include "public/platform/WebRect.h"

namespace blink {

using namespace HTMLNames;

inline HTMLVideoElement::HTMLVideoElement(Document& document)
    : HTMLMediaElement(videoTag, document)
{
    if (document.settings())
        m_defaultPosterURL = AtomicString(document.settings()->defaultVideoPosterURL());
}

PassRefPtrWillBeRawPtr<HTMLVideoElement> HTMLVideoElement::create(Document& document)
{
    RefPtrWillBeRawPtr<HTMLVideoElement> video = adoptRefWillBeNoop(new HTMLVideoElement(document));
    video->ensureUserAgentShadowRoot();
    video->suspendIfNeeded();
    return video.release();
}

DEFINE_TRACE(HTMLVideoElement)
{
    visitor->trace(m_imageLoader);
    HTMLMediaElement::trace(visitor);
}

bool HTMLVideoElement::layoutObjectIsNeeded(const ComputedStyle& style)
{
    return HTMLElement::layoutObjectIsNeeded(style);
}

LayoutObject* HTMLVideoElement::createLayoutObject(const ComputedStyle&)
{
    return new LayoutVideo(this);
}

void HTMLVideoElement::attach(const AttachContext& context)
{
    HTMLMediaElement::attach(context);

    updateDisplayState();
    if (shouldDisplayPosterImage()) {
        if (!m_imageLoader)
            m_imageLoader = HTMLImageLoader::create(this);
        m_imageLoader->updateFromElement();
        if (layoutObject())
            toLayoutImage(layoutObject())->imageResource()->setImageResource(m_imageLoader->image());
    }
}

void HTMLVideoElement::collectStyleForPresentationAttribute(const QualifiedName& name, const AtomicString& value, MutableStylePropertySet* style)
{
    if (name == widthAttr)
        addHTMLLengthToStyle(style, CSSPropertyWidth, value);
    else if (name == heightAttr)
        addHTMLLengthToStyle(style, CSSPropertyHeight, value);
    else
        HTMLMediaElement::collectStyleForPresentationAttribute(name, value, style);
}

bool HTMLVideoElement::isPresentationAttribute(const QualifiedName& name) const
{
    if (name == widthAttr || name == heightAttr)
        return true;
    return HTMLMediaElement::isPresentationAttribute(name);
}

void HTMLVideoElement::parseAttribute(const QualifiedName& name, const AtomicString& value)
{
    if (name == posterAttr) {
        // In case the poster attribute is set after playback, don't update the
        // display state, post playback the correct state will be picked up.
        if (displayMode() < Video || !hasAvailableVideoFrame()) {
            // Force a poster recalc by setting m_displayMode to Unknown directly before calling updateDisplayState.
            HTMLMediaElement::setDisplayMode(Unknown);
            updateDisplayState();
        }
        if (!posterImageURL().isEmpty()) {
            if (!m_imageLoader)
                m_imageLoader = HTMLImageLoader::create(this);
            m_imageLoader->updateFromElement(ImageLoader::UpdateIgnorePreviousError);
        } else {
            if (layoutObject())
                toLayoutImage(layoutObject())->imageResource()->setImageResource(0);
        }
        // Notify the player when the poster image URL changes.
        if (webMediaPlayer())
            webMediaPlayer()->setPoster(posterImageURL());
    } else {
        HTMLMediaElement::parseAttribute(name, value);
    }
}

bool HTMLVideoElement::supportsFullscreen() const
{
    if (!document().page())
        return false;

    if (!webMediaPlayer())
        return false;

    return true;
}

unsigned HTMLVideoElement::videoWidth() const
{
    if (!webMediaPlayer())
        return 0;
    return webMediaPlayer()->naturalSize().width;
}

unsigned HTMLVideoElement::videoHeight() const
{
    if (!webMediaPlayer())
        return 0;
    return webMediaPlayer()->naturalSize().height;
}

bool HTMLVideoElement::isURLAttribute(const Attribute& attribute) const
{
    return attribute.name() == posterAttr || HTMLMediaElement::isURLAttribute(attribute);
}

const AtomicString HTMLVideoElement::imageSourceURL() const
{
    const AtomicString& url = getAttribute(posterAttr);
    if (!stripLeadingAndTrailingHTMLSpaces(url).isEmpty())
        return url;
    return m_defaultPosterURL;
}

void HTMLVideoElement::setDisplayMode(DisplayMode mode)
{
    DisplayMode oldMode = displayMode();
    KURL poster = posterImageURL();

    if (!poster.isEmpty()) {
        // We have a poster path, but only show it until the user triggers display by playing or seeking and the
        // media engine has something to display.
        // Don't show the poster if there is a seek operation or
        // the video has restarted because of loop attribute
        if (mode == Video && oldMode == Poster && !hasAvailableVideoFrame())
            return;
    }

    HTMLMediaElement::setDisplayMode(mode);

    if (layoutObject() && displayMode() != oldMode)
        layoutObject()->updateFromElement();
}

void HTMLVideoElement::updateDisplayState()
{
    if (posterImageURL().isEmpty())
        setDisplayMode(Video);
    else if (displayMode() < Poster)
        setDisplayMode(Poster);
}

void HTMLVideoElement::paintCurrentFrame(SkCanvas* canvas, const IntRect& destRect, const SkPaint* paint) const
{
    if (!webMediaPlayer())
        return;

    SkXfermode::Mode mode;
    if (!paint || !SkXfermode::AsMode(paint->getXfermode(), &mode))
        mode = SkXfermode::kSrcOver_Mode;

    // TODO(junov, philipj): crbug.com/456529 Pass the whole SkPaint instead of only alpha and xfermode
    webMediaPlayer()->paint(canvas, destRect, paint ? paint->getAlpha() : 0xFF, mode);
}

bool HTMLVideoElement::copyVideoTextureToPlatformTexture(WebGraphicsContext3D* context, Platform3DObject texture, GLenum internalFormat, GLenum type, bool premultiplyAlpha, bool flipY)
{
#ifdef MINIBLINK_NOT_IMPLEMENTED
    if (!webMediaPlayer())
        return false;

    ASSERT(Extensions3DUtil::canUseCopyTextureCHROMIUM(GL_TEXTURE_2D, internalFormat, type, 0));
    return webMediaPlayer()->copyVideoTextureToPlatformTexture(context, texture, internalFormat, type, premultiplyAlpha, flipY);
#endif // MINIBLINK_NOT_IMPLEMENTED
    notImplemented();
    return false;
}

bool HTMLVideoElement::hasAvailableVideoFrame() const
{
    if (!webMediaPlayer())
        return false;

    return webMediaPlayer()->hasVideo() && webMediaPlayer()->readyState() >= WebMediaPlayer::ReadyStateHaveCurrentData;
}

void HTMLVideoElement::webkitEnterFullscreen(ExceptionState& exceptionState)
{
    if (isFullscreen())
        return;

    if (!supportsFullscreen()) {
        exceptionState.throwDOMException(InvalidStateError, "This element does not support fullscreen mode.");
        return;
    }

    enterFullscreen();
}

void HTMLVideoElement::webkitExitFullscreen()
{
    if (isFullscreen())
        exitFullscreen();
}

bool HTMLVideoElement::webkitSupportsFullscreen()
{
    return supportsFullscreen();
}

bool HTMLVideoElement::webkitDisplayingFullscreen()
{
    return isFullscreen();
}

void HTMLVideoElement::didMoveToNewDocument(Document& oldDocument)
{
    if (m_imageLoader)
        m_imageLoader->elementDidMoveToNewDocument();
    HTMLMediaElement::didMoveToNewDocument(oldDocument);
}

unsigned HTMLVideoElement::webkitDecodedFrameCount() const
{
    if (!webMediaPlayer())
        return 0;

    return webMediaPlayer()->decodedFrameCount();
}

unsigned HTMLVideoElement::webkitDroppedFrameCount() const
{
    if (!webMediaPlayer())
        return 0;

    return webMediaPlayer()->droppedFrameCount();
}

KURL HTMLVideoElement::posterImageURL() const
{
    String url = stripLeadingAndTrailingHTMLSpaces(imageSourceURL());
    if (url.isEmpty())
        return KURL();
    return document().completeURL(url);
}

KURL HTMLVideoElement::mediaPlayerPosterURL()
{
    return posterImageURL();
}

PassRefPtr<Image> HTMLVideoElement::getSourceImageForCanvas(SourceImageMode mode, SourceImageStatus* status) const
{
    if (!hasAvailableVideoFrame()) {
        *status = InvalidSourceImageStatus;
        return nullptr;
    }

    IntSize intrinsicSize(videoWidth(), videoHeight());
    OwnPtr<ImageBuffer> imageBuffer = ImageBuffer::create(intrinsicSize);
    if (!imageBuffer) {
        *status = InvalidSourceImageStatus;
        return nullptr;
    }

    paintCurrentFrame(imageBuffer->canvas(), IntRect(IntPoint(0, 0), intrinsicSize), nullptr);

    *status = (mode == CopySourceImageIfVolatile) ? NormalSourceImageStatus : ExternalSourceImageStatus;
    return imageBuffer->copyImage(mode == CopySourceImageIfVolatile ? CopyBackingStore : DontCopyBackingStore, Unscaled);
}

bool HTMLVideoElement::wouldTaintOrigin(SecurityOrigin* destinationSecurityOrigin) const
{
    return !isMediaDataCORSSameOrigin(destinationSecurityOrigin);
}

FloatSize HTMLVideoElement::elementSize() const
{
    return FloatSize(videoWidth(), videoHeight());
}

} // namespace blink
