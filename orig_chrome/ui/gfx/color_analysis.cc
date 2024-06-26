// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/color_analysis.h"

#include <limits.h>
#include <stdint.h>

#include <algorithm>
#include <limits>
#include <vector>

#include "base/logging.h"
#include "base/memory/scoped_ptr.h"
#include "third_party/skia/include/core/SkBitmap.h"
#include "third_party/skia/include/core/SkUnPreMultiply.h"
#include "ui/gfx/codec/png_codec.h"
#include "ui/gfx/color_utils.h"

namespace color_utils {
namespace {

    // RGBA KMean Constants
    const uint32_t kNumberOfClusters = 4;
    const int kNumberOfIterations = 50;

    const HSL kDefaultLowerHSLBound = { -1, -1, 0.15 };
    const HSL kDefaultUpperHSLBound = { -1, -1, 0.85 };

    // Background Color Modification Constants
    const SkColor kDefaultBgColor = SK_ColorWHITE;

    // Support class to hold information about each cluster of pixel data in
    // the KMean algorithm. While this class does not contain all of the points
    // that exist in the cluster, it keeps track of the aggregate sum so it can
    // compute the new center appropriately.
    class KMeanCluster {
    public:
        KMeanCluster()
        {
            Reset();
        }

        void Reset()
        {
            centroid_[0] = centroid_[1] = centroid_[2] = 0;
            aggregate_[0] = aggregate_[1] = aggregate_[2] = 0;
            counter_ = 0;
            weight_ = 0;
        }

        inline void SetCentroid(uint8_t r, uint8_t g, uint8_t b)
        {
            centroid_[0] = r;
            centroid_[1] = g;
            centroid_[2] = b;
        }

        inline void GetCentroid(uint8_t* r, uint8_t* g, uint8_t* b)
        {
            *r = centroid_[0];
            *g = centroid_[1];
            *b = centroid_[2];
        }

        inline bool IsAtCentroid(uint8_t r, uint8_t g, uint8_t b)
        {
            return r == centroid_[0] && g == centroid_[1] && b == centroid_[2];
        }

        // Recomputes the centroid of the cluster based on the aggregate data. The
        // number of points used to calculate this center is stored for weighting
        // purposes. The aggregate and counter are then cleared to be ready for the
        // next iteration.
        inline void RecomputeCentroid()
        {
            if (counter_ > 0) {
                centroid_[0] = static_cast<uint8_t>(aggregate_[0] / counter_);
                centroid_[1] = static_cast<uint8_t>(aggregate_[1] / counter_);
                centroid_[2] = static_cast<uint8_t>(aggregate_[2] / counter_);

                aggregate_[0] = aggregate_[1] = aggregate_[2] = 0;
                weight_ = counter_;
                counter_ = 0;
            }
        }

        inline void AddPoint(uint8_t r, uint8_t g, uint8_t b)
        {
            aggregate_[0] += r;
            aggregate_[1] += g;
            aggregate_[2] += b;
            ++counter_;
        }

        // Just returns the distance^2. Since we are comparing relative distances
        // there is no need to perform the expensive sqrt() operation.
        inline uint32_t GetDistanceSqr(uint8_t r, uint8_t g, uint8_t b)
        {
            return (r - centroid_[0]) * (r - centroid_[0]) + (g - centroid_[1]) * (g - centroid_[1]) + (b - centroid_[2]) * (b - centroid_[2]);
        }

        // In order to determine if we have hit convergence or not we need to see
        // if the centroid of the cluster has moved. This determines whether or
        // not the centroid is the same as the aggregate sum of points that will be
        // used to generate the next centroid.
        inline bool CompareCentroidWithAggregate()
        {
            if (counter_ == 0)
                return false;

            return aggregate_[0] / counter_ == centroid_[0] && aggregate_[1] / counter_ == centroid_[1] && aggregate_[2] / counter_ == centroid_[2];
        }

        // Returns the previous counter, which is used to determine the weight
        // of the cluster for sorting.
        inline uint32_t GetWeight() const
        {
            return weight_;
        }

        static bool SortKMeanClusterByWeight(const KMeanCluster& a,
            const KMeanCluster& b)
        {
            return a.GetWeight() > b.GetWeight();
        }

    private:
        uint8_t centroid_[3];

        // Holds the sum of all the points that make up this cluster. Used to
        // generate the next centroid as well as to check for convergence.
        uint32_t aggregate_[3];
        uint32_t counter_;

