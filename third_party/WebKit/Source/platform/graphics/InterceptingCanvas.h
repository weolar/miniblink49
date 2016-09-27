/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
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

#ifndef InterceptingCanvas_h
#define InterceptingCanvas_h

#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkPicture.h"
#include "wtf/Assertions.h"
#include "wtf/Noncopyable.h"

namespace blink {

class InterceptingCanvasBase : public SkCanvas {
    WTF_MAKE_NONCOPYABLE(InterceptingCanvasBase);
public:
    template<typename DerivedCanvas> class CanvasInterceptorBase {
        WTF_MAKE_NONCOPYABLE(CanvasInterceptorBase);
    protected:
        CanvasInterceptorBase(InterceptingCanvasBase* canvas)
            : m_canvas(canvas)
        {
            ++m_canvas->m_callNestingDepth;
        }

        ~CanvasInterceptorBase()
        {
            ASSERT(m_canvas->m_callNestingDepth > 0);
            if (!--m_canvas->m_callNestingDepth)
                m_canvas->m_callCount++;
        }

        DerivedCanvas* canvas() { return static_cast<DerivedCanvas*>(m_canvas); }
        bool topLevelCall() const { return m_canvas->callNestingDepth() == 1; }
        InterceptingCanvasBase* m_canvas;
    };

    void resetStepCount() { m_callCount = 0; }

protected:
    explicit InterceptingCanvasBase(SkBitmap bitmap) : SkCanvas(bitmap), m_callNestingDepth(0), m_callCount(0) { }
    InterceptingCanvasBase(int width, int height) : SkCanvas(width, height), m_callNestingDepth(0), m_callCount(0) { }

    void unrollDrawPicture(const SkPicture*, const SkMatrix*, const SkPaint*, SkPicture::AbortCallback*);

    void onDrawPaint(const SkPaint&) override = 0;
    void onDrawPoints(PointMode, size_t count, const SkPoint pts[], const SkPaint&) override = 0;
    void onDrawRect(const SkRect&, const SkPaint&) override = 0;
    void onDrawOval(const SkRect&, const SkPaint&) override = 0;
    void onDrawRRect(const SkRRect&, const SkPaint&) override = 0;
    void onDrawPath(const SkPath&, const SkPaint&) override = 0;
    void onDrawBitmap(const SkBitmap&, SkScalar left, SkScalar top, const SkPaint*) override = 0;
    void onDrawBitmapRect(const SkBitmap&, const SkRect* src, const SkRect& dst, const SkPaint*, DrawBitmapRectFlags) override = 0;
    void onDrawBitmapNine(const SkBitmap&, const SkIRect& center, const SkRect& dst, const SkPaint*) override = 0;
    void onDrawImage(const SkImage*, SkScalar, SkScalar, const SkPaint*) override = 0;
    void onDrawImageRect(const SkImage*, const SkRect* src, const SkRect& dst, const SkPaint*) override = 0;
    void onDrawSprite(const SkBitmap&, int left, int top, const SkPaint*) override = 0;
    void onDrawVertices(VertexMode vmode, int vertexCount, const SkPoint vertices[], const SkPoint texs[],
        const SkColor colors[], SkXfermode* xmode, const uint16_t indices[], int indexCount, const SkPaint&) override = 0;

    void onDrawDRRect(const SkRRect& outer, const SkRRect& inner, const SkPaint&) override = 0;
    void onDrawText(const void* text, size_t byteLength, SkScalar x, SkScalar y, const SkPaint&) override = 0;
    void onDrawPosText(const void* text, size_t byteLength, const SkPoint pos[], const SkPaint&) override = 0;
    void onDrawPosTextH(const void* text, size_t byteLength, const SkScalar xpos[], SkScalar constY, const SkPaint&) override = 0;
    void onDrawTextOnPath(const void* text, size_t byteLength, const SkPath&, const SkMatrix*, const SkPaint&) override = 0;
    void onDrawTextBlob(const SkTextBlob*, SkScalar x, SkScalar y, const SkPaint&) override = 0;
    void onClipRect(const SkRect&, SkRegion::Op, ClipEdgeStyle) override = 0;
    void onClipRRect(const SkRRect&, SkRegion::Op, ClipEdgeStyle) override = 0;
    void onClipPath(const SkPath&, SkRegion::Op, ClipEdgeStyle) override = 0;
    void onClipRegion(const SkRegion&, SkRegion::Op) override = 0;
    void onDrawPicture(const SkPicture*, const SkMatrix*, const SkPaint*) override = 0;
    void didSetMatrix(const SkMatrix&) override = 0;
    void didConcat(const SkMatrix&) override = 0;
    void willSave() override = 0;
    SaveLayerStrategy willSaveLayer(const SkRect* bounds, const SkPaint*, SaveFlags) override = 0;
    void willRestore() override = 0;

