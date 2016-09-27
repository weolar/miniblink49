// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef DevToolsHostFileSystem_h
#define DevToolsHostFileSystem_h

#include "platform/heap/Handle.h"
#include "wtf/text/WTFString.h"

namespace blink {

class DOMFileSystem;
class DevToolsHost;

class DevToolsHostFileSystem {
public:
    static DOMFileSystem* isolatedFileSystem(DevToolsHost&, const String& fileSystemName, const String& rootURL);
    static void upgradeDraggedFileSystemPermissions(DevToolsHost&, DOMFileSystem*);
private:
    DevToolsHostFileSystem();
    ~DevToolsHostFileSystem();
};

} // namespace blink

#endif // !defined(DevToolsHostFileSystem_h)
