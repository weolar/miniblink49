// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_INPUT_TOP_CONTROLS_MANAGER_CLIENT_H_
#define CC_INPUT_TOP_CONTROLS_MANAGER_CLIENT_H_

namespace cc {

class LayerTreeImpl;

class CC_EXPORT TopControlsManagerClient {
public:
    virtual float TopControlsHeight() const = 0;
    virtual void SetCurrentTopControlsShownRatio(float ratio) = 0;
    virtual float CurrentTopControlsShownRatio() const = 0;
    virtual void DidChangeTopControlsPosition() = 0;
    virtual bool HaveRootScrollLayer() const = 0;

protected:
    virtual ~TopControlsManagerClient() { }
};

} // namespace cc

#endif // CC_INPUT_TOP_CONTROLS_MANAGER_CLIENT_H_
