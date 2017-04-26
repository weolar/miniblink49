// Copyright (c) 2016 GitHub, Inc.
// Use of this source code is governed by the MIT license that can be
// found in the LICENSE file.

#ifndef ATOM_COMMON_API_REMOTE_OBJECT_FREER_H_
#define ATOM_COMMON_API_REMOTE_OBJECT_FREER_H_

#include "common/api/ObjectLifeMonitor.h"
#include "cef/include/base/cef_macros.h"

namespace atom {

class RemoteObjectFreer : public ObjectLifeMonitor {
public:
    static void bindTo(v8::Isolate* isolate, v8::Local<v8::Object> target, int objectId);

protected:
    RemoteObjectFreer(v8::Isolate* isolate, v8::Local<v8::Object> target, int objectId);
    virtual ~RemoteObjectFreer() override;

    virtual void onRunDestructor() override;

private:
    int m_objectId;

    DISALLOW_COPY_AND_ASSIGN(RemoteObjectFreer);
};

}  // namespace atom

#endif  // ATOM_COMMON_API_REMOTE_OBJECT_FREER_H_
