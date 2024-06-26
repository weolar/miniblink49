// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CC_DEBUG_TRACED_DISPLAY_ITEM_LIST_H_
#define CC_DEBUG_TRACED_DISPLAY_ITEM_LIST_H_

#include <string>

#include "base/memory/ref_counted.h"
#include "base/trace_event/trace_event.h"
#include "cc/debug/traced_value.h"

namespace cc {

class DisplayItemList;

class TracedDisplayItemList
    : public base::trace_event::ConvertableToTraceFormat {
public:
    static scoped_refptr<ConvertableToTraceFormat> AsTraceableDisplayItemList(
        scoped_refptr<const DisplayItemList> list,
        bool include_items)
    {
        return scoped_refptr<ConvertableToTraceFormat>(
            new TracedDisplayItemList(list, include_items));
    }
    void AppendAsTraceFormat(std::string* out) const override;

private:
    explicit TracedDisplayItemList(scoped_refptr<const DisplayItemList> list,
        bool include_items);
    ~TracedDisplayItemList() override;

    scoped_refptr<const DisplayItemList> display_item_list_;
    bool include_items_;

    DISALLOW_COPY_AND_ASSIGN(TracedDisplayItemList);
};

} // namespace cc

#endif // CC_DEBUG_TRACED_DISPLAY_ITEM_LIST_H_
