// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/image/image.h"

#include <algorithm>
#include <set>
#include <utility>

#include "base/logging.h"
#include "base/macros.h"
#include "build/build_config.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "ui/gfx/geometry/size.h"
#include "ui/gfx/image/image_png_rep.h"
#include "ui/gfx/image/image_skia.h"
#include "ui/gfx/image/image_skia_source.h"

#if !defined(OS_IOS)
#include "ui/gfx/codec/png_codec.h"
#endif

#if defined(OS_IOS)
#include "base/mac/foundation_util.h"
#include "ui/gfx/image/image_skia_util_ios.h"
#elif defined(OS_MACOSX)
#include "base/mac/foundation_util.h"
#include "base/mac/mac_util.h"
#include "ui/gfx/image/image_skia_util_mac.h"
#endif

namespace gfx {

namespace internal {

#if defined(OS_IOS)
    scoped_refptr<base::RefCountedMemory> Get1xPNGBytesFromUIImage(
        UIImage* uiimage);
    // Caller takes ownership of the returned UIImage.
    UIImage* CreateUIImageFromPNG(
        const std::vector<ImagePNGRep>& image_png_reps);
    gfx::Size UIImageSize(UIImage* image);
#elif defined(OS_MACOSX)
    scoped_refptr<base::RefCountedMemory> Get1xPNGBytesFromNSImage(
        NSImage* nsimage);
    // Caller takes ownership of the returned NSImage.
    NSImage* NSImageFromPNG(const std::vector<ImagePNGRep>& image_png_reps,
        CGColorSpaceRef color_space);
    gfx::Size NSImageSize(NSImage* image);
#endif // defined(OS_MACOSX)

#if defined(OS_IOS)
    ImageSkia* ImageSkiaFromPNG(
        const std::vector<ImagePNGRep>& image_png_reps);
    scoped_refptr<base::RefCountedMemory> Get1xPNGBytesFromImageSkia(
        const ImageSkia* skia);
#else
    // Returns a 16x16 red image to visually show error in decoding PNG.
    // Caller takes ownership of returned ImageSkia.
    ImageSkia* GetErrorImageSkia()
    {
        SkBitmap bitmap;
        bitmap.allocN32Pixels(16, 16);
        bitmap.eraseARGB(0xff, 0xff, 0, 0);
        return new ImageSkia(ImageSkiaRep(bitmap, 1.0f));
    }

    class PNGImageSource : public ImageSkiaSource {
    public:
        PNGImageSource() { }
        ~PNGImageSource() override { }

        ImageSkiaRep GetImageForScale(float scale) override
        {
            if (image_skia_reps_.empty())
                return ImageSkiaRep();

            const ImageSkiaRep* rep = NULL;
            // gfx::ImageSkia passes one of the resource scale factors. The source
            // should return:
            // 1) The ImageSkiaRep with the highest scale if all available
            // scales are smaller than |scale|.
            // 2) The ImageSkiaRep with the smallest one that is larger than |scale|.
            for (ImageSkiaRepSet::const_iterator iter = image_skia_reps_.begin();
                 iter != image_skia_reps_.end(); ++iter) {
                if ((*iter).scale() == scale)
                    return (*iter);
                if (!rep || rep->scale() < (*iter).scale())
                    rep = &(*iter);
                if (rep->scale() >= scale)
                    break;
            }
            return rep ? *rep : ImageSkiaRep();
        }

        const gfx::Size size() const
        {
            return size_;
        }

        bool AddPNGData(const ImagePNGRep& png_rep)
        {
            const gfx::ImageSkiaRep rep = ToImageSkiaRep(png_rep);
            if (rep.is_null())
                return false;
            if (size_.IsEmpty())
                size_ = gfx::Size(rep.GetWidth(), rep.GetHeight());
            image_skia_reps_.insert(rep);
            return true;
        }

        static ImageSkiaRep ToImageSkiaRep(const ImagePNGRep& png_rep)
        {
            scoped_refptr<base::RefCountedMemory> raw_data = png_rep.raw_data;
            CHECK(raw_data.get());
            SkBitmap bitmap;
            if (!PNGCodec::Decode(raw_data->front(), raw_data->size(),
                    &bitmap)) {
                LOG(ERROR) << "Unable to decode PNG for " << png_rep.scale << ".";
                return ImageSkiaRep();
            }
            return ImageSkiaRep(bitmap, png_rep.scale);
        }

