// Copyright (c) 2014 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "common/api/event_emitter.h"

#include "common/api/event.h"
#include "gin/arguments.h"
#include "gin/dictionary.h"
#include "gin/object_template_builder.h"
//#include "ui/events/event_constants.h"

//#include "atom/common/node_includes.h"

namespace mate {

namespace {

v8::Persistent<v8::ObjectTemplate> event_template;

void PreventDefault(gin::Arguments* args) {
//     gin::Dictionary self(args->isolate(), args->GetThis());
//     self.Set("defaultPrevented", true);
    DebugBreak();
}

// Create a pure JavaScript Event object.
v8::Local<v8::Object> CreateEventObject(v8::Isolate* isolate) {
    if (event_template.IsEmpty()) {
        event_template.Reset(isolate, gin::ObjectTemplateBuilder(isolate)
            .SetMethod("preventDefault", &PreventDefault)
            .Build());
    }

    return v8::Local<v8::ObjectTemplate>::New(
        isolate, event_template)->NewInstance();
}

}  // namespace

namespace internal {

v8::Local<v8::Object> CreateJSEvent(v8::Isolate* isolate, v8::Local<v8::Object> object) {
    v8::Local<v8::Object> event;
    event = CreateEventObject(isolate);
    
    gin::Dictionary(isolate, event).Set("sender", object);
    return event;
}

v8::Local<v8::Object> CreateCustomEvent(
    v8::Isolate* isolate,
    v8::Local<v8::Object> object,
    v8::Local<v8::Object> custom_event) {
    v8::Local<v8::Object> event = CreateEventObject(isolate);
    (void)event->SetPrototype(custom_event->CreationContext(), custom_event);
    gin::Dictionary(isolate, event).Set("sender", object);
    return event;
}

v8::Local<v8::Object> CreateEventFromFlags(v8::Isolate* isolate, int flags) {
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
