// Copyright (c) 2016 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "common/api/RemoteObjectFreer.h"

// #include "third_party/WebKit/public/web/WebLocalFrame.h"
// #include "third_party/WebKit/public/web/WebView.h"

// using blink::WebLocalFrame;
// using blink::WebView;

namespace atom {

namespace {

    // content::RenderView* GetCurrentRenderView() {
    //   WebLocalFrame* frame = WebLocalFrame::frameForCurrentContext();
    //   if (!frame)
    //     return nullptr;
    //
    //   WebView* view = frame->view();
    //   if (!view)
    //     return nullptr;  // can happen during closing.
    //
    //   return content::RenderView::FromWebView(view);
    // }

} // namespace

// static
void RemoteObjectFreer::bindTo(v8::Isolate* isolate, v8::Local<v8::Object> target, int objectId)
{
    new RemoteObjectFreer(isolate, target, objectId);
}

RemoteObjectFreer::RemoteObjectFreer(v8::Isolate* isolate, v8::Local<v8::Object> target, int objectId)
    : ObjectLifeMonitor(isolate, target)
    , m_objectId(objectId)
{
}

RemoteObjectFreer::~RemoteObjectFreer()
{
}

void RemoteObjectFreer::onRunDestructor()
{
    // TODO
    //   content::RenderView* render_view = GetCurrentRenderView();
    //   if (!render_view)
    //     return;
    //
    //   base::string16 channel = base::ASCIIToUTF16("ipc-message");
    //   base::ListValue args;
    //   args.AppendString("ELECTRON_BROWSER_DEREFERENCE");
    //   args.AppendInteger(object_id_);
    //   render_view->Send(
    //       new AtomViewHostMsg_Message(render_view->GetRoutingID(), channel, args));
}

} // namespace atom
