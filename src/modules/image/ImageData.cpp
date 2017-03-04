/**
 * Copyright (c) 2006-2017 LOVE Development Team
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

using love::thread::Lock;

namespace love
{
namespace image
{

love::Type ImageData::type("ImageData", &Data::type);

ImageData::ImageData()
	: data(nullptr)
{
}

ImageData::~ImageData()
{
}

size_t ImageData::getSize() const
{
	return size_t(getWidth() * getHeight()) * getPixelSize();
}

void *ImageData::getData() const
{
	return data;
}

bool ImageData::isSRGB() const
{
	return false;
}

bool ImageData::inside(int x, int y) const
{
	return x >= 0 && x < getWidth() && y >= 0 && y < getHeight();
}

void ImageData::setPixel(int x, int y, const Pixel &p)
{
	if (!inside(x, y))
		throw love::Exception("Attempt to set out-of-range pixel!");

	size_t pixelsize = getPixelSize();
	unsigned char *pixeldata = data + ((y * width + x) * pixelsize);

	Lock lock(mutex);
	memcpy(pixeldata, &p, pixelsize);
}

void ImageData::getPixel(int x, int y, Pixel &p) const
{
	if (!inside(x, y))
		throw love::Exception("Attempt to get out-of-range pixel!");

	size_t pixelsize = getPixelSize();

	Lock lock(mutex);
	memcpy(&p, data + ((y * width + x) * pixelsize), pixelsize);
}

void ImageData::paste(ImageData *src, int dx, int dy, int sx, int sy, int sw, int sh)
{
	PixelFormat dstformat = getFormat();
	PixelFormat srcformat = src->getFormat();

	int srcW = src->getWidth();
	int srcH = src->getHeight();
	int dstW = getWidth();
	int dstH = getHeight();

	size_t srcpixelsize = src->getPixelSize();
	size_t dstpixelsize = getPixelSize();

	// Check bounds; if the data ends up completely out of bounds, get out early.
	if (sx >= srcW || sx + sw < 0 || sy >= srcH || sy + sh < 0
			|| dx >= dstW || dx + sw < 0 || dy >= dstH || dy + sh < 0)
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

	if (dx + sw > dstW)
		sw = dstW - dx;

	if (dy + sh > dstH)
		sh = dstH - dy;

	if (sx + sw > srcW)
		sw = srcW - sx;

	if (sy + sh > srcH)
		sh = srcH - sy;

	Lock lock2(src->mutex);
	Lock lock1(mutex);

	uint8 *s = (uint8 *) src->getData();
	uint8 *d = (uint8 *) getData();

	// If the dimensions match up, copy the entire memory stream in one go
	if (srcformat == dstformat && (sw == dstW && dstW == srcW && sh == dstH && dstH == srcH))
	{
		memcpy(d, s, srcpixelsize * sw * sh);
	}
	else if (sw > 0)
	{
		// Otherwise, copy each row individually.
		for (int i = 0; i < sh; i++)
		{
			Row rowsrc = {s + (sx + (i + sy) * srcW) * srcpixelsize};
			Row rowdst = {d + (dx + (i + dy) * dstW) * dstpixelsize};

			if (srcformat == dstformat)
				memcpy(rowdst.u8, rowsrc.u8, srcpixelsize * sw);

			else if (srcformat == PIXELFORMAT_RGBA8 && dstformat == PIXELFORMAT_RGBA16)
				pasteRGBA8toRGBA16(rowsrc, rowdst, sw);
			else if (srcformat == PIXELFORMAT_RGBA8 && dstformat == PIXELFORMAT_RGBA16F)
				pasteRGBA8toRGBA16F(rowsrc, rowdst, sw);
			else if (srcformat == PIXELFORMAT_RGBA8 && dstformat == PIXELFORMAT_RGBA32F)
				pasteRGBA8toRGBA32F(rowsrc, rowdst, sw);

			else if (srcformat == PIXELFORMAT_RGBA16 && dstformat == PIXELFORMAT_RGBA8)
				pasteRGBA16toRGBA8(rowsrc, rowdst, sw);
			else if (srcformat == PIXELFORMAT_RGBA16 && dstformat == PIXELFORMAT_RGBA16F)
				pasteRGBA16toRGBA16F(rowsrc, rowdst, sw);
			else if (srcformat == PIXELFORMAT_RGBA16 && dstformat == PIXELFORMAT_RGBA32F)
				pasteRGBA16toRGBA32F(rowsrc, rowdst, sw);

			else if (srcformat == PIXELFORMAT_RGBA16F && dstformat == PIXELFORMAT_RGBA8)
				pasteRGBA16FtoRGBA8(rowsrc, rowdst, sw);
			else if (srcformat == PIXELFORMAT_RGBA16F && dstformat == PIXELFORMAT_RGBA16)
				pasteRGBA16FtoRGBA16(rowsrc, rowdst, sw);
			else if (srcformat == PIXELFORMAT_RGBA16F && dstformat == PIXELFORMAT_RGBA32F)
				pasteRGBA16FtoRGBA32F(rowsrc, rowdst, sw);

			else if (srcformat == PIXELFORMAT_RGBA32F && dstformat == PIXELFORMAT_RGBA8)
				pasteRGBA32FtoRGBA8(rowsrc, rowdst, sw);
			else if (srcformat == PIXELFORMAT_RGBA32F && dstformat == PIXELFORMAT_RGBA16)
				pasteRGBA32FtoRGBA16(rowsrc, rowdst, sw);
			else if (srcformat == PIXELFORMAT_RGBA32F && dstformat == PIXELFORMAT_RGBA16F)
				pasteRGBA32FtoRGBA16F(rowsrc, rowdst, sw);

			else
				throw love::Exception("Unsupported pixel format combination in ImageData:paste!");
		}
	}
}

void ImageData::pasteRGBA8toRGBA16(Row src, Row dst, int w)
{
	for (int i = 0; i < w * 4; i++)
		dst.u16[i] = (uint16) src.u8[i] << 8u;
}

void ImageData::pasteRGBA8toRGBA16F(Row src, Row dst, int w)
{
	for (int i = 0; i < w * 4; i++)
		dst.f16[i] = floatToHalf(src.u8[i] / 255.0f);
}

void ImageData::pasteRGBA8toRGBA32F(Row src, Row dst, int w)
{
	for (int i = 0; i < w * 4; i++)
		dst.f32[i] = src.u8[i] / 255.0f;
}

void ImageData::pasteRGBA16toRGBA8(Row src, Row dst, int w)
{
	for (int i = 0; i < w * 4; i++)
		dst.u8[i] = src.u16[i] >> 8u;
}

void ImageData::pasteRGBA16toRGBA16F(Row src, Row dst, int w)
{
	for (int i = 0; i < w * 4; i++)
		dst.f16[i] = floatToHalf(src.u16[i] / 65535.0f);
}

void ImageData::pasteRGBA16toRGBA32F(Row src, Row dst, int w)
{
	for (int i = 0; i < w * 4; i++)
		dst.f32[i] = src.u16[i] / 65535.0f;
}

void ImageData::pasteRGBA16FtoRGBA8(Row src, Row dst, int w)
{
	for (int i = 0; i < w * 4; i++)
		dst.u8[i] = (uint8) (halfToFloat(src.f16[i]) * 255.0f);
}

void ImageData::pasteRGBA16FtoRGBA16(Row src, Row dst, int w)
{
	for (int i = 0; i < w * 4; i++)
		dst.u16[i] = (uint16) (halfToFloat(src.f16[i]) * 65535.0f);
}

void ImageData::pasteRGBA16FtoRGBA32F(Row src, Row dst, int w)
{
	for (int i = 0; i < w * 4; i++)
		dst.f32[i] = halfToFloat(src.f16[i]);
}

void ImageData::pasteRGBA32FtoRGBA8(Row src, Row dst, int w)
{
	for (int i = 0; i < w * 4; i++)
		dst.u8[i] = (uint8) (src.f32[i] * 255.0f);
}

void ImageData::pasteRGBA32FtoRGBA16(Row src, Row dst, int w)
{
	for (int i = 0; i < w * 4; i++)
		dst.u16[i] = (uint16) (src.f32[i] * 65535.0f);
}

void ImageData::pasteRGBA32FtoRGBA16F(Row src, Row dst, int w)
{
	for (int i = 0; i < w * 4; i++)
		dst.f16[i] = (uint16) floatToHalf(src.f32[i]);
}

love::thread::Mutex *ImageData::getMutex() const
{
	return mutex;
}

size_t ImageData::getPixelSize() const
{
	return getPixelFormatSize(format);
}

bool ImageData::validPixelFormat(PixelFormat format)
{
	switch (format)
	{
	case PIXELFORMAT_RGBA8:
	case PIXELFORMAT_RGBA16:
	case PIXELFORMAT_RGBA16F:
	case PIXELFORMAT_RGBA32F:
		return true;
	default:
		return false;
	}
}

bool ImageData::getConstant(const char *in, EncodedFormat &out)
{
	return encodedFormats.find(in, out);
}

bool ImageData::getConstant(EncodedFormat in, const char *&out)
{
	return encodedFormats.find(in, out);
}

StringMap<ImageData::EncodedFormat, ImageData::ENCODED_MAX_ENUM>::Entry ImageData::encodedFormatEntries[] =
{
	{"tga", ENCODED_TGA},
	{"png", ENCODED_PNG},
};

StringMap<ImageData::EncodedFormat, ImageData::ENCODED_MAX_ENUM> ImageData::encodedFormats(ImageData::encodedFormatEntries, sizeof(ImageData::encodedFormatEntries));

} // image
} // love