    private:
        struct Compare {
            bool operator()(const ImageSkiaRep& rep1, const ImageSkiaRep& rep2)
            {
                return rep1.scale() < rep2.scale();
            }
        };

        typedef std::set<ImageSkiaRep, Compare> ImageSkiaRepSet;
        ImageSkiaRepSet image_skia_reps_;
        gfx::Size size_;

        DISALLOW_COPY_AND_ASSIGN(PNGImageSource);
    };

    ImageSkia* ImageSkiaFromPNG(
        const std::vector<ImagePNGRep>& image_png_reps)
    {
        if (image_png_reps.empty())
            return GetErrorImageSkia();
        scoped_ptr<PNGImageSource> image_source(new PNGImageSource);

        for (size_t i = 0; i < image_png_reps.size(); ++i) {
            if (!image_source->AddPNGData(image_png_reps[i]))
                return GetErrorImageSkia();
        }
        const gfx::Size& size = image_source->size();
        DCHECK(!size.IsEmpty());
        if (size.IsEmpty())
            return GetErrorImageSkia();
        return new ImageSkia(image_source.release(), size);
    }

    scoped_refptr<base::RefCountedMemory> Get1xPNGBytesFromImageSkia(
        const ImageSkia* image_skia)
    {
        ImageSkiaRep image_skia_rep = image_skia->GetRepresentation(1.0f);

        scoped_refptr<base::RefCountedBytes> png_bytes(new base::RefCountedBytes());
        if (image_skia_rep.scale() != 1.0f || !PNGCodec::EncodeBGRASkBitmap(image_skia_rep.sk_bitmap(), false, &png_bytes->data())) {
            return NULL;
        }
        return png_bytes;
    }
#endif

    class ImageRepPNG;
    class ImageRepSkia;
    class ImageRepCocoa;
    class ImageRepCocoaTouch;

    // An ImageRep is the object that holds the backing memory for an Image. Each
    // RepresentationType has an ImageRep subclass that is responsible for freeing
    // the memory that the ImageRep holds. When an ImageRep is created, it expects
    // to take ownership of the image, without having to retain it or increase its
    // reference count.
    class ImageRep {
    public:
        explicit ImageRep(Image::RepresentationType rep)
            : type_(rep)
        {
        }

        // Deletes the associated pixels of an ImageRep.
        virtual ~ImageRep() { }

        // Cast helpers ("fake RTTI").
        ImageRepPNG* AsImageRepPNG()
        {
            CHECK_EQ(type_, Image::kImageRepPNG);
            return reinterpret_cast<ImageRepPNG*>(this);
        }

        ImageRepSkia* AsImageRepSkia()
        {
            CHECK_EQ(type_, Image::kImageRepSkia);
            return reinterpret_cast<ImageRepSkia*>(this);
        }

#if defined(OS_IOS)
        ImageRepCocoaTouch* AsImageRepCocoaTouch()
        {
            CHECK_EQ(type_, Image::kImageRepCocoaTouch);
            return reinterpret_cast<ImageRepCocoaTouch*>(this);
        }
#elif defined(OS_MACOSX)
        ImageRepCocoa* AsImageRepCocoa()
        {
            CHECK_EQ(type_, Image::kImageRepCocoa);
            return reinterpret_cast<ImageRepCocoa*>(this);
        }
#endif

        Image::RepresentationType type() const
        {
            return type_;
        }

        virtual int Width() const = 0;
        virtual int Height() const = 0;
        virtual gfx::Size Size() const = 0;

    private:
        Image::RepresentationType type_;
    };

    class ImageRepPNG : public ImageRep {
    public:
        ImageRepPNG()
            : ImageRep(Image::kImageRepPNG)
        {
        }

        ImageRepPNG(const std::vector<ImagePNGRep>& image_png_reps)
            : ImageRep(Image::kImageRepPNG)
            , image_png_reps_(image_png_reps)
        {
        }

        ~ImageRepPNG() override { }

        int Width() const override { return Size().width(); }

        int Height() const override { return Size().height(); }

        gfx::Size Size() const override
        {
            // Read the PNG data to get the image size, caching it.
            if (!size_cache_) {
                for (std::vector<ImagePNGRep>::const_iterator it = image_reps().begin();
                     it != image_reps().end(); ++it) {
                    if (it->scale == 1.0f) {
                        size_cache_.reset(new gfx::Size(it->Size()));
                        return *size_cache_;
                    }
                }
                size_cache_.reset(new gfx::Size);
            }

            return *size_cache_;
        }

