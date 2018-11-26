// Copyright (c) 2016 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ATOM_COMMON_API_REMOTE_CALLBACK_FREER_H_
#define ATOM_COMMON_API_REMOTE_CALLBACK_FREER_H_

#include "browser/api/ApiWebContents.h"
#include "common/api/ObjectLifeMonitor.h"

#include "cef/include/base/cef_macros.h"

namespace atom {

class RemoteCallbackFreer : public ObjectLifeMonitor, public WebContentsObserver {
public:
    static void bindTo(v8::Isolate* isolate, v8::Local<v8::Object> target, int objectId, WebContents* webConents);

protected:
    RemoteCallbackFreer(v8::Isolate* isolate, v8::Local<v8::Object> target, int objectId, WebContents* webConents);
    ~RemoteCallbackFreer() override;

    // ObjectLifeMonitor
    virtual void onRunDestructor() override;

    // WebContentsObserver:
    virtual void onWebContentsDeleted(WebContents* webConents) override;

private:
    int m_objectId;
    WebContents* m_webContents;

    DISALLOW_COPY_AND_ASSIGN(RemoteCallbackFreer);
};

}  // namespace atom

#endif  // ATOM_COMMON_API_REMOTE_CALLBACK_FREER_H_
