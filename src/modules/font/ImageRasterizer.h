/**
 * Copyright (c) 2006-2018 LOVE Development Team
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

#ifndef LOVE_FONT_IMAGE_RASTERIZER_H
#define LOVE_FONT_IMAGE_RASTERIZER_H

// LOVE
#include "font/Rasterizer.h"
#include "image/ImageData.h"
#include "common/Color.h"

#include <map>

namespace love
{
namespace font
{

/**
 * Holds data for a font object.
 **/
class ImageRasterizer : public Rasterizer
{
public:
	ImageRasterizer(love::image::ImageData *imageData, uint32 *glyphs, int numglyphs, int extraspacing, float dpiscale);
	virtual ~ImageRasterizer();

	// Implement Rasterizer
	int getLineHeight() const override;
	GlyphData *getGlyphData(uint32 glyph) const override;
	int getGlyphCount() const override;
	bool hasGlyph(uint32 glyph) const override;
	DataType getDataType() const override;


private:

	// Information about a glyph in the ImageData
	struct ImageGlyphData
	{
		int x;
		int width;
	};

	// Load all the glyph positions into memory
	void load();

	// The image data
	StrongRef<love::image::ImageData> imageData;

	// The glyphs in the font
	uint32 *glyphs;

	// Number of glyphs in the font
	int numglyphs;

	int extraSpacing;

	std::map<uint32, ImageGlyphData> imageGlyphs;

	// Color used to identify glyph separation in the source ImageData
	Color spacer;

}; // ImageRasterizer

} // font
} // love

#endif // LOVE_FONT_IMAGE_RASTERIZER_H
