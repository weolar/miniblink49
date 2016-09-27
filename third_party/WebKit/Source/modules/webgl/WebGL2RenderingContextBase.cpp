// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "modules/webgl/WebGL2RenderingContextBase.h"

#include "bindings/modules/v8/WebGLAny.h"
#include "core/html/HTMLCanvasElement.h"
#include "core/html/HTMLImageElement.h"
#include "core/html/HTMLVideoElement.h"
#include "core/html/ImageData.h"
#include "modules/webgl/WebGLActiveInfo.h"
#include "modules/webgl/WebGLBuffer.h"
#include "modules/webgl/WebGLFenceSync.h"
#include "modules/webgl/WebGLFramebuffer.h"
#include "modules/webgl/WebGLProgram.h"
#include "modules/webgl/WebGLQuery.h"
#include "modules/webgl/WebGLRenderbuffer.h"
#include "modules/webgl/WebGLSampler.h"
#include "modules/webgl/WebGLSync.h"
#include "modules/webgl/WebGLTexture.h"
#include "modules/webgl/WebGLTransformFeedback.h"
#include "modules/webgl/WebGLUniformLocation.h"
#include "modules/webgl/WebGLVertexArrayObject.h"

#include "platform/NotImplemented.h"

namespace blink {

namespace {

const GLuint webGLTimeoutIgnored = 0xFFFFFFFF;

WGC3Dsync syncObjectOrZero(const WebGLSync* object)
{
    return object ? object->object() : 0;
}

} // namespace

WebGL2RenderingContextBase::WebGL2RenderingContextBase(HTMLCanvasElement* passedCanvas, PassOwnPtr<WebGraphicsContext3D> context, const WebGLContextAttributes& requestedAttributes)
    : WebGLRenderingContextBase(passedCanvas, context, requestedAttributes)
{

}

WebGL2RenderingContextBase::~WebGL2RenderingContextBase()
{
    m_readFramebufferBinding = nullptr;

    m_boundCopyReadBuffer = nullptr;
    m_boundCopyWriteBuffer = nullptr;
    m_boundPixelPackBuffer = nullptr;
    m_boundPixelUnpackBuffer = nullptr;
    m_boundTransformFeedbackBuffer = nullptr;
    m_boundUniformBuffer = nullptr;

    m_currentBooleanOcclusionQuery = nullptr;
    m_currentTransformFeedbackPrimitivesWrittenQuery = nullptr;
}

void WebGL2RenderingContextBase::initializeNewContext()
{
    ASSERT(!isContextLost());
    ASSERT(drawingBuffer());

    m_readFramebufferBinding = nullptr;

    m_boundCopyReadBuffer = nullptr;
    m_boundCopyWriteBuffer = nullptr;
    m_boundPixelPackBuffer = nullptr;
    m_boundPixelUnpackBuffer = nullptr;
    m_boundTransformFeedbackBuffer = nullptr;
    m_boundUniformBuffer = nullptr;

    m_currentBooleanOcclusionQuery = nullptr;
    m_currentTransformFeedbackPrimitivesWrittenQuery = nullptr;

    m_max3DTextureSize = 0;
    webContext()->getIntegerv(GL_MAX_3D_TEXTURE_SIZE, &m_max3DTextureSize);
    m_max3DTextureLevel = WebGLTexture::computeLevelCount(m_max3DTextureSize, m_max3DTextureSize, m_max3DTextureSize);

    GLint numCombinedTextureImageUnits = 0;
    webContext()->getIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &numCombinedTextureImageUnits);
    m_samplerUnits.clear();
    m_samplerUnits.resize(numCombinedTextureImageUnits);

    WebGLRenderingContextBase::initializeNewContext();
}

void WebGL2RenderingContextBase::copyBufferSubData(GLenum readTarget, GLenum writeTarget, GLintptr readOffset, GLintptr writeOffset, GLsizeiptr size)
{
    if (isContextLost())
        return;

    webContext()->copyBufferSubData(readTarget, writeTarget, readOffset, writeOffset, size);
}

void WebGL2RenderingContextBase::getBufferSubData(GLenum target, GLintptr offset, DOMArrayBuffer* returnedData)
{
    if (isContextLost())
        return;

    void* mappedData = webContext()->mapBufferRange(target, offset, returnedData->byteLength(), GL_MAP_READ_BIT);

    if (!mappedData)
        return;

    memcpy(returnedData->data(), mappedData, returnedData->byteLength());

    webContext()->unmapBuffer(target);
}

void WebGL2RenderingContextBase::blitFramebuffer(GLint srcX0, GLint srcY0, GLint srcX1, GLint srcY1, GLint dstX0, GLint dstY0, GLint dstX1, GLint dstY1, GLbitfield mask, GLenum filter)
{
    if (isContextLost())
        return;

    webContext()->blitFramebufferCHROMIUM(srcX0, srcY0, srcX1, srcY1, dstX0, dstY0, dstX1, dstY1, mask, filter);
}

void WebGL2RenderingContextBase::framebufferTextureLayer(GLenum target, GLenum attachment, const WebGLTexture* texture, GLint level, GLint layer)
{
    if (isContextLost())
        return;

    if (texture && !texture->validate(contextGroup(), this)) {
        synthesizeGLError(GL_INVALID_VALUE, "framebufferTextureLayer", "no texture or texture not from this context");
        return;
    }

    webContext()->framebufferTextureLayer(target, attachment, objectOrZero(texture), level, layer);
}

ScriptValue WebGL2RenderingContextBase::getInternalformatParameter(ScriptState* scriptState, GLenum target, GLenum internalformat, GLenum pname)
{
    if (isContextLost())
        return ScriptValue::createNull(scriptState);

    switch (pname) {
    case GL_SAMPLES:
        {
            GLint length = -1;
            webContext()->getInternalformativ(target, internalformat, GL_NUM_SAMPLE_COUNTS, 1, &length);
            if (length <= 0)
                return WebGLAny(scriptState, DOMInt32Array::create(0));

            scoped_ptr<GLint[]> values(new GLint[length]);
            for (GLint ii = 0; ii < length; ++ii)
                values[ii] = 0;
            webContext()->getInternalformativ(target, internalformat, GL_SAMPLES, length, values.get());
            return WebGLAny(scriptState, DOMInt32Array::create(values.get(), length));
        }
    default:
        synthesizeGLError(GL_INVALID_ENUM, "getInternalformatParameter", "invalid parameter name");
        return ScriptValue::createNull(scriptState);
    }
}

void WebGL2RenderingContextBase::invalidateFramebuffer(GLenum target, Vector<GLenum>& attachments)
{
    if (isContextLost())
        return;

    webContext()->invalidateFramebuffer(target, attachments.size(), attachments.data());
}

void WebGL2RenderingContextBase::invalidateSubFramebuffer(GLenum target, Vector<GLenum>& attachments, GLint x, GLint y, GLsizei width, GLsizei height)
{
    if (isContextLost())
        return;

    webContext()->invalidateSubFramebuffer(target, attachments.size(), attachments.data(), x, y, width, height);
}

void WebGL2RenderingContextBase::readBuffer(GLenum mode)
{
    if (isContextLost())
        return;

    switch (mode) {
    case GL_BACK:
    case GL_NONE:
    case GL_COLOR_ATTACHMENT0:
        break;
    default:
        if (mode > GL_COLOR_ATTACHMENT0
            && mode < static_cast<GLenum>(GL_COLOR_ATTACHMENT0 + maxColorAttachments()))
            break;
        synthesizeGLError(GL_INVALID_ENUM, "readBuffer", "invalid read buffer");
        return;
    }

    WebGLFramebuffer* readFramebufferBinding = getFramebufferBinding(GL_READ_FRAMEBUFFER);
    if (!readFramebufferBinding) {
        ASSERT(drawingBuffer());
        if (mode != GL_BACK && mode != GL_NONE) {
            synthesizeGLError(GL_INVALID_OPERATION, "readBuffer", "invalid read buffer");
            return;
        }
        m_readBufferOfDefaultFramebuffer = mode;
        // translate GL_BACK to GL_COLOR_ATTACHMENT0, because the default
        // framebuffer for WebGL is not fb 0, it is an internal fbo.
        if (mode == GL_BACK)
            mode = GL_COLOR_ATTACHMENT0;
    } else {
        if (mode == GL_BACK) {
            synthesizeGLError(GL_INVALID_OPERATION, "readBuffer", "invalid read buffer");
            return;
        }
        readFramebufferBinding->readBuffer(mode);
    }
    webContext()->readBuffer(mode);
}

void WebGL2RenderingContextBase::renderbufferStorageImpl(
    GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height,
    const char* functionName)
{
    switch (internalformat) {
    case GL_R8UI:
    case GL_R8I:
    case GL_R16UI:
    case GL_R16I:
    case GL_R32UI:
    case GL_R32I:
    case GL_RG8UI:
    case GL_RG8I:
    case GL_RG16UI:
    case GL_RG16I:
    case GL_RG32UI:
    case GL_RG32I:
    case GL_RGBA8UI:
    case GL_RGBA8I:
    case GL_RGB10_A2UI:
    case GL_RGBA16UI:
    case GL_RGBA16I:
    case GL_RGBA32UI:
    case GL_RGBA32I:
        if (samples > 0) {
            synthesizeGLError(GL_INVALID_OPERATION, functionName,
                "for integer formats, samples > 0");
            break;
        }
    case GL_R8:
    case GL_RG8:
    case GL_RGB8:
    case GL_RGB565:
    case GL_RGBA8:
    case GL_SRGB8_ALPHA8:
    case GL_RGB5_A1:
    case GL_RGBA4:
    case GL_RGB10_A2:
    case GL_DEPTH_COMPONENT16:
    case GL_DEPTH_COMPONENT24:
    case GL_DEPTH_COMPONENT32F:
    case GL_DEPTH24_STENCIL8:
    case GL_DEPTH32F_STENCIL8:
    case GL_STENCIL_INDEX8:
        if (!samples) {
            webContext()->renderbufferStorage(target, internalformat, width, height);
        } else {
            webContext()->renderbufferStorageMultisampleCHROMIUM(
                target, samples, internalformat, width, height);
        }
        m_renderbufferBinding->setInternalFormat(internalformat);
        m_renderbufferBinding->setSize(width, height);
        break;
    default:
        synthesizeGLError(GL_INVALID_ENUM, functionName, "invalid internalformat");
        break;
    }
}

void WebGL2RenderingContextBase::renderbufferStorageMultisample(GLenum target, GLsizei samples, GLenum internalformat, GLsizei width, GLsizei height)
{
    const char* functionName = "renderbufferStorageMultisample";
    if (isContextLost())
        return;
    if (target != GL_RENDERBUFFER) {
        synthesizeGLError(GL_INVALID_ENUM, functionName, "invalid target");
        return;
    }
    if (!m_renderbufferBinding || !m_renderbufferBinding->object()) {
        synthesizeGLError(GL_INVALID_OPERATION, functionName, "no bound renderbuffer");
        return;
    }
    if (!validateSize("renderbufferStorage", width, height))
        return;
    if (samples < 0) {
        synthesizeGLError(GL_INVALID_VALUE, functionName, "samples < 0");
        return;
    }
    renderbufferStorageImpl(target, samples, internalformat, width, height, functionName);
    applyStencilTest();
}

