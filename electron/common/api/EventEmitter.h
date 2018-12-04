// Copyright (c) 2014 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ATOM_BROWSER_API_EVENT_EMITTER_H_
#define ATOM_BROWSER_API_EVENT_EMITTER_H_

#include "common/api/EventEmitterCaller.h"
#include "gin/wrappable.h"

#include <vector>
#include <functional>

namespace mate {

namespace internal {

v8::Local<v8::Object> createJSEvent(v8::Isolate* isolate, v8::Local<v8::Object> object);
v8::Local<v8::Object> createJSEventWithSender(v8::Isolate* isolate, v8::Local<v8::Object> object, std::function<void(std::string)>&& callback);
v8::Local<v8::Object> createCustomEvent(v8::Isolate* isolate, v8::Local<v8::Object> object, v8::Local<v8::Object> event);
v8::Local<v8::Object> createEventFromFlags(v8::Isolate* isolate, int flags);

}  // namespace internal

   // Provide helperers to emit event in JavaScript.
template<typename T>
class EventEmitter : public gin::Wrappable<T> {
public:
    typedef std::vector<v8::Local<v8::Value>> ValueArray;

    // Make the convinient methods visible:
    // https://isocpp.org/wiki/faq/templates#nondependent-name-lookup-members
    v8::Local<v8::Object> getWrapper() { return Wrappable<T>::GetWrapper(isolate()); }
    v8::Isolate* isolate() const { return Wrappable<T>::isolate(); }

    // this.emit(name, event, args...);
    template<typename... Args>
    bool emitCustomEvent(const base::StringPiece& name,
        v8::Local<v8::Object> event,
        const Args&... args) {
        return emitWithEvent(name, internal::createCustomEvent(isolate(), GetWrapper(), event), args...);
    }

    // this.emit(name, new Event(flags), args...);
    template<typename... Args>
    bool emitWithFlags(const base::StringPiece& name,
        int flags,
        const Args&... args) {
        return emitCustomEvent(name, internal::createEventFromFlags(isolate(), flags), args...);
    }

    // this.emit(name, new Event(), args...);
    template<typename... Args>
    bool emit(const std::string& name, const Args&... args) { // base::StringPiece
        return emitWithoutSender(name, args...);
    }

    // this.emit(name, new Event(sender, message), args...);
    template<typename... Args>
    bool emitWithoutSender(const std::string& name, const Args&... args) { // base::StringPiece
        //v8::Locker locker(isolate());
        v8::HandleScope handle_scope(isolate());
        v8::Local<v8::Object> event = internal::createJSEvent(isolate(), getWrapper());
        return emitWithEvent(name, event, args...);
    }

    template<typename... Args>
    bool emitWithSender(const std::string& name, std::function<void(std::string)>&& callback, const Args&... args) { // base::StringPiece
        //v8::Locker locker(isolate());
        v8::HandleScope handle_scope(isolate());
        v8::Local<v8::Object> event = internal::createJSEventWithSender(isolate(), getWrapper(), std::move(callback));
        return emitWithEvent(name, event, args...);
    }

protected:
    EventEmitter() {}

private:
    // this.emit(name, event, args...);
    template<typename... Args>
    bool emitWithEvent(const std::string& name, // base::StringPiece
        v8::Local<v8::Object> event,
        const Args&... args) {
        //v8::Locker locker(isolate());
        v8::HandleScope handle_scope(isolate());
        emitEvent(isolate(), getWrapper(), name, event, args...);
        return event->Get(gin::StringToV8(isolate(), "defaultPrevented"))->BooleanValue();
    }

    DISALLOW_COPY_AND_ASSIGN(EventEmitter);
};

}  // namespace mate

#endif  // ATOM_BROWSER_API_EVENT_EMITTER_H_
