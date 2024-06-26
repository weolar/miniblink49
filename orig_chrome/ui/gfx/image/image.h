// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// An Image wraps an image any flavor, be it platform-native GdkBitmap/NSImage,
// or a SkBitmap. This also provides easy conversion to other image types
// through operator overloading. It will cache the converted representations
// internally to prevent double-conversion.
//
// The lifetime of both the initial representation and any converted ones are
// tied to the lifetime of the Image's internal storage. To allow Images to be
// cheaply passed around by value, the actual image data is stored in a ref-
// counted member. When all Images referencing this storage are deleted, the
// actual representations are deleted, too.
//
// Images can be empty, in which case they have no backing representation.
// Attempting to use an empty Image will result in a crash.

#ifndef UI_GFX_IMAGE_IMAGE_H_
#define UI_GFX_IMAGE_IMAGE_H_

#include <stddef.h>

#include <map>
#include <vector>

#include "base/memory/ref_counted_memory.h"
#include "base/memory/scoped_ptr.h"
#include "build/build_config.h"
#include "ui/gfx/gfx_export.h"
#include "ui/gfx/native_widget_types.h"

#if defined(OS_MACOSX) && !defined(OS_IOS)
typedef struct CGColorSpace* CGColorSpaceRef;
#endif

class SkBitmap;

namespace gfx {
struct ImagePNGRep;
class ImageSkia;
class Size;

namespace internal {
    class ImageRep;
    class ImageStorage;
}

class GFX_EXPORT Image {
public:
    enum RepresentationType {
        kImageRepCocoa,
        kImageRepCocoaTouch,
        kImageRepSkia,
        kImageRepPNG,
    };

    using RepresentationMap = std::map<RepresentationType, scoped_ptr<internal::ImageRep>>;

    // Creates an empty image with no representations.
    Image();

    // Creates a new image by copying the raw PNG-encoded input for use as the
    // default representation.
    explicit Image(const std::vector<ImagePNGRep>& image_reps);

    // Creates a new image by copying the ImageSkia for use as the default
    // representation.
    explicit Image(const ImageSkia& image);

#if defined(OS_IOS)
    // Does not retain |image|; expects to take ownership.
    explicit Image(UIImage* image);
#elif defined(OS_MACOSX)
    // Does not retain |image|; expects to take ownership.
    // A single NSImage object can contain multiple bitmaps so there's no reason
    // to pass a vector of these.
    explicit Image(NSImage* image);
#endif

    // Initializes a new Image by AddRef()ing |other|'s internal storage.
    Image(const Image& other);

    // Copies a reference to |other|'s storage.
    Image& operator=(const Image& other);

    // Deletes the image and, if the only owner of the storage, all of its cached
    // representations.
    ~Image();

    // Creates an image from the passed in 1x bitmap.
    // WARNING: The resulting image will be pixelated when painted on a high
    // density display.
    static Image CreateFrom1xBitmap(const SkBitmap& bitmap);

    // Creates an image from the PNG encoded input.
    // For example (from an std::vector):
    // std::vector<unsigned char> png = ...;
    // gfx::Image image =
    //     Image::CreateFrom1xPNGBytes(&png.front(), png.size());
    static Image CreateFrom1xPNGBytes(const unsigned char* input,
        size_t input_size);

    // Creates an image from the PNG encoded input.
    static Image CreateFrom1xPNGBytes(
        const scoped_refptr<base::RefCountedMemory>& input);

    // Converts the Image to the desired representation and stores it internally.
    // The returned result is a weak pointer owned by and scoped to the life of
    // the Image. Must only be called if IsEmpty() is false.
    const SkBitmap* ToSkBitmap() const;
    const ImageSkia* ToImageSkia() const;
#if defined(OS_IOS)
    UIImage* ToUIImage() const;
#elif defined(OS_MACOSX)
    NSImage* ToNSImage() const;
#endif

    // Returns the raw PNG-encoded data for the bitmap at 1x. If the data is
    // unavailable, either because the image has no data for 1x or because it is
    // empty, an empty RefCountedBytes object is returned. NULL is never
    // returned.
    scoped_refptr<base::RefCountedMemory> As1xPNGBytes() const;

    // Same as ToSkBitmap(), but returns a null SkBitmap if this image is empty.
    SkBitmap AsBitmap() const;

    // Same as ToImageSkia(), but returns an empty ImageSkia if this
    // image is empty.
    ImageSkia AsImageSkia() const;

#if defined(OS_MACOSX) && !defined(OS_IOS)
    // Same as ToSkBitmap(), but returns nil if this image is empty.
    NSImage* AsNSImage() const;
#endif

    // Performs a conversion, like above, but returns a copy of the result rather
    // than a weak pointer. The caller is responsible for deleting the result.
    // Note that the result is only a copy in terms of memory management; the
    // backing pixels are shared amongst all copies (a fact of each of the
    // converted representations, rather than a limitation imposed by Image) and
    // so the result should be considered immutable.
    scoped_refptr<base::RefCountedMemory> Copy1xPNGBytes() const;
    ImageSkia* CopyImageSkia() const;
    SkBitmap* CopySkBitmap() const;
#if defined(OS_IOS)
    UIImage* CopyUIImage() const;
#elif defined(OS_MACOSX)
    NSImage* CopyNSImage() const;
#endif

    // Inspects the representations map to see if the given type exists.
    bool HasRepresentation(RepresentationType type) const;

    // Returns the number of representations.
    size_t RepresentationCount() const;

    // Returns true if this Image has no representations.
    bool IsEmpty() const;

    // Width and height of image in DIP coordinate system.
    int Width() const;
    int Height() const;
    gfx::Size Size() const;

    // Swaps this image's internal representations with |other|.
    void SwapRepresentations(gfx::Image* other);

#if defined(OS_MACOSX) && !defined(OS_IOS)
    // Set the default representation's color space. This is used for converting
    // to NSImage. This is used to compensate for PNGCodec not writing or reading
    // colorspace ancillary chunks. (sRGB, iCCP).
    void SetSourceColorSpace(CGColorSpaceRef color_space);
#endif // defined(OS_MACOSX) && !defined(OS_IOS)

private:
    // Returns the type of the default representation.
    RepresentationType DefaultRepresentationType() const;

    // Returns the ImageRep of the appropriate type or NULL if there is no
    // representation of that type (and must_exist is false).
    internal::ImageRep* GetRepresentation(
        RepresentationType rep_type, bool must_exist) const;

    // Stores a representation into the map.
    void AddRepresentation(scoped_ptr<internal::ImageRep> rep) const;

    // Internal class that holds all the representations. This allows the Image to
    // be cheaply copied.
    scoped_refptr<internal::ImageStorage> storage_;
};

} // namespace gfx

#endif // UI_GFX_IMAGE_IMAGE_H_
