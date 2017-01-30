// Copyright (c) 2014 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ATOM_BROWSER_API_EVENT_EMITTER_H_
#define ATOM_BROWSER_API_EVENT_EMITTER_H_

#include <vector>

#include "common/api/event_emitter_caller.h"
#include "gin/wrappable.h"

namespace mate {

namespace internal {

v8::Local<v8::Object> CreateJSEvent(v8::Isolate* isolate, v8::Local<v8::Object> object);
v8::Local<v8::Object> CreateCustomEvent(
    v8::Isolate* isolate,
    v8::Local<v8::Object> object,
    v8::Local<v8::Object> event);
v8::Local<v8::Object> CreateEventFromFlags(v8::Isolate* isolate, int flags);

}  // namespace internal

// Provide helperers to emit event in JavaScript.
template<typename T>
class EventEmitter : public gin::Wrappable<T> {
 public:
  typedef std::vector<v8::Local<v8::Value>> ValueArray;

 
  // Make the convinient methods visible:
  // https://isocpp.org/wiki/faq/templates#nondependent-name-lookup-members
  v8::Local<v8::Object> GetWrapper() { return Wrappable<T>::GetWrapper(isolate()); }
  v8::Isolate* isolate() const { return Wrappable<T>::isolate(); }

  // this.emit(name, event, args...);
  template<typename... Args>
  bool EmitCustomEvent(const base::StringPiece& name,
                       v8::Local<v8::Object> event,
                       const Args&... args) {
    return EmitWithEvent(
        name,
        internal::CreateCustomEvent(isolate(), GetWrapper(), event), args...);
  }

  // this.emit(name, new Event(flags), args...);
  template<typename... Args>
  bool EmitWithFlags(const base::StringPiece& name,
                     int flags,
                     const Args&... args) {
    return EmitCustomEvent(
        name,
        internal::CreateEventFromFlags(isolate(), flags), args...);
  }

  // this.emit(name, new Event(), args...);
  template<typename... Args>
  bool Emit(const std::string& name, const Args&... args) { // base::StringPiece
      return EmitWithSender(name, args...);
  }

  // this.emit(name, new Event(sender, message), args...);
  template<typename... Args>
  bool EmitWithSender(const std::string& name, const Args&... args) { // base::StringPiece
    //v8::Locker locker(isolate());
    v8::HandleScope handle_scope(isolate());
    v8::Local<v8::Object> event = internal::CreateJSEvent(isolate(), GetWrapper());
    return EmitWithEvent(name, event, args...);
  }

 protected:
  EventEmitter() {}

 private:
  // this.emit(name, event, args...);
  template<typename... Args>
  bool EmitWithEvent(const std::string& name, // base::StringPiece
                     v8::Local<v8::Object> event,
                     const Args&... args) {
    //v8::Locker locker(isolate());
    v8::HandleScope handle_scope(isolate());
    EmitEvent(isolate(), GetWrapper(), name, event, args...);
    return event->Get(gin::StringToV8(isolate(), "defaultPrevented"))->BooleanValue();
  }

  DISALLOW_COPY_AND_ASSIGN(EventEmitter);

  v8::Isolate* m_isolate;
};

}  // namespace mate

#endif  // ATOM_BROWSER_API_EVENT_EMITTER_H_
