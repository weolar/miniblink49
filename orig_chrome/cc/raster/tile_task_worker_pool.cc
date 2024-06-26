// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/raster/tile_task_worker_pool.h"

#include <algorithm>

#include "base/trace_event/trace_event.h"
#include "cc/playback/raster_source.h"
#include "skia/ext/refptr.h"
#include "third_party/skia/include/core/SkCanvas.h"
#include "third_party/skia/include/core/SkDrawFilter.h"
#include "third_party/skia/include/core/SkSurface.h"

//#include "third_party/WebKit/Source/wtf/CurrentTime.h"

// extern DWORD g_rasterTime; // weolar TODO
// extern DWORD g_nowTime;
// extern int g_mouseTest;
// extern DWORD g_rasterTimeInMouse;

namespace wke {
extern bool g_drawTileLineEnable;
}

namespace cc {
namespace {

    class TaskSetFinishedTaskImpl : public TileTask {
    public:
        explicit TaskSetFinishedTaskImpl(
            base::SequencedTaskRunner* task_runner,
            const base::Closure& on_task_set_finished_callback)
            : task_runner_(task_runner)
            , on_task_set_finished_callback_(on_task_set_finished_callback)
        {
        }

        // Overridden from Task:
        void RunOnWorkerThread() override
        {
            TRACE_EVENT0("cc", "TaskSetFinishedTaskImpl::RunOnWorkerThread");
            TaskSetFinished();
        }

        // Overridden from TileTask:
        void ScheduleOnOriginThread(TileTaskClient* client) override { }
        void CompleteOnOriginThread(TileTaskClient* client) override { }

    protected:
        ~TaskSetFinishedTaskImpl() override { }

        void TaskSetFinished()
        {
            task_runner_->PostTask(FROM_HERE, on_task_set_finished_callback_);
        }

    private:
        scoped_refptr<base::SequencedTaskRunner> task_runner_;
        const base::Closure on_task_set_finished_callback_;

