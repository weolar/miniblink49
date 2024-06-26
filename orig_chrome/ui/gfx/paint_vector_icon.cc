// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/paint_vector_icon.h"

#include <map>
#include <tuple>

#include "base/i18n/rtl.h"
#include "base/lazy_instance.h"
#include "base/macros.h"
#include "base/strings/string_number_conversions.h"
#include "base/strings/string_split.h"
#include "third_party/skia/include/core/SkPaint.h"
#include "third_party/skia/include/core/SkPath.h"
#include "third_party/skia/include/core/SkXfermode.h"
#include "ui/gfx/canvas.h"
#include "ui/gfx/image/canvas_image_source.h"
#include "ui/gfx/vector_icon_types.h"
#include "ui/gfx/vector_icons.h"

namespace gfx {

namespace {

    // Translates a string such as "MOVE_TO" into a command such as MOVE_TO.
    CommandType CommandFromString(const std::string& source)
    {
#define RETURN_IF_IS(command) \
    if (source == #command)   \
        return command;

        RETURN_IF_IS(NEW_PATH);
        RETURN_IF_IS(PATH_COLOR_ARGB);
        RETURN_IF_IS(PATH_MODE_CLEAR);
        RETURN_IF_IS(STROKE);
        RETURN_IF_IS(CAP_SQUARE);
        RETURN_IF_IS(MOVE_TO);
        RETURN_IF_IS(R_MOVE_TO);
        RETURN_IF_IS(LINE_TO);
        RETURN_IF_IS(R_LINE_TO);
        RETURN_IF_IS(H_LINE_TO);
        RETURN_IF_IS(R_H_LINE_TO);
        RETURN_IF_IS(V_LINE_TO);
        RETURN_IF_IS(R_V_LINE_TO);
        RETURN_IF_IS(CUBIC_TO);
        RETURN_IF_IS(R_CUBIC_TO);
        RETURN_IF_IS(CIRCLE);
        RETURN_IF_IS(ROUND_RECT);
        RETURN_IF_IS(CLOSE);
        RETURN_IF_IS(CANVAS_DIMENSIONS);
        RETURN_IF_IS(CLIP);
        RETURN_IF_IS(DISABLE_AA);
        RETURN_IF_IS(FLIPS_IN_RTL);
        RETURN_IF_IS(END);
#undef RETURN_IF_IS

        NOTREACHED();
        return CLOSE;
    }

    std::vector<PathElement> PathFromSource(const std::string& source)
    {
        std::vector<PathElement> path;
        std::vector<std::string> pieces = base::SplitString(
            source, "\n ,f", base::TRIM_WHITESPACE, base::SPLIT_WANT_NONEMPTY);
        for (const auto& piece : pieces) {
            double value;
            if (base::StringToDouble(piece, &value))
                path.push_back(PathElement(SkDoubleToScalar(value)));
            else
                path.push_back(PathElement(CommandFromString(piece)));
        }
        return path;
    }

    void PaintPath(Canvas* canvas,
        const PathElement* path_elements,
        size_t dip_size,
        SkColor color)
    {
        canvas->Save();
        SkPath path;
        path.setFillType(SkPath::kEvenOdd_FillType);

        size_t canvas_size = kReferenceSizeDip;
        std::vector<SkPath> paths;
        std::vector<SkPaint> paints;
        SkRect clip_rect = SkRect::MakeEmpty();
        bool flips_in_rtl = false;

        for (size_t i = 0; path_elements[i].type != END; i++) {
            if (paths.empty() || path_elements[i].type == NEW_PATH) {
                paths.push_back(SkPath());
                paths.back().setFillType(SkPath::kEvenOdd_FillType);

                paints.push_back(SkPaint());
                paints.back().setColor(color);
                paints.back().setAntiAlias(true);
                paints.back().setStrokeCap(SkPaint::kRound_Cap);
            }

            SkPath& path = paths.back();
            SkPaint& paint = paints.back();
            switch (path_elements[i].type) {
            // Handled above.
            case NEW_PATH:
                continue;

            case PATH_COLOR_ARGB: {
                int a = SkScalarFloorToInt(path_elements[++i].arg);
                int r = SkScalarFloorToInt(path_elements[++i].arg);
                int g = SkScalarFloorToInt(path_elements[++i].arg);
                int b = SkScalarFloorToInt(path_elements[++i].arg);
                paint.setColor(SkColorSetARGB(a, r, g, b));
                break;
            }

            case PATH_MODE_CLEAR: {
                paint.setXfermodeMode(SkXfermode::kClear_Mode);
                break;
            };

            case STROKE: {
                paint.setStyle(SkPaint::kStroke_Style);
                SkScalar width = path_elements[++i].arg;
                paint.setStrokeWidth(width);
                break;
            }

            case CAP_SQUARE: {
                paint.setStrokeCap(SkPaint::kSquare_Cap);
                break;
            }

            case MOVE_TO: {
                SkScalar x = path_elements[++i].arg;
                SkScalar y = path_elements[++i].arg;
                path.moveTo(x, y);
                break;
            }

            case R_MOVE_TO: {
                SkScalar x = path_elements[++i].arg;
                SkScalar y = path_elements[++i].arg;
                path.rMoveTo(x, y);
                break;
            }

            case LINE_TO: {
                SkScalar x = path_elements[++i].arg;
                SkScalar y = path_elements[++i].arg;
                path.lineTo(x, y);
                break;
            }

            case R_LINE_TO: {
                SkScalar x = path_elements[++i].arg;
                SkScalar y = path_elements[++i].arg;
                path.rLineTo(x, y);
                break;
            }

            case H_LINE_TO: {
                SkPoint last_point;
                path.getLastPt(&last_point);
                SkScalar x = path_elements[++i].arg;
                path.lineTo(x, last_point.fY);
                break;
            }

            case R_H_LINE_TO: {
                SkScalar x = path_elements[++i].arg;
                path.rLineTo(x, 0);
                break;
            }

            case V_LINE_TO: {
                SkPoint last_point;
                path.getLastPt(&last_point);
                SkScalar y = path_elements[++i].arg;
                path.lineTo(last_point.fX, y);
                break;
            }

            case R_V_LINE_TO: {
                SkScalar y = path_elements[++i].arg;
                path.rLineTo(0, y);
                break;
            }

            case CUBIC_TO: {
                SkScalar x1 = path_elements[++i].arg;
                SkScalar y1 = path_elements[++i].arg;
                SkScalar x2 = path_elements[++i].arg;
                SkScalar y2 = path_elements[++i].arg;
                SkScalar x3 = path_elements[++i].arg;
                SkScalar y3 = path_elements[++i].arg;
                path.cubicTo(x1, y1, x2, y2, x3, y3);
                break;
            }

            case R_CUBIC_TO: {
                SkScalar x1 = path_elements[++i].arg;
                SkScalar y1 = path_elements[++i].arg;
                SkScalar x2 = path_elements[++i].arg;
                SkScalar y2 = path_elements[++i].arg;
                SkScalar x3 = path_elements[++i].arg;
                SkScalar y3 = path_elements[++i].arg;
                path.rCubicTo(x1, y1, x2, y2, x3, y3);
                break;
            }

            case CIRCLE: {
                SkScalar x = path_elements[++i].arg;
                SkScalar y = path_elements[++i].arg;
                SkScalar r = path_elements[++i].arg;
                path.addCircle(x, y, r);
                break;
            }

            case ROUND_RECT: {
                SkScalar x = path_elements[++i].arg;
                SkScalar y = path_elements[++i].arg;
                SkScalar w = path_elements[++i].arg;
                SkScalar h = path_elements[++i].arg;
                SkScalar radius = path_elements[++i].arg;
                path.addRoundRect(SkRect::MakeXYWH(x, y, w, h), radius, radius);
                break;
            }

            case CLOSE: {
                path.close();
                break;
            }

            case CANVAS_DIMENSIONS: {
                SkScalar width = path_elements[++i].arg;
                canvas_size = SkScalarTruncToInt(width);
                break;
            }

            case CLIP: {
                SkScalar x = path_elements[++i].arg;
                SkScalar y = path_elements[++i].arg;
                SkScalar w = path_elements[++i].arg;
                SkScalar h = path_elements[++i].arg;
                clip_rect = SkRect::MakeXYWH(x, y, w, h);
                break;
            }

            case DISABLE_AA: {
                paint.setAntiAlias(false);
                break;
            }

            case FLIPS_IN_RTL: {
                flips_in_rtl = true;
                break;
            }

            case END:
                NOTREACHED();
                break;
            }
        }

        if (flips_in_rtl && base::i18n::IsRTL()) {
            canvas->Scale(-1, 1);
            canvas->Translate(gfx::Vector2d(-static_cast<int>(canvas_size), 0));
        }

        if (dip_size != canvas_size) {
            SkScalar scale = SkIntToScalar(dip_size) / SkIntToScalar(canvas_size);
            canvas->sk_canvas()->scale(scale, scale);
        }

        if (!clip_rect.isEmpty())
            canvas->sk_canvas()->clipRect(clip_rect);

        DCHECK_EQ(paints.size(), paths.size());
        for (size_t i = 0; i < paths.size(); ++i)
            canvas->DrawPath(paths[i], paints[i]);
        canvas->Restore();
    }

    class VectorIconSource : public CanvasImageSource {
    public:
        VectorIconSource(VectorIconId id,
            size_t dip_size,
            SkColor color,
            VectorIconId badge_id)
            : CanvasImageSource(
                gfx::Size(static_cast<int>(dip_size), static_cast<int>(dip_size)),
                false)
            , id_(id)
            , color_(color)
            , badge_id_(badge_id)
        {
        }

        VectorIconSource(const std::string& definition,
            size_t dip_size,
            SkColor color)
            : CanvasImageSource(
                gfx::Size(static_cast<int>(dip_size), static_cast<int>(dip_size)),
                false)
            , id_(VectorIconId::VECTOR_ICON_NONE)
            , path_(PathFromSource(definition))
            , color_(color)
            , badge_id_(VectorIconId::VECTOR_ICON_NONE)
        {
        }

        ~VectorIconSource() override { }

        // CanvasImageSource:
        void Draw(gfx::Canvas* canvas) override
        {
            if (path_.empty()) {
                PaintVectorIcon(canvas, id_, size_.width(), color_);
                if (badge_id_ != VectorIconId::VECTOR_ICON_NONE)
                    PaintVectorIcon(canvas, badge_id_, size_.width(), color_);
            } else {
                PaintPath(canvas, path_.data(), size_.width(), color_);
            }
        }

    private:
        const VectorIconId id_;
        const std::vector<PathElement> path_;
        const SkColor color_;
        const VectorIconId badge_id_;

        DISALLOW_COPY_AND_ASSIGN(VectorIconSource);
    };

    // This class caches vector icons (as ImageSkia) so they don't have to be drawn
    // more than once. This also guarantees the backing data for the images returned
    // by CreateVectorIcon will persist in memory until program termination.
    class VectorIconCache {
    public:
        VectorIconCache() { }
        ~VectorIconCache() { }

        ImageSkia GetOrCreateIcon(VectorIconId id,
            size_t dip_size,
            SkColor color,
            VectorIconId badge_id)
        {
            IconDescription description(id, dip_size, color, badge_id);
            auto iter = images_.find(description);
            if (iter != images_.end())
                return iter->second;

            ImageSkia icon(
                new VectorIconSource(id, dip_size, color, badge_id),
                gfx::Size(static_cast<int>(dip_size), static_cast<int>(dip_size)));
            images_.insert(std::make_pair(description, icon));
            return icon;
        }

    private:
        struct IconDescription {
            IconDescription(VectorIconId id,
                size_t dip_size,
                SkColor color,
                VectorIconId badge_id)
                : id(id)
                , dip_size(dip_size)
                , color(color)
                , badge_id(badge_id)
            {
            }

            bool operator<(const IconDescription& other) const
            {
                return std::tie(id, dip_size, color, badge_id) < std::tie(other.id, other.dip_size, other.color, other.badge_id);
            }

            VectorIconId id;
            size_t dip_size;
            SkColor color;
            VectorIconId badge_id;
        };

        std::map<IconDescription, ImageSkia> images_;

        DISALLOW_COPY_AND_ASSIGN(VectorIconCache);
    };

    static base::LazyInstance<VectorIconCache> g_icon_cache = LAZY_INSTANCE_INITIALIZER;

} // namespace

void PaintVectorIcon(Canvas* canvas,
    VectorIconId id,
    size_t dip_size,
    SkColor color)
{
    DCHECK(VectorIconId::VECTOR_ICON_NONE != id);
    const PathElement* path = canvas->image_scale() == 1.f
        ? GetPathForVectorIconAt1xScale(id)
        : GetPathForVectorIcon(id);
    PaintPath(canvas, path, dip_size, color);
}

ImageSkia CreateVectorIcon(VectorIconId id, size_t dip_size, SkColor color)
{
    return CreateVectorIconWithBadge(id, dip_size, color,
        VectorIconId::VECTOR_ICON_NONE);
}

ImageSkia CreateVectorIconWithBadge(VectorIconId id,
    size_t dip_size,
    SkColor color,
    VectorIconId badge_id)
{
    return g_icon_cache.Get().GetOrCreateIcon(id, dip_size, color, badge_id);
}

ImageSkia CreateVectorIconFromSource(const std::string& source,
    size_t dip_size,
    SkColor color)
{
    return ImageSkia(
        new VectorIconSource(source, dip_size, color),
        gfx::Size(static_cast<int>(dip_size), static_cast<int>(dip_size)));
}

} // namespace gfx
