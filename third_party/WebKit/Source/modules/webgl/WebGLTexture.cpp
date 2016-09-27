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

#include "config.h"

#include "modules/webgl/WebGLTexture.h"

#include "modules/webgl/WebGLRenderingContextBase.h"

namespace blink {

PassRefPtrWillBeRawPtr<WebGLTexture> WebGLTexture::create(WebGLRenderingContextBase* ctx)
{
    return adoptRefWillBeNoop(new WebGLTexture(ctx));
}

WebGLTexture::WebGLTexture(WebGLRenderingContextBase* ctx)
    : WebGLSharedPlatform3DObject(ctx)
    , m_target(0)
    , m_minFilter(GL_NEAREST_MIPMAP_LINEAR)
    , m_magFilter(GL_LINEAR)
    , m_wrapR(GL_REPEAT)
    , m_wrapS(GL_REPEAT)
    , m_wrapT(GL_REPEAT)
    , m_isNPOT(false)
    , m_isCubeComplete(false)
    , m_isComplete(false)
    , m_needToUseBlackTexture(false)
    , m_isFloatType(false)
    , m_isHalfFloatType(false)
    , m_isWebGL2OrHigher(ctx->isWebGL2OrHigher())
    , m_immutable(false)
    , m_baseLevel(0)
    , m_maxLevel(1000)
{
    setObject(ctx->webContext()->createTexture());
}

WebGLTexture::~WebGLTexture()
{
    // Always perform detach here to ensure that platform object
    // deletion happens with Oilpan enabled. It keeps the code regular
    // to do it with or without Oilpan enabled.
    //
    // See comment in WebGLBuffer's destructor for additional
    // information on why this is done for WebGLSharedObject-derived
    // objects.
    detachAndDeleteObject();
}

void WebGLTexture::setTarget(GLenum target, GLint maxLevel)
{
    if (!object())
        return;
    // Target is finalized the first time bindTexture() is called.
    if (m_target)
        return;
    switch (target) {
    case GL_TEXTURE_2D:
    case GL_TEXTURE_2D_ARRAY:
    case GL_TEXTURE_3D:
        m_target = target;
        m_info.resize(1);
        m_info[0].resize(maxLevel);
        break;
    case GL_TEXTURE_CUBE_MAP:
        m_target = target;
        m_info.resize(6);
        for (int ii = 0; ii < 6; ++ii)
            m_info[ii].resize(maxLevel);
        break;
    }
}

void WebGLTexture::setParameteri(GLenum pname, GLint param)
{
    if (!object() || !m_target)
        return;
    switch (pname) {
    case GL_TEXTURE_MIN_FILTER:
        switch (param) {
        case GL_NEAREST:
        case GL_LINEAR:
        case GL_NEAREST_MIPMAP_NEAREST:
        case GL_LINEAR_MIPMAP_NEAREST:
        case GL_NEAREST_MIPMAP_LINEAR:
        case GL_LINEAR_MIPMAP_LINEAR:
            m_minFilter = param;
            break;
        }
        break;
    case GL_TEXTURE_MAG_FILTER:
        switch (param) {
        case GL_NEAREST:
        case GL_LINEAR:
            m_magFilter = param;
            break;
        }
        break;
    case GL_TEXTURE_WRAP_R:
        switch (param) {
        case GL_CLAMP_TO_EDGE:
        case GL_MIRRORED_REPEAT:
        case GL_REPEAT:
            m_wrapR = param;
            break;
        }
        break;
    case GL_TEXTURE_WRAP_S:
        switch (param) {
        case GL_CLAMP_TO_EDGE:
        case GL_MIRRORED_REPEAT:
        case GL_REPEAT:
            m_wrapS = param;
            break;
        }
        break;
    case GL_TEXTURE_WRAP_T:
        switch (param) {
        case GL_CLAMP_TO_EDGE:
        case GL_MIRRORED_REPEAT:
        case GL_REPEAT:
            m_wrapT = param;
            break;
        }
        break;
    case GL_TEXTURE_BASE_LEVEL:
        if (m_isWebGL2OrHigher && param >= 0)
            m_baseLevel = param;
        break;
    case GL_TEXTURE_MAX_LEVEL:
        if (m_isWebGL2OrHigher && param >= 0)
            m_maxLevel = param;
        break;
    default:
        return;
    }
    update();
}

void WebGLTexture::setParameterf(GLenum pname, GLfloat param)
{
    if (!object() || !m_target)
        return;
    GLint iparam = static_cast<GLint>(param);
    setParameteri(pname, iparam);
}

void WebGLTexture::setLevelInfo(GLenum target, GLint level, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth, GLenum type)
{
    ASSERT(!m_immutable);

    if (!object() || !m_target)
        return;
    // We assume level, internalFormat, width, height, depth, and type have all been
    // validated already.
    int index = mapTargetToIndex(target);
    if (index < 0)
        return;
    m_info[index][level].setInfo(internalFormat, width, height, depth, type);
    update();
}

void WebGLTexture::setTexStorageInfo(GLenum target, GLint levels, GLenum internalFormat, GLsizei width, GLsizei height, GLsizei depth)
{
    ASSERT(!m_immutable);

    // We assume level, internalFormat, width, height, and depth have all been
    // validated already.
    if (!object() || !m_target || target != m_target)
        return;

    GLenum type = getValidTypeForInternalFormat(internalFormat);
    if (type == GL_NONE)
        return;

    for (size_t ii = 0; ii < m_info.size(); ++ii) {
        GLsizei levelWidth = width;
        GLsizei levelHeight = height;
        GLsizei levelDepth = depth;
        for (GLint level = 0; level < levels; ++level) {
            LevelInfo& info = m_info[ii][level];
            info.setInfo(internalFormat, levelWidth, levelHeight, levelDepth, type);
            levelWidth = std::max(1, levelWidth >> 1);
            levelHeight = std::max(1, levelHeight >> 1);
            levelDepth = std::max(1, levelDepth >> 1);
        }
    }
    update();

    m_immutable = true;
}

void WebGLTexture::generateMipmapLevelInfo()
{
    if (!object() || !m_target)
        return;
    if (!canGenerateMipmaps())
        return;
    if (!m_isComplete) {
        size_t baseLevel = std::min(m_baseLevel, m_info[0].size() - 1);
        for (size_t ii = 0; ii < m_info.size(); ++ii) {
            const LevelInfo& info0 = m_info[ii][baseLevel];
            GLsizei width = info0.width;
            GLsizei height = info0.height;
            GLsizei depth = info0.depth;
            GLint levelCount = computeLevelCount(width, height, depth);
            size_t maxLevel = m_isWebGL2OrHigher ? std::min(m_maxLevel, baseLevel + levelCount - 1) : baseLevel + levelCount - 1;
            for (size_t level = baseLevel + 1; level <= maxLevel; ++level) {
                width = std::max(1, width >> 1);
                height = std::max(1, height >> 1);
                depth = std::max(1, depth >> 1);
                LevelInfo& info = m_info[ii][level];
                info.setInfo(info0.internalFormat, width, height, depth, info0.type);
            }
        }
        m_isComplete = true;
    }
    m_needToUseBlackTexture = false;
}

GLenum WebGLTexture::getInternalFormat(GLenum target, GLint level) const
{
    const LevelInfo* info = getLevelInfo(target, level);
    if (!info)
        return 0;
    return info->internalFormat;
}

GLenum WebGLTexture::getType(GLenum target, GLint level) const
{
    const LevelInfo* info = getLevelInfo(target, level);
    if (!info)
        return 0;
    return info->type;
}

GLsizei WebGLTexture::getWidth(GLenum target, GLint level) const
{
    const LevelInfo* info = getLevelInfo(target, level);
    if (!info)
        return 0;
    return info->width;
}

GLsizei WebGLTexture::getHeight(GLenum target, GLint level) const
{
    const LevelInfo* info = getLevelInfo(target, level);
    if (!info)
        return 0;
    return info->height;
}

GLsizei WebGLTexture::getDepth(GLenum target, GLint level) const
{
    const LevelInfo* info = getLevelInfo(target, level);
    if (!info)
        return 0;
    return info->depth;
}

bool WebGLTexture::isValid(GLenum target, GLint level) const
{
    const LevelInfo* info = getLevelInfo(target, level);
    if (!info)
        return 0;
    return info->valid;
}

bool WebGLTexture::isNPOT(GLsizei width, GLsizei height)
{
    ASSERT(width >= 0 && height >= 0);
    if (!width || !height)
        return false;
    if ((width & (width - 1)) || (height & (height - 1)))
        return true;
    return false;
}

bool WebGLTexture::isNPOT() const
{
    if (!object())
        return false;
    return m_isNPOT;
}

bool WebGLTexture::needToUseBlackTexture(TextureExtensionFlag flag) const
{
    if (!object())
        return false;
    if (m_needToUseBlackTexture)
        return true;
    if ((m_isFloatType && !(flag & TextureFloatLinearExtensionEnabled)) || (m_isHalfFloatType && !(flag && TextureHalfFloatLinearExtensionEnabled))) {
        if (m_magFilter != GL_NEAREST || (m_minFilter != GL_NEAREST && m_minFilter != GL_NEAREST_MIPMAP_NEAREST))
            return true;
    }
    return false;
}

void WebGLTexture::deleteObjectImpl(WebGraphicsContext3D* context3d)
{
    context3d->deleteTexture(m_object);
    m_object = 0;
}

int WebGLTexture::mapTargetToIndex(GLenum target) const
{
    if (m_target == GL_TEXTURE_2D) {
        if (target == GL_TEXTURE_2D)
            return 0;
    } else if (m_target == GL_TEXTURE_CUBE_MAP) {
        switch (target) {
        case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
            return 0;
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
            return 1;
        case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
            return 2;
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
            return 3;
        case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
            return 4;
        case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
            return 5;
        }
    } else if (m_target == GL_TEXTURE_3D) {
        if (target == GL_TEXTURE_3D)
            return 0;
    } else if (m_target == GL_TEXTURE_2D_ARRAY) {
        if (target == GL_TEXTURE_2D_ARRAY)
            return 0;
    }
    return -1;
}

bool WebGLTexture::canGenerateMipmaps()
{
    if (!m_isWebGL2OrHigher && isNPOT())
        return false;

    size_t baseLevel = std::min(m_baseLevel, m_info[0].size() - 1);
    const LevelInfo& base = m_info[0][baseLevel];
    for (size_t ii = 0; ii < m_info.size(); ++ii) {
        const LevelInfo& info = m_info[ii][baseLevel];
        if (!info.valid
            || info.width != base.width || info.height != base.height || info.depth != base.depth
            || info.internalFormat != base.internalFormat || info.type != base.type
            || (m_info.size() > 1 && !m_isCubeComplete))
            return false;
    }
    return true;
}

GLint WebGLTexture::computeLevelCount(GLsizei width, GLsizei height, GLsizei depth)
{
    // return 1 + log2Floor(std::max(width, height));
    GLsizei n = std::max(std::max(width, height), depth);
    if (n <= 0)
        return 0;
    GLint log = 0;
    GLsizei value = n;
    for (int ii = 4; ii >= 0; --ii) {
        int shift = (1 << ii);
        GLsizei x = (value >> shift);
        if (x) {
            value = x;
            log += shift;
        }
    }
    ASSERT(value == 1);
    return log + 1;
}

void WebGLTexture::update()
{
    m_isNPOT = false;
    for (size_t ii = 0; ii < m_info.size(); ++ii) {
        if (isNPOT(m_info[ii][0].width, m_info[ii][0].height)) {
            m_isNPOT = true;
            break;
        }
    }
    m_isComplete = true;
    m_isCubeComplete = true;

    size_t baseLevel = std::min(m_baseLevel, m_info[0].size() - 1);
    const LevelInfo& base = m_info[0][baseLevel];
    size_t levelCount = computeLevelCount(base.width, base.height, base.depth);
    size_t maxLevel = m_isWebGL2OrHigher ? std::min(m_maxLevel, baseLevel + levelCount - 1) : baseLevel + levelCount - 1;

    if (baseLevel > maxLevel) {
        m_isComplete = false;
    }
    else {
        for (size_t ii = 0; ii < m_info.size() && m_isComplete; ++ii) {
            const LevelInfo& info0 = m_info[ii][baseLevel];
            if (!info0.valid
                || info0.width != base.width || info0.height != base.height || info0.depth != base.depth
                || info0.internalFormat != base.internalFormat || info0.type != base.type
                || (m_info.size() > 1 && info0.width != info0.height)) {
                if (m_info.size() > 1)
                    m_isCubeComplete = false;
                m_isComplete = false;
                break;
            }
            GLsizei width = info0.width;
            GLsizei height = info0.height;
            GLsizei depth = info0.depth;
            for (size_t level = baseLevel + 1; level <= maxLevel; ++level) {
                width = std::max(1, width >> 1);
                height = std::max(1, height >> 1);
                depth = std::max(1, depth >> 1);
                const LevelInfo& info = m_info[ii][level];
                if (!info.valid
                    || info.width != width || info.height != height || info.depth != depth
                    || info.internalFormat != info0.internalFormat || info.type != info0.type) {
                    m_isComplete = false;
                    break;
                }

            }
        }
    }
    m_isFloatType = m_info[0][0].type == GL_FLOAT;
    m_isHalfFloatType = m_info[0][0].type == GL_HALF_FLOAT_OES;

    m_needToUseBlackTexture = false;
    // NPOT
    if (!m_isWebGL2OrHigher && m_isNPOT && ((m_minFilter != GL_NEAREST && m_minFilter != GL_LINEAR)
        || m_wrapS != GL_CLAMP_TO_EDGE || m_wrapT != GL_CLAMP_TO_EDGE || (m_target == GL_TEXTURE_3D && m_wrapR != GL_CLAMP_TO_EDGE)))
        m_needToUseBlackTexture = true;
    // If it is a Cube texture, check Cube Completeness first
    if (m_info.size() > 1 && !m_isCubeComplete)
        m_needToUseBlackTexture = true;
    // Completeness
    if (!m_isComplete && m_minFilter != GL_NEAREST && m_minFilter != GL_LINEAR)
        m_needToUseBlackTexture = true;
}

const WebGLTexture::LevelInfo* WebGLTexture::getLevelInfo(GLenum target, GLint level) const
{
    if (!object() || !m_target)
        return nullptr;
    int targetIndex = mapTargetToIndex(target);
    if (targetIndex < 0 || targetIndex >= static_cast<int>(m_info.size()))
        return nullptr;
    if (level < 0 || level >= static_cast<GLint>(m_info[targetIndex].size()))
        return nullptr;
    return &(m_info[targetIndex][level]);
}

// TODO(bajones): Logic surrounding relationship of internalFormat, format, and type needs to be revisisted for WebGL 2.0
GLenum WebGLTexture::getValidTypeForInternalFormat(GLenum internalFormat)
{
    switch (internalFormat) {
    case GL_R8:
        return GL_UNSIGNED_BYTE;
    case GL_R8_SNORM:
        return GL_BYTE;
    case GL_R16F:
        return GL_HALF_FLOAT;
    case GL_R32F:
        return GL_FLOAT;
    case GL_R8UI:
        return GL_UNSIGNED_BYTE;
    case GL_R8I:
        return GL_BYTE;
    case GL_R16UI:
        return GL_UNSIGNED_SHORT;
    case GL_R16I:
        return GL_SHORT;
    case GL_R32UI:
        return GL_UNSIGNED_INT;
    case GL_R32I:
        return GL_INT;
    case GL_RG8:
        return GL_UNSIGNED_BYTE;
    case GL_RG8_SNORM:
        return GL_BYTE;
    case GL_RG16F:
        return GL_HALF_FLOAT;
    case GL_RG32F:
        return GL_FLOAT;
    case GL_RG8UI:
        return GL_UNSIGNED_BYTE;
    case GL_RG8I:
        return GL_BYTE;
    case GL_RG16UI:
        return GL_UNSIGNED_SHORT;
    case GL_RG16I:
        return GL_SHORT;
    case GL_RG32UI:
        return GL_UNSIGNED_INT;
    case GL_RG32I:
        return GL_INT;
    case GL_RGB8:
        return GL_UNSIGNED_BYTE;
    case GL_SRGB8:
        return GL_UNSIGNED_BYTE;
    case GL_RGB565:
        return GL_UNSIGNED_SHORT_5_6_5;
    case GL_RGB8_SNORM:
        return GL_BYTE;
    case GL_R11F_G11F_B10F:
        return GL_UNSIGNED_INT_10F_11F_11F_REV;
    case GL_RGB9_E5:
        return GL_UNSIGNED_INT_5_9_9_9_REV;
    case GL_RGB16F:
        return GL_HALF_FLOAT;
    case GL_RGB32F:
        return GL_FLOAT;
    case GL_RGB8UI:
        return GL_UNSIGNED_BYTE;
    case GL_RGB8I:
        return GL_BYTE;
    case GL_RGB16UI:
        return GL_UNSIGNED_SHORT;
    case GL_RGB16I:
        return GL_SHORT;
    case GL_RGB32UI:
        return GL_UNSIGNED_INT;
    case GL_RGB32I:
        return GL_INT;
    case GL_RGBA8:
        return GL_UNSIGNED_BYTE;
    case GL_SRGB8_ALPHA8:
        return GL_UNSIGNED_BYTE;
    case GL_RGBA8_SNORM:
        return GL_BYTE;
    case GL_RGB5_A1:
        return GL_UNSIGNED_SHORT_5_5_5_1;
    case GL_RGBA4:
        return GL_UNSIGNED_SHORT_4_4_4_4;
    case GL_RGB10_A2:
        return GL_UNSIGNED_INT_2_10_10_10_REV;
    case GL_RGBA16F:
        return GL_HALF_FLOAT;
    case GL_RGBA32F:
        return GL_FLOAT;
    case GL_RGBA8UI:
        return GL_UNSIGNED_BYTE;
    case GL_RGBA8I:
        return GL_BYTE;
    case GL_RGB10_A2UI:
        return GL_UNSIGNED_INT_2_10_10_10_REV;
    case GL_RGBA16UI:
        return GL_UNSIGNED_SHORT;
    case GL_RGBA16I:
        return GL_SHORT;
    case GL_RGBA32I:
        return GL_INT;
    case GL_RGBA32UI:
        return GL_UNSIGNED_INT;
    case GL_DEPTH_COMPONENT16:
        return GL_UNSIGNED_SHORT;
    case GL_DEPTH_COMPONENT24:
        return GL_UNSIGNED_INT;
    case GL_DEPTH_COMPONENT32F:
        return GL_FLOAT;
    case GL_DEPTH24_STENCIL8:
        return GL_UNSIGNED_INT_24_8;
    case GL_DEPTH32F_STENCIL8:
        return GL_FLOAT_32_UNSIGNED_INT_24_8_REV;
    default:
        return GL_NONE;
    }
}

GLenum WebGLTexture::getValidFormatForInternalFormat(GLenum internalFormat)
{
    switch (internalFormat) {
    case GL_R8:
    case GL_R8_SNORM:
    case GL_R16F:
    case GL_R32F:
        return GL_RED;
    case GL_R8UI:
    case GL_R8I:
    case GL_R16UI:
    case GL_R16I:
    case GL_R32UI:
    case GL_R32I:
        return GL_RED_INTEGER;
    case GL_RG8:
    case GL_RG8_SNORM:
    case GL_RG16F:
    case GL_RG32F:
        return GL_RG;
    case GL_RG8UI:
    case GL_RG8I:
    case GL_RG16UI:
    case GL_RG16I:
    case GL_RG32UI:
    case GL_RG32I:
        return GL_RG_INTEGER;
    case GL_RGB8:
    case GL_SRGB8:
    case GL_RGB565:
    case GL_RGB8_SNORM:
    case GL_R11F_G11F_B10F:
    case GL_RGB9_E5:
    case GL_RGB16F:
    case GL_RGB32F:
        return GL_RGB;
    case GL_RGB8UI:
    case GL_RGB8I:
    case GL_RGB16UI:
    case GL_RGB16I:
    case GL_RGB32UI:
    case GL_RGB32I:
        return GL_RGB_INTEGER;
    case GL_RGBA8:
    case GL_SRGB8_ALPHA8:
    case GL_RGBA8_SNORM:
    case GL_RGB5_A1:
    case GL_RGBA4:
    case GL_RGB10_A2:
    case GL_RGBA16F:
    case GL_RGBA32F:
        return GL_RGBA;
    case GL_RGBA8UI:
    case GL_RGBA8I:
    case GL_RGB10_A2UI:
    case GL_RGBA16UI:
    case GL_RGBA16I:
    case GL_RGBA32I:
    case GL_RGBA32UI:
        return GL_RGBA_INTEGER;
    case GL_DEPTH_COMPONENT16:
    case GL_DEPTH_COMPONENT24:
    case GL_DEPTH_COMPONENT32F:
        return GL_DEPTH_COMPONENT;
    case GL_DEPTH24_STENCIL8:
    case GL_DEPTH32F_STENCIL8:
        return GL_DEPTH_STENCIL;
    default:
        return GL_NONE;
    }
}

} // namespace blink
