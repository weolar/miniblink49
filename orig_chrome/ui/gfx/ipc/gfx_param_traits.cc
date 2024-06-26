// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "ui/gfx/ipc/gfx_param_traits.h"

#include <stddef.h>
#include <stdint.h>

#include <string>

#include "third_party/skia/include/core/SkBitmap.h"
#include "ui/gfx/geometry/point3_f.h"
#include "ui/gfx/geometry/rect.h"
#include "ui/gfx/geometry/rect_f.h"
#include "ui/gfx/geometry/scroll_offset.h"
#include "ui/gfx/range/range.h"

#if defined(OS_MACOSX)
#include "ipc/mach_port_mac.h"
#endif

namespace {

struct SkBitmap_Data {
    // The color type for the bitmap (bits per pixel, etc).
    SkColorType fColorType;

    // The alpha type for the bitmap (opaque, premul, unpremul).
    SkAlphaType fAlphaType;

    // The width of the bitmap in pixels.
    uint32_t fWidth;

    // The height of the bitmap in pixels.
    uint32_t fHeight;

    void InitSkBitmapDataForTransfer(const SkBitmap& bitmap)
    {
        const SkImageInfo& info = bitmap.info();
        fColorType = info.colorType();
        fAlphaType = info.alphaType();
        fWidth = info.width();
        fHeight = info.height();
    }

    // Returns whether |bitmap| successfully initialized.
    bool InitSkBitmapFromData(SkBitmap* bitmap,
        const char* pixels,
        size_t pixels_size) const
    {
        if (!bitmap->tryAllocPixels(
                SkImageInfo::Make(fWidth, fHeight, fColorType, fAlphaType)))
            return false;
        if (pixels_size != bitmap->getSize())
            return false;
        memcpy(bitmap->getPixels(), pixels, pixels_size);
        return true;
    }
};

} // namespace

