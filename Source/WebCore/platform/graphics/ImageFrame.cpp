/*
 * Copyright (C) 2016 Apple Inc.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY APPLE INC. ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL APPLE INC. OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "config.h"
#include "ImageFrame.h"

#include <wtf/NeverDestroyed.h>

namespace WebCore {

ImageFrame::ImageFrame()
{
}

ImageFrame::~ImageFrame()
{
    clearImage();
}

const ImageFrame& ImageFrame::defaultFrame()
{
    static NeverDestroyed<ImageFrame> sharedInstance;
    return sharedInstance;
}

ImageFrame& ImageFrame::operator=(const ImageFrame& other)
{
    if (this == &other)
        return *this;

    m_decoding = other.m_decoding;
    m_size = other.m_size;

#if !USE(CG)
    if (other.backingStore())
        initialize(*other.backingStore());
    else
        m_backingStore = nullptr;
    m_disposalMethod = other.m_disposalMethod;
#endif

    m_nativeImage = other.m_nativeImage;
    m_subsamplingLevel = other.m_subsamplingLevel;
    m_decodingOptions = other.m_decodingOptions;

    m_orientation = other.m_orientation;
    m_duration = other.m_duration;
    m_hasAlpha = other.m_hasAlpha;
    return *this;
}

unsigned ImageFrame::clearImage()
{
#if !USE(CG)
    if (hasBackingStore())
        m_backingStore = nullptr;
#endif

    if (!hasNativeImage())
        return 0;

    unsigned frameBytes = this->frameBytes();

    clearNativeImageSubimages(m_nativeImage);
    m_nativeImage = nullptr;
    m_decodingOptions = { };

    return frameBytes;
}

unsigned ImageFrame::clear()
{
    unsigned frameBytes = clearImage();
    *this = ImageFrame();
    return frameBytes;
}

#if !USE(CG)
bool ImageFrame::initialize(const ImageBackingStore& backingStore)
{
    if (&backingStore == this->backingStore())
        return true;

    m_backingStore = ImageBackingStore::create(backingStore);
    return m_backingStore != nullptr;
}

bool ImageFrame::initialize(const IntSize& size, bool premultiplyAlpha)
{
    if (size.isEmpty())
        return false;

    m_backingStore = ImageBackingStore::create(size, premultiplyAlpha);
    return m_backingStore != nullptr;
}
#endif

IntSize ImageFrame::size() const
{
#if !USE(CG)
    if (hasBackingStore())
        return backingStore()->size();
#endif
    return m_size;
}
    
bool ImageFrame::hasNativeImage(const std::optional<SubsamplingLevel>& subsamplingLevel) const
{
    return m_nativeImage && (!subsamplingLevel || *subsamplingLevel >= m_subsamplingLevel);
}

bool ImageFrame::hasFullSizeNativeImage(const std::optional<SubsamplingLevel>& subsamplingLevel) const
{
    return hasNativeImage(subsamplingLevel) && (m_decodingOptions.isSynchronous() || m_decodingOptions.hasFullSize());
}

bool ImageFrame::hasDecodedNativeImageCompatibleWithOptions(const std::optional<SubsamplingLevel>& subsamplingLevel, const DecodingOptions& decodingOptions) const
{
    return hasNativeImage(subsamplingLevel) && m_decodingOptions.isAsynchronousCompatibleWith(decodingOptions);
}

Color ImageFrame::singlePixelSolidColor() const
{
    if (!hasNativeImage() || m_size != IntSize(1, 1))
        return Color();

    return nativeImageSinglePixelSolidColor(m_nativeImage);
}

}
