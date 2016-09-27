/*
 * Copyright (C) 2009 Apple Inc. All rights reserved.
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

#ifndef CanvasRenderingContext_h
#define CanvasRenderingContext_h

#include "core/CoreExport.h"
#include "core/dom/ActiveDOMObject.h"
#include "core/html/HTMLCanvasElement.h"
#include "platform/heap/Handle.h"
#include "wtf/HashSet.h"
#include "wtf/Noncopyable.h"
#include "wtf/text/StringHash.h"

namespace blink { class WebLayer; }

namespace blink {

class CanvasImageSource;
class HTMLCanvasElement;
class ImageData;

class CORE_EXPORT CanvasRenderingContext : public NoBaseWillBeGarbageCollectedFinalized<CanvasRenderingContext>, public ActiveDOMObject, public ScriptWrappable {
    WTF_MAKE_NONCOPYABLE(CanvasRenderingContext);
    WTF_MAKE_FAST_ALLOCATED_WILL_BE_REMOVED(CanvasRenderingContext);
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(CanvasRenderingContext);
public:
    ~CanvasRenderingContext() override { }

    // A Canvas can either be "2D" or "webgl" but never both. If you request a 2D canvas and the existing
    // context is already 2D, just return that. If the existing context is WebGL, then destroy it
    // before creating a new 2D context. Vice versa when requesting a WebGL canvas. Requesting a
    // context with any other type string will destroy any existing context.
    enum ContextType {
        // Do not change assigned numbers of existing items: add new features to the end of the list.
        Context2d = 0,
        ContextExperimentalWebgl = 2,
        ContextWebgl = 3,
        ContextWebgl2 = 4,
        ContextTypeCount,
    };

    static ContextType contextTypeFromId(const String& id);
    static ContextType resolveContextTypeAliases(ContextType);

#if !ENABLE(OILPAN)
    void ref() { m_canvas->ref(); }
    void deref() { m_canvas->deref(); }
#endif

    HTMLCanvasElement* canvas() const { return m_canvas; }

    virtual ContextType contextType() const = 0;
    virtual bool isAccelerated() const { return false; }
    virtual bool hasAlpha() const { return true; }
    virtual void setIsHidden(bool) = 0;
    virtual bool isContextLost() const { return true; }

    // Return true if the content is updated.
    virtual bool paintRenderingResultsToCanvas(SourceDrawingBuffer) { return false; }

    virtual WebLayer* platformLayer() const { return nullptr; }

    enum LostContextMode {
        NotLostContext,

        // Lost context occurred at the graphics system level.
        RealLostContext,

        // Lost context provoked by WEBGL_lose_context.
        WebGLLoseContextLostContext,

        // Lost context occurred due to internal implementation reasons.
        SyntheticLostContext,
    };
    virtual void loseContext(LostContextMode) { }

    // Canvas2D-specific interface
    virtual bool is2d() const { return false; }
    virtual void restoreCanvasMatrixClipStack() { }
    virtual void reset() { }
    virtual void clearRect(float x, float y, float width, float height) { }
    virtual void didSetSurfaceSize() { }
    virtual void setShouldAntialias(bool) { }
    virtual unsigned hitRegionsCount() const { return 0; }
    virtual void setFont(const String&) { }

    // WebGL-specific interface
    virtual bool is3d() const { return false; }
    virtual void setFilterQuality(SkFilterQuality) { ASSERT_NOT_REACHED(); }
    virtual void reshape(int width, int height) { ASSERT_NOT_REACHED(); }
    virtual void markLayerComposited() { ASSERT_NOT_REACHED(); }
    virtual ImageData* paintRenderingResultsToImageData(SourceDrawingBuffer) { ASSERT_NOT_REACHED(); return nullptr; }
    virtual int externallyAllocatedBytesPerPixel() { ASSERT_NOT_REACHED(); return 0; }

    bool wouldTaintOrigin(CanvasImageSource*);
    void didMoveToNewDocument(Document*);

protected:
    CanvasRenderingContext(HTMLCanvasElement*);
    DECLARE_VIRTUAL_TRACE();

    // ActiveDOMObject notifications
    bool hasPendingActivity() const final;
    void stop() override = 0;

private:
    RawPtrWillBeMember<HTMLCanvasElement> m_canvas;
    HashSet<String> m_cleanURLs;
    HashSet<String> m_dirtyURLs;
};

} // namespace blink

#endif
