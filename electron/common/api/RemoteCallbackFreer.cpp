// Copyright (c) 2016 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "common/api/RemoteCallbackFreer.h"

namespace atom {

RemoteCallbackFreer::RemoteCallbackFreer(v8::Isolate* isolate, v8::Local<v8::Object> target, int objectId, WebContents* webContents)
    : ObjectLifeMonitor(isolate, target)
    , m_webContents(webContents)
    , m_objectId(objectId)
{
    webContents->addObserver(this);
}

RemoteCallbackFreer::~RemoteCallbackFreer()
{
}

// static
void RemoteCallbackFreer::bindTo(v8::Isolate* isolate, v8::Local<v8::Object> target, int objectId, WebContents* webContents)
{
    new RemoteCallbackFreer(isolate, target, objectId, webContents);
}

void RemoteCallbackFreer::onRunDestructor()
{
    //     base::string16 channel = base::ASCIIToUTF16("ELECTRON_RENDERER_RELEASE_CALLBACK");
    //     base::ListValue args;
    //     args.AppendInteger(m_objectId);
    //     Send(new AtomViewMsg_Message(routing_id(), false, channel, args));
//     DebugBreak();
//     m_webContents->removeObserver(nullptr);
}

void RemoteCallbackFreer::onWebContentsDeleted(WebContents* webConents)
{
    delete this;
}

} // namespace atom