/* Texture objects */
bool WebGL2RenderingContextBase::validateTexStorage(const char* functionName, GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, TexStorageType functionType)
{
    if (functionType == TexStorageType2D) {
        if (target != GL_TEXTURE_2D && target != GL_TEXTURE_CUBE_MAP) {
            synthesizeGLError(GL_INVALID_ENUM, functionName, "invalid 2D target");
            return false;
        }
    } else {
        if (target != GL_TEXTURE_3D && target != GL_TEXTURE_2D_ARRAY) {
            synthesizeGLError(GL_INVALID_ENUM, functionName, "invalid 3D target");
            return false;
        }
    }

    WebGLTexture* tex = validateTextureBinding(functionName, target, false);
    if (!tex)
        return false;

    if (tex->isImmutable()) {
        synthesizeGLError(GL_INVALID_OPERATION, functionName, "attempted to modify immutable texture");
        return false;
    }

    if (width <= 0 || height <= 0 || depth <= 0) {
        synthesizeGLError(GL_INVALID_VALUE, functionName, "invalid dimensions");
        return false;
    }

    if (levels <= 0) {
        synthesizeGLError(GL_INVALID_VALUE, functionName, "invalid levels");
        return false;
    }

    if (target == GL_TEXTURE_3D) {
        if (levels > log2(std::max(std::max(width, height), depth)) + 1) {
            synthesizeGLError(GL_INVALID_OPERATION, functionName, "to many levels");
            return false;
        }
    } else {
        if (levels > log2(std::max(width, height)) + 1) {
            synthesizeGLError(GL_INVALID_OPERATION, functionName, "to many levels");
            return false;
        }
    }

    return true;
}

void WebGL2RenderingContextBase::texStorage2D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height)
{
    if (isContextLost() || !validateTexStorage("texStorage2D", target, levels, internalformat, width, height, 1, TexStorageType2D))
        return;

    WebGLTexture* tex = validateTextureBinding("texStorage2D", target, false);
    webContext()->texStorage2DEXT(target, levels, internalformat, width, height);
    tex->setTexStorageInfo(target, levels, internalformat, width, height, 1);
}

void WebGL2RenderingContextBase::texStorage3D(GLenum target, GLsizei levels, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth)
{
    if (isContextLost() || !validateTexStorage("texStorage3D", target, levels, internalformat, width, height, depth, TexStorageType3D))
        return;

    WebGLTexture* tex = validateTextureBinding("texStorage3D", target, false);
    webContext()->texStorage3D(target, levels, internalformat, width, height, depth);
    tex->setTexStorageInfo(target, levels, internalformat, width, height, depth);
}

void WebGL2RenderingContextBase::texImage3D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, DOMArrayBufferView* pixels)
{
    if (isContextLost())
        return;

    void* data = pixels ? pixels->baseAddress() : 0;
    Vector<uint8_t> tempData;
    if (data && (m_unpackFlipY || m_unpackPremultiplyAlpha)) {
        // FIXME: WebGLImageConversion needs to be updated to accept image depth.
        notImplemented();
        return;
    }

    WebGLTexture* tex = validateTextureBinding("texImage3D", target, true);
    if (!tex)
        return;

    if (tex->isImmutable()) {
        synthesizeGLError(GL_INVALID_OPERATION, "texImage3D", "attempted to modify immutable texture");
        return;
    }

    webContext()->texImage3D(target, level, convertTexInternalFormat(internalformat, type), width, height, depth, border, format, type, pixels);
    tex->setLevelInfo(target, level, internalformat, width, height, depth, type);
}

bool WebGL2RenderingContextBase::validateTexSubImage3D(const char* functionName, GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth)
{
    switch (target) {
    case GL_TEXTURE_3D:
    case GL_TEXTURE_2D_ARRAY:
        break;
    default:
        synthesizeGLError(GL_INVALID_ENUM, functionName, "invalid target");
        return false;
    }

    WebGLTexture* tex = validateTextureBinding(functionName, target, false);
    if (!tex)
        return false;

    if (width - xoffset > tex->getWidth(target, level)
        || height - yoffset > tex->getHeight(target, level)
        || depth - zoffset > tex->getDepth(target, level)) {
        synthesizeGLError(GL_INVALID_OPERATION, functionName, "dimensions out of range");
        return false;
    }

    return true;
}

void WebGL2RenderingContextBase::texSubImage3DImpl(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLenum format, GLenum type, Image* image, WebGLImageConversion::ImageHtmlDomSource domSource, bool flipY, bool premultiplyAlpha)
{
    if (!validateTexSubImage3D("texSubImage3D", target, level, xoffset, yoffset, zoffset, image->width(), image->height(), 1))
        return;

    // All calling functions check isContextLost, so a duplicate check is not needed here.
    Vector<uint8_t> data;
    WebGLImageConversion::ImageExtractor imageExtractor(image, domSource, premultiplyAlpha, m_unpackColorspaceConversion == GL_NONE);
    if (!imageExtractor.extractSucceeded()) {
        synthesizeGLError(GL_INVALID_VALUE, "texSubImage3D", "bad image");
        return;
    }
    WebGLImageConversion::DataFormat sourceDataFormat = imageExtractor.imageSourceFormat();
    WebGLImageConversion::AlphaOp alphaOp = imageExtractor.imageAlphaOp();
    const void* imagePixelData = imageExtractor.imagePixelData();

    bool needConversion = true;
    if (type == GL_UNSIGNED_BYTE && sourceDataFormat == WebGLImageConversion::DataFormatRGBA8 && format == GL_RGBA && alphaOp == WebGLImageConversion::AlphaDoNothing && !flipY) {
        needConversion = false;
    } else {
        if (!WebGLImageConversion::packImageData(image, imagePixelData, format, type, flipY, alphaOp, sourceDataFormat, imageExtractor.imageWidth(), imageExtractor.imageHeight(), imageExtractor.imageSourceUnpackAlignment(), data)) {
            synthesizeGLError(GL_INVALID_VALUE, "texSubImage3D", "bad image data");
            return;
        }
    }

    if (m_unpackAlignment != 1)
        webContext()->pixelStorei(GL_UNPACK_ALIGNMENT, 1);
    webContext()->texSubImage3D(target, level, xoffset, yoffset, zoffset, imageExtractor.imageWidth(), imageExtractor.imageHeight(), 1, format, type, needConversion ? data.data() : imagePixelData);
    if (m_unpackAlignment != 1)
        webContext()->pixelStorei(GL_UNPACK_ALIGNMENT, m_unpackAlignment);
}

void WebGL2RenderingContextBase::texSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, DOMArrayBufferView* pixels, ExceptionState& exceptionState)
{
    if (isContextLost() || !pixels || !validateTexSubImage3D("texSubImage3D", target, level, xoffset, yoffset, zoffset, width, height, depth))
        return;

    // FIXME: Ensure pixels is large enough to contain the desired texture dimensions.

    void* data = pixels->baseAddress();
    Vector<uint8_t> tempData;
    bool changeUnpackAlignment = false;
    if (data && (m_unpackFlipY || m_unpackPremultiplyAlpha)) {
        if (!WebGLImageConversion::extractTextureData(width, height, format, type,
            m_unpackAlignment,
            m_unpackFlipY, m_unpackPremultiplyAlpha,
            data,
            tempData))
            return;
        data = tempData.data();
        changeUnpackAlignment = true;
    }
    if (changeUnpackAlignment)
        webContext()->pixelStorei(GL_UNPACK_ALIGNMENT, 1);
    webContext()->texSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, data);
    if (changeUnpackAlignment)
        webContext()->pixelStorei(GL_UNPACK_ALIGNMENT, m_unpackAlignment);
}

void WebGL2RenderingContextBase::texSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLenum format, GLenum type, ImageData* pixels, ExceptionState& exceptionState)
{
    if (isContextLost() || !pixels || !validateTexSubImage3D("texSubImage3D", target, level, xoffset, yoffset, zoffset, pixels->width(), pixels->height(), 1))
        return;

    Vector<uint8_t> data;
    bool needConversion = true;
    // The data from ImageData is always of format RGBA8.
    // No conversion is needed if destination format is RGBA and type is USIGNED_BYTE and no Flip or Premultiply operation is required.
    if (format == GL_RGBA && type == GL_UNSIGNED_BYTE && !m_unpackFlipY && !m_unpackPremultiplyAlpha) {
        needConversion = false;
    } else {
        if (!WebGLImageConversion::extractImageData(pixels->data()->data(), pixels->size(), format, type, m_unpackFlipY, m_unpackPremultiplyAlpha, data)) {
            synthesizeGLError(GL_INVALID_VALUE, "texSubImage3D", "bad image data");
            return;
        }
    }
    if (m_unpackAlignment != 1)
        webContext()->pixelStorei(GL_UNPACK_ALIGNMENT, 1);
    webContext()->texSubImage3D(target, level, xoffset, yoffset, zoffset, pixels->width(), pixels->height(), 1, format, type, needConversion ? data.data() : pixels->data()->data());
    if (m_unpackAlignment != 1)
        webContext()->pixelStorei(GL_UNPACK_ALIGNMENT, m_unpackAlignment);
}

void WebGL2RenderingContextBase::texSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLenum format, GLenum type, HTMLImageElement* image, ExceptionState& exceptionState)
{
    if (isContextLost() || !image || !validateTexSubImage3D("texSubImage3D", target, level, xoffset, yoffset, zoffset, image->width(), image->height(), 1))
        return;

    if (isContextLost() || !validateHTMLImageElement("texSubImage3D", image, exceptionState))
        return;

    RefPtr<Image> imageForRender = image->cachedImage()->imageForLayoutObject(image->layoutObject());
    if (imageForRender->isSVGImage())
        imageForRender = drawImageIntoBuffer(imageForRender.get(), image->width(), image->height(), "texSubImage3D");

    texSubImage3DImpl(target, level, xoffset, yoffset, zoffset, format, type, imageForRender.get(), WebGLImageConversion::HtmlDomImage, m_unpackFlipY, m_unpackPremultiplyAlpha);
}

void WebGL2RenderingContextBase::texSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLenum format, GLenum type, HTMLCanvasElement* canvas, ExceptionState& exceptionState)
{
    if (isContextLost() || !validateHTMLCanvasElement("texSubImage3D", canvas, exceptionState))
        return;

    if (!canvas->renderingContext() || !canvas->renderingContext()->isAccelerated()) {
        ASSERT(!canvas->renderingContext() || canvas->renderingContext()->is2d());
        // 2D canvas has only FrontBuffer.
        texSubImage3DImpl(target, level, xoffset, yoffset, zoffset, format, type, canvas->copiedImage(FrontBuffer).get(),
            WebGLImageConversion::HtmlDomCanvas, m_unpackFlipY, m_unpackPremultiplyAlpha);
        return;
    }

    // FIXME: Can we do an accelerated copy in this case?
    notImplemented();
}

void WebGL2RenderingContextBase::texSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLenum format, GLenum type, HTMLVideoElement* video, ExceptionState& exceptionState)
{
    if (isContextLost() || !validateHTMLVideoElement("texSubImage3D", video, exceptionState))
        return;

    RefPtr<Image> image = videoFrameToImage(video, ImageBuffer::fastCopyImageMode());
    if (!image)
        return;
    texSubImage3DImpl(target, level, xoffset, yoffset, zoffset, format, type, image.get(), WebGLImageConversion::HtmlDomVideo, m_unpackFlipY, m_unpackPremultiplyAlpha);
}

void WebGL2RenderingContextBase::copyTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLint x, GLint y, GLsizei width, GLsizei height)
{
    if (isContextLost())
        return;

    notImplemented();
}

void WebGL2RenderingContextBase::compressedTexImage3D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, DOMArrayBufferView* data)
{
    if (isContextLost())
        return;

    WebGLTexture* tex = validateTextureBinding("compressedTexImage3D", target, true);
    if (!tex)
        return;

    if (tex->isImmutable()) {
        synthesizeGLError(GL_INVALID_OPERATION, "compressedTexImage3D", "attempted to modify immutable texture");
        return;
    }

    webContext()->compressedTexImage3D(target, level, internalformat, width, height, depth, border, data->byteLength(), data->baseAddress());
    tex->setLevelInfo(target, level, internalformat, width, height, depth, GL_UNSIGNED_BYTE);
}