        const std::vector<ImagePNGRep>& image_reps() const { return image_png_reps_; }

    private:
        std::vector<ImagePNGRep> image_png_reps_;

        // Cached to avoid having to parse the raw data multiple times.
        mutable scoped_ptr<gfx::Size> size_cache_;

        DISALLOW_COPY_AND_ASSIGN(ImageRepPNG);
    };

    class ImageRepSkia : public ImageRep {
    public:
        // Takes ownership of |image|.
        explicit ImageRepSkia(ImageSkia* image)
            : ImageRep(Image::kImageRepSkia)
            , image_(image)
        {
        }

        ~ImageRepSkia() override { }

        int Width() const override { return image_->width(); }

        int Height() const override { return image_->height(); }

        gfx::Size Size() const override { return image_->size(); }

        ImageSkia* image() { return image_.get(); }

    private:
        scoped_ptr<ImageSkia> image_;

        DISALLOW_COPY_AND_ASSIGN(ImageRepSkia);
    };

#if defined(OS_IOS)
    class ImageRepCocoaTouch : public ImageRep {
    public:
        explicit ImageRepCocoaTouch(UIImage* image)
            : ImageRep(Image::kImageRepCocoaTouch)
            , image_(image)
        {
            CHECK(image);
        }

        ~ImageRepCocoaTouch() override
        {
            base::mac::NSObjectRelease(image_);
            image_ = nil;
        }

        int Width() const override { return Size().width(); }

        int Height() const override { return Size().height(); }

        gfx::Size Size() const override { return internal::UIImageSize(image_); }

        UIImage* image() const { return image_; }

    private:
        UIImage* image_;

        DISALLOW_COPY_AND_ASSIGN(ImageRepCocoaTouch);
    };
#elif defined(OS_MACOSX)
    class ImageRepCocoa : public ImageRep {
    public:
        explicit ImageRepCocoa(NSImage* image)
            : ImageRep(Image::kImageRepCocoa)
            , image_(image)
        {
            CHECK(image);
        }

        ~ImageRepCocoa() override
        {
            base::mac::NSObjectRelease(image_);
            image_ = nil;
        }

        int Width() const override { return Size().width(); }

        int Height() const override { return Size().height(); }

        gfx::Size Size() const override { return internal::NSImageSize(image_); }

        NSImage* image() const { return image_; }

    private:
        NSImage* image_;

        DISALLOW_COPY_AND_ASSIGN(ImageRepCocoa);
    };
#endif // defined(OS_MACOSX)

    // The Storage class acts similarly to the pixels in a SkBitmap: the Image
    // class holds a refptr instance of Storage, which in turn holds all the
    // ImageReps. This way, the Image can be cheaply copied.
    class ImageStorage : public base::RefCounted<ImageStorage> {
    public:
        ImageStorage(Image::RepresentationType default_type)
            : default_representation_type_(default_type)
#if defined(OS_MACOSX) && !defined(OS_IOS)
            , default_representation_color_space_(
                  base::mac::GetGenericRGBColorSpace())
#endif // defined(OS_MACOSX) && !defined(OS_IOS)
        {
        }

        Image::RepresentationType default_representation_type()
        {
            return default_representation_type_;
        }
        Image::RepresentationMap& representations() { return representations_; }

#if defined(OS_MACOSX) && !defined(OS_IOS)
        void set_default_representation_color_space(CGColorSpaceRef color_space)
        {
            default_representation_color_space_ = color_space;
        }
        CGColorSpaceRef default_representation_color_space()
        {
            return default_representation_color_space_;
        }
#endif // defined(OS_MACOSX) && !defined(OS_IOS)

    private:
        friend class base::RefCounted<ImageStorage>;

        ~ImageStorage() { }

        // The type of image that was passed to the constructor. This key will always
        // exist in the |representations_| map.
        Image::RepresentationType default_representation_type_;

#if defined(OS_MACOSX) && !defined(OS_IOS)
        // The default representation's colorspace. This is used for converting to
        // NSImage. This field exists to compensate for PNGCodec not writing or
        // reading colorspace ancillary chunks. (sRGB, iCCP).
        // Not owned.
        CGColorSpaceRef default_representation_color_space_;
#endif // defined(OS_MACOSX) && !defined(OS_IOS)

