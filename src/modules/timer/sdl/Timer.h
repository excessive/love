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

#ifndef LOVE_TIMER_SDL_TIMER_H
#define LOVE_TIMER_SDL_TIMER_H

// SDL
#include <SDL.h>

// LOVE
#include <timer/Timer.h>

namespace love
{
namespace timer
{
namespace sdl
{

/**
 * An SDL timer module. Can keep track of time between certain function
 * calls, and provides access to a FPS metric which updates once each second.
 **/
class Timer : public love::timer::Timer
{
public:

	/**
	 * Constructor. Initializes the SDL/timer subsystem.
	 **/
	Timer();

	/**
	 * Destructor.
	 **/
	virtual ~Timer();

	const char *getName() const;
	void step();
	void sleep(double seconds);
	double getDelta() const;
	int getFPS() const;
	double getTime() const;
	double getMicroTime() const;

private:

	// Timing vars for benchmarking.
	Uint32 time_init;

	// Frame delta vars.
	Uint32 currTime;
	Uint32 prevTime;
	Uint32 prevFpsUpdate;

	// Updated with a certain frequency.
	int fps;

	// The frequency by which to update the FPS.
	double fpsUpdateFrequency;

	// Frames since last FPS update.
	int frames;

	// The current timestep.
	double dt;

}; // Timer

} // sdl
} // timer
} // love

#endif // LOVE_TIMER_SDL_TIMER_H
