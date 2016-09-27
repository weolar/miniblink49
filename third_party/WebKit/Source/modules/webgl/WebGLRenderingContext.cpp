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
#include "modules/webgl/WebGLRenderingContext.h"

#include "core/frame/LocalFrame.h"
#include "core/frame/Settings.h"
#include "core/layout/LayoutBox.h"
#include "core/loader/FrameLoader.h"
#include "core/loader/FrameLoaderClient.h"
#include "modules/webgl/ANGLEInstancedArrays.h"
#include "modules/webgl/CHROMIUMSubscribeUniform.h"
#include "modules/webgl/EXTBlendMinMax.h"
#include "modules/webgl/EXTFragDepth.h"
#include "modules/webgl/EXTShaderTextureLOD.h"
#include "modules/webgl/EXTTextureFilterAnisotropic.h"
#include "modules/webgl/EXTsRGB.h"
#include "modules/webgl/OESElementIndexUint.h"
#include "modules/webgl/OESStandardDerivatives.h"
#include "modules/webgl/OESTextureFloat.h"
#include "modules/webgl/OESTextureFloatLinear.h"
#include "modules/webgl/OESTextureHalfFloat.h"
#include "modules/webgl/OESTextureHalfFloatLinear.h"
#include "modules/webgl/OESVertexArrayObject.h"
#include "modules/webgl/WebGLCompressedTextureATC.h"
#include "modules/webgl/WebGLCompressedTextureETC1.h"
#include "modules/webgl/WebGLCompressedTexturePVRTC.h"
#include "modules/webgl/WebGLCompressedTextureS3TC.h"
#include "modules/webgl/WebGLContextAttributeHelpers.h"
#include "modules/webgl/WebGLContextEvent.h"
#include "modules/webgl/WebGLDebugRendererInfo.h"
#include "modules/webgl/WebGLDebugShaders.h"
#include "modules/webgl/WebGLDepthTexture.h"
#include "modules/webgl/WebGLDrawBuffers.h"
#include "modules/webgl/WebGLLoseContext.h"
#include "platform/CheckedInt.h"
#include "platform/graphics/gpu/DrawingBuffer.h"
#include "public/platform/Platform.h"

