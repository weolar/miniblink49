// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef WebWorkerContentSettingsClientProxy_h
#define WebWorkerContentSettingsClientProxy_h

namespace blink {

class WebString;

// Proxy interface to talk to the document's ContentSettingsClient
// implementation.
// This proxy is created by the embedder and is passed to the worker's
// WorkerGlobalScope in blink. Each allow method is called on the worker thread
// and may destructed on the worker thread.
class WebWorkerContentSettingsClientProxy {
public:
    virtual ~WebWorkerContentSettingsClientProxy() { }

    // Deprecated: This function should be removed.
    virtual bool allowDatabase(const WebString& name, const WebString& displayName, unsigned long estimatedSize)
    {
        return true;
    }

    virtual bool requestFileSystemAccessSync()
    {
        return true;
    }

    virtual bool allowIndexedDB(const WebString& name)
    {
        return true;
    }
};

} // namespace blink

#endif // WebWorkerContentSettingsClientProxy_h