        // All the representations of an Image. Size will always be at least one, with
        // more for any converted representations.
        Image::RepresentationMap representations_;

        DISALLOW_COPY_AND_ASSIGN(ImageStorage);
    };

} // namespace internal

Image::Image()
{
    // |storage_| is NULL for empty Images.
}

Image::Image(const std::vector<ImagePNGRep>& image_reps)
{
    // Do not store obviously invalid ImagePNGReps.
    std::vector<ImagePNGRep> filtered;
    for (size_t i = 0; i < image_reps.size(); ++i) {
        if (image_reps[i].raw_data.get() && image_reps[i].raw_data->size())
            filtered.push_back(image_reps[i]);
    }

    if (filtered.empty())
        return;

    storage_ = new internal::ImageStorage(Image::kImageRepPNG);
    AddRepresentation(make_scoped_ptr(new internal::ImageRepPNG(filtered)));
}

Image::Image(const ImageSkia& image)
{
    if (!image.isNull()) {
        storage_ = new internal::ImageStorage(Image::kImageRepSkia);
        AddRepresentation(
            make_scoped_ptr(new internal::ImageRepSkia(new ImageSkia(image))));
    }
}

#if defined(OS_IOS)
Image::Image(UIImage* image)
    : storage_(new internal::ImageStorage(Image::kImageRepCocoaTouch))
{
    if (image)
        AddRepresentation(make_scoped_ptr(new internal::ImageRepCocoaTouch(image)));
}
#elif defined(OS_MACOSX)
Image::Image(NSImage* image)
{
    if (image) {
        storage_ = new internal::ImageStorage(Image::kImageRepCocoa);
        AddRepresentation(make_scoped_ptr(new internal::ImageRepCocoa(image)));
    }
}
#endif

Image::Image(const Image& other)
    : storage_(other.storage_)
{
}

Image& Image::operator=(const Image& other)
{
    storage_ = other.storage_;
    return *this;
}

Image::~Image()
{
}

// static
Image Image::CreateFrom1xBitmap(const SkBitmap& bitmap)
{
    return Image(ImageSkia::CreateFrom1xBitmap(bitmap));
}

// static
Image Image::CreateFrom1xPNGBytes(const unsigned char* input,
    size_t input_size)
{
    if (input_size == 0u)
        return Image();

    scoped_refptr<base::RefCountedBytes> raw_data(new base::RefCountedBytes());
    raw_data->data().assign(input, input + input_size);

    return CreateFrom1xPNGBytes(raw_data);
}

Image Image::CreateFrom1xPNGBytes(
    const scoped_refptr<base::RefCountedMemory>& input)
{
    if (!input.get() || input->size() == 0u)
        return Image();

    std::vector<ImagePNGRep> image_reps;
    image_reps.push_back(ImagePNGRep(input, 1.0f));
    return Image(image_reps);
}

const SkBitmap* Image::ToSkBitmap() const
{
    // Possibly create and cache an intermediate ImageRepSkia.
    return ToImageSkia()->bitmap();
}

const ImageSkia* Image::ToImageSkia() const
{
    internal::ImageRep* rep = GetRepresentation(kImageRepSkia, false);
    if (!rep) {
        scoped_ptr<internal::ImageRep> scoped_rep;
        switch (DefaultRepresentationType()) {
        case kImageRepPNG: {
            internal::ImageRepPNG* png_rep = GetRepresentation(kImageRepPNG, true)->AsImageRepPNG();
            scoped_rep.reset(new internal::ImageRepSkia(
                internal::ImageSkiaFromPNG(png_rep->image_reps())));
            break;
        }
#if defined(OS_IOS)
        case kImageRepCocoaTouch: {
            internal::ImageRepCocoaTouch* native_rep = GetRepresentation(kImageRepCocoaTouch, true)
                                                           ->AsImageRepCocoaTouch();
            scoped_rep.reset(new internal::ImageRepSkia(
                new ImageSkia(ImageSkiaFromUIImage(native_rep->image()))));
            break;
        }
#elif defined(OS_MACOSX)
        case kImageRepCocoa: {
            internal::ImageRepCocoa* native_rep = GetRepresentation(kImageRepCocoa, true)->AsImageRepCocoa();
            scoped_rep.reset(new internal::ImageRepSkia(
                new ImageSkia(ImageSkiaFromNSImage(native_rep->image()))));
            break;
        }
#endif
        default:
            NOTREACHED();
        }
        CHECK(scoped_rep);
        rep = scoped_rep.get();
        AddRepresentation(std::move(scoped_rep));
    }
    return rep->AsImageRepSkia()->image();
}

#if defined(OS_IOS)
UIImage* Image::ToUIImage() const
{
    internal::ImageRep* rep = GetRepresentation(kImageRepCocoaTouch, false);
    if (!rep) {
        scoped_ptr<internal::ImageRep> scoped_rep;
        switch (DefaultRepresentationType()) {
        case kImageRepPNG: {
            internal::ImageRepPNG* png_rep = GetRepresentation(kImageRepPNG, true)->AsImageRepPNG();
            scoped_rep.reset(new internal::ImageRepCocoaTouch(
                internal::CreateUIImageFromPNG(png_rep->image_reps())));
            break;
        }
        case kImageRepSkia: {
            internal::ImageRepSkia* skia_rep = GetRepresentation(kImageRepSkia, true)->AsImageRepSkia();
            UIImage* image = UIImageFromImageSkia(*skia_rep->image());
            base::mac::NSObjectRetain(image);
            scoped_rep.reset(new internal::ImageRepCocoaTouch(image));
            break;
        }
        default:
            NOTREACHED();
        }
        CHECK(scoped_rep);
        rep = scoped_rep.get();
        AddRepresentation(std::move(scoped_rep));
    }
    return rep->AsImageRepCocoaTouch()->image();
}
#elif defined(OS_MACOSX)
NSImage* Image::ToNSImage() const
{
    internal::ImageRep* rep = GetRepresentation(kImageRepCocoa, false);
    if (!rep) {
        scoped_ptr<internal::ImageRep> scoped_rep;
        CGColorSpaceRef default_representation_color_space = storage_->default_representation_color_space();

        switch (DefaultRepresentationType()) {
        case kImageRepPNG: {
            internal::ImageRepPNG* png_rep = GetRepresentation(kImageRepPNG, true)->AsImageRepPNG();
            scoped_rep.reset(new internal::ImageRepCocoa(internal::NSImageFromPNG(
                png_rep->image_reps(), default_representation_color_space)));
            break;
        }
        case kImageRepSkia: {
            internal::ImageRepSkia* skia_rep = GetRepresentation(kImageRepSkia, true)->AsImageRepSkia();
            NSImage* image = NSImageFromImageSkiaWithColorSpace(*skia_rep->image(),
                default_representation_color_space);
            base::mac::NSObjectRetain(image);
            scoped_rep.reset(new internal::ImageRepCocoa(image));
            break;
        }
        default:
            NOTREACHED();
        }
        CHECK(scoped_rep);
        rep = scoped_rep.get();
        AddRepresentation(std::move(scoped_rep));
    }
    return rep->AsImageRepCocoa()->image();
}
#endif

scoped_refptr<base::RefCountedMemory> Image::As1xPNGBytes() const
{
    if (IsEmpty())
        return new base::RefCountedBytes();

    internal::ImageRep* rep = GetRepresentation(kImageRepPNG, false);

    if (rep) {
        const std::vector<ImagePNGRep>& image_png_reps = rep->AsImageRepPNG()->image_reps();
        for (size_t i = 0; i < image_png_reps.size(); ++i) {
            if (image_png_reps[i].scale == 1.0f)
                return image_png_reps[i].raw_data;
        }
        return new base::RefCountedBytes();
    }

    scoped_refptr<base::RefCountedMemory> png_bytes(NULL);
    switch (DefaultRepresentationType()) {
#if defined(OS_IOS)
    case kImageRepCocoaTouch: {
        internal::ImageRepCocoaTouch* cocoa_touch_rep = GetRepresentation(kImageRepCocoaTouch, true)
                                                            ->AsImageRepCocoaTouch();
        png_bytes = internal::Get1xPNGBytesFromUIImage(
            cocoa_touch_rep->image());
        break;
    }
#elif defined(OS_MACOSX)
    case kImageRepCocoa: {
        internal::ImageRepCocoa* cocoa_rep = GetRepresentation(kImageRepCocoa, true)->AsImageRepCocoa();
        png_bytes = internal::Get1xPNGBytesFromNSImage(cocoa_rep->image());
        break;
    }
#endif
    case kImageRepSkia: {
        internal::ImageRepSkia* skia_rep = GetRepresentation(kImageRepSkia, true)->AsImageRepSkia();
        png_bytes = internal::Get1xPNGBytesFromImageSkia(skia_rep->image());
        break;
    }
    default:
        NOTREACHED();
    }
    if (!png_bytes.get() || !png_bytes->size()) {
        // Add an ImageRepPNG with no data such that the conversion is not
        // attempted each time we want the PNG bytes.
        AddRepresentation(make_scoped_ptr(new internal::ImageRepPNG()));
        return new base::RefCountedBytes();
    }

    // Do not insert representations for scale factors other than 1x even if
    // they are available because:
    // - Only the 1x PNG bytes can be accessed.
    // - ImageRepPNG is not used as an intermediate type in converting to a
    //   final type eg (converting from ImageRepSkia to ImageRepPNG to get an
    //   ImageRepCocoa).
    std::vector<ImagePNGRep> image_png_reps;
    image_png_reps.push_back(ImagePNGRep(png_bytes, 1.0f));
    AddRepresentation(make_scoped_ptr(new internal::ImageRepPNG(image_png_reps)));
    return png_bytes;
}

SkBitmap Image::AsBitmap() const
{
    return IsEmpty() ? SkBitmap() : *ToSkBitmap();
}

ImageSkia Image::AsImageSkia() const
{
    return IsEmpty() ? ImageSkia() : *ToImageSkia();
}

#if defined(OS_MACOSX) && !defined(OS_IOS)
NSImage* Image::AsNSImage() const
{
    return IsEmpty() ? nil : ToNSImage();
}
#endif

scoped_refptr<base::RefCountedMemory> Image::Copy1xPNGBytes() const
{
    scoped_refptr<base::RefCountedMemory> original = As1xPNGBytes();
    scoped_refptr<base::RefCountedBytes> copy(new base::RefCountedBytes());
    copy->data().assign(original->front(), original->front() + original->size());
    return copy;
}

ImageSkia* Image::CopyImageSkia() const
{
    return new ImageSkia(*ToImageSkia());
}

SkBitmap* Image::CopySkBitmap() const
{
    return new SkBitmap(*ToSkBitmap());
}

#if defined(OS_IOS)
UIImage* Image::CopyUIImage() const
{
    UIImage* image = ToUIImage();
    base::mac::NSObjectRetain(image);
    return image;
}
#elif defined(OS_MACOSX)
NSImage* Image::CopyNSImage() const
{
    NSImage* image = ToNSImage();
    base::mac::NSObjectRetain(image);
    return image;
}
#endif

bool Image::HasRepresentation(RepresentationType type) const
{
    return storage_.get() && storage_->representations().count(type) != 0;
}

size_t Image::RepresentationCount() const
{
    if (!storage_.get())
        return 0;

    return storage_->representations().size();
}

bool Image::IsEmpty() const
{
    return RepresentationCount() == 0;
}

int Image::Width() const
{
    if (IsEmpty())
        return 0;
    return GetRepresentation(DefaultRepresentationType(), true)->Width();
}

int Image::Height() const
{
    if (IsEmpty())
        return 0;
    return GetRepresentation(DefaultRepresentationType(), true)->Height();
}

gfx::Size Image::Size() const
{
    if (IsEmpty())
        return gfx::Size();
    return GetRepresentation(DefaultRepresentationType(), true)->Size();
}

void Image::SwapRepresentations(Image* other)
{
    storage_.swap(other->storage_);
}

#if defined(OS_MACOSX) && !defined(OS_IOS)
void Image::SetSourceColorSpace(CGColorSpaceRef color_space)
{
    if (storage_.get())
        storage_->set_default_representation_color_space(color_space);
}
#endif // defined(OS_MACOSX) && !defined(OS_IOS)

Image::RepresentationType Image::DefaultRepresentationType() const
{
    CHECK(storage_.get());
    return storage_->default_representation_type();
}

internal::ImageRep* Image::GetRepresentation(
    RepresentationType rep_type, bool must_exist) const
{
    CHECK(storage_.get());
    RepresentationMap::const_iterator it = storage_->representations().find(rep_type);
    if (it == storage_->representations().end()) {
        CHECK(!must_exist);
        return NULL;
    }
    return it->second.get();
}

void Image::AddRepresentation(scoped_ptr<internal::ImageRep> rep) const
{
    CHECK(storage_.get());
    RepresentationType type = rep->type();
    storage_->representations().insert(std::make_pair(type, std::move(rep)));
}

} // namespace gfx