void WebGL2RenderingContextBase::compressedTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, DOMArrayBufferView* data)
{
    if (isContextLost())
        return;

    WebGLTexture* tex = validateTextureBinding("compressedTexSubImage3D", target, true);
    if (!tex)
        return;

    if (format != tex->getInternalFormat(target, level)) {
        synthesizeGLError(GL_INVALID_OPERATION, "compressedTexSubImage3D", "format does not match texture format");
        return;
    }

    webContext()->compressedTexSubImage3D(target, level, xoffset, yoffset, zoffset,
        width, height, depth, format, data->byteLength(), data->baseAddress());
}

GLint WebGL2RenderingContextBase::getFragDataLocation(WebGLProgram* program, const String& name)
{
    if (isContextLost() || !validateWebGLObject("getFragDataLocation", program))
        return -1;

    return webContext()->getFragDataLocation(objectOrZero(program), name.utf8().data());
}

void WebGL2RenderingContextBase::uniform1ui(const WebGLUniformLocation* location, GLuint v0)
{
    if (isContextLost() || !location)
        return;

    if (location->program() != m_currentProgram) {
        synthesizeGLError(GL_INVALID_OPERATION, "uniform1ui", "location not for current program");
        return;
    }

    webContext()->uniform1ui(location->location(), v0);
}

void WebGL2RenderingContextBase::uniform2ui(const WebGLUniformLocation* location, GLuint v0, GLuint v1)
{
    if (isContextLost() || !location)
        return;

    if (location->program() != m_currentProgram) {
        synthesizeGLError(GL_INVALID_OPERATION, "uniform2ui", "location not for current program");
        return;
    }

    webContext()->uniform2ui(location->location(), v0, v1);
}

void WebGL2RenderingContextBase::uniform3ui(const WebGLUniformLocation* location, GLuint v0, GLuint v1, GLuint v2)
{
    if (isContextLost() || !location)
        return;

    if (location->program() != m_currentProgram) {
        synthesizeGLError(GL_INVALID_OPERATION, "uniform3ui", "location not for current program");
        return;
    }

    webContext()->uniform3ui(location->location(), v0, v1, v2);
}

void WebGL2RenderingContextBase::uniform4ui(const WebGLUniformLocation* location, GLuint v0, GLuint v1, GLuint v2, GLuint v3)
{
    if (isContextLost() || !location)
        return;

    if (location->program() != m_currentProgram) {
        synthesizeGLError(GL_INVALID_OPERATION, "uniform4ui", "location not for current program");
        return;
    }

    webContext()->uniform4ui(location->location(), v0, v1, v2, v3);
}

void WebGL2RenderingContextBase::uniform1uiv(const WebGLUniformLocation* location, Vector<GLuint>& value)
{
    if (isContextLost() || validateUniformParameters("uniform1uiv", location, value.data(), value.size(), 1))
        return;

    webContext()->uniform1uiv(location->location(), value.size(), value.data());
}

void WebGL2RenderingContextBase::uniform2uiv(const WebGLUniformLocation* location, Vector<GLuint>& value)
{
    if (isContextLost() || !validateUniformParameters("uniform2uiv", location, value.data(), value.size(), 2))
        return;

    webContext()->uniform2uiv(location->location(), value.size() / 2, value.data());
}

void WebGL2RenderingContextBase::uniform3uiv(const WebGLUniformLocation* location, Vector<GLuint>& value)
{
    if (isContextLost() || !validateUniformParameters("uniform3uiv", location, value.data(), value.size(), 3))
        return;

    webContext()->uniform3uiv(location->location(), value.size() / 3, value.data());
}

void WebGL2RenderingContextBase::uniform4uiv(const WebGLUniformLocation* location, Vector<GLuint>& value)
{
    if (isContextLost() || !validateUniformParameters("uniform4uiv", location, value.data(), value.size(), 4))
        return;

    webContext()->uniform4uiv(location->location(), value.size() / 4, value.data());
}

void WebGL2RenderingContextBase::uniformMatrix2x3fv(const WebGLUniformLocation* location, GLboolean transpose, DOMFloat32Array* value)
{
    if (isContextLost() || !validateUniformMatrixParameters("uniformMatrix2x3fv", location, transpose, value, 6))
        return;
    webContext()->uniformMatrix2x3fv(location->location(), value->length() / 6, transpose, value->data());
}

void WebGL2RenderingContextBase::uniformMatrix2x3fv(const WebGLUniformLocation* location, GLboolean transpose, Vector<GLfloat>& value)
{
    if (isContextLost() || !validateUniformMatrixParameters("uniformMatrix2x3fv", location, transpose, value.data(), value.size(), 6))
        return;
    webContext()->uniformMatrix2x3fv(location->location(), value.size() / 6, transpose, value.data());
}

void WebGL2RenderingContextBase::uniformMatrix3x2fv(const WebGLUniformLocation* location, GLboolean transpose, DOMFloat32Array* value)
{
    if (isContextLost() || !validateUniformMatrixParameters("uniformMatrix3x2fv", location, transpose, value, 6))
        return;
    webContext()->uniformMatrix3x2fv(location->location(), value->length() / 6, transpose, value->data());
}

void WebGL2RenderingContextBase::uniformMatrix3x2fv(const WebGLUniformLocation* location, GLboolean transpose, Vector<GLfloat>& value)
{
    if (isContextLost() || !validateUniformMatrixParameters("uniformMatrix3x2fv", location, transpose, value.data(), value.size(), 6))
        return;
    webContext()->uniformMatrix3x2fv(location->location(), value.size() / 6, transpose, value.data());
}

void WebGL2RenderingContextBase::uniformMatrix2x4fv(const WebGLUniformLocation* location, GLboolean transpose, DOMFloat32Array* value)
{
    if (isContextLost() || !validateUniformMatrixParameters("uniformMatrix2x4fv", location, transpose, value, 8))
        return;
    webContext()->uniformMatrix2x4fv(location->location(), value->length() / 8, transpose, value->data());
}

void WebGL2RenderingContextBase::uniformMatrix2x4fv(const WebGLUniformLocation* location, GLboolean transpose, Vector<GLfloat>& value)
{
    if (isContextLost() || !validateUniformMatrixParameters("uniformMatrix2x4fv", location, transpose, value.data(), value.size(), 8))
        return;
    webContext()->uniformMatrix2x4fv(location->location(), value.size() / 8, transpose, value.data());
}

void WebGL2RenderingContextBase::uniformMatrix4x2fv(const WebGLUniformLocation* location, GLboolean transpose, DOMFloat32Array* value)
{
    if (isContextLost() || !validateUniformMatrixParameters("uniformMatrix4x2fv", location, transpose, value, 8))
        return;
    webContext()->uniformMatrix4x2fv(location->location(), value->length() / 8, transpose, value->data());
}

void WebGL2RenderingContextBase::uniformMatrix4x2fv(const WebGLUniformLocation* location, GLboolean transpose, Vector<GLfloat>& value)
{
    if (isContextLost() || !validateUniformMatrixParameters("uniformMatrix4x2fv", location, transpose, value.data(), value.size(), 8))
        return;
    webContext()->uniformMatrix4x2fv(location->location(), value.size() / 8, transpose, value.data());
}

void WebGL2RenderingContextBase::uniformMatrix3x4fv(const WebGLUniformLocation* location, GLboolean transpose, DOMFloat32Array* value)
{
    if (isContextLost() || !validateUniformMatrixParameters("uniformMatrix3x4fv", location, transpose, value, 12))
        return;
    webContext()->uniformMatrix3x4fv(location->location(), value->length() / 12, transpose, value->data());
}

void WebGL2RenderingContextBase::uniformMatrix3x4fv(const WebGLUniformLocation* location, GLboolean transpose, Vector<GLfloat>& value)
{
    if (isContextLost() || !validateUniformMatrixParameters("uniformMatrix3x4fv", location, transpose, value.data(), value.size(), 12))
        return;
    webContext()->uniformMatrix3x4fv(location->location(), value.size() / 12, transpose, value.data());
}

void WebGL2RenderingContextBase::uniformMatrix4x3fv(const WebGLUniformLocation* location, GLboolean transpose, DOMFloat32Array* value)
{
    if (isContextLost() || !validateUniformMatrixParameters("uniformMatrix4x3fv", location, transpose, value, 12))
        return;
    webContext()->uniformMatrix4x3fv(location->location(), value->length() / 12, transpose, value->data());
}

void WebGL2RenderingContextBase::uniformMatrix4x3fv(const WebGLUniformLocation* location, GLboolean transpose, Vector<GLfloat>& value)
{
    if (isContextLost() || !validateUniformMatrixParameters("uniformMatrix4x3fv", location, transpose, value.data(), value.size(), 12))
        return;
    webContext()->uniformMatrix4x3fv(location->location(), value.size() / 12, transpose, value.data());
}

void WebGL2RenderingContextBase::vertexAttribI4i(GLuint index, GLint x, GLint y, GLint z, GLint w)
{
    if (isContextLost())
        return;

    if (index >= m_maxVertexAttribs) {
        synthesizeGLError(GL_INVALID_VALUE, "vertexAttribI4i", "index out of range");
        return;
    }

    webContext()->vertexAttribI4i(index, x, y, z, w);
    VertexAttribValue& attribValue = m_vertexAttribValue[index];
    attribValue.type = Int32ArrayType;
    attribValue.value.intValue[0] = x;
    attribValue.value.intValue[1] = y;
    attribValue.value.intValue[2] = z;
    attribValue.value.intValue[3] = w;
}

void WebGL2RenderingContextBase::vertexAttribI4iv(GLuint index, const Vector<GLint>& value)
{
    if (isContextLost())
        return;

    if (!value.data()) {
        synthesizeGLError(GL_INVALID_VALUE, "vertexAttribI4iv", "no array");
        return;
    }
    if (value.size() < 4) {
        synthesizeGLError(GL_INVALID_VALUE, "vertexAttribI4iv", "invalid size");
        return;
    }
    if (index >= m_maxVertexAttribs) {
        synthesizeGLError(GL_INVALID_VALUE, "vertexAttribI4iv", "index out of range");
        return;
    }

    webContext()->vertexAttribI4iv(index, value.data());
    VertexAttribValue& attribValue = m_vertexAttribValue[index];
    attribValue.type = Int32ArrayType;
    attribValue.value.intValue[0] = value[0];
    attribValue.value.intValue[1] = value[1];
    attribValue.value.intValue[2] = value[2];
    attribValue.value.intValue[3] = value[3];
}

void WebGL2RenderingContextBase::vertexAttribI4ui(GLuint index, GLuint x, GLuint y, GLuint z, GLuint w)
{
    if (isContextLost())
        return;

    if (index >= m_maxVertexAttribs) {
        synthesizeGLError(GL_INVALID_VALUE, "vertexAttribI4ui", "index out of range");
        return;
    }

    webContext()->vertexAttribI4ui(index, x, y, z, w);
    VertexAttribValue& attribValue = m_vertexAttribValue[index];
    attribValue.type = Uint32ArrayType;
    attribValue.value.uintValue[0] = x;
    attribValue.value.uintValue[1] = y;
    attribValue.value.uintValue[2] = z;
    attribValue.value.uintValue[3] = w;
}

