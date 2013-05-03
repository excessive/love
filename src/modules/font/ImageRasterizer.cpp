/**
 * Copyright (c) 2006-2013 LOVE Development Team
 *
 * This software is provided 'as-is', without any express or implied
 * warranty.  In no event will the authors be held liable for any damages
 * arising from the use of this software.
 *
 * Permission is granted to anyone to use this software for any purpose,
 * including commercial applications, and to alter it and redistribute it
 * freely, subject to the following restrictions:
 *
 * 1. The origin of this software must not be misrepresented; you must not
 *    claim that you wrote the original software. If you use this software
 *    in a product, an acknowledgment in the product documentation would be
 *    appreciated but is not required.
 * 2. Altered source versions must be plainly marked as such, and must not be
 *    misrepresented as being the original software.
 * 3. This notice may not be removed or altered from any source distribution.
 **/

// LOVE
#include "ImageRasterizer.h"

#include "common/Exception.h"
#include <string.h>

namespace love
{
namespace font
{

inline bool equal(const love::image::pixel &a, const love::image::pixel &b)
{
	return (a.r == b.r && a.g == b.g && a.b == b.b && a.a == b.a);
}

ImageRasterizer::ImageRasterizer(love::image::ImageData *data, unsigned int *glyphs, int numglyphs)
	: imageData(data)
	, glyphs(glyphs)
	, numglyphs(numglyphs)
{
	imageData->retain();
	load();
}

ImageRasterizer::~ImageRasterizer()
{
	imageData->release();
}

int ImageRasterizer::getLineHeight() const
{
	return getHeight();
}

GlyphData *ImageRasterizer::getGlyphData(unsigned int glyph) const
{
	GlyphMetrics gm;
	memset(&gm, 0, sizeof(GlyphMetrics));

	// Set relevant glyph metrics if the glyph is in this ImageFont
	std::map<unsigned int, ImageGlyphData>::const_iterator it = imageGlyphs.find(glyph);
	if (it != imageGlyphs.end())
	{
		gm.width = it->second.width;
		gm.advance = it->second.width + it->second.spacing;
	}

	gm.height = metrics.height;

	GlyphData *g = new GlyphData(glyph, gm, GlyphData::FORMAT_RGBA);

	if (gm.width == 0)
		return g;

	love::image::pixel *gdpixels = (love::image::pixel *) g->getData();
	love::image::pixel *imagepixels = (love::image::pixel *) imageData->getData();

	// copy glyph pixels from imagedata to glyphdata
	for (int i = 0; i < g->getWidth() * g->getHeight(); i++)
	{
		love::image::pixel p = imagepixels[ it->second.x + (i % gm.width) + (imageData->getWidth() * (i / gm.width)) ];

		// Use transparency instead of the spacer color
		if (equal(p, spacer))
			gdpixels[i].r = gdpixels[i].g = gdpixels[i].b = gdpixels[i].a = 0;
		else
			gdpixels[i] = p;
	}

	return g;
}

void ImageRasterizer::load()
{
	love::image::pixel *pixels = (love::image::pixel *) imageData->getData();

	unsigned int imgw = (unsigned int) imageData->getWidth();
	unsigned int imgh = (unsigned int) imageData->getHeight();

	// Set the only metric that matters
	metrics.height = imgh;

	// Reading texture data begins
	spacer = pixels[0];

	unsigned int start = 0;
	unsigned int end = 0;

	for (unsigned int i = 0; i < numglyphs; ++i)
	{
		start = end;

		// Finds out where the first character starts
		while (start < imgw && equal(pixels[start], spacer))
			++start;

		// set previous glyph's spacing
		if (i > 0 && imageGlyphs.size() > 0)
			imageGlyphs[glyphs[i - 1]].spacing = (start > end) ? (start - end) : 0;

		end = start;

		// Find where glyph ends.
		while (end < imgw && !equal(pixels[end], spacer))
			++end;

		if (start >= end)
			break;

		ImageGlyphData imageGlyph;
		imageGlyph.x = start;
		imageGlyph.width = end - start;

		imageGlyphs[glyphs[i]] = imageGlyph;
	}

	// Find spacing of last glyph
	if (numglyphs > 0)
	{
		start = end;
		while (start < imgw && equal(pixels[start], spacer))
			++start;

		imageGlyphs[glyphs[numglyphs - 1]].spacing = (start > end) ? (start - end) : 0;
	}
}

int ImageRasterizer::getGlyphCount() const
{
	return numglyphs;
}

} // font
} // love