        DISALLOW_COPY_AND_ASSIGN(TaskSetFinishedTaskImpl);
    };

} // namespace

// This allows a micro benchmark system to run tasks with highest priority,
// since it should finish as quickly as possible.
size_t TileTaskWorkerPool::kBenchmarkTaskPriority = 0u;
// Task priorities that make sure task set finished tasks run before any
// other remaining tasks. This is combined with the task set type to ensure
// proper prioritization ordering between task set types.
size_t TileTaskWorkerPool::kTaskSetFinishedTaskPriorityBase = 1u;
// For correctness, |kTileTaskPriorityBase| must be greater than
// |kTaskSetFinishedTaskPriorityBase + kNumberOfTaskSets|.
size_t TileTaskWorkerPool::kTileTaskPriorityBase = 10u;

TileTaskWorkerPool::TileTaskWorkerPool()
{
}

TileTaskWorkerPool::~TileTaskWorkerPool()
{
}

// static
scoped_refptr<TileTask> TileTaskWorkerPool::CreateTaskSetFinishedTask(
    base::SequencedTaskRunner* task_runner,
    const base::Closure& on_task_set_finished_callback)
{
    return make_scoped_refptr(
        new TaskSetFinishedTaskImpl(task_runner, on_task_set_finished_callback));
}

// static
void TileTaskWorkerPool::ScheduleTasksOnOriginThread(TileTaskClient* client,
    TaskGraph* graph)
{
    TRACE_EVENT0("cc", "TileTaskWorkerPool::ScheduleTasksOnOriginThread");

    for (TaskGraph::Node::Vector::iterator it = graph->nodes.begin();
         it != graph->nodes.end(); ++it) {
        TaskGraph::Node& node = *it;
        TileTask* task = static_cast<TileTask*>(node.task);

        if (!task->HasBeenScheduled()) {
            task->WillSchedule();
            task->ScheduleOnOriginThread(client);
            task->DidSchedule();
        }
    }
}

// static
void TileTaskWorkerPool::InsertNodeForTask(TaskGraph* graph,
    TileTask* task,
    size_t priority,
    size_t dependencies)
{
    DCHECK(std::find_if(graph->nodes.begin(), graph->nodes.end(),
               TaskGraph::Node::TaskComparator(task))
        == graph->nodes.end());
    graph->nodes.push_back(TaskGraph::Node(task, priority, dependencies));
}

// static
void TileTaskWorkerPool::InsertNodesForRasterTask(
    TaskGraph* graph,
    RasterTask* raster_task,
    const ImageDecodeTask::Vector& decode_tasks,
    size_t priority)
{
    size_t dependencies = 0u;

    // Insert image decode tasks.
    for (ImageDecodeTask::Vector::const_iterator it = decode_tasks.begin();
         it != decode_tasks.end(); ++it) {
        ImageDecodeTask* decode_task = it->get();

        // Skip if already decoded.
        if (decode_task->HasCompleted())
            continue;

        dependencies++;

        // Add decode task if it doesn't already exists in graph.
        TaskGraph::Node::Vector::iterator decode_it = std::find_if(graph->nodes.begin(), graph->nodes.end(),
            TaskGraph::Node::TaskComparator(decode_task));
        if (decode_it == graph->nodes.end())
            InsertNodeForTask(graph, decode_task, priority, 0u);

        graph->edges.push_back(TaskGraph::Edge(decode_task, raster_task));
    }

    InsertNodeForTask(graph, raster_task, priority, dependencies);
}

static bool IsSupportedPlaybackToMemoryFormat(ResourceFormat format)
{
    switch (format) {
    case RGBA_4444:
    case RGBA_8888:
    case BGRA_8888:
        return true;
    case ALPHA_8:
    case LUMINANCE_8:
    case RGB_565:
    case ETC1:
    case RED_8:
        return false;
    }
    NOTREACHED();
    return false;
}

class SkipImageFilter : public SkDrawFilter {
public:
    bool filter(SkPaint* paint, Type type) override
    {
        if (type == kBitmap_Type)
            return false;
        DebugBreak();
        SkShader* shader = paint->getShader();
        return !shader /*|| !shader->isABitmap()*/;
    }
};

void ShowVisualIndicatorBegin(SkCanvas* canvas, const gfx::Rect& content_rect)
{
    canvas->save();
    if (!wke::g_drawTileLineEnable)
        return;
}

void ShowVisualIndicatorEnd(SkCanvas* canvas, const gfx::Rect& content_rect)
{
    canvas->restore();
    if (!wke::g_drawTileLineEnable)
        return;

    SkColor color = (GetTickCount() % 3) * (GetTickCount() % 7) * GetTickCount();

    // only color the invalidated area
    SkPaint paint;
    paint.setColor(color);
    SkIRect rect;
    rect.set(0, 0, content_rect.width(), content_rect.height());
    canvas->drawIRect(rect, paint);

    paint.setARGB(128, 255, 255, 255);
    canvas->drawRectCoords(0, 0, content_rect.width(), 17, paint);
    paint.setARGB(255, 255, 0, 0);

    // paint the tile boundaries
    paint.setARGB(64, 255, 0, 0);
    paint.setStrokeWidth(3);
    canvas->drawLine(0, 0, content_rect.width(), content_rect.height(), paint);
    paint.setARGB(64, 0, 255, 0);
    canvas->drawLine(0, content_rect.height(), content_rect.width(), 0, paint);
    paint.setARGB(128, 0, 0, 255);
    canvas->drawLine(content_rect.width(), 0, content_rect.width(), content_rect.height(), paint);

    char* output = new char[300];
    sprintf(output, "ShowVisualIndicatorEnd: %d %d\n", content_rect.width(), content_rect.height());
    OutputDebugStringA(output);
    delete output;
}

// static
void TileTaskWorkerPool::PlaybackToMemory(void* memory,
    ResourceFormat format,
    const gfx::Size& size,
    size_t stride,
    const RasterSource* raster_source,
    const gfx::Rect& canvas_bitmap_rect,
    const gfx::Rect& canvas_playback_rect,
    float scale,
    bool include_images)
{
    TRACE_EVENT0("cc", "TileTaskWorkerPool::PlaybackToMemory");

    //DWORD rasterTime = (DWORD)(WTF::currentTimeMS() * 100);

    DCHECK(IsSupportedPlaybackToMemoryFormat(format)) << format;

    // Uses kPremul_SkAlphaType since the result is not known to be opaque.
    SkImageInfo info = SkImageInfo::MakeN32(size.width(), size.height(), kPremul_SkAlphaType);
    SkColorType buffer_color_type = ResourceFormatToSkColorType(format);
    bool needs_copy = buffer_color_type != info.colorType();

    // Use unknown pixel geometry to disable LCD text.
    SkSurfaceProps surface_props(0, kUnknown_SkPixelGeometry);
    if (raster_source->CanUseLCDText()) {
        // LegacyFontHost will get LCD text and skia figures out what type to use.
        surface_props = SkSurfaceProps(SkSurfaceProps::kLegacyFontHost_InitType);
    }

    if (!stride)
        stride = info.minRowBytes();
    DCHECK_GT(stride, 0u);

    skia::RefPtr<SkDrawFilter> image_filter;
    if (!include_images)
        image_filter = skia::AdoptRef(new SkipImageFilter);

    if (!needs_copy) {
        skia::RefPtr<SkSurface> surface = skia::AdoptRef(
            SkSurface::NewRasterDirect(info, memory, stride, &surface_props));
        skia::RefPtr<SkCanvas> canvas = skia::SharePtr(surface->getCanvas());
        canvas->setDrawFilter(image_filter.get());

        ShowVisualIndicatorBegin(canvas.get(), canvas_bitmap_rect);
        raster_source->PlaybackToCanvas(canvas.get(), canvas_bitmap_rect,
            canvas_playback_rect, scale);
        ShowVisualIndicatorEnd(canvas.get(), canvas_bitmap_rect);

        //     DWORD nowTime = (DWORD)(WTF::currentTimeMS() * 100);
        //     DWORD detTime = nowTime - rasterTime;
        //     InterlockedExchangeAdd((unsigned int *)&g_rasterTime, (unsigned int)(detTime));
        //
        //     if (1 == g_mouseTest) {
        //         InterlockedExchangeAdd((unsigned int *)&g_rasterTimeInMouse, (unsigned int)(detTime));
        //     }
        //     InterlockedExchange(&g_nowTime, nowTime);

        return;
    }

    skia::RefPtr<SkSurface> surface = skia::AdoptRef(SkSurface::NewRaster(info, &surface_props));
    skia::RefPtr<SkCanvas> canvas = skia::SharePtr(surface->getCanvas());
    canvas->setDrawFilter(image_filter.get());
    // TODO(reveman): Improve partial raster support by reducing the size of
    // playback rect passed to PlaybackToCanvas. crbug.com/519070
    raster_source->PlaybackToCanvas(canvas.get(), canvas_bitmap_rect,
        canvas_bitmap_rect, scale);

    {
        TRACE_EVENT0("cc", "TileTaskWorkerPool::PlaybackToMemory::ConvertPixels");

        SkImageInfo dst_info = SkImageInfo::Make(info.width(), info.height(), buffer_color_type,
            info.alphaType(), info.profileType());
        bool rv = canvas->readPixels(dst_info, memory, stride, 0, 0);
        DCHECK(rv);
    }
}

} // namespace cc
