/**
 * Simple DDS data parser for compressed 2D textures.
 *
 * Copyright (c) 2013 Alexander Szpakowski.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 **/

#include "ddsparse.h"
#include "ddsinfo.h"

#include <algorithm>

namespace dds
{

using namespace dds::dxinfo;

// Creates a packed uint representation of a FourCC code.
static inline uint32_t FourCC(char a, char b, char c, char d)
{
	uint32_t fcc = ((uint32_t) a)
	| (((uint32_t) b) << 8)
	| (((uint32_t) c) << 16)
	| (((uint32_t) d) << 24);

	return fcc;
}

Parser::Image::Image()
	: width(0)
	, height(0)
	, dataSize(0)
	, data(0)
{
}

bool Parser::isDDS(const void *data, size_t dataSize)
{
	const uint8_t *readData = (const uint8_t *) data;
	ptrdiff_t offset = 0;

	// Is the data large enough to hold the DDS header?
	if(dataSize < sizeof(uint32_t) + sizeof(DDSHeader))
		return false;

	// All DDS files start with "DDS ".
	if((*(uint32_t *) readData) != FourCC('D','D','S',' '))
		return false;

	offset += sizeof(uint32_t);

	DDSHeader *header = (DDSHeader *) &readData[offset];


	// Verify header to validate DDS data.
	if (header->size != sizeof(DDSHeader) || header->format.size != sizeof(DDSPixelFormat))
		return false;

	offset += sizeof(DDSHeader);

	// Check for DX10 extension.
	if ((header->format.flags & DDPF_FOURCC) && (header->format.fourCC == FourCC('D','X','1','0')))
	{
		// Data must be big enough for both headers plus the magic value.
		if (dataSize < (sizeof(uint32_t) + sizeof(DDSHeader) + sizeof(DDSHeader10)))
			return false;

	}

	return true;
}

Parser::Parser(const void *data, size_t dataSize, Options opts)
	: format(FORMAT_UNKNOWN)
	, options(opts)
{
	parseData(data, dataSize);
}

Parser::Parser(const Parser &other)
	: format(other.format)
	, options(other.options)
{
	std::vector<Image>::const_iterator it;
	for (it = other.texData.begin(); it != other.texData.end(); ++it)
	{
		Image img = *it;

		if ((options & OPTIONS_COPY_DATA) && img.dataSize > 0)
		{
			uint8_t *data = 0;

			try
			{
				data = new uint8_t[img.dataSize];
			}
			catch (std::exception &)
			{
				clearData();
				throw;
			}

			img.data = data;
		}

		texData.push_back(img);
	}
}

Parser::~Parser()
{
	// Delete any data we created.
	clearData();
}

Format Parser::getFormat() const
{
	return format;
}

const Parser::Image *Parser::getImageData(size_t miplevel) const
{
	if (miplevel >= texData.size())
		return 0;

	return &texData[miplevel];
}

size_t Parser::getNumMipmaps() const
{
	return texData.size();
}

bool Parser::ownsData() const
{
	return (options & OPTIONS_COPY_DATA) != 0;
}

Format Parser::parseDDSFormat(const DDSPixelFormat &fmt) const
{
	if (fmt.flags & DDPF_FOURCC)
	{
		if (fmt.fourCC == FourCC('D','X','T','1'))
			return FORMAT_DXT1;
		else if (fmt.fourCC == FourCC('D','X','T','3'))
			return FORMAT_DXT3;
		else if (fmt.fourCC == FourCC('D','X','T','5'))
			return FORMAT_DXT5;
		else if (fmt.fourCC == FourCC('A','T','I','2'))
			return FORMAT_BC5u;
	}

	return FORMAT_UNKNOWN;
}

Format Parser::parseDX10Format(DXGIFormat fmt) const
{
	Format f = FORMAT_UNKNOWN;

	switch (fmt)
	{
	case DXGI_FORMAT_BC1_TYPELESS:
	case DXGI_FORMAT_BC1_UNORM:
	case DXGI_FORMAT_BC1_UNORM_SRGB:
		f = FORMAT_DXT1;
		break;
	case DXGI_FORMAT_BC2_TYPELESS:
	case DXGI_FORMAT_BC2_UNORM:
	case DXGI_FORMAT_BC2_UNORM_SRGB:
		f = FORMAT_DXT3;
		break;
	case DXGI_FORMAT_BC3_TYPELESS:
	case DXGI_FORMAT_BC3_UNORM:
	case DXGI_FORMAT_BC3_UNORM_SRGB:
		f = FORMAT_DXT5;
		break;
	case DXGI_FORMAT_BC5_SNORM:
		f = FORMAT_BC5s;
		break;
	case DXGI_FORMAT_BC5_TYPELESS:
	case DXGI_FORMAT_BC5_UNORM:
		f = FORMAT_BC5u;
		break;
	case DXGI_FORMAT_BC7_TYPELESS:
	case DXGI_FORMAT_BC7_UNORM:
		f = FORMAT_BC7;
		break;
	case DXGI_FORMAT_BC7_UNORM_SRGB:
		f = FORMAT_BC7srgb;
	default:
		break;
	}

	return f;
}

size_t Parser::parseImageSize(Format fmt, int width, int height) const
{
	size_t size = 0;

	switch (fmt)
	{
	case FORMAT_DXT1:
	case FORMAT_DXT3:
	case FORMAT_DXT5:
	case FORMAT_BC5s:
	case FORMAT_BC5u:
	case FORMAT_BC7:
	case FORMAT_BC7srgb:
		{
			int numBlocksWide = 0;
			if (width > 0)
				numBlocksWide = std::max(1, (width + 3) / 4);

			int numBlocksHigh = 0;
			if (height > 0)
				numBlocksHigh = std::max(1, (height + 3) / 4);

			int numBytesPerBlock = (fmt == FORMAT_DXT1 ? 8 : 16);

			size = numBlocksWide * numBytesPerBlock * numBlocksHigh;
		}
		break;
	default:
		break;
	}

	return size;
}

bool Parser::parseTexData(const uint8_t *data, size_t dataSize, Format fmt, int w, int h, int mips)
{
	size_t offset = 0;

	for (int i = 0; i < mips; i++)
	{
		Image img;
		img.width = w;
		img.height = h;

		img.dataSize = parseImageSize(fmt, img.width, img.height);

		// Make sure the data size is valid.
		if (img.dataSize == 0 || (offset + img.dataSize) > dataSize)
		{
			// Clean up any data we allocated previously.
			clearData();
			return false;
		}

		// Create our own copy of the data, if requested.
		if (options & OPTIONS_COPY_DATA)
		{
			uint8_t *newData = 0;

			try
			{
				newData = new uint8_t[img.dataSize];
			}
			catch (std::exception &)
			{
				// Clean up before throwing.
				clearData();
				throw;
			}

			memcpy(newData, &data[offset], img.dataSize);
			img.data = newData;
		}
		else
			img.data = &data[offset];

		texData.push_back(img);

		// Move to the next mip level.
		offset += img.dataSize;

		w = std::max(w / 2, 1);
		h = std::max(h / 2, 1);
	}

	return true;
}

bool Parser::parseData(const void *data, size_t dataSize)
{
	if (!isDDS(data, dataSize))
		return false;

	const uint8_t *readData = (const uint8_t *) data;
	ptrdiff_t offset = sizeof(uint32_t);

	DDSHeader *header = (DDSHeader *) &readData[offset];
	offset += sizeof(DDSHeader);


	// Check for DX10 extension.
	if ((header->format.flags & DDPF_FOURCC) && (header->format.fourCC == FourCC('D','X','1','0')))
	{
		DDSHeader10 *header10 = (DDSHeader10 *) &readData[offset];
		offset += sizeof(DDSHeader10);

		// We can't deal with 1D/3D textures.
		switch (header10->resourceDimension)
		{
		case D3D10_RESOURCE_DIMENSION_TEXTURE2D:
		case D3D10_RESOURCE_DIMENSION_UNKNOWN:
			break;
		default:
			return false;
		}

		// We also can't deal with texture arrays and cubemaps.
		if (header10->arraySize > 1)
			return false;

		format = parseDX10Format(header10->dxgiFormat);
	}
	else
		format = parseDDSFormat(header->format);


	if (format == FORMAT_UNKNOWN)
		return false;


	int w = header->width;
	int h = header->height;

	int mips = std::max((int) header->mipMapCount, 1);

	return parseTexData(&readData[offset], dataSize - offset, format, w, h, mips);
}

void Parser::clearData()
{
	if (options & OPTIONS_COPY_DATA)
	{
		// Delete any data we created.
		std::vector<Image>::iterator it;
		for (it = texData.begin(); it != texData.end(); ++it)
		{
			delete[] it->data;
			it->data = 0;
		}
	}

	texData.clear();
}

} // dds