        // The weight of the cluster, determined by how many points were used
        // to generate the previous centroid.
        uint32_t weight_;
    };

    // Un-premultiplies each pixel in |bitmap| into an output |buffer|. Requires
    // approximately 10 microseconds for a 16x16 icon on an Intel Core i5.
    void UnPreMultiply(const SkBitmap& bitmap, uint32_t* buffer, int buffer_size)
    {
        SkAutoLockPixels auto_lock(bitmap);
        uint32_t* in = static_cast<uint32_t*>(bitmap.getPixels());
        uint32_t* out = buffer;
        int pixel_count = std::min(bitmap.width() * bitmap.height(), buffer_size);
        for (int i = 0; i < pixel_count; ++i)
            *out++ = SkUnPreMultiply::PMColorToColor(*in++);
    }

} // namespace

KMeanImageSampler::KMeanImageSampler()
{
}

KMeanImageSampler::~KMeanImageSampler()
{
}

GridSampler::GridSampler()
    : calls_(0)
{
}

GridSampler::~GridSampler()
{
}

int GridSampler::GetSample(int width, int height)
{
    // Hand-drawn bitmaps often have special outlines or feathering at the edges.
    // Start our sampling inset from the top and left edges. For example, a 10x10
    // image with 4 clusters would be sampled like this:
    // ..........
    // .0.4.8....
    // ..........
    // .1.5.9....
    // ..........
    // .2.6......
    // ..........
    // .3.7......
    // ..........
    const int kPadX = 1;
    const int kPadY = 1;
    int x = kPadX + (calls_ / kNumberOfClusters) * ((width - 2 * kPadX) / kNumberOfClusters);
    int y = kPadY + (calls_ % kNumberOfClusters) * ((height - 2 * kPadY) / kNumberOfClusters);
    int index = x + (y * width);
    ++calls_;
    return index % (width * height);
}

SkColor FindClosestColor(const uint8_t* image,
    int width,
    int height,
    SkColor color)
{
    uint8_t in_r = SkColorGetR(color);
    uint8_t in_g = SkColorGetG(color);
    uint8_t in_b = SkColorGetB(color);
    // Search using distance-squared to avoid expensive sqrt() operations.
    int best_distance_squared = std::numeric_limits<int32_t>::max();
    SkColor best_color = color;
    const uint8_t* byte = image;
    for (int i = 0; i < width * height; ++i) {
        uint8_t b = *(byte++);
        uint8_t g = *(byte++);
        uint8_t r = *(byte++);
        uint8_t a = *(byte++);
        // Ignore fully transparent pixels.
        if (a == 0)
            continue;
        int distance_squared = (in_b - b) * (in_b - b) + (in_g - g) * (in_g - g) + (in_r - r) * (in_r - r);
        if (distance_squared < best_distance_squared) {
            best_distance_squared = distance_squared;
            best_color = SkColorSetRGB(r, g, b);
        }
    }
    return best_color;
}

// For a 16x16 icon on an Intel Core i5 this function takes approximately
// 0.5 ms to run.
// TODO(port): This code assumes the CPU architecture is little-endian.
SkColor CalculateKMeanColorOfBuffer(uint8_t* decoded_data,
    int img_width,
    int img_height,
    const HSL& lower_bound,
    const HSL& upper_bound,
    KMeanImageSampler* sampler)
{
    SkColor color = kDefaultBgColor;
    if (img_width > 0 && img_height > 0) {
        std::vector<KMeanCluster> clusters;
        clusters.resize(kNumberOfClusters, KMeanCluster());

        // Pick a starting point for each cluster
        std::vector<KMeanCluster>::iterator cluster = clusters.begin();
        while (cluster != clusters.end()) {
            // Try up to 10 times to find a unique color. If no unique color can be
            // found, destroy this cluster.
            bool color_unique = false;
            for (int i = 0; i < 10; ++i) {
                int pixel_pos = sampler->GetSample(img_width, img_height) % (img_width * img_height);

                uint8_t b = decoded_data[pixel_pos * 4];
                uint8_t g = decoded_data[pixel_pos * 4 + 1];
                uint8_t r = decoded_data[pixel_pos * 4 + 2];
                uint8_t a = decoded_data[pixel_pos * 4 + 3];
                // Skip fully transparent pixels as they usually contain black in their
                // RGB channels but do not contribute to the visual image.
                if (a == 0)
                    continue;

                // Loop through the previous clusters and check to see if we have seen
                // this color before.
                color_unique = true;
                for (std::vector<KMeanCluster>::iterator
                         cluster_check
                     = clusters.begin();
                     cluster_check != cluster; ++cluster_check) {
                    if (cluster_check->IsAtCentroid(r, g, b)) {
                        color_unique = false;
                        break;
                    }
                }

                // If we have a unique color set the center of the cluster to
                // that color.
                if (color_unique) {
                    cluster->SetCentroid(r, g, b);
                    break;
                }
            }

            // If we don't have a unique color erase this cluster.
            if (!color_unique) {
                cluster = clusters.erase(cluster);
            } else {
                // Have to increment the iterator here, otherwise the increment in the
                // for loop will skip a cluster due to the erase if the color wasn't
                // unique.
                ++cluster;
            }
        }

        // If all pixels in the image are transparent we will have no clusters.
        if (clusters.empty())
            return color;

        bool convergence = false;
        for (int iteration = 0;
             iteration < kNumberOfIterations && !convergence;
             ++iteration) {

            // Loop through each pixel so we can place it in the appropriate cluster.
            uint8_t* pixel = decoded_data;
            uint8_t* decoded_data_end = decoded_data + (img_width * img_height * 4);
            while (pixel < decoded_data_end) {
                uint8_t b = *(pixel++);
                uint8_t g = *(pixel++);
                uint8_t r = *(pixel++);
                uint8_t a = *(pixel++);
                // Skip transparent pixels, see above.
                if (a == 0)
                    continue;

                uint32_t distance_sqr_to_closest_cluster = UINT_MAX;
                std::vector<KMeanCluster>::iterator closest_cluster = clusters.begin();

                // Figure out which cluster this color is closest to in RGB space.
                for (std::vector<KMeanCluster>::iterator cluster = clusters.begin();
                     cluster != clusters.end(); ++cluster) {
                    uint32_t distance_sqr = cluster->GetDistanceSqr(r, g, b);

                    if (distance_sqr < distance_sqr_to_closest_cluster) {
                        distance_sqr_to_closest_cluster = distance_sqr;
                        closest_cluster = cluster;
                    }
                }

                closest_cluster->AddPoint(r, g, b);
            }

            // Calculate the new cluster centers and see if we've converged or not.
            convergence = true;
            for (std::vector<KMeanCluster>::iterator cluster = clusters.begin();
                 cluster != clusters.end(); ++cluster) {
                convergence &= cluster->CompareCentroidWithAggregate();

                cluster->RecomputeCentroid();
            }
        }

        // Sort the clusters by population so we can tell what the most popular
        // color is.
        std::sort(clusters.begin(), clusters.end(),
            KMeanCluster::SortKMeanClusterByWeight);

        // Loop through the clusters to figure out which cluster has an appropriate
        // color. Skip any that are too bright/dark and go in order of weight.
        for (std::vector<KMeanCluster>::iterator cluster = clusters.begin();
             cluster != clusters.end(); ++cluster) {
            uint8_t r, g, b;
            cluster->GetCentroid(&r, &g, &b);

            SkColor current_color = SkColorSetARGB(SK_AlphaOPAQUE, r, g, b);
            HSL hsl;
            SkColorToHSL(current_color, &hsl);
            if (IsWithinHSLRange(hsl, lower_bound, upper_bound)) {
                // If we found a valid color just set it and break. We don't want to
                // check the other ones.
                color = current_color;
                break;
            } else if (cluster == clusters.begin()) {
                // We haven't found a valid color, but we are at the first color so
                // set the color anyway to make sure we at least have a value here.
                color = current_color;
            }
        }
    }

    // Find a color that actually appears in the image (the K-mean cluster center
    // will not usually be a color that appears in the image).
    return FindClosestColor(decoded_data, img_width, img_height, color);
}

SkColor CalculateKMeanColorOfPNG(scoped_refptr<base::RefCountedMemory> png,
    const HSL& lower_bound,
    const HSL& upper_bound,
    KMeanImageSampler* sampler)
{
    int img_width = 0;
    int img_height = 0;
    std::vector<uint8_t> decoded_data;
    SkColor color = kDefaultBgColor;

    if (png.get() && png->size() && gfx::PNGCodec::Decode(png->front(), png->size(), gfx::PNGCodec::FORMAT_BGRA, &decoded_data, &img_width, &img_height)) {
        return CalculateKMeanColorOfBuffer(&decoded_data[0],
            img_width,
            img_height,
            lower_bound,
            upper_bound,
            sampler);
    }
    return color;
}

SkColor CalculateKMeanColorOfPNG(scoped_refptr<base::RefCountedMemory> png)
{
    GridSampler sampler;
    return CalculateKMeanColorOfPNG(
        png, kDefaultLowerHSLBound, kDefaultUpperHSLBound, &sampler);
}

SkColor CalculateKMeanColorOfBitmap(const SkBitmap& bitmap,
    const HSL& lower_bound,
    const HSL& upper_bound,
    KMeanImageSampler* sampler)
{
    // SkBitmap uses pre-multiplied alpha but the KMean clustering function
    // above uses non-pre-multiplied alpha. Transform the bitmap before we
    // analyze it because the function reads each pixel multiple times.
    int pixel_count = bitmap.width() * bitmap.height();
    scoped_ptr<uint32_t[]> image(new uint32_t[pixel_count]);
    UnPreMultiply(bitmap, image.get(), pixel_count);

    return CalculateKMeanColorOfBuffer(reinterpret_cast<uint8_t*>(image.get()),
        bitmap.width(),
        bitmap.height(),
        lower_bound,
        upper_bound,
        sampler);
}

SkColor CalculateKMeanColorOfBitmap(const SkBitmap& bitmap)
{
    GridSampler sampler;
    return CalculateKMeanColorOfBitmap(
        bitmap, kDefaultLowerHSLBound, kDefaultUpperHSLBound, &sampler);
}

gfx::Matrix3F ComputeColorCovariance(const SkBitmap& bitmap)
{
    // First need basic stats to normalize each channel separately.
    SkAutoLockPixels bitmap_lock(bitmap);
    gfx::Matrix3F covariance = gfx::Matrix3F::Zeros();
    if (!bitmap.getPixels())
        return covariance;

    // Assume ARGB_8888 format.
    DCHECK(bitmap.colorType() == kN32_SkColorType);

    int64_t r_sum = 0;
    int64_t g_sum = 0;
    int64_t b_sum = 0;
    int64_t rr_sum = 0;
    int64_t gg_sum = 0;
    int64_t bb_sum = 0;
    int64_t rg_sum = 0;
    int64_t rb_sum = 0;
    int64_t gb_sum = 0;

    for (int y = 0; y < bitmap.height(); ++y) {
        SkPMColor* current_color = static_cast<uint32_t*>(bitmap.getAddr32(0, y));
        for (int x = 0; x < bitmap.width(); ++x, ++current_color) {
            SkColor c = SkUnPreMultiply::PMColorToColor(*current_color);
            SkColor r = SkColorGetR(c);
            SkColor g = SkColorGetG(c);
            SkColor b = SkColorGetB(c);

            r_sum += r;
            g_sum += g;
            b_sum += b;
            rr_sum += r * r;
            gg_sum += g * g;
            bb_sum += b * b;
            rg_sum += r * g;
            rb_sum += r * b;
            gb_sum += g * b;
        }
    }

    // Covariance (not normalized) is E(X*X.t) - m * m.t and this is how it
    // is calculated below.
    // Each row below represents a row of the matrix describing (co)variances
    // of R, G and B channels with (R, G, B)
    int pixel_n = bitmap.width() * bitmap.height();
    covariance.set(
        static_cast<float>(
            static_cast<double>(rr_sum) / pixel_n - static_cast<double>(r_sum * r_sum) / pixel_n / pixel_n),
        static_cast<float>(
            static_cast<double>(rg_sum) / pixel_n - static_cast<double>(r_sum * g_sum) / pixel_n / pixel_n),
        static_cast<float>(
            static_cast<double>(rb_sum) / pixel_n - static_cast<double>(r_sum * b_sum) / pixel_n / pixel_n),
        static_cast<float>(
            static_cast<double>(rg_sum) / pixel_n - static_cast<double>(r_sum * g_sum) / pixel_n / pixel_n),
        static_cast<float>(
            static_cast<double>(gg_sum) / pixel_n - static_cast<double>(g_sum * g_sum) / pixel_n / pixel_n),
        static_cast<float>(
            static_cast<double>(gb_sum) / pixel_n - static_cast<double>(g_sum * b_sum) / pixel_n / pixel_n),
        static_cast<float>(
            static_cast<double>(rb_sum) / pixel_n - static_cast<double>(r_sum * b_sum) / pixel_n / pixel_n),
        static_cast<float>(
            static_cast<double>(gb_sum) / pixel_n - static_cast<double>(g_sum * b_sum) / pixel_n / pixel_n),
        static_cast<float>(
            static_cast<double>(bb_sum) / pixel_n - static_cast<double>(b_sum * b_sum) / pixel_n / pixel_n));
    return covariance;
}

bool ApplyColorReduction(const SkBitmap& source_bitmap,
    const gfx::Vector3dF& color_transform,
    bool fit_to_range,
    SkBitmap* target_bitmap)
{
    DCHECK(target_bitmap);
    SkAutoLockPixels source_lock(source_bitmap);
    SkAutoLockPixels target_lock(*target_bitmap);

    DCHECK(source_bitmap.getPixels());
    DCHECK(target_bitmap->getPixels());
    DCHECK_EQ(kN32_SkColorType, source_bitmap.colorType());
    DCHECK_EQ(kAlpha_8_SkColorType, target_bitmap->colorType());
    DCHECK_EQ(source_bitmap.height(), target_bitmap->height());
    DCHECK_EQ(source_bitmap.width(), target_bitmap->width());
    DCHECK(!source_bitmap.empty());

    // Elements of color_transform are explicitly off-loaded to local values for
    // efficiency reasons. Note that in practice images may correspond to entire
    // tab captures.
    float t0 = 0.0;
    float tr = color_transform.x();
    float tg = color_transform.y();
    float tb = color_transform.z();

    if (fit_to_range) {
        // We will figure out min/max in a preprocessing step and adjust
        // actual_transform as required.
        float max_val = std::numeric_limits<float>::min();
        float min_val = std::numeric_limits<float>::max();
        for (int y = 0; y < source_bitmap.height(); ++y) {
            const SkPMColor* source_color_row = static_cast<SkPMColor*>(
                source_bitmap.getAddr32(0, y));
            for (int x = 0; x < source_bitmap.width(); ++x) {
                SkColor c = SkUnPreMultiply::PMColorToColor(source_color_row[x]);
                uint8_t r = SkColorGetR(c);
                uint8_t g = SkColorGetG(c);
                uint8_t b = SkColorGetB(c);
                float gray_level = tr * r + tg * g + tb * b;
                max_val = std::max(max_val, gray_level);
                min_val = std::min(min_val, gray_level);
            }
        }

        // Adjust the transform so that the result is scaling.
        float scale = 0.0;
        t0 = -min_val;
        if (max_val > min_val)
            scale = 255.0f / (max_val - min_val);
        t0 *= scale;
        tr *= scale;
        tg *= scale;
        tb *= scale;
    }

    for (int y = 0; y < source_bitmap.height(); ++y) {
        const SkPMColor* source_color_row = static_cast<SkPMColor*>(
            source_bitmap.getAddr32(0, y));
        uint8_t* target_color_row = target_bitmap->getAddr8(0, y);
        for (int x = 0; x < source_bitmap.width(); ++x) {
            SkColor c = SkUnPreMultiply::PMColorToColor(source_color_row[x]);
            uint8_t r = SkColorGetR(c);
            uint8_t g = SkColorGetG(c);
            uint8_t b = SkColorGetB(c);

            float gl = t0 + tr * r + tg * g + tb * b;
            if (gl < 0)
                gl = 0;
            if (gl > 0xFF)
                gl = 0xFF;
            target_color_row[x] = static_cast<uint8_t>(gl);
        }
    }

    return true;
}

bool ComputePrincipalComponentImage(const SkBitmap& source_bitmap,
    SkBitmap* target_bitmap)
{
    if (!target_bitmap) {
        NOTREACHED();
        return false;
    }

    gfx::Matrix3F covariance = ComputeColorCovariance(source_bitmap);
    gfx::Matrix3F eigenvectors = gfx::Matrix3F::Zeros();
    gfx::Vector3dF eigenvals = covariance.SolveEigenproblem(&eigenvectors);
    gfx::Vector3dF principal = eigenvectors.get_column(0);
    if (eigenvals == gfx::Vector3dF() || principal == gfx::Vector3dF())
        return false; // This may happen for some edge cases.
    return ApplyColorReduction(source_bitmap, principal, true, target_bitmap);
}

} // color_utils
