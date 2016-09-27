/*
 * Copyright (C) 2006, 2007, 2009, 2010, 2011, 2012 Apple Inc. All rights reserved.
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

#ifndef CanvasRenderingContext2D_h
#define CanvasRenderingContext2D_h

#include "bindings/core/v8/ScriptWrappable.h"
#include "bindings/modules/v8/UnionTypesModules.h"
#include "core/html/canvas/CanvasContextCreationAttributes.h"
#include "core/html/canvas/CanvasRenderingContext.h"
#include "core/html/canvas/CanvasRenderingContextFactory.h"
#include "modules/ModulesExport.h"
#include "modules/canvas2d/Canvas2DContextAttributes.h"
#include "modules/canvas2d/CanvasPathMethods.h"
#include "modules/canvas2d/CanvasRenderingContext2DState.h"
#include "platform/graphics/GraphicsTypes.h"
#include "wtf/Vector.h"
#include "wtf/text/WTFString.h"

namespace blink { class WebLayer; }

namespace blink {

class CanvasImageSource;
class Element;
class ExceptionState;
class FloatRect;
class FloatSize;
class Font;
class FontMetrics;
class HitRegion;
class HitRegionOptions;
class HitRegionManager;
class ImageData;
class Path2D;
class SVGMatrixTearOff;
class TextMetrics;

typedef HTMLImageElementOrHTMLVideoElementOrHTMLCanvasElementOrImageBitmap CanvasImageSourceUnion;

class MODULES_EXPORT CanvasRenderingContext2D final : public CanvasRenderingContext, public CanvasPathMethods {
    DEFINE_WRAPPERTYPEINFO();
public:
    class Factory : public CanvasRenderingContextFactory {
        WTF_MAKE_NONCOPYABLE(Factory);
    public:
        Factory() {}
        ~Factory() override {}

        PassOwnPtrWillBeRawPtr<CanvasRenderingContext> create(HTMLCanvasElement* canvas, const CanvasContextCreationAttributes& attrs, Document& document) override
        {
            return adoptPtrWillBeNoop(new CanvasRenderingContext2D(canvas, attrs, document));
        }
        CanvasRenderingContext::ContextType contextType() const override { return CanvasRenderingContext::Context2d; }
        void onError(HTMLCanvasElement*, const String& error) override { }
    };

    ~CanvasRenderingContext2D() override;

    void strokeStyle(StringOrCanvasGradientOrCanvasPattern&) const;
    void setStrokeStyle(const StringOrCanvasGradientOrCanvasPattern&);

    void fillStyle(StringOrCanvasGradientOrCanvasPattern&) const;
    void setFillStyle(const StringOrCanvasGradientOrCanvasPattern&);

    float lineWidth() const;
    void setLineWidth(float);

    String lineCap() const;
    void setLineCap(const String&);

    String lineJoin() const;
    void setLineJoin(const String&);

    float miterLimit() const;
    void setMiterLimit(float);

    const Vector<float>& getLineDash() const;
    void setLineDash(const Vector<float>&);

    float lineDashOffset() const;
    void setLineDashOffset(float);

    float shadowOffsetX() const;
    void setShadowOffsetX(float);

    float shadowOffsetY() const;
    void setShadowOffsetY(float);

    float shadowBlur() const;
    void setShadowBlur(float);

    String shadowColor() const;
    void setShadowColor(const String&);

    float globalAlpha() const;
    void setGlobalAlpha(float);

    bool isContextLost() const override;

    bool shouldAntialias() const;
    void setShouldAntialias(bool) override;

    String globalCompositeOperation() const;
    void setGlobalCompositeOperation(const String&);

    String filter() const;
    void setFilter(const String&);

    void save();
    void restore();

    PassRefPtrWillBeRawPtr<SVGMatrixTearOff> currentTransform() const;
    void setCurrentTransform(PassRefPtrWillBeRawPtr<SVGMatrixTearOff>);

    void scale(float sx, float sy);
    void rotate(float angleInRadians);
    void translate(float tx, float ty);
    void transform(float m11, float m12, float m21, float m22, float dx, float dy);
    void setTransform(float m11, float m12, float m21, float m22, float dx, float dy);
    void resetTransform();

    void beginPath();

    void fill(const String& winding = "nonzero");
    void fill(Path2D*, const String& winding = "nonzero");
    void stroke();
    void stroke(Path2D*);
    void clip(const String& winding = "nonzero");
    void clip(Path2D*, const String& winding = "nonzero");

    bool isPointInPath(const float x, const float y, const String& winding = "nonzero");
    bool isPointInPath(Path2D*, const float x, const float y, const String& winding = "nonzero");
    bool isPointInStroke(const float x, const float y);
    bool isPointInStroke(Path2D*, const float x, const float y);

    void scrollPathIntoView();
    void scrollPathIntoView(Path2D*);

    void clearRect(float x, float y, float width, float height) override;
    void fillRect(float x, float y, float width, float height);
    void strokeRect(float x, float y, float width, float height);

    void drawImage(const CanvasImageSourceUnion&, float x, float y, ExceptionState&);
    void drawImage(const CanvasImageSourceUnion&, float x, float y, float width, float height, ExceptionState&);
    void drawImage(const CanvasImageSourceUnion&, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, ExceptionState&);
    void drawImage(CanvasImageSource*, float sx, float sy, float sw, float sh, float dx, float dy, float dw, float dh, ExceptionState&);

    CanvasGradient* createLinearGradient(float x0, float y0, float x1, float y1);
    CanvasGradient* createRadialGradient(float x0, float y0, float r0, float x1, float y1, float r1, ExceptionState&);
    CanvasPattern* createPattern(const CanvasImageSourceUnion&, const String& repetitionType, ExceptionState&);

    ImageData* createImageData(ImageData*) const;
    ImageData* createImageData(float width, float height, ExceptionState&) const;
    ImageData* getImageData(float sx, float sy, float sw, float sh, ExceptionState&) const;
    void putImageData(ImageData*, float dx, float dy);
    void putImageData(ImageData*, float dx, float dy, float dirtyX, float dirtyY, float dirtyWidth, float dirtyHeight);

    void reset() override;

    String font() const;
    void setFont(const String&) override;

    String textAlign() const;
    void setTextAlign(const String&);

    String textBaseline() const;
    void setTextBaseline(const String&);

    String direction() const;
    void setDirection(const String&);

    void fillText(const String& text, float x, float y);
    void fillText(const String& text, float x, float y, float maxWidth);
    void strokeText(const String& text, float x, float y);
    void strokeText(const String& text, float x, float y, float maxWidth);
    PassRefPtrWillBeRawPtr<TextMetrics> measureText(const String& text);

    bool imageSmoothingEnabled() const;
    void setImageSmoothingEnabled(bool);

    void getContextAttributes(Canvas2DContextAttributes&) const;

    void drawFocusIfNeeded(Element*);
    void drawFocusIfNeeded(Path2D*, Element*);

    void addHitRegion(const HitRegionOptions&, ExceptionState&);
    void removeHitRegion(const String& id);
    void clearHitRegions();
    HitRegion* hitRegionAtPoint(const LayoutPoint&);
    unsigned hitRegionsCount() const override;

    void loseContext(LostContextMode) override;
    void didSetSurfaceSize() override;

    void restoreCanvasMatrixClipStack() override;

private:
    friend class CanvasRenderingContext2DAutoRestoreSkCanvas;

    CanvasRenderingContext2D(HTMLCanvasElement*, const CanvasContextCreationAttributes& attrs, Document&);

    CanvasRenderingContext2DState& modifiableState();
    const CanvasRenderingContext2DState& state() const { return *m_stateStack.last(); }

    void setShadow(const FloatSize& offset, float blur, RGBA32 color);

    void dispatchContextLostEvent(Timer<CanvasRenderingContext2D>*);
    void dispatchContextRestoredEvent(Timer<CanvasRenderingContext2D>*);
    void tryRestoreContextEvent(Timer<CanvasRenderingContext2D>*);

    bool computeDirtyRect(const FloatRect& localBounds, SkIRect*);
    bool computeDirtyRect(const FloatRect& localBounds, const SkIRect& transformedClipBounds, SkIRect*);
    void didDraw(const SkIRect&);

    SkCanvas* drawingCanvas() const;

    void unwindStateStack();
    void realizeSaves();

    bool shouldDrawImageAntialiased(const FloatRect& destRect) const;

    template<typename DrawFunc, typename ContainsFunc>
    bool draw(const DrawFunc&, const ContainsFunc&, const SkRect& bounds, CanvasRenderingContext2DState::PaintType, CanvasRenderingContext2DState::ImageType = CanvasRenderingContext2DState::NoImage);
    void drawPathInternal(const Path&, CanvasRenderingContext2DState::PaintType, SkPath::FillType = SkPath::kWinding_FillType);
    void drawImageInternal(CanvasImageSource*, Image*, const FloatRect& srcRect, const FloatRect& dstRect, const SkPaint*);
    void clipInternal(const Path&, const String& windingRuleString);

    bool isPointInPathInternal(const Path&, const float x, const float y, const String& windingRuleString);
    bool isPointInStrokeInternal(const Path&, const float x, const float y);

    void scrollPathIntoViewInternal(const Path&);

    void drawTextInternal(const String&, float x, float y, CanvasRenderingContext2DState::PaintType, float* maxWidth = nullptr);

    const Font& accessFont();
    int getFontBaseline(const FontMetrics&) const;

    void clearCanvas();
    bool rectContainsTransformedRect(const FloatRect&, const SkIRect&) const;

    void inflateStrokeRect(FloatRect&) const;

    template<typename DrawFunc>
    void compositedDraw(const DrawFunc&, CanvasRenderingContext2DState::PaintType, CanvasRenderingContext2DState::ImageType);

    void drawFocusIfNeededInternal(const Path&, Element*);
    bool focusRingCallIsValid(const Path&, Element*);
    void drawFocusRing(const Path&);
    void updateFocusRingElementAccessibility(const Path&, Element*);

    void validateStateStack();

    enum DrawType {
        ClipFill, // Fill that is already known to cover the current clip
        UntransformedUnclippedFill
    };

    void checkOverdraw(const SkRect&, const SkPaint*, CanvasRenderingContext2DState::ImageType, DrawType);

    CanvasRenderingContext::ContextType contextType() const override { return CanvasRenderingContext::Context2d; }
    bool is2d() const override { return true; }
    bool isAccelerated() const override;
    bool hasAlpha() const override { return m_hasAlpha; }
    void setIsHidden(bool) override;
    void stop() final;
    DECLARE_VIRTUAL_TRACE();

    virtual bool isTransformInvertible() const;

    WebLayer* platformLayer() const override;

    WillBeHeapVector<OwnPtrWillBeMember<CanvasRenderingContext2DState>> m_stateStack;
    PersistentWillBeMember<HitRegionManager> m_hitRegionManager;
    AntiAliasingMode m_clipAntialiasing;
    bool m_hasAlpha;
    LostContextMode m_contextLostMode;
    bool m_contextRestorable;
    unsigned m_tryRestoreContextAttemptCount;
    Timer<CanvasRenderingContext2D> m_dispatchContextLostEventTimer;
    Timer<CanvasRenderingContext2D> m_dispatchContextRestoredEventTimer;
    Timer<CanvasRenderingContext2D> m_tryRestoreContextEventTimer;
};

DEFINE_TYPE_CASTS(CanvasRenderingContext2D, CanvasRenderingContext, context, context->is2d(), context.is2d());

} // namespace blink

#endif // CanvasRenderingContext2D_h
