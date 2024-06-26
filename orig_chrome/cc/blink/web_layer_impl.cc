// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/blink/web_layer_impl.h"

#include <utility>
#include <vector>

#include "base/bind.h"
#include "base/lazy_instance.h"
#include "base/strings/string_util.h"
#include "base/threading/thread_checker.h"
#include "base/trace_event/trace_event_impl.h"
#include "cc/animation/animation.h"
#include "cc/base/region.h"
#include "cc/base/switches.h"
#include "cc/blink/web_animation_impl.h"
#include "cc/blink/web_blend_mode.h"
#include "cc/blink/web_filter_operations_impl.h"
#include "cc/blink/web_to_cc_animation_delegate_adapter.h"
#include "cc/layers/layer.h"
#include "cc/layers/layer_position_constraint.h"
#include "cc/trees/layer_tree_host.h"
#include "third_party/WebKit/public/platform/WebFloatPoint.h"
#include "third_party/WebKit/public/platform/WebFloatRect.h"
#include "third_party/WebKit/public/platform/WebGraphicsLayerDebugInfo.h"
#include "third_party/WebKit/public/platform/WebLayerClient.h"
#include "third_party/WebKit/public/platform/WebLayerPositionConstraint.h"
#include "third_party/WebKit/public/platform/WebLayerScrollClient.h"
#include "third_party/WebKit/public/platform/WebSize.h"
#include "third_party/skia/include/utils/SkMatrix44.h"
#include "ui/gfx/geometry/rect_conversions.h"
#include "ui/gfx/geometry/vector2d_conversions.h"

namespace wke {
extern bool g_headlessEnable;
}

using blink::WebColor;
using blink::WebFilterOperations;
using blink::WebFloatPoint;
using blink::WebLayer;
using blink::WebRect;
using blink::WebSize;
using blink::WebVector;
using cc::Animation;
using cc::Layer;

namespace cc_blink {
namespace {

