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

#ifndef WebGLFramebuffer_h
#define WebGLFramebuffer_h

#include "modules/webgl/WebGLContextObject.h"
#include "modules/webgl/WebGLSharedObject.h"
#include "wtf/PassRefPtr.h"
#include "wtf/RefCounted.h"

namespace blink {

class WebGLRenderbuffer;
class WebGLTexture;

class WebGLFramebuffer final : public WebGLContextObject {
    DEFINE_WRAPPERTYPEINFO();
public:
    class WebGLAttachment : public RefCountedWillBeGarbageCollectedFinalized<WebGLAttachment> {
    public:
        virtual ~WebGLAttachment();

        virtual GLsizei width() const = 0;
        virtual GLsizei height() const = 0;
        virtual GLenum format() const = 0;
        // For texture attachment, type() returns the type of the attached texture.
        // For renderbuffer attachment, the type of the renderbuffer may vary with GL implementation.
        // To avoid confusion, it would be better to not implement type() for renderbuffer attachment and
        // we should always use the internalformat of the renderbuffer and avoid using type() API.
        virtual GLenum type() const = 0;
        virtual WebGLSharedObject* object() const = 0;
        virtual bool isSharedObject(WebGLSharedObject*) const = 0;
        virtual bool valid() const = 0;
        virtual void onDetached(WebGraphicsContext3D*) = 0;
        virtual void attach(WebGraphicsContext3D*, GLenum target, GLenum attachment) = 0;
        virtual void unattach(WebGraphicsContext3D*, GLenum target, GLenum attachment) = 0;

        DEFINE_INLINE_VIRTUAL_TRACE() { }

    protected:
        WebGLAttachment();
    };

    ~WebGLFramebuffer() override;

    static PassRefPtrWillBeRawPtr<WebGLFramebuffer> create(WebGLRenderingContextBase*);

    Platform3DObject object() const { return m_object; }

    void setAttachmentForBoundFramebuffer(GLenum target, GLenum attachment, GLenum texTarget, WebGLTexture*, GLint level);
    void setAttachmentForBoundFramebuffer(GLenum target, GLenum attachment, WebGLRenderbuffer*);
    // If an object is attached to the currently bound framebuffer, remove it.
    void removeAttachmentFromBoundFramebuffer(GLenum target, WebGLSharedObject*);
    // If a given attachment point for the currently bound framebuffer is not null, remove the attached object.
    void removeAttachmentFromBoundFramebuffer(GLenum target, GLenum attachment);
    WebGLSharedObject* getAttachmentObject(GLenum) const;

    GLenum colorBufferFormat() const;

    // This should always be called before drawArray, drawElements, clear,
    // readPixels, copyTexImage2D, copyTexSubImage2D if this framebuffer is
    // currently bound.
    // Return false if the framebuffer is incomplete.
    bool onAccess(WebGraphicsContext3D*, const char** reason);

    // Software version of glCheckFramebufferStatus(), except that when
    // FRAMEBUFFER_COMPLETE is returned, it is still possible for
    // glCheckFramebufferStatus() to return FRAMEBUFFER_UNSUPPORTED,
    // depending on hardware implementation.
    GLenum checkStatus(const char** reason) const;

    bool hasEverBeenBound() const { return object() && m_hasEverBeenBound; }

    void setHasEverBeenBound() { m_hasEverBeenBound = true; }

    bool hasStencilBuffer() const;

    // Wrapper for drawBuffersEXT/drawBuffersARB to work around a driver bug.
    void drawBuffers(const Vector<GLenum>& bufs);

    GLenum getDrawBuffer(GLenum);

    void readBuffer(const GLenum colorBuffer) { m_readBuffer = colorBuffer; }

    GLenum getReadBuffer() const { return m_readBuffer; }

    DECLARE_VIRTUAL_TRACE();

protected:
    explicit WebGLFramebuffer(WebGLRenderingContextBase*);

    bool hasObject() const override { return m_object != 0; }
    void deleteObjectImpl(WebGraphicsContext3D*) override;

private:
    WebGLAttachment* getAttachment(GLenum attachment) const;
    bool isAttachmentComplete(WebGLAttachment* attachedObject, GLenum attachment, const char** reason) const;

    // Check if the framebuffer is currently bound.
    bool isBound(GLenum target) const;

    // attach 'attachment' at 'attachmentPoint'.
    void attach(GLenum target, GLenum attachment, GLenum attachmentPoint);

    // Check if a new drawBuffers call should be issued. This is called when we add or remove an attachment.
    void drawBuffersIfNecessary(bool force);

    Platform3DObject m_object;

    typedef WillBeHeapHashMap<GLenum, RefPtrWillBeMember<WebGLAttachment>> AttachmentMap;

    AttachmentMap m_attachments;

    bool m_hasEverBeenBound;

    Vector<GLenum> m_drawBuffers;
    Vector<GLenum> m_filteredDrawBuffers;

    GLenum m_readBuffer;
};

} // namespace blink

#endif // WebGLFramebuffer_h
