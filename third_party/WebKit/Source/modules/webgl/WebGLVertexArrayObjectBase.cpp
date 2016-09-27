// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#include "modules/webgl/WebGLVertexArrayObjectBase.h"

#include "modules/webgl/WebGLRenderingContextBase.h"

namespace blink {

WebGLVertexArrayObjectBase::WebGLVertexArrayObjectBase(WebGLRenderingContextBase* ctx, VaoType type)
    : WebGLContextObject(ctx)
    , m_object(0)
    , m_type(type)
    , m_hasEverBeenBound(false)
#if ENABLE(OILPAN)
    , m_destructionInProgress(false)
#endif
    , m_boundElementArrayBuffer(nullptr)
{
    m_vertexAttribState.reserveCapacity(ctx->maxVertexAttribs());

    switch (m_type) {
    case VaoTypeDefault:
        break;
    default:
        m_object = context()->webContext()->createVertexArrayOES();
        break;
    }
}

WebGLVertexArrayObjectBase::~WebGLVertexArrayObjectBase()
{
#if ENABLE(OILPAN)
    m_destructionInProgress = true;
#endif

    // Delete the platform framebuffer resource. Explicit detachment
    // is for the benefit of Oilpan, where this vertex array object
    // isn't detached when it and the WebGLRenderingContextBase object
    // it is registered with are both finalized. Without Oilpan, the
    // object will have been detached.
    //
    // To keep the code regular, the trivial detach()ment is always
    // performed.
    detachAndDeleteObject();
}

void WebGLVertexArrayObjectBase::dispatchDetached(WebGraphicsContext3D* context3d)
{
    if (m_boundElementArrayBuffer)
        m_boundElementArrayBuffer->onDetached(context3d);

    for (size_t i = 0; i < m_vertexAttribState.size(); ++i) {
        VertexAttribState* state = m_vertexAttribState[i].get();
        if (state->bufferBinding)
            state->bufferBinding->onDetached(context3d);
    }
}

void WebGLVertexArrayObjectBase::deleteObjectImpl(WebGraphicsContext3D* context3d)
{
    switch (m_type) {
    case VaoTypeDefault:
        break;
    default:
        context3d->deleteVertexArrayOES(m_object);
        m_object = 0;
        break;
    }

#if ENABLE(OILPAN)
    // These m_boundElementArrayBuffer and m_vertexAttribState heap
    // objects must not be accessed during destruction in the oilpan
    // build. They could have been already finalized. The finalizers
    // of these objects (and their elements) will themselves handle
    // detachment.
    if (!m_destructionInProgress)
        dispatchDetached(context3d);
#else
    dispatchDetached(context3d);
#endif
}

void WebGLVertexArrayObjectBase::setElementArrayBuffer(PassRefPtrWillBeRawPtr<WebGLBuffer> buffer)
{
    if (buffer)
        buffer->onAttached();
    if (m_boundElementArrayBuffer)
        m_boundElementArrayBuffer->onDetached(context()->webContext());
    m_boundElementArrayBuffer = buffer;
}

WebGLVertexArrayObjectBase::VertexAttribState* WebGLVertexArrayObjectBase::getVertexAttribState(size_t index)
{
    ASSERT(index < context()->maxVertexAttribs());
    // Lazily create the vertex attribute states.
    for (size_t i = m_vertexAttribState.size(); i <= index; i++)
        m_vertexAttribState.append(adoptPtrWillBeNoop(new VertexAttribState()));
    return m_vertexAttribState[index].get();
}

void WebGLVertexArrayObjectBase::setVertexAttribState(
    GLuint index, GLsizei bytesPerElement, GLint size, GLenum type, GLboolean normalized, GLsizei stride, GLintptr offset, PassRefPtrWillBeRawPtr<WebGLBuffer> buffer)
{
    GLsizei validatedStride = stride ? stride : bytesPerElement;
    VertexAttribState* state = getVertexAttribState(index);

    if (buffer)
        buffer->onAttached();
    if (state->bufferBinding)
        state->bufferBinding->onDetached(context()->webContext());

    state->bufferBinding = buffer;
    state->bytesPerElement = bytesPerElement;
    state->size = size;
    state->type = type;
    state->normalized = normalized;
    state->stride = validatedStride;
    state->originalStride = stride;
    state->offset = offset;
}

void WebGLVertexArrayObjectBase::unbindBuffer(PassRefPtrWillBeRawPtr<WebGLBuffer> buffer)
{
    if (m_boundElementArrayBuffer == buffer) {
        m_boundElementArrayBuffer->onDetached(context()->webContext());
        m_boundElementArrayBuffer = nullptr;
    }

    for (size_t i = 0; i < m_vertexAttribState.size(); ++i) {
        VertexAttribState* state = m_vertexAttribState[i].get();
        if (state->bufferBinding == buffer) {
            buffer->onDetached(context()->webContext());
            state->bufferBinding = nullptr;
        }
    }
}

void WebGLVertexArrayObjectBase::setVertexAttribDivisor(GLuint index, GLuint divisor)
{
    VertexAttribState* state = getVertexAttribState(index);
    state->divisor = divisor;
}

DEFINE_TRACE(WebGLVertexArrayObjectBase::VertexAttribState)
{
    visitor->trace(bufferBinding);
}

DEFINE_TRACE(WebGLVertexArrayObjectBase)
{
    visitor->trace(m_boundElementArrayBuffer);
    visitor->trace(m_vertexAttribState);
    WebGLContextObject::trace(visitor);
}

} // namespace blink
