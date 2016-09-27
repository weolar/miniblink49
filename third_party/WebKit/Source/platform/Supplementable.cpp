// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "platform/Supplementable.h"

// This Supplementable.cpp, which includes only config.h and
// Supplementable.h, should be in Source/platform,
// because Supplementable is not compiled without this cpp.
// So if we don't have this cpp, we will see unresolved symbol error
// when constructor/destructor's address is required.
// i.e. error LNK2005: "public: virtual __cdecl
// blink::SupplementTracing<0>::~SupplementTracing<0>(void)"