void WebGL2RenderingContextBase::vertexAttribI4uiv(GLuint index, const Vector<GLuint>& value)
{
    if (isContextLost())
        return;

    if (!value.data()) {
        synthesizeGLError(GL_INVALID_VALUE, "vertexAttribI4uiv", "no array");
        return;
    }
    if (value.size() < 4) {
        synthesizeGLError(GL_INVALID_VALUE, "vertexAttribI4uiv", "invalid size");
        return;
    }
    if (index >= m_maxVertexAttribs) {
        synthesizeGLError(GL_INVALID_VALUE, "vertexAttribI4uiv", "index out of range");
        return;
    }

    webContext()->vertexAttribI4uiv(index, value.data());
    VertexAttribValue& attribValue = m_vertexAttribValue[index];
    attribValue.type = Uint32ArrayType;
    attribValue.value.uintValue[0] = value[0];
    attribValue.value.uintValue[1] = value[1];
    attribValue.value.uintValue[2] = value[2];
    attribValue.value.uintValue[3] = value[3];
}

void WebGL2RenderingContextBase::vertexAttribIPointer(GLuint index, GLint size, GLenum type, GLsizei stride, GLintptr offset)
{
    if (isContextLost())
        return;

    switch (type) {
    case GL_BYTE:
    case GL_UNSIGNED_BYTE:
    case GL_SHORT:
    case GL_UNSIGNED_SHORT:
    case GL_INT:
    case GL_UNSIGNED_INT:
        break;
    default:
        synthesizeGLError(GL_INVALID_ENUM, "vertexAttribIPointer", "invalid type");
        return;
    }
    if (index >= m_maxVertexAttribs) {
        synthesizeGLError(GL_INVALID_VALUE, "vertexAttribIPointer", "index out of range");
        return;
    }
    if (size < 1 || size > 4 || stride < 0 || stride > 255) {
        synthesizeGLError(GL_INVALID_VALUE, "vertexAttribIPointer", "bad size or stride");
        return;
    }
    if (!validateValueFitNonNegInt32("vertexAttribIPointer", "offset", offset))
        return;
    if (!m_boundArrayBuffer) {
        synthesizeGLError(GL_INVALID_OPERATION, "vertexAttribIPointer", "no bound ARRAY_BUFFER");
        return;
    }
    unsigned typeSize = sizeInBytes(type);
    ASSERT((typeSize & (typeSize - 1)) == 0); // Ensure that the value is POT.
    if ((stride & (typeSize - 1)) || (static_cast<GLintptr>(offset) & (typeSize - 1))) {
        synthesizeGLError(GL_INVALID_OPERATION, "vertexAttribIPointer", "stride or offset not valid for type");
        return;
    }
    GLsizei bytesPerElement = size * typeSize;

    m_boundVertexArrayObject->setVertexAttribState(index, bytesPerElement, size, type, false, stride, static_cast<GLintptr>(offset), m_boundArrayBuffer);
    webContext()->vertexAttribIPointer(index, size, type, stride, static_cast<GLintptr>(offset));
}

/* Writing to the drawing buffer */
void WebGL2RenderingContextBase::vertexAttribDivisor(GLuint index, GLuint divisor)
{
    if (isContextLost())
        return;

    if (index >= m_maxVertexAttribs) {
        synthesizeGLError(GL_INVALID_VALUE, "vertexAttribDivisor", "index out of range");
        return;
    }

    m_boundVertexArrayObject->setVertexAttribDivisor(index, divisor);
    webContext()->vertexAttribDivisorANGLE(index, divisor);
}

void WebGL2RenderingContextBase::drawArraysInstanced(GLenum mode, GLint first, GLsizei count, GLsizei instanceCount)
{
    if (!validateDrawArrays("drawArraysInstanced", mode, first, count))
        return;

    if (!validateDrawInstanced("drawArraysInstanced", instanceCount))
        return;

    clearIfComposited();

    handleTextureCompleteness("drawArraysInstanced", true);
    webContext()->drawArraysInstancedANGLE(mode, first, count, instanceCount);
    handleTextureCompleteness("drawArraysInstanced", false);
    markContextChanged(CanvasChanged);
}

void WebGL2RenderingContextBase::drawElementsInstanced(GLenum mode, GLsizei count, GLenum type, GLintptr offset, GLsizei instanceCount)
{
    if (!validateDrawElements("drawElementsInstanced", mode, count, type, offset))
        return;

    if (!validateDrawInstanced("drawElementsInstanced", instanceCount))
        return;

    clearIfComposited();

    handleTextureCompleteness("drawElementsInstanced", true);
    webContext()->drawElementsInstancedANGLE(mode, count, type, static_cast<GLintptr>(offset), instanceCount);
    handleTextureCompleteness("drawElementsInstanced", false);
    markContextChanged(CanvasChanged);
}

void WebGL2RenderingContextBase::drawRangeElements(GLenum mode, GLuint start, GLuint end, GLsizei count, GLenum type, GLintptr offset)
{
    if (!validateDrawElements("drawRangeElements", mode, count, type, offset))
        return;

    clearIfComposited();

    handleTextureCompleteness("drawRangeElements", true);
    webContext()->drawRangeElements(mode, start, end, count, type, static_cast<GLintptr>(offset));
    handleTextureCompleteness("drawRangeElements", false);
    markContextChanged(CanvasChanged);
}

void WebGL2RenderingContextBase::drawBuffers(const Vector<GLenum>& buffers)
{
    if (isContextLost())
        return;

    GLsizei n = buffers.size();
    const GLenum* bufs = buffers.data();
    if (!m_framebufferBinding) {
        if (n != 1) {
            synthesizeGLError(GL_INVALID_VALUE, "drawBuffers", "more than one buffer");
            return;
        }
        if (bufs[0] != GL_BACK && bufs[0] != GL_NONE) {
            synthesizeGLError(GL_INVALID_OPERATION, "drawBuffers", "BACK or NONE");
            return;
        }
        // Because the backbuffer is simulated on all current WebKit ports, we need to change BACK to COLOR_ATTACHMENT0.
        GLenum value = (bufs[0] == GL_BACK) ? GL_COLOR_ATTACHMENT0 : GL_NONE;
        webContext()->drawBuffersEXT(1, &value);
        setBackDrawBuffer(bufs[0]);
    } else {
        if (n > maxDrawBuffers()) {
            synthesizeGLError(GL_INVALID_VALUE, "drawBuffers", "more than max draw buffers");
            return;
        }
        for (GLsizei i = 0; i < n; ++i) {
            if (bufs[i] != GL_NONE && bufs[i] != static_cast<GLenum>(GL_COLOR_ATTACHMENT0_EXT + i)) {
                synthesizeGLError(GL_INVALID_OPERATION, "drawBuffers", "COLOR_ATTACHMENTi_EXT or NONE");
                return;
            }
        }
        m_framebufferBinding->drawBuffers(buffers);
    }
}

bool WebGL2RenderingContextBase::validateClearBuffer(const char* functionName, GLenum buffer, GLsizei size)
{
    switch (buffer) {
    case GL_COLOR:
    case GL_FRONT:
    case GL_BACK:
    case GL_FRONT_AND_BACK:
        if (size < 4) {
            synthesizeGLError(GL_INVALID_VALUE, functionName, "invalid array size");
            return false;
        }
        break;
    case GL_DEPTH:
    case GL_STENCIL:
        if (size < 1) {
            synthesizeGLError(GL_INVALID_VALUE, functionName, "invalid array size");
            return false;
        }
        break;
    default:
        synthesizeGLError(GL_INVALID_ENUM, functionName, "invalid buffer");
        return false;
    }
    return true;
}

void WebGL2RenderingContextBase::clearBufferiv(GLenum buffer, GLint drawbuffer, DOMInt32Array* value)
{
    if (isContextLost() || !validateClearBuffer("clearBufferiv", buffer, value->length()))
        return;

    webContext()->clearBufferiv(buffer, drawbuffer, value->data());
}

void WebGL2RenderingContextBase::clearBufferiv(GLenum buffer, GLint drawbuffer, const Vector<GLint>& value)
{
    if (isContextLost() || !validateClearBuffer("clearBufferiv", buffer, value.size()))
        return;

    webContext()->clearBufferiv(buffer, drawbuffer, value.data());
}

void WebGL2RenderingContextBase::clearBufferuiv(GLenum buffer, GLint drawbuffer, DOMUint32Array* value)
{
    if (isContextLost() || !validateClearBuffer("clearBufferuiv", buffer, value->length()))
        return;

    webContext()->clearBufferuiv(buffer, drawbuffer, value->data());
}

void WebGL2RenderingContextBase::clearBufferuiv(GLenum buffer, GLint drawbuffer, const Vector<GLuint>& value)
{
    if (isContextLost() || !validateClearBuffer("clearBufferuiv", buffer, value.size()))
        return;

    webContext()->clearBufferuiv(buffer, drawbuffer, value.data());
}

void WebGL2RenderingContextBase::clearBufferfv(GLenum buffer, GLint drawbuffer, DOMFloat32Array* value)
{
    if (isContextLost() || !validateClearBuffer("clearBufferfv", buffer, value->length()))
        return;

    webContext()->clearBufferfv(buffer, drawbuffer, value->data());
}

void WebGL2RenderingContextBase::clearBufferfv(GLenum buffer, GLint drawbuffer, const Vector<GLfloat>& value)
{
    if (isContextLost() || !validateClearBuffer("clearBufferfv", buffer, value.size()))
        return;

    webContext()->clearBufferfv(buffer, drawbuffer, value.data());
}

void WebGL2RenderingContextBase::clearBufferfi(GLenum buffer, GLint drawbuffer, GLfloat depth, GLint stencil)
{
    if (isContextLost())
        return;

    webContext()->clearBufferfi(buffer, drawbuffer, depth, stencil);
}

PassRefPtrWillBeRawPtr<WebGLQuery> WebGL2RenderingContextBase::createQuery()
{
    if (isContextLost())
        return nullptr;
    RefPtrWillBeRawPtr<WebGLQuery> o = WebGLQuery::create(this);
    addSharedObject(o.get());
    return o;
}

void WebGL2RenderingContextBase::deleteQuery(WebGLQuery* query)
{
    if (m_currentBooleanOcclusionQuery == query) {
        webContext()->endQueryEXT(m_currentBooleanOcclusionQuery->getTarget());
        m_currentBooleanOcclusionQuery = nullptr;
    }

    if (m_currentTransformFeedbackPrimitivesWrittenQuery == query) {
        webContext()->endQueryEXT(GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN);
        m_currentTransformFeedbackPrimitivesWrittenQuery = nullptr;
    }

    deleteObject(query);
}

GLboolean WebGL2RenderingContextBase::isQuery(WebGLQuery* query)
{
    if (isContextLost() || !query)
        return 0;

    return webContext()->isQueryEXT(query->object());
}

void WebGL2RenderingContextBase::beginQuery(GLenum target, WebGLQuery* query)
{
    bool deleted;
    if (!checkObjectToBeBound("beginQuery", query, deleted))
        return;
    if (deleted) {
        synthesizeGLError(GL_INVALID_OPERATION, "beginQuery", "attempted to begin a deleted query object");
        return;
    }

    if (query->getTarget() && query->getTarget() != target) {
        synthesizeGLError(GL_INVALID_OPERATION, "beginQuery", "query type does not match target");
        return;
    }

    switch (target) {
    case GL_ANY_SAMPLES_PASSED:
    case GL_ANY_SAMPLES_PASSED_CONSERVATIVE:
        {
            if (m_currentBooleanOcclusionQuery) {
                synthesizeGLError(GL_INVALID_OPERATION, "beginQuery", "a query is already active for target");
                return;
            }
            m_currentBooleanOcclusionQuery = query;
        }
        break;
    case GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN:
        {
            if (m_currentTransformFeedbackPrimitivesWrittenQuery) {
                synthesizeGLError(GL_INVALID_OPERATION, "beginQuery", "a query is already active for target");
                return;
            }
            m_currentTransformFeedbackPrimitivesWrittenQuery = query;
        }
        break;
    default:
        synthesizeGLError(GL_INVALID_ENUM, "beginQuery", "invalid target");
        return;
    }

    if (!query->getTarget())
        query->setTarget(target);

    webContext()->beginQueryEXT(target, query->object());
}

