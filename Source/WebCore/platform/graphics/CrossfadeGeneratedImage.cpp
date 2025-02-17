/*
 * Copyright (C) 2011 Apple Inc. All rights reserved.
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
#include "CrossfadeGeneratedImage.h"

#include "FloatRect.h"
#include "GraphicsContext.h"
#include "ImageBuffer.h"
#include "TextStream.h"

namespace WebCore {

CrossfadeGeneratedImage::CrossfadeGeneratedImage(Image& fromImage, Image& toImage, float percentage, const FloatSize& crossfadeSize, const FloatSize& size)
    : m_fromImage(fromImage)
    , m_toImage(toImage)
    , m_percentage(percentage)
    , m_crossfadeSize(crossfadeSize)
{
    setContainerSize(size);
}

static void drawCrossfadeSubimage(GraphicsContext& context, Image& image, CompositeOperator operation, float opacity, const FloatSize& targetSize)
{
    FloatSize imageSize = image.size();

    // SVGImage resets the opacity when painting, so we have to use transparency layers to accurately paint one at a given opacity.
    bool useTransparencyLayer = image.isSVGImage();

    GraphicsContextStateSaver stateSaver(context);
    
    CompositeOperator drawImageOperation = operation;

    if (useTransparencyLayer) {
        context.setCompositeOperation(operation);
        context.beginTransparencyLayer(opacity);
        drawImageOperation = CompositeSourceOver;
    } else
        context.setAlpha(opacity);

    if (targetSize != imageSize)
        context.scale(FloatSize(targetSize.width() / imageSize.width(), targetSize.height() / imageSize.height()));

    context.drawImage(image, IntPoint(), ImagePaintingOptions(drawImageOperation));

    if (useTransparencyLayer)
        context.endTransparencyLayer();
}

void CrossfadeGeneratedImage::drawCrossfade(GraphicsContext& context)
{
    // Draw nothing if either of the images hasn't loaded yet.
    if (m_fromImage.ptr() == Image::nullImage() || m_toImage.ptr() == Image::nullImage())
        return;

    GraphicsContextStateSaver stateSaver(context);

    context.clip(FloatRect(FloatPoint(), m_crossfadeSize));
    context.beginTransparencyLayer(1);

    drawCrossfadeSubimage(context, m_fromImage.get(), CompositeSourceOver, 1 - m_percentage, m_crossfadeSize);
    drawCrossfadeSubimage(context, m_toImage.get(), CompositePlusLighter, m_percentage, m_crossfadeSize);

    context.endTransparencyLayer();
}

void CrossfadeGeneratedImage::draw(GraphicsContext& context, const FloatRect& dstRect, const FloatRect& srcRect, CompositeOperator compositeOp, BlendMode blendMode, DecodingMode, ImageOrientationDescription)
{
    GraphicsContextStateSaver stateSaver(context);
    context.setCompositeOperation(compositeOp, blendMode);
    context.clip(dstRect);
    context.translate(dstRect.x(), dstRect.y());
    if (dstRect.size() != srcRect.size())
        context.scale(FloatSize(dstRect.width() / srcRect.width(), dstRect.height() / srcRect.height()));
    context.translate(-srcRect.x(), -srcRect.y());
    
    drawCrossfade(context);
}

void CrossfadeGeneratedImage::drawPattern(GraphicsContext& context, const FloatRect& dstRect, const FloatRect& srcRect, const AffineTransform& patternTransform, const FloatPoint& phase, const FloatSize& spacing, CompositeOperator compositeOp, BlendMode blendMode)
{
    std::unique_ptr<ImageBuffer> imageBuffer = ImageBuffer::create(size(), context.renderingMode());
    if (!imageBuffer)
        return;

    // Fill with the cross-faded image.
    GraphicsContext& graphicsContext = imageBuffer->context();
    drawCrossfade(graphicsContext);

    // Tile the image buffer into the context.
    imageBuffer->drawPattern(context, dstRect, srcRect, patternTransform, phase, spacing, compositeOp, blendMode);
}

void CrossfadeGeneratedImage::dump(TextStream& ts) const
{
    GeneratedImage::dump(ts);
    ts.dumpProperty("from-image", m_fromImage.get());
    ts.dumpProperty("to-image", m_toImage.get());
    ts.dumpProperty("percentage", m_percentage);
}

}
