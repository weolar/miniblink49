// Copyright 2014 the V8 project authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef V8_PROTOTYPE_H_
#define V8_PROTOTYPE_H_

#include "src/isolate.h"
#include "src/objects.h"

namespace v8 {
namespace internal {

/**
 * A class to uniformly access the prototype of any Object and walk its
 * prototype chain.
 *
 * The PrototypeIterator can either start at the prototype (default), or
 * include the receiver itself. If a PrototypeIterator is constructed for a
 * Map, it will always start at the prototype.
 *
 * The PrototypeIterator can either run to the null_value(), the first
 * non-hidden prototype, or a given object.
 */

class PrototypeIterator {
 public:
  enum WhereToStart { START_AT_RECEIVER, START_AT_PROTOTYPE };

  enum WhereToEnd { END_AT_NULL, END_AT_NON_HIDDEN };

  const int kProxyPrototypeLimit = 100 * 1000;

  PrototypeIterator(Isolate* isolate, Handle<JSReceiver> receiver,
                    WhereToStart where_to_start = START_AT_PROTOTYPE,
                    WhereToEnd where_to_end = END_AT_NULL)
      : object_(NULL),
        handle_(receiver),
        isolate_(isolate),
        where_to_end_(where_to_end),
        is_at_end_(false),
        seen_proxies_(0) {
    CHECK(!handle_.is_null());
    if (where_to_start == START_AT_PROTOTYPE) Advance();
  }

  PrototypeIterator(Isolate* isolate, JSReceiver* receiver,
                    WhereToStart where_to_start = START_AT_PROTOTYPE,
                    WhereToEnd where_to_end = END_AT_NULL)
      : object_(receiver),
        isolate_(isolate),
        where_to_end_(where_to_end),
        is_at_end_(false),
        seen_proxies_(0) {
    if (where_to_start == START_AT_PROTOTYPE) Advance();
  }

  explicit PrototypeIterator(Map* receiver_map)
      : object_(receiver_map->prototype()),
        isolate_(receiver_map->GetIsolate()),
        where_to_end_(END_AT_NULL),
        is_at_end_(object_->IsNull()) {}

  explicit PrototypeIterator(Handle<Map> receiver_map)
      : object_(NULL),
        handle_(handle(receiver_map->prototype(), receiver_map->GetIsolate())),
        isolate_(receiver_map->GetIsolate()),
        where_to_end_(END_AT_NULL),
        is_at_end_(handle_->IsNull()) {}

  ~PrototypeIterator() {}

  bool HasAccess() const {
    // We can only perform access check in the handlified version of the
    // PrototypeIterator.
    DCHECK(!handle_.is_null());
    if (handle_->IsAccessCheckNeeded()) {
      return isolate_->MayAccess(handle(isolate_->context()),
                                 Handle<JSObject>::cast(handle_));
    }
    return true;
  }

  template <typename T = Object>
  T* GetCurrent() const {
    DCHECK(handle_.is_null());
    return T::cast(object_);
  }

  template <typename T = Object>
  static Handle<T> GetCurrent(const PrototypeIterator& iterator) {
    DCHECK(!iterator.handle_.is_null());
    DCHECK(iterator.object_ == NULL);
    return Handle<T>::cast(iterator.handle_);
  }

  void Advance() {
    if (handle_.is_null() && object_->IsJSProxy()) {
      is_at_end_ = true;
      object_ = isolate_->heap()->null_value();
      return;
    } else if (!handle_.is_null() && handle_->IsJSProxy()) {
      is_at_end_ = true;
      handle_ = isolate_->factory()->null_value();
      return;
    }
    AdvanceIgnoringProxies();
  }

  void AdvanceIgnoringProxies() {
    Object* object = handle_.is_null() ? object_ : *handle_;
    Map* map = HeapObject::cast(object)->map();

    Object* prototype = map->prototype();
    is_at_end_ = where_to_end_ == END_AT_NON_HIDDEN
                     ? !map->has_hidden_prototype()
                     : prototype->IsNull();

    if (handle_.is_null()) {
      object_ = prototype;
    } else {
      handle_ = handle(prototype, isolate_);
    }
  }

  // Returns false iff a call to JSProxy::GetPrototype throws.
  // TODO(neis): This should probably replace Advance().
  MUST_USE_RESULT bool AdvanceFollowingProxies() {
    DCHECK(!(handle_.is_null() && object_->IsJSProxy()));
    if (!HasAccess()) {
      // Abort the lookup if we do not have access to the current object.
      handle_ = isolate_->factory()->null_value();
      is_at_end_ = true;
      return true;
    }
    return AdvanceFollowingProxiesIgnoringAccessChecks();
  }

  MUST_USE_RESULT bool AdvanceFollowingProxiesIgnoringAccessChecks() {
    if (handle_.is_null() || !handle_->IsJSProxy()) {
      AdvanceIgnoringProxies();
      return true;
    }

    // Due to possible __proto__ recursion limit the number of Proxies
    // we visit to an arbitrarily chosen large number.
    seen_proxies_++;
    if (seen_proxies_ > kProxyPrototypeLimit) {
      isolate_->Throw(
          *isolate_->factory()->NewRangeError(MessageTemplate::kStackOverflow));
      return false;
    }
    MaybeHandle<Object> proto =
        JSProxy::GetPrototype(Handle<JSProxy>::cast(handle_));
    if (!proto.ToHandle(&handle_)) return false;
    is_at_end_ = where_to_end_ == END_AT_NON_HIDDEN || handle_->IsNull();
    return true;
  }

  bool IsAtEnd() const { return is_at_end_; }

 private:
  Object* object_;
  Handle<Object> handle_;
  Isolate* isolate_;
  WhereToEnd where_to_end_;
  bool is_at_end_;
  int seen_proxies_;

  DISALLOW_COPY_AND_ASSIGN(PrototypeIterator);
};


}  // namespace internal

}  // namespace v8

#endif  // V8_PROTOTYPE_H_
