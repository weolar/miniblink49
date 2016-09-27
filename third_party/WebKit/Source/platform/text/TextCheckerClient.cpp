// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "platform/text/TextCheckerClient.h"

// This TextCheckerClient.cpp, which includes only config.h and
// TextCheckerClient.h, should be in Source/platform/text,
// because TextCheckerClient is not compiled without this cpp.
// So if we don't have this cpp, we will see unresolved symbol error
// when constructor/destructor's address is required.
