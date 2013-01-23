/**
 * Copyright (c) 2006-2012 LOVE Development Team
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

#include "Font.h"

#include "TrueTypeRasterizer.h"
#include "font/ImageRasterizer.h"

#include "libraries/utf8/utf8.h"

namespace love
{
namespace font
{
namespace freetype
{

Font::Font()
{
	if (FT_Init_FreeType(&library))
		throw love::Exception("TrueTypeFont Loading error: FT_Init_FreeType failed\n");
}

Font::~Font()
{
	FT_Done_FreeType(library);
}

Rasterizer *Font::newRasterizer(Data *data, int size)
{
	return new TrueTypeRasterizer(library, data, size);
}

Rasterizer *Font::newRasterizer(love::image::ImageData *data, std::string text)
{
	size_t strlen = text.size();
	size_t numglyphs = 0;
	
	unsigned int *glyphs = new unsigned int[strlen];

	try
	{
		utf8::iterator<std::string::const_iterator> i(text.begin(), text.begin(), text.end());
		utf8::iterator<std::string::const_iterator> end(text.end(), text.begin(), text.end());

		while (i != end)
		{
			if (numglyphs >= strlen)
				throw love::Exception("foo");

			glyphs[numglyphs++] = *i++;
		}
	}
	catch (love::Exception &)
	{
		delete [] glyphs;
		throw;
	}
	catch (utf8::exception &e)
	{
		delete [] glyphs;
		throw love::Exception("%s", e.what());
	}
	
	Rasterizer *r = newRasterizer(data, glyphs, numglyphs);
	delete [] glyphs;
	
	return r;
}

Rasterizer *Font::newRasterizer(love::image::ImageData *data, unsigned int *glyphs, int numglyphs)
{
	return new ImageRasterizer(data, glyphs, numglyphs);
}

GlyphData *Font::newGlyphData(Rasterizer *r, unsigned int glyph)
{
	return r->getGlyphData(glyph);
}

const char *Font::getName() const
{
	return "love.font.freetype";
}

} // freetype
} // font
} // love
