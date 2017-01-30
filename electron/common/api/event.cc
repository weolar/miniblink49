// Copyright (c) 2014 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "common/api/event.h"

#include "gin/converter.h"
#include "gin/object_template_builder.h"

namespace mate {

gin::WrapperInfo Event::kWrapperInfo = { gin::kEmbedderNativeGin };

Event::Event(v8::Isolate* isolate) {

}

Event::~Event() {
}

void Event::PreventDefault(v8::Isolate* isolate) {
    GetWrapper(isolate)->Set(gin::StringToV8(isolate, "defaultPrevented"), v8::True(isolate));
}

bool Event::SendReply(const std::string& json) {
    return false;
}

// static
gin::Handle<Event> Event::Create(v8::Isolate* isolate) {
    return gin::CreateHandle(isolate, new Event(isolate));
}

// static
void Event::BuildPrototype(v8::Isolate* isolate, v8::Local<v8::FunctionTemplate> prototype) {
    prototype->SetClassName(gin::StringToV8(isolate, "Event"));
    gin::ObjectTemplateBuilder(isolate/*, prototype->PrototypeTemplate()*/)
        .SetMethod("preventDefault", &Event::PreventDefault)
        .SetMethod("sendReply", &Event::SendReply);
}

}  // namespace mate
