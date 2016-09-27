/*
 * Copyright (c) 2013, Google Inc. All rights reserved.
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

#include "config.h"
#include "modules/imagebitmap/WindowImageBitmapFactories.h"

#include "bindings/core/v8/ExceptionState.h"
#include "bindings/core/v8/ScriptPromiseResolver.h"
#include "core/dom/ExceptionCode.h"
#include "core/dom/ExecutionContext.h"
#include "core/fileapi/Blob.h"
#include "core/frame/ImageBitmap.h"
#include "core/frame/LocalDOMWindow.h"
#include "core/html/HTMLCanvasElement.h"
#include "core/html/HTMLImageElement.h"
#include "core/html/HTMLVideoElement.h"
#include "core/html/ImageData.h"
#include "core/workers/WorkerGlobalScope.h"
#include "modules/canvas2d/CanvasRenderingContext2D.h"
#include "platform/SharedBuffer.h"
#include "platform/graphics/BitmapImage.h"
#include "platform/graphics/ImageSource.h"
#include "public/platform/WebSize.h"
#include <v8.h>

namespace blink {

static LayoutSize sizeFor(HTMLImageElement* image)
{
    if (ImageResource* cachedImage = image->cachedImage())
        return cachedImage->imageSizeForLayoutObject(image->layoutObject(), 1.0f); // FIXME: Not sure about this.
    return LayoutSize();
}

static IntSize sizeFor(HTMLVideoElement* video)
{
    if (WebMediaPlayer* webMediaPlayer = video->webMediaPlayer())
        return webMediaPlayer->naturalSize();
    return IntSize();
}

static ScriptPromise fulfillImageBitmap(ScriptState* scriptState, PassRefPtrWillBeRawPtr<ImageBitmap> imageBitmap)
{
    RefPtrWillBeRawPtr<ScriptPromiseResolver> resolver = ScriptPromiseResolver::create(scriptState);
    ScriptPromise promise = resolver->promise();
    if (imageBitmap) {
        resolver->resolve(imageBitmap);
    } else {
        resolver->reject(ScriptValue(scriptState, v8::Null(scriptState->isolate())));
    }
    return promise;
}

ScriptPromise WindowImageBitmapFactories::createImageBitmap(ScriptState* scriptState, EventTarget& eventTarget, HTMLImageElement* image, ExceptionState& exceptionState)
{
    LayoutSize s = sizeFor(image);
    return createImageBitmap(scriptState, eventTarget, image, 0, 0, s.width(), s.height(), exceptionState);
}

ScriptPromise WindowImageBitmapFactories::createImageBitmap(ScriptState* scriptState, EventTarget& eventTarget, HTMLImageElement* image, int sx, int sy, int sw, int sh, ExceptionState& exceptionState)
{
    // This variant does not work in worker threads.
    ASSERT(eventTarget.toDOMWindow());

    if (!image->cachedImage()) {
        exceptionState.throwDOMException(InvalidStateError, "No image can be retrieved from the provided element.");
        return ScriptPromise();
    }
    if (image->cachedImage()->image()->isSVGImage()) {
        exceptionState.throwDOMException(InvalidStateError, "The image element contains an SVG image, which is unsupported.");
        return ScriptPromise();
    }
    if (!sw || !sh) {
        exceptionState.throwDOMException(IndexSizeError, String::format("The source %s provided is 0.", sw ? "height" : "width"));
        return ScriptPromise();
    }
    if (!image->cachedImage()->image()->currentFrameHasSingleSecurityOrigin()) {
        exceptionState.throwSecurityError("The source image contains image data from multiple origins.");
        return ScriptPromise();
    }
    Document* document = eventTarget.toDOMWindow()->document();
    if (!image->cachedImage()->passesAccessControlCheck(document->securityOrigin()) && document->securityOrigin()->taintsCanvas(image->src())) {
        exceptionState.throwSecurityError("Cross-origin access to the source image is denied.");
        return ScriptPromise();
    }
    // FIXME: make ImageBitmap creation asynchronous crbug.com/258082
    return fulfillImageBitmap(scriptState, ImageBitmap::create(image, IntRect(sx, sy, sw, sh)));
}

ScriptPromise WindowImageBitmapFactories::createImageBitmap(ScriptState* scriptState, EventTarget& eventTarget, HTMLVideoElement* video, ExceptionState& exceptionState)
{
    IntSize s = sizeFor(video);
    return createImageBitmap(scriptState, eventTarget, video, 0, 0, s.width(), s.height(), exceptionState);
}

ScriptPromise WindowImageBitmapFactories::createImageBitmap(ScriptState* scriptState, EventTarget& eventTarget, HTMLVideoElement* video, int sx, int sy, int sw, int sh, ExceptionState& exceptionState)
{
    // This variant does not work in worker threads.
    ASSERT(eventTarget.toDOMWindow());

    if (video->networkState() == HTMLMediaElement::NETWORK_EMPTY) {
        exceptionState.throwDOMException(InvalidStateError, "The provided element has not retrieved data.");
        return ScriptPromise();
    }
    if (video->readyState() <= HTMLMediaElement::HAVE_METADATA) {
        exceptionState.throwDOMException(InvalidStateError, "The provided element's player has no current data.");
        return ScriptPromise();
    }
    if (!sw || !sh) {
        exceptionState.throwDOMException(IndexSizeError, String::format("The source %s provided is 0.", sw ? "height" : "width"));
        return ScriptPromise();
    }
    if (!video->hasSingleSecurityOrigin()) {
        exceptionState.throwSecurityError("The source video contains image data from multiple origins.");
        return ScriptPromise();
    }
    if (!video->webMediaPlayer()->didPassCORSAccessCheck()
        && eventTarget.toDOMWindow()->document()->securityOrigin()->taintsCanvas(video->currentSrc())) {
        exceptionState.throwSecurityError("Cross-origin access to the source video is denied.");
        return ScriptPromise();
    }
    // FIXME: make ImageBitmap creation asynchronous crbug.com/258082
    return fulfillImageBitmap(scriptState, ImageBitmap::create(video, IntRect(sx, sy, sw, sh)));
}

ScriptPromise WindowImageBitmapFactories::createImageBitmap(ScriptState* scriptState, EventTarget& eventTarget, CanvasRenderingContext2D* context, ExceptionState& exceptionState)
{
    return createImageBitmap(scriptState, eventTarget, context->canvas(), exceptionState);
}

ScriptPromise WindowImageBitmapFactories::createImageBitmap(ScriptState* scriptState, EventTarget& eventTarget, CanvasRenderingContext2D* context, int sx, int sy, int sw, int sh, ExceptionState& exceptionState)
{
    return createImageBitmap(scriptState, eventTarget, context->canvas(), sx, sy, sw, sh, exceptionState);
}

ScriptPromise WindowImageBitmapFactories::createImageBitmap(ScriptState* scriptState, EventTarget& eventTarget, HTMLCanvasElement* canvas, ExceptionState& exceptionState)
{
    return createImageBitmap(scriptState, eventTarget, canvas, 0, 0, canvas->width(), canvas->height(), exceptionState);
}

ScriptPromise WindowImageBitmapFactories::createImageBitmap(ScriptState* scriptState, EventTarget& eventTarget, HTMLCanvasElement* canvas, int sx, int sy, int sw, int sh, ExceptionState& exceptionState)
{
    // This variant does not work in worker threads.
    ASSERT(eventTarget.toDOMWindow());

    if (!canvas->originClean()) {
        exceptionState.throwSecurityError("The canvas element provided is tainted with cross-origin data.");
        return ScriptPromise();
    }
    if (!sw || !sh) {
        exceptionState.throwDOMException(IndexSizeError, String::format("The source %s provided is 0.", sw ? "height" : "width"));
        return ScriptPromise();
    }

    // FIXME: make ImageBitmap creation asynchronous crbug.com/258082
    return fulfillImageBitmap(scriptState, canvas->isPaintable() ? ImageBitmap::create(canvas, IntRect(sx, sy, sw, sh)) : nullptr);
}

} // namespace blink
