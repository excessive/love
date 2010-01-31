/**
* Copyright (c) 2006-2009 LOVE Development Team
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

#include "Source.h"

#include "Pool.h"

// STD
#include <iostream>

namespace love
{
namespace audio
{
namespace openal
{

	static int acc = 0;

	Source::Source(Pool * pool, love::sound::SoundData * soundData)
		: love::audio::Source(Source::TYPE_STATIC), pool(pool), pitch(1.0f), 
		volume(1.0f), looping(false), decoder(0), valid(false)
	{
		alGenBuffers(1, buffers);
		ALenum fmt = getFormat(soundData->getChannels(), soundData->getBits());
		alBufferData(buffers[0], fmt, soundData->getData(), soundData->getSize(), soundData->getSampleRate());
	}

	Source::Source(Pool * pool, love::sound::Decoder * decoder)
		: love::audio::Source(Source::TYPE_STREAM), pool(pool), pitch(1.0f), 
		volume(1.0f), looping(false), decoder(decoder), valid(false)
	{
		decoder->retain();
		alGenBuffers(MAX_BUFFERS, buffers);
	}

	Source::~Source()
	{
		stop();
		alDeleteBuffers((type == TYPE_STATIC) ? 1 : MAX_BUFFERS, buffers);
	}

	love::audio::Source * Source::copy()
	{
		return 0;
	}

	void Source::play()
	{
		valid = pool->play(this, source);
	}

	void Source::stop()
	{
		pool->stop(this);
	}

	void Source::pause()
	{
		pool->pause();
	}

	void Source::resume()
	{
		pool->resume();
	}

	void Source::rewind()
	{
		pool->rewind();
	}

	bool Source::isStopped() const
	{
		if(valid)
		{
			ALenum state;
			alGetSourcei(source, AL_SOURCE_STATE, &state);
			return (state == AL_STOPPED);
		}

		return true;
	}

	bool Source::isFinished() const
	{
		return type == TYPE_STATIC ? isStopped() : isStopped() && !isLooping() && decoder->isFinished();
	}

	void Source::update()
	{
		if(valid && type == TYPE_STATIC)
		{
			// Looping mode could have changed.
			alSourcei(source, AL_LOOPING, isLooping() ? AL_TRUE : AL_FALSE);
		}
		else if(valid && type == TYPE_STREAM)
		{
			// Number of processed buffers.
			ALint processed;

			alGetSourcei(source, AL_BUFFERS_PROCESSED, &processed);

			while(processed--)
			{
				ALuint buffer;

				// Get a free buffer.
				alSourceUnqueueBuffers(source, 1, &buffer);

				if(streamAtomic(buffer, decoder) > 0)
					alSourceQueueBuffers(source, 1, &buffer);
			}
		}
	}

	void Source::setPitch(float pitch)
	{
		if(valid)
			alSourcef(source, AL_PITCH, pitch);

		this->pitch = pitch;
	}

	float Source::getPitch() const
	{
		if(valid)
		{
			ALfloat f;
			alGetSourcef(source, AL_PITCH, &f);
			return f;
		}

		// In case the Source isn't playing.
		return pitch;
	}

	void Source::setVolume(float volume)
	{
		if(valid)
		{
			alSourcef(source, AL_GAIN, volume);
		}

		this->volume = volume;
	}

	float Source::getVolume() const
	{
		if(valid)
		{
			ALfloat f;
			alGetSourcef(source, AL_GAIN, &f);
			return f;
		}

		// In case the Source isn't playing.
		return volume;
	}

	void Source::setPosition(float * v)
	{
		if(valid)
			alSourcefv(source, AL_POSITION, v);
	}

	void Source::getPosition(float * v) const
	{
		if(valid)
			alGetSourcefv(source, AL_POSITION, v);
	}

	void Source::setVelocity(float * v)
	{
		if(valid)
			alSourcefv(source, AL_VELOCITY, v);
	}

	void Source::getVelocity(float * v) const
	{
		if(valid)
			alGetSourcefv(source, AL_VELOCITY, v);
	}

	void Source::setDirection(float * v)
	{
		if(valid)
			alSourcefv(source, AL_DIRECTION, v);
	}

	void Source::getDirection(float * v) const
	{
		if(valid)
			alGetSourcefv(source, AL_DIRECTION, v);
	}

	void Source::setLooping(bool looping)
	{
		this->looping = looping;
	}

	bool Source::isLooping() const
	{
		return looping;
	}

	void Source::playAtomic()
	{
		if(type == TYPE_STATIC)
		{
			alSourcei(source, AL_BUFFER, buffers[0]);
			alSourcei(source, AL_LOOPING, isLooping() ? AL_TRUE : AL_FALSE);
		}
		else if(type == TYPE_STREAM)
		{
			int usedBuffers = 0;

			for(int i = 0; i < MAX_BUFFERS; i++)
			{
				int decoded = streamAtomic(buffers[i], decoder);
				++usedBuffers;
				if(decoded < decoder->getSize())
					break;
			}

			if(usedBuffers > 0)
				alSourceQueueBuffers(source, usedBuffers, buffers);
		}

		// Set these properties. These may have changed while we've
		// been without an AL source.
		alSourcef(source, AL_PITCH, pitch);
		alSourcef(source, AL_GAIN, volume);

		alSourcePlay(source);
	}

	void Source::stopAtomic()
	{
		if(valid)
		{
			if(type == TYPE_STATIC)
			{
				alSourceStop(source);
			}
			else if(type == TYPE_STREAM)
			{
				alSourceStop(source);

				int queued = 0;
				alGetSourcei(source, AL_BUFFERS_QUEUED, &queued);

				while(queued--)
				{
					ALuint buffer;
					alSourceUnqueueBuffers(source, 1, &buffer);
				}
			}

			reset(source);
		}
		rewindAtomic();
		valid = false;
	}

	void Source::pauseAtomic()
	{
		if(valid)
		{
			alSourcePause(source);
		}
	}

	void Source::resumeAtomic()
	{
		if(valid)
		{
			alSourcePlay(source);
		}
	}

	void Source::rewindAtomic()
	{
		if(valid && type == TYPE_STATIC)
		{
			alSourceRewind(source);
		}
		else if(valid && type == TYPE_STREAM)
		{
			decoder->rewind();
		}
	}

	void Source::reset(ALenum source)
	{
		alSourcei(source, AL_BUFFER, AL_NONE);
	}

	ALenum Source::getFormat(int channels, int bits) const
	{
		if(channels == 1 && bits == 8)
			return AL_FORMAT_MONO8;
		else if(channels == 1 && bits == 16)
			return AL_FORMAT_MONO16;
		else if(channels == 2 && bits == 8)
			return AL_FORMAT_STEREO8;
		else if(channels == 2 && bits == 16)
			return AL_FORMAT_STEREO16;
		else
			return 0;
	}

	int Source::streamAtomic(ALuint buffer, love::sound::Decoder * d)
	{
		//if(isLooping() && d->isFinished())
		//	d->rewind();

		// Get more sound data.
		int decoded = d->decode();

		int fmt = getFormat(d->getChannels(), d->getBits());

		//printf("Decoded %i ( %i )", decoded, d->getSize());

		if(decoded > 0 && fmt != 0)
		{
			alBufferData(buffer, fmt, d->getBuffer(), decoded, d->getSampleRate());
			acc++;
			//printf(" [%i] ", acc);
		}

		if(decoded < d->getSize() && isLooping())
		{
			printf(" Rewinding at %i.\n", acc);
			d->rewind();
			acc = 0;
		}

		return decoded;
	}

} // openal
} // audio
} // love