namespace IPC {

void ParamTraits<gfx::Point>::Write(base::Pickle* m, const gfx::Point& p)
{
    WriteParam(m, p.x());
    WriteParam(m, p.y());
}

bool ParamTraits<gfx::Point>::Read(const base::Pickle* m,
    base::PickleIterator* iter,
    gfx::Point* r)
{
    int x, y;
    if (!ReadParam(m, iter, &x) || !ReadParam(m, iter, &y))
        return false;
    r->set_x(x);
    r->set_y(y);
    return true;
}

void ParamTraits<gfx::Point>::Log(const gfx::Point& p, std::string* l)
{
    l->append(base::StringPrintf("(%d, %d)", p.x(), p.y()));
}

void ParamTraits<gfx::PointF>::Write(base::Pickle* m, const gfx::PointF& p)
{
    WriteParam(m, p.x());
    WriteParam(m, p.y());
}

bool ParamTraits<gfx::PointF>::Read(const base::Pickle* m,
    base::PickleIterator* iter,
    gfx::PointF* r)
{
    float x, y;
    if (!ReadParam(m, iter, &x) || !ReadParam(m, iter, &y))
        return false;
    r->set_x(x);
    r->set_y(y);
    return true;
}

void ParamTraits<gfx::PointF>::Log(const gfx::PointF& p, std::string* l)
{
    l->append(base::StringPrintf("(%f, %f)", p.x(), p.y()));
}

void ParamTraits<gfx::Point3F>::Write(base::Pickle* m, const gfx::Point3F& p)
{
    WriteParam(m, p.x());
    WriteParam(m, p.y());
    WriteParam(m, p.z());
}

bool ParamTraits<gfx::Point3F>::Read(const base::Pickle* m,
    base::PickleIterator* iter,
    gfx::Point3F* r)
{
    float x, y, z;
    if (!ReadParam(m, iter, &x) || !ReadParam(m, iter, &y) || !ReadParam(m, iter, &z))
        return false;
    r->set_x(x);
    r->set_y(y);
    r->set_z(z);
    return true;
}

void ParamTraits<gfx::Point3F>::Log(const gfx::Point3F& p, std::string* l)
{
    l->append(base::StringPrintf("(%f, %f, %f)", p.x(), p.y(), p.z()));
}

void ParamTraits<gfx::Size>::Write(base::Pickle* m, const gfx::Size& p)
{
    DCHECK_GE(p.width(), 0);
    DCHECK_GE(p.height(), 0);
    int values[2] = { p.width(), p.height() };
    m->WriteBytes(&values, sizeof(int) * 2);
}

bool ParamTraits<gfx::Size>::Read(const base::Pickle* m,
    base::PickleIterator* iter,
    gfx::Size* r)
{
    const char* char_values;
    if (!iter->ReadBytes(&char_values, sizeof(int) * 2))
        return false;
    const int* values = reinterpret_cast<const int*>(char_values);
    if (values[0] < 0 || values[1] < 0)
        return false;
    r->set_width(values[0]);
    r->set_height(values[1]);
    return true;
}

void ParamTraits<gfx::Size>::Log(const gfx::Size& p, std::string* l)
{
    l->append(base::StringPrintf("(%d, %d)", p.width(), p.height()));
}

void ParamTraits<gfx::SizeF>::Write(base::Pickle* m, const gfx::SizeF& p)
{
    float values[2] = { p.width(), p.height() };
    m->WriteBytes(&values, sizeof(float) * 2);
}

bool ParamTraits<gfx::SizeF>::Read(const base::Pickle* m,
    base::PickleIterator* iter,
    gfx::SizeF* r)
{
    const char* char_values;
    if (!iter->ReadBytes(&char_values, sizeof(float) * 2))
        return false;
    const float* values = reinterpret_cast<const float*>(char_values);
    r->set_width(values[0]);
    r->set_height(values[1]);
    return true;
}

void ParamTraits<gfx::SizeF>::Log(const gfx::SizeF& p, std::string* l)
{
    l->append(base::StringPrintf("(%f, %f)", p.width(), p.height()));
}

void ParamTraits<gfx::Vector2d>::Write(base::Pickle* m,
    const gfx::Vector2d& p)
{
    int values[2] = { p.x(), p.y() };
    m->WriteBytes(&values, sizeof(int) * 2);
}

bool ParamTraits<gfx::Vector2d>::Read(const base::Pickle* m,
    base::PickleIterator* iter,
    gfx::Vector2d* r)
{
    const char* char_values;
    if (!iter->ReadBytes(&char_values, sizeof(int) * 2))
        return false;
    const int* values = reinterpret_cast<const int*>(char_values);
    r->set_x(values[0]);
    r->set_y(values[1]);
    return true;
}

void ParamTraits<gfx::Vector2d>::Log(const gfx::Vector2d& v, std::string* l)
{
    l->append(base::StringPrintf("(%d, %d)", v.x(), v.y()));
}

void ParamTraits<gfx::Vector2dF>::Write(base::Pickle* m,
    const gfx::Vector2dF& p)
{
    float values[2] = { p.x(), p.y() };
    m->WriteBytes(&values, sizeof(float) * 2);
}

bool ParamTraits<gfx::Vector2dF>::Read(const base::Pickle* m,
    base::PickleIterator* iter,
    gfx::Vector2dF* r)
{
    const char* char_values;
    if (!iter->ReadBytes(&char_values, sizeof(float) * 2))
        return false;
    const float* values = reinterpret_cast<const float*>(char_values);
    r->set_x(values[0]);
    r->set_y(values[1]);
    return true;
}

void ParamTraits<gfx::Vector2dF>::Log(const gfx::Vector2dF& v, std::string* l)
{
    l->append(base::StringPrintf("(%f, %f)", v.x(), v.y()));
}

void ParamTraits<gfx::Rect>::Write(base::Pickle* m, const gfx::Rect& p)
{
    int values[4] = { p.x(), p.y(), p.width(), p.height() };
    m->WriteBytes(&values, sizeof(int) * 4);
}

bool ParamTraits<gfx::Rect>::Read(const base::Pickle* m,
    base::PickleIterator* iter,
    gfx::Rect* r)
{
    const char* char_values;
    if (!iter->ReadBytes(&char_values, sizeof(int) * 4))
        return false;
    const int* values = reinterpret_cast<const int*>(char_values);
    if (values[2] < 0 || values[3] < 0)
        return false;
    r->SetRect(values[0], values[1], values[2], values[3]);
    return true;
}

void ParamTraits<gfx::Rect>::Log(const gfx::Rect& p, std::string* l)
{
    l->append(base::StringPrintf("(%d, %d, %d, %d)", p.x(), p.y(),
        p.width(), p.height()));
}

void ParamTraits<gfx::RectF>::Write(base::Pickle* m, const gfx::RectF& p)
{
    float values[4] = { p.x(), p.y(), p.width(), p.height() };
    m->WriteBytes(&values, sizeof(float) * 4);
}

bool ParamTraits<gfx::RectF>::Read(const base::Pickle* m,
    base::PickleIterator* iter,
    gfx::RectF* r)
{
    const char* char_values;
    if (!iter->ReadBytes(&char_values, sizeof(float) * 4))
        return false;
    const float* values = reinterpret_cast<const float*>(char_values);
    r->SetRect(values[0], values[1], values[2], values[3]);
    return true;
}

void ParamTraits<gfx::RectF>::Log(const gfx::RectF& p, std::string* l)
{
    l->append(base::StringPrintf("(%f, %f, %f, %f)", p.x(), p.y(),
        p.width(), p.height()));
}

void ParamTraits<SkBitmap>::Write(base::Pickle* m, const SkBitmap& p)
{
    size_t fixed_size = sizeof(SkBitmap_Data);
    SkBitmap_Data bmp_data;
    bmp_data.InitSkBitmapDataForTransfer(p);
    m->WriteData(reinterpret_cast<const char*>(&bmp_data),
        static_cast<int>(fixed_size));
    size_t pixel_size = p.getSize();
    SkAutoLockPixels p_lock(p);
    m->WriteData(reinterpret_cast<const char*>(p.getPixels()),
        static_cast<int>(pixel_size));
}

bool ParamTraits<SkBitmap>::Read(const base::Pickle* m,
    base::PickleIterator* iter,
    SkBitmap* r)
{
    const char* fixed_data;
    int fixed_data_size = 0;
    if (!iter->ReadData(&fixed_data, &fixed_data_size) || (fixed_data_size <= 0)) {
        NOTREACHED();
        return false;
    }
    if (fixed_data_size != sizeof(SkBitmap_Data))
        return false; // Message is malformed.

    const char* variable_data;
    int variable_data_size = 0;
    if (!iter->ReadData(&variable_data, &variable_data_size) || (variable_data_size < 0)) {
        NOTREACHED();
        return false;
    }
    const SkBitmap_Data* bmp_data = reinterpret_cast<const SkBitmap_Data*>(fixed_data);
    return bmp_data->InitSkBitmapFromData(r, variable_data, variable_data_size);
}

void ParamTraits<SkBitmap>::Log(const SkBitmap& p, std::string* l)
{
    l->append("<SkBitmap>");
}

void ParamTraits<gfx::Range>::Write(base::Pickle* m, const gfx::Range& r)
{
    m->WriteUInt32(r.start());
    m->WriteUInt32(r.end());
}

bool ParamTraits<gfx::Range>::Read(const base::Pickle* m,
    base::PickleIterator* iter,
    gfx::Range* r)
{
    uint32_t start, end;
    if (!iter->ReadUInt32(&start) || !iter->ReadUInt32(&end))
        return false;
    r->set_start(start);
    r->set_end(end);
    return true;
}

void ParamTraits<gfx::Range>::Log(const gfx::Range& r, std::string* l)
{
    l->append(base::StringPrintf("(%d, %d)", r.start(), r.end()));
}

void ParamTraits<gfx::ScrollOffset>::Write(base::Pickle* m,
    const param_type& p)
{
    m->WriteDouble(p.x());
    m->WriteDouble(p.y());
}

bool ParamTraits<gfx::ScrollOffset>::Read(const base::Pickle* m,
    base::PickleIterator* iter,
    param_type* r)
{
    double x = 0.f;
    double y = 0.f;
    if (!iter->ReadDouble(&x))
        return false;
    if (!iter->ReadDouble(&y))
        return false;
    r->set_x(x);
    r->set_y(y);
    return true;
}

void ParamTraits<gfx::ScrollOffset>::Log(const param_type& p, std::string* l)
{
    l->append("(");
    LogParam(p.x(), l);
    l->append(", ");
    LogParam(p.y(), l);
    l->append(")");
}

#if defined(OS_MACOSX) && !defined(OS_IOS)
void ParamTraits<gfx::ScopedRefCountedIOSurfaceMachPort>::Write(
    base::Pickle* m,
    const param_type p)
{
    MachPortMac mach_port_mac(p.get());
    ParamTraits<MachPortMac>::Write(m, mach_port_mac);
}

bool ParamTraits<gfx::ScopedRefCountedIOSurfaceMachPort>::Read(
    const base::Pickle* m,
    base::PickleIterator* iter,
    param_type* r)
{
    MachPortMac mach_port_mac;
    if (!ParamTraits<MachPortMac>::Read(m, iter, &mach_port_mac))
        return false;
    r->reset(mach_port_mac.get_mach_port());
    return true;
}

void ParamTraits<gfx::ScopedRefCountedIOSurfaceMachPort>::Log(
    const param_type& p,
    std::string* l)
{
    l->append("IOSurface Mach send right: ");
    LogParam(p.get(), l);
}
#endif // defined(OS_MACOSX) && !defined(OS_IOS)

} // namespace IPC

// Generate param traits write methods.
#include "ipc/param_traits_write_macros.h"
namespace IPC {
#undef UI_GFX_IPC_GFX_PARAM_TRAITS_MACROS_H_
#include "ui/gfx/ipc/gfx_param_traits_macros.h"
} // namespace IPC

// Generate param traits read methods.
#include "ipc/param_traits_read_macros.h"
namespace IPC {
#undef UI_GFX_IPC_GFX_PARAM_TRAITS_MACROS_H_
#include "ui/gfx/ipc/gfx_param_traits_macros.h"
} // namespace IPC

// Generate param traits log methods.
#include "ipc/param_traits_log_macros.h"
namespace IPC {
#undef UI_GFX_IPC_GFX_PARAM_TRAITS_MACROS_H_
#include "ui/gfx/ipc/gfx_param_traits_macros.h"
} // namespace IPC
