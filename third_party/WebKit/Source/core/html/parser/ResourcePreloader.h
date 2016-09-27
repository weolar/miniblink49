// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ResourcePreloader_h
#define ResourcePreloader_h

#include "core/CoreExport.h"
#include "core/html/parser/PreloadRequest.h"

namespace blink {

class CORE_EXPORT ResourcePreloader {
public:
    virtual void takeAndPreload(PreloadRequestStream&);
private:
    virtual void preload(PassOwnPtr<PreloadRequest>) = 0;
};

}

#endif
