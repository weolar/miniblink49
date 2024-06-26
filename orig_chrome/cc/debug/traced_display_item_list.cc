// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
#include "cc/debug/traced_display_item_list.h"

#include "base/json/json_writer.h"
#include "cc/debug/traced_value.h"
#include "cc/playback/display_item_list.h"

namespace cc {

TracedDisplayItemList::TracedDisplayItemList(
    scoped_refptr<const DisplayItemList> list,
    bool include_items)
    : display_item_list_(list)
    , include_items_(include_items)
{
}

TracedDisplayItemList::~TracedDisplayItemList()
{
}

void TracedDisplayItemList::AppendAsTraceFormat(std::string* out) const
{
    scoped_refptr<base::trace_event::ConvertableToTraceFormat> convertable = display_item_list_->AsValue(include_items_);
    convertable->AppendAsTraceFormat(out);
}

} // namespace cc
