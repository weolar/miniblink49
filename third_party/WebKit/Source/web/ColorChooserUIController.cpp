/*
 * Copyright (C) 2012 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1.  Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 * 2.  Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE AND ITS CONTRIBUTORS "AS IS" AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL APPLE OR ITS CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "web/ColorChooserUIController.h"

#include "core/html/forms/ColorChooserClient.h"
#include "platform/graphics/Color.h"
#include "public/platform/WebColor.h"
#include "public/web/WebColorChooser.h"
#include "public/web/WebColorSuggestion.h"
#include "public/web/WebFrameClient.h"
#include "web/WebLocalFrameImpl.h"

namespace blink {


ColorChooserUIController::ColorChooserUIController(LocalFrame* frame, ColorChooserClient* client)
    : m_client(client)
    , m_frame(frame)
{
}

ColorChooserUIController::~ColorChooserUIController()
{
    // The client cannot be accessed when finalizing.
    m_client = nullptr;
    endChooser();
}

DEFINE_TRACE(ColorChooserUIController)
{
    visitor->trace(m_frame);
    visitor->trace(m_client);
    ColorChooser::trace(visitor);
}

void ColorChooserUIController::openUI()
{
    openColorChooser();
}

void ColorChooserUIController::setSelectedColor(const Color& color)
{
    if (m_chooser)
        m_chooser->setSelectedColor(static_cast<WebColor>(color.rgb()));
}

void ColorChooserUIController::endChooser()
{
    if (m_chooser)
        m_chooser->endChooser();
}

AXObject* ColorChooserUIController::rootAXObject()
{
    return 0;
}

void ColorChooserUIController::didChooseColor(const WebColor& color)
{
    ASSERT(m_client);
    m_client->didChooseColor(Color(static_cast<RGBA32>(color)));
}

void ColorChooserUIController::didEndChooser()
{
    m_chooser = nullptr;
    if (m_client)
        m_client->didEndChooser();
}

void ColorChooserUIController::openColorChooser()
{
    ASSERT(!m_chooser);
    WebLocalFrameImpl* frame = WebLocalFrameImpl::fromFrame(m_frame);
    WebFrameClient* webFrameClient = frame->client();
    if (!webFrameClient)
        return;
    m_chooser = adoptPtr(webFrameClient->createColorChooser(
        this, static_cast<WebColor>(m_client->currentColor().rgb()), m_client->suggestions()));
}

} // namespace blink
