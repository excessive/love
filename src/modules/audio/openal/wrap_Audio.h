/**
* Copyright (c) 2006-2011 LOVE Development Team
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

#ifndef LOVE_AUDIO_OPENAL_WRAP_AUDIO_H
#define LOVE_AUDIO_OPENAL_WRAP_AUDIO_H

// LOVE
#include "audio/wrap_Audio.h"

namespace love
{
namespace audio
{
namespace openal
{
	extern "C" LOVE_EXPORT int luaopen_love_audio(lua_State * L);

} // openal
} // audio
} // love

#endif // LOVE_AUDIO_OPENAL_WRAP_AUDIO_H
