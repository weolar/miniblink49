// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_PLAYBACK_DISPLAY_ITEM_PROTO_FACTORY_H_
#define CC_PLAYBACK_DISPLAY_ITEM_PROTO_FACTORY_H_

#include "base/macros.h"
#include "base/memory/ref_counted.h"
#include "cc/playback/display_item.h"
#include "cc/playback/display_item_list.h"

namespace cc {

namespace proto {
    class DisplayItem;
}

class DisplayItemProtoFactory {
public:
    static DisplayItem* AllocateAndConstruct(scoped_refptr<DisplayItemList> list,
        const proto::DisplayItem& proto);

private:
    DisplayItemProtoFactory() { }
    virtual ~DisplayItemProtoFactory() { }

    DISALLOW_COPY_AND_ASSIGN(DisplayItemProtoFactory);
};

} // namespace cc

#endif // CC_PLAYBACK_DISPLAY_ITEM_PROTO_FACTORY_H_
