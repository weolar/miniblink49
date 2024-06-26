// Copyright 2015 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/tiles/image_decode_controller.h"

#include "cc/debug/devtools_instrumentation.h"

namespace cc {
namespace {

    class ImageDecodeTaskImpl : public ImageDecodeTask {
    public:
        ImageDecodeTaskImpl(ImageDecodeController* controller,
            const SkImage* image,
            int layer_id,
            uint64_t source_prepare_tiles_id)
            : controller_(controller)
            , image_(skia::SharePtr(image))
            , layer_id_(layer_id)
            , source_prepare_tiles_id_(source_prepare_tiles_id)
        {
        }

        // Overridden from Task:
        void RunOnWorkerThread() override
        {
            TRACE_EVENT1("cc", "ImageDecodeTaskImpl::RunOnWorkerThread",
                "source_prepare_tiles_id", source_prepare_tiles_id_);
            devtools_instrumentation::ScopedImageDecodeTask image_decode_task(
                image_.get());
            controller_->DecodeImage(image_.get());

            // Release the reference after decoding image to ensure that it is not kept
            // alive unless needed.
            image_.clear();
        }

        // Overridden from TileTask:
        void ScheduleOnOriginThread(TileTaskClient* client) override { }
        void CompleteOnOriginThread(TileTaskClient* client) override
        {
            controller_->OnImageDecodeTaskCompleted(layer_id_, image_.get(),
                !HasFinishedRunning());
        }

    protected:
        ~ImageDecodeTaskImpl() override { }

    private:
        ImageDecodeController* controller_;
        skia::RefPtr<const SkImage> image_;
        int layer_id_;
        uint64_t source_prepare_tiles_id_;

        DISALLOW_COPY_AND_ASSIGN(ImageDecodeTaskImpl);
    };

} // namespace

ImageDecodeController::ImageDecodeController() { }

ImageDecodeController::~ImageDecodeController() { }

scoped_refptr<ImageDecodeTask> ImageDecodeController::GetTaskForImage(
    const PositionImage& image,
    int layer_id,
    uint64_t prepare_tiles_id)
{
    uint32_t generation_id = image.image->uniqueID();
    scoped_refptr<ImageDecodeTask>& decode_task = image_decode_tasks_[layer_id][generation_id];
    if (!decode_task)
        decode_task = CreateTaskForImage(image.image, layer_id, prepare_tiles_id);
    return decode_task;
}

scoped_refptr<ImageDecodeTask> ImageDecodeController::CreateTaskForImage(
    const SkImage* image,
    int layer_id,
    uint64_t prepare_tiles_id)
{
    return make_scoped_refptr(
        new ImageDecodeTaskImpl(this, image, layer_id, prepare_tiles_id));
}

void ImageDecodeController::DecodeImage(const SkImage* image)
{
    //image->preroll();
    DebugBreak();
}

void ImageDecodeController::AddLayerUsedCount(int layer_id)
{
    ++used_layer_counts_[layer_id];
}

void ImageDecodeController::SubtractLayerUsedCount(int layer_id)
{
    if (--used_layer_counts_[layer_id])
        return;

    // Clean up decode tasks once a layer is no longer used.
    used_layer_counts_.erase(layer_id);
    image_decode_tasks_.erase(layer_id);
}

void ImageDecodeController::OnImageDecodeTaskCompleted(int layer_id,
    const SkImage* image,
    bool was_canceled)
{
    // If the task has successfully finished, then keep the task until the layer
    // is no longer in use. This ensures that we only decode a image once.
    // TODO(vmpstr): Remove this when decode lifetime is controlled by cc.
    if (!was_canceled)
        return;

    // Otherwise, we have to clean up the task so that a new one can be created if
    // we need to decode the image again.
    LayerImageTaskMap::iterator layer_it = image_decode_tasks_.find(layer_id);
    if (layer_it == image_decode_tasks_.end())
        return;

    ImageTaskMap& image_tasks = layer_it->second;
    ImageTaskMap::iterator task_it = image_tasks.find(image->uniqueID());
    if (task_it == image_tasks.end())
        return;
    image_tasks.erase(task_it);
}

} // namespace cc
