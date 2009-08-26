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

#ifndef LOVE_AUDIO_OPENAL_SOUND_H
#define LOVE_AUDIO_OPENAL_SOUND_H

// LOVE
#include <sound/SoundData.h>
#include <audio/Sound.h>
#include <common/config.h>
#include "Pool.h"



// OpenAL
#ifdef LOVE_MACOSX
#include <OpenAL/alc.h>
#include <OpenAL/al.h>
#else
#include <AL/alc.h>
#include <AL/al.h>
#endif

namespace love
{
namespace audio
{
namespace openal
{
	// Forward declarations.
	class Audio;

	class Sound : public love::audio::Sound
	{
	private:
	
		Pool * pool;

		// Sounds only need one buffer.
		ALuint buffer;

		ALuint source;

	public:
		Sound(Pool * pool, love::sound::SoundData * data);
		virtual ~Sound();

		// Implements Audible.
		void play(love::audio::Source * s);
		void update(love::audio::Source * s);
		void stop(love::audio::Source * s);
		void rewind(love::audio::Source * s);

	}; // Sound

} // openal
} // audio
} // love

#endif // LOVE_AUDIO_OPENAL_SOUND_H