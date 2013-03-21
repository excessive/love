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

#include "ImageData.h"

#include <stdio.h>

using love::thread::Lock;

namespace love
{
namespace image
{

ImageData::ImageData()
{
	mutex = thread::newMutex();
}

ImageData::~ImageData()
{
	delete mutex;
}

void *ImageData::getData() const
{
	return data;
}

int ImageData::getSize() const
{
	return getWidth()*getHeight()*sizeof(pixel);
}

int ImageData::getWidth() const
{
	return width;
}

int ImageData::getHeight() const
{
	return height;
}

bool ImageData::inside(int x, int y) const
{
	return x >= 0 && x < getWidth() && y >= 0 && y < getHeight();
}

void ImageData::setPixel(int x, int y, pixel c)
{
	if (!inside(x, y))
		throw love::Exception("Attempt to set out-of-range pixel!");

	Lock lock(mutex);

	pixel *pixels = (pixel *)getData();
	pixels[y*getWidth()+x] = c;
}

pixel ImageData::getPixel(int x, int y)
{
	if (!inside(x, y))
		throw love::Exception("Attempt to get out-of-range pixel!");

	Lock lock(mutex);

	pixel *pixels = (pixel *)getData();
	return pixels[y*getWidth()+x];
}

void ImageData::paste(ImageData *src, int dx, int dy, int sx, int sy, int sw, int sh)
{
	Lock lock2(src->mutex);
	Lock lock1(mutex);

	pixel *s = (pixel *)src->getData();
	pixel *d = (pixel *)getData();

	// Check bounds; if the data ends up completely out of bounds, get out early.
	if (sx >= src->getWidth() || sx + sw < 0 || sy >= src->getHeight() || sy + sh < 0
			|| dx >= getWidth() || dx + sw < 0 || dy >= getHeight() || dy + sh < 0)
		return;

	// Normalize values to the inside of both images.
	if (dx < 0)
	{
		sw += dx;
		sx -= dx;
		dx = 0;
	}
	if (dy < 0)
	{
		sh += dy;
		sy -= dy;
		dy = 0;
	}
	if (sx < 0)
	{
		sw += sx;
		dx -= sx;
		sx = 0;
	}
	if (sy < 0)
	{
		sh += sy;
		dy -= sy;
		sy = 0;
	}
	if (dx + sw > getWidth())
	{
		sw = getWidth() - dx;
	}
	if (dy + sh > getHeight())
	{
		sh = getHeight() - dy;
	}
	if (sx + sw > src->getWidth())
	{
		sw = src->getWidth() - sx;
	}
	if (sy + sh > src->getHeight())
	{
		sh = src->getHeight() - sy;
	}

	// If the dimensions match up, copy the entire memory stream in one go
	if (sw == getWidth() && getWidth() == src->getWidth()
			&& sh == getHeight() && getHeight() == src->getHeight())
		memcpy(d, s, sizeof(pixel) * sw * sh);
	else if (sw > 0)  // Otherwise, copy each row individually
	{
		for (int i = 0; i < sh; i++)
		{
			memcpy(d + dx + (i + dy) * getWidth(), s + sx + (i + sy) * src->getWidth(), sizeof(pixel) * sw);
		}
	}
}

bool ImageData::getConstant(const char *in, ImageData::Format &out)
{
	return formats.find(in, out);
}

bool ImageData::getConstant(ImageData::Format in, const char  *&out)
{
	return formats.find(in, out);
}

StringMap<ImageData::Format, ImageData::FORMAT_MAX_ENUM>::Entry ImageData::formatEntries[] =
{
	{"tga", ImageData::FORMAT_TGA},
	{"bmp", ImageData::FORMAT_BMP},
	{"jpg", ImageData::FORMAT_JPG},
	{"png", ImageData::FORMAT_PNG},
};

StringMap<ImageData::Format, ImageData::FORMAT_MAX_ENUM> ImageData::formats(ImageData::formatEntries, sizeof(ImageData::formatEntries));

} // image
} // love
