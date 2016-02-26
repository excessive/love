/**
 * Copyright (c) 2006-2016 LOVE Development Team
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

// LOVE
#include "Timer.h"
#include "common/delay.h"

// SDL
#include <SDL.h>

namespace love
{
namespace timer
{
namespace sdl
{

Timer::Timer()
{
	// Init the SDL timer system (needed for SDL_Delay.)
	if (SDL_InitSubSystem(SDL_INIT_TIMER) < 0)
		throw love::Exception("Could not initialize SDL timer subsystem (%s)", SDL_GetError());
}

Timer::~Timer()
{
	// Quit SDL timer.
	SDL_QuitSubSystem(SDL_INIT_TIMER);
}

const char *Timer::getName() const
{
	return "love.timer.sdl";
}

void Timer::sleep(double seconds) const
{
	if (seconds > 0)
		love::sleep((unsigned int)(seconds*1000));
}

} // sdl
} // timer
} // love
