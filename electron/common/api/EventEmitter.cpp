// Copyright (c) 2014 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "common/api/EventEmitter.h"

#include "common/api/Event.h"
#include "gin/arguments.h"
#include "gin/dictionary.h"
#include "gin/object_template_builder.h"

namespace mate {

namespace {

v8::Persistent<v8::ObjectTemplate> eventTemplate;

void preventDefault(gin::Arguments* args) {
    v8::Local<v8::Object> obj;
    args->GetHolder(&obj);
    gin::Dictionary self(args->isolate(), obj);
    self.Set("defaultPrevented", true);
}

void sendReply(const std::string& json) {
    //     gin::Dictionary self(args->isolate(), args->GetThis());
    //     self.Set("defaultPrevented", true);
    DebugBreak();
}

// Create a pure JavaScript Event object.
v8::Local<v8::Object> createEventObject(v8::Isolate* isolate) {
    if (eventTemplate.IsEmpty()) {
        eventTemplate.Reset(isolate, gin::ObjectTemplateBuilder(isolate)
            .SetMethod("preventDefault", &preventDefault)
            .SetMethod("sendReply", &sendReply)
            .Build());
    }

    return v8::Local<v8::ObjectTemplate>::New(isolate, eventTemplate)->NewInstance();
}

}  // namespace

namespace internal {

v8::Local<v8::Object> createJSEvent(v8::Isolate* isolate, v8::Local<v8::Object> object) {
    v8::Local<v8::Object> event = createEventObject(isolate);
    gin::Dictionary(isolate, event).Set("sender", object);
    return event;
}

v8::Local<v8::Object> createJSEventWithSender(v8::Isolate* isolate, v8::Local<v8::Object> object, std::function<void(std::string)>&& callback) {
    Event* event = mate::Event::create(isolate, v8::Object::New(isolate), std::move(callback));
    v8::Local<v8::Object> wrapper = event->GetWrapper(isolate);
    gin::Dictionary(isolate, wrapper).Set("sender", object);
    return wrapper;
}

v8::Local<v8::Object> createCustomEvent(v8::Isolate* isolate, v8::Local<v8::Object> object, v8::Local<v8::Object> customEvent) {
    v8::Local<v8::Object> event = createEventObject(isolate);
    (void)event->SetPrototype(customEvent->CreationContext(), customEvent);
    gin::Dictionary(isolate, event).Set("sender", object);
    return event;
}

v8::Local<v8::Object> createEventFromFlags(v8::Isolate* isolate, int flags) {
//     gin::Dictionary obj = gin::Dictionary::CreateEmpty(isolate);
//     obj.Set("shiftKey", static_cast<bool>(flags & ui::EF_SHIFT_DOWN));
//     obj.Set("ctrlKey", static_cast<bool>(flags & ui::EF_CONTROL_DOWN));
//     obj.Set("altKey", static_cast<bool>(flags & ui::EF_ALT_DOWN));
//     obj.Set("metaKey", static_cast<bool>(flags & ui::EF_COMMAND_DOWN));
//     return obj.GetHandle();
    DebugBreak();
    return v8::Local<v8::Object>();
}

}  // namespace internal

}  // namespace mate
