// Copyright 2013 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "cc/playback/picture.h"

#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "base/values.h"
#include "cc/test/fake_content_layer_client.h"
#include "cc/test/skia_common.h"
#include "testing/gtest/include/gtest/gtest.h"
#include "third_party/skia/include/core/SkGraphics.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/skia_util.h"

namespace cc {
namespace {

    TEST(PictureTest, AsBase64String)
    {
        SkGraphics::Init();

        gfx::Rect layer_rect(100, 100);

        gfx::Size tile_grid_size(100, 100);

        FakeContentLayerClient content_layer_client;

        scoped_ptr<base::Value> tmp;

        SkPaint red_paint;
        red_paint.setColor(SkColorSetARGB(255, 255, 0, 0));
        SkPaint green_paint;
        green_paint.setColor(SkColorSetARGB(255, 0, 255, 0));

        // Invalid picture (not a dict).
        tmp.reset(new base::StringValue("abc!@#$%"));
        scoped_refptr<Picture> invalid_picture = Picture::CreateFromValue(tmp.get());
        EXPECT_FALSE(invalid_picture.get());

        // Single full-size rect picture.
        content_layer_client.add_draw_rect(layer_rect, red_paint);

        scoped_refptr<Picture> one_rect_picture = Picture::Create(layer_rect, &content_layer_client, tile_grid_size, false,
            RecordingSource::RECORD_NORMALLY);
        scoped_ptr<base::Value> serialized_one_rect(one_rect_picture->AsValue());

        // Reconstruct the picture.
        scoped_refptr<Picture> one_rect_picture_check = Picture::CreateFromValue(serialized_one_rect.get());
        EXPECT_TRUE(one_rect_picture_check);

        // Check for equivalence.
        unsigned char one_rect_buffer[4 * 100 * 100] = { 0 };
        DrawPicture(one_rect_buffer, layer_rect, one_rect_picture);
        unsigned char one_rect_buffer_check[4 * 100 * 100] = { 0 };
        DrawPicture(one_rect_buffer_check, layer_rect, one_rect_picture_check);

        EXPECT_EQ(one_rect_picture->LayerRect(), one_rect_picture_check->LayerRect());
        EXPECT_EQ(0, memcmp(one_rect_buffer, one_rect_buffer_check, 4 * 100 * 100));

        // Two rect picture.
        content_layer_client.add_draw_rect(gfx::Rect(25, 25, 50, 50), green_paint);

        scoped_refptr<Picture> two_rect_picture = Picture::Create(layer_rect, &content_layer_client, tile_grid_size, false,
            RecordingSource::RECORD_NORMALLY);

        scoped_ptr<base::Value> serialized_two_rect(two_rect_picture->AsValue());

        // Reconstruct the picture.
        scoped_refptr<Picture> two_rect_picture_check = Picture::CreateFromValue(serialized_two_rect.get());
        EXPECT_TRUE(two_rect_picture_check);

        // Check for equivalence.
        unsigned char two_rect_buffer[4 * 100 * 100] = { 0 };
        DrawPicture(two_rect_buffer, layer_rect, two_rect_picture);
        unsigned char two_rect_buffer_check[4 * 100 * 100] = { 0 };
        DrawPicture(two_rect_buffer_check, layer_rect, two_rect_picture_check);

        EXPECT_EQ(two_rect_picture->LayerRect(), two_rect_picture_check->LayerRect());
        EXPECT_EQ(0, memcmp(two_rect_buffer, two_rect_buffer_check, 4 * 100 * 100));
    }

    TEST(PictureTest, CreateFromSkpValue)
    {
        SkGraphics::Init();

        gfx::Rect layer_rect(100, 200);

        gfx::Size tile_grid_size(100, 200);

        FakeContentLayerClient content_layer_client;

        scoped_ptr<base::Value> tmp;

        SkPaint red_paint;
        red_paint.setColor(SkColorSetARGB(255, 255, 0, 0));
        SkPaint green_paint;
        green_paint.setColor(SkColorSetARGB(255, 0, 255, 0));

        // Invalid picture (not a dict).
        tmp.reset(new base::StringValue("abc!@#$%"));
        scoped_refptr<Picture> invalid_picture = Picture::CreateFromSkpValue(tmp.get());
        EXPECT_TRUE(!invalid_picture.get());

        // Single full-size rect picture.
        content_layer_client.add_draw_rect(layer_rect, red_paint);
        scoped_refptr<Picture> one_rect_picture = Picture::Create(layer_rect, &content_layer_client, tile_grid_size, false,
            RecordingSource::RECORD_NORMALLY);
        scoped_ptr<base::Value> serialized_one_rect(
            one_rect_picture->AsValue());

        const base::DictionaryValue* value = NULL;
        EXPECT_TRUE(serialized_one_rect->GetAsDictionary(&value));

        // Decode the picture from base64.
        const base::Value* skp_value;
        EXPECT_TRUE(value->Get("skp64", &skp_value));

        // Reconstruct the picture.
        scoped_refptr<Picture> one_rect_picture_check = Picture::CreateFromSkpValue(skp_value);
        EXPECT_TRUE(one_rect_picture_check);

        EXPECT_EQ(100, one_rect_picture_check->LayerRect().width());
        EXPECT_EQ(200, one_rect_picture_check->LayerRect().height());
    }

    TEST(PictureTest, RecordingModes)
    {
        SkGraphics::Init();

        gfx::Rect layer_rect(100, 200);

        gfx::Size tile_grid_size(100, 200);

        FakeContentLayerClient content_layer_client;
        EXPECT_EQ(NULL, content_layer_client.last_canvas());

        scoped_refptr<Picture> picture = Picture::Create(layer_rect, &content_layer_client, tile_grid_size, false,
            RecordingSource::RECORD_NORMALLY);
        EXPECT_TRUE(content_layer_client.last_canvas() != NULL);
        EXPECT_EQ(ContentLayerClient::PAINTING_BEHAVIOR_NORMAL,
            content_layer_client.last_painting_control());
        EXPECT_TRUE(picture.get());

        picture = Picture::Create(layer_rect, &content_layer_client, tile_grid_size,
            false, RecordingSource::RECORD_WITH_SK_NULL_CANVAS);
        EXPECT_TRUE(content_layer_client.last_canvas() != NULL);
        EXPECT_EQ(ContentLayerClient::PAINTING_BEHAVIOR_NORMAL,
            content_layer_client.last_painting_control());
        EXPECT_TRUE(picture.get());

        picture = Picture::Create(layer_rect, &content_layer_client, tile_grid_size, false,
            RecordingSource::RECORD_WITH_PAINTING_DISABLED);
        EXPECT_TRUE(content_layer_client.last_canvas() != NULL);
        EXPECT_EQ(ContentLayerClient::DISPLAY_LIST_PAINTING_DISABLED,
            content_layer_client.last_painting_control());
        EXPECT_TRUE(picture.get());

        picture = Picture::Create(layer_rect, &content_layer_client, tile_grid_size, false,
            RecordingSource::RECORD_WITH_CACHING_DISABLED);
        EXPECT_TRUE(content_layer_client.last_canvas() != NULL);
        EXPECT_EQ(ContentLayerClient::DISPLAY_LIST_CACHING_DISABLED,
            content_layer_client.last_painting_control());
        EXPECT_TRUE(picture.get());

        // RECORD_WITH_CONSTRUCTION_DISABLED is not supported for Picture.

        EXPECT_EQ(5, RecordingSource::RECORDING_MODE_COUNT);
    }

} // namespace
} // namespace cc
