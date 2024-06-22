// Copyright (c) 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "config.h"

#include "MediaFeatureNames.h"

#include "wtf/StdLibExtras.h"

// Generated from:
// - css/MediaFeatureNames.in

namespace blink {
namespace MediaFeatureNames {

using namespace WTF;

const int kNameCount = 42;

void* NamesStorage[kNameCount * ((sizeof(AtomicString) + sizeof(void *) - 1) / sizeof(void *))];

const AtomicString& devicePixelRatioMediaFeature = reinterpret_cast<AtomicString*>(&NamesStorage)[0];
const AtomicString& maxDevicePixelRatioMediaFeature = reinterpret_cast<AtomicString*>(&NamesStorage)[1];
const AtomicString& minDevicePixelRatioMediaFeature = reinterpret_cast<AtomicString*>(&NamesStorage)[2];
const AtomicString& transform3dMediaFeature = reinterpret_cast<AtomicString*>(&NamesStorage)[3];
const AtomicString& anyHoverMediaFeature = reinterpret_cast<AtomicString*>(&NamesStorage)[4];
const AtomicString& anyPointerMediaFeature = reinterpret_cast<AtomicString*>(&NamesStorage)[5];
const AtomicString& aspectRatioMediaFeature = reinterpret_cast<AtomicString*>(&NamesStorage)[6];
const AtomicString& colorMediaFeature = reinterpret_cast<AtomicString*>(&NamesStorage)[7];
const AtomicString& colorIndexMediaFeature = reinterpret_cast<AtomicString*>(&NamesStorage)[8];
const AtomicString& deviceAspectRatioMediaFeature = reinterpret_cast<AtomicString*>(&NamesStorage)[9];
const AtomicString& deviceHeightMediaFeature = reinterpret_cast<AtomicString*>(&NamesStorage)[10];
const AtomicString& deviceWidthMediaFeature = reinterpret_cast<AtomicString*>(&NamesStorage)[11];
const AtomicString& displayModeMediaFeature = reinterpret_cast<AtomicString*>(&NamesStorage)[12];
const AtomicString& gridMediaFeature = reinterpret_cast<AtomicString*>(&NamesStorage)[13];
const AtomicString& heightMediaFeature = reinterpret_cast<AtomicString*>(&NamesStorage)[14];
const AtomicString& hoverMediaFeature = reinterpret_cast<AtomicString*>(&NamesStorage)[15];
const AtomicString& maxAspectRatioMediaFeature = reinterpret_cast<AtomicString*>(&NamesStorage)[16];
const AtomicString& maxColorMediaFeature = reinterpret_cast<AtomicString*>(&NamesStorage)[17];
const AtomicString& maxColorIndexMediaFeature = reinterpret_cast<AtomicString*>(&NamesStorage)[18];
const AtomicString& maxDeviceAspectRatioMediaFeature = reinterpret_cast<AtomicString*>(&NamesStorage)[19];
const AtomicString& maxDeviceHeightMediaFeature = reinterpret_cast<AtomicString*>(&NamesStorage)[20];
const AtomicString& maxDeviceWidthMediaFeature = reinterpret_cast<AtomicString*>(&NamesStorage)[21];
const AtomicString& maxHeightMediaFeature = reinterpret_cast<AtomicString*>(&NamesStorage)[22];
const AtomicString& maxMonochromeMediaFeature = reinterpret_cast<AtomicString*>(&NamesStorage)[23];
const AtomicString& maxResolutionMediaFeature = reinterpret_cast<AtomicString*>(&NamesStorage)[24];
const AtomicString& maxWidthMediaFeature = reinterpret_cast<AtomicString*>(&NamesStorage)[25];
const AtomicString& minAspectRatioMediaFeature = reinterpret_cast<AtomicString*>(&NamesStorage)[26];
const AtomicString& minColorMediaFeature = reinterpret_cast<AtomicString*>(&NamesStorage)[27];
const AtomicString& minColorIndexMediaFeature = reinterpret_cast<AtomicString*>(&NamesStorage)[28];
const AtomicString& minDeviceAspectRatioMediaFeature = reinterpret_cast<AtomicString*>(&NamesStorage)[29];
const AtomicString& minDeviceHeightMediaFeature = reinterpret_cast<AtomicString*>(&NamesStorage)[30];
const AtomicString& minDeviceWidthMediaFeature = reinterpret_cast<AtomicString*>(&NamesStorage)[31];
const AtomicString& minHeightMediaFeature = reinterpret_cast<AtomicString*>(&NamesStorage)[32];
const AtomicString& minMonochromeMediaFeature = reinterpret_cast<AtomicString*>(&NamesStorage)[33];
const AtomicString& minResolutionMediaFeature = reinterpret_cast<AtomicString*>(&NamesStorage)[34];
const AtomicString& minWidthMediaFeature = reinterpret_cast<AtomicString*>(&NamesStorage)[35];
const AtomicString& monochromeMediaFeature = reinterpret_cast<AtomicString*>(&NamesStorage)[36];
const AtomicString& orientationMediaFeature = reinterpret_cast<AtomicString*>(&NamesStorage)[37];
const AtomicString& pointerMediaFeature = reinterpret_cast<AtomicString*>(&NamesStorage)[38];
const AtomicString& resolutionMediaFeature = reinterpret_cast<AtomicString*>(&NamesStorage)[39];
const AtomicString& scanMediaFeature = reinterpret_cast<AtomicString*>(&NamesStorage)[40];
const AtomicString& widthMediaFeature = reinterpret_cast<AtomicString*>(&NamesStorage)[41];

void init()
{
    struct NameEntry {
        const char* name;
        unsigned hash;
        unsigned char length;
    };

    static const NameEntry kNames[] = {
        { "-webkit-device-pixel-ratio", 12532418, 26 },
        { "-webkit-max-device-pixel-ratio", 15699528, 30 },
        { "-webkit-min-device-pixel-ratio", 6230028, 30 },
        { "-webkit-transform-3d", 11719803, 20 },
        { "any-hover", 6083964, 9 },
        { "any-pointer", 5691974, 11 },
        { "aspect-ratio", 10453312, 12 },
        { "color", 2734929, 5 },
        { "color-index", 13972807, 11 },
        { "device-aspect-ratio", 5022071, 19 },
        { "device-height", 9832216, 13 },
        { "device-width", 4551487, 12 },
        { "display-mode", 11513881, 12 },
        { "grid", 1786690, 4 },
        { "height", 6697833, 6 },
        { "hover", 3481422, 5 },
        { "max-aspect-ratio", 12094319, 16 },
        { "max-color", 629821, 9 },
        { "max-color-index", 7430117, 15 },
        { "max-device-aspect-ratio", 12942084, 23 },
        { "max-device-height", 1041693, 17 },
        { "max-device-width", 5334797, 16 },
        { "max-height", 12768354, 10 },
        { "max-monochrome", 15262871, 14 },
        { "max-resolution", 956048, 14 },
        { "max-width", 6572179, 9 },
        { "min-aspect-ratio", 16386843, 16 },
        { "min-color", 12833068, 9 },
        { "min-color-index", 9407342, 15 },
        { "min-device-aspect-ratio", 5241967, 23 },
        { "min-device-height", 9440595, 17 },
        { "min-device-width", 8878672, 16 },
        { "min-height", 12467950, 10 },
        { "min-monochrome", 5118153, 14 },
        { "min-resolution", 3458349, 14 },
        { "min-width", 1786673, 9 },
        { "monochrome", 1202583, 10 },
        { "orientation", 10339552, 11 },
        { "pointer", 8738242, 7 },
        { "resolution", 1320318, 10 },
        { "scan", 2480616, 4 },
        { "width", 12902275, 5 },
    };

    for (size_t i = 0; i < WTF_ARRAY_LENGTH(kNames); i++) {
        StringImpl* stringImpl = StringImpl::createStatic(kNames[i].name, kNames[i].length, kNames[i].hash);
        void* address = reinterpret_cast<AtomicString*>(&NamesStorage) + i;
        new (address) AtomicString(stringImpl);
    }
}

} // MediaFeatureNames
} // namespace blink