    unsigned callNestingDepth() const { return m_callNestingDepth; }
    unsigned callCount() const { return m_callCount; }

private:
    unsigned m_callNestingDepth;
    unsigned m_callCount;
};

template<typename DerivedCanvas> class CanvasInterceptor { };

template<typename DerivedCanvas, typename Interceptor = CanvasInterceptor<DerivedCanvas>>
    class InterceptingCanvas : public InterceptingCanvasBase {
protected:
    explicit InterceptingCanvas(SkBitmap bitmap) : InterceptingCanvasBase(bitmap) { }
    InterceptingCanvas(int width, int height) : InterceptingCanvasBase(width, height) { }

    void onDrawPaint(const SkPaint& paint) override
    {
        Interceptor interceptor(this);
        this->SkCanvas::onDrawPaint(paint);
    }

    void onDrawPoints(PointMode mode, size_t count, const SkPoint pts[], const SkPaint& paint) override
    {
        Interceptor interceptor(this);
        this->SkCanvas::onDrawPoints(mode, count, pts, paint);
    }

    void onDrawRect(const SkRect& rect, const SkPaint& paint) override
    {
        Interceptor interceptor(this);
        this->SkCanvas::onDrawRect(rect, paint);
    }

    void onDrawOval(const SkRect& rect, const SkPaint& paint) override
    {
        Interceptor interceptor(this);
        this->SkCanvas::onDrawOval(rect, paint);
    }

    void onDrawRRect(const SkRRect& rrect, const SkPaint& paint) override
    {
        Interceptor interceptor(this);
        this->SkCanvas::onDrawRRect(rrect, paint);
    }

    void onDrawPath(const SkPath& path, const SkPaint& paint) override
    {
        Interceptor interceptor(this);
        this->SkCanvas::onDrawPath(path, paint);
    }

    void onDrawBitmap(const SkBitmap& bitmap, SkScalar left, SkScalar top, const SkPaint* paint) override
    {
        Interceptor interceptor(this);
        this->SkCanvas::onDrawBitmap(bitmap, left, top, paint);
    }

    void onDrawBitmapRect(const SkBitmap& bitmap, const SkRect* src, const SkRect& dst, const SkPaint* paint, DrawBitmapRectFlags flags) override
    {
        Interceptor interceptor(this);
        this->SkCanvas::onDrawBitmapRect(bitmap, src, dst, paint, flags);
    }

    void onDrawBitmapNine(const SkBitmap& bitmap, const SkIRect& center, const SkRect& dst, const SkPaint* paint) override
    {
        Interceptor interceptor(this);
        this->SkCanvas::onDrawBitmapNine(bitmap, center, dst, paint);
    }

    void onDrawImage(const SkImage* image, SkScalar x, SkScalar y, const SkPaint* paint) override
    {
        Interceptor interceptor(this);
        this->SkCanvas::onDrawImage(image, x, y, paint);
    }

    void onDrawImageRect(const SkImage* image, const SkRect* src, const SkRect& dst, const SkPaint* paint) override
    {
        Interceptor interceptor(this);
        this->SkCanvas::onDrawImageRect(image, src, dst, paint);
    }

    void onDrawSprite(const SkBitmap& bitmap, int left, int top, const SkPaint* paint) override
    {
        Interceptor interceptor(this);
        this->SkCanvas::onDrawSprite(bitmap, left, top, paint);
    }