void WebGL2RenderingContextBase::endQuery(GLenum target)
{
    if (isContextLost())
        return;

    switch (target) {
    case GL_ANY_SAMPLES_PASSED:
    case GL_ANY_SAMPLES_PASSED_CONSERVATIVE:
        {
            if (m_currentBooleanOcclusionQuery && m_currentBooleanOcclusionQuery->getTarget() == target) {
                m_currentBooleanOcclusionQuery = nullptr;
            } else {
                synthesizeGLError(GL_INVALID_OPERATION, "endQuery", "target query is not active");
                return;
            }
        }
        break;
    case GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN:
        {
            if (m_currentTransformFeedbackPrimitivesWrittenQuery) {
                m_currentTransformFeedbackPrimitivesWrittenQuery = nullptr;
            } else {
                synthesizeGLError(GL_INVALID_OPERATION, "endQuery", "target query is not active");
                return;
            }
        }
        break;
    default:
        synthesizeGLError(GL_INVALID_ENUM, "endQuery", "invalid target");
        return;
    }

    webContext()->endQueryEXT(target);
}

PassRefPtrWillBeRawPtr<WebGLQuery> WebGL2RenderingContextBase::getQuery(GLenum target, GLenum pname)
{
    if (isContextLost())
        return nullptr;

    if (pname != GL_CURRENT_QUERY) {
        synthesizeGLError(GL_INVALID_ENUM, "getQuery", "invalid parameter name");
        return nullptr;
    }

    switch (target) {
    case GL_ANY_SAMPLES_PASSED:
    case GL_ANY_SAMPLES_PASSED_CONSERVATIVE:
        if (m_currentBooleanOcclusionQuery && m_currentBooleanOcclusionQuery->getTarget() == target)
            return PassRefPtrWillBeRawPtr<WebGLQuery>(m_currentBooleanOcclusionQuery.get());
        break;
    case GL_TRANSFORM_FEEDBACK_PRIMITIVES_WRITTEN:
        return PassRefPtrWillBeRawPtr<WebGLQuery>(m_currentTransformFeedbackPrimitivesWrittenQuery.get());
    default:
        synthesizeGLError(GL_INVALID_ENUM, "getQuery", "invalid target");
        return nullptr;
    }
    return nullptr;
}

ScriptValue WebGL2RenderingContextBase::getQueryParameter(ScriptState* scriptState, WebGLQuery* query, GLenum pname)
{
    if (isContextLost() || !validateWebGLObject("getQueryParameter", query))
        return ScriptValue::createNull(scriptState);

    switch (pname) {
    case GL_QUERY_RESULT:
        {
            GLuint value;
            webContext()->getQueryObjectuivEXT(objectOrZero(query), pname, &value);
            return WebGLAny(scriptState, value);
        }
    case GL_QUERY_RESULT_AVAILABLE:
        {
            GLuint value;
            webContext()->getQueryObjectuivEXT(objectOrZero(query), pname, &value);
            return WebGLAny(scriptState, value == GL_TRUE);
        }
    default:
        synthesizeGLError(GL_INVALID_ENUM, "getQueryParameter", "invalid parameter name");
        return ScriptValue::createNull(scriptState);
    }
}

PassRefPtrWillBeRawPtr<WebGLSampler> WebGL2RenderingContextBase::createSampler()
{
    if (isContextLost())
        return nullptr;
    RefPtrWillBeRawPtr<WebGLSampler> o = WebGLSampler::create(this);
    addSharedObject(o.get());
    return o;
}

void WebGL2RenderingContextBase::deleteSampler(WebGLSampler* sampler)
{
    for (size_t i = 0; i < m_samplerUnits.size(); ++i) {
        if (sampler == m_samplerUnits[i]) {
            m_samplerUnits[i] = nullptr;
            webContext()->bindSampler(i, 0);
        }
    }

    deleteObject(sampler);
}

GLboolean WebGL2RenderingContextBase::isSampler(WebGLSampler* sampler)
{
    if (isContextLost() || !sampler)
        return 0;

    return webContext()->isSampler(sampler->object());
}

void WebGL2RenderingContextBase::bindSampler(GLuint unit, WebGLSampler* sampler)
{
    bool deleted;
    if (!checkObjectToBeBound("bindSampler", sampler, deleted))
        return;
    if (deleted) {
        synthesizeGLError(GL_INVALID_OPERATION, "bindSampler", "attempted to bind a deleted sampler");
        return;
    }

    if (unit >= m_samplerUnits.size()) {
        synthesizeGLError(GL_INVALID_VALUE, "bindSampler", "texture unit out of range");
        return;
    }

    m_samplerUnits[unit] = sampler;

    webContext()->bindSampler(unit, objectOrZero(sampler));
}

void WebGL2RenderingContextBase::samplerParameteri(WebGLSampler* sampler, GLenum pname, GLint param)
{
    if (isContextLost() || !validateWebGLObject("samplerParameteri", sampler))
        return;

    webContext()->samplerParameteri(objectOrZero(sampler), pname, param);
}

void WebGL2RenderingContextBase::samplerParameterf(WebGLSampler* sampler, GLenum pname, GLfloat param)
{
    if (isContextLost() || !validateWebGLObject("samplerParameterf", sampler))
        return;

    webContext()->samplerParameterf(objectOrZero(sampler), pname, param);
}

ScriptValue WebGL2RenderingContextBase::getSamplerParameter(ScriptState* scriptState, WebGLSampler* sampler, GLenum pname)
{
    if (isContextLost() || !validateWebGLObject("getSamplerParameter", sampler))
        return ScriptValue::createNull(scriptState);

    switch (pname) {
    case GL_TEXTURE_COMPARE_FUNC:
    case GL_TEXTURE_COMPARE_MODE:
    case GL_TEXTURE_MAG_FILTER:
    case GL_TEXTURE_MIN_FILTER:
    case GL_TEXTURE_WRAP_R:
    case GL_TEXTURE_WRAP_S:
    case GL_TEXTURE_WRAP_T:
        {
            GLint value = 0;
            webContext()->getSamplerParameteriv(objectOrZero(sampler), pname, &value);
            return WebGLAny(scriptState, static_cast<unsigned>(value));
        }
    case GL_TEXTURE_MAX_LOD:
    case GL_TEXTURE_MIN_LOD:
        {
            GLfloat value = 0.f;
            webContext()->getSamplerParameterfv(objectOrZero(sampler), pname, &value);
            return WebGLAny(scriptState, value);
        }
    default:
        synthesizeGLError(GL_INVALID_ENUM, "getSamplerParameter", "invalid parameter name");
        return ScriptValue::createNull(scriptState);
    }
}

PassRefPtrWillBeRawPtr<WebGLSync> WebGL2RenderingContextBase::fenceSync(GLenum condition, GLbitfield flags)
{
    if (isContextLost())
        return nullptr;

    RefPtrWillBeRawPtr<WebGLSync> o = WebGLFenceSync::create(this, condition, flags);
    addSharedObject(o.get());
    return o.release();
}

GLboolean WebGL2RenderingContextBase::isSync(WebGLSync* sync)
{
    if (isContextLost() || !sync)
        return 0;

    return webContext()->isSync(sync->object());
}

void WebGL2RenderingContextBase::deleteSync(WebGLSync* sync)
{
    deleteObject(sync);
}

GLenum WebGL2RenderingContextBase::clientWaitSync(WebGLSync* sync, GLbitfield flags, GLuint timeout)
{
    if (isContextLost() || !validateWebGLObject("clientWaitSync", sync))
        return GL_WAIT_FAILED;

    GLuint64 timeout64 = (timeout == webGLTimeoutIgnored ? GL_TIMEOUT_IGNORED : timeout);
    return webContext()->clientWaitSync(syncObjectOrZero(sync), flags, timeout64);
}

void WebGL2RenderingContextBase::waitSync(WebGLSync* sync, GLbitfield flags, GLuint timeout)
{
    if (isContextLost() || !validateWebGLObject("waitSync", sync))
        return;

    GLuint64 timeout64 = (timeout == webGLTimeoutIgnored ? GL_TIMEOUT_IGNORED : timeout);
    webContext()->waitSync(syncObjectOrZero(sync), flags, timeout64);
}

ScriptValue WebGL2RenderingContextBase::getSyncParameter(ScriptState* scriptState, WebGLSync* sync, GLenum pname)
{
    if (isContextLost() || !validateWebGLObject("getSyncParameter", sync))
        return ScriptValue::createNull(scriptState);

    switch (pname) {
    case GL_OBJECT_TYPE:
    case GL_SYNC_STATUS:
    case GL_SYNC_CONDITION:
    case GL_SYNC_FLAGS:
        {
            GLint value = 0;
            GLsizei length = -1;
            webContext()->getSynciv(syncObjectOrZero(sync), pname, 1, &length, &value);
            return WebGLAny(scriptState, static_cast<unsigned>(value));
        }
    default:
        synthesizeGLError(GL_INVALID_ENUM, "getSyncParameter", "invalid parameter name");
        return ScriptValue::createNull(scriptState);
    }
}

PassRefPtrWillBeRawPtr<WebGLTransformFeedback> WebGL2RenderingContextBase::createTransformFeedback()
{
    if (isContextLost())
        return nullptr;
    RefPtrWillBeRawPtr<WebGLTransformFeedback> o = WebGLTransformFeedback::create(this);
    addSharedObject(o.get());
    return o;
}

void WebGL2RenderingContextBase::deleteTransformFeedback(WebGLTransformFeedback* feedback)
{
    if (feedback == m_transformFeedbackBinding)
        m_transformFeedbackBinding = nullptr;

    deleteObject(feedback);
}

GLboolean WebGL2RenderingContextBase::isTransformFeedback(WebGLTransformFeedback* feedback)
{
    if (isContextLost() || !feedback)
        return 0;

    if (!feedback->hasEverBeenBound())
        return 0;

    return webContext()->isTransformFeedback(feedback->object());
}

void WebGL2RenderingContextBase::bindTransformFeedback(GLenum target, WebGLTransformFeedback* feedback)
{
    bool deleted;
    if (!checkObjectToBeBound("bindTransformFeedback", feedback, deleted))
        return;
    if (deleted) {
        synthesizeGLError(GL_INVALID_OPERATION, "bindTransformFeedback", "attempted to bind a deleted transform feedback object");
        return;
    }

    if (target != GL_TRANSFORM_FEEDBACK) {
        synthesizeGLError(GL_INVALID_ENUM, "bindTransformFeedback", "target must be TRANSFORM_FEEDBACK");
        return;
    }

    m_transformFeedbackBinding = feedback;

    webContext()->bindTransformFeedback(target, objectOrZero(feedback));
    if (feedback)
        feedback->setTarget(target);
}

void WebGL2RenderingContextBase::beginTransformFeedback(GLenum primitiveMode)
{
    if (isContextLost())
        return;

    webContext()->beginTransformFeedback(primitiveMode);
}

void WebGL2RenderingContextBase::endTransformFeedback()
{
    if (isContextLost())
        return;

    webContext()->endTransformFeedback();
}

void WebGL2RenderingContextBase::transformFeedbackVaryings(WebGLProgram* program, const Vector<String>& varyings, GLenum bufferMode)
{
    if (isContextLost() || !validateWebGLObject("transformFeedbackVaryings", program))
        return;

    Vector<CString> keepAlive; // Must keep these instances alive while looking at their data
    Vector<const char*> varyingStrings;
    for (size_t i = 0; i < varyings.size(); ++i) {
        keepAlive.append(varyings[i].ascii());
        varyingStrings.append(keepAlive.last().data());
    }

    webContext()->transformFeedbackVaryings(objectOrZero(program), varyings.size(), varyingStrings.data(), bufferMode);
}

