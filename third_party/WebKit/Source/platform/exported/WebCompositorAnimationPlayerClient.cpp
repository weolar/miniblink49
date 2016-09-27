// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "public/platform/WebCompositorAnimationPlayerClient.h"

// This WebCompositorAnimationPlayerClient.cpp, which includes only config.h
// and WebCompositorAnimationPlayerClient.h, should be in
// Source/platform/exported, because WebCompositorAnimationPlayerClient is not
// compiled without this cpp.
// So if we don't have this cpp, we will see unresolved symbol error
// when constructor/destructor's address is required.
