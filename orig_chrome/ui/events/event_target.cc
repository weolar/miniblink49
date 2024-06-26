// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/events/event_target.h"

#include <algorithm>

#include "base/logging.h"
#include "ui/events/event.h"

namespace ui {

EventTarget::EventTarget()
    : target_handler_(NULL)
{
}

EventTarget::~EventTarget()
{
}

void EventTarget::ConvertEventToTarget(EventTarget* target,
    LocatedEvent* event)
{
}

void EventTarget::AddPreTargetHandler(EventHandler* handler)
{
    pre_target_list_.push_back(handler);
}

void EventTarget::PrependPreTargetHandler(EventHandler* handler)
{
    pre_target_list_.insert(pre_target_list_.begin(), handler);
}

void EventTarget::RemovePreTargetHandler(EventHandler* handler)
{
    EventHandlerList::iterator find = std::find(pre_target_list_.begin(),
        pre_target_list_.end(),
        handler);
    if (find != pre_target_list_.end())
        pre_target_list_.erase(find);
}

void EventTarget::AddPostTargetHandler(EventHandler* handler)
{
    post_target_list_.push_back(handler);
}

void EventTarget::RemovePostTargetHandler(EventHandler* handler)
{
    EventHandlerList::iterator find = std::find(post_target_list_.begin(),
        post_target_list_.end(),
        handler);
    if (find != post_target_list_.end())
        post_target_list_.erase(find);
}

bool EventTarget::IsPreTargetListEmpty() const
{
    return pre_target_list_.empty();
}

EventHandler* EventTarget::SetTargetHandler(EventHandler* target_handler)
{
    EventHandler* original_target_handler = target_handler_;
    target_handler_ = target_handler;
    return original_target_handler;
}

void EventTarget::GetPreTargetHandlers(EventHandlerList* list)
{
    EventTarget* target = this;
    while (target) {
        EventHandlerList::reverse_iterator it, rend;
        for (it = target->pre_target_list_.rbegin(),
            rend = target->pre_target_list_.rend();
             it != rend;
             ++it) {
            list->insert(list->begin(), *it);
        }
        target = target->GetParentTarget();
    }
}

void EventTarget::GetPostTargetHandlers(EventHandlerList* list)
{
    EventTarget* target = this;
    while (target) {
        for (EventHandlerList::iterator it = target->post_target_list_.begin(),
                                        end = target->post_target_list_.end();
             it != end; ++it) {
            list->push_back(*it);
        }
        target = target->GetParentTarget();
    }
}

} // namespace ui
