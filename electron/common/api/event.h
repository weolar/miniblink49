// Copyright (c) 2014 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ATOM_BROWSER_API_EVENT_H_
#define ATOM_BROWSER_API_EVENT_H_

#include "gin/wrappable.h"
#include "gin/handle.h"

namespace mate {

class Event : public gin::Wrappable<Event> {
public:
    static gin::WrapperInfo kWrapperInfo;
    static gin::Handle<Event> Create(v8::Isolate* isolate);

    static void BuildPrototype(v8::Isolate* isolate,
        v8::Local<v8::FunctionTemplate> prototype);

    // event.PreventDefault().
    void PreventDefault(v8::Isolate* isolate);

    // event.sendReply(json), used for replying synchronous message.
    bool SendReply(const std::string& json);

protected:
    explicit Event(v8::Isolate* isolate);
    ~Event() override;

private:
    DISALLOW_COPY_AND_ASSIGN(Event);
};

}  // namespace mate

#endif  // ATOM_BROWSER_API_EVENT_H_