PassRefPtrWillBeRawPtr<WebGLActiveInfo> WebGL2RenderingContextBase::getTransformFeedbackVarying(WebGLProgram* program, GLuint index)
{
    if (isContextLost() || !validateWebGLObject("getTransformFeedbackVarying", program))
        return nullptr;

    notImplemented();
    return nullptr;
}

void WebGL2RenderingContextBase::pauseTransformFeedback()
{
    if (isContextLost())
        return;

    webContext()->pauseTransformFeedback();
}

void WebGL2RenderingContextBase::resumeTransformFeedback()
{
    if (isContextLost())
        return;

    webContext()->resumeTransformFeedback();
}

void WebGL2RenderingContextBase::bindBufferBase(GLenum target, GLuint index, WebGLBuffer* buffer)
{
    if (isContextLost() || !validateWebGLObject("bindBufferBase", buffer))
        return;

    webContext()->bindBufferBase(target, index, objectOrZero(buffer));
}

void WebGL2RenderingContextBase::bindBufferRange(GLenum target, GLuint index, WebGLBuffer* buffer, GLintptr offset, GLsizeiptr size)
{
    if (isContextLost() || !validateWebGLObject("bindBufferRange", buffer))
        return;

    webContext()->bindBufferRange(target, index, objectOrZero(buffer), offset, size);
}

ScriptValue WebGL2RenderingContextBase::getIndexedParameter(ScriptState* scriptState, GLenum target, GLuint index)
{
    if (isContextLost())
        return ScriptValue::createNull(scriptState);

    switch (target) {
    case GL_TRANSFORM_FEEDBACK_BUFFER_BINDING:
    case GL_UNIFORM_BUFFER_BINDING:
        notImplemented();
        return ScriptValue::createNull(scriptState);
    case GL_TRANSFORM_FEEDBACK_BUFFER_SIZE:
    case GL_TRANSFORM_FEEDBACK_BUFFER_START:
    case GL_UNIFORM_BUFFER_SIZE:
    case GL_UNIFORM_BUFFER_START:
        {
            GLint64 value = -1;
            webContext()->getInteger64i_v(target, index, &value);
            return WebGLAny(scriptState, value);
        }
    default:
        synthesizeGLError(GL_INVALID_ENUM, "getIndexedParameter", "invalid parameter name");
        return ScriptValue::createNull(scriptState);
    }
}

Vector<GLuint> WebGL2RenderingContextBase::getUniformIndices(WebGLProgram* program, const Vector<String>& uniformNames)
{
    Vector<GLuint> result;
    if (isContextLost() || !validateWebGLObject("getUniformIndices", program))
        return result;

    Vector<CString> keepAlive; // Must keep these instances alive while looking at their data
    Vector<const char*> uniformStrings;
    for (size_t i = 0; i < uniformNames.size(); ++i) {
        keepAlive.append(uniformNames[i].ascii());
        uniformStrings.append(keepAlive.last().data());
    }

    result.resize(uniformNames.size());
    webContext()->getUniformIndices(objectOrZero(program), uniformStrings.size(), uniformStrings.data(), result.data());
    return result;
}

Vector<GLint> WebGL2RenderingContextBase::getActiveUniforms(WebGLProgram* program, const Vector<GLuint>& uniformIndices, GLenum pname)
{
    Vector<GLint> result;
    if (isContextLost() || !validateWebGLObject("getActiveUniforms", program))
        return result;

    result.resize(uniformIndices.size());
    webContext()->getActiveUniformsiv(objectOrZero(program), uniformIndices.size(), uniformIndices.data(), pname, result.data());
    return result;
}

GLuint WebGL2RenderingContextBase::getUniformBlockIndex(WebGLProgram* program, const String& uniformBlockName)
{
    if (isContextLost() || !validateWebGLObject("getUniformBlockIndex", program))
        return 0;
    if (!validateString("getUniformBlockIndex", uniformBlockName))
        return 0;

    return webContext()->getUniformBlockIndex(objectOrZero(program), uniformBlockName.utf8().data());
}

ScriptValue WebGL2RenderingContextBase::getActiveUniformBlockParameter(ScriptState* scriptState, WebGLProgram* program, GLuint uniformBlockIndex, GLenum pname)
{
    if (isContextLost() || !validateWebGLObject("getActiveUniformBlockParameter", program))
        return ScriptValue::createNull(scriptState);

    switch (pname) {
    case GL_UNIFORM_BLOCK_BINDING:
    case GL_UNIFORM_BLOCK_DATA_SIZE:
    case GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS:
        {
            GLint intValue = 0;
            webContext()->getActiveUniformBlockiv(objectOrZero(program), uniformBlockIndex, pname, &intValue);
            return WebGLAny(scriptState, static_cast<unsigned>(intValue));
        }
    case GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES:
        {
            GLint uniformCount = 0;
            webContext()->getActiveUniformBlockiv(objectOrZero(program), uniformBlockIndex, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &uniformCount);

            Vector<GLint> indices(uniformCount);
            webContext()->getActiveUniformBlockiv(objectOrZero(program), uniformBlockIndex, pname, indices.data());
            return WebGLAny(scriptState, DOMUint32Array::create(reinterpret_cast<GLuint*>(indices.data()), indices.size()));
        }
    case GL_UNIFORM_BLOCK_REFERENCED_BY_VERTEX_SHADER:
    case GL_UNIFORM_BLOCK_REFERENCED_BY_FRAGMENT_SHADER:
        {
            GLint boolValue = 0;
            webContext()->getActiveUniformBlockiv(objectOrZero(program), uniformBlockIndex, pname, &boolValue);
            return WebGLAny(scriptState, static_cast<bool>(boolValue));
        }
    default:
        synthesizeGLError(GL_INVALID_ENUM, "getActiveUniformBlockParameter", "invalid parameter name");
        return ScriptValue::createNull(scriptState);
    }
}

String WebGL2RenderingContextBase::getActiveUniformBlockName(WebGLProgram* program, GLuint uniformBlockIndex)
{
    if (isContextLost() || !validateWebGLObject("getActiveUniformBlockName", program))
        return String();

    GLint maxNameLength = -1;
    webContext()->getProgramiv(objectOrZero(program), GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &maxNameLength);
    if (maxNameLength <= 0) {
        // This state indicates that there are no active uniform blocks
        synthesizeGLError(GL_INVALID_VALUE, "getActiveUniformBlockName", "invalid uniform block index");
        return String();
    }
    scoped_ptr<GLchar[]> name(new GLchar[maxNameLength]);
    GLsizei length;
    webContext()->getActiveUniformBlockName(objectOrZero(program), uniformBlockIndex, maxNameLength, &length, name.get());
    return String(name.get(), length);
}

void WebGL2RenderingContextBase::uniformBlockBinding(WebGLProgram* program, GLuint uniformBlockIndex, GLuint uniformBlockBinding)
{
    if (isContextLost() || !validateWebGLObject("uniformBlockBinding", program))
        return;

    webContext()->uniformBlockBinding(objectOrZero(program), uniformBlockIndex, uniformBlockBinding);
}

PassRefPtrWillBeRawPtr<WebGLVertexArrayObject> WebGL2RenderingContextBase::createVertexArray()
{
    if (isContextLost())
        return nullptr;

    RefPtrWillBeRawPtr<WebGLVertexArrayObject> o = WebGLVertexArrayObject::create(this, WebGLVertexArrayObjectBase::VaoTypeUser);
    addContextObject(o.get());
    return o.release();
}

void WebGL2RenderingContextBase::deleteVertexArray(WebGLVertexArrayObject* vertexArray)
{
    if (isContextLost() || !vertexArray)
        return;

    if (!vertexArray->isDefaultObject() && vertexArray == m_boundVertexArrayObject)
        setBoundVertexArrayObject(nullptr);

    vertexArray->deleteObject(webContext());
}

GLboolean WebGL2RenderingContextBase::isVertexArray(WebGLVertexArrayObject* vertexArray)
{
    if (isContextLost() || !vertexArray)
        return 0;

    if (!vertexArray->hasEverBeenBound())
        return 0;

    return webContext()->isVertexArrayOES(vertexArray->object());
}

void WebGL2RenderingContextBase::bindVertexArray(WebGLVertexArrayObject* vertexArray)
{
    if (isContextLost())
        return;

    if (vertexArray && (vertexArray->isDeleted() || !vertexArray->validate(0, this))) {
        webContext()->synthesizeGLError(GL_INVALID_OPERATION);
        return;
    }

    if (vertexArray && !vertexArray->isDefaultObject() && vertexArray->object()) {
        webContext()->bindVertexArrayOES(objectOrZero(vertexArray));

        vertexArray->setHasEverBeenBound();
        setBoundVertexArrayObject(vertexArray);
    } else {
        webContext()->bindVertexArrayOES(0);
        setBoundVertexArrayObject(nullptr);
    }
}

void WebGL2RenderingContextBase::bindFramebuffer(GLenum target, WebGLFramebuffer* buffer)
{
    bool deleted;
    if (!checkObjectToBeBound("bindFramebuffer", buffer, deleted))
        return;

    if (deleted)
        buffer = 0;

    switch (target) {
    case GL_DRAW_FRAMEBUFFER:
        break;
    case GL_FRAMEBUFFER:
    case GL_READ_FRAMEBUFFER:
        m_readFramebufferBinding = buffer;
        break;
    default:
        synthesizeGLError(GL_INVALID_ENUM, "bindFramebuffer", "invalid target");
        return;
    }

    setFramebuffer(target, buffer);
}

void WebGL2RenderingContextBase::deleteFramebuffer(WebGLFramebuffer* framebuffer)
{
    if (!deleteObject(framebuffer))
        return;
    GLenum target = 0;
    if (framebuffer == m_framebufferBinding) {
        if (framebuffer == m_readFramebufferBinding) {
            target = GL_FRAMEBUFFER;
            m_framebufferBinding = nullptr;
            m_readFramebufferBinding = nullptr;
        } else {
            target = GL_DRAW_FRAMEBUFFER;
            m_framebufferBinding = nullptr;
        }
    } else if (framebuffer == m_readFramebufferBinding) {
        target = GL_READ_FRAMEBUFFER;
        m_readFramebufferBinding = nullptr;
    }
    if (target) {
        drawingBuffer()->setFramebufferBinding(target, 0);
        // Have to call drawingBuffer()->bind() here to bind back to internal fbo.
        drawingBuffer()->bind(target);
    }
}

