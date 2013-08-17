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

#ifndef LOVE_IMAGE_COMPRESSED_DATA_H
#define LOVE_IMAGE_COMPRESSED_DATA_H

// LOVE
#include "common/Data.h"
#include "common/StringMap.h"
#include "common/int.h"

// STL
#include <vector>

namespace love
{
namespace image
{

/**
 * CompressedData represents image data which is designed to be uploaded to the
 * GPU and rendered in its compressed form, without being un-compressed.
 * http://renderingpipeline.com/2012/07/texture-compression/
 **/
class CompressedData : public Data
{
public:

	// Recognized compressed image data formats.
	enum Format
	{
		FORMAT_UNKNOWN,
		FORMAT_DXT1,
		FORMAT_DXT3,
		FORMAT_DXT5,
		FORMAT_BC4,
		FORMAT_BC4s,
		FORMAT_BC5,
		FORMAT_BC5s,
		FORMAT_MAX_ENUM
	};

	// Compressed image data can have multiple mipmap levels, each represented
	// by a sub-image.
	struct SubImage
	{
		int width, height;
		size_t size;
		uint8 *data;
	};

	CompressedData();
	virtual ~CompressedData();

	// Implements Data. Note that data for different mipmap levels is not always
	// stored contiguously in memory, so getData() and getSize() don't make
	// much sense. Use getData(miplevel) and getSize(mipleveL) instead.
	virtual void *getData() const;
	virtual int getSize() const;

	/**
	 * Gets the number of mipmaps in this Compressed Image Data.
	 * Includes the base image level.
	 **/
	int getMipmapCount() const;

	/**
	 * Gets the size in bytes of a sub-image at the specified mipmap level.
	 **/
	int getSize(int miplevel) const;

	/**
	 * Gets the byte data of a sub-image at the specified mipmap level.
	 **/
	void *getData(int miplevel) const;

	/**
	 * Gets the width of a sub-image at the specified mipmap level.
	 **/
	int getWidth(int miplevel) const;

	/**
	 * Gets the height of a sub-image at the specified mipmap level.
	 **/
	int getHeight(int miplevel) const;

	/**
	 * Gets the format of the compressed data.
	 **/
	Format getFormat() const;

	static bool getConstant(const char *in, Format &out);
	static bool getConstant(Format in, const char *&out);

protected:

	Format format;

	// Texture info for each mipmap level.
	std::vector<SubImage> dataImages;

	void checkMipmapLevelExists(int miplevel) const;

private:

	static StringMap<Format, FORMAT_MAX_ENUM>::Entry formatEntries[];
	static StringMap<Format, FORMAT_MAX_ENUM> formats;

}; // CompressedData

} // image
} // love

#endif // LOVE_IMAGE_COMPRESSED_DATA_H