    base::LazyInstance<cc::LayerSettings> g_layer_settings = LAZY_INSTANCE_INITIALIZER;

} // namespace

WebLayerImpl::WebLayerImpl()
    : layer_(Layer::Create(LayerSettings()))
    , contents_opaque_is_fixed_(false)
{
    web_layer_client_ = nullptr;
    layer_->SetLayerClient(this);
}

WebLayerImpl::WebLayerImpl(scoped_refptr<Layer> layer)
    : layer_(layer)
    , contents_opaque_is_fixed_(false)
{
    web_layer_client_ = nullptr;
    layer_->SetLayerClient(this);
}

WebLayerImpl::~WebLayerImpl()
{
    if (animation_delegate_adapter_.get())
        layer_->set_layer_animation_delegate(nullptr);
    web_layer_client_ = nullptr;
}

// static
void WebLayerImpl::SetLayerSettings(const cc::LayerSettings& settings)
{
    g_layer_settings.Get() = settings;
}

// static
const cc::LayerSettings& WebLayerImpl::LayerSettings()
{
    return g_layer_settings.Get();
}

int WebLayerImpl::id() const
{
    return layer_->id();
}

void WebLayerImpl::invalidateRect(const blink::WebRect& rect)
{
    if (wke::g_headlessEnable)
        return;

    layer_->SetNeedsDisplayRect(rect);
}

void WebLayerImpl::invalidate()
{
    if (wke::g_headlessEnable)
        return;
    layer_->SetNeedsDisplay();
}

void WebLayerImpl::addChild(WebLayer* child)
{
    layer_->AddChild(static_cast<WebLayerImpl*>(child)->layer());
}

void WebLayerImpl::insertChild(WebLayer* child, size_t index)
{
    layer_->InsertChild(static_cast<WebLayerImpl*>(child)->layer(), index);
}

void WebLayerImpl::replaceChild(WebLayer* reference, WebLayer* new_layer)
{
    layer_->ReplaceChild(static_cast<WebLayerImpl*>(reference)->layer(),
        static_cast<WebLayerImpl*>(new_layer)->layer());
}

void WebLayerImpl::removeFromParent()
{
    layer_->RemoveFromParent();
}

void WebLayerImpl::removeAllChildren()
{
    layer_->RemoveAllChildren();
}

void WebLayerImpl::setBounds(const WebSize& size)
{
    layer_->SetBounds(size);
}

WebSize WebLayerImpl::bounds() const
{
    return layer_->bounds();
}

void WebLayerImpl::setMasksToBounds(bool masks_to_bounds)
{
    layer_->SetMasksToBounds(masks_to_bounds);
}

bool WebLayerImpl::masksToBounds() const
{
    return layer_->masks_to_bounds();
}

void WebLayerImpl::setMaskLayer(WebLayer* maskLayer)
{
    layer_->SetMaskLayer(maskLayer ? static_cast<WebLayerImpl*>(maskLayer)->layer() : 0);
}

void WebLayerImpl::setReplicaLayer(WebLayer* replica_layer)
{
    layer_->SetReplicaLayer(
        replica_layer ? static_cast<WebLayerImpl*>(replica_layer)->layer() : 0);
}

void WebLayerImpl::setOpacity(float opacity)
{
    layer_->SetOpacity(opacity);
}

float WebLayerImpl::opacity() const
{
    return layer_->opacity();
}

void WebLayerImpl::setBlendMode(blink::WebBlendMode blend_mode)
{
    layer_->SetBlendMode(BlendModeToSkia(blend_mode));
}

blink::WebBlendMode WebLayerImpl::blendMode() const
{
    return BlendModeFromSkia(layer_->blend_mode());
}

void WebLayerImpl::setIsRootForIsolatedGroup(bool isolate)
{
    layer_->SetIsRootForIsolatedGroup(isolate);
}

bool WebLayerImpl::isRootForIsolatedGroup()
{
    return layer_->is_root_for_isolated_group();
}

void WebLayerImpl::setOpaque(bool opaque)
{
    if (contents_opaque_is_fixed_)
        return;
    layer_->SetContentsOpaque(opaque);
}

bool WebLayerImpl::opaque() const
{
    return layer_->contents_opaque();
}

void WebLayerImpl::setPosition(const WebFloatPoint& position)
{
    layer_->SetPosition(position);
}

WebFloatPoint WebLayerImpl::position() const
{
    return layer_->position();
}

void WebLayerImpl::setTransform(const SkMatrix44& matrix)
{
    gfx::Transform transform;
    transform.matrix() = matrix;
    layer_->SetTransform(transform);
}

void WebLayerImpl::setTransformOrigin(const blink::WebFloatPoint3D& point)
{
    gfx::Point3F gfx_point = point;
    layer_->SetTransformOrigin(gfx_point);
}

blink::WebFloatPoint3D WebLayerImpl::transformOrigin() const
{
    return layer_->transform_origin();
}

SkMatrix44 WebLayerImpl::transform() const
{
    return layer_->transform().matrix();
}

void WebLayerImpl::setDrawsContent(bool draws_content)
{
    layer_->SetIsDrawable(draws_content);
}

bool WebLayerImpl::drawsContent() const
{
    return layer_->DrawsContent();
}

void WebLayerImpl::setShouldFlattenTransform(bool flatten)
{
    layer_->SetShouldFlattenTransform(flatten);
}

void WebLayerImpl::setRenderingContext(int context)
{
    layer_->Set3dSortingContextId(context);
}

void WebLayerImpl::setUseParentBackfaceVisibility(
    bool use_parent_backface_visibility)
{
    layer_->set_use_parent_backface_visibility(use_parent_backface_visibility);
}

void WebLayerImpl::setBackgroundColor(WebColor color)
{
    layer_->SetBackgroundColor(color);
}

WebColor WebLayerImpl::backgroundColor() const
{
    return layer_->background_color();
}

void WebLayerImpl::setFilters(const WebFilterOperations& filters)
{
    const WebFilterOperationsImpl& filters_impl = static_cast<const WebFilterOperationsImpl&>(filters);
    layer_->SetFilters(filters_impl.AsFilterOperations());
}

void WebLayerImpl::setBackgroundFilters(const WebFilterOperations& filters)
{
    const WebFilterOperationsImpl& filters_impl = static_cast<const WebFilterOperationsImpl&>(filters);
    layer_->SetBackgroundFilters(filters_impl.AsFilterOperations());
}

void WebLayerImpl::setAnimationDelegate(
    blink::WebCompositorAnimationDelegate* delegate)
{
    animation_delegate_adapter_.reset(
        new WebToCCAnimationDelegateAdapter(delegate));
    layer_->set_layer_animation_delegate(animation_delegate_adapter_.get());
}

bool WebLayerImpl::addAnimation(blink::WebCompositorAnimation* animation)
{
    bool result = layer_->AddAnimation(
        static_cast<WebCompositorAnimationImpl*>(animation)->PassAnimation());
    delete animation;
    return result;
}

void WebLayerImpl::removeAnimation(int animation_id)
{
    layer_->RemoveAnimation(animation_id);
}

void WebLayerImpl::removeAnimation(
    int animation_id,
    blink::WebCompositorAnimation::TargetProperty target_property)
{
    layer_->RemoveAnimation(
        animation_id, static_cast<Animation::TargetProperty>(target_property));
}

void WebLayerImpl::pauseAnimation(int animation_id, double time_offset)
{
    layer_->PauseAnimation(animation_id, time_offset);
}

bool WebLayerImpl::hasActiveAnimation()
{
    return layer_->HasActiveAnimation();
}

void WebLayerImpl::setForceRenderSurface(bool force_render_surface)
{
    layer_->SetForceRenderSurface(force_render_surface);
}

void WebLayerImpl::setScrollPositionDouble(blink::WebDoublePoint position)
{
    layer_->SetScrollOffset(gfx::ScrollOffset(position.x, position.y));
}

void WebLayerImpl::setScrollCompensationAdjustment(
    blink::WebDoublePoint position)
{
    layer_->SetScrollCompensationAdjustment(
        gfx::Vector2dF(position.x, position.y));
}

blink::WebDoublePoint WebLayerImpl::scrollPositionDouble() const
{
    return blink::WebDoublePoint(layer_->scroll_offset().x(),
        layer_->scroll_offset().y());
}

void WebLayerImpl::setScrollClipLayer(WebLayer* clip_layer)
{
    if (!clip_layer) {
        layer_->SetScrollClipLayerId(Layer::INVALID_ID);
        return;
    }
    layer_->SetScrollClipLayerId(clip_layer->id());
}

bool WebLayerImpl::scrollable() const
{
    return layer_->scrollable();
}

void WebLayerImpl::setUserScrollable(bool horizontal, bool vertical)
{
    layer_->SetUserScrollable(horizontal, vertical);
}

bool WebLayerImpl::userScrollableHorizontal() const
{
    return layer_->user_scrollable_horizontal();
}

bool WebLayerImpl::userScrollableVertical() const
{
    return layer_->user_scrollable_vertical();
}

void WebLayerImpl::setHaveWheelEventHandlers(bool have_wheel_event_handlers)
{
    layer_->SetHaveWheelEventHandlers(have_wheel_event_handlers);
}

bool WebLayerImpl::haveWheelEventHandlers() const
{
    return layer_->have_wheel_event_handlers();
}

void WebLayerImpl::setHaveScrollEventHandlers(bool have_scroll_event_handlers)
{
    layer_->SetHaveScrollEventHandlers(have_scroll_event_handlers);
}

bool WebLayerImpl::haveScrollEventHandlers() const
{
    return layer_->have_scroll_event_handlers();
}

void WebLayerImpl::setShouldScrollOnMainThread(
    bool should_scroll_on_main_thread)
{
    layer_->SetShouldScrollOnMainThread(should_scroll_on_main_thread);
}

bool WebLayerImpl::shouldScrollOnMainThread() const
{
    return layer_->should_scroll_on_main_thread();
}

void WebLayerImpl::setNonFastScrollableRegion(const WebVector<WebRect>& rects)
{
    cc::Region region;
    for (size_t i = 0; i < rects.size(); ++i)
        region.Union(rects[i]);
    layer_->SetNonFastScrollableRegion(region);
}

WebVector<WebRect> WebLayerImpl::nonFastScrollableRegion() const
{
    size_t num_rects = 0;
    for (cc::Region::Iterator region_rects(layer_->non_fast_scrollable_region());
         region_rects.has_rect();
         region_rects.next())
        ++num_rects;

    WebVector<WebRect> result(num_rects);
    size_t i = 0;
    for (cc::Region::Iterator region_rects(layer_->non_fast_scrollable_region());
         region_rects.has_rect();
         region_rects.next()) {
        result[i] = region_rects.rect();
        ++i;
    }
    return result;
}

void WebLayerImpl::setFrameTimingRequests(
    const WebVector<std::pair<int64_t, WebRect>>& requests)
{
    std::vector<cc::FrameTimingRequest> frame_timing_requests(requests.size());
    for (size_t i = 0; i < requests.size(); ++i) {
        frame_timing_requests[i] = cc::FrameTimingRequest(
            requests[i].first, gfx::Rect(requests[i].second));
    }
    layer_->SetFrameTimingRequests(frame_timing_requests);
}

WebVector<std::pair<int64_t, WebRect>> WebLayerImpl::frameTimingRequests()
    const
{
    const std::vector<cc::FrameTimingRequest>& frame_timing_requests = layer_->FrameTimingRequests();

    size_t num_requests = frame_timing_requests.size();

    WebVector<std::pair<int64_t, WebRect>> result(num_requests);
    for (size_t i = 0; i < num_requests; ++i) {
        result[i] = std::make_pair(frame_timing_requests[i].id(),
            frame_timing_requests[i].rect());
    }
    return result;
}

void WebLayerImpl::setTouchEventHandlerRegion(const WebVector<WebRect>& rects)
{
    cc::Region region;
    for (size_t i = 0; i < rects.size(); ++i)
        region.Union(rects[i]);
    layer_->SetTouchEventHandlerRegion(region);
}

WebVector<WebRect> WebLayerImpl::touchEventHandlerRegion() const
{
    size_t num_rects = 0;
    for (cc::Region::Iterator region_rects(layer_->touch_event_handler_region());
         region_rects.has_rect();
         region_rects.next())
        ++num_rects;

    WebVector<WebRect> result(num_rects);
    size_t i = 0;
    for (cc::Region::Iterator region_rects(layer_->touch_event_handler_region());
         region_rects.has_rect();
         region_rects.next()) {
        result[i] = region_rects.rect();
        ++i;
    }
    return result;
}

static_assert(static_cast<ScrollBlocksOn>(blink::WebScrollBlocksOnNone) == SCROLL_BLOCKS_ON_NONE,
    "ScrollBlocksOn and WebScrollBlocksOn enums must match");
static_assert(static_cast<ScrollBlocksOn>(blink::WebScrollBlocksOnStartTouch) == SCROLL_BLOCKS_ON_START_TOUCH,
    "ScrollBlocksOn and WebScrollBlocksOn enums must match");
static_assert(static_cast<ScrollBlocksOn>(blink::WebScrollBlocksOnWheelEvent) == SCROLL_BLOCKS_ON_WHEEL_EVENT,
    "ScrollBlocksOn and WebScrollBlocksOn enums must match");
static_assert(
    static_cast<ScrollBlocksOn>(blink::WebScrollBlocksOnScrollEvent) == SCROLL_BLOCKS_ON_SCROLL_EVENT,
    "ScrollBlocksOn and WebScrollBlocksOn enums must match");

void WebLayerImpl::setScrollBlocksOn(blink::WebScrollBlocksOn blocks)
{
    layer_->SetScrollBlocksOn(static_cast<ScrollBlocksOn>(blocks));
}

blink::WebScrollBlocksOn WebLayerImpl::scrollBlocksOn() const
{
    return static_cast<blink::WebScrollBlocksOn>(layer_->scroll_blocks_on());
}

void WebLayerImpl::setIsContainerForFixedPositionLayers(bool enable)
{
    layer_->SetIsContainerForFixedPositionLayers(enable);
}

bool WebLayerImpl::isContainerForFixedPositionLayers() const
{
    return layer_->IsContainerForFixedPositionLayers();
}

static blink::WebLayerPositionConstraint ToWebLayerPositionConstraint(
    const cc::LayerPositionConstraint& constraint)
{
    blink::WebLayerPositionConstraint web_constraint;
    web_constraint.isFixedPosition = constraint.is_fixed_position();
    web_constraint.isFixedToRightEdge = constraint.is_fixed_to_right_edge();
    web_constraint.isFixedToBottomEdge = constraint.is_fixed_to_bottom_edge();
    return web_constraint;
}

static cc::LayerPositionConstraint ToLayerPositionConstraint(
    const blink::WebLayerPositionConstraint& web_constraint)
{
    cc::LayerPositionConstraint constraint;
    constraint.set_is_fixed_position(web_constraint.isFixedPosition);
    constraint.set_is_fixed_to_right_edge(web_constraint.isFixedToRightEdge);
    constraint.set_is_fixed_to_bottom_edge(web_constraint.isFixedToBottomEdge);
    return constraint;
}

void WebLayerImpl::setPositionConstraint(
    const blink::WebLayerPositionConstraint& constraint)
{
    layer_->SetPositionConstraint(ToLayerPositionConstraint(constraint));
}

blink::WebLayerPositionConstraint WebLayerImpl::positionConstraint() const
{
    return ToWebLayerPositionConstraint(layer_->position_constraint());
}

void WebLayerImpl::setScrollClient(blink::WebLayerScrollClient* scroll_client)
{
    if (scroll_client) {
        layer_->set_did_scroll_callback(
            base::Bind(&blink::WebLayerScrollClient::didScroll,
                base::Unretained(scroll_client)));
    } else {
        layer_->set_did_scroll_callback(base::Closure());
    }
}

bool WebLayerImpl::isOrphan() const
{
    return !layer_->layer_tree_host();
}

void WebLayerImpl::setWebLayerClient(blink::WebLayerClient* client)
{
    web_layer_client_ = client;
}

class TracedDebugInfo : public base::trace_event::ConvertableToTraceFormat {
public:
    // This object takes ownership of the debug_info object.
    explicit TracedDebugInfo(blink::WebGraphicsLayerDebugInfo* debug_info)
        : debug_info_(debug_info)
    {
    }
    void AppendAsTraceFormat(std::string* out) const override
    {
        DCHECK(thread_checker_.CalledOnValidThread());
        blink::WebString web_string;
        debug_info_->appendAsTraceFormat(&web_string);
        out->append(web_string.utf8());
    }

private:
    ~TracedDebugInfo() override { }
    scoped_ptr<blink::WebGraphicsLayerDebugInfo> debug_info_;
    base::ThreadChecker thread_checker_;
};

scoped_refptr<base::trace_event::ConvertableToTraceFormat>
WebLayerImpl::TakeDebugInfo()
{
    if (!web_layer_client_)
        return nullptr;
    blink::WebGraphicsLayerDebugInfo* debug_info = web_layer_client_->takeDebugInfoFor(this);

    if (debug_info)
        return new TracedDebugInfo(debug_info);
    else
        return nullptr;
}

void WebLayerImpl::setScrollParent(blink::WebLayer* parent)
{
    cc::Layer* scroll_parent = nullptr;
    if (parent)
        scroll_parent = static_cast<WebLayerImpl*>(parent)->layer();
    layer_->SetScrollParent(scroll_parent);
}

void WebLayerImpl::setClipParent(blink::WebLayer* parent)
{
    cc::Layer* clip_parent = nullptr;
    if (parent)
        clip_parent = static_cast<WebLayerImpl*>(parent)->layer();
    layer_->SetClipParent(clip_parent);
}

Layer* WebLayerImpl::layer() const
{
    return layer_.get();
}

void WebLayerImpl::SetContentsOpaqueIsFixed(bool fixed)
{
    contents_opaque_is_fixed_ = fixed;
}

} // namespace cc_blink
