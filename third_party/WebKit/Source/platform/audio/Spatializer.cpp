// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#if ENABLE(WEB_AUDIO)
#include "platform/audio/Spatializer.h"

#include "platform/audio/StereoPanner.h"

namespace blink {

PassOwnPtr<Spatializer> Spatializer::create(PanningModel model, float sampleRate)
{
    switch (model) {
    case PanningModelEqualPower:
        return adoptPtr(new StereoPanner(sampleRate));
    default:
        ASSERT_NOT_REACHED();
        return nullptr;
    }
}

Spatializer::~Spatializer()
{
}

} // namespace blink

#endif // ENABLE(WEB_AUDIO)
