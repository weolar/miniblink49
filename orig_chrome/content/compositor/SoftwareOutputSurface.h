// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef content_compositor_SoftwareOutputSurface_h
#define content_compositor_SoftwareOutputSurface_h

#include "cc/output/output_surface.h"

namespace content {

class WebPageOcBridge;

class SoftwareOutputSurface : public cc::OutputSurface {
public:
    static scoped_ptr<SoftwareOutputSurface> Create(WebPageOcBridge* webPageOcBridge);
    virtual ~SoftwareOutputSurface();

    // cc::OutputSurface
    virtual void SwapBuffers(cc::CompositorFrame* frame) override;
    virtual bool BindToClient(cc::OutputSurfaceClient* client) override;

    void firePaintEvent(HDC hdc, const RECT& paintRect);

private:
    SoftwareOutputSurface(WebPageOcBridge* webPageOcBridge);
    void Initialize();
};

} // content

#endif // content_compositor_SoftwareOutputSurface_h