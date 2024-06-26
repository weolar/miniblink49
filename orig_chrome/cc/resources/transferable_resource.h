// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_RESOURCES_TRANSFERABLE_RESOURCE_H_
#define CC_RESOURCES_TRANSFERABLE_RESOURCE_H_

#include <vector>

#include "base/basictypes.h"
#include "cc/base/cc_export.h"
#include "cc/base/resource_id.h"
#include "cc/resources/resource_format.h"
#include "gpu/command_buffer/common/mailbox_holder.h"
#include "ui/gfx/geometry/size.h"

namespace cc {

struct ReturnedResource;
typedef std::vector<ReturnedResource> ReturnedResourceArray;
struct TransferableResource;
typedef std::vector<TransferableResource> TransferableResourceArray;

struct CC_EXPORT TransferableResource {
    TransferableResource();
    ~TransferableResource();

    ReturnedResource ToReturnedResource() const;
    static void ReturnResources(const TransferableResourceArray& input,
        ReturnedResourceArray* output);

    ResourceId id;
    ResourceFormat format;
    uint32 filter;
    gfx::Size size;
    gpu::MailboxHolder mailbox_holder;
    bool read_lock_fences_enabled;
    bool is_repeated;
    bool is_software;
    bool allow_overlay;
};

} // namespace cc

#endif // CC_RESOURCES_TRANSFERABLE_RESOURCE_H_
