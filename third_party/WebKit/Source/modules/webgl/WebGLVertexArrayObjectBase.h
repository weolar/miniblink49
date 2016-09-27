// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebGLVertexArrayObjectBase_h
#define WebGLVertexArrayObjectBase_h

#include "modules/webgl/WebGLBuffer.h"
#include "modules/webgl/WebGLContextObject.h"
#include "platform/heap/Handle.h"
#include "wtf/PassRefPtr.h"

namespace blink {

class WebGLVertexArrayObjectBase : public WebGLContextObject {
public:
    enum VaoType {
        VaoTypeDefault,
        VaoTypeUser,
    };

    ~WebGLVertexArrayObjectBase() override;

    Platform3DObject object() const { return m_object; }

    // Cached values for vertex attrib range checks
    class VertexAttribState final : public NoBaseWillBeGarbageCollected<VertexAttribState> {
    public:
        VertexAttribState()
            : enabled(false)
            , bytesPerElement(0)
            , size(4)
            , type(GL_FLOAT)
            , normalized(false)
            , stride(16)
            , originalStride(0)
            , offset(0)
            , divisor(0)
        {
        }

        DECLARE_TRACE();

        bool enabled;
        RefPtrWillBeMember<WebGLBuffer> bufferBinding;
        GLsizei bytesPerElement;
        GLint size;
        GLenum type;
        bool normalized;
        GLsizei stride;
        GLsizei originalStride;
        GLintptr offset;
        GLuint divisor;
    };

    bool isDefaultObject() const { return m_type == VaoTypeDefault; }

    bool hasEverBeenBound() const { return object() && m_hasEverBeenBound; }
    void setHasEverBeenBound() { m_hasEverBeenBound = true; }

    PassRefPtrWillBeRawPtr<WebGLBuffer> boundElementArrayBuffer() const { return m_boundElementArrayBuffer; }
    void setElementArrayBuffer(PassRefPtrWillBeRawPtr<WebGLBuffer>);

    VertexAttribState* getVertexAttribState(size_t);
    void setVertexAttribState(GLuint, GLsizei, GLint, GLenum, GLboolean, GLsizei, GLintptr, PassRefPtrWillBeRawPtr<WebGLBuffer>);
    void unbindBuffer(PassRefPtrWillBeRawPtr<WebGLBuffer>);
    void setVertexAttribDivisor(GLuint index, GLuint divisor);

    DECLARE_VIRTUAL_TRACE();

protected:
    WebGLVertexArrayObjectBase(WebGLRenderingContextBase*, VaoType);

private:
    void dispatchDetached(WebGraphicsContext3D*);
    bool hasObject() const override { return m_object != 0; }
    void deleteObjectImpl(WebGraphicsContext3D*) override;

    Platform3DObject m_object;

    VaoType m_type;
    bool m_hasEverBeenBound;
#if ENABLE(OILPAN)
    bool m_destructionInProgress;
#endif
    RefPtrWillBeMember<WebGLBuffer> m_boundElementArrayBuffer;
    WillBeHeapVector<OwnPtrWillBeMember<VertexAttribState>> m_vertexAttribState;
};

} // namespace blink

#endif // WebGLVertexArrayObjectBase_h