namespace blink {

PassOwnPtrWillBeRawPtr<CanvasRenderingContext> WebGLRenderingContext::Factory::create(HTMLCanvasElement* canvas, const CanvasContextCreationAttributes& attrs, Document&)
{
    WebGLContextAttributes attributes = toWebGLContextAttributes(attrs);
    OwnPtr<WebGraphicsContext3D> context(createWebGraphicsContext3D(canvas, attributes, 1));
    if (!context)
        return nullptr;
    OwnPtr<Extensions3DUtil> extensionsUtil = Extensions3DUtil::create(context.get());
    if (!extensionsUtil)
        return nullptr;
    if (extensionsUtil->supportsExtension("GL_EXT_debug_marker")) {
        String contextLabel(String::format("WebGLRenderingContext-%p", context.get()));
        context->pushGroupMarkerEXT(contextLabel.ascii().data());
    }

    OwnPtrWillBeRawPtr<WebGLRenderingContext> renderingContext = adoptPtrWillBeNoop(new WebGLRenderingContext(canvas, context.release(), attributes));

    if (!renderingContext->drawingBuffer()) {
        canvas->dispatchEvent(WebGLContextEvent::create(EventTypeNames::webglcontextcreationerror, false, true, "Could not create a WebGL context."));
        return nullptr;
    }

    renderingContext->initializeNewContext();
    renderingContext->registerContextExtensions();

    return renderingContext.release();
}

void WebGLRenderingContext::Factory::onError(HTMLCanvasElement* canvas, const String& error)
{
    canvas->dispatchEvent(WebGLContextEvent::create(EventTypeNames::webglcontextcreationerror, false, true, error));
}

WebGLRenderingContext::WebGLRenderingContext(HTMLCanvasElement* passedCanvas, PassOwnPtr<WebGraphicsContext3D> context, const WebGLContextAttributes& requestedAttributes)
    : WebGLRenderingContextBase(passedCanvas, context, requestedAttributes)
{
}

WebGLRenderingContext::~WebGLRenderingContext()
{
}

void WebGLRenderingContext::registerContextExtensions()
{
    // Register extensions.
    static const char* const bothPrefixes[] = { "", "WEBKIT_", 0, };

    registerExtension<ANGLEInstancedArrays>(m_angleInstancedArrays);
    registerExtension<CHROMIUMSubscribeUniform>(m_chromiumSubscribeUniform);
    registerExtension<EXTBlendMinMax>(m_extBlendMinMax);
    registerExtension<EXTFragDepth>(m_extFragDepth);
    registerExtension<EXTShaderTextureLOD>(m_extShaderTextureLOD);
    registerExtension<EXTsRGB>(m_extsRGB);
    registerExtension<EXTTextureFilterAnisotropic>(m_extTextureFilterAnisotropic, ApprovedExtension, bothPrefixes);
    registerExtension<OESElementIndexUint>(m_oesElementIndexUint);
    registerExtension<OESStandardDerivatives>(m_oesStandardDerivatives);
    registerExtension<OESTextureFloat>(m_oesTextureFloat);
    registerExtension<OESTextureFloatLinear>(m_oesTextureFloatLinear);
    registerExtension<OESTextureHalfFloat>(m_oesTextureHalfFloat);
    registerExtension<OESTextureHalfFloatLinear>(m_oesTextureHalfFloatLinear);
    registerExtension<OESVertexArrayObject>(m_oesVertexArrayObject);
    registerExtension<WebGLCompressedTextureATC>(m_webglCompressedTextureATC, ApprovedExtension, bothPrefixes);
    registerExtension<WebGLCompressedTextureETC1>(m_webglCompressedTextureETC1);
    registerExtension<WebGLCompressedTexturePVRTC>(m_webglCompressedTexturePVRTC, ApprovedExtension, bothPrefixes);
    registerExtension<WebGLCompressedTextureS3TC>(m_webglCompressedTextureS3TC, ApprovedExtension, bothPrefixes);
    registerExtension<WebGLDebugRendererInfo>(m_webglDebugRendererInfo);
    registerExtension<WebGLDebugShaders>(m_webglDebugShaders);
    registerExtension<WebGLDepthTexture>(m_webglDepthTexture, ApprovedExtension, bothPrefixes);
    registerExtension<WebGLDrawBuffers>(m_webglDrawBuffers);
    registerExtension<WebGLLoseContext>(m_webglLoseContext, ApprovedExtension, bothPrefixes);
}

DEFINE_TRACE(WebGLRenderingContext)
{
    visitor->trace(m_angleInstancedArrays);
    visitor->trace(m_chromiumSubscribeUniform);
    visitor->trace(m_extBlendMinMax);
    visitor->trace(m_extFragDepth);
    visitor->trace(m_extShaderTextureLOD);
    visitor->trace(m_extsRGB);
    visitor->trace(m_extTextureFilterAnisotropic);
    visitor->trace(m_oesTextureFloat);
    visitor->trace(m_oesTextureFloatLinear);
    visitor->trace(m_oesTextureHalfFloat);
    visitor->trace(m_oesTextureHalfFloatLinear);
    visitor->trace(m_oesStandardDerivatives);
    visitor->trace(m_oesVertexArrayObject);
    visitor->trace(m_oesElementIndexUint);
    visitor->trace(m_webglLoseContext);
    visitor->trace(m_webglDebugRendererInfo);
    visitor->trace(m_webglDebugShaders);
    visitor->trace(m_webglDrawBuffers);
    visitor->trace(m_webglCompressedTextureATC);
    visitor->trace(m_webglCompressedTextureETC1);
    visitor->trace(m_webglCompressedTexturePVRTC);
    visitor->trace(m_webglCompressedTextureS3TC);
    visitor->trace(m_webglDepthTexture);
    WebGLRenderingContextBase::trace(visitor);
}

} // namespace blink
