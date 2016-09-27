/*
 * Copyright (C) 2011 Google Inc. All rights reserved.
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

#ifndef ColorChooserUIController_h
#define ColorChooserUIController_h

#include "core/html/forms/ColorChooser.h"
#include "platform/heap/Handle.h"
#include "platform/text/PlatformLocale.h"
#include "public/web/WebColorChooserClient.h"
#include "wtf/OwnPtr.h"

namespace blink {

class ColorChooserClient;
class LocalFrame;
class WebColorChooser;

class ColorChooserUIController : public NoBaseWillBeGarbageCollectedFinalized<ColorChooserUIController>, public WebColorChooserClient, public ColorChooser {
    WILL_BE_USING_GARBAGE_COLLECTED_MIXIN(ColorChooserUIController);
public:
    static PassOwnPtrWillBeRawPtr<ColorChooserUIController> create(LocalFrame* frame, ColorChooserClient* client)
    {
        return adoptPtrWillBeNoop(new ColorChooserUIController(frame, client));
    }

    ~ColorChooserUIController() override;
    DECLARE_VIRTUAL_TRACE();

    virtual void openUI();

    // ColorChooser functions:
    void setSelectedColor(const Color&) final;
    void endChooser() override;
    AXObject* rootAXObject() override;

    // WebColorChooserClient functions:
    void didChooseColor(const WebColor&) final;
    void didEndChooser() final;

protected:
    ColorChooserUIController(LocalFrame*, ColorChooserClient*);

    void openColorChooser();
    OwnPtr<WebColorChooser> m_chooser;
    RawPtrWillBeMember<ColorChooserClient> m_client;

private:
    RawPtrWillBeMember<LocalFrame> m_frame;
};

} // namespace blink

#endif // ColorChooserUIController_h
