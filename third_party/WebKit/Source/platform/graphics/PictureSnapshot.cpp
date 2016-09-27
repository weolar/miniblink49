/*
 * Copyright (C) 2013 Google Inc. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *
 *     * Redistributions of source code must retain the above copyright
 * notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following disclaimer
 * in the documentation and/or other materials provided with the
 * distribution.
 *     * Neither the name of Google Inc. nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "platform/graphics/PictureSnapshot.h"

#include "platform/graphics/ImageBuffer.h"
#include "platform/graphics/ImageSource.h"
#include "platform/graphics/LoggingCanvas.h"
#include "platform/graphics/ProfilingCanvas.h"
#include "platform/graphics/ReplayingCanvas.h"
#include "platform/image-decoders/ImageDecoder.h"
#include "platform/image-decoders/ImageFrame.h"
#include "platform/image-encoders/skia/PNGImageEncoder.h"
#include "third_party/skia/include/core/SkBitmapDevice.h"
#include "third_party/skia/include/core/SkPictureRecorder.h"
#include "third_party/skia/include/core/SkStream.h"
#include "wtf/HexNumber.h"
#include "wtf/text/Base64.h"
#include "wtf/text/TextEncoding.h"

namespace blink {

PictureSnapshot::PictureSnapshot(PassRefPtr<const SkPicture> picture)
    : m_picture(picture)
{
}

static bool decodeBitmap(const void* data, size_t length, SkBitmap* result)
{
    RefPtr<SharedBuffer> buffer = SharedBuffer::create(static_cast<const char*>(data), length);
    OwnPtr<ImageDecoder> imageDecoder = ImageDecoder::create(*buffer, ImageSource::AlphaPremultiplied, ImageSource::GammaAndColorProfileIgnored);
    if (!imageDecoder)
        return false;
    imageDecoder->setData(buffer.get(), true);
    ImageFrame* frame = imageDecoder->frameBufferAtIndex(0);
    if (!frame)
        return true;
    *result = frame->getSkBitmap();
    return true;
}

PassRefPtr<PictureSnapshot> PictureSnapshot::load(const Vector<RefPtr<TilePictureStream>>& tiles)
{
    ASSERT(!tiles.isEmpty());
    Vector<RefPtr<SkPicture>> pictures;
    pictures.reserveCapacity(tiles.size());
    FloatRect unionRect;
    for (const auto& tileStream : tiles) {
        SkMemoryStream stream(tileStream->data.begin(), tileStream->data.size());
        RefPtr<SkPicture> picture = adoptRef(SkPicture::CreateFromStream(&stream, decodeBitmap));
        if (!picture)
            return nullptr;
        FloatRect cullRect(picture->cullRect());
        cullRect.moveBy(tileStream->layerOffset);
        unionRect.unite(cullRect);
        pictures.append(picture);
    }
    if (tiles.size() == 1)
        return adoptRef(new PictureSnapshot(pictures[0]));
    SkPictureRecorder recorder;
    SkCanvas* canvas = recorder.beginRecording(unionRect.width(), unionRect.height(), 0, 0);
    for (size_t i = 0; i < pictures.size(); ++i) {
        canvas->save();
        canvas->translate(tiles[i]->layerOffset.x() - unionRect.x(), tiles[i]->layerOffset.y() - unionRect.y());
        pictures[i]->playback(canvas, 0);
        canvas->restore();
    }
    return adoptRef(new PictureSnapshot(adoptRef(recorder.endRecordingAsPicture())));
}

bool PictureSnapshot::isEmpty() const
{
    return m_picture->cullRect().isEmpty();
}

PassOwnPtr<Vector<char>> PictureSnapshot::replay(unsigned fromStep, unsigned toStep, double scale) const
{
    const SkIRect bounds = m_picture->cullRect().roundOut();
    SkBitmap bitmap;
    bitmap.allocPixels(SkImageInfo::MakeN32Premul(bounds.width(), bounds.height()));
    bitmap.eraseARGB(0, 0, 0, 0);
    {
        ReplayingCanvas canvas(bitmap, fromStep, toStep);
        canvas.scale(scale, scale);
        canvas.resetStepCount();
        m_picture->playback(&canvas, &canvas);
    }
    OwnPtr<Vector<char>> base64Data = adoptPtr(new Vector<char>());
    Vector<char> encodedImage;
#ifdef MINIBLINK_NOT_IMPLEMENTED
    if (!PNGImageEncoder::encode(bitmap, reinterpret_cast<Vector<unsigned char>*>(&encodedImage)))
        return nullptr;
#endif // MINIBLINK_NOT_IMPLEMENTED
    notImplemented();
    base64Encode(encodedImage, *base64Data);
    return base64Data.release();
}

PassOwnPtr<PictureSnapshot::Timings> PictureSnapshot::profile(unsigned minRepeatCount, double minDuration, const FloatRect* clipRect) const
{
    OwnPtr<PictureSnapshot::Timings> timings = adoptPtr(new PictureSnapshot::Timings());
    timings->reserveCapacity(minRepeatCount);
    const SkIRect bounds = m_picture->cullRect().roundOut();
    SkBitmap bitmap;
    bitmap.allocPixels(SkImageInfo::MakeN32Premul(bounds.width(), bounds.height()));
    bitmap.eraseARGB(0, 0, 0, 0);

    double now = WTF::monotonicallyIncreasingTime();
    double stopTime = now + minDuration;
    for (unsigned step = 0; step < minRepeatCount || now < stopTime; ++step) {
        timings->append(Vector<double>());
        Vector<double>* currentTimings = &timings->last();
        if (timings->size() > 1)
            currentTimings->reserveCapacity(timings->begin()->size());
        ProfilingCanvas canvas(bitmap);
        if (clipRect) {
            canvas.clipRect(SkRect::MakeXYWH(clipRect->x(), clipRect->y(), clipRect->width(), clipRect->height()));
            canvas.resetStepCount();
        }
        canvas.setTimings(currentTimings);
        m_picture->playback(&canvas);
        now = WTF::monotonicallyIncreasingTime();
    }
    return timings.release();
}

PassRefPtr<JSONArray> PictureSnapshot::snapshotCommandLog() const
{
    const SkIRect bounds = m_picture->cullRect().roundOut();
    LoggingCanvas canvas(bounds.width(), bounds.height());
    m_picture->playback(&canvas);
    return canvas.log();
}

} // namespace blink
