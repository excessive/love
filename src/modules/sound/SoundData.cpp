/**
* Copyright (c) 2006-2010 LOVE Development Team
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

#include "SoundData.h"

// C
#include <climits>
#include <cstdlib>
#include <cstring>

// STL
#include <iostream>
#include <vector>

namespace love
{
namespace sound
{
	SoundData::SoundData(Decoder * decoder)
		: data(0), size(0), sampleRate(Decoder::DEFAULT_SAMPLE_RATE), bits(0), channels(0)
	{
		int decoded = decoder->decode();

		while(decoded > 0)
		{
			// Expand or allocate buffer. Note that realloc may move
			// memory to other locations.
			data = (char*)realloc(data, size + decoder->getSize());

			// Copy memory into new part of memory.
			memcpy(data + size, decoder->getBuffer(), decoded);

			// Keep this up to date.
			size += decoded;

			decoded = decoder->decode();
		}

		channels = decoder->getChannels();
		bits = decoder->getBits();
		sampleRate = decoder->getSampleRate();
	}

	SoundData::SoundData(int samples, int sampleRate, int bits, int channels)
		: data(0), size(samples*(bits/8)), sampleRate(sampleRate), bits(bits), channels(channels)
	{
		data = (char*)malloc(size);
	}

	SoundData::~SoundData()
	{
		if(data != 0)
			free(data);
	}

	void * SoundData::getData() const
	{
		return (void*)data;
	}

	int SoundData::getSize() const
	{
		return (int)size;
	}

	int SoundData::getChannels() const
	{
		return channels;
	}

	int SoundData::getBits() const
	{
		return bits;
	}

	int SoundData::getSampleRate() const
	{
		return sampleRate;
	}

	void SoundData::setSample(int i, float sample)
	{
		// Check range.
		if(i < 0 || i >= size/(bits/8))
			return;

		if(bits == 16)
		{
			short * s = (short *)data;
			s[i] = (short)(sample*(float)SHRT_MAX);
			return;
		}
		else
		{
			data[i] = (char)(sample*(float)CHAR_MAX);
			return;
		}
	}

	float SoundData::getSample(int i) const
	{
		// Check range.
		if(i < 0 || i >= size/(bits/8))
			return 0;

		if(bits == 16)
		{
			short * s = (short *)data;
			return (float)s[i]/(float)SHRT_MAX;
		}
		else
		{
			return (float)data[i]/(float)CHAR_MAX;
		}
	}

} // sound
} // love
