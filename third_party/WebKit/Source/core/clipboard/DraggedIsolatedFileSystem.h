// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DraggedIsolatedFileSystem_h
#define DraggedIsolatedFileSystem_h

#include "core/CoreExport.h"
#include "platform/heap/Handle.h"
#include "wtf/Forward.h"

namespace blink {

class DataObject;

class CORE_EXPORT DraggedIsolatedFileSystem {
    WTF_MAKE_NONCOPYABLE(DraggedIsolatedFileSystem);
public:
    DraggedIsolatedFileSystem() { }
    virtual ~DraggedIsolatedFileSystem() { }

    using FileSystemIdPreparationCallback = void(*)(DataObject*, const String&);
    static void init(FileSystemIdPreparationCallback);

    static void prepareForDataObject(DataObject*, const String& filesystemId);

private:
    static FileSystemIdPreparationCallback s_prepareCallback;
};

} // namespace blink

#endif // DraggedIsolatedFileSystem_h
