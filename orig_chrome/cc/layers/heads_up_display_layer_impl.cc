// Copyright 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/layers/heads_up_display_layer_impl.h"

#include <algorithm>
#include <vector>

#include "base/numerics/safe_conversions.h"
#include "base/strings/stringprintf.h"
#include "base/trace_event/trace_event.h"
#include "base/trace_event/trace_event_argument.h"
#include "cc/debug/debug_colors.h"
#include "cc/debug/frame_rate_counter.h"
#include "cc/output/begin_frame_args.h"
#include "cc/output/renderer.h"
#include "cc/quads/texture_draw_quad.h"
#include "cc/resources/memory_history.h"
#include "cc/trees/layer_tree_host_impl.h"
#include "cc/trees/layer_tree_impl.h"
#include "skia/ext/platform_canvas.h"
#include "third_party/skia/include/core/SkPaint.h"
#include "third_party/skia/include/core/SkPath.h"
#include "third_party/skia/include/core/SkTypeface.h"
#include "third_party/skia/include/effects/SkColorMatrixFilter.h"
#include "ui/gfx/geometry/point.h"
#include "ui/gfx/geometry/size.h"
#include "ui/gfx/geometry/size_conversions.h"
#include "ui/gfx/hud_font.h"

namespace cc {

static inline SkPaint CreatePaint()
{
    SkPaint paint;
#if (SK_R32_SHIFT || SK_B32_SHIFT != 16)
    // The SkCanvas is in RGBA but the shader is expecting BGRA, so we need to
    // swizzle our colors when drawing to the SkCanvas.
    SkColorMatrix swizzle_matrix;
    for (int i = 0; i < 20; ++i)
        swizzle_matrix.fMat[i] = 0;
    swizzle_matrix.fMat[0 + 5 * 2] = 1;
    swizzle_matrix.fMat[1 + 5 * 1] = 1;
    swizzle_matrix.fMat[2 + 5 * 0] = 1;
    swizzle_matrix.fMat[3 + 5 * 3] = 1;

    skia::RefPtr<SkColorMatrixFilter> filter = skia::AdoptRef(SkColorMatrixFilter::Create(swizzle_matrix));
    paint.setColorFilter(filter.get());
#endif
    return paint;
}

HeadsUpDisplayLayerImpl::Graph::Graph(double indicator_value,
    double start_upper_bound)
    : value(0.0)
    , min(0.0)
    , max(0.0)
    , current_upper_bound(start_upper_bound)
    , default_upper_bound(start_upper_bound)
    , indicator(indicator_value)
{
}

double HeadsUpDisplayLayerImpl::Graph::UpdateUpperBound()
{
    double target_upper_bound = std::max(max, default_upper_bound);
    current_upper_bound += (target_upper_bound - current_upper_bound) * 0.5;
    return current_upper_bound;
}

HeadsUpDisplayLayerImpl::HeadsUpDisplayLayerImpl(LayerTreeImpl* tree_impl,
    int id)
    : LayerImpl(tree_impl, id)
    ,
    //typeface_(gfx::GetHudTypeface()),
    internal_contents_scale_(1.f)
    , fps_graph_(60.0, 80.0)
    , paint_time_graph_(16.0, 48.0)
    , fade_step_(0)
{
    DebugBreak();
    if (!typeface_) {
        typeface_ = skia::AdoptRef(
            SkTypeface::CreateFromName("monospace", SkTypeface::kBold));
    }
}

HeadsUpDisplayLayerImpl::~HeadsUpDisplayLayerImpl() { }

scoped_ptr<LayerImpl> HeadsUpDisplayLayerImpl::CreateLayerImpl(
    LayerTreeImpl* tree_impl)
{
    return HeadsUpDisplayLayerImpl::Create(tree_impl, id());
}

void HeadsUpDisplayLayerImpl::AcquireResource(
    ResourceProvider* resource_provider)
{
    for (ScopedPtrVector<ScopedResource>::iterator it = resources_.begin();
         it != resources_.end();
         ++it) {
        if (!resource_provider->InUseByConsumer((*it)->id())) {
            resources_.swap(it, resources_.end() - 1);
            return;
        }
    }

    scoped_ptr<ScopedResource> resource = ScopedResource::Create(resource_provider);
    resource->Allocate(internal_content_bounds_,
        ResourceProvider::TEXTURE_HINT_IMMUTABLE, RGBA_8888);
    resources_.push_back(resource.Pass());
}

class ResourceSizeIsEqualTo {
public:
    explicit ResourceSizeIsEqualTo(const gfx::Size& size_)
        : compare_size_(size_)
    {
    }

