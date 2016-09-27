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

#include "config.h"
#include "web/FullscreenController.h"

#include "core/dom/Document.h"
#include "core/dom/Fullscreen.h"
#include "core/frame/LocalFrame.h"
#include "core/frame/PageScaleConstraintsSet.h"
#include "core/html/HTMLMediaElement.h"
#include "core/html/HTMLVideoElement.h"
#include "platform/LayoutTestSupport.h"
#include "platform/RuntimeEnabledFeatures.h"
#include "public/platform/WebLayerTreeView.h"
#include "public/web/WebFrameClient.h"
#include "web/WebLocalFrameImpl.h"
#include "web/WebSettingsImpl.h"
#include "web/WebViewImpl.h"

namespace blink {

PassOwnPtrWillBeRawPtr<FullscreenController> FullscreenController::create(WebViewImpl* webViewImpl)
{
    return adoptPtrWillBeNoop(new FullscreenController(webViewImpl));
}

FullscreenController::FullscreenController(WebViewImpl* webViewImpl)
{
}

void FullscreenController::didEnterFullScreen()
{
    
}

void FullscreenController::didExitFullScreen()
{
    
}

void FullscreenController::enterFullScreenForElement(Element* element)
{
}

void FullscreenController::exitFullScreenForElement(Element* element)
{
}

void FullscreenController::updateSize()
{
}

void FullscreenController::updatePageScaleConstraints(bool removeConstraints)
{
}

DEFINE_TRACE(FullscreenController)
{

}

} // namespace blink