ScriptValue WebGL2RenderingContextBase::getParameter(ScriptState* scriptState, GLenum pname)
{
    if (isContextLost())
        return ScriptValue::createNull(scriptState);
    switch (pname) {
    case GL_SHADING_LANGUAGE_VERSION:
        return WebGLAny(scriptState, "WebGL GLSL ES 3.00 (" + String(webContext()->getString(GL_SHADING_LANGUAGE_VERSION)) + ")");
    case GL_VERSION:
        return WebGLAny(scriptState, "WebGL 2.0 (" + String(webContext()->getString(GL_VERSION)) + ")");

    case GL_COPY_READ_BUFFER_BINDING:
        return WebGLAny(scriptState, PassRefPtrWillBeRawPtr<WebGLObject>(m_boundCopyReadBuffer.get()));
    case GL_COPY_WRITE_BUFFER_BINDING:
        return WebGLAny(scriptState, PassRefPtrWillBeRawPtr<WebGLObject>(m_boundCopyWriteBuffer.get()));
    case GL_DRAW_FRAMEBUFFER_BINDING:
        return WebGLAny(scriptState, PassRefPtrWillBeRawPtr<WebGLObject>(m_framebufferBinding.get()));
    case GL_FRAGMENT_SHADER_DERIVATIVE_HINT:
        return getUnsignedIntParameter(scriptState, pname);
    case GL_MAX_3D_TEXTURE_SIZE:
        return getIntParameter(scriptState, pname);
    case GL_MAX_ARRAY_TEXTURE_LAYERS:
        return getIntParameter(scriptState, pname);
    case GC3D_MAX_CLIENT_WAIT_TIMEOUT_WEBGL:
        return WebGLAny(scriptState, 0u);
    case GL_MAX_COLOR_ATTACHMENTS:
        return getIntParameter(scriptState, pname);
    case GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS:
        return getInt64Parameter(scriptState, pname);
    case GL_MAX_COMBINED_UNIFORM_BLOCKS:
        return getIntParameter(scriptState, pname);
    case GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS:
        return getInt64Parameter(scriptState, pname);
    case GL_MAX_DRAW_BUFFERS:
        return getIntParameter(scriptState, pname);
    case GL_MAX_ELEMENT_INDEX:
        return getInt64Parameter(scriptState, pname);
    case GL_MAX_ELEMENTS_INDICES:
        return getIntParameter(scriptState, pname);
    case GL_MAX_ELEMENTS_VERTICES:
        return getIntParameter(scriptState, pname);
    case GL_MAX_FRAGMENT_INPUT_COMPONENTS:
        return getIntParameter(scriptState, pname);
    case GL_MAX_FRAGMENT_UNIFORM_BLOCKS:
        return getIntParameter(scriptState, pname);
    case GL_MAX_FRAGMENT_UNIFORM_COMPONENTS:
        return getIntParameter(scriptState, pname);
    case GL_MAX_PROGRAM_TEXEL_OFFSET:
        return getIntParameter(scriptState, pname);
    case GL_MAX_SAMPLES:
        return getIntParameter(scriptState, pname);
    case GL_MAX_SERVER_WAIT_TIMEOUT:
        return getInt64Parameter(scriptState, pname);
    case GL_MAX_TEXTURE_LOD_BIAS:
        return getFloatParameter(scriptState, pname);
    case GL_MAX_TRANSFORM_FEEDBACK_INTERLEAVED_COMPONENTS:
        return getIntParameter(scriptState, pname);
    case GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_ATTRIBS:
        return getIntParameter(scriptState, pname);
    case GL_MAX_TRANSFORM_FEEDBACK_SEPARATE_COMPONENTS:
        return getIntParameter(scriptState, pname);
    case GL_MAX_UNIFORM_BLOCK_SIZE:
        return getInt64Parameter(scriptState, pname);
    case GL_MAX_UNIFORM_BUFFER_BINDINGS:
        return getIntParameter(scriptState, pname);
    case GL_MAX_VARYING_COMPONENTS:
        return getIntParameter(scriptState, pname);
    case GL_MAX_VERTEX_OUTPUT_COMPONENTS:
        return getIntParameter(scriptState, pname);
    case GL_MAX_VERTEX_UNIFORM_BLOCKS:
        return getIntParameter(scriptState, pname);
    case GL_MAX_VERTEX_UNIFORM_COMPONENTS:
        return getIntParameter(scriptState, pname);
    case GL_MIN_PROGRAM_TEXEL_OFFSET:
        return getIntParameter(scriptState, pname);
    case GL_PACK_ROW_LENGTH:
        return getIntParameter(scriptState, pname);
    case GL_PACK_SKIP_PIXELS:
        return getIntParameter(scriptState, pname);
    case GL_PACK_SKIP_ROWS:
        return getIntParameter(scriptState, pname);
    case GL_PIXEL_PACK_BUFFER_BINDING:
        return WebGLAny(scriptState, PassRefPtrWillBeRawPtr<WebGLObject>(m_boundPixelPackBuffer.get()));
    case GL_PIXEL_UNPACK_BUFFER_BINDING:
        return WebGLAny(scriptState, PassRefPtrWillBeRawPtr<WebGLObject>(m_boundPixelUnpackBuffer.get()));
    case GL_RASTERIZER_DISCARD:
        return getBooleanParameter(scriptState, pname);
    case GL_READ_BUFFER:
        {
            GLenum value = 0;
            if (!isContextLost()) {
                WebGLFramebuffer* readFramebufferBinding = getFramebufferBinding(GL_READ_FRAMEBUFFER);
                if (!readFramebufferBinding)
                    value = m_readBufferOfDefaultFramebuffer;
                else
                    value = readFramebufferBinding->getReadBuffer();
            }
            return WebGLAny(scriptState, value);
        }
    case GL_READ_FRAMEBUFFER_BINDING:
        return WebGLAny(scriptState, PassRefPtrWillBeRawPtr<WebGLObject>(m_readFramebufferBinding.get()));
    case GL_SAMPLE_ALPHA_TO_COVERAGE:
        return getBooleanParameter(scriptState, pname);
    case GL_SAMPLE_COVERAGE:
        return getBooleanParameter(scriptState, pname);
    case GL_SAMPLER_BINDING:
        return WebGLAny(scriptState, PassRefPtrWillBeRawPtr<WebGLObject>(m_samplerUnits[m_activeTextureUnit].get()));
    case GL_TEXTURE_BINDING_2D_ARRAY:
        return WebGLAny(scriptState, PassRefPtrWillBeRawPtr<WebGLObject>(m_textureUnits[m_activeTextureUnit].m_texture2DArrayBinding.get()));
    case GL_TEXTURE_BINDING_3D:
        return WebGLAny(scriptState, PassRefPtrWillBeRawPtr<WebGLObject>(m_textureUnits[m_activeTextureUnit].m_texture3DBinding.get()));
    case GL_TRANSFORM_FEEDBACK_ACTIVE:
        return getBooleanParameter(scriptState, pname);
    case GL_TRANSFORM_FEEDBACK_BUFFER_BINDING:
        return WebGLAny(scriptState, PassRefPtrWillBeRawPtr<WebGLObject>(m_boundTransformFeedbackBuffer.get()));
    case GL_TRANSFORM_FEEDBACK_BINDING:
        return WebGLAny(scriptState, PassRefPtrWillBeRawPtr<WebGLObject>(m_transformFeedbackBinding.get()));
    case GL_TRANSFORM_FEEDBACK_PAUSED:
        return getBooleanParameter(scriptState, pname);
    case GL_UNIFORM_BUFFER_BINDING:
        return WebGLAny(scriptState, PassRefPtrWillBeRawPtr<WebGLObject>(m_boundUniformBuffer.get()));
    case GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT:
        return getIntParameter(scriptState, pname);
    case GL_UNPACK_IMAGE_HEIGHT:
        return getIntParameter(scriptState, pname);
    case GL_UNPACK_ROW_LENGTH:
        return getIntParameter(scriptState, pname);
    case GL_UNPACK_SKIP_IMAGES:
        return getBooleanParameter(scriptState, pname);
    case GL_UNPACK_SKIP_PIXELS:
        return getBooleanParameter(scriptState, pname);
    case GL_UNPACK_SKIP_ROWS:
        return getBooleanParameter(scriptState, pname);

    default:
        return WebGLRenderingContextBase::getParameter(scriptState, pname);
    }
}

ScriptValue WebGL2RenderingContextBase::getInt64Parameter(ScriptState* scriptState, GLenum pname)
{
    GLint64 value = 0;
    if (!isContextLost())
        webContext()->getInteger64v(pname, &value);
    return WebGLAny(scriptState, value);
}

bool WebGL2RenderingContextBase::validateCapability(const char* functionName, GLenum cap)
{
    switch (cap) {
    case GL_RASTERIZER_DISCARD:
        return true;
    default:
        return WebGLRenderingContextBase::validateCapability(functionName, cap);
    }
}

bool WebGL2RenderingContextBase::validateBufferTarget(const char* functionName, GLenum target)
{
    switch (target) {
    case GL_ARRAY_BUFFER:
    case GL_COPY_READ_BUFFER:
    case GL_COPY_WRITE_BUFFER:
    case GL_ELEMENT_ARRAY_BUFFER:
    case GL_PIXEL_PACK_BUFFER:
    case GL_PIXEL_UNPACK_BUFFER:
    case GL_TRANSFORM_FEEDBACK_BUFFER:
    case GL_UNIFORM_BUFFER:
        return true;
    default:
        synthesizeGLError(GL_INVALID_ENUM, functionName, "invalid target");
        return false;
    }
}

bool WebGL2RenderingContextBase::validateAndUpdateBufferBindTarget(const char* functionName, GLenum target, WebGLBuffer* buffer)
{
    if (!validateBufferTarget(functionName, target))
        return false;

    if (buffer) {
        switch (buffer->getInitialTarget()) {
        case GL_ELEMENT_ARRAY_BUFFER:
            switch (target) {
            case GL_ARRAY_BUFFER:
            case GL_PIXEL_PACK_BUFFER:
            case GL_PIXEL_UNPACK_BUFFER:
            case GL_TRANSFORM_FEEDBACK_BUFFER:
            case GL_UNIFORM_BUFFER:
                synthesizeGLError(GL_INVALID_OPERATION, functionName,
                    "element array buffers can not be bound to a different target");

                return false;
            default:
                break;
            }
            break;
        case GL_ARRAY_BUFFER:
        case GL_COPY_READ_BUFFER:
        case GL_COPY_WRITE_BUFFER:
        case GL_PIXEL_PACK_BUFFER:
        case GL_PIXEL_UNPACK_BUFFER:
        case GL_UNIFORM_BUFFER:
        case GL_TRANSFORM_FEEDBACK_BUFFER:
            if (target == GL_ELEMENT_ARRAY_BUFFER) {
                synthesizeGLError(GL_INVALID_OPERATION, functionName,
                    "buffers bound to non ELEMENT_ARRAY_BUFFER targets can not be bound to ELEMENT_ARRAY_BUFFER target");
                return false;
            }
            break;
        default:
            break;
        }
        if (target == GL_TRANSFORM_FEEDBACK_BUFFER) {
            if (m_boundArrayBuffer == buffer
                || m_boundVertexArrayObject->boundElementArrayBuffer() == buffer
                || m_boundCopyReadBuffer == buffer
                || m_boundCopyWriteBuffer == buffer
                || m_boundPixelPackBuffer == buffer
                || m_boundPixelUnpackBuffer == buffer
                || m_boundUniformBuffer == buffer) {
                synthesizeGLError(GL_INVALID_OPERATION, functionName,
                    "a buffer bound to TRANSFORM_FEEDBACK_BUFFER can not be bound to any other targets");
                return false;
            }
        } else if (m_boundTransformFeedbackBuffer == buffer) {
            synthesizeGLError(GL_INVALID_OPERATION, functionName,
                "a buffer bound to TRANSFORM_FEEDBACK_BUFFER can not be bound to any other targets");
            return false;
        }
    }

    switch (target) {
    case GL_ARRAY_BUFFER:
        m_boundArrayBuffer = buffer;
        break;
    case GL_COPY_READ_BUFFER:
        m_boundCopyReadBuffer = buffer;
        break;
    case GL_COPY_WRITE_BUFFER:
        m_boundCopyWriteBuffer = buffer;
        break;
    case GL_ELEMENT_ARRAY_BUFFER:
        m_boundVertexArrayObject->setElementArrayBuffer(buffer);
        break;
    case GL_PIXEL_PACK_BUFFER:
        m_boundPixelPackBuffer = buffer;
        break;
    case GL_PIXEL_UNPACK_BUFFER:
        m_boundPixelUnpackBuffer = buffer;
        break;
    case GL_TRANSFORM_FEEDBACK_BUFFER:
        m_boundTransformFeedbackBuffer = buffer;
        break;
    case GL_UNIFORM_BUFFER:
        m_boundUniformBuffer = buffer;
        break;
    default:
        ASSERT_NOT_REACHED();
        break;
    }

    if (buffer && !buffer->getInitialTarget())
        buffer->setInitialTarget(target);
    return true;
}

