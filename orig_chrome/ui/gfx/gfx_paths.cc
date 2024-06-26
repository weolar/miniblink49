// Copyright (c) 2011 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/gfx_paths.h"

#include "base/command_line.h"
#include "base/files/file_path.h"
#include "base/files/file_util.h"
#include "base/path_service.h"

namespace gfx {

bool PathProvider(int key, base::FilePath* result)
{
    base::FilePath cur;
    switch (key) {
    // The following are only valid in the development environment, and
    // will fail if executed from an installed executable (because the
    // generated path won't exist).
    case DIR_TEST_DATA:
        if (!PathService::Get(base::DIR_SOURCE_ROOT, &cur))
            return false;
        cur = cur.Append(FILE_PATH_LITERAL("ui"));
        cur = cur.Append(FILE_PATH_LITERAL("gfx"));
        cur = cur.Append(FILE_PATH_LITERAL("test"));
        cur = cur.Append(FILE_PATH_LITERAL("data"));
        if (!base::PathExists(cur)) // we don't want to create this
            return false;
        break;
    default:
        return false;
    }

    *result = cur;
    return true;
}

// This cannot be done as a static initializer sadly since Visual Studio will
// eliminate this object file if there is no direct entry point into it.
void RegisterPathProvider()
{
    PathService::RegisterProvider(PathProvider, PATH_START, PATH_END);
}

} // namespace gfx
