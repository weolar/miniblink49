/*
 * Copyright (C) 2006 Apple Computer, Inc.
 * Copyright (C) 2007-2009 Torch Mobile, Inc.
 * Copyright (C) Research In Motion Limited 2009-2010. All rights reserved.
 *
 * Portions are Copyright (C) 2001 mozilla.org
 *
 * Other contributors:
 *   Stuart Parmenter <stuart@mozilla.com>
 *   weolar <weolar@qq.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Alternatively, the contents of this file may be used under the terms
 * of either the Mozilla Public License Version 1.1, found at
 * http://www.mozilla.org/MPL/ (the "MPL") or the GNU General Public
 * License Version 2.0, found at http://www.fsf.org/copyleft/gpl.html
 * (the "GPL"), in which case the provisions of the MPL or the GPL are
 * applicable instead of those above.  If you wish to allow use of your
 * version of this file only under the terms of one of those two
 * licenses (the MPL or the GPL) and not to allow others to use your
 * version of this file under the LGPL, indicate your decision by
 * deletingthe provisions above and replace them with the notice and
 * other provisions required by the MPL or the GPL, as the case may be.
 * If you do not delete the provisions above, a recipient may use your
 * version of this file under any of the LGPL, the MPL or the GPL.
 */

#include "config.h"
#include <windows.h>
#include "ImageGDIPlusDecoder.h"

#undef min
#undef max
using std::max;
using std::min;
#include <gdiplus.h>

#include "platform/image-decoders/GDIPlus/GDIPlusReader.h"
#include "platform/graphics/GDIPlusInit.h"
#include "public/platform/Platform.h"
#include <wtf/PassOwnPtr.h>

namespace blink {

static const size_t sizeOfFileHeader = 14;

ImageGDIPlusDecoder::ImageGDIPlusDecoder(ImageSource::AlphaOption alphaOption,
    ImageSource::GammaAndColorProfileOption gammaAndColorProfileOption,
    GDIPlusDecoderType type, size_t maxDecodedBytes)
    : ImageDecoder(alphaOption, gammaAndColorProfileOption, maxDecodedBytes)
    , m_type(type)
    , m_gdipBitmap(nullptr)
{

}

ImageGDIPlusDecoder::~ImageGDIPlusDecoder()
{
}

String ImageGDIPlusDecoder::filenameExtension() const
{
    return GDIPlusDecoderPNG == m_type ? "png" : "jpg";
}

void ImageGDIPlusDecoder::setData(SharedBuffer* data, bool allDataReceived)
{
    if (failed())
        return;

    if (!data) {
        ImageDecoder::setData(data, allDataReceived);
        return;
    }

//     decodeToBitmapByGDIPlus(data, &m_gdipBitmap);
//     if (!m_gdipBitmap) {
//         setFailed();
//         return;
//     }
// 
//     if (!m_gdipBitmap->GetWidth() || !m_gdipBitmap->GetHeight())
//         return;

    ImageDecoder::setData(data, allDataReceived);
    decode(true); // to ensure m_reader
}

bool ImageGDIPlusDecoder::setFailed()
{
    m_reader.clear();
    return ImageDecoder::setFailed();
}

void ImageGDIPlusDecoder::decode(bool onlySize)
{
    if (failed())
        return;

    // If we couldn't decode the image but we've received all the data, decoding
    // has failed.
    if (!decodeHelper(onlySize) && isAllDataReceived())
        setFailed();
    // If we're done decoding the image, we don't need the BMPImageReader
    // anymore.  (If we failed, |m_reader| has already been cleared.)
    else if (!m_frameBufferCache.isEmpty() && (m_frameBufferCache.first().status() == ImageFrame::FrameComplete))
        m_reader.clear();
}

bool ImageGDIPlusDecoder::decodeHelper(bool onlySize)
{
    if (!m_data.get())
        return false;

    if (!m_reader) {
        m_reader = adoptPtr(new GDIPlusReader(this));
        m_reader->setForceBitMaskAlpha();
        m_reader->setData(m_data.get());
    }

    if (!m_frameBufferCache.isEmpty())
        m_reader->setBuffer(&m_frameBufferCache.first());

    return m_reader->decode(onlySize);
}

} // namespace blink