bool WebGL2RenderingContextBase::validateFramebufferTarget(GLenum target)
{
    switch (target) {
    case GL_FRAMEBUFFER:
    case GL_READ_FRAMEBUFFER:
    case GL_DRAW_FRAMEBUFFER:
        return true;
    default:
        return false;
    }
}

WebGLFramebuffer* WebGL2RenderingContextBase::getFramebufferBinding(GLenum target)
{
    if (target == GL_READ_FRAMEBUFFER)
        return m_readFramebufferBinding.get();
    return m_framebufferBinding.get();
}

bool WebGL2RenderingContextBase::validateGetFramebufferAttachmentParameterFunc(const char* functionName, GLenum target, GLenum attachment)
{
    if (!validateFramebufferTarget(target)) {
        synthesizeGLError(GL_INVALID_ENUM, functionName, "invalid target");
        return false;
    }

    WebGLFramebuffer* framebufferBinding = getFramebufferBinding(target);
    ASSERT(framebufferBinding || drawingBuffer());
    if (!framebufferBinding) {
    // for the default framebuffer
        switch (attachment) {
        case GL_BACK:
        case GL_DEPTH:
        case GL_STENCIL:
            break;
        default:
            synthesizeGLError(GL_INVALID_ENUM, functionName, "invalid attachment");
            return false;
        }
    } else {
    // for the FBO
        switch (attachment) {
        case GL_COLOR_ATTACHMENT0:
        case GL_DEPTH_ATTACHMENT:
        case GL_STENCIL_ATTACHMENT:
            break;
        case GL_DEPTH_STENCIL_ATTACHMENT:
            if (framebufferBinding->getAttachmentObject(GL_DEPTH_ATTACHMENT) != framebufferBinding->getAttachmentObject(GL_STENCIL_ATTACHMENT)) {
                synthesizeGLError(GL_INVALID_OPERATION, functionName, "different objects are bound to the depth and stencil attachment points");
                return false;
            }
            break;
        default:
            if (attachment > GL_COLOR_ATTACHMENT0
                && attachment < static_cast<GLenum>(GL_COLOR_ATTACHMENT0 + maxColorAttachments()))
                break;
            synthesizeGLError(GL_INVALID_ENUM, functionName, "invalid attachment");
            return false;
        }
    }
    return true;
}

ScriptValue WebGL2RenderingContextBase::getFramebufferAttachmentParameter(ScriptState* scriptState, GLenum target, GLenum attachment, GLenum pname)
{
    if (isContextLost() || !validateGetFramebufferAttachmentParameterFunc("getFramebufferAttachmentParameter", target, attachment))
        return ScriptValue::createNull(scriptState);

    WebGLFramebuffer* framebufferBinding = getFramebufferBinding(target);
    ASSERT(!framebufferBinding || framebufferBinding->object());

    WebGLSharedObject* attachmentObject = framebufferBinding ? framebufferBinding->getAttachmentObject(attachment) : 0;
    if (framebufferBinding && !attachmentObject) {
        if (pname == GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE)
            return WebGLAny(scriptState, GL_NONE);
        if (pname == GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME)
            return WebGLAny(scriptState, 0);
        synthesizeGLError(GL_INVALID_OPERATION, "getFramebufferAttachmentParameter", "invalid parameter name");
        return ScriptValue::createNull(scriptState);
    }

    switch (pname) {
    case GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE:
        if (!attachmentObject) {
            ASSERT(!framebufferBinding);
            return WebGLAny(scriptState, GL_FRAMEBUFFER_DEFAULT);
        }
        ASSERT(attachmentObject->isTexture() || attachmentObject->isRenderbuffer());
        if (attachmentObject->isTexture())
            return WebGLAny(scriptState, GL_TEXTURE);
        if (attachmentObject->isRenderbuffer())
            return WebGLAny(scriptState, GL_RENDERBUFFER);
        break;
    case GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME:
        if (!attachmentObject)
            break;
        return WebGLAny(scriptState, PassRefPtrWillBeRawPtr<WebGLObject>(attachmentObject));
    case GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE:
    case GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LAYER:
    case GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL:
        if (!attachmentObject || !attachmentObject->isTexture())
            break;
    case GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE:
    case GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE:
    case GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE:
    case GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE:
    case GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE:
    case GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE:
        {
            GLint value = 0;
            webContext()->getFramebufferAttachmentParameteriv(target, attachment, pname, &value);
            return WebGLAny(scriptState, value);
        }
    case GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE:
    case GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING:
        {
            GLint value = 0;
            webContext()->getFramebufferAttachmentParameteriv(target, attachment, pname, &value);
            return WebGLAny(scriptState, static_cast<unsigned>(value));
        }
    default:
        break;
    }
    synthesizeGLError(GL_INVALID_ENUM, "getFramebufferAttachmentParameter", "invalid parameter name");
    return ScriptValue::createNull(scriptState);
}

DEFINE_TRACE(WebGL2RenderingContextBase)
{
    visitor->trace(m_readFramebufferBinding);
    visitor->trace(m_transformFeedbackBinding);
    visitor->trace(m_boundCopyReadBuffer);
    visitor->trace(m_boundCopyWriteBuffer);
    visitor->trace(m_boundPixelPackBuffer);
    visitor->trace(m_boundPixelUnpackBuffer);
    visitor->trace(m_boundTransformFeedbackBuffer);
    visitor->trace(m_boundUniformBuffer);
    visitor->trace(m_currentBooleanOcclusionQuery);
    visitor->trace(m_currentTransformFeedbackPrimitivesWrittenQuery);
    visitor->trace(m_samplerUnits);
    WebGLRenderingContextBase::trace(visitor);
}

WebGLTexture* WebGL2RenderingContextBase::validateTextureBinding(const char* functionName, GLenum target, bool useSixEnumsForCubeMap)
{
    WebGLTexture* tex = nullptr;
    switch (target) {
    case GL_TEXTURE_2D_ARRAY:
        tex = m_textureUnits[m_activeTextureUnit].m_texture2DArrayBinding.get();
        if (!tex)
            synthesizeGLError(GL_INVALID_OPERATION, functionName, "no texture bound to GL_TEXTURE_2D_ARRAY");
        break;
    case GL_TEXTURE_3D:
        tex = m_textureUnits[m_activeTextureUnit].m_texture3DBinding.get();
        if (!tex)
            synthesizeGLError(GL_INVALID_OPERATION, functionName, "no texture bound to GL_TEXTURE_3D");
        break;
    default:
        return WebGLRenderingContextBase::validateTextureBinding(functionName, target, useSixEnumsForCubeMap);
    }
    return tex;
}

GLint WebGL2RenderingContextBase::getMaxTextureLevelForTarget(GLenum target)
{
    switch (target) {
    case GL_TEXTURE_3D:
        return m_max3DTextureLevel;
    case GL_TEXTURE_2D_ARRAY:
        return m_maxTextureLevel;
    }
    return WebGLRenderingContextBase::getMaxTextureLevelForTarget(target);
}

ScriptValue WebGL2RenderingContextBase::getTexParameter(ScriptState* scriptState, GLenum target, GLenum pname)
{
    if (isContextLost() || !validateTextureBinding("getTexParameter", target, false))
        return ScriptValue::createNull(scriptState);

    switch (pname) {
    case GL_TEXTURE_WRAP_R:
    case GL_TEXTURE_COMPARE_FUNC:
    case GL_TEXTURE_COMPARE_MODE:
    case GL_TEXTURE_IMMUTABLE_LEVELS:
        {
            GLint value = 0;
            webContext()->getTexParameteriv(target, pname, &value);
            return WebGLAny(scriptState, static_cast<unsigned>(value));
        }
    case GL_TEXTURE_IMMUTABLE_FORMAT:
        {
            GLint value = 0;
            webContext()->getTexParameteriv(target, pname, &value);
            return WebGLAny(scriptState, static_cast<bool>(value));
        }
    case GL_TEXTURE_BASE_LEVEL:
    case GL_TEXTURE_MAX_LEVEL:
        {
            GLint value = 0;
            webContext()->getTexParameteriv(target, pname, &value);
            return WebGLAny(scriptState, value);
        }
    case GL_TEXTURE_MAX_LOD:
    case GL_TEXTURE_MIN_LOD:
        {
            GLfloat value = 0.f;
            webContext()->getTexParameterfv(target, pname, &value);
            return WebGLAny(scriptState, value);
        }
    default:
        return WebGLRenderingContextBase::getTexParameter(scriptState, target, pname);
    }
}

WebGLBuffer* WebGL2RenderingContextBase::validateBufferDataTarget(const char* functionName, GLenum target)
{
    WebGLBuffer* buffer = nullptr;
    switch (target) {
    case GL_ELEMENT_ARRAY_BUFFER:
        buffer = m_boundVertexArrayObject->boundElementArrayBuffer().get();
        break;
    case GL_ARRAY_BUFFER:
        buffer = m_boundArrayBuffer.get();
        break;
    case GL_COPY_READ_BUFFER:
        buffer = m_boundCopyReadBuffer.get();
        break;
    case GL_COPY_WRITE_BUFFER:
        buffer = m_boundCopyWriteBuffer.get();
        break;
    case GL_PIXEL_PACK_BUFFER:
        buffer = m_boundPixelPackBuffer.get();
        break;
    case GL_PIXEL_UNPACK_BUFFER:
        buffer = m_boundPixelUnpackBuffer.get();
        break;
    case GL_TRANSFORM_FEEDBACK_BUFFER:
        buffer = m_boundTransformFeedbackBuffer.get();
        break;
    case GL_UNIFORM_BUFFER:
        buffer = m_boundUniformBuffer.get();
        break;
    default:
        synthesizeGLError(GL_INVALID_ENUM, functionName, "invalid target");
        return nullptr;
    }
    if (!buffer) {
        synthesizeGLError(GL_INVALID_OPERATION, functionName, "no buffer");
        return nullptr;
    }
    return buffer;
}

void WebGL2RenderingContextBase::removeBoundBuffer(WebGLBuffer* buffer)
{
    if (m_boundCopyReadBuffer == buffer)
        m_boundCopyReadBuffer = nullptr;
    if (m_boundCopyWriteBuffer == buffer)
        m_boundCopyWriteBuffer = nullptr;
    if (m_boundPixelPackBuffer == buffer)
        m_boundPixelPackBuffer = nullptr;
    if (m_boundPixelUnpackBuffer == buffer)
        m_boundPixelUnpackBuffer = nullptr;
    if (m_boundTransformFeedbackBuffer == buffer)
        m_boundTransformFeedbackBuffer = nullptr;
    if (m_boundUniformBuffer == buffer)
        m_boundUniformBuffer = nullptr;

    WebGLRenderingContextBase::removeBoundBuffer(buffer);
}

void WebGL2RenderingContextBase::restoreCurrentFramebuffer()
{
    bindFramebuffer(GL_DRAW_FRAMEBUFFER, m_framebufferBinding.get());
    bindFramebuffer(GL_READ_FRAMEBUFFER, m_readFramebufferBinding.get());
}

GLenum WebGL2RenderingContextBase::boundFramebufferColorFormat()
{
    if (m_readFramebufferBinding && m_readFramebufferBinding->object())
        return m_readFramebufferBinding->colorBufferFormat();
    if (m_requestedAttributes.alpha())
        return GL_RGBA;
    return GL_RGB;
}

} // namespace blink
