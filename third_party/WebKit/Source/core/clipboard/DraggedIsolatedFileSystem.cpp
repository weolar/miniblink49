// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/clipboard/DraggedIsolatedFileSystem.h"

namespace blink {

DraggedIsolatedFileSystem::FileSystemIdPreparationCallback DraggedIsolatedFileSystem::s_prepareCallback = nullptr;

void DraggedIsolatedFileSystem::init(DraggedIsolatedFileSystem::FileSystemIdPreparationCallback callback)
{
    ASSERT(!s_prepareCallback);
    s_prepareCallback = callback;
}

void DraggedIsolatedFileSystem::prepareForDataObject(DataObject* dataObject, const String& filesystemId)
{
    ASSERT(s_prepareCallback);
    (*s_prepareCallback)(dataObject, filesystemId);
}

}
