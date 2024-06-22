// Copyright (c) 2013 GitHub, Inc.
// Copyright (c) 2012 Intel Corp. All rights reserved.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#include "common/api/ObjectLifeMonitor.h"

namespace atom {

ObjectLifeMonitor::ObjectLifeMonitor(v8::Isolate* isolate, v8::Local<v8::Object> target)
    : isolate_(isolate)
    , context_(isolate, isolate->GetCurrentContext())
    , target_(isolate, target)
/*weak_ptr_factory_(this)*/ {
    target_.SetWeak(this, onObjectGC, v8::WeakCallbackType::kParameter);
}

ObjectLifeMonitor::~ObjectLifeMonitor()
{
    if (target_.IsEmpty())
        return;
    target_.ClearWeak();
    target_.Reset();
}

// static
void ObjectLifeMonitor::onObjectGC(const v8::WeakCallbackInfo<ObjectLifeMonitor>& data)
{
    ObjectLifeMonitor* self = data.GetParameter();
    self->target_.Reset();
    self->onRunDestructor();
    data.SetSecondPassCallback(freeMe);
}

// static
void ObjectLifeMonitor::freeMe(const v8::WeakCallbackInfo<ObjectLifeMonitor>& data)
{
    delete data.GetParameter();
}

} // namespace atom