    void onDrawVertices(VertexMode vmode, int vertexCount, const SkPoint vertices[], const SkPoint texs[],
        const SkColor colors[], SkXfermode* xmode, const uint16_t indices[], int indexCount, const SkPaint& paint) override
    {
        Interceptor interceptor(this);
        this->SkCanvas::onDrawVertices(vmode, vertexCount, vertices, texs, colors, xmode, indices, indexCount, paint);
    }

    void onDrawDRRect(const SkRRect& outer, const SkRRect& inner, const SkPaint& paint) override
    {
        Interceptor interceptor(this);
        this->SkCanvas::onDrawDRRect(outer, inner, paint);
    }

    void onDrawText(const void* text, size_t byteLength, SkScalar x, SkScalar y, const SkPaint& paint) override
    {
        Interceptor interceptor(this);
        this->SkCanvas::onDrawText(text, byteLength, x, y, paint);
    }

    void onDrawPosText(const void* text, size_t byteLength, const SkPoint pos[], const SkPaint& paint) override
    {
        Interceptor interceptor(this);
        this->SkCanvas::onDrawPosText(text, byteLength, pos, paint);
    }

    void onDrawPosTextH(const void* text, size_t byteLength, const SkScalar xpos[], SkScalar constY, const SkPaint& paint) override
    {
        Interceptor interceptor(this);
        this->SkCanvas::onDrawPosTextH(text, byteLength, xpos, constY, paint);
    }

    void onDrawTextOnPath(const void* text, size_t byteLength, const SkPath& path, const SkMatrix* matrix, const SkPaint& paint) override
    {
        Interceptor interceptor(this);
        this->SkCanvas::onDrawTextOnPath(text, byteLength, path, matrix, paint);
    }

    void onDrawTextBlob(const SkTextBlob *blob, SkScalar x, SkScalar y, const SkPaint& paint) override
    {
        Interceptor interceptor(this);
        this->SkCanvas::onDrawTextBlob(blob, x, y, paint);
    }

    void onClipRect(const SkRect& rect, SkRegion::Op op, ClipEdgeStyle edgeStyle) override
    {
        Interceptor interceptor(this);
        this->SkCanvas::onClipRect(rect, op, edgeStyle);
    }

    void onClipRRect(const SkRRect& rrect, SkRegion::Op op, ClipEdgeStyle edgeStyle) override
    {
        Interceptor interceptor(this);
        this->SkCanvas::onClipRRect(rrect, op, edgeStyle);
    }

    void onClipPath(const SkPath& path, SkRegion::Op op, ClipEdgeStyle edgeStyle) override
    {
        Interceptor interceptor(this);
        this->SkCanvas::onClipPath(path, op, edgeStyle);
    }

    void onClipRegion(const SkRegion& region, SkRegion::Op op) override
    {
        Interceptor interceptor(this);
        this->SkCanvas::onClipRegion(region, op);
    }

    void onDrawPicture(const SkPicture* picture, const SkMatrix* matrix, const SkPaint* paint) override
    {
        this->unrollDrawPicture(picture, matrix, paint, nullptr);
    }

    void didSetMatrix(const SkMatrix& matrix) override
    {
        Interceptor interceptor(this);
        this->SkCanvas::didSetMatrix(matrix);
    }

    void didConcat(const SkMatrix& matrix) override
    {
        Interceptor interceptor(this);
        this->SkCanvas::didConcat(matrix);
    }

    void willSave() override
    {
        Interceptor interceptor(this);
        this->SkCanvas::willSave();
    }

    SkCanvas::SaveLayerStrategy willSaveLayer(const SkRect* bounds, const SkPaint* paint, SaveFlags flags) override
    {
        Interceptor interceptor(this);
        return this->SkCanvas::willSaveLayer(bounds, paint, flags);
    }

    void willRestore() override
    {
        Interceptor interceptor(this);
        this->SkCanvas::willRestore();
    }
};

} // namespace blink

#endif // InterceptingCanvas_h