    bool operator()(const ScopedResource* resource)
    {
        return resource->size() == compare_size_;
    }

private:
    const gfx::Size compare_size_;
};

void HeadsUpDisplayLayerImpl::ReleaseUnmatchedSizeResources(
    ResourceProvider* resource_provider)
{
    ScopedPtrVector<ScopedResource>::iterator it_erase = resources_.partition(ResourceSizeIsEqualTo(internal_content_bounds_));
    resources_.erase(it_erase, resources_.end());
}

bool HeadsUpDisplayLayerImpl::WillDraw(DrawMode draw_mode,
    ResourceProvider* resource_provider)
{
    if (draw_mode == DRAW_MODE_RESOURCELESS_SOFTWARE)
        return false;

    internal_contents_scale_ = GetIdealContentsScale();
    internal_content_bounds_ = gfx::ScaleToCeiledSize(bounds(), internal_contents_scale_);

    ReleaseUnmatchedSizeResources(resource_provider);
    AcquireResource(resource_provider);
    return LayerImpl::WillDraw(draw_mode, resource_provider);
}

void HeadsUpDisplayLayerImpl::AppendQuads(
    RenderPass* render_pass,
    AppendQuadsData* append_quads_data)
{
    if (!resources_.back()->id())
        return;

    SharedQuadState* shared_quad_state = render_pass->CreateAndAppendSharedQuadState();
    PopulateScaledSharedQuadState(shared_quad_state, internal_contents_scale_);

    gfx::Rect quad_rect(internal_content_bounds_);
    gfx::Rect opaque_rect(contents_opaque() ? quad_rect : gfx::Rect());
    gfx::Rect visible_quad_rect(quad_rect);
    bool premultiplied_alpha = true;
    gfx::PointF uv_top_left(0.f, 0.f);
    gfx::PointF uv_bottom_right(1.f, 1.f);
    const float vertex_opacity[] = { 1.f, 1.f, 1.f, 1.f };
    bool flipped = false;
    bool nearest_neighbor = false;
    TextureDrawQuad* quad = render_pass->CreateAndAppendDrawQuad<TextureDrawQuad>();
    quad->SetNew(shared_quad_state,
        quad_rect,
        opaque_rect,
        visible_quad_rect,
        resources_.back()->id(),
        premultiplied_alpha,
        uv_top_left,
        uv_bottom_right,
        SK_ColorTRANSPARENT,
        vertex_opacity,
        flipped,
        nearest_neighbor);
    ValidateQuadResources(quad);
}

void HeadsUpDisplayLayerImpl::UpdateHudTexture(
    DrawMode draw_mode,
    ResourceProvider* resource_provider)
{
    if (draw_mode == DRAW_MODE_RESOURCELESS_SOFTWARE || !resources_.back()->id())
        return;

    SkISize canvas_size;
    if (hud_surface_)
        canvas_size = hud_surface_->getCanvas()->getDeviceSize();
    else
        canvas_size.set(0, 0);

    if (canvas_size.width() != internal_content_bounds_.width() || canvas_size.height() != internal_content_bounds_.height() || !hud_surface_) {
        TRACE_EVENT0("cc", "ResizeHudCanvas");

        hud_surface_ = skia::AdoptRef(SkSurface::NewRasterN32Premul(
            internal_content_bounds_.width(), internal_content_bounds_.height()));
    }

    UpdateHudContents();

    {
        TRACE_EVENT0("cc", "DrawHudContents");
        hud_surface_->getCanvas()->clear(SkColorSetARGB(0, 0, 0, 0));
        hud_surface_->getCanvas()->save();
        hud_surface_->getCanvas()->scale(internal_contents_scale_,
            internal_contents_scale_);

        DrawHudContents(hud_surface_->getCanvas());

        hud_surface_->getCanvas()->restore();
    }

    TRACE_EVENT0("cc", "UploadHudTexture");
    SkImageInfo info;
    size_t row_bytes = 0;
    const void* pixels = hud_surface_->getCanvas()->peekPixels(&info, &row_bytes);
    DCHECK(pixels);
    DCHECK(info.colorType() == kN32_SkColorType);
    resource_provider->CopyToResource(resources_.back()->id(),
        static_cast<const uint8_t*>(pixels),
        internal_content_bounds_);
}

void HeadsUpDisplayLayerImpl::ReleaseResources()
{
    resources_.clear();
}

gfx::Rect HeadsUpDisplayLayerImpl::GetEnclosingRectInTargetSpace() const
{
    DCHECK_GT(internal_contents_scale_, 0.f);
    return GetScaledEnclosingRectInTargetSpace(internal_contents_scale_);
}

void HeadsUpDisplayLayerImpl::UpdateHudContents()
{
    const LayerTreeDebugState& debug_state = layer_tree_impl()->debug_state();

    // Don't update numbers every frame so text is readable.
    base::TimeTicks now = layer_tree_impl()->CurrentBeginFrameArgs().frame_time;
    if (base::TimeDelta(now - time_of_last_graph_update_).InSecondsF() > 0.25f) {
        time_of_last_graph_update_ = now;

        if (debug_state.show_fps_counter) {
            FrameRateCounter* fps_counter = layer_tree_impl()->frame_rate_counter();
            fps_graph_.value = fps_counter->GetAverageFPS();
            fps_counter->GetMinAndMaxFPS(&fps_graph_.min, &fps_graph_.max);
        }

        if (debug_state.ShowMemoryStats()) {
            MemoryHistory* memory_history = layer_tree_impl()->memory_history();
            if (memory_history->End())
                memory_entry_ = **memory_history->End();
            else
                memory_entry_ = MemoryHistory::Entry();
        }
    }

    fps_graph_.UpdateUpperBound();
    paint_time_graph_.UpdateUpperBound();
}

void HeadsUpDisplayLayerImpl::DrawHudContents(SkCanvas* canvas)
{
    const LayerTreeDebugState& debug_state = layer_tree_impl()->debug_state();

    if (debug_state.ShowHudRects()) {
        DrawDebugRects(canvas, layer_tree_impl()->debug_rect_history());
        if (IsAnimatingHUDContents()) {
            layer_tree_impl()->SetNeedsRedraw();
        }
    }

    if (!debug_state.show_fps_counter)
        return;

    SkRect area = DrawFPSDisplay(canvas, layer_tree_impl()->frame_rate_counter(), 0, 0);
    area = DrawGpuRasterizationStatus(canvas, 0, area.bottom(),
        SkMaxScalar(area.width(), 150));

    if (debug_state.ShowMemoryStats())
        DrawMemoryDisplay(canvas, 0, area.bottom(), SkMaxScalar(area.width(), 150));
}
int HeadsUpDisplayLayerImpl::MeasureText(SkPaint* paint,
    const std::string& text,
    int size) const
{
    const bool anti_alias = paint->isAntiAlias();
    paint->setAntiAlias(true);
    paint->setTextSize(size);
    paint->setTypeface(typeface_.get());
    SkScalar text_width = paint->measureText(text.c_str(), text.length());

    paint->setAntiAlias(anti_alias);
    return SkScalarCeilToInt(text_width);
}
void HeadsUpDisplayLayerImpl::DrawText(SkCanvas* canvas,
    SkPaint* paint,
    const std::string& text,
    SkPaint::Align align,
    int size,
    int x,
    int y) const
{
    const bool anti_alias = paint->isAntiAlias();
    paint->setAntiAlias(true);

    paint->setTextSize(size);
    paint->setTextAlign(align);
    paint->setTypeface(typeface_.get());
    canvas->drawText(text.c_str(), text.length(), x, y, *paint);

    paint->setAntiAlias(anti_alias);
}

void HeadsUpDisplayLayerImpl::DrawText(SkCanvas* canvas,
    SkPaint* paint,
    const std::string& text,
    SkPaint::Align align,
    int size,
    const SkPoint& pos) const
{
    DrawText(canvas, paint, text, align, size, pos.x(), pos.y());
}

void HeadsUpDisplayLayerImpl::DrawGraphBackground(SkCanvas* canvas,
    SkPaint* paint,
    const SkRect& bounds) const
{
    paint->setColor(DebugColors::HUDBackgroundColor());
    canvas->drawRect(bounds, *paint);
}

void HeadsUpDisplayLayerImpl::DrawGraphLines(SkCanvas* canvas,
    SkPaint* paint,
    const SkRect& bounds,
    const Graph& graph) const
{
    // Draw top and bottom line.
    paint->setColor(DebugColors::HUDSeparatorLineColor());
    canvas->drawLine(bounds.left(),
        bounds.top() - 1,
        bounds.right(),
        bounds.top() - 1,
        *paint);
    canvas->drawLine(
        bounds.left(), bounds.bottom(), bounds.right(), bounds.bottom(), *paint);

    // Draw indicator line (additive blend mode to increase contrast when drawn on
    // top of graph).
    paint->setColor(DebugColors::HUDIndicatorLineColor());
    paint->setXfermodeMode(SkXfermode::kPlus_Mode);
    const double indicator_top = bounds.height() * (1.0 - graph.indicator / graph.current_upper_bound) - 1.0;
    canvas->drawLine(bounds.left(),
        bounds.top() + indicator_top,
        bounds.right(),
        bounds.top() + indicator_top,
        *paint);
    paint->setXfermode(nullptr);
}

SkRect HeadsUpDisplayLayerImpl::DrawFPSDisplay(
    SkCanvas* canvas,
    const FrameRateCounter* fps_counter,
    int right,
    int top) const
{
    const int kPadding = 4;
    const int kGap = 6;

    const int kFontHeight = 15;

    const int kGraphWidth = base::saturated_cast<int>(fps_counter->time_stamp_history_size()) - 2;
    const int kGraphHeight = 40;

    const int kHistogramWidth = 37;

    int width = kGraphWidth + kHistogramWidth + 4 * kPadding;
    int height = kFontHeight + kGraphHeight + 4 * kPadding + 2;
    int left = bounds().width() - width - right;
    SkRect area = SkRect::MakeXYWH(left, top, width, height);

    SkPaint paint = CreatePaint();
    DrawGraphBackground(canvas, &paint, area);

    SkRect text_bounds = SkRect::MakeXYWH(left + kPadding,
        top + kPadding,
        kGraphWidth + kHistogramWidth + kGap + 2,
        kFontHeight);
    SkRect graph_bounds = SkRect::MakeXYWH(left + kPadding,
        text_bounds.bottom() + 2 * kPadding,
        kGraphWidth,
        kGraphHeight);
    SkRect histogram_bounds = SkRect::MakeXYWH(graph_bounds.right() + kGap,
        graph_bounds.top(),
        kHistogramWidth,
        kGraphHeight);

    const std::string value_text = base::StringPrintf("FPS:%5.1f", fps_graph_.value);
    const std::string min_max_text = base::StringPrintf("%.0f-%.0f", fps_graph_.min, fps_graph_.max);

    VLOG(1) << value_text;

    paint.setColor(DebugColors::FPSDisplayTextAndGraphColor());
    DrawText(canvas,
        &paint,
        value_text,
        SkPaint::kLeft_Align,
        kFontHeight,
        text_bounds.left(),
        text_bounds.bottom());
    DrawText(canvas,
        &paint,
        min_max_text,
        SkPaint::kRight_Align,
        kFontHeight,
        text_bounds.right(),
        text_bounds.bottom());

    DrawGraphLines(canvas, &paint, graph_bounds, fps_graph_);

    // Collect graph and histogram data.
    SkPath path;

    const int kHistogramSize = 20;
    double histogram[kHistogramSize] = { 1.0 };
    double max_bucket_value = 1.0;

    for (FrameRateCounter::RingBufferType::Iterator it = --fps_counter->end(); it;
         --it) {
        base::TimeDelta delta = fps_counter->RecentFrameInterval(it.index() + 1);

        // Skip this particular instantaneous frame rate if it is not likely to have
        // been valid.
        if (!fps_counter->IsBadFrameInterval(delta)) {
            double fps = 1.0 / delta.InSecondsF();

            // Clamp the FPS to the range we want to plot visually.
            double p = fps / fps_graph_.current_upper_bound;
            if (p > 1.0)
                p = 1.0;

            // Plot this data point.
            SkPoint cur = SkPoint::Make(graph_bounds.left() + it.index(),
                graph_bounds.bottom() - p * graph_bounds.height());
            if (path.isEmpty())
                path.moveTo(cur);
            else
                path.lineTo(cur);

            // Use the fps value to find the right bucket in the histogram.
            int bucket_index = floor(p * (kHistogramSize - 1));

            // Add the delta time to take the time spent at that fps rate into
            // account.
            histogram[bucket_index] += delta.InSecondsF();
            max_bucket_value = std::max(histogram[bucket_index], max_bucket_value);
        }
    }

    // Draw FPS histogram.
    paint.setColor(DebugColors::HUDSeparatorLineColor());
    canvas->drawLine(histogram_bounds.left() - 1,
        histogram_bounds.top() - 1,
        histogram_bounds.left() - 1,
        histogram_bounds.bottom() + 1,
        paint);
    canvas->drawLine(histogram_bounds.right() + 1,
        histogram_bounds.top() - 1,
        histogram_bounds.right() + 1,
        histogram_bounds.bottom() + 1,
        paint);

    paint.setColor(DebugColors::FPSDisplayTextAndGraphColor());
    const double bar_height = histogram_bounds.height() / kHistogramSize;

    for (int i = kHistogramSize - 1; i >= 0; --i) {
        if (histogram[i] > 0) {
            double bar_width = histogram[i] / max_bucket_value * histogram_bounds.width();
            canvas->drawRect(
                SkRect::MakeXYWH(histogram_bounds.left(),
                    histogram_bounds.bottom() - (i + 1) * bar_height,
                    bar_width,
                    1),
                paint);
        }
    }

    // Draw FPS graph.
    paint.setAntiAlias(true);
    paint.setStyle(SkPaint::kStroke_Style);
    paint.setStrokeWidth(1);
    canvas->drawPath(path, paint);

    return area;
}

SkRect HeadsUpDisplayLayerImpl::DrawMemoryDisplay(SkCanvas* canvas,
    int right,
    int top,
    int width) const
{
    if (!memory_entry_.total_bytes_used)
        return SkRect::MakeEmpty();

    const int kPadding = 4;
    const int kFontHeight = 13;

    const int height = 3 * kFontHeight + 4 * kPadding;
    const int left = bounds().width() - width - right;
    const SkRect area = SkRect::MakeXYWH(left, top, width, height);

    const double kMegabyte = 1024.0 * 1024.0;

    SkPaint paint = CreatePaint();
    DrawGraphBackground(canvas, &paint, area);

    SkPoint title_pos = SkPoint::Make(left + kPadding, top + kFontHeight);
    SkPoint stat1_pos = SkPoint::Make(left + width - kPadding - 1,
        top + kPadding + 2 * kFontHeight);
    SkPoint stat2_pos = SkPoint::Make(left + width - kPadding - 1,
        top + 2 * kPadding + 3 * kFontHeight);

    paint.setColor(DebugColors::MemoryDisplayTextColor());
    DrawText(canvas,
        &paint,
        "GPU memory",
        SkPaint::kLeft_Align,
        kFontHeight,
        title_pos);

    std::string text = base::StringPrintf(
        "%6.1f MB used", memory_entry_.total_bytes_used / kMegabyte);
    DrawText(canvas, &paint, text, SkPaint::kRight_Align, kFontHeight, stat1_pos);

    if (!memory_entry_.had_enough_memory)
        paint.setColor(SK_ColorRED);
    text = base::StringPrintf("%6.1f MB max ",
        memory_entry_.total_budget_in_bytes / kMegabyte);
    DrawText(canvas, &paint, text, SkPaint::kRight_Align, kFontHeight, stat2_pos);

    return area;
}

SkRect HeadsUpDisplayLayerImpl::DrawGpuRasterizationStatus(SkCanvas* canvas,
    int right,
    int top,
    int width) const
{
    std::string status;
    SkColor color = SK_ColorRED;
    switch (layer_tree_impl()->GetGpuRasterizationStatus()) {
    case GpuRasterizationStatus::ON:
        status = "on";
        color = SK_ColorGREEN;
        break;
    case GpuRasterizationStatus::ON_FORCED:
        status = "on (forced)";
        color = SK_ColorGREEN;
        break;
    case GpuRasterizationStatus::OFF_DEVICE:
        status = "off (device)";
        color = SK_ColorRED;
        break;
    case GpuRasterizationStatus::OFF_VIEWPORT:
        status = "off (viewport)";
        color = SK_ColorYELLOW;
        break;
    case GpuRasterizationStatus::MSAA_CONTENT:
        status = "MSAA (content)";
        color = SK_ColorCYAN;
        break;
    case GpuRasterizationStatus::OFF_CONTENT:
        status = "off (content)";
        color = SK_ColorYELLOW;
        break;
    }

    if (status.empty())
        return SkRect::MakeEmpty();

    const int kPadding = 4;
    const int kFontHeight = 13;

    const int height = 2 * kFontHeight + 3 * kPadding;
    const int left = bounds().width() - width - right;
    const SkRect area = SkRect::MakeXYWH(left, top, width, height);

    SkPaint paint = CreatePaint();
    DrawGraphBackground(canvas, &paint, area);

    SkPoint gpu_status_pos = SkPoint::Make(left + width - kPadding,
        top + 2 * kFontHeight + 2 * kPadding);

    paint.setColor(color);
    DrawText(canvas, &paint, "GPU raster: ", SkPaint::kLeft_Align, kFontHeight,
        left + kPadding, top + kFontHeight + kPadding);
    DrawText(canvas, &paint, status, SkPaint::kRight_Align, kFontHeight,
        gpu_status_pos);

    return area;
}

void HeadsUpDisplayLayerImpl::DrawDebugRect(
    SkCanvas* canvas,
    SkPaint* paint,
    const DebugRect& rect,
    SkColor stroke_color,
    SkColor fill_color,
    float stroke_width,
    const std::string& label_text) const
{
    gfx::Rect debug_layer_rect = gfx::ScaleToEnclosingRect(rect.rect, 1.0 / internal_contents_scale_,
        1.0 / internal_contents_scale_);
    SkIRect sk_rect = RectToSkIRect(debug_layer_rect);
    paint->setColor(fill_color);
    paint->setStyle(SkPaint::kFill_Style);
    canvas->drawIRect(sk_rect, *paint);

    paint->setColor(stroke_color);
    paint->setStyle(SkPaint::kStroke_Style);
    paint->setStrokeWidth(SkFloatToScalar(stroke_width));
    canvas->drawIRect(sk_rect, *paint);

    if (label_text.length()) {
        const int kFontHeight = 12;
        const int kPadding = 3;

        // The debug_layer_rect may be huge, and converting to a floating point may
        // be lossy, so intersect with the HUD layer bounds first to prevent that.
        gfx::Rect clip_rect = debug_layer_rect;
        clip_rect.Intersect(gfx::Rect(internal_content_bounds_));
        SkRect sk_clip_rect = RectToSkRect(clip_rect);

        canvas->save();
        canvas->clipRect(sk_clip_rect);
        canvas->translate(sk_clip_rect.x(), sk_clip_rect.y());

        SkPaint label_paint = CreatePaint();
        label_paint.setTextSize(kFontHeight);
        label_paint.setTypeface(typeface_.get());
        label_paint.setColor(stroke_color);

        const SkScalar label_text_width = label_paint.measureText(label_text.c_str(), label_text.length());
        canvas->drawRect(SkRect::MakeWH(label_text_width + 2 * kPadding,
                             kFontHeight + 2 * kPadding),
            label_paint);

        label_paint.setAntiAlias(true);
        label_paint.setColor(SkColorSetARGB(255, 50, 50, 50));
        canvas->drawText(label_text.c_str(),
            label_text.length(),
            kPadding,
            kFontHeight * 0.8f + kPadding,
            label_paint);

        canvas->restore();
    }
}

void HeadsUpDisplayLayerImpl::DrawDebugRects(
    SkCanvas* canvas,
    DebugRectHistory* debug_rect_history)
{
    SkPaint paint = CreatePaint();

    const std::vector<DebugRect>& debug_rects = debug_rect_history->debug_rects();
    std::vector<DebugRect> new_paint_rects;

    for (size_t i = 0; i < debug_rects.size(); ++i) {
        SkColor stroke_color = 0;
        SkColor fill_color = 0;
        float stroke_width = 0.f;
        std::string label_text;

        switch (debug_rects[i].type) {
        case PAINT_RECT_TYPE:
            new_paint_rects.push_back(debug_rects[i]);
            continue;
        case PROPERTY_CHANGED_RECT_TYPE:
            stroke_color = DebugColors::PropertyChangedRectBorderColor();
            fill_color = DebugColors::PropertyChangedRectFillColor();
            stroke_width = DebugColors::PropertyChangedRectBorderWidth();
            break;
        case SURFACE_DAMAGE_RECT_TYPE:
            stroke_color = DebugColors::SurfaceDamageRectBorderColor();
            fill_color = DebugColors::SurfaceDamageRectFillColor();
            stroke_width = DebugColors::SurfaceDamageRectBorderWidth();
            break;
        case REPLICA_SCREEN_SPACE_RECT_TYPE:
            stroke_color = DebugColors::ScreenSpaceSurfaceReplicaRectBorderColor();
            fill_color = DebugColors::ScreenSpaceSurfaceReplicaRectFillColor();
            stroke_width = DebugColors::ScreenSpaceSurfaceReplicaRectBorderWidth();
            break;
        case SCREEN_SPACE_RECT_TYPE:
            stroke_color = DebugColors::ScreenSpaceLayerRectBorderColor();
            fill_color = DebugColors::ScreenSpaceLayerRectFillColor();
            stroke_width = DebugColors::ScreenSpaceLayerRectBorderWidth();
            break;
        case TOUCH_EVENT_HANDLER_RECT_TYPE:
            stroke_color = DebugColors::TouchEventHandlerRectBorderColor();
            fill_color = DebugColors::TouchEventHandlerRectFillColor();
            stroke_width = DebugColors::TouchEventHandlerRectBorderWidth();
            label_text = "touch event listener";
            break;
        case WHEEL_EVENT_HANDLER_RECT_TYPE:
            stroke_color = DebugColors::WheelEventHandlerRectBorderColor();
            fill_color = DebugColors::WheelEventHandlerRectFillColor();
            stroke_width = DebugColors::WheelEventHandlerRectBorderWidth();
            label_text = "mousewheel event listener";
            break;
        case SCROLL_EVENT_HANDLER_RECT_TYPE:
            stroke_color = DebugColors::ScrollEventHandlerRectBorderColor();
            fill_color = DebugColors::ScrollEventHandlerRectFillColor();
            stroke_width = DebugColors::ScrollEventHandlerRectBorderWidth();
            label_text = "scroll event listener";
            break;
        case NON_FAST_SCROLLABLE_RECT_TYPE:
            stroke_color = DebugColors::NonFastScrollableRectBorderColor();
            fill_color = DebugColors::NonFastScrollableRectFillColor();
            stroke_width = DebugColors::NonFastScrollableRectBorderWidth();
            label_text = "repaints on scroll";
            break;
        case ANIMATION_BOUNDS_RECT_TYPE:
            stroke_color = DebugColors::LayerAnimationBoundsBorderColor();
            fill_color = DebugColors::LayerAnimationBoundsFillColor();
            stroke_width = DebugColors::LayerAnimationBoundsBorderWidth();
            label_text = "animation bounds";
            break;
        }

        DrawDebugRect(canvas,
            &paint,
            debug_rects[i],
            stroke_color,
            fill_color,
            stroke_width,
            label_text);
    }

    if (new_paint_rects.size()) {
        paint_rects_.swap(new_paint_rects);
        fade_step_ = DebugColors::kFadeSteps;
    }
    if (fade_step_ > 0) {
        fade_step_--;
        for (size_t i = 0; i < paint_rects_.size(); ++i) {
            DrawDebugRect(canvas,
                &paint,
                paint_rects_[i],
                DebugColors::PaintRectBorderColor(fade_step_),
                DebugColors::PaintRectFillColor(fade_step_),
                DebugColors::PaintRectBorderWidth(),
                "");
        }
    }
}

const char* HeadsUpDisplayLayerImpl::LayerTypeAsString() const
{
    return "cc::HeadsUpDisplayLayerImpl";
}

void HeadsUpDisplayLayerImpl::AsValueInto(
    base::trace_event::TracedValue* dict) const
{
    LayerImpl::AsValueInto(dict);
    dict->SetString("layer_name", "Heads Up Display Layer");
}

} // namespace cc
