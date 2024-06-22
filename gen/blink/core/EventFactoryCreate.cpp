// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"
#include "core/events/EventFactory.h"

#include "EventHeaders.h"
#include "platform/RuntimeEnabledFeatures.h"

namespace blink {

PassRefPtrWillBeRawPtr<Event> EventFactory::create(const String& type)
{
    if (type == "FontFaceSetLoadEvent")
        return FontFaceSetLoadEvent::create();
    if (type == "MediaQueryListEvent")
        return MediaQueryListEvent::create();
    if (type == "AnimationEvent")
        return AnimationEvent::create();
    if (type == "AnimationPlayerEvent" && RuntimeEnabledFeatures::webAnimationsAPIEnabled())
        return AnimationPlayerEvent::create();
    if (type == "ApplicationCacheErrorEvent")
        return ApplicationCacheErrorEvent::create();
    if (type == "AutocompleteErrorEvent")
        return AutocompleteErrorEvent::create();
    if (type == "BeforeUnloadEvent")
        return BeforeUnloadEvent::create();
    if (type == "ClipboardEvent")
        return ClipboardEvent::create();
    if (type == "CompositionEvent")
        return CompositionEvent::create();
    if (equalIgnoringCase(type, "CustomEvent"))
        return CustomEvent::create();
    if (type == "ErrorEvent")
        return ErrorEvent::create();
    if (equalIgnoringCase(type, "Event"))
        return Event::create();
    if (type == "FocusEvent")
        return FocusEvent::create();
    if (type == "HashChangeEvent")
        return HashChangeEvent::create();
    if (type == "KeyboardEvent")
        return KeyboardEvent::create();
    if (type == "MessageEvent")
        return MessageEvent::create();
    if (equalIgnoringCase(type, "MouseEvent"))
        return MouseEvent::create();
    if (type == "MutationEvent")
        return MutationEvent::create();
    if (type == "PageTransitionEvent")
        return PageTransitionEvent::create();
    if (type == "PointerEvent" && RuntimeEnabledFeatures::pointerEventEnabled())
        return PointerEvent::create();
    if (type == "PopStateEvent")
        return PopStateEvent::create();
    if (type == "ProgressEvent")
        return ProgressEvent::create();
    if (type == "PromiseRejectionEvent" && RuntimeEnabledFeatures::promiseRejectionEventEnabled())
        return PromiseRejectionEvent::create();
    if (type == "RelatedEvent" && RuntimeEnabledFeatures::contextMenuEnabled())
        return RelatedEvent::create();
    if (type == "ResourceProgressEvent")
        return ResourceProgressEvent::create();
    if (type == "SecurityPolicyViolationEvent")
        return SecurityPolicyViolationEvent::create();
    if (type == "TextEvent")
        return TextEvent::create();
    if (type == "TouchEvent" && RuntimeEnabledFeatures::touchEnabled())
        return TouchEvent::create();
    if (type == "TransitionEvent")
        return TransitionEvent::create();
    if (equalIgnoringCase(type, "UIEvent"))
        return UIEvent::create();
    if (type == "WheelEvent")
        return WheelEvent::create();
    if (type == "MediaKeyEvent" && RuntimeEnabledFeatures::prefixedEncryptedMediaEnabled())
        return MediaKeyEvent::create();
    if (type == "TrackEvent")
        return TrackEvent::create();
    if (type == "SVGZoomEvent")
        return SVGZoomEvent::create();
    if (type == "XMLHttpRequestProgressEvent")
        return XMLHttpRequestProgressEvent::create();
    if (type == "WebKitAnimationEvent")
        return AnimationEvent::create();
    if (equalIgnoringCase(type, "Events"))
        return Event::create();
    if (equalIgnoringCase(type, "HTMLEvents"))
        return Event::create();
    if (type == "KeyboardEvents")
        return KeyboardEvent::create();
    if (equalIgnoringCase(type, "MouseEvents"))
        return MouseEvent::create();
    if (type == "MutationEvents")
        return MutationEvent::create();
    if (type == "OrientationEvent" && RuntimeEnabledFeatures::orientationEventEnabled())
        return Event::create();
    if (type == "SVGEvents")
        return Event::create();
    if (type == "SVGZoomEvents")
        return SVGZoomEvent::create();
    if (equalIgnoringCase(type, "UIEvents"))
        return UIEvent::create();
    if (type == "WebKitTransitionEvent")
        return TransitionEvent::create();
    return nullptr;
}

} // namespace blink
